id     osc_fm2s_macro_vsync
name   "fm2s vsync macro osc"
author bsp
cat    osc

oversample 16

arg window 0 0 3

param p_op12_mix 0.5
param p_op2_noise 0
param p_op12_pha 0
param p_op12_pha_rand 0
param p_op12_vsync 0
param p_op12_det 0
param p_op2_ratio 0.5
param p_op2_level 0

mod m_op12_mix
mod m_op2_noise
mod m_op12_pha
mod m_op12_fb
mod m_op12_vsync
mod m_op12_det
mod m_op2_ratio
mod m_op2_level

# 2*2=4 permutated settings
macro osc_mix
  sin1  tri1  sin2  tri2
  permutate 0 1

# 3*3=9 permutated settings
macro osc2_noise
  cutoff level
  permutate 0 1 0.5

# 4*4*4*4=256 permutated settings
macro osc_pha
  sin1   tri1  sin2  tri2
  permutate 0.0 0.75 0.25 0.5

# 2*2*2*2=16 permutated settings
macro osc_pha_rand
  sin1   tri1  sin2  tri2
  permutate 0 1

# 2*2*2*2=16 permutated settings
macro osc_vsync
  sin1   tri1  sin2  tri2
  permutate 1 16

# 3*3*3*3=81 permutated settings
macro osc_det
  sin1   tri1  sin2  tri2
  permutate 0.0 -0.01 0.01


var v_phrand_sin_1
var v_phrand_tri_1
var v_phrand_sin_2
var v_phrand_tri_2

var v_freq_1_sin
var v_freq_1_tri

var v_freq
var v_freq_2_sin
var v_freq_2_tri
var v_level_2
var v_osc_2_raw
var v_osc_2

<init:
  mac osc_pha_rand $p_op12_pha_rand

  fsr seed=$4489 cycle=0 reset=0
  * osc_pha_rand.sin1
  sto v_phrand_sin_1

  fsr seed=$ccdf cycle=0 reset=0
  * osc_pha_rand.tri1
  sto v_phrand_tri_1

  fsr seed=$1351 cycle=0 reset=0
  * osc_pha_rand.sin2
  sto v_phrand_sin_2

  fsr seed=$ae47 cycle=0 reset=0
  * osc_pha_rand.tri2
  sto v_phrand_tri_2

<prepare:
  mac osc_mix    pos=$m_op12_mix
  mac osc2_noise pos=$m_op2_noise
  mac osc_pha    pos=$m_op12_pha
  mac osc_vsync  pos=$m_op12_vsync
  mac osc_det    pos=$m_op12_det

  1
  sto v_freq
  + osc_det.sin1
  sto v_freq_1_sin
  $v_freq
  + osc_det.tri1
  sto v_freq_1_tri

  $m_op2_ratio
  fma 2 -1
  bts 4096 4
  pow 3
  qua 100
  sto v_freq
  + osc_det.sin2
  sto v_freq_2_sin
  $v_freq
  + osc_det.tri2
  sto v_freq_2_tri

<out:
  $m_op2_level
  * 16
  sto v_level_2

  sin freq=$v_freq_2_sin vsync=osc_vsync.sin2 window=$window
    phase:
      osc_pha.sin2
      + $v_phrand_sin_2
  * osc_mix.sin2
  + tri freq=$v_freq_2_tri vsync=osc_vsync.tri2 window=$window
      phase:
        osc_pha.tri2
        + $v_phrand_tri_2
    * osc_mix.tri2
  * $v_level_2
  sto v_osc_2_raw
  + fsr seed=$2938 cycle=0 reset=0
    svf mode=0 
      freq:
        osc2_noise.cutoff
        pow 3
    * osc2_noise.level
    * $v_level_2
  sto v_osc_2

  sin freq=$v_freq_1_sin vsync=osc_vsync.sin1 window=$window
    phase:
      $v_osc_2
      + osc_pha.sin1
      + $v_phrand_sin_1
  * osc_mix.sin1
  + tri freq=$v_freq_1_tri vsync=osc_vsync.tri1 window=$window
      phase:
        $v_osc_2
        + osc_pha.tri1
        + $v_phrand_tri_1
    * osc_mix.tri1
