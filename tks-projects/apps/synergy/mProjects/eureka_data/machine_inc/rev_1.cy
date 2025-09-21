#
# a variation of the classic schroeder reverb algorithm
#  (note) uses *serial*, dry/wet balanced, and bandpass-filterd delay lines
#
author bsp

def rev_1_dly n=0.5 fb=0.5 c=0 lpf=0.07 hpf=0.7
  lpf %lpf%
  hbx %hpf%
  dly s=4096 n=%n% lin=1 fb=%fb%
   r:
     apf c=%c%

def rev_1 t=0.85 lpf=0.07 hpf=0.7 amp=8 mix=0.5
  global:
    var v_fb%%
  . 0.7
    + %t%
      * 0.29
    sto v_fb%%
  ipl t=%mix%
    b:
      pre
      + ~rev_1_dly n=0.348046875 lpf=%lpf% hpf=%hpf% fb=$v_fb%% c=-0.67139
        * 0.05099 #p_1_mix
          * %amp%
      + ~rev_1_dly n=0.371484375 lpf=%lpf% hpf=%hpf% fb=$v_fb%% c=-0.85269
        * 0.08499 #p_2_mix
          * %amp%
      + ~rev_1_dly n=0.481640625 lpf=%lpf% hpf=%hpf% fb=$v_fb%% c=-0.42776
        * 0.14731 #p_3_mix
          * %amp%
      + ~rev_1_dly n=0.512109375 lpf=%lpf% hpf=%hpf% fb=$v_fb%% c=-0.72805
        * 0.12748 #p_4_mix
          * %amp%
