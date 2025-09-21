# same as kick_2 but with named args

rate 44100 Hz
freq    35 Hz
dur    200 ms

arg freq_rmp1_ms 100
arg freq_rmp2_ms 500
arg bitcrush 0.2
arg transient_ms 10
arg transient_amp 10
arg flt_env_ms 150
arg flt_env_amp 4 min=1 max=4
arg flt_env_sus 0.1
arg vol_env_ms 200
arg noise_flt 0.8
arg noise_lvl 0.14

<out:
  sin
    freq:
      1.0
      + rmp $freq_rmp1_ms start=1.8 end=0
      + rmp $freq_rmp2_ms start=1.2 end=0
  bit $bitcrush
  * rmp $transient_ms $transient_amp 1
  clp 1.0
  box
    freq:
      rmp $flt_env_ms start=$flt_env_amp end=$flt_env_sus
      clp
  * rmp $vol_env_ms start=2 end=0
    clp
  + nos
    * rmp  8 start=4 end=0
    * rmp  2 start=1 end=0
#    clp
    box $noise_flt
    * $noise_lvl
  clp


