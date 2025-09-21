id chip_pwm_saw_1
oversample 4

param p_phase2 0
param p_level2 1
param p_box1 0.8
param p_box2 0.8

mod m_phase2
mod m_level2
mod m_box1
mod m_box2

<out:
  saw
  box $m_box1
  - saw phase=$m_phase2
    box $m_box2
    * $m_level2
  # boo 0.5
