/// TKS_Context.h
///
/// (c) 2006-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_CONTEXT_H__
#define __TKS_CONTEXT_H__

#define PTN_MAX_CLASS_STACK 256

class PTN_TryCatchFinally;

#include <setjmp.h>

class TKS_Context {
public:
   TKS_Context(void);
   ~TKS_Context();

public:

   sBool b_running; // 1=eval statements, 0=exit 

   // ---- runtime class stack ----
   TKS_ScriptClassInstance *class_stack[PTN_MAX_CLASS_STACK+1];
   
   TKS_ScriptClassInstance**tksvm_class_stack; // == class_stack
   TKS_ScriptClassInstance *tksvm_class_stack_object;
   sUI                      tksvm_class_stack_index;

   YAC_Value               *tks_fstack;  // holds configuration.function_stack_size values
   sSI                      tks_fstacki;
   sSI                      stacki_displace;// = 0;
   YAC_Value               *tks_fstackc; // (start of) current function stackframe

   void use(void); // re-init stack pointer, reset current exception etc

   YAC_Object *tksvm_indirect_object; // for indirect ecalls

   // (note) added for vst2scan.tks utility
   sBool b_thread;  // 1=context belongs to TKS_Thread instance
   jmp_buf jmp_thread_exit;


#ifdef DX_TRACEFUNCTIONCALLS 
   TKS_FunctionCallStackEntry  *tks_fcallstack; 
   sSI                          tks_fcallstacki; 
#endif 

   void             pushClassStack  (YAC_Object *_co);
   void             popClassStack   (void);
   void             clearClassStack (void);
    
   // ---- runtime function localvar stack 
   void             initFunctionStack (void); 
   void             freeFunctionStack (void); 

   YAC_Value *pushStackFrame            (PTN_Function *_f); // alloc new stackframe, return pointer
   void       pushStackFrame2           (PTN_Function *_f); // really change tks_fstackc
   void       popStackFrame             (PTN_Function *_f); 

   // ---- switch cmpvalue stack
   YAC_Value  switch_stack[TKS_SWITCH_STACK_SIZE];
   sSI        switch_stacki;

   void       initSwitchStack (void);
   void       freeSwitchStack (void);
   YAC_Value *pushSwitchValue (void);
   void       popSwitchValue  (void);

   // ---- break flag stack
   sBool      break_stack[TKS_BREAK_STACK_SIZE];
   sSI        break_stacki;

   void       initBreakStack (void);
   void       freeBreakStack (void);
   sBool     *pushBreak      (void);
   void       popBreak       (void);

  // ---- function call stack
   
#ifdef DX_TRACEFUNCTIONCALLS 
   // ---- runtime functioncall stack (used for backtrace) 
   void             initFunctionCallStack  (void); 
   void             freeFunctionCallStack  (void); 
   void             pushFunctionCall       (const PTN_Function *_calledFunction, sUI _srcLoc); 
   void             pushFunctionCallY      (YAC_Object *_object, const YAC_CommandY *_commandY, sUI _srcLoc); 
   inline void      popFunctionCall        (void) { tks_fcallstacki++; } 
   inline void      popFunctionCallY       (void) { tks_fcallstacki++; } 
   void             printFunctionCallStack (void); 
   void             copyFunctionCallStack  (TKS_FunctionCallStackEntry**_entryReturn, sUI *_numReturn);
#endif 

   TKS_CachedObject     current_exception_var;  // value.object_val != NULL if exception was raised
   TKS_CachedObject    *saved_exception;        // required for re-throwing an exception, see PTN_TryCatchFinally, PTN_Throw
   sBool                b_exception_raised;     // 1 indicates that a new exception has been raised
   sBool                b_exception_handled;    // 1 indicates that the exception ought to be passed to next upper frame
   const PTN_Node      *exception_default_node; // used for native calls without a direct node context
   const PTN_TryCatchFinally *last_try;         // used to decide whether to discard uncriticalerrors or not
   void               discardUncriticalError     (void); // discard uncriticalerror if not caught by current_try resp. parents
   void               raiseExceptionByTypeAndMsg (const PTN_Node *_nodeOrNull, const TKS_ExceptionType *_t, YAC_String *_s);
   void               handleException            (const PTN_Node *_nodeOrNull);
   void               printException             (void);
   void               raiseRuntimeException      (const PTN_Node *_nodeOrNull, sUI _id, const char *_msg, const char*_file, sSI _line);
   void               raiseRuntimeException      (const PTN_Node *_nodeOrNull, sUI _id, YAC_String *_msg, const char*_file, sSI _line);
   void               raiseInvalidPointer        (const PTN_Node *_nodeOrNull, const char *_msg, const char*_file, sSI _line, YAC_Object *_o);
   void               reRaiseException           (void);
   void               raiseException             (sUI _id, const char *_message, const char *_file, sSI _line, const PTN_Node *_node);
   void               setCurrentException        (TKS_Exception *_e);

   // returns YAC_TRUE when jmp target/env was set, YAC_FALSE when target was reached via longJmpThreadExit() (fatal error occured)
   sBool setJmpThreadExit (void);

   // called when a fatal error occured and the thread must be terminated immediately (w/o cleaning up)
   void longJmpThreadExit (void);

   // ---- context name (mainly for debugging purposes) ----
   YAC_String name;

#ifdef TKS_TRACE_NODES
   // Src location of last seen AST node
   sUI last_seen_srcloc;
#endif
};


#endif // __TKS_CONTEXT_H__
