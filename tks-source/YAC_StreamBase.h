/// YAC_StreamBase.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef TKS_STREAMBASE_H__
#define TKS_STREAMBASE_H__


/// Inheritance:
////
///       Stream
///            Buffer(Stream)
///            File(Stream)
///            StdInStream
///            StdOutStream
///            StdErrStream
///            PakFile(Stream)
///            Process(Stream)


class YAC_StreamBase : public YAC_Object {
public:
   sUI byteOrder;

   YAC_StreamBase();
   ~YAC_StreamBase();

   sSI   YAC_VCALL yacStreamGetErrorCode (void) { return 0xCCCCCCCC; };
   sUI   YAC_VCALL yacStreamGetByteOrder (void);
   void  YAC_VCALL yacStreamSetByteOrder (sUI);
   void  YAC_VCALL yacStreamSeek         (sSI _off, sUI _mode);
   sUI   YAC_VCALL yacStreamGetOffset    (void) { return 0; };
   void  YAC_VCALL yacStreamSetOffset    (sUI) { };
   sUI   YAC_VCALL yacStreamGetSize      (void) { return 0; }
   sBool YAC_VCALL yacStreamEOF          (void);
   sSI   YAC_VCALL yacStreamRead         (sU8 *, sUI /*num*/);
   sU8   YAC_VCALL yacStreamReadI8       (void);
   sU16  YAC_VCALL yacStreamReadI16      (void);
   sU32  YAC_VCALL yacStreamReadI32      (void);
   sU64  YAC_VCALL yacStreamReadI64      (void);
   sF32  YAC_VCALL yacStreamReadF32      (void);
   sF64  YAC_VCALL yacStreamReadF64      (void);
   void  YAC_VCALL yacStreamReadObject   (YAC_Object*_dest); //getObject
   sSI   YAC_VCALL yacStreamReadString   (YAC_String *, sUI _maxlen); // raw chars
   sSI   YAC_VCALL yacStreamReadBuffer   (YAC_Buffer *_b, sUI _off, sUI _num, sBool _resize);
   sSI   YAC_VCALL yacStreamWrite        (sU8 *, sUI _num);
   void  YAC_VCALL yacStreamWriteI8      (sU8);
   void  YAC_VCALL yacStreamWriteI16     (sU16);
   void  YAC_VCALL yacStreamWriteI32     (sS32);
   void  YAC_VCALL yacStreamWriteI64     (sS64);
   void  YAC_VCALL yacStreamWriteF32     (sF32);
   void  YAC_VCALL yacStreamWriteF64     (sF64);
   void  YAC_VCALL yacStreamWriteObject  (YAC_Object *_src); //setObject
   sSI   YAC_VCALL yacStreamWriteString  (YAC_String *, sUI _off, sUI _num);
   sSI   YAC_VCALL yacStreamWriteBuffer  (YAC_Buffer *_buf, sUI _off, sUI _num);
   sSI   YAC_VCALL yacStreamReadLine     (YAC_String *_s, sUI _num);

   sBool YAC_VCALL yacStreamOpenLogic    (sChar *_name_in_pakfile);
   sBool YAC_VCALL yacStreamOpenLocal    (sChar *_local_pathname, sSI _write);
   void  YAC_VCALL yacStreamClose        (void);

   sSI  _deserialize                  (YAC_Object *_dest, sSI _typeinfo);
   sSI   eof                          (void);
   sSI  getByteOrder                  (void);
   sSI  getErrorCode                  (void);
   void getErrorStringByCode          (sSI _code, YAC_Value *_r);
   void setI8                         (sSI);
   void setI16                        (sSI);
   void setI32                        (sSI);
   void setF32                        (sF32);
   void setF64                        (sF32);
   sSI  getI8                         (void); // same as S8 (was unsigned before v0.9.14.30)
   sSI  getU8                         (void);
   sSI  getS8                         (void) { return getI8(); }
   sSI  getB8                         (void) { return (0 != getI8()) ? YAC_TRUE : YAC_FALSE; }
   sSI  getI16                        (void); // same as S16 (backwards compatibility)
   sSI  getU16                        (void);
   sSI  getS16                        (void) { return getI16(); }
   sSI  getI32                        (void);
   sF32 getF32                        (void);
   sF32 getF64                        (void);
   sUI  getOffset                     (void);
   sUI  getSize                       (void);
   sSI  readBuffer                    (YAC_Object *_buf, sSI _off, sSI _num, sSI _resize);
   sSI  readLine                      (YAC_String *_s, sSI _num);
   sSI  readString                    (YAC_String *_s, sSI _num);
   void _seek                         (sSI _off, sSI _mode);
   void _serialize                    (YAC_Object *_src, sSI _typeinfo);
   void setByteOrder                  (sSI);
   void setOffset                     (sUI);
   sSI  writeBuffer                   (YAC_Object *_buf, sSI _off, sSI _num);
   sSI  writeString                   (YAC_String *_s  , sSI _off, sSI _num);
   void setObject                     (YAC_Object *_src );
   void getObject                     (YAC_Object *_dest);
   void _close                        (void);

   // write from generic array object
   void setI8a                        (YAC_Object *_a);
   void setI16a                       (YAC_Object *_a);
   void setI32a                       (YAC_Object *_a);
   void setF32a                       (YAC_Object *_a);
   void setF64a                       (YAC_Object *_a);

   // read to generic array object
   void getI8a                        (YAC_Object *_a, sUI _num);  // (note) reallocates target array
   void getU8a                        (YAC_Object *_a, sUI _num);
   void getI16a                       (YAC_Object *_a, sUI _num);
   void getU16a                       (YAC_Object *_a, sUI _num);
   void getI32a                       (YAC_Object *_a, sUI _num);
   void getF32a                       (YAC_Object *_a, sUI _num);
   void getF64a                       (YAC_Object *_a, sUI _num);

   // vector-like utility write functions (similar to FloatArray).
   //  calls yacStreamWriteF32() for each parameter
   void add2f (sF32 _val1, sF32 _val2);
   void add3f (sF32 _val1, sF32 _val2, sF32 _val3);
   void add4f (sF32 _val1, sF32 _val2, sF32 _val3, sF32 _val4);

   void setRgba (sUI _argb32);
   void setBgra (sUI _argb32);
};

extern void YacStreamGetErrorStringByCode(sSI _code, YAC_Value *_r);

#endif  // TKS_STREAMBASE_H__
