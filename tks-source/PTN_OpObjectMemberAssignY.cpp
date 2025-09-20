/// PTN_OpObjectMemberAssignY.cpp
///
/// (c) 2003-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Statement.h"
#include "PTN_Expr.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_OpObjectMemberAssignY.h"
#include "PTN_DoubleArgExpr.h"


PTN_OpObjectMemberAssignY::PTN_OpObjectMemberAssignY(TKS_CachedObject   * _var,  
                                                     const YAC_CommandY * _get_command,  
                                                     const YAC_CommandY * _set_command,  
                                                     sU8                  _assign_operator,  
                                                     PTN_Expr           * _expr,  
                                                     sUI                  _clid 
                                                     )  
{ 
   var             = _var; 
   get_command     = _get_command; 
   set_command     = _set_command; 
   assign_operator = _assign_operator; 
   expr            = _expr; 
   expr_opt        = expr->getEval(); 
   clid            = _clid;
   b_indirect      = YAC_FALSE;
} 

PTN_OpObjectMemberAssignY::~PTN_OpObjectMemberAssignY() {
   TKS_DELETE_SAFE(expr);
} 

sBool PTN_OpObjectMemberAssignY::semanticCheck(void) { 
   return 
      (set_command!=NULL)                  && 
      (b_indirect || (NULL != var))        &&
      (expr?expr->semanticCheck():0)       && 
      (assign_operator?(get_command!=0):1)
      ;
} 

sBool PTN_OpObjectMemberAssignY::resolveXRef(void) { 
   return expr->resolveXRef();
} 

void PTN_OpObjectMemberAssignY::optimize(void) { 
   tks_optimize_expr(&expr, (sU8) set_command->y_arg_types[0]); 
   expr_opt = expr->getEval(); 
} 

static void PTN_OpObjectMemberAssignY__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_OpObjectMemberAssignY *st = (const PTN_OpObjectMemberAssignY*) _st; 

   YAC_Object *robj = (st->b_indirect ? _env->context->tksvm_indirect_object : Dgetvar(st->var)->value.object_val);
   
   if(TKS_VALID(robj))
   { 
#ifdef DX_SAFEMODE
      if(!tkscript->cpp_typecast_map[robj->class_ID][st->clid]) 
      { 
         Drtthrow(_st, TKS_EXCEPTION_CLASSTYPEMISMATCH, "cannot assign to member of incompatible c++ object");
         return; 
      } 
#endif // DX_SAFEMODE
      YAC_Value r; 
      st->expr_opt(_env, &r, st->expr); 
      if(Dhaveexception)
      {
         r.unsetFast();
         Dhandleexception(st->expr);
         return;
      }

      void *rany = (void*) robj;

      if(st->get_command)  
      { 
         YAC_Value cret; 
         // ---- call get command and write result to return_value ---- 

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
            Ddubiousfixstringtype(&cret);
            break;
            
         case 8: // var return
            ((void(YAC_CALL*)(void*, YAC_Value*))st->get_command->y_adr)(rany/*this*/, &cret); 
            Ddubiousfixstringtype(&cret);
            break;
         }

         // ---- r = returnval + r ---- 
         YAC_Value r2; 
         PTN_DoubleArgExpr::EvalOp(_env, &r2, &cret, &r, st->assign_operator, _st); 
         // ---- r= assignment unsets previous r=expr->eval() ---- 
         // ---- r might contain new object ---- 
         cret.unsetFast();
         r = &r2; 
      }
      
      if(r.type!=st->set_command->y_arg_types[0]) 
      {
         r.typecast(st->set_command->y_arg_types[0]); 
      }

      yacmemptr         current_call_memptr;
      yacmem            call_values[1];
      current_call_memptr.mem = call_values; 
      call_values[0].any = r.value.any; 

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
            YAC_Value rv;
            ((YAC_Object*(YAC_CALL*)(void*, yacmemptr, YAC_Value*))st->set_command->y_adr)(rany, current_call_memptr, &rv); 
            rv.unsetFast();
         }
         break;
      }
      
      r.unsetFast(); 
   } 
   else 
   { 
      Drtthrowinvalidpointer(_st, "cannot assign to member of invalid c++ object", robj);
   } 
} 

Fevalst PTN_OpObjectMemberAssignY::getEvalSt(void) const {
   return PTN_OpObjectMemberAssignY__eval; 
} 

void PTN_OpObjectMemberAssignY::eval(PTN_Env *_env) const { 
   PTN_OpObjectMemberAssignY__eval(_env, this); 
} 



#ifdef TKS_JIT 
sBool PTN_OpObjectMemberAssignY::forceHybrid(void) { 
   return (assign_operator!=YAC_OP_ASSIGN)|expr->forceHybrid(); 
} 

sU8 PTN_OpObjectMemberAssignY::compile(VMCore *_vm) { 
   sU8 r = expr->compile(_vm); 
   _vm->typecastStack(r, set_command->y_arg_types[0]); 
   _vm->addOpcode(VMOP_PUSHS); 
   _vm->pushv(var); 
   _vm->addCPPCallY(set_command, 0/*no return object*/);

   if(set_command->y_return_type)
   {
      // Discard return value
      _vm->addOpcode(VMOP_INCSTP);
   }

   if(next) return next->compileHybridStatement(_vm); 
   else return 0; 
}
#endif
