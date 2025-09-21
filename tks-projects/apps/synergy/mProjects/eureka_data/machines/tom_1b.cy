rate 14000
freq 60

arg freq_ms 200 min=10 max=1000 
arg freq_st 2.5 min=0.5 max=16
arg freq_en 1.0 min=0.5 max=16

arg freq2_ms 200 min=10 max=1000 
arg freq2_st 1.5 min=0.0625 max=4
arg freq2_en 1.0 min=0.0625 max=4

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
  * 0.75
  + sin 
      freq:
       rmp $freq2_ms $freq2_st $freq2_en
    * 0.25
  * 0.5
  + 0.5
  lut wshape
#  * rmp $amp_ms 0 0.999
#    lut amp
  + nos
    * rmp $nos_ms $nos_st $nos_en
