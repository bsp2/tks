def osc_sin_hdcy_1 freq=1 hdcy=0.25
  vst myosc_harm 1
  sin
    freq:
      %freq%
  * $myosc_harm
  mul myosc_harm %hdcy%
  + sin
      freq:
        %freq%
        + 1
    * $myosc_harm
    mul myosc_harm %hdcy%
  + sin
      freq:
        %freq%
        + 2
    * $myosc_harm
    mul myosc_harm %hdcy%
  + sin
      freq:
        %freq%
        + 3
    * $myosc_harm
    mul myosc_harm %hdcy%
