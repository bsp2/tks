
ALL_SRC= \
	../../../libs/modularcv/input.tks \
	../../../libs/modularcv/output.tks \
	../../../libs/modularcv/module.tks \
	../../../libs/modularcv/patch.tks \
	../../../libs/modularcv/modcv_add.tks \
	../../../libs/modularcv/modcv_adsr.tks \
	../../../libs/modularcv/modcv_attenuvertor.tks \
	../../../libs/modularcv/modcv_autorpnslider.tks \
	../../../libs/modularcv/modcv_bernoulli.tks \
	../../../libs/modularcv/modcv_bezier01.tks \
	../../../libs/modularcv/modcv_clock.tks \
	../../../libs/modularcv/modcv_const.tks \
	../../../libs/modularcv/modcv_crossfade.tks \
	../../../libs/modularcv/modcv_cvseq16.tks \
	../../../libs/modularcv/modcv_delay.tks \
	../../../libs/modularcv/modcv_flip.tks \
	../../../libs/modularcv/modcv_gatehysteresis.tks \
	../../../libs/modularcv/modcv_gateoutput.tks \
	../../../libs/modularcv/modcv_input.tks \
	../../../libs/modularcv/modcv_keymap.tks \
	../../../libs/modularcv/modcv_ledmonitor.tks \
	../../../libs/modularcv/modcv_lfo.tks \
	../../../libs/modularcv/modcv_label.tks \
	../../../libs/modularcv/modcv_math.tks \
	../../../libs/modularcv/modcv_mix4.tks \
	../../../libs/modularcv/modcv_memory.tks \
	../../../libs/modularcv/modcv_multiple.tks \
	../../../libs/modularcv/modcv_multinoteout.tks \
	../../../libs/modularcv/modcv_noteinput.tks \
	../../../libs/modularcv/modcv_noteoutput.tks \
	../../../libs/modularcv/modcv_output.tks \
	../../../libs/modularcv/modcv_pan.tks \
	../../../libs/modularcv/modcv_patternquantizer.tks \
	../../../libs/modularcv/modcv_perfinput.tks \
	../../../libs/modularcv/modcv_perfoutput.tks \
	../../../libs/modularcv/modcv_quantizer.tks \
	../../../libs/modularcv/modcv_rpnctl.tks \
	../../../libs/modularcv/modcv_rpnout.tks \
	../../../libs/modularcv/modcv_rsh.tks \
	../../../libs/modularcv/modcv_serial.tks \
	../../../libs/modularcv/modcv_shift.tks \
	../../../libs/modularcv/modcv_shifttrig.tks \
	../../../libs/modularcv/modcv_shuttlecontrol.tks \
	../../../libs/modularcv/modcv_slew.tks \
	../../../libs/modularcv/modcv_spring.tks \
	../../../libs/modularcv/modcv_switch.tks \
	../../../libs/modularcv/modcv_transpose.tks \
	../../../libs/modularcv/modcv_trigseq16.tks \
	../../../libs/modularcv/modcv_trigtogate.tks \
	../../../libs/modularcv/modcv_unavail.tks \
	../../../libs/modularcv/modcv_vca.tks \
	NodeModular.tks \
	ModularRack.tks \
	NodeModularEditor.xfm \
	NodeModularEditor.tks \
	node_modular.tkp

node_modular.tsl: $(ALL_SRC)
	tks -cl node_modular && mv node_modular.tsl ../libraries/

.PHONY: clean
clean:
	rm -f node_modular.tsl
