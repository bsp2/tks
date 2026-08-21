/// YAC_NibbleStream.cpp
///
/// (c) 2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///

#include <stdlib.h>

#include "tks.h"

#include "TKS_StreamIO.h"
#include "YAC_Buffer.h"
#include "YAC_Double.h"
#include "YAC_NibbleStream.h"


YAC_NibbleStream::YAC_NibbleStream(void) {
   stream = NULL;
#ifdef YAC_LITTLE_ENDIAN
   byteOrder = YAC_LITTLEENDIAN;
#else
   byteOrder = YAC_BIGENDIAN;
#endif
   bit_avail  = 0u;
   bit_buffer = 0u;
}

YAC_NibbleStream::~YAC_NibbleStream() {
}

void YAC_NibbleStream::_setStream(YAC_Object *_s) {
   if(YAC_IS_STREAM(_s))
   {
      stream = (YAC_StreamBase*)_s;
   }
   else
   {
      stream = NULL;
   }
}

YAC_Object *YAC_NibbleStream::_getStream(void) {
   return stream;
}

sBool YAC_VCALL YAC_NibbleStream::yacStreamIsReadable(void) {
   return (NULL != stream) ? stream->yacStreamIsReadable() : YAC_FALSE;
}

sBool YAC_VCALL YAC_NibbleStream::yacStreamIsWritable(void) {
   return (NULL != stream) ? stream->yacStreamIsWritable() : YAC_FALSE;
}

sSI YAC_VCALL YAC_NibbleStream::yacStreamGetErrorCode(void) {
   return (NULL != stream) ? stream->yacStreamGetErrorCode() : 0;
}

sU8 YAC_VCALL YAC_NibbleStream::readRawU8(void) {
   return (NULL != stream) ? stream->yacStreamReadI8() : 0u;
}

void YAC_VCALL YAC_NibbleStream::writeRawU8(sU8 _u8) {
   if(NULL != stream) stream->yacStreamWriteI8(_u8);
}

void YAC_VCALL YAC_NibbleStream::yacStreamSeek(sSI _off, sUI _mode) {
   if(NULL != stream) stream->yacStreamSeek(_off, _mode);
   bit_avail = 0u;
   bit_buffer = 0u;
}

sUI YAC_VCALL YAC_NibbleStream::yacStreamGetOffset(void) {
   return (NULL != stream) ? stream->yacStreamGetOffset() : 0u;
}

void  YAC_VCALL YAC_NibbleStream::yacStreamSetOffset(sUI _offset) {
   if(NULL != stream) stream->yacStreamSetOffset(_offset);
   bit_avail  = 0u;
   bit_buffer = 0u;
}

sUI YAC_VCALL YAC_NibbleStream::yacStreamGetSize(void) {
   return (NULL != stream) ? stream->yacStreamGetSize() : 0u;
}

sBool YAC_VCALL YAC_NibbleStream::yacStreamEOF(void) {
   return (NULL != stream) ? stream->yacStreamEOF() : YAC_FALSE;
}

sU8 YAC_VCALL YAC_NibbleStream::yacStreamReadI8(void) {
   sU8 r = getU4();
   r |= getU4() << 4;
   return r;
}

sU16 YAC_VCALL YAC_NibbleStream::yacStreamReadI16(void) {
   sU16 r = yacStreamReadI8();
   r |= yacStreamReadI8() << 8;
   return r;
}

sU32 YAC_VCALL YAC_NibbleStream::yacStreamReadI32(void) {
   sU32 r = yacStreamReadI16();
   r |= yacStreamReadI16() << 16;
   return r;
}

sU64 YAC_VCALL YAC_NibbleStream::yacStreamReadI64(void) {
   sU64 r = yacStreamReadI32();
   r |= sU64(yacStreamReadI32()) << 32;
   return r;
}

sF32 YAC_VCALL YAC_NibbleStream::yacStreamReadF32(void) {
   yacmem m; m.u32 = yacStreamReadI32();
   return m.f32;
}

sF64 YAC_VCALL YAC_NibbleStream::yacStreamReadF64(void) {
   union {
      sU64 u64;
      sF64 f64;
   } u;
   u.u64 = yacStreamReadI64();
   return u.f64;
}

void YAC_VCALL YAC_NibbleStream::yacStreamWriteI8(sU8 _u8) {
   setI4(_u8 & 15u);
   setI4((_u8 >> 4) & 15u);
}

void YAC_VCALL YAC_NibbleStream::yacStreamWriteI16(sU16 _u16) {
   yacStreamWriteI8(_u16 & 255u);
   yacStreamWriteI8((_u16 >> 8) & 255u);
}

void YAC_VCALL YAC_NibbleStream::yacStreamWriteI32(sS32 _s32) {
   yacmem m; m.s32 = _s32;
   yacStreamWriteI16(m.u32 & 65535u);
   yacStreamWriteI16((m.u32 >> 16) & 65535u);
}

void YAC_VCALL YAC_NibbleStream::yacStreamWriteI64(sS64 _s64) {
   union {
      sU64 u64;
      sS64 s64;
   } u;
   u.s64 = _s64;
   yacStreamWriteI32(u.u64 & 4294967295u);
   yacStreamWriteI32((u.u64 >> 32) & 4294967295u);
}

void YAC_VCALL YAC_NibbleStream::yacStreamWriteF32(sF32 _f32) {
   union {
      sU32 u32;
      sS32 s32;
      sF32 f32;
   } u;
   u.f32 = _f32;
   yacStreamWriteI32(u.s32);
}

void YAC_VCALL YAC_NibbleStream::yacStreamWriteF64(sF64 _f64) {
   union {
      sS64 s64;
      sF64 f64;
   } u;
   u.f64 = _f64;
   yacStreamWriteI64(u.s64);
}

void YAC_VCALL YAC_NibbleStream::yacStreamClose(void) {
   if(NULL != stream)
   {
      yacStreamFlush();
      stream->yacStreamClose();
   }
}

sUI YAC_NibbleStream::getU4(void) {
   sUI r;
   if(bit_avail > 0u)
   {
      r = (bit_buffer >> 4) & 15u;
      bit_avail = 0u;
   }
   else
   {
      bit_buffer = readRawU8();
      r = bit_buffer & 15u;
      bit_avail = 4u;
   }
   return r;
}

sSI YAC_NibbleStream::getS4(void) {
   yacmem m; m.u8 = getU4();
   if(m.u8 & 8u)
      m.u32 |= ~7u;
   return m.s32;
}

sSI YAC_NibbleStream::getI4(void) {
   yacmem m; m.u8 = getU4();
   if(m.u8 & 8u)
      m.u32 |= ~7u;
   return m.s32;
}

sUI YAC_NibbleStream::getU12(void) {
   sUI r = getU4();
   r |= getU4() << 4;
   r |= getU4() << 8;
   return r;
}

sSI YAC_NibbleStream::getS12(void) {
   yacmem r; r.u32 = getU4();
   r.u32 |= getU4() << 4;
   r.u32 |= getU4() << 8;
   if(r.u32 & (1u << 11))
      r.u32 |= 0xFFFFF000u;
   return r.s32;
}

sSI YAC_NibbleStream::getI12(void) {
   return getS12();
}

sUI YAC_NibbleStream::getU20(void) {
   sUI r = getU4();
   r |= getU4() << 4;
   r |= getU4() << 8;
   r |= getU4() << 12;
   r |= getU4() << 16;
   return r;
}

sSI YAC_NibbleStream::getS20(void) {
   yacmem r; r.u32 = getU4();
   r.u32 |= getU4() << 4;
   r.u32 |= getU4() << 8;
   r.u32 |= getU4() << 12;
   r.u32 |= getU4() << 16;
   if(r.u32 & (1u << 19))
      r.u32 |= 0xFFF00000u;
   return r.s32;
}

sSI YAC_NibbleStream::getI20(void) {
   return getS20();
}

sUI YAC_NibbleStream::getU28(void) {
   sUI r = getU4();
   r |= getU4() << 4;
   r |= getU4() << 8;
   r |= getU4() << 12;
   r |= getU4() << 16;
   r |= getU4() << 20;
   r |= getU4() << 24;
   return r;
}

sSI YAC_NibbleStream::getS28(void) {
   yacmem r; r.u32 = getU4();
   r.u32 |= getU4() << 4;
   r.u32 |= getU4() << 8;
   r.u32 |= getU4() << 12;
   r.u32 |= getU4() << 16;
   r.u32 |= getU4() << 20;
   r.u32 |= getU4() << 24;
   if(r.u32 & (1u << 27))
      r.u32 |= 0xF0000000u;
   return r.s32;
}

sSI YAC_NibbleStream::getI28(void) {
   return getS28();
}

void YAC_NibbleStream::setI4(sUI _v4) {
   bit_buffer |= ((_v4 & 15u) << bit_avail);
   if(4u == bit_avail)
   {
      writeRawU8(bit_buffer);
      bit_buffer = 0u;
      bit_avail = 0u;
   }
   else
   {
      bit_avail = 4u;
   }
}

void YAC_NibbleStream::setI12(sUI _v12) {
   setI4( (_v12     ) & 15u );
   setI4( (_v12 >> 4) & 15u );
   setI4( (_v12 >> 8) & 15u );
}

void YAC_NibbleStream::setI20(sUI _v20) {
   setI4( (_v20      ) & 15u );
   setI4( (_v20 >>  4) & 15u );
   setI4( (_v20 >>  8) & 15u );
   setI4( (_v20 >> 12) & 15u );
   setI4( (_v20 >> 16) & 15u );
}

void YAC_NibbleStream::setI28(sUI _v28) {
   setI4( (_v28      ) & 15u );
   setI4( (_v28 >>  4) & 15u );
   setI4( (_v28 >>  8) & 15u );
   setI4( (_v28 >> 12) & 15u );
   setI4( (_v28 >> 16) & 15u );
   setI4( (_v28 >> 20) & 15u );
   setI4( (_v28 >> 24) & 15u );
}

void YAC_VCALL YAC_NibbleStream::yacStreamFlush(void) {
   if(NULL != stream)
   {
      if(stream->yacStreamIsWritable())
      {
         if(4u == bit_avail)
         {
            // Flush bit buffer
            stream->yacStreamWriteI8(bit_buffer);
            bit_buffer = 0u;
            bit_avail = 0u;
         }
      }
   }   
}
