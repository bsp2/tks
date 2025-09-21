
ALL_SRC= \
	NodeScript.tks \
	NodeScriptEditor.xfm \
	NodeScriptEditor.tks \
	node_script.tkp

node_script.tsl: $(ALL_SRC)
	tks -cl node_script && mv node_script.tsl ../libraries/

.PHONY: clean
clean:
	rm -f node_script.tsl
