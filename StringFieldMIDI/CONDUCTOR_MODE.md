# Conductor Mode - MIDI CC Control

The String Field MIDI Generator supports **MIDI CC control** for creating a "conductor" that controls multiple instances simultaneously.

## How It Works

Any MIDI CC messages sent to the plugin will control the mapped parameters across **all instances**. This allows you to:
- Control multiple tracks from one automation lane
- Use hardware controllers to perform live
- Create evolving textures that change globally

---

## TL;DR - Quick Start

**30-second setup:**
1. Enable **IAC Driver** in Audio MIDI Setup (one-time)
2. Create **External MIDI track** in Logic
3. Add CC automation (e.g., CC 22 for Energy: 0→127)
4. Set track **Output: IAC Driver Bus 1** (press I to show Inspector)
5. On plugin tracks: set **Input: IAC Driver Bus 1**, enable input monitoring
6. Play → all instances morph together!

**Can't find Output?** You need an **External MIDI** track, not Software Instrument.

---

## Signal Flow

Here's how the conductor system routes MIDI:

```
┌─────────────────────────────────────────────────────────────┐
│ CONDUCTOR TRACK (External MIDI)                             │
│  - CC Automation (e.g., CC 22: 0 → 127)                    │
│  - Output: IAC Driver Bus 1                                 │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
          ┌──────────────────────┐
          │   IAC Driver Bus 1   │  (Virtual MIDI Bus)
          └──────────┬───────────┘
                     │
         ┌───────────┴───────────┬────────────┬──────────────┐
         ▼                       ▼            ▼              ▼
┌─────────────────┐    ┌─────────────────┐  ┌─────────────────┐
│ Plugin Track 1  │    │ Plugin Track 2  │  │ Plugin Track 3  │
│ Input: IAC Bus1 │    │ Input: IAC Bus1 │  │ Input: IAC Bus1 │
│ [SF MIDI Plugin]│    │ [SF MIDI Plugin]│  │ [SF MIDI Plugin]│
│ → Instrument    │    │ → Instrument    │  │ → Instrument    │
└─────────────────┘    └─────────────────┘  └─────────────────┘

Result: All instances receive the same CC messages and change together!
```

---

## Default CC Mappings

Out of the box, these MIDI CC numbers are mapped using **unused CC range (20-31)** to avoid conflicts:

| CC Number | Parameter      | Use Case                          |
|-----------|----------------|-----------------------------------|
| **20**    | Rate           | Note generation rate control      |
| **21**    | Density        | Texture density control           |
| **22**    | Energy         | Feldman → Chaos morphing          |
| **23**    | Center         | Pitch center shifts               |
| **24**    | Spread         | Pitch range expansion/contraction |
| **25**    | Velocity       | Dynamics control                  |
| **26**    | Memory         | Motivic repetition strength       |
| **27**    | Articulation   | Timbre/technique selection        |
| **28**    | Pulse          | Pulse mode on/off                 |
| **29**    | Tempo          | Pulse tempo changes               |
| **30**    | Regularity     | Rhythmic strictness               |

---

## Setup in Logic Pro

### Method 1: IAC Driver (Recommended)

#### One-Time Setup: Enable IAC Driver

The IAC (Inter-Application Communication) Driver is a virtual MIDI bus built into macOS:

1. Open **Audio MIDI Setup**:
   - Go to `/Applications/Utilities/Audio MIDI Setup`
   - Or press Cmd+Space and type "Audio MIDI Setup"

2. Open MIDI Studio:
   - Click **Window** menu → **Show MIDI Studio**

3. Enable IAC Driver:
   - Double-click the **IAC Driver** icon
   - Check the box: **"Device is online"**
   - Click **Apply**
   - You can close Audio MIDI Setup now

You only need to do this once per Mac.

---

#### Per-Project Setup: Create Conductor Track

1. **Create External MIDI Track** (the conductor):
   - In Logic Pro, click **Track** → **New Track...** (or Cmd+Option+N)
   - Select **External MIDI** (NOT Software Instrument!)
   - Click **Create**
   - Name it "Conductor" (optional but helpful)

   **Why External MIDI?** Only External MIDI tracks have MIDI output routing. Software Instrument tracks don't expose this setting.

2. **Add CC Automation to Conductor:**
   - Select the conductor track
   - Press **A** to show automation
   - Click the automation dropdown (says "Volume" by default)
   - Select **MIDI CC** (at the bottom of the list)
   - Choose your CC number (e.g., **"20 Undefined"** for Rate, **"22 Undefined"** for Energy)
   - Draw automation curves on the timeline (click to create points, drag to shape)

3. **Route Conductor Output to IAC Driver:**
   - Select the conductor track
   - Press **I** to show Inspector (left sidebar panel)
   - In the **"Track"** section at the top, find **"Output:"** dropdown
   - Click **Output:** and select **IAC Driver** → **Bus 1**

   **Can't find "Output"?** Make sure you created an External MIDI track, not a Software Instrument track.

---

#### Setup Plugin Tracks to Receive

For each track that has the String Field MIDI plugin:

1. **Set Input Source:**
   - Select the plugin track
   - Press **I** to show Inspector
   - In the **"Track"** section, find **"Input:"** dropdown
   - Select **IAC Driver** → **Bus 1**

2. **Enable Input Monitoring:**
   - On the track header, click the **Input Monitoring** button (icon that looks like a speaker/waveform)
   - Or enable **Record** button (R) on the track
   - This allows the track to receive MIDI from IAC Driver even when not recording

3. **Repeat for all plugin instances**

---

#### Test It!

1. Draw some CC automation on the conductor track (e.g., CC 22 going from 0 to 127)
2. Press Play
3. All plugin instances should respond to the automation simultaneously!

**All instances will now respond to the conductor's CC messages!**

### Method 2: Logic Scripter (Advanced)

Create a "conductor" track using Scripter to generate evolving CC patterns:

```javascript
// Example: Slowly modulate Energy (CC 22)
var energy = Math.sin(GetTimingInfo().blockStartBeat * 0.05) * 63.5 + 63.5;
var cc = new ControlChange;
cc.number = 22;  // CC 22 → Energy
cc.value = Math.round(energy);
cc.send();
```

Route this track's MIDI output to IAC Driver Bus 1.

### Method 3: Hardware Controller

Connect a MIDI controller with knobs/faders and route it to IAC Driver:
- Map hardware knob to CC 22 to control Energy
- Map another knob to CC 21 to control Density
- Map fader to CC 29 to control Tempo
- Move one control → all instances respond together

---

## Musical Examples

### Example 1: Global Energy Sweep
```
Conductor Track:
  - CC 22 (Energy): 0.2 → 0.8 over 2 minutes

Result on all tracks:
  - Starts with long Feldman-style notes
  - Gradually becomes agitated and staccato
  - All articulations morph in sync
```

### Example 2: Density Breathing
```
Conductor Track:
  - CC 21 (Density): 0.0 → 0.5 → 0.0 (sine wave, 30 second cycle)

Result:
  - Texture fades in and out
  - All tracks breathe together
  - Creates orchestral swells
```

### Example 3: Tempo Acceleration
```
Conductor Track (Pulse Mode ON):
  - CC 29 (Tempo): 60 BPM → 180 BPM over 1 minute

Result:
  - All instances accelerate together
  - Creates Ligeti-style rhythmic intensification
```

### Example 4: Articulation Exploration
```
Conductor Track:
  - CC 27 (Articulation): 0.0 → 1.0 → 0.0 (triangle wave, 20 seconds)

Result:
  - All instruments morph through articulations
  - Legato → staccato → flutter → back to legato
  - Timbral waves across ensemble
```

---

## Advanced: Custom CC Mappings

### Supported Parameters

You can map MIDI CCs to any of these parameters:

| Parameter ID    | Description                    |
|-----------------|--------------------------------|
| `rate`          | Note generation rate (Hz)      |
| `density`       | Probabilistic note gating      |
| `energy`        | Feldman ↔ Chaos (affects rhythm/timbre) |
| `center`        | Pitch center (MIDI note)       |
| `spread`        | Pitch range (semitones)        |
| `vel`           | Base velocity                  |
| `seed`          | Random seed                    |
| `routes`        | Number of MIDI channels        |
| `memory`        | Motivic repetition kernel      |
| `articulation`  | Articulation center            |
| `pulse`         | Pulse mode on/off              |
| `tempo`         | Pulse tempo (BPM)              |
| `regularity`    | Rhythmic variance              |
| `pedal`         | Sustain pedal on/off           |
| `pcmode`        | Pitch class set mode           |

### MIDI Learn API

The plugin includes a MIDI learn system (currently accessible via code):

```cpp
// Enable learn mode for a parameter
processor.setMIDILearnMode(true, "energy");

// Send any CC message → it gets mapped to "energy"
// Future CC messages on that number will control energy

// Query current mapping
int ccNumber = processor.getCCForParameter("energy");  // Returns CC number or -1

// Clear mapping
processor.clearCCMapping("energy");

// Clear all
processor.clearAllCCMappings();
```

---

## Tips for Conductor Workflows

1. **Use Automation Lanes Wisely:**
   - One CC lane can control dozens of plugin instances
   - Reduces project complexity vs per-track automation

2. **Layer Different Seeds:**
   - Each instance uses different Seed value
   - Same CC controls → different but related output
   - Creates rich, varied textures

3. **Combine with Track Stacks:**
   - Group all String Field tracks into a stack
   - Conductor track sends MIDI to entire stack
   - Mix controls at stack level

4. **Save Snapshots:**
   - Use Logic's Snapshots to save CC values
   - Recall different "moods" instantly

5. **Hardware Performance:**
   - Map hardware knobs to CC 20-30 for live control
   - CC 22 (Energy) and CC 21 (Density) are great for performance
   - Use expression pedal for CC 27 (Articulation) shifts
   - Perform global transformations live

---

## CC Value Ranges

MIDI CCs send values 0-127. The plugin automatically maps these to parameter ranges:

```
CC 0   → Parameter minimum
CC 64  → Parameter midpoint
CC 127 → Parameter maximum
```

**Example (Energy):**
```
CC 22 = 0   → Energy 0.0 (Feldman mode)
CC 22 = 64  → Energy 0.5 (balanced)
CC 22 = 127 → Energy 1.0 (maximum chaos)
```

---

## Troubleshooting

### Common Issues

**❌ Can't find "Output" setting in Inspector**

**Solution:** You created a **Software Instrument** track instead of **External MIDI** track.
- Delete the conductor track
- Create new track: Track → New Track → **External MIDI**
- Only External MIDI tracks have MIDI output routing

---

**❌ Plugin tracks not responding to conductor**

**Checklist:**
1. Is IAC Driver enabled? (Check Audio MIDI Setup)
2. Is conductor **Output** set to **IAC Driver Bus 1**?
3. Are plugin tracks **Input** set to **IAC Driver Bus 1**?
4. Is **Input Monitoring** enabled on plugin tracks? (speaker icon or R button)
5. Are you drawing automation on the correct CC numbers (20-30)?

---

**❌ CCs not working / Parameters not changing**

**Solutions:**
- Verify CC number matches the mapping table (CC 20 = Rate, CC 22 = Energy, etc.)
- Check automation is actually drawing (press A to show automation view)
- Restart Logic Pro after updating plugin
- Use Logic's **MIDI Monitor** to verify CC messages are being sent:
  - Window → Show MIDI Activity
  - You should see CC messages when playing

---

**❌ Only one instance responds, not all**

**Solution:** Only the first instance has **Input** set to IAC Driver Bus 1.
- Go through each plugin track
- Set **Input: IAC Driver Bus 1** on every track
- Enable input monitoring on every track

---

**❌ IAC Driver not appearing in Logic**

**Solution:** IAC Driver not enabled.
- Open Audio MIDI Setup
- Window → Show MIDI Studio
- Double-click IAC Driver
- Check "Device is online"
- **Restart Logic Pro** after enabling

---

**❌ Parameters jumping or behaving strangely**

**Solution:** Multiple automation sources conflicting.
- Check if parameter has both CC automation AND track automation
- Disable one source
- CC automation will override manual knob adjustments

---

**Want different CC mappings?**
- Currently requires code changes (edit `PluginProcessor.cpp` constructor)
- MIDI Learn API available (see above)
- Future versions may include UI for custom mapping

---

## Next Steps

- Try the examples above
- Experiment with Scripter for complex patterns
- Combine CC automation with timeline automation for hybrid control
- Use hardware controllers for live performance

The conductor system turns String Field MIDI into an **ensemble instrument** where one controller shapes the behavior of many voices!
