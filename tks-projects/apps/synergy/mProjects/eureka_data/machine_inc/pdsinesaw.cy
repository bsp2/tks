# sine-saw phase distortion
#
#   usage:
#     inc pdsinesaw
#     ~pdsinesaw id=pdsw
#     <prepare:
#        prepare pdsw
#     <out:
#        sin
#          pd:
#            calc pdsw
#
#   see also: "sine-saw pd 1" example patch
#
#

def pdsinesaw
  global:
    arg a_%%_bend 0 -8 2
    var v_xexp

  prepare:
    1.0
    - $a_%%_bend
    fma 0.9 0.05
    sto v_xexp

  calc:
    + 0.5
    wrp m=0 x=1
    - 0.5
    * 2
    pow $v_xexp
    fma 0.5 0.5
