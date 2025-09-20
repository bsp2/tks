/// PTN_HashIniExpr.cpp
///
/// (c) 2001-2012 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_ExprHashEntry.h"
#include "PTN_ConstVal.h"
#include "PTN_HashIniExpr.h"
#include "YAC_HashTable.h"


PTN_HashIniExpr::PTN_HashIniExpr(sBool _bAlwaysNew) {
   first_entry  = NULL;
   num_entries  = 0;
   ht           = NULL; 
   is_const     = 0;
   b_always_new = _bAlwaysNew;
}

PTN_HashIniExpr::~PTN_HashIniExpr() {
   if(first_entry)  
   {  
      delete first_entry; first_entry=NULL;
   }
   if(ht)  
   {  
      YAC_DELETE(ht); ht=NULL;  
   }
}

sBool PTN_HashIniExpr::semanticCheck(void) {
   if(!b_always_new)
   {
      if(NULL == ht)
      {
         ht=(YAC_HashTable*)YAC_NEW_CORE_POOLED(YAC_CLID_HASHTABLE); 
         ht->varcache.allocCache(num_entries*2); 
      }
   }
   return (first_entry?first_entry->semanticCheck():1);
}

sBool PTN_HashIniExpr::resolveXRef(void) {
   return (first_entry?first_entry->resolveXRef():1);
}

void PTN_HashIniExpr::optimize(void) { 
   if(!ht)  
   {
      if(first_entry)   
      {
         first_entry->optimize(); 
      } 
   }
}

sBool PTN_HashIniExpr::isConst(void) { 
   if(!b_always_new)
   {
      sBool r=1; 
      sBool r2=0; 
      PTN_ExprHashEntry *c=first_entry; 
      while(r&&c) 
      { 
         r=r&&c->expr->isConst(); 
         r2=r2|tks_isobjectconstval(c->expr); 
         c=c->next; 
      } 
      if(r)  
      {
         if(r2) 
         { 
            optimize(); 
            YAC_Value t; 
            PTN_Env env; env.initDefault();
            eval(&env, &t, 0); 
            is_const=1; 
         } 
         else  
         {
            return 1;  
         } 
      }
   }
   return 0;  
} 

void PTN_HashIniExpr::evalConst(YAC_Value *_r) { 
   // xxx TKS_MT: only during init ? use *real* compiling thread context (temp scripts!)
   PTN_Env env; env.initDefault();
   eval(&env, _r, 1); 
   ht=0; 
} 

static void PTN_HashIniExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
   PTN_HashIniExpr *st = (PTN_HashIniExpr*)_st; // xxx discard const qualifier
   st->eval(_env, _r, 0); // XXX 
} 

void PTN_HashIniExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_HashIniExpr__eval(_env, _r, this); 
} 

Feval PTN_HashIniExpr::getEval(void) const { 
   return PTN_HashIniExpr__eval; 
} 

void PTN_HashIniExpr::eval(PTN_Env *_env, YAC_Value *_r, sBool _const) {
   // xxx TODO: move to __eval
   if(!is_const) 
   { 
      if(b_always_new)
      {
         ht=(YAC_HashTable*)YAC_NEW_CORE_POOLED(YAC_CLID_HASHTABLE); 
         ht->varcache.allocCache(num_entries*2); 
      }
      int i; 
      PTN_ExprHashEntry *en=first_entry;
      for(i=0; i<num_entries; i++)
      { 
         if(_const) 
         {
            en->expr->evalConst(_r);  
         }
         else  
         {
            en->expr_opt(_env, _r, en->expr); 
            if(Dhaveexception)
            {
               _r->unset();
               Dhandleexception(en->expr);
               return;
            }
         }
         // Note: must copy entry name if the expr is destroyed after constant evaluation
         TKS_CachedObject *co = ht->varcache.createEntry(&en->name, _const);
         co->copySafe(_r); 
         en = en->next;
      } 
   }
   if(b_always_new)
   {
      _r->initObject(ht, 1);
      ht= NULL;
   }
   else
   {
      _r->initObject(ht, _const);
   }
}

