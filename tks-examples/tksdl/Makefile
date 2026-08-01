
INSTALL_GFX:="$(TKS_TARGET_SITE_PREFIX)/applications/gfx"
INSTALL_AUDIO:="$(TKS_TARGET_SITE_PREFIX)/applications/audio"

TKX_OPT:= -ps -nln

.PHONY: install
install:
	mkdir -p "$(INSTALL_GFX)"
	mkdir -p "$(INSTALL_AUDIO)"
	tks $(TKX_OPT) -c color         && mv color.tkx        "$(INSTALL_GFX)/"
	tks $(TKX_OPT) -c bobfield      && mv bobfield.tkx     "$(INSTALL_GFX)/"
	tks $(TKX_OPT) -c crparticles   && mv crparticles.tkx  "$(INSTALL_GFX)/"
	tks $(TKX_OPT) -c cubeworld     && mv cubeworld.tkx    "$(INSTALL_GFX)/"
	tks $(TKX_OPT) -c point_sprite  && mv point_sprite.tkx "$(INSTALL_GFX)/"
	tks $(TKX_OPT) -c rigid_body    && mv rigid_body.tkx   "$(INSTALL_GFX)/"
	tks $(TKX_OPT) -c torus         && mv torus.tkx        "$(INSTALL_GFX)/"
	tks $(TKX_OPT) -c audio_sinus   && mv audio_sinus.tkx  "$(INSTALL_AUDIO)/"
	tks $(TKX_OPT) -c sine_piano    && mv sine_piano.tkx   "$(INSTALL_AUDIO)/"
