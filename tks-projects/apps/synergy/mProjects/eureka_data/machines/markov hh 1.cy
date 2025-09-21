id markov_hh_1
#oversample 4

param p_ab 0.1
param p_ba 0.1

mod m_ab
mod m_ba

var v_out

<out:
  fsr signed=0 reset=0
  mkv a=-1 b=1 ab=$m_ab ba=$m_ba act=0
