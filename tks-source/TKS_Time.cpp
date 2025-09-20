/// TKS_Time.cpp
///
/// (c) 2002-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///     - The filetime nanoSeconds code was borrowed from <http://root.cern.ch/root/html/src/TTimeStamp.cxx.html>
///       which was originally written by Rene Brun and Fons Rademakers and released under terms of the
///       GNU Lesser General Public License.
///
///     -
///

#include "tks.h"
#include "TKS_Time.h"

#ifdef YAC_WIN32
#include <windows.h>

static LARGE_INTEGER pfcFrequency;
static LARGE_INTEGER pfcCurrent;
//static LARGE_INTEGER pfcStart;
#endif

#ifndef YAC_WIN32
#include <sys/time.h>
#endif


void tks_init_time(void) {
#ifdef YAC_WIN32
   if( ::QueryPerformanceFrequency(&pfcFrequency) == 0 )
   {
      pfcFrequency.LowPart  = 0;
      pfcFrequency.HighPart = 0;
   }
#endif
}


TKS_Time::TKS_Time(void) {
   yday = 0;
   nanoSec = 0;
   wday = mday = sec = min = hour = month = year = (sU8)0;
}

TKS_Time::~TKS_Time(void) {
}

void TKS_Time::now(void) { 
   time(&utime);
   _localtime();
}

void TKS_Time::_localtime(void) { 
#ifndef YAC_WIN32
   struct timeval tvnano; gettimeofday(&tvnano, NULL);
   nanoSec = (sUI) tvnano.tv_usec*1000;
#endif
   struct tm*ntime = ::localtime(&utime);
   initTime(ntime); 
} 

void TKS_Time::_gmtime(void) { 
   struct tm*ntime = ::gmtime(&utime); 
   initTime(ntime); 
} 

void TKS_Time::initTime(void *_ntime) { 
   struct tm*ntime=(struct tm*)_ntime; 
   yday = (sU16) ntime->tm_yday; 
   wday = (sU8) ntime->tm_wday; 
   mday = (sU8) ntime->tm_mday; 
   sec  = (sU8) ntime->tm_sec; 
   min  = (sU8) ntime->tm_min; 
   hour = (sU8) ntime->tm_hour; 
   month= (sU8) ntime->tm_mon; 
   year = (sU8) ntime->tm_year;
   
#ifdef YAC_WIN32
   
#ifdef DX_USE_FILETIME
   // borrowed from <http://root.cern.ch/root/html/src/TTimeStamp.cxx.html>:
   
   // Set Date/Time to current time as reported by the system.
   // No accounting for nanoseconds with std ANSI functions,
   // ns part faked so that subsequent calls simply add 1 to it
   // this ensures that calls within the same second come back
   // distinct (and sortable). Time is since Jan 1, 1970.
   
   ULARGE_INTEGER time; GetSystemTimeAsFileTime((FILETIME *)&time);
   
   // NT keeps time in FILETIME format which is 100ns ticks since
   // Jan 1, 1601. TTimeStamp uses time in 100ns ticks since Jan 1, 1970,
   // the difference is 134774 days.
   nanoSec = (sSI) ((time.QuadPart * (sU64) 100) % (sU64) 1000000000);
   
   time.QuadPart -=
      (sU64) (1000*1000*10)       // seconds
      * (sU64) (60 * 60 * 24)       // days
      * (sU64) (134774);            // # of days
   
   sec    = (sSI) (time.QuadPart/(sU64) (1000*1000*10));
#endif // DX_USE_FILETIME
   
#ifdef DX_USE_PERFORMANCE_COUNTER
   // Use the CPU performance counter (if available)
   if( ::QueryPerformanceCounter(&pfcCurrent) )
   {
      ////tkscript->printf("[...] [win32] performance counter = %08x.%08x\n", pfcCurrent.HighPart, pfcCurrent.LowPart);
      ////tkscript->printf("[...] [win32] performanceCounterFrequency = %08x.%08x\n", pfcFrequency.HighPart, pfcFrequency.LowPart );
      
      sF64 s = ((sF64)pfcCurrent.QuadPart) / ((sF64)pfcFrequency.QuadPart);
      
      nanoSec = (sSI) (1000000000.0 * (s- ((sSI)s)));
   }
   else
   {
      nanoSec = 0;
   }
   
#endif // DX_USE_PERFORMANCE_COUNTER
   
#endif // YAC_WIN32
} 

void TKS_Time::calc(void) {
   // Calc utime from date
   struct tm ntime;
   time_t utime;
   
   ntime.tm_year  = year;
   ntime.tm_mon   = month;
   ntime.tm_hour  = hour;
   ntime.tm_min   = min;
   ntime.tm_sec   = sec;
   ntime.tm_mday  = mday;
   ntime.tm_isdst = -1;  // setting this to 0 may increase tm_hour by 1, see <http://stackoverflow.com/questions/12122084/confusing-behaviour-of-mktime-function-increasing-tm-hour-count-by-one>
   
   utime = mktime(&ntime);

   // printf("xxx calc(): utime=%u\n", (sUI)utime);

   this->utime = utime;
   
   if(utime != (time_t)-1)
   {
      wday = (sU8) ntime.tm_wday;
      yday = (sU16) ntime.tm_yday;

      year  = ntime.tm_year;
      month = ntime.tm_mon;
      hour  = ntime.tm_hour;
      min   = ntime.tm_min;
      sec   = ntime.tm_sec;
      mday  = ntime.tm_mday;
   }
}

sSI TKS_Time::getNanoSec(void) {
   return nanoSec;
}

void TKS_Time::setNanoSec(sSI _n) {
   if(_n>=0)
   {
      if(_n <= 999999999)
      {
         nanoSec = (sUI) _n;
         return;
      }
   }
   nanoSec = 0;
}

sSI TKS_Time::getSec(void) {
   return sec;
}

void TKS_Time::setSec(sSI _s) {
   if( (_s>=0) && (_s<=59) )
   {
      sec = (sU8)_s;
   }
}

sSI TKS_Time::getMin(void) {
   return min;
}

void TKS_Time::setMin(sSI _m) {
   if( (_m>=0) && (_m<=59) )
   {
      min = (sU8)_m;
   }
}

sSI TKS_Time::getHour(void) {
   return hour;
}

void TKS_Time::setHour(sSI _h) {
   if( (_h>=0) && (_h<=23) )
   {
      hour = (sU8)_h;
   }
}

sSI TKS_Time::getMonth(void) {
   return month;
}

void TKS_Time::setMonth(sSI _m) {
   if( (_m>=0) && (_m<=11) )
   {
      month = (sU8)_m;
   }
}

sSI TKS_Time::getYear(void) {
   return year + 1900;
}

void TKS_Time::setYear(sSI _y) {
   if(_y>=0)
   {
      year = (sU8) (_y - 1900);
   }
}

sSI TKS_Time::getWeekday(void) {
   return wday;
}

sSI TKS_Time::getYearday(void) {
   return yday;
}

sSI TKS_Time::getMonthday(void) {
   return mday;
}

void TKS_Time::setMonthday(sSI _d) {
   if( (_d>=0) && (_d<=31) )
   {
      mday = (sU8) _d;
   }
}

void TKS_Time::setUtime(sUI _i) {
   utime = (time_t) _i;
}

sUI TKS_Time::getUtime(void) {
   return (sUI) utime;
}

void YAC_VCALL TKS_Time::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   
   YAC_SERIALIZE_I16(yday);
   YAC_SERIALIZE_I8 (wday);
   YAC_SERIALIZE_I8 (mday);
   YAC_SERIALIZE_I8 (sec);
   YAC_SERIALIZE_I8 (min);
   YAC_SERIALIZE_I8 (hour);
   YAC_SERIALIZE_I8 (month);
   YAC_SERIALIZE_I8 (year);
   // (todo) write utime
}

sUI YAC_VCALL TKS_Time::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   
   yday  = YAC_DESERIALIZE_I16();
   wday  = YAC_DESERIALIZE_I8();
   mday  = YAC_DESERIALIZE_I8();
   sec   = YAC_DESERIALIZE_I8();
   min   = YAC_DESERIALIZE_I8();
   hour  = YAC_DESERIALIZE_I8();
   month = YAC_DESERIALIZE_I8();
   year  = YAC_DESERIALIZE_I8();
   // (todo) read utime
   
   return 1;
}

sBool YAC_VCALL TKS_Time::yacToString(YAC_String *_s) const {
   // 20201231235959 format (yyyymmddhhmmss)
   _s->alloc(31+1);
   Dyac_snprintf((char*)_s->chars, 31, "%04u%02u%02u%02u%02u%02u", 
                 year + 1900,
                 month + 1,
                 mday,
                 hour,
                 min,
                 sec
                 );
   _s->fixLength(); 
   return YAC_TRUE;
}

void TKS_Time::_getString(YAC_Value *_r) {
   _r->initEmptyString();
   yacToString(_r->value.string_val);
}

void YAC_VCALL TKS_Time::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o!=((YAC_Object*)this))
   {
      if(YAC_OP_ASSIGN == _cmd)
      {
         if(YAC_BCHK(_o, class_ID))
         {
            YAC_CAST_ARG(TKS_Time, o, _o);
            yday  = o->yday;
            wday  = o->wday;
            mday  = o->mday;
            sec   = o->sec;
            min   = o->min;
            hour  = o->hour;
            month = o->month;
            year  = o->year;
            nanoSec = o->nanoSec;
            utime = o->utime;
            return;
         }
      }
      else if(YAC_OP_CEQ == _cmd)
      {
         if(YAC_BCHK(_o, class_ID))
         {
            YAC_CAST_ARG(TKS_Time, o, _o);
            _r->initInt(o->utime == utime);
            return;
         }
         _r->initInt(YAC_FALSE);
         return;
      }
      else if(YAC_OP_CNE == _cmd)
      {
         if(YAC_BCHK(_o, class_ID))
         {
            YAC_CAST_ARG(TKS_Time, o, _o);
            _r->initInt(o->utime != utime);
            return;
         }
         _r->initInt(YAC_FALSE);
         return;
      }
      else if(YAC_OP_CLT == _cmd)
      {
         if(YAC_BCHK(_o, class_ID))
         {
            YAC_CAST_ARG(TKS_Time, o, _o);
            _r->initInt(utime < o->utime);
            return;
         }
         _r->initInt(YAC_FALSE);
         return;
      }
      else if(YAC_OP_CLE == _cmd)
      {
         if(YAC_BCHK(_o, class_ID))
         {
            YAC_CAST_ARG(TKS_Time, o, _o);
            _r->initInt(utime <= o->utime);
            return;
         }
         _r->initInt(YAC_FALSE);
         return;
      }
      else if(YAC_OP_CGT == _cmd)
      {
         if(YAC_BCHK(_o, class_ID))
         {
            YAC_CAST_ARG(TKS_Time, o, _o);
            _r->initInt(utime > o->utime);
            return;
         }
         _r->initInt(YAC_FALSE);
         return;
      }
      else if(YAC_OP_CGE == _cmd)
      {
         if(YAC_BCHK(_o, class_ID))
         {
            YAC_CAST_ARG(TKS_Time, o, _o);
            _r->initInt(utime >= o->utime);
            return;
         }
         _r->initInt(YAC_FALSE);
         return;
      }
   }
   YAC_Object::yacOperator(_cmd, _o, _r); 
}

sBool TKS_Time::_liesBetween(YAC_Object *_tStart, YAC_Object *_tEnd) {
   sBool r = YAC_FALSE;

   if(YAC_BCHK(_tStart, class_ID))
   {
      if(YAC_BCHK(_tEnd, class_ID))
      {
         YAC_CAST_ARG(TKS_Time, tStart, _tStart);
         YAC_CAST_ARG(TKS_Time, tEnd, _tEnd);

         if(tStart->utime <= tEnd->utime)
         {
            r = 
               (utime >= tStart->utime) &&
               (utime <= tEnd->utime)
               ;
         }
      }
   }

   return r;
}

