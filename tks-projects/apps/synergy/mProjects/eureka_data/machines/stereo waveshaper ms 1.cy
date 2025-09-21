id     stereo_waveshaper_ms_1
author bsp
cat    waveshaper

modparam drywet 0.77
modparam iogain 0.5
modparam pan_ms 0.5
modparam pan    0.5
modparam vol    0.5

curve 0 ws_mid
curve 1 ws_side

var v_i
var v_o_m
var v_o_s

var v_amp_l
var v_amp_r

<prepare:

  $pan_ms
  fma 2 -1 
  vst vpan_ms

  $iogain
  fma 2 -1
  bts 2 2
  sto v_i
  rcp
  . pre
    * pal $vpan_ms
    sto v_o_m
  * par $vpan_ms
  sto v_o_s

  $pan
  fma 2 -1 
  vst vpan
  $vol
  fma 2 -1
  bts 4 4
  . pre
    * pal $vpan
    sto v_amp_l
  * par $vpan
  sto v_amp_r

<out:

  # mid
  rdl
  + rdr
  * 0.5
  ipl t=$drywet
    b:
     * $v_i
     lut ws_mid lin=1 b=1 c=1
     * $v_o_m
  vst v_mid

  # side
  rdl
  - rdr
  * 0.5
  ipl t=$drywet
    b:
     * $v_i
     lut ws_side lin=1 b=1 c=1
     * $v_o_s

  # merge back to stereo
  vst v_side
  + $v_mid
  wrl a=$v_amp_l

  $v_mid
  - $v_side
  wrr a=$v_amp_r
