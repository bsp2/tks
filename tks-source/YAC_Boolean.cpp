/// YAC_Boolean.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>

#include "tks.h"
#include "YAC_Boolean.h"


YAC_Boolean::YAC_Boolean(void) {
   class_ID = YAC_CLID_BOOLEAN;
   value    = 0;
}

YAC_Boolean::~YAC_Boolean(void) {
}

void YAC_Boolean::_New(sSI _v, YAC_Value *_r) {
   YAC_Boolean *ro=(YAC_Boolean *)YAC_NEW_CORE_POOLED(YAC_CLID_BOOLEAN);
   ro->value=(_v&1);
   _r->initObject(ro, 1);
}

void YAC_Boolean::_News(YAC_Object *_s, YAC_Value *_r) {
   if(YAC_Is_String(_s))
   {
      YAC_String *s = (YAC_String*) _s;
      YAC_Boolean *ro=(YAC_Boolean *)YAC_NEW_CORE_POOLED(YAC_CLID_BOOLEAN);
      if(s->_compareIgnoreCase(&tkscript->s_true))
      {
         ro->value = 1;
      }
      else if(s->_compareIgnoreCase(&tkscript->s_false))
      {
         ro->value = 0;
      }
      else
      {
         if(! s->yacScanI(&_r->value.int_val))
         {
            tkscript->yacDelete(ro);
            _r->initNull();
            return;
         }
         else
         {
            ro->value = _r->value.int_val & 1;
         }
      }
      _r->initObject(ro, 1);
   }
   else
   {
      _r->initNull();
   }
}

void YAC_VCALL YAC_Boolean::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_I8((sU8)value);
}

sUI YAC_VCALL YAC_Boolean::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   value=YAC_DESERIALIZE_I8();
   return 1;
}

sBool YAC_VCALL YAC_Boolean::yacScanI(sSI *_v) {
   *_v=(sSI)value;
   return 1;
}

sBool YAC_VCALL YAC_Boolean::yacScanF32(sF32 *_v) {
   *_v=(sF32)value;
   return 1;
}

sBool YAC_VCALL YAC_Boolean::yacScanF64(sF64 *_v) {
   *_v=(sF64)value;
   return 1;
}

sBool YAC_VCALL YAC_Boolean::yacScanI64(sS64 *_v) {
   *_v=(sS64)value;
   return 1;
}

void YAC_VCALL YAC_Boolean::yacValueOfI(sSI _v) {
   value=(sBool)(_v&1);
}

void YAC_VCALL YAC_Boolean::yacValueOfF32(sF32 _v) {
   // boolean<->float true/false threshold
   value=(sBool)(_v>=YAC_FLOAT_DEVIATION)||(_v<=-YAC_FLOAT_DEVIATION);
}

void YAC_VCALL YAC_Boolean::yacValueOfF64(sF64 _v) {
   value=(sBool)(_v>=YAC_FLOAT_DEVIATION)||(_v<=-YAC_FLOAT_DEVIATION);
}

void YAC_VCALL YAC_Boolean::yacValueOfI64(sS64 _v) {
   value=(sBool)(_v&1);
}

void YAC_VCALL YAC_Boolean::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_cmd == YAC_OP_NOT)
   {
      value = (!value)&1;
   }
   else if(_cmd == YAC_OP_BITNOT)
   {
      value = (~value)&1;
   }
   else
   {
      if(YAC_VALID(_o))
      {
         sSI oval;
         if(_o->yacScanI(&oval))
         {
            myOperator(_cmd, (sBool)(oval&1), _r);
            return;
         }
         else
         {
            myOperator(_cmd, 0, _r);
            return;
         }
      }
      YAC_Object::yacOperator(_cmd, _o, _r);
   }
}

void YAC_VCALL YAC_Boolean::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
   myOperator(_cmd, (sBool)(_iv&1), _r);
}

void YAC_VCALL YAC_Boolean::yacOperatorF32(sSI _cmd, sF32 _fv, YAC_Value *_r) {
   myOperator(_cmd,
      (sBool)(_fv>=YAC_FLOAT_DEVIATION)||(_fv<=-YAC_FLOAT_DEVIATION),
      _r);
}

sBool YAC_VCALL YAC_Boolean::yacEquals(YAC_Object *_o) {
   if(YAC_Is_Boolean(_o))
   {
      YAC_Boolean*b=(YAC_Boolean*)_o;
      return (value == b->value);
   }
   return 0;
}


void YAC_Boolean::myOperator(sSI _cmd, sBool oval, YAC_Value *_r) {
   switch(_cmd)
   {
   case YAC_OP_ASSIGN:
      value=oval;
      break;
   case YAC_OP_ADD:
      value=(value+oval)&1;
      break;
   case YAC_OP_SUB:
      value=(value-oval)&1;
      break;
   case YAC_OP_MUL:
      value=(value*oval)&1;
      break;
   case YAC_OP_DIV:
      if(oval)
         value/=oval;
      else
         value=0;
      break;
   case YAC_OP_MOD:
      if(oval)
         value=value%oval;
      else
         value=0;
      break;
   case YAC_OP_SHL:
      if(oval)
         value=0;
      break;
   case YAC_OP_SHR:
      if(oval)
         value=0;
      break;
   case YAC_OP_CEQ:
      YAC_RETI(value==oval);
      break;
   case YAC_OP_CNE:
      YAC_RETI(value!=oval);
      break;
   case YAC_OP_CLE:
      YAC_RETI(value<=oval);
      break;
   case YAC_OP_CLT:
      YAC_RETI(value<oval);
      break;
   case YAC_OP_CGE:
      YAC_RETI(value>=oval);
      break;
   case YAC_OP_CGT:
      YAC_RETI(value>oval);
      break;
   case YAC_OP_AND:
      value&=oval;
      YAC_RETI(value);
      break;
   case YAC_OP_OR:
      value|=oval;
      break;
   case YAC_OP_EOR:
      value^=oval;
      break;
   case YAC_OP_LAND:
      YAC_RETI(value&&oval);
      break;
   case YAC_OP_LOR:
      YAC_RETI(value||oval);
      break;
   case YAC_OP_LEOR:
      YAC_RETI(value^oval);
      break;
   }
}

void YAC_Boolean::setValue(sSI _v) {
   value=(_v&1);
}

sSI YAC_Boolean::getValue(void) {
   return value;
}

void YAC_Boolean::_getString(YAC_Value *_r) const {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING8);
   YAC_Boolean::yacToString(rs);
   YAC_RETS(rs, 1);
}

sBool YAC_VCALL YAC_Boolean::yacToString(YAC_String *rs) const {
   rs->alloc(6);
   Dyac_snprintf((char*)rs->chars, 6, "%s", (char*)(value?"true":"false"));
   rs->fixLength();
   return 1;
}
