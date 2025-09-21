id fof_lut_1

arg p_wave 0  # <0.25:tri <0.5:sin <0.75:pul <1:saw

modparam ratio  0.5
modparam modamt 0.5
modparam amp    0.7

lut tri    voice 256
lut dampen voice 256

var v_ratio
var v_pha
var v_tri_dampen
var v_modamt        smooth=1
var v_amp           smooth=1

<prepare:
  $m_ratio
  fma 15 1
  sto v_ratio

  $m_modamt
  * 8
  sto v_modamt

  $m_amp
  * 1
    - $m_modamt
      * 0.5
  sto v_amp


<lut_tri:
  sto v_pha
  if p_wave < 0.25
    then:
      tri freq=0 phase=$v_pha
    else:
      if p_wave < 0.5
        then:
          sin freq=0 phase=$v_pha
        else:
          if p_wave < 0.75
            then:
              pul freq=0 phase=$v_pha
            else:
              saw freq=0 phase=$v_pha
  # box 0.5

<lut_dampen:
  sto v_pha
  1
  - $v_pha
  pow 3
  * 1
    - 1
      - tri freq=0 phase=$v_pha
        fma 0.5 0.5
        sto v_tri_dampen
      pow 3
  * $v_tri_dampen
  # trc

<out:
  pha
  sto v_pha
  lut tri lin=1
  + $v_pha
    * $v_ratio
    lut tri lin=1 
    * $v_pha
      lut dampen lin=1
      * $v_modamt
  * $v_amp
  clp
