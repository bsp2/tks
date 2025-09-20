#!/bin/bash

TKS_WWW_PATH=http://tkscript.org/files/tmp/12Aug2025/
##TKS_WWW_PATH=http://tkscript.org/files/current/

# (note) TKS_PACKAGES lists all files to be downloaded / unzipped
# (note) the list of packages to be built is configured in makefile.[macos|linux|msvc]

# download all packages
TKS_PACKAGES=( eqxmms tkanalogrytm tkbluetooth tkchipmunk tkdist2d tkfileutils tkfreeglut tkfreetype2 tkfreeverb tkmath tkmidi tkmidi_alsa tkmidi_portmidi tkmidipipe tkoldmath tkopengl tkportaudio tkportaudio_alsa tkradiastools tks-build tks-examples tks-source tksamplechain tksampleedit tksampler tksdl tksdl2 tksdl_net tksidplay2 tksmdi tkspeexdsp tksqlite tktriangulate tkminnie tkui tkclap tkvst2 tkvst2_stub tkvst2_nogui tkvst2_macos tkzip yac yingtest  debugtext tkunit dog md minnie org librs libplot libplot-fs graph )

# download minimal package selection
#TKS_PACKAGES=( tkfileutils tkfreetype2 tkmath tkoldmath tkopengl tkportaudio tkportaudio_alsa tks-build tks-examples tks-source tksdl tksdl2 tksdl_net tktriangulate tkui yac yingtest )

echo "TKS_PACKAGES=${TKS_PACKAGES[@]}"

# uncomment if the VST2 SDK (aeffect.h) is available (tkvst2)
#export TKS_OPT_PACKAGES=y

###kill -INT $$

mkdir -p packages
mkdir -p build
cd build

# download source packages
for d in ${TKS_PACKAGES[@]} ; do test -e "../packages/$d.zip" || (echo "downloading $d" ; cd ../packages/ ; wget -q "${TKS_WWW_PATH}/$d.zip") ; done

# unzip source packages
for d in ${TKS_PACKAGES[@]} ; do test -d "$d" || (echo "unpacking $d" ; unzip -q -o "../packages/$d.zip") ; done
echo "";

if [ $(echo $OSTYPE|grep darwin) ]; then
  echo "OS: MacOS";
  OK=1
  for d in sdl2 freetype libzip portaudio; do
    [ "$OK" = 1 ] && [ $(brew list|grep $d) = "$d" ] && OK=1 || OK=0
    [ "$OK" = 1 ] && echo "brew dependency $d found" || echo "missing brew package $d"
  done
  if [ "$OK" = "1" ]; then
    echo "all dependencies found"
    if [ "$HOSTTYPE" = "arm64" ]; then
       echo "sourcing setenv_macos_arm64.sh"
       . ./setenv_macos_arm64.sh
    else
       echo "sourcing setenv_macos_x86_64.sh"
       . ./setenv_macos_x86_64.sh
    fi
    echo "(note) please edit makefile.macos if you want to exclude plugins"
    echo "starting build"
    m all_clean && m all_bin && echo "type \"m all_install\" to install to \"$TKS_PREFIX\"" || echo "build failed"
  else
    echo "missing dependencies. please run"
    echo "  % brew install sdl2 freetype libzip portaudio"
  fi
fi

if [ "${OS}" = "Windows_NT" ]; then
  echo "OS: Windows";
  echo "(note) please edit makefile.msvc if you want to exclude plugins"
  echo "(note) you may also need to edit setenv_msvc.sh and adjust \$VCTK and \$WINSDK_VER"
  echo "sourcing setenv_msvc.sh"
  . ./setenv_msvc.sh
  echo "starting build"
  m all_clean && m all_bin && echo "type \"m all_install\" to install to \"$TKS_PREFIX\""
fi

if [ "${OSTYPE}" = "linux-gnu" ]; then
  echo "OS: Linux";
  echo "(note) please edit makefile.linux if you want to exclude plugins"
  echo "sourcing setenv_linux.sh"
  . ./setenv_linux.sh
  echo "starting build"
  m all_clean && m all_bin && echo "type \"sudo su\" followed by \". ./setenv_linux.sh ; m all_install\" to install to \"$TKS_PREFIX\""
fi
