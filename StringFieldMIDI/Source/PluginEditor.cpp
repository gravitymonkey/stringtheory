#include "PluginEditor.h"

StringFieldMIDIEditor::StringFieldMIDIEditor(StringFieldMIDIProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    // Set custom vintage LookAndFeel for all sliders
    auto setupSlider = [this](juce::Slider& slider, juce::Label& label,
                              const juce::String& text)
    {
        addAndMakeVisible(slider);
        addAndMakeVisible(label);
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colour(0xFFD4AF37));
        label.setFont(juce::Font("Courier New", 11.0f, juce::Font::bold));

        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 65, 18);
        slider.setLookAndFeel(&vintageLAF);
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xFFFFBF00));
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF1A1A1A));
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF5A5A5A));
    };

    setupSlider(rateSlider, rateLabel, "RATE");
    setupSlider(densitySlider, densityLabel, "DENSITY");
    setupSlider(energySlider, energyLabel, "ENERGY");
    setupSlider(centerSlider, centerLabel, "CENTER");
    setupSlider(spreadSlider, spreadLabel, "SPREAD");
    setupSlider(velSlider, velLabel, "VELOCITY");
    setupSlider(seedSlider, seedLabel, "SEED");
    setupSlider(routesSlider, routesLabel, "ROUTES");
    setupSlider(memorySlider, memoryLabel, "MEMORY");
    setupSlider(articulationSlider, articulationLabel, "ARTICULATION");
    setupSlider(pulseSlider, pulseLabel, "PULSE");
    setupSlider(tempoSlider, tempoLabel, "TEMPO");
    setupSlider(regularitySlider, regularityLabel, "REGULARITY");
    setupSlider(pcModeSlider, pcModeLabel, "PC MODE");
    setupSlider(pedalSlider, pedalLabel, "PEDAL");

    // Setup PC set text editor
    addAndMakeVisible(pcSetEditor);
    addAndMakeVisible(pcSetLabel);
    pcSetLabel.setText("PC SET", juce::dontSendNotification);
    pcSetLabel.setJustificationType(juce::Justification::centredLeft);
    pcSetLabel.setColour(juce::Label::textColourId, juce::Colour(0xFFD4AF37));
    pcSetLabel.setFont(juce::Font("Courier New", 11.0f, juce::Font::bold));

    pcSetEditor.setMultiLine(false);
    pcSetEditor.setReturnKeyStartsNewLine(false);
    pcSetEditor.setText(processor.getPitchClassSet());
    pcSetEditor.setColour(juce::TextEditor::textColourId, juce::Colour(0xFFFFBF00));
    pcSetEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF1A1A1A));
    pcSetEditor.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xFF5A5A5A));
    pcSetEditor.setFont(juce::Font("Courier New", 14.0f, juce::Font::bold));
    pcSetEditor.addListener(this);

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
    routesAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "routes", routesSlider);
    memoryAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "memory", memorySlider);
    articulationAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "articulation", articulationSlider);
    pulseAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "pulse", pulseSlider);
    tempoAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "tempo", tempoSlider);
    regularityAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "regularity", regularitySlider);
    pcModeAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "pcmode", pcModeSlider);
    pedalAttachment = std::make_unique<SliderAttachment>(
        processor.apvts, "pedal", pedalSlider);

    setSize(700, 600);
}

void StringFieldMIDIEditor::textEditorTextChanged(juce::TextEditor&)
{
    processor.setPitchClassSet(pcSetEditor.getText());
}

void StringFieldMIDIEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Vintage dark panel background with gradient
    juce::ColourGradient bgGradient(
        juce::Colour(0xFF2F2F2F), 0.0f, 0.0f,
        juce::Colour(0xFF1A1A1A), 0.0f, (float)getHeight(),
        false);
    g.setGradientFill(bgGradient);
    g.fillAll();

    // Draw subtle brushed metal texture
    g.setColour(juce::Colour(0x0A000000));
    for (int y = 0; y < getHeight(); y += 3)
        g.drawLine(0.0f, (float)y, (float)getWidth(), (float)y, 0.5f);

    // Outer frame (brass bezel)
    g.setColour(juce::Colour(0xFF8B7355));
    g.drawRect(bounds, 8);
    g.setColour(juce::Colour(0xFFD4AF37));
    g.drawRect(bounds.reduced(4), 2);

    // Title plate
    auto titleArea = bounds.removeFromTop(60).reduced(20, 15);
    g.setColour(juce::Colour(0xFF1A1A1A));
    g.fillRoundedRectangle(titleArea.toFloat(), 3.0f);

    g.setColour(juce::Colour(0xFF8B7355));
    g.drawRoundedRectangle(titleArea.toFloat(), 3.0f, 1.5f);

    g.setColour(juce::Colour(0xFFD4AF37));
    g.setFont(juce::Font("Courier New", 22.0f, juce::Font::bold));
    g.drawFittedText("STRING FIELD MIDI",
                     titleArea,
                     juce::Justification::centred, 1);

    // Subtitle
    g.setFont(juce::Font("Courier New", 10.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xFF8B7355));
    g.drawText("GENERATIVE ALGORITHM v0.1.2",
              titleArea.withTrimmedTop(30),
              juce::Justification::centred);

    // Screw decorations (vintage hardware aesthetic)
    auto drawScrew = [&g](float x, float y)
    {
        g.setColour(juce::Colour(0xFF4A4A4A));
        g.fillEllipse(x - 4.0f, y - 4.0f, 8.0f, 8.0f);
        g.setColour(juce::Colour(0xFF6A6A6A));
        g.drawEllipse(x - 4.0f, y - 4.0f, 8.0f, 8.0f, 1.0f);
        g.setColour(juce::Colour(0xFF2A2A2A));
        g.drawLine(x - 3.0f, y, x + 3.0f, y, 1.0f);
    };

    drawScrew(15.0f, 15.0f);
    drawScrew((float)getWidth() - 15.0f, 15.0f);
    drawScrew(15.0f, (float)getHeight() - 15.0f);
    drawScrew((float)getWidth() - 15.0f, (float)getHeight() - 15.0f);
}

void StringFieldMIDIEditor::resized()
{
    auto area = getLocalBounds().reduced(30);
    area.removeFromTop(55); // Title space

    // Reserve space for PC controls at bottom
    auto pcControlsArea = area.removeFromBottom(70);

    // 3×3 grid layout for rotary knobs
    int knobSize = 100;
    int horizontalSpacing = 30;
    int verticalSpacing = 20;

    // Calculate total width needed and center the grid
    int totalWidth = 3 * knobSize + 2 * horizontalSpacing;
    int startX = area.getX() + (area.getWidth() - totalWidth) / 2;

    auto setupKnob = [&](juce::Slider& slider, juce::Label& label, int row, int col)
    {
        int x = startX + col * (knobSize + horizontalSpacing);
        int y = area.getY() + row * (knobSize + verticalSpacing);

        auto knobArea = juce::Rectangle<int>(x, y, knobSize, knobSize);
        slider.setBounds(knobArea.withTrimmedTop(16));
        label.setBounds(x, y, knobSize, 16);
    };

    // Row 1: Rate, Density, Energy
    setupKnob(rateSlider, rateLabel, 0, 0);
    setupKnob(densitySlider, densityLabel, 0, 1);
    setupKnob(energySlider, energyLabel, 0, 2);

    // Row 2: Center, Spread, Velocity
    setupKnob(centerSlider, centerLabel, 1, 0);
    setupKnob(spreadSlider, spreadLabel, 1, 1);
    setupKnob(velSlider, velLabel, 1, 2);

    // Row 3: Seed, Routes, Memory
    setupKnob(seedSlider, seedLabel, 2, 0);
    setupKnob(routesSlider, routesLabel, 2, 1);
    setupKnob(memorySlider, memoryLabel, 2, 2);

    // Row 4: Articulation (centered)
    setupKnob(articulationSlider, articulationLabel, 3, 1);

    // PC controls at bottom (separate area)
    pcControlsArea.removeFromTop(10); // Spacing

    // PC Mode toggle (small knob on left)
    auto pcModeArea = pcControlsArea.removeFromLeft(80);
    pcModeLabel.setBounds(pcModeArea.removeFromTop(16));
    pcModeSlider.setBounds(pcModeArea.removeFromTop(45));

    pcControlsArea.removeFromLeft(10); // Spacing

    // Pedal toggle (small knob)
    auto pedalArea = pcControlsArea.removeFromLeft(70);
    pedalLabel.setBounds(pedalArea.removeFromTop(16));
    pedalSlider.setBounds(pedalArea.removeFromTop(45));

    pcControlsArea.removeFromLeft(10); // Spacing

    // Pulse toggle (small knob)
    auto pulseArea = pcControlsArea.removeFromLeft(70);
    pulseLabel.setBounds(pulseArea.removeFromTop(16));
    pulseSlider.setBounds(pulseArea.removeFromTop(45));

    pcControlsArea.removeFromLeft(10); // Spacing

    // Tempo knob
    auto tempoArea = pcControlsArea.removeFromLeft(80);
    tempoLabel.setBounds(tempoArea.removeFromTop(16));
    tempoSlider.setBounds(tempoArea.removeFromTop(45));

    pcControlsArea.removeFromLeft(10); // Spacing

    // Regularity knob
    auto regularityArea = pcControlsArea.removeFromLeft(90);
    regularityLabel.setBounds(regularityArea.removeFromTop(16));
    regularitySlider.setBounds(regularityArea.removeFromTop(45));

    pcControlsArea.removeFromLeft(10); // Spacing

    // PC Set text editor (rest of width)
    pcSetLabel.setBounds(pcControlsArea.removeFromTop(16));
    pcSetEditor.setBounds(pcControlsArea.removeFromTop(32).reduced(2, 2));
}
