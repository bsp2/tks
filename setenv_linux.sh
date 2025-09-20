export TKS_ROOT=`pwd`

export BUILD_64=y
export BUILD_ARM=n

# use clang instead of gcc
#  (note) arm32 (raspberrypi): clang14 ~10% slower than GCC12 (sieve)
#export BUILD_CLANG=y

if [ "${BUILD_ARM}" = "y" ]; then
  export PATH=${TKS_ROOT}/../arm-linux-gnueabihf:$PATH
fi

export CROSS_COMPILE=

# "" or omap3
export CROSS_TARGET=

export CROSS_ROOT=

# installation path, as seen on target
export TKS_TARGET_PREFIX=/usr/bin/
export TKS_TARGET_SITE_PREFIX=/usr/lib/tks/

# installation path, as seen on development host
export TKS_PREFIX=${TKS_TARGET_PREFIX}
export TKS_SITE_PREFIX=${TKS_TARGET_SITE_PREFIX}

if [ "" = "`alias m 2>/dev/null`" ] ; then
 m() { make -j10 -f makefile.linux $* ; }
#  alias m="make -j4 -f makefile.linux" 
fi
