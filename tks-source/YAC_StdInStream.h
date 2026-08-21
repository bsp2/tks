/// YAC_StdInStream.h
///
/// (c) 2003-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef YAC_STDINSTREAM_H__
#define YAC_STDINSTREAM_H__


class YAC_StdInStream: public YAC_StreamBase {
public:
   sBool b_eof;

   YAC_StdInStream(void);
   ~YAC_StdInStream();

   void  YAC_VCALL yacOperator                   (sSI, YAC_Object *, YAC_Value *);

   sSI   YAC_VCALL yacStreamGetErrorCode         (void);
   void  YAC_VCALL yacStreamGetErrorStringByCode (sSI, YAC_Value*);
   sUI   YAC_VCALL yacStreamGetOffset            (void);
   void  YAC_VCALL yacStreamSetOffset            (sUI);
   void  YAC_VCALL yacStreamSeek                 (sSI _offset, sUI _mode);
   sU32  YAC_VCALL yacStreamGetSize              (void);
   sSI   YAC_VCALL yacStreamRead                 (sU8 *, sU32);
   sU8   YAC_VCALL yacStreamReadI8               (void);
   sBool YAC_VCALL yacStreamEOF                  (void);
   sBool YAC_VCALL yacIsStream                   (void);

   sBool YAC_VCALL yacStreamIsReadable (void) { return YAC_TRUE;  }
   sBool YAC_VCALL yacStreamIsWritable (void) { return YAC_FALSE; }

   sSI  isOpen    (void);
   void flush     (void);
};


#endif // YAC_STDINSTREAM_H__
