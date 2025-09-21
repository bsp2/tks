
arg p_fsr_seed $4489 0 65535
arg p_fsr_bits 1 0 1
arg p_fsr_shift_1  7 -15 15
arg p_fsr_shift_2  -9 -15 15
arg p_fsr_shift_3  13 -15 15
arg p_fsr_lvl  1 0 1
arg p_nos_seed $3d9f 0 65535
arg p_nos_lvl  1 0 1
arg p_hpf_freq 0.5 0 1
arg p_hpf_res  0.5 0 1

<out:
  fsr seed=$p_fsr_seed bits=$p_fsr_bits shift1=$p_fsr_shift_1 shift2=$p_fsr_shift_2 shift3=$p_fsr_shift_3
  * $p_fsr_lvl
  + nos seed=$p_nos_seed
    * $p_nos_lvl
  svf mode=1 freq=$p_hpf_freq res=$p_hpf_res

