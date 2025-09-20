
#ifndef TK_PASTREAM_H__
#define TK_PASTREAM_H__

YG("portaudio")

#include <pthread.h>

#include <alsa/asoundlib.h>

class _PortAudio;


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
   void *stream_cbk; // Function object/handle
   void *stream_cbk_context; // YAC script context handle

   YAC_MutexHandle mtx_async;
   YAC_Host *yac_host_obj;

   sBool     b_in_running;
   pthread_t pthread_id_in;

   sBool     b_out_running;
   pthread_t pthread_id_out;

   YAC_FloatArray *input_buf;
   YAC_FloatArray *output_buf;

   snd_pcm_t *in_handle;
   snd_pcm_t *out_handle;
   sBool     b_running;
   sBool     b_active;
   sBool     b_processing_cbk;
   pthread_t pthread_id;

   unsigned char in_buf[TKPORTAUDIO_MAX_BUFSIZE];
   unsigned char out_buf[TKPORTAUDIO_MAX_BUFSIZE];
   sUI period_size; // actual alsa period size
   sUI num_periods;
   sF32 dbg_avg_avail;

   sUI sample_rate;
   sSI sample_format; // paInt8, paInt16, paInt24, paFloat32
   snd_pcm_format_t alsa_sample_format;
   sUI num_in_ch;
   sUI num_out_ch;

   sF32 in_ringbuf[TKPORTAUDIO_RINGSIZE];
   sUI in_ringbuf_avail;     // #frames
   sUI in_ringbuf_readoff;   // #samples
   sUI in_ringbuf_writeoff;  // #samples

   sF32 out_ringbuf[TKPORTAUDIO_RINGSIZE];
   sUI out_ringbuf_avail;     // #frames
   sUI out_ringbuf_readoff;   // #samples
   sUI out_ringbuf_writeoff;  // #samples


  public:
   void waitProcessCbk(void);
   sSI pcmAvailUpdateRecover(snd_pcm_t *handle);

   sBool hwParamsSet(snd_pcm_t *handle, snd_pcm_hw_params_t *hwparams, int _line);
   sBool hwParamsGet(snd_pcm_t *handle, snd_pcm_hw_params_t *hwparams, int _line);

  public:
   _PaStream(void);
   ~_PaStream();

   YAC(_PaStream);

   // for internal use only
   sBool initStream (_PortAudio *_pa,
                     void *_cbk,
                     _PaStreamParameters *_inParams,
                     _PaStreamParameters *_outParams,
                     sUI _sampleRate,
                     sUI _framesPerBuffer
                     );

   sBool initStream2 (_PortAudio *_pa,
                      void *_cbk,
                      _PaStreamParameters *_inParams,
                      _PaStreamParameters *_outParams,
                      sUI _sampleRate,
                      sUI _numPeriods,
                      sUI _periodSize,
                      sUI _bufferSize
                      );

   void closeHandles (void);
   void resetRingBuf (void);
   void callStreamCallback (const sUI _numFrames);
   void processInput (sBool _bVerbose);
   void processOutput (sBool _bVerbose);
   void writeZeroes (const sUI _numFrames);

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
      API compatibility STUB
    */
   YM void _macOSAudioWorkgroupJoin (YAC_Value *_r);


   /* @method macOSAudioWorkgroupLeave,PaStreamMacOSWorkgroupJoinToken joinToken
      API compatibility STUB
    */
   YM void _macOSAudioWorkgroupLeave (YAC_Object *_joinToken);
};


#endif // TK_PASTREAM_H__
