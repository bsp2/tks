/// YAC_Long.cpp
///
/// (c) 2004-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>

#include "tks.h"
#include "YAC_Long.h"


YAC_Long::YAC_Long(void) {
   class_ID = YAC_CLID_LONG;
   value    = 0;
}

YAC_Long::~YAC_Long(void) {
}

void YAC_Long::_Newi(sSI _v, YAC_Value *_r) {
   YAC_Long *ro = (YAC_Long *)YAC_NEW_CORE_POOLED(YAC_CLID_LONG);
   ro->value = (sS64)_v;
   _r->initObject(ro, 1);
}

void YAC_Long::_News(YAC_Object *_s, YAC_Value *_r) {
   YAC_Long *ro = (YAC_Long *)YAC_NEW_CORE_POOLED(YAC_CLID_LONG);
   _r->initObject(ro, 1);
   if(YAC_Is_String(_s))
   {
      YAC_String *s = (YAC_String*)_s;
      s->yacScanI64(&ro->value);
   }
}

sBool YAC_VCALL YAC_Long::yacEquals(YAC_Object *_o) {
   if(YAC_Is_Long(_o))
   {
      YAC_Long *l = (YAC_Long*) _o;
      return (value == l->value);
   }
   return YAC_FALSE;
}

void YAC_VCALL YAC_Long::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
  YAC_BEG_SERIALIZE();
  YAC_SERIALIZE_I64(value);
}

sUI YAC_VCALL YAC_Long::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
  YAC_BEG_DESERIALIZE();
  value=YAC_DESERIALIZE_I64();
  return 1;
}

sBool YAC_VCALL YAC_Long::yacScanI(sSI *_v) {
	*_v = (sSI)value;
	return YAC_TRUE;
}

sBool YAC_VCALL YAC_Long::yacScanF32(sF32 *_v) {
	*_v = (sF32)value;
	return YAC_TRUE;
}

sBool YAC_VCALL YAC_Long::yacScanF64(sF64 *_v) {
	*_v = (sF64)value;
	return YAC_TRUE;
}

sBool YAC_VCALL YAC_Long::yacScanI64(sS64 *_v) {
	*_v = value;
	return YAC_TRUE;
}

void YAC_VCALL YAC_Long::yacValueOfI(sSI _v) {
	value = _v;
}

void YAC_VCALL YAC_Long::yacValueOfF32(sF32 _v) {
	value = (sS64)_v;
}

void YAC_VCALL YAC_Long::yacValueOfF64(sF64 _v) {
	value = (sS64)_v;
}

void YAC_VCALL YAC_Long::yacValueOfI64(sS64 _v) {
	value = _v;
}

void YAC_VCALL YAC_Long::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
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
      	sS64 oval;
	   	if(_o->yacScanI64(&oval))
         {
			   myOperator(_cmd, oval, _r);
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

void YAC_VCALL YAC_Long::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
	myOperator(_cmd, (sS64)_iv, _r);
}

void YAC_VCALL YAC_Long::yacOperatorF32(sSI _cmd, sF32 _fv, YAC_Value *_r) {
	myOperator(_cmd, (sS64)_fv, _r);
}

void YAC_Long::myOperator(sSI _cmd, sS64 oval, YAC_Value *_r) {
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
			value = 0;
		break;
	case YAC_OP_MOD:
		if(oval)
			value = value % oval;
		else
			value = 0;
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

void YAC_Long::setValue(sSI _v) {
   value = _v;
}

sSI YAC_Long::getValue(void) {
   return (sSI)value;
}

void YAC_Long::_printf_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
   _printf_YAC_RARG(_s, rs);
   YAC_RETS(rs, 1);
}

void YAC_Long::_printf_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, YAC_CLID_STRING))
   {
      YAC_String *rs = (YAC_String *) _r;
      if(YAC_BCHK(_s, YAC_CLID_STRING))
      {
         YAC_String *s = (YAC_String*)_s;
         if(rs->_realloc(s->length + 32))
         {
            Dyac_snprintf((char*)rs->chars, rs->buflen, (char*)s->chars, value);
            rs->fixLength();
         }
      }
   }
}

void YAC_Long::_getString(YAC_Value *_r) const {
   //
   // Note: msvc6 doesnt support %lld printf-ormat
   //
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING); // xxx use prebuf string64
	YAC_Long::yacToString(rs);
	YAC_RETS(rs, 1);
}

sBool YAC_VCALL YAC_Long::yacToString(YAC_String *rs) const {
	rs->alloc(64);
	sU8 *d = rs->chars;
	sS64 t = value;
	if(t < 0)
	{
		*d++ = '-';
		t = -t;
	}
	if(t)
	{
#ifdef YAC_VC
		sS64 dv = 1000000000000000000;
#else
		sS64 dv = 1000000000000000000LL;
#endif
		sBool bfirstzero = 0;
		while(dv)
		{
			sS8 ct = (sS8)(t / dv);
			t = t % dv;
			dv /= 10;
			if(ct || bfirstzero)
			{
				bfirstzero = 1;
				*d++ = (sU8) (48+ct);
			}
		}
	}
	else
		*d++ = '0';
	*d++ = 0;
	rs->length=(sSI)(d-rs->chars);
	return 1;
}
