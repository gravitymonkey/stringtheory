#include "PluginEditor.h"

StringFieldMIDIEditor::StringFieldMIDIEditor(StringFieldMIDIProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    // Configure sliders
    auto setupSlider = [this](juce::Slider& slider, juce::Label& label,
                              const juce::String& text)
    {
        addAndMakeVisible(slider);
        addAndMakeVisible(label);
        label.setText(text, juce::dontSendNotification);
        label.attachToComponent(&slider, true);
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    };

    setupSlider(rateSlider, rateLabel, "Rate");
    setupSlider(densitySlider, densityLabel, "Density");
    setupSlider(energySlider, energyLabel, "Energy");
    setupSlider(centerSlider, centerLabel, "Center");
    setupSlider(spreadSlider, spreadLabel, "Spread");
    setupSlider(velSlider, velLabel, "Velocity");
    setupSlider(seedSlider, seedLabel, "Seed");

    // Attach to parameters
    rateAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "rate", rateSlider);
    densityAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "density", densitySlider);
    energyAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "energy", energySlider);
    centerAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "center", centerSlider);
    spreadAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "spread", spreadSlider);
    velAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "vel", velSlider);
    seedAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "seed", seedSlider);

    setSize(500, 400);
}

void StringFieldMIDIEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(
        juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("String Field MIDI Generator",
                     getLocalBounds().removeFromTop(30),
                     juce::Justification::centred, 1);
}

void StringFieldMIDIEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(30); // Title space

    int sliderHeight = 40;
    int labelWidth = 100;

    rateSlider.setBounds(area.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    densitySlider.setBounds(area.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    energySlider.setBounds(area.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    centerSlider.setBounds(area.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    spreadSlider.setBounds(area.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    velSlider.setBounds(area.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
    seedSlider.setBounds(area.removeFromTop(sliderHeight).withTrimmedLeft(labelWidth));
}
