/// PTN_DoubleArgExpr.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL)
///
///

#include <math.h> // for floorf()

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_BuiltinFun.h"
#include "PTN_Statement.h"
#include "tks_inc_jit.h"

#include "PTN_VarExpr.h"
#include "PTN_DoubleArgExpr.h"


#define Dqintval(v,i) v.type=1; v.value.int_val=i
#define Dqfloatval(v,f) v.type=2; v.value.float_val=f

static void PTN_DoubleArgExpr__eval      (PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__eval_beq  (PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__eval_bne  (PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__eval_land (PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__eval_lor  (PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st);

static void PTN_DoubleArgExpr__evaliiBEQ (PTN_Env *_env, YAC_Value *_r,  const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiBNE (PTN_Env *_env, YAC_Value *_r,  const PTN_Expr *_st);

static void PTN_DoubleArgExpr__evaliiADD (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiSUB (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiMUL (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiDIV (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiMOD (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiSHL (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiSHR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiCEQ (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiCNE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiCLE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiCLT (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiCGE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiCGT (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiAND (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiOR  (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiEOR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiLAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evaliiLOR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);

static void PTN_DoubleArgExpr__evalffADD (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffSUB (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffMUL (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffDIV (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffMOD (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffSHL (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffSHR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffCEQ (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffCNE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffCLE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffCLT (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffCGE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffCGT (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffAND (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffOR  (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffEOR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffLAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalffLOR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);

static void PTN_DoubleArgExpr__evalifADD (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifSUB (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifMUL (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifDIV (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifMOD (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifSHL (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifSHR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifCEQ (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifCNE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifCLE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifCLT (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifCGE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifCGT (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifAND (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifOR  (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifEOR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifLAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalifLOR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);

static void PTN_DoubleArgExpr__evalfiADD (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiSUB (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiMUL (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiDIV (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiMOD (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiSHL (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiSHR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiCEQ (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiCNE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiCLE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiCLT (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiCGE (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiCGT (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiAND (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiOR  (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiEOR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiLAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);
static void PTN_DoubleArgExpr__evalfiLOR (PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st);



PTN_DoubleArgExpr::PTN_DoubleArgExpr(void) {
   expr1 = expr2 = NULL;
   expr_operator = 0xFF;
}

PTN_DoubleArgExpr::PTN_DoubleArgExpr(PTN_Expr * _expr1,
                                     sU8        _op,
                                     PTN_Expr * _expr2
                                     )
{
   expr1         = _expr1;
   expr2         = _expr2;
   expr1_opt     = expr1->getEval();
   expr2_opt     = expr2->getEval();
   expr_operator = _op;
}

PTN_DoubleArgExpr::~PTN_DoubleArgExpr() {
   if(expr1) {	delete expr1; expr1 = NULL; }
   if(expr2) { delete expr2; expr1 = NULL; }
}

sBool PTN_DoubleArgExpr::isConst(void) {
   return
      (expr1?expr1->isConst():1) &&
      (expr2?expr2->isConst():1) ;
}

sBool PTN_DoubleArgExpr::semanticCheck(void) {
   return
      (expr1?expr1->semanticCheck():0) &&
      (expr2?expr2->semanticCheck():0) ;
}

sBool PTN_DoubleArgExpr::resolveXRef(void) {
   if(expr1)
   {
      if(expr1->resolveXRef())
      {
         if(expr2)
         {
            if(expr2->resolveXRef())
            {
               return 1;
            }
            else
            {
               Dprintf("[---] DoubleArgExpr::resolveXRef: (expr2)\n");
            }
         }
         else
         {
            Dprintf("[---] DoubleArgExpr::resolveXRef: (expr2==0)\n");
         }
      }
      else
      {
         Dprintf("[---] DoubleArgExpr::resolveXRef: (expr1)\n");
      }
   }
   else
   {
      Dprintf("[---] DoubleArgExpr::resolveXRef: (expr1==0)\n");
   }
   return 0;
}

#define Beval1 \
   PTN_DoubleArgExpr*st=(PTN_DoubleArgExpr*)_st; \
   YAC_Value r1; \
   st->expr1_opt(_env, &r1, st->expr1); \
   if(Dhaveexception)\
   {\
      r1.unsetFast();\
      Dhandleexception(st->expr1);\
      return;\
   }(void)0

#define Beval2 \
   YAC_Value r2; \
   st->expr2_opt(_env, &r2, st->expr2); \
   if(Dhaveexception)\
   {\
      r1.unsetFast();\
      r2.unsetFast();\
      Dhandleexception(st->expr2);\
      return;\
   }(void)0

////static int xxx_eval_debugcount = 10;

static void PTN_DoubleArgExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   Dtracest;
   PTN_DoubleArgExpr *st = (PTN_DoubleArgExpr*)_st;

   switch(st->expr_operator)
   {
      default:
         // Always eval left- and right-hand side expressions
         {
            Beval1;
            Beval2;
            /*if(tks_debug_called && (--xxx_eval_debugcount > 0))
              {
              printf("xxx DoubleArgExpr__eval:\n");
              printf(" r1.type=%d r1.value.int_val=0x%p\n", r1.type, r1.value.int_val);
              printf(" r2.type=%d r2.value.int_val=0x%p\n", r2.type, r2.value.int_val);
              if(4 == r1.type)
              {
              printf("r1.class_id=%d\n", r1.value.object_val->class_ID);
              printf("r1.className=%s\n", r1.value.object_val->yacClassName());
              }
              if(4 == r2.type)
              {
              printf("r2.class_id=%d\n", r2.value.object_val->class_ID);
              printf("r2.validation_tag=0x%08x\n", r2.value.object_val->validation_tag);
              printf("r2.className=%s\n", r2.value.object_val->yacClassName());
              }
              }*/
            PTN_DoubleArgExpr::EvalOp(_env, _r, &r1, &r2, st->expr_operator, _st);
         }
         break;

      case YAC_OP_LAND:
         // Short-circuit evaluation (potentially skip right-hand side expr)
         PTN_DoubleArgExpr__eval_land(_env, _r, _st);
         break;

      case YAC_OP_LOR:
         // Short-circuit evaluation (potentially skip right-hand side expr)
         PTN_DoubleArgExpr__eval_lor(_env, _r, _st);
         break;
   }
}

#define Beval1_beq \
   PTN_DoubleArgExpr*st=(PTN_DoubleArgExpr*)_st; \
   YAC_Value r1; \
   ((PTN_BuiltinFun*)st->expr1)->expr_opt(_env, &r1, ((PTN_BuiltinFun*)st->expr1)->expr); \
   if(Dhaveexception)\
   {\
      r1.unsetFast();\
      Dhandleexception(((PTN_BuiltinFun*)st->expr1)->expr);\
      return;\
   }(void)0

#define Beval2_beq \
   YAC_Value r2; \
   ((PTN_BuiltinFun*)st->expr2)->expr_opt(_env, &r2, ((PTN_BuiltinFun*)st->expr2)->expr); \
   if(Dhaveexception)\
   {\
      r1.unsetFast();\
      r2.unsetFast();\
      Dhandleexception(((PTN_BuiltinFun*)st->expr2)->expr); \
      return;\
   }(void)0

static void PTN_DoubleArgExpr__eval_beq(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   Dtracest;
   Beval1_beq;
   Beval2_beq;
   PTN_DoubleArgExpr::EvalBEQ(r1, r2, _r);
}

static void PTN_DoubleArgExpr__eval_bne(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   Dtracest;
   Beval1_beq;
   Beval2_beq;
   PTN_DoubleArgExpr::EvalBNE(r1, r2, _r);
}

static void PTN_DoubleArgExpr__evaliiBEQ(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   // (note) compare 32bit int or float bit patterns
   Dtracest;
   Beval1_beq;
   Beval2_beq;
   _r->initInt(r1.value.int_val == r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiBNE(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   // (note) compare 32bit int or float bit patterns
   Dtracest;
   Beval1_beq;
   Beval2_beq;
   _r->initInt(r1.value.int_val != r2.value.int_val);
}

static void PTN_DoubleArgExpr__eval_land(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   Dtracest;
   Beval1;

   switch(r1.type)
   {
      default:
      case YAC_TYPE_VOID:
         _r->initInt(YAC_FALSE);
         break;

      case YAC_TYPE_INT:
         if(0 != r1.value.int_val)
         {
            Beval2;
            switch(r2.type)
            {
               default:
               case YAC_TYPE_VOID:
                  _r->initInt(YAC_FALSE);
                  break;

               case YAC_TYPE_INT:
                  _r->initInt(0 != r2.value.int_val);
                  break;

               case YAC_TYPE_FLOAT:
                  _r->initInt(0 != sSI(r2.value.float_val));
                  break;

               case YAC_TYPE_OBJECT:
                  if(YAC_VALID(r2.value.object_val))
                  {
                     YAC_Value r2z;
                     r2.value.object_val->yacOperatorI(YAC_OP_CNE, 0, &r2z);  // not zero ?
                     r2.unsetFast();
                     if(YAC_TYPE_INT == r2z.type)
                     {
                        _r->initInt(0 != r2z.value.int_val);
                     }
                     else
                     {
                        r2z.unsetFast();
                        _r->initInt(YAC_FALSE);
                     }
                  }
                  else
                  {
                     _r->initInt(YAC_FALSE);
                  }
                  break;

               case YAC_TYPE_STRING:
                  if(YAC_VALID(r2.value.object_val))
                  {
                     _r->initInt(!r2.value.string_val->isBlank());
                     r2.unsetFast();
                  }
                  else
                  {
                     _r->initInt(YAC_FALSE);
                  }
                  break;
            }
         }
         else
         {
            _r->initInt(YAC_FALSE);
         }
         break;

      case YAC_TYPE_FLOAT:
         if(0 != sSI(r1.value.float_val))
         {
            Beval2;
            switch(r2.type)
            {
               default:
               case YAC_TYPE_VOID:
                  _r->initInt(YAC_FALSE);
                  break;

               case YAC_TYPE_INT:
                  _r->initInt(0 != r2.value.int_val);
                  break;

               case YAC_TYPE_FLOAT:
                  _r->initInt(0 != sSI(r2.value.float_val));
                  break;

               case YAC_TYPE_OBJECT:
                  if(YAC_VALID(r2.value.object_val))
                  {
                     YAC_Value r2z;
                     r2.value.object_val->yacOperatorI(YAC_OP_CNE, 0, &r2z);  // not zero ?
                     r2.unsetFast();
                     if(YAC_TYPE_INT == r2z.type)
                     {
                        _r->initInt(0 != r2z.value.int_val);
                     }
                     else
                     {
                        r2z.unsetFast();
                        _r->initInt(YAC_FALSE);
                     }
                  }
                  else
                  {
                     _r->initInt(YAC_FALSE);
                  }
                  break;

               case YAC_TYPE_STRING:
                  if(YAC_VALID(r2.value.object_val))
                  {
                     _r->initInt(!r2.value.string_val->isBlank());
                     r2.unsetFast();
                  }
                  else
                  {
                     _r->initInt(YAC_FALSE);
                  }
                  break;
            }
         } // if r1 int_val
         else
         {
            _r->initInt(YAC_FALSE);
         }
         break;

      case YAC_TYPE_OBJECT:
         if(YAC_VALID(r1.value.object_val))
         {
            Beval2;
            YAC_Value r1z;
            r1.value.object_val->yacOperatorI(YAC_OP_CNE, 0, &r1z);  // not zero ?
            r1.unsetFast();
            if((YAC_TYPE_INT == r1z.type) && (0 != r1z.value.int_val))
            {
               switch(r2.type)
               {
                  default:
                  case YAC_TYPE_VOID:
                     _r->initInt(YAC_FALSE);
                     break;

                  case YAC_TYPE_INT:
                     _r->initInt(0 != r2.value.int_val);
                     break;

                  case YAC_TYPE_FLOAT:
                     _r->initInt(0 != sSI(r2.value.float_val));
                     break;

                  case YAC_TYPE_OBJECT:
                     if(YAC_VALID(r2.value.object_val))
                     {
                        YAC_Value r2z;
                        r2.value.object_val->yacOperatorI(YAC_OP_CNE, 0, &r2z);  // not zero ?
                        r2.unsetFast();
                        if(YAC_TYPE_INT == r2z.type)
                        {
                           _r->initInt(0 != r2z.value.int_val);
                        }
                        else
                        {
                           r2z.unsetFast();
                           _r->initInt(YAC_FALSE);
                        }
                     }
                     else
                     {
                        _r->initInt(YAC_FALSE);
                     }
                     break;

                  case YAC_TYPE_STRING:
                     if(YAC_VALID(r2.value.object_val))
                     {
                        _r->initInt(!r2.value.string_val->isBlank());
                        r2.unsetFast();
                     }
                     else
                     {
                        _r->initInt(YAC_FALSE);
                     }
                     break;
               } // switch r2.type
            } // if 0 != r1i
            else
            {
               r1z.unsetFast();
               _r->initInt(YAC_FALSE);
            }
         } // if r1 valid
         else
         {
            _r->initInt(YAC_FALSE);
         }
         break;

      case YAC_TYPE_STRING:
         if(YAC_VALID(r1.value.string_val))
         {
            if(!r1.value.string_val->isBlank())
            {
               Beval2;
               switch(r2.type)
               {
                  default:
                  case YAC_TYPE_VOID:
                     _r->initInt(YAC_FALSE);
                     break;

                  case YAC_TYPE_INT:
                     _r->initInt(0 != r2.value.int_val);
                     break;

                  case YAC_TYPE_FLOAT:
                     _r->initInt(0 != sSI(r2.value.float_val));
                     break;

                  case YAC_TYPE_OBJECT:
                     if(YAC_VALID(r2.value.object_val))
                     {
                        YAC_Value r2z;
                        r2.value.object_val->yacOperatorI(YAC_OP_CNE, 0, &r2z);  // not zero ?
                        r2.unsetFast();
                        if(YAC_TYPE_INT == r2z.type)
                        {
                           _r->initInt(0 != r2z.value.int_val);
                        }
                        else
                        {
                           r2z.unsetFast();
                           _r->initInt(YAC_FALSE);
                        }
                     }
                     else
                     {
                        _r->initInt(YAC_FALSE);
                     }
                     break;

                  case YAC_TYPE_STRING:
                     if(YAC_VALID(r2.value.object_val))
                     {
                        _r->initInt(!r2.value.string_val->isBlank());
                        r2.unsetFast();
                     }
                     else
                     {
                        _r->initInt(YAC_FALSE);
                     }
                     break;
               } // switch r2.type
            }
            else
            {
               _r->initInt(YAC_FALSE);
            }
            r1.unsetFast();
         } // if r1 valid
         else
         {
            _r->initInt(YAC_FALSE);
         }
         break;
   } // switch r1.type
}


static void PTN_DoubleArgExpr__eval_lor(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   Dtracest;
   Beval1;

   sBool bRet2 = YAC_FALSE;

   switch(r1.type)
   {
      default:
      case YAC_TYPE_VOID:
         _r->initInt(YAC_FALSE);
         bRet2 = YAC_TRUE;
         break;

      case YAC_TYPE_INT:
         if(0 == r1.value.int_val)
         {
            bRet2 = YAC_TRUE;
         } // if 0 == r1 int_val
         else
         {
            _r->initInt(YAC_TRUE);
         }
         break;

      case YAC_TYPE_FLOAT:
         if(0 == sSI(r1.value.float_val))
         {
            bRet2 = YAC_TRUE;
         } // if 0 == r1 float_val
         else
         {
            _r->initInt(YAC_TRUE);
         }
         break;

      case YAC_TYPE_OBJECT:
         if(YAC_VALID(r1.value.object_val))
         {
            YAC_Value r1z;
            r1.value.object_val->yacOperatorI(YAC_OP_CNE, 0, &r1z);  // not zero ?
            r1.unsetFast();
            if((YAC_TYPE_INT == r1z.type) && (0 == r1z.value.int_val))
            {
               bRet2 = YAC_TRUE;
            } // if 0 == r1z int_val
            else
            {
               r1z.unsetFast();
               _r->initInt(YAC_TRUE);
            }
         } // if r1 valid
         else
         {
            bRet2 = YAC_TRUE;
         }
         break;

      case YAC_TYPE_STRING:
         if(YAC_VALID(r1.value.string_val))
         {
            if(r1.value.string_val->isBlank())
            {
               bRet2 = YAC_TRUE;
            }
            else
            {
               _r->initInt(YAC_TRUE);
            }
            r1.unsetFast();
         } // if r1 valid
         else
         {
            bRet2 = YAC_TRUE;
         }
         break;
   } // switch r1.type


   // Left-hand side is void, null, or 0
   if(bRet2)
   {
      Beval2;
      switch(r2.type)
      {
         default:
         case YAC_TYPE_VOID:
            _r->initInt(YAC_FALSE);
            break;

         case YAC_TYPE_INT:
            _r->initInt(0 != r2.value.int_val);
            break;

         case YAC_TYPE_FLOAT:
            _r->initInt(0 != sSI(r2.value.float_val));
            break;

         case YAC_TYPE_OBJECT:
            if(YAC_VALID(r2.value.object_val))
            {
               YAC_Value r2z;
               r2.value.object_val->yacOperatorI(YAC_OP_CNE, 0, &r2z);  // not zero ?
               r2.unsetFast();
               if(YAC_TYPE_INT == r2z.type)
               {
                  _r->initInt(0 != r2z.value.int_val);
               }
               else
               {
                  r2z.unsetFast();
                  _r->initInt(YAC_FALSE);
               }
            }
            else
            {
               _r->initInt(YAC_FALSE);
            }
            break;

         case YAC_TYPE_STRING:
            if(YAC_VALID(r2.value.object_val))
            {
               _r->initInt(!r2.value.string_val->isBlank());
               r2.unsetFast();
            }
            else
            {
               _r->initInt(YAC_FALSE);
            }
            break;
      } // switch r2.type
   }
}

void PTN_DoubleArgExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_DoubleArgExpr__eval(_env, _r, this);
}

static void PTN_DoubleArgExpr__evaliiADD(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val+r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiSUB(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val-r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiMUL(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val*r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiDIV(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   if(r2.value.int_val)
   {
      ret->initInt(r1.value.int_val/r2.value.int_val);
   }
   else
   {
      ret->initInt(0);
   }
}

static void PTN_DoubleArgExpr__evaliiMOD(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   if(r2.value.int_val)
   {
      ret->initInt(r1.value.int_val%r2.value.int_val);
   }
   else
   {
      ret->initInt(0);
   }
}

static void PTN_DoubleArgExpr__evaliiSHL(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val<<r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiSHR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val>>r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiCEQ(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val==r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiCNE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val!=r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiCLE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val<=r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiCLT(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val<r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiCGE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val>=r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiCGT(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val>r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val&r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val|r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiEOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val^r2.value.int_val);
}

static void PTN_DoubleArgExpr__evaliiLAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   // Short-circuit evaluation
   if(0 != r1.value.int_val)
   {
      Beval2;
      ret->initInt(0 != r2.value.int_val);
   }
   else
   {
      ret->initInt(YAC_FALSE);
   }
}

static void PTN_DoubleArgExpr__evaliiLOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   // Short-circuit evaluation
   Beval1;
   if(0 == r1.value.int_val)
   {
      Beval2;
      ret->initInt(0 != r2.value.int_val);
   }
   else
   {
      ret->initInt(YAC_TRUE);
   }
}

static void PTN_DoubleArgExpr__evalffADD(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initFloat(r1.value.float_val+r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalffSUB(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initFloat(r1.value.float_val-r2.value.float_val );
}

static void PTN_DoubleArgExpr__evalffMUL(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initFloat(r1.value.float_val*r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalffDIV(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   if(Dfltnonzero(r2.value.float_val))
   {
      ret->initFloat(r1.value.float_val / r2.value.float_val);
   }
   else
   {
      ret->initFloat(0.0f);
   }
}

static void PTN_DoubleArgExpr__evalffMOD(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;

   if(r2.value.float_val != 0.0f)
   {
      ret->initFloat(r1.value.float_val - r2.value.float_val * floorf(r1.value.float_val/r2.value.float_val));
   }
   else
   {
      ret->initFloat(0.0f);
   }
}

static void PTN_DoubleArgExpr__evalffSHL(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sSI)r1.value.float_val)<<((sSI)r2.value.float_val) );
}

static void PTN_DoubleArgExpr__evalffSHR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sSI)r1.value.float_val)>>((sSI)r2.value.float_val) );
}

static void PTN_DoubleArgExpr__evalffCEQ(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(Dfltequal(r1.value.float_val, r2.value.float_val));
}

static void PTN_DoubleArgExpr__evalffCNE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(Dfltnotequal(r1.value.float_val, r2.value.float_val));
}

static void PTN_DoubleArgExpr__evalffCLE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.float_val<=r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalffCLT(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.float_val<r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalffCGE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.float_val>=r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalffCGT(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.float_val>r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalffAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sSI)r1.value.float_val)&((sSI)r2.value.float_val) );
}

static void PTN_DoubleArgExpr__evalffOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sSI)r1.value.float_val)|((sSI)r2.value.float_val) );
}

static void PTN_DoubleArgExpr__evalffEOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sSI)r1.value.float_val)^((sSI)r2.value.float_val) );
}

static void PTN_DoubleArgExpr__evalffLAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   // Short-circuit evaluation
   Beval1;
   if(0 != (((sSI)r1.value.float_val)))
   {
      Beval2;
      ret->initInt(0 != (((sSI)r2.value.float_val)) );
   }
   else
   {
      ret->initInt(YAC_FALSE);
   }
}

static void PTN_DoubleArgExpr__evalffLOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   // Short-circuit evaluation
   Beval1;
   if(0 == (((sSI)r1.value.float_val)))
   {
      Beval2;
      ret->initInt(0 != (((sSI)r2.value.float_val)) );
   }
   else
   {
      ret->initInt(YAC_TRUE);
   }
}

static void PTN_DoubleArgExpr__evalifADD(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initFloat(r1.value.int_val+r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalifSUB(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initFloat(r1.value.int_val-r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalifMUL(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initFloat(r1.value.int_val*r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalifDIV(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   if(Dfltnonzero(r2.value.float_val))
   {
      ret->initFloat(r1.value.int_val/r2.value.float_val);
   }
   else
   {
      ret->initFloat(0.0f);
   }
}

static void PTN_DoubleArgExpr__evalifMOD(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;

   if(r2.value.float_val != 0.0f)
   {
      ret->initFloat((sF32)r1.value.int_val - r2.value.float_val * floorf(((sF32)r1.value.int_val)/r2.value.float_val));
   }
   else
   {
      ret->initFloat(0.0f);
   }
}

static void PTN_DoubleArgExpr__evalifSHL(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val<<((sSI)r2.value.float_val));
}

static void PTN_DoubleArgExpr__evalifSHR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val>>((sSI)r2.value.float_val));
}

static void PTN_DoubleArgExpr__evalifCEQ(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(Dfltequal((sF32)r1.value.int_val, r2.value.float_val));
   // ret->initInt(r1.value.int_val==((sSI)r2.value.float_val));
}

static void PTN_DoubleArgExpr__evalifCNE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(Dfltnotequal((sF32)r1.value.int_val, r2.value.float_val));
   // ret->initInt(r1.value.int_val!=((sSI)r2.value.float_val));
}

static void PTN_DoubleArgExpr__evalifCLE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sF32)r1.value.int_val)<=r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalifCLT(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sF32)r1.value.int_val)<r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalifCGE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sF32)r1.value.int_val)>=r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalifCGT(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sF32)r1.value.int_val)>r2.value.float_val);
}

static void PTN_DoubleArgExpr__evalifAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val&((sSI)r2.value.float_val));
}

static void PTN_DoubleArgExpr__evalifOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.int_val|((sSI)r2.value.float_val));
}

static void PTN_DoubleArgExpr__evalifEOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt( ((r1.value.int_val)^(((sSI)r2.value.float_val))) );
}

static void PTN_DoubleArgExpr__evalifLAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   // Short-circuit evaluation
   Beval1;
   if(0 != r1.value.int_val)
   {
      Beval2;
      ret->initInt(0 != (((sSI)r2.value.float_val)));
   }
   else
   {
      ret->initInt(YAC_FALSE);
   }
}

static void PTN_DoubleArgExpr__evalifLOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   // Short-circuit evaluation
   Beval1;
   if(0 == r1.value.int_val)
   {
      Beval2;
      ret->initInt(0 != (((sSI)r2.value.float_val)));
   }
   else
   {
      ret->initInt(YAC_TRUE);
   }
}



static void PTN_DoubleArgExpr__evalfiADD(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initFloat(r1.value.float_val+r2.value.int_val);
}

static void PTN_DoubleArgExpr__evalfiSUB(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initFloat(r1.value.float_val-r2.value.int_val);
}

static void PTN_DoubleArgExpr__evalfiMUL(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initFloat(r1.value.float_val*r2.value.int_val);
}

static void PTN_DoubleArgExpr__evalfiDIV(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   if(r2.value.int_val)
   {
      ret->initFloat(r1.value.float_val/((sF32)r2.value.int_val) );
   }
   else
   {
      ret->initFloat(0.0f);
   }

}

static void PTN_DoubleArgExpr__evalfiMOD(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;

   if(0 != r2.value.int_val)
   {
      ret->initFloat(r1.value.float_val - (sF32)r2.value.int_val * floorf(r1.value.float_val/((sF32)r2.value.int_val)));
   }
   else
   {
      ret->initFloat(0.0f);
   }
}

static void PTN_DoubleArgExpr__evalfiSHL(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sSI)r1.value.float_val)<<r2.value.int_val );
}

static void PTN_DoubleArgExpr__evalfiSHR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sSI)r1.value.float_val)>>r2.value.int_val );
}

static void PTN_DoubleArgExpr__evalfiCEQ(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(Dfltequal(r1.value.float_val, ((sF32)r2.value.int_val)));
}

static void PTN_DoubleArgExpr__evalfiCNE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(Dfltnotequal(r1.value.float_val, ((sF32)r2.value.int_val)));
}

static void PTN_DoubleArgExpr__evalfiCLE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.float_val<=((sF32)r2.value.int_val) );
}

static void PTN_DoubleArgExpr__evalfiCLT(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.float_val<((sF32)r2.value.int_val) );
}

static void PTN_DoubleArgExpr__evalfiCGE(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.float_val>=((sF32)r2.value.int_val) );
}

static void PTN_DoubleArgExpr__evalfiCGT(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(r1.value.float_val>((sF32)r2.value.int_val) );
}

static void PTN_DoubleArgExpr__evalfiAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sSI)r1.value.float_val)&r2.value.int_val );
}

static void PTN_DoubleArgExpr__evalfiOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((sSI)r1.value.float_val)|r2.value.int_val );
}

static void PTN_DoubleArgExpr__evalfiEOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   Beval1;
   Beval2;
   ret->initInt(((((sSI)r1.value.float_val))^(r2.value.int_val)) );
}

static void PTN_DoubleArgExpr__evalfiLAND(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   // Short-circuit evaluation
   Beval1;
   if(0 != (((sSI)r1.value.float_val)))
   {
      Beval2;
      ret->initInt(0 != r2.value.int_val);
   }
   else
   {
      ret->initInt(YAC_FALSE);
   }
}

static void PTN_DoubleArgExpr__evalfiLOR(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   Dtracest;
   // Short-circuit evaluation
   Beval1;
   if(0 == (((sSI)r1.value.float_val)))
   {
      Beval2;
      ret->initInt(0 != r2.value.int_val);
   }
   else
   {
      ret->initInt(YAC_TRUE);
   }
}

sUI PTN_DoubleArgExpr::getType(void) const {
   sUI t1 = expr1->getType();
   sUI t2 = expr2->getType();
   sUI t = t1 | (t2 << 4);
   switch(t)
   {
      default:
         return 0;
      case 0x11:
         return 1;
      case 0x22:
      case 0x21: // int-float
      case 0x12: // float-int
         switch(expr_operator)
         {
            default:
               return 2;
            case YAC_OP_MOD:
            case YAC_OP_SHL:
            case YAC_OP_SHR:
            case YAC_OP_CEQ:
            case YAC_OP_CNE:
            case YAC_OP_CLE:
            case YAC_OP_CLT:
            case YAC_OP_CGE:
            case YAC_OP_CGT:
            case YAC_OP_AND:
            case YAC_OP_OR:
            case YAC_OP_EOR:
            case YAC_OP_LAND:
            case YAC_OP_LOR:
               return 1;
         }
         // break

      case 0x44: // string-string
         switch(expr_operator)
         {
            default:
               return 4;
            case YAC_OP_MOD:
            case YAC_OP_SHL:
            case YAC_OP_SHR:
            case YAC_OP_CEQ:
            case YAC_OP_CNE:
            case YAC_OP_CLE:
            case YAC_OP_CLT:
            case YAC_OP_CGE:
            case YAC_OP_CGT:
            case YAC_OP_AND:
            case YAC_OP_OR:
            case YAC_OP_EOR:
            case YAC_OP_LAND:
            case YAC_OP_LOR:
               return 1;
         }
         // break
   }
}

Feval PTN_DoubleArgExpr::getEval(void) const {
   sUI t1;
   sUI t2;
   if(YAC_OP_BEQ != expr_operator && YAC_OP_BNE != expr_operator)
   {
      t1 = expr1->getType();
      t2 = expr2->getType();
   }
   else
   {
      // @(a) == @(b)  or  @(a) != @(b)
      PTN_BuiltinFun *fun1 = (PTN_BuiltinFun*)expr1;
      PTN_BuiltinFun *fun2 = (PTN_BuiltinFun*)expr2;
      t1 = fun1->expr->getType();
      t2 = fun2->expr->getType();
   }
   sUI t = t1 | (t2 << 4);
   switch(t)
   {
      default:
      case 0x00: // var-var
         switch(expr_operator)
         {
            default:
               return PTN_DoubleArgExpr__eval;
            case YAC_OP_BEQ:
               return PTN_DoubleArgExpr__eval_beq;
            case YAC_OP_BNE:
               return PTN_DoubleArgExpr__eval_bne;
         }
         break;
      case 0x11: // int-int
         switch(expr_operator)
         {
            default:
               return PTN_DoubleArgExpr__eval;
            case YAC_OP_BEQ:
               return PTN_DoubleArgExpr__evaliiBEQ;
            case YAC_OP_BNE:
               return PTN_DoubleArgExpr__evaliiBNE;
            case YAC_OP_ADD:
               return PTN_DoubleArgExpr__evaliiADD;
            case YAC_OP_SUB:
               return PTN_DoubleArgExpr__evaliiSUB;
            case YAC_OP_MUL:
               return PTN_DoubleArgExpr__evaliiMUL;
            case YAC_OP_DIV:
               return PTN_DoubleArgExpr__evaliiDIV;
            case YAC_OP_MOD:
               return PTN_DoubleArgExpr__evaliiMOD;
            case YAC_OP_SHL:
               return PTN_DoubleArgExpr__evaliiSHL;
            case YAC_OP_SHR:
               return PTN_DoubleArgExpr__evaliiSHR;
            case YAC_OP_CEQ:
               return PTN_DoubleArgExpr__evaliiCEQ;
            case YAC_OP_CNE:
               return PTN_DoubleArgExpr__evaliiCNE;
            case YAC_OP_CLE:
               return PTN_DoubleArgExpr__evaliiCLE;
            case YAC_OP_CLT:
               return PTN_DoubleArgExpr__evaliiCLT;
            case YAC_OP_CGE:
               return PTN_DoubleArgExpr__evaliiCGE;
            case YAC_OP_CGT:
               return PTN_DoubleArgExpr__evaliiCGT;
            case YAC_OP_AND:
               return PTN_DoubleArgExpr__evaliiAND;
            case YAC_OP_OR:
               return PTN_DoubleArgExpr__evaliiOR;
            case YAC_OP_EOR:
               return PTN_DoubleArgExpr__evaliiEOR;
            case YAC_OP_LAND:
               return PTN_DoubleArgExpr__evaliiLAND;
            case YAC_OP_LOR:
               return PTN_DoubleArgExpr__evaliiLOR;
         }
         break;

      case 0x22: // float-float
         switch(expr_operator)
         {
            default:
               return PTN_DoubleArgExpr__eval;
            case YAC_OP_BEQ:
               return PTN_DoubleArgExpr__evaliiBEQ;  // true if float bit patterns match
            case YAC_OP_BNE:
               return PTN_DoubleArgExpr__evaliiBNE;
            case YAC_OP_ADD:
               return PTN_DoubleArgExpr__evalffADD;
            case YAC_OP_SUB:
               return PTN_DoubleArgExpr__evalffSUB;
            case YAC_OP_MUL:
               return PTN_DoubleArgExpr__evalffMUL;
            case YAC_OP_DIV:
               return PTN_DoubleArgExpr__evalffDIV;
            case YAC_OP_MOD:
               return PTN_DoubleArgExpr__evalffMOD;
            case YAC_OP_SHL:
               return PTN_DoubleArgExpr__evalffSHL;
            case YAC_OP_SHR:
               return PTN_DoubleArgExpr__evalffSHR;
            case YAC_OP_CEQ:
               return PTN_DoubleArgExpr__evalffCEQ;
            case YAC_OP_CNE:
               return PTN_DoubleArgExpr__evalffCNE;
            case YAC_OP_CLE:
               return PTN_DoubleArgExpr__evalffCLE;
            case YAC_OP_CLT:
               return PTN_DoubleArgExpr__evalffCLT;
            case YAC_OP_CGE:
               return PTN_DoubleArgExpr__evalffCGE;
            case YAC_OP_CGT:
               return PTN_DoubleArgExpr__evalffCGT;
            case YAC_OP_AND:
               return PTN_DoubleArgExpr__evalffAND;
            case YAC_OP_OR:
               return PTN_DoubleArgExpr__evalffOR;
            case YAC_OP_EOR:
               return PTN_DoubleArgExpr__evalffEOR;
            case YAC_OP_LAND:
               return PTN_DoubleArgExpr__evalffLAND;
            case YAC_OP_LOR:
               return PTN_DoubleArgExpr__evalffLOR;
         }
         break;

      case 0x21: // int-float
         switch(expr_operator)
         {
            default:
               return PTN_DoubleArgExpr__eval;
            case YAC_OP_BEQ:
               return PTN_DoubleArgExpr__evaliiBEQ;  // (note) true if int and float bit patterns match
            case YAC_OP_BNE:
               return PTN_DoubleArgExpr__evaliiBNE;
            case YAC_OP_ADD:
               return PTN_DoubleArgExpr__evalifADD;
            case YAC_OP_SUB:
               return PTN_DoubleArgExpr__evalifSUB;
            case YAC_OP_MUL:
               return PTN_DoubleArgExpr__evalifMUL;
            case YAC_OP_DIV:
               return PTN_DoubleArgExpr__evalifDIV;
            case YAC_OP_MOD:
               return PTN_DoubleArgExpr__evalifMOD;
            case YAC_OP_SHL:
               return PTN_DoubleArgExpr__evalifSHL;
            case YAC_OP_SHR:
               return PTN_DoubleArgExpr__evalifSHR;
            case YAC_OP_CEQ:
               return PTN_DoubleArgExpr__evalifCEQ;
            case YAC_OP_CNE:
               return PTN_DoubleArgExpr__evalifCNE;
            case YAC_OP_CLE:
               return PTN_DoubleArgExpr__evalifCLE;
            case YAC_OP_CLT:
               return PTN_DoubleArgExpr__evalifCLT;
            case YAC_OP_CGE:
               return PTN_DoubleArgExpr__evalifCGE;
            case YAC_OP_CGT:
               return PTN_DoubleArgExpr__evalifCGT;
            case YAC_OP_AND:
               return PTN_DoubleArgExpr__evalifAND;
            case YAC_OP_OR:
               return PTN_DoubleArgExpr__evalifOR;
            case YAC_OP_EOR:
               return PTN_DoubleArgExpr__evalifEOR;
            case YAC_OP_LAND:
               return PTN_DoubleArgExpr__evalifLAND;
            case YAC_OP_LOR:
               return PTN_DoubleArgExpr__evalifLOR;
         }
         break;

      case 0x12: // float-int
         switch(expr_operator)
         {
            default:
               return PTN_DoubleArgExpr__eval;
            case YAC_OP_BEQ:
               return PTN_DoubleArgExpr__evaliiBEQ;  // (note) true if float and int bit patterns match
            case YAC_OP_BNE:
               return PTN_DoubleArgExpr__evaliiBNE;
            case YAC_OP_ADD:
               return PTN_DoubleArgExpr__evalfiADD;
            case YAC_OP_SUB:
               return PTN_DoubleArgExpr__evalfiSUB;
            case YAC_OP_MUL:
               return PTN_DoubleArgExpr__evalfiMUL;
            case YAC_OP_DIV:
               return PTN_DoubleArgExpr__evalfiDIV;
            case YAC_OP_MOD:
               return PTN_DoubleArgExpr__evalfiMOD;
            case YAC_OP_SHL:
               return PTN_DoubleArgExpr__evalfiSHL;
            case YAC_OP_SHR:
               return PTN_DoubleArgExpr__evalfiSHR;
            case YAC_OP_CEQ:
               return PTN_DoubleArgExpr__evalfiCEQ;
            case YAC_OP_CNE:
               return PTN_DoubleArgExpr__evalfiCNE;
            case YAC_OP_CLE:
               return PTN_DoubleArgExpr__evalfiCLE;
            case YAC_OP_CLT:
               return PTN_DoubleArgExpr__evalfiCLT;
            case YAC_OP_CGE:
               return PTN_DoubleArgExpr__evalfiCGE;
            case YAC_OP_CGT:
               return PTN_DoubleArgExpr__evalfiCGT;
            case YAC_OP_AND:
               return PTN_DoubleArgExpr__evalfiAND;
            case YAC_OP_OR:
               return PTN_DoubleArgExpr__evalfiOR;
            case YAC_OP_EOR:
               return PTN_DoubleArgExpr__evalfiEOR;
            case YAC_OP_LAND:
               return PTN_DoubleArgExpr__evalfiLAND;
            case YAC_OP_LOR:
               return PTN_DoubleArgExpr__evalfiLOR;
         }
         break;
   }

}

void PTN_DoubleArgExpr::EvalOp(PTN_Env *_env, YAC_Value *ret,
                               YAC_Value *r1, YAC_Value *r2,
                               sU8 _op,
                               const PTN_Node *_nodeOrNull
                               )
{
   // Called for string-string and by var assignments (=, +=, -=, ..)
   // // YAC_Value v1;
   // // YAC_Value v2;

   sU8 atc = (sU8) ( ((sU8)(r2->type<<3)) | ((sU8)r1->type) );

   switch(atc)
   {
      case 0:  // void-void
         ret->initVoid();
         break;
      case 1:  // int-void
         switch(_op)
         {
         case YAC_OP_CEQ:
         case YAC_OP_CLT:
         case YAC_OP_CLE:
         case YAC_OP_CGE:
         case YAC_OP_CGT:
            ret->initInt(0);
            break;
         case YAC_OP_CNE:
            ret->initInt(1);
            break;
         case YAC_OP_OR:
         case YAC_OP_EOR:
         case YAC_OP_ADD:
         case YAC_OP_SUB:
            ret->initInt(r1->value.int_val);
            break;
         default:
            ret->initVoid();
            r1->unset();
            break;
         }
         break;
      case 2:  // float-void
         switch(_op)
         {
         case YAC_OP_CEQ:
         case YAC_OP_CLT:
         case YAC_OP_CLE:
         case YAC_OP_CGE:
         case YAC_OP_CGT:
            ret->initInt(0);
            break;
         case YAC_OP_CNE:
            ret->initInt(1);
            break;
         case YAC_OP_ADD:
         case YAC_OP_SUB:
            ret->initFloat(r1->value.float_val);
            break;
         default:
            ret->initVoid();
            r1->unset();
            break;
         }
         break;
      case 3:  // object-void
         switch(_op)
         {
            default:
               ret->initVoid();
               r1->unset();
               break;
            case YAC_OP_CEQ:
               ret->initInt(r1->isNullOrIF0());
               r1->unset();
               break;
            case YAC_OP_CNE:
               ret->initInt(!r1->isNullOrIF0());
               r1->unset();
               break;
         }
         break;
      case 4:  // string-void
         if(TKS_VALID(r1->value.object_val))
         {
            if(r1->deleteme)
            {
               r1->value.string_val->append("<void>");
               r1->deleteme = 0;
               ret->type = YAC_TYPE_STRING;
               ret->value.string_val=r1->value.string_val;
               ret->deleteme = 1;
            }
            else
            {
               YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
               if(s)
               {
                  s->yacCopy(r1->value.string_val);
                  s->append("<void>");
                  ret->value.string_val=s;
                  ret->type = YAC_TYPE_STRING;
                  ret->deleteme = 1;
               }
            }
         }
         else
         {
            Drtthrowinvalidpointer(_nodeOrNull, "invalid left-hand side object", r1->value.object_val);
         }
         return;//ret;
      case 8:  // void-int
         switch(_op)
         {
         case YAC_OP_CEQ:
         case YAC_OP_CLT:
         case YAC_OP_CLE:
         case YAC_OP_CGE:
         case YAC_OP_CGT:
            ret->initInt(0);
            break;
         case YAC_OP_CNE:
            ret->initInt(1);
            break;
         case YAC_OP_OR:
         case YAC_OP_EOR:
         case YAC_OP_ADD:
            ret->initInt(r2->value.int_val);
            break;
         case YAC_OP_SUB:
            ret->initInt(-r2->value.int_val);
            break;
         default:
            r2->unset();
            ret->initVoid();
            break;
         }
         break;
      case 16: // void-float
         switch(_op)
         {
         case YAC_OP_CEQ:
         case YAC_OP_CLT:
         case YAC_OP_CLE:
         case YAC_OP_CGE:
         case YAC_OP_CGT:
            ret->initInt(0);
            break;
         case YAC_OP_CNE:
            ret->initInt(1);
            break;
         case YAC_OP_ADD:
            ret->initFloat(r2->value.float_val);
            break;
         case YAC_OP_SUB:
            ret->initFloat(-r2->value.float_val);
            break;
         default:
            r2->unset();
            ret->initVoid();
            break;
         }
         break;
      case 24: // void-object
      case 32: // void-string
         switch(_op)
         {
            default:
               ret->initObject(0,0);
               break;
            case YAC_OP_CEQ:
               ret->initInt( YAC_VALID(r2->value.object_val)==0 );
               break;
            case YAC_OP_CNE:
               ret->initInt( YAC_VALID(r2->value.object_val) );
               break;
         }
         return;// ret;
      case 9:  // int-int
     PTN_DoubleArgExpr_intint:
         switch(_op)
         {
            default:
            case YAC_OP_ASSIGN: // Called by TKScriptClassInstance::setClassMemberValue()
               ret->initInt(r2->value.int_val);
               break;
            case YAC_OP_ADD:
               ret->initInt(r1->value.int_val+r2->value.int_val);
               break;
            case YAC_OP_SUB:
               ret->initInt(r1->value.int_val-r2->value.int_val);
               break;
            case YAC_OP_MUL:
               ret->initInt(r1->value.int_val*r2->value.int_val);
               break;
            case YAC_OP_DIV:
               if(r2->value.int_val)
               {
                  ret->initInt(r1->value.int_val/r2->value.int_val);
               }
               break;
            case YAC_OP_MOD:
               if(r2->value.int_val)
               {
                  ret->initInt(r1->value.int_val%r2->value.int_val);
               }
               break;
            case YAC_OP_SHL:
               ret->initInt(r1->value.int_val<<r2->value.int_val);
               break;
            case YAC_OP_SHR:
               ret->initInt(r1->value.int_val>>r2->value.int_val);
               break;
            case YAC_OP_CEQ:
               ret->initInt(r1->value.int_val==r2->value.int_val);
               break;
            case YAC_OP_CNE:
               ret->initInt(r1->value.int_val!=r2->value.int_val);
               break;
            case YAC_OP_CLE:
               ret->initInt(r1->value.int_val<=r2->value.int_val);
               break;
            case YAC_OP_CLT:
               ret->initInt(r1->value.int_val<r2->value.int_val);
               break;
            case YAC_OP_CGE:
               ret->initInt(r1->value.int_val>=r2->value.int_val);
               break;
            case YAC_OP_CGT:
               ret->initInt(r1->value.int_val>r2->value.int_val);
               break;
            case YAC_OP_AND:
               ret->initInt(r1->value.int_val&r2->value.int_val);
               break;
            case YAC_OP_OR:
               ret->initInt(r1->value.int_val|r2->value.int_val);
               break;
            case YAC_OP_EOR:
               ret->initInt((r1->value.int_val)^(r2->value.int_val));
               break;
            case YAC_OP_LAND:
               ret->initInt((r1->value.int_val)&&(r2->value.int_val));
               break;
            case YAC_OP_LOR:
               ret->initInt((r1->value.int_val)||(r2->value.int_val));
               break;
         }
         break;
      case 17: // int-float
         switch(_op)
         {
            default:
            case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
               ret->initInt((sSI)r2->value.float_val);
               break;
            case YAC_OP_ADD:
               ret->initFloat(r1->value.int_val+r2->value.float_val);
               break;
            case YAC_OP_SUB:
               ret->initFloat(r1->value.int_val-r2->value.float_val);
               break;
            case YAC_OP_MUL:
               ret->initFloat(r1->value.int_val*r2->value.float_val);
               break;
            case YAC_OP_DIV:
               if(Dfltnonzero(r2->value.float_val))
                  ret->initFloat(r1->value.int_val/r2->value.float_val);
               else
                  ret->initFloat(0.0f);
               break;
            case YAC_OP_MOD:
               // // if(((sSI)r2->value.float_val))
               // //    ret->initInt(r1->value.int_val%((sSI)r2->value.float_val));
               if(0.0f != r2->value.float_val)
                  ret->initFloat(sF32(r1->value.int_val) - r2->value.float_val * floorf(sF32(r1->value.int_val)/r2->value.float_val));
               else
                  ret->initFloat(0.0f);
               break;
            case YAC_OP_SHL:
               ret->initInt(r1->value.int_val<<((sSI)r2->value.float_val));
               break;
            case YAC_OP_SHR:
               ret->initInt(r1->value.int_val>>((sSI)r2->value.float_val));
               break;
            case YAC_OP_CEQ:
               ret->initInt(r1->value.int_val==((sSI)r2->value.float_val));
               break;
            case YAC_OP_CNE:
               ret->initInt(r1->value.int_val!=((sSI)r2->value.float_val));
               break;
            case YAC_OP_CLE:
               ret->initInt(r1->value.int_val<=((sSI)r2->value.float_val));
               break;
            case YAC_OP_CLT:
               ret->initInt(r1->value.int_val<((sSI)r2->value.float_val));
               break;
            case YAC_OP_CGE:
               ret->initInt(r1->value.int_val>=((sSI)r2->value.float_val));
               break;
            case YAC_OP_CGT:
               ret->initInt(r1->value.int_val>((sSI)r2->value.float_val));
               break;
            case YAC_OP_AND:
               ret->initInt(r1->value.int_val&((sSI)r2->value.float_val));
               break;
            case YAC_OP_OR:
               ret->initInt(r1->value.int_val|((sSI)r2->value.float_val));
               break;
            case YAC_OP_EOR:
               ret->initInt( ((r1->value.int_val)^(((sSI)r2->value.float_val))) );
               break;
            case YAC_OP_LAND:
               ret->initInt( ((r1->value.int_val)&&(((sSI)r2->value.float_val))) );
               break;
            case YAC_OP_LOR:
               ret->initInt( ((r1->value.int_val)||(((sSI)r2->value.int_val))) );
               break;
         }
         break;


         // duplicate rvalue if it's "read-only"
#define Ddupr2 	YAC_Object *n; if(!r2->deleteme) \
               {\
               n=YAC_CLONE_POOLED(NULL, r2->value.object_val);\
               n->yacOperatorAssign(r2->value.object_val);\
      } else { n=r2->value.object_val; r2->deleteme=0; }

#define Ddupr1 	YAC_Object *n; if(!r1->deleteme) \
               {\
               n=YAC_CLONE_POOLED(NULL, r1->value.object_val);\
               n->yacOperatorAssign(r1->value.object_val);\
      } else { n=r1->value.object_val; r1->deleteme=0; }

#define Ddupr1ir2type YAC_Object *r1n=YAC_CLONE_POOLED(_env->context, r2->value.object_val); \
               r1n->yacValueOfI(r1->value.int_val);

#define Ddupr2ir1type YAC_Object *r2n=YAC_CLONE_POOLED(_env->context, r1->value.object_val); \
               r2n->yacValueOfI(r2->value.int_val);

#define Ddupr1fr2type YAC_Object *r1n=YAC_CLONE_POOLED(_env->context, r2->value.object_val); \
               r1n->yacValueOfF32(r1->value.float_val);

#define Ddupr2fr1type YAC_Object *r2n=YAC_CLONE_POOLED(_env->context, r1->value.object_val); \
               r2n->yacValueOfF32(r2->value.float_val);

      case 25: // int-object
         if(YAC_VALID(r2->value.object_val))
         {
            if(r2->value.object_val->yacOperatorPriority()<YAC_OP_PRIO_INTEGER)
            {
               sSI iv; r2->value.object_val->yacScanI(&iv);
               r2->unset();
               r2->initInt(iv);
               goto PTN_DoubleArgExpr_intint;
            }
            sSI iv;
            switch(_op)
            {
               default:
               case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
                  r2->value.object_val->yacScanI(&iv);
                  ret->initInt(iv);
                  r2->unset();
                  break;
               case YAC_OP_ADD:
               {
                  Ddupr2;
                  n->yacOperatorI(YAC_OP_ADD, r1->value.int_val, ret);
                  ret->initObject(n, 1);
               }
               break;
               case YAC_OP_SUB:
               {
                  Ddupr1ir2type;
                  r1n->yacOperator(YAC_OP_SUB, r2->value.object_val, ret);
                  ret->initObject(r1n, 1);
                  r2->unset();
               }
               break;
               case YAC_OP_MUL:
               {
                  Ddupr2;
                  n->yacOperatorI(YAC_OP_MUL, r1->value.int_val, ret);
                  ret->initObject(n, 1);
               }
               break;
               case YAC_OP_DIV:
               {
                  Ddupr1ir2type;
                  r1n->yacOperator(YAC_OP_DIV, r2->value.object_val, ret);
                  ret->initObject(r1n, 1);
                  r2->unset();
               }
               break;
               case YAC_OP_MOD:
               {
                  r2->value.object_val->yacScanI(&iv);
                  if(iv)
                     ret->initInt( r1->value.int_val % iv);
                  else
                     ret->initInt(0);
                  r2->unset();
               }
               break;
               case YAC_OP_SHL:
               {
                  Ddupr1ir2type;
                  r1n->yacOperator(YAC_OP_SHL, r2->value.object_val, ret);
                  ret->initObject(r1n, 1);
                  r2->unset();
               }
               break;
               case YAC_OP_SHR:
               {
                  Ddupr1ir2type;
                  r1n->yacOperator(YAC_OP_SHR, r2->value.object_val, ret);
                  ret->initObject(r1n, 1);
                  r2->unset();
               }
               break;
               case YAC_OP_CEQ:
               {
                  r2->value.object_val->yacOperatorI(YAC_OP_CEQ, r1->value.int_val, ret);
                  r2->unset();
               }
               break;
               case YAC_OP_CNE:
               {
                  r2->value.object_val->yacOperatorI(YAC_OP_CNE, r1->value.int_val, ret);
                  r2->unset();
               }
               break;
               case YAC_OP_CLE:
               {
                  r2->value.object_val->yacOperatorI(YAC_OP_CGE, r1->value.int_val, ret); // swap op
                  r2->unset();
               }
               break;
               case YAC_OP_CLT:
               {
                  r2->value.object_val->yacOperatorI(YAC_OP_CGT, r1->value.int_val, ret); // swap op
                  r2->unset();
               }
               break;
               case YAC_OP_CGE:
               {
                  r2->value.object_val->yacOperatorI(YAC_OP_CLE, r1->value.int_val, ret); // swap op
                  r2->unset();
               }
               break;
               case YAC_OP_CGT:
               {
                  r2->value.object_val->yacOperatorI(YAC_OP_CLT, r1->value.int_val, ret); // swap op
                  r2->unset();
               }
               break;
               case YAC_OP_AND:
               {
                  r2->value.object_val->yacScanI(&iv);
                  ret->initInt( r1->value.int_val & iv);
                  r2->unset();
               }
               break;
               case YAC_OP_OR:
               {
                  Ddupr2;
                  n->yacOperatorI(YAC_OP_OR, r1->value.int_val, ret);
                  ret->initObject(n, 1);
               }
               break;
               case YAC_OP_EOR:
               {
                  Ddupr1ir2type;
                  r1n->yacOperator(YAC_OP_EOR, r2->value.object_val, ret);
                  ret->initObject(r1n, 1);
                  r2->unset();
               }
               break;
               case YAC_OP_LAND:
                  r2->value.object_val->yacOperatorI(YAC_OP_LAND, r1->value.int_val, ret);
                  r2->unset();
                  break;
               case YAC_OP_LOR:
                  r2->value.object_val->yacOperatorI(YAC_OP_LOR, r1->value.int_val, ret);
                  r2->unset();
                  break;
            }

         } // YAC_VALID(r2)
         else
         {
            ret->initInt(r1->value.int_val);
         }
         break;
      case 33: // int-string
         switch(_op)
         {
            default:
            case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
               if(YAC_VALID(r2->value.object_val))
               {
                  r2->value.string_val->yacScanI(&r1->value.int_val);
                  ret->initInt((sSI)r1->value.int_val);
                  r2->unset();
               }
               break;
            case YAC_OP_ADD:
               if(YAC_VALID(r2->value.object_val))
               {
                  YAC_String *ts = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
                  ts->yacValueOfI(r1->value.int_val);
                  ts->append(r2->value.string_val);
                  ret->initString(ts, 1);
                  r2->unset();
               }
               else
               {
                  ret->initInt(r1->value.int_val);
               }
               break;
            case YAC_OP_SUB:
               if(YAC_VALID(r2->value.object_val))
               {
                  sSI iv;
                  if(r2->value.string_val->yacScanI(&iv))
                     ret->initInt(r1->value.int_val-iv);
                  else
                     ret->initInt(r1->value.int_val);
                  r2->unset();
               }
               else
               {
                  ret->initInt(r1->value.int_val);
               }
               break;
            case YAC_OP_MUL:
#if 0
               // pre 31Jan2020 (v0.9.64.2)
               if(YAC_VALID(r2->value.object_val))
               {
                  sSI iv;
                  if(r2->value.string_val->yacScanI(&iv))
                     ret->initInt(r1->value.int_val*iv);
                  else
                     ret->initInt(0);
                  r2->unset();
               }
               else
               {
                  ret->initInt(r1->value.int_val);
               }
#else
               ret->initEmptyString();
               if(YAC_VALID(r2->value.object_val))
               {
                  if(r1->value.int_val > 0)
                  {
                     ret->value.string_val->_appendRepeat_YAC_RSELF(r2->value.string_val, r1->value.int_val);
                  }
                  r2->unset();
               }
               // (note) else: no error when RHS is null
#endif
               break;
            case YAC_OP_DIV:
               if(YAC_VALID(r2->value.object_val))
               {
                  sSI iv;
                  if(r2->value.string_val->yacScanI(&iv))
                     ret->initInt((iv!=0)?r1->value.int_val/iv:0 );
                  else
                     ret->initInt(0);//Dqintval(ret, 0);
                  r2->unset();
               }
               else
               {
                  ret->initInt(r1->value.int_val);
               }
               break;
            case YAC_OP_MOD:
               if(YAC_VALID(r2->value.object_val))
               {
                  sSI iv;
                  if(r2->value.string_val->yacScanI(&iv))
                     ret->initInt(
                        (((sSI)iv)!=0)
                        ?
                        ((sSI)r1->value.int_val)%((sSI)iv):0
                        );
                  else
                     ret->initInt(0);
                  r2->unset();
               }
               else
               {
                  ret->initInt(r1->value.int_val);
               }
               break;
         }
         break;
      case 10: // float-int
         switch(_op)
         {
            default:
            case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
               ret->initFloat((sF32)r2->value.int_val);
               break;
            case YAC_OP_ADD:
               ret->initFloat(r1->value.float_val+r2->value.int_val);
               break;
            case YAC_OP_SUB:
               ret->initFloat(r1->value.float_val-r2->value.int_val);
               break;
            case YAC_OP_MUL:
               ret->initFloat(r1->value.float_val*r2->value.int_val);
               break;
            case YAC_OP_DIV:
               if(r2->value.int_val)
                  ret->initFloat(r1->value.float_val/((sF32)r2->value.int_val) );
               break;
            case YAC_OP_MOD:
               // // if(r2->value.int_val)
               // //    ret->initInt(((sSI)r1->value.float_val)%r2->value.int_val );
               if(r2->value.int_val)
                  ret->initFloat(r1->value.float_val - sF32(r2->value.int_val) * floorf(r1->value.float_val/sF32(r2->value.int_val)));
               else
                  ret->initFloat(0.0f);
               break;
            case YAC_OP_SHL:
               ret->initInt(((sSI)r1->value.float_val)<<r2->value.int_val );
               break;
            case YAC_OP_SHR:
               ret->initInt(((sSI)r1->value.float_val)>>r2->value.int_val );
               break;
            case YAC_OP_CEQ:
               ret->initInt(Dfltequal(r1->value.float_val, ((sF32)r2->value.int_val)));
               break;
            case YAC_OP_CNE:
               ret->initInt(Dfltnotequal(r1->value.float_val, ((sF32)r2->value.int_val)));
               break;
            case YAC_OP_CLE:
               ret->initInt(r1->value.float_val<=((sF32)r2->value.int_val) );
               break;
            case YAC_OP_CLT:
               ret->initInt(r1->value.float_val<((sF32)r2->value.int_val) );
               break;
            case YAC_OP_CGE:
               ret->initInt(r1->value.float_val>=((sF32)r2->value.int_val) );
               break;
            case YAC_OP_CGT:
               ret->initInt(r1->value.float_val>((sF32)r2->value.int_val) );
               break;
            case YAC_OP_AND:
               ret->initInt(((sSI)r1->value.float_val)&r2->value.int_val );
               break;
            case YAC_OP_OR:
               ret->initInt(((sSI)r1->value.float_val)|r2->value.int_val );
               break;
            case YAC_OP_EOR:
               ret->initInt(((((sSI)r1->value.float_val))^(r2->value.int_val)) );
               break;
            case YAC_OP_LAND:
               ret->initInt(((((sSI)r1->value.float_val))&&(r2->value.int_val)) );
               break;
            case YAC_OP_LOR:
               ret->initInt(((((sSI)r1->value.float_val))||(r2->value.int_val)) );
               break;
         }
         break;
      case 18: // float-float
     PTN_DoubleArgExpr_floatfloat:
         switch(_op)
         {
            default:
            case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
               ret->initFloat((sF32)r2->value.float_val);
               break;
            case YAC_OP_ADD:
               ret->initFloat(r1->value.float_val+r2->value.float_val);
               break;
            case YAC_OP_SUB:
               ret->initFloat(r1->value.float_val-r2->value.float_val );
               break;
            case YAC_OP_MUL:
               ret->initFloat(r1->value.float_val*r2->value.float_val);
               break;
            case YAC_OP_DIV:
               if(Dfltnonzero(r2->value.float_val))
                  ret->initFloat(r1->value.float_val/r2->value.float_val);
               else
                  ret->initFloat(0.0f);
               break;
            case YAC_OP_MOD:
               ////if(((sSI)r2->value.float_val))
               ////   ret->initInt(((sSI)r1->value.float_val)%((sSI)r2->value.float_val) );
               if(r2->value.float_val != 0.0f)
                  ret->initFloat(r1->value.float_val - r2->value.float_val * floorf(r1->value.float_val / r2->value.float_val));
               else
                  ret->initFloat(0.0f);
               break;
            case YAC_OP_SHL:
               ret->initInt(((sSI)r1->value.float_val)<<((sSI)r2->value.float_val) );
               break;
            case YAC_OP_SHR:
               ret->initInt(((sSI)r1->value.float_val)>>((sSI)r2->value.float_val) );
               break;
            case YAC_OP_CEQ:
               ret->initInt(Dfltequal(r1->value.float_val, r2->value.float_val));
               break;
            case YAC_OP_CNE:
               ret->initInt(Dfltnotequal(r1->value.float_val, r2->value.float_val));
               break;
            case YAC_OP_CLE:
               ret->initInt(r1->value.float_val<=r2->value.float_val);
               break;
            case YAC_OP_CLT:
               ret->initInt(r1->value.float_val<r2->value.float_val);
               break;
            case YAC_OP_CGE:
               ret->initInt(r1->value.float_val>=r2->value.float_val);
               break;
            case YAC_OP_CGT:
               ret->initInt(r1->value.float_val>r2->value.float_val);
               break;
            case YAC_OP_AND:
               ret->initInt(((sSI)r1->value.float_val)&((sSI)r2->value.float_val) );
               break;
            case YAC_OP_OR:
               ret->initInt(((sSI)r1->value.float_val)|((sSI)r2->value.float_val) );
               break;
            case YAC_OP_EOR:
               ret->initInt(((((sSI)r1->value.float_val))^(((sSI)r2->value.float_val))) );
               break;
            case YAC_OP_LAND:
               ret->initInt(((((sSI)r1->value.float_val))&&(((sSI)r2->value.float_val))) );
               break;
            case YAC_OP_LOR:
               ret->initInt((((sSI)r1->value.float_val))||(((sSI)r2->value.float_val)) );
               break;
         }
         break;
      case 26: // float-object
         if(YAC_VALID(r2->value.object_val))
         {
            if(r2->value.object_val->yacOperatorPriority()<YAC_OP_PRIO_FLOAT)
            {
               sF32 fv; r2->value.object_val->yacScanF32(&fv);
               r2->unset();
               r2->initFloat(fv);
               goto PTN_DoubleArgExpr_floatfloat;
            }

            sF32 fv;
            switch(_op)
            {
               default:
               case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
                  r2->value.object_val->yacScanF32(&fv);
                  ret->initFloat(fv);
                  r2->unset();
                  break;
               case YAC_OP_ADD:
               {
                  Ddupr2;
                  n->yacOperatorF32(YAC_OP_ADD, r1->value.float_val, ret);
                  ret->initObject(n, 1);
               }
               break;
               case YAC_OP_SUB:
               {
                  Ddupr1fr2type;
                  r1n->yacOperator(YAC_OP_SUB, r2->value.object_val, ret);
                  ret->initObject(r1n, 1);
                  r2->unset();
               }
               break;
               case YAC_OP_MUL:
               {
                  Ddupr2;
                  n->yacOperatorF32(YAC_OP_MUL, r1->value.float_val, ret);
                  ret->initObject(n, 1);
               }
               break;
               case YAC_OP_DIV:
               {
                  Ddupr1fr2type;
                  r1n->yacOperator(YAC_OP_DIV, r2->value.object_val, ret);
                  ret->initObject(r1n, 1);
                  r2->unset();
               }
               break;
               case YAC_OP_MOD:
               {
                  sSI iv;
                  r2->value.object_val->yacScanI(&iv);
                  if(iv)
                     ret->initInt( ((sSI)r1->value.float_val) % iv);
                  else
                     ret->initInt(0);
                  r2->unset();
               }
               break;
               case YAC_OP_SHL:
               {
                  Ddupr1fr2type;
                  r1n->yacOperator(YAC_OP_SHL, r2->value.object_val, ret);
                  ret->initObject(r1n, 1);
                  r2->unset();
               }
               break;
               case YAC_OP_SHR:
               {
                  Ddupr1fr2type;
                  r1n->yacOperator(YAC_OP_SHR, r2->value.object_val, ret);
                  ret->initObject(r1n, 1);
                  r2->unset();
               }
               break;
               case YAC_OP_CEQ:
               {
                  r2->value.object_val->yacOperatorF32(YAC_OP_CEQ, r1->value.float_val, ret);
                  r2->unset();
               }
               break;
               case YAC_OP_CNE:
               {
                  r2->value.object_val->yacOperatorF32(YAC_OP_CNE, r1->value.float_val, ret);
                  r2->unset();
               }
               break;
               case YAC_OP_CLE:
               {
                  r2->value.object_val->yacOperatorF32(YAC_OP_CGE, r1->value.float_val, ret); // swap op
                  r2->unset();
               }
               break;
               case YAC_OP_CLT:
               {
                  r2->value.object_val->yacOperatorF32(YAC_OP_CGT, r1->value.float_val, ret); // swap op
                  r2->unset();
               }
               break;
               case YAC_OP_CGE:
               {
                  r2->value.object_val->yacOperatorF32(YAC_OP_CLE, r1->value.float_val, ret); // swap op
                  r2->unset();
               }
               break;
               case YAC_OP_CGT:
               {
                  r2->value.object_val->yacOperatorF32(YAC_OP_CLT, r1->value.float_val, ret); // swap op
                  r2->unset();
               }
               break;
               case YAC_OP_AND:
               {
                  sSI iv;
                  r2->value.object_val->yacScanI(&iv);
                  ret->initInt( r1->value.int_val & iv);
                  r2->unset();
               }
               break;
               case YAC_OP_OR:
               {
                  Ddupr2;
                  n->yacOperatorF32(YAC_OP_OR, r1->value.float_val, ret);
                  ret->initObject(n, 1);
               }
               break;
               case YAC_OP_EOR:
               {
                  Ddupr1fr2type;
                  r1n->yacOperator(YAC_OP_EOR, r2->value.object_val, ret);
                  ret->initObject(r1n, 1);
                  r2->unset();
               }
               break;
               case YAC_OP_LAND:
                  r2->value.object_val->yacOperatorF32(YAC_OP_LAND, r1->value.float_val, ret);
                  r2->unset();
                  break;
               case YAC_OP_LOR:
                  r2->value.object_val->yacOperatorF32(YAC_OP_LOR, r1->value.float_val, ret);
                  r2->unset();
                  break;
            }

         } // YAC_VALID
         else
         {
            ret->initFloat(r1->value.float_val);
         }
         break;
      case 34: // float-string
         switch(_op)
         {
            default:
            case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
               if(YAC_VALID(r2->value.object_val))
               {
                  r2->value.string_val->yacScanF32(&r1->value.float_val);
                  ret->initFloat((sF32)r1->value.float_val);
                  r2->unset();
               }
               break;
            case YAC_OP_ADD:
            {
               YAC_String *ts = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
               ts->yacValueOfF32(r1->value.float_val);
               ts->_append_YAC_RSELF(r2->value.string_val);
               ret->initString(ts, 1);
               r2->unset();
            }
            break;
            case YAC_OP_SUB:
               if(YAC_VALID(r2->value.object_val))
               {
                  sF32 fv;
                  if(r2->value.string_val->yacScanF32(&fv))
                     ret->initFloat(r1->value.float_val-fv);
                  else
                     ret->initFloat(r1->value.float_val);
                  r2->unset();
               }
               else
               {
                  ret->initFloat(r1->value.float_val);
               }
               break;
            case YAC_OP_MUL:
               if(YAC_VALID(r2->value.object_val))
               {
                  sF32 fv;
                  if(r2->value.string_val->yacScanF32(&fv))
                     ret->initFloat(r1->value.float_val*fv);
                  else
                     ret->initFloat(0.0f);
                  r2->unset();
               }
               else
               {
                  ret->initFloat(r1->value.float_val);
               }
               break;
            case YAC_OP_DIV:
               if(YAC_VALID(r2->value.object_val))
               {
                  sF32 fv;
                  if(r2->value.string_val->yacScanF32(&fv))
                     ret->initFloat((Dfltnonzero(fv))?r1->value.float_val/fv:0 );
                  else
                     ret->initFloat(0.0f);
                  r2->unset();
               }
               else
               {
                  ret->initFloat(r1->value.float_val);
               }
               break;
            case YAC_OP_MOD:
               if(YAC_VALID(r2->value.object_val))
               {
                  sF32 fv;
                  if(r2->value.string_val->yacScanF32(&fv))
                     ret->initInt(
                        (((sSI)fv)!=0)
                        ?
                        ((sSI)r1->value.float_val)%((sSI)fv):0
                        );
                  else
                     ret->initInt(0);
                  r2->unset();
               }
               else
               {
                  ret->initFloat(r1->value.float_val);
               }
               break;
            case YAC_OP_SHL:
               r2->unset();
               break;
            case YAC_OP_SHR:
               r2->unset();
               break;
            case YAC_OP_CEQ:
               if(YAC_VALID(r2->value.object_val))
               {
                  sF32 fv;
                  if(r2->value.string_val->yacScanF32(&fv))
                     ret->initInt(Dfltequal(r1->value.float_val, fv));
                  else
                     ret->initInt(0);
                  r2->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CNE:
               if(YAC_VALID(r2->value.object_val))
               {
                  sF32 fv;
                  if(r2->value.string_val->yacScanF32(&fv))
                     ret->initInt(Dfltnotequal(r1->value.float_val, fv));
                  else
                     ret->initInt(1);
                  r2->unset();
               }
               else
               {
                  ret->initInt(1);
               }
               break;
            case YAC_OP_CLE:
               if(YAC_VALID(r2->value.object_val))
               {
                  sF32 fv;
                  if(r2->value.string_val->yacScanF32(&fv))
                     ret->initInt(r1->value.float_val<=fv);
                  else
                     ret->initInt(0);
                  r2->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CLT:
               if(YAC_VALID(r2->value.object_val))
               {
                  sF32 fv;
                  if(r2->value.string_val->yacScanF32(&fv))
                     ret->initInt(r1->value.float_val<fv);
                  else
                     ret->initInt(0);
                  r2->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CGE:
               if(YAC_VALID(r2->value.object_val))
               {
                  sF32 fv;
                  if(r2->value.string_val->yacScanF32(&fv))
                     ret->initInt(r2->value.float_val>=fv);
                  else
                     ret->initInt(0);
                  r2->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CGT:
               if(YAC_VALID(r2->value.object_val))
               {
                  sF32 fv;
                  if(r2->value.string_val->yacScanF32(&fv))
                     ret->initInt(r1->value.float_val>fv);
                  else
                     ret->initInt( 0);
                  r2->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_AND:
               r2->unset();
               break;
            case YAC_OP_OR:
               r2->unset();
               break;
            case YAC_OP_EOR:
               r2->unset();
               break;
            case YAC_OP_LAND:
               r2->unset();
               break;
            case YAC_OP_LOR:
               r2->unset();
               break;
         }
         break;
      case 11: // object-int
         if(TKS_VALID(r1->value.object_val))
         {
            if(r1->value.object_val->yacOperatorPriority()<YAC_OP_PRIO_INTEGER)
            {
               sSI iv; r1->value.object_val->yacScanI(&iv);
               r1->unset();
               r1->initInt(iv);
               goto PTN_DoubleArgExpr_intint;
            }
            switch(_op)
            {
               default:
               case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
                  r1->value.object_val->yacValueOfI(r2->value.int_val);
                  ret->copySafe(r1);
                  break;
               case YAC_OP_ADD:
               case YAC_OP_SUB:
               case YAC_OP_MUL:
               case YAC_OP_DIV:
               case YAC_OP_MOD:
               case YAC_OP_SHL:
               case YAC_OP_SHR:
               case YAC_OP_AND:
               case YAC_OP_OR:
               case YAC_OP_EOR:
               {
                  Ddupr1;
                  n->yacOperatorI(_op, r2->value.int_val, ret);
                  if(ret->type==YAC_TYPE_VOID)
                  {
                     ret->initObject(n, 1);
                  }
                  else
                  {
                     YAC_DELETE(n);
                  }
               }
               break;
               case YAC_OP_CEQ:
               case YAC_OP_CNE:
               case YAC_OP_CLE:
               case YAC_OP_CLT:
               case YAC_OP_CGE:
               case YAC_OP_CGT:
               {
                  r1->value.object_val->yacOperatorI(_op, r2->value.int_val, ret);
                  r1->unset();
               }
               break;
               case YAC_OP_LAND:
               case YAC_OP_LOR:
                  r1->value.object_val->yacOperatorI(_op, r2->value.int_val, ret);
                  r1->unset();
                  break;
            }

         }
         else
         {
            Drtthrowinvalidpointer(_nodeOrNull, "invalid left-hand side object", r1->value.object_val);
         }
         break;
      case 19: // object-float
         if(TKS_VALID(r1->value.object_val))
         {
            if(r1->value.object_val->yacOperatorPriority()<YAC_OP_PRIO_FLOAT)
            {
               sF32 fv; r1->value.object_val->yacScanF32(&fv);
               r1->unset();
               r1->initFloat(fv);
               goto PTN_DoubleArgExpr_floatfloat;
            }

            switch(_op)
            {
               default:
               case YAC_OP_ASSIGN: // Called by TKScriptClassInstance::setClassMemberValue()
                  r1->value.object_val->yacValueOfF32(r2->value.float_val);
                  ret->copySafe(r1);
                  break;
               case YAC_OP_ADD:
               case YAC_OP_SUB:
               case YAC_OP_MUL:
               case YAC_OP_DIV:
               case YAC_OP_MOD:
               case YAC_OP_SHL:
               case YAC_OP_SHR:
               case YAC_OP_AND:
               case YAC_OP_OR:
               case YAC_OP_EOR:
               {
                  Ddupr1;
                  n->yacOperatorF32(_op, r2->value.float_val, ret);
                  if(ret->type == YAC_TYPE_VOID)
                  {
                     ret->initObject(n, 1);
                  }
                  else
                  {
                     YAC_DELETE(n);
                  }
               }
               break;
               case YAC_OP_CEQ:
               case YAC_OP_CNE:
               case YAC_OP_CLE:
               case YAC_OP_CLT:
               case YAC_OP_CGE:
               case YAC_OP_CGT:
               {
                  r1->value.object_val->yacOperatorF32(_op, r2->value.float_val, ret);
                  r1->unset();
               }
               break;
               case YAC_OP_LAND:
               case YAC_OP_LOR:
                  r1->value.object_val->yacOperatorF32(_op, r2->value.float_val, ret);
                  r1->unset();
                  break;
            }

         }
         else
         {
            Drtthrowinvalidpointer(_nodeOrNull, "invalid left-hand side object", r1->value.object_val);
         }

         break;
      case 27: // object-object
      case 35: // object-string
         switch(_op)
         {
            case YAC_OP_CEQ:
               if(YAC_VALID(r1->value.object_val))
               {
                  r1->value.object_val->yacOperator(_op, r2->value.object_val, ret);
                  r1->unset();
               }
               else
               {
                  ret->initInt( YAC_VALID(r2->value.object_val) ==0 );
               }
               r2->unset();
               break;
            case YAC_OP_CNE:
               if(YAC_VALID(r1->value.object_val))
               {
                  r1->value.object_val->yacOperator(_op, r2->value.object_val, ret);
                  r1->unset();
               }
               else
               {
                  ret->initInt( YAC_VALID(r2->value.object_val) ==1 );
               }
               r2->unset();
               break;
            case YAC_OP_CLE:
            case YAC_OP_CLT:
            case YAC_OP_CGE:
            case YAC_OP_CGT:
               if(YAC_VALID(r1->value.object_val))
               {
                  r1->value.object_val->yacOperator(_op, r2->value.object_val, ret);
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               r2->unset();
               break;

            case YAC_OP_SHL: //object<<object, manipulate left handside object, do not create copies
            {
               if(YAC_VALID(r1->value.object_val))
               {
                  r1->value.object_val->yacOperator(_op, r2->value.object_val, ret);
                  r1->unset();
               }
               r2->unset();
            }
            break;
            case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
               if(YAC_VALID(r1->value.object_val))
               {
                  r1->value.object_val->yacOperatorAssign(r2->value.object_val);
               }
               r2->unset();

               break;

            default:
            case YAC_OP_ADD: //object+object, spawn new object if necessary
            case YAC_OP_SUB:
            case YAC_OP_MUL:
            case YAC_OP_DIV:
            case YAC_OP_MOD:
            case YAC_OP_AND:
            case YAC_OP_OR:
            case YAC_OP_EOR:
               if(TKS_VALID(r1->value.object_val))
               {
                  if(YAC_VALID(r2->value.object_val))
                  {
                     sUI prio1=r1->value.object_val->yacOperatorPriority();
                     sUI prio2=r2->value.object_val->yacOperatorPriority();

                     if((prio1 < YAC_OP_PRIO_INTEGER) && (prio2 < YAC_OP_PRIO_INTEGER) &&
                        (prio1 > YAC_OP_PRIO_NONE)    && (prio2 > YAC_OP_PRIO_NONE)
                        )
                     {
                        // result has 32bit integer type (e.g. Short*Short)
                        //  (todo) create unsigned object when both r1+r2 are unsigned
                        YAC_Object *ro = YAC_NEW_CORE_POOLED(YAC_CLID_INTEGER);
                        ro->yacOperatorAssign(r1->value.object_val);
                        ro->yacOperator(_op, r2->value.object_val, ret);
                        if(ret->type==YAC_TYPE_VOID)
                        {
                           ret->initObject(ro, 1);
                        }
                        else
                        {
                           YAC_DELETE(ro);
                        }
                        r1->unset();
                        r2->unset();
                     }
                     else if(prio2 > prio1)
                     {
                        // ---- result has r2 type ----
                        YAC_Object *ro = YAC_CLONE_POOLED(_env->context, r2->value.object_val);
                        ro->yacOperatorAssign(r1->value.object_val);
                        ro->yacOperator(_op, r2->value.object_val, ret);
                        if(ret->type==YAC_TYPE_VOID)
                        {
                           ret->initObject(ro, 1);
                        }
                        else
                        {
                           YAC_DELETE(ro);
                        }
                        r1->unset();
                        r2->unset();
                     }
                     else
                     {
                        // ---- result has r1 type ----
                        YAC_Object *to;
                        YAC_Value opr;
                        if(r1->deleteme)
                        {
                           to=r1->value.object_val; // use existing temp object
                           r1->deleteme=0;
                        }
                        else
                        {
                           to = YAC_CLONE_POOLED(_env->context, r1->value.object_val);
                           if(to) to->yacOperatorAssign(r1->value.object_val);
                        }
                        if(to)
                        {
                           to->yacOperator(_op, r2->value.object_val, &opr);
                           if(opr.type==YAC_TYPE_VOID) // new: support for object1xobject2 !=object1
                           {
                              ret->initObject(to, 1);
                           }
                           else
                           {
                              YAC_DELETE(to);
                              ret->copySafe(&opr);
                           }
                        }
                        else
                           ret->initVoid();
                        r2->unset();
                        opr.unsetFast();
                     }
                  }
               }
               else
               {
                  r2->unset();
                  Drtthrowinvalidpointer(_nodeOrNull, "invalid left-hand side object", r1->value.object_val);
               }
               break;

         }
         break;
      case 12: // string-int
         switch(_op)
         {
            default:
            case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
               if(TKS_VALID(r1->value.object_val))
               {
                  r1->value.string_val->yacValueOfI(r2->value.int_val);
               }
               else
               {
                  Drtthrowinvalidpointer(_nodeOrNull, "invalid left-hand side string", r1->value.object_val);
               }
               break;

            case YAC_OP_ADD:
            {
               YAC_String t;
               t.yacValueOfI(r2->value.int_val);
               YAC_String *ts = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
               ts->yacCopy(r1->value.string_val);
               ts->append(&t);
               ret->initString(ts, 1);
               r1->unset();
            }
            break;
            case YAC_OP_SUB:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(iv-r2->value.int_val);
                  }
                  else
                  {
                     ret->initInt(-r2->value.int_val);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(r2->value.int_val);
               }
               break;
            case YAC_OP_MUL:
#if 0
               if(YAC_VALID(r1->value.object_val))
               {
                  // pre 31Jan2020 (0.9.64.2)
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(iv*r2->value.int_val);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(r2->value.int_val);
               }
#else
               if(YAC_VALID(r1->value.object_val))
               {
                  ret->initEmptyString();
                  if(r2->value.int_val > 0)
                  {
                     ret->value.string_val->_appendRepeat_YAC_RSELF(r1->value.string_val, r2->value.int_val);
                  }
                  r1->unset();
               }
               else
               {
                  Drtthrowinvalidpointer(_nodeOrNull, "invalid left-hand side string", r1->value.object_val);
               }
#endif
               break;
            case YAC_OP_DIV:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(r2->value.int_val?iv/r2->value.int_val:0);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_MOD:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(r2->value.int_val?iv%r2->value.int_val:0);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_SHL:
               if(YAC_VALID(r1->value.object_val))
               {   // copy r1 from 0 to r2
                  if(r2->value.int_val>=0)
                  {
                     ret->initEmptyString();
                     r1->value.string_val->substring(ret->value.string_val, 0, r2->value.int_val);
                  }
                  else
                  {
                     ret->initNewString(r1->value.string_val);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initEmptyString();
               }
               break;
            case YAC_OP_SHR:
               if(YAC_VALID(r1->value.object_val))
               {   // copy r1 from r2 to end
                  if(r2->value.int_val>=0)
                  {
                     ret->initEmptyString();
                     r1->value.string_val->substring(ret->value.string_val,
                                                     r2->value.int_val, r1->value.string_val->length-r2->value.int_val);
                  }
                  else
                  {
                     ret->initEmptyString();
                  }
                  r1->unset();
               }
               else
               {
                  ret->initEmptyString();
               }
               break;
            case YAC_OP_CEQ:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(iv==r2->value.int_val);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CNE:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(iv!=r2->value.int_val);
                  }
                  else
                  {
                     ret->initInt(1);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(1);
               }
               break;
            case YAC_OP_CLE:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(iv<=r2->value.int_val);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CLT:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(iv<r2->value.int_val);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CGE:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(iv>=r2->value.int_val);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CGT:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(iv>r2->value.int_val);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_AND:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(iv&r2->value.int_val);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_OR:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(iv|r2->value.int_val);
                  }
                  else
                  {
                     ret->initInt(r2->value.int_val);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_EOR:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt(iv^r2->value.int_val);
                  }
                  else
                  {
                     ret->initInt(r2->value.int_val);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_LAND:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt((iv)&&(r2->value.int_val));
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_LOR:
               if(YAC_VALID(r1->value.object_val))
               {
                  sSI iv;
                  if(r1->value.string_val->yacScanI(&iv))
                  {
                     ret->initInt((iv)||(r2->value.int_val));
                  }
                  else
                  {
                     ret->initInt((r2->value.int_val));
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
         }
         break;
      case 20: // string-float
         switch(_op)
         {
            default:
            case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
               /////if(YAC_VALID(r1->value.string_val)) ??
               {
                  r1->value.string_val->yacValueOfF32(r2->value.float_val);
               }
               break;
            case YAC_OP_ADD:
            {
               YAC_String t;
               t.yacValueOfF32(r2->value.float_val);
               YAC_String *ts = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
               YAC_String *r1s = r1->value.string_val;
               if(YAC_VALID(r1s))
               {
                  ts->alloc(r1s->length+t.length+1);
                  ts->empty();
                  ts->yacCopy(r1s);
                  ts->append(&t);
               }
               else
               {
                  ts->yacCopy(&t);
               }
               ret->initString(ts, 1);
               r1->unset();
            }
            break;
            case YAC_OP_SUB:
               if(YAC_VALID(r1->value.object_val))
               {
                  sF32 fv;
                  if(r1->value.string_val->yacScanF32(&fv))
                  {
                     ret->initFloat(fv-r2->value.float_val);
                  }
                  else
                  {
                     ret->initFloat(0.0f);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initFloat(-r2->value.float_val);
               }
               break;
            case YAC_OP_MUL:
               if(YAC_VALID(r1->value.object_val))
               {
                  sF32 fv;
                  if(r1->value.string_val->yacScanF32(&fv))
                  {
                     ret->initFloat(fv*r2->value.float_val);
                  }
                  else
                  {
                     ret->initFloat(0.0f);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initFloat(0.0f);
               }
               break;
            case YAC_OP_DIV:
               if(YAC_VALID(r1->value.object_val))
               {
                  sF32 fv;
                  if(r1->value.string_val->yacScanF32(&fv))
                  {
                     ret->initFloat((Dfltnonzero(r2->value.float_val))?fv/r2->value.float_val:0);
                  }
                  else
                  {
                     ret->initFloat(0.0f);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initFloat(0.0f);
               }
               break;
            case YAC_OP_MOD:
               if(YAC_VALID(r1->value.object_val))
               {
                  sF32 fv;
                  if(r1->value.string_val->yacScanF32(&fv))
                  {
                     ret->initInt(
                        (((sSI)r2->value.float_val)!=0)
                        ?
                        ((sSI)fv)%((sSI)r2->value.float_val):0
                        );
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_SHL:
               if(YAC_VALID(r1->value.object_val))
               {   // copy r1 from 0 to (int)r2
                  if(r2->value.float_val>=0)
                  {
                     ret->initEmptyString();
                     r1->value.string_val->substring(ret->value.string_val, 0, (sSI)(r2->value.float_val));
                  }
                  else
                  {
                     ret->initNewString(r1->value.string_val);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initEmptyString();
               }
               break;
            case YAC_OP_SHR:
               if(YAC_VALID(r1->value.object_val))
               {   // copy r1 from r2 to end
                  if(r2->value.float_val>=0)
                  {
                     ret->initEmptyString();
                     r1->value.string_val->substring(ret->value.string_val,
                                                     (sSI)r2->value.float_val, r1->value.string_val->length-((sSI)r2->value.float_val));
                  }
                  else
                  {
                     ret->initEmptyString();
                  }
                  r1->unset();
               }
               else
               {
                  ret->initEmptyString();
               }
               break;
            case YAC_OP_CEQ:
               if(YAC_VALID(r1->value.object_val))
               {
                  sF32 fv;
                  if(r1->value.string_val->yacScanF32(&fv))
                  {
                     ret->initInt(Dfltequal(fv, r2->value.float_val));
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CNE:
               if(YAC_VALID(r1->value.object_val))
               {
                  sF32 fv;
                  if(r1->value.string_val->yacScanF32(&fv))
                  {
                     ret->initInt(Dfltnotequal(fv, r2->value.float_val));
                  }
                  else
                  {
                     ret->initInt(1);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(1);
               }
               break;
            case YAC_OP_CLE:
               if(YAC_VALID(r1->value.object_val))
               {
                  sF32 fv;
                  if(r1->value.string_val->yacScanF32(&fv))
                  {
                     ret->initInt(fv<=r2->value.float_val);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CLT:
               if(YAC_VALID(r1->value.object_val))
               {
                  sF32 fv;
                  if(r1->value.string_val->yacScanF32(&fv))
                  {
                     ret->initInt(fv<r2->value.float_val);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CGE:
               if(YAC_VALID(r1->value.object_val))
               {
                  sF32 fv;
                  if(r1->value.string_val->yacScanF32(&fv))
                  {
                     ret->initInt(fv>=r2->value.float_val);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_CGT:
               if(YAC_VALID(r1->value.object_val))
               {
                  sF32 fv;
                  if(r1->value.string_val->yacScanF32(&fv))
                  {
                     ret->initInt(fv>r2->value.float_val);
                  }
                  else
                  {
                     ret->initInt(0);
                  }
                  r1->unset();
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case YAC_OP_AND:
            case YAC_OP_OR:
            case YAC_OP_EOR:
            case YAC_OP_LAND:
            case YAC_OP_LOR:	 r1->unset(); ret->initInt(0); break;
         }
         break;
      case 28: // string-object
         // s << ifs; serialization..
         switch(_op)
         {
            case YAC_OP_ASSIGN:
               if(YAC_VALID(r1->value.object_val))
               {
                  r1->value.object_val->yacOperatorAssign(r2->value.object_val);
                  r2->unset();
                  *ret=r1;
               }
               break;
            case YAC_OP_CEQ:
               if(YAC_VALID(r1->value.object_val))
               {
                  r1->value.object_val->yacOperator(YAC_OP_CEQ, r2->value.object_val, ret);
                  r1->unset();
                  r2->unset();
               }
               else
               {
                  if(YAC_VALID(r2->value.object_val))
                  {
                     ret->initInt(r2->value.object_val->class_ID==YAC_CLID_OBJECT);
                     r2->unset();
                  }
                  else
                  {
                     ret->initInt(1);
                  }
               }
               break;
            case YAC_OP_CNE:
               if(YAC_VALID(r1->value.object_val))
               {
                  r1->value.object_val->yacOperator(YAC_OP_CNE, r2->value.object_val, ret);
               }
               else
               {
                  if(YAC_VALID(r2->value.object_val))
                  {
                     ret->initInt(r2->value.object_val->class_ID!=YAC_CLID_OBJECT);
                     r2->unset();
                  }
                  else
                  {
                     ret->initInt(0);
                  }
               }
               break;
            case YAC_OP_CLT:
            case YAC_OP_CLE:
            case YAC_OP_CGE:
            case YAC_OP_CGT:
               if(YAC_VALID(r1->value.object_val))
               {
                  r1->value.object_val->yacOperator(_op, r2->value.object_val, ret);
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            default:
               if(YAC_VALID(r1->value.object_val))
               {
                  if(r1->deleteme ||
                     ( (_op==YAC_OP_SHL) &&
                       (
                          r1->value.object_val->yacIsStream() ||   // ofs << obj
                          r2->value.object_val->yacIsStream()      // obj << ifs
                        )
                       )
                     )
                  {
                     // ---- direct modification
                     YAC_Value opr;
                     r1->value.object_val->yacOperator(_op, r2->value.object_val, &opr);
                     r2->unset();
                     *ret=r1;
                  }
                  else
                  {   // ---- first duplicate object then call operator on new object and return it
                     YAC_Object *o = YAC_CLONE_POOLED(_env->context, r1->value.object_val);
                     ////o->yacCopy(r1->value.object_val);
                     o->yacOperatorAssign(r1->value.object_val);
                     r1->value.object_val = o;
                     r1->deleteme = 1;
                     YAC_Value opr;
                     r1->value.object_val->yacOperator(_op, r2->value.object_val, &opr);
                     r2->unset();
                     *ret = r1;
                  }
               }
               else
               {
                  r2->unset();
               }
               break;
         }
         break;
      case 36: // string-string
         switch(_op)
         {
            default:
            case YAC_OP_ASSIGN: // called by TKScriptClassInstance::setClassMemberValue()
               r1->value.string_val->yacCopy(r2->value.string_val);
               break;
            case YAC_OP_ADD:
               if(YAC_VALID(r1->value.string_val))
               {
                  if(r1->deleteme)
                  {
                     r1->deleteme = 0;
                     ret->initString(r1->value.string_val, 1);
                     ret->value.string_val->_append_YAC_RSELF(r2->value.string_val);
                  }
                  else
                  {
                     YAC_String *t = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
                     if(YAC_VALID(r2->value.string_val))
                     {
                        t->alloc(1+r1->value.string_val->length+(r2->value.string_val?r2->value.string_val->length:0));
                        t->empty();
                        t->yacCopy(r1->value.string_val);
                        t->append(r2->value.string_val);
                     }
                     else
                     {
                        t->alloc(r1->value.string_val->length+7);
                        t->empty();
                        t->yacCopy(r1->value.string_val);
                        t->append("<null>");  // pre27Feb2016: "<void>"
                     }
                     ret->initString(t, 1);
                  }
               }
               else
               {
                  ret->fastDerefValue(r2);
               }
               r2->unset();
               break;
            case YAC_OP_SUB:
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_MUL:
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_DIV:
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_MOD:
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_SHL:
               if(YAC_VALID(r1->value.object_val))
               {   // ---- copy r1 until first occurence of r2 ----
                  sSI fi=r1->value.string_val->indexOf(r2->value.string_val, 0);
                  if(fi>=0)
                  {
                     ret->initEmptyString();
                     r1->value.string_val->substring(ret->value.string_val, 0, fi);
                  }
                  else ret->initNewString(r1->value.string_val);
                  r1->unset();
                  r2->unset();
               }
               else
               {
                  ret->initEmptyString();
                  r2->unset();
               }
               break;
            case YAC_OP_SHR:
               if(YAC_VALID(r1->value.object_val))
               {   // ---- copy r1 starting with first occurence of r2 ----
                  sSI fi=r1->value.string_val->indexOf(r2->value.string_val, 0);
                  if(fi>=0)
                  {
                     ret->initEmptyString();
                     r1->value.string_val->substring(ret->value.string_val, fi, r1->value.string_val->length-fi);
                  }
                  else ret->initEmptyString();
               }
               else
               {
                  ret->initEmptyString();
               }
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_CEQ:
               if(YAC_VALID(r1->value.object_val))
               {
                  ret->initInt(r1->value.string_val->compare(r2->value.string_val));
               }
               else
               {
                  ret->initInt( (YAC_VALID(r2->value.object_val)) == 0 );
               }
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_CNE:
               if(YAC_VALID(r1->value.object_val))
               {
                  ret->initInt(!(r1->value.string_val->compare(r2->value.string_val)));
               }
               else
               {
                  ret->initInt( (YAC_VALID(r2->value.object_val)) != 0 );
               }
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_CLE:
               if(YAC_VALID(r1->value.object_val))
               {	// ---- check if r1 starts with r2 ----
                  if(YAC_VALID(r2->value.object_val))
                  {
                     ret->initInt(r1->value.string_val->startsWith(r2->value.string_val));
                  }
                  else
                  {
                     ret->initInt(0);
                  }
               }
               else
               {
                  ret->initInt(0);
               }
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_CLT:
               if(YAC_VALID(r1->value.object_val))
               {
                  // ---- r1 is logical predecessor to r2 ----
                  if(YAC_VALID(r2->value.object_val))
                  {
                     ret->initInt(r1->value.string_val->isPredecessor(r2->value.string_val, 0) );
                  }
                  else
                  {
                     ret->initInt(0);
                  }
               }
               else
               {
                  ret->initInt(0);
               }
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_CGE:
               if(YAC_VALID(r1->value.object_val))
               {	// ---- check if r1 ends with r2 ----
                  if(YAC_VALID(r2->value.object_val))
                  {
                     ret->initInt(r1->value.string_val->endsWith(r2->value.string_val));
                  }
                  else
                  {
                     ret->initInt(0);
                  }
               }
               else
               {
                  ret->initInt(0);
               }
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_CGT:
               if(YAC_VALID(r1->value.object_val))
               {
                  // ---- r1 is logical successor to r2 ----
                  if(YAC_VALID(r2->value.string_val))
                  {
                     ret->initInt(! r1->value.string_val->isPredecessor(r2->value.string_val, 0) );
                  }
                  else
                  {
                     ret->initInt(1);
                  }
               }
               else
               {
                  ret->initInt(0);
               }
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_AND:
               if(YAC_VALID(r1->value.object_val))
               {
                  // ---- r2 is substring of r1 ----
                  if(YAC_VALID(r2->value.string_val))
                  {
                     ret->initInt(r1->value.string_val->isSubstring(r2->value.string_val, 0) );
                  }
                  else
                  {
                     ret->initInt(0);
                  }
               }
               else
               {
                  ret->initInt(0);
               }
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_OR:
               if(YAC_VALID(r1->value.object_val))
               {
                  ret->initNewString(r1->value.string_val);
                  if(YAC_VALID(r2->value.string_val))
                  {
                     ret->value.string_val->words((*r2->value.string_val)[0]!=' '); // XXXX clean up object list?
                  }
               }
               else
               {
                  ret->initEmptyString();
               }
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_EOR:
               ret->initNewString(r1->value.string_val);
               if(YAC_VALID(r2->value.string_val))
               {
                  ret->value.string_val->split((*r2->value.string_val)[0]); // XXX clean up object list?
               }
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_LAND:
               if(YAC_VALID(r1->value.object_val))
               {
                  if(YAC_VALID(r2->value.object_val))
                  {
                     // both string are not empty
                     ret->initInt((!r1->value.string_val->isBlank())&&(!r2->value.string_val->isBlank()));
                  }
               }
               r1->unset();
               r2->unset();
               break;
            case YAC_OP_LOR:
               if(YAC_VALID(r1->value.object_val))
               {
                  if(YAC_VALID(r2->value.object_val))
                  {
                     ret->initInt((!r1->value.string_val->isBlank())||(!r2->value.string_val->isBlank()) );
                  }
               }
               r1->unset();
               r2->unset();
               break;
         }
         break;
   }
}

void PTN_DoubleArgExpr::EvalBEQ(YAC_Value &r1, YAC_Value &r2, YAC_Value *_r) {
   // compare bit pattern / address  (@(a) == @(b))
   switch(r1.type)
   {
      default:
      case YAC_TYPE_VOID:
         _r->initInt(YAC_FALSE);
         break;

      case YAC_TYPE_INT:
      case YAC_TYPE_FLOAT:
         switch(r2.type)
         {
            default:
            case YAC_TYPE_VOID:
               _r->initInt(YAC_FALSE);
               break;

            case YAC_TYPE_INT:
            case YAC_TYPE_FLOAT:
               // @(i) == @(f)  or  @(i) == @(i)  or @(f) == @(f)  or @(f) == @(i)
               _r->initInt(r1.value.int_val == r2.value.int_val);
               break;

            case YAC_TYPE_OBJECT:
            case YAC_TYPE_STRING:
               // true if @(0) == @(null)
               _r->initInt( (0 == r1.value.int_val) && !YAC_VALID(r2.value.object_val) );
               r2.unsetFast();
               break;
         }
         break;

      case YAC_TYPE_OBJECT:
      case YAC_TYPE_STRING:
         switch(r2.type)
         {
            default:
            case YAC_TYPE_VOID:
               _r->initInt(YAC_FALSE);
               break;

            case YAC_TYPE_INT:
            case YAC_TYPE_FLOAT:
               // true if @(null) == @(0)
               _r->initInt( !YAC_VALID(r1.value.object_val) && (0 == r2.value.int_val) );
               break;

            case YAC_TYPE_OBJECT:
            case YAC_TYPE_STRING:
               // true if @(obj1) == @(obj2)  or  @(null) == @(null)
               _r->initInt( ((void*)r1.value.object_val == (void*)r2.value.object_val) ||
                            (!YAC_VALID(r1.value.object_val) && !YAC_VALID(r2.value.object_val))
                            );
               r2.unsetFast();
               break;
         }
         r1.unsetFast();
         break;
   }
}

void PTN_DoubleArgExpr::EvalBNE(YAC_Value &r1, YAC_Value &r2, YAC_Value *_r) {
   EvalBEQ(r1, r2, _r);
   _r->value.int_val ^= 1;
}

sBool PTN_DoubleArgExpr::optimizeBEQ(void) {
   // @(a) == @(b) / @(a) != @(b) ?
   if(YAC_OP_CEQ == expr_operator || YAC_OP_CNE == expr_operator)
   {
      if(PTN_BUILTINFUN == expr1->getID() &&
         PTN_BUILTINFUN == expr2->getID()
         )
      {
         PTN_BuiltinFun *fun1 = (PTN_BuiltinFun*)expr1;
         PTN_BuiltinFun *fun2 = (PTN_BuiltinFun*)expr2;
         if(fun1->isFunAddress() && fun2->isFunAddress())
         {
            // @(a) == @(b)  or  @(a) != @(b)
            expr_operator = (YAC_OP_CEQ == expr_operator) ? YAC_OP_BEQ : YAC_OP_BNE;
            tks_optimize_expr(&fun1->expr, 0/*rtti_hint*/);
            tks_optimize_expr(&fun2->expr, 0/*rtti_hint*/);
            return YAC_TRUE;
         }
      }
   }
   return YAC_FALSE;
}

void PTN_DoubleArgExpr::optimize(void) {
   if(!optimizeBEQ())
   {
      tks_optimize_expr(&expr1, 0/*rtti_hint*/);
      tks_optimize_expr(&expr2, 0/*rtti_hint*/);
      expr1_opt = expr1->getEval();
      expr2_opt = expr2->getEval();
   }
   // else: don't optimize PTN_BuiltinFunExprs expr1/expr2 or they may turn into PTN_Constvals
}

#ifdef TKS_JIT
sBool PTN_DoubleArgExpr::forceHybrid(void) {
   return expr1->forceHybrid() || expr2->forceHybrid();
}

sU8 PTN_DoubleArgExpr::compile(VMCore *_vm) {
   sBool spq=0;
   if( (expr1->getID()==PTN_VAREXPR) && (expr2->getID()==PTN_VAREXPR) )
   {   // --- shortpath to i*i, f*f ----
      spq=((PTN_VarExpr*)expr1)->cached_object == ((PTN_VarExpr*)expr2)->cached_object;
   }
   sU8 r1=expr1->compile(_vm);
   if(r1!=255)
   {
      if(spq&&(expr_operator==YAC_OP_MUL))
      {
         switch(r1)
         {
            case 1: Dasmop(VMOP_SIQUAD); return 1;
            case 2: Dasmop(VMOP_SFQUAD); return 2;
         }
      }
      sU8 r2=expr2->compile(_vm);
      if(r2!=255)
         return CompileOp(_vm, r1, r2, expr_operator);
   }
   return 255;
}

sU8 PTN_DoubleArgExpr::CompileOp(VMCore *_vm, sU8 r1, sU8 r2, sU8 expr_operator) {
   YAC_Value v;
   switch(r1)
   {
      default: return 0xFF;
      case 1:
         switch(r2)
         {
            default: return 0xFF;
            case 0:
               // -------- i X void --------
               return 1;
            case 1:
               // -------- i X i --------
               switch(expr_operator)
               {
                  case YAC_OP_AND: _vm->addOpcode(VMOP_SIAND); return 1;
                  case YAC_OP_OR:  _vm->addOpcode(VMOP_SIOR);  return 1;
                  case YAC_OP_EOR: _vm->addOpcode(VMOP_SIEOR); return 1;
                  case YAC_OP_ADD: _vm->addOpcode(VMOP_SIADD); return 1;
                  case YAC_OP_SUB: _vm->addOpcode(VMOP_SISUB); return 1;
                  case YAC_OP_MUL: _vm->addOpcode(VMOP_SIMUL); return 1;
                  case YAC_OP_DIV: _vm->addOpcode(VMOP_SIDIV); return 1;
                  case YAC_OP_MOD: _vm->addOpcode(VMOP_SIMOD); return 1;
                  case YAC_OP_SHL: _vm->addOpcode(VMOP_SIASL); return 1;
                  case YAC_OP_SHR: _vm->addOpcode(VMOP_SIASR); return 1;
                  case YAC_OP_CEQ: _vm->addOpcode(VMOP_SICMPBGT+5); return 1;
                  case YAC_OP_CNE: _vm->addOpcode(VMOP_SICMPBGT+4); return 1;
                  case YAC_OP_CLE: _vm->addOpcode(VMOP_SICMPBGT+3); return 1;
                  case YAC_OP_CLT: _vm->addOpcode(VMOP_SICMPBGT+2); return 1;
                  case YAC_OP_CGE: _vm->addOpcode(VMOP_SICMPBGT+1); return 1;
                  case YAC_OP_CGT: _vm->addOpcode(VMOP_SICMPBGT+0); return 1;
                  case YAC_OP_LAND:
                     Dasmop(VMOP_SITESTBZ2);
                     Dasmop(VMOP_SITESTBZ);
                     Dasmop(VMOP_SIAND);
                     return 1;
                  case YAC_OP_LOR:
                     Dasmop(VMOP_SITESTBZ2);
                     Dasmop(VMOP_SITESTBZ);
                     Dasmop(VMOP_SIOR);
                     return 1;
               }
               break;
            case 2:
               // -------- i X f --------
               switch(expr_operator)
               {
                  case YAC_OP_LAND:
                  case YAC_OP_LOR:
                  default:
                     return 0xFF;
                  case YAC_OP_AND:
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIAND);
                     return 1;
                  case YAC_OP_OR:
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIOR);
                     return 1;
                  case YAC_OP_EOR:
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIEOR);
                     return 1;
                  case YAC_OP_ADD:
                     Dasmop(VMOP_STCIF2);
                     Dasmop(VMOP_SFADD);
                     return 2;
                  case YAC_OP_SUB:
                     Dasmop(VMOP_STCIF2);
                     Dasmop(VMOP_SFSUB);
                     return 2;
                  case YAC_OP_MUL:
                     Dasmop(VMOP_STCIF2);
                     Dasmop(VMOP_SFMUL);
                     return 2;
                  case YAC_OP_DIV:
                     Dasmop(VMOP_STCIF2);
                     Dasmop(VMOP_SFDIV);
                     return 2;
                  case YAC_OP_MOD:
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIMOD);
                     return 1;
                  case YAC_OP_SHL:
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIASL);
                     return 1;
                  case YAC_OP_SHR:
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIASR);
                     return 1;
                  case YAC_OP_CEQ:
                     Dasmop(VMOP_STCIF2);
                     Dasmopv(VMOP_SFCMPBGT, 5);
                     return 1;
                  case YAC_OP_CNE:
                     Dasmop(VMOP_STCIF2);
                     Dasmopv(VMOP_SFCMPBGT, 4);
                     return 1;
                  case YAC_OP_CLE:
                     Dasmop(VMOP_STCIF2);
                     Dasmopv(VMOP_SFCMPBGT, 3);
                     return 1;
                  case YAC_OP_CLT:
                     Dasmop(VMOP_STCIF2);
                     Dasmopv(VMOP_SFCMPBGT, 2);
                     return 1;
                  case YAC_OP_CGE:
                     Dasmop(VMOP_STCIF2);
                     Dasmopv(VMOP_SFCMPBGT, 1);
                     return 1;
                  case YAC_OP_CGT:
                     Dasmop(VMOP_STCIF2);
                     Dasmopv(VMOP_SFCMPBGT, 0);
                     return 1;
               }
               break;

            case 3:
            case 4:
               // -------- i X o --------
               return 0xFF;
         } // switch(r2)
         break;

      case 2:
         switch(r2)
         {
            default:
               return 0xFF;
            case 0:
               // -------- f X void --------
               return 2;
            case 1:
               // -------- f X i --------
               switch(expr_operator)
               {
                  default:
                     return 0xFF;
                  case YAC_OP_AND:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_SIAND);
                     return 1;
                  case YAC_OP_OR:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_SIOR);
                     return 1;
                  case YAC_OP_EOR:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_SIEOR);
                     return 1;
                  case YAC_OP_ADD:
                     Dasmop(VMOP_STCIF);
                     Dasmop(VMOP_SFADD);
                     return 2;
                  case YAC_OP_SUB:
                     Dasmop(VMOP_STCIF);
                     Dasmop(VMOP_SFSUB);
                     return 2;
                  case YAC_OP_MUL:
                     Dasmop(VMOP_STCIF);
                     Dasmop(VMOP_SFMUL);
                     return 2;
                  case YAC_OP_DIV:
                     Dasmop(VMOP_STCIF);
                     Dasmop(VMOP_SFDIV);
                     return 2;
                  case YAC_OP_MOD:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_SIMOD);
                     return 1;
                  case YAC_OP_SHL:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_SIASL);
                     return 1;
                  case YAC_OP_SHR:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_SIASR);
                     return 1;
                  case YAC_OP_CEQ:
                     Dasmop(VMOP_STCIF);
                     _vm->addOpcode(VMOP_SFCMPBGT+5);
                     return 1;
                  case YAC_OP_CNE:
                     Dasmop(VMOP_STCIF);
                     _vm->addOpcode(VMOP_SFCMPBGT+4);
                     return 1;
                  case YAC_OP_CLE:
                     Dasmop(VMOP_STCIF);
                     _vm->addOpcode(VMOP_SFCMPBGT+3);
                     return 1;
                  case YAC_OP_CLT:
                     Dasmop(VMOP_STCIF);
                     _vm->addOpcode(VMOP_SFCMPBGT+2);
                     return 1;
                  case YAC_OP_CGE:
                     Dasmop(VMOP_STCIF);
                     _vm->addOpcode(VMOP_SFCMPBGT+1);
                     return 1;
                  case YAC_OP_CGT:
                     Dasmop(VMOP_STCIF);
                     _vm->addOpcode(VMOP_SFCMPBGT+0);
                     return 1;
               }
               return 2;
            case 2:
               // -------- f X f --------
               switch(expr_operator)
               {
                  default:
                     return 0xFF;
                  case YAC_OP_AND:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIAND);
                     return 1;
                  case YAC_OP_OR:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIOR);
                     return 1;
                  case YAC_OP_EOR:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIEOR);
                     return 1;
                  case YAC_OP_ADD:
                     _vm->addOpcode(VMOP_SFADD);
                     return 2;
                  case YAC_OP_SUB:
                     _vm->addOpcode(VMOP_SFSUB);
                     return 2;
                  case YAC_OP_MUL:
                     _vm->addOpcode(VMOP_SFMUL);
                     return 2;
                  case YAC_OP_DIV:
                     _vm->addOpcode(VMOP_SFDIV);
                     return 2;
                  case YAC_OP_MOD:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIMOD);
                     return 1;
                  case YAC_OP_SHL:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIASL);
                     return 1;
                  case YAC_OP_SHR:
                     Dasmop(VMOP_STCFI2);
                     Dasmop(VMOP_STCFI);
                     Dasmop(VMOP_SIASR);
                     return 1;
                  case YAC_OP_CEQ:
                     _vm->addOpcode(VMOP_SFCMPBGT+5);
                     return 1;
                  case YAC_OP_CNE:
                     _vm->addOpcode(VMOP_SFCMPBGT+4);
                     return 1;
                  case YAC_OP_CLE:
                     _vm->addOpcode(VMOP_SFCMPBGT+3);
                     return 1;
                  case YAC_OP_CLT:
                     _vm->addOpcode(VMOP_SFCMPBGT+2);
                     return 1;
                  case YAC_OP_CGE:
                     _vm->addOpcode(VMOP_SFCMPBGT+1);
                     return 1;
                  case YAC_OP_CGT:
                     _vm->addOpcode(VMOP_SFCMPBGT+0);
                     return 1;
               }
               return 2;

            case 3:
               // -------- f X o --------
               return 0xFF;
         } // switch(r2)
         break;

      case 3:
         switch(r2)
         {
            case 0:
               // -------- o X void --------
               return 3;
            default:
            case 1:
               // ---- o X i ----
               switch(expr_operator)
               {
                  case YAC_OP_CLT:
                  case YAC_OP_CLE:
                  case YAC_OP_CEQ:
                  case YAC_OP_CNE:
                  case YAC_OP_CGE:
                  case YAC_OP_CGT:
                     _vm->pushci(expr_operator);
                     _vm->addAPICall3((sUI)&tksvm_operatori);
                     Dasmop(VMOP_INCSTP);  // clean up expr_operator
                     Dasmop(VMOP_INCSTP);  // clean up rhs
                     Dasmop(VMOP_SRETI);   // overwrite lhs with return value
                     return 1;
               }
               return 0xFF;
            case 2:
               // ---- o X f ----
               switch(expr_operator)
               {
                  case YAC_OP_CLT:
                  case YAC_OP_CLE:
                  case YAC_OP_CEQ:
                  case YAC_OP_CNE:
                  case YAC_OP_CGE:
                  case YAC_OP_CGT:
                     _vm->pushci(expr_operator);
                     _vm->addAPICall3((sUI)&tksvm_operatorf);
                     Dasmop(VMOP_INCSTP);  // clean up expr_operator
                     Dasmop(VMOP_INCSTP);  // clean up rhs
                     Dasmop(VMOP_SRETI);   // overwrite lhs with return value (floatval always returned in eax/d0/r0)
                     return 1;
               }
               return 0xFF;
            case 3:
               //-------- o X o --------
               switch(expr_operator)
               {
                  default:
                     Dprintf("[---] JIT compiler does not support \"new\" (non comparing object-object operator \"%i\")\n", expr_operator);
                     return 0xFF;
                  case YAC_OP_CLT:
                  case YAC_OP_CLE:
                  case YAC_OP_CEQ:
                  case YAC_OP_CNE:
                  case YAC_OP_CGE:
                  case YAC_OP_CGT:
                     _vm->pushci(expr_operator);
                     _vm->addAPICall3((sUI)&tksvm_operator);
                     Dasmop(VMOP_INCSTP); // clean up expr_operator
                     Dasmop(VMOP_INCSTP); // clean up rhs
                     Dasmop(VMOP_SRETI);  // overwrite lhs with return value
                     return 1;
               }
         }
         break;
   } // switch(r1)
   return 0xfF;
}
#endif
