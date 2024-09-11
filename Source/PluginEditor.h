#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"  // Assuming you have a custom look and feel class

//==============================================================================
/**
    This class is responsible for handling the plugin's GUI.
*/
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

    /* Mouse event handlers to show / hide text box on knob hover
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    */
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
