
#ifndef __TK_PADEVICEINFO_H__
#define __TK_PADEVICEINFO_H__

YG("portaudio")


/* @class PaDeviceInfo,Object
 */
YCR class _PaDeviceInfo : public YAC_Object {
  public:
   YAC_String *hw_name;  // for snd_pcm_open()
   YAC_String *name;
   sUI sample_rate;
   sSI sample_format;  // pa*
   sSI alsa_sample_format;  // SND_PCM_FORMAT_*
   sUI num_in;
   sUI num_out;
   sF32 min_input_latency;  // in seconds
   sF32 max_input_latency;  // in seconds
   sF32 min_output_latency;  // in seconds
   sF32 max_output_latency;  // in seconds

  public:
   _PaDeviceInfo(void);
   ~_PaDeviceInfo();

   YAC(_PaDeviceInfo);

   void initDeviceInfo (const char *_hwName, const char *_name,
                        sUI _sampleRate,
                        sSI _sampleFormat,
                        sSI _alsaSmpFormat,
                        sUI _numIn, sUI _numOut,
                        sF32 _defMinInputLatency, sF32 _defMaInputLatency,
                        sF32 _defMinOutputLatency, sF32 _defMaxOutputLatency
                        );

   
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
   YM sUI _getMaxInputChannels(void);


   /* @method getMaxOutputChannels:int
    */
   YM sUI _getMaxOutputChannels(void);


   /* @method getInputChannelName,int ch:String
    */
   YM void _getInputChannelName(sUI _ch, YAC_Value *_r);


   /* @method getOutputChannelName,int ch:String
    */
   YM void _getOutputChannelName(sUI _ch, YAC_Value *_r);


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
      Query default sample format (paUInt8, paInt8, paInt16, paInt24, paInt32, paFloat32, ..)
    */
   YM sSI _getDefaultSampleFormat(void);

};


#endif // __TK_PADEVICEINFO_H__
