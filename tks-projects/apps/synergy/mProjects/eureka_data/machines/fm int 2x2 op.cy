dither y

arg p_rmp_ms 250  0 2000
arg p_amp    0.75 0 8
arg p_mod_f  0.5  0 15
arg p_mod_l  0.5  0 4

arg p_rmp2_ms 250  0 2000
arg p_amp2    0.25 0 8
arg p_mod2_f  0.25 0 15
arg p_mod2_l  0.75 0 4

curve 0 amp
curve 1 mod

curve 2 amp2
curve 3 mod2

<out:
  rmp $p_rmp_ms 0 1
  vst v_rmp
  sin
    phase:
      sin f=$p_mod_f
      * $v_rmp
        lut mod l=1
      * $p_mod_l
  * $v_rmp
    lut amp l=1 c=1
    * $p_amp

  . rmp $p_rmp2_ms 0 1
    vst v_rmp2
  + sin
      phase:
        sin f=$p_mod2_f
        * $v_rmp2
          lut mod2 l=1
        * $p_mod2_l
    * $v_rmp2
      lut amp2 l=1 c=1
      * $p_amp2
    
  clp
