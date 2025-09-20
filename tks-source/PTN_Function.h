/// PTN_Function.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef PTN_FUNCTION_H__
#define PTN_FUNCTION_H__


class TKS_ScriptConstraint;
class PTN_TryCatchFinally;

// Discard uncriticalerror if there is no try catch block to handle it
#define Dbeginfuncalltry const PTN_TryCatchFinally*__sav_last_try=_env->context->last_try; _env->context->last_try=NULL
#define Dendfuncalltry _env->context->last_try=__sav_last_try; _env->context->discardUncriticalError()

#ifdef TKS_MT
#define Dbeginfuncall(a) if((a)->mutex) (a)->mutex->lock(); Dbeginfuncalltry
#define Dendfuncall(a) Dendfuncalltry; if((a)->mutex) (a)->mutex->unlock()
#else
#define Dbeginfuncall(a) Dbeginfuncalltry
#define Dendfuncall(a) Dendfuncalltry
#endif // TKS_MT

class PTN_Function : public PTN_Statement { // , public YAC_Object
public:
   static sSI object_counter;

public:
   YAC_String                name;
   YAC_String                documentation;
   PTN_ArgVarList *          argvarlist;
   PTN_Statement *           body;
   TKS_ObjectCache          *fun_vars;         // static and local cachedobject*. NULL if function has no variables.
   YAC_Value *               opt_local_vars;   // references to fun_vars.elements, used to build stackframes
   sUI                       num_local_vars;   // initialized by prepareStackFrame()
   TKS_FormalArgument *      formal_args;      // *new* relaxed argument forward declaration
   TKS_ClassDecl *           parent_class;     // reference to class if this is a class function ("static method"). Also used by delegate methods (which run in the context of a class).
   TKS_Namespace *           nspace;           // reference to namespace or NULL (= global namespace) in which this function was declared
   sUI                       protection_flags; // TKS_PROT_xxx, for class functions
   sBool                     is_class_function; // 1=static method
   sU8                       return_type;           // YAC_TYPE_xxx (default is YAC_TYPE_VARIANT = 0)
   YAC_String                return_class_name;     // Name of unresolved return type class name
   TKS_FreezedNamespaceArray return_class_nspaces;  // Which namespaces to search when resolving the return class type
   YAC_Object           *    return_template_object;
#ifdef DX_SCRIPTCONSTRAINTS
   TKS_ScriptConstraint *    return_constraint;     // if!=null then apply this constraint to each returned value
   sUI                       return_constraint_type;
#endif

   // The yac_function_object is used by YAC_Host
   //   * yacEvalFunction
   //   * yacEvalFunctionReturn
   //   * yacFindFunction
   //   * yacFindFunctionInModule
   //   * yacFindVariableInFunction
   // to wrap a PTN_Function in a TKS_FunctionObject "YAC" Object.
   // This could have been using multiple inheritance but seems to cause
   // problems with certain C++ compilers (e.g. OpenWatcom1.4)
   TKS_FunctionObject *function_object;

#ifdef TKS_MT
   // if !=NULL this function/method will be synchronized using this mutex
   TKS_Mutex *mutex;
   sBool b_own_mutex; // 1= anonymous mutex (deleted in function d'tor), 0=shared (named) mutex
#endif

#ifdef TKS_PROFILE_FUNCTIONS
   sF64 profile_millisec;
   sUI  profile_callcount;
#endif // TKS_PROFILE_FUNCTIONS


public:
   PTN_Function(void);
   PTN_Function(YAC_String *_name);
   ~PTN_Function();

   void initFunction (void);

   sUI   getKey                   (void);
   sUI   getID                    (void) const;
   sBool semanticCheck            (void);
   void  print                    (sU16 _i=0);
   void  evalCall                 (PTN_Env *_env, YAC_Value *_r) const;
   void  evalVoidCall             (TKS_Context *_context) const;
   void  subGenCallList           (void);
   void  optimize                 (void);
   sBool resolveXRef              (void);
#ifdef TKS_JIT
   sU8   compile                  (VMCore *_vm);
   sBool forceHybrid              (void);
#endif


   // --- f u n c t i o n   v a r i a b l e s
   TKS_ObjectCache * getCreateFunVars (void);
   TKS_CachedObject *findFunVar       (YAC_String *_name);

   sBool setArg              (TKS_Context *_context, sU8 _index, YAC_Value *_v);  // xxx not for local vars, used by yacOperator* script methods
   void  setArgsByList       (PTN_Env *_env, PTN_ArgList *_arglist, YAC_Value *_newstackframe);
   void  setArgsByValues     (PTN_Env *_env, YAC_Value *_argValues, YAC_Value *_newstackframe);
   void  setLastArgConstraint(TKS_ScriptConstraint *_scs);
   void  setLastArgNotNullConstraint(void);

   void  addFormalArgument (YAC_String *_name, sSI _type, YAC_String *_unresolvedClassName, sBool _bPointerArray, sBool _array, sBool _local, TKS_FreezedNamespaceArray _nsa, TKS_Namespace *_forcedNSpace);
   sBool resolveFormalArgs (void); // called after class references have been resolved
   sBool compareFormalArgs (PTN_Function *);
   void  formalArgsToString (YAC_String *_d);

   void checkArgumentConstraint(PTN_Env *_env, YAC_Value *_cv, TKS_FormalArgument *_a);
   void handleConstraintViolation(YAC_Value *_cv,
                                  const sChar *_vname,
                                  TKS_ScriptConstraint *_constraint,
                                  TKS_FormalArgument *_a /* 0 == return constraint failed */
                                  ) const;

   void       prepareStackFrameTemplate (void); // called by optimize()
   void       freeStackFrameTemplate    (void); // called by ~PTN_Function

   void                emitExplanation   (void); // dump name/signature/documentation to stdout
   TKS_FunctionObject *getOrCreateFunctionObject (void);
   sBool               initMutex         (sBool _bAnonymousMutex, YAC_String *_mutexName);
};


#endif // PTN_FUNCTION_H__
