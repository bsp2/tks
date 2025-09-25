/// PTN_Function.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "tks_inc_function.h"

#include "YAC_ValueObject.h"
#include "YAC_Integer.h"
#include "YAC_Float.h"

#include "tks_inc_class.h"
#include "tks_inc_compiler.h"

#include "TKS_Context.h"
#include "TKS_Mutex.h"

#ifdef TKS_DCON
#include <stdio.h>
#endif

sSI PTN_Function::object_counter = 0;


PTN_Function::PTN_Function(void) {
   initFunction();
}

PTN_Function::PTN_Function(YAC_String *_name) {
   initFunction();

   name.yacCopy(_name);
}

void PTN_Function::initFunction(void) {
   PTN_Function::object_counter++;
   body                   = NULL;
   argvarlist             = NULL;
   fun_vars               = NULL;
   opt_local_vars         = NULL;
   num_local_vars         = 0u;
   formal_args            = NULL;
   parent_class           = NULL;
   nspace                 = NULL;
   protection_flags       = 0u;
   is_class_function      = YAC_FALSE;
   return_type            = YAC_TYPE_VARIANT;
   return_template_object = NULL;
   return_class_nspaces   = NULL;
   function_object        = NULL;

#ifdef DX_SCRIPTCONSTRAINTS
   return_constraint      = NULL;
   return_constraint_type = TKS_CONSTRAINTTYPE_NONE;
#endif // DX_SCRIPTCONSTRAINTS

#ifdef TKS_MT
   mutex                  = NULL;
   b_own_mutex            = YAC_FALSE;
#endif // TKS_MT

#ifdef TKS_PROFILE_FUNCTIONS
   profile_millisec       = 0.0;
   profile_callcount      = 0;
#endif // TKS_PROFILE_FUNCTIONS
}

PTN_Function::~PTN_Function() {
   PTN_Function::object_counter--;

   if(function_object)
   {
      YAC_DELETE(function_object);
      function_object = NULL;
   }

   if(body)
   {
      delete body;
      body = NULL;
   }

   name.free();

   if(argvarlist)
   {
      delete argvarlist;
      argvarlist = NULL;
   }

   if(formal_args)
   {
      delete formal_args;
      formal_args = NULL;
   }

   if(NULL != fun_vars)
   {
      delete fun_vars;
      fun_vars = NULL;
   }

   freeStackFrameTemplate();

   if(NULL != return_class_nspaces)
   {
      delete[] return_class_nspaces;
      return_class_nspaces = NULL;
   }

#ifdef TKS_MT
   if(b_own_mutex)
   {
      delete mutex;
      mutex = NULL;
      b_own_mutex = YAC_FALSE;
   }
#endif // TKS_MT

}

sBool PTN_Function::initMutex(sBool _bAnonymousMutex, YAC_String *_mutexName) {
#ifdef TKS_MT
   if(_bAnonymousMutex)
   {
      b_own_mutex = YAC_TRUE;
      mutex = new TKS_Mutex();
   }
   else
   {
      b_own_mutex = YAC_FALSE; // named mutexes are shared among n functions/methods in m scripts, they are freed at exit
      if(NULL != _mutexName)
      {
         mutex = tkscript->findNamedMutex(_mutexName);
         if(NULL == mutex)
         {
            mutex = tkscript->createNamedMutex(_mutexName);
            if(NULL == mutex)
            {
               Dprintf("[---] PTN_Function::initMutex: failed to create named mutex (funname=%s).\n", (char*)name.chars);
               return YAC_FALSE;
            }
         }
      }
      else
      {
         mutex = NULL;
      }
   }
#endif
   return YAC_TRUE;
}

sUI PTN_Function::getID(void) const {
   return body ? PTN_FUNCTIONBODY : PTN_FUNCTION;
}

TKS_ObjectCache *PTN_Function::getCreateFunVars(void) {
   if(NULL == fun_vars)
   {
      fun_vars = new TKS_ObjectCache();
      fun_vars->allocCache(TKS_MAX_FUNCTION_VARIABLES);
   }
   return fun_vars;
}

TKS_CachedObject *PTN_Function::findFunVar(YAC_String *_name) {
   if(NULL != fun_vars)
   {
      return fun_vars->findEntry(_name);
   }
   else
   {
      return NULL;
   }
}

sBool PTN_Function::semanticCheck(void) {
   if(!body)
   {
#ifdef TKS_DCON
      if(parent_class)
      {
         Dprintf("\n[---] missing body for function \"%s::%s\".\n",
                 (char*)parent_class->name.chars,
                 (char*)name.chars
                 );
      }
      else
      {
         Dprintf("\n[---] missing body for function \"%s\".\n", (char*)name.chars);
      }
#endif
      Dcterror(PTNERR_UFUNMISSINGBODY);
      return YAC_FALSE;
   }

#if 0
   if(tkscript->b_runtime)
   {
      Dprintf("xxx PTN_Function::semanticCheck: name=\"%s\" argvarlist=%p\n", (const char*)name.chars, argvarlist);
   }
#endif // 0

   if(argvarlist)
   {
      argvarlist->init(); // ---- create linear argument table
   }

   if(return_type >= YAC_TYPE_OBJECT)
   {
      if(YAC_TYPE_STRING != return_type) // string type is already resolved
      {
         if(!return_class_name.isBlank())
         {
            // ---- resolve class name
            // ---- try script class
            TKS_ClassDecl *cd = Dtkscompiler->findClassDeclNamespaceArray(&return_class_name, return_class_nspaces);
            if(cd)
            {
               // ---- found script class
               delete [] return_class_nspaces; // the array is no longer needed
               return_class_nspaces = NULL;
               return_template_object = cd->class_template;
            }
            else
            {
               // try C++ class
               TKS_ClassTemplate *tmp = tkscript->findTemplate(&return_class_name);
               if(tmp)
               {
                  // ---- found c++ class
                  return_template_object = tmp->template_object;
               }
               else
               {
                  Dprintf("\n[---] Return: cannot resolve return_type class \"%s\".\n",
                          (char*)return_class_name.chars
                          );
                  return YAC_FALSE;
               }
            }
         }
      }
   }
   return body->semanticCheckAll();
}

sBool PTN_Function::resolveXRef(void) {
   return body->resolveXRefAll();
}

void PTN_Function::optimize(void) {
   subGenCallList();
   prepareStackFrameTemplate();
   if(body)
   {
      body->optimizeAll();
   }

#ifdef TKS_FIX_ABSTRACTVARS
   if(fun_vars)
   {
      fun_vars->fixAbstractVars();
   }
#endif // TKS_FIX_ABSTRACTVARS
}

void PTN_Function::subGenCallList(void) {
   // tkscript->printf("xxx PTN_Function::subGenCallList: this=%p name=%s body=%p\n", this, name.chars, body);
   // tkscript->printf("xxx    body->st_list=%p\n", body->st_list);
   if(body)
   {
      body->genCallList();
   }
}

sBool PTN_Function::setArg(TKS_Context *_context, sU8 _i, YAC_Value *_v) {
   if(argvarlist)
   {
      YAC_Value *co = argvarlist->getVar(_context, _i);
      if(co)
      {
         co->derefPointerAssignValue(_v);
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

void PTN_Function::setArgsByList(PTN_Env *_env, PTN_ArgList *arglist, YAC_Value *_nsf) {
   //
   // Note: caller must ensure that function actually has arguments
   //

   // ---- function has n arguments
   // ---- evaluate arguments and store in function parameter vars
   PTN_ArgListNode *  ce  = arglist->first_arg;
   TKS_CachedObject **cvl = argvarlist->cached_vars;
   sU8 i;
   _env->context->stacki_displace -= num_local_vars;

#ifdef DX_SCRIPTCONSTRAINTS
   TKS_FormalArgument *a = formal_args;
#endif

   for(i = 0u; i < arglist->num_args; i++)
   {
      TKS_CachedObject *cv = *cvl++;
      YAC_Value r;
      ce->expr_opt(_env, &r, ce->expr);
      if(Dhaveexception)
      {
         _env->context->stacki_displace += num_local_vars;
         r.unsetFast();
         Dhandleexception(ce->expr);
         return;
      }

#ifdef DX_SCRIPTCONSTRAINTS
      if(a && tkscript->configuration.b_enable_constraints)
      {
         checkArgumentConstraint(_env, &r, a);
      }
#endif

      YAC_Value *dv = ((cv->flags & TKS_CachedObject::ISLOCAL) ? &_nsf[cv->local_index] : cv);

      if(cv->flags & TKS_CachedObject::ISDYNAMIC)
      {
         dv->unset();
         dv->copySafe(&r); // always replace variable type and value
      }
      else
      {
         dv->derefPointerAssignValue(&r); // grab rw object from r, convert to formal type
      }
      ce = ce->next;

#ifdef DX_SCRIPTCONSTRAINTS
      if(a)
      {
         a = a->next;
      }
#endif
   }
   _env->context->stacki_displace += num_local_vars;
}

void PTN_Function::setArgsByValues(PTN_Env *_env, YAC_Value *_argValues, YAC_Value *_nsf) {
   if(argvarlist)
   {
      if(_argValues)
      {
         // ---- function has n arguments
         // ---- evaluate arguments and store in function parameter vars
         YAC_Value *argVal = _argValues;
         TKS_CachedObject **cvl = argvarlist->cached_vars;
         sU8 i;
         _env->context->stacki_displace -= num_local_vars;

#ifdef DX_SCRIPTCONSTRAINTS
         TKS_FormalArgument *a = formal_args;
#endif

         for(i = 0u; i < argvarlist->num_vars; i++)
         {
            TKS_CachedObject *cv=*cvl++;

 #ifdef DX_SCRIPTCONSTRAINTS
            if(a&&tkscript->configuration.b_enable_constraints)
            {
               checkArgumentConstraint(_env, argVal, a);
            }
#endif
            if(cv->flags & TKS_CachedObject::ISDYNAMIC)
            {
               ((cv->flags & TKS_CachedObject::ISLOCAL) ? &_nsf[cv->local_index] : cv)
                  ->copySafe(argVal); // always replace variable type and value
            }
            else
            {
               ((cv->flags&TKS_CachedObject::ISLOCAL) ? &_nsf[cv->local_index] : cv)
                  ->derefPointerAssignValue(argVal); // grab rw object from r, convert to formal type
            }
            argVal++;

#ifdef DX_SCRIPTCONSTRAINTS
            if(a)
            {
               a = a->next;
            }
#endif
         }
         _env->context->stacki_displace += num_local_vars;
      }
   }
}

#ifdef DX_SCRIPTCONSTRAINTS
void PTN_Function::checkArgumentConstraint(PTN_Env *_env, YAC_Value *_cv, TKS_FormalArgument *_a) {
   switch(_a->constraint_type & 7)
   {
      case TKS_CONSTRAINTTYPE_NONE:
      default:
         break;

      case TKS_CONSTRAINTTYPE_EXPR:
         if(!_a->script_constraint->checkConstraintExpr(_env, _cv))
         {
            handleConstraintViolation(_cv, (sChar*)_a->name.chars, _a->script_constraint, _a);
            return;
         }
         break;

      case TKS_CONSTRAINTTYPE_NOTNULL:
      {
         // ---- check whether given value is an instanceof the formal class type
         sBool bok = YAC_FALSE;
         if(_cv->type >= YAC_TYPE_OBJECT)
         {
            if(YAC_VALID(_cv->value.object_val))
            {
               if(_a->class_type)
               {
                  if(_a->class_type >= 0)
                  {
                     bok=(tkscript->cpp_typecast_map[_cv->value.object_val->class_ID][_a->class_type]);
#ifdef TKS_DCON
                     if(!bok)
                     {
                        Dprintf("[---] C++ class \"%s\" is not a base class of \"%s\".\n",
                                (char*)_cv->value.object_val->yacClassName(),
                                (char*)TKSCRIPT__ATOM_SPEEDPATH[_a->class_type]->template_object->yacClassName()
                                );
                     }
#endif
                  }
                  else
                  {
                     bok=(_cv->value.object_val->yacMetaClassInstanceOf(_a->cached_meta_class));
#ifdef TKS_DCON
                     if(!bok)
                     {
                        TKS_ClassDecl *cd = ((TKS_ScriptClassInstance*)_cv->value.object_val)->class_decl;
                        Dprintf("[---] script class \"%s::%s\" is not a base class of \"%s::%s\".\n",
                                (char*)cd->nspace->name.chars,
                                (char*)cd->name.chars,
                                (char*)_a->cached_meta_class->class_decl->nspace->name.chars,
                                (char*)_a->cached_meta_class->yacMetaClassName()
                                );
                     }
#endif
                  }
               }
               else
               {
                  bok=1;
               }
            }
            // xxx else invalid argument object
         }
         if(!bok)
         {
            handleConstraintViolation(_cv, (sChar*)_a->name.chars, _a->script_constraint, _a);
            return;
         }
      }
      break;
   } // switch constraint type
}

void PTN_Function::handleConstraintViolation(YAC_Value *_cv,
                                             const sChar *_vname,
                                             TKS_ScriptConstraint *_constraint,
                                             TKS_FormalArgument *_a /* NULL==return constraint failed */
                                             ) const
{
#ifdef TKS_DCON
   YAC_ValueObject vo; vo.initValue(_cv);
   YAC_String vos; vo.yacToString(&vos);

   Dprintf("[---] constraint violation \"%s\" (%s==%s).\n",
           (const char*)(_constraint?(const char*)_constraint->name.chars:"notnull"),
           (const char*)_vname,
           (const char*)vos.chars
           );
   if(_constraint)
   {
      if(!_constraint->comment.isBlank())
      {
         Dprintf("[---]  >>>\"%s\"<<<\n", (char*)_constraint->comment.chars);
      }
   }
   Dprintf("[---] in call to %s::%s(",
           parent_class?(char*)parent_class->name.chars:"",
           (char*)name.chars
           );
   sUI k = 0u;
   TKS_FormalArgument *ca = formal_args;
   while(ca)
   {
      if(k > 0u)
      {
         Dprintf(", ");
      }
      if(ca == _a)
      {
         Dprintf("***");
      }
      const char *tn;
      switch(ca->type)
      {
         default:
         case 0: tn="void";   break;
         case 1: tn="int";    break;
         case 2: tn="float";  break;
         case 3: tn=(const char*)ca->class_name.chars; break;
         case 4: tn="String"; break;
      }
      Dprintf("%s %s", tn, (const char*)ca->name.chars);
      switch(ca->constraint_type)
      {
        case TKS_CONSTRAINTTYPE_EXPR:
            if(ca->script_constraint)
            {
               Dprintf(".%s",(char*)ca->script_constraint->name.chars);
            }
            // xxx else internal error...
            break;
         case TKS_CONSTRAINTTYPE_NOTNULL:
            Dprintf("notnull");
            break;
      }
      if(ca == _a)
      {
         Dprintf("***");
      }
      ca = ca->next;
      k++;
   }
   Dprintf(") : ");

   if(!_a) // ---- no argument given means that the return constraint failed !
   {
      Dprintf("***");
   }

   switch(return_type & 7u)
   {
      default:
         Dprintf("*ILL*");
         break;

      case YAC_TYPE_VARIANT: // 0
         Dprintf("var");
         break;

      case YAC_TYPE_INT: // 1
         Dprintf("int");
         break;

      case YAC_TYPE_FLOAT: // 2
         Dprintf("float");
         break;

      case YAC_TYPE_OBJECT: // 3
         if(!return_class_name.isBlank())
         {
            Dprintf((char*)return_class_name.chars);
         }
         else
         {
            Dprintf("Object");
         }
         break;

      case YAC_TYPE_STRING: // 4
         Dprintf("String");
         break;
   } // switch return_type

   switch(return_constraint_type)
   {
      case TKS_CONSTRAINTTYPE_EXPR:
         if(return_constraint)
         {
            Dprintf(".%s", (char*)return_constraint->name.chars);
         }
         break;

      case TKS_CONSTRAINTTYPE_NOTNULL:
         Dprintf(".notnull");
         break;
   } // switch return constraint type

   if(!_a) // ---- no argument given means that the return constraint failed !
   {
      Dprintf("***\n");
   }
   else
   {
      Dprintf("\n");
   }


#endif // TKS_DCON

   // (TODO) throw runtime exception, this is not a compile time error !
   if(_a)
   {
      tkscript->handleCompiletimeError(_a->script_constraint->constraint_expr->src_loc, PTNERR_CONSTRAINTFAILED);
   }
   else
   {
      tkscript->handleCompiletimeError(src_loc, PTNERR_CONSTRAINTFAILED);
   }
}

#endif // DX_SCRIPTCONSTRAINTS


sUI PTN_Function::getKey(void) {
   return name.getKey();
}

void PTN_Function::evalCall(PTN_Env *_env, YAC_Value *_r) const {
#ifdef TKS_DCON
   if(tkscript->configuration.debug_level > 98u)
   {
      if(parent_class)
      {
         Dprintf("[dbg]: funcall(name=\"%s::%s\")\n",
                 (char*)parent_class->name.chars,
                 (char*)name.chars
                 );
      }
      else
      {
         Dprintf("[dbg]: funcall(name=\"%s\")\n",
                 (char*)name.chars
                 );
      }
   }
#endif
   _r->unset(); // xxx can probably skip this..

   Dcondbeginfunprofile;
   {
      PTN_Env env(_env);
      body->evalFirst(&env);
      _r->copySafe(&env.cret);
   }
   Dcondendfunprofile((PTN_Function*)this);

   //
   // Typecast return value if necessary
   //
   if(return_type >= YAC_TYPE_OBJECT)
   {
      if(return_template_object)
      {
         if(_r->type >= YAC_TYPE_OBJECT)
         {
            Ddubiousfixstringtype(_r);

            if(YAC_VALID(_r->value.object_val))
            {
               // Is returned type derived from declared return type?
               if(! tkscript->cpp_typecast_map[_r->value.object_val->class_ID][return_template_object->class_ID])
               {
                  // No; Allocate new return object and assign value from return expression
                  YAC_Object *t;
                  t = YAC_CLONE_POOLED(_env->context, return_template_object);
                  t->yacOperatorAssign(_r->value.object_val);
                  _r->unsetFast();
                  _r->initObject(t, 1);
               }
               else
               {
                  if(YAC_CLID_CLASS == _r->value.object_val->class_ID) // class instance?
                  {
                     // ---- is formal return type compatible to returned value ?
                     if(! ((TKS_ScriptClassInstance*) _r->value.object_val)->class_decl->ancestor_table[
                        ((TKS_ScriptClassInstance*)return_template_object)->class_decl->class_id
                        ]
                        )
                     {
                        // Alocate new object and assign
                        YAC_Object *t;
                        t = YAC_CLONE_POOLED(_env->context, return_template_object);
                        t->yacOperatorAssign(_r->value.object_val);
                        _r->unsetFast();
                        _r->initObject(t, 1);
                     }
                     else
                     {
                        // Script classes are compatible, keep return value
                     }
                  }
                  else
                  {
                     // C++ classes are compatible, keep return value
                  }
               }
            }
            else
            {
               // keep null objects
            }
         } // else if v.type >= YAC_TYPE_OBJECT
         else
         {
            // expected Object return type but have void/int/float
            switch(_r->type)
            {
               case YAC_TYPE_VOID: // void -> Object
                  _r->initNull();
                  break;

               case YAC_TYPE_INT: // int -> Object
               {
                  YAC_Object *t = YAC_CLONE_POOLED(_env->context, return_template_object);
                  t->yacValueOfI(_r->value.int_val);
                  _r->initObject(t, 1);
               }
               break;

               case YAC_TYPE_FLOAT: // float -> Object
               {
                  YAC_Object *t = YAC_CLONE_POOLED(_env->context, return_template_object);
                  t->yacValueOfF32(_r->value.float_val);
                  _r->initObject(t, 1);
               }
               break;
            }
         }
      }
      else
      {
         // Any object type is allowed (no template object)
         if(_r->type < YAC_TYPE_OBJECT)
         {
            // Cast to best matching Object type (e.g. int -> Integer, float -> Float, ..)
            _r->typecast(YAC_TYPE_OBJECT);
         }
      }
   }
   else if(YAC_TYPE_INT == return_type)
   {
      switch(_r->type)
      {
         case YAC_TYPE_VOID:
            _r->initInt(0); // cast <void> to 0
            break;

         case YAC_TYPE_FLOAT: // float -> int
            _r->initInt((sSI)_r->value.float_val); // cast float to int
            break;

         case YAC_TYPE_OBJECT: // Object -> int
         case YAC_TYPE_STRING: // String -> int
            if(YAC_VALID(_r->value.object_val))
            {
               sSI i;
               if(!_r->value.object_val->yacScanI(&i))
               {
                  i = 0; // cannot scan, silently assign 0
               }
               _r->safeInitInt(i);
            }
            else
            {
               _r->initInt(0); // cast null to 0
            }
            break;
      }
   }
   else if(YAC_TYPE_FLOAT == return_type)
   {
      switch(_r->type)
      {
         case YAC_TYPE_VOID: // void -> float
            _r->initFloat(0.0f); // cast <void> to 0.0f
            break;

         case YAC_TYPE_INT: // int -> float
            _r->initFloat((sF32)_r->value.int_val); // cast int to float
            break;

         case YAC_TYPE_OBJECT: // Object -> float
         case YAC_TYPE_STRING: // String -> float
            if(YAC_VALID(_r->value.object_val))
            {
               sF32 f;
               if(!_r->value.object_val->yacScanF32(&f))
               {
                  f = 0.0f; // cannot scan, silently assign 0.0f
               }
               _r->safeInitFloat(f);
            }
            else
            {
               _r->initFloat(0.0f); // cast null to 0.0f
            }
            break;
      }
   }


   //
   // Now check whether the return value does not violate the return type constraint (if exists)
   //
#ifdef DX_SCRIPTCONSTRAINTS
   if((tkscript->configuration.b_enable_constraints))
   {
      if(return_constraint)
      {
         if(!return_constraint->checkConstraintExpr(_env, _r))
         {
            handleConstraintViolation(_r, "<return-value>", return_constraint, 0);
         }
      }
      else if(TKS_CONSTRAINTTYPE_NOTNULL == return_constraint_type)
      {
         sBool bok = YAC_FALSE;
         if(_r->type >= YAC_TYPE_OBJECT)
         {
            if(YAC_VALID(_r->value.object_val))
            {
               if(return_type >= YAC_TYPE_OBJECT)
               {
                  if(return_template_object)
                  {
                     if(!return_template_object->yacMetaClassName())
                     {
                        bok =
                           tkscript->cpp_typecast_map
                           [_r->value.object_val->class_ID][return_template_object->class_ID]
                           ;

#ifdef TKS_DCON
                        if(!bok)
                        {
                           Dprintf("[---] C++ class \"%s\" is not a base class of \"%s\".\n",
                                   (char*)_r->value.object_val->yacClassName(),
                                   (char*)return_template_object->yacClassName()
                                   );
                        }
#endif
                     }
                     else
                     {
                        bok = (_r->value.object_val->yacMetaClassInstanceOf(return_template_object));
#ifdef TKS_DCON
                        if(!bok)
                        {
                           TKS_ClassDecl *cd = ((TKS_ScriptClassInstance*)_r->value.object_val)->class_decl;
                           Dprintf("[---] script class \"%s::%s\" is not a base class of \"%s::%s\".\n",
                                   (char*)cd->nspace->name.chars,
                                   (char*)cd->name.chars,
                                   (char*)((TKS_ScriptClassInstance*)return_template_object)->class_decl->nspace->name.chars,
                                   (char*)return_template_object->yacMetaClassName()
                                   );
                        }
#endif
                     }
                  }
                  else
                  {
                     bok = 1; // returns Object
                  }
               }
               else
               {
                  bok = 1; /// xxx else internal error
               }
            }
         }

         if(!bok)
         {
            handleConstraintViolation(_r, "<return-value>", 0, 0);
         }
      }
   }
#endif
   ////::printf("function::Evalcall ret: type=%i value=%i deleteme=%i\n", _env->cret.type, _env->cret.value.int_val, _env->cret.deleteme);
}

void PTN_Function::evalVoidCall(TKS_Context *_context) const {
#ifdef TKS_DCON
   if(tkscript->configuration.debug_level > 98u)
   {
      if(parent_class)
      {
         Dprintf("[dbg]: voidfuncall(name=\"%s::%s::%s\")\n",
                 (char*)parent_class->nspace->name.chars,
                 (char*)parent_class->name.chars,
                 (char*)name.chars
                 );
      }
      else
      {
         Dprintf("[dbg]: voidfuncall(name=\"%s\")\n",
                 (char*)name.chars
                 );
      }
   }

#endif

   Dcondbeginfunprofile;

   PTN_Env env(_context);
   body->evalFirst(&env);
   env.cret.unsetFast();

   Dcondendfunprofile((PTN_Function*)this);
}


#ifdef TKS_JIT
sBool PTN_Function::forceHybrid(void) {
   return YAC_FALSE;
}

sU8 PTN_Function::compile(VMCore *_vm) {
   if(body)
   {
      return body->compile(_vm);
   }
   else
   {
      return 0u;
   }
}
#endif

void PTN_Function::prepareStackFrameTemplate(void) {
   if(num_local_vars)
   {
      opt_local_vars=new YAC_Value[num_local_vars];
      sUI i;
      sUI j=0;
      TKS_CachedObject **ce = fun_vars->objects;
      for(i = 0u; i < num_local_vars && j < fun_vars->max_entries; j++,ce++)
      {
         if(*ce)
         {
            if( (*ce)->flags & TKS_CachedObject::ISLOCAL )
            {
               if( ((*ce)->flags & TKS_CachedObject::PTRASSIGN) )
               {
                  if(YAC_TYPE_STRING == (*ce)->type)
                     opt_local_vars[(*ce)->local_index].initString(0, 0);
                  else
                     opt_local_vars[(*ce)->local_index].initObject(0,0);
               }
               else
               {
                  opt_local_vars[(*ce)->local_index].derefPointerAssignValue( *ce );  // xxx
               }
               i++;
            }
         }
      }
   }
}

void PTN_Function::freeStackFrameTemplate(void) {
   if(num_local_vars&&opt_local_vars)
   {
      sUI i;
      for(i = 0u; i < num_local_vars; i++)
      {
         opt_local_vars[i].unset();
      }
      delete [] opt_local_vars;
      num_local_vars = 0u;
      opt_local_vars = 0u;
   }
}

void PTN_Function::addFormalArgument(YAC_String *_name, sSI _type, YAC_String *_className,
                                     sBool _bPointerArray, sBool _array, sBool _local,
                                     TKS_FreezedNamespaceArray _nsa, TKS_Namespace *_forcedNSpace
                                     ) {
   TKS_FormalArgument *a = new TKS_FormalArgument(_name, _type, _className, _bPointerArray, _array, _local);
   a->script_constraint  = NULL;
   a->freezed_namespaces = _nsa;
   a->forced_nspace      = _forcedNSpace;
   TKS_FormalArgument *c = formal_args;
   if(c)
   {
      while(c->next)
      {
         c = c->next;
      }
      c->next = a;
   }
   else
   {
      formal_args = a;
   }
}

sBool PTN_Function::resolveFormalArgs(void) {
#if 0
   if(tkscript->b_runtime)
   {
      Dprintf("xxx PTN_Function::resolveFormalArgs: name=\"%s\" formal_args=%p\n", (const char*)name.chars, formal_args);
   }
#endif // 0
   sBool ret = YAC_TRUE;
   if(argvarlist)
   {
      return YAC_TRUE;
   }
   TKS_FormalArgument *a = formal_args;
   if(a)
   {
      PTN_NEW_STATIC_NODE(argvarlist, PTN_ArgVarList)();
   }
   while(ret && a)
   {
      if(argvarlist->num_vars >= 255u)
      {
#ifdef TKS_DCON
         if(parent_class)
         {
            Dprintf("[---] more than 255 arguments declared for function \"%s::%s\".\n",
                    (char*)parent_class->name.chars,
                    (char*)name.chars
                    );
         }
         else
         {
            Dprintf("[---] more than 255 arguments declared for function \"%s\".\n",
                    (char*)name.chars
                    );
         }

#endif
         return YAC_FALSE;
      }
      TKS_CachedObject *co = a->createVariable(getCreateFunVars(), this);
      if(co)
      {
         argvarlist->addVar(co);
         a = a->next;
      }
      else
      {
         ret = YAC_FALSE;
         Dprintf("[---] failed to resolve parameter type \"%s\" in function %s::%s().\n",
                 (char*)a->class_name.chars,
                 (char*)parent_class->name.chars,
                 (char*)name.chars
                 );
      }
   }
   return ret;
}

void PTN_Function::setLastArgConstraint(TKS_ScriptConstraint *_scs) {
   TKS_FormalArgument *a = formal_args;
   if(a)
   {
      while(a->next)
      {
         a = a->next;
      }
      a->script_constraint = _scs;
      a->constraint_type = TKS_CONSTRAINTTYPE_EXPR;
   }
}

void PTN_Function::setLastArgNotNullConstraint(void) {
   TKS_FormalArgument *a = formal_args;
   if(a)
   {
      while(a->next)
      {
         a = a->next;
      }
      a->constraint_type = TKS_CONSTRAINTTYPE_NOTNULL;
   }
}

void PTN_Function::emitExplanation(void) { // dump name/signature/documentation to stdout
#ifdef TKS_DCON
   tks_fputs((const char*)name.chars);
   // Emit signature, e.g. myMethod:int i,float f,String s:int
   TKS_FormalArgument *a = formal_args;
   while(a)
   {
      // Emit argument type
      tks_fputc((int)',');
      switch(a->var->type)
      {
         case YAC_TYPE_INT:
            tks_fputs("int");
            break;

         case YAC_TYPE_FLOAT:
            tks_fputs("float");
            break;

         case YAC_TYPE_OBJECT:
            if(YAC_VALID(a->var->value.object_val))
            {
               const char *metaClassName = a->var->value.object_val->yacMetaClassName();
               if(NULL != metaClassName)
               {
                  TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance*) a->var->value.object_val;
                  if(sci->class_decl->nspace != &Dtkscompiler->default_namespace)
                  {
                     tks_fputs((const char*)sci->class_decl->nspace->name.chars);
                     tks_fputs("::");
                  }
                  tks_fputs((const char*)metaClassName);
               }
               else
               {
                  tks_fputs((const char*)a->var->value.object_val->yacClassName());
               }
            }
            else
            {
               tks_fputs("Object");
            }
            break;

         case YAC_TYPE_STRING:
            tks_fputs("String");
            break;
      }
      // Emit argument name
      tks_fputc((int)' ');
      tks_fputs((const char*)a->name.chars);
      a = a->next;
   }
   // Emit return type signature part
   if(YAC_TYPE_VARIANT != return_type)
   {
      tks_fputc((int)':');
      switch(return_type)
      {
         case YAC_TYPE_INT:
            tks_fputs("int");
            break;

         case YAC_TYPE_FLOAT:
            tks_fputs("float");
            break;

         case YAC_TYPE_OBJECT:
            if(return_class_name.isBlank())
            {
               tks_fputs("Object");
            }
            else
            {
               const char *metaClassName = return_template_object->yacMetaClassName();
               if(NULL != metaClassName)
               {
                  TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance*) return_template_object;
                  if(sci->class_decl->nspace != &Dtkscompiler->default_namespace)
                  {
                     tks_fputs((const char*)sci->class_decl->nspace->name.chars);
                     tks_fputs("::");
                  }
                  tks_fputs((const char*)metaClassName);
               }
               else
               {
                  tks_fputs((const char*)return_class_name.chars);
               }
            }
            break;

         case YAC_TYPE_STRING:
            tks_fputs("String");
            break;
      }
   }
   // Emit method name/documentation separator
   tks_fputc(CC_SEPARATOR);
   if(!documentation.isBlank())
   {
      tks_fputs((char*)documentation.chars);
   }
#endif
}

TKS_FunctionObject *PTN_Function::getOrCreateFunctionObject(void) {
   if(NULL == function_object)
   {
      function_object = (TKS_FunctionObject*)YAC_NEW_CORE_POOLED(TKS_CLID_FUNCTION);
      function_object->class_ID = TKS_CLID_FUNCTION;
      function_object->fun = this;
   }
   return function_object;
}

sBool PTN_Function::compareFormalArgs(PTN_Function *_f) {
   TKS_FormalArgument *arg   = formal_args;
   TKS_FormalArgument *f_arg = _f->formal_args;
   while(arg)
   {
      if(!f_arg)
      {
         return YAC_FALSE;
      }
      if(arg->is_local != f_arg->is_local)
      {
         return YAC_FALSE;
      }
      if(arg->is_array != f_arg->is_array)
      {
         return YAC_FALSE;
      }
      if(arg->type != f_arg->type)
      {
         return YAC_FALSE;
      }
      if(arg->type == YAC_TYPE_OBJECT)
      {
         // Resolve left hand side class
         if(!arg->resolveClassType())
         {
            Dprintf("[---] PTN_Function::compareFormalArgs: unable to resolve class \"%s\".\n",
                    (char*) arg->class_name.chars
                    );
         }

         // Resolve right hand side class
         if(!f_arg->resolveClassType())
         {
            Dprintf("[---] PTN_Function::compareFormalArgs: unable to resolve class \"%s\".\n",
                    (char*) f_arg->class_name.chars
                    );
         }

         if(NULL != arg->cached_meta_class)
         {
            // Other arg must also be script class

            if(NULL == f_arg->cached_meta_class)
            {
               // Other class is a C++ class -> Type mismatch
               return YAC_FALSE;
            }

            // Compatible classes ?
            if(!arg->cached_meta_class->class_decl->ancestor_table
               [f_arg->cached_meta_class->class_decl->class_id]
               )
            {
               return YAC_FALSE;
            }
         }
         else
         {
            // Other arg must also be a C++ class

            if(NULL == f_arg->cached_cpp_class)
            {
               // Other class is a Script class -> Type mismatch
               return YAC_FALSE;
            }

            // Compatible classes ?
            if(!tkscript->cpp_typecast_map
               [arg->cached_cpp_class->class_ID]
               [f_arg->cached_cpp_class->class_ID]
               )
            {
               return YAC_FALSE;
            }
         }
      }
      arg   = arg   -> next;
      f_arg = f_arg -> next;
   }
   return (NULL == f_arg);
}

void PTN_Function::formalArgsToString(YAC_String *_d) {
   TKS_FormalArgument *arg = formal_args;
   int i = 0;
   while(arg)
   {
      if(i++ > 0)
         _d->append(", ");
      if(arg->is_local)
      {
         _d->append("local ");
      }
      switch(arg->type)
      {
         case YAC_TYPE_INT:
            _d->append("int ");
            break;

         case YAC_TYPE_FLOAT:
            _d->append("float ");
            break;

         case YAC_TYPE_OBJECT:
            if(arg->class_name.isBlank())  // "Object" pointer type ??
            {
               _d->append("Object");
            }
            else
            {
               // Note: the class_name should already have been resolved at this point
               if(arg->cached_cpp_class)
               {
                  _d->append("yac::");
               }
               else if(arg->cached_meta_class)
               {
                  _d->append(&arg->cached_meta_class->class_decl->nspace->name);
                  _d->append("::");
               }
               _d->append(&arg->class_name);
            }
            _d->append(" ");
            break;

         case YAC_TYPE_STRING:
            _d->append("String ");
            break;
      }
      _d->append(&arg->name);
      if(arg->is_array)
      {
         _d->append("[]");
      }
      arg = arg -> next;
   }
}
