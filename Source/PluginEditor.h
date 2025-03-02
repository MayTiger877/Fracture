/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

using namespace juce;

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SpaceObjects.h"

//==============================================================================
/**
*/
class FractureAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    FractureAudioProcessorEditor (FractureAudioProcessor&);
    ~FractureAudioProcessorEditor() override;

    void timerCallback() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FractureAudioProcessor& audioProcessor;

    Slider m_dryWetKnob;
    Slider m_delayTimeKnob;
    Slider m_feedbackKnob;
    Slider m_stereoKnob;
	Slider m_shakeKnob;

	Label m_dryWetLabel;
	Label m_delayTimeLabel;
	Label m_feedbackLabel;
	Label m_stereoLabel;
	Label m_shakeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_dryWetKnobListener;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_delayTimeKnobListener;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_feedbackKnobListener;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_stereoKnobListener;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_shakeKnobListener;

    void initializeKnobs();

    //---------------------------------------------------
    //             Visuals
    SpaceObject objects[6];
    juce::Random randomShake;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FractureAudioProcessorEditor)
};
