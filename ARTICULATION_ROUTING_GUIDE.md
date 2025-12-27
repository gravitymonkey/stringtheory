# Multi-Articulation Routing Guide
## String Field MIDI Generator v0.1.1

---

## Overview

The **Num Routes** parameter enables energy-weighted routing across multiple MIDI channels, allowing you to create timbral variety where different articulations emerge organically based on the Energy parameter.

---

## Quick Setup

### 1. Prepare Your Articulations

Organize your instrument articulations in order from **smoothest to most agitated**:

```
Channel 1: Legato / Sustained / Soft          (Low Energy)
Channel 2: Normal / Moderate
Channel 3: Marcato / Accented
Channel 4: Staccato / Short
Channel 5: Staccatissimo / Very short
Channel 6: Flutter tongue / Tremolo / Extreme  (High Energy)
```

### 2. Configure in Logic Pro

**Option A: Multi-Channel Instrument**
1. Create Software Instrument track
2. Add String Field MIDI to MIDI FX slot
3. Load a multi-channel instrument (Kontakt, VSL, etc.)
4. Configure each articulation to respond to channels 1, 2, 3...
5. Set **Num Routes** to match your articulation count

**Option B: Multiple Tracks**
1. Create Software Instrument track with String Field MIDI
2. Create additional instrument tracks for each articulation
3. Set each track's MIDI input to "String Field MIDI track"
4. Set each track to listen to specific MIDI channel (1, 2, 3...)
5. Set **Num Routes** to match track count

### 3. Set Parameters

```
Num Routes: [number of articulations, e.g., 4]
Energy: 0.2 - 0.8 (automate this for evolving timbres)
```

---

## How Energy Maps to Channels

The plugin uses this algorithm:

```
Target Channel = (Energy × (NumRoutes - 1)) + jitter
Jitter = ±40% of channel range
Final Channel = clamped to [1, NumRoutes]
```

### Examples with 4 Routes:

| Energy | Primary Channel | Likely Range |
|--------|----------------|--------------|
| 0.0    | 1              | 1-2          |
| 0.15   | 1              | 1-2          |
| 0.35   | 2              | 1-3          |
| 0.5    | 2-3            | 2-3          |
| 0.65   | 3              | 2-4          |
| 0.85   | 4              | 3-4          |
| 1.0    | 4              | 3-4          |

The jitter ensures organic variation—no rigid thresholds.

---

## Instrument-Specific Examples

### Woodwinds (Flute, Clarinet, Oboe)

**4 Routes:**
1. Legato
2. Normal sustain
3. Staccato
4. Flutter tongue

**6 Routes:**
1. Legato pp
2. Legato mf
3. Normal sustain
4. Marcato
5. Staccato
6. Flutter tongue / Air tones

### Strings (Violin, Cello)

**5 Routes:**
1. Sul tasto (soft, over fingerboard)
2. Ordinario legato
3. Sul ponticello (bright, near bridge)
4. Spiccato (bouncing bow)
5. Col legno / Tremolo

### Brass (Trumpet, Horn)

**4 Routes:**
1. Straight tone, soft
2. Normal sustain
3. Accent / Sforzando
4. Flutter tongue / Growl

### Percussion (Vibraphone, Marimba)

**3 Routes:**
1. Soft mallets
2. Medium mallets
3. Hard mallets

---

## Musical Strategies

### Static Energy (Single Timbral Field)
- Set Energy to 0.2: Mostly legato, sustained (Feldman-ish)
- Set Energy to 0.7: Mostly staccato, agitated

### Slowly Evolving Energy (Gradual Timbral Shift)
- Automate Energy from 0.1 → 0.9 over 5 minutes
- Creates seamless transition from sustained to percussive

### Energy as Secondary Parameter
- Use low Density (0.2) + varying Energy (0.1-0.8)
- Sparse events with unpredictable timbres
- Very effective for quiet, pointillistic textures

### Energy Cycling
- Automate Energy in a slow sine wave (0.2 ↔ 0.6, period = 2 minutes)
- Creates breathing, cyclical timbre without repetition

---

## Troubleshooting

### "I'm not hearing different articulations"

**Check:**
1. Num Routes is set correctly (not 1)
2. Your instrument is configured for multi-channel input
3. Each articulation is assigned to the correct MIDI channel
4. Energy is varying (if stuck at 0.1, you'll only hear channel 1)

### "The articulations don't match the energy levels I expected"

**Remember:**
- ±40% jitter means overlap between channels
- Energy 0.5 might trigger channels 1, 2, or 3 (not just channel 2)
- This is intentional—organic variation, not rigid mapping

### "Some articulations never play"

**Possible causes:**
1. Num Routes is set lower than your actual channel count
2. Energy range is too narrow (e.g., always 0.1-0.3 won't reach channel 6)
3. Try automating Energy across full range (0.0-1.0)

---

## Advanced Techniques

### Heterogeneous Ensembles

Use different instrument families on different channels:

```
Channel 1: Flute legato
Channel 2: Clarinet legato
Channel 3: Violin tremolo
Channel 4: Vibraphone soft
```

Low energy = woodwinds, high energy = strings/percussion

### Spectral Routing

Map channels to harmonic content rather than articulation:

```
Channel 1: Pure sine (fundamental only)
Channel 2: Sine + 2nd harmonic
Channel 3: Sine + 2nd + 3rd
Channel 4: Full harmonic series
```

Energy becomes **spectral richness**

### Spatial Routing

Combine with panning:

```
Channel 1: Far left
Channel 2: Mid-left
Channel 3: Mid-right
Channel 4: Far right
```

Energy becomes **spatial position**

---

## Philosophical Note

This feature extends the **multi-dimensional state space** concept:

- Feldman removed thematic development but kept precise control
- String theory describes hidden dimensions affecting observable physics
- Our "hidden dimension" = articulation/timbre
- Energy doesn't just affect duration—it affects the **vibrational quality** itself

Two notes at the same pitch can exist in radically different dimensional states (legato vs. flutter), creating **continuity without identity**.

---

## Version

Guide for String Field MIDI v0.1.1
December 27, 2025
