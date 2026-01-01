#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// Custom vintage LookAndFeel for rotary knobs
class VintageLookAndFeel : public juce::LookAndFeel_V4
{
public:
    VintageLookAndFeel()
    {
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFFD4AF37));
        setColour(juce::Slider::thumbColourId, juce::Colour(0xFFFFBF00));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override
    {
        auto radius = juce::jmin(width / 2, height / 2) - 8.0f;
        auto centreX = x + width * 0.5f;
        auto centreY = y + height * 0.5f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Outer ring (brushed brass)
        g.setColour(juce::Colour(0xFF8B7355)); // Dark brass
        g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

        // Inner knob body
        g.setColour(juce::Colour(0xFF3A3A3A)); // Dark grey
        g.fillEllipse(centreX - radius * 0.85f, centreY - radius * 0.85f,
                     radius * 1.7f, radius * 1.7f);

        // Highlight gradient for 3D effect
        juce::ColourGradient gradient(
            juce::Colour(0xFF5A5A5A), centreX - radius * 0.5f, centreY - radius * 0.5f,
            juce::Colour(0xFF2A2A2A), centreX + radius * 0.5f, centreY + radius * 0.5f,
            false);
        g.setGradientFill(gradient);
        g.fillEllipse(centreX - radius * 0.8f, centreY - radius * 0.8f,
                     radius * 1.6f, radius * 1.6f);

        // Tick marks around knob
        g.setColour(juce::Colour(0xFFD4AF37)); // Brass
        for (float i = 0; i <= 10; ++i)
        {
            auto tickAngle = rotaryStartAngle + (i / 10.0f) * (rotaryEndAngle - rotaryStartAngle);
            auto tickRadius = radius * 0.95f;
            auto tickLength = (i == 0 || i == 5 || i == 10) ? 8.0f : 5.0f;

            juce::Path tick;
            tick.addLineSegment(juce::Line<float>(
                centreX + tickRadius * std::cos(tickAngle - juce::MathConstants<float>::halfPi),
                centreY + tickRadius * std::sin(tickAngle - juce::MathConstants<float>::halfPi),
                centreX + (tickRadius - tickLength) * std::cos(tickAngle - juce::MathConstants<float>::halfPi),
                centreY + (tickRadius - tickLength) * std::sin(tickAngle - juce::MathConstants<float>::halfPi)
            ), 1.5f);
            g.strokePath(tick, juce::PathStrokeType(1.5f));
        }

        // Pointer (brass indicator line)
        juce::Path pointer;
        auto pointerLength = radius * 0.65f;
        auto pointerThickness = 3.0f;
        pointer.addRectangle(-pointerThickness * 0.5f, -pointerLength, pointerThickness, pointerLength);
        g.setColour(juce::Colour(0xFFFFBF00)); // Bright amber
        g.fillPath(pointer, juce::AffineTransform::rotation(angle)
                                                   .translated(centreX, centreY));

        // Center cap
        g.setColour(juce::Colour(0xFF1A1A1A));
        g.fillEllipse(centreX - 6.0f, centreY - 6.0f, 12.0f, 12.0f);
        g.setColour(juce::Colour(0xFFD4AF37));
        g.drawEllipse(centreX - 6.0f, centreY - 6.0f, 12.0f, 12.0f, 1.0f);
    }
};

class StringFieldMIDIEditor : public juce::AudioProcessorEditor,
                               private juce::TextEditor::Listener
{
public:
    StringFieldMIDIEditor(StringFieldMIDIProcessor&);
    ~StringFieldMIDIEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    // TextEditor::Listener
    void textEditorTextChanged(juce::TextEditor&) override;

private:
    StringFieldMIDIProcessor& processor;

    VintageLookAndFeel vintageLAF;

    juce::Slider rateSlider, densitySlider, energySlider;
    juce::Slider centerSlider, spreadSlider, velSlider;
    juce::Slider seedSlider, routesSlider, memorySlider;
    juce::Slider articulationSlider, pcModeSlider, pedalSlider;
    juce::Slider pulseSlider, tempoSlider, regularitySlider;

    juce::Label rateLabel, densityLabel, energyLabel;
    juce::Label centerLabel, spreadLabel, velLabel;
    juce::Label seedLabel, routesLabel, memoryLabel;
    juce::Label articulationLabel, pcModeLabel, pcSetLabel, pedalLabel;
    juce::Label pulseLabel, tempoLabel, regularityLabel;

    juce::TextEditor pcSetEditor;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> rateAttachment;
    std::unique_ptr<SliderAttachment> densityAttachment;
    std::unique_ptr<SliderAttachment> energyAttachment;
    std::unique_ptr<SliderAttachment> centerAttachment;
    std::unique_ptr<SliderAttachment> spreadAttachment;
    std::unique_ptr<SliderAttachment> velAttachment;
    std::unique_ptr<SliderAttachment> seedAttachment;
    std::unique_ptr<SliderAttachment> routesAttachment;
    std::unique_ptr<SliderAttachment> memoryAttachment;
    std::unique_ptr<SliderAttachment> articulationAttachment;
    std::unique_ptr<SliderAttachment> pulseAttachment;
    std::unique_ptr<SliderAttachment> tempoAttachment;
    std::unique_ptr<SliderAttachment> regularityAttachment;
    std::unique_ptr<SliderAttachment> pcModeAttachment;
    std::unique_ptr<SliderAttachment> pedalAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringFieldMIDIEditor)
};
