#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <deque>

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

    // === Public API for Editor ===
    void setPitchClassSet(const juce::String& pcString) { parsePitchClassSet(pcString); }
    juce::String getPitchClassSet() const { return lastPCSetString; }

    // MIDI Learn API
    void setMIDILearnMode(bool enabled, const juce::String& paramID = "");
    bool isMIDILearning() const { return midiLearnEnabled; }
    juce::String getMIDILearnParameter() const { return midiLearnParameterID; }
    int getCCForParameter(const juce::String& paramID) const;
    void clearCCMapping(const juce::String& paramID);
    void clearAllCCMappings();

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

    // Sustain pedal state (automatic, energy/density controlled)
    bool pedalDown = false;
    int64_t nextPedalChangeSample = 0;
    bool lastPedalParamState = false;  // Track parameter changes

    // RNG
    juce::Random rng;
    int lastSeed = 1;

    // Memory kernels (Feldman-ish fragile memory)
    std::deque<int> recentNotes;      // Pitch memory
    std::deque<double> recentIntervals;  // Rhythm memory

    // Pitch-class set state
    std::vector<int> pitchClassSet;       // Current PC set (0-11)
    std::vector<int> remainingPCs;        // PCs not yet exhausted
    std::map<int, int> pcToMidiMap;       // PC → MIDI note memory (for octave consistency)
    juce::String lastPCSetString;         // Track when to re-parse

    // MIDI Learn state
    std::map<int, juce::String> ccToParameterMap;  // CC number → parameter ID
    bool midiLearnEnabled = false;
    juce::String midiLearnParameterID;

    // === Helper Methods ===
    void handleTransportStop(juce::MidiBuffer& midi);
    void scheduleNextNote(double bpm, double ppqPos);
    void schedulePedalChange(float energy, float density);
    int pickNote(int center, int spread);
    int pickVelocity(int baseVel);
    int pickArticulation(int numRoutes, float articulation, float energy);
    double calculateDuration(float energy);

    // Pitch-class set helpers
    void parsePitchClassSet(const juce::String& pcString);
    void transformPitchClassSet();  // Apply random Tn or TnI
    int mapPCToMIDI(int pitchClass, int center, int spread);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringFieldMIDIProcessor)
};
