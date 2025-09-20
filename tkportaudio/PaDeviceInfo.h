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

#ifndef __TK_PADEVICEINFO_H__
#define __TK_PADEVICEINFO_H__

YG("portaudio")


/* @class PaDeviceInfo,Object
 */
YCR class _PaDeviceInfo : public YAC_Object {
  public:
   const PaDeviceInfo *device_info;
   sUI device_index;

  public:
   _PaDeviceInfo(void);
   ~_PaDeviceInfo();

   YAC(_PaDeviceInfo);

   // for internal use only
   void initDeviceInfo(sUI _devIdx, const PaDeviceInfo *_devInfo);
   
   /* @method getName:String
    */
   YM void _getName(YAC_Value *_r);


   /* @method getHostApiIndex:int
    */
   YM sSI _getHostApiIndex(void);


   /* @method getHostApiName:String
    */
   YM void _getHostApiName(YAC_Value *_r);
   

   /* @method getHostApiInfo:PaHostApiInfo
    */
   YM void _getHostApiInfo(YAC_Value *_r);


   /* @method getMaxInputChannels:int
    */
   YM sSI _getMaxInputChannels(void);


   /* @method getMaxOutputChannels:int
    */
   YM sSI _getMaxOutputChannels(void);


   /* @method getInputChannelName,int ch:String
    */
   YM void _getInputChannelName(sSI _ch, YAC_Value *_r);


   /* @method getOutputChannelName,int ch:String
    */
   YM void _getOutputChannelName(sSI _ch, YAC_Value *_r);


   /* @method getDefaultLowInputLatency:float
      Query lowest possible latency (in seconds)
    */
   YM sF32 _getDefaultLowInputLatency(void);


   /* @method getDefaultLowOutputLatency:float
      Query lowest possible latency (in seconds)
    */
   YM sF32 _getDefaultLowOutputLatency(void);


   /* @method getDefaultHighInputLatency:float
      Query highest possible latency (in seconds)
    */
   YM sF32 _getDefaultHighInputLatency(void);


   /* @method getDefaultHighOutputLatency:float
      Query highest possible latency (in seconds)
    */
   YM sF32 _getDefaultHighOutputLatency(void);


   /* @method getDefaultSampleRate:float
      Query default sample rate
    */
   YM sF32 _getDefaultSampleRate(void);


   /* @method getDefaultSampleFormat:int
      Query default sample format (paInt16, paInt24, paInt32, paFloat32, ..)
    */
   YM sSI _getDefaultSampleFormat(void);

};


#endif // __TK_PADEVICEINFO_H__
