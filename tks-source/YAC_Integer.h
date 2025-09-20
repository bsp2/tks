/// YAC_Integer.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __YAC_INTEGER_H__
#define __YAC_INTEGER_H__


class YAC_Integer : public YAC_Object { 
public: 
   sSI value;
public:
   YAC_Integer  (void);
   ~YAC_Integer ();
   
   void  YAC_VCALL yacSerialize         (YAC_Object *, sUI);
   sUI   YAC_VCALL yacDeserialize       (YAC_Object *, sUI);
   void  YAC_VCALL yacOperator          (sSI, YAC_Object *, YAC_Value *);
   void  YAC_VCALL yacOperatorI         (sSI, sSI, YAC_Value *_ret); 
   void  YAC_VCALL yacOperatorF32       (sSI, sF32, YAC_Value *_ret); 
   sUI   YAC_VCALL yacOperatorPriority  (void) { return YAC_OP_PRIO_INTEGER; } 
   sSI   YAC_VCALL yacTensorRank        (void) { return YAC_TENSOR_RANK_SCALAR; }

   sBool YAC_VCALL yacEquals           (YAC_Object *);
   sBool YAC_VCALL yacScanI            (sSI *); 
   sBool YAC_VCALL yacScanF32          (sF32 *); 
   sBool YAC_VCALL yacScanF64          (sF64 *); 
   sBool YAC_VCALL yacScanI64          (sS64 *); 
   sBool YAC_VCALL yacToString         (YAC_String *_s) const; 
   
   void  YAC_VCALL yacValueOfI         (sSI); 
   void  YAC_VCALL yacValueOfF32       (sF32); 
   void  YAC_VCALL yacValueOfF64       (sF64);  
   void  YAC_VCALL yacValueOfI64       (sS64);  

   void  myOperator          (sSI, sS32, YAC_Value *_ret); 
   void  setValue            (sSI);
   sSI   getValue            (void);
   void  _printf_YAC_RVAL    (YAC_Object*, YAC_Value *_r); 
   void  _printf_YAC_RARG    (YAC_Object*, YAC_Object *_r); 

   void  _getString          (YAC_Value *_r) const; 
   void  _New                (sSI, YAC_Value *_r); 
};


#endif // __YAC_INTEGER_H__
