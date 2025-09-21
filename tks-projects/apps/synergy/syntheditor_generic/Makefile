
ALL_SRC= \
	GenericSynthEdit.tks \
	GenericSynthEdit.xfm

syntheditor_generic.tsl: $(ALL_SRC)
	tks -cl syntheditor_generic && mv syntheditor_generic.tsl ../libraries/

.PHONY: clean
clean:
	rm -f syntheditor_generic.tsl
