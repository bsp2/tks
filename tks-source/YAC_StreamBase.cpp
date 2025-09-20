/// YAC_StreamBase.cpp
///
/// (c) 2002-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include <stdlib.h>

#include "tks.h"

#include "TKS_StreamIO.h"
#include "YAC_Buffer.h"
#include "YAC_Double.h"


YAC_StreamBase::YAC_StreamBase(void) {
#ifdef YAC_LITTLE_ENDIAN
   byteOrder = YAC_LITTLEENDIAN;
#else
   byteOrder = YAC_BIGENDIAN;
#endif
}

YAC_StreamBase::~YAC_StreamBase(void) {
}

sSI YAC_VCALL YAC_StreamBase::yacStreamWrite(sU8 *_src, sUI _num/*num*/) {
   for(sUI i = 0u; i < _num; i++)
   {
      yacStreamWriteI8(*_src++);
   }
   return _num;
}

sSI YAC_VCALL YAC_StreamBase::yacStreamRead(sU8 *_d, sUI _num/*num*/) {
   for(sUI i = 0u; i < _num; i++)
   {
      *_d++ = yacStreamReadI8();
   }
   return _num;
}

sU8 YAC_VCALL YAC_StreamBase::yacStreamReadI8(void) { 
   tkscript->printf("[---] YAC_StreamBase::yacStreamReadI8 default impl.\n"); 
   return 0; 
} 

sU16 YAC_VCALL YAC_StreamBase::yacStreamReadI16(void) {
   // ---- read next word from stream ----
   union {
      sU8  r_b[2];
      sU16 r_w;
   } ur;

#ifdef YAC_LITTLE_ENDIAN
   if(byteOrder)
   {
      // ---- big->little ----
      yacStreamRead(&ur.r_b[1], 1);
      yacStreamRead(&ur.r_b[0], 1);
   }
   else
   {
      // ---- little->little ----
      yacStreamRead(ur.r_b, 2);
   }
#else
   if(byteOrder)
   {
      // ---- big->big ----
      yacStreamRead(ur.r_b, 2);
   }
   else
   {
      // ---- little->big ----
      yacStreamRead(&ur.r_b[1], 1);
      yacStreamRead(&ur.r_b[0], 1);
   }
#endif

   // // return *(sU16*)r;
   return ur.r_w;
}

sU32 YAC_VCALL YAC_StreamBase::yacStreamReadI32(void) {
   // ---- read next doubleword from stream ----
   union {
      sU8  r_b[4];
      sU32 r_d;
   } ur;

#ifdef YAC_LITTLE_ENDIAN
   if(byteOrder)
   {
      // ---- big->little ----
      yacStreamRead(&ur.r_b[3], 1);
      yacStreamRead(&ur.r_b[2], 1);
      yacStreamRead(&ur.r_b[1], 1);
      yacStreamRead(&ur.r_b[0], 1);
   }
   else
   {
      // ---- little->little ----
      yacStreamRead(ur.r_b, 4);
   }
#else
   if(byteOrder)
   {
      // ---- big->big ----
      yacStreamRead(ur.r_b, 4);
   }
   else
   {
      // ---- little->big ----
      yacStreamRead(&ur.r_b[3], 1);
      yacStreamRead(&ur.r_b[2], 1);
      yacStreamRead(&ur.r_b[1], 1);
      yacStreamRead(&ur.r_b[0], 1);
   }
#endif

   // // return ((sU32)(*(sU32*)r));
   return ur.r_d;
}

sU64 YAC_VCALL YAC_StreamBase::yacStreamReadI64(void) { 
   // ---- read next doubleword from stream ---- 
   union {
      sU8  r_b[8]; 
      sU64 r_q;
   } ur;

#ifdef YAC_LITTLE_ENDIAN 
   if(byteOrder) 
   { 
      // ---- big->little ---- 
      yacStreamRead(&ur.r_b[7], 1); 
      yacStreamRead(&ur.r_b[6], 1); 
      yacStreamRead(&ur.r_b[5], 1); 
      yacStreamRead(&ur.r_b[4], 1); 
      yacStreamRead(&ur.r_b[3], 1); 
      yacStreamRead(&ur.r_b[2], 1); 
      yacStreamRead(&ur.r_b[1], 1); 
      yacStreamRead(&ur.r_b[0], 1); 
   } 
   else 
   { 
      // ---- little->little ---- 
      yacStreamRead(ur.r_b, 8); 
   } 
#else 
   if(byteOrder) 
   { 
      // ---- big->big ---- 
      yacStreamRead(ur.r_b, 8); 
   } 
   else 
   { 
      // ---- little->big ---- 
      yacStreamRead(&ur.r_b[7], 1); 
      yacStreamRead(&ur.r_b[6], 1); 
      yacStreamRead(&ur.r_b[5], 1); 
      yacStreamRead(&ur.r_b[4], 1); 
      yacStreamRead(&ur.r_b[3], 1); 
      yacStreamRead(&ur.r_b[2], 1); 
      yacStreamRead(&ur.r_b[1], 1); 
      yacStreamRead(&ur.r_b[0], 1); 
   } 
#endif 

   // // return ((sU64)(*(sU64*)r)); 
   return ur.r_q;
} 

sF32 YAC_VCALL YAC_StreamBase::yacStreamReadF32(void) {
   // ---- read next IEEE 32bit float from stream ----
   yac_ieee_float_t raw;
   yac_ieee_float_t r;
   yacStreamRead(raw.u8, 4);

#ifdef YAC_LITTLE_ENDIAN 
   if(byteOrder)
   {
      // big -> little
      // tkscript->printf("xxxx yacStreamReadF32: big -> little\n");
#if 0
      yac_ieee_float_t rev;
      r.le.sign     = rev.be.sign;
      r.le.exponent = rev.be.exponent;
      r.le.mantissa = rev.be.mantissa;
      r.u32 = rev.u32;
#else
      r.u8[0] = raw.u8[3];
      r.u8[1] = raw.u8[2];
      r.u8[2] = raw.u8[1];
      r.u8[3] = raw.u8[0];
#endif
   }
   else
   {
      r.u32 = raw.u32;
   }
#else
   if(!byteOrder)
   {
      // little -> big
#if 0
      yac_ieee_float_t rev;
      rev.u8[0] = raw.u8[3];
      rev.u8[1] = raw.u8[2];
      rev.u8[2] = raw.u8[1];
      rev.u8[3] = raw.u8[0];
      r.u32 = rev.u32;
#else
      r.u8[0] = raw.u8[3];
      r.u8[1] = raw.u8[2];
      r.u8[2] = raw.u8[1];
      r.u8[3] = raw.u8[0];
#endif
      // // r.be.sign     = rev.le.sign;
      // // r.be.exponent = rev.le.exponent;
      // // r.be.mantissa = rev.le.mantissa;
   }
   else
   {
      r.u32 = raw.u32;
   }
#endif // YAC_LITTLE_ENDIAN

   return r.f32;
}

sF64 YAC_VCALL YAC_StreamBase::yacStreamReadF64(void) { 
   // ---- read next IEEE 64bit double from stream ---- 
   yac_ieee_double_t raw;
   yac_ieee_double_t r;
   yacStreamRead(raw.u8, 8);

#ifdef YAC_LITTLE_ENDIAN 
   if(byteOrder)
   {
      // big -> little
#if 0
      r.le.sign      = raw.be.sign;
      r.le.exponent  = raw.be.exponent;
      r.le.mantissa1 = raw.be.mantissa1;
      r.le.mantissa2 = raw.be.mantissa2;
#else
      r.u8[0] = raw.u8[7];
      r.u8[1] = raw.u8[6];
      r.u8[2] = raw.u8[5];
      r.u8[3] = raw.u8[4];
      r.u8[4] = raw.u8[3];
      r.u8[5] = raw.u8[2];
      r.u8[6] = raw.u8[1];
      r.u8[7] = raw.u8[0];
#endif
   }
   else
   {
      r.u64 = raw.u64;
   }
#else
   if(!byteOrder)
   {
      // little -> big
#if 0
      r.be.sign      = raw.le.sign;
      r.be.exponent  = raw.le.exponent;
      r.be.mantissa1 = raw.le.mantissa1;
      r.be.mantissa2 = raw.le.mantissa2;
#else
      r.u8[0] = raw.u8[7];
      r.u8[1] = raw.u8[6];
      r.u8[2] = raw.u8[5];
      r.u8[3] = raw.u8[4];
      r.u8[4] = raw.u8[3];
      r.u8[5] = raw.u8[2];
      r.u8[6] = raw.u8[1];
      r.u8[7] = raw.u8[0];
#endif
   }
   else
   {
      r.u64 = raw.u64;
   }
#endif // YAC_LITTLE_ENDIAN

   return r.f64;
} 

void YAC_VCALL YAC_StreamBase::yacStreamWriteI8(sU8 _i) {
   tkscript->printf("[---] YAC_StreamBase::yacStreamWriteI8(%i 0x%02x) default impl.\n", _i, _i); 
}

void YAC_StreamBase::setI8(sSI _i) {
   yacStreamWriteI8((sU8)_i);
}

void YAC_StreamBase::setI16(sSI _i) {
   yacStreamWriteI16((sU16)_i);
}

void YAC_StreamBase::setI32(sSI _i) {
   yacStreamWriteI32(_i);
}

void YAC_StreamBase::setF32(sF32 _f) {
   yacStreamWriteF32(_f);
}

void YAC_StreamBase::setF64(sF32 _f) { 
   yacStreamWriteF64((sF64)_f); 
} 

void YAC_VCALL YAC_StreamBase::yacStreamWriteI16(sU16 _value) {
   // ---- write next word to stream ----
   union {
      sU8  r_b[2];
      sU16 r_w;
   } ur;

   // // *(sU16*)r=(sU16)_value;
   ur.r_w = (sU16)_value;

#ifdef YAC_LITTLE_ENDIAN
   if(byteOrder)
   {
      // ---- little->big ----
      yacStreamWrite(&ur.r_b[1], 1); // store high byte
      yacStreamWrite(&ur.r_b[0], 1); // store low byte
   }
   else
   {
      // ---- little->little ----
      yacStreamWrite(ur.r_b, 2);
   }
#else
   if(byteOrder)
   {
      // ---- big->big ----
      yacStreamWrite(ur.r_b, 2);
   }
   else
   {
      // ---- big->little ----
      yacStreamWrite(&ur.r_b[1], 1); // store low byte
      yacStreamWrite(&ur.r_b[0], 1); // store high byte
   }
#endif
}

void YAC_VCALL YAC_StreamBase::yacStreamWriteI32(sS32 _value) {
   // ---- write next doubleword to stream ----
   union {
      sU8  r_b[4];
      sU32 r_d;
   } ur;

   // // *(sU32*)r=(sU32)_value;
   ur.r_d = (sU32)_value;

#ifdef YAC_LITTLE_ENDIAN
   if(byteOrder)
   {
      // little->big
      yacStreamWrite(&ur.r_b[3], 1); // store high byte of high word
      yacStreamWrite(&ur.r_b[2], 1); // store low byte of high word
      yacStreamWrite(&ur.r_b[1], 1); // store high byte of low word
      yacStreamWrite(&ur.r_b[0], 1); // store low byte of low word
   }
   else
   {
      // ---- little->little ----
      yacStreamWrite(ur.r_b, 4);
   }
#else
   if(byteOrder)
   {
      // ---- big->big ----
      yacStreamWrite(ur.r_b, 4);
   }
   else
   {
      // ---- big->little ----
      yacStreamWrite(&ur.r_b[3], 1); // store low byte of low word
      yacStreamWrite(&ur.r_b[2], 1); // store high byte of low word
      yacStreamWrite(&ur.r_b[1], 1); // store low byte of high word
      yacStreamWrite(&ur.r_b[0], 1); // store high byte of high word
   }
#endif
}

void YAC_VCALL YAC_StreamBase::yacStreamWriteI64(sS64 _value) { 
   // ---- write next doubleword to stream ---- 
   union {
      sU8  r_b[8];
      sU64 r_q;
   } ur;

   // // *(sU64*)r=(sU64)_value; 
   ur.r_q = (sU64)_value;

#ifdef YAC_LITTLE_ENDIAN 
   if(byteOrder) 
   { 
      // little->big 
      yacStreamWrite(&ur.r_b[7], 1);  
      yacStreamWrite(&ur.r_b[6], 1); 
      yacStreamWrite(&ur.r_b[5], 1); 
      yacStreamWrite(&ur.r_b[4], 1); 
      yacStreamWrite(&ur.r_b[3], 1); 
      yacStreamWrite(&ur.r_b[2], 1); 
      yacStreamWrite(&ur.r_b[1], 1); 
      yacStreamWrite(&ur.r_b[0], 1); 
   } 
   else 
   { 
      // ---- little->little ---- 
      yacStreamWrite(ur.r_b, 8); 
   } 
#else 
   if(byteOrder) 
   { 
      // ---- big->big ---- 
      yacStreamWrite(ur.r_b, 8); 
   } 
   else 
   { 
      // ---- big->little ---- 
      yacStreamWrite(&ur.r_b[7], 1); 
      yacStreamWrite(&ur.r_b[6], 1); 
      yacStreamWrite(&ur.r_b[5], 1); 
      yacStreamWrite(&ur.r_b[4], 1); 
      yacStreamWrite(&ur.r_b[3], 1); 
      yacStreamWrite(&ur.r_b[2], 1); 
      yacStreamWrite(&ur.r_b[1], 1); 
      yacStreamWrite(&ur.r_b[0], 1); 
   } 
#endif 
} 

void YAC_VCALL YAC_StreamBase::yacStreamWriteF32(sF32 _value) {
   // ---- store next IEEE 32bit float to stream ----
#ifdef YAC_LITTLE_ENDIAN 
   if(byteOrder)
   {
      yac_ieee_float_t f; f.f32 = _value;
      yac_ieee_float_t fs;
#if 0
      fs.be.sign     = f.le.sign;
      fs.be.exponent = f.le.exponent;
      fs.be.mantissa = f.le.mantissa;
#else
      fs.u8[0] = f.u8[3];
      fs.u8[1] = f.u8[2];
      fs.u8[2] = f.u8[1];
      fs.u8[3] = f.u8[0];
#endif
      yacStreamWrite(fs.u8, 4);
   }
   else
   {
      yacStreamWrite((sU8*)&_value, 4);
   }
#else
   if(!byteOrder)
   {
      yac_ieee_float_t f; f.f32 = _value;
      yac_ieee_float_t fs;
#if 0
      fs.le.sign     = f.be.sign;
      fs.le.exponent = f.be.exponent;
      fs.le.mantissa = f.be.mantissa;
#else
      fs.u8[0] = f.u8[3];
      fs.u8[1] = f.u8[2];
      fs.u8[2] = f.u8[1];
      fs.u8[3] = f.u8[0];
#endif
      yacStreamWrite(fs.u8, 4);
   }
   else
   {
      yacStreamWrite((sU8*)&_value, 4);
   }
#endif // YAC_LITTLE_ENDIAN

}

void YAC_VCALL YAC_StreamBase::yacStreamWriteF64(sF64 _value) { 
   // ---- store next IEEE 64bit double to stream ---- 
#ifdef YAC_LITTLE_ENDIAN 
   if(byteOrder)
   {
      yac_ieee_double_t f; f.f64 = _value;
      yac_ieee_double_t fs;
#if 0
      fs.be.sign      = f.le.sign;
      fs.be.exponent  = f.le.exponent;
      fs.be.mantissa1 = f.le.mantissa1;
      fs.be.mantissa2 = f.le.mantissa2;
#else
      fs.u8[0] = f.u8[7];
      fs.u8[1] = f.u8[6];
      fs.u8[2] = f.u8[5];
      fs.u8[3] = f.u8[4];
      fs.u8[4] = f.u8[3];
      fs.u8[5] = f.u8[2];
      fs.u8[6] = f.u8[1];
      fs.u8[7] = f.u8[0];
#endif
      yacStreamWrite(fs.u8, 8);
   }
   else
   {
      yacStreamWrite((sU8*)&_value, 8);
   }
#else
   if(!byteOrder)
   {
      yac_ieee_double_t f; f.f64 = _value;
      yac_ieee_double_t fs;
#if 0
      fs.le.sign      = f.be.sign;
      fs.le.exponent  = f.be.exponent;
      fs.le.mantissa1 = f.be.mantissa1;
      fs.le.mantissa2 = f.be.mantissa2;
#else
      fs.u8[0] = f.u8[7];
      fs.u8[1] = f.u8[6];
      fs.u8[2] = f.u8[5];
      fs.u8[3] = f.u8[4];
      fs.u8[4] = f.u8[3];
      fs.u8[5] = f.u8[2];
      fs.u8[6] = f.u8[1];
      fs.u8[7] = f.u8[0];
#endif
      yacStreamWrite(fs.u8, 8);
   }
   else
   {
      yacStreamWrite((sU8*)&_value, 8);
   }
#endif // YAC_LITTLE_ENDIAN
} 

sSI YAC_VCALL YAC_StreamBase::yacStreamReadLine(YAC_String *s, sUI _num) {
   if(s && s->alloc(_num + 1u)) 
   { 
      s->length = 1u;
      s->chars[0] = 0u;
      s->key = YAC_LOSTKEY;
      sUI i = 0u; 
      sUI k = 0u; 
      while(i < _num) 
      { 
         sU8 c = yacStreamReadI8();  // returns 0 on EOF
         // // yac_host->printf("xxx yacStreamReadLine: c=%u eof=%d\n", c, yacStreamEOF());
         if(('\n' == c) || ('\r' == c) || (4u/*EOT*/ == c) || (0u == c))
         { 
            s->chars[i++] = 0; 
            s->length = i; 
            return k; 
         } 
         else 
         {
            s->chars[i++] = c; 
            k++;
         }
      } 
      s->chars[i++] = 0u;
      s->length = i;
      return k; 
   } 
   return 0u;
} 

sSI YAC_VCALL YAC_StreamBase::yacStreamReadString(YAC_String *s, sUI _num) {
   s->free();
   if(!_num) return 0;
   sChar *nc = new(std::nothrow)sChar[_num+1]; 
   if(NULL != nc)
   {
      sUI i = 0; 
      sUI k = 0;
      do
      {
         nc[i] = yacStreamReadI8();
         i++; 
         k++;
         
      } while(i < _num && nc[i-1]);
      if(0 != nc[i-1])
      {
         nc[i++] = 0;
      }
      if(s->alloc(i))  // (todo) use 'nc' as new 's->chars' ?
      {
         s->length = i;
         for(--i; i<0x7FFFFFF; i--)
         {
            s->chars[i] = nc[i];
         }
      }
      delete [] nc;
      s->key = YAC_LOSTKEY; 
      return k;
   }
   return 0;
}

sSI YAC_VCALL YAC_StreamBase::yacStreamWriteString(YAC_String *s, sUI _off, sUI _num) {
   sUI l = s->getLength();
   if(_off < l)
   {
      if(0 == _num)
         _num = l;
      if((_off + _num) > l)
         _num = (l - _off);
      {
         if(_num)
         {
            // valid string area, write chars
            sU8 *b = (sU8*)s->chars;
            return yacStreamWrite(b+_off, _num);
         }
      }
   }
   return 0;
}

sSI YAC_VCALL YAC_StreamBase::yacStreamReadBuffer(YAC_Buffer *_buf, sUI _off, sUI _num, sBool _resize) {
/*if(_num<=(_buf->size-_off))
return yacStreamRead(_buf->buffer+_off, _num); 
else 
   return 0; // buffer too small*/ 
   if(_buf->size < (_num+_off)) 
   {
      if(_resize)
      {
         YAC_Value r;
         //_buf->tksinter_resize((_off+_num), &r); 
         if(!_buf->yacArrayRealloc((_off + _num), 0, 0, 0))
         {
            // buffer re-allocation failed
            return 0;
         }
      }
      else
      {
         // buffer too small
         return 0;
      }
   } 
   // buffer size is ok now read bytes
   return yacStreamRead(_buf->buffer+_off, _num);
}

sSI YAC_VCALL YAC_StreamBase::yacStreamWriteBuffer(YAC_Buffer *_b, sUI _off, sUI _num) {
   sUI l = _b->size;
   if(_off < l)
   {
      if(0u == _num)
         _num = l;
      if((_off + _num) > l)
         _num = (l - _off);
      // ---- valid buffer area, write bytes ----
      sU8 *cb = _b->buffer;
      // yac_host->printf("xxx yacStreamWriteBuffer: num=%u\n", _num);
      return yacStreamWrite(cb+_off, _num);
   }
   return 0;
}

sSI YAC_StreamBase::readString(YAC_String *_s, sSI _num) {
   // ---- try to read exactly _num chars from stream --- 
   return yacStreamReadString((YAC_String*)_s, _num);
}

sSI YAC_StreamBase::readLine(YAC_String *_s, sSI _num) {
   // ---- try to read max. _num chars from stream --- 
   return yacStreamReadLine((YAC_String*)_s, _num);
}

sSI YAC_StreamBase::writeString(YAC_String *_s, sSI _off, sSI _num) {
   // ---- write substring to stream, also useful to skip ASCIIZ ----
   return yacStreamWriteString(_s, _off, _num);
}

sSI YAC_StreamBase::readBuffer(YAC_Object *_buf, sSI _off, sSI _num, sSI _resize) {
   if(YAC_BCHK(_buf, YAC_CLID_BUFFER))
      return yacStreamReadBuffer((YAC_Buffer*)_buf, (sUI)_off, (sUI)_num, (sBool)_resize);
   else
      return 0;
}

sSI YAC_StreamBase::writeBuffer(YAC_Object *_s, sSI _off, sSI _num) {
   if(YAC_BCHK(_s, YAC_CLID_BUFFER))
      return yacStreamWriteBuffer((YAC_Buffer*)_s, (sUI)_off, (sUI)_num);
   else
      return 0;
}

sSI YAC_StreamBase::getI8(void) {
   // (note) 255 is returned as -1 (0xffffffff)
   return (sSI) ((sS8)yacStreamReadI8());
}

sSI YAC_StreamBase::getU8(void) {
   // (note) 255 is returned as 255 (0x000000ff)
   return (sSI) yacStreamReadI8();
}

sSI YAC_StreamBase::getI16(void) {
   // (note) 65535 is returned as -1 (0xffffffff)
   return (sSI) ((sS16)yacStreamReadI16());
}

sSI YAC_StreamBase::getU16(void) {
   // (note) 65535 is returned as 65535 (0x0000ffff)
   return (sSI) yacStreamReadI16();
}

sSI YAC_StreamBase::getI32(void) {
   return yacStreamReadI32();
}

sF32 YAC_StreamBase::getF32(void) {
   return yacStreamReadF32();
}

sF32 YAC_StreamBase::getF64(void) { 
   return (sF32)yacStreamReadF64(); 
} 

void YAC_VCALL YAC_StreamBase::yacStreamWriteObject(YAC_Object *_obj) {
   if(_obj)
      _obj->yacSerialize(this, 0 /** no typeinfo **/);
}

void YAC_StreamBase::setObject(YAC_Object *_obj) {
   yacStreamWriteObject(_obj);
}

void YAC_VCALL YAC_StreamBase::yacStreamReadObject(YAC_Object *_obj) {
   if(_obj)
      _obj->yacDeserialize(this, 0 /** no typeinfo **/);
}

void YAC_StreamBase::getObject(YAC_Object *_d) {
   yacStreamReadObject(_d);
}

void YAC_StreamBase::_serialize(YAC_Object *_src, sSI _typeinfo) {
   if(_src)
      _src->yacSerialize(this, _typeinfo);
}

sSI YAC_StreamBase::_deserialize(YAC_Object *_src, sSI _typeinfo) {
   if(_src)
      return _src->yacDeserialize(this, _typeinfo);
   else
      return 0;
}

void YAC_VCALL YAC_StreamBase::yacStreamSetByteOrder(sUI _order) {
   byteOrder = (_order > 0);
}

sUI YAC_VCALL YAC_StreamBase::yacStreamGetByteOrder(void) {
   return byteOrder;
}

void YAC_StreamBase::setByteOrder(sSI _order) {
   yacStreamSetByteOrder(_order);
}

sSI YAC_StreamBase::getByteOrder(void) {
   return yacStreamGetByteOrder();
}

sUI YAC_StreamBase::getOffset(void) {
   return yacStreamGetOffset();
}

sUI YAC_StreamBase::getSize(void) {
   return yacStreamGetSize();
}

sBool YAC_VCALL YAC_StreamBase::yacStreamEOF(void) {
   sUI off = yacStreamGetOffset();
   return (yacStreamGetSize() == off);
}

sSI YAC_StreamBase::eof(void) {
   return yacStreamEOF();
}

sSI YAC_StreamBase::getErrorCode(void) {
   return yacStreamGetErrorCode();
}

void YAC_StreamBase::getErrorStringByCode(sSI _code, YAC_Value *_r) {
   _r->initEmptyString();
   switch(_code)
   {
   case TKS_StreamIO::ERRINVALIDSEEK:
      _r->value.string_val->copy("ERRINVALIDSEEK");
      break;
   case TKS_StreamIO::ERRIO:
      _r->value.string_val->copy("ERRIO");
      break;
   case TKS_StreamIO::ERRCREATEFILE:
      _r->value.string_val->copy("ERRCREATEFILE");
      break;
   case TKS_StreamIO::ERROPENFILE:
      _r->value.string_val->copy("ERROPENFILE");
      break;
   }
}

void YAC_VCALL YAC_StreamBase::yacStreamSeek(sSI _off, sUI _mode) {
   sUI size = yacStreamGetSize();
   if(size)
   {
      sUI t=yacStreamGetOffset();
      switch(_mode)
      {
      case YAC_BEG:
         if( ((sUI)_off) > size )
            t = size;
         else
            t = (sUI)_off;
         break;
      case YAC_CUR:
         if( ((sUI)(t+_off)) > size )
            t=size;
         else if( ((sSI)(t+_off)) < 0 )
            t=0;
         else
            t+=_off;
         break;
      case YAC_END:
         if(_off > ((sSI)size))
            t = 0;
         else if(_off < 0)
            t = size;
         else
            t = size - (sUI)_off;
         break;
      }
      yacStreamSetOffset(t);
   }
}

sBool YAC_VCALL YAC_StreamBase::yacStreamOpenLogic(sChar *_local_pathname) {
   (void)_local_pathname;
   return YAC_FALSE;
}

sBool YAC_VCALL YAC_StreamBase::yacStreamOpenLocal(sChar *_local_pathname, sSI _access) {
   (void)_local_pathname;
   (void)_access;
   return YAC_FALSE;
}

void YAC_VCALL YAC_StreamBase::yacStreamClose(void) {
}

void YAC_StreamBase::setOffset(sUI _off) {
   yacStreamSetOffset(_off);
}

void YAC_StreamBase::_seek(sSI _off, sSI _mode) {
   yacStreamSeek(_off, _mode);
}

void YAC_StreamBase::_close(void) { 
   yacStreamClose(); 
} 

void YacStreamGetErrorStringByCode(sSI _code, YAC_Value *_r) { 
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   if(s) 
   { 
      s->visit((sChar*)strerror(_code)); 
      _r->initString(s,1); 
   } 
} 

void YAC_StreamBase::setI8a(YAC_Object *_a) {
   if(YAC_VALID(_a))
   {
      sUI ne = _a->yacArrayGetNumElements();
      for(sUI i = 0u; i < ne; i++)
      {
         YAC_Value v;
         _a->yacArrayGet(NULL/*context*/, i, &v);
         v.typecast(YAC_TYPE_INT);
         yacStreamWriteI8((sS8)v.value.int_val);
      }
   }
}

void YAC_StreamBase::setI16a(YAC_Object *_a) {
   if(YAC_VALID(_a))
   {
      sUI ne = _a->yacArrayGetNumElements();
      for(sUI i = 0u; i < ne; i++)
      {
         YAC_Value v;
         _a->yacArrayGet(NULL/*context*/, i, &v);
         v.typecast(YAC_TYPE_INT);
         yacStreamWriteI16((sS16)v.value.int_val);
      }
   }
}

void YAC_StreamBase::setI32a(YAC_Object *_a) {
   if(YAC_VALID(_a))
   {
      sUI ne = _a->yacArrayGetNumElements();
      for(sUI i = 0u; i < ne; i++)
      {
         YAC_Value v;
         _a->yacArrayGet(NULL/*context*/, i, &v);
         v.typecast(YAC_TYPE_INT);
         yacStreamWriteI32(v.value.int_val);
      }
   }
}

void YAC_StreamBase::setF32a(YAC_Object *_a) {
   if(YAC_VALID(_a))
   {
      sUI ne = _a->yacArrayGetNumElements();
      for(sUI i = 0u; i < ne; i++)
      {
         YAC_Value v;
         _a->yacArrayGet(NULL/*context*/, i, &v);
         v.typecast(YAC_TYPE_FLOAT);
         yacStreamWriteF32(v.value.float_val);
      }
   }
}

void YAC_StreamBase::setF64a(YAC_Object *_a) {
   if(YAC_VALID(_a))
   {
      sUI ne = _a->yacArrayGetNumElements();
      for(sUI i = 0u; i < ne; i++)
      {
         YAC_Value v;
         _a->yacArrayGet(NULL/*context*/, i, &v);
         sBool bDone = YAC_FALSE;
         if(YAC_TYPE_OBJECT == v.type)
         {
            if(YAC_VALID(v.value.object_val))
            {
               sF64 f64;
               if(v.value.object_val->yacScanF64(&f64))
               {
                  yacStreamWriteF64(f64);
                  bDone = YAC_TRUE;
               }
               v.unsetFast();
            }
         }
         if(!bDone)
         {
            v.typecast(YAC_TYPE_FLOAT);
            yacStreamWriteF64(v.value.float_val);
         }
      }
   }
}

void YAC_StreamBase::getI8a(YAC_Object *_a, sUI _num) {
   if(YAC_VALID(_a))
   {
      if(_a->yacArrayRealloc(_num))
      {
         for(sUI i = 0u; i < _num; i++)
         {
            YAC_Value v;
            v.initInt(getI8());
            _a->yacArraySet(NULL/*context*/, i, &v);
         }
      }
   }
}

void YAC_StreamBase::getU8a(YAC_Object *_a, sUI _num) {
   if(YAC_VALID(_a))
   {
      if(_a->yacArrayRealloc(_num))
      {
         for(sUI i = 0u; i < _num; i++)
         {
            YAC_Value v;
            v.initInt(getU8());
            _a->yacArraySet(NULL/*context*/, i, &v);
         }
      }
   }
}

void YAC_StreamBase::getI16a(YAC_Object *_a, sUI _num) {
   if(YAC_VALID(_a))
   {
      if(_a->yacArrayRealloc(_num))
      {
         for(sUI i = 0u; i < _num; i++)
         {
            YAC_Value v;
            v.initInt(getI16());
            _a->yacArraySet(NULL/*context*/, i, &v);
         }
      }
   }
}

void YAC_StreamBase::getU16a(YAC_Object *_a, sUI _num) {
   if(YAC_VALID(_a))
   {
      if(_a->yacArrayRealloc(_num))
      {
         for(sUI i = 0u; i < _num; i++)
         {
            YAC_Value v;
            v.initInt(getU16());
            _a->yacArraySet(NULL/*context*/, i, &v);
         }
      }
   }
}

void YAC_StreamBase::getI32a(YAC_Object *_a, sUI _num) {
   if(YAC_VALID(_a))
   {
      if(_a->yacArrayRealloc(_num))
      {
         for(sUI i = 0u; i < _num; i++)
         {
            YAC_Value v;
            v.initInt(getI32());
            _a->yacArraySet(NULL/*context*/, i, &v);
         }
      }
   }
}

void YAC_StreamBase::getF32a(YAC_Object *_a, sUI _num) {
   if(YAC_VALID(_a))
   {
      if(_a->yacArrayRealloc(_num))
      {
         for(sUI i = 0u; i < _num; i++)
         {
            YAC_Value v;
            v.initFloat(getF32());
            _a->yacArraySet(NULL/*context*/, i, &v);
         }
      }
   }
}

void YAC_StreamBase::getF64a(YAC_Object *_a, sUI _num) {
   if(YAC_VALID(_a))
   {
      if(_a->yacArrayRealloc(_num))
      {
         for(sUI i = 0u; i < _num; i++)
         {
            sF64 f64 = getF64();
            YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
            d->yacValueOfF64(f64);
            YAC_Value v;
            v.initObject(d, 1);
            _a->yacArraySet(NULL/*context*/, i, &v);
            v.unsetFast();
         }
      }
   }
}

void YAC_StreamBase::add2f(sF32 _val1, sF32 _val2) {
   yacStreamWriteF32(_val1);
   yacStreamWriteF32(_val2);
}

void YAC_StreamBase::add3f(sF32 _val1, sF32 _val2, sF32 _val3) {
   yacStreamWriteF32(_val1);
   yacStreamWriteF32(_val2);
   yacStreamWriteF32(_val3);
}

void YAC_StreamBase::add4f(sF32 _val1, sF32 _val2, sF32 _val3, sF32 _val4) {
   yacStreamWriteF32(_val1);
   yacStreamWriteF32(_val2);
   yacStreamWriteF32(_val3);
   yacStreamWriteF32(_val4);
}

void YAC_StreamBase::setRgba(sUI _argb32) {
   yacStreamWriteI8( (_argb32 >> 16) & 255u );  // r
   yacStreamWriteI8( (_argb32 >>  8) & 255u );  // g
   yacStreamWriteI8( (_argb32      ) & 255u );  // b
   yacStreamWriteI8( (_argb32 >> 24) & 255u );  // a
}

void YAC_StreamBase::setBgra(sUI _argb32) {
   yacStreamWriteI8( (_argb32      ) & 255u );  // b
   yacStreamWriteI8( (_argb32 >>  8) & 255u );  // g
   yacStreamWriteI8( (_argb32 >> 16) & 255u );  // r
   yacStreamWriteI8( (_argb32 >> 24) & 255u );  // a
}
