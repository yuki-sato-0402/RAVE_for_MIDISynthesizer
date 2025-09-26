#ifndef ANIRA_RAVE_model_CONFIG_H
#define ANIRA_RAVE_model_CONFIG_H

#include <anira/anira.h>

static std::vector<anira::ModelData> modelData = {
    {RAVE_MODEL_DIR + std::string("altoSaxV1_e02d7ebfe3_streaming_norm.ts"), anira::InferenceBackend::CUSTOM},
};

static std::vector<anira::TensorShape> tensorShape = {
    {{{1, 1, 2048}}, {{1, 1, 2048}}, anira::InferenceBackend::CUSTOM},
};

static anira::ProcessingSpec processingSpec{
    {1}, // preprocess_input_channels
    {1},  // postprocess_output_channels
    {2048}, // preprocess_input_size
    {2048}, // postprocess_output_size
    {2048}  // internal_model_latency
};

static anira::InferenceConfig rave_model_config(
    modelData,
    tensorShape,
    processingSpec,
    200.00f,
    5,//number of warm up inferences (There is a risk of a loud noise playing the moment the app launches.)
    false // session_exclusive_processor because of cached convolution layers in the model
);

#endif //ANIRA_RAVE_model_CONFIG_H
