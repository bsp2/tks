# fibonacci oscillator
#   'exp' is in range 0..1
#author bsp

def osc_fibonacci exp=0.1
  global:
   lut fib%% shared 64
   var fib_l 0
   var fib_c 0.00000001
   var fib_exp

   <lut_fib%%:
     $fib_l
     + $fib_c
     vst t
     set fib_l $fib_c
     set fib_c $t
     * 1/171677

   <prepare:
     %exp%
     clp 1 0
     pow
     fma 0.575 0.025
     sto fib_exp

  pha
    pd:
      fma
      pow $exp
      fam
  fma 2 -1
  lut fib%% lin=1 b=1
