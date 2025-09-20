
#ifndef __TK_PAHOSTAPIINFO_H__
#define __TK_PAHOSTAPIINFO_H__

YG("portaudio")


/* @class PaHostApiInfo,Object
 */
YCR class _PaHostApiInfo : public YAC_Object {
  public:

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
