
ALL_SRC= \
	Prophet12_OscForm.xfm \
	Prophet12_OscForm.tks \
	Prophet12_EnvForm.xfm \
	Prophet12_EnvForm.tks \
	Prophet12_LFOForm.xfm \
	Prophet12_LFOForm.tks \
	Prophet12_DelayForm.xfm \
	Prophet12_DelayForm.tks \
	Prophet12_MatrixEntryForm.xfm \
	Prophet12_MatrixEntryForm.tks \
	Prophet12_MatrixForm.xfm \
	Prophet12_MatrixForm.tks \
	Prophet12_MorphForm.xfm \
	Prophet12_MorphForm.tks \
	P12_RandOscDialog.xfm \
	P12_RandOscDialog.tks \
	Prophet12.tks \
	Prophet12.xfm

syntheditor_dsi_prophet12.tsl: $(ALL_SRC)
	tks -cl syntheditor_dsi_prophet12 && mv syntheditor_dsi_prophet12.tsl ../libraries/

.PHONY: clean
clean:
	rm -f syntheditor_dsi_prophet12.tsl
