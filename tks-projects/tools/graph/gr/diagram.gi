// common diagram style classes

style def {
  bgcolor            = #eee
  shape              = round
  shape_round_amount = 20%
  shape_round_limit  = 5
  border_width       = 1.5
  pad_r              = 0
  ipad               = 3,4,3,4
  arrow_len          = 9
  arrow_ilen         = 20%
  arrow_angle        = 30
  edge_width         = 1.5
  align              = center
  // bg_pattern = line_1
}

style invis {
  def
  shape       = none
  label_style = none
  align       = left,centery
  pad_h       = 0
  s           = 0,16
  pad_l       = 0
}

style helper {
  invis
  pad     = 0
  s       = 4
  align   = center
  shape   = round
  bgcolor = #000
  helper  = true
}

style spacer {
  invis
  pad    = 0
  s      = 16,16
  helper = true
}

style lb {
  shape        = rect
  bgcolor      = #f9f9f9
  border_width = 0
  font         = small
  ipad         = 0
}

style rcurve {
  edge_shape = rcurve
}

style none {
}

layout       = fixed
xmajor       = true
zero_margin  = true
row_padding  = 8
col_padding  = 8
ipad         = 4
bgcolor      = #f9f9f9
border_shape = none
pad          = 0

label_class=lb
spacer_class=spacer
