#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OctaveFuzzPedalAudioProcessor::OctaveFuzzPedalAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
        #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
        #endif
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
        #endif
    )
#endif
{
    fuzzAmount = 0.5f;   // Initialize fuzz amount
    gainAmount = 1.0f;     // Initialize gain amount to 1.0 (no gain by default)
    blendAmount = 0.5f;  // Initialize blend amount

    // Initialize SmoothedValue for filter cutoff frequencies
    smoothedHighPassCutoff.reset(44100.0, 0.01); // Assuming a default sample rate; adjust as needed
    smoothedLowPassCutoff.reset(44100.0, 0.01);  // Assuming a default sample rate; adjust as needed
}

OctaveFuzzPedalAudioProcessor::~OctaveFuzzPedalAudioProcessor()
{
}

//==============================================================================
const juce::String OctaveFuzzPedalAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OctaveFuzzPedalAudioProcessor::acceptsMidi() const
{
    return false;
}

bool OctaveFuzzPedalAudioProcessor::producesMidi() const
{
    return false;
}

bool OctaveFuzzPedalAudioProcessor::isMidiEffect() const
{
    return false;
}

double OctaveFuzzPedalAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OctaveFuzzPedalAudioProcessor::getNumPrograms()
{
    return 1;  // Only 1 program is available
}

int OctaveFuzzPedalAudioProcessor::getCurrentProgram()
{
    return 0;  // Single program, so always return 0
}

void OctaveFuzzPedalAudioProcessor::setCurrentProgram (int index)
{
    // No action needed since we have only one program
}

const juce::String OctaveFuzzPedalAudioProcessor::getProgramName (int index)
{
    return {};  // Return an empty string since there are no named programs
}

void OctaveFuzzPedalAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    // No action needed since we don't support program names
}

void OctaveFuzzPedalAudioProcessor::setGain(float newGain)
{
    targetGain = newGain;  // Store the target gain
    smoothedGain.setTargetValue(newGain); // Set the smoothed gain target
}


//==============================================================================
void OctaveFuzzPedalAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Store the sample rate for future use
    lastSampleRate = sampleRate;
}

void OctaveFuzzPedalAudioProcessor::releaseResources()
{
    // When playback stops, you can use this method to free up resources
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OctaveFuzzPedalAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // This function checks if the audio bus layout is supported.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
}
#endif

void OctaveFuzzPedalAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that are not in use
    for (int channel = totalNumInputChannels; channel < totalNumOutputChannels; ++channel)
        buffer.clear(channel, 0, buffer.getNumSamples());

    // Check for valid buffer size
    int numSamples = buffer.getNumSamples();
    if (numSamples <= 0)
    {
        return;
    }

    // Retrieve the current values for parameters
    float fuzzAmount = getFuzzAmount();
    constexpr float gainAmount = 10.0f;
    bool octaveUpEnabled = getOctaveUpEnabled();
    float blendAmount = getBlendAmount();

    // Adjusted output gain compensation factor to maintain consistent volume
    float outputGainCompensation = 1.0f + (5.0f - fuzzAmount) * 0.5f; 
    // Compensation factor for octave-up effect
    float octaveBoostFactor = octaveUpEnabled ? 2.5f : 1.0f; // Adjust the boost factor as needed (e.g., 1.5x louder)

    // Process each channel
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Smooth the gain ramp using smoothedGain
            float smoothedGainValue = smoothedGain.getNextValue();

            // Original input sample (dry signal)
            float inputSample = channelData[sample];

            // Apply gain to the input sample
            float gainedSample = gainAmount * inputSample;

            // Initialize processedSample with the gained input
            float processedSample = gainedSample;

            // Apply effects based on button state
            if (octaveUpEnabled)
            {
                // Apply octave-up effect (rectify signal)
                processedSample = std::fabs(inputSample * 8.0f) * octaveBoostFactor;

            }
            else
            {
                // Apply fuzz effect with soft clipping using tanh
                float amplifiedSample = fuzzAmount * processedSample;

                // Additional gain to ensure tanh produces distortion
                amplifiedSample *= 5.0f;

                // Apply soft clipping using tanh function
                processedSample = std::tanh(amplifiedSample);
            }

             //Apply output gain compensation
            processedSample *= outputGainCompensation;

            // Blend wet (processed effect) and dry (original input) signals
            float finalSample = (1.0f - blendAmount) * inputSample + blendAmount * processedSample;

            // Apply the smoothed gain to the final sample
            finalSample *= smoothedGainValue;

            // Write the blended sample back to the buffer
            channelData[sample] = finalSample;
        }

    }
}

//==============================================================================
bool OctaveFuzzPedalAudioProcessor::hasEditor() const
{
    return true;  // Return true to indicate that the plugin has a GUI editor
}

juce::AudioProcessorEditor* OctaveFuzzPedalAudioProcessor::createEditor()
{
    return new OctaveFuzzPedalAudioProcessorEditor(*this);
}

//==============================================================================
void OctaveFuzzPedalAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save your plugin's state here (parameters, etc.)
}

void OctaveFuzzPedalAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Load your plugin's state here (parameters, etc.)
}

//==============================================================================
// This creates new instances of the plugin.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OctaveFuzzPedalAudioProcessor();
}