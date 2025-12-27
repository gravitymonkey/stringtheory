cmake_minimum_required(VERSION 3.15)

project(StringFieldMIDI VERSION 0.1.0)

# Add JUCE
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Add the plugin target
juce_add_plugin(StringFieldMIDI
    COMPANY_NAME "StringTheory"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE STth
    PLUGIN_CODE SfMi
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "String Field MIDI")

# Source files
target_sources(StringFieldMIDI
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h)

# Compile definitions
target_compile_definitions(StringFieldMIDI
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

# Link libraries
target_link_libraries(StringFieldMIDI
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
