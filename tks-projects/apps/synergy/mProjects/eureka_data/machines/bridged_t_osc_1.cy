id bridged_t_osc_1

# declare param p_amp and mod m_amp
modparam dcy        0.99
modparam trig_level 0.25
modparam bpf_freq   0.47
modparam bpf_res    0.01
modparam dcy2       0
modparam drive      0
modparam drivemix   0

var v_trig
#oversample 8

<init:
  set v_trig 1

<out:
  $v_trig
  *sr $dcy
      fma 0.3 0.7
  sto v_trig
  * $trig_level
    * 4
  svf mode=bpf sr=0
    res:
      $bpf_res
      * 0.2
    freq:
      $bpf_freq
      kbd
    postlpf:
      #lle
      #  c:
      #    $color
      #    * -0.02
      #    clp 0 -0.04
      * 1
        - $dcy2
          * 0.01
  vst out
  ipl t=$drivemix
    b:
      $out
      * $drive
        fma 2 -1
        bts 16 2
      tanh
#  clp
