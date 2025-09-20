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

#ifndef TK_PASTREAM_H__
#define TK_PASTREAM_H__

YG("portaudio")


/* @class PaStream,Object
 */
YC class _PaStreamMacOSWorkgroupJoinToken : public YAC_Object {
  public:
#ifdef YAC_MACOS
   os_workgroup_join_token_s join_token;
#endif // YAC_MACOS

  public:
   _PaStreamMacOSWorkgroupJoinToken(void);
   ~_PaStreamMacOSWorkgroupJoinToken();

   YAC(_PaStreamMacOSWorkgroupJoinToken);
};


/* @class PaStream,Object
 */
YC class _PaStream : public YAC_Object {
  public:
   PaStream *stream;

   void *stream_cbk; // Function object/handle
   void *stream_cbk_context; // YAC script context handle

   YAC_FloatArray *input_buf;
   YAC_FloatArray *output_buf;

   sUI            num_input_channels;
   sUI            num_output_channels;
   PaSampleFormat output_sample_format;
   PaSampleFormat input_sample_format;

#ifdef YAC_MACOS
   os_workgroup_t macos_audio_workgroup;
#endif // YAC_MACOS


  public:
   _PaStream(void);
   ~_PaStream();

   YAC(_PaStream);

   // for internal use only
   void initStream(PaStream *_stream,
                   void *_cbk,
                   sUI _numIn, sUI _numOut,
                   PaSampleFormat _inFmt, PaSampleFormat _outFmt
                   );


   /* @method start
      Start audio I/O
    */
   YM void _start(void);


   /* @method stop
      Stop audio I/O
    */
   YM void _stop(void);


   /* @method abort
    */
   YM void _abort(void);


   /* @method close
      Close stream and free resources.
    */
   YM void _close(void);


   /* @method isStopped:boolean
    */
   YM sSI _isStopped(void);


   /* @method isActive:boolean
    */
   YM sSI _isActive(void);


   /* @method getCpuLoad:float
    */
   YM sF32 _getCpuLoad(void);


   /* @method getNumInputChannels:int
    */
   YM sSI _getNumInputChannels(void);


   /* @method getNumOutputChannels:int
    */
   YM sSI _getNumOutputChannels(void);


   /* @method getInputLatency:float
    */
   YM sF32 _getInputLatency(void);


   /* @method getOutputLatency:float
    */
   YM sF32 _getOutputLatency(void);


   /* @method getSampleRate:float
    */
   YM sF32 _getSampleRate(void);


   /* @method macOSAudioWorkgroupJoin:PaStreamMacOSWorkgroupJoinToken
      Called from worker thread (when entered)
      Returns (deletable) joinToken that has to be passed to macOSAudioWorkgroupLeave() at end of worker thread
    */
   YM void _macOSAudioWorkgroupJoin (YAC_Value *_r);


   /* @method macOSAudioWorkgroupLeave,PaStreamMacOSWorkgroupJoinToken joinToken
      Called from worker thread (at exit)
    */
   YM void _macOSAudioWorkgroupLeave (YAC_Object *_joinToken);

};


#endif // TK_PASTREAM_H__
