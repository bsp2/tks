#
# Common plugin makefile for macOS / GCC target
#
#  Also see plugin_common.mk, plugin_common_gcc.mk
#
#

ifneq ("$(STATIC_TARGET)","")
CLEAN_RULES+= clean_static_target
endif

include $(YAC_ROOTPATH)/plugin_common.mk

#
# Determine TARGET filename if only PLUGIN name is known
#
ifeq ("$(STATIC_TARGET)","")
ifeq ("$(TARGET)","")
TARGET=$(PLUGIN).dylib
endif
endif

ifeq ($(BUILD_ARM),y)
# arm64
EXTRAFLAGS += -I/opt/homebrew/include
EXTRAFLAGS_CPP += -I/opt/homebrew/include
else
# x86_64
EXTRAFLAGS += -I/usr/local/homebrew/include
EXTRAFLAGS_CPP += -I/usr/local/homebrew/include
endif

CPPFLAGS += -Wall -fPIC -I"$(YAC_ROOTPATH)/yac" $(EXTRAFLAGS) $(EXTRAFLAGS_CPP)
CFLAGS += -Wall -fPIC -I"$(YAC_ROOTPATH)/yac" $(EXTRAFLAGS) $(EXTRAFLAGS_C)
OBJCFLAGS += -fPIC -I"$(YAC_ROOTPATH)/yac"
EXTRALIBS += -L$(CROSS_ROOT)/usr/lib -lm
ifeq ($(BUILD_ARM),y)
# arm64
EXTRALIBS += -L/opt/homebrew/lib
else
# x86_64
EXTRALIBS += -L/usr/local/homebrew/lib
endif

#CPPFLAGS += -I$(CROSS_ROOT)/usr/include
#CFLAGS += -I$(CROSS_ROOT)/usr/include


#
# Install plugin
#  (Note: cannot depend on "bin" target since this would overwrite the UPX compressed file..)
#
ifneq ($(PLUGIN),"")
.PHONY: install
install: $(INSTALL_RULES)
	$(CP) $(TARGET) "$(TKS_SITE_PREFIX)/plugins/"
	@echo "[...] $(TARGET) installed to \"$(TKS_SITE_PREFIX)/plugins/\".";
endif


#
# Build plugin library
#
ifneq ("$(STATIC_TARGET)","")
.PHONY: static_target
static_target: $(BIN_RULES) $(STATIC_TARGET_OBJ)
	$(AR) -r $(STATIC_TARGET) $(STATIC_TARGET_OBJ)
#	$(AR) -t $(STATIC_TARGET)
# (note) __cstring section (0x98c3=39107 bytes) contains shader sources and log strings
#	objdump -s -j __cstring ../../../shadervg/shadervg.o

.PHONY: clean_static_target
clean_static_target:
	@echo "clean_static_target"
	rm -f $(STATIC_TARGET) $(STATIC_TARGET_OBJ)
else
ifeq ("$(CUSTOM_BIN_TARGET)",y)
else
.PHONY: bin
bin: $(BIN_RULES) $(ALL_OBJ)
	$(CPP) -dynamiclib -o $(TARGET) $(ALL_OBJ) $(LDFLAGS) $(EXTRALIBS)
#-install_name "$(TKS_SITE_PREFIX)/plugins/$(TARGET)"
endif
endif

ifneq ($(DEBUG),y)
ifneq ("$(PLUGIN)","")
	$(STRIP) "$(TARGET)"
endif
endif
	@echo "Build finished at `date +%H:%M`."


.cpp.o:
	$(CPP) $(CPPFLAGS) $(OPTFLAGS) $(DBGFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(OPTFLAGS) $(DBGFLAGS) -c $< -o $@

.m.o:
	$(CC) $(OBJCFLAGS) $(OPTFLAGS) $(DBGFLAGS) $(EXTRAFLAGS_OBJC) -c $< -o $@

%.o : %.mm
	$(CC) $(OBJCFLAGS) $(OPTFLAGS) $(DBGFLAGS) $(EXTRAFLAGS_OBJC) -c $< -o $@


#
# Create YAC interface code
#
ifneq ($(PLUGIN),"")
.PHONY: yac
yac: $(YAC_RULES)
	tks app:ying *.h $(YAC_EXTRA_HEADERS)
endif

#
# Extract documentation (in "DOG" format)
#
ifneq ("$(PLUGIN)","")
.PHONY: ee
ee::	$(EE_RULES)
	$(TKS) app:dog++ -pn $(PLUGIN) -mn $(PLUGIN) *.h >"$(YAC_ROOTPATH)/apidocs/ee/$(PLUGIN).ee"
endif

#
# Make clean and remove backup files
#
.PHONY: realclean
realclean: $(REALCLEAN_RULES) clean
	$(RM) `$(FIND) . -name \*\~` *.map


#
# Deploy via SSH
#
ifneq ("$(TARGET)","")
.PHONY: deploy
deploy: bin
	scp $(TARGET) ${SCP_USER}@${SCP_HOST}:${SCP_PLUGIN_PATH}/
endif
