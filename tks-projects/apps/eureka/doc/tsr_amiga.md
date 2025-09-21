% Eureka - Amiga / Tiny Synergy Replay (TSR) Technical Notes
% bsp
% 16-Sep-2021
>>>
16Oct2020, 27Aug2021, 16Sep2021
<<<
$(var:header)


# Table of Contents
$(buf!toc)


# Introduction
Tiny Synergy Replay (TSR) is a speed and size optimized replay format for the Commodore Amiga (tested on a rev6 Amiga 500).

The exporter in the `Synergy` sequencer works on an arbitrary stream of note/controller events and uses a simple compression scheme to identify and optimize recurring events.

TSR supports a (tiny) subset of the Eureka sampler, including
   - multi-sampled keymaps, modwheel, and velocity zones
   - multiple, (sequencer-)modulatable loops (sample regions) per sample zone
   - oneshot and looped sample replay
   - four 16-step modulation sequencers
       - volume
       - pitch fine
       - pitch coarse
       - sample loop
   - sample offset randomization
   - velocity to sample offset
   - velocity to volume
   - linear pitch glide
   - volume ramps (fade in)
   - volume fade out at note off (release time)
   - 1D and 2D wavetables

For performance reasons, many of the more complex features, like the keymap based sample zone selection, will be baked into the `.tsr` file during export.


# MIDI Ports

Note and controller data received on MIDI ports `vst_a:1` .. `vst_a:4` is mapped to Amiga sound channels 1..4.


# MIDI Events

| Event         :|: Description
| Program Change | Select first Sample-Instrument that is assigned the given program number
| Note On        | Play note. C-3..B-5 (after zone transpose) correspond to Protracker notes C-1..B-3
|                |+ - Note numbers C-0 .. G-10 (0..127) can be used to select zones from multi-sample keymaps
|                |+ - Velocity can be mapped to volume (via volume velocity amount, `Vel`)
|                |+ - Velocity can be used to select zones from multi-sample keymaps
|                |+ - Velocity can be mapped to sample offset (via sample offset velocity amount, `OffV`)
|                |+ - Start sample offset can be randomized (via sample offset random amount, `OffR`)
| Note Off       | Stop note / start release phase (velocity is ignored)
| Pitchbend      | Pitchbend (-1..+1 octave)
| CC#1  Modwheel | Filter multi-sample zones at note-on
| CC#2  Breath   | Wavetable X-Pos (normalized, 0..127)
| CC#4  Foot     | Wavetable Y-Pos (normalized, 0..127)
| CC#7  Vol      | Volume (0..127 is mapped to Amiga volume 0..64)
| CC#11 Expr     | Jump to Loop (absolute values, i.e. 0=loop 0, 1=loop 1, 2=loop 2, ..)
| CC#16 Gen1     | (Re-)trig Mod Sequencer 1 when >0 (Volume)
| CC#17 Gen2     | (Re-)trig Mod Sequencer 2 when >0 (Pitch Fine Tune)
| CC#18 Gen3     | (Re-)trig Mod Sequencer 3 when >0 (Pitch Coarse)
| CC#19 Gen4     | (Re-)trig Mod Sequencer 4 when >0 (Loop Table Index)
| CC#80 Gen5     | Volume ModSeq table (0..15)
| CC#82 Gen7     | Freq Coarse ModSeq table (0..15) (e.g. for arpeggios)
] Received MIDI Events


# Program Number assignment
In order to mark a Sample Instrument for Amiga export, it must be assigned a unique MIDI program number (1..128, or 1..9, A..Z, a..z).

This is also used for switching samples via MIDI program change events.


# Sample Replay

The Amiga replay supports regular (forward) sample playback as well as wavetable (granular) playback.


## Limits (max zones)
Each Sample Instrument in `Eureka` consists of *n* sample zones.
The Amiga replay supports a maximum of 255 sample zones (sum total).


## Loops
Each sample zone contains a loop table with an arbitrary number of loop entries (up to 85).
When the repeat count of the first loop is `1`, the next loop will be queued for playback automatically.
A repeat count of `0` will play a loop indefinitely.

Loops can be switched while a sample is playing, either by using the `CC#11 Expr` MIDI controller (mapped to `Jump To Loop Abs` modmatrix target), or by using **modseq #4**.

Whether zone loops are queued or played immediately is controlled by the "jump to loop immediately" checkbox (`I` above the loop table).


## Play-from-start
The Play-From-Start checkbox (`Start`, directly above the loop table) causes the sample to be played from the beginning before jumping to the loop.
I.e. for regular sample loops, only a single loop table entry is required (similar to Protracker).

When the checkbox is set to mixed mode, the sample will be played from the position defined by the offset of the *last* loop table entry.
This is mostly useful for multisamples.


## DMA alignment
Keep in mind that even though the samples are 8bit, the Amiga sample DMA requires word-aligned sample offsets / lengths.

## Wavetables
Wavetables use either a 1D, or a 2D layout. The cycle length range (`CycleLen`) is 8..1024 sample frames.

### 1D
1D wavetables consist of up to 128 cycles.
If the wavetable size exceeds 16 cycles, it must be aligned to a multiple of 16.

### 2D
2D wavetables consist of up to 256 cycles (1x1 .. 16x16).


## Glide
When glide is enabled in a sample zone (mode=`Time(lin)`), the glide speed will be mapped to a 4bit `glide_speed` value during export (see `glide_speed_tbl` in the replay). The range is 7.5 - 7680 milliseconds. Separate up and down glide speeds are supported.

Note glides are activated by playing overlapping notes.

note: glide is calculated in linear note-number space


## Volume Ramp fade in
When the volume ramp up time (`Rmp`) is greater or equal than 16 millisecs, a short fade-in (6 replay ticks) will be applied during Amiga replay.
note: the exporter sets a flag, i.e. the actual ramp up time in the editor does not matter (it should be 16 to make it sound as close as possible to the HW replay)



# Sample Instrument Initialization

## amiga_init
The `amiga_init` page command (status bar command line) prepares a regular init patch for TinySynergyReplay (TSR) use.

For example, the sample rate will be set to 16574 Hz (middle C-5) and the mod matrix will be set up to route the four mod sequencers and map the MIDI performance controllers.

CAUTION: do not change the mod matrix routing manually, the exporter relies on the exact order of entries

## editable modmatrix entries

### volume, pitch fine and coarse amounts
The `amount` setting of entries 2, 6, and 10 may be edited and is used for scaling the modsequencer step values during export.

### manual or LFO wavetable position
Manual vs LFO-controlled wavetable position modulation is selected by toggling modmatrix entries 11+12 and 15+16 (mutually exclusive).





# Envelopes and LFOs

The regular bezier-curve envelopes and volume / freq LFOs must **NOT** be used (they are too complex for the 7 MHz 68000 CPU in an Amiga 500).

The only exception to this is the **volume envelope release time** (`R` in `Vol` widget row).


## Mod Sequencers
The mod sequencers are used as follows:

   - **modseq #1** Volume Envelope or LFO (also supports `Slew` interpolation)
   - **modseq #2** Pitch Fine Tune Envelope or LFO
   - **modseq #3** Pitch Coarse Tune Envelope or LFO
   - **modseq #4** Loop Table Envelope or LFO

Mod sequencer features supported by the Amiga replay:
   - configurable length  (`Len`, 1..16 steps)
   - repeat offset (`Rep`)
   - replay tick synchronization (`Sync`, must be enabled)
   - configurable speed (1..255 ticks)
   - scale (`Scl`) and offset (`Off`)
   - key-on reset (`Rst`)
   - delayed start (`Rst` in mixed mode) and retrig via CC#16..19 
   - one-shot mode (`One`)

   


## Pan and Aux wavetable LFOs
The Pan and Aux LFOs are used for wavetable x / y position modulation (when enabled in the mod matrix).

The Amiga replay supports 
   - key-on reset (`Rst` checkbox)
   - configurable start phase (`Phase` floatparam)
   - oneshot-mode (`One` checkbox)
   - triangle waveform


# Cycle Sample Synthesis

Each Sample Instrument (Sample Bank) stores up to 32 `Cycle` synth patches which are processed in order of occurence.
Patches without valid target zone assignments will be skipped.

When the zone double-click mode (toggle button right next to zone table) is set to `S`, double-clicking a zone opens the patch source code window. Setting the mode to `M` brings up the main patch window (param sliders) instead.

See the [Cycle Manual](cycle.html) for a detailed description of the `Cycle` script syntax.


## Render Area
The sample synthesis target area is determined by the **last** entry in a zone's loop table.

TIP: it's sometimes useful to create a *dummy zone* that's never audible and simply serves to setup a render area. this can be achieved by e.g. setting the zone's velocity range (`LV` and `HV`) to 0

### Resizing a render area
Copy and paste (or cut) sample data within a zone loop area to increase (or decrease) its size and automatically shift all following zones as required.

### Initial waveform size
The page command `resize_waveform <x> <y>` resizes the waveform to `<x> * <y>` sample frames.

Click into the status bar and type `res` followed by `cursor down` to initiate the command.


## Reorder zones
A sample instrument's `Cycle` synth patches can be reordered to match the zone order. Choose `SampleSynth: Reorder patches by zones` from the zone table context menu (RMB click).


## Cloning zones and synth patches
Hold lshift while hold-clicking the `+` button of the zone table to clone a zone and its sample synth patch.
The new zone will be assigned a new render area and the waveform will be resized when necessary.




$(buf!footnote)
$(buf!w3validator)

Document created in $(var:gen_ms) on $(var:localdatetime)
].create
