
def sinedcy
  param p_%%_timescl 0.5   group=time   section=%%
  param p_%%_rate    0     group=rate   section=%%
  param p_%%_vsync   0     group=vsync  section=%%
  param p_%%_level   1     group=level  section=%%
  curve c_%%_amp

  calc:
    sin
      freq:
        $p_%%_rate
        fma 15 1
        qua 32
      vsync:
        $p_%%_vsync
        fma 15 1
        qua 4
    * rmp 2048 0 1
      * $p_%%_timescl
        bts 32 16
      lut c_%%_amp clamp=1 lin=1
    * $p_%%_level
