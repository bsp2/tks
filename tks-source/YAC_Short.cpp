/// YAC_Short.cpp
///
/// (c) 2004-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>

#include "tks.h"
#include "YAC_Short.h"


YAC_Short::YAC_Short(void) {
   class_ID = YAC_CLID_SHORT;
   value    = 0;
}

YAC_Short::~YAC_Short(void) { }

void YAC_Short::_New (sSI _v, YAC_Value *_r) {
   YAC_Short *ro=(YAC_Short *)YAC_NEW_CORE_POOLED(YAC_CLID_SHORT);
   ro->value=(sS16)_v;
   _r->initObject(ro, 1);
}

sBool YAC_VCALL YAC_Short::yacEquals(YAC_Object *_o) {
   if(YAC_Is_Short(_o))
   {
      YAC_Short *s=(YAC_Short*) _o;
      return (value == s->value);
   }
   return 0;
}

void YAC_VCALL YAC_Short::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_I16(value);
}

sUI YAC_VCALL YAC_Short::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   value=(sS16)YAC_DESERIALIZE_I16();
   return 1;
}

sBool YAC_VCALL YAC_Short::yacScanI(sSI *_v) {
   *_v=(sSI)value;
   return 1;
}

sBool YAC_VCALL YAC_Short::yacScanF32(sF32 *_v) {
   *_v=(sF32)value;
   return 1;
}

sBool YAC_VCALL YAC_Short::yacScanF64(sF64 *_v) {
   *_v=(sF64)value;
   return 1;
}

sBool YAC_VCALL YAC_Short::yacScanI64(sS64 *_v) {
   *_v=(sS64)value;
   return 1;
}

void YAC_VCALL YAC_Short::yacValueOfI(sSI _v) {
   value=(sS16)_v;
}

void YAC_VCALL YAC_Short::yacValueOfF32(sF32 _v) {
   value=(sS16)_v;
}

void YAC_VCALL YAC_Short::yacValueOfF64(sF64 _v) {
   value=(sS16)_v;
}

void YAC_VCALL YAC_Short::yacValueOfI64(sS64 _v) {
   value=(sS16)_v;
}

void YAC_VCALL YAC_Short::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   switch(_cmd)
   {
   case YAC_OP_SHL:
      YAC_Object::yacOperator(YAC_OP_SHL, _o, _r);
      break;
   case YAC_OP_NEG:
      value = (sS16) -value;
      break;
   case YAC_OP_NOT:
      value = !value;
      break;
   case YAC_OP_BITNOT:
      value = (sS16) ~value;
      break;
   default:
      if(YAC_VALID(_o))
      {
         sSI oval;
         if(_o->yacScanI(&oval))
         {
            myOperator(_cmd, (sS16)oval, _r);
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

void YAC_VCALL YAC_Short::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
   myOperator(_cmd, (sS16)_iv, _r);
}

void YAC_VCALL YAC_Short::yacOperatorF32(sSI _cmd, sF32 _fv, YAC_Value *_r) {
   myOperator(_cmd, (sS16)_fv, _r);
}

void YAC_Short::myOperator(sSI _cmd, sS16 oval, YAC_Value *_r) {
   switch(_cmd)
   {
   case YAC_OP_ASSIGN:
      value = oval;
      break;
   case YAC_OP_ADD:
      value = (sS16)( value + oval );
      break;
   case YAC_OP_SUB:
      value = (sS16)( value - oval );
      break;
   case YAC_OP_MUL:
      value = (sS16)( value * oval );
      break;
   case YAC_OP_DIV:
      if(oval)
         value = (sS16)( value / oval );
      else
         value = 0;
      break;
   case YAC_OP_MOD:
      if(oval)
         value = (sS16) (value % oval);
      else
         value = 0;
      break;
   case YAC_OP_SHL:
      value = (sS16)( value<<oval );
      break;
   case YAC_OP_SHR:
      value = (sS16)( value>>oval );
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
      value = (sS16)( value & oval );
      YAC_RETI(value);
      break;
   case YAC_OP_OR:
      value = (sS16)( value | oval );
      break;
   case YAC_OP_EOR:
      value = (sS16)( value ^ oval );
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

void YAC_Short::setValue(sSI _v) {
   value=(sS16)_v;
}

sSI YAC_Short::getValue(void) {
   return (sSI)value;
}

void YAC_Short::_printf_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING64);
   _printf_YAC_RARG(_s, rs);
   YAC_RETS(rs, 1);
}

void YAC_Short::_printf_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, YAC_CLID_STRING))
   {
      YAC_String *rs = (YAC_String *) _r;

      if(YAC_BCHK(_s, YAC_CLID_STRING))
      {
         YAC_String *s=(YAC_String*)_s;
         if(rs->_realloc(s->length + 8))
         {
            Dyac_snprintf((char*)rs->chars, rs->buflen, (char*)s->chars, (sSI)value);
            rs->fixLength();
         }
      }
   }
}

void YAC_Short::_getString(YAC_Value *_r) const {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING); // xxx use prebuf string16
   YAC_Short::yacToString(rs);
   YAC_RETS(rs, 1);
}

sBool YAC_VCALL YAC_Short::yacToString(YAC_String *rs) const {
   rs->alloc(8);
   Dyac_snprintf((char*)rs->chars, 8, "%d", (sSI)value);
   rs->fixLength();
   return 1;
}
