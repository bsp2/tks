
ALL_SRC= \
	NodeBuffer.tks \
	NB_OutputForm.xfm \
	NB_OutputForm.tks \
	NB_ArpNotesTM.tks \
	NodeBufferEditor.xfm \
	NodeBufferEditor.tks \
	node_buffer.tkp

node_buffer.tsl: $(ALL_SRC)
	tks -cl node_buffer && mv node_buffer.tsl ../libraries/

.PHONY: clean
clean:
	rm -f node_buffer.tsl
