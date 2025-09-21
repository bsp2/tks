id sine_shaper_fm_1
oversample 16

param p_freq_c 0.5
param p_freq_m 0.5
param p_modamt 0.5
param p_phase_c 0
param p_phase_m 0

mod m_freq_c
mod m_freq_m
mod m_modamt
mod m_phase_c
mod m_phase_m

<out:
  sin freq=0
    phase:
      sin
        freq:
          $m_freq_m
          fma 2 -1
          bts 4 16
      * fma 0.5 0.5
        * 16
        * $m_modamt
      + $m_phase_m

  * sin phase=$m_phase_c
      freq:
        $m_freq_c
        fma 2 -1
        bts 4 16
