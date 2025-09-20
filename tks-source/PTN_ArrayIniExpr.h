/// PTN_ArrayIniExpr.h
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#ifndef __PTN_ARRAYINIEXPR_H__
#define __PTN_ARRAYINIEXPR_H__


class PTN_ArrayIniExpr : public PTN_Expr {
public:
   enum __arraytypes {
      ATYPE_UNKNOWN = 0xFF,
      ATYPE_VALUE   = 0,
      ATYPE_INT     = 1,
      ATYPE_FLOAT   = 2,
      ATYPE_POINTER = 3,
      ATYPE_STRING  = 4,
         
      ATYPE_PAD
   }; 
   
public:
   PTN_ExprArrayEntry *first_entry;
   sSI                num_entries;      // array size
   sU8                cached_arraytype; // first evaluation
   union _arrays {
      YAC_IntArray     *int_array;
      YAC_FloatArray   *float_array;
      YAC_PointerArray *pointer_array;  // array of arrays, array of hashes..
      YAC_ValueArray   *value_array;
      YAC_StringArray  *string_array;
      YAC_Object       *any_array;
   } arrays;
   sBool              is_const;     // 2=const+optimized in isConst(), 1=const+optimized in optimize(), 0=dynamic 
   sBool              b_always_new; // 1=always return array copies (reentrancy!)
   
public:
   PTN_ArrayIniExpr  (PTN_ExprArrayEntry *_first, sBool _bAlwaysNew);
   ~PTN_ArrayIniExpr ();
   
   sBool     semanticCheck (void);
   sBool     resolveXRef   (void);
   void      optimize      (void); 
   sBool     isConst       (void);
   void      eval          (PTN_Env *_env, YAC_Value *) const; 
   void      eval          (PTN_Env *_env, YAC_Value *, sBool _const); 
   void      evalConst     (YAC_Value *); 
   Feval     getEval       (void) const; 

   void allocArray (void);
};


#endif // __PTN_ARRAYINIEXPR_H__
