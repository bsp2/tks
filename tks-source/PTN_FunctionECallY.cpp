/// PTN_FunctionECallY.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>.
///     - distributed under terms of the GNU General Public License (GPL).
///
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_CallableExpr.h"
#include "PTN_ArgListNode.h"
#include "PTN_ArgList.h"
#include "PTN_FunctionECall.h"
#include "PTN_FunctionECallY.h"

#include "tks_inc_class.h"
#include "tks_inc_compiler.h"


// ---- This node is used to call YAC-style "C" plugin functions (also see YInG, the YAC interface generator)
PTN_FunctionECallY::PTN_FunctionECallY(void) {
   command         = NULL;
   arglist         = NULL;
   retobj_expr     = NULL;
   retobj_expr_opt = NULL;
   b_ret_retobj    = YAC_FALSE;
}

PTN_FunctionECallY::PTN_FunctionECallY(YAC_CommandY *_fun, PTN_ArgList *_args) {
   command         = _fun;
   arglist         = _args;
   retobj_expr     = NULL;
   retobj_expr_opt = NULL;
   b_ret_retobj    = YAC_FALSE;
}

PTN_FunctionECallY::~PTN_FunctionECallY() {
   TKS_DELETE_SAFE(arglist);
   TKS_DELETE_SAFE(retobj_expr);
   command = NULL;
}

sBool PTN_FunctionECallY::handleRootCommand(void) {
   return CheckRootCommand((NULL != retobj_expr), &command, 0, NULL);
}

sBool PTN_FunctionECallY::CheckRootCommand(sBool _bHaveRetObjExpr,
                                           const YAC_CommandY**_cmdAddr,
                                           sBool _bRSELF, sBool *_bRSELF_emulRet
                                           ) {
   const YAC_CommandY *command = *_cmdAddr;

   //
   // Called after node has been initialized (retobj_expr, b_ret_retobj)
   //
   if(_bRSELF)
   {
      if(NULL == command->y_adr)
      {
         if(NULL != command->y_retself_cmd)
         {
            // Use RSELF variant (used only for method calls!)
            *_cmdAddr = command->y_retself_cmd;
            *_bRSELF_emulRet = 0;
            return 1;
         }
         else if(NULL != command->y_retarg_cmd)
         {
            // Fallback to RARG, generate synthetic argument in method call
            *_cmdAddr = command->y_retarg_cmd;
            *_bRSELF_emulRet = 1;
            return 1;
         }
         else
         {
            Dprintf("[---] missing RSELF/RARG variant for method \"%s\".\n", command->y_name);
            return 0;
         }
      }
      else
      {
         Dprintf("[---] =>* or <=>* used but method \"%s\" does not support either RSELF or RARG.\n", command->y_name);
         return 0;
      }
   }
   else if(_bHaveRetObjExpr)
   {
      if(NULL == command->y_adr)
      {
         if(NULL != command->y_retarg_cmd)
         {
            // Use RARG variant
            *_cmdAddr = command->y_retarg_cmd;
            return 1;
         }
         else
         {
            Dprintf("[---] missing RARG variant for function/method \"%s\".\n", command->y_name);
            return 0;
         }
      }
      else
      {
         Dprintf("[---] return object expr. used but function/method \"%s\" does not support it.\n", command->y_name);
         return 0;
      }
   }
   else
   {
      if(NULL == command->y_adr)
      {
         if(NULL != command->y_retval_cmd)
         {
            // Use RVAL variant
            *_cmdAddr = command->y_retval_cmd;
            return 1;
         }
         else
         {
            Dprintf("[---] missing RVAL variant for function/method \"%s\".\n", command->y_name);
            return 0;
         }
      }
      else
      {
         // Regular function/method call
         return 1;
      }
   }
}



sBool PTN_FunctionECallY::semanticCheck(void) {

   // b_command may have changed
   handleRootCommand();


   if(command)
   {
      if(retobj_expr)
      {
         if(!retobj_expr->semanticCheck())
         {
#ifdef TKS_DCON
            tkscript->printf("\n[---] <retobj_expr> semantic check failed in call to C function \"%s\"\n",
                             command->y_name);
            tkscript->printf("[---] in module \"%s\", line %i.\n",
                             Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                             Dsrcloc_linenr(src_loc)
                             );
#endif
            return YAC_FALSE;
         }
      }
      if(arglist)
      {
         if(command->y_arg_num != arglist->num_args)
         {
#ifdef TKS_DCON
            tkscript->printf("\n[---] wrong number of arguments to C function \"%s\" (have=%i need=%i)\n",
                             command->y_name, arglist->num_args, command->y_arg_num
                             );
            tkscript->printf("[---] in module \"%s\", line %i.\n",
                             Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                             Dsrcloc_linenr(src_loc)
                             );
#endif
            return YAC_FALSE;
         }
         return arglist->semanticCheck();
      }
      else
      {
         if(command->y_arg_num)
         {
#ifdef TKS_DCON
            tkscript->printf("\n[---] missing arguments for call to C function \"%s\" (need=%i)\n",
                             command->y_name, command->y_arg_num
                             );
            tkscript->printf("[---] in module \"%s\", line %i.\n",
                             Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                             Dsrcloc_linenr(src_loc)
                             );
#endif
            return YAC_FALSE;
         }
      }
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

static void PTN_FunctionECallY__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   const PTN_FunctionECallY *st = (const PTN_FunctionECallY *)_st;

#ifdef DX_TRACEFUNCTIONCALLS
   _env->context->pushFunctionCallY(0, st->command, st->src_loc);
#endif

   Dcondbegincallsiteprofile;

   // ---- eval call, eval arguments first ----
   YAC_Value args[TKS_MAX_ARGS]; // allocate args on stack to be reentrant
   yacmemptr current_call_memptr;
   yacmem call_values[TKS_MAX_ARGS];
   current_call_memptr.mem = call_values;

   sUI numargs;
   sU8 i;
   YAC_Value retObjVal;

   i = 0;

   // Evaluate argument expressions
	if(st->arglist != NULL)
	{
		numargs = st->arglist->num_args;
		PTN_ArgListNode *c = st->arglist->first_arg;
		for(; i<numargs; i++)
		{
#ifdef TKS_MT
			c->expr->evalTC(_env, (sU8)st->command->y_arg_types[i], &args[i]);
			call_values[i].any = args[i].value.any;
#else
			c->expr->evalTC(_env, (sU8)st->command->y_arg_types[i], &c->cv);
			call_values[i].any = c->cv.value.any;
#endif
			c = c->next;
		}
	}
   else
   {
      numargs = 0;
   }

   // Add return object to parameter list ("fun() => ret")
   if(st->retobj_expr_opt)
   {
      st->retobj_expr_opt(_env, &retObjVal, st->retobj_expr);
      retObjVal.typecast(YAC_TYPE_OBJECT);
      call_values[i].any = retObjVal.value.any;
   }

   Dcondbeginfunprofile;


	// ---- now we collected all args ----
	switch(st->command->y_callstyle)
	{
	case 0: // ---- wrapped args, no return
		((void(YAC_CALL*)(yacmemptr))st->command->y_adr)(current_call_memptr);
		break;
	case 1: // ---- wrapped args+int return
		_r->value.int_val=((sSI(YAC_CALL*)(yacmemptr))st->command->y_adr)(current_call_memptr);
		_r->type = YAC_TYPE_INT;
		break;
	case 2: // ---- wrapped args+ float return
		_r->value.float_val= ((sF32(YAC_CALL*)(yacmemptr))st->command->y_adr)(current_call_memptr);
		_r->type = YAC_TYPE_FLOAT;
		break;
	case 3: // ---- wrapped args+object return
		_r->value.object_val=(YAC_Object*)((void*(YAC_CALL*)(yacmemptr))st->command->y_adr)(current_call_memptr);
		_r->type = YAC_TYPE_OBJECT;
		_r->deleteme = 0;
		Ddubiousfixstringtype(_r);
		break;
	case 4: // ---- wrapped args+var return
		((void(YAC_CALL*)(yacmemptr, YAC_Value*))st->command->y_adr)(current_call_memptr, _r);
		Ddubiousfixstringtype(_r);
		break;
	case 5: // ---- no args, var return
		((void(YAC_CALL*)(YAC_Value*))st->command->y_adr)(_r);
		Ddubiousfixstringtype(_r);
		break;

	case 8:  // ---- void, no args
		((void(YAC_CALL*)(void))st->command->y_adr)();
		break;
	case 9: // ---- int return, no args
		_r->value.int_val=((sSI(YAC_CALL*)(void))st->command->y_adr)();
      _r->type = YAC_TYPE_INT;
      break;
	case 10: // ---- float return, no args
		_r->value.float_val = ((sF32(YAC_CALL*)(void))st->command->y_adr)();
      _r->type = YAC_TYPE_FLOAT;
      break;
	case 11: // ---- object return, no args
		_r->value.object_val = (YAC_Object*)((void*(YAC_CALL*)(void))st->command->y_adr)();
      _r->type = YAC_TYPE_OBJECT;
      _r->deleteme = 0;
      Ddubiousfixstringtype(_r);
      break;

#ifdef TKS_ALLOWINTFLOATMIXUP
      // Note: on 64bit systems we need to separate between int/float (32bit) and pointer (64bit) arguments
#error "TKS_ALLOWINTFLOATMIXUP currently broken, may be removed in future releases anyway"
#include "autogen_PTN_FunctionECallY_switch.cpp"
#endif // TKS_ALLOWINTFLOATMIXUP
	}

   Dfixfpuflags;

   Dcondendfunprofile((YAC_CommandY*) st->command);

   // Unset argument values
#ifdef TKS_MT
   for(i=0; i<numargs; i++)
   {
      args[i].unset();
   }
#else
	if(st->arglist)
	{
		PTN_ArgListNode *c = st->arglist->first_arg;
		while(c)
		{
			c->cv.unset();
			c=c->next;
		}
	}
#endif // TKS_MT

   if(st->b_ret_retobj)
   {
      // In case of "fun() <=> GetNewObj()"
      _r->copySafe(&retObjVal);
   }
   else
   {
      // Usually the retobj is not deletable but it *may* be
      retObjVal.unsetFast();
   }

   Dcondendcallsiteprofile(st->src_loc);

#ifdef DX_TRACEFUNCTIONCALLS
   _env->context->popFunctionCallY();
#endif

   // Discard any uncritical error that is not caught by the caller frame
   _env->context->discardUncriticalError();
}

Feval PTN_FunctionECallY::getEval(void) const {
   return PTN_FunctionECallY__eval;
}

void PTN_FunctionECallY::evalCallableExpr(PTN_Env *_env, YAC_Value *_r) const {
   return PTN_FunctionECallY__eval(_env, _r, this);
}

void PTN_FunctionECallY::eval(PTN_Env *_env, YAC_Value *_r) const {
   return PTN_FunctionECallY__eval(_env, _r, this);
}

void PTN_FunctionECallY::optimize(void) {
   if(retobj_expr)
   {
      tks_optimize_expr(&retobj_expr, 0);
      retobj_expr_opt = retobj_expr->getEval();
   }
	if(arglist)
	{
		////----arglist->optimize();
		PTN_ArgListNode *c=arglist->first_arg;
		sUI i=0;
		while(c)
		{
			tks_optimize_expr(&c->expr, (sU8) command->y_arg_types[i++]);
			c->expr_opt=c->expr->getEval(); // xxx unused
			c=c->next;
		}
	}
}

sBool PTN_FunctionECallY::resolveXRef(void) {
   return
      (retobj_expr?retobj_expr->resolveXRef():1) &&
      (arglist?arglist->resolveXRef():1);
}

#ifdef TKS_JIT
sBool PTN_FunctionECallY::forceHybrid(void) {
   return
      (command->y_return_type==4) || // Variable return type ?
      (retobj_expr?(retobj_expr->forceHybrid() || (retobj_expr->getType() < YAC_TYPE_OBJECT)):0) ||
      (arglist?arglist->forceHybrid():0)
      ;
}

sU8 PTN_FunctionECallY::compile(VMCore *_vm) {
	return compileCallStatement(_vm);
}

sU8 PTN_FunctionECallY::compileCallStatement(VMCore *_vm) {

   if(NULL != retobj_expr)
   {
      // Compile returnobject parameter function call (=>, <=>)

      // Compile last argument (return object!)
      retobj_expr->compile(_vm);

      if(NULL != arglist)
      {
         arglist->compileY(command, _vm);
      }

      if(command->y_callstyle < 5) // use wrapped args ?
      {
         _vm->addOpcode(VMOP_PUSHS);
      }

      _vm->addCCallY(command, 1/*one return object*/);

      if(!b_ret_retobj)
      {
         // Clean up retobject parameter
         _vm->addOpcode(VMOP_INCSTP);
         return 0;
      }
      else
      {
         // retobj parameter is left on stack as return value
         return 3;
      }
   }
   else
   {
      // Compile regular function call

      if(0 != command->y_return_type)
      {
         // Reserve stack space for return value
         _vm->addOpcode(VMOP_DECSTP);
      }

      if(NULL != arglist)
      {
         arglist->compileY(command, _vm);
      }

      if(command->y_callstyle < 5) // use wrapped args
      {
         _vm->addOpcode(VMOP_PUSHS);
      }

      _vm->addCCallY(command, 0/*no return object*/);

      return command->y_return_type;
   }
}

#endif // TKS_JIT
