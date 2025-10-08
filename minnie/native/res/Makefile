FONT_PATH:=../../tools/font_convert/out/

.PHONY: all
all: images fonts patterns

.PHONY: images
images: escher.c escher_alpha.c

.PHONY: patterns
patterns: pattern_1_alpha.c

.PHONY: fonts
fonts: font_default_30 font_default_256 font_default_256_sdf

#
# images
#

escher.raw: escher.png
	tks app:png_to_raw escher.png escher.raw

escher_alpha.raw: escher.png
	tks app:png_to_raw -a escher.png escher_alpha.raw

escher.c: escher.raw
	tks app:bin2c escher.raw -id tex_escher_argb32 -a -o escher.c

escher_alpha.c: escher_alpha.raw
	tks app:bin2c escher_alpha.raw -id tex_escher_alpha -a -o escher_alpha.c

#
# fonts
#

.PHONY:
font_default_30: $(FONT_PATH)default_30.png
	tks app:png_to_raw -a $(FONT_PATH)default_30.png font_default_30_tex.raw
	tks app:bin2c font_default_30_tex.raw -id font_default_30_tex -a -o font_default_30_tex.c
	tks app:bin2c $(FONT_PATH)default_30.bin -id font_default_30 -a -o font_default_30.c

.PHONY:
font_default_256: $(FONT_PATH)default_256.png
	tks app:png_to_raw -a $(FONT_PATH)default_256.png font_default_256_tex.raw
	tks app:bin2c font_default_256_tex.raw -id font_default_256_tex -a -o font_default_256_tex.c
	tks app:bin2c $(FONT_PATH)default_256.bin -id font_default_256 -a -o font_default_256.c

.PHONY:
font_default_256_sdf: $(FONT_PATH)default_256_sdf.png
	tks app:png_to_raw -a $(FONT_PATH)default_256_sdf.png font_default_256_sdf_tex.raw
	tks app:bin2c font_default_256_sdf_tex.raw -id font_default_256_sdf_tex -a -o font_default_256_sdf_tex.c
	tks app:bin2c $(FONT_PATH)default_256_sdf.bin -id font_default_256_sdf -a -o font_default_256_sdf.c

#
# patterns
#
pattern_1_alpha.raw: pattern_1.png
	tks app:png_to_raw -a pattern_1.png pattern_1_alpha.raw

pattern_1_alpha.c: pattern_1_alpha.raw
	tks app:bin2c pattern_1_alpha.raw -id tex_pattern_1_alpha -a -o pattern_1_alpha.c

.PHONY: clean
clean:
	rm -f ../res/*.raw *.o

.PHONY: realclean
realclean: clean
	rm -f ../res/*.c
