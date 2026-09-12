#
# Common makefile include for tks-source and plugins
#


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
RSYNC_CMD=rsync -a -v -e ssh -u -r -l -z --exclude=\*.ini


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
TKS       = $(TKS_PREFIX)/tks
ZIP       = zip
UPX       = upx
MD5SUM    = md5sum
m         = $(MAKE) -f makefile.linux

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
  endif # BUILD_64
else
  ifeq ($(BUILD_64),y)
    ARCH=X64
  else
    ARCH=X86
  endif
endif # BUILD_ARM


#
# C compiler flags
#
CFLAGS= -Wall


#
# C++ compiler flags
#
CPPFLAGS= -Wall



#
# Assembly flags
#
AFLAGS=



#
# Linker flags
#  (note) GNU_HASH needed for Yocto build
#
LDFLAGS=-Wl,--hash-style=gnu



#
# Extra includes
#
EXTRA_INCLUDES=



#
# Extra library paths
#
EXTRA_LIBS=
EXTRA_LIBS+= -L"${TKS_LIB_PREFIX}"



#
# Target architecture flags
#
ARCHFLAGS=
ifeq ($(BUILD_ARM),y)
  ifeq ($(BUILD_64),y)
    ARCHFLAGS+= -DARCH_ARM64
  else
    ARCHLAGS+= -DARCH_ARM32
  endif # BUILD_64
else
  ifeq ($(BUILD_64),y)
    ARCHFLAGS+= -DARCH_X64
  else
    ARCHFLAGS+= -DARCH_X86
  endif # BUILD_64
endif # BUILD_ARM

CFLAGS+= $(ARCHFLAGS)
CPPFLAGS+= $(ARCHFLAGS)



#
# Target machine flags
#
MFLAGS=

ifeq ($(CROSS_TARGET),OMAP3)
  MFLAGS += -pipe -mlittle-endian -march=armv7-a -mcpu=cortex-a8 -mfpu=neon
  #EXTRA_INCLUDES= -I/home/bsp/omap35x/zlib-1.2.3
  #EXTRA_LIBS= -L/home/bsp/omap35x/zlib-1.2.3
endif

ifeq ($(CROSS_TARGET),DNX_POKY)
  ifeq ($(BUILD_64),y)
    MFLAGS+= -march=armv8-a+crc
  else
    #-mthumb
    MFLAGS += -march=armv7-a -mfpu=neon -mfloat-abi=hard
  endif # BUILD_64
endif # CROSS_TARGET POKY



#
# Cross root
#
ifneq ($(CROSS_ROOT),)
  CFLAGS  += --sysroot=$(CROSS_ROOT)
  CPPFLAGS+= --sysroot=$(CROSS_ROOT)
  LDFLAGS += --sysroot=$(CROSS_ROOT)
endif # /CROSS_ROOT


#
# Optimization flags
#
OPTFLAGS=

ifeq ($(RELEASE),y)
  ifeq ($(OPT_SIZE),y)
    OPTFLAGS+= -fno-exceptions -fno-unwind-tables
    ifeq ("${BUILD_CLANG}","y")
      OPTFLAGS+= -Oz
      OPTFLAGS+= -ffunction-sections -fdata-sections
      LDFLAGS+= -Wl,--gc-sections
    else
      OPTFLAGS+= -Os
      LDFLAGS+= -dead_strip
    endif # BUILD_CLANG
  else
    OPTFLAGS += -O3
  endif # OPT_SIZE
  ifeq ("${BUILD_CLANG}","y")
    ifeq ($(OPT_LTO),y)
      OPTFLAGS+= -flto
      LDFLAGS += -flto
    endif # OPT_LTO
  endif # BUILD_CLANG
endif # /RELEASE



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
# (Other) Target dependent flags
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
# Nothing to change after this line-----------------------------
#
CFLAGS+= $(MFLAGS) $(EXTRA_INCLUDES) $(DBGFLAGS)
CPPFLAGS+= $(MFLAGS) $(EXTRA_INCLUDES) $(DBGFLAGS)
LDFLAGS+= $(EXTRA_LIBS)
