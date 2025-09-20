/*
 * Copyright (c) 2009-2023 Ross Bencina and Phil Burk, TkScript bindings by Bastian Spiegel
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

#ifndef TK_PORTAUDIO_H__
#define TK_PORTAUDIO_H__

YG("portaudio")


/* @class PortAudio,Object
Pseudo-static utility class which is used to init/tear down the library, query devices, open streams etc.


For an example on how to use this library, see the "test.tks" script in the tkportaudio/ folder.

*/
YCS class _PortAudio : public YAC_Object {

// portaudio stream sample formats:
// (the following lines are NOT comments!)


/* @constant PA_FLOAT32,int
  single precision floating point sample format (4 bytes)
 */
//#define PA_FLOAT32 YCI paFloat32


/* @constant PA_INT32,int
  32bit signed integer sample format (4 bytes)
 */
//#define PA_INT32   YCI paInt32


/* @constant PA_INT24,int
  24bit signed integer sample format (3 bytes)
 */
//#define PA_INT24   YCI paInt24


/* @constant PA_INT16,int
  16bit signed integer sample format (2 bytes)
 */
//#define PA_INT16   YCI paInt16


/* @constant PA_INT8,int
  8bit signed integer sample format (1 byte)
 */
//#define PA_INT8    YCI paInt8


/* @constant PA_UINT8,int
  8bit unsigned integer sample format (1 byte)
 */
//#define PA_UINT8   YCI paUInt8

/* #define paFloat32        ((PaSampleFormat) 0x00000001) /\**< @see PaSampleFormat *\/ */
/* #define paInt32          ((PaSampleFormat) 0x00000002) /\**< @see PaSampleFormat *\/ */
/* #define paInt24          ((PaSampleFormat) 0x00000004) /\**< Packed 24 bit format. @see PaSampleFormat *\/ */
/* #define paInt16          ((PaSampleFormat) 0x00000008) /\**< @see PaSampleFormat *\/ */
/* #define paInt8           ((PaSampleFormat) 0x00000010) /\**< @see PaSampleFormat *\/ */
/* #define paUInt8          ((PaSampleFormat) 0x00000020) /\**< @see PaSampleFormat *\/ */

  public:
   sBool is_initialized;

  public:
   _PortAudio(void);
   ~_PortAudio();

   YAC(_PortAudio);


   /* @method Initialize
      Initialize the portaudio library
   */
   YM void _Initialize (void);


   /* @method GetNumDevices:int
      Query number of available devices
   */
   YM sSI _GetNumDevices (void);


   /* @method getDeviceInfo,int devIdx:PaDeviceInfo
      Query information for the given device

      @arg devIdx 0..GetNumDevices()
    */
   YM void _GetDeviceInfo (sUI _devIdx, YAC_Value *_r);


   /* @method showControlPanel,int devIdx,int hwnd
      Show ASIO control panel (Windows)
      @arg devIdx Device index (0)
      @arg hwnd Parent window handle
   */
   YM void _ShowControlPanel (sUI _devIdx, sUI _hwnd);


   /* @method GetDefaultInputDeviceIndex:int
      Query default input device index
    */
   YM sSI _GetDefaultInputDeviceIndex (void);


   /* @method GetDefaultOutputDeviceIndex:int
      Query default output device index
    */
   YM sSI _GetDefaultOutputDeviceIndex (void);


   /* @method IsFormatSupported,PaStreamParameters inParams,PaStreamParameters outParams,float sampleRate:boolean
    */
   YM sBool _IsFormatSupported (YAC_Object *_inputParameters, YAC_Object *_outputParameters, sF32 _sampleRate);


   /* @method OpenStream,PaStreamParameters inParams,PaStreamParameters outParams,float sampleRate,Function streamCbk,PaStream retStream:boolean
      Open audio stream and initialize the given PaStream object.

      Note: The stream callback always uses float arrays. Conversions to/from the actual sample formats are transparent to the application.

      @arg inParams Input stream parameters
      @arg outParams Output stream parameters
      @arg sampleRate Sample rate (Hz)
      @arg framesPerBuffer Suggested frames per callback (0=let portaudio decide, results in 8 frames on windows 10/asio)
      @arg streamCbk Stream callback function. Argument list must be (PaStream stream, FloatArray input, FloatArray output, int numFrames).
      @arg retStream The stream object to initialize
      @return 1(true) if the stream has been opened, 0(false) otherwise.
    */
   YM sBool _OpenStream (YAC_Object *_inParams, YAC_Object *_outParams, sF32 _sampleRate, sUI _framesPerBuffer, YAC_Object *_streamCbk, YAC_Object *_retStream);


   /* @method Terminate
      Tear down the portaudio library.

      All streams/info objects become invalid after this call (resp. their internal data does).
    */
   YM void _Terminate (void);


   /* @method GetSampleSize,int sampleFormat:int
      Query sample size (number of bytes) for the given sample format.

      @arg sampleFormat See PA_xxx constants, e.g. §PA_INT24.
    */
   YM sSI _GetSampleSize (sSI _format);

};


#endif // TK_PORTAUDIO_H__
