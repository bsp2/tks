
#include $(TKS_ROOT)/install_macos.mk

.cpp.o:
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(DBGFLAGS) -fPIC -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(OPTFLAGS) $(DBGFLAGS) -fPIC -c $< -o $@


$(TARGET).dylib: $(ALL_OBJ)
	$(CXX) -dynamiclib -o "$(TARGET).dylib" $(LDFLAGS) $(ALL_OBJ) $(EXTRALIBS)
ifneq ($(DEBUG),y)
	$(STRIP) "$(TARGET).dylib"
endif

.PHONY: bin
bin: $(TARGET).dylib
	@ls -ld "$(TARGET).dylib"
	@echo "Build finished at `date +%H:%M`."

.PHONY: install
install:
	$(CP) "$(TARGET).dylib" "$(TARGET_DIR)/"

.PHONY: clean
clean:
	$(RM) $(ALL_OBJ) "$(TARGET).map" "$(TARGET).lib" "$(TARGET).exp" "$(TARGET).so" "$(TARGET).dylib" "$(TARGET).dll"
