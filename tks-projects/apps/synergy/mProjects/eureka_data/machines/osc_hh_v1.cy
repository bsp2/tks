id     osc_hh_v1
name   osc_hh_v1
cat    osc
author bsp

oversample 16

param p_tune_1   0.5 -1  1    group=tune
param p_tune_2   0.5 -1  1    group=tune
param p_tune_3   0.5 -1  1    group=tune
param p_tune_4   0.5 -1  1    group=tune
param p_tune_5   0.5 -1  1    group=tune
param p_level    0.0 0   1    group=shape
param p_width    0.0 0   1    group=shape
param p_vsync    0.0 0   1    group=shape

var v_speed_1
var v_speed_2
var v_speed_3
var v_speed_4
var v_speed_5

var v_level_1
var v_level_2
var v_level_3
var v_level_4
var v_level_5

var v_pw_1
var v_pw_2
var v_pw_3
var v_pw_4
var v_pw_5

var v_vs_1
var v_vs_2
var v_vs_3
var v_vs_4
var v_vs_5

macro m_level
  lv1 lv2 lv3 lv4 lv5
  permutate 0 1

macro m_width
  w1 w2 w3 w4 w5
  permutate 0 1

macro m_vsync
  vs1 vs2 vs3 vs4 vs5
  permutate 0 1

<prepare:
  $p_tune_1
  bts 16 24
  sto v_speed_1

  $p_tune_2
  bts 16 24
  sto v_speed_2

  $p_tune_3
  bts 16 24
  sto v_speed_3

  $p_tune_4
  bts 16 24
  sto v_speed_4

  $p_tune_5
  bts 16 24
  sto v_speed_5

  mac m_level $p_level
  mac m_width $p_width
  mac m_vsync $p_vsync

  1.0
  - m_level.lv1
  * 0.75
  + 0.25
  sto v_level_1

  1.0
  - m_level.lv2
  * 0.75
  + 0.25
  sto v_level_2

  1.0
  - m_level.lv3
  * 0.75
  + 0.25
  sto v_level_3

  1.0
  - m_level.lv4
  * 0.75
  + 0.25
  sto v_level_4

  1.0
  - m_level.lv5
  * 0.75
  + 0.25
  sto v_level_5

  m_width.w1
  * 0.4
  + 0.5
  sto v_pw_1

  m_width.w2
  * 0.4
  + 0.5
  sto v_pw_2

  m_width.w3
  * 0.4
  + 0.5
  sto v_pw_3

  m_width.w4
  * 0.4
  + 0.5
  sto v_pw_4

  m_width.w5
  * 0.4
  + 0.5
  sto v_pw_5

  m_vsync.vs1
  + 1.0
  * 7.0
  sto v_vs_1

  m_vsync.vs2
  + 1.0
  * 7.0
  sto v_vs_2

  m_vsync.vs3
  + 1.0
  * 7.0
  sto v_vs_3

  m_vsync.vs4
  + 1.0
  * 7.0
  sto v_vs_4

  m_vsync.vs5
  + 1.0
  * 7.0
  sto v_vs_5

<out:
  pul freq=$v_speed_1 width=$v_pw_1 vsync=$v_vs_1 reset=0
   phase:
     sin freq=0.0001
  * $v_level_1
  + pul freq=$v_speed_2 width=$v_pw_2 vsync=$v_vs_2 reset=0
      phase:
       sin freq=0.0003
    * $v_level_2
  + pul freq=$v_speed_3 width=$v_pw_3 vsync=$v_vs_3 reset=0
      phase:
       sin freq=0.0005
    * $v_level_3
  + pul freq=$v_speed_4 width=$v_pw_4 vsync=$v_vs_4 reset=0
      phase:
       sin freq=0.0007
    * $v_level_4
  + pul freq=$v_speed_5 width=$v_pw_5 vsync=$v_vs_5 reset=0
      phase:
       sin freq=0.0011
    * $v_level_5
  clp
