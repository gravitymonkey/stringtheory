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
| **Energy** | 0.0 - 1.0 | Controls duration, repetition, AND articulation (low = Feldman mode: long notes + repetition + sustained, high = short + varied + agitated) |
| **Center** | 24 - 96 (MIDI) | Center pitch of the field |
| **Spread** | 0 - 36 semitones | Bandwidth of pitch selection |
| **Velocity** | 1 - 127 | Base velocity with ±10 variation |
| **Seed** | 1 - 99999 | RNG seed for deterministic sequences |
| **Num Routes** | 1 - 16 | Number of articulation channels (see Multi-Articulation Routing below) |
| **Memory** | 0 - 16 | Anti-repetition strength (0 = off, 8 = moderate, 16 = strong Feldman mode) |
| **PC Mode** | 0 - 2 | Pitch-class set mode (0 = off/chromatic, 1 = transpose, 2 = transpose+invert) |
| **Sustain Pedal** | 0 - 1 | Enable/disable automatic sustain pedal (0 = off, 1 = on) |

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

## Memory: Feldman's Fragile Memory

The **Memory** parameter implements anti-repetition for both pitch and rhythm, creating Morton Feldman's aesthetic of "continuity without repetition."

### How It Works

**Memory = 0:** No memory (pure random)
- Notes and rhythms can repeat immediately
- Good for exploring raw randomness

**Memory = 1-8:** Moderate anti-repetition
- Avoids repeating the last N notes
- Avoids repeating similar rhythmic intervals (within 15%)
- Creates subtle variety without obvious avoidance

**Memory = 9-16:** Strong Feldman mode
- Large memory buffer prevents obvious patterns
- Notes recur, but never feel repetitive
- Rhythms breathe without metric predictability

### Dual Memory System

1. **Pitch Memory:** Tracks last N notes, avoids immediate repetition
   - Memory size = Memory parameter value
   - Example: Memory=8 → won't repeat any of the last 8 notes

2. **Rhythm Memory:** Tracks last N/2 inter-onset intervals
   - Memory size = Memory parameter / 2
   - Avoids intervals within 15% of recent ones
   - Example: Memory=8 → rhythm memory of last 4 intervals

### Musical Effect

**Memory = 0:**
```
Pitch:  C E C G C E C (C repeats frequently)
Rhythm: 0.5s 0.5s 0.5s 0.5s (periodic)
```

**Memory = 8:**
```
Pitch:  C E G A F# D Bb Eb (no immediate repeats)
Rhythm: 0.5s 0.7s 0.4s 0.6s 0.45s (varied, non-periodic)
```

**Result:** Continuity (same pitch/tempo range) without repetition (no phrases recur)

This is exactly Feldman's technique - events feel related but never identical.

## Energy-Memory Interaction: True Feldman Mode

The **Energy** parameter scales the effectiveness of the **Memory** system, creating a continuum from static repetition to active variation.

### How It Works

**Memory strength = 0.2 + (0.8 × Energy)**

- **Low Energy (0.0-0.3):** Memory system weakened → **embraces repetition**
  - Pitch: Same notes repeat often (hypnotic, static)
  - Rhythm: Regular pulse with minimal variation
  - Duration: Long sustained notes (1.2 seconds)
  - Articulation: Routes to legato/sustained channels
  - **Result:** Classic Feldman stasis - suspended, contemplative

- **Medium Energy (0.4-0.6):** Balanced memory → moderate variation
  - Pitch: Some repetition, some variety
  - Rhythm: Gently varied pulse
  - Duration: Medium note lengths
  - Articulation: Mixed sustained and detached

- **High Energy (0.7-1.0):** Full memory strength → **avoids repetition**
  - Pitch: Strong anti-repetition (diverse melodic material)
  - Rhythm: Actively varied, unpredictable
  - Duration: Short staccato notes (0.08 seconds)
  - Articulation: Routes to agitated/extreme channels
  - **Result:** Active, restless, constantly shifting

### Musical Example

**Settings:** Memory=8, Spread=12, Center=60

**Low Energy (0.15):**
```
Pitch:  C C C E E C C G C E E E (hypnotic repetition)
Rhythm: 0.5s 0.5s 0.5s 0.5s 0.5s (nearly regular)
Duration: 1.2s per note (long, sustained)
```

**High Energy (0.85):**
```
Pitch:  C E G# F A D Bb F# Eb B (no repeats, all different)
Rhythm: 0.3s 0.6s 0.4s 0.7s 0.35s (constantly shifting)
Duration: 0.08s per note (short, percussive)
```

### Why This Matters

Morton Feldman's late work embraces **static repetition** as an aesthetic:
- Long sustained tones
- Pitch insistence (repeating the same note many times)
- Hypnotic, meditative quality
- "Time suspended" rather than "time progressing"

By setting **Energy low**, you get TRUE Feldman mode where repetition isn't avoided—it's celebrated.

## Automatic Sustain Pedal: Creating Harmonic Washes

The plugin can automatically control the **sustain pedal (CC 64)** based on **Energy** and **Density**, creating natural chord blending.

### Enable/Disable

The **Sustain Pedal** parameter turns the automatic pedal feature on or off:
- **1 (ON)**: Automatic pedal control enabled (default)
- **0 (OFF)**: No pedal messages sent (use for instruments that don't respond to CC 64)

### How It Works

**Pedal engagement = (1.0 - Energy) × Density**

The sustain pedal **crosses note boundaries**, allowing multiple notes to ring together and create chords/harmonic washes.

### Low Energy (0.15) + Low Density (0.25):
- Engagement: ~21%
- **Pedal down duration:** ~8 seconds (spans many notes)
- **Pedal up duration:** ~1 second (brief gaps)
- **Result:** Notes blend into sustained chords, creating Feldman's characteristic washes of sound

### High Energy (0.85) + High Density (0.8):
- Engagement: ~12%
- **Pedal down duration:** ~0.5 seconds (brief touches)
- **Pedal up duration:** ~5 seconds (mostly off)
- **Result:** Clean, articulated notes with minimal blending

### Example Timeline (Energy=0.15, Density=0.25):
```
Time:   0s    2s    4s    6s    8s    9s    11s   13s   15s   23s   24s
Pedal:  DOWN---------------------UP---DOWN--------------UP---DOWN----------
Notes:  C     E     C     G      E    C     F     D     C    Bb    G
Result: [   C-E-C-G chord wash  ]     [  C-F-D chord  ]     [  Bb-G...
```

### Why This Matters

At **low energy**, the long pedal-down periods create:
- **Harmonic clouds** - multiple notes sustaining together
- **Blurred boundaries** - no clear separation between events
- **Ambient washes** - especially beautiful with piano, strings, pads
- **Feldman's vertical density** - his late works often have sustained harmonic fields

At **high energy**, minimal pedal use creates:
- **Clean articulation** - each note distinct
- **Rhythmic clarity** - percussive, defined
- **No harmonic buildup** - dry, focused

### Musical Applications

**Piano/Acoustic Instruments:**
- Low energy sustain creates natural resonance and harmonic overtones
- Mimics holding the sustain pedal down while playing sparse chords

**Strings/Pads:**
- Sustain prevents ADSR retriggering, creating smooth legato washes
- Multiple notes blend into evolving textures

**Synths:**
- Works with any instrument that responds to CC 64
- Can create ambient drones from individual note events

**No extra parameter needed** - the pedal behavior is musically intelligent and automatic!

## Philosophy

This plugin uses string theory as a **constraint engine** to create:

- **States, not objects** - Notes are vibrational modes, not discrete symbols
- **Time as field** - No teleological progression, only parameter drift
- **Continuity without causality** - Events don't develop toward goals
- **Low-energy statistical mechanics** - Sparse, quiet, non-teleological music

Inspired by Morton Feldman's removal of causality without removing precision.

## Version Status

**v0.1.4 (Current):** Automatic Sustain Pedal ✅
- **NEW:** Automatic sustain pedal (CC 64) controlled by Energy/Density
- **NEW:** Sustain Pedal enable/disable parameter (default ON)
- **NEW:** Pedal crosses note boundaries to create chords/harmonic washes
- **NEW:** Low energy = long pedal phrases (8 sec), high energy = minimal pedal
- Energy scales Memory effectiveness (low energy = embrace repetition, true Feldman mode)
- Vintage analog rotary knob UI with brass aesthetic
- PC Set mode controls visible in UI
- Sample-accurate MIDI generation
- 11 parameter control (Memory, PC Mode, Sustain Pedal enable, PC Set text input)
- Energy controls: duration, repetition, articulation routing, sustain pedal
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
