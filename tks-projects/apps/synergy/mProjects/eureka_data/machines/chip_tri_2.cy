id chip_tri_2
oversample 4

arg a_oct 1 1 4
arg_values a_oct 1=1 2=2 3=3 4=4

param p_mulu 0 0 1
param p_muld 0 0 1
param p_spd 0 0 1
param p_tri_mix
param p_slu
param p_sld

mod m_mulu
mod m_muld
mod m_spd
mod m_slu
mod m_sld

var v_pha
var v_inc

<init:
  set v_inc 1

<prepare:
  spd
#  trc

var v_spd

<out:

  $v_inc
  cge 0
    $v_pha
    + $a_oct
      * spd
        * $m_spd
          + 1
          - $v_pha
            * $m_mulu
              # fma 0.75 0.25
    sto v_pha
    cge 1
      set v_inc -1
  $v_inc
  clt 0
    $v_pha
    - $a_oct
      * spd
        * $m_spd
          + 1
          - $v_pha
            * $m_muld
              # fma 0.75 0.25
    sto v_pha
    clt 0
      set v_inc 1

  $v_pha
  fma 2 -1
  ipl t=$p_tri_mix
   b:
     tri freq=0.5
  slw
    up:
      $m_slu
      fma 0.99 0.01
    down:
      $m_sld
      fma 0.99 0.01
  clp
