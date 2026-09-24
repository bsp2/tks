inc lut4
inc tri_hs

arg p_amp 0.7 0 8

curve user_curve
lcurve wav
lcurve mylcurve

<out:
  pha
  ~lut4 wav
  lut mylcurve lin=1 b=1 c=1
  * $p_amp
  clp
