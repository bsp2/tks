/// PTN_ArgList.cpp
/// 
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_compiler.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_ArgListNode.h"
#include "PTN_ArgList.h"


PTN_ArgList::PTN_ArgList(void) { 
   first_arg = NULL; 
   num_args  = 0; 
} 

PTN_ArgList::~PTN_ArgList() { 
   if(first_arg) 
   { 
      delete first_arg;
      first_arg = NULL;
      num_args  = 0; 
   } 
} 

void PTN_ArgList::addArg(PTN_Expr *_expr) { 
   PTN_ArgListNode *nn;
   PTN_NEW_STATIC_NODE(nn, PTN_ArgListNode)(_expr); 
   PTN_ArgListNode *c = first_arg; 

   if(NULL != c) 
   { 
      while(NULL != c->next)
      {
         c = c->next;
      }

      c->next = nn; 
      nn->previous = c; 
   } 
   else 
   { 
      first_arg = nn;
   } 

   num_args++;
} 

sBool PTN_ArgList::semanticCheck(void) {
   PTN_ArgListNode *c = first_arg;
   sBool r = YAC_TRUE;

   while(r && (NULL != c))
   {
      if(NULL != c->expr)
      {
         r = r && (c->expr->semanticCheck());
         c = c->next;
      }
      else
      {
         r = YAC_FALSE;
      }
   }

   return r; 
} 

void PTN_ArgList::optimize(void) { 
   PTN_ArgListNode *c = first_arg;

   while(NULL != c) 
   { 
      tks_optimize_expr(&c->expr, c->rtti_hint); 
      c->expr_opt = c->expr->getEval(); 
      c = c->next; 
   } 
} 

sBool PTN_ArgList::resolveXRef(void) { 
   sBool r = YAC_TRUE;
   PTN_ArgListNode *c = first_arg;

   while(r && (NULL != c))
   { 
      if(NULL != c->expr)
      { 
         r = r && c->expr->resolveXRef(); 
      } 
      else
      {
         r = YAC_FALSE;
      }

      c = c->next; 
   }

   return r; 
} 

#ifdef TKS_JIT 
sBool PTN_ArgList::forceHybrid(void) {
   sBool r = YAC_FALSE;
   PTN_ArgListNode *c = first_arg; 

   while(NULL != c)
   {
      if(NULL != c->expr)
      { 
         r = r | c->expr->forceHybrid(); 
      } 
      else 
      {
         r = YAC_TRUE; // list error?=?= 
      }

      c = c->next; 
   }

   return r; 
} 

sUI PTN_ArgList::compileY(const YAC_CommandY *_com, VMCore *_vm) { 
   PTN_ArgListNode *c = first_arg;

   while(NULL != c->next)
   {
      c = c->next;
   }

   sUI i = _com->y_arg_num; 

   while(NULL != c)
   {  
      sUI r = c->expr->compile(_vm);

      if(255 != r) 
      { 
         _vm->typecastStack(r, _com->y_arg_types[--i]); 
         c = c->previous;
      }
      else 
      {
         return 255; 
      }
   }

   return 0;
}

#endif // TKS_JIT 
