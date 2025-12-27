#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

class StringFieldMIDIProcessor : public juce::AudioProcessor
{
public:
    StringFieldMIDIProcessor();
    ~StringFieldMIDIProcessor() override = default;

    // === JUCE AudioProcessor Interface ===
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    bool isBusesLayoutSupported(const BusesLayout&) const override { return true; }

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "StringFieldMIDI"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }

    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    // === Parameter Access ===
    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    // === State Variables ===
    double sr = 44100.0;
    bool wasPlaying = false;

    // Scheduler (sample-time)
    int64_t sampleCounter = 0;
    int64_t nextNoteOnSample = 0;

    // Active note (monophonic v0.1)
    int activeNote = -1;
    int activeChannel = 1;
    int64_t noteOffSample = 0;

    // RNG
    juce::Random rng;
    int lastSeed = 1;

    // === Helper Methods ===
    void handleTransportStop(juce::MidiBuffer& midi);
    void scheduleNextNote(double bpm, double ppqPos);
    int pickNote(int center, int spread);
    int pickVelocity(int baseVel);
    int pickArticulation(int numRoutes, float energy);
    double calculateDuration(float energy);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringFieldMIDIProcessor)
};
