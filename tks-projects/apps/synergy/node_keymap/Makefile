
ALL_SRC= \
	NodeKeymap.tks \
	NKM_EntriesTM.tks \
	NodeKeymapEditor.xfm \
	NodeKeymapEditor.tks \
	node_keymap.tkp

node_keymap.tsl: $(ALL_SRC)
	tks -cl node_keymap && mv node_keymap.tsl ../libraries/

.PHONY: clean
clean:
	rm -f node_keymap.tsl
