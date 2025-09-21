
ALL_SRC= \
	NAR_Defs.tks \
	NAR_EditKitDialog.tks \
	NAR_EditKitDialog.xfm \
	NAR_EditSoundDialog.tks \
	NAR_EditSoundDialog.xfm \
	NAR_KitTM.tks \
	NAR_MapActionTM.tks \
	NAR_MapEntryTM.tks \
	NAR_MapGroupTM.tks \
	NAR_MuteTM.tks \
	NAR_PatternTM.tks \
	NAR_PerfTM.tks \
	NAR_SaveSoundPresetDialog.tks \
	NodeAnalogRytm.tks \
	NodeAnalogRytmEditor.tks \
	NodeAnalogRytmEditor.xfm \
	node_analogrytm.tkp

node_analogrytm.tsl: $(ALL_SRC)
	tks -cl node_analogrytm && mv node_analogrytm.tsl ../libraries/

.PHONY: clean
clean:
	rm -f node_analogrytm.tsl
