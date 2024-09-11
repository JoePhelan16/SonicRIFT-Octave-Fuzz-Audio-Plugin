#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        // Load the knob image from memory (make sure blueknob.png is added to BinaryData)
        knobImage = juce::ImageCache::getFromMemory(BinaryData::blueknob_png, BinaryData::blueknob_pngSize);
    }

    void drawRotarySlider(juce::Graphics& g,
                          int x, int y, int width, int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        if (knobImage.isValid())  // Check if the image is valid
        {
            const double rotation = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
            const float radius = juce::jmin(width / 2, height / 2) - 4.0f;
            const float centerX = (float)x + (float)width * 0.5f;
            const float centerY = (float)y + (float)height * 0.5f;
            const float knobRadius = knobImage.getWidth() / 2.0f;

            // Save the current state of the Graphics context
            g.saveState();

            // Move the graphics context to the center of the knob
            g.addTransform(juce::AffineTransform::rotation(rotation, centerX, centerY));

            // Draw the image centered at the calculated position
            g.drawImageTransformed(knobImage,
                                   juce::AffineTransform::translation(centerX - knobRadius, centerY - knobRadius),
                                   false);

            // Restore the graphics context to its original state
            g.restoreState();
        }
        else
        {
            // Fallback to default rotary slider if image is not available
            juce::LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPosProportional, rotaryStartAngle, rotaryEndAngle, slider);
        }
    }

private:
    juce::Image knobImage;
};
