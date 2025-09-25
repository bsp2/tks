#
# Makefile for tkunit library
# 
# (c) 2008 Bastian Spiegel <bs@tkscript.de> 
#

TKS_PREFIX=/usr

TARGET = tkunit.tsl

#
# Install script library
#
install: tkunitlib
	cp $(TARGET) $(TKS_PREFIX)/lib/tks/libraries/
	@echo "[...] $(TARGET) installed to \"$(TKS_PREFIX)/lib/tks/libraries/\".";


#
# Build script library
#
tkunitlib:	
		tks -cl tkunit
		@echo "Build finished at `date +%H:%M`."


#
# Show help
#
help:
		@echo "       install   : Build and install tkunit.tsl to \"$(TKS_PREFIX)/lib/tks/libraries/\"."




#
# Remove object files and targets.
#
clean:
		@echo "cleaning up.."
		rm -f $(TARGET)

#
# Make clean and remove backup files
#
realclean:	clean
		rm -f `find . -name \*\~`


