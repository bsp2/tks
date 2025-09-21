id     osc_lofi_fm2s_1
name   "osc lofi fm2s 1"
author bsp
cat    osc

oversample 8

param p_ratio 0.5
param p_level 0

mod m_ratio
mod m_level

var v_ratio
var v_level
var v_loglevel

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
      ##* 64
      ##clp floor=0.0000001 ceil=64
      ### 0..1 => log(0.0000001) => (log(1)+log(64))
      ##log
      * 20.9701259149
      - 16.118095651
     

<prepare:
  $m_ratio
  # 0..1 => -1..1
  fma 2 -1
  # -1..1 => /4..*64
  bts 4 64
  sto v_ratio

  $m_level
  * 16
  clp floor=0.0000001 ceil=64
  sto v_level
  # (note) log(0.0000001)=-16.118095651
  log
  sto v_loglevel

<out:
 pha
   phase:
     pha freq=$v_ratio
     # (note) max logsin=log(1+1)=0.69314718056
     lut logsin
     # (note) max v_loglevel=log(64)=4.15888308336
     + $v_loglevel
     # (note) max sum=4.85203026392
     # (note) min sum=log(1)+log(0.0000001)=0-16.118095651
     # (note) max tbl index 1.0 => 4.85203026392 + 16.118095651 => 20.9701259149
     + 16.118095651
     * 1/20.9701259149
     lut exp
     # e.g. 3.2*0.66 = exp(log(3.2+1)+log(0.66))-0.66 = 2.112
     #   or -0.74*0.839 = exp(log(-0.74+1)+log(0.839))-0.839 = -0.62086
     - $v_level
 lut logsinexp


