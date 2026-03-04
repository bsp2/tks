sSI BigFloat::base;

BigFloat::BigFloat(void) {
	mpf_init(bigfloat);
	mpf_init(tmp);
	digits=20;
	base=10;
	precision=mpf_get_prec(bigfloat);
}

BigFloat::~BigFloat(void) {
	mpf_clear(tmp);
	mpf_clear(bigfloat);
}

sBool BigFloat::yacToString(YAC_String *_s) const {
#ifdef YAC_BIGSTRING
	mp_exp_t exponent;
	_s->alloc(digits+2+2+13);
//	((char*)_s->chars)[0]='0';
//	((char*)_s->chars)[1]='.';
	mpf_get_str((char*)_s->chars+1,&exponent,base,digits,bigfloat);
	((char*)_s->chars)[0]=((char*)_s->chars)[1];
	((char*)_s->chars)[1]='.';
	char *pos=(char*)_s->chars+2;
	while (*pos) pos++;
	if (exponent!=1) {
		*pos='E';
		pos++;
		*pos=0;
		_s->fixLength();
		_s->yacOperatorI(YAC_OP_ADD,exponent-1,NULL);
	} else _s->fixLength();
	return 1;
#endif
	return 0;
}

sBool YAC_VCALL BigFloat::yacScanI(sSI *val) {
	if (!mpf_fits_sint_p(bigfloat)) return 0;
//	double fVal=mpf_get_d(bigfloat);
	*val=mpf_get_si(bigfloat);
	return 1;
}

sBool YAC_VCALL BigFloat::yacScanF32(sF32 *val) {
	*val=mpf_get_d(bigfloat);
	return 1;
}

sBool YAC_VCALL BigFloat::yacScanF64(sF64 *val) {
	*val=mpf_get_d(bigfloat);
	return 1;
}

sBool YAC_VCALL BigFloat::yacScanI64(sS64 *val) {
	if (!mpf_fits_slong_p(bigfloat)) return 0;
	*val=mpf_get_si(bigfloat);
	return 1;
}

void YAC_VCALL BigFloat::yacValueOfI(sSI val) {
	mpf_set_si(bigfloat,val);
}

void YAC_VCALL BigFloat::yacValueOfF32(sF32 val) {
	mpf_set_d(bigfloat,val);
}

void YAC_VCALL BigFloat::yacValueOfF64(sF64 val) {
	mpf_set_d(bigfloat,val);
}

void YAC_VCALL BigFloat::yacValueOfI64(sS64 val) {
	mpf_set_d(bigfloat,val);
}

void YAC_VCALL BigFloat::yacSerialize(YAC_Object *_ofs,sUI _rtti) {
	YAC_BEG_SERIALIZE();
	_ofs->yacStreamWriteI32(bigfloat->_mp_prec);
	_ofs->yacStreamWriteI32(bigfloat->_mp_size);
	_ofs->yacStreamWriteI8(sizeof(mp_exp_t));
	_ofs->yacStreamWriteI8(sizeof(mp_limb_t));
	int len=bigfloat->_mp_size,i;
	if (len<0) len=-len;
	switch (sizeof(mp_exp_t)) {
		case 1:_ofs->yacStreamWriteI8(bigfloat->_mp_exp);
		       break;
		case 2:_ofs->yacStreamWriteI16(bigfloat->_mp_exp);
		       break;
		case 4:_ofs->yacStreamWriteI32(bigfloat->_mp_exp);
		       break;
		case 8:_ofs->yacStreamWriteI64(bigfloat->_mp_exp);
		       break;
	}
	switch (sizeof(mp_limb_t)) {
		case 1:
		       for (i=0;i<len;i++) _ofs->yacStreamWriteI8(bigfloat->_mp_d[i]);
		       break;
		case 2:
		       for (i=0;i<len;i++) _ofs->yacStreamWriteI16(bigfloat->_mp_d[i]);
		       break;
		case 4:
		       for (i=0;i<len;i++) _ofs->yacStreamWriteI32(bigfloat->_mp_d[i]);
		       break;
		case 8:
		       for (i=0;i<len;i++) _ofs->yacStreamWriteI64(bigfloat->_mp_d[i]);
		       break;
	}

}

sUI YAC_VCALL BigFloat::yacDeserialize(YAC_Object *_ifs,sUI _rtti) {
	YAC_BEG_DESERIALIZE();
	int strprec=_ifs->yacStreamReadI32();
	int size=_ifs->yacStreamReadI32();
	int len=size,i,expsize=_ifs->yacStreamReadI8(),limbsize=_ifs->yacStreamReadI8();
	if (len<0) len=-len;
	mpf_set_prec(bigfloat,strprec);
	bigfloat->_mp_size=size;
	if (expsize!=sizeof(mp_exp_t))
		yac_host->printf("[tkgmp] Warning! Exp Size mismatch in BigFloat::Deserialize detected!\n");
	switch (sizeof(expsize)) {
		case 1:bigfloat->_mp_exp=_ifs->yacStreamReadI8();
		       break;
		case 2:bigfloat->_mp_exp=_ifs->yacStreamReadI16();
		       break;
		case 4:bigfloat->_mp_exp=_ifs->yacStreamReadI32();
		       break;
		case 8:bigfloat->_mp_exp=_ifs->yacStreamReadI64();
		       break;
	}
	if (limbsize!=sizeof(mp_limb_t))
		yac_host->printf("[tkgmp] Warning! Limb Size mismatch in BigFloat::Deserialize detected!\n");
	switch (sizeof(expsize)) {
		case 1:
		       for (i=0;i<len;i++) bigfloat->_mp_d[i]=_ifs->yacStreamReadI8();
		       break;
		case 2:
		       for (i=0;i<len;i++) bigfloat->_mp_d[i]=_ifs->yacStreamReadI16();
		       break;
		case 4:
		       for (i=0;i<len;i++) bigfloat->_mp_d[i]=_ifs->yacStreamReadI32();
		       break;
		case 8:
		       for (i=0;i<len;i++) bigfloat->_mp_d[i]=_ifs->yacStreamReadI64();
		       break;
	}
	return 1;
}

void YAC_VCALL BigFloat::yacOperatorI(sSI _cmd,sSI _iv, YAC_Value *_r) {
	switch (_cmd) {
		case YAC_OP_ASSIGN:
			mpf_set_si(bigfloat,_iv);
			break;
		case YAC_OP_ADD:
			if (_iv>=0) mpf_add_ui(bigfloat,bigfloat,_iv);
			else mpf_sub_ui(bigfloat,bigfloat,(-_iv));
			break;
		case YAC_OP_SUB:
			if (_iv>=0) mpf_sub_ui(bigfloat,bigfloat,_iv);
			else mpf_add_ui(bigfloat,bigfloat,(-_iv));
			break;
		case YAC_OP_MUL:
			if (_iv>=0) mpf_mul_ui(bigfloat,bigfloat,_iv);
			else {
				mpf_mul_ui(bigfloat,bigfloat,(-_iv));
				mpf_neg(bigfloat,bigfloat);
			}
			break;
		case YAC_OP_DIV:
			if (_iv>=0) mpf_div_ui(bigfloat,bigfloat,_iv);
			else {
				mpf_div_ui(bigfloat,bigfloat,(-_iv));
				mpf_neg(bigfloat,bigfloat);
			}
			break;
		case YAC_OP_SHL:
			if (_iv>=0) mpf_mul_2exp(bigfloat,bigfloat,_iv);
			else mpf_div_2exp(bigfloat,bigfloat,(-_iv));
			break;
		case YAC_OP_SHR:
			if (_iv>=0) mpf_div_2exp(bigfloat,bigfloat,_iv);
			else mpf_mul_2exp(bigfloat,bigfloat,(-_iv));
			break;
		case YAC_OP_CEQ:
			YAC_RETI(mpf_cmp_si(bigfloat,_iv)==0);
			break;
		case YAC_OP_CNE:
			YAC_RETI(mpf_cmp_si(bigfloat,_iv)!=0);
			break;
		case YAC_OP_CLE:
			YAC_RETI(mpf_cmp_si(bigfloat,_iv)<=0);
			break;
		case YAC_OP_CLT:
			YAC_RETI(mpf_cmp_si(bigfloat,_iv)<0);
			break;
		case YAC_OP_CGE:
			YAC_RETI(mpf_cmp_si(bigfloat,_iv)>=0);
			break;
		case YAC_OP_CGT:
			YAC_RETI(mpf_cmp_si(bigfloat,_iv)>0);
			break;
		case YAC_OP_NEG:
			mpf_neg(bigfloat,bigfloat);
			break;
	}
}

void YAC_VCALL BigFloat::yacOperatorF(sSI _cmd, sF32 _fv, YAC_Value *_r) {
	switch (_cmd) {
		case YAC_OP_ASSIGN:
			mpf_set_d(bigfloat,_fv);
			break;
		case YAC_OP_ADD:
			mpf_set_d(tmp,_fv);
			mpf_add(bigfloat,bigfloat,tmp);
			break;
		case YAC_OP_SUB:
			mpf_set_d(tmp,_fv);
			mpf_sub(bigfloat,bigfloat,tmp);
			break;
		case YAC_OP_MUL:
			mpf_set_d(tmp,_fv);
			mpf_mul(bigfloat,bigfloat,tmp);
			break;
		case YAC_OP_DIV:
			mpf_set_d(tmp,_fv);
			mpf_div(bigfloat,bigfloat,tmp);
			break;
		case YAC_OP_CEQ:
			YAC_RETI(mpf_cmp_d(bigfloat,_fv)==0);
			break;
		case YAC_OP_CNE:
			YAC_RETI(mpf_cmp_d(bigfloat,_fv)!=0);
			break;
		case YAC_OP_CLE:
			YAC_RETI(mpf_cmp_d(bigfloat,_fv)<=0);
			break;
		case YAC_OP_CLT:
			YAC_RETI(mpf_cmp_d(bigfloat,_fv)<0);
			break;
		case YAC_OP_CGE:
			YAC_RETI(mpf_cmp_d(bigfloat,_fv)>=0);
			break;
		case YAC_OP_CGT:
			YAC_RETI(mpf_cmp_d(bigfloat,_fv)>0);
			break;
		case YAC_OP_NEG:
			mpf_neg(bigfloat,bigfloat);
			break;
	}
}

void YAC_VCALL BigFloat::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
	sF32 floatVal;
	if (YAC_VALID(_o)) {
		if (YAC_BCHK(_o,clid_BigFloat)) {
			BigFloat *bf=(BigFloat*)_o;
			switch (_cmd) {
				case YAC_OP_ASSIGN:
					mpf_set(bigfloat,bf->bigfloat);
					break;
				case YAC_OP_ADD:
					mpf_add(bigfloat,bigfloat,bf->bigfloat);
					break;
				case YAC_OP_SUB:
					mpf_sub(bigfloat,bigfloat,bf->bigfloat);
					break;
				case YAC_OP_MUL:
					mpf_mul(bigfloat,bigfloat,bf->bigfloat);
					break;
				case YAC_OP_DIV:
					mpf_div(bigfloat,bigfloat,bf->bigfloat);
					break;
				case YAC_OP_CEQ:
					YAC_RETI(mpf_cmp(bigfloat,bf->bigfloat)==0);
					break;
				case YAC_OP_CNE:
					YAC_RETI(mpf_cmp(bigfloat,bf->bigfloat)!=0);
					break;
				case YAC_OP_CLE:
					YAC_RETI(mpf_cmp(bigfloat,bf->bigfloat)<=0);
					break;
				case YAC_OP_CLT:
					YAC_RETI(mpf_cmp(bigfloat,bf->bigfloat)<0);
					break;
				case YAC_OP_CGE:
					YAC_RETI(mpf_cmp(bigfloat,bf->bigfloat)>=0);
					break;
				case YAC_OP_CGT:
					YAC_RETI(mpf_cmp(bigfloat,bf->bigfloat)>0);
					break;
				case YAC_OP_NEG:
					mpf_neg(bigfloat,bigfloat);
					break;
			}
		} else if (YAC_BCHK(_o,clid_BigInt)) {
			BigInt *bi=(BigInt*)_o;
			switch (_cmd) {
				case YAC_OP_ASSIGN:
					mpf_set_z(bigfloat,bi->bigint);
					break;
				case YAC_OP_ADD:
					mpf_set_z(tmp,bi->bigint);
					mpf_add(bigfloat,bigfloat,tmp);
					break;
				case YAC_OP_SUB:
					mpf_set_z(tmp,bi->bigint);
					mpf_sub(bigfloat,bigfloat,tmp);
					break;
				case YAC_OP_MUL:
					mpf_set_z(tmp,bi->bigint);
					mpf_mul(bigfloat,bigfloat,tmp);
					break;
				case YAC_OP_DIV:
					mpf_set_z(tmp,bi->bigint);
					mpf_div(bigfloat,bigfloat,tmp);
					break;
				case YAC_OP_CEQ:
					mpf_set_z(tmp,bi->bigint);
					YAC_RETI(mpf_cmp(bigfloat,tmp)==0);
					break;
				case YAC_OP_CNE:
					mpf_set_z(tmp,bi->bigint);
					YAC_RETI(mpf_cmp(bigfloat,tmp)!=0);
					break;
				case YAC_OP_CLE:
					mpf_set_z(tmp,bi->bigint);
					YAC_RETI(mpf_cmp(bigfloat,tmp)<=0);
					break;
				case YAC_OP_CLT:
					mpf_set_z(tmp,bi->bigint);
					YAC_RETI(mpf_cmp(bigfloat,tmp)<0);
					break;
				case YAC_OP_CGE:
					mpf_set_z(tmp,bi->bigint);
					YAC_RETI(mpf_cmp(bigfloat,tmp)>=0);
					break;
				case YAC_OP_CGT:
					mpf_set_z(tmp,bi->bigint);
					YAC_RETI(mpf_cmp(bigfloat,tmp)>0);
					break;
				case YAC_OP_NEG:
					mpf_neg(bigfloat,bigfloat);
					break;
			}
		} else if (YAC_BCHK(_o,clid_BigRat)) {
			BigRat *br=(BigRat*)_o;
			switch (_cmd) {
				case YAC_OP_ASSIGN:
					mpf_set_q(bigfloat,br->bigrat);
					break;
				case YAC_OP_ADD:
					mpf_set_q(tmp,br->bigrat);
					mpf_add(bigfloat,bigfloat,tmp);
					break;
				case YAC_OP_SUB:
					mpf_set_q(tmp,br->bigrat);
					mpf_sub(bigfloat,bigfloat,tmp);
					break;
				case YAC_OP_MUL:
					mpf_set_q(tmp,br->bigrat);
					mpf_mul(bigfloat,bigfloat,tmp);
					break;
				case YAC_OP_DIV:
					mpf_set_q(tmp,br->bigrat);
					mpf_div(bigfloat,bigfloat,tmp);
					break;
				case YAC_OP_CEQ:
					mpf_set_q(tmp,br->bigrat);
					YAC_RETI(mpf_cmp(bigfloat,tmp)==0);
					break;
				case YAC_OP_CNE:
					mpf_set_q(tmp,br->bigrat);
					YAC_RETI(mpf_cmp(bigfloat,tmp)!=0);
					break;
				case YAC_OP_CLE:
					mpf_set_q(tmp,br->bigrat);
					YAC_RETI(mpf_cmp(bigfloat,tmp)<=0);
					break;
				case YAC_OP_CLT:
					mpf_set_q(tmp,br->bigrat);
					YAC_RETI(mpf_cmp(bigfloat,tmp)<0);
					break;
				case YAC_OP_CGE:
					mpf_set_q(tmp,br->bigrat);
					YAC_RETI(mpf_cmp(bigfloat,tmp)>=0);
					break;
				case YAC_OP_CGT:
					mpf_set_q(tmp,br->bigrat);
					YAC_RETI(mpf_cmp(bigfloat,tmp)>0);
					break;
				case YAC_OP_NEG:
					mpf_neg(bigfloat,bigfloat);
					break;
			}
		} else if (YAC_BCHK(_o,YAC_CLID_STRING)) {
			YAC_String *str=(YAC_String*)_o;
			switch (_cmd) {
				case YAC_OP_ASSIGN:
					setString(str);
					break;
			}
		} else if (_o->yacScanF32(&floatVal)) yacOperatorF(_cmd,floatVal,_r);
		else YAC_Object::yacOperator(_cmd,_o,_r);
	}
}

//base can be negative, which translates to exponent in base10 always
void BigFloat::setBase(sSI val) {
	if ((val>=2)||(val<=-2)) base=val;
	else base=2;
}

sSI BigFloat::getBase(void) {
	return base;
}

void BigFloat::setDigits(sSI val) {
	if (val>0) digits=val;
	else digits=1;
}

sSI BigFloat::getDigits(void) {
	return digits;
}

void BigFloat::setPrecision(sSI val) {
	if (val>0) {
		mpf_set_prec(bigfloat,val);
		precision=val;
	}
}

sSI BigFloat::getPrecision() {
	precision=mpf_get_prec(bigfloat);
	return precision;
}

sBool BigFloat::setString(YAC_String *val) {
	if (mpf_set_str(bigfloat,(char*)val->chars,base)==0) return 1;
	else return 0;
}

void BigFloat::getString(YAC_Value *_r) {
	YAC_String *res=YAC_New_String();
	yacToString(res);
	YAC_RETS(res,1);
}

