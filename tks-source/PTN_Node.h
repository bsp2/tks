/// PTN_Node.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef PTN_NODE_H__
#define PTN_NODE_H__


class PTN_Expr;
class PTN_Statement;
class TKS_CachedScript;
class TKS_ClassDecl;
class TKS_ClassDeclMethod;
class YAC_IntArray;
class YAC_FloatArray;
class YAC_PointerArray;
class YAC_HashTable;
class TKS_ScriptClassInstance;

#ifdef TKS_JIT
class VMCore;
#endif

//
// Note:
//   Usually, the statement/expression objects are used as "const"s.
//   However, there are some, not thread safe, code paths still left.
//
typedef void(*Fevalst)(PTN_Env*, const PTN_Statement *);
typedef void(*Feval)(PTN_Env *, YAC_Value*, const PTN_Expr *);

#define Dgetvar(__a) ((__a->flags&TKS_CachedObject::ISLOCAL)?&_env->context->tks_fstackc[__a->local_index]:__a)
#define Dgetvarfrom(__b, __a) ((__a->flags&TKS_CachedObject::ISLOCAL)?&__b->tks_fstackc[__a->local_index]:__a)

#define Dsrcloc_module(a) sU16(((a)>>16)&0xFFFF)
#define Dsrcloc_linenr(a) sU16(((a)&0xFFFF)/*+1u*/)

#define TKSERR_TXT_UNKNOWNMEMBER "[---] module,variable,member or static c++ object \"%s\" is unresolved.\n"


enum __ptnodeids {
  PTN_NODE=0,
  PTN_ARGS,
  PTN_EXPR,
  PTN_VAREXPR,
  PTN_PREINCVAREXPR,
  PTN_PREDECVAREXPR,
  PTN_POSTINVARCEXPR,
  PTN_POSTDECVAREXPR,
  PTN_PREINCVARSTAT,
  PTN_PREDECVARSTAT,
  PTN_POSTINCVARSTAT,
  PTN_POSTDECVARSTAT,
  PTN_CONSTVAL/*12*/,
  PTN_SINGLEARGEXPR,
  PTN_DOUBLEARGEXPR,
  PTN_CALL,
  PTN_STATEMENT,
  PTN_FOR,
  PTN_WHILE,
  PTN_IFELSE,
  PTN_DOWHILE,
  PTN_BUILTINFUN,
  PTN_SWITCH,
  PTN_SWITCHCASE,
  PTN_SWITCHDEFAULTCASE,
  PTN_LOOP,
  PTN_FUNCTION,
  PTN_FUNCTIONBODY,
  PTN_FUNCTIONCALL,
  PTN_FUNCTIONECALL,
  PTN_RETURN,
  PTN_FOREACH,
  PTN_DEFINE,
  PTN_MODULEMEMBEREXPR,
  PTN_CLASSDECL,
  PTN_CLASSMETHODBODY,
  PTN_TRACESTAT,
  PTN_PREPARE,
  PTN_EXTFUN,
  PTN_EMPTY,
  PTN_TRYCATCHFINALLY,

  PTN_NUMNODES
};


enum __ptnodesemanticerrors {
  PTNERR_NOERROR=0,
  PTNERR_ANYERR,
  PTNERR_UFUNMISSINGBODY,
  PTNERR_CLASSSTACKOVERFLOW,
  PTNERR_MODULENOTFOUND,
  PTNERR_MODULEMEMBERNOTFOUND,
  PTNERR_MODULEMETHODNOTFOUND,
  PTNERR_CANTCOMPILE,
  PTNERR_ERRCOMPILE,
  PTNERR_FSTACKOVERFLOW,
  PTNERR_BREAKSTACKOVERFLOW,
  PTNERR_SWITCHSTACKOVERFLOW,
  PTNERR_CONSTRAINTFAILED,

  PTNERR_PAD
};



class PTN_Node {
public:
   static sSI object_counter;
public:
   static YAC_Value  zero_val;
   sUI               src_loc; // (module_index<<16) | linenr

public:

#ifdef TKS_USE_NODEALLOCATOR
   ////void *operator new    (size_t _size) { (void)_size; /* should never be called */ return NULL; }
   void  operator delete (void*_ptr)    { (void)_ptr; /* do nothing */ }
   void *operator new    (size_t, void *_this) { return _this; }
#endif // TKS_USE_NODEALLOCATOR

public:
   static const char *GetErrorByCode(sU16);

   PTN_Node(void);
   virtual ~PTN_Node();

   virtual sUI   getID         (void) const; // used to check whether statements needs ";" terminator
   virtual sBool semanticCheck (void); // check whether node is ready to be evaluated
   virtual sBool isConst       (void);
   virtual void  optimize      (void);
   virtual sBool resolveXRef   (void);
#ifdef TKS_JIT
   virtual sU8   compile       (VMCore *); // return expression type, 0xFF=unsupported, 0=statement
   virtual sBool forceHybrid   (void);
#endif

   sBool needSemicolon(void) const;

   virtual void initIndirect(void) { }
};




#ifdef YAC_GCC
// unfortunately cygwin GCC 3.4.4 has a bug which leads to error messages like e.g.
/////PTN_ArrayIniExpr.cpp:198: error: `_env->PTN_Env::context->TKS_Context::raiseRuntimeException(((PTN_Node*)this), 5u, ((co
/////nst char*)"cannot cast <void> to int/float/Object array element"), ((const char*)"PTN_ArrayIniExpr.cpp"), 198)' has type
///// `void' and is not a throw-expression
// if using "0" in the else branch of the ternary expression statement
// here's a workaround:
// XXX TODO: safe to delete this code ?
class TKS_Nop {
   public: static inline void nop(void) {}
};
#define TKS_NOP TKS_Nop::nop()
#else
#define TKS_NOP 0
#endif // YAC_GCC

//
// Utility macros to throw runtime exceptions
//
#define Drtthrow(n,t,m) if(!_env->context);else _env->context->raiseRuntimeException(n,t,m, __FILE__, __LINE__)
#define Drtthrowinvalidpointer(n,m,o) if(!_env->context);else _env->context->raiseInvalidPointer(n, m, __FILE__, __LINE__, o)

// Compile-time error macros.
//  (TODO) these should be obsolete but are still used in some places, even for some *runtime* errors
//         like stack over-/under flows (PTNERR_FSTACKOVERFLOW, PTNERR_CONSTRAINTFAILED)
//         where actually an exception should be used (Drtthrow())
#define Dcterror(i) tkscript->handleCompiletimeError(src_loc, i)


#ifdef TKS_JIT
#define Dasmop(a) _vm->addOpcode((a))
#define Dasmopv(a,b) _vm->addOpcode((a)+(b))

extern void YAC_CALL tksvm_unsetvo       (sUI _del, sUI _type, YAC_Object *_o);
extern void YAC_CALL tksvm_opassign      (YAC_Object *_this, YAC_Object *_o);
extern sUI  YAC_CALL tksvm_operator      (sS32 _cmd, YAC_Object *_right, YAC_Object *_left);
extern sSI  YAC_CALL tksvm_operatori     (sS32 _cmd, sS32 _right, YAC_Object *_left);
extern sUI  YAC_CALL tksvm_operatorf     (sS32 _cmd, sF32 _right, YAC_Object *_left);
extern void YAC_CALL tksvm_pointerassign (TKS_CachedObject *co, YAC_Object *_o);
extern void YAC_CALL tksvm_pointerassign_local (TKS_Context *_context, TKS_CachedObject *co, YAC_Object *_o);
extern void YAC_CALL tksvm_delete        (TKS_CachedObject *co);
extern void YAC_CALL tksvm_delete_local  (TKS_Context *_context, TKS_CachedObject *co);
extern void YAC_CALL tksvm_hybridstat    (PTN_Env *_env, PTN_Statement *_st);
extern sS32 YAC_CALL tksvm_irnd          (sS32 _val);
extern sF32 YAC_CALL tksvm_frnd          (sF32 _val);
extern void        YAC_CALL tksvm_classmemberassign_if       (TKS_CachedObject *_co, sUI id, sUI _if);
extern void        YAC_CALL tksvm_classmemberassign_stack_if (TKS_Context *_context, sUI id, sUI _if);
extern sUI         YAC_CALL tksvm_classmemberexpr_if         (TKS_CachedObject *_co, sUI id);
extern sUI         YAC_CALL tksvm_classmemberexpr_stack_if   (TKS_Context *_context, sUI id);
extern YAC_Object *YAC_CALL tksvm_classmemberexpr_o          (TKS_CachedObject *_co, sUI id);
extern YAC_Object *YAC_CALL tksvm_classmemberexpr_stack_o    (TKS_Context *_context, sUI id);
extern YAC_Object *YAC_CALL tksvm_objectarrayindexexpr    (TKS_CachedObject *_co, sUI _idx);
extern YAC_Object *YAC_CALL tksvm_pointerarrayindexexpr   (TKS_CachedObject *_co, sUI _idx);
extern YAC_String *YAC_CALL tksvm_stringarrayindexexpr    (TKS_CachedObject *_co, sUI _idx);
extern YAC_Object *YAC_CALL tksvm_classarrayindexexpr     (TKS_CachedObject *_co, sUI _idx);
extern void        YAC_CALL tksvm_objectarrayindexassign  (TKS_CachedObject *_co, sUI idx, YAC_Object *o);
extern void        YAC_CALL tksvm_pointerarrayindexassign (TKS_CachedObject *_co, sUI idx, YAC_Object *o);
extern void        YAC_CALL tksvm_stringarrayindexassign  (TKS_CachedObject *_co, sUI idx, YAC_String *s);
extern void        YAC_CALL tksvm_classarrayindexassign   (TKS_CachedObject *_co, sUI idx, TKS_ScriptClassInstance *cc);

#endif // TKS_JIT

extern const char*typenames[4];


#endif // PTN_NODE_H__
