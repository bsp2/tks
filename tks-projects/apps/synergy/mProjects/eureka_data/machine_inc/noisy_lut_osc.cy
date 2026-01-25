#arg p_amp_1    0.7  0 2
#arg p_amp_2    0.2  0 2
#arg p_rndamt_1 0.11 0 1
#arg p_rndamt_2 0.23 0 1
#arg p_rndslw_1 0.5  0 1
#arg p_rndslw_2 0.3  0 1

def noisy_lut_osc freq=1 seed=0x5fa7 rndamt=0.01 rndslw=0.5
  lcurve wav

  calc:
    pha
      freq:
        %freq%
        + fsr seed=%seed%
          slw %rndslw% %rndslw%
          * %rndamt%
    lut wav lin=1
    #sin

#~noisy_lut_osc id=myosc1 freq=1    seed=0x5fa7 rndamt=$p_rndamt_1 rndslw=$p_rndslw_1
#~noisy_lut_osc id=myosc2 freq=1.5  seed=0xcd39 rndamt=$p_rndamt_2 rndslw=$p_rndslw_2
