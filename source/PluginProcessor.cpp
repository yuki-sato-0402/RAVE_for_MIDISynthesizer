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

    dryWetRangeParam = *apvts.getRawParameterValue("dryWetRange");
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

            // === First Note-On ===
            if (!noteActive && loopStep == 0)
            {
                rootNote = noteNumber;
                double freq = juce::MidiMessage::getMidiNoteInHertz(rootNote);
                osc.setFrequency(static_cast<float>(freq));
                adsr.noteOn();

                noteActive = true;
                loopStep = 1; 
                lastNote = rootNote;

                //std::cout << "Root Note On: " << rootNote << std::endl;
            }
            // === Second to Ninth Time ===
            else if (noteActive && loopStep > 0 && loopStep <= 8 && !waitingForRelease)
            {
                int diff = noteNumber - rootNote;

                latentVariableBias[loopStep - 1] = static_cast<float>(diff) / 12.0f * 2.5f;

                lastNote = noteNumber;
                waitingForRelease = true; // Next up: waiting for the off
                sendActionMessage("NoteOn" + juce::String(latentVariableBias[loopStep - 1]) + ","  + juce::String(loopStep));
                //std::cout << loopStep - 1 << " latentVariableBias[loopStep - 1]: " << latentVariableBias[loopStep - 1] << std::endl;
                custom_backend->setLatentBiasPublic(loopStep - 1 , latentVariableBias[loopStep - 1]);
            }
        }
        else if (msg.isNoteOff())
        {
            int noteNumber = msg.getNoteNumber();

            // Reset when the first one turns off
            if (noteActive && noteNumber == rootNote)
            {
                noteActive = false;
                loopStep = 0;
                sendActionMessage("RootNoteOff");
                adsr.noteOff();
            }
            // Progression Rule: Once the previous note is off, proceed to the next step.
            else if (waitingForRelease && noteNumber == lastNote)
            {
                waitingForRelease = false;
                loopStep++;

                if (loopStep > 8) 
                {
                    loopStep = 1; // Starting from the second time
                    std::cout << "Loop Completed -> Restart" << std::endl;
                }
            }
        }
    }


    juce::dsp::AudioBlock<float> audioBlock(buffer);
    //juce::dsp::ProcessContextReplacing rewrites the buffer itself.
    osc.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    adsr.applyEnvelopeToBuffer(buffer, 0, buffer.getNumSamples());

    dry_wet_mixer.pushDrySamples(buffer);

    inference_handler->process(buffer.getArrayOfWritePointers(), (size_t) buffer.getNumSamples());
    
    dry_wet_mixer.mixWetSamples(buffer);

    gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));

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
    }else if (parameterID == "outputGain") 
    {
        gainParam = newValue;
        gain.setGainLinear(gainParam);
        std::cout << "OutputGain changed to: " << newValue << std::endl;
    }else if (parameterID == "attackTime") 
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

    juce::AudioProcessor::suspendProcessing(false);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RAVE_for_MIDISynthesizer_Processor();
}