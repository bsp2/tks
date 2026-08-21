/// YAC_NibbleStream.h
///
/// (c) 2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef YAC_NIBBLESTREAM_H__
#define YAC_NIBBLESTREAM_H__


class YAC_NibbleStream : public YAC_StreamBase {
  public:
   YAC_StreamBase *stream;
   sU8 bit_avail;
   sU8 bit_buffer;

  public:
   YAC_NibbleStream(void);
   ~YAC_NibbleStream();

  protected:
   sU8  readRawU8 (void);
   void writeRawU8 (sU8 _u8);

  public:
   sSI   YAC_VCALL yacStreamGetErrorCode (void);
   void  YAC_VCALL yacStreamSeek         (sSI _off, sUI _mode);
   sUI   YAC_VCALL yacStreamGetOffset    (void);
   void  YAC_VCALL yacStreamSetOffset    (sUI);
   sUI   YAC_VCALL yacStreamGetSize      (void);
   sBool YAC_VCALL yacStreamEOF          (void);
   sU8   YAC_VCALL yacStreamReadI8       (void);
   sU16  YAC_VCALL yacStreamReadI16      (void);
   sU32  YAC_VCALL yacStreamReadI32      (void);
   sU64  YAC_VCALL yacStreamReadI64      (void);
   sF32  YAC_VCALL yacStreamReadF32      (void);
   sF64  YAC_VCALL yacStreamReadF64      (void);
   void  YAC_VCALL yacStreamWriteI8      (sU8);
   void  YAC_VCALL yacStreamWriteI16     (sU16);
   void  YAC_VCALL yacStreamWriteI32     (sS32);
   void  YAC_VCALL yacStreamWriteI64     (sS64);
   void  YAC_VCALL yacStreamWriteF32     (sF32);
   void  YAC_VCALL yacStreamWriteF64     (sF64);
   void  YAC_VCALL yacStreamClose        (void);

   sBool YAC_VCALL yacStreamIsReadable (void);
   sBool YAC_VCALL yacStreamIsWritable (void);
   void  YAC_VCALL yacStreamFlush      (void);

  public:
   void _setStream (YAC_Object *_s);
   YAC_Object *_getStream (void);

   sUI  getU4 (void);
   sSI  getS4 (void);
   sSI  getI4 (void);

   sUI  getU12 (void);
   sSI  getS12 (void);
   sSI  getI12 (void);

   sUI  getU20 (void);
   sSI  getS20 (void);
   sSI  getI20 (void);

   sUI  getU28 (void);
   sSI  getS28 (void);
   sSI  getI28 (void);

   void setI4 (sUI _v4);
   void setI12 (sUI _v12);
   void setI20 (sUI _v20);
   void setI28 (sUI _v28);
};


#endif // YAC_NIBBLESTREAM_H__
