export TKS_ROOT=`pwd`

export BUILD_64=n
export BUILD_ARM=y

#if [ "${BUILD_ARM}" = "y" ]; then
#  export PATH=${TKS_ROOT}/../arm-linux-gnueabihf:$PATH
#fi

if [ "${BUILD_64}" = "y" ]; then
export CROSS_COMPILE=aarch64-testro-linux-
else
export CROSS_COMPILE=arm-testro-linux-gnueabi-
fi

# "" or omap3
export CROSS_TARGET=DNX_POKY

export CROSS_ROOT=${SDKTARGETSYSROOT}

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

alias m="make -j10 -f makefile.linux"
