
#ifndef TK_PASTREAM_H__
#define TK_PASTREAM_H__

YG("portaudio")

#include <pthread.h>

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

   YAC_FloatArray *input_buf;
   YAC_FloatArray *output_buf;

   int       dsp_fd;
   sBool     b_running;
   sBool     b_active;
   pthread_t pthread_id;

   unsigned char out_buf[PAR_FRAGBYTESIZE];

  public:
   _PaStream(void);
   ~_PaStream();

   YAC(_PaStream);

   // for internal use only
   sBool initStream(void *_cbk,
                    _PaStreamParameters *_inParams,
                    _PaStreamParameters *_outParams
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
