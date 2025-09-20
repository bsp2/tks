/// YAC_ValueObject.h
///
/// (c) 2001-2020 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __YAC_VALUEOBJECT_H__
#define __YAC_VALUEOBJECT_H__


class YAC_ValueObject : public YAC_Object, public YAC_Value  {
public:
   YAC_ValueObject(void);
   ~YAC_ValueObject(); 
    
   void         YAC_VCALL yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r);
   
   void         YAC_VCALL yacOperatorAssign    (YAC_Object*);
   void         YAC_VCALL yacValueOfI          (sSI);
   void         YAC_VCALL yacValueOfI64        (sS64);
   void         YAC_VCALL yacValueOfF32        (sF32);
   void         YAC_VCALL yacValueOfF64        (sF64);
   sBool        YAC_VCALL yacToString          (YAC_String *) const;
   sBool        YAC_VCALL yacIsComposite       (void) {return 1;}
   sSI          YAC_VCALL yacTensorRank        (void) { return YAC_TENSOR_RANK_SCALAR; } // Rank depends on current assignment!
   
   void        _getValue          (YAC_Value *_r);
   sSI         _getType           (void);
   void        _setObjectValue    (YAC_Object *_o);
   void        _setStringValue    (YAC_Object *_s);
   YAC_Object *_setNewObject      (YAC_Object *_templ);
   void        _setValue          (YAC_Object *);
   void        _getString         (YAC_Value *_r) const;
   void        _setIntValue       (sSI);
   void        _setFloatValue     (sF32); 
   void        _initNull          (void); 
   void        _initVoid          (void); 
   void        _unset             (void);
   void        _getDeref          (YAC_Value *_r);
   sSI         _isDeletable       (void);
   void        _unlinkObject      (YAC_Value *_r);
   void        _assign            (YAC_Object *_valueObject);
   void        _pointerAssign     (YAC_Object *_valueObject);
   sBool       _isVoid            (void);
   sBool       _isInt             (void);
   sBool       _isFloat           (void);
   sBool       _isObject          (void);
   sBool       _isString          (void);
};


#endif // __YAC_VALUEOBJECT_H__
