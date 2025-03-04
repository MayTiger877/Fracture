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
	m_shakeKnobListener = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SHAKE", m_shakeKnob);

    initializeKnobs();

	startTimer(60);
}

void FractureAudioProcessorEditor::initializeKnobs()
{
	m_dryWetKnob.setSliderStyle(Slider::Rotary);
	m_dryWetKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
	m_dryWetKnob.setRange(0.0f, 100.0f, 0.5f);
	m_dryWetKnob.setValue(50.0f);
	m_dryWetKnob.setColour(Slider::rotarySliderFillColourId, Colours::white);
    m_dryWetKnob.setBounds(5, 50 - 40, 100, 100);
    addAndMakeVisible(m_dryWetKnob);
	m_dryWetLabel.setBounds(10, 110 - 40, 100, 100);
	m_dryWetLabel.setText("Dry/Wet", dontSendNotification);
	addAndMakeVisible(m_dryWetLabel);

    m_delayTimeKnob.setSliderStyle(Slider::Rotary);
    m_delayTimeKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    m_delayTimeKnob.setRange(0.5f, 500.0f, 0.5f);
    m_delayTimeKnob.setValue(50.0f);
    m_delayTimeKnob.setColour(Slider::rotarySliderFillColourId, Colours::white);
    m_delayTimeKnob.setBounds(105, 100 - 40, 100, 100);
    addAndMakeVisible(m_delayTimeKnob);
	m_delayTimeLabel.setBounds(110, 160 - 40, 100, 100);
	m_delayTimeLabel.setText("Delay Time", dontSendNotification);
	addAndMakeVisible(m_delayTimeLabel);

    m_feedbackKnob.setSliderStyle(Slider::Rotary);
    m_feedbackKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    m_feedbackKnob.setRange(0.0f, 1.0f, 0.05f);
    m_feedbackKnob.setValue(0.2f);
    m_feedbackKnob.setColour(Slider::rotarySliderFillColourId, Colours::white);
    m_feedbackKnob.setBounds(5, 170 - 40, 100, 100);
    addAndMakeVisible(m_feedbackKnob);
	m_feedbackLabel.setBounds(10, 230 - 40, 100, 100);
	m_feedbackLabel.setText("Feedback", dontSendNotification);
	addAndMakeVisible(m_feedbackLabel);

    m_stereoKnob.setSliderStyle(Slider::Rotary);
    m_stereoKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    m_stereoKnob.setRange(0.0f, 400.0f, 0.5f);
    m_stereoKnob.setValue(0.0f);
    m_stereoKnob.setColour(Slider::rotarySliderFillColourId, Colours::white);
    m_stereoKnob.setBounds(105, 230 - 40, 100, 100);
    addAndMakeVisible(m_stereoKnob);
	m_stereoLabel.setBounds(110, 290 - 40, 100, 100);
	m_stereoLabel.setText("Stereo", dontSendNotification);
	addAndMakeVisible(m_stereoLabel);

	m_shakeKnob.setSliderStyle(Slider::Rotary);
	m_shakeKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
	m_shakeKnob.setRange(0.0f, 10.0f, 0.5f);
	m_shakeKnob.setValue(2.0f);
	m_shakeKnob.setColour(Slider::rotarySliderFillColourId, Colours::white);
	m_shakeKnob.setBounds(5, 300 - 40, 100, 100);
	addAndMakeVisible(m_shakeKnob);
	m_shakeLabel.setBounds(10, 360 - 40, 100, 100);
	m_shakeLabel.setText("Shake", dontSendNotification);
	addAndMakeVisible(m_shakeLabel);
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

	g.setColour(juce::Colours::white);
    g.drawLine(400, 30, 400, 380, 2);
    g.drawLine(350, 30, 390, 380, 2);
    g.drawLine(450, 30, 410, 380, 2);

	float currentDelayTime = m_delayTimeKnob.getValue();
	float currentFeedback = m_feedbackKnob.getValue();
	float currentStereo = m_stereoKnob.getValue();
	float currentShake = m_shakeKnob.getValue();
    
    for (int i = 0; i < 6; i++)
    {
		float currentShakeX = randomShake.nextFloat() * currentShake;
        float currentShakeY = randomShake.nextFloat() * currentShake;
		float side = (i % 2 == 0) ? 1 : -1;

        objects[i].setPosition(350 + currentShakeX + (side * ((currentStereo / 80) * (5 * i) * 0.4)),
                               330 + currentShakeY - (i * (60 * currentDelayTime / 500.0)));
        
        //float feedbackGradiant = (i * 0.1 / juce::jmap<float>(currentFeedback, 0.0, 1.0, 0.1, 1.0));
        objects[i].setAlpha(pow(currentFeedback,i));

        objects[i].draw(g, 100);
    }

}

void FractureAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void FractureAudioProcessorEditor::timerCallback()
{
	repaint();
}