#pragma once

#include <JuceHeader.h>
#include <anira/anira.h>
#include "RaveModelConfigDecoder.h"
#include "RaveModelConfigEncoder.h"

//==============================================================================
class RAVE_for_MIDISynthesiser_Processor  : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    RAVE_for_MIDISynthesiser_Processor();
    ~RAVE_for_MIDISynthesiser_Processor() override = default;

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

    juce::AudioProcessorValueTreeState& getValueTreeState() { return parameters; }

    juce::MidiMessageCollector& getMidiMessageCollector() noexcept { return midiMessageCollector; }

    //==============================================================================
    float getLatentVariables(const int index);

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;

    void processesNonRealtime(const juce::AudioBuffer<float>& buffer) const;

    juce::AudioProcessorValueTreeState parameters;
    juce::StringArray modelFileNames;

    // Optional ContextConfig
    anira::ContextConfig anira_context_config;

    anira::InferenceConfig inference_config_encoder = rave_model_encoder_config;
    anira::InferenceConfig inference_config_decoder = rave_model_decoder_config;
    anira::PrePostProcessor pp_processor_encoder;
    anira::PrePostProcessor pp_processor_decoder;
    
    anira::InferenceHandler inference_handler_encoder;
    anira::InferenceHandler inference_handler_decoder;
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

    float latentVariable1Param = 0.0f;
    float latentVariable2Param = 0.0f;
    float latentVariable3Param = 0.0f;
    float latentVariable4Param = 0.0f;
    float latentVariable5Param = 0.0f;
    float latentVariable6Param = 0.0f;
    float latentVariable7Param = 0.0f;
    float latentVariable8Param = 0.0f;

    float scaledValue1 = 0.0f;
    float scaledValue2 = 0.0f;
    float scaledValue3 = 0.0f;
    float scaledValue4 = 0.0f;
    float scaledValue5 = 0.0f;
    float scaledValue6 = 0.0f;
    float scaledValue7 = 0.0f;
    float scaledValue8 = 0.0f;

    double mutedSamples = 0;
    int64_t totalSamplesProcessed = 0;

    juce::MidiMessageCollector midiMessageCollector;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RAVE_for_MIDISynthesiser_Processor)
};
