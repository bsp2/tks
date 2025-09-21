id phase_warp_2
#oversample 4

param p_pow 0.5
param p_vsync

mod m_pow
mod m_vsync

<out:
  ipl
    a:
      tri freq=0
        vsync:
          $m_vsync
          fma 15 1
          qua 4
        phase:
          pha
          fma 2 -1
          pow
            exp:
              $m_pow
              fma 2 -1
              bts 16 16
          fma 0.5 0.5
    b:
      tri
    t:
      sin freq=0.5
      pow 2


  # saw window=1 vsync=1.5

