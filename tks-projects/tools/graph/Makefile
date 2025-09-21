INSTALL_PATH:=$(TKS_TARGET_SITE_PREFIX)


graph.tkx: main.tks main.xfm graph.tkp doc/graph.css doc/res/*.png gr/*.gr doc/res/*.svg
	tks -c graph

.PHONY: install
install: graph.tkx
	chmod ugo+r graph.tkx
	cp graph.tkx "$(INSTALL_PATH)/applications/"
