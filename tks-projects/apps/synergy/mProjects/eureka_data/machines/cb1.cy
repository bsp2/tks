rate 16574
freq 587
oct 0
xfade 128
#skip 64

curve 3 amp

arg detune 1.44 0 2

arg amp 0.3 min=0 max=4
arg amp_rng 0.4
arg amp_min 0.6
arg amp_ms 50 0 1000

<out:
  tri
  + tri freq=$detune
  * rmp $amp_ms 0 0.99
    lut amp
    * $amp_rng
    + $amp_min
  clp

