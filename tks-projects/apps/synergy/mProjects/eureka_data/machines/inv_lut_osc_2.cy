id      inv_lut_osc_2
name   "inv lut osc 2"
author  bsp
cat     osc

arg p_fltkbdamt 0.25 -2 2

param p_cutoff  0.4
param p_res     0.5
param p_drive   0 0 3
param p_cutoff2 0.4
param p_res2    0.5
param p_vsync   0

mod m_cutoff
mod m_res
mod m_drive
mod m_cutoff2
mod m_res2
mod m_vsync

lut ws voice 4096
lut ws_inv voice 4096 inv

var v_cutoff
var v_drive
var v_cutoff2
var v_vsync

<lut_ws:
  # transform
  fma 2 -1
  * $v_drive
  tanh
  clp
  sta ws_inv 0.5 0.5 x 2 -1


<prepare:
  $m_cutoff
  kbd $p_fltkbdamt 0
  sto v_cutoff

  10
  pow $m_drive
  sto v_drive

  $m_cutoff2
  kbd $p_fltkbdamt 0
  sto v_cutoff2

  $m_vsync
  bts 16
  sto v_vsync

<out:
   saw vsync=$v_vsync window=1
   #svf freq=$v_cutoff2 res=$m_res2
   clp
   fma 0.5 0.5
   lut ws
   svf freq=$v_cutoff res=$m_res
   clp
   ##pow
   ##  exp:
   ##    div 1.0 $exp  
   fma 0.5 0.5
   lut ws_inv
   #svf freq=$v_cutoff res=$m_res
   #clp
