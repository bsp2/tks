# 3rd order polynomial sine approximation template

lut tmp_sin_p3_sin_q shared 128
lut tmp_sin_p3_sin   shared 512

<lut_tmp_sin_p3_sin_q:
  $x
  * 0.5
  * 3
  - $x
    * 0.5
    pow 3
    * 4.0
  
<lut_tmp_sin_p3_sin:
  * 4
  cge 3
    - 3
    r- 1
    lut tmp_sin_p3_sin_q
    neg
   else:
    cge 2
      - 2
      lut tmp_sin_p3_sin_q
      neg
     else:
      cge 1
        - 1
        r- 1
        lut tmp_sin_p3_sin_q
       else:
        lut tmp_sin_p3_sin_q
        #0


def sin_p3 f=1 vs=1 win=0 ph=0
  pha f=%f% vs=%vs% win=%win% ph=%ph%
  lut tmp_sin_p3_sin lin=1
