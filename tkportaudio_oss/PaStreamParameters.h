
#ifndef __TK_PASTREAMPARAMETERS_H__
#define __TK_PASTREAMPARAMETERS_H__

YG("portaudio")


/* @class PaStreamParameters,Object
 */
YC class _PaStreamParameters : public YAC_Object {

  public:
   sSI channel_count;
   sSI sample_format;

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
