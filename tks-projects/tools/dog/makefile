
dog.tkx: *.tks dog.tkp
	tks -c dog

.PHONY: install
install: dog.tkx
	chmod ugo+r dog.tkx
	cp dog.tkx "$(TKS_SITE_PREFIX)/applications/"

.PHONY: run_test
run_test:
	(cd tests/ ; rm -f _dog*.html .dog_cache; tks -ee test.tkp >test.ee ; tks app:dog test.ee )

