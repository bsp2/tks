// ----
// ----    file: Clipboard_win32.cpp
// ----  author: (C) 2005 Bastian Spiegel <bs@tkscript.de>
// ----    date: 25-Jun-2005
// ---- license: Distributed under terms of the Lesser General Public License (LGPL). 
// ----          See <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
// ----
// ----    note: 
// ----

#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <yac.h>

#include "inc_sdl.h"

#include "HAL.h"
extern YAC_Host *yac_host;

#include "Clipboard.h"

#ifdef TKSDL_WIN32_CLIPBOARD 

#define MAX_CLIPBOARD_SIZE (512*1024)


// ----
// ---- Store the given object in the clipboard.
// ---- Currently, only String objects are supported which will be stored in ANSI 8Bit text format.
// ----
void SDL_SetClipboard(YAC_Object *_o) {
   if(YAC_BCHK(_o, YAC_CLID_STRING))
   {
      YAC_String *s=(YAC_String*)_o;
      if((s->length>1)&&(s->length<MAX_CLIPBOARD_SIZE))
      {
         HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT|GMEM_DDESHARE, MAX_CLIPBOARD_SIZE );
         if(hMem==NULL)
         {
            yac_host->printf("[---] SDL_SetClipboard_win32(): ::GlobalAlloc() failed.\n");
         }
         else
         {
            LPTSTR str=(LPTSTR)::GlobalLock(hMem);
            if(str!=NULL)
            {
               sUI i;
               for(i=0; i<s->length; i++)
               {
                  str[i]=s->chars[i];
               }
               str[i-1]=0;
               
               if(! ::GlobalUnlock(hMem))
               {
                  if(::GetLastError()!=NO_ERROR)
                  {
                     yac_host->printf("[---] SDL_SetClipboard_win32(): ::GlobalUnlock() failed with error code %08x.\n",
                        ::GetLastError());
                  }
                  else
                  {
                     int failCount = 0;
                     for(;;)
                     {
                        if(::OpenClipboard(NULL))
                        {
                           ::EmptyClipboard();
                           ::SetClipboardData(CF_TEXT, hMem);
                           ::CloseClipboard();
                           break;
                        }
                        else if(++failCount > 100)
                        {
                           yac_host->printf("[---] SDL_SetClipboard_win32(): ::OpenClipboard() failed with error code 0x%08x.\n", ::GetLastError());
                           break;
                        }
                     }
                  }
               }
               else
               {
                  yac_host->printf("[---] SDL_SetClipboard_win32(): ::GlobalUnlock(): handle is still locked.\n");
               }
            }
            else
            {
               yac_host->printf("[---] SDL_SetClipboard_win32(): ::GlobalLock() failed with error code %08x.\n",
                  ::GetLastError());
            }
            if(::GlobalDiscard(hMem)==NULL)
            {
               yac_host->printf("[---] SDL_SetClipboard_win32(): ::GlobalDiscard() failed with error code %08x.\n",
                  ::GetLastError());
            }
            /*if(::GlobalFree(hMem)!=NULL)
            {
            yac_host->printf("[---] SDL_SetClipboard_win32(): ::GlobalFree() failed with error code %08x.\n",
            ::GetLastError());
         }*/
         }
      }
   }
}

// ----
// ---- Query the current clipboard data, store in new object and return it.
// ---- Currently, only String object return values are supported.
// ----
void SDL_GetClipboard(YAC_Value *_r) {
   
   
   if(::OpenClipboard(NULL/*(HWND)_windowHandle*/))
   {
      YAC_String *s=YAC_New_String();
      _r->initString(s, 1);
      HGLOBAL hMem = ::GetClipboardData(CF_TEXT);
      if(hMem!=NULL)
      {
         LPTSTR str = (LPTSTR)::GlobalLock(hMem);
         if(str!=NULL)
         {
            YAC_String t; t.visit(str);
            s->yacCopy(&t);
            ::GlobalUnlock(hMem);
         }
      }
      ::CloseClipboard();
   }
   else
   {
      yac_host->printf("[---] SDL_SetClipboard_win32(): ::OpenClipboard() failed.\n");
   }
   
}

#endif // TKSDL_WIN32_CLIPBOARD
