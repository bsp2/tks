
rate 9000
freq 110

arg tri_freq 1.0 min=0.5 max=2
arg tri_lvl 0.25

arg tri2_freq 1.0 min=0.5 max=16
arg tri2_lvl 0.25

arg flt_ms 200 min=10 max=1000

arg amp_ms 200 min=10 max=1000
arg amp_lvl 1.0 min=0.25 max=16

curve 0 wshape
curve 1 amp
curve 2 wshape2

<out:
  + tri $tri_freq
    * $tri_lvl     
#  * tri $tri2_freq
#    * $tri2_lvl
#    abs
  * 0.4999
  + 0.5
  lut wshape
  box
    freq:
      rmp $flt_ms 1 0
  * rmp $amp_ms 0 0.999
    lut amp
  * $amp_lvl
  clp
  + 0
    * 0.4999
    + 0.5
    lut wshape2
