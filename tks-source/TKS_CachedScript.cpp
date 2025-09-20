/// TKS_CachedScript.cpp
///
/// author : (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///    published under terms of the GNU general public license (GPL).
///
/// created: 07Jun2001
/// changed: 27Feb2005, ..., 25Jul2005, ..., 21Dec2005, 18-Jan-2006, 20-Mai-2006
///          20Jun2006, ..., 30Oct2007, 17Nov2007, ..., 05Apr2008, 14-Apr-2008, ...., 12-May-2008
///          19Sep2009, ..., 19Apr2010, 12Mar2012, ..., 23Oct2015, ..., 22Jun2020, 18Jun2023, 22Aug2024
///

#define ERRTKS_CLASSCOMMANDNOTFOUND "[---] class \"%s\" has no method called \"%s\".\n"

#include <stdlib.h>
#include <string.h>

#include "tks.h"

#include "TKS_ScriptConstraint.h"

#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "PTN_ArgVar.h"
#include "PTN_ArgVarList.h"
#include "PTN_ArgListNode.h"
#include "PTN_ArgList.h"
#include "TKS_FormalArgument.h"
#include "TKS_ScriptObjectID.h"
#include "TKS_FunctionObject.h"
#include "PTN_Function.h"

#include "tks_inc_class.h"
#include "tks_inc_compiler.h"

#include "TKS_TokenTable.h"

#include "tks_inc_jit.h"

#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "YAC_ListNode.h"

#include "YAC_ClassArray.h"
#include "YAC_ValueArray.h"
#include "YAC_IntArray.h"
#include "YAC_FloatArray.h"
#include "YAC_HashTable.h"
#include "YAC_PointerArray.h"
#include "YAC_ObjectArray.h"
#include "YAC_StringArray.h"
#include "YAC_ValueArray.h"

#include "PTN_ARGBExpr.h"
#include "PTN_ArrayIniExpr.h"
#include "PTN_BreakableStatement.h"
#include "PTN_Break.h"
#include "PTN_BuiltinFun.h"
#include "PTN_CallableExpr.h"
#include "PTN_Call.h"
#include "PTN_ClampWrapStat.h"
#include "PTN_ClassDecl.h"
#include "PTN_ClassMemberExpr.h"
#include "PTN_ClassMemberIncDecExpr.h"
#include "PTN_ClassMemberIncDecStat.h"
#include "PTN_ClassMemberIndexedVarExpr.h"
#include "PTN_ClassMemberIndirectClassExpr.h"
#include "PTN_ClassMemberIndirectClassMethodECall.h"
#include "PTN_ClassMemberIndirectClassStat.h"
#include "PTN_ClassMemberIndirectObjectECall.h"
#include "PTN_ClassMemberIndirectObjectExpr.h"
#include "PTN_ClassMemberIndirectObjectStat.h"
#include "PTN_ClassMemberOpAssign.h"
#include "PTN_ClassMemberOpIndexedVarAssign.h"
#include "PTN_ClassMemberPointerAssign.h"
#include "PTN_ClassMethodBody.h"
#include "PTN_ClassMethodECall.h"
#include "PTN_ClassMethodDelegateCall.h"
#include "PTN_ClassMethodDelegateAssign.h"
#include "PTN_ClassVarDecl.h"
#include "PTN_ConditionalExpr.h"
#include "PTN_ConstraintVarExpr.h"
#include "PTN_ConstVal.h"
#include "PTN_ConstValReference.h"
#include "PTN_Define.h"
#include "PTN_DerefExpr.h"
#include "PTN_DoubleArgExpr.h"
#include "PTN_DoWhile.h"
#include "PTN_VarAssign.h"
#include "PTN_IndexedVarAssign.h"
#include "PTN_DynamicIndexedVarAssign.h"
#include "PTN_DynamicVarAssign.h"
#include "PTN_PointerAssign.h"
#include "PTN_DynamicVarPointerAssign.h"
#include "PTN_DynTraceStat.h"
#include "PTN_ECallY.h"
#include "PTN_EmptyStatement.h"
#include "PTN_Env.h"
#include "PTN_ExceptionVarExpr.h"
#include "PTN_ExprArrayEntry.h"
#include "PTN_Expr.h"
#include "PTN_ExprHashEntry.h"
#include "PTN_ExprStat.h"
#include "PTN_FlushStat.h"
#include "PTN_ForEach.h"
#include "PTN_For.h"
#include "PTN_FunctionBody.h"
#include "PTN_FunctionCall.h"
#include "PTN_FunctionECall.h"
#include "PTN_FunctionECallY.h"
#include "PTN_FunctionSignalStatement.h"
#include "PTN_HashIniExpr.h"
#include "PTN_IArrayExpr.h"
#include "PTN_IfElse.h"
#include "PTN_IMemberExpr.h"
#include "PTN_IMethodECall.h"
#include "PTN_IndexedVarExpr.h"
#include "PTN_InstanceOfExpr.h"
#include "PTN_LoopStatement.h"
#include "PTN_ModuleDef.h"
#include "PTN_NewClassExpr.h"
#include "PTN_NewExpr.h"
#include "PTN_ObjClampWrapStat.h"
#include "PTN_ObjectMemberIncDecExprY.h"
#include "PTN_ObjectMemberIncDecStatY.h"
#include "PTN_Node.h"
#include "PTN_OpVarAssign.h"
#include "PTN_OpIndexedVarAssign.h"
#include "PTN_OpDynamicVarAssign.h"
#include "PTN_OpObjectMemberAssignY.h"
#include "PTN_VarExpr.h"
#include "PTN_VarStatement.h"
#include "PTN_PostDecVarExpr.h"
#include "PTN_PostDecVarStat.h"
#include "PTN_PostIncVarExpr.h"
#include "PTN_PostIncVarStat.h"
#include "PTN_PreDecVarExpr.h"
#include "PTN_PreDecVarStat.h"
#include "PTN_PreIncVarExpr.h"
#include "PTN_PreIncVarStat.h"
#include "PTN_PrepareStat.h"
#include "PTN_PrintStat.h"
#include "PTN_Return.h"
#include "PTN_RGBExpr.h"
#include "PTN_SingleArgExpr.h"
#include "PTN_Statement.h"
#include "PTN_SwitchCase.h"
#include "PTN_SwitchDefaultCase.h"
#include "PTN_Switch.h"
#include "PTN_ThisExpr.h"
#include "PTN_TraceStat.h"
#include "PTN_TripleArgExpr.h"
#include "PTN_UnresolvedModuleMemberExpr.h"
#include "PTN_UnresolvedModuleMemberIncDecExpr.h"
#include "PTN_UnresolvedModuleMemberPointerAssign.h"
#include "PTN_UnresolvedModuleMemberPostIncDecStat.h"
#include "PTN_UnresolvedModuleMethodECall.h"
#include "PTN_UnresolvedOpModuleMemberAssign.h"
#include "PTN_ValueListEntry.h"
#include "PTN_ValueListExpr.h"
#include "PTN_VarDecl.h"
#include "PTN_While.h"
#include "PTN_Throw.h"
#include "PTN_TryCatchFinally.h"




#ifdef YAC_WIN32
#include <windows.h>
#endif
#include "TKS_Plugin.h"
#include "TKS_PluginRegistry.h"
#include "TKS_CachedScript.h"
#include "TKS_CachedScript_macros.h"


TKS_StateStackEntry TKS_CachedScript::state_stack[TKS_MAX_PARSER_STACKDEPTH];

PTN_BreakableStatement *TKS_CachedScript::break_stack[TKS_BREAK_STACK_SIZE+1];


// some pseudo keywords
static YAC_String tok_var        ("var");
static YAC_String tok_notnull    ("notnull");
static YAC_String tok_Value      ("Value");
static YAC_String tok_callbacks  ("callbacks");
static YAC_String tok_underscore ("_");
static YAC_String tok_exception  ("exception");
static YAC_String tok_defaultExceptionVarName ("!");

const char *tks_get_rtti_name_by_id(sU8 _id) {
   switch(_id)
   {
      case YAC_TYPE_VOID:     return "void";   // 0
      case YAC_TYPE_INT:      return "int";    // 1
      case YAC_TYPE_FLOAT:    return "float";  // 2
      case YAC_TYPE_OBJECT:   return "Object"; // 3
      case YAC_TYPE_STRING:   return "String"; // 4; virtual type, used for optimizations
      default:    return "**illRTTI**";
   }
}

//
// Parser states
//
enum __tkscsqstates {
   ST_OFMIND=0,
   ST_VARDECL,
   ST_VARASSIGN,
   ST_BREAK,
   ST_CALL,
   ST_IF,
   ST_SEQ,
   ST_SSEQSTAT,
   ST_SSEQ,
   ST_LOOPSEQ,
   ST_EXPR,
   ST_BITEXPR,
   ST_RELEXPR,
   ST_SEXPR,
   ST_ADDEXPR,
   ST_TERM,
   ST_FACTOR,
   ST_CMP,
   ST_ADD,
   ST_MUL,
   ST_ECALL,
   ST_BUILTINFUN,
   ST_ARGLIST,
   ST_FOR,
   ST_PREINCVARSTAT,
   ST_PREDECVARSTAT,
   ST_POSTINCVARSTAT,
   ST_POSTDECVARSTAT,
   ST_PREINCVAREXPR,
   ST_PREDECVAREXPR,
   ST_POSTINCVAREXPR,
   ST_POSTDECVAREXPR,
   ST_WHILE,
   ST_DOWHILE,
   ST_SWITCH,
   ST_SWITCHCASE,
   ST_LOOP,
   ST_UFUNDECL,
   ST_UFUNBODY,
   ST_UFUNCALL,
   ST_UFUNECALL,
   ST_UFUNRETURN,
   ST_UFUNEVENT,
   ST_NAMESPACE,
   ST_DYNARRAYDECL,
   ST_NEWEXPR,
   ST_FOREACH,
   ST_PTRASSIGN,
   ST_ENUM,
   ST_DEFINE,
   ST_OBJVARASSIGN,
   ST_READPROPERTY,
   ST_PREINCDECREADPROPERTY,
   ST_MODULE,
   ST_CLASSDECL,
   ST_CLASSMETHODBODY,
   ST_PRINT,
   ST_TRACE,
   ST_DTRACE,
   ST_CLASSSUPERECALL,
   ST_IEXPR,
   ST_IARRAYEXPR,
   ST_EXPRSTAT,
   ST_ARRAYINIEXPR,
   ST_HASHINIEXPR,
   ST_VALUELISTEXPR,
   ST_DEREFEXPR,
   ST_PREPARESTAT,
   ST_CLAMPWRAPSTAT,
   ST_RGB,
   ST_COMPILE,
   ST_FLUSH,
   ST_CONSTRAINT,
   ST_DELEGATE,
   ST_DELEGATECALL,
   ST_EXPLAIN,
   ST_THROWSTAT,
   ST_TRYSTAT,
   ST_MUTEXDECL,

   ST_NUMSTATES
};

#if 0
// occasionally used for debugging
static const char *debug__get_seq_name_by_id(sU16 _id) {
   switch(_id)
	{
      case ST_VARDECL:                return "VARDECL";
      case ST_VARASSIGN:              return "VARASSIGN";
      case ST_BREAK:                  return "BREAK";
      case ST_CALL:                   return "CALL";
      case ST_IF:                     return "IF";
      case ST_WHILE:                  return "WHILE";
      case ST_DOWHILE:                return "DOWHILE";
      case ST_SWITCH:                 return "SWITCH";
      case ST_SWITCHCASE:             return "SWITCHCASE";
      case ST_SEQ:                    return "SEQ";
      case ST_SSEQSTAT:               return "SSEQSTAT";
      case ST_SSEQ:                   return "SSEQ";
      case ST_LOOPSEQ:                return "LOOPSEQ";
      case ST_EXPR:                   return "EXPR";
      case ST_IEXPR:                  return "IEXPR";
      case ST_IARRAYEXPR:             return "IARRAYEXPR";
      case ST_SEXPR:                  return "SEXPR";
      case ST_TERM:                   return "TERM";
      case ST_FACTOR:                 return "FACTOR";
      case ST_CMP:                    return "CMP";
      case ST_ADD:                    return "ADD";
      case ST_MUL:                    return "MUL";
      case ST_ECALL:                  return "ECALL";
      case ST_ARGLIST:                return "ARGLIST";
      case ST_FOR:                    return "FOR";
      case ST_PREINCVARSTAT:          return "PREINCVARSTAT";
      case ST_PREDECVARSTAT:          return "PREDECVARSTAT";
      case ST_POSTINCVARSTAT:	        return "POSTINCVARSTAT";
      case ST_POSTDECVARSTAT:	        return "POSTDECVARSTAT";
      case ST_PREINCVAREXPR:	        return "PREINCVAREXPR";
      case ST_PREDECVAREXPR:	        return "PREDECVAREXPR";
      case ST_POSTINCVAREXPR:	        return "POSTINCVAREXPR";
      case ST_POSTDECVAREXPR:	        return "POSTDECVAREXPR";
      case ST_LOOP:                   return "LOOP";
      case ST_UFUNDECL:               return "UFUNDECL";
      case ST_UFUNBODY:               return "UFUNBODY";
      case ST_UFUNCALL:               return "UFUNCALL";
      case ST_UFUNECALL:              return "UFUNECALL";
      case ST_UFUNRETURN:             return "UFUNRETURN";
      case ST_UFUNEVENT:              return "UFUNEVENT";
      case ST_DYNARRAYDECL:	        return "DYNARRAYDECL";
      case ST_NEWEXPR:                return "NEWEXPR";
      case ST_FOREACH:                return "FOREACH";
      case ST_PTRASSIGN:              return "PTRASSIGN";
      case ST_ENUM:                   return "ENUM";
      case ST_DEFINE:                 return "DEFINE";
      case ST_OBJVARASSIGN: 	        return "OBJVARASSIGN";
      case ST_READPROPERTY:           return "READPROPERTY";
      case ST_PREINCDECREADPROPERTY:  return "PREINCDECREADPROPERTY";
      case ST_MODULE:                 return "MODULE";
      case ST_CLASSDECL:              return "CLASSDECL";
      case ST_CLASSMETHODBODY:        return "CLASSMETHODBODY";
      case ST_PRINT:                  return "PRINT";
      case ST_TRACE:                  return "TRACE";
      case ST_DTRACE:                 return "DTRACE";
      case ST_CLASSSUPERECALL:        return "CLASSSUPERECALL";
      case ST_EXPRSTAT:               return "EXPRSTAT";
      case ST_ARRAYINIEXPR:           return "ARRAYINIEXPR";
      case ST_HASHINIEXPR:            return "HASHINIEXPR";
      case ST_VALUELISTEXPR:          return "VALUELISTEXPR";
      case ST_DEREFEXPR:              return "DEREFEXPR";
      case ST_PREPARESTAT:            return "PREPARESTAT";
      case ST_CLAMPWRAPSTAT:          return "CLAMPWRAPSTAT";
      case ST_RGB:                    return "RGB";
      case ST_COMPILE:                return "COMPILE";
      case ST_FLUSH:                  return "FLUSH";
      case ST_DELEGATE:               return "DELEGATE";
      case ST_DELEGATECALL:           return "DELEGATECALL";
      case ST_EXPLAIN:                return "EXPLAIN";
      case ST_THROWSTAT:              return "THROWSTAT";
      case ST_TRYSTAT:                return "TRYSTAT";
      case ST_MUTEXDECL:              return "MUTEXDECL";
      default:                        return "INVALID_STATE";
	}
}
#endif // 0


TKS_CachedScript::TKS_CachedScript(void) {
   is_tmod = YAC_FALSE;
   tmod_class_nsp = NULL;
   linenr_offset = 0u;
   init();
}

TKS_CachedScript::~TKS_CachedScript() {
   clean();
   freeActiveNamespaces();
}

void TKS_CachedScript::init(void) {
   b_own_node_allocator            = YAC_FALSE;
   b_done                          = YAC_FALSE;
   b_global_module_statements_done = YAC_FALSE;

   node_allocator        = NULL;
   compiled_script       = NULL;
   parser_tree           = NULL;
   parser_tree_num_nodes = 0u;
   cfunction             = NULL;

   initActiveNamespaces();
   selectNamespace(NULL);
}

void TKS_CachedScript::clean(void) {
   sBool bDebug = (tkscript->configuration.debug_level > 79);
   if(bDebug) ::printf("~TKS_CachedScript::clean: free module vars\n");
   module_vars.freeCache(bDebug);
   if(bDebug) ::printf("~TKS_CachedScript::clean: free functions\n");
   functions.freeCache(bDebug);
   if(bDebug) ::printf("~TKS_CachedScript::clean: delete parser tree\n");
   if(parser_tree)
   {
      delete parser_tree;
      parser_tree = NULL;
   }
   if(bDebug) ::printf("~TKS_CachedScript::clean: delete compiled script\n");
   if(compiled_script)
   {
      delete compiled_script;
      compiled_script = NULL;
   }
#ifdef TKS_USE_NODEALLOCATOR
   if(bDebug) ::printf("~TKS_CachedScript::clean: free node allocator\n");
   freeNodeAllocator();
#endif // TKS_USE_NODEALLOCATOR
   if(bDebug) ::printf("~TKS_CachedScript::clean: LEAVE\n");
}

void TKS_CachedScript::initActiveNamespaces(void) {
   active_namespaces = new TKS_ActiveNamespace();
   active_namespaces->nspace = &Dtkscompiler->default_namespace;
   active_namespaces->next   = NULL;
}

void TKS_CachedScript::freeActiveNamespaces(void) {
   //
   // Delete entire "active_namespaces" list
   //
   TKS_ActiveNamespace *a = active_namespaces;
   TKS_ActiveNamespace *n;
   while(a)
   {
      n = a->next;
      delete a;
      a = n;
   }
   active_namespaces = NULL;
}

void TKS_CachedScript::setTempScriptClassNamespace(TKS_Namespace *_nsp) {
   tmod_class_nsp = _nsp;
}

void TKS_CachedScript::selectNamespace(TKS_Namespace *_nspace) {
   if(NULL == _nspace)
   {
      // // if(NULL != tmod_class_nsp)
      // //    _nspace = tmod_class_nsp;
      // // else
         _nspace = &Dtkscompiler->default_namespace;
   }
   create_namespace = _nspace;
}

void TKS_CachedScript::useNamespace(TKS_Namespace *_nspace) {
   //
   // Add the given namespace to the top of the active_namespaces list
   //
   if(_nspace == &Dtkscompiler->default_namespace)
   {
      // Never reached (hopefully)...paranoia...
      useDefaultNamespace();
      return;
   }

   // Remove namespace in case it already exists
   TKS_ActiveNamespace *a = active_namespaces;
   TKS_ActiveNamespace *p = NULL;
   while(a->next)
   {
      if(a->nspace == _nspace)
      {
         // Unlink node
         if(NULL == p)
         {
            // List head
            active_namespaces = a->next;
         }
         else
         {
            p->next = a->next;
         }
         delete a;
         break;
      }
      else
      {
         p = a;
         a = a->next;
      }
   }

   // Add namespace
   a = new TKS_ActiveNamespace();
   a->nspace = _nspace;
   a->next   = active_namespaces;
   active_namespaces = a;
}

void TKS_CachedScript::useDefaultNamespace(void) {
   //
   // Remove all namespace entries except for the default_namespace
   //
   for(;;)
   {
      TKS_ActiveNamespace *n = active_namespaces->next;
      if(NULL != n) // do not remove last list entry (default_namespace)
      {
         delete active_namespaces;
         active_namespaces = n;
      }
      else
      {
         // Done
         return;
      }
   }
}

void TKS_CachedScript::addConstant(YAC_String*_sc, YAC_String*_sv) {
   TKS_CachedObject*co=tkscript->uconst_table.createStringInstance(_sc, 1);
   co->value.string_val->yacCopy(_sv);
}

TKS_CachedObject *TKS_CachedScript::findConst(YAC_String *_sv) const {
   TKS_CachedObject *co = tkscript->global_constant_namespace.findEntry(_sv);
   if(!co)
   {
         // ---- try constant replacement table ----
      co=tkscript->uconst_table.findEntry(_sv);
      if(co)
      {
         _sv=co->value.string_val;
         TKS_CachedObject *nco;
         do
         {
            nco=tkscript->uconst_table.findEntry(_sv);
            if(!nco)
            {
               nco=tkscript->global_constant_namespace.findEntry(_sv);
               if(nco)
               {
                     // registered class constant
                  return nco;
               }
            }
            else
            {
               co=nco;
               _sv=co->value.string_val;
            }
         } while(nco);
            //isconst=!sv->getQuotFlag();
      }
         //else isconst=1;
   }
   if(!co)
   {
         // ---- try class constant
      if(cclassdecl)
         co=cclassdecl->findConstantSuper(_sv);
   }

      //else
      //{
      // VAR is a registered Class constant
      //setPNode(new PTN_ConstVal());
      //	return co; //popnode.constval->const_value.initValue(co);
      //}
   return co;
}

sSI TKS_CachedScript::getIntConst(YAC_String *_sv) const {
   _sv->resubstEscapeSeq();
   switch(_sv->checkConversions())
   {
      case 0:
         break;
      case 1: // int
      {
         sSI intval;
         _sv->yacScanI(&intval);
         return intval;
      }
      case 2: // float
      {
         sF32 floatval;
         _sv->yacScanF32(&floatval);
         return (sSI)floatval;
      }
      default: // String, treat as constant name and resolve
      {
         TKS_CachedObject *co = findConst(_sv);
         if(co)
         {
            return co->getIntValue();
         }
      }
      break;
   }
   return 0;
}

sBool TKS_CachedScript::scanSource(YAC_String *_newsourcescript, sBool _bRefCopy) {
   Dtkscompiler->current_module = this;
   if(_newsourcescript)
   {
      //clean();
      module_vars.allocCache(TKS_MAX_MODULE_VARIABLES+1);
      functions.allocCache(TKS_MAX_FUNCTIONS+1);

      compiled_script = new TKS_TokenizedScript();

      if(_newsourcescript->chars[0] < 5 ) // Starts with version number?
      {
         // Load pre-tokenized data
         yacmemptr mp; mp.u8 = _newsourcescript->chars;
         Dpcscan_startSample(readTokens);
         if(!compiled_script->readTokens(mp, _newsourcescript->getBufLen()))
         {
            ////clean();
            return YAC_FALSE;
         }
         Dpcscan_stopSample(readTokens);
      }
      else
      {
         // Parse real string
         if(_newsourcescript->startsWith("#!"))
         {
            // ---- handle she.bang line #!/usr/bin/tks
            sUI i = 0u;
            sU8 c;
            while(i < _newsourcescript->length)
            {
               c = _newsourcescript->chars[i];
               if(c)
               {
                  if(10 == c)
                     break;
                  else
                     _newsourcescript->chars[i++] = 32;
               }
               else
                  break;
            }
         }

         Dpcscan_startSample(scan);
         sUI num_tokens = compiled_script->scan(_newsourcescript, _bRefCopy, 0 /*, default params*/);
         Dpcscan_stopSample(scan);

         if(num_tokens >= ERRTKS)
         {
            //tkscript->printf("[---] TKS_CachedScript::scanSource: compiled_script->scan() failed (num_tokens=%u)", num_tokens);
            /////clean();
            return YAC_FALSE;
         }
         else
         {
            Dpcscan_startSample(optimize);
            compiled_script->optimize();
            Dpcscan_stopSample(optimize);
         }
      }

      Dpcscan_startSample(scanClasses);

      sBool bScanOk = scanClasses();

      selectNamespace(NULL);  // [05Sep2025] fix for namespace-at-end-of-module issue
      useDefaultNamespace();

      if(bScanOk)
      {
         Dpcscan_stopSample(scanClasses);
         return YAC_TRUE;
      }
      else
      {
         // tkscript->printf("[---] TKS_CachedScript::scanSource: scanClasses() failed, deleting compiled_script.\n");
         delete compiled_script;
         compiled_script = NULL;
      }
   }
   return YAC_FALSE;
}

sBool TKS_CachedScript::compileSource(void) {
   Dtkscompiler->current_module = this;

   useDefaultNamespace();

   sBool bCompileOk = compile();

   selectNamespace(NULL);  // [05Sep2025] fix for namespace-at-end-of-module issue
   useDefaultNamespace();

   if(bCompileOk)
   {
      Dtkscompiler->parse_error = 0;

      if(!tkscript->configuration.b_keep_source_after_compile)
      {
         // Dprintf("[trc] TKS_CachedScript::compileSource: compile() OK, deleting compiled_script=%p.\n", compiled_script);
         delete compiled_script;
         compiled_script = NULL;
      }

#ifdef TKS_FIX_ABSTRACTVARS
      module_vars.fixAbstractVars();
#endif // TKS_FIX_ABSTRACTVARS

      return YAC_TRUE;
   }
   else
   {
      // Dprintf("[---] TKS_CachedScript::compileSource: compile() failed, deleting compiled_script=%p.\n", compiled_script);
      delete compiled_script;
      compiled_script = NULL;
   }
   return YAC_FALSE;
}

PTN_Function *TKS_CachedScript::findFunction(YAC_String *_name) const {

   // try current module
   TKS_CachedObject *co = functions.findEntry(_name);
   if(NULL != co)
   {
      return (PTN_Function*) co->value.any;
   }

   // try active namespaces
   if(NULL == co)
   {
      TKS_ActiveNamespace *a = active_namespaces;
      while(a)
      {
         PTN_Function *f = a->nspace->findFunction(_name);
         if(NULL != f)
         {
            return f;
         }

         // Search next namespace
         a = a->next;
      }
   }

   return NULL;
}

PTN_Function *TKS_CachedScript::findFunction(const char *_name) const {
   YAC_String name;
   name.visit(_name);
   return findFunction(&name);
}

void TKS_CachedScript::pushState(void) {
   stp--;
   state_stack[stp].state=state;
   state_stack[stp].state_index=state_index;
   state_stack[stp].initParams();
}

void TKS_CachedScript::popState(void) {
   state=state_stack[stp].state;
   state_index=++state_stack[stp].state_index;
   stp++;
}


TKS_CachedObject *TKS_CachedScript::findVar(YAC_String *         _sv,
                                            TKS_ActiveNamespace *_nspaceList,
                                            TKS_Namespace *      _forcedNSpace
                                            ) const
{
   //
   // Find local function var, static class member or global variable
   //
   TKS_CachedObject *co;
   if(NULL == _forcedNSpace)
   {
      // Try to find local function variable or static class member
      co = findVarLocal(_sv);
   }
   else
   {
      // Only search selected namespace
      if(_forcedNSpace == &Dtkscompiler->default_namespace)
      {
         return findModuleVar(_sv);
      }
      else
      {
         return _forcedNSpace->findVar(_sv);
      }
   }
   if(!co)
   {
      return findVarGlobal(_sv, _nspaceList);
   }
   else
   {
      return co;
   }
}

TKS_CachedObject *TKS_CachedScript::findVarGlobal(YAC_String *         _sv,
                                                  TKS_ActiveNamespace *_nspaceList
                                                  ) const
{
   //
   // Find global variable (global var, module var or namespace var)
   //
   TKS_CachedObject *co;

   // Try global namespace, e.g. 'Arguments'
   co = tkscript->global_instance_namespace.findEntry(_sv);

   if(!co)
   {
      // Try module var namespace
      co = module_vars.findEntry(_sv);
   }

   if(!co)
   {
      // Try namespace list
      TKS_ActiveNamespace *a = _nspaceList;
      while(!co && a)
      {
         co = a->nspace->findVar(_sv);
         a = a->next;
      }
   }

   return co;
}

TKS_CachedObject *TKS_CachedScript::findVarLocal(YAC_String *_sv) const {
   //
   // Find local function variable or static class member of current class
   //
   TKS_CachedObject *co = NULL;
   if(cfunction)
   {
      co = cfunction->findFunVar(_sv);
   }
   if(co == NULL)
   {
      if(cclassdecl)
      {
         co=cclassdecl->getStaticMember(_sv);
         if(co != NULL)
         {
            if(!cclassdecl->allowMemberAccess(_sv, this, cclassdecl, create_namespace))
            {
               co = NULL;
            }
         }
      }
   }
   return co;
}


sBool TKS_CachedScript::scanClasses(void) {
   sUI toki = 0;
   sU16 tok = TOKEN_INT;
   sU16 ntok = TOKEN_INT;
   sU16 nntok = TOKEN_INT;
   sU16 nnntok = TOKEN_INT;
   sU32 *lineptr = compiled_script->lines.elements;
   sU16 *stok = compiled_script->tokens.elements;
   sUI num_tok = compiled_script->tokens.num_elements;
   state = 0;
   state_index = 0;
   YAC_String *sv = NULL;
   YAC_String *tsv = NULL;
   YAC_String ctempname;
   cclassdecl = NULL;
   TKS_CachedObject *co = NULL;
   sU8 vartype = 0; // 1=int, 2=float, 3=object/class, 4=string, 5=Pointer
   sU8 vardecl_tag = 0;
   sU8 vardecl_tagcount = 0;
   sUI prot_flag = 0;   //
   sUI formal_arg_type = 0;
   sUI static_flag = 0;
   sUI tempi = 0;
   sBool is_local = 0;
   sBool b_var_ref = 0; // '*' in MyClass *arrayArg[], or String[] *arrayArg[] (=> PointerArray arg)
   sBool new_local = 0;
   sBool fail_local = 0;
   sBool is_array=0;
   sBool b_pointerarray=0;
   // xxx check for duplicate implicit Main module
   name.copy("Main");
   cfunction = NULL;
   popnode.node = NULL;
   YAC_String *ctempname2 = NULL;
   YAC_String *fun_varname = NULL;
   YAC_String *last_class_constant_name = NULL;
   YAC_String *next_mutex_name = NULL;
   sBool nextisdestructorname=0; // prepend '~'
   sBool b_expect_method_name = 0;
   sBool b_anonymous_mutex = 0;
   sBool b_this_mutex      = 0;
   sBool b_method_abstract = 0;
   sBool b_method_virtual  = 0;
   sBool b_class_abstract  = 0;
   sBool b_member_ref      = 0;
   TKS_ClassDecl *temp_classdecl = NULL;
   TKS_ExceptionType *temp_baseexception = NULL;
   first_stat = NULL;
   TKS_Namespace *cnspace = NULL; // current namespace, only valid temporary. is reset to NULL after reading (paranoia)

   useDefaultNamespace();

   if(is_tmod)
   {
      tagged_vars.alloc(TKS_MAX_TEMPSCRIPT_TAGGED_VARS);
   }

   if(stok && lineptr)
   {
      sS32 linenr = -sS32(linenr_offset);
      for(; (toki < num_tok) && !Dtkscompiler->parse_error ; toki++)
      {
         tok=*stok++;
         if(tok<=TOKEN_STRINGVAL)
         {
            tok=TOKEN_VAR;
            if((toki+1)<num_tok)
            {
               ntok=*stok++;
               if(ntok>=compiled_script->strings.num_elements)
               {
                  Dprintf("\n[---] internal compiler error: accessing string %i/%i\n",
                          compiled_script->strings.num_elements,
                          compiled_script->strings.max_elements
                          );
                  Dclassparseerror;
               }
               sv=&compiled_script->strings.elements[ntok]; // xxx boundary check
               toki++;
               if((toki+1)<num_tok)
               {
                  ntok=*stok;
                  if(ntok<=TOKEN_STRINGVAL)
                  {
                     ntok=TOKEN_VAR;
                     if((toki+3)<num_tok)
                     {
                        nntok=stok[2];
                        if(nntok<=TOKEN_STRINGVAL)
                        {
                           nntok=TOKEN_VAR;
                           if((toki+5)<num_tok)
                           {
                              nnntok = stok[4];
                              if(nnntok<=TOKEN_STRINGVAL)
                              {
                                 nnntok = TOKEN_VAR;
                              }
                           }
                           else
                           {
                              nnntok = TOKEN_INT;
                           }
                        }
                        else
                        {
                           if((toki+4)<num_tok)
                           {
                              nnntok = stok[3];
                              if(nnntok <= TOKEN_STRINGVAL)
                              {
                                 nnntok = TOKEN_VAR;
                              }
                           }
                           else
                           {
                              nnntok = TOKEN_INT;
                           }
                        }
                     }
                     else
                     {
                        nntok=TOKEN_INT;
                        nnntok = TOKEN_INT;
                     }
                  }
                  else
                  {
                     if((toki+2)<num_tok)
                     {
                        nntok=stok[1];
                        if(nntok<=TOKEN_STRINGVAL)
                        {
                           nntok=TOKEN_VAR;
                           if((toki+4)<num_tok)
                           {
                              nnntok = stok[3];
                              if(nnntok<=TOKEN_STRINGVAL)
                              {
                                 nnntok = TOKEN_VAR;
                              }
                           }
                           else
                           {
                              nnntok = TOKEN_INT;
                           }
                        }
                        else
                        {
                           if((toki+3)<num_tok)
                           {
                              nnntok = stok[2];
                              if(nnntok <= TOKEN_STRINGVAL)
                              {
                                 nnntok = TOKEN_VAR;
                              }
                           }
                           else
                           {
                              nnntok = TOKEN_INT;
                           }
                        }
                     }
                     else
                     {
                        nntok=TOKEN_INT;
                        nnntok = TOKEN_INT;
                     }
                  }
               }
               else
               {
                  ntok=TOKEN_INT;
                  nntok=TOKEN_INT;
                  nnntok = TOKEN_INT;
               }
            }
            else
            {
               Dprintf("\n[---] internal error: string index must follow token \n");
               Dclassparseerror;
            }
         }
         else
         {
            if((toki+1)<num_tok)
            {
               ntok=*stok;
               if(ntok<=TOKEN_STRINGVAL)
               {
                  ntok=TOKEN_VAR;
                  if((toki+3)<num_tok)
                  {
                     nntok=stok[2];
                     if(nntok<=TOKEN_STRINGVAL)
                     {
                        nntok=TOKEN_VAR;
                        if((toki+5)<num_tok)
                        {
                           nnntok = stok[4];
                           if(nnntok <= TOKEN_STRINGVAL)
                           {
                              nnntok = TOKEN_VAR;
                           }
                        }
                        else
                        {
                           nnntok = TOKEN_INT;
                        }
                     }
                     else
                     {
                        if((toki+4)<num_tok)
                        {
                           nnntok = stok[3];
                           if(nnntok <= TOKEN_STRINGVAL)
                           {
                              nnntok = TOKEN_VAR;
                           }
                        }
                        else
                        {
                           nnntok = TOKEN_INT;
                        }
                     }
                  }
                  else
                  {
                     nntok=TOKEN_INT;
                     nnntok = TOKEN_INT;
                  }
               }
               else
               {
                  if((toki+2)<num_tok)
                  {
                     nntok=stok[1];
                     if(nntok<=TOKEN_STRINGVAL)
                     {
                        nntok=TOKEN_VAR;
                        if((toki+4)<num_tok)
                        {
                           nnntok = stok[3];
                           if(nnntok <= TOKEN_STRINGVAL)
                           {
                              nnntok = TOKEN_VAR;
                           }
                        }
                        else
                        {
                           nnntok = TOKEN_INT;
                        }
                     }
                     else
                     {
                        if((toki+3)<num_tok)
                        {
                           nnntok = stok[2];
                           if(nnntok <= TOKEN_STRINGVAL)
                           {
                              nnntok = TOKEN_VAR;
                           }
                        }
                        else
                        {
                           nnntok = TOKEN_INT;
                        }
                     }
                  }
                  else
                  {
                     nntok=TOKEN_INT;
                     nnntok = TOKEN_INT;
                  }
               }
            }
            else
            {
               ntok=TOKEN_INT;
               nntok=TOKEN_INT;
               nnntok = TOKEN_INT;
            }
         }


               while (toki >= *lineptr)
               {
                  linenr++;
                  lineptr++;
                  if(tkscript->configuration.debug_level > 98)
                     tkscript->printf("[...] scanning line %i\n", linenr);
               }
                  /*#ifndef TKS_JIT
                    if(tok==TOKEN_COMPILE) tok=TOKEN_NOP;
                    if(ntok==TOKEN_COMPILE) ntok=TOKEN_NOP;
                    #endif*/

               switch(state)
               {
                  default:
                     switch(tok)
                     {
                     case TOKEN_ABSTRACT:
                        if(TOKEN_CLASS == ntok)
                        {
                           b_class_abstract = 1;
                        }
                        else
                        {
                           Dclassparseerror;
                        }
                        break;

                     case TOKEN_CLASS:
                        {
#ifndef TKS_ALLOW_TEMP_CLASSES
                           if(is_tmod)
                           {
                              Dprintf("[---] \"class\" not allowed in temporary modules.\n");
                              Dclassparseerror;
                           }
                           else
#endif // TKS_ALLOW_TEMP_CLASSES
                           {
                              state=ST_CLASSDECL;
                              state_index=1;
                           }
                        }
                        break;

                     case TOKEN_ASSIGN:
                        state = ST_MUTEXDECL;
                        state_index = 1;
                        break;

                     case TOKEN_USE:
                        state=ST_UFUNEVENT;
                        state_index=1;
                        break;

                     case TOKEN_NAMESPACE:
                        state = ST_NAMESPACE;
                        state_index = 1;
                        break;

                     case TOKEN_MODULE:
                        state=ST_MODULE;
                        break;

                     case TOKEN_CONSTRAINT:
                        state=ST_CONSTRAINT;
                        break;

                     case TOKEN_DEFINE:
                     case TOKEN_DEFINEALT:
                        state=ST_DEFINE;
                        state_index=1;
                        break;
                        // else consume, find next classdecl
                     }
                     break;

                  case ST_DEFINE:
                     switch(state_index)
                     {
                        case 1:
                           if(is_tmod) // [02Jan2018] don't allow (global) defines in tempscripts
                           {
                              Dprintf("[---] \"define\" not allowed in temporary modules.\n");
                              Dclassparseerror;
                           }

                           if(tok!=TOKEN_VAR)
                           {
                              Dclassparseerror;
                           }

                           if(sv->compare(&tok_exception))
                           {
                              // define exception <name> [extends <basename>] ;
                              state_index = 50;
                           }
                           else
                           {
                              // define <name> <value>
                              co=findConst(sv);
                              if(co)
                              {
                                 Dprintf("\n[---] define: constant value \"%s\" already used.\n",
                                    (char*)sv->chars);
                                 Dclassparseerror;
                              }
                              tsv=sv;//Dsetparams(0, sv);
                              state_index++;
                           }
                           break;
                        case 2:
                           if(tok==TOKEN_SUB)
                           {
                              state_index++;
                           }
                           else
                           {
                              if(tok!=TOKEN_VAR)
                              {
                                 Dclassparseerror;
                              }
                                 //addConstant(Dgetparams(0), sv);
                              addConstant(tsv, sv);
                              state_index=0;
                              state=0;
                           }
                           break;
                        case 3:
                           if(tok!=TOKEN_VAR)
                           {
                              Dclassparseerror;
                           }
                           {
                              YAC_String t;
                              t.copy("-");
                              t.append(sv);
                                 //addConstant(Dgetparams(0), &t);
                              addConstant(tsv, &t);
                              state_index=0;
                              state=0;
                           }
                           break;

                        // ST_DEFINE<exception>, expect exception <name>
                        case 50:
                           if(tok != TOKEN_VAR)
                           {
                              Dclassparseerror;
                           }
                           {
                              TKS_ExceptionType *et = tkscript->getExceptionTypeByName(NULL, sv);
                              if(et != NULL)
                              {
                                 Dprintf("[---] exception type \"%s\" already defined.\n", (char*)sv->chars);
                                 Dclassparseerror;
                              }
                           }
                           tsv = sv; // remember exception name
                           state_index++;
                           break;
                        case 51:
                           if(tok == TOKEN_TERM) // ';' ?
                           {
                              if(!tkscript->addExceptionType(new TKS_ExceptionType(tkscript->getExceptionTypeById(TKS_EXCEPTION_ERROR), tsv)))
                              {
                                 Dprintf("[---] failed to add exception type.\n");
                                 Dclassparseerror;
                              }
                              state_index=0;
                              state=0;
                           }
                           else if( (tok == TOKEN_EXTENDS) || (tok == TOKEN_TELSE) ) // extends , :
                           {
                              state_index++; // continue to parse base class name
                           }
                           break;
                        case 52:
                           if(NULL == cnspace)
                           {
                              Dclassparsenspace(53);
                           }

                           if(tok != TOKEN_VAR) // exception base class name
                           {
                              Dclassparseerror;
                           }
                           if(TOKEN_MEMBER == ntok) // MyClass::MyBaseException, mynamespace::MyClass::MyBaseException ?
                           {
                              // must be classname
                              if(NULL == cnspace)
                              {
                                 temp_classdecl = Dtkscompiler->findClassDeclNamespaceList(sv, active_namespaces);
                              }
                              else
                              {
                                 temp_classdecl = Dtkscompiler->findClassDeclNamespace(sv, cnspace);
                              }
                              if(NULL == temp_classdecl)
                              {
#ifdef TKS_DCON
                                 if(NULL != cnspace)
                                 {
                                    Dprintf("[---] class \"%s::%s\" not found while parsing base exception type.\n",
                                       (char*)cnspace->name.chars,
                                       (char*)sv->chars
                                       );
                                 }
                                 else
                                 {
                                    Dprintf("[---] class \"%s\" not found while parsing base exception type.\n", (char*)sv->chars);
                                 }
#endif
                                 Dclassparseerror;
                              }
                              else
                              {
                                 cnspace = NULL;
                                 state_index = 54;
                              }
                           }
                           else
                           {
                              // MyClass::MyBaseException
                              if(NULL != cnspace)
                              {
                                 Dclassparseerror; // do not allow mynamespace::MyBaseException
                              }
                              TKS_ExceptionType *ebt = tkscript->getExceptionTypeByName(NULL, sv);
                              if(ebt == NULL)
                              {
                                 Dprintf("[---] unknown exception base type \"%s\".\n", (char*)sv->chars);
                                 Dclassparseerror;
                              }
                              else
                              {
                                 if(!tkscript->addExceptionType(new TKS_ExceptionType(ebt, tsv)))
                                 {
                                    Dprintf("[---] failed to add exception type.\n");
                                    Dclassparseerror;
                                 }
                                 state_index = 0;
                                 state = 0;
                              }
                           }
                           break;
                        case 53: // consume '::' between namespace and exception basename
                           state_index = 52;
                           break;
                        case 54: // MyClass::MyBaseException, mynamespace::MyClass::MyBaseException
                           state_index++; // consume '::' between classname and base exception name
                           break;
                        case 55: // parse MyBaseException in MyClass::MyBaseException or mynamespace::MyClass::MyBaseException
                           if(TOKEN_VAR != tok)
                           {
                              Dclassparseerror;
                           }
                           temp_baseexception = tkscript->getExceptionTypeByName(temp_classdecl, sv);
                           if(NULL == temp_baseexception)
                           {
                              Dprintf("[---] unknown base exception \"%s::%s::%s\".\n",
                                 (char*)temp_classdecl->nspace->name.chars,
                                 (char*)temp_classdecl->name.chars, (char*)sv->chars);
                              Dclassparseerror;
                           }
                           if(!tkscript->addExceptionType(new TKS_ExceptionType(temp_baseexception, tsv)))
                           {
                              Dprintf("[---] failed to add exception type.\n");
                              Dclassparseerror;
                           }

                           state_index = 0;
                           state = 0;
                           break;
                     }
                     break;


                  case ST_MODULE:
                     {
                        TKS_CachedScript *m = Dtkscompiler->findModule(sv);
                        if(m)
                        {
                           Dprintf("[---] module name \"%s\" already in use (first used in file \"%s\").\n", (char*) sv->chars, (char*) m->src_name.chars);
                           Dclassparseerror;
                        }
                     }

                     name.yacCopy(sv);
                     state=0;
                     state_index=0;
                     break;

                  case ST_MUTEXDECL:
                     switch(state_index)
                     {
                     case 1:
                        state_index++; // skip <mutexname>
                        break;
                     case 2:
                        // consume '='
                        state=0;
                        state_index=0; // the =<mtxname>= statement is handled in 2nd pass
                        break;
                     }
                     break;

                  case ST_CONSTRAINT:
                     if(tok!=TOKEN_VAR)
                     {
                        Dclassparseerror;
                     }
                     else
                     {
                        if( ! sv->compare(&tok_notnull) )
                        {
#ifdef DX_SCRIPTCONSTRAINTS
                           TKS_ScriptConstraint *scs=new TKS_ScriptConstraint();
                           scs->name.yacCopy(sv);
                           scs->constraint_expr = NULL;
                           tkscript->addScriptConstraint(scs);
#endif // DX_SCRIPTCONSTRAINTS
                        }
                        state=0;
                        state_index=0;
                     }
                     break;

                  case ST_UFUNEVENT: // use <pluginname>; or use namespace <mynamespace>
                     switch(state_index)
                     {
                        case 1:
                           if(TOKEN_VAR == tok)
                           {
                              if(ntok==TOKEN_TERM)
                              {
                                 // ---- load plugin (use particlesystem; syntax) ----
                                 if(!sv->compare(&tok_callbacks))
                                 {
                                    TKS_PluginRegistry *pr=(TKS_PluginRegistry*)tkscript->plugins;

#ifdef AUTOLOAD_TKOPENGL
                                    if(sv->compare("tkopengl")) // XXX autoload SDL plugin
                                    {
                                       // (note) must load SDL before OpenGL
                                       YAC_String s; s.visit("tksdl");
                                       pr->loadPlugin(&s);
                                    }
#endif // AUTOLOAD_TKOPENGL

                                    if(!pr->loadPlugin(sv))
                                    {
                                       Dclassparseerror;
                                    }
#ifdef AUTOLOAD_TKOPENGL
                                    if(sv->compare("tksdl")) // XXX autoload opengl plugin
                                    {
                                       YAC_String s; s.visit("tkopengl");
                                       pr->loadPlugin(&s);
                                    }
#endif // AUTOLOAD_TKOPENGL
                                 }
                              }
                              state=0;
                              state_index=0;
                              // consume next
                           }
                           else if(TOKEN_NAMESPACE == tok)
                           {
                              state_index++;
                           }
                           break;
                        case 2:
                           // use namespace <mynamespace>; or use namespace default;
                           if(TOKEN_DEFAULT == tok)
                           {
                              useDefaultNamespace();
                           }
                           else if(TOKEN_VAR != tok)
                           {
                              Dclassparseerror;
                           }
                           else
                           {
                              TKS_Namespace *ns = tkscript->findNamespace(sv);
                              if(NULL == ns)
                              {
                                 Dprintf("[---] namespace \"%s\" does not exist.\n",
                                    (char*) sv->chars
                                    );
                                 Dclassparseerror;
                              }
                              useNamespace(ns); // Add namespace to search list
                           }
                           state = 0;
                           state_index = 0;
                           break;
                     }
                     break;

                  case ST_NAMESPACE:
                     // namespace default;
                     // namespace <mynamespacename>;
                     switch(state_index)
                     {
                     case 1:
                        if(TOKEN_DEFAULT == tok)
                        {
                           // Switch back to default namespace (i.e. clear namespace search list)
                           useDefaultNamespace();
                           selectNamespace(NULL);
                           state = 0;
                           state_index = 0;
                        }
                        else if(TOKEN_VAR == tok)
                        {
                           // Declare user-defined namespace
                           TKS_Namespace *ns;
                           ns = tkscript->findNamespace(sv);
                           if(NULL == ns)
                           {
                              ns = tkscript->addNamespace(sv);
                           }
                           if(NULL == ns)
                           {
                              // Failed to add namespace (already exists or maximum number of namespaces reached)
                              Dclassparseerror;
                           }
                           else
                           {
                              // Add namespace to search list
                              useNamespace(ns);
                              selectNamespace(ns);
                              state = 0;
                              state_index = 0;
                           }
                        }
                        else
                        {
                           Dclassparseerror;
                        }
                        break;
                     }
                     break;


                  case ST_CLASSDECL:
                     switch(state_index)
                     {
                        case 0: // "class"
                           if(tok!=TOKEN_CLASS)
                           {
                              Dclassparseerror;
                           }
                           state_index++;
                           vardecl_tag=0;
                           break;
                        case 1: // "MyClass"
                           if(tok!=TOKEN_VAR)
                           {
                              Dprintf("\n[---] expected classname.\n");
                              Dclassparseerror;
                           }
                           ////cclassname = sv;
                           last_class_constant_name = NULL;
                           next_mutex_name = NULL;
                           b_anonymous_mutex = 0;
                           b_this_mutex      = 0;
                           b_method_abstract = 0;
                           b_method_virtual  = 0;
                           b_member_ref      = 0;
                           is_array          = 0;
                           cclassdecl = Dtkscompiler->addClassDecl(sv, this, Dsrcloc, (NULL != tmod_class_nsp) ? tmod_class_nsp : create_namespace);
                           if(!cclassdecl)
                           {
                              // ---- could not be declared, e.g. class decl table is full
                              Dclassparseerror;
                           }
                           else
                           {
                              if(cclassdecl->is_declared)
                              {
                                 Dprintf("\n[---] class \"%s\" is already declared.\n",
                                    (char*)sv->chars);
                                 Dclassparseerror;
                              }
                              else
                              {
                                 // ---- class is newly added or until now just forwared-referenced
                                 // ---- by other classes
                                 cclassdecl->is_declared=1;

                                 cclassdecl->b_abstract = b_class_abstract;
                                 b_class_abstract = 0;

                                 if(
                                    (ntok==TOKEN_TELSE) || // : ancestor list optional
                                    (ntok==TOKEN_EXTENDS)
                                    )
                                 {
                                    state_index=6;
                                 }
                                 else
                                 {
                                    state_index++;
                                 }
                              }
                           }
                           break;
                        case 2: // {
                           if(tok!=TOKEN_BEGBLOCK)
                           {
                              Dclassparseerror;
                           }
                           b_expect_method_name = 0;
                           state_index++;
                           prot_flag=TKS_PROT_NONE;
                           vartype=0;
                           static_flag=0;
                           break;

                        case 3: // vardecl, fundecl
                           cfunction=0;
                           if(b_expect_method_name)
                           {
                              if(tok!=TOKEN_VAR)
                              {
                                 if(ntok!=TOKEN_BEGEXPR)
                                 {
                                    Dclassparseerror;
                                 }
                              }
                           }

                           switch(tok)
                           {
                              default:
                                 Dclassparseerror;
                                 break;
                              case TOKEN_EXPLAIN: // explain "text" ;
                                 state_index=80;
                                 break;
                              case TOKEN_DEFINE:
                                 if(static_flag)
                                 {
                                    Dclassparseerror;
                                 }
                                 if(prot_flag)
                                 {
                                    Dprintf("[---] warning: class constant protection flags are ignored.\n");
                                 }
                                 state_index=200; // parse class constant
                                 break;
                              case TOKEN_FUNCTION: // ST_UFUNBODY
                                 state_index=100;
                                 break;
                              case TOKEN_ENDBLOCK:
                                    // end of class definition
                                 state=0;
                                 state_index=0;
                                 break;
                              case TOKEN_ASSIGN: // =, =test=
                                 if(nntok==TOKEN_ASSIGN)
                                 {
                                    // use/(implicetely)create named mutex for method/function
                                    state_index = 95; // parse next_mutex_name
                                    b_anonymous_mutex = 0; // *should* already be reset
                                 }
                                 else
                                 {
                                    // create anonymous mutex for method/function
                                    b_anonymous_mutex = 1;
                                    next_mutex_name = NULL; // *should* already be reset
                                 }
                                 break;
                              case TOKEN_CMPEQ: // ==, method is synchronized on "this"
                                 cclassdecl->b_this_mutex = 1;
                                 b_this_mutex = 1;
                                 break;
                              case TOKEN_METHOD:
                                 if(static_flag)
                                 {
                                    state_index=100;
                                 }
                                 else
                                 {
                                    //state_index=99;
                                    b_expect_method_name = 1;
                                 }
                                 break;
                              case TOKEN_TAG:
                                 if(static_flag)
                                 {
                                    Dprintf("[---] static class members cannot be serialized.\n");
                                    Dclassparseerror;
                                 }
                                 else
                                 {
                                    vardecl_tag=TKS_CachedObject::TAGGED;
                                    vardecl_tagcount++;
                                 }
                                 break;
                              case TOKEN_STATIC:
                                 if(static_flag)
                                 {
                                    Dclassparseerror;
                                 }
                                 static_flag=1;
                                 break;
                              case TOKEN_PUBLIC:
                                 if(prot_flag!=TKS_PROT_NONE)
                                 {
                                    Dclassparseerror;
                                 }
                                 prot_flag=TKS_PROT_PUBLIC;
                                 break;
                              case TOKEN_PROTECTED:
                                 if(prot_flag!=TKS_PROT_NONE)
                                 {
                                    Dclassparseerror;
                                 }
                                 prot_flag=TKS_PROT_PROTECTED;
                                 break;
                              case TOKEN_PRIVATE:
                                 if(prot_flag!=TKS_PROT_NONE)
                                 {
                                    Dclassparseerror;
                                 }
                                 prot_flag=TKS_PROT_PRIVATE;
                                 break;
                              case TOKEN_MODULE:
                                 if(prot_flag!=TKS_PROT_NONE)
                                 {
                                    Dclassparseerror;
                                 }
                                 prot_flag=TKS_PROT_MODULE;
                                 break;
                              case TOKEN_NAMESPACE:
                                 if(prot_flag!=TKS_PROT_NONE)
                                 {
                                    Dclassparseerror;
                                 }
                                 prot_flag = TKS_PROT_NSPACE;
                                 break;
                              case TOKEN_INT:
                                 vartype=1; // integer declaration
                                 state_index++;
                                 break;
                              case TOKEN_FLOAT:
                                 vartype=2; // float declaration
                                 state_index++;
                                 break;
                              case TOKEN_STRING:
                                 vartype=4; // String declaration
                                 state_index++;
                                 break;
                              case TOKEN_BITNOT: // Constructor declaration ~MyClass()
                                 if( (ntok!=TOKEN_VAR) || nextisdestructorname )
                                 {
                                    Dclassparseerror;
                                 }
                                 nextisdestructorname=1;
                                 break;
                              case TOKEN_OBJECT:
                                 vartype=5;
                                 state_index++;
                                 break;
                              case TOKEN_MEMBER:
                                 Dclassdefaultnspace;
                                 break;

                              case TOKEN_ABSTRACT:
                                 if(b_method_abstract)
                                 {
                                    Dprintf("[---] \"abstract\" keyword used more than once in method declaration.\n");
                                    Dclassparseerror;
                                 }
                                 else if(b_method_virtual)
                                 {
                                    Dclassparseerror;
                                 }
                                 else
                                 {
                                    b_method_abstract = 1;
                                 }
                                 break;

                              case TOKEN_VIRTUAL:
                                 if(b_method_virtual)
                                 {
                                    Dprintf("[---] \"virtual\" keyword used more than once in method declaration.\n");
                                    Dclassparseerror;
                                 }
                                 else if(b_method_abstract)
                                 {
                                    Dclassparseerror;
                                 }
                                 else
                                 {
                                    b_method_virtual = 1;
                                 }
                                 break;

                              case TOKEN_VAR: // Object or Class instantiation
                                 {
                                    ctempname.yacCopy(sv);

                                    if(sv->compare(&tok_var))
                                    {
                                       Dprintf("[---] variant field type is not allowed within classes.\n");
                                       Dclassparseerror;
                                    }


                                    if(nextisdestructorname)
                                    {
                                       Dclassassertnonspace;

                                       if(static_flag)
                                       {
                                          Dclassparseerror;
                                       }
                                       nextisdestructorname=0;
                                       // ~MyClass()
                                       ctempname.insert(0, "~");
                                    }
                                    if(b_expect_method_name||(ntok==TOKEN_BEGBLOCK)||(ntok==TOKEN_COMPILE)||(ntok==TOKEN_BEGEXPR)||(ntok==TOKEN_TELSE)||(ntok==TOKEN_RETURNS))
                                    {
                                       Dclassassertnonspace;

                                       // myfun(..
                                       //method_rtti=0;
                                       b_expect_method_name = 0;
                                       //cfunction=cclassdecl->addMethod(&ctempname, prot_flag);


                                       if(static_flag)
                                       {
                                          if(cclassdecl->findFunction(&ctempname))
                                          {
                                             Dprintf("[---] class \"%s\" already has a function called \"%s\".\n", (char*)cclassdecl->name.chars, (char*)sv->chars);
                                             Dclassparseerror;
                                          }
                                          else if(b_this_mutex)
                                          {
                                             Dprintf("[---] static method \"%s::%s\" cannot be synchronized on \"this\".\n", (char*)cclassdecl->name.chars, (char*)sv->chars);
                                             Dclassparseerror;
                                          }
                                          cfunction = cclassdecl->addFunction(&ctempname, prot_flag, b_anonymous_mutex, next_mutex_name, Dsrcloc);
                                          if(NULL == cfunction)
                                          {
                                             Dclassparseerror;
                                          }
                                       }
                                       else
                                       {
                                          if(cclassdecl->findMethod(&ctempname))
                                          {
                                             Dprintf("[---] class \"%s\" already has a method called \"%s\".\n", (char*)cclassdecl->name.chars, (char*)sv->chars);
                                             Dclassparseerror;
                                          }
                                          cfunction = cclassdecl->addMethod(
                                             &ctempname,
                                             prot_flag,
                                             b_this_mutex,
                                             b_anonymous_mutex,
                                             next_mutex_name,
                                             b_method_abstract,
                                             b_method_virtual,
                                             Dsrcloc
                                             );
                                          if(NULL == cfunction)
                                          {
                                             Dclassparseerror;
                                          }
                                       }
                                       b_anonymous_mutex = 0;
                                       b_this_mutex      = 0;
                                       b_method_abstract = 0;
                                       b_method_virtual  = 0;
                                       next_mutex_name   = NULL;
                                       prot_flag         = TKS_PROT_NONE;
                                       vartype           = 0;
                                       static_flag       = 0;
                                       formal_arg_type   = YAC_TYPE_VARIANT;
                                       is_array          = 0;
                                       is_local          = 0;
                                       b_pointerarray    = 0;
                                       if(ntok==TOKEN_BEGBLOCK) // function foo { ??
                                       {
                                          state_index=108; // consume function body
                                       }
                                       else
                                       {
                                          if((ntok==TOKEN_TELSE)||(ntok==TOKEN_RETURNS))
                                             state_index=1030;
                                          else if(ntok==TOKEN_COMPILE)
                                             state_index=107;
                                          else
                                             state_index=101;
                                       }

                                       //method_argi=0;
                                       //state_index=9;
                                    }
                                    else
                                    {
                                       if(nextisdestructorname)
                                       {
                                          // ~MyClass myclassinstance;
                                          Dclassparseerror;
                                       }

                                       Dclassparsenspace(10);

                                       // Member declaration, ctempname holds member type
                                       vartype = 3; // or 6
                                       state_index++;
                                    }
                                 }
                                 break;
                           }
                           break;

                        case 31: // consume first ']' in Double[]*aoa_ref[]
                           if(TOKEN_ENDARRAY != tok)
                           {
                              Dclassparseerror;
                           }
                           state_index = 4;
                           break;

                        case 32: // consume first '[' in Double*[]*aoa_ref[]
                           if(TOKEN_BEGARRAY != tok)
                           {
                              Dclassparseerror;
                           }
                           state_index = 31;
                           break;

                        case 4:
                           if(TOKEN_BEGARRAY == tok)
                           {
                              if(is_array)
                              {
                                 Dclassparseerror;
                              }
                              is_array = 1;
                              // '*' must follow after ']'
                              state_index = 31;
                              break;
                           }
                           if(TOKEN_MUL == tok) // '*' in SomeClass*ref_member
                           {
                              if(b_member_ref)
                              {
                                 Dclassparseerror;
                              }
                              else if(TOKEN_BEGARRAY == ntok)
                              {
                                 if(!is_array)
                                 {
                                    // String*[], Double*[]
                                    b_pointerarray = 1;
                                    state_index = 32;
                                    break;
                                 }
                                 else
                                 {
                                    Dclassparseerror;
                                 }
                              }
                              else
                              {
                                 b_member_ref = 1;
                                 break;
                              }
                           }
                           if(tok!=TOKEN_VAR)
                           {
                                 // expected variable instance name
                              Dclassparseerror;
                           }
                           if(cclassdecl->findMemberSuper(sv, static_flag))
                           {
                              Dprintf("\n[---] class \"%s\" member \"%s\" already declared.\n",
                                               (char*)cclassdecl->name.chars, (char*)sv->chars);
                              Dclassparseerror;
                           }
                           else
                           {
                              ctempname2 = sv; // remember member name for initializers
                              if(b_pointerarray)
                              {
                                 // int*[] ia, String*[] sa, Double*[] da, ..
                                 // int*[]*ia, String*[]*sa, Double*[]*da, ..
                                 vartype=5; // Pointer(-Array)
                                 is_array=1;
                                 b_pointerarray=0;
                              }
                              else if(is_array)
                              {
                                 if(ntok == TOKEN_BEGARRAY)
                                 {
                                    // int[] ia[], String[] saoa[], Double[] da[], ..
                                    // int[]*ia[], String[]*saoa[], Double[]*da[], ..
                                    if(b_member_ref && (nntok != TOKEN_ENDARRAY))
                                    {
                                       Dprintf("\n[---] array size not allowed in array-of-arrays ref\n");
                                       Dclassparseerror;
                                    }
                                    vartype=5; // PointerArray (array of arrays)
                                    is_array=1;
                                 }
                                 // else int[] ia, String[] sa, Double[] da, ..
                              }
                              else
                              {
                                 // int ia[1], String sa[2], Double da[3], ..
                                 is_array = (ntok == TOKEN_BEGARRAY);
                                 if(b_member_ref && is_array)
                                 {
                                    // int *ia[1], String *sa[2], Double *da[3]
                                    vartype=5; // PointerArray
                                    b_member_ref=0;
                                 }
                              }
                              switch(vartype)
                              {
                                 default:
                                 {
                                    Dprintf("\n[---] internal error: unknown vartype in class decl.\n");
                                    Dclassparseerror;
                                 }
                                 break;
                                 case 1:
                                       // int
                                    Dclassassertnonspace;

                                    if(is_array)
                                    {
                                       cclassdecl->addInt(sv, 1, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                       if(ntok==TOKEN_BEGARRAY)
                                          state_index=14;  // increment below to state_index=15
                                    }
                                    else
                                    {
                                       cclassdecl->addInt(sv, 0, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                    }
                                    b_member_ref = 0;
                                    is_array = 0;
                                    break;
                                 case 2:
                                       // float
                                    Dclassassertnonspace;

                                    if(is_array)
                                    {
                                       cclassdecl->addFloat(sv, 1, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                       if(ntok==TOKEN_BEGARRAY)
                                          state_index=14;  // increment below to state_index=15
                                    }
                                    else
                                    {
                                       cclassdecl->addFloat(sv, 0, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                    }
                                    b_member_ref = 0;
                                    is_array = 0;
                                    break;
                                 case 3:
                                    // Object/Class instantiation
                                    {
                                       TKS_ClassDecl *ucl;
                                       TKS_ClassTemplate *tmp;

                                       if(!findAddClassType(&ctempname, active_namespaces, cnspace, Dsrcloc, &tmp, &ucl))
                                       {
                                          Dclassparseerror;
                                       }
                                       else if(NULL != tmp)
                                       {
                                          // C++ class
                                          if(is_array)
                                          {
                                             cclassdecl->addObject(tmp, sv, 1, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                             if(ntok==TOKEN_BEGARRAY)
                                                state_index=14;  // increment below to state_index=15
                                          }
                                          else
                                          {
                                             cclassdecl->addObject(tmp, sv, 0, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                          }
                                          b_member_ref = 0;
                                          is_array = 0;
                                       }
                                       else
                                       {
                                          // Script class
                                          if(is_array)
                                          {
                                             if(ctempname.compare(cclassdecl->name))
                                             {
                                                Dprintf("\n[---] cannot instantiate array of class \"%s\" in \"%s\".\n",
                                                                 (char*)ctempname.chars, (char*)ctempname.chars);
                                                Dclassparseerror;
                                             }
                                             else
                                             {
                                                cclassdecl->addClass(ucl, sv, 1, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                                if(ntok==TOKEN_BEGARRAY)
                                                   state_index=14;  // increment below to state_index=15
                                             }
                                          }
                                          else
                                          {
                                             cclassdecl->addClass(ucl, sv, 0, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                          }
                                          b_member_ref = 0;
                                          is_array = 0;
                                       }

                                       // Reset current namespace ptr
                                       cnspace = NULL;

                                    }
                                    break;
                                 case 4:
                                       // String instantiation
                                    if(is_array)
                                    {
                                       cclassdecl->addString(sv, 1, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                       if(ntok==TOKEN_BEGARRAY)
                                          state_index=14;  // increment below to state_index=15
                                    }
                                    else
                                    {
                                       cclassdecl->addString(sv, 0, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                    }
                                    b_member_ref = 0;
                                    is_array = 0;
                                    break;
                                 case 5:
                                    // Pointer instantiation (Object mypointer)
                                    if(is_array)
                                    {
                                       cclassdecl->addPointer(sv, 1, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                       if(ntok==TOKEN_BEGARRAY)
                                          state_index=14;  // increment below to state_index=15
                                    }
                                    else
                                    {
                                       cclassdecl->addPointer(sv, 0, vardecl_tag, vardecl_tagcount, prot_flag, static_flag, b_member_ref);
                                    }
                                    b_member_ref = 0;
                                    is_array = 0;
                                    break;

                              }
                              state_index++;
                           }
                           break;
                              //classdecl_check_term:
                        case 5:
                           if(tok==TOKEN_TERM)
                           {
                              vardecl_tag=0;
                              prot_flag=TKS_PROT_NONE;
                              vartype=0;
                              static_flag=0;
                              state_index=3; // loop class vardecl
                           }
                           else
                              if(tok==TOKEN_LISTSEP)
                              {
                                    // ---- loop member decl ----
                                 state_index=4;
                              }
                              else
                              {
                                 if((tok==TOKEN_ASSIGN)||(tok==TOKEN_CMPLE))
                                 {
                                    TKS_ClassDeclMember *m=cclassdecl->findMemberSuper(ctempname2, static_flag);
                                    if(NULL == m)
                                    {
                                       tkscript->printf("xxx class member m=%p clzname=\"%s\" member=\"%s\" does not exist\n", m, cclassdecl->name.chars, ctempname2->chars);
                                    }
                                    cclassdecl->addInitializationMember(m, (tok==TOKEN_CMPLE));

                                    // ---- int i=42  /  String s="hello" ----
                                    // ---- consume expression
                                    sUI ri=0;
                                    sUI ti=toki+1;
                                    sSI numopenparentheses=1;
                                    sSI numopenblocks=1;
                                    sSI numopenarrays=1;
                                    while(ti<num_tok)
                                    {
                                       tok=stok[ri++];
                                       ti++;
                                       if(tok==TOKEN_BEGEXPR)
                                          numopenparentheses++;
                                       else if(tok==TOKEN_ENDEXPR)
                                          numopenparentheses--;
                                       else if(tok==TOKEN_BEGBLOCK)
                                          numopenblocks++;
                                       else if(tok==TOKEN_HASHINI)
                                          numopenblocks++;
                                       else if(tok==TOKEN_ENDBLOCK)
                                          numopenblocks--;
                                       else if(tok==TOKEN_BEGARRAY)
                                          numopenarrays++;
                                       else if(tok==TOKEN_ENDARRAY)
                                          numopenarrays--;
                                       else
                                       if(tok==TOKEN_LISTSEP)
                                       {
                                          if((numopenparentheses==1)&&
                                             (numopenblocks==1)&&
                                             (numopenarrays==1)
                                             )
                                             break;
                                       }
                                       else if(tok==TOKEN_TERM)
                                       {
                                          break;
                                       }
                                       else if(tok<=TOKEN_STRINGVAL)
                                       {
                                          ri++; // consume string index
                                          ti++;
                                       }
                                    }
                                    if(ti>=num_tok)
                                    {
                                       Dprintf("\n[---] reaching EOF while looking for \';\'.\n");
                                       Dclassparseerror;
                                    }
                                    stok+=ri;
                                    toki+=ri;
                                    if(stok[-1]==TOKEN_TERM)
                                    {
                                       vardecl_tag=0;
                                       prot_flag=TKS_PROT_NONE;
                                       vartype=0;
                                       static_flag=0;
                                       state_index=3; // loop class vardecl
                                    }
                                    else if(stok[-1]==TOKEN_LISTSEP)
                                    {
                                       // ---- loop member decl ----
                                       state_index=4;
                                    }
                                    else
                                    {
                                       Dclassparseerror;
                                    }

                                 } // if tok==ASSIGN
                                 else
                                 {
                                    // ---- neither ',' nor '='
                                    Dclassparseerror;
                                 }
                              }
                           break;
                        case 6:
                              // ancestorlist
                           if(
                              (tok!=TOKEN_TELSE)&&
                              (tok!=TOKEN_EXTENDS)
                              )
                           {
                              Dclassparseerror;
                           }
                           state_index++;
                           break;
                        case 7:
                           Dclassparsenspace(11);

                           if(TOKEN_VAR != tok)
                           {
                              Dprintf("\n[---] expected ancestor name.\n");
                              Dclassparseerror;
                           }
                           {
                              TKS_ClassTemplate *clt;
                              TKS_ClassDecl *ac;

                              // Find baseclass
                              if(!findAddClassType(sv, active_namespaces, cnspace, Dsrcloc, &clt, &ac))
                              {
                                 Dclassparseerror;
                              }
                              if(NULL == ac)
                              {
                                 // Found C++ type, cannot use that
                                 ac = Dtkscompiler->addClassDecl(sv, this, Dsrcloc, (NULL != tmod_class_nsp) ? tmod_class_nsp : create_namespace);
                              }

                              // /*if(NULL == cnspace)
                              // {
                              //    ac = Dtkscompiler->addClassDecl(sv, this, Dsrcloc, create_namespace);
                              // }
                              // else
                              // {
                              //    ac = Dtkscompiler->addClassDecl(sv, this, Dsrcloc, cnspace);

                              // }*/

                              if(!cclassdecl->addAncestor(ac))
                              {
#ifdef TKS_DCON
                                 if(NULL != cnspace)
                                 {
                                    Dprintf("\n[---] class \"%s::%s\" is already a base class of class \"%s\".\n",
                                       (char*)cnspace->name.chars,
                                       (char*)cclassdecl->name.chars,
                                       (char*)sv->chars
                                       );
                                 }
                                 else
                                 {
                                    Dprintf("\n[---] class \"%s\" is already a base class of class \"%s\".\n",
                                       (char*)cclassdecl->name.chars,
                                       (char*)sv->chars
                                       );
                                 }
#endif
                                 Dclassparseerror;
                              }
                              cnspace = NULL;
                           }
                           state_index++;
                           break;
                        case 8:
                              // "{" for class member decls or "," for ancestor list
                           if(tok==TOKEN_BEGBLOCK)
                           {
                              state_index=3;
                           }
                           else
                           {
                              if(tok==TOKEN_LISTSEP)
                              {
                                 state_index=7;
                              }
                              else
                              {
                                 Dclassparseerror;
                              }
                           }
                           break;
                        case 9: // method decl
                              // consume (
                           state_index++;
                           break;
                        case 10: // skip '::' between namespace and class member type
                           state_index = 3; // continue with parsing the member type name
                           break;
                        case 11: // skip '::' between namespace and baseclass name
                           state_index = 7; // continue parsing the baseclass list
                           break;

                        case 15:
                              // consume array decl []
                           if(tok!=TOKEN_BEGARRAY)
                           {
                              Dclassparseerror;
                           }
                           state_index++;
                           break;
                        case 16:
                           if(tok!=TOKEN_ENDARRAY)
                           {
                              if(tok==TOKEN_VAR)
                              {
                                 cclassdecl->setLastArraySize(getIntConst(sv));
                                 state_index++;
                              }
                              else
                              {
                                 Dprintf("\n[---] invalid array size expression, expected \"]\".\n");
                                 Dclassparseerror;
                              }
                           }
                           else
                              state_index=5;
                           break;
                        case 17:
                           if(tok!=TOKEN_ENDARRAY)
                           {
                              Dprintf("\n[---] invalid array size expression, expected \"]\".\n");
                              Dclassparseerror;
                           }
                           else
                              state_index=5;
                           break;

                        case 80: // explain "text" ;
                           if(tok!=TOKEN_VAR)
                           {
                              Dclassparseerror;
                           }
                           if(last_class_constant_name == NULL)
                           {
                              // Set documentation for class
                              if(! cclassdecl->documentation.isBlank() )
                              {
                                 Dprintf("[---] class documentation text already set.\n");
                                 Dclassparseerror;
                              }
                              cclassdecl->documentation.yacCopy(sv);
                           }
                           else
                           {
                              // Set documentation for last defined class constant
                              if(!cclassdecl->setConstantDocumentation(last_class_constant_name, sv))
                              {
                                 // Doc. already set
                                 Dprintf("[---] class constant documentation already set.\n");
                                 Dclassparseerror;
                              }
                           }
                           state_index++;
                           break;
                        case 81:
                           if(tok!=TOKEN_TERM)
                           {
                              Dclassparseerror;
                           }
                           state_index=3; // continue class-parsing
                           break;

                        case 95: // ---- Parse next_mutex_name
                           if(tok!=TOKEN_VAR)
                           {
                              Dclassparseerror;
                           }
                           else
                           {
                              next_mutex_name = sv;
                              state_index++;
                           }
                           break;
                        case 96:
                           // consume '=' in "=<mutexname>="
                           if(tok!=TOKEN_ASSIGN)
                           {
                              Dclassparseerror;
                           }
                           else
                           {
                              state_index = 3; // continue class parsing
                           }
                           break;

                        case 100: // ---- function in the namespace of a class (a static method)
                           if(tok!=TOKEN_VAR)
                           {
                              Dclassparseerror;
                           }
                           b_expect_method_name=0;//[02Oct2023]
                           if(cclassdecl->findFunction(sv))
                           {
                              Dprintf("[---] class \"%s\" already has a function named \"%s\".\n", (char*)cclassdecl->name.chars, (char*)sv->chars);
                              Dclassparseerror;
                           }
                           cfunction=cclassdecl->addFunction(sv, prot_flag, b_anonymous_mutex, next_mutex_name, Dsrcloc);
                           if(NULL == cfunction)
                           {
                              Dclassparseerror;
                           }
                           b_anonymous_mutex = 0;
                           b_this_mutex = 0;
                           next_mutex_name = NULL;
                           prot_flag = TKS_PROT_NONE;
                           vartype = 0;
                           static_flag = 0;
                           formal_arg_type = YAC_TYPE_VARIANT;
                           is_array = 0;
                           is_local = 0;
                           if(ntok==TOKEN_BEGBLOCK) // function foo { ??
                           {
                              state_index=108; // consume function body
                           }
                           else
                              if((ntok==TOKEN_TELSE)||(ntok==TOKEN_RETURNS))
                                 state_index=1030;
                              else if(ntok==TOKEN_COMPILE)
                                 state_index=107;
                              else
                                 state_index++;
                           break;


                        case 101: // ---- parse formal argument list
                           if(tok!=TOKEN_BEGEXPR)
                           {
                              Dclassparseerror;
                           }
                           if(ntok==TOKEN_ENDEXPR)
                           {
                                 // ---- empty formal argument list
                              state_index=103;
                           }
                           else
                           {
                              is_local=0;
                              is_array=0;
                              b_var_ref=0;
                              state_index=110;
                           }
                           break;
                        case 103: // empty formal argument list / arglist finished
                           if(tok==TOKEN_ENDEXPR)
                           {
                              if((ntok==TOKEN_TELSE)||(ntok==TOKEN_RETURNS)) // return type declaration? function f(...) : int [;|{..}]
                              {
                                 state_index=1030;
                              }
                              else if(ntok==TOKEN_BEGBLOCK) // function implementation follows?
                              {
                                 state_index=108; // consume function body
                              }
                              else if(ntok==TOKEN_COMPILE)
                              {
                                 state_index=107; // consume "compile" then function body
                              }
                              else if(ntok==TOKEN_TERM)
                                 state_index=109; // consume ";"
                              else
                              {
                                 Dclassparseerror;
                              }
                           }
                           else
                           {
                              Dclassparseerror;
                           }
                           break;
                        case 1030:
                           // ---- parse return type declaration
                           // ---- consume "returns" resp. ":"
                           state_index++;
                           break;
                        case 1031:
                           {
                              sBool bDoBreak = 0;
                              switch(tok)
                              {
                              default:
                                 Dclassparseerror;
                                 break;
                              case TOKEN_MEMBER:
                                 // "returns ::<type>", search only default namespace
                                 Dclassdefaultnspace;
                                 /////cnspace = &Dtkscompiler->default_namespace;
                                 break;
                              case TOKEN_VAR:

                                 if(TOKEN_MEMBER == ntok) // Dclassparsenspace(1032)
                                 {
                                    Dclassfindnspace;
                                    state_index = 1032;
                                    bDoBreak = 1;
                                    break;
                                 }


                                 if(sv->compare(&tok_var))
                                 {
                                    Dassertnonspace;
                                    cfunction->return_type=YAC_TYPE_VARIANT;
                                 }
                                 else
                                 {
                                    cfunction->return_type=YAC_TYPE_OBJECT;
                                    cfunction->return_class_name.yacCopy(sv);
                                    Dfreezenspaces(cfunction->return_class_nspaces);
                                 }
                                 break;
                              case TOKEN_INT:
                                 Dassertnonspace;
                                 cfunction->return_type=YAC_TYPE_INT;
                                 break;
                              case TOKEN_FLOAT:
                                 Dassertnonspace;
                                 cfunction->return_type=YAC_TYPE_FLOAT;
                                 break;
                              case TOKEN_OBJECT:
                                 Dassertnonspace;
                                 cfunction->return_type=YAC_TYPE_OBJECT;
                                 break;
                              case TOKEN_STRING:
                                 Dassertnonspace;
                                 cfunction->return_type=YAC_TYPE_STRING;
                                 cfunction->return_template_object = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_STRING]->template_object;
                                 break;
                              }

                              if(bDoBreak)
                              {
                                 break;
                              }
                           }
                           if(ntok==TOKEN_BEGBLOCK) // function implementation follows?
                           {
                              state_index=108; // consume function body
                           }
                           else if(ntok==TOKEN_COMPILE)
                           {
                              state_index=107; // consume "compile" then function body
                           }
                           else if(ntok==TOKEN_TERM)
                              state_index=109; // consume ";"
                           else if(ntok==TOKEN_DOT) // return constraint
                              state_index=1040;
                           else
                           {
                              Dclassparseerror;
                           }
                           break;
                        case 1032: // skip '::' between <nspace> and <type>
                           state_index = 1031;
                           break;
                        case 1040: // parse return constraint
                           state_index++; // skip '.'
                           break;
                        case 1041:
                           if(tok==TOKEN_VAR)
                           {
                              if(sv->compare(&tok_notnull))
                              {
#ifdef DX_SCRIPTCONSTRAINTS
                                 cfunction->return_constraint_type=TKS_CONSTRAINTTYPE_NOTNULL;
#endif
                              }
                              else
                              {
#ifdef DX_SCRIPTCONSTRAINTS
                                 TKS_ScriptConstraint *scs=tkscript->findScriptConstraint(sv);
                                 if(scs)
                                 {
                                    cfunction->return_constraint_type=TKS_CONSTRAINTTYPE_EXPR;
                                    cfunction->return_constraint=scs;
                                 }
                                 else
                                 {
                                    Dprintf("[---] script constraint \"%s\" is undefined.\n",
                                       (char*)sv->chars);
                                    Dclassparseerror;
                                 }
#endif // DX_SCRIPTCONSTRAINTS
                              }
                           }
                           else
                           {
                              Dclassparseerror;
                           }
                           if(ntok==TOKEN_BEGBLOCK) // function implementation follows?
                           {
                              state_index=108; // consume function body
                           }
                           else if(ntok==TOKEN_COMPILE)
                           {
                              state_index=107; // consume "compile" then function body
                           }
                           else if(ntok==TOKEN_TERM)
                              state_index=109; // consume ";"
                           else
                           {
                              Dclassparseerror;
                           }
                           break;

                        case 107:
                           state_index++; // consume TOKEN_COMPILE
                           break;
                        case 108:
                           if(tok!=TOKEN_BEGBLOCK)
                           {
                              Dclassparseerror;
                           }
                           else
                           {
                                 // ---- consume function body
                              sUI num_open_brackets=1;
                              sUI ri=0;
                              sUI ti=toki+1;
                              while(num_open_brackets&&ti<num_tok)
                              {
                                 tok=stok[ri++];
                                 ti++;
                                 if(tok==TOKEN_BEGBLOCK)
                                    num_open_brackets++;
                                 else
                                    if(tok==TOKEN_ENDBLOCK)
                                       num_open_brackets--;
                                    else
                                       if(tok<=TOKEN_STRINGVAL)
                                       {
                                          ri++; // consume string index
                                          ti++;
                                       }
                              }
                              if(ti>=num_tok)
                              {
                                 Dprintf("\n[---] reaching EOF while looking for closing >>}<< of inplace function body \"%s::%s\".\n",
                                                  (char*)cclassdecl->name.chars, (char*)cfunction->name.chars);
                                 Dclassparseerror;
                              }
                              if(stok[ri]==TOKEN_TERM) // skip ';' at end of function body
                                 ri++;
                              stok+=ri;
                              toki+=ri;
                              state_index=3; // try next class member/method/function
                           }
                           break;
                        case 109:
                           state_index=3;
                           break;
                        case 110:  // ---- parse formal argument list
                           {
                              // // if(TOKEN_MUL == tok) // consume '*' in SomeClass*myarr[]
                              // // {
                              // //    if(b_var_ref)
                              // //    {
                              // //       Dparseerror;
                              // //    }
                              // //    else
                              // //    {
                              // //       b_var_ref = 1;
                              // //       break;
                              // //    }
                              // // }
                              sBool bDoBreak = 0;
                              if(NULL == cnspace)
                              {
                                 if(tok==TOKEN_LOCAL)
                                 {
                                    if(fail_local&&is_local) // catch "local local"
                                    {
                                       Dclassparseerror;
                                    }
                                    else
                                    {
                                       is_local = 1;
                                       new_local = 1;
                                       fail_local = 1;
                                    }
                                    break;
                                 }
                                 else
                                 {
                                    fail_local=0;
                                 }
                              }

                              switch(tok)
                              {
                              case TOKEN_INT:
                                 b_pointerarray = 0;  // [22Jun2020]
                                 is_array = 0;
                                 b_var_ref = 0;
                                 Dclassassertnonspace;

                                 formal_arg_type=YAC_TYPE_INT;
                                 tsv=0;
                                 break;
                              case TOKEN_FLOAT:
                                 b_pointerarray = 0;  // [22Jun2020]
                                 is_array = 0;
                                 b_var_ref = 0;
                                 Dclassassertnonspace;

                                 formal_arg_type=YAC_TYPE_FLOAT;
                                 tsv=0;
                                 break;
                              case TOKEN_OBJECT:
                                 b_pointerarray = 0;  // [22Jun2020]
                                 is_array = 0;
                                 b_var_ref = 0;
                                 Dclassassertnonoryacspace;

                                 formal_arg_type=YAC_TYPE_OBJECT;
                                 tsv=0;
                                 break;
                              case TOKEN_STRING:
                                 b_pointerarray = 0;  // [22Jun2020]
                                 is_array = 0;
                                 b_var_ref = 0;
                                 Dclassassertnonoryacspace;

                                 formal_arg_type=YAC_TYPE_STRING;
                                 tsv=0;

                                 break;
                              case TOKEN_MEMBER: // ::MyClass _arg
                                 bDoBreak = 1;
                                 Dclassdefaultnspace;
                                 break;

                              case TOKEN_VAR:
                                 b_pointerarray=0;  // [22Jun2020]
                                 is_array=0;
                                 b_var_ref=0;

                                 if(NULL == cnspace)
                                 {
                                    if(TOKEN_MEMBER == ntok) // Dclassparsenspace(116);
                                    {
                                       Dclassfindnspace;
                                       state_index = 116;
                                       bDoBreak = 1;
                                       break;
                                    }
                                 }

                                 if((ntok==TOKEN_DOT)||(ntok==TOKEN_LISTSEP)||(ntok==TOKEN_ENDEXPR)) // '.', ',' or ')'
                                 {
                                    Dclassassertnonspace;

                                    Dungettok;
                                    state_index=111;
                                    goto case110_break;
                                 }
                                 else if(sv->compare(&tok_var))
                                    {
                                       Dclassassertnonspace;

                                       formal_arg_type=YAC_TYPE_VARIANT;
                                       tsv=0;
                                    }
                                    else
                                    {
                                       if(ntok==TOKEN_BEGARRAY)
                                       {
                                          if(nntok == TOKEN_ENDARRAY)
                                          {
                                             // prevent Double[] from entering state 1120 (parse 'sv' as type name instead)
                                             tsv = sv; // remember type name
                                             formal_arg_type=YAC_TYPE_OBJECT;
                                             is_array=1;
                                          }
                                          else
                                          {
                                             Dclassassertnonspace;

                                             fun_varname=sv;//.yacCopy(sv); // remember variable name
                                             state_index = 1120;
                                             goto case110_break;
                                          }
                                       }
                                       else
                                       {
                                          tsv = sv; // remember type name
                                          formal_arg_type=YAC_TYPE_OBJECT;
                                       }
                                    }
                                    break;
                              default:
                                 Dclassparseerror;
                                 break;
                              }

                              if(bDoBreak)
                              {
                                 break;
                              }

                              is_local = new_local; // Reset local flag if new formal arg type is found
                              new_local = 0;

                              if(ntok==TOKEN_BEGARRAY)  // String[]
                              {
                                 // // if(b_var_ref)
                                 // // {
                                 // //    Dclassparseerror;
                                 // // }
                                 state_index=112;  // consume []
                              }
                              else
                                 state_index++;   // String myarg, SomeClass *myarg ([] must follow)

                           }
                        case110_break:
                           break; // end function

                        case 1110: // consume first ']' in Double*[]
                           if(TOKEN_ENDARRAY != tok)
                           {
                              Dclassparseerror;
                           }
                           state_index = 111;
                           break;

                        case 1111: // consume first '[' in Double*[]
                           if(TOKEN_BEGARRAY != tok)
                           {
                              Dclassparseerror;
                           }
                           state_index = 1110;
                           break;

                        case 111:
                           if(TOKEN_MUL == tok) // consume '*' in String[]*myarr[] or MyClass*myarr[]
                           {
                              if(b_var_ref)
                              {
                                 // MyClass**myarr
                                 Dclassparseerror;
                              }
                              else if(TOKEN_BEGARRAY == ntok)
                              {
                                 if(!is_array)
                                 {
                                    // String*[], Double*[], ..
                                    b_pointerarray = 1;
                                    state_index=1111;
                                    break;
                                 }
                                 else
                                 {
                                    Dclassparseerror;
                                 }
                              }
                              else
                              {
                                 b_var_ref = 1;
                                 break;
                              }
                           }
                           if(tok!=TOKEN_VAR)
                           {
                              Dclassparseerror; // expected formal argument name
                           }
                           if(ntok==TOKEN_BEGARRAY)
                           {
                              if(is_array)
                              {
                                 // String[] arg[]
                                 is_array = 0;
                                 b_pointerarray = 1;
                                 /////Dclassparseerror;
                              }
                              else
                              {
                                 is_array=1;
                                 state_index=1111;
                              }
                              // else: String*arg[], MyClass*arg[] => create PointerArray
                           }
                           fun_varname=sv;//.yacCopy(sv);
                           // run into case 1122
                        case 1122:
                           {
                              TKS_FreezedNamespaceArray nsa;
                              TKS_Namespace *save_cnspace = cnspace;
                              if(NULL != tsv) // "Object" pointer type?
                              {
                                 Dfreezenspaces(nsa); // xxx this will reset cnspace to NULL, what about  MyClass a,b,c style arglists?
                              }
                              else
                              {
                                 if( (NULL == cnspace) || (cnspace == &Dtkscompiler->yac_namespace) ) // yac::Object
                                 {
                                    nsa = NULL;
                                 }
                                 else
                                 {
                                    Dclassparseerror;
                                 }
                              }

                              cfunction->addFormalArgument(fun_varname,
                                                           formal_arg_type,
                                                           tsv/*class_name*/,
                                                           b_pointerarray,
                                                           is_array,
                                                           is_local,
                                                           nsa,
                                                           save_cnspace
                                                           );
                              cnspace = NULL;
                              b_var_ref = 0;
                           }

                           if(ntok==TOKEN_BEGARRAY) // [ array var ?
                              state_index=114;
                           else if(ntok==TOKEN_DOT) // .constraint ?
                              state_index=160;
                           else if(ntok==TOKEN_LISTSEP) // next argument?
                              state_index=120; // first consume ','
                           else
                              state_index=103; // expect ')'
                           break;
                        case 112: // consume String[] name
                           is_array=1;
                           if(tok!=TOKEN_BEGARRAY)
                           {
                              Dclassparseerror;
                           }
                           state_index++;
                           break;
                        case 113:
                           if(tok!=TOKEN_ENDARRAY)
                           {
                              Dclassparseerror;
                           }
                           state_index=111; // parse name
                           break;
                        case 116: // consume '::' between namespace and classname
                           state_index = 110;
                           break;

                        case 160: // constraints
                           if(tok!=TOKEN_DOT)
                           {
                              Dparseerror;
                           }
                           else
                           {
                              state_index++;
                           }
                           break;
                        case 161:
                           if(tok!=TOKEN_VAR)
                           {
                              Dparseerror;
                           }
                           else
                           {
                              if( ! sv->compare(&tok_notnull))
                              {
#ifdef DX_SCRIPTCONSTRAINTS
                                 TKS_ScriptConstraint *scs=tkscript->findScriptConstraint(sv);
                                 if(scs)
                                 {
                                    cfunction->setLastArgConstraint(scs);
#endif // DX_SCRIPTCONSTRAINTS
                                    if(ntok==TOKEN_LISTSEP) // next argument?
                                       state_index=120; // first consume ','
                                    else
                                       state_index=103; // expect ')'
#ifdef DX_SCRIPTCONSTRAINTS
                                 }
                                 else
                                 {
                                    Dprintf("[---] script constraint \"%s\" is undefined.\n",
                                       (char*)sv->chars);
                                    Dparseerror;
                                 }
#endif // DX_SCRIPTCONSTRAINTS
                              }
                              else
                              {
#ifdef DX_SCRIPTCONSTRAINTS
                                 cfunction->setLastArgNotNullConstraint();
#endif // DX_SCRIPTCONSTRAINTS
                                 if(ntok==TOKEN_LISTSEP) // next argument?
                                    state_index=120; // first consume ','
                                 else
                                    state_index=103; // expect ')'
                              }
                           }
                           break;

                        case 1120: // consume name[]  (without type)
                           // tok==TOKEN_BEGARRAY
                           is_array=1;
                           state_index=1122;
                           break;
                        /*case 1121:
                           if(tok!=TOKEN_ENDARRAY)
                           {
                              Dclassparseerror;
                           }
                           state_index=1122;
                           break;*/

                        case 114: // consume String name[]
                           if(tok!=TOKEN_BEGARRAY)
                           {
                              Dclassparseerror;
                           }
                           state_index++;
                           break;
                        case 115:
                           if(tok!=TOKEN_ENDARRAY)
                           {
                              Dclassparseerror;
                           }
                           if(ntok==TOKEN_LISTSEP) // next argument?
                              state_index=120;
                           else
                           {
                              /*if(ntok!=TOKEN_ENDEXPR)
                              {
                                 Dclassparseerror;
                              }
                              else
                              {*/
                                 state_index=103; // consume ')'
                              /*}*/
                           }
                           break;
                        case 120: // consume ','
                           //////////////////is_local=0;
                           is_array=0;
                           state_index=110; // parse next argument
                           break;
                              // ---- end parse static method


                        case 200:
                              // ---- parse class constant
                           if(tok==TOKEN_VAR)
                           {
                              if(tok_exception.compare(sv))
                              {
                                 state_index=300; // parse class exception
                              }
                              else
                              {
                                 Dclassparseerror;
                              }
                           }
                           else
                           {
                              tempi=tok; // remember type
                              state_index++;
                           }
                           break;
                        case 201:
                           if(tok!=TOKEN_VAR)
                           {
                              Dclassparseerror;
                           }
                           switch(tempi)
                           {
                              case TOKEN_INT:
                                 co=cclassdecl->addIntConstant(sv);
                                 break;
                              case TOKEN_FLOAT:
                                 co=cclassdecl->addFloatConstant(sv);
                                 break;
                              case TOKEN_OBJECT:
                                 co=cclassdecl->addObjectConstant(sv);
                                 break;
                              case TOKEN_STRING:
                                 co=cclassdecl->addStringConstant(sv);
                                 break;
                              default:
                                 Dprintf("[---] expected int, float, Object or String type for constant \"%s::%s\".\n",
                                                  (char*)cclassdecl->name.chars,
                                                  (char*)sv->chars);
                                 Dclassparseerror;
                                 break;
                           }
                           last_class_constant_name = sv;
                           state_index++;
                           break;
                        case 202:
                           if(tok!=TOKEN_ASSIGN)
                           {
                              if((TOKEN_TERM == tok) && (YAC_TYPE_STRING == co->type)) // (note) 20120312
                              {
                                 // define String ACTION_TEXTENTERED;
                                 //  (constname == constval)
                                 co->value.string_val->visit(&co->name);
                                 co->flags &= ~TKS_CachedObject::ISCLASSCONSTUNINITIALIZED;
                                 state_index=3; // parse next class member/method/function
                                 break;
                              }
                              else
                              {
                                 Dclassparseerror;
                              }
                           }
                           // run into case 203
                        case 203:
                           ////else
                           {
                                 // ---- seek statement terminator ';'
                                 // ---- skip forward until next occurence of ";" or "{"
                              sUI ri=0;
                              sUI ti=toki+1;
                              while(ti<num_tok)
                              {
                                 tok=stok[ri++];
                                 ti++;
                                 if(tok==TOKEN_TERM) // just forward decl
                                 {
                                    state_index=3; // parse next class member/method/function
                                    break;
                                 }
                                 else
                                    if(
                                       (tok==TOKEN_ENDBLOCK)||
                                       (tok==TOKEN_DEFINE)||
                                       (tok==TOKEN_FUNCTION)||
                                       (tok==TOKEN_METHOD)||
                                       (tok==TOKEN_PUBLIC)||
                                       (tok==TOKEN_PRIVATE)||
                                       (tok==TOKEN_PROTECTED)||
                                       (tok==TOKEN_MODULE)
                                       )
                                    {
                                       tkscript->printf("\n[---] reaching EOF while looking for ';' after class constant definition.\n");
                                       Dclassparseerror;
                                    }
                                    else
                                       if(tok<=TOKEN_STRINGVAL)
                                       {
                                          ri++; // consume string index
                                          ti++;
                                       }
                              }
                              if(ti>=num_tok)
                              {
                                 Dprintf("\n[---] reaching EOF while looking for ';' after class constant definition.\n");
                                 Dclassparseerror;
                              }
                              stok+=ri;
                              toki+=ri;
                           }
                           break;

                        case 300: // define exception TestError [:,extends [SomeClass::,::]BaseError]
                           if(TOKEN_VAR != tok) // exception exception name
                           {
                              Dclassparseerror;
                           }
                           if(tkscript->getExceptionTypeByName(cclassdecl, sv) != NULL)
                           {
                              Dprintf("[---] class exception type \"%s\" already defined.\n", (char*)sv->chars);
                              Dclassparseerror;
                           }
                           else
                           {
                              if( (ntok==TOKEN_TELSE) || (ntok==TOKEN_EXTENDS) )
                              {
                                 tsv = sv; // remember exception name
                                 state_index=302;
                              }
                              else
                              {
                                 TKS_ExceptionType *et = new TKS_ExceptionType(tkscript->getExceptionTypeById(TKS_EXCEPTION_ERROR), sv);
                                 et->parent_class = cclassdecl;
                                 if(!tkscript->addExceptionType(et))
                                 {
                                    Dprintf("[---] failed to add class exception type.\n");
                                    delete et;
                                    Dclassparseerror;
                                 }
                                 else
                                 {
                                    state_index++; // consume ';'
                                 }
                              }
                           }
                           break;
                        case 301: // define exception TestError ; finished
                           if(tok!=TOKEN_TERM)
                           {
                              Dclassparseerror;
                           }
                           state_index=3;
                           break;
                        case 302: // parse define exception TestError extends..
                           // consume ':' or 'extends'
                           state_index++;
                           break;
                        case 303:
                           if(TOKEN_MEMBER == tok) // '::GlobalBaseError'
                           {
                              state_index=310;
                           }
                           else
                           {
                              if(TOKEN_VAR != tok) // expect base exception name
                              {
                                 Dclassparseerror;
                              }
                              else
                              {
                                 if(TOKEN_MEMBER == ntok) // 'SomeClass::SomeBaseError'
                                 {
                                    if(NULL == cnspace) // Dclassparsenspace(306)
                                    {
                                       cnspace = tkscript->findNamespace(sv);
                                       if(NULL != cnspace) // extends somenamespace::SomeClass::SomeBaseError
                                       {
                                          if(TOKEN_MEMBER != nnntok)
                                          {
                                             Dclassparseerror;
                                          }
                                          else
                                          {
                                             state_index = 306;
                                             break;
                                          }
                                       }
                                    }
                                    if(NULL == cnspace)
                                    {
                                       temp_classdecl = Dtkscompiler->findClassDeclNamespaceList(sv, active_namespaces);
                                    }
                                    else
                                    {
                                       temp_classdecl = Dtkscompiler->findClassDeclNamespace(sv, cnspace);
                                    }
                                    if(temp_classdecl == NULL)
                                    {
#ifdef TKS_DCON
                                       if(NULL != cnspace)
                                       {
                                          Dprintf("[---] class \"%s::%s\" not found while parsing base exception type.\n",
                                             (char*)cnspace->name.chars,
                                             (char*)sv->chars
                                             );
                                       }
                                       else
                                       {
                                          Dprintf("[---] class \"%s\" not found while parsing base exception type.\n", (char*)sv->chars);
                                       }
#endif
                                       Dclassparseerror;
                                    }
                                    else
                                    {
                                       cnspace = NULL;
                                       state_index=320;
                                    }
                                 }
                                 else
                                 {
                                    state_index++; // 'SomeBaseError'
                                 }
                              }
                           }
                           break;
                        case 304:
                           {
                              // Resolve base exception type, first search class exception
                              temp_baseexception = tkscript->getExceptionTypeByName(cclassdecl, sv) ;
                              if(temp_baseexception==NULL)
                              {
                                 // Try global exception
                                 temp_baseexception = tkscript->getExceptionTypeByName(NULL, sv);
                              }
                              if(temp_baseexception==NULL)
                              {
                                 Dprintf("[---] unknown base exception \"%s\".\n", (char*)sv->chars);
                                 Dclassparseerror;
                              }
                           }
                           // run into case 305
                        case 305: //(
                           if(tok!=TOKEN_TERM) // define exception MyException extends BaseException ; finished
                           {
                              Dclassparseerror;
                           }
                           else
                           {
                              TKS_ExceptionType *et = new TKS_ExceptionType(temp_baseexception, tsv);
                              et->parent_class = cclassdecl;
                              if(!tkscript->addExceptionType(et))
                              {
                                 Dprintf("[---] failed to add class exception type.\n");
                                 delete et;
                                 Dclassparseerror;
                              }
                              else
                              {
                                 state_index=3; // continue class parsing
                              }
                           }
                           break;
                        case 306: // parse '::' between namespace and exception classname (extends)
                           state_index = 303;
                           break;
                        case 310: // '::MyGlobalBaseException' or '::MyClass::MyException'
                           if(tok!=TOKEN_VAR)
                           {
                              Dclassparseerror;
                           }
                           else
                           {
                              if(TOKEN_MEMBER == ntok) // ::MyClass::MyBaseException ?
                              {
                                 // sv must be MyClass name (in default namespace)
                                 temp_classdecl = Dtkscompiler->findClassDeclNamespace(sv, NULL);
                                 if(NULL == temp_classdecl)
                                 {
                                    Dprintf("[---] class \"%s\" not found while parsing base exception type.\n", (char*)sv->chars);
                                    Dclassparseerror;
                                 }
                                 else
                                 {
                                    state_index=320;
                                 }
                              }
                              else
                              {
                                 temp_baseexception = tkscript->getExceptionTypeByName(NULL, sv);
                                 if(temp_baseexception==NULL)
                                 {
                                    Dprintf("[---] unknown base exception \"%s\".\n", (char*)sv->chars);
                                    Dclassparseerror;
                                 }
                                 else
                                 {
                                    state_index=305; // parse ';' and add new exception type
                                 }
                              }
                           }
                           break;
                        case 311: // consume '::' after ::MyClass::MyBaseException
                           state_index = 310;
                           break;
                        case 320: // consume '::' in 'MyClass::MyBaseException'
                           state_index++;
                           break;
                        case 321:
                           if(tok!=TOKEN_VAR)
                           {
                              Dclassparseerror;
                           }
                           else
                           {
                              temp_baseexception = tkscript->getExceptionTypeByName(temp_classdecl, sv);
                              if(temp_baseexception==NULL)
                              {
                                 Dprintf("[---] unknown base exception \"%s::%s::%s\".\n",
                                    (char*)temp_classdecl->nspace->name.chars, // always default namespace..
                                    (char*)temp_classdecl->name.chars, (char*)sv->chars);
                                 Dclassparseerror;
                              }
                              else
                              {
                                 state_index=305; // parse ';' and add new exception type
                              }
                           }
                           break;


                     } // ---- ST_CLASSDECL::switch state_index
                     break;
               }
            }
         }

   useDefaultNamespace();  // [05Sep2025]

         return Dtkscompiler->parse_error?0:1;
}


void TKS_CachedScript::parseError(sU16 _line/*, sU16 _state, sU16 _state_index*/, sUI _toki, sU16 _tok) {
   Dtkscompiler->parse_error = YAC_TRUE;
   static const sUI tokRange = 23u;   // (todo) hardcoded error output token range  (+-23 tokens)
   sUI st = (_toki > tokRange) ? (_toki - tokRange):0;
   if(st > 0u)
   {
      if(compiled_script->tokens.elements[st-1u] <= TOKEN_STRINGVAL)
         st--;
      if(st > 0u)
         if(compiled_script->tokens.elements[st-1u] <= TOKEN_STRINGVAL)
            st--;
   }
   sUI en =
      ( (sUI(_toki)+tokRange) < compiled_script->tokens.num_elements)
      ? (sUI(_toki)+tokRange)
      : compiled_script->tokens.num_elements
      ;
#ifdef TKS_DCON
   const sChar *tokName = tks_get_token_name(_tok);
   Dprintf("[---] parse error in module \"%s\" (file \"%s\")\n[---]  line %i, near token %i/%i *** %s ***:\n",
           (char*) name.chars,
           ((src_name.isBlank())?"<temp>":(char*)src_name.chars),
           _line/*+1*/,
           _toki, compiled_script->tokens.num_elements, tokName
           );
   compiled_script->printTokens(st, en, _toki, linenr_offset);
#endif

      //yac_host->printf("popnode.node=%08x first_stat=%08x\n", (sUI)popnode.node, (sUI)first_stat);
   if(popnode.node)
      if(popnode.node->getID()!=PTN_FUNCTION)
         delete popnode.node;
   delete first_stat;

   unwindStateStack();
}

void TKS_CachedScript::unwindStateStack(void) {
   //
   // unwind sequence stack and delete unbound statement sequences
   //
   //  This fxn is called after a parse error occured.
   //
   //  Note: This implementation is (probably) NOT complete.
   //         - a proper cleanup behaviour must be implemented for ALL states that store new objects
   //           in the stateentry "param" fields.
   //           In case of an error, these "unbound" new objects need to be deleted to avoid mem leaks.
   //
   //
   //Dprintf("xxx unwindstatestack: state=%d/%s\n", state, debug__get_seq_name_by_id(state));
   {
      sSI i = stp;
      sSI pState = -1;
      while(i < TKS_MAX_PARSER_STACKDEPTH)
      {
         sSI cState = state_stack[i].state;
         //Dprintf("xxx unwindstatestack:   state_stack[%d].state=%d/%s\n", i, cState, debug__get_seq_name_by_id(cState));
         if(ST_SWITCHCASE != cState) // Note: switchcase manages statement sequence by itself
         {
            switch(pState)
            {
            case ST_SEQ:
               {
                  // Delete first statement of (partial) statement sequence
                  PTN_Statement *s = state_stack[i].param[1].node.statement;
                  TKS_DELETE_SAFE(s);
               }
               break;

            case ST_SWITCHCASE:
               {
                  // Delete first statement of (partial) switchcase statement sequence
                  PTN_Statement *s = state_stack[i].param[2].node.statement;
                  if(NULL != s)
                  {
                     while(s->prev) s = s->prev;
                     TKS_DELETE_SAFE(s);
                  }

                  PTN_Expr *e = state_stack[i].param[1].node.expr;
                  if(NULL != e)
                  {
                     TKS_DELETE_SAFE(e);
                  }
               }
               break;

            case ST_SWITCH:
               {
                  // Delete partially initialized PTN_Switch object (and associated switchcases)
                  TKS_DELETE_SAFE(state_stack[i].param[0].node.switchstatement);
               }
               break;

            case ST_TRYSTAT:
               {
                  TKS_DELETE_SAFE(state_stack[i].param[0].node.trycatchfinallystat);
               }
               break;
            }
         }
         pState = cState; // next parent state
         i++;
      }
   }
}

void TKS_CachedScript::setTNode(PTN_Node *_node) {
   tnode.node = _node;
   tnode.node->src_loc = Dsrcloc;
   parser_tree_num_nodes++;
}

void TKS_CachedScript::setPNode(PTN_Node *_node) {
   popnode.node = _node;
   popnode.node->src_loc = Dsrcloc;
   parser_tree_num_nodes++;
}

void TKS_CachedScript::spawnClassConstantExpr(TKS_CachedObject *_co) {
   PTN_Node *nn;
   PTN_NEW_NODE(nn, PTN_ConstValReference) (_co);
   setPNode(nn);
}

void TKS_CachedScript::pushBreak(PTN_BreakableStatement *_bst) {
   if(breaki > 0)
   {
      break_stack[--breaki] = _bst;
      pbreak = _bst;
   }
   else
   {
      pbreak = NULL;
      Dprintf("[---] break stack overflow.\n");
      // xxx abort with parse error
   }
}

void TKS_CachedScript::popBreak(void) {
   if(TKS_BREAK_STACK_SIZE != breaki)
   {
      if(++breaki == TKS_BREAK_STACK_SIZE)
         pbreak = NULL;
      else
         pbreak = break_stack[breaki];
   }
   else
   {
      pbreak = NULL;
      Dprintf("[---] break stack underflow.\n");
      // xxx abort with parse error
   }
}

void TKS_CachedScript::pushExceptionVarname(YAC_String *_s) {
   if(trycatch_stacki>0)
   {
      current_exception_varname_stack[--trycatch_stacki] = _s;
      current_exception_varname = _s;
   }
   else
   {
      current_exception_varname = NULL;
      Dprintf("[---] exception varname stack overflow.\n");
      // xxx abort with parse error
   }
}

void TKS_CachedScript::popExceptionVarname(void) {
   if(trycatch_stacki != TKS_TRYCATCHSTACKDEPTH)
   {
      if(++trycatch_stacki == TKS_TRYCATCHSTACKDEPTH)
      {
         current_exception_varname = NULL;
      }
      else
      {
         current_exception_varname = current_exception_varname_stack[trycatch_stacki];
      }
   }
   else
   {
      current_exception_varname = NULL;
      Dprintf("[---] exception varname stack underflow.\n");
      // xxx abort with parse error
   }
}

sBool TKS_CachedScript::compile(void) {
   cclassdecl = NULL;
   cfunction  = NULL;
   tkscript->ptn_error_code=PTNERR_NOERROR;
   sU16 *stok = compiled_script->tokens.elements;
   sU32 *lineptr = compiled_script->lines.elements;
   first_stat = NULL;
   if(stok && lineptr)
   {
      sU16 last_state = ST_OFMIND;
      stp = TKS_MAX_PARSER_STACKDEPTH;
      state = ST_SEQ;
      state_index = 1;
      sUI num_tok = compiled_script->tokens.num_elements;
      sUI toki = 0;
      sU16 tok = TOKEN_INT;
      sU16 ntok = TOKEN_INT; // next token
      sU16 nntok = TOKEN_INT; // next next token
      sU16 nnntok = TOKEN_INT; // next next next token
      sU16 last_tok = TOKEN_BLOCKCOMMENT;
      linenr = -sS32(linenr_offset);
      VMCore *vmcore = NULL;

      PTN_Node *nn;
      PTN_NEW_NODE(nn, PTN_EmptyStatement) ();
      setTNode(nn);

      breaki=TKS_BREAK_STACK_SIZE;
      break_stack[TKS_BREAK_STACK_SIZE]=pbreak=0;

      first_stat = tnode.statement;
      Dpushstate;
      Dsetparamn(0,tnode.statement);
      Dsetparamn(1,tnode.statement);
      Dpushstate;
      state_index = 0u;
      popnode.node = NULL;
      tnode.node = NULL;
      TKS_CachedObject *co = NULL;
      TKS_CachedObject *co2 = NULL;
      YAC_String cvarname;
      PTN_Statement *last_vardecl = NULL; // for vardecl listen
      sBool template_vardecl = YAC_FALSE; // 1=use template objects instead of instances in argument vardecl lists
      PTN_ArgVarList *curr_argvarlist = NULL;
      sU8 vardecl_tag = 0u;
      sUI static_flag = 0u;
      sBool nextisdestructorname=0; // prepend '~'
      YAC_String ctempname;  // temporary class name (with ~)
      YAC_String *ctempname2 = NULL;
      sBool b_expect_method_name = YAC_FALSE;
#ifdef DX_SCRIPTCONSTRAINTS
      TKS_ScriptConstraint *current_constraint = NULL;
#endif // DX_SCRIPTCONSTRAINTS
      PTN_TryCatchFinally *current_try = NULL;
      sBool b_force_this = YAC_FALSE; // used for this.x =, this.x+=, this.x<=null, this.x++
      sBool b_force_this_dot = YAC_FALSE; // used for this.x =, this.x+=, this.x<=null, this.x++
      TKS_Namespace *cnspace = NULL; // must be reset to NULL after reading/using

      trycatch_stacki = TKS_TRYCATCHSTACKDEPTH ;
      current_exception_varname = NULL;
      for(int i = 0; i < trycatch_stacki; i++)
         current_exception_varname_stack[i] = NULL;

      YAC_String *sv = NULL;
      YAC_String sv_object; sv_object.visit("Object");
      YAC_String *tsv = NULL;

      sBool b_var_ref = 0; // '*' used after object class type => do not instantiate class (vardecls)
      sBool is_array = 0;  // for String[]*array_of_stringarrays[], int[]*array_of_intarrays[] style type decls
      sBool b_pointerarray = 0;

      tkscript->trace_enabled = 1;

      useDefaultNamespace();  // [05Sep2025]

      for(; toki < num_tok || stp < TKS_MAX_PARSER_STACKDEPTH; toki++)
      {
         if(state != last_state)
            last_state = state;

         if(toki < num_tok)
         {
            tok = *stok++;
            if(tok == TOKEN_NOP)
               continue;
            if( (toki+1) < num_tok )
            {
               if(tok <= TOKEN_STRINGVAL)
               {
                  tok = TOKEN_VAR;
                  ntok = *stok++; // get stringvalue reference
                  if(ntok >= compiled_script->strings.num_elements)
                  {
                     Dprintf("\n[---] internal error: accessing string %i/%i\n",
                             compiled_script->strings.num_elements,
                             compiled_script->strings.max_elements
                             );
                     Dparseerror;
                  }
                  sv = &compiled_script->strings.elements[ntok]; // xxx boundary check
                  toki++;
               }
            }
            else
            {
               ntok = TOKEN_INT; // no next token, unset
               nntok = TOKEN_INT; // no next next token, unset
               nnntok = TOKEN_INT;
            }

            if( (toki+1) < num_tok )
            {
               ntok = stok[0];
               if(ntok <= TOKEN_STRINGVAL)
               {
                  ntok = TOKEN_VAR;
                  if( (toki+3) < num_tok)
                  {
                     nntok = stok[2];
                     if(nntok <= TOKEN_STRINGVAL)
                     {
                        nntok = TOKEN_VAR;
                        if( (toki+5) < num_tok)
                           nnntok = stok[4];
                        else
                           nnntok = TOKEN_INT;
                     }
                     else
                     {
                        if((toki+4) < num_tok)
                           nnntok = stok[3];
                        else
                           nnntok = TOKEN_INT;
                     }
                  }
                  else
                  {
                     nntok = TOKEN_INT;
                     nnntok = TOKEN_INT;
                  }
               }
               else
               {
                  if( (toki+2) < num_tok )
                  {
                     nntok = stok[1];
                     if(nntok <= TOKEN_STRINGVAL)
                     {
                        nntok = TOKEN_VAR;
                        if((toki+4) < num_tok)
                           nnntok = stok[3];
                        else
                           nnntok = TOKEN_INT;
                     }
                     else
                     {
                        if( (toki+3) < num_tok)
                           nnntok = stok[2];
                        else
                           nnntok = TOKEN_INT;
                     }
                  }
                  else
                  {
                     nntok = TOKEN_INT;
                     nnntok = TOKEN_INT;
                  }
               }
            }
            else
            {
               ntok = TOKEN_INT; // unset next token
               nntok = TOKEN_INT;
               nnntok = TOKEN_INT;
            }

            while(toki >= *lineptr)
            {
               linenr++;
               lineptr++;
            }
            // if token is a stringval, sv points to that string

            if(tok != last_tok)
            {
               last_tok = tok;
               if(ntok == tok)
                  last_tok = TOKEN_BLOCKCOMMENT;
            }
         } // toki<num_tok

         switch(state)
         {
            case ST_SEQ:
               switch(state_index)
               {
                  case 0:
                     if(tok==TOKEN_BEGBLOCK)
                     {
                        if(ntok==TOKEN_ENDBLOCK) // empty { } block
                        {
                           state_index=3;
                        }
                        else
                        {
                           ////Dprintf("xxx unwindstatestack: set first statement stp=%d param state=%d/%s\n", stp, state, debug__get_seq_name_by_id(state));
                           PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                           setTNode(nn);
                           Dsetparamn(0,tnode.statement);
                           Dsetparamn(1,tnode.statement);

                           state_index++;
                           Dpushstate;
                           last_state=ST_OFMIND;
                           state=ST_SEQ;
                           state_index=0;
                        }
                     }
                     else
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SSEQ;
                        state_index=0;
                     }
                     break;

                  case 3:
                     PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                     setPNode(nn); // empty block

                     popState();
                     break;

                  case 1: // <SSEQ> finished
                     Dungettok;
                     popState();
                     break;

                  case 2: // looping <SEQ>
                     if(Dgetparami(0)&&popnode.statement) // previous statement?
                     {
                        // ---- double link statement list ----
                        tnode=Dgetparamn(0);
                        PTN_Statement *tst=popnode.statement;
                        //if((tst->getID()!=PTN_FUNCTION)&&(tst->getID()!=PTN_FUNCTIONBODY))
                        while(tst->prev)
                           tst=tst->prev;
                        tst->prev=tnode.statement;
                        tnode.statement->next=tst;
                        //tnode.statement->next=popnode.statement;
                        while(popnode.statement->next) // new 19Sep09 (correctly link multiple {}, {} blocks)
                        {
                           popnode.statement = popnode.statement->next;
                        }
                        Dsetparamn(0, popnode.statement);
                        popnode.statement = NULL;
                     }
                     if(tok==TOKEN_ENDBLOCK)
                     {
                        // sequence <SEQ> finished
                        popnode=Dgetparamn(1); // first statement
                        ////popnode.statement->genCallList(); // removed 19Sep09
                        popState();
                     }
                     else
                     {
                        if((toki+1)>/*=*/num_tok) // xxx 21Nov2007
                        {
                           popnode=Dgetparamn(1); // first statement
                           ////popnode.statement->genCallList(); // removed 19Sep09
                           popState(); // global exit
                        }
                        else
                        {
                           // next statement
                           Dungettok;
                           state_index=1; // loop <SEQ> until "}"
                           Dpushstate;
                           last_state=ST_OFMIND;
                           state=ST_SEQ;
                           state_index=0;
                        }
                     }
                     break;
                  default:
                     Dparseerror;
               }
               if(stp==TKS_MAX_PARSER_STACKDEPTH)
               {
                  parser_tree=first_stat;
                  parser_tree->genCallList();

                  sBool r = parser_tree->semanticCheckAll();
                  if(!r)
                  {
#ifdef TKS_DCON
                     Dprintf("[---] semantic check failed in module \"%s\".\n", (char*)name.chars);
                     ////if(tkscript->ptn_error_code)
                     {
                        //Dprintf("\n[---] CSQ: semantic check failed with %i \"%s\"\n",
                        //   tkscript->ptn_error_code,
                        //   PTN_Node::GetErrorByCode(tkscript->ptn_error_code));
                     }
#endif
                  }
                  // "semantic" check passed
                  return r;
               }
               break;

            case ST_SSEQ: // single statement with TERM token ";"
               switch(state_index)
               {
                  case 0:
                  {
                     Dungettok;
                     Dpushstate;
                     state=ST_SSEQSTAT;
                     state_index=0;
                  }
                  break;
                  case 1:
                     if(popnode.statement)
                     { // ---- does the last statement require a ';' ??
                        sUI pid=popnode.node->getID();
                        if(
                           pid==PTN_FOR             ||
                           pid==PTN_IFELSE          ||
                           pid==PTN_WHILE           ||
                           pid==PTN_DOWHILE         ||
                           pid==PTN_SWITCH          ||
                           pid==PTN_LOOP            ||
                           pid==PTN_FUNCTIONBODY    ||
                           pid==PTN_FOREACH         ||
                           pid==PTN_DEFINE          ||
                           pid==PTN_CLASSDECL       ||
                           pid==PTN_CLASSMETHODBODY ||
                           pid==PTN_TRACESTAT       ||
                           pid==PTN_PREPARE         ||
                           pid==PTN_TRYCATCHFINALLY ||
                           pid==PTN_EMPTY
                           )
                        {
                           Dungettok;
                           popState();
                        }
                        else
                        {
                           if(TOKEN_TERM == tok)
                           {
                              popState();
                           }
                           else
                           {
                              Dprintf("[---] missing >>>;<<< terminator after last statement.\n");
                              Dparseerror;
                           }
                        }
                     }
                     else
                     {
                        popState(); // skipping PTN_VarDecl
                     }
                     break;
               }
               break;


            case ST_MUTEXDECL:
               switch(state_index) // =mymutex=
               {
                  case 1:
                     if( (tok!=TOKEN_VAR) || (ntok!=TOKEN_ASSIGN) )
                     {
                        Dparseerror;
                     }
                     else
                     {
                        // Create new named mutex
#ifdef TKS_MT
                        TKS_NamedMutex *mtx = tkscript->findNamedMutex(sv);
                        if(mtx == NULL)
                        {
                           mtx = tkscript->createNamedMutex(sv);
                        }
#endif // TKS_MT
                        state_index++;
                     }
                     break;
                  case 2:
                     // consume "="
                     PTN_NEW_NODE(nn, PTN_EmptyStatementNoTerm)();
                     setPNode(nn);
                     popState();
                     break;
               }
               break;

            case ST_EXPLAIN: // module or function/method documentation statement
               switch(state_index)
               {
                  case 1:
                     if(tok!=TOKEN_VAR) // expect documentation string
                     {
                        Dparseerror;
                     }
                     if(cfunction)
                     {
                        if(! cfunction->documentation.isBlank() )
                        {
                           Dprintf("[---] function/method documentation text already set.\n");
                           Dparseerror;
                        }
                        cfunction->documentation.yacCopy(sv);
                     }
                     else
                     {
                        if(! documentation.isBlank() )
                        {
                           Dprintf("[---] module documentation text already set.\n");
                           Dparseerror;
                        }
                        documentation.yacCopy(sv);
                     }
                     state_index++;
                     break;
                  case 2:
                     if(tok!=TOKEN_TERM)
                     {
                        Dparseerror;
                     }
                     Dungettok;
                     PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                     setPNode(nn);
                     popState();
                     break;
               }
               break;

            case ST_THROWSTAT:
               switch(state_index)
               {
                  case 1:

                     if( (NULL == cnspace) && (TOKEN_MEMBER == tok) )
                     {
                        if(TOKEN_MEMBER==nntok)
                        {
                           // throw ::MyGlobalClass::MyException
                           cnspace = &Dtkscompiler->default_namespace; // Ddefaultnspace
                           // loop current substate
                           break;
                        }
                        else
                        {
                           // throw ::MyGlobalException [<msgexpr>] ;
                           Dsetparamn(3, NULL); // default= no msgexpr
                           state_index=20;
                        }
                     }
                     else
                     {
                        if(TOKEN_VAR != tok)
                        {
                           Dparseerror;
                        }
                        else
                        {
                           if(TOKEN_MEMBER == ntok) // '::'
                           {
                              if(NULL == cnspace) // Dparsenspace(3)
                              {
                                 cnspace = tkscript->findNamespace(sv);
                                 if(NULL != cnspace) // mynamespace::MyClass::myexceptionname
                                 {
                                    if(TOKEN_MEMBER != nnntok) // do not allow mynamespace::myexceptionname
                                    {
                                       Dparseerror;
                                    }
                                    state_index = 3;
                                    break;
                                 }
                                 else if(TOKEN_MEMBER == nnntok) // addressing namespace which does not exist
                                 {
                                    Dprintf("[---] namespace \"%s\" does not exist.\n",
                                            (char*) sv->chars
                                            );
                                    Dparseerror;
                                 }
                              }
                              // throw MyClass::MyException [<msgexpr>] ;
                              Dsetparams(0, sv); // remember class name
                              Dsetparamn(3, NULL); // default= no msgexpr
                              state_index=10;
                           }
                           else
                           {
                              // throw <MyGlobalException> [<msg_expr>] ;
                              PTN_NEW_NODE(nn, PTN_Throw)();
                              setPNode(nn);
                              if( (!sv->getQuotFlag()) && sv->compare(current_exception_varname))
                              {
                                 popnode.throwstat->exception_type = NULL; // re-throw current exception
                              }
                              else
                              {
                                 if(cclassdecl != NULL)
                                 {
                                    // Try to find exception type either in this or in one of the ancestor classes
                                    TKS_ExceptionType *et = tkscript->getExceptionTypeByName(cclassdecl, sv);
                                    if(et == NULL)
                                    {
                                       // Must be a global exception type
                                       et = tkscript->lazyGetExceptionTypeByName(NULL, Dsrcloc, sv);
                                    }
                                    // else Use class specific exception type
                                    popnode.throwstat->exception_type = et;
                                 }
                                 else
                                 {
                                    // Get global exception type
                                    popnode.throwstat->exception_type = tkscript->lazyGetExceptionTypeByName(NULL, Dsrcloc, sv);
                                 }
                              }
                              if(ntok==TOKEN_TERM)
                              {
                                 popState();
                              }
                              else
                              {
                                 Dsetparamn(0, popnode.node);
                                 Dpushstate;
                                 state = ST_EXPR;
                                 state_index = 0;
                              }
                           }
                        } // else tok!=var
                     } // else tok==TOKEN_MEMBER
                     break;
                  case 2: // <msg_expr> returned
                     tnode = Dgetparamn(0);
                     tnode.throwstat->msg_expr = popnode.expr;
                     popnode = tnode;
                     popState();
                     Dungettok;
                     break;
                  case 3: // skip '::' between namespace and classname
                     state_index = 1;
                     break;
                  case 10: // throw MyClass::MyException [<msgexpr>] ;  consume '::'
                     state_index++;
                     break;
                  case 11:
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        Dsetparams(1, sv); // remember exception name
                        Dsetparamnspace(2, cnspace);
                        cnspace = NULL;
                        if(ntok!=TOKEN_TERM)
                        {
                           Dpushstate;
                           state = ST_EXPR; // parse msgexpr
                           state_index = 0;
                        }
                        else
                        {
                           state_index+=2; // no msgexpr
                        }
                     }
                     break;
                  case 12: // throw MyClass::MyException [<msgexpr>] ;  <msgexpr> finished
                     Dsetparamn(3, popnode.expr);
                     // run into case 13
                  case 13: // throw MyClass::MyException [<msgexpr>] ;  finished
                  {
                     Dungettok; // do not consume ';'
                     PTN_NEW_NODE(nn, PTN_Throw)();
                     setPNode(nn);
                     popnode.throwstat->msg_expr = Dgetparamn(3).expr;
                     YAC_String *clname = Dgetparams(0);
                     YAC_String *ename = Dgetparams(1);
                     cnspace = Dgetparamnspace(2);

                     TKS_ClassDecl *cd;
                     if(NULL == cnspace)
                     {
                        cd = Dtkscompiler->findClassDeclNamespaceList(clname, active_namespaces);
                     }
                     else
                     {
                        cd = Dtkscompiler->findClassDeclNamespace(clname, cnspace);
                     }
                     if(cd == NULL)
                     {
#ifdef TKS_DCON
                        if(NULL != cnspace)
                        {
                           Dprintf("[---] class \"%s::%s\" not found while parsing throw statement.\n",
                                   (char*)cnspace->name.chars,
                                   (char*)clname->chars
                                   );
                        }
                        else
                        {
                           Dprintf("[---] class \"%s\" not found while parsing throw statement.\n", (char*)clname->chars);
                        }
#endif
                        Dparseerror;
                     }
                     else
                     {
                        cnspace = NULL;
                        popnode.throwstat->exception_type = tkscript->lazyGetExceptionTypeByName(cd, Dsrcloc, ename);
                     }
                     popState();
                  }
                  break;
                  case 20: // throw ::MyException [<msgexpr>] ;  parse exception name
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        Dsetparams(0, sv); // remember exception name
                        if(ntok!=TOKEN_TERM)
                        {
                           Dpushstate;
                           state = ST_EXPR; // parse msgexpr
                           state_index = 0;
                        }
                        else
                        {
                           state_index+=2; // no msgexpr
                        }
                     }
                     break;
                  case 21: // throw ::MyException [<msgexpr>] ;  <msgexpr> finished
                     Dsetparamn(3, popnode.expr);
                     // run into case 22
                  case 22: // throw ::MyException [<msgexpr>] ;  finished
                  {
                     Dungettok; // do not consume ';'
                     PTN_NEW_NODE(nn, PTN_Throw)();
                     setPNode(nn);
                     popnode.throwstat->msg_expr = Dgetparamn(3).expr;
                     YAC_String *ename = Dgetparams(0);
                     popnode.throwstat->exception_type = tkscript->lazyGetExceptionTypeByName(NULL, Dsrcloc, ename);
                     popState();
                  }
                  break;

               }
               break;

            case ST_TRYSTAT:
               switch(state_index)
               {
                  case 1:
                     PTN_NEW_NODE(nn, PTN_TryCatchFinally)(current_try);
                     setPNode(nn);
                     current_try = popnode.trycatchfinallystat;
                     Dsetparamn(0, popnode.node);
                     if(tok==TOKEN_BEGBLOCK) // shortpath to single sequence / sequence list
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SEQ;
                        state_index=0;
                        break;
                     }
                     else
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SSEQ;
                        state_index=0;
                     }
                     break;
                  case 2: // <try_body> finished
                     Dgetparamn(0).trycatchfinallystat->try_body = popnode.statement;
                     state_index++;
                     // run into case 3
                  case 3:
                     Dsetparamn(3, NULL); // default parent classdecl = 0
                     Dsetparami(2, Dsrcloc); // remember src location of catch/finally
                     if(tok == TOKEN_FINALLY)
                     {
                        state_index = 20;
                     }
                     else if(tok != TOKEN_CATCH)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        if(ntok==TOKEN_MEMBER)
                        {
                           // catch ::MyGlobalException ?
                           state_index=10; // consume '::'
                        }
                        else
                        {
                           state_index++;
                        }
                     }
                     break;
                  case 4: // catch <exception_name>
                     state_index++;
                     if(tok == TOKEN_BEGEXPR)
                     {
                        if(ntok==TOKEN_MEMBER)
                        {
                           if(TOKEN_MEMBER == nnntok)
                           {
                              // catch (::MyClass::MyException
                              cnspace = &Dtkscompiler->default_namespace; // Ddefaultnspace
                              state_index = 14;
                              break;
                           }
                           else
                           {
                              // catch (::MyGlobalException
                              state_index=10; // consume '::'
                           }
                        }
                        break; // consume '('
                     }
                     // run into case 5
                  case 5:
                     if(TOKEN_VAR != tok) // expect exception, class or namespace name
                     {
                        Dparseerror;
                     }
                     if(TOKEN_MEMBER == ntok) // catch MyClass::MyException  or  catch MyNamespace::MyClass::MyException
                     {
                        if(NULL == cnspace) // Dparsenspace(14)
                        {
                           cnspace = tkscript->findNamespace(sv);
                           if(NULL != cnspace)
                           {
                              if(TOKEN_MEMBER != nnntok) // do not allow mynamespace::myexception
                              {
                                 Dparseerror;
                              }
                              state_index = 14;
                              break;
                           }
                        }
                        TKS_ClassDecl *cd;
                        if(NULL == cnspace)
                        {
                           cd = Dtkscompiler->findClassDeclNamespaceList(sv, active_namespaces);
                        }
                        else
                        {
                           cd = Dtkscompiler->findClassDeclNamespace(sv, cnspace);
                        }
                        if(NULL == cd)
                        {
#ifdef TKS_DCON
                           if(NULL != cnspace)
                           {
                              Dprintf("[---] class \"%s::%s\" not found while parsing catch{}.\n",
                                      (char*)cnspace->name.chars,
                                      (char*)sv->chars
                                      );
                           }
                           else
                           {
                              Dprintf("[---] class \"%s\" not found while parsing catch{}.\n", (char*)sv->chars);
                           }
#endif
                           Dparseerror;
                        }
                        else
                        {
                           cnspace = NULL;
                           Dsetparamn(3, (PTN_Node*)cd);
                           state_index=12; // consume '::' and parse exception name
                        }
                     }
                     else
                     {
                        Dsetparams(1, sv); // remember exception name
                        if(ntok == TOKEN_VAR) // (optional) exception var name?
                        {
                           state_index = 6;
                        }
                        else
                        {
                           pushExceptionVarname(&tok_defaultExceptionVarName); // anonymous exception, assign default (unusable) name
                           state_index = 7;
                        }
                     }
                     break;
                  case 6: // (optional) exception var name
                     pushExceptionVarname(sv);
                     state_index++;
                     break;
                  case 7:
                     state_index++;
                     if(tok == TOKEN_ENDEXPR)
                     {
                        break; // consume ')'
                     }
                     // run into case 8
                  case 8: // parse catch{} body
                     if(tok==TOKEN_BEGBLOCK) // shortpath to single sequence / sequence list
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SEQ;
                        state_index=0;
                        break;
                     }
                     else
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SSEQ;
                        state_index=0;
                     }
                     break;
                  case 9: // catch body finished
                  {
                     TKS_ClassDecl *cd = (TKS_ClassDecl*)Dgetparamn(3).node;
                     PTN_TryCatchFinally *tcf = Dgetparamn(0).trycatchfinallystat;
                     TKS_ExceptionType *et = NULL;
                     if(cd == NULL)
                     {
                        // First search class exception
                        et = tkscript->getExceptionTypeByName(cclassdecl, Dgetparams(1));
                        if(et==NULL)
                        {
                           et = tkscript->lazyGetExceptionTypeByName(NULL, Dgetparami(2), Dgetparams(1));
                        }
                     }
                     else
                     {
                        et = tkscript->lazyGetExceptionTypeByName(cd, Dgetparami(2), Dgetparams(1));
                     }
                     tcf -> addCatchSorted( et, popnode.statement);
                     Dungettok;
                     if( (tok == TOKEN_CATCH) || (tok == TOKEN_FINALLY) )
                     {
                        state_index = 3; // loop catch blocks / parse finally
                     }
                     else
                     {
                        popnode = Dgetparamn(0);
                        popExceptionVarname();
                        current_try = popnode.trycatchfinallystat->parent;
                        popState();
                     }
                  }
                  break;
                  case 10: // consume '::' in 'catch ::MyGlobalException'
                     state_index++;
                     break;
                  case 11:
                     if(tok != TOKEN_VAR) // expect exception name
                     {
                        Dparseerror;
                     }
                     Dsetparams(1, sv); // remember exception name
                     if(ntok == TOKEN_VAR) // (optional) exception var name?
                     {
                        state_index = 6;
                     }
                     else
                     {
                        pushExceptionVarname(&tok_defaultExceptionVarName); // anonymous exception, assign default (unusable) name
                        state_index = 7;
                     }
                     break;
                  case 12: // consume '::' in 'catch MyClass::MyException'
                     state_index++;
                     break;
                  case 13:
                     if(tok != TOKEN_VAR) // expect exception name
                     {
                        Dparseerror;
                     }
                     Dsetparams(1, sv); // remember exception name
                     if(ntok == TOKEN_VAR) // (optional) exception var name?
                     {
                        state_index = 6;
                     }
                     else
                     {
                        pushExceptionVarname(&tok_defaultExceptionVarName); // anonymous exception, assign default (unusable) name
                        state_index = 7;
                     }
                     break;
                  case 14: // consume '::' between namespace and classname (catch)
                     state_index = 5;
                     break;


                  case 20: // parse finally
                     if(Dgetparamn(0).trycatchfinallystat -> finally_body != NULL)
                     {
                        Dprintf("[---] try statement already has a finally block.\n");
                        Dparseerror;
                     }
                     if(tok==TOKEN_BEGBLOCK) // shortpath to single sequence / sequence list
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SEQ;
                        state_index=0;
                        break;
                     }
                     else
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SSEQ;
                        state_index=0;
                     }
                     break;
                  case 21: // finally body finished
                     Dgetparamn(0).trycatchfinallystat -> finally_body = popnode.statement;
                     Dungettok;
                     if( (tok == TOKEN_CATCH) || (tok == TOKEN_FINALLY) )
                     {
                        state_index = 3; // loop catch blocks / parse finally
                     }
                     else
                     {
                        popnode = Dgetparamn(0);
                        /////current_exception_varname = NULL;
                        current_try = popnode.trycatchfinallystat->parent;
                        popState();
                     }
                     break;
               }
               break;


            case ST_SSEQSTAT: // single statement without TERM token ";"
               switch(state_index)
               {
                  case 1:
                     Dungettok;
                     popState();
                     break;
                  case 0:
                  {
                     if(b_force_this_dot)
                     {
                        if(tok != TOKEN_DOT)
                        {
                           Dprintf("[---] expected \'.\'.\n");
                           Dparseerror;
                        }
                        else
                        {
                           b_force_this_dot = 0;
                           break;
                        }
                     }
                     else if(b_force_this)
                     {
                        if(tok != TOKEN_VAR)
                        {
                           Dprintf("[---] expected member name.\n");
                           Dparseerror;
                        }
                     }
                     switch(tok)
                     {
                        default:
                           Dparseerror;
                           break;

                        case TOKEN_BREAK:
                           Dpushstate;
                           state=ST_BREAK;
                           state_index=0;
                           break;

                        case TOKEN_EXPLAIN:
                           Dpushstate;
                           state=ST_EXPLAIN;
                           state_index=1;
                           break;

                        case TOKEN_COMPILE:
                           if(vmcore)
                           {
                              Dprintf("[---] compile keyword within compile {} sequence.\n");
                              Dparseerror;
                           }
                           else
                           {
                              Dpushstate;
                              state=ST_COMPILE;
                              state_index=1;
                           }
                           break;
                        case TOKEN_THROW:
                           Dpushstate;
                           state=ST_THROWSTAT;
                           state_index=1;
                           Dsetparami(0, 0);
                           break;
                        case TOKEN_TRY:
                           Dpushstate;
                           state=ST_TRYSTAT;
                           state_index=1;
                           Dsetparami(0, 0);
                           break;
                           /////case TOKEN_CLAMP:
                           /////case TOKEN_WRAP:
                        case TOKEN_PREPARE:
                           Dpushstate;
                           state=ST_PREPARESTAT;
                           state_index=1;
                           break;
                           /////case TOKEN_DTRACE:
                           /////case TOKEN_TRACE:
                           /////case TOKEN_PRINT:
                           /////case TOKEN_STDOUT:
                           /////case TOKEN_STDERR:
                           /////case TOKEN_DCON:
                           /////case TOKEN_DIE:
                           /////case TOKEN_FLUSH:
                        case TOKEN_ABSTRACT:
                           // next token is "class" (checked in scanner)
                           break;
                        case TOKEN_CLASS:
#ifndef TKS_ALLOW_TEMP_CLASSES
                           if(is_tmod)
                           {
                              Dparseerror;
                           }
                           else
#endif // TKS_ALLOW_TEMP_CLASSES
                           {
                              Dungetc;
                              Dpushstate;
                              state=ST_CLASSDECL;
                              state_index=0;
                           }
                           break;
                        case TOKEN_MODULE:
                           Dungetc;
                           Dpushstate;
                           state=ST_MODULE;
                           state_index=0;
                           break;
                        case TOKEN_ENUM:
                           Dungetc;
                           Dpushstate;
                           state=ST_ENUM;
                           state_index=0;
                           break;
                        case TOKEN_DEFINE:
                        case TOKEN_DEFINEALT:
                           Dungetc;
                           Dpushstate;
                           state=ST_DEFINE;
                           state_index=0;
                           break;
                        case TOKEN_CONSTRAINT:
                           Dpushstate;
                           state=ST_CONSTRAINT; // does not emit node
                           state_index=1;
                           break;
                        case TOKEN_ARRAY:
                           Dungetc;
                           Dpushstate;
                           state=ST_DYNARRAYDECL;
                           state_index=0;
                           break;
                        case TOKEN_USE: // ST_UFUNEVENT
                           Dungetc;
                           Dpushstate;
                           state=ST_UFUNEVENT;
                           state_index=0;
                           break;
                        case TOKEN_NAMESPACE: // ST_NAMESPACE
                           Dpushstate;
                           state = ST_NAMESPACE;
                           state_index = 1;
                           break;
                        case TOKEN_RETURN: // ST_UFUNRETURN
                           if(!cfunction)
                           {
                              Dprintf("\n[---] return outside a function.\n");
                              Dparseerror;
                           }
                           Dungetc;
                           Dpushstate;
                           state=ST_UFUNRETURN;
                           state_index=0;
                           break;

                        case TOKEN_ASSIGN: // =mymutex=
                           Dpushstate;
                           state = ST_MUTEXDECL;
                           state_index=1;
                           break;

                        case TOKEN_FUNCTION: // ST_UFUNDECL
                           if(cfunction)
                           {
                              Dprintf("\n[---] functions within functions are not allowed.\n");
                              cfunction=0;
                              Dparseerror;
                           }
                           Dungetc;
                           Dpushstate;
                           Dsetparamn(0, 0); // no function existing yet, used for classes
                           state=ST_UFUNDECL;
                           state_index=0;
                           break;

                        case TOKEN_DELEGATE: // delegate MyClass::test(...
                           Dpushstate;
                           state=ST_DELEGATE;
                           state_index=1;
                           break;

                        case TOKEN_TERM: // empty statement
                           Dungettok;
                           PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_LOOP:    Dungetc; Dpushstate; state_index=0; state=ST_LOOP;    break;
                        case TOKEN_FOREACH: Dungetc; Dpushstate; state_index=0; state=ST_FOREACH; break;
                        case TOKEN_FOR:     Dungetc; Dpushstate; state_index=0; state=ST_FOR;     break;
                        case TOKEN_IF:      Dungetc; Dpushstate; state_index=0; state=ST_IF;      break;
                        case TOKEN_SWITCH:  Dungetc; Dpushstate; state_index=0; state=ST_SWITCH;  break;
                        case TOKEN_WHILE:   Dungetc; Dpushstate; state_index=0; state=ST_WHILE;   break;
                        case TOKEN_DO:      Dungetc; Dpushstate; state_index=0; state=ST_DOWHILE; break;
                        case TOKEN_TAG:
                        case TOKEN_LOCAL:
                        case TOKEN_INT:
                        case TOKEN_FLOAT:
                        case TOKEN_STRING:
                        case TOKEN_OBJECT:  Dungetc; Dpushstate; state_index=0; state=ST_VARDECL; vardecl_tag=0; break;
                        case TOKEN_THIS:
                           if(cclassdecl)
                           {
                              ///sv = &tks_this_name;
                              ////*tks_this_adr = cclassdecl->class_template;
                              b_force_this = 1;
                              b_force_this_dot = 1;
                              ////goto label_sseqstat_var;
                              // ---- Reuse ClassMember* nodes ; skip local/global var search
                           }
                           else
                           {
                              Dprintf("\n[---] \"this\" cannot be used outside classes.\n");
                              Dparseerror;
                           }
                           break;
                        case TOKEN_MEMBER:
                           Ddefaultnspace;
                           break;
                        case TOKEN_VAR: // ST_VARASSIGN, ST_CALL
///label_sseqstat_var:

                           // Check for builtin procedure name (clamp, wrap, print, ..)
                           if(NULL == cnspace)
                           {
                              sU8 bprocId = tks_get_pseudotoken_id_by_name(sv);
                              if(bprocId >= TKS_BPROC_FIRST)
                              {
                                 if(bprocId == TKS_BPROC_STDOUT)
                                 {
                                    Dpushstate;
                                    Dsetparami(0, 1); // target
                                    Dsetparami(1, 0); // dynamic trace
                                    Dsetparami(2, 0); // add newline
                                    state=ST_PRINT;
                                    state_index=1;
                                    break;
                                 }
                                 else if(bprocId == TKS_BPROC_STDERR)
                                 {
                                    Dpushstate;
                                    Dsetparami(0, 2);
                                    Dsetparami(1, 0);
                                    Dsetparami(2, 0); // add newline
                                    state=ST_PRINT;
                                    state_index=1;
                                    break;
                                 }
                                 else if(bprocId == TKS_BPROC_DCON)
                                 {
                                    Dpushstate;
                                    Dsetparami(0, 0);
                                    Dsetparami(1, 1); // dynamic trace
                                    Dsetparami(2, 0); // add newline
                                    state=ST_PRINT;
                                    state_index=1;
                                    break;
                                 }
                                 else if(bprocId == TKS_BPROC_DIE)
                                 {
                                    Dpushstate;
                                    Dsetparami(0, 3);
                                    Dsetparami(1, 0);
                                    Dsetparami(2, 1); // add newline
                                    state=ST_PRINT;
                                    state_index=1;
                                    break;
                                 }
                                 else if(bprocId == TKS_BPROC_PRINT)
                                 {
                                    Dpushstate;
                                    Dsetparami(0, 1); // target stdout
                                    Dsetparami(1, 0); // dynamic trace
                                    Dsetparami(2, 1); // add newline
                                    state=ST_PRINT;
                                    state_index=1;
                                    break;
                                 }
                                 else if(bprocId == TKS_BPROC_TRACE)
                                 {
                                    Dpushstate;
                                    Dsetparami(0, 0); // target dcon
                                    Dsetparami(2, 1); // add newline
                                    state=ST_TRACE;
                                    state_index=1;
                                    break;
                                 }
                                 else if(bprocId == TKS_BPROC_DTRACE)
                                 {
                                    Dpushstate;
                                    state=ST_DTRACE;
                                    state_index=0;
                                    Dpushstate;
                                    state=ST_EXPR;
                                    state_index=0;
                                    break;
                                 }
                                 else if(bprocId == TKS_BPROC_STDFLUSH)
                                 {
                                    Dpushstate;
                                    state=ST_FLUSH;
                                    state_index=1;
                                    break;
                                 }
                                 else if(bprocId == TKS_BPROC_CLAMP)
                                 {
                                    Dpushstate;
                                    state=ST_CLAMPWRAPSTAT;
                                    state_index=1;
                                    Dsetparami(0, 0);
                                    break;
                                 }
                                 else if(bprocId == TKS_BPROC_WRAP)
                                 {
                                    Dpushstate;
                                    state=ST_CLAMPWRAPSTAT;
                                    state_index=1;
                                    Dsetparami(0, 1);
                                    break;
                                 }
                              }
                           } // check for builtin procedure name

                           if(nntok == TOKEN_ENDARRAY)
                              ntok = TOKEN_VAR; // prevent Double[]*doublearray[] from entering ST_VARASSIGN (=> go to ST_VARDECL instead)

                           switch(ntok)
                           {
                              case TOKEN_MEMBER:
                                 if( (NULL == cnspace) && (TOKEN_VAR == nntok) )
                                 {
                                    cnspace = tkscript->findNamespace(sv);
                                    if(NULL != cnspace)
                                    {
                                       // Is namespace, parse and loop state
                                       state_index = 4;
                                       break;
                                    }
                                 }
                                 if(b_force_this)
                                 {
                                    Dparseerror;
                                 }
                                 if( (NULL == cnspace) && cclassdecl)
                                 {
                                    if((nntok==TOKEN_DOT)||(nntok==TOKEN_BEGARRAY))
                                    {
                                       Dungettok;
                                       Dpushstate;
                                       state=ST_EXPRSTAT;
                                       state_index=0;
                                    }
                                    else
                                    {
                                       Dungettok;
                                       Dpushstate;
                                       Dsetparami(0, 1);
                                       state=ST_CLASSSUPERECALL;
                                       state_index=0;
                                    }
                                 }
                                 else
                                 {
                                    Dungettok;
                                    Dpushstate;
                                    state=ST_CLASSMETHODBODY;
                                    state_index=0;
                                 }
                                 break;
                              case TOKEN_ASL: // "<<"
                                 Dpushstate;
                                 state = ST_VARASSIGN;
                                 state_index=100;
                                 Dsetparams(0, sv);
                                 Dsetparami(2, YAC_OP_SHL);
                                 break;

                              case TOKEN_CMPLE: // "<="
                                 Dpushstate;
                                 state = ST_PTRASSIGN;
                                 state_index = 1;
                                 break;
                              case TOKEN_ADDASSIGN:
                              case TOKEN_SUBASSIGN:
                              case TOKEN_MULASSIGN:
                              case TOKEN_DIVASSIGN:
                              case TOKEN_MODASSIGN:
                              case TOKEN_ASLASSIGN:
                              case TOKEN_ASRASSIGN:
                              case TOKEN_BORASSIGN:
                              case TOKEN_BANDASSIGN:
                              case TOKEN_EORASSIGN:
                              case TOKEN_ASSIGN: // s =
                                 ////Dungettok;
                                 Dpushstate;
                                 state=ST_VARASSIGN;
                                 state_index = 1;
                                 break;
                              case TOKEN_DOT:
                                 if(nnntok==TOKEN_BEGEXPR) // "(" follows?
                                 {
                                    if(b_force_this)
                                    {
                                       Dparseerror;
                                    }
                                    /// object.method(..
                                    Dungettok;
                                    Dpushstate;
                                    state=ST_CALL;
                                    state_index=0;
                                    Dpushstate;
                                    state=ST_ECALL;
                                    state_index=0;
                                 }
                                 else
                                 {
                                    /// object.member[=,+=,-=,*=,/=,%=,<<=,>>=,|=,&=,^=]
                                    /// classinst.member[=,+=,-=,*=,/=,%=,<<=,>>=,|=,&=,^=, <=]
                                    switch(nnntok)
                                    {
                                       default:
                                          Dprintf("[---] invalid assignment operator follows.\n");
                                          Dparseerror;
                                          break;
                                       case TOKEN_ADDASSIGN:
                                       case TOKEN_SUBASSIGN:
                                       case TOKEN_MULASSIGN:
                                       case TOKEN_DIVASSIGN:
                                       case TOKEN_MODASSIGN:
                                       case TOKEN_ASLASSIGN:
                                       case TOKEN_ASRASSIGN:
                                       case TOKEN_BORASSIGN:
                                       case TOKEN_BANDASSIGN:
                                       case TOKEN_EORASSIGN:
                                       case TOKEN_ASSIGN: // xxx
                                          ////Dungettok;
                                          Dpushstate;
                                          state=ST_OBJVARASSIGN;
                                          state_index = 1;
                                          break;
                                       case TOKEN_INC: // xxx
                                       case TOKEN_DEC: // xxx
                                          Dungettok;
                                          Dpushstate;
                                          state=ST_READPROPERTY;
                                          Dsetparami(2, 1);
                                          state_index=0;
                                          break;
                                       case TOKEN_CMPLE:
                                          Dungettok;
                                          Dpushstate;
                                          Dsetparami(2,1);
                                          state=ST_READPROPERTY;
                                          state_index=0;
                                          break;
                                       case TOKEN_BEGARRAY:
                                       case TOKEN_DOT:
                                          // ExprStat
                                          // object.member .
                                          // classinst.member .
                                          // object.member [
                                          // classinst.member [
                                          if(b_force_this)
                                          {
                                             Dparseerror;
                                          }
                                          Dungettok;
                                          Dpushstate;
                                          Dsetparamn(0,0);
                                          state=ST_EXPRSTAT;
                                          state_index=0;
                                          break;
                                    } // switch nnntok
                                 }
                                 break;
                              case TOKEN_INC:
                                 Dungettok;
                                 Dpushstate;
                                 state=ST_POSTINCVARSTAT;
                                 state_index=0;
                                 break;
                              case TOKEN_DEC:
                                 Dungettok;
                                 Dpushstate;
                                 state=ST_POSTDECVARSTAT;
                                 state_index=0;
                                 break;
                              case TOKEN_BEGEXPR:
                                 if(b_force_this)
                                 {
                                    Dparseerror;
                                 }
                                 Dungettok;
                                 Dpushstate;
                                 state=ST_UFUNCALL;
                                 state_index=0;
                                 Dpushstate;
                                 state=ST_UFUNECALL;
                                 state_index=0;
                                 break;
                              case TOKEN_MUL:
                              case TOKEN_VAR:
                                 /*if(tok==TOKEN_THIS)
                                   {
                                   Dparseerror;
                                   }*/
                                 if(b_force_this)
                                 {
                                    Dparseerror;
                                 }
                                 Dungettok;
                                 Dpushstate;
                                 vardecl_tag=0;
                                 state=ST_VARDECL;
                                 state_index=0;
                                 break;
                              case TOKEN_BEGARRAY:
                                 ///Dungettok;
                                 Dpushstate;
                                 state=ST_VARASSIGN;
                                 state_index = 5;
                                 break;
                              default:
                                 Dparseerror;
                                 break;
                           } // switch ntok
                           break;

                        case TOKEN_INC: // "++"
                           Dungettok;
                           Dpushstate;
                           state=ST_PREINCVARSTAT;
                           state_index=0;
                           break;

                        case TOKEN_DEC: // "--"
                           Dungettok;
                           Dpushstate;
                           state=ST_PREDECVARSTAT;
                           state_index=0;
                           break;
                     }
                     break;
                  }
                  break;

                  case 2:
                     // consume TOKEN_TERM
                     // PTN_VarDecl node skipping
                     if(tok!=TOKEN_TERM)
                     {
                        Dparseerror;
                     }
                     state_index=0;
                     break;

                  case 3:
                     // no TOKEN_TERM, skip "trace" nodes
                     Dungettok;
                     popState();
                     break;

                  case 4: // consume '::' between namespace and class/varname
                     state_index = 0;
                     break;

               }
               break;



            case ST_BREAK:
               ////if(vmcore)
               ////{
               ////   Dprintf("[---] the break keyword is not supported by the JIT compiler.\n");
               ////   Dparseerror;
               ////}
               ////else
            {
               if(pbreak)
               {
                  Dungettok;
                  pbreak->useBreak(); // Hint for parent statement that this st-sequence uses break;
                  PTN_NEW_NODE(nn, PTN_Break)();
                  popnode.breakstat = (PTN_Break*)nn;/////pbreak->getBreakAdr());
                  popnode.breakstat->parent_st = pbreak;
                  setPNode(popnode.node);
               }
               else
               {
                  Dprintf("[---] unexpected break;\n");
                  Dparseerror;
               }
               popState();
            }
            break;


#ifdef TKS_JIT
            case ST_COMPILE:
               switch(state_index)
               {
                  case 1:
                     /*if(vmasm)
                       {
                       #ifdef TKS_DCON
                       tkscript->printf("[---] trying to compile within an asm/compile block?)\n");
                       #endif
                       Dparseerror;
                       }*/
                     // -------- create new VM --------
                     PTN_NEW_NODE(nn, VMCore)();
                     vmcore = (VMCore*) nn;
                     setPNode(vmcore);
                     vmcore->prepareCompile();
                     Dpushstate;
                     state_index=0;
                     if(tok==TOKEN_BEGBLOCK) state=ST_SEQ;
                     else state=ST_SSEQ;
                     Dungettok;
                     break;
                  case 2:
                     // statement (block) finished
                     Dungettok;
                     vmcore->parser_tree=popnode.statement;
                     popnode.statement=vmcore;
                     popState();
                     vmcore=0;
                     break;
               }
               break;
#else
               //#error "[---] JIT support not compiled in."
            case ST_COMPILE: // **FAKE** compile
               switch(state_index)
               {
                  case 1:
                     // -------- create new VM --------
                     PTN_NEW_NODE(nn, VMCore)();
                     vmcore = (VMCore*) nn;
                     setPNode(vmcore);
                     //vmcore->prepareCompile();
                     Dpushstate;
                     state_index=0;
                     if(tok==TOKEN_BEGBLOCK) state=ST_SEQ;
                     else state=ST_SSEQ;
                     Dungettok;
                     break;
                  case 2:
                     // statement (block) finished
                     Dungettok;
                     vmcore->parser_tree=popnode.statement;
                     popnode.statement=vmcore;
                     popState();
                     vmcore=0;
                     break;
               }
               break;
#endif

            case ST_CLAMPWRAPSTAT:
               // par i0 == 1 => WRAP : CLAMP
               switch(state_index)
               {
                  case 1:
                     Dparsenspace(4);

                     if(TOKEN_VAR != tok)
                     {
                        Dparseerror;
                     }
                     co = findVar(sv, active_namespaces, cnspace);
                     if(!co)
                     {
                        Dprintf("\n[---] cannot find clamp/wrap variable \"%s\".\n",
                                (char*)sv->chars);
                        Dparseerror;
                     }
                     {
                        sU8 md = (sU8) (Dgetparami(0)?PTN_ClampWrapStat::MD_WRAP:PTN_ClampWrapStat::MD_CLAMP);
                        Dsetparami(0,md);
                        Dsetparamv(1, (void*)co);
                        Dpushstate;
                        state=ST_EXPR;
                        state_index=0;
                     }
                     break;
                  case 4: // skip '::' between namespace and varname
                     state_index = 1;
                     break;

                  case 2:
                     Dsetparamn(2, popnode.expr);
                     Dungettok;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 3:
                  {
                     co=(TKS_CachedObject*)Dgetparamv(1);
                     PTN_Expr *min=Dgetparamn(2).expr;
                     PTN_Expr *max=popnode.expr;
                     if(co->type==3)
                     {
                        // ---- generic object clamp/wrap stat ----
                        PTN_NEW_NODE(nn, PTN_ObjClampWrapStat)(Dgetparami(0), co, min, max);
                        setPNode(nn);
                     }
                     else
                     {
                        if(min->isConst()&&max->isConst())
                        {
                           if(min->semanticCheck())
                           {
                              if(min->resolveXRef())
                              {
                                 if(max->semanticCheck())
                                 {
                                    if(max->resolveXRef())
                                    {
                                       min->optimize();
                                       max->optimize();
                                       PTN_Env env; env.initDefault();
                                       YAC_Value minv; min->eval(&env, &minv);
                                       YAC_Value maxv; max->eval(&env, &maxv);
                                       delete min;
                                       delete max;
                                       PTN_NEW_NODE(nn, PTN_ClampWrapStat)(Dgetparami(0), co, minv, maxv);
                                       setPNode(nn);
                                    }
                                    else
                                    {
                                       Dprintf("[---] ClampWrapStat: max expr resolveXRef() failed.\n");
                                       Dparseerror;
                                       break;
                                    }
                                 }
                                 else
                                 {
                                    Dprintf("[---] ClampWrapStat: max expr semanticCheck() failed.\n");
                                    Dparseerror;
                                    break;
                                 }
                              }
                              else
                              {
                                 Dprintf("[---] ClampWrapStat: min expr resolveXRef() failed.\n");
                                 Dparseerror;
                                 break;
                              }
                           }
                           else
                           {
                              Dprintf("[---] ClampWrapStat: min expr semanticCheck() failed.\n");
                              Dparseerror;
                              break;
                           }
                        }
                        else
                        {
                           Dprintf("\n[---] clip/wrap range expressions have to be constant..\n");
                           Dparseerror;
                        }
                     }
                     Dungettok;
                     popState();
                     cnspace = NULL;
                  }
                  break;
               }
               break;

            case ST_PREPARESTAT:
               switch(state_index)
               {
                  case 1:
                     Dungettok;
                     Dpushstate;
                     state=ST_SEQ;
                     state_index=0;
                     break;
                  case 2:
                     // <SEQ> finished
                     Dungettok;
                     PTN_NEW_NODE(nn, PTN_PrepareStat)(popnode.statement);
                     setPNode(nn);
                     popState();
                     break;
               }
               break;

            case ST_EXPRSTAT:
               switch(state_index)
               {
                  case 0:
                     Dungettok;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 1:
                     // <EXPR> finished
                     Dungettok;
                     PTN_NEW_NODE(nn, PTN_ExprStat)(popnode.expr);
                     setPNode(nn);
                     popState();
                     break;
                  case 2:
                     // paramn 0=expr node
                     Dungettok;
                     tnode=Dgetparamn(0);
                     state_index=0;
                     Dpushstate;
                     Dsetparamn(0, tnode.node);
                     state=ST_IEXPR;
                     state_index=0;
                     break;

               }
               break;

            case ST_CLASSSUPERECALL:
               // SuperClass::method()
               //
               // params:
               // 0: 1==gen statement, 0==gen ecall
               //
               // note: cnspace must already be set
               //
               switch(state_index)
               {
                  case 0:
                     // ancestor class name
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     Dsetparams(1, sv); // remember superclassname
                     state_index++;
                     break;
                  case 1:
                     if(tok!=TOKEN_MEMBER)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 2:
                  {
                     // expect methodname
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     sBool btrynext=1;

                     if(cclassdecl)
                     {
                        // BaseClass::myconstant, BaseClass::mymethod(
                        YAC_String *scname = Dgetparams(1);

                        TKS_ClassDecl *ac;

                        // Check whether both class name and namespace match
                        sBool bSelfClass = scname->compare(cclassdecl->name);

                        if(bSelfClass)
                        {
                           if(NULL == cnspace)
                           {
                              // Must match active namespace / current default namespace
                              bSelfClass = (cclassdecl->nspace == create_namespace);
                           }
                           else
                           {
                              // Must match selected namespace,
                              // mynamespace::BaseClass::myconstant,
                              // mynamespace::BaseClass::mymethod(
                              bSelfClass = (cclassdecl->nspace == cnspace);
                           }
                        }

                        if(bSelfClass)
                        {
                           ac = cclassdecl;
                        }
                        else
                        {
                           // Resolve scriptclass
                           TKS_ClassDecl *sccd;
                           if(NULL == cnspace)
                           {
                              sccd = Dtkscompiler->findClassDeclNamespaceList(scname, active_namespaces);
                           }
                           else
                           {
                              sccd = Dtkscompiler->findClassDeclNamespace(scname, cnspace);
                           }
                           if(NULL != sccd)
                           {
                              ac = cclassdecl->findAncestorDep(sccd);
                           }
                           else
                           {
                              ac = NULL;
                           }
                           if(!ac)
                           {
                              Dprintf("\n[---] unknown reference \"%s::%s\" (failed to resolve baseclass).\n",
                                      (char*)scname->chars,
                                      (char*)sv->chars
                                      );
                              Dparseerror;
                           }
                           else
                           {
                              Dsetparamv(1, (void*)ac);

                              // ---- CScriptBaseClass::constant, mynamespace::CScriptBaseClas::constant ?
                              TKS_CachedObject *co = ac->findConstant(sv); // only search baseclass ac
                              if(co)
                              {
                                 if(Dgetparami(0)) // spawn statement?
                                 {
                                    Dparseerror;
                                    break;
                                 }
                                 else
                                 {
                                    spawnClassConstantExpr(co);
                                    btrynext=0;
                                    Dpopstate;
                                    cnspace = NULL;
                                 }

                              }
                              else
                              {
                                 btrynext=1;
                              }


                              if(btrynext)
                              {
                                 TKS_ClassDeclMethod *meth = ac->findMethodSuper(sv);
                                 if(cfunction)
                                 {
                                    if(cfunction->is_class_function)
                                    {
                                       meth=0; // cannot call method without an object
                                    }
                                 }
                                 if(meth)
                                 {
                                    Dsetparamv(1, (void*)ac);
                                    Dsetparamv(3, (void*)meth);
                                    state_index++; // parse arglist
                                    cnspace = NULL;
                                    btrynext=0;
                                 }
                                 //else
                                 //   btrynext=1;
                              }
                           } // else ac
                        } // btrynext
                     } // if cclassdecl

                     if(btrynext)
                     {
                        // ---- call to a static class function ??
                        TKS_ClassDecl *cd;
                        YAC_String *scname = Dgetparams(1);

                        if(NULL == cnspace)
                        {
                           cd = Dtkscompiler->findClassDeclNamespaceList(scname, active_namespaces);
                        }
                        else
                        {
                           if(cnspace != &Dtkscompiler->yac_namespace)
                           {
                              cd = cnspace->findClassDecl(scname);
                              if(NULL == cd)
                              {
                                 Dprintf("[---] script class \"%s::%s\" does not exist.\n",
                                         (char*)cnspace->name.chars,
                                         (char*)scname->chars
                                         );
                                 Dparseerror;
                              }
                           }
                           else
                           {
                              cd = NULL;
                           }
                        }
                        if(cd)
                        {
                           btrynext=0;
                           PTN_Function *f = cd->findFunctionSuper(sv); // new 05-May-07
                           if(f)
                           {
                              DcheckFunctionAccess(cd, f);
                              Dsetparamv(1, (void*)f); // remember function
                              state_index=103; // parse arglist
                              cnspace = NULL;
                           }
                           else
                           {
                              Dprintf("\n[---] unknown reference to function \"%s::%s\".\n",
                                      (char*)Dgetparams(1)->chars, (char*)sv->chars);
                              Dparseerror;
                              break;
                           }
                        }

                        if(btrynext && ((NULL==cnspace) || (cnspace == &Dtkscompiler->yac_namespace)) )
                        {
                           // ---- try "static" c++ class method call (template method call) ----
                           ////tkscript->printf("try static class call\n");
                           TKS_ClassTemplate *clt=(TKS_ClassTemplate*)tkscript->findTemplate(sv);
                           if(clt)
                           {
                              if(  (!clt->may_instantiate) || ( ((*sv)[0]>='A') && ((*sv)[0]<='Z') )  )
                              {
                                 btrynext=0;
                                 if(Dgetparami(0))
                                 {
                                    state=ST_CALL;
                                 }
                                 else
                                 {
                                    state=ST_ECALL;
                                 }
                                 cnspace = NULL;
                                 state_index=0;
                                 Dungettok;
                              }
                              else
                              {
                                 Dprintf("[---] cannot access method of non-static c++ class without an instance \"%s\".\n",
                                         (char*)sv->chars);
                                 Dparseerror;
                                 break;
                              }
                           }
                           else
                           {
                              if(cnspace == &Dtkscompiler->yac_namespace) // forced C++ namespace ?
                              {
                                 Dprintf("[---] C++ class \"%s\" does not exist.\n",
                                         (char*)sv->chars
                                         );
                                 Dparseerror;
                              }
                           }
                        }

                        if(btrynext)
                        {
                           Dprintf("[---] failed to resolve class \"%s\".\n", (char*)sv->chars);
                           Dparseerror;
                        }
                     }
                     break;
                  }

                  case 3:
                     if(tok!=TOKEN_BEGEXPR)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_ARGLIST;
                     state_index=0;
                     break;
                  case 4:
                     // FUNARGS finished
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     {
                        TKS_ClassDecl *ac=(TKS_ClassDecl*)Dgetparamv(1);
                        PTN_NEW_NODE(nn, PTN_ClassMethodECall)(0, (TKS_ClassDeclMethod*)Dgetparamv(3), popnode.arglist);
                        setPNode(nn);
                        popnode.classmethodecall->no_virtual=1;
                        popnode.classmethodecall->class_id=ac->class_id;
                        if(Dgetparami(0))
                        {
                           // gen statement
                           PTN_NEW_NODE(nn, PTN_Call)(popnode.callableexpr);
                           setPNode(nn);
                        }
                        popState();
                     }
                     break;
                  case 103:
                     // ---- parse args of static method call
                     if(tok!=TOKEN_BEGEXPR)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_ARGLIST; //FUNARGS;
                     state_index=0;
                     break;
                  case 104:
                     // ST_ARGLIST finished
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        PTN_NEW_NODE(nn, PTN_FunctionECall)((PTN_Function*)Dgetparamv(1), popnode.arglist);
                        setPNode(nn);
                        if(Dgetparami(0))
                        {
                           PTN_NEW_NODE(nn, PTN_Call)(popnode.callableexpr);
                           setPNode(nn);
                        }
                        popState();
                     }
                     break;
               }
               break;

            case ST_DTRACE:
               switch(state_index)
               {
                  case 1:
                     Dungettok;
                     PTN_NEW_NODE(nn, PTN_DynTraceStat)(popnode.expr);
                     setPNode(nn);
                     popState();
                     break;
               }
               break;

            case ST_TRACE:
               // parami[0]: taget
               switch(state_index)
               {
                  case 1:
                     switch(tok)
                     {
                        case TOKEN_TRUE:
                           tkscript->trace_enabled=1;
                           PTN_NEW_NODE(nn, PTN_TraceStat)();
                           popnode.statement = (PTN_Statement*)nn;
                           popState();
                           break;
                        case TOKEN_FALSE:
                           tkscript->trace_enabled=0;
                           PTN_NEW_NODE(nn, PTN_TraceStat)();
                           popnode.statement = (PTN_Statement*)nn;
                           popState();
                           break;
                        default:
                           Dungettok;
                           if(tkscript->trace_enabled)
                           {
                              // ---- jump into ST_PRINT state ----
                              state=ST_PRINT;
                              state_index=1;
                              //Dsetparami(0, 0); // target == dcon
                              Dsetparami(1, 1); // depend on dynamic trace status
                              Dpushstate;
                              state=ST_EXPR;
                              state_index=0;
                           }
                           else
                           {
                              // consume statement
                              Dpushstate;
                              state=ST_EXPR;
                              state_index=0;
                           }
                           break;
                     }
                     break;
                  case 2:
                     // consume <EXPR> if trace is globally disabled
                     delete popnode.expr;
                     popnode.expr=0;
                     Dungettok;
                     popState();
                     state=ST_SSEQSTAT;
                     state_index=0; // return to ST_SSEQSTAT, do not gen node
                     break;
               }
               break;

            case ST_PRINT:
               switch(state_index)
               {
                  // case 0 is skipped
                  case 1:
                     Dungettok;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 2:
                     Dungettok;
                     PTN_NEW_NODE(nn, PTN_PrintStat)(Dgetparami(0), popnode.expr, Dgetparami(1), Dgetparami(2));
                     setPNode(nn);
                     popState();
                     break;
               }
               break;

            case ST_FLUSH:
               switch(state_index)
               {
                  case 1:
                     if(tok!=TOKEN_BEGEXPR)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 2:
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     PTN_NEW_NODE(nn, PTN_FlushStat)(popnode.expr);
                     setPNode(nn);
                     popState();
                     break;
               }
               break;

            case ST_CLASSMETHODBODY: // [mynamespace::] MyClass::myMethod(..) { .. }
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     if(NULL == cnspace)
                     {
                        cclassdecl = Dtkscompiler->findClassDeclNamespaceList(sv, active_namespaces);
                     }
                     else
                     {
                        cclassdecl = Dtkscompiler->findClassDeclNamespace(sv, cnspace);
                        cnspace = NULL;
                     }
                     if(!cclassdecl)
                     {
#ifdef TKS_DCON
                        if(NULL != cnspace)
                        {
                           Dprintf("\n[---] unknown class \"%s::%s\".\n",
                                   (char*)cnspace->name.chars,
                                   (char*)sv->chars
                                   );
                        }
                        else
                        {
                           Dprintf("\n[---] unknown class \"%s\".\n",
                                   (char*)sv->chars
                                   );
                        }
#endif
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     if(tok!=TOKEN_MEMBER)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 2:
                     if(tok!=TOKEN_VAR)
                     {
                        if(tok==TOKEN_BITNOT)
                        {
                           // defining body of destructor
                           nextisdestructorname=1;
                           break;
                        }
                        else
                        {
                           Dparseerror;
                        }
                     }
                     else
                     {
                        ctempname.yacCopy(sv);
                        if(nextisdestructorname)
                        {
                           ctempname.insert(0, "~");
                           nextisdestructorname=0;
                        }
                        PTN_Function *f = cclassdecl->findFunction(&ctempname);
                        if(f)
                        {
                           // ---- implementation for class function ("static method")
                           if(f->body)
                           {
                              Dprintf("\n[---] function \"%s\" of class \"%s\" already has a body.\n",
                                      (char*)ctempname.chars, (char*)cclassdecl->name.chars);
                              Dparseerror;
                           }
                           else
                           {
                              cfunction=f;
                              Dungettok;
                              Dpushstate;
                              state=ST_UFUNBODY;
                              state_index=1;
                           }

                        }
                        else
                        {
                           // ---- implementation for class method
                           TKS_ClassDeclMethod *m=cclassdecl->findMethod(&ctempname);
                           if(!m)
                           {
                              Dprintf("\n[---] class \"%s\" has no method named \"%s\"- cannot assign body.\n",
                                      (char*)cclassdecl->name.chars, (char*)ctempname.chars);
                              Dparseerror;
                           }
                           if(m->function.body)
                           {
                              Dprintf("\n[---] method \"%s\" of class \"%s\" already has a body.\n",
                                      (char*)ctempname.chars, (char*)cclassdecl->name.chars);
                              Dparseerror;
                           }
                           cfunction = &m->function;
                           Dungettok;
                           Dpushstate;
                           state=ST_UFUNBODY;
                           state_index=1;
                        }
                     }
                     break;
                  case 3:
                     // fun body finished
                     Dungettok;
                     cclassdecl=0;
                     //cfunction->fun_vars.optimize();
                     cfunction=0;
                     delete popnode.node;
                     PTN_NEW_NODE(nn, PTN_ClassMethodBody)();
                     setPNode(nn);
                     popState();
                     break;
               }
               break;

            case ST_CLASSDECL:
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_CLASS)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }

                     cclassdecl = Dtkscompiler->addClassDecl(sv, this, Dsrcloc, (NULL != tmod_class_nsp) ? tmod_class_nsp : create_namespace);

                     if(
                        (ntok==TOKEN_TELSE)||
                        (ntok==TOKEN_EXTENDS)
                        )
                        state_index=7; // ":" ancestors
                     else
                     {
                        state_index++;
                     }
                     static_flag=0;
                     break;
                  case 2:
                     if(tok!=TOKEN_BEGBLOCK)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     b_expect_method_name = 0;
                     break;
                  case 3:
                     b_var_ref=0; // [22Jun2020]
                     // // b_member_ref=0;
                     is_array=0;
                     b_pointerarray=0;
                     if(tok==TOKEN_ENDBLOCK)
                     {
                        // classdecl finished
                        PTN_NEW_NODE(nn, PTN_ClassDecl)(cclassdecl);
                        setPNode(nn);
                        cclassdecl=0;
                        popState();
                     }
                     else
                     {
                        switch(tok)
                        {
                           default:
                              Dparseerror;
                              break;
                           case TOKEN_EXPLAIN:
                              state_index=80;
                              break;
                           case TOKEN_DEFINE:
                              state_index=200; // parse class constant "define x y = <expr>" or "define exception .."
                              break;
                           case TOKEN_FUNCTION:
                              state_index=100;
                              break;
                           case TOKEN_STATIC:
                              static_flag=1;
                              break;
                           case TOKEN_METHOD:
                              b_expect_method_name = 1;
                              if(static_flag)
                                 state_index=100;
                              break;
                           case TOKEN_ASSIGN: // =, =mymutex=
                              if(nntok==TOKEN_ASSIGN)
                                 state_index=95; // consume mutex name
                              //else just consume token (already parsed in scanClasses())
                              break;
                           case TOKEN_CMPEQ: // == , synchronize method on "this"
                              // just consume token
                              break;
                           case TOKEN_TAG:
                           case TOKEN_PUBLIC:
                           case TOKEN_PRIVATE:
                           case TOKEN_PROTECTED:
                           case TOKEN_MODULE:
                           case TOKEN_NAMESPACE:
                           case TOKEN_VIRTUAL:
                           case TOKEN_ABSTRACT:
                              break;
                           case TOKEN_INT:
                           case TOKEN_FLOAT:
                           case TOKEN_STRING:
                           case TOKEN_OBJECT:
                              state_index++;
                              break;
                           case TOKEN_BITNOT:
                              if(ntok!=TOKEN_VAR)
                              {
                                 Dparseerror;
                              }
                              nextisdestructorname=1;
                              break;
                           case TOKEN_MEMBER:
                              Ddefaultnspace;
                              break;

                           case TOKEN_VAR:

                              Dparsenspace(14);
                              cnspace = NULL; // already handled in scanner pass

                              ctempname.yacCopy(sv);
                              if(nextisdestructorname)
                              {
                                 if(ntok!=TOKEN_BEGEXPR)
                                 {
                                    Dparseerror;
                                 }
                                 ctempname.insert(0, "~");
                                 nextisdestructorname=0;

                              }
                              cfunction=0;
                              if(b_expect_method_name||(ntok==TOKEN_BEGEXPR)||(ntok==TOKEN_BEGBLOCK)||(ntok==TOKEN_TELSE)||(ntok==TOKEN_RETURN)||(ntok==TOKEN_COMPILE)) // "mymethod(.."
                              {
                                 b_expect_method_name=0;
                                 //Dungettok;
                                 //Dsetparams(0, &ctempname); // cannot be nested
                                 if(static_flag)
                                 {
                                    cfunction = cclassdecl->findFunction(&ctempname);
                                    if(!cfunction)
                                    {
                                       Dprintf("\n[---] cannot find function \"%s\" in class \"%s\".\n",
                                               (char*)ctempname.chars, (char*)cclassdecl->name.chars);
                                       Dparseerror;
                                    }
                                 }
                                 else
                                 {
                                    TKS_ClassDeclMethod *meth=cclassdecl->findMethod(&ctempname);
                                    if(!meth)
                                    {
                                       Dprintf("\n[---] cannot find method \"%s\" in class \"%s\".\n",
                                               (char*)ctempname.chars, (char*)cclassdecl->name.chars);
                                       Dparseerror;
                                    }
                                    cfunction = &meth->function;
                                 }
                                 //state_index=5;
                                 //Dpushstate;
                                 //Dsetparamn(0, meth->function);
                                 //state=ST_UFUNDECL;
                                 //state_index=1;
                                 if(ntok==TOKEN_BEGBLOCK)
                                 {
                                    state_index=101;
                                    // --- parse function body
                                    Dpushstate;
                                    state=ST_SEQ;
                                    state_index=0;
                                 }
                                 else if(ntok==TOKEN_COMPILE)
                                 {
                                    state_index=91;
                                 }
                                 else
                                 {
                                    state_index=101;
                                 }

                                 // Begin function SEQ
                              }
                              else
                              {
                                 // Parse regular member decl
                                 state_index++;
                              }
                              break;
                        }
                     }
                     break;
                  case 31: // consume first ']' in int[]*aoa_int[]
                     if(TOKEN_ENDARRAY != tok)
                     {
                        Dparseerror;
                     }
                     state_index = 4;
                     break;
                  case 32: // consume first '[' in int*[]*aoa_int[]
                     if(TOKEN_BEGARRAY != tok)
                     {
                        Dparseerror;
                     }
                     state_index = 31;
                     break;
                  case 4:
                     if(TOKEN_BEGARRAY == tok)
                     {
                        is_array = 1;
                        // '*' follows after ']' (error handling already done in scanClasses())
                        state_index = 31;
                        break;
                     }
                     if(TOKEN_MUL == tok)
                     {
                        if(TOKEN_BEGARRAY == ntok)
                        {
                           b_pointerarray = 1;
                           state_index = 32;
                           break;
                        }
                        // Consume '*' in SomeClass*ref_member
                        break;
                     }
                     // ---- <type> a,b,c;
                     if(tok!=TOKEN_VAR) // expect member name
                     {
                        Dparseerror;
                     }
                     ctempname2=sv; // re-member name
                     if(ntok==TOKEN_BEGARRAY) state_index=10;
                     else state_index++;
                     break;
                  case 5:
                     // "int i;" finished, now check for initializer '='
                     if(tok==TOKEN_LISTSEP)
                     {
                        state_index=4;
                     }
                     else if(tok==TOKEN_TERM)
                     {
                        state_index=3;
                        static_flag=0;
                     }
                     else if((tok==TOKEN_ASSIGN)||(tok==TOKEN_CMPLE))
                     {
                        // ---- parse constant initializer expression
                        state_index=300;
                        Dpushstate;
                        state_index=0;
                        state=ST_EXPR;
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 6:
                     // methoddecl finished
                     if(popnode.node->getID()==PTN_FUNCTIONBODY)
                     {
                        // allow optional ";" at end of function body implementation
                        if(tok==TOKEN_TERM) {
                           state_index=3; // consume ";"
                           static_flag=0;
                           cfunction=0;
                        }
                        else
                        {
                           Dungettok;
                           state_index=3;
                           static_flag=0;
                           cfunction=0;
                        }
                     }
                     else
                     {
                        if(tok!=TOKEN_TERM)
                        {
                           Dparseerror;
                        }
                        else {
                           state_index=3;
                           static_flag=0;
                           cfunction=0;
                        }
                     }
                     break;
                  case 7:
                     if( (tok!=TOKEN_TELSE)&&
                         (tok!=TOKEN_EXTENDS)
                         )
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 8: // ancestor list
                     if(TOKEN_MEMBER == ntok)
                     {
                        state_index = 13;
                        break;
                     }
                     else if(TOKEN_MEMBER == tok)
                     {
                        break; // skip '::' in ::MyDefaultNamespaceBaseClass
                     }
                     if(TOKEN_VAR != tok)
                     {
                        Dparseerror;
                     }
                     // consume ancestor name
                     state_index++;
                     break;
                  case 9:
                     if(tok==TOKEN_LISTSEP)
                     {
                        state_index=8;
                     }
                     else
                     {
                        if(tok!=TOKEN_BEGBLOCK) // "{"
                        {
                           Dparseerror;
                        }
                        else {
                           state_index=3;
                           static_flag=0;
                        }
                     }
                     break;
                  case 10:
                     // consume array []
                     if(tok!=TOKEN_BEGARRAY)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 11:
                     if(tok!=TOKEN_ENDARRAY)
                     {
                        if(tok!=TOKEN_VAR)
                        {
                           Dparseerror;
                        }
                        state_index++;
                     }
                     else
                        state_index=5;
                     break;
                  case 12:
                     if(tok!=TOKEN_ENDARRAY)
                     {
                        Dparseerror;
                     }
                     state_index=5;
                     break;
                  case 13: // skip '::' between namespace and baseclassname
                     state_index = 8;
                     break;
                  case 14: // skip '::' between namespace and classname
                     state_index = 3;
                     break;

                  case 80: // skip documentation text (already handled in scanner-pass)
                     state_index++;
                     break;
                  case 81: // skip ';'
                     state_index = 3;
                     break;

                  case 91:
                     // tok==TOKEN_COMPILE
                     state_index=101;
                     // ---- parse JITed function body
                     Dpushstate;
                     state=ST_COMPILE;
                     state_index=1;
                     break;

                  case 95:
                     state_index=3;// just consume mutex name (in "=<mutexname>="); continue class parsing
                     break;

                  case 98:
                     if(tok!=TOKEN_VAR) // expect function name
                     {
                        Dparseerror;
                     }
                     else
                     {
                        TKS_ClassDeclMethod *meth=cclassdecl->findMethod(sv);
                        if(!meth)
                        {
                           Dprintf("\n[---] cannot find method \"%s\" in class \"%s\".\n",
                                   (char*)sv->chars, (char*)cclassdecl->name.chars);
                           Dparseerror;
                        }
                     }
                     state_index+=2;
                     break;
                  case 100:
                     if(tok!=TOKEN_VAR) // expect function name
                     {
                        Dparseerror;
                     }
                     b_expect_method_name=0;//[02Oct2023]
                     cfunction=cclassdecl->findFunction(sv);
                     state_index++;
                     break;
                  case 101:
                  {
                     // ---- skip forward until next occurence of ";" or "{"
                     sUI ri=0;
                     sUI ti=toki+1;
                     while(ti<num_tok)
                     {
                        tok=stok[ri++];
                        ti++;
                        if(tok==TOKEN_TERM) // just forward decl
                        {
                           state_index=3; // parse next class member/method/function
                           static_flag=0;
                           cfunction=0;
                           break;
                        }
                        else
                           if(tok==TOKEN_BEGBLOCK)
                           {
                              // --- parse function body
                              Dpushstate;
                              state=ST_SEQ;
                              state_index=0;
                              ri--;
                              //cclassdecl_save=cclassdecl;
                              //cclassdecl=0; // class functions may not see class members
                              break;
                           }
                           else
                              if(tok==TOKEN_COMPILE)
                              {
                                 // ---- parse JITed function body
                                 Dpushstate;
                                 state=ST_COMPILE;
                                 state_index=1;
                                 //cclassdecl_save=cclassdecl;
                                 //cclassdecl=0; // class functions may not see class members
                                 break;
                              }
                              else
                                 if(
                                    (tok==TOKEN_ENDBLOCK)||
                                    (tok==TOKEN_DEFINE)||
                                    (tok==TOKEN_FUNCTION)||
                                    (tok==TOKEN_METHOD)||
                                    (tok==TOKEN_PUBLIC)||
                                    (tok==TOKEN_PRIVATE)||
                                    (tok==TOKEN_PROTECTED)||
                                    (tok==TOKEN_MODULE)
                                    )
                                 {
                                    tkscript->printf("\n[---] reaching EOF while looking for ';', 'compile' or '{'");
                                    Dparseerror;
                                 }
                                 else
                                    if(tok<=TOKEN_STRINGVAL)
                                    {
                                       ri++; // consume string index
                                       ti++;
                                    }
                     }
                     if(ti>=num_tok)
                     {
                        Dprintf("\n[---] reaching EOF while looking for ';', 'compile' or '{'");
                        Dparseerror;
                     }
                     stok+=ri;
                     toki+=ri;
                  }
                  break;
                  case 102:
                     // ---- class function body finished
                     cfunction->body=popnode.statement;
                     popnode.statement=0;
                     cfunction=0;
                     static_flag=0;
                     // ---- consume optional ';' at end of function/method body
                     if(tok!=TOKEN_TERM)
                     {
                        Dungettok;
                     }
                     state_index=3;
                     break;

                  case 200: // int,float,Object,String constant type
                     if(tok==TOKEN_VAR)
                     {
                        // ---- consume 'exception'
                        state_index=250;
                     }
                     else
                     {
                        // ---- parse class constant
                        state_index++; // consume type
                     }
                     break;
                  case 201: // constant name
                     // ---- lookup class constant
                     // tkscript->printf("xxx case 201: cclassdecl->findConstant(cclassdecl=\"%s\" sv=\"%s\") => %p\n", cclassdecl->name.chars, sv->chars, (void*)cclassdecl->findConstant(sv));
                     Dsetparamv(0, (void*)cclassdecl->findConstant(sv));
                     state_index++;
                     break;
                  case 202: // '='
                     if(TOKEN_TERM == tok)
                     {
                        // define String ACTION_TEXTENTERED;
                        state_index=3; // continue class parsing
                        static_flag=0;
                     }
                     else
                     {
                        pushState();
                        state=ST_EXPR;
                        state_index=0;
                     }
                     break;
                  case 203:
                     // initializer <expr> finished
                  {
                     // (TODO) the initializer expr may itself contain references to other class constants
                     //        which might not have been assigned values, yet.
                     //        - use PTN_ClassConstVal instead of PTN_ConstVal
                     //        - add CLASSCONSTUNINITIALIZED flag to CO
                     //        - add classconstinitializer entry (co, expr)
                     //        - before semanticCheckAll(), evaluate const initializers until
                     //          no expr isConst() anymore
                     //           - raise circular const. dependency error if #isConst has not decreased
                     //        --> trouble is, the parser usually checks isConst() immediately, this
                     //            would have to be removed to the respective node semanticCheck(), first.

                     co=(TKS_CachedObject*)Dgetparamv(0); // get constant
                     if(!popnode.expr->isConst())
                     {
                        Dprintf("[---] class constant \"%s::%s\" initializer expr must be constant/resolvable.\n",
                                (char*)cclassdecl->name.chars,
                                (char*)co->name.chars);
                        Dparseerror;
                     }
                     else
                     {// ---- evaluate class constant expr
                        YAC_Value r;
                        if(!popnode.expr->semanticCheck())
                        {
                           Dprintf("[---] semantic check failed for class constant \"%s::%s\".\n",
                                   (char*)cclassdecl->name.chars,
                                   (char*)co->name.chars);
                        }
                        if(!popnode.expr->resolveXRef())
                        {
                           Dprintf("[---] resolveXRef() failed for class constant \"%s::%s\".\n",
                                   (char*)cclassdecl->name.chars,
                                   (char*)co->name.chars);
                        }
                        popnode.expr->optimize();
                        PTN_Env env; env.initDefault();
                        popnode.expr->eval(&env, &r);
                        co->derefPointerAssignValue(&r); // grab r/w object
                        if(co->type>=YAC_TYPE_OBJECT)
                        {
                           if(!co->deleteme)
                           {
                              // --- clone constant object
                              YAC_Object *o=co->value.object_val;
                              co->value.object_val = YAC_CLONE_POOLED(env.context, o);
                              co->value.object_val->yacOperatorAssign(o);
                              co->deleteme=1;
                           }
                        }

                        // Flag class constant as initialized
                        co->flags &= ~TKS_CachedObject::ISCLASSCONSTUNINITIALIZED;

                        r.unset();
                        delete popnode.expr;
                        popnode.expr=0;
                        if(tok!=TOKEN_TERM)
                        {
                           Dparseerror;
                        }
                        state_index=3;
                        static_flag=0;
                     }
                  }
                  break; // class constant finished

                  case 250: // consume until ';' (class exception has already been parsed in pass1)
                     if(tok==TOKEN_TERM)
                     {
                        state_index=3; // continue class parsing
                        static_flag=0;
                     }
                     // else stay in 250 until ';' is encountered
                     break;

                  case 301:
                     // ---- constant member initializer expression finished
                     if(popnode.expr->semanticCheck())
                     {
                        if(popnode.expr->resolveXRef())
                        {
                           TKS_ClassDeclMember *m=cclassdecl->findMemberSuper(ctempname2, static_flag);
                           sUI rtti=m->rtti;
                           if(rtti>YAC_TYPE_STRING) // int,float,string,object,...class internal obj ids...
                              rtti=YAC_TYPE_OBJECT;
                           tks_optimize_expr(&popnode.expr, 0);
                           if(0/*allow arbitrary init expr.*/&&!popnode.expr->isConst())
                           {
                              tkscript->printf("[---] class member \"%s::%s\" initializer expression must be constant (type=%s)!\n",
                                               (char*)cclassdecl->name.chars,
                                               (char*)ctempname2->chars,
                                               tks_typenames[rtti]
                                               );
                              Dparseerror;
                           }
                           else
                           {
                              // ---- expression is ok, store in TKS_ClassDeclMember
                              m->init_expr=popnode.expr;
                              popnode.expr=0;
                           }
                        }
                        else
                        {
                           tkscript->printf("[---] class member \"%s::%s\" initializer expression resolveXRef() failed.\n",
                                            (char*)cclassdecl->name.chars,
                                            (char*)ctempname2->chars
                                            );
                           Dparseerror;
                        }
                     }
                     else
                     {
                        tkscript->printf("[---] class member \"%s::%s\" initializer expression semanticCheck() failed.\n",
                                         (char*)cclassdecl->name.chars,
                                         (char*)ctempname2->chars
                                         );
                        Dparseerror;
                     }
                     if(tok==TOKEN_TERM)
                     {
                        state_index=3;
                        static_flag=0;
                     }
                     else if(tok==TOKEN_LISTSEP)
                     {
                        state_index=4;
                     }
                     else
                     {
                        Dparseerror;
                     }
                     // ----
                     break;
               }
               break;

            case ST_MODULE:
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_MODULE)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     name.yacCopy(sv);
                     PTN_NEW_NODE(nn, PTN_ModuleDef)();
                     setPNode(nn);
                     popState();
                     break;
                  default:
                     Dparseerror;
                     break;
               }
               break;

            case ST_DEFINE:
               switch(state_index)
               {
                  case 0:
                     state_index++;
                     break;
                  case 1:
                     if(sv->compare(&tok_exception))
                     {
                        state_index = 50;
                     }
                     else
                     {
                        state_index++;
                     }
                     break;
                  case 2:
                     if(tok==TOKEN_SUB)
                     {
                        state_index++;
                     }
                     else
                     {
                        PTN_NEW_NODE(nn, PTN_Define)();
                        setPNode(nn);
                        popState();
                     }
                     break;
                  case 3:
                     PTN_NEW_NODE(nn, PTN_Define)();
                     setPNode(nn);
                     popState();
                     break;

                  case 50: // define exception <name> [extends <basename>]
                     // consume <name>
                     if(ntok == TOKEN_TERM)
                     {
                        PTN_NEW_NODE(nn, PTN_Define)();
                        setPNode(nn);
                        popState();
                     }
                     else
                     {
                        // Loop state until ';' is the next token
                        // The actual parsing has already been done in the scanner pass
                     }
                     break;
               }
               break;

            case ST_CONSTRAINT:
               switch(state_index)
               {
                  case 1:
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        Dpushstate;
#ifdef DX_SCRIPTCONSTRAINTS
                        current_constraint=tkscript->findScriptConstraint(sv); // added in pass1
#endif // DX_SCRIPTCONSTRAINTS
                        state=ST_EXPR;
                        state_index=0;
                     }
                     break;
                  case 2:
#ifdef DX_SCRIPTCONSTRAINTS
                     current_constraint->constraint_expr=popnode.expr;
#else
                     delete popnode.expr;
#endif // DX_SCRIPTCONSTRAINTS

                     if(tok==TOKEN_VAR)
                     {
#ifdef DX_SCRIPTCONSTRAINTS
                        // ---- parse constraint comment
                        current_constraint->comment.yacCopy(sv);
#endif // DX_SCRIPTCONSTRAINTS
                        state_index++;
                     }
                     else if(tok!=TOKEN_TERM)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        Dungettok;
#ifdef DX_SCRIPTCONSTRAINTS
                        current_constraint=0;
#endif // DX_SCRIPTCONSTRAINTS
                        PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                        popnode.statement = (PTN_Statement*)nn;
                        Dpopstate;
                     }
                     break;
                  case 3:
                     if(tok!=TOKEN_TERM)
                     {
                        Dparseerror;
                     }
                     else
                     {
#ifdef DX_SCRIPTCONSTRAINTS
                        current_constraint = NULL;
#endif // DX_SCRIPTCONSTRAINTS
                        Dungettok;
                        PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                        popnode.statement = (PTN_Statement*)nn;
                        Dpopstate;
                     }
                     break;

               }
               break;

            case ST_ENUM:
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_ENUM)
                     {
                        Dparseerror;
                     }
                     Dsetparami(1, 0); // current enumeration value
                     state_index++;
                     break;
                  case 1:
                     if(tok!=TOKEN_BEGBLOCK)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 2:
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     sv->resubstEscapeSeq();
                     Dsetparams(0, sv); // save enumerated constant name
                     //if(ntok==TOKEN_LISTSEP||ntok==TOKEN_ENDBLOCK)
                     if(ntok!=TOKEN_ASSIGN)
                     {
                        // skip const expr initializer
                        state_index=5;
                     }
                     else state_index++;
                     break;
                  case 3:
                     if(tok!=TOKEN_ASSIGN)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 4:
                     // ST_EXPR finished
                     if(!popnode.expr->isConst())
                     {
                        Dprintf("\n[---] enum: trying to use non-constant initializer for enum-constant \"%s\".\n",
                                (char*)Dgetparams(0)->chars);
                        Dparseerror;
                     }
                     if(!popnode.expr->semanticCheck())
                     {
                        Dprintf("\n[---] enum: semanticCheck() failed for enum-constant \"%s\".\n",
                                (char*)Dgetparams(0)->chars);
                        Dparseerror;
                     }
                     if(!popnode.expr->resolveXRef())
                     {
                        Dprintf("\n[---] enum: resolveXRef() failed for enum-constant \"%s\".\n",
                                (char*)Dgetparams(0)->chars);
                        Dparseerror;
                     }
                     Dungettok;
                     {
                        YAC_Value r;
                        popnode.expr->optimize();
                        PTN_Env env; env.initDefault();
                        popnode.expr->eval(&env, &r);
                        if(r.type!=1)
                        {
                           Dprintf("\n[---] enum: trying to use non-integer type initializer for enum-constant \"%s\".\n",
                                   (char*)Dgetparams(0)->chars);
                           Dparseerror;
                        }
                        Dsetparami(1, r.value.int_val);
                        delete popnode.expr;
                        popnode.expr=0;
                        state_index++;
                     }
                     break;
                  case 5:
                  {
                     // ---- enumeration entry finished
                     YAC_String t;
                     t.yacValueOfI((sSI)(Dgetparami(1)));
                     co=findConst(Dgetparams(0));
                     if(co)
                     {
                        Dprintf("\n[---] enum: enumeration value \"%s\" already used.\n",
                                (char*)Dgetparams(0)->chars);
                        Dparseerror;
                     }
                     addConstant(Dgetparams(0), &t);
                  }
                  if(tok==TOKEN_ENDBLOCK)
                  {
                     PTN_NEW_NODE(nn, PTN_EmptyStatementNoTerm)();
                     setPNode(nn); // return empty statement
                     popState();
                  }
                  else
                  {
                     if(tok!=TOKEN_LISTSEP)
                     {
                        //Dparseerror;
                        Dungettok;
                     }
                     state_index=2; // loop
                     Dsetparami(1, Dgetparami(1)+1);
                  }
                  break;
               }
               break;

            case ST_PTRASSIGN:
               switch(state_index) // note: sv and cnspace must have been set before this state is entered
               {
                  /*case 0:
                    if(tok!=TOKEN_VAR)
                    {
                    Dparseerror;
                    }
                    Dsetparams(0, sv);
                    state_index++;
                    break;*/
                  case 1:
                     Dsetparams(0, sv);
                     Dsetparamnspace(1, cnspace);
                     cnspace = NULL;
                     if(tok!=TOKEN_CMPLE)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 2:
                     Dungettok;
                     {
                        sv = Dgetparams(0); // Get varname
                        cnspace = Dgetparamnspace(1);

                        co = NULL;

                        if(NULL == cnspace)
                        {
                           if(!b_force_this)
                           {
                              co = findVarLocal(sv);
                              if(co)
                              {
                                 if(co->flags&TKS_CachedObject::ISDYNAMIC)
                                 {
                                    PTN_NEW_NODE(nn, PTN_DynamicVarPointerAssign)(co, popnode.expr);
                                    setPNode(nn);
                                 }
                                 else
                                 {
                                    PTN_NEW_NODE(nn, PTN_PointerAssign)(co, popnode.expr);
                                    setPNode(nn);
                                 }
                              }
                           }
                           else
                           {
                              co = NULL;
                           }

                           if( (!co) && cclassdecl)
                           {
                              co=cclassdecl->getMember(sv);
                              if(!b_force_this)
                              {
                                 DcheckFunMemberAccess(sv);
                              }
                              if(co)
                              {
                                 b_force_this = 0;
                                 DcheckMemberAccess(cclassdecl, sv);
                                 PTN_NEW_NODE(nn, PTN_ClassMemberPointerAssign)(0, co, popnode.expr);
                                 setPNode(nn);
                                 popnode.classmemberpointerassign->class_id=cclassdecl->class_id;
                              }
                              else if(b_force_this)
                              {
                                 Dprintf("[---] class \"%s\" has no member called \"%s\".\n",
                                         (char*)cclassdecl->name.chars,
                                         (char*)sv->chars);
                                 Dparseerror;
                              }
                           }
                        }

                        if(!co)
                        {
                           co = findVar(sv, active_namespaces, cnspace);
                           if(!co)
                           {
                              if(tkscript->configuration.b_enable_unresolved_members)
                              {
                                 ///YAC_String t;
                                 //t="Main";
                                 //setPNode(new PTN_UnresolvedModuleMemberPointerAssign(&t, sv, popnode.expr));
                                 PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberPointerAssign)(&name, 1, sv, popnode.expr);
                                 setPNode(nn);
                                 Dfreezenspaces(popnode.unresolvedmodulememberpointerassign->nspace_array);
                                 cnspace = NULL;
                              }
                              else
                              {
                                 Dprintf("[---] cross reference resolving for identifier \"%s\" is turned OFF.\n", sv->chars);
                                 Dparseerror;
                              }
                           }
                           else
                           {
                              cnspace = NULL;
                              if(co->flags&TKS_CachedObject::ISDYNAMIC)
                              {
                                 PTN_NEW_NODE(nn, PTN_DynamicVarPointerAssign)(co, popnode.expr);
                                 setPNode(nn);
                              }
                              else
                              {
                                 PTN_NEW_NODE(nn, PTN_PointerAssign)(co, popnode.expr);
                                 setPNode(nn);
                              }
                           }
                        }
                        cnspace = NULL;
                        popState();
                     }
                     break;
               }
               break;

            case ST_DYNARRAYDECL: // xxx TODO either remove this or support it in classes, too
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_ARRAY)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     if(tok!=TOKEN_CMPLT)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 2:
                  {
                     YAC_Object *a=0;
                     switch(tok)
                     {
                        default:
                           Dprintf("\n[---] dynamic array decl: expected ClassName .\n");
                           Dparseerror;
                           break;
                        case TOKEN_VAR:
                        {
                           TKS_ClassTemplate *clt=tkscript->findTemplate(sv);
                           if(!clt)
                           {
                              TKS_ClassDecl *cd=Dtkscompiler->findClassDeclNamespace(sv, NULL);
                              if(cd)
                              {
                                 a=YAC_NEW_CORE(YAC_CLID_CLASSARRAY);
                                 ((YAC_ClassArray*)a)->class_decl=cd;
                              }

                              else
                              {
                                 Dprintf("\n[---] dynamic array decl: unknown Class \"%s\".\n",
                                         (char*)(sv?(char*)sv->chars:"**illClassName**"));
                                 Dparseerror;
                              }
                           }
                           else
                           {
                              a=clt->template_object->yacArrayNew();
                           }
                        }
                        break;
                        case TOKEN_INT:
                           a= YAC_NEW_CORE(YAC_CLID_INTARRAY);
                           break;
                        case TOKEN_FLOAT:
                           a= YAC_NEW_CORE(YAC_CLID_FLOATARRAY);
                           break;
                        case TOKEN_STRING:
                           a= YAC_NEW_CORE(YAC_CLID_STRINGARRAY);
                           break;
                        case TOKEN_OBJECT:
                           a= YAC_NEW_CORE(YAC_CLID_OBJECTARRAY);
                           break;
                     }
                     if(!a)
                     {
                        Dprintf("\n[---] failed to create array object.\n");
                        Dparseerror;
                     }
                     Dsetparamv(0, (void*)a);
                  }
                  state_index++;
                  break;
                  case 3:
                     if(tok!=TOKEN_CMPGT)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 4:
                     if(tok!=TOKEN_VAR)
                     {
                        Dprintf("\n[---} dynamic array decl: expected array variable name.\n");
                        Dparseerror;
                     }
                     {
                        YAC_Object *a=(YAC_Object *)Dgetparamv(0);
                        if(cfunction)
                        {
                           co = cfunction->getCreateFunVars()->createObjectEntry(sv, 1, a, 1);
                        }
                        else
                        {
                           co = Dglobalvars.createObjectEntry(sv, 1, a, 1);
                        }
                        if(!co)
                        {
                           Dprintf("\n[---] failed to create array variable \"%s\".\n",
                                   (char*)sv->chars);
                           Dparseerror;
                        }
                        co->flags|=Dgetparami(2);
                        if(state_stack[stp].state==ST_SSEQSTAT)
                        {
                           if((nntok==TOKEN_ENDBLOCK)||((toki+2)>=num_tok))
                           {
                              PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                              popnode.statement = (PTN_Statement*)nn;
                              popState();
                           }
                           else
                           {
                              popnode.statement=0;
                              popState();
                              state_index=2;
                           }
                        }
                        else
                        {
                           PTN_NEW_NODE(nn, PTN_VarDecl)(co, 0);
                           setPNode(nn); // xxx skip expression
                           popState();
                        }
                     }
                     break;
               }
               break;

            case ST_UFUNEVENT: // use <function> for <obj>.<signalname>
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_USE)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     if(TOKEN_NAMESPACE == tok) // use namespace 'mynamespace'
                     {
                        state_index = 7;
                        break;
                     }
                     else if(tok!=TOKEN_VAR)
                     {
                        Dprintf("\n[---] expected function name or \"callbacks\" keyword.\n");
                        Dparseerror;
                     }
                     if(sv->compare(&tok_callbacks)) // use callbacks;
                     {
                        // ---- special keyword, autobind signal handlers ---
                        PTN_NEW_NODE(nn, PTN_FunctionSignalStatement)();
                        setPNode(nn);
                        popState();
                        break;
                     }
                     else if(ntok==TOKEN_TERM)
                     {
                        // use <pluginname>;
                        PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                        setPNode(nn);
                        popState();
                     }
                     else
                     {
                        // ---- use <callback> for <obj>.<signal>; syntax ----
                        Dsetparams(0, sv);
                        state_index++;
                     }
                     break;

                  case 2:
                     if(tok!=TOKEN_FOR)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 3:
                     Dparsenspace(6);

                     if(tok!=TOKEN_VAR)
                     {
                        Dprintf("\n[---] expected Object name.\n");
                        Dparseerror;
                     }
                     Dsetparams(1, sv);
                     state_index++;
                     break;
                  case 6: // skip '::' between namespace and objname
                     state_index = 3;
                     break;

                  case 4:
                     if(tok!=TOKEN_DOT)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 5:
                     if(tok!=TOKEN_VAR)
                     {
                        Dprintf("\n[---] expected signal name.\n");
                        Dparseerror;
                     }
                     {
                        // Resolve function
                        YAC_String *funname = Dgetparams(0);
                        PTN_Function *fun = findFunction(funname);

                        if(!fun)
                        {
                           Dprintf("\n[---] ST_UFUNEVENT: function \"%s\" not declared.\n",
                                   (char*)(funname?(char*)funname->chars:"**empty funname**"));
                           Dparseerror;
                        }

                        // Resolve variable or C++ class template object
                        YAC_String *objname = Dgetparams(1);

                        co = findVar(objname, active_namespaces, cnspace);
                        if( !co && (NULL == cnspace) )
                        {
                           TKS_ClassTemplate *clt = tkscript->findTemplate(objname);
                           if(clt)
                           {
                              // Use C++ template object
                              co = &clt->static_co;
                           }
                        }
                        if(!co)
                        {
#ifdef TKS_DCON
                           if(NULL != cnspace)
                           {
                              Dprintf("\n[---] unknown reference to variable \"%s::%s\".\n",
                                      (char*) cnspace->name.chars,
                                      (char*)(objname?(char*)objname->chars:"**ill objname**"));
                           }
                           else
                           {
                              Dprintf("\n[---] unknown reference to variable or C++ class \"%s\".\n",
                                      (char*)(objname?(char*)objname->chars:"**ill objname**"));
                           }
#endif
                           Dparseerror;
                        }
                        if(!co->value.object_val)
                        {
                           Dprintf("\n[---] cannot bind signal to empty (Pointer) object \"%s\".\n",
                                   (char*)objname->chars);
                           Dparseerror;
                        }

                        YAC_Object *obj=co->value.object_val;
                        // find sigid by name
                        YAC_String cln; cln.visit((char*)obj->yacClassName());
                        TKS_ClassTemplate *clt=tkscript->findTemplate(&cln);
                        if(!clt)
                        {
                           Dprintf("\n[---] internal error: unknown Class \"%s\".\n",
                                   (char*)cln.chars);
                           Dparseerror;
                        }
                        sU8 i;
                        sU8 sigid=TKS_MAXSIGNALS;
                        for(i=0; i<TKS_MAXSIGNALS; i++)
                        {
                           if(clt->signals[i].name.compare(sv))
                           {
                              sigid=i;
                              i=TKS_MAXSIGNALS;
                           }
                        }
                        if(sigid==TKS_MAXSIGNALS) // else TKS_MAXSIGNALS+1 if found
                        {
                           Dprintf("\n[---] unknown signal \"%s\".\n",
                                   (char*)(sv->chars?(char*)sv->chars:"**illSigName**"));
                           Dparseerror;
                        }
                        PTN_NEW_NODE(nn, PTN_FunctionSignalStatement)(fun->getOrCreateFunctionObject()/*, obj*/, sigid, co);
                        setPNode(nn);
                        popState();
                        cnspace = NULL;
                     }
                     break;

                  case 7: // use namespace 'mynamespace'; or use namespace default;
                     if(TOKEN_DEFAULT == tok)
                     {
                        useDefaultNamespace();
                     }
                     else if(TOKEN_VAR != tok)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        TKS_Namespace *ns = tkscript->findNamespace(sv); // namespace must exist since scanner pass already used it
                        useNamespace(ns);
                     }
                     PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                     setPNode(nn);
                     popState();
                     break;
               }
               break;

            case ST_NAMESPACE:
               // namespace default;
               // namespace <mynamespacename>;
               switch(state_index)
               {
                  case 1:
                     if(TOKEN_DEFAULT == tok)
                     {
                        // Switch back to default namespace / clear search list
                        useDefaultNamespace();
                        selectNamespace(NULL);
                        if(TOKEN_TERM != ntok)
                        {
                           Dparseerror;
                        }
                        else
                        {
                           PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                           setPNode(nn);
                           popState();
                        }
                     }
                     else if(TOKEN_VAR == tok)
                     {
                        // Check whether the namespace has been declared (paranoia, should always succeed since it has
                        // been added in the scanner pass)
                        TKS_Namespace *ns = tkscript->findNamespace(sv);
                        if(NULL == ns)
                        {
                           // Namespace does not exist (should not happen at this point)
                           Dparseerror;
                        }
                        else
                        {
                           // Switch to user-defined namespace
                           useNamespace(ns);
                           selectNamespace(ns);
                           if(TOKEN_TERM != ntok)
                           {
                              Dparseerror;
                           }
                           else
                           {
                              PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                              setPNode(nn);
                              popState();
                           }
                        }
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;
               }
               break;

            case ST_UFUNRETURN:
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_RETURN)
                     {
                        Dparseerror;
                     }
                     Dsetparami(0,1); // b_break flag
                     if(ntok==TOKEN_TERM)
                     {
                        // return;
                        ////////PTN_NEW_NODE(nn, PTN_ConstVal)();
                        ///////popnode.expr = (PTN_Expr*)nn;
                        popnode.expr = NULL;
                        state_index++;
                     }
                     else if(ntok==TOKEN_ASSIGN)
                     {
                        // return=<expr>;
                        Dsetparami(0,0); // do not break
                        state_index=10;
                     }
                     else
                     {
                        Dpushstate;
                        state=ST_EXPR;
                        state_index=0;
                     }
                     break;
                  case 1: // return <EXPR> finished
                     Dungettok;
                     PTN_NEW_NODE(nn, PTN_Return)(popnode.expr, cfunction, Dgetparami(0));
                     setPNode(nn);
                     popState();
                     break;
                  case 10: // consume '=' in return=<expr>;
                     state_index=0;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;

                  default:
                     Dparseerror;
                     break;
               }
               break;

            case ST_DELEGATECALL: // delegate <name_expr>(...)
               // param0: 0=emit expression, 1=emit statement
               switch(state_index)
               {
                  case 1:
                     // ---- parse name expr
                     Dungettok;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 2:
                     if(cclassdecl)
                     {
                        // got expression, now parse args
                        if(tok!=TOKEN_BEGEXPR)
                        {
                           Dparseerror;
                        }
                        else
                        {
                           Dsetparamn(1, popnode.expr); // remember name expr
                           Dpushstate;
                           state=ST_ARGLIST;
                           state_index=0;
                        }
                     }
                     else
                     {
                        Dprintf("[---] cannot call delegate outside a class.\n");
                        Dparseerror;
                     }
                     break;
                  case 3:
                     // got arglist, now emit node
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        if(NULL != cfunction)
                        {
                           PTN_NEW_NODE(nn, PTN_ClassMethodDelegateCall)(cclassdecl, cfunction->is_class_function, Dgetparamn(1).expr, popnode.arglist);
                           setPNode(nn);
                           if(Dgetparami(0))
                           {
                              // gen statement
                              PTN_NEW_NODE(nn, PTN_Call)(popnode.callableexpr);
                              setPNode(nn);
                           }
                           popState();
                        }
                        else
                        {
                           Dprintf("[---] call call delegate outside a method.\n");
                           Dparseerror;
                        }
                     }
                     break;
               }
               break;

            case ST_DELEGATE:
               switch(state_index)
               {
                  case 1:
                     if( (TOKEN_VAR == nntok) && (TOKEN_MEMBER == ntok) )
                     {
                        // delegate MyNamespace::MyClass:name (..) {}
                        Dparsenspace(5);
                        // Not reached
                     }
                     if((ntok==TOKEN_TELSE)&&(nnntok==TOKEN_BEGEXPR)) // delegate MyClass:name (..) {}
                     {
                        if(cfunction)
                        {
                           Dprintf("\n[---] delegate functions within functions are not allowed.\n");
                           cfunction=0;
                           Dparseerror;
                        }
                        else
                        {
                           if(tok!=TOKEN_VAR)
                           {
                              Dparseerror;
                           }
                           else
                           {
                              TKS_ClassDecl *cd;
                              if(NULL != cnspace)
                              {
                                 // Only find in selected namespace
                                 cd = Dtkscompiler->findClassDeclNamespace(sv, cnspace);
                                 cnspace = NULL;
                              }
                              else
                              {
                                 // Find in all visible namespaces
                                 cd = Dtkscompiler->findClassDeclNamespaceList(sv, active_namespaces);
                              }
                              if(cd)
                              {
                                 Dsetparamv(0, (void *)cd);
                                 state_index++;
                              }
                              else
                              {
#ifdef TKS_DCON
                                 if(NULL != cnspace)
                                 {
                                    Dprintf("[---] cannot find delegate method class \"%s::%s\".\n",
                                            (char*)cnspace->name.chars,
                                            (char*)sv->chars
                                            );
                                 }
                                 else
                                 {
                                    Dprintf("[---] cannot find delegate method class \"%s\".\n", (char*)sv->chars);
                                 }
#endif
                                 Dparseerror;
                              }
                           }
                        } // if cfunction
                     }
                     else // delegate <obj>:<name>=fun;
                     {
                        Dassertnonspace;

                        // ---- parse obj expr
                        Dungettok;
                        state_index=19; // parse ST_DELEGATE::assign
                        Dpushstate;
                        state=ST_EXPR;
                        state_index=0;
                     }
                     break;

                  case 5: // consume '::' between namespace and classname
                     state_index = 1;
                     break;


                     // parse ST_DELEGATE::assign
                  case 20: // got obj expr
                     if(tok==TOKEN_BEGEXPR)
                     {
                        // ---- delegate <nameexpr> (
                        // ---- fall back to delegate call statement
                        Dsetparami(0,1);
                        state=ST_DELEGATECALL;
                        state_index=2;
                        Dungettok;
                     }
                     else if(tok!=TOKEN_TELSE)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        Dsetparamn(0, popnode.expr); // remember obj expr
                        Dpushstate;
                        state=ST_EXPR;
                        state_index=0;
                     }
                     break;
                  case 21: // got name expr
                     if(tok!=TOKEN_ASSIGN)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        Dsetparamn(1, popnode.expr); // remember name expr
                        state_index++;
                     }
                     break;
                  case 22:
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        co=functions.findEntry(sv);
                        if(!co)
                        {
                           Dprintf("\n[---] failed to resolve delegate function \"%s\".\n",
                                   (char*)(sv?(char*)sv->chars:"NULL"));
                           Dparseerror;
                        }
                        else
                        {
                           PTN_Function *fun=(PTN_Function *)co->value.object_val;
                           PTN_NEW_NODE(nn, PTN_ClassMethodDelegateAssign)(Dgetparamn(0).expr, Dgetparamn(1).expr, fun);
                           setPNode(nn);
                           popState();
                        }
                     }
                     break;

                     // parser ST_DELEGATE::decl
                  case 2:
                     state_index++; // consume ':'
                     break;
                  case 3:
                     if(tok!=TOKEN_VAR) // expect function name
                     {
                        Dparseerror;
                     }
                     else
                     {
                        // ---- create delegate script function
                        PTN_NEW_NODE(nn, PTN_Function)(sv);
                        cfunction = (PTN_Function*)nn;
                        co = functions.createAnyEntry(&cfunction->name, 0, (void *) cfunction);
                        if(!co)
                        {
                           if(sv&&sv->chars&&!sv->isBlank())
                           {
                              Dprintf("\n[---] failed to create delegate function \"%s\" hashtable entry.\n", (char*)sv->chars);
                              Dparseerror;
                           }
                           else
                           {
                              Dprintf("\n[---] failed to create delegate function: empty function-name.\n");
                              Dparseerror;
                           }
                        }
                        cfunction->parent_class=(TKS_ClassDecl*)Dgetparamv(0);
                        cclassdecl = cfunction->parent_class;
                        // ---- parse regular function decl
                        Dungetstr;
                        Dpushstate;
                        state=ST_UFUNDECL;
                        state_index=1;
                        Dsetparamn(0, cfunction);
                     }
                     break;
                  case 4: // return function
                     Dungettok;
                     cclassdecl=0;
                     popState();
                     break;
               }
               break;

            case ST_UFUNDECL:
               // paramn(0): method (PTN_Function) declaration (w/o body/argvarlist)
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_FUNCTION)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     curr_argvarlist=0;
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     {
                        sBool newfun=1;
                        if(!Dgetparamn(0).functiondecl)
                        {
                           co=functions.findEntry(sv);
                           if(co)
                           {
                              PTN_Function *tfun=(PTN_Function*)co->value.object_val;
                              if(tfun->body)
                              {
                                 Dprintf("\n[---] function \"%s\" already has a body.\n",
                                         (char*)sv->chars);
                                 Dparseerror;
                              }
                              else
                              {
                                 cfunction=tfun;
                                 newfun=0;
                              }
                           }
                           else
                           {
                              PTN_NEW_NODE(nn, PTN_Function)(sv);
                              cfunction = (PTN_Function*) nn; // new local script fun
                              cfunction->src_loc = Dsrcloc;
                              if(create_namespace != &Dtkscompiler->default_namespace)
                              {
                                 (void)create_namespace->addFunction(cfunction);
                              }
                           }
                        }
                        else
                        {
                           cfunction=Dgetparamn(0).functiondecl; // get function from class method
                        }
                        parser_tree_num_nodes++;
                        Dsetparami(1, newfun); // remember whether function is declared or just implemented!
                        if(newfun)
                        {
                           cfunction->src_loc = Dsrcloc;
                           if(!cclassdecl)
                           {
                              cfunction->name.yacCopy(sv);
                              // generate global function
                              // create hashtable entry for function
                              co = functions.createAnyEntry(&cfunction->name, 0, (void *)cfunction);
                              if(!co)
                              {
                                 if(sv&&sv->chars&&!sv->isBlank())
                                 {
                                    Dprintf("\n[---] failed to create function \"%s\" hashtable entry.\n", (char*)sv->chars);
                                    Dparseerror;
                                 }
                                 else
                                 {
                                    Dprintf("\n[---] failed to create function: empty function-name.\n");
                                    Dparseerror;
                                 }
                              }
                           }
                        }
                     }
                     state_index++;
                     break;
                  case 2:
                     if(tok!=TOKEN_BEGEXPR)
                     {
                        // declare fun {   (skipping formal argument list)
                        if((tok!=TOKEN_BEGBLOCK)&&(tok!=TOKEN_COMPILE))
                        {
                           // ---- single statement function ----
                           Dungettok;
                           if(Dgetparamn(0).functiondecl)   // from inline class method body decl
                           {
                              PTN_NEW_NODE(nn, PTN_FunctionBody)();
                              popnode.node=nn;
                           }
                           else
                           {
                              popnode.statement=cfunction;
                           }
                           cfunction=0;
                           popState();
                        }
                        else
                        {
                           Dungettok;
                           state_index=7;
                           // begin function SEQ
                           Dpushstate;
                           state=ST_SEQ;
                           state_index=0;
                        }
                     }
                     else
                     {
                        state_index++;
                     }
                     break;
                  case 3:
                     if(tok==TOKEN_ENDEXPR)
                     {
                        if((ntok==TOKEN_TELSE)||(ntok==TOKEN_RETURNS))
                        {
                           // empty arglist + function return type?
                           state_index=30;
                        }
                        else
                           // function implementation follows?
                           if((ntok==TOKEN_BEGBLOCK)||(ntok==TOKEN_COMPILE))
                           {
                              state_index=7;
                              Dpushstate;
                              // begin function SEQ
                              state=ST_SEQ;
                              state_index=0;
                           }
                           else
                           {
                              // no function body implementation here
                              // will be done later using function myfun {...}
                              if(Dgetparamn(0).functiondecl)   // from inline class method body decl
                              {
                                 PTN_NEW_NODE(nn, PTN_FunctionBody)();
                                 popnode.node=nn; // new PTN_Function()
                              }
                              else
                              {
                                 popnode.functiondecl=cfunction;
                              }
                              cfunction=0;
                              popState();
                           }
                     }
                     else
                     {
                        if(/**inline classdecl**/(cclassdecl!=0)||/**new fun+impl**/Dgetparami(1)||/**class method impl**/(Dgetparamn(0).node!=0))
                        {
                           // ---- start first variable declaration ---
                           PTN_NEW_NODE(nn, PTN_ArgVarList)();
                           curr_argvarlist = (PTN_ArgVarList*) nn;
                           Dungettok;
                           Dpushstate;
                           vardecl_tag=0;
                           template_vardecl=1;
                           state=ST_VARDECL;
                           state_index=0;
                        }
                        else
                        {
                           tkscript->printf("\n[---] function argument list is already declared.\n");
                           Dparseerror;
                        }
                     }
                     break;
                  case 30:
                     // consume ':' or 'returns'
                     state_index++;
                     break;
                  case 31:
                  {
                     sBool bDoBreak = 0;
                     switch(tok)
                     {
                        default:
                           Dparseerror;
                           break;
                        case TOKEN_MEMBER:
                           // "returns ::<type>"
                           ////cnspace = &Dtkscompiler->default_namespace;
                           Ddefaultnspace;
                           break;
                        case TOKEN_VAR:
                           if(TOKEN_MEMBER == ntok) // Dparsenspace
                           {
                              Dfindnspace;
                              state_index = 37;
                              bDoBreak = 1;
                           }
                           if(sv->compare(&tok_var))
                           {
                              Dassertnonspace;
                              cfunction->return_type = YAC_TYPE_VARIANT;
                           }
                           else
                           {
                              cfunction->return_type = YAC_TYPE_OBJECT;
                              cfunction->return_class_name.yacCopy(sv);
                              Dfreezenspaces(cfunction->return_class_nspaces);
                           }
                           break;
                        case TOKEN_INT:
                           Dassertnonspace;
                           cfunction->return_type = YAC_TYPE_INT;
                           break;
                        case TOKEN_FLOAT:
                           Dassertnonspace;
                           cfunction->return_type = YAC_TYPE_FLOAT;
                           break;
                        case TOKEN_OBJECT:
                           Dassertnonspace;
                           cfunction->return_type = YAC_TYPE_OBJECT;
                           break;
                        case TOKEN_STRING:
                           Dassertnonspace;
                           cfunction->return_type = YAC_TYPE_STRING;
                           cfunction->return_template_object = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_STRING]->template_object;
                           break;
                     }

                     if(bDoBreak)
                     {
                        break;
                     }

                     if(ntok==TOKEN_DOT) // consume script constraint?
                     {
                        state_index=35;
                        break;
                     }
                  }
                  // ---- run into case 36
                  case 36: // script constraint consumed
                     if((ntok==TOKEN_BEGBLOCK)||(ntok==TOKEN_COMPILE))
                     {
                        state_index=7;
                        Dpushstate;
                        // begin function SEQ
                        state=ST_SEQ;
                        state_index=0;
                     }
                     else
                     {
                        // no function body implementation here
                        // will be done later using function myfun {...}
                        if(Dgetparamn(0).functiondecl)   // from inline class method body decl
                        {
                           PTN_NEW_NODE(nn, PTN_FunctionBody)();
                           popnode.node=nn; // new PTN_Function()
                        }
                        else
                        {
                           popnode.functiondecl=cfunction;
                        }
                        cfunction->argvarlist=curr_argvarlist;
                        curr_argvarlist=0;
                        cfunction=0;
                        popState();
                     }
                     break;

                  case 35: // consume script constraint
                     state_index++;
                     break;

                  case 37: // consume '::' between namespace and classname
                     state_index = 31;
                     break;


                  case 4:  // vardecl finished
                     template_vardecl=0;
                     curr_argvarlist->addVar(popnode.vardecl->var);
                     delete popnode.vardecl; popnode.vardecl=0;
                     if(tok==TOKEN_LISTSEP) // another param?
                     {
                        if(curr_argvarlist->num_vars==255)
                        {
                           Dprintf("\n[---] trying to declare function with more than 256 parameters.\n");
                           Dparseerror;
                        }
                        state_index=3;
                        Dpushstate;
                        vardecl_tag=0;
                        template_vardecl=1;
                        state=ST_VARDECL;
                        state_index=0;
                     }
                     else
                     {
                        if(tok==TOKEN_ENDEXPR) // end of param-list?
                        {
                           if((ntok==TOKEN_TELSE)||(ntok==TOKEN_RETURNS))
                           {
                              // arglist + function return type?
                              state_index=30;
                           }
                           else
                              if((ntok==TOKEN_BEGBLOCK)||(ntok==TOKEN_COMPILE))
                              {
                                 // ---- implement function/method ----
                                 state_index=7;
                                 Dpushstate;
                                 // begin function SEQ
                                 state=ST_SEQ;
                                 state_index=0;
                              }
                              else
                              {
                                 // ---- no function implemention ---
                                 // ---- if this is not a new function something went terribly wrong ----
                                 if(!(Dgetparami(1)))
                                 {
                                    Dprintf("[---] trying to redeclare function \"%s\".\n",
                                            cfunction->name.chars);
                                    Dparseerror;
                                 }

                                 PTN_Function *tf=Dgetparamn(0).functiondecl;  // cfunction!??
                                 if(tf)  // within class method body decl
                                 {
                                    tf->argvarlist=curr_argvarlist;
                                    PTN_NEW_NODE(nn, PTN_FunctionBody)();
                                    popnode.functionbody = (PTN_FunctionBody*) nn;
                                 }
                                 else
                                 {
                                    tf=cfunction;
                                    tf->argvarlist=curr_argvarlist;
                                    popnode.functiondecl=tf;
                                 }
                                 curr_argvarlist=0;
                                 cfunction=0;
                                 popState();
                              }
                        }
                        else
                        {
                           Dparseerror;
                        }
                     }
                     break;
                  case 8:
                     Dungettok;
                     cfunction->body=popnode.statement;

                     // class method body ? avoid double deletion
                     // function implementation? avoid double node usage resulting
                     // in bad linked statement lists
                     //if(Dgetparamn(0).functiondecl)
                     if((Dgetparamn(0).functiondecl)&&(!cfunction->parent_class))
                     {
                        //PTN_Function *decl=Dgetparamn(0).functiondecl;
                        //decl->argvarlist=curr_argvarlist;
                        cfunction->argvarlist=curr_argvarlist;
                        PTN_NEW_NODE(nn, PTN_FunctionBody)();
                        popnode.functionbody = (PTN_FunctionBody*) nn;
                     }
                     else
                     {
                        if(Dgetparami(1))
                        {
                           cfunction->argvarlist=curr_argvarlist;
                           popnode.functiondecl=cfunction;
                        }
                        else
                        {
                           PTN_NEW_NODE(nn, PTN_FunctionBody)();
                           popnode.functionbody = (PTN_FunctionBody*) nn;
                        }
                     }
                     curr_argvarlist=0;
                     cfunction=0;
                     popState();
                     break;

                  default:
                     Dparseerror;
                     break;
               }
               break;

            case ST_UFUNBODY:
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_FUNCTION)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     // find function
                     if(!cclassdecl)
                     {
                        TKS_CachedObject *co=functions.findEntry(sv);
                        if(!co)
                        {
                           Dprintf("\n[---] missing declaration for function \"%s\".\n", (char*)sv->chars);
                           Dparseerror;
                        }
                        cfunction = (PTN_Function*)co->value.object_val;
                     }
                     if(ntok==TOKEN_BEGEXPR) state_index=3;
                     else
                     {
                        if((ntok!=TOKEN_BEGBLOCK)&&(ntok!=TOKEN_COMPILE))
                        {
                           toki++;
                           Dparseerror;
                        }
                        else
                        {
                           Dpushstate;
                           // begin function SEQ
                           state=ST_SEQ;
                           state_index=0;
                        }
                     }
                     break;
                  case 2:
                     Dungettok;
                     cfunction->body=popnode.statement; // xxx already done in SEQ
                     PTN_NEW_NODE(nn, PTN_FunctionBody)();
                     setPNode(nn);
                     if(!cclassdecl) cfunction=0;
                     popState();
                     break;
                  case 3: // consume ()
                     if(tok!=TOKEN_BEGEXPR)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 4: // consume ()
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     state_index=1;
                     Dpushstate;
                     // begin function SEQ
                     state=ST_SEQ;
                     state_index=0;
                     break;
               }
               break;

            case ST_LOOP:
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_LOOP)
                     {
                        Dparseerror;
                     }
                     PTN_NEW_NODE(nn, PTN_LoopStatement)();
                     popnode.loopstatement = (PTN_LoopStatement*) nn;
                     setPNode(popnode.node);
                     pushBreak(popnode.breakable_statement);
                     Dsetparamn(1,popnode.node);
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 1:
                     // <EXPR> finished
                     Dsetparamn(0, popnode.expr);
                     Dungettok;
                     Dpushstate;
                     state=ST_SEQ;
                     state_index=0;
                     break;
                  case 2:
                     Dungettok;
                     tnode=Dgetparamn(1);
                     tnode.loopstatement->init(Dgetparamn(0).expr, popnode.statement);
                     popnode=tnode;
                     popBreak();
                     popState();
                     break;
               }
               break;

            case ST_PREINCVARSTAT:
               switch(state_index)
               {
                  case 0:
                     co = NULL;
                     if(tok!=TOKEN_INC)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     Dparsenspace(2);

                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     if(NULL == cnspace)
                     {
                        co = findVarLocal(sv);
                        if(co)
                        {
                           PTN_NEW_NODE(nn, PTN_PreIncVarStat)();
                           setPNode(nn);
                           popnode.preincvarstat->cached_object=co;
                           popState();
                           break;
                        }

                        if( (NULL==cnspace) && cclassdecl)
                        {
                           co = cclassdecl->getMember(sv);
                           DcheckFunMemberAccess(sv);
                           if(co)
                           {
                              DcheckMemberAccess(cclassdecl, sv);
                              PTN_NEW_NODE(nn, PTN_ClassMemberIncDecStat)(0, co, 0);
                              setPNode(nn);
                              popState();
                           }
                        }
                     }
                     else
                     {
                        co = NULL;
                     }
                     if(!co)
                     {
                        ////co = findVarGlobal(sv, active_namespaces, cnspace);
                        co = findVar(sv, active_namespaces, cnspace);
                        cnspace = NULL;
                        if(!co) // xxx
                        {
                           Dprintf("[---] unknown reference to variable \"%s\".\n",
                                   (char*)(sv->chars));
                           Dparseerror;
                        }
                        PTN_NEW_NODE(nn, PTN_PreIncVarStat)();
                        setPNode(nn);
                        popnode.preincvarstat->cached_object=co;
                        popState();
                     }
                     break;
                  case 2: // consume '::' between namespace and varname
                     state_index = 1;
                     break;
               }
               break;

            case ST_PREDECVARSTAT:
               switch(state_index)
               {
                  case 0:
                     co = NULL;
                     if(tok!=TOKEN_DEC)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     Dparsenspace(2);

                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     if(NULL == cnspace)
                     {
                        co = findVarLocal(sv);
                        if(co)
                        {
                           PTN_NEW_NODE(nn, PTN_PreDecVarStat)();
                           setPNode(nn);
                           popnode.predecvarstat->cached_object=co;
                           popState();
                           break;
                        }

                        if(cclassdecl)
                        {
                           co=cclassdecl->getMember(sv);
                           DcheckFunMemberAccess(sv);
                           if(co)
                           {
                              DcheckMemberAccess(cclassdecl, sv);
                              PTN_NEW_NODE(nn, PTN_ClassMemberIncDecStat)(0, co, 1);
                              setPNode(nn);
                              popState();
                           }
                        }
                     }
                     else
                     {
                        co = NULL;
                     }
                     if(!co)
                     {
                        ////co = findVarGlobal(sv, active_namespaces, cnspace);
                        co = findVar(sv, active_namespaces, cnspace);
                        cnspace = NULL;
                        if(!co)
                        {
                           Dprintf("[---] unknown reference to variable \"%s\".\n",
                                   (char*)(sv->chars));
                           Dparseerror;
                        }
                        PTN_NEW_NODE(nn, PTN_PreDecVarStat)();
                        setPNode(nn);
                        popnode.predecvarstat->cached_object=co;
                        popState();
                     }
                     break;
                  case 2:
                     // consume '::' between namespace and varname
                     state_index = 1;
                     break;
               }
               break;

            case ST_POSTINCVARSTAT: // note: cnspace must be already set
               switch(state_index)
               {
                  case 0:
                     co = NULL;
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     {
                        if(NULL == cnspace)
                        {
                           if(!b_force_this)
                           {
                              co = findVarLocal(sv);
                              if(co)
                              {
                                 Dsetparamv(0, (void*)co);
                                 Dsetparami(1, 0);
                                 state_index++;
                                 break;
                              }
                           }

                           if(cclassdecl)
                           {
                              co = cclassdecl->getMember(sv);
                              if(!b_force_this)
                              {
                                 // hack to make function vars shadow class members
                                 DcheckFunMemberAccess(sv);
                              }
                              if(co)
                              {
                                 b_force_this = 0;
                                 DcheckMemberAccess(cclassdecl, sv);
                                 Dsetparami(1, 1);
                                 Dsetparamv(0, (void*)co);
                              }
                              else if(b_force_this)
                              {
                                 Dprintf("[---] class \"%s\" has no member called \"%s\".\n",
                                         (char*)cclassdecl->name.chars,
                                         (char*)sv->chars);
                                 Dparseerror;
                              }
                           }
                        }
                        if(!co)
                        {
                           ////co = findVarGlobal(sv);
                           co = findVar(sv, active_namespaces, cnspace); // xxx can IncDecStat/PostIncVarStat handle local vars?
                           if(!co)
                           {
                              if(NULL == cnspace)
                              {
                                 state_index=10-1; // unresolved variable
                                 Dsetparamv(0, (void*)sv);
                              }
                              else
                              {
                                 Dprintf("[---] cannot find variable \"%s::%s\".\n",
                                         (char*)cnspace->name.chars,
                                         (char*)sv->chars
                                         );
                                 Dparseerror;
                              }
                           }
                           else
                           {
                              Dsetparamv(0, (void*)co);
                              Dsetparami(1, 0);
                           }
                        }
                     }
                     cnspace = NULL;
                     state_index++;
                     break;
                  case 1:
                     if(tok!=TOKEN_INC)
                     {
                        Dparseerror;
                     }
                     if(Dgetparami(1))
                     {
                        PTN_NEW_NODE(nn, PTN_ClassMemberIncDecStat)(0, (TKS_CachedObject*)Dgetparamv(0), 0);
                        setPNode(nn);
                        popnode.classmemberincdecstat->class_id=cclassdecl->class_id;
                     }
                     else
                     {
                        PTN_NEW_NODE(nn, PTN_PostIncVarStat)();
                        setPNode(nn);
                        popnode.postincvarstat->cached_object=(TKS_CachedObject*)Dgetparamv(0);
                     }
                     popState();
                     break;
                  case 10:
                     if(tok!=TOKEN_INC)
                     {
                        Dparseerror;
                     }
                     if(tkscript->configuration.b_enable_unresolved_members)
                     {
                        PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberPostIncDecStat)(&name, 1, (YAC_String*)Dgetparamv(0), 0);
                        setPNode(nn);
                        Dfreezenspaces(popnode.unresolvedmodulememberpostincdecstat->nspace_array);
                        popState();
                     }
                     else
                     {
                        Dprintf("[---] cross referencing for identifier \"%s\" is turned OFF.\n",
                                (char*)((YAC_String*)Dgetparamv(0))->chars);
                        Dparseerror;
                     }
                     break;
               }
               break;

            case ST_POSTDECVARSTAT: // note: cnspace must already be set
               switch(state_index)
               {
                  case 0:
                     co = NULL;
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     {
                        if(NULL == cnspace)
                        {
                           if(!b_force_this)
                           {
                              co = findVarLocal(sv);
                              if(co)
                              {
                                 Dsetparamv(0, (void *)co);
                                 Dsetparami(1, 0);
                                 state_index++;
                                 break;
                              }
                           }

                           if(cclassdecl)
                           {
                              co=cclassdecl->getMember(sv);
                              if(!b_force_this)
                              {
                                 // hack to make function vars shadow class members
                                 DcheckFunMemberAccess(sv);
                              }
                              if(co)
                              {
                                 b_force_this = 0;
                                 DcheckMemberAccess(cclassdecl, sv);
                                 Dsetparamv(0, (void *)co);
                                 Dsetparami(1, 1);
                              }
                              else if(b_force_this)
                              {
                                 Dprintf("[---] class \"%s\" has no member called \"%s\".\n",
                                         (char*)cclassdecl->name.chars,
                                         (char*)sv->chars);
                                 Dparseerror;
                              }
                           }
                        }
                        if(!co)
                        {
                           ////co = findVarGlobal(sv);
                           co = findVar(sv, active_namespaces, cnspace); // xxx can IncDecStat/PostDecVarStat handle local vars?
                           if(!co)
                           {
                              if(NULL == cnspace)
                              {
                                 state_index=10-1; // unresolved variable
                                 Dsetparamv(0, (void *)sv);
                              }
                              else
                              {
                                 Dprintf("[---] cannot find variable \"%s::%s\".\n",
                                         (char*)cnspace->name.chars,
                                         (char*)sv->chars
                                         );
                                 Dparseerror;
                              }
                           }
                           else
                           {
                              Dsetparamv(0, (void *)co);
                              Dsetparami(1, 0);
                           }
                        }
                     }
                     cnspace = NULL;
                     state_index++;
                     break;
                  case 1:
                     if(tok!=TOKEN_DEC)
                     {
                        Dparseerror;
                     }
                     if(Dgetparami(1))
                     {
                        PTN_NEW_NODE(nn, PTN_ClassMemberIncDecStat)(0, (TKS_CachedObject*)Dgetparamv(0), 1);
                        setPNode(nn);
                        popnode.classmemberincdecstat->class_id=cclassdecl->class_id;
                     }
                     else
                     {
                        PTN_NEW_NODE(nn, PTN_PostDecVarStat)();
                        setPNode(nn);
                        popnode.postdecvarstat->cached_object=(TKS_CachedObject*)Dgetparamv(0);
                     }
                     popState();
                     break;
                  case 10:
                     if(tok!=TOKEN_DEC)
                     {
                        Dparseerror;
                     }
                     if(tkscript->configuration.b_enable_unresolved_members)
                     {
                        PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberPostIncDecStat)(&name, 1, (YAC_String*)Dgetparamv(0), 1);
                        setPNode(nn);
                        Dfreezenspaces(popnode.unresolvedmodulememberpostincdecstat->nspace_array);
                        popState();
                     }
                     else
                     {
                        Dprintf("[---] cross referencing for identifier %s is turned OFF.\n",
                                (char*)((YAC_String*)Dgetparamv(0))->chars);
                        Dparseerror;
                     }
                     break;
               }
               break;

            case ST_FOREACH:
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_FOREACH)
                     {
                        Dparseerror;
                     }
                     PTN_NEW_NODE(nn, PTN_ForEach)();
                     popnode.foreachstatement = (PTN_ForEach*) nn;
                     setPNode(popnode.node);
                     pushBreak(popnode.breakable_statement);
                     Dsetparamn(3, popnode.node);
                     state_index++;
                     break;
                  case 1:
                     Dparsenspace(5);

                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }

                     co = findVar(sv, active_namespaces, cnspace);
                     if(!co)
                     {
                        Dprintf("\n[---] foreach variable \"%s\" not found.\n",
                                (char*)sv->chars);
                        Dparseerror;
                     }
                     else
                     {
                        cnspace = NULL;
                        Dsetparamv(0, (void *)co);
                        state_index++;
                     }
                     break;
                  case 5: // skip '::' between namespace and varname
                     state_index = 1;
                     break;
                  case 2:
                     if(tok == TOKEN_VAR) // expect "in" string (without quotation marks, of course)
                     {
                        if(!sv->getQuotFlag())
                        {
                           if(sv->compare("in"))
                           {
                              // Parse object expr
                              Dpushstate;
                              state=ST_EXPR;
                              state_index=0;
                              break;
                           }
                        }
                     }
                     Dparseerror;
                     break;
                  case 3:
                     Dsetparamn(1, popnode.expr);
                     Dpushstate;
                     if(tok==TOKEN_BEGBLOCK)
                     {
                        Dungettok;
                        state=ST_SEQ;
                        state_index=0;
                     }
                     else
                     {
                        Dungettok;
                        state=ST_SSEQ; // shortpath to single sequence
                        state_index=0;
                     }
                     break;
                  case 4: // sequence returned
                     Dungettok;
                     {
                        TKS_CachedObject *cvar=(TKS_CachedObject*)Dgetparamv(0);
                        tnode=Dgetparamn(3);
                        tnode.foreachstatement->init(cvar, Dgetparamn(1).expr, popnode.statement);
                        popnode=tnode;
                     }
                     popBreak();
                     popState();
                     break;
               }
               break;

            case ST_FOR:
               switch(state_index)
               {
                  case 0:
                     if(TOKEN_FOR == tok)
                     {
                        state_index++;
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 1:
                     if(TOKEN_BEGEXPR == tok)
                     {
                        if(TOKEN_ESAC == ntok) // infinite for(;;) loop
                        {
                           PTN_NEW_NODE(nn, PTN_While)();
                           setTNode(nn);
                           Dsetparamn(3, tnode.statement);
                           pushBreak(tnode.breakable_statement);
                           state_index=10;
                        }
                        else
                        {
                           PTN_NEW_NODE(nn, PTN_For)();
                           setTNode(nn);
                           Dsetparamn(3, tnode.statement);
                           pushBreak(tnode.breakable_statement);
                           Dpushstate;
                           state=ST_SSEQSTAT;
                           state_index=0;
                        }
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 2: //for(<SSEQ> finished; <EXPR>, <SSEQ>) <SEQ>
                     Dsetparamn(0, popnode.statement);
                     if(tok!=TOKEN_TERM)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 3:
                     Dsetparamn(1, popnode.expr);
                     if(tok==TOKEN_TERM)
                     {
                        if(ntok==TOKEN_ENDEXPR)
                        {	// leere veraenderungsanweisung
                           PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                           setPNode(nn);
                           state_index++;
                        }
                        else
                        {
                           Dpushstate;
                           state=ST_SSEQSTAT;
                           state_index=0;
                        }
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 4:
                     Dsetparamn(2, popnode.statement);
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_SEQ;
                     state_index=0;
                     break;
                  case 5:
                     Dungettok;
                     tnode.forstatement = Dgetparamn(3).forstatement;
                     tnode.forstatement->init(
                        Dgetparamn(0).statement,
                        Dgetparamn(1).expr,
                        Dgetparamn(2).statement,
                        popnode.statement);
                     popnode = tnode;
                     popBreak();
                     popState();
                     break;
                  case 10: // for (;;). consume ';;'
                     state_index++;
                     break;
                  case 11:
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_SEQ;
                     state_index=0;
                     break;
                  case 12:
                     // for(;;) {/*...*/} finished
                     Dungettok;
                     tnode.whilestatement = Dgetparamn(3).whilestatement;
                     PTN_NEW_NODE(nn, PTN_ConstVal)(1);
                     tnode.whilestatement->init((PTN_Expr*)nn, popnode.statement);
                     popnode = tnode;
                     popBreak();
                     popState();
                     break;

               }
               break;


            case ST_SWITCH:
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_SWITCH) // "switch"
                     {
                        Dparseerror;
                     }
                     Dsetparamn(0, NULL); // PTN_Switch self
                     state_index++;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 2:
                     // <EXPR> finished
                     PTN_NEW_NODE(nn, PTN_Switch)(popnode.expr);
                     setPNode(nn);
                     pushBreak(popnode.breakable_statement); // only used for nested breaks!
                     Dsetparamn(0, popnode.node);
                     Dsetparamn(1, 0); // last switchcase
                     state_index++;
                     //break;
                  case 3:
                     if(tok!=TOKEN_BEGBLOCK) //"{"
                     {
                        Dparseerror;
                     }
                     if(ntok==TOKEN_ENDBLOCK) state_index=4; //"}" // empty switch() statement, useless but supported
                     else
                     {
                        state_index=4; // popstate skips 4
                        Dpushstate;
                        state=ST_SWITCHCASE;
                        state_index=0;
                     }
                     break;
                  case 4:
                     if(tok!=TOKEN_ENDBLOCK) // empty switch statement finished
                     {
                        Dparseerror;
                     }
                     popBreak();
                     popState(); // <SWITCH> finished
                     break;
                  case 5:
                     // <SWITCHCASE> finished
                     tnode=Dgetparamn(0); // get Switch node
                     t2node=Dgetparamn(1); // get last switchcase
                     if(t2node.node) // next switchcase?
                     {
                        // create double link list of switchcases
                        t2node.switchcase->next=popnode.switchcase;
                        popnode.switchcase->prev=t2node.switchcase;
                     }
                     else
                     {
                        // remember first switchcase
                        tnode.switchstatement->switch_case=popnode.switchcase;
                     }
                     if(popnode.switchcase->getID()==PTN_SWITCHDEFAULTCASE)
                     {
                        if(tnode.switchstatement->default_case!=NULL)
                        {
                           Dprintf("[---] default switch case label is already set.\n");
                           Dparseerror;
                        }
                        tnode.switchstatement->default_case = popnode.switchdefaultcase;
                     }
                     Dsetparamn(1, popnode.switchcase);
                     if(tok==TOKEN_CASE||tok==TOKEN_DEFAULT) //"case" "default"
                     {
                        // loop switchcase
                        state_index=4;
                        Dungettok;
                        Dpushstate;
                        state=ST_SWITCHCASE;
                        state_index=0;
                     }
                     else
                     {
                        if(tok!=TOKEN_ENDBLOCK) // "}"
                        {
                           Dparseerror;
                        }
                        popnode=tnode; // <SWITCH> finished
                        popBreak();
                        popState();
                     }
                     break;
               }
               break;

            case ST_SWITCHCASE: // <SWITCHCASE>
               switch(state_index)
               {
                  case 0:
                     Dsetparamn(1, NULL); // unset expr
                     Dsetparamn(2, NULL); // unset last seq node

                     if(tok==TOKEN_CASE)
                     {
                        Dsetparami(0, 0);
                        state_index++;
                        Dpushstate;
                        state=ST_EXPR;
                        state_index=0;
                     }
                     else
                     {
                        if(tok==TOKEN_DEFAULT)
                        {
                           Dsetparami(0, 1);
                           Dsetparamn(1, 0);
                           state_index++;
                        }
                        else
                        {
                           Dparseerror;
                        }
                     }
                     break;
                  case 101: // default (without :)
                  case 102: // case <expr> (without :)

                  case 1: // default:
                     if(tok!=TOKEN_TELSE)
                     {
                        Dungettok; // ignore ':'
                        ntok=tok;
                     }
                     state_index++;
                     goto switchcase_checkseq;
                  case 2: // case <EXPR>:
                     // <EXPR> finished
                     Dsetparamn(1, popnode.node);
                     if(tok!=TOKEN_TELSE)
                     {
                        Dungettok; // ignore ':'
                        ntok=tok;
                     }
                  switchcase_checkseq:
                     if(ntok==TOKEN_DEFAULT||ntok==TOKEN_CASE||ntok==TOKEN_ENDBLOCK)
                     {
                        // <SWITCHCASE> finished, no <SEQ>, no break
                        if(Dgetparami(0)) // isDefault?
                        {
                           PTN_NEW_NODE(nn, PTN_SwitchDefaultCase)(0, 0, 0);
                           setPNode(nn);
                        }
                        else
                        {
                           PTN_NEW_NODE(nn, PTN_SwitchCase)(Dgetparamn(1).expr, 0, 0);
                           setPNode(nn);
                        }
                        popState();
                     }
                     else
                     {
                        if(ntok==TOKEN_BREAK) state_index=5; // "break" ?
                        else
                        {
                           Dsetparamn(2, 0); // unset last seq node
                           Dpushstate;
                           state=ST_SEQ;
                           state_index=0;
                        }
                     }
                     break;
                  case 3:
                     // <SEQ> finished
                     tnode=Dgetparamn(2); // get last stat
                     if(tnode.statement)
                     {
                        // create dllist of switchcase statements
                        tnode.statement->next=popnode.statement;
                        popnode.statement->prev=tnode.statement;
                     }
                     if(tok==TOKEN_BREAK) // "break" ?
                     {
                        Dsetparamn(2, popnode.node); // store <SEQ>
                        Dsetparami(3, (tok==TOKEN_BREAK));
                        state_index++;
                     }
                     else
                     { // no break
                        if(tok==TOKEN_DEFAULT||tok==TOKEN_CASE||tok==TOKEN_ENDBLOCK)
                        {
                           Dungettok;
                           if(Dgetparami(0)) // isDefault?
                           {
                              // find first statement of block
                              while(popnode.statement->prev) popnode.statement=popnode.statement->prev;
                              PTN_NEW_NODE(nn, PTN_SwitchDefaultCase)(0, popnode.statement, 0);
                              setPNode(nn);
                           }
                           else
                           {
                              // find first statement of block
                              while(popnode.statement->prev) popnode.statement=popnode.statement->prev;
                              PTN_NEW_NODE(nn, PTN_SwitchCase)(Dgetparamn(1).expr, popnode.statement, 0);
                              setPNode(nn);
                           }
                           popState();
                        }
                        else
                        {
                           Dungettok;
                           ////Dsetparamn(2, popnode.node); // store <SEQ>
                           while(popnode.statement->next) // new 19Sep09
                           {
                              popnode.node = popnode.statement->next;
                           }
                           Dsetparamn(2, popnode.node); // store <SEQ>
                           state_index=2;
                           Dpushstate;
                           state=ST_SEQ;
                           state_index=0;
                           // get next statement
                        }
                     }
                     break;
                  case 4:
                     if(tok!=TOKEN_TERM) // break ";"
                     {
                        Dparseerror;
                     }
                     // <SEQ> finished with break ;
                     { // break
                        if(Dgetparami(0)) // isDefault?
                        {
                           tnode=Dgetparamn(2);
                           // find first statement of block
                           while(tnode.statement->prev) tnode.statement=tnode.statement->prev;
                           PTN_NEW_NODE(nn, PTN_SwitchDefaultCase)(0, tnode.statement, 1);
                           setPNode(nn);
                        }
                        else
                        {
                           tnode=Dgetparamn(2);
                           // find first statement of block
                           while(tnode.statement->prev) tnode.statement=tnode.statement->prev;
                           PTN_NEW_NODE(nn, PTN_SwitchCase)(Dgetparamn(1).expr, tnode.statement, 1);
                           setPNode(nn);
                        }
                        popState();
                     }
                     break;
                  case 5: // empty <SEQ>, "break" ;
                     if(tok!=TOKEN_BREAK) // nested breaks are handled differently!
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 6: // empty <SEQ> break ";"
                  {
                     // <SWITCHCASE> finished, no <SEQ>, no break
                     if(Dgetparami(0)) // isDefault?
                     {
                        PTN_NEW_NODE(nn, PTN_SwitchDefaultCase)(0, 0, 1);
                        setPNode(nn);
                     }
                     else
                     {
                        PTN_NEW_NODE(nn, PTN_SwitchCase)(Dgetparamn(1).expr, 0, 1);
                        setPNode(nn);
                     }
                     popState();
                  }
                  break;

               }
               break;

            case ST_IF:
               // param 0: (state 2): condition expr
               // param 1: (state 3): saved "if" body if "else" exists
               // param 2: (state 1): PTN_IfElse object
               switch(state_index)
               {
                  case 0: // if
                     if(tok==TOKEN_IF) state_index++;
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 1: // <EXPR>

                     ////PTN_NEW_NODE(nn, PTN_IfElse)();
                     ////Dsetparamn(2, nn);

                     Dungettok;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 2: // <SEQ>
                     Dsetparamn(0, popnode.node); // cond
                     if(tok==TOKEN_BEGBLOCK) // shortpath to single sequence / sequence list
                     {
                        Dungettok;
                        Dpushstate;
                        // begin FOR body SEQ
                        state=ST_SEQ;
                        state_index=0;
                        break;
                     }
                     else
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SSEQ;
                        state_index=0;
                     }
                     break;
                  case 3:
                     if(tok==TOKEN_ELSE) // else <SEQ>
                     {
                        Dsetparamn(1, popnode.node); // save "if" body
                        if(ntok==TOKEN_BEGBLOCK)
                        {
                           Dpushstate;
                           state=ST_SEQ;
                           state_index=0;
                        }
                        else
                        {
                           ////Dungettok;
                           Dpushstate;
                           state=ST_SSEQ;
                           state_index=0;
                        }
                     }
                     else
                     {
                        // sequence if <EXPR> <SEQ> finished
                        PTN_NEW_NODE(nn, PTN_IfElse)(Dgetparamn(0).expr, popnode.statement, 0);
                        setPNode(nn);
                        Dungettok;
                        popState();
                     }
                     break;
                  case 4: // sequence if <EXPR> <SEQ> else <SEQ> finished
                     PTN_NEW_NODE(nn, PTN_IfElse)(Dgetparamn(0).expr, Dgetparamn(1).statement, popnode.statement);
                     setPNode(nn);
                     Dungettok;
                     popState();
                     break;
               }
               break;

            case ST_WHILE:
               switch(state_index)
               {
                  case 0: // while
                     if(tok==TOKEN_WHILE)
                        state_index++;
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 1: // <EXPR>
                     PTN_NEW_NODE(nn, PTN_While)();
                     popnode.node=nn;
                     setPNode(popnode.node);
                     pushBreak(popnode.breakable_statement);
                     Dsetparamn(1, popnode.node);
                     Dungettok;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 2: // <SEQ>
                     Dsetparamn(0, popnode.node); // cond
                     if(tok==TOKEN_BEGBLOCK) // shortpath to single sequence / sequence list
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SEQ;
                        state_index=0;
                        break;
                     }
                     else
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SSEQ;
                        state_index=0;
                     }
                     break;
                  case 3:
                     // sequence while <EXPR> <SEQ> finished
                     tnode=Dgetparamn(1);
                     tnode.whilestatement->init(Dgetparamn(0).expr, popnode.statement);
                     popnode=tnode;
                     Dungettok;
                     popBreak();
                     popState();
                     break;
               }
               break;

            case ST_DOWHILE:
               switch(state_index)
               {
                  case 0: // do
                     if(tok==TOKEN_DO) state_index++;
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 1: // <SEQ>
                     PTN_NEW_NODE(nn, PTN_DoWhile)();
                     popnode.node=nn;
                     setPNode(popnode.node);
                     pushBreak(popnode.breakable_statement);
                     Dsetparamn(1, popnode.node);

                     if(tok==TOKEN_BEGBLOCK)
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SEQ;
                        state_index=0;
                        break;
                     }
                     else
                     {
                        Dungettok;
                        Dpushstate;
                        state=ST_SSEQ;
                        state_index=0;
                     }
                     break;
                  case 2:
                     Dsetparamn(0, popnode.node); // body
                     if(tok!=TOKEN_WHILE)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 3: // <EXPR>
                     Dungettok;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 4:
                     // sequence do <SEQ> while <EXPR> finished
                     tnode=Dgetparamn(1);
                     tnode.dowhilestatement->init(Dgetparamn(0).statement, popnode.expr);
                     popnode=tnode;
                     Dungettok;
                     popBreak();
                     popState();
                     break;
               }
               break;

            case ST_VARDECL:
               // has to be entered at state_index 0 !!
               switch(state_index)
               {
                  case 0: // <INT|FLOAT|STRING|OBJECT> <var> <;|= <EXPR> ;>
                     last_vardecl = 0;
                     b_var_ref = 0;
                     is_array = 0;
                     b_pointerarray = 0;
                     //vardecl_tag=0;
                     switch(tok)
                     {
                        case TOKEN_TAG: // variable tag token?
                           vardecl_tag|=TKS_CachedObject::TAGGED;
                           break;
                        case TOKEN_LOCAL:
#ifdef TKS_JIT
                           if(!(vmcore||cfunction))
#else
                              if(!(cfunction))
#endif
                              {
                                 tkscript->printf("[---] trying to declare local variable outside of function block.\n");
                                 Dparseerror;
                              }
                           vardecl_tag |= TKS_CachedObject::ISLOCAL;
                           break;
                        case TOKEN_INT: // fetch variable type
                           Dassertnonspace;
                           Dsetparami(0, YAC_TYPE_INT);
                           state_index++;
                           break;
                        case TOKEN_FLOAT:
                           Dassertnonspace;
                           Dsetparami(0, YAC_TYPE_FLOAT);
                           state_index++;
                           break;
                        case TOKEN_OBJECT:
                           Dassertnonspace;
                           Dsetparami(0, YAC_TYPE_OBJECT);
                           Dsetparams(2, &sv_object);
                           state_index++;
                           break;
                        case TOKEN_STRING:
                           Dassertnonspace;
                           Dsetparami(0, YAC_TYPE_STRING);
                           state_index++;
                           break;
                        case TOKEN_MEMBER: // ::MyClass t
                           Ddefaultnspace;
                           break;
                        case TOKEN_VAR:
                           Dparsenspace(10);
                           Dsetparami(0, YAC_TYPE_OBJECT);
                           Dsetparams(2, sv); // remember classname
                           if(sv->compare(&tok_var))
                           {
                              vardecl_tag |= TKS_CachedObject::ISDYNAMIC;
                              Dsetparami(0, 0); // rtti = (void) tags dynamic var
                           }
                           state_index++;
                           break;
                        default:
                           Dparseerror;
                     }
                     break;
                  case 10: // consume '::' between namespace and classname
                     state_index = 0;
                     break;
                  case 11: // consume first ']' in int[]*aoa_int[]
                     if(TOKEN_ENDARRAY != tok)
                     {
                        Dparseerror;
                     }
                     state_index=1;
                     break;
                  case 12: // consume first '[' in Double*[]
                     if(TOKEN_BEGARRAY != tok)
                     {
                        Dparseerror;
                     }
                     state_index=11;
                     break;
                  case 1:
                     if(TOKEN_BEGARRAY == tok)
                     {
                        if(is_array)
                        {
                           Dparseerror;
                        }
                        is_array = 1;
                        // '*' must follow after ']'
                        state_index = 11;
                        break;
                     }
                     if(TOKEN_MUL == tok) // consume '*' in SomeClass*ref
                     {
                        if(b_var_ref)
                        {
                           // SomeClass**ref
                           Dparseerror;
                        }
                        else if(TOKEN_BEGARRAY == ntok)
                        {
                           if(!is_array)
                           {
                              // String*[], Double*[]
                              b_pointerarray = 1;
                              state_index = 12;
                              break;
                           }
                           else
                           {
                              Dparseerror;
                           }
                        }
                        else
                        {
                           b_var_ref = 1;
                           break;
                        }
                     }
                     // // if(is_array && !b_var_ref)
                     // // {
                     // //    // int[]a ..
                     // //    Dparseerror;
                     // // }
                     if(TOKEN_CMPLE == ntok) // SomeClass c <= .. => skip init
                     {
                        if(is_array)
                        {
                           // int[]*a <= ..
                           Dparseerror;
                        }
                        b_var_ref = 1;
                     }
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror; // instance name/classname expected
                     }
                     else
                     {
                        if(cfunction)
                        {
                           co = cfunction->findFunVar(sv);
                        }
                        else
                        {
                           co = Dglobalvars.findEntry(sv);
                        }
                        if(!co)
                        {
                           co = tkscript->global_instance_namespace.findEntry(sv);
                        }
                        if(co)
                        {
                           Dprintf("\n[---] variable \"%s\" is already declared.\n",
                                   (char*)sv->chars);
                           Dparseerror;
                        }
                        co = NULL;
                        Dsetparams(1, sv); // remember varname/classname
                        state_index++; // <VAR> only
                     }
                     break;
                  case 2:
                     if(tok==TOKEN_BEGARRAY)
                     {
/*                           if(last_vardecl)
                             {
                             #ifdef TKS_DCON
                             tkscript->printf("\n[---] array declaration not allowed in variable list.\n");
                             #endif
                             Dparseerror;
                             }*/
                        // begin static array declaration
                        if(is_array)
                        {
                           // array of arrays
                           //  String[] array_of_stringarray[]
                           //  String[] *ref_to_array_of_stringarray[]
                           template_vardecl = 0;/////b_var_ref;
                           b_pointerarray = 1;
                        }
                        if(ntok==TOKEN_ENDARRAY)
                        {
                           state_index=8;
                           PTN_NEW_NODE(nn, PTN_ConstVal)(0);
                           setPNode(nn);
                        }
                        else
                        {
                           if(b_var_ref)
                           {
                              tkscript->printf("\n[---] array size declaration not allowed for array-of-arrays ref\n");
                              Dparseerror;
                           }
                           state_index=7;
                           Dpushstate;
                           state=ST_EXPR;
                           state_index=0;
                        }
                     }
                     else
                     {
                        if(is_array)
                        {
                           // alternative array decl syntax (supports reference to array that is initially null)
                           //   int[]     ia
                           //   int[]    *ia_ref
                           //   String[]  sa
                           //   String[] *sa_ref
                           //   Double[]  da
                           //   Double[] *da_ref
                           state_index=81;
                           Dungettok;  // rewind ';' or ','
                           tok = TOKEN_VAR;  // rewind var name
                           Dungettok;
                           PTN_NEW_NODE(nn, PTN_ConstVal)(0);
                           setPNode(nn);
                           template_vardecl = b_var_ref;
                           Dsetparams(1, sv); // varname
                           break;
                        }
                        // create new instance in variable cache
                        co = NULL;
                        int baseType = Dgetparami(0);
                        if(b_pointerarray)
                        {
                           YAC_Object *o = NULL;
                           sBool o_del = YAC_TRUE;
                           if(b_var_ref)
                           {
                              // reference to PointerArray (initially null)
                              // e.g. Double*[] *pointerarray_ref
                              o = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_POINTERARRAY]->template_object;
                              o_del = 0;
                              b_var_ref = 0;
                           }
                           else
                           {
                              // int*[]*aoa[<expr_or_0>]  => create PointerArray
                              // OR Double*[] mypointerarrayinstance
                              o = YAC_NEW_CORE(YAC_CLID_POINTERARRAY);
                              // // YAC_PointerArray *a = (YAC_PointerArray*)o;
                           }
                           if(cfunction)
                           {
                              co=cfunction->getCreateFunVars()->createObjectEntry(sv, 1, o, o_del);
                           }
                           else
                           {
                              co = Dglobalvars.createObjectEntry(sv, 1, o, o_del);
                           }
                           if(!o_del)
                           {
                              co->flags |= TKS_CachedObject::PTRASSIGN | TKS_CachedObject::NOINIT;
                           }
                           b_pointerarray = 0;
                        }
                        else switch(baseType)
                        {
                           case YAC_TYPE_VARIANT:
                              if(cfunction)
                              {
                                 co = cfunction->getCreateFunVars()->createVariantEntry(Dgetparams(1), 1);
                              }
                              else
                              {
                                 co = Dglobalvars.createVariantEntry(Dgetparams(1), 1);
                              }
                              if(!co)
                              {
                                 Dprintf("\n[---] failed to create dynamic variable \"%s\"\n", (char*)Dgetparams(1)->chars);
                                 Dparseerror;
                              }
                              b_var_ref = 0;
                              break;
                           case YAC_TYPE_INT:
                              if(cfunction)
                              {
                                 co=cfunction->getCreateFunVars()->createIntegerEntry(Dgetparams(1), 1);
                              }
                              else
                              {
                                 co = Dglobalvars.createIntegerEntry(Dgetparams(1), 1);
                              }
                              if(!co)
                              {
                                 Dprintf("\n[---] failed to create integer variable \"%s\"\n", (char*)Dgetparams(1)->chars);
                                 Dparseerror;
                              }
                              b_var_ref = 0;
                              break;
                           case YAC_TYPE_FLOAT:
                              if(cfunction)
                              {
                                 co = cfunction->getCreateFunVars()->createFloatEntry(Dgetparams(1), 1);
                              }
                              else
                              {
                                 co = Dglobalvars.createFloatEntry(Dgetparams(1), 1);
                              }
                              if(!co)
                              {
                                 Dprintf("\n[---] failed to create float variable \"%s\"\n", (char*)Dgetparams(1)->chars);
                                 Dparseerror;
                              }
                              b_var_ref = 0;
                              break;
                           case YAC_TYPE_OBJECT:
                           {
                              TKS_ClassTemplate *clt;
                              TKS_ClassDecl *cd;
                              if(!findDeclaredClassType(Dgetparams(2), active_namespaces, cnspace, &clt, &cd))
                              {
                                 Dparseerror;
                              }
                              else if(cfunction)
                              {
                                 if(template_vardecl)
                                 {
                                    ////co=cfunction->fun_vars.createObjectTemplate(Dgetparams(1), Dgetparams(2));
                                    ////co = cfunction->fun_vars.createAnyEntry(Dgetparams(1), (clt?(void*) clt:(void*) cd) );
                                    co = cfunction->getCreateFunVars()->createObjectEntry(Dgetparams(1), 1, (clt? (clt->template_object): cd->class_template), 0);
                                    if(!co)
                                    {
                                       Dparseerror;
                                    }
                                    if(b_var_ref)
                                    {
                                       co->flags |= TKS_CachedObject::PTRASSIGN | TKS_CachedObject::NOINIT;
                                       b_var_ref = 0;
                                    }
                                 }
                                 else
                                 {
                                    ////co=cfunction->fun_vars.createObjectInstance(Dgetparams(1), Dgetparams(2));
                                    YAC_Object *o;
                                    sBool bOwn;
                                    if(clt)
                                    {
                                       if(b_var_ref || (0 != (vardecl_tag & TKS_CachedObject::ISLOCAL)))
                                       {
                                          o = clt->template_object;
                                          bOwn = 0;
                                       }
                                       else
                                       {
                                          o = YAC_CLONE_POOLED(tkscript->main_context, clt->template_object); // xxx TKS_MT: use *real* thread context (script classes will probably crash)
                                          bOwn = 1;
                                       }
                                    }
                                    else
                                    {
                                       if(cd->b_abstract || b_var_ref || (0 != (vardecl_tag & TKS_CachedObject::ISLOCAL)))
                                       {
                                          o = cd->class_template; // will later be replaced by NULL
                                          bOwn = 0;
                                       }
                                       else
                                       {
                                          o = YAC_CLONE_POOLED(tkscript->main_context, cd->class_template); // xxx TKS_MT: use *real* thread context
                                          bOwn = 1;
                                       }
                                    }
                                    co = cfunction->getCreateFunVars()->createObjectEntry(Dgetparams(1), 1, o, bOwn);
                                    if(!co)
                                    {
                                       Dparseerror;
                                    }
                                    if(b_var_ref)
                                    {
                                       co->flags |= TKS_CachedObject::PTRASSIGN | TKS_CachedObject::NOINIT;
                                       b_var_ref = 0;
                                    }
                                 }
                              }
                              else
                              {
                                 ////co=module_vars.createObjectInstance(Dgetparams(1), Dgetparams(2));
                                 YAC_Object *o;
                                 sBool bOwn;
                                 if(clt)
                                 {
                                    if(b_var_ref)
                                    {
                                       o = clt->template_object;
                                       bOwn = 0;
                                    }
                                    else
                                    {
                                       o = YAC_CLONE_POOLED(tkscript->main_context, clt->template_object); // xxx TKS_MT: use *real* thread context (script classes will probably crash)
                                       bOwn = 1;
                                    }
                                 }
                                 else
                                 {
                                    if(cd->b_abstract || b_var_ref)
                                    {
                                       o = cd->class_template; // will later be replaced by NULL
                                       bOwn = 0;
                                    }
                                    else
                                    {
                                       o = YAC_CLONE_POOLED(tkscript->main_context, cd->class_template); // xxx TKS_MT: use *real* thread context
                                       bOwn = 1;
                                    }
                                 }
                                 co = Dglobalvars.createObjectEntry(Dgetparams(1), 1, o, bOwn);
                                 if(!co)
                                 {
                                    Dparseerror;
                                 }
                                 if(b_var_ref)
                                 {
                                    co->flags |= TKS_CachedObject::PTRASSIGN | TKS_CachedObject::NOINIT;
                                    b_var_ref = 0;
                                 }
                              }
                           }
                           break;
                           case YAC_TYPE_STRING:
                              if(cfunction)
                              {
                                 if(template_vardecl || b_var_ref || (0 != (vardecl_tag & TKS_CachedObject::ISLOCAL)))
                                 {
                                    co = cfunction->getCreateFunVars()->createStringTemplate(Dgetparams(1), 1);
                                 }
                                 else
                                 {
                                    co = cfunction->getCreateFunVars()->createStringInstance(Dgetparams(1), 1);
                                 }
                              }
                              else if(template_vardecl || b_var_ref)
                              {
                                 co = Dglobalvars.createStringTemplate(Dgetparams(1), 1);
                              }
                              else
                              {
                                 co = Dglobalvars.createStringInstance(Dgetparams(1), 1);
                              }
                              if(!co)
                              {
                                 Dprintf("\n[---] failed to create String variable \"%s\"\n", (char*)Dgetparams(1)->chars);
                                 Dparseerror;
                              }
                              if(b_var_ref)
                              {
                                 co->flags |= TKS_CachedObject::PTRASSIGN | TKS_CachedObject::NOINIT;
                                 b_var_ref = 0;
                              }
                              break;
                        }
                        if(!co)
                        {
                           Dprintf("xxx TKS_CachedScript: ever reached???");
                           // xxx ever reached!!?!??
                           YAC_String *classname=Dgetparams(2);
                           sU16 i;
                           for(i=1; i<Dtkscompiler->class_decls.num_elements; i++)
                           {
                              TKS_ClassDecl *cd=Dtkscompiler->class_decls.elements[i];
                              if(cd->name.compare(classname))
                              {
                                 YAC_Object *t;
                                 sBool bOwn;
                                 sBool islocal = ((vardecl_tag&TKS_CachedObject::ISLOCAL)!=0);
                                 if(islocal)
                                 {
                                    t = cd->class_template;
                                    bOwn = 0;
                                 }
                                 else if(cd->b_abstract)
                                 {
                                    t = cd->class_template;
                                    bOwn = 0;
                                 }
                                 else
                                 {
                                    t = cd->class_template->yacNew((void*)tkscript->main_context);
                                    bOwn = 1;
                                 }
                                 if(cfunction)
                                 {
                                    co = cfunction->getCreateFunVars()->createObjectEntry(Dgetparams(1), 1, t, bOwn);
                                    if(vardecl_tag&TKS_CachedObject::ISLOCAL)
                                    {
                                       co->local_index=cfunction->num_local_vars++;
                                    }
                                 }
                                 else
                                 {
                                    co = Dglobalvars.createObjectEntry(Dgetparams(1), 1, t, bOwn);
                                 }
                                 co->flags|=vardecl_tag;
                                 if(tok==TOKEN_CMPLE)
                                 {
                                    co->flags |= TKS_CachedObject::PTRASSIGN;
                                    state_index=8;
                                    Dsetparamv(3, (void *)co);
                                    Dpushstate;
                                    state=ST_EXPR;
                                    state_index=0;
                                 }
                                 else
                                 {
                                    PTN_NEW_NODE(nn, PTN_VarDecl)(co, 0);
                                    PTN_VarDecl *c = (PTN_VarDecl*) nn;
                                    if(last_vardecl)
                                    {
                                       last_vardecl->next=c;
                                       c->prev=last_vardecl;
                                    }
                                    setPNode(c);
                                    if((tok==TOKEN_LISTSEP)&&(state_stack[stp].state!=ST_UFUNDECL))
                                    {
                                       last_vardecl=c;
                                       state_index=1;
                                    }
                                    else
                                    {
                                       // ---- end loop/ function/method arg decl ----
                                       cnspace = NULL;
                                       Dungettok;
                                       popState();
                                    }
                                 }
                                 break;
                              }
                           }
                           // this is only reached when TKSCRIPT_RTTI object type co
                           // could not be created
                           // try forward reference to class instance
                           if(i==Dtkscompiler->class_decls.num_elements)
                           {
                              Dprintf("\n[---] Class \"%s\" undeclared. cannot create instance \"%s\"\n",
                                      (char*)classname->chars,
                                      (char*)Dgetparams(1)->chars);
                              Dparseerror;
                              return 0;
                           }
                           /// xxx try ClassDecl instance
                        }
                        else
                        {
                           co->flags|=vardecl_tag;
                           if(cfunction&&(vardecl_tag&TKS_CachedObject::ISLOCAL))
                              co->local_index=cfunction->num_local_vars++;

                           if(is_tmod && (NULL == cfunction) && (vardecl_tag & TKS_CachedObject::TAGGED))
                           {
                              if(tagged_vars.num_elements < tagged_vars.max_elements)
                              {
                                 tagged_vars.elements[tagged_vars.num_elements++] = co;
                              }
                           }

                           if(tok==TOKEN_ASSIGN) // "="
                           {
                              state_index++; // popstate skips expression
                              Dsetparamv(3, (void *)co);
                              Dpushstate;
                              state=ST_EXPR;
                              state_index=0;
                           }
                           else
                           {
                              if(tok==TOKEN_CMPLE) // "<="
                              {
                                 co->flags |= TKS_CachedObject::PTRASSIGN;
                                 state_index=8;
                                 Dsetparamv(3, (void *)co);
                                 Dpushstate;
                                 state=ST_EXPR;
                                 state_index=0;
                              }
                              else
                              {
                                 // sequence <VARDECL> finished
                                 if(state_stack[stp].state==ST_SSEQSTAT)
                                 {
                                    if((ntok==TOKEN_ENDBLOCK)||((toki+1)>=num_tok))
                                    {
                                       if(!last_vardecl)
                                       {
                                          // falls einziges statement im block
                                          PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                                          setPNode(nn);
                                       }
                                       cnspace = NULL;
                                       Dungettok;
                                       popState();
                                    }
                                    else
                                    {
                                       if((tok==TOKEN_LISTSEP)&&(state_stack[stp].state!=ST_UFUNDECL))
                                       {
                                          state_index=1;//4489;
                                       }
                                       else
                                       {
                                          cnspace = NULL;
                                          Dungettok;
                                          popState();
                                          if(!last_vardecl)
                                             state_index=2; // skip double-linking of statementlist
                                       }
                                    }
                                 }
                                 else
                                 {
                                    PTN_NEW_NODE(nn, PTN_VarDecl)(co, 0);
                                    setPNode(nn);
                                    if(state_stack[stp].state!=ST_UFUNDECL)
                                    {
                                       // allow variable lists
                                       PTN_VarDecl *c=popnode.vardecl;
                                       if(last_vardecl)
                                       {
                                          last_vardecl->next=c;
                                          c->prev=last_vardecl;
                                       }
                                       if(tok==TOKEN_LISTSEP)
                                       {
                                          last_vardecl=c;
                                          // ---- loop vardecl ----
                                          state_index=1;//4489;
                                       }
                                       else
                                       {
                                          // ---- single var decl ----
                                          cnspace = NULL;
                                          Dungettok;
                                          popState();
                                       }
                                    }
                                    else
                                    {
                                       // ---- function/method param decl ----
                                       cnspace = NULL;
                                       Dungettok;
                                       popState();
                                    }
                                 }

                              }
                           }
                        }
                     }
                     break;
                  case 4:
                  case 7:
                     // sequence <VARDECL> + assign finished
                     co2=(TKS_CachedObject*)Dgetparamv(3);
                     PTN_NEW_NODE(nn, PTN_VarDecl)(co2, popnode.expr);
                     setPNode(nn);
                     if(state_stack[stp].state!=ST_UFUNDECL)
                     {
                        // ---- allow vardecl list ----
                        PTN_Statement *c=popnode.statement;
                        if(last_vardecl)
                        {
                           last_vardecl->next=c;
                           c->prev=last_vardecl;
                        }
                        if(tok==TOKEN_LISTSEP)
                        {
                           // ---- loop vardecl ----
                           last_vardecl=c;
                           state_index=1;//4489; // next vardecl
                        }
                        else
                        {
                           // ---- single var decl ----
                           cnspace = NULL;
                           Dungettok;
                           popState();
                        }
                     }
                     else
                     {
                        // ---- function/method arg decl ----
                        cnspace = NULL;
                        Dungettok;
                        popState();
                     }
                     break;
                  case 8: // dynamic array decl
                     if(tok!=TOKEN_ENDARRAY)
                     {
                        Dparseerror;
                     }
                     // run into case 81
                  case 81:
                     {
                        PTN_Expr *expr=popnode.expr;
                        if(expr->isConst())
                        {
                           if(!expr->semanticCheck())
                           {
                              Dprintf("\n[---] array declaration: semanticCheck() failed.\n");
                              delete expr;
                              Dparseerror;
                              break;
                           }
                           if(!expr->resolveXRef())
                           {
                              Dprintf("\n[---] array declaration: resolveXRef() failed.\n");
                              delete expr;
                              Dparseerror;
                              break;
                           }
                           expr->optimize();
                           YAC_Value r;
                           PTN_Env env; env.initDefault();
                           expr->eval(&env, &r);
                           delete expr;
                           popnode.expr=0;
                           if(r.type!=YAC_TYPE_INT)
                           {
                              Dprintf("\n[---] array declaration: expected >>integer<< dimension expression.\n");
                              Dparseerror;
                              break;
                           }
                           if(r.value.int_val>=0) //&&r.value.int_val)
                           {
                              sv=Dgetparams(1); // get variable name
                              {
                                 co = NULL; // create new instance in variable cache
                                 YAC_Object *o = NULL;
                                 sBool o_del = YAC_TRUE;
                                 sU16 co_flags=0;
                                 if(b_pointerarray && b_var_ref)
                                 {
                                    // reference to PointerArray (initially null)
                                    // e.g. Double*[] *pointerarray_ref
                                    o = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_POINTERARRAY]->template_object;
                                    o_del = 0;
                                 }
                                 else if(/*is_array || */(b_pointerarray && !b_var_ref))
                                 {
                                    // XXXXXXXXXXXX int[]*aoa[<expr_or_0>]  => create PointerArray
                                    // OR Double*[] mypointerarrayinstance
                                    // OR Double*[] myarrayofarrays[]
                                    o = YAC_NEW_CORE(YAC_CLID_POINTERARRAY);
                                    YAC_PointerArray *a = (YAC_PointerArray*)o;
                                    a->alloc(r.value.int_val);
                                 }
                                 else switch(Dgetparami(0))
                                 {
                                    case 0: // void/variable type
                                       if(template_vardecl)
                                       {
                                          o=TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_VALUEARRAY]->template_object;
                                          o_del=0;
                                       }
                                       else
                                       {
                                          o = YAC_NEW_CORE(YAC_CLID_VALUEARRAY);
                                          YAC_ValueArray *va=(YAC_ValueArray*)o;
                                          va->alloc(r.value.int_val);
                                       }
                                       vardecl_tag=0;
                                       break;
                                    case YAC_TYPE_INT:
                                       if(template_vardecl)
                                       {
                                          o=TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_INTARRAY]->template_object;
                                          o_del=0;
                                       }
                                       else
                                       {
                                          o = YAC_NEW_CORE(YAC_CLID_INTARRAY);
                                          YAC_IntArray *ia=(YAC_IntArray*)o;
                                          ia->alloc(r.value.int_val);
                                       }
                                       break;
                                    case YAC_TYPE_FLOAT:
                                       if(template_vardecl)
                                       {
                                          o=TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_FLOATARRAY]->template_object;
                                          o_del=0;
                                       }
                                       else
                                       {
                                          o = YAC_NEW_CORE(YAC_CLID_FLOATARRAY);
                                          YAC_FloatArray *a=(YAC_FloatArray*)o;
                                          a->alloc(r.value.int_val);
                                       }
                                       break;
                                    case YAC_TYPE_OBJECT:
                                    {
                                       tsv = Dgetparams(2); // Get class/type name

                                       if(b_var_ref || ( (NULL == cnspace) && (tsv->compare(&sv_object)) ) )
                                       {
                                          if(template_vardecl)
                                          {
                                             o = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_POINTERARRAY]->template_object;
                                             o_del = 0;
                                          }
                                          else
                                          {
                                             // "pointer" array
                                             o = YAC_NEW_CORE(YAC_CLID_POINTERARRAY);
                                             YAC_PointerArray *a = (YAC_PointerArray*)o;
                                             a->alloc(r.value.int_val);
                                          }
                                       }
                                       else
                                       {
                                          if( (NULL == cnspace) && (tsv->compare(&tok_Value)) )
                                          {
                                             if(template_vardecl)
                                             {
                                                o = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_VALUEARRAY]->template_object;
                                                o_del = 0;
                                             }
                                             else
                                             {
                                                // ValueArray
                                                o = YAC_NEW_CORE(YAC_CLID_VALUEARRAY);
                                                YAC_ValueArray*a = (YAC_ValueArray*)o;
                                                a->alloc(r.value.int_val);
                                             }
                                          }
                                          else
                                          {
                                             // Array of script or C++ objects

                                             TKS_ClassTemplate *clt;
                                             TKS_ClassDecl *cd;

                                             // Find class template according to current namespace context
                                             if(!findDeclaredClassType(tsv, active_namespaces, cnspace, &clt, &cd))
                                             {
                                                o = NULL;
                                                Dparseerror;
                                             }
                                             else
                                             {
                                                if(cd)
                                                {
                                                   // Script class
                                                   if(template_vardecl)
                                                   {
                                                      o = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_CLASSARRAY]->template_object;
                                                      o_del = 0;
                                                   }
                                                   else
                                                   {
                                                      YAC_ClassArray *cca = (YAC_ClassArray*)YAC_NEW_CORE(YAC_CLID_CLASSARRAY);
                                                      cca->class_decl = cd;
                                                      cca->alloc(r.value.int_val);
                                                      o = cca;
                                                   }
                                                }
                                                else
                                                {
                                                   // C++ class
                                                   if(template_vardecl)
                                                   {
                                                      o = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_OBJECTARRAY]->template_object;
                                                      o_del = 0;
                                                   }
                                                   else
                                                   {
                                                      o = clt->template_object->yacArrayNew();
                                                      if(!o)
                                                      {
                                                         o = YAC_NEW_CORE(YAC_CLID_OBJECTARRAY);
                                                         if(o)
                                                         {
                                                            // generic Object array
                                                            YAC_ObjectArray *a=(YAC_ObjectArray*)o;
                                                            a->template_object=clt->template_object;
                                                            a->alloc(r.value.int_val);
                                                         }
                                                      }
                                                      else
                                                      {
                                                         // special array object
                                                         o->yacArrayAlloc(r.value.int_val, 0, 0, 0);
                                                      }
                                                   } // else if template_vardecl
                                                }
                                             }
                                          }
                                       } // else if pointer
                                    }
                                    break;
                                    case YAC_TYPE_STRING:
                                       if(template_vardecl)
                                       {
                                          o = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_STRINGARRAY]->template_object;
                                          o_del = 0;
                                       }
                                       else
                                       {
                                          o = YAC_NEW_CORE(YAC_CLID_STRINGARRAY);
                                          YAC_StringArray *a = (YAC_StringArray*)o;
                                          a->alloc(r.value.int_val);
                                       }
                                       break;
                                 }
                                 if(!o)
                                 {
                                    Dprintf("\n[---] failed to create array Object \"%s\" rtti=%s.\n",
                                            (char*)sv->chars, tks_get_rtti_name_by_id((sU8)Dgetparami(0)));
                                    Dparseerror;
                                 }
                                 if(cfunction)
                                 {
                                    co=cfunction->getCreateFunVars()->createObjectEntry(sv, 1, o, o_del);
                                 }
                                 else
                                 {
                                    co = Dglobalvars.createObjectEntry(sv, 1, o, o_del);
                                 }

                                 if(!co)
                                 {
                                    if(o_del)
                                    {
                                       YAC_DELETE(o);
                                    }
                                    Dprintf("\n[---] failed to create array TKS_CachedObject \"%s\" rtti=%s\n", Dgetparams(1), tks_get_rtti_name_by_id((sU8)Dgetparami(0)));
                                    Dparseerror;
                                 }
                                 co->flags=co_flags|vardecl_tag;
                                 if(!o_del && b_var_ref)
                                    co->flags |= TKS_CachedObject::PTRASSIGN | TKS_CachedObject::NOINIT;  // [22Jun2020]
                                 if(cfunction&&(vardecl_tag&TKS_CachedObject::ISLOCAL))
                                    co->local_index=cfunction->num_local_vars++;
                                 if(state_stack[stp].state==ST_SSEQSTAT)
                                 {
                                    if((nntok==TOKEN_ENDBLOCK)||((toki+2)>=num_tok))
                                    {
                                       PTN_NEW_NODE(nn, PTN_EmptyStatement)();
                                       popnode.statement = (PTN_Statement*) nn;
                                       ////popState();
                                    }
                                    else
                                    {
                                       // ---- int ia[]<=[1,2,3,4] !? ----
                                       /*
                                          if((!template_vardecl)&&(ntok==TOKEN_CMPLE)) // "<="
                                          {
                                          Dsetparami(3, (int)co);
                                          state_index=100;
                                          break; // don't pop state
                                          }
                                          else
                                          {
                                          popnode.statement=0;
                                          popState();
                                          state_index=2;
                                          }
                                       */
                                       if(!template_vardecl) // check for "<=", "="
                                       {
                                          if(ntok==TOKEN_CMPLE)
                                          {
                                             Dsetparamv(3, (void *)co); // a b[]<=[...]
                                             state_index=100;
                                             break; // don't pop state
                                          }
                                          else if(ntok==TOKEN_ASSIGN) // a b[]=[...]
                                          {
                                             Dsetparamv(3, (void *)co);
                                             state_index=200;
                                             break; // don't pop state
                                          }

                                       }
                                       popnode.statement=0;
                                       ////popState();
                                       state_index=2;
                                       if(ntok==TOKEN_LISTSEP) // xxx fieber
                                       {
                                          ////////delete expr;
                                          state_index=90; // consume ',' then go to state 1
                                          break;
                                       }
                                    }
                                 }
                                 else
                                 {
                                    PTN_NEW_NODE(nn, PTN_VarDecl)(co, 0);
                                    setPNode(nn);
                                 }
                              }
                              cnspace = NULL;
                              popState();
                           }
                           else
                           {
                              Dprintf("\n[---] array declaration: integer dimension expression must not be negative.\n");
                              //////delete expr;
                              //////popnode.expr=0;
                              Dparseerror;
                              break;
                           }
                        }
                        else
                        {
                           Dprintf("\n[---] array declaration: expected >>constant<< dimension expression.\n");
                           delete expr;
                           popnode.expr=0;
                           Dparseerror;
                           break;
                        }

                        /////delete expr; // array depth expr is not used anymore
                     }
                     break;
                  case 9:
                     co=(TKS_CachedObject*)Dgetparamv(3);
                     // ---- vardecl+pointerassign, <EXPR> finished
                     if(co->flags&TKS_CachedObject::ISDYNAMIC)
                     {
                        PTN_NEW_NODE(nn, PTN_DynamicVarPointerAssign)(co, popnode.expr);
                        setPNode(nn);
                     }
                     else
                     {
                        PTN_NEW_NODE(nn, PTN_PointerAssign)(co, popnode.expr);
                        setPNode(nn);
                     }
                     if(state_stack[stp].state!=ST_UFUNDECL)
                     {
                        // ---- allow variable lists
                        PTN_Statement *c=popnode.statement;
                        if(last_vardecl)
                        {
                           last_vardecl->next=c;
                           c->prev=last_vardecl;
                        }
                        if(tok==TOKEN_LISTSEP)
                        {
                           last_vardecl=c;
                           // ---- loop vardecl ----
                           state_index=1;//4489;
                        }
                        else
                        {
                           // ---- single var decl ----
                           cnspace = NULL;
                           Dungettok;
                           popState();
                        }
                     }
                     else
                     {
                        // ---- function/method param decl (never reached) ----
                        cnspace = NULL;
                        Dungettok;
                        popState();
                     }
                     break;
                  case 90: // xxx fieber
                     // consume ','
                     state_index=1;
                     break;
                  case 100:
                     // consume "<=" token before array ini expr
                     state_index=8;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;

                  case 200:
                     // consume "=" token before array ini expr
                     state_index=6;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;


                  default:
                     Dparseerror;
               }
               break;

            case ST_OBJVARASSIGN: // note: sv and cnspace must already be set
               switch(state_index)
               {
                  /*case 0:
                    if(tok==TOKEN_VAR) // get object/classinstance name
                    {
                    Dsetparams(0,sv);
                    state_index++;
                    }
                    else
                    {
                    Dparseerror;
                    }
                    break;*/
                  case 1:
                     Dparsenspace(5);

                     Dsetparams(0, sv); // Remember objectvarname or classname
                     if(TOKEN_DOT != tok)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 5: // skip '::' between namespace and classname
                     state_index = 1;
                     break;

                  case 2:
                     if(tok==TOKEN_VAR) // get member name
                     {
                        Dsetparams(1, sv);
                        state_index++;
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 3:
                     switch(tok)
                     {
                        case TOKEN_ASSIGN:
                           Dsetparami(2, 0); // operator select
                           break;
                        case TOKEN_ADDASSIGN:
                           Dsetparami(2, YAC_OP_ADD);
                           break;
                        case TOKEN_SUBASSIGN:
                           Dsetparami(2, YAC_OP_SUB);
                           break;
                        case TOKEN_MULASSIGN:
                           Dsetparami(2, YAC_OP_MUL);
                           break;
                        case TOKEN_DIVASSIGN:
                           Dsetparami(2, YAC_OP_DIV);
                           break;
                        case TOKEN_MODASSIGN:
                           Dsetparami(2, YAC_OP_MOD);
                           break;
                        case TOKEN_ASLASSIGN:
                           Dsetparami(2, YAC_OP_SHL);
                           break;
                        case TOKEN_ASRASSIGN:
                           Dsetparami(2, YAC_OP_SHR);
                           break;
                        case TOKEN_BORASSIGN:
                           Dsetparami(2, YAC_OP_OR);
                           break;
                        case TOKEN_BANDASSIGN:
                           Dsetparami(2, YAC_OP_AND);
                           break;
                        case TOKEN_EORASSIGN:
                           Dsetparami(2, YAC_OP_EOR);
                           break;
                        default:
                           Dprintf("\n[---] unknown operator for component member assign.\n");
                           Dparseerror;
                     }
                     Dsetparamnspace(3, cnspace);
                     cnspace = NULL;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 4:
                  {
                     // ST_EXPR finished
                     // determine whether object is c++-object or script-class instance
                     Dungettok;
                     YAC_String *cln=Dgetparams(0);
                     YAC_String *mn=Dgetparams(1);
                     int assignop=Dgetparami(2);
                     cnspace = Dgetparamnspace(3);

                     // xzzz Check if cln is a class name
                     {
                        TKS_ClassDecl *cd;
                        if(NULL != cnspace)
                        {
                           cd = Dtkscompiler->findClassDeclNamespace(cln, cnspace);
                        }
                        else
                        {
                           cd = Dtkscompiler->findClassDeclNamespaceList(cln, active_namespaces);
                        }
                        if(cd)
                        {
                           co = cd->getStaticMember(mn);
                           if(co != NULL)
                           {
                              DcheckMemberAccess(cd, mn);

                              // Static member access, spawn varassign node
                              if(assignop)
                              {
                                 PTN_NEW_NODE(nn, PTN_OpVarAssign)(co, popnode.expr, assignop);
                                 setPNode(nn);
                              }
                              else
                              {
                                 PTN_NEW_NODE(nn, PTN_VarAssign)(co, popnode.expr);
                                 setPNode(nn);
                              }
                              cnspace = NULL;
                              popState();
                              break;
                           }
                           else
                           {
                              Dprintf("\n[---] class \"%s\" has no static member \"%s\".\n",
                                      (char*)cln->chars, (char*)mn->chars);
                              Dparseerror;
                           }
                        }
                     }

                     // xxx find function/global var first
                     co=0;
                     if( (NULL == cnspace) && cclassdecl)
                     {
                        co=cclassdecl->getMember(cln);
                        if(!b_force_this)
                        {
                           // hack to make function vars shadow class members
                           DcheckFunMemberAccess(cln);
                        }
                        if(co)
                        {
                           b_force_this = 0;
                           DcheckMemberAccess(cclassdecl, cln);

                           sU8 mrtti = (sU8) ((co->value.int_val >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK);
                           switch(mrtti)
                           {
                              default:
                                 // int, float
                              {
                                 Dprintf("\n[---] class \"%s\" member \"%s\" has no method named \"%s\".\n",
                                         (char*)cclassdecl->name.chars,
                                         (char*)cln->chars,
                                         (char*)mn->chars
                                         );
                                 Dparseerror;
                              }
                              break;
                              case TKS_CLASSDECL_RTTI_OBJECT:
                              case TKS_CLASSDECL_RTTI_STRING:
                              case TKS_CLASSDECL_RTTI_INTARRAY:
                              case TKS_CLASSDECL_RTTI_FLOATARRAY:
                              case TKS_CLASSDECL_RTTI_OBJECTARRAY:
                              case TKS_CLASSDECL_RTTI_STRINGARRAY:
                              case TKS_CLASSDECL_RTTI_POINTERARRAY:
                              case TKS_CLASSDECL_RTTI_CLASSARRAY:
                              {
                                 // called instance is object
                                 // find associated command
                                 TKS_ClassDeclMember *clm=cclassdecl->findMemberSuper(cln, 0/*non-static*/);
                                 DcheckMemberAccess(cclassdecl, cln);
                                 // must exist since getMember() succeeded
                                 // get template object
                                 YAC_Object *to=clm->value.object_val;
                                 YAC_String classname; classname.visit((char*)to->yacClassName());
                                 TKS_ClassTemplate *clt=tkscript->findTemplate(&classname);
                                 if(!clt)
                                 {
                                    Dprintf("\n[---] unknown class template \"%s\"\n", (char*)classname.chars);
                                    Dparseerror;
                                 }
                                 YAC_String setmethodname;
                                 setmethodname.copy("set");
                                 setmethodname.append(mn);
                                 setmethodname.chars[3]&=~32; // mk uppercase
                                 const YAC_CommandY *set_ccmdy = clt->findCommandY(&setmethodname);
                                 if(!set_ccmdy)
                                 {
                                    Dprintf("\n[---] c++-class \"%s\" has no (writable) property called \"%s\"\n",
                                            (char*)(classname.chars), (char*)(mn->chars));
                                    toki-=3;
                                    Dparseerror;
                                 }
                                 else if(set_ccmdy->y_arg_num!=1)
                                 {
                                    Dprintf("\n[---] class \"%s\" set_command \"%s\" takes more than 1 argument.\n",
                                            (char*)(classname.chars), (char*)(setmethodname.chars));
                                    Dparseerror;
                                 }
                                 else if(set_ccmdy->y_return_type>3)
                                 {
                                    Dprintf("\n[---] class \"%s\" set_command \"%s\" returns a value.\n",
                                            (char*)(classname.chars), (char*)(setmethodname.chars));
                                    Dparseerror;
                                 }

                                 const YAC_CommandY *get_ccmdy=0;
                                 if(assignop) // everything but assign requires getmethod
                                 {
                                    YAC_String getmethodname;
                                    getmethodname.copy("get");
                                    getmethodname.append(mn);
                                    getmethodname.chars[3]&=~32; // mk uppercase
                                    get_ccmdy=clt->findCommandY(&getmethodname);
                                    if(!get_ccmdy)
                                    {
                                       Dprintf("\n[---] c++-class \"%s\" instance \"%s\" has no (readable) property called \"%s\"\n",
                                               (char*)(clt->class_name.chars), (char*)(cln->chars), (char*)(mn->chars));
                                       toki--;
                                       Dparseerror;
                                    }
                                 }
                                 PTN_NEW_NODE(nn, PTN_OpObjectMemberAssignY)(0, get_ccmdy, set_ccmdy, assignop, popnode.expr, clt->template_object->class_ID);
                                 setTNode(nn);
                                 tnode.opobjectmemberassigny->initIndirect();
                                 PTN_NEW_NODE(nn, PTN_ClassMemberIndirectObjectStat)(co, tnode.statement);
                                 setPNode(nn);
                                 ////tnode.opobjectmemberassigny->var = &popnode.classmemberindirectobjectstat->stat_member;
                                 popState();
                                 cnspace = NULL;
                              }
                              break;
                              case TKS_CLASSDECL_RTTI_CLASS:
                              {
                                 // ---- called instance is class
                                 TKS_ClassDeclMember *clm  =cclassdecl->findMemberSuper(cln, 0/*non-static*/);
                                 DcheckMemberAccess(cclassdecl, cln);
                                 //TKS_ClassDecl       *mdecl=clm->value.class_val;
                                 TKS_ClassDecl       *mdecl=clm->value.sobject_val->class_decl;
                                 // ---- get TKS_ClassDeclMethod
                                 TKS_ClassDeclMember *mmember=mdecl->findMemberSuper(mn, 0/*non-static*/);
                                 if(!mmember)
                                 {
                                    Dprintf("\n[---] class \"%s\" has no member named \"%s\"\n",
                                            (char*)(mdecl->name.chars), (char*)(mn->chars));
                                    Dparseerror;
                                 }
                                 TKS_CachedObject *cc_co=mdecl->getMember(mn);
                                 DcheckMemberAccess(mdecl, mn);
                                 PTN_NEW_NODE(nn, PTN_ClassMemberOpAssign)(/*co*/0, cc_co, assignop, popnode.expr);
                                 setTNode(nn);
                                 tnode.classmemberopassign->class_id = mdecl->class_id;
                                 tnode.classmemberopassign->initIndirect();
                                 PTN_NEW_NODE(nn, PTN_ClassMemberIndirectClassStat)(co, tnode.statement);
                                 setPNode(nn);
                                 ////tnode.classmemberopassign->var=&popnode.classmemberindirectclassstat->stat_member;
                                 popState();
                                 cnspace = NULL;
                              }
                              break;
                           }
                        }
                        else if(b_force_this)
                        {
                           Dprintf("[---] class \"%s\" has no member called \"%s\".\n",
                                   (char*)cclassdecl->name.chars,
                                   (char*)cln->chars);
                           Dparseerror;
                        }

                     }
                     if(!co)
                     {
                        co = findVar(cln, active_namespaces, cnspace);
                        if( !co && ((NULL == cnspace) || (cnspace == &Dtkscompiler->yac_namespace)) )
                        {
                           // ---- try to find static class instance ----
                           TKS_ClassTemplate *clt = (TKS_ClassTemplate*)tkscript->findTemplate(cln);
                           if(clt)
                           {
                              if(!clt->may_instantiate)
                              {
                                 co = &clt->static_co;
                              }
                           }
                        }
                        if(co)
                        {
                           // check whether variable is an object
                           if(co->type!=YAC_TYPE_OBJECT)
                           {
                              Dprintf("[---] variable \"%s\" is not an object.\n",
                                      (char*)(cln->chars));
                              Dparseerror;
                           }
                           else
                           {
                              if(!co->value.object_val)
                              {
                                 Dprintf("\n[---] cannot operate on empty (Pointer) object \"%s\".\n",
                                         (char*)cln->chars);
                                 Dparseerror;
                              }

                              if(co->value.object_val->class_ID==YAC_CLID_CLASS)
                              {
                                 TKS_ScriptClassInstance *cc=(TKS_ScriptClassInstance*)co->value.object_val;
                                 TKS_CachedObject *cc_co=cc->getMember(mn);
                                 if(!cc_co)
                                 {
                                    Dprintf("\n[---] class \"%s::%s\" has no member called \"%s\".\n",
                                            (char*)cc->class_decl->nspace->name.chars,
                                            (char*)cc->class_decl->name.chars,
                                            (char*)mn->chars
                                            );
                                    Dparseerror;
                                 }
                                 DcheckMemberAccess(cc->class_decl, mn);
                                 PTN_NEW_NODE(nn, PTN_ClassMemberOpAssign)(co, cc_co, assignop, popnode.expr);
                                 setPNode(nn);
                                 popState();
                                 cnspace = NULL;
                              }
                              else // ! ScriptClass
                              {
                                 // is object instance
                                 YAC_String setmethodname;
                                 setmethodname.copy("set");
                                 setmethodname.append(mn);
                                 setmethodname.chars[3]&=~32; // mk uppercase
                                 YAC_String classname; classname.visit((char*)co->value.object_val->yacClassName());
                                 TKS_ClassTemplate *clt=tkscript->findTemplate(&classname);
                                 if(!clt)
                                 {
                                    // xxx internal error ?
                                    Dprintf("\n[---] unknown class template \"%s\"\n", (char*)classname.chars);
                                    Dparseerror;
                                 }
                                 const YAC_CommandY *set_ccmdy = clt->findCommandY(&setmethodname);
                                 if(!set_ccmdy)
                                 {
                                    Dprintf("\n[---] c++-class \"%s\" has no (writable) property called \"%s\"\n",
                                            (char*)(cln->chars), (char*)(mn->chars));
                                    toki-=3;
                                    Dparseerror;
                                 }
                                 else if(set_ccmdy->y_arg_num!=1)
                                 {
                                    Dprintf("\n[---] c++-class \"%s\" property \"%s\" set method illegal argcount (%i!=1)\n",
                                            (char*)(cln->chars), (char*)(mn->chars), set_ccmdy->y_arg_num);
                                    toki-=3;
                                    Dparseerror;
                                 }
                                 const YAC_CommandY *get_ccmdy = 0;
                                 if(assignop) // everything but assign requires getmethod
                                 {
                                    YAC_String getmethodname;
                                    getmethodname.copy("get");
                                    getmethodname.append(mn);
                                    getmethodname.chars[3]&=~32; // mk uppercase
                                    get_ccmdy=clt->findCommandY(&getmethodname);
                                    if(!get_ccmdy)
                                    {
                                       Dprintf("\n[---] c++-class \"%s\" has no readable property called \"%s\"\n",
                                               (char*)(cln->chars), (char*)(mn->chars));
                                       Dparseerror;
                                    }
                                 }
                                 PTN_NEW_NODE(nn, PTN_OpObjectMemberAssignY)(co, get_ccmdy, set_ccmdy, assignop, popnode.expr, clt->template_object->class_ID);
                                 setPNode(nn);
                                 popState();
                                 cnspace = NULL;
                              }
                              // generate
                           }
                        }
                        else
                        {
                           // must be script-class instance
                           /// xxx find script-class
                           /// cannot resolve now, must instantiate all classes first
                           /// to resolve forward references
                           PTN_NEW_NODE(nn, PTN_UnresolvedOpModuleMemberAssign)(cln, 0, mn, assignop, popnode.expr);
                           setPNode(nn);
                           Dfreezenspaces(popnode.unresolvedopmodulememberassign->nspace_array);
                           popState();
                           cnspace = NULL;
                        }
                     }
                  }
                  break;
               }
               break;

            case ST_VARASSIGN:
               switch(state_index) // note: sv, cnspace, b_force_this must have been set before this state is entered
               {
                  /*case 0:
                    if(tok==TOKEN_VAR)
                    {
                    Dsetparams(0,sv);
                    state_index++;
                    }
                    else
                    {
                    Dparseerror;
                    }
                    break;*/
                  case 1:
                     Dsetparams(0, sv);
                     Dsetparamnspace(1, cnspace);
                     cnspace = NULL;
                     switch(tok)
                     {
                        default:
                        {
                           Dparseerror;
                        }
                        break; // never reached..
                        case TOKEN_ASSIGN:
                           Dsetparami(2, 0); // operator select
                           break;
                        case TOKEN_ADDASSIGN:
                           Dsetparami(2, YAC_OP_ADD);
                           break;
                        case TOKEN_SUBASSIGN:
                           Dsetparami(2, YAC_OP_SUB);
                           break;
                        case TOKEN_MULASSIGN:
                           Dsetparami(2, YAC_OP_MUL);
                           break;
                        case TOKEN_DIVASSIGN:
                           Dsetparami(2, YAC_OP_DIV);
                           break;
                        case TOKEN_MODASSIGN:
                           Dsetparami(2, YAC_OP_MOD);
                           break;
                        case TOKEN_ASLASSIGN:
                           Dsetparami(2, YAC_OP_SHL);
                           break;
                        case TOKEN_ASRASSIGN:
                           Dsetparami(2, YAC_OP_SHR);
                           break;
                        case TOKEN_BORASSIGN:
                           Dsetparami(2, YAC_OP_OR);
                           break;
                        case TOKEN_BANDASSIGN:
                           Dsetparami(2, YAC_OP_AND);
                           break;
                        case TOKEN_EORASSIGN:
                           Dsetparami(2, YAC_OP_EOR);
                           break;
                     }
                     state_index++;
                     break;
                  case 2:
                     Dungettok;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 3: // <EXPR> finished
                     Dungettok;
                     // sequence <VARASSIGN> finished
                     {
                        YAC_String *varname=(YAC_String*)Dgetparams(0);
                        cnspace = Dgetparamnspace(1);
                        sU8 aop = (sU8) Dgetparami(2);

                        co = NULL;

                        if(NULL == cnspace)
                        {
                           if(!b_force_this)
                           {
                              // Try local variable / static class member
                              if(varname->compare(&tok_underscore))
                              {
                                 // _ = ..
                                 co=&tkscript->nil_var;
                              }
                              else
                              {
                                 co = findVarLocal(varname);
                              }
                              if(co)
                              {
                                 if(co->flags&TKS_CachedObject::ISDYNAMIC)
                                 {
                                    if(aop)
                                    {
                                       PTN_NEW_NODE(nn, PTN_OpDynamicVarAssign)(co, popnode.expr, aop);
                                       setPNode(nn);
                                    }
                                    else
                                    {
                                       PTN_NEW_NODE(nn, PTN_DynamicVarAssign)(co, popnode.expr);
                                       setPNode(nn);
                                    }
                                 }
                                 else
                                 {
                                    if(aop)
                                    {
                                       PTN_NEW_NODE(nn, PTN_OpVarAssign)(co, popnode.expr, aop);
                                       setPNode(nn);
                                    }
                                    else
                                    {
                                       PTN_NEW_NODE(nn, PTN_VarAssign)(co, popnode.expr);
                                       setPNode(nn);
                                    }
                                 }
                                 popState();
                                 break;
                              }
                           }


                           // Try regular class member
                           if(cclassdecl)
                           {
                              co=cclassdecl->getMember(varname);
                              if(!b_force_this)
                              {
                                 // hack to make local vars shadow class members
                                 DcheckFunMemberAccess(varname);
                              }
                              if(co)
                              {
                                 b_force_this = 0;
                                 DcheckMemberAccess(cclassdecl, varname);
                                 PTN_NEW_NODE(nn, PTN_ClassMemberOpAssign)(0, co, aop, popnode.expr);
                                 setPNode(nn);
                                 popnode.classmemberopassign->class_id=cclassdecl->class_id;
                                 popState();
                                 break;
                              }
                              else if(b_force_this)
                              {
                                 Dprintf("[---] class \"%s\" has no member called \"%s\".\n",
                                         (char*)cclassdecl->name.chars,
                                         (char*)varname->chars);
                                 Dparseerror;
                              }
                           }
                        }

                        // Try global variable
                        if(!co)
                        {
                           co = findVar(varname, active_namespaces, cnspace);
                           cnspace = NULL;

                           if(!co)
                           {
                              if(NULL == cnspace)
                              {
                                 if(tkscript->configuration.b_enable_unresolved_members)
                                 {
                                    ////YAC_String t;
                                    ////t="Main";
                                    ////setPNode(new PTN_UnresolvedOpModuleMemberAssign(&t, varname, aop, popnode.expr));
                                    PTN_NEW_NODE(nn, PTN_UnresolvedOpModuleMemberAssign)(&name, 1, varname, aop, popnode.expr);
                                    setPNode(nn);
                                    Dfreezenspaces(popnode.unresolvedopmodulememberassign->nspace_array);
                                 }
                                 else
                                 {
                                    Dprintf("[---] cross reference resolving for identifier \"%s\" is turned OFF.\n", varname->chars);
                                    Dparseerror;
                                 }
                              }
                              else
                              {
                                 Dprintf("[---] namespace \"%s\" has no variable named \"%s\".\n",
                                         (char*)cnspace->name.chars,
                                         (char*)varname->chars
                                         );
                                 Dparseerror;
                              }
                           }
                           else
                           {
                              if(co->flags&TKS_CachedObject::ISDYNAMIC)
                              {
                                 if(aop)
                                 {
                                    PTN_NEW_NODE(nn, PTN_OpDynamicVarAssign)(co, popnode.expr, aop);
                                    setPNode(nn);
                                 }
                                 else
                                 {
                                    PTN_NEW_NODE(nn, PTN_DynamicVarAssign)(co, popnode.expr);
                                    setPNode(nn);
                                 }
                              }
                              else
                              {
                                 if(aop)
                                 {
                                    PTN_NEW_NODE(nn, PTN_OpVarAssign)(co, popnode.expr, aop);
                                    setPNode(nn);
                                 }
                                 else
                                 {
                                    PTN_NEW_NODE(nn, PTN_VarAssign)(co, popnode.expr);
                                    setPNode(nn);
                                 }
                              }
                           }
                           popState();
                           break;
                        }
                     }
                     ///////popState();
                     break;

                     /*case 4: // static array indexed varassign
                       if(tok!=TOKEN_VAR)
                       {
                       Dparseerror;
                       }
                       Dsetparams(0, sv); // remember array base name
                       state_index++;
                       break;*/
                  case 5:
                     Dsetparams(0, sv); // remember array base name
                     if(tok!=TOKEN_BEGARRAY)
                     {
                        Dparseerror;
                     }
                     Dsetparamnspace(1, cnspace);
                     cnspace = NULL;
                     if(ntok==TOKEN_ENDARRAY)
                     {
                        state_index=6;
                        PTN_NEW_NODE(nn, PTN_ConstVal)(0);
                        setPNode(nn);
                     }
                     else
                     {
                        Dpushstate;
                        state=ST_EXPR;
                        state_index=0;
                     }
                     break;
                  case 6:
                     if(tok!=TOKEN_ENDARRAY)
                     {
                        Dparseerror;
                     }
                     {
                        sv = Dgetparams(0); // array type name
                        cnspace = Dgetparamnspace(1);

                        PTN_Expr *expr=popnode.expr;
                        {
                           Dsetparamn(1, expr); // remember index expression
                           co = NULL;
                           if(NULL == cnspace)
                           {
                              if(cclassdecl)
                              {
                                 co = cclassdecl->getMember(sv);
                                 DcheckFunMemberAccess(sv);
                                 if(co)
                                 {
                                    DcheckMemberAccess(cclassdecl, sv);
                                    Dsetparami(3, 2);
                                    Dsetparamv(0, (void *)co);
                                 }
                              }
                           }
                           if(!co)
                           {
                              co = findVar(sv, active_namespaces, cnspace);
                              cnspace = NULL;
                              if(co)
                              {
                                 if(co->flags&TKS_CachedObject::ISDYNAMIC)
                                 {
                                    Dsetparamv(2, (void *)co);
                                    state_index=200;
                                    break;
                                 }
                                 // dynamic array
                                 if(co->type<YAC_TYPE_OBJECT)
                                 {
                                    Dprintf("\n[---] trying to index >>%s<< variable \"%s\".\n",
                                            tks_get_rtti_name_by_id((sU8)co->type), (char*)sv->chars);
                                    Dparseerror;
                                 }
                                 Dsetparami(3, 0);
                                 Dsetparamv(0, (void *)co);
                              }
                              else
                              {
                                 Dprintf("\n[---] unknown reference to HashTable/Array variable \"%s\".\n",
                                         (char*)sv->chars);
                                 Dparseerror;
                              }
                           }
                           state_index++;
                        }
                     }
                     break;
                  case 7:
                     switch(tok)
                     {
                        default:
                        {
                           Dparseerror;
                        }
                        break; // never reached..
                        case TOKEN_ASSIGN:
                           Dsetparami(2, 0); // operator select
                           break;
                        case TOKEN_ADDASSIGN:
                           Dsetparami(2, YAC_OP_ADD);
                           break;
                        case TOKEN_SUBASSIGN:
                           Dsetparami(2, YAC_OP_SUB);
                           break;
                        case TOKEN_MULASSIGN:
                           Dsetparami(2, YAC_OP_MUL);
                           break;
                        case TOKEN_DIVASSIGN:
                           Dsetparami(2, YAC_OP_DIV);
                           break;
                        case TOKEN_MODASSIGN:
                           Dsetparami(2, YAC_OP_MOD);
                           break;
                        case TOKEN_ASLASSIGN:
                           Dsetparami(2, YAC_OP_SHL);
                           break;
                        case TOKEN_ASRASSIGN:
                           Dsetparami(2, YAC_OP_SHR);
                           break;
                        case TOKEN_BORASSIGN:
                           Dsetparami(2, YAC_OP_OR);
                           break;
                        case TOKEN_BANDASSIGN:
                           Dsetparami(2, YAC_OP_AND);
                           break;
                        case TOKEN_EORASSIGN:
                           Dsetparami(2, YAC_OP_EOR);
                           break;
                     }
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 8:
                     Dungettok;
                     co=(TKS_CachedObject*)Dgetparamv(0);
                     {
                        int par3=Dgetparami(3);
                        switch(par3)
                        {
                           case 0:
                           {
                              sUI iop=Dgetparami(2);
                              if(iop)
                              {
                                 PTN_NEW_NODE(nn, PTN_OpIndexedVarAssign)(co, Dgetparamn(1).expr, popnode.expr, iop);
                                 setPNode(nn);
                              }
                              else
                              {
                                 PTN_NEW_NODE(nn, PTN_IndexedVarAssign)(co, Dgetparamn(1).expr, popnode.expr);
                                 setPNode(nn);
                              }
                           }
                           break;
                           case 2:
                           {
                              PTN_NEW_NODE(nn, PTN_ClassMemberOpIndexedVarAssign)(0, co, Dgetparamn(1).expr, popnode.expr, Dgetparami(2));
                              setPNode(nn);
                              popnode.classmemberopindexedvarassign->class_id=cclassdecl->class_id;
                           }
                           break;
                        }
                     }
                     popState();
                     break;
                  case 100:
                     // eat a token. mjam.
                     state_index=2;
                     break;
                  case 200:
                     if(tok!=TOKEN_ASSIGN)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 201:
                     Dungettok;
                     PTN_NEW_NODE(nn, PTN_DynamicIndexedVarAssign)((TKS_CachedObject*)Dgetparamv(2), Dgetparamn(1).expr, popnode.expr);
                     setPNode(nn);
                     popState();
                     break;

                  default:
                     Dparseerror;
               }
               break;

            case ST_IARRAYEXPR:
               /** param0 = expr node **/
               /** <EXPR>[ **/
               switch(state_index)
               {
                  case 0:
                     Dungettok;
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 1:
                     /** index expr finished **/
                     if(tok!=TOKEN_ENDARRAY)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 2:
                     PTN_NEW_NODE(nn, PTN_IArrayExpr)(Dgetparamn(0).expr, popnode.expr);
                     setPNode(nn);
                     if(tok==TOKEN_DOT)
                     {
                        /** <EXPR> [ <EXPR> ] . **/
                        Dsetparamn(0, popnode.node);
                        state=ST_IEXPR;
                        state_index=0;
                     }
                     else
                     {
                        if(tok==TOKEN_BEGARRAY)
                        {
                           // multidimensional array expr
                           Dsetparamn(0, popnode.node);
                           state_index=0;
                        }
                        else
                        {
                           Dungettok;
                           popState();
                        }
                     }
                     break;

                  case 100:
                     if(tok!=TOKEN_BEGARRAY)
                     {
                        Dparseerror;
                     }
                     state_index=0;
                     break;
               }
               break;

            case ST_IEXPR:
               /** <EXPR> . **/
               /** param0 = expr node **/
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     if(ntok==TOKEN_BEGEXPR)
                     {
                        // <EXPR> . method ( <ARGS> )
                        Dsetparams(1, sv);
                        state_index++;
                     }
                     else
                     {
                        // <EXPR> . member
                        PTN_NEW_NODE(nn, PTN_IMemberExpr)(Dgetparamn(0).expr, sv);
                        setPNode(nn);
                        if(ntok==TOKEN_BEGARRAY) state_index=3;
                        else
                        {
                           if(ntok==TOKEN_DOT) state_index=4;
                           else
                           {
                              popState();
                           }
                        }
                     }
                     break;
                  case 1:
                     if(tok!=TOKEN_BEGEXPR)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_ARGLIST; //FUNARGS;
                     state_index=0;
                     break;
                  case 2:
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     // <ST_ARGLIST> finished
                     PTN_NEW_NODE(nn, PTN_IMethodECall)(Dgetparamn(0).expr, Dgetparams(1), popnode.arglist);
                     setPNode(nn);
                     Dtryindirectobjectexpr;
                     break;
                  case 3:
                     // <EXPR> . member [
                     Dsetparamn(0, popnode.expr);
                     state=ST_IARRAYEXPR;
                     state_index=0;
                     break;
                  case 4:
                     // <EXPR> . member .
                     Dsetparamn(0, popnode.expr);
                     state_index=0;
                     break;

                  case 100:
                     if(tok!=TOKEN_DOT)
                     {
                        Dparseerror;
                     }
                     state_index=0;
                     break;
               }
               break;


            case ST_HASHINIEXPR:
               switch(state_index)
               {
                  // Param 0: int 1=new #[a=1], 0= [a=1]
                  case 0:
                     if(tok!=TOKEN_HASHINI)
                     {
                        Dparseerror;
                     }
                     PTN_NEW_NODE(nn, PTN_HashIniExpr)(Dgetparami(0));
                     setPNode(nn);
                     Dsetparamn(0, popnode.hashiniexpr);
                     Dsetparamn(1, 0); // last expr
                     state_index++;
                     break;
                  case 1:
                     if(tok!=TOKEN_VAR)
                     {
                        if(tok==TOKEN_ENDARRAY)
                        {
                           // Empty hashtable
                           popnode=Dgetparamn(0);
                           popState();
                           break;
                        }
                        else
                        {
                           Dparseerror;
                        }
                     }
                     Dsetparams(2, sv);
                     state_index++;
                     break;
                  case 2:
                     if(tok!=TOKEN_ASSIGN)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 3:
                     // <EXPR> finished
                     PTN_NEW_NODE(nn, PTN_ExprHashEntry)(popnode.expr, Dgetparams(2));
                     setPNode(nn);
                     tnode=Dgetparamn(1); // get last node
                     if(!tnode.exprhashentry)
                     {
                        tnode=Dgetparamn(0);
                        tnode.hashiniexpr->first_entry=popnode.exprhashentry;
                        tnode.hashiniexpr->num_entries++;
                     }
                     else
                     {
                        tnode.exprhashentry->next=popnode.exprhashentry;
                        tnode=Dgetparamn(0);
                        tnode.hashiniexpr->num_entries++;
                     }

                     if(tok==TOKEN_ENDARRAY)
                     {
                        popnode=Dgetparamn(0);
                        popState();
                     }
                     else
                     {
                        if(tok!=TOKEN_LISTSEP)
                        {
                           Dungettok;
                        }
                        Dsetparamn(1, popnode.exprhashentry);
                        state_index=1;
                     }
                     break;
               }
               break;

            case ST_ARRAYINIEXPR:
               switch(state_index)
               {
                  // Param 0: int 1=new [1,2,3], 0= [1,2,3]
                  case 0:
                     if(tok!=TOKEN_BEGARRAY)
                     {
                        Dparseerror;
                     }
                     PTN_NEW_NODE(nn, PTN_ArrayIniExpr)(0, Dgetparami(0));
                     setPNode(nn);
                     Dsetparamn(0, popnode.arrayiniexpr); // remember first node
                     if(ntok==TOKEN_ENDARRAY)
                     {
                        state_index=10;
                     }
                     else
                     {
                        Dsetparamn(1, 0); // last exprarrayentry
                        Dpushstate;
                        state=ST_EXPR;
                        state_index=0;
                     }
                     break;
                  case 10:
                     /// empty array ini []
                     Dtryindirectobjectexpr;
                     // //popState();
                     break;
                  case 1:
                     // <EXPR> finished
                     PTN_NEW_NODE(nn, PTN_ExprArrayEntry)(popnode.expr);
                     setPNode(nn);
                     tnode=Dgetparamn(1); // get last node
                     if(!tnode.exprarrayentry)
                     {
                        tnode=Dgetparamn(0);
                        tnode.arrayiniexpr->first_entry=popnode.exprarrayentry;
                        tnode.arrayiniexpr->num_entries++;
                     }
                     else
                     {
                        tnode.exprarrayentry->next=popnode.exprarrayentry;
                        tnode=Dgetparamn(0);
                        tnode.arrayiniexpr->num_entries++;
                     }

                     if(tok==TOKEN_ENDARRAY)
                     {
                        popnode=Dgetparamn(0);
                        Dtryindirectobjectexpr;
                        // //popState();
                     }
                     else
                     {
                        if(tok!=TOKEN_LISTSEP)  // ',' is optional
                        {
                           Dungettok;
                        }
                        if((TOKEN_LISTSEP == tok) && (TOKEN_ENDARRAY == ntok))
                        {
                           state_index=2; // consume ']'
                        }
                        else
                        {
                           // <EXPR> , <ARRAYINIEXPR>
                           Dsetparamn(1, popnode.exprarrayentry);
                           state_index=0;
                           Dpushstate;
                           state=ST_EXPR;
                        }
                     }
                     break;

                  case 2: // ']' after ','
                     popnode=Dgetparamn(0);
                     Dtryindirectobjectexpr;
                     break;
               }
               break;

            case ST_VALUELISTEXPR:
               switch(state_index)
               {
                  // Param 0: int=1 => new {1,2,3}, 0={1,2,3}
                  case 0:
                     if(tok!=TOKEN_BEGBLOCK)
                     {
                        Dparseerror;
                     }
                     PTN_NEW_NODE(nn, PTN_ValueListExpr)(Dgetparami(0));
                     setPNode(nn);
                     Dsetparamn(0, popnode.valuelistexpr); // first node
                     if(ntok==TOKEN_ENDBLOCK)
                     {
                        state_index=10;
                     }
                     else
                     {
                        Dsetparamn(1, 0); // last valuelistentry
                        Dpushstate;
                        state=ST_EXPR;
                        state_index=0;
                     }
                     break;
                  case 10:
                     /// empty list {}
                     // //popState();
                     Dtryindirectobjectexpr;
                     break;
                  case 1:
                     // <EXPR> finished
                     PTN_NEW_NODE(nn, PTN_ValueListEntry)(popnode.expr);
                     setPNode(nn);
                     tnode=Dgetparamn(1); // get last node
                     if(!tnode.valuelistentry)
                     {
                        tnode=Dgetparamn(0);
                        tnode.valuelistexpr->first_entry=popnode.valuelistentry;
                     }
                     else
                     {
                        tnode.valuelistentry->next=popnode.valuelistentry;
                        // //tnode=Dgetparamn(0);
                     }

                     if(tok==TOKEN_ENDBLOCK)
                     {
                        popnode=Dgetparamn(0);
                        // //popState();
                        Dtryindirectobjectexpr;
                     }
                     else
                     {
                        if(tok!=TOKEN_LISTSEP)
                        {
                           Dungettok;
                        }
                        if((TOKEN_LISTSEP == tok) && (TOKEN_ENDBLOCK == ntok))
                        {
                           // Consume '}' after ','
                           state_index = 2;
                        }
                        else
                        {
                           // <EXPR> , <EXPR>
                           Dsetparamn(1, popnode.node);
                           state_index=0;
                           Dpushstate;
                           state=ST_EXPR;
                        }
                     }
                     break;

                  case 2:  // Consume '}' after ','
                     popnode = Dgetparamn(0);
                     Dtryindirectobjectexpr;
                     break;
               }
               break;

            case ST_DEREFEXPR:
               switch(state_index)
               {
                  case 1:
                     Dparsenspace(2);

                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }

                     if( (NULL == cnspace) && (current_exception_varname != NULL) )
                     {
                        if(sv->compare(current_exception_varname))
                        {
                           PTN_NEW_NODE(nn, PTN_ExceptionVarExpr)(1);
                           setPNode(nn);
                           Dtryindirectobjectexpr;
                           break;
                        }
                     }
                     else
                     {
                        co = NULL;

                        // Unlink class member object ?
                        if(cclassdecl != NULL)
                        {
                           // check whether its a member of the current class
                           co = cclassdecl->getMember(sv);
                           DcheckFunMemberAccess(sv);
                           if(co)
                           {
                              DcheckMemberAccess(cclassdecl, sv);
                              PTN_NEW_NODE(nn, PTN_ClassMemberExpr)(0, co);
                              setPNode(nn);
                              popnode.classmemberexpr->b_deref = 1;
                              popnode.classmemberexpr->class_id = cclassdecl->class_id;
                           }
                        }

                        // Unlink variable object ?
                        if(co == NULL)
                        {
                           co = findVar(sv, active_namespaces, cnspace);
                           cnspace = NULL;
                           if(!co)
                           {
                              Dprintf("\n[---] cannot find deref variable \"%s\".\n", (char*)sv->chars);
                              Dparseerror;
                           }
                           else
                           {
                              PTN_NEW_NODE(nn, PTN_DerefExpr)(co);
                              setPNode(nn);
                           }
                        }
                        popState();
                     }
                     break;
                  case 2: // skip '::' between namespace and varname
                     state_index = 1;
                     break;
               }
               break;

            case ST_EXPR:
               switch(state_index)
               {
                  case 0: // <BITEXPR>
                     Dsetparami(0, 0/*YAC_OP_NOP*/);
                     Dungettok;
                     Dpushstate;
                     state=ST_BITEXPR;
                     state_index=0;
                     break;
                  case 1: // <CMP> optional
                     switch(tok)
                     {
                        default:
                           // sequence <BITEXPR> finished
                           if(tok==TOKEN_TIF)
                           {
                              // <EXPR> ? <EXPRA> : <EXPRB>
                              state_index=5;
                              Dsetparamn(0, popnode.node);
                              Dpushstate;
                              state=ST_EXPR;
                              state_index=0;
                           }
                           else
                           {
                              if(tok==TOKEN_DOT)
                              {
                                 // <EXPR> .
                                 Dsetparamn(0, popnode.node);
                                 state=ST_IEXPR;
                                 state_index=0;
                              }
                              else
                              {
                                 if(tok==TOKEN_BEGARRAY)
                                 {
                                    /** <EXPR>[<EXPR>] **/
                                    Dsetparamn(0, popnode.node);
                                    state=ST_IARRAYEXPR;
                                    state_index=0;
                                 }
                                 else
                                 {
                                    Dungettok;
                                    popState();
                                 }
                              }
                           }
                           break;
                           // ---- int i=***10+1*** ----
                        case TOKEN_LAND:  // "&&"
                           Dsetparami(0, YAC_OP_LAND);
                           Dsetparamn(1, popnode.node);
                           state_index+=2;  // <CMP> finished, skip token
                           break;
                        case TOKEN_LOR:   // "||"
                           Dsetparami(0, YAC_OP_LOR);
                           Dsetparamn(1, popnode.node);
                           state_index+=2;  // <CMP> finished, skip token
                           break;
                        // // case TOKEN_INSTANCEOF: // <expr> instanceof <expr>  [22Aug2024] now a relop (see ST_RELEXPR)
                        // //    Dsetparamn(1, popnode.node);
                        // //    state_index=10-1;
                        // //    Dpushstate;
                        // //    state_index=0;
                        // //    state=ST_EXPR;
                        // //    break;

                     }
                     break;
                  case 2: // <BITEXPR>
                  case 3: // <BITEXPR>
                     Dungettok;
                     Dpushstate;
                     state=ST_BITEXPR;
                     state_index=0;
                     break;
                  case 4:
                  {
                     // sequence <EXPR> (<BITEXPR> <CMP> <BITEXPR>) finished
                     {
                        PTN_NEW_NODE(nn, PTN_DoubleArgExpr)(Dgetparamn(1).expr, Dgetparami(0), popnode.expr);
                        setPNode(nn);
                        if(tok!=TOKEN_TIF)
                        {
                           switch(tok)
                           {
                              default:
                                 Dungettok;
                                 popState();
                                 break;

                              case TOKEN_LAND: // "&&"
                                 Dsetparami(0, YAC_OP_LAND);
                                 Dsetparamn(1, popnode.node);
                                 state_index=3;
                                 break;

                              case TOKEN_LOR: // "||"
                                 Dsetparami(0, YAC_OP_LOR);
                                 Dsetparamn(1, popnode.node);
                                 state_index=3;
                                 break;
                           }
                        }
                        else
                        {
                           // ---- enter ternary operator a ? b : c ----
                           Dsetparamn(0, popnode.node);
                           state_index=5;
                           Dpushstate;
                           state=ST_EXPR;
                           state_index=0;
                        }
                     }
                     break;
                  }
                  break;
                  case 6: // conditional operator
                     // <EXPRA> finished
                     // <EXPR> ? <EXPRA> : <EXPRB>
                     if(tok!=TOKEN_TELSE)
                     {
                        Dparseerror;
                     }
                     Dsetparamn(1, popnode.node);
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 7: // conditional operator
                     // <EXPRB> finished
                     Dungettok;
                     PTN_NEW_NODE(nn, PTN_ConditionalExpr)(Dgetparamn(0).expr, Dgetparamn(1).expr, popnode.expr);
                     setPNode(nn);
                     popState();
                     break;

                  // // case 10: // <expr> instanceof <exprb>  [22Aug2024] now a relop (see ST_RELEXPR)
                  // //    Dungettok;
                  // //    PTN_NEW_NODE(nn, PTN_InstanceOfExpr)(Dgetparamn(1).expr, popnode.expr);
                  // //    setPNode(nn);
                  // //    popState();
                  // //    break;
               }
               break;


            case ST_BITEXPR:
               // param0: YAC_OP_xxx
               // param1: left hand side expr(sexpr)
               switch(state_index)
               {
                  case 0: // <RELEXPR>
                     Dsetparami(0, 0/*YAC_OP_NOP*/);
                     Dungettok;
                     Dpushstate;
                     state=ST_RELEXPR;
                     state_index=0;
                     break;

                  case 1:
                     // <RELEXPR> finished
                     switch(tok)
                     {
                        default:
                           // sequence <RELEXPR> finished
                           popState();
                           Dungettok;
                           break;

                        case TOKEN_BAND: // "&"
                           Dsetparami(0, YAC_OP_AND);
                           Dsetparamn(1, popnode.node);
                           state_index++;
                           break;

                        case TOKEN_BOR:  // "|"
                           Dsetparami(0, YAC_OP_OR);
                           Dsetparamn(1, popnode.node);
                           state_index++;
                           break;

                        case TOKEN_EOR: // "^"
                           Dsetparami(0, YAC_OP_EOR);
                           Dsetparamn(1, popnode.node);
                           state_index++;
                           break;

                     }
                     break;

                  case 2:
                     Dungettok;
                     Dpushstate;
                     state=ST_RELEXPR;
                     state_index=0;
                     break;

                  case 3:
                     // right hand side <RELEXPR> finished
                     PTN_NEW_NODE(nn, PTN_DoubleArgExpr)(Dgetparamn(1).expr, Dgetparami(0), popnode.expr);
                     setPNode(nn);
                     Dungettok;
                     state_index = 1; // Loop BITEXPR sequence
                     break;
               }
               break;


            case ST_RELEXPR:
               // param0: YAC_OP_xxx
               // param1: left hand side expr(sexpr)
               // param2: middle expr(sexpr) (only in tripleargexpr state)
               // param3: right operator (only in tripleargexpr state)
               switch(state_index)
               {
                  case 0: // <SEXPR>
                     Dsetparami(0, 0/*YAC_OP_NOP*/);
                     Dungettok;
                     Dpushstate;
                     state=ST_SEXPR;
                     state_index=0;
                     break;

                  case 1:
                     // <SEXPR> finished
                     switch(tok)
                     {
                        default:
                           // sequence <RELEXPR> finished
                           popState();
                           Dungettok;
                           break;

                        case TOKEN_CMPEQ: // "=="
                           Dsetparami(0, YAC_OP_CEQ);
                           Dsetparamn(1, popnode.node);
                           state_index++;  // <CMP> finished, skip token
                           break;
                        case TOKEN_CMPNE: // "!="
                           Dsetparami(0, YAC_OP_CNE);
                           Dsetparamn(1, popnode.node);
                           state_index++;  // <CMP> finished, skip token
                           break;
                        case TOKEN_CMPLE: // "<="
                           Dsetparami(0, YAC_OP_CLE);
                           Dsetparamn(1, popnode.node);
                           state_index++;  // <CMP> finished, skip token
                           break;
                        case TOKEN_CMPGE: // ">=
                           Dsetparami(0, YAC_OP_CGE);
                           Dsetparamn(1, popnode.node);
                           state_index++;  // <CMP> finished, skip token
                           break;
                        case TOKEN_CMPLT: // "<"
                           Dsetparami(0, YAC_OP_CLT);
                           Dsetparamn(1, popnode.node);
                           state_index++;  // <CMP> finished, skip token
                           break;
                        case TOKEN_CMPGT: // ">"
                           Dsetparami(0, YAC_OP_CGT);
                           Dsetparamn(1, popnode.node);
                           state_index++;  // <CMP> finished, skip token
                           break;
                        case TOKEN_INSTANCEOF: // <sexpr> instanceof <sexpr>
                           Dsetparamn(1, popnode.node);
                           state_index=5-1;
                           Dpushstate;
                           state_index=0;
                           state=ST_SEXPR;
                           break;
                     }
                     break;

                  case 2:
                     Dungettok;
                     Dpushstate;
                     state=ST_SEXPR;
                     state_index=0;
                     break;

                  case 3:
                     // right hand side <SEXPR> finished
                     if( (tok>=TOKEN_CMPLE) && (tok<=TOKEN_CMPGT) )
                     {
                        // ---- enter tripleargexpr a<b<c ----
                        Dsetparamn(2, popnode.expr);
                        switch(tok)
                        {
                           case TOKEN_CMPEQ:	Dsetparami(3, YAC_OP_CEQ);	break;
                           case TOKEN_CMPNE:	Dsetparami(3, YAC_OP_CNE);	break;
                           case TOKEN_CMPLT:	Dsetparami(3, YAC_OP_CLT);	break;
                           case TOKEN_CMPLE:	Dsetparami(3, YAC_OP_CLE);	break;
                           case TOKEN_CMPGT:	Dsetparami(3, YAC_OP_CGT);	break;
                           case TOKEN_CMPGE:	Dsetparami(3, YAC_OP_CGE);	break;
                        }

                        Dpushstate;
                        state=ST_SEXPR;
                        state_index=0;
                     }
                     else
                     {
                        PTN_NEW_NODE(nn, PTN_DoubleArgExpr)(Dgetparamn(1).expr, Dgetparami(0), popnode.expr);
                        setPNode(nn);
                        Dungettok;
                        popState(); // RELEXPR sequence finished
                     }
                     break;

                  case 4:
                     // interval end expr finished
                     PTN_NEW_NODE(nn, PTN_TripleArgExpr)(Dgetparamn(1).expr, Dgetparami(0), Dgetparamn(2).expr, Dgetparami(3), popnode.expr);
                     setPNode(nn);
                     Dungettok;
                     state_index = 1; // Loop RELEXPR sequence
                     break;

                  case 5:  // <sexpr> instanceof <sexprb>
                     Dungettok;
                     PTN_NEW_NODE(nn, PTN_InstanceOfExpr)(Dgetparamn(1).expr, popnode.expr);
                     setPNode(nn);
                     popState();
                     break;

               }
               break;


            case ST_SEXPR:
               // param0: YAC_OP_xxx
               // param1: left hand side expr(addexpr)
               switch(state_index)
               {
                  case 0: // <ADDEXPR>
                     Dsetparami(0, 0/*YAC_OP_NOP*/);
                     Dungettok;
                     Dpushstate;
                     state=ST_ADDEXPR;
                     state_index=0;
                     break;

                  case 1:
                     // <ADDEXPR> finished
                     switch(tok)
                     {
                        default:
                           // sequence <SEXPR> finished
                           popState();
                           Dungettok;
                           break;

                        case TOKEN_ASL:  // "<<"
                           Dsetparami(0, YAC_OP_SHL);
                           Dsetparamn(1, popnode.node);
                           state_index++;
                           break;

                        case TOKEN_ASR:  // ">>"
                           Dsetparami(0, YAC_OP_SHR);
                           Dsetparamn(1, popnode.node);
                           state_index++;
                           break;

                     }
                     break;

                  case 2:
                     Dungettok;
                     Dpushstate;
                     state=ST_ADDEXPR;
                     state_index=0;
                     break;

                  case 3:
                     // right hand side <ADDEXPR> finished
                     PTN_NEW_NODE(nn, PTN_DoubleArgExpr)(Dgetparamn(1).expr, Dgetparami(0), popnode.expr);
                     setPNode(nn);
                     Dungettok;
                     state_index = 1; // Loop ADDEXPR sequence
                     break;
               }
               break;


            case ST_ADDEXPR:
               // param0: YAC_OP_xxx
               // param1: left hand side expr(term)
               switch(state_index)
               {
                  case 0: // <TERM>
                     Dsetparami(0, 0/*YAC_OP_NOP*/);
                     Dungettok;
                     Dpushstate;
                     state=ST_TERM;
                     state_index=0;
                     break;

                  case 1:
                     // <TERM> finished
                     switch(tok)
                     {
                        default:
                           // sequence <ADDEXPR> finished
                           popState();
                           Dungettok;
                           break;

                        case TOKEN_ADD: // "+"
                           Dsetparami(0, YAC_OP_ADD);
                           Dsetparamn(1, popnode.node);
                           state_index++;
                           break;

                        case TOKEN_SUB: // "-"
                           Dsetparami(0, YAC_OP_SUB);
                           Dsetparamn(1, popnode.node);
                           state_index++;
                           break;

                     }
                     break;

                  case 2:
                     Dungettok;
                     Dpushstate;
                     state=ST_TERM;
                     state_index=0;
                     break;

                  case 3:
                     // right hand side <TERM> finished
                     PTN_NEW_NODE(nn, PTN_DoubleArgExpr)(Dgetparamn(1).expr, Dgetparami(0), popnode.expr);
                     setPNode(nn);
                     Dungettok;
                     state_index = 1; // Loop ADDEXPR sequence
                     break;
               }
               break;


            case ST_TERM:
               // param0: YAC_OP_xxx
               // param1: left hand side expr(factor)
               switch(state_index)
               {
                  case 0: // <FACTOR>
                     Dsetparami(0, 0/*YAC_OP_NOP*/);
                     Dungettok;
                     Dpushstate;
                     state=ST_FACTOR;
                     state_index=0;
                     break;

                  case 1:
                     // <FACTOR> finished
                     switch(tok)
                     {
                        default:
                           // sequence <TERM> finished
                           popState();
                           Dungettok;
                           break;

                        case TOKEN_MUL: // "*"
                           Dsetparami(0, YAC_OP_MUL);
                           Dsetparamn(1, popnode.node);
                           state_index++;
                           break;

                        case TOKEN_DIV: // "/"
                           Dsetparami(0, YAC_OP_DIV);
                           Dsetparamn(1, popnode.node);
                           state_index++;
                           break;

                        case TOKEN_MOD: // "%"
                           Dsetparami(0, YAC_OP_MOD);
                           Dsetparamn(1, popnode.node);
                           state_index++;
                           break;

                     }
                     break;

                  case 2:
                     Dungettok;
                     Dpushstate;
                     state=ST_FACTOR;
                     state_index=0;
                     break;

                  case 3:
                     // right hand side <FACTOR> finished
                     PTN_NEW_NODE(nn, PTN_DoubleArgExpr)(Dgetparamn(1).expr, Dgetparami(0), popnode.expr);
                     setPNode(nn);
                     Dungettok;
                     state_index = 1; // Loop TERM sequence
                     break;
               }
               break;


            case ST_RGB:
               switch(state_index)
               {
                  case 1:
                     // par i0 == 1 => RGBA, 2 => RGBA_4B
                     if(tok!=TOKEN_BEGEXPR)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 2: // r
                     if(tok!=TOKEN_LISTSEP)
                     {
                        Dparseerror;
                     }
                     Dsetparamn(1, popnode.node);
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 3: // g
                     if(tok!=TOKEN_LISTSEP)
                     {
                        Dparseerror;
                     }
                     Dsetparamn(2, popnode.node);
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 4: // b
                     if(Dgetparami(0))
                     {
                        if(tok!=TOKEN_LISTSEP)
                        {
                           Dparseerror;
                        }
                        Dsetparamn(3, popnode.node);
                        Dpushstate;
                        state=ST_EXPR;
                        state_index=0;
                     }
                     else
                     {
                        if(tok!=TOKEN_ENDEXPR)
                        {
                           Dparseerror;
                        }
                        PTN_NEW_NODE(nn, PTN_RGBExpr)(Dgetparamn(1).expr, Dgetparamn(2).expr, popnode.expr);
                        setPNode(nn);
                        popState();
                     }
                     break;
                  case 5:
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     PTN_NEW_NODE(nn, PTN_ARGBExpr)(Dgetparamn(1).expr, Dgetparamn(2).expr, Dgetparamn(3).expr, popnode.expr);
                     if(2 == Dgetparami(0))
                     {
                        ((PTN_ARGBExpr*)nn)->b_rgba_4b = YAC_TRUE;
                     }
                     setPNode(nn);
                     popState();
                     break;
               }
               break;

            case ST_FACTOR:
               // param 2: 1==unary op bind
               switch(state_index)
               {
                  case 0:
                     Dsetparami(0,0/*YAC_OP_NOP*/);
                     // run into case 9
                  case 9:
                     switch(tok)
                     {
                        case TOKEN_MEMBER:
                           Ddefaultnspace;
                           break;
                        case TOKEN_BEGARRAY: // [1,2,3]
                           Dungettok;
                           Dsetparami(0,0); // disable new [] by default
                           state=ST_ARRAYINIEXPR;
                           state_index=0;
                           break;
                        case TOKEN_HASHINI: // #["a"=1,"b"=2]
                           Dungettok;
                           Dsetparami(0,0); // disable new [] by default
                           state=ST_HASHINIEXPR;
                           state_index=0;
                           break;
                        case TOKEN_BEGBLOCK: // {1,2,3}
                           Dungettok;
                           Dsetparami(0,0);
                           state=ST_VALUELISTEXPR;
                           state_index=0;
                           break;
                        case TOKEN_NEW: // "new"
                           switch(ntok)
                           {
                              case TOKEN_BEGARRAY: // new [1,2,3]
                                 Dsetparami(0,1); // new
                                 state=ST_ARRAYINIEXPR;
                                 state_index=0;
                                 break;
                              case TOKEN_HASHINI: // new #["a"=1,"b"=2]
                                 Dsetparami(0,1); // new
                                 state=ST_HASHINIEXPR;
                                 state_index=0;
                                 break;
                              case TOKEN_BEGBLOCK: // {1,2,3}
                                 Dsetparami(0,1); // new
                                 state=ST_VALUELISTEXPR;
                                 state_index=0;
                                 break;
                              default:
                                 Dungettok;
                                 Dpushstate;
                                 state=ST_NEWEXPR;
                                 state_index=0;
                                 break;
                           }
                           break;

                        case TOKEN_BEGEXPR: // "("
                           if( (nntok==TOKEN_ENDEXPR) && ((ntok==TOKEN_INT)||(ntok==TOKEN_FLOAT)||(ntok==TOKEN_CHARS)||(ntok==TOKEN_OBJECT)||(ntok==TOKEN_STRING))  )
                           {
                              // (int), (float), (Object), (String), (char)
                              Dpushstate;
                              state_index=10;
                              state=ST_BUILTINFUN;
                           }
                           else
                           {
                              state_index=1; // popstate skips <EXPR>
                              Dpushstate;
                              state=ST_EXPR;
                              state_index=0;
                           }
                           break;
                        case TOKEN_DEREF:
                           state_index=1;
                           state=ST_DEREFEXPR;
                           break;
                        case TOKEN_NOT:     // "!"
                           Dsetparami(0, YAC_OP_NOT);
                        case TOKEN_BITNOT:   // "!"
                           if(tok==TOKEN_BITNOT)
                              Dsetparami(0, YAC_OP_BITNOT);
                        case TOKEN_SUB:	   // "-"
                           if(tok==TOKEN_SUB)
                           {
                              Dsetparami(0,YAC_OP_SUB);
                           }
                           state_index=2; // popstate skips <FACTOR>
                           Dpushstate;
                           state=ST_FACTOR;
                           state_index=0; // skip param3 init
                           break;
                        case TOKEN_INT:
                        case TOKEN_FLOAT:
                        case TOKEN_CHARS:
                           ///case TOKEN_STRING:
                           ///case TOKEN_OBJECT:
                        case TOKEN_ADDRESS:
                           Dungettok; // builtin function call
                           Dpushstate;
                           state=ST_BUILTINFUN;
                           state_index=0;
                           break;
                        case TOKEN_VALUE:
                           Dpushstate;
                           state=ST_BUILTINFUN;
                           state_index=1;
                           Dsetparami(0, TKS_BFUN_VALUE); // 19=TKS_BFUN_VALUE
                           Dpushstate;
                           state=ST_EXPR;
                           state_index=0;
                           break;
                        case TOKEN_INC: // "++"
                           if(nntok==TOKEN_DOT)
                           {
                              // ++object.member, ++classinstance.member
                              Dungettok;
                              state_index=3;
                              Dpushstate;
                              state=ST_PREINCDECREADPROPERTY;
                              state_index=0;
                           }
                           else
                           {
                              // ++var
                              Dungettok;
                              state_index=3;
                              Dpushstate;
                              state=ST_PREINCVAREXPR;
                              state_index=0;
                           }
                           break;
                        case TOKEN_DEC: // "--"
                           if(nntok==TOKEN_DOT)
                           {
                              // --object.member, --classinstance.member
                              Dungettok;
                              state_index=3;
                              Dpushstate;
                              state=ST_PREINCDECREADPROPERTY;
                              state_index=0;
                           }
                           else
                           {
                              Dungettok;
                              state_index=3;
                              Dpushstate;
                              state=ST_PREDECVAREXPR;
                              state_index=0;
                           }
                           break;
                        case TOKEN_OBJECT:
                           if(ntok==TOKEN_BEGEXPR)
                           {
                              Dungettok; // builtin function call
                              Dpushstate;
                              state=ST_BUILTINFUN;
                              state_index=0;
                           }
                           else
                           {
                              PTN_NEW_NODE(nn, PTN_ConstVal)();
                              setPNode(nn);
                              popnode.constval->const_value.initObject(TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_OBJECT]->template_object, 0);
                              popState();
                           }
                           break;
                        case TOKEN_STRING:
                           if(ntok==TOKEN_BEGEXPR)
                           {
                              Dungettok; // builtin function call
                              Dpushstate;
                              state=ST_BUILTINFUN;
                              state_index=0;
                           }
                           else
                           {
                              PTN_NEW_NODE(nn, PTN_ConstVal)();
                              setPNode(nn);
                              popnode.constval->const_value.initObject(TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_STRING]->template_object, 0);
                              popState();
                           }
                           break;
                        case TOKEN_E:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sF32)sM_E);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_LOG10:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sF32)sM_LOG10E);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_LN10:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sF32)sM_LN10);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_1PI:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sF32)(1.0/sM_PI));
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_PI:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sF32)sM_PI);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_RANDMAX:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sSI)RAND_MAX);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_PI2:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sF32)sM_PI*0.5f);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_2PI:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sF32)(2*sM_PI)/*sPI2*/);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_SQRT2:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sF32)sM_SQRT2);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_1SQRT2:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sF32)sM_SQRT1_2);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_TRUE:
                           //case TOKEN_ON:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sSI)1);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_NULL:
                           PTN_NEW_NODE(nn, PTN_ConstVal)(0);
                           setPNode(nn);
#ifdef TKS_JIT // only when using *real* jit instead of the stub
                           if(vmcore)
                           {
                              popnode.constval->const_value.initObject(0, 0);
                           }
                           else
#endif
                           {
                              popnode.constval->const_value.initNull();
                           }
                           popState();
                           break;
                        case TOKEN_VOID:
                           PTN_NEW_NODE(nn, PTN_ConstVal)(0);
                           setPNode(nn);
                           popnode.constval->const_value.initVoid();
                           popState();
                           break;
                        case TOKEN_DEFAULT:
                        case TOKEN_FALSE:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sSI)0);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_MAYBE:
                           PTN_NEW_NODE(nn, PTN_ConstVal)((sSI)-1);
                           setPNode(nn);
                           popState();
                           break;
                        case TOKEN_DELEGATE:
                           Dpushstate;
                           Dsetparami(0, 0); // emit expression
                           state=ST_DELEGATECALL;
                           state_index=1;
                           break;
                        case TOKEN_THIS:
                           if(cclassdecl)
                           {
                              PTN_NEW_NODE(nn, PTN_ThisExpr)();
                              setPNode(nn);
                              Dtryindirectobjectexpr; // ???
                              //popState();
                           }
                           else
                           {
                              Dprintf("\n[---] \"this\" cannot be used outside classes.\n");
                              Dparseerror;
                           }
                           break;
                        case TOKEN_VAR:

                           if(NULL == cnspace)
                           {
                              // tkscript->printf("xxx parse TOKEN_VAR sv->getQuotFlag()=%d sv->chars=\"%s\"\n", sv->getQuotFlag(), sv->chars);
                              // Is string constant ("hello, world.")
                              if(sv->getQuotFlag())
                              {
                                 goto st_factor_string_const; // skip regular <var>::member, <var>.<member>, <var>() expr.
                              }

                              // Check if its a builtin function name, '(' must follow in this case
                              if(ntok == TOKEN_BEGEXPR)
                              {
                                 sU8 bfunId = tks_get_pseudotoken_id_by_name(sv);
                                 if(bfunId > 0)
                                 {
                                    if(bfunId < TKS_BFUN_PAD)
                                    {
                                       Dpushstate;
                                       state=ST_BUILTINFUN;
                                       Dsetparami(0, bfunId);
                                       state_index=1;
                                       break;
                                    }
                                    else if(bfunId < TKS_BFUN2_LAST)
                                    {
                                       if(TKS_BFUN2_RGB == bfunId)
                                       {
                                          Dpushstate;
                                          Dsetparami(0, 0);
                                          state_index=1;
                                          state=ST_RGB;
                                          break;
                                       }
                                       else if(TKS_BFUN2_ARGB == bfunId)
                                       {
                                          Dpushstate;
                                          Dsetparami(0, 1);
                                          state_index=1;
                                          state=ST_RGB;
                                          break;
                                       }
                                       else if(TKS_BFUN2_RGBA_4B == bfunId)
                                       {
                                          Dpushstate;
                                          Dsetparami(0, 2);
                                          state_index=1;
                                          state=ST_RGB;
                                          break;
                                       }
                                    }
                                 }
                              } // check builtin fun pseudotoken

                              if(current_exception_varname != NULL)
                              {
                                 if(sv->compare(current_exception_varname))
                                 {
                                    PTN_NEW_NODE(nn, PTN_ExceptionVarExpr)(0);
                                    setPNode(nn);
                                    Dtryindirectobjectexpr;
                                    break;
                                 }
                              }

                              if(sv->compare(&tok_underscore))
                              {
#ifdef DX_SCRIPTCONSTRAINTS
                                 if(current_constraint)
                                 {
                                    PTN_NEW_NODE(nn, PTN_ConstraintVarExpr)(&current_constraint->cval);
                                    setPNode(nn);
                                    popState();
                                    break;
                                 }
                                 else
                                 {
                                    Dprintf("[---] trying to use \"_\" outside of a constraint expr.\n");
                                    Dparseerror;
                                 }
#else
                                 // dummy code
                                 PTN_NEW_NODE(nn, PTN_ConstVal);
                                 setPNode(nn);
                                 popnode.constval->const_value.initInt(0);
                                 popState();
#endif // DX_SCRIPTCONSTRAINTS
                                 break;
                              }
                           } // if cnspace is null

                           if(TOKEN_MEMBER == ntok)
                           {
                              // is namespace ?
                              if(NULL != cnspace)
                              {
                                 // namespace already selected
                                 if(TOKEN_BEGEXPR == nnntok)
                                 {
                                    // mynamespace::myclass::mymethod(
                                    Dungettok;
                                    Dpushstate;
                                    Dsetparami(0, 0);
                                    state=ST_CLASSSUPERECALL;
                                    state_index=0;
                                 }
                                 else
                                 {
                                    Dparseerror;
                                 }
                              }
                              else
                              {
                                 cnspace = tkscript->findNamespace(sv);
                                 if(NULL == cnspace)
                                 {
                                    // Is not a namespace, try MyClass::mymethod(
                                    Dungettok;
                                    Dpushstate;
                                    Dsetparami(0, 0);
                                    state=ST_CLASSSUPERECALL;
                                    state_index=0;
                                 }
                                 else
                                 {
                                    if(TOKEN_VAR == nntok) // namespace::class/varname
                                    {
                                       // skip '::' between namespace and class/varname
                                       state_index = 10;
                                    }
                                    else
                                    {
                                       Dparseerror;
                                    }
                                    break;
                                 }
                              }
                           }
                           else
                           {
                              if(ntok==TOKEN_DOT)
                              {
                                 if(nnntok!=TOKEN_BEGEXPR)
                                 {
                                    // object.member / classinstance.member
                                    Dungetstr;
                                    state_index = 0; // reset from 9 (nspace parsing)
                                    Dpushstate;
                                    state=ST_READPROPERTY;
                                    Dsetparami(2, 0);
                                    state_index=0;
                                 }
                                 else
                                 {
                                    // object.method(...
                                    Dungetstr;
                                    state_index = 0; // reset from 9 (nspace parsing)
                                    Dpushstate;
                                    state=ST_ECALL;
                                    state_index=0;
                                 }
                              }
                              else
                              {
                                 if(ntok==TOKEN_BEGEXPR) // myfun (
                                 {
                                    Dungettok;
                                    state_index = 0; // reset from 9 (nspace parsing)
                                    Dpushstate;
                                    state=ST_UFUNECALL;
                                    state_index=0;
                                 }
                                 else
                                 {
                                   st_factor_string_const:
                                    if(ntok==TOKEN_BEGARRAY) // "["
                                    {
                                       Dsetparams(1, sv);
                                       state_index=7;
                                    }
                                    else
                                    {
                                       // <FACTOR> finished, constant/variable-reference
                                       popnode.node=0;
                                       Dsetparami(3,0);
                                       sBool isconst=sv->getQuotFlag();
                                       sBool allowpost=0;
                                       if(!isconst)
                                       {
                                          TKS_ClassTemplate *clt = NULL;
                                          if( (NULL == cnspace) || (cnspace==&Dtkscompiler->yac_namespace) )
                                          {
                                             clt = tkscript->findTemplate(sv);
                                          }
                                          if(clt)
                                          {
                                             PTN_NEW_NODE(nn, PTN_ConstVal)();
                                             setPNode(nn);
                                             popnode.constval->const_value.initObject(clt->template_object, 0);
                                             cnspace = NULL;
                                          }
                                          else
                                          {
                                             /** check for ClassName */
                                             TKS_ClassDecl *constcd;
                                             if(NULL != cnspace)
                                             {
                                                constcd = Dtkscompiler->findClassDeclNamespace(sv, cnspace);
                                             }
                                             else
                                             {
                                                constcd = Dtkscompiler->findClassDeclNamespaceList(sv, active_namespaces);
                                             }
                                             if(constcd)
                                             {
                                                PTN_NEW_NODE(nn, PTN_ConstVal)();
                                                setPNode(nn);
                                                popnode.constval->const_value.initObject(constcd->class_template, 0);
                                                cnspace = NULL;
                                             }
                                             else
                                             {
                                                co=0;
                                                if( (NULL==cnspace) && cclassdecl)
                                                {
                                                   co=cclassdecl->findConstantSuper(sv);
                                                   if(co) // is it a (CurrentScriptClass.)constant ?
                                                   {
                                                      spawnClassConstantExpr(co);
                                                      popState();
                                                      goto exit_classconst_uconst;
                                                   }

                                                   // check whether its a member of the current class
                                                   co=cclassdecl->getMember(sv);
                                                   DcheckFunMemberAccess(sv);

                                                   if(co)
                                                   {
                                                      DcheckMemberAccess(cclassdecl, sv);
                                                      PTN_NEW_NODE(nn, PTN_ClassMemberExpr)(0, co);
                                                      setPNode(nn);
                                                      popnode.classmemberexpr->class_id=cclassdecl->class_id;
                                                      Dsetparami(3, 1);
                                                      allowpost=1;
                                                   }
                                                }
                                                if(!co)
                                                {
                                                   co = findVar(sv, active_namespaces, cnspace);
                                                   if(!co && (NULL==cnspace) )
                                                   {
                                                      // check class constant
                                                      co=tkscript->global_constant_namespace.findEntry(sv);  // Dfindconst
                                                      if(!co)
                                                      {
                                                         // try constant replacement table
                                                         co=tkscript->uconst_table.findEntry(sv);
                                                         if(co)
                                                         {
                                                            sv=co->value.string_val;
                                                            TKS_CachedObject *nco;
                                                            do
                                                            {
                                                               nco=tkscript->uconst_table.findEntry(sv);
                                                               if(!nco)
                                                               {
                                                                  nco=tkscript->global_constant_namespace.findEntry(sv);
                                                                  if(nco)
                                                                  {
                                                                     // registered class constant (int/float)
                                                                     PTN_NEW_NODE(nn, PTN_ConstVal)();
                                                                     setPNode(nn);
                                                                     popnode.constval->const_value.initInt(nco->value.int_val);
                                                                     popState();
                                                                     goto exit_classconst_uconst;
                                                                  }
                                                               }
                                                               else
                                                               {
                                                                  co=nco;
                                                                  sv=co->value.string_val;
                                                               }
                                                            } while(nco);
                                                            isconst=!sv->getQuotFlag();
                                                         }
                                                         else isconst=1;
                                                      }
                                                      else
                                                      {
                                                         // VAR is a registered Class constant
                                                         PTN_NEW_NODE(nn, PTN_ConstVal)();
                                                         setPNode(nn);
                                                         popnode.constval->const_value.initValue(co);
                                                      }
                                                   }
                                                   else
                                                   {
                                                      if(NULL == co)
                                                      {
                                                         Dprintf("[---] variable \"%s::%s\" does not exist.\n",
                                                                 (char*)cnspace->name.chars,
                                                                 (char*)sv->chars
                                                                 );
                                                         Dparseerror;
                                                      }
                                                      PTN_NEW_NODE(nn, PTN_VarExpr)(co);
                                                      setPNode(nn);
                                                      allowpost=1;
                                                      cnspace = NULL;
                                                   }
                                                }
                                             }
                                          }
                                       }
                                       else
                                       {
                                          // is "string constant"
                                          PTN_NEW_NODE(nn, PTN_ConstVal)();
                                          setPNode(nn);
                                          sv->resubstEscapeSeq();
                                          popnode.constval->const_value.initNewString(sv);
                                          isconst=0;
                                          Dtryindirectobjectexpr;
                                       }

                                       if(isconst) // is NOT string constant
                                       {
                                          // constant value
                                          PTN_NEW_NODE(nn, PTN_ConstVal)();
                                          setPNode(nn);
                                          {
                                             {
                                                sv->resubstEscapeSeq();
                                                sU16 checkconv=sv->checkConversions();
                                                switch(checkconv)
                                                {
                                                   case 1: // int
                                                   {
                                                      sSI intval=0;
                                                      sv->yacScanI(&intval);
                                                      popnode.constval->const_value.initInt(intval);
                                                   }
                                                   break;
                                                   case 2: // float
                                                   {
                                                      sF32 floatval=0;
                                                      sv->yacScanF32(&floatval);
                                                      popnode.constval->const_value.initFloat(floatval);
                                                   }
                                                   break;
                                                   case 3: // newline constant
                                                      popnode.constval->const_value.initInt((int)'\n');
                                                      break;
                                                   case 4: // tab constant
                                                      popnode.constval->const_value.initInt((int)'\t');
                                                      break;
                                                   default: // char constant
                                                      if(checkconv>256)
                                                      {
                                                         ////YAC_String t;
                                                         ////t="Main";
                                                         // Check if it is a FunctionObject reference
                                                         PTN_Function *f;
                                                         if(NULL == cnspace)
                                                         {
                                                            f = findFunction(sv);
                                                         }
                                                         else
                                                         {
                                                            f = NULL;
                                                         }
                                                         if(NULL != f)
                                                         {
                                                            TKS_FunctionObject *fo = f->getOrCreateFunctionObject();
                                                            popnode.constval->const_value.initObject(fo, 0);
                                                         }
                                                         else
                                                         {
                                                            if(NULL == cnspace)
                                                            {
                                                               if(tkscript->configuration.b_enable_unresolved_members)
                                                               {
                                                                  ////setPNode(new PTN_UnresolvedModuleMemberExpr(&t, sv));
                                                                  delete popnode.node; // delete PTN_ConstVal
                                                                  PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberExpr)(&name, 1, sv);
                                                                  setPNode(nn);
                                                                  Dfreezenspaces(popnode.unresolvedmodulememberexpr->nspace_array);
                                                                  Dsetparami(3,2);
                                                                  isconst=0;
                                                                  allowpost=1;
                                                               }
                                                               else
                                                               {
                                                                  Dprintf("[---] cross reference resolving for identifier \"%s\" is turned OFF.\n", (char*)sv->chars);
                                                                  Dparseerror;
                                                               }
                                                            }
                                                            else
                                                            {
                                                               Dprintf("[---] unknown identifier \"%s::%s\".\n",
                                                                       (char*)cnspace->name.chars,
                                                                       (char*)sv->chars
                                                                       );
                                                               Dparseerror;
                                                            }
                                                         }
                                                      }
                                                      else
                                                      {
                                                         popnode.constval->const_value.initInt((int)checkconv);
                                                      }
                                                      break;
                                                }
                                             }
                                          }
                                       }
                                       else
                                       {
                                          if(NULL == popnode.node)
                                          {
                                             // is "string constant"
                                             PTN_NEW_NODE(nn, PTN_ConstVal)();
                                             setPNode(nn);
                                             sv->resubstEscapeSeq();
                                             popnode.constval->const_value.initNewString(sv);
                                             isconst=1;
                                          }
                                       }

                                       if(/*!isconst&&*/allowpost)
                                       {
                                          if(TOKEN_INC == ntok) // "++"
                                          {
                                             state_index=5;
                                             Dsetparamn(0, popnode.node);
                                          }
                                          else
                                          {
                                             if(ntok==TOKEN_DEC) // "--"
                                             {
                                                state_index=6;
                                                Dsetparamn(0, popnode.node);
                                             }
                                          }
                                       }
                                       else
                                       {
                                          if(ntok==TOKEN_INC)
                                          {
                                             // fix "bla"+++bla expr from
                                             // "bla"++ +bla to
                                             // "bla"+ ++bla
                                             if(nntok==TOKEN_ADD)
                                             {
                                                sU16 t=stok[1];
                                                stok[1]=stok[0];
                                                stok[0]=t;
                                             }
                                          }
                                          else
                                          {
                                             if(ntok==TOKEN_DEC)
                                             {
                                                // fix "bla"---bla expr from
                                                // "bla"-- -bla to
                                                // "bla"- --bla
                                                if(nntok==TOKEN_SUB)
                                                {
                                                   sU16 t=stok[1];
                                                   stok[1]=stok[0];
                                                   stok[0]=t;
                                                }
                                             }
                                          }
                                       }
                                       if( (9 == state_index) || (0 == state_index) )
                                       {
                                          popState();
                                       }
                                    } // if ntok==BEGARRAY..
                                 } // if ntok==BEGEXPR..
                              }
                           }
                        exit_classconst_uconst:
                           break;
                        default:
                           Dparseerror;
                     }
                     break;

                  case 10: // skip '::' between namespace and class/varname
                     state_index = 9;
                     break;

                  case 1: //
                     // <FACTOR> finished, <ECALL>
                     Dungettok;
                     popState();
                     break;
                  case 2:
                     if(tok!=TOKEN_ENDEXPR) // ")"
                     {
                        Dparseerror;
                     }
                     // sequence <FACTOR> finished "(" <EXPR> ")"
                     popState();
                     break;
                  case 3: // ?["!"|"-"] <FACTOR>
                     // !|- <FACTOR> finished
                     PTN_NEW_NODE(nn, PTN_SingleArgExpr)(popnode.expr, Dgetparami(0));
                     setPNode(nn);
                     Dungettok;
                     popState();
                     break;
                  case 4: // PREINC/DEC VarExpr finished
                     Dungettok;
                     popState();
                     break;
                  case 5:
                     if(tok!=TOKEN_INC) // var "++"
                     {
                        Dparseerror;
                     }
                     tnode=Dgetparamn(0);
                     if(Dgetparami(3))
                     {
                        if(Dgetparami(3)==1)
                        {
                           // was classmember expr
                           PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(0, tnode.classmemberexpr->member, 1, 0);
                           setPNode(nn);
                           popnode.classmemberincdecexpr->class_id=tnode.classmemberexpr->class_id;
                           delete tnode.node;
                           parser_tree_num_nodes--;
                           popState();
                        }
                        else
                        {
                           if(tkscript->configuration.b_enable_unresolved_members)
                           {
                              PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberIncDecExpr)
                                 (
                                    &tnode.unresolvedmodulememberexpr->module_name,
                                    tnode.unresolvedmodulememberexpr->b_default_module,
                                    &tnode.unresolvedmodulememberexpr->module_member_name,
                                    1, 0
                                  );
                              setPNode(nn);
                              Dfreezenspaces(popnode.unresolvedmodulememberincdecexpr->nspace_array);
                              delete tnode.node;
                              parser_tree_num_nodes--;
                              popState();
                           }
                           else
                           {
                              delete tnode.node;
                              parser_tree_num_nodes--;
                              popState();
                              Dprintf("[---] cross referencing for identifier \"%s\" is turned OFF.\n",
                                      (char*)tnode.unresolvedmodulememberexpr->module_member_name.chars);
                              Dparseerror;
                           }
                        }
                     }
                     else
                     {
                        PTN_NEW_NODE(nn, PTN_PostIncVarExpr)();
                        popnode.postincvarexpr = (PTN_PostIncVarExpr*) nn;
                        popnode.postincvarexpr->cached_object=tnode.varexpr->cached_object;
                        delete tnode.node;
                        Dsetsrcloc;
                        popState();
                     }
                     break;
                  case 6:
                     if(tok!=TOKEN_DEC) // var "--"
                     {
                        Dparseerror;
                     }
                     tnode=Dgetparamn(0);
                     if(Dgetparami(3))
                     {
                        if(Dgetparami(3)==1)
                        {
                           // was classmember expr
                           PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(0, tnode.classmemberexpr->member, 1, 1);
                           popnode.classmemberincdecexpr = (PTN_ClassMemberIncDecExpr*)nn;
                           popnode.classmemberincdecexpr->class_id=tnode.classmemberexpr->class_id;
                           Dsetsrcloc;
                           delete tnode.node;
                        }
                        else
                        {
                           if(tkscript->configuration.b_enable_unresolved_members)
                           {
                              PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberIncDecExpr)
                                 (
                                    &tnode.unresolvedmodulememberexpr->module_name,
                                    tnode.unresolvedmodulememberexpr->b_default_module,
                                    &tnode.unresolvedmodulememberexpr->module_member_name,
                                    1, 1
                                  );
                              popnode.unresolvedmodulememberincdecexpr = (PTN_UnresolvedModuleMemberIncDecExpr*) nn;
                              Dfreezenspaces(popnode.unresolvedmodulememberincdecexpr->nspace_array);
                              Dsetsrcloc;
                              delete tnode.node;
                           }
                           else
                           {
                              delete tnode.node;
                              Dprintf("[---] cross referencing for identifier \"%s\" is turned OFF.\n",
                                      (char*)tnode.unresolvedmodulememberexpr->module_member_name.chars);
                              Dparseerror;
                           }
                        }
                     }
                     else
                     {
                        PTN_NEW_NODE(nn, PTN_PostDecVarExpr)();
                        setPNode(nn);
                        popnode.postdecvarexpr->cached_object=tnode.varexpr->cached_object;
                        delete tnode.node;
                     }
                     popState();
                     break;
                  case 7: // var [ <INTEXPR> ]
                     if(tok!=TOKEN_BEGARRAY)
                     {
                        Dparseerror;
                     }
                     Dsetparamnspace(2, cnspace);
                     cnspace = NULL;
                     if(ntok==TOKEN_ENDARRAY)
                     {
                        PTN_NEW_NODE(nn, PTN_ConstVal)(0);
                        setPNode(nn);
                        state_index++;
                     }
                     else
                     {
                        Dpushstate;
                        state=ST_EXPR;
                        state_index=0;
                     }
                     break;
                  case 8: // <INTEXPR> finished
                     if(tok!=TOKEN_ENDARRAY)
                     {
                        Dparseerror;
                     }
                     {
                        cnspace = Dgetparamnspace(2);
                        PTN_Expr *expr=popnode.expr;
                        //if(expr->isConst())
                        {
                           sv = Dgetparams(1); // get variable name
                           co=0;
                           if( (NULL==cnspace) && cclassdecl)
                           {
                              co=cclassdecl->getMember(sv);
                              DcheckFunMemberAccess(sv);
                              if(co)
                              {
                                 DcheckMemberAccess(cclassdecl, sv);
                                 PTN_NEW_NODE(nn, PTN_ClassMemberIndexedVarExpr)(0, co, expr);
                                 setPNode(nn);
                                 popnode.classmemberindexedvarexpr->class_id=cclassdecl->class_id;
                                 Dtryindirectobjectexpr;
                              }
                           }
                           if(!co)
                           {
                              co = findVar(sv, active_namespaces, cnspace);
                              cnspace = NULL;
                              if(co)
                              {
                                 if(co->flags&TKS_CachedObject::ISDYNAMIC)
                                 {
                                    PTN_NEW_NODE(nn, PTN_VarExpr)(co);
                                    setTNode(nn);
                                    PTN_NEW_NODE(nn, PTN_IArrayExpr)(tnode.expr, popnode.expr);
                                    setPNode(nn);
                                    popState();
                                    break;
                                 }

                                 if(co->type < YAC_TYPE_OBJECT)
                                 {
                                    Dprintf("\n[---] trying to index >>%s<< variable \"%s\".\n",
                                            tks_get_rtti_name_by_id((sU8)co->type), (char*)sv->chars);
                                    Dparseerror;
                                 }
                                 else
                                 {
                                    PTN_NEW_NODE(nn, PTN_IndexedVarExpr)(co, expr);
                                    setPNode(nn);
                                    Dtryindirectobjectexpr;
                                 }
                              }
                              else
                              {
                                 Dprintf("\n[---] unknown reference to HashTable/Array \"%s\".\n", (char*)sv->chars);
                                 Dparseerror;
                              }
                           }
                        }
                     }
                     break;
                  default:
                     Dparseerror;
                     break;
               }
               break;

            case ST_NEWEXPR:
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_NEW)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     Dparsenspace(4);

                     if(tok!=TOKEN_VAR)
                     {
                        if( (NULL==cnspace) && (tok==TOKEN_STRING) ) // new String
                        {
                           PTN_NEW_NODE(nn, PTN_NewExpr)();
                           setPNode(nn);
                           popnode.newexpr->template_object=TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_STRING]->template_object;
                        }
                        else // new Object() is not allowed
                        {
                           Dparseerror;
                        }
                     }
                     else
                     {
                        // Try script class
                        TKS_ClassDecl *cd;
                        if(NULL == cnspace)
                        {
                           cd = Dtkscompiler->findClassDeclNamespaceList(sv, active_namespaces);
                        }
                        else
                        {
                           cd = Dtkscompiler->findClassDeclNamespace(sv, cnspace);
                        }
                        if(cd)
                        {
                           PTN_NEW_NODE(nn, PTN_NewClassExpr)(cd);
                           setPNode(nn); // new MyClass, new mynamespace::MyClass
                        }
                        else
                        {
                           // Try C++ class

                           TKS_ClassTemplate *clt;

                           if( (NULL==cnspace) || (cnspace==&Dtkscompiler->yac_namespace) )
                           {
                              clt = tkscript->findTemplate(sv);
                           }
                           else
                           {
                              clt = NULL;
                           }
                           if(clt)
                           {
                              // new CPPClass, new yac::CPPClass
                              PTN_NEW_NODE(nn, PTN_NewExpr)();
                              setPNode(nn);
                              popnode.newexpr->template_object=clt->template_object;
                           }
                           else
                           {
                              Dprintf("\n[---] new: unknown class \"%s\"\n", (char*)(sv->chars));
                              Dparseerror;
                           }
                        }

                        cnspace = NULL;
                     }
                     if(ntok==TOKEN_BEGEXPR)
                     {
                        state_index++;
                     }
                     else
                     {
                        popState();
                     }
                     break;
                  case 2:
                     // consume ()
                     if(tok!=TOKEN_BEGEXPR)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 3:
                     // consume ()
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     popState();
                     break;
                  case 4: // consume '::' between namespace and classname
                     state_index = 1;
                     break;
               }
               break;

            case ST_PREINCVAREXPR:
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_INC)
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     Dparsenspace(2);

                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     if(!sv->getQuotFlag())
                     {
                        co=0;
                        if( (NULL == cnspace) && cclassdecl)
                        {
                           co=cclassdecl->getMember(sv);
                           DcheckFunMemberAccess(sv);
                           if(co)
                           {
                              DcheckMemberAccess(cclassdecl, sv);
                              PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(0, co, 0, 0);
                              setPNode(nn);
                              popnode.classmemberincdecexpr->class_id=cclassdecl->class_id;
                           }
                        }
                        if(!co)
                        {
                           co = findVar(sv, active_namespaces, cnspace);
                           if(!co)
                           {
                              Dprintf("[---] unknown reference to variable or class property >>%s<<\n", (char*)sv->chars);
                              Dparseerror;
                           }
                           else
                           {
                              PTN_NEW_NODE(nn, PTN_PreIncVarExpr)();
                              setPNode(nn);
                              popnode.preincvarexpr->cached_object=co;
                           }
                        }
                     }
                     else
                     {
                        Dprintf("\n[---] expected variable name, got string-constant >>%s<<\n", (char*)sv->chars);
                        Dparseerror;
                     }
                     popState();
                     cnspace = NULL;
                     break;
                  case 2: // skip '::' between namespace and varname
                     state_index = 1;
                     break;
               }
               break;

            case ST_PREDECVAREXPR:
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_DEC) // "--"
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 1:
                     Dparsenspace(2);

                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     if(!sv->getQuotFlag())
                     {
                        co=0;
                        if( (NULL == cnspace) && cclassdecl)
                        {
                           co=cclassdecl->getMember(sv);
                           DcheckFunMemberAccess(sv);
                           if(co)
                           {
                              DcheckMemberAccess(cclassdecl, sv);
                              PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(0, co, 0, 1);
                              setPNode(nn);
                              popnode.classmemberincdecexpr->class_id=cclassdecl->class_id;
                           }
                        }
                        if(!co)
                        {
                           co = findVar(sv, active_namespaces, cnspace);
                           if(!co)
                           {
                              Dprintf("[---] unknown reference to variable or class property >>%s<<\n", (char*)sv->chars);
                              Dparseerror;
                           }
                           else
                           {
                              PTN_NEW_NODE(nn, PTN_PreDecVarExpr)();
                              setPNode(nn);
                              popnode.predecvarexpr->cached_object=co;
                           }
                        }
                     }
                     else
                     {
                        Dprintf("\n[---] expected variable name, not string-constant >>%s<<\n", (char*)sv->chars);
                        Dparseerror;
                     }
                     popState();
                     cnspace = NULL;
                     break;
                  case 2: // skip '::' between namespace and varname
                     state_index = 1;
                     break;
               }
               break;

            case ST_UFUNCALL:
               switch(state_index)
               {
                  case 0:
                     Dungettok;
                     Dpushstate;
                     state=ST_UFUNECALL;
                     state_index=0;
                     break;
                  case 1: // <ST_UFUNECALL> finished
                     Dungettok;
                     if(popnode.node->getID()==PTN_FUNCTIONECALL)
                     {
                        PTN_NEW_NODE(nn, PTN_FunctionCall)(popnode.functionecall);
                        setPNode(nn);
                     }
                     else
                     {
                        PTN_NEW_NODE(nn, PTN_Call)(popnode.callableexpr);
                        setPNode(nn);
                     }
                     popState();
                     break;
                  default:
                     Dparseerror;
               }
               break;

            case ST_UFUNECALL:
               // note: cnspace must already be set
               // cnspace is currently being ignored almost entirely since functions are placed in the module namespace (may change)
               // the only valid cnspace is "yac"
               //
               switch(state_index)
               {
                  case 0: // <VAR> ( <FUNARGS> )
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     Dsetparams(0,sv); // remember function-name
                     state_index++;
                     break;
                  case 1:
                     if(tok!=TOKEN_BEGEXPR)
                     {
                        Dparseerror;
                     }
                     Dsetparamnspace(1, cnspace);
                     cnspace = NULL;
                     Dpushstate;
                     state=ST_ARGLIST;
                     state_index=0;
                     break;
                  case 2: // <ST_ARGLIST> finished
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     {
                        sBool btrynext=1;
                        YAC_String *fn = Dgetparams(0);
                        cnspace = Dgetparamnspace(1);
                        // yac_host->printf("xxx try fn=\"%s\" cnspace=%p\n", fn->chars, cnspace);
                        if( (NULL == cnspace) && cclassdecl)
                        {
                           // ---- try static class method call ----
                           {
                              PTN_Function *fun = cclassdecl->findFunctionSuper(fn);
                              // yac_host->printf("xxx try static method fn=\"%s\" => fun=%p\n", fn->chars, fun);
                              if(NULL != fun)
                              {
                                 PTN_NEW_NODE(nn, PTN_FunctionECall)(fun, popnode.arglist);
                                 setPNode(nn);
                                 btrynext=0;
                                 Dtryindirectobjectexprnopopstate;
                              }
                           }
                           if(btrynext)
                           {
                              // ---- try class method call ----
                              TKS_ClassDeclMethod *meth=cclassdecl->getMethod(fn);
                              if(cfunction)
                                 if(cfunction->is_class_function)
                                    meth=0; // cannot call method without an object
                              if(meth)
                              {
                                 DcheckMethodAccess(cclassdecl, fn);
                                 PTN_NEW_NODE(nn, PTN_ClassMethodECall)(0, meth, popnode.arglist);
                                 setPNode(nn);
                                 popnode.classmethodecall->class_id=cclassdecl->class_id;
                                 btrynext=0;
                                 Dtryindirectobjectexprnopopstate;
                              }
                           }
                        }
                        if(btrynext)
                        {
                           // ---- try YAC c++ call
                           if( cclassdecl && ((NULL == cnspace) || (cnspace == &Dtkscompiler->yac_namespace)) )
                           {
                              TKS_ClassTemplate *clt = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_CLASS];
                              const YAC_CommandY *ccmdy = clt->findCommandY(fn);
                              if(NULL != ccmdy)
                              {
                                 // Is YAC C++ call
                                 PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, NULL/*co*/, YAC_FALSE/*bIndirect*/, popnode.arglist, YAC_CLID_CLASS);
                                 setPNode(nn);
                                 btrynext = 0;
                                 Dtryindirectobjectexprnopopstate;
                              }
                           }
                        }
                        if(btrynext)
                        {
                           /// ---- try global script function ----
                           PTN_Function *fun;
                           if(NULL != cnspace)
                           {
                              fun = NULL; // ignore cnspace for now
                           }
                           else
                           {
                              // // co = functions.findEntry(fn);
                              // // if(NULL != co)
                              // // {
                              // //    fun = (PTN_Function*)co->value.object_val;
                              // // }
                              fun = findFunction(fn);  // [05Sep2025] search module + active namespaces
                           }
                           if(fun)
                           {
                              if(fun->parent_class) // check if this is a delegate function
                              {
                                 Dprintf("[---] please use \"delegate <name>()\" to call delegate methods.\n");
                                 Dparseerror;
                              }
                              PTN_NEW_NODE(nn, PTN_FunctionECall)(fun, popnode.arglist);
                              setPNode(nn);
                              btrynext=0;
                              Dtryindirectobjectexprnopopstate;
                           }
                        }
                        if(btrynext)
                        {
                           // ---- try "c" plugin function ----
                           if( (NULL==cnspace) || (cnspace==&Dtkscompiler->yac_namespace) )
                           {
                              co = tkscript->yac_functions.findEntry(fn);
                           }
                           else
                           {
                              co = NULL;
                           }
                           if( co )
                           {
                              // ---- try new-style "c" plugin function ----
                              YAC_CommandY *f=(YAC_CommandY*)co->value.object_val;
                              PTN_NEW_NODE(nn, PTN_FunctionECallY)(f, popnode.arglist);
                              setPNode(nn);
                              cnspace = NULL;
                              if(TOKEN_RETOBJASSIGN == ntok) // => follows?
                              {
                                 Dsetparamn(0, popnode.node);
                                 Dsetparami(1, 0);
                                 state_index = 100;
                                 break;
                              }
                              else if(TOKEN_RETOBJRET == ntok) // <=> follows?
                              {
                                 Dsetparamn(0, popnode.node);
                                 Dsetparami(1, 1);
                                 state_index = 100;
                                 break;
                              }
                              else
                              {
                                 Dtryindirectobjectexprnopopstate;
                              }
                           }
                           else
                           {
                              if(NULL == cnspace)
                              {
                                 if(tkscript->configuration.b_enable_unresolved_functions)
                                 {
                                    // ---- return unresolved function call ----
                                    ////YAC_String t;
                                    ////t="Main";
                                    ////setPNode(new PTN_UnresolvedModuleMethodECall(&t, fn, popnode.arglist));
                                    // yac_host->printf("xxx add unresolved method call \"%s\"\n", name.chars);
                                    PTN_NEW_NODE(nn, PTN_UnresolvedModuleMethodECall)(&name, fn, popnode.arglist);
                                    setPNode(nn);

                                    Dtryindirectobjectexprnopopstate;
                                    if(tkscript->configuration.debug_level >= 10)
                                       tkscript->printf("[...] implicit declaration of function \"%s\".\n", (char*)(fn?(char*)fn->chars:"empty string"));
                                 }
                                 else
                                 {
                                    Dprintf("[---] cross referencing for function \"%s\" is turned OFF.\n",
                                            (char*)((char*)(fn?(char*)fn->chars:"empty string")));
                                    Dparseerror;
                                 }
                              }
                              else
                              {
                                 Dprintf("[---] failed to resolve function \"%s::%s\".\n",
                                         (char*)cnspace->name.chars,
                                         (char*)fn->chars
                                         );
                                 Dparseerror;
                              }

                           }
                        }
                     }
                     popState();
                     break;

                  case 100: // consume '=>' resp. '<=>'
                     Dpushstate;
                     state = ST_EXPR;
                     state_index = 0;
                     break;
                  case 101: // =>,<=> "retobjexpr" finished
                  {
                     PTN_FunctionECallY *functionecally = Dgetparamn(0).functionecally;
                     functionecally->retobj_expr = popnode.expr;
                     functionecally->b_ret_retobj = Dgetparami(1);
                     popnode.functionecally = functionecally;
                     sUI t = tok;
                     Dungettok;
                     ntok = t; // For macro call
                     Dtryindirectobjectexprnopopstate
                        popState();
                  }
                  break;

               }
               break;

            case ST_CALL:
               switch(state_index)
               {
                  case 0:
                     Dungettok;
                     Dpushstate;
                     state=ST_ECALL;
                     state_index=0;
                     break;
                  case 1:
                     Dungettok;
                     // sequence <CALL> finished (<ECALL>)
                     PTN_NEW_NODE(nn, PTN_Call)(popnode.callableexpr);
                     setPNode(nn);
                     /// ??? check zero return and try alternate path
                     popState();
                     break;
                  default:
                     Dparseerror;
               }
               break;

            case ST_BUILTINFUN:
               switch(state_index)
               {
                  case 0:
                  {
                     sU8 funsel;
                     switch(tok)
                     {
                        default: Dparseerror;
                        case TOKEN_ADDRESS: funsel = TKS_BFUN_ADDRESS;  break;
                        case TOKEN_OBJECT:  funsel = TKS_BFUN_TCOBJECT; break;
                        case TOKEN_STRING:  funsel = TKS_BFUN_TCSTR;    break;
                        case TOKEN_FLOAT:   funsel = TKS_BFUN_TCFLOAT;  break;
                        case TOKEN_INT:     funsel = TKS_BFUN_TCINT;    break;
                        case TOKEN_CHARS:   funsel = TKS_BFUN_TCCHR;    break;
                     }
                     Dsetparami(0, funsel);
                     state_index++;
                  }
                  break;

                  case 1:
                     if(tok!=TOKEN_BEGEXPR)
                     {
                        Dparseerror;
                     }
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;
                  case 2:
                     // <EXPR> finished
                     if(tok!=TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     PTN_NEW_NODE(nn, PTN_BuiltinFun)(popnode.expr, Dgetparami(0));
                     setPNode(nn);
                     popState();
                     break;

                  case 10:
                     // Parse (int) (float) (Object) (String) typecast expr.
                     switch(tok)
                     {
                        default:
                           Dparseerror;
                           break;
                        case TOKEN_INT:     Dsetparami(0, TKS_BFUN_TCINT);    break;
                        case TOKEN_FLOAT:   Dsetparami(0, TKS_BFUN_TCFLOAT);  break;
                        case TOKEN_OBJECT:  Dsetparami(0, TKS_BFUN_TCOBJECT); break;
                        case TOKEN_STRING:  Dsetparami(0, TKS_BFUN_TCSTR);    break;
                     }
                     state_index++;
                     break;
                  case 11:
                     if(tok != TOKEN_ENDEXPR)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        Dpushstate;
                        Dsetparami(2,0); // unary op bind
                        state=ST_FACTOR;
                        state_index=0;
                     }
                     break;
                  case 12:
                     // <EXPR> finished
                     Dungettok;
                     PTN_NEW_NODE(nn, PTN_BuiltinFun)(popnode.expr, Dgetparami(0));
                     setPNode(nn);
                     popState();
                     break;
               }
               break;

            case ST_PREINCDECREADPROPERTY:
               switch(state_index)
               {
                  case 0:
                     if(tok==TOKEN_INC)
                     {
                        Dsetparami(2, 2);
                        state=ST_READPROPERTY;
                     }
                     else
                     {
                        if(tok==TOKEN_DEC)
                        {
                           Dsetparami(2, 4);
                           state=ST_READPROPERTY;
                        }
                        else
                        {
                           Dparseerror;
                        }
                     }
                     break;
                  default:
                     Dparseerror;
                     break;
               }
               break;

            case ST_READPROPERTY:
               /// initial parami(2)==1 forces statement nodes  (??? &1 ??!)
               ///         parami(2)&2 => preinc expr
               ///         parami(2)&4 => predec expr
               ///
               /// note: cnspace must already be set
               ///
               switch(state_index)
               {
                  case 0:
                     if(tok!=TOKEN_VAR)
                     {
                        /*if(tok==TOKEN_THIS)
                          {
                          sv = &tks_this_name;
                          }
                          else*/
                        {
                           Dparseerror;
                        }
                     }
                     if(!b_force_this)
                     {

                        TKS_ClassDecl *cd;
                        if(NULL != cnspace)
                        {
                           cd = Dtkscompiler->findClassDeclNamespace(sv, cnspace);
                        }
                        else
                        {
                           cd = Dtkscompiler->findClassDeclNamespaceList(sv, active_namespaces);
                        }
                        if(cd)
                        {
                           // ---- parse static class member expr MyClass.i, MyClass::i
                           Dsetparamv(0, (void *)cd);
                           state_index=100;
                           cnspace = NULL;
                        }
                        else
                        {
                           Dsetparams(0, sv); // remember object name
                           if(NULL == cnspace)
                           {
                              co = findVarLocal(sv); // find local variable / static class member
                              if(!co)
                              {
                                 // Find String.length like static C++ class member
                                 co = tkscript->global_template_namespace.findEntry(sv);
                                 if(co)
                                 {
                                    TKS_ClassTemplate *t=(TKS_ClassTemplate*)co->value.object_val;
                                    co=&t->static_co;
                                 }
                              }
                           }
                           else
                           {
                              co = NULL;
                           }
                           state_index++;
                        }
                     }
                     else
                     {
                        Dassertnonspace;

                        co = NULL;
                        Dsetparams(0, sv);
                        state_index++;
                     }
                     break;
                  case 100:
                     // ---- parse MyClass.static_i, MyClass::static_i expression
                     if( (tok!=TOKEN_DOT)&&(tok!=TOKEN_MEMBER) )
                     {
                        Dparseerror;
                     }
                     state_index++;
                     break;
                  case 101:
                     if(tok!=TOKEN_VAR)
                     {
                        Dparseerror;
                     }
                     else
                     {
                        TKS_ClassDecl *cd = (TKS_ClassDecl*)Dgetparamv(0);
                        co = cd->getStaticMember(sv);
                        if(!co)
                        {
                           co = cd->findConstantSuper(sv);
                           if(co) // is it a (CurrentScriptClass.)constant ?
                           {
                              spawnClassConstantExpr(co);
                              popState();
                           }
                           else
                           {
                              PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberExpr)(&cd->name, 0, sv);
                              setPNode(nn);
                              Dfreezenspaces(popnode.unresolvedmodulememberexpr->nspace_array);
                              popState();
                           }
                        }
                        else
                        {
                           DcheckMemberAccess(cd, sv);

                           sBool stat=Dgetparami(2)&1;
                           sBool preinc=(Dgetparami(2)&2)==2;
                           sBool predec=(Dgetparami(2)&4)==4;
                           if(preinc)
                           {
                              if(stat)
                              {
                                 PTN_NEW_NODE(nn, PTN_PreIncVarStat)();
                                 setPNode(nn);
                                 popnode.preincvarstat->cached_object=co;
                              }
                              else
                              {
                                 PTN_NEW_NODE(nn, PTN_PreIncVarExpr)();
                                 setPNode(nn);
                                 popnode.preincvarexpr->cached_object=co;
                              }
                           }
                           else if(predec)
                           {
                              if(stat)
                              {
                                 PTN_NEW_NODE(nn, PTN_PreDecVarStat)();
                                 setPNode(nn);
                                 popnode.predecvarstat->cached_object=co;
                              }
                              else
                              {
                                 PTN_NEW_NODE(nn, PTN_PreDecVarExpr)();
                                 setPNode(nn);
                                 popnode.predecvarexpr->cached_object=co;
                              }
                           }
                           else
                           {
                              if(ntok==TOKEN_INC)
                              {
                                 if(stat)
                                 {
                                    PTN_NEW_NODE(nn, PTN_PostIncVarStat)();
                                    setPNode(nn);
                                    popnode.postincvarstat->cached_object=co;
                                 }
                                 else
                                 {
                                    PTN_NEW_NODE(nn, PTN_PostIncVarExpr)();
                                    setPNode(nn);
                                    popnode.postincvarexpr->cached_object=co;
                                 }
                              }
                              else if(ntok==TOKEN_DEC)
                              {
                                 if(stat)
                                 {
                                    PTN_NEW_NODE(nn, PTN_PostDecVarStat)();
                                    setPNode(nn);
                                    popnode.postdecvarstat->cached_object=co;
                                 }
                                 else
                                 {
                                    PTN_NEW_NODE(nn, PTN_PostDecVarExpr)();
                                    setPNode(nn);
                                    popnode.postdecvarexpr->cached_object=co;
                                 }
                              }
                              else
                              {
                                 if(stat)
                                 {
                                    if(ntok==TOKEN_CMPLE) // '<='
                                    {
                                       // CStatic.o <= <objexpr>   , parse objexpr
                                       Dsetparamn(0, (PTN_Node*)co); // remember static member CachedObject
                                       state_index=103; // consume '<='
                                       break;
                                    }
                                    else
                                    {
                                       Dparseerror;//setPNode(new PTN_VarStat(co));
                                    }
                                 }
                                 else
                                 {
                                    PTN_NEW_NODE(nn, PTN_VarExpr)(co);
                                    setPNode(nn);
                                 }
                              }
                           }
                           if((ntok==TOKEN_INC)||(ntok==TOKEN_DEC))
                              state_index++;
                           else
                              popState();
                        }
                     }
                     break;
                  case 102:
                     popState(); // MyClass.static_i++, MyClass.static_i--
                     break;

                  case 103: // consume '<=' in CStatic.o<=null
                     Dpushstate;
                     state=ST_EXPR;
                     state_index=0;
                     break;

                  case 104: // CStatic.o <= <objexpr> finished
                     Dungettok;
                     PTN_NEW_NODE(nn, PTN_PointerAssign)((TKS_CachedObject*)Dgetparamn(0).node, popnode.expr);
                     setPNode(nn);
                     popState();
                     break;

                  case 1: // "."
                     if(tok==TOKEN_DOT) // '.'
                     {
                        if(co)
                        {
                           // local variable or static script/c++ class member found
                           if(co->flags&TKS_CachedObject::ISDYNAMIC)
                           {
                              PTN_NEW_NODE(nn, PTN_VarExpr)(co);
                              setPNode(nn);
                              Dsetparamn(0, popnode.expr);
                              state=ST_IEXPR;
                              state_index=0;
                              break;
                           }
                        }
                        state_index++;
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 2: // method/member-name
                     if(tok==TOKEN_VAR)
                     {
                        Dsetparams(1,sv); // remember method/member name
                        state_index++;
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 3:
                  {
                     // ??? read scriptinstance.member
                     // ??? read object.member shortpath to object.getMember()
                     YAC_String *cln=Dgetparams(0);
                     YAC_String *mn=Dgetparams(1);
                     int par2=Dgetparami(2);

                     if(co)
                     {
                        if(co->type<YAC_TYPE_OBJECT)
                        {
                           Dprintf("\n[---] \"%s\" is not an Object.\n", (char*)(cln->chars));
                           Dparseerror;
                        }
                        if(!co->value.object_val)
                        {
                           Dprintf("\n[---] cannot address empty (Pointer) object \"%s\".\n",
                                   (char*)cln->chars);
                           Dparseerror;
                        }

                        if(co->value.object_val->class_ID==YAC_CLID_CLASS)
                        {
                           TKS_ScriptClassInstance *cc=(TKS_ScriptClassInstance*)co->value.object_val;

                           TKS_CachedObject *cc_co=cc->getMember(mn);
                           //DcheckMemberAccess(cc->class_decl, mn)
                           if(!cc_co)
                           {
                              Dprintf("\n[---] %sclass \"%s::%s\" has no member called \"%s\".\n",
                                      (!cc->class_decl->is_declared ? "undeclared " : ""),
                                      (char*)cc->class_decl->nspace->name.chars,
                                      (char*)cc->class_decl->name.chars,
                                      (char*)mn->chars
                                      );
                              Dparseerror;
                           }
                           else
                           {
                              DcheckMemberAccess(cc->class_decl, mn);
                              sBool preinc=(Dgetparami(2)&2)==2;
                              sBool predec=(Dgetparami(2)&4)==4;

                              if((Dgetparami(2)==1)&&(tok==TOKEN_CMPLE)) // check for classmember pointerassign
                              {
                                 // class.member<=objexpr
                                 if(preinc||predec)
                                 {
                                    Dprintf("\n[---] cannot preinc/predec and pointer assign at the same time.\n");
                                    Dparseerror;
                                 }
                                 Dsetparamv(0, (void *)co);
                                 Dsetparamv(1, (void *)cc_co);
                                 state_index=5;
                                 Dpushstate;
                                 state=ST_EXPR;
                                 state_index=0;
                                 break;
                              }
                              else
                              {
                                 if(Dgetparami(2)==1)
                                 {
                                    if(preinc||predec)
                                    {
                                       Dprintf("\n[---] cannot preinc/predec and postinc at the same time.\n");
                                       Dparseerror;
                                    }
                                    if(tok==TOKEN_INC)
                                    {
                                       // class.member++ statement
                                       PTN_NEW_NODE(nn, PTN_ClassMemberIncDecStat)(co, cc_co, 0);
                                       setPNode(nn);
                                    }
                                    else
                                    {
                                       if(tok==TOKEN_DEC)
                                       {
                                          // class.member-- statement
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIncDecStat)(co, cc_co, 1);
                                          setPNode(nn);
                                       }
                                       else
                                       {
                                          Dprintf("\n[---] class.member statement not possible.\n");
                                          Dparseerror;
                                       }
                                    }
                                 }
                                 else
                                 {
                                    if(preinc)
                                    {
                                       PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(co, cc_co, 0, 0);
                                       setPNode(nn);
                                       Dungettok;
                                    }
                                    else
                                    {
                                       if(predec)
                                       {
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(co, cc_co, 0, 1);
                                          setPNode(nn);
                                          Dungettok;
                                       }
                                       else
                                       {
                                          if(tok==TOKEN_INC)
                                          {
                                             // class.member++ expr
                                             PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(co, cc_co, 1, 0);
                                             setPNode(nn);
                                             //popState();
                                          }
                                          else
                                          {
                                             if(tok==TOKEN_DEC)
                                             {
                                                // class.member-- expr
                                                PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(co, cc_co, 1, 1);
                                                setPNode(nn);
                                                //popState();
                                             }
                                             else
                                             {
                                                // class.member expr
                                                PTN_NEW_NODE(nn, PTN_ClassMemberExpr)(co, cc_co);
                                                setPNode(nn);
                                                if(tok==TOKEN_DOT)
                                                {
                                                   Dsetparamn(0, popnode.expr);
                                                   state=ST_IEXPR;
                                                   state_index=0;
                                                   break;
                                                }
                                                else
                                                {
                                                   if(tok==TOKEN_BEGARRAY)
                                                   {
                                                      Dsetparamn(0, popnode.expr);
                                                      state=ST_IARRAYEXPR;
                                                      state_index=0;
                                                      break;
                                                   }
                                                   else
                                                   {
                                                      Dungettok;
                                                   }
                                                }
                                             }
                                          }
                                       }
                                    }
                                 }
                              } // CMPLE
                           }
                        }
                        else // !SCRIPTCLASS
                        {
                           YAC_String getmethodname;
                           getmethodname.copy("get"); // build get-method name
                           getmethodname.append(mn);
                           getmethodname.chars[3]&=~32; // make first char of member name uppercase
                           YAC_String classname; classname.visit((char*)co->value.object_val->yacClassName());
                           TKS_ClassTemplate *clt=tkscript->findTemplate(&classname);
                           if(!clt)
                           {
                              Dprintf("\n[---] unknown class template \"%s\"\n", (char*)classname.chars);
                              Dparseerror;
                           }
                           const YAC_CommandY*ccmdy = clt->findCommandY(&getmethodname);
                           if(!ccmdy)
                           {
                              Dprintf("\n[---] c++-class \"%s\" instance \"%s\" has no (readable) property called \"%s\"\n",
                                      (char*)(clt->class_name.chars), (char*)(cln->chars), (char*)(mn->chars));
                              toki--;
                              Dparseerror;
                           }
                           if( (tok!=TOKEN_INC) && (tok!=TOKEN_DEC) )
                           {
                              // ---- object.member ----
                              // call C++ get*() method
                              PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, co, YAC_FALSE/*bIndirect*/, /**arglist**/0, clt->template_object->class_ID);
                              setPNode(nn);
                              if(tok==TOKEN_DOT)
                              {
                                 Dsetparamn(0, popnode.expr);
                                 state=ST_IEXPR;
                                 state_index=0;
                                 break;
                              }
                              else
                              {
                                 if(tok==TOKEN_BEGARRAY)
                                 {
                                    Dsetparamn(0, popnode.expr);
                                    state=ST_IARRAYEXPR;
                                    state_index=0;
                                    break;
                                 }
                                 else
                                 {
                                    Dungettok;
                                 }
                              }

                           }
                           else
                           {
                              // build write member method name
                              YAC_String setmethodname;
                              setmethodname.copy("set");
                              setmethodname.append(mn);
                              setmethodname.chars[3]&=~32;
                              const YAC_CommandY *setcmdy = clt->findCommandY(&setmethodname);
                              if(!setcmdy)
                              {
                                 Dprintf("\n[---] c++-class \"%s\" has no (writable) property called \"%s\"\n",
                                         (char*)(cln->chars), (char*)(mn->chars));
                                 toki-=3;
                                 Dparseerror;
                              }
                              if(tok==TOKEN_INC)
                              {
                                 if(Dgetparami(2)==1)
                                 {
                                    // object.member++ ; statement
                                    PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecStatY)(co, ccmdy, setcmdy, 0, clt->template_object->class_ID);
                                    setPNode(nn);
                                 }
                                 else
                                 {
                                    // object.member++ expr
                                    PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecExprY)(co, ccmdy, setcmdy, 1, 0, clt->template_object->class_ID);
                                    setPNode(nn);
                                 }
                              }
                              else
                              {
                                 if(Dgetparami(2)==1)
                                 {
                                    // object.member-- ; statement
                                    PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecStatY)(co, ccmdy, setcmdy, 1, clt->template_object->class_ID);
                                    setPNode(nn);
                                 }
                                 else
                                 {
                                    // object.member-- expr
                                    PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecExprY)(co, ccmdy, setcmdy, 1, 1, clt->template_object->class_ID);
                                    setPNode(nn);
                                 }
                              }
                           }
                        }
                        popState();
                        break;
                     } // end local / static class co.member


                     co=0;
                     if( (NULL == cnspace) && cclassdecl)
                     {
                        co=cclassdecl->getMember(cln);
                        if(!b_force_this)
                        {
                           DcheckFunMemberAccess(cln);
                        }
                        if(co)
                        {
                           b_force_this = 0;
                           DcheckMemberAccess(cclassdecl, cln);
                           sU8 mrtti = (sU8) ((co->value.int_val >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK);
                           switch(mrtti)
                           {
                              default:
                                 // int, float
                              {
                                 Dprintf("\n[---] class \"%s\" member \"%s\" has no method named \"%s\".\n",
                                         (char*)cclassdecl->name.chars,
                                         (char*)cln->chars,
                                         (char*)mn->chars);
                                 Dparseerror;
                              }
                              break;
                              case TKS_CLASSDECL_RTTI_OBJECT:
                              case TKS_CLASSDECL_RTTI_STRING:
                              case TKS_CLASSDECL_RTTI_INTARRAY:
                              case TKS_CLASSDECL_RTTI_FLOATARRAY:
                              case TKS_CLASSDECL_RTTI_OBJECTARRAY:
                              case TKS_CLASSDECL_RTTI_STRINGARRAY:
                              case TKS_CLASSDECL_RTTI_POINTERARRAY:
                              case TKS_CLASSDECL_RTTI_CLASSARRAY:
                              {
                                 // called instance is object
                                 // find associated command
                                 TKS_ClassDeclMember *clm=cclassdecl->findMemberSuper(cln, 0/*non-static*/);
                                 DcheckMemberAccess(cclassdecl, cln);
                                 // must exist since getMember() succeeded
                                 // get template object
                                 YAC_Object *to=clm->value.object_val;
                                 YAC_String classname; classname.visit((char*)to->yacClassName());
                                 TKS_ClassTemplate *clt=tkscript->findTemplate(&classname);
                                 if(!clt)
                                 {
                                    Dprintf("\n[---] unknown class template \"%s\"\n", (char*)classname.chars);
                                    Dparseerror;
                                 }
                                 YAC_String getmethodname;
                                 getmethodname.copy("get"); // build get-method name
                                 getmethodname.append(mn);
                                 getmethodname.chars[3]&=~32; // make first char of member name uppercase

                                 const YAC_CommandY *ccmdy = clt->findCommandY(&getmethodname);
                                 if(!ccmdy)
                                 {
                                    Dprintf("\n[---] c++-class \"%s\" instance \"%s\" has no (readable) property called \"%s\"\n",
                                            (char*)(clt->class_name.chars), (char*)(cln->chars), (char*)(mn->chars));
                                    toki--;
                                    Dparseerror;
                                 }
                                 if( (tok!=TOKEN_INC)&&(tok!=TOKEN_DEC) )
                                 {
                                    // object.member XXXXXXXXX
                                    // Script class member call C++ object get*() method
                                    PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, NULL/*co*/, YAC_TRUE/*bIndirect*/, NULL/*arglist*/, clt->template_object->class_ID);
                                    setTNode(nn);
                                    PTN_NEW_NODE(nn, PTN_ClassMemberIndirectObjectECall)(co, tnode.callableexpr);
                                    setPNode(nn);
                                    ////tnode.ecally->cached_object=&popnode.classmemberindirectobjectecall->ecall_member;
                                    ////pnode.classmemberindirectobjectecall->b_indirect = 1;
                                    if(tok==TOKEN_DOT)
                                    {
                                       Dsetparamn(0, popnode.expr);
                                       state=ST_IEXPR;
                                       state_index=0;
                                       break;
                                    }
                                    else
                                    {
                                       if(tok==TOKEN_BEGARRAY)
                                       {
                                          Dsetparamn(0, popnode.expr);
                                          state=ST_IARRAYEXPR;
                                          state_index=0;
                                          break;
                                       }
                                       else
                                       {
                                          Dungettok;
                                       }
                                    }

                                 }
                                 else
                                 {
                                    // build write member method name
                                    YAC_String setmethodname;
                                    setmethodname.copy("set");
                                    setmethodname.append(mn);
                                    setmethodname.chars[3]&=~32;
                                    const YAC_CommandY *setcmdy = clt->findCommandY(&setmethodname);
                                    if(!setcmdy)
                                    {
                                       Dprintf("\n[---] c++-class \"%s\" has no (writable) property called \"%s\"\n",
                                               (char*)(classname.chars), (char*)(mn->chars));
                                       toki-=3;
                                       Dparseerror;
                                    }
                                    if(tok==TOKEN_INC)
                                    {
                                       if(Dgetparami(2)==1)
                                       {
                                          // object.member++ ; statement
                                          PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecStatY)(/*co*/0, ccmdy, setcmdy, 0, clt->template_object->class_ID);
                                          setTNode(nn);
                                          tnode.objectmemberincdecstaty->initIndirect();
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIndirectObjectStat)(co, tnode.statement);
                                          setPNode(nn);
                                          ////tnode.objectmemberincdecstaty->var=&popnode.classmemberindirectobjectstat->stat_member;
                                       }
                                       else
                                       {
                                          // object.member++ expr
                                          PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecExprY)(/*co*/0, ccmdy, setcmdy, 1, 0, clt->template_object->class_ID);
                                          setTNode(nn);
                                          tnode.objectmemberincdecexpry->initIndirect();
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIndirectObjectExpr)(co, tnode.expr);
                                          setPNode(nn);
                                          ////tnode.objectmemberincdecexpry->var=&popnode.classmemberindirectobjectexpr->expr_member;
                                       }
                                    }
                                    else
                                    {
                                       if(Dgetparami(2)==1)
                                       {
                                          // object.member-- ; statement
                                          PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecStatY)(/*co*/0, ccmdy, setcmdy, 1, clt->template_object->class_ID);
                                          setTNode(nn);
                                          tnode.objectmemberincdecstaty->initIndirect();
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIndirectObjectStat)(co, tnode.statement);
                                          setPNode(nn);
                                          ////tnode.objectmemberincdecstaty->var=&popnode.classmemberindirectobjectstat->stat_member;
                                       }
                                       else
                                       {
                                          // object.member-- expr
                                          PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecExprY)(/*co*/0, ccmdy, setcmdy, 1, 1, clt->template_object->class_ID);
                                          setTNode(nn);
                                          tnode.objectmemberincdecexpry->initIndirect();
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIndirectObjectExpr)(co, tnode.expr);
                                          setPNode(nn);
                                          ////tnode.objectmemberincdecexpry->var=&popnode.classmemberindirectobjectexpr->expr_member;
                                       }
                                    }
                                 }
                                 popState();
                              }
                              break;
                              case TKS_CLASSDECL_RTTI_CLASS:
                              {
                                 // called instance is class
                                 TKS_ClassDeclMember *clm=cclassdecl->findMemberSuper(cln, 0/*non-static*/);
                                 DcheckMemberAccess(cclassdecl, cln);
                                 TKS_ClassDecl *mdecl=clm->value.sobject_val->class_decl;
                                 TKS_ClassDeclMember *mmember=mdecl->findMemberSuper(mn, 0/*non-static*/);
                                 if(!mmember)
                                 {
                                    Dprintf("\n[---] class \"%s\" has no member named \"%s\"\n",
                                            (char*)(mdecl->name.chars), (char*)(mn->chars));
                                    Dparseerror;
                                 }
                                 TKS_CachedObject *cc_co=mdecl->getMember(mn);
                                 DcheckMemberAccess(mdecl, mn);
                                 sBool preinc = ((Dgetparami(2)&2) == 2);
                                 sBool predec = ((Dgetparami(2)&4) == 4);
                                 sBool bForceSt = ((Dgetparami(2)&1) == 1);

                                 if(bForceSt) // 23Oct2015 (do not allow ptrassign within tripleargexpr  (e.g. "if(myclass.mymember <= myvar <= 10)")
                                 {
                                    if(tok == TOKEN_CMPLE) // check for pointerassign
                                    {
                                       if(preinc || predec)
                                       {
                                          Dprintf("\n[---] cannot preinc/predec and pointer assign at the same time.\n");
                                          Dparseerror;
                                       }
                                       Dsetparamv(0, (void *)co);
                                       Dsetparamv(1, (void *)cc_co);
                                       Dsetparami(2, mdecl->class_id);
                                       state_index = 6;
                                       Dpushstate;
                                       state = ST_EXPR;
                                       state_index = 0;
                                       break;
                                    }
                                 }

                                 if(Dgetparami(2)==1) // emit statement ?
                                 {
                                    if(preinc||predec)
                                    {
                                       Dprintf("\n[---] cannot preinc/predec and postinc at the same time.\n");
                                       Dparseerror;
                                    }
                                    if(tok==TOKEN_INC)
                                    {
                                       // class.member++ statement
                                       PTN_NEW_NODE(nn, PTN_ClassMemberIncDecStat)(/*co*/0, cc_co, 0);
                                       setTNode(nn);
                                       tnode.classmemberincdecstat->class_id=mdecl->class_id;
                                       PTN_NEW_NODE(nn, PTN_ClassMemberIndirectClassStat)(co, tnode.statement);
                                       setPNode(nn);
                                       ////tnode.classmemberincdecstat->var=&popnode.classmemberindirectclassstat->stat_member;
                                       popState();
                                    }
                                    else
                                    {
                                       if(tok==TOKEN_DEC)
                                       {
                                          // class.member-- statement
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIncDecStat)(/*co*/0, cc_co, 1);
                                          setTNode(nn);
                                          tnode.classmemberincdecstat->class_id=mdecl->class_id;
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIndirectClassStat)(co, tnode.statement);
                                          setPNode(nn);
                                          ////tnode.classmemberincdecstat->var=&popnode.classmemberindirectclassstat->stat_member;
                                          popState();
                                       }
                                       else
                                       {
                                          Dprintf("\n[---] class.member statement not possible.\n");
                                          Dparseerror;
                                       }
                                    }
                                 }
                                 else
                                 {
                                    if(preinc)
                                    {
                                       PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(/*co*/0, cc_co, 0, 0);
                                       setTNode(nn);
                                       tnode.classmemberincdecexpr->class_id=mdecl->class_id;
                                       PTN_NEW_NODE(nn, PTN_ClassMemberIndirectClassExpr)(co, tnode.expr);
                                       setPNode(nn);
                                       ////tnode.classmemberincdecexpr->var=&popnode.classmemberindirectclassexpr->expr_member;
                                       Dungettok;
                                       popState();
                                    }
                                    else
                                    {
                                       if(predec)
                                       {
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(/*co*/0, cc_co, 0, 1);
                                          setTNode(nn);
                                          tnode.classmemberincdecexpr->class_id=mdecl->class_id;
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIndirectClassExpr)(co, tnode.expr);
                                          setPNode(nn);
                                          ////tnode.classmemberincdecexpr->var=&popnode.classmemberindirectclassexpr->expr_member;
                                          Dungettok;
                                          popState();
                                       }
                                       else
                                       {
                                          if(tok==TOKEN_INC)
                                          {
                                             // class.member++ expr
                                             PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(/*co*/0, cc_co, 1, 0);
                                             setTNode(nn);
                                             tnode.classmemberincdecexpr->class_id=mdecl->class_id;
                                             PTN_NEW_NODE(nn, PTN_ClassMemberIndirectClassExpr)(co, tnode.expr);
                                             setPNode(nn);
                                             ////tnode.classmemberincdecexpr->var=&popnode.classmemberindirectclassexpr->expr_member;
                                             popState();
                                          }
                                          else
                                          {
                                             if(tok==TOKEN_DEC)
                                             {
                                                // class.member-- expr
                                                PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(/*co*/0, cc_co, 1, 1);
                                                setTNode(nn);
                                                tnode.classmemberincdecexpr->class_id=mdecl->class_id;
                                                PTN_NEW_NODE(nn, PTN_ClassMemberIndirectClassExpr)(co, tnode.expr);
                                                setPNode(nn);
                                                ////tnode.classmemberincdecexpr->var=&popnode.classmemberindirectclassexpr->expr_member;
                                                popState();
                                             }
                                             else
                                             {
                                                // class.member expr
                                                PTN_NEW_NODE(nn, PTN_ClassMemberExpr)(co, cc_co);
                                                setTNode(nn);
                                                tnode.classmemberexpr->class_id=mdecl->class_id;
                                                PTN_NEW_NODE(nn, PTN_ClassMemberIndirectClassExpr)(co, tnode.expr);
                                                setPNode(nn);
                                                ////tnode.classmemberexpr->var=&popnode.classmemberindirectclassexpr->expr_member;
                                                if(tok==TOKEN_DOT)
                                                {
                                                   Dsetparamn(0, popnode.expr);
                                                   state=ST_IEXPR;
                                                   state_index=0;
                                                }
                                                else
                                                {
                                                   if(tok==TOKEN_BEGARRAY)
                                                   {
                                                      Dsetparamn(0, popnode.expr);
                                                      state=ST_IARRAYEXPR;
                                                      state_index=0;
                                                   }
                                                   else
                                                   {
                                                      Dungettok;
                                                      popState();
                                                   }
                                                }
                                             }
                                          }
                                       }
                                    }
                                 }
                              }
                              break;
                           }
                        } // if(co) else
                        else if(b_force_this) // this.member.othermember<=
                        {
                           Dprintf("[---] class \"%s\" has no member called \"%s\".\n",
                                   (char*)cclassdecl->name.chars,
                                   (char*)sv->chars);
                           Dparseerror;
                        }
                     } // if (cclassdecl)

                     if(!co)
                     {
                        ////co = findVarGlobal(cln);
                        co = findVar(cln, active_namespaces, cnspace);

                        /*if(!co)
                          {
                          co=tkscript->global_template_namespace.findEntry(cln);
                          if(co)
                          {
                          TKS_ClassTemplate *t=(TKS_ClassTemplate*)co->value.object_val;
                          co=&t->static_co;
                          }
                          }*/
                        if(!co /*module.member*/)
                        {
                           // note: module.member path
                           if(NULL != cnspace)
                           {
                              Dprintf("[---] failed to resolve variable \"%s::%s\".\n",
                                      (char*)cnspace->name.chars,
                                      (char*)cln->chars
                                      );
                              Dparseerror;
                           }

                           // is not object.member path
                           // find module
                           if(tok==TOKEN_CMPLE)
                           {
                              if(par2!=1)
                              {
                                 Dparseerror;
                              }
                              else
                              {
                                 // module.member<=expr
                                 Dsetparams(0, cln);
                                 Dsetparams(1, mn);
                                 state_index=7;
                                 Dpushstate;
                                 state=ST_EXPR;
                                 state_index=0;
                              }
                           }
                           else
                           {
                              if(!tkscript->configuration.b_enable_unresolved_members)
                              {
                                 Dprintf("[---] cross referencing for identifier \"%s\" is turned OFF.\n",
                                         (char*)mn->chars);
                                 Dparseerror;
                              }
                              else
                              {
                                 if(tok==TOKEN_INC)
                                 {
                                    if(par2==1)
                                    {
                                       // classinstance.member++ ; statement
                                       PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberPostIncDecStat)(cln, 0, mn, 0);
                                       setPNode(nn);
                                       Dfreezenspaces(popnode.unresolvedmodulememberpostincdecstat->nspace_array);
                                    }
                                    else
                                       if(par2)
                                       {
                                          Dprintf("\n[---] cannot preinc/dec and postinc at the same time.\n");
                                          Dparseerror;
                                       }
                                       else
                                       {
                                          // classinstance.member++ expr
                                          PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberIncDecExpr)(cln, 0, mn, 1, 0);
                                          setPNode(nn);
                                          Dfreezenspaces(popnode.unresolvedmodulememberincdecexpr->nspace_array);
                                       }
                                 }
                                 else
                                 {
                                    if(tok==TOKEN_DEC)
                                    {
                                       if(par2==1)
                                       {
                                          // classinstance.member-- ; statement
                                          PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberPostIncDecStat)(cln, 0, mn, 1);
                                          setPNode(nn);
                                          Dfreezenspaces(popnode.unresolvedmodulememberpostincdecstat->nspace_array);
                                       }
                                       else
                                          if(par2)
                                          {
                                             Dprintf("\n[---] cannot preinc/dec and postdec at the same time.\n");
                                             Dparseerror;
                                          }
                                          else
                                          {
                                             // classinstance.member-- expr
                                             PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberIncDecExpr)(cln, 0, mn, 1, 1);
                                             setPNode(nn);
                                             Dfreezenspaces(popnode.unresolvedmodulememberincdecexpr->nspace_array);
                                          }
                                    }
                                    else
                                    {
                                       // check for pre INC, DEC
                                       if((par2&2)==2)
                                       {
                                          // ++classinst.member expr
                                          PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberIncDecExpr)(cln, 0, mn, 0, 0);
                                          setPNode(nn);
                                          Dfreezenspaces(popnode.unresolvedmodulememberincdecexpr->nspace_array);
                                       }
                                       else
                                       {
                                          if((par2&4)==4)
                                          {
                                             // --classinst.member expr
                                             PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberIncDecExpr)(cln, 0, mn, 0, 1);
                                             setPNode(nn);
                                             Dfreezenspaces(popnode.unresolvedmodulememberincdecexpr->nspace_array);
                                          }
                                          else
                                          {
                                             // classinst.member expr
                                             PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberExpr)(cln, 0, mn);
                                             setPNode(nn);
                                             Dfreezenspaces(popnode.unresolvedmodulememberexpr->nspace_array);
                                             if(tok==TOKEN_DOT)
                                             {
                                                Dsetparamn(0, popnode.expr);
                                                state=ST_IEXPR;
                                                state_index=0;
                                                break;
                                             }
                                             else
                                             {
                                                if(tok==TOKEN_BEGARRAY)
                                                {
                                                   Dsetparamn(0, popnode.expr);
                                                   state=ST_IARRAYEXPR;
                                                   state_index=0;
                                                   break;
                                                }
                                             }
                                          }
                                       }
                                       Dungettok;
                                    }
                                 }
                                 popState();
                              }
                           } // if tok==CMPLE
                        } // is not module member V
                        else // if !co local or class member
                        {
                           cnspace = NULL;

                           // local variable or static script/c++ class member found
                           if(co->flags&TKS_CachedObject::ISDYNAMIC)
                           {
                              PTN_NEW_NODE(nn, PTN_VarExpr)(co);
                              setPNode(nn);
                              Dsetparamn(0, popnode.expr);
                              Dungettok;
                              Dungettok;
                              Dungettok;
                              state=ST_IEXPR;
                              state_index=0;
                              break;
                           }

                           if(co->type<YAC_TYPE_OBJECT)
                           {
                              Dprintf("\n[---] \"%s\" is not an Object.\n", (char*)(cln->chars));
                              Dparseerror;
                           }
                           if(!co->value.object_val)
                           {
                              Dprintf("\n[---] cannot address empty (Pointer) object \"%s\".\n",
                                      (char*)cln->chars);
                              Dparseerror;
                           }

                           if(co->value.object_val->class_ID==YAC_CLID_CLASS)
                           {
                              TKS_ScriptClassInstance *cc=(TKS_ScriptClassInstance*)co->value.object_val;

                              TKS_CachedObject *cc_co=cc->getMember(mn);
                              //DcheckMemberAccess(cc->class_decl, mn)
                              if(!cc_co)
                              {
                                 Dprintf("\n[---] %sclass \"%s::%s\" has no member called \"%s\".\n",
                                         (!cc->class_decl->is_declared ? "undeclared" : ""),
                                         (char*)cc->class_decl->nspace->name.chars,
                                         (char*)cc->class_decl->name.chars,
                                         (char*)mn->chars);
                                 Dparseerror;
                              }
                              else
                              {
                                 DcheckMemberAccess(cc->class_decl, mn);
                                 sBool preinc = ((Dgetparami(2)&2)==2);
                                 sBool predec = ((Dgetparami(2)&4)==4);

                                 if((Dgetparami(2)==1)&&(tok==TOKEN_CMPLE)) // check for classmember pointerassign
                                 {
                                    // class.member<=objexpr
                                    if(preinc||predec)
                                    {
                                       Dprintf("\n[---] cannot preinc/predec and pointer assign at the same time.\n");
                                       Dparseerror;
                                    }
                                    Dsetparamv(0, (void *)co);
                                    Dsetparamv(1, (void *)cc_co);
                                    state_index=5;
                                    Dpushstate;
                                    state=ST_EXPR;
                                    state_index=0;
                                    break;
                                 }
                                 else
                                 {
                                    if(Dgetparami(2)==1)
                                    {
                                       if(preinc||predec)
                                       {
                                          Dprintf("\n[---] cannot preinc/predec and postinc at the same time.\n");
                                          Dparseerror;
                                       }
                                       if(tok==TOKEN_INC)
                                       {
                                          // class.member++ statement
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIncDecStat)(co, cc_co, 0);
                                          setPNode(nn);
                                       }
                                       else
                                       {
                                          if(tok==TOKEN_DEC)
                                          {
                                             // class.member-- statement
                                             PTN_NEW_NODE(nn, PTN_ClassMemberIncDecStat)(co, cc_co, 1);
                                             setPNode(nn);
                                          }
                                          else
                                          {
                                             Dprintf("\n[---] class.member statement not possible.\n");
                                             Dparseerror;
                                          }
                                       }
                                    }
                                    else
                                    {
                                       if(preinc)
                                       {
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(co, cc_co, 0, 0);
                                          setPNode(nn);
                                          Dungettok;
                                       }
                                       // local variable or static script/c++ class member found
                                       if(co->flags&TKS_CachedObject::ISDYNAMIC)
                                       {
                                          PTN_NEW_NODE(nn, PTN_VarExpr)(co);
                                          setPNode(nn);
                                          Dsetparamn(0, popnode.expr);
                                          state=ST_IEXPR;
                                          state_index=0;
                                          break;
                                       }
                                       else
                                       {
                                          if(predec)
                                          {
                                             PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(co, cc_co, 0, 1);
                                             setPNode(nn);
                                             Dungettok;
                                          }
                                          else
                                          {
                                             if(tok==TOKEN_INC)
                                             {
                                                // class.member++ expr
                                                PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(co, cc_co, 1, 0);
                                                setPNode(nn);
                                                //popState();
                                             }
                                             else
                                             {
                                                if(tok==TOKEN_DEC)
                                                {
                                                   // class.member-- expr
                                                   PTN_NEW_NODE(nn, PTN_ClassMemberIncDecExpr)(co, cc_co, 1, 1);
                                                   setPNode(nn);
                                                   //popState();
                                                }
                                                else
                                                {
                                                   // class.member expr
                                                   PTN_NEW_NODE(nn, PTN_ClassMemberExpr)(co, cc_co);
                                                   setPNode(nn);
                                                   if(tok==TOKEN_DOT)
                                                   {
                                                      Dsetparamn(0, popnode.expr);
                                                      state=ST_IEXPR;
                                                      state_index=0;
                                                      break;
                                                   }
                                                   else
                                                   {
                                                      if(tok==TOKEN_BEGARRAY)
                                                      {
                                                         Dsetparamn(0, popnode.expr);
                                                         state=ST_IARRAYEXPR;
                                                         state_index=0;
                                                         break;
                                                      }
                                                      else
                                                      {
                                                         Dungettok;
                                                      }
                                                   }
                                                }
                                             }
                                          }
                                       }
                                    }
                                 } // CMPLE
                              }
                           }
                           else // !SCRIPTCLASS
                           {
                              YAC_String getmethodname;
                              getmethodname.copy("get"); // build get-method name
                              getmethodname.append(mn);
                              getmethodname.chars[3]&=~32; // make first char of member name uppercase
                              YAC_String classname; classname.visit((char*)co->value.object_val->yacClassName());
                              TKS_ClassTemplate *clt=tkscript->findTemplate(&classname);
                              if(!clt)
                              {
                                 Dprintf("\n[---] unknown class template \"%s\"\n", (char*)classname.chars);
                                 Dparseerror;
                              }
                              const YAC_CommandY *ccmdy = clt->findCommandY(&getmethodname);
                              if(!ccmdy)
                              {
                                 Dprintf("\n[---] c++-class \"%s\" instance \"%s\" has no (readable) property called \"%s\"\n",
                                         (char*)(clt->class_name.chars), (char*)(cln->chars), (char*)(mn->chars));
                                 toki--;
                                 Dparseerror;
                              }
                              if( (tok!=TOKEN_INC) && (tok!=TOKEN_DEC) )
                              {
                                 // ---- object.member ----
                                 // Call C++ get*() method
                                 PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, co, YAC_FALSE/*bIndirect*/, /**arglist**/0, clt->template_object->class_ID);
                                 setPNode(nn);
                                 if(tok==TOKEN_DOT)
                                 {
                                    Dsetparamn(0, popnode.expr);
                                    state=ST_IEXPR;
                                    state_index=0;
                                    break;
                                 }
                                 else
                                 {
                                    if(tok==TOKEN_BEGARRAY)
                                    {
                                       Dsetparamn(0, popnode.expr);
                                       state=ST_IARRAYEXPR;
                                       state_index=0;
                                       break;
                                    }
                                    else
                                    {
                                       Dungettok;
                                    }
                                 }

                              }
                              else
                              {
                                 // build write member method name
                                 YAC_String setmethodname;
                                 setmethodname.copy("set");
                                 setmethodname.append(mn);
                                 setmethodname.chars[3]&=~32;
                                 const YAC_CommandY *setcmdy = clt->findCommandY(&setmethodname);
                                 if(!setcmdy)
                                 {
                                    Dprintf("\n[---] c++-class \"%s\" has no (writable) property called \"%s\"\n",
                                            (char*)(cln->chars), (char*)(mn->chars));
                                    toki-=3;
                                    Dparseerror;
                                 }
                                 if(tok==TOKEN_INC)
                                 {
                                    if(Dgetparami(2)==1)
                                    {
                                       // object.member++ ; statement
                                       PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecStatY)(co, ccmdy, setcmdy, 0, clt->template_object->class_ID);
                                       setPNode(nn);
                                    }
                                    else
                                    {
                                       // object.member++ expr
                                       PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecExprY)(co, ccmdy, setcmdy, 1, 0, clt->template_object->class_ID);
                                       setPNode(nn);
                                    }
                                 }
                                 else
                                 {
                                    if(Dgetparami(2)==1)
                                    {
                                       // object.member-- ; statement
                                       PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecStatY)(co, ccmdy, setcmdy, 1, clt->template_object->class_ID);
                                       setPNode(nn);
                                    }
                                    else
                                    {
                                       // object.member-- expr
                                       PTN_NEW_NODE(nn, PTN_ObjectMemberIncDecExprY)(co, ccmdy, setcmdy, 1, 1, clt->template_object->class_ID);
                                       setPNode(nn);
                                    }
                                 }
                              }
                           }
                           popState();
                        } // end global co.member
                     } // !co
                  }
                  break;

                  // 4=<ARGLIST>    //<FUNARGS>
                  case 5: // ")"
                     // XXXXX DEAD CODE ???????????

                     if(tok==TOKEN_ENDEXPR) // ")"
                     {
                        // <ECALL> finished
                        YAC_String *cln=Dgetparams(0);
                        YAC_String *mn=Dgetparams(1);
                        co=findVar(cln, active_namespaces, cnspace);
                        cnspace = NULL;
                        if(!co)
                        {
                           Dprintf("\n[---] unknown reference to Object \"%s\".\n", (char*)(cln->chars));
                           Dparseerror;
                        }
                        if(co->type<YAC_TYPE_OBJECT)
                        {
                           Dprintf("\n[---] \"%s\" is not an Object.\n", (char*)(cln->chars));
                           Dparseerror;
                        }
                        if(!co->value.object_val)
                        {
                           PTN_NEW_NODE(nn, PTN_VarExpr)(co);
                           setTNode(nn);
                           PTN_NEW_NODE(nn, PTN_IMethodECall)(tnode.expr, mn, popnode.arglist);
                           setPNode(nn);
                           Dtryindirectobjectexpr;
                        }
                        else
                        {
                           YAC_String cln; cln.visit((char*)co->value.object_val->yacClassName());
                           TKS_ClassTemplate *clt=tkscript->findTemplate(&cln);
                           if(!clt)
                           {
                              Dprintf("\n[---] unknown class template \"%s\"\n", (char*)cln.chars);
                              Dparseerror;
                           }
                           const YAC_CommandY *ccmdy = clt->findCommandY(mn);
                           if(!ccmdy)
                           {
                              Dprintf(ERRTKS_CLASSCOMMANDNOTFOUND,
                                      (char*)cln.chars, (char*)(mn->chars));
                              Dparseerror;
                           }
                           PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, co, YAC_FALSE/*bIndirect*/, popnode.arglist, clt->template_object->class_ID);
                           setPNode(nn);

                           if(TOKEN_RETOBJASSIGN == ntok) // => follows?
                           {
                              Dsetparamn(0, popnode.node);
                              Dsetparami(1, 0);
                              if(TOKEN_MUL == nntok)
                              {
                                 state_index = 295;
                              }
                              else
                              {
                                 state_index = 300;
                              }
                              break;
                           }
                           else if(TOKEN_RETOBJRET == ntok) // <=> follows?
                           {
                              Dsetparamn(0, popnode.node);
                              Dsetparami(1, 1);
                              if(TOKEN_MUL == nntok)
                              {
                                 state_index = 295;
                              }
                              else
                              {
                                 state_index = 300;
                              }
                              break;
                           }
                           else
                           {
                              /*if(!popnode.ecally->handleRootCommand())
                                {
                                #ifdef TKS_DCON
                                tkscript->printf("[---] handleRootCommand() failed (missing => return expr or RVAL implementation?).\n");
                                #endif
                                Dparseerror;
                                }*/
                              Dtryindirectobjectexprnopopstate;
                           }
                           ////popState();
                        }
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;

                  case 6:
                     // class.member<=expr
                     // <EXPR> finished
                  {
                     TKS_CachedObject *class_var=(TKS_CachedObject*)Dgetparamv(0);
                     TKS_CachedObject *class_member=(TKS_CachedObject*)Dgetparamv(1);
                     PTN_NEW_NODE(nn, PTN_ClassMemberPointerAssign)(class_var, class_member, popnode.expr);
                     setPNode(nn);
                     popState();
                     Dungettok;
                  }
                  break;

                  case 7:
                     // class.member<=expr within class
                     // <EXPR> finished
                  {
                     TKS_CachedObject *class_var=(TKS_CachedObject*)Dgetparamv(0);
                     TKS_CachedObject *class_member=(TKS_CachedObject*)Dgetparamv(1);
                     PTN_NEW_NODE(nn, PTN_ClassMemberPointerAssign)(/*co*/0, class_member, popnode.expr);
                     setTNode(nn);
                     tnode.classmemberpointerassign->class_id=Dgetparami(2);
                     PTN_NEW_NODE(nn, PTN_ClassMemberIndirectClassStat)(class_var, tnode.statement);
                     setPNode(nn);
                     ////tnode.classmemberpointerassign->var=&popnode.classmemberindirectclassstat->stat_member;
                     popState();
                     Dungettok;
                  }
                  break;

                  case 8:
                     // unresolved module.member<=expr
                     // <EXPR> finished
                  {
                     if(tkscript->configuration.b_enable_unresolved_members)
                     {
                        PTN_NEW_NODE(nn, PTN_UnresolvedModuleMemberPointerAssign)(Dgetparams(0), 0, Dgetparams(1), popnode.expr);
                        setPNode(nn);
                        Dfreezenspaces(popnode.unresolvedmodulememberpointerassign->nspace_array);
                        popState();
                        Dungettok;
                     }
                     else
                     {
                        Dprintf("[---] cross referencing for identifier \"%s\" is turned OFF.\n",
                                (char*)Dgetparams(1)->chars);
                        Dparseerror;
                     }
                  }
                  break;

                  case 295: // consume '=>' after C++ method call and before * token
                     state_index++;
                     break;
                  case 296: // consume '*' after =>
                  {
                     PTN_ECallY *ecally = Dgetparamn(0).ecally;
                     ecally->b_rself = 1;
                     ecally->b_ret_retobj = Dgetparami(1);
                     popnode.ecally = ecally;
                     /*if(!popnode.ecally->handleRootCommand())
                       {
                       #ifdef TKS_DCON
                       tkscript->printf("[---] handleRootCommand() failed (method does not support => return expr?).\n");
                       #endif
                       Dparseerror;
                       }
                       else*/
                     {
                        Dtryindirectobjectexprnopopstate
                           }
                     popState();
                  }
                  break;

                  case 300: // consume '=>' after C++ method call
                     Dpushstate;
                     state = ST_EXPR;
                     state_index = 0;
                     break;
                  case 301: // => "retobjexpr" finished
                  {
                     PTN_ECallY *ecally = Dgetparamn(0).ecally;
                     ecally->retobj_expr = popnode.expr;
                     ecally->b_ret_retobj = Dgetparami(1);
                     popnode.ecally = ecally;
                     sUI t = tok;
                     Dungettok;
                     ntok = t; // For macro call
                     /*if(!popnode.ecally->handleRootCommand())
                       {
                       #ifdef TKS_DCON
                       tkscript->printf("[---] handleRootCommand() failed (method does not support => return expr?).\n");
                       #endif
                       Dparseerror;
                       }
                       else*/
                     {
                        Dtryindirectobjectexprnopopstate
                           }
                     popState();
                  }
                  break;


               }
               break; // end of ST_READPROPERTY


            case ST_ECALL: // note: cnspace must already be set
               // params:
               //     0 - class/var name
               //     1 - method name (temp)
               //     3 - cnspace save
               switch(state_index)
               {
                  case 0:
                     if(tok==TOKEN_VAR)
                     {
                        Dsetparams(0, sv); // remember object resp. class name
                        state_index++;
                     }
                     /*else if(tok==TOKEN_THIS)
                       {
                       sv = &tks_this_name;
                       Dsetparams(0, sv);
                       state_index++;
                       }*/
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 1:
                     if(tok==TOKEN_DOT) // '.'
                     {
                        state_index++;
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 2: // method-name
                     if(tok==TOKEN_VAR)
                     {
                        Dsetparams(1,sv); // remember method name
                        state_index++;
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;
                  case 3: // "("
                     if(tok==TOKEN_BEGEXPR) // "("
                     {
                        state_index++; // popstate skips <ST_ARGLIST>
                        Dsetparamnspace(3, cnspace);
                        cnspace = NULL;
                        Dpushstate;
                        state=ST_ARGLIST;
                        state_index=0;
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;

                     // 4=<ST_ARGLIST>
                  case 5: // ")"
                     if(tok==TOKEN_ENDEXPR) // ")"
                     {
                        // <ST_ARGLIST> finished
                        YAC_String *cln=Dgetparams(0);
                        YAC_String *mn=Dgetparams(1);
                        cnspace = Dgetparamnspace(3);

                        TKS_ClassTemplate *clt=0;

                        /*if(NULL == cnspace)
                          {
                          co = findVarLocal(cln);
                          }
                          else
                          {
                          co = NULL;
                          }*/

                        co = findVar(cln, active_namespaces, cnspace);

                        if(co)
                        {
                           if( (!co->value.object_val) || (co->type<YAC_TYPE_OBJECT) )
                           {
                              cnspace = NULL;
                              PTN_NEW_NODE(nn, PTN_VarExpr)(co);
                              setTNode(nn);
                              PTN_NEW_NODE(nn, PTN_IMethodECall)(tnode.expr, mn, popnode.arglist);
                              setPNode(nn);
                              Dtryindirectobjectexpr;
                           }
                           else
                           {
                              // yac_host->printf("xxx findVar cln=\"%s\" cnspace=%p => co=%p\n", cln->chars, cnspace, co);
                              if(co->value.object_val->class_ID==YAC_CLID_CLASS)
                              {
                                 TKS_ScriptClassInstance *cc=(TKS_ScriptClassInstance*)co->value.object_val;
                                 TKS_CachedObject *cc_co=cc->getMethod(mn); // find class method
                                 if(cc_co)
                                 {
                                    DcheckMethodAccess(cc->class_decl, mn);
                                    cnspace = NULL;
                                    PTN_NEW_NODE(nn, PTN_ClassMethodECall)(co, (TKS_ClassDeclMethod*)cc_co->value.object_val, popnode.arglist);
                                    setPNode(nn);
                                    Dtryindirectobjectexpr;
                                 }
                                 else
                                 {
                                    const YAC_CommandY *ccmdy = NULL;
                                    clt = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_CLASS];
                                    ccmdy = clt->findCommandY(mn); // find yac* method
                                    if(!ccmdy)
                                    {
                                       Dprintf("\n[---] class \"%s::%s\" has no method named \"%s\"\n",
                                               (char*)cc->class_decl->nspace->name.chars,
                                               (char*)(cc->class_decl->name.chars), (char*)(mn->chars));
                                       Dparseerror;
                                    }
                                    else
                                    {
                                       // Script class instance yac* C++ call
                                       cnspace = NULL;
                                       PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, co, YAC_FALSE/*bIndirect*/, popnode.arglist, clt->template_object->class_ID);
                                       setPNode(nn);
                                       Dtryindirectobjectexpr;
                                    }
                                 }
                              }
                              else
                              {
                                 YAC_String classname; classname.visit((char*)co->value.object_val->yacClassName());
                                 clt=tkscript->findTemplate(&classname);
                                 if(!clt)
                                 {
                                    Dprintf("\n[---] unknown class template \"%s\"\n", (char*)classname.chars);
                                    Dparseerror;
                                 }
                                 const YAC_CommandY *ccmdy = clt->findCommandY(mn);
                                 if(!ccmdy)
                                 {
                                    Dprintf(ERRTKS_CLASSCOMMANDNOTFOUND,
                                            (char*)classname.chars, (char*)(mn->chars));
                                    Dparseerror;
                                 }
                                 // Regular C++ method call
                                 cnspace = NULL;
                                 PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, co, YAC_FALSE/*bIndirect*/, popnode.arglist, clt->template_object->class_ID);
                                 setPNode(nn);

                                 if(TOKEN_RETOBJASSIGN == ntok) // => follows?
                                 {
                                    Dsetparamn(0, popnode.node);
                                    Dsetparami(1, 0);
                                    if(TOKEN_MUL == nntok)
                                    {
                                       state_index = 95;
                                    }
                                    else
                                    {
                                       state_index = 100;
                                    }
                                    break;
                                 }
                                 else if(TOKEN_RETOBJRET == ntok) // <=> follows?
                                 {
                                    Dsetparamn(0, popnode.node);
                                    Dsetparami(1, 1);
                                    if(TOKEN_MUL == nntok)
                                    {
                                       state_index = 95;
                                    }
                                    else
                                    {
                                       state_index = 100;
                                    }
                                    break;
                                 }
                                 else
                                 {
                                    /*if(!popnode.ecally->handleRootCommand())
                                      {
                                      #ifdef TKS_DCON
                                      tkscript->printf("[---] handleRootCommand() failed (missing => return expr or RVAL implementation?).\n");
                                      #endif
                                      Dparseerror;
                                      }*/
                                    Dtryindirectobjectexpr;
                                 }
                              }
                           }
                           break;
                        } // if findVarLocal

                        if( (NULL == cnspace) && cclassdecl)
                        {
                           co = cclassdecl->getMember(cln);
                           DcheckFunMemberAccess(cln);
                           if(co)
                           {
                              DcheckMemberAccess(cclassdecl, cln);
                              // xxx PTN_ClassMemberIndirectObjectCallExpr
                              sUI cpi=co->value.int_val;
                              sU8 mrtti = (sU8) ((cpi >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK);
                              switch(mrtti)
                              {
                                 default:
                                    // ---- int, float
                                 {
                                    Dprintf("\n[---] class \"%s\" member \"%s\" has no method named \"%s\".\n",
                                            (char*)cclassdecl->name.chars,
                                            (char*)cln->chars,
                                            (char*)mn->chars);
                                    Dparseerror;
                                 }
                                 break;
                                 case TKS_CLASSDECL_RTTI_POINTER:
                                 case TKS_CLASSDECL_RTTI_OBJECT:
                                 case TKS_CLASSDECL_RTTI_STRING:
                                 case TKS_CLASSDECL_RTTI_INTARRAY:
                                 case TKS_CLASSDECL_RTTI_FLOATARRAY:
                                 case TKS_CLASSDECL_RTTI_OBJECTARRAY:
                                 case TKS_CLASSDECL_RTTI_STRINGARRAY:
                                 case TKS_CLASSDECL_RTTI_POINTERARRAY:
                                 case TKS_CLASSDECL_RTTI_CLASSARRAY:
                                 {
                                    // ---- called instance is object
                                    // ---- find associated command
                                    TKS_ClassDeclMember *clm=cclassdecl->findMemberSuper(cln, 0/*non-static*/);
                                    DcheckMemberAccess(cclassdecl, cln);
                                    // ---- must exist since getMember() succeeded
                                    // ---- get template object
                                    YAC_Object *to=clm->value.object_val;
                                    if(NULL == to)
                                    {
                                       to = tkscript->atom_speedpath[YAC_CLID_OBJECT]->template_object;
                                    }
                                    YAC_String classname; classname.visit((char*)to->yacClassName());
                                    TKS_ClassTemplate *clt=tkscript->findTemplate(&classname);
                                    if(!clt)
                                    {
                                       Dprintf("\n[---] unknown class template \"%s\"\n", (char*)classname.chars);
                                       Dparseerror;
                                    }
                                    const YAC_CommandY *ccmdy = clt->findCommandY(mn);
                                    if(!ccmdy)
                                    {
                                       Dprintf(ERRTKS_CLASSCOMMANDNOTFOUND,
                                               (char*)classname.chars, (char*)(mn->chars));
                                       Dparseerror;
                                    }
                                    else
                                    {
                                       // Script class member C++ object call
                                       PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, NULL/*co*/, YAC_TRUE/*bIndirect*/, popnode.arglist, clt->template_object->class_ID);
                                       setTNode(nn);
                                    }
                                    PTN_NEW_NODE(nn, PTN_ClassMemberIndirectObjectECall)(co, tnode.callableexpr);
                                    setPNode(nn);
                                    ////tnode.ecally->cached_object=&popnode.classmemberindirectobjectecall->ecall_member;
                                    ////tnode.ecally->b_indirect = 1;

                                    if(TOKEN_RETOBJASSIGN == ntok) // => follows?
                                    {
                                       Dsetparamn(0, tnode.node);
                                       Dsetparami(1, 0);
                                       Dsetparamn(2, popnode.node);
                                       if(TOKEN_MUL == nntok)
                                       {
                                          state_index = 105;
                                       }
                                       else
                                       {
                                          state_index = 110;
                                       }
                                       break;
                                    }
                                    else if(TOKEN_RETOBJRET == ntok) // <=> follows?
                                    {
                                       Dsetparamn(0, tnode.node);
                                       Dsetparami(1, 1);
                                       Dsetparamn(2, popnode.node);
                                       if(TOKEN_MUL == nntok)
                                       {
                                          state_index = 105;
                                       }
                                       else
                                       {
                                          state_index = 110;
                                       }
                                       break;
                                    }
                                    else
                                    {
                                       /*if(!tnode.ecally->handleRootCommand())
                                         {
                                         #ifdef TKS_DCON
                                         tkscript->printf("[---] handleRootCommand() failed (missing => return expr or RVAL implementation?).\n");
                                         #endif
                                         Dparseerror;
                                         }*/
                                       ///////Dtryindirectobjectexprnopopstate;
                                    }

                                    Dtryindirectobjectexpr;
                                 }
                                 break;
                                 case TKS_CLASSDECL_RTTI_CLASS:
                                 {
                                    // called instance is class
                                    TKS_ClassDeclMember *clm=cclassdecl->findMemberSuper(cln, 0/*non-static*/);
                                    DcheckMemberAccess(cclassdecl, cln);
                                    TKS_ClassDecl *mdecl= clm->value.sobject_val->class_decl;
                                    TKS_ClassDeclMethod *mmeth=mdecl->getMethod(mn);
                                    if(mmeth)
                                    {
                                       DcheckMethodAccess(mdecl, mn);
                                       PTN_NEW_NODE(nn, PTN_ClassMethodECall)(0/*var*/, mmeth, popnode.arglist);
                                       setTNode(nn);
                                       tnode.classmethodecall->class_id=mdecl->class_id;
                                       PTN_NEW_NODE(nn, PTN_ClassMemberIndirectClassMethodECall)(co, tnode.classmethodecall); // calls initIndirect()
                                       setPNode(nn);
                                       ////tnode.classmethodecall->var=&popnode.classmemberindirectclassmethodecall->ecall_member;
                                       ////tnode.classmethodecall->b_indirect = 1;
                                       Dtryindirectobjectexpr;
                                    }
                                    else
                                    {
                                       TKS_ClassTemplate *clt=TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_CLASS];
                                       const YAC_CommandY*ccmdy = clt->findCommandY(mn); // find yac* method
                                       if(!ccmdy)
                                       {
                                          Dprintf("\n[---] class \"%s\" has no method named \"%s\"\n",
                                                  (char*)(mdecl->name.chars), (char*)(mn->chars));
                                          Dparseerror;
                                       }
                                       else
                                       {
                                          // Script class member C++ yac* call
                                          PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, NULL/*co*/, YAC_TRUE/*bIndirect*/, popnode.arglist, clt->template_object->class_ID);
                                          setTNode(nn);
                                          PTN_NEW_NODE(nn, PTN_ClassMemberIndirectObjectECall)(co, tnode.callableexpr);
                                          setPNode(nn);
                                          ////tnode.ecally->cached_object=&popnode.classmemberindirectobjectecall->ecall_member;
                                          ////tnode.ecally->b_indirect = 1;
                                          Dtryindirectobjectexpr;
                                       }
                                    }
                                 }
                                 break;
                              }
                           }
                        } // if cclassdecl

                        clt=0;
                        if(!co)
                        {
                           // ---- try to resolve call to static script class method
                           TKS_ClassDecl *cd;
                           if(NULL == cnspace)
                           {
                              cd = Dtkscompiler->findClassDeclNamespaceList(cln, active_namespaces);
                           }
                           else
                           {
                              cd = Dtkscompiler->findClassDeclNamespace(cln, cnspace);
                           }
                           if(cd)
                           {
                              clt=TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_OBJECT];
                              PTN_Function *f=cd->findFunctionSuper(mn);
                              // TODO checkClassFunctionAccess()
                              if(f)
                              {
                                 DcheckFunctionAccess(cd, f);
                                 PTN_NEW_NODE(nn, PTN_FunctionECall)(f, popnode.arglist);
                                 setPNode(nn);
                                 cnspace = NULL;
                                 popState();
                                 co=(TKS_CachedObject*)1;
                              }
                           }
                        }

                        // ---- try static C++ call
                        if(!co && ((NULL==cnspace) || (cnspace == &Dtkscompiler->yac_namespace)) )
                        {
                           if(!clt) // search scriptclass for yac* methods..
                           {
                              clt=(TKS_ClassTemplate*)tkscript->findTemplate(cln);
                           }
                           if(clt)
                           {
                              // static Class.method() expr
                              if( (!clt->may_instantiate) || (((*mn)[0]>='A')&&((*mn)[0]<='Z')) )
                              {
                                 const YAC_CommandY *ccmdy = clt->findCommandY(mn);
                                 if(!ccmdy)
                                 {
                                    Dprintf(ERRTKS_CLASSCOMMANDNOTFOUND,
                                            (char*)(cln->chars), (char*)(mn->chars));
                                    Dparseerror;
                                 }
                                 // "static" C++ method call
                                 cnspace = NULL;
                                 PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, &clt->static_co, YAC_FALSE/*bIndirect*/, popnode.arglist, clt->template_object->class_ID);
                                 setPNode(nn);

                                 if(TOKEN_RETOBJASSIGN == ntok) // => follows?
                                 {
                                    Dsetparamn(0, popnode.node);
                                    Dsetparami(1, 0);
                                    if(TOKEN_MUL == nntok)
                                    {
                                       state_index = 95;
                                    }
                                    else
                                    {
                                       state_index = 100;
                                    }
                                    break;
                                 }
                                 else if(TOKEN_RETOBJRET == ntok) // <=> follows?
                                 {
                                    Dsetparamn(0, popnode.node);
                                    Dsetparami(1, 1);
                                    if(TOKEN_MUL == nntok)
                                    {
                                       state_index = 95;
                                    }
                                    else
                                    {
                                       state_index = 100;
                                    }
                                    break;
                                 }

                                 Dtryindirectobjectexpr;
                                 break;
                              }
                              else
                              {
                                 Dprintf("[---] cannot access non-static c++ method \"%s::%s\" without an instance.\n",
                                         (char*)cln->chars, (char*)mn->chars);
                                 Dparseerror;
                                 break;
                              }
                           }

                           // Try global/static class member
                           ////co = findVarGlobal(cln, active_namespaces, cnspace);
                           co = findVar(cln, active_namespaces, cnspace);

                           if(!co)
                           {
                              if(NULL == cnspace)
                              {
                                 // ---- myclassinstance.method(..
                                 if(tkscript->configuration.b_enable_unresolved_functions)
                                 {
                                    PTN_NEW_NODE(nn, PTN_UnresolvedModuleMethodECall)(cln, mn, popnode.arglist);
                                    setPNode(nn);
                                    Dtryindirectobjectexpr;
                                 }
                                 else
                                 {
                                    Dprintf("[---] cross referencing for function \"%s\" is turned OFF.\n",
                                            (char*)mn->chars);
                                    Dparseerror;
                                 }
                              }
                              else
                              {
                                 Dprintf("[---] cannot find variable \"%s::%s\".\n",
                                         (char*)cnspace->name.chars,
                                         (char*)cln->chars
                                         );
                                 Dparseerror;
                              }
                           }
                           else
                           {
                              cnspace = NULL;

                              if( (!co->value.object_val) || (co->type<YAC_TYPE_OBJECT) )
                              {
                                 PTN_NEW_NODE(nn, PTN_VarExpr)(co);
                                 setTNode(nn);
                                 PTN_NEW_NODE(nn, PTN_IMethodECall)(tnode.expr, mn, popnode.arglist);
                                 setPNode(nn);
                                 Dtryindirectobjectexpr;
                              }
                              else
                              {
                                 if(co->value.object_val->class_ID==YAC_CLID_CLASS)
                                 {
                                    TKS_ScriptClassInstance *cc=(TKS_ScriptClassInstance*)co->value.object_val;
                                    TKS_CachedObject *cc_co=cc->getMethod(mn); // find class method
                                    if(cc_co)
                                    {
                                       DcheckMethodAccess(cc->class_decl, mn);
                                       PTN_NEW_NODE(nn, PTN_ClassMethodECall)(co, (TKS_ClassDeclMethod*)cc_co->value.object_val, popnode.arglist);
                                       setPNode(nn);
                                       Dtryindirectobjectexpr;
                                    }
                                    else
                                    {
                                       clt = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_CLASS];
                                       const YAC_CommandY *ccmdy = clt->findCommandY(mn); // find yac* method
                                       if(!ccmdy)
                                       {
                                          Dprintf("\n[---] class \"%s::%s\" has no method named \"%s\"\n",
                                                  (char*)cc->class_decl->nspace->name.chars,
                                                  (char*)(cc->class_decl->name.chars), (char*)(mn->chars));
                                          Dparseerror;
                                       }
                                       else
                                       {
                                          // Script class instance C++ yac* call
                                          PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, co, YAC_FALSE/*bIndirect*/, popnode.arglist, clt->template_object->class_ID);
                                          setPNode(nn);
                                          Dtryindirectobjectexpr;
                                       }
                                    }
                                 }
                                 else
                                 {
                                    YAC_String classname; classname.visit((char*)co->value.object_val->yacClassName());
                                    clt=tkscript->findTemplate(&classname);
                                    if(!clt)
                                    {
                                       Dprintf("\n[---] unknown class template \"%s\"\n", (char*)classname.chars);
                                       Dparseerror;
                                    }
                                    const YAC_CommandY *ccmdy = clt->findCommandY(mn);
                                    if(!ccmdy)
                                    {
                                       Dprintf(ERRTKS_CLASSCOMMANDNOTFOUND,
                                               (char*)classname.chars, (char*)(mn->chars));
                                       Dparseerror;
                                    }
                                    // Regular C++ method call
                                    PTN_NEW_NODE(nn, PTN_ECallY)(ccmdy, co, YAC_FALSE/*bIndirect*/, popnode.arglist, clt->template_object->class_ID);
                                    setPNode(nn);
                                    if(TOKEN_RETOBJASSIGN == ntok) // => follows?
                                    {
                                       Dsetparamn(0, popnode.node);
                                       Dsetparami(1, 0);
                                       if(TOKEN_MUL == nntok)
                                       {
                                          state_index = 95;
                                       }
                                       else
                                       {
                                          state_index = 100;
                                       }
                                       break;
                                    }
                                    else if(TOKEN_RETOBJRET == ntok) // <=> follows?
                                    {
                                       Dsetparamn(0, popnode.node);
                                       Dsetparami(1, 1);
                                       if(TOKEN_MUL == nntok)
                                       {
                                          state_index = 95;
                                       }
                                       else
                                       {
                                          state_index = 100;
                                       }
                                       break;
                                    }
                                    else
                                    {
                                       /*if(!popnode.ecally->handleRootCommand())
                                         {
                                         #ifdef TKS_DCON
                                         tkscript->printf("[---] handleRootCommand() failed (missing => return expr or RVAL implementation?).\n");
                                         #endif
                                         Dparseerror;
                                         }*/
                                       Dtryindirectobjectexpr;
                                    }
                                 }
                              }
                           }
                        } // !co
                     }
                     else
                     {
                        Dparseerror;
                     }
                     break;

                  case 95: // consume '=>' after C++ method call and before * token
                     state_index++;
                     break;
                  case 96: // consume '*' after =>
                  {
                     PTN_ECallY *ecally = Dgetparamn(0).ecally;
                     ecally->b_rself = 1;
                     ecally->b_ret_retobj = Dgetparami(1);
                     popnode.ecally = ecally;
                     Dtryindirectobjectexprnopopstate
                        popState();
                  }
                  break;

                  case 100: // consume '=>' after C++ method call
                     Dpushstate;
                     state = ST_EXPR;
                     state_index = 0;
                     break;
                  case 101: // => "retobjexpr" finished
                  {
                     PTN_ECallY *ecally = Dgetparamn(0).ecally;
                     ecally->retobj_expr = popnode.expr;
                     ecally->b_ret_retobj = Dgetparami(1);
                     popnode.ecally = ecally;
                     sUI t = tok;
                     Dungettok;
                     ntok = t; // For macro call
                     Dtryindirectobjectexprnopopstate;
                     popState();
                  }
                  break;

                  case 105: // consume '=>' after indirect C++ method call (i.e. if C++ object is a class member) and before * token
                     state_index++;
                     break;
                  case 106: // consume '*' after =>
                  {
                     PTN_ECallY *ecally = Dgetparamn(0).ecally;
                     ecally->b_rself = 1;
                     ecally->b_ret_retobj = Dgetparami(1);
                     popnode.node = Dgetparamn(2).node;
                     Dtryindirectobjectexprnopopstate
                        popState();
                  }
                  break;

                  case 110: // consume '=>' after indirect C++ method call (i.e. if C++ object is a class member)
                     Dpushstate;
                     state = ST_EXPR;
                     state_index = 0;
                     break;
                  case 111: // => "retobjexpr" finished
                  {
                     PTN_ECallY *ecally = Dgetparamn(0).ecally;
                     ecally->retobj_expr = popnode.expr;
                     ecally->b_ret_retobj = Dgetparami(1);
                     popnode.node = Dgetparamn(2).node;
                     sUI t = tok;
                     Dungettok;
                     ntok = t; // For macro call
                     Dtryindirectobjectexprnopopstate;
                     popState();
                  }
                  break;

               }
               break; // end of ST_ECALL

            case ST_ARGLIST:
               switch(state_index)
               {
                  case 0:
                     if(tok==TOKEN_ENDEXPR) // ")"
                     {
                        // ---- empty arglist ---
                        popnode.arglist=(PTN_ArgList*)0;
                        popState();
                        Dungettok;
                     }
                     else
                     {
                        Dungettok;
                        // ---- parse arguments ----
                        PTN_NEW_NODE(nn, PTN_ArgList)();
                        Dsetparamn(0, nn);
                        Dpushstate;
                        state=ST_EXPR; state_index=0;
                     }
                     break;
                  case 1:
                     // ---- arg expr finished ----
                  {
                     PTN_ArgList *al=Dgetparamn(0).arglist;
                     al->addArg(popnode.expr);
                     if(tok==TOKEN_ENDEXPR)
                     {
                        // ---- arglist finished ----
                        popnode.arglist=al;
                        Dsetsrcloc;
                        parser_tree_num_nodes++;
                        Dungettok;
                        popState();
                     }
                     else
                     {
                        if(tok==TOKEN_LISTSEP)
                        {
                           state_index=0;
                           Dpushstate;
                           state=ST_EXPR; state_index=0;
                        }
                        else
                        {
                           Dparseerror;
                        }
                     }
                  }
               }
               break;

         }
      }
      return 1;
   }
   return 0;
}

TKS_CachedObject *TKS_CachedScript::findModuleVar(YAC_String *_name) const {
   return module_vars.findEntry(_name);
}

/*void TKS_CachedScript::createThisVariable(void) {
// Trick to make this.member_name = <expr> , this.method_name() work
// xxx TKS_MT: not thread safe
tks_this_name.visit("this");
TKS_CachedObject *co = tkscript->global_instance_namespace.createObjectEntry(&tks_this_name, NULL, 0);
tks_this_adr = (TKS_ScriptClassInstance**) &co->value.object_val;
}
*/

sBool TKS_CachedScript::findAddClassType(YAC_String *         _name,
                                         TKS_ActiveNamespace *_activeNSpaces,
                                         TKS_Namespace *      _cnspace,
                                         sUI                  _srcloc,
                                         TKS_ClassTemplate**  _retTemplate,
                                         TKS_ClassDecl**      _retClassDecl
                                         )

{
   //
   // Find C++ class template or script class decl in the given forced namespace (cnspace)
   // or in the activeNSpaces context.
   //
   // If cnspace is NULL, try to find script class first, if that does not exist,
   // try C++ class. If that doesn't exist either, add a script class forward decl.
   //
   *_retClassDecl = NULL;
   *_retTemplate = NULL;
   sBool bTryYAC = 1;
   sBool bForceYAC = (_cnspace == &Dtkscompiler->yac_namespace);

   if( !bForceYAC ) // is not forced C++ namespace?
   {
      if(NULL != _cnspace) // xxx bTryYAC = (NULL == cnspace)
      {
         // Explicit namespace is given, do not try to find C++ class
         bTryYAC = 0;
      }

      // Try to find existing script class, either in default namespace or in explicit nspace
      if(NULL == _cnspace)
      {
         *_retClassDecl = Dtkscompiler->findClassDeclNamespaceList(_name, _activeNSpaces);
      }
      else
      {
         *_retClassDecl = Dtkscompiler->findClassDeclNamespace(_name, _cnspace);
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

   if(NULL == *_retTemplate)
   {
      // Script class is not declared, yet -> Add forward decl.
      if(NULL == _cnspace)
      {
         // Use last activated namespace if no explicit namespace is given
         _cnspace = (NULL != tmod_class_nsp) ? tmod_class_nsp : create_namespace;
      }
      *_retClassDecl = Dtkscompiler->addClassDecl(_name, this, _srcloc, _cnspace);

      // Ok, Added script class
      return 1;
   }

   // Never reached
   return 0;
}


sBool TKS_CachedScript::findDeclaredClassType(YAC_String *         _name,
                                              TKS_ActiveNamespace *_activeNSpaces,
                                              TKS_Namespace *      _cnspace,
                                              TKS_ClassTemplate**  _retTemplate,
                                              TKS_ClassDecl**      _retClassDecl
                                              ) const

{
   //
   // Find C++ class template or script class decl in the given forced namespace (cnspace)
   // or in the activeNSpaces context.
   //
   // If cnspace is NULL, try to find script class first, if that does not exist,
   // try C++ class. If that doesn't exist either, return an error.
   //
   *_retClassDecl = NULL;
   *_retTemplate = NULL;
   sBool bTryYAC = 1;
   sBool bForceYAC = (_cnspace == &Dtkscompiler->yac_namespace);

   if( !bForceYAC ) // is not forced C++ namespace?
   {
      if(NULL != _cnspace) // xxx bTryYAC = (NULL == cnspace)
      {
         // Explicit namespace is given, do not try to find C++ class
         bTryYAC = 0;
      }

      // Try to find existing script class, either in default namespace or in explicit nspace
      if(NULL == _cnspace)
      {
         *_retClassDecl = Dtkscompiler->findClassDeclNamespaceList(_name, _activeNSpaces);
      }
      else
      {
         *_retClassDecl = Dtkscompiler->findClassDeclNamespace(_name, _cnspace);
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

   if(NULL == *_retTemplate)
   {
      // Script class is not declared, yet -> Add forward decl.
      if(NULL == _cnspace)
      {
         // Use last activated namespace if no explicit namespace is given
         _cnspace = _activeNSpaces->nspace;
      }
      Dprintf("[---] class \"%s::%s\" has not been declared.\n",
              (char*) _cnspace->name.chars,
              (char*) _name->chars
              );

      // Failed
      return 0;
   }

   // Never reached
   return 0;
}

void TKS_CachedScript::visitNodeAllocator(TKS_LinearAllocator *_allocator) {
   node_allocator       = _allocator;
   b_own_node_allocator = YAC_FALSE;
}

void TKS_CachedScript::newNodeAllocator(void) {
   node_allocator       = new(std::nothrow) TKS_LinearAllocator(TKS_NODEALLOCATOR_STRIPESIZE);
   b_own_node_allocator = YAC_TRUE;
}

void TKS_CachedScript::freeNodeAllocator(void) {
   if(b_own_node_allocator)
   {
      delete node_allocator;
      b_own_node_allocator = YAC_FALSE;
   }
   node_allocator = NULL;
}
