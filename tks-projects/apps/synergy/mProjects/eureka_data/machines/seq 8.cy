arg p_amp 1 0 8

map myseq
     freq
  0  C-5/C-5
  1  D#5/C-5
  2  D-6/C-5
  3  C-5/C-5
  4  G-5/C-5
  5  A#5/C-5
  6  F-4/C-5
  7  A#4/C-5

<out:
  # clock
  clk div=1/4 bpm=0
  vst v_clk

  # freq seq
  vsq xxxxxxxx cg=$v_clk lp=0
    s:
      map myseq.freq def=1 mode=lin
  vst v_freq

  # osc
  sin f=$v_freq
  * 0.9
  + sin ph=0.7
      f:
        $v_freq
        * 1.51
    * 0.25
  + sin ph=0.2
      f:
        $v_freq
        * 3.01
    * 0.01

  # amp env seq
  * tsq xxxxxxxx cg=$v_clk lp=0 dur=30
      adsr a=0.4 as=-3.5 d=0.8 s=0.2 r=1.5 rs=1.354
      slw 0.02 0.7

  # output amp    
  * $p_amp
  clp
