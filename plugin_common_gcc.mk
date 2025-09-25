#
# Plugin targets for all (Linux-like) GCC based platforms
#

CPPFLAGS += -I$(YAC_ROOTPATH)/yac
# -Wall -I$(CROSS_ROOT)/usr/include
#CFLAGS += -Wall
# -Wall-I$(CROSS_ROOT)/usr/include


#
# Build plugin library
#
bin: $(BIN_RULES) $(ALL_OBJ)
	$(CPP) -shared -o "$(TARGET)" -Wl,-soname,$(TARGET) $(ALL_OBJ) $(LDFLAGS) $(EXTRALIBS)
ifneq ($(DEBUG),y)
	$(STRIP) "$(TARGET)"
endif
	@echo "Build finished at `date +%H:%M`."


.cpp.o:
	$(CPP) $(CPPFLAGS) $(OPTFLAGS) $(DBGFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(OPTFLAGS) $(DBGFLAGS) -c $< -o $@



#
# Create YAC interface code
#
.PHONY: yac
yac: $(YAC_RULES)
	tks app:ying *.h $(YAC_EXTRA_HEADERS)


#
# Extract documentation (in "DOG" format)
#
.PHONY: ee
ee::	$(EE_RULES)
	$(TKS) app:dog++ -pn $(PLUGIN) -mn $(PLUGIN) *.h >"$(YAC_ROOTPATH)/apidocs/ee/$(PLUGIN).ee"


#
# Make clean and remove backup files
#
.PHONY: realclean
realclean: $(REALCLEAN_RULES) clean
	$(RM) `$(FIND) . -name \*\~` *.map
