# String Field MIDI Generator

An Audio Unit MIDI Effect plugin that applies string theory concepts to algorithmic composition, creating music inspired by Morton Feldman's aesthetic.

## Quick Start

### Installation
The plugin is already installed at:
```
~/Library/Audio/Plug-Ins/Components/String Field MIDI.component
```

### Usage in Logic Pro
1. Create a Software Instrument track
2. Add **MIDI FX → Audio Units → StringTheory → String Field MIDI**
3. Add an instrument (ES2, Alchemy, etc.) in the Instrument slot
4. Press Play

### Recommended Starting Settings (Feldman-ish)
- **Rate:** 1.0 - 2.0 Hz (sparse events)
- **Density:** 0.20 - 0.30 (breathing texture)
- **Energy:** 0.15 - 0.25 (long, sustained notes)
- **Center:** 48 - 72 (low to mid register)
- **Spread:** 5 - 12 semitones (narrow bandwidth)
- **Velocity:** 60 - 80 (soft dynamics)
- **Seed:** Any value (each seed = unique sequence)

## Parameters

| Parameter | Range | Description |
|-----------|-------|-------------|
| **Rate** | 0.05 - 20.0 Hz | Event generation frequency |
| **Density** | 0.0 - 1.0 | Probability of event firing (0 = silent, 1 = all events) |
| **Energy** | 0.0 - 1.0 | Controls duration, jitter, AND articulation (low = long notes + sustained, high = short + agitated) |
| **Center** | 24 - 96 (MIDI) | Center pitch of the field |
| **Spread** | 0 - 36 semitones | Bandwidth of pitch selection |
| **Velocity** | 1 - 127 | Base velocity with ±10 variation |
| **Seed** | 1 - 99999 | RNG seed for deterministic sequences |
| **Num Routes** | 1 - 16 | Number of articulation channels (see Multi-Articulation Routing below) |

## Multi-Articulation Routing

The plugin supports **energy-weighted articulation selection** across multiple MIDI channels, allowing you to create timbral variety within a single generative stream.

### How It Works

1. **Set Num Routes** to the number of different articulations you want (1-16)
2. Load different instrument articulations in Logic Pro on separate MIDI channels
3. The plugin automatically routes notes to different channels based on **Energy** level

### Channel Ordering (Critical!)

**Arrange your articulations from smoothest to most agitated:**

- **Channel 1:** Legato, sustained tones, soft attacks (Low Energy)
- **Channel 2:** Normal sustain
- **Channel 3:** Moderate articulation
- **...**
- **Channel N-1:** Staccato, short attacks
- **Channel N:** Flutter tongue, tremolo, noise, extreme techniques (High Energy)

### Example Setup in Logic Pro

**Flute with 4 Articulations (Num Routes = 4):**
1. Channel 1: Flute Legato (Energy 0.0-0.3)
2. Channel 2: Flute Normal (Energy 0.3-0.6)
3. Channel 3: Flute Staccato (Energy 0.6-0.85)
4. Channel 4: Flute Flutter Tongue (Energy 0.85-1.0)

As Energy varies, the plugin will organically shift between articulations, with ±40% jitter to prevent rigid mapping.

### Setting Up in Logic Pro

1. Create a Software Instrument track
2. Add **MIDI FX → String Field MIDI**
3. Add multiple instrument plugins in the **Instrument** slot:
   - Use **Multi-Channel** instruments, OR
   - Route to separate tracks using **External MIDI**
4. Configure each instrument to listen to channels 1, 2, 3, etc.
5. Set **Num Routes** to match your articulation count
6. Automate **Energy** to explore different timbral territories

### Musical Effect

- **Low Energy (0.1-0.3):** Sustained, contemplative, mostly legato (Feldman-ish)
- **Medium Energy (0.4-0.6):** Mixed articulations, organic variation
- **High Energy (0.7-0.9):** Agitated, percussive, extreme techniques

This creates a **multi-dimensional state space** where pitch, duration, density, AND timbre all evolve together.

## Philosophy

This plugin uses string theory as a **constraint engine** to create:

- **States, not objects** - Notes are vibrational modes, not discrete symbols
- **Time as field** - No teleological progression, only parameter drift
- **Continuity without causality** - Events don't develop toward goals
- **Low-energy statistical mechanics** - Sparse, quiet, non-teleological music

Inspired by Morton Feldman's removal of causality without removing precision.

## Version Status

**v0.1.1 (Current):** Monophonic Live Generator with Multi-Articulation ✅
- Sample-accurate MIDI generation
- 8 parameter control (added Num Routes)
- Energy-weighted articulation routing across 1-16 MIDI channels
- Deterministic seeded RNG
- Transport integration
- Truly monophonic (fixed voice layering bug)
- Validated and ready to use

**v0.2 (Planned):** MIDI Export
- Export generated sequences as .mid files
- Event logging with timestamps

**v0.3 (Planned):** Polyphony & Advanced Features
- Multi-voice generation
- State engine with drift
- Memory kernel (avoid repetition)
- Loop handling
- Advanced parameters

## Building from Source

### Prerequisites
- macOS
- Xcode with Command Line Tools
- CMake
- JUCE framework (included via submodule)

### Build Steps
```bash
cd StringFieldMIDI
mkdir build && cd build
cmake .. -G Xcode
xcodebuild -configuration Release -scheme StringFieldMIDI_AU
```

Plugin will be automatically installed to `~/Library/Audio/Plug-Ins/Components/`

### Validate
```bash
auval -v aumi SfMi STth
```

## Documentation

- **StringFieldMIDI_Development_Guide.md** - Complete development specification
- **BUILD_SUMMARY.md** - v0.1 build details and testing checklist

## License

Proprietary - For development use

## Author

Built following the String Field MIDI Generator development specification.
