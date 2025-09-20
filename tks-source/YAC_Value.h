/// YAC_Value.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef __YAC_VALUE_H__
#define __YAC_VALUE_H__


extern const char*tks_typenames[5];
extern YAC_Object*yac_null; 


class YAC_Value : public YAC_ValueS {
public:
   TKS_INLINE_METHOD YAC_Value(void) {  
     type = 0;  
   }  

   //TKS_INLINE_METHOD ~YAC_Value() = 0;//{ }

   TKS_INLINE_METHOD void        operator =              (YAC_Value*_v) {
      // ---- this one gets executed ---- 
      fastDerefValue(_v);
   }

   TKS_INLINE_METHOD void        operator =              (YAC_Value&_v) {
      // ---- this one is only there to satisfy the c++ compiler (vc6) ---- 
      fastDerefValue(&_v);
   }

   void        assignValue             (YAC_Value*_v);
   

   TKS_INLINE_METHOD void        copySafe                (YAC_Value*_v) { // _v->deleteme=0
      if(_v)
      {
         if((type>=YAC_TYPE_OBJECT) && deleteme && value.object_val) 
         {
            if(_v->type >= YAC_TYPE_OBJECT)
            {
               if(value.any == _v->value.any)
               {
                  if(_v->deleteme)
                  {
                     deleteme     = 1; 
                     _v->deleteme = 0;
                  }
                  return;
               }
               else
               {
                  YAC_DELETE(value.object_val);
               }
            }
         }
         deleteme      = _v->deleteme;
         _v->deleteme  = 0;
         type          = _v->type;
         value.any     = _v->value.any;
      }
      else
      {
         if((type>=YAC_TYPE_OBJECT) && deleteme && value.object_val)  
         {
            YAC_DELETE(value.object_val);
         }
         initInt(0); // xxx should be initVoid() ?
      }
   }

   TKS_INLINE_METHOD void        derefObjectValue        (YAC_Value *_r) {
      if(type>=YAC_TYPE_OBJECT) 
      {
         _r->copySafe(this); 
      }
      else 
      {
         _r->initNull(); 
      }
   }
   
   void        derefPointerAssignValue (YAC_Value *_v);

   TKS_INLINE_METHOD void	      fixStringType           (void) {
      if((type==YAC_TYPE_OBJECT) && value.object_val)  
      {
         if(value.object_val->class_ID==YAC_CLID_STRING)	  
         {
            type=YAC_TYPE_STRING;  
         } 
      }
   }

   TKS_INLINE_METHOD sSI         getIntValue             (void) {
      switch(type)
      {
      default:
      case 0:	return 0;
      case 1:	return value.int_val;
      case 2:	return (sSI)value.float_val;
      case 3:
      case 4:
         {
            sSI r;
            if(YAC_VALID(value.object_val)) 
            { 
               value.object_val->yacScanI(&r); 
            } 
            else 
            { 
               r=0; 
            }
            return r;
         }
      }
   }

   TKS_INLINE_METHOD sF32        getFloatValue           (void) {
      switch(type)
      {
      default:
      case 0:	return 0.0f;
      case 1:	return (sF32)value.int_val;
      case 2:	return value.float_val;
      case 3:
      case 4:
         {
            sF32 r;
            if(YAC_VALID(value.object_val)) 
            { 
               value.object_val->yacScanF32(&r); 
            } 
            else 
            { 
               r=0.0f; 
            }
            return r;
         }
      }
   }

   TKS_INLINE_METHOD YAC_Object *getObjectValue          (void) {
      switch(type)
      {
      default:
      case 0:	
      case 1:
      case 2: return 0;
      case 3:
      case 4: return value.object_val;
      }
   }

   void        getStringValue          (YAC_Value *_r);

   const char *getTypeName             (void) const;

   void        initEmptyString         (void);

   TKS_INLINE_METHOD void        initInt                 (sSI _i) {
      deleteme = 0;
      type = 1;
      value.int_val = _i;
   }

   TKS_INLINE_METHOD void        initFloat               (sF32 _f) {
      deleteme = 0;  
      type = YAC_TYPE_FLOAT;  
      value.float_val = _f;
   }

   void        initNewString           (YAC_String *s);

   TKS_INLINE_METHOD void        initAny                 (void *_o, sBool _del) {
      // xxx still used??
      type      = YAC_TYPE_OBJECT;  
      value.any = _o;  
      deleteme  = _del; 
   }

   TKS_INLINE_METHOD void        initObject              (YAC_Object *_o, sBool _del) {
      type             = YAC_TYPE_OBJECT;  
      value.object_val = _o;  
      deleteme         = _del; 
   }

   TKS_INLINE_METHOD void        initString              (YAC_String *_s, sBool _del) {
      deleteme         = _del; 
      type             = YAC_TYPE_STRING; 
      value.string_val =_s; 
   }

   TKS_INLINE_METHOD void        fastDerefValue          (YAC_Value *_v) {
      deleteme     = _v->deleteme;  
      _v->deleteme = 0; 
      type         = _v->type;  
      value        = _v->value;  
   }

   TKS_INLINE_METHOD void        initValue               (const YAC_Value *_v) {
      if(_v) 
      { 
         if((type>=YAC_TYPE_OBJECT) && deleteme && value.object_val)  
         {
            YAC_DELETE(value.object_val);
         }
         deleteme  = 0; 
         value.any = _v->value.any; 
         type      = _v->type; 
      } 
      else 
      { 
         if((type>=YAC_TYPE_OBJECT) && deleteme && value.object_val)  
         {
            YAC_DELETE(value.object_val);
         }
         deleteme=0; 
         initVoid(); 
      } 
   }

   TKS_INLINE_METHOD void        initVoid                (void) {
      deleteme = 0;  
      type = 0;  
      value.object_val = 0; 
   }

   TKS_INLINE_METHOD void        safeInitInt             (sSI _i) {
      unsetFast();  
      initInt(_i); 
   }

   TKS_INLINE_METHOD void        safeInitFloat           (sF32 _f) {
      unsetFast();  
      initFloat(_f); 
   }

   TKS_INLINE_METHOD void        safeInitString          (YAC_String *_s, sBool _del) {
      unsetFast();   
      initString(_s, _del);  
   }

   TKS_INLINE_METHOD void        safeInitObject          (YAC_Object *_o, sBool _new) {
      unsetFast();   
      initObject(_o, _new); 
   }

   void        typecast                (sUI _type);

   TKS_INLINE_METHOD void        unset                   (void) {
      if( (type>=YAC_TYPE_OBJECT) && deleteme && value.any)
      { 
         YAC_DELETE(value.object_val);
         deleteme=0;
      } 
      type      = YAC_TYPE_VOID; 
      value.any = NULL;  
   }

   TKS_INLINE_METHOD void        unsetFast               (void) {
      if( (type>=YAC_TYPE_OBJECT) && deleteme && value.any)
      {
         YAC_DELETE(value.object_val);
      }
   }

   TKS_INLINE_METHOD void        unsetObject             (void) {
      if( (type >= YAC_TYPE_OBJECT) && deleteme && value.any) 
      { 
         YAC_DELETE(value.object_val);
         deleteme = 0; 
      } 
      value.object_val = 0; 
   }

                     void        toString                (YAC_String *_s) const;

   TKS_INLINE_METHOD void        duplicate               (TKS_Context *_ctx, YAC_Value *c) {
      switch(c->type) 
      { 
      case 0: 
         break; 
      case 1: 
         initInt(c->value.int_val); 
         break; 
      case 2: 
         initFloat(c->value.float_val); 
         break; 
      case 3: 
      case 4: 
         if(YAC_VALID(c->value.object_val))
         { 
            value.object_val = YAC_CLONE_POOLED(_ctx, c->value.object_val);  // xxx TKS_MT: use *real* thread context (no c'tors/init otherwise)
            ////value.object_val->yacOperatorAssign(c->value.object_val); 
            value.object_val->yacArrayCopySize(c->value.object_val);
            type = c->type; 
            deleteme = 1; 
         } 
         else 
         { 
            type = c->type; 
            deleteme = 0; 
            value.object_val = 0;
         } 
         break; 
      } 
   }

   TKS_INLINE_METHOD void        initNull                (void) {
      type             = YAC_TYPE_OBJECT; 
      value.object_val = yac_null; 
      deleteme         = 0; 
   }

   TKS_INLINE_METHOD void        safeInitNull            (void) {
      unsetFast(); 
      initNull(); 
   }

   TKS_INLINE_METHOD sBool       isNull(void) {
      switch(type)
      {
      default:
      case YAC_TYPE_VOID:
         return 1;
      case YAC_TYPE_INT:
         return (value.int_val == 0);
      case YAC_TYPE_FLOAT:
         return (((sSI)value.float_val) == 0);
      case YAC_TYPE_OBJECT:
      case YAC_TYPE_STRING:
         if(value.object_val)
         {
            return (value.object_val->class_ID == YAC_CLID_OBJECT);
         }
         else
         {
            return 1;
         }
         break;
      }
   }

   TKS_INLINE_METHOD sBool       isNullOrIF0(void) {
      switch(type) 
      { 
      default: 
      case YAC_TYPE_VOID: 
         return 1; 
      case YAC_TYPE_INT: 
         return (value.int_val==0); 
      case YAC_TYPE_FLOAT: 
         return (((sSI)value.float_val)==0); 
      case YAC_TYPE_OBJECT: 
      case YAC_TYPE_STRING: 
         if(value.object_val) 
         { 
            if(value.object_val->class_ID == YAC_CLID_OBJECT)
            {
               return 1;
            }
            else
            {
               sSI i;
               if(value.object_val->yacScanI(&i))  // Number objects (e.g. Boolean)
               {
                  return (0 == i);
               }
               else
               {
                  return 0;
               }
            }
         } 
         else 
         { 
            return 1; 
         } 
         break; 
      } 
   }

   sBool       compareValue            (YAC_Value *_v);
};

 
#endif // __YAC_VALUE_H__
