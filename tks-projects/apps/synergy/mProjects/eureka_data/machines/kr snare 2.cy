
wt_cyclelen 6500
wt_w        4
wt_h        1

arg p_amp 0.7 0 8

arg sin_freq 1 0.5 2
arg fun_freq_ms   200 1 500
arg fun_freq_amt  0.9 -2 2
arg fun_amp_ms 200 1 500
arg fun_lvl 1 0 3
arg noise_amp_ms 200 1 500
arg noise_bpf_min 0.5
arg noise_bpf_rng 0.25
arg noise_bpf_ms 200 1 500
arg noise_res 1
arg noise_lvl1 1 0 2
arg noise_lvl2 1 0 2
arg noise_seed $1324 0 $ffff

curve 0 fun_amp
curve 1 fun_freq
curve 2 noise_amp
curve 3 noise_bpf
curve 4 fun_ws
curve 5 noise_ws
curve 6 out_ws

var v_noise


<fundamental:
  sin
    f:
      $sin_freq
      + rmp $fun_freq_ms 0 0.99
        lut fun_freq lin=1
        * $fun_freq_amt
  * rmp $fun_amp_ms 0 1
    lut fun_amp lin=1 clamp=1
  * $fun_lvl
  tanh
  lut fun_ws b=1 lin=1

<noise:
  fsr seed=$noise_seed cycle=0
  * rmp $noise_amp_ms 0 1
    lut noise_amp lin=0 clamp=1
  svf mode=bpf r=$noise_res
    f:
      $noise_bpf_min
      + $noise_bpf_rng
        * rmp $noise_bpf_ms 0 0.99
          lut noise_bpf lin=1
  * $noise_lvl1
  tanh
  lut noise_ws b=1 lin=0
  * $noise_lvl2

<out: 
  * $p_amp
  tanh
  lut out_ws b=1 lin=1
  clp
