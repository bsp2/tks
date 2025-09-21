arg p_amp 0.7 0 8

curve 0 globalcurve

inc .mysin
inc .myosc
inc .patchui
inc .patch

#~myosc id=osc1
#~myosc id=osc2

#def uipatch
#  set osc1.in_freq 0.3
#  calc osc1
#  set osc2.in_phase osc1.out
#  set osc2.in_freq 1.5
#  calc osc2
#  osc2.out
#  #sin

<out:
  ~patch
  #sin f=0
  clp
