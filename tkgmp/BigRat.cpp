sSI BigRat::base;

BigRat::BigRat(void) {
	mpq_init(bigrat);
	mpq_init(tmp);
	//mpz_set_ui(mpq_denref(bigrat),1);
	//mpz_set_ui(mpq_denref(tmp),1);
	base=10;
}

BigRat::~BigRat(void) {
	mpq_clear(tmp);
	mpq_clear(bigrat);
}

sBool BigRat::yacToString(YAC_String *_s) const {
#ifdef YAC_BIGSTRING
	sSI size;
	((BigRat*)this)->canonicalize();  // xxx discard const qualifier (todo)
	size=mpz_sizeinbase(mpq_numref(bigrat),base);
	size+=mpz_sizeinbase(mpq_denref(bigrat),base);
	size+=3;
//	_s->alloc(mpz_sizeinbase(mpq_numref(bigrat),base)+
//			mpz_sizeinbase(mpq_denref(bigrat),base)+3);
	_s->alloc(size);
	mpq_get_str((char*)_s->chars,base,bigrat);
	_s->fixLength();
	return 1;
#endif
	return 0;
}

sBool YAC_VCALL BigRat::yacScanI(sSI *val) {
	double me=mpq_get_d(bigrat);
	*val=(sSI)me;
	return 1;
}

sBool YAC_VCALL BigRat::yacScanF32(sF32 *val) {
	double me=mpq_get_d(bigrat);
	*val=(sF32)me;
	return 1;
}

sBool YAC_VCALL BigRat::yacScanF64(sF64 *val) {
	double me=mpq_get_d(bigrat);
	*val=me;
	return 1;
}

sBool YAC_VCALL BigRat::yacScanI64(sS64 *val) {
	double me=mpq_get_d(bigrat);
	*val=(sS64)me;
	return 1;
}

void YAC_VCALL BigRat::yacValueOfI(sSI val) {
	mpq_set_d(bigrat,(double)val);
}

void YAC_VCALL BigRat::yacValueOfF32(sF32 val) {
	mpq_set_d(bigrat,(double)val);
}

void YAC_VCALL BigRat::yacValueOfF64(sF64 val) {
	mpq_set_d(bigrat,(double)val);
}

void YAC_VCALL BigRat::yacValueOfI64(sS64 val) {
	mpq_set_d(bigrat,(double)val);
}

void YAC_VCALL BigRat::yacSerialize(YAC_Object *_ofs,sUI _rtti) {
// 	size_t nom_count, denom_count;
// //	mpz_t nom, denom;
// 	sSI sign;
// //	nom=mpq_numref(bigrat);
// //	denom=mpq_denref(bigrat);
// 	sign=mpz_sgn(mpq_numref(bigrat));
// 	if (sign<0) mpz_neg(mpq_numref(bigrat),mpq_numref(bigrat));
// 	YAC_BEG_SERIALIZE();
// 	sU8 *data=(sU8*)mpz_export(NULL,&nom_count,1,1,1,0,mpq_numref(bigrat));
// 	_ofs->yacStreamWriteI32(nom_count);
// 	_ofs->yacStreamWriteI8((sU8)sign);
// 	_ofs->yacStreamWrite(data,nom_count);
// 	delete data;
// 	data=(sU8*)mpz_export(NULL,&denom_count,1,1,1,0,mpq_denref(bigrat));
// 	_ofs->yacStreamWriteI32(denom_count);
// 	_ofs->yacStreamWrite(data,denom_count);
// 	delete data;
// 	if (sign<0) mpz_neg(mpq_numref(bigrat),mpq_numref(bigrat));
}

sUI YAC_VCALL BigRat::yacDeserialize(YAC_Object *_ifs,sUI _rtti) {
// 	size_t nom_count, denom_count;
// 	sSI sign;
// 	//mpz_t nom,denom;
// 	//nom=mpq_numref(bigrat);
// 	//denom=mpq_denref(bigrat);
// 	YAC_BEG_DESERIALIZE();
// 	nom_count=_ifs->yacStreamReadI32();
// 	sign=(sS8)_ifs->yacStreamReadI8();
// 	sU8 *data=new sU8[nom_count];
// 	_ifs->yacStreamRead(data,nom_count);
// 	mpz_import(mpq_numref(bigrat),nom_count,1,1,1,0,(void*)data);
// 	if (sign<0) mpz_neg(mpq_numref(bigrat),mpq_numref(bigrat));
// 	//delete data;
// 	denom_count=_ifs->yacStreamReadI32();
// 	if (denom_count>nom_count) {
// 		delete data;
// 		data=new sU8[denom_count];
// 	}
// 	_ifs->yacStreamRead(data,denom_count);
// 	mpz_import(mpq_denref(bigrat),denom_count,1,1,1,0,(void*)data);
// 	delete data;
// 	return 1;
   return 0;
}

void YAC_VCALL BigRat::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
	//yac_host->printf("BigRat yacOpI entered _iv=%d\n",_iv);
	switch (_cmd) {
		case YAC_OP_ASSIGN:
			mpq_set_si(bigrat,_iv,1);
			break;
		case YAC_OP_ADD:
			mpq_set_si(tmp,_iv,1);
			mpq_add(bigrat,bigrat,tmp);
			break;
		case YAC_OP_SUB:
			mpq_set_si(tmp,_iv,1);
			mpq_sub(bigrat,bigrat,tmp);
			break;
		case YAC_OP_MUL:
			mpq_set_si(tmp,_iv,1);
			mpq_mul(bigrat,bigrat,tmp);
			break;
		case YAC_OP_DIV:
			mpq_set_si(tmp,_iv,1);
			mpq_div(bigrat,bigrat,tmp);
			break;
		case YAC_OP_SHL:
			mpq_mul_2exp(bigrat,bigrat,_iv);
			break;
		case YAC_OP_SHR:
			mpq_div_2exp(bigrat,bigrat,_iv);
			break;
		case YAC_OP_CEQ:YAC_RETI(mpq_cmp_si(bigrat,_iv,1)==0);
				break;
		case YAC_OP_CNE:YAC_RETI(mpq_cmp_si(bigrat,_iv,1)!=0);
				break;
		case YAC_OP_CLE:YAC_RETI(mpq_cmp_si(bigrat,_iv,1)<=0);
				break;
		case YAC_OP_CLT:YAC_RETI(mpq_cmp_si(bigrat,_iv,1)<0);
				break;
		case YAC_OP_CGE:YAC_RETI(mpq_cmp_si(bigrat,_iv,1)>=0);
				break;
		case YAC_OP_CGT:YAC_RETI(mpq_cmp_si(bigrat,_iv,1)>0);
				break;
		case YAC_OP_NEG:mpq_neg(bigrat,bigrat);
				break;
	}
}

void YAC_VCALL BigRat::yacOperatorF(sSI _cmd, sF32 _fv, YAC_Value *_r) {
	switch (_cmd) {
		case YAC_OP_ASSIGN:mpq_set_d(bigrat,_fv);
				   break;
		case YAC_OP_ADD:mpq_set_d(tmp,_fv);
				mpq_add(bigrat,bigrat,tmp);
				break;
		case YAC_OP_SUB:mpq_set_d(tmp,_fv);
				mpq_sub(bigrat,bigrat,tmp);
				break;
		case YAC_OP_MUL:mpq_set_d(tmp,_fv);
				mpq_mul(bigrat,bigrat,tmp);
				break;
		case YAC_OP_DIV:mpq_set_d(tmp,_fv);
				mpq_div(bigrat,bigrat,tmp);
				break;
		case YAC_OP_CEQ:mpq_set_d(tmp,_fv);
				YAC_RETI(mpq_equal(bigrat,tmp));
				break;
		case YAC_OP_CNE:mpq_set_d(tmp,_fv);
				YAC_RETI(!mpq_equal(bigrat,tmp));
				break;
		case YAC_OP_CLE:mpq_set_d(tmp,_fv);
				YAC_RETI(mpq_cmp(bigrat,tmp)<=0);
				break;
		case YAC_OP_CLT:mpq_set_d(tmp,_fv);
				YAC_RETI(mpq_cmp(bigrat,tmp)<0);
				break;
		case YAC_OP_CGE:mpq_set_d(tmp,_fv);
				YAC_RETI(mpq_cmp(bigrat,tmp)>=0);
				break;
		case YAC_OP_CGT:mpq_set_d(tmp,_fv);
				YAC_RETI(mpq_cmp(bigrat,tmp)>0);
				break;
		case YAC_OP_NEG:mpq_neg(bigrat,bigrat);
				break;
	}
}

void YAC_VCALL BigRat::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
	sF32 floatVal;
	if (YAC_VALID(_o)) {
		if (YAC_BCHK(_o,clid_BigRat)) {
			BigRat *o=(BigRat*)_o;
			switch (_cmd) {
				case YAC_OP_ASSIGN:mpq_set(bigrat,o->bigrat);
						   break;
				case YAC_OP_ADD:mpq_add(bigrat,bigrat,o->bigrat);
						break;
				case YAC_OP_SUB:mpq_sub(bigrat,bigrat,o->bigrat);
						break;
				case YAC_OP_MUL:mpq_mul(bigrat,bigrat,o->bigrat);
					       break;
				case YAC_OP_DIV:mpq_div(bigrat,bigrat,o->bigrat);
						break;
				case YAC_OP_CEQ:YAC_RETI(mpq_equal(bigrat,o->bigrat));
						break;
				case YAC_OP_CNE:YAC_RETI(!mpq_equal(bigrat,o->bigrat));
						break;
				case YAC_OP_CLE:YAC_RETI(mpq_cmp(bigrat,o->bigrat)<=0);
						break;
				case YAC_OP_CLT:YAC_RETI(mpq_cmp(bigrat,o->bigrat)<0);
						break;
				case YAC_OP_CGE:YAC_RETI(mpq_cmp(bigrat,o->bigrat)>=0);
						break;
				case YAC_OP_CGT:YAC_RETI(mpq_cmp(bigrat,o->bigrat)>0);
						break;
				case YAC_OP_NEG:mpq_neg(bigrat,bigrat);
						break;
				default:YAC_Object::yacOperator(_cmd,_o,_r);
			}
		} else if (YAC_BCHK(_o,clid_BigInt)) {
			BigInt *o=(BigInt*)_o;
			switch (_cmd) {
				case YAC_OP_ASSIGN:mpq_set_z(bigrat,o->bigint);
						   break;
				case YAC_OP_ADD:mpq_set_z(tmp,o->bigint);
						mpq_add(bigrat,bigrat,tmp);
						break;
				case YAC_OP_SUB:mpq_set_z(tmp,o->bigint);
						mpq_sub(bigrat,bigrat,tmp);
						break;
				case YAC_OP_MUL:mpq_set_z(tmp,o->bigint);
						mpq_mul(bigrat,bigrat,tmp);
						break;
				case YAC_OP_DIV:mpq_set_z(tmp,o->bigint);
						mpq_div(bigrat,bigrat,tmp);
						break;
				case YAC_OP_CEQ:mpq_set_z(tmp,o->bigint);
						YAC_RETI(mpq_equal(bigrat,tmp));
						break;
				case YAC_OP_CNE:mpq_set_z(tmp,o->bigint);
						YAC_RETI(!mpq_equal(bigrat,tmp));
						break;
				case YAC_OP_CLE:mpq_set_z(tmp,o->bigint);
						YAC_RETI(mpq_cmp(bigrat,tmp)<=0);
						break;
				case YAC_OP_CLT:mpq_set_z(tmp,o->bigint);
						YAC_RETI(mpq_cmp(bigrat,tmp)<0);
						break;
				case YAC_OP_CGE:mpq_set_z(tmp,o->bigint);
						YAC_RETI(mpq_cmp(bigrat,tmp)>=0);
						break;
				case YAC_OP_CGT:mpq_set_z(tmp,o->bigint);
						YAC_RETI(mpq_cmp(bigrat,tmp)>0);
						break;
				case YAC_OP_NEG:mpq_neg(bigrat,bigrat);
						break;
				default:YAC_Object::yacOperator(_cmd,_o,_r);
			}
		} else if (YAC_BCHK(_o,YAC_CLID_STRING)) {
			YAC_String *o=(YAC_String*)_o;
			switch (_cmd) {
				case YAC_OP_ASSIGN:setString(o);
						   break;
			}
		} else if (_o->yacScanF32(&floatVal)) {
			yacOperatorF(_cmd,floatVal,_r);
		} else YAC_Object::yacOperator(_cmd,_o,_r);
	}
}

void BigRat::setBase(sSI val) {
	if (val>2) base=val;
	else base=2;
}

sSI BigRat::getBase(void) {
	return base;
}

sBool BigRat::setString(YAC_String *val) {
	if (mpq_set_str(bigrat,(char*)val->chars,base)==0) return 1;
	else return 0;
}

void BigRat::getString(YAC_Value *_r) {
	YAC_String *res=YAC_New_String();
	yacToString(res);
	YAC_RETS(res,1);
}

sBool BigRat::setNumerator(YAC_Object *val) {
	sF64 floatVal;
	if (YAC_VALID(val)) {
		if (YAC_BCHK(val,clid_BigInt)) {
			BigInt *bi=(BigInt*)val;
			mpz_set(mpq_numref(bigrat),bi->bigint);
		} else if (YAC_BCHK(val,clid_BigRat)) {
			BigRat *br=(BigRat*)val;
			mpz_set_q(mpq_numref(bigrat),br->bigrat);
		} else if (YAC_BCHK(val,clid_BigFloat)) {
			BigFloat *bf=(BigFloat*)val;
			mpz_set_f(mpq_numref(bigrat),bf->bigfloat);
		} else if (YAC_BCHK(val,YAC_CLID_STRING)) {
			YAC_String *str=(YAC_String*)val;
			mpz_set_str(mpq_numref(bigrat),(const char*)str->chars,base);
		} else if (val->yacScanF64(&floatVal)) {
			mpz_set_d(mpq_numref(bigrat),floatVal);
		} else return 0;
		return 1;
	}
	return 0;
}

void BigRat::getNumerator(YAC_Value *_r) {
	BigInt *res=(BigInt*)yac_host->yacNewByID(clid_BigInt);
	mpz_set(res->bigint,mpq_numref(bigrat));
	YAC_RETO(res,1);
}

sBool BigRat::setDenumerator(YAC_Object *val) {
	sF64 floatVal;
	if (YAC_VALID(val)) {
		if (YAC_BCHK(val,clid_BigInt)) {
			BigInt *bi=(BigInt*)val;
			mpz_set(mpq_denref(bigrat),bi->bigint);
		} else if (YAC_BCHK(val,clid_BigRat)) {
			BigRat *br=(BigRat*)val;
			mpz_set_q(mpq_denref(bigrat),br->bigrat);
		} else if (YAC_BCHK(val,clid_BigFloat)) {
			BigFloat *bf=(BigFloat*)val;
			mpz_set_f(mpq_denref(bigrat),bf->bigfloat);
		} else if (YAC_BCHK(val,YAC_CLID_STRING)) {
			YAC_String *str=(YAC_String*)val;
			mpz_set_str(mpq_denref(bigrat),(const char*)str->chars,base);
		} if (val->yacScanF64(&floatVal)) {
			mpz_set_d(mpq_denref(bigrat),floatVal);
		} else return 0;
		return 1;
	}
	return 0;
}

void BigRat::getDenumerator(YAC_Value *_r) {
	BigInt *res=(BigInt*)yac_host->yacNewByID(clid_BigInt);
	mpz_set(res->bigint,mpq_denref(bigrat));
	YAC_RETO(res,1);
}

void BigRat::canonicalize() {
//	yac_host->printf("BigRat canonicalize() called.\n");
	mpq_canonicalize(bigrat);
}
