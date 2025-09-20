/// TKS_Exception.cpp
///
/// (c) 2006-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "TKS_FunctionCallStackEntry.h"
#include "PTN_Node.h"
#include "tks_inc_compiler.h"
#include "tks_inc_class.h"

#include "TKS_ExceptionType.h"
#include "TKS_Exception.h"


TKS_Exception::TKS_Exception(void) {
   class_ID        = TKS_CLID_EXCEPTION;
   exception_type  = NULL;
   node            = NULL;
   node_handled    = NULL;
   cpp_src_file    = NULL;
   cpp_src_line    = 0u;
   message         = NULL;
#ifdef DX_TRACEFUNCTIONCALLS
   stack_trace     = NULL;
   stack_trace_num = 0u;
#endif
}

TKS_Exception::TKS_Exception(const TKS_ExceptionType *_t, const PTN_Node *_node) {
   class_ID        = TKS_CLID_EXCEPTION;
   exception_type  = _t;
   node            = _node;
   node_handled    = NULL;
   cpp_src_file    = NULL;
   cpp_src_line    = 0u;
   message         = NULL;
#ifdef DX_TRACEFUNCTIONCALLS
   stack_trace     = NULL;
   stack_trace_num = 0u;
#endif
}

TKS_Exception::TKS_Exception(const TKS_ExceptionType *_t) {
   class_ID        = TKS_CLID_EXCEPTION;
   exception_type  = _t;
   node            = NULL;
   node_handled    = NULL;
   cpp_src_file    = NULL;
   cpp_src_line    = 0u;
   message         = NULL;
#ifdef DX_TRACEFUNCTIONCALLS
   stack_trace     = NULL;
   stack_trace_num = 0u;
#endif
}

TKS_Exception::~TKS_Exception() {
   YAC_DELETE_SAFE(cpp_src_file);
   YAC_DELETE_SAFE(message);

#ifdef DX_TRACEFUNCTIONCALLS
   if(NULL != stack_trace)
   {
      delete [] stack_trace;
      stack_trace = NULL;
   }
#endif
}

void TKS_Exception::printNameAndMessage(sSI _sid) {

   (void)_sid;

   tkscript->printf("[---] exception name=\"%s\"", (char*)exception_type->name.chars);
   if(NULL != message)
   {
      tkscript->printf(". message=\"%s\"", (char*)message->chars);
   }
   tkscript->printf(".\n");

   /*if(cpp_src_file != NULL)
   {
      tkscript->printf("[---] thrown in C++ source \"%s\" line %i.\n",
         (char*)cpp_src_file->chars,
         cpp_src_line
         );
   }
   if(node != NULL)
   {
      sUI src_loc = node->src_loc;
      if(cpp_src_file != NULL)
      {
         tkscript->printf("[---] called in script module");
      }
      else
      {
         tkscript->printf("[---] thrown in script module");
      }

      tkscript->printf(" \"%s\" line %i.\n",
         Dtkscompiler->getModuleNameByIndex((src_loc>>16)&0xFFFF),
         (src_loc&0xFFFF)+1
         );
   }
   */
}

void TKS_Exception::printStackTrace(sSI _sid) {
#ifdef DX_TRACEFUNCTIONCALLS

   tkscript->printf("[---] Runtime callstack:\n");

   sSI j = 1;
   if(NULL != cpp_src_file)
   {
      tkscript->printf("[---]      1.: <native> \"%s\":%i: <throw>\n",
         (char*)cpp_src_file->chars,
         cpp_src_line
         );
      j++;
   }

   if(NULL != node)
   {
      sUI src_loc = node->src_loc;
      if(NULL != cpp_src_file)
      {
         // Called from script module
         tkscript->printf("[---]      2.: \"%s\":%i: <call>\n",
                          Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                          Dsrcloc_linenr(src_loc)
                          );
         j++;
      }
      else
      {
         // Thrown in script module
         tkscript->printf("[---]      1.: \"%s\":%i: <throw>\n",
                          Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                          Dsrcloc_linenr(src_loc)
                          );
         j++;
      }

   }

   if(NULL != stack_trace)
   {
      TKS_FunctionCallStackEntry *fcse = stack_trace;
      for(sUI i=0; i<stack_trace_num; i++)
      {
         fcse++->print(j++, _sid);
      }
   }
#else
   (void)_sid;
#endif // DX_TRACEFUNCTIONCALLS
}

sSI TKS_Exception::getId(void) const {
   return exception_type->exception_id;
}

const YAC_String *TKS_Exception::getName(void) const {
   return &exception_type->name;
}

void TKS_Exception::getFullName(YAC_Value *_r) {
   YAC_String *r = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   if(NULL != exception_type->parent_class)
   {
      r->yacCopy(&exception_type->parent_class->name);
      r->append("::");
      r->append(&exception_type->name);
   }
   else
   {
      r->copy("::");
      r->append(&exception_type->name);
   }
   _r->initString(r, 1);
}

YAC_String *TKS_Exception::getMessage(void) {
   return message;
}

void TKS_Exception::getStackTrace(YAC_Value *_r) {
#ifdef DX_TRACEFUNCTIONCALLS
   _r->initEmptyString();

   YAC_String *s = _r->value.string_val;
   YAC_String t;
   t.alloc(32);
   sSI j = 1;

   if(NULL != node_handled)
   {
      sUI src_loc = node_handled->src_loc;
      t.printf("     0.: \"%s\":%i",
               Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
               Dsrcloc_linenr(src_loc)
               );
      s->append(&t);
      s->append(": <handled>\n");
   }

   if(NULL != cpp_src_file)
   {
      s->append("     1.: <native> \"");
      s->append(cpp_src_file);
      s->append("\":");
      t.printf("%i", cpp_src_line);
      s->append(&t);
      s->append(": <throw>\n");
      j++;
   }

   if(NULL != node)
   {
      sUI src_loc = node->src_loc;
      if(cpp_src_file != NULL)
      {
         // Called from script module
         s->append("     2.: \""); //%s\":%i.\n",
         s->append(Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)));
         s->append("\":");
         t.printf("%i", Dsrcloc_linenr(src_loc));
         s->append(&t);
         s->append(": <call>\n");
         j++;
      }
      else
      {
         // Thrown in script module
         s->append("     1.: \"");
         s->append(Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)));
         s->append("\":");
         t.printf("%i", Dsrcloc_linenr(src_loc));
         s->append(&t);
         s->append(": <throw>\n");
         j++;
      }

   }

   if(NULL != stack_trace)
   {
      TKS_FunctionCallStackEntry *fcse = stack_trace;
      for(sUI i = 0u; i < stack_trace_num; i++)
      {
         fcse++->appendString(s, j++);
      }
   }
#else
   (void)_r;
#endif // DX_TRACEFUNCTIONCALLS
}
