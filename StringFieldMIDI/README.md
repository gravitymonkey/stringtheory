# String Field MIDI Generator

A JUCE-based Audio Unit MIDI FX plugin that generates algorithmic MIDI sequences inspired by string theory concepts and Morton Feldman's aesthetic of "continuity without causality."

**Current Version:** v0.1.3

---

## Building the Plugin

### Prerequisites

- **macOS** (tested on macOS 15.2+)
- **Xcode** (with command-line tools)
- **CMake** (installed via Homebrew: `brew install cmake`)
- **JUCE** (included as submodule)

### Build Steps

1. **Navigate to build directory:**
   ```bash
   cd /Users/jasonuechi/dev/stringtheory/StringFieldMIDI/build
   ```

2. **Build the AU plugin:**
   ```bash
   xcodebuild -configuration Release -scheme StringFieldMIDI_AU
   ```

3. **Plugin installs automatically to:**
   ```
   ~/Library/Audio/Plug-Ins/Components/String Field MIDI.component
   ```

4. **Validate the plugin:**
   ```bash
   auval -v aumi SfMi STth
   ```

   You should see `AU VALIDATION SUCCEEDED.`

### Troubleshooting Build

- **CMake not found:** Install via `brew install cmake`
- **Xcode errors:** Make sure Xcode command-line tools are installed: `xcode-select --install`
- **Validation fails:** Check Console.app for error messages from auval

---

## Using the Plugin

### Installation in Logic Pro

1. **Restart Logic Pro** after building (to scan new AU)
2. **Create a MIDI track**
3. **Add MIDI FX:**
   - Click "MIDI FX" slot
   - Navigate to: **StringTheory → String Field MIDI**
4. **Add an instrument** after the MIDI FX (e.g., Alchemy, sampler)
5. **Press Play** - the plugin generates MIDI notes algorithmically

---

## Parameters (15 Total)

### Core Generation Parameters

#### **Rate** (0.05 - 20.0 Hz, default: 2.0)
- **What it does:** Controls how frequently notes are generated (in Hertz)
- **Musical effect:**
  - Low (0.05 - 0.5 Hz): Sparse, meditative textures
  - Medium (1 - 4 Hz): Flowing melodic lines
  - High (10 - 20 Hz): Rapid, dense note clouds
- **Ignored when:** Pulse mode is ON (Tempo takes over)

#### **Density** (0.0 - 1.0, default: 0.25)
- **What it does:** Probabilistic gate - percentage of generated notes that actually sound
- **Musical effect:**
  - 0.0: Silence (all notes gated)
  - 0.25: Sparse, pointillistic
  - 0.5: Medium texture
  - 1.0: Every generated note plays

#### **Energy** (0.0 - 1.0, default: 0.2)
- **What it does:** Global "agitation" parameter affecting multiple dimensions
- **Musical effect:**
  - **Low Energy (0.0 - 0.3):**
    - Long note durations (1.2s)
    - Strong motivic repetition (if Memory > 0)
    - Minimal rhythmic jitter
    - Narrow articulation spread (focused timbre)
    - Feldman-esque sustained notes
  - **High Energy (0.7 - 1.0):**
    - Short staccato notes (0.08s)
    - Less repetition (more exploration)
    - High rhythmic variance
    - Wide articulation spread
    - Chaotic, agitated feel

### Pitch Parameters

#### **Center** (MIDI note 24 - 96, default: 60/Middle C)
- **What it does:** Sets the center of the pitch range
- **Musical effect:**
  - 36: Bass register
  - 60: Middle register (default)
  - 84: High register

#### **Spread** (0 - 36 semitones, default: 12)
- **What it does:** Range above and below Center
- **Musical effect:**
  - 0: Plays only the Center note
  - 12: One octave range (Center ± 12 semitones)
  - 36: Three octave range (wide melodic exploration)

### Expression Parameters

#### **Velocity** (1 - 127, default: 80)
- **What it does:** Base MIDI velocity with ±10 random variation
- **Musical effect:**
  - 40: Quiet, delicate (pp)
  - 80: Medium loud (mf)
  - 110: Loud (f)

#### **Seed** (1 - 99999, default: 1)
- **What it does:** Random number generator seed for deterministic output
- **Musical effect:**
  - Changing seed = different sequence with same other parameters
  - Same seed = identical playback (reproducible)
  - Use different seeds on different tracks for variation

### Multi-Articulation Parameters

#### **Num Routes** (1 - 16, default: 1)
- **What it does:** Number of MIDI channels to use for multi-articulation routing
- **Musical effect:**
  - 1: Monophonic, single MIDI channel
  - 4: Routes to channels 1-4 (e.g., 4 violin articulations)
  - 8: Routes to channels 1-8 (full articulation palette)
- **Setup:** Assign different articulations to each MIDI channel in your instrument

#### **Articulation** (0.0 - 1.0, default: 0.5)
- **What it does:** Center of weighted distribution for channel/articulation selection
- **Musical effect:**
  - 0.0: Favors channel 1 (typically legato, sustained)
  - 0.5: Favors middle channels (balanced)
  - 1.0: Favors highest channel (typically staccato, extreme)
- **Interaction with Energy:**
  - Low Energy = narrow spread (stays near Articulation center)
  - High Energy = wide spread (explores many articulations)

### Memory Parameter

#### **Memory** (0 - 16, default: 0)
- **What it does:** Size of motivic repetition kernel (FAVORS repeating recent material)
- **Musical effect:**
  - **0:** Pure random, no repetition
  - **8:** Repeats from last 8 notes (motivic loops emerge)
    - 80% repeat probability at low energy
    - 24% repeat probability at high energy
  - **16:** Repeats from last 16 notes (strong motivic development)
- **Rhythm memory:** Also tracks last N/2 intervals for rhythmic ostinatos
- **With low Energy:** Creates Steve Reich-like minimalist loops
- **With high Energy:** Thematic material with more exploration

### Pulse Mode Parameters

#### **Pulse** (OFF/ON, default: OFF)
- **What it does:** Switches from Rate-based to Tempo-based rhythm generation
- **Musical effect:**
  - OFF: Organic, unpredictable Feldman-style timing (uses Rate)
  - ON: Tempo-locked rhythms (uses Tempo and Regularity)

#### **Tempo** (40 - 360 BPM, default: 120)
- **What it does:** Sets pulse tempo in beats per minute
- **Musical effect:**
  - Only active when Pulse = ON
  - 60 BPM: Slow, meditative pulse
  - 120 BPM: Standard tempo
  - 320 BPM: Fast techno/gabber speeds
- **Ignored when:** Pulse = OFF

#### **Regularity** (0.0 - 1.0, default: 0.5)
- **What it does:** Controls rhythmic variance around the tempo
- **Musical effect:**
  - Only active when Pulse = ON
  - **1.0:** Strict metronomic (±5% variance) - clear pulse
  - **0.5:** Loose swing (±50% variance) - pulse perceptible but humanized
  - **0.0:** High variance (±100%) - tempo not perceptible
- **Ignored when:** Pulse = OFF

### Advanced Parameters

#### **Sustain Pedal** (OFF/ON, default: ON)
- **What it does:** Enables automatic sustain pedal (CC 64) based on Energy/Density
- **Musical effect:**
  - OFF: Dry, no pedal
  - ON: Algorithmic pedal application
    - Low Energy + High Density = long pedal washes (8 seconds down)
    - High Energy = minimal pedal (0.5 seconds, sparse)
- **Channel-specific:** When switching articulations, pedal releases on old channel

#### **PC Mode** (0-2, default: 0)
- **What it does:** Pitch class set constraint mode
- **Musical effect:**
  - 0: Chromatic (all 12 pitches available)
  - 1: Transpose only (cycles through PC set with transpositions)
  - 2: Transpose + Invert (T/I operations on PC set)
- **Requires:** PC Set text input (e.g., "0,2,4,5,7,9,11" for major scale)

---

## Workflow Examples

### Example 1: Minimalist Loops (Reich-style)
```
Rate:         3 Hz
Density:      0.8
Energy:       0.2 (long notes, strong loops)
Memory:       12 (repeat last 12 notes)
Center:       60
Spread:       7 (perfect 5th range)
Seed:         1

Result: Short melodic cells repeat and phase
```

### Example 2: Feldman-esque Sparse Texture
```
Rate:         0.5 Hz
Density:      0.15 (very sparse)
Energy:       0.15 (very low)
Memory:       0 (no repetition)
Center:       72
Spread:       24 (two octaves)
Pedal:        ON

Result: Long, sustained tones with wide intervallic leaps
```

### Example 3: Techno Pulse with Articulations
```
Pulse:        ON
Tempo:        140 BPM
Regularity:   0.9 (tight)
Density:      0.7
Routes:       4 (4 articulations)
Articulation: 0.7 (favor aggressive)
Energy:       0.6
Memory:       4 (short motivic loops)

Result: Fast rhythmic pattern cycling through articulations
```

### Example 4: Evolving Chaos
```
Rate:         5 Hz
Density:      0.5
Energy:       0.8 (high chaos)
Memory:       0 (pure random)
Center:       60
Spread:       36 (three octaves)
Routes:       8
Articulation: Automate 0.0 → 1.0 over time

Result: Dense, unpredictable texture morphing through timbres
```

---

## MIDI CC Control (Conductor Mode)

The plugin responds to MIDI CC messages, allowing one "conductor" track to control multiple instances simultaneously.

### Default CC Mappings

| CC# | Controller    | Parameter     |
|-----|---------------|---------------|
| 1   | Mod Wheel     | Energy        |
| 7   | Volume        | Density       |
| 11  | Expression    | Articulation  |
| 73  | Attack        | Memory        |
| 74  | Brightness    | Tempo         |
| 71  | Resonance     | Regularity    |

**See CONDUCTOR_MODE.md for detailed instructions.**

### Quick Conductor Setup (Logic Pro)

1. Create MIDI track (your "conductor")
2. Add CC automation (e.g., CC 1 for Energy)
3. Route MIDI to all tracks with the plugin
4. All instances respond together!

---

## Technical Details

- **Plugin Type:** Audio Unit MIDI FX (aumi)
- **Manufacturer Code:** STth
- **Plugin Code:** SfMi
- **Monophonic:** One note at a time (true solo performer behavior)
- **Multi-channel:** Routes to MIDI channels 1-N based on Num Routes
- **Sample-accurate:** MIDI generation scheduled at sample precision
- **State Saving:** All parameters + CC mappings save with project

---

## Tips & Tricks

1. **Layer multiple instances** with different Seeds for rich textures
2. **Use Track Stacks** to group and mix multiple instances
3. **Automate Energy** for dramatic Feldman → Chaos transformations
4. **Combine Pulse and Rate modes** by switching Pulse on/off over time
5. **Memory + Low Energy** creates recognizable motivic loops
6. **High Density + Low Energy + Pedal** = ambient washes
7. **Articulation automation** creates timbral evolution
8. **Hardware controller on CC 1** for live Energy performance

---

## Changelog

### v0.1.3 (Current)
- Added MIDI CC control (Conductor Mode)
- Flipped Memory logic (now FAVORS repetition instead of avoiding it)
- Added Pulse/Tempo/Regularity for tempo-locked rhythms
- Added Articulation parameter (decoupled from Energy)
- Increased Tempo max to 360 BPM
- Fixed monophonic channel-switching (no voice overlap)
- Improved sustain pedal behavior (channel-specific release)

### v0.1.2
- Added Memory parameter (motivic repetition)
- Added Multi-Articulation routing (Num Routes)
- Vintage analog UI with rotary knobs

### v0.1.1
- Added energy-weighted articulation routing
- Fixed monophonic voice layering bug

### v0.1.0
- Initial release
- Basic generative MIDI with Rate/Density/Energy
- Pitch class set support

---

## License

© 2025 StringTheory

---

## Support

For issues, questions, or feature requests, see the source code repository.
