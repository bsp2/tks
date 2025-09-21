# AON-style frequency modulation (not phase modulation!)
#  requires "aon_fm_oscwaves" sample to be loaded

arg p_carrier_amp      0.7  0 8
arg p_carrier_amp_env  1    0 1     group=amp
arg p_carrier_amp_ms   0    1 2000  group=amp 
arg p_carrier_freq     0   -1 1     group=freq
arg p_carrier_freq_env 0   -1 1     group=freq
arg p_carrier_freq_ms  0    1 2000  group=freq

arg p_mod_amp      0  0 32    group=amp
arg p_mod_amp_env  1  0  1    group=amp
arg p_mod_amp_ms   0  1 2000  group=amp
arg p_mod_freq     0 -1  1    group=freq
arg p_mod_freq_env 0 -1  1    group=freq
arg p_mod_freq_ms  0  1 2000  group=freq

arg p_carrier_wave 0  0 11  group=shape
arg p_mod_wave     0  0 11  group=shape

#arg p_hpf_cfg 0 0 13
arg p_hpf 0 0 1
arg p_hpf_res 0 0 1

arg p_lin     1 0 1
arg p_lin_lut 1 0 1

arg_values p_carrier_wave 0="Sinus" 1="Triangle" 2="Turnpoint" 3="Parabola" 4="Extremes" 5="Up&Down" 6="LineMix" 7="M-Shape" 8="Heart" 9="Sawtooth" 10="Sample" 11="DoubleSinus"
arg_values p_mod_wave     0="Sinus" 1="Triangle" 2="Turnpoint" 3="Parabola" 4="Extremes" 5="Up&Down" 6="LineMix" 7="M-Shape" 8="Heart" 9="Sawtooth" 10="Sample" 11="DoubleSinus"

curve 0 carrier_freq
curve 1 carrier_amp
curve 2 mod_freq
curve 3 mod_amp

# (note) 12 sub-waves (128 frames per cycle)
zone 0 aon_fm_oscwaves.all
zone 1 aon_fm_oscwaves.all

<out:
  zlp 0 div=12 sub=$p_carrier_wave lin=$p_lin
    rate:
      $p_carrier_freq
      + rmp $p_carrier_freq_ms 0 0.99
        lut carrier_freq clamp=1 lin=$p_lin_lut
        * $p_carrier_freq_env
      bts 16
      + zlp 1 div=12 sub=$p_mod_wave lin=$p_lin
          rate:
            $p_mod_freq
            + rmp $p_mod_freq_ms 0 0.99
              lut mod_freq clamp=1 lin=$p_lin_lut
              * $p_mod_freq_env
            bts 16
        * rmp $p_mod_amp_ms 0 0.99
          lut mod_amp clamp=1 lin=$p_lin_lut
          * $p_mod_amp_env
          * $p_mod_amp
  * rmp $p_carrier_amp_ms 0 0.99
    lut carrier_amp clamp=1 lin=$p_lin_lut
    * $p_carrier_amp_env
    * $p_carrier_amp
  clp
#  hpf cfg=$p_hpf_cfg
  svf mode=1 freq=$p_hpf res=$p_hpf_res
  svf mode=1 freq=$p_hpf res=$p_hpf_res
  svf mode=1 freq=$p_hpf res=$p_hpf_res
  svf mode=1 freq=$p_hpf res=$p_hpf_res
  clp
