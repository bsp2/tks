rate 44100 Hz
freq    35 Hz
dur    200 ms

<out:
  sin
    freq:
      1.0
      + rmp 100 start=1.8 end=0
      + rmp 500 start=1.2 end=0
  bit 0.2
  * rmp 10 10 1
  clp 1.0
  box
    freq:
      rmp 150 start=4 end=0.1
      clp
  * rmp 200 start=2 end=0
    clp
  + nos
    * rmp  8 start=4 end=0
    * rmp  2 start=1 end=0
#    clp
    box 0.8
    * 0.14
  clp

