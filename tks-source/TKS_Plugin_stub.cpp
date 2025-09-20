/// TKS_Plugin_stub.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "TKS_Plugin.h"


int TKS_Plugin::plafBuildPathName(int _variant) {
   (void)_variant;
   path_name.empty();
   return -1;
}

sBool TKS_Plugin::plafOpenLibrary(const char *_pathName) {
   (void)_pathName;
   dll_handle = NULL;

   Dprintf("[---] TKS_Plugin_stub: plafOpenLibrary() called.\n");

   return 0;
}

void TKS_Plugin::plafCloseLibrary(void) {
}

void *TKS_Plugin::plafFindFunction(const char *_name) const {
   return NULL;
}

const char *TKS_Plugin::plafGetErrorString(void) {
   return "n/a";
}
