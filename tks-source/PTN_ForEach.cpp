/// PTN_ForEach.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_BreakableStatement.h"
#include "PTN_ForEach.h"


PTN_ForEach::PTN_ForEach(void) {
   cvar         = NULL;
   obj_expr     = NULL;
   obj_expr_opt = NULL;
   body         = NULL;
   break_tag    = 0;
}

PTN_ForEach::~PTN_ForEach() {
   TKS_DELETE_SAFE(body);

   TKS_DELETE_SAFE(obj_expr);

   cvar = NULL;
}

void PTN_ForEach::init(TKS_CachedObject *_cvar, PTN_Expr *_objexpr, PTN_Statement *_body) { 
   cvar         = _cvar;
   obj_expr     = _objexpr;
   /////obj_expr_opt = obj_expr->getEval(); // xxx
   body         = _body;
}

void PTN_ForEach::useBreak(void) {
   break_tag = 1;
}

sUI PTN_ForEach::getID(void) const {
	return PTN_FOREACH;
}

sBool PTN_ForEach::semanticCheck(void) {
   return 
      (cvar)                                     &&
      (body ? body->semanticCheckAll() : 0)      &&
      (obj_expr ? obj_expr->semanticCheck() : 0) 
      ;
}

void PTN_ForEach::optimize(void) {
   if(body) 
   {
      body->optimizeAll();
   }
   if(obj_expr)  
   { 
      obj_expr->optimize(); 
      obj_expr_opt = obj_expr->getEval(); 
   } 
   b_dynamicvar = (cvar->type == YAC_TYPE_VARIANT);
}

sBool PTN_ForEach::resolveXRef(void) {
   return 
      (body->resolveXRefAll())  &&
      (obj_expr->resolveXRef())
      ;
}

void PTN_ForEach::subGenCallList(void) {
	if(body)
   {
      body->genCallList();
   }
}
 
static void PTN_ForEach__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_ForEach *st = (const PTN_ForEach*)_st; 
   sU8 bytes[YAC_MAX_ITERATOR_SIZE]; // Iterator object is created on the stack
   sBool bHaveIterator = 0;
   YAC_Iterator *it = (YAC_Iterator*) bytes;

   YAC_Value *co = Dgetvar(st->cvar); 
   
   YAC_Value ort;

   st->obj_expr_opt(_env, &ort, st->obj_expr); 
   if(Dhaveexception)
   {
      ort.unsetFast();
      Dhandleexception(st->obj_expr);
      return;
   }

   if(ort.type < YAC_TYPE_OBJECT)
   {
      Drtthrow(_st, TKS_EXCEPTION_TYPEMISMATCH, "cannot get iterator for non-object value");
      return;
   }
   else
   { 
      /*if(ort.value.object_val == NULL) 
      { 
         return; // xxx ???
      }*/
      if(TKS_VALID(ort.value.object_val))
      {
         bHaveIterator = ort.value.object_val->yacIteratorInit(it);
      }
      else
      {
         Drtthrowinvalidpointer(_st, "cannot get iterator for invalid foreach object", ort.value.object_val);
         return;
      } 
   }
   if(bHaveIterator)
   {
      ////void *oldvar = (void*) co->value.object_val;
      ////sUI olddeleteme = co->deleteme;
      ////co->deleteme = 0;

      sBool *bBreak = _env->context->pushBreak();

      it->begin();
      YAC_Value r; it->getNext(&r);
      while(_env->context->b_running && (r.type!=0xFF) && _env->continue_flag && !*bBreak)
      {
         /*if(r.type >= YAC_TYPE_OBJECT)
         { 
            if(r.value.object_val) 
            { 
               Ddubiousfixstringtype(&r);
            } 
         } */
         /*if(4 == r.type)
         {
            r.type = 3; // fix String to Object type
         }*/
         if(st->b_dynamicvar || (r.type==co->type) || ((r.type >= 3) && (co->type >= 3)) )
         { 
            co->copySafe(&r); 
            st->body->evalFirst(_env); 
            it->getNext(&r);
            // ---- use break to exit loop 
         }
         else  
         { 
            Dprintf("[---] type mismatch in foreach loop; (%i!=%i)\n", r.type, co->type); 
            r.type = 0xFF; 
         }
      } 

      it->end();

      ////co->value.object_val = (YAC_Object*)oldvar;
      ////co->deleteme         = olddeleteme;

      if(*bBreak) 
      {
         _env->continue_flag = 1; 
      }
      _env->context->popBreak();
   }
   ort.unsetFast();
}
 
Fevalst PTN_ForEach::getEvalSt(void) const { 
	return PTN_ForEach__eval; 
} 
 
void PTN_ForEach::eval(PTN_Env *_env) const { 
	PTN_ForEach__eval(_env, this); 
} 

