/// TKS_Pool.h
///
/// (c) 2001-2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_POOL_H__
#define __TKS_POOL_H__

 
class TKS_Pool : public YAC_Object, public Pool {
public:
   YAC_Object *template_object;
   sBool       fix_stringtype;
   sBool       init_classtype;
   
public:
   TKS_Pool(void);
   ~TKS_Pool();
   
   sBool         YAC_VCALL yacIteratorInit        (YAC_Iterator *) const;
   void          YAC_VCALL yacArraySet            (void *_context, sUI _index, YAC_Value *_value);
   void          YAC_VCALL yacArrayGet            (void *_context, sUI _id, YAC_Value *_ret); 
   sUI           YAC_VCALL yacArrayGetElementType (void) { return YAC_TYPE_OBJECT; } 
   void          YAC_VCALL yacSerialize           (YAC_Object *, sUI);
   sUI           YAC_VCALL yacDeserialize         (YAC_Object *, sUI);
   
   void          YAC_VCALL yacOperator            (sSI, YAC_Object *, YAC_Value *);
   
   void          setTemplate            (YAC_Object *);
   YAC_Object *  getTemplate            (void);
   sSI           _alloc                 (sSI _num);
   void          _free                  (void);
   sSI           getNumElements         (void);
   sSI           getMaxElements         (void);
   void          _qFreeByObject         (YAC_Object *_o);
};

#endif // __TKS_POOL_H__
