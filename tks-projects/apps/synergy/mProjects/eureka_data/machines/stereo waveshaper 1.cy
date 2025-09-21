id     stereo_waveshaper_1
author bsp
cat    waveshaper

modparam drywet 0.7
modparam iogain 0.5
modparam pan    0.5
modparam vol    0.5

curve 0 ws_left
curve 1 ws_right

var v_i
var v_o_l
var v_o_r

<prepare:

  $pan
  fma 2 -1 
  vst vpan

  $iogain
  fma 2 -1
  bts 2 2
  sto v_i
  rcp

  * $vol
    fma 2 -1
    bts 4 4
  . pre
    * pal $vpan
    sto v_o_l
  * par $vpan
  sto v_o_r

<out:
  # left
  rdl
  ipl t=$drywet
    b:
     * $v_i
     lut ws_left lin=1 b=1 c=1
     * $v_o_l
  wrl

  # right
  rdr
  ipl t=$drywet
    b:
     * $v_i
     lut ws_right lin=1 b=1 c=1
     * $v_o_r
  wrr
