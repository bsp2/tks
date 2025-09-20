#
# rebuild mib/*.mib from min/*.min:
#
#   for SW-AA rasterizer:
#    $ make clean ; make AA=y
#
#   for OpenGL edge-AA shader:
#    $ make clean ; make AA=n
#
#

VPATH= %.min min

ALL_MIB= \
   mib/test000_rect.mib       \
   mib/test001_lines.mib      \
   mib/test002_fill.mib       \
   mib/test003_concave.mib    \
   mib/test004_fill.mib       \
   mib/test005_concave.mib    \
   mib/test006_cubic.mib      \
   mib/test007_svg.mib        \
   mib/test008_ellipse.mib    \
   mib/test009_cake.mib       \
   mib/test010_d2d.mib        \
   mib/test011_cursor.mib     \
   mib/test012_stars.mib      \
   mib/test013_reverse.mib    \
   mib/test014_mask.mib       \
   mib/test015_minnie.mib     \
   mib/test016_aa.mib         \
   mib/test017_minnie_aa.mib  \
   mib/test018_cake_aa.mib    \
   mib/test019_3d_aa.mib      \
   mib/test020_clip2d.mib     \
   mib/test021_clip2d.mib     \
   mib/test022_clip2d.mib     \
   mib/test023_clip2d.mib     \
   mib/test024_clip2d.mib     \
   mib/test025_clip2d.mib     \
   mib/test026_clip2d.mib     \
   mib/test027_clip2dfb.mib   \
   mib/test028_3d.mib         \
   mib/test029_3d.mib         \
   mib/test030_minnie_3d.mib  \
   mib/test031_tf2dclip.mib   \
   mib/test032_subpath.mib    \
   mib/test033_valentines.mib \
   mib/test034_ducky.mib      \
   mib/test035_bicycle.mib    \
   mib/test036_crab.mib       \
   mib/test037_logo.mib       \
   mib/test038_logo_noaa.mib  \
   mib/test039_wildboar.mib   \
   mib/test040_tiger.mib      \
   mib/test041_rabbit.mib     \
   mib/test042_oil_lamp.mib   \
   mib/test043_elefant.mib    \
   mib/test044_linejoin_miterlimit.mib \
   mib/test045_linejoin.mib       \
   mib/test046_evenodd.mib        \
   mib/test047_shapes.mib         \
   mib/test048_nano.mib           \
   mib/test049_debug.mib          \
   mib/test050_world.mib          \
   mib/test051_rect.mib           \
   mib/test052_ellipse.mib        \
   mib/test053_elephant.mib       \
   mib/test054_365845.mib         \
   mib/test055_dragon_986054.mib  \
   mib/test056_satellite.mib      \
   mib/test057_lightbulb.mib      \
   mib/test058_motorbike.mib      \
   mib/test059_abstractbike.mib   \
   mib/test060_watermelon.mib     \
   mib/test061_man.mib            \
   mib/test062_gorilla.mib        \
   mib/test063_quill.mib          \
   mib/test064_fairy.mib          \
   mib/test065_fish.mib           \
   mib/test066_cat.mib            \
   mib/test067_reaper.mib         \
   mib/test068_pidgeon.mib        \
   mib/test069_bluejay.mib        \
   mib/test070_fox1.mib           \
   mib/test071_fox2.mib           \
   mib/test072_fox3.mib           \
   mib/test073_cookie.mib         \
   mib/test074_roundrect.mib      \
   mib/test075_roundrect_svg.mib  \
   mib/test076_hamster.mib        \
   mib/test077_giraffe.mib        \
   mib/test078_butterfly.mib      \
   mib/test079_minimal.mib        \
   mib/test080_debug_l1.mib       \
   mib/test081_debug_l2.mib       \
   mib/test082_circle.mib         \
   mib/test083_ellipse_fill.mib   \
   mib/test084_ellipse_stroke.mib \
   mib/test085_ellipse_fill_stroke.mib \
   mib/test086_rect_fill.mib      \
   mib/test087_rect_fill_stroke.mib \
   mib/test088_rect_stroke.mib    \
   mib/test089_roundrect_fill_stroke.mib \
   mib/test090_roundrect_fill.mib \
   mib/test091_roundrect_stroke.mib


# (note) paths are reversed when (SW-)AA is enabled
ifeq ("$(REV)","y")
override REV="REV=y"
else ifeq ("$(REV)","n")
override REV="REV=n"
else
override REV=n
endif

IC=/Users/bsp/scripts/iterm2/imgcat

mib/%.mib : %.min
#	@echo "AA=$(AA)"
	tks mib $< $@ $(AA)

.PHONY: all
all:
	make clean ; make convert

.PHONY: convert
convert: $(ALL_MIB)
	ls -ld mib/*.mib

.PHONY: clean
clean:
	rm -f mib/*.mib

.PHONY: md
md:
	make -C doc md

.PHONY: install
install:
	tks -c mib && mv mib.tkx "$(TKS_SITE_PREFIX)/applications/"

.PHONY: view_pngs
view_pngs:
	. ./view_pngs.sh

.PHONY: tiger_debug
tiger_debug:
	rm mib/test040_tiger.mib; make mib/test040_tiger.mib ; tks draw 40

.PHONY: debug
debug:
#	rm mib/test002_fill.mib; make mib/test002_fill.mib ; tks draw 2
#	rm mib/test003_concave.mib; make mib/test003_concave.mib ; tks draw 3
#	rm mib/test005_concave.mib; make mib/test005_concave.mib; tks draw 5
#	rm mib/test010_d2d.mib; make mib/test010_d2d.mib ; tks draw 10
#	rm mib/test011_cursor.mib; make mib/test011_cursor.mib ; tks draw 11
#	rm mib/test012_stars.mib; make mib/test012_stars.mib ; tks draw 12
#	rm mib/test035_bicycle.mib; make mib/test035_bicycle.mib ; tks draw 35
#	rm mib/test039_wildboar.mib; make mib/test039_wildboar.mib REV=y ; tks draw 39
#	rm mib/test040_tiger.mib; make mib/test040_tiger.mib ; tks draw 40
#	rm mib/test044_linejoin_miterlimit.mib; make mib/test044_linejoin_miterlimit.mib ; tks draw 44
#	rm mib/test045_linejoin.mib; make mib/test045_linejoin.mib ; tks draw 45
#	rm mib/test046_evenodd.mib; make mib/test046_evenodd.mib ; tks draw 46
#	rm mib/test047_shapes.mib; make mib/test047_shapes.mib ; tks draw 47
#	rm mib/test048_nano.mib; make mib/test048_nano.mib ; tks draw 48
#	rm mib/test049_debug.mib; make mib/test049_debug.mib ; tks draw 49
#	rm mib/test050_world.mib; make mib/test050_world.mib ; tks draw 50
#	rm mib/test053_elephant.mib; make mib/test053_elephant.mib ; tks draw 53
#	rm mib/test054_365845.mib; make mib/test054_365845.mib ; tks draw 54
#	rm mib/test055_dragon_986054.mib; make mib/test055_dragon_986054.mib ; tks draw 55
#	rm mib/test056_satellite.mib; make mib/test056_satellite.mib ; tks draw 56
#	rm mib/test057_lightbulb.mib; make mib/test057_lightbulb.mib ; tks draw 57
#	rm mib/test058_motorbike.mib; make mib/test058_motorbike.mib ; tks draw 58
#	rm mib/test059_abstractbike.mib; make mib/test059_abstractbike.mib ; tks draw 59
#	rm mib/test060_watermelon.mib; make mib/test060_watermelon.mib ; tks draw 60
#	rm mib/test061_man.mib; make mib/test061_man.mib ; tks draw 61
#	rm mib/test062_gorilla.mib; make mib/test062_gorilla.mib ; tks draw 62
#	rm mib/test063_quill.mib; make mib/test063_quill.mib ; tks draw 63
#	rm mib/test064_fairy.mib; make mib/test064_fairy.mib ; tks draw 64
#	rm mib/test065_fish.mib; make mib/test065_fish.mib ; tks draw 65
#	rm mib/test066_cat.mib; make mib/test066_cat.mib ; tks draw 66
#	rm mib/test067_reaper.mib; make mib/test067_reaper.mib ; tks draw 67
#	rm mib/test068_pidgeon.mib; make mib/test068_pidgeon.mib ; tks draw 68
#	rm mib/test069_bluejay.mib; make mib/test069_bluejay.mib ; tks draw 69
#	rm mib/test070_fox1.mib; make mib/test070_fox1.mib ; tks draw 70
#	rm mib/test071_fox2.mib; make mib/test071_fox2.mib ; tks draw 71
#	rm mib/test072_fox3.mib; make mib/test072_fox3.mib ; tks draw 72
#	rm mib/test073_cookie.mib; make mib/test073_cookie.mib ; tks draw 73
#	rm mib/test074_roundrect.mib; make mib/test074_roundrect.mib ; tks draw 74
#	rm mib/test075_roundrect_svg.mib; make mib/test075_roundrect_svg.mib ; tks draw 75
#	rm mib/test076_hamster.mib; make mib/test076_hamster.mib ; tks draw 76
#	rm mib/test077_giraffe.mib; make mib/test077_giraffe.mib ; tks draw 77
#	rm mib/test078_butterfly.mib; make mib/test078_butterfly.mib ; tks draw 78
#	rm mib/test079_minimal.mib; make mib/test079_minimal.mib ; tks draw 79
#	rm mib/test080_debug_l1.mib; make mib/test080_debug_l1.mib ; tks draw 49
#	rm mib/test081_debug_l2.mib; make mib/test081_debug_l2.mib ; tks draw 81
#	rm mib/test082_circle.mib; make mib/test082_circle.mib ; tks draw 82
#	rm mib/test083_ellipse_fill.mib; make mib/test083_ellipse_fill.mib #; tks draw 83
#	rm mib/test084_ellipse_stroke.mib; make mib/test084_ellipse_stroke.mib #; tks draw 84
#	rm mib/test085_ellipse_fill_stroke.mib; make mib/test085_ellipse_fill_stroke.mib #; tks draw 85
#	rm mib/test086_rect_fill.mib; make mib/test086_rect_fill.mib #; tks draw 86
#	rm mib/test087_rect_fill_stroke.mib; make mib/test087_rect_fill_stroke.mib #; tks draw 87
#	rm mib/test088_rect_stroke.mib; make mib/test088_rect_stroke.mib #; tks draw 88
#	rm mib/test089_roundrect_fill_stroke.mib; make mib/test089_roundrect_fill_stroke.mib #; tks draw 89
#	rm mib/test090_roundrect_fill.mib; make mib/test090_roundrect_fill.mib #; tks draw 90
	rm mib/test091_roundrect_stroke.mib; make mib/test091_roundrect_stroke.mib #; tks draw 91
