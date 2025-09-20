/*
 * Copyright (c) 2009 Ross Bencina and Phil Burk, TkScript bindings by Bastian Spiegel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * The text above constitutes the entire PortAudio license; however, 
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also 
 * requested that these non-binding requests be included along with the 
 * license above.
 */

#include "tkportaudio.h"

#include "PaHostApiInfo.h"


_PaHostApiInfo::_PaHostApiInfo(void) {
   host_api_info = NULL;
}

_PaHostApiInfo::~_PaHostApiInfo() {
}

sSI _PaHostApiInfo::_getApiTypeId(void) {
   return (sSI) host_api_info->type;
}

void _PaHostApiInfo::_getName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->visit(host_api_info->name);
   _r->initString(s, 1);
}

sSI _PaHostApiInfo::_getDeviceCount(void) {
   return (sSI) host_api_info->deviceCount;
}

sSI _PaHostApiInfo::_getDefaultInputDeviceIndex(void) {
   return (sSI) host_api_info->defaultInputDevice;
}

sSI _PaHostApiInfo::_getDefaultOutputDeviceIndex(void) {
   return (sSI) host_api_info->defaultOutputDevice;
}


