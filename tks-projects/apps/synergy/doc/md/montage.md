% Synergy MIDI Tracker - Yamaha Montage Editor
% bsp
% 30-Nov-2024
>>>
31Oct2022, 02Nov2022, 04Nov2022, 15Nov2022, 17Nov2022, 19Nov2022, 31Mar2023, 30Nov2024
<<<
$(var:header)


# Table of Contents
$(buf!toc)

# Yamaha Montage Editor

This is an editor for the Yamaha Montage 6/7/8 line of synths.

It has been tested with Montage firmware 3.51.0 (the latest as of Nov 4th, 2022).

The editor adds the concept of "part variations" and "variation scenes".
Each patch stores up to 16 Montage performances. Each Montage part gets 16 variations that can be switches on-the-fly.
Part variation and variation scene selections can also be triggered via the synth keyboard and can be recorded back into the Synergy sequencer.


## Features

- Part Mixer
   - quick-edit all 16 parts on one page
   - copy patch / part EQ / part FX settings from other performance part / variation
- FM matrix editor with automatic algorithm selection
   - multi-edit, cycle, shuffle, randomize operators
   - scale operator AEG envelopes (shorten/lengthen)
   - operator presets
      - 1..8 operators per preset (8 preset types)
- Envelope mixer
   - morph between PEG / AEG / FEG preset envelopes
   - randomization of preset selections and morph amounts
   - persistent EG mixer state (saved per part variation / FM operator / AWM2 element)
- FM Sound mixer
   - morph between 4 FM-X patches
   - can (optionally) morph each operator separately
   - can limit morphing to specific parameter groups
   - randomization of sound selections and x/y morph positions
   - persistent sound mixer state (saved per part variation)
- AWM2 element editor
   - reads user waveform names from text file (`montage_user_waveforms.txt`)
- Part LFO editor
   - utility functions for generating / modifying user LFO waveform
- Part Controller Box (mod matrix) editor
- Edit part insert effects
- Realtime FM sound morphing
   - 2 or 4 sound variations (morph X or morph X+Y)
   - x/y positions controlled via (internal) RPNs (0..127)
- Realtime AEG scaling
   - scaling factor controlled via (internal) RPN (0..255)
   - configurable scaling exponent (give weight to either short or long envelope times)
   - FM sound morphing and AEG scaling can be used simultaneously
- 15 global banks * 128 patches / performances (`A`..`O`)
- 1 project-specific patch bank (`P`)
- up to 16 parts per performance (AWM2, FM-X or DRUM)
   - part layer flag: automatically play neighbouring parts in editor
      - play layered parts from sequencer by sending notes to proxy node automation port A/B
   - rearrange parts via drag'n'drop
- up to 16 variations per part
   - technically up to 16 performances per patch
   - utility functions to sync part Level / CtlBox / FX / part EQ / master EQ/FX from Main variation
   - variation changes can be sequenced via `proxy_layer_[1..16]_prgchg` RPN (proxy node automation port A / B)
   - select part variation via `F-2+C-4..D#5` synth key combo
   - part variation changes can be recorded back into sequencer
      - either via synth key combos
      - or by selecting part variations in the editor UI
- up to 32 variation scenes
   - change all part variations at once (or <keep> current part variation)
   - select variation scene via `F#2+C-4..G-6` synth key combo
   - variation scene changes can be recorded back into sequencer
      - either via synth key combos
      - or by selecting variation scenes in the editor UI
- option to force part outputs to "Main L&R" / "Asgn L&R"
- option(s) to remap sliders (and change mapping via synth key combos)
   - part volume (`F-2+B-3`)
   - current part assignable knobs 1..8 (`E-2+B-3`)
   - part 1..8 assignable knob 1/2/../8  (`E-2+C-3..G-3`)
   - part 9..16 assignable knob 1/2/../8  (`F-2+C-3..G-3`)


## Setup

note: the example project is preconfigured for the Montage (i.e. the editor *should* work out of the box)

Manual configuration:
- activate `MONTAGE-1` input and output devices (`MONTAGE Port1` on macOS), assign `montage` device alias and `Yamaha Montage / MODX` MIDI synth profile
- create + activate a MIDI morph scene that forwards Montage MIDI events to the `remote_audio` virtual MIDI device
   - `remote_audio` events will then be forwarded by the synth editor to the current part(s)
       - `synergy_config.tks`:`pagesysex_auto_devicename` setting
   - preset scene: `Yamaha Montage Editor` (Scene => Tools.. => Load Scene)
- optionally: create Proxy node instance, set Output to `montage` and automation device A to e.g. `play_a`
   - forwards (sequenced) `proxy_layer_[1..16]_prgchg` events to the editor
- turn off synth `Local Control` (on synth: Utility => MIDI I/O => Local Control)

note: synth part kbdctl switches should be set to `off` (or layered sounds may not play correctly)


## MODX (experimental)
To enable MODX support, edit `synergy_config_local.tks` and change
~~~~
b_pagesysex_modx = 0;
~~~~
to
~~~~
b_pagesysex_modx = 1;
~~~~

This will make the synth editor send MODX SysEx model ids (0x07) instead of Montage ids (0x02).

caution: MODX support is experimental and untested. Since the SysEx documentation for both synths is mostly identical, it _should_ work, though.



## General Notes
- ui: FloatParam widgets: hold RMB + move mouse to change value, click LMB (while still holding RMB) for undo
   - move mouse horizontally for fine adjustments, and vertically for coarse ones
- ui: FloatParam widgets: inc/dec values via mousewheel
- ui: ComboBox widgets:
   - click RMB to preview selection without closing the ComboBoxPopup (e.g. AWM2 waveform, "< COPY >" comboboxes)
   - click MMB to switch between preview and page scroll mode (mouse wheel)
   - type chars to filter entries (RETURN jumps to next search result)
- ui: ComboField widets: LMB hold-click or cursor left/right starts text edit mode (e.g. patch / variation names)
- the UI tooltips often contain additional documentation
- switching part types requires bulk dumps (takes ~1.5 sec)
- performance settings (EQ/FX/..) are only updated when part 1 variation is changed
- param changes and variations are sent as param diffs (fast)
   - up to 8 params per replay tick when replay is running
   - up to 64 params when replay is stopped
- Patch + FM Matrix tabs: part button double-click (un-)solos part
- FM Common / FM Matrix / 1..8 / Part Mixer tabs: RMB click opens EG Mixer dialog
- Part Mixer: `L:` label-button click (un-)mutes part
- Part Mixer: part label-button mouse wheel selects previous / next part variation
- Part Mixer: part label-button RMB click opens context menu
- Part Mixer: enabling `Fav only` checkbox limits `< COPY >` options and FM Sound Mixer patch options to star-rated part variations
- FM Sound Mixer: skips "Init" patches when randomizing part variation selections
- Element/Op 1..8 param forms: hold LALT to multi-edit param


## Synth Notes

- the FM parts are mostly identical to the Yamaha FS1R except that
   - the FS1R has additional 8 unvoiced ops per part
   - the FS1R has formant op waves

The FM matrix editor is shared between the FS1R and Montage synth editors.
The four "part copy" comboboxes (at the bottom) are only active in FS1R mode. In the Montage editor there's a separate tab for that and the comboboxes will simply indicate the currently edited part (in groups of four). The part level and note shift widgets are active in both editors.


### Note-on latched parameters
One very useful feature of the synth is that many parameters are read only at note-on, i.e. changing those parameters will not affect currently playing voices.
These parameters include (but are not limited to):
  - Part Common:
      - micro tuning
      - eq3
  - FM-X:
     - pan rand
     - pan alt
     - key on delay
     - filter type
     - filter gain
     - algorithm
     - eg settings (common FEG/PEG)
  - AWM2:
     - waveform
     - coarse
     - fine
     - pan
     - pan rand
     - pan alt
     - filter type
     - filter gain
     - eq type
     - eq Q
     - eq1 freq
     - eq1 gain
     - eq2 freq
     - eq2 gain
     - LFO(2) settings (except for "speed")
     - eg settings

By using part variations it is therefore possible to simultaneously play many different sounds on a single part
(i.e. play note, select variation that modifies e.g. Op attack level or AWM2 waveform, play next note, ..).

## Variation Notes

What a variation is supposed to be used for:
  - (minor) sound patch changes
     - e.g. Op Levels, EGs, element waveform selection, ..
  - part EQ changes
     - especially the per-voice EQ3
  - part level / pan / effect send changes
  - minor (insert-)FX param changes
  - assignable knob value changes
  - part level AEG / FEG / Filter changes ("Quick" / "Easy Edit" controls)

What a variation cannot / should **not** do (when used for realtime updates):
  - change part type
  - change AWM2 element on/off switches
  - change insert FX types
  - change MSeq / Arp setup
  - change Superknob settings
  - change (Montage) scene setup
  - change audio setup
  - change Master EQ/Reverb/Variation/Effect setup

The more settings a part variation modifies, the longer it takes to send the required SysEx param updates to the synth.
Param updates are limited to 8 per replay tick (while replaying is running) to avoid choking the MIDI bus.

While the replay is running, variation changes filter out the following data groups (see `MontagePerformance.tks` for details):
  - G_SYSTEM
  - G_SEQSETUP
  - G_MICROTUNE
  - G_NAME
  - G_MSEQ
  - G_ARP
  - G_SUPERKNOB
  - G_KNOBNAME
  - G_SCENE
  - G_AUDIO
  - G_AUDITION
  - G_MEQ
  - G_MFX
  - G_TEMPO

When not used for realtime updates, variations can still be useful for storing alternative performance setups, i.e. before deciding what works best for a particular project.

tip: many patch parameters can be modulated via the synth's mod matrix (Controller Box). This takes up less MIDI bandwidth, plus the mod matrix can be used on top of variation changes.


## Proxy Node / Layered Parts

Proxy nodes connect the Synergy sequencer and MIDI router to built-in synth editor instances.
The proxy node `Output` device (and its assigned MIDI synth profile) determines which synth the node targets.

### Automation ports
Like all sequencer nodes in Synergy, a proxy node has two automation ports (A + B).

In case of the Montage editor, the automation ports are used to
  - receive "Proxy: Program Change" RPN (registered parameter number) events to select variation scenes
  - receive "Proxy: Layer \[1..16\] PrgChg" RPN events to select part \[1..16\] variations
  - receive CCs, transform them (e.g. map sliders to assignable knobs) and forward them to the synth
  - receive other performance controllers like channel aftertouch and pitchbend, and layer / forward them to the synth
  - receive note ons+offs and layer / forward them to the synth

Since the Montage has no assignable MIDI receive channels per part, it is recommended to send all MIDI data through a proxy node so that the synth editor can replicate the MIDI events to layered parts.

In order to layer a part with the previous part, enable the layer toggle button below a part button on the editor "Patch" tab.

### Input port
The input port can receive the following MIDI messages (channels 1..16 map to Montage parts 1..16):
  - RPN#1000 : Realtime AEG scaling (0..255 => -9..9)
  - RPN#1001 : Realtime FM sound morphing position X (0..127 => 0..1) (variations 1+2)
  - RPN#1001 : Realtime FM sound morphing position Y (0..127 => 0..1) (variations 1+2+3+4)


## Keyboard Shortcuts

| Key Sequence           :|:Description
|            lctrl - 1..9 | select part 1..8
|   lctrl - lshift - 1..9 | select part 9..16
|               lctrl - s | send patch manually
|      lctrl - lshift - s | send patch manually (clear MIDI cache + invalidate replay state)
|               lctrl - e | query / receive performance
|      lctrl - lshift - e | query / receive performance to clipboard
|                lalt - e | query / receive part
|               lctrl - f | show FM sound mixer
|               lctrl - g | show EG mixer
|               lctrl - c | copy patch to clipboard
|      lctrl - lshift - c | copy variation to clipboard
|               lctrl - v | paste patch from clipboard
|      lctrl - lshift - v | paste variation from clipboard
|              lshift - c | copy op or element to clipboard (1..8 tab) OR copy PFX (Patch tab) OR copy CTL (Part CTL tab)
|              lshift - v | paste op or element from clipboard (1..8 tab) OR paste PFX (Patch tab) OR paste CTL (Part CTL tab)
|              lshift - x | swap op or element with clipboard
|                lalt - c | copy part to clipboard
|                lalt - v | paste part from clipboard
|                lalt - x | swap part with clipboard
|             lalt - LEFT | move part left
|            lalt - RIGHT | move part right
|               lctrl - n | init patch
|             lctrl - TAB | toggle part solo
|                       q | select op or element 1
|                       w | select op or element 2
|                       e | select op or element 3
|                       r | select op or element 4
|                       t | select op or element 5
|                       y | select op or element 6
|                       u | select op or element 7
|                       i | select op or element 8
|              lshift - 1 | toggle mute op 1
|              lshift - 2 | toggle mute op 2
|              lshift - 3 | toggle mute op 3
|              lshift - 4 | toggle mute op 4
|              lshift - 5 | toggle mute op 5
|              lshift - 6 | toggle mute op 6
|              lshift - 7 | toggle mute op 7
|              lshift - 8 | toggle mute op 8
|                       1 | select tab "Patch"  (or 'a')
|                       2 | select tab "Part Mix"
|                       3 | select tab "FM Common"
|                       4 | select tab "FM Matrix" (or 'k')
|                       5 | select tab "Part LFO"
|                       6 | select tab "Insert FX"
|                       7 | select tab "Part CTL"
|                lalt - r | reset part quick edit controls
|               lctrl - r | reset performance quick edit controls
|                    HOME | select next patch
|           lshift - HOME | select next part variation
|                     END | select previous patch
|            lshift - END | select previous part variation
|                  PAGEUP | select next part
|                PAGEDOWN | select previous part
] Keyboard Shortcuts


## Demo Videos
   - [Offline FM sound morphing / randomization](https://www.youtube.com/watch?v=CHmBDO1ULyk)
   - [Variation scenes](https://youtu.be/7m8nhkFZgxg)
   - [Realtime FM sound morphing](https://youtu.be/6zupNa2NGl4)
   - [Offline and Realtime FM Operator AEG Scaling](https://youtu.be/yMBcrBB16Ws)
   - [Part Insert FX editing](https://youtu.be/q1nGGxZy9hY)
   - [FMMatrix operator presets](https://youtu.be/DWg_ccttCxY)



$(buf!footnote)
$(buf!w3validator)

Document created in $(var:gen_ms) on $(var:localdatetime)
].create
