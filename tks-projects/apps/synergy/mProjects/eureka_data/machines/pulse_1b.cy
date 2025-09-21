rate 44100 Hz
freq   440 Hz
dur   800 ms

<out:
  pul
    width:
      sin 0.01
      * 0.2
      + 0.5
  box
    freq:
      rmp 130 1 0.1

  + pul 2.01
      width:
        sin 0.0025
        * 0.2
        + 0.5
    box
      freq:
        rmp 150 1 0.125
    * 0.5

  + pul 1.99
      width:
        sin 0.0075
        * 0.2
        + 0.5
    box
      freq:
        rmp 190 1 0.7
    * 0.5
  * 0.5
