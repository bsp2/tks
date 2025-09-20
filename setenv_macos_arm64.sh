export TKS_ROOT=`pwd`

export BUILD_64=y
export BUILD_ARM=y

export CROSS_COMPILE=

# ""
export CROSS_TARGET=

export CROSS_ROOT=

# installation path, as seen on target
export TKS_TARGET_PREFIX=/usr/local/bin/
export TKS_TARGET_SITE_PREFIX=/usr/local/lib/tks/

# installation path, as seen on development host
export TKS_PREFIX=${TKS_TARGET_PREFIX}
export TKS_SITE_PREFIX=${TKS_TARGET_SITE_PREFIX}

m() { make -j10 -f makefile.macos $* ; }
