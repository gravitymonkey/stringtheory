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
| **Energy** | 0.0 - 1.0 | Controls duration and jitter (low = long notes, high = short) |
| **Center** | 24 - 96 (MIDI) | Center pitch of the field |
| **Spread** | 0 - 36 semitones | Bandwidth of pitch selection |
| **Velocity** | 1 - 127 | Base velocity with ±10 variation |
| **Seed** | 1 - 99999 | RNG seed for deterministic sequences |

## Philosophy

This plugin uses string theory as a **constraint engine** to create:

- **States, not objects** - Notes are vibrational modes, not discrete symbols
- **Time as field** - No teleological progression, only parameter drift
- **Continuity without causality** - Events don't develop toward goals
- **Low-energy statistical mechanics** - Sparse, quiet, non-teleological music

Inspired by Morton Feldman's removal of causality without removing precision.

## Version Status

**v0.1 (Current):** Monophonic Live Generator ✅
- Sample-accurate MIDI generation
- 7 parameter control
- Deterministic seeded RNG
- Transport integration
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
