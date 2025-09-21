id     osc_filter_afm3s_1
name   "osc filter afm3s 1"
author bsp
cat    osc

# - three lofi ops
# - op2 modulates op1
# - op1 through lpf
# - op3 modulates op1 level

oversample 16

param p_ratio2 0.5
param p_level2 0

param p_ratio3 0.5
param p_level3 0.5

mod m_ratio2
mod m_level2

mod m_ratio3
mod m_level3

var v_ratio2
var v_level2
var v_loglevel2

var v_ratio3
var v_level3
var v_loglevel3

lut logsinexp shared 256
lut logsin shared 256
lut exp shared 2048

<lut_logsinexp:
  # e
  2.71828182846
  pow
    exp:
      sin freq=0 phase=$x
      + 1
      clp floor=0.0000001 ceil=2
      log
  - 1
  #trc

<lut_logsin:
  sin freq=0 phase=$x
  # -1..1 => 0..2
  + 1
  clp floor=0.0000001 ceil=2
  log

<lut_exp:
  # 0..1 => -16.118095651 .. 4.85203026392
  2.71828182846
  pow
    exp:
      $x
      * 20.9701259149
      - 16.118095651
     

<prepare:
  $m_ratio2
  # 0..1 => -1..1
  fma 2 -1
  # -1..1 => /4..*64
  bts 4 64
  qua 32
  sto v_ratio2

  $m_level2
  * 5
  clp floor=0.0000001 ceil=64
  sto v_level2
  # (note) log(0.0000001)=-16.118095651
  log
  sto v_loglevel2

  $m_ratio3
  # 0..1 => -1..1
  fma 2 -1
  # -1..1 => /4..*64
  bts 4 64
  qua 100
  sto v_ratio3

  $m_level3
  * 1.5
  clp floor=0.0000001 ceil=64
  sto v_level3
  # (note) log(0.0000001)=-16.118095651
  log
  sto v_loglevel3

# (note) max logsin=log(1+1)=0.69314718056
# (note) max v_loglevel=log(64)=4.15888308336
# (note) max sum=4.85203026392
# (note) min sum=log(1)+log(0.0000001)=0-16.118095651
# (note) max tbl index 1.0 => 4.85203026392 + 16.118095651 => 20.9701259149
# e.g. 3.2*0.66 = exp(log(3.2+1)+log(0.66))-0.66 = 2.112
#   or -0.74*0.839 = exp(log(-0.74+1)+log(0.839))-0.839 = -0.62086
<out:
 pha
   phase:
     pha freq=$v_ratio2
     lut logsin
     + $v_loglevel2
     + 16.118095651
     * 1/20.9701259149
     lut exp
     - $v_level2
#     * 0
 lut logsin
 + pha freq=$v_ratio3
   lut logsin
   + $v_loglevel3
#   + 16.118095651
#   * 1/20.9701259149
 + 16.118095651
 * 1/20.9701259149
 lut exp
 - $v_level3

 #- $v_level3
