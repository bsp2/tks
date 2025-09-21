
oct 0

curve 0 pitch
curve 3 amp

arg nos_ms 10 0 200
arg nos_lvl 0.1 0 1
arg pul_freq 0.75 0 8
arg nos_lpf 0.5 0 1

arg bpf 0.5 0 1
arg nos2_lvl 0.2 0 1

arg amp_ms 200 0 1000
arg amp_lvl 1.0 0 2

xfade 100


<out:
 nos
  * rmp $nos_ms 1 0
  * pul freq=$pul_freq phase=0.5
     * 0.5
     + 0.5
  box freq=$nos_lpf
  * $nos_lvl
  + nos
     svf mode=2
      freq:
        $bpf
     * $nos2_lvl
  * rmp $amp_ms 0 0.93
    lut amp
    * $amp_lvl
 clp
