#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h" 

class OctaveFuzzPedalAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Slider::Listener, private juce::Timer, private juce::MouseListener
{
public:
    OctaveFuzzPedalAudioProcessorEditor(OctaveFuzzPedalAudioProcessor&);
    ~OctaveFuzzPedalAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    // Listener for slider changes
    void sliderValueChanged(juce::Slider* slider) override;
private:
    OctaveFuzzPedalAudioProcessor& audioProcessor;
    void timerCallback() override;  // Timer callback to periodically update GUI elements

    // UI components
    juce::Slider fuzzAmountKnob;
    juce::Slider blendKnob;
    juce::Label fuzzAmountLabel;
    juce::Label blendLabel;

    // Separate labels for showing values
    juce::Label fuzzAmountValueLabel;
    juce::Label blendValueLabel;

    juce::Image backgroundTexture;  // Background image for the pedal
    juce::ImageButton octaveToggleButton;
    juce::TooltipWindow tooltipWindow;  // Tooltip window for showing knob descriptions

    CustomLookAndFeel customLookAndFeel;  // Instance of custom LookAndFeel for the knobs
   
    // Indicator lights for octave up
    juce::Image OctavateOFFImage;
    juce::Image OctavateOnImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OctaveFuzzPedalAudioProcessorEditor)
};
