
eureka.tkx: eureka.tkp *.tks
	tks -c eureka

.PHONY: install
install: eureka.tkx
# (note) eureka writes / loads *.dat cache files to / from current dir
#         i.e. it's not a good idea to launch the app from an arbitrary dir
#	cp eureka.tkx "$(TKS_SITE_PREFIX)/applications/"

.PHONY: install_rls
install_rls:
	tks -c eureka && mv eureka.tkx ../synergy/macos_installer_arm64/Synergy_arm64.app/Contents/MacOS/eureka/data.tkx
	tks -c eureka && mv eureka.tkx ../synergy/macos_installer_x86_64/Synergy_x86_64.app/Contents/MacOS/eureka/data.tkx
