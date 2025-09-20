/// TKS_ScriptEngine.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#ifndef TKS_SCRIPTENGINE_H__
#define TKS_SCRIPTENGINE_H__


class TKS_ScriptClassInstance;
class TKS_ScriptConstraint;
class TKS_FunctionCallStackEntry;
class PTN_Node;
class PTN_Function;
class Pool;
class TKS_Context;
class TKS_Thread;
class TKS_LinearAllocator;

// Pseudo tokens for builtin functions with more than one argument
#define TKS_BFUN2_RGB      0x40
#define TKS_BFUN2_ARGB     0x41
#define TKS_BFUN2_RGBA_4B  0x42
#define TKS_BFUN2_LAST     0x43

// Pseudo tokens for builtin procedures, also see PTN_BuiltinFun.h for builtin function ids
#define TKS_BPROC_FIRST    0x80 // index of first function, used to determine whether a pseudo token represents a function or a procedure
#define TKS_BPROC_STDOUT   0x80
#define TKS_BPROC_STDERR   0x81
#define TKS_BPROC_DCON     0x82
#define TKS_BPROC_DIE      0x83
#define TKS_BPROC_PRINT    0x84
#define TKS_BPROC_TRACE    0x85
#define TKS_BPROC_DTRACE   0x86
#define TKS_BPROC_STDFLUSH 0x87
#define TKS_BPROC_CLAMP    0x88
#define TKS_BPROC_WRAP     0x89
#define TKS_BPROC_LAST     0x8A

struct TKS_PrintHook {
   yac_printhook_fxn_t  fxn;
   YAC_String          *output;
   void                *userdata;
   TKS_PrintHook       *next;
};

class TKS_ScriptEngine : public YAC_Host {
public:
   // ---- YAC_Host virtual method interface
   sUI                YAC_VCALL yacQueryInterfaces       (void) {return YAC_HOST_INTERFACE_ALL;}
   sUI                YAC_VCALL yacGetDebugLevel         (void);
   sUI                YAC_VCALL yacRunning               (void);
   void               YAC_VCALL yacPrint                 (const sChar *_s); // _sid: 0=dcon, 1=STDOUT, 2=STDERR
   void               YAC_VCALL yacPrintHookAdd          (yac_printhook_fxn_t _fxn, YAC_String *_output, void *_userdata);
   void               YAC_VCALL yacPrintHookRemove       (yac_printhook_fxn_t _fxn, YAC_String *_output, void *_userdata);
   sUI                YAC_VCALL yacRegisterClass         (YAC_Object *_template, sUI _may_instantiate); // will be deleted upon clean()
   sUI                YAC_VCALL yacRegisterFunction      (void *_adr, const char *_name, sUI _returntype, const char *_return_otype, sUI _numArgs, const sUI *_argtypes, const char **_argtypenames, sUI _callstyle);

   YAC_Object *       YAC_VCALL yacNew                   (const char *_namespace, const char *_classname);
   YAC_Object *       YAC_VCALL yacNewByID               (sUI _classID);
   void               YAC_VCALL yacDelete                (YAC_Object *_apiobject);
   YAC_Object *       YAC_VCALL yacNewPooledByID         (sUI _classID, sUI _poolHint);
   void               YAC_VCALL yacNewDeleteModifyCounter(sSI _deltaByteSize);
   sSI                YAC_VCALL yacNewDeleteGetCounter   (void);
   YAC_Object *       YAC_VCALL yacGetClassTemplateByID  (sUI _id);

   void               YAC_VCALL yacSendUserEvent         (YAC_Object *);
   sUI                YAC_VCALL yacMilliSeconds          (void);

   sUI                YAC_VCALL yacEvalFunction            (YAC_ContextHandle _context, YAC_FunctionHandle _yac_fun, sUI _numArgs, YAC_Value *_args);
   sUI                YAC_VCALL yacEvalFunctionReturn      (YAC_ContextHandle _context, YAC_FunctionHandle _yac_fun, sUI _numArgs, YAC_Value *_args, YAC_Value *_r);
   sSI                YAC_VCALL yacEvalMethodByName        (YAC_Object *, const char*, YAC_Value*, sUI, YAC_Value *);
   YAC_FunctionHandle YAC_VCALL yacFindFunction            (sChar *_name);
   YAC_ModuleHandle   YAC_VCALL yacCompileModule           (sChar *_source);
   YAC_FunctionHandle YAC_VCALL yacFindFunctionInModule    (YAC_ModuleHandle _mod, sChar *_name);
   void               YAC_VCALL yacDeleteModule            (YAC_ModuleHandle _mod);
   YAC_VariableHandle YAC_VCALL yacFindVariableInModule    (YAC_ModuleHandle _mod, sChar *_name);
   void               YAC_VCALL yacSetVariable             (YAC_VariableHandle _var, YAC_Value *_v);
   void               YAC_VCALL yacGetVariable             (YAC_VariableHandle _var, YAC_Value *_r);
   YAC_VariableHandle YAC_VCALL yacFindVariableInFunction  (YAC_FunctionHandle _fn, sChar *_name);
   sUI                YAC_VCALL yacGetClassIDByName        (sChar *_name);

   sUI                YAC_VCALL yacScanFlags               (YAC_String *_s);
   sSI                YAC_VCALL yacStringReplace           (YAC_String *_d, YAC_String *_a, YAC_String *_b);
   sSI                YAC_VCALL yacStringScan              (YAC_String *,sSI*);
   sSI                YAC_VCALL yacStringScan              (YAC_String *,sF32*);

   YAC_ExceptionId    YAC_VCALL yacExceptionRegister       (const char *, YAC_ExceptionId _baseId);
   YAC_ExceptionId    YAC_VCALL yacExceptionGetIdByName    (const char *);
   void               YAC_VCALL yacExceptionRaise          (YAC_ContextHandle _context, YAC_ExceptionId _id, const char *_message=NULL, const char *_file=NULL, sSI _line=0);

   YAC_CallbackId     YAC_VCALL yacCallbackCreate          (const char *_name);
   YAC_CallbackId     YAC_VCALL yacCallbackGetIdByName     (const char *_name);
   sBool              YAC_VCALL yacCallbackSetFunById      (YAC_CallbackId _callbackId, YAC_CFunctionPtr _fun);
   sSI                YAC_VCALL yacCallbackSetFunByName    (const char *_name, YAC_CFunctionPtr _fun);
   YAC_CFunctionPtr   YAC_VCALL yacCallbackGetFunById      (YAC_CallbackId _callbackId);

   YAC_ContextHandle  YAC_VCALL yacContextCreate           (void);
   void               YAC_VCALL yacContextDestroy          (YAC_ContextHandle _context);
   YAC_ContextHandle  YAC_VCALL yacContextGetDefault       (void);
   void               YAC_VCALL yacContextSetDefault       (YAC_ContextHandle _context);

   YAC_MutexHandle    YAC_VCALL yacMutexCreate             (void);
   void               YAC_VCALL yacMutexDestroy            (YAC_MutexHandle _mutexHandle);
   YAC_MutexHandle    YAC_VCALL yacMutexFindByName         (const char *_name);
   void               YAC_VCALL yacMutexLock               (YAC_MutexHandle _mutexHandle);
   void               YAC_VCALL yacMutexUnlock             (YAC_MutexHandle _mutexHandle);

   sSI                YAC_VCALL yacAtomicInc               (volatile sSI *_v, sSI _inc);
   sSI                YAC_VCALL yacAtomicDec               (volatile sSI *_v, sSI _dec);
   sBool              YAC_VCALL yacAtomicCompareAndSwap    (volatile sSI *_v, sSI _old, sSI _new);
   void               YAC_VCALL yacAtomicSet               (volatile sSI *_v, sSI _new);
   sSI                YAC_VCALL yacAtomicGet               (volatile sSI *_v);

   YAC_ThreadHandle YAC_VCALL yacThreadNew               (void);
   void             YAC_VCALL yacThreadAllocEventQueue   (YAC_ThreadHandle _thread, sSI _size);
   sBool            YAC_VCALL yacThreadStart             (YAC_ThreadHandle _thread, yac_thread_fxn_t _fxn, void *_userData);
   void             YAC_VCALL yacThreadSetPriority       (YAC_ThreadHandle _thread, yac_thread_priority_t _priority);
   void             YAC_VCALL yacThreadSetCPUCore        (YAC_ThreadHandle _thread, sSI _coreIdx);
   void             YAC_VCALL yacThreadSendEvent         (YAC_ThreadHandle _thread, YAC_Event *_event);
   YAC_Event *      YAC_VCALL yacThreadPeekEvent         (YAC_ThreadHandle _thread);
   YAC_Event *      YAC_VCALL yacThreadPeekEventById     (YAC_ThreadHandle _thread, sSI _id);
   YAC_Event *      YAC_VCALL yacThreadWaitEvent         (YAC_ThreadHandle _thread, sUI _timeoutMS);
   YAC_Event *      YAC_VCALL yacThreadWaitEventById     (YAC_ThreadHandle _thread, sSI _id, sUI _timeoutMS);
   sUI              YAC_VCALL yacThreadRand              (YAC_ThreadHandle _thread);
   void             YAC_VCALL yacThreadSRand             (YAC_ThreadHandle _thread, sUI _seed);
   void             YAC_VCALL yacThreadYield             (void);
   void             YAC_VCALL yacThreadDelay             (void);
   void             YAC_VCALL yacThreadWait              (YAC_ThreadHandle _thread);
   void             YAC_VCALL yacThreadDelete            (YAC_ThreadHandle _thread);
   YAC_ThreadHandle YAC_VCALL yacThreadNewFromCurrent    (void);


   //
   // stubs for reserved vtable entries
   //
   void               YAC_VCALL vtable_entry_0_12_reserved (void) { }
   void               YAC_VCALL vtable_entry_0_13_reserved (void) { }
   void               YAC_VCALL vtable_entry_0_14_reserved (void) { }
   void               YAC_VCALL vtable_entry_0_15_reserved (void) { }

   void               YAC_VCALL vtable_entry_1_2_reserved  (void) { }
   void               YAC_VCALL vtable_entry_1_3_reserved  (void) { }
   void               YAC_VCALL vtable_entry_1_4_reserved  (void) { }
   void               YAC_VCALL vtable_entry_1_5_reserved  (void) { }
   void               YAC_VCALL vtable_entry_1_6_reserved  (void) { }
   void               YAC_VCALL vtable_entry_1_7_reserved  (void) { }

   void               YAC_VCALL vtable_entry_2_4_reserved  (void) { }
   void               YAC_VCALL vtable_entry_2_5_reserved  (void) { }
   void               YAC_VCALL vtable_entry_2_6_reserved  (void) { }
   void               YAC_VCALL vtable_entry_2_7_reserved  (void) { }
   void               YAC_VCALL vtable_entry_2_8_reserved  (void) { }
   void               YAC_VCALL vtable_entry_2_9_reserved  (void) { }
   void               YAC_VCALL vtable_entry_2_10_reserved (void) { }
   void               YAC_VCALL vtable_entry_2_11_reserved (void) { }
   void               YAC_VCALL vtable_entry_2_12_reserved (void) { }
   void               YAC_VCALL vtable_entry_2_13_reserved (void) { }
   void               YAC_VCALL vtable_entry_2_14_reserved (void) { }
   void               YAC_VCALL vtable_entry_2_15_reserved (void) { }

   void               YAC_VCALL vtable_entry_3_15_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_16_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_17_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_18_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_19_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_20_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_21_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_22_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_23_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_24_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_25_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_26_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_27_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_28_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_29_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_30_reserved (void) { }
   void               YAC_VCALL vtable_entry_3_31_reserved (void) { }

   void               YAC_VCALL vtable_entry_4_1_reserved  (void) { }
   void               YAC_VCALL vtable_entry_4_2_reserved  (void) { }
   void               YAC_VCALL vtable_entry_4_3_reserved  (void) { }
   void               YAC_VCALL vtable_entry_4_4_reserved  (void) { }
   void               YAC_VCALL vtable_entry_4_5_reserved  (void) { }
   void               YAC_VCALL vtable_entry_4_6_reserved  (void) { }
   void               YAC_VCALL vtable_entry_4_7_reserved  (void) { }

   void               YAC_VCALL vtable_entry_5_3_reserved  (void) { }
   void               YAC_VCALL vtable_entry_5_4_reserved  (void) { }
   void               YAC_VCALL vtable_entry_5_5_reserved  (void) { }
   void               YAC_VCALL vtable_entry_5_6_reserved  (void) { }
   void               YAC_VCALL vtable_entry_5_7_reserved  (void) { }
   void               YAC_VCALL vtable_entry_5_8_reserved  (void) { }
   void               YAC_VCALL vtable_entry_5_9_reserved  (void) { }

   void               YAC_VCALL vtable_entry_6_6_reserved  (void) { }
   void               YAC_VCALL vtable_entry_6_7_reserved  (void) { }
   void               YAC_VCALL vtable_entry_6_8_reserved  (void) { }
   void               YAC_VCALL vtable_entry_6_9_reserved  (void) { }

   void               YAC_VCALL vtable_entry_7_5_reserved  (void) { }
   void               YAC_VCALL vtable_entry_7_6_reserved  (void) { }
   void               YAC_VCALL vtable_entry_7_7_reserved  (void) { }
   void               YAC_VCALL vtable_entry_7_8_reserved  (void) { }
   void               YAC_VCALL vtable_entry_7_9_reserved  (void) { }

   void               YAC_VCALL vtable_entry_8_3_reserved  (void) { }
   void               YAC_VCALL vtable_entry_8_4_reserved  (void) { }
   void               YAC_VCALL vtable_entry_8_5_reserved  (void) { }
   void               YAC_VCALL vtable_entry_8_6_reserved  (void) { }
   void               YAC_VCALL vtable_entry_8_7_reserved  (void) { }
   void               YAC_VCALL vtable_entry_8_8_reserved  (void) { }
   void               YAC_VCALL vtable_entry_8_9_reserved  (void) { }

   void               YAC_VCALL vtable_entry_9_17_reserved (void) { }
   void               YAC_VCALL vtable_entry_9_18_reserved (void) { }
   void               YAC_VCALL vtable_entry_9_19_reserved (void) { }
   void               YAC_VCALL vtable_entry_9_20_reserved (void) { }


   // _r may be NULL
   sBool evalFunction(YAC_ContextHandle _context,
                      YAC_FunctionHandle _yac_fun,
                      sUI _numArgs, YAC_Value *_args,
                      YAC_Value *_r
                      );
   sBool evalFunction2(YAC_ContextHandle _context,
                       PTN_Function *_f,
                       sUI _numArgs, YAC_Value *_args,
                       YAC_Value *_r
                       );
   sBool evalScriptMethodByName(YAC_ContextHandle _context,
                                TKS_ScriptClassInstance *_o,
                                const char *_name,
                                YAC_Value *_args, sUI _numArgs,
                                YAC_Value *_r
                                );

   // ---- plugin2plugin callbacks ----
   TKS_PluginCallback *first_callback;
   sSI next_callback_id;
   TKS_PluginCallback *callback_map[TKS_MAX_PLUGINCALLBACKS];

   void initCallbacks(void);
   void freeCallbacks(void);
   TKS_PluginCallback *findCreateCallback(const char *_name, sBool _bCreate);

   // ---- resources/compiler internals ----
   TKX_PakFile      tkx;
   void            *compiler; // TKSCompiler *compiler;

   // ---- configuration ----
   TKS_Configuration configuration;

   // ---- state ----
   sBool            b_runtime;      // 0=add self to class initializers when class c'tor is run
   sBool            b_initializing; // 1=safe to call class constructors/destructors
   sBool            b_initclasses;  // 1=remove scriptobject from initializer list if they are deleted during init
   sSI              i_return;
   TKS_Thread *     main_thread;
   TKS_Context *    main_context;
   TKS_Context *    temp_context;
   TKS_Context *    contexts[TKS_MAX_CONTEXTS];
   void             initContexts (void);
   void             freeContexts (void);
   TKS_Context *    lockGlobalContext   (void); // used for all non thread-safe code paths that need a context
   void             unlockGlobalContext (void); // the global context is protected by the mtx_global mutex

   void             printAllFunctionCallStacks (void);

   // ---- debug console ----
   sBool            trace_enabled;
   sBool            dtrace_enabled;

   void lockMtxPrint (void);
   void unlockMtxPrint (void);

   TKS_PrintHook   *print_hooks;  // first (deletable) print hook or NULL
   sBool            b_iterating_print_hooks;  // temporarily true while iterating print hooks
   void             freePrintHooks (void);
   void             callPrintHooks (YAC_String *_s);

   void             print (const char *_s, sSI _sid); // _sid: 0=dcon, 1=STDOUT, 2=STDERR
   void             printf (const char *_fmt, ...);

   // ---- compile error handling ----
   // (note) this was (mostly) replaced by exception handling
   sU16             ptn_error_code; // compiletime/init error code (a relict from early versions of tkscript..)
   sUI              ptn_error_loc; // location of runtime error

   void             checkCompiletimeError  (void);
   void             handleCompiletimeError (sUI _srcloc, sUI _error);

   void             printResolveError      (sChar *);
   void             printResolveClassError (void /*TKClassDecl**/ *_class);

   void             handleFatalError (void);

   // ---- variables/constants ----
   TKS_ObjectCache global_instance_namespace; // global variables, e.g. 'Arguments' (*not* global vars declared in scripts!)
   TKS_ObjectCache global_constant_namespace; // global constants
   TKS_ObjectCache uconst_table;

   void clearInstanceNamespace (void);

   // ---- the magic "_" variable serves as an assignment target to effectively discard the result of an expression
   // ---- like in e.g. "_=[somearray][2].someMethod().some.complex.indirect.expr();"
   TKS_CachedObject nil_var;

   // ---- C/C++ API ----
   TKS_ObjectCache    yac_functions;
#ifndef TKS_NEWDELETE_INLINE
   TKS_ClassTemplate *atom_speedpath[YAC_MAX_CLASSES];
#endif // TKS_NEWDELETE_INLINE
   sUI                next_plugin_clid;
   void              *plugins; // TKSPluginRegistry *plugins;
   TKS_ObjectCache    global_template_namespace;

   PTN_Function      *tks_lib_init_fxn;  // used only in bg/lib mode (TKS_LIB build option)
   PTN_Function      *tks_lib_exit_fxn;  // used only in bg/lib mode
#ifdef TKS_VST
   PTN_Function      *tks_lib_vst_open_editor_fxn;
   PTN_Function      *tks_lib_vst_close_editor_fxn;
   PTN_Function      *tks_lib_vst_set_sample_rate_fxn;
   PTN_Function      *tks_lib_vst_set_bpm_fxn;
   PTN_Function      *tks_lib_vst_process_replacing_fxn;
   PTN_Function      *tks_lib_vst_get_bank_chunk_fxn;
   PTN_Function      *tks_lib_vst_get_program_chunk_fxn;
   PTN_Function      *tks_lib_vst_set_bank_chunk_fxn;
   PTN_Function      *tks_lib_vst_set_program_chunk_fxn;
#endif // TKS_VST

   sBool            registerClass (YAC_Object *_template, sUI _may_instantiate, sBool _isplugin); // will be deleted upon clean()
   void             freeYacFunctions (void);
   sBool            registerBuiltinClasses (void);
   void             clearTemplateNamespace (void);
   void             deleteTemplateNamespace (void);
   TKS_ClassTemplate *findTemplate (YAC_String *_templ);
   YAC_Object      *findTemplateObjectByUniqueClassName (YAC_String *);
   YAC_Object      *newObjectByName (TKS_Namespace *_nspace, YAC_String *_name);
   YAC_Object      *newObjectArrayByName (TKS_Namespace *_nspace, YAC_String *_name);
   YAC_Object      *deserializeNewObject (YAC_Object *, YAC_Object *_templateORnull);

   // ---- script exec ----
   void             runGlobalModuleStatements (PTN_Env*);
   void             compileRun (sBool _bRunInBG);
   sBool            loadLocalTSL (YAC_String *_libName);
   void             stop (void);

#ifdef TKS_VST
   void vstOpenEditor       (YAC_ContextHandle _context);       // call tks_lib_vst_open_editor script function
   void vstCloseEditor      (YAC_ContextHandle _context);       // call tks_lib_vst_close_editor script function
   void vstSetSampleRate    (YAC_ContextHandle _context, sF32 _rate); // call tks_lib_vst_set_sample_rate script function
   void vstSetBPM           (YAC_ContextHandle _context, sF32 _bpm);  // call tks_lib_vst_set_bpm script function
   void vstProcessReplacing (YAC_ContextHandle _context, YAC_Object *_inputs, YAC_Object *_outputs, sUI _numSampleFrames);
   void vstGetBankChunk     (YAC_ContextHandle _context, YAC_Object *_buffer);
   void vstGetProgramChunk  (YAC_ContextHandle _context, YAC_Object *_buffer);
   void vstSetBankChunk     (YAC_ContextHandle _context, YAC_Object *_buffer);
   void vstSetProgramChunk  (YAC_ContextHandle _context, YAC_Object *_buffer);
#endif // TKS_VST

   // ---- application start arguments ----
   sSI              cached_argc;
   sChar**          cached_argv;

   void             setArgs (sSI _argc, char **_argv);
   void             createArgArray (void);

   // ---- Script callbacks for OS signals
   void            *sig_signal;
   void            *sig_segv;
   void            *sig_segvcontinue;

   // ---- milli seconds counter ----
   struct __milliSeconds {
#ifdef YAC_WIN32
      sUI start;
#else
      void *start;
#endif
   } milliSeconds;

#ifdef DX_RDTSC
   static sF64 pfc_scale;
#endif // DX_RDTSC

   void sleep        (sUI _ms); // put current thread to sleep for _ms milliseconds
   void usleep       (sUI _us); // busy wait for _us microseconds
   void initProfiler (void);    // Query frequency and calibrate RDTSC if profiling is enabled

   void emitExplanations(void); // dump documentation to STDOUT

#ifdef DX_SCRIPTCONSTRAINTS
   // ---- script constraints ----
   TKS_ObjectCache script_constraints;
   void                  initScriptConstraints (void);
   void                  freeScriptConstraints (void);
   sBool                 addScriptConstraint   (TKS_ScriptConstraint *_scs);
   TKS_ScriptConstraint *findScriptConstraint  (YAC_String *_name);
#endif // DX_SCRIPTCONSTRAINTS

   // ---- exceptions ----
   TKS_ExceptionType *exception_types; // single linked list
   sUI                next_exception_id;
   sU8                exception_typecast_map[TKS_MAX_EXCEPTIONS][TKS_MAX_EXCEPTIONS];

   sUI                addExceptionType           (TKS_ExceptionType *); // insert into sll
   sUI                addExceptionType2          (TKS_ExceptionType *); // assign id, add to typecastmap
   TKS_ExceptionType *addUndeclaredExceptionType (/*TKS_ClassDecl *_suggestedClass, */TKS_ClassDecl *_class, sUI _srcLoc, YAC_String *_name);
   void               initExceptions             (void);
   void               freeExceptions             (void);
   TKS_ExceptionType *getExceptionTypeById       (sUI _id);
   TKS_ExceptionType *getExceptionTypeByName     (TKS_ClassDecl *_class, YAC_String *_name);
   TKS_ExceptionType *lazyGetExceptionTypeByName (/*TKS_ClassDecl *_suggestedClass, */TKS_ClassDecl *_class, sUI _srcLoc, YAC_String *_name);
   sBool              areAllExceptionsDeclared   (void);

   // ---- namespaces ----
   TKS_ObjectCache namespaces;
   YAC_String s_nspace_yac;
   void           initNamespaces     (void);
   void           freeNamespaceVars  (void);
   void           freeNamespaceTypes (void);
   void           freeNamespaces     (void);
   TKS_Namespace *addNamespace       (YAC_String *_name);
   TKS_Namespace *findNamespace      (YAC_String *_name);

   sBool findDeclaredClassType( // Search only _forcedNSpace or the given namespace array
      YAC_String                *_name,
      TKS_FreezedNamespaceArray  _nsa,
      TKS_Namespace             *_forcedNSpace,
      TKS_ClassTemplate        **_retTemplate,
      TKS_ClassDecl            **_retClassDecl
      );

#ifdef TKS_FIX_ABSTRACTVARS
   void fixAllAbstractVars(void);
#endif // TKS_FIX_ABSTRACTVARS


   // ---- mutexes ----
#ifdef TKS_MT
   TKS_Mutex *mtx_global;
   TKS_Mutex *mtx_print;
   TKS_NamedMutex *named_mutexes[TKS_MAX_NAMED_MUTEXES];
   void initMutexes(void);
   void freeMutexes(void);
   TKS_NamedMutex *findNamedMutex   (YAC_String *_name); // returns NULL if mutex was not found
   TKS_NamedMutex *createNamedMutex (YAC_String *_name); // "=<name>=" mutex declaration
   ////void destroyNamedMutex (TKS_Mutex *_mutex); // called by ~TKS_CachedScript xxx really needed?
#endif //TKS_MT

   // ---- profiling ----
#ifdef TKS_PROFILE_STATEMENTS
   TKS_StatementProfiler statement_profiler;
#endif

   YAC_String s_getenv;
   YAC_String s_true;
   YAC_String s_false;

   YAC_String tks_local_pathname; // folder for "local" chapter, ~/.tks/ resp. <winsystemroot>/temp/tks/. see tks_init_streamio()

   public:
   TKS_ScriptEngine(void);
   ~TKS_ScriptEngine();

   sBool init(void);
   sBool init2(void); // called after configuration has been parsed

   void  initMilliSeconds     (void);
   void  exitMilliSeconds     (void);
   sUI   getMilliSeconds      (void);
   sUI   getStartMilliSeconds (void);

   static sF64 GetMilliSecondsDouble (void);

#if 0
   void createTestObject (void);
#endif

   // pseudo token tables for builtin functions and procedures (sin, cos, ...; print, stdout,...)
   void exportPseudoTokenEntityTree (void);

   static sBool b_allow_char_allocator; // 1=use char allocator
   static TKS_LinearAllocator *char_allocator;
};

extern sUI tks_get_pseudotoken_id_by_name (YAC_String *_sv);


extern void TKS_SegVContinue (void);

// ---- stdio helpers
extern void tks_fputs2 (const char *_string); // write string to stderr
extern void tks_fputs  (const char *_string); // write string to stdout
extern void tks_fputc  (char _c);             // write char to stdout


extern YAC_String              *tks_empty_string;
extern YAC_Object              *yac_null;
extern yacmemptr                tksvm_zero_args;


#ifdef YAC_OBJECT_COUNTER
#ifdef TKS_MT
extern TKS_Mutex *mtx_yac_object_counter;
#endif // TKS_MT
#endif // YAC_OBJECT_COUNTER



#ifdef TKS_NEWDELETE_INLINE
#ifdef YAC_OBJECT_POOL

static inline YAC_Object *yacNewPooledByID__inline(sUI _classID, sUI _poolHint) {
   YAC_Object *o = ObjectPool::New(_classID, _poolHint);
   if(NULL == o)
   {
      return TKSCRIPT__ATOM_SPEEDPATH[_classID]->template_object->yacNewObject();
   }
   return o;
}

static inline void yacDelete__inline(YAC_Object *_o) {
   if(_o)
   {
      if(_o->pool_handle.pool_id)
      {
         ObjectPool::Delete(_o);
      }
      else
      {
         delete _o;
      }
   }
}

#else

static inline YAC_Object *yacNewPooledByID__inline(sUI _classId, sUI _poolHint) {
   return TKSCRIPT__ATOM_SPEEDPATH[_classID]->template_object->yacNewObject();
}

static inline void yacDelete__inline(YAC_Object *_o) {
   if(_o)
   {
      delete _o
   }
}

#endif // YAC_OBJECT_POOL

static inline YAC_Object *yacNewByID__inline(sUI _classID) {
   return TKSCRIPT__ATOM_SPEEDPATH[_classID]->template_object->yacNewObject();
}

#endif // TKS_NEWDELETE_INLINE


#endif // TKS_SCRIPTENGINE_H__
