
.cpp.o:
	$(CXX) $(CXXFLAGS) $(OPTFLAGS_PLUGIN) -c $< -Fo"$@"

.c.o:
	$(CC) $(CXXFLAGS) $(OPTFLAGS_PLUGIN) -c $< -Fo"$@"


$(TARGET).dll: $(ALL_OBJ)
	$(LD) $(LDFLAGS_SIZE) -OUT:"$(TARGET).dll" $(ALL_OBJ) $(EXTRALIBS)

.PHONY: bin
bin: $(TARGET).dll
	@ls -ld "$(TARGET).dll"
	@echo "Build finished at `date +%H:%M`."

.PHONY: install
install:
	$(CP) "$(TARGET).dll" "$(TARGET_DIR)/"

.PHONY: clean
clean:
	$(RM) $(ALL_OBJ) "$(TARGET).dll" "$(TARGET).map" "$(TARGET).lib" "$(TARGET).exp" "$(TARGET).so"
