/// Clipboard_SDL2.cpp
///
/// (c) 2023 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///

#define YAC_PRINTF defined
#include <yac.h>

#include "inc_sdl.h"

#include "HAL.h"
extern YAC_Host *yac_host;

#include "Clipboard.h"

extern "C" int   SDL_SetClipboardText (const char *);
extern "C" char *SDL_GetClipboardText (void);
extern "C" void  SDL_free (void *);


void SDL_SetClipboard(YAC_Object *_o) {
   if(YAC_BCHK(_o, YAC_CLID_STRING))
   {
      YAC_CAST_ARG(YAC_String, s, _o);
      if(s->length > 0)
      {
         // yac_host->printf("xxx SDL_SetClipboard s=\"%s\"\n", s->chars);
         SDL_SetClipboardText((const char*)s->chars);
         return;
      }
   }
   SDL_SetClipboardText("");
}

void SDL_GetClipboard(YAC_Value *_r) {
   // yac_host->printf("xxx SDL_GetClipboard\n");
   _r->initEmptyString();
   char *cbChars = SDL_GetClipboardText();
   _r->value.string_val->copy(cbChars);
   SDL_free((void*)cbChars);
   // yac_host->printf("xxx   return \"%s\"\n", _r->value.string_val->chars);
}

