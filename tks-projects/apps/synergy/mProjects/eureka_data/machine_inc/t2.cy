
# (note) phase=0.75 aligns the triangle with the default "tri" osc
def osc_sin_tri_1_lut freq=1 phase=0.75

  # (note) lines inside the "global:" section are placed outside the current lane
  global:
    lut osc_sin_tri_1_wave%% shared 16384

    # (note) the lut init lane is inserted before the current lane
    <lut_osc_sin_tri_1_wave%%:
      sin freq=0 phase=$x
      #tri

  # (note) the following lines are inserted into the current lane (which instantiates the template)
  pha freq=%freq%
  lut osc_sin_tri_1_wave%%
