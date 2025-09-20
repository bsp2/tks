#
# Common plugin makefile for "msvc" / win32 target
#
#  Also see plugin_common.mk

include $(YAC_ROOTPATH)/plugin_common.mk

#
# Determine TARGET filename if only PLUGIN name is known
#
ifeq ("$(TARGET)","")
TARGET=$(PLUGIN).dll
endif

CPPFLAGS += -I"$(YAC_ROOTPATH)/yac" -DWIN32 $(EXTRAFLAGS) $(EXTRAFLAGS_CPP) -DYAC_FORCE_NO_TLS
CFLAGS += $(EXTRAFLAGS_C)
EXTRALIBS += -DLL -MAP


#
# Build plugin library
#
bin: $(BIN_RULES) $(ALL_OBJ)
	$(LD) $(LDFLAGS_SIZE) -OUT:$(TARGET) $(ALL_OBJ) $(EXTRALIBS)
	@echo "Build finished at `date +%H:%M`."


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
ee:	$(EE_RULES)
	$(TKS) app:dog++ -pn $(PLUGIN) -mn $(PLUGIN) *.h >"$(YAC_ROOTPATH)/docs/$(PLUGIN).ee"


#
# Make clean and remove backup files
#
.PHONY: realclean
realclean: $(REALCLEAN_RULES) clean
	$(RM) `$(FIND) . -name \*\~` $(TARGET) *.exp *.lib *.plg *.ncb *.map *.manifest
