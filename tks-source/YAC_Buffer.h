/// YAC_Buffer.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef YAC_BUFFER_H__
#define YAC_BUFFER_H__


class YAC_Buffer : public YAC_StreamBase {
public:
   sUI   size;
   sUI   io_offset;
   sU8  *buffer;
   sBool deleteme;

public:
   YAC_Buffer(void);
   ~YAC_Buffer();

   sSI   YAC_VCALL yacTensorRank       (void) {return YAC_TENSOR_RANK_VECTOR;}

   void  YAC_VCALL yacArraySet(void *_context, sUI _index, YAC_Value *_value);
   void  YAC_VCALL yacArrayGet(void *_context, sUI _index, YAC_Value *_value);
   sUI   YAC_VCALL yacArrayGetWidth(void)            { return size; }
   sUI   YAC_VCALL yacArrayGetNumElements(void)      { return io_offset; }
   sUI   YAC_VCALL yacArrayGetMaxElements(void)      { return size; }
   sUI   YAC_VCALL yacArrayGetHeight(void)           { return 1; }
   sUI   YAC_VCALL yacArrayGetElementType(void)      { return 1; }
   sUI   YAC_VCALL yacArrayGetElementByteSize(void)  { return sizeof(sU8); }
   sUI   YAC_VCALL yacArrayGetStride(void)           { return size; }
   void *YAC_VCALL yacArrayGetPointer(void)          { return (void*)buffer; }

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
   void  YAC_VCALL yacGetConstantStringList(YAC_String *_s);

   sU8  getI8OF(sSI);
   sS8  getS8OF(sSI);
   sU16 getI16OF(sSI);
   sS16 getS16OF(sSI);
   sU32 getI32OF(sSI);
   sS32 getS32OF(sSI);
   sF32 getF32OF(sSI);

   void  YAC_VCALL yacOperator       (sSI, YAC_Object*, YAC_Value*);
   void  YAC_VCALL yacOperatorAssign (YAC_Object *_robj);
   sBool YAC_VCALL yacEquals         (YAC_Object *);

   virtual sBool setSize    (sUI _numbytes);
   virtual sBool resize     (sUI _numbytes);
   virtual void  free       (void);

   void  pokeI8       (sSI _offset, sSI _value);
   void  pokeI14      (sSI _offset, sSI _value);
   void  pokeI16      (sSI _offset, sSI _value);
   virtual void  pokeI32      (sSI _offset, sSI _value);
   void  pokeF32      (sSI _offset, sF32 _value);
   sSI   peekI8       (sSI _offset);
   sSI   peekS8       (sSI _offset);
   sSI   peekI14      (sSI _offset);
   sSI   peekI16      (sSI _offset);
   sSI   peekS16      (sSI _offset);
   virtual sSI   peekI32      (sSI _offset);
   sSI   peekS32      (sSI _offset);
   sF32  peekF32      (sSI _offset);
   void  fillZero     (void);
   void  fillI8       (sUI _c);
   sBool substring    (YAC_String *_d, sUI _off, sUI _num);
   void  getString    (sUI _off, sUI _len, YAC_Value *_r);
   sBool setString    (sUI _off, YAC_String *_s);
   sSI   gzip         (YAC_Object *, sSI, sSI, sSI _level);
   sSI   gunzip       (YAC_Object *, sSI, sSI, sSI _est_size);
   sSI   _getChecksum (void);
   sBool visit        (YAC_Buffer *_buffer, sUI _off, sUI _len);
   sUI   base64Encode (YAC_String *_out);
   sUI   base64Decode (YAC_String *_in);
   void  truncate     (sUI _newSize);
   sUI   readBufferBits  (sUI _startBit, sUI _numBits);
   void  writeBufferBits (sUI _startBit, sUI _numBits, sUI _val);
   void  hexdump      (sUI _start, sUI _num);

   sBool YAC_VCALL yacArrayAlloc  (sUI _sx, sUI, sUI, sUI);
   sBool YAC_VCALL yacArrayRealloc(sUI _sx, sUI _sy, sUI _type, sUI _elementbytesize);

   sBool _compareBuffer (YAC_Buffer *_o) const;

   virtual sUI ringWriteBuffer (sUI _ringStartOff, sUI _ringSz, sUI _writeOff, YAC_Buffer *_src, sUI _srcOff, sUI _numBytes);
   virtual sUI ringReadBuffer (sUI _ringStartOff, sUI _ringSz, sUI _readOff, YAC_Buffer *_dst, sUI _dstOff, sUI _numBytes);
};


#endif // YAC_BUFFER_H__
