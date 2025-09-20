/// PTN_ConstVal.cpp
///
/// (c) 2001-2008 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "PTN_ConstVal.h"


PTN_ConstVal::PTN_ConstVal(void) {
}

PTN_ConstVal::PTN_ConstVal(sF32 _v) {
   const_value.initFloat(_v);
}

PTN_ConstVal::PTN_ConstVal(sSI _v) {
   const_value.initInt(_v);
}

PTN_ConstVal::PTN_ConstVal(YAC_Object *_o) {
   const_value.initObject(_o, 0);
}

PTN_ConstVal::~PTN_ConstVal() {
   const_value.unsetFast();
}

void PTN_ConstVal::eval(PTN_Env *_env, YAC_Value *_r) const {
   (void)_env;
   _r->value.any = const_value.value.any;
   _r->type      = const_value.type;
   _r->deleteme  = 0;
}

static void PTN_ConstVal__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   Dtracest;
   _r->value.any = ((PTN_ConstVal*)_st)->const_value.value.any;
   _r->type      = ((PTN_ConstVal*)_st)->const_value.type;
   _r->deleteme  = 0;
   (void)_env;
}

Feval PTN_ConstVal::getEval(void) const {
   return PTN_ConstVal__eval;
}

sUI PTN_ConstVal::getType(void) const {
   return const_value.type;
}

void PTN_ConstVal::evalConst(YAC_Value *_r) {
   _r->value.any = const_value.value.any;
   _r->type      = const_value.type;
   _r->deleteme  = const_value.deleteme;
   const_value.deleteme = 0;
}

sBool PTN_ConstVal::isConst(void) {
   return 1;
}

#ifdef TKS_JIT
sU8 PTN_ConstVal::compile(VMCore *_vm) {
   switch(const_value.type)
   {
   default:
      Dcterror(PTNERR_ERRCOMPILE);
      return 0xFF;
   case 1:
      _vm->pushci(const_value.value.int_val);
      return 1;
   case 2:
      _vm->pushcf(const_value.value.float_val);
      return 2;
   case 3:
   case 4:
      _vm->pushci(const_value.value.int_val);
      return 3;
   }
}

sBool PTN_ConstVal::forceHybrid(void) {
   return 0;
}
#endif


sBool tks_isobjectconstval(PTN_Expr *_expr) {
   if(_expr)
   {
      if(_expr->getID()==PTN_CONSTVAL)
      {
         // ---- keep object/string constants! ----
         return (((PTN_ConstVal*)_expr)->const_value.type>=YAC_TYPE_OBJECT)&&
            (((PTN_ConstVal*)_expr)->const_value.deleteme);
      }
   }
   return 0;
}
