/// TKS_ScriptModule.cpp
///
/// (c) 2005-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#include <stdlib.h>
#include <string.h>

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_compiler.h"
#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "YAC_ListNode.h"
#include "YAC_PointerArray.h"
#include "YAC_ObjectArray.h"

#include "TKS_ScriptObjectID.h"
#include "TKS_ScriptVariable.h"
#include "TKS_FunctionObject.h"
#include "TKS_ScriptModule.h"

#include "TKS_Mutex.h"


static void loc_print_hook(YAC_String *_output, void *_userdataScriptModule, YAC_String *_s) {
   TKS_ScriptModule *script = (TKS_ScriptModule*)_userdataScriptModule;
   (void)script;
   // ::printf("xxx loc_print_hook: s=\"%s\"\n", (const char*)_s->chars);
   _output->append(_s);
}

TKS_ScriptModule::TKS_ScriptModule(void) {
   line_offset         = 0u;
   cscript             = NULL;
   class_nsp           = NULL;
   last_error_line_idx = -1;
   print_hook_output   = NULL;
}

TKS_ScriptModule::~TKS_ScriptModule() {
   unload();
   uninstallPrintHook();
}

sBool TKS_ScriptModule::setClassNamespace(YAC_String *_nspName) {
   sBool ret = YAC_TRUE;
   if(NULL != _nspName)
   {
      class_nsp = tkscript->findNamespace(_nspName);

      if(NULL == class_nsp)
      {
         class_nsp = tkscript->addNamespace(_nspName);
         ret = (NULL != class_nsp) ? YAC_TRUE : YAC_FALSE;
      }
   }
   else
   {
      class_nsp = NULL;
   }
   return ret;
}

void TKS_ScriptModule::installPrintHook(void) {
   if(NULL == print_hook_output)
   {
      print_hook_output = YAC_New_String();
      if(NULL != print_hook_output)
      {
         tkscript->yacPrintHookAdd(&loc_print_hook, print_hook_output, (void*)this/*userdata*/);
      }
   }
}

void TKS_ScriptModule::uninstallPrintHook(void) {
   if(NULL != print_hook_output)
   {
      tkscript->yacPrintHookRemove(&loc_print_hook, print_hook_output, (void*)this/*userdata*/);
      delete print_hook_output;
      print_hook_output = NULL;
   }
}

void TKS_ScriptModule::setLineOffset(sUI _offset) {
   line_offset = _offset;
}

sSI TKS_ScriptModule::load(YAC_String *_s) {
   // Dprintf("xxx TKS_ScriptModule::load: ENTER\n");

#ifdef TKS_MT
   // The compiler is currently not re-entrant / thread safe. => Lock global mutex
   (void)tkscript->lockGlobalContext();
#endif //TKS_MT

   unload();

   TKS_Compiler *c = Dtkscompiler;
   script_id.s_id.id = c->newTempModule();

   last_error_line_idx = -1;

   if( ((sSI)script_id.s_id.id) >= 0)
   {
      script_id.s_id.magic = c->temp_modules[script_id.s_id.id].magic;
      TKS_CachedScript *cs = c->temp_modules[script_id.s_id.id].cscript;
      cs->linenr_offset = line_offset;
      cs->is_tmod = YAC_TRUE;

      if(NULL != class_nsp)
      {
         // select tmod_class_nsp as default create_namespace for classes
         cs->setTempScriptClassNamespace(class_nsp);
      }

      TKS_TokenizedScript::AllocTSLCache();

      if(cs->scanSource(_s, YAC_FALSE/*copy source*/))
      {
         TKS_TokenizedScript::FreeTSLCache();

#ifdef TKS_ALLOW_TEMP_CLASSES
         if(!c->createClassTemplates())
         {
            Dprintf("[---] TKS_ScriptModule::load: createClassTemplates() failed.\n");
            c->deleteTempModule(script_id.s_id.id);
            tkscript->main_context->b_running = YAC_TRUE;  // [17Sep2017] continue after failed attempt
            c->parse_error = YAC_FALSE;

#ifdef TKS_MT
            tkscript->unlockGlobalContext();
#endif // TKS_MT

            tkscript->ptn_error_code = 0u;
            tkscript->ptn_error_loc  = 0u;

            // Dprintf("xxx TKS_ScriptModule::load: LEAVE (fail)\n");
   
            return YAC_FALSE;
         }

         // Prepare class method args
         // Dprintf("xxx TKS_ScriptModule::load: call resolveFunctionAndMethodArguments\n");
         if(!c->resolveFunctionAndMethodArguments(cs/*fltMod*/))
         {
            Dprintf("[---] TKS_ScriptModule::load: resolveFunctionAndMethodArguments() failed.\n");
            c->deleteTempModule(script_id.s_id.id);
            tkscript->main_context->b_running = YAC_TRUE;  // [17Sep2017] continue after failed attempt
            c->parse_error = YAC_FALSE;

#ifdef TKS_MT
            tkscript->unlockGlobalContext();
#endif // TKS_MT

            tkscript->ptn_error_code = 0;
            tkscript->ptn_error_loc  = 0;

            // Dprintf("xxx TKS_ScriptModule::load: LEAVE (fail)\n");

            return YAC_FALSE;
         }

#endif // TKS_ALLOW_TEMP_CLASSES

         // // Dprintf("xxx call cs->compileSource()\n");
         if(cs->compileSource())
         {
            if(cs->parser_tree->resolveXRefAll())
            {
               cs->parser_tree->optimizeAll();

               if(c->initializeStaticMethods())
               {
                  cscript = (void*)cs;

#ifdef TKS_MT
                  tkscript->unlockGlobalContext();
#endif // TKS_MT
                  
                  // Dprintf("xxx TKS_ScriptModule::load: LEAVE (OK)\n");
                  
                  return YAC_TRUE;
               }
               else
               {
                  Dprintf("[---] TKS_ScriptModule::load: initializeStaticMethods() failed\n");
                  c->cleanTempClasses(cs);
               }
            }
            else
            {
               last_error_line_idx = Dsrcloc_linenr(tkscript->ptn_error_loc);
               Dprintf("[---]\n[---] parse error in temporary module, line %i.\n[---]\n",
                       Dsrcloc_linenr(tkscript->ptn_error_loc)
                       );
               c->cleanTempClasses(cs);
               // Dprintf("xxx parse error in temporary module: END cleanTempClasses\n");
            }
         }
         else
         {
            last_error_line_idx = cs->getLineIdx();
            Dprintf("[---] failed to compile temporary module (line %i).\n", (last_error_line_idx/*+1*/));
            c->cleanTempClasses(cs);
         }
      }
      else
      {
         c->cleanTempClasses(cs);

         Dprintf("\n[---] failed to preprocess temporary module.\n");
         TKS_TokenizedScript::FreeTSLCache();
      }
      c->deleteTempModule(script_id.s_id.id);
      tkscript->main_context->b_running = YAC_TRUE;  // [17Sep2017] continue after failed attempt
      c->parse_error = YAC_FALSE;
   }
   else
   {
      Dprintf("\n[---] newTempModule() failed (id=%d)\n", script_id.s_id.id);
   }

#ifdef TKS_MT
   tkscript->unlockGlobalContext();
#endif //TKS_MT

   // Reset error state
   //  (note) [02Jan2018] TKS_ScriptEngine::yacRunning() now also checks only b_running flag, not ptn_error_code
   tkscript->ptn_error_code = 0u;
   tkscript->ptn_error_loc  = 0u;

   // Dprintf("xxx TKS_ScriptModule::load: LEAVE (fail)\n");

   return YAC_FALSE;
}

void TKS_ScriptModule::unload(void) {
   if(cscript)
   {
      TKS_Compiler *c = Dtkscompiler;
      TKS_CachedScript *cs = (TKS_CachedScript*)cscript;
      c->cleanTempClasses(cs);
      c->deleteTempModule(script_id.s_id.id);
      cscript = NULL;
   }
}

sSI TKS_ScriptModule::getLastErrorLineIndex(void) {
   return last_error_line_idx;
}

void TKS_ScriptModule::findVariable(YAC_String *_s, YAC_Value *_r) {
   if(cscript)
   {
      TKS_CachedScript *cs = (TKS_CachedScript*)cscript;
      TKS_CachedObject *co = cs->module_vars.findEntry(_s);
      if(co)
      {
         TKS_ScriptVariable *v = (TKS_ScriptVariable*)YAC_NEW_CORE_POOLED(TKS_CLID_VARIABLE);
         v->var       = co;
         v->value.any = co->value.any;
         v->type      = co->type;
         v->deleteme  = YAC_FALSE;
         v->script_id.s_id.id    = script_id.s_id.id;
         v->script_id.s_id.magic = script_id.s_id.magic;
         YAC_RETO(v, 1);
         return;
      }
   }
   YAC_RETO(0,0);
}

void TKS_ScriptModule::findTaggedVariableByIndex(sSI _index, YAC_Value *_r) {
   TKS_CachedScript *cs=(TKS_CachedScript *)cscript;
   if(((sUI)_index) < cs->tagged_vars.num_elements)
   {
      TKS_ScriptVariable *v = (TKS_ScriptVariable*)YAC_NEW_CORE_POOLED(TKS_CLID_VARIABLE);
      TKS_CachedObject *co = cs->tagged_vars.elements[_index];
      v->var       = co;
      v->value.any = co->value.any;
      v->type      = co->type;
      v->deleteme  = YAC_FALSE;
      v->script_id.s_id.id    = script_id.s_id.id;
      v->script_id.s_id.magic = script_id.s_id.magic;
      YAC_RETO(v, 1);
      return;
   }
   YAC_RETO(0,0);
}

void TKS_ScriptModule::findFunction(YAC_String *_s, YAC_Value *_r) {
   if(cscript)
   {
      TKS_CachedScript *cs=(TKS_CachedScript *)cscript;
      PTN_Function *f=cs->findFunction(_s);
      if(f)
      {
         TKS_FunctionObject *fi = (TKS_FunctionObject*)YAC_NEW_CORE_POOLED(TKS_CLID_FUNCTION);
         fi->fun = f;
         fi->script_id.s_id.id    = script_id.s_id.id;
         fi->script_id.s_id.magic = script_id.s_id.magic;
         YAC_RETO(fi, 1);
         return;
      }
   }
   YAC_RETO(NULL,0);
}

void TKS_ScriptModule::eval(void) {
   if(NULL != cscript)
   {
      TKS_CachedScript *cs = (TKS_CachedScript*)cscript;

      // xxx TKS_MT: use *real* context (context creation is slow)
      TKS_Context *context = (TKS_Context *) tkscript->yacContextCreate();

      context->b_running = YAC_TRUE;

      PTN_Env env;
      env.context = context;
      env.continue_flag = YAC_TRUE;
      cs->parser_tree->evalFirst(&env);
      env.cret.unset();

      tkscript->yacContextDestroy((void*)context);
   }
}

void TKS_ScriptModule::getPrintHookOutputAndClear(YAC_Value *_r) {
   if(NULL != print_hook_output)
   {
      tkscript->lockMtxPrint();
      _r->initNewString(print_hook_output);
      print_hook_output->empty();
      tkscript->unlockMtxPrint();
   }
   else
   {
      _r->initNull();
   }
}
