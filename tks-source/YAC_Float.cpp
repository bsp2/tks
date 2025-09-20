/// YAC_Float.cpp
///
/// (c) 2004-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>

#include "tks.h"
#include "YAC_Float.h"


YAC_Float::YAC_Float(void) {
   class_ID = YAC_CLID_FLOAT;
   value    = 0.0f;
}

YAC_Float::~YAC_Float(void) {
}

void YAC_Float::_New (sF32 _v, YAC_Value *_r) {
   YAC_Float *ro=(YAC_Float *)YAC_NEW_CORE_POOLED(YAC_CLID_FLOAT);
   ro->value=_v;
   _r->initObject(ro, 1);
}

void YAC_VCALL YAC_Float::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F32(value);
}

sUI YAC_VCALL YAC_Float::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   value=YAC_DESERIALIZE_F32();
   return 1;
}

sBool YAC_VCALL YAC_Float::yacEquals(YAC_Object *_o) {
   if(YAC_Is_Float(_o))
   {
      YAC_Float *f=(YAC_Float*)_o;
      return (value>=(f->value-YAC_FLOAT_DEVIATION))&&(value<=(f->value+YAC_FLOAT_DEVIATION));
   }
   return 0;
}

sBool YAC_VCALL YAC_Float::yacScanI(sSI *_v) {
   *_v=(sSI)value;
   return 1;
}

sBool YAC_VCALL YAC_Float::yacScanF32(sF32 *_v) {
   *_v=(sF32)value;
   return 1;
}

sBool YAC_VCALL YAC_Float::yacScanF64(sF64 *_v) {
   *_v=(sF64)value;
   return 1;
}

sBool YAC_VCALL YAC_Float::yacScanI64(sS64 *_v) {
   *_v=(sS64)value;
   return 1;
}

void YAC_VCALL YAC_Float::yacValueOfI(sSI _v) {
   value=(sF32)_v;
}

void YAC_VCALL YAC_Float::yacValueOfF32(sF32 _v) {
   value=_v;
}

void YAC_VCALL YAC_Float::yacValueOfF64(sF64 _v) {
   value=(sF32)_v;
}

void YAC_VCALL YAC_Float::yacValueOfI64(sS64 _v) {
   value=(sF32)_v;
}

void YAC_VCALL YAC_Float::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   sF32 oval;
   switch(_cmd)
   {
      case YAC_OP_SHL:
         YAC_Object::yacOperator(YAC_OP_SHL, _o, _r);
         break;

      case YAC_OP_NEG:
         value=-value;
         break;

      default:
         if(YAC_VALID(_o))
         {
            if(_o->yacScanF32(&oval))
            {
               myOperator(_cmd, oval, _r);
               return;
            }
            else
            {
               myOperator(_cmd, 0.0f, _r);
               return;
            }
         }
         YAC_Object::yacOperator(_cmd, _o, _r);
         break;
   }
}

void YAC_VCALL YAC_Float::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
   myOperator(_cmd, (sF32)_iv, _r);
}

void YAC_VCALL YAC_Float::yacOperatorF32(sSI _cmd, sF32 _fv, YAC_Value *_r) {
   myOperator(_cmd, _fv, _r);
}

void YAC_Float::myOperator(sSI _cmd, sF32 oval, YAC_Value *_r) {
   switch(_cmd)
   {
   case YAC_OP_ASSIGN:  value=oval;  break;
   case YAC_OP_ADD:     value+=oval; break;
   case YAC_OP_SUB:     value-=oval; break;
   case YAC_OP_MUL:     value*=oval; break;
   case YAC_OP_DIV:     if(Dfltnonzero(oval))value/=oval; break;
   case YAC_OP_MOD:
      if(((sSI)oval))
         value=(sF32)(((sSI)value)%((sSI)oval));
      break;
   case YAC_OP_SHL:
      value=(sF32)(((sSI)value)<<((sSI)oval));
      break;
   case YAC_OP_SHR:
      value=(sF32)(((sSI)value)>>((sSI)oval));
      break;
   case YAC_OP_CEQ:  YAC_RETI(Dfltequal(value, oval)); break;
   case YAC_OP_CNE:  YAC_RETI(Dfltnotequal(value, oval)); break;
   case YAC_OP_CLE:  YAC_RETI(value<=oval); break;
   case YAC_OP_CLT:  YAC_RETI(value<oval);  break;
   case YAC_OP_CGE:  YAC_RETI(value>=oval); break;
   case YAC_OP_CGT:  YAC_RETI(value>oval);  break;
   case YAC_OP_AND:
      //value=(sF32)(((sSI)value)&((sSI)oval));
      YAC_RETI( (((sSI)value)&((sSI)oval)) );
      value=(sF32)_r->value.int_val;
      break;
   case YAC_OP_OR:
      value=(sF32)(((sSI)value)|((sSI)oval));
      break;
   case YAC_OP_EOR:
      value=(sF32)(((sSI)value)^((sSI)oval));
      break;
   case YAC_OP_LAND:
      YAC_RETI(((sSI)value)&&((sSI)oval));
      break;
   case YAC_OP_LOR:
      YAC_RETI(((sSI)value)||((sSI)oval));
      break;
   case YAC_OP_LEOR:
      YAC_RETI(((sSI)value)^((sSI)oval));
      break;
   }
}

void YAC_Float::setValue(sF32 _v) {
   value=_v;
}

sF32 YAC_Float::getValue(void) {
   return value;
}

void YAC_Float::_printf_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING64);
   _printf_YAC_RARG(_s, rs);
   YAC_RETS(rs, 1);
}

void YAC_Float::_printf_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, YAC_CLID_STRING))
   {
      YAC_String *rs = (YAC_String*) _r;

      if(YAC_BCHK(_s, YAC_CLID_STRING))
      {
         YAC_String *s=(YAC_String*)_s;
         if(rs->_realloc(s->length + 32))
         {
            Dyac_snprintf((char*)rs->chars, rs->buflen, (char*)s->chars, value); // xxx buffer overwrite possible
            rs->fixLength();
         }
      }
   }
}


void YAC_Float::_getString(YAC_Value *_r) const {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING); // xxx use prebuf string64
   YAC_Float::yacToString(rs);
   YAC_RETS(rs, 1);
}

sBool YAC_VCALL YAC_Float::yacToString(YAC_String *rs) const {
   rs->alloc(32);
   Dyac_snprintf((char*)rs->chars, 32, "%g", value);
   rs->fixLength();
   return 1;
}
