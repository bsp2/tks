/// PTN_FunctionSignalStatement.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTAnyNodePtr.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_CallableExpr.h"
#include "PTN_ArgListNode.h"
#include "PTN_ArgList.h"
#include "PTN_ArgVar.h"
#include "PTN_ArgVarList.h" 
#include "TKS_FormalArgument.h"
#include "TKS_ScriptObjectID.h"
#include "TKS_FunctionObject.h"
#include "PTN_Function.h"
#include "PTN_FunctionECall.h"
#include "PTN_FunctionECallY.h"
#include "PTN_Call.h"
#include "PTN_FunctionCall.h"
#include "TKS_Compiler.h"
#include "TKS_StateStackEntry.h"
#include "TKS_Token.h"
#include "TKS_TokenTable.h"
#include "TKS_TokenizedScript.h"
#include "TKS_CachedScript.h"
#include "TKS_Compiler.h"
#include "PTN_FunctionSignalStatement.h"


PTN_FunctionSignalStatement::PTN_FunctionSignalStatement(void) {
  function_object = NULL;
  cached_object   = NULL;
  signal_id       = 0;
}

PTN_FunctionSignalStatement::PTN_FunctionSignalStatement(
                                                         TKS_FunctionObject *_fun,
                                                         sU8                 _sigid,
                                                         TKS_CachedObject *  _co
                                                         )
{
   function_object = _fun;
   signal_id       = _sigid;
   cached_object   = _co;
}

PTN_FunctionSignalStatement::~PTN_FunctionSignalStatement() {
   function_object = NULL;
   signal_id       = 0;
   cached_object   = NULL;
}


sBool PTN_FunctionSignalStatement::semanticCheck(void) {
	if( (NULL == function_object) && (NULL == cached_object) ) 
   {
		return 1; // autobind mode 
   }
	return 
      (NULL != function_object) && 
      (NULL != cached_object)
      ;
}
 
static void PTN_FunctionSignalStatement__eval(PTN_Env *_env, const PTN_Statement *_st) {  
   Dtracest;
	const PTN_FunctionSignalStatement *st = (const PTN_FunctionSignalStatement*)_st; 

	if( (NULL == st->function_object) && (NULL == st->cached_object) )
	{
		// ---- auto bind signal handlers for all "static" YAC classes ----
		sU8 mi=(sU8) ((st->src_loc>>16) -1); // get module_index
		StaticList::Node *c=Dtkscompiler->compiled_modules;
		c=StaticList::Tail(c);
		if(c)
		{
			sUI i;
			for(i=0; c&&i<mi; i++) c=c->previous;
			if(c)
			{
				TKS_CachedScript *cs=(TKS_CachedScript*)c->data;
				PTN_Function *f;
				YAC_String t; 
				// ---- scan all classes  ---- 
				for(i=0; i<YAC_MAX_CLASSES; i++) 
				{ 
					TKS_ClassTemplate *clt = TKSCRIPT__ATOM_SPEEDPATH[i]; 
					if(clt) 
					{ 
						if(clt->may_instantiate==YAC_CLASSTYPE_STATIC) 
						{ 
							sSI j=clt->num_signals; 
							if(j) 
							{ 
								while(--j>=0) // ---- scan all signals ---- 
								{ 
									TKS_SignalEntry *sig=&clt->signals[j]; 
									f = cs->findFunction(&sig->name); 
									if(f != NULL)
									{ 
										if(tkscript->configuration.debug_level>63) 
                              {
                                 Dprintf("[...] autobind signal %s::%s\n", 
                                    (sChar*)clt->class_name.chars, (sChar*)sig->name.chars); 
                              }
										clt->template_object->yacRegisterSignal(j, (YAC_FunctionHandle) f->getOrCreateFunctionObject()); 
									}
                           else
                           {
                              //printf("[dbg] failed to auto-bind signal \"%s:%s\"\n", clt->class_name.chars, sig->name.chars);
                           }
								} 
							} 
						} 
					} 
				}
			}
		}
		return;
	} 

   // Bind function to signal slot in YAC_Object
   YAC_Value *co = Dgetvar(st->cached_object); 
   
   if(TKS_VALID(co->value.object_val))
   {
      co->value.object_val->yacRegisterSignal(st->signal_id, (YAC_FunctionHandle) st->function_object); 
   }
   else
   {
      Drtthrowinvalidpointer(_st, "cannot register signal handler for invalid c++ object", co->value.object_val);
   }
}

Fevalst PTN_FunctionSignalStatement::getEvalSt(void) const {
	return PTN_FunctionSignalStatement__eval;
}

void PTN_FunctionSignalStatement::eval(PTN_Env *_env) const {
	PTN_FunctionSignalStatement__eval(_env, this);
}

