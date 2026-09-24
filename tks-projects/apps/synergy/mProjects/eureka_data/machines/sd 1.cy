# "the frequency ratios of the oscillators in the TR-909 low tom are VCO3:VCO1:VCO2 = 1:1.5:2.77"
#    - https://modwiggler.com/forum/viewtopic.php?t=222069

wt_w 5
wt_cyclelen 24000


arg p_tune    1    0 4
arg p_ratio_1 1    0 4
arg p_ratio_2 1.5  0 4
arg p_ratio_3 2.77 0 4

arg p_amp_1 0.7 0 1
arg p_amp_2 0.3 0 1
arg p_amp_3 0.1 0 1
arg p_osc_decay_1 70 0 1000
arg p_osc_decay_2 50 0 1000
arg p_osc_decay_3 30 0 1000
arg p_phase_1 0 0 1
arg p_phase_2 0 0 1
arg p_phase_3 0 0 1

arg p_noise_freq  0.5 0 1
arg p_noise_level 0   0 1
arg p_noise_decay 10  0 1000

arg p_noise_freq_2  0.5 0 1
arg p_noise_level_2 0   0 1
arg p_noise_decay_2 10  0 1000

arg p_hpf   0 0 13
arg p_boost 0 0  1
arg p_drive 1 0  2

curve 0 shape

<out:
  sin phase=$p_phase_1
    f:
      $p_tune
      * $p_ratio_1
  * rmp $p_osc_decay_1 1 0
    pow 3
  * $p_amp_1
    pow 3
  + sin phase=$p_phase_2
      f:
        $p_tune
        * $p_ratio_2
    * rmp $p_osc_decay_2 1 0
      pow 3
    * $p_amp_2
      pow 3
  + sin phase=$p_phase_3
      f:
        $p_tune
        * $p_ratio_3
    * $p_amp_3
      pow 3
    * rmp $p_osc_decay_3 1 0
      pow 4
  + fsr cycle=0
    svf mode=bpf freq=$p_noise_freq
    * rmp $p_noise_decay 1 0
      pow 5
    * $p_noise_level
      pow 3
  + fsr cycle=0 seed=$1234
    svf mode=bpf freq=$p_noise_freq_2
    * rmp $p_noise_decay_2 1 0
      pow 5
    * $p_noise_level_2
      pow 3
  * $p_drive
  hpf $p_hpf
  boo $p_boost
  clp
  * 0.5
  + 0.5
  lut shape lin=1
  * rmp 0 1
#  tan
