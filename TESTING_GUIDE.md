# String Field MIDI Generator - Testing Guide

## Overview
This guide provides detailed test procedures for validating the v0.1 plugin in Logic Pro.

---

## Setup

1. **Open Logic Pro**
2. **Create New Project**
   - Empty Project → Software Instrument → Create
3. **Add Plugin**
   - Channel Strip → MIDI FX (slot above Instrument) → Audio Units → StringTheory → String Field MIDI
4. **Add Synthesizer**
   - Instrument slot → Choose any synth (recommended: ES2 or Alchemy)
   - Set synth to a simple, clean sound (sine wave or basic pad)

---

## TC1: Basic Generation

**Objective:** Verify plugin generates MIDI events at expected rate

**Setup:**
- Rate: 2.0 Hz
- Density: 1.0
- Energy: 0.5
- Center: 60
- Spread: 12
- Velocity: 80
- Seed: 1

**Procedure:**
1. Press Play in Logic Pro
2. Listen for approximately 2 notes per second
3. Verify notes are sounding
4. Stop playback

**Expected Result:**
- Notes generate at ~2 Hz (one note every 0.5 seconds)
- Notes are audible through the synth
- No hanging notes after stop

**Status:** ⬜ Pass / ⬜ Fail

**Notes:**
```


```

---

## TC2: Determinism

**Objective:** Verify same seed produces identical sequence

**Setup:**
- Rate: 2.0
- Density: 1.0
- Energy: 0.2
- Center: 60
- Spread: 12
- Velocity: 80
- Seed: 42

**Procedure:**
1. Press Play, let run for 8 bars
2. Note the sequence of pitches (record or memorize pattern)
3. Stop and rewind to bar 1
4. Press Play again
5. Verify sequence is identical

**Expected Result:**
- Exact same pitches in exact same order
- Same timing
- Same durations

**Status:** ⬜ Pass / ⬜ Fail

**Notes:**
```


```

---

## TC3: Density Parameter

**Objective:** Verify density controls event probability

**Setup:**
- Rate: 4.0 (fast rate to make density clear)
- Energy: 0.5
- Center: 60
- Spread: 12
- Velocity: 80
- Seed: 1

**Procedure:**
1. Set Density to 0.0
   - Press Play for 4 bars
   - Expected: Complete silence (no notes)

2. Set Density to 0.25
   - Press Play for 4 bars
   - Expected: ~25% of potential events (sparse)

3. Set Density to 0.5
   - Press Play for 4 bars
   - Expected: ~50% of potential events

4. Set Density to 1.0
   - Press Play for 4 bars
   - Expected: All events fire (~4 per second)

**Expected Result:**
- Density = 0.0: No notes
- Density increases: More notes
- Density = 1.0: Maximum note density

**Status:** ⬜ Pass / ⬜ Fail

**Notes:**
```


```

---

## TC4: Energy and Duration

**Objective:** Verify energy parameter affects note duration

**Setup:**
- Rate: 1.0 (slow for clarity)
- Density: 1.0
- Center: 60
- Spread: 0 (same note for consistency)
- Velocity: 80
- Seed: 1

**Procedure:**
1. Set Energy to 0.0 (low)
   - Press Play for 4 bars
   - Listen to note durations
   - Expected: Long, sustained notes (~1.2 seconds each)

2. Set Energy to 0.5 (medium)
   - Press Play for 4 bars
   - Expected: Medium duration notes (~0.6 seconds)

3. Set Energy to 1.0 (high)
   - Press Play for 4 bars
   - Expected: Very short, staccato notes (~0.08 seconds)

**Expected Result:**
- Low energy = Feldman-ish long notes
- High energy = Pointillistic short notes
- Clear audible difference between settings

**Status:** ⬜ Pass / ⬜ Fail

**Notes:**
```


```

---

## TC5: Transport Stop

**Objective:** Verify no hanging notes when transport stops

**Setup:**
- Rate: 2.0
- Density: 1.0
- Energy: 0.1 (very long notes)
- Center: 60
- Spread: 12
- Velocity: 80
- Seed: 1

**Procedure:**
1. Press Play
2. Let several long notes start (energy is low, so notes are sustained)
3. Press Stop while notes are still sounding
4. Listen carefully

**Expected Result:**
- All notes immediately stop when transport stops
- No hanging notes or stuck MIDI
- Complete silence after stop

**Status:** ⬜ Pass / ⬜ Fail

**Notes:**
```


```

---

## TC6: Register Parameters

**Objective:** Verify Center and Spread control pitch range

**Setup:**
- Rate: 3.0 (fast to hear many pitches)
- Density: 1.0
- Energy: 0.3
- Velocity: 80
- Seed: 1

**Procedure:**
1. **Test Center:**
   - Spread: 0
   - Center: 48 (C3) - Play 4 bars
     Expected: All notes are C3
   - Center: 72 (C5) - Play 4 bars
     Expected: All notes are C5

2. **Test Spread:**
   - Center: 60 (C4)
   - Spread: 0 - Play 4 bars
     Expected: All notes are C4 (60)
   - Spread: 12 - Play 4 bars
     Expected: Notes range from C3 (48) to C5 (72)
   - Spread: 24 - Play 4 bars
     Expected: Notes range from C2 (36) to C6 (84)

**Expected Result:**
- Center controls middle pitch
- Spread = 0: single note only
- Spread increases: wider pitch range
- All notes within center ± spread

**Status:** ⬜ Pass / ⬜ Fail

**Notes:**
```


```

---

## TC7: Seed Variation

**Objective:** Verify different seeds produce different sequences

**Setup:**
- Rate: 2.0
- Density: 1.0
- Energy: 0.3
- Center: 60
- Spread: 12
- Velocity: 80

**Procedure:**
1. Seed: 1
   - Press Play for 8 bars
   - Note the sequence (record or write down first 5 notes)

2. Stop and rewind to bar 1

3. Seed: 100
   - Press Play for 8 bars
   - Note the sequence
   - Expected: Different from Seed 1

4. Stop and rewind to bar 1

5. Seed: 1 (again)
   - Press Play for 8 bars
   - Expected: Matches original Seed 1 sequence

**Expected Result:**
- Seed 1 ≠ Seed 100 (different sequences)
- Seed 1 (second time) = Seed 1 (first time) (deterministic)

**Status:** ⬜ Pass / ⬜ Fail

**Notes:**
```


```

---

## Aesthetic Test: Feldman Mode

**Objective:** Verify the plugin can create Feldman-ish sparse, contemplative textures

**Setup (Feldman Aesthetic):**
- Rate: 0.8 Hz (very slow)
- Density: 0.20 (very sparse)
- Energy: 0.15 (very long notes)
- Center: 55 (low-mid register)
- Spread: 7 (narrow bandwidth)
- Velocity: 65 (soft)
- Seed: Any

**Synth Settings:**
- Use a soft, clean sound (Alchemy: Warm Pad, or ES2: Simple Sine)
- Add reverb (medium to long tail)

**Procedure:**
1. Press Play
2. Listen for at least 2 minutes
3. Observe the character:
   - Sparse events (not crowded)
   - Long, sustained notes
   - Quiet dynamics
   - Narrow pitch range
   - No obvious pattern or development
   - Contemplative, "floating" quality

**Expected Aesthetic:**
- Sounds like late Feldman
- No teleological progression
- Events feel independent yet coherent
- Time feels suspended
- "Low-energy statistical mechanics"

**Status:** ⬜ Pass / ⬜ Fail

**Notes:**
```


```

---

## Test Summary

| Test | Status | Notes |
|------|--------|-------|
| TC1: Basic Generation | ⬜ | |
| TC2: Determinism | ⬜ | |
| TC3: Density | ⬜ | |
| TC4: Energy/Duration | ⬜ | |
| TC5: Transport Stop | ⬜ | |
| TC6: Register | ⬜ | |
| TC7: Seed Variation | ⬜ | |
| Aesthetic Test | ⬜ | |

---

## Known Issues (To Be Discovered)

Document any unexpected behavior here:

```




```

---

## Recommendations for v0.2/v0.3

Based on testing, note features that would be valuable:

```




```
