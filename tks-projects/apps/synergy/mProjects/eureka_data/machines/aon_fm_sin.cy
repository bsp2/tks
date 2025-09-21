arg p_carrier_amp      0.7  0 8
arg p_carrier_amp_env  1    0 1
arg p_carrier_freq     0   -1 1
arg p_carrier_freq_env 1    0 1

arg p_mod_freq     0 -1  1
arg p_mod_freq_env 1  0  1
arg p_mod_amp      0  0 32
arg p_mod_amp_env  1  0  1

curve 0 carrier_freq
curve 1 carrier_amp
curve 2 mod_freq
curve 3 mod_amp

<out:
  sin
    freq:
      $p_carrier_freq
      + rmp 0 0 0.99
        lut carrier_freq
        * $p_carrier_freq_env
      bts 16
      + sin
          freq:
            $p_mod_freq
            + rmp 0 0 0.99
              lut mod_freq
              * $p_mod_freq_env
            bts 16
        * rmp 0 0 0.99
          lut mod_amp
          * $p_mod_amp_env
          + $p_mod_amp
  * rmp 0 0 0.99
    lut carrier_amp
    * $p_carrier_amp_env
    + $p_carrier_amp
  clp
