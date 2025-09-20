// ----
// ----    file: Clipboard_stub.cpp
// ----  author: (C) 2005 Bastian Spiegel <bs@tkscript.de>
// ----    date: 23-Aug-2005
// ---- license: Distributed under terms of the Lesser General Public License (LGPL). 
// ----          See <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
// ----
// ----    note: 
// ----

#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#define YAC_PRINTF
#include <yac.h>


#include "HAL.h"
extern YAC_Host *yac_host;

#include "Clipboard.h"


#define MAX_CLIPBOARD_SIZE 65536


static YAC_String clip_data;

// ----
// ---- Store the given object in the clipboard.
// ---- Currently, only String objects are supported which will be stored in ANSI 8Bit text format.
// ----
void SDL_SetClipboard(YAC_Object *_o) {
   if(YAC_BCHK(_o, YAC_CLID_STRING))
   {
      YAC_CAST_ARG(YAC_String, s, _o);
      if( (s->length > 0) && (s->length < MAX_CLIPBOARD_SIZE) )
      {
         clip_data.yacCopy(s);
      }
   }
}

// ----
// ---- Query the current clipboard data, store in new object and return it.
// ---- Currently, only String object return values are supported.
// ----
void SDL_GetClipboard(YAC_Value *_r) {
   
   _r->initString(&clip_data, 0);
}


