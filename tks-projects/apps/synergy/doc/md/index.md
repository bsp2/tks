% Synergy MIDI Tracker
% bsp
% 05-Jun-2025
>>>
03Nov2019, 04Nov2019, 05Nov2019, 07Nov2019, 08Nov2019, 09Nov2019, 17Nov2019, 03Apr2020, 04Apr2020, 07May2020, 15Oct2020, 14May2021, 15May2021, 02Jun2021, 03Jun2021, 14Jun2021, 20Jun2021, 25Jul2021, 28Jul2021, 21Aug2021, 27Aug2021, 13Sep2021, 05Oct2021, 15Oct2021, 24Oct2021, 26Oct2021, 28Oct2021, 31Oct2021, 01Nov2021, 13Nov2021, 21Nov2021, 12Dec2021, 19Dec2021, 20Dec2021, 23Dec2021, 31Dec2021, 09Jan2022, 16Jan2022, 13Feb2022, 23Feb2022, 27Feb2022, 18Mar2022, 20Mar2022, 01Apr2022, 03Apr2022, 08Apr2022, 21Apr2022, 23Apr2022, 24Apr2022, 24Jun2022, 31Oct2022, 04Nov2022, 19Nov2022, 25Nov2022, 16Dec2022, 23Dec2022, 01Feb2023, 05Feb2023, 12Feb2023, 15Feb2023, 18Feb2023, 20Feb2023, 26Feb2023, 31Mar2023, 01Apr2023, 03Aug2023, 04Aug2023, 05Aug2023, 17Aug2023, 18Aug2023, 14Dec2023, 15Dec2023, 15Dec2023, 23Nov2024, 28Nov2024, 30Nov2024, 21Dec2024, 23Dec2024, 27May2025, 29May2025, 31May2025, 04Jun2025, 05Jun2025
<<<
$(var:header)


# About

`Synergy` is a free and open source (semi-)modular MIDI sequencer, patch editor and event processor / router, available for macOS, Windows, and Linux.

It is designed as a **performance sequencer** that supports extensive realtime sequence modulations and live arrangements of patterns and clips, all of which can be recorded back to new sequences for further editing and / or (live-)recording passes.

`Eureka` is its bundled DAW companion app that serves as a **sampler** (with support for per-voice plugins (FX + synthesis), (additive) wavetables (incl. resynthesis) and live-recording / monophonic realtime polyphonization), a **sample editor** (with support for [SoX](https://sourceforge.net/projects/sox/), [Rubberband](https://rubberbandaudio.com/) and [MPG123](https://www.mpg123.de/)), a multitrack **mixer** and **recorder**, a polyphonic **modular synthesizer** (with support for native code generation), and a **plugin host** (CLAP, VST2, STFX). It connects with `Synergy` either via high-speed shared memory, or any other MIDI transports.

Sequencer Features:
- a **Tracker** sequencer with microtiming, zoomable patterns, customizable columns, (N)RPN and 14bit CCs, graphical envelopes, undo, ..
- a drumcomputer-like **step sequencer** with support for microtiming, swing, polyrhythmic and polymetric tracks, per-track speeds, ..
- a DAW-style **timeline editor** ("arranger") with support for clip selection / state recording, nested timelines, undo, ..
- **freeform** monitor and **piano roll** sequencer nodes for recording and playing back arbitrary MIDI data, including SysEx dumps
- a **Modular** sequencer node (55 modules)
- a ModMatrix **event processor** with support for polyphonic events and cubic bezier curve look up tables
- a **script** event processor (with support for custom C++ plugins)
- extensive **MIDI remote control** for all nodes and the UI itself
- lots of **keyboard** shortcuts for fast workflows (with builtin help / reminder tooltips)
- support for multi-channel MIDI and **MPE** (MIDI polyphonic expression)
- groove patterns
- multi-node and multi-pattern-sequence recording with configurable precount and undo
- built-in virtual MIDI ports (for exchanging events between nodes)
- **SysEx** patch editors and librarians for the Oberheim Matrix 1000, Roland Alpha Juno / MKS50, DSI Prophet 08 (rev1), DSI Prophet 12, Black Corporation Deckard's Dream, Korg Radias, Novation Peak (FW 2.0), Yamaha FS1R, Elektron Analog Rytm (FW1.70), Moog Sub 37, Yamaha Montage, Sequential Pro 3, Oberheim OB-X8
- import of **Analog Rytm** drum computer patterns (tracker + step sequencer)
- Analog Rytm sample chain creator and sample / kit uploader
- **preset** management for nodes, pipes, curves
- MIDI **synth profiles** (for mapping notes and controller numbers to human-readable names)
- standard MIDI file import and export (**SMF** formats 0 and 1)
- bidirectional controller support for
   - **Linnstrument** HW controller (clip launching, virtual sliders and step sequencing)
   - **PreSonus FaderPort** HW controllers (recallable faders, manual+automatic labeling, ..)
   - **DJTechTools MIDIFighter Twister** HW controller (recallable rotaries / controller scenes, step editing, ..)
   - **Softube Console 1 Mk 2** HW controller (recallable rotaries / controller scenes, ..)
- *Light*, *Dark*, and *Mono* **themes**, and customizable color grading


# Concept

Projects consist of up to 16 **pipes** which may contain an arbitrary number of **nodes** (with 13 node types to choose from).
Nodes modify incoming MIDI events or generate new events.

The output of the first pipe (*"master"*) is sent to all nodes while the output of all other pipes is sent to the MIDI ports.

Each node has two **automation ports** which can be used to remote control (*legato* or *queued*) pattern changes, mute states, play positions, playback speeds, note transpositions, and other (node-specific) features via external HW controllers and / or other nodes.


# Screenshots
$(inc:screenshots.md)
].pimages


# Documentation
The documentation is still a work in progress. See [here](adoc/index.html) for an older version which might be slightly out of date but is still valid for the most part.

See [Getting started](GETTING_STARTED.html) for a brief introduction / setup guide.


## Keyboard Shortcuts

[Keyboard Shortcut Tables](keyboard.html)

## PreSonus FaderPort support

[PreSonus FaderPort support](faderport.html)

## Yamaha Montage synth editor

[Yamaha Montage synth editor](montage.html)

## Eureka DAW

Documentation for the (optional) plugin host, sampler, and multi-track recorder app `Eureka` is available [here](eureka/readme.html).

Documentation for the `Cycle` modular software synth / sample generator is available [here](eureka/cycle.html).

>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Technical notes for the Amiga 500 "Tiny Synergy Replay" are available [here](eureka/tsr_amiga.html).
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


# Changelog
A detailed changelog is available [here](CHANGELOG.txt).


# Downloads

Hotfix for timeline-debug-output-enabled and track-page-merge-sample-crash issues (all platforms, just replace the existing file after installing one of 01Jun2025 releases): [eureka_timeline_debug_output_track_merge_sample_hotfix-05Jun2025.7z](files/eureka_timeline_debug_output_track_merge_sample_hotfix-05Jun2025.7z)

## macOS

  |cols=140,40
  |:File                                                                                                    |:Description
  | [synergy-miditracker-01Jun2025-macos_arm64.dmg](files/synergy-miditracker-01Jun2025-macos_arm64.dmg)    | Latest (stable) Release
  | [synergy-miditracker-01Jun2025-macos_x86_64.dmg](files/synergy-miditracker-01Jun2025-macos_x86_64.dmg)  | Latest (stable) Release
  ].table_noborder

>>>mac_quarantine
- after copying the app to the `/Applications` folder, it must be openend via context menu "open" instead of double-clicking it (first run only).
- alternatively, the download-quarantine attribute can be removed from the app using the `terminal` app:
~~~~
% xattr -r -d com.apple.quarantine /Applications/Synergy\_arm64.app
~~~~
<<<
note: $(buf!mac_quarantine) 

note: tested on arm64 macOS 13.5, 14.7, and x86_64 10.12+


## Windows

  |cols=140,40
  |:File                                                                                       |:Description
  | [synergy-miditracker-01Jun2025-win64.7z](files/synergy-miditracker-01Jun2025-win64.7z)     | Latest (stable) Release
  ].table_noborder

important: when upgrading to a new release, backup your `synergy_config_local.tks` and then replace the default local config

note: tested on x86\_64 Windows 10


## Linux

## Source Code

  |cols=70,40
  |:File                                                                                 |:Description
  | [synergy-miditracker-05Jun2025-src.7z](files/synergy-miditracker-05Jun2025-src.7z)  | Latest release (macOS / Windows  / Linux)
  ].table_noborder


# Audio Clips
[A collection of audio demos](audio_clips/clips.html)



# Contact
Feel free to drop me a [mail](bs@tkscript.de). You can also find me on the [Gearspace](http://www.gearspace.com/board/electronic-music-instruments-electronic-music-production/) and [KVR Audio](https://www.kvraudio.com/) online discussion forums.


# Table of Contents
$(buf!toc)



$(buf!footnote)
$(buf!w3validator)

Document created in $(var:gen_ms) on $(var:localdatetime)
].create
