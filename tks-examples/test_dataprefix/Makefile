
.PHONY: all
all: app


.PHONY: lib
lib:
	rm -f mylib.tsl
	tks -cl mylib


.PHONY: app
app: lib
	rm -f myapp.tkx
	tks -c myapp
