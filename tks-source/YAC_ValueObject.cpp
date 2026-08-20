/// YAC_ValueObject.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"

#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "tks_inc_jit.h"
#include "PTN_VarExpr.h"
#include "PTN_DoubleArgExpr.h"

#include "YAC_Value.h"
#include "YAC_Long.h"
#include "YAC_Double.h"
#include "YAC_ValueObject.h"

#include "PTN_Node.h"
#include "PTN_Statement.h"


YAC_ValueObject::YAC_ValueObject(void) {
   initVoid();
}

YAC_ValueObject::~YAC_ValueObject() {
   _unset();
}

void YAC_ValueObject::_unset(void) {
   if( (type>=YAC_TYPE_OBJECT) && deleteme && value.any)
   {
      YAC_DELETE(value.object_val);
      deleteme = YAC_FALSE;
   }
   type      = YAC_TYPE_VOID;
   value.any = NULL;
}

void YAC_VCALL YAC_ValueObject::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_I8(type);
   switch(type)
   {
      case YAC_TYPE_VOID:
         break;

      case YAC_TYPE_INT:
         YAC_SERIALIZE_I32(value.int_val);
         break;

      case YAC_TYPE_FLOAT:
         YAC_SERIALIZE_F32(value.float_val);
         break;

      case YAC_TYPE_OBJECT:
      case YAC_TYPE_STRING:
         if(deleteme && YAC_VALID(value.object_val))
         {
            YAC_SERIALIZE_I8(1); // use flag
            value.object_val->yacSerialize(_ofs, YAC_TRUE/*bRTTI*/);
         }
         else
         {
            YAC_SERIALIZE_I8(0); // use flag
         }
         break;
   }
}

sUI YAC_VCALL YAC_ValueObject::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   type = YAC_DESERIALIZE_I8();
   switch(type)
   {
      default:
      case YAC_TYPE_VOID:
         break;

      case YAC_TYPE_INT:
         value.int_val = YAC_DESERIALIZE_I32();
         break;

      case YAC_TYPE_FLOAT:
         value.float_val = YAC_DESERIALIZE_F32();
         break;

      case YAC_TYPE_OBJECT:
      case YAC_TYPE_STRING:
      {
         sBool bUsed = YAC_DESERIALIZE_I8();
         if(bUsed)
         {
            value.object_val = tkscript->deserializeNewObject(_ifs, NULL/*template*/);
         }
         else
         {
            value.object_val = NULL;
         }
         break;
      }
   }
   return YAC_TRUE;
}

void YAC_VCALL YAC_ValueObject::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   switch(_cmd)
   {
   case YAC_OP_ASSIGN:
      yacOperatorAssign(_o);
      return;
   case YAC_OP_CEQ:
      if(YAC_BCHK(_o, YAC_CLID_VALUE))
      {
         _r->initInt(compareValue((YAC_ValueObject*)_o));
         return;
      }
      break;
   case YAC_OP_CNE:
      if(YAC_BCHK(_o, YAC_CLID_VALUE))
      {
         _r->initInt(!compareValue((YAC_ValueObject*)_o));
         return;
      }
      break;
   }
   // ---- call default operator
   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_VCALL YAC_ValueObject::yacOperatorAssign(YAC_Object *_o) {
   if(YAC_BCHK(_o, YAC_CLID_VALUE))
   {
      copySafe((YAC_ValueObject*)_o);
   }
   else if(YAC_BCHK(_o, YAC_CLID_STRING))
   {
      unsetFast();
      initNewString((YAC_String*)_o);
   }
   else
   {
      safeInitObject(_o, 0);
   }
}

sBool YAC_VCALL YAC_ValueObject::yacScanI(sSI *_retInt) {
   *_retInt = getIntValue();
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_ValueObject::yacScanF32(sF32 *_retFloat) {
   *_retFloat = getFloatValue();
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_ValueObject::yacScanF64(sF64 *_retFloat64) {
   *_retFloat64 = getFloatValue();
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_ValueObject::yacScanI64(sS64 *_retInt64) {
   *_retInt64 = getIntValue();
   return YAC_TRUE;
}

// the yacValueOf*() methodes are usually called in Value v = <value> assignments
void YAC_VCALL YAC_ValueObject::yacValueOfI(sSI _v) {
   safeInitInt(_v);
}

void YAC_VCALL YAC_ValueObject::yacValueOfI64(sS64 _v) {
   YAC_Long *lo = (YAC_Long*) YAC_NEW_CORE_POOLED(YAC_CLID_LONG);
   lo->value = _v;
   safeInitObject(lo, 1);
}

void YAC_VCALL YAC_ValueObject::yacValueOfF32(sF32 _v) {
   safeInitFloat(_v);
}

void YAC_VCALL YAC_ValueObject::yacValueOfF64(sF64 _v) {
   YAC_Double *n = (YAC_Double*) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   n->value = _v;
   safeInitObject(n, 1);
}

void YAC_ValueObject::_getValue(YAC_Value *_r) {
   _r->initValue(this);
}

void YAC_ValueObject::_setValue(YAC_Object *_o) {
   if(YAC_CHK(_o, YAC_CLID_VALUE))
   {
      //unset();
      //derefPointerAssignValue((YAC_ValueObject*)_o);
      copySafe((YAC_ValueObject*)_o);
   }
}

void YAC_ValueObject::_setIntValue(sSI _i) {
   safeInitInt(_i);
}

void YAC_ValueObject::_setFloatValue(sF32 _f) {
   safeInitFloat(_f);
}

sSI YAC_ValueObject::_getType(void) {
   return type;
}

void YAC_ValueObject::_setObjectValue(YAC_Object *_o) {
   unset();
   type = YAC_TYPE_OBJECT;
   if(YAC_VALID(_o))
   {
      value.object_val = _o;
   }
   else
   {
      value.object_val = yac_null;
   }
}

void YAC_ValueObject::_initNull(void) {
   safeInitNull();
}

void YAC_ValueObject::_setStringValue(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      if(YAC_Is_String(_s))
      {
         unset();
         type = YAC_TYPE_STRING;
         value.object_val = _s;
      }
      else
      {
         // Try to convert generic Object to String
         if(type == YAC_TYPE_STRING)
         {
            // Try to re-use current String value if possible
            if(YAC_VALID(value.string_val))
            {
               if(deleteme)
               {
                  value.string_val->empty();
                  _s->yacToString(value.string_val);
                  return;
               }
            }
         }
         unset();
         initEmptyString();
         _s->yacToString(value.string_val);
      }
   }
   else
   {
      safeInitNull();
   }
}

YAC_Object *YAC_ValueObject::_setNewObject(YAC_Object *_templ) {
   if(_templ)
   {
      unsetFast();
      TKS_Context *ctx = tkscript->lockGlobalContext();
      initObject(YAC_CLONE_POOLED(ctx, _templ), 1);// xxx TKS_MT: use *real* thread context (c'tors!) ??
      tkscript->unlockGlobalContext();
      /// xxx yacOperatorAssign
      return value.object_val;
   }
   return 0;
}


void YAC_ValueObject::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING32);
   toString(s);
   YAC_RETS(s,1);
}

sBool YAC_VCALL YAC_ValueObject::yacToString(YAC_String *_s) const {
   toString(_s);
   return 1;
}

void YAC_ValueObject::_initVoid(void) {
   unset();
}

void YAC_ValueObject::_getDeref (YAC_Value *_r) {
   _r->copySafe(this);
}

sSI YAC_ValueObject::_isDeletable(void) {
   if(type >= YAC_TYPE_OBJECT)
   {
      return deleteme;
   }
   return 0;
}

void YAC_ValueObject::_unlinkObject(YAC_Value *_r) {
   _r->type = type;
   _r->value.object_val = value.object_val; // copy int/float/Object value
   if(type >= YAC_TYPE_OBJECT)
   {
      _r->deleteme = deleteme;
      deleteme = 0;
   }
}

void YAC_ValueObject::_assign(YAC_Object *_valueObject) {
   PTN_Env env; env.initDefault();
   YAC_Value r;
   YAC_Value *rhs = NULL;
   YAC_Value t;
   if(YAC_BCHK(_valueObject, YAC_CLID_VALUE))
   {
      rhs = (YAC_ValueObject*)_valueObject;
   }
   else if(YAC_Is_Float(_valueObject))
   {
      t.initFloat(0.0f);
      _valueObject->yacScanF32(&t.value.float_val);
      rhs = &t;
   }
   else if(YAC_Is_Integer(_valueObject))
   {
      t.initInt(0);
      _valueObject->yacScanI(&t.value.int_val);
      rhs = &t;
   }
   else if(YAC_VALID(_valueObject))
   {
      t.initObject(_valueObject, YAC_FALSE/*del*/);
      rhs = &t;
   }
   if(NULL != rhs)
   {
      // yac_host->printf("xxx YAC_ValueObject::assign: this.type=%u vo.type=%u vo.value.object_val=%p\n", type, _valueObject->type,
      PTN_DoubleArgExpr::EvalOp(&env, this,
                                this, rhs,
                                YAC_OP_ASSIGN,
                                NULL/*nodeOrNull*/
                                );
   }
}

void YAC_ValueObject::_pointerAssign(YAC_Object *_valueObject) {
   if(type >= YAC_TYPE_OBJECT)
   {
      if(YAC_BCHK(_valueObject, YAC_CLID_VALUE))
      {
         YAC_CAST_ARG(YAC_ValueObject, vo, _valueObject);
         if(vo->type >= YAC_TYPE_OBJECT)
         {
            derefPointerAssignValue(vo);
         }
      }
      else
      {
         YAC_Value o;
         o.initObject(_valueObject, 0);
         derefPointerAssignValue(&o);
      }
   }
}

sBool YAC_ValueObject::_isVoid(void) {
   return (YAC_TYPE_VOID == type);
}

sBool YAC_ValueObject::_isInt(void) {
   return (YAC_TYPE_INT == type);
}

sBool YAC_ValueObject::_isFloat (void) {
   return (YAC_TYPE_FLOAT == type);
}

sBool YAC_ValueObject::_isObject(void) {
   return (YAC_TYPE_OBJECT == type);
}

sBool YAC_ValueObject::_isString(void) {
   return
      (YAC_TYPE_STRING == type) ||
      ((YAC_TYPE_OBJECT == type) && (YAC_Is_String(value.object_val)));
}
