#ifndef NN_INFERENCE_TEMPLATE_PLUGINPARAMETERS_H
#define NN_INFERENCE_TEMPLATE_PLUGINPARAMETERS_H
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RaveModelConfig.h"
#include "HorizontalMeter.h"

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

class RAVE_for_MIDISynthesizer_ProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener, private juce::Timer, public juce::ActionListener
{
public:
  RAVE_for_MIDISynthesizer_ProcessorEditor(RAVE_for_MIDISynthesizer_Processor& p, juce::AudioProcessorValueTreeState& apvts);
  ~RAVE_for_MIDISynthesizer_ProcessorEditor() override = default;

  //==============================================================================
  void paint (juce::Graphics& g) override;
  void resized() override; 
  void sliderValueChanged(juce::Slider* slider) override;
  void ADSRGraph(juce::Graphics& g);
  void timerCallback() override;
  void actionListenerCallback(const juce::String& message) override;
  void openFileButtonClicked();
  typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
  typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

  std::string selectedFileName;

private:
  RAVE_for_MIDISynthesizer_Processor& processorRef;
  juce::AudioProcessorValueTreeState& valueTreeState;
  juce::Slider MixSlider;
  juce::Slider GainSlider;
  juce::Slider  AttackSlider;
  juce::Slider  DecaySlider;
  juce::Slider  SustainSlider;
  juce::Slider  ReleaseSlider;
  juce::Slider  latentVariable1Slider;
  juce::Slider  latentVariable2Slider;
  juce::Slider  latentVariable3Slider;
  juce::Slider  latentVariable4Slider;
  juce::Slider  latentVariable5Slider;
  juce::Slider  latentVariable6Slider;
  juce::Slider  latentVariable7Slider;
  juce::Slider  latentVariable8Slider;

  juce::TextButton openFileButton{ "Open Model File" };
  
  juce::Label  MixLabel;
  juce::Label  GainLabel;
  juce::Label  AttackLabel;
  juce::Label  DecayLabel;
  juce::Label  SustainLabel;
  juce::Label  ReleaseLabel;
  
  juce::Label  ScaleLabel;
  juce::Label  latentVariable1Label;
  juce::Label  latentVariable2Label;
  juce::Label  latentVariable3Label;
  juce::Label  latentVariable4Label;
  juce::Label  latentVariable5Label;   
  juce::Label  latentVariable6Label;
  juce::Label  latentVariable7Label;
  juce::Label  latentVariable8Label;

  juce::Label  BiasLabel;
  juce::Label  latentVariable1BiasLabel;
  juce::Label  latentVariable2BiasLabel;
  juce::Label  latentVariable3BiasLabel;
  juce::Label  latentVariable4BiasLabel;
  juce::Label  latentVariable5BiasLabel;
  juce::Label  latentVariable6BiasLabel;
  juce::Label  latentVariable7BiasLabel;
  juce::Label  latentVariable8BiasLabel;
  void updateBiasLabel(juce::Label& label);
  float bias = 0;
  juce::Label* previousBias = nullptr;  

  std::unique_ptr<SliderAttachment> MixSliderAttachment;
  std::unique_ptr<SliderAttachment> GainSliderAttachment;
  std::unique_ptr<SliderAttachment> AttackSliderAttachment;
  std::unique_ptr<SliderAttachment> DecaySliderAttachment;
  std::unique_ptr<SliderAttachment> SustainSliderAttachment;
  std::unique_ptr<SliderAttachment> ReleaseSliderAttachment;
  std::unique_ptr<SliderAttachment> latentVariable1SliderAttachment;
  std::unique_ptr<SliderAttachment> latentVariable2SliderAttachment;
  std::unique_ptr<SliderAttachment> latentVariable3SliderAttachment;
  std::unique_ptr<SliderAttachment> latentVariable4SliderAttachment;
  std::unique_ptr<SliderAttachment> latentVariable5SliderAttachment;
  std::unique_ptr<SliderAttachment> latentVariable6SliderAttachment;
  std::unique_ptr<SliderAttachment> latentVariable7SliderAttachment;
  std::unique_ptr<SliderAttachment> latentVariable8SliderAttachment;

  //std::unique_ptr<ComboBoxAttachment> comboBoxAttachment;

  juce::MidiKeyboardState midiKeyboardState;
  juce::MidiKeyboardComponent midiKeyboardComponent { midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard };

  juce::Rectangle<int> adsrGraphArea;

  HorizontalMeter latentVariable1Meter, latentVariable2Meter, latentVariable3Meter, latentVariable4Meter,
      latentVariable5Meter, latentVariable6Meter, latentVariable7Meter, latentVariable8Meter;

  std::unique_ptr<juce::FileChooser> fileChooser;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RAVE_for_MIDISynthesizer_ProcessorEditor)
};
#endif //NN_INFERENCE_TEMPLATE_PLUGINPARAMETERS_H
