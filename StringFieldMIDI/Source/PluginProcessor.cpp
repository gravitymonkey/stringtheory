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

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "routes", "Num Routes", 1, 16, 1));

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

int StringFieldMIDIProcessor::pickArticulation(int numRoutes, float energy)
{
    // Energy-weighted articulation selection
    // Low energy (0.0) -> Channel 1 (legato, sustained)
    // High energy (1.0) -> Channel numRoutes (staccato, flutter, extreme)

    if (numRoutes <= 1)
        return 1;

    // Map energy to channel range (0 to numRoutes-1)
    float channelFloat = energy * (float)(numRoutes - 1);

    // Add jitter (±40% of range) for organic variation
    float jitterRange = 0.4f * (float)(numRoutes - 1);
    float jitter = (rng.nextFloat() - 0.5f) * jitterRange;
    channelFloat += jitter;

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

void StringFieldMIDIProcessor::scheduleNextNote(double bpm, double ppqPos)
{
    float rate = *apvts.getRawParameterValue("rate");
    float energy = *apvts.getRawParameterValue("energy");

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
            juce::MidiMessage::noteOff(activeChannel, activeNote),
            offset
        );
        activeNote = -1;
        noteOffSample = 0;
    }

    // 2. Emit note-on if scheduled (with density check)
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

            int note = pickNote(center, spread);
            int vel = pickVelocity(baseVel);
            int channel = pickArticulation(numRoutes, energy);
            int offset = (int)(nextNoteOnSample - blockStart);

            // MONOPHONIC: Force note-off on previous note before starting new one
            if (activeNote >= 0)
            {
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

juce::AudioProcessorEditor* StringFieldMIDIProcessor::createEditor()
{
    return new StringFieldMIDIEditor(*this);
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StringFieldMIDIProcessor();
}
