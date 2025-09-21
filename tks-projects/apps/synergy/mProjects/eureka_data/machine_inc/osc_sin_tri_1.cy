
# n = 2i + 1
# lvl(i) = pow(-1,i) * pow(n, -2)
#        = pow(-1,i) * pow(2*i + 1, -2)
# i=0: pow(-1,0) * pow(2*0 + 1, -2) = 1
# i=1: pow(-1,1) * pow(2*1 + 1, -2) = -0.111111111111
# i=2: pow(-1,2) * pow(2*2 + 1, -2) = 0.04
# i=3: pow(-1,3) * pow(2*3 + 1, -2) = -0.0204081632653
# i=4: pow(-1,4) * pow(2*4 + 1, -2) = 0.0123456790123
# i=5: pow(-1,5) * pow(2*5 + 1, -2) = -0.00826446280992
# i=6: pow(-1,6) * pow(2*6 + 1, -2) = 0.00591715976331
# i=7: pow(-1,7) * pow(2*7 + 1, -2) = -0.00444444444444

# (note) phase=0.75 aligns the triangle with the default "tri" osc
def osc_sin_tri_1 freq=1 phase=0.75 detune=0
  sin phase=%phase%
    freq:
      %freq%
  + sin 
      phase:
        %phase%
        * 3
          + %detune%
      freq:
        %freq%
        * 3 # 2*1+1
          + %detune%
    * -0.111111111111
  + sin
      phase:
        %phase%
        * 5
          + %detune%
      freq:
        %freq%
        * 5 # 2*2+1
          + %detune%
    * 0.04
  + sin
      phase:
        %phase%
        * 7
          + %detune%
      freq:
        %freq%
        * 7 # 2*3+1
          + %detune%
    * -0.0204081632653
  + sin
      phase:
        %phase%
        * 9
          + %detune%
      freq:
        %freq%
        * 9 # 2*4+1
          + %detune%
    * 0.0123456790123
  + sin
      phase:
        %phase%
        * 11
          + %detune%
      freq:
        %freq%
        * 11 # 2*5+1
          + %detune%
    * -0.00826446280992
  + sin
      phase:
        %phase%
        * 13
          + %detune%
      freq:
        %freq%
        * 13 # 2*6+1
          + %detune%
    * 0.00591715976331
  + sin
      phase:
        %phase%
        * 15
          + %detune%
      freq:
        %freq%
        * 15 # 2*7+1
          + %detune%
    * -0.00444444444444
  * 0.810569469139 # (8/pi^2)

