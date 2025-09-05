#ifndef NN_INFERENCE_TEMPLATE_PLUGINPARAMETERS_H
#define NN_INFERENCE_TEMPLATE_PLUGINPARAMETERS_H
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RaveModelConfigDecoder.h"
#include "RaveModelConfigEncoder.h"

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

class RAVE_for_MIDISynthesiser_ProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
  RAVE_for_MIDISynthesiser_ProcessorEditor(RAVE_for_MIDISynthesiser_Processor& p, juce::AudioProcessorValueTreeState& vts);
  ~RAVE_for_MIDISynthesiser_ProcessorEditor() override = default;

  //==============================================================================
  void paint (juce::Graphics& g) override;
  void resized() override; 
  void sliderValueChanged(juce::Slider* slider) override;
  void ADSRGraph(juce::Graphics& g);
  typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
  typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

  std::string selectedFileName;

private:
  RAVE_for_MIDISynthesiser_Processor& processorRef;
  juce::AudioProcessorValueTreeState& valueTreeState;
  juce::Slider MixSlider;
  juce::Slider GainSlider;
  juce::Slider  AttackSlider;
  juce::Slider  DecaySlider;
  juce::Slider  SustainSlider;
  juce::Slider  ReleaseSlider;
  
  //juce::ComboBox modelComboBox;
  //juce::StringArray modelFileNames;
  
  juce::Label  MixLabel;
  juce::Label  GainLabel;
  juce::Label  AttackLabel;
  juce::Label  DecayLabel;
  juce::Label  SustainLabel;
  juce::Label  ReleaseLabel;
  //juce::Label  modelLabel;

  std::unique_ptr<SliderAttachment> MixSliderAttachment;
  std::unique_ptr<SliderAttachment> GainSliderAttachment;
  std::unique_ptr<SliderAttachment> AttackSliderAttachment;
  std::unique_ptr<SliderAttachment> DecaySliderAttachment;
  std::unique_ptr<SliderAttachment> SustainSliderAttachment;
  std::unique_ptr<SliderAttachment> ReleaseSliderAttachment;

  //std::unique_ptr<ComboBoxAttachment> comboBoxAttachment;

  juce::MidiKeyboardState midiKeyboardState;
  juce::MidiKeyboardComponent midiKeyboardComponent { midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard };

  juce::Rectangle<int> adsrGraphArea;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RAVE_for_MIDISynthesiser_ProcessorEditor)
};
#endif //NN_INFERENCE_TEMPLATE_PLUGINPARAMETERS_H
