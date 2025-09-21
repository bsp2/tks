# RMS compressor
id     cycle_rms_comp_mono_1
author bsp
cat    compressor

mp wet   0.7  0.0 1.0
mp punch 0.29 0   1.0
mp exp   0.2  0   1.0
mp atk   0.08 0   1.0
mp rls   0.35 0   1.0

zone 0 drumloop.0

var v_punch
var v_exp
var v_atk
var v_rls

<prepare:
  $punch
  fma 7.5 0.5
  sto v_punch

  $exp
  fma 3.5 0.5
  sto v_exp

  $atk
  pow 3
  fma 0.4 0.00001 #0.0002
  #trc
  sto v_atk

  $rls
  pow 2
  fma 0.002 0.00005
  #trc
  sto v_rls

<out:
  vst v_in

  rms s=16 w=1 #e=10.4
  vst v_rms
  * $v_atk
  #trc e=512
  + 0.0009
  vst v_slw
  $v_rms
  slw u=$v_slw d=$v_rls #0.0003
  #trc e=512
  * $v_punch
  clp 1 0
  + 1
  rcp
  - 0.5
  clp 4 0
  + 0.1
  pow $v_exp
  * 3.2
  clp 4 0

  ipl t=$wet
    b:
     * $v_in

    a:
      $v_in

  clp
