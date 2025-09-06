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
        juce::NormalisableRange<float>(1, 1000, 1), 200),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable1",  1}, "latentVariable1",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 1.0f), 
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable2",  1}, "latentVariable2",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 1.0f), 
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable3",  1}, "latentVariable3",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable4",  1}, "latentVariable4",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable5",  1}, "latentVariable5",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable6",  1}, "latentVariable6",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable7",  1}, "latentVariable7",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 1.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable8",  1}, "latentVariable8",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 1.0f),    
        }
        ),
        //anira_context_config(
        //    std::thread::hardware_concurrency() / 2 > 0 ? std::thread::hardware_concurrency() / 2 : 1 // Total number of threads
        //),
        pp_processor_encoder(inference_config_encoder),
        pp_processor_decoder(inference_config_decoder),
        inference_handler_encoder(pp_processor_encoder, inference_config_encoder),
        inference_handler_decoder(pp_processor_decoder, inference_config_decoder),
        dry_wet_mixer(32768) // 32768 samples of max latency compensation for the dry-wet mixer
{
    parameters.addParameterListener("dryWetRange", this);
    parameters.addParameterListener("outputGain", this);
    //parameters.addParameterListener("modelSelection", this);
    parameters.addParameterListener("attackTime", this);
    parameters.addParameterListener("decayTime", this);
    parameters.addParameterListener("sustain", this);
    parameters.addParameterListener("releaseTime", this);
    parameters.addParameterListener("latentVariable1", this);
    parameters.addParameterListener("latentVariable2", this);
    parameters.addParameterListener("latentVariable3", this);
    parameters.addParameterListener("latentVariable4", this);
    parameters.addParameterListener("latentVariable5", this);
    parameters.addParameterListener("latentVariable6", this);
    parameters.addParameterListener("latentVariable7", this);
    parameters.addParameterListener("latentVariable8", this);

    dryWetRangeParam = *parameters.getRawParameterValue("dryWetRange");
    gainParam = *parameters.getRawParameterValue("outputGain");
    //modelIndex = static_cast<int>(*parameters.getRawParameterValue("modelSelection"));
    //std::cout << "Initial modelIndex: " << modelIndex.size << std::endl;
    adsrParams.attack = *parameters.getRawParameterValue("attackTime") / 1000.0f; 
    adsrParams.decay = *parameters.getRawParameterValue("decayTime") / 1000.0f;  
    adsrParams.sustain = *parameters.getRawParameterValue("sustain") / 100.0f;         
    adsrParams.release = *parameters.getRawParameterValue("releaseTime") / 1000.0f; 
    latentVariable1Param = *parameters.getRawParameterValue("latentVariable1");
    latentVariable2Param = *parameters.getRawParameterValue("latentVariable2");
    latentVariable3Param = *parameters.getRawParameterValue("latentVariable3");
    latentVariable4Param = *parameters.getRawParameterValue("latentVariable4");
    latentVariable5Param = *parameters.getRawParameterValue("latentVariable5");
    latentVariable6Param = *parameters.getRawParameterValue("latentVariable6");
    latentVariable7Param = *parameters.getRawParameterValue("latentVariable7");
    latentVariable8Param = *parameters.getRawParameterValue("latentVariable8");
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

    anira::HostConfig host_config_encoder {
        static_cast<float>(samplesPerBlock),
        static_cast<float>(sampleRate),
        // true // Shall smaller buffers be allowed? If true more latency
    };
    // The decoder needs to be prepared with the buffer size and sample rate of the latent space.
    anira::HostConfig host_config_decoder {
        static_cast<float>((float) samplesPerBlock / 2048.f),
        static_cast<float>((float) sampleRate / 2048.f),
        // true // Shall smaller buffers be allowed?
    };
    inference_handler_encoder.prepare(host_config_encoder);
    inference_handler_decoder.prepare(host_config_decoder);

    // Encoder latency must be multiplied by 2048, because the encoder compresses the audio data by a factor of 2048 in the time domain.
    int new_latency_encoder = (int) inference_handler_encoder.get_latency() * 2048;
    int new_latency_decoder = (int) inference_handler_decoder.get_latency();
    int new_latency = new_latency_encoder + new_latency_decoder; // The total latency is the sum of the latencies of both encoders

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
        inference_handler_encoder.set_inference_backend(anira::InferenceBackend::LIBTORCH);
        inference_handler_decoder.set_inference_backend(anira::InferenceBackend::LIBTORCH);
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

    // For the RAVE model, we need to process the encoder and decoder separately.
    float* latent_space_ptrs[8];
    for (int i = 0; i < 8; ++i) {
        latent_space_ptrs[i] = latent_space[i];
    }
    m_count_input_samples += buffer.getNumSamples();
    inference_handler_encoder.push_data(buffer.getArrayOfWritePointers(), (size_t) buffer.getNumSamples());
    // Only pop data from the encoder when we have enough samples needed for one time step in the latent space vector (2048 samples).
    while (m_count_input_samples >= 2048) {
        size_t received_samples = inference_handler_encoder.pop_data(latent_space_ptrs, 1);
        if (received_samples == 0) {
            std::cout << "No data received from encoder!" << std::endl;
            break;
        } else {
            m_count_input_samples -= 2048;
        }
        // Make some latent space modulation :)
        latent_space[0][0] *= latentVariable1Param;
        latent_space[1][0] *= latentVariable2Param;
        latent_space[2][0] *= latentVariable3Param;
        latent_space[3][0] *= latentVariable4Param;
        latent_space[4][0] *= latentVariable5Param;
        latent_space[5][0] *= latentVariable6Param;
        latent_space[6][0] *= latentVariable7Param;
        latent_space[7][0] *= latentVariable8Param;

        //std::cout << "Latent space values: ";
        //for (int i = 0; i < 8; ++i) {
        //    std::cout << latent_space[i][0] << " ";
        //}
        //std::cout << std::endl;

        inference_handler_decoder.push_data(latent_space_ptrs, received_samples);
    }
    inference_handler_decoder.pop_data(buffer.getArrayOfWritePointers(), (size_t) buffer.getNumSamples());
    
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
    }else if (parameterID == "latentVariable1") 
    {
        latentVariable1Param = newValue;
        std::cout << "latentVariable1 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable2") 
    {
        latentVariable2Param = newValue;
        std::cout << "latentVariable2 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable3") 
    {
        latentVariable3Param = newValue;
        std::cout << "latentVariable3 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable4") 
    {
        latentVariable4Param = newValue;
        std::cout << "latentVariable4 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable5") 
    {
        latentVariable5Param = newValue;
        std::cout << "latentVariable5 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable6") 
    {
        latentVariable6Param = newValue;
        std::cout << "latentVariable6 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable7") 
    {
        latentVariable7Param = newValue;
        std::cout << "latentVariable7 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable8") 
    {
        latentVariable8Param = newValue;
        std::cout << "latentVariable8 changed to: " << newValue << std::endl;
    }           
}

void RAVE_for_MIDISynthesiser_Processor::processesNonRealtime(const juce::AudioBuffer<float>& buffer) const {
    double durationInSeconds = static_cast<double>(buffer.getNumSamples()) / getSampleRate();
    auto durationInMilliseconds = std::chrono::duration<double, std::milli>(durationInSeconds * 1000);
    std::this_thread::sleep_for(durationInMilliseconds);
}

float RAVE_for_MIDISynthesiser_Processor::getLatentVariables(const int index) {
    switch (index) {
        case 1:
            return latent_space[0][0];
        case 2:
            return latent_space[1][0];
        case 3:
            return latent_space[2][0];
        case 4:
            return latent_space[3][0];
        case 5:
            return latent_space[4][0];
        case 6:
            return latent_space[5][0];
        case 7:
            return latent_space[6][0];
        case 8:
            return latent_space[7][0];
        default:
            throw std::out_of_range("Index must be between 1 and 8.");
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RAVE_for_MIDISynthesiser_Processor();
}