# Adjust this path as necessary (14.0 is for Visual Studio 2015)
#export VCTK="c:/Program Files (x86)/Microsoft Visual Studio 14.0/VC"
#export VCTK="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.21.27702"
#export VCTK="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.22.27905"
#export VCTK="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.23.28105"
#export VCTK="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.24.28314"
#export VCTK="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.25.28610"
#export VCTK="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.26.28801"
# vs2022:
#export VCTK="c:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.32.31326"
#VCTK="c:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532"
#VCTK="c:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.37.32822"
VCTK="c:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.39.33519"
#VCTK="c:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207"
export VCTK_MSYS=`cygpath "${VCTK}"`

# Adjust this to match the Windows SDK installation path
export WINSDK_PATH="C:/Program Files (x86)/Windows Kits/10/"

# Adjust this to match the installed Windows 10 SDK version
##export WINSDK_VER=10.0.10240.0
#export WINSDK_VER=10.0.14393.0
#export WINSDK_VER=10.0.16299.0
export WINSDK_VER=10.0.20348.0
#export WINSDK_VER=10.0.26100.0

m() { make -j10 -f makefile.msvc $* ; }

if [ -d "${VCTK_MSYS}" ] ; then

  if [ -d "${WINSDK_PATH}/Lib/${WINSDK_VER}" ] ; then

    export PATH=$PATH:"${VCTK_MSYS}/bin"
#    export PATH=$PATH:"${VCTK_MSYS}/bin/Hostx64/x64"

    if [ -d "${VCTK_MSYS}/bin/Hostx64" ] ; then
        # vs2019
        export VCTK_BINPATH_64=`cygpath "${VCTK}/bin/Hostx64/x64"`
        export VCTK_BINPATH_32=`cygpath "${VCTK}/bin/Hostx64/x86"`
    else
        # vs2017 or older
        export VCTK_BINPATH_64=`cygpath "${VCTK}/bin/x86_amd64"`
        export VCTK_BINPATH_32=`cygpath "${VCTK}/bin"`
    fi

    export TKS_ROOT=`pwd`
#    export PATH=${TKS_ROOT}/../win_x64:$PATH
    export PATH="/c/Program Files/tks":$PATH

  else
    echo "Windows SDK path \"${WINSDK_PATH}/Lib/${WINSDK_VER}\" does not exist."
    echo "Please edit \"setenv_mscv.sh\" and fix the WINSDK_PATH and WINSDK_VER settings !"
  fi

else
  echo "Visual Studio directory \"${VCTK}\" does not exist."
  echo "Please edit \"setenv_mscv.sh\" and fix the VCTK path !"
fi

# when not set, use "Program Files" / "Program Files (x86)" in x64 / x86 builds
#export TKS_PREFIX=/c/Program Files/tks

# installation path, as seen on target
export TKS_TARGET_PREFIX=/c/Program\ Files/tks/
export TKS_TARGET_SITE_PREFIX=/c/Program\ Files/tks/

# installation path, as seen on development host
export TKS_PREFIX=${TKS_TARGET_PREFIX}
export TKS_SITE_PREFIX=${TKS_TARGET_SITE_PREFIX}


export TKS_ZIP_PREFIX=/f/Projects/zipped/
