/// TKS_ScriptObjectID.cpp
///
/// (c) 2005-2008 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdlib.h>
#include <string.h>

#include "tks.h"
#include "tks_inc_compiler.h"
 
#include "TKS_ScriptObjectID.h"


TKS_ScriptObjectID::TKS_ScriptObjectID(void) {
   s_id.id    = TKS_SCRIPTID_ALWAYS_VALID;
   s_id.magic = TKS_SCRIPTID_ALWAYS_VALID;
}

sBool TKS_ScriptObjectID::isValid(void) {
   if( (TKS_SCRIPTID_ALWAYS_VALID == s_id.id) && (TKS_SCRIPTID_ALWAYS_VALID == s_id.magic) )
   {
      // objects that do *not* originate from temporary modules are always valid
      return 1;
   }
   else if(
      ((TKS_Compiler*)tkscript->compiler)->
      temp_modules[s_id.id].magic==s_id.magic
      ) 
   {
      return 1; 
   }
   else
   {
      Dprintf("[---] temporary script object %08x:%i is invalid (script already deleted).\n",
         s_id.magic, s_id.id);
      return 0;
   }
}
