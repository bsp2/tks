export TKS_ROOT=`pwd`

export BUILD_64=y
export BUILD_ARM=n

export CROSS_COMPILE=

# ""
export CROSS_TARGET=

export CROSS_ROOT=

# installation path, as seen on target
export TKS_TARGET_PREFIX=/usr/local/homebrew/bin/
export TKS_TARGET_SITE_PREFIX=/usr/local/homebrew/lib/tks/
# libtks
export TKS_TARGET_INCLUDE_PREFIX=/usr/local/homebrew/include/
export TKS_TARGET_LIB_PREFIX=/usr/local/homebrew/lib/

# installation path, as seen on development host
export TKS_PREFIX=${TKS_TARGET_PREFIX}
export TKS_SITE_PREFIX=${TKS_TARGET_SITE_PREFIX}
export TKS_INCLUDE_PREFIX=${TKS_TARGET_INCLUDE_PREFIX}
export TKS_LIB_PREFIX=${TKS_TARGET_LIB_PREFIX}

# for all_install target
export PATH="$PATH:${TKS_PREFIX}"

m() { make -j10 -f makefile.macos $* ; }
