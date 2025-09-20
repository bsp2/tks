/// TKS_ScriptEngine.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/// Define to benchmark ObjectPool vs. malloc()
//#define RUN_POOL_BENCHMARK defined

#include "tks.h"

#if !defined(YAC_WIN32) || defined(YAC_CYGWIN)
#include <sys/time.h>
#endif

#ifdef YAC_POSIX
#include <unistd.h>
#endif

#ifdef YAC_WIN32
#include <windows.h>
static LARGE_INTEGER pfcFrequency;
#include <intrin.h>
#endif

#ifdef DX_PTHREADS
#include <pthread.h>
#endif

#ifdef YAC_MACOS
#include <libkern/OSAtomic.h>
#endif


#include "tks_inc_class.h"
#include "tks_inc_compiler.h"
#include "tks_inc_jit.h"

#include "YAC_ObjectArray.h"
#include "YAC_ClassArray.h"
#include "TKS_Mutex.h"
#include "TKS_Time.h"
#include "TKS_Plugin.h"
#include "TKS_PluginRegistry.h"
#include "TKS_ScriptObjectID.h"
#include "TKS_ScriptModule.h"
#include "TKS_ScriptConstraint.h"
#include "TKS_StringIterator.h"
#include "TKS_PluginCallback.h"

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


// Required for Object pools
#include "Pool.h"
#include "ObjectPool.h"

#ifdef DX_TRACEFUNCTIONCALLS
#include "TKS_FunctionCallStackEntry.h"
#endif

#include "EntityTree.h"
#ifdef TKS_EXPORT_ENTITY_SCANNER
#include "PTN_BuiltinFun.h"
#endif // TKS_EXPORT_ENTITY_SCANNER

#include "TKS_Context.h"
#include "TKS_Condition.h"
#include "TKS_Mailbox.h"
#include "TKS_Thread.h"

#include "TKS_FunctionProfiler.h"


static sSI zero_ifos[4]={0,0,0,0};


YAC_Object *yac_null=0;


#ifdef TKS_USE_CHARALLOCATOR
sBool TKS_ScriptEngine::b_allow_char_allocator = YAC_FALSE;
TKS_LinearAllocator *TKS_ScriptEngine::char_allocator = NULL;
#endif // TKS_USE_CHARALLOCATOR



#ifdef TKS_NEWDELETE_INLINE
TKS_ClassTemplate *tkscript__atom_speedpath[YAC_MAX_CLASSES];
#else
#undef TKSCRIPT__ATOM_SPEEDPATH
// Do not use tkscript-> var. in "this", might be uninitialized!
#define TKSCRIPT__ATOM_SPEEDPATH atom_speedpath
#endif


#ifdef DX_RDTSC
sF64 TKS_ScriptEngine::pfc_scale;
#endif // DX_RDTSC


#ifdef RUN_POOL_BENCHMARK
static void debug__run_pool_benchmark(void) {
   //
   // Used to benchmark the ObjectPool feature.
   //
   //  (TODO) move this to a separate file
   //
   YAC_Object **objects = new YAC_Object *[16384];
   sUI i;
   sUI tStart, tEnd, tRand, tPlacement, tPool, tMalloc;
   sUI iter;

   // vcall speed benchmark
   YAC_String *stempl = (YAC_String*) TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_STRING]->template_object;
   objects[0] = YAC_NEW_CORE(YAC_CLID_STRING);
   for(iter=0; iter<3; iter++)
   {
      tStart = getMilliSeconds();
      for(i=0; i<10000000*10; i++)
      {
         // To simulate vcall overhead of yacFinalizeObject call
         stempl->yacPoolInit(objects[0]);
      }
      tEnd = getMilliSeconds();

      printf("[dbg] vcall ms = %d.\n", (tEnd-tStart)/10);
   }
   YAC_DELETE(objects[0]);


   // Benchmark rand()
   tRand = 0x7FFFFFF;
   {
      sUI k;

      for(iter=0; iter<3; iter++)
      {
         srand(0x12345678);
         tStart = getMilliSeconds();

         for(i=0; i<10000000; i++)
         {
            k = rand() & 16383;
         }

         tEnd = getMilliSeconds();
         if( (tEnd-tStart) < tRand )
         {
            tRand = (tEnd-tStart);
         }
         printf("[dbg] rand ms = %d.   k=%08x\n", (tEnd-tStart), k);
      }


   }


   for(i=0; i<16384; i++)
   {
      objects[i] = NULL;
   }

   // Placement new benchmark
   //   vcalls involved:
   //       YAC_Object::yacFinalizeObject()
   tPlacement = 0x7FFFFFFF;
   for(iter=0; iter<3; iter++)
   {
      sU8 *data = new(std::nothrow) sU8[16384*sizeof(YAC_String)];


      srand(0x12345678);
      tStart = getMilliSeconds();

      for(i=0; i<10000000; i++)
      {
         sUI k = rand() & 16383;

         if(NULL != objects[k])
         {
            objects[k]->yacFinalizeObject(NULL);
            objects[k] = NULL;
         }
         else
         {
            objects[k] = &((YAC_String*)data)[k];
            new(objects[k])YAC_String();
         }
      }

      for(i=0; i<16384; i++)
      {
         if(NULL != objects[i])
         {
            objects[i]->yacFinalizeObject(NULL);
            objects[i] = NULL;
         }
      }

      tEnd = getMilliSeconds();

      if( (tEnd-tStart) < tPlacement)
      {
         tPlacement = (tEnd-tStart);
      }

      printf("[dbg] construct ms = %d.\n", (tEnd-tStart));

      delete [] data;
   }

   // ObjectPool benchmark
   //   vcalls involved:
   //         YAC_Host::yacNewPooledByID()
   //       YAC_Object::yacPoolInit()
   //       YAC_Object::yacFinalizeObject()
   tPool = 0x7FFFFFFF;
   for(iter=0; iter<3; iter++)
   {
      srand(0x12345678);
      tStart = getMilliSeconds();

      for(i=0; i<10000000; i++)
      {
         sUI k = rand() & 16383;
         if(objects[k])
         {
            YAC_DELETE(objects[k]);
            objects[k] = NULL;
         }
         else
         {
            objects[k] = YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
         }
      }
      for(i=0; i<16384; i++)
      {
         if(NULL != objects[i])
         {
            YAC_DELETE(objects[i]);
            objects[i] = NULL;
         }
      }
      tEnd = getMilliSeconds();

      if( (tEnd-tStart) < tPool )
      {
         tPool = (tEnd-tStart);
      }

      printf("[dbg] pool ms = %d.\n", (tEnd-tStart));
      ObjectPool::Print();
   }

   // New/delete benchmark
   //   vcalls involved:
   //         YAC_Host::yacNewByID()
   //       YAC_Object::yacNewObject()
   tMalloc = 0x7FFFFFFF;
   for(iter=0; iter<3; iter++)
   {
      srand(0x12345678);
      tStart = getMilliSeconds();

      for(i=0; i<10000000; i++)
      {
         sUI k = rand() & 16383;
         if(objects[k])
         {
            delete objects[k];
            objects[k] = NULL;
         }
         else
         {
            objects[k] = YAC_NEW_CORE(YAC_CLID_STRING);
         }
      }
      for(i=0; i<16384; i++)
      {
         if(NULL != objects[i])
         {
            YAC_DELETE(objects[i]);
            objects[i] = NULL;
         }
      }

      tEnd = getMilliSeconds();

      if( (tEnd-tStart) < tMalloc )
      {
         tMalloc = (tEnd-tStart);
      }
      printf("[dbg] malloc ms = %d.\n", (tEnd-tStart));
   }

   delete [] objects;

   tPlacement -= tRand;
   tMalloc -= tRand;
   tPool -= tRand;

   printf("[dbg] Pool benchmark results:\n");
   printf("[dbg] rand() overhead (subtracted from results): %d ms\n", tRand);
   printf("[dbg]   placement new: %d ms (%3.2f)\n", tPlacement, ((sF32)tPlacement)/tMalloc);
   printf("[dbg]            pool: %d ms (%3.2f)\n", tPool, ((sF32)tPool)/tMalloc);
   printf("[dbg]          malloc: %d ms (1.0)\n", tMalloc);
}
#endif //RUN_POOL_BENCHMARK




TKS_ScriptEngine::TKS_ScriptEngine(void) {

   print_hooks = NULL;
   b_iterating_print_hooks = YAC_FALSE;

#ifdef YAC_OBJECT_COUNTER
#ifdef TKS_MT
   mtx_yac_object_counter = new TKS_Mutex();
#endif // TKS_MT
#endif // YAC_OBJECT_COUNTER

   ObjectPool::Init();

#ifdef TKS_USE_CHARALLOCATOR
   char_allocator         = new TKS_LinearAllocator(TKS_CHARALLOCATOR_STRIPESIZE);
   b_allow_char_allocator = YAC_TRUE;
#endif // TKS_USE_CHARALLOCATOR

   initMilliSeconds();
   tks_empty_string = YAC_New_String();
   b_runtime        = YAC_FALSE;
   b_initclasses    = YAC_FALSE;
   ptn_error_code   = 0;
   ptn_error_loc    = 0u;
   trace_enabled    = YAC_TRUE;
   dtrace_enabled   = YAC_TRUE;
   compiler         = (void*) new TKS_Compiler();

   sUI i;
   sUI j;
   for(i = 0u; i < YAC_MAX_CLASSES; i++)
   {
      TKSCRIPT__ATOM_SPEEDPATH[i] = NULL;
   }
   for(i = 0u; i < YAC_MAX_CLASSES; i++)
   {
      for(j = 0u; j < YAC_MAX_CLASSES; j++)
      {
         cpp_typecast_map[i][j] = YAC_FALSE;  // (todo) memset
      }
   }

   plugins = (void*)new TKS_PluginRegistry();

   initNamespaces();

   exception_types   = NULL;
   next_exception_id = 0;

   // ---- init random seed ----
   ::srand(getStartMilliSeconds());

#ifdef TKS_JIT
   VMCore::InitTables();
#endif
   i_return = 0;

   sig_signal       = NULL;
   sig_segv         = NULL;
   sig_segvcontinue = NULL;

   cached_argc = 0;
   cached_argv = NULL;

   main_thread = NULL;

   tks_lib_init_fxn = NULL;
   tks_lib_exit_fxn = NULL;

#ifdef TKS_VST
   tks_lib_vst_open_editor_fxn       = NULL;
   tks_lib_vst_close_editor_fxn      = NULL;
   tks_lib_vst_set_sample_rate_fxn   = NULL;
   tks_lib_vst_set_bpm_fxn           = NULL;
   tks_lib_vst_process_replacing_fxn = NULL;
   tks_lib_vst_get_bank_chunk_fxn    = NULL;
   tks_lib_vst_get_program_chunk_fxn = NULL;
   tks_lib_vst_set_bank_chunk_fxn    = NULL;
   tks_lib_vst_set_program_chunk_fxn = NULL;
#endif // TKS_VST

   temp_context = NULL;
   main_context = NULL;
   for(i = 0u; i < TKS_MAX_CONTEXTS; i++)
   {
      contexts[i] = NULL;  // (todo) memset
   }

#ifdef DX_SCRIPTCONSTRAINTS
   initScriptConstraints();
#endif // DX_SCRIPTCONSTRAINTS

#ifdef TKS_MT
   initMutexes();
#endif //TKS_MT

}

TKS_ScriptEngine::~TKS_ScriptEngine() {

   freePrintHooks();

   sBool bDebug = (tkscript->configuration.debug_level > 79);
   if(bDebug) ::printf("~TKS_ScriptEngine: freeScriptConstraints()\n");

#ifdef DX_SCRIPTCONSTRAINTS
   freeScriptConstraints();
#endif // DX_SCRIPTCONSTRAINTS

   if(bDebug) ::printf("~TKS_ScriptEngine: clearInstanceNamespace()\n");
   clearInstanceNamespace();

   if(bDebug) ::printf("~TKS_ScriptEngine: free global constants\n");
   global_constant_namespace.freeCache();

   if(bDebug) ::printf("~TKS_ScriptEngine: free uconst\n");
   uconst_table.freeCache();

   if(bDebug) ::printf("~TKS_ScriptEngine: free namespace vars\n");
   freeNamespaceVars();

   if(bDebug) ::printf("~TKS_ScriptEngine: clear template namespace\n");
   clearTemplateNamespace();

   if(bDebug) ::printf("~TKS_ScriptEngine: delete compiler\n");

   // (note) delete String linear allocator but some strings whose char* references it may still exist
   //          not a problem as long as the char* is not dereferenced.
   TKS_Compiler *com = (TKS_Compiler*)compiler;
   delete com;
   compiler = NULL;

   if(bDebug) ::printf("~TKS_ScriptEngine: free pak file\n");
   tkx.cleanUp();

   if(bDebug) ::printf("~TKS_ScriptEngine: freeYacFunctions()\n");
   freeYacFunctions();

   if(bDebug) ::printf("~TKS_ScriptEngine: delete empty string\n");
   YAC_DELETE(tks_empty_string);

   if(bDebug) ::printf("~TKS_ScriptEngine: freeExceptions()\n");
   freeExceptions();

   if(bDebug) ::printf("~TKS_ScriptEngine: freeNamespaceTypes()\n");
   freeNamespaceTypes();

   if(bDebug) ::printf("~TKS_ScriptEngine: delete plugins()\n");
   if(plugins)
   {
      delete (TKS_PluginRegistry*)plugins;
      plugins = NULL;
   }

   if(bDebug) ::printf("~TKS_ScriptEngine: freeCallbacks()\n");
   freeCallbacks();

   if(bDebug) ::printf("~TKS_ScriptEngine: freeContexts()\n");
   freeContexts();

   if(bDebug) ::printf("~TKS_ScriptEngine: freeMutexes()\n");
#ifdef TKS_MT
   freeMutexes();
#endif //TKS_MT

   if(bDebug) ::printf("~TKS_ScriptEngine: freeNamespaces()\n");
   freeNamespaces();

   if(bDebug) ::printf("~TKS_ScriptEngine: delete char_allocator\n");

#ifdef TKS_USE_CHARALLOCATOR
   b_allow_char_allocator = YAC_FALSE;
   //::printf("::~TKS_ScriptEngine 18b\n");
   delete char_allocator;
   char_allocator = NULL;
#endif // TKS_USE_CHARALLOCATOR

   if(bDebug) ::printf("~TKS_ScriptEngine: ObjectPool::Exit\n");

   ObjectPool::Exit();

   //::printf("::~TKS_ScriptEngine 20\n");

#ifdef TKS_DCON
   if(configuration.debug_level)
   {
      ::printf("[...] TKS_ScriptEngine::~TKS_ScriptEngine()\n");
   }
#endif // TKS_DCON

   //::printf("::~TKS_ScriptEngine 21\n");

   exitMilliSeconds();

#ifdef YAC_OBJECT_COUNTER
#ifdef TKS_MT
   //::printf("::~TKS_ScriptEngine 21b\n");
   delete mtx_yac_object_counter;
   mtx_yac_object_counter = NULL;
#endif // TKS_MT
#endif // YAC_OBJECT_COUNTER

   if(bDebug) ::printf("~TKS_ScriptEngine: LEAVE\n");
}

void TKS_ScriptEngine::freeYacFunctions(void) {
   if(yac_functions.objects)
   {
      for(sUI i = 0u; i < yac_functions.max_entries; i++)
      {
         if(yac_functions.objects[i])
         {
            YAC_CommandY *c = (YAC_CommandY*)yac_functions.objects[i]->value.object_val;
            if(c)
            {
               yac_functions.objects[i]->value.object_val=0;
               delete c;
            }
         }
      }
      yac_functions.freeCache();
   }
}

void TKS_ScriptEngine::initProfiler(void) {
#if defined(YAC_WIN32) && !defined(YAC_GCC)

   if( ::QueryPerformanceFrequency(&pfcFrequency) == 0 )
   {
      pfcFrequency.LowPart  = 0;
      pfcFrequency.HighPart = 0;
   }

#ifdef DX_RDTSC
   // Calibration loop
   if(configuration.debug_level > 1)
      ::printf("[dbg] [RDTSC] Calibrating update rate..");
   LARGE_INTEGER calS;
   LARGE_INTEGER cycS;
   ::QueryPerformanceCounter(&calS);
   __asm {
      RDTSC
         mov cycS.HighPart, edx
         mov cycS.LowPart, eax
   }

   ::Sleep(100);

   LARGE_INTEGER calE;
   LARGE_INTEGER cycE;
   __asm {
      RDTSC
         mov cycE.HighPart, edx
         mov cycE.LowPart, eax
   }
   ::QueryPerformanceCounter(&calE);
   pfc_scale = ( ((sF64)calE.QuadPart) - ((sF64)calS.QuadPart) ) / ( ((sF64)cycE.QuadPart) - ((sF64)cycS.QuadPart) );

   if(configuration.debug_level > 1)
      ::printf(".done! scale=%f\n", pfc_scale);
#endif // DX_RDTSC
#endif // YAC_WIN32
}

void TKS_ScriptEngine::initContexts(void) {
   temp_context = new TKS_Context();
   temp_context->b_running = 1;
   temp_context->use();
   main_context = new TKS_Context();
   main_context->b_running = 1;
   main_context->use();
}

void TKS_ScriptEngine::freeContexts(void) {
   if(NULL != main_context)
   {
      delete main_context;
      main_context = NULL;
   }
   if(NULL != temp_context)
   {
      delete temp_context;
      temp_context = NULL;
   }
   for(sUI i = 0u; i < TKS_MAX_CONTEXTS; i++)
   {
      if(NULL != contexts[i])
      {
         delete contexts[i];
         contexts[i] = NULL;
      }
   }

   if(NULL != main_thread)
   {
      YAC_DELETE(main_thread);
      main_thread = NULL;
   }
}

TKS_Context *TKS_ScriptEngine::lockGlobalContext(void) {
   if(configuration.b_lock_global_context)
   {
#ifdef TKS_MT
      mtx_global->lock();
#endif //TKS_MT
   }
   // ////temp_context->use();
   // //return temp_context; // global temp context
   // XXX not thread safe !!
   return main_context; // Must use main context to handle exceptions! xxx use lockCurrentContext() instead, use thread local storage!
}

void TKS_ScriptEngine::unlockGlobalContext(void) { // the global context is protected by the mtx_global mutex
   if(configuration.b_lock_global_context)
   {
      // xxx re-init contexts[1]
#ifdef TKS_MT
      mtx_global->unlock();
#endif //TKS_MT
   }
}


TKS_ClassTemplate *TKS_ScriptEngine::findTemplate(YAC_String *_templ) {
   TKS_CachedObject *cs=global_template_namespace.findEntry(_templ);
   if(cs) return (TKS_ClassTemplate*)cs->value.object_val;
   return 0;
}

YAC_Object *YAC_VCALL TKS_ScriptEngine::yacGetClassTemplateByID(sUI _id) {
   if(_id<YAC_MAX_CLASSES)
   {
      TKS_ClassTemplate *t=TKSCRIPT__ATOM_SPEEDPATH[_id];
      if(t)
         return t->template_object;
   }
   return 0;
}

void TKS_ScriptEngine::clearTemplateNamespace(void) {
   sU16 i;
   for(i=0; i<YAC_MAX_CLASSES; i++)
   {
      TKSCRIPT__ATOM_SPEEDPATH[i] = NULL;
   }
   global_template_namespace.freeCache();

}

sUI YAC_VCALL TKS_ScriptEngine::yacRegisterClass(YAC_Object *_template, sUI _may_instantiate) {
   _template->class_ID=next_plugin_clid++;
   if(!next_plugin_clid)
   {
#ifdef TKS_DCON
      TKS_ScriptEngine::printf("[---] registerPluginClass: cannot handle more than %i plugins.\n",
         TKS_MAX_PLUGINS);
#endif
      return 0;
   }
#ifdef TKS_DCON
   if(tkscript->configuration.debug_level>63)
      TKS_ScriptEngine::printf("[...] registerPluginClass: assigning class_ID %i. template@%08x\n", _template->class_ID, (void*)_template);
#endif
   if(registerClass(_template, _may_instantiate, 1))
   {
      return _template->class_ID;
   }
   else
   {
      return 0;
   }
}

sBool TKS_ScriptEngine::registerClass(YAC_Object *_template, sUI _mayinstantiate, sBool _isplugin) {
   if(_template)
   {
      TKS_ClassTemplate *ctl = new TKS_ClassTemplate();
      ctl->isplugin=_isplugin;
      YAC_String cls; cls.visit((char*)_template->yacClassName());
#ifdef TKS_DCON
      if(configuration.debug_level > 63)
      {
         TKS_ScriptEngine::printf("registerClass(" YAC_PRINTF_PTRPREFIX "%p,%i) name=\"%s\"\n", (void*)_template, _mayinstantiate,
                                  (char*)cls.chars);
      }
#endif
      TKS_CachedObject *tm=global_template_namespace.createObjectEntry(&cls, 1, ctl, 1);
      if(tm)
      {
         // ---- new style plugin interface, read YInG reflection map ----
         sBool r2= ctl->setTemplateY(_template);
         if(r2)
         {
            sUI clid=_template->class_ID;
            if(clid==YAC_CLID_STRING) tm->type=YAC_TYPE_STRING;
            else                      tm->type=YAC_TYPE_OBJECT;
            TKSCRIPT__ATOM_SPEEDPATH[clid]=ctl;

            // ---- get constants (old style, "MYCONST:42 MYCONST2:$2b" ...)
            YAC_String cons;
            _template->yacGetConstantStringList(&cons);
            if(!cons.isBlank())
            {
               TKS_StringIterator is;
               cons.words(0);
               is.begin(&cons);
               if(is.tail())
                  do
                  {
                     YAC_String cn;
                     YAC_String cv;
                     sSI ddi=is.current->indexOf(":");
                     if(ddi!=-1)
                     {
                        // ---- extract constant-name
                        is.current->substring(&cn, 0, ddi);
                        //cn.toUpper();
                        is.current->substring(&cv, ddi+1, is.current->length-ddi-1);
                        sSI iv;
                        sF32 fv;
                        switch(cv.checkConversions())
                        {
                        case 1:
                           if(cv.yacScanI(&iv))
                           {
                              TKS_CachedObject *co=global_constant_namespace.createIntegerEntry(&cn, 1);
                              if(!co)
                              {
#ifdef TKS_DCON
                                 TKS_ScriptEngine::printf("\n[---] TKS_ScriptEngine::registerClass: failed to create CONSTANT integer instance \"%s\".\n",
                                    (char*)is.current->chars);
#endif
                                 return 0;
                              }
                              co->type=YAC_TYPE_INT;
                              co->value.int_val=iv;
                           }
                           else
                           {
#ifdef TKSDCON
                              TKS_ScriptEngine::printf("\n[---] TKS_ScriptEngine::registerClass: \"%s\"::\"%s\" failed to scan int CONSTANT.\n",
                                 (char*)cls.chars, (char*)is.current->chars);
#endif
                              return 0;
                           }
                           break;
                        case 2:
                           if(cv.yacScanF32(&fv))
                           {
                              TKS_CachedObject *co=global_constant_namespace.createIntegerEntry(&cn, 1);
                              if(!co)
                              {
#ifdef TKS_DCON
                                 TKS_ScriptEngine::printf("\n[---] TKS_ScriptEngine::registerClass: failed to create CONSTANT float instance \"%s\".\n",
                                    (char*)is.current->chars);
#endif
                                 return 0;
                              }
                              co->type=YAC_TYPE_FLOAT;
                              co->value.float_val=fv;
                           }
                           else
                           {
#ifdef TKS_DCON
                              TKS_ScriptEngine::printf("\n[---] TKS_ScriptEngine::registerClass: \"%s\"::\"%s\" failed to scan int CONSTANT.\n",
                                 (char*)cls.chars, (char*)is.current->chars);
#endif
                              return 0;
                           }
                           break;
                        default:
#ifdef TKS_DCON
                           TKS_ScriptEngine::printf("\n[---] TKS_ScriptEngine::registerClass: \"%s\"::\"%s\" failed to scan unknown CONSTANT.\n",
                              (char*)cls.chars, (char*)is.current->chars);
#endif
                           return 0;
                        }
                     }
                  } while(is.previous());
                  is.end();
                  cons.freeStack();
            }

            // ---- get constants (new style, supports int/float constant types) ----
            sUI numc=_template->yacConstantGetNum();
            if(numc)
            {
               sUI i;
               const char** all_const_names = _template->yacConstantGetNames();
               const sUI  * all_const_types = _template->yacConstantGetTypes();
               yacmemptr    all_const_values= _template->yacConstantGetValues();
               YAC_String cn;
               for(i=0; i<numc; i++)
               {
                  cn.visit((char*)all_const_names[i]);
                  TKS_CachedObject *co=global_constant_namespace.createIntegerEntry(&cn, YAC_TRUE/*bCopyName*/);
                  if(all_const_types[i]==1)
                     co->initInt(all_const_values.s32[i]);
                  else
                     co->initFloat(all_const_values.f32[i]);
               }
            }

            ctl->may_instantiate = _mayinstantiate;
            ctl->static_co.flags=0;
            ctl->static_co.deleteme=0;
            ctl->static_co.type=3;
            ctl->static_co.value.object_val=_template;
            return 1;//tm;
       }
       else
       {
#ifdef TKS_DCON
          TKS_ScriptEngine::printf("\n[---] failed to register class \"%s\".\n", (char*)(cls.chars));
#endif
       }
   }
    }
    return 0;
}


sBool TKS_ScriptEngine::init(void) {

   // ---- allocate C++-template and const/variable memory ----
   global_template_namespace.allocCache(YAC_MAX_CLASSES + 1);
   global_instance_namespace.allocCache(TKS_MAX_GLOBAL_VARIABLES + 1);
   global_constant_namespace.allocCache(TKS_MAX_GLOBAL_CONSTANTS + 1);
   uconst_table.allocCache(TKS_MAX_ENUM_CONSTANTS + 1);
   yac_functions.allocCache(TKS_C_MAXEXTFUNCTIONS + 1);
   tksvm_zero_args.s32 = zero_ifos;

   initCallbacks();

   initContexts();

#ifdef DX_SCRIPTCONSTRAINTS
   initScriptConstraints(); // (TODO) already done in c'tor ?!!
#endif // DX_SCRIPTCONSTRAINTS

   initExceptions();

   // Pseudo tokens
   s_true .visit("true");
   s_false.visit("false");


#ifdef TKS_EXPORT_ENTITY_SCANNER
   exportPseudoTokenEntityTree();
#endif // TKS_EXPORT_ENTITY_SCANNER

   // ---- register core classes and create static variables ----
   if(registerBuiltinClasses())
   {
      // Setup main thread object
      main_thread = (TKS_Thread*) yacNewByID(TKS_CLID_THREAD);
      main_thread->initMain();
      main_thread->context = main_context;
#ifndef YAC_FORCE_NO_TLS
      tks_current_thread = main_thread;
#endif // HAVE_TLS

      return YAC_TRUE;
   }
   else
   {
      return YAC_FALSE;
   }
}

#ifdef TKS_EXPORT_ENTITY_SCANNER
void TKS_ScriptEngine::exportPseudoTokenEntityTree(void) {

   printf("TKS_ScriptEngine::exportPseudoTokenEntityTree()\n");

   EntityTree entityTree;

   // Builtin functions
   entityTree.addEntity("sin",       TKS_BFUN_SIN);
   entityTree.addEntity("cos",       TKS_BFUN_COS);
   entityTree.addEntity("tan",       TKS_BFUN_TAN);
   entityTree.addEntity("tanh",      TKS_BFUN_TANH);
   entityTree.addEntity("asin",      TKS_BFUN_ASIN);
   entityTree.addEntity("acos",      TKS_BFUN_ACOS);
   entityTree.addEntity("sqrt",      TKS_BFUN_SQRT);
   entityTree.addEntity("rad",       TKS_BFUN_RAD);
   entityTree.addEntity("deg",       TKS_BFUN_DEG);
   entityTree.addEntity("abs",       TKS_BFUN_ABS);
   entityTree.addEntity("frac",      TKS_BFUN_FRAC);
   ////entityTree.addEntity("tcint",     TKS_BFUN_TCINT); // xxx obsolete
   ////entityTree.addEntity("tcfloat",   TKS_BFUN_TCFLOAT);
   ////entityTree.addEntity("tcstr",     TKS_BFUN_TCSTR);
   ////entityTree.addEntity("tcobject",  TKS_BFUN_TCOBJECT);
   ////entityTree.addEntity("tcpointer", TKS_BFUN_TCPOINTER); // xxx obsolete
   entityTree.addEntity("round",     TKS_BFUN_ROUND);
   entityTree.addEntity("floor",     TKS_BFUN_FLOOR);
   entityTree.addEntity("ceil",      TKS_BFUN_CEIL);
   entityTree.addEntity("sign",      TKS_BFUN_SIGN);
   entityTree.addEntity("rand",      TKS_BFUN_RND);
   entityTree.addEntity("rnd",       TKS_BFUN_RND); // alias for rand..
   entityTree.addEntity("2n",        TKS_BFUN_2N);
   entityTree.addEntity("tcchar",    TKS_BFUN_TCCHR);
   entityTree.addEntity("typeid",    TKS_BFUN_TYPEID);
   entityTree.addEntity("typename",  TKS_BFUN_TYPENAME);
   entityTree.addEntity("log",       TKS_BFUN_LOG);
   entityTree.addEntity("log2",      TKS_BFUN_LOG2);
   entityTree.addEntity("exp",       TKS_BFUN_EXP);

   // Builtin functions that take more than one argument
   entityTree.addEntity("rgb",     TKS_BFUN2_RGB);
   entityTree.addEntity("argb",    TKS_BFUN2_ARGB);
   entityTree.addEntity("rgba_4b", TKS_BFUN2_RGBA_4B);

   // Builtin procedures
   entityTree.addEntity("stdout",   TKS_BPROC_STDOUT);
   entityTree.addEntity("stderr",   TKS_BPROC_STDERR);
   entityTree.addEntity("dcon",     TKS_BPROC_DCON);
   entityTree.addEntity("die",      TKS_BPROC_DIE);
   entityTree.addEntity("print",    TKS_BPROC_PRINT);
   entityTree.addEntity("trace",    TKS_BPROC_TRACE);
   entityTree.addEntity("dtrace",   TKS_BPROC_DTRACE);
   entityTree.addEntity("stdflush", TKS_BPROC_STDFLUSH);
   entityTree.addEntity("clamp",    TKS_BPROC_CLAMP);
   entityTree.addEntity("wrap",     TKS_BPROC_WRAP);

   entityTree.exportToC("autogen_tks_pseudotoken_entitytree_lookup.cpp", "tks_get_pseudotoken_id_by_name");
   printf("[...] wrote \"autogen_tks_pseudotoken_entitytree_lookup.cpp\"\n");
}
sUI tks_get_pseudotoken_id_by_name(YAC_String *_sv) {
   while(1); // re-run after create the real function body !
}
#else
#include "autogen_tks_pseudotoken_entitytree_lookup.cpp"
#endif // TKS_EXPORT_ENTITY_SCANNER


sBool TKS_ScriptEngine::init2(void) {

   // (note) always init profiler b/c of milliSecondsFloat() API function
   // // if(configuration.b_enable_statement_profiling || configuration.b_enable_function_profiling)
   // // {
      initProfiler();
   // // }

   return 1;
}

#ifdef DX_SCRIPTCONSTRAINTS
void TKS_ScriptEngine::initScriptConstraints(void) {
   script_constraints.allocCache(TKS_MAX_CONSTRAINTS + 1);
}

void TKS_ScriptEngine::freeScriptConstraints(void) {
   script_constraints.freeCache();
}

sBool TKS_ScriptEngine::addScriptConstraint(TKS_ScriptConstraint *_scs) {
   TKS_CachedObject *co=script_constraints.findEntry(&_scs->name);
   if(co)
   {
#ifdef TKS_DCON
      printf("[---] ScriptEngine::addScriptConstraint(): constraint \"%s\" already exists.\n",
         (char*)_scs->name.chars);
#endif
   }
   else
   {
      script_constraints.createObjectEntry(&_scs->name, 0, _scs, 1);
      return 1;
   }
   return 0;
}

TKS_ScriptConstraint *TKS_ScriptEngine::findScriptConstraint(YAC_String *_name) {
   if(_name)
   {
      TKS_CachedObject *co=script_constraints.findEntry(_name);
      if(co)
      {
         return (TKS_ScriptConstraint*)co->value.object_val;
      }
   }
   return 0;
}
#endif // DX_SCRIPTCONSTRAINTS


void TKS_ScriptEngine::handleCompiletimeError(sUI _srcloc, sUI _errorcode) {
   //
   // Obsolete error handler? This is only used for pre-runtime / compile time errors
   //  (PTNERR_CANTCOMPILE, PTNERR_UFUNMISSINGBODY, PTNERR_MODULEMEMBERNOTFOUND,
   //   PTNERR_MODULENOTFOUND, PTNERR_MODULEMETHODNOTFOUND)
   //
   //  also see Dcterror()
   //
   //  ==> use Drtthrow() at run time !
   //
   ptn_error_loc  = _srcloc;
   ptn_error_code = (sU16) _errorcode;

   if(!tkscript->b_runtime)  // [02Jan2018] avoid prg shutdown when temp script fails to compile
   {
      // xxx TKS_MT: stop all threads
      main_context->b_running = 0;
#ifdef TKS_DCON
      const char *errName = PTN_Node::GetErrorByCode(ptn_error_code);
      printf("[---] runtime error %i \"%s\" in module \"%s\" line %i.\n",
             ptn_error_code, errName,
             Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(ptn_error_loc)),
             Dsrcloc_linenr(ptn_error_loc)
             );

      // xxx TKS_MT: print all contexts
      //  (TODO) this is useless since this should be only called at compile time
      tkscript->printAllFunctionCallStacks();  // xxx TKS_MT synchronize
#endif

      // Stop all script thread contexts
      //  (TODO) this is useless since this should be only called at compile time
      for(sUI i = 0u; i < TKS_MAX_CONTEXTS; i++)
      {
         if(NULL != contexts[i])
            contexts[i]->b_running = YAC_FALSE;
      }
      b_runtime = YAC_FALSE; // xxx TKS_MT: use synchronized yacStop() call
      tkscript->i_return = 10;
   }
}

void TKS_ScriptEngine::checkCompiletimeError(void) {
   // (TODO) obsolete ??
   if(ptn_error_code)
   {
#ifdef TKS_DCON
      /*TKS_ScriptEngine::printf("\n[---] runtime error %i \"%s\" in module \"%s\" line %i.\n",
      ptn_error_code, PTN_Node::GetErrorByCode(ptn_error_code),
      ((TKS_Compiler*)compiler)->getModuleNameByIndex(Dsrcloc_module(ptn_error_loc)),
         Dsrcloc_linenr(ptn_error_loc)); */
#endif
      b_runtime = YAC_FALSE;
      tkscript->i_return = 10;
   }
}

void TKS_ScriptEngine::printAllFunctionCallStacks(void) {
#ifdef DX_TRACEFUNCTIONCALLS
   main_context->printFunctionCallStack();
   for(sUI i = 0u; i < TKS_MAX_CONTEXTS; i++)
   {
      if(NULL != contexts[i])
         contexts[i] -> printFunctionCallStack();
   }
#endif // DX_TRACEFUNCTIONCALLS
}

void TKS_ScriptEngine::runGlobalModuleStatements(PTN_Env *_env) {
   // ---- run initializers, global module statements ----
   // ---- has to be done on real app start (classes!) ----
   StaticList::Node *cn = ((TKS_Compiler*)compiler)->compiled_modules;
   cn = StaticList::Tail(cn);
   _env->context->b_running = YAC_TRUE;

   while(cn && _env->continue_flag && _env->context->b_running)
   {
      TKS_CachedScript *cs = (TKS_CachedScript*)cn->data;
      if(cs && !cs->b_global_module_statements_done)
      {
         cs->b_global_module_statements_done = YAC_TRUE;

         cs->parser_tree->evalFirst(_env);
         _env->cret.unset();

         if(Dhaveexception)
         {
            Dprintf("[---] exception thrown while running global statements of module \"%s\" (file \"%s\").\n",
               (char*)cs->name.chars, (char*)cs->src_name.chars);
            _env->context->b_running = YAC_FALSE;
            tkscript->i_return = 10;
            Dhandleexception(NULL);

//#ifndef TKS_PRINTEXCEPTION
//            // Print the exception in case it has not been printed by handleException()
//            _env->context->printException();
//#endif // TKS_PRINTEXCEPTION

         }
      }
      cn = cn->previous;
   }
}

void TKS_ScriptEngine::lockMtxPrint(void) {
#ifdef TKS_MT
   mtx_print->lock();
#endif //TKS_MT
}

void TKS_ScriptEngine::unlockMtxPrint(void) {
#ifdef TKS_MT
   mtx_print->unlock();
#endif //TKS_MT
}

void YAC_VCALL TKS_ScriptEngine::yacPrint(const sChar *_s) {
   // (note) called by plugins via Dyac_host_printf (plugin-local YAC_Host::printf() then redirects to yacPrint())
   print(_s, 0);
}

void TKS_ScriptEngine::freePrintHooks(void) {
   if(NULL != print_hooks)
   {
      TKS_PrintHook *c = print_hooks;
      while(NULL != c)
      {
         TKS_PrintHook *n = c->next;
         delete c;
         c = n;
      }
      print_hooks = NULL;
   }
}

void TKS_ScriptEngine::yacPrintHookAdd(yac_printhook_fxn_t _fxn, YAC_String *_output, void *_userdata) {
   lockMtxPrint();
   TKS_PrintHook *h = new(std::nothrow)TKS_PrintHook;
   if(NULL != h)
   {
      h->fxn      = _fxn;
      h->output   = _output;
      h->userdata = _userdata;
      h->next     = print_hooks;
      print_hooks = h;
   }
   unlockMtxPrint();
}

void TKS_ScriptEngine::yacPrintHookRemove(yac_printhook_fxn_t _fxn, YAC_String *_output, void *_userdata) {
   lockMtxPrint();
   TKS_PrintHook *c = print_hooks;
   TKS_PrintHook *p = NULL;
   while(NULL != c)
   {
      if(c->fxn == _fxn && (void*)c->output == (void*)_output && c->userdata == _userdata)
      {
         if(NULL != p)
            p->next = c->next;
         else
            print_hooks = c->next;
         delete c;
         break;
      }
      p = c;
      c = c->next;
   }
   unlockMtxPrint();
}

void TKS_ScriptEngine::callPrintHooks(YAC_String *_s) {
   // (note) mtx_print already locked by caller
   b_iterating_print_hooks = YAC_TRUE;

   TKS_PrintHook *c = print_hooks;
   while(NULL != c)
   {
      c->fxn(c->output, c->userdata, _s);
      c = c->next;
   }

   b_iterating_print_hooks = YAC_FALSE;
}

void TKS_ScriptEngine::print(const char *_s, sSI _sid) {
   // (note) called by e.g. PTN_PrintStat (trace, print, stdout, stderr, die)
   lockMtxPrint();
   if(_s)
   {
      if(NULL == print_hooks || configuration.debug_level > 0)
      {
         switch(_sid)
         {
            case 0: // CONSOLE, STDOUT
            case 1:
               tks_fputs(_s);
               break;
            case 2: // STDERR
            case 3: // DIE
               tks_fputs2(_s);
               break;
         }
      }

      if(NULL != print_hooks &&  !b_iterating_print_hooks)
      {
         b_iterating_print_hooks = YAC_TRUE;
         YAC_String s;
         s.visit(_s);
         callPrintHooks(&s);
         b_iterating_print_hooks = YAC_FALSE;
      }
   }

   unlockMtxPrint();
}

void TKS_ScriptEngine::printf(const char *_fmt, ...) {
   if(configuration.b_enableprint && _fmt)
   {
      lockMtxPrint();

      static char buf[128*1024];
      va_list va;
      va_start(va, _fmt);
#ifdef YAC_VC
      _vsnprintf((char*)buf, 128*1024, _fmt, va);
#else
#ifdef HAVE_VSNPRINTF
      vsnprintf((char*)buf, 128*1024, _fmt, va);
#else
#warning "compiling without VSNPRINTF() support\n"
      vsprintf((char*)buf, _fmt, va);
#endif
#endif
      ////vsprintf(buf,/*, sizeof(buf)/sizeof(char),*/ _fmt, va);
      va_end(va);

      if(NULL == print_hooks || configuration.debug_level > 0)
      {
         ::fputs(buf, stdout);
      }

      if(NULL != print_hooks && !b_iterating_print_hooks)
      {
         b_iterating_print_hooks = YAC_TRUE;
         YAC_String s;
         s.visit(buf);
         callPrintHooks(&s);
         b_iterating_print_hooks = YAC_FALSE;
      }

      unlockMtxPrint();
   }
}

//
//
//
//
//
//

void tks_fputs2(const char *_string) {
   // print to fileno 2 (stderr)
   if(NULL != _string)
   {
      ::fputs(_string, stderr);
      ::fflush(stderr);
   }
}

void tks_fputs(const char* _string) {
   // print to fileno 0 (stdout)
   if(NULL != _string)
   {
      ::fputs(_string, stdout);
      ::fflush(stdout);
   }
}

void tks_fputc(char _c) {
   ::fputc(_c, stdout);
}

void TKS_ScriptEngine::handleFatalError(void) {
   printf("[---] fatal error occured, exiting..\n");
   exit(20);
}

void TKS_ScriptEngine::emitExplanations(void) {
#ifdef TKS_DCON
   // Write project name
   tks_fputs((const char*)tkx.str_prjname.chars);
   tks_fputc(CC_SEPARATOR);

   StaticList::Node *cn = ((TKS_Compiler*)compiler)->compiled_modules;
   cn=StaticList::Tail(cn);
   sUI i, j;
   while(NULL != cn)
   {
      TKS_CachedScript *cs = (TKS_CachedScript*)cn->data;
      if(cs)
      {
         // Emit module documentation
         tks_fputc(CC_MODULE);
         tks_fputs((const char*)cs->name.chars);
         tks_fputc(CC_SEPARATOR); // newline separates module name and documentation
         if(!cs->documentation.isBlank())
         {
            tks_fputs((const char*)cs->documentation.chars);
         }

         // Emit function documentation
         for(i = 0u; i < cs->functions.max_entries; i++)
         {
            TKS_CachedObject *co = cs->functions.objects[i];
            if(co)
            {
               PTN_Function *f = (PTN_Function*) co->value.any;
               tks_fputc(CC_FUNCTION);
               f->emitExplanation();
            }
         }

         // Now find all classes that have been declared in the current module
         for(i = 1u; i < ((TKS_Compiler*)compiler)->class_decls.num_elements; i++)
         {
            TKS_ClassDecl *cd = ((TKS_Compiler*)compiler)->class_decls.elements[i];
            if(cd->isUsed())
            {
               if(cd->module == cs)
               {
                  // Emit class documentation
                  tks_fputc(CC_CLASS);
                  if(cd->nspace != &Dtkscompiler->default_namespace)
                  {
                     tks_fputs((const char*)cd->nspace->name.chars); // xxx handle namespace in DOG
                     tks_fputs("::");
                  }
                  tks_fputs((const char*)cd->name.chars);

                  // Emit comma separated ancestor class list (if any)
                  if(cd->ancestors.num_elements)
                  {
                     j = 0;
                     do
                     {
                        TKS_ClassDecl *anc = cd->ancestors.elements[j++];
                        tks_fputc((int)',');
                        if(anc->nspace != &Dtkscompiler->default_namespace)
                        {
                           tks_fputs((const char*)anc->nspace->name.chars); // xxx handle namespace in DOG
                           tks_fputs("::");
                        }
                        tks_fputs((const char*)anc->name.chars);
                     } while(j < cd->ancestors.num_elements);
                  }
                  tks_fputc(CC_SEPARATOR); // write name/documentation separator
                  if(! cd->documentation.isBlank())
                  {
                     tks_fputs((const char*)cd->documentation.chars);
                  }

                  // Emit non-static method documentation
                  for(j=0; j<cd->methods.num_elements; j++)
                  {
                     TKS_ClassDeclMethod *m = cd->methods.elements[j];
                     tks_fputc(CC_METHOD);
                     m->function.emitExplanation();
                  }

                  // Emit static method documentation
                  if(NULL != cd->functions)
                  {
                     for(j=0; j<cd->functions->max_entries; j++)
                     {
                        TKS_CachedObject *co = cd->functions->objects[j];
                        if(co)
                        {
                           PTN_Function *f = (PTN_Function*) co->value.any;
                           tks_fputc(CC_FUNCTION);
                           f->emitExplanation();
                        }
                     }
                  }

                  // Emit class constant documentation
                  if(NULL != cd->constants)
                  {
                     for(j=0; j<cd->constants->max_entries; j++)
                     {
                        TKS_CachedObject *co_val = cd->constants->objects[j];
                        if(NULL != co_val)
                        {
                           TKS_CachedObject *co_doc;
                           if(NULL != cd->constants_documentation)
                           {
                              co_doc = cd->constants_documentation->findEntry(&co_val->name);
                           }
                           else
                           {
                              co_doc = NULL;
                           }
                           tks_fputc(CC_CONSTANT);
                           tks_fputs((const char*)co_val->name.chars);
                           tks_fputc(',');
                           switch(co_val->type)
                           {
                              case YAC_TYPE_VOID:
                                 Dprintf("[---] internal error: class constant with VOID type found while emitting explanation.\n");
                                 break;
                              case YAC_TYPE_INT:
                                 tks_fputs("int");
                                 break;
                              case YAC_TYPE_FLOAT:
                                 tks_fputs("float");
                                 break;
                              case YAC_TYPE_OBJECT:
                                 if(YAC_VALID(co_val->value.object_val))
                                 {
                                    const char *metaClassName = co_val->value.object_val->yacMetaClassName();
                                    if(NULL != metaClassName)
                                    {
                                       tks_fputs((const char*)metaClassName);
                                    }
                                    else
                                    {
                                       tks_fputs((const char*)co_val->value.object_val->yacClassName());
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
                           tks_fputc(CC_SEPARATOR);
                           if(NULL != co_doc)
                           {
                              if(!co_doc->value.string_val->isBlank())
                              {
                                 tks_fputs((const char*)co_doc->value.string_val->chars);
                              }
                           }

                        } // NULL != co_val
                     } // loop constants
                  } // if constants

                  // Emit end of class control character
                  tks_fputc(CC_END);

               } // if class was declared within current module
            } // if cd->isUsed()
         } // loop classes

         // Emit end of module control character
         tks_fputc(CC_END);
      }
      cn = cn->previous;
   } // loop modules
#endif // TKS_DCON
}

void YAC_VCALL TKS_ScriptEngine::yacSendUserEvent(YAC_Object *_o) {
   // (note) see tksdl[2] plugin
   (void)_o;

   // // Dtksapplication->tksinter_sendUserEvent(_o);
}

sUI YAC_VCALL TKS_ScriptEngine::yacMilliSeconds(void) {
   return getMilliSeconds();
}

sUI YAC_VCALL TKS_ScriptEngine::yacRunning(void) {
   /*if(NULL != _context)
   {
      TKS_Context *context = (TKS_Context*)_context;
      return (sUI)(!ptn_error_code) && context->b_running;
   }
   else
   {
      return 0;
   }*/
   ////return (sUI) (!ptn_error_code) && main_context->b_running;
   return (sUI) main_context->b_running;  // [02Jan2018] keep running after tempscript compile error (at runtime)
}


YAC_Object *TKS_ScriptEngine::findTemplateObjectByUniqueClassName(YAC_String *_s) {
   //
   // Mainly used for deserialization (Pool, ObjectArray)
   //
   // The following classname types are accepted:
   //    "MyClass" - regular C++ class
   //    "@MyClass" - script class in default/global namespace
   //    "@mynamespace:MyClass" - script class in user defined namespace
   //
   if(_s)
   {
      if(_s->chars)
      {
         if(_s->chars[0]=='@')
         {
            /// ---- script class ----
            YAC_String nspaceName;
            YAC_String className;
            sSI idxNSP;
            TKS_Namespace *nspace;
            idxNSP = _s->indexOf(':', 0);
            if(-1 != idxNSP)
            {
               // Extrace namespace name
               _s->substring(&nspaceName, 1, idxNSP-1);
               nspace = findNamespace(&nspaceName);
               if(NULL == nspace)
               {
                  Dprintf("[---] ScriptEngine::findTemplateObjectByUniqueClassName: unable to resolve namespace \"%s\".\n",
                     (char*)nspaceName.chars
                     );
                  return 0;
               }
               // Extract class name
               _s->substring(&className, idxNSP, _s->length-1-idxNSP);
            }
            else
            {
               // Use global namespace, extract class name
               nspace = NULL;
               _s->substring(&className, 1, _s->length-1);
            }

#ifdef TKS_DCON
            TKS_ScriptEngine::printf("[dbg] find user class \"%s::%s\"\n",
               (char*)nspaceName.chars,
               (char*)className.chars
               );
#endif

            TKS_ClassDecl *cd = ((TKS_Compiler*)compiler)->findClassDeclNamespace(&className, nspace);
            if(cd)
            {
               return cd->class_template;
            }
         }
         else
         {
            /// ---- c++ class ---
            TKS_ClassTemplate *clt = findTemplate(_s);
            if(clt)
            {
               return clt->template_object;
            }
         }

#ifdef TKS_DCON
         TKS_ScriptEngine::printf("[---] findTemplateObjectByUniqueClassName(\"%s\") failed.\n",
            _s->chars);
#endif
         return 0;
      }
   }
#ifdef TKS_DCON
   TKS_ScriptEngine::printf("[---] findTemplateObjectByUniqueClassName(\"\") failed.\n");
#endif
   return 0;
}

YAC_Object *TKS_ScriptEngine::deserializeNewObject(YAC_Object *_ifs, YAC_Object *_template) {
   YAC_String cln;
   _ifs->yacStreamReadString(&cln, TKS_MAX_CLASSNAMELENGTH);
   YAC_Object *r=0;
   YAC_Object *o_template=0;

   if(cln.compare("Class"))
   {
      o_template = _template; // try suggested _template instead of class base class..
   }

   /// ---- try to find c++ class ----
   if(!o_template)
   {
      TKS_ClassTemplate *clt=findTemplate(&cln);
      if(clt)
      {
         o_template=clt->template_object;
      }
   }

   // ---- try to find script class ----
   if(!o_template)
   {
      // xxx DEAD CODE ???
      TKS_ClassDecl *cd=((TKS_Compiler*)compiler)->findClassDeclNamespace(&cln, NULL);
      if(cd)
      {
         o_template=cd->class_template;
      }
   }

   if(!o_template)
   {
      // ---- unknown stream object, try suggested _template.
      o_template=_template;
   }

   if(o_template)
   {
      r = YAC_CLONE_POOLED(NULL, o_template);
      if(r)
      {
         r->yacDeserialize(_ifs, 0); // skip classname check
      }
   }
   return r;
}

void TKS_ScriptEngine::clearInstanceNamespace(void) {
   global_instance_namespace.freeCache();
}

void TKS_ScriptEngine::compileRun(sBool _bRunInBG) {

   if(configuration.debug_level)
   {
#ifdef TKS_DCON
      TKS_ScriptEngine::printf("[...] using module path \"%s\".\n", (char*)configuration.module_path.chars);
      TKS_ScriptEngine::printf("[...] using library path \"%s\".\n", (char*)configuration.library_path.chars);
#endif
   }

   i_return = 0;

   // don't call c'tors (see TKS_ScriptClassInstance.cpp)
   b_initializing = YAC_TRUE;

   // Make cmdline args (all except runtime args) visible through the global "Arguments" StringArray variable
   createArgArray();

   // Chapters were originally used to e.g. display a loading animation / intro while fetching the
   // rest of the .tkx file from the network.
   if(configuration.default_chapter.isBlank())
   {
      tkx.selectFirstChapter();
   }
   else
   {
      tkx.selectChapterByName(&configuration.default_chapter);
   }


   PTN_Env env; env.initDefault();

   if(((TKS_Compiler*)compiler)->compileCurrentChapter( &env ))
   {
#ifdef TKS_USE_CHARALLOCATOR
      TKS_ScriptEngine::b_allow_char_allocator = YAC_FALSE;
#endif //TKS_USE_CHARALLOCATOR

      if(configuration.debug_level > 63)
      {
#ifdef TKS_DCON
#ifdef YAC_OBJECT_COUNTER
         TKS_ScriptEngine::printf("[...]    Object::object_counter = %i\n", YAC_Object::object_counter);
#endif // YAC_OBJECT_COUNTER
         TKS_ScriptEngine::printf("[...] Statement::object_counter = %i\n", PTN_Statement::object_counter);
#endif
      }

      ptn_error_code = 0u;
      ptn_error_loc  = 0u;

      b_runtime     = YAC_TRUE;
      b_initclasses = YAC_TRUE;  // remove SCI from TKS_Compiler::class_initializers when it is freed

      // ---- construct classes ----
      env.context->b_running = YAC_TRUE;
      ((TKS_Compiler*)compiler)->initializeClasses(&env);

      b_initializing = YAC_FALSE; // xxx is already 0 (see initializeClasses())
      b_initclasses  = YAC_FALSE;

      // ---- reset function local variable stackframe pointers
      env.context->tks_fstacki = configuration.function_stack_size;
      env.context->stacki_displace = 0;

      if(tkscript->configuration.b_emit_explanations)
      {
         // Print module/class/method/function documentation (explain "...") to STDOUT
         emitExplanations();
      }
      else
      {

#ifdef RUN_POOL_BENCHMARK
         debug__run_pool_benchmark();
#endif // RUN_POOL_BENCHMARK

         runGlobalModuleStatements(&env);

         // Find tks_lib_init() and tks_lib_exit() functions (for lib/bg mode)
         if(_bRunInBG)
         {
            YAC_String modName;
            modName.visit("Main");
            TKS_CachedScript *cs = Dtkscompiler->findModule(&modName);
            if(NULL != cs)
            {
               tks_lib_init_fxn = cs->findFunction("tks_lib_init");
               tks_lib_exit_fxn = cs->findFunction("tks_lib_exit");

#ifdef TKS_VST
               tks_lib_vst_open_editor_fxn       = cs->findFunction("tks_lib_vst_open_editor");
               tks_lib_vst_close_editor_fxn      = cs->findFunction("tks_lib_vst_close_editor");
               tks_lib_vst_set_sample_rate_fxn   = cs->findFunction("tks_lib_vst_set_sample_rate");
               tks_lib_vst_set_bpm_fxn           = cs->findFunction("tks_lib_vst_set_bpm");
               tks_lib_vst_process_replacing_fxn = cs->findFunction("tks_lib_vst_process_replacing");
               tks_lib_vst_get_bank_chunk_fxn    = cs->findFunction("tks_lib_vst_get_bank_chunk");
               tks_lib_vst_get_program_chunk_fxn = cs->findFunction("tks_lib_vst_get_program_chunk");
               tks_lib_vst_set_bank_chunk_fxn    = cs->findFunction("tks_lib_vst_set_bank_chunk");
               tks_lib_vst_set_program_chunk_fxn = cs->findFunction("tks_lib_vst_set_program_chunk");
#endif // TKS_VST

            }
         }

         if(env.continue_flag)
         {
            if(_bRunInBG)
            {
               // ---- run tks_lib_init() function ----
               if(NULL != tks_lib_init_fxn)
               {
                  tks_lib_init_fxn->evalCall(&env, &env.cret);
                  env.cret.unset();
               }
            }
            else
            {
               // ---- start main function ----
               PTN_Function *mfun = ((TKS_Compiler*)compiler)->main_fun;

               if(mfun)
               {
                  mfun->evalCall(&env, &env.cret);
                  env.cret.unset();
               }
            }
         }

         if(env.context->b_exception_raised)
         {
            env.context->printException();
         }

         checkCompiletimeError();   // (TODO) obsolete?? (replaced by exceptions)

         if(!_bRunInBG)
         {
#ifdef TKS_PROFILE_STATEMENTS
            if(tkscript->configuration.b_enable_statement_profiling)
            {
               statement_profiler.printResultString();
            }
#endif // TKS_PROFILE_STATEMENTS

#ifdef TKS_PROFILE_FUNCTIONS
            if(tkscript->configuration.b_enable_function_profiling)
            {
               TKS_FunctionProfiler::PrintResults();
            }
#endif // TKS_PROFILE_FUNCTIONS
         }

      }

      if(!_bRunInBG)
      {
         // Stop all script thread contexts
         for(int i=0; i<TKS_MAX_CONTEXTS; i++)
         {
            if(NULL != contexts[i])
            {
               contexts[i]->b_running = YAC_FALSE;
            }
         }

         //////((TKS_Compiler*)compiler)->clean();

#ifdef TKS_DCON
         if(configuration.debug_level > 0u)
            TKS_ScriptEngine::printf("[...] TKS_ScriptEngine::compileRun() finished.\n");
#endif
      }

   }
   else
   {
#ifdef TKS_DCON
      TKS_ScriptEngine::printf("[---] failed to compile chapter \"%s\".\n", tkx.current_chapter_name.chars);
#endif
      Dtkscompiler->parse_error = 1;
      if(0u == tkscript->ptn_error_code)
      {
         tkscript->ptn_error_code = PTNERR_ANYERR; // disables destructors
      }
      //////Dtkscompiler->no_destructors = 1;
   }
}

sBool TKS_ScriptEngine::loadLocalTSL(YAC_String *_libName) {
   // (note) caller must ensure that threads are not currently creating/deleting script class instances

   // Merge TSL file contents to current (main) chapter
   //
   // yac_host->printf("xxx loadLocalTSL 1\n");
   if(!tkscript->tkx.mergeLocalTSL(_libName))
   {
      return YAC_FALSE;
   }

   // yac_host->printf("xxx loadLocalTSL 2\n");
   // don't call c'tors (see TKS_ScriptClassInstance.cpp)
   b_initializing = YAC_TRUE;

   PTN_Env env; env.initDefault();

   // yac_host->printf("xxx loadLocalTSL 3\n");

   if(((TKS_Compiler*)compiler)->compileCurrentChapter( &env, YAC_TRUE/*bTSL*/ ))
   {
      // yac_host->printf("xxx loadLocalTSL 3.1\n");
      ptn_error_code = 0u;
      ptn_error_loc  = 0u;

      b_runtime     = YAC_TRUE;
      b_initclasses = YAC_TRUE;  // remove SCI from TKS_Compiler::class_initializers when it is freed

      // ---- construct classes ----
      env.context->b_running = YAC_TRUE;
      ((TKS_Compiler*)compiler)->initializeClasses(&env);

      // yac_host->printf("xxx loadLocalTSL 3.2\n");

      b_initializing = YAC_FALSE; // xxx is already 0 (see initializeClasses())
      b_initclasses  = YAC_FALSE;

      runGlobalModuleStatements(&env);

      return YAC_TRUE;
   }
   else
   {
#ifdef TKS_DCON
      TKS_ScriptEngine::printf("[---] failed to compile TSL \"%s\".\n", (char*)_libName->chars);
#endif
      Dtkscompiler->parse_error = 1;
      if(0u == tkscript->ptn_error_code)
      {
         tkscript->ptn_error_code = PTNERR_ANYERR; // disables destructors
      }
      //////Dtkscompiler->no_destructors = 1;
   }
   // yac_host->printf("xxx loadLocalTSL 4\n");

   return YAC_FALSE;
}

void TKS_ScriptEngine::stop(void) {
   // Only when compileRun(bRunInBG=1) was called

   // Call "tks_lib_exit" function
   if(NULL != tks_lib_exit_fxn)
   {
      PTN_Env env; env.initDefault();
      tks_lib_exit_fxn->evalCall(&env, &env.cret);
      env.cret.unset();
   }

#ifdef TKS_PROFILE_STATEMENTS
   if(tkscript->configuration.b_enable_statement_profiling)
   {
      statement_profiler.printResultString();
   }
#endif // TKS_PROFILE_STATEMENTS

#ifdef TKS_PROFILE_FUNCTIONS
   if(tkscript->configuration.b_enable_function_profiling)
   {
      TKS_FunctionProfiler::PrintResults();
   }
#endif // TKS_PROFILE_FUNCTIONS

   // Stop all script thread contexts
   for(sUI i = 0u; i < TKS_MAX_CONTEXTS; i++)
   {
      if(NULL != contexts[i])
      {
         contexts[i]->b_running = YAC_FALSE;
      }
   }

   //////((TKS_Compiler*)compiler)->clean();

#ifdef TKS_DCON
   if(configuration.debug_level>0)
      TKS_ScriptEngine::printf("[...] TKS_ScriptEngine::stop: compileRun() finished.\n");
#endif
}

#ifdef TKS_VST
void TKS_ScriptEngine::vstOpenEditor(YAC_ContextHandle _context) {
   // Call tks_lib_vst_open_editor script function
   if(NULL != tks_lib_vst_open_editor_fxn)
   {
      tkscript->evalFunction2(_context,
                              tks_lib_vst_open_editor_fxn,
                              0/*numArgs*/, NULL/*args*/,
                              NULL/*r*/
                              );
   }
}

void TKS_ScriptEngine::vstCloseEditor(YAC_ContextHandle _context) {
   // Call tks_lib_vst_close_editor script function
   if(NULL != tks_lib_vst_close_editor_fxn)
   {
      tkscript->evalFunction2(_context,
                              tks_lib_vst_close_editor_fxn,
                              0/*numArgs*/, NULL/*args*/,
                              NULL/*r*/
                              );
   }
}

void TKS_ScriptEngine::vstSetSampleRate(YAC_ContextHandle _context, sF32 _rate) {
   // Call tks_lib_vst_set_sample_rate script function
   if(NULL != tks_lib_vst_set_sample_rate_fxn)
   {
      YAC_Value args[1];
      args[0].initFloat(_rate);
      tkscript->evalFunction2(_context,
                              tks_lib_vst_set_sample_rate_fxn,
                              1/*numArgs*/, args,
                              NULL/*r*/
                              );
   }
}

void TKS_ScriptEngine::vstSetBPM(YAC_ContextHandle _context, sF32 _bpm) {
   // Call tks_lib_vst_set_bpm script function
   if(NULL != tks_lib_vst_set_sample_rate_fxn)
   {
      YAC_Value args[1];
      args[0].initFloat(_bpm);
      tkscript->evalFunction2(_context,
                              tks_lib_vst_set_bpm_fxn,
                              1/*numArgs*/, args,
                              NULL/*r*/
                              );
   }
}

void TKS_ScriptEngine::vstProcessReplacing(YAC_ContextHandle _context, YAC_Object *_inputs, YAC_Object *_outputs, sUI _numSampleFrames) {
   // Call tks_lib_vst_process_replacing script function
   if(NULL != tks_lib_vst_process_replacing_fxn)
   {
      YAC_Value args[3];
      args[0].initObject(_inputs, 0);
      args[1].initObject(_outputs, 0);
      args[2].initInt(_numSampleFrames);
      tkscript->evalFunction2(_context,
                              tks_lib_vst_process_replacing_fxn,
                              3/*numArgs*/, args,
                              NULL/*r*/
                              );
   }
}

void TKS_ScriptEngine::vstGetBankChunk(YAC_ContextHandle _context, YAC_Object *_buffer) {
   if(NULL != tks_lib_vst_get_bank_chunk_fxn)
   {
      YAC_Value args[1];
      args[0].initObject(_buffer, 0);
      tkscript->evalFunction2(_context,
                              tks_lib_vst_get_bank_chunk_fxn,
                              1/*numArgs*/, args,
                              NULL/*r*/
                              );
   }
}

void TKS_ScriptEngine::vstGetProgramChunk(YAC_ContextHandle _context, YAC_Object *_buffer) {
   if(NULL != tks_lib_vst_get_program_chunk_fxn)
   {
      YAC_Value args[1];
      args[0].initObject(_buffer, 0);
      tkscript->evalFunction2(_context,
                              tks_lib_vst_get_program_chunk_fxn,
                              1/*numArgs*/, args,
                              NULL/*r*/
                              );
   }
}

void TKS_ScriptEngine::vstSetBankChunk(YAC_ContextHandle _context, YAC_Object *_buffer) {
   if(NULL != tks_lib_vst_set_bank_chunk_fxn)
   {
      YAC_Value args[1];
      args[0].initObject(_buffer, 0);
      tkscript->evalFunction2(_context,
                              tks_lib_vst_set_bank_chunk_fxn,
                              1/*numArgs*/, args,
                              NULL/*r*/
                              );
   }
}

void TKS_ScriptEngine::vstSetProgramChunk(YAC_ContextHandle _context, YAC_Object *_buffer) {
   if(NULL != tks_lib_vst_set_program_chunk_fxn)
   {
      YAC_Value args[1];
      args[0].initObject(_buffer, 0);
      tkscript->evalFunction2(_context,
                              tks_lib_vst_set_program_chunk_fxn,
                              1/*numArgs*/, args,
                              NULL/*r*/
                              );
   }
}
#endif // TKS_VST

void TKS_ScriptEngine::printResolveError(sChar *_name) {
#ifdef TKS_DCON
   TKS_ScriptEngine::printf("[---] (JIT) failed to resolve variable \"%s\" in module \"%s\" line %i.\n",
                            _name,
                            ((TKS_Compiler*)compiler)->getModuleNameByIndex(Dsrcloc_module(ptn_error_loc)),
                            Dsrcloc_linenr(ptn_error_loc)
                            );
#endif
}

void TKS_ScriptEngine::printResolveClassError(void *_class) {
#ifdef TKS_DCON
   TKS_ClassDecl *cd=(TKS_ClassDecl*)_class;
   TKS_ScriptEngine::printf("[---] failed to resolve class \"%s::%s\",\n[---] (first ref'd in module \"%s\" line %i).\n",
                            (char*)cd->nspace->name.chars,
                            (char*)cd->name.chars,
                            ((TKS_Compiler*)compiler)->getModuleNameByIndex(Dsrcloc_module(cd->src_loc)),
                            Dsrcloc_linenr(cd->src_loc)
                            );
#endif
}

YAC_Object *YAC_VCALL TKS_ScriptEngine::yacNew(const char *_nspaceName, const char *_classname) {
   YAC_String classname;
   classname.visit(_classname);
   if(NULL != _nspaceName)
   {
      YAC_String nspaceName;
      nspaceName.visit(_nspaceName);
      TKS_Namespace *nspace = findNamespace(&nspaceName);
      if(NULL != nspace)
      {
         return newObjectByName(nspace, &classname);
      }
      else
      {
         // Namespace not found
         return NULL;
      }
   }
   else
   {
      return newObjectByName(NULL, &classname);
   }
}

YAC_Object *TKS_ScriptEngine::newObjectByName(TKS_Namespace *_nspace, YAC_String *_className) {
   // Script class
   TKS_ClassDecl *cd = ((TKS_Compiler*)tkscript->compiler)->findClassDeclNamespace(_className, _nspace);
   // yac_host->printf("xxx TKS_ScriptEngine::newObjectByName: className=\"%s\" => class_decl=%p\n", _className->chars, cd);
   if(cd)
   {
      // yac_host->printf("xxx TKS_ScriptEngine::newObjectByName:    cd->class_template=%p\n", cd->class_template);
      return YAC_CLONE_POOLED(tkscript->yacContextGetDefault(), cd->class_template); // xxx TKS_MT: use *real* thread context
   }
   else
   {
      // C++ class
      TKS_ClassTemplate *clt = findTemplate(_className);
      if(clt)
      {
         return YAC_CLONE_POOLED(tkscript->yacContextGetDefault(), clt->template_object); // xxx TKS_MT: use *real* thread context (script classes will probably crash)
      }
   }
   // Failed
   return NULL;
}

YAC_Object *TKS_ScriptEngine::newObjectArrayByName(TKS_Namespace *_nspace, YAC_String *_className) {
   // Script class
   TKS_ClassDecl *cd = Dtkscompiler->findClassDeclNamespace(_className, _nspace);
   if(cd)
   {
      TKS_Context *ctx = lockGlobalContext();
      ////YAC_ClassArray *a = (YAC_ClassArray*) YAC_NEW_CORE_POOLED(YAC_CLID_CLASSARRAY);
      YAC_ClassArray *a=(YAC_ClassArray *)TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_CLASSARRAY]->template_object->yacNew((void*)ctx); // xxx TKS_MT: use *real* thread context
      unlockGlobalContext();
      a->class_decl = cd;
      return a;
   }
   else
   {
      TKS_ClassTemplate *tmp=tkscript->findTemplate(_className);
      if(tmp)
      {
         if(tmp->may_instantiate == YAC_CLASSTYPE_NORMAL) // ---- allocated by both c++ and tkscript code
         {
            // Has special array object?
            YAC_Object *o = tmp->template_object->yacArrayNew();
            if(!o)
            {
               // Create generic ObjectCrray
               o = YAC_NEW_CORE_POOLED(YAC_CLID_OBJECTARRAY);
               ////o = tkscript->TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_OBJECTARRAY]->template_object->yacNew(NULL); // xxx TKS_MT: use *real* thread context
               ((YAC_ObjectArray*)o)->template_object = tmp->template_object;
            }
            return o;
         }
         else
         {
            Dprintf("\n[---] newObjectArrayByName: array of class \"%s\" may not be instantiated.\n",
               (char*)tmp->class_name.chars);
            return NULL;
         }
      }
   }
   // Failed
   return NULL;
}

#if 0
void TKS_ScriptEngine::createTestObject(void) {
   printf("xxx ~~~~~~~~~~~~~~~~~~~~~~~~~ createTestObject ~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
   YAC_String nspName;
   nspName.visit("ui");
   TKS_Namespace *nspace = findNamespace(&nspName);
   YAC_String className;
   className.visit("View");
   YAC_Object *o = newObjectByName(nspace, &className);
   printf("xxx TKS_ScriptEngine::createTestObject: view=%p\n", o);
   yacDelete(o);
}
#endif

void YAC_VCALL TKS_ScriptEngine::yacDelete(YAC_Object *_o) {
   if(_o)
   {
      if(YAC_VALID_TAG != _o->validation_tag)
      {
         Dprintf("\n[---] yacDelete: double-free detected, object=0x%p.\n", _o);
         return;
      }

#ifdef TKS_ALLOWNODELETEOBJECTS
      if(! configuration.b_enable_delete_objects)
      {
         // Call destructors
         ////_o->yacFinalizeObject(NULL); // do not call since it unsets the vtable!

         // Mark invalid but do not actually delete
         _o->validation_tag = YAC_INVALID_TAG;
         return;
      }
#endif // TKS_ALLOWNODELETEOBJECTS

#ifdef YAC_OBJECT_POOL
      if(_o->pool_handle.pool_id)
      {
         ObjectPool::Delete(_o);
      }
      else
      {
         delete _o;
      }
#else
      delete _o;
#endif // YAC_OBJECT_POOL
   }
}

YAC_Object *YAC_VCALL TKS_ScriptEngine::yacNewByID(sUI _classID) {
   //::printf("xxx yacNewByID(classID=%d)\n", _classID);
   ////YAC_Object *o = atom_speedpath[_classID]->template_object->yacNew(NULL); // xxx TKS_MT: use *real* thread context (c'tors + exceptions!)
   YAC_Object *o = TKSCRIPT__ATOM_SPEEDPATH[_classID]->template_object->yacNewObject();
   return o;
}

YAC_Object *YAC_VCALL TKS_ScriptEngine::yacNewPooledByID(sUI _classID, sUI _poolHint) {
#ifdef YAC_OBJECT_POOL
   YAC_Object *o = ObjectPool::New(_classID, _poolHint);
   if(NULL == o)
   {
      // Pooling might be disabled or the pools are full
      return TKSCRIPT__ATOM_SPEEDPATH[_classID]->template_object->yacNewObject();//yacNewByID(_classID);
   }
   return o;
#else
   // Do not use pooling
   ////return TKS_ScriptEngine::yacNewByID(_classID);
   return TKSCRIPT__ATOM_SPEEDPATH[_classID]->template_object->yacNewObject()
#endif // YAC_OBJECT_POOL
}

void YAC_VCALL TKS_ScriptEngine::yacNewDeleteModifyCounter(sSI _deltaByteSize) {
#ifdef YAC_GLOBAL_NEWDELETE
   if(_deltaByteSize > 0)
   {
      yac_global_newdelete_numallocs++;
   }
   else
   {
      yac_global_newdelete_numfrees++;
   }
   yac_global_newdelete_counter += _deltaByteSize;
#else
   printf("[---] TKS_ScriptEngine::yacNewDeleteModifyCounter called but build is not configured for YAC_GLOBAL_NEWDELETE\n");
#endif // YAC_GLOBAL_NEWDELETE
}

sSI YAC_VCALL TKS_ScriptEngine::yacNewDeleteGetCounter(void) {
#ifdef YAC_GLOBAL_NEWDELETE
   return yac_global_newdelete_counter;
#else
   printf("[---] TKS_ScriptEngine::yacNewDeleteGetCounter called but build is not configured for YAC_GLOBAL_NEWDELETE\n");
   return 0;
#endif // YAC_GLOBAL_NEWDELETE
}

sUI YAC_VCALL TKS_ScriptEngine::yacGetDebugLevel(void) {
   return (sUI)configuration.debug_level;
}

sBool TKS_ScriptEngine::findDeclaredClassType(YAC_String *_name,
                                              TKS_FreezedNamespaceArray _nsa,
                                              TKS_Namespace *_forcedNSpace,
                                              TKS_ClassTemplate**_retTemplate,
                                              TKS_ClassDecl**_retClassDecl
                                              )

{
   //
   // Find C++ class template or script class decl in the given namespaces
   //
   // If _nsa is NULL, try to find script class first, if that does not exist,
   // try C++ class.
   //
   *_retClassDecl = NULL;
   *_retTemplate = NULL;
   sBool bTryYAC = 1;
   sBool bForceYAC = ( _forcedNSpace == &Dtkscompiler->yac_namespace );

   if( !bForceYAC ) // is not forced C++ namespace?
   {
      if(NULL != _forcedNSpace)
      {
         // Explicit namespace is given, do not try to find C++ class
         bTryYAC = 0;

         *_retClassDecl = Dtkscompiler->findClassDeclNamespace(_name, _forcedNSpace);
      }
      else
      {
         // Try to find existing script class, either in default namespace or in given namespace array
         *_retClassDecl = Dtkscompiler->findClassDeclNamespaceArray(_name, _nsa);
      }
      if(NULL != *_retClassDecl) // Script classes have priority over C++ classes
      {
         // OK, found existing script class
         return 1;
      }
   }

   if(bTryYAC)
   {
      // Is not an existing class, try C++
      *_retTemplate = tkscript->findTemplate(_name);
      if(NULL != *_retTemplate)
      {
         // OK, Found C++ class
         return 1;
      }
      else if(bForceYAC)
      {
         Dprintf("[---] C++ class \"%s\" does not exist.\n", (char*)_name->chars);
         // Caller should raise parse error
         return 0;
      }
   }

   // Class not found
   if(NULL != _forcedNSpace)
   {
      Dprintf("[---] Script class \"%s::%s\" does not exist.\n",
         (char*)_forcedNSpace->name.chars,
         (char*)_name->chars
         );
   }
   else
   {
      Dprintf("[---] Script class \"%s\" does not exist.\n",
         (char*)_name->chars
         );
   }

   // Failed
   return 0;
}

sBool TKS_ScriptEngine::evalFunction2(YAC_ContextHandle _context, PTN_Function *_f,
                                      sUI _numArgs, YAC_Value *_args,
                                      YAC_Value *_r
                                      ) {

   if(NULL == _f)
   {
      // should not be reachable (DEBUG)
      printf("[!!!] TKS_ScriptEngine::evalFunction2: f is NULL !!!\n");
      if(NULL != _r)
         _r->initVoid();
      return YAC_FALSE;
   }

   TKS_Context *context = (TKS_Context*)_context;

   if(context->b_running)
   {
#if 0
      // Use context as thread default context if none exists
      // !!!!! NO !!!!!  it can happen that the "tks_current_context" has already been deleted (e.g. when it was set to the MIDITimer::script_context!)
      // apps must use yacContextSetDefault() if a script fxn/method is to be called from a non-script thread (e.g. the windows multimedia timer)
#ifndef YAC_FORCE_NO_TLS
      if((NULL == tks_current_thread) && (NULL == tks_current_context))
      {
         tks_current_context = _context;
      }
#endif // YAC_FORCE_NO_TLS
#endif // 0

      // Validate argument count
      if(NULL != _f->argvarlist)
      {
         if(_f->argvarlist->num_vars != _numArgs)
         {
            tkscript->printf("[---] yacEvalFunction: argument mismatch in call to function \"%s::%s\" (expected %d, have %d).\n",
                             (_f->parent_class?((char*)_f->parent_class->name.chars) : ""),
                             (char*)_f->name.chars,
                             _f->argvarlist->num_vars, _numArgs);
            return YAC_FALSE;
         }
      }
      else
      {
         if(0u != _numArgs)
         {
            tkscript->printf("[---] yacEvalFunction: argument mismatch in call to function \"%s::%s\" (expected 0, have %d).\n",
                             (_f->parent_class?((char*)_f->parent_class->name.chars) : ""),
                             (char*)_f->name.chars,
                             _numArgs);
            return YAC_FALSE;
         }
      }

      PTN_Env env(context);
      PTN_Env *_env = &env; // For the macros..
      Dbeginfuncall(_f);

      // Prepare local var stackframe (if required)
      YAC_Value *stackc = NULL;
      YAC_Value *nsf;
      if(_f->num_local_vars > 0)
      {
         if((nsf = _env->context->pushStackFrame(_f))!=0) // get expected call ("displaced") stackframe
         {
            stackc = _env->context->tks_fstackc;
         }
         else
         {
            tkscript->printf("[---] yacEvalFunction: pushStackFrame() failed.\n");
            Dendfuncall(_f);
            return YAC_FALSE;
         }
      }
      else
      {
         nsf = NULL;
      }

#ifdef DX_TRACEFUNCTIONCALLS
      context->pushFunctionCall(_f, 0/*src_loc*/);
#endif

      // Set argument values
      _f->setArgsByValues(_env, _args, nsf);

      if(_f->num_local_vars > 0)
      {
         _env->context->pushStackFrame2(_f); // really change tks_fstackc
      }

      // Evaluate function statements
      {
         YAC_Value r;
         _f->evalCall(_env, &r);
         // Copy return value
         if(NULL != _r)
         {
            _r->copySafe(&r);
         }
         r.unset();
      }


//// Note: should have already been printed out, see PTN_Statement::evalFirst
////         if(context->b_exception_raised)
////         {
////            context->printException();
////         }


      // Pop localvar stackframe
      if(_f->num_local_vars > 0)
      {
         _env->context->popStackFrame(_f);
         _env->context->tks_fstackc = stackc;
      }

#ifdef DX_TRACEFUNCTIONCALLS
      context->popFunctionCall();
#endif

#ifdef TKS_JIT
      ////tksvm_env.continue_flag=1;
#endif
      Dendfuncall(_f);

      // Show exception name/message/backtrace
      if(_env->context->b_exception_raised)
      {
         _env->context->printException();
      }

      return YAC_TRUE;
   } // if ctx running
   return YAC_FALSE;
}

sBool TKS_ScriptEngine::evalFunction(YAC_ContextHandle _context, YAC_FunctionHandle _sf,
                                     sUI _numArgs, YAC_Value *_args,
                                     YAC_Value *_r
                                     )
{
   // _r may be NULL
   if( _context && _sf && ( !(_numArgs/*||_args*/)||(_numArgs&&_args) ) )
   {
      TKS_FunctionObject *fo = (TKS_FunctionObject *) _sf;
      PTN_Function *f = fo->fun;
      // tkscript->printf("xxx evalFunction FunctionObject fo=%p f=%p\n", fo, fo->fun);

      ////printf("xxx TKS_ScriptEngine::evalFunction: context->b_running=%d\n", context->b_running);
      return evalFunction2(_context, f, _numArgs, _args, _r);
   }
   return YAC_FALSE;
}


sUI YAC_VCALL TKS_ScriptEngine::yacEvalFunction(YAC_ContextHandle _context,
                                                YAC_FunctionHandle _sf,
                                                sUI _numArgs, YAC_Value *_args
                                                ) {
   // TODO: remove yacEvalFunctionReturn and add _r argument
   return (sUI)evalFunction(_context, _sf, _numArgs, _args, NULL);
}

sUI YAC_VCALL TKS_ScriptEngine::yacEvalFunctionReturn(YAC_ContextHandle _context,
                                                      YAC_FunctionHandle _yac_fun,
                                                      sUI _numArgs, YAC_Value *_args,
                                                      YAC_Value *_r
                                                      ) {
   // TODO: rename to yacEvalFunction
   return (sUI)evalFunction(_context, _yac_fun, _numArgs, _args, _r);
}


sBool TKS_ScriptEngine::evalScriptMethodByName(YAC_ContextHandle _context,
                                               TKS_ScriptClassInstance *_o,
                                               const char*_name,
                                               YAC_Value*_args, sUI _numArgs,
                                               YAC_Value *_r
                                               )
{
   // _r may be NULL

   if( _context && ( !(_numArgs||_args)||(_numArgs&&_args) ) )
   {
      TKS_Context *context =(TKS_Context*)_context;
      if(context->b_running)
      {
         // Resolve method
         TKS_CachedObject *co = _o->getMethod(_name);

         if(NULL == co)
         {
            Dprintf("[---] evalScriptMethodByName: class \"%s\" has no method called \"%s\".\n", _o->yacMetaClassName(), _name);
            _r->initVoid();
            return YAC_FALSE;
         }

         // Found method
         TKS_ClassDeclMethod *m = (TKS_ClassDeclMethod*) co->value.object_val;

         // xxx how to handle protection flags? ignore?
         PTN_Function *f = &m->function;

         // Validate argument count
         if(NULL != f->argvarlist)
         {
            if(f->argvarlist->num_vars != _numArgs)
            {
               Dprintf("[---] evalScriptMethodByName: argument mismatch in call to function \"%s::%s\" (expected %d, have %d).\n",
                  (f->parent_class?((char*)f->parent_class->name.chars) : ""),
                  (char*)f->name.chars,
                  f->argvarlist->num_vars, _numArgs);
               return YAC_FALSE;
            }
         }
         else
         {
            if(0 != _numArgs)
            {
               tkscript->printf("[---] evalScriptMethodByName: argument mismatch in call to function \"%s::%s\" (expected 0, have %d).\n",
                  (f->parent_class?((char*)f->parent_class->name.chars) : ""),
                  (char*)f->name.chars,
                  _numArgs);
               return YAC_FALSE;
            }
         }

         PTN_Env env(context);
         PTN_Env *_env = &env; // For the macros
         Dbeginfuncall(f);

         // Push <this>
         context->pushClassStack(_o);

         // Prepare local var stackframe (if required)
         YAC_Value *stackc = NULL;
         YAC_Value *nsf;
         if(f->num_local_vars > 0)
         {
            if((nsf = _env->context->pushStackFrame(f))!=0) // get expected call ("displaced") stackframe
            {
               stackc = _env->context->tks_fstackc;
            }
            else
            {
               tkscript->printf("[---] evalScriptMethodByName: pushStackFrame() failed.\n");
               Dendfuncall(f);
               // Pop <this>
               context->popClassStack();
               return YAC_FALSE;
            }
         }
         else
         {
            nsf = NULL;
         }

         // Set argument values
         f->setArgsByValues(_env, _args, nsf);

         if(f->num_local_vars > 0)
         {
            _env->context->pushStackFrame2(f); // really change tks_fstackc
         }

         // Evaluate function statements
         {
            f->evalCall(_env, &env.cret);

            // Copy return value
            if(NULL != _r)
            {
               _r->copySafe(&env.cret);
            }
            env.cret.unset();
         }

         // Pop localvar stackframe
         if(f->num_local_vars > 0)
         {
            _env->context->popStackFrame(f);
            _env->context->tks_fstackc = stackc;
         }

#ifdef TKS_JIT
         ////tksvm_env.continue_flag = 1;
#endif
         Dendfuncall(f);

         // Pop <this>
         context->popClassStack();

         return YAC_TRUE;
      } // if ctx running
   }
   return YAC_FALSE;
}


void TKS_ScriptEngine::setArgs(sSI _argc, char**_argv) {
   cached_argc=_argc;
   cached_argv=_argv;
}

void TKS_ScriptEngine::createArgArray(void) {
   YAC_Object *o = YAC_NEW_CORE(YAC_CLID_STRINGARRAY);
   if(cached_argc)
   {
      (void)o->yacArrayAlloc(cached_argc, 0,0,0);
      sSI i;
      YAC_Value r;
      for(i=0; i<cached_argc; i++)
      {
         o->yacArrayGet(NULL, i, &r); // TKS_MT: use *real* thread context (exceptions!)
         if((r.type==4)&&(r.value.string_val)) // xxx unnecessary if ??
            r.value.string_val->copy((sChar*)cached_argv[i]);
      }
      o->yacArraySetWidth(cached_argc);
   }
   YAC_String name; name.visit("Arguments");
   tkscript->global_instance_namespace.createObjectEntry(&name, 1, o, 1);
}

YAC_FunctionHandle YAC_VCALL TKS_ScriptEngine::yacFindFunction(sChar *_name) {
   YAC_String s;
   s.visit(_name);
   sSI mi = s.indexOf(".", 0);
   if(-1 != mi)
   {
      YAC_String t;
      s.substring(&t, 0, mi);
      TKS_CachedScript *cs = Dtkscompiler->findModule(&t);
      if(cs)
      {
         s.substring(&t, mi+1, s.length-mi-1);
         PTN_Function *f = cs->findFunction(&t);
         if(f)
         {
            return (void*)f->getOrCreateFunctionObject();
         }
      }
   }
   return NULL;
}

YAC_ModuleHandle YAC_VCALL TKS_ScriptEngine::yacCompileModule(sChar *_source) {
   TKS_ScriptModule *sc = (TKS_ScriptModule*)YAC_NEW_CORE(TKS_CLID_SCRIPT);
   YAC_String source;
   YAC_Value r;
   source.copy(_source);
   if(!source.isBlank())
   {
      source.resubstEscapeSeq2();
      if(sc->load(&source))
         return (void*)sc;
   }
   delete sc;
   return NULL;
}

YAC_FunctionHandle YAC_VCALL TKS_ScriptEngine::yacFindFunctionInModule(YAC_ModuleHandle _mod, sChar *_name) {
   if(_mod)
   {
      TKS_ScriptModule *sc = (TKS_ScriptModule*)_mod;
      TKS_CachedScript *cs = (TKS_CachedScript*)sc->cscript;
      YAC_String name; name.visit(_name);
      PTN_Function *f = cs->findFunction(&name);
      if(f)
      {
         return f->getOrCreateFunctionObject();
      }
   }
   return NULL;
}

YAC_VariableHandle YAC_VCALL TKS_ScriptEngine::yacFindVariableInModule(YAC_ModuleHandle _mod, sChar *_name) {
   if(_mod)
   {
      TKS_ScriptModule *sc=(TKS_ScriptModule*)_mod;
      TKS_CachedScript *cs=(TKS_CachedScript*)sc->cscript;
      YAC_String s_name; s_name.visit(_name);
      TKS_ActiveNamespace a;
      a.nspace = &Dtkscompiler->default_namespace;
      a.next = NULL;
      TKS_CachedObject *co=cs->findVar(&s_name, &a, NULL);
      return(void*)co;
   }
   return 0;
}

YAC_VariableHandle YAC_VCALL TKS_ScriptEngine::yacFindVariableInFunction(YAC_FunctionHandle _fn, sChar *_name) {
   if(_fn)
   {
      TKS_FunctionObject *sf = (TKS_FunctionObject *) _fn;
      PTN_Function *f = sf->fun;

      YAC_String nm; nm.visit(_name);
      return (void*)f->findFunVar(&nm);
   }
   return 0;
}

void YAC_VCALL TKS_ScriptEngine::yacSetVariable(YAC_VariableHandle _var, YAC_Value *_v) {
   if(_var)
   {
      TKS_CachedObject *co = (TKS_CachedObject*)_var;
      co->derefPointerAssignValue(_v);
   }
}

void YAC_VCALL TKS_ScriptEngine::yacGetVariable(YAC_VariableHandle _var, YAC_Value *_r) {
   if(_var)
   {
      _r->initValue((TKS_CachedObject*)_var);
   }
}


void YAC_VCALL TKS_ScriptEngine::yacDeleteModule(void *_mod) {
   if(_mod)
   {
      TKS_ScriptModule *sc = (TKS_ScriptModule*)_mod;
      delete sc;
   }
}

sUI YAC_VCALL TKS_ScriptEngine::yacScanFlags(YAC_String *_s) {
   if(_s)
   {
      sUI r=0;
      YAC_String ts;
      ts.yacCopy(_s);
      ts.split(',');
      TKS_StringIterator si;
      si.begin(&ts);
      if(si.tail())
      {
         do
         {
            YAC_String *c=si.current;
            TKS_CachedObject *co=tkscript->global_constant_namespace.findEntry(c);
            if(!co)
               co=tkscript->uconst_table.findEntry(c);
            if(co)
               r = r | co->getIntValue();
            else
            {
               tkscript->printf("[---] yacScanFlags: unknown constant \"%s\".\n", c?(char*)c->chars:"NULL");
            }
         }
         while(si.previous());
      }
      ts.freeStack();
      return r;
   }
   return 0;
}

sSI YAC_VCALL TKS_ScriptEngine::yacStringReplace(YAC_String *_d, YAC_String *_a, YAC_String *_b) {
   return _d->replace(_a, _b);
}

sSI YAC_VCALL TKS_ScriptEngine::yacStringScan(YAC_String *_s, sSI*_r) {
   if(_s)return _s->yacScanI(_r);
   return 0;
}

sSI YAC_VCALL TKS_ScriptEngine::yacStringScan(YAC_String *_s, sF32*_r) {
   if(_s)return _s->yacScanF32(_r);
   return 0;
}

sUI YAC_VCALL TKS_ScriptEngine::yacRegisterFunction(void *_adr, const char *_name, sUI _returntype, const char *_return_otype, sUI _numArgs, const sUI *_argtypes, const char **_argtypenames, sUI _callstyle) {
   if(_numArgs > 16)
   {
      Dprintf("[---] yacRegisterFunction(name=\"%s\"): max argument count exceeded (%i > %i).\n",
         _name, _numArgs, 16);
      return 0;
   }

   YAC_CommandY**varRetAddr = NULL;

   YAC_String fn; fn.visit((char*)_name);

   // Handle >=0.9.9.0 =>, <=> style function variants (return argument/value)

   sBool bRARG  = (-1 != fn.indexOf("_YAC_RARG"));
   sBool bRVAL  = (-1 != fn.indexOf("_YAC_RVAL"));
   if(bRARG || bRVAL)
   {
      if(bRARG && bRVAL)
      {
         Dprintf("[---] yacRegisterFunction(name=\"%s\"): function variant cannot only be either RARG or RVAL.\n", _name);
         return 0;
      }

      YAC_String fnRoot;
      fnRoot.copy((char*)_name);
      if(bRARG)
      {
         fnRoot.replace("_YAC_RARG", "");
      }
      else if(bRVAL)
      {
         fnRoot.replace("_YAC_RVAL", "");
      }

      // Validate function signature
      if(bRVAL)
      {
         if(4 != _returntype)
         {
            Dprintf("[---] yacRegisterFunction(name=\"%s\"): RVAL function variant must have variable return type (YAC_Value *_r).\n", _name);
            return 0;
         }
      }
      else if(bRARG)
      {
         if(0 == _numArgs)
         {
            Dprintf("[---] yacRegisterFunction(name=\"%s\"): last argument to RARG function variant must be YAC_Object* (no args found).\n", _name);
            return 0;
         }
         else
         {
            if(0 != _returntype)
            {
               Dprintf("[---] yacRegisterFunction(name=\"%s\"): RARG function variant must have void return type.\n", _name);
               return 0;
            }
            // Decrease argcount since the last argument is actually the return type!
            _numArgs--;
            if(_argtypes[_numArgs] < YAC_TYPE_OBJECT)
            {
               Dprintf("[---] yacRegisterFunction(name=\"%s\"): last argument to RARG function variant must be YAC_Object* (return arg has wrong type).\n", _name);
               return 0;
            }
         }
      }

      // Find or create root function entry if it doesn't already exist
      TKS_CachedObject *coRoot = yac_functions.findEntry(&fnRoot);
      if(NULL == coRoot)
      {
         YAC_CommandY *f = new YAC_CommandY();

         coRoot = yac_functions.createEntry(&fnRoot, 1);
         coRoot->value.object_val = (YAC_Object*) f;
         coRoot->type     = 3;
         coRoot->flags    = 0;
         coRoot->deleteme = 0;

         f->y_adr          = NULL;
         f->y_name         = (const char*) coRoot->name.chars;
         f->y_return_type  = 0;
         f->y_return_otype = 0;
         f->y_arg_num      = _numArgs;
         f->y_arg_types    = NULL;
         f->y_arg_otypes   = NULL;
         f->y_callstyle    = 0;

      }
      if(NULL != coRoot)
      {
         // Found/created root function entry
         YAC_CommandY *fRoot = (YAC_CommandY*) coRoot->value.object_val;
         if(bRARG)
         {
            if(NULL == fRoot->y_retarg_cmd)
            {
               if(_numArgs == fRoot->y_arg_num)
               {
                  varRetAddr = &fRoot->y_retarg_cmd;
               }
               else
               {
                  Dprintf("[---] yacRegisterFunction(name=\"%s\"): RARG function variant has different argcount than root function entry (%u != %u).\n",
                     _name, _numArgs, fRoot->y_arg_num
                     );
                  return 0;
               }
            }
            else
            {
               Dprintf("[---] yacRegisterFunction(name=\"%s\"): RARG function variant already registered.\n", _name);
               return 0;
            }
         }
         else if(bRVAL)
         {
            if(NULL == fRoot->y_retval_cmd)
            {
               if(_numArgs == fRoot->y_arg_num)
               {
                  varRetAddr = &fRoot->y_retval_cmd;
               }
               else
               {
                  Dprintf("[---] yacRegisterFunction(name=\"%s\"): RVAL function variant has different argcount than root function entry (%u != %u).\n",
                     _name, _numArgs, fRoot->y_arg_num
                     );
                  return 0;
               }
            }
            else
            {
               Dprintf("[---] yacRegisterFunction(name=\"%s\"): RVAL function variant already registered.\n", _name);
               return 0;
            }
         }
      }
      else
      {
         Dprintf("[---] yacRegisterFunction(name=\"%s\"): failed to create root function entry.\n", _name);
         return 0;
      }
   }


   // Create new function command
   YAC_CommandY *f = new YAC_CommandY();
   f->y_adr          = _adr;
   f->y_name         = _name;
   f->y_return_type  = _returntype;
   f->y_return_otype = _return_otype;
   f->y_arg_num      = _numArgs;
   f->y_arg_types    = _argtypes;
   f->y_arg_otypes   = _argtypenames;
   f->y_callstyle    = _callstyle;

   if(NULL == varRetAddr)
   {
      TKS_CachedObject *co;

      co = yac_functions.findEntry(&fn);
      if(NULL == co)
      {
         co = yac_functions.createEntry(&fn, 1);
         if(co)
         {
            co->value.object_val = (YAC_Object*) f;
            co->type     = 3;
            co->flags    = 0;
            co->deleteme = 0;
            // Ok, registered regular function
            return 1;
         }
         else
         {
#ifdef TKS_DCON
            printf("[---] yacRegisterFunction: failed to register \"%s\", max. # of functions exceeded.\n", _name);
#endif
            return 0;
         }
      }
      else
      {
#ifdef TKS_DCON
         printf("[---] yacRegisterFunction: function \"%s\" is already registered%s.\n", _name,
            ((NULL == ((YAC_CommandY*)co->value.object_val)->y_adr) ? " (expecting RARG or RVAL function variant)" : "")
            );
#endif
         return 0;
      }
   }
   else
   {
      *varRetAddr = f;
      // Ok, registered RARG/RVAL function variant
      return 1;
   }
}

sUI YAC_VCALL TKS_ScriptEngine::yacGetClassIDByName(sChar *_name) {
   YAC_String nm; nm.visit(_name);
   TKS_ClassTemplate *clt=tkscript->findTemplate(&nm);
   if(clt)
   {
      return clt->template_object->class_ID;
   }
   else
   {
      return (sUI)-1;
   }
}

sSI YAC_VCALL TKS_ScriptEngine::yacEvalMethodByName(YAC_Object *_o,
                                                    const char *_name,
                                                    YAC_Value*  _args,
                                                    sUI         _numArgs,
                                                    YAC_Value * _r
                                                    )
{
   if(YAC_VALID(_o))
   {
      sUI clid = _o->class_ID;

       // xxx actually it depends on the called method. ScriptClass objects have C++ methods, too. (yac*)
      if(YAC_CLID_CLASS == clid)
      {
         ////TKS_Context *ctx = lockGlobalContext();
         ////sSI r = evalScriptMethodByName(ctx, (TKS_ScriptClassInstance*)_o, _name, _args, _numArgs, _r);
         sSI r = evalScriptMethodByName(yacContextGetDefault(), (TKS_ScriptClassInstance*)_o, _name, _args, _numArgs, _r);
         ////unlockGlobalContext();
         return r;
      }

      if(clid<YAC_MAX_CLASSES)
      {
         TKS_ClassTemplate *clt=TKSCRIPT__ATOM_SPEEDPATH[clid];
         if(clt)
         {
            YAC_String t;
            t.visit((sChar*)_name);
            const YAC_CommandY *cmdy = clt->findCommandY(&t);

            // Handle case where function has RVAL variant and root command is unimplemented
            sBool bRSELFEmul = 0;
            sBool bCRC = PTN_FunctionECallY::CheckRootCommand(0, &cmdy, 0, &bRSELFEmul);
            if(!bCRC || bRSELFEmul)
            {
               printf("[---] TKS::yacEvalMethodByName: missing support for RSELFEmul in call to C++ method \"%s::%s\" (bRCR=%d bRSELFEmul=%d).\n",
                  clt->class_name.chars,
                  cmdy->y_name,
                  bCRC, bRSELFEmul
                  );
               return 0;
            }

            if(cmdy)
            {
               // ---- method found, now prepare arguments ----
               if(_numArgs == cmdy->y_arg_num)
               {
                  //YAC_Value args[16]; // xxx numargs hardcoded
                  yacmem call_args[16]; // xxx numargs hardcoded
                  yacmemptr argp; argp.mem = call_args;
                  sUI i;
                  if(cmdy->y_arg_num)
                  {
                     // xxx this should not happen since CheckRootCommand() fix..
                     if(!cmdy->y_arg_types)
                     {
                        // This can currently happen if a C++ method only has rarg or rval or rself variant but no root implementation
                        printf("[---] TKS::yacEvalMethodByName: missing support for unimplemented root call of rarg/rval/rself style method.\n");
                        return 0;
                     }

                     for(i=0; i<cmdy->y_arg_num; i++)
                     {
                        _args[i].typecast(cmdy->y_arg_types[i]);
                        call_args[i].any=_args[i].value.any;
                     }
                  }
                  // ---- now we collected all args ----
                  switch(cmdy->y_callstyle)
                  {
                  case 0:
                     ((void(YAC_CALL*)(void*))cmdy->y_adr)((void*)_o);
                     break;
                  case 1: // args, no return
                     ((void(YAC_CALL*)(void*, yacmemptr))cmdy->y_adr)((void*)_o, argp);
                     break;
                  case 2: // int return
                     _r->initInt(((sSI(YAC_CALL*)(void*))cmdy->y_adr)((void*)_o));
                     break;
                  case 3: // args+int return
                     _r->initInt(((sSI(YAC_CALL*)(void*, yacmemptr))cmdy->y_adr)((void*)_o, argp));
                     break;
                  case 4: // float return
                     _r->initFloat(((sF32(YAC_CALL*)(void*))cmdy->y_adr)((void*)_o));
                     break;
                  case 5: // args+ float return
                     _r->initFloat(((sF32(YAC_CALL*)(void*, yacmemptr))cmdy->y_adr)((void*)_o, argp));
                     break;
                  case 6: // object return
                     _r->initObject((YAC_Object*)((void*(YAC_CALL*)(void*))cmdy->y_adr)((void*)_o), 0);
                     break;
                  case 7: // args+object return
                     _r->initObject((YAC_Object*)((void*(YAC_CALL*)(void*, yacmemptr))cmdy->y_adr)((void*)_o, argp), 0);
                     break;
                  case 8: // var return
                     ((void(YAC_CALL*)(void*, YAC_Value*))cmdy->y_adr)((void*)_o, _r);
                     break;
                  case 9: // args+var return
                     ((void(YAC_CALL*)(void*, yacmemptr, YAC_Value*))cmdy->y_adr)((void*)_o, argp, _r);
                     break;
                  }
                  // ---- caller has to unset values ----
                  return 1;
               }
               else
               {
#ifdef TKS_DCON
                  printf("TKScript::yacEvalMethodByName(\"%s\"): wrong argument count (class=\"%s\", class_ID=%i).\n", _name, _o->yacClassName(), clid);
#endif
               }
            } // if cmdy
            else
            {
#ifdef TKS_DCON
               printf("TKScript::yacEvalMethodByName(\"%s\"): method not found (class=\"%s\", class_ID=%i).\n", _name, _o->yacClassName(), clid);
#endif
            }
         }
         else
         {
#ifdef TKS_DCON
            printf("TKScript::yacEvalMethodByName(\"%s\"): failed to find class template for class_ID==%i, name=\"%s\".\n", _name, clid, _o->yacClassName());
#endif
         }
      }
      else
      {
#ifdef TKS_DCON
         printf("TKScript::yacEvalMethodByName(\"%s\"): illegal class_ID (%i).\n", _name, clid);
#endif
      }
   }
   else
   {
#ifdef TKS_DCON
      printf("TKScript::yacEvalMethodByName(\"%s\"): illegal object pointer _o==" YAC_PRINTF_PTRPREFIX "%p.\n", _name, (void*)_o);
#endif
   }
   return 0;
}


void TKS_ScriptEngine::initMilliSeconds(void) {
#ifdef YAC_WIN32
#ifdef TKS_WIN32_USE_PERFCOUNTER
   ::QueryPerformanceFrequency(&pfcFrequency);
   milliSeconds.start = getMilliSeconds();
#else
   milliSeconds.start=GetTickCount();
#endif // TKS_WIN32_USE_PERFCOUNTER
#else
   milliSeconds.start=(void*) ::malloc(sizeof(struct timeval));
   ::gettimeofday((struct timeval*)milliSeconds.start, 0);
#endif
   tks_init_time();
}

void TKS_ScriptEngine::exitMilliSeconds(void) {
#ifndef YAC_WIN32
   ::free(milliSeconds.start);
#endif
   milliSeconds.start=0;
}

sUI TKS_ScriptEngine::getMilliSeconds(void) {
#ifdef YAC_WIN32
#ifdef TKS_WIN32_USE_PERFCOUNTER
   LARGE_INTEGER pfcCurrent;
   if( ::QueryPerformanceCounter(&pfcCurrent) )
   {
      sF64 s = ((sF64)pfcCurrent.QuadPart) / ((sF64)pfcFrequency.QuadPart);

      return (sUI) ((1000.0 * s) - milliSeconds.start);
   }
   else
   {
      return 0;
   }
#else
   return (sUI) (::GetTickCount() - milliSeconds.start);
#endif // TKS_WIN32_USE_PERFCOUNTER
#else
   struct timeval c; ::gettimeofday(&c, 0);
   struct timeval*s=(struct timeval*)milliSeconds.start;
   return (sUI)  ( (c.tv_sec-s->tv_sec)*1000 + (c.tv_usec-s->tv_usec)/1000 );
#endif
}

sUI TKS_ScriptEngine::getStartMilliSeconds(void) {
#ifdef YAC_WIN32
   return (sUI) milliSeconds.start;
#else
   struct timeval*s=(struct timeval*)milliSeconds.start;
   return (sUI)  ( s->tv_sec*1000 + s->tv_usec/1000 );
#endif
}

sF64 TKS_ScriptEngine::GetMilliSecondsDouble(void) {
   //
   // Note: This function is used for profiling purposes only
   //
#if defined(YAC_WIN32) && defined(YAC_VC)
   LARGE_INTEGER pfcCurrent;

#ifdef DX_RDTSC
   // Note: RTDSC is fast but may be unreliable if CPU reduces frequency to save power.
   //       QueryPerformanceCounter() is really really slow, btw.
   //       While the result may be either off or the counter is incrementing at a frequency
   //       other than the CPU frequency, the relation of the profiling results should remain the same.
   __asm {
      RDTSC
      mov pfcCurrent.HighPart, edx
      mov pfcCurrent.LowPart, eax
   }
#else
   ::QueryPerformanceCounter(&pfcCurrent);
#endif // DX_RDTSC
   {
      ////tkscript->printf("[...] [win32] performance counter = %08x.%08x\n", pfcCurrent.HighPart, pfcCurrent.LowPart);
      ////tkscript->printf("[...] [win32] performanceCounterFrequency = %08x.%08x\n", pfcFrequency.HighPart, pfcFrequency.LowPart );

      sF64 s = ((sF64)pfcCurrent.QuadPart) / ((sF64)pfcFrequency.QuadPart);
      sF64 r = (1000.0 * s);
#ifdef DX_RDTSC
      r *= pfc_scale;
#endif // DX_RDTSC
      return r;
   }
#else
   struct timeval tvnano; gettimeofday(&tvnano, NULL);
   // // // return (tvnano.tv_usec/1000.0) + ((sF64)(tvnano.tv_sec%10)) * 1000.0;
   return (tvnano.tv_usec/1000.0) + ((sF64)(tvnano.tv_sec)) * 1000.0;
#endif
}

void TKS_ScriptEngine::sleep(sUI _ms) {
#ifdef YAC_WIN32
   ::Sleep(_ms);
#endif

#ifdef YAC_POSIX
   ::usleep(1000 * _ms);
#endif
}

void TKS_ScriptEngine::usleep(sUI _us) {
#ifdef YAC_WIN32
   if(_us >= 1000u)
   {
      ::Sleep(_us / 1000u);
   }
   else
   {
      // busy wait
      sF64 tStart = GetMilliSecondsDouble();
      sF64 tEnd = tStart + (_us / 1000.0f);
      for(;;)
      {
         if(GetMilliSecondsDouble() >= tEnd)
            return;
      }
   }
#endif

#ifdef YAC_POSIX
   ::usleep(_us);
#endif
}

sUI tks_grow_array(sUI _num) {
   // sUI ret = (_num+1)+((sUI)(_num*0.33f));
   sUI ret = (_num+1u) + sUI(_num>>2);
   ret = sMAX(256u, ret);
   return ret;
}

// Very nasty hack that can/should probably only be used with event based systems (e.g. GUIs)
void TKS_SegVContinue(void)
{
   // Run script callback if available
   if(tkscript->sig_segvcontinue)
   {
       // TKS_MT: use *real* thread context (create new context?)
      void *ctx = tkscript->yacContextCreate();
      yac_host->yacEvalFunction(ctx, tkscript->sig_segvcontinue, 0, NULL);
      tkscript->yacContextDestroy(ctx);
   }

   tkscript->checkCompiletimeError();  // (TODO) obsolete ??!

   ((TKS_Compiler*)tkscript->compiler)->clean();
   Dprintf("[...] TKS_ScriptEngine::TKS_SegVContinue() finished.\n");

   ::exit(255);
}

#ifdef TKS_FIX_ABSTRACTVARS
void TKS_ScriptEngine::fixAllAbstractVars(void) {
   global_instance_namespace.fixAbstractVars();

   // Fix namespace variables
   if(namespaces.num_entries)
   {
      for(sUI i = 0u; i < namespaces.max_entries; i++)
      {
         if(namespaces.objects[i])
         {
            TKS_Namespace *nsp = (TKS_Namespace*) namespaces.objects[i]->value.object_val;
            nsp->global_vars.fixAbstractVars();
         }
      }
   }

#if 0
   // Fix static class members
   if(Dtkscompiler->class_decls.num_elements)
   {
      for(sUI i = 1u; i < Dtkscompiler->class_decls.num_elements; i++)
      {
         TKS_ClassDecl *cd = Dtkscompiler->class_decls.elements[i];
         if(cd->members.num_elements)
         {
            cd->fixAbstractVars();
         }
      }
   }
#endif // 0
}
#endif // TKS_FIX_ABSTRACTVARS

void TKS_ScriptEngine::initExceptions(void) {
   exception_types        = NULL;
   next_exception_id      = TKS_NUM_EXCEPTIONS;

   int i, j;
   for(i=0; i<TKS_MAX_EXCEPTIONS; i++)
   {
      for(j=0; j<TKS_MAX_EXCEPTIONS; j++)
      {
         exception_typecast_map[i][j] = 0;
      }
   }
   // Add core exceptions
   addExceptionType(new TKS_ExceptionType(0                                , TKS_EXCEPTION_ERROR                     , "Error"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_ERROR              , TKS_EXCEPTION_CRITICALERROR             , "CriticalError"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_ERROR              , TKS_EXCEPTION_UNCRITICALERROR           , "UncriticalError"));

   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CRITICALERROR      , TKS_EXCEPTION_INVALIDPOINTER            , "InvalidPointer"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CRITICALERROR      , TKS_EXCEPTION_DEATH                     , "Death"));
   ////addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CRITICALERROR      , TKS_EXCEPTION_OUTOFMEMORY               , "OutOfMemory"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CRITICALERROR      , TKS_EXCEPTION_TYPEMISMATCH              , "TypeMismatch"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_TYPEMISMATCH       , TKS_EXCEPTION_CLASSTYPEMISMATCH         , "ClassTypeMismatch"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CLASSTYPEMISMATCH  , TKS_EXCEPTION_NATIVECLASSTYPEMISMATCH   , "NativeClassTypeMismatch"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CLASSTYPEMISMATCH  , TKS_EXCEPTION_SCRIPTCLASSTYPEMISMATCH   , "ScriptClassTypeMismatch"));
   ////addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_TYPEMISMATCH       , TKS_EXCEPTION_DELEGATETYPEMISMATCH      , "DelegateTypeMismatch"));
   ////addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_TYPEMISMATCH       , TKS_EXCEPTION_FOREACHTYPEMISMATCH       , "ForeachTypeMismatch"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CRITICALERROR      , TKS_EXCEPTION_NOTFOUND                  , "NotFound"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_NOTFOUND           , TKS_EXCEPTION_CLASSMETHODNOTFOUND       , "ClassMethodNotFound"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CLASSMETHODNOTFOUND, TKS_EXCEPTION_NATIVECLASSMETHODNOTFOUND , "NativeClassMethodNotFound"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CLASSMETHODNOTFOUND, TKS_EXCEPTION_SCRIPTCLASSMETHODNOTFOUND , "ScriptClassMethodNotFound"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_NOTFOUND           , TKS_EXCEPTION_CLASSMEMBERNOTFOUND       , "ClassMemberNotFound"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CLASSMEMBERNOTFOUND, TKS_EXCEPTION_NATIVECLASSMEMBERNOTFOUND , "NativeClassMemberNotFound"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CLASSMEMBERNOTFOUND, TKS_EXCEPTION_SCRIPTCLASSMEMBERNOTFOUND , "ScriptClassMemberNotFound"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_NOTFOUND           , TKS_EXCEPTION_MODULENOTFOUND            , "ModuleNotFound"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_NOTFOUND           , TKS_EXCEPTION_MODULEMEMBERNOTFOUND      , "ModuleMemberNotFound"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CRITICALERROR      , TKS_EXCEPTION_ARRAYOUTOFBOUNDS          , "ArrayOutOfBounds"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_ARRAYOUTOFBOUNDS   , TKS_EXCEPTION_READARRAYOUTOFBOUNDS      , "ReadArrayOutOfBounds"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_ARRAYOUTOFBOUNDS   , TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS     , "WriteArrayOutOfBounds"));

   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CRITICALERROR      , TKS_EXCEPTION_CONSTRAINTVIOLATION       , "ConstraintViolation"));
   addExceptionType(new TKS_ExceptionType(TKS_EXCEPTION_CONSTRAINTVIOLATION, TKS_EXCEPTION_NOTNULLCONSTRAINTVIOLATION, "NotNullConstraintViolation"));
}

void TKS_ScriptEngine::freeExceptions(void) {
   if(NULL != exception_types)
   {
      delete exception_types;
      exception_types = NULL;
   }
}

YAC_ExceptionId YAC_VCALL TKS_ScriptEngine::yacExceptionRegister(const char *_name, YAC_ExceptionId _baseId) {
   TKS_ExceptionType *bt = getExceptionTypeById(_baseId);
   if(NULL != bt)
   {
      // Test whether the exception has already been declared resp. fwd-referenced
      YAC_String n; n.visit(_name);
      TKS_ExceptionType *t = getExceptionTypeByName(NULL, &n);
      if(NULL == t)
      {
         t = new TKS_ExceptionType();
         t->parent_exception = bt;
         t->name.copy(_name);
         t->is_declared = 1;
         return addExceptionType(t);
      }
      else
      {
         if(t->is_declared)
         {
            Dprintf("[---] failed to register exception \"%s\" because it has already been declared.\n");
            return 0;
         }
         t->is_declared = 1;
         t->parent_exception = bt;
         return addExceptionType2(t);
      }
   }
   else
   {
      Dprintf("[---] failed to register exception \"%s\" because base exception (id=%i) is unknown.\n",
         _name, _baseId);
   }
   return 0;
}

sUI TKS_ScriptEngine::addExceptionType(TKS_ExceptionType *_t) {
   _t->next_exception = exception_types;
   exception_types = _t;
   return addExceptionType2(_t);
}

sUI TKS_ScriptEngine::addExceptionType2(TKS_ExceptionType *_t) {
   if(_t->exception_id == 0xFFFFffff)
   {
      if( next_exception_id >= TKS_MAX_EXCEPTIONS)
      {
         Dprintf("[---] addExceptionType: max. number of exception types exceeded (%i).\n", TKS_MAX_EXCEPTIONS);
         delete _t;
         return 0;
      }
      _t->exception_id = next_exception_id++;
   }
   exception_typecast_map[_t->exception_id][TKS_EXCEPTION_ERROR] = 1; // allow cast to base type
   exception_typecast_map[_t->exception_id][_t->exception_id] = 1; // allow cast to self
   TKS_ExceptionType *pt = _t->parent_exception;
   while(NULL != pt)
   {
      // Allow casts to all parents of parent exception
      exception_typecast_map[_t->exception_id][pt->exception_id] = 1; // allow cast to super type
      pt = pt->parent_exception;
   }
   return _t->exception_id;
}

TKS_ExceptionType *TKS_ScriptEngine::addUndeclaredExceptionType(/*TKS_ClassDecl *_suggestedClass, */TKS_ClassDecl *_class, sUI _srcLoc, YAC_String *_name) {
   TKS_ExceptionType *t = new TKS_ExceptionType();
   t->name.yacCopy(_name);
   t->next_exception  = exception_types;
   t->src_loc         = _srcLoc;
   t->parent_class    = _class;
   ////t->suggested_class = _suggestedClass;
   exception_types = t;
   return t;
}

sBool TKS_ScriptEngine::areAllExceptionsDeclared(void) {
   TKS_ExceptionType *t = exception_types;
   while(NULL != t)
   {
      if(! t->is_declared)
      {
         Dprintf("[---] exception \"%s::%s\" (first referenced in module \"%s\" line %i) is undefined.\n",
                 (t->parent_class?(char*)t->parent_class->name.chars:""),
                 (char*) t->name.chars,
                 ((TKS_Compiler*)compiler)->getModuleNameByIndex(Dsrcloc_module(t->src_loc)),
                 Dsrcloc_linenr(t->src_loc)
                 );
         return 0;
      }
      t = t->next_exception;
   }
   return 1;
}

TKS_ExceptionType *TKS_ScriptEngine::getExceptionTypeById(sUI _id) {
   TKS_ExceptionType *t = exception_types;
   while(NULL != t)
   {
      if(t->exception_id == _id)
      {
         return t;
      }
      else
      {
         t = t->next_exception;
      }
   }
   return NULL;
}

TKS_ExceptionType *TKS_ScriptEngine::lazyGetExceptionTypeByName(/*TKS_ClassDecl *_suggestedClass, */TKS_ClassDecl *_class, sUI _srcLoc, YAC_String *_name) {
   TKS_ExceptionType *t = getExceptionTypeByName(_class, _name);
   if(t == NULL)
   {
      t = addUndeclaredExceptionType(/*_suggestedClass, */_class, _srcLoc, _name);
   }
   return t;
}

TKS_ExceptionType *TKS_ScriptEngine::getExceptionTypeByName(TKS_ClassDecl *_class, YAC_String *_name) {
   TKS_ExceptionType *t = exception_types;
   while(NULL != t)
   {
      if(t->name.compare(_name))
      {
         if(NULL != _class)
         {
            if(NULL != t->parent_class)
            {
               if(_class->ancestor_table[t->parent_class->class_id])
               {
                  return t;
               }
            }
         }
         else
         {
            return t;
         }
      }
      t = t->next_exception;
   }
   return NULL;
}

YAC_ExceptionId YAC_VCALL TKS_ScriptEngine::yacExceptionGetIdByName(const char *_name) {
   YAC_String s; s.visit((char*)_name);
   TKS_ExceptionType *t = exception_types;
   while(NULL != t)
   {
      if(t->name.compare(&s))
      {
         return t->exception_id;
      }
      else
      {
         t = t->next_exception;
      }
   }
   return 0;
}

void YAC_VCALL TKS_ScriptEngine::yacExceptionRaise(YAC_ContextHandle _context, YAC_ExceptionId _id,
                                                   const char *_message, const char *_file, sSI _line
                                                   ) {
   // xxx assumes that C++ methods raise exceptions only within the main thread !!!
   if(NULL != _context)
   {
      TKS_Context *context = (TKS_Context*) _context;
      context->raiseException(_id, _message, _file, _line, NULL);
   }
}

void TKS_ScriptEngine::initNamespaces(void) {
   namespaces.allocCache(TKS_MAX_NAMESPACES + 1);
   s_nspace_yac.visit("yac");
}

void TKS_ScriptEngine::freeNamespaceVars(void) {
   TKS_CachedObject **co = namespaces.objects;
   sUI i;
   for(i = 0u; i < namespaces.max_entries; i++)
   {
      if(*co)
      {
         TKS_Namespace *n = (TKS_Namespace*) (*co)->value.object_val;
         n->freeVars();
      }
      co++;
   }
}

void TKS_ScriptEngine::freeNamespaceTypes(void) {
   TKS_CachedObject **co = namespaces.objects;
   sUI i;
   for(i=0; i<namespaces.max_entries; i++)
   {
      if(*co)
      {
         TKS_Namespace *n = (TKS_Namespace*) (*co)->value.object_val;
         n->freeTypes();
      }
      co++;
   }
}

void TKS_ScriptEngine::freeNamespaces(void) {
   namespaces.freeCache();
}

TKS_Namespace *TKS_ScriptEngine::addNamespace(YAC_String *_name) {

   // Is "virtual" yac namespace ?
   if(s_nspace_yac.compare(_name))
   {
#ifdef TKS_DCON
      printf("[---] ScriptEngine: namespace \"yac\" already exists.\n");
#endif
      return NULL;
   }

   // Check whether user-defined namespace already exists
   TKS_CachedObject *co = namespaces.findEntry(_name);
   if(co)
   {
#ifdef TKS_DCON
      printf("[---] ScriptEngine::addNamespace: namespace \"%s\" already exists.\n",
         (char*)_name->chars);
#endif
   }
   else
   {
      TKS_Namespace *ns = new TKS_Namespace();
      ns->name.yacCopy(_name);

      if(NULL != namespaces.createObjectEntry(&ns->name, 0, (YAC_Object*)ns, 1))
      {
         return ns;
      }
      else
      {
#ifdef TKS_DCON
         printf("[---] ScriptEngine::addNamespace: failed to add namespace \"%s\".\n", (char*)_name->chars);
#endif
         return NULL;
      }
   }
   return NULL;
}

TKS_Namespace *TKS_ScriptEngine::findNamespace(YAC_String *_name) {

   // Is virtual "yac" namespace ?
   if(s_nspace_yac.compare(_name))
   {
      return &Dtkscompiler->yac_namespace;
   }
   else
   {
      // Find user defined namespace
      TKS_CachedObject *co = namespaces.findEntry(_name);
      if(co)
      {
         return (TKS_Namespace*) co->value.object_val;
      }
      else
      {
         return NULL;
      }
   }
}

void TKS_ScriptEngine::initCallbacks(void) {
   first_callback = NULL;
   next_callback_id = 0;
   for(int i=0; i < TKS_MAX_PLUGINCALLBACKS; i++)
   {
      callback_map[i] = NULL;
   }
}

void TKS_ScriptEngine::freeCallbacks(void) {
   TKS_PluginCallback *c = first_callback;
   while(NULL != c)
   {
      TKS_PluginCallback *n = c -> next;
      delete c;
      c = n;
   }
   first_callback = NULL;
}

TKS_PluginCallback *TKS_ScriptEngine::findCreateCallback(const char *_name, sBool _bCreate) {
   TKS_PluginCallback *c = first_callback;
   YAC_String name; name.visit(_name);
   while(NULL != c)
   {
      if(c->name.compare(&name))
      {
         if(_bCreate)
         {
            return NULL; // already exists
         }
         else
         {
            return c; // found existing
         }
      }
      c = c -> next;
   }
   if(_bCreate)
   {
      // Create new callback
      c = new TKS_PluginCallback;
      c->callback_id    = next_callback_id;
      c->cdecl_function = NULL;
      c->next           = first_callback;
      c->name.yacCopy(&name);
      callback_map[next_callback_id++] = c;
      first_callback = c;
      return c;
   }
   else
   {
      return NULL; // Not found
   }
}

YAC_CallbackId TKS_ScriptEngine::yacCallbackCreate (const char *_name) {
   TKS_PluginCallback *cb = findCreateCallback(_name, 1);

   if(NULL != cb)
   {
      return cb->callback_id;
   }
   else
   {
      // Unable to create/ callback already exists
      return -1;
   }
}

YAC_CallbackId TKS_ScriptEngine::yacCallbackGetIdByName (const char *_name) {
   TKS_PluginCallback *cb = findCreateCallback(_name, 0);

   if(NULL != cb)
   {
      return cb->callback_id;
   }
   else
   {
      // Callback was not found
      return -1;
   }
}

sBool TKS_ScriptEngine::yacCallbackSetFunById(YAC_CallbackId _callbackId, YAC_CFunctionPtr _fun) {
   sUI callbackId = (sUI) _callbackId;
   if(callbackId < TKS_MAX_PLUGINCALLBACKS)
   {
      TKS_PluginCallback *cb = callback_map[_callbackId];
      // printf("xxx TKS_ScriptEngine::yacCallbackSetFunById: callbackId=%d cb=%p fun=%p\n", _callbackId, cb, _fun);
      if(NULL != cb)
      {
         cb->cdecl_function = _fun;
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

sSI TKS_ScriptEngine::yacCallbackSetFunByName(const char *_name, YAC_CFunctionPtr _fun) {
   TKS_PluginCallback *cb = findCreateCallback(_name, YAC_TRUE/*bCreate*/);
   if(NULL != cb)
   {
      cb->cdecl_function = _fun;
      return cb->callback_id;
   }
   else
   {
      // Failed to create callback
      return -1;
   }
}

YAC_CFunctionPtr TKS_ScriptEngine::yacCallbackGetFunById(sSI _callbackId) {
   sUI callbackId = (sUI) _callbackId;
   if(callbackId < TKS_MAX_PLUGINCALLBACKS)
   {
      TKS_PluginCallback *cb = callback_map[_callbackId];
      // printf("xxx TKS_ScriptEngine::yacCallbackGetFunById: callbackId=%d => cb=%p\n", _callbackId, cb);
      if(NULL != cb)
      {
         // printf("xxx TKS_ScriptEngine::yacCallbackGetFunById: callbackId=%d => cb=%p cb->cdecl_function=%p\n", _callbackId, cb, cb->cdecl_function);
         return cb->cdecl_function;
      }
   }
   return NULL;
}

YAC_ContextHandle TKS_ScriptEngine::yacContextCreate(void) {
   for(int i=0;i<TKS_MAX_CONTEXTS; i++)
   {
      if(contexts[i] == NULL)
      {
         contexts[i] = new TKS_Context();
         contexts[i]->b_running = 1;
         // printf("xxx yacContextCreate: ctx=%p (%u)\n", contexts[i], i);
         return (YAC_ContextHandle) contexts[i];
      }
   }
   Dprintf("[---] yacContextCreate() failed.\n");
   return NULL;
}

void  TKS_ScriptEngine::yacContextDestroy (YAC_ContextHandle _context) {
   for(int i=0; i<TKS_MAX_CONTEXTS; i++)
   {
      if( (YAC_ContextHandle)contexts[i] == _context )
      {
         TKS_Context *context = (TKS_Context*) _context;
         // printf("xxx yacContextDestroy: ctx=%p (%u)\n", contexts[i], i);
         delete context;
         contexts[i] = NULL;
         return;
      }
   }
   Dprintf("[---] yacContextDestroy() failed (unregistered context).\n");
}

YAC_ContextHandle YAC_VCALL TKS_ScriptEngine::yacContextGetDefault (void) {
#ifndef YAC_FORCE_NO_TLS
   if(NULL != tks_current_thread)
   {
      return (YAC_ContextHandle) tks_current_thread->context;
   }
   else
   {
      // Note: null context ?
      //    - thread was started in a plugin
      //    - plugin thread did not alloc/set script context but calls script functions
      //    - may happen if threading is done by 3rd party library (e.g. MIDI timer in win32 API)
      //    --> silently initialize thread pointer (--> will be cleant up when object pool is destroyed)
      if(NULL == tks_current_context)
      {
         Dprintf("[~~~] TKS_ScriptEngine::yacContextGetDefault: warning: implicit thread context allocation..\n");
         Dprintf("[~~~] TKS_ScriptEngine::yacContextGetDefault:           please set context via yacContextSetDefault() in your plugin thread !\n");
         tks_current_thread = (TKS_Thread*) YAC_NEW_CORE_POOLED(TKS_CLID_THREAD);
         tks_current_thread->initFromCurrent();
         return (YAC_ContextHandle) tks_current_thread->context;
      }
      else
      {
         ////printf("xxx yacContextGetDefault: tks_current_context=%p\n", tks_current_context);
         return tks_current_context;
      }
   }
#else
   return (YAC_ContextHandle) main_context;
#endif // YAC_FORCE_NO_TLS
}

void YAC_VCALL TKS_ScriptEngine::yacContextSetDefault (YAC_ContextHandle _context) {
#ifndef YAC_FORCE_NO_TLS
   tks_current_context = _context;
#endif // YAC_FORCE_NO_TLS
}

#ifdef TKS_MT
void TKS_ScriptEngine::initMutexes(void) {
   for(sUI i = 0u; i < TKS_MAX_NAMED_MUTEXES; i++)
      named_mutexes[i] = NULL;  // (todo) memset

   mtx_global = new TKS_Mutex();
   mtx_print = new TKS_Mutex();
}

void TKS_ScriptEngine::freeMutexes(void) {
   for(int i=0; i<TKS_MAX_NAMED_MUTEXES; i++)
   {
      if(NULL != named_mutexes[i])
      {
         delete named_mutexes[i];
         named_mutexes[i] = NULL;
      }
   }
   if(NULL != mtx_global)
   {
      delete mtx_global;
      mtx_global = NULL;
   }
   if(NULL != mtx_print)
   {
      delete mtx_print;
      mtx_print = NULL;
   }
}
#endif //TKS_MT

YAC_MutexHandle TKS_ScriptEngine::yacMutexCreate (void) {
#ifdef TKS_MT
   for(int i=0; i<TKS_MAX_NAMED_MUTEXES; i++)
   {
      if(named_mutexes[i] == NULL)
      {
         named_mutexes[i] = new TKS_NamedMutex();
         return (YAC_MutexHandle) named_mutexes[i];
      }
   }
#endif //TKS_MT
   Dprintf("[---] yacMutexCreate() failed (max. number of mutexes exceeded).\n");
   return NULL;
}


#ifdef TKS_MT
TKS_NamedMutex *TKS_ScriptEngine::findNamedMutex (YAC_String *_name) {
   for(sUI i = 0u; i < TKS_MAX_NAMED_MUTEXES; i++)
   {
      if(NULL != named_mutexes[i])
      {
         if(named_mutexes[i]->name.compare(_name))
            return named_mutexes[i];
      }
   }
   return NULL; // not found
}

TKS_NamedMutex *TKS_ScriptEngine::createNamedMutex (YAC_String *_name) {
   for(int i=0; i<TKS_MAX_NAMED_MUTEXES; i++)
   {
      if(NULL == named_mutexes[i])
      {
         named_mutexes[i] = new TKS_NamedMutex();
         named_mutexes[i]->name.yacCopy(_name);
         return named_mutexes[i];
      }
   }
   Dprintf("[---] createNamedMutex() failed (max. number of mutexes exceeded).\n");
   return NULL;
}
#endif //TKS_MT

/*
// xxx this will be used as soon as multiple TKS_Compiler instances are supported
  void TKS_ScriptEngine::destroyNamedMutex (TKS_Mutex *_mutex) {
   for(int i=0; i<TKS_MAX_MUTEXES; i++)
   {
      if( mutexes[i] == _mutex)
      {
         delete mutexes[i];
         mutexes[i] = NULL;
         return;
      }
   }
#ifdef TKS_DCON
   tkscript->printf("[---] destroyNamedMutex () failed (invalid mutex).\n");
#endif
}
*/

void TKS_ScriptEngine::yacMutexDestroy(YAC_MutexHandle _mutexHandle) {
#ifdef TKS_MT
   for(sUI i = 0u; i < TKS_MAX_NAMED_MUTEXES; i++)
   {
      if( ((YAC_MutexHandle)named_mutexes[i]) == _mutexHandle)
      {
         if(named_mutexes[i]->name.isBlank())
         {
            delete named_mutexes[i];
            named_mutexes[i] = NULL;
         }
         else
         {
            Dprintf("[---] yacMutexDestroy() failed (delete named mutex).\n");
         }
         return;
      }
   }
#endif //TKS_MT
   Dprintf("[---] yacMutexDestroy() failed (invalid mutex handle).\n");
}

YAC_MutexHandle TKS_ScriptEngine::yacMutexFindByName (const char *_name) {
#ifdef TKS_MT
   YAC_String name; name.visit(_name);
   /*if(name.compare("__DEBUG"))
   {
      int xxx = 42;
   }*/
   for(sUI i = 0u; i < TKS_MAX_NAMED_MUTEXES; i++)
   {
      if(NULL != named_mutexes[i])
      {
         if(named_mutexes[i]->name.compare(&name) )
         {
            return (YAC_MutexHandle) named_mutexes[i];
         }
      }
   }
#endif //TKS_MT
   return NULL; // mutex not found
}

void TKS_ScriptEngine::yacMutexLock(YAC_MutexHandle _mutexHandle) {
#ifdef TKS_MT
   if(NULL != _mutexHandle)
   {
      TKS_Mutex *mutex = (TKS_Mutex*) _mutexHandle;
      mutex->lock();
   }
   else
   {
      printf("[---] TKS::yacMutexLock: mutexHandle is NULL !!\n");
   }
#endif //TKS_MT
}

void TKS_ScriptEngine::yacMutexUnlock(YAC_MutexHandle _mutexHandle) {
#ifdef TKS_MT
   if(NULL != _mutexHandle)
   {
      TKS_Mutex *mutex = (TKS_Mutex*) _mutexHandle;
      mutex->unlock();
   }
   else
   {
      printf("[---] TKS::yacMutexUnlock: mutexHandle is NULL !!\n");
   }
#endif //TKS_MT
}

#if (__GNUC__ >= 4) && ((__GNUC__ > 4) || (__GNUC_MINOR__ >= 1))
// ---- GCC / Linux ----
sSI TKS_ScriptEngine::yacAtomicInc(volatile sSI *_v, sSI _inc) {
   return __sync_add_and_fetch(_v, _inc);
}

sSI TKS_ScriptEngine::yacAtomicDec(volatile sSI *_v, sSI _dec) {
   return __sync_sub_and_fetch(_v, _dec);
}

sBool TKS_ScriptEngine::yacAtomicCompareAndSwap(volatile sSI *_v, sSI _old, sSI _new) {
   return (sBool)__sync_bool_compare_and_swap(_v, _old, _new);
}

sSI TKS_ScriptEngine::yacAtomicGet(volatile sSI *_v) {
   for(;;)
   {
      sSI val = *_v;
      if(__sync_bool_compare_and_swap(_v, val, val))
         return val;
   }
}

void TKS_ScriptEngine::yacAtomicSet(volatile sSI *_v, sSI _new) {
   for(;;)
   {
      sSI old = *_v;
      if(__sync_bool_compare_and_swap(_v, old, _new))
         return;
   }
}

#elif defined(YAC_MACOS)
// ---- macOS ----
//  (note) usually the GCC/CLANG codepath^^ (__sync_*()) is taken on macOS, too
sSI TKS_ScriptEngine::yacAtomicInc(volatile sSI *_v, sSI _inc) {
   return OSAtomicAdd32Barrier(_inc, _v);
}

sSI TKS_ScriptEngine::yacAtomicDec(volatile sSI *_v, sSI _dec) {
   return OSAtomicAdd32Barrier(-_dec, _v);
}

sBool TKS_ScriptEngine::yacAtomicCompareAndSwap(volatile sSI *_v, sSI _old, sSI _new) {
   // returns true if the swap occured
   return (sBool)OSAtomicCompareAndSwapIntBarrier(_old, _new, _v);
}

void TKS_ScriptEngine::yacAtomicSet(volatile sSI *_v, sSI _new) {
   for(;;)
   {
      sSI old = *_v;
      if(OSAtomicCompareAndSwap32Barrier(old, _new, _v))
         return;
   }
}

sSI TKS_ScriptEngine::yacAtomicGet(volatile sSI *_v) {
   for(;;)
   {
      sSI value = *_v;
      if(OSAtomicCompareAndSwap32Barrier(value, value, _v))
         return value;
   }
}

#elif defined(YAC_WIN32)
// ---- Windows ----
sSI TKS_ScriptEngine::yacAtomicInc(volatile sSI *_v, sSI _inc) {
   return (sSI)InterlockedAdd((volatile long*)_v, (long)_inc);
}

sSI TKS_ScriptEngine::yacAtomicDec(volatile sSI *_v, sSI _dec) {
   return (sSI)InterlockedAdd((volatile long*)_v, (long)-_dec);
}

sBool TKS_ScriptEngine::yacAtomicCompareAndSwap(volatile sSI *_v, sSI _old, sSI _new) {
   return (_InterlockedCompareExchange((volatile long*)_v, _new, _old) == _old);
}

void TKS_ScriptEngine::yacAtomicSet(volatile sSI *_v, sSI _new) {
   (void)_InterlockedExchange((volatile long*)_v, (long)_new);
}

sSI TKS_ScriptEngine::yacAtomicGet(volatile sSI *_v) {
   return (sSI)InterlockedAdd((volatile long*)_v, (long)0);
}

#else
// ---- Unknown OS (STUB) ----
sSI TKS_ScriptEngine::yacAtomicInc(volatile sSI *_v, sSI _inc) {
   printf("[~~~] TKS_ScriptEngine::yacAtomicInc: STUB\n");
   *_v += _inc;
   return *_v;
}

sSI TKS_ScriptEngine::yacAtomicDec(volatile sSI *_v, sSI _dec) {
   printf("[~~~] TKS_ScriptEngine::yacAtomicDec: STUB\n");
   *_v -= _dec;
   return *_v;
}

sBool TKS_ScriptEngine::yacAtomicCompareAndSwap(volatile sSI *_v, sSI _old, sSI _new) {
   printf("[~~~] TKS_ScriptEngine::yacAtomicCompareAndSwap: STUB\n");
   sSI old = *_v;
   if(*_v == _old)
   {
      *_v = _new;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

void TKS_ScriptEngine::yacAtomicSet(volatile sSI *_v, sSI _new) {
   printf("[~~~] TKS_ScriptEngine::yacAtomicSet: STUB\n");
   *_v = _new;
}

sSI TKS_ScriptEngine::yacAtomicGet(volatile sSI *_v) {
   printf("[~~~] TKS_ScriptEngine::yacAtomicGet: STUB\n");
   return *_v;
}
#endif

YAC_ThreadHandle TKS_ScriptEngine::yacThreadNew(void) {
   TKS_Thread *th = (TKS_Thread*)yacNewByID(TKS_CLID_THREAD);
   return (YAC_ThreadHandle)th;
}

void TKS_ScriptEngine::yacThreadAllocEventQueue(YAC_ThreadHandle _thread, sSI _size) {
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      th->_allocEventQueue(_size);
   }
}

sBool TKS_ScriptEngine::yacThreadStart(YAC_ThreadHandle _thread, yac_thread_fxn_t _fxn, void *_userData) {
   sBool r = YAC_FALSE;
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      // (note) userData is never dereferenced, does not need to be an actual Object
      r = th->create2(_fxn, (YAC_Object*)_userData);
   }
   return r;
}

void TKS_ScriptEngine::yacThreadSetPriority(YAC_ThreadHandle _thread, yac_thread_priority_t _priority) {
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      th->_setPriority(_priority);
   }
}

void TKS_ScriptEngine::yacThreadSetCPUCore(YAC_ThreadHandle _thread, sSI _coreIdx) {
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      th->_setCPUCore(_coreIdx/*-1==any*/);
   }
}

void TKS_ScriptEngine::yacThreadSendEvent(YAC_ThreadHandle _thread, YAC_Event *_event) {
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      th->sendEvent2(_event);
   }
}

YAC_Event *TKS_ScriptEngine::yacThreadPeekEvent(YAC_ThreadHandle _thread) {
   YAC_Event *r = NULL;
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      r = (YAC_Event*)th->_peekEvent();
   }
   return r;
}

YAC_Event *TKS_ScriptEngine::yacThreadPeekEventById(YAC_ThreadHandle _thread, sSI _id) {
   YAC_Event *r = NULL;
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      r = (YAC_Event*)th->_peekEventById(_id);
   }
   return r;
}

YAC_Event *TKS_ScriptEngine::yacThreadWaitEvent(YAC_ThreadHandle _thread, sUI _timeoutMS) {
   YAC_Event *r = NULL;
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      YAC_Value rv;
      th->_waitEvent(_timeoutMS, &rv);
      r = (YAC_Event*)rv.value.object_val;
      rv.deleteme = YAC_FALSE;
   }
   return r;
}

YAC_Event *TKS_ScriptEngine::yacThreadWaitEventById(YAC_ThreadHandle _thread, sSI _id, sUI _timeoutMS) {
   YAC_Event *r = NULL;
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      YAC_Value rv;
      th->_waitEventById(_id, _timeoutMS, &rv);
      r = (YAC_Event*)rv.value.object_val;
   }
   return r;
}

sUI TKS_ScriptEngine::yacThreadRand(YAC_ThreadHandle _thread) {
   sUI r = 0u;
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      r = th->_rand();
   }
   return r;
}

void TKS_ScriptEngine::yacThreadSRand(YAC_ThreadHandle _thread, sUI _seed) {
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      th->_srand(_seed);
   }
}

void TKS_ScriptEngine::yacThreadYield(void) {
   (void)TKS_Thread::Yield2();
}

void TKS_ScriptEngine::yacThreadDelay(void) {
   (void)TKS_Thread::Delay2();
}

void TKS_ScriptEngine::yacThreadWait(YAC_ThreadHandle _thread) {
   if(NULL != _thread)
   {
      YAC_CAST_ARG(TKS_Thread, th, _thread);
      th->_wait();
   }
}

void TKS_ScriptEngine::yacThreadDelete(YAC_ThreadHandle _thread) {
   yacDelete((YAC_Object*)_thread);
}

YAC_ThreadHandle TKS_ScriptEngine::yacThreadNewFromCurrent(void) {
   TKS_Thread *th = (TKS_Thread*)yacNewByID(TKS_CLID_THREAD);
   if(NULL != th)
   {
      th->initFromCurrent();
#ifndef YAC_FORCE_NO_TLS
      tks_current_thread = th;
#endif // HAVE_TLS
   }
   return (YAC_ThreadHandle)th;
}


YAC_String            *tks_empty_string;
yacmemptr              tksvm_zero_args;

#ifdef DX_TRACEFUNCTIONCALLS
TKS_FunctionCallStackEntry  *tks_fcallstack;
sSI                          tks_fcallstacki;
#endif

#ifdef TKS_POOL_MAX_STRINGS
Pool *tks_pool_String = NULL;
#endif
