
#all:
#	tks -cl debugtext
#	cp debugtext.tsl /c/Program\ Files/tks/libraries/

debugtext.tsl: *.tks debugtext.tkp
	tks -cl debugtext

.PHONY: install
install: debugtext.tsl
	chmod ugo+r debugtext.tsl
	cp debugtext.tsl "$(TKS_SITE_PREFIX)/libraries/"
