# RMS compressor
id     cycle_rms_comp_stereo_1
author bsp
cat    compressor

mp wet      0.7
mp punch    0.29
mp exp      0.2
mp atk      0.08
mp rls      0.35
mp three_d  0.5
mp sc_hpf   0.25
mp sc_gain  0.5
mp wet_gain 0.5

zone 0 drumloop.0

var v_threed_l
var v_threed_r

def rms_comp threed=0
  global:
    var v_punch%%
    var v_exp%%
    var v_atk%%
    var v_rls%%
    var v_sc_hpf%%
    var v_sc_gain%%
    var v_wet_gain%%

  <prepare:
    $punch
    * %threed%
      fma 0.5 1
    fma 7.5 0.5
    sto v_punch%%
  
    $exp
    * %threed%
      fma 0.05 1
    fma 3.5 0.5
    sto v_exp%%
  
    $atk
    * %threed%
      fma 0.4 1
    pow 3
    fma 0.4 0.00001 #0.0002
    #trc
    sto v_atk%%
  
    $rls
    * %threed%
      fma 0.4 1
    pow 2
    fma 0.002 0.00005
    #trc
    sto v_rls%%

    $sc_hpf
    * %threed%
      fma 0.1 1
    clp 1 0
    p2s
    sto v_sc_hpf%%

    $sc_gain
    * %threed%
      fma -0.1 1
    fma 4 -2
    pow b=10
    sto v_sc_gain%%
    #trc

    $wet_gain
    fma 2 -1
    pow b=10
    sto v_wet_gain%%

  # process
  vst v_in
  svf mode=hpf freq=$v_sc_hpf%% res=0.7
  * $v_sc_gain%%

  rms s=16 w=1 #e=10.4
  vst v_rms
  * $v_atk%%
  #trc e=512
  + 0.0009
  vst v_slw
  $v_rms
  slw u=$v_slw d=$v_rls%% #0.0003
  #trc e=512
  * $v_punch%%
  clp 1 0
  + 1
  rcp
  - 0.5
  clp 4 0
  + 0.1
  pow $v_exp%%
  * 3.2
  clp 4 0

  ipl t=$wet
    b:
     * $v_in
     * $v_wet_gain%%

    a:
      $v_in

<out:
  $three_d
  fma 2 -1
  sto v_threed_l
  rdl
  ~rms_comp threed=$v_threed_l
  clp
  wrl

  $three_d
  fma -2 1
  sto v_threed_r
  rdr
  ~rms_comp threed=$v_threed_r
  clp
  wrr
