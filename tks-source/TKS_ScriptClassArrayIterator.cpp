/// TKS_ScriptClassArrayIterator.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
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
