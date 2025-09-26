#include "CustomBackendProcessor.h"
#include <anira/utils/Logger.h>

namespace anira {


std::vector<std::atomic<float>> CustomBackend::latent_meter(8);
std::vector<std::atomic<float>> CustomBackend::scale(8);
std::vector<std::atomic<float>> CustomBackend::bias(8);

CustomBackend::CustomBackend(InferenceConfig& inference_config) : BackendBase(inference_config) ,instanceForSetLatent(inference_config){
    torch::set_num_threads(1);

    for (unsigned int i = 0; i < m_inference_config.m_num_parallel_processors; ++i) {
        m_instances.emplace_back(std::make_shared<Instance>(m_inference_config));
    }

}

CustomBackend::~CustomBackend() {
}

void CustomBackend::prepare() {
    for(auto& instance : m_instances) {
        instance->prepare();
    }
}

void CustomBackend::process(std::vector<BufferF>& input, std::vector<BufferF>& output, std::shared_ptr<SessionElement> session) { 
    while (true) {
        for(auto& instance : m_instances) {
            if (!(instance->m_processing.exchange(true))) {
                instance->process(input, output, session);
                instance->m_processing.exchange(false);
                return;
            }
        }
    }
}


//warm up inference
CustomBackend::Instance::Instance(InferenceConfig& inference_config) : m_inference_config(inference_config) {
    try {
        m_module = torch::jit::load(m_inference_config.get_model_path(anira::InferenceBackend::CUSTOM));
    }
    catch (const c10::Error& e) {
        LOG_ERROR << "[ERROR] error loading the model\n";
        LOG_ERROR << e.what() << std::endl;
    }
    m_inputs.resize(m_inference_config.get_tensor_input_shape().size());
    m_input_data.resize(m_inference_config.get_tensor_input_shape().size());
    for (size_t i = 0; i < m_inference_config.get_tensor_input_shape().size(); i++) {
        m_input_data[i].resize(m_inference_config.get_tensor_input_size()[i]);
        m_inputs[i] = torch::from_blob(m_input_data[i].data(), m_inference_config.get_tensor_input_shape(anira::InferenceBackend::CUSTOM)[i]);
    }

    for (size_t i = 0; i < m_inference_config.m_warm_up; i++) {
        if (!m_inference_config.get_model_function(InferenceBackend::CUSTOM).empty()) {
            auto method = m_module.get_method(m_inference_config.get_model_function(InferenceBackend::CUSTOM));
            m_outputs = method(m_inputs);
        } else {
            // Run inference
            m_outputs = m_module.forward(m_inputs); 
        }
    }

    
    for (size_t i = 0; i < static_cast<size_t>(latent_size); i++) {
        CustomBackend::latent_meter[i].store(0.0f, std::memory_order_relaxed);
        CustomBackend::scale[i].store(1.0f, std::memory_order_relaxed);
        CustomBackend::bias[i].store(0.0f, std::memory_order_relaxed);
    }

}

void CustomBackend::Instance::prepare() {
    for (size_t i = 0; i < m_inference_config.get_tensor_input_shape().size(); i++) {
        m_input_data[i].clear();
    }
}

void CustomBackend::Instance::process(std::vector<BufferF>& input, std::vector<BufferF>& output, std::shared_ptr<SessionElement> session) {
    (void)session;
    for (size_t i = 0; i < m_inference_config.get_tensor_input_shape().size(); i++) {
        m_input_data[i].swap_data(input[i].get_memory_block());
        input[i].reset_channel_ptr();
        // This is necessary because the tensor data pointers seem to change from inference to inference
        m_inputs[i] = torch::from_blob(m_input_data[i].data(), m_inference_config.get_tensor_input_shape(anira::InferenceBackend::CUSTOM)[i]);
    }

    // Run inference
    if (!m_inference_config.get_model_function(InferenceBackend::CUSTOM).empty()) {
        std::cout << "!m_inference_config.get_model_function(InferenceBackend::CUSTOM).empty() "<< std::endl;
        auto method = m_module.get_method(m_inference_config.get_model_function(InferenceBackend::CUSTOM));
        m_outputs = method(m_inputs);
    } else {
        //m_outputs = m_module.forward(m_inputs);
        auto latents_ivalue = m_module.get_method("encode")(m_inputs);
        m_latents = latents_ivalue.toTensor();
        float* latents_ptr = m_latents.data_ptr<float>();
        for (size_t i = 0; i < latent_size; i++) {
            float scale_factor = scale[i].load(std::memory_order_relaxed);
            float bias_factor = bias[i].load(std::memory_order_relaxed);
            //std::cout << "bias_factor "<<  bias_factor << " " << i << std::endl;
            latents_ptr[i] = (latents_ptr[i] + bias_factor) * scale_factor ;  
            if (std::isnan(latents_ptr[i]) || std::isinf(latents_ptr[i]) ||  abs(latents_ptr[i]) > 10) {
                latents_ptr[i] = 0.0f;
            }
            latent_meter[i].store(latents_ptr[i], std::memory_order_relaxed);
        }  
     
        m_outputs = m_module.get_method("decode")({m_latents});
    }

    // We need to copy the data because we cannot access the data pointer ref of the tensor directly
    if(m_outputs.isTuple()) {
        for (size_t i = 0; i < m_inference_config.get_tensor_output_shape().size(); i++) {
            for (size_t j = 0; j < m_inference_config.get_tensor_output_size()[i]; j++) {
                output[i].get_memory_block()[j] = m_outputs.toTuple()->elements()[i].toTensor().view({-1}).data_ptr<float>()[j];
            }
        }
    } else if(m_outputs.isTensorList()) {
        for (size_t i = 0; i < m_inference_config.get_tensor_output_shape().size(); i++) {
            for (size_t j = 0; j < m_inference_config.get_tensor_output_size()[i]; j++) {
                output[i].get_memory_block()[j] = m_outputs.toTensorList().get(i).view({-1}).data_ptr<float>()[j];
            }
        }
    } else if (m_outputs.isTensor()) {
        for (size_t j = 0; j < m_inference_config.get_tensor_output_size()[0]; j++) {
            output[0].get_memory_block()[j] = m_outputs.toTensor().view({-1}).data_ptr<float>()[j];
        }
    }
}


void CustomBackend::Instance::setLatentScale(size_t index, float value)
{
    if (index >= 0 && index < latent_size)
    {
        scale[index].store(value, std::memory_order_relaxed);
    }
}


void CustomBackend::Instance::setLatentBias(size_t index, float value)
{
    if (index >= 0 && index < latent_size)
    {
        bias[index].store(value, std::memory_order_relaxed);
    }
}

float CustomBackend::Instance::getEncoderOutput(int index){
    return latent_meter[static_cast<size_t>(index)].load(std::memory_order_relaxed);
} // namespace anira



}