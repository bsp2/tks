
#ifeq ("$(OS)","Windows_NT")
#INSTALL_PATH:=/c/Program Files/tks/
#else
#INSTALL_PATH:=/usr/lib/tks/
#endif
INSTALL_PATH:=$(TKS_TARGET_SITE_PREFIX)


md.tkx: md.tks CycleSyntaxHighlighter_HTML.tks MinSyntaxHighlighter_HTML.tks builtin.css icons/*.png
	tks -c md

.PHONY: install
install: md.tkx
	chmod ugo+r md.tkx
	cp md.tkx "$(INSTALL_PATH)/applications/"

.PHONY: test
test: test.md test.css
	tks app:md test.md

.PHONY: gallery_test
gallery_test: gallery_test.md test.css
	tks md gallery_test.md

.PHONY: all
all: md.tkx test install
