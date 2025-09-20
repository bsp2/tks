/// PTN_ArrayIniExpr.cpp
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_ConstVal.h"
#include "YAC_IntArray.h"
#include "YAC_FloatArray.h"
#include "YAC_PointerArray.h"
#include "YAC_ValueArray.h"
#include "YAC_StringArray.h"

#include "PTN_ExprArrayEntry.h"
#include "PTN_ArrayIniExpr.h"


PTN_ArrayIniExpr::PTN_ArrayIniExpr(PTN_ExprArrayEntry *_first, sBool _bAlwaysNew) {
   first_entry      = _first;
   num_entries      = 0;
   arrays.any_array = NULL;
   cached_arraytype = ATYPE_UNKNOWN; 
   is_const         = 0;
   b_always_new     = _bAlwaysNew;
}


PTN_ArrayIniExpr::~PTN_ArrayIniExpr() {
   if(NULL != first_entry)
   {
      delete first_entry;
      first_entry = NULL;
   }

   if(NULL != arrays.any_array)
   {
      YAC_DELETE(arrays.any_array);
      arrays.any_array = NULL;
   }
}


sBool PTN_ArrayIniExpr::semanticCheck(void) {
   return (first_entry ? first_entry->semanticCheck() : YAC_TRUE);
}


sBool PTN_ArrayIniExpr::resolveXRef(void) {
   if(NULL != first_entry)  
   { 
      return first_entry->resolveXRef();  
   }
   else  
   { 
      return YAC_TRUE;
   }
}


void PTN_ArrayIniExpr::optimize(void) {
   if(2 != is_const) 
   { 
      if(NULL != first_entry)  
      { 
         first_entry->optimize();  
      }
   } 
   else  
   {
      // Already optimized in isConst()
      is_const = 1;
   }
}


sBool PTN_ArrayIniExpr::isConst(void) { 
   if(!b_always_new)
   {
      sBool r  = YAC_TRUE; 
      sBool r2 = YAC_FALSE; 
      PTN_ExprArrayEntry *c = first_entry; 

      while(r && (NULL != c))
      { 
         r  = r && c->expr->isConst(); 
         r2 = r2 | tks_isobjectconstval(c->expr); 
         c  = c->next; 
      }

      if(r) 
      { 
         if(r2) 
         { 
            optimize(); 
            YAC_Value t;  
            PTN_Env env; env.initDefault();
            eval(&env, &t, 0); 
            is_const = 2; // set optimized flag as a hint for subsequent optimize() call
         } 
         else  
         {
            return YAC_TRUE;
         }
      } 
   }

   return YAC_FALSE;  
} 


void PTN_ArrayIniExpr::evalConst(YAC_Value *_r) { 
   // xxx TKS_MT: is evalConst really only called during initialization? (temp scripts!)
   PTN_Env env; env.initDefault();
   eval(&env, _r, 1); 
   arrays.any_array = NULL;
} 


static void PTN_ArrayIniExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   PTN_ArrayIniExpr *st = (PTN_ArrayIniExpr*)_st; // xxx discard const qualifier
   Dtracest;
   st->eval(_env, _r, 0); // (todo) optimize me
} 


void PTN_ArrayIniExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_ArrayIniExpr__eval(_env, _r, this); 
} 


Feval PTN_ArrayIniExpr::getEval(void) const {
   return PTN_ArrayIniExpr__eval; 
} 


void PTN_ArrayIniExpr::eval(PTN_Env *_env, YAC_Value *_r, sBool _const) {
   if(!is_const) 
   {
      if(NULL == first_entry)
      {
         // Empty array
         // xxx use YAC_ValueArray
         cached_arraytype=ATYPE_POINTER; 
         arrays.pointer_array = (YAC_PointerArray*)YAC_NEW_CORE_POOLED(YAC_CLID_POINTERARRAY);
         arrays.pointer_array->alloc(num_entries);
         _r->initObject(arrays.any_array, _const);
         return;
      } 
      
      if(ATYPE_UNKNOWN == cached_arraytype)
      {
         /** evaluate entries and determine array type **/  
         PTN_ExprArrayEntry *en = first_entry; 

         while(NULL != en)
         {
            sU8 at = (sU8) en->expr->getType();

            if(ATYPE_UNKNOWN == cached_arraytype)
            {
               cached_arraytype = at;
            }
            else if(at != cached_arraytype) // mixed array ?
            {
               if( (ATYPE_INT == cached_arraytype) && (ATYPE_FLOAT == at) )
               {
                  cached_arraytype = ATYPE_FLOAT;
               }
               else if( (ATYPE_FLOAT != cached_arraytype) || (ATYPE_INT != at) )
               {
                  cached_arraytype = ATYPE_VALUE; 
               }
            }
            
            en = en->next;
         }

         // Create array object
         allocArray();

      }
      else if(b_always_new)
      {
         allocArray();
      }
      /** evaluate other entries **/

      if(NULL != _env)
      {
         _env->context->exception_default_node = this;
      }

      // Iterate and evaluate array element expressions
      PTN_ExprArrayEntry *en = first_entry; 

      sSI ci = 0;
      do
      {
         if(_const)  
         {
            en->expr->evalConst(_r);  
         }
         else  
         {
            en->expr->eval(_env, _r);  
         }

         if(NULL != _env)
         {
            if(Dhaveexception)
            {
               _r->initVoid();
               _env->context->exception_default_node = NULL;
               Dhandleexception(this);
               return;
            }
         }

         if(ATYPE_VALUE != cached_arraytype)
         {
            _r->typecast(cached_arraytype);
         }
         
         arrays.any_array->yacArraySet((void*)_env->context, ci, _r);
         _r->unset();

         en = en->next;
      }
      while(++ci < num_entries); 

      if(NULL != _env)
      {
         _env->context->exception_default_node = NULL;
      }
   }

   if(b_always_new)
   {
      _r->initObject(arrays.any_array, 1);
      arrays.any_array = NULL;
   }
   else
   {
      _r->initObject(arrays.any_array, _const);
   }
}


void PTN_ArrayIniExpr::allocArray(void) {
   switch(cached_arraytype)
   {
   default: 
   case ATYPE_VALUE:
      cached_arraytype = ATYPE_VALUE;
      arrays.value_array = (YAC_ValueArray*)YAC_NEW_CORE_POOLED(YAC_CLID_VALUEARRAY);
      arrays.value_array->alloc(num_entries);
      break;

   case ATYPE_INT: // int
      cached_arraytype = ATYPE_INT;
      arrays.int_array = (YAC_IntArray*)YAC_NEW_CORE_POOLED(YAC_CLID_INTARRAY);
      arrays.int_array->alloc(num_entries);
      break;

   case ATYPE_FLOAT: // float
      cached_arraytype   = ATYPE_FLOAT;
      arrays.float_array = (YAC_FloatArray*)YAC_NEW_CORE_POOLED(YAC_CLID_FLOATARRAY);
      arrays.float_array->alloc(num_entries);
      break;

   case ATYPE_POINTER: // object
      cached_arraytype     = ATYPE_POINTER;
      arrays.pointer_array = (YAC_PointerArray*)YAC_NEW_CORE_POOLED(YAC_CLID_POINTERARRAY);
      arrays.pointer_array->alloc(num_entries);
      break;

   case ATYPE_STRING: // string
      cached_arraytype     = ATYPE_STRING;
      arrays.string_array = (YAC_StringArray*)YAC_NEW_CORE_POOLED(YAC_CLID_STRINGARRAY);
      arrays.string_array->alloc(num_entries);
      break;
   }
}
