
ALL_SRC= \
	NodeDelay.tks \
	NodeDelayEditor.xfm \
	NodeDelayEditor.tks \
	node_delay.tkp

node_delay.tsl: $(ALL_SRC)
	tks -cl node_delay && mv node_delay.tsl ../libraries/

.PHONY: clean
clean:
	rm -f node_delay.tsl
