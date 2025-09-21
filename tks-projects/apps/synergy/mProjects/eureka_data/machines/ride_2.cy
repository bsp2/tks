rate 8000
freq 220

arg freq1 1.0 min=0.5 max=8
arg freq2 1.1 min=0.5 max=8
arg freq3 1.1 min=0.5 max=8
arg freq4 1.1 min=0.5 max=8
arg freq5 1.1 min=0.5 max=8
arg flt 0.3
arg flt_amp 1.0 min=0.5 max=4

var t

<out:
  pul $freq1 0 0.5
  * 0.2
  + pul $freq2 0 0.5
    * 0.2
  + pul $freq3 0 0.5
    * 0.2
  + pul $freq4 0 0.5
    * 0.2
  + pul $freq5 0 0.5
    * 0.2
  sto t
  box $flt
  r- $t
  * $flt_amp
#  * 0.25

