/// YAC_StdErrStream.h
///
/// (c) 2003-2006 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __YAC_STDERRSTREAM_H__
#define __YAC_STDERRSTREAM_H__


class YAC_StdErrStream: public YAC_StreamBase { 
public: 
   YAC_StdErrStream(void); 
   ~YAC_StdErrStream(); 
   
   void  YAC_VCALL yacOperator                   (sSI, YAC_Object *, YAC_Value *); 
 
   sSI   YAC_VCALL yacStreamGetErrorCode         (void); 
   void  YAC_VCALL yacStreamGetErrorStringByCode (sSI, YAC_Value*); 
   sUI   YAC_VCALL yacStreamGetOffset            (void); 
   sU32  YAC_VCALL yacStreamGetSize              (void); 
   void  YAC_VCALL yacStreamWriteI8              (sU8); 
   sSI   YAC_VCALL yacStreamWrite                (sU8 *, sU32); 
   sBool YAC_VCALL yacStreamEOF                  (void); 
   sBool YAC_VCALL yacIsStream                   (void); 
   
   sSI  isOpen    (void); 
   void flush     (void); 
}; 


#endif
