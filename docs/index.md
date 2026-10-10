% TkScript
% bsp
% 2026

$(var:header)

  
  
$(buf!toc)

# Introduction

TkScript ("TKS") is a free and open source "glue" script language for C / C++ frameworks.

It is mainly designed for audio / graphics / UI applications and text processing tools.

Features:
- C-like syntax
- classes with multiple-inheritance
- reflection / type introspection
- exception handling
- namespaces
- stream I/O and (de-)serialization
- RAII, _no_ garbage collection
- (true / native) multithreading
- dynamically (un-)loadable script modules and classes
- self contained application and library packages ("pak" files)
- extendable via native C(++) plugin functions / classes / methods

Software written in TkScript includes:
- The [Synergy](http://miditracker.org) MIDI sequencer and [Eureka](http://miditracker.org/eureka/readme.html) DAW
- Games
- Text processing tools
   - [md](md/test.html) - a markdown to HTML converter
   - [org](org/org_manual.html) - an [org-mode](https://orgmode.org/) like text processor, mind map converter, and project plan scheduler + report generator
   - [graph](graph/graph.html) - an ASCII based SVG graph generator (with OpenGL preview)

TkScript is accompanied by a set of add-on plugins / libraries that provide bindings for:
- [OpenGL](https://www.khronos.org/registry/OpenGL-Refpages/)
- Window / Event handling (via [SDL](http://www.libsdl.org/))
- Math (linear algebra)
- File I/O
- Zip files
- Networking
- Low latency audio ([ASIO](https://www.steinberg.net/en/company/developers.html), [PortAudio](http://www.portaudio.com/), [OSS](http://www.opensound.com/oss.html), [ALSA](https://www.alsa-project.org))
- [MIDI](https://www.midi.org/)
- XML based user interfaces (`tkui`)
 
TkScript has mainly been tested on the following platforms:
- arm64 / x86_64 macOS 10 / 13 / 14 / 15 / 26
- x86 / x64 Windows (2000 / XP / 7 / 8 / 10 / 11)
- x86 / x64 / ARM Linux (e.g. Debian, Ubuntu, Raspberry Pi OS, Yocto, ..)


# License

TkScript is distributed under the terms of the GNU General Public License (GPLv2).

The add-on plugins and libraries are distributed under the terms of the GNU Lesser (library) General Public License (LGPLv2), or the MIT / BSD licenses (see plugin / library source for details).

  
note: Software written in TkScript may use any license (FOSS, closed source, ..).


# Platform compatibility matrix

| Component                :| Windows | Linux | macOS
|---------------------------+---------+-------+------
| tks                       | Y       | Y     | Y
| eqxmms                    | Y       | Y     | Y
| tkanalogrytm              | Y       | Y     | Y
| tkbluetooth               | Y       | Y     | -
| tkchipmunk                | Y       | Y     | Y
| tkclap                    | Y       | y     | Y
| tkfileutils               | Y       | Y     | Y
| tkfreeglut                | Y       | Y     | Y
| tkfreetype2               | Y       | Y     | Y
| tkfreeverb                | Y       | Y     | Y
| tkgmp                     | Y       | Y     | Y
| tkmath                    | Y       | Y     | Y
| tkmidi                    | Y       | -     | -
| tkmidi\_alsa              | -       | Y     | -
| tkmidi\_portmidi          | Y       | Y     | Y
| tkmidipipe                | Y       | Y     | Y
| tkminnie                  | Y       | Y     | Y
| tkoldmath                 | Y       | Y     | Y
| tkopengl                  | Y       | Y     | Y
| tkportaudio               | Y       | Y     | Y
| tkportaudio\_alsa         | -       | Y     | -
| tkportaudio\_oss          | -       | Y     | -
| tkradiastools             | Y       | Y     | Y
| tkreplay                  | Y       | Y     | Y
| tksamplechain             | Y       | Y     | Y
| tksampleedit              | Y       | Y     | Y
| tksampler                 | Y       | Y     | Y
| tksdl                     | Y       | Y     | -
| tksdl12-compat            | -       | Y     | Y
| tksdl2                    | Y       | Y     | Y
| tksdl3                    | -       | Y     | Y
| tksdl_net                 | Y       | Y     | Y
| tksidplay2                | Y       | Y     | Y
| tksmdi                    | Y       | -     | -
| tkspeexdsp                | Y       | Y     | Y
| tksqlite                  | Y       | Y     | Y
| tktriangulate             | Y       | Y     | Y
| tkui                      | Y       | Y     | Y
| tkvst2                    | Y       | -     | -
| tkvst2\_stub              | -       | Y     | Y
| tkvst2\_nogui             | -       | Y     | Y
| tkvst2\_macos             | -       | -     | Y
| tkwii                     | Y       | -     | -
| tkzip                     | Y       | Y     | Y
| yingtest                  | Y       | Y     | Y
|---------------------------+---------+-------+------

# Downloads

## Precompiled Binaries

|noheader
|cols=1,1
| macOS 26 arm64            :|:[tks-macos_arm64.zip](files/current/tks-macos_arm64.zip) (v0.9.92.34, 10Oct2026)
| Debian GNU/Linux 13 arm64 :|:[tks-aarch64-raspberry.tar.gz](files/current/tks-aarch64-raspberry.tar.gz) (v0.9.92.34, 10Oct2026)
| Windows 10 x86\_64        :|:[tks.zip](files/current/tks.zip) (v0.9.92.34, 10Oct2026)
].table_noborder

tip: on macOS, copy the extracted the files to `/usr/local/` (optional)


## Source
The latest source-code packages are here:
- [tks-source](files/current/tks-source.zip)
- [tks-build](files/current/tks-build.zip)
- [yac](files/current/yac.zip), [yingtest](files/current/yingtest.zip)
- [tkanalogrytm](files/current/tkanalogrytm.zip)
- [tkchipmunk](files/current/tkchipmunk.zip) (v7)
- [tkclap](files/current/tkclap.zip)
- [tkfreetype2](files/current/tkfreetype2.zip)
- [tkfileutils](files/current/tkfileutils.zip)
- [tkmath](files/current/tkmath.zip), [tkoldmath](files/current/tkoldmath.zip), [tkgmp](files/current/tkgmp.zip)
- [tkminnie](files/current/minnie.zip)
- [tkopengl](files/current/tkopengl.zip)
- [tksdl](files/current/tksdl.zip) (Windows, deprecated), [tksdl12-compat](files/current/tksdl12-compat.zip) (deprecated), [tksdl2](files/current/tksdl2.zip) (Windows, Linux, macOS)
- [tksdl_net](files/current/tksdl_net.zip), [tkbluetooth](files/current/tkbluetooth.zip)
- [tkmidi](files/current/tkmidi.zip) (Windows), [tkmidi_alsa](files/current/tkmidi_alsa.zip) (Linux), [tkmidi_portmidi](files/current/tkmidi_portmidi.zip) (macOS)
- [tkmidipipe](files/current/tkmidipipe.zip)
- [tkportaudio](files/current/tkportaudio.zip), [tkportaudio_alsa](files/current/tkportaudio_alsa.zip)
- [tkradiastools](files/current/tkradiastools.zip)
- [svg_loader](files/current/svg_loader.zip), [tktriangulate](files/current/tktriangulate.zip)
- [tksampler](files/current/tksampler.zip), [tksamplechain](files/current/tksamplechain.zip), [tksampleedit](files/current/tksampleedit.zip)
- [tksmdi](files/current/tksmdi.zip)
- [tksqlite](files/current/tksqlite.zip)
- [tkui](files/current/tkui.zip)
- [tkunit](files/current/tkunit.zip)
- [tkvst2](files/current/tkvst2.zip) (Windows), [tkvst2 (stub)](files/current/tkvst2_stub.zip), [tkvst2 (nogui)](files/current/tkvst2_nogui.zip), [tkvst2 (macOS)](files/current/tkvst2_macos.zip),
- [tkzip](files/current/tkzip.zip)

>>>tip_build
How to build (via install helper):
~~~
$ wget http://tkscript.org/tks\_install\_helper.sh
$ chmod +x tks\_install\_helper.sh
$ ./tks\_install\_helper.sh
~~~
downloads the source packages and build them
~~~
$ sudo su
% cd build
% . ./setenv\_linux.sh
% m all\_install
~~~
installs them.

note: tested on macOS 13+14+15+26+27, Debian GNU/Linux 13, Windows 10+11
note: use [tks_install_helper_tmp.sh](http://tkscript.org/tks_install_helper_tmp.sh) to build the latest work-in-progress / beta version (last updated: **v0.9.92.30, 18Sep2026**)
note: as of September 20th, 2025, `tks` is also available on [GitHub](https://github.com/bsp2/tks)

How to build (manually):
- The first three packages (source, build, yac) are mandatory for building a command-line version of `tks`.
- The remaining packages are optional.
- Edit the toplevel makefile (`makefile.linux`, `makefile.macos` or `makefile.msvc`) to choose which additional plugins will be built.
- Edit+Run `. setenv_linux.sh`, `. ./setenv_macos_arm64.sh`, `. ./setenv_macos_x86_64.sh` or `. setenv_msvc.sh` to set up cross compiling / target platform / include+lib paths.
- It is recommended to set up a shell function for `make` invocation, e.g. `m(){ make -j20 -f makefile.msvc $* ; }` (done by setenv*)

Requirements / Prerequisites:
- The Windows build environment requires [MSYS2](https://www.msys2.org/) and the Microsoft Visual Studio compiler, e.g. the free-as-in-beer [Community Edition](https://visualstudio.microsoft.com/vs/community/).
- The Linux build environment requires GCC / G++, GNU make, and GNU Bash.
   - optional (recommended) packages: `$ sudo apt-get install build-essential libsdl3-dev libsdl2-dev libsdl2-net-dev libpng-dev libfreetype-dev libgtk2.0-dev libzip-dev libgnutls28-dev libasound2-dev`
   - optional packages: `$ sudo apt-get install portaudio19-dev libportmidi-dev sox rubberband-cli`
- The macOS build environment requires Clang (xcode toolchain), make, zsh, and the following [brew](https://brew.sh/) packages: `brew install sdl2 libpng freetype` (tested with arm64 and x86\_64) (and optionally `brew install portaudio portmidi` and `brew install sdl12-compat sdl2_net meson libzip gnutls`).

The plugin bindings use the `YaC` (*Yet another Component object model*) interface, and the `YInG` interface generator.
To rebuild the bindings for a specific module, run
~~~
$ m yac
~~~
in the plugin (or tks-source) directory.
<<<<
TIP: $(buf!tip_build) 

## Other
### emacs
- my [emacs config](.emacs) (`~/.emacs`)
- `iimage-mode` source code inline images (e.g. `<class.png>`): [src_images.zip](files/src_images.zip) (`~/src_images`)
- my [local emacs lisp extension archive](emacs-3rdparty_lisp.tgz) (`~/scripts/emacs-3rdparty_lisp`)
   - includes `ob-tks.el` and `ob-tks-rs.el` extensions for [cellular](librs_org/cellular.org.html) (*notebook*) and [literate](librs_org/literate.org.html) programming in [org-mode](https://orgmode.org/)


# Examples
- [tks-examples](files/current/tks-examples.zip)


# Applications
- [Synergy MIDITracker](http://miditracker.org) (music sequencer, 2009-2026)
- [Eureka](http://miditracker.org/eureka/readme.html) (audio sampler and VST2 / STFX / CLAP plugin host, 2018-2026)


# Tools
- [dog](files/current/dog.zip) (API documentation generator, from 2009) ([docs](dog/dog.html))
- [md](files/current/md.zip) (markdown document generator, 2018-2024) ([docs](md/test.html))
- [org](files/current/org.zip) (org-mode-like document processor, 2020) ([docs](org/org_manual.html))
- [graph](files/current/graph.zip) (ASCII to SVG diagram / graph generator with OpenGL preview, 2024) ([docs](graph/graph.html))
- [minnie](https://github.com/bsp2/minnie/tree/main/native) (vector graphics API and size-optimized binary stream format. supports conversion from SVG. 2018-2026) ([docs](minnie/minnie.html))


# Libraries
- [debugtext](files/current/debugtext.zip) OpenGL debug overlay. used by some (older) tksdl examples
- [libplot](files/current/libplot.zip) Simple function plotter (windowed and PNG export). Useful for `org-mode` notebooks.
- [libplot-fs](files/current/libplot-fs.zip) Simple GLSL pixel shader renderer (windowed and PNG export). Useful for `org-mode` notebooks.
- [librs](files/current/librs.zip) Remote-Script library.
    - for executing scripts in the context of another process
    - used by `ob-tks-rs.el` emacs `org-mode` extension and Synergy / Eureka MIDI sequencer / DAW applications
       - see HTML exported [cellular](librs_org/cellular.org.html) and [literate](librs_org/literate.org.html) `.org` test documents
- [tkui](files/current/tkui.zip) User Interface toolkit (SDL / OpenGL based with XML / SGML interface descriptions (`.xfm` XML forms)
    - Core widget classes: `BezierEdit`, `Button`, `CheckBox`, `ColorBox`, `ColorButton`, `ComboBox`, `ComboField`, `Dial`, `FloatParam`, `GraphForm`, `HSVColorPicker`, `Label`, `LayerSwitch`, `PopupMenu`, `RadioButton`, `RepeatButton`, `ScrollPane`, `Scroller`, `Slider`, `Spacer`, `SplitPane`, `StatusBar`, `TabSwitch`, (Tree)`TableView`, `TextEdit`, `TextField`, `TextView`, `TitledPanel`, `ViewPane`, `WindowDock`, `XMLForm`, `XYPad`
    - Core dialog classes: `ChoiceDialog`, `InfoDialog`, `FloatParamDialog`, `KeyHelpDialog`, `StringDialog`, `TextEditDialog`, `TextInputDialog`
    - Core layout classes: `BorderLayout`, `FlowLayout`, `GridLayout`


# Games
For your amusement, some of the older stuff can be downloaded here:
- [retrovaders2](files/current/retrovaders2.zip) (a Space Invaders like game, from 2001)
- [loadmd2](files/current/loadmd2.zip) (a loader / viewer for Quake2 models, from 2002)
- [equalize_it](files/current/equalize_it_dev.zip) (a SID player and collection of C64 tunes, from 2003)
- [gkraft](files/current/gkraft.zip) (2D cave flyer, from 2004)
- [racer](files/current/racer.zip) (3D racing game, from 2008)
- [jumpy](files/current/jumpy.zip) (2D jump'n'run, from 2010)
- [tequila](files/current/tequila.zip) (a classic Amiga500 demo effect ported to OpenGL, from 2010)


# Documentation
See [here](tks/ref.html) for language reference guide.

See [here](apidocs/prj_core.html) for API reference.

For historical purposes, [here](old_preMar2019/news_2015.html) are the old changelogs from 2002 - 2015.


  
$(buf!w3validator)

Document created in $(var:gen_ms) on $(var:localdatetime)
].create
