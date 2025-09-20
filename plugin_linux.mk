#
# Common plugin makefile for Linux / GCC target
#
#  Also see plugin_common.mk, plugin_common_gcc.mk
#
#

include $(YAC_ROOTPATH)/plugin_common.mk

#
# Determine TARGET filename if only PLUGIN name is known
#
ifeq ("$(TARGET)","")
TARGET=$(PLUGIN).so
endif

CPPFLAGS += -fPIC $(EXTRAFLAGS) $(EXTRAFLAGS_CPP)
CFLAGS += -fPIC $(EXTRAFLAGS) $(EXTRAFLAGS_C)
EXTRALIBS += -L$(CROSS_ROOT)/usr/lib -lm


#
# Install plugin
#  (Note: cannot depend on "bin" target since this would overwrite the UPX compressed file..)
#
install: $(INSTALL_RULES)
	$(CP) $(TARGET) "$(TKS_SITE_PREFIX)/plugins/"
	@echo "[...] $(TARGET) installed to \"$(TKS_SITE_PREFIX)/plugins/\".";


include $(YAC_ROOTPATH)/plugin_common_gcc.mk


#
# Deploy via SSH
#
.PHONY: deploy
deploy: bin
	scp $(TARGET) ${SCP_USER}@${SCP_HOST}:${SCP_PLUGIN_PATH}/
