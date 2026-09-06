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

  OscMixSliderAttachment.reset (new SliderAttachment (valueTreeState, "oscMix", OscMixSlider));
  OscMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  OscMixSlider.setTextValueSuffix (" %");     
  OscMixSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, OscMixSlider.getTextBoxWidth(), OscMixSlider.getTextBoxHeight());
  addAndMakeVisible(OscMixSlider);
  OscMixSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  OscMixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  OscMixSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  OscMixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  OscMixSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  OscMixLabel.setText ("oscMix", juce::dontSendNotification);
  OscMixLabel.setJustificationType(juce::Justification::centred);
  OscMixLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(OscMixLabel);

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

  addAndMakeVisible(LatentVariableLabel);
  LatentVariableLabel.setText ("LatentVariable", juce::dontSendNotification);
  LatentVariableLabel.setJustificationType(juce::Justification::left);
  LatentVariableLabel.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(ScaleLabel);
  ScaleLabel.setText ("Scale Factor", juce::dontSendNotification);
  ScaleLabel.setJustificationType(juce::Justification::centred);
  ScaleLabel.setColour(juce::Label::textColourId, juce::Colours::black);

  addAndMakeVisible(BiasLabel);
  BiasLabel.setText ("Bias", juce::dontSendNotification);
  BiasLabel.setJustificationType(juce::Justification::centred);
  BiasLabel.setColour(juce::Label::textColourId, juce::Colours::black);

  // --- Dimension 1 ---
  addAndMakeVisible(latentVariable1Meter);
  latentVariable1Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable1Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable1Label);
  latentVariable1Label.setText ("1", juce::dontSendNotification);
  latentVariable1Label.setJustificationType(juce::Justification::centred);
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

  addAndMakeVisible(latentVariable1BiasSlider);
  latentVariable1BiasSliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable1Bias", latentVariable1BiasSlider));
  latentVariable1BiasSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable1BiasSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable1BiasSlider.getTextBoxWidth() - 30, latentVariable1BiasSlider.getTextBoxHeight());  
  latentVariable1BiasSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable1BiasSlider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable1BiasSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable1BiasSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable1BiasSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  // --- Dimension 2 ---
  addAndMakeVisible(latentVariable2Meter);
  latentVariable2Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable2Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable2Label);
  latentVariable2Label.setText ("2", juce::dontSendNotification);
  latentVariable2Label.setJustificationType(juce::Justification::centred);
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

  addAndMakeVisible(latentVariable2BiasSlider);
  latentVariable2BiasSliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable2Bias", latentVariable2BiasSlider));
  latentVariable2BiasSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable2BiasSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable2BiasSlider.getTextBoxWidth() - 30, latentVariable2BiasSlider.getTextBoxHeight());  
  latentVariable2BiasSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable2BiasSlider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable2BiasSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable2BiasSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable2BiasSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  // --- Dimension 3 ---
  addAndMakeVisible(latentVariable3Meter);
  latentVariable3Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable3Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable3Label);
  latentVariable3Label.setText ("3", juce::dontSendNotification);
  latentVariable3Label.setJustificationType(juce::Justification::centred);
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

  addAndMakeVisible(latentVariable3BiasSlider);
  latentVariable3BiasSliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable3Bias", latentVariable3BiasSlider));
  latentVariable3BiasSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable3BiasSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable3BiasSlider.getTextBoxWidth() - 30, latentVariable3BiasSlider.getTextBoxHeight());  
  latentVariable3BiasSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable3BiasSlider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable3BiasSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable3BiasSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable3BiasSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  // --- Dimension 4 ---
  addAndMakeVisible(latentVariable4Meter);
  latentVariable4Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable4Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable4Label);
  latentVariable4Label.setText ("4", juce::dontSendNotification);
  latentVariable4Label.setJustificationType(juce::Justification::centred);
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

  addAndMakeVisible(latentVariable4BiasSlider);
  latentVariable4BiasSliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable4Bias", latentVariable4BiasSlider));
  latentVariable4BiasSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable4BiasSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable4BiasSlider.getTextBoxWidth() - 30, latentVariable4BiasSlider.getTextBoxHeight());  
  latentVariable4BiasSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable4BiasSlider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable4BiasSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable4BiasSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable4BiasSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  // --- Dimension 5 ---
  addAndMakeVisible(latentVariable5Meter);
  latentVariable5Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable5Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable5Label);
  latentVariable5Label.setText ("5", juce::dontSendNotification);
  latentVariable5Label.setJustificationType(juce::Justification::centred);
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

  addAndMakeVisible(latentVariable5BiasSlider);
  latentVariable5BiasSliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable5Bias", latentVariable5BiasSlider));
  latentVariable5BiasSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable5BiasSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable5BiasSlider.getTextBoxWidth() - 30, latentVariable5BiasSlider.getTextBoxHeight());  
  latentVariable5BiasSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable5BiasSlider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable5BiasSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable5BiasSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable5BiasSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  // --- Dimension 6 ---
  addAndMakeVisible(latentVariable6Meter);
  latentVariable6Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable6Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));  
  addAndMakeVisible(latentVariable6Label);
  latentVariable6Label.setText ("6", juce::dontSendNotification);
  latentVariable6Label.setJustificationType(juce::Justification::centred);
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

  addAndMakeVisible(latentVariable6BiasSlider);
  latentVariable6BiasSliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable6Bias", latentVariable6BiasSlider));
  latentVariable6BiasSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable6BiasSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable6BiasSlider.getTextBoxWidth() - 30, latentVariable6BiasSlider.getTextBoxHeight());  
  latentVariable6BiasSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable6BiasSlider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable6BiasSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable6BiasSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable6BiasSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  // --- Dimension 7 ---
  addAndMakeVisible(latentVariable7Meter);
  latentVariable7Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable7Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));  
  addAndMakeVisible(latentVariable7Label);
  latentVariable7Label.setText ("7", juce::dontSendNotification);
  latentVariable7Label.setJustificationType(juce::Justification::centred);
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

  addAndMakeVisible(latentVariable7BiasSlider);
  latentVariable7BiasSliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable7Bias", latentVariable7BiasSlider));
  latentVariable7BiasSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable7BiasSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable7BiasSlider.getTextBoxWidth() - 30, latentVariable7BiasSlider.getTextBoxHeight());  
  latentVariable7BiasSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable7BiasSlider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable7BiasSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable7BiasSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable7BiasSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  // --- Dimension 8 ---
  addAndMakeVisible(latentVariable8Meter);
  latentVariable8Meter.setColour(juce::Label::textColourId, juce::Colours::black);
  latentVariable8Meter.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow.darker(0.25));
  addAndMakeVisible(latentVariable8Label);
  latentVariable8Label.setText ("8", juce::dontSendNotification);  
  latentVariable8Label.setJustificationType(juce::Justification::centred);
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

  addAndMakeVisible(latentVariable8BiasSlider);
  latentVariable8BiasSliderAttachment.reset (new SliderAttachment (valueTreeState, "latentVariable8Bias", latentVariable8BiasSlider));
  latentVariable8BiasSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  latentVariable8BiasSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, latentVariable8BiasSlider.getTextBoxWidth() - 30, latentVariable8BiasSlider.getTextBoxHeight());  
  latentVariable8BiasSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::white);
  latentVariable8BiasSlider.setColour(juce::Slider::trackColourId, juce::Colours::greenyellow.darker(0.25).withAlpha(0.75f));
  latentVariable8BiasSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.darker(0.1f));
  latentVariable8BiasSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  latentVariable8BiasSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow.darker(0.25));

  startTimer(100); 

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
  auto padding1 = 20; 
  auto padding2 = 10;         
  auto leftTopWidth = area.getWidth() / 2 - 40;
  auto componentWidth1 = (leftTopWidth - (padding1 * 3)) / 4; 
  auto componentWidth2 = (area.getWidth() / 2  - 100 ) / 4;
  auto componentHeight1 = (area.getHeight()-  100)/ 10 ;
  auto componentHeight2 = (area.getHeight()-  200 - (componentHeight1 * 3)) / 9 ;

  MixSlider.setBounds(padding1,  padding1 + 15, componentWidth1 ,  componentHeight1 * 3 - 15);
  OscMixSlider.setBounds(MixSlider.getRight() + padding1, padding1 + 15, componentWidth1 , componentHeight1 * 3 - 15);
  GainSlider.setBounds(OscMixSlider.getRight() + padding1, padding1 + 15, componentWidth1 , componentHeight1 * 3 - 15);
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

  int rightX = ReleaseSlider.getRight() + padding2;
  int rightWidth = area.getWidth() - rightX - padding1;

  int numWidth = 20;
  int gap = 6;
  int availWidth = rightWidth - numWidth - (gap * 3);

  int meterWidth = static_cast<int>(availWidth * 0.34);
  int scaleWidth = static_cast<int>(availWidth * 0.33);
  int biasWidth = static_cast<int>(availWidth * 0.33);

  int numX = rightX;
  int meterX = numX + numWidth + gap;
  int scaleX = meterX + meterWidth + gap;
  int biasX = scaleX + scaleWidth + gap;

  LatentVariableLabel.setBounds(numX, padding1 + 10, numWidth + meterWidth + gap, componentHeight2);
  ScaleLabel.setBounds(scaleX, padding1 + 10, scaleWidth, componentHeight2);
  BiasLabel.setBounds(biasX, padding1 + 10, biasWidth, componentHeight2);

  latentVariable1Label.setBounds(numX, componentHeight2 + (padding1 * 2), numWidth, componentHeight2);
  latentVariable2Label.setBounds(numX, latentVariable1Label.getBottom() + padding1, numWidth, componentHeight2);
  latentVariable3Label.setBounds(numX, latentVariable2Label.getBottom() + padding1, numWidth, componentHeight2);
  latentVariable4Label.setBounds(numX, latentVariable3Label.getBottom() + padding1, numWidth, componentHeight2);
  latentVariable5Label.setBounds(numX, latentVariable4Label.getBottom() + padding1, numWidth, componentHeight2);
  latentVariable6Label.setBounds(numX, latentVariable5Label.getBottom() + padding1, numWidth, componentHeight2);
  latentVariable7Label.setBounds(numX, latentVariable6Label.getBottom() + padding1, numWidth, componentHeight2);
  latentVariable8Label.setBounds(numX, latentVariable7Label.getBottom() + padding1, numWidth, componentHeight2);

  latentVariable1Meter.setBounds(meterX, latentVariable1Label.getY(), meterWidth, componentHeight2);
  latentVariable2Meter.setBounds(meterX, latentVariable2Label.getY(), meterWidth, componentHeight2);
  latentVariable3Meter.setBounds(meterX, latentVariable3Label.getY(), meterWidth, componentHeight2);
  latentVariable4Meter.setBounds(meterX, latentVariable4Label.getY(), meterWidth, componentHeight2);
  latentVariable5Meter.setBounds(meterX, latentVariable5Label.getY(), meterWidth, componentHeight2);
  latentVariable6Meter.setBounds(meterX, latentVariable6Label.getY(), meterWidth, componentHeight2);
  latentVariable7Meter.setBounds(meterX, latentVariable7Label.getY(), meterWidth, componentHeight2);
  latentVariable8Meter.setBounds(meterX, latentVariable8Label.getY(), meterWidth, componentHeight2);

  latentVariable1Slider.setBounds(scaleX, latentVariable1Label.getY(), scaleWidth, componentHeight2);
  latentVariable2Slider.setBounds(scaleX, latentVariable2Label.getY(), scaleWidth, componentHeight2);
  latentVariable3Slider.setBounds(scaleX, latentVariable3Label.getY(), scaleWidth, componentHeight2);
  latentVariable4Slider.setBounds(scaleX, latentVariable4Label.getY(), scaleWidth, componentHeight2);
  latentVariable5Slider.setBounds(scaleX, latentVariable5Label.getY(), scaleWidth, componentHeight2);
  latentVariable6Slider.setBounds(scaleX, latentVariable6Label.getY(), scaleWidth, componentHeight2);
  latentVariable7Slider.setBounds(scaleX, latentVariable7Label.getY(), scaleWidth, componentHeight2);
  latentVariable8Slider.setBounds(scaleX, latentVariable8Label.getY(), scaleWidth, componentHeight2);

  latentVariable1BiasSlider.setBounds(biasX, latentVariable1Label.getY(), biasWidth, componentHeight2);
  latentVariable2BiasSlider.setBounds(biasX, latentVariable2Label.getY(), biasWidth, componentHeight2);
  latentVariable3BiasSlider.setBounds(biasX, latentVariable3Label.getY(), biasWidth, componentHeight2);
  latentVariable4BiasSlider.setBounds(biasX, latentVariable4Label.getY(), biasWidth, componentHeight2);
  latentVariable5BiasSlider.setBounds(biasX, latentVariable5Label.getY(), biasWidth, componentHeight2);
  latentVariable6BiasSlider.setBounds(biasX, latentVariable6Label.getY(), biasWidth, componentHeight2);
  latentVariable7BiasSlider.setBounds(biasX, latentVariable7Label.getY(), biasWidth, componentHeight2);
  latentVariable8BiasSlider.setBounds(biasX, latentVariable8Label.getY(), biasWidth, componentHeight2);

  
  midiKeyboardComponent.setBounds(padding1, AttackSlider.getBottom() + padding1, area.getWidth() - 40, componentHeight1* 3);

  MixLabel.setBounds(MixSlider.getX(), MixSlider.getY() - 15, MixSlider.getWidth(),MixSlider.getTextBoxHeight() );
  OscMixLabel.setBounds(OscMixSlider.getX(), OscMixSlider.getY() - 15, OscMixSlider.getWidth(),OscMixSlider.getTextBoxHeight() );
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
  latentVariable4Meter.setValue(processorRef.getLatentVariables(3));
  latentVariable5Meter.setValue(processorRef.getLatentVariables(4));
  latentVariable6Meter.setValue(processorRef.getLatentVariables(5));
  latentVariable7Meter.setValue(processorRef.getLatentVariables(6));
  latentVariable8Meter.setValue(processorRef.getLatentVariables(7));
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