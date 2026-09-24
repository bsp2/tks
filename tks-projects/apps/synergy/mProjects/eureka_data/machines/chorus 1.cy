id chorus_1
author bsp

# declare param p_amp and mod m_amp
mp dry       0.5 0 1
mp wet       0.5 0 1
mp len       0.2 0 1
mp len_mod   0.2 0 1
mp frq       0.3 0 1
mp fb        0.1 0 1
mp slwup     0.1
mp slwdn     0.1
mp lfo_shape

<out:
  #sin
  #* $m_amp
  #  * 2
  #clp
  #vst v_in

  $m_frq
  clp 1 0
  fma 0.05 0.0001
  vst v_frq

  rdl
  dly size=8192 fb=$m_fb lin=1
    len:
      ipl win=linear
        a:
          tri fixed=1 phase=0.0 f=$v_frq
        b:
          pul fixed=1 phase=0.0 f=$v_frq
        t:
          $m_lfo_shape
          clp 1 0
      slw
        down:
          $m_slwdn
          clp 1 0
          pow 3
        up:
          $m_slwup
          clp 1 0
          pow 3
      fma 0.5 0.5
      * $m_len_mod
        clp 1 0
        * 0.02
      + $m_len
        clp 1 0
        * 0.04
        + 0.0004
  vst v_dly_l

  rdr
  dly size=8192 fb=$m_fb lin=1
    len:
      ipl win=linear
        a:
          tri fixed=1 phase=0.5 f=$v_frq
        b:
          pul fixed=1 phase=0.5 f=$v_frq
        t:
          $m_lfo_shape
      slw
        down:
          $m_slwdn
          clp 1 0
          pow 3
        up:
          $m_slwup
          clp 1 0
          pow 3
      fma 0.5 0.5
      * $m_len_mod
        clp 1 0
        * 0.02
      + $m_len
        clp 1 0
        * 0.04
        + 0.0004
  vst v_dly_r

  rdl
    * $m_dry
      clp 1 -1
  + $v_dly_l
    * $m_wet
      clp 1 -1
  fix
  wrl

  rdr
    * $m_dry
      clp 1 -1
  + $v_dly_r
    * $m_wet
      clp 1 -1
  fix
  wrr
