/// YAC_Integer.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>

#include "tks.h"
#include "YAC_Integer.h"


YAC_Integer::YAC_Integer(void) {
   class_ID = YAC_CLID_INTEGER;
   value    = 0;
}

YAC_Integer::~YAC_Integer(void) {
}

void YAC_Integer::_New (sSI _v, YAC_Value *_r) {
   YAC_Integer *ro = (YAC_Integer *) YAC_NEW_CORE_POOLED(YAC_CLID_INTEGER);
   ro->value = _v;
   _r->initObject(ro, 1);
}

sBool YAC_VCALL YAC_Integer::yacEquals(YAC_Object *_o) {
   if(YAC_Is_Integer(_o))
   {
      YAC_Integer *i = (YAC_Integer*) _o;
      return (value == i->value);
   }
   return 0;
}

void YAC_VCALL YAC_Integer::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_I32(value);
}

sUI YAC_VCALL YAC_Integer::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   value=YAC_DESERIALIZE_I32();
   return 1;
}

sBool YAC_VCALL YAC_Integer::yacScanI(sSI *_v) {
   *_v=(sSI)value;
   return 1;
}

sBool YAC_VCALL YAC_Integer::yacScanF32(sF32 *_v) {
   *_v=(sF32)value;
   return 1;
}

sBool YAC_VCALL YAC_Integer::yacScanF64(sF64 *_v) {
   *_v=(sF64)value;
   return 1;
}

sBool YAC_VCALL YAC_Integer::yacScanI64(sS64 *_v) {
   *_v=(sS64)value;
   return 1;
}

void YAC_VCALL YAC_Integer::yacValueOfI(sSI _v) {
   value=_v;
}

void YAC_VCALL YAC_Integer::yacValueOfF32(sF32 _v) {
   value=(sSI)_v;
}

void YAC_VCALL YAC_Integer::yacValueOfF64(sF64 _v) {
   value=(sSI)_v;
}

void YAC_VCALL YAC_Integer::yacValueOfI64(sS64 _v) {
   value=(sSI)_v;
}

void YAC_VCALL YAC_Integer::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   switch(_cmd)
   {
   case YAC_OP_SHL:
      YAC_Object::yacOperator(YAC_OP_SHL, _o, _r);
      break;
   case YAC_OP_NEG:
      value = -value;
      break;
   case YAC_OP_NOT:
      value = !value;
      break;
   case YAC_OP_BITNOT:
      value = ~value;
      break;
   default:
      if(YAC_VALID(_o))
      {
         sSI oval;
         if(_o->yacScanI(&oval))
         {
            myOperator(_cmd, (sS32)oval, _r);
            return;
         }
         else
         {
            myOperator(_cmd, 0, _r);
            return;
         }
      }
      YAC_Object::yacOperator(_cmd, _o, _r);
      break;
   }
}

void YAC_VCALL YAC_Integer::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
   myOperator(_cmd, (sS32)_iv, _r);
}

void YAC_VCALL YAC_Integer::yacOperatorF32(sSI _cmd, sF32 _fv, YAC_Value *_r) {
   myOperator(_cmd, (sS32)_fv, _r);
}

void YAC_Integer::myOperator(sSI _cmd, sS32 oval, YAC_Value *_r) {
   switch(_cmd)
   {
   case YAC_OP_ASSIGN:
      value = oval;
      break;
   case YAC_OP_ADD:
      value += oval;
      break;
   case YAC_OP_SUB:
      value -= oval;
      break;
   case YAC_OP_MUL:
      value *= oval;
      break;
   case YAC_OP_DIV:
      if(oval)
      {
         value/=oval;
      }
      else
      {
         value=0;
      }
      break;
   case YAC_OP_MOD:
      if(oval)
      {
         value = value % oval;
      }
      else
      {
         value=0;
      }
      break;
   case YAC_OP_SHL:
      value = value << oval;
      break;
   case YAC_OP_SHR:
      value = value >> oval;
      break;
   case YAC_OP_CEQ:
      YAC_RETI(value == oval);
      break;
   case YAC_OP_CNE:
      YAC_RETI(value != oval);
      break;
   case YAC_OP_CLE:
      YAC_RETI(value <= oval);
      break;
   case YAC_OP_CLT:
      YAC_RETI(value < oval);
      break;
   case YAC_OP_CGE:
      YAC_RETI(value >= oval);
      break;
   case YAC_OP_CGT:
      YAC_RETI(value > oval);
      break;
   case YAC_OP_AND:
      value&=oval;
      YAC_RETI(value);
      break;
   case YAC_OP_OR:
      value |= oval;
      break;
   case YAC_OP_EOR:
      value ^= oval;
      break;
   case YAC_OP_LAND:
      YAC_RETI(value && oval);
      break;
   case YAC_OP_LOR:
      YAC_RETI(value || oval);
      break;
   case YAC_OP_LEOR:
      YAC_RETI(value ^ oval);
      break;
   }
}

void YAC_Integer::setValue(sSI _v) {
   value=_v;
}

sSI YAC_Integer::getValue(void) {
   return value;
}

void YAC_Integer::_printf_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING64);
   _printf_YAC_RARG(_s, rs);
   YAC_RETS(rs, 1);
}

void YAC_Integer::_printf_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, YAC_CLID_STRING))
   {
      YAC_String *rs = (YAC_String *) _r;

      if(YAC_BCHK(_s, YAC_CLID_STRING))
      {
         YAC_String *s = (YAC_String*)_s;
         if(rs->_realloc(s->length + 16))
         {
            Dyac_snprintf((char*)rs->chars, rs->buflen, (char*)s->chars, value);
            rs->fixLength();
         }
      }
   }
}

void YAC_Integer::_getString(YAC_Value *_r) const {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING); // xxx use prebuf string16
   YAC_Integer::yacToString(rs);
   YAC_RETS(rs, 1);
}

sBool YAC_VCALL YAC_Integer::yacToString(YAC_String *rs) const {
   rs->alloc(16);
   Dyac_snprintf((char*)rs->chars, 16, "%d", value);
   rs->fixLength();
   return 1;
}
