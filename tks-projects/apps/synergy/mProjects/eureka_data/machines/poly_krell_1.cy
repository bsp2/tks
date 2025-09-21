id poly_krell_1

param p_amp 0.5 0 1
param p_vsync
param p_sinefd

mod m_amp
mod m_gate
mod m_vsync
mod m_sinefd

var v_atk
var v_dcy
var v_sus
var v_rls
var v_rls_shp
var v_gate
var v_gate_dur_on
var v_gate_dur_off
var v_freq
var v_vsync

<init:
  set v_atk 0.001
  set v_dcy 0.005
  set v_sus 0.4
  set v_rls 0.005
  set v_rls_shp 0
  set v_gate 1
  set v_gate_dur_on 9000
  set v_gate_dur_off 2000
  set v_freq 1
  set v_vsync 1

<prepare:
  #$v_gate_dur_on
  #trc
  #$v_gate_dur_off
  #trc

<out:
  sin window=2
    vsync:
      $m_vsync
      + $v_vsync
        slw 0.05 0.01
    freq:
      $v_freq
      slw 0.05 0.005
  ipl
    t:
      1
      - $m_sinefd
    b:
      sin
  * $v_gate_dur_on
    +sr -1
    sto v_gate_dur_on
    $v_gate_dur_on
    . fed
        #trc
        set v_gate_dur_off
          value:
            fsr seed=$ab23 signed=0 reset=0
            fma 32000 16000
        set v_gate 0
    clt 0
      #trc
      $v_gate_dur_off
      +sr -1
      sto v_gate_dur_off
      clt 0
        set v_gate 1
        set v_gate_dur_on
          value:
            fsr seed=$1328 signed=0 reset=0
            fma 12000 4000
        set v_atk
          value:
            fsr seed=$7782 signed=0 reset=0
            fma 0.01 0.001
        set v_dcy
          value:
            fsr seed=$3731 signed=0 reset=0
            fma 0.01 0.001
        set v_rls
          value:
            fsr seed=$4535 signed=0 reset=0
            fma 0.01 0.001
        set v_freq
          value:
            fsr seed=$6502 signed=0 reset=0
            qua 3
            fma 3 1
        set v_vsync
          value:
            fsr seed=$1371 signed=0 reset=0
            qua 3
            fma 3 1
    $v_gate
    adsr a=$v_atk d=$v_dcy s=$v_sus r=$v_rls scl=0.01 rs=$v_rls_shp
    slw 0.001 0.001
  