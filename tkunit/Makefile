#
# Makefile for tkunit library
#
# (c) 2008-2025 Bastian Spiegel <bs@tkscript.de>
#

TARGET= tkunit.tsl

#
# Build script library
#
$(TARGET): *.tks
	tks -cl tkunit
	@echo "Build finished at `date +%H:%M`."


#
# Install script library
#
.PHONY: install
install: $(TARGET)
	cp $(TARGET) $(TKS_SITE_PREFIX)/libraries/
	@echo "[...] $(TARGET) installed to \"$(TKS_SITE_PREFIX)/libraries/\".";


#
# Show help
#
.PHONY: help
help:
	@echo "       install   : Build and install tkunit.tsl to \"$(TKS_SITE_PREFIX)/libraries/\"."


#
# Remove object files and targets.
#
.PHONY: clean
clean:
	@echo "cleaning up.."
	rm -f $(TARGET)

#
# Make clean and remove backup files
#
.PHONY: realclean
realclean:	clean
	rm -f `find . -name \*\~`
