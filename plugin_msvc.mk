#
# Common plugin makefile for "msvc" / win32 target
#
#  Also see plugin_common.mk

ifneq ("$(STATIC_TARGET)","")
CLEAN_RULES+= clean_static_target
endif

include ${TKS_ROOT}/plugin_common.mk

#
# Determine TARGET filename if only PLUGIN name is known
#
ifeq ("$(STATIC_TARGET)","")
ifeq ("$(TARGET)","")
TARGET=$(PLUGIN).dll
endif
endif

CPPFLAGS += -I"${TKS_ROOT}/yac" -DWIN32 $(EXTRAFLAGS) $(EXTRAFLAGS_CPP) -DYAC_FORCE_NO_TLS
CFLAGS += $(EXTRAFLAGS_C)


#
# Build plugin library
#
ifneq ("$(STATIC_TARGET)","")
.PHONY: static_target
static_target: $(BIN_RULES) $(STATIC_TARGET_OBJ)
	$(LIB) -OUT:$(STATIC_TARGET) $(STATIC_TARGET_OBJ)

.PHONY: clean_static_target
clean_static_target:
	@echo "clean_static_target"
	rm -f $(STATIC_TARGET) $(STATIC_TARGET_OBJ)
else
ifeq ("$(CUSTOM_BIN_TARGET)",y)
else
.PHONY: bin
bin: $(BIN_RULES) $(ALL_OBJ)
	$(LD) $(LDFLAGS_SIZE) -OUT:$(TARGET) $(ALL_OBJ) $(EXTRALIBS) -DLL -MAP
	@echo "Build finished at `date +%H:%M`."
endif
endif


.cpp.o:
	$(CC) $(CPPFLAGS) $(OPTFLAGS_PLUGIN) -c $< -Fo"$@"

.c.o:
	$(CC) $(CPPFLAGS) $(OPTFLAGS_PLUGIN) -c $< -Fo"$@"


#
# Install plugin
#  (Note: cannot depend on "bin" target since this would overwrite the UPX compressed file..)
#
install: $(INSTALL_RULES)
	$(CP) $(TARGET) "$(TKS_SITE_PREFIX)/plugins/"
	@echo "[...] $(TARGET) installed to \"$(TKS_SITE_PREFIX)/plugins/\".";


#
# Create YAC interface code
#
.PHONY: yac
yac: $(YAC_RULES)
	$(TKS) app:ying *.h $(YAC_EXTRA_HEADERS)


#
# Extract documentation (in "DOG" format)
#
.PHONY: ee
ee::	$(EE_RULES)
	$(TKS) app:dog++ -pn $(PLUGIN) -mn $(PLUGIN) *.h >"${TKS_ROOT}/apidocs/ee/$(PLUGIN).ee"


#
# Make clean and remove backup files
#
.PHONY: realclean
realclean: $(REALCLEAN_RULES) clean
	$(RM) `$(FIND) . -name \*\~ -o -name .DS_Store` $(TARGET) *.exp *.lib *.plg *.ncb *.map *.manifest
