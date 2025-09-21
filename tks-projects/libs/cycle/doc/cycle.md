% Cycle Sample Generator / Soft Synth
% bsp
% 12-Jun-2025
>>>
11Oct2020, 13Oct2020, 14Oct2020, 31Jul2021, 12Aug2021, 24Aug2021, 16Jan2022,
06Jan2023, 07Jan2023, 28Mar2023, 07Apr2023, 19Apr2023, 04May2023, 05May2023,
06May2023, 07May2023, 08May2023, 09May2023, 12May2023, 16May2023, 27May2023,
28May2023, 29May2023, 31May2023, 01Jun2023, 05Aug2023, 19Aug2023, 23Nov2023,
24Nov2023, 30Nov2023, 15Dec2023, 08Jan2024, 14Jan2024, 15Jan2024, 17Jan2024,
18Jan2024, 19Jan2024, 20Jan2024, 21Jan2024, 22Jan2024, 27Jan2024, 28Jan2024,
31Jan2024, 02Feb2024, 03Feb2024, 04Feb2024, 06Feb2024, 07Feb2024, 08Feb2024,
09Feb2024, 10Feb2024, 14Feb2024, 15Feb2024, 16Feb2024, 17Feb2024, 24Feb2024,
25Feb2024, 19Apr2024, 21Apr2024, 22Apr2024, 23Apr2024, 24Apr2024, 09Aug2024,
10Aug2024, 11Aug2024, 13Aug2024, 15Sep2024, 08Oct2024, 08Nov2024, 09Nov2024,
10Nov2024, 30Nov2024, 04Dec2024, 06Dec2024, 13Dec2024, 15Dec2024, 17Dec2024,
18Dec2024, 19Dec2024, 20Dec2024, 21Dec2024, 22Dec2024, 23Dec2024, 08Jan2025,
09Jan2025, 13Jan2025, 14Jan2025, 15Jan2025, 29Jan2025, 29May2025, 31May2025,
01Jun2025, 12Jun2025
<<<
$(var:header)


# Table of Contents
$(buf!toc)


# Preface

`Cycle` is a modular soft-synth which emits `TKS` or `C` sample generation code, or [STFX](https://github.com/bsp2/stfx/) voice plugins.

It operates either in *hi-fi* (32bit float), or *lo-fi* (8bit signed integer) mode:

   - *hi-fi* uses 32bit floating point calculations while *lo-fi* uses 16bit fixed point calculations with signed 8bit sample output

   - in *lo-fi* mode, `Cycle` can be used to calculate sample data for Amiga MOD or TSR (Tiny Synergy Replay) files.
   - lo-fi sub-trees can be embedded in hi-fi patches (see [#int][]).

While the processing blocks (*modules*) in `Cycle` are rather simple, their combination can yield quite complex sounds, even with (by today's standards, slow) CPUs like the Motorola M68000.

In STFX plugin-mode, `Cycle` generates native-code `C` voice plugins via the GCC / Clang compiler.

STFX plugins and float sample generation code can also be used in the standalone Synergy replay.



# Global Keywords

Global keywords may appear at the beginning of a Cycle script.

>>>note_os
note: 8 is usually a good factor when targeting ~16kHz samples (=128kHz internal rate)
<<<

>>>caution_os
caution: Oversampling is ignored when exporting to lo-fi 8bit "C" sample generator source
<<<

>>>note_wtfreq
note: the current cycle freq is available via `$wt_freq` variable during synthesis
<<<

| Keyword            :|: Description
| arg                :| Declare argument (will be editable in UI) (shortcut form: `a`)
| arg_values         :| Argument preset values (shortcut form: `av`)
| var                :| Declare variable (shortcut form: `v`)
| oversample_factor  :| Oversample factor (0.03125..32, def=1) (also supports fractional values, e.g. "1/8")
|+oversample          |+ $(buf!caution_os)
|                    :|+ $(buf!note_os)
| oversample_quality :| Oversample quality (0..10, def=4)
|                     |+ $(buf!caution_os)
| dither              | `y`, `n`, or mode (0=off, 2=dithering, 3=distortion shaping, ..8=adaptive noise shaping)
| rate               :| Override zone sample-rate
| freq               :| Output frequency. Default is 261.63 (middle C, MIDI note 60 / C-5).
|                     |+ alternatively: MIDI note C-1..G-A (followed by finetune, e.g. `G-4 -0.2`)
|                     |+ alternatively: float ratio, e.g. 16574/64
| oct                :| Octave shift (-4..4)
| dur                :| Override sample duration (frames or, when followed by "ms", milliseconds)
| wt_w               :| Override wavetable width  (1..16 in 2D mode, 1..128 in 1D mode)
| wt_h               :| Override wavetable height (1..16 in 2D mode, 1 in 1D mode)
| wt_cyclelen        :| Override wavetable cycle length (number of frames. 1..1048576)
| wt_freqs           :| Per-cycle note frequencies/rates (multisampled keymaps)
|                     |+ e.g. `wt_freqs c-3 g-3 c-4 "g-4+0.11" c-5 g-5 c-6 g-6 c-7 g-7`
|                     |+ $(buf!note_wtfreq)
| skip               :| Skip output of first "n" sample frames or, when followed by "ms", milliseconds
| xfade              :| Crossfade end of loop with samples before loop start. Sample frames or loop-len percentage (when followed by \%)
| mirror             :| Calculate first half of waveform and mirror to second half (single cycle waveforms and wavetables)
| curve              :| Declare curve lookup-table. Must be followed by curve index (0..15) and curve id (e.g. "amp") (shortcut form: `c`)
| lut                :| Declare procedural lookup-table. Must be followed by LUT id, optional mode (voice/shared), optional table size (default=256), and optional "inv" (inverse) flag
| zone               :| Declare external zone reference. Must be followed by ref idx (0..) and zone id (e.g. "myzone", or "myothersmp.myzone") (shortcut form: `z`)
| macro              :| Declare macro parameter set(s). See [#macros][].
| inc                 | include global `.cy` file (the suffix is auto-appended) OR include patch-local buffer when include name starts with `.`
].cmdline Global Keywords

>>>plugin_name
note: the plugin name (and dll / so / dylib) will be prefixed with `cycle_`
<<<

>>>plugin_cat
|noheader
| |:unknown     |:limiter  |:pan     |:transient
| | ringmod     | enhance  | pitch   | convolver
| | waveshaper  | dac      | chorus  | mixer
| | filter      | lofi     | flanger | comparator
| | eq          | gate     | delay   | utility
| | compressor  | amp      | reverb  | osc
].table_noborder_small
<<<

## Plugin keywords
Keywords used in plugin-generation mode:
| Keyword            :|: Description
| id                 :| Plugin id (mandatory)
| name               :| Plugin name (optional. def=\<id\>)
|+                    |+$(buf!plugin_name)
| author             :| Plugin author (optional)
| cat                :|
|+category            |+Plugin category (optional. def="osc")
|+                    |+$(buf!plugin_cat)
| param               | Declare plugin parameter (shortcut form: `p`)
| param_values        | Declare plugin preset values (shortcut form: `pv`)
| mod                 | Declare plugin modulation target (shortcut form: `m`)
| modparam            | Declare plugin parameter and modulation target (param will be prefixed with `p\_`, modulation with `m\_`) (shortcut form: `mp`)
| xinc                | import native code (C / C++) extension module (e.g. `xinc xamp`). The file suffix `.h` is auto-appended.
].cmdline Plugin Keywords



# Values and sample output

The same value type is used for module parameters, args, variables, and intermediate sample data.

In *hi-fi* mode, values are represented by 32 bit floating point numbers. The final sample output is also a `float`.

In *lo-fi* mode, values are represented by signed twos-complement, 16 bit fixed point numbers (4:11), i.e. the value range is -15..15.
The final sample output is a signed 8 bit sample.


# Output lanes

Lanes divide the script into output sections:

~~~
<out:
  sin
~~~ lang=cycle
] hello, world.

~~~
<layer_1:
  sin

<layer_2:
  sin f=1.5
~~~ lang=cycle
] hello, layered world.

Each lane consists of a list or tree of [#modules][]:
~~~
<out:
  sin
  abs
  fma
~~~ lang=cycle
] lane module list

~~~
<out:
  sin
    phase:
      sin
      * 0.1
  abs
  fma
~~~ lang=cycle
] lane module tree

note: [#fma][] converts from unipolar (0..1) to bipolar (-1..1) values by default (`*2 - 1`)
note: a patch should contain at least one output section ("lane") (else the output will be 0 / silence)



## Layers
When a script contains multiple lanes, they will be summed to create the final sample output.
This can be used for layered synthesis (lane == layer).


## Init lane
A script may contain an optional `init` section that is evaluated before the main section.

It can be used to implement procedural macro parameters that read args and write one or many variables.

~~~
arg angle 0 0 1

var v_sin
var v_cos

<init:
  sin freq=0 phase=$angle
  sto v_sin
  sin freq=0
    phase:
      $angle
      + 0.25
  sto v_cos
~~~ lang=cycle
] sin / cos init example

note: the `init` section must occur before the first audio lane

note: in plugin-mode, the `init` section is evaluated each time a new note is played (note-on)


## Prepare lane
The `prepare` section is evaluated before the main section(s), and after the (optional) `init` section.

Heavy calculations, which do not need to be evaluated per sample frame, should be placed in the `prepare` section.

~~~
id     simple_2op_fm_osc
name   "simple_2op_fm_osc"
author bsp
cat    osc

param p_op2_ratio 0.5 0 1
param p_op2_level 0 0 16

mod m_op2_ratio
mod m_op2_level

var v_op2_freq

<prepare:
  # ratio 0..1 => (1/4096)^3 .. 1 .. 4^3
  $m_op2_ratio
  # 0..1 => -1..1
  fma 2 -1
  # -1=>div 4096, +1=>mul 4
  bts 4096 4
  pow 3
  # quantize to 100 fractional divisions per integer
  qua 100
  sto v_op2_freq

<out:
  sin
    phase:
      sin freq=$v_op2_freq
      * $m_op2_level
~~~ lang=cycle
] simple 2 operator FM / phase modulation oscillator plugin

note: in plugin-mode, the `prepare` section is called at a rate of 1kHz while a voice is playing.


## Wavetable Cycle Init lane
The optional `wt_init` section is evaluated at the beginning of each wavetable cycle.

It can be used to implement procedural macro parameters that read the built-in `$wt_x` / `wt_y` vars and write one or many variables.

~~~
arg angle_scl 1 0 15
arg angle_off 0 -1 1

var v_sin
var v_cos

<wt_init:
  sin freq=0
    phase:
      $wt_x
      * $angle_scl
      + $angle_off
  sto v_sin
  sin freq=0
    phase:
      $wt_x
      * $angle_scl
      + $angle_off
      + 0.25
  sto v_cos
~~~ lang=cycle
] sin / cos wavetable cycle init example

note: the `wt_init` section must occur before the first audio lane, and after the (optional) `init` lane


# Constants

Constants are typically used for static module input values, e.g.
~~~
<out:
  sin freq=1.5
~~~ lang=cycle
] constant input value

A constant is either a floating point number (in the range -15..15), or a note ratio in the form of `d#3/g-3`:
~~~
<out:
  sin freq=d#4/c-3
~~~ lang=cycle
] constant note ratio

>>>tipconstopt
tip:
Many modules have specialized / optimized code generation paths for constant inputs.

Some inputs only accept constant values (marked by `const` in the module's **Inputs** documentation).
<<<
$(buf!tipconstopt)


# Args

Args are named constants that are declared by the global keyword `arg` (before any output section).
Arg values are editable in the (auto-generated) script UI (slider + value widgets).

Example:
~~~
arg lpf1_ms 30 min=0 max=500
arg lpf1_ms 30 0 500
arg lpf_rng 0.3
~~~ lang=cycle
] declare args

The default (implicit) min / max range is 0..1

Script code can reference args by prefixing the arg id with a `$` sign, e.g.
~~~
arg amp_lvl 1.5

<out:
  sin
  * $amp_lvl
  clp
~~~ lang=cycle
] declare and read arg constant

Instead of `arg`, its shortcut form `a` can be used instead:
~~~
a lvl 0.7

<out:
  sin
  * $lvl
  clp
~~~ lang=cycle
] arg decl shortcut form

caution: args and variables share the same namespace


## Arg Preset Values
Each arg can be assigned a list of preset values (shown when hold-clicking the value widget).

Example:
~~~~
arg        angle 0 0 1
arg_values angle 0="0 deg" 0.25="90 deg" 0.5="180 deg" 0.75="270" 1="360 deg"
~~~~ lang=cycle
] arg preset values

Instead of `arg_values`, its shortcut form `av` can be used instead:
~~~~
a  mode 0 0 3
av mode 0=red 1=green 2=blue
~~~~ lang=cycle
] arg preset values shortcut syntax

note: arg preset values always refer to _scaled_ values, i.e. within min..max range

# Variables

Variables are very similar to args.
While they do **not** generate UI widgets, their content may be updated at any time via the [#sto][] module.

Script code can read variables by prefixing the var id with a `$` sign, e.g.
~~~
var myvar

<out:
  2.5
  sto myvar
  sin
  * $myvar
  clp
~~~ lang=cycle
] declare and read var

Instead of `var`, its shortcut form `v` can be used instead:
~~~
v amp

<out:
  2.5
  sto amp
  sin
  * $amp
  clp
~~~ lang=cycle
] variable shortcut syntax

Variables declared within a lane (or a lane within a template instantiation) will be prefixed with the lane id (and a unique template instantiation id).
~~~
<out:
  var myvar
  2.5
  sto myvar
  sin
  * $myvar
  clp
~~~ lang=cycle
] lane-local var


## Declare and store var shortcut
~~~
<out:
  tri phase=0.25
  vst tri
  + $tri
    hld 0.7
  * 0.5
~~~ lang=cycle
] vst example

see [#vst][]


## Smooth interpolation
In plugin mode, variables can be interpolated smoothly over a processing block.

When `smooth` mode is enabled, the new variable value should be written within the `\<prepare\:` lane.

Reading from the variable within an audio lane will returns its interpolated value.

Example:
~~~~
id smooth_var_test_1

param p_freq 0.5

mod m_freq

var v_freq smooth=1

<prepare:
  $m_freq
  # 0..1 => -1..1
  fma 2 -1
  # -1..1 => 1/4..*4
  bts 4 4
  sto v_freq

<out:
  sin freq=$v_freq
~~~~ lang=cycle
] smoothly interpolated variable

caution: args and variables share the same namespace

## Built-in Variables

`$x` stores the current, normalized position in procedural look-up tables ([#lut][])
`$wt_x` stores the current, normalized horizontal (or 1D) wavetable position (0..1)
`$wt_y` stores the current, normalized vertical (2D) wavetable position (0..1)


# Operators

Each module may be prefixed by an operator that determines how its output is combined with the previous module's output.

| Operator | Id           | Description
|      =  :| OP\_REP      |: Replace
|      +  :| OP\_ADD      |: Add
|      +sr:| OP\_ADD\_SR  |: Add (over)samplerate-independent value
|      -  :| OP\_SUB      |: Subtract `(a-b)`
|      r- :| OP\_RSUB     |: Reverse-Subtract `(b-a)`
|      *  :| OP\_MUL      |: Multiply
|      *sr:| OP\_MUL\_SR  |: Multiply by (over)samplerate-independent value
|      &  :| OP\_AND      |: Bitwise and
|      \| :| OP\_OR       |: Bitwise or
|      ^  :| OP\_EOR      |: Bitwise exclusive-or
|      && :| OP\_LAND     |: Logical and  `(a \> 0) & (b \> 0)`
|    \|\| :| OP\_LOR      |: Logical or  `(a \> 0) \| (b \> 0)`
|    \^\^ :| OP\_LEOR     |: Logical eor  `(a \> 0) ^ (b \> 0)`
|     \!& :| OP\_LNAND    |: Logical nand  `\!((a > 0) & (b \> 0))`
|    \!\| :| OP\_LNOR     |: Logical nor  `\!((a \> 0) \| (b \> 0))`
|     \!^ :| OP\_LNEOR    |: Logical neor `\!((a \> 0) ^ (b \> 0))`
|      m  :| OP\_MIN      |: Minimum
|      x  :| OP\_MAX      |: Maximum
|      M  :| OP\_ABSMIN   |: Absolute minimum
|      X  :| OP\_ABSMAX   |: Absolute maximum
|      .  :| OP\_NONE     |: Emit but discard result of sub-tree
|      \_ :| OP\_SKIP     |: Skip sub-tree (do not emit)
|     am  :| OP\_AM       |: Arithmetic mean
|     qm  :| OP\_QM       |: Quadratic mean sqrt(a*a+b*b)
|     QM  :| OP\_QMU      |: Unipolar Quadratic mean rescaled to -1..1
|     hm  :| OP\_HM       |: Harmonic mean `(2\*a\*b)/(a+b)`
|     HM  :| OP\_HMU      |: Unipolar Harmonic mean rescaled to -1..1
|     gm  :| OP\_GM       |: Geometric mean `sign(a\*b)\*sqrt(abs(a\*b))`
|     GM  :| OP\_GMU      |: Unipolar Geometric mean rescaled to -1..1
].table_ops Operators

>>>tiptreeinitval
The initial output value of a sub-tree is copied from the parent output value.

While this will immediately be overwritten by a new value in most cases, it allows a sub-tree to mix in a processed version of the parent output, e.g. using a [#dly][]:
~~~
<out:
  sin
  rmp 50 1 0
  pow 3
  + dly n=0.2
    * 0.25
~~~ lang=cycle
] process parent output
<<<
tip: $(buf!tiptreeinitval)

>>>tipskiptree
The `\_` operator can be used for (temporarily) skipping a sub-tree (e.g. for testing purposes):
~~~
<out:
  sin
  + sin f=1.5
    * 0.25
~~~ lang=cycle
] sub-tree enabled

~~~
<out:
  sin
  _ sin f=1.5
    * 0.25
~~~ lang=cycle
] sub-tree disabled
<<<
tip: $(buf!tipskiptree)

The default operator is `=` (`replace`):
~~~
<out:
  = sin
~~~ lang=cycle
] default op

~~~
<out:
  sin
~~~ lang=cycle
] default op short form

## Truth table
Logical operator truth table:
~~~~
a b  &&   ||   ^^   !&   !|   !^
0 0  0    0    0    1    1    1
0 1  0    1    1    1    0    0
1 0  0    1    1    1    0    0
1 1  1    1    0    0    0    1
~~~~
] truth table

tip: can be used for pulse / gate signal math (sequencers)


# Modules

Modules generate new sample frames or process the output of the previous module.

Static / constant module input values are set by `key=value` pairs, or simply by value (in internal parameter order)
~~~
<out:
  saw
  lpf freq=0.4 res=0.5
  lpf 0.4 0.5
~~~ lang=cycle
] const module input example

Dynamic module input value sub-trees must be placed on separate, indented lines.
Modules thus become nested (stacked), i.e. module inputs can be controlled by the output of other modules:
~~~
<out:
  sin
    freq:
      sin 5.0
      * 0.25   # could also be written as  fma 0.25 1
      + 1
  * 3.0
  clp
~~~ lang=cycle
] stacked / dynamic module input example

Please note that some module parameters cannot be dynamic (e.g. `hpf` filter configuration).
Also keep in mind that dynamic parameters require more complex code that will lengthen the synthesis duration.

tip: some module inputs have implicit *previous output* values, like the `pd` (phase-distortion) *insert* of the [#pha][] module.

tip: in the Cycle machine source editor, press `lctrl-i` to expand a basic `key=value` input to (an arbitrarily complex) input sub-tree


## abs
] discard sign of previous output

Return absolute value of previous output.

##-# Example
~~~
<out:
  -42
  abs
~~~ lang=cycle
] absolute

This is equivalent to full-wave rectification ([#fwr][]).

See [#hwr][] for half-wave rectification.


## adsr
] four stage envelope generator

Analog-style Attack / Decay / Sustain / Release envelope generator.

The previous output should be a gate signal (rising edge restarts envelope, falling edge starts release phase).

##-# Inputs

|noheader
|:       |:`gate` / `g`   |:gate input (when not connected, use previous output)
|:       |:`a`            |:(attack rate) range is 0..1
|:       |:`as`           |:(attack shape) range is -16..16
|:       |:`d`            |:(decay rate) range is 0..1
|:       |:`ds`           |:(decay shape) range is -16..16
|:       |:`s`            |:(sustain level) range is 0..1
|:       |:`r`            |:(release rate) range is 0..1
|:       |:`rs`           |:(release shape) range is -16..16`
|: const |:`reset`        |:0=reset level only during voice allocation ("analog") 1=reset level when envelope restarts ("digital")
|:       |:`scl`          |:sets rate scaling (0..f) (e.g. 0.01=suitable for "prepare" lane (1000Hz rate), 0.001=suitable for audio rate) (default is 0.001)
|: const |:`mode` / `m`   |:selects envelope flavour. 0=rate-shaping (v2/default, smoother). 1=level-shaping (v1, more percussive)
|: const |:`vel` / `v`    |:enables velocity (gate level) scaling (1=on, 0=off)
|:       |:`onend` / `oe` |:evaluated when end of envelope is reached
].table_inputs

tip: it's often sufficient to calculate envelopes in a `\<prepare:` lane (i.e. per 1kHz sample block) instead of per sample frame


## apf
] all-pass filter

All-pass filter (1st order) previous output.

##-# Examples
~~~
arg p_amp 0.7 0 8
arg p_c   0.5 -1 1

<out:
  saw
  apf c=$p_c
  * rmp 0 1
  * $p_amp
  clp
~~~ lang=cycle
] test apf 1 example patch

~~~
arg p_amp 0.7 0 8
arg p_f   1.0 0 1

<out:
  saw
  apf f=$p_f
  * rmp 0 1
  * $p_amp
  clp
~~~ lang=cycle
] test apf 2 example patch

~~~
arg p_amp 0.7 0 8
arg p_f   1.0 0 1

<out:
  saw
  apf ## dynamic frequency input (=> force float mode)
    f:
      $p_f
      * rmp 0 1
  * rmp 0 1
  * $p_amp
  clp
~~~ lang=cycle
] test apf 3 example patch


~~~
id simple_phaser

mp c 0.5
mp ap_mix 0.5

<out:
  $c
  fma #0..1=>-1..1
  vst v_c

  saw
  ipl t=$ap_mix
    b:
      rep 4
        apf c=$v_c
  clp
~~~ lang=cycle
] test apf 4 simple_phaser plugin


##-# Inputs

|noheader
|:       |:`c`          |: sets filter coefficient (-1..1)
|:       |:`freq` / `f` |: sets normalized filter frequency (0..1) (overrides coefficient input)
].table_inputs

caution: when the `freq` input is connected and it is not a constant value, the module falls back to float mode (for tan())


## bit
] bit-crush previous output

Bit-crush (shift) previous output.

Converts the input to a 16 bit integer and right-shifts it by `shift*15` bits.

##-# Examples
~~~
<out:
  sin
  bit shift=0.5
  bit 0.5
~~~ lang=cycle
] 8 bit reduction

~~~
arg p_amp 0.7 0 8
arg p_bit 8 0 15

def bit_linear bit=8
  vst p
  $p
  bit
    shift:
      %bit%
      itg
      * 1/15
  ipl
    b:
      $p
      bit
        shift:
         %bit%
         itg
         fam 1 1/15
    t:   
      %bit%
      frc

<out:
  sin
  ~bit_linear bit=$p_bit
  * rmp 0 1
  * $p_amp
  clp
~~~ lang=cycle
] linear bit reduction template

~~~
arg p_amp 0.7 0 8
arg p_bit 8 0 15

inc bit_linear

<out:
  sin
  ~bit_linear bit=$p_bit
  * rmp 0 1
  * $p_amp
  clp
~~~ lang=cycle
] linear bit reduction template include

##-# Inputs

|noheader
|:       |:`shift` / `s` |:range is 0..1 (0..16 bits)
].table_inputs


## boo
] boost previous output

Boost sample deltas of previous output.

##-# Example
~~~
<out:
  sin
  boo amt=0.6
  boo 0.6
~~~ lang=cycle
] boost sample deltas

##-# Inputs

|noheader
|:       |:`amount` / `amt` |:range is 0..1                          |:
|: const |:`sr`             |:1=(over)samplerate-independent amount. |: def=1
].table_inputs


## box
] box filter previous output

Box filter previous output.

Simple but computationally inexpensive.

##-# Example
~~~
<out:
  saw
  box freq=0.5
  box 0.5
~~~ lang=cycle
] box lowpass filter

##-# Inputs

|noheader
|:       |: `freq` / `f` |: range is 0..1                               |: 
|: const |: `sr`         |: 1=(over)samplerate-independent filter freq. |: def=1
].table_inputs


## bpf
] --reserved--

**reserved**
tip: use [#svf][] in `mode=bpf` instead


## bts
] bipolar to scale, e.g. -1..1 =\> 1/16..\*1..\*16

Bipolar to scale.

##-# Example
~~~
arg $freq
<out:
  sin
    freq:
      $freq
      # 0..1 => -1..1
      fma 2 -1
      # -1..1 => 1/16 .. *16
      bts 16
~~~ lang=cycle
] bipolar to exponential scale

##-# Inputs

|noheader
|:       |: `div` / `d` |: range is 1..32 |: def=8
|:       |: `mul` / `m` |: range is 1..32 |: def=8
].table_inputs

note: uses `div` input when `mul` is not connected

caution: only supported in float-mode (fall back to float in int mode)


## buf
] --reserved--

**not implemented, yet**


## ceq
] shortcut for cmp ==

~~~
var t
<out:
  $t
  ceq 0
    1
  sto t
~~~~ lang=cycle
] ceq example

##-# Inputs

|noheader
|:       |: `rhs` |: right-hand side value
].table_inputs

see [#cmp][]


## cge
] shortcut for cmp \>=

~~~
var t
<init:
  set t 0

<out:
  $t
  +sr 0.0001
  cge 1
    1
  sto t
~~~~ lang=cycle
] cge example

##-# Inputs

|noheader
|:       |: `rhs` |: right-hand side value
].table_inputs

see [#cmp][]


## cgt
] shortcut for cmp \>

~~~
var t
<init:
  set t 0

<out:
  $t
  +sr 0.0001
  cgt 1
    1
  sto t
~~~~ lang=cycle
] cgt example

##-# Inputs

|noheader
|:       |: `rhs` |: right-hand side value
].table_inputs

see [#cmp][]


## cle
] shortcut for cmp \<=

~~~
var t
<init:
  set t 0

<out:
  $t
  cle 0.5
    +sr 0.001
  sto t
~~~~ lang=cycle
] cle example

##-# Inputs

|noheader
|:       |: `rhs` |: right-hand side value
].table_inputs

see [#cmp][]


## clk
] generate clock pulses

Generate clock pulse width configurable duty cycle duration.

##-# Example
~~~~
id clk_test
<out:
  sin
  * clk div=1/4
    adsr
~~~~ lang=cycle
] modulate sine level by adsr env which is retriggered every 16th note

##-# Inputs

|noheader
|:       |: `hz`             |: range is 0..f (clock ticks per second) |: 
|:       |: `bpm`            |: range is 0..f (0=host bpm, beats per minute (quarter notes) otherwise. used when bpm is not connected) |: def=0.0
|:       |: `div`            |: range is 0..f (clock divider. <1 speeds up clock, >1 slows it down)  |: def=1.0
|:       |: `dur`            |: duty cycle duration in milliseconds |: def=5
|: const |: `force` / `f`    |: 1=emit 0 at end of duty cycle when cycle duration is longer than interval |: def=1
|:       |: `restart` / `rs` |: restart clock counter on rising edge
|:       |: `swing` / `sw`   |: \<0: pull every 2nd pulse \>0: push/delay every 2nd pulse  |: def=0.0
|:       |: `onclk` / `oc`   |: evaluated on each clock pulse. must return new pulse level (0..1, 0 discards pulse)
].table_inputs


## clp
] clip previous output

Clip previous output

##-# Examples
~~~
<out:
  sin
  * 7
  clp
~~~ lang=cycle
] clip to -1..+1

~~~
<out:
  sin
  * 7
  clp 0.5
~~~ lang=cycle
] clip to -0.5..+0.5

~~~
<out:
  sin
  * 7
  clp ceil=0.6 floor=-0.75
~~~ lang=cycle
] separate boundaries

~~~
<out:
  sin
  * 7
  # same as clp ceil=0.6 floor=-0.75
  clp 0.6 -0.75
~~~ lang=cycle
] separate boundaries (short form)

##-# Inputs

|noheader
|:       |: `ceil` / `c`  |: range is 0..1 (default=1)
|:       |: `floor` / `f` |: range is -1..1 (default=-1) (when not connected, floor = -ceil)
].table_inputs


## clt
] shortcut for cmp \<

~~~
var t
<init:
  set t 1

<out:
  $t
  +sr -0.001
  clt 0
    0
  sto t
~~~~ lang=cycle
] clt example

##-# Inputs

|noheader
|:       |: `rhs` / `r` |: right-hand side value
].table_inputs

see [#cmp][]


## cmp
] run conditional statement sequence

Compare previous output value to right-hand side value and run statement sequence when result is true.

Supported operators: `\<`, `\<=`, `==`, `\!=`, `\>=`, `\>`.

~~~
var t
<init:
  set t 1

<out:
  $t
  +sr -0.001
  cmp < 0
    0
  sto t
~~~~ lang=cycle
] cmp example

~~~
id cmp_test_2
var t
<out:
  tri
  fma 0.6 0.4
  sto t
  sin
  clt
    rhs:
      $t
      neg
    -1
  cgt $t
    1
  box 0.8
~~~~ lang=cycle
] cmp example plugin

~~~
id test_cmp_else_1

<prepare:
  tri
  clt 0
      -1
    else:
      1
  trc

<out:
  sin
~~~ lang=cycle
] if / else example

##-# Inputs

|noheader
|:       |: `rhs` / `r` |: right-hand side value
|:       |: `else`      |: evaluated when comparison failed
].table_inputs

tip: see [#clt][], [#cle][], [#ceq][], [#cne][], [#cge][], [#cgt][] short cut modules


## cne
] shortcut for cmp \!=

~~~
var t
<out:
  $t
  cne 1
    1
  sto t
~~~~ lang=cycle
] cne example

##-# Inputs

|noheader
|:       |: `rhs` |: right-hand side value
].table_inputs

see [#cmp][]


## con
] --reserved--

**reserved**


## dec
] decrement variable

Decrement variable

If the `delta` input is not connected, decrement variable by previous output value.

~~~
var f
<out:
  set f 0
  rep 16
    dec f 0.1
    dec f delta=0.1
    dec f
      delta:
        0.05
        * 2
~~~ lang=cycle
] decrement variable

~~~
var f
<out:
  set f 0
  0.1
  dec f
~~~ lang=cycle
] decrement by 0.1

##-# Inputs

|noheader
|:       |: `delta` |: when connected, decrement variable by given value. when not connected, decrement by previous output value.
].table_inputs


## div
] divide

Divide values.

##-# Example
~~~
arg p_num 16 1 32
<out:
  div 1.0 $p_num
~~~ lang=cycle
] output 1 / $p_num

##-# Inputs

|noheader
|:       |: `a` |: nominator input
|:       |: `b` |: denominator input
].table_inputs

tip: useful in conjunction with `rep` / `lop`, e.g. when indexing LUTs

caution: this is a rather expensive operation in integer / lo-fi mode (uses iterative approximation when inputs are not constant)


## dly
] delay

Store previous output value in delay ring buffer read delayed value.

If the `freq` input is connected, the module is set to tuned-delay-line mode (key follow) and the input value determines the delay length scaling (e.g. 2.0 doubles the length and thus transposes the delay one octave down).

##-# Examples
~~~
arg p_amp 0.7 0 2

<out:
  sin
  * rmp 50 1 0
    pow 3
  + dly len=0.08 fb=0.5
    * 0.25
  * $p_amp
  clp
~~~ lang=cycle
] dly test 1

~~~
# test feedback insert
arg p_amp 0.7 0 2

<out:
  saw
  * rmp 100 1 0
    pow 3
  svf r=1
    f:
      rmp 70 1 0
      pow 3
      p2s
      fma 0.8 0.2
  + dly size=32768 len=0.2 fb=0.5
      read:
        svf f=0.1 r=0.6
    * 0.25
  * $p_amp
  clp
~~~ lang=cycle
] dly test 2

~~~
# tuned delay line test
skip 180
oct  0

arg p_amp     0.7  0   2
arg p_scl     1    0   4
arg p_fb      0.67 0   1
arg p_ins_flt 0.1  0   1
arg p_ins_res 0.64 0.1 1
arg p_mix     0.9  0   1

<out:
  fsr
  * rmp 10 1 0
    pow 3
  * 1.0
  svf r=0.5
    f:
      rmp 50 1 0
      pow 3
      p2s
      fma 0.5 0.5
  ipl t=$p_mix
    b:
      dly s=1024 fb=$p_fb
        f:
          $p_scl
        r:
          svf r=$p_ins_res
            f:
              $p_ins_flt
              p2s
  * $p_amp
  #hbx f=0.7
  clp
~~~ lang=cycle
] dly test 4

~~~
# tuned delay line voice plugin test
id test_dly_stfx_4

mp dly_scl  0.224
mp dly_fb   0.889
mp dly_mix  0.987
mp osc_lpf  0.919
mp osc_res  1.0
mp dly_lpf  0.228
mp dly_res  1.0
mp osc_lvl  1.0

<out:
  fsr reset=0
  * $m_osc_lvl

  svf r=$m_osc_res
    f:
      $m_osc_lpf
      kbd mode=1 amt=1
  
  ipl
    t:
      $m_dly_mix
      clp 1 0
    b:
      dly s=8192 fb=$m_dly_fb lin=1
        f:
          $m_dly_scl
          fma 2 -1
          bts 16 16
        read:
          svf
            r:
              $m_dly_res
            f:
              $m_dly_lpf
              kbd mode=1 amt=1
  clp
~~~ lang=cycle
] dly test stfx 4


##-# Inputs

|noheader
|: const |: `size` / `s`      |: maximum delay buffer size in frames            |: def=32768
|:       |: `len` / `n`       |: current delay buffer length (0..1 => 0..size)  |: def=0.5
|:       |: `freq` / `f`      |: tuned delay line rate divider (1=note speed, 0.5=1 oct up, ..) (when connected)
|:       |: `feedback` / `fb` |: feedback amount (0..1)                         |: def=0
|: const |: `lin` / `l`       |: 1=linear interpolation 0=nearest neighbour)    |: def=0 (nearest)
|:       |: `read` / `r`      |: delay-read insert (pre feedback)
|:       |: `write` / `w`     |: delay-write insert (post feedback)
].table_inputs

note: in lo-fi / integer mode, the delay buffer size is limited to 32767 frames
note: the delay size is multiplied by the oversample factor
note: linear interpolation is only supported in hi-fi / float mode

see [#xdly][]


## drv
] --reserved--

**reserved**


## end
] close current lane

Close current lane.

note: optional. can be used for placing [#lut][] declarations in between lanes.

note: when lane is a lut init lane, skip the output write (lane must write to lut via [#sta][] instead)


## env
] AD/S/R envelope generator

Generates a three-segment (Attack/Decay, Sustain, Release) envelope.

Previous output is gate signal (rising edge (re-)starts envelope, falling edge starts release phase).

The subtree takes the current normalized envelope position (0..1) as input and must return the envelope level for that position.

##-# Example
~~~~
id env\_1
# (note) please map "Gate" to plugin mod "gate" in the mod matrix or the envelope won't trigger
mod m\_gate
# Bezier curve 0 stores the envelope shape
curve 0 env
<out:
  sin
  * $m_gate
    env ad=0.07 s=0.4 r=0.01 scl=0.001
      # subtree returns env level for normalized position input 0..1
      lut env lin=1 clamp=1
~~~~ lang=cycle
] env example

### loop
When `loop` mode is enabled, the gate-off signal is ignored and the envelope will repeat continously, switching `ad` / `r` rates at the `s`ustain position.

This can (also) be used for audio-rate waveform synthesis.

tip: scale the rates by `kbd mode=2 amt=1` for chromatic key tracking

##-# Example
~~~~
id env\_3
oversample 4

modparam ad 0.5 0 1
modparam s 0.5
modparam r 0.5

# Bezier curve 'env' stores the envelope shape
curve env

var v_ad  smooth=1
var v_s   smooth=1
var v_r   smooth=1

<prepare:
  $m_ad
  clp 1 0
  pow 3
  kbd mode=2 amt=1
  sto v_ad

  $m_s
  sto v_s

  $m_r
  clp 1 0
  pow 3
  kbd mode=2 amt=1
  sto v_r

<out:
  sin
  * 1 # gate
    env ad=$v\_ad s=$v\_s r=$v\_r scl=0.01 loop=1
      lut env lin=1 clamp=1
    # slw 0.05 0.1
~~~~ lang=cycle
] looping audio-rate envelope


##-# Inputs

|noheader
|:       |: `ad`             |: attack/decay rate (0..1)   |: def=0.01
|:       |: `s`              |: sustain position (0..1)    |: def=0.5
|:       |: `r`              |: release rate (0..1)        |: def=0.01
|: const |: `loop` / `lp`    |: 1=loop 0=oneshot / halt at sustain pos  |: def=0 (off)
|:       |: `restart` / `rs` |: restart signal
|:       |: `sr`             |: enable sample-rate independent envelope speed (must be const) (voice plugins only) |: def=1 (on)
|:       |: `onend` / `oe`   |: evaluated when end of envelope is reached / envelope loops
|:       |: `scl`            |: envelope rate scaling (0..1)  |: def=0.001
|: const |: `tuned` / `tun`  |: normalize envelope rates and follow pitch (auto-enables loop mode)  |: def=0 (off)
].table_inputs


## eq3
] 3-band equalizer

A simple 3-band equalizer

~~~~
arg p_amp 0.7 0 8

arg fl 0.1 0 1
arg fh 0.2 0 1
arg gl 0.5 -2 2
arg gm 0.5 -2 2
arg gh 0.5 -2 2
arg eq3_amt 1 0 1
arg p_amp_eq 1.5 0.5 4

<out:
  saw
  * $p_amp
  ipl t=$eq3_amt
    b:
     eq3 freqlo=$fl freqhi=$fh gainlo=$gl gainmid=$gm gainhi=$gh
     * $p_amp_eq
  clp
~~~~ lang=cycle
] eq3 test 1 example

##-# Inputs

|noheader
|:       |: `freqlo` / `fl`  |: Low frequency (0..1)                          |: def=0.5
|:       |: `freqhi` / `fh`  |: High frequency (0..1)                         |: def=0.5
|:       |: `gainlo` / `gl`  |: Low Gain (negative values invert phase)       |: def=0.5
|:       |: `gainmid` / `gm` |: Mid Gain                                      |: def=0.5
|:       |: `gainhi` / `gh`  |: High Gain                                     |: def=0.5
|: const |: `sr`             |: 1=(over)samplerate-independent freqlo/freqhi. |: def=1
].table_inputs


## fed
] detect logic signal falling edge

Detect logic signal falling edge and evaluate sub-tree.

~~~~
id edge_detect

var v_out
var v_dcy

<out:
  pul
  red
    set v_dcy 0.999
    set v_out 1
  fed
    set v_dcy 0.95
  $v_out
  *sr $v_dcy
  sto v_out
  fma 2 -1
~~~~ lang=cycle
] rising / falling edge detect osc

note: this module has no inputs

see also: [#red][]


## fix
] fix NaN and Inf (debug aid)

Replace NaN (Not a Number) and Infinity values by 0.

This should not be necessary in regular patches but may be useful for debugging purposes.


## fld
] fold previous output

Fold previous output value at ceiling.

##-# Examples
~~~
<out:
  sin
  * 2.0
  fld ceil=1.0
~~~ lang=cycle
] fold sine oscillator

note: `ceil` sets min=-ceil, max=+ceil

~~~
<out:
  sin
  * 2.0
  fld min=-1.0 max=1.0
~~~ lang=cycle
] fold sine oscillator (separate min / max)
note: `min` / `max` override default ceil

##-# Inputs

|noheader
|:       |: `ceil` / `c` |: sets min=-val, max=+val |: def=1.0
|:       |: `min` / `m`  |: minimum value
|:       |: `max` / `x`  |: maximum value
|: const |: `hq`         |: 1=enable high-quality (float) mode (arbitrary number of folds). |: def=1 (on)
].table_inputs

see also: [#wrp][]


## fam
] fused add multiply

Fused add multiply: `out = (out + add) * mul`

##-# Examples
~~~
<out:
  sin
  # -1..+1 => 0..1
  fam
~~~ lang=cycle
] bipolar to unipolar example

~~~
arg exp 1 0 16

<out:
  sin
  fam # -1..+1 => 0..1
  pow $exp
  fma # 0..1 => -1..+1
~~~ lang=cycle
] test_fam_1 example patch

##-# Inputs

|noheader
|:       |: `add` / `a` |: input is in range -15..15  |: def=1.0
|:       |: `mul` / `m` |: input is in range -15..15  |: def=0.5
].table_inputs


## fma
] fused multiply add

Fused multiply add: `out = out * mul + add`

##-# Example
~~~
<out:
  sin
  # -1..+1 => 0..1
  fma 0.5 0.5
  # 0..1 => -1..+1
  fma
~~~ lang=cycle
] fused multiply add

tip: by default, [#fma][] converts unipolar values to bipolar (`0..1 => -1..1`) while [#fam][] converts bipolar to unipolar (`-1..1 => 0..1`)

~~~
<out:
  sin
  fam ## to unipolar
  pow
  fma ## back to bipolar
~~~ lang=cycle
] bipolar <=> unipolar

##-# Inputs

|noheader
|:       |: `mul` / `m` |: input is in range -15..15  |: def=2.0
|:       |: `add` / `a` |: input is in range -15..15  |: def=-1.0
].table_inputs


## frc
] fractional part of previous output

Calculate fractional part of previous output (discard integer bits)

##-# Examples
~~~
<out:
  1.67
  frc      # => 0.67
  trc

  -1.67
  frc      # => 0.33
  trc

  -1.67
  frc s=1  # => -0.67
  trc
~~~ lang=cycle
] unsigned and signed fractional parts

~~~
<out:
  sin
  * 3.3
  frc s=1
~~~ lang=cycle
] test frc 1 example patch

##-# Inputs

|noheader
|: const  |: `signed` / `s` |: 0=unsigned mode 1=signed mode |: def=0 (unsigned)
].table_inputs

tip: the (default) unsigned mode is especially useful for oscillator phases (i.e. to wrap any value into the normalized 0..1 range)

note: [#itg][] does the opposite and discards all fractional bits


## fsr
] linear feedback shift register noise

Generate linear feedback shift register ("C64") noise (normalized to -1..1).

##-# Examples
~~~
<out:
  fsr
~~~ lang=cycle
] noise

~~~
id noise
<out:
  fsr seed=$4489 bits=1.0 reset=0
~~~ lang=cycle
] noise voice plugin with custom seed and no note-on reset

##-# Inputs

|noheader
|: const |: `seed` / `s`     |: is the initial shift register state (16 bits). must not be 0. |: def=$4489
|: const |: `bits` / `b`     |: determines the number of output bits (1.0=all bits, 0=1 bit (actually two, sign+msb). |: def=1.0
|: const |: `cycle` / `cy`   |: determines whether the shift register state resets at each (wavetable) cycle. Ignored in plugin mode. |: def=1 (true)
|: const |: `shift1` / `sh1` |: is the first feedback shift (negative=shift left). -15..15. |: def=7
|: const |: `shift2` / `sh2` |: is the second feedback shift. -15..15. |: def=-9
|: const |: `shift3` / `sh3` |: is the third feedback shift. -15..15. |: def=13
|: const |: `reset` / `rs`   |: range is 0..1 (0=disable plugin voice note on reset and eval only when voice is allocated) |: def=1
|: const |: `signed` / `sgn` |: 1=generate signed values (-1..1)  0=generate unsigned values (0..1)  |: def=1
].table_inputs

note: changing the number of `bits` does not make an audible difference, except when the output is used as a controller value

note: `fsr` noise contains less low frequencies than `nos` noise


## fwr
] full-wave rectification

Discard sign of previous output and optionally rescale to -1..1 range.

##-# Examples
~~~
<out:
  -0.8
  fwr   # ==> 0.8
  0.9 b=1
  fwr   # ==> 0.8  (== 0.9*2-1)
~~~ lang=cycle
] uni- and bipolar full wave rectification

~~~
id fwr_test_1

mp amp 0.5

<out:
  sin
  fwr b=1  ## bipolar mode scales 0..1 output to -1..1
  * $m_amp
    * 2
  clp
~~~ lang=cycle
] fwr_test_1 voice plugin example


See [#hwr][] for half-wave rectification.

##-# Inputs

|noheader
|: const |: `bipolar` / `b` |: range is 0/1 (1=rescale to -1..1 range)  |: def=0 (unipolar)
|: const |: `neg` / `n`     |: range is 0/1 (1=flip output sign)        |: def=0 (positive)
].table_inputs


## hbx
] high-pass box filter

High-pass box filter previous output.

Simple but computationally inexpensive.

##-# Example
~~~
<out:
  saw
  hbx freq=0.5
  hbx 0.5
~~~ lang=cycle
] high-pass box filter

##-# Inputs

|noheader
|:       |: `freq` / `f` |: range is 0..1                         |: def=0.5
|: const |: `sr`         |: 1=(over)samplerate-independent freq.  |: def=1 (on)
].table_inputs


## hwr
] half-wave rectification

Clip previous output to 0..f and optionally rescale to -1..1 range.

##-# Examples
~~~~
<out:
  -0.8
  hwr   # ==> 0
  0.9 b=1
  hwr   # ==> 0.8  (== 0.9*2-1)
~~~~ lang=cycle
] half-wave rectification

~~~~
id hwr_test_2_max
<out:
  sin
  vst a
  sin phase=0.1 f=2.01
  vst b
  $a   # max(a,b) = (b+hwr(a-b))
  - $b
  hwr
  + $b
~~~~ lang=cycle
] max

~~~
id hwr_test_3_min
<out:
  sin
  vst a
  sin phase=0.1 f=2.01
  vst b
  # min(a,b) = (a-hwr(a-b))
  $a
  - $b
  hwr n=1   # -hwr
  + $a   
~~~ lang=cycle
] min


See [#fwr][] / [#abs][] for full-wave rectification.

##-# Inputs

|noheader
|: const |: `bipolar` / `b` |: range is 0/1 (1=rescale to -1..1 range)  |: def=0 (unipolar)
|: const |: `neg` / `n`     |: range is 0/1 (1=flip output sign)        |: def=0 (positive)
].table_inputs


## hld
] sample and hold previous output

Sample and hold previous output.

##-# Example
~~~
<out:
  hld rate=0.5
~~~ lang=cycle
] sample and hold

##-# Inputs

|noheader
|:       |: `rate` / `r`      |: range is 0..1 (=> 0..samplerate)                    |: def=1.0
|: const |: `sr`              |: 1=(over)samplerate-independent freq. 0=fixed freq.  |: def=1 (on)
|:       |: `onsample` / `os` |: evaluated when a new sample is taken. input is previous output, output is new sample&hold value. does not modify previous output.
].table_inputs

note: when the previous output uses complex calculations, it is more efficient to render these into a separate zone at a lower rate, then read the precalced values via `zon`


## hpf
] high-pass filter previous output

High-pass filter previous output. Uses preconfigured, static cutoff/resonance configurations (biquad coefficients).

##-# Example
~~~
<out:
  saw
  hpf cfg=3
~~~ lang=cycle
] high-pass filter


### Filter configurations

| cfg:| cutoff:| resonance :|
| 0   | 127    | 0
| 1   | 127    | 108
| 2   | 122    | 0
| 3   | 122    | 108
| 4   | 115    | 0
| 5   | 115    | 100
| 6   | 110    | 0
| 7   | 110    | 100
| 8   | 100    | 0
| 9   | 100    | 97
| 10  | 90     | 0
| 11  | 90     | 70
| 12  | 77     | 0
| 13  | 77     | 79

##-# Inputs

|noheader
|: const |: `cfg` / `c` |: selects filter config (0..13)  |: def=0
].table_inputs


## if
] conditional code generation

Conditional code generation (compile time).

Syntax:
~~~
  if <arg> [<,<=,==,!=,>=,>] <constant>
    then:
       ..
    else:
       ..
~~~ lang=cycle

##-# Example
~~~
arg mod_waveform;

<out:
  if mod_waveform == 0
    then:
      nos
    else:
      if mod_waveform < 0.33
        then:
          sin
        else:
          if mod_waveform < 0.66
            then:
              tri
            else:
              saw
~~~ lang=cycle
] compile time if / else cascade

##-# Inputs

|noheader
|:       |: `then`  |: evaluated when expression is true
|:       |: `else`  |: evaluated when expression is false
].table_inputs


## inc
] increment variable

Increment variable

If the `delta` input is not connected, increment variable by previous output value.

##-# Examples
~~~
var f
<out:
  set f 0
  rep 16
    inc f 0.1
    inc f delta=0.1
    inc f
      delta:
        0.05
        * 2
~~~ lang=cycle
] increment using delta input

~~~
var f
<out:
  set f 0
  0.1
  inc f
~~~ lang=cycle
] increment by 0.1

##-# Inputs

|noheader
|:       |: `delta` |: when connected, increment variable by given value. when not connected, increment by previous output value.
].table_inputs


## int
] emit sub-tree in integer mode

Evaluate sub-tree in integer mode.

The previous output value is converted to 16bit fixpoint (1\:4\:11) before the sub-tree is emitted. Afterwards, the integer output value is converted back to float.

##-# Example
~~~
<out:
  int
    sin
~~~ lang=cycle
] integer sub-tree

note: in integer mode, no conversions take place and the sub-tree is emitted as usual


## ipl
] Interpolate from a to b at time t

Interpolate from `a` to `b` at time `t`.

The module behaves differently depending on which inputs are connected:
  - when `a` is connected and `b` is not, the previous output is used as the destination
  - when `b` is connected and `a` is not, the previous output is used as the source
  - when `a` and `b` are connected and `t` is not, the previous output is used as the interpolation amount
  - when `a`, `b` and `t` are connected, the previous output is ignored

##-# Examples
~~~~
arg a_blend 0 0 1
<out:
  ipl win=smoothstep
    a:
      sin
    b:
      tri
    t:
      $a_blend
~~~~ lang=cycle
] blend between sine and triangle using a smoothstep window

~~~~
id osc_vsync

param p_vsync       0 0 1
param p_window_amt  1 0 1
param p_window_freq 0 0 1

mod m_vsync
mod m_window_amt
mod m_window_freq

<out:
  sin
    vsync:
      # 0..1 => 1..16
      $m_vsync
      fma 15 1
  # interpolate between sine wave and 0 using a triangle window
  #  (note) the 'a' input of the 'ipl' module is the previous output (sin)
  #          (since 'a' is not explicitily connected)
  ipl b=0
    t:
      1.0
      - tri
         freq:
           # 0..1 => 1..16
           $m_window_freq
           fma 15 1
           # to integer
           itg
        # -1..1 => 0..1
        fma 0.5 0.5
      * $m_window_amt
~~~~ lang=cycle
] virtual oscillator sync voice plugin that uses a triangle window with customizable frequency

##-# Inputs

|noheader
|:       |: `a`   |: source value         |: def=0.0
|:       |: `b`   |: destination value    |: def=1.0
|:       |: `t`   |: interpolation amount |: def=1.0
|: const |: `win` |: interpolation window. 0=linear, 1=smoothstep, 2=in\_quad, 3=out\_quad, 4=in\_cubic, 5=out\_cubic, 6=in\_quintic, 7=out\_quintic  |: def=0 (linear)
].table_inputs

tip: The initial input to `a`, `b`,  `t` (mode-dependent) is the previous output. This can be used to crossfade between processed versions of the input signal.



## itg
] integral part of previous output

Calculate integral part of previous output (discard fractional bits)

##-# Example
~~~
<out:
  sin
  * 16
  itg
  * 1/16
~~~ lang=cycle
] cast to integer

note: [#frc][] does the opposite and discards all integer bits


## kbd
] MIDI note keyboard tracking

`mode=0`:
  - scale input by 127 (normalized float to MIDI note), add `(current_note - 64 + (off * 12))`, clamp to 0..127 (when `clamp=1`), then convert the note to its corresponding frequency and divides it by the sample rate (back to normalized 0..1 range).
  - ignore `ctr` input (can use `off` instead)
  - useful for filter cutoff frequencies (linear to exponential).
  - factors in current sample_rate and oversample_factor
     - caution when passing the output to another module which also does this !

`mode=1`:
  - equal tempered rate scaling
  - multiply input by note rate. keyboard tracking is centered around `ctr` note (def=60=5*12=C-5).
  - e.g. at center note and `off=0`, rate is 1.0. at one octave above it's 2.0, and at one octave below it's 0.5
  - ignore `scl` and `clamp` inputs
  - useful for scaling oscillator `freq` (set osc `fixed=1` to disable built-in keyboard tracking)

`mode=2`:
  - linear(exponent) rate scaling
  - multiply input by note rate. keyboard tracking is centered around `ctr` note (def=60=5*12=C-5).
  - e.g. at center note and `off=0`, rate is 1.0. at one octave above it's 2.0, and at one octave below it's 0.5
  - ignore `scl` and `clamp` inputs
  - can also be used for scaling oscillator `freq` (set osc `fixed=1` to disable built-in keyboard tracking)
  - the CPU load is lighter but the rate does not exactly follow the equal tempered scale


##-# Examples
~~~
id     cutoff_kbd_tracking_test
name   "cutoff kbd tracking test"
author bsp
cat    filter

param p_cutoff 0.5 0 1
param p_keyfollow 1 0 1
param p_octave_off 0 -4 4

<out:
  svf
    freq:
      $p_cutoff
      kbd $p_keyfollow $p_octave_off
~~~ lang=cycle
] filter cutoff keyboard tracking example plugin

~~~
id     pitch_kbd_tracking_test
name   "pitch kbd tracking test"
author bsp
cat    osc

param p_keyfollow 1 0 1
param p_octave_off 0 -4 4

<out:
  sin fixed=1
    freq:
      1.0
      kbd mode=1 amt=$p_keyfollow off=$p_octave_off ctr=60
~~~ lang=cycle
] oscillator pitch keyboard tracking example plugin

##-# Inputs

|noheader
|:       |: `amt` / `a`   |: is in the range -16..16 (keyboard follow amount)        |: def=1.0
|:       |: `off` / `o`   |: is in the range -4..4 (octave offset)                   |: def=0.0
|:       |: `scl` / `s`   |: is in the range 0..16 (post scaling)                    |: def=4.0
|: const |: `clamp` / `c` |: toggles clamp-to-1.0 (should be set to 0 when using this for pitch tracking) |: def=1 (on)
|: const |: `mode` / `m`  |: selects mode (0=filter 1=eqtemp 2=lin_exp)              |: def=0 (filter)
|: const |: `ctr`         |: sets keyboard center note (only used in `pitch=1` mode) |: def=60 (C-5)
].table_inputs

caution: only supported in plugin / float mode (fall back to float within int-subtree)


## lle
] log / lin / exp scaling

Scale previous output by logarithmic(`c\<0`)..linear(`c=0`)..exponential(`c\>0`) factor

##-# Example
~~~
<out:
  8
  lle c=1.4
  trc
~~~ lang=cycle
] log / lin / exp

##-# Inputs

|noheader
|:       |: `c` |: is in the range -f..f.  |: def=0.0
].table_inputs

caution: only supported in float-mode (fall back to float in int mode)


## log
] calc logarithm of previous output

Return logarithm of previous output.

##-# Example
~~~
<out:
  8
  log base=2
  trc
~~~ lang=cycle
] calc log2(8)

##-# Inputs

|noheader
|:       |: `base` / `b` |: range is 1..f. When not connected, return the natural logarithm.
].table_inputs

caution: only supported in float-mode (fall back to float in int mode)


## lop
] loop statement sequence

Loop statement sequence. Supports dynamic loop iterators.

~~~
<out:
  2.0
  * 4.0
  vst num
  0.0
  vst i
  lop $num
    + 0.01
~~~ lang=cycle
] dynamic loop

##-# Inputs

|noheader
|:        |: `num` / `n` |: range is 0..1023 (0..15 in integer / lo-fi mode)  |: def=1
].table_inputs

CAUTION: some modules (e.g. `sin`) use variables to store their state. evaluating such modules in a loop will cause all iterations to use the same state.
TIP: use `rep` to generate unrolled loops where each iteration will have its own state


## lpf
] 1RC low pass filter

1RC resonant low pass filter.

Simple but computationally inexpensive.

##-# Example
~~~
<out:
  saw
  lpf freq=0.4 res=0.5
  lpf 0.4 0.5
~~~ lang=cycle
] low-pass filter

##-# Inputs

|noheader
|:       |: `freq` / `f` |: range is 0..1  |: def=0.5
|:       |: `res` / `r`  |: range is 0..1  |: def=0.5
].table_inputs


## lut
] read from look-up table

Use previous output as normalized 0..1 index for curve look-up table (LUT).

The first argument is either the name of a curve, or the name of a plugin array param that is associated with a curve.

##-# Example
~~~
curve amp

<out:
  sin
  * rmp 0 0 1
    lut amp clamp=0 lin=0
~~~ lang=cycle
] cubic bezier curve lookup-table

##-# Inputs

|noheader
|: const |: `clamp` / `c`   |: must be either 0 (repeat lut) or 1 (clamp to last index) |: def=0(wrap-around)
|: const |: `lin` / `l`     |: must be either 0 (nearest neighbour) or 1 (linear interpolation) |: def=0(nearest)
|: const |: `scl` / `s`     |: scales the index by a float number (hifi mode) or clamps to the index before last (lofi mode, when scl != 1.0) (at compile time). This can be used to avoid (costly) index clamping in linear filtered mode by using wrap mode instead (`clamp=0`) and scaling the index by e.g. `scl=31/32` (when LUT / curve has 32 elements) to avoid filtering the last with the first element when the index approaches 1.0. |: def=1.0
|: const |: `bipolar` / `b` |: enable bipolar index mode (flip sign of index+output as required) |: def=0(unipolar)
|: const |: `in` / `i`      |: 1=scale index input (0..1) by 0..size-1 instead of 0..size  |: def=0(*0..size)
].table_inputs

note: the `lut` module is often combined with a [#rmp][] or [#pha][] module that generates the look-up index

note: when using a lut as a replacement table for e.g. oscillator phases (0..1), `in` should be set to 1


## mac
] update interpolated macro fields

The `mac` module updates the current, interpolated values of a macro's fields.

See [#macros][] for further information and examples.

##-# Inputs

|noheader
|:       |: `pos` / `p` |: range is 0..1 (last setting is lerped back to first setting)  |: def=0.0
].table_inputs


## map
] remap value

Remap previous output value using a non-contiguous table.

##-# Inputs

|noheader
|: const |: `mode` / `m` |: 0=`nearest` 1=`match` 2=`up` 3=`down` 4=`ipl`/`lin`  |: def=0 (nearest)
|:       |: `def`        |: when connected, emit default value when value could not be mapped
|:       |: `ipld`       |: when connected, distort interpolation `t` (when mode=`ipl`) (in=0..1, out=0..1)
].table_inputs

See [#Maps][] for further info and example code.


## mkv
] markov-chain element

A dual state, probability driven generator that is not just useful for weather forecasts but also for sound synthesis.

Two probabilities `ab` and `ba` determine the chances of transitioning from state a to b (`ab`) or from state b to a (`ba`).

See `markov_bd_1`, `markov_sd_1`, `markov_hh_1` example patches.

##-# Inputs

|noheader
|:       |: `a`   |: input A  |: def=0.0
|:       |: `b`   |: input B  |: def=1.0
|:       |: `r`   |: random input. use previous output value when not connected.
|:       |: `ab`  |: A =\> B probability (0..1)  |: def=0.5
|:       |: `ba`  |: B =\> A probability (0..1)  |: def=0.5
|:       |: `onb` |: evaluated when entering "b" state
|:       |: `ona` |: evaluated when entering "a" state
|:       |: `act` |: 1=always evaluate `a` and `b` input. 2=evaluate only during state transitions (initial state is floating) |: def=1 (always eval a + b)
].table_inputs


## mul
] multiply variable

Multiply variable

If the `factor`/`f` input is not connected, multiply variable by previous output value.

##-# Examples
~~~
var f
<out:
  set f 1.0
  rep 16
    mul f 0.5
    mul f factor=0.5
    mul f
      factor:
        0.5
~~~ lang=cycle
] multiply variable by input

~~~
var f
<out:
  set f 2.0
  0.5
  mul f
~~~ lang=cycle
] multiply variable by previous output

##-# Inputs

|noheader
|:       |: `factor` |: when connected, multiply by input instead of previous output value
].table_inputs


## neg
] negate previous output

Negate previous output

##-# Example
~~~
<out:
  saw
  neg
~~~ lang=cycle
] flip phase


## nos
] generate xor-shift white noise

Render white noise via xor-shift pseudo random number generator

##-# Example
~~~
<out:
  nos seed=0x44894489 cycle=0
~~~ lang=cycle
] xor-shift noise

##-# Inputs

|noheader
|: const |: `seed`  / `s`  |: is the random seed (a 32bit integer).               |: the default seed is 0x3d9fb971
|: const |: `cycle` / `cy` |: range is 0/1 (0=disable wavetable cycle reset)      |: def=1 (on)
|: const |: `reset` / `rs` |: range is 0/1 (0=disable plugin voice note on reset) |: def=1 (on)
].table_inputs


## not
] invert previous logic signal output

Invert previous logic signal output

##-# Examples
~~~
<out:
  1
  # 1 => 0
  not
  # 0 => 1
  not
~~~ lang=cycle
] invert previous output

~~~
var v_state

<out:
  # invert the boolean state of the given variable
  not v_state
~~~ lang=cycle
] invert boolean variable


## note
] return voice note number (plugins)

Returns the current voice note number (0..127).

##-# Example
~~~
id note_test_1
<out:
  sin
    freq:
      0.1
      + note
        * 1/32
~~~ lang=cycle
] note example

`note` can also be used in short form value assignments to (dynamic) inputs.

note: only available in voice plugin mode. in sample synth mode, this simply returns 60.0 (middle-C).


## nth
] pass every nth clock signal

Discards all clock signals except for every nth one.

##-# Examples
~~~
<out:
  clk div=1/4
  nth 4
~~~ lang=cycle
] pass every 4th 16th note clock signal (e.g. a base drum)

~~~
<out:
  clk div=1/4
  nth 8 shift=-4
~~~ lang=cycle
] pass every 8th 16th note clock signal, starting with the 4th one (e.g. a snare drum)

##-# Inputs

|noheader
|: const |: `every` / `e`    |: pass every nth pulse (const integer)  |: def=1
|: const |: `shift` / `s`    |: shift pulse counter (<0: delay first passed pulse)  |: def=0
|:       |: `restart` / `rs` |: reset pulse counter on rising edge
|:       |: `onclk` / `oc`   |: run statement block when duty cycle begins. must return pulse level scaling (0..1, 0 discards pulse)
].table_inputs


## pal
] pan left

Calc left channel pan factor

##-# Example
~~~
id pan_test
modparam vol 1
modparam pan 0.5

var v_vol_l  smooth=1
var v_vol_r  smooth=1

<prepare:
  $pan
  fma 2 -1
  vst vpan

  $vol
  . pre
    * pal $vpan
    sto v_vol_l
  * par $vpan
  sto v_vol_r

<out:
  sin
  wrl a=$v_vol_l
  wrr a=$v_vol_r

~~~ lang=cycle
] stereo-panned sine


##-# Inputs

|noheader
|:       |: `pos` / `p`       |: pan position (-1..1)  |: def=0(center)
].table_inputs

see also: [#wrl][]


## par
] pan right

Calc right channel pan factor

##-# Inputs

|noheader
|:       |: `pos` / `p`       |: pan position (-1..1)  |: def=0(center)
].table_inputs

see also: [#wrr][]


## pha
] phase accumulator

Generate phase output (0..1 normalized saw-wave).

##-# Example
~~~
curve mywaveshape

<out:
  pha freq=1.5 phase=0.25
  lut mywaveshape
~~~ lang=cycle
] phase table lookup (single-cycle bezier wave)

##-# Inputs

|noheader
|:       |: `freq` / `f`      |: range is 0.25..4 (1.0=middle C in fixed frequency mode)   |: def=1.0
|:       |: `phase` / `ph`    |: range is 0..1 (start phase)                               |: def=0.0
|:       |: `vsync` / `vs`    |: range is 0.01..4 (virtual oscillator sync)                |: def=1.0 (off)
|: const |: `cycle` / `cy`    |: range is 0/1 (0=disable wavetable cycle reset)            |: def=1 (on)
|: const |: `reset` / `rs`    |: range is 0/1 (0=disable plugin voice note on reset        |: def=1 (on)
|: const |: `fixed` / `fx`    |: range is 0..1 (1=enable fixed frequency mode)             |: def=0 (follow kbd)
|:       |: `phasemod` / `pm` |: range is 0..1 (post vsync phase modulation)               |: def=0.0 (off)
|:       |: `pd`              |: phase distortion insert (subtree). input is phase (0..1), output is new phase (0..1). only supported in float/hi-fi mode.
].table_inputs


## p2s
] exponential power-of-two scaling

Return `(pow(2, exp * prev_out) - 1.0) / (pow(2, exp) - 1.0)`.

Can be used for e.g. scaling filter cutoff frequencies (linear to exponential).

##-# Examples
~~~
arg cutoff

<out:
  svf
    freq:
      $cutoff
      p2s 7
~~~ lang=cycle
] (non-kbd-tracked) exponential cutoff scaling

When the `kbd` (and optionally the `off`) input is connected and Cycle is in STFX plugin-generation mode, the module input follows the keyboard.

~~~
id     cutoff_kbd_tracking_test_p2s
name   "cutoff kbd tracking test p2s"
author bsp
cat    filter

param p_cutoff     0.7   0 1
param p_keyfollow  1.0   0 1
param p_octave_off 0.0  -4 4

<out:
  saw
  svf sr=0
    freq:
      $p_cutoff
      p2s kbd=$p_keyfollow off=$p_octave_off
~~~ lang=cycle
] filter cutoff keyboard tracking example plugin

##-# Inputs

|noheader
|:       |: `exp` / `e` |: range is 0..15                             |: def=7.0
|: const |: `scl` / `s` |: range is 0..1 (0=\>scl=(pow(2,exp)-1))     |: def=0.0
|:       |: `kbd` / `k` |: range is -16..16 (keyboard follow amount)  |: def=0.0
|:       |: `off` / `o` |: range is -4..4 (octave offset)             |: def=0.0
].table_inputs

caution: when `kbd` is connected (in plugin mode), this factors in the current samplerate and oversample factor. caution when passing the output to another module which also does this !
caution: only supported in float-mode (fall back to float in int mode)

tip: for keyboard tracking purposes, the [#kbd][] module is usually the preferred solution


## pow
] raise previous output to the power of 'f'

Return previous output raised to the power of 2 or 3 (fast-path), or an arbitrary exponent (float-mode only).

##-# Examples
~~~
<out:
  sin
  pow 3
~~~ lang=cycle
] pow fastpath

~~~
<out:
  pow
    base:
      2.71828182845904523 # e
    exp:
      sin
~~~ lang=cycle
] generic pow with base input connected

##-# Inputs

|noheader
|:       |: `exp` / `e`  |: range is \[2, 3\] (int) or any float value. when not connected (and `base` is), use previous output as exponent     |: def=2.0
|:       |: `base` / `b` |: range is any float val. when not connected, use previous output as base.  |: def=2.0
].table_inputs

tip: useful for bending [#rmp][] envelopes into non-linear shapes

caution: constant exponents 2 and 3 behave differently than arbitrary (or dynamic) exponents, i.e. -2²=+4 while e.g. -2^4=-16
caution: arbitrary or dynamic exponents are only supported in float-mode (fall back to float in int mode)


## pre
] Return previous output

This is a *dummy* (no-op) module that simply returns the previous output (no extra code generated).

>>>notepre
`pre` _must_ be the first module in a template or module input that reads the previous output via an operator and a template call, e.g.
~~~
def myrev              
  + ~mydly
~~~ lang=cycle
] not working

..must be written as:
~~~
def myrev 
  pre     
  + ~mydly
~~~ lang=cycle
] working

This is currently required due to a parser-issue which may be proper-fixed later.
<<<
caution: $(buf!notepre)

see also: [Templates / pre](#pre-2)


## pul
] generate pulse wave

Generate pulse wave

##-# Example
~~~
<out:
  pul freq=1 phase=0 width=0.5 vsync=2.5
~~~ lang=cycle
] vsynced pulse (rectangle) oscillator

##-# Inputs

|noheader
|:       |: `freq` / `f`      |: range is 0.25..4 (1.0=middle C in fixed frequency mode)  |: def=1.0
|:       |: `phase` / `ph`    |: range is 0..1 (start phase)                              |: def=0.0
|:       |: `width` / `w`     |: range is 0..1 (length of duty cycle)                     |: def=0.5
|:       |: `vsync` / `vs`    |: range is 0.01..4 (virtual oscillator sync)               |: def=1.0 (off)
|: const |: `cycle` / `cy`    |: range is 0/1 (0=disable wavetable cycle reset)           |: def=1 (on)
|: const |: `reset` / `rs`    |: range is 0/1 (0=disable plugin voice note on reset)      |: def=1 (on)
|: const |: `fixed` / `fx`    |: range is 0/1 (1=enable fixed frequency mode)             |: def=0 (follow kbd)
|:       |: `window` / `win`  |: range is 0..8 (0=`off`/`none`(windowed vsync off), 1=`sin`(sine window), 2=`hs`(half-sine window), 3=`tri`(triangle window), 4=`hse1`(hse1_36), 5=`hse2`, 6=`hse3`, 7=`hse4`, 8=`hse5`) (must be const OR return amp factor (see below))
|:       |: `phasemod` / `pm` |: range is 0..1 (post vsync phase modulation)              |: def=0.0 (off)
|:       |: `pd`              |: phase distortion insert (subtree). input is phase (0..1), output is new phase (0..1). only supported in float/hi-fi mode.
].table_inputs

caution: windowed vsync is only supported in float-mode (ignored in integer mode)

note: when `window` input is _not_ constant, it is treated as an insert which receives the window phase and returns the window amplification factor (0..1). I.e. this can be used to implement custom window functions.


## qua
] quantize previous output

Quantize float value to n fractional steps per integer.

##-# Example
~~~
<out:
  sin
  # reduce 32bit -1..1 float to ~4.32 bits
  #   (.., -0.2, -0.1, 0.0, 0.1, 0.2, ..)
  qua num=10
~~~ lang=cycle
] quantize

##-# Inputs

|noheader
|:       |: `num` / `n` |: range is 1..n  |: def=32
].table_inputs

tip: useful for controllers, e.g. FM ratios

tip: when num=1, simply discard fractional bits (cast to integer)

caution: only supported in float-mode (fall back to float in int mode) (except when num=1)


## rbl
] read sample from (cross-zone) voicebus left channel

Read sample from (cross-zone) voicebus left channel.

##-# Examples

~~~
id vb_read_prev
param lvl 1 0 1
<out:
   sin
     phase:
       rbl 0
       * $lvl
~~~ lang=cycle
] phase-modulate sine wave by previous zone output

~~~
id vb_read_1
param lvl 1 0 1
<out:
   sin
     phase:
       rbl 1
       * $lvl
~~~ lang=cycle
] phase-modulate sine wave by voice bus 1

~~~
id vb_read_nri
# (note) useful range is 0..32%
param bus 0 0  100
param lvl 1 0    1
<out:
   sin
     phase:
       rbl nri=$bus
       * $lvl
~~~ lang=cycle
] phase-modulate sine wave by previous zone output (integer bus nr)

~~~
id vb_read_nrf
# (note) useful range is 0..32%
param bus 0 0 1
param lvl 1 0 1
<out:
   sin
     phase:
       rbl nrf=$bus
       * $lvl
~~~ lang=cycle
] phase-modulate sine wave by previous zone output (float bus nr)

##-# Inputs

|noheader
|:       |: `nri` |: range is 0..32 (bus 1..32, 0=previous zone nr)   |: def=1
|:       |: `nrf` |: range is 0..0.32 (bus 1..32, 0=previous zone nr) |: def=0.1
].table_inputs

caution: only supported in STFX plugin mode (returns 0 otherwise)


## rbr
] read sample from (cross-zone) voicebus right channel

Read sample from (cross-zone) voicebus right channel.

see [#rbl][]


## rcp
] reciprocal (1 / x)

Calculate reciprocal (`1/x`) of previous output.

##-# Example
~~~
arg p_sin_scl 1.5 1 4

<out:
  sin
  * $p_sin_scl
  rcp
  clp
  * tri ph=0.25 ## fade-out infinity
    abs
  * $p_sin_scl
~~~ lang=cycle
] test rcp 1 example patch

## rdl
] read left channel (plugin) input

Read left channel input sample.

caution: available only in plugin mode.


## rdr
] read right channel (plugin) input

Read right channel input sample.

caution: available only in plugin mode.


## red
] detect logic signal rising edge

Detect logic signal rising edge and evaluate sub-tree.

##-# Example
~~~~
id edge_detect

var v_out
var v_dcy

<out:
  pul
  red
    set v_dcy 0.999
    set v_out 1
  fed
    set v_dcy 0.95
  $v_out
  *sr $v_dcy
  sto v_out
  fma 2 -1
~~~~ lang=cycle
] rising / falling edge detect osc

note: this module has no inputs

see also: [#fed][]


## rep
] loop / unroll statement sequence

Loop / unroll statement sequence.

important: The number of iterations must be constant.

##-# Example
~~~
arg p_num 8 1 32
var i
<out:
  1.0
  sto i
  0.0
  rep $p_num
    + sin
        freq:
          $i
          * 0.5
      * 0.1
  clp
~~~ lang=cycle
] unrolled loop

##-# Inputs

|noheader
|: const |: `num` / `n` |: range is 0..1023  |: def=1
].table_inputs



## rev
] --reserved--

**reserved**


## rmp
] generate linear ramp

Generate linear ramp

##-# Examples
~~~
<out:
  rmp millisec=8 start=0 end=1 cycle=1
~~~ lang=cycle
] linear 8ms ramp

~~~
<out:
  rmp 0 0 1
~~~ lang=cycle
] linear ramp from 0..1 that spans entire sample

##-# Inputs

|noheader
|: const |: `millisec` / `ms` |: range is 0..4000. 0 auto-adjusts the duration to match the target zone sample length  |: def=200
|: const |: `start` / `s`     |: range is 0..15 (start value)  |: def=1.0
|: const |: `end` / `e`       |: range is 0..15 (end value)    |: def=0.0
|: const |: `cycle` / `cy`    |: determines whether the ramp position is reset at each new wavetable cycle (0 or 1) |: def=1 (on)
].table_inputs





## rms
] root mean square

Calc root mean square (RMS).

##-# Example
~~~
<out:
  sin
  rms
  trc
~~~ lang=cycle
] rms

##-# Inputs

|noheader
|: const |: `size` / `s`   |: range is 1..4096 (history size)   |: def=16
|: const |: `win` / `w`    |: 1=enable window mode              |: def=0(off)
|: const |: `exp` / `e`    |: window exponent (<=1: disable)    |: def=0
].table_inputs


## saw
] generate sawtooth wave

Generate sawtooth wave

##-# Example
~~~
<out:
  saw
~~~ lang=cycle
] sawtooth oscillator

~~~
oversample 2
<out:
  saw freq=2 phase=0 vsync=2.5
~~~ lang=cycle
] vsynced sawtooth oscillator (second harmonic)

##-# Inputs

|noheader
|:       |: `freq` / `f`      |: range is 0.25..4 (1.0=middle C in fixed frequency mode)  |: def=1.0
|:       |: `phase` / `ph`    |: range is 0..1 (start phase)                              |: def=0.0
|:       |: `vsync` / `vs`    |: range is 0.01..4 (virtual oscillator sync)               |: def=0.0 (off)
|:       |: `cycle` / `cy`    |: range is 0/1 (0=disable wavetable cycle reset)           |: def=1 (on)
|: const |: `reset` / `rs`    |: range is 0/1 (0=disable plugin voice note on reset)      |: def=1 (on)
|: const |: `fixed` / `fx`    |: range is 0/1 (1=enable fixed frequency mode)             |: def=0 (follow kbd)
|:       |: `window` / `win`  |: range is 0..8 (0=`off`/`none`(windowed vsync off), 1=`sin`(sine window), 2=`hs`(half-sine window), 3=`tri`(triangle window), 4=`hse1`(hse1_36), 5=`hse2`, 6=`hse3`, 7=`hse4`, 8=`hse5`) (must be const OR return amp factor (see below))
|:       |: `phasemod` / `pm` |: range is 0..1 (post vsync phase modulation)              |: def=0.0 (off)
|:       |: `pd`              |: phase distortion insert (subtree). input is phase (0..1), output is new phase (0..1). only supported in float/hi-fi mode.
].table_inputs

caution: windowed vsync is only supported in float-mode (ignored in integer mode)

note: when `window` input is _not_ constant, it is treated as an insert which receives the window phase and returns the window amplification factor (0..1). I.e. this can be used to implement custom window functions.


## sat
] saturate previous output

Saturate previous output.

Processes previous output value through tangens hyperbolicus (tanh) function.

##-# Example

~~~
arg drive 0 -1 1

var v_amp

<prepare:
  # amp=/10..*10
  10
  pow $drive
  sto v_amp

<out:
  sin
  * $v_amp
  sat  # same as tanh
~~~ lang=cycle
] tanh saturation

~~~
arg drive 0 0 2

lut tanh shared 4096

<lut_tanh:
  # 0..1 => -100..+100
  fma 200 -100
  sat  # same as tanh

var v_amp

<prepare:
  # amp=*1..*100
  10
  pow $drive
  sto v_amp

<out:
  sin
  * $v_amp
  # -100..+100 => 0..1
  fma 1/200 0.5
  lut tanh lin=1
~~~ lang=cycle
] tanh saturation with lookup-table

See [#tanh][] (deprecated name in earlier releases)

note: useful for saturation-like non-linear distortion (converges to -1..1)

caution: only supported in float-mode (fall back to float in int mode)


## set
] set variable

Set variable

If the `value`/`v` input is not connected, set variable to previous output value.

##-# Examples
~~~
var f
<out:
  1.23
  set f
~~~ lang=cycle
] set f to 1.23

~~~
var f
<out:
  set f 1.23
  set f value=1.23
  rep 16
    inc f 0.1
~~~ lang=cycle
] increment f 16 times


##-# Inputs

|noheader
|:       |: `value` / `v` |: when connected, set variable to input value instead of previous output value
].table_inputs


## sin
] generate sine wave

Generate sine wave

##-# Examples
~~~
<out:
  sin
~~~ lang=cycle
] sine oscillator

~~~
<out:
  sin freq=1 phase=0 vsync=2.5 window=sin
~~~ lang=cycle
] windowed / vsynced sine oscillator

~~~
<out:
  sin
    pd:
      tri freq=1
~~~ lang=cycle
] sine osc phase distortion

##-# Inputs

|noheader
|:       |: `freq` / `f`     |: range is 0.25..4 (1.0=middle C in fixed frequency mode)   |: def=1.0
|:       |: `phase` / `ph`   |: range is 0..1 (start phase)                               |: def=0.0
|:       |: `vsync` / `vs`   |: range is 0.01..4 (virtual oscillator sync)                |: def=1.0 (off)
|: const |: `cycle` / `cy`   |: range is 0/1 (0=disable wavetable cycle reset)            |: def=1 (on)
|: const |: `reset` / `rs`   |: range is 0/1 (0=disable plugin voice note on reset)       |: def=1 (on)
|: const |: `fixed` / `fx`   |: range is 0/1 (1=enable fixed frequency mode)              |: def=0 (follow kbd)
|:       |: `window` / `win` |: range is 0..8 (0=`off`/`none`(windowed vsync off), 1=`sin`(sine window), 2=`hs`(half-sine window), 3=`tri`(triangle window), 4=`hse1`(hse1_36), 5=`hse2`, 6=`hse3`, 7=`hse4`, 8=`hse5`) (must be const OR return amp factor (see below))
|:       |: `phasemod`/`pm`  |: range is 0..1 (post vsync phase modulation)               |: def=0.0 (off)
|:       |: `pd`             |: phase distortion insert (subtree). input is phase (0..1), output is new phase (0..1). only supported in float/hi-fi mode.
].table_inputs

caution: windowed vsync is only supported in float-mode (ignored in integer mode)

note: when `window` input is _not_ constant, it is treated as an insert which receives the window phase and returns the window amplification factor (0..1). I.e. this can be used to implement custom window functions.

tip: all oscillators can be used as wavefolders by setting their speed to 0, then modulating their phase input


## slf
] read past sample output

Read past sample output ("self" feedback)

##-# Example
~~~
<out:
  pul
  + slf rate=0.98
  clp
~~~ lang=cycle
] mix past output

##-# Inputs

|noheader
|:       |: `rate` / `r` |: range is 0..1   |: def=0.985
].table_inputs


## slw
] slew

Slew from `a` to `b` with configurable up / down amounts.

The module behaves differently depending on which inputs are connected:
  - when `a` is connected and `b` is not, the previous output is used as the destination
  - when `b` is connected and `a` is not, the previous output is used as the source
  - when neither `a` and `b` are connected, the previous output is used as the destination and an implicit (source) state variable is allocated
     (initially 0, then continously updated to the `slw` output value)

##-# Example
~~~
arg amp    0.7
arg slw_up 0.15
arg slw_dn 0.11

<out:
  pul
  slw up=$slw_up down=$slw_dn
  * rmp 0 1
  * $amp
  clp
~~~ lang=cycle
] cycle int slw example patch

##-# Inputs

|noheader
|:       |: `up` / `u`   |: (b \> a) slew amount (1=instant .. 0=never)   |: def=0.3
|:       |: `down` / `d` |: (b \< a) slew amount (1=instant .. 0=never)   |: def=0.1
|:       |: `a`          |: source value (optional)
|:       |: `b`          |: destination value (optional)
|: const |: `sr`         |: 1=(over)samplerate-independent slew amounts. 0=fixed slew amounts. (float/hi-fi mode only)  |: def=1
].table_inputs


## spd
] return voice note speed (plugins)

Returns current note speed (including oversampling factor).
Can be used to accumulate normalized (0..1) phases.

##-# Example
~~~
id spd_test

var v_phase

<out:
  $v_phase
  + spd
  sto v_phase
  # wrap-around (keep only the fractional bits)
  frc
  # 0..1 => -1..1
  fma 2 -1
~~~ lang=cycle
] a simple saw oscillator (pitch follows keyboard)

note: in sample calculator (instead of voice plugin) mode, this simply returns `(1.0 / oversampling_factor)`


## spr
] dampened spring

Apply dampened spring oscillation to previous output value.

##-# Example
~~~
id spring_time_1
<out:
  pul
  * 0.5
  spr spd=0.1 dcy=0.8
  clp
~~~ lang=cycle
] dampened spring example plugin

##-# Inputs

|noheader
|:       |: `spd` / `s` |: speed (0..1)  |: def=0.1
|:       |: `dcy` / `d` |: decay (0..1)  |: def=0.8
|:       |: `sr`        |: 1=(over-)samplerate independency (float/hi-fi mode only) |: def=1 (on)
].table_inputs


## ssl
] sine slice oscillator

Sine slice oscillator.

This is a rather experimental oscillator that mainly generates "chiptune"-ish sounds.
It works similar to how the additional 7 waveforms on the Yamaha TX81Z were created back in the 1980ies.

The (built-in) sine table as well as the destination waveform(-cycle) is divided into 8 regions / slices.
Each target slice is assigned one of the 8 sine regions (0..7) via the slice sequence `seq` (first parameter).

The `zeromask` input determines which slices are silenced while `modmask` selects which ones are affected by the modulation `mode` and modulation amount `mod`.

`seqmod` is multiplied by 16384, converted to a 16bit integer, replicated to 32 bit and added to the base sequence (where each nibble (4 bits) represents a slice, e.g. `$01234567`). In other words: it (dynamically) shifts the sequence's slice selection with wildy varying results.

##-# Example
~~~
<out:
  ssl 01234567 freq=1.0 phase=0.0 mode=4 mod=0 seq=0 seqmod=0 modmask=1.0 zeromask=0.0
~~~ lang=cycle

The first argument is the base slice sequence (0=region 0, 7=region 7).
The sequence is repeated if it has less than 8 entries.

##-# Inputs

>>>ssl_mode
|:Mode            :|: Description
| 0 point         :| ignore fractional (mod) bits, simply recombine waveforms (fastest)
| 1 phase          | fractional (mod) bits determine source region start offset shift ("phase" mod 1/8 sin)
| 2 lerp           | fractional (mod) bits are used to lerp sine source region to cosine wave
| 3 shift          | fractional (mod) bits are used to shift source region (dc offset + reflect/fold)
| 4 ang            | fractional (mod) bits determine source waveform start offset shift ("phase" +- 2PI)
| 5 amp            | fractional (mod) bits are used to amplify source region
] SSL mode settings
<<<

|noheader
|:       |: `freq` / `f`      |: range is 0.25..4 (1.0=middle C in fixed frequency mode)   |: def=1.0
|:       |: `phase` / `ph`    |: range is 0..1                                             |: def=0.0
|:       |: `mod`             |: range is -1..1 (slice modulation)                         |: def=0.0
|: const |: `seq` / `s`       |: range is 0..1 (0=base slice sequence, 00000001..7777777)
|:       |: `modmask` / `mm`  |: range is 0..1 (0=no slices, 1=all slices)                 |: def=1.0 (all)
|:       |: `zeromask` / `zm` |: range is 0..1 (0=no slices cleared, 1=all slices cleared) |: def=0.0 (none)
|: const |: `mode` / `m`      |: range is 0..5 : $(buf!ssl_mode)                           |: def=0 (point)
|:       |: `seqmod` / `sm`   |: range is 0..3.9999 (per-sample sequence modulation)       |: def=0 (no modulation)
].table_inputs


## sta
] store variable in LUT

Store variable in LUT at normalized index (0..1 => 0..lut_size) determined by previous output value.

Syntax: `sta \<lut\_id\> \[\<index\_mul\> \<index\_add\>\] \<src\_var\> \[\<var\_mul\> \[\<var\_add\>\]\]`

##-# Example
~~~~~
lut ws shared 4096
lut ws_inv shared 4096 inv

<lut_ws:
  # transform implicit input x={0..1} => {-1..1}
  fma 2 -1
  pow 3
  # store in inverse LUT 'ws_inv'
  #  - transform 'pow' result (-1..1) into normalized index (0..1)
  #  - transform x from 0..1 to -1..1 and store in LUT
  sta ws_inv 0.5 0.5 x 2 -1
  # store 'pow' result in LUT 'ws' (done by auto-generated code)
  #  (note) holes in the ws_inv LUT will be auto-filled afterwards

<out:
   sin
   # -1..1 => 0..1
   fma 0.5 0.5
   lut ws
   lpf freq=0.3 res=0.5
   clp
   # -1..1 => 0..1
   fma 0.5 0.5
   lut ws_inv
~~~~~ lang=cycle
] inverse exponential lookup table


~~~~~
curve ws
lut ws_inv shared 2048 inv

<lut_ws_inv:
  # use implicit input x (0..1) as index into bezier curve 'ws'
  lut ws
  clp
  # store in inverse LUT 'ws_inv'
  #  - transform 'clp' result (-1..1) into normalized index (0..1)
  #  - transform x from 0..1 to -1..1 and store in LUT
  sta ws_inv 0.5 0.5 x 2 -1
  # don't write output value to lut (already written)
  end

<out:
   sin
   # -1..1 => 0..1
   fma 0.5 0.5
   lut ws
   lpf freq=0.3 res=0.5
   clp
   # -1..1 => 0..1
   fma 0.5 0.5
   lut ws_inv
~~~~~ lang=cycle
] inverse bezier curve lookup table

note: referencing an inverse LUT in another LUT's init lane via [#sta][] causes the inverse LUT to be filled before the init lane is executed, and potential holes (unwritten elements) in the inverse LUT to be closed afterwards


## sto
] store previous output in variable

Store previous output in variable

##-# Examples
~~~
var t

<out:
  sin
  sto t
  * $t
  * $t
~~~ lang=cycle
] sin*sin oscillator

The variable is initialized with 0 but retains its content over multiple sample frame iterations.
~~~
var sin_val

<out:
  $sin_val
  + sin
  sto sin_val
  clp
~~~ lang=cycle
] accumulate sine


## svf
] state-variable filter (lpf, hpf, bpf, notch)

Process previous output with two-pole state-variable filter.

##-# Example
~~~
<out:
  saw
  svf freq=0.4 res=0.9 mode=lpf
  svf 0.4 0.5 0
~~~ lang=cycle
] resonant four-pole (-24dB/oct) low-pass filter

##-# Inputs

|noheader
|:       |: `freq` / `f`     |: range is 0..1  |: def=0.5
|:       |: `res` / `r`      |: range is 0(full res)..1(no res) |: def=1.0 (no resonance)
|: const |: `mode` / `m`     |: 0=`lpf`, 1=`hpf`, 2=`bpf`, 3=`notch`  |: def=0=lpf
|:       |: `postlpf` / `pl` |: post-LPF / pre-HPF insert
|:       |: `posthpf` / `ph` |: post-HPF / pre-BPF insert
|:       |: `postbpf` / `pb` |: post-BPF / pre-LPF insert
|: const |: `sr`             |: 1=enable (over-)samplerate independent frequency (voice-plugins)  |: def=1 (on)
].table_inputs


## tan
] tangens

Calculate the tangens of the previous output value.

tip: tan(0.125*2PI) = tan(0.785398163398) = 1.0, i.e. scale normalized -1..1 input by 0.78539 to create a tan waveshaper (see `test tan 1` example patch)

caution: forces fallback to hifi / float mode



## tanh
] saturate previous output

note: this is an alias for [#sat][]


## tmp
] instantiate template

This is the (deprecated) long-form of the `~` template module instantiation command.

see [#templates][]


## trc
] debug-trace last output value

Debug-trace last output value (print to console).

##-# Example
~~~
lut mylut shared 16

<lut_mylut:
  # print current $x
  trc

<out:
  sin
~~~ lang=cycle
] debug-print normalized LUT index

##-# Inputs

|noheader
|: const |: `c` |: 1=trace value changes only, 0=trace all values  |: def=1 (on)
|: const |: `z` |: 1=trace 0 values, 0=skip                        |: def=1 (on)
|: const |: `e` |: >1=trace every nth value, 0=trace all           |: def=0 (all)
].table_inputs


## tri
] generate triangle wave

Generate triangle wave

##-# Examples
~~~
<out:
  tri
~~~ lang=cycle
] triangle oscillator

~~~
<out:
  tri freq=1 phase=0 vsync=2.5
~~~ lang=cycle
] vsynced triangle oscillator

##-# Inputs

|noheader
|:       |: `freq` / `f`      |: range is 0.25..4 (1.0=middle C in fixed frequency mode)  |: def=1.0
|:       |: `phase` / `ph`    |: range is 0..1 (start phase)                              |: def=0.0
|:       |: `vsync` / `vs`    |: range is 0.01..4 (virtual oscillator sync)               |: def=1.0 (off)
|: const |: `cycle` / `cy`    |: range is 0/1 (0=disable wavetable cycle reset)           |: def=1 (on)
|: const |: `reset` / `rs`    |: range is 0/1 (0=disable plugin voice note on reset)      |: def=1 (on)
|: const |: `fixed` / `fx`    |: range is 0/1 (1=enable fixed frequency mode)             |: def=0 (follow kbd)
|:       |: `window` / `win`  |: range is 0..8 (0=`off`/`none`(windowed vsync off), 1=`sin`(sine window), 2=`hs`(half-sine window), 3=`tri`(triangle window), 4=`hse1`(hse1_36), 5=`hse2`, 6=`hse3`, 7=`hse4`, 8=`hse5`) (must be const OR return amp factor (see below))
|:       |: `phasemod` / `pm` |: range is 0..1 (post vsync phase modulation)              |: def=0.0 (off)
|:       |: `pd`              |: phase distortion insert (subtree). input is phase (0..1), output is new phase (0..1). only supported in float/hi-fi mode.
].table_inputs

caution: windowed vsync is only supported in float-mode (ignored in integer mode)

note: when `window` input is _not_ constant, it is treated as an insert which receives the window phase and returns the window amplification factor (0..1). I.e. this can be used to implement custom window functions.


## tsq
] generate trigger sequence

Generate trigger sequence with configurable duty cycle duration.

##-# Example
~~~
var v_clk
<out:
  clk div=1/4
  sto v_clk
  sin
  * tsq x.x..x...x....x.....x......x.......x cg=$v_clk
    adsr
~~~ lang=cycle
] envelope restarted by trigger pattern

~~~
var v_clk
<out:
  clk div=1/4
  sto v_clk
  sin
  * tsq x.x..x...x....x.....x......x.......x cg=$v_clk
    adsr
~~~ lang=cycle
] envelope restarted by trigger pattern

~~~~~
id arrayparam_seq_test_1

param p_seq

curve 0 steps param=p_seq size=16 var=4

var v_env_lvl

<out:
  clk div=1/4
  # (note) 'x' is a dummy pattern
  tsq x len=16 stepth=0.5
    step:
      # input is step index (0..15)
      * 1/16
      lut steps
    ontrig:
      set v_env_lvl 1

  tri
  * $v_env_lvl
    *sr 0.9993
    sto v_env_lvl
    slw 0.05 0.05
~~~~~ lang=cycle
] read trigger pattern from array param


##-# Inputs

|noheader
|:       |: `dur` / `d`      |: range is 0..f (trigger pulse duty cycle duration in milliseconds)  |: def=5
|:       |: `restart` / `rs` |: range is 0/1 (restart sequence on rising edge)
|:       |: `shift` / `sh`   |: range is 0..1 (shift play position by normalized position (0..1 => 0..pat_len))  |: def=0
|:       |: `gate` / `g`     |: range is 0/1 (play current step on rising edge)
|:       |: `clk` / `c`      |: range is 0/1 (advance to next step on rising edge)
|:       |: `cg`             |: range is 0/1 (clock+gate, play current step and advance to next step on rising edge)
|: const |: `loop` / `lp`    |: range is 0/1 (1=repeat pattern, 0=oneshot/stop at end of pattern)  |: def=1 (on)
|: const |: `len` / `l`      |: sequence length (1..32(float), 1..15(integer/lo-fi)). default=0=use xx.xx pattern length. |: def=0 (use seq len)
|:       |: `step` / `s`     |: when connected, eval subtree when value is needed for step 'x'. input is step index (0..n), output is step value.
|: const |: `stepth` / `sth` |: threshold for `step` output (value=1 when output is `> stepth`, 0 otherwise).  |: def=0.0
|:       |: `ontrig` / `ot`  |: evaluated each time a step is triggered/gated. must return new pulse level (0..1, 0 discards pulse)
|:       |: `onend` / `oe`   |: evaluated when end of pattern (or first step in loop mode) is reached during clock tick
].table_inputs

important: when neither `clk`, `gate` or `cg` are connected, the clock+gate input is read from the previous output value


## vel
] return voice velocity (plugins)

Returns the current voice velocity (0..1).

##-# Examples
~~~
id vel_test_1
<out:
  sin
  * vel  # linear velocity curve
~~~ lang=cycle
] velocity example 1

`vel` can also be used in short form value assignments to (dynamic) inputs:
~~~
id vel_test_2
<out:
  sin
  * ipl a=0.1 b=0.9 t=vel win=smoothstep  # non-linear, biased velocity curve
~~~ lang=cycle
] velocity example 2

note: only available in voice plugin mode. in sample synth mode, this simply returns 1.0.


## vpl
] invoke voice plugin

Process previous output via [stfx](https://github.com/bsp2/stfx/) voice plugin.

##-# Examples
~~~
<out:
  sin 5.0
  vpl "bsp ws quintic" "Dry / Wet"=0.5 Drive=0.39
~~~ lang=cycle
] single line voice plugin invocation

~~~
arg drive 0.39 min=0 max=1

<out:
  sin 5.0
  vpl "bsp ws quintic" "Dry / Wet"=0.5 "Drive"=$drive
~~~ lang=cycle
] single line voice plugin invocation with arg slider

~~~
arg drive 0.39 min=0 max=1

<out:
  sin 5.0
  vpl "bsp ws quintic"
    "Dry / Wet" = 1
    "X^0"       = 0
    "X^1"       = 0.5
    "X^2"       = 0.74
    "X^3"       = 0.8
    "X^4"       = 0.62
    "X^5"       = 0.5
    "Drive"     = $drive
~~~ lang=cycle
] multi line voice plugin invocation with arg slider

In Lo-Fi (integer) mode, the input/output values are temporarily converted to / from float.


note: See Eureka sampler "FX" tab for a list of available plugins.

tip: Plugin setups can be exported to single- or multi-line Cycle scripts (clipboard) via the Eureka sampler "FX" tab context menu

caution: `vpl` modules cannot be exported to "C" source


## vsq
] generate value sequence

Generate value sequence.

##-# Example
~~~
arg p_amp 1 0 8

map myseq
     freq
  0  C-5/C-5
  1  D#5/C-5
  2  D-6/C-5
  3  C-5/C-5
  4  G-5/C-5
  5  A#5/C-5
  6  F-4/C-5
  7  A#4/C-5

<out:
  # clock
  clk div=1/4 bpm=0
  vst v_clk

  # pitch seq
  vsq xxxxxxxx cg=$v_clk lp=0
    s:
      map myseq.freq def=1 mode=lin
  vst v_freq

  # osc
  sin f=$v_freq
  * 0.9
  + sin ph=0.7
      f:
        $v_freq
        * 1.51
    * 0.25
  + sin ph=0.2
      f:
        $v_freq
        * 3.01
    * 0.01

  # amp env seq
  * tsq xxxxxxxx cg=$v_clk lp=0 dur=30
      adsr a=0.4 as=-3.5 d=0.8 s=0.2 r=1.5 rs=1.354
      slw 0.02 0.7

  # output amp    
  * $p_amp
  clp
~~~ lang=cycle
] seq test 1 example patch

note: use `step`/`s` input to return sequence value for given sequence step index (initial value when evaluating `step` input)

See [#tsq][] module



## vst
] declare variable and store previous output
Declare variable and store previous output.

important: the variable will be declared within the scope of the current lane

##-# Example
~~~
<out:
  tri phase=0.25
  vst tri
  + $tri
    hld 0.7
  * 0.5
~~~ lang=cycle
] sample&hold triangle osc mix


## wbl
] write sample to (cross-zone) voicebus left channel

Write sample to (cross-zone) voicebus left channel.

##-# Examples
~~~
id vb_out_test
param lvl 1 0 1
<out:
  sin
  wbl 1
~~~ lang=cycle
] write sine output to voice bus 1

##-# Inputs

|noheader
|:       |: `nri` |: range is 1..32 (bus 1..32)      |: def=1
|:       |: `nrf` |: range is 0.1..0.32 (bus 1..32)  |: def=0.1
].table_inputs

caution: only supported in STFX plugin mode


## wbr
] write sample to (cross-zone) voicebus right channel

Write sample to (cross-zone) voicebus right channel.

see [#wbl][]


## wrl
] write left channel (plugin) output

Write left channel output sample.

##-# Inputs

|noheader
|:       |: `value` / `v` |: when connected, read sample from input instead of previous output value
|:       |: `amp` / `a`   |: when connected, amplify value (e.g. by pan factor)
].table_inputs

caution: available only in plugin mode.

note: implicitely enables STEREO plugin mode

see also: [#pal][]


## wrp
] wrap previous output

Wrap previous output value at ceiling.

##-# Examples
~~~
<out:
  sin
  * 2.0
  wrp
~~~ lang=cycle
] wrap into -1..1 range

~~~
<out:
  sin
  * 2.0
  wrp ceil=0.8
~~~ lang=cycle
] wrap into -0.8..0.8 range
note: `ceil` sets min=-ceil, max=+ceil

~~~
<out:
  sin
  * 2.0
  wrp min=-0.5 max=0.9
~~~ lang=cycle
] wrap into -0.5..0.9 range
note: `min` / `max` override default ceil

~~~
oversample 4

arg amp_pre 0.99 0 8
arg wrp_min -1 -1 1
arg wrp_max  1 -1 1
arg p_amp 0.7 0 8

<out:
  sin
  * $amp_pre
  wrp min=$wrp_min max=$wrp_max
  * rmp 0 1
  * $p_amp
  clp
~~~ lang=cycle
] test wrp 1 example

##-# Inputs

|noheader
|:       |: `ceil` / `c` |: sets min=-val, max=+val   |: def=1.0
|:       |: `min` / `m`  |: minimum value (override -ceil)
|:       |: `max` / `x`  |: maximum value (override ceil)
|: const |: `hq`         |: 1=enable high-quality (float) mode (arbitary number of wrap-arounds).  |: def=1
].table_inputs

see also: [#fld][]



## wrr
] write right channel (plugin) output

Write right channel output sample.

##-# Inputs

|noheader
|:       |: `value` / `v` |: when connected, read sample from input instead of previous output value
|:       |: `amp` / `a`   |: when connected, amplify value (e.g. by pan factor)
].table_inputs

caution: available only in plugin mode.

note: implicitely enables STEREO plugin mode

see also: [#par][]


## xfd
] crossfade

Crossfade between previous output and input `b`. At the center position, the two inputs are summed.

##-# Example
~~~
arg mod_waveform 0 -1 1
<out:
  tri
  xfd amt=$mod_waveform
    b:
      saw
~~~ lang=cycle
] crossfade between triangle and saw

##-# Inputs

|noheader
|:       |: `amount` / `amt` |: crossfade-to-b amount (-1=previous input .. 0=previous input and b .. 1=b)
].table_inputs

See also: [#ipl][]


## zlp
] read looped sample frame from other zone

Read looped sample frame from other zone. Can be used to mix samples or create chords.

##-# Example
~~~
zone 0 otherzone
zone 1 othersmp.otherzone

<out:
  zlp 0 rate=0.3 phase=0.5
  + zlp 1 0.9
  clp
~~~ lang=cycle
] mix looped sample zones

##-# Inputs

|noheader
|:       |: `rate` / `r`   |: range is 0..15   |: def=1.0
|:       |: `phase` / `ph` |: range is 0..1    |: def=0.0
|: const |: `div` / `d`    |: range is 0..256 (set repeatlen to zonelen / div)
|:       |: `sub` / `s`    |: range is 0..256 (shifts start offset by sub*repeatplen, e.g. for selecting sub-wave from wavetable. requires `div` to be connected)
|: const |: `lin` / `l`    |: enables bilinear filtering   |: def=0 (off)
].table_inputs

note: sample len is determined by last loop length of the specified zone
note: sample len should be a power of two (read offset is ANDed with (sampleLen-1))


## zon
] read sample frame from other zone

Read sample frame from other zone. Can be used to mix samples or create chords.

##-# Examples
~~~
zone 0 otherzone
zone 1 othersmp.otherzone

<out:
  zon 0 rate=0.3
  + zon 1 0.9
  clp
~~~ lang=cycle
] Mix two zones at different rates

~~~
zone 0 loopzone

<out:
  # 'and=255' causes zone frames 0..255 to be repeated
  zon 0 rate=0.5 lin=1 and=255
~~~ lang=cycle
] Upsampled loop

~~~
zone 0 singlenote_g3

<out:
  zon 0
  + zon 0 d#3/g-3
  + zon 0 c-3/g-3
  * 0.5
  clp
~~~ lang=cycle
] Minor chord

##-# Inputs

|noheader
|: const |: `rate` / `r`     |: range is 0..15                                                |: def=1.0
|: const |: `lin` / `l`      |: enables bilinear filtering                                    |: def=0 (nearest)
|: const |: `and` / `a`      |: enables power-of-two wrap-around (0=off, 1..65535 otherwise)  |: def=0 (off)
|:       |: `andf` / `af`    |: logic-and read position with float converted to fixpoint      |: def=0.0 (off)
|:       |: `restart` / `rs` |: rising edge restarts sample
|:       |: `pos` / `p`      |: normalized restart position (0..1)                            |: def=0.0
|:       |: `freq` / `f`     |: range is 0..15 (effective_rate = `rate * current_note_speed * freq`)
|: const |: `clamp` / `c`    |: clamp source offset (always in float mode, def=1 in int mode) |: def=1 (on)
].table_inputs

tip: in order to create chords, synthesize the highest note to a zone, then in a following zone read and mix the previously rendered zone at different speeds / rates

See also: [#zlp][] (supports arbitrary loops)


## zsq
] sequence zone sample

Sequence (other) zone sample.

Can be used to create (multi-channel) drum loops.

##-# Example
~~~
zone 0 bd
zone 1 sd

<out:
  #       0   1   2   3   4   5   6   7
  = zsq 0 xx......x....x.. note=16 bpm=125 rate=1
  + zsq 1 ....x.......x... bpm=125 rate=1.1 lin=1
  clp
~~~ lang=cycle
] Kick / snare drum-sample sequence

##-# Inputs

|noheader
|: const |: `note`  |: range is 1..256 (step note duration)               |: def=16
|: const |: `bpm`   |: range is 0..1000 (<10: use project bpm)            |: def=0.0
|: const |: `swing` |: range is -0.5..0.5                                 |: def=0.0
|: const |: `rate`  |: range is 0.125..8                                  |: def=1.0
|: const |: `lin`   |: range is 0..1 (>=0.5: enable bilinear filtering)   |: def=0/nearest
|: const |: `div`   |: range is 0..256 (divide source zone into n regions)
|:       |: `off`   |: range is 0..1 (select sub-divided source region, e.g. `$wt_x`)
].table_inputs

tip: When the input zones (one-shot samples) are placed in the same samplebank as the sequenced loop, ticking the `Recalc All` checkbox (next to the `F11: Synth` button at the bottom of the screen) will cause all procedural zones to be recalculated when any synth patch changes. This makes it possible to tweak the input sounds and immediately hear the edits in the context of the loop.

tip: `rate` can (like any constant value) also be expressed as a note ratio, e.g. `c-3/d#3`


# Curve Lookup Tables (LUTs)

Cycle supports up to 64 cubic bezier curve tables (16 global + 48 local) which can be used as amp or filter envelopes, single-cycle waveforms, or sample waveshapers.

Curve tables are internally converted (sampled) to short, power-of-two sized arrays. The array sizes are configured in the UI.

Before a curve table can be used with a [#lut][] module, it must first be declared with a `curve` statement:
~~~
curve 0 amp

<out:
  sin
  * rmp 0 0 0.95   # millisec=0 (auto-fit sample size) start=0 end=0.95
    lut
~~~ lang=cycle
] Using a curve LUT as an amp envelope (no interpolation)

~~~
curve 0 amp

<out:
  sin
  * rmp 0 0 1   # millisec=0 (auto-fit sample size) start=0 end=1
    lut amp lin=1 clamp=1
~~~ lang=cycle
] Amp envelope with linear interpolation (clamp to end of curve)

## Auto enumeration

The curve index (first arg after `curve` keyword) may be skipped or set to -1. This will cause the curve to be auto-assigned to slot 0..15.

note: The curve index allocator attempts to retain the previous curve assignment based on the curve's alias / id.

caution: When removing an auto-enumerated curve declaration from a script (by deleting it or commenting it out), the curve data will most likely be lost since the curve index optimizer will reuse the curve when remapping other auto-enumerated curves

~~~~
arg p_amp     0.7 0 8
arg p_feg_amt 0.2 0 4

curve freq
curve amp

<out:
  sin
    freq:
      rmp 0 0 1 
      lut freq
      pow 2
      fma $p_feg_amt 1
  * rmp 0 0 1
    lut amp
  * $p_amp
  clp
~~~~ lang=cycle
] curve auto enum test 1


## Lcurves

In addition to the 16 *global* curves, another 48 *local* cubic bezier curves may be defined per patch.

While this is primarily intended for auto-generated patches, an `lcurve` can always be used in place of a (global) `curve`.

The main difference is that local curves will not be listed in the UI.

To edit them, press `TAB` (or RMB-click) on the `lcurve` declaration or `lut` module invocation line in the source code, or, in case of lcurves within templates, the template instantiation line (if the template declares more than one `lcurve`, a multiple-selection popup will be shown).


##-# Examples
~~~
inc lut4

arg p_amp 0.7 0 8

curve user_curve  #unused
lcurve wav
lcurve mylcurve

<out:
  pha
  ~lut4 wav
  lut mylcurve lin=1 b=1 c=1
  * $p_amp
  clp
~~~ lang=cycle
] lcurve test 1 example patch


~~~
arg p_amp_1 0.7 0 2
arg p_amp_2 0.2 0 2
arg p_rndamt_1 0.11 0 1
arg p_rndamt_2 0.23e 0 1
arg p_rndslw_1 0.5 0 1
arg p_rndslw_2 0.3 0 1

def myosc freq=1 seed=0x5fa7 rndamt=0.01 rndslw=0.5
  lcurve wav%%
  lcurve test%%

  calc:
    pha
      freq:
        %freq%
        + fsr seed=%seed%
          slw %rndslw% %rndslw%
          * %rndamt%
    lut wav%% lin=1
    #sin

~myosc id=myosc1 freq=1    seed=0x5fa7 rndamt=$p_rndamt_1 rndslw=$p_rndslw_1
~myosc id=myosc2 freq=1.5  seed=0xcd39 rndamt=$p_rndamt_2 rndslw=$p_rndslw_2

<out:
  calc myosc1
  * $p_amp_1
  + calc myosc2
    * $p_amp_2
  clp
~~~ lang=cycle
] lcurve test 3 example patch



# Procedural Lookup Tables (LUTs)

Cycle supports an arbitrary number of procedural look up tables which share the same namespace with curve lookup tables.

The table size must be a power of two.

Tables are calculated either per-voice (at note-on, using the then-current parameters and modulation), or are statically shared among all voices.

~~~
lut pow shared 256

<lut_pow:
  # (note) implicit var "x" stores normalized table position (0..1=last table element)
  # (note) the default out value is $x
  fma 2 -1
  pow 1.44

<out:
  sin
  fma 0.5 0.5
  lut pow lin=1
~~~ lang=cycle
] shared LUT example

~~~
param p_exp 1 1 16

mod m_exp

lut pow voice 32

<lut_pow:
  # (note) implicit var "x" stores normalized table position (0..1=last table element)
  # (note) the default out value is $x
  fma 2 -1
  pow exp=$m_exp

<out:
  sin
  fma 0.5 0.5
  lut pow lin=1
~~~ lang=cycle
] per-voice modulated LUT example

tip: lookup tables can greatly improve the rendering performance by precalculating recurring values

note: the default table size is 64
note: the default table mode is `voice`
note: LUT init sections must be named `lut\_\<id\>` (e.g. `\<lut\_pow\:`)
warning: if no init section has been declared for a LUT used by a "lut" module, compilation will fail
note: the shared LUTs are calculated after the `\<init\:` section
note: the per-voice (or per-wavetable cycle) LUTs are updated after the `\<prepare\:` / `\<wt\_init\:` section

## Inverse Lookup Tables
See [#sta][] module documentation for examples.


# Macros

Macros are used to define smoothly interpolated value sets, controlled by a single parameter (or other value).

They are mostly useful to hide the internal complexity of a Cycle patch, and reduce the number of (plugin-)parameters.

The [#mac][] module updates the current, interpolated values of the macro fields.

The current value of a macro field is accessed (read) using the `\<macro_id\>.\<field_name\>` syntax.

##-# Examples
~~~
id     osc_mix_macro_manual
name   "osc mix macro manual"
author bsp
cat    osc

# Plugin parameter
param p_osc_mix 0 0 1

# 4 manually configured settings
macro osc_mix
  sin   tri  saw   pul
  1     0     0    0
  0     1     0    0
  0     0     1    0
  0     0     0    1

<prepare:
  # calculate interpolated macro values
  #  (updated 1000 times per second)
  mac osc_mix pos=$p_osc_mix

<out:
  # calculate current output sample
  sin
  * osc_mix.sin
  + tri
    * osc_mix.tri
  + saw
    * osc_mix.saw
  + pul
    * osc_mix.pul
~~~ lang=cycle
] macro example (manual settings)

~~~
id     osc_mix_macro_permutate
name   "osc mix macro permutate"
author bsp
cat    osc

# Plugin parameter
param p_osc_mix 0 0 1

# auto-generated settings
#  (permutations of values 0/1/-1 => 3*3*3*3=81 settings)
macro osc_mix
  sin   tri  saw   pul
  permutate 0 1 -1

<prepare:
  # calculate interpolated macro values
  #  (updated 1000 times per second)
  mac osc_mix $p_osc_mix

<out:
  # calculate current output sample
  sin
  * osc_mix.sin
  + tri
    * osc_mix.tri
  + saw
    * osc_mix.saw
  + pul
    * osc_mix.pul
~~~ lang=cycle
] macro example (permutated settings)

`pos` is in the range 0..1 (last setting is lerped back to first setting)

note: the maximum number of (auto-generated) permutations is limited to 16384

tip: alternatively to macros, the `<init:` and `<prepare:` sections may also contain arbitrarily complex, procedural pre-calculations


# Maps

Maps are used to remap values using non-contiguous tables.

Applications include microtuning scales, drum maps or envelopes.

A map is divided into *n* regions:
~~~
id map_test_4

map mymap
       freq vol cutoff   # map field names
  0.0  1    0.2   0.8    ## map region 1 starts at 0.0
  4.0  1.5  0.5   0.2    ## map region 2 starts at 4.0
  5.0  2.5  1.0   0.1    ## map region 3 starts at 5.0

<out:
  note
  & 7
  vst mapkey  # store (note modulo 8) in var 'mapkey'
  saw
    freq:
      $mapkey
      map mymap.freq def=1 mode=lin  # remap previous output value using linear interpolation
  svf
    freq:
      $mapkey
      map mymap.cutoff def=0.5 mode=up  # remap previous output value (round up to nearest map key)
  * $mapkey
    map mymap.vol def=0 mode=down  # remap previous output value (round down to nearest map key)
~~~ lang=cycle
] map example

See [#map][] module.


# Templates

Templates are parametrizable code snippets. Each template supports an arbitrary number of variables which will be replaced by either their default values or the values assigned in the template instantiation.

The special var `%%` is replaced by a unique template instantiation id (used when generating global variables, lanes, or parameter / modulation names).

tip: variables declared within a template are automatically prefixed with a unique id during instatiation

Template applications:
- act as a namespace for variables (when instantiated in- or outside of a lane)
- generate args, params, mods (when instantiated outside of a lane or when using the [#global][] section)
- generate audio layers (when instantiated outside of a lane, see [Audio lanes](#audio-lanes))
- insert parametrizable code into a lane (when instantiated inside a lane or when inserting a lane of an explicitily named global template instance (see [init, prepare, calc lanes](#init-prepare-calc-lanes)))

## Example
A simple, dual oscillator additive synthesis patch plugin.
Each macro oscillator renders the fundamental plus three harmonics with adjustable level decay.

The oscillators levels and harmonic decays can be changed in the sample voiceplugin parameter editor and can also be modulated in the sample's modmatrix.

~~~
id test_template_2

modparam lvl1  0.75
modparam hdcy1 0.25
modparam lvl2  0.25
modparam hdcy2 0.125

def osc_sin_hdcy_1 freq=1 hdcy=0.25
  vst myosc_harm 1
  sin
    freq:
      %freq%
  * $myosc_harm
  mul myosc_harm %hdcy%
  + sin
      freq:
        %freq%
        + 1
    * $myosc_harm
    mul myosc_harm %hdcy%
  + sin
      freq:
        %freq%
        + 2
    * $myosc_harm
    mul myosc_harm %hdcy%
  + sin
      freq:
        %freq%
        + 3
    * $myosc_harm
    mul myosc_harm %hdcy%

<out:
  ~osc_sin_hdcy_1 1 $m_hdcy1
  * $m_lvl1
  + ~osc_sin_hdcy_1 freq=2.5 hdcy=$m_hdcy2
    * $m_lvl2
~~~ lang=cycle
] additive synthesis template example

## Instance id
Template instances are usually auto-enumerated (1..n) but the instance name can be overridden using the `id` attribute:
~~~
def myclass freq=1
  var freq = %freq%
  var out
  arg a_%%_amp  0.125  # a_mc_amp for id=mc
  arg a_%id%_fb 0      # a_mc_fb for id=mc

~myclass id=mc

<out:
  mc.out
    * $a_mc_fb
  + tri freq=mc.freq
    * $a_mc_amp
      * 8
  sto mc.out
  * rmp 0 1
  clp
~~~ lang=cycle
] explicit instance id ("template instance id" example patch)

## Audio lanes
Template declarations may also include audio lanes.

tip: all audio lanes are summed to generate the final output, see [#layers][]

~~~
id test_template_4

def myosc f1=1 f2=1.5 f3=2 dcy=0.5
  modparam myosc%%_lvl 1

  <out_myosc%%:
    sin freq=%f1%
    vst lvl %dcy%
    + sin freq=%f2%
      * $lvl
    mul lvl %dcy%
    + sin freq=%f3%
      * $lvl
    * $m_myosc%%_lvl
    end # (note) end lane so the second ~myosc does not end up in the first myosc lane

~myosc 1 1.5 2 dcy=0.5
~myosc 1.015 1.5025 2.015 dcy=0.25
~~~ lang=cycle
] lanes and instance id example

### init, prepare, calc lanes
The `init`, `prepare` and `calc` lanes are not treated as audio lanes but can rather be inserted at any point in the patch.

~~~~
arg p_amp 0.5

def myclass
  arg a_%%_freq 0 -1 1  # a_mc_freq for id=mc1
  arg a_%%_amp 1        # a_mc_amp for id=mc1
  var v_rate
  init:
    var myinitvar   # test vardecl (internally prefixed by templ class+instance id+lane id)
    42 
    trc             # debug: print 42 to see if init was called
  prepare:
    $a_%%_freq
    bts 4 4         # -1..1 => 1/4..*4
    trc             # debug: print rate
    sto v_rate
  calc:
    tri freq=$v_rate
    * $a_%%_amp

~myclass id=mc1
~myclass id=mc2

<init:      # note-on
  init mc1  # insert init lanes
  init mc2

<prepare:      # prepare next block (update internal vars)
  prepare mc1  # insert prepare lanes
  prepare mc2

<out:          # calculate output
  calc mc1     # insert calc lanes
  + calc mc2
  * rmp 0 1
  * $p_amp
  clp
~~~~ lang=cycle
] init, prepare, calc template lanes ("template instance id 2" example patch)

## global 
Lines inside the `global:` section are placed outside the current lane.

~~~
arg p_amp 0.7 0 8

def sineosc
  global:
    arg p_%%_freq 1 1 4
    arg p_%%_amp 0.7

  sin freq=$p_%%_freq
  * $p_%%_amp

<out:
  ~sineosc
  * rmp 0 1
  * $p_amp
  clp
~~~~ lang=cycle
] test_template_6

## luts

Templates may also contain procedural look-up-tables (LUTs). See `test\_template\_5` example patch (and `osc\_sin\_tri\_1\_lut.cy` machine include file) for a LUT based oscillator template.

~~~
id test_template_5

inc osc_sin_tri_1_lut

<out:
  ~osc_sin_tri_1_lut
  clp
~~~ lang=cycle
] lut based template oscillator plugin

## pre

If the first module in an input or template declaration is a template call and an operator other than the default (`=` / `A`) is used, the first module must be [#pre][].

Example:
~~~
def tmp_mul 
  * 0.1

def tmp_abs
  abs

def mytmp
  pre
  + ~tmp_mul
    #trc
    ~tmp_abs

<out:
  sin
  * 0.5
  ~mytmp
  clp
~~~ lang=cycle
] pre

This is currently required due to a parser-issue which may be proper-fixed later.

## structs

Templates can also be used as variable containers (*structures*):
~~~
# example for a struct-like template
arg p_amp 0.7 0 8

def mystruct
  var x
  var y

  <init:  ## (note) init is evaluated before prepare
    set x 1.0
    set y 0.2

~mystruct id=s
~mystruct id=t

<prepare:
  s.x          ## print default x=1.0
  trc
  set t.y 0.7  ## override default y=0.3

<out:
  sin
  * s.x  ## => *1.0
  * t.y  ## => *0.7
  * rmp 0 1
  * $p_amp
  clp
~~~ lang=cycle
] test template struct 1


# Includes

Re-usable code snippets can be placed in `STConfig.cycle_machine_inc_path` (`eureka/machine_inc/` by default) and then inserted into patches:

~~~
id test_template_3

modparam m_lvl1   1
modparam m_lvl2   0
modparam m_trimix 0

# the (global) include file defines the template "osc_sin_tri_1"
inc osc_sin_tri_1

<out:
  ~osc_sin_tri_1 freq=1
  * $m_lvl1
  + ~osc_sin_tri_1 1.5
    * $m_lvl2
  ipl t=$m_trimix
    b:
      tri
      * $m_lvl1
      + tri freq=1.5
        * $m_lvl2
~~~ lang=cycle
] include file example

tip: in the Cycle machine source editor, press `lctrl-g` to edit the include file under the cursor, or `lctrl-h` to copy its native path name (for opening it in an external editor)

tip: press `lctrl-t` to show a selection dialog that lists all available includes. upon confirmation, `inc` lines will be inserted for all selected items.


## Patch-buffers

Patches may contain an arbitrary number of source buffers.
This can be used to split complex patches into multiple chunks (e.g. template definitions, lanes, ..).

Patch-buffer names must commence with a `.` character.

~~~
def mytemplate det=0
  sin
    freq:
      1
      + %det%
  + sin
      freq:
       1
       + %det%
       * 2
~~~ lang=cycle
] \.mytemplateinc buffer

~~~
arg p_amp 0.7 0 8
arg p_det 0 -1 1

inc .mytemplateinc

<out:
  ~mytemplate
  + ~mytemplate det=$p_det
  * rmp 0 1
  * 0.25
  * $p_amp
  clp
~~~ lang=cycle
] main source

note: see `multibuf test 3` example patch


Just like global includes, patch buffers are selected for editing by pressing `lctrl-g` while the cursor is on an `inc` or `~` template instantiation line.

A patch buffer is deleted by removing all its content.

note: the last seen cursor position is stored per buffer but it is _not_ persistent across project file saves



# Plugins
Cycle patches can also be exported as native code (C/C++) voice plugins.

The following restrictions apply:
  - [#slf][] and [#zsq][] modules may not be used
  - no references to voice plugins (`vpl`)
  - `dur`, `wt_w`, `wt_h`, `wt_cyclelen`, `wt_freqs`, `skip`, `xfade`, `mirror`, `zone` keywords are not allowed

note: sample zones referenced by modules will be embedded in the plugin (e.g. [#zon][], [#zlp][], [#ssl][])

Example:
~~~~
id fm
oversample 4
param p_mod_freq 0.5 min=0 max=1
param p_mod_amt  0       0     1
param p_vsync    0       1    16
mod m_mod_freq
mod m_mod_amt
<out:
  sin
    phase:
      sin vsync=$p_vsync window=sin
        freq:
          # 0..1 => 1/16 .. 0 .. *16
          $m_mod_freq
          fma 2 -1
          bts 16
      * $m_mod_amt
~~~~ lang=cycle
] simple 2-op FM plugin

## Plugin header
Voice plugin patches **must** declare a unique plugin id:
~~~
id     myplugin_v1
name   "my plugin v1"
author bsp
cat    osc
~~~ lang=cycle

note: the `id` keyword enables voice-plugin mode. `name`, `author` and `cat` are optional.

## Parameters and modulations
Plugins have "n" parameters (shared among all voices), and "n" mods (per-voice modulation).
It is recommended to not exceed 8 parameters and mods.

note: parameter names may be prefixed with `p_` (prefix will be removed during export)

note: mod names may be prefixed with `m_` (prefix will be removed during export)

If a mod has a parameter of the same name (ignoring the optional prefix), the effective mod value is the sum of the shared and mod values.

~~~~
float modcutoff = shared->params[PARAM_CUTOFF] + voice->mods[MOD_CUTOFF];
~~~~
] auto-generated code example

Frequency, parameters, and modulation will be interpolated over processing blocks. The block update rate (in Eureka) is 1000Hz, i.e. a processing block consists of e.g. 48 sample frames at a sample rate of 48kHz.

### Params
Parameters are always in the (normalized) range `0..1`.

Parameters may have default (UI reset) values, which will be exported to the plugin:
~~~
param p_cutoff 0.9
~~~ lang=cycle
] default reset value

Normalized parameters are scaled to their (optional) min / max value range when they are accessed:
~~~
param p 0.5 1 16
~~~~ lang=cycle
] p will be scaled to range 1..16. the effective default value is 1+0.5*(16-1)=8.5

Instead of `param`, its shortcut form `p` can be used instead:
~~~
p drv 0.5
~~~~ lang=cycle
] param shortcut syntax

note: when *not* in plugin mode, `param` falls back to `arg` (no arrays supported in that case)


#### Param Preset Values
Each parameter can be assigned a list of preset values (shown when hold-clicking the param value widget).

##-# Examples
~~~~
param p_angle 0 0 1

param_values p_angle 0="0 deg" 0.25="90 deg" 0.5="180 deg" 0.75="270" 1="360 deg"
~~~~ lang=cycle
] param preset values example 1

~~~~
param p_mode 0 0 3

pv p_mode 0=red 1=green 2=blue
~~~~ lang=cycle
] param preset values example 2 (shortcut syntax)

note: parameter preset values always refer to _unscaled_, normalize 0..1 values

note: when *not* in plugin mode, `param_values` falls back to `arg_values`


#### Param Groups
Parameters can be assigned to a group (in order to classify them).

Grouped parameters can, for example, be randomized all at once in the UI.

~~~~
param p_tune_1  0.5 -1 1  group=tune
param p_tune_2  0.5 -1 1  group=tune
param p_tune_3  0.5 -1 1  group=tune
param p_tune_4  0.5 -1 1  group=tune
param p_tune_5  0.5 -1 1  group=tune
~~~~ lang=cycle
] parameter groups (excerpt from osc_hh_v1.cy)

#### Param Sections
Parameters can (also) be assigned to sections (in order to group them in the UI).

~~~~
id more_than_8_params

param p_amp 1 0 1

def myosc id=0
  param p_myosc%id%_freq   0   0 1  section=myosc%id% group=tune
  param p_myosc%id%_amp    0.5 0 1  section=myosc%id% group=amp
  param p_myosc%id%_vsync  0   0 1  section=myosc%id% group=vsync
  sin
    freq:
      $p_myosc%id%_freq
      fma 2 -1
      bts 4 4
    vsync:
      $p_myosc%id%_vsync
      bts 1 16
  * $p_myosc%id%_amp
  

<out:
  + ~myosc id=0
  + ~myosc id=1
  + ~myosc id=2
  + ~myosc id=3
  
  * $p_amp
  clp
~~~~ lang=cycle
] parameter sections ("more than 8 params" example patch plugin)


#### Array Params
Each parameter can be associated with an array ([#lut][]). Arrays have up to 32 variations, which are selected by the (normalized) parameter value.
The maximum array size is 256.

Each array param may also be associated with a curve, which defines the initial element values for a new plugin instance.


Example:
~~~~
# additive synthesis using 16 sine partials
#  the 'level' array defines the partial magnitudes
#
id     array_test_1
name   array_test_1
cat    osc
author bsp

# selects 'level' array variation 'level' (0..1 => variation 1..7..0)
param p_level 0 0 1

# modulates 'level' array variation (per block)
mod   m_level

# connect curve 'level' to param 'p_level'
#   - array size = 16 elements
#   - num array variations = 8
#   - initial (variation 1) values are defined by curve 0
curve 0 level param=p_level size=16 var=8

var v_index
var v_out

<out:
  set v_index 0
  set v_freq 1
  set v_out 0
  rep 16
    $v_out
    + sin freq=$v_freq
        * $v_index
          lut level clamp=0 lin=0
          * 1/16
    sto v_out
    inc v_index 1/16
    inc v_freq
  $v_out
~~~~ lang=cycle
] additive synthesis using 16 sine partials

##### array keyword and per-element names and ranges

The following alternative syntax for an array declaration allows to specify names, reset values, display min / max ranges, and editing precision (number of fractional digits) per element:

~~~~
id array_param

param p_amp 0 0 1

# "0" is the index of the curve (0..15) which defines the initial array element values for new plugin instances
#   (note) this parameter can be set to -1, or skipped entirely
array 0 p_amp dpyMin=0 dpyMax=1 size=4 var=4
  0 fundamental 1
  1 partial2    0
  2 partial3    0
  3 partial4    0

mod m_amp

<out:
  0
  + sin
    * 0/4
      lut p_amp
  + sin freq=2
    * 1/4
      lut p_amp
  + sin freq=3
    * 2/4
      lut p_amp
  + sin freq=4
    * 3/4
      lut p_amp
~~~~ lang=cycle
] four-partial additive array param oscillator


Plugin param arrays can also be addressed like structs:
~~~~
id array_param_elname_test_2

param p_amp 0 0 1

array p_amp dpyMin=0 dpyMax=1 size=4 var=4
  0 fundamental 1
  1 partial2    0
  2 partial3    0
  3 partial4    0

mod m_amp

<out:
  0
  + sin
    * p_amp.fundamental
  + sin freq=2
    * p_amp.partial2
  + sin freq=3
    * p_amp.partial3
  + sin freq=4
    * p_amp.partial4
~~~~ lang=cycle
] four-partial additive array param oscillator (struct-like element access)


Instead of `array`, its shortcut form `ap` (array parameter) can be used instead:
~~~~
id array_shortcut_test_1
param cfg 0
ap cfg s=3 v=4  # == array 0 param=cfg size=3 var=4
  0 freq   0.5 0    1  3  # def=1 min=0 max=1  prec=3
  1 amp    1              # def=1 min=0 max=1  prec=5
  2 vsync  1   1    32 2  # def=1 min=1 max=32 prec=2
<out:
  sin window=hse4
    freq:
      cfg.freq
      fma 2 -1  #  0..1 => -1..1
      bts 4 16  # -1..1 => /4..*16
    vsync:
      cfg.vsync
      fma 31 1  # 0..1 => 1..32
  * cfg.amp
~~~~ lang=cycle
] array parameter shortcut syntax

caution: while each array element has its own reset / default value, display value range, and editing precision, accessing it in a patch will always return the storage value, i.e. it is up to the patch to actually scale it to the display range

The following attributes are supported in an array declaration:
|:Name    |:Shortcut name |:Description
| param   | p             | associated parameter name (can be omitted when it's the third word)
| size    | s             | array size (1..256)
| var     | v             | num variations (1..32)
| min     | -             | minimum storage value (-1..1, def=0)
| max     | -             | maximum storage value (-1..1, def=1)
| dpyMin  | dmin          | minimum display value (arbitrary float) (def=storage min)
| dpyMax  | dmax          | maximum display value (arbitrary float) (def=storage max)
| dpyPrec | dprec         | display value editing precision (num fractional digits, 0..7) (def=5)
] Array parameter attributes


note: the initial values of the first variation of an array are defined by the associated (bezier) curve

caution: the curve resolution should equal the array size (a power of two), otherwise the initial (variation 1) array values will either be a partial copy of the curve (when the curve size is larger than the array size), or the remaining elements will be filled with 0 (when the curve size is smaller than the array size).

caution: when wrap-around mode is used (`clamp=0`), the array size should be a power of two

note: the effective, interpolated (per-voice) array values are updated at the beginning of a block whenever the curve selection (array control parameter and / or modulation) has changed, or when the voice has just been started.

note: while array param floating point elements are stored in signed 16bit format in Eureka sampler patches (`.esa`), from a plugin's point of view they are always 32bit IEEE floats.

tip: in the editor, array params have an 'a' button next to them which opens the array / variation editor dialog



### Mods
Mods have an implicit reset value of 0 and are typically in the range `-1..1`.

~~~
mod m_cutoff
~~~ lang=cycle
] declare modmatrix destination

note: the actual range is unlimited (float value range), i.e. clipping it (if necessary) is up to the patch


### modparam shortcut
In plugin mode, patches usually declare a `mod` for each `param` so it can be modulated via the modulation matrix.

The `modparam` and `mp` keywords declare both a `param` and a `mod`, and auto-prefixes the names with `p_` and `m_`:
~~~~
id myplugin

modparam amp   # declare both param p\_amp and mod m\_amp

<out:
  sin
  * $m_amp  # read \($p\_amp + $m\_amp)
~~~~ lang=cycle
] modparam example 1

~~~~
id myplugin

mp amp   # declare both param p\_amp and mod m\_amp

<out:
  sin
  * $m_amp  # read \($p\_amp + $m\_amp)  (recommended syntax)
  * $amp    # read \($p\_amp + $m\_amp)  (easier to read but might be ambiguous)
~~~~ lang=cycle
] modparam example 2 (shortcut syntax)



### Args and envelopes
(Regular) args and curves (spline envelopes) will be baked into the plugin source.

### Audio input
The initial value is the output of the previous sound module (voice plugin, sample, VST effect, ..).

~~~
id     my_amp
name   "my amp"
author bsp
cat    amp

param p_level 1
mod   m_level

<out:
  * $m_level
~~~ lang=cycle
] simple "amp" plugin

### init
The `<init:` section can be used to implement macro parameters that calculate additional internal (voice state) variables from user defined parameters. Code placed in this section will be executed everytime a new note is played.

### prepare
The `<prepare:` section is evaluated before the main section(s), and after the (optional) `<init:` section.

Heavy calculations, which do not need to be evaluated per sample frame, should be placed in the `prepare` section.

Code placed in this section will be executed once per processed block. This can be used to implement macro modulation that reads the current modulation variables and writes one or many (internal voice-state) variables.

### Free runnning oscillators
By default, the phase and noise generator states reset when a new note is played.

The `sin`, `tri`, `saw`, `pul`, `pha`, `fsr`, `nos` modules have a (constant) `reset` input that determines whether the reset occurs at note on (`reset=1`, default), or just once when the voice is allocated (`reset=0`). The `cycle` input should be set to 0 (`cycle=0`).

Example:
~~~
<out:
  fsr cycle=0 reset=0
~~~ lang=cycle
] free-running linear feedback shift register noise oscillator

## Stereo processing

By default, the audio input and output is monophonic. However, when (at least one of) the [#rdl][], [#rdr][], [#wrl][], [#wrr][] modules is used, the audio signal path becomes stereo.

In stereo mode, the default output variable (`out` in the generated code) is still available but will _not_ be written to the audio input.
Instead, the [#wrl][] / [#wrr][] modules take the previous output value and write it to the left / right channel output.
Vice versa, [#rdl][] / [#rdr][] read the current left / channel input.

Example:
~~~
id stereo_test_1

# declare param p_amp and mod m_amp
modparam detune 0.5 0 1

var v_freq_l smooth=1
var v_freq_r smooth=1

<prepare:
  # calc detuned left / right channel oscillator frequencies
  $m_detune
  fma 0.1 -0.05
  . neg
    + 1
    sto v_freq_l
  +1
  sto v_freq_r

<out:
  sin freq=$v_freq_l
  * 0.7
  wrl # write left channel output

  sin freq=$v_freq_r
  * 0.7
  wrr # write right channel output
~~~ lang=cycle
] stereo sine oscillator plugin

## Native code extension modules (C / C++)

In plugin mode, new Cycle modules can be implemented via native code (C / C++) extensions.

The `xinc` keyword is used to import a native code header file.

The `eureka_config.tks:stfx_inc` variable sets the default search path for native code extension files (usually `$(PROGRAM_DIR)/include/xinc/`).

Example:
~~~
id ext_mod_test_1
xinc "xamp.h"
modparam drive 0.5 0 1
<out:
  sin
  xamp sat=1
    drive:
      $drive
      fma 2 -1  #  0..1 => -1..1
      bts 4 16  # -1..1 => /4..*16
~~~ lang=cycle
] xamp example

tip: the `xinc` files are read everytime the script is compiled, i.e. changes to the C / C++ code take effect immediately and do not require an application restart


### Header file interface definition keywords

During import, the header file is scanned to find the module name, statically compiled configuration items, dynamic inputs, module vars, voice struct fields, and init / prepare / render code to be inserted for each module instance:
~~~
/*
 * Native code extension module definition (parsed by Cycle):
 *
 *  @cycle_mod      xamp
 *  @cycle_pre      // %cymod% pre-include code line
 *  @cycle_static   // %cymod% module-static test code line
 *  @cycle_init     // %cymod% init function test code line
 *  @cycle_new      // %cyid% voice_new() test code line
 *  @cycle_delete   // %cyid% voice_delete() test code line
 *  @cycle_voice    // %cyid% voice_t field struct test code line
 *  @cycle_noteon   // %cyid% noteon function test code line
 *  @cycle_input    drive 0.5
 *  @cycle_config   sat 0  0=off 1=on
 *  @cycle_emit     %cyio% *= %cyin_drive%;
 *  @cycle_emit     if(%cycf_sat% > 0) %cyio% = tanhf(%cyio%);
 *
 */
~~~
] xamp example extension module

In this (very simple) example module, the entire native code implementation is written using the `@cycle_emit` keywords (each one adds a line of code for each module instantation).
In a more complex module, the header file will contain additional `C` structs or `C++` classes. See the `x303.h`, `xldf.h`, `xrlf.h` and `xrm.h` example files.

tip: while the main extension file is just a `.h` (or `.hpp`) header, this may use all C / C++ features and may include additional `.c` / `.cpp` / `.cxx` / `.h` / `.hpp` files.

### Module input and output

Extension modules may read the output of the previous module from `%cyio%` (float) and also write new output to it.

### Static configuration items

Configuration options declared via `@cycle_config` will be turned into `#define`s and can be inserted anywhere in a (module-instance specific) `@cycle\_` code line via the `%cycf_\<config\_item\_id\>%` syntax.

In a Cycle script, these will look like regular inputs (e.g. the `sat` input in the xamp example above).

tip: When static configuration items are used in conditional statements (`if`, `switch`, ..), the C/C++ compiler will _not_ generate runtime checks and instead optimize the code for the current configuration

tip: an (optional) list of constant names can be assigned to each config item (e.g. `0=off 1=on`)

### Dynamic inputs
Inputs declared via `@cycle_input` will be evaluated at runtime. The current input value for a module instance is stored in a variable which can be refered to via `%cyin_\<input\_id\>` anywhere in a (module-instance specific) `@cycle\_` code line.

note: Unless the module is very simple, the input values are usually passed to a user-defined `calc()` / `process()` function or method.

### Voice fields
The native code implementation for a `calc()` method is usually placed in a class which is instantiated per voice, and per module instance (see `include/xinc/xrm.h`):
~~~
 [..]
 * @cycle_voice  je_ringmod_state_t %cyid%;
 [..]
~~~
] je ringmod voice field instance

The field can (optionally) be reset at note on (see `include/xinc/x303.h`):
~~~
 [..]
 * @cycle_noteon voice->%cyid%.reset(voice->sample_rate, voice->sr_factor);
 [..]
~~~
] 303 filter voice field note on reset

tip: `voice->sample_rate` stores the host's output sample rate (e.g. 32000, 44100, 48000, 96000). `voice->sr_factor` stores the ratio between the patches reference sample rate (def=48kHz) and patch-internal oversampling factor and the host sample rate (e.g. when both host and reference sample rate are set to 48kHz and the oversampling factor is 4, `sr_factor` is set to `1/4`). A properly written extension module behaves the same at all sample rates and oversampling factors so make sure to incorporate `sr_factor` in your code (e.g. when calculating envelopes or filter cutoffs).

### Extension modules

#### x303
] 303-ish filter

A 303-ish filter (written in 1998 by Andy Sloane).

##-# Inputs

|noheader
|: |: `freq`   |: cutoff frequency (0..1)
|: |: `q`      |: resonance (0..1)
|: |: `trig`   |: envelope trigger (set to 1 to trigger at note-on)
|: |: `envmod` |: filter envelope modulation amount
|: |: `envdcy` |: filter envelope decay
].table_inputs


#### xdi
] diode waveshaper

A diode wave shaper (based on JE's ringmod which is based on Julian Parker's [A simple model of diode-based ring-modulator](http://recherche.ircam.fr/pub/dafx11/Papers/66_e.pdf)).

##-# Inputs

|noheader
|: const |: `vb`    |: forward voltage-bias
|: const |: `vbvl`  |: linear voltage threshold
|: const |: `slope` |: linear voltage slope
].table_inputs


#### xdly
] delay

A delay.

##-# Inputs

|noheader
|: |: `t`   |: normalized delay time (0..1 => 0..delay_buffer_size-1) |: def=0.5
|: |: `fb`  |: feedback amount (0..1)                                 |: def=0.0
|: |: `wet` |: dry / wet mix                                          |: def=0.5
].table_inputs


#### xfs
] frequency shifter with built-in sin / cos oscillators

A frequency shifter based on Squinky Lab's sql\_bootyshifter.

##-# Inputs

|noheader
|: |: `freq`  |: frequency shift (-f..f)
|: |: `range` |: range in Hz             |: def=500
].table_inputs


#### xfsxy
] frequency shifter with sin / cos inputs

A frequency shifter based on Squinky Lab's sql\_bootyshifter.

Dedicated `x` / `y` inputs allow for more fine-grained control over the sine / cosine inputs.

##-# Inputs

|noheader
|: |: `x` |: sine input
|: |: `y` |: cosine input
].table_inputs


#### xldf
] Moog-ish ladder filter

A Moog-ish ladder filter (adapted from a musicdsp.org post in 2010)

##-# Inputs

|noheader
|:       |: `freq` |: cutoff frequency (0..1)
|:       |: `q`    |: resonance (0..1)
|: const |: `mode` |: 0=`lpf` 1=`hpf` 2=`bpf`
].table_inputs


#### xlrf
] Linkwitz-Riley LP/HP/AP filter

~~~
The sum of the Linkwitz-Riley (Butterworth squared) HP and LP outputs, will result an
 all-pass filter at Fc and flat magnitude response - close to ideal for crossovers.
~~~
] Lubomir I. Ivanov

based on an implementation by `moc.snoitcudorpnrec@mij`

##-# Inputs

|noheader
|:  |: `freq` |: cutoff frequency (0..1)
|:  |: `mode` |: 0=lpf 1=hpf          |: def=0 (lpf)
].table_inputs


#### xrm
] four quadrant diode ring modulator

A four quadrant diode ring modulator (based on JE's ringmod which is based on Julian Parker's [A simple model of diode-based ring-modulator](http://recherche.ircam.fr/pub/dafx11/Papers/66_e.pdf).

##-# Inputs

|noheader
|:       |: `a`     |: carrier input
|:       |: `b`     |: modulator input
|:       |: `alvl`  |: carrier level
|:       |: `blvl`  |: modulator level
|:       |: `off`   |: offset
|:       |: `vb`    |: forward voltage-bias
|:       |: `vbvl`  |: linear voltage threshold
|:       |: `slope` |: linear voltage slope
|: const |: `apol`  |: carrier polarity (0..2)
|: const |: `bpol`  |: modulator polarity (0..2)
|: const |: `mode`  |: 0=ring 1=sum 2=diff 3=min 4=max
].table_inputs




## Export
Press `lctrl-p` or `lctrl-w` in the Cycle dialog to export the current patch as a plugin (and (re)load it).

Press `lctrl-lshift-p` to toggle auto-export when patch is saved / edited (`lctrl-s` or after arg changes).

## Compiler installation
Cycle uses Clang / GCC to compile the plugins.

No knowledge of C/C++ is required to create new plugins but the following (free) build tools have to be installed so the application can invoke them in the background.


### Windows
Please install MSYS2 <https://www.msys2.org/>, then (in MSYS2 bash console) type:
~~~
$ pacman -Syu
$ pacman -S base-devel gcc
~~~
to update the package database and install the compiler environment.

note: In case MSYS2 was _not_ installed to its default directory, edit `eureka_config.tks` and update the `stfx_cc_msys2_dir` variable accordingly.

### macOS
Please install brew <https://brew.sh/> (which will also install the XCode commandline tools):
~~~
$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
~~~
(see <https://mac.install.guide/commandlinetools/3.html>)

note: XCode updates are installed through the macOS `Software Update` and their availability appears to depend on the currently installed macOS version

### Linux
On Debian or Ubuntu, type
~~~
$ sudo apt update
$ sudo apt install build-essential
~~~



$(buf!footnote)
$(buf!w3validator)

Document created in $(var:gen_ms) on $(var:localdatetime)
].create
