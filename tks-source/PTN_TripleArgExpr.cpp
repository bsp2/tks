/// PTN_TripleArgExpr.cpp
///
/// (c) 2001-2015 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "tks_inc_compiler.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_ConstVal.h"
#include "PTN_TripleArgExpr.h"


PTN_TripleArgExpr::PTN_TripleArgExpr(void) {
   expr1 = expr2 = expr3 = NULL;
   expr_operator1 = expr_operator2 = 0;
}

PTN_TripleArgExpr::PTN_TripleArgExpr(PTN_Expr *_e1, sU8 _op1, PTN_Expr *_e2, sU8 _op2, PTN_Expr *_e3) {
   expr1          = _e1;
   expr2          = _e2;
   expr3          = _e3; 
   expr1_opt      = expr1->getEval(); 
   expr2_opt      = expr2->getEval(); 
   expr3_opt      = expr3->getEval();
   expr_operator1 = _op1;
   expr_operator2 = _op2;
}

PTN_TripleArgExpr::~PTN_TripleArgExpr() {
   TKS_DELETE_SAFE(expr1);

   TKS_DELETE_SAFE(expr2);

   TKS_DELETE_SAFE(expr3);
}

sBool PTN_TripleArgExpr::semanticCheck(void) {
   return 
      (expr1 ? expr1->semanticCheck() : 0) &&
      (expr2 ? expr2->semanticCheck() : 0) &&
      (expr3 ? expr3->semanticCheck() : 0)
      ;
}

sBool PTN_TripleArgExpr::resolveXRef(void) { 
   return
      expr1->resolveXRef() &&
      expr2->resolveXRef() &&
      expr3->resolveXRef()
      ;
} 

sBool PTN_TripleArgExpr::isConst(void) {
   sBool r;
   r=(expr1?expr1->isConst():0);
   r=r&&(expr2?expr2->isConst():0);
   r=r&&(expr3?expr3->isConst():0);
   return r;
}

void PTN_TripleArgExpr::optimize(void) {
   if(expr1) 
   {
      if(expr1->isConst())
      {
         YAC_Value r;
         expr1->evalConst(&r);
         delete expr1;
         PTN_ConstVal *cv;
         PTN_NEW_STATIC_NODE(cv, PTN_ConstVal)();
         cv->const_value=&r;
         expr1=cv;
         if(r.type>2)
         {
            Dprintf("\n[---] warning: TripleArgExpr 1 is neither integer nor float.\n");
         }
      }
      else
      {
         expr1->optimize(); 
      }
      expr1_opt=expr1->getEval(); 
   }
   if(expr2) 
   {
      if(expr2->isConst())
      {
         YAC_Value r;
         expr2->evalConst(&r);
         delete expr2;
         PTN_ConstVal *cv;
         PTN_NEW_STATIC_NODE(cv, PTN_ConstVal)();
         cv->const_value=&r;
         expr2=cv;
         if(r.type>2)
         {
            Dprintf("\n[---] warning: TripleArgExpr 2 is neither integer nor float.\n");
         }
      }
      else
      {
         expr2->optimize(); 
      }
      expr2_opt=expr2->getEval(); 
   }
   if(expr3) 
   {
      if(expr3->isConst())
      {
         YAC_Value r;
         expr3->evalConst(&r);
         delete expr3;
         PTN_ConstVal *cv;
         PTN_NEW_STATIC_NODE(cv, PTN_ConstVal)();
         cv->const_value = &r;
         expr3 = cv;
         if(r.type >= YAC_TYPE_OBJECT)
         {
            Dprintf("\n[---] warning: TripleArgExpr 3 is neither integer nor float.\n");
         }
      }
      else
      {
         expr3->optimize(); 
      }
      expr3_opt=expr3->getEval(); 
   }
}

static void PTN_TripleArgExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
   const PTN_TripleArgExpr *st = (const PTN_TripleArgExpr*)_st; 
   
   sBool r=0;
   YAC_Value re1;
   YAC_Value re2;
   YAC_Value re3;
   st->expr1_opt(_env, &re1, st->expr1); 
   if(Dhaveexception)
   {
      re1.unsetFast();
      Dhandleexception(st->expr1);
      return;
   }
   
   st->expr2_opt(_env, &re2, st->expr2); 
   if(Dhaveexception)
   {
      re1.unsetFast();
      re2.unsetFast();
      Dhandleexception(st->expr2);
      return;
   }
   
   st->expr3_opt(_env, &re3, st->expr3); 
   if(Dhaveexception)
   {
      re1.unsetFast();
      re2.unsetFast();
      re3.unsetFast();
      Dhandleexception(st->expr3);
      return;
   }

   if(re1.type >= YAC_TYPE_OBJECT)
   {
      re1.typecast(YAC_TYPE_FLOAT);
   }

   if(re2.type >= YAC_TYPE_OBJECT)
   {
      re2.typecast(YAC_TYPE_FLOAT);
   }

   if(re3.type >= YAC_TYPE_OBJECT)
   {
      re3.typecast(YAC_TYPE_FLOAT);
   }
   
   sU16 atc= (sU16) ( (re1.type<<6) | (re2.type<<3) | (re3.type) );
   switch(atc)
   {
   default:
      r=0;
      break;
      case (1<<6)|(1<<3)|1: 
         // int-int-int
         switch(st->expr_operator1)
         {
            case YAC_OP_CEQ:
               r=re1.value.int_val==re2.value.int_val;
               break;
            case YAC_OP_CNE:
               r=re1.value.int_val!=re2.value.int_val;
               break;
            case YAC_OP_CLE:
               r=re1.value.int_val<=re2.value.int_val;
               break;
            case YAC_OP_CLT:
               r=re1.value.int_val<re2.value.int_val;
               break;
            case YAC_OP_CGE:
               r=re1.value.int_val>=re2.value.int_val;
               break;
            case YAC_OP_CGT:
               r=re1.value.int_val>re2.value.int_val;
               break;
         }
         switch(st->expr_operator2)
         {
            case YAC_OP_CEQ:
               r=r&&(re2.value.int_val==re3.value.int_val);
               break;
            case YAC_OP_CNE:
               r=r&&(re2.value.int_val!=re3.value.int_val);
               break;
            case YAC_OP_CLE:
               r=r&&(re2.value.int_val<=re3.value.int_val);
               break;
            case YAC_OP_CLT:
               r=r&&(re2.value.int_val<re3.value.int_val);
               break;
            case YAC_OP_CGE:
               r=r&&(re2.value.int_val>=re3.value.int_val);
               break;
            case YAC_OP_CGT:
               r=r&&(re2.value.int_val>re3.value.int_val);
               break;
         }
         break;

      case (2<<6)|(2<<3)|2: 
         // float-float-float
         switch(st->expr_operator1)
         {
            case YAC_OP_CEQ:
               r=Dfltequal(re1.value.float_val, re2.value.float_val);
               break;
            case YAC_OP_CNE:
               r=Dfltnotequal(re1.value.float_val, re2.value.float_val);
               break;
            case YAC_OP_CLE:
               r=re1.value.float_val<=re2.value.float_val;
               break;
            case YAC_OP_CLT:
               r=re1.value.float_val<re2.value.float_val;
               break;
            case YAC_OP_CGE:
               r=re1.value.float_val>=re2.value.float_val;
               break;
            case YAC_OP_CGT:
               r=re1.value.float_val>re2.value.float_val;
               break;
         }
         switch(st->expr_operator2)
         {
            case YAC_OP_CEQ:
               r=r&&Dfltequal(re2.value.float_val, re3.value.float_val);
               break;
            case YAC_OP_CNE:
               r=r&&Dfltnotequal(re2.value.float_val, re3.value.float_val);
               break;
            case YAC_OP_CLE:
               r=r&&(re2.value.float_val<=re3.value.float_val);
               break;
            case YAC_OP_CLT:
               r=r&&(re2.value.float_val<re3.value.float_val);
               break;
            case YAC_OP_CGE:
               r=r&&(re2.value.float_val>=re3.value.float_val);
               break;
            case YAC_OP_CGT:
               r=r&&(re2.value.float_val>re3.value.float_val);
               break;
         }
         break;
      case (1<<6)|(2<<3)|2: 
         // int-float-float
         switch(st->expr_operator1)
         {
            case YAC_OP_CEQ:
               r=Dfltequal(((sF32)re1.value.int_val), re2.value.float_val);
               break;
            case YAC_OP_CNE:
               r=Dfltnotequal(((sF32)re1.value.int_val), re2.value.float_val);
               break;
            case YAC_OP_CLE:
               r=((sF32)re1.value.int_val)<=re2.value.float_val;
               break;
            case YAC_OP_CLT:
               r=((sF32)re1.value.int_val)<re2.value.float_val;
               break;
            case YAC_OP_CGE:
               r=((sF32)re1.value.int_val)>=re2.value.float_val;
               break;
            case YAC_OP_CGT:
               r=((sF32)re1.value.int_val)>re2.value.float_val;
               break;
         }
         switch(st->expr_operator2)
         {
            case YAC_OP_CEQ:
               r=r&&Dfltequal(re2.value.float_val, re3.value.float_val);
               break;
            case YAC_OP_CNE:
               r=r&&Dfltnotequal(re2.value.float_val, re3.value.float_val);
               break;
            case YAC_OP_CLE:
               r=r&&(re2.value.float_val<=re3.value.float_val);
               break;
            case YAC_OP_CLT:
               r=r&&(re2.value.float_val<re3.value.float_val);
               break;
            case YAC_OP_CGE:
               r=r&&(re2.value.float_val>=re3.value.float_val);
               break;
            case YAC_OP_CGT:
               r=r&&(re2.value.float_val>re3.value.float_val);
               break;
         }
         break;
      case (2<<6)|(1<<3)|2: 
         // float-int-float
         switch(st->expr_operator1)
         {
            case YAC_OP_CEQ:
               r=Dfltequal(re1.value.float_val, ((sF32)re2.value.int_val));
               break;
            case YAC_OP_CNE:
               r=Dfltnotequal(re1.value.float_val, ((sF32)re2.value.int_val));
               break;
            case YAC_OP_CLE:
               r=re1.value.float_val<=((sF32)re2.value.int_val);
               break;
            case YAC_OP_CLT:
               r=re1.value.float_val<((sF32)re2.value.int_val);
               break;
            case YAC_OP_CGE:
               r=re1.value.float_val>=((sF32)re2.value.int_val);
               break;
            case YAC_OP_CGT:
               r=re1.value.float_val>((sF32)re2.value.int_val);
               break;
         }
         switch(st->expr_operator2)
         {
            case YAC_OP_CEQ:
               r=r&&Dfltequal(((sF32)re2.value.int_val), re3.value.float_val);
               break;
            case YAC_OP_CNE:
               r=r&&Dfltnotequal(((sF32)re2.value.int_val), re3.value.float_val);
               break;
            case YAC_OP_CLE:
               r=r&&(((sF32)re2.value.int_val)<=re3.value.float_val);
               break;
            case YAC_OP_CLT:
               r=r&&(((sF32)re2.value.int_val)<re3.value.float_val);
               break;
            case YAC_OP_CGE:
               r=r&&(((sF32)re2.value.int_val)>=re3.value.float_val);
               break;
            case YAC_OP_CGT:
               r=r&&(((sF32)re2.value.int_val)>re3.value.float_val);
               break;
         }
         break;
      case (2<<6)|(2<<3)|1: 
         // float-float-int
         switch(st->expr_operator1)
         {
            case YAC_OP_CEQ:
               r=Dfltequal(re1.value.float_val, re2.value.float_val);
               break;
            case YAC_OP_CNE:
               r=Dfltnotequal(re1.value.float_val, re2.value.float_val);
               break;
            case YAC_OP_CLE:
               r=re1.value.float_val<=re2.value.float_val;
               break;
            case YAC_OP_CLT:
               r=re1.value.float_val<re2.value.float_val;
               break;
            case YAC_OP_CGE:
               r=re1.value.float_val>=re2.value.float_val;
               break;
            case YAC_OP_CGT:
               r=re1.value.float_val>re2.value.float_val;
               break;
         }
         switch(st->expr_operator2)
         {
            case YAC_OP_CEQ:
               r=r&&Dfltequal(re2.value.float_val, ((sF32)re3.value.int_val));
               break;
            case YAC_OP_CNE:
               r=r&&Dfltnotequal(re2.value.float_val, ((sF32)re3.value.int_val));
               break;
            case YAC_OP_CLE:
               r=r&&(re2.value.float_val<=((sF32)re3.value.int_val));
               break;
            case YAC_OP_CLT:
               r=r&&(re2.value.float_val<((sF32)re3.value.int_val));
               break;
            case YAC_OP_CGE:
               r=r&&(re2.value.float_val>=((sF32)re3.value.int_val));
               break;
            case YAC_OP_CGT:
               r=r&&(re2.value.float_val>((sF32)re3.value.int_val));
               break;
         }
         break;
      case (1<<6)|(1<<3)|2: 
         // int-int-float
         switch(st->expr_operator1)
         {
            case YAC_OP_CEQ:
               r=re1.value.int_val==re2.value.int_val;
               break;
            case YAC_OP_CNE:
               r=re1.value.int_val!=re2.value.int_val;
               break;
            case YAC_OP_CLE:
               r=re1.value.int_val<=re2.value.int_val;
               break;
            case YAC_OP_CLT:
               r=re1.value.int_val<re2.value.int_val;
               break;
            case YAC_OP_CGE:
               r=re1.value.int_val>=re2.value.int_val;
               break;
            case YAC_OP_CGT:
               r=re1.value.int_val>re2.value.int_val;
               break;
         }
         switch(st->expr_operator2)
         {
            case YAC_OP_CEQ:
               r=r&&Dfltequal(((sF32)re2.value.int_val), re3.value.float_val);
               break;
            case YAC_OP_CNE:
               r=r&&Dfltnotequal(((sF32)re2.value.int_val), re3.value.float_val);
               break;
            case YAC_OP_CLE:
               r=r&&(((sF32)re2.value.int_val)<=re3.value.float_val);
               break;
            case YAC_OP_CLT:
               r=r&&(((sF32)re2.value.int_val)<re3.value.float_val);
               break;
            case YAC_OP_CGE:
               r=r&&(((sF32)re2.value.int_val)>=re3.value.float_val);
               break;
            case YAC_OP_CGT:
               r=r&&(((sF32)re2.value.int_val)>re3.value.float_val);
               break;
         }
         break;
      case (2<<6)|(1<<3)|1: 
         // float-int-int
         switch(st->expr_operator1)
         {
            case YAC_OP_CEQ:
               r=Dfltequal(re1.value.float_val, ((sF32)re2.value.int_val));
               break;
            case YAC_OP_CNE:
               r=Dfltnotequal(re1.value.float_val, ((sF32)re2.value.int_val));
               break;
            case YAC_OP_CLE:
               r=re1.value.float_val<=((sF32)re2.value.int_val);
               break;
            case YAC_OP_CLT:
               r=re1.value.float_val<((sF32)re2.value.int_val);
               break;
            case YAC_OP_CGE:
               r=re1.value.float_val>=((sF32)re2.value.int_val);
               break;
            case YAC_OP_CGT:
               r=re1.value.float_val>((sF32)re2.value.int_val);
               break;
         }
         switch(st->expr_operator2)
         {
            case YAC_OP_CEQ:
               r=r&&(re2.value.int_val==re3.value.int_val);
               break;
            case YAC_OP_CNE:
               r=r&&(re2.value.int_val!=re3.value.int_val);
               break;
            case YAC_OP_CLE:
               r=r&&(re2.value.int_val<=re3.value.int_val);
               break;
            case YAC_OP_CLT:
               r=r&&(re2.value.int_val<re3.value.int_val);
               break;
            case YAC_OP_CGE:
               r=r&&(re2.value.int_val>=re3.value.int_val);
               break;
            case YAC_OP_CGT:
               r=r&&(re2.value.int_val>re3.value.int_val);
               break;
         }
         break;
      case (1<<6)|(2<<3)|1: 
         // int-float-int
         switch(st->expr_operator1)
         {
            case YAC_OP_CEQ:
               r=Dfltequal(((sF32)re1.value.int_val), re2.value.float_val);
               break;
            case YAC_OP_CNE:
               r=Dfltnotequal(((sF32)re1.value.int_val), re2.value.float_val);
               break;
            case YAC_OP_CLE:
               r=((sF32)re1.value.int_val)<=re2.value.float_val;
               break;
            case YAC_OP_CLT:
               r=((sF32)re1.value.int_val)<re2.value.float_val;
               break;
            case YAC_OP_CGE:
               r=((sF32)re1.value.int_val)>=re2.value.float_val;
               break;
            case YAC_OP_CGT:
               r=((sF32)re1.value.int_val)>re2.value.float_val;
               break;
         }
         switch(st->expr_operator2)
         {
            case YAC_OP_CEQ:
               r=r&&(Dfltequal(re2.value.float_val, ((sF32)re3.value.int_val)));
               break;
            case YAC_OP_CNE:
               r=r&&(Dfltnotequal(re2.value.float_val,((sF32)re3.value.int_val)));
               break;
            case YAC_OP_CLE:
               r=r&&(re2.value.float_val<=((sF32)re3.value.int_val));
               break;
            case YAC_OP_CLT:
               r=r&&(re2.value.float_val<((sF32)re3.value.int_val));
               break;
            case YAC_OP_CGE:
               r=r&&(re2.value.float_val>=((sF32)re3.value.int_val));
               break;
            case YAC_OP_CGT:
               r=r&&(re2.value.float_val>((sF32)re3.value.int_val));
               break;
         }
         break;
   }
   re1.unsetFast();
   re2.unsetFast();
   re3.unsetFast(); 
   _r->initInt(r);
}

void PTN_TripleArgExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_TripleArgExpr__eval(_env, _r, this);
}

Feval PTN_TripleArgExpr::getEval(void) const {
   return PTN_TripleArgExpr__eval;
}

