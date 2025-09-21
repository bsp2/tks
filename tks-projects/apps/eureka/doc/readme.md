% Eureka Sampler / DAW
% bsp
% 21-Jun-2025
>>>
17Nov2019, 15May2020, 16Oct2020, 06Feb2021, 31May2021, 04Jan2023, 18Mar2023, 24Mar2023, 31Mar2023, 04Aug2023, 05Aug2023, 10Aug2023, 16Aug2023, 18Aug2023, 19Aug2023, 07Dec2023, 14Dec2023, 15Dec2023, 16Dec2023, 03Feb2024, 27Apr2024, 07Jul2024, 04Aug2024, 06Aug2024, 10Aug2024, 12Aug2024, 28Sep2024, 30Sep2024, 03Oct2024, 04Oct2024, 05Oct2024, 28Nov2024, 30Nov2024, 23Dec2024, 26Dec2024, 28Dec2024, 31Dec2024, 04Jan2025, 20Mar2025, 21Mar2025, 22May2025, 28May2025, 29May2025, 30May2025, 07Jun2025, 11Jun2025, 12Jun2025, 15Jun2025, 20Jun2025, 21Jun2025
<<<
$(var:header)


# About

"Eureka" is a sampler, synth, plugin host, and multi-track recorder designed for low latency audio.

It is the audio companion app to the MIDI-only "Synergy" sequencer.


# Table of Contents
$(buf!toc)


# Features
- arbitrary number of instrument tracks
   - arbitrary number of *lanes* per track
   - alternative [MPE](#midi-polyphonic-expression-mpe), lane-per-channel ([LPC](#lane-per-channel-lpc)), voice-round-robin ([VRR](#voice-round-robin-vrr)), drum pad sampler ([drum kit](#drum-pad-sampler-drum)), and timeline modes
   - side chain inputs
   - mid / side processing
   - routing between lanes, tracks and outputs
      - arbitrary number of outputs per track and lane
   - track mod matrix
      - for modulating module parameters via MIDI
      - cubic bezier curve note velocity, pitchbend, pressure, and CC / RPN / NRPN controller shaping
      - note and velocity range filters
      - continuous controller filters

- built-in lane modules
   - sampler
   - delay
   - gain / pan / stereowidth
   - 3-band equalizer
   - mid / side conversion
   - (cross-track) sync (+- 100ms)
   - Audio to MIDI RPN
   - MIDI RPN to Audio
   - Gate to Audio
   - Idle detection
   - [CLAP](https://cleveraudio.org/) plugin ([plugin database](https://clapdb.tech/))
   - VST2 plugin
   - [STFX](https://github.com/bsp2/stfx/) plugin

- sampler
   - arbitrary number of instances
   - arbitrary number of samples
      - stored as `.wav` + `.esa` files
      - (optional) YAML export for Rossum-Electro [Assimil8or](https://www.rossum-electro.com/products/assimil8or/) eurorack sampler
          - set `b_write_yml_project = true;` to enable this (see `eureka_config.tks`)
   - up to 32 sample namespaces
      - e.g. for separating audio clips and drum kits from regular samples
   - seamless sample switching
      - via MIDI program change or RPN
   - extensive (per voice) mod matrix
      - up to 16 entries per sample zone
      - up to 16 matrix variations per sample zone
      - 183 sources
      - 203 destinations
      - log / lin / exp curve per entry
      - slew per entry
         - can also be used to generate attack / decay mini-envelopes
      - 4 scratch registers
      - 13 op modes per entry (add, mul, replace, blend, compare, step, ..)
      - support for 14 bit hi-res continuous controllers (CCs)
         - modwheel (CC#1), breath control (CC#2), foot control (CC#4), expression (CC#11)
   - arbitrary number of zones (layers) per sample
      - mono or stereo
      - note, velocity, modulation (e.g. modwheel) filters
      - "alt" groups
      - "mutex" groups
   - up to 32 voice buses
      - for routing audio between sample zones
      - sample zone cross modulation (RM, AM, FM, ..)
   - arbitrary number of sample loops per zone
      - pitch or time based number of repeats
      - oneshot, forward, backward, pingpong, xfade loop modes
      - loops can be selected and switched via MIDI and mod matrix
   - monophonic and polyphonic glide
      - time or pitch based
      - separate up / down settings
      - sample / envelope / LFO / modseq legato retrig settings
   - 1D / 2D wavetables
      - static cycle length (2..4096 frames)
      - dynamic, pitch-dependent cycle length ("lo-fi timestretch")
   - additive (re-)synthesis
      - up to 256 partials
      - configurable stereo spread
      - up to 8 realtime-interpolatable resynthesis parameter sets
      - partial bit and sample rate reduction
      - conversion to regular wavetables (export to sample clipboard)
   - 4x3 ASR cubic bezier envelopes per voice
      - modulatable attack / decay+sustain / release speeds and levels
      - loopable sustain section
   - 4 LFOs per voice
      - sine / saw up+down / triangle / pulse / noise shapes
      - configurable start phase (fixed or random)
      - delay and fade-in
      - oneshot and loop modes
      - global or per-voice
   - 4 mod sequencers per voice
      - oneshot and loop modes
      - slew
      - forward, backward, pingpong, random, random(no repeat) play modes
      - time, note on, note off, mod rise / fall / edge / zcross step modes
      - up to 16 patterns, selectable via MIDI / mod matrix
   - MIDI or threshold triggered sampling
      - polyphonization of monophonic (oscillator) sound sources
         - update rate is ~300Hz
   - sample start and loop point modulation
      - random, velocity-based, and / or mod matrix controlled
   - voice calibration tables
      - for (subtle) voice modulation
      - up to 6 lanes
   - per-voice plugins
      - free and open source [STFX](https://github.com/bsp2/stfx/) plugin format
      - filters, waveshapers, ring-modulation, distortion, oscillators, ..
      - sample FM (sample read-rate modulation, `SR` plugin destination)
      - sample AM (amplitude modulation, `AM` plugin destination)
      - envelopes / modulation (`VM1`..`VM8` plugin destinations)
   - [Cycle](cycle.html) software modular
      - "offline" sample synthesis
      - can also generate native code / realtime [STFX](https://github.com/bsp2/stfx/) voice plugins

- sample editor
   - cut / copy / paste / trim / fade-in+out / crossfade / invert / reverse / normalize / declick / ..
   - high quality resampling and other effects via [SoX](https://sox.sourceforge.net/)
   - time stretching / pitch shifting via [Rubberband](https://breakfastquay.com/rubberband/)
   - undo / redo
   - snap to zero crossings or quarter notes (BPM based)
   - automatic region detection
   - automatic pitch detection
   - automatic loop point detection
   - `.wav`, `.8svx`, `.aiff`, `.aiffc`, `.au`, `.caf`, `.cdda`, `.cdr`, `.flac`, `.ogg`, `.opus`, `.sds`, `.txw`, `.voc`, `.w64` import

- sample import from field recorders
   - audition sample (regions) via keyboard or MIDI
      - oneshot or loop playback
   - copy selected samples to local folder
   - tested with Zoom H4n pro and Tascam DR-05x

- instrument and effect plugin host
   - CLAP (Windows, macOS, Linux(todo: editor UIs))
   - VST2 (Windows, macOS, Linux(todo: editor UIs))
   - STFX (Windows, macOS, Linux)
   - plugin browser(s)
   - patch browser(s)
      - import / export `.clp` (CLAP)
      - import / export `.fxp` / `.fxb` (VST2)
   - generic parameter editors with filterable parameter selection
      - in addition to plugin editor UIs
   - up to 32 MIDI-controllable parameter sets per module instance
      - abs and smooth interpolation modes
      - up to 5 parameter set groups per track

- multi-core rendering
   - automatic or explicit track CPU core assignment

- multi-track stem recording
   - `.wav` export
   - recorded audio (regions) can immediately be played via MIDI
      - "live capture" mode: record+autotrim incoming audio
   - various options for creating new multi-samples from recorded audio

- multi-track timeline editor
   - `Timeline` track mode
      - arbitrary number of simultaneous timelines (one per track)
   - up to 128 MIDI-program-changeable variations per timeline
      - e.g. for splitting larger timelines into sections which are then arranged in Synergy
      - or for switching audio timelines in a live performance

- (not released, yet): stand-alone player library
   - up to 256 tracks
   - supports (almost) every sampler feature
   - supports procedural samples ([Cycle](cycle.html))
   - supports (custom) STFX voice and track plugins


# Prerequisites

Hardware:
- a PC running Windows 10 or higher
- or a PC or other computer running Linux (e.g Debian 12)
- or a Mac running macOS 10.12+ (x64) or 13.x+ (arm64)
- an audio interface capable of low-latency I/O (required on Windows, recommended on macOS)

Software:
- Synergy sequencer

>>>note_cc
For building custom STFX plugins from [Cycle](cycle.html) patches, a C compiler is required (e.g. [Clang](https://clang.llvm.org/) or [GCC](https://gcc.gnu.org/)).
Please see the "STFX plugin export: compiler command line" section in `eureka_config.tks` or the [Cycle manual](cycle.html#compiler-installation) for installation details.
<<<
note: $(buf!note_cc)

# Installation

1) install [Synergy](http://miditracker.org/#downloads)
2) open the "stop button" menu and select `Start "Eureka" audio host..` to launch the application
3) in Eureka, open the preferences dialog (`Prefs..` button), and configure the audio device, sample rate, paths, .. as required.
    - The settings will be saved to `eureka_config_local.tks` when the preferences dialog is closed.


## MIDI loopback (deprecated)

tip: since v20230803, Synergy and Eureka come with a built-in virtual MIDI driver ("LocalMIDI") so the following MIDI loopback configuration is not necessary anymore.

1) install MIDI loopback device driver (e.g. [loopMIDI](https://www.tobias-erichsen.de/software/loopmidi.html) on Windows), or use the built-in IAC driver on macOS
2) add two loopback devices named `remote_audio` and `remote_seq`
3) add four loopback devices named `vst_a`, `vst_b`, `vst_c`, `vst_d` (optional: also add `vst\_e`..`vst\_h`)
4) set `b_enable_localmidi = false;` in both `synergy_config.tks` and `eureka_config.tks`

note: on macOS, launch the `Audio MIDI Setup.app`, select `Window => Show MIDI Studio`, double click the IAC driver (red), set Device Name to "" (empty string), then add the `remote\_audio`, `remote\_seq`, `vst\_a`, `vst\_b`, `vst\_c`, `vst\_d`, `vst\_e`, `vst\_f`, `vst\_g`, `vst\_h` MIDI ports, and click "Apply".



## Optional

 1) open `eureka_config.tks` in a text editor
 2) adjust `b_minimal_startup` variable (`false`=enable VST/voiceplugin/samplecache scans)
 3) adjust `rootpath` variable
 4) adjust `audiotrack_wav_rootpath` variable
 5) adjust `exported_samples_wav_rootpath` variable
 6) adjust `project_samples_wav_rootpath` variable
 7) adjust `sample_rootpath` variable
 8) adjust `b_force_single_thread` variable (set to `false` to enable multicore rendering (default on macOS))
 9) adjust `clap_paths` / `vst_paths` variables
10) adjust `atrk_rec_skip_num_frames` (latency compensation) as required (default=-80)

note: on macOS, multi-threaded audio rendering is enabled by default
note: on Windows, for best multi-core performance, clone of the `SetAffinity_*()` functions in `eureka_config.tks` and adjust it to your CPU (CPU core info is printed to console during startup, look for `[dbg] cpuinfo`). 


## Audio input and output channel aliases

 1) open `eureka_config_local.tks` in a text editor
 2) edit the `audio_in_aliases` and / or `audio_out_aliases` arrays


# UI

## Windows and dialogs
- RMB-clicking a window title bar closes the window (e.g. the Scope or wave browser)
- LMB-clicking outside a popup-type dialog or pressing `ESCAPE` closes it (most other windows)

## FloatParams
- vertical RMB-drag or cursor `UP` / `DOWN` coarse edits the value
- horizontal RMB-drag fine edits the value
- holding `lshift` before / while starting to drag increases the coarse / fine precision (ten-fold)
- holding `lctrl` snaps to integers
- LMB-click during RMB-drag reverts the value (same with sliders and dials)
- mouse WHEEL up / down increments / decrements the value in coarse steps
- `lshift` WHEEL or cursor `UP` / `DOWN` increments / decrements the value in fine steps
- LMB-hold-click or `p` shows the preset combobox popup (if there are any presets)
- LMB-double-click cycles through reset values
- RMB-click or `SPACE` shows FloatParam dialog (dial + range edit + reset)
   - note: some parameters have auto-extending ranges which can be reset here
- `t` / `r` scale the value by +-50%
- `i` / `u` double / halve the value
- `n` flips the sign
- `h` toggles hexadecimal mode
- `m` toggles mouse drag precision (float value decimal places, default=enabled)

note: many floatparam widgets use different precisions (decimal places) for mouse drag and text editing (i.e. mouse drag is often less precise)

## Statusbar
- displays messages
- doubles as a command line for executing "page commands" (e.g. resize / allocate waveform on sample page)
   - `lctrl-backtick` (or `^`) sets keyboard focus
   - RMB click shows presets commands (with default args)
   - `UP` shows history
   - `DOWN` shows presets

## Tables
- MMB-click toggles between paging and row selection mousewheel scrolling mode
- `a` opens the context menu (if any)
- RMB-click previews the selected item or starts cell editing (when available)
- RMB-drag or `lshift`-LMB-click selects multiple items / range (when available) (`lctrl` click toggles item selection)
- `SPACE` or `RETURN` selects item (same as double-clicking row)


# Keyboard Shortcuts

note: shortcuts assume a US keyboard layout (unless stated otherwise).

## Global

   |noheader
   |:a             |:open context menu (e.g. in tables)
   |:lshift - q    |:quit
   |:RCTRL         |:toggle recording
   |:numpad ENTER  |:
   |+\=            |:toggle replay
   |:BACKSPACE     |:stop all voices
   |:q             |:toggle scope
   |:SCROLL-LOCK   |
   |:h             |
   |+BACKQUOTE     |+hold to play 261.63Hz (middle C) reference tone
   ].kbd_table Global keyboard shortcuts

## "Audio" page
   |noheader
   |:ESCAPE             |:
   |+LMB double-click   |+ show projects page
   |:F1                 |: show samples page
   |:n                  |:Create new audio track
   |:SPACE              |:Start / Stop audio track replay
   |:PAGEUP             |:Select previous audio track
   |:PAGEDOWN           |:Select next audio track
   |:lalt - i           |:Show sample import dialog
   |:lalt - l           |:Load sequencer project
   |:lctrl - 2          |:show Project page
   |:lctrl - 3          |:show Track page
   |:lctrl - 1          |:
   |+lctrl - 5          |+show Samples page
   ].kbd_table Audio Page keyboard shortcuts

## "Samples" page

   |noheader
   |:ESCAPE             |:
   |+LMB double-click   |+ cancel sample filter, or hide sample browser, or return to previous page
   |:F2                 |:edit sample or zone name (when table has keyboard focus)
   |:F9                 |:
   |+lctrl - 9          |+toggle WAV sample browser
   |:F10                |:
   |+lctrl - 0          |+toggle zone drumpad view
   |:F11                |:show synth dialog (Cycle)
   |:w                  |:show synth dialog (Cycle)  OR  show FM Stack editor
   |:lctrl - TAB        |:toggle zone table model (show names or velocity/mod ranges)
   |:LEFT               |:
   |+s                  |+Focus previous table
   |:RIGHT              |:
   |+d                  |+Focus next table
   |:lshift - f         |:Focus sample name filter
   |:lctrl - f          |:Focus namespace filter and show selector (UP focuses sample table)
   |:lctrl - lshift - f |:Focus sample namespace and show selector (UP focuses sample table)
   |:1                  |:select all zones
   |:2                  |:select previous zone
   |:3                  |:select next zone
   |:4                  |:edit previous ModSample OR previous sample pad of parent track
   |:5                  |:edit next ModSample OR next sample pad of parent track
   |:b                  |:show sample patch browser (add new sample(s))
   |:lshift - b         |:show sample patch browser (replace sample)
   |:c                  |:show Cycle synth editor + curve editor (if sample is procedural)
   |:lctrl - x d        |:post-rec re-init drum sample
   |:e                  |:select and zoom to highlighted (loop) area in sample view  OR  exec default row action (e.g. edit cycle patch)
   |:g                  |:show param editor dialog for currently selected voice plugin
   |:lshift - n         |:create new zone from clipboard
   |:lctrl - s          |:
   |+lctrl - x s        |+save sample as
   |:t                  |:edit sample tags
   |:lctrl - t          |:auto-assign (guess) sample tags
   |:lshift - t         |:show sample tags menu
   |:v                  |:edit last edited volume envelope
   |:lshift - v         |:show volume envelope quick-preset selector
   |:m                  |:show sample mixer dialog
   |:F1                 |:show Audio page
   |:lctrl - 1          |:show Audio page
   |:lctrl - 2          |:show Project page
   |:lctrl - 3          |:show Track page
   ].kbd_table Samples Page keyboard shortcuts

..when samples table has keyboard focus:

   |noheader
   |:F2              |:edit sample name
   |:INSERT          |:create new sample
   |:lshift - INSERT |:clone selected sample
   |:DELETE          |:delete selected sample
   |:lctrl - DELETE  |:delete selected sample without confirmation
   |:lctrl - UP      |:move selected sample up
   |:lctrl - DOWN    |:move selected sample down
   ].kbd_table Samples Page Sample Table keyboard shortcuts

..when zone table has keyboard focus:

   |noheader
   |:F2              |:edit zone name
   |:INSERT          |:create new zone
   |:lshift - INSERT |:clone selected zone
   |:lctrl - INSERT  |:clone zone synth (Cycle)
   |:DELETE          |:delete selected zone
   |:lctrl - DELETE  |:delete selected zone without confirmation
   |:lctrl - UP      |:move selected zone up
   |:lctrl - DOWN    |:move selected zone down
   ].kbd_table Samples Page Zone Table keyboard shortcuts

..when loops table has keyboard focus:

   |noheader
   |:INSERT          |:add loop
   |:lshift - INSERT |:clone loop
   |:DELETE          |:delete loop
   |:lctrl - UP      |:move selected loop up
   |:lctrl - DOWN    |:move selected loop down
   ].kbd_table Samples Page Loops Table keyboard shortcuts

..when voice plugins table has keyboard focus:

   |noheader
   |:F12      |:rescan+reload voice plugins
   | DELETE   | set current plugin slot type to "-"
   | INSERT   | set focus to plugin type combobox and show selector
   | g        | show param editor dialog
   ].kbd_table Samples Page Plugin Table keyboard shortcuts

..in sample zone envelope curve dialog:

   |noheader
   |:ESCAPE   |:
   |+v        |+hide dialog
   |:1        | edit attack envelope
   |:2        | edit sustain envelope
   |:3        | edit release envelope
   |:4        | edit last edited freq envelope
   |:5        | edit last edited volume envelope
   |:6        | edit last edited pan envelope
   |:7        | edit last edited aux envelope
   ].kbd_table Samples Page Plugin Table keyboard shortcuts

note: see Cycle curve editor for all bezier editor shortcuts

..in Cycle synth patch main dialog:
   |noheader
   |:ESC OR F11         |:hide dialog
   |:a                  |:show tools menu
   |:c                  |:show curve editor
   |:lctrl - c          |:copy patch to clipboard
   |:d                  |:force sample recalc / plugin export
   |:lctrl - d          |:cycle through auto-recalc modes
   |:e                  |:show machine source editor
   |:f                  |:fit dialog to \#args
   |:lctrl - n          |:add new sine lane
   |:RALT               |:
   |+lctrl - p          |+rebuild + reload voice plugin or recalc sample
   |:lctrl - lshift - p |: toggle auto-export / auto-recalc
   |:lctrl - TAB        |:show curve editor
   |:lctrl - SPACE      |:show machine source editor
   |:r                  |:randomize last selected arg group
   |:lshift - r         |:randomize all arg groups
   |:lctrl - s          |:force sample recalc / plugin export
   |:lctrl - lshift - s |:save machine preset
   |:lctrl - v          |:paste patch from clipboard
   |:lshift - v         |:paste shared patch from clipboard
   |+lctrl - w          |+rebuild + reload voice plugin or recalc sample
   |:lctrl - lshift - w |: toggle auto-export / auto-recalc
   ].kbd_table Cycle source editor keyboard shortcuts

..in Cycle synth patch machine source editor:
   |noheader
   |:ESC             |:
   |+lctrl - SPACE   |+hide source editor (and save source changes)
   |:lctrl - f       |:find text (press again to find next occurence)
   |:lctrl - g       |:
   |:lctrl - x SPACE |+edit include file (or template) under cursor (press esc to return to machine source)
   |:lctrl - h       |:copy include file pathname under cursor to clipboard (e.g. for external editors)
   |:lctrl - i       |:expand current input under cursor
   |:lctrl - p       |:rebuild + reload voice plugin or recalc sample
   |:lctrl - s       |:save source changes (and auto-rebuild / recalc when enabled)
   |:lctrl - t       |:show selection dialog and insert `inc` statements for selected (template-)includes
   |:lctrl - w       |:rebuild + reload voice plugin or recalc sample
   |:TAB             |:increase indentation of line or block
   |+                |+ OR select / edit curve (alt: RMB-click)
   |+                |+ OR edit number under cursor (when there is no selection and cursor is placed after first word)
   |:lshift - TAB    |:decrease indentation of line or block
   |:lctrl - TAB     |:show curve editor dialog
   |:lctrl - x TAB   |:set keyboard focus to embedded curve editor
   |:lalt - LEFT     |:show previous patch-buffer
   |:lalt - RIGHT    |:show next patch-buffer
   ].kbd_table Cycle source editor keyboard shortcuts

>>>note_numberedit
Number-edit (via TAB or RMB-drag) supports value-range annotations which use the 
"# <min> <max> [<prec> [<stepCoarse> [<stepFine>]]]
<<<
tip: $(buf!note_numberedit)

..in Cycle curve editor:
   |noheader
   |:ESC           |:unzoom envelope or hide editor
   |:c             |:hide editor
   |:lctrl - TAB   |:show machine editor
   |:lshift - 1    |:
   |+5             |+show curves 1..4
   |:lshift - 2    |:
   |+6             |+show curves 5..8
   |:lshift - 3    |:
   |+7             |+show curves 9..12
   |:lshift - 4    |:
   |+8             |+show curves 13..16
   |:1             |:(un-)zoom 1st curve in current curve group
   |:2             |:(un-)zoom 2nd curve in current curve group
   |:3             |:(un-)zoom 3rd curve in current curve group
   |:4             |:(un-)zoom 4th curve in current curve group
   |:e             |:(un-)zoom currently focused curve
   |:lctrl - f     |:toggle draw fill area
   |:lctrl - p     |:
   |+lctrl - w     |+
   |+RALT          |+
   |+RCTRL         |+export and reload voice plugin
   ].kbd_table Cycle curve editor keyboard shortcuts

..in Cycle curve editor or envelope curve editor:
   |noheader
   |:TAB                        |: minimize / close editor dialog / refocus machine text editor
   |:lctrl - a                  |:
   |:RMB hold                   |+
   |:lctrl + RMB                |+
   |+right windows key          |+show context menu
   |:LMB+RMB hold + mouse drag  |:zoom x/y (hold LMB for 0.25s, then also hold RMB, then drag mouse)
   |:lctrl - SPACE              |:reset zoom
   |:lctrl - lshift - SPACE     |:toggle draw envelope (debug)
   |:DELETE                     |:delete selected segments
   |:mouse over point           |:show coordinate tooltip
   |:LMB doubleclick            |:create new center point
   |:lshift + LMB doubleclick   |:create new center point and straight line from previous point
   |:LMB + mouse drag           |:move points  (press RMB while still holding LMB to undo)
   |:LMB hold + mouse drag      |:multi-select center points within rectangular region
   |:lctrl + LMB + mouse drag x |:warp points around selected center points
   |:RMB + mouse drag           |:(toggle) symmetric control point translation
   |:lctrl - lshift - mouse drag|:(toggle) symmetric control point distance scaling
   |:lalt + mouse drag          |:
   |+MMB + mouse drag           |+pan
   |:lshift + mouse drag        |:scale control point distance
   |:lalt + mouse wheel         |:zoom X
   |:lalt + lctrl + mouse wheel |:zoom Y
   |:mouse wheel                |:select previous / next point
   |:lctrl - mouse wheel        |:select previous / next center point
   |:LEFT                       |:select previous control point in current segment
   |:RIGHT                      |:select next control point in current segment
   |:UP                         |:
   |+d                          |+select next center point
   |:DOWN                       |:
   |+a                          |+select previous center point
   |+lctrl - e                  |:toggle drag-rotate mode (bend curve segments by LMB-dragging them)
   |:f                          |:flip x
   |:lshift - f                 |:flip y
   |:lctrl - lshift - f         |:make seamless (align last to first)
   |:lctrl - f                  |:toggle draw-fill
   |:g                          |:toggle grid
   |:lshift - g                 |:cycle grid subdivision
   |:s                          |:toggle snap-to-grid
   |:u                          |:zoom out (x)
   |:i                          |:zoom in (x)
   |:o                          |:zoom out (y)
   |:p                          |:zoom in (y)
   |:m                          |:toggle smooth-edit mode (def=disabled)
   |:lshift - m                 |:toggle smooth-edit x-dist-only submode (when enabled, ignore y-distance) (def=enabled)
   |:lctrl - m                  |:toggle smooth-edit control-point-dist submode (when enabled, scale control points) (def=enabled)
   |:n                          |:smooth all (10%)
   |:lctrl - n                  |:smooth all (25%)
   |:lctrl - lshift - n         |:smooth all (50%)
   |:lctrl - q                  |:simplify (coarse) (e.g. after FMStack export)
   |:lctrl - lshift - q         |:simplify (fine)
   |:lctrl - r                  |:make all control points with angle < 45 degrees symmetrical
   |:lctrl - lshift - r         |:make all control points symmetrical
   |:lctrl - b                  |:set current envelope as backdrop
   |:lctrl - lshift - b         |:clear backdrop
   |:lctrl - y                  |:
   |:lctrl - z                  |:undo
   |:lctrl - lshift - y         |:
   |:lctrl - lshift - z         |:redo
   ].kbd_table Bezier curve editor keyboard shortcuts


## "Project" page

   |noheader
   |:WHEEL UP           |:select previous track
   |:WHEEL DOWN         |:select next track
   |:cursor keys        |:select track
   |:lctrl WHEEL UP     |:decrease track size
   |:lctrl WHEEL DOWN   |:increase track size
   |:1..6               |:select track size 1..6
   |:e                  |:
   |+LMB double-click   |+edit focused track
   |:f                  |:edit first ModSample or ModCLAP or ModVST2 of focused track
   |:d                  |:mute focused track
   |:s                  |:solo focused track
   |:n                  |:add new track
   |:lctrl - n          |:add new track and edit it
   |:lshift - n         |:add new audio track for selected MIDI track and edit it
   |                    |+e.g. "drums" => "a_drums"
   ].kbd_table Project Page keyboard shortcuts

## "Track" page

   |noheader
   |:LEFT                 |:select previous track
   |:RIGHT                |:select next track
   |:F3                   |:show/hide plugin browser
   |:RETURN               |:edit first or last selected mod
   |:lctrl - TAB          |:
   |+lctrl - r            |+toggle modmatrix / lanes view (alt key: F4, lctrl-4)
   |:lctrl - q            |+show / hide browser
   |:lctrl - w            |+show browser and select default list type
   |:lctrl - h            |:sync mods on other lanes with last selected mod
   |:lctrl - lshift - h   |:sync mods + modmatrices on other lanes with last selected mod
   |:lctrl - j            |:sync other lanes with last selected lane
   |:lctrl - lshift - j   |:sync other lanes + modmatrices with last selected lane
   |:lctrl - l            |:load track patch
   |:lctrl - s            |:save track patch
   |:lctrl - q            |:hide plugin browser or show Instrument (or STFX) plugins
   |:lctrl - f            |:toggle VST2 Effect / Instrument plugin browser
   |:lctrl - e            |:show track patch variations table
   |:lctrl - t            |:show STFX plugin browser
   |:lctrl - lshift - t   |:enable experimental (resampler) menu items / keys
   |:lctrl - z            |:
   |:lctrl - y            |+show CLAP plugin browser
   |:lctrl - 1            |:show Audio Tracks page
   |:lctrl - 2            |:
   |+LMB double click     |+show Project page
   ].kbd_table Track Page keyboard shortcuts


lane modules:

   |noheader
   |:LMB hold or double click |:show (CLAP / VST2) patch browser
   |:lshift - LMB click   |:show generic param editor instead of regular UI
   |:lalt - LMB click     |:show parameter set editor
   ].kbd_table Track Page keyboard shortcuts

when CLAP / VST2 editor window is focused:

   |noheader
   |:ESC   |:hide plugin window
   | `     | sync MPE/LPC/VRR mirror mods (^ on german kbd)
   | F12   | add mod matrix entry for last touched parameter
   | RCTRL | toggle audio(-track)recording
   ].kbd_table CLAP / VST2 Window keyboard shortcuts

..timeline track mode:

   |noheader
   |:SPACE                |:start/stop replay (start from left loop locator, rewind to selection/clip start)
   |:lctrl - SPACE        |:start/stop replay (start from cursor pos, rewind to cursor position)
   |:LEFT                 |:move edit cursor left
   |:lctrl - LEFT         |:move edit cursor left (fast)
   |:lshift - LEFT        |:extend / start selection to previous grid unit
   |:RIGHT                |:move edit cursor right
   |:lctrl - RIGHT        |:move edit cursor right (fast)
   |:lshift - RIGHT       |:extend / start selection to next grid unit
   |:UP                   |:move edit cursor to previous track
   |:lshift - UP          |:extend / start selection to previous track
   |:DOWN                 |:move edit cursor to next track
   |:lshift - DOWN        |:extend / start selection to next track
   |:INSERT               |:insert space (snap-grid-units when there's no selection)
   |:lshift - INSERT      |:insert space (snap-grid-units on all tracks when there's no selection)
   |:DELETE               |:delete selected clips or space
   |:lctrl - a            |:select all track clips, all clips, or none
   |:lctrl - lshift - a   |:select none
   |:lctrl - c            |:copy selected clips to clipboard
   |:c                    |:set loop to selected clips, region, or canvas (prefer clips)
   |:lshift - c           |:set loop to selected region, clips, or canvas (prefer region)
   |:lalt - c             |:copy timeline to clipboard
   |:e                    |:erase(clear) selection (split clip if required) (do not modify samples) OR select 'erase' mode
   |:lctrl - v            |:paste SampleView clipboard as new sample+clip OR paste timeline clips from clipboard (depends on which clipboard is more recent)
   |:lctrl - lshift - v   |:insert space on all tracks and shift-paste timeline clips from clipboard
   |:lalt - v             |:paste timeline from clipboard
   |:l                    |:toggle draw-clip-labels
   |:m                    |:merge selected clips (do not modify samples)
   |:n                    |:toggle normalize-waveform-display
   |:r                    |:split selected clips at cursor position OR select 'razor' mode
   |:lshift - r           |:expand selection to all tracks and split at cursor position
   |:s                    |:toggle snap-to-grid
   |:lshift - s           |:cycle snap-to-grid units
   |:t                    |:toggle mute selected clips or select 'mute' mode
   |:q                    |:select 'edit' mode
   |:w                    |:move cursor to top/left of selection OR create clip from selection OR select 'draw' mode
   |:lctrl - w            |:cut selected clips to clipboard
   |:lctrl - y            |:undo (lctrl-z on german kbd)
   |:lctrl - lshift - y   |:redo (lctrl-lshift-z on german kbd)
   |:lalt - z             |:select clips until end (lalt-y on german kbd)
   |:mouse wheel          |:scroll vertically OR zoom in / out
   |:lctrl - lshift - mouse wheel |: zoom in / out
   |:lctrl - mouse wheel  |:inc / dec track height
   |:lshift - mouse wheel |:scroll horizontally
   |:LMB-drag             |:select region / clips within region
   |:LMB-click            |:select clip
   |:LMB-dblclick         |:toggle clip zoom
   |:RMB-click            |:edit clip sample
   |+                     |+ while dragging clip position or length: mini undo
   ].kbd_table Timeline keyboard shortcuts


## Sample View

| Key Sequence           :|:Description
| lctrl - lshift - SPACE  | Show all
|          lctrl - SPACE  | Fit selection to window or show all
|                      a  | Show context menu
|     lctrl - lshift - a  | Select none
|           LMB dblclick  | Set selection to highlighted area or Select none
|              lctrl - a  | Select all / show all
|              lctrl - b  | Declick selection end (512 sample frames fade out)
|     lctrl - lshift - b  | Declick selection start (512 sample frames fade in)
|              lctrl - c  | Copy selection to clipboard
|     lctrl - lshift - c  | Append selection to clipboard
|                 RETURN  | Append selection to clipboard
|              lctrl - d  | Silence selection
|     lctrl - lshift - d  | Surround selection with silence (for transient detection tools)
|                      e  | Select and zoom to highlighted area
|              lctrl - e  | (partially handled by SampleView) Export selection to WAV (send event)
|     lctrl - lshift - e  | (partially handled by SampleView) Multi-Export regions to WAV (send event)
|              lctrl - f  | Fade-out selection
|     lctrl - lshift - f  | Fade-in selection
|              lctrl - g  | Toggle show highlight
|              lctrl - h  | Copy selection to highlight (modifies either sampleoff/len or current loop off/len)
|              lctrl - i  | Toggle show RMS
|              lctrl - j  | Toggle show regions
|             lshift - j  | Toggle RMS autoshop mode
|     lctrl - lshift - j  | Toggle autochop dialog
|     lctrl - lshift - i  | Toggle show samples
|              lctrl - k  | Delete left channel
|     lctrl - lshift - k  | Delete right channel
|     lctrl - lshift - l  | Clear undo/redo history
|              lctrl - m  | Convert to mono
|     lctrl - lshift - m  | Convert to stereo
|              lctrl - n  | Normalize selection
|     lctrl - lshift - n  | Normalize regions
|              lctrl - o  | Amplify selection (show dialog)
|              lctrl - p  | Phase-invert selection
|              lctrl - q  | Select next region, or select next cycle (in cycle-edit mode) (same as cursor RIGHT)
|     lctrl - lshift - q  | Select previous region, or select previous cycle (in cycle-edit mode) (same as cursor LEFT)
|              lctrl - r  | Reverse selection
|     lctrl - lshift - r  | Crossfade selection
|             lshift - r  | Loop-xfade selection. Needs 128 frames before selection start.
|              lctrl - s  | Tighten selection (via transient detection)
|                      s  |
|              MMB click  |+Toggle BPM snap / Marker snap (off (zero-crossing if enabled) => marker => bpm)
|             lshift - s  | Cycle BPM snap grid (1/16, 1/8, 1/4, 1/1)
|     lctrl - lshift - s  | Show SoX / Rubberband dialog
|              lctrl - t  | Truncate to selection
|     lctrl - lshift - t  | Auto-trim (select all, tighten selection, trim)
|              lctrl - v  | Paste clipboard at end of selection
|     lctrl - lshift - v  | Replace selection by clipboard
|             lshift - v  | Add clipboard to selection (mix)
|              lctrl - w  | Cut selection to clipboard
|              lctrl - y  | Undo ('z' on german kbd)
|     lctrl - lshift - y  | Redo ('z' on german kbd)
|              lctrl - z  |
|               MMB hold  |+Toggle snap-to-zero-crossing mode ('y' on german kbd)
|             lshift - z  | Toggle cycle edit mode (same as MMB click) ('y' on german kbd)
|                     UP  | Zoom in
|                   DOWN  | Zoom out
|                   LEFT  | Select previous region, or select previous cycle (in cycle-edit mode)
|           lctrl - LEFT  | Scroll left
|  lctrl - lshift - LEFT  | Move region left (+add silence around region)
|                  RIGHT  | Select next region, or select next cycle (in cycle-edit mode)
|          lctrl - RIGHT  | Scroll right
|                      `  | Toggle freedraw mode (^ on german kbd)
| lctrl - lshift - RIGHT  | Move region right (+add silence around region)
|                 DELETE  | Delete selection
|                   HOME  | Move selection start to 0
|                    END  | Move selection end to end of waveform
|                         |+  Press twice to move selection start to end of wave
|                    END  | Set selection start to end of selection or end of wave
|           lshift - END  | Expand selection end to end of waveform
|                         |+ Press twice to move selection start to end of wave
|          WHEEL UP/DOWN  | Zoom in/out
|                         |+  Select previous / next cycle (in cycle-edit mode)
|                         |+  Select previous / next region (when selection exceeds max cycle length)
|          lctrl + WHEEL  | Zoom vertically
|          lctrl + WHEEL  | Zoom vertically in cycle mode
|        RMB + MOUSEMOVE  | Scroll left / right
|        LMB + MOUSEMOVE  | Mark region / autoscroll
|               RMB hold  | Show popup menu
|                    MMB  | Cycle bpm/marker snap modes
| LMB click in upper area | Set marker
| RMB click in upper area | Delete marker
|      lctrl + LMB click  | Toggle marker
].kbd_table Sample View keyboard shortcuts



# MIDI implementation

## vst_a .. vst_z ports

- can be assigned to instrument tracks
- events will be forwarded to all lanes / modules matching the event's MIDI channel and module's mod matrix filter
   - in default track mode: sent to all track lane-modules
   - in LPC track mode: sent to the lane(-modules) matching the event's MIDI channel
      - "Lane Offset" determines which lane is mapped to MIDI channel #1
   - in MPE track mode: sent to the lane(-modules) matching the event's MIDI channel.
      - "Master channel" events (usually on MIDI channel #1) are sent to all MPE lanes
      - "Lane Offset" determines which lane is mapped to the first available MIDI channel (usually MIDI channel #2)
   - in [drum pad sampler mode](#drum-pad-sampler-drum): sent to all track lane-modules. Notes will trigger the matching drum pad.
      - pitchbend and modwheel / breathctl / footctl / volume / balance / pan / expression / brightness / general 1..8 performance CCs (and All Notes Off CC) are forwarded to all pad sample players
      - MIDI notes C-3..G-3 (36..43) select the currently active drum kit variation


## remote_audio port

- primarily used for receiving SysEx requests / replies from the "Synergy" sequencer process

- context-sensitive MIDI event routing
   - the current instrument track can be played via the `remote_audio` MIDI port (in addition to its main MIDI port `vst_a`..`vst_h`)
   - the current sample (on the sample page) can be played via the `remote_audio` MIDI port, **unless** the sample page was entered by clicking on a `ModSample` instance
   - the current audio track (on the start page) can be played via the `remote_audio` MIDI port

- can also be used for receiving direct module parameter changes (i.e. without track or sampler mod matrix mapping)
   - see "Remote" tracker instrument parameter type in "Synergy"

note: in Synergy, the `remote_audio` device is also used for context-sensitive routing of MIDI events (e.g. in synth editors)


## remote_seq port

- used for sending SysEx requests / replies to the "Synergy" sequencer process


## Module specific MIDI events

### Sampler

#### Start offset (audio clips)
`RPN_SMP_OFFMS_LO` (88), `RPN_SMP_OFFMS_HI` (89) set the sample start offset in milliseconds
This is used for audio clips in the Synergy Arranger and for "audio patterns" in the Synergy Tracker node.

#### Sample selection by unique id (UID)
`RPN_SMP_UID` (90) selects a sample by its unique id.
Can be used as an alternative to MIDI program changes.

#### Sample selection by namespace-local index
`RPN_SMP_NSP_ABS` (80) selects a sample in the current sample's namespace using a namespace-local index. 0=no change, 1..n selects sample 1..n (see "Local" column in the samples table)

`RPN_SMP_NSP_REL` (81) selects a sample using a normalized parameter value in the range 0..16383 (e.g. when mapped to a HW controller)

This feature is useful for selecting sample variations without the need to assign MIDI program numbers to each and every variation.

#### Performance controllers
- pitchbend, channel+poly pressure, and performance controller `CC#1`/`CC#33` (modwheel), `CC#2`/`CC#34` (breath control), `CC#4`/`CC#36` (foot control), `CC#11`/`CC#43` (expression pedal), `CC#7`(volume), `CC#8`(balance), `CC#10`(panorama), `CC#16`..`CC#19` (general purpose 1..4), `CC#74` (brightness), `CC#80`..`CC#83` (general purpose 5..8) can be mapped in the sampler mod matrix
   - the performance CCs support 7bit (MSB) and 14bit (MSB+LSB) precision
- `CC#64` (sustain pedal) sustains all pressed notes (when value is >= 64)
- `CC#65` (portamento switch) allow glide when value is >= 64 (def=yes)
- `CC#66` (sostenuto pedal) sustains only the currently pressed notes (when value is >= 64)
- `CC#121` (controller reset) resets all performance controllers
- `CC#127` (all notes off) stops all voices


### CLAP, VST2 and STFX plugins

#### Mod Matrix
Each module instance has a mod matrix that can be used to map MIDI events to module parameters.

note: this is not to be confused with the sampler mod matrix, which is processed per-voice


#### Param Sets

Parameter sets are used to switch or interpolate module parameters.
Each module supports up to 32 parameter sets, and each parameter set stores an arbitrary number of parameter values.

A module's param set editor is invoked via the module context menu, by clicking the middle mouse button, or by holding LALT and clicking the left mouse button.

The group number (1..5) assigned to a param set determines which MIDI RPNs are used to modulate the param set selection.

- `RPN_PARAMSET_1..5` (70..74) select a parameter set for group 1..5 (without interpolation)
   - value range is 1..32
- `RPN_PARAMSET_SMOOTH_1..5` (75..79) select a parameter set for group 1..5 (with interpolation)
   - value range is 0..16383
   - 512 fractional steps between two parameter sets
   - values 15872..16383 interpolate between parameter sets 32 and 1



## Track / Lane Fading RPNs and CC

`RPN_LANE_FADE_MASK` (85) and `RPN_LANE_FADE_NR` (86) (received on a track's MIDI port) fade in a set of lanes or a single lane (and fade-out the deselected ones).

This feature is enabled by ticking the `Fade` checkbox on the Track page.

Alternatively, a configurable MIDI CC can be used to fade-in a specific lane (the FloatParam widget next to CheckBox selects the controller number).

The fade-in and fade-out speeds are configured using the "slew" widgets.

While especially useful for ambient music, this essentially is a general purpose crossfader between an arbitrary number of sounds.



# Files and Directories

## Project

When a project is saved in Synergy, it will also be saved in Eureka.
The Eureka project name is determined by the Synergy project name.

For example, when the Synergy project is named `test`, the following Eureka project files and directories will be written:
~~~~
mProjects/eureka_data/projects/test.epr
mProjects/eureka_data/projects/test/
mProjects/eureka_data/projects/test/samples/
mProjects/eureka_data/projects/test/tracks/
mProjects/eureka_data/projects/test/exported_samples/
~~~~

### samples
`samples/` contains the `.wav` files for the samples along with the `.esa` sampler patches, e.g.
~~~~~
samples/mysample.esa
samples/mysample.wav
~~~~~

### tracks
`tracks/` contains the `.wav` files for the "audio tracks" (stems and scratchpads).

### exported_samples
`exported_samples/` contains the `.wav` files exported in the (Audio page) sample editor via the `Export Selection` and `Multi-Export Regions` context menu items.


## Library

### Samples
The sample library path is configured in the `Prefs` dialog or by editing `eureka_config_local.tks` (`sample_rootpath`).

By default, it points to
~~~~~
mProjects/samples/
~~~~~
note: the sample library scan result is stored in `samplecache.dat`
tip: it is _not_ recommended to point this directory to huge 3rd party archives. Rather put your favourites and own samples in there.
tip: other samples can be imported into a project from any drive via drag'n'drop
note: the library source path will be preserved when importing samples from the library. this allows the sample to be later saved back to the library again

#### User Samples
The name of the user sample library directory (relative to the sample library path) is configured in the `Prefs` dialog or by editing `eureka_config_local.tks` (`sample_userpath`).

Before changing this setting, make sure that the corresponding directory exists.

For example, when the user sample path is set to `bsp`, the following directories should exist:
~~~~~
mProjects/samples/bsp/
mProjects/samples/bsp/drumkit/
~~~~~

##### Drum Kits
The `drumkit/` directory (within the user sample path) contains patches and samples (`.edk`, `.wav`, `.esa`) for tracks that are set to [Drum Pad Sampler mode](#drum-pad-sampler-drum).

Each drum kit is stored in a subdirectory, i.e. the example drum kit `808_95` is associated with the following files and directories:
~~~~~~
mProjects/samples/bsp/drumkit/808_95.edk
mProjects/samples/bsp/drumkit/808_95_bd.esa
mProjects/samples/bsp/drumkit/808_95_bd.wav
mProjects/samples/bsp/drumkit/808_95_ch.esa
mProjects/samples/bsp/drumkit/808_95_ch.wav
mProjects/samples/bsp/drumkit/808_95_sd.esa
mProjects/samples/bsp/drumkit/808_95_sd.wav
~~~~~~

note: a unique sample namespace should be assigned to drum kits and drum kit samples in order to distinguish them from other samples. The `From Kit` utility function creates / assigns a namespace based on the drum kit's name. `To pads` assigns the selected namespace to all drum kit samples uniquely referenced by the drum kit (and its variations)


### Track patches
Track patches (`.epa` files) are stored in the `patches/` directory, which, by default, points to
~~~~
mProjects/eureka_data/patches/
~~~~

Track patches contain the module, mod matrix, and output setups for all track lanes.

caution: plugins and samples in track patches are stored by reference, i.e. they must exist in the library when the patch is loaded. Samples must be saved to the library (`Save As..`) before saving the track patch (via the `Save` button on the `Current Track` page)

note: a track patch may contain an arbitrary number of variations. By default, a new variation is created each time the patch is saved (see `Auto-create track patch variations` setting in the `Prefs` dialog which is saved in `eureka_config_local.tks`)


### CLAP plugins

#### Plugin Paths
The CLAP plugin paths are configured in `eureka_config.tks` (`clap_paths`).

note: the CLAP scan result is stored in `clapcache.dat`.
note: when `b_clap_deepscan_whitelist` is enabled, the cache is written to / read from `clapcache_wl.dat`
note: plugin paths can also be added via the `$CLAP_PATH` environment variable (`.` separated list on macOS / Linux, `;` on Windows)

#### Patches
CLAP patches are stored in the `clappatch_rootpath` which, by default, points to
~~~~~~
mProjects/eureka_data/clap_patches/
~~~~~~

note: the patch folder contains (auto-created) subdirectories (one per plugin)


### VST plugins

#### Plugin Paths
The VST2 plugin paths are configured in `eureka_config.tks` (`vst_paths`).

note: the VST2 scan result is stored in `vstcache.dat`. Eureka will incrementally scan for added / removed plugins during startup.
note: when `b_vst_deepscan_whitelist` is enabled, the cache is written to / read from `vstcache_wl.dat`

#### FXP / FXB Patches
VST patches are stored in the `vstpatch_rootpath` which, by default, points to
~~~~~~
mProjects/eureka_data/vst_patches/
~~~~~~

note: the patch folder contains (auto-created) subdirectories (one per plugin)

### STFX plugins
STFX plugins are located in the `voice_plugins/` application subdirectory.

#### Patches
STFX patches are stored in the `stfxpatch_rootpath` which, by default, points to
~~~~~~
mProjects/eureka_data/stfx_patches/
~~~~~~

note: the patch folder contains (auto-created) subdirectories (one per plugin)


# Pages

## Audio page

- used to manage an arbitrary number of mono or stereo audio tracks
   - scratch pads
   - stems

- audio tracks can record..
   - the incoming audio of a specific interface channel (pair) (`Audio In` mode)
   - the outgoing audio of a specific interface channel (pair) (`Audio Out` mode)
   - the outgoing audio of a specific track (`Track` mode)

- recording (for all armed tracks) starts..
   - when an MMC record-start message is received from the sequencer
      - e.g. by starting the replay via `lshift - (np) ENTER`
   - or when the record button in the lower right window corner is pressed

- the sample view below the audio tracks list displays the currently selected track
   - the track selection is changed via `PAGEUP` / `PAGEDOWN` keys, mousewheel, or by clicking on a track

- audio track (preview) replay is done via the 'scratch' sample player (routed directly to the main outputs)
   - the widgets below the sample editor configure the basic replay settings, i.e.
      - velocity-to-volume amount
      - release curve (`Gate`, `Short`, `Medium`, `Long`, `Infinite`)
      - volume ramping
      - loop switch
      - polyphony
      - sample start offset
      - sample start offset randomization
      - pan position randomization

- the `Live Capture` mode is used for "hands-free", threshold-level based autosampling
   - sampling starts when the selected input exceeds the silence threshold
      - and stops when the input goes back to silence
      - the recorded sample can immediately be played afterwards
   - for example, with two MIDI keyboards:
      - route one to a monophonic synthesizer (or use the synthesizer's keyboard if it has one)
      - route the audio output of the synthesizer to the audio track
      - route the second MIDI keyboard to the `remote_audio` MIDI port of the sampler
      - each time the synth patch has changed, press and release a key on the synth, then play the auto-recorded sample polyphonically

- new (multi-)samples can be created via the `Create..` popup menu

tip: samples are automatically split into regions which can be selected by double-clicking them, or by cycling through them with the mousewheel or the left and right cursor keys

tip: a new collection of regions can be assembled in the clipboard by copying the first region via `lctrl-c` (replaces the clipboard), then adding further regions by pressing `RETURN`. The regions will be separated by brief periods of silence. The result can be pasted to any sample view (`lctrl-v` (insert), `lctrl-lshift-v` (replace selection))


## Tracks page

- cursor keys or mousewheel change the track focus
   - MIDI events received on the (virtual) `remote_audio` port are redirected to the focused track

- to edit an instrument track, right-click the track's title bar, press the `e` key, or click the `Edit` button (when visible)

- the first sampler or CLAP / VST2 plugin instance of an instrument track can be edited by middle-clicking the track's title bar

- if a track named `master` exists, a new track's audio output will be auto-routed to it

- tracks are reordered by drag'n'dropping a track onto another track (upper half = before, lower half = after).

## Current Track page

- a track consists of *n* lanes, and each lane consists of *m* modules, which are processed in the order of occurence.
   - the output of a track can be routed to other tracks, or audio interface outputs.
      - when it is set to `Default`, the output is routed to the main output (interface channels 1 + 2)
      - each track may have an arbitrary number of outputs
   - the output of a lane can be routed to other lanes, tracks, or audio interface outputs.
      - when it is set to `Default`, the output is routed to the track output(s)
      - each lane may have an arbitrary number of outputs
   - each track, and each lane, has 4 audio inputs:
      - left
      - right
      - side chain left
      - side chain right
   - each module has a mod matrix which is used to map MIDI events to module parameters
      - this is not to be confused with the `ModSample` sample zone mod matrix (which is saved in the sample)
      - cubic bezier modulation curves can be employed to create non-linear parameter modulation
      - the module mod matrix can be queried from the sequencer, e.g. when importing parameters into a `CtlMorph` or `Tracker` node
      - a new mod matrix entry for the last touched (CLAP / VST2) parameter can quickly be added by pressing the `F12` key
      - the mod matrix view will initially display the last highlighted lane module (mouse over)
   - each module also has a MIDI input filter which can restrict the note and velocity ranges, filter out continuous controller (CC), pitchbend, and (poly-)pressure events, and transpose incoming notes

- a new `ModSample` instance is created by right clicking on an empty lane mod slot and choosing `Builtin->Sampler`

- cursor left / right or mousewheel selects the previous / next track

- hold-clicking the background navigates to the parent page (usually the `Tracks` page)
   - on the project and tracks pages, double-clicking accomplishes the same thing

### Track modes

- in general:
   - a modules' MIDI input filter can be used to filter out events, restrict the module to certain note ranges, transpose notes, and apply note-on / note-off velocity curves

#### Default

- incoming MIDI is routed to all lanes / modules

#### Lane Per Channel (LPC)

- for this mode, the MIDI channel selection should be set to `*`

- each MIDI channel is routed to a different lane
   - the lane offset setting determines which lane is mapped to MIDI channel #1
   - the lane number setting determines the number of MIDI channels
      - lanes beyond that may be used for FX processing

#### Mirror Lane Per Channel (MLPC)

- same as LPC except that (CLAP / VST2) parameter edits are automatically mirrored to all other-lane modules in the same grid y position

#### MIDI Polyphonic Expression (MPE)

- for this mode, the MIDI channel selection should be set to `*`

- each MIDI channel is routed to a different lane
   - MIDI events on MIDI channel #1 (*master* channel) are routed to all lanes
   - the lane offset setting determines which lane is mapped to MIDI channel #2
   - the lane number setting determines the number of number of MIDI channels (e.g. 6 = #2..#7)
      - lanes beyond that may be used for FX processing

#### Mirror MIDI Polyphonic Expression (MMPE)

- same as MPE except that (CLAP / VST2) parameter edits are automatically mirrored to all other-lane modules in the same grid y position

#### Voice Round Robin (VRR)

- the lanes are cycled at each note-on
   - the lane offset setting determines which lane is mapped to the first note
   - the lane number setting determines when to cycle back to the first (\<lane offset\>) lane
      - lanes beyond that may be used for FX processing

#### Mirror Voice Round Robin (MVRR)

- same as VRR except that (CLAP / VST2) parameter edits are automatically mirrored to all other-lane modules in the same grid y position

#### Drum Pad Sampler (Drum)

- supports up to 32 drum pads and up to 8 variations
   - pads are played via MIDI notes `C-4` .. `B-5` (bank A) and `C-6` .. `B-7` (bank B)
   - variations are selected via MIDI notes `C-3` .. `G-3`

- incoming MIDI events are routed to all pads and lanes / modules

- each pad can be assigned a sample
   - either by selecting an existing one
   - or by drag'n'dropping a `.wav` file or `.esa` sampler patch onto the pad

- pads can be linked so that a single note plays multiple sounds
   - up to 8 link groups (`a`..`h`)

- pads can be mutually exclusive so that e.g. a closed hihat cuts off an open hihat sound
   - up to 8 mutex groups (`A`..`H`)

- the audio output of each pad's sample player can be sent to up to 8 lanes

##### Drum Pad Layouts

>>>drumlayout8x2a
~~~~
   Pad Layout 8x2 A

   +-------+-------+-------+-------+-------+-------+-------+-------+
   |  BD1  |  SD1  |  CP1  |  LT1  |  MT1  |  HT1  |  CH1  |  OH1  |
   +-------+-------+-------+-------+-------+-------+-------+-------+
   |  BD2  |  SD2  |  CP2  |  MA1  |  CL1  |  CB1  |  RS1  |  CY1  |
   +-------+-------+-------+-------+-------+-------+-------+-------+
    =
   +-------+-------+-------+-------+-------+-------+-------+-------+
   |  C-4  |  D-4  |  E-4  |  F-4  |  G-4  |  A-4  |  A#4  |  B-4  |
   +-------+-------+-------+-------+-------+-------+-------+-------+
   |  C-5  |  D-5  |  E-5  |  F-5  |  G-5  |  A-5  |  A#5  |  B-5  |
   +-------+-------+-------+-------+-------+-------+-------+-------+
~~~~
<<<

>>>drumlayout8x2b
~~~~
   Pad Layout 8x2 B

   +-------+-------+-------+-------+-------+-------+-------+-------+
   |  BT1  | ZAP1  |  FX1  | COPEN | CMUFF | CSLAP |NOISE_S|NOISE_L|
   +-------+-------+-------+-------+-------+-------+-------+-------+
   |  BT2  | ZAP2  |  FX2  |  MA2  |  CL2  |  CB2  |  RS2  |  CY2  |
   +-------+-------+-------+-------+-------+-------+-------+-------+
    =
   +-------+-------+-------+-------+-------+-------+-------+-------+
   |  C-6  |  D-6  |  E-6  |  F-6  |  G-6  |  A-6  |  A#6  |  B-6  |
   +-------+-------+-------+-------+-------+-------+-------+-------+
   |  C-7  |  D-7  |  E-7  |  F-7  |  G-7  |  A-7  |  A#7  |  B-7  |
   +-------+-------+-------+-------+-------+-------+-------+-------+
~~~~
] e.g. DSI Tempest or Novation SL mk3
<<<

>>>drumlayout4x4a
~~~~
Pad Layout 4x4 A

+--------+--------+--------+--------+   +-----+-----+-----+-----+
|   RS   |   CH   |   OH   |   CY   |   | A#5 | A#4 | B-4 | B-5 |
+--------+--------+--------+--------+   +-----+-----+-----+-----+
|   LT   |   MT   |   HT   |   CB   |   | F-4 | G-4 | A-4 | A-5 |
+--------+--------+--------+--------+ = +-----+-----+-----+-----+
| BDSHRT | SDSHRT | CPSHRT |   CL   |   | C-4 | D-4 | E-4 | G-5 |
+--------+--------+--------+--------+   +-----+-----+-----+-----+
| BDLONG | SDLONG | CPLONG |   MA   |   | C-5 | D-5 | E-5 | F-5 |
+--------+--------+--------+--------+   +-----+-----+-----+-----+
~~~~
<<<

>>>drumlayout4x4b
~~~~
Pad Layout 4x4 B

+--------+--------+--------+--------+   +-----+-----+-----+-----+
|  RS2   |NOISESRT|NOISELNG|  CY2   |   | A#7 | A#6 | B-6 | B-7 |
+--------+--------+--------+--------+   +-----+-----+-----+-----+
| C_OPEN | C_MUFF | C_SLAP |  CB2   |   | F-6 | G-6 | A-6 | A-7 |
+--------+--------+--------+--------+ = +-----+-----+-----+-----+
|  BT1   |  ZAP1  |  FX1   |  CL2   |   | C-6 | D-6 | E-6 | G-7 |
+--------+--------+--------+--------+   +-----+-----+-----+-----+
|  BT2   |  ZAP2  |  FX2   |  MA2   |   | C-7 | D-7 | E-7 | F-7 |
+--------+--------+--------+--------+   +-----+-----+-----+-----+
~~~~
] e.g. Akai MPK49
<<<

>>>drumlayout3x4a
~~~~
Pad Layout 3x4 A

+--------+--------+--------+   +-----+-----+-----+
|   MA   |   CB   |   CY   |   | F-5 | A-5 | B-5 |
+--------+--------+--------+   +-----+-----+-----+
|   RS   |   CH   |   OH   |   | A#5 | A#4 | B-4 |
+--------+--------+--------+ = +-----+-----+-----+
|   LT   |   MT   |   HT   |   | F-4 | G-4 | A-4 |
+--------+--------+--------+   +-----+-----+-----+
| BDSHRT | SDSHRT | CPSHRT |   | C-4 | D-4 | E-4 |
+--------+--------+--------+   +-----+-----+-----+
~~~~
<<<

>>>drumlayout3x4b
~~~~
Pad Layout 3x4 B

+--------+--------+--------+   +-----+-----+-----+
|  FX1   |  FX2   |   CY2  |   | E-6 | E-7 | B-7 |
+--------+--------+--------+   +-----+-----+-----+
|  ZAP1  |NOISE_S |NOISE_L |   | D-6 | A#6 | B-6 |
+--------+--------+--------+ = +-----+-----+-----+
|   LC   |   MC   |   HC   |   | F-6 | G-6 | A-6 |
+--------+--------+--------+   +-----+-----+-----+
| BDLONG | SDLONG | CPLONG |   | C-5 | D-5 | E-5 |
+--------+--------+--------+   +-----+-----+-----+
~~~~
<<<

>>>drumlayout4x3a
~~~~
Pad Layout 4x3 A

+--------+--------+--------+--------+   +-----+-----+-----+-----+
|   CH   |   OH   |   CY   |   CB   |   | A#4 | B-4 | B-5 | A-5 |
+--------+--------+--------+--------+   +-----+-----+-----+-----+
|   BT   |   LT   |   MT   |   HT   | = | C-6 | F-4 | G-4 | A-4 |
+--------+--------+--------+--------+   +-----+-----+-----+-----+
|   BD   |   SD   |   RS   |   CP   |   | C-4 | D-4 | A#5 | E-4 |
+--------+--------+--------+--------+   +-----+-----+-----+-----+
~~~~
] e.g. Elektron Analog Rytm

>>>drumlayout4x2a
~~~~
Pad Layout 4x2 A

+-------+-------+-------+-------+   +-----+-----+-----+-----+
|  BD1  |  SD1  |  CP1  |  CH   | = | C-4 | D-4 | E-4 | A#4 |
+-------+-------+-------+-------+   +-----+-----+-----+-----+
|  LT1  |  MT1  |  HT1  |  OH   |   | F-4 | G-4 | A-4 | B-4 |
+-------+-------+-------+-------+   +-----+-----+-----+-----+
~~~~
<<<

>>>drumlayout4x2b
~~~~
Pad Layout 4x2 B

+-------+-------+-------+-------+   +-----+-----+-----+-----+
|  BD2  |  SD2  |  CP2  |  RS   |   | C-5 | D-5 | E-5 | A#5 |
+-------+-------+-------+-------+   +-----+-----+-----+-----+
|  MA1  |  CL1  |  CB1  |  CY   | = | F-5 | G-5 | A-5 | B-5 |
+-------+-------+-------+-------+   +-----+-----+-----+-----+
~~~~
] e.g. Akai MPK mini
<<<

Available drum kit pad layouts:
 - 8x2
    - Bank A: $(buf!drumlayout8x2a)
    - Bank B: $(buf!drumlayout8x2b)
 - 4x4
    - Bank A: $(buf!drumlayout4x4a)
    - Bank B: $(buf!drumlayout4x4b)
 - 4x3
    - Bank A: $(buf!drumlayout4x3a)
 - 4x2
    - Bank A: $(buf!drumlayout4x2a)
    - Bank B: $(buf!drumlayout4x2b)

note: All layouts share the same note-to-sound mapping, e.g. `C-4` always maps to `BD1`

note: The drum pad layouts, including the default layout, are defined in `eureka_config.tks` (search for `drum pad`)

###### Drum kit pad indices, MIDI notes, short and long names
note: AR=Analog Rytm, MD=Machinedrum, TB=Tanzbaer

|: Idx |: NrAB |: Note |: NoteNr |: Name |: Alt.Name   |: Tempest Pad/Note  |: AR  |: MD   |: TB
|  0   | A1    | C-4   | 48      | BD1   | BDSHRT      | A1  / C-4          | C-0  | C-3   | C-4
|  8   | A2    | C-5   | 60      | BD2   | BDLONG      | A9  / C-5          | -    | -     | D-4
| 16   | B1    | C-6   | 72      | BD3   | BT1         | B1  / C-6          | E-0  | -     | -
| 24   | B2    | C-7   | 84      | BD4   | BT2         | B9  / C-7          | -    | -     | -
|      |       |       |         |       |             |                    |      |       |
|  1   | A3    | D-4   | 50      | SD1   | SDSHRT      | A2  / D-4          | C#0  | D-3   | E-4
|  9   | A4    | D-5   | 62      | SD2   | SDLONG      | A10 / D-5          | -    | -     | -
| 17   | B3    | D-6   | 74      | SD3   | ZAP1        | B2  / D-6          | -    | A-4*  | -
| 25   | B4    | D-7   | 86      | SD4   | ZAP2        | B10 / D-7          | -    | -     | -
|      |       |       |         |       |             |                    |      |       |
|  2   | A5    | E-4   | 52      | CP1   | CPSHRT      | A3  / E-4          | D#0  | A-3   | D-5
| 10   | A6    | E-5   | 64      | CP2   | CPLONG      | A11 / E-5          | -    | -     | -
| 18   | B5    | E-6   | 76      | CP3   | FX1         | B3  / E-6          | -    | -     | -
| 26   | B6    | E-7   | 88      | CP4   | FX2         | B11 / E-7          | -    | -     | -
|      |       |       |         |       |             |                    |      |       |
|  3   | A7    | F-4   | 53      | LT1   | LOWTOM      | A4  / F-4          | F-0  | G-3   | E-5
| 11   | A8    | F-5   | 65      | LT2   | MA1         | A12 / F-5          | -    | B-4*  | E-6
| 19   | B7    | F-6   | 77      | LT3   | C_OPEN      | B4  / F-6          | -    | -     | F-5
| 27   | B8    | F-7   | 89      | LT4   | MA2         | B12 / F-7          | -    | -     | -
|      |       |       |         |       |             |                    |      |       |
|  4   | A9    | G-4   | 55      | MT1   | MIDTOM      | A5  / G-4          | F#0  | F-3   | G-5
| 12   | A10   | G-5   | 67      | MT2   | CLAVES1     | A13 / G-5          | -    | C-5*  | C-5
| 20   | B9    | G-6   | 79      | MT3   | C_MUFF      | B5  / G-6          | -    | -     | A-5
| 28   | B10   | G-7   | 91      | MT4   | CLAVES2     | B13 / G-7          | -    | -     | -
|      |       |       |         |       |             |                    |      |       |
|  5   | A11   | A-4   | 57      | HT1   | HITOM       | A6  / A-4          | G-0  | E-3   | B-5
| 13   | A12   | A-5   | 69      | HT2   | CB1         | A14 / A-5          | B-0  | C-4   | D-6
| 21   | B11   | A-6   | 81      | HT3   | C_SLAP      | B6  / A-6          | -    |       | C-6
| 29   | B12   | A-7   | 93      | HT4   | CB2         | B14 / A-7          | -    |       | -
|      |       |       |         |       |             |                    |      |       |
|  6   | A13   | A#4   | 58      | CH1   | CLOSEDHAT   | A7  / A#4          | G#0  | D-4   | B-4
| 14   | A14   | A#5   | 70      | CH2   | RS1         | A15 / A#5          | D-0  | B-3   | F-4
| 22   | B13   | A#6   | 82      | CH3   | NOISESHRT   | B7  / A#6          | -    | D-5*  | -
| 30   | B14   | A#7   | 94      | CH4   | RS2         | B15 / A#7          | -    | -     | -
|      |       |       |         |       |             |                    |      |       |
|  7   | A15   | B-4   | 59      | OH1   | OPENHAT     | A8  / B-4          | A-0  | E-4   | A-4
| 15   | A16   | B-5   | 71      | OH2   | CY1         | A16 / B-5          | A#0  | F-4   | G-4
| 23   | B15   | B-6   | 83      | OH3   | NOISELONG   | B8  / B-6          | -    | -     | -
| 31   | B16   | B-7   | 95      | OH4   | CY2         | B16 / B-7          | -    | G-4   | -
].small_table

in linear pad index order:

|: Idx |: NrAB |: Note |: NoteNr |: Name |: Alt.Name |: Tempest Pad/Note  |: AR  |: MD   |: TB
|  0   | A1    | C-4   | 48      | BD1   | BDSHRT    | A1  / C-4          | C-0  | C-3   | C-4
|  1   | A3    | D-4   | 50      | SD1   | SDSHRT    | A2  / D-4          | C#0  | D-3   | E-4
|  2   | A5    | E-4   | 52      | CP1   | CPSHRT    | A3  / E-4          | D#0  | A-3   | D-5
|  3   | A7    | F-4   | 53      | LT1   | LOWTOM    | A4  / F-4          | F-0  | G-3   | E-5
|  4   | A9    | G-4   | 55      | MT1   | MIDTOM    | A5  / G-4          | F#0  | F-3   | G-5
|  5   | A11   | A-4   | 57      | HT1   | HITOM     | A6  / A-4          | G-0  | E-3   | B-5
|  6   | A13   | A#4   | 58      | CH1   | CLOSEDHAT | A7  / A#4          | G#0  | D-4   | B-4
|  7   | A15   | B-4   | 59      | OH1   | OPENHAT   | A8  / B-4          | A-0  | E-4   | A-4
|  8   | A2    | C-5   | 60      | BD2   | BDLONG    | A9  / C-5          | -    | -     | D-4
|  9   | A4    | D-5   | 62      | SD2   | SDLONG    | A10 / D-5          | -    | -     | -
| 10   | A6    | E-5   | 64      | CP2   | CPLONG    | A11 / E-5          | -    | -     | -
| 11   | A8    | F-5   | 65      | LT2   | MA1       | A12 / F-5          | -    | B-4*  | E-6
| 12   | A10   | G-5   | 67      | MT2   | CLAVES1   | A13 / G-5          | -    | C-5*  | C-5
| 13   | A12   | A-5   | 69      | HT2   | CB1       | A14 / A-5          | B-0  | C-4   | D-6
| 14   | A14   | A#5   | 70      | CH2   | RS1       | A15 / A#5          | D-0  | B-3   | F-4
| 15   | A16   | B-5   | 71      | OH2   | CY1       | A16 / B-5          | A#0  | F-4   | G-4
|      |       |       |         |       |           |                    |      |       |
| 16   | B1    | C-6   | 72      | BD3   | BT1       | B1  / C-6          | E-0  | -     | -
| 17   | B3    | D-6   | 74      | SD3   | ZAP1      | B2  / D-6          | -    | A-4*  | -
| 18   | B5    | E-6   | 76      | CP3   | FX1       | B3  / E-6          | -    | -     | -
| 19   | B7    | F-6   | 77      | LT3   | C_OPEN    | B4  / F-6          | -    | -     | F-5
| 20   | B9    | G-6   | 79      | MT3   | C_MUFF    | B5  / G-6          | -    | -     | A-5
| 21   | B11   | A-6   | 81      | HT3   | C_SLAP    | B6  / A-6          | -    |       | C-6
| 22   | B13   | A#6   | 82      | CH3   | NOISESHRT | B7  / A#6          | -    | D-5*  | -
| 23   | B15   | B-6   | 83      | OH3   | NOISELONG | B8  / B-6          | -    | -     | -
| 24   | B2    | C-7   | 84      | BD4   | BT2       | B9  / C-7          | -    | -     | -
| 25   | B4    | D-7   | 86      | SD4   | ZAP2      | B10 / D-7          | -    | -     | -
| 26   | B6    | E-7   | 88      | CP4   | FX2       | B11 / E-7          | -    | -     | -
| 27   | B8    | F-7   | 89      | LT4   | MA2       | B12 / F-7          | -    | -     | -
| 28   | B10   | G-7   | 91      | MT4   | CLAVES2   | B13 / G-7          | -    | -     | -
| 29   | B12   | A-7   | 93      | HT4   | CB2       | B14 / A-7          | -    |       | -
| 30   | B14   | A#7   | 94      | CH4   | RS2       | B15 / A#7          | -    | -     | -
| 31   | B16   | B-7   | 95      | OH4   | CY2       | B16 / B-7          | -    | G-4   | -
].small_table

#### Timeline
- arbitrary number of simultaneous timelines (one per track)
- up to 128 MIDI-program-changeable variations per timeline
   - e.g. for splitting larger timelines into sections which are then arranged in Synergy
   - or for switching audio timelines in a live performance
- fully editable clip samples
   - envelopes, effects, filters, layers, ..
- up to 8 lane sends per timeline track
- per-clip lane-send modulation
   - hold LALT while editing level / pan dial
- seek to timeline position via MIDI RPNs 88+89 (smpoffms)
   - e.g. sent by Synergy arranger node audio tracks
- track MIDI events are forwarded to timeline tracks
   - pitchbend
   - performance controllers (14 bit):
      - CC#1:33 mod wheel
      - CC#2:34 breath control
      - CC#4:36 foot control
      - CC#7:39 volume
      - CC#8:40 balance
      - CC#10:42 pan
      - CC#11:43 expression
      - CC#16:48 general purpose #1
      - CC#17:49 general purpose #2
      - CC#18:50 general purpose #3
      - CC#19:51 general purpose #4
      - CC#74:106 brightness
      - CC#80:112 general purpose #5
      - CC#81:113 general purpose #6
      - CC#82:114 general purpose #7
      - CC#83:115 general purpose #8
   - CC#121 reset all controllers
   - per-timeline-track MIDI modulation if track midi channel is set to `*`
      - forwarded to all timeline tracks otherwise
- notes:
   - create new clip+sample by pasting SampleView clipboard into timeline
      - alternatively, use `Create..=>Create timeline clip from selection` on `Audio` page, then create a timeline clip and select a sample for the clip (click on `-` in the clip panel or RMB-click the clip)
   - when copy'n'pasting clips, samples will be copied *by reference* (i.e. not duplicated)
      - to duplicate a sample, edit it by RMB-clicking a clip, then clone it on the sample page (this will also update the clip)
   - sync with Synergy replay: create an audio track in a Synergy 'Arranger' and set it to the same MIDI port as the timeline track in Eureka. It may be necessary to refresh the track info by clicking 'Query' in the audio sample form below the arranger timeline. The arranger will automatically switch to audio-timeline mode when it detects that the MIDI port is associated with an audio-timeline track
   - the timeline editor is mostly controlled by keyboard shortcuts (menus will be added in future versions). see above for documentation
      - most important keys:
         - 'c' sets loop area (to selected clip(s) or selection rectangle)
         - hold LCTRL while LMB-selecting a region to prioritize rectangular selection instead of drag-moving clips
         - q/w/r/t selects edit mode (select/draw/razor/mute)
         - 's' toggles snap-to-grid (lshift-s cycles grid resolution)
         - SPACE replays the loop area
         - lctrl-SPACE starts the replay from the current cursor position)
   - selecting a rectangular (clip) region in the timeline editor will also update the SampleView selection
   - arbitrary, global or per timeline track modulation can be added via Synergy sequencer nodes (or live controllers)
      - in addition, the Synergy Arranger also supports per-clip performance controller changes (mw/bc/fc/ex/gp1..4)
   - it is recommended to use multiple timeline tracks (e.g. one per instrument group), and break down complex arrangements into multiple timeline 'variations'(sections), launched by one or more Synergy Arrangers (in sync with realtime MIDI playback).
      - well, that, or simply print everything to audio and use the Synergy sequencer just for preparing stems / clips
         - and / or: use a timeline to sequence *one shots* (e.g. a drum pattern)

### Plugins
- plugins are instanced by drag'n'dropping them from the plugin browser

- plugin patches named `init` will be auto-loaded after plugin instantiation
   - `init\_delay` will be loaded when the plugin filter is set to "delay", `init\_reverb` when it is set to "reverb", and so on

- plugins instances are deleted by drag'n'dropping them back to the plugin browser

- the default plugin browser type can be configured in `eureka_config.tks` (`pgtrack_default_list_type`)

## Samples page

- each project may contain an arbitrary number of samples
   - a unique MIDI program number (1..128) can be assigned to each sample
   - alternatively, samples can be selected by their unique id (1...16384). see [Sampler MIDI events](#sampler).

- samples can be sorted into (up to 32) namespaces
   - namespaces are added, renamed, or deleted by hold-clicking a namespace combofield
      - renaming an empty slot will add a namespace
      - renaming a non-empty slot will rename the namespace
      - renaming a non-empty slot to "" will delete the namespace

- the way the Samples page is entered matters:
   - when the Samples page is entered by clicking the `Samples` page menu button (`F1` or `F5` key):
      - the sample will be played through a 'scratch' sample player that is routed to the main output (without effects)
      - the sample list will be filtered by the last selected sample namespace
   - when the Samples page is entered by clicking on a track's `ModSample` sampler instance:
      - the sample will be played through the track, its effects, and audio routings
      - the sample list will be filtered by the initially selected sample namespace
      - selecting a sample will change the sampler's sample selection
         - changing the sample's namespace will also update the track sampler's preferred namespace
      - keys `4` and `5` select the previous / next sampler instance of the parent track
         - alternatively, click the `<` and `>` buttons
            - clicking the track name button on the right shows the list of currently selected track samples
   - when the Samples page is entered by clicking on a drum pad (when the track is set to [Drum](#drum-pad-sampler-drum) mode):
       - the sample will be played through the track, its effects, and audio routings
       - the sample list will be filtered by the initially selected sample namespace
       - keys `4` and `5` select the previous / next (used) drum pad
          - alternatively, click the `<` and `>` buttons
             - clicking the drum pad name button on the right shows the list of currently selected drum pad samples

- keys `2` and `3` select the previous / next zone of the current sample. key `1` selects all zones.

- clicking on a sample once will select it, twice will select the first tabs (`Patch` and `Loops`), thrice will execute the default action for the first selected zone
   - the default action (no op, play zone, edit synth patch, edit synth patch machine) is changed by clicking the middle label button right below the zones list (`-`, `P`, `S`, `M`)

- the edit precision of an initial performance controller can be toggled by clicking the corresponding label (mw, bc, fc, ex, gp1..4)

### Zones

- each sample may contain an arbitrary number of sample zones (*layers*)

- all sample zones share the same waveform
   - but not necessarily the same waveform area
   - each sample zone maintains the last seen sample editor view (scroll position + zoom)

- each zone may define an arbitrary number of loops
   - loops may not necessarily be actual loops but can also be used as oneshot sample regions
   - selecting a loop (in the loops table) sets the initial note-on loop
      - the `R:` widget sets the note-off loop
   - the `Rep:` setting defines the number of loop iterations before advancing to the next loop
      - setting this to 0 causes the loop to be played indefinitely
      - when the `ms:` option is checked, this value is interpreted as number of milliseconds instead (*timed loops*)
   - the `Jump To Loop` mod matrix targets can be used to change the currently playing loop
   - pressing `lctrl-h` (when the sample editor is focused) sets the currently edited loop to the selected area (`Copy selection to highlight` item in the sample editor context menu)

- other zone features include..
   - 4 global or per-voice LFOs with fade in, start-phase and oneshot options
   - 4 global or per-voice, loopable mod sequencers with oneshot option and up to 16 MIDI-switchable patterns
   - up to 6 voice calibration tables, e.g. for adding subtle variations or stereo spread
      - table 2 (pan) can also be applied to the partial stereo spread of additive wavetables
      - all tables are available as mod matrix sources
   - time or frequency domain (*additive*) wavetable modes
   - monophonic and polyphonic glide modes

- when multiple zones are selected, parameter edits will update all selected zones (*multi-edit*)


### Live Recording

- the sample waveform can be recorded from incoming mono or stereo audio when the sample is selected in a track's `ModSample` sampler instance

- recording settings are located in the `Rec` tab and include..
   - input selection
      - left and / or right channel
      - side chain left / right channel
   - latency compensation
      - skip *n* sample frames at the start of a new recording
   - (default) target area
      - index into `Loops` table
   - recording mode
      - MIDI or manual trigger (`Wait for trigger*` modes)
      - threshold-level trigger (`Wait for signal` mode)
      - continuous (`Continuous`, `Continuous Ring` modes)
   - monitor mode
      - listen to incoming audio (`Monitor` checkbox) or a mix between the audio input and sampler output (`Sum` checkbox)
   - overdub mode
      - add recorded audio to existing waveform instead of replacing it (`Init` checkbox)
   - double buffer mode
      - when enabled (not set to `-`), splits the waveform into two equal halves
      - the sides (front-/backbuffer) are swapped..
          - in `-` mode: never
          - in `End` mode: when the last recording is finished
          - in `Voice (Queue)` mode: when the next voice or the next recording is started
          - in `Rec Start` mode: when the next recording is started
          - in `Param` mode: when receiving a `LiveRec DblBuf Index` parameter change message
   - copy mode
      - copies recordings to voice-internal buffers so the next recordings won't interfer with the playback of previous recordings
   - live-looped oscillator cycle detection mode
      - can be used to "polyphonize" monophonic sound sources, e.g. a modular system
      - the sound source must be a constant-pitch tone
      - the loop cycle is updated around 300 times per second
         - fast enough for manual modulation, LFOs, and (slow) envelopes

- recording can be controlled via the `LiveRec Start`, `LiveRec Stop`, `LiveRec Continue` modmatrix destinations
   - `LiveRec Start` starts the recording when the value exceeds 0.5 (i.e. 64 when the source is a MIDI CC)
   - `LiveRec Stop` stops the recording when the value exceeds 0.5 (i.e. 64 when the source is a MIDI CC)
   - `LiveRec Continue` continues the recording when the value exceeds 0.5 (i.e. 64 when the source is a MIDI CC)

- more fine-grained control is possible by sending parameter changes directly to the sampler instance
   - this can be used to change the recording region (target loop), explicitely select the front-/backbuffer area of a double buffered waveform, and start / stop / continue the recording for specific zones
      - parameter names: `LiveRec Start Mask`, `LiveRec Continue Mask`, `LiveRec Stop Mask`, `LiveRec Target Loop Shift`, `LiveRec Start Index`, `LiveRec Continue Index`, `LiveRec Stop Index`, `LiveRec DblBuf Index`
      - see `Tracker` node `Remote` instrument parameter type

- the `Freeze` button stops live recording and copies the last started voice recording buffer to the sample waveform

- the `Capture Dialog` is used for building wavetables (in the sample clipboard)
   - it becomes available when both live recording and `Copy` mode are enabled
   - the currently detected oscillator cycle will be resampled to the given cycle len and added to the sample clipboard

>>>aclipnote
The Synergy Arranger audio-track feature and the Tracker node "audio patterns" use the sample live-recording mode to record / overdub audio clips.
By convention, the names of audio clip samples and namespaces start with `a_` (e.g. `"a_prophet12_1"`)

tip: As of June 1st, 2025, the new audio timeline feature supercedes this
<<<
note: $(buf!aclipnote)


### Voice Buses

- voice buses are used to route the output of a sample zone (*layer*) to other zones.
  voice buses are used in conjunction with voice plugins.

- each sample supports up to 32 voice buses
   - see `Zone` tab `Bus:` combobox

- zones that write to a voice bus must occur before zones that read from it

### Voice Plugins

- on the `FX` tab, up to four plugins may be configured for each sample zone
   - voice plugins use the lightweight, free open source [STFX](https://github.com/bsp2/stfx/) plugin format

- voice plugins are instantiated *per voice* (hence the name)
   - effects like distortion, waveshaping or ring modulation sound quite different when applied to each voice separately instead of the polyphonic mix (like a VST plugin)

- the plugin processing order / routing can be configured using 24 algorithms plus the filter position
   - serial, parallel, ..
   - not selecting a particular algorithm results in the plugins being processed in linear order (1..4)

- the parameter configurations and routing can be saved as presets
   - `FX` tab `P` button opens preset browser

- voice plugins can access any of the 32 voice buses
   - by convention, the names of plugins which support this start with `x`, e.g. `"x je ring modulator"`

- Eureka currently ships with 154 open source plugins
   - many have been written from scratch
   - a few have been ported from VCV Rack
   - some have been auto-generated via [Cycle](cycle.html#plugins)

### Procedural Samples

- procedural sample mode is activated by selecting `Cycle` in the `Synth` combobox (at the top of the window)
   - samples can be synthesized in 8 bit integer mode (with internal 11 bit fixed point precision + 4 bit headroom), or with 32 bit floating point precision
      - the integer mode can be interesting for "grungy", "LoFi" sounds
      - in ("HiFi") float mode, sections of a *machine* script may revert to integer mode via the [int](cycle.html#int) module
   - procedural sample generation "C" code can be exported for use in games or demos
      - integer mode supports classic 68k based CPUs (e.g. Amiga 500, Atari ST, ..) as well as modern CPUs
      - float mode supports modern CPUs (basically everything since the mid / late 90ies)

- unlike static samples, no waveforms (`.wav` files) will be saved for procedural samples. Merely the sample size and instructions for how to synthesize the sample will be stored in the `.esa` (sample patch) files (in the library and / or the project directory)

- each sample stores up to 32 patches which can target any of the sample's zones
   - the render area for a procedural sample (zone) is determined by the zone's last loop area
   - each patch is defined by a *machine* (the synth script), up to 16 cubic bezier curves (*LUTs*), and up to 32 parameters (*args*)
      - *machines* can be saved as presets
      - bezier curves are converted to lookup tables with user-defined precision
   - multiple patches may share the same *machine* (`Share:` combobox)
      - common *args* may be inherited from the shared *machine*
         - on a per parameter basis, see checkbox next to each each *arg*
   - the sample patches will be processed in the order they are defined (1..32)
      - it is recommended to order the zones accordingly
         - the zones table context menu has a utility function for aligning the patch order to the zone order (`SampleSynth: Reorder patches by zones`)

- procedural samples can either be entirely synthesized, or be created by processing other (static) sample zones
   - reading from other samples can be used to create precalculated variations of static samples
      - see [zlp](cycle.html#zlp), [zon](cycle.html#zon), and [zsq](cycle.html#zsq) modules
   - any STFX voice plugin can be used in a procedural patch
      - see [vpl](cycle.html#vpl) module
      - parameters for the currently selected plugin can be copied to the clipboard (in single or multi-line `Cycle` format) via the `FX` tab context menu (or by pressing `lctrl-c` / `lctrl-lshift-c` when the `FX` tab has the keyboard focus)
      - new (polyphonic and realtime) STFX voice plugins can be generated from Cycle patches

- see example patches, plugins, and the [Cycle](cycle.html) manual for details

tip: empty sample waveforms can be created via the `resize_waveform` and `resize_16th` statusbar page commands


# Projects

- Eureka projects will be saved when saving projects in `Synergy`
   - hold-clicking `Save` shows an emergency save dialog

- to load a project, load a `Synergy` project, then click `Load` on the Eureka start page
   - hold-clicking `Load` shows a load dialog which you will most likely never need to use



$(buf!footnote)
$(buf!w3validator)

Document created in $(var:gen_ms) on $(var:localdatetime)
].create
