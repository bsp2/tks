/// Cursor.cpp
///
/// (c) 2006-2023 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///

#include <stdlib.h>

#define YAC_BIGSTRING defined
#include <yac.h>

#include "inc_sdl.h"

#include "HAL.h"
#include "Mouse.h"
#include "Cursor.h"

extern YAC_Host *yac_host;
extern sUI clid_Cursor;


_Cursor::_Cursor(void) {
   sdl_cursor      = NULL;
   previous_cursor = NULL;
   data            = NULL;
   mask            = NULL;
   width           = 0;
   height          = 0;
   hotx            = 0;
   hoty            = 0;
   b_created       = YAC_FALSE;
}

_Cursor::~_Cursor() {
   freeCursor();
}

void _Cursor::freeCursor(void) {
   if(sdl_cursor != NULL)
   {
#if 0
      // (note) occasionally crashes.
      // (todo) why ? (maybe b/c SDL_QuitSubSystem(SDL_INIT_VIDEO) has already been called?)
      SDL_FreeCursor(sdl_cursor);
#endif
      sdl_cursor = NULL;
   }

   if(data != NULL)
   {
      delete [] data;
      data = NULL;
   }

   if(mask != NULL)
   {
      delete [] mask;
      mask = NULL;
   }

   previous_cursor = NULL;

   width     = 0;
   height    = 0;
   hotx      = 0;
   hoty      = 0;
   b_created = 0;
}

sBool _Cursor::_create(YAC_Object *_s, sSI _w, sSI _h, sSI _hotx, sSI _hoty) {
   freeCursor();

   if( (_w > 0) && (_h > 0) )
   {
      if( (0 == (_w&7)) && (0 == (_h&7)) )
      {
         if(YAC_BCHK(_s, YAC_CLID_STRING))
         {
            YAC_String *s = (YAC_String *) _s;
            if(s->length >= (sUI) (_w*_h))
            {
               sUI zoom = (tks_HAL->cursor_zoom > 1u) ? tks_HAL->cursor_zoom : 1u;
               sUI dataSz = ((_w>>3) * _h) * zoom * zoom;
               data = new sU8[dataSz];
               mask = new sU8[dataSz];
               memset((void*)data, 0, dataSz);
               memset((void*)mask, 0, dataSz);
               sSI x,y;
               sU8 *str = (sU8*) s->chars;
               sU8 *d = data;
               sU8 *m = mask;
               sUI zoomCntY = 0u;
               sSI zoomW = _w * sSI(zoom);
               sSI zoomH = _h * sSI(zoom);
               // yac_host->printf("xxx Cursor sz=(%d; %d) => zoomSz=(%d; %d)\n", _w, _h, zoomW, zoomH);
               for(y = 0; y < zoomH; y++)
               {
                  sU8 bit = (1u << 7)/*128u*/;
                  sUI zoomCntX = 0u;
                  for(x = 0; x < zoomW; x++)
                  {
                     //yac_host->printf("x=%i bit=%01x\n", x, bit);
                     bit = bit >> 1;
                     if(0u == bit)
                     {
                        bit = (1u << 7)/*128u*/;
                        d++;
                        m++;
                     }
                     switch(*str)
                     {
                        case 'X':
                           *d |= bit;
                           *m |= bit;
                           break;
                        case 'x':
                           *d |= bit;
                           break;
                        case '.':
                           *m |= bit;
                           break;
                     }

                     // Advance X
                     zoomCntX++;
                     if(zoomCntX >= zoom)
                     {
                        zoomCntX = 0u;
                        str++;
                     }
                  }

                  // Advance Y
                  zoomCntY++;
                  if(zoomCntY >= zoom)
                  {
                     zoomCntY = 0u;
                  }
                  else
                  {
                     str -= _w;  // repeat last row
                  }
               }
               width  = zoomW;
               height = zoomH;
               hotx   = _hotx * zoom;
               hoty   = _hoty * zoom;
               return YAC_TRUE;
            }
         }
      }
   }
   return YAC_FALSE;
}

sBool _Cursor::_show(void) {
   if(tks_HAL->view_flags & _HAL::VP_OPENED)
   {
      if(!b_created)
      {
         if( (NULL != data) && (NULL != mask) )
         {
            //yac_host->printf(" data=%p mask=%p w=%i h=%i hotx=%i hoty=%i\n", data, mask, _w, _h, _hotx, _hoty);
            sdl_cursor = SDL_CreateCursor((Uint8 *)data, (Uint8 *)mask, width, height, hotx, hoty);
            b_created = (NULL != sdl_cursor);
            delete [] data;
            data = NULL;
            delete [] mask;
            mask = NULL;
         }
      }
      if(b_created)
      {
         SDL_Cursor *prev = SDL_GetCursor();
         if(prev != sdl_cursor)
         {
            previous_cursor = prev;
         }
         SDL_SetCursor(sdl_cursor);

         // Re-enable cursor
         if(TKS_MOUSE_POINTER_MANUAL != tks_HAL->mouse.pointer_mode)
         {
            tks_HAL->mouse.auto_hide_timeout = tks_HAL->mouse.def_auto_hide_timeout;
            tks_HAL->mouse.show              = TKS_MOUSE_SHOW_ENABLE;
         }

         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

void _Cursor::_revert(void) {
   if(tks_HAL->view_flags&_HAL::VP_OPENED)
   {
      if(previous_cursor != NULL)
      {
         SDL_SetCursor(previous_cursor);
         previous_cursor = NULL;
      }
   }
}
  
