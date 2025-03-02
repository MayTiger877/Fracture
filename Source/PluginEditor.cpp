/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FractureAudioProcessorEditor::FractureAudioProcessorEditor (FractureAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (600, 400);
    
    m_dryWetKnobListener = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRYWET", m_dryWetKnob);
    m_delayTimeKnobListener = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAYTIME", m_delayTimeKnob);
    m_feedbackKnobListener = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FEEDBACK", m_feedbackKnob);
    m_stereoKnobListener = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "STEREO", m_feedbackKnob);

    initializeKnobs();
}

void FractureAudioProcessorEditor::initializeKnobs()
{
	m_dryWetKnob.setSliderStyle(Slider::Rotary);
	m_dryWetKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
	m_dryWetKnob.setRange(0.0f, 100.0f, 0.5f);
	m_dryWetKnob.setValue(50.0f);
	m_dryWetKnob.setColour(Slider::rotarySliderFillColourId, Colours::white);
    m_dryWetKnob.setBounds(5, 50, 100, 100);
    addAndMakeVisible(m_dryWetKnob);
	m_dryWetLabel.setBounds(10, 110, 100, 100);
	m_dryWetLabel.setText("Dry/Wet", dontSendNotification);
	addAndMakeVisible(m_dryWetLabel);

    m_delayTimeKnob.setSliderStyle(Slider::Rotary);
    m_delayTimeKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    m_delayTimeKnob.setRange(0.5f, 500.0f, 0.5f);
    m_delayTimeKnob.setValue(50.0f);
    m_delayTimeKnob.setColour(Slider::rotarySliderFillColourId, Colours::white);
    m_delayTimeKnob.setBounds(105, 100, 100, 100);
    addAndMakeVisible(m_delayTimeKnob);
	m_delayTimeLabel.setBounds(110, 160, 100, 100);
	m_delayTimeLabel.setText("Delay Time", dontSendNotification);
	addAndMakeVisible(m_delayTimeLabel);

    m_feedbackKnob.setSliderStyle(Slider::Rotary);
    m_feedbackKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    m_feedbackKnob.setRange(0.0f, 1.0f, 0.05f);
    m_feedbackKnob.setValue(0.2f);
    m_feedbackKnob.setColour(Slider::rotarySliderFillColourId, Colours::white);
    m_feedbackKnob.setBounds(5, 200, 100, 100);
    addAndMakeVisible(m_feedbackKnob);
	m_feedbackLabel.setBounds(10, 260, 100, 100);
	m_feedbackLabel.setText("Feedback", dontSendNotification);
	addAndMakeVisible(m_feedbackLabel);

    m_stereoKnob.setSliderStyle(Slider::Rotary);
    m_stereoKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    m_stereoKnob.setRange(0.0f, 400.0f, 0.5f);
    m_stereoKnob.setValue(0.0f);
    m_stereoKnob.setColour(Slider::rotarySliderFillColourId, Colours::white);
    m_stereoKnob.setBounds(105, 250, 100, 100);
    addAndMakeVisible(m_stereoKnob);
	m_stereoLabel.setBounds(110, 310, 100, 100);
	m_stereoLabel.setText("Stereo", dontSendNotification);
	addAndMakeVisible(m_stereoLabel);
}

FractureAudioProcessorEditor::~FractureAudioProcessorEditor()
{
}

//==============================================================================
void FractureAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    
    for (int i = 0; i < 6; i++)
    {
        objects[i].setPosition(300, 300);
        objects[i].draw(g, 100);
    }

}

void FractureAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
