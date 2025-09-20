
#ifndef __TK_PADEVICEINFO_H__
#define __TK_PADEVICEINFO_H__

YG("portaudio")


/* @class PaDeviceInfo,Object
 */
YCR class _PaDeviceInfo : public YAC_Object {
  public:

  public:
   _PaDeviceInfo(void);
   ~_PaDeviceInfo();

   YAC(_PaDeviceInfo);

   void initDeviceInfo(void);

   
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

};


#endif // __TK_PADEVICEINFO_H__
