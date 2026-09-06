export TKS_ROOT=`pwd`

export BUILD_64=n
export BUILD_ARM=y

export BUILD_RASPBIAN=y

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
# libtks
export TKS_TARGET_INCLUDE_PREFIX=/usr/include/
export TKS_TARGET_LIB_PREFIX=/usr/lib/

# installation path, as seen on development host
export TKS_PREFIX=${TKS_TARGET_PREFIX}
export TKS_SITE_PREFIX=${TKS_TARGET_SITE_PREFIX}
export TKS_INCLUDE_PREFIX=${TKS_TARGET_INCLUDE_PREFIX}
export TKS_LIB_PREFIX=${TKS_TARGET_LIB_PREFIX}

if [ "" = "`alias m`" ] ; then
# m() { make -j10 -f makefile.linux $* ; }
  alias m="make -j2 -f makefile.linux" 
fi
