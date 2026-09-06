
TKS=tks

.PHONY: test_tsl
test_tsl: all
	$(TKS) synergy test_tsl && echo "test_tsl ok"

.PHONY: test_exit
test_exit: all
	time $(TKS) synergy test_skip_run

.PHONY: all
all: nodes syntheditors tkx

.PHONY: tkx
tkx:
	$(TKS) -c synergy

.PHONY: nodes
nodes:
	make -C node_analogrytm
	make -C node_buffer
	make -C node_ctlmorph
	make -C node_delay
	make -C node_keymap
	make -C node_modular
	make -C node_monopoly
	make -C node_proxy
	make -C node_script

.PHONY: nodes_clean
nodes_clean:
	make -C node_analogrytm clean
	make -C node_buffer clean
	make -C node_ctlmorph clean
	make -C node_delay clean
	make -C node_keymap clean
	make -C node_modular clean
	make -C node_monopoly clean
	make -C node_proxy clean
	make -C node_script clean

.PHONY: syntheditors
syntheditors:
	make -C syntheditor_dsi_prophet08
	make -C syntheditor_dsi_prophet12
	make -C syntheditor_generic
	make -C syntheditor_korg_radias
	make -C syntheditor_oberheim_matrix1000
	make -C syntheditor_yamaha_fs1r_montage

.PHONY: syntheditors_clean
syntheditors_clean:
	make -C syntheditor_dsi_prophet08 clean
	make -C syntheditor_dsi_prophet12 clean
	make -C syntheditor_generic clean
	make -C syntheditor_korg_radias clean
	make -C syntheditor_oberheim_matrix1000 clean
	make -C syntheditor_yamaha_fs1r_montage clean

.PHONY: clean
clean: nodes_clean syntheditors_clean
	rm -f synergy.tkx

.PHONY: install_rls
install_rls:
	tks -c synergy && mv synergy.tkx ../synergy/macos_installer_arm64/Synergy_arm64.app/Contents/MacOS/data.tkx
	tks -c synergy && mv synergy.tkx ../synergy/macos_installer_x86_64/Synergy_x86_64.app/Contents/MacOS/data.tkx
