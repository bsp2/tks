# AON-style frequency modulation (not phase modulation!)
#  requires "aon_fm_oscwaves" sample to be loaded (during recompilation)
#  (note) this is the voice-plugin version (embeds FM oscwaves)

id     aon_fm_v1
name   aon_fm_v1
cat    osc
author bsp

oversample 4

param p_carrier_wave  0       0 11  group=shape
param p_carrier_freq  0.5    -1  1  group=freq
param p_mod_wave      0       0 11  group=shape
param p_mod_freq      0.5    -1  1  group=freq
param p_mod_amp       0.5   -16 16  group=amp
param p_mod_kbd       0.625  -4  4
param p_carrier_kbd   0.625  -4  4
param p_mod_flt       0.8     0  1  group=filter

param_values p_carrier_wave 0="Sinus" 0.090909="Triangle" 0.181818="Turnpoint" 0.272727="Parabola" 0.363636="Extremes" 0.454545="Up&Down" 0.545454="LineMix" 0.636363="M-Shape" 0.727272="Heart" 0.818181="Sawtooth" 0.90909="Sample" 1="DoubleSinus"
param_values p_mod_wave     0="Sinus" 0.090909="Triangle" 0.181818="Turnpoint" 0.272727="Parabola" 0.363636="Extremes" 0.454545="Up&Down" 0.545454="LineMix" 0.636363="M-Shape" 0.727272="Heart" 0.818181="Sawtooth" 0.90909="Sample" 1="DoubleSinus"

mod m_carrier_wave
mod m_carrier_freq
mod m_mod_wave
mod m_mod_freq
mod m_mod_amp
mod m_mod_flt

arg p_lin     1 0 1
arg p_lin_lut 1 0 1

# (note) 12 sub-waves (128 frames per cycle)
zone 0 aon_fm_oscwaves.all
zone 1 aon_fm_oscwaves.all

var v_carrier_kbd
var v_mod_kbd
var v_mod_flt

<prepare:
  1.0
  kbd $p_carrier_kbd off=1/12 clamp=0
  sto v_carrier_kbd

  1.0
  kbd $p_mod_kbd off=1/12 clamp=0
  sto v_mod_kbd

  $m_mod_flt
  kbd 1 0
  sto v_mod_flt

<out:
  zlp 0 div=12 sub=$m_carrier_wave lin=$p_lin
    rate:
      $p_carrier_freq
      + $m_carrier_freq
      bts 16
      + zlp 1 div=12 sub=$m_mod_wave lin=$p_lin
          rate:
            $p_mod_freq
            + $m_mod_freq
            bts 16
            * $v_mod_kbd
        svf mode=0 freq=$v_mod_flt
        * $m_mod_amp
      * $v_carrier_kbd
