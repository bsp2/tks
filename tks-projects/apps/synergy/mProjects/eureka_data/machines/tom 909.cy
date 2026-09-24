# "the frequency ratios of the oscillators in the TR-909 low tom are VCO3:VCO1:VCO2 = 1:1.5:2.77"
#    - https://modwiggler.com/forum/viewtopic.php?t=222069


arg p_tune    1    0 4
arg p_ratio_1 1    0 4
arg p_ratio_2 1.5  0 4
arg p_ratio_3 2.77 0 4

arg p_amp_1 0.7 0 1
arg p_amp_2 0.3 0 1
arg p_amp_3 0.1 0 1
arg p_phase_1 0 0 1
arg p_phase_2 0 0 1
arg p_phase_3 0 0 1
arg p_noise_freq  0.5 0 1
arg p_noise_level 0   0 1
arg p_noise_decay 10  0 200
arg p_drive 1 0 2

curve 0 shape

<out:
  sin phase=$p_phase_1
    f:
      $p_tune
      * $p_ratio_1
  * $p_amp_1
    pow 3
  + sin phase=$p_phase_2
      f:
        $p_tune
        * $p_ratio_2
    * $p_amp_2
      pow 3
  + sin phase=$p_phase_3
      f:
        $p_tune
        * $p_ratio_3
    * $p_amp_3
      pow 3
  + fsr
    svf mode=bpf freq=$p_noise_freq
    * rmp $p_noise_decay 1 0
      pow 3
    * $p_noise_level
      pow 3
  * $p_drive
  clp
  * 0.5
  + 0.5
  lut shape lin=1
  * rmp 0 1
#  tan
