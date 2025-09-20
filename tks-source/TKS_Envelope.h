/// TKS_Envelope.h 
/// 
/// (c) 2001-2022 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 
#ifndef __TKS_ENVELOPE_H__
#define __TKS_ENVELOPE_H__


class TKS_Envelope :  public YAC_FloatArray {
public:
   enum __constants {
      TKS_ENVELOPE_SH        = 0,
      TKS_ENVELOPE_LINEAR    = 1,
      TKS_ENVELOPE_COSINE    = 2,
      TKS_ENVELOPE_QUADRATIC = 3,
      TKS_ENVELOPE_CUBIC     = 4,
      TKS_ENVELOPE_QUINTIC   = 5,
      TKS_ENVELOPE_SHRESET   = 6,
        
      TKS_ENVELOPE_PAD 
   }; 
   
public: 
   sUI  current_index; 
   sF64 abs_time; 
   sF64 delta_time; 
   sF64 time_advance; 
   sUI  env_fun; 
   sUI  b_shreset; 

protected:
   void tickPrecise2 (sF32);
   
public: 
   TKS_Envelope(void); 
   ~TKS_Envelope(); 
   
   void YAC_VCALL yacGetConstantStringList(YAC_String *); 
   
   void YAC_VCALL yacOperator(sSI, YAC_Object*, YAC_Value *); 

   sF32 YAC_VCALL yacEnvGetValue (void);
   void YAC_VCALL yacEnvTickPrecise (sF32 _dt);
   void YAC_VCALL yacEnvSetTime (sF32 _t);

   void free (void);  // overwrite YAC_FloatArray::free()
   
   // Note: the methods are all virtual so that this Envelope class
   //       can be used in YAC plugins simply by including this header!
   virtual sF32 get                   (void); 
   virtual sF32 getAndResetNew        (sBool _bResetNew);   // bResetNew=reset "new event" flag after reading
   virtual void reset                 (void); 
   virtual void setInterpolation      (sSI _funid); 
   virtual sSI  getInterpolation      (void); 
   virtual void tickPrecise           (sF32); 
   virtual void setSpeed              (sF32 _a); 
   virtual sF32 getSpeed              (void); 
   virtual sF32 getTime               (void); 
   virtual void setTime               (sF32 _t); 
   virtual sF32 getDeltaTime          (void); 
   virtual sSI  isNewEvent            (void);
   virtual void valueAtTimeRaster     (sF32 _t, sF32 _res, YAC_Value *_r);
   virtual void valueAtTimeSH         (sF32 _t, YAC_Value *_r);
   virtual sF32 valueAtTime           (sF32 _t);
   virtual sSI  timeToIndex           (sF32 _t);
   virtual sF32 indexToTime           (sSI _idx);
#ifndef YAC_NO_HOST
   virtual void insertReplaceEvent    (sF32 _t, sF32 _value, sF32 _windowSize);
   virtual void insertReplaceEventMSB4(sF32 _t, sSI _value, sF32 _windowSize);
   virtual void insertReplaceEventLSB4(sF32 _t, sSI _value, sF32 _windowSize);
   virtual void insertReplaceEventPat1(sF32 _t, sSI _value, sF32 _windowSize);
   virtual void insertReplaceEventPat2(sF32 _t, sSI _value, sF32 _windowSize);
   virtual void insertReplaceEventPat3(sF32 _t, sSI _value, sF32 _windowSize);
   virtual void removeEvent           (sF32 _t); // Remove event at absolute time, retain succeeding event times
   virtual void removeRange           (sF32 _t, sF32 _l); // Remove all events in given time slice
   virtual void removeRangeUntilNext  (sF32 _t, sF32 _l, sF32 _nextT); // Remove range until next event and keep abstime of following event (if any)
   virtual void removeRangeSH         (sF32 _t, sF32 _l); // Remove range or sample&hold style envelope. 
   virtual void shiftEvents           (sF32 _t, sF32 _l); // Shift events starting at _t by _l ticks
   virtual void shiftNextEvent        (sF32 _t, sF32 _l, sF32 _nextT); // Shift only next event and keep abstime of following event (if any)
   virtual sF32 getNextEventTimeAfter (sF32 _t); // Return absolute time of event after _t, or -1 if there is no event
   virtual void rotateEvents          (sF32 _r, sF32 _startT, sF32 _maxT);
   virtual sF32 getPreviousEventTimeBefore (sF32 _t); // Return absolute time of event before _t, or -1 if there is no event
#endif // YAC_NO_HOST
   virtual sUI  getCurrentIndex       (void);
   
}; 

#endif // __TKS_ENVELOPE_H__ 
