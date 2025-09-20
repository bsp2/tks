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

#ifndef TKPORTAUDIO_H__
#define TKPORTAUDIO_H__

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

//#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>

Dyac_std_exid_decl;


#include "portaudio/include/portaudio.h"

#ifdef WIN32
#ifdef PA_USE_ASIO
#include "portaudio/include/pa_asio.h"
#endif
#endif

#ifdef YAC_MACOS
#include <os/workgroup.h>
#include <pa_mac_core.h>
#endif // YAC_MACOS


#define TKPORTAUDIO_MAX_BUFSIZE  4096
#define TKPORTAUDIO_MAX_CHANNELS 32

#include "ying_portaudio.h"



#endif // TKPORTAUDIO_H__
