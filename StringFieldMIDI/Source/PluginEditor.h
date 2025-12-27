#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class StringFieldMIDIEditor : public juce::AudioProcessorEditor
{
public:
    StringFieldMIDIEditor(StringFieldMIDIProcessor&);
    ~StringFieldMIDIEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    StringFieldMIDIProcessor& processor;

    juce::Slider rateSlider, densitySlider, energySlider;
    juce::Slider centerSlider, spreadSlider, velSlider;
    juce::Slider seedSlider, routesSlider;

    juce::Label rateLabel, densityLabel, energyLabel;
    juce::Label centerLabel, spreadLabel, velLabel;
    juce::Label seedLabel, routesLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> rateAttachment;
    std::unique_ptr<SliderAttachment> densityAttachment;
    std::unique_ptr<SliderAttachment> energyAttachment;
    std::unique_ptr<SliderAttachment> centerAttachment;
    std::unique_ptr<SliderAttachment> spreadAttachment;
    std::unique_ptr<SliderAttachment> velAttachment;
    std::unique_ptr<SliderAttachment> seedAttachment;
    std::unique_ptr<SliderAttachment> routesAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringFieldMIDIEditor)
};
