
wt_w 3
wt_cyclelen 12000

arg p_amp 0.7 0 8

arg trig_ms 3   0 30
arg trig_exp 1  0 10

arg o_freq_env_ms  10 0 50
arg o_freq_env_exp 1  0 10
arg o_freq_env_lvl 0  0 1

arg o1_freq 0.5 0 1
arg o1_res  0.7 0 1
arg o1_lvl  1   0 1

arg o2_freq 0.5 0 1
arg o2_res  0.7 0 1
arg o2_lvl  1   0 1

arg nos_ms   50  0 500
arg nos_exp  1   0 10
arg nos_freq 0.5 0 1
arg nos_res  0.5 0 1
arg nos_lvl  0.5 0 1

arg p_tan_mix 0.5 0 1
arg p_tan_lpf 0.5 0 1

<out:
  rmp $o_freq_env_ms 1 0
  pow $o_freq_env_exp
  * $o_freq_env_lvl
  vst freq_env

  rmp $trig_ms 1 0
  pow $trig_exp
  vst trig
  svf
    r:
      $o1_res
      pow 3
    f:
      $o1_freq
      + $freq_env
      * 0.1
  * $o1_lvl
  + $trig
    svf
      r:
        $o2_res
        pow 3
      f:
        $o2_freq
        + $freq_env
        * 0.1
    * $o2_lvl
  + nos cycle=0 reset=0
    * rmp $nos_ms 1 0
    pow $nos_exp
    svf mode=hpf f=$nos_freq r=$nos_res
    * $nos_lvl
  * $p_amp
  ipl t=$p_tan_mix
    a:
      tan
      svf f=$p_tan_lpf
  clp
