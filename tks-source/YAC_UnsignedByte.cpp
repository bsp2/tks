/// YAC_UnsignedByte.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>

#include "tks.h"
#include "YAC_UnsignedByte.h"


YAC_UnsignedByte::YAC_UnsignedByte(void) {
   class_ID = YAC_CLID_UNSIGNEDBYTE;
   value    = 0u;
}

YAC_UnsignedByte::~YAC_UnsignedByte(void) {
}

void YAC_UnsignedByte::_New (sSI _v, YAC_Value *_r) {
   YAC_UnsignedByte *ro=(YAC_UnsignedByte *)YAC_NEW_CORE_POOLED(YAC_CLID_UNSIGNEDBYTE);
   ro->value=(sU8)_v;
   _r->initObject(ro, 1);
}

sBool YAC_VCALL YAC_UnsignedByte::yacEquals(YAC_Object *_o) {
   if(YAC_Is_UnsignedByte(_o))
   {
      YAC_UnsignedByte *ub=(YAC_UnsignedByte*) _o;
      return (value == ub->value);
   }
   return YAC_FALSE;
}

void YAC_VCALL YAC_UnsignedByte::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_I8(value);
}

sUI YAC_VCALL YAC_UnsignedByte::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   value = YAC_DESERIALIZE_I8();
   return 1u;
}

sBool YAC_VCALL YAC_UnsignedByte::yacScanI(sSI *_v) {
   *_v = (sSI)value;
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedByte::yacScanF32(sF32 *_v) {
   *_v = (sF32)value;
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedByte::yacScanF64(sF64 *_v) {
   *_v = (sF64)value;
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedByte::yacScanI64(sS64 *_v) {
   *_v = (sS64)value;
   return YAC_TRUE;
}

void YAC_VCALL YAC_UnsignedByte::yacValueOfI(sSI _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = (sU8)m.u32;
}

void YAC_VCALL YAC_UnsignedByte::yacValueOfF32(sF32 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = (sU8)m.u32;
}

void YAC_VCALL YAC_UnsignedByte::yacValueOfF64(sF64 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = (sU8)m.u32;
}

void YAC_VCALL YAC_UnsignedByte::yacValueOfI64(sS64 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = (sU8)m.u32;
}

void YAC_VCALL YAC_UnsignedByte::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_SHL:
         YAC_Object::yacOperator(YAC_OP_SHL, _o, _r);
         break;

      case YAC_OP_NEG:
         value = (sU8) ((~value)+1);
         break;

      case YAC_OP_NOT:
         value = !value;
         break;

      case YAC_OP_BITNOT:
         value = (sU8) ~value;
         break;

      default:
         if(YAC_VALID(_o))
         {
            yacmem oval;
            if(_o->yacScanI(&oval.s32))
            {
               myOperator(_cmd, (sU8)oval.u32, _r);
               return;
            }
            else
            {
               myOperator(_cmd, 0u, _r);
               return;
            }
         }
         YAC_Object::yacOperator(_cmd, _o, _r);
         break;
   }
}

void YAC_VCALL YAC_UnsignedByte::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
   yacmem m; m.s32 = (sS32)_iv;  // prevent truncation to 0
   myOperator(_cmd, (sU8)m.u32, _r);
}

void YAC_VCALL YAC_UnsignedByte::yacOperatorF32(sSI _cmd, sF32 _fv, YAC_Value *_r) {
   yacmem m; m.s32 = (sS32)_fv;  // prevent truncation to 0
   myOperator(_cmd, (sU8)m.u32, _r);
}

void YAC_UnsignedByte::myOperator(sSI _cmd, sU8 oval, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_NEG:
         value = (sU8)-oval;
         break;

      case YAC_OP_NOT:
         value = !oval;
         break;

      case YAC_OP_BITNOT:
         value = (sU8) ~oval;
         break;

      case YAC_OP_ASSIGN:
         value = oval;
         break;

      case YAC_OP_ADD:
         value = (sU8)( value + oval );
         break;

      case YAC_OP_SUB:
         value = (sU8)( value - oval );
         break;

      case YAC_OP_MUL:
         value = (sU8)( value * oval );
         break;

      case YAC_OP_DIV:
         if(oval)
            value = (sU8)( value / oval );
         else
            value = 0u;
         break;

      case YAC_OP_MOD:
         if(oval)
            value = (sU8)( value % oval );
         else
            value = 0u;
         break;

      case YAC_OP_SHL:
         value = (sU8)( value << oval );
         break;

      case YAC_OP_SHR:
         value = (sU8)( value >> oval );
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
         value = (sU8)( value & oval );
         break;

      case YAC_OP_OR:
         value = (sU8)( value | oval );
         break;

      case YAC_OP_EOR:
         value = (sU8)( value ^ oval );
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

void YAC_UnsignedByte::setValue(sSI _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = (sU8)m.u32;
}

sSI YAC_UnsignedByte::getValue(void) {
   return (sSI)value;
}

void YAC_UnsignedByte::_printf_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING64);
   _printf_YAC_RARG(_s, rs);
   YAC_RETS(rs, 1);
}

void YAC_UnsignedByte::_printf_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, YAC_CLID_STRING))
   {
      YAC_String *rs = (YAC_String*) _r;

      if(YAC_BCHK(_s, YAC_CLID_STRING))
      {
         YAC_String *s = (YAC_String*)_s;
         if(rs->_realloc(s->length + 8))
         {
            Dyac_snprintf((char*)rs->chars, rs->buflen, (char*)s->chars, value);
            rs->fixLength();
         }
      }
   }
}

void YAC_UnsignedByte::_getString(YAC_Value *_r) const {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING8);
   YAC_UnsignedByte::yacToString(rs);
   YAC_RETS(rs, 1);
}

sBool YAC_VCALL YAC_UnsignedByte::yacToString(YAC_String *rs) const {
   rs->alloc(8);
   Dyac_snprintf((char*)rs->chars, 8, "%u", (sUI)value);
   rs->fixLength();
   return YAC_TRUE;
}
