# mirror y

def lut2y lut=mylut lin=1 b=0 in=1
  * 2
  cge 1
    - 1
    lut %lut% lin=%lin% c=1 b=%b% in=%in%
    neg
   else:
    lut %lut% lin=%lin% c=1 b=%b% in=%in%
