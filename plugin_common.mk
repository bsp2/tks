#
#
# Common makefile include for all plugin targets on all platforms
#  
# This file is included by the platform-specific plugin_xyz.mk include files.
#
#



#
#
# In order to extend existing targets, add 
# your targets to the
#
#       HELP_RULES - extends "help" target
#    INSTALL_RULES - extends "install" target
#        BIN_RULES - extends "bin" target
#        YAC_RULES - extends "yac" target
#         EE_RULES - extends "ee" target
#      CLEAN_RULES - extends "clean" target
#  REALCLEAN_RULES - extends "realclean" target
#
# variables  
#
#

#
#
# You also need to set the following vars before including this makefile:
#
#

#
# Base plugin name. [required]
#
#PLUGIN=myplugin

#
# Output file name. If not set, use $PLUGIN.dll
#
##TARGET="myplugin.dll" [optional]


#
# Additional, platform dependent files
#
#PLAF_OBJ=platformdependentfile1.o


#
# Addition include directories
#
#EXTRAFLAGS=-Iadditional_include_dir


#
# Additional linker flags and libraries
#
#EXTRALIBS=-LIBPATH:libs other.lib



#
# Show available targets
#
help: $(HELP_RULES)
	@echo "       help      : this page."
	@echo "       bin       : build dynamically linked $(TARGET)."
	@echo "       yac       : (re-build) YAC interface sources (*.h)"
	@echo "       install   : build $(TARGET) and install to \"$(TKS_SITE_PREFIX)/plugins/\"."
	@echo "       clean     : remove object files."
	@echo "       realclean : clean and remove backup files."


#
# Compress plugin library (**deprecated**)
#
.PHONY: upx
upx:	bin $(UPX_RULES) 
	$(UPX) -9 $(TARGET)


#
# Remove object files and targets.
#
.PHONY: clean
clean: $(CLEAN_RULES)
	@echo "[...] cleaning up.."
	$(RM) $(ALL_OBJ) $(TARGET)
