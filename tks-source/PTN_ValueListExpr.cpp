/// PTN_ValueListExpr.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_compiler.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "YAC_ListNode.h"
#include "PTN_ValueListEntry.h"
#include "PTN_ValueListExpr.h"

#include "PTN_ConstVal.h"



PTN_ValueListExpr::PTN_ValueListExpr(sBool _bAlwaysNew) { 
	value_list   = 0; 
	first_entry  = 0; 
	last_entry   = 0; 
	is_const     = 0; 
   b_always_new = _bAlwaysNew;
} 
 
PTN_ValueListExpr::~PTN_ValueListExpr() { 
	if(value_list) 
	{ 
		YAC_DELETE(value_list); 
		value_list=0; 
	} 
	if(first_entry) 
	{ 
		delete first_entry; 
		first_entry=0; 
		last_entry=0; 
	} 
} 
 
void PTN_ValueListExpr::addExpr(PTN_Expr *_expr) { 
   PTN_ValueListEntry *nn;
   PTN_NEW_STATIC_NODE(nn, PTN_ValueListEntry)(_expr);
	if(!first_entry) 
	{ 
		first_entry = nn; 
		last_entry=first_entry; 
	} 
	else 
	{ 
		last_entry->next = nn; 
		last_entry=last_entry->next; 
	} 
} 
 
sBool PTN_ValueListExpr::semanticCheck(void) { 
	PTN_ValueListEntry *e=first_entry; 
	sBool r=1; 
	while(r&&e) 
	{ 
		r=r&&e->expr->semanticCheck(); 
		e=e->next; 
	} 
	return r; 
} 
 
void PTN_ValueListExpr::optimize(void) { 
	if(is_const!=2) 
	{ 
		PTN_ValueListEntry *e=first_entry; 
		while(e) 
		{ 
			tks_optimize_expr(&e->expr, 0); 
			e->expr_opt=e->expr->getEval(); 
			e=e->next; 
		} 
	} 
	else 
		is_const=1; // already optimized 
} 
 
sBool PTN_ValueListExpr::resolveXRef(void) { 
	value_list = (YAC_ListNode*)YAC_NEW_CORE_POOLED(YAC_CLID_LISTNODE); 
	YAC_ListNode *c = value_list; 
	PTN_ValueListEntry *e = first_entry; 
	sBool r = 1; 
	while(r&&e) 
	{ 
		 
		r=r&&e->expr->resolveXRef(); 
		e=e->next; 
		if((!b_always_new) && e) 
		{ 
			c->appendEmpty(); 
			c=c->next; 
		} 
	} 
	return r; 
} 
 
sBool PTN_ValueListExpr::isConst(void) { 
   if(!b_always_new)
   {
      sBool r=1; 
      sBool r2=0; 
      PTN_ValueListEntry *e=first_entry; 
      while(r&&e) 
      { 
         r=r&&e->expr->isConst(); 
         r2=r2|tks_isobjectconstval(e->expr); 
         e=e->next; 
      } 
      if(r) 
      { 
         if(r2) 
         { 
            optimize(); 
            YAC_Value tv; 
            PTN_Env env; env.initDefault();
            eval(&env, &tv, 0); 
            is_const=2; 
         } 
         else
         {
            return 1; 
         }
      } 
   }
	return 0;  
} 
 
void PTN_ValueListExpr::evalConst(YAC_Value *_r) { 
   PTN_Env env; env.initDefault();
	eval(&env, _r, 1); 
	//value_list = NULL; 
} 
 
void PTN_ValueListExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
	((PTN_ValueListExpr*)this)->eval(_env, _r, 0); 
} 
 
void PTN_ValueListExpr::eval(PTN_Env *_env, YAC_Value *_r, sBool _const) { 
   /// xxx TODO: move to __eval function
	if(!is_const) 
	{ 
		PTN_ValueListEntry *e = first_entry;
      if(NULL == e)
      {
         _r->initNull();
         return; // empty list
      }
      if(b_always_new)
      {
         if(NULL != value_list)
         {
            YAC_DELETE(value_list);
         }
         value_list = (YAC_ListNode*)YAC_NEW_CORE_POOLED(YAC_CLID_LISTNODE);
      }
      YAC_ListNode *c = value_list;
		while(e) 
		{ 
			if(_const) 
         {
				e->expr->evalConst(c); 
         }
			else 
         {
				e->expr->eval(_env, c); 
         }
			e = e->next; 
         if(e&&b_always_new)
         {
            c = c->appendEmpty();
         }
         else
         {
			   c = c->next; 
         }
		} 
	} 
   if(b_always_new)
   {
      _r->initObject(value_list, 1);
      value_list = NULL;
   }
   else
   {
	   _r->initObject(value_list, _const); 
      if(_const)
      {
         value_list = NULL;
      }
   }
} 
 
static void PTN_ValueListExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
	PTN_ValueListExpr*st = (PTN_ValueListExpr*)_st; 
	st->eval(_env, _r, 0); 
} 
 
Feval PTN_ValueListExpr::getEval(void) const {
	return PTN_ValueListExpr__eval; 
} 
