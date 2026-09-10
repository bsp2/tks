Standalone "C" replay for exported Synergy / Eureka / Cycle projects.

- plays multi-channel standard MIDI files (SMF, up to 256 channels, exported from 'Synergy')
- uses 'Eureka' / 'tksampler' engine
- up to 127 samples with up to 255 zones / sample
- (user-defined) voice and track plugins (instruments and fx)
- native-code procedural samples and wavetables (exported from 'Cycle')
- application-controlled MIDI note, program change, and modulation events ((poly-)pressure, pitchbend, CC, RPN)
- multiple instances
- typical arm64 code-size (gzip'd): ~183k (demo project, 25 synth+fx plugins) / ~54k (sampler only)
- MIT license

see [replay](http://miditracker.org/replay.html) for further information.
