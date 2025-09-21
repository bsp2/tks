
plot.tsl: *.tks plot.tkp
	tks -cl plot

.PHONY: install
install: plot.tsl
	chmod ugo+r plot.tsl
	cp plot.tsl "$(TKS_SITE_PREFIX)/libraries/"
