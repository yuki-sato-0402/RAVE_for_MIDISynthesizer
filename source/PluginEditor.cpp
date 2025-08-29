/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#include "PluginEditor.h"
#include "PluginProcessor.h"

RAVE_for_MIDISynthesiser_ProcessorEditor::RAVE_for_MIDISynthesiser_ProcessorEditor(RAVE_for_MIDISynthesiser_Processor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p),processorRef (p),  valueTreeState(vts)
{

  juce::ignoreUnused (processorRef);
  
  MixSliderAttachment.reset (new SliderAttachment (valueTreeState, "dryWetRange", MixSlider));
  MixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  MixSlider.setTextValueSuffix (" %");     
  MixSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, MixSlider.getTextBoxWidth(), MixSlider.getTextBoxHeight());
  addAndMakeVisible(MixSlider);
  MixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
  MixSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  MixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.withAlpha(0.75f));
  MixSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.brighter(1.5));
  MixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  MixSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow);

 
  MixLabel.setText ("dryWetRange", juce::dontSendNotification);
  MixLabel.setJustificationType(juce::Justification::centred);
  MixLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(MixLabel);

  GainSliderAttachment.reset (new SliderAttachment (valueTreeState, "outputGain", GainSlider));
  GainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  GainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, GainSlider.getTextBoxWidth(), GainSlider.getTextBoxHeight());
  addAndMakeVisible(GainSlider);
  GainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
  GainSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  GainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.withAlpha(0.75f));
  GainSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.brighter(1.5));
  GainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  GainSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow);
 
  GainLabel.setText ("outputGain", juce::dontSendNotification);
  GainLabel.setJustificationType(juce::Justification::centred);
  GainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(GainLabel);


  //juce::File modelDir(RAVE_MODEL_DIR);
  
  //juce::Array<juce::File> files = modelDir.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.ts");
  //if(files.size() == 0) {
  //    std::cout << "No model files found in directory: " << modelDir.getFullPathName() << std::endl;
  //} else {
  //    std::cout << "Model files found in directory: " << modelDir.getFullPathName() << std::endl;
  //    for (auto& file : files)
  //    {
  //        modelFileNames.add(file.getFileName().toStdString());

  //    }
  //    modelComboBox.addItemList(modelFileNames, 1);
  //    //modelComboBox.setSelectedId(1);
  //}
  //comboBoxAttachment.reset (new ComboBoxAttachment (valueTreeState, "modelSelection", modelComboBox));

  //addAndMakeVisible(modelComboBox);
  //modelComboBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::black);
  //modelComboBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
  //modelComboBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::greenyellow);
  //modelComboBox.setColour(juce::ComboBox::arrowColourId, juce::Colours::greenyellow);
  //modelComboBox.setJustificationType(juce::Justification::centred);
  
  //modelLabel.setText ("RAVE Model", juce::dontSendNotification);
  //modelLabel.setJustificationType(juce::Justification::centred);
  //modelLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  //addAndMakeVisible(modelLabel);

  AttackSliderAttachment.reset (new SliderAttachment (valueTreeState, "attackTime", AttackSlider));
  AttackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  AttackSlider.setTextValueSuffix (" ms");     
  AttackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, AttackSlider.getTextBoxWidth(), AttackSlider.getTextBoxHeight());
  addAndMakeVisible(AttackSlider);
  AttackSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
  AttackSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  AttackSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.withAlpha(0.75f));
  AttackSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.brighter(1.5));
  AttackSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  AttackSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow);
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
  DecaySlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
  DecaySlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  DecaySlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.withAlpha(0.75f));
  DecaySlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.brighter(1.5));
  DecaySlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  DecaySlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow);
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
  SustainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
  SustainSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  SustainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.withAlpha(0.75f));
  SustainSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.brighter(1.5));
  SustainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  SustainSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow);
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
  ReleaseSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
  ReleaseSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
  ReleaseSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::greenyellow.withAlpha(0.75f));
  ReleaseSlider.setColour(juce::Slider::thumbColourId , juce::Colours::greenyellow.brighter(1.5));
  ReleaseSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  ReleaseSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::greenyellow); 
  ReleaseSlider.addListener(this);

  ReleaseLabel.setText ("releaseTime", juce::dontSendNotification);
  ReleaseLabel.setJustificationType(juce::Justification::centred);
  ReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::black);
  addAndMakeVisible(ReleaseLabel);

  addAndMakeVisible(midiKeyboardComponent);
  midiKeyboardState.addListener(&processorRef.getMidiMessageCollector());

  setSize(620, 520);
}

void RAVE_for_MIDISynthesiser_ProcessorEditor::paint (juce::Graphics& g)
{
  g.fillAll(juce::Colours::greenyellow);

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

void RAVE_for_MIDISynthesiser_ProcessorEditor::resized()
{

  auto area = getLocalBounds();
  auto componentWidth1 = (area.getWidth() - 80 ) / 3; 
  auto componentWidth2 = (area.getWidth() - 100 ) / 4;
  auto componentHeight = (area.getHeight()-  100)/ 10 ;
  auto padding = 20;          

  MixSlider.setBounds(padding,  padding + 15, componentWidth1 ,  componentHeight * 3 - 15);
  GainSlider.setBounds(MixSlider.getRight() + padding, padding + 15, componentWidth1 , componentHeight * 3 - 15);
  //modelComboBox.setBounds(GainSlider.getRight() + padding, padding + 40, componentWidth1 , componentHeight );
  

  int graphHeight = 60;
  int graphPadding = 10;
  adsrGraphArea = juce::Rectangle<int>(
      AttackSlider.getX(),
      AttackSlider.getBottom() + graphPadding,
      ReleaseSlider.getRight() - AttackSlider.getX(),
      graphHeight
  );

  adsrGraphArea.setBounds(padding, MixSlider.getBottom() + padding, area.getWidth() - 40, componentHeight * 2 );


  AttackSlider.setBounds(padding, adsrGraphArea.getBottom() + padding + 15, componentWidth2 , componentHeight * 2);
  DecaySlider.setBounds(AttackSlider.getRight() + padding, adsrGraphArea.getBottom()+ padding + 15, componentWidth2 , componentHeight * 2);
  SustainSlider.setBounds(DecaySlider.getRight() + padding, adsrGraphArea.getBottom()+ padding + 15, componentWidth2 , componentHeight * 2);
  ReleaseSlider.setBounds(SustainSlider.getRight() + padding, adsrGraphArea.getBottom()+ padding + 15, componentWidth2 , componentHeight* 2); 
  
  midiKeyboardComponent.setBounds(padding, AttackSlider.getBottom() + padding, area.getWidth() - 40, componentHeight* 3);

  MixLabel.setBounds(MixSlider.getX(), MixSlider.getY() - 15, MixSlider.getWidth(),MixSlider.getTextBoxHeight() );
  GainLabel.setBounds(GainSlider.getX(), GainSlider.getY() - 15, GainSlider.getWidth(),GainSlider.getTextBoxHeight() );
  //modelLabel.setBounds(modelComboBox.getX(), modelComboBox.getY() - 35, modelComboBox.getWidth(), modelComboBox.getHeight());
  AttackLabel.setBounds(AttackSlider.getX(), AttackSlider.getY() - 15, AttackSlider.getWidth(),AttackSlider.getTextBoxHeight());
  DecayLabel.setBounds(DecaySlider.getX(), DecaySlider.getY() - 15, DecaySlider.getWidth(), DecaySlider.getTextBoxHeight());
  SustainLabel.setBounds(SustainSlider.getX(), SustainSlider.getY() - 15, SustainSlider.getWidth(), SustainSlider.getTextBoxHeight());
  ReleaseLabel.setBounds(ReleaseSlider.getX(), ReleaseSlider.getY() - 15, ReleaseSlider.getWidth(), ReleaseSlider.getTextBoxHeight());
}

void RAVE_for_MIDISynthesiser_ProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    juce::ignoreUnused(slider);
    repaint();
}