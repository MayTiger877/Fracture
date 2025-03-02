/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FractureAudioProcessor::FractureAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	 : AudioProcessor (BusesProperties()
					 #if ! JucePlugin_IsMidiEffect
					  #if ! JucePlugin_IsSynth
					   .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
					  #endif
					   .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
					 #endif
					   ), apvts(*this, nullptr, juce::Identifier("PARAMETERS"), createParameters())
#endif
{
}

FractureAudioProcessor::~FractureAudioProcessor()
{
}

//==============================================================================
const juce::String FractureAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool FractureAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
	return true;
   #else
	return false;
   #endif
}

bool FractureAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
	return true;
   #else
	return false;
   #endif
}

bool FractureAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
	return true;
   #else
	return false;
   #endif
}

double FractureAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int FractureAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int FractureAudioProcessor::getCurrentProgram()
{
	return 0;
}

void FractureAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FractureAudioProcessor::getProgramName (int index)
{
	return {};
}

void FractureAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FractureAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	m_sampleRate = sampleRate;
	m_samplesPerBlock = samplesPerBlock;
    
    auto delayBufferSize = static_cast<int>(sampleRate * 2.0); // 2 seconds delay
    m_delayBuffer.setSize(getTotalNumOutputChannels(), static_cast<int>(delayBufferSize));

	m_filter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 1000.0f);
}

void FractureAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FractureAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
	juce::ignoreUnused (layouts);
	return true;
  #else
	// This is the place where you check if the params is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
	 && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input params matches the output params
   #if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
   #endif

	return true;
  #endif
}
#endif

void FractureAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels  = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {

        // Copy input signal to a delay buffer
        fillBuffer(buffer, channel);

        // Read from the past in the delay buffer, then add back to main buffer
        readFromBuffer(buffer, m_delayBuffer, channel);

        // Copy input signal to a delay buffer
        feedbackBuffer(buffer, channel);
    }

    updateBufferPositions(buffer, m_delayBuffer);
}

void FractureAudioProcessor::fillBuffer(juce::AudioBuffer<float>& buffer, int channel)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = m_delayBuffer.getNumSamples();
    auto* wet = apvts.getRawParameterValue("DRYWET");

    //buffer.applyGain(0, bufferSize, 1.0f - (wet->load() / 100.0f)); TODO- check this crap

    // Check to see if main buffer copies to delay buffer without needing to wrap..
    if (delayBufferSize > bufferSize + m_writePosition)
    {
        // if yes
               // copy main buffer contents to delay buffer
        m_delayBuffer.copyFrom(channel, m_writePosition, buffer.getWritePointer(channel), bufferSize);
    }
    // if no
    else
    {
        // Determine how much space is left at the end of the delay buffer
        auto numSamplesToEnd = delayBufferSize - m_writePosition;

        // Copy that amount of contents to the end...
        m_delayBuffer.copyFrom(channel, m_writePosition, buffer.getWritePointer(channel), numSamplesToEnd);

        // Calculate how much contents is remaining to copy
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;

        // Copy remaining amount to beginning of delay buffer
        m_delayBuffer.copyFrom(channel, 0, buffer.getWritePointer(channel, numSamplesToEnd-1), numSamplesAtStart);
    }

    /*
       DBG ("Delay Buffer Size: " << delayBufferSize);
       DBG ("Buffer Size: " << bufferSize);
       DBG ("Write Position: " << m_writePosition);
       */
}

void FractureAudioProcessor::feedbackBuffer(juce::AudioBuffer<float>& buffer, int channel)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = m_delayBuffer.getNumSamples();
    // feedback
    auto fb = apvts.getRawParameterValue("FEEDBACK")->load();

    // Check to see if main buffer copies to delay buffer without needing to wrap...
    if (delayBufferSize >= bufferSize + m_writePosition)
    {
        // copy main buffer contents to delay buffer
        m_delayBuffer.addFromWithRamp(channel, m_writePosition, buffer.getWritePointer(channel), bufferSize, fb, fb);
    }
    // if no
    else
    {
        // Determine how much space is left at the end of the delay buffer
        auto numSamplesToEnd = delayBufferSize - m_writePosition;

        // Copy that amount of contents to the end...
        m_delayBuffer.addFromWithRamp(channel, m_writePosition, buffer.getWritePointer(channel), numSamplesToEnd, fb, fb);

        // Calculate how much contents is remaining to copy
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;

        // Copy remaining amount to beginning of delay buffer
        m_delayBuffer.addFromWithRamp(channel, 0, buffer.getWritePointer(channel, numSamplesToEnd), numSamplesAtStart, fb, fb);
    }
}

void FractureAudioProcessor::readFromBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& m_delayBuffer, int channel)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = m_delayBuffer.getNumSamples();

    auto percent = apvts.getRawParameterValue("DRYWET")->load();
    auto g = juce::jmap(percent, 0.f, 100.f, 0.f, 1.f);
    auto dryGain = 1.f - g;

    auto stereoDiff = apvts.getRawParameterValue("STEREO")->load();
    auto delayTimeLeft = apvts.getRawParameterValue("DELAYTIME")->load();
    auto delayTimeRight = delayTimeLeft + stereoDiff;

    auto delayTime = channel == 0 ? delayTimeLeft : delayTimeRight;

    // m_writePosition = "Where is pur audio currently?"
    // readPosition = m_writePosition - sampleRate -> 1 second in the past (samplerate amount of audio)
    // 1 second of audio from in the past
    auto readPosition = std::round(m_writePosition - (getSampleRate() * delayTime / 1000.0f));

    if (readPosition < 0)
        readPosition += delayBufferSize;

    //buffer.applyGainRamp(0, bufferSize, dryGain, dryGain); TODO- same as other TODO

    //    auto g = 0.7f; // volume delay

    if (readPosition + bufferSize < delayBufferSize)
    {
        buffer.addFromWithRamp(channel, 0, m_delayBuffer.getReadPointer(channel, readPosition), bufferSize, g, g);
    }
    else
    {
        auto numSamplesToEnd = delayBufferSize - readPosition;
        buffer.addFromWithRamp(channel, 0, m_delayBuffer.getReadPointer(channel, readPosition), numSamplesToEnd, g, g);

        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        buffer.addFromWithRamp(channel, numSamplesToEnd, m_delayBuffer.getReadPointer(channel, 0), numSamplesAtStart, g, g);
    }
}

void FractureAudioProcessor::updateBufferPositions(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& m_delayBuffer)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = m_delayBuffer.getNumSamples();

    m_writePosition += bufferSize;
    m_writePosition %= delayBufferSize; // assure that right position always stays between 0 and delay buffer size
}


//==============================================================================
bool FractureAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FractureAudioProcessor::createEditor()
{
	return new FractureAudioProcessorEditor (*this);
}

//==============================================================================
void FractureAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void FractureAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new FractureAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout FractureAudioProcessor::createParameters()
{
	juce::AudioProcessorValueTreeState::ParameterLayout params;

    params.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "DRYWET", 1 }, "Dry/Wet", 0.0f, 100.0f, 0.0f));

    params.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "DELAYTIME", 1 }, "Delay", 0.5f, 500.0f, 0.0f));

    params.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "FEEDBACK", 1 }, "Feedback", 0.0f, 1.0f, 0.0f));

    params.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "STEREO", 1 }, "Stereo", 0.0f, 400.0f, 0.0f));

	params.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "SHAKE", 1 }, "Shake", 0.0f, 10.0f, 2.0f));

	return params;
}