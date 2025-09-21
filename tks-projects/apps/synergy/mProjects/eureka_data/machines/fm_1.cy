rate 44100 Hz
freq   220 Hz
dur    400 ms

<out:
  sin
    phase:
      sin
        freq:
          rmp 100 1.75 1
      * 0.3
  * 0.85
  * rmp 400 3 0
    clp


#     box
#       freq:
#         rmp 200 1 0
#     box
#       freq:
#         rmp 100 1 0.2
#     box
#       freq:
#         rmp 50 1 0.5
