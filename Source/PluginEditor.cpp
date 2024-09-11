#include <JuceHeader.h>
#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
OctaveFuzzPedalAudioProcessorEditor::OctaveFuzzPedalAudioProcessorEditor(OctaveFuzzPedalAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(500, 400);  // Set the size of the editor window

    // Load the background texture
    backgroundTexture = juce::ImageCache::getFromMemory(BinaryData::pedalBackground_jpg, BinaryData::pedalBackground_jpgSize);

    // Load Font from BinaryData
    juce::Typeface::Ptr customTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::Super_Caramel_ttf, BinaryData::Super_Caramel_ttfSize);
    juce::Font customFont(customTypeface.get());

    // Load images for the toggle button
    juce::Image mainButtonOff = juce::ImageFileFormat::loadFrom(BinaryData::mainbuttonOff_png, BinaryData::mainbuttonOff_pngSize);
    juce::Image mainButtonOn = juce::ImageFileFormat::loadFrom(BinaryData::mainbuttonOn_png, BinaryData::mainbuttonOn_pngSize);

    // Set images for the ImageButton
    octaveToggleButton.setImages(true, true, true, // Toggle state
        mainButtonOff, 1.0f, {},   // Off state image
        mainButtonOn, 1.0f, {},    // On state image
        mainButtonOn, 1.0f, {});   // Hover state image

    // Load images for the indicator lights
    OctavateOFFImage = juce::ImageFileFormat::loadFrom(BinaryData::OctavateOFF_png, BinaryData::OctavateOFF_pngSize);
    OctavateOnImage = juce::ImageFileFormat::loadFrom(BinaryData::OctavateOn_png, BinaryData::OctavateOn_pngSize);


    // Add the button to the editor
    addAndMakeVisible(octaveToggleButton);

    // Set button click handler
    octaveToggleButton.onClick = [this] {
        // Toggle octave effect in the processor
        bool isOctaveEnabled = !audioProcessor.getOctaveUpEnabled();
        audioProcessor.setOctaveUpEnabled(isOctaveEnabled);

        // Optionally, change button image manually if not using automatic toggle
        octaveToggleButton.setToggleState(isOctaveEnabled, juce::NotificationType::dontSendNotification);
        repaint();  // This forces the GUI to refresh and call the paint method again

        };

    // Fuzz Amount Knob
    fuzzAmountKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    fuzzAmountKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);  // Start with no text box visible
    fuzzAmountKnob.setRange(0.1, 1.3, 0.01);
    fuzzAmountKnob.setValue(audioProcessor.getFuzzAmount());
    fuzzAmountKnob.onValueChange = [this] { audioProcessor.setFuzzAmount(fuzzAmountKnob.getValue()); };
    fuzzAmountKnob.setLookAndFeel(&customLookAndFeel); // Apply custom LookAndFeel
    fuzzAmountKnob.addMouseListener(this, true);  // Add mouse listener to handle hover
    addAndMakeVisible(fuzzAmountKnob);

    // Fuzz Label
    fuzzAmountLabel.setText("FUZZ", juce::dontSendNotification);
    fuzzAmountLabel.attachToComponent(&fuzzAmountKnob, false);  // Attach to knob, position below
    fuzzAmountLabel.setJustificationType(juce::Justification::centredTop); // Center text at the top
    fuzzAmountLabel.setFont(customFont.withHeight(20.0f));  // Apply custom font
    fuzzAmountLabel.setColour(juce::Label::textColourId, juce::Colours::rebeccapurple); // Set label color
    addAndMakeVisible(fuzzAmountLabel);

    // Blend Knob
    blendKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    blendKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);  // Start with no text box visible
    blendKnob.setRange(0.1, 1.0, 0.01);
    blendKnob.setValue(audioProcessor.getBlendAmount());
    blendKnob.onValueChange = [this] { audioProcessor.setBlendAmount(blendKnob.getValue()); };
    blendKnob.setLookAndFeel(&customLookAndFeel);
    blendKnob.addMouseListener(this, true);  // Add mouse listener to handle hover
    addAndMakeVisible(blendKnob);

    // Blend Label
    blendLabel.setText("BLEND", juce::dontSendNotification);
    blendLabel.attachToComponent(&blendKnob, false); // Attach to knob, position below
    blendLabel.setJustificationType(juce::Justification::centredTop); // Center text at the top
    blendLabel.setFont(customFont.withHeight(20.0f));  // Apply custom font
    blendLabel.setColour(juce::Label::textColourId, juce::Colours::rebeccapurple); // Set label color
    addAndMakeVisible(blendLabel);

    startTimerHz(30); // Start the timer to periodically update GUI elements if needed
}

OctaveFuzzPedalAudioProcessorEditor::~OctaveFuzzPedalAudioProcessorEditor()
{
    // Reset to the default LookAndFeel to avoid dangling pointers
    fuzzAmountKnob.setLookAndFeel(nullptr);
    blendKnob.setLookAndFeel(nullptr);

    // Remove listeners to avoid dangling references
    fuzzAmountKnob.removeListener(this);
    blendKnob.removeListener(this);
}

//==============================================================================
void OctaveFuzzPedalAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Draw the background texture
    g.drawImage(backgroundTexture, getLocalBounds().toFloat());
   
    // Calculate destination rectangle for the indicator light
    juce::Rectangle<float> indicatorRect(150, 75, 200, 50);

    if (audioProcessor.getOctaveUpEnabled())
    {
        g.drawImage(OctavateOnImage,
            indicatorRect.getX(), indicatorRect.getY(),
            indicatorRect.getWidth(), indicatorRect.getHeight(),
            0, 0, OctavateOnImage.getWidth(), OctavateOnImage.getHeight());
    }
    else
    {
        g.drawImage(OctavateOFFImage,
            indicatorRect.getX(), indicatorRect.getY(),
            indicatorRect.getWidth(), indicatorRect.getHeight(),
            0, 0, OctavateOFFImage.getWidth(), OctavateOFFImage.getHeight());
    }
}
void OctaveFuzzPedalAudioProcessorEditor::resized()
{
    // Set knob positions
    fuzzAmountKnob.setBounds(25, 50, 100, 100); // Position the fuzz knob
    octaveToggleButton.setBounds(190.5, 175, 119, 77);  // Adjust as needed
    blendKnob.setBounds(375, 50, 100, 100); // Position the blend knob
}

void OctaveFuzzPedalAudioProcessorEditor::timerCallback()
{
    // This function can be used to update the GUI periodically, if needed
}

void OctaveFuzzPedalAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &fuzzAmountKnob)
    {
        // Update fuzz amount in the processor
        audioProcessor.setFuzzAmount(fuzzAmountKnob.getValue());
    }
    else if (slider == &blendKnob)
    {
        // Update blend amount in the processor
        audioProcessor.setBlendAmount(blendKnob.getValue());
    }
}

/*==============================================================================
void OctaveFuzzPedalAudioProcessorEditor::mouseEnter(const juce::MouseEvent& event)
{
    /*
    if (event.eventComponent == &fuzzAmountKnob)
        fuzzAmountKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);  // Show text box below knob
    else if (event.eventComponent == &blendKnob)
        blendKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);  // Show text box below knob
}

void OctaveFuzzPedalAudioProcessorEditor::mouseExit(const juce::MouseEvent& event)
{
    if (event.eventComponent == &fuzzAmountKnob)
        fuzzAmountKnob.setTextBoxStyle(juce::Slider::NoTextBox, true, 80, 20);  // Hide text box
    else if (event.eventComponent == &blendKnob)
        blendKnob.setTextBoxStyle(juce::Slider::NoTextBox, true, 80, 20);  // Hide text box
} */
