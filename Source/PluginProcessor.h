#pragma once

#include <JuceHeader.h>

class OctaveFuzzPedalAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    OctaveFuzzPedalAudioProcessor();
    ~OctaveFuzzPedalAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
   
    // Methods to get and set the octave effect state
    bool getOctaveUpEnabled() const { return octaveUpEnabled; }
    void setOctaveUpEnabled(bool enabled) { octaveUpEnabled = enabled; }

    // Accessor methods for parameters
    float getFuzzAmount() const { return fuzzAmount; }
    void setFuzzAmount(float amount) { fuzzAmount = amount; }
    
    float getGainAmount() const { return gainAmount; }  // Accessor for gain amount
    void setGainAmount(float amount) { gainAmount = amount; }  // Mutator for gain amount

    float getBlendAmount() const { return blendAmount; }
    void setBlendAmount(float amount) { blendAmount = amount; }

    // Add this method to set the target gain
    void setGain(float newGain);

private:
    //==============================================================================
    float fuzzAmount;
    float gainAmount;      // Parameter for gain amount before fuzz effect
    bool octaveUpEnabled = false;  // Internal state to track octave effect
    float blendAmount;

    double lastSampleRate = 44100.0; // Default sample rate

    juce::IIRFilter highPassFilter;  // IIR filter for high-pass filtering
    juce::IIRFilter lowPassFilter;   // Low-pass filter instance
    
    // Gain ramping state
    float currentGain = 1.0f;        // Current gain level
    float targetGain = 1.0f;         // Target gain level
    float gainIncrement = 0.0f;      // Gain increment per sample for ramping
    juce::SmoothedValue<float> smoothedGain{ 1.0f }; // Initialized to a neutral gain value

    juce::SmoothedValue<float> smoothedHighPassCutoff{ 20.0f }; // Default low value for DC removal
    juce::SmoothedValue<float> smoothedLowPassCutoff{ 20000.0f }; // Default high value
    // Private methods for updating filters
    void updateHighPassFilter(float cutoff); // Declare updateHighPassFilter function
    void updateLowPassFilter(float cutoff);  // Declare updateLowPassFilter function

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OctaveFuzzPedalAudioProcessor)
};
