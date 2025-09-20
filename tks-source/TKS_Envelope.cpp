/// TKS_Envelope.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
 
#include <math.h> 

#ifndef YAC_NO_HOST
#include "tks.h" 
#include "YAC_FloatArray.h" 
#else
#include <yac.h>
#endif // YAC_NO_HOST

 
#include "TKS_Envelope.h" 
 
 
TKS_Envelope::TKS_Envelope(void) { 
   abs_time      = 0.0; 
   delta_time    = 0.0; 
   current_index = 0; 
   time_advance  = 1.0; 
   b_shreset     = 1; 
   env_fun       = TKS_ENVELOPE_LINEAR;
} 

TKS_Envelope::~TKS_Envelope() { 
} 

void TKS_Envelope::free(void) {

   // (note) keep delta_time / abs_time

   // // sF32 t = getTime();

   YAC_FloatArray::free();

   current_index = 0;

   // // setTime(t);
}

void YAC_VCALL TKS_Envelope::yacGetConstantStringList(YAC_String *_c) { 
   _c->append( 
      "ENV_SH:$0 " 
      "ENV_LINEAR:$1 " 
      "ENV_COSINE:$2 " 
      "ENV_QUADRATIC:$3 " 
      "ENV_CUBIC:$4 " 
      "ENV_QUINTIC:$5 " 
      "ENV_SHRESET:$6 " 
      "NUM_ENVELOPE_TYPES:$7 " 
      ); 
} 

void YAC_VCALL TKS_Envelope::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o!=((YAC_Object*)this)) 
   {
      if(YAC_OP_ASSIGN == _cmd)
      {
#ifndef YAC_NO_HOST
         if(YAC_BCHK(_o, TKS_CLID_ENVELOPE))
#endif // YAC_NO_HOST
         {
            TKS_Envelope *o = (TKS_Envelope *) _o;
            // Copy envelope specific properties, the rest is copied by the YAC_FloatArray yacOperator
            abs_time      = o->abs_time;
            delta_time    = o->delta_time;
            current_index = o->current_index;
            time_advance  = o->time_advance;
            b_shreset     = o->b_shreset;
            env_fun       = o->env_fun;
         }
      }
   }
   YAC_FloatArray::yacOperator(_cmd, _o, _r); 
} 

sF32 TKS_Envelope::get(void) { 
   return getAndResetNew(YAC_TRUE);
}

sF32 TKS_Envelope::yacEnvGetValue(void) { 
   // yac_host->printf("xxx TKS_Envelope::yacEnvGetValue: this=%p elements=%p &elements=%p\n", this, elements, &elements);
   // yac_host->printf("xxx TKS_Envelope::yacEnvGetValue: this=%p current_index=%u num_elements=%u delta_time=%f\n", this, current_index, num_elements);
   return getAndResetNew(YAC_TRUE);
}

void TKS_Envelope::yacEnvTickPrecise(sF32 _prec) {
   tickPrecise(_prec);
}

sF32 TKS_Envelope::getAndResetNew(sBool _bResetNew) {

/*   if(abs_time > 1.04f)
   {
      int xxx = 42;
   }
   */


   sUI ci=current_index<<1; 
   if( ci <num_elements) 
   { 
      if(delta_time<0.0)
      {
         return 0.0f; // Wait until first event
      }
      switch(env_fun) 
      { 
      default: 
         return 0.0f; 
         
      case TKS_ENVELOPE_SH: 
         // ---- sample and hold ---- 
         if(_bResetNew)  // 17Dec2015
         {
            b_shreset = 0; // 04Dec2015
         }
         return  elements[ci+1]; 
         
      case TKS_ENVELOPE_SHRESET: 
         if(b_shreset)  
         {
            if(_bResetNew)  // 17Dec2015
            {
               b_shreset = 0;
            }
            return elements[ci+1]; 
         }
         else  
         {
            return 0.0f; 
         }
         
      case TKS_ENVELOPE_LINEAR: 
         // ---- linear interpolation---- 
         if( (ci+3) < num_elements ) 
         { 
            // --- interpolate to next point 
            sF32 nt = elements[ci+2+0];
            ////printf("xxx TKS_Envelope::get env linear nt=%f Dfltzero=%d\n", nt, Dfltzero(nt));
            if(Dfltzero(nt))
            {
               // Distance to next point too small
               return elements[ci+1];
            }
            else
            {
               sF64 s = (delta_time / nt);
               ////printf("xxx ==> dt=%f nt=%f s=%f\n", delta_time, nt, s);
               if(Dfltzero(s))
               {
                  return elements[ci+1];
               }
               else
               {
                  sF64 t = (elements[ci+1] +	(elements[ci+2+1]-elements[ci+1]) * s);
                  if(Dfltequal_abs(t, 0.0))
                  {
                     return 0.0f;
                  }
                  else
                  {
                     return (sF32) t;
                  }
               }
            }
         } 
         else 
         { 
            // ---- clamp to last value ---- 
            return elements[ci+1]; 
         }
         
         
      case TKS_ENVELOPE_COSINE: 
         // ---- cosine interpolation---- 
         // ---- y=y1+(y2-y1)*(1-cos(PI*x))/2 ---- 
         if( (ci+3) < num_elements ) 
         { 
            // --- interpolate to next point 
            return  (sF32) (
               elements[ci+1] +	 
               (elements[ci+2+1]-elements[ci+1]) *  
               (1.0f-(sF32)::cos((sF32)sM_PI*(delta_time/elements[ci+2+0]))) * 0.5f
                            );
         } 
         else 
         { 
            // ---- clamp to last value ---- 
            return elements[ci+1]; 
         } 

      case TKS_ENVELOPE_QUADRATIC: 
         // ---- quadratic interpolation---- 
         // ---- y= (y1+(y2-y1)*x) * (1-x*x) + (y2+(y3-y2)*(x-1))*x*x ---- 
         if( (ci+5) < num_elements ) // right border 
         { 
            // --- interpolate to next point 
            sF32 y1=elements[ci+0+1]; 
            sF32 y2=elements[ci+2+1]; 
            sF32 y3=elements[ci+4+1]; 
            sF64 x=delta_time/elements[ci+2+0]; 
            sF64 xx=x*x; 
            return  (sF32) (
               ( y1+(y2-y1)*x     ) * (1-xx) + 
               ( y2+(y3-y2)*(x-1) ) * xx     
                            );
         } 
         else 
         { 
            // ---- clamp to last value ---- 
            return elements[ci+1]; 
         }
         break; 

      case TKS_ENVELOPE_CUBIC: 
         // ---- cubic interpolation---- 
         // ---- y= (y1+y3-(y0+y2)*x*x*x + (2*(y0-y1)+y2-y3)*x*x + (y2-y0)*x + y1 ---- 
         if( (ci+7) < num_elements ) 
         { 
            // --- interpolate to next point 
            sF32 y0=elements[ci+0+1]; 
            sF32 y1=elements[ci+2+1]; 
            sF32 y2=elements[ci+4+1]; 
            sF32 y3=elements[ci+6+1]; 
            sF64 x=delta_time/elements[ci+2+0]; 
            sF64 xx=x*x; 
            return (sF32) (
               (y1+y3-(y0+y2))      *xx*x +  
               (2.0f*(y0-y1)+y2-y3) *xx   +  
               (y2-y0)              *x    +  
               y1                         );
         } 
         else 
         { 
            // ---- clamp to last value ---- 
            return elements[ci+1]; 
         } 
         break;

      case TKS_ENVELOPE_QUINTIC: 
         // ---- quintic interpolation---- 
         // ---- y= 3*(y1+y3-y2-y4)      *x*x*x*x*x + 
         // ----    (8*(y2-y1)+7*(y4-y3))*x*x*x*x   + 
         // ----    (6*(y1-y2)+4*(y3-y4))*x*x*x     + 
         // ----    (y3-y1)              *x         + 
         // ----    y2 
         if( (ci+7) < num_elements ) 
         { 
            // --- interpolate to next point 
            sF32 y1=elements[ci+0+1]; 
            sF32 y2=elements[ci+2+1]; 
            sF32 y3=elements[ci+4+1]; 
            sF32 y4=elements[ci+6+1]; 
            sF64 x=delta_time/elements[ci+2+0]; 
            sF64 xx=x*x; 
            sF64 xxxx=xx*xx; 
            return (sF32) (
               3.0f*(y1+y3-y2-y4)          *xxxx*x   + 
               (8.0f*(y2-y1)+7.0f*(y4-y3)) *xxxx     + 
               (6.0f*(y1-y2)+4.0f*(y3-y4)) *xx*x     + 
               (y3-y1)                     *x        + 
               y2                                    ); 
         } 
         else 
         { 
            // ---- clamp to last value ---- 
            return elements[ci+1]; 
         } 
         break; 
      } 
   } 
   else 
   {
      return 0.0f;
   }
} 

void TKS_Envelope::reset(void) { 
   abs_time=0.0f; 
   delta_time=0.0f; 
   current_index=0; 
   b_shreset=1; 
} 

void TKS_Envelope::setSpeed(sF32 _a) { 
   time_advance = (sF64) _a; 
} 

sF32 TKS_Envelope::getSpeed(void) { 
   return (sF32) time_advance; 
} 

sF32 TKS_Envelope::getTime(void) { 
   return (sF32) abs_time; 
} 

sF32 TKS_Envelope::getDeltaTime(void) { 
   return (sF32) delta_time; 
} 

sUI TKS_Envelope::getCurrentIndex(void) { 
   return current_index; 
} 

void TKS_Envelope::setTime(sF32 _t) { 
   // yac_host->printf("xxx TKS_Envelope::setTime(%f)\n", _t);
   
   abs_time = 0.0; 
   /** seek control point to find delta_time **/ 
   current_index = 0; 
   for(; current_index<(num_elements>>1); current_index++)
   {
      sF64 dt = elements[current_index<<1];
      if(_t < (abs_time+dt))
      {
         if( (current_index == 0) && (Dfltnonzero(dt)) )
         {
            // First event is delayed
            b_shreset = 0;
            abs_time = _t;
            delta_time = _t - dt;
         }
         else
         {
            if(current_index>0)
            {
               current_index--;
            }
            delta_time = _t-abs_time;
            abs_time = _t;
            if(Dfltzero(delta_time))
            {
               b_shreset = 1;
            }
            else
            {
               b_shreset = 0;
            }
         }
         return;
      }
      else
      {
         abs_time += dt;
         if(current_index == ((num_elements>>1)-1))
         {
            if(Dfltequal(_t, abs_time))
            {
               // Exactly the last envelope entry?
               b_shreset = 1;
               delta_time = 0.0f;
               return;
            }
            else
            {
               // Beyond last env. entry
               b_shreset = 0;
               delta_time = _t - abs_time;
               abs_time = _t;
               return;
            }
         }
      }
   }
   // No elements
   current_index = 0;
   abs_time      = _t;
   delta_time    = 0.0; 
   b_shreset     = 1;
} 

void TKS_Envelope::yacEnvSetTime(sF32 _t) { 
   setTime(_t);
}

void TKS_Envelope::setInterpolation(sSI _funid) { 
   env_fun   = _funid; 
   b_shreset = 1; 
} 

sSI TKS_Envelope::getInterpolation(void) { 
   return env_fun; 
} 

sSI TKS_Envelope::isNewEvent(void) {
   return b_shreset && (num_elements >= 2);
}

void TKS_Envelope::tickPrecise(sF32 _prec) { 
   sUI curIdx = current_index;
   tickPrecise2(_prec);
   
   sBool bShReset = b_shreset;

   while(curIdx != current_index)
   {
      curIdx = current_index;
      tickPrecise2(0.0f);
   }

   b_shreset = bShReset;
}

void TKS_Envelope::tickPrecise2(sF32 _prec) { 
   sF64 tAdv = time_advance * _prec;
   ////printf("xxx tickPrecise2: delta_time=%f time_advance=%f prec=%f tAdv=%f\n", delta_time, time_advance, _prec, tAdv);
   abs_time += tAdv; 
   sF64 oldDT = delta_time;
   delta_time += tAdv; 

   if(num_elements) 
   { 
      switch(env_fun) 
      { 
      case TKS_ENVELOPE_SH: 
      case TKS_ENVELOPE_SHRESET: 
         if(oldDT < 0.0)
         {
            if(delta_time >= 0.0)
            {
               b_shreset = 1;
            }
         }
         else if( ((sUI)(current_index))!=((num_elements>>1)-1)) // is not last element? 
         { 
            ////yac_host->printf("xxx TKS_Envelope: delta_time=%f shreset=1\n", delta_time);
            if(delta_time >= elements[(current_index+1)*2]) // next element reached? 
            { 
               current_index++; 
               delta_time -= elements[current_index<<1]; 
               b_shreset = 1; 
            } 
            else 
            {
               b_shreset = 0; 
            }
         } 
         else 
         {
            b_shreset = 0; 
         }
         break; 

      case TKS_ENVELOPE_LINEAR: 
      case TKS_ENVELOPE_COSINE: 
         if(num_elements>=(2*2)) 
         {
            if( ((sUI)(current_index))!=((num_elements>>1)-2)) // is not last element? 
            { 
               if(delta_time>=elements[(current_index+1)*2]) // next element reached? 
               { 
                  current_index++; 
                  delta_time -= elements[current_index<<1]; 
                  b_shreset = 1;
               } 
            } 
            else
            {
               // abs_time -= tAdv; 
               // delta_time -= tAdv; 
            }
         }
         break; 

      case TKS_ENVELOPE_QUADRATIC: 
         if(num_elements >= (3*2)) 
         {
            if( ((sUI)(current_index))!=((num_elements>>1)-3)) // is not last element? 
            { 
               if(delta_time >= elements[(current_index+1)*2]) // next element reached? 
               { 
                  current_index++; 
                  delta_time -= elements[current_index<<1]; 
                  b_shreset = 1;
               } 
            } 
            else
            {
               // abs_time -= tAdv; 
               // delta_time -= tAdv; 
            }
         }
         break; 

      case TKS_ENVELOPE_CUBIC: 
      case TKS_ENVELOPE_QUINTIC: 
         if(num_elements >= (4*2)) 
         {
            if( ((sUI)(current_index))!=((num_elements>>1)-4)) // is not last element? 
            { 
               if(delta_time >= elements[(current_index+1)*2]) // next element reached? 
               { 
                  current_index++; 
                  delta_time -= elements[current_index<<1]; 
                  b_shreset = 1;
               } 
            } 
            else
            {
               // abs_time -= tAdv; 
               // delta_time -= tAdv; 
            }
         }
         break; 
      } // switch env_fun 
   } // if num_elements 
} 

void TKS_Envelope::valueAtTimeRaster(sF32 _t, sF32 _res, YAC_Value *_r) {
   _r->initVoid();
   sSI i;
   sF32 t = 0.0f;
   for(i=0; i< (sSI)num_elements; i+=2)
   {
      t += elements[i+0]; // Calc absolute time
      if(t >= _t)
      {
         if( (t < (_t+_res)) )
         {
            ////tkscript->printf(" found value _t=%f _res=%f t=%f deltatime=%f\n", _t, _res, t, elements[i]);
            _r->initFloat(elements[i+1]);
            return;
         }
         else
         {
            return;
         }
      }
   }
}

void TKS_Envelope::valueAtTimeSH(sF32 _t, YAC_Value *_r) {
   // Sample+Hold last value after 't'
   _r->initVoid();
   sSI i;
   sF32 t = 0.0f;
   for(i=0; i< (sSI)num_elements; i+=2)
   {
      t += elements[i+0]; // Calc absolute time
      if(_t >= t)
      {
         _r->initFloat(elements[i+1]);
      }
      else
      {
         return;
      }
   }
}

sF32 TKS_Envelope::valueAtTime(sF32 _t) {
   setTime(_t);
   return get();
}

sSI TKS_Envelope::timeToIndex(sF32 _t) {
   if(num_elements>1)
   {
      sF32 t = elements[0];

      if(_t < t)
      {
         // before first event
         return -2;
      }

      sSI i;
      for(i=0; i< (sSI)(num_elements-2); i+=2)
      {
         if(_t >= t)
         {
            if(_t < (t+elements[i+2]))
            {
               return i>>1;
            }
            t += elements[i+2];
         }
         else
         {
            return -1;
         }
      }
      // Last event
      if(_t >= t)
      {
         return i>>1;
      }
   }
   return -1;
}

sF32 TKS_Envelope::indexToTime(sSI _idx) {
   if(num_elements>1)
   {
      sF32 t = 0.0f;
      _idx *= 2;
      sSI i=0;
      for(; (i<(sSI)num_elements)&&(i<=_idx); i+=2)
      {
         t += elements[i+0];
      }
      return t;
   }
   return -1;
}

#ifndef YAC_NO_HOST
void TKS_Envelope::insertReplaceEvent(sF32 _t, sF32 _value, sF32 _windowSize) {

   removeRange(_t, _windowSize);
   shiftEvents(_t, _windowSize);

   // Treat time as integer (use elipson comparison?)
   sSI et = (sSI) _t;
   sSI ewe = (sSI) (et + _windowSize);
   if(et >= 0)
   {
      sSI t = 0;
      sSI i=0;
      if(num_elements>0)
      {
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if( et < t)
            {
               if(t < ewe)
               {
                  // Overwrite existing event and maintain deltatime
                  elements[i+1] = _value;
                  return;
               }
               else
               {
                  // Insert before current event
                  sSI timeToEvent = t - et; 
                  insert(i, _value);
                  insert(i, elements[i+1+0] - (sF32)timeToEvent);
                  elements[i+2+0] = (sF32) timeToEvent;

                  setTime((sF32)abs_time); // xxx 25Mar2015
                  return;
               }
            }
            else if(Dfltequal(_t, t))
            {
               // Replace current event
               elements[i+1] = _value;
               return;
            }
            // Examine next event
         }
         // Add after last event
         // "t" is the absolute time of the last event
         add((sF32) (et - t));
         add(_value);
         setTime((sF32)abs_time); // xxx 25Mar2015
      }
      else
      {
         add((sF32)et);
         add(_value);
         setTime((sF32)abs_time); // xxx 25Mar2015
      }

   }
}

void TKS_Envelope::insertReplaceEventMSB4(sF32 _t, sSI _value, sF32 _windowSize) {
   // Treat time as integer (use elipson comparison?)
   sSI et = (sSI) _t;
   sSI ewe = (sSI) (et + _windowSize);
   _value = (_value<<4) &0xF0;
   if(et >= 0)
   {
      sSI t = 0;
      if(num_elements>0)
      {
         sSI i=0;
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(et < t)
            {
               if(t < ewe)
               {
                  // Overwrite existing event and maintain deltatime
                  insertReplaceEvent(_t, (sF32) (( ((sU8)elements[i+1])&0xF) | _value), _windowSize);
                  // // elements[i+1] = (sF32) (( ((sU8)elements[i+1])&0xF) | _value);
                  return;
               }
               else
               {
                  // Insert before current event
                  insertReplaceEvent(_t, (sF32)_value, _windowSize);
                  // // sSI timeToEvent = t - et; 
                  // // insert(i, (sF32)_value);
                  // // insert(i, elements[i+1+0] - (sF32)timeToEvent);
                  // // elements[i+2+0] = (sF32) timeToEvent;

                  // // setTime((sF32)abs_time); // xxx 25Mar2015
                  return;
               }
            }
            else if(et == t)
            {
               // Replace current event
               insertReplaceEvent(_t, (sF32) (( ((sU8)elements[i+1])&0xF) | _value), _windowSize);
               // // elements[i+1] = (sF32) (( ((sU8)elements[i+1])&0xF) | _value);
               return;
            }
            // Examine next event
         }
         // Add after last event
         // "t" is the absolute time of the last event
         insertReplaceEvent(_t, (sF32)_value, _windowSize);
         // // add((sF32) (et - t));
         // // add((sF32) _value);
         // // setTime((sF32)abs_time); // xxx 25Mar2015
      }
      else
      {
         insertReplaceEvent(_t, (sF32)_value, _windowSize);
         // // add((sF32)et);
         // // add((sF32)_value);
         // // setTime((sF32)abs_time); // xxx 25Mar2015
      }

   }
}

void TKS_Envelope::insertReplaceEventLSB4(sF32 _t, sSI _value, sF32 _windowSize) {
   // Treat time as integer (use elipson comparison?)
   sSI et = (sSI) _t;
   sSI ewe = (sSI) (et + _windowSize);
   _value &= 0x0F;
   if(et >= 0)
   {
      sSI t = 0;
      sSI i=0;
      if(num_elements>0)
      {
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(et < t)
            {
               if(t < ewe)
               {
                  // Overwrite existing event and maintain deltatime
                  insertReplaceEvent(_t, (sF32) (( ((sU8)elements[i+1])&0xF0) | _value), _windowSize);
                  // // elements[i+1] = (sF32) (( ((sU8)elements[i+1])&0xF0) | _value);
                  return;
               }
               else
               {
                  // Insert before current event
                  insertReplaceEvent(_t, (sF32)_value, _windowSize);
                  // // sSI timeToEvent = t - et; 
                  // // insert(i, (sF32)_value);
                  // // insert(i, elements[i+1+0] - (sF32)timeToEvent);
                  // // elements[i+2+0] = (sF32) timeToEvent;

                  // // setTime((sF32)abs_time); // xxx 25Mar2015
                  return;
               }
            }
            else if(et == t)
            {
               // Replace current event
               insertReplaceEvent(_t, (sF32) (( ((sU8)elements[i+1])&0xF0) | _value), _windowSize);
               // // elements[i+1] = (sF32) (( ((sU8)elements[i+1])&0xF0) | _value);
               return;
            }
            // Examine next event
         }
         // Add after last event
         // "t" is the absolute time of the last event
         insertReplaceEvent(_t, (sF32)_value, _windowSize);
         // // add((sF32) (et - t));
         // // add((sF32)_value);
         // // setTime((sF32)abs_time); // xxx 25Mar2015
      }
      else
      {
         insertReplaceEvent(_t, (sF32)_value, _windowSize);
         // // add((sF32)et);
         // // add((sF32)_value);
         // // setTime((sF32)abs_time); // xxx 25Mar2015
      }

   }
}

void TKS_Envelope::insertReplaceEventPat1(sF32 _t, sSI _value, sF32 _windowSize) {
   // '01A..32D' (first digit)
   sSI et = (sSI) _t;
   sSI ewe = (sSI) (et + _windowSize);
   _value &= 0x03;
   sSI insVal = _value * 4 * 10;
   if(insVal >= 4)
      insVal -= 4; // 11A => 10A
   if(insVal > 127)
      insVal = 127;
   
   if(et >= 0)
   {
      sSI t = 0;
      sSI i=0;
      if(num_elements>0)
      {
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(et < t)
            {
               if(t < ewe)
               {
                  // Overwrite existing event and maintain deltatime
                  sSI oldVal = (sU8)elements[i+1];
                  sSI oldVal3 = oldVal & 3;
                  sSI oldVal2 = ((oldVal >> 2)+1) % 10;
                  insVal = ((_value * 4 * 10) + (oldVal2 * 4)) - 4 + oldVal3;
                  if(insVal > 127)
                     insVal = 127;
                  insertReplaceEvent(_t, (sF32)insVal, _windowSize);
                  // // elements[i+1] = (sF32) insVal;
                  return;
               }
               else
               {
                  // Insert before current event
                  insertReplaceEvent(_t, (sF32)insVal, _windowSize);
                  // // sSI timeToEvent = t - et; 
                  // // insert(i, (sF32)insVal);
                  // // insert(i, elements[i+1+0] - (sF32)timeToEvent);
                  // // elements[i+2+0] = (sF32) timeToEvent;

                  // // setTime((sF32)abs_time); // xxx 25Mar2015
                  return;
               }
            }
            else if(et == t)
            {
               // Replace current event
               sSI oldVal = (sU8)elements[i+1];
               sSI oldVal3 = oldVal & 3;
               sSI oldVal2 = ((oldVal >> 2)+1) % 10;
               insVal = ((_value * 4 * 10) + (oldVal2 * 4)) - 4 + oldVal3;
               if(insVal > 127)
                  insVal = 127;
               insertReplaceEvent(_t, (sF32)insVal, _windowSize);
               // // elements[i+1] = (sF32) insVal;
               return;
            }
            // Examine next event
         }
         // Add after last event
         // "t" is the absolute time of the last event
         insertReplaceEvent(_t, (sF32)insVal, _windowSize);
         // // add((sF32) (et - t));
         // // add((sF32)insVal);
         // // setTime((sF32)abs_time); // xxx 25Mar2015
      }
      else
      {
         insertReplaceEvent(_t, (sF32)insVal, _windowSize);
         // // add((sF32)et);
         // // add((sF32)insVal);
         // // setTime((sF32)abs_time); // xxx 25Mar2015
      }
   }
}

void TKS_Envelope::insertReplaceEventPat2(sF32 _t, sSI _value, sF32 _windowSize) {
   // '01A..32D' (second digit)
   sSI et = (sSI) _t;
   sSI ewe = (sSI) (et + _windowSize);
   _value = _value % 10;
   sSI insVal = _value * 4;
   if(insVal >= 4)
      insVal -= 4;

   if(et >= 0)
   {
      sSI t = 0;
      sSI i=0;
      if(num_elements>0)
      {
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(et < t)
            {
               if(t < ewe)
               {
                  // Overwrite existing event and maintain deltatime
                  sSI oldVal = (sU8)elements[i+1];
                  sSI oldVal1 = ( ((oldVal >> 2)+1) / 10) % 10;
                  sSI oldVal3 = oldVal & 3;
                  insVal = ((oldVal1 * 4 * 10) + (_value * 4)) - 4;
                  if(insVal < 0)
                     insVal = 0;
                  if(insVal > 124)
                  {
                     insVal = ((0 * 4 * 10) + (_value * 4)) - 4;
                  }
                  insVal += oldVal3;
                  insertReplaceEvent(_t, (sF32)insVal, _windowSize);
                  // // elements[i+1] = (sF32) insVal;
                  return;
               }
               else
               {
                  // Insert before current event
                  insertReplaceEvent(_t, (sF32)insVal, _windowSize);
                  // // sSI timeToEvent = t - et; 
                  // // insert(i, (sF32)insVal);
                  // // insert(i, elements[i+1+0] - (sF32)timeToEvent);
                  // // elements[i+2+0] = (sF32) timeToEvent;

                  // // setTime((sF32)abs_time); // xxx 25Mar2015
                  return;
               }
            }
            else if(et == t)
            {
               // Replace current event
               sSI oldVal = (sU8)elements[i+1];
               sSI oldVal1 = ( ((oldVal >> 2)+1) / 10) % 10;
               sSI oldVal3 = oldVal & 3;
               insVal = ((oldVal1 * 4 * 10) + (_value * 4)) - 4;
               if(insVal < 0)
                  insVal = 0;
               if(insVal > 124)
               {
                  insVal = ((0 * 4 * 10) + (_value * 4)) - 4;
               }
               insVal += oldVal3;
               insertReplaceEvent(_t, (sF32)insVal, _windowSize);
               // // elements[i+1] = (sF32) insVal;
               return;
            }
            // Examine next event
         }
         // Add after last event
         // "t" is the absolute time of the last event
         insertReplaceEvent(_t, (sF32)insVal, _windowSize);
         // // add((sF32) (et - t));
         // // add((sF32)insVal);
         // // setTime((sF32)abs_time); // xxx 25Mar2015
      }
      else
      {
         insertReplaceEvent(_t, (sF32)insVal, _windowSize);
         // // add((sF32)et);
         // // add((sF32)insVal);
         // // setTime((sF32)abs_time); // xxx 25Mar2015
      }
   }
}

void TKS_Envelope::insertReplaceEventPat3(sF32 _t, sSI _value, sF32 _windowSize) {
   // '01A..32D' (third digit)
   sSI et = (sSI) _t;
   sSI ewe = (sSI) (et + _windowSize);
   _value = _value & 3;
   sSI insVal = _value;

   if(et >= 0)
   {
      sSI t = 0;
      sSI i=0;
      if(num_elements>0)
      {
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(et < t)
            {
               if(t < ewe)
               {
                  // Overwrite existing event and maintain deltatime
                  sSI oldVal = (sU8)elements[i+1];
                  sSI oldVal12 = oldVal & ~3;
                  insVal = oldVal12+ _value;
                  if(insVal > 127)
                     insVal = 127;
                  insertReplaceEvent(_t, (sF32)insVal, _windowSize);
                  // // elements[i+1] = (sF32) insVal;
                  return;
               }
               else
               {
                  // Insert before current event
                  insertReplaceEvent(_t, (sF32)insVal, _windowSize);
                  // // sSI timeToEvent = t - et; 
                  // // insert(i, (sF32)insVal);
                  // // insert(i, elements[i+1+0] - (sF32)timeToEvent);
                  // // elements[i+2+0] = (sF32) timeToEvent;

                  // // setTime((sF32)abs_time); // xxx 25Mar2015
                  return;
               }
            }
            else if(et == t)
            {
               // Replace current event
               sSI oldVal = (sU8)elements[i+1];
               sSI oldVal12 = oldVal & ~3;
               insVal = oldVal12 + _value;
               if(insVal > 127)
                  insVal = 127;
               insertReplaceEvent(_t, (sF32)insVal, _windowSize);
               // // elements[i+1] = (sF32) insVal;
               return;
            }
            // Examine next event
         }
         // Add after last event
         // "t" is the absolute time of the last event
         insertReplaceEvent(_t, (sF32)insVal, _windowSize);
         // // add((sF32) (et - t));
         // // add((sF32)insVal);
         // // setTime((sF32)abs_time); // xxx 25Mar2015
      }
      else
      {
         insertReplaceEvent(_t, (sF32)insVal, _windowSize);
         // // add((sF32)et);
         // // add((sF32)insVal);
         // // setTime((sF32)abs_time); // xxx 25Mar2015
      }
   }
}

void TKS_Envelope::removeEvent(sF32 _t) {
   // Treat time as integer (use epsilon comparison?)
   sSI et = (sSI) _t;
   if(et >= 0)
   {
      sSI t = 0;
      sSI i=0;
      if(num_elements>0)
      {
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(et == t)
            {
               // Shift following event (if any)
               if( (i+2) < (sSI) num_elements )
               {
                  // Add delta time to next event
                  elements[i+2+0] = elements[i+2+0] + elements[i+0];
               }
               // Remove current event
               _delete(i); // Time
               _delete(i); // Value

               setTime((sF32)abs_time); // xxx 25Mar2015
               return;
            }
            // Examine next event
         }
      }
   }
}

void TKS_Envelope::removeRange(sF32 _t, sF32 _l) {
   // Treat time as integer (use epsilon comparison?)
   sSI et = (sSI) _t; // Range start
   sSI l = (sSI) _l; // Range length
   if(et >= 0)
   {
      sSI t = 0;
      sSI i=0;
      if(num_elements>0)
      {
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(et <= t)
            {
               // Range starts before/at current event
               if( (et+l) > t )
               {
                  // Range crosses current event, remove it
                  // Shift following event (if any)
                  if( (i+2) < (sSI) num_elements )
                  {
                     // Adjust delta time of next event
                     elements[i+2+0] = elements[i+2+0] + elements[i+0];
                  }
                  // Remove current event
                  _delete(i); // Time
                  _delete(i); // Value
                  i -= 2;
                  t -= eventTimeDelta;
               }
               else
               {
                  // Range ends before current event; only shift delta time
                  elements[i+0] -= l;
                  setTime((sF32)abs_time); // xxx 25Mar2015
                  return;
               }
            }

            // Examine next event
         }
      }
   }
   setTime((sF32)abs_time); // xxx 25Mar2015
}

void TKS_Envelope::removeRangeSH(sF32 _t, sF32 _l) {
   // Treat time as integer (use epsilon comparison?)
   sSI et = (sSI) _t; // Range start
   sSI l = (sSI) _l; // Range length
   if(et >= 0)
   {
      sSI t = 0;
      sSI i=0;
      if(num_elements>0)
      {
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(et <= t)
            {
               // Range starts before/at current event
               if( (et+l) > t )
               {
                  // Range crosses current event
                  if( (i+2) < (sSI) num_elements )
                  {
                     sSI tLen = (sSI) (elements[i+2+0]);
                     if( (et+l) >= (t+tLen) )
                     {
                        // Event entirely covered by range, remove it
                        sSI lastLen = (sSI) elements[i+0];
                        _delete(i); // Time
                        _delete(i); // Value
                        if(((sUI)i) < num_elements)
                        {
                           // Adjust delta time of next event
                           elements[i+0] = (sF32) lastLen;
                        }
                        i -= 2;
                        t -= eventTimeDelta;
                        l -= tLen;
                     }
                     else
                     {
                        // Only covered partially 
                        elements[i+0] -= (t - et); // Shift this event
                        elements[i+2+0] -= (l - (t-et)); // Shift next event
                        t = et;
                        l -= (t - et) + (l - (t-et));
                     }
                  }
               }
               else
               {
                  // Range ends before current event; only shift delta time
                  elements[i+0] -= l;
                  setTime((sF32)abs_time); // xxx 25Mar2015
                  return;
               }
            }

            // Examine next event
         }
      }
   }
   setTime((sF32)abs_time); // xxx 25Mar2015
}

void TKS_Envelope::shiftEvents(sF32 _t, sF32 _l) {
   // Treat time as integer (use epsilon comparison?)
   sSI et = (sSI) _t; // Shift offset
   if(et >= 0)
   {
      sSI t = 0;
      sSI i=0;
      if(num_elements>0)
      {
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(et <= t)
            {
               // Shift current event 
               elements[i+0] = elements[i+0] + _l;
               setTime((sF32)abs_time); // xxx 25Mar2015
               return;
            }
            // Examine next event
         }
      }
   }
}

void TKS_Envelope::removeRangeUntilNext(sF32 _t, sF32 _l, sF32 _maxT) {
   // Treat time as integer (use epsilon comparison?)
   sSI et = (sSI) _t; // Range start
   sSI l = (sSI) _l; // Range length
   if(et >= 0)
   {
      sSI t = 0;
      sSI i=0;
      if(num_elements>0)
      {
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(t < _maxT)
            {
               if(et <= t)
               {
                  // Range starts before/at current event
                  if( (et+l) > t )
                  {
                     // Range crosses current event, remove it
                     // Shift following event (if any)
                     if( (i+2) < (sSI) num_elements )
                     {
                        // Adjust delta time of next event
                        elements[i+2+0] = elements[i+2+0] + elements[i+0];
                     }
                     // Remove current event
                     _delete(i); // Time
                     _delete(i); // Value
                     i -= 2;
                     t -= eventTimeDelta;
                  }
                  else
                  {
                     // Range ends before current event; only shift delta time
                     if( (i+2) < (sSI)num_elements )
                     {
                        // Have next event
                        elements[i+0] = elements[i+0] - l;
                        elements[i+2] = elements[i+2] + l;
                     }
                     else
                     {
                        // No next event, simply shift
                        elements[i+0] -= l;
                     }
                     setTime((sF32)abs_time); // xxx 25Mar2015
                     return;
                  }
               }
               // Examine next event
            } // else: out of range (maxT)
         }
      }
   }
   setTime((sF32)abs_time); // xxx 25Mar2015
}

void TKS_Envelope::shiftNextEvent(sF32 _t, sF32 _l, sF32 _maxT) {
   // Treat time as integer (use epsilon comparison?)
   sSI et = (sSI) _t; // Shift offset
   if(et >= 0)
   {
      sSI t = 0;
      sSI i=0;
      if(num_elements>0)
      {
         for(; (i < (sSI)num_elements); i+=2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(t < _maxT)
            {
               if(et <= t)
               {
                  // Shift current event 
                  if(_l >= 0.0f)
                  {
                     // Shift right
                     if( (i+2) < (sSI)num_elements )
                     {
                        // Have next event
                        if( (elements[i+2+0] - _l) <= 0)
                        {
                           // Shifted event crosses next event, cancel
                        }
                        else
                        {
                           elements[i+0] = elements[i+0] + _l;
                           elements[i+2] = elements[i+2] - _l;
                        }
                     }
                     else
                     {
                        // No next event, simply shift
                        elements[i+0] = elements[i+0] + _l;
                     }
                  }
                  else
                  {
                     // Shift left
                     if(0 == i)
                     {
                        if( ((elements[i+0] + _l) < 0.0f) )
                        {
                           _l = -elements[i+0];
                        }

                        if( (i+2) < (sSI)num_elements )
                        {
                           // Have next event
                           elements[i+0] = elements[i+0] + _l;
                           elements[i+2] = elements[i+2] - _l;
                           }
                        else
                        {
                           // No next event, simply shift
                           elements[i+0] = elements[i+0] + _l;
                        }
                     }
                     else
                     {
                        if( ((elements[i+0] + _l) >= 1.0f) || (0 == i) )
                        {
                           if( (i+2) < (sSI)num_elements )
                           {
                              // Have next event
                              elements[i+0] = elements[i+0] + _l;
                              elements[i+2] = elements[i+2] - _l;
                           }
                           else
                           {
                              // No next event, simply shift
                              elements[i+0] = elements[i+0] + _l;
                           }
                        }
                        else
                        {
                           // Shifted event crosses previous event, cancel
                        }
                     }
                  }
                  setTime((sF32)abs_time); // xxx 25Mar2015
                  return;
               }
               // Examine next event
            }
            else
            {
               // Next ctl. out of range
               setTime((sF32)abs_time); // xxx 25Mar2015
               return;
            }
         }
      }
   }
   setTime((sF32)abs_time); // xxx 25Mar2015
}

sF32 TKS_Envelope::getNextEventTimeAfter(sF32 _t) {
   // Treat time as integer (use epsilon comparison?)
   sSI et = (sSI) _t; // Shift offset
   if(et >= 0)
   {
      sSI t = 0;
      sSI i = 0;
      if(num_elements > 0)
      {
         for(; (i < (sSI)num_elements); i += 2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            t += eventTimeDelta; // Calc absolute pattern time
            if(t >= et)
            {
               return (sF32)t;
            }
         }
      }
   }
   return -1;
}

void TKS_Envelope::rotateEvents(sF32 _r, sF32 _startT, sF32 _maxT) {
   // Horribly unoptimized implementation but this is not called very frequently
   sSI r = (sSI)_r;
   sSI maxT = (sSI) _maxT;
   sSI startT = (sSI) _startT;

   if(num_elements > 0)
   {
      if(startT < maxT)
      {
         if(startT >= 0)
         {
            if(0 != r)
            {
               if( (r<0) ? (-r < maxT) : (r < maxT) )
               {
                  TKS_Envelope *src = new TKS_Envelope;
                  src->yacOperatorAssign(this);

                  sSI numT = (maxT - startT);
                  
                  sSI srcT = -r;
                  if(srcT < 0)
                     srcT += numT;

                  sSI t = 0;
                  
                  while(t < numT)
                  {
                     YAC_Value r;
                     r.initVoid();
                     
                     src->valueAtTimeRaster((sF32)(srcT + startT), 1.0f/*res*/, &r);

                     if(YAC_TYPE_FLOAT == r.type)
                     {
                        insertReplaceEvent((sF32)(t + startT), r.value.float_val, 1.0f/*windowSize*/);
                     }
                     else
                     {
                        removeRange((sF32)(t + startT), 1.0f);
                        shiftEvents((sF32)(t + startT), 1.0f);
                     }
                     
                     t++;
                     srcT = (srcT + 1) % numT;
                  }

                  delete src;
                  
                  setTime((sF32)abs_time);
               }
            }
         }
      }
   }

}

sF32 TKS_Envelope::getPreviousEventTimeBefore(sF32 _t) {
   // Return absolute time of event before _t, or -1 if there is no event
   // Treat time as integer (use epsilon comparison?)
   sSI et = (sSI) _t; // Shift offset
   if(et >= 0)
   {
      sSI t = 0;
      sSI i = 0;
      if(num_elements > 0)
      {
         for(; (i < (sSI)num_elements); i += 2)
         {
            sSI eventTimeDelta = (sSI) elements[i+0];
            sSI tNext = t + eventTimeDelta; // Calc absolute pattern time
            // yac_host->printf("xxx t=%d tNext=%d et=%d\n", t, tNext, et);
            if(tNext >= et)
            {
               return (0 == i) ? -1.0f : (sF32)t;
            }
            else if( (i+2) == (sSI)num_elements )
            {
               return (sF32)tNext;
            }
            t = tNext;
         }
      }
   }
   return -1.0f;
}
#endif // YAC_NO_HOST
