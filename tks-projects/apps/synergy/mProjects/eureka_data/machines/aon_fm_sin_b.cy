arg p_carrier_amp      0.7  0 8
arg p_carrier_amp_env  1    0 1
arg p_carrier_amp_ms   300  1 2000
arg p_carrier_freq     0   -1 1
arg p_carrier_freq_env 1    0 1
arg p_carrier_freq_ms  300  1 2000

arg p_mod_amp        0  0   32
arg p_mod_amp_env    1  0    1
arg p_mod_amp_ms   300  1 2000

arg p_mod_freq       0 -1     1
arg p_mod_freq_env   1  0     1
arg p_mod_freq_ms  300  1  2000

arg p_lin_lut 1 0 1

curve 0 carrier_freq
curve 1 carrier_amp
curve 2 mod_freq
curve 3 mod_amp

<out:
  sin
    freq:
      $p_carrier_freq
      + rmp $p_carrier_freq_ms 0 0.99
        lut carrier_freq clamp=1 lin=$p_lin_lut
        * $p_carrier_freq_env
      bts 16
      + sin
          freq:
            $p_mod_freq
            + rmp $p_mod_freq_ms 0 0.99
              lut mod_freq clamp=1 lin=$p_lin_lut
              * $p_mod_freq_env
            bts 16
        * rmp $p_mod_amp_ms 0 0.99
          lut mod_amp clamp=1 lin=$p_lin_lut
          * $p_mod_amp_env
          + $p_mod_amp
  * rmp $p_carrier_amp_ms 0 0.99
    lut carrier_amp clamp=1 lin=$p_lin_lut
    * $p_carrier_amp_env
    + $p_carrier_amp
  clp
