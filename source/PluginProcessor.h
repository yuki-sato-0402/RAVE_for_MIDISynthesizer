#pragma once

#include <JuceHeader.h>
#include <anira/anira.h>
#include "RaveModelConfig.h"

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

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;

    void processesNonRealtime(const juce::AudioBuffer<float>& buffer) const;

    juce::AudioProcessorValueTreeState parameters;
    juce::StringArray modelFileNames;

    // Optional ContextConfig
    anira::ContextConfig anira_context_config;
    //anira::InferenceConfig inference_config  = makeRaveModelConfig("water_pondbrain_b2048_r48000_z16.ts"); // Default model config
    anira::InferenceConfig inference_config  = rave_model_config; 
    anira::PrePostProcessor pp_processor;
    anira::InferenceHandler inference_handler;

    juce::dsp::DryWetMixer<float> dry_wet_mixer;

    //lamda type oscillator
    juce::dsp::Oscillator<float> osc { [](float x) { return std::sin (x); } };
    juce::dsp::Gain<float> gain;
    
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    float dryWetRangeParam;
    float gainParam;
    int modelIndex;

    double mutedSamples = 0;
    int64_t totalSamplesProcessed = 0;

    juce::MidiMessageCollector midiMessageCollector;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RAVE_for_MIDISynthesiser_Processor)
};
