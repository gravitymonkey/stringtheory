# String Field MIDI Generator
## Development Guide & Implementation Specification

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Compositional Philosophy](#compositional-philosophy)
3. [Technical Architecture](#technical-architecture)
4. [Development Environment Setup](#development-environment-setup)
5. [Plugin Architecture (JUCE Framework)](#plugin-architecture-juce-framework)
6. [Implementation Roadmap](#implementation-roadmap)
7. [Core Algorithm Design](#core-algorithm-design)
8. [Source Code Structure](#source-code-structure)
9. [Testing & Validation](#testing--validation)
10. [Future Enhancements](#future-enhancements)
11. [References & Resources](#references--resources)

---

## Project Overview

### Goal
Build a MIDI generator plugin that applies string theory concepts to algorithmic composition, creating music inspired by Morton Feldman's aesthetic. The plugin generates MIDI events that can be routed to software instruments in Logic Pro (and other DAWs).

### Core Concept
Use string theory as a **constraint engine** (not literal sonification) to determine what *can* happen compositionally, rather than what *does* happen. This creates:

- **Identity as state, not object** - Notes are vibrational modes, not discrete symbols
- **Time as field, not arrow** - No teleological progression, only parameter drift
- **Continuity without causality** - Events don't develop toward goals
- **Low-energy statistical mechanics** - Feldman-like sparse, quiet, non-teleological music

### Deliverables

**v0.1 - Live Generator**
- AU MIDI Effect plugin for macOS
- Generates MIDI during DAW playback
- Basic parameter set (Rate, Density, Energy, Register, Seed)
- Monophonic output
- Deterministic (same seed = same output)

**v0.2 - MIDI Export**
- "Export MIDI" button to save generated sequences as .mid files
- Event logging with timestamps
- Bounce to file capability

**v0.3 - Enhanced Routing**
- Virtual MIDI port support (optional)
- Polyphonic voice management
- Loop handling
- Advanced state engine with drift

---

## Compositional Philosophy

### The String Theory Framework

#### 1. Pitch as Vibrational Mode
- A pitch is not a discrete object but a mode of vibration of an underlying "string"
- Define a string as a continuous frequency band (e.g., 110–880 Hz)
- Notes are stable standing-wave modes, not equal-tempered steps
- Harmony emerges as resonant compatibility, not chord progression

#### 2. Multidimensional Harmony (Extra Dimensions)
String theory's extra dimensions are compact and hidden but affect physics. Musical mapping:
- **Audible pitch** = 1st dimension
- **Hidden dimensions** = parameters like:
  - Bow pressure / articulation
  - Timbral brightness
  - Micro-intonation drift
  - Spatialization
  - Noise content

Two events may be the "same note" but live in radically different dimensional states.

#### 3. Energy States Instead of Themes
- No themes, no motives
- Only energy configurations defined by:
  - Density (events per unit time)
  - Bandwidth (pitch spread)
  - Attack rate
  - Spectral roughness
  - Spatial spread

Listeners perceive continuity without repetition.

#### 4. Symmetry Breaking as Drama
- Begin with extreme symmetry (uniform rhythm, mirrored pitch fields)
- Introduce tiny perturbations
- Let instability grow until structure "condenses"
- Compose phase transitions, not climaxes

#### 5. Memory Without Development
- Maintain a decaying memory kernel
- Past states affect probability, not prohibition
- Creates déjà vu without motif, without return
- Peak Feldman aesthetic

### Feldman Connection

**Why This Sounds Like Feldman:**

Feldman's core move was removing causality without removing precision:
- No development
- No goal-oriented form
- No "this causes that"
- Extreme care about duration, spacing, density, register, memory

**Conceptual Overlap:**

| Feldman | String Theory Composition |
|---------|---------------------------|
| Notes don't "go" anywhere | States don't resolve |
| Time is a surface | Time is a parameter field |
| Repetition without identity | Modes recur, but never identically |
| Memory is fragile | Past states influence probability, not form |
| Quiet as structure | Low energy = stability |

### Mental Model

Think of this as:
- **Xenakis** = high-energy statistical mechanics
- **Feldman** = low-energy statistical mechanics
- **Your system** = the full phase space

String theory provides the language to talk about modes, dimensions, energy, and stability.

### Litmus Test

If your algorithm:
- Never knows "where it's going"
- But always knows "how it feels right now"
- And resists memory without erasing it

Then you are absolutely in Feldman territory.

---

## Technical Architecture

### Platform & Framework

**JUCE Framework** (https://juce.com/)
- Industry-standard C++ framework for audio plugins
- Cross-format support (AU, VST3, AAX, Standalone)
- Built-in parameter management (AudioProcessorValueTreeState)
- MIDI handling via `juce::MidiBuffer`
- GUI components and preset management

**Target Format:** Audio Unit (AU) for macOS/Logic Pro
- Logic Pro only accepts AU plugins (not VST)
- AU has dedicated MIDI FX slot in channel strip
- Plugin must set `isMidiEffect() = true`

### Plugin Type: MIDI Effect

From the Audio Plugin Developer reference:
- MIDI effects accept and/or produce MIDI
- Process MIDI events in `processBlock(AudioBuffer&, MidiBuffer&)`
- Can optionally process audio but primarily transform/generate MIDI
- Examples: arpeggiators, note remappers, generators

**Key JUCE Plugin Characteristics:**
```cpp
bool acceptsMidi() const override { return true; }
bool producesMidi() const override { return true; }
bool isMidiEffect() const override { return true; }
```

### Architecture Layers

#### 1. State Engine (Slow Drift)
- Keeps energy, density, register bias, spectral width
- Updates on host tempo/PPQ position
- Deterministic with playback
- Maintains RNG seed for repeatability

#### 2. Event Renderer (Fast)
- Per audio block, decides whether to emit note-on/off events
- Quantizes to musical grid (or intentionally doesn't)
- Maintains note lifetimes for proper note-offs
- Sample-accurate event placement within buffer

#### 3. Parameter Management
- JUCE AudioProcessorValueTreeState for automation
- Float parameters: Rate, Density, Energy
- Int parameters: Seed, Register Center, Register Spread, Velocity
- State save/recall for DAW projects

#### 4. Transport Integration
- Read host playhead info (tempo, PPQ position, isPlaying)
- Only generate when transport is playing
- Handle transport stop/start gracefully
- Send All Notes Off on stop

---

## Development Environment Setup

### Prerequisites

1. **macOS** (for AU development and Logic Pro testing)
2. **Xcode** (latest stable version)
   - Command Line Tools installed
   - Apple Developer account (free tier OK for local development)
3. **JUCE Framework**
   - Download from https://juce.com/
   - Install Projucer (JUCE's project management tool)
4. **Logic Pro** (for testing)
5. **C++ knowledge** (C++17 or later)

### Installation Steps

1. **Download JUCE:**
   ```bash
   # Option A: Download from juce.com
   # Option B: Clone from GitHub
   git clone https://github.com/juce-framework/JUCE.git ~/JUCE
   ```

2. **Build Projucer:**
   - Navigate to `JUCE/extras/Projucer/Builds/MacOSX/`
   - Open `Projucer.xcodeproj` in Xcode
   - Build and run

3. **Set JUCE Path:**
   - Open Projucer
   - Go to Preferences → Global Paths
   - Set "JUCE Modules" path to your JUCE installation

### Project Structure

```
StringFieldMIDI/
├── StringFieldMIDI.jucer          # Projucer project file
├── Source/
│   ├── PluginProcessor.h          # Main audio processor
│   ├── PluginProcessor.cpp        # Processing logic
│   ├── PluginEditor.h             # GUI
│   ├── PluginEditor.cpp           # GUI implementation
│   ├── StateEngine.h              # State drift engine
│   ├── StateEngine.cpp            # State implementation
│   └── MIDIScheduler.h            # Event scheduling
├── Builds/
│   └── MacOSX/                    # Xcode project (generated)
└── JuceLibraryCode/               # JUCE modules (generated)
```

---

## Plugin Architecture (JUCE Framework)

### Key Plugin Methods (from Audio Plugin Developer PDF)

#### `processBlock(AudioBuffer<float>&, MidiBuffer&)`
The heart of the plugin. Called regularly by the host.

**Purpose:**
- Process incoming audio/MIDI
- Generate outgoing audio/MIDI
- Sample-accurate event placement

**For MIDI Generator:**
```cpp
void processBlock(juce::AudioBuffer<float>& buffer, 
                  juce::MidiBuffer& midiMessages)
{
    // 1. Clear audio (we don't output audio)
    buffer.clear();
    
    // 2. Read host playhead
    auto playhead = getPlayHead();
    
    // 3. Check if playing
    if (!isPlaying) {
        sendAllNotesOff(midiMessages);
        return;
    }
    
    // 4. Generate MIDI events for this block
    generateEventsForBlock(midiMessages, buffer.getNumSamples());
}
```

#### `prepareToPlay(double sampleRate, int samplesPerBlock)`
Called before playback starts.

**Purpose:**
- Initialize buffers
- Set sample rate
- Reset state

```cpp
void prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    sampleCounter = 0;
    nextNoteOnSample = 0;
    activeNote = -1;
    // Reset state engine
}
```

#### `releaseResources()`
Called when playback stops.

**Purpose:**
- Clean up resources
- Free memory

#### Parameter Management

**AudioProcessorValueTreeState:**
```cpp
juce::AudioProcessorValueTreeState apvts;

// In constructor:
apvts (*this, nullptr, "PARAMS", createParameterLayout())

// Create parameters:
static AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<AudioParameterFloat>(
        "rate", "Rate", 0.05f, 20.0f, 2.0f));
    
    // ... more parameters
    
    return { params.begin(), params.end() };
}
```

### MIDI Buffer Operations

**Adding Events:**
```cpp
// Note On
midiMessages.addEvent(
    juce::MidiMessage::noteOn(channel, noteNumber, velocity),
    sampleOffset  // Position within block
);

// Note Off
midiMessages.addEvent(
    juce::MidiMessage::noteOff(channel, noteNumber),
    sampleOffset
);

// Control Change
midiMessages.addEvent(
    juce::MidiMessage::controllerEvent(channel, controllerNum, value),
    sampleOffset
);
```

**All Notes Off:**
```cpp
void sendAllNotesOff(juce::MidiBuffer& midi)
{
    for (int ch = 1; ch <= 16; ++ch)
    {
        midi.addEvent(juce::MidiMessage::controllerEvent(ch, 123, 0), 0);
        midi.addEvent(juce::MidiMessage::controllerEvent(ch, 120, 0), 0);
    }
}
```

### Playhead Information

```cpp
if (auto* playhead = getPlayHead())
{
    juce::AudioPlayHead::CurrentPositionInfo info;
    if (playhead->getCurrentPosition(info))
    {
        bool isPlaying = info.isPlaying;
        double bpm = info.bpm > 0.0 ? info.bpm : 120.0;
        double ppqPosition = info.ppqPosition;
        double timeInSeconds = info.timeInSeconds;
    }
}
```

### State Persistence

```cpp
void getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}
```

---

## Implementation Roadmap

### Phase 1: v0.1 - Monophonic Live Generator

**Goal:** Working AU plugin that generates MIDI during playback.

**Tasks:**
1. Create JUCE project in Projucer
   - Enable AU format
   - Set plugin characteristics (MIDI Effect)
   - Configure build settings

2. Implement basic PluginProcessor
   - Parameter setup (Rate, Density, Energy, Center, Spread, Velocity, Seed)
   - Transport awareness
   - Sample counter and timeline tracking

3. Implement monophonic event scheduler
   - "Next event time" calculation
   - Note-on generation
   - Automatic note-off scheduling
   - Sample-accurate placement

4. Implement state engine (simple)
   - Seeded RNG
   - Basic note picking (center ± spread)
   - Velocity variation
   - Energy affects duration

5. Create minimal GUI
   - 7 parameter sliders
   - Randomize seed button
   - Basic visual feedback

6. Test in Logic Pro
   - Install AU in ~/Library/Audio/Plug-Ins/Components/
   - Validate with auval (Audio Unit Validator)
   - Test transport start/stop
   - Verify deterministic behavior

### Phase 2: v0.2 - MIDI Export

**Goal:** Add ability to save generated MIDI to .mid files.

**Tasks:**
1. Implement event logger
   - Ring buffer of recent MIDI events
   - Store: PPQ position, note number, velocity, duration
   - Configurable history length (e.g., last 1000 events or 60 seconds)

2. Create MIDI file writer
   - Use juce::MidiFile class
   - Convert event log to MIDI file format
   - Handle tempo/time signature

3. Add UI controls
   - "Export MIDI" button
   - File save dialog
   - Status indicator

4. Test export workflow
   - Generate sequence
   - Export to .mid
   - Import into Logic Pro
   - Verify accuracy

### Phase 3: v0.3 - Polyphony & Advanced Features

**Goal:** Multiple simultaneous voices, advanced state engine, loop handling.

**Tasks:**
1. Implement polyphonic voice management
   - Voice allocation
   - Voice stealing (if needed)
   - Per-voice note-off tracking

2. Enhance state engine
   - Multi-dimensional state drift
   - Energy affects multiple parameters
   - Memory kernel (weighted past states)
   - Probability distributions (not just uniform random)

3. Handle DAW loop points
   - Detect playhead jumps
   - Reset state on loop (if deterministic mode)
   - Allow continuous evolution (if free mode)

4. Add advanced parameters
   - Duration bias (long notes vs. pointillism)
   - Quantize amount (grid vs. float)
   - Multiple energy dimensions
   - Spectral width

5. Optional: Virtual MIDI port
   - macOS CoreMIDI integration
   - Create virtual MIDI source
   - Or companion menubar app

---

## Core Algorithm Design

### State Representation

```cpp
struct GeneratorState
{
    // Energy parameters
    float energy;           // 0.0 - 1.0 (affects drift speed, jitter, duration)
    float density;          // 0.0 - 1.0 (probability of event generation)
    
    // Pitch field
    int registerCenter;     // MIDI note number (e.g., 60)
    int registerSpread;     // Semitones (e.g., 12)
    
    // Temporal
    float rate;             // Events per second
    
    // RNG
    int seed;
    juce::Random rng;
    
    // Memory (Phase 3)
    std::deque<int> recentNotes;
    float memoryWeight;
};
```

### Event Scheduling (Monophonic)

**Concept:**
- Maintain "next event time" as absolute sample position
- Each `processBlock()`, check if event falls within current block
- If yes, emit event and schedule next one

**Pseudocode:**
```
Initialize:
    sampleCounter = 0
    nextNoteOnSample = 0
    activeNote = -1
    noteOffSample = 0

Per processBlock(numSamples):
    blockStart = sampleCounter
    blockEnd = sampleCounter + numSamples
    
    // Handle note-off if scheduled in this block
    if (activeNote >= 0 && noteOffSample in [blockStart, blockEnd)):
        offset = noteOffSample - blockStart
        emit NoteOff(activeNote) at offset
        activeNote = -1
    
    // Handle note-on if scheduled in this block
    if (nextNoteOnSample in [blockStart, blockEnd)):
        offset = nextNoteOnSample - blockStart
        
        // Decide whether to emit (density check)
        if (random.nextFloat() <= density):
            note = pickNote(center, spread)
            vel = pickVelocity(baseVel)
            emit NoteOn(note, vel) at offset
            
            // Schedule note-off
            duration = calculateDuration(energy)
            noteOffSample = nextNoteOnSample + duration
            activeNote = note
        
        // Schedule next potential event
        scheduleNextEvent()
    
    sampleCounter += numSamples
```

### Note Selection Algorithm (v0.1)

**Simple Random:**
```cpp
int pickNote(int center, int spread)
{
    if (spread <= 0) 
        return juce::jlimit(0, 127, center);
    
    int lo = juce::jlimit(0, 127, center - spread);
    int hi = juce::jlimit(0, 127, center + spread);
    
    return rng.nextInt(juce::Range<int>(lo, hi + 1));
}
```

**With Velocity Variation:**
```cpp
int pickVelocity(int baseVel)
{
    // Feldman-ish small variability
    int variation = rng.nextInt(juce::Range<int>(-10, 11));
    return juce::jlimit(1, 127, baseVel + variation);
}
```

### Duration Calculation (Energy-Based)

**Feldman Aesthetic:** Low energy = longer notes (sustained, contemplative)

```cpp
double calculateDuration(float energy, double sampleRate)
{
    // Map energy inversely to duration
    // Low energy (0.0) -> long notes (1.2 sec)
    // High energy (1.0) -> short notes (0.08 sec)
    double durationSec = juce::jmap(
        (double)energy, 
        0.0, 1.0,
        1.2, 0.08
    );
    
    return durationSec * sampleRate;  // Convert to samples
}
```

### Interval Calculation (Rate + Energy Jitter)

```cpp
void scheduleNextNote(double bpm, double ppqPos)
{
    float rate = *apvts.getRawParameterValue("rate");
    float energy = *apvts.getRawParameterValue("energy");
    
    // Base interval in seconds
    double baseInterval = 1.0 / juce::jmax(0.001f, rate);
    
    // Add jitter based on energy
    // Higher energy = more rhythmic instability
    double jitterRange = energy * baseInterval;
    double jitter = (rng.nextDouble() - 0.5) * jitterRange;
    
    double intervalSec = juce::jmax(0.001, baseInterval + jitter);
    
    // Convert to samples
    nextNoteOnSample = sampleCounter + (int64_t)(intervalSec * sr);
}
```

### Deterministic Behavior

**Key Principle:** Same seed + same timeline = same output

```cpp
// In processBlock, before generating events:
const int seed = (int)*apvts.getRawParameterValue("seed");
if (seed != lastSeed)
{
    rng.setSeed((int64_t)seed);
    lastSeed = seed;
    
    // Reset scheduler to make change immediate
    nextNoteOnSample = sampleCounter;
}
```

---

## Source Code Structure

### PluginProcessor.h

```cpp
#pragma once
#include <JuceHeader.h>

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
    int64_t noteOffSample = 0;

    // RNG
    juce::Random rng;
    int lastSeed = 1;

    // === Helper Methods ===
    void handleTransportStop(juce::MidiBuffer& midi);
    void scheduleNextNote(double bpm, double ppqPos);
    int pickNote(int center, int spread);
    int pickVelocity(int baseVel);
    double calculateDuration(float energy);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringFieldMIDIProcessor)
};
```

### PluginProcessor.cpp (Key Sections)

```cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"

StringFieldMIDIProcessor::StringFieldMIDIProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMS", createParameters())
{
    rng.setSeed(1);
}

juce::AudioProcessorValueTreeState::ParameterLayout 
StringFieldMIDIProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "rate", "Rate", 0.05f, 20.0f, 2.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "density", "Density", 0.0f, 1.0f, 0.25f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "energy", "Energy", 0.0f, 1.0f, 0.2f));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "center", "Register Center", 24, 96, 60));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "spread", "Register Spread", 0, 36, 12));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "vel", "Velocity", 1, 127, 80));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "seed", "Seed", 1, 99999, 1));

    return { params.begin(), params.end() };
}

void StringFieldMIDIProcessor::prepareToPlay(double sampleRate, int)
{
    sr = sampleRate;
    sampleCounter = 0;
    nextNoteOnSample = 0;
    activeNote = -1;
    noteOffSample = 0;
    wasPlaying = false;
}

void StringFieldMIDIProcessor::handleTransportStop(juce::MidiBuffer& midi)
{
    // All Notes Off (CC 123) + All Sound Off (CC 120)
    for (int ch = 1; ch <= 16; ++ch)
    {
        midi.addEvent(juce::MidiMessage::controllerEvent(ch, 123, 0), 0);
        midi.addEvent(juce::MidiMessage::controllerEvent(ch, 120, 0), 0);
    }
    activeNote = -1;
    noteOffSample = 0;
}

int StringFieldMIDIProcessor::pickNote(int center, int spread)
{
    if (spread <= 0) 
        return juce::jlimit(0, 127, center);
    
    int lo = juce::jlimit(0, 127, center - spread);
    int hi = juce::jlimit(0, 127, center + spread);
    
    return rng.nextInt(juce::Range<int>(lo, hi + 1));
}

int StringFieldMIDIProcessor::pickVelocity(int baseVel)
{
    int variation = rng.nextInt(juce::Range<int>(-10, 11));
    return juce::jlimit(1, 127, baseVel + variation);
}

double StringFieldMIDIProcessor::calculateDuration(float energy)
{
    // Low energy -> long notes (Feldman-ish)
    double durationSec = juce::jmap((double)energy, 0.0, 1.0, 1.2, 0.08);
    return durationSec * sr;
}

void StringFieldMIDIProcessor::scheduleNextNote(double bpm, double ppqPos)
{
    float rate = *apvts.getRawParameterValue("rate");
    float density = *apvts.getRawParameterValue("density");
    float energy = *apvts.getRawParameterValue("energy");

    // Density check: probabilistically skip
    if (rng.nextFloat() > density)
    {
        double baseInterval = 1.0 / juce::jmax(0.001f, rate);
        nextNoteOnSample = sampleCounter + (int64_t)(baseInterval * sr);
        return;
    }

    // Calculate interval with energy-based jitter
    double baseInterval = 1.0 / juce::jmax(0.001f, rate);
    double jitter = (rng.nextDouble() - 0.5) * (energy * baseInterval);
    double intervalSec = juce::jmax(0.001, baseInterval + jitter);

    nextNoteOnSample = sampleCounter + (int64_t)(intervalSec * sr);
}

void StringFieldMIDIProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // === Read Playhead ===
    bool isPlaying = false;
    double bpm = 120.0;
    double ppqPos = 0.0;

    if (auto* playhead = getPlayHead())
    {
        juce::AudioPlayHead::CurrentPositionInfo info;
        if (playhead->getCurrentPosition(info))
        {
            isPlaying = info.isPlaying;
            bpm = info.bpm > 0.0 ? info.bpm : 120.0;
            ppqPos = info.ppqPosition;
        }
    }

    // === Seed Handling ===
    int seed = (int)*apvts.getRawParameterValue("seed");
    if (seed != lastSeed)
    {
        rng.setSeed((int64_t)seed);
        lastSeed = seed;
        nextNoteOnSample = sampleCounter;
    }

    // === Transport Stop ===
    if (wasPlaying && !isPlaying)
        handleTransportStop(midiMessages);
    
    wasPlaying = isPlaying;

    if (!isPlaying)
    {
        sampleCounter += buffer.getNumSamples();
        return;
    }

    // === Event Generation ===
    const int numSamples = buffer.getNumSamples();
    const int64_t blockStart = sampleCounter;
    const int64_t blockEnd = sampleCounter + numSamples;

    // Initialize scheduler if needed
    if (nextNoteOnSample <= blockStart)
        scheduleNextNote(bpm, ppqPos);

    // 1. Emit note-off if scheduled
    if (activeNote >= 0 && 
        noteOffSample >= blockStart && 
        noteOffSample < blockEnd)
    {
        int offset = (int)(noteOffSample - blockStart);
        midiMessages.addEvent(
            juce::MidiMessage::noteOff(1, activeNote), 
            offset
        );
        activeNote = -1;
        noteOffSample = 0;
    }

    // 2. Emit note-on if scheduled
    if (nextNoteOnSample >= blockStart && nextNoteOnSample < blockEnd)
    {
        int center = (int)*apvts.getRawParameterValue("center");
        int spread = (int)*apvts.getRawParameterValue("spread");
        int baseVel = (int)*apvts.getRawParameterValue("vel");
        float energy = *apvts.getRawParameterValue("energy");

        int note = pickNote(center, spread);
        int vel = pickVelocity(baseVel);
        int offset = (int)(nextNoteOnSample - blockStart);

        midiMessages.addEvent(
            juce::MidiMessage::noteOn(1, note, (juce::uint8)vel),
            offset
        );

        // Schedule note-off
        double duration = calculateDuration(energy);
        noteOffSample = nextNoteOnSample + (int64_t)duration;
        activeNote = note;

        // Schedule next event
        scheduleNextNote(bpm, ppqPos);
    }

    sampleCounter += numSamples;
}

void StringFieldMIDIProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void StringFieldMIDIProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StringFieldMIDIProcessor();
}
```

### PluginEditor.h (Minimal GUI)

```cpp
#pragma once
#include <JuceHeader.h>
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
    juce::Slider seedSlider;

    juce::Label rateLabel, densityLabel, energyLabel;
    juce::Label centerLabel, spreadLabel, velLabel;
    juce::Label seedLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    std::unique_ptr<SliderAttachment> rateAttachment;
    std::unique_ptr<SliderAttachment> densityAttachment;
    std::unique_ptr<SliderAttachment> energyAttachment;
    std::unique_ptr<SliderAttachment> centerAttachment;
    std::unique_ptr<SliderAttachment> spreadAttachment;
    std::unique_ptr<SliderAttachment> velAttachment;
    std::unique_ptr<SliderAttachment> seedAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringFieldMIDIEditor)
};
```

### PluginEditor.cpp (Minimal GUI)

```cpp
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
```

---

## Testing & Validation

### Build Process

1. **Generate Xcode Project:**
   - Open `StringFieldMIDI.jucer` in Projucer
   - Click "Save Project and Open in IDE"

2. **Build in Xcode:**
   - Select "All" or "AU" scheme
   - Build (⌘B)
   - Product will be in `~/Library/Audio/Plug-Ins/Components/`

3. **Validate AU:**
   ```bash
   auval -v aufx Midi YrID
   # Replace YrID with your manufacturer code from Projucer
   ```

### Logic Pro Testing

**Setup:**
1. Open Logic Pro
2. Create Software Instrument track
3. Add your plugin to MIDI FX slot (not Audio FX!)
4. Add a software synth (e.g., ES2, Alchemy)

**Test Cases:**

**TC1: Basic Generation**
- Set Rate = 2.0 Hz
- Set Density = 1.0 (always generate)
- Set Seed = 1
- Press play
- Verify: Notes generated at ~2 Hz

**TC2: Determinism**
- Play 4 bars with Seed = 1
- Stop and rewind to start
- Play again
- Verify: Exact same note sequence

**TC3: Density**
- Set Density = 0.0
- Press play
- Verify: No notes generated
- Set Density = 0.5
- Verify: Approximately half the events vs. Density = 1.0

**TC4: Energy & Duration**
- Set Energy = 0.0 (low)
- Press play
- Verify: Long, sustained notes
- Set Energy = 1.0 (high)
- Verify: Short, pointillistic notes

**TC5: Transport Stop**
- Generate some notes
- Stop transport while notes are held
- Verify: All notes immediately stop (no hanging notes)

**TC6: Register**
- Set Center = 60 (middle C)
- Set Spread = 0
- Verify: All notes are C (MIDI 60)
- Set Spread = 12
- Verify: Notes range C4–C5 (MIDI 48-72)

**TC7: Seed Variation**
- Seed = 1 → Play → Note sequence A
- Seed = 2 → Play → Note sequence B
- Verify: A ≠ B
- Seed = 1 again → Verify: Sequence A returns

### Common Issues & Solutions

**Issue: Plugin doesn't appear in Logic**
- Solution: Run `auval` to check validation
- Ensure plugin is in `~/Library/Audio/Plug-Ins/Components/`
- Restart Logic Pro

**Issue: Hanging notes**
- Solution: Check `handleTransportStop()` is called
- Verify note-off tracking logic
- Add safety timeout for note-offs

**Issue: Timing drift**
- Solution: Use PPQ-based scheduling (Phase 2)
- Verify sample counter increments correctly
- Check for integer overflow on long sessions

**Issue: No MIDI output**
- Solution: Confirm `producesMidi() == true`
- Check MIDI buffer is not cleared after generation
- Verify events have correct sample offsets (0 to numSamples-1)

---

## Future Enhancements

### Phase 3 Features

#### Polyphonic Voice Management

```cpp
struct Voice
{
    int noteNumber = -1;
    int64_t noteOffSample = 0;
    bool active = false;
};

std::array<Voice, 8> voices;  // 8 concurrent voices

int allocateVoice()
{
    // Find inactive voice
    for (int i = 0; i < voices.size(); ++i)
        if (!voices[i].active) return i;
    
    // Voice stealing: take oldest
    int oldest = 0;
    for (int i = 1; i < voices.size(); ++i)
        if (voices[i].noteOffSample < voices[oldest].noteOffSample)
            oldest = i;
    
    return oldest;
}
```

#### Advanced State Engine

**Multi-dimensional drift:**
```cpp
struct StateEngine
{
    // Current state
    float energy = 0.2f;
    float density = 0.25f;
    int registerCenter = 60;
    float spectralWidth = 0.5f;
    
    // Drift parameters
    float energyDrift = 0.001f;
    float densityDrift = 0.0005f;
    
    // Memory kernel
    std::deque<int> recentNotes;
    static constexpr size_t maxMemory = 20;
    
    void update(double deltaTime)
    {
        // Brownian motion with bounds
        energy += (rng.nextFloat() - 0.5f) * energyDrift;
        energy = juce::jlimit(0.0f, 1.0f, energy);
        
        density += (rng.nextFloat() - 0.5f) * densityDrift;
        density = juce::jlimit(0.0f, 1.0f, density);
    }
    
    int pickNoteWithMemory(int center, int spread)
    {
        int note = pickNote(center, spread);
        
        // Avoid recent repetition (Feldman: fragile memory)
        int attempts = 0;
        while (attempts < 5 && 
               std::find(recentNotes.begin(), recentNotes.end(), note) 
                   != recentNotes.end())
        {
            note = pickNote(center, spread);
            ++attempts;
        }
        
        // Update memory
        recentNotes.push_back(note);
        if (recentNotes.size() > maxMemory)
            recentNotes.pop_front();
        
        return note;
    }
};
```

#### Loop Handling

```cpp
void processBlock(...)
{
    // Detect playhead jump (loop point)
    if (previousPPQ > currentPPQ + 0.1)  // Backward jump
    {
        // Option A: Reset state (deterministic mode)
        if (deterministicMode)
            resetStateToPosition(currentPPQ);
        
        // Option B: Continue evolving (free mode)
        // Do nothing, let state drift continue
    }
    
    previousPPQ = currentPPQ;
}
```

#### MIDI File Export

```cpp
void exportMIDI(const juce::File& outputFile)
{
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(960);
    
    juce::MidiMessageSequence sequence;
    
    // Convert event log to MIDI messages
    for (const auto& event : eventLog)
    {
        double timeInBeats = event.ppqPosition;
        int timestamp = (int)(timeInBeats * 960.0);
        
        if (event.isNoteOn)
        {
            sequence.addEvent(
                juce::MidiMessage::noteOn(1, event.note, event.velocity),
                timestamp
            );
        }
        else
        {
            sequence.addEvent(
                juce::MidiMessage::noteOff(1, event.note),
                timestamp
            );
        }
    }
    
    midiFile.addTrack(sequence);
    
    juce::FileOutputStream stream(outputFile);
    if (stream.openedOk())
        midiFile.writeTo(stream);
}
```

### Advanced Compositional Features

#### Spectral Modes (Phase 4)

Instead of equal-tempered notes, use harmonic series:

```cpp
// Define fundamental frequency
double fundamentalHz = 110.0;  // A2

// Generate harmonic series
std::vector<double> harmonics;
for (int n = 1; n <= 16; ++n)
    harmonics.push_back(fundamentalHz * n);

// Convert to MIDI note numbers (approximate)
int pickHarmonicNote()
{
    int harmonic = rng.nextInt(harmonics.size());
    double freqHz = harmonics[harmonic];
    
    // Freq to MIDI: note = 69 + 12*log2(f/440)
    double note = 69.0 + 12.0 * std::log2(freqHz / 440.0);
    return (int)std::round(note);
}
```

#### Brane Model (Phase 5)

Separate "planes" for different ensembles:

```cpp
enum class Brane
{
    Sustained,   // Long notes, woodwinds
    Articulated, // Short attacks, strings
    Percussive   // Noise bursts, percussion
};

struct BraneState
{
    Brane type;
    int midiChannel;
    float energy;
    int registerCenter;
};

// Material can "migrate" between branes
void transferEnergy(BraneState& from, BraneState& to, float amount)
{
    from.energy -= amount;
    to.energy += amount;
    
    // Could trigger events on the receiving brane
}
```

---

## References & Resources

### Documentation

**JUCE Framework:**
- Official Docs: https://docs.juce.com/
- Tutorials: https://juce.com/learn/tutorials
- Forum: https://forum.juce.com/
- API Reference: https://docs.juce.com/master/classes.html

**Audio Unit (AU):**
- Apple Developer: https://developer.apple.com/documentation/audiotoolbox/audio_unit_v3_plug-ins
- AU Validation: `/usr/bin/auval`

**MIDI Specification:**
- MIDI Association: https://www.midi.org/specifications
- MIDI 1.0 Detailed Specification (purchase from midi.org)

### Books

1. **Audio Plugin Developer** (uploaded PDF)
   - Chapters on JUCE framework
   - Plugin architecture patterns
   - MIDI processing techniques

2. **The JUCE Book** (unofficial community resource)
   - https://github.com/getdunne/juce-cookbook

3. **Designing Sound** by Andy Farnell
   - Sound synthesis concepts
   - Algorithmic composition

### Compositional References

**Morton Feldman:**
- "Give My Regards to Eighth Street: Collected Writings"
- "Essays" (edited by Walter Zimmermann)
- Listen: "Rothko Chapel", "For Philip Guston", late piano works

**Iannis Xenakis:**
- "Formalized Music: Thought and Mathematics in Composition"
- Listen: "Pithoprakta", "Metastaseis"

**Spectral Music:**
- Gérard Grisey: "Partiels", "Vortex Temporum"
- Tristan Murail: "Gondwana", "Les Sept Paroles"

### Code Examples

**JUCE MIDI Plugins:**
- JUCE arpeggiator example: `JUCE/examples/Plugins/ArpeggiatorPluginDemo.h`
- JUCE MIDI logger: `JUCE/examples/Plugins/MidiLoggerPluginDemo.h`

**Open Source MIDI Generators:**
- Hypercyclic (MIDI Sequencer): https://github.com/anu-prakash-dev/Hypercyclic
- ChordGun (Chord Generator): https://github.com/sjaelee/ChordGun

### Community

**Forums & Discord:**
- JUCE Forum: https://forum.juce.com/
- Audio Programmer Discord: https://discord.gg/audioprogrammer
- r/audiodev (Reddit): https://reddit.com/r/audiodev

**YouTube Channels:**
- The Audio Programmer
- Valerio Velardo (The Sound of AI)
- Joshua Hodge (JUCE tutorials)

---

## Appendix: Quick Start Checklist

### Day 1: Setup
- [ ] Install Xcode
- [ ] Download JUCE
- [ ] Build Projucer
- [ ] Create new Audio Plugin project
- [ ] Enable AU format
- [ ] Set "MIDI Effect" characteristic
- [ ] Build empty plugin
- [ ] Validate with `auval`
- [ ] Load in Logic Pro

### Day 2: Minimal Prototype
- [ ] Implement parameter layout (7 parameters)
- [ ] Add playhead reading
- [ ] Implement sample counter
- [ ] Add "next event" scheduler
- [ ] Generate single note-on
- [ ] Schedule corresponding note-off
- [ ] Test in Logic Pro

### Day 3: Core Algorithm
- [ ] Implement `pickNote()` with center/spread
- [ ] Implement `pickVelocity()` with variation
- [ ] Implement `calculateDuration()` based on energy
- [ ] Add energy-based jitter to scheduling
- [ ] Add density probabilistic gating
- [ ] Test parameter sweeps

### Day 4: Polish
- [ ] Implement transport stop handling (All Notes Off)
- [ ] Add deterministic seed behavior
- [ ] Create basic GUI (7 sliders)
- [ ] Implement state save/load
- [ ] Test in Logic Pro with various synths
- [ ] Document known issues

### Day 5: Validation
- [ ] Run full test suite (TC1-TC7)
- [ ] Test edge cases (very high/low parameter values)
- [ ] Test long sessions (>10 minutes)
- [ ] Test with multiple instances
- [ ] Test with different buffer sizes
- [ ] Create demo project in Logic Pro

---

## Closing Notes

This document provides a complete roadmap for building the String Field MIDI Generator. The approach balances:

1. **Theoretical grounding** in string theory concepts and Feldman's aesthetic
2. **Practical implementation** using industry-standard tools (JUCE)
3. **Incremental development** from minimal prototype to advanced features

**Philosophy First, Implementation Second:** The compositional philosophy is what makes this interesting. The JUCE/AU implementation is just the delivery mechanism. Keep returning to the core ideas:
- States, not objects
- Energy, not narrative
- Continuity, not causality
- Memory without development

**Start Small, Iterate:** v0.1 is intentionally minimal. Get a single note generating correctly before adding polyphony, complex state engines, or MIDI export. The algorithm can evolve significantly while the plugin scaffolding remains stable.

**Test in Context:** This isn't an academic exercise—test constantly in Logic Pro with real synths. Let your ears guide parameter ranges and behavior. If it doesn't sound "Feldman-ish" at low energy settings, adjust the algorithm until it does.

**Embrace Determinism:** Repeatability (via seed) is crucial for composition. Musicians need to be able to "print" a generated sequence, edit it, and continue working. This is why the PPQ-based scheduler and seeded RNG are so important.

Good luck building! This is fertile creative and technical territory.

---

**Document Version:** 1.0  
**Created:** December 27, 2024  
**Author:** Development specification for String Field MIDI Generator  
**Target Platform:** macOS / Logic Pro / JUCE Framework  
**License:** Proprietary - For development use
