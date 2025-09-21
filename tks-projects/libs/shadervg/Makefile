
shadervg.tsl: *.tks shadervg.tkp
	tks -cl shadervg

.PHONY: install
install: shadervg.tsl
	chmod ugo+r shadervg.tsl
	cp shadervg.tsl "$(TKS_SITE_PREFIX)/libraries/"
