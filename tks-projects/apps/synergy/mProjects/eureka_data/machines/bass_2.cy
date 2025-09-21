# todo: optimize

rate 14000
freq 60

arg freq_ms 200 min=10 max=1000 
arg freq_st 1.1 min=0.5 max=4
arg freq_en 1.0 min=0.5 max=4

arg fm_freq 4.0
arg phase_ms 200 min=10 max=1000 
arg phase_st 1 min=0 max=1
arg phase_en 0 min=0 max=1

arg sin_lvl 0.5

arg tri_freq 2.0
arg tri_lvl 0.25

arg nos_ms 200 min=10 max=1000
arg nos_st 0.2
arg nos_en 0
arg amp_ms 200 min=10 max=1000

curve 0 wshape
curve 1 amp

<out:
  sin
   freq:
     rmp $freq_ms $freq_st $freq_en
   phase:
     sin $fm_freq
     * rmp $phase_ms $phase_st $phase_en
  * $sin_lvl
  + tri $tri_freq
    * $tri_lvl     
  * 0.5
  + 0.5
  lut wshape
  * rmp $amp_ms 0 0.999
    lut amp
#  + nos
#    * rmp $nos_ms $nos_st $nos_en
