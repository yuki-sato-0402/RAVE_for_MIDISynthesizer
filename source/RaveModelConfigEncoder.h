#ifndef ANIRA_RAVE_MODEL_CONFIG_ENCODER_H
#define ANIRA_RAVE_MODEL_CONFIG_ENCODER_H

#include <anira/anira.h>

static std::vector<anira::ModelData> model_data_rave_model_encoder_config = {
#ifdef USE_LIBTORCH
    {RAVE_MODEL_DIR + std::string("altoSaxV1_e02d7ebfe3_streaming_norm.ts"), anira::InferenceBackend::LIBTORCH, std::string("encode")},
#endif
};

//You need to check the dimension of the latent variables. (Refer to confirmLatentSpace.py)
static std::vector<anira::TensorShape> tensor_shape_rave_model_encoder_config = {
    {{{1, 1, 2048}}, {{1, 8, 1}}, anira::InferenceBackend::LIBTORCH}
};

static anira::ProcessingSpec processing_spec_rave_model_encoder_config{
    {1}, // preprocess_input_channels
    {8}  // postprocess_output_channels
};

static anira::InferenceConfig rave_model_encoder_config(
    model_data_rave_model_encoder_config,
    tensor_shape_rave_model_encoder_config,
    processing_spec_rave_model_encoder_config,
    100.00f,
    5,
    true // session_exclusive_processor because of cached convolution layers in the model
);

#endif //ANIRA_RAVE_model_CONFIG_ENCODER_H
