/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#include "PluginEditor.h"
#include "PluginProcessor.h"

RAVE_for_MIDISynthesizer_ProcessorEditor::RAVE_for_MIDISynthesizer_ProcessorEditor(RAVE_for_MIDISynthesizer_Processor& p, juce::AudioProcessorValueTreeState& apvts)
    : AudioProcessorEditor (&p),processorRef (p),  valueTreeState(apvts)
{
  MixSliderAttachment.reset (new SliderAttachment (valueTreeState, "dryWetRange", MixSlider));
  MixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  MixSlider.setTextValueSuffix (" %");     
  MixSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, MixSlider.getTextBoxWidth(), MixSlider.getTextBoxHeight());
  addAndMakeVisible(MixSlider);
  MixSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  MixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  MixSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  MixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  MixSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

 
  MixLabel.setText ("dryWetRange", juce::dontSendNotification);
  MixLabel.setJustificationType(juce::Justification::centred);
  MixLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(MixLabel);

  GainSliderAttachment.reset (new SliderAttachment (valueTreeState, "outputGain", GainSlider));
  GainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  GainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, GainSlider.getTextBoxWidth(), GainSlider.getTextBoxHeight());
  addAndMakeVisible(GainSlider);
  GainSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  GainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  GainSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  GainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  GainSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));
 
  GainLabel.setText ("outputGain", juce::dontSendNotification);
  GainLabel.setJustificationType(juce::Justification::centred);
  GainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(GainLabel);

  openFileButton.onClick = [this] { openFileButtonClicked(); };
  addAndMakeVisible(openFileButton);
  openFileButton.setColour(juce::TextButton::buttonColourId, juce::Colours::greenyellow.darker(0.1f));
  openFileButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

  AttackSliderAttachment.reset (new SliderAttachment (valueTreeState, "attackTime", AttackSlider));
  AttackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  AttackSlider.setTextValueSuffix (" ms");     
  AttackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, AttackSlider.getTextBoxWidth(), AttackSlider.getTextBoxHeight());
  addAndMakeVisible(AttackSlider);
  AttackSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  AttackSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  AttackSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  AttackSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  AttackSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));
  AttackSlider.addListener(this);

  AttackLabel.setText ("attackTime", juce::dontSendNotification);
  AttackLabel.setJustificationType(juce::Justification::centred);
  AttackLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(AttackLabel);

  DecaySliderAttachment.reset (new SliderAttachment (valueTreeState, "decayTime", DecaySlider));
  DecaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  DecaySlider.setTextValueSuffix (" ms");     
  DecaySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, DecaySlider.getTextBoxWidth(), DecaySlider.getTextBoxHeight());
  addAndMakeVisible(DecaySlider);
  DecaySlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  DecaySlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  DecaySlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  DecaySlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  DecaySlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));
  DecaySlider.addListener(this);

  DecayLabel.setText ("decayTime", juce::dontSendNotification);
  DecayLabel.setJustificationType(juce::Justification::centred);
  DecayLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(DecayLabel);

  SustainSliderAttachment.reset (new SliderAttachment (valueTreeState, "sustain", SustainSlider));
  SustainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  SustainSlider.setTextValueSuffix (" %");     
  SustainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, SustainSlider.getTextBoxWidth(), SustainSlider.getTextBoxHeight());
  addAndMakeVisible(SustainSlider);
  SustainSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  SustainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  SustainSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  SustainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  SustainSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));
  SustainSlider.addListener(this);

  SustainLabel.setText ("sustain", juce::dontSendNotification);
  SustainLabel.setJustificationType(juce::Justification::centred);
  SustainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(SustainLabel);

  ReleaseSliderAttachment.reset (new SliderAttachment (valueTreeState, "releaseTime", ReleaseSlider));
  ReleaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  ReleaseSlider.setTextValueSuffix (" ms");     
  ReleaseSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, ReleaseSlider.getTextBoxWidth(), ReleaseSlider.getTextBoxHeight()); 
  addAndMakeVisible(ReleaseSlider);
  ReleaseSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  ReleaseSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  ReleaseSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  ReleaseSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  ReleaseSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25)); 
  ReleaseSlider.addListener(this);

  ReleaseLabel.setText ("releaseTime", juce::dontSendNotification);
  ReleaseLabel.setJustificationType(juce::Justification::centred);
  ReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(ReleaseLabel);

  addAndMakeVisible(ScaleLabel);
  ScaleLabel.setText ("Scale Factor", juce::dontSendNotification);
  ScaleLabel.setJustificationType(juce::Justification::centred);
  ScaleLabel.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(latentVariable1Meter);
  latentVariable1Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable1Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable1Label);
  latentVariable1Label.setText ("latentVariable 1", juce::dontSendNotification);
  latentVariable1Label.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(latentVariable1Slider);
  latentVariable1SliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable1", latentVariable1Slider));
  latentVariable1Slider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable1Slider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable1Slider.getTextBoxWidth() - 30, latentVariable1Slider.getTextBoxHeight());  
  latentVariable1Slider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable1Slider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable1Slider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable1Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable1Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25)); 


  addAndMakeVisible(latentVariable2Meter);
  latentVariable2Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable2Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable2Label);
  latentVariable2Label.setText ("latentVariable 2", juce::dontSendNotification);
  latentVariable2Label.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(latentVariable2Slider);
  latentVariable2SliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable2", latentVariable2Slider));
  latentVariable2Slider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable2Slider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable2Slider.getTextBoxWidth() - 30, latentVariable2Slider.getTextBoxHeight());  
  latentVariable2Slider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable2Slider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable2Slider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable2Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable2Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  addAndMakeVisible(latentVariable3Meter);
  latentVariable3Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable3Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable3Label);
  latentVariable3Label.setText ("latentVariable 3", juce::dontSendNotification);
  latentVariable3Label.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(latentVariable3Slider);
  latentVariable3SliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable3", latentVariable3Slider));
  latentVariable3Slider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable3Slider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable3Slider.getTextBoxWidth() - 30, latentVariable3Slider.getTextBoxHeight());  
  latentVariable3Slider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable3Slider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable3Slider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable3Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable3Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  addAndMakeVisible(latentVariable4Meter);
  latentVariable4Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable4Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable4Label);
  latentVariable4Label.setText ("latentVariable 4", juce::dontSendNotification);
  latentVariable4Label.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(latentVariable4Slider);
  latentVariable4SliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable4", latentVariable4Slider));
  latentVariable4Slider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable4Slider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable4Slider.getTextBoxWidth() - 30, latentVariable4Slider.getTextBoxHeight());  
  latentVariable4Slider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable4Slider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable4Slider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable4Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable4Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  addAndMakeVisible(latentVariable5Meter);
  latentVariable5Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable5Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable5Label);
  latentVariable5Label.setText ("latentVariable 5", juce::dontSendNotification);
  latentVariable5Label.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(latentVariable5Slider);
  latentVariable5SliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable5", latentVariable5Slider));
  latentVariable5Slider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable5Slider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable5Slider.getTextBoxWidth() - 30, latentVariable5Slider.getTextBoxHeight());  
  latentVariable5Slider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable5Slider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable5Slider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable5Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable5Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25)); 

  addAndMakeVisible(latentVariable6Meter);
  latentVariable6Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable6Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));  
  addAndMakeVisible(latentVariable6Label);
  latentVariable6Label.setText ("latentVariable 6", juce::dontSendNotification);
  latentVariable6Label.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(latentVariable6Slider);
  latentVariable6SliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable6", latentVariable6Slider));
  latentVariable6Slider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable6Slider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable6Slider.getTextBoxWidth() - 30, latentVariable6Slider.getTextBoxHeight());  
  latentVariable6Slider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable6Slider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable6Slider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable6Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable6Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  addAndMakeVisible(latentVariable7Meter);
  latentVariable7Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable7Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));  
  addAndMakeVisible(latentVariable7Label);
  latentVariable7Label.setText ("latentVariable 7", juce::dontSendNotification);
  latentVariable7Label.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(latentVariable7Slider);
  latentVariable7SliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable7", latentVariable7Slider));
  latentVariable7Slider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable7Slider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable7Slider.getTextBoxWidth() - 30, latentVariable7Slider.getTextBoxHeight());  
  latentVariable7Slider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable7Slider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable7Slider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable7Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable7Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  addAndMakeVisible(latentVariable8Meter);
  latentVariable8Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable8Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable8Label);
  latentVariable8Label.setText ("latentVariable 8", juce::dontSendNotification);  
  latentVariable8Label.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(latentVariable8Slider);
  latentVariable8SliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable8", latentVariable8Slider));
  latentVariable8Slider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable8Slider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable8Slider.getTextBoxWidth() - 30, latentVariable8Slider.getTextBoxHeight());  
  latentVariable8Slider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable8Slider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable8Slider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable8Slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable8Slider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25)); 
  
  addAndMakeVisible(BiasLabel);
  BiasLabel.setText ("Bias", juce::dontSendNotification);
  BiasLabel.setJustificationType(juce::Justification::centred);
  BiasLabel.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(latentVariable1BiasLabel);
  latentVariable1BiasLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable1BiasLabel.setJustificationType(juce::Justification::centred);
  latentVariable1BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);
  
  addAndMakeVisible(latentVariable2BiasLabel);
  latentVariable2BiasLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable2BiasLabel.setJustificationType(juce::Justification::centred);
  latentVariable2BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);
  
  addAndMakeVisible(latentVariable3BiasLabel);
  latentVariable3BiasLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable3BiasLabel.setJustificationType(juce::Justification::centred);
  latentVariable3BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);

  addAndMakeVisible(latentVariable4BiasLabel);
  latentVariable4BiasLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable4BiasLabel.setJustificationType(juce::Justification::centred);
  latentVariable4BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);

  addAndMakeVisible(latentVariable5BiasLabel);
  latentVariable5BiasLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable5BiasLabel.setJustificationType(juce::Justification::centred);
  latentVariable5BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);

  addAndMakeVisible(latentVariable6BiasLabel);
  latentVariable6BiasLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable6BiasLabel.setJustificationType(juce::Justification::centred);
  latentVariable6BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);
  
  addAndMakeVisible(latentVariable7BiasLabel);
  latentVariable7BiasLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable7BiasLabel.setJustificationType(juce::Justification::centred);
  latentVariable7BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);

  addAndMakeVisible(latentVariable8BiasLabel);
  latentVariable8BiasLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable8BiasLabel.setJustificationType(juce::Justification::centred);
  latentVariable8BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);

  startTimer(100); 
  processorRef.addActionListener(this);

  addAndMakeVisible(midiKeyboardComponent);
  midiKeyboardState.addListener(&processorRef.getMidiMessageCollector());

  setSize(920, 520);
}

void RAVE_for_MIDISynthesizer_ProcessorEditor::paint (juce::Graphics& g)
{
  g.fillAll(juce::Colours::greenyellow.darker(0.25));
  ADSRGraph(g);
}

void RAVE_for_MIDISynthesizer_ProcessorEditor::resized()
{

  auto area = getLocalBounds();
  auto componentWidth1 = (area.getWidth() / 2 - 80 ) / 3; 
  auto componentWidth2 = (area.getWidth() / 2  - 100 ) / 4;
  auto componentWidth3 = (area.getWidth() / 2  - 20) / 9;
  auto componentHeight1 = (area.getHeight()-  100)/ 10 ;
  auto componentHeight2 = (area.getHeight()-  200 - (componentHeight1 * 3)) / 9 ;
  auto padding1 = 20; 
  auto padding2 = 10;         

  MixSlider.setBounds(padding1,  padding1 + 15, componentWidth1 ,  componentHeight1 * 3 - 15);
  GainSlider.setBounds(MixSlider.getRight() + padding1, padding1 + 15, componentWidth1 , componentHeight1 * 3 - 15);
  openFileButton.setBounds(GainSlider.getRight() + padding1, padding1 + 15, componentWidth1 , (componentHeight1 * 3 - 15) / 2);

  int graphHeight = 60;
  int graphpadding1 = 10;
  adsrGraphArea = juce::Rectangle<int>(
      AttackSlider.getX(),
      AttackSlider.getBottom() + graphpadding1,
      ReleaseSlider.getRight() - AttackSlider.getX(),
      graphHeight
  );

  adsrGraphArea.setBounds(padding1, MixSlider.getBottom() + padding1, area.getWidth() / 2 - 40, componentHeight1 * 2 );


  AttackSlider.setBounds(padding1, adsrGraphArea.getBottom() + padding1 + 15, componentWidth2 , componentHeight1 * 2);
  DecaySlider.setBounds(AttackSlider.getRight() + padding1, adsrGraphArea.getBottom()+ padding1 + 15, componentWidth2 , componentHeight1 * 2);
  SustainSlider.setBounds(DecaySlider.getRight() + padding1, adsrGraphArea.getBottom()+ padding1 + 15, componentWidth2 , componentHeight1 * 2);
  ReleaseSlider.setBounds(SustainSlider.getRight() + padding1, adsrGraphArea.getBottom()+ padding1 + 15, componentWidth2 , componentHeight1* 2);

  latentVariable1Label.setBounds(ReleaseSlider.getRight() + padding2 , componentHeight2 + (padding1 * 2), componentWidth3 * 2, componentHeight2);
  latentVariable2Label.setBounds(ReleaseSlider.getRight() + padding2 , latentVariable1Label.getBottom() + padding1 , componentWidth3 * 2, componentHeight2);
  latentVariable3Label.setBounds(ReleaseSlider.getRight() + padding2 , latentVariable2Label.getBottom() + padding1 , componentWidth3 * 2, componentHeight2);
  latentVariable4Label.setBounds(ReleaseSlider.getRight() + padding2 , latentVariable3Label.getBottom() + padding1 , componentWidth3 * 2, componentHeight2);
  latentVariable5Label.setBounds(ReleaseSlider.getRight() + padding2 , latentVariable4Label.getBottom() + padding1 , componentWidth3 * 2, componentHeight2);
  latentVariable6Label.setBounds(ReleaseSlider.getRight() + padding2 , latentVariable5Label.getBottom() + padding1 , componentWidth3 * 2, componentHeight2);
  latentVariable7Label.setBounds(ReleaseSlider.getRight() + padding2 , latentVariable6Label.getBottom() + padding1 , componentWidth3 * 2, componentHeight2);
  latentVariable8Label.setBounds(ReleaseSlider.getRight() + padding2 , latentVariable7Label.getBottom() + padding1 , componentWidth3 * 2, componentHeight2);

  latentVariable1Meter.setBounds(latentVariable1Label.getRight() + padding2, latentVariable1Label.getY(), componentWidth3 * 4, componentHeight2);
  latentVariable2Meter.setBounds(latentVariable1Label.getRight() + padding2, latentVariable2Label.getY(), componentWidth3 * 4, componentHeight2);
  latentVariable3Meter.setBounds(latentVariable1Label.getRight() + padding2, latentVariable3Label.getY(), componentWidth3 * 4, componentHeight2);
  latentVariable4Meter.setBounds(latentVariable1Label.getRight() + padding2, latentVariable4Label.getY(), componentWidth3 * 4, componentHeight2);
  latentVariable5Meter.setBounds(latentVariable1Label.getRight() + padding2, latentVariable5Label.getY(), componentWidth3 * 4, componentHeight2);
  latentVariable6Meter.setBounds(latentVariable1Label.getRight() + padding2, latentVariable6Label.getY(), componentWidth3 * 4, componentHeight2);
  latentVariable7Meter.setBounds(latentVariable1Label.getRight() + padding2, latentVariable7Label.getY(), componentWidth3 * 4, componentHeight2);
  latentVariable8Meter.setBounds(latentVariable1Label.getRight() + padding2, latentVariable8Label.getY(), componentWidth3 * 4, componentHeight2);
  
  
  latentVariable1Slider.setBounds(latentVariable1Meter.getRight() , latentVariable1Meter.getY(), componentWidth3 * 2, componentHeight2);
  latentVariable2Slider.setBounds(latentVariable1Meter.getRight() , latentVariable2Meter.getY(), componentWidth3 * 2, componentHeight2);
  latentVariable3Slider.setBounds(latentVariable1Meter.getRight() , latentVariable3Meter.getY(), componentWidth3 * 2, componentHeight2);
  latentVariable4Slider.setBounds(latentVariable1Meter.getRight() , latentVariable4Meter.getY(), componentWidth3 * 2, componentHeight2);
  latentVariable5Slider.setBounds(latentVariable1Meter.getRight() , latentVariable5Meter.getY(), componentWidth3 * 2, componentHeight2);
  latentVariable6Slider.setBounds(latentVariable1Meter.getRight() , latentVariable6Meter.getY(), componentWidth3 * 2, componentHeight2);
  latentVariable7Slider.setBounds(latentVariable1Meter.getRight() , latentVariable7Meter.getY(), componentWidth3 * 2, componentHeight2);
  latentVariable8Slider.setBounds(latentVariable1Meter.getRight() , latentVariable8Meter.getY(), componentWidth3 * 2, componentHeight2);

  latentVariable1BiasLabel.setBounds(latentVariable1Slider.getRight()+ padding2 , latentVariable1Meter.getY(), componentWidth3, componentHeight2);
  latentVariable2BiasLabel.setBounds(latentVariable1Slider.getRight()+ padding2 , latentVariable2Meter.getY(), componentWidth3, componentHeight2);
  latentVariable3BiasLabel.setBounds(latentVariable1Slider.getRight()+ padding2 , latentVariable3Meter.getY(), componentWidth3, componentHeight2);
  latentVariable4BiasLabel.setBounds(latentVariable1Slider.getRight()+ padding2 , latentVariable4Meter.getY(), componentWidth3, componentHeight2);
  latentVariable5BiasLabel.setBounds(latentVariable1Slider.getRight()+ padding2 , latentVariable5Meter.getY(), componentWidth3, componentHeight2);
  latentVariable6BiasLabel.setBounds(latentVariable1Slider.getRight()+ padding2 , latentVariable6Meter.getY(), componentWidth3, componentHeight2);
  latentVariable7BiasLabel.setBounds(latentVariable1Slider.getRight()+ padding2 , latentVariable7Meter.getY(), componentWidth3, componentHeight2);
  latentVariable8BiasLabel.setBounds(latentVariable1Slider.getRight()+ padding2 , latentVariable8Meter.getY(), componentWidth3, componentHeight2);

  
  ScaleLabel.setBounds(latentVariable1Slider.getX() , padding1 + 10, componentWidth3 * 2, componentHeight2);
  BiasLabel.setBounds(latentVariable1BiasLabel.getX() , padding1 + 10, componentWidth3, componentHeight2);
  
  midiKeyboardComponent.setBounds(padding1, AttackSlider.getBottom() + padding1, area.getWidth() - 40, componentHeight1* 3);

  MixLabel.setBounds(MixSlider.getX(), MixSlider.getY() - 15, MixSlider.getWidth(),MixSlider.getTextBoxHeight() );
  GainLabel.setBounds(GainSlider.getX(), GainSlider.getY() - 15, GainSlider.getWidth(),GainSlider.getTextBoxHeight() );
  AttackLabel.setBounds(AttackSlider.getX(), AttackSlider.getY() - 15, AttackSlider.getWidth(),AttackSlider.getTextBoxHeight());
  DecayLabel.setBounds(DecaySlider.getX(), DecaySlider.getY() - 15, DecaySlider.getWidth(), DecaySlider.getTextBoxHeight());
  SustainLabel.setBounds(SustainSlider.getX(), SustainSlider.getY() - 15, SustainSlider.getWidth(), SustainSlider.getTextBoxHeight());
  ReleaseLabel.setBounds(ReleaseSlider.getX(), ReleaseSlider.getY() - 15, ReleaseSlider.getWidth(), ReleaseSlider.getTextBoxHeight());

}

void RAVE_for_MIDISynthesizer_ProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    juce::ignoreUnused(slider);
    repaint();
}

void RAVE_for_MIDISynthesizer_ProcessorEditor::ADSRGraph(juce::Graphics& g)
{
  float attack  = static_cast<float>(AttackSlider.getValue()); 
  float decay   = static_cast<float>(DecaySlider.getValue());
  float sustain = static_cast<float>(SustainSlider.getValue());
  float release = static_cast<float>(ReleaseSlider.getValue());

  auto area = adsrGraphArea;

  float totalTime = attack + decay + release;
  if (totalTime < 1.0f) totalTime = 1.0f; 

  int x0 = area.getX();
  int x1 = x0 + static_cast<int>(area.getWidth() * attack / totalTime);
  int x2 = x1 + static_cast<int>(area.getWidth() * decay / totalTime);
  int x3 = area.getRight();

  int y0 = area.getBottom();
  int y1 = area.getY();
  int y2 = area.getY() + static_cast<int>(area.getHeight() * (1.0f - sustain / 100.0f));
  int y3 = area.getBottom();

  juce::Path adsrPath;
  adsrPath.startNewSubPath(x0, y0); // start point (0,0)
  adsrPath.lineTo(x1, y1);          // Attack
  adsrPath.lineTo(x2, y2);          // Decay→Sustain
  adsrPath.lineTo(x3, y3);          // Release

  g.setColour(juce::Colours::black);
  g.strokePath(adsrPath, juce::PathStrokeType(2.0f));

}



void RAVE_for_MIDISynthesizer_ProcessorEditor::timerCallback()
{
  latentVariable1Meter.setValue(processorRef.getLatentVariables(0));
  latentVariable2Meter.setValue(processorRef.getLatentVariables(1)); 
  latentVariable3Meter.setValue(processorRef.getLatentVariables(2));
  latentVariable3Meter.setValue(processorRef.getLatentVariables(3));
  latentVariable5Meter.setValue(processorRef.getLatentVariables(4));
  latentVariable6Meter.setValue(processorRef.getLatentVariables(5));
  latentVariable7Meter.setValue(processorRef.getLatentVariables(6));
  latentVariable8Meter.setValue(processorRef.getLatentVariables(7));
}

void RAVE_for_MIDISynthesizer_ProcessorEditor::actionListenerCallback(const juce::String& message)
{
  if (message.startsWith("NoteOn"))
  {
    auto TwoMassages  = message.fromFirstOccurrenceOf("n", false, false);
    auto biasStr  = TwoMassages.upToFirstOccurrenceOf(",", false, false);
    auto loopStr  = message.fromFirstOccurrenceOf(",", false, false);

    bias = biasStr.getFloatValue();
    int loopStep  = loopStr.getIntValue();

    std::cout <<"NoteOn: " <<"bias=" << bias << " loopStep=" << loopStep << std::endl;

    if (loopStep == 1) // Example: When only the first bias is reflected in the UI
    {
        latentVariable1BiasLabel.setColour(juce::Label::backgroundColourId, juce::Colours::white);
        latentVariable1BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);
        if (previousBias != nullptr)
          updateBiasLabel(*previousBias); 
        previousBias = &latentVariable1BiasLabel;
    }else if(loopStep == 2)
    {
        latentVariable2BiasLabel.setColour(juce::Label::backgroundColourId, juce::Colours::white); 
        latentVariable2BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);
        if (previousBias != nullptr)
          updateBiasLabel(*previousBias); 
        previousBias = &latentVariable2BiasLabel;
    }else if(loopStep == 3)
    {
        latentVariable3BiasLabel.setColour(juce::Label::backgroundColourId, juce::Colours::white);
        latentVariable3BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);
        if (previousBias != nullptr)
          updateBiasLabel(*previousBias); 
        previousBias = &latentVariable3BiasLabel;
    }else if(loopStep == 4)
    {
        latentVariable4BiasLabel.setColour(juce::Label::backgroundColourId, juce::Colours::white);
        latentVariable4BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);
        if (previousBias != nullptr)
          updateBiasLabel(*previousBias); 
        previousBias = &latentVariable4BiasLabel;        
    }else if(loopStep == 5)
    {
        latentVariable5BiasLabel.setColour(juce::Label::backgroundColourId, juce::Colours::white);
        latentVariable5BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);
        if (previousBias != nullptr)
          updateBiasLabel(*previousBias); 
        previousBias = &latentVariable5BiasLabel;       
    }else if(loopStep == 6)
    {
        latentVariable6BiasLabel.setColour(juce::Label::backgroundColourId, juce::Colours::white);
        latentVariable6BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);
        if (previousBias != nullptr)
          updateBiasLabel(*previousBias); 
        previousBias = &latentVariable6BiasLabel;
    }else if(loopStep == 7)
    {
        latentVariable7BiasLabel.setColour(juce::Label::backgroundColourId, juce::Colours::white);
        latentVariable7BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);
        if (previousBias != nullptr)
          updateBiasLabel(*previousBias); 
        previousBias = &latentVariable7BiasLabel;      
    }else if(loopStep == 8)
    {
        latentVariable8BiasLabel.setColour(juce::Label::backgroundColourId, juce::Colours::white);
        latentVariable8BiasLabel.setText(juce::String::formatted("%.2f", bias), juce::dontSendNotification);
        if (previousBias != nullptr)
          updateBiasLabel(*previousBias); 
        previousBias = &latentVariable8BiasLabel;            
    }
  }else if(message.startsWith("RootNoteOff")){
    //previousBias = nullptr;  
    std::cout <<"RootNoteOff" << std::endl;
  }
}

void RAVE_for_MIDISynthesizer_ProcessorEditor::updateBiasLabel(juce::Label& label)
{
    label.setColour(juce::Label::backgroundColourId, juce::Colours::white.withAlpha(0.f));
}

void RAVE_for_MIDISynthesizer_ProcessorEditor::openFileButtonClicked()
{
    fileChooser = std::make_unique<juce::FileChooser> (
        "Select a file to load...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*.ts"
    );

    auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(flags, [this](const juce::FileChooser& fc)
    {
        auto file = fc.getResult();
        if (file.existsAsFile())
        {
            //(AudioProcessorValueTreeState) itself holds a ValueTree named state internally.
            valueTreeState.state.setProperty("lastFilePath", file.getFullPathName(), nullptr);
        }
    });
}