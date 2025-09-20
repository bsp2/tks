/// PTN_ECallY.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Statement.h"
#include "PTN_Expr.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_ArgListNode.h"
#include "PTN_ArgList.h"
#include "PTN_CallableExpr.h"
#include "PTN_FunctionECallY.h"
#include "PTN_ECallY.h"
#include "tks_inc_compiler.h"
#include "tks_inc_class.h"


PTN_ECallY::PTN_ECallY(void) {
   command         = NULL;
   cached_object   = NULL;
   b_indirect      = YAC_FALSE;
   arglist         = NULL;
   clid            = 0xFFFF;
   retobj_expr     = NULL;
   retobj_expr_opt = NULL;
   b_ret_retobj    = YAC_FALSE;
   b_rself         = YAC_FALSE;
   b_rself_emul    = YAC_FALSE;
   evalCPPCall_opt = NULL;
   b_statement     = YAC_FALSE;
#ifndef TKS_MT
   current_call_memptr.mem = call_values;
#endif
}

PTN_ECallY::PTN_ECallY(const YAC_CommandY *_fun,
                       TKS_CachedObject * _obj,
                       sBool              _bIndirect,
                       PTN_ArgList *      _args,
                       sUI                _clid
                       )
{
   command         = _fun;
   cached_object   = _obj;
   b_indirect      = _bIndirect;
   arglist         = _args;
   clid            = _clid;
   retobj_expr     = NULL;
   retobj_expr_opt = NULL;
   b_ret_retobj    = YAC_FALSE;
   b_rself         = YAC_FALSE;
   b_rself_emul    = YAC_FALSE;
   evalCPPCall_opt = NULL;
   b_statement     = YAC_FALSE;
#ifndef TKS_MT
   current_call_memptr.mem = call_values;
#endif
}

PTN_ECallY::~PTN_ECallY() {
   if(arglist)
   {
      delete arglist;
      arglist = NULL;
   }
   if(retobj_expr)
   {
      delete retobj_expr;
      retobj_expr = NULL;
   }
   command = NULL;
   cached_object = NULL;
}

void PTN_ECallY::setStatementHint(void) {
   b_statement = 1;
}

sBool PTN_ECallY::handleRootCommand(void) {

   // Used as a statement but =>, <=> is not used ?
   if(b_statement && !retobj_expr)
   {
      // Modify self if command is available
      // Example: String s; s.append("hello, world.");
      if(command->y_retself_cmd)
      {
         b_rself = 1;
      }
   }

   if(PTN_FunctionECallY::CheckRootCommand((NULL != retobj_expr), &command, b_rself, &b_rself_emul))
   {
      // TODO: compare retobj_expr to cached_object and collapse to RSELF call if possible

      if(b_rself_emul)
      {
         // Emulate RSELF using RARG
         evalCPPCall_opt = &PTN_ECallY::evalCPPCallRSELF_emul;
      }
      else if(b_rself)
      {
         if(b_ret_retobj)
         {
            // <=>*
            evalCPPCall_opt = &PTN_ECallY::evalCPPCallRSELF;
         }
         else
         {
            // Do not return self => regular method call
            evalCPPCall_opt = &PTN_ECallY::evalCPPCall;
         }
      }
      else if(retobj_expr)
      {
         // => "retobj_expr", <=> "retobj_expr"
         evalCPPCall_opt = &PTN_ECallY::evalCPPCallRARG;
      }
      else
      {
         // Regular method call
         evalCPPCall_opt = &PTN_ECallY::evalCPPCall;
      }
      return 1;
   }
   else
   {
      return 0;
   }
}

sBool PTN_ECallY::semanticCheck(void) {

   // b_command may have changed
   handleRootCommand();

   if(command)
   {
      if(arglist)
      {
         if(command->y_arg_num != arglist->num_args)
         {
#ifdef TKS_DCON
            tkscript->printf("\n[---] wrong number of arguments to C++ method \"%s\" (have=%i need=%i)\n",
                             command->y_name, arglist->num_args, command->y_arg_num
                             );
            tkscript->printf("[---] in module \"%s\", line %i.\n",
                             Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                             Dsrcloc_linenr(src_loc)
                             );
#endif
            return YAC_FALSE;
         }
         if(!arglist->semanticCheck())
         {
            return YAC_FALSE;
         }
      }
      else
      {
         if(command->y_arg_num)
         {
#ifdef TKS_DCON
            tkscript->printf("\n[---] missing arguments for call to C++ method \"%s\" (need=%i)\n",
                             command->y_name, command->y_arg_num);
            tkscript->printf("[---] in module \"%s\", line %i.\n",
                             Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                             Dsrcloc_linenr(src_loc)
                             );
#endif
            return 0;
         }
      }

      // if cached_object is NULL use current scriptclass "this" object or indirect object (if b_indirect is set)
      if(cached_object)
      {
         if(retobj_expr)
         {
            if(b_rself)
            {
               Dprintf("\n[---] internal error: b_rself and retobj_expr used simultaneously.\n");
               return 0;
            }

            if(!retobj_expr->semanticCheck())
            {
#ifdef TKS_DCON
               tkscript->printf("\n[---] <retobj_expr> semantic check failed in call to C++ method \"%s\"\n",
                                command->y_name);
               tkscript->printf("[---] in module \"%s\", line %i.\n",
                                Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                                Dsrcloc_linenr(src_loc)
                                );
#endif
               return 0;
            }
         }
      }
      return 1;
   }
   return 0;
}

void PTN_ECallY::EvalCPPCall(YAC_Object*_o,
                             const YAC_CommandY*command,
                             PTN_ArgList*_args, yacmemptr current_call_memptr,
                             PTN_Env *_env, YAC_Value *_r
                             )
{
	PTN_ArgListNode *c;
   sU8 i;
   sU8 numargs;
#ifdef TKS_MT
   YAC_Value args[TKS_MAX_ARGS];
#endif
	if(_args)
	{
		numargs = (sU8) _args->num_args;
		c=_args->first_arg;
#ifdef TKS_MT
		for(i=0; i<numargs; i++)
		{
			c->expr->evalTC(_env, (sU8) command->y_arg_types[i], &args[i]);
			current_call_memptr.mem[i].any = args[i].value.any;
			c=c->next;
		}
#else
		for(i=0; i<numargs; i++)
		{
			c->expr->evalTC(_env, command->y_arg_types[i], &c->cv);
			current_call_memptr.mem[i].any = c->cv.value.any;
			c=c->next;
		}
#endif
	}
   else
   {
      numargs = 0;
   }

   Dcondbeginfunprofile;

	// ---- now we collected all args ----
	switch(command->y_callstyle)
	{
	case 0:
		((void(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		break;
	case 1: // ---- args, no return
		((void(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		break;
	case 2: // ---- int return
		_r->value.int_val=((sSI(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_INT;
		break;
	case 3: // ---- args+int return
		_r->value.int_val=((sSI(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_INT;
		break;
	case 4: // ---- float return
		_r->value.float_val= ((sF32(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_FLOAT;
		break;
	case 5: // ---- args+ float return
		_r->value.float_val= ((sF32(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_FLOAT;
		break;
	case 6: // ---- object return
		_r->value.object_val=(YAC_Object*)((void*(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_OBJECT;
		_r->deleteme = 0;
		break;
	case 7: // ---- args+object return
		_r->value.object_val=(YAC_Object*)((void*(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_OBJECT;
		_r->deleteme = 0;
		break;
	case 8: // ---- var return
		((void(YAC_CALL*)(void*, YAC_Value*))command->y_adr)((void*)_o, _r);
		break;
	case 9: // ---- args+var return
		((void(YAC_CALL*)(void*, yacmemptr, YAC_Value*))command->y_adr)((void*)_o, current_call_memptr, _r);
		break;
	}

   Dfixfpuflags;

   Dcondendfunprofile((YAC_CommandY*)command);

#ifdef TKS_MT
	for(i=0; i<numargs; i++)
	{
		args[i].unset();
	}
#else
	if(_args)
	{
		c=_args->first_arg;
		while(c)
		{
			c->cv.unset();
			c=c->next;
		}
	}
#endif // TKS_MT

   // Discard any uncritical error that is not caught by the caller frame
   _env->context->discardUncriticalError();
}


void PTN_ECallY::evalCPPCallRARG(YAC_Object *_o, PTN_Env *_env, YAC_Value *_r) const {
   //
   // Call C++ Method that stores its return value in the last (Object) parameter
   //
	PTN_ArgListNode *c;
   sUI i;
   sUI numargs;
#ifdef TKS_MT
   YAC_Value args[16];
#endif

#ifdef TKS_MT
   yacmemptr current_call_memptr;
   yacmem linargs[TKS_MAX_ARGS];
   current_call_memptr.mem = linargs;
#endif

   YAC_Value retObjVal;

   i = 0;

	if(arglist)
	{
		numargs = (sUI) arglist->num_args;
		c = arglist->first_arg;
#ifdef TKS_MT
		for(; i<numargs; i++)
		{
			c->expr->evalTC(_env, (sU8) command->y_arg_types[i], &args[i]);
			current_call_memptr.mem[i].any = args[i].value.any;
			c=c->next;
		}
#else
		for(i=0; i<numargs; i++)
		{
			c->expr->evalTC(_env, command->y_arg_types[i], &c->cv);
			current_call_memptr.mem[i].any = c->cv.value.any;
			c=c->next;
		}
#endif
	}
   else
   {
      numargs = 0;
   }

   // Add return object to parameter list
   retobj_expr_opt(_env, &retObjVal, retobj_expr);
   retObjVal.typecast(YAC_TYPE_OBJECT);
   current_call_memptr.mem[i].any = retObjVal.value.any;

	// ---- now we collected all args ----
	switch(command->y_callstyle)
	{
	case 0:
		((void(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		break;
	case 1: // ---- args, no return
		((void(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		break;
	case 2: // ---- int return
		_r->value.int_val=((sSI(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_INT;
		break;
	case 3: // ---- args+int return
		_r->value.int_val=((sSI(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_INT;
		break;
	case 4: // ---- float return
		_r->value.float_val= ((sF32(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_FLOAT;
		break;
	case 5: // ---- args+ float return
		_r->value.float_val= ((sF32(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_FLOAT;
		break;
	case 6: // ---- object return
		_r->value.object_val=(YAC_Object*)((void*(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_OBJECT;
		_r->deleteme = 0;
		break;
	case 7: // ---- args+object return
		_r->value.object_val=(YAC_Object*)((void*(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_OBJECT;
		_r->deleteme = 0;
		break;
	case 8: // ---- var return
		((void(YAC_CALL*)(void*, YAC_Value*))command->y_adr)((void*)_o, _r);
		break;
	case 9: // ---- args+var return
		((void(YAC_CALL*)(void*, yacmemptr, YAC_Value*))command->y_adr)((void*)_o, current_call_memptr, _r);
		break;
	}
#ifdef TKS_MT
	for(i=0; i<numargs; i++)
	{
		args[i].unset();
	}
#else
	if(arglist)
	{
		c = arglist->first_arg;
		while(c)
		{
			c->cv.unset();
			c=c->next;
		}
	}
#endif // TKS_MT

   if(b_ret_retobj)
   {
      // In case of "fun() <=> GetNewObj()"
      _r->copySafe(&retObjVal);
   }
   else
   {
      // Usually the retobj is not deletable but it *may* be
      retObjVal.unsetFast();
   }

   // Discard any uncritical error that is not caught by the caller frame
   _env->context->discardUncriticalError();
}


void PTN_ECallY::evalCPPCallRSELF(YAC_Object *_o, PTN_Env *_env, YAC_Value *_r) const {
   //
   // Call C++ Method that modifies "this" and returns "this"
   //
	PTN_ArgListNode *c;
   sUI i;
   sUI numargs;
#ifdef TKS_MT
   YAC_Value args[TKS_MAX_ARGS];
#endif

#ifdef TKS_MT
   yacmemptr current_call_memptr;
   yacmem linargs[TKS_MAX_ARGS];
   current_call_memptr.mem = linargs;
#endif

   i = 0;

	if(arglist)
	{
		numargs = (sUI) arglist->num_args;
		c = arglist->first_arg;
#ifdef TKS_MT
		for(; i<numargs; i++)
		{
			c->expr->evalTC(_env, (sU8) command->y_arg_types[i], &args[i]);
			current_call_memptr.mem[i].any = args[i].value.any;
			c=c->next;
		}
#else
		for(i=0; i<numargs; i++)
		{
			c->expr->evalTC(_env, command->y_arg_types[i], &c->cv);
			current_call_memptr.mem[i].any = c->cv.value.any;
			c=c->next;
		}
#endif
	}
   else
   {
      numargs = 0;
   }

   // ---- now we collected all args ----
	switch(command->y_callstyle)
	{
	case 0:
		((void(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		break;
	case 1: // ---- args, no return
		((void(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		break;
	case 2: // ---- int return
		_r->value.int_val=((sSI(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_INT;
		break;
	case 3: // ---- args+int return
		_r->value.int_val=((sSI(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_INT;
		break;
	case 4: // ---- float return
		_r->value.float_val= ((sF32(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_FLOAT;
		break;
	case 5: // ---- args+ float return
		_r->value.float_val= ((sF32(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_FLOAT;
		break;
	case 6: // ---- object return
		_r->value.object_val=(YAC_Object*)((void*(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_OBJECT;
		_r->deleteme = 0;
		break;
	case 7: // ---- args+object return
		_r->value.object_val=(YAC_Object*)((void*(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_OBJECT;
		_r->deleteme = 0;
		break;
	case 8: // ---- var return
		((void(YAC_CALL*)(void*, YAC_Value*))command->y_adr)((void*)_o, _r);
		break;
	case 9: // ---- args+var return
		((void(YAC_CALL*)(void*, yacmemptr, YAC_Value*))command->y_adr)((void*)_o, current_call_memptr, _r);
		break;
	}
#ifdef TKS_MT
	for(i=0; i<numargs; i++)
	{
		args[i].unset();
	}
#else
	if(arglist)
	{
		c = arglist->first_arg;
		while(c)
		{
			c->cv.unset();
			c=c->next;
		}
	}
#endif // TKS_MT

   // Return "this"
   _r->safeInitObject(_o, 0);

   // Discard any uncritical error that is not caught by the caller frame
   _env->context->discardUncriticalError();
}


void PTN_ECallY::evalCPPCallRSELF_emul(YAC_Object *_o, PTN_Env *_env, YAC_Value *_r) const {
   //
   // Call C++ method that modifies "this" but has no dedicated RSELF method. (a fallback to RARG is used instead)
   //
	PTN_ArgListNode *c;
   sUI i;
   sUI numargs;
#ifdef TKS_MT
   YAC_Value args[TKS_MAX_ARGS];
#endif

#ifdef TKS_MT
   yacmemptr current_call_memptr;
   yacmem linargs[TKS_MAX_ARGS];
   current_call_memptr.mem = linargs;
#endif

   i = 0;

	if(arglist)
	{
		numargs = (sUI) arglist->num_args;
		c = arglist->first_arg;
#ifdef TKS_MT
		for(; i<numargs; i++)
		{
			c->expr->evalTC(_env, (sU8) command->y_arg_types[i], &args[i]);
			current_call_memptr.mem[i].any = args[i].value.any;
			c=c->next;
		}
#else
		for(i=0; i<numargs; i++)
		{
			c->expr->evalTC(_env, command->y_arg_types[i], &c->cv);
			current_call_memptr.mem[i].any = c->cv.value.any;
			c=c->next;
		}
#endif
	}
   else
   {
      numargs = 0;
   }

   // Add return object ("this") to parameter list
   current_call_memptr.mem[i].any = (void*) _o;

	// ---- now we collected all args ----
	switch(command->y_callstyle)
	{
	case 0:
		((void(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		break;
	case 1: // ---- args, no return
		((void(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		break;
	case 2: // ---- int return
		_r->value.int_val=((sSI(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_INT;
		break;
	case 3: // ---- args+int return
		_r->value.int_val=((sSI(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_INT;
		break;
	case 4: // ---- float return
		_r->value.float_val= ((sF32(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_FLOAT;
		break;
	case 5: // ---- args+ float return
		_r->value.float_val= ((sF32(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_FLOAT;
		break;
	case 6: // ---- object return
		_r->value.object_val=(YAC_Object*)((void*(YAC_CALL*)(void*))command->y_adr)((void*)_o);
		_r->type = YAC_TYPE_OBJECT;
		_r->deleteme = 0;
		break;
	case 7: // ---- args+object return
		_r->value.object_val=(YAC_Object*)((void*(YAC_CALL*)(void*, yacmemptr))command->y_adr)((void*)_o, current_call_memptr);
		_r->type = YAC_TYPE_OBJECT;
		_r->deleteme = 0;
		break;
	case 8: // ---- var return
		((void(YAC_CALL*)(void*, YAC_Value*))command->y_adr)((void*)_o, _r);
		break;
	case 9: // ---- args+var return
		((void(YAC_CALL*)(void*, yacmemptr, YAC_Value*))command->y_adr)((void*)_o, current_call_memptr, _r);
		break;
	}
#ifdef TKS_MT
	for(i=0; i<numargs; i++)
	{
		args[i].unset();
	}
#else
	if(arglist)
	{
		c = arglist->first_arg;
		while(c)
		{
			c->cv.unset();
			c=c->next;
		}
	}
#endif // TKS_MT

   if(b_ret_retobj)
   {
      _r->safeInitObject(_o, 0);
   }

   // Discard any uncritical error that is not caught by the caller frame
   _env->context->discardUncriticalError();
}


void PTN_ECallY::evalCPPCall(YAC_Object *_o, PTN_Env *_env, YAC_Value *_r) const {

#ifdef TKS_MT
   yacmemptr current_call_memptr;
   yacmem linargs[TKS_MAX_ARGS];
   current_call_memptr.mem = linargs;
#endif

   PTN_ECallY::EvalCPPCall(_o, command, arglist, current_call_memptr, _env, _r);
}


static void PTN_ECallY__evalCallStatement(PTN_Env *_env, const PTN_Statement *_st) {
   //
   // This is called when call is used as a statement and the return value has to be discarded
   //

   const PTN_ECallY *e = (const PTN_ECallY*) _st; // _st is not really a statement in this case :)

   YAC_Object *o;
   if(NULL != e->cached_object)
   {
      YAC_Value *co = Dgetvar(e->cached_object);
      o = co->value.object_val;
   }
   else if(e->b_indirect)
   {
      // "indirect" call
      o = _env->context->tksvm_indirect_object;
   }
   else
   {
      // "this" call
      o = _env->context->tksvm_class_stack_object;
   }

   if(TKS_VALID(o))
   {
#ifdef DX_SAFEMODE
      if(!tkscript->cpp_typecast_map[o->class_ID][e->clid])
      {
         YAC_String s; s.alloc(1024);
         const char*coClassName = (const char*) o->yacClassName();
         s.printf("cannot call method of incompatible c++ object (compiled for %s, have %s)",
            TKSCRIPT__ATOM_SPEEDPATH[e->clid]->template_object->yacClassName(),
            coClassName);
         Drtthrow(e, TKS_EXCEPTION_NATIVECLASSTYPEMISMATCH, (const char*)s.chars);
         return;
      }
#endif // DX_SAFEMODE


#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->pushFunctionCallY(o, e->command, e->src_loc);
#endif

      Dcondbeginfunprofile;


      // Invoke C++ method
      {
         YAC_Value r;
         (e->*e->evalCPPCall_opt)(o, _env, &r);

         // Discard return value
         r.unsetFast();
      }

      Dcondendfunprofile((YAC_CommandY*)e->command);


#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->popFunctionCallY();
#endif

   }
   else
   {
      Drtthrowinvalidpointer(e, "cannot call method of invalid c++ object", o);
   }
}

Fevalst PTN_ECallY::getEvalCallSt(void) const {
   //
   // This is only called when ECallY is used as a statement (return value must be discarded in this case)
   //
   return PTN_ECallY__evalCallStatement;
}

void PTN_ECallY__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   //
   // Note: this is also called when call is used as an expression, i.e. do *NOT* discard the return value!
   //

   const PTN_ECallY *st = (const PTN_ECallY *)_st;

   YAC_Object *o;
   if(NULL != st->cached_object)
   {
      YAC_Value *co = Dgetvar(st->cached_object);
      o = co->value.object_val;
   }
   else if(st->b_indirect)
   {
      // "indirect" call  (e.g. myclassmember.myNativeMethod())
      o = _env->context->tksvm_indirect_object;
   }
   else
   {
      // "this" call  (e.g. yacMetaClassName())
      o = _env->context->tksvm_class_stack_object;
   }

   if(TKS_VALID(o))
   {
      if(!tkscript->cpp_typecast_map[o->class_ID][st->clid])
      {
         YAC_String s; s.alloc(1024);
         const char *coClassName = (const char*) o->yacClassName();
         s.printf("cannot call method of incompatible c++ object (compiled for %s, have %s)",
            TKSCRIPT__ATOM_SPEEDPATH[st->clid]->template_object->yacClassName(),
            coClassName);
         Drtthrow(st, TKS_EXCEPTION_NATIVECLASSTYPEMISMATCH, (const char*)s.chars);
         return;
      }

#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->pushFunctionCallY(o, st->command, st->src_loc);
#endif

      Dcondbeginfunprofile;

      // Invoke C++ method
      (st->*st->evalCPPCall_opt)(o, _env, _r);

      Dcondendfunprofile((YAC_CommandY*)st->command);


#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->popFunctionCallY();
#endif

      Ddubiousfixstringtype(_r);
   }
   else
   {
      Drtthrowinvalidpointer(st, "cannot call method of invalid c++ object", o);
   }
}

Feval PTN_ECallY::getEval(void) const {
   return PTN_ECallY__eval;
}

void PTN_ECallY::evalCallableExpr(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ECallY__eval(_env, _r, this);
}

void PTN_ECallY::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ECallY__eval(_env, _r, this);
}

void PTN_ECallY::optimize(void) {
   if(retobj_expr)
   {
      tks_optimize_expr(&retobj_expr, 0);
      retobj_expr_opt = retobj_expr->getEval();
   }
   if(arglist)
   {
      ////----arglist->optimize();
      PTN_ArgListNode *c = arglist->first_arg;
      sUI i = 0u;
      while(c)
      {
#if 0
         tkscript->printf("[>>>] PTN_ECallY::optimize: optimize argIdx=%u in module \"%s\" line %i.\n",
                          i,
                          Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                          Dsrcloc_linenr(src_loc)
                          );
#endif
         tks_optimize_expr(&c->expr, (sU8) command->y_arg_types[i++]);
         c->expr_opt = c->expr->getEval();
         c = c->next;
      }
   }
}

sBool PTN_ECallY::resolveXRef(void) {
	return
      (retobj_expr?retobj_expr->resolveXRef():1) &&
      (arglist?arglist->resolveXRef():1);
}

#ifdef TKS_JIT
sBool PTN_ECallY::forceHybrid(void) {
	return
      (NULL == cached_object) || // "this" or "indirect" call ?
      (command->y_return_type==4) || // variable return type ?
      (retobj_expr?(retobj_expr->forceHybrid() || (retobj_expr->getType() < YAC_TYPE_OBJECT)):0) ||
      (arglist?arglist->forceHybrid():0);
}

sU8 PTN_ECallY::compile(VMCore *_vm) {
	return compileCallStatement(_vm);
}

sU8 PTN_ECallY::compileCallStatement(VMCore *_vm) {
   sUI r;

   if(b_rself_emul)
   {
      // Compile "modify self" method call (=>*, <=>*), emulated via RARG method variant

      // Push "this" -> last argument -> return object!
      _vm->pushv(cached_object);

      // Compile argument list
      if(NULL != arglist)
      {
         r = arglist->compileY(command, _vm);
      }
      else
      {
         r = 0;
      }

      if(255 != r)
      {
         // Push address of arguments
         _vm->addOpcode(VMOP_PUSHS);

         // Push "this"
         _vm->pushv(cached_object);

         // Call method
         _vm->addCPPCallY(command, 1/*one return object*/);

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
   }
   else if(retobj_expr)
   {
      // Compile returnobject parameter method call (=>, <=>)

      // Compile last argument (return object!)
      retobj_expr->compile(_vm);

      // Compile argument list
      if(NULL != arglist)
      {
         r = arglist->compileY(command, _vm);
      }
      else
      {
         r = 0;
      }

      if(255 != r)
      {
         // Push address of arguments
         _vm->addOpcode(VMOP_PUSHS);

         // Push "this"
         _vm->pushv(cached_object);

         // Call method
         _vm->addCPPCallY(command, 1/*one return object*/);

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
   }
   else
   {
      // Compile regular method call / =>* call if RSELF variant exists
      if(NULL != arglist)
      {
         r = arglist->compileY(command, _vm);
      }
      else
      {
         r = 0;
      }

      if(255 != r)
      {
         if(0 != command->y_arg_num)
         {
            _vm->addOpcode(VMOP_PUSHS);
         }

         _vm->pushv(cached_object);

         _vm->addCPPCallY(command, 0/*no return object*/);

         return command->y_return_type;
      }
   }

   // Failed
   return 255;
}

#endif // TKS_JIT
