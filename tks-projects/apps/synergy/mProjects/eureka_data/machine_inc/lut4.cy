# seg1: normal
# seg2: mirror x
# seg3: mirror y
# seg4: mirror x+y

def lut4 lut=mylut lin=1 b=0 in=1
  * 4
  cge 3
    - 4
    neg
    lut %lut% lin=%lin% c=1 b=%b% in=%in%
    neg
   else:
    cge 2
      - 2
      lut %lut% lin=%lin% c=1 b=%b% in=%in%
      neg
     else:
      cge 1
        - 2
        neg
        lut %lut% lin=%lin% c=1 b=%b% in=%in%
       else:
        lut %lut% lin=%lin% c=1 b=%b% in=%in%
