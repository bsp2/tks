/*
 * Copyright (c) 2009-2018 Ross Bencina and Phil Burk, TkScript bindings by Bastian Spiegel
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

YAC_Host *yac_host;

#include "PaHostApiInfo.h"
#include "PaDeviceInfo.h"
#include "PaStreamParameters.h"
#include "PaStream.h"
#include "PortAudio.h"


#include "ying_portaudio_PortAudio.cpp"
#include "ying_portaudio_PaHostApiInfo.cpp"
#include "ying_portaudio_PaDeviceInfo.cpp"
#include "ying_portaudio_PaStreamParameters.cpp"
#include "ying_portaudio_PaStreamMacOSWorkgroupJoinToken.cpp"
#include "ying_portaudio_PaStream.cpp"

#include "ying_portaudio.cpp"


// Implement standard exception ID variables (see yac.h)
Dyac_std_exid_impl;



// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

   // Resolve "standard" exception IDs
   Dyac_std_exid_resolve;

   YAC_Init_portaudio(_host);
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_portaudio(_host);
}


#include <yac_host.cpp>
