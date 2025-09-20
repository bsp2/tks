/// TKS_DummyStream.h
///
/// (c) 2015 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef __TKS_DUMMYSTREAM_H__
#define __TKS_DUMMYSTREAM_H__


class TKS_DummyStream : public YAC_StreamBase {
public:
   sUI   size;
   sUI   io_offset;
   
public:
   TKS_DummyStream(void);
   ~TKS_DummyStream();
   
   sSI   YAC_VCALL yacTensorRank       (void) {return YAC_TENSOR_RANK_VECTOR;}

   sUI   YAC_VCALL yacArrayGetWidth(void)            { return sMAX(io_offset, size); } 
   sUI   YAC_VCALL yacArrayGetNumElements(void)      { return io_offset; } 
   sUI   YAC_VCALL yacArrayGetMaxElements(void)      { return sMAX(io_offset, size); } 
   sUI   YAC_VCALL yacArrayGetHeight(void)           { return 1; }
   sUI   YAC_VCALL yacArrayGetElementType(void)      { return 1; }
   sUI   YAC_VCALL yacArrayGetElementByteSize(void)  { return sizeof(sU8); }
   sUI   YAC_VCALL yacArrayGetStride(void)           { return sMAX(io_offset, size); }
   void *YAC_VCALL yacArrayGetPointer(void)          { return (void*)NULL; }
   
   sUI   YAC_VCALL yacStreamGetSize(void);
   sUI   YAC_VCALL yacStreamGetOffset(void);
   void  YAC_VCALL yacStreamSetOffset(sUI);
   sU8   YAC_VCALL yacStreamReadI8(void);
   sU16  YAC_VCALL yacStreamReadI16(void);
   sU32  YAC_VCALL yacStreamReadI32(void);
   sF32  YAC_VCALL yacStreamReadF32(void);
   void  YAC_VCALL yacStreamWriteI8(sU8 _value);
   void  YAC_VCALL yacStreamWriteI16(sU16 _value);
   void  YAC_VCALL yacStreamWriteI32(sS32 _value);
   void  YAC_VCALL yacStreamWriteF32(sF32 _value);
   sBool YAC_VCALL yacIsStream(void);

   void  YAC_VCALL yacSerialize(YAC_Object *, sUI _typeinfo); 
   sUI   YAC_VCALL yacDeserialize(YAC_Object *, sUI _typeinfo); 
   
   void  YAC_VCALL yacOperator       (sSI, YAC_Object*, YAC_Value*);

   virtual sSI  setSize    (sSI _numbytes);
  
};


#endif // __YAC_BUFFER_H__
