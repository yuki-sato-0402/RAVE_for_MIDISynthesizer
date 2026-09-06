#pragma once

#include <JuceHeader.h>
#include <anira/anira.h>
#include "RaveModelConfig.h"
#include "CustomBackendProcessor.h"
//==============================================================================
class RAVE_for_MIDISynthesizer_Processor  : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener, 
public juce::ValueTree::Listener, public juce::ActionBroadcaster
{
public:
    //==============================================================================
    RAVE_for_MIDISynthesizer_Processor();
    ~RAVE_for_MIDISynthesizer_Processor() override = default;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //juce::AudioProcessorValueTreeState& getValueTreeState() { return apvts; }

    juce::MidiMessageCollector& getMidiMessageCollector() noexcept { return midiMessageCollector; }

    //==============================================================================
    float getLatentVariables(const int index);

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;

    void processesNonRealtime(const juce::AudioBuffer<float>& buffer) const;

    juce::AudioProcessorValueTreeState apvts;
    juce::ValueTree valueTree;

    // Optional ContextConfig
    anira::ContextConfig anira_context_config;

    std::unique_ptr<anira::InferenceConfig> inference_config = std::make_unique<anira::InferenceConfig>(rave_model_config);
    std::unique_ptr<anira::PrePostProcessor> pp_processor;
    std::unique_ptr<anira::CustomBackend> custom_backend;
    std::unique_ptr<anira::InferenceHandler> inference_handler;

    void updateModel(std::string newModelPath);

    int m_count_input_samples = 0;

    juce::dsp::DryWetMixer<float> dry_wet_mixer;

    //lamda type oscillator
    juce::dsp::Oscillator<float> osc { [](float x) { return std::sin (x); } };
    juce::dsp::Gain<float> gain;
    
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    float dryWetRangeParam;
    float gainParam;
    int modelIndex;

    //midi control
    int lastNoteNumber = -1;
    bool noteActive = false;

    float latentVariable1ScaleParam = 0.0f;
    float latentVariable2ScaleParam = 0.0f;
    float latentVariable3ScaleParam = 0.0f;
    float latentVariable4ScaleParam = 0.0f;
    float latentVariable5ScaleParam = 0.0f;
    float latentVariable6ScaleParam = 0.0f;
    float latentVariable7ScaleParam = 0.0f;
    float latentVariable8ScaleParam = 0.0f;

    float latentVariable1BiasParam = 0.0f;
    float latentVariable2BiasParam = 0.0f;
    float latentVariable3BiasParam = 0.0f;
    float latentVariable4BiasParam = 0.0f;
    float latentVariable5BiasParam = 0.0f;
    float latentVariable6BiasParam = 0.0f;
    float latentVariable7BiasParam = 0.0f;
    float latentVariable8BiasParam = 0.0f;


    double mutedSamples = 0;
    int64_t totalSamplesProcessed = 0;

    juce::MidiMessageCollector midiMessageCollector;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RAVE_for_MIDISynthesizer_Processor)
};
