/// YAC_Value.cpp
///
/// (c) 2001-2023 by Bastian Spiegel <bs@tkscript.de>
///    published under terms of the GNU general public license (GPL).
///

extern "C" {
#include <stdio.h>
#include <stdlib.h>
}

#include "tks.h"

#include "YAC_Value.h" 
#include "YAC_Integer.h" 
#include "YAC_Float.h"


const char *tks_typenames[5] = {"void", "int", "float", "Object", "String"};


void YAC_Value::assignValue(YAC_Value *_v) {
   // dont grab pointer, just copy 
   // typecast values, scan strings, copy objects
   switch(type) 
   { 
      case YAC_TYPE_VARIANT/*0*/:  // any -> variant
         initValue(_v); 
         break; 

      case YAC_TYPE_INT: 
      {
         switch(_v->type) 
         { 
            case 1: // int->int
               value.int_val=_v->value.int_val; 
               break; 
            case 2: // float->int
               value.int_val=(sSI)_v->value.float_val; 
               break; 
            case 3: // Object->int
            case 4: // String->int
               if(YAC_VALID(_v->value.object_val)) 
               { 
                  _v->value.object_val->yacScanI(&value.int_val); 
               } 
               else  
               {
                  value.int_val = 0;  
               }
               break; 
         } 
      }
      break; 

      case YAC_TYPE_FLOAT:
      {
         switch(_v->type) 
         { 
            case 1: // int -> float
               value.float_val=(sF32)_v->value.int_val; 
               break; 
            case 2: // float -> float
               value.float_val=_v->value.float_val; 
               break; 
            case 3: // Object -> float
            case 4: // String -> float
               if(YAC_VALID(_v->value.object_val)) 
               { 
                  _v->value.object_val->yacScanF32(&value.float_val); 
               } 
               else  
               {
                  value.float_val=0.0f;  
               }
               break; 
         } 
      }
      break; 

      case YAC_TYPE_OBJECT:
      case YAC_TYPE_STRING:
      {
         switch(_v->type) 
         { 
            case 1: // int -> Object
               if(YAC_VALID(value.object_val))  
               {
                  value.object_val->yacOperatorI(YAC_OP_ASSIGN, _v->value.int_val, 0);  
               }
               break; 

            case 2: // float -> Object
               if(YAC_VALID(value.object_val))  
               {
                  value.object_val->yacOperatorF32(YAC_OP_ASSIGN, _v->value.float_val, 0);  
               }
               break; 
            case 3: // Object -> Object
            case 4: // String -> Object
               if(YAC_VALID(value.object_val))  
               {
                  value.object_val->yacOperatorAssign(_v->value.object_val);  
               }
               break; 
         } 
      }
      break; 

#if 0
      case YAC_TYPE_STRING: 
      { 
         switch(_v->type) 
         { 
            case 1: // int -> String
               if(deleteme && YAC_VALID(value.string_val)) 
               { 
                  value.string_val->alloc(32);
                  value.string_val->printf("%i", _v->value.int_val); 
               } 
               else 
               { 
                  value.string_val = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING16);
                  value.string_val->printf("%i", _v->value.int_val); 
                  deleteme=1; 
               } 
               break; 
            case 2: // float -> String
               if(deleteme && YAC_VALID(value.string_val)) 
               { 
                  value.string_val->alloc(32);
                  value.string_val->printf("%g", _v->value.float_val); 
               } 
               else 
               { 
                  value.string_val = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING32);
                  ////value.string_val->alloc(48); 
                  value.string_val->printf("%g", _v->value.float_val); 
                  deleteme=1; 
               } 
               break; 
            case 3: // Object -> String
            case 4: // String -> String
               if(! YAC_VALID(value.string_val)) 
               { 
                  value.string_val = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
                  deleteme=1; 
               } 
               ////if(YAC_VALID(_v->value.object_val)) 
               {
                  ////_v->value.object_val->yacToString(value.string_val); 
                  value.string_val->yacCopy(_v->value.object_val);
               }
               break; 
               ////case 4: 
               ////   if(! YAC_VALID(value.string_val))
               ////   { 
               ////      value.string_val = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
               ////      deleteme=1; 
               ////   } 
               ////   value.string_val->yacCopy(_v->value.string_val); 
               ////   break; 
         } 
      }
      break;
#endif // 0
   }

}

void YAC_Value::derefPointerAssignValue (YAC_Value *_v) {
   // unsets value after copy/assign
   // <=, grab read-write objects (usually _v is a temp value) 
   // Used to assign current values in function argument lists
   switch(type) // required variable type 
   { 
      case YAC_TYPE_VARIANT: // any -> variant
         copySafe(_v); 
         break; 
      case YAC_TYPE_INT: 
         switch(_v->type) 
         { 
            case YAC_TYPE_VOID:  // void -> int
               value.int_val = 0; 
               break;
            case YAC_TYPE_OBJECT:  // Object -> init
            case YAC_TYPE_STRING:  // String -> init
               if(YAC_VALID(_v->value.object_val)) 
               { 
                  if(! _v->value.object_val->yacScanI(&value.int_val)) 
                  { 
                     value.int_val=0; 
                  } 
               } 
               else 
               {
                  value.int_val=0;  
               } 
               _v->unsetFast();
               break; 
            case YAC_TYPE_INT: // int -> int
               value.int_val=_v->value.int_val;  
               break; 
            case YAC_TYPE_FLOAT: // float -> int
               value.int_val=(sSI)_v->value.float_val;  
               break; 
         } 
         break; 
      case YAC_TYPE_FLOAT: 
         switch(_v->type) 
         { 
            default:
               // ---- illegal argument ---- 
               _v->unset(); 
               break; 
            case YAC_TYPE_VOID:  // void -> float
               value.float_val=0.0f; 
               break;
            case YAC_TYPE_OBJECT: // Object -> float
            case YAC_TYPE_STRING: // String -> float
               if(YAC_VALID(_v->value.object_val)) 
               { 
                  if(! _v->value.object_val->yacScanF32(&value.float_val)) 
                  { 
                     value.float_val = 0.0f; 
                  } 
               } 
               else 
               { 
                  value.float_val=0.0f;  
               } 
               _v->unsetFast(); 
               break;  
            case YAC_TYPE_INT: // int -> float
               value.float_val=(sF32)_v->value.int_val; 
               break; 
            case YAC_TYPE_FLOAT: // float -> float
               value.float_val=_v->value.float_val; 
               break; 
         } 
         break; 
         
      case YAC_TYPE_OBJECT: 
         switch(_v->type) 
         { 
            case YAC_TYPE_VOID: // void -> Object
               safeInitNull(); 
               break;
            case YAC_TYPE_INT:  // int -> Object
               if(YAC_VALID(value.object_val) && YAC_BCHK(value.object_val, YAC_CLID_STRING))
               {
                  // "this" is String with Object type
                  YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING16);
                  s->yacValueOfI(_v->value.int_val); 
                  safeInitString(s, 1); 
               }
               else
               {
                  YAC_Integer *no = (YAC_Integer*) YAC_NEW_CORE_POOLED(YAC_CLID_INTEGER);
                  no->value = _v->value.int_val; 
                  safeInitObject(no, 1); 
               } 
               break; 
            case YAC_TYPE_FLOAT: // float -> Object
               if(YAC_VALID(value.object_val) && YAC_BCHK(value.object_val, YAC_CLID_STRING))
               {
                  // "this" is String with Object type
                  YAC_String *s = (YAC_String*)YAC_NEW_CORE_POOLED(YAC_CLID_STRING32);
                  s->yacValueOfF32(_v->value.float_val);
                  safeInitString(s, 1);
               }
               else
               { 
                  YAC_Float *no = (YAC_Float*) YAC_NEW_CORE_POOLED(YAC_CLID_FLOAT);
                  no->value = _v->value.float_val; 
                  safeInitObject(no, 1); 
               } 
               break; 
            case YAC_TYPE_STRING: // String -> Object
               // copy non-deletable reference
               copySafe(_v); 
               type = YAC_TYPE_OBJECT; // preserve original type
               break;
            case YAC_TYPE_OBJECT: // Object -> Object
               copySafe(_v); 
               break; 
         } 
         break; 

      case YAC_TYPE_STRING: 
         switch(_v->type) 
         { 
            case YAC_TYPE_VOID:  // void -> String
               safeInitNull();
               // illegal argument 
               break; 
            case YAC_TYPE_OBJECT: // Object -> String
               if(YAC_VALID(_v->value.object_val)) 
               {  
                  if(YAC_BCHK(_v->value.object_val, YAC_CLID_STRING))
                  {
                     // Actually is String object with type YAC_TYPE_OBJECT
                     copySafe(_v);
                  }
                  else
                  {
                     if( value.object_val != (YAC_Object*)this ) 
                     { 
                        // Note: must NOT re-use current String since this has a nasty side-effect in param lists
                        //         1.call: setText(String) <-- pass real string
                        //         2.call: setText(float)  <-- String from last call is re-used to store float conv.
                        //             --> do not allow this! (hence the !deleteme condition)
                        if(!deleteme || ! YAC_VALID(value.string_val)) 
                        { 
                           initEmptyString(); 
                        }
                        _v->value.object_val->yacToString(value.string_val); 
                        _v->unset();  
                     }
                  }
               }  
               else 
               { 
                  safeInitNull(); 
               }
               break; 
            case YAC_TYPE_INT:  // int -> String
            { 
               YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING16);
               s->yacValueOfI(_v->value.int_val); 
               safeInitString(s, 1); 
            }
            break; 
            case YAC_TYPE_FLOAT: // float -> String
            { 
               YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING32);
               s->yacValueOfF32(_v->value.float_val); 
               safeInitString(s, 1); 
            } 
            break;  
            case YAC_TYPE_STRING: // String -> String
               copySafe(_v); 
               break; 
         } 
         break; 
   } 
}


// ---- toString ---- 
void YAC_Value::toString(YAC_String *s) const { 
   sBool is_valid; 
   sBool is_string; 
   sBool is_class; 
    
   s->alloc(2048); 
   s->empty(); 

   if(type >= YAC_TYPE_OBJECT) 
   { 
      if(value.object_val) 
      { 
         sBool isNull=(value.object_val->class_ID==YAC_CLID_OBJECT); 
         is_valid = YAC_VALID(value.object_val);
         if(is_valid)
         { 
            if(type == YAC_TYPE_STRING) 
            { 
               is_string=1; 
               is_class=0; 
            } 
            else 
            { 
               is_string=(value.object_val->class_ID==YAC_CLID_STRING); 
               is_class=(!is_string) && (value.object_val->yacMetaClassName()!=0); 
            } 
         } 
         else 
         { 
            is_string=0; is_class=0; 
         } 
          
         if(is_valid) 
         { 
            if(is_class) 
            { 
               // ---- meta-class instance 
               YAC_String t; 
               value.object_val->yacToString(&t); 
               s->printf("<class<%s>%c" YAC_PRINTF_PTRPREFIX "%p(%s)>", 
                         (char*)value.object_val->yacMetaClassName(), 
                         (char) (deleteme?'@':'#'), 
                         (void*)value.object_val, 
                         (char*)t.chars 
                         ); 
            } 
            else 
            { 
               // ---- object 
               YAC_String t; 
               value.object_val->yacToString(&t); 
               s->printf("<%s%c" YAC_PRINTF_PTRPREFIX "%p(%s)>", 
                         (char*)value.object_val->yacClassName(), 
                         (char) (deleteme?'@':'#'), 
                         (void *)value.object_val, 
                         (char*)t.chars 
                         ); 
            } 
             
         } 
         else 
         { 
            // ---- invalid object (null or already deleted..) 

            if(isNull) 
               s->printf("<null%c" YAC_PRINTF_PTRPREFIX "%p>", 
                         (char) (deleteme?'@':'#'), 
                         value.any 
                         ); 
            else 
               s->printf("<*ILL*(%s)%c" YAC_PRINTF_PTRPREFIX "%p>", 
                         (char*)((type==4)?"String":"Object"), 
                         (char) (deleteme?'@':'#'), 
                         value.any
                         ); 
         } 
      } 
      else 
      { 
         s->printf("<null>"); 
      } 
   } 
   else if(type==YAC_TYPE_INT)
   { 
      // ---- int 
      s->printf("<int:%i>", 
                value.int_val); 
   } 
   else if(type==YAC_TYPE_FLOAT) 
   { 
      // --- float 
      s->printf("<float:%g>", 
                value.float_val); 
   } 
   else 
   { 
      s->printf("<void>"); 
   } 
} 


sBool YAC_Value::compareValue(YAC_Value *_v) { 
   if(_v) 
   { 
      if(type==_v->type) 
      { 
         switch(type) 
         { 
            default: 
            case YAC_TYPE_VOID: 
               return 1; 
            case YAC_TYPE_INT: 
               return (value.int_val==_v->value.int_val); 
               break; 
            case YAC_TYPE_FLOAT: 
               return  
                  (value.float_val>=(_v->value.float_val-YAC_FLOAT_DEVIATION))&& 
                  (value.float_val<=(_v->value.float_val+YAC_FLOAT_DEVIATION)); 
               break; 
            case YAC_TYPE_OBJECT: 
            case YAC_TYPE_STRING: 
               ////if(isNullOrIF0()) 
               if(isNull())
               { 
                  ////return _v->isNullOrIF0(); 
                  return _v->isNull();
               } 
               else 
               { 
                  YAC_Value r; 
                  value.object_val->yacOperator(YAC_OP_CEQ, _v->value.object_val, &r); 
                  return r.value.int_val; ////(r.value.object_val != NULL); 
               } 
               break; 
         } 
      } 
   } 
   return 0; 
}
 
const char *YAC_Value::getTypeName(void) const { 
   // xxx still used ??
   switch(type) 
   { 
      case YAC_TYPE_VOID:   return "void"; 
      case YAC_TYPE_INT:    return "int"; 
      case YAC_TYPE_FLOAT:  return "float"; 
      case YAC_TYPE_OBJECT: return "Object"; 
      case YAC_TYPE_STRING: return "String"; 
      default:              return "???"; 
   } 
}

void YAC_Value::initEmptyString(void) {
   deleteme         = 1;
   type             = YAC_TYPE_STRING;
   value.string_val = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
}

void YAC_Value::initNewString(YAC_String *s) {
   initEmptyString();
   value.string_val->yacCopy(s);
   value.string_val->bflags |= TKS_STRFL_IMM; // make chars immutable
}

void YAC_Value::getStringValue(YAC_Value *_r) {
   if(type==YAC_TYPE_STRING) 
   {
      _r->initString(value.string_val, 0);  
   }
   else 
   { 
      YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
      switch(type)
      {
      case 0:	s->alloc(8);   s->copy("<void>");                break;
      case 1:	s->alloc(32);  s->printf("%i", value.int_val);	 break;
      case 2:	s->alloc(48);  s->printf("%g", value.float_val); break;
      case 3:	s->alloc(128); s->printf("<Object %s @" YAC_PRINTF_PTRPREFIX "%p>", (char*)(value.object_val?value.object_val->yacClassName():"???"), (void*)value.object_val );
         break;
      }
      _r->initString(s, 1);
   }
}

void YAC_Value::typecast(sUI _type) {
   sSI         i_t; 
   sF32        f_t; 
   
   switch(_type) 
   { 
      default: 
         unset(); 
         break; 
      case YAC_TYPE_INT: 
         switch(type) 
         {  
            default:
            case 0: 
               initInt(0); 
               break; 
            case 1: 
               break; 
            case 2: 
               initInt((sSI)value.float_val);
               break; 
            case 3: 
               //i_t=(value.object_val!=0); 
               //safeInitInt(i_t);  
               if(value.object_val) 
               { 
                  value.object_val->yacScanI(&i_t); 
                  safeInitInt(i_t); 
               } 
               else 
                  initInt(0);
               break; 
            case 4: 
               if(value.string_val)  
               { 
                  if(value.string_val->checkFloatConversion()) 
                  { 
                     value.string_val->yacScanF32(&f_t); 
                     safeInitInt((sSI)f_t); 
                  } 
                  else 
                  {
                     value.string_val->yacScanI(&i_t);  
                     safeInitInt(i_t); 
                  } 
               }
               else  
               { 
                  safeInitInt(0); 
               }
               break; 
         } 
         break; 

      case YAC_TYPE_FLOAT: 
         switch(type) 
         {  
            default: 
            case 0: 
               initFloat(0.0f); 
               break;
            case 1: 
               initFloat((sF32)value.int_val);
               break; 
            case 2: 
               break; 
            case 3:  
            case 4: 
               // ---- convert object to float value 
               if(value.object_val) 
                  value.object_val->yacScanF32(&f_t); 
               else 
                  f_t=0.0f; 
               safeInitFloat(f_t); 
               break; 
         } 
         break; 

      case YAC_TYPE_OBJECT:  
         switch(type) 
         { 
            default: 
            case YAC_TYPE_VOID: 
               // ---- typecast void to "null" Object 
               initObject(TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_OBJECT]->template_object, 0); 
               break; 
            case YAC_TYPE_INT: 
               // ---- typecast int to Integer Object 
               { 
                  YAC_Integer *io = (YAC_Integer*) YAC_NEW_CORE_POOLED(YAC_CLID_INTEGER);
                  io->value=value.int_val; 
                  initObject(io, 1); 
               } 
               break; 
            case YAC_TYPE_FLOAT: 
               // ---- typecast float to Float Object 
               { 
                  YAC_Float *fo = (YAC_Float*) YAC_NEW_CORE_POOLED(YAC_CLID_FLOAT);
                  fo->value=value.float_val; 
                  initObject(fo, 1); 
               } 
               break; 
            case YAC_TYPE_OBJECT: 
            case YAC_TYPE_STRING: 
               break; 
         } 
         break;

      case YAC_TYPE_STRING: 
      { 
         switch(type) 
         {  
            default:
            case YAC_TYPE_VOID: 
               initEmptyString(); 
               break; 
            case YAC_TYPE_INT: 
               i_t = value.int_val; 
               value.string_val = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING16);
               //value.string_val->alloc(32); 
               value.string_val->printf("%i", i_t); 
               deleteme=1; 
               type=YAC_TYPE_STRING; 
               break; 
            case YAC_TYPE_FLOAT: 
               f_t = value.float_val; 
               value.string_val = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING32);
               //value.string_val->alloc(48); 
               value.string_val->printf("%g", f_t); 
               deleteme=1; 
               type=YAC_TYPE_STRING; 
               break; 
            case YAC_TYPE_OBJECT:  
               if(YAC_VALID(value.object_val))
               { 
                  if(value.object_val->class_ID!=YAC_CLID_STRING) // string fix? 
                  { 
                     YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
                     value.object_val->yacToString(s); // convert Object to String 
                     safeInitString(s, 1); 
                  } 
               } 
               else 
               { 
                  // ---- convert null object to empty String "" 
                  ////initString((YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING), 1); 
                  initNull();
               } 
               break;
            case YAC_TYPE_STRING: 
               break; 
         } 
      } 
      break; 
   } 
}
