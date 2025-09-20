/// YAC_File.h
///
/// (c) 2003-2006 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __YAC_FILE_H__
#define __YAC_FILE_H__


class YAC_File: public YAC_StreamBase {
public:
   FILE *handle; 
   char *tmpName;
   
public:
   YAC_File(void);
   ~YAC_File();
   
   void  YAC_VCALL yacOperator(sSI, YAC_Object *, YAC_Value *);
  
   void  YAC_VCALL yacGetConstantStringList(YAC_String *_s);
   
   sSI   YAC_VCALL yacStreamGetErrorCode         (void); 
   void  YAC_VCALL yacStreamGetErrorStringByCode (sSI, YAC_Value*);
   sUI   YAC_VCALL yacStreamGetOffset            (void);
   void  YAC_VCALL yacStreamSetOffset            (sUI);
   void  YAC_VCALL yacStreamSeek                 (sSI _offset, sUI _mode);
   sU32  YAC_VCALL yacStreamGetSize              (void);
   sSI   YAC_VCALL yacStreamRead                 (sU8 *, sU32);
   void  YAC_VCALL yacStreamWriteI8              (sU8);
   sU8   YAC_VCALL yacStreamReadI8               (void);
   sSI   YAC_VCALL yacStreamWrite                (sU8 *, sU32);
   sBool YAC_VCALL yacStreamEOF                  (void);
   sBool YAC_VCALL yacStreamOpenLocal            (sChar *_logic_name, sSI _access);
   void  YAC_VCALL yacStreamClose                (void);
   sBool YAC_VCALL yacIsStream                   (void);
   
   sSI  _open     (YAC_String *_name, sSI _access);
   sSI  openLocal (YAC_String *_name, sSI _access);
   sSI  isOpen    (void);
   void flush     (void); 
   void _openTemp (YAC_String *_dir, YAC_String *_pfx, YAC_Value *_r); 
   void removeTemp(void);
   
};


#endif
