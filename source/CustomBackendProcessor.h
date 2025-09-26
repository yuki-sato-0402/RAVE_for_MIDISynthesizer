#ifndef ANIRA_CUSTOMBACKEND_H
#define ANIRA_CUSTOMBACKEND_H

// Avoid min/max macro conflicts on Windows for CUSTOM compatibility
#ifdef _WIN32
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
#endif

#include "../modules/anira/include/anira/InferenceConfig.h"
#include "../modules/anira/include/anira/utils/Buffer.h"
#include "../modules/anira/include/anira/backends/BackendBase.h"
#include "../modules/anira/include/anira/scheduler/SessionElement.h"
#include <stdlib.h>
#include <memory>

// CUSTOM headers trigger many warnings; disabling for cleaner build logs
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244 4267 4996)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wall"
#endif

#include <torch/script.h>
#include <torch/torch.h>

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

namespace anira {

/**
 * @brief CUSTOM-based neural network inference processor
 * 
 * The CustomBackend class provides neural network inference capabilities using
 * Facebook's PyTorch C++ API (CUSTOM). It supports loading TorchScript models
 * and performing real-time inference with parallel processing capabilities.
 * 
 * @warning This class is only available when compiled with USE_LIBTORCH defined
 * @see BackendBase, CustomBackend::Instance, InferenceConfig, ModelData, SessionElement
 */
class ANIRA_API CustomBackend : public BackendBase {
public:
    /**
     * @brief Constructs a CUSTOM processor with the given inference configuration
     * 
     * Initializes the CUSTOM processor and creates the necessary number of parallel
     * processing instances based on the configuration's num_parallel_processors setting.
     * 
     * @param inference_config Reference to inference configuration containing model path,
     *                        tensor shapes, and processing parameters
     * 
     * @par Model Loading:
     * The constructor attempts to load the TorchScript model specified in the configuration.
     * If a model function is specified, it will be used; otherwise, the default forward
     * method is called.
     */
    CustomBackend(InferenceConfig& inference_config);

    /**
     * @brief Destructor that properly cleans up CUSTOM resources
     * 
     * Ensures proper cleanup of all CUSTOM modules, tensors, and allocated memory.
     * All processing instances are safely destroyed.
     */
    ~CustomBackend() override;

    /**
     * @brief Prepares all CUSTOM instances for inference operations
     * 
     * Loads the TorchScript model into all parallel processing instances, allocates
     * input/output tensors, and performs warm-up inferences if specified in the configuration.
     */
    void prepare() override;

    /**
     * @brief Processes input buffers through the CUSTOM model
     * 
     * Performs neural network inference using CUSTOM, converting audio buffers to
     * PyTorch tensors, executing the model, and converting results back to audio buffers.
     * 
     * @param input Vector of input buffers containing audio samples or parameter data
     * @param output Vector of output buffers to receive processed results
     * @param session Shared pointer to session element providing thread-safe instance access
     */
    void process(std::vector<BufferF>& input, std::vector<BufferF>& output, std::shared_ptr<SessionElement> session) override;

    void setLatentScalePublic(size_t index, float value) {
        instanceForSetLatent.setLatentScale(index, value);
    }

    void setLatentBiasPublic(size_t index, float value) {
        instanceForSetLatent.setLatentBias(index, value);
    }

    float getEncoderOutputPublic(int index){
        return instanceForSetLatent.getEncoderOutput(index);
    }

    static std::vector<std::atomic<float>> latent_meter;
    static std::vector<std::atomic<float>> scale;
    static std::vector<std::atomic<float>> bias;
    

private:
    /**
     * @brief Internal processing instance for thread-safe CUSTOM operations
     * 
     * Each Instance represents an independent CUSTOM processing context with its own
     * model, tensors, and memory allocation. This design enables parallel processing
     * without shared state or synchronization overhead.
     * 
     * @par Thread Safety:
     * Each instance is used by only one thread at a time, eliminating the need for
     * locks during inference operations. The atomic processing flag ensures safe
     * instance allocation across threads.
     * 
     * @see CustomBackend
     */
    struct Instance {
        /**
         * @brief Constructs a CUSTOM processing instance
         * 
         * @param inference_config Reference to inference configuration
         */
        Instance(InferenceConfig& inference_config);

        /**
         * @brief Prepares this instance for inference operations
         * 
         * Loads the TorchScript model, allocates tensors, and performs initialization.
         */
        void prepare();

        /**
         * @brief Processes input through this instance's model
         * 
         * @param input Input buffers to process
         * @param output Output buffers to fill with results
         * @param session Session element for context (unused in instance)
         */
        void process(std::vector<BufferF>& input, std::vector<BufferF>& output, std::shared_ptr<SessionElement> session);
        
        void setLatentScale(size_t index, float value);
        void setLatentBias(size_t index, float value);
        float getEncoderOutput(int index);
        torch::jit::script::Module m_module;        ///< Loaded TorchScript model for inference

        std::vector<MemoryBlock<float>> m_input_data;  ///< Pre-allocated input data buffers

        std::vector<c10::IValue> m_inputs;          ///< PyTorch input tensor values
        torch::Tensor m_latents;
        c10::IValue m_outputs;  
        size_t latent_size = 8;  
       
        //std::vector<std::unique_ptr<std::atomic<float>>> scale;
        //std::vector<std::unique_ptr<std::atomic<float>>> bias;
        InferenceConfig& m_inference_config;        ///< Reference to inference configuration
        std::atomic<bool> m_processing {false};     ///< Flag indicating if instance is currently processing

#if DOXYGEN
        // Since Doxygen does not find classes structures nested in std::shared_ptr
        MemoryBlock<float>* __doxygen_force_0; ///< Placeholder for Doxygen documentation
#endif
    };

    Instance instanceForSetLatent;
    
    std::vector<std::shared_ptr<Instance>> m_instances;  ///< Vector of parallel processing instances

#if DOXYGEN
    Instance* __doxygen_force_0; ///< Placeholder for Doxygen documentation
#endif
};

} // namespace anira

#endif //ANIRA_CustomBackend_H