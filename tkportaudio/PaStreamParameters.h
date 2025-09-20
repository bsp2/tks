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

#ifndef __TK_PASTREAMPARAMETERS_H__
#define __TK_PASTREAMPARAMETERS_H__

YG("portaudio")


/* @class PaStreamParameters,Object
 */
YC class _PaStreamParameters : public YAC_Object {

  public:
   PaStreamParameters params;

  public:
   _PaStreamParameters(void);
   ~_PaStreamParameters();

   YAC(_PaStreamParameters);

   /* @method getDeviceIndex:int
    */
   YM sSI  _getDeviceIndex(void);


   /* @method setDeviceIndex,int devIdx
    */
   YM void _setDeviceIndex(sSI _devIdx);


   /* @method getChannelCount:int
    */
   YM sSI _getChannelCount(void);


   /* @method setChannelCount,int channelCount
    */
   YM void _setChannelCount(sSI _channelCount);


   /* @method getSampleFormat:int
      See PA_xxx constants, e.g. §PA_INT24.
   */
   YM sSI _getSampleFormat(void);


   /* @method setSampleFormat,int sampleFormat
      See PA_xxx constants, e.g. §PA_INT24.
    */
   YM void _setSampleFormat(sSI _sampleFormat);

   
   /* @method getSuggestedLatency:float
      Query suggested latency (in seconds)
    */
   YM sF32 _getSuggestedLatency(void);


   /* @method setSuggestedLatency,float latency
      Set suggested latency (in seconds)
    */
   YM void _setSuggestedLatency(sF32 _latency);
};


#endif // __TK_PASTREAMPARAMETERS_H__
