// ---- 
// ----    file: Clipboard.h
// ----  author: (c) 2005 Bastian Spiegel <bs@tkscript.de>
// ----    date: 25-Jun-2005
// ---- license: Distributed under terms of the Lesser General Public License (LGPL). 
// ----          See <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
// ----    info: Helper functions to access the native desktop clipboard.
// ---- 
#ifndef __TKSDL_CLIPBOARD_H__
#define __TKSDL_CLIPBOARD_H__

YG("hal1");



// ----
// ---- Store the given object in the clipboard.
// ---- Currently, only String objects are supported which will be stored in ANSI 8Bit text format.
// ----
YF void YAC_CALL SDL_SetClipboard(YAC_Object *_o);

// ----
// ---- Query the current clipboard data, store in new object and return it.
// ---- Currently, only String object return values are supported.
// ----
YF void YAC_CALL SDL_GetClipboard(YAC_Value *_r);

#endif
