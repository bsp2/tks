/// YAC_FloatArray.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <math.h>

#include "tks.h"
#include "PTN_Node.h"
#include "YAC_IntArray.h"
#include "YAC_FloatArray.h"
#include "TKS_FloatArrayIterator.h"

// imported from api.cpp
extern sF32 YAC_CALL APIC_mathDistancePointPlane2d(sF32 px, sF32 py, sF32 qx, sF32 qy, sF32 nx, sF32 ny);


// based on http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
//
//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code
//  for your own purposes, free or commercial.
//
struct YAC_Biquad2 {
   // per channel 

   // Filter type
   enum {
      LPF  = 1,  // Low Pass
      HPF  = 2,  // High Pass
      BPF  = 3,  // Band Pass
      BRF  = 4,  // Band Reject / Notch
      PEQ  = 5,  // Parametric EQ
      LSH  = 6,  // Low Shelf
      HSH  = 7   // High Shelf
   };

   sF64 x1, x2;  // input delayed by 1/2 samples
   sF64 y1, y2;  // output delayed by 1/2 samples
   sF64 z1, z2;

   inline void reset(void) {
      x1 = x2 = 0.0f;
      y1 = y2 = 0.0f;
      z1 = z2 = 0.0;

      a0 = 1.0;
      a1 = a2 = b1 = b2 = 0.0;
   }

   sF64 a0, a1, a2, b1, b2;

   void calcParams(sSI _type,
                   sF32 _dbGain,
                   sF32 _freq,   // 0..1
                   sF32 _res     // 0..1
                   ) {
#define Q _res
#define Fc _freq

      // (note) sound dies after a short while with fmax=1.0
      // // _freq = sMIN(0.992f, sMAX(_freq, 0.001f));
      _freq = sMIN(0.9999f, sMAX(_freq, 0.001f));

      // (note) sound becomes very quiet with res < 0.5
      _res += 0.5f;
      if(_res < 0.5f)
         _res = 0.5f;
      else if(_res > 1.5f)
         _res = 1.5f;

      sF64 norm;
      sF64 V = pow(10, fabs(_dbGain) / 20.0);  // PEQ, LSH, HSH
      sF64 K = tan(sM_PI * Fc);
      switch(_type)
      {
         case LPF:
            norm = 1 / (1 + K / Q + K * K);
            a0 = K * K * norm;
            a1 = 2 * a0;
            a2 = a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
            break;
            
         case HPF:
            norm = 1 / (1 + K / Q + K * K);
            a0 = 1 * norm;
            a1 = -2 * a0;
            a2 = a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
#if 0
            {
               static int xxx = 0;
               if(0 == (++xxx & 511))
               {
                  yac_host->printf("xxx hpf a0=%f a1=%f b1=%f b2=%f\n", a0, a1, b1, b2);
               }
            }
#endif
            break;
            
         case BPF:
            norm = 1 / (1 + K / Q + K * K);
            a0 = K / Q * norm;
            a1 = 0;
            a2 = -a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
            break;
            
         case BRF:
            norm = 1 / (1 + K / Q + K * K);
            a0 = (1 + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = a0;
            b1 = a1;
            b2 = (1 - K / Q + K * K) * norm;
            break;
            
         case PEQ:
            if(_dbGain >= 0)
            {
               // boost
               norm = 1 / (1 + 1/Q * K + K * K);
               a0 = (1 + V/Q * K + K * K) * norm;
               a1 = 2 * (K * K - 1) * norm;
               a2 = (1 - V/Q * K + K * K) * norm;
               b1 = a1;
               b2 = (1 - 1/Q * K + K * K) * norm;
            }
            else
            {
               // cut
               norm = 1 / (1 + V/Q * K + K * K);
               a0 = (1 + 1/Q * K + K * K) * norm;
               a1 = 2 * (K * K - 1) * norm;
               a2 = (1 - 1/Q * K + K * K) * norm;
               b1 = a1;
               b2 = (1 - V/Q * K + K * K) * norm;
            }
            break;

         case LSH:
            if(_dbGain >= 0)
            {
               // boost
               norm = 1 / (1 + sqrt(2) * K + K * K);
               a0 = (1 + sqrt(2*V) * K + V * K * K) * norm;
               a1 = 2 * (V * K * K - 1) * norm;
               a2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
               b1 = 2 * (K * K - 1) * norm;
               b2 = (1 - sqrt(2) * K + K * K) * norm;
            }
            else
            {
               // cut
               norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
               a0 = (1 + sqrt(2) * K + K * K) * norm;
               a1 = 2 * (K * K - 1) * norm;
               a2 = (1 - sqrt(2) * K + K * K) * norm;
               b1 = 2 * (V * K * K - 1) * norm;
               b2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
            }
            break;

         case HSH:
            if(_dbGain >= 0)
            {
               // boost
               norm = 1 / (1 + sqrt(2) * K + K * K);
               a0 = (V + sqrt(2*V) * K + K * K) * norm;
               a1 = 2 * (K * K - V) * norm;
               a2 = (V - sqrt(2*V) * K + K * K) * norm;
               b1 = 2 * (K * K - 1) * norm;
               b2 = (1 - sqrt(2) * K + K * K) * norm;
            }
            else
            {
               // cut
               norm = 1 / (V + sqrt(2*V) * K + K * K);
               a0 = (1 + sqrt(2) * K + K * K) * norm;
               a1 = 2 * (K * K - 1) * norm;
               a2 = (1 - sqrt(2) * K + K * K) * norm;
               b1 = 2 * (K * K - V) * norm;
               b2 = (V - sqrt(2*V) * K + K * K) * norm;
            }
            break;
      }
#undef Q
#undef Fc
   }

   sF32 filter(const sF32 _inSmp) {
      sF64 out = _inSmp * a0 + z1;
      z1 = _inSmp * a1 + z2 - b1 * out;
      z2 = _inSmp * a2 - b2 * out;

      sF32 out32 = sF32(out);
      return Dyac_denorm_32(out32);
   }
};


YAC_FloatArray::YAC_FloatArray(void) {
   class_ID     = YAC_CLID_FLOATARRAY;
   max_elements = 0u;
   num_elements = 0u;
   own_data     = YAC_FALSE;
   elements     = NULL;
}

YAC_FloatArray::~YAC_FloatArray() {
   YAC_FloatArray::free();
}

sBool YAC_VCALL YAC_FloatArray::yacIteratorInit(YAC_Iterator *_it) const {
   Diteratorinit(TKS_FloatArrayIterator);
   return 1;
}

sBool YAC_VCALL YAC_FloatArray::yacToString(YAC_String *_s) const {
   _s->alloc(2+num_elements*12+1);
   _s->empty();
   _s->append("[");
   sUI i;
   if(num_elements)
   {
      YAC_String s; s.yacValueOfF32(elements[0]);
      _s->append(&s);
   }
   for(i=1; i<num_elements; i++)
   {
      _s->append(", ");
      YAC_String s; s.yacValueOfF32(elements[i]);
      _s->append(&s);
   }
   _s->append("]");
   return 1;
}

void YAC_FloatArray::_getString(YAC_Value *_r) const {
   _r->initEmptyString();
   yacToString(_r->value.string_val);
}

void YAC_FloatArray::_getString2(YAC_Value *_r) const {
   _r->initEmptyString();
   YAC_String *s = _r->value.string_val;
   s->append("[");
   char buf[64];
   sUI ne = num_elements >> 1;
   sUI j = 0u;
   for(sUI i = 0u; i < ne; i++)
   {
      if(i > 0u)
         s->append(", ");
      Dyac_snprintf(buf, 64, "(%g;%g)", elements[j + 0u], elements[j + 1u]);
      s->append(buf);
      j += 2u;
   }
   s->append("]");
}

void YAC_FloatArray::_getString3(YAC_Value *_r) const {
   _r->initEmptyString();
   YAC_String *s = _r->value.string_val;
   s->append("[");
   char buf[96];
   sUI ne = (num_elements / 3u);
   sUI j = 0u;
   for(sUI i = 0u; i < ne; i++)
   {
      if(i > 0u)
         s->append(", ");
      Dyac_snprintf(buf, 64, "(%g;%g;%g)", elements[j+0u], elements[j+1u], elements[j+2u]);
      s->append(buf);
      j += 3u;
   }
   s->append("]");
}

void YAC_VCALL YAC_FloatArray::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();

   YAC_SERIALIZE_I32(num_elements);
   sUI i;
   for(i=0; i<num_elements; i++)
   {
      YAC_SERIALIZE_F32(elements[i]);
   }
}

sBool YAC_FloatArray::alloc(sUI _maxElements) {
   // tkscript->printf("YAC_FloatArray::alloc(%u)\n", _maxElements);
   if(_maxElements > 0u)
   {
      if(_maxElements > TKS_MAX_FLOATARRAYALLOCSIZE)
      {
         tkscript->printf("[---] FloatArray::insane array size (maxelements=%08x)\n", _maxElements);
         return YAC_FALSE;
      }
      YAC_FloatArray::free();
      elements = new(std::nothrow) sF32[_maxElements];
      if(elements)
      {
         max_elements = _maxElements;
         num_elements = 0u;
         own_data     = YAC_TRUE;
         return YAC_TRUE;
      }
      else
      {
         Dprintf("[---] IntArray: failed to allocate %u elements\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // alloc(0) -> free()
      YAC_FloatArray::free();
      return YAC_TRUE;
   }
}

void YAC_FloatArray::_visit(YAC_Object *_fa, sSI _off, sSI _len) {
   YAC_FloatArray::free();

   if(YAC_VALID(_fa))
   {
      if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
      {
         YAC_FloatArray *fa = (YAC_FloatArray *)_fa;
         sUI off = (sUI) _off;
         sUI len = (sUI) _len;
         if(off < fa->num_elements )
         {
            own_data = YAC_FALSE;
            elements = fa->elements + off;
            if( (off+len) <= fa->num_elements)
            {
               num_elements = max_elements = len;
            }
            else
            {
               num_elements = max_elements = fa->num_elements - off;
            }
         }
      }
   }
}

void YAC_FloatArray::_unlinkFrom(YAC_Object *_fa) {
   if((void*)_fa != (void*)this)
   {
      YAC_FloatArray::free();

      if(YAC_VALID(_fa))
      {
         if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
         {
            YAC_CAST_ARG(YAC_FloatArray, fa, _fa);
            own_data = fa->own_data;
            fa->own_data = YAC_FALSE;
            elements = fa->elements;
            num_elements = fa->num_elements;
            max_elements = fa->max_elements;
         }
      }
   }
}

sUI YAC_VCALL YAC_FloatArray::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();

   sUI ne = YAC_DESERIALIZE_I32();

   if(ne > 0)
   {
      if(/*own_data*/alloc(ne))
      {
         num_elements=ne;
         sUI i;
         for(i=0; i<num_elements; i++)
            elements[i]=YAC_DESERIALIZE_F32();
      }
      else
      {
         /// ---- out of memory, skip stream ----
         _ifs->yacStreamSeek(ne*4, YAC_CUR);
      }
   }
   else
   {
      free();
   }

   return 1;
}

void YAC_VCALL YAC_FloatArray::yacArrayCopySize(YAC_Object *_array) {
   //if(own_data)
   //    {
   YAC_FloatArray *o=(YAC_FloatArray*)_array;
   YAC_FloatArray::alloc(o->max_elements);
   //}
}

void YAC_VCALL YAC_FloatArray::yacArraySet(void *_context, sUI _index, YAC_Value *_value) {
   if(((sUI)_index)>=max_elements)
   {
      if(tkscript->configuration.b_autoresizearrays)
      {
         if(YAC_FloatArray::realloc(tks_grow_array(_index+1)))
         {
            num_elements = _index + 1;
            goto yacfloatarray_yacarrayset_1;
         }
      }

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("FloatArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS, msg);
      return;
   }

   if(((sUI)_index) >= num_elements) // track highest access point
   {
      num_elements = _index + 1;
   }

yacfloatarray_yacarrayset_1:
   if(_value->type != YAC_TYPE_FLOAT)
   {
      _value->typecast(YAC_TYPE_FLOAT);
   }
   elements[_index] = _value->value.float_val;
}

void YAC_VCALL YAC_FloatArray::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   if(((sUI)_index)<max_elements)
   {
      _r->initFloat(elements[_index]);
   }
   else
   {
      _r->initVoid();

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("FloatArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

sF32 YAC_FloatArray::get(sSI _index) {
   if( ((sUI)_index) < max_elements )
   {
      return elements[_index];
   }
   else
   {
      return 0.0f;
   }
}

void YAC_FloatArray::_set(sUI _idx, sF32 _val) {
   if(_idx < max_elements)
   {
      elements[_idx] = _val;

      if(_idx >= num_elements)
      {
         num_elements = _idx + 1u;
      }
   }
}

sF32 YAC_FloatArray::_getRev(sSI _index) {
   sUI idx = ((sUI)(num_elements - 1 - _index));
   if(idx < max_elements)
   {
      return elements[idx];
   }
   else
   {
      return 0.0f;
   }
}

void YAC_FloatArray::_setRev(sSI _idx, sF32 _val) {
   const sUI idx = ((sUI)(num_elements - 1 - _idx));
   if(idx < max_elements)
   {
      elements[idx] = _val;
   }
}

sF32 YAC_FloatArray::_getDefault(sUI _index, sF32 _def) {
   return (_index < num_elements) ? elements[_index] : _def;
}

sBool YAC_VCALL YAC_FloatArray::yacArrayAlloc(sUI _maxelements, sUI,sUI,sUI) {
   if(alloc(_maxelements))
   {
      return (elements != NULL);
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool YAC_VCALL YAC_FloatArray::yacArrayRealloc(sUI _maxelements, sUI,sUI,sUI) {
   if(realloc(_maxelements))
   {
      return (elements != NULL);
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool YAC_FloatArray::realloc(sUI _maxElements) {
   // tkscript->printf("YAC_FloatArray::realloc(%u)\n", _maxElements);
   if(_maxElements > 0u)
   {
      sF32 *ne = new(std::nothrow) sF32[_maxElements];
      if(ne)
      {
         sUI i = 0u;
         if(_maxElements <= max_elements)
         {
            for(; i < _maxElements; i++)
            {
               ne[i] = elements[i];
            }
         }
         else
         {
            for(; i < max_elements; i++)
            {
               ne[i] = elements[i];
            }
         }
         if(own_data)
         {
            delete [] elements;
         }
         elements     = ne;
         own_data     = YAC_TRUE;
         max_elements = _maxElements;
         if(num_elements > max_elements)
         {
            num_elements = max_elements;
         }
         return YAC_TRUE;
      }
      else
      {
         Dprintf("\n[---] failed to realloc FloatArray to %u\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // realloc(0) -> free()
      YAC_FloatArray::free();
      return YAC_TRUE;
   }
}

void YAC_VCALL YAC_FloatArray::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o!=((YAC_Object*)this))
   {
      if(YAC_OP_ASSIGN == _cmd)
      {
         if(YAC_BCHK(_o, YAC_CLID_FLOATARRAY))
         {
            YAC_FloatArray *o = (YAC_FloatArray*)_o;

            // Only reallocate if array does not own data or new data does not fit
            sBool bRealloc = !own_data || (max_elements < o->num_elements);

            if(bRealloc)
            {
               YAC_FloatArray::realloc(o->num_elements);
            }

            num_elements = o->num_elements;
            for(sUI i = 0u; i < num_elements; i++)
            {
               elements[i] = o->elements[i];
            }
            return;
         }
         else if(YAC_VALID(_o))
         {
            if(_o->yacArrayGetNumElements())
            {
               sUI ne = _o->yacArrayGetNumElements();
               if(YAC_FloatArray::realloc(ne)) // XXX optimize no keep elements
               {
                  sUI i;
                  num_elements = ne;
                  for(i=0; i<ne; i++)
                  {
                     YAC_Value r;
                     _o->yacArrayGet(NULL, i, &r); // xxx TKS_MT use *real* thread context (exceptions!)
                     r.typecast(YAC_TYPE_FLOAT);
                     elements[i] = r.value.float_val;
                  }
               }
               // xxx print OOM error
            }
            return;
         }
      }
      else if(YAC_OP_ADD == _cmd)
      {
         _join(this, _o);
         return;
      }
      else if(YAC_OP_CEQ == _cmd)
      {
         _r->initInt(_isEqual(_o));
         return;
      }
      else if(YAC_OP_CNE == _cmd)
      {
         _r->initInt(!_isEqual(_o));
         return;
      }
   }
   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_FloatArray::free(void) {
   if(elements)
   {
      if(own_data)
      {
         delete [] elements;
         own_data = YAC_FALSE;
      }
      elements = NULL;
   }
   num_elements = 0u;
   max_elements = 0u;
}

sUI YAC_FloatArray::getMaxElements(void) {
   return max_elements;
}

sUI YAC_FloatArray::getNumElements(void) {
   return num_elements;
}

sBool YAC_FloatArray::add(sF32 _val) {
   if(num_elements < max_elements)
   {
      elements[num_elements++] = _val;
      return YAC_TRUE;
   }
   else
   {
      sUI oldNumElements = num_elements;
      YAC_Value v; v.initFloat(_val);
      yacArraySet(NULL, num_elements, &v); // xxx TKS_MT: should use real thread context (exceptions)
      return (num_elements == (oldNumElements + 1u));
   }
}

sBool YAC_FloatArray::add2(sF32 _val1, sF32 _val2) {
   if((num_elements + 1u) < max_elements)
   {
      elements[num_elements++] = _val1;
      elements[num_elements++] = _val2;
      return YAC_TRUE;
   }
   else
   {
      sSI r = add(_val1);
      if(r)
         r = add(_val2);
      return r;
   }
}

sBool YAC_FloatArray::add3(sF32 _val1, sF32 _val2, sF32 _val3) {
   if((num_elements + 2u) < max_elements)
   {
      elements[num_elements++] = _val1;
      elements[num_elements++] = _val2;
      elements[num_elements++] = _val3;
      return YAC_TRUE;
   }
   else
   {
      sSI r = add(_val1);
      if(r)
         r = add(_val2);
      if(r)
         r = add(_val3);
      return r;
   }
}

sBool YAC_FloatArray::add4(sF32 _val1, sF32 _val2, sF32 _val3, sF32 _val4) {
   if((num_elements + 3u) < max_elements)
   {
      elements[num_elements++] = _val1;
      elements[num_elements++] = _val2;
      elements[num_elements++] = _val3;
      elements[num_elements++] = _val4;
      return YAC_TRUE;
   }
   else
   {
      sSI r = add(_val1);
      if(r)
         r = add(_val2);
      if(r)
         r = add(_val3);
      if(r)
         r = add(_val4);
      return r;
   }
}

sBool YAC_FloatArray::addFromARGB32(sUI _argb32) {
   sF32 fr = ((_argb32 >> 16) & 255u) * (1.0f / 255);
   sF32 fg = ((_argb32 >>  8) & 255u) * (1.0f / 255);
   sF32 fb = ((_argb32      ) & 255u) * (1.0f / 255);
   sF32 fa = ((_argb32 >> 24) & 255u) * (1.0f / 255);
   return add4(fr, fg, fb, fa);
}

sBool YAC_FloatArray::addArray(YAC_Object *_a, sUI _off, sUI _len) {
   if(TKS_VALID(_a))
   {
      sUI ne = _a->yacArrayGetNumElements();
      if(_off < ne)
      {
         if( (_off + _len) > ne )
         {
            _len = (ne - _off);
         }
         while(_len-- > 0)
         {
            YAC_Value r;
            _a->yacArrayGet(NULL, _off++, &r);
            r.typecast(YAC_TYPE_FLOAT);
            add(r.value.float_val);
         }
         return YAC_TRUE;
      }
   }
   else
   {
      Dyac_throw_def(INVALIDPOINTER, "_array is not a valid Object");
   }
   return YAC_FALSE;
}

sSI YAC_FloatArray::addEmpty(sUI _num) {
   if( (num_elements + _num) <= max_elements)  // XXX not 32bit overflow safe
   {
      num_elements += _num;
      return num_elements;
   }
   else
   {
      return 0;
   }
}

void YAC_FloatArray::empty(void) {
   num_elements = 0u;
}

sBool YAC_FloatArray::isEmpty(void) {
   return (0u == num_elements);
}

sSI YAC_FloatArray::insert(sSI _index, sF32 _val) {
   sUI index = (sUI) _index;
   if(index >= num_elements)
   {
      if(index == num_elements)
      {
         return add(_val);
      }
      else
      {
         Dprintf("[---] FloatArray::insert: Cannot insert beyond array boundaries (index=%i, numElements=%i).\n", index, num_elements);
         return 0;
      }
   }
   // Check if current array can hold new element
   if(num_elements == max_elements)
   {
      if(! YAC_FloatArray::realloc(tks_grow_array(num_elements+1)) )
      {
         // re-allocation failed
         return 0;
      }
   }
   // Shift elements
   for(sUI i=num_elements; i>index; i--)
   {
      elements[i] = elements[i-1];
   }
   elements[index] = _val;
   num_elements++;
   return 1;
}

sBool YAC_FloatArray::_delete(sSI _index) {
   if(((sUI)_index) < num_elements)
   {
      if(num_elements > 1u)
      {
         for(sUI i = ((sUI)_index); i < (num_elements - 1u); i++)
            elements[i] = elements[i + 1u];
      }
      num_elements--;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_FloatArray::_delete2(sSI _index) {
   if(((sUI)_index + 1u) < num_elements)
   {
      if(num_elements > 2u)
      {
         for(sUI i = ((sUI)_index); i < (num_elements - 2u); i++)
            elements[i] = elements[i + 2u];
      }
      num_elements -= 2u;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_FloatArray::_delete3(sSI _index) {
   if(((sUI)_index + 2u) < num_elements)
   {
      if(num_elements > 3u)
      {
         for(sUI i = ((sUI)_index); i < (num_elements - 3u); i++)
            elements[i] = elements[i + 3u];
      }
      num_elements -= 3u;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_FloatArray::_delete4(sSI _index) {
   if(((sUI)_index + 3u) < num_elements)
   {
      if(num_elements > 4u)
      {
         for(sUI i = ((sUI)_index); i < (num_elements - 4u); i++)
            elements[i] = elements[i + 4u];
      }
      num_elements -= 4u;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

void YAC_FloatArray::fill(sF32 _v) {
   if(elements && num_elements)
   {
      if(0.0f == _v)
      {
         ::memset((void*)elements, 0u, num_elements * sizeof(sF32));
      }
      else
      {
         for(sUI i=0u; i < num_elements; i++)
            elements[i] = _v;
      }
   }
}

sBool YAC_FloatArray::allocAndFill(sUI _num, sF32 _c) {
   sBool r = (alloc(_num) > 0);
   if(r)
   {
      num_elements = max_elements;
      fill(_c);
   }
   return r;
}

void YAC_FloatArray::fillRegion(sSI _off, sSI _len, sF32 _val) {
   // Clip start
   if(_off < 0)
   {
      _len += _off;
      _off = 0;
   }

   // Clip end
   if(sUI(_off) < num_elements)
   {
      if(sUI(_len) <= num_elements)
      {
         if( (_off + _len) > sSI(num_elements) )
            _len = sSI(num_elements) - _off;

         // Fill
         if(_len > 0)
         {
            sF32 *d = elements + _off;
            const sUI len = sUI(_len);
            for(sUI i = 0u; i < len; i++)
               *d++ = _val;
         }
      }
   }
}

void YAC_FloatArray::fillFromARGB32(sUI _argb32) {
   sF32 fr = ((_argb32 >> 16) & 255u) * (1.0f / 255);
   sF32 fg = ((_argb32 >>  8) & 255u) * (1.0f / 255);
   sF32 fb = ((_argb32      ) & 255u) * (1.0f / 255);
   sF32 fa = ((_argb32 >> 24) & 255u) * (1.0f / 255);
   sUI ne = num_elements / 4u;
   sUI k = 0u;
   for(sUI i = 0u; i < ne; i++)
   {
      elements[k++] = fr;
      elements[k++] = fg;
      elements[k++] = fb;
      elements[k++] = fa;
   }
}

void YAC_FloatArray::reverse(void) {
   if(num_elements && elements)
   {
      sF32 *s = elements;
      sSI l = num_elements >> 1;
      sF32 *d = &elements[num_elements - 1u];
      for(; l >= 0; l--)
      {
         sF32 t = *s;
         *s++ = *d;
         *d-- = t;
      }
   }
}

void YAC_VCALL YAC_FloatArray::yacArraySetWidth(sUI _num) {
   if(_num<=max_elements)
   {
      num_elements=_num;
   }
}

sUI YAC_FloatArray::_read32(YAC_Object *_ifs, sUI _num, sUI _doff) {
   sUI num;
   if(_ifs)
   {
      if(!_num)
      {
         sUI streamOff = _ifs->yacStreamGetOffset();
         sUI bytesleft=_ifs->yacStreamGetSize() - streamOff;
         num=bytesleft>>2;
      }
      else
      {
         num=_num;
      }

      if( (_doff+num) >max_elements)
      {
         if(!YAC_FloatArray::realloc(_doff+num))
         {
            return 0;
         }
      }
      sUI i=0;
      sF32 *e=elements+_doff;
      do
      {
         *e++=_ifs->yacStreamReadF32();
      }
      while ((++i<num)&&(!_ifs->yacStreamEOF()));

      if( (_doff+i)>num_elements)
      {
         num_elements=_doff+i;
      }
      return i;
   }
   return 0;
}

sUI YAC_FloatArray::_read64(YAC_Object *_ifs, sUI _num, sUI _doff) {
   sUI num;
   if(_ifs)
   {
      if(!_num)
      {
         sUI streamOff = _ifs->yacStreamGetOffset();
         sUI bytesleft=_ifs->yacStreamGetSize() - streamOff;
         num=bytesleft>>3;
      }
      else
         num=_num;

      if( (_doff+num) >max_elements)
      {
         if(!YAC_FloatArray::realloc(_doff+num))
         {
            return 0;
         }
      }
      sUI i=0;
      sF32 *e=elements+_doff;
      do
      {
         *e++=(sF32)_ifs->yacStreamReadF64();
      }
      while ((++i<num)&&(!_ifs->yacStreamEOF()));

      if( (_doff+i)>num_elements)
      {
         num_elements=_doff+i;
      }
      return i;
   }
   return 0;
}

sUI YAC_FloatArray::_write32(YAC_Object *_ofs, sUI _num, sUI _soff) {
   if(_ofs)
   {
      sUI num;
      if(!_num)
      {
         num=num_elements;
      }
      else
      {
         num=_num;
      }
      if( (_soff+num) >max_elements)
      {
         num=max_elements;
      }
      sUI i=0;
      sF32 *e=elements+_soff;
      do
      {
         _ofs->yacStreamWriteF32(*e++);
      }
      while ((++i<num)&&(!_ofs->yacStreamGetErrorCode()));

      return i;
   }
   return 0;
}

sUI YAC_FloatArray::_write64(YAC_Object *_ofs, sUI _num, sUI _soff) {
   if(_ofs)
   {
      sUI num;
      if(0u == _num)
      {
         num = num_elements;
      }
      else
      {
         num = _num;
      }
      if((_soff+num) > max_elements)
      {
         num = max_elements;
      }
      sUI i = 0u;
      sF32 *e = elements + _soff;
      do
      {
         _ofs->yacStreamWriteF64(*e++);
      }
      while( (++i < num) && (!_ofs->yacStreamGetErrorCode()) );

      return i;
   }
   return 0u;
}

void YAC_FloatArray::scale(YAC_Object *_a, sF32 _s) {
   if(YAC_BCHK(_a, YAC_CLID_FLOATARRAY))
   {
      YAC_CAST_ARG(YAC_FloatArray, fa, _a);

      if(fa->num_elements > num_elements)
      {
         if(max_elements < fa->num_elements)
            alloc(fa->num_elements);
         num_elements = fa->num_elements;
      }

      if(fa->num_elements <= num_elements)
      {
         for(sUI i = 0; i < fa->num_elements; i++)
         {
            elements[i] = fa->elements[i] * _s;
         }
      }
   }
   else if(YAC_BCHK(_a, YAC_CLID_INTARRAY))
   {
      YAC_CAST_ARG(YAC_IntArray, ia, _a);

      if(ia->num_elements > num_elements)
      {
         if(max_elements < ia->num_elements)
            alloc(ia->num_elements);
         num_elements = ia->num_elements;
      }

      if(ia->num_elements <= num_elements)
      {
         for(sUI i = 0; i < ia->num_elements; i++)
         {
            elements[i] = ia->elements[i] * _s;
         }
      }
   }
}

void YAC_FloatArray::setNumElements(sSI _ne) {
   if( (((sUI)_ne) <= max_elements) )
   {
      num_elements = _ne;
   }
}

void YAC_FloatArray::blend(YAC_Object *_fa, sF32 _f) {
   if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
   {
      YAC_FloatArray *fa=(YAC_FloatArray*)_fa;
      sUI ne=fa->num_elements>num_elements?num_elements:fa->num_elements;
      while(--ne<num_elements)
      {
         elements[ne]+=(fa->elements[ne]-elements[ne])*_f;
      }
   }
}

void YAC_FloatArray::blendAB(YAC_Object *_fa, YAC_Object *_fb, sF32 _f) {
   if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
   {
      if(YAC_BCHK(_fb, YAC_CLID_FLOATARRAY))
      {
         YAC_FloatArray *fa = (YAC_FloatArray*)_fa;
         YAC_FloatArray *fb = (YAC_FloatArray*)_fb;
         sUI ne = fa->num_elements>num_elements ? num_elements : fa->num_elements;
         ne = fb->num_elements>ne ? ne : fb->num_elements;
         while(--ne < num_elements)
         {
            elements[ne] = fa->elements[ne] + (fb->elements[ne] - fa->elements[ne]) * _f;
         }
      }
   }
}

void YAC_FloatArray::copyFrom(YAC_Object *_fa, sSI _off, sSI _len, sSI _doff) {
   if(YAC_BCHK(_fa,YAC_CLID_FLOATARRAY))
   {
      sUI off  = (sUI)_off;
      sUI doff = (sUI)_doff;
      sUI len  = (sUI)_len;
      YAC_FloatArray *fa = (YAC_FloatArray*)_fa;
      sUI opl = (off + len) ;

      if( (doff + len) <= max_elements )
      {
         if(opl <= fa->max_elements)
         {
            sUI i = doff;
            sUI j = off;
            while(j < opl)
            {
               elements[i++] = fa->elements[j++];
            }
         }
      }
   }
}

sF32 YAC_FloatArray::getMin(void) {
   if(num_elements)
   {
      //sF32 min=3.40282347e+38F; // FLT_MAX
	   sF32 min=sF32_MAX; // FLT_MAX
      for(sUI i=0; i<num_elements; i++)
      {
         sF32 t=elements[i];
         if(t<min)
         {
            min=t;
         }
      }
      return min;
   }
   else
   {
      return 0.0f;
   }
}

sSI YAC_FloatArray::getMinIdx(void) {
   if(num_elements > 0)
   {
	   sF32 min = sF32_MAX;
      sSI minIdx = -1;

      for(sUI i=0; i<num_elements; i++)
      {
         sF32 t = elements[i];

         if(t < min)
         {
            min = t;
            minIdx = sSI(i);
         }
      }

      return minIdx;
   }
   else
   {
      return -1;
   }
}

sF32 YAC_FloatArray::getAbsMin(void) {
   if(num_elements)
   {
      sF32 min = sF32_MAX; // FLT_MAX
      for(sUI i=0; i<num_elements; i++)
      {
         yacmem t;
         t.f32 = elements[i];
         t.u32 &= ~0x80000000u;
         if(t.f32 < min)
         {
            min = t.f32;
         }
      }
      return min;
   }
   else
   {
      return 0.0f;
   }
}

sF32 YAC_FloatArray::getMax(void) {
   if(num_elements)
   {
      sF32 max = 1.17549435E-38F; // FLT_MIN

      for(sUI i=0; i<num_elements; i++)
      {
         sF32 t = elements[i];

         if(t > max)
         {
            max = t;
         }
      }
      return max;
   }
   else
   {
      return 0.0f;
   }
}

sSI YAC_FloatArray::getMaxIdx(void) {
   if(num_elements > 0)
   {
      sF32 max = 1.17549435E-38F; // FLT_MIN
      sSI maxIdx = -1;

      for(sUI i=0; i<num_elements; i++)
      {
         sF32 t = elements[i];

         if(t > max)
         {
            max = t;
            maxIdx = sSI(i);
         }
      }

      return maxIdx;
   }
   else
   {
      return 0;
   }
}

sF32 YAC_FloatArray::getAbsMax(void) {
   if(num_elements)
   {
      sF32 max=1.17549435E-38F; // FLT_MIN
      for(sUI i=0; i<num_elements; i++)
      {
         yacmem t;
         t.f32 = elements[i];
         t.u32 &= ~0x80000000u;
         if(t.f32 > max)
         {
            max = t.f32;
         }
      }
      return max;
   }
   else
   {
      return 0.0f;
   }
}

sF32 YAC_FloatArray::getMean(void) {
   if(num_elements)
   {
      sF64 avg=0.0f;
      for(sUI i=0; i<num_elements; i++)
      {
         avg+=(sF64)elements[i];
      }
      return (sF32)(avg/(sF64)num_elements);
   }
   else
   {
      return 0.0f;
   }
}

sF32 YAC_FloatArray::getAbsMean(void) {
   if(num_elements > 0u)
   {
      sF64 avg = 0.0f;
      yaccmemptr s; s.f32 = elements;

      for(sUI i = 0u; i < num_elements; i++)
      {
         yacmem32 t;
         t.f32 = s.f32[i];
         t.u32 &= ~0x80000000u;
         avg += t.f32;
      }
      return (sF32)(avg / num_elements);
   }
   else
   {
      return 0.0f;
   }
}

sF32 YAC_FloatArray::getRms(void) {
   sF32 r = 0.0f;
   if(num_elements > 0u)
   {
      sF64 avg = 0.0f;
      yaccmemptr s; s.f32 = elements;

      for(sUI i = 0u; i < num_elements; i++)
      {
         avg += s.f32[i] * s.f32[i];
      }

      r = (sF32)sqrt(avg / num_elements);
      r += 0.01f;
      r -= 0.01f;
   }
   return r;
}

sF32 YAC_FloatArray::getRmsMax(void) {
   sF32 r = 0.0f;
   if(num_elements > 0u)
   {
      sF64 avg = 0.0f;
      sF64 avgMax = 0.0f;
      yaccmemptr s; s.f32 = elements;

      for(sUI i = 0u; i < num_elements; i++)
      {
         avg += s.f32[i] * s.f32[i];
         if(avg > avgMax)
            avgMax = avg;
      }

      r = (sF32)sqrt(avgMax / num_elements);
      r += 0.01f;
      r -= 0.01f;
   }
   return r;
}

sBool YAC_FloatArray::contains(sF32 _v) {
   sUI i;
   for(i=0; i<num_elements; i++)
   {
      if( (elements[i]>=(_v-YAC_FLOAT_DEVIATION)) && (elements[i]<=(_v+YAC_FLOAT_DEVIATION)) )
      {
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

sSI YAC_FloatArray::indexOf(sF32 _v, sSI _off) {
   sUI i = (sUI) _off;
   for(; i < num_elements; i++)
   {
      if(elements[i] == _v)
      {
         return (sSI)i;
      }
   }
   return -1;
}

sSI YAC_FloatArray::indexOfNot(sF32 _v, sSI _off) {
   sUI i = (sUI) _off;
   for(; i < num_elements; i++)
   {
      if(elements[i] != _v)
      {
         return (sSI)i;
      }
   }
   return -1;
}

sSI YAC_FloatArray::rearrange(YAC_Object *_ia) {
   if(YAC_VALID(_ia))
   {
      if(YAC_Is_IntArray(_ia))
      {
         YAC_IntArray *ia = (YAC_IntArray*) _ia;
         if( (ia->num_elements == num_elements) && (ia->num_elements > 1) && (num_elements > 1) )
         {

            sF32 *tf = new(std::nothrow) sF32[num_elements];
            if(tf)
            {
               sUI i;
               for(i=0; i<num_elements; i++)
               {
                  sUI srcId = ia->elements[i];
                  if(srcId > num_elements)
                  {
                     Dprintf("[---] FloatArray::rearrange: source index (%i) out of bounds.\n", (sSI) srcId);
                     return 0;
                  }
                  tf[i] = elements[srcId];
               }
               for(i=0; i<num_elements; i++)
               {
                  elements[i] = tf[i];
               }

               delete [] tf;

               return 1;
            }
            else
            {
               Dprintf("[---] FloatArray::rearrange: failed to allocate %i indices.\n", num_elements);
            }

         }
      }
   }
   return 0;
}

void YAC_FloatArray::sortByValue(YAC_Object *_ia) {
   if(YAC_BCHK(_ia, YAC_CLID_INTARRAY))
   {
      YAC_IntArray *ia=(YAC_IntArray*)_ia;
      if(elements&&num_elements)
      {
         if(ia->realloc(num_elements))
         {
            ia->num_elements=num_elements;
            for(sUI i=0; i<num_elements; i++)
            {
               ia->elements[i]=i;
            }
            quicksortByValue((sSI*)ia->elements, 0, num_elements-1);
         }
      }
      else
      {
         ia->empty();
      }
   }
}

void YAC_FloatArray::fixNaNToZero(void) {
   for(sUI i = 0u; i < num_elements; i++)
   {
      yac_ieee_float_u t;
      t.f32 = elements[i];
      if(Dyac_chknan_32(t.u32))
         elements[i] = 0.0f;
   }
}

void YAC_FloatArray::quicksortByValue(sSI *_ia, sSI _l, sSI _r) {
   sF32 m = elements[ _ia[(_l + _r) >>1] ];
   // if(Dyac_chknan_32(m))
   //    m = 0.0f;
   sSI i = _l, j = _r, ti;
   while(i <= j)
   {
      sF32 t = elements[_ia[i]];
      // if(Dyac_chknan_32(t))
      //    t = 0.0f;
      while( t < m )
      {
         i++;
         t = elements[_ia[i]];
         // if(Dyac_chknan_32(t))
         //    t = 0.0f;
      }

      t = elements[_ia[j]];
      // if(Dyac_chknan_32(t))
      //    t = 0.0f;
      while( m < elements[_ia[j]] )
      {
         j--;
         t = elements[_ia[j]];
         // if(Dyac_chknan_32(t))
         //    t = 0.0f;
      }
      if(i <= j)
      {
         ti       = _ia[j];
         _ia[j--] = _ia[i];
         _ia[i++] = ti;
      }
   }
   if(_l < j)
   {
      quicksortByValue(_ia, _l, j);
   }
   if(i < _r)
   {
      quicksortByValue(_ia, i, _r);
   }
}

sF32 YAC_FloatArray::winLinear(sF32 _index) {
   if(_index >= 0.0f)
   {
      if(num_elements > 1)
      {
         sSI idx = (sSI) _index;
         sF32 a = _index - (sF32)idx;

         if(sUI(idx) >= num_elements)
         {
            // Clip right
            return 0.0f;
         }
         else if(_index >= (sF32)(num_elements-1))
         {
            // last to first index
            return elements[idx] + (elements[0] - elements[idx]) * a;
         }
         else
         {
            // in be-tween
            return elements[idx] + (elements[idx + 1] - elements[idx]) * a;
         }
      }
      else
      {
         if(num_elements > 0)
         {
            return elements[0];
         }
      }
   }
   // Clip left / empty array
   return 0.0f;
}

sF32 YAC_FloatArray::winCosine(sF32 _index) {
   if(_index>=0.0f)
   {
      if(num_elements>1)
      {
         sSI idx = (sSI) _index;
         sF32 a = _index - (sF32)idx;

			// ---- cosine interpolation----
			// ---- y=y1+(y2-y1)*(1-cos(PI*x))/2 ----

         if(_index >= (sF32)(num_elements-1))
         {
            // last to first index
				return  elements[idx] +
					(elements[0]-elements[idx]) * (1.0f-(sF32)::cos((sF32)sM_PI*a)) * 0.5f;
         }
         else if(_index >= (sF32)num_elements)
         {
            // Clip right
            return 0.0f;
         }
         else
         {
            // in be-tween
				return  elements[idx] +
					(elements[idx+1]-elements[idx]) * (1.0f-(sF32)::cos((sF32)sM_PI*a)) * 0.5f;
         }
      }
      else
      {
         if(num_elements>0)
         {
            return elements[0];
         }
      }
   }
   // Clip left / empty array
   return 0.0f;
}

sF32 YAC_FloatArray::winQuadratic(sF32 _index) {
   if(_index < (sF32)num_elements)
   {
      if(_index>=0.0f)
      {
         if(num_elements>2)
         {
            sSI idx = (sSI) _index;
            sF32 a = _index - (sF32)idx;

            // ---- quadratic interpolation----
            // ---- y= (y1+(y2-y1)*x) * (1-x*x) + (y2+(y3-y2)*(x-1))*x*x ----
            sF32 y1, y2, y3;
            sF32 aa=a*a;


            if(_index >= (sF32)(num_elements-1))
            {
               // last to first index
               y1 = elements[num_elements-1];
               y2 = elements[0];
               y3 = elements[1];
            }
            else if(_index >= (sF32)(num_elements-2))
            {
               y1 = elements[num_elements-2];
               y2 = elements[num_elements-1];
               y3 = elements[0];
            }
            else
            {
               y1 = elements[idx];
               y2 = elements[idx+1];
               y3 = elements[idx+2];
            }

            return
               ( y1+(y2-y1)*a     ) * (1-aa) +
               ( y2+(y3-y2)*(a-1) ) * aa     ;
         }
      }
   }
   // Clip left / right / empty array
   return 0.0f;
}

sF32 YAC_FloatArray::winCubic(sF32 _index) {
   if(_index < (sF32)num_elements)
   {
      if(_index>=0.0f)
      {
         if(num_elements>2)
         {
            sSI idx = (sSI) _index;
            sF32 a = _index - (sF32)idx;

   			// ---- cubic interpolation----
	   		// ---- y= (y1+y3-(y0+y2)*x*x*x + (2*(y0-y1)+y2-y3)*x*x + (y2-y0)*x + y1 ----
            sF32 y1, y2, y3, y4;
            sF32 aa = a * a;

            if(_index >= (sF32)(num_elements-1))
            {
               // last to first index
               y1 = elements[num_elements-2];
               y2 = elements[num_elements-1];
               y3 = elements[0];
               y4 = elements[1];
            }
            else if(_index >= (sF32)(num_elements-2))
            {
               y1 = elements[num_elements-3];
               y2 = elements[num_elements-2];
               y3 = elements[num_elements-1];
               y4 = elements[0];
            }
            else if(_index < 1.0f)
            {
               y1 = elements[num_elements-1];
               y2 = elements[0];
               y3 = elements[1];
               y4 = elements[2];
            }
            else
            {
               y1 = elements[idx-1];
               y2 = elements[idx  ];
               y3 = elements[idx+1];
               y4 = elements[idx+2];
            }


				return
					(y2+y4-(y1+y3))      *aa*a +
					(2.0f*(y1-y2)+y3-y4) *aa   +
					(y3-y1)              *a    +
					y2                         ;
         }
      }
   }
   // Clip left / right / empty array
   return 0.0f;
}

sF32 YAC_FloatArray::winQuintic(sF32 _index) {
   if(_index < (sF32)num_elements)
   {
      if(_index>=0.0f)
      {
         if(num_elements>2)
         {
            sSI idx = (sSI) _index;
            sF32 a = _index - (sF32)idx;

            // ---- quintic interpolation----
            // ---- y= 3*(y1+y3-y2-y4)      *x*x*x*x*x +
            // ----    (8*(y2-y1)+7*(y4-y3))*x*x*x*x   +
            // ----    (6*(y1-y2)+4*(y3-y4))*x*x*x     +
            // ----    (y3-y1)              *x         +
            // ----    y2
            sF32 y1, y2, y3, y4;
            sF32 aa = a * a;
				sF32 aaaa = aa * aa;

            if(_index >= (sF32)(num_elements-1))
            {
               // last to first index
               y1 = elements[num_elements-2];
               y2 = elements[num_elements-1];
               y3 = elements[0];
               y4 = elements[1];
            }
            else if(_index >= (sF32)(num_elements-2))
            {
               y1 = elements[num_elements-3];
               y2 = elements[num_elements-2];
               y3 = elements[num_elements-1];
               y4 = elements[0];
            }
            else if(_index < 1.0f)
            {
               y1 = elements[num_elements-1];
               y2 = elements[0];
               y3 = elements[1];
               y4 = elements[2];
            }
            else
            {
               y1 = elements[idx-1];
               y2 = elements[idx  ];
               y3 = elements[idx+1];
               y4 = elements[idx+2];
            }

				return
				   3.0f*(y1+y3-y2-y4)         *aaaa*a   +
				  (8.0f*(y2-y1)+7.0f*(y4-y3)) *aaaa     +
				  (6.0f*(y1-y2)+4.0f*(y3-y4)) *aa*a     +
				  (y3-y1)                     *a        +
				  y2                                    ;
         }
      }
   }
   // Clip left / right / empty array
   return 0.0f;
}

sSI YAC_FloatArray::_swap(sSI _indexs, sSI _indexd) {
   sUI idxs = (sUI) _indexs;
   sUI idxd = (sUI) _indexd;
   if(idxs != idxd)
   {
      if(idxs < num_elements)
      {
         if(idxd < num_elements)
         {
            sF32 t;
            t = elements[idxd];
            elements[idxd] = elements[idxs];
            elements[idxs] = t;
            return 1;
         }
      }
   }
   return 0;
}

void YAC_FloatArray::_useAll(void) {
   num_elements = max_elements;
}

sF32 YAC_FloatArray::_getFirst(void) {
   if(num_elements > 0)
   {
      return elements[0];
   }
   else
   {
      return 0.0f;
   }
}

sF32 YAC_FloatArray::_getLast(void) {
   if(num_elements > 0)
   {
      return elements[num_elements - 1];
   }
   else
   {
      return 0.0f;
   }
}

void YAC_FloatArray::_interleave(sUI _numDestCh, sUI _destCh,
                                 YAC_Object *_src, sUI _numSrcCh, sUI _srcCh,
                                 sUI _numCh, sUI _numElem)
{
   if(YAC_BCHK(_src, YAC_CLID_FLOATARRAY))
   {
      YAC_CAST_ARG(YAC_FloatArray, src, _src);

      if(_destCh < _numDestCh)
      {
         if(_srcCh < _numSrcCh)
         {
            if( (_numElem * _numCh) <= max_elements )
            {
               if( (_numElem * _numCh) <= src->max_elements )
               {
                  if((_numElem * _numDestCh) <= max_elements)
                  {
                     if((_numElem * _numSrcCh) <= src->max_elements)
                     {
                        sF32 *s = src->elements + _srcCh;
                        sF32 *d = elements + _destCh;
                        sUI i;
                        for(i=0; i<_numElem; i++)
                        {
                           sUI ch;
                           for(ch=0; ch<_numCh; ch++)
                           {
                              d[ch] = s[ch];
                           }
                           d += _numDestCh;
                           s += _numSrcCh;
                        }
                     }
                  }
               }
            }
         }
      }
   }
   else
   {
      Dyac_throw_def(INVALIDPOINTER, "src is not a valid FloatArray object");
   }
}

sSI YAC_FloatArray::bezierCubicDeCasteljau2dRec(sF32 a1x, sF32 a1y,
                                                sF32 a2x, sF32 a2y,
                                                sF32 a3x, sF32 a3y,
                                                sF32 a4x, sF32 a4y,
                                                sSI _rec, sSI _recMax,
                                                sF32 _thresholdB, sF32 _thresholdP,
                                                YAC_FloatArray *_retT, sF32 _ct
                                                )
{
   // Recursively divide bezier curve
   // for diagram see http://www.math.washington.edu/~king/coursedir/m445w01/lab/lab04/lab04.html
   //
   //      a2___
   //      /    b2------- a3
   //     /   /   \      /
   //    /__c1--d--c2   /
   //   b1          \  /
   //  /             b3
   // a1             /
   //               /
   //              a4


   // Find midpoint between (a1x, a1y) and (a2x, a2y) ("b1")
   sF32 b1x, b1y;
   b1x = (a1x + a2x) * 0.5f;
   b1y = (a1y + a2y) * 0.5f;

   // Find midpoint between (a2x, a2y) and (a3x, a3y) ("b2")
   sF32 b2x, b2y;
   b2x = (a2x + a3x) * 0.5f;
   b2y = (a2y + a3y) * 0.5f;

   // Find midpoint between (a3x, a3y) and (a4x, a4y) ("b3")
   sF32 b3x, b3y;
   b3x = (a3x + a4x) * 0.5f;
   b3y = (a3y + a4y) * 0.5f;

   // Find midpoint between (b1x, b1y) and (b2x, b2y) ("c1")
   sF32 c1x = (b1x + b2x) * 0.5f;
   sF32 c1y = (b1y + b2y) * 0.5f;

   // Find midpoint between (b2x, b2y) and (b3x, b3y) ("c2")
   sF32 c2x = (b2x + b3x) * 0.5f;
   sF32 c2y = (b2y + b3y) * 0.5f;

   // Find (bezier) midpoint d between (c1x, c1y) and (c2x, c2y)
   sF32 d1x = (c1x + c2x) * 0.5f;
   sF32 d1y = (c1y + c2y) * 0.5f;

   sF32 distP = sqrtf( (c2x-c1x)*(c2x-c1x) + (c2y-c1y)*(c2y-c1y) );
   sBool bDistP = (distP > _thresholdP);

   sF32 dist = 0.0f;

   if(!bDistP)
   {
      // Calc normal vector for (a1, a4)
      sF32 nx = -(a4y - a1y);
      sF32 ny = (a4x - a1x);

      // Calc distance of bezier point to (a1, a4) line
      dist = APIC_mathDistancePointPlane2d(d1x, d1y, a1x, a1y, nx, ny);
      dist = sABS(dist);//xxx
   }

   // yac_host->printf("xxx rec=%d recMax=%d distP=%f thresholdP=%f distB=%f thresholdB=%f\n", _rec, _recMax, distP, _thresholdP, dist, _thresholdB);
   if( (_rec < 2) || ( (bDistP || (dist > _thresholdB)) && (_rec < _recMax) ) )
   {
      sF32 dt = 1.0f / (1<<(_rec+2));

      // Left bezier curve (a1, b1, c1, d1)
      bezierCubicDeCasteljau2dRec(
         a1x, a1y,
         b1x, b1y,
         c1x, c1y,
         d1x, d1y,
         _rec+1, _recMax,
         _thresholdB, _thresholdP,
         _retT, _ct - dt
         );

      // Output bezier midpoint
      add(d1x);
      add(d1y);

      // Output curve time
      if(NULL != _retT)
      {
         _retT->add(_ct);
      }

      // Right bezier curve (d1, c2, b3, a4)
      return bezierCubicDeCasteljau2dRec(
         d1x, d1y,
         c2x, c2y,
         b3x, b3y,
         a4x, a4y,
         _rec+1, _recMax,
         _thresholdB, _thresholdP,
         _retT, _ct + dt
         );
   }
   else
   {
      // Limit/Threshold reached, rewind stackframes
      return _rec;
   }
}

void YAC_FloatArray::_bezierCubicDeCasteljau2d(sF32 x0, sF32 y0,
                                               sF32 x1, sF32 y1,
                                               sF32 x2, sF32 y2,
                                               sF32 x3, sF32 y3,
                                               sF32 _scaleX, sF32 _scaleY,
                                               sSI _recMax,
                                               sF32 _thresholdB, sF32 _thresholdP,
                                               YAC_Object *_retT
                                              )
{
   // Note: do *not* empty so multiple bezier segments can be added to this array
   ////empty();

   sUI numStart = num_elements;

   // Curve time return array
   YAC_FloatArray *retT;
   if(YAC_BCHK(_retT, YAC_CLID_FLOATARRAY))
   {
      retT = (YAC_FloatArray*) _retT;
   }
   else
   {
      retT = NULL;
   }

   // Start point
   add(x0);
   add(y0);

   if(NULL != retT)
      retT->add(0.0f);

   // Calculate bezier curve (add x/y coords to array)
   bezierCubicDeCasteljau2dRec(
      x0, y0,
      x1, y1,
      x2, y2,
      x3, y3,
      0/*rec*/, _recMax,
      _thresholdB, _thresholdP,
      retT, 0.5f
      );

   // End point
   add(x3);
   add(y3);

   if(NULL != retT)
      retT->add(1.0f);

   // Scale generated path (newly added elements)
   if(1.0f != _scaleX || 1.0f != _scaleY)
   {
      for(sUI i = numStart; i < num_elements; i += 2u)
      {
         elements[i + 0u] = elements[i + 0u] * _scaleX;
         elements[i + 1u] = elements[i + 1u] * _scaleY;
      }
   }
}

sSI YAC_FloatArray::findNearestIdx1d(sF32 _x, sUI _off, sUI _stride, sF32 _maxDist) {
   sSI minIdx = -1;
   if( num_elements >= 1u && _stride > 0u )
   {
      sF32 minDist = _maxDist;
      sUI k = _off;
      while(k < num_elements)
      {
         sF32 px = elements[k];
         sF32 d = sABS(px - _x);
         if(d < minDist)
         {
            minDist = d;
            minIdx = (sSI)k;
         }
         k += _stride;
      }
   }
   return minIdx;
}

sSI YAC_FloatArray::findNearestIdx2d(sF32 _x, sF32 _y, sUI _off, sUI _stride, sF32 _maxDist, sF32 _aspect) {
   sSI minIdx = -1;
   if((num_elements >= 2) && (_stride > 0))
   {
      sF32 minDist = _maxDist * _maxDist;
      sUI k = _off;
      sF32 oma;
      if(_aspect > 0.0f)
      {
         oma = 1.0f / _aspect;
      }
      else
      {
         oma = 1.0f;
      }
      while(k < (num_elements-1))
      {
         sF32 px = elements[k+0];
         sF32 py = elements[k+1];
         sF32 dx = (_x - px);
         sF32 dy = (_y - py) * oma;
         sF32 d = /*sqrt*/(dx*dx + dy*dy);
         //printf("xxx k=%d p=(%f; %f) d=(%f(%f); %f(%f)) d=%f\n", k, px, py, dx, dx*dx, dy, dy*dy, d);
         if(d < minDist)
         {
            //printf("xxx new minDist d=%f\n", d);
            minDist = d;
            minIdx = (sSI)k;
         }
         k += _stride;
      }
      //printf("xxx return minIdx=%d, minDist=%f\n", minIdx, minDist);
   }
   return minIdx;
}

sF32 YAC_FloatArray::sum(sUI _off, sUI _num) {
   sF32 r = 0.0f;

   if(_off < num_elements)
   {
      if(_num <= num_elements)
      {
         sUI endOff = (_off + _num);

         if(endOff >= _off)
         {
            sUI k = _off;

            while(k < endOff)
            {
               r += elements[k];

               k++;
            }
         }
      }
   }

   return r;
}

sF32 YAC_FloatArray::sumPathLen1d(sUI _off, sUI _num) {
   sF32 r = 0.0f;

   if(_off < num_elements)
   {
      if(_num <= num_elements)
      {
         sUI endOff = (_off + _num);

         if(endOff >= _off)
         {
            sUI k = _off;
            sF32 pLast = elements[k];
            k += 1;

            while(k < endOff)
            {
               sF32 pNew = elements[k];
               sF32 dist = pNew - pLast;
               r += dist;
               pLast = pNew;
               k += 1;
            }
         }
      }
   }
   return r;
}

sF32 YAC_FloatArray::sumPathLen2d(sUI _off, sUI _num) {
   _off *= 2;
   _num *= 2;
   sF64 r = 0.0;
   if(_off < num_elements)
   {
      //if(_num <= num_elements)
      if((_num <= num_elements) && ((_off+_num) <= num_elements))
      {
         sUI endOff = (_off + _num);
         if(endOff >= _off)
         {
            sUI k = _off;
            _num /= 2;
            sF64 pxLast = elements[k + 0];
            sF64 pyLast = elements[k + 1];
            k += 2;
            while(k < endOff)
            {
               sF64 pxNew = elements[k + 0];
               sF64 pyNew = elements[k + 1];
               sF64 distX = pxNew - pxLast;
               sF64 distY = pyNew - pyLast;
               sF64 dist = sqrt(distX*distX + distY*distY);
               r += dist;
               ////printf("xxx i=%d l=(%f; %f) n=(%f; %f) d=(%f; %f) d=%f r=%f\n",
               ////   k/2, pxLast, pyLast, pxNew, pyNew, distX, distY, dist, r);
               pxLast = pxNew;
               pyLast = pyNew;
               k += 2;
            }
         }
      }
   }
   return (sF32) r;
}

sBool YAC_FloatArray::loadFromStreamUnsigned8(YAC_Object *_stream, sUI _numElements) {
   if(alloc(_numElements))
   {
      num_elements = max_elements;

      sUI i = 0;
      while(i < num_elements)
      {
         sS32 s = _stream->yacStreamReadI8();
         elements[i++] = (sF32) ( ((s - 128) << 24) / double((sUI(1)<<31)-1) );
      }

      return 1;
   }

   return 0;
}

sBool YAC_FloatArray::loadFromStreamSigned16(YAC_Object *_stream, sUI _numElements) {
   if(alloc(_numElements))
   {
      num_elements = max_elements;

      sUI i = 0;
      while(i < num_elements)
      {
         sS32 s = _stream->yacStreamReadI16();
         elements[i++] = (sF32) ( (s << 16) / double((sUI(1)<<31)-1) );
      }

      return 1;
   }

   return 0;
}

sBool YAC_FloatArray::loadFromStreamSigned24(YAC_Object *_stream, sUI _numElements) {
   if(alloc(_numElements))
   {
      num_elements = max_elements;

      sUI i = 0;
      while(i < num_elements)
      {
         sS32 sa = _stream->yacStreamReadI8(); // read bits 0..7
         sS32 sb = _stream->yacStreamReadI8(); // read bits 8..15
         sS32 sc = _stream->yacStreamReadI8(); // read bits 16..23
         sS32 s = ((sc&255)<<24) | ((sb&255)<<16) | ((sa&255)<<8);
         elements[i++] = (sF32) ( s / double((sUI(1)<<31)-1) );
      }

      return 1;
   }

   return 0;
}

sBool YAC_FloatArray::loadFromStreamFloat32(YAC_Object *_stream, sUI _numElements) {
   if(alloc(_numElements))
   {
      num_elements = max_elements;

      // (todo) read array ?
      sUI i = 0;
      while(i < num_elements)
      {
         elements[i++] = _stream->yacStreamReadF32();
      }

      return 1;
   }

   return 0;
}

sBool YAC_FloatArray::_isEqual(YAC_Object *_fa) {
   if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
   {
      YAC_CAST_ARG(YAC_FloatArray, fa, _fa);

      if(fa->num_elements == num_elements)
      {
         sUI i;
         for(i=0; i<num_elements; i++)
         {
            // // printf("xxx compare[%u] l=%f r=%f\n", i, fa->elements[i], elements[i]);
            if(fa->elements[i] != elements[i])
            {
               return YAC_FALSE;
            }
         }
         return YAC_TRUE;
      }
   }
   else if(YAC_VALID(_fa))
   {
      // Compare to any array object (cast elements to float)
      sUI ne = _fa->yacArrayGetNumElements();
      if(ne == num_elements)
      {
         for(sUI i = 0u; i < ne; i++)
         {
            YAC_Value t;
            yacArrayGet(NULL/*ctx*/, i, &t);
            t.typecast(YAC_TYPE_FLOAT);
            if(Dfltnotequal(t.value.float_val, elements[i]))
            {
               t.unsetFast();
               return YAC_FALSE;
            }
            t.unsetFast();
         }
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

sBool YAC_FloatArray::_join(YAC_Object *_a, YAC_Object *_b) {
   if(YAC_Is_FloatArray(_a) && YAC_Is_FloatArray(_b))
   {
      // Join FloatArrays
      YAC_CAST_ARG(YAC_FloatArray, a, _a);
      YAC_CAST_ARG(YAC_FloatArray, b, _b);

      sBool bDone = YAC_FALSE;

      sUI neAB = (a->num_elements + b->num_elements);

      if(neAB > 0)
      {
         if( ((void*)this) == ((void*)_a) )
         {
            if(neAB <= max_elements)
            {
               sUI i = 0u;
               sUI k = num_elements;
               for(; i < b->num_elements; i++)
               {
                  elements[k++] = b->elements[i++];
               }
               num_elements = neAB;
               bDone = YAC_TRUE;
            }
         }

         if(!bDone)
         {
            sF32 *d = new(std::nothrow) sF32[neAB];

            if(NULL != d)
            {
               sUI k = 0;
               sUI i;

               i = 0;
               while(i < a->num_elements)
               {
                  d[k++] = a->elements[i++];
               }

               i = 0;
               while(i < b->num_elements)
               {
                  d[k++] = b->elements[i++];
               }

               free();
               elements = d;
               own_data = 1;
               num_elements = max_elements = neAB;
            }
            else
            {
               printf("[---] YAC_FloatArray::join: failed to allocate %u elements.\n", neAB);
               return 0;
            }
         }
      }  // if neAB > 0
      else
      {
         YAC_FloatArray::free();
      }
   }
   else
   {
      // Generic
      sUI neA;
      if(YAC_VALID(_a))
      {
         neA = _a->yacArrayGetNumElements();
      }
      else
      {
         neA = 0;
      }

      sUI neB;
      if(YAC_VALID(_b))
      {
         neB = _b->yacArrayGetNumElements();
      }
      else
      {
         neB = 0;
      }

      sUI neAB = (neA + neB);
      if(neAB > 0)
      {
         sF32 *d = new(std::nothrow) sF32[neAB];

         if(NULL != d)
         {
            sUI k = 0;
            sUI i;

            i = 0;
            while(i < neA)
            {
               YAC_Value r;
               _a->yacArrayGet(NULL/*context*/, i++, &r);
               r.typecast(YAC_TYPE_FLOAT);
               d[k++] = r.value.float_val;
               r.unsetFast();
            }

            i = 0;
            while(i < neB)
            {
               YAC_Value r;
               _b->yacArrayGet(NULL/*context*/, i++, &r);
               r.typecast(YAC_TYPE_FLOAT);
               d[k++] = r.value.float_val;
               r.unsetFast();
            }

            free();
            elements = d;
            own_data = 1;
            num_elements = max_elements = neAB;
         }
         else
         {
            printf("[---] YAC_FloatArray::join: failed to allocate %u elements.\n", neAB);
            return 0;
         }
      }
      else
      {
         YAC_FloatArray::free();
      }

   }

   return 1;
}

sF32 YAC_FloatArray::getAbsMaxDiff(YAC_Object *_fa) {
   if(YAC_Is_FloatArray(_fa))
   {
      YAC_CAST_ARG(YAC_FloatArray, o, _fa);

      if(o->num_elements >= num_elements)
      {
         sF32 maxDiff = 0.0f;

         for(sUI i = 0; i < num_elements; i++)
         {
            sF32 diff = o->elements[i] - elements[i];
            if(diff < 0.0f)
               diff = -diff;
            if(diff > maxDiff)
               maxDiff = diff;
         }
         return maxDiff;
      }
   }
   return 0.0f;
}

sSI YAC_FloatArray::findIdxBefore(sF32 _val) {
   // assumes monotonically increasing array values (1,2,3,..)
   for(sUI i = 0; i < num_elements; i++)
   {
      if(elements[i] >= _val)
         return sSI(i) - 1;
   }
   return -1;
}

void YAC_FloatArray::biquad(YAC_Object *_faIn, sSI _type, sF32 _dbGain, sF32 _freq, sF32 _res) {
   if(YAC_Is_FloatArray(_faIn))
   {
      YAC_CAST_ARG(YAC_FloatArray, o, _faIn);

      sUI ne = sMIN(num_elements, o->num_elements);

      // yac_host->printf("xxx biquad: ne=%u num_elements=%u o->num_elements=%u\n", ne, num_elements, o->num_elements);

      if(ne > 0u)
      {
         YAC_Biquad2 flt;
         flt.reset();
         flt.calcParams(_type, _dbGain, _freq, _res);

         for(sUI i = 0; i < ne; i++)
         {
            elements[i] = flt.filter(o->elements[i]);
         }
      }
   }   
}

void YAC_FloatArray::mergeArraySkipVal(YAC_Object *_fa, float _skipVal) {
   if(YAC_Is_FloatArray(_fa))
   {
      YAC_CAST_ARG(YAC_FloatArray, o, _fa);
      sUI ne = sMIN(num_elements, o->num_elements);
      for(sUI i = 0u; i < ne; i++)
      {
         sF32 f = o->elements[i];
         if(f != _skipVal)
            elements[i] = f;
         // else: keep value
      }
   }
}

sBool YAC_FloatArray::saveToStreamSigned16(YAC_Object *_stream, sUI _numElements) {
   const sUI ne = (0u == _numElements) ? num_elements : (_numElements <= num_elements) ? _numElements : num_elements;
   for(sUI i = 0u; i < ne; i++)
   {
      sF32 f = elements[i];
      yacmem s;
      if(f >= 0.0f)
      {
         if(f > 1.0f)
            f = 1.0f;
         s.s16 = sS16(f * 32767.0f);
      }
      else
      {
         if(f < -1.0f)
            f = -1.0f;
         s.s16 = sS16(f * 32768.0f);
      }
      _stream->yacStreamWriteI16(s.u16);
   }
   return YAC_TRUE;
}

class FloatArrayBitStream4 {
   YAC_Object *stream;
   sU8 bit_offset;
   sU8 bit_buffer;

public:
   FloatArrayBitStream4(YAC_Object *_stream) {
      stream = _stream;
      bit_offset = 0u;
      bit_buffer = 0u;
   }

   ~FloatArrayBitStream4() {
   }

   void write4(sU8 _v) {
      if(0u == bit_offset)
      {
         bit_buffer = _v & 15u;
         bit_offset = 4u;
      }
      else
      {
         bit_buffer |= ((_v & 15u) << 4);
         stream->yacStreamWriteI8(bit_buffer);
         bit_offset = 0u;
      }
   }

   void write8(sU8 _v) {
      write4(_v & 15u);
      write4((_v >> 4) & 15u);
   }

   void write12(sU16 _v) {
      write4(_v & 15u);
      write4((_v >> 4) & 15u);
      write4((_v >> 8) & 15u);
   }

   void flush(void) {
      if(bit_offset > 0u)
      {
         stream->yacStreamWriteI8(bit_buffer);
         bit_offset = 0u;
      }
   }
  
};

sBool YAC_FloatArray::saveToStreamSigned12FrameEnc(YAC_Object *_stream, sUI _numElements) {
   const sUI ne = (0u == _numElements) ? num_elements : (_numElements <= num_elements) ? _numElements : num_elements;
   if(ne > 0u)
   {
      FloatArrayBitStream4 bs(_stream);
#define S16_FRAME_SZ 16
// #define S16_FRAME_SZ 8
      // Convert to signed 12 bit
      sS16 *smp16 = new(std::nothrow)sS16[ne];
      yacmem s;
      for(sUI i = 0u; i < ne; i++)
      {
         sF32 f = elements[i];
         if(f >= 0.0f)
         {
            if(f > 1.0f)
               f = 1.0f;
            s.s16 = sS16(f * 2047.0f);
         }
         else
         {
            if(f < -1.0f)
               f = -1.0f;
            s.s16 = sS16(f * 2048.0f);
         }
         smp16[i] = s.s16;
      }

      sUI numFrames = ne / S16_FRAME_SZ;
      sUI srcOff = 0u;
      sS16 last = 0;
      sS16 cur;
      for(sUI frameIdx = 0u; frameIdx < numFrames; frameIdx++)
      {
         if( (srcOff + S16_FRAME_SZ) > ne )
         {
            // Partial last frame, save raw delta values
            bs.write4(0u/*TYPE=12bit raw values*/);
            while(srcOff < ne)
            {
               s.s16 = smp16[srcOff++];
               bs.write12(s.u16);
            }
         }
         else
         {
            // Find min/max delta
            sS32 maxDelta = 0;
            sS32 minDelta = 0;
            sS16 lastTmp = last;
            sS16 curTmp;
            for(sUI i = 0u; i < S16_FRAME_SZ; i++)
            {
               curTmp = smp16[srcOff + i];
               sS32 delta = (curTmp - lastTmp);
               // if(0u == (frameIdx&7))
               //    yac_host->printf("xxx frameIdx=%u srcOff=%u cur=%d last=%d delta=%d\n", frameIdx, srcOff+i, curTmp, lastTmp, delta);
               if(delta > maxDelta)
                  maxDelta = delta;
               else if(delta < minDelta)
                  minDelta = delta;
               lastTmp = curTmp;
            }
            // yac_host->printf("xxx frameIdx=%u minDelta=%d maxDelta=%d\n", frameIdx, minDelta, maxDelta);

            if( (minDelta < -2048) || (maxDelta > 2047) )
            {
               bs.write4(0u/*TYPE=12bit raw values*/);
               for(sUI i = 0u; i < S16_FRAME_SZ; i++)
               {
                  s.s16 = last = smp16[srcOff++];
                  bs.write12(s.u16);
               }               
            }
            else if( (minDelta < -128) || (maxDelta > 127) )
            {
               bs.write4(1u/*TYPE=12bit delta values*/);
               for(sUI i = 0u; i < S16_FRAME_SZ; i++)
               {
                  cur = smp16[srcOff++];
                  s.s16 = (cur - last);
                  last = cur;
                  bs.write12(s.u16);
               }
            }
            else if( (minDelta < -8) || (maxDelta > 7) )
            {
               bs.write4(2u/*TYPE=8bit delta values*/);
               for(sUI i = 0u; i < S16_FRAME_SZ; i++)
               {
                  cur = smp16[srcOff++];
                  s.s8 = sS8(cur - last);
                  last = cur;
                  bs.write8(s.u8);
               }
            }
            else
            {
               bs.write4(3u/*TYPE=4bit delta values*/);
               for(sUI i = 0u; i < S16_FRAME_SZ; i++)
               {
                  cur = smp16[srcOff++];
                  s.s16 = (cur - last);
                  bs.write4(s.u16 & 15u);
                  last = cur;
               }
            }
         }
      } // loop frames

      bs.flush();

      delete [] smp16;
   }
   return YAC_TRUE;
}
