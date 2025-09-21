
ALL_SRC= \
	NodeMonoPoly.tks \
	NodeMonoPolyEditor.xfm \
	NodeMonoPolyEditor.tks \
	node_monopoly.tkp

node_monopoly.tsl: $(ALL_SRC)
	tks -cl node_monopoly && mv node_monopoly.tsl ../libraries/

.PHONY: clean
clean:
	rm -f node_monopoly.tsl
