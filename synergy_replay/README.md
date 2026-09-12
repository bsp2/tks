Standalone "C" replay for exported Synergy / Eureka / Cycle projects.

- plays multi-channel standard MIDI files (SMF, up to 256 channels, exported from 'Synergy')
- uses 'Eureka' / 'tksampler' engine
- up to 127 samples with up to 255 zones / sample
- (user-defined) voice and track plugins (instruments and fx)
- native-code procedural samples and wavetables (exported from 'Eureka' / 'Cycle')
- procedural track samples (pre-render MIDI sequences to samples)
- application-controlled MIDI note, program change, and modulation events ((poly-)pressure, pitchbend, CC, RPN)
- multiple instances
- typical arm64 code-size (gzip'd): ~159k (demo project, 25 synth+fx plugins) / ~87k (FM only) / ~34k (sampler only)
- MIT license

see [replay](http://miditracker.org/replay.html) for further information.


# Build

After cloning the git repository (`git clone https://github.com/bsp2/tks.git`), build the replay demo as follows:

## macOS

``` shell
% cd git/tks
% . ./setenv_macos_arm64.sh
% cd synergy_replay
% m clean
% m plugins
% m bin
% ./sr_replay
```

note: use `% m OPT_SIZE=n plugins`, `% m OPT_SIZE=n bin` for full optimization
note: use `% m LOG=n PROFILE=n plugins`, `% m LOG=n PROFILE=n sz` for size optimization

## GNU / Linux

``` shell
$ cd git/tks
$ . ./setenv_linux.sh
$ cd synergy_replay
% m clean
$ m plugins
$ m bin
$ ./sr_replay
```

note: use `$ export BUILD_CLANG=y`, `$ m OPT_SIZE=n OPT_LTO=y plugins`, `$ m OPT_SIZE=n OPT_LTO=y bin` for full optimization

## Windows / MSVC
(todo)

note: `./sr_replay 1` (any cmdline arg) runs the demo in benchmark mode
