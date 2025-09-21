
ALL_SRC= \
	NodeProxy.tks \
	NodeProxyEditor.xfm \
	NodeProxyEditor.tks \
	node_proxy.tkp

node_proxy.tsl: $(ALL_SRC)
	tks -cl node_proxy && mv node_proxy.tsl ../libraries/

.PHONY: clean
clean:
	rm -f node_proxy.tsl
