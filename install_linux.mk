#
# Common makefile include for tks-source and plugins
#


#
# Location of "*.mk" files and "yac/" subdir
#  (absolute or relative to plugin dir)
#
ifeq ($(YAC_ROOTPATH),)
YAC_ROOTPATH=../
endif


# n=32bit build ("x86", jit)
# y=64bit build ("amd64", no jit)
ifeq ($(BUILD_64),)
BUILD_64=y
endif

ifeq ($(BUILD_ARM),)
BUILD_ARM=n
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
DEBUG=n
endif


#
# for 'deploy' target (Linux/embedded)
#
SCP_USER=root
SCP_HOST=$(BOARD_IP)
SCP_PLUGIN_PATH=/usr/lib/tks/plugins/
SCP_TKS_PREFIX=/usr/



#
# Cross compiler setup
#
#  (note) CROSS_COMPILE and CROSS_ROOT vars should be set in the shell
#          so that this file does not need to be modified for different
#          targets
#
#  (note) CROSS_TARGET can be set to 'OMAP3' (Open Pandora) or 'DNX_POKY' (ARM Poky Linux w/ DaveNX OpenGL|ES GPU)
#          this will disable the AUTOLOAD_TKOPENGL hack in "tks-source/TKS_CachedScript.cpp"
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
ifeq ($(BUILD_CLANG),y)
CPP       = $(CROSS_COMPILE)clang++
CC        = $(CROSS_COMPILE)clang
else
CPP       = $(CROSS_COMPILE)g++
CC        = $(CROSS_COMPILE)gcc
endif
AS        = $(CROSS_COMPILE)as
STRIP     = $(CROSS_COMPILE)strip
CP        = cp -f
FIND      = find
INSTALL   = ginstall
MAKE      = make
RM        = rm -f
SED       = sed
TKS       = tks
ZIP       = zip
UPX       = upx
MD5SUM    = md5sum


# 
# Number of parallel targets to make
# 
ifeq ($(BUILD_RASPBIAN),y)
NUMJOBS=2
else
#NUMJOBS=`grep -c "BogoMIPS" /proc/cpuinfo`
NUMJOBS=`grep -c "vendor_id" /proc/cpuinfo`
endif


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

ifeq ($(CROSS_TARGET),OMAP3)
MFLAGS += -mlittle-endian -march=armv7-a -mcpu=cortex-a8 -mfpu=neon
endif

ifeq ($(CROSS_TARGET),DNX_POKY)
ifeq ($(BUILD_64),y)
MFLAGS+= -march=armv8-a+crc
else
#-mthumb
MFLAGS += -march=armv7-a -mfpu=neon -mfloat-abi=hard
endif
endif





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
#EXTRA_INCLUDES= -I/home/bsp/omap35x/zlib-1.2.3
EXTRA_INCLUDES= -I$(CROSS_ROOT)/usr/include



#
# Extra library paths
#
#EXTRA_LIBS=
#EXTRA_LIBS= -L/home/bsp/omap35x/zlib-1.2.3
EXTRA_LIBS= -L$(CROSS_ROOT)/usr/lib


#
# Optimization flags
#
OPTFLAGS=

ifeq ($(RELEASE),y)

ifeq ($(CROSS_TARGET),OMAP3)
CFLAGS  += -pipe -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon
CPPFLAGS+= -pipe -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon
OPTFLAGS += -O2
else ifeq ($(CROSS_TARGET),DNX_POKY)
CFLAGS  += $(MFLAGS) --sysroot=$(CROSS_ROOT)
CPPFLAGS+= $(MFLAGS) --sysroot=$(CROSS_ROOT)
LDFLAGS += $(MFLAGS) --sysroot=$(CROSS_ROOT)
OPTFLAGS += -O3
else
OPTFLAGS += -O3
# for Raspberry Pi or Poky Linux builds
ifeq ($(BUILD_ARM),y)
ifeq ($(BUILD_64),n)
#OPTFLAGS+= -mtune=cortex-a8 -mfpu=neon
CFLAGS  += -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard
CPPFLAGS+= -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard
LDFLAGS += -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard
ifneq ($(CROSS_ROOT),)
CFLAGS  += --sysroot=$(CROSS_ROOT)
CPPFLAGS+= --sysroot=$(CROSS_ROOT)
LDFLAGS += --sysroot=$(CROSS_ROOT)
endif

endif
endif

endif

endif


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
ifeq ($(CROSS_TARGET),OMAP3)
LDFLAGS+= -Wl,-R./ 
CFLAGS   += -DOMAP3
CPPFLAGS += -DOMAP3
endif

ifeq ($(CROSS_TARGET),DNX_POKY)
LDFLAGS+= -Wl,-R./ 
CFLAGS   += -DDNX_POKY
CPPFLAGS += -DDNX_POKY
endif


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
