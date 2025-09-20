/// TKS_ActiveNamespace.cpp
///
/// (c) 2008-2009 Bastian Spiegel <bs@tkscript.de>
///    - distributed under terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"


TKS_FreezedNamespaceArray TKS_ActiveNamespace::freeze(void) const {
   //
   // Convert list of namespace to NULL terminated array
   //

   // Count number of entries
   sUI num = 0;
   const TKS_ActiveNamespace *a = this;
   while(a)
   {
      num++;
      a = a->next;
   }

   // Allocate array and convert list
   a = this;
   TKS_FreezedNamespaceArray f = new TKS_Namespace*[num + 1];
   sUI i = 0;
   while(a)
   {
      f[i++] = a->nspace;
      a = a->next;
   }
   f[i] = NULL;

   return f;
}
