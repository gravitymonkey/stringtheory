#include "PluginProcessor.h"
#include "PluginEditor.h"

StringFieldMIDIProcessor::StringFieldMIDIProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMS", createParameters())
{
    rng.setSeed(1);

    // Set up default CC mappings (conductor-ready out of the box)
    // Using unused CC range 20-31 to avoid conflicts with standard controllers
    ccToParameterMap[20] = "rate";           // CC 20 → Rate
    ccToParameterMap[21] = "density";        // CC 21 → Density
    ccToParameterMap[22] = "energy";         // CC 22 → Energy
    ccToParameterMap[23] = "center";         // CC 23 → Center
    ccToParameterMap[24] = "spread";         // CC 24 → Spread
    ccToParameterMap[25] = "vel";            // CC 25 → Velocity
    ccToParameterMap[26] = "memory";         // CC 26 → Memory
    ccToParameterMap[27] = "articulation";   // CC 27 → Articulation
    ccToParameterMap[28] = "pulse";          // CC 28 → Pulse
    ccToParameterMap[29] = "tempo";          // CC 29 → Tempo
    ccToParameterMap[30] = "regularity";     // CC 30 → Regularity
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

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "routes", "Num Routes", 1, 16, 1));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "memory", "Memory", 0, 16, 0));

    // Articulation: center of weighted distribution for route selection
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "articulation", "Articulation", 0.0f, 1.0f, 0.5f));

    // Pulse mode controls
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "pulse", "Pulse", 0, 1, 0));  // 0=Off, 1=On

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "tempo", "Tempo", 40.0f, 360.0f, 120.0f));  // BPM

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "regularity", "Regularity", 0.0f, 1.0f, 0.5f));

    // PC Mode: 0=Off, 1=Transpose, 2=Transpose+Invert
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "pcmode", "PC Mode", 0, 2, 0));

    // Sustain Pedal Enable: 0=Off, 1=On (automatic)
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "pedal", "Sustain Pedal", 0, 1, 1));  // Default ON

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
    // Release sustain pedal on all channels
    for (int ch = 1; ch <= 16; ++ch)
    {
        midi.addEvent(juce::MidiMessage::controllerEvent(ch, 64, 0), 0);  // Pedal up
        midi.addEvent(juce::MidiMessage::controllerEvent(ch, 123, 0), 0); // All Notes Off
        midi.addEvent(juce::MidiMessage::controllerEvent(ch, 120, 0), 0); // All Sound Off
    }
    activeNote = -1;
    noteOffSample = 0;
    pedalDown = false;
    nextPedalChangeSample = 0;
}

int StringFieldMIDIProcessor::pickNote(int center, int spread)
{
    if (spread <= 0)
        return juce::jlimit(0, 127, center);

    int pcMode = (int)*apvts.getRawParameterValue("pcmode");

    // === PITCH-CLASS SET MODE ===
    if (pcMode > 0 && !pitchClassSet.empty())
    {
        // Check if set is exhausted
        if (remainingPCs.empty())
            transformPitchClassSet();  // Transform and reset

        if (remainingPCs.empty())  // Fallback if still empty
            return juce::jlimit(0, 127, center);

        // Pick next pitch class from remaining set
        int pitchClass = remainingPCs.back();
        remainingPCs.pop_back();

        // Map to MIDI note with octave memory
        return mapPCToMIDI(pitchClass, center, spread);
    }

    // === CHROMATIC MODE ===
    int lo = juce::jlimit(0, 127, center - spread);
    int hi = juce::jlimit(0, 127, center + spread);

    // Get parameters
    int memorySize = (int)*apvts.getRawParameterValue("memory");
    float energy = *apvts.getRawParameterValue("energy");

    // MOTIVIC MEMORY MODE: Higher memory = more repetition of recent notes
    // Scale memory strength by energy
    // Low energy = strong repetition (motivic loops)
    // High energy = weaker repetition (more exploration)
    float memoryStrength = juce::jmap(energy, 0.0f, 1.0f, 1.0f, 0.3f);

    // No memory: use simple random
    if (memorySize <= 0 || recentNotes.empty())
    {
        int note = rng.nextInt(juce::Range<int>(lo, hi + 1));

        // Store in memory for future
        if (memorySize > 0)
        {
            recentNotes.push_back(note);
            if ((int)recentNotes.size() > memorySize)
                recentNotes.pop_front();
        }

        return note;
    }

    // With memory: FAVOR repeating recent notes (strength scaled by energy)
    // Probability to repeat from memory vs pick something new
    float repeatProbability = memoryStrength * 0.8f;  // 80% at low energy, 24% at high energy

    int note;
    if (rng.nextFloat() < repeatProbability && !recentNotes.empty())
    {
        // Pick randomly from recent notes (motivic repetition)
        int memoryIndex = rng.nextInt(juce::Range<int>(0, (int)recentNotes.size()));
        note = recentNotes[memoryIndex];
    }
    else
    {
        // Pick from full range (exploration)
        note = rng.nextInt(juce::Range<int>(lo, hi + 1));
    }

    // Update memory
    recentNotes.push_back(note);
    if ((int)recentNotes.size() > memorySize)
        recentNotes.pop_front();

    return note;
}

int StringFieldMIDIProcessor::pickVelocity(int baseVel)
{
    int variation = rng.nextInt(juce::Range<int>(-10, 11));
    return juce::jlimit(1, 127, baseVel + variation);
}

int StringFieldMIDIProcessor::pickArticulation(int numRoutes, float articulation, float energy)
{
    // Weighted distribution for articulation selection
    // articulation (0.0-1.0): center of distribution
    //   0.0 = favor channel 1 (legato, sustained)
    //   0.5 = favor middle channels
    //   1.0 = favor channel numRoutes (staccato, extreme)
    // energy: controls spread width
    //   Low energy = narrow focus (stay near center)
    //   High energy = wide spread (explore many articulations)

    if (numRoutes <= 1)
        return 1;

    // Map articulation to channel center (0-indexed: 0 to numRoutes-1)
    float centerChannel = articulation * (float)(numRoutes - 1);

    // Spread width controlled by energy
    // Low energy (0.0): ±0.5 channel (very focused)
    // High energy (1.0): ±(numRoutes-1) (full exploration)
    float spreadWidth = juce::jmap(energy, 0.0f, 1.0f,
                                    0.5f,
                                    (float)(numRoutes - 1));

    // Triangular distribution: sum of two random variables
    // Creates a peak at center, falls off linearly
    float r1 = rng.nextFloat() - 0.5f;  // -0.5 to 0.5
    float r2 = rng.nextFloat() - 0.5f;
    float offset = (r1 + r2) * spreadWidth;

    float channelFloat = centerChannel + offset;

    // Clamp and convert to 1-indexed MIDI channel
    int channel = (int)std::round(channelFloat) + 1;
    return juce::jlimit(1, numRoutes, channel);
}

double StringFieldMIDIProcessor::calculateDuration(float energy)
{
    // Low energy -> long notes (Feldman-ish)
    double durationSec = juce::jmap((double)energy, 0.0, 1.0, 1.2, 0.08);
    return durationSec * sr;
}

void StringFieldMIDIProcessor::schedulePedalChange(float energy, float density)
{
    // AUTOMATIC SUSTAIN PEDAL (Energy/Density controlled)
    // Low energy + low density = pedal stays DOWN for long periods (creates chords/washes)
    // High energy + high density = pedal rarely used (clean articulation)

    // Calculate engagement probability
    float engagementProb = (1.0f - energy) * density;

    // Decide whether to use pedal at all
    if (engagementProb < 0.15f)
    {
        // Very low engagement - schedule long OFF period
        if (pedalDown)
        {
            pedalDown = false;
            double offDurationSec = 5.0 + rng.nextDouble() * 10.0;  // 5-15 seconds off
            nextPedalChangeSample = sampleCounter + (int64_t)(offDurationSec * sr);
        }
        else
        {
            nextPedalChangeSample = sampleCounter + (int64_t)(5.0 * sr);
        }
        return;
    }

    if (pedalDown)
    {
        // Pedal is down - schedule when to lift it
        // Low energy = long pedal down (many notes blend together)
        // High energy = short pedal down (minimal blending)
        double pedalDownDurationSec = juce::jmap((double)energy,
                                                  0.0, 1.0,
                                                  8.0, 0.5);  // 8 seconds to 0.5 seconds

        // Add some randomness (±30%)
        double variation = (rng.nextDouble() - 0.5) * 0.6 * pedalDownDurationSec;
        pedalDownDurationSec = juce::jmax(0.5, pedalDownDurationSec + variation);

        nextPedalChangeSample = sampleCounter + (int64_t)(pedalDownDurationSec * sr);
    }
    else
    {
        // Pedal is up - schedule when to press it
        // At low energy, short gaps between pedal phrases
        // At high energy, longer gaps (pedal rarely engaged)
        double pedalUpDurationSec = juce::jmap((double)energy,
                                                0.0, 1.0,
                                                1.0, 5.0);  // 1 second to 5 seconds

        // Add some randomness
        double variation = (rng.nextDouble() - 0.5) * 0.6 * pedalUpDurationSec;
        pedalUpDurationSec = juce::jmax(0.5, pedalUpDurationSec + variation);

        nextPedalChangeSample = sampleCounter + (int64_t)(pedalUpDurationSec * sr);
    }
}

void StringFieldMIDIProcessor::scheduleNextNote(double bpm, double ppqPos)
{
    // === PULSE MODE ===
    bool pulseEnabled = *apvts.getRawParameterValue("pulse") > 0.5f;

    if (pulseEnabled)
    {
        float tempo = *apvts.getRawParameterValue("tempo");
        float regularity = *apvts.getRawParameterValue("regularity");

        // Beat interval in seconds
        double beatInterval = 60.0 / juce::jmax(40.0f, tempo);

        // Regularity controls variance:
        // High regularity (1.0) = strict tempo, minimal variance (±5%)
        // Low regularity (0.0) = high variance (±100%), tempo not perceivable
        float varianceAmount = juce::jmap(regularity, 0.0f, 1.0f, 1.0f, 0.05f);

        // Random variance around beat interval
        double variance = (rng.nextDouble() - 0.5) * 2.0 * varianceAmount * beatInterval;
        double intervalSec = juce::jmax(0.05, beatInterval + variance);

        nextNoteOnSample = sampleCounter + (int64_t)(intervalSec * sr);
        return;
    }

    // === RATE MODE (original behavior) ===
    float rate = *apvts.getRawParameterValue("rate");
    float energy = *apvts.getRawParameterValue("energy");
    int memorySize = (int)*apvts.getRawParameterValue("memory");

    double baseInterval = 1.0 / juce::jmax(0.001f, rate);

    // RHYTHMIC MEMORY MODE: Higher memory = more repetition of recent rhythms
    // Low energy = strong rhythmic repetition (ostinato patterns)
    // High energy = weaker repetition (more varied rhythm)
    float memoryStrength = juce::jmap(energy, 0.0f, 1.0f, 1.0f, 0.3f);

    // No rhythm memory: use simple jitter
    if (memorySize <= 0 || recentIntervals.empty())
    {
        double jitter = (rng.nextDouble() - 0.5) * (energy * baseInterval);
        double intervalSec = juce::jmax(0.001, baseInterval + jitter);

        // Store in rhythm memory for future
        if (memorySize > 0)
        {
            recentIntervals.push_back(intervalSec);
            int rhythmMemorySize = juce::jmax(1, memorySize / 2);  // Rhythm memory is half of pitch memory
            if ((int)recentIntervals.size() > rhythmMemorySize)
                recentIntervals.pop_front();
        }

        nextNoteOnSample = sampleCounter + (int64_t)(intervalSec * sr);
        return;
    }

    // With rhythm memory: FAVOR repeating recent intervals (strength scaled by energy)
    int rhythmMemorySize = juce::jmax(1, memorySize / 2);
    float repeatProbability = memoryStrength * 0.75f;  // 75% at low energy, 22.5% at high energy

    double intervalSec;
    if (rng.nextFloat() < repeatProbability && !recentIntervals.empty())
    {
        // Pick from recent intervals (rhythmic ostinato)
        int memoryIndex = rng.nextInt(juce::Range<int>(0, (int)recentIntervals.size()));
        intervalSec = recentIntervals[memoryIndex];

        // Add slight variation (±10%) to avoid mechanical feel
        double variation = (rng.nextDouble() - 0.5) * 0.2 * intervalSec;
        intervalSec = juce::jmax(0.001, intervalSec + variation);
    }
    else
    {
        // Pick new interval (exploration)
        double jitter = (rng.nextDouble() - 0.5) * (energy * baseInterval);
        intervalSec = juce::jmax(0.001, baseInterval + jitter);
    }

    // Update rhythm memory
    recentIntervals.push_back(intervalSec);
    if ((int)recentIntervals.size() > rhythmMemorySize)
        recentIntervals.pop_front();

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
        juce::Optional<juce::AudioPlayHead::PositionInfo> posInfo = playhead->getPosition();
        if (posInfo.hasValue())
        {
            isPlaying = posInfo->getIsPlaying();
            auto bpmVal = posInfo->getBpm();
            if (bpmVal.hasValue())
                bpm = *bpmVal;
            auto ppqVal = posInfo->getPpqPosition();
            if (ppqVal.hasValue())
                ppqPos = *ppqVal;
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

    // === MIDI Learn / CC Processing ===
    for (const juce::MidiMessageMetadata metadata : midiMessages)
    {
        const juce::MidiMessage& message = metadata.getMessage();

        if (message.isController())
        {
            int ccNumber = message.getControllerNumber();
            int ccValue = message.getControllerValue();

            // MIDI Learn mode: map this CC to the learning parameter
            if (midiLearnEnabled && !midiLearnParameterID.isEmpty())
            {
                ccToParameterMap[ccNumber] = midiLearnParameterID;
                midiLearnEnabled = false;
                midiLearnParameterID = "";
            }
            // Normal mode: apply CC to mapped parameter
            else if (ccToParameterMap.find(ccNumber) != ccToParameterMap.end())
            {
                juce::String paramID = ccToParameterMap[ccNumber];
                auto* param = apvts.getParameter(paramID);

                if (param != nullptr)
                {
                    // Normalize CC value (0-127) to parameter range (0.0-1.0)
                    float normalizedValue = ccValue / 127.0f;
                    param->setValueNotifyingHost(normalizedValue);
                }
            }
        }
    }

    // === Sustain Pedal Parameter Change ===
    bool pedalParamOn = *apvts.getRawParameterValue("pedal") > 0.5f;
    if (pedalParamOn != lastPedalParamState)
    {
        // When disabling pedal: immediately release on all channels
        if (!pedalParamOn && lastPedalParamState)
        {
            for (int ch = 1; ch <= 16; ++ch)
                midiMessages.addEvent(juce::MidiMessage::controllerEvent(ch, 64, 0), 0);
            pedalDown = false;
            nextPedalChangeSample = 0;
        }
        // When enabling pedal: let the algorithm schedule it (judicious application)
        // No immediate action needed - scheduleNextNote will handle it

        lastPedalParamState = pedalParamOn;
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

    // Initialize schedulers if needed
    if (nextNoteOnSample <= blockStart)
        scheduleNextNote(bpm, ppqPos);

    // Check if sustain pedal is enabled
    int pedalEnabled = (int)*apvts.getRawParameterValue("pedal");

    if (pedalEnabled > 0)
    {
        if (nextPedalChangeSample <= blockStart)
        {
            float energy = *apvts.getRawParameterValue("energy");
            float density = *apvts.getRawParameterValue("density");
            schedulePedalChange(energy, density);
        }

        // 1. Handle sustain pedal changes
        if (nextPedalChangeSample >= blockStart && nextPedalChangeSample < blockEnd)
        {
            int offset = (int)(nextPedalChangeSample - blockStart);

            // Toggle pedal state and send CC 64
            pedalDown = !pedalDown;
            int pedalValue = pedalDown ? 127 : 0;

            // Send to all channels (sustain is global)
            for (int ch = 1; ch <= 16; ++ch)
            {
                midiMessages.addEvent(
                    juce::MidiMessage::controllerEvent(ch, 64, pedalValue),
                    offset
                );
            }

            // Schedule next pedal change
            float energy = *apvts.getRawParameterValue("energy");
            float density = *apvts.getRawParameterValue("density");
            schedulePedalChange(energy, density);
        }
    }

    // 2. Emit note-off if scheduled
    if (activeNote >= 0 &&
        noteOffSample >= blockStart &&
        noteOffSample < blockEnd)
    {
        int offset = (int)(noteOffSample - blockStart);
        midiMessages.addEvent(
            juce::MidiMessage::noteOff(activeChannel, activeNote),
            offset
        );
        activeNote = -1;
        noteOffSample = 0;
    }

    // 3. Emit note-on if scheduled (with density check)
    if (nextNoteOnSample >= blockStart && nextNoteOnSample < blockEnd)
    {
        float density = *apvts.getRawParameterValue("density");

        // Density probabilistic gating
        if (rng.nextFloat() <= density)
        {
            int center = (int)*apvts.getRawParameterValue("center");
            int spread = (int)*apvts.getRawParameterValue("spread");
            int baseVel = (int)*apvts.getRawParameterValue("vel");
            int numRoutes = (int)*apvts.getRawParameterValue("routes");
            float energy = *apvts.getRawParameterValue("energy");
            float articulation = *apvts.getRawParameterValue("articulation");

            int note = pickNote(center, spread);
            int vel = pickVelocity(baseVel);
            int channel = pickArticulation(numRoutes, articulation, energy);
            int offset = (int)(nextNoteOnSample - blockStart);

            // MONOPHONIC: Force note-off on previous note before starting new one
            if (activeNote >= 0)
            {
                // If switching channels, release sustain pedal on old channel first
                // This ensures true monophonic behavior across articulations
                if (channel != activeChannel && pedalDown)
                {
                    midiMessages.addEvent(
                        juce::MidiMessage::controllerEvent(activeChannel, 64, 0),
                        offset
                    );
                }

                midiMessages.addEvent(
                    juce::MidiMessage::noteOff(activeChannel, activeNote),
                    offset
                );
            }

            midiMessages.addEvent(
                juce::MidiMessage::noteOn(channel, note, (juce::uint8)vel),
                offset
            );

            // Schedule note-off
            double duration = calculateDuration(energy);
            noteOffSample = nextNoteOnSample + (int64_t)duration;
            activeNote = note;
            activeChannel = channel;
        }

        // Schedule next event
        scheduleNextNote(bpm, ppqPos);
    }

    sampleCounter += numSamples;
}

// === Pitch-Class Set Helper Functions ===

void StringFieldMIDIProcessor::parsePitchClassSet(const juce::String& pcString)
{
    pitchClassSet.clear();
    remainingPCs.clear();
    pcToMidiMap.clear();

    if (pcString.isEmpty())
        return;

    // Parse pitch-class notation: 0-9, A=10, B=11
    for (int i = 0; i < pcString.length(); ++i)
    {
        juce::juce_wchar ch = pcString[i];
        int pc = -1;

        if (ch >= '0' && ch <= '9')
            pc = ch - '0';
        else if (ch == 'A' || ch == 'a')
            pc = 10;
        else if (ch == 'B' || ch == 'b')
            pc = 11;

        if (pc >= 0 && pc <= 11)
        {
            // Avoid duplicates
            if (std::find(pitchClassSet.begin(), pitchClassSet.end(), pc) == pitchClassSet.end())
                pitchClassSet.push_back(pc);
        }
    }

    // Initialize remaining PCs (randomized order for exhaustion)
    remainingPCs = pitchClassSet;

    // Fisher-Yates shuffle using juce::Random
    for (int i = (int)remainingPCs.size() - 1; i > 0; --i)
    {
        int j = rng.nextInt(i + 1);
        std::swap(remainingPCs[i], remainingPCs[j]);
    }

    lastPCSetString = pcString;
}

void StringFieldMIDIProcessor::transformPitchClassSet()
{
    if (pitchClassSet.empty())
        return;

    int pcMode = (int)*apvts.getRawParameterValue("pcmode");

    if (pcMode == 1)
    {
        // Transpose only (Tn)
        int transposition = rng.nextInt(12);  // T0 to T11
        for (int& pc : pitchClassSet)
            pc = (pc + transposition) % 12;
    }
    else if (pcMode == 2)
    {
        // Transpose + Invert (TnI)
        bool invert = rng.nextBool();
        int transposition = rng.nextInt(12);

        if (invert)
        {
            for (int& pc : pitchClassSet)
                pc = (12 - pc + transposition) % 12;  // Inversion then transposition
        }
        else
        {
            for (int& pc : pitchClassSet)
                pc = (pc + transposition) % 12;
        }
    }

    // Re-shuffle for new exhaustion cycle
    remainingPCs = pitchClassSet;

    // Fisher-Yates shuffle using juce::Random
    for (int i = (int)remainingPCs.size() - 1; i > 0; --i)
    {
        int j = rng.nextInt(i + 1);
        std::swap(remainingPCs[i], remainingPCs[j]);
    }

    // Clear octave memory on transformation
    pcToMidiMap.clear();
}

int StringFieldMIDIProcessor::mapPCToMIDI(int pitchClass, int center, int spread)
{
    int lo = juce::jlimit(0, 127, center - spread);
    int hi = juce::jlimit(0, 127, center + spread);

    // Check if we've already assigned a MIDI note to this PC (octave memory)
    auto it = pcToMidiMap.find(pitchClass);
    if (it != pcToMidiMap.end())
    {
        int midiNote = it->second;
        // Make sure it's still in range
        if (midiNote >= lo && midiNote <= hi)
            return midiNote;
    }

    // Find all valid MIDI notes matching this pitch class within range
    std::vector<int> candidates;
    for (int note = lo; note <= hi; ++note)
    {
        if (note % 12 == pitchClass)
            candidates.push_back(note);
    }

    if (candidates.empty())
    {
        // Fallback: find closest note with this PC
        for (int note = center; note >= 0; --note)
            if (note % 12 == pitchClass) { candidates.push_back(note); break; }
        for (int note = center; note <= 127; ++note)
            if (note % 12 == pitchClass) { candidates.push_back(note); break; }
    }

    if (candidates.empty())
        return center;  // Emergency fallback

    // Randomly pick octave
    int chosenNote = candidates[rng.nextInt((int)candidates.size())];

    // Store in memory
    pcToMidiMap[pitchClass] = chosenNote;

    return chosenNote;
}

void StringFieldMIDIProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();

    // Add PC set string to state
    state.setProperty("pcset", lastPCSetString, nullptr);

    // Add MIDI CC mappings to state
    juce::String ccMappingsString;
    for (const auto& mapping : ccToParameterMap)
    {
        ccMappingsString += juce::String(mapping.first) + ":" + mapping.second + ";";
    }
    state.setProperty("ccmappings", ccMappingsString, nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void StringFieldMIDIProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        apvts.replaceState(state);

        // Restore PC set string
        if (state.hasProperty("pcset"))
        {
            juce::String pcString = state.getProperty("pcset").toString();
            parsePitchClassSet(pcString);
        }

        // Restore MIDI CC mappings
        if (state.hasProperty("ccmappings"))
        {
            ccToParameterMap.clear();
            juce::String ccMappingsString = state.getProperty("ccmappings").toString();
            juce::StringArray mappings = juce::StringArray::fromTokens(ccMappingsString, ";", "");

            for (const auto& mapping : mappings)
            {
                juce::StringArray parts = juce::StringArray::fromTokens(mapping, ":", "");
                if (parts.size() == 2)
                {
                    int ccNumber = parts[0].getIntValue();
                    juce::String paramID = parts[1];
                    ccToParameterMap[ccNumber] = paramID;
                }
            }
        }
    }
}

juce::AudioProcessorEditor* StringFieldMIDIProcessor::createEditor()
{
    return new StringFieldMIDIEditor(*this);
}

// === MIDI Learn Methods ===

void StringFieldMIDIProcessor::setMIDILearnMode(bool enabled, const juce::String& paramID)
{
    midiLearnEnabled = enabled;
    midiLearnParameterID = enabled ? paramID : "";
}

int StringFieldMIDIProcessor::getCCForParameter(const juce::String& paramID) const
{
    for (const auto& mapping : ccToParameterMap)
    {
        if (mapping.second == paramID)
            return mapping.first;
    }
    return -1;  // No mapping found
}

void StringFieldMIDIProcessor::clearCCMapping(const juce::String& paramID)
{
    for (auto it = ccToParameterMap.begin(); it != ccToParameterMap.end();)
    {
        if (it->second == paramID)
            it = ccToParameterMap.erase(it);
        else
            ++it;
    }
}

void StringFieldMIDIProcessor::clearAllCCMappings()
{
    ccToParameterMap.clear();
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StringFieldMIDIProcessor();
}
