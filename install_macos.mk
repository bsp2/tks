#
# Common makefile include for tks-source and plugins on macOS
#


#
# Location of "*.mk" files and "yac/" subdir
#  (absolute or relative to plugin dir)
#
ifeq ($(YAC_ROOTPATH),)
YAC_ROOTPATH=..
endif


# n=32bit build ("x86", jit)
# y=64bit build ("amd64", no jit)
ifeq ($(BUILD_64),)
BUILD_64=y
endif

ifeq ($(BUILD_ARM),)
BUILD_ARM=y
endif


#
# Enable optimizations 
#
ifeq ($(RELEASE),)
RELEASE=y
endif


#
# Enable debug symbols.
#  Strip executable if NOT set to 'y'
#
ifeq ($(DEBUG),)
DEBUG=
endif


#
# for 'deploy' target (remote cross-compile)
#
SCP_USER=root
SCP_HOST=${BOARD_IP}
SCP_PLUGIN_PATH=/usr/lib/tks/plugins/
SCP_TKS_PREFIX=/usr/



#
# Cross compiler setup
#
#  (note) CROSS_COMPILE and CROSS_ROOT vars should be set in the shell
#          so that this file does not need to be modified for different
#          targets
#
#  (note) CROSS_TARGET is supposed to be used for specific boards (like 'OMAP3' (Open Pandora) or 'DNX_POKY' in Linux builds)
#

# Host compiler (default)
#CROSS_COMPILE=

# Code Sourcery ARMv7
#CROSS_COMPILE=arm-none-linux-gnueabi-

# Montavista ARMv5
#CROSS_COMPILE=arm_v5t_le-


# Location of target root FS (on dev. host)
#CROSS_ROOT=

#CROSS_ROOT=/bsp/pandora-dev/arm-2011.09



#
# Where to install tks
#   (creates 
#         $(TKS_SITE_PREFIX)/plugins
#         $(TKS_SITE_PREFIX)/libraries
#         $(TKS_SITE_PREFIX)/applications
#         $(TKS_SITE_PREFIX)/modules 
#         directories,
#    copies tks.sh to $(TKS_PREFIX)/tks and
#    tks.bin to $(TKS_PREFIX)/tks.bin
#    )
#

# Target installation paths for executable and libraries/plugins/modules
#  (also used to build "tks.sh" startup script, see tks-source/install.tks)
#  (note: the TARGET vars are for paths used on the target)
ifeq ($(TKS_TARGET_PREFIX),)
#TKS_TARGET_PREFIX=/usr/bin/
#TKS_TARGET_SITE_PREFIX=/usr/lib/tks/
#TKS_TARGET_PREFIX=$(TKS_ROOT)/../arm-linux-gnueabihf/
endif

ifeq ($(TKS_TARGET_SITE_PREFIX),)
TKS_TARGET_SITE_PREFIX=$(TKS_TARGET_PREFIX)
endif


# Installation paths used by development host
ifeq ($(TKS_PREFIX),)
TKS_PREFIX=$(CROSS_ROOT)$(TKS_TARGET_PREFIX)/bin/
#TKS_PREFIX=$(TKS_ROOT)/../arm-linux-gnueabihf/
endif

ifeq ($(TKS_SITE_PREFIX),)
TKS_SITE_PREFIX=$(CROSS_ROOT)$(TKS_TARGET_SITE_PREFIX)
#TKS_SITE_PREFIX=$(TKS_ROOT)/../arm-linux-gnueabihf/
endif


#
# Tool setup
#
AR        = $(CROSS_COMPILE)ar
CPP       = $(CROSS_COMPILE)g++
CC        = $(CROSS_COMPILE)gcc
AS        = $(CROSS_COMPILE)as
STRIP     = $(CROSS_COMPILE)strip -x
CP        = cp -f
FIND      = find
INSTALL   = ginstall
MAKE      = make
RM        = rm -f
SED       = sed
TKS       = tks
ZIP       = zip
7Z        = 7zz
UPX       = upx
MD5SUM    = md5sum


# 
# Number of parallel targets to make
# 
NUMJOBS=`sysctl -n hw.ncpu`
#NUMJOBS=4


#
# Target architecture 
#
ifeq ($(BUILD_ARM),y)
ifeq ($(BUILD_64),y)
ARCH=ARM64
else
ARCH=ARM32
endif
else
ifeq ($(BUILD_64),y)
ARCH=X64
else
ARCH=X86
endif
endif


#
# C compiler flags
#
CFLAGS= -Wall


#
# C++ compiler flags
#
CPPFLAGS= -Wall


#
# Objective-C compiler flags (macOS)
#
OBJCFLAGS=
# -lobjc


#
# Target architecture flags
#
ARCHFLAGS=
ifeq ($(BUILD_ARM),y)
ifeq ($(BUILD_64),y)
ARCHFLAGS+= -DARCH_ARM64
else
ARCHLAGS+= -DARCH_ARM32
endif
else
ifeq ($(BUILD_64),y)
ARCHFLAGS+= -DARCH_X64
else
ARCHFLAGS+= -DARCH_X86
endif
endif

CFLAGS+= $(ARCHFLAGS)
CPPFLAGS+= $(ARCHFLAGS)



#
# Target machine flags
#
MFLAGS=

#ifeq ($(CROSS_TARGET),OMAP3)
#MFLAGS += -mlittle-endian -march=armv7-a -mcpu=cortex-a8 -mfpu=neon
#endif

#ifeq ($(CROSS_TARGET),DNX_POKY)
#ifeq ($(BUILD_64),y)
#MFLAGS+= -march=armv8-a+crc
#else
##-mthumb
#MFLAGS += -march=armv7-a -mfpu=neon -mfloat-abi=hard
#endif
#endif





#
# Assembly flags
#
AFLAGS=



#
# Linker flags
#
LDFLAGS=


#
# Extra includes
#
#EXTRA_INCLUDES=
EXTRA_INCLUDES=
# -I$(CROSS_ROOT)/usr/include



#
# Extra library paths
#
#EXTRA_LIBS=
EXTRA_LIBS=
# -L$(CROSS_ROOT)/usr/lib


#
# Optimization flags
#
OPTFLAGS=

ifeq ($(RELEASE),y)

OPTFLAGS += -O3
#OPTFLAGS += -O2
#OPTFLAGS += -O2 -fsanitize=undefined
#OPTFLAGS +=

endif


#
# arm64 / x86_64 macOS cross-compilation
#
MACTARGET=
ifeq ($(BUILD_ARM),y)
# arm64 (m1/m2)
MACTARGET+= -target arm64-apple-macos13
else
# legacy x86_64
MACTARGET+= -target x86_64-apple-macos10.12
#MACTARGET+= -target x86_64-apple-macos13
endif
CFLAGS    += $(MACTARGET)
CPPFLAGS  += $(MACTARGET)
OBJCFLAGS += $(MACTARGET)
LDFLAGS   += $(MACTARGET)


#
# Debug flags
#
DBGFLAGS=

ifeq ($(DEBUG),y)
DBGFLAGS += -g
#DBGFLAGS= -g -pg
#DBGFLAGS= -ggdb3
endif


#
# Target dependent flags
#
#ifeq ($(CROSS_TARGET),xyz)
#LDFLAGS+= -Wl,-R./ 
#CFLAGS   += -DXYZ
#CPPFLAGS += -DXYZ
#endif


#
# Do not define if libgcc is missing hypotf() (old versions only)
# Set to 'y' if it has.
#
HAVE_HYPOTF=y



#
# Nothing to change after this line-----------------------------
#
CFLAGS+= $(EXTRA_INCLUDES) $(DBGFLAGS)
CPPFLAGS+= $(EXTRA_INCLUDES) $(DBGFLAGS)
LDFLAGS+= $(EXTRA_LIBS)

ifeq ($(HAVE_HYPOTF),y)
CFLAGS += -DHAVE_HYPOTF
CPPFLAGS += -DHAVE_HYPOTF
endif
