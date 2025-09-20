/// TKS_ScriptClassArrayIterator.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "tks_inc_class.h"
#include "YAC_ClassArray.h"
#include "tks_inc_compiler.h"
 
#include "TKS_ScriptClassArrayIterator.h" 


TKS_ScriptClassArrayIterator::TKS_ScriptClassArrayIterator(const YAC_ClassArray*_a) {
   array         = _a;
   current_index = 0;
}

TKS_ScriptClassArrayIterator::~TKS_ScriptClassArrayIterator() {
   array         = NULL;
   current_index = 0;
}

void YAC_VCALL TKS_ScriptClassArrayIterator::getNext(YAC_Value*r) {
   if(current_index<array->num_elements) 
   {
      r->initObject(array->elements[current_index++], 0); 
   }
   else
   {
      r->type      = 0xFF;
      r->value.any = NULL;
      r->deleteme  = 0;
   }
}
