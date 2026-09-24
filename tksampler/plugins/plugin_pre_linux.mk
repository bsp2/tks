
include $(TKS_ROOT)/install_linux.mk

ifeq ($(TARGET_DIR),)
TARGET_DIR:=$(TKS_ROOT)/tks-projects/apps/eureka/voice_plugins/
endif

ifneq (${STFX_SKIP_MAIN_INIT},)
CFLAGS   += -DSTFX_SKIP_MAIN_INIT
CXXFLAGS += -DSTFX_SKIP_MAIN_INIT
endif

ifneq (${STFX_SKIP_NAMES_AND_RESETS},)
CFLAGS   += -DSTFX_SKIP_NAMES_AND_RESETS
CXXFLAGS += -DSTFX_SKIP_NAMES_AND_RESETS
endif

ifneq (${CYCLE_SKIP_UI},)
CFLAGS   += -DCYCLE_SKIP_UI
CXXFLAGS += -DCYCLE_SKIP_UI
endif

CFLAGS+= -Wno-unused-variable -Wno-unused-function
CXXFLAGS+= -Wno-unused-variable -Wno-unused-function
EXTRALIBS+=
