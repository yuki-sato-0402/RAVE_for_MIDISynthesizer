#pragma once

#include <JuceHeader.h>
#include <anira/anira.h>
#include "RaveModelConfig.h"
#include "CustomBackendProcessor.h"
//==============================================================================
class RAVE_for_MIDISynthesizer_Processor  : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener, 
public juce::ValueTree::Listener
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

    static constexpr size_t maxNumVoices = 4;

    struct Voice {
        int noteNumber = -1;        // Currently assigned MIDI note number (-1 indicates unassigned)
        bool active = false;        // Whether the voice is currently playing
        uint64_t noteOnCounter = 0; // Timestamp of when the note was turned on (used for voice stealing)

        juce::dsp::Oscillator<float> sinOsc { [](float x) { return std::sin (x); } };                // Sine wave oscillator
        juce::dsp::Oscillator<float> squareOsc { [](float x) { return x < 0.0f ? -1.0f : 1.0f; } }; // Square wave oscillator
        juce::ADSR adsr;                                                                              // Voice-independent ADSR envelope generator

        // Sample rate and ADSR parameter initialization
        void prepare(const juce::dsp::ProcessSpec& spec, double sampleRate, const juce::ADSR::Parameters& params)
        {
            sinOsc.prepare(spec);
            squareOsc.prepare(spec);
            adsr.setSampleRate(sampleRate);
            adsr.setParameters(params);
        }
    };

    std::array<Voice, maxNumVoices> voices; 
    uint64_t globalNoteCounter = 0;         // Globally count up the note on identifier counter

    juce::dsp::Gain<float> gain;
    juce::ADSR::Parameters adsrParams;

    float dryWetRangeParam = 0.0f;
    float oscMixParam = 0.0f;
    float gainParam = 0.5f;
    int modelIndex = 0;

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
