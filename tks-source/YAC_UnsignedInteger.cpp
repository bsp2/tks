/// YAC_UnsignedInteger.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>

#include "tks.h"
#include "YAC_UnsignedInteger.h"


YAC_UnsignedInteger::YAC_UnsignedInteger(void) {
   class_ID = YAC_CLID_UNSIGNEDINTEGER;
   value    = 0u;
}

YAC_UnsignedInteger::~YAC_UnsignedInteger(void) { }

void YAC_UnsignedInteger::_New (sSI _v, YAC_Value *_r) {
   YAC_UnsignedInteger *ro = (YAC_UnsignedInteger *)YAC_NEW_CORE_POOLED(YAC_CLID_UNSIGNEDINTEGER);
   ro->value = *(sUI*)&_v;
   _r->initObject(ro, 1);
}

sBool YAC_VCALL YAC_UnsignedInteger::yacEquals(YAC_Object *_o) {
   if(YAC_Is_UnsignedInteger(_o))
   {
      YAC_UnsignedInteger *ub=(YAC_UnsignedInteger *) _o;
      return (value == ub->value);
   }
   return YAC_FALSE;
}

void YAC_VCALL YAC_UnsignedInteger::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_I32(value);
}

sUI YAC_VCALL YAC_UnsignedInteger::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   value=YAC_DESERIALIZE_I32();
   return 1u;
}

sBool YAC_VCALL YAC_UnsignedInteger::yacScanI(sSI *_v) {
   *_v = (sSI)value;
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedInteger::yacScanF32(sF32 *_v) {
   *_v = (sF32)value;
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedInteger::yacScanF64(sF64 *_v) {
   *_v = (sF64)value;
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedInteger::yacScanI64(sS64 *_v) {
   *_v = (sS64)value;
   return YAC_TRUE;
}

void YAC_VCALL YAC_UnsignedInteger::yacValueOfI(sSI _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = m.u32;
}

void YAC_VCALL YAC_UnsignedInteger::yacValueOfF32(sF32 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = m.u32;
}

void YAC_VCALL YAC_UnsignedInteger::yacValueOfF64(sF64 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = m.u32;
}

void YAC_VCALL YAC_UnsignedInteger::yacValueOfI64(sS64 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = m.u32;
}

void YAC_VCALL YAC_UnsignedInteger::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_SHL:
         YAC_Object::yacOperator(YAC_OP_SHL, _o, _r);
         break;

      case YAC_OP_NEG:
         value = ~value + 1u;
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
            yacmem oval;
            if(_o->yacScanI(&oval.s32))
            {
               myOperator(_cmd, oval.u32, _r);
            }
            else
            {
               myOperator(_cmd, 0u, _r);
            }
            return;
         }
         YAC_Object::yacOperator(_cmd, _o, _r);
         break;
   }
}

void YAC_VCALL YAC_UnsignedInteger::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
   yacmem m; m.s32 = _iv;  // prevent truncation to 0
   myOperator(_cmd, m.u32, _r);
}

void YAC_VCALL YAC_UnsignedInteger::yacOperatorF32(sSI _cmd, sF32 _fv, YAC_Value *_r) {
   yacmem m; m.s32 = (sS32)_fv;  // prevent truncation to 0
   myOperator(_cmd, m.u32, _r);
}

void YAC_UnsignedInteger::myOperator(sSI _cmd, sU32 oval, YAC_Value *_r) {
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
            value /= oval;
         else
            value = 0u;
         break;

      case YAC_OP_MOD:
         if(oval)
            value = value % oval;
         else
            value = 0u;
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
         value &= oval;
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

void YAC_UnsignedInteger::setValue(sSI _v) {
   yacmem m; m.s32 = _v;  // prevent truncation to 0
   value = m.u32;
}

sSI YAC_UnsignedInteger::getValue(void) {
   return (sSI)value;
}

void YAC_UnsignedInteger::_printf_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING64);
   _printf_YAC_RARG(_s, rs);
   YAC_RETS(rs, 1);
}

void YAC_UnsignedInteger::_printf_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, YAC_CLID_STRING))
   {
      YAC_String *rs = (YAC_String*) _r;

      if(YAC_BCHK(_s, YAC_CLID_STRING))
      {
         YAC_String *s = (YAC_String*)_s;
         if(rs->_realloc(s->length + 16))
         {
            Dyac_snprintf((char*)rs->chars, rs->buflen, (char*)s->chars, (sUI)value);
            rs->fixLength();
         }
      }
   }
}

void YAC_UnsignedInteger::_getString(YAC_Value *_r) const {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING16);
   YAC_UnsignedInteger::yacToString(rs);
   YAC_RETS(rs, 1);
}

sBool YAC_VCALL YAC_UnsignedInteger::yacToString(YAC_String *rs) const {
   rs->alloc(16);
   Dyac_snprintf((char*)rs->chars, 16, "%u", (sUI)value);
   rs->fixLength();
   return YAC_TRUE;
}
