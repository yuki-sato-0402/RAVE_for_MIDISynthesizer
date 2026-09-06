#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RAVE_for_MIDISynthesizer_Processor::RAVE_for_MIDISynthesizer_Processor() 
        : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                       ),
        apvts(*this, nullptr, juce::Identifier("PARAMETERS"),
        juce::AudioProcessorValueTreeState::ParameterLayout {
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "dryWetRange",  1}, "DryWetRange",
        juce::NormalisableRange<float>(0.f, 100.f, 0.01f), 0.f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "oscMix",  1}, "OscMix",
        juce::NormalisableRange<float>(0.f, 100.f, 0.01f), 0.f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "outputGain",  1}, "OutputGain",
        juce::NormalisableRange<float>(0.f, 1.f, 0.01f), 0.5f),
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
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable1Bias",  1}, "latentVariable1Bias",
        juce::NormalisableRange<float>(-2.5f, 2.5f, 0.01f), 0.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable2Bias",  1}, "latentVariable2Bias",
        juce::NormalisableRange<float>(-2.5f, 2.5f, 0.01f), 0.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable3Bias",  1}, "latentVariable3Bias",
        juce::NormalisableRange<float>(-2.5f, 2.5f, 0.01f), 0.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable4Bias",  1}, "latentVariable4Bias",
        juce::NormalisableRange<float>(-2.5f, 2.5f, 0.01f), 0.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable5Bias",  1}, "latentVariable5Bias",
        juce::NormalisableRange<float>(-2.5f, 2.5f, 0.01f), 0.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable6Bias",  1}, "latentVariable6Bias",
        juce::NormalisableRange<float>(-2.5f, 2.5f, 0.01f), 0.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable7Bias",  1}, "latentVariable7Bias",
        juce::NormalisableRange<float>(-2.5f, 2.5f, 0.01f), 0.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "latentVariable8Bias",  1}, "latentVariable8Bias",
        juce::NormalisableRange<float>(-2.5f, 2.5f, 0.01f), 0.0f),
        }
        ),
        //anira_context_config(
        //    std::thread::hardware_concurrency() / 2 > 0 ? std::thread::hardware_concurrency() / 2 : 1 // Total number of threads
        //),
        //In InferenceHandler.cpp, the inference_backend is set to m_inference_manager.
        dry_wet_mixer(32768) // 32768 samples of max latency compensation for the dry-wet mixer
{
    pp_processor = std::make_unique<anira::PrePostProcessor>(*inference_config);
    custom_backend = std::make_unique<anira::CustomBackend>(*inference_config);
    inference_handler = std::make_unique<anira::InferenceHandler>(*pp_processor, *inference_config, *custom_backend);

    apvts.addParameterListener("dryWetRange", this);
    apvts.addParameterListener("oscMix", this);
    apvts.addParameterListener("outputGain", this);
    apvts.state.addListener(this);// for filechooser lastFilePath
    apvts.addParameterListener("attackTime", this);
    apvts.addParameterListener("decayTime", this);
    apvts.addParameterListener("sustain", this);
    apvts.addParameterListener("releaseTime", this);
    apvts.addParameterListener("latentVariable1", this);
    apvts.addParameterListener("latentVariable2", this);
    apvts.addParameterListener("latentVariable3", this);
    apvts.addParameterListener("latentVariable4", this);
    apvts.addParameterListener("latentVariable5", this);
    apvts.addParameterListener("latentVariable6", this);
    apvts.addParameterListener("latentVariable7", this);
    apvts.addParameterListener("latentVariable8", this);
    apvts.addParameterListener("latentVariable1Bias", this);
    apvts.addParameterListener("latentVariable2Bias", this);
    apvts.addParameterListener("latentVariable3Bias", this);
    apvts.addParameterListener("latentVariable4Bias", this);
    apvts.addParameterListener("latentVariable5Bias", this);
    apvts.addParameterListener("latentVariable6Bias", this);
    apvts.addParameterListener("latentVariable7Bias", this);
    apvts.addParameterListener("latentVariable8Bias", this);

    dryWetRangeParam = *apvts.getRawParameterValue("dryWetRange");
    oscMixParam = *apvts.getRawParameterValue("oscMix");
    gainParam = *apvts.getRawParameterValue("outputGain");
    //modelIndex = static_cast<int>(*apvts.getRawParameterValue("modelSelection"));
    //std::cout << "Initial modelIndex: " << modelIndex.size << std::endl;
    adsrParams.attack = *apvts.getRawParameterValue("attackTime") / 1000.0f; 
    adsrParams.decay = *apvts.getRawParameterValue("decayTime") / 1000.0f;  
    adsrParams.sustain = *apvts.getRawParameterValue("sustain") / 100.0f;         
    adsrParams.release = *apvts.getRawParameterValue("releaseTime") / 1000.0f; 
    latentVariable1ScaleParam = *apvts.getRawParameterValue("latentVariable1");
    latentVariable2ScaleParam = *apvts.getRawParameterValue("latentVariable2");
    latentVariable3ScaleParam = *apvts.getRawParameterValue("latentVariable3");
    latentVariable4ScaleParam = *apvts.getRawParameterValue("latentVariable4");
    latentVariable5ScaleParam = *apvts.getRawParameterValue("latentVariable5");
    latentVariable6ScaleParam = *apvts.getRawParameterValue("latentVariable6");
    latentVariable7ScaleParam = *apvts.getRawParameterValue("latentVariable7");
    latentVariable8ScaleParam = *apvts.getRawParameterValue("latentVariable8");

    latentVariable1BiasParam = *apvts.getRawParameterValue("latentVariable1Bias");
    latentVariable2BiasParam = *apvts.getRawParameterValue("latentVariable2Bias");
    latentVariable3BiasParam = *apvts.getRawParameterValue("latentVariable3Bias");
    latentVariable4BiasParam = *apvts.getRawParameterValue("latentVariable4Bias");
    latentVariable5BiasParam = *apvts.getRawParameterValue("latentVariable5Bias");
    latentVariable6BiasParam = *apvts.getRawParameterValue("latentVariable6Bias");
    latentVariable7BiasParam = *apvts.getRawParameterValue("latentVariable7Bias");
    latentVariable8BiasParam = *apvts.getRawParameterValue("latentVariable8Bias");

    custom_backend->setLatentScalePublic(0, latentVariable1ScaleParam);
    custom_backend->setLatentScalePublic(1, latentVariable2ScaleParam);
    custom_backend->setLatentScalePublic(2, latentVariable3ScaleParam);
    custom_backend->setLatentScalePublic(3, latentVariable4ScaleParam);
    custom_backend->setLatentScalePublic(4, latentVariable5ScaleParam);
    custom_backend->setLatentScalePublic(5, latentVariable6ScaleParam);
    custom_backend->setLatentScalePublic(6, latentVariable7ScaleParam);
    custom_backend->setLatentScalePublic(7, latentVariable8ScaleParam);

    custom_backend->setLatentBiasPublic(0, latentVariable1BiasParam);
    custom_backend->setLatentBiasPublic(1, latentVariable2BiasParam);
    custom_backend->setLatentBiasPublic(2, latentVariable3BiasParam);
    custom_backend->setLatentBiasPublic(3, latentVariable4BiasParam);
    custom_backend->setLatentBiasPublic(4, latentVariable5BiasParam);
    custom_backend->setLatentBiasPublic(5, latentVariable6BiasParam);
    custom_backend->setLatentBiasPublic(6, latentVariable7BiasParam);
    custom_backend->setLatentBiasPublic(7, latentVariable8BiasParam);
}


//==============================================================================
const juce::String RAVE_for_MIDISynthesizer_Processor::getName() const
{
    return JucePlugin_Name;
}

bool RAVE_for_MIDISynthesizer_Processor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RAVE_for_MIDISynthesizer_Processor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RAVE_for_MIDISynthesizer_Processor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RAVE_for_MIDISynthesizer_Processor::getTailLengthSeconds() const
{
    return 0.0;
}

int RAVE_for_MIDISynthesizer_Processor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RAVE_for_MIDISynthesizer_Processor::getCurrentProgram()
{
    return 0;
}

void RAVE_for_MIDISynthesizer_Processor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String RAVE_for_MIDISynthesizer_Processor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void RAVE_for_MIDISynthesizer_Processor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void RAVE_for_MIDISynthesizer_Processor::prepareToPlay (double sampleRate, int samplesPerBlock)
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
    inference_handler->prepare(host_config);
    
    int new_latency = (int) inference_handler->get_latency();

    juce::dsp::ProcessSpec monoSpec {sampleRate,
                                     static_cast<juce::uint32>(samplesPerBlock),
                                     1};
    for (auto& v : voices)
    {
        v.prepare(monoSpec, sampleRate, adsrParams);
    }
    globalNoteCounter = 0;

    gain.prepare(spec);
    gain.setGainLinear(gainParam);

    setLatencySamples(new_latency);
    dry_wet_mixer.setWetLatency((float) new_latency);
    dry_wet_mixer.setWetMixProportion(dryWetRangeParam / 100.0f);

    midiMessageCollector.reset(sampleRate);

    std::cout << "$Using CUSTOM backend for inference.$" << std::endl;
    inference_handler->set_inference_backend(anira::InferenceBackend::CUSTOM);
}

void RAVE_for_MIDISynthesizer_Processor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool RAVE_for_MIDISynthesizer_Processor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() != layouts.getMainOutputChannelSet())
        return false;

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono())
        return false;
    else
        return true;
}

void RAVE_for_MIDISynthesizer_Processor::processBlock (juce::AudioBuffer<float>& buffer,
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
            globalNoteCounter++; // Update the timestamp with every new Note On event.

            Voice* targetVoice = nullptr; // Pointer to the voice that will be used for this note event.

            // [Step 1] If there is already a voice playing the same note number, reuse it (retrigger).
            for (auto& v : voices)
            {
                if (v.active && v.noteNumber == noteNumber)
                {
                    targetVoice = &v; // Reuse the existing voice for retriggering
                    break;
                }
            }

            // [Step 2] Search for an unused (available) voice.
            if (targetVoice == nullptr)
            {
                for (auto& v : voices)
                {
                    if (!v.active)
                    {
                        targetVoice = &v; // Found an available voice
                        break;
                    }
                }
            }

            // [Step 3] If all 4 voices are occupied, steal the oldest one (Voice Stealing)
            if (targetVoice == nullptr)
            {
                uint64_t oldestTime = std::numeric_limits<uint64_t>::max();
                for (auto& v : voices)
                {
                    if (v.noteOnCounter < oldestTime)
                    {
                        oldestTime = v.noteOnCounter;
                        targetVoice = &v; // Steal the oldest voice
                    }
                }
            }

            // Set the note information and frequency for the selected voice and start the ADSR.
            if (targetVoice != nullptr)
            {
                targetVoice->noteNumber = noteNumber;
                targetVoice->active = true;
                targetVoice->noteOnCounter = globalNoteCounter;

                double freq = juce::MidiMessage::getMidiNoteInHertz(noteNumber);
                targetVoice->sinOsc.setFrequency(static_cast<float>(freq));
                targetVoice->squareOsc.setFrequency(static_cast<float>(freq));

                targetVoice->adsr.noteOn();
            }
        }
        else if (msg.isNoteOff())
        {
            int noteNumber = msg.getNoteNumber();
            // Transition the ADSR of the voice matching the released note number to the release state (Note Off).
            for (auto& v : voices)
            {
                if (v.active && v.noteNumber == noteNumber)
                {
                    v.adsr.noteOff();
                }
            }
        }
    }

    // Audio synthesis and rendering
    buffer.clear(); 

    // Calculate the mix ratio of sine wave to square wave (oscMixParam: 0.0f = 100% sine, 100.0f = 100% square)
    float sinRatio = 1.0f - (oscMixParam / 100.0f);
    float squareRatio = oscMixParam / 100.0f;

    int numSamples = buffer.getNumSamples();
    juce::AudioBuffer<float> voiceBuffer(1, numSamples);  // Voice-specific mixed audio work buffer
    juce::AudioBuffer<float> sinBuffer(1, numSamples);    // Sine wave dedicated buffer
    juce::AudioBuffer<float> squareBuffer(1, numSamples); // Square wave dedicated buffer

    // Generate waveforms for each voice, apply envelopes, and add to the main buffer
    for (auto& v : voices)
    {
        // Voice is active or has a pending release envelope
        if (v.active || v.adsr.isActive())
        {
            voiceBuffer.clear();
            sinBuffer.clear();
            squareBuffer.clear();

            juce::dsp::AudioBlock<float> sinBlock(sinBuffer);
            juce::dsp::AudioBlock<float> squareBlock(squareBuffer);

            // Generate sine and square waves independently
            v.sinOsc.process(juce::dsp::ProcessContextReplacing<float>(sinBlock));
            v.squareOsc.process(juce::dsp::ProcessContextReplacing<float>(squareBlock));

            auto* vWrite = voiceBuffer.getWritePointer(0);
            const auto* sRead = sinBuffer.getReadPointer(0);
            const auto* sqRead = squareBuffer.getReadPointer(0);

            // Blend two types of waveforms at a specified ratio on a sample-by-sample basis.
            for (int i = 0; i < numSamples; ++i)
            {
                vWrite[i] = (sRead[i] * sinRatio + sqRead[i] * squareRatio) * 0.5f;
            }

            // Apply voice-specific ADSR envelope
            v.adsr.applyEnvelopeToBuffer(voiceBuffer, 0, numSamples);

            // Add the voice's audio to the main output buffer (polyphonic synthesis)
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                buffer.addFrom(ch, 0, voiceBuffer, 0, 0, numSamples);
            }

            // Completely deactivate the voice once the ADSR release phase has finished.
            if (!v.adsr.isActive())
            {
                v.active = false;
                v.noteNumber = -1;
            }
        }
    }

    dry_wet_mixer.pushDrySamples(buffer);

    inference_handler->process(buffer.getArrayOfWritePointers(), (size_t) buffer.getNumSamples());
    
    dry_wet_mixer.mixWetSamples(buffer);

    juce::dsp::AudioBlock<float> audioBlock(buffer);
    gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));

    if (isNonRealtime()) {
        processesNonRealtime(buffer);
    }

    //Cut the sound during warm-up inference
    if (totalSamplesProcessed < static_cast<int64_t>(mutedSamples))
    {
        int64_t remainingSamples = std::min(static_cast<int64_t>(numSamples), static_cast<int64_t>(mutedSamples) - totalSamplesProcessed);
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.clear(channel, 0, static_cast<int>(remainingSamples));
    }
    totalSamplesProcessed += numSamples;
}

//==============================================================================
bool RAVE_for_MIDISynthesizer_Processor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RAVE_for_MIDISynthesizer_Processor::createEditor()
{
    return new RAVE_for_MIDISynthesizer_ProcessorEditor (*this,  apvts);
}

//==============================================================================
void RAVE_for_MIDISynthesizer_Processor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void RAVE_for_MIDISynthesizer_Processor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}


void RAVE_for_MIDISynthesizer_Processor::parameterChanged(const juce::String &parameterID, float newValue) {
    if (parameterID == "dryWetRange") 
    {
        dryWetRangeParam = newValue;
        dry_wet_mixer.setWetMixProportion(dryWetRangeParam / 100.0f);
        std::cout << "DryWetRange changed to: " << dryWetRangeParam << std::endl;
    }else if (parameterID == "oscMix") 
    {
        oscMixParam = newValue;
        std::cout << "oscMix changed to: " << oscMixParam << std::endl;
    }else if (parameterID == "outputGain") 
    {
        gainParam = newValue;
        gain.setGainLinear(gainParam);
        std::cout << "OutputGain changed to: " << newValue << std::endl;
    }else if (parameterID == "attackTime") 
    {
        adsrParams.attack = newValue / 1000.0f;
        for (auto& v : voices) v.adsr.setParameters(adsrParams);
        std::cout << "AttackTime changed to: " << newValue << " ms" << std::endl;
    }else if (parameterID == "decayTime") 
    {
        adsrParams.decay = newValue / 1000.0f;
        for (auto& v : voices) v.adsr.setParameters(adsrParams);
        std::cout << "DecayTime changed to: " << newValue << " ms" << std::endl;
    }else if (parameterID == "sustain") 
    {
        adsrParams.sustain = newValue / 100.0f; 
        for (auto& v : voices) v.adsr.setParameters(adsrParams);
        std::cout << "Sustain changed to: " << newValue << std::endl;
    }else if (parameterID == "releaseTime") 
    {
        adsrParams.release = newValue / 1000.0f; 
        for (auto& v : voices) v.adsr.setParameters(adsrParams);
        std::cout << "ReleaseTime changed to: " << newValue << " ms" << std::endl;
    }else if (parameterID == "latentVariable1") 
    {
        custom_backend->setLatentScalePublic(0, newValue);
        std::cout << "latentVariable1 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable2") 
    {
        custom_backend->setLatentScalePublic(1, newValue);
        std::cout << "latentVariable2 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable3") 
    {
        custom_backend->setLatentScalePublic(2, newValue);
        std::cout << "latentVariable3 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable4") 
    {
        custom_backend->setLatentScalePublic(3, newValue);
        std::cout << "latentVariable4 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable5") 
    {
        custom_backend->setLatentScalePublic(4, newValue);
        std::cout << "latentVariable5 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable6") 
    {
        custom_backend->setLatentScalePublic(5, newValue);
        std::cout << "latentVariable6 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable7") 
    {
        custom_backend->setLatentScalePublic(6, newValue);
        std::cout << "latentVariable7 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable8") 
    {
        custom_backend->setLatentScalePublic(7, newValue);
        std::cout << "latentVariable8 changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable1Bias") 
    {
        latentVariable1BiasParam = newValue;
        custom_backend->setLatentBiasPublic(0, newValue);
        std::cout << "latentVariable1Bias changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable2Bias") 
    {
        latentVariable2BiasParam = newValue;
        custom_backend->setLatentBiasPublic(1, newValue);
        std::cout << "latentVariable2Bias changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable3Bias") 
    {
        latentVariable3BiasParam = newValue;
        custom_backend->setLatentBiasPublic(2, newValue);
        std::cout << "latentVariable3Bias changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable4Bias") 
    {
        latentVariable4BiasParam = newValue;
        custom_backend->setLatentBiasPublic(3, newValue);
        std::cout << "latentVariable4Bias changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable5Bias") 
    {
        latentVariable5BiasParam = newValue;
        custom_backend->setLatentBiasPublic(4, newValue);
        std::cout << "latentVariable5Bias changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable6Bias") 
    {
        latentVariable6BiasParam = newValue;
        custom_backend->setLatentBiasPublic(5, newValue);
        std::cout << "latentVariable6Bias changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable7Bias") 
    {
        latentVariable7BiasParam = newValue;
        custom_backend->setLatentBiasPublic(6, newValue);
        std::cout << "latentVariable7Bias changed to: " << newValue << std::endl;
    }else if (parameterID == "latentVariable8Bias") 
    {
        latentVariable8BiasParam = newValue;
        custom_backend->setLatentBiasPublic(7, newValue);
        std::cout << "latentVariable8Bias changed to: " << newValue << std::endl;
    }           
}

void RAVE_for_MIDISynthesizer_Processor::processesNonRealtime(const juce::AudioBuffer<float>& buffer) const {
    double durationInSeconds = static_cast<double>(buffer.getNumSamples()) / getSampleRate();
    auto durationInMilliseconds = std::chrono::duration<double, std::milli>(durationInSeconds * 1000);
    std::this_thread::sleep_for(durationInMilliseconds);
}

float RAVE_for_MIDISynthesizer_Processor::getLatentVariables(const int index) {
    return custom_backend->getEncoderOutputPublic(index);
} 

void RAVE_for_MIDISynthesizer_Processor::valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property)
{
    // Check if the "lastFilePath" property has been modified
    if (property == juce::Identifier("lastFilePath"))
    {
        std::string currentFilePath = tree.getProperty(property).toString().toStdString();
        std::cout << "ValueTree Property Changed - lastFilePath: " << currentFilePath << std::endl;
        
        updateModel(currentFilePath);
    }
}

void RAVE_for_MIDISynthesizer_Processor::updateModel(std::string newModelPath){
    juce::AudioProcessor::suspendProcessing(true);
    inference_handler.reset();
    pp_processor.reset();
    custom_backend.reset();
    inference_config.reset();

    std::vector<anira::ModelData> newModelData = {
        {newModelPath, anira::InferenceBackend::CUSTOM}
    };
    
    inference_config = std::make_unique<anira::InferenceConfig>(
        newModelData,
        tensorShape,    
        processingSpec,
        200.00f,
        5,
        false
    );

    custom_backend = std::make_unique<anira::CustomBackend>(*inference_config);
    pp_processor = std::make_unique<anira::PrePostProcessor>(*inference_config);
    
    // Assigning to the member variable inference_config causes it to remain in memory even after the function terminates.
    inference_handler = std::make_unique<anira::InferenceHandler>(
        *pp_processor, *inference_config, *custom_backend
    );

    anira::HostConfig host_config {
        static_cast<float>(getBlockSize()),
        static_cast<float>(getSampleRate())
    };
    inference_handler->prepare(host_config);
    int new_latency = (int) inference_handler->get_latency();
    setLatencySamples(new_latency);
    dry_wet_mixer.setWetLatency((float) new_latency);
    inference_handler->set_inference_backend(anira::InferenceBackend::CUSTOM);

    for (size_t i = 0; i < 8; ++i) {
        juce::String scaleID = "latentVariable" + juce::String(i + 1);
        juce::String biasID = "latentVariable" + juce::String(i + 1) + "Bias";
        custom_backend->setLatentScalePublic(i, *apvts.getRawParameterValue(scaleID));
        custom_backend->setLatentBiasPublic(i, *apvts.getRawParameterValue(biasID));
    }

    juce::AudioProcessor::suspendProcessing(false);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RAVE_for_MIDISynthesizer_Processor();
}