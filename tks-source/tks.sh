#!/bin/sh
export TKS_PLUGIN_PATH=SITEPREFIX/plugins/
export TKS_MODULE_PATH=SITEPREFIX/modules/
export TKS_LIBRARY_PATH=SITEPREFIX/libraries/
export TKS_APPLICATION_PATH=SITEPREFIX/applications/
export LD_LIBRARY_PATH=SITEPREFIX/plugins/
#export LD_PRELOAD="$TKS_PLUGIN_PATH/tksdl.so;$TKS_PLUGIN_PATH/tkopengl.so"
PREFIX/tks.bin "$@"
