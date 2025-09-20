/// YAC_Double.cpp
///
/// (c) 2004-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>

#include "tks.h"
#include "YAC_Double.h"


YAC_Double::YAC_Double(void) {
  class_ID = YAC_CLID_DOUBLE;
  value    = 0.0;
}

YAC_Double::~YAC_Double(void) {
}

void YAC_Double::_Newi(sSI _v, YAC_Value *_r) {
   YAC_Double *ro=(YAC_Double *)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   ro->value=(sF64)_v;
   _r->initObject(ro, 1);
}

void YAC_Double::_Newf(sF32 _v, YAC_Value *_r) {
   YAC_Double *ro=(YAC_Double *)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   ro->value=(sF64)_v;
   _r->initObject(ro, 1);
}

void YAC_Double::_News(YAC_Object *_s, YAC_Value *_r) {
   YAC_Double *ro=(YAC_Double *)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(ro, 1);
   if(YAC_Is_String(_s))
   {
      YAC_String *s=(YAC_String*)_s;
      s->yacScanF64(&ro->value);
   }
}

sBool YAC_VCALL YAC_Double::yacEquals(YAC_Object *_o) {
   if(YAC_Is_Double(_o))
   {
      YAC_Double *f=(YAC_Double *) _o;
      return (value>=(f->value-YAC_DOUBLE_DEVIATION))&&(value<=(f->value+YAC_DOUBLE_DEVIATION));
   }
   return 0;
}

void YAC_VCALL YAC_Double::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
  YAC_BEG_SERIALIZE();
  _ofs->yacStreamWriteF64(value);
}

sUI YAC_VCALL YAC_Double::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
	YAC_BEG_DESERIALIZE();
	value=(sF64)_ifs->yacStreamReadF64();
	return 1;
}

sBool YAC_VCALL YAC_Double::yacScanI(sSI *_v) {
	*_v=(sSI)value;
	return 1;
}

sBool YAC_VCALL YAC_Double::yacScanF32(sF32 *_v) {
	*_v=(sF32)value;
	return 1;
}

sBool YAC_VCALL YAC_Double::yacScanF64(sF64 *_v) {
	*_v=(sF64)value;
	return 1;
}

sBool YAC_VCALL YAC_Double::yacScanI64(sS64 *_v) {
	*_v=(sS64)value;
	return 1;
}

void YAC_VCALL YAC_Double::yacValueOfI(sSI _v) {
	value=(sF64)_v;
}

void YAC_VCALL YAC_Double::yacValueOfF32(sF32 _v) {
	value=(sF64)_v;
}

void YAC_VCALL YAC_Double::yacValueOfF64(sF64 _v) {
	value=_v;
}

void YAC_VCALL YAC_Double::yacValueOfI64(sS64 _v) {
	value=(sF64)_v;
}

void YAC_VCALL YAC_Double::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
	sF64 oval;
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
   	   	if(_o->yacScanF64(&oval))
            {
			      myOperator(_cmd, oval, _r);
               return;
            }
            else
            {
               myOperator(_cmd, 0.0, _r);
               return;
            }
         }
		   YAC_Object::yacOperator(_cmd, _o, _r);
         break;
   }
}

void YAC_VCALL YAC_Double::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
	myOperator(_cmd, (sF64)_iv, _r);
}

void YAC_VCALL YAC_Double::yacOperatorF32(sSI _cmd, sF32 _fv, YAC_Value *_r) {
	myOperator(_cmd, (sF64)_fv, _r);
}

void YAC_Double::myOperator(sSI _cmd, sF64 oval, YAC_Value *_r) {
	switch(_cmd)
	{
	case YAC_OP_ASSIGN:  value  = oval; break;
	case YAC_OP_ADD:     value += oval; break;
	case YAC_OP_SUB:     value -= oval; break;
	case YAC_OP_MUL:     value *= oval; break;
	case YAC_OP_DIV:     if(Ddblnonzero(oval))value/=oval;      break;
	case YAC_OP_MOD:
		if(((sSI)oval))
			value=(sF64)(((sSI)value)%((sSI)oval));
		break;
	case YAC_OP_SHL:
		value=(sF64)(((sSI)value)<<((sSI)oval));
		break;
	case YAC_OP_SHR:
		value=(sF64)(((sSI)value)>>((sSI)oval));
		break;
	case YAC_OP_CEQ:  YAC_RETI(Ddblequal(value, oval)); break;
	case YAC_OP_CNE:  YAC_RETI(Ddblnotequal(value, oval)); break;
	case YAC_OP_CLE:  YAC_RETI(value<=oval); break;
	case YAC_OP_CLT:  YAC_RETI(value<oval);  break;
	case YAC_OP_CGE:  YAC_RETI(value>=oval); break;
	case YAC_OP_CGT:  YAC_RETI(value>oval);  break;
	case YAC_OP_AND:
		//value=(sF64)(((sSI)value)&((sSI)oval));
		YAC_RETI( (((sSI)value)&((sSI)oval)) );
		value=(sF64)_r->value.int_val;
		break;
	case YAC_OP_OR:
		value=(sF64)(((sSI)value)|((sSI)oval));
		break;
	case YAC_OP_EOR:
		value=(sF64)(((sSI)value)^((sSI)oval));
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

void YAC_Double::setValue(sF32 _v) {
    value=(sF64)_v;
}

sF32 YAC_Double::getValue(void) {
    return (sF32)value;
}

void YAC_Double::_printf_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
   _printf_YAC_RARG(_s, rs);
   YAC_RETS(rs, 1);
}

void YAC_Double::_printf_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, YAC_CLID_STRING))
   {
      YAC_String *rs = (YAC_String *) _r;

      if(YAC_BCHK(_s, YAC_CLID_STRING))
      {
         YAC_String *s = (YAC_String*)_s;
         if(rs->_realloc(s->length + 128))
         {
            Dyac_snprintf((char*)rs->chars, rs->buflen, (char*)s->chars, value);
            rs->fixLength();
         }
      }
   }
}

void YAC_Double::_getString(YAC_Value *_r) const {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING); // xxx use prebuffered string
   YAC_Double::yacToString(rs);
   YAC_RETS(rs, 1);
}

sBool YAC_VCALL YAC_Double::yacToString(YAC_String *rs) const {
   rs->alloc(128);
   Dyac_snprintf((char*)rs->chars, 128, "%.64lg", value);
   rs->fixLength();
	return 1;
}
