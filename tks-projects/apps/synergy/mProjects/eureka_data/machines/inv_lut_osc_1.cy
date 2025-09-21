id      inv_lut_osc_1
name   "inv lut osc 1"
author  bsp
cat     osc

arg p_fltkbdamt 0.25 -2 2

param p_res    0.5
param p_cutoff 0.4
param p_exp    5 1 15

mod m_cutoff
mod m_res
mod m_exp

lut ws voice 4096
lut ws_inv voice 4096

var v_cutoff;

<lut_ws:
  # transform
  fma 2 -1
  pow $m_exp

<lut_ws_inv:
  fma 2 -1
  pow
    exp:
      div 1.0 $m_exp  

<prepare:
  $m_cutoff
  kbd $p_fltkbdamt 0
  sto v_cutoff

<out:
   saw
   fma 0.49 0.5
   lut ws
   ##pow $exp
   svf freq=$v_cutoff res=$m_res
   clp
   ##pow
   ##  exp:
   ##    div 1.0 $exp  
   fma 0.49 0.5
   lut ws_inv
   svf freq=$v_cutoff res=$m_res
   clp
