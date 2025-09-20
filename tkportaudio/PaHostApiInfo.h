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

#ifndef __TK_PAHOSTAPIINFO_H__
#define __TK_PAHOSTAPIINFO_H__

YG("portaudio")


/* @class PaHostApiInfo,Object
 */
YCR class _PaHostApiInfo : public YAC_Object {
  public:
   const PaHostApiInfo *host_api_info;

  public:
   _PaHostApiInfo(void);
   ~_PaHostApiInfo();

   YAC(_PaHostApiInfo);
   
   
   YM sSI _getApiTypeId(void);

   YM void _getName(YAC_Value *_r);

   YM sSI _getDeviceCount(void);

   YM sSI _getDefaultInputDeviceIndex(void);

   YM sSI _getDefaultOutputDeviceIndex(void);

};

#endif // __TK_PAHOSTAPIINFO_H__
