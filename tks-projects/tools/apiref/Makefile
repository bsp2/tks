
.phony: all
all: test.h
	tks apiref -d --autodoc autodoc_test.ini --buffer-prefix test test.h -o autogen__apiref.md
	tks app:md -css apiref.css -il -o test.html test.md 
