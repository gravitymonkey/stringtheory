# Conductor Mode - MIDI CC Control

The String Field MIDI Generator supports **MIDI CC control** for creating a "conductor" that controls multiple instances simultaneously.

## How It Works

Any MIDI CC messages sent to the plugin will control the mapped parameters across **all instances**. This allows you to:
- Control multiple tracks from one automation lane
- Use hardware controllers to perform live
- Create evolving textures that change globally

---

## Default CC Mappings

Out of the box, these MIDI CC numbers are mapped:

| CC Number | CC Name        | Parameter      | Use Case                          |
|-----------|----------------|----------------|-----------------------------------|
| **1**     | Mod Wheel      | Energy         | Feldman → Chaos morphing          |
| **7**     | Volume         | Density        | Texture density control           |
| **11**    | Expression     | Articulation   | Timbre/technique selection        |
| **74**    | Brightness     | Tempo          | Pulse tempo changes               |
| **71**    | Resonance      | Regularity     | Rhythmic strictness               |
| **73**    | Attack         | Memory         | Motivic repetition strength       |

---

## Setup in Logic Pro

### Method 1: MIDI Track as Conductor

1. **Create a MIDI track** (not assigned to an instrument)
2. **Add MIDI CC automation:**
   - Click the automation dropdown
   - Select "MIDI CC" → choose your CC (e.g., "1 Mod Wheel")
3. **Draw automation curves** on the timeline
4. **Route to all plugin instances:**
   - Send the MIDI track output to each track using the plugin
   - Or use External Instrument routing

**All instances will respond to the CC messages!**

### Method 2: Logic Scripter (Advanced)

Create a "conductor" track using Scripter to generate evolving CC patterns:

```javascript
// Example: Slowly modulate Energy (CC 1)
var energy = Math.sin(GetTimingInfo().blockStartBeat * 0.05) * 63.5 + 63.5;
var cc = new ControlChange;
cc.number = 1;  // Mod Wheel → Energy
cc.value = Math.round(energy);
cc.send();
```

Route this track's MIDI output to all plugin instances.

### Method 3: Hardware Controller

Connect a MIDI controller with knobs/faders:
- Map hardware CC 1 (mod wheel) to control Energy
- Map hardware CC 7 (volume) to control Density
- Move one knob → all instances respond

---

## Musical Examples

### Example 1: Global Energy Sweep
```
Conductor Track:
  - CC 1 (Energy): 0.2 → 0.8 over 2 minutes

Result on all tracks:
  - Starts with long Feldman-style notes
  - Gradually becomes agitated and staccato
  - All articulations morph in sync
```

### Example 2: Density Breathing
```
Conductor Track:
  - CC 7 (Density): 0.0 → 0.5 → 0.0 (sine wave, 30 second cycle)

Result:
  - Texture fades in and out
  - All tracks breathe together
  - Creates orchestral swells
```

### Example 3: Tempo Acceleration
```
Conductor Track (Pulse Mode ON):
  - CC 74 (Tempo): 60 BPM → 180 BPM over 1 minute

Result:
  - All instances accelerate together
  - Creates Ligeti-style rhythmic intensification
```

### Example 4: Articulation Exploration
```
Conductor Track:
  - CC 11 (Articulation): 0.0 → 1.0 → 0.0 (triangle wave, 20 seconds)

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
   - Map Mod Wheel (CC 1) to Energy for live control
   - Use Expression Pedal (CC 11) for Articulation shifts
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
CC 1 = 0   → Energy 0.0 (Feldman mode)
CC 1 = 64  → Energy 0.5 (balanced)
CC 1 = 127 → Energy 1.0 (maximum chaos)
```

---

## Troubleshooting

**CCs not working?**
- Make sure MIDI is routing to the track (check Logic's track inspector)
- Verify CC number matches the mapping table
- Check that the plugin is receiving MIDI (use Logic's MIDI monitor)

**Want different CC mappings?**
- Currently requires code changes (see MIDI Learn API above)
- Future versions will include UI for custom mapping

---

## Next Steps

- Try the examples above
- Experiment with Scripter for complex patterns
- Combine CC automation with timeline automation for hybrid control
- Use hardware controllers for live performance

The conductor system turns String Field MIDI into an **ensemble instrument** where one controller shapes the behavior of many voices!
