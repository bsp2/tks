id rotate

# declare param p_amp and mod m_amp
mp amp      0.5  0 1
mp ang_off  0    0 1
mp f1       0.5  0 1
mp f2       0.5  0 1
mp fang     0.5  0 1
mp fang_amt 0    0 1

lut sintbl shared 1024
#lut costbl shared 1024

<lut_sintbl:
  sin ph=$x f=0

#<lut_costbl:
#  sin f=0
#    ph:
#      $x
#      + 0.25

<out:
  sin
    f:
      $m_f1
      fma 2 -1
      bts 6 6
  vst in1
  sin
    f:
      $m_f2
      fma 2 -1
      bts 6 6
  vst in2

  sin fixed=0
    f:
      $m_fang
      fma 2 -1
      bts 6 6
  * $m_fang_amt
  + $m_ang_off
  vst ang

  $ang
  lut sintbl lin=1
  vst sa
  #trc

  $ang
  + 0.25
  lut sintbl lin=1
  vst ca
  #trc

  $in1
  * $ca
  - $in2
    * $sa
  vst ox

  $in1
  * $sa
  + $in2
    * $ca
  vst oy

#  pha
#  lut sintbl lin=1

  #$ox
  #+ $oy

  wrl
    v:
      $ox
      * $amp
  wrr
    v:
      $oy
      * $amp

  #$ang
  #trc

  * $m_amp
  clp
