/// YAC_UnsignedShort.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>

#include "tks.h"
#include "YAC_UnsignedShort.h"


YAC_UnsignedShort::YAC_UnsignedShort(void) {
   class_ID = YAC_CLID_UNSIGNEDSHORT;
   value    = 0u;
}

YAC_UnsignedShort::~YAC_UnsignedShort(void) {
}

void YAC_VCALL YAC_UnsignedShort::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_I16(value);
}

sBool YAC_VCALL YAC_UnsignedShort::yacEquals(YAC_Object *_o) {
   if(YAC_Is_UnsignedShort(_o))
   {
      YAC_UnsignedShort *us=(YAC_UnsignedShort *) _o;
      return (value == us->value);
   }
   return YAC_FALSE;
}

sUI YAC_VCALL YAC_UnsignedShort::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   value = YAC_DESERIALIZE_I16();
   return YAC_TRUE;
}

void YAC_UnsignedShort::_New (sSI _v, YAC_Value *_r) {
   YAC_UnsignedShort *ro=(YAC_UnsignedShort*)YAC_NEW_CORE_POOLED(YAC_CLID_UNSIGNEDSHORT);
   ro->value = (sU16)_v;
   _r->initObject(ro, 1);
}

sBool YAC_VCALL YAC_UnsignedShort::yacScanI(sSI *_v) {
   *_v = (sSI)value;
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedShort::yacScanF32(sF32 *_v) {
   *_v = (sF32)value;
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedShort::yacScanF64(sF64 *_v) {
   *_v = (sF64)value;
   return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedShort::yacScanI64(sS64 *_v) {
   *_v = (sS64)value;
   return YAC_TRUE;
}

void YAC_VCALL YAC_UnsignedShort::yacValueOfI(sSI _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = (sU16)m.u32;
}

void YAC_VCALL YAC_UnsignedShort::yacValueOfF32(sF32 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = (sU16)m.u32;
}

void YAC_VCALL YAC_UnsignedShort::yacValueOfF64(sF64 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = (sU16)m.u32;
}

void YAC_VCALL YAC_UnsignedShort::yacValueOfI64(sS64 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = (sU16)m.u32;
}

void YAC_VCALL YAC_UnsignedShort::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_SHL:
         YAC_Object::yacOperator(YAC_OP_SHL, _o, _r);
         break;

      case YAC_OP_NEG:
         value = (sU16) ((~value)+1);
         break;

      case YAC_OP_NOT:
         value = !value;
         break;

      case YAC_OP_BITNOT:
         value = (sU16) ~value;
         break;

      default:
         if(YAC_VALID(_o))
         {
            yacmem oval;
            if(_o->yacScanI(&oval.s32))
            {
               myOperator(_cmd, (sU16)oval.u32, _r);
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

void YAC_VCALL YAC_UnsignedShort::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
   yacmem m; m.s32 = (sS32)_iv;  // prevent truncation to 0
   myOperator(_cmd, (sU16)m.u32, _r);
}

void YAC_VCALL YAC_UnsignedShort::yacOperatorF32(sSI _cmd, sF32 _fv, YAC_Value *_r) {
   yacmem m; m.s32 = (sS32)_fv;  // prevent truncation to 0
   myOperator(_cmd, (sU16)m.u32, _r);
}

void YAC_UnsignedShort::myOperator(sSI _cmd, sU16 oval, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_ASSIGN:
         value = oval;
         break;

      case YAC_OP_ADD:
         value = (sU16)( value + oval );
         break;

      case YAC_OP_SUB:
         value = (sU16)( value - oval );
         break;

      case YAC_OP_MUL:
         value = (sU16)( value * oval );
         break;

      case YAC_OP_DIV:
         if(oval)
            value = (sU16)( value / oval );
         else
            value = 0u;
         break;

      case YAC_OP_MOD:
         if(oval)
            value = (sU16)( value % oval );
         else
            value = 0u;
         break;

      case YAC_OP_SHL:
         value = (sU16)( value<<oval );
         break;

      case YAC_OP_SHR:
         value = (sU16)( value>>oval );
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
         value = (sU16)( value & oval );
         break;

      case YAC_OP_OR:
         value = (sU16)( value | oval );
         break;

      case YAC_OP_EOR:
         value = (sU16)( value ^ oval );
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

void YAC_UnsignedShort::setValue(sSI _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = (sU16)m.u32;
}

sSI YAC_UnsignedShort::getValue(void) {
   return (sSI)value;
}

void YAC_UnsignedShort::_printf_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING64);
   _printf_YAC_RARG(_s, rs);
   YAC_RETS(rs, 1);
}

void YAC_UnsignedShort::_printf_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, YAC_CLID_STRING))
   {
      YAC_String *rs = (YAC_String*) _r;
      if(YAC_BCHK(_s, YAC_CLID_STRING))
      {
         YAC_String *s=(YAC_String*)_s;
         if(rs->_realloc(s->length + 8))
         {
            Dyac_snprintf((char*)rs->chars, rs->buflen, (char*)s->chars, (sUI)value); // xxx buffer overwrite possible
            rs->fixLength();
         }
      }
   }
}

void YAC_UnsignedShort::_getString(YAC_Value *_r) const {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING8);
   YAC_UnsignedShort::yacToString(rs);
   YAC_RETS(rs, 1);
}

sBool YAC_VCALL YAC_UnsignedShort::yacToString(YAC_String *rs) const {
   rs->alloc(8);
   Dyac_snprintf((char*)rs->chars, 8, "%u", (sUI)value);
   rs->fixLength();
   return YAC_TRUE;
}
