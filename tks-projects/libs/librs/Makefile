
rs.tsl: *.tks rs.tkp
	tks -cl rs

.PHONY: install
install: rs.tsl
	chmod ugo+r rs.tsl
	cp rs.tsl "$(TKS_SITE_PREFIX)/libraries/"
