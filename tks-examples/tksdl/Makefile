
INSTALL_GFX="$(TKS_TARGET_SITE_PREFIX)/applications/gfx"
INSTALL_AUDIO="$(TKS_TARGET_SITE_PREFIX)/applications/audio"

.PHONY: install
install:
	mkdir -p "$(INSTALL_GFX)"
	mkdir -p "$(INSTALL_AUDIO)"
	tks -c color         && mv color.tkx        "$(INSTALL_GFX)/"
	tks -c bobfield      && mv bobfield.tkx     "$(INSTALL_GFX)/"
	tks -c crparticles   && mv crparticles.tkx  "$(INSTALL_GFX)/"
	tks -c cubeworld     && mv cubeworld.tkx    "$(INSTALL_GFX)/"
	tks -c point_sprite  && mv point_sprite.tkx "$(INSTALL_GFX)/"
	tks -c rigid_body    && mv rigid_body.tkx   "$(INSTALL_GFX)/"
	tks -c torus         && mv torus.tkx        "$(INSTALL_GFX)/"
	tks -c audio_sinus   && mv audio_sinus.tkx  "$(INSTALL_AUDIO)/"
	tks -c sine_piano    && mv sine_piano.tkx   "$(INSTALL_AUDIO)/"
