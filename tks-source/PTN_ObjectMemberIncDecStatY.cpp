/// PTN_ObjectMemberIncDecStatY.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Statement.h"
#include "PTN_Expr.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_ObjectMemberIncDecStatY.h"


PTN_ObjectMemberIncDecStatY::PTN_ObjectMemberIncDecStatY(void) { 
   var         = NULL; 
   get_command = NULL; 
   set_command = NULL; 
   dec         = 0; 
   clid        = (sUI)-1;  
   b_indirect  = YAC_FALSE;
} 

PTN_ObjectMemberIncDecStatY::PTN_ObjectMemberIncDecStatY( 
                                                         TKS_CachedObject   * _co,  
                                                         const YAC_CommandY * _getcmd,  
                                                         const YAC_CommandY * _setcmd,  
                                                         sBool                _dec,  
                                                         sUI                  _clid 
                                                         )  
{ 
   var         = _co; 
   get_command = _getcmd; 
   set_command = _setcmd; 
   dec         = _dec; 
   clid        = _clid;  
   b_indirect  = YAC_FALSE;
} 

PTN_ObjectMemberIncDecStatY::~PTN_ObjectMemberIncDecStatY() { 
   var         = NULL; 
   get_command = NULL; 
   set_command = NULL; 
   dec         = 0; 
} 

sBool PTN_ObjectMemberIncDecStatY::semanticCheck(void) { 
   return 
      (b_indirect || (var != NULL)) && 
      (get_command != NULL) && 
      (set_command != NULL)
      ; 
} 

static void PTN_ObjectMemberIncDecStatY__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_ObjectMemberIncDecStatY*st = (const PTN_ObjectMemberIncDecStatY*)_st; 

   YAC_Object *robj;
   if(st->var)
   {
      robj = Dgetvar(st->var)->value.object_val;
   }
   else if(st->b_indirect)
   {
      robj = _env->context->tksvm_indirect_object;
   }
   else
   {
      // xxx use _env->context->tksvm_class_stack_object ?? (YAC_Object currently has no inc/dec-able fields, though)
      robj = NULL;
   }
   
#ifdef DX_SAFEMODE
   if(!TKS_VALID(robj)) 
   { 
      Drtthrowinvalidpointer(_st, "cannot inc/dec member of invalid c++ object", robj);
      return; 
   } 
   if(!tkscript->cpp_typecast_map[robj->class_ID][st->clid]) 
   { 
      Drtthrow(_st, TKS_EXCEPTION_CLASSTYPEMISMATCH, "cannot inc/dec member of incompatible c++ object");
      return; 
   } 
#endif // DX_SAFEMODE

   // Evaluate getter method
   void *rany = (void*) robj;

   YAC_Value cret; 
   switch(st->get_command->y_callstyle)
   {
   default:
      Drtthrow(_st, TKS_EXCEPTION_TYPEMISMATCH, "getter method uses unsupported callstyle");
      return;
      
   case 2: // int return
      cret.value.int_val = ((sSI(YAC_CALL*)(void*))st->get_command->y_adr)(rany/*this*/); 
      cret.type = YAC_TYPE_INT;
      break;

   case 4: // float return
      cret.value.float_val = ((sF32(YAC_CALL*)(void*))st->get_command->y_adr)(rany/*this*/); 
      cret.type = YAC_TYPE_FLOAT;
      break;

   case 6: // Object return
      cret.value.object_val = ((YAC_Object*(YAC_CALL*)(void*))st->get_command->y_adr)(rany/*this*/); 
      cret.type = YAC_TYPE_OBJECT;
      break;

   case 8: // var return
      ((void(YAC_CALL*)(void*, YAC_Value*))st->get_command->y_adr)(rany/*this*/, &cret); 
      break;
   }

   Dfixfpuflags;
   
   if(st->dec) 
   { 
      switch(cret.type) 
      { 
      case YAC_TYPE_INT: 
         cret.value.int_val--; 
         break; 
      case YAC_TYPE_FLOAT: 
         cret.value.float_val-=1.0f; 
         break; 
      case YAC_TYPE_OBJECT: 
      case YAC_TYPE_STRING: 
         if(YAC_VALID(cret.value.object_val))
         {
            YAC_Value r;
            cret.value.object_val->yacOperatorI(YAC_OP_SUB, 1, &r);
            r.unsetFast();
         }
         else
         {
            Drtthrow(_st, TKS_EXCEPTION_INVALIDPOINTER, "cannot dec invalid member object");
            cret.unsetFast(); 
         }
         // in-place modification of (number?) object. no need to set method afterwards.
         return; 
      } 
   } 
   else 
   { 
      switch(cret.type) 
      { 
      case YAC_TYPE_INT: 
         cret.value.int_val++; 
         break; 
      case YAC_TYPE_FLOAT: 
         cret.value.float_val+=1.0f; 
         break; 
      case YAC_TYPE_OBJECT: 
      case YAC_TYPE_STRING: 
         if(YAC_VALID(cret.value.object_val))
         {
            YAC_Value r;
            cret.value.object_val->yacOperatorI(YAC_OP_ADD, 1, &r);
            r.unsetFast();
         }
         else
         {
            Drtthrow(_st, TKS_EXCEPTION_INVALIDPOINTER, "cannot dec invalid member object");
            cret.unsetFast(); 
         }
         // in-place modification of (number?) object. no need to set method afterwards.
         return; 
      } 
   } 

   // Evaluate setter method
   yacmemptr         current_call_memptr;
   yacmem            call_values[1];
   current_call_memptr.mem = call_values; 
   call_values[0].any = cret.value.any; 
   switch(st->set_command->y_callstyle)
   {
   default:
      Drtthrow(_st, TKS_EXCEPTION_TYPEMISMATCH, "setter method uses unsupported callstyle");
      return;

   case 1: // args, no return
      ((void(YAC_CALL*)(void*, yacmemptr))st->set_command->y_adr)(rany, current_call_memptr); 
      break;

   case 3: // args+int return
      ((sSI(YAC_CALL*)(void*, yacmemptr))st->set_command->y_adr)(rany, current_call_memptr); 
      break;

   case 5: // args+float return
      ((sF32(YAC_CALL*)(void*, yacmemptr))st->set_command->y_adr)(rany, current_call_memptr); 
      break;

   case 7: // args+Object return
      ((YAC_Object*(YAC_CALL*)(void*, yacmemptr))st->set_command->y_adr)(rany, current_call_memptr); 
      break;

   case 9: // args+var return
      {
         YAC_Value r;
         ((YAC_Object*(YAC_CALL*)(void*, yacmemptr, YAC_Value*))st->set_command->y_adr)(rany, current_call_memptr, &r); 
         r.unsetFast();
      }
      break;
   }

   Dfixfpuflags;
   
} 

Fevalst PTN_ObjectMemberIncDecStatY::getEvalSt(void) const { 
   return PTN_ObjectMemberIncDecStatY__eval; 
} 

void PTN_ObjectMemberIncDecStatY::eval(PTN_Env *_env) const { 
   PTN_ObjectMemberIncDecStatY__eval(_env, this); 
} 
