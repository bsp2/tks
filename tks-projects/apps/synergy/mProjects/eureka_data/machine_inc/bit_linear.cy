# linear bit reduction
author bsp

def bit_linear bit=8
  vst p
  $p
  bit
    shift:
      %bit%
      itg
      * 1/15
  ipl
    b:
      $p
      bit
        shift:
         %bit%
         itg
         fam 1 1/15
    t:   
      %bit%
      frc
