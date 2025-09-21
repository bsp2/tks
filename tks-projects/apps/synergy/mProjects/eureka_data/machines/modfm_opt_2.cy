# http://www.csounds.com/journal/issue11/distortionSynthesis.html
id modfm_opt_2

arg a_phase_reset 0

param p_k
param p_freq 0.5
param p_amp 0.5
param p_plain_fm

mod m_k
mod m_freq
mod m_amp
mod m_plain_fm

lut t_exp shared 16384

var v_k    smooth=1
var v_freq smooth=1

<lut_t_exp:
  # e^x
  pow base=2.71828182845904523
    exp:
      $x
      fma 128 -64  # 0..1 => -64..+64
 
<prepare:
  $m_k
  fma 8 0
  sto v_k

  $m_freq
  fma 2 -1
  bts 16 4
  qua 16
  sto v_freq

<out:
  # ---- cos modulator
  $v_k
  * sin freq=$v_freq phase=0.25 reset=$a_phase_reset
  fma 1/128 0.5
  lut t_exp
  # ---- cos carrier ring modulation
  * sin phase=0.25 reset=$a_phase_reset
  # normalize by e^-k
  * $v_k
    neg
    fma 1/128 0.5
    lut t_exp
  * $m_amp
    * 2
  ## clp 16 -16

  # lerp to plain FM for comparison
  ipl t=$m_plain_fm
    b:
      sin reset=$a_phase_reset
        phasemod:
          sin freq=$v_freq reset=$a_phase_reset
          * $v_k
      * $m_amp
