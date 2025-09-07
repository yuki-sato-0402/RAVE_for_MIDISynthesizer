#ifndef ANIRA_RAVE_MODEL_CONFIG_DECODER_H
#define ANIRA_RAVE_MODEL_CONFIG_DECODER_H

#include <anira/anira.h>

static std::vector<anira::ModelData> model_data_rave_model_decoder_config = {
#ifdef USE_LIBTORCH
    {RAVE_MODEL_DIR + std::string("waterV1_e02d7ebfe3_streaming_norm8.ts"), anira::InferenceBackend::LIBTORCH, std::string("decode")},
#endif
};

//You need to check the dimension of the latent variables. (Refer to confirmLatentSpace.py)
static std::vector<anira::TensorShape> tensor_shape_rave_model_decoder_config = {
    {{{1, 8, 1}}, {{1, 1, 2048}}, anira::InferenceBackend::LIBTORCH}
};

static anira::ProcessingSpec processing_spec_rave_model_decoder_config{
    {8}, // preprocess_input_channels
    {1},  // postprocess_output_channels
    {1}, // preprocess_input_size
    {2048}, // postprocess_output_size
    {2048} // internal_model_latency
};

static anira::InferenceConfig rave_model_decoder_config(
    model_data_rave_model_decoder_config,
    tensor_shape_rave_model_decoder_config,
    processing_spec_rave_model_decoder_config,
    160.00f,
    5,
    false // session_exclusive_processor because of cached convolution layers in the model
);

#endif //ANIRA_RAVE_model_CONFIG_DECODER_H
