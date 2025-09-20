/// TKS_Exception.h 
/// 
/// (c) 2006-2009 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 
#ifndef __TKS_EXCEPTION_H__
#define __TKS_EXCEPTION_H__


class PTN_Node;
class TKS_CachedScript;
class TKS_FunctionCallStackEntry;
class TKS_ExceptionType;


class TKS_Exception : public YAC_Object {
public:
   YAC_String                 *message;      // Additional, user defined message or NULL
   const TKS_ExceptionType    *exception_type;
   const PTN_Node             *node;         // script source location or 0, where the exception occured
   const PTN_Node             *node_handled; // where the exception was first handled
   YAC_String                 *cpp_src_file; // name of the C++ source file if this exception was thrown within native code
   sUI                         cpp_src_line; // line number (1..) within the C++ source (if cpp_src_file!=NULL)
#ifdef DX_TRACEFUNCTIONCALLS 
   TKS_FunctionCallStackEntry *stack_trace;  // a snapshot of the callstack[]
   sUI                         stack_trace_num; // number of elements in stack_trace[]
#endif

public:
   TKS_Exception(void);
   TKS_Exception(const TKS_ExceptionType *);
   TKS_Exception(const TKS_ExceptionType *, const PTN_Node *);
   ~TKS_Exception();

   sSI         getId         (void) const; // return exception typeid
   const YAC_String *getName       (void) const; // return exception typename
   void        getFullName   (YAC_Value *_r); // return fully qualified name, e.g. "MyClass::MyException" or "::MyGlobalException"
   YAC_String *getMessage    (void); // return current message
   void        getStackTrace (YAC_Value *_r); // return stack trace string

   void printNameAndMessage (sSI _sid);
   void printStackTrace     (sSI _sid);
};


#endif // __TKS_EXCEPTION_H__
