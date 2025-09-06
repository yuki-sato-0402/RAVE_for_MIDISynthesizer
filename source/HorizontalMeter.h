#pragma once
#include <JuceHeader.h>

class HorizontalMeter : public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.fillAll(juce::Colours::black);

        auto zeroX = bounds.getCentreX();
        float v = juce::jlimit(-5.0f, 5.0f, value);

        float halfWidth = bounds.getWidth() / 2.0f;
        float length = (v / 5.0f) * halfWidth;

        g.setColour(juce::Colours::lightgrey);
        if (v >= 0.0f)
            g.fillRect(zeroX, 0.0f, length, bounds.getHeight());
        else
            g.fillRect(zeroX + length, 0.0f, -length, bounds.getHeight());

        g.setColour(juce::Colours::grey);
        g.drawLine((float)zeroX, 0.0f, (float)zeroX, bounds.getHeight(), 1.0f);

    }

    void setValue(float newValue)
    {
        value = newValue;
        repaint();
    }

private:
    float value = 0.0f; // Normalized value between 0.0
};
