/// TKS_FunctionCallStackEntry.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "tks_inc_class.h"
#include "tks_inc_compiler.h"

#include "TKS_FunctionCallStackEntry.h"


void TKS_FunctionCallStackEntry::init(TKS_FunctionCallStackEntry *_src) {
   function    = _src->function;
   commandY    = _src->commandY;
   yclass_name = _src->yclass_name;
   src_loc     = _src->src_loc;
}

void TKS_FunctionCallStackEntry::print(sSI _line, sSI _sid) {
   (void)_sid;

   if(NULL != function)
   {
      // ---- script function / method
      const char *callerModName = Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc));
      printf("[---] %6i.: \"%s\":%u: %s::%s() (in \"%s\":%u)\n",
             _line,
             callerModName,
             Dsrcloc_linenr(src_loc),
             function->parent_class?((char*)function->parent_class->name.chars):"",
             (char*)function->name.chars,
             Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(function->src_loc)),
             Dsrcloc_linenr(function->src_loc)
             );
   }
   else
   {
      // ---- YAC/C++ function / method
      printf("[---] %6i.: \"%s\":%u: <native> %s::%s()\n",
             _line,
             Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
             Dsrcloc_linenr(src_loc),
             (char*)yclass_name,
             (char*)commandY->y_name
             );
   }
}

void TKS_FunctionCallStackEntry::appendString(YAC_String *_s, sSI _line) {
   YAC_String t; t.alloc(32);
   t.printf("%6i", _line);
   _s->append(&t);
   _s->append(".: \"");
   _s->append(Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)));
   _s->append("\":");
   t.printf("%i", Dsrcloc_linenr(src_loc));
   _s->append(&t);
   _s->append(": ");

   if(NULL != function)
   {
      // ---- script function / method
      if(NULL != function->parent_class)
      {
         _s->append(&function->parent_class->name);
      }
      _s->append("::");
      _s->append(&function->name);
      _s->append("() (in \"");
      _s->append(Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(function->src_loc)));
      _s->append("\":");
      t.printf("%i", Dsrcloc_linenr(function->src_loc));
      _s->append(&t);
      _s->append(")\n");
   }
   else
   {
      // ---- YAC/C++ function / method
      _s->append("<native> ");
      _s->append(yclass_name);
      _s->append("::");
      _s->append((char*)commandY->y_name);
      _s->append("()\n");
   }
}
