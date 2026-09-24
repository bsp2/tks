
#include $(TKS_ROOT)/install_linux.mk

.cpp.o:
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(DBGFLAGS) -fPIC -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(OPTFLAGS) $(DBGFLAGS) -fPIC -c $< -o $@


$(TARGET).so: $(ALL_OBJ)
	$(CXX) -shared -o "$(TARGET).so" -Wl,-soname,$(TARGET).so $(LDFLAGS) $(ALL_OBJ) $(EXTRALIBS)
ifneq ($(DEBUG),y)
	$(STRIP) "$(TARGET).so"
endif

.PHONY: bin
bin: $(TARGET).so
	@ls -ld "$(TARGET).so"
	@echo "Build finished at `date +%H:%M`."

.PHONY: install
install:
	$(CP) "$(TARGET).so" "$(TARGET_DIR)/"

.PHONY: clean
clean:
	$(RM) $(ALL_OBJ) "$(TARGET).dll" "$(TARGET).map" "$(TARGET).lib" "$(TARGET).exp" "$(TARGET).so"
