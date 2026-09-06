
plot-fs.tsl: *.tks plot-fs.tkp
	tks -cl plot-fs

.PHONY: install
install: plot-fs.tsl
	chmod ugo+r plot-fs.tsl
	cp plot-fs.tsl "$(TKS_SITE_PREFIX)/libraries/"

