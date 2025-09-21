# https://till.com/articles/wavepalette/
#  [bsp] added detuning and harmonic decay
def evenangle_hs_det rep=128 f=1
  global:
    mp ead%%_fmul 0.5
    mp ead%%_dcy  0.5
    var v_freq_mul%%
    var v_dcy%%

  <prepare: 
    $ead%%_fmul
    fma 2 -1
    * %rep%
      rcp
    bts 128 128
    sto v_freq_mul%%

    1
    - pow b=2
        e:
          $ead%%_dcy
          clp 1 0
          * -8
    sto v_dcy%%

  vst v_s    2
  vst v_freq 1
  vst v_lvl  1
  sin f=%f%
  rep %rep%
    + sin
        f:
          $v_s
          * %f%
          * $v_freq
      * $v_s
        pow
        rcp
      * $v_lvl
    inc v_s 2
    mul v_freq $v_freq_mul%%
    mul v_lvl $v_dcy%%
