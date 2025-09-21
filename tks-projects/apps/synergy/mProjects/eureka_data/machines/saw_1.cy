rate 44100 Hz 
freq   220 Hz 
dur    600 ms 

arg flt0 0.8
arg amp1 0.5

arg detune1 1.01 min=0.5 max=2
arg flt1 0.7 min=0 max=1
arg amp1 0.2 min=0 max=1

arg detune2 0.99 min=0.5 max=2
arg flt2 0.7 min=0 max=1
arg amp2 0.2 min=0 max=1

arg fltmain 0.5
 
<out: 
  saw 1.0 
    box $flt0
    * 0.5 
  + saw $detune1
    box $flt1
    * $amp1
  + saw $detune2
    box $flt2
    * $amp2
  box $fltmain
