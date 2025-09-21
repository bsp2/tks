id markov_sd_1
#oversample 2

param p_ab 0.1
param p_ba 0.1
param p_ab2 0.1
param p_ba2 0.1
#param p_ab3 0.1
#param p_ba3 0.1
param p_bpf 0.6
param p_noise_t 0.5
param p_body_t 0.5
param p_penv_amt 0.5

mod m_ab
mod m_ba
mod m_bpf
mod m_noise_t
mod m_body_t
mod m_penv_amt

var v_noise
var v_body
var v_out

<out:
  fsr seed=$abcd signed=0 reset=1
  mkv ab=$m_ab ba=$m_ba act=2
    a:
      fsr seed=$1234 signed=0 reset=0
      mkv a=-1 b=1 ab=$p_ab2 ba=$p_ba2 act=2
    b:
      fsr seed=$5678 signed=0 reset=0
      mkv a=-1 b=1 ab=$p_ab2 ba=$p_ba2 act=2
  * 1
    adsr a=1 d=0.5 ds=1.9 s=0
      scl:
        0.005
        * $m_noise_t
          fma 2 -1
          bts 4
    pow 1.3
  # bandpass-filter
  svf mode=2 freq=$m_bpf res=0.8
  sto v_noise

  sin
    freq:
      1
      adsr a=1 d=0.9 ds=1.9 s=0
        scl:
          0.005
          * $m_body_t
            fma 2 -1
            bts 2
      * 1.2
        * $m_penv_amt
          fma 4 -2
      + 0.5
  * 1
    adsr a=1 d=0.5 ds=1.7 s=0
      scl:
        0.001
        * $m_body_t
          fma 2 -1
          bts 4
    pow 2
  sto v_body

  $v_noise
  * 0.9
  + $v_body
    * 0.9
  clp
  