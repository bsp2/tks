/// tkui.cpp
///
/// (c) 2005-2013 by Bastian Spiegel <bs@tkscript.de>
///     - published under terms of the GNU general public license (GPL)
///   
///   This is the  main entry point for the "tkui" vector-based GUI plugin.
///   
/// created: 02Apr2005
/// changed: 11Aug2005, 23May2013
///
///
///

#include <stdio.h>
#include <stdarg.h>

#define YAC_PRINTF
#define YAC_BIGSTRING
#include <yac.h>

YAC_Host *yac_host;

#include "TKUI_CommonDialogs.h"

#include "ying_ui.h"

#include "ying_ui.cpp"

#include "yac_host.cpp"


void YAC_CALL YAC_Init(YAC_Host *_host) {
   yac_host=_host;
   YAC_Init_ui(_host);
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   if(yac_host->yacGetDebugLevel() > 1)
   {
      yac_host->printf("[...] tkui::YAC_Exit() beg.\n");
   }
   
   YAC_Exit_ui(_host);
   
   if(yac_host->yacGetDebugLevel() > 1)
   {
      yac_host->printf("[...] tkui::YAC_Exit() end.\n");
   }
}

