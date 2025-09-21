id pd_insert_1
oversample 4

param p_mod 0.5 0 1

mod m_mod

<out:
  sin
    pd:
      # phase input 0..1 => -1..1
      fma 2 -1
      pow
        exp:
          $m_mod
          # 0..1 => -1..1
          fma 2 -1
          # -1..1 => 1/4..*4
          bts 4 4
      # -1..1 => 0..1
      fma 0.5 0.5
  clp
