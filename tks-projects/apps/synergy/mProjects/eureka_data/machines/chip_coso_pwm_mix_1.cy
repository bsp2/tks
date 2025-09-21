id chip_coso_pwm_mix_1
oversample 4

param p_amp 0.5 0 1

mod m_amp

var v_pul
var v_sin

param p_phase
param p_pwm_f 0.5
param p_pwm_a 0.5

<out:
  pul freq=4
    width:
      tri
        phase:
          $p_phase
        freq:
          $p_pwm_f
          fma 2 -1
          bts 16 16
          * 0.001
      * $p_pwm_a
        * 0.4
      + 0.5     
  box 0.6
  sto v_pul

  sin
  sto v_sin 

  pha
  clt 0.25
    $v_pul
  cge 0.25
    $v_sin

  * $m_amp
    * 2
  clp
