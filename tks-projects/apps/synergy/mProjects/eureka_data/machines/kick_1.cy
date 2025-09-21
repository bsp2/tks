rate 44100 Hz
freq   110 Hz
dur   1000 ms

<out:
  sin
    freq:
      1.0
      + rmp 130 start=1.5 end=0
  bit 0.5
  * 5.0
  clp 1.0
