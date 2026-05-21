
include $(TKS_ROOT)/install_linux.mk

ifeq ($(TARGET_DIR),)
TARGET_DIR:=$(TKS_ROOT)/tks-projects/apps/eureka/voice_plugins/
endif

ifneq (${STFX_SKIP_MAIN_INIT},)
CFLAGS   += -DSTFX_SKIP_MAIN_INIT
CPPFLAGS += -DSTFX_SKIP_MAIN_INIT
endif

CFLAGS+= -Wno-unused-variable -Wno-unused-function
CPPFLAGS+= -Wno-unused-variable -Wno-unused-function
EXTRALIBS+=
