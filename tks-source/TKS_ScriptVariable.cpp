/// TKS_ScriptVariable.cpp
///
/// (c) 2001-2006 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdlib.h>
#include <string.h>

#include "tks.h"
#include "tks_inc_compiler.h"
#include "YAC_Value.h"
#include "YAC_ValueObject.h"

#include "TKS_ScriptObjectID.h"
#include "TKS_ScriptVariable.h"


TKS_ScriptVariable::TKS_ScriptVariable(void) {
   var = 0;
}

TKS_ScriptVariable::~TKS_ScriptVariable() {
}

sBool TKS_ScriptVariable::isValid(void) {
   return script_id.isValid();
}

sSI TKS_ScriptVariable::query(void) {
   if(isValid())
   {
      if(value.any != var->value.any)
      {
         unsetFast();
         deleteme = 0;
         value.any = var->value.any;
      }
      return 1;
   }
   return 0; 
}

sSI TKS_ScriptVariable::store(void) {
   if(isValid())
   {
      if(value.any != var->value.any)
      {
         var->unsetFast();
         var->deleteme = 0;
         var->value.any = value.any;
         return 1;
      }
   }
   return 0; 
}

sSI TKS_ScriptVariable::storeDeref(void) {
   if(isValid())
   {
      if(value.any != var->value.any)
      {
         var->unsetFast();
         var->deleteme = deleteme;
         deleteme = 0;
         var->value.any = value.any;
         return 1;
      }
   }
   return 0;
} 

YAC_String *TKS_ScriptVariable::getName(void) {
   if(isValid()) 
   {
      return &var->name;
   }
   else 
   {
      return 0;
   }
}
