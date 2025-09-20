#ifndef __TKFREEVERB_H__
#define __TKFREEVERB_H__



YG("freeverb")


#include "Components/revmodel.hpp"


YC class Freeverb : public YAC_Object {
  protected:
   revmodel rev;

  public:
   Freeverb(void);
   ~Freeverb();
   
   YAC(Freeverb);

   YM void mute        (void);
   YM void processMix  (YAC_Object *_in, YAC_Object *_out, sUI _numSamples, sUI _skip);
   YM void setRoomSize (sF32 _value);
   YM sF32 getRoomSize (void);
   YM void setDamp     (sF32 _value);
   YM sF32 getDamp     (void);
   YM void setWet      (sF32 _value);
   YM sF32 getWet      (void);
   YM void setDry      (sF32 _value);
   YM sF32 getDry      (void);
   YM void setWidth    (sF32 _value);
   YM sF32 getWidth    (void);
   YM void setMode     (sF32 _value);
   YM sF32 getMode     (void);
};




#endif // __TKFREEVERB_H__

