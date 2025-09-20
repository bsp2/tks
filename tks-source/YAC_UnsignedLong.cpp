/// YAC_UnsignedLong.cpp
///
/// (c) 2006-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>

#include "tks.h"
#include "YAC_UnsignedLong.h"


YAC_UnsignedLong::YAC_UnsignedLong(void) {
  class_ID = YAC_CLID_UNSIGNEDLONG;
  value    = 0ul;
}

YAC_UnsignedLong::~YAC_UnsignedLong(void) {
}

void YAC_UnsignedLong::_Newi(sSI _v, YAC_Value *_r) {
   YAC_UnsignedLong *ro=(YAC_UnsignedLong *)YAC_NEW_CORE_POOLED(YAC_CLID_UNSIGNEDLONG);
   ro->value=(sS64)_v;
   _r->initObject(ro, 1);
}

void YAC_UnsignedLong::_News(YAC_Object *_s, YAC_Value *_r) {
   YAC_UnsignedLong *ro = (YAC_UnsignedLong *)YAC_NEW_CORE_POOLED(YAC_CLID_UNSIGNEDLONG);
   _r->initObject(ro, 1);
   if(YAC_Is_String(_s))
   {
      YAC_String *s = (YAC_String*)_s;
      s->yacScanI64((sS64*)&ro->value);  // XXX can only scan 63bit values; -1 => 0xFFFFFFFFFFFFFFFF
   }
}

sBool YAC_VCALL YAC_UnsignedLong::yacEquals(YAC_Object *_o) {
   if(YAC_Is_Long(_o))
   {
      YAC_UnsignedLong *l = (YAC_UnsignedLong*) _o;
      return (value == l->value);
   }
   return YAC_FALSE;
}

void YAC_VCALL YAC_UnsignedLong::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
  YAC_BEG_SERIALIZE();
  YAC_SERIALIZE_I64((sS64)value);
}

sUI YAC_VCALL YAC_UnsignedLong::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
  YAC_BEG_DESERIALIZE();
  value = (sU64) YAC_DESERIALIZE_I64();
  return 1u;
}

sBool YAC_VCALL YAC_UnsignedLong::yacScanI(sSI *_v) {
	*_v = (sSI)value;
	return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedLong::yacScanF32(sF32 *_v) {
	*_v = (sF32) ((sS64)value);
	return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedLong::yacScanF64(sF64 *_v) {
	*_v = (sF64) ((sS64)value);
	return YAC_TRUE;
}

sBool YAC_VCALL YAC_UnsignedLong::yacScanI64(sS64 *_v) {
	*_v = (sS64) value;
	return YAC_TRUE;
}

void YAC_VCALL YAC_UnsignedLong::yacValueOfI(sSI _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
	value = (sU64)m.u32;
}

void YAC_VCALL YAC_UnsignedLong::yacValueOfF32(sF32 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
	value = (sU64)m.u32;
}

void YAC_VCALL YAC_UnsignedLong::yacValueOfF64(sF64 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
	value = (sU64)m.u32;
}

void YAC_VCALL YAC_UnsignedLong::yacValueOfI64(sS64 _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
	value = (sU64)m.u32;
}

void YAC_VCALL YAC_UnsignedLong::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_SHL:
         YAC_Object::yacOperator(YAC_OP_SHL, _o, _r);
         break;
      case YAC_OP_NEG:
         value=(sU64) -*((sS64*)&value);
         break;
      case YAC_OP_NOT:
         value=!value;
         break;
      case YAC_OP_BITNOT:
         value=~value;
         break;
      default:
         if(YAC_VALID(_o))
         {
            yacmem64 oval;
            if(_o->yacScanI64(&oval.s64))
            {
               myOperator(_cmd, oval.u64, _r);
               return;
            }
            else
            {
               myOperator(_cmd, 0ul, _r);
               return;
            }
         }
         YAC_Object::yacOperator(_cmd, _o, _r);
         break;
   }
}

void YAC_VCALL YAC_UnsignedLong::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
   yacmem m; m.s32 = (sS32)_iv;  // prevent truncation to 0
	myOperator(_cmd, (sU64)m.u32, _r);
}

void YAC_VCALL YAC_UnsignedLong::yacOperatorF32(sSI _cmd, sF32 _fv, YAC_Value *_r) {
   yacmem m; m.s32 = (sS32)_fv;  // prevent truncation to 0
	myOperator(_cmd, (sU64)m.u32, _r);
}

void YAC_UnsignedLong::myOperator(sSI _cmd, sU64 oval, YAC_Value *_r) {
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
            value = 0ul;
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

void YAC_UnsignedLong::setValue(sSI _v) {
   yacmem m; m.s32 = (sS32)_v;  // prevent truncation to 0
   value = (sU64)m.u32;
}

sSI YAC_UnsignedLong::getValue(void) {
  return (sSI)value;
}

void YAC_UnsignedLong::_printf_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
   _printf_YAC_RARG(_s, rs);
   YAC_RETS(rs, 1);
}

void YAC_UnsignedLong::_printf_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, YAC_CLID_STRING))
   {
      YAC_String *rs = (YAC_String*) _r;

      if(YAC_BCHK(_s, YAC_CLID_STRING))
      {
         YAC_String *s=(YAC_String*)_s;
         if(rs->_realloc(s->length + 32))
         {
            Dyac_snprintf((char*)rs->chars, rs->buflen, (char*)s->chars, value);
            rs->fixLength();
         }
      }
   }
}

void YAC_UnsignedLong::_getString(YAC_Value *_r) const { // msvc doesnt support %lld printf-ormat (use %I64d ?)
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING32);
   YAC_UnsignedLong::yacToString(rs);
   YAC_RETS(rs, 1);
}

sBool YAC_VCALL YAC_UnsignedLong::yacToString(YAC_String *rs) const {
	rs->alloc(32);
	sU8 *d = rs->chars;
	sU64 t = value;
	if(t)
	{
#ifdef YAC_VC
		sU64 dv=10000000000000000000;
#else
		sU64 dv=1000000000000000000LL;
#endif
		sBool bfirstzero=0;
		while(dv)
		{
			sS8 ct=(sS8)(t/dv);
			t=t % dv;
			dv/=10;
			if(ct||bfirstzero)
			{
				bfirstzero=1;
				*d++ = (sU8) (48+ct);
			}
		}
	}
	else
		*d++ = '0';
	*d++ = 0;
	rs->length = (sSI)(d-rs->chars);
	return YAC_TRUE;
}
