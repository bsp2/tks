/// TKS_Time.h
//
/// (c) 2001-2020 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#ifndef __TKS_TIME_H__
#define __TKS_TIME_H__

#include <time.h>

class TKS_Time : public YAC_Object {
public:
    sU16 yday;
    sU8 wday;
    sU8 mday;
    sU8 sec;
    sU8 min;
    sU8 hour;
    sU8 month;
    sU8 year; // 1900-2155
    sU32 nanoSec;
    time_t utime;

public:
    TKS_Time(void);
    ~TKS_Time();
 
   sBool         YAC_VCALL yacToString                (YAC_String *_s) const;
   void          YAC_VCALL yacOperator                (sSI, YAC_Object*, YAC_Value*);

   void YAC_VCALL yacSerialize   (YAC_Object *, sUI);
	sUI  YAC_VCALL yacDeserialize (YAC_Object *, sUI);

public:
   void calc           (void); 
	sSI  getHour        (void); 
	sSI  getMin         (void); 
	sSI  getMonth       (void); 
	sSI  getMonthday    (void);
   sSI  getNanoSec     (void);
	sSI  getSec         (void); 
   sUI  getUtime       (void);
	sSI  getWeekday     (void); 
	sSI  getYear        (void); 
	sSI  getYearday     (void); 
   void _gmtime        (void); 
   void initTime       (void*); 
	void setMonthday    (sSI);
   void _localtime     (void); 
	void now            (void); 
	void setHour        (sSI); 
	void setMin         (sSI); 
	void setMonth       (sSI); 
	void setSec         (sSI); 
   void setUtime       (sUI);
	void setYear        (sSI); 
   void setNanoSec     (sSI);
   void _getString     (YAC_Value *_r);
   sBool _liesBetween  (YAC_Object *_tStart, YAC_Object *_tEnd);
};


extern void tks_init_time(void);


#endif
