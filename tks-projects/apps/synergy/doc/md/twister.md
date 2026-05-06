% Synergy MIDI Tracker - MIDI Fighter Twister
% bsp
% 03-May-2026
>>>
03May2026
<<<
$(var:header)


# Table of Contents
$(buf!toc)

# DJ Tech Tools MIDI Fighter Twister support

See [MIDI Fighter home page](https://www.midifighter.com/#Twister).

## Features

- 4 banks * (16 clickable rotaries)
   - Bank A is split into another 4 virtual banks `A:A`..`A:D`
- edit up to 64 bi-directional controllers
   - `Ctl 1` .. `Ctl 64` (RPN#100 .. RPN#163)
       - context dependent
          - Tracker node: edit (non-empty) steps of current envelope column
          - Modular: edit current module parameters
          - Eureka: used as default modmatrix sources
             - e.g. when mapping VST / CLAP parameters
                - `TAB` or `F12` in plugin UI maps last touched parameter
   - Eureka integration
      - Select track (RPN#299)
      - Edit lane fading (RPN#86)
      - Smoothed parameter set 1 + 2 selection (RPN#75 and RPN#76)
      - Edit output 1..4 levels of current track (RPN#300 .. RPN#303)
   - MIDI continuous controllers (current context MIDI port)
      - Modwheel (CC#1)
      - Breathcontrol (CC#2)
      - Footcontrol (CC#4)
      - Expression pedal (CC#11)

## Setup

- make sure that Twister support is enabled in `synergy_config.tks` (enabled by default)
   - `static boolean b_mf_twister = true;`
- enable MIDI Figher Twister MIDI input and output devices (`Devs` page)
   - set input/output aliases to `twister`
   - select `MIDI Fighter Twister Encoders` profile for MIDI channel 1
   - select `MIDI Fighter Twister Buttons` profile for MIDI channel 2
   - don't forget to save the configuration (may need to restart app)
- on `Scene` page, load `twister mixed` scene preset into an empty scene
   - `Tools` => `Load Scene`
   - make sure the scene is active (selected)
- on `MIDIMap(g)` page
   - assign the bank A row 2 buttons to `MFTWISTER_VBANK_A` .. `MFTWISTER_VBANK_D`
      - this enables access to `Ctl 17` .. `Ctl 64` in virtual banks `A:A` .. `A:D`
   - assign bank B row 1 rotary 1 (top left) to `AUDIO_TRACK_SELECT`
      - optionally assign (unused) rotary buttons to `AUDIO_TRACK_SELECT_1` .. `AUDIO_TRACK_SELECT_32`

note: The rotaries (and most of the rotary buttons) in banks C and D are not mapped by default. Feel free to assign them as you like. Don't overwrite the bank select buttons, though.

caution: This is a programmable controller. The mapping described here assumes the default configuration (see `midi_synth_profiles/midifighter_twister_enc.msp` (rotaries) and `midi_synth_profiles/midifighter_twister_bt.msp` (buttons) MIDI synth profiles).

## HW controls

note: first line: rotary  second line: button/rotary click

### Bank A

#### Virtual bank A:A
~~~~
   [Ctl 1 ]  [Ctl 2 ]  [Ctl 3 ]  [Ctl 4 ]
   [Bank A]  [Bank B]  [Bank C]  [Bank D]

   [Ctl 5 ]  [Ctl 6 ]  [Ctl 7 ]  [Ctl 8 ]
   [VBankA]  [VBankB]  [VBankC]  [VBankD]

   [Ctl 9 ]  [Ctl 10]  [Ctl 11]  [Ctl 12]
   [-     ]  [-     ]  [-     ]  [-     ]

   [Ctl 13]  [Ctl 14]  [Ctl 15]  [Ctl 16]
   [-     ]  [-     ]  [-     ]  [-     ]
~~~~
] Bank A / Virtual bank A:A

#### Virtual Bank A:B
~~~~
   [Ctl 17]  [Ctl 18]  [Ctl 19]  [Ctl 20]
   [Bank A]  [Bank B]  [Bank C]  [Bank D]

   [Ctl 21]  [Ctl 22]  [Ctl 23]  [Ctl 24]
   [VBankA]  [VBankB]  [VBankC]  [VBankD]

   [Ctl 25]  [Ctl 26]  [Ctl 27]  [Ctl 28]
   [-     ]  [-     ]  [-     ]  [-     ]

   [Ctl 29]  [Ctl 30]  [Ctl 31]  [Ctl 32]
   [-     ]  [-     ]  [-     ]  [-     ]
~~~~
] Virtual Bank A:B

#### Virtual Bank A:C
~~~~
   [Ctl 33]  [Ctl 34]  [Ctl 35]  [Ctl 36]
   [Bank A]  [Bank B]  [Bank C]  [Bank D]

   [Ctl 37]  [Ctl 38]  [Ctl 39]  [Ctl 40]
   [VBankA]  [VBankB]  [VBankC]  [VBankD]

   [Ctl 41]  [Ctl 42]  [Ctl 43]  [Ctl 44]
   [-     ]  [-     ]  [-     ]  [-     ]

   [Ctl 45]  [Ctl 46]  [Ctl 47]  [Ctl 48]
   [-     ]  [-     ]  [-     ]  [-     ]
~~~~
] Virtual Bank A:C

#### Virtual Bank A:D
~~~~
   [Ctl 49]  [Ctl 50]  [Ctl 51]  [Ctl 52]
   [Bank A]  [Bank B]  [Bank C]  [Bank D]

   [Ctl 53]  [Ctl 54]  [Ctl 55]  [Ctl 56]
   [VBankA]  [VBankB]  [VBankC]  [VBankD]

   [Ctl 57]  [Ctl 58]  [Ctl 59]  [Ctl 60]
   [-     ]  [-     ]  [-     ]  [-     ]

   [Ctl 61]  [Ctl 62]  [Ctl 63]  [Ctl 64]
   [-     ]  [-     ]  [-     ]  [-     ]
~~~~
] Virtual Bank A:D

### Bank B

note: bi-directional (Eureka) DAW control

~~~~
   [TrkSel]  [LaneFd]  [PSet 1]  [PSet 2]
   [Bank A]  [Bank B]  [Bank C]  [Bank D]

   [Out1Lv]  [Out2Lv]  [Out3Lv]  [Out4Lv]
   [-     ]  [-     ]  [-     ]  [-     ]

   [CC:MW ]  [CC:BC ]  [CC:FC ]  [CC:Ex ]
   [-     ]  [-     ]  [-     ]  [-     ]

   [Ctl 1 ]  [Ctl 2 ]  [Ctl 3 ]  [Ctl 4 ]
   [-     ]  [-     ]  [-     ]  [-     ]
~~~~
] Bank B

### Bank C

note: user defined

~~~~
   [-     ]  [-     ]  [-     ]  [-     ]
   [Bank A]  [Bank B]  [Bank C]  [Bank D]

   [-     ]  [-     ]  [-     ]  [-     ]
   [-     ]  [-     ]  [-     ]  [-     ]

   [-     ]  [-     ]  [-     ]  [-     ]
   [-     ]  [-     ]  [-     ]  [-     ]

   [-     ]  [-     ]  [-     ]  [-     ]
   [-     ]  [-     ]  [-     ]  [-     ]
~~~~
] Bank C

### Bank D

note: user defined

~~~~
   [-     ]  [-     ]  [-     ]  [-     ]
   [Bank A]  [Bank B]  [Bank C]  [Bank D]

   [-     ]  [-     ]  [-     ]  [-     ]
   [-     ]  [-     ]  [-     ]  [-     ]

   [-     ]  [-     ]  [-     ]  [-     ]
   [-     ]  [-     ]  [-     ]  [-     ]

   [-     ]  [-     ]  [-     ]  [-     ]
   [-     ]  [-     ]  [-     ]  [-     ]
~~~~
] Bank D

note: left and right side center buttons can also be used to select banks A..D
