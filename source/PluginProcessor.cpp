#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RAVE_for_MIDISynthesiser_Processor::RAVE_for_MIDISynthesiser_Processor() 
        : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                       ),
        parameters(*this, nullptr, juce::Identifier("PARAMETERS"),
        juce::AudioProcessorValueTreeState::ParameterLayout {
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "dryWetRange",  1}, "DryWetRange",
        juce::NormalisableRange<float>(0.f, 100.f, 0.01f), 0.f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "outputGain",  1}, "OutputGain",
        juce::NormalisableRange<float>(0.f, 1.f, 0.01f), 0.5f),
        //std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ "modelSelection", 1}, "Model Selection", 
        //modelFileNames, 0),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "attackTime",  1}, "AttackTime",
        juce::NormalisableRange<float>(1, 1000, 1), 100),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "decayTime",  1}, "DecayTime",
        juce::NormalisableRange<float>(1, 1000, 1), 300),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "sustain",  1}, "Sustain",
        juce::NormalisableRange<float>(0, 100, 1), 30),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "releaseTime",  1}, "ReleaseTime",
        juce::NormalisableRange<float>(1, 1000, 1), 200)      
        }
        ),
        //anira_context_config(
        //    std::thread::hardware_concurrency() / 2 > 0 ? std::thread::hardware_concurrency() / 2 : 1 // Total number of threads
        //),
        pp_processor(inference_config),
        inference_handler(pp_processor, inference_config),
        dry_wet_mixer(32768) // 32768 samples of max latency compensation for the dry-wet mixer
{
    parameters.addParameterListener("dryWetRange", this);
    parameters.addParameterListener("outputGain", this);
    //parameters.addParameterListener("modelSelection", this);
    parameters.addParameterListener("attackTime", this);
    parameters.addParameterListener("decayTime", this);
    parameters.addParameterListener("sustain", this);
    parameters.addParameterListener("releaseTime", this);

    dryWetRangeParam = *parameters.getRawParameterValue("dryWetRange");
    gainParam = *parameters.getRawParameterValue("outputGain");
    //modelIndex = static_cast<int>(*parameters.getRawParameterValue("modelSelection"));
    //std::cout << "Initial modelIndex: " << modelIndex.size << std::endl;
    adsrParams.attack = *parameters.getRawParameterValue("attackTime") / 1000.0f; 
    adsrParams.decay = *parameters.getRawParameterValue("decayTime") / 1000.0f;  
    adsrParams.sustain = *parameters.getRawParameterValue("sustain") / 100.0f;         
    adsrParams.release = *parameters.getRawParameterValue("releaseTime") / 1000.0f; 
}


//==============================================================================
const juce::String RAVE_for_MIDISynthesiser_Processor::getName() const
{
    return JucePlugin_Name;
}

bool RAVE_for_MIDISynthesiser_Processor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RAVE_for_MIDISynthesiser_Processor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RAVE_for_MIDISynthesiser_Processor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RAVE_for_MIDISynthesiser_Processor::getTailLengthSeconds() const
{
    return 0.0;
}

int RAVE_for_MIDISynthesiser_Processor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RAVE_for_MIDISynthesiser_Processor::getCurrentProgram()
{
    return 0;
}

void RAVE_for_MIDISynthesiser_Processor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String RAVE_for_MIDISynthesiser_Processor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void RAVE_for_MIDISynthesiser_Processor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void RAVE_for_MIDISynthesiser_Processor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mutedSamples = sampleRate * 2;
    totalSamplesProcessed = 0;
    juce::dsp::ProcessSpec spec {sampleRate,
                                 static_cast<juce::uint32>(samplesPerBlock),
                                 static_cast<juce::uint32>(getTotalNumInputChannels())};
    dry_wet_mixer.prepare(spec);

    anira::HostConfig host_config {
        static_cast<float>(samplesPerBlock),
        static_cast<float>(sampleRate),
        // true // Shall smaller buffers be allowed? If true more latency
    };

    inference_handler.prepare(host_config);

    int new_latency = (int) inference_handler.get_latency(); // The 0th tensor is the audio data tensor, so we only need the first element of the latency vector
    

    osc.prepare(spec);
    osc.setFrequency(440.0f);
    gain.prepare(spec);
    gain.setGainLinear(gainParam);

   
    setLatencySamples(new_latency);
    dry_wet_mixer.setWetLatency((float) new_latency);
    dry_wet_mixer.setWetMixProportion(dryWetRangeParam / 100.0f);

    midiMessageCollector.reset(sampleRate);
    adsr.setSampleRate(sampleRate);
    adsr.setParameters(adsrParams);

    #ifdef USE_LIBTORCH
        std::cout << "$Using LibTorch backend for inference.$" << std::endl;
        inference_handler.set_inference_backend(anira::InferenceBackend::LIBTORCH);
    #endif
}

void RAVE_for_MIDISynthesiser_Processor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool RAVE_for_MIDISynthesiser_Processor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
        return false;

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono())
        return false;
    else
        return true;
}

void RAVE_for_MIDISynthesiser_Processor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    midiMessageCollector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());
    
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            int noteNumber = msg.getNoteNumber();
            double freq = juce::MidiMessage::getMidiNoteInHertz(noteNumber);
            osc.setFrequency(static_cast<float>(freq));
            adsr.noteOn(); 
        }else if (msg.isNoteOff())
        {
            adsr.noteOff();
        }
    }

    juce::dsp::AudioBlock<float> audioBlock(buffer);
    //juce::dsp::ProcessContextReplacing rewrites the buffer itself.
    osc.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    adsr.applyEnvelopeToBuffer(buffer, 0, buffer.getNumSamples());

    gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));

    dry_wet_mixer.pushDrySamples(buffer);
    inference_handler.process(buffer.getArrayOfWritePointers(), (size_t) buffer.getNumSamples());
    dry_wet_mixer.mixWetSamples(buffer);

    if (isNonRealtime()) {
        processesNonRealtime(buffer);
    }

    //Cut the sound during warm-up inference
    int numSamples = buffer.getNumSamples();
    if (totalSamplesProcessed < static_cast<int64_t>(mutedSamples))
    {
        int64_t remainingSamples = std::min(static_cast<int64_t>(numSamples), static_cast<int64_t>(mutedSamples) - totalSamplesProcessed);
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.clear(channel, 0, static_cast<int>(remainingSamples));
    }
    totalSamplesProcessed += numSamples;
}

//==============================================================================
bool RAVE_for_MIDISynthesiser_Processor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RAVE_for_MIDISynthesiser_Processor::createEditor()
{
    return new RAVE_for_MIDISynthesiser_ProcessorEditor (*this,  parameters);
}

//==============================================================================
void RAVE_for_MIDISynthesiser_Processor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void RAVE_for_MIDISynthesiser_Processor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}


void RAVE_for_MIDISynthesiser_Processor::parameterChanged(const juce::String &parameterID, float newValue) {
    if (parameterID == "dryWetRange") 
    {
        dryWetRangeParam = newValue;
        dry_wet_mixer.setWetMixProportion(dryWetRangeParam / 100.0f);
        std::cout << "DryWetRange changed to: " << dryWetRangeParam << std::endl;
    }else if (parameterID == "outputGain") 
    {
        gainParam = newValue;
        gain.setGainLinear(gainParam);
        std::cout << "OutputGain changed to: " << newValue << std::endl;
    }
    //else if (parameterID == "modelSelection") 
    //{
    //    modelIndex = static_cast<int>(newValue);
    //    std::string selectedFile = modelFileNames[modelIndex].toStdString();
    //    std::cout << "ModelSelection changed to: " << selectedFile << std::endl;
    //    inference_config = makeRaveModelConfig(selectedFile);
    //}
    else if (parameterID == "attackTime") 
    {
        adsrParams.attack = newValue / 1000.0f;
        adsr.setParameters(adsrParams);
        std::cout << "AttackTime changed to: " << newValue << " ms" << std::endl;
    }else if (parameterID == "decayTime") 
    {
        adsrParams.decay = newValue / 1000.0f;
        adsr.setParameters(adsrParams);
        std::cout << "DecayTime changed to: " << newValue << " ms" << std::endl;
    }else if (parameterID == "sustain") 
    {
        adsrParams.sustain = newValue / 100.0f; 
        adsr.setParameters(adsrParams);
        std::cout << "Sustain changed to: " << newValue << std::endl;
    }else if (parameterID == "releaseTime") 
    {
        adsrParams.release = newValue / 1000.0f; 
        adsr.setParameters(adsrParams);
        std::cout << "ReleaseTime changed to: " << newValue << " ms" << std::endl;
    }       
}

void RAVE_for_MIDISynthesiser_Processor::processesNonRealtime(const juce::AudioBuffer<float>& buffer) const {
    double durationInSeconds = static_cast<double>(buffer.getNumSamples()) / getSampleRate();
    auto durationInMilliseconds = std::chrono::duration<double, std::milli>(durationInSeconds * 1000);
    std::this_thread::sleep_for(durationInMilliseconds);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RAVE_for_MIDISynthesiser_Processor();
}