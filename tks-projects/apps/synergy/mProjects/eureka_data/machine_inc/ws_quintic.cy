#
# Quintic wave shaper
#
#  (note) ported from "fx_example/ws_quintic.c" voice plugin
#
#
def ws_quintic l0=0.001 l1=1 l2=1 l3=1 l4=1 l5=1 drive=2.0628
  # calc in^1..in^5
  vst x1
  * $x1
  vst x2
  * $x1
  vst x3
  * $x1
  vst x4
  * $x1
  vst x5
  # summed/weighted output
  %l0%
  + $x1
    *%l1%
  + $x2
    *%l2%
  + $x3
    *%l3%
  + $x4
    *%l4%
  + $x5
    *%l5%
  * %drive%
  tanh
