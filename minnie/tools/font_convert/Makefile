
TTF_NORMAL=../../../tkui/fonts/Tahoma.ttf
TTF_BOLD=../../../tkui/fonts/tahomabd.ttf

.PHONY: all
all:
	tks font_convert $(TTF_NORMAL) default normal 9
	tks font_convert $(TTF_NORMAL) default normal 18
	tks font_convert $(TTF_NORMAL) default normal 11
	tks font_convert $(TTF_NORMAL) default normal 22
	tks font_convert $(TTF_NORMAL) default normal 13
	tks font_convert $(TTF_NORMAL) default normal 26
	tks font_convert $(TTF_NORMAL) default normal 30
	tks font_convert $(TTF_BOLD)   default bold   13
	tks font_convert $(TTF_BOLD)   default bold   26
	tks font_convert $(TTF_NORMAL) default normal 17
	tks font_convert $(TTF_NORMAL) default normal 32
	tks font_convert $(TTF_NORMAL) default normal 34
	tks font_convert $(TTF_NORMAL) default normal 36
	tks font_convert $(TTF_NORMAL) default normal 24
	tks font_convert $(TTF_NORMAL) default normal 48
	tks font_convert $(TTF_NORMAL) default normal 64
	tks font_convert $(TTF_NORMAL) default normal 128

.PHONY: s256
s256:
	tks font_convert $(TTF_NORMAL) default normal 256

.PHONY: sdf512
sdf512:
	tks font_convert $(TTF_NORMAL) default normal 128 sdf 4

.PHONY: sdf256
sdf256:
	tks font_convert $(TTF_NORMAL) default normal 128 sdf 2

.PHONY: sdf128
sdf128:
	tks font_convert $(TTF_NORMAL) default normal 128 sdf

.PHONY: sdf96
sdf96:
	tks font_convert $(TTF_NORMAL) default normal 96 sdf

.PHONY: sdf64
sdf64:
	tks font_convert $(TTF_NORMAL) default normal 64 sdf

.PHONY: sdf48
sdf48:
	tks font_convert $(TTF_NORMAL) default normal 48 sdf

.PHONY: sdf36
sdf36:
	tks font_convert $(TTF_NORMAL) default normal 36 sdf

.PHONY: sdf32
sdf32:
	tks font_convert $(TTF_NORMAL) default normal 32 sdf

.PHONY: sdf
#sdf128 sdf96
sdf: sdf128 sdf64 sdf48 sdf36 sdf32
