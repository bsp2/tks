/// PTN_IMemberExpr.cpp
///
/// (c) 2001-2020 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "PTN_CallableExpr.h"
#include "PTN_IMemberExpr.h"

#include "tks_inc_class.h"


PTN_IMemberExpr::PTN_IMemberExpr(PTN_Expr *_objexpr, YAC_String *_membername) {
   obj_expr     = _objexpr; 
   obj_expr_opt = obj_expr->getEval();

   member_name.yacCopy(_membername);
   get_method_name.copy("get");
   get_method_name.append(&member_name);
   get_method_name.chars[3] &= ~32; // make first char of member name uppercase
}

PTN_IMemberExpr::~PTN_IMemberExpr() {
   TKS_DELETE_SAFE(obj_expr);
}

sBool PTN_IMemberExpr::semanticCheck(void) {
   return (obj_expr ? obj_expr->semanticCheck() : 0);
}

sBool PTN_IMemberExpr::resolveXRef(void) {
   if(obj_expr)  
   { 
      return obj_expr->resolveXRef(); 
   }
   Dprintf("[---] PTN_IMemberExpr::resolveXRef: missing obj_expr.\n");
   return 0;
}

void PTN_IMemberExpr::optimize(void) {
   if(obj_expr)  
   { 
      obj_expr->optimize(); 
      obj_expr_opt = obj_expr->getEval(); 
   }
}

static void PTN_IMemberExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   const PTN_IMemberExpr *st = (const PTN_IMemberExpr*)_st;
   
   TKS_CachedObject *co;
   const YAC_CommandY *cppCommandY;

   //
   // Evaluate object expression
   //
   YAC_Value iobjVal;
   st->obj_expr_opt(_env, &iobjVal, st->obj_expr); 
   if(Dhaveexception)
   {
      Dhandleexception(st->obj_expr);
      iobjVal.unsetFast();
      return;
   }

   //
   // autobox int/float values
   //
   iobjVal.typecast(YAC_TYPE_OBJECT);

   if(iobjVal.type >= YAC_TYPE_OBJECT)
   {
      if(TKS_VALID(iobjVal.value.object_val))
      { 
         //
         // First try meta class member
         //
         sUI ccid = iobjVal.value.object_val->class_ID; 

         // Is meta class ?
         if(YAC_CLID_CLASS == ccid)
         {
            TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance*) iobjVal.value.object_val;

            // Try to resolve member
            co = sci->class_decl->getMember((YAC_String*) &st->member_name);
            if(co)
            {
               // Read meta class member value
               sci->getClassMemberValueByID(_r, (sUI)co->value.int_val);
            } 
            else
            {
               Drtthrow(st, TKS_EXCEPTION_SCRIPTCLASSMEMBERNOTFOUND, "failed to resolve script-class member");
            } 
         }
         else
         {
            //
            // Try C++ getter method
            //

            // Lookup command
            TKS_ClassTemplate *clt = TKSCRIPT__ATOM_SPEEDPATH[ccid];
            cppCommandY = clt->findCommandY((YAC_String*)&st->get_method_name);
            
            if(cppCommandY) 
            { 
               if(cppCommandY->y_retval_cmd)  // _YAC_RVAL ?
                  cppCommandY = cppCommandY->y_retval_cmd;

               // Read C++ property via getter method
               cppCommandY->execRead(iobjVal.value.object_val, _r);
               Ddubiousfixstringtype(_r);
            } 
            else 
            {
               Drtthrow(st, TKS_EXCEPTION_NATIVECLASSMEMBERNOTFOUND, "failed to resolve c++-class member");
            }
         }
      }
      else
      {
         Drtthrowinvalidpointer(st, "cannot get member of invalid object", iobjVal.value.object_val);
      }
   }
   else
   { 
      // type mismatch..
      Drtthrow(st, TKS_EXCEPTION_TYPEMISMATCH, "cannot get member of non-object type");
   } 

   iobjVal.unsetFast();
}

Feval PTN_IMemberExpr::getEval(void) const {
   return PTN_IMemberExpr__eval;
}

void PTN_IMemberExpr::evalCallableExpr(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_IMemberExpr__eval(_env, _r, this);
}

void PTN_IMemberExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_IMemberExpr__eval(_env, _r, this);
}
