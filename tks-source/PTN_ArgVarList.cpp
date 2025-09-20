/// PTN_ArgVarList.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_compiler.h"
#include "PTN_Node.h"
#include "PTN_ArgVar.h"
#include "PTN_ArgVarList.h"


PTN_ArgVarList::PTN_ArgVarList(void) {
   first_var   = NULL;
   num_vars    = 0u;
   cached_vars = NULL;
}

PTN_ArgVarList::~PTN_ArgVarList() {
   freeVarLinkedList(); // in case init() was never called

   num_vars = 0u;

   if(NULL != cached_vars)
   {
      delete [] cached_vars;
      cached_vars = NULL;
   }
}

void PTN_ArgVarList::freeVarLinkedList(void) {
   if(NULL != first_var)
   {
      delete first_var;
      first_var = NULL;
   }
}

void PTN_ArgVarList::addVar(TKS_CachedObject *_var) {

   if(_var->type >= YAC_TYPE_OBJECT)
   {
      _var->flags |= TKS_CachedObject::PTRASSIGN; // do not create object instance when creating a stackframe
   }

   PTN_ArgVar *c = first_var;
   PTN_ArgVar *nn;
   PTN_NEW_STATIC_NODE(nn, PTN_ArgVar)(_var);

   if(NULL != c)
   {
      while(NULL != c->next)
      {
         c = c->next;
      }

      c->next = nn;
   }
   else
   {
      first_var = nn;
   }

   num_vars++;
}

void PTN_ArgVarList::init(void) {
   if(num_vars > 0u)
   {
      // Create linear array of function hash entry references
      cached_vars = new TKS_CachedObject*[num_vars];
      PTN_ArgVar *c = first_var;
      sUI i = 0;

      while(NULL != c)
      {
         cached_vars[i++] = c->var;
         c = c->next;
      }

      // The single linked var list is not used any further
      freeVarLinkedList();
   }
}

YAC_Value *PTN_ArgVarList::getVar(TKS_Context *_context, sUI _id) {
   if(num_vars > _id)
   {
      return Dgetvarfrom(_context, cached_vars[_id]);
   }
   else
   {
      return NULL;
   }
}
