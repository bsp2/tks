
rate 14000
freq 220 Hz

arg nos_lvl 0.25
arg body 0.75
arg body_f_ms 120 min=1 max=300
arg body_f_st 1.3 min=0 max=8
arg body_f_en 1.0 min=0 max=8
arg boost_ms 100
arg boost_st 0.3
arg boost_en 0.3

<out:
  nos
    * $nos_lvl
  + sin
      freq:
        rmp $body_f_ms $body_f_st $body_f_en
    * $body
 boo
   amount:
    rmp $boost_ms $boost_st $boost_en


