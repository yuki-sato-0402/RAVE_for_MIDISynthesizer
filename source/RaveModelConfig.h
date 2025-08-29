#ifndef ANIRA_RAVE_model_CONFIG_H
#define ANIRA_RAVE_model_CONFIG_H

#include <anira/anira.h>

//inline anira::InferenceConfig makeRaveModelConfig(const std::string& fileName)
//{
//    std::vector<anira::ModelData> modelData = {
//    #ifdef USE_LIBTORCH
//        {RAVE_MODEL_DIR + fileName, anira::InferenceBackend::LIBTORCH}
//    #endif
//    };
//    std::vector<anira::TensorShape> tensorShape = {
//        {{{1, 1, 2048}}, {{1, 1, 2048}}, anira::InferenceBackend::LIBTORCH},
//    };
//    anira::ProcessingSpec processingSpec{
//        {1}, {1}, {2048}, {2048}, {2048}
//    };
    
//    return anira::InferenceConfig(
//        modelData,
//        tensorShape,
//        processingSpec,
//        62.66f,
//        5,
//        true // session_exclusive_processor because of cached convolution layers in the model
//    );          
//}

static std::vector<anira::ModelData> modelData = {
#ifdef USE_LIBTORCH
    {RAVE_MODEL_DIR + std::string("/bpiyush_sound-of-waterV1_e02d7ebfe3_streaming.ts"), anira::InferenceBackend::LIBTORCH},
#endif
};

static std::vector<anira::TensorShape> tensorShape = {
    {{{1, 1, 2048}}, {{1, 1, 2048}}, anira::InferenceBackend::LIBTORCH},
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
