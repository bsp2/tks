
ALL_SRC= \
	CM_CyclicMorphDialog.tks \
	CM_CyclicMorphDialog.xfm \
	NCM_EntriesTM.tks \
	NCM_GroupCopyDialog.tks \
	NCM_GroupCopyDialog.xfm \
	NCM_GroupCopyDialog_template.xfm \
	NodeCtlMorph.tks \
	NodeCtlMorphEditor.tks \
	NodeCtlMorphEditor.xfm \
	node_ctlmorph.tkp

node_ctlmorph.tsl: $(ALL_SRC)
	tks -cl node_ctlmorph && mv node_ctlmorph.tsl ../libraries/

.PHONY: clean
clean:
	rm -f node_ctlmorph.tsl
