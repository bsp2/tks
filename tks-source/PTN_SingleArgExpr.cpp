/// PTN_SingleArgExpr.cpp
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
#include "PTN_SingleArgExpr.h"


PTN_SingleArgExpr::PTN_SingleArgExpr(void) {
  expr1          = NULL;
  unary_operator = 0xFF;
}

PTN_SingleArgExpr::PTN_SingleArgExpr(PTN_Expr *_expr1, sU8 _op) {
  expr1     = _expr1;
  expr1_opt = expr1->getEval();
  if(YAC_OP_SUB == _op)
  {
     unary_operator = YAC_OP_NEG;
  }
  else
  {
     unary_operator = _op;
  }
}

PTN_SingleArgExpr::~PTN_SingleArgExpr() {
   TKS_DELETE_SAFE(expr1);
}

sBool PTN_SingleArgExpr::isConst(void) {
	return (expr1 ? expr1->isConst() : 1);
}

sBool PTN_SingleArgExpr::semanticCheck(void) {
	return (expr1 ? expr1->semanticCheck() : 0);
}

sBool PTN_SingleArgExpr::resolveXRef(void) {
    return (expr1 ? expr1->resolveXRef() : 0);
}

void PTN_SingleArgExpr::optimize(void) {
	tks_optimize_expr(&expr1, 0);
	expr1_opt = expr1->getEval();
}

static void PTN_SingleArgExpr__eval(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   const PTN_SingleArgExpr *st = (const PTN_SingleArgExpr*)_st;

   st->expr1_opt(_env, ret, st->expr1);
   if(Dhaveexception)
   {
      ret->unset();
      Dhandleexception(st->expr1);
      return;
   }

   switch(st->unary_operator)
   {
      case YAC_OP_NOT:
      {
         switch(ret->type)
         {
            case 1:
               ret->value.int_val=!ret->value.int_val;
               break;
            case 2:
               ret->initInt(!((sSI)ret->value.float_val));
               break;
            case 3:
            case 4:
               ret->safeInitInt(ret->isNullOrIF0());
               break;
         }
      }
      break;

      case YAC_OP_BITNOT:
      {
         switch(ret->type)
         {
            case 1:
               *(sUI*)&ret->value.int_val=~((sUI)ret->value.int_val);
               break;
            case 2:
               ret->initInt(~((sSI)ret->value.float_val));
               break;
            case 3:
               if(ret->value.object_val)
               {
                  YAC_Value opr;
                  if(ret->deleteme)
                  {
                     ret->value.object_val->yacOperator(YAC_OP_BITNOT, 0, &opr);
                  }
                  else
                  {
                     YAC_Object *o = YAC_CLONE_POOLED(_env->context, ret->value.object_val);
                     o->yacOperatorAssign(ret->value.object_val);
                     o->yacOperator(YAC_OP_BITNOT, 0, &opr);
                     ret->initObject(o, 1);
                  }
               }
               else
               {
                  ret->initInt(-1);
               }
               break;
            case 4:
               ret->safeInitInt(ret->value.object_val?0:-1);
               break;
         }
      }
      break;

      case YAC_OP_NEG:
      {
         switch(ret->type)
         {
            case 1:
               ret->value.int_val=-ret->value.int_val;
               break;
            case 2:
               ret->value.float_val=-ret->value.float_val;
               break;
            case 3:
               if(ret->value.object_val)
               {
                  YAC_Value opr;
                  if(ret->deleteme)
                  {
                     ret->value.object_val->yacOperator(YAC_OP_NEG, 0, &opr);
                  }
                  else
                  {
                     YAC_Object *o = YAC_CLONE_POOLED(_env->context, ret->value.object_val);
                     o->yacOperatorAssign(ret->value.object_val);
                     o->yacOperator(YAC_OP_NEG, 0, &opr);
                     ret->initObject(o, 1);
                  }
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case 4:
               ret->safeInitInt(0);
               break;
         }
      }
      break;
   }
}

Feval PTN_SingleArgExpr::getEval(void) const {
	return PTN_SingleArgExpr__eval;
}

void PTN_SingleArgExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
	PTN_SingleArgExpr__eval(_env, _r, this);
}


#ifdef TKS_JIT
sBool PTN_SingleArgExpr::forceHybrid(void) {
   if(!expr1->forceHybrid())
   {
      switch(expr1->getType())
      {
      default:
      case YAC_TYPE_VARIANT:
      case YAC_TYPE_OBJECT:
      case YAC_TYPE_STRING:
         return 1;
      case YAC_TYPE_INT:
      case YAC_TYPE_FLOAT:
         return 0;
      }
   }
   else
      return 1;
}

sU8 PTN_SingleArgExpr::compile(VMCore *_vm) {
	sU8 r=expr1->compile(_vm);
	switch(r)
	{
	case 1:
		switch(unary_operator)
		{
		case YAC_OP_NOT:
			Dasmop(VMOP_SINOT);
			return 1;
		case YAC_OP_BITNOT:
			Dasmop(VMOP_SIINV);
			return 1;
		case YAC_OP_NEG:
		case YAC_OP_SUB:
			Dasmop(VMOP_SINEG);
			return 1;
		}
		break;
	case 2:
		switch(unary_operator)
		{
		case YAC_OP_NEG:
		case YAC_OP_SUB:
			Dasmop(VMOP_SFNEG);
			return 2;
		}
		break;
	case 3:
	case 4:
		return 0xFF;
	}
	return 0xFF;
}
#endif
