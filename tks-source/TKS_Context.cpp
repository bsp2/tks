/// TKS_Context.cpp
///
/// (c) 2006-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"

#include "tks.h"

#ifndef YAC_WIN32
#include <sys/time.h>
#endif

#ifdef YAC_POSIX
#include <unistd.h>
#endif

#ifdef YAC_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef DX_PTHREADS
#include <pthread.h>
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

#include "PTN_TryCatchFinally.h"

#include "Pool.h"

#ifdef DX_TRACEFUNCTIONCALLS
#include "TKS_FunctionCallStackEntry.h"
#endif


TKS_Context::TKS_Context(void) {
   b_running               = YAC_FALSE;
   b_exception_raised      = YAC_FALSE;
   b_exception_handled     = YAC_FALSE;
   exception_default_node  = NULL;
   current_exception_var.unset();
   saved_exception         = NULL;
   tksvm_indirect_object   = NULL;
   tks_fstack              = NULL;
   tks_fstacki             = tkscript->configuration.function_stack_size;
   stacki_displace         = 0;
   tks_fstackc             = NULL;
#ifdef DX_TRACEFUNCTIONCALLS
   tks_fcallstack          = NULL;
   tks_fcallstacki         = 0;
#endif // DX_TRACEFUNCTIONCALLS
   tksvm_class_stack_index = 0;
   tksvm_class_stack_object= NULL;
#ifdef TKS_TRACE_NODES
   last_seen_srcloc        = 0u;
#endif
   last_try                = NULL;
   b_thread                = YAC_FALSE;

   clearClassStack();

   initFunctionStack();

#ifdef DX_TRACEFUNCTIONCALLS
   initFunctionCallStack();
#endif // DX_TRACEFUNCTIONCALLS

   initSwitchStack();
   initBreakStack();
}

TKS_Context::~TKS_Context() {
   setCurrentException(NULL);
   current_exception_var.unset();
   saved_exception = NULL;

   freeFunctionStack();
#ifdef DX_TRACEFUNCTIONCALLS
   freeFunctionCallStack();
#endif // DX_TRACEFUNCTIONCALLS
   freeSwitchStack();
   freeBreakStack();
}

void TKS_Context::use(void) {
   b_exception_raised      = YAC_FALSE;
   b_exception_handled     = YAC_FALSE;
   exception_default_node  = NULL;
   current_exception_var.unset();
   tks_fstackc             = &tks_fstack[tkscript->configuration.function_stack_size];
#ifdef DX_TRACEFUNCTIONCALLS
   tks_fcallstacki         = tkscript->configuration.functioncall_stack_size;
#endif // DX_TRACEFUNCTIONCALLS
#ifdef TKS_TRACE_NODES
   last_seen_srcloc        = 0u;
#endif
   stacki_displace         = 0;
   tksvm_class_stack_index = PTN_MAX_CLASS_STACK;
   last_try                = NULL;
}

void TKS_Context::clearClassStack(void) {
   for(sUI i = 0u; i < (PTN_MAX_CLASS_STACK + 1u); i++)
   {
      class_stack[i] = 0;
   }
   tksvm_class_stack_index = PTN_MAX_CLASS_STACK;
   tksvm_class_stack       = class_stack;
}

void TKS_Context::pushClassStack(YAC_Object *_o) {
#ifdef DX_SAFEMODE
   if(tksvm_class_stack_index > 0)
   {
#endif
      tksvm_class_stack_object = class_stack[--tksvm_class_stack_index]=(TKS_ScriptClassInstance*)_o;
      // // *tks_this_adr = tksvm_class_stack_object; // lil trick for statements that assign to this, call this etc.
#ifdef DX_SAFEMODE
   }
   else
   {
      tkscript->printf("\n[---] class stack overflow (push).\n");
      b_running = YAC_FALSE;
      tkscript->handleFatalError();
      tksvm_class_stack_object = NULL;
   }
#endif
}

void TKS_Context::popClassStack(void) {
#ifdef DX_SAFEMODE
   if(PTN_MAX_CLASS_STACK != tksvm_class_stack_index)
   {
#endif
      tksvm_class_stack_index++;
      if(PTN_MAX_CLASS_STACK != tksvm_class_stack_index)
      {
         tksvm_class_stack_object = (TKS_ScriptClassInstance*) class_stack[tksvm_class_stack_index];
      }
#ifdef DX_SAFEMODE
      else
      {
         tksvm_class_stack_object = NULL;
      }
#endif
#ifdef DX_SAFEMODE
   }
   else
   {
      tkscript->printf("\n[---] class stack overflow (pop).\n");
      b_running = YAC_FALSE;
      tkscript->handleFatalError();
   }
#endif
}

YAC_Value *TKS_Context::pushStackFrame(const PTN_Function *_f) {
   // (note) caller must ensure that function has local vars (_f->num_local_vars > 0u)
   // (note) caller uses current this->tks_fstackc if it has no local vars
   // (note) caller must restore this->tks_fstackc after popStackFrame()
   const sUI numLocalVars = _f->num_local_vars;

#define Dmethodnameargs (NULL != _f->parent_class) ? (const char*)_f->parent_class->name.chars : NULL, (NULL != _f->parent_class) ? "::":"", (const char*)_f->name.chars
   // Dyac_host_printf("xxx pushStackFrame f->name=\"%s%s%s\"\n", Dmethodnameargs);
   // Dyac_host_printf("xxx TKS_Context::pushStackFrame: tks_fstacki=%d numLocalVars=%u stacki_displace=%d\n", tks_fstacki, numLocalVars, stacki_displace);

   if( (sSI(tks_fstacki) - sSI(numLocalVars) + stacki_displace) >= sSI(numLocalVars) )
   {
      YAC_Value *nsfc = &tks_fstack[tks_fstacki - numLocalVars + stacki_displace];
      ::memcpy((void*)nsfc, (const void*)_f->nsf_template, _f->nsf_template_sz);  // copy types + zero/NULL values

      const sUI numDuplicate = _f->nsf_template_num_duplicate_indices;
      const sUI *duplicateLUT = _f->nsf_template_duplicate_indices;
      sUI duplicateIdx = 0u;

      while(duplicateIdx < numDuplicate)
      {
         // "local Double d;"
         // Dyac_host_printf("xxx nsfc duplicate object %s%s%s\n", Dmethodnameargs);
         const sUI i = duplicateLUT[duplicateIdx++];
         const YAC_Value *cp = &_f->opt_local_vars[i];
         nsfc[i].duplicate(this, cp);
      }

      return nsfc;
   }
   else
   {
      Dprintf("[---] Function::pushStackFrame: recursion depth (%i) exceeded.\n", tkscript->configuration.function_stack_size);
      b_running = YAC_FALSE;
      tkscript->handleFatalError();
      return NULL;
   }
}

void TKS_Context::pushStackFrame2(const PTN_Function *_f) {
   tks_fstacki -= _f->num_local_vars;
   tks_fstackc  = &tks_fstack[tks_fstacki + stacki_displace];
}

void TKS_Context::popStackFrame(const PTN_Function *_f) {
   // (?) optimize, only unset() object vars (but: "local var x;" ?)
   sUI i = 0u;
   YAC_Value *v = tks_fstackc;
   sUI numLocalVars = _f->num_local_vars;
   while(i < numLocalVars)
   {
      v[i].unset();
      i++;
   }
   tks_fstacki += numLocalVars;
}

void TKS_Context::discardUncriticalError(void) {
   if(b_exception_raised)
   {
      TKS_Exception *e = (TKS_Exception *)current_exception_var.value.object_val;
      if(tkscript->exception_typecast_map[e->exception_type->exception_id][TKS_EXCEPTION_UNCRITICALERROR])
      {
         sBool bCanCatch = YAC_FALSE;
         if(NULL != last_try)
         {
            bCanCatch = last_try->canCatchType(e->exception_type->exception_id);
         }
         if(!bCanCatch)
         {
            b_exception_raised = YAC_FALSE;
            b_running = YAC_TRUE;
            current_exception_var.unset();
         }
      }
   }
}

void TKS_Context::raiseException(sUI _id, const char *_message, const char *_file, sSI _line, const PTN_Node *_node) {

   // Stop script execution
   b_running = YAC_FALSE;

   TKS_ExceptionType *t = tkscript->getExceptionTypeById(_id);
   if(NULL != t)
   {
      TKS_Exception *e = new TKS_Exception( t, _node );

      if(NULL != _message)
      {
         YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
         s->copy(_message);
         e->message = s;
      }

      if(NULL != _file)
      {
         YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
         s->copy(_file);
         e->cpp_src_file = s;
         e->cpp_src_line = _line;
      }

#ifdef DX_TRACEFUNCTIONCALLS
      copyFunctionCallStack(&e->stack_trace, &e->stack_trace_num);
#endif

      setCurrentException(e);
   }
   else
   {
      Dprintf("[---] failed to raise unknown exception (id=%i). Exiting..\n", _id);
   }
}

void TKS_Context::setCurrentException(TKS_Exception *_e) {
   current_exception_var.unsetFast();
   if(NULL != _e)
   {
      // If the origin node has not been set already then use the "last seen" default node
      if(NULL == _e->node)
      {
         _e->node = exception_default_node;
      }

      // Signalize that a new exception is raised
      b_exception_raised  = YAC_TRUE;
      b_exception_handled = YAC_FALSE;

      // Stop script execution
      b_running = YAC_FALSE;
      current_exception_var.initObject(_e, 1);
   }
   else
   {
      current_exception_var.initObject(NULL, 0);
   }
}

void TKS_Context::reRaiseException(void) {
   b_exception_raised  = YAC_TRUE;
   b_exception_handled = YAC_FALSE;
   b_running           = YAC_FALSE;
   current_exception_var.copySafe(saved_exception);
}

void TKS_Context::raiseExceptionByTypeAndMsg(const PTN_Node *_nodeOrNull, const TKS_ExceptionType *_t, YAC_String *_msg) {
   TKS_Exception *e = new TKS_Exception(_t);

   e->message = _msg;
   e->node    = _nodeOrNull;

#ifdef DX_TRACEFUNCTIONCALLS
   copyFunctionCallStack(&e->stack_trace, &e->stack_trace_num);
#endif

   setCurrentException(e);
}

void TKS_Context::raiseInvalidPointer(const PTN_Node *_nodeOrNull,
                                      const char     *_msg,
                                      const char     *_file,
                                      sSI             _line,
                                      YAC_Object     *_o
                                      ) {
   YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   msg->copy(_msg);
#ifdef TKS_ALLOWNODELETEOBJECTS

   if(!tkscript->configuration.b_enable_delete_objects)
   {
      if(_o && (YAC_CLID_OBJECT != _o->class_ID) ) // not NULL or null ?
      {
         // Add object class type name to msg
         msg->append(" (className=\"");
         msg->append(_o->yacClassName());
         msg->append("\", metaClassName=\"");
         msg->append(_o->yacMetaClassName());
         msg->append("\")");
      }
      else
      {
         msg->append(" (<null>)");
      }

   }
#endif // TKS_ALLOWNODELETEOBJECTS

   raiseRuntimeException(_nodeOrNull, TKS_EXCEPTION_INVALIDPOINTER, msg, _file, _line);
}

void TKS_Context::raiseRuntimeException(const PTN_Node *_nodeOrNull, sUI _id, const char *_msg, const char*_file, sSI _line) {
   // yac_host->printf("xxx TKS_Context::raiseRuntimeException: b_thread=%d tkscript->configuration.b_thread_terminate_on_exception=%d\n", b_thread, tkscript->configuration.b_thread_terminate_on_exception);
   if(b_thread && tkscript->configuration.b_thread_terminate_on_exception)
   {
      // Fatal error, terminate thread immediately
      longJmpThreadExit();
   }
   else
   {
      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
      msg->copy(_msg);
      raiseRuntimeException(_nodeOrNull, _id, msg, _file, _line);
   }
}

void TKS_Context::raiseRuntimeException(const PTN_Node *_nodeOrNull, sUI _id, YAC_String *_msg, const char*_file, sSI _line) {

   if(b_thread && tkscript->configuration.b_thread_terminate_on_exception)
   {
      // Fatal error, terminate thread immediately
      longJmpThreadExit();
   }
   else
   {
      raiseExceptionByTypeAndMsg(_nodeOrNull, tkscript->getExceptionTypeById(_id), _msg);

      if(NULL != _file)
      {
         YAC_String *f = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
         f->copy(_file);
         TKS_Exception *e = (TKS_Exception *) current_exception_var.value.object_val;
         e->cpp_src_file = f;
         e->cpp_src_line = (sUI)_line;
      }
   }
}

void TKS_Context::handleException(const PTN_Node *_nodeOrNull) {
   // Do not reset b_exception_raised so a catch{} has a chance to take care of the exception

   if(!b_exception_handled)
   {
      b_exception_handled = YAC_TRUE;

      // Remember where the exception was handled
      TKS_Exception *e = (TKS_Exception *) current_exception_var.value.object_val;
      e->node_handled = _nodeOrNull;

#ifdef TKS_PRINTEXCEPTION
      printException();
#endif // TKS_PRINTEXCEPTION
   }
}

void TKS_Context::printException(void) {
#ifdef TKS_DCON
   TKS_Exception *e = (TKS_Exception *) current_exception_var.value.object_val;
   if(NULL != e)
   {
      sBool bCritical = tkscript->exception_typecast_map[e->exception_type->exception_id][TKS_EXCEPTION_CRITICALERROR];

      sBool bVerbose = bCritical || (tkscript->configuration.debug_level >= 20) ;

      if(bVerbose)
      {
         Dprintf("[---] caught %s exception \"%s\"", (bCritical?"critical":"uncritical"), (char*)e->exception_type->name.chars);
         if(NULL != e->message)
         {
            Dprintf(", message=\"%s\"\n", (char*)e->message->chars);
         }
         else
         {
            Dprintf("\n");
         }

         // // always print critical stack traces if(tkscript->configuration.debug_level>=10) // Print exception details if running with "-d 10"
         if(NULL != e->node_handled)
         {
            sUI src_loc = e->node_handled->src_loc;
            Dprintf("[---] in module \"%s\" line %i",
                    Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                    Dsrcloc_linenr(src_loc)
                    );
         }
         Dprintf(".\n");
         // Print exception details (origin, message, stacktrace)
         e->printStackTrace(0);
      }
   }
#endif // TKS_DCON
}

void TKS_Context::initFunctionStack(void) {
   // Alloc stack, skip c'tors, init void type (0)
   tks_fstack = (YAC_Value*) ::malloc(sizeof(YAC_Value) * tkscript->configuration.function_stack_size);
   ::memset((void*)tks_fstack, 0, sizeof(YAC_Value) * tkscript->configuration.function_stack_size);
   tks_fstackc = &tks_fstack[tkscript->configuration.function_stack_size];
}

void TKS_Context::freeFunctionStack(void) {
   // Clean up function stack frames
   if(tks_fstack)
   {
      if(sUI(tks_fstacki) < tkscript->configuration.function_stack_size)
      {
         Dprintf("[...] cleaning up %i objects\n", (tkscript->configuration.function_stack_size - tks_fstacki));
         sUI i = tks_fstacki;
         while(i < tkscript->configuration.function_stack_size)
         {
            tks_fstack[i++].unsetFast();
         }
      }
      ::free(tks_fstack);
      tks_fstack = NULL;
   }
}

void TKS_Context::initSwitchStack(void) {
   switch_stacki = TKS_SWITCH_STACK_SIZE;
}

void TKS_Context::freeSwitchStack(void) {
   sSI i = (switch_stacki >= 0) ? switch_stacki : 0;
   while(i < TKS_SWITCH_STACK_SIZE)
   {
      switch_stack[i++].unset();
   }
}

YAC_Value *TKS_Context::pushSwitchValue(void) {
   if(switch_stacki > 0)
   {
      return &switch_stack[--switch_stacki];
   }
   else
   {
      Dprintf("[---] switch stack overflow (push).\n");
      b_running = YAC_FALSE;
      tkscript->handleFatalError();
      return NULL;
   }
}

void TKS_Context::popSwitchValue(void) {
   if(switch_stacki < TKS_SWITCH_STACK_SIZE)
   {
      switch_stacki++;
   }
   else
   {
      Dprintf("[---] switch stack overflow (pop).\n");
      b_running = YAC_FALSE;
      tkscript->handleFatalError();
   }
}


void TKS_Context::initBreakStack(void) {
   break_stacki = TKS_BREAK_STACK_SIZE;
}

void TKS_Context::freeBreakStack(void) {
}

sBool *TKS_Context::pushBreak(void) {
   if(break_stacki > 0)
   {
      sBool *bBreak = &break_stack[--break_stacki];
      *bBreak = YAC_FALSE;
      return bBreak;
   }
   else
   {
      Dprintf("[---] break stack overflow (push).\n");
      b_running = YAC_FALSE;
      tkscript->handleFatalError();
      return NULL;
   }
}

void TKS_Context::popBreak(void) {
   if(break_stacki < TKS_BREAK_STACK_SIZE)
   {
      break_stacki++;
   }
   else
   {
      Dprintf("[---] break stack overflow (pop).\n");
      b_running = YAC_FALSE;
      tkscript->handleFatalError();
   }
}


#ifdef DX_TRACEFUNCTIONCALLS

void TKS_Context::initFunctionCallStack(void) {
   freeFunctionCallStack();
   tks_fcallstack = new TKS_FunctionCallStackEntry[tkscript->configuration.functioncall_stack_size];
   tks_fcallstacki = tkscript->configuration.functioncall_stack_size;
}

void TKS_Context::freeFunctionCallStack(void) {
   if(tks_fcallstack)
   {
      delete [] tks_fcallstack;
      tks_fcallstack = NULL;
   }
}

void TKS_Context::pushFunctionCall(const PTN_Function *_calledFunction, sUI _srcLoc) {
   TKS_FunctionCallStackEntry *fcse = &tks_fcallstack[--tks_fcallstacki];
   fcse->function   = _calledFunction;
   fcse->commandY   = NULL;
   fcse->yclass_name= NULL;
   fcse->src_loc    = _srcLoc;
   // xxx check function call stack size
}

void TKS_Context::pushFunctionCallY(YAC_Object *_o, const YAC_CommandY *_cmd, sUI _srcLoc) {
   TKS_FunctionCallStackEntry *fcse = &tks_fcallstack[--tks_fcallstacki];
   fcse->function   = NULL;
   fcse->commandY   = _cmd;
   fcse->yclass_name= _o ? _o->yacClassName() : "";
   fcse->src_loc    = _srcLoc;
   // xxx check function call stack size
}

void TKS_Context::printFunctionCallStack(void) {
#ifdef TKS_DCON
   sUI i = (sUI)tks_fcallstacki;
   sSI j = 1;
   if(sUI(tks_fcallstacki) < tkscript->configuration.functioncall_stack_size)
   {
      Dprintf("[---] Runtime callstack:\n");
      while(i < tkscript->configuration.functioncall_stack_size)
      {
         TKS_FunctionCallStackEntry *fcse = &tks_fcallstack[i];
         fcse->print(j, 0);
         j++;
         i++;
      }
#ifdef TKS_TRACE_NODES
      if(0u != last_seen_srcloc)
      {
         Dprintf("[---] Last seen AST node was %s:%i.\n",
                 Dtkscompiler->getModuleNameByIndex((last_seen_srcloc>>16)&0xFFFF),
                 (last_seen_srcloc&0xFFFF)+1
                 );

      }
#endif // TKS_TRACE_NODES
   }
#endif
}

void TKS_Context::copyFunctionCallStack(TKS_FunctionCallStackEntry**_entryReturn, sUI *_numReturn) {
   sSI num = sSI(tkscript->configuration.functioncall_stack_size) - tks_fcallstacki;
   if(num >= 0)
   {
      TKS_FunctionCallStackEntry *e = new TKS_FunctionCallStackEntry[num];
      *_entryReturn = e;
      *_numReturn   = num;
      sSI i = 0, j = tks_fcallstacki;
      for(; i < num; i++, j++)
      {
         e[i].init(&tks_fcallstack[j]);
      }
   }
   else
   {
      *_entryReturn = NULL;
      *_numReturn   = 0u;
   }
}

#endif // DX_TRACEFUNCTIONCALLS

sBool TKS_Context::setJmpThreadExit(void) {
   return (0 == setjmp(jmp_thread_exit));
}

void TKS_Context::longJmpThreadExit(void) {
   longjmp(jmp_thread_exit, YAC_FALSE);
}
