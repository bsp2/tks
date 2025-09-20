/// TKS_DummyStream.cpp
///
/// (c) 2015 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include <string.h>
#include "tks.h"
#include "PTN_Node.h"
#include "TKS_DummyStream.h"


TKS_DummyStream::TKS_DummyStream(void) { 
   class_ID  = TKS_CLID_DUMMYSTREAM; 
   size      = 0; 
   io_offset = 0; 
} 
 
TKS_DummyStream::~TKS_DummyStream(void) { 
} 
 
void YAC_VCALL TKS_DummyStream::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o)
   {
      if(_o!=((YAC_Object*)this))
      {
         if(_cmd==YAC_OP_SHL)
         {
            _o->yacSerialize(this, 1);
            return;
         }
         else if(YAC_OP_ASSIGN == _cmd)
         {
            yacOperatorAssign(_o);
            return;
         }
      }
   }
   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_VCALL TKS_DummyStream::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   // will this ever be called ? serializing a DummyStream does not exactly make sense..
   YAC_BEG_SERIALIZE();
   if(_rtti)
   {
      YAC_SERIALIZE_I32(size);
   }
   if(size)
   {
      sUI i;
      for(i=0; i<size; i++)
      {
         YAC_SERIALIZE_I8(0);
      }
   }
}

sUI YAC_VCALL TKS_DummyStream::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   // will this ever be called ? deserializing a DummyStream does not exactly make sense..
   if(!_rtti)
      return 0;
   YAC_BEG_DESERIALIZE();
   sUI nsize=YAC_DESERIALIZE_I32();
   if(nsize)
   {
      // ---- allocate new buffer ----
      if(setSize(nsize))
      {
         sUI i;
         for(i=0; i<nsize; i++)
         {
            // ---- deserialize a byte ;) ----
            (void)YAC_DESERIALIZE_I8();
         }
      }
      else
      {
         // ---- out of mem, skip data -----
         _ifs->yacStreamSeek(nsize, YAC_CUR);
      }
   }
   return 1;
}
 
sSI TKS_DummyStream::setSize(sSI _num) {

   io_offset = 0;
   size      = (sUI)_num;

   return YAC_TRUE;
}

sBool YAC_VCALL TKS_DummyStream::yacIsStream(void) {
   return YAC_TRUE;
}

sUI YAC_VCALL TKS_DummyStream::yacStreamGetSize(void) {
   return sMAX(io_offset, size);
}

void YAC_VCALL TKS_DummyStream::yacStreamWriteI8(sU8 _value) {
   (void)_value;
   io_offset++;

   if(io_offset > size)
      size = io_offset;
}

void YAC_VCALL TKS_DummyStream::yacStreamWriteI16(sU16 _value) {
   (void)_value;
   io_offset += 2;

   if(io_offset > size)
      size = io_offset;
}

void YAC_VCALL TKS_DummyStream::yacStreamWriteI32(sSI _value) {
   (void)_value;
   io_offset += 4;

   if(io_offset > size)
      size = io_offset;
}

void YAC_VCALL TKS_DummyStream::yacStreamWriteF32(sF32 _value) {
   (void)_value;
   io_offset += 4;

   if(io_offset > size)
      size = io_offset;
}

sU8 YAC_VCALL TKS_DummyStream::yacStreamReadI8(void) {
   io_offset++;

   if(io_offset > size)
      size = io_offset;

   return 0;
}

sU16 YAC_VCALL TKS_DummyStream::yacStreamReadI16(void) {
   io_offset += 2;

   if(io_offset > size)
      size = io_offset;

   return 0;
}

sU32 YAC_VCALL TKS_DummyStream::yacStreamReadI32(void) {
   io_offset += 4;

   if(io_offset > size)
      size = io_offset;

   return 0;
}

sF32 YAC_VCALL TKS_DummyStream::yacStreamReadF32(void) {
   io_offset += 4;

   if(io_offset > size)
      size = io_offset;

   return 0.0f;
}

void YAC_VCALL TKS_DummyStream::yacStreamSetOffset(sUI _off) {
   io_offset = (sUI)_off;
}

sUI YAC_VCALL TKS_DummyStream::yacStreamGetOffset(void) {
   return io_offset;
}
