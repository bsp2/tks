sSI BigInt::base;

BigInt::BigInt(void) {
	base=10;
	mpz_init(bigint);
}

BigInt::~BigInt(void) {
	mpz_clear(bigint);
}

sBool BigInt::yacToString(YAC_String *_s) const {
#ifdef YAC_BIGSTRING
	_s->alloc(mpz_sizeinbase(bigint,base)+2);
	mpz_get_str((char*)_s->chars,base,bigint);
	_s->fixLength();
	return 1;
#endif
	return 0;
}

sBool YAC_VCALL BigInt::yacScanI(sSI *val) {
	if (!mpz_fits_sint_p(bigint)) return 0;
	*val=(sSI)mpz_get_si(bigint);
	return 1;
}

sBool YAC_VCALL BigInt::yacScanF32(sF32 *val) {
	*val=(sF32)mpz_get_d(bigint);
	return 1;
}

sBool YAC_VCALL BigInt::yacScanF64(sF64 *val) {
	*val=mpz_get_d(bigint);
	return 1;
}

sBool YAC_VCALL BigInt::yacScanI64(sS64 *val) {
	if (!mpz_fits_slong_p(bigint)) return 0;//wont go over 32bit on 32bit systems
	*val=(sS64)mpz_get_si(bigint);
	return 1;
}

void YAC_VCALL BigInt::yacValueOfI(sSI val) {
	mpz_set_si(bigint,val);
}

void YAC_VCALL BigInt::yacValueOfF32(sF32 val) {
	mpz_set_d(bigint,val);
}

void YAC_VCALL BigInt::yacValueOfF64(sF64 val) {
	mpz_set_d(bigint,val);
}

void YAC_VCALL BigInt::yacValueOfI64(sS64 val) {
	mpz_set_si(bigint,val);
}

void YAC_VCALL BigInt::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
// 	size_t countp;
// 	sSI sign=mpz_sgn(bigint);
// 	if (sign<0) mpz_neg(bigint,bigint);
// 	yac_host->printf("sizeof(countp)=%d sign=%d\n",sizeof(countp),sign);
// 	YAC_BEG_SERIALIZE();
// 	sU8 *data=(sU8*)mpz_export(NULL,&countp,1,1,1,0,bigint);
// 	_ofs->yacStreamWriteI32(countp);
// 	_ofs->yacStreamWriteI8((sU8)sign);
// 	_ofs->yacStreamWrite(data,countp);
// 	delete data;
// 	if (sign<0) mpz_neg(bigint,bigint);
}

sUI YAC_VCALL BigInt::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
// 	size_t countp;
// 	sSI sign;
// 	YAC_BEG_DESERIALIZE();
// 	countp=_ifs->yacStreamReadI32();
// 	sign=(sS8)_ifs->yacStreamReadI8();
// 	yac_host->printf("Deserialize sign=%d\n",sign);
// 	sU8 *data=new sU8[countp];
// 	_ifs->yacStreamRead(data,countp);
// 	mpz_import(bigint,countp,1,1,1,0,(void*)data);
// 	if (sign<0) mpz_neg(bigint,bigint);
// 	delete data;
// 	return 1;
   return 0;
}

void YAC_VCALL BigInt::yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r) {
//	yac_host->printf("yacOperatorI entered _iv=%d\n",_iv);
	mpz_t tmp;
	switch (_cmd) {
		case YAC_OP_ASSIGN: mpz_set_si(bigint,_iv);
				    break;
		case YAC_OP_ADD: if (_iv>=0) mpz_add_ui(bigint,bigint,_iv);
				else mpz_sub_ui(bigint,bigint,(-_iv));
				 break;
		case YAC_OP_SUB: if (_iv>0) mpz_sub_ui(bigint,bigint,_iv);
				else mpz_add_ui(bigint,bigint,(-_iv));
				 break;
		case YAC_OP_MUL: mpz_mul_si(bigint,bigint,_iv);
				 break;
		case YAC_OP_DIV: if (_iv>=0) {
					 mpz_fdiv_q_ui(bigint,bigint,_iv);
				 } else {
					 mpz_fdiv_q_ui(bigint,bigint,(-_iv));
					 mpz_mul_si(bigint,bigint,-1);
				 }
				 break;
		case YAC_OP_MOD: mpz_fdiv_r_ui(bigint,bigint,_iv);
				 break;
		case YAC_OP_SHL:mpz_mul_2exp(bigint,bigint,_iv);
				break;
		case YAC_OP_SHR:mpz_fdiv_q_2exp(bigint,bigint,_iv);
				break;
		case YAC_OP_CEQ:YAC_RETI(mpz_cmp_si(bigint,_iv)==0);
				break;
		case YAC_OP_CNE:YAC_RETI(mpz_cmp_si(bigint,_iv)!=0);
				break;
		case YAC_OP_CLE:YAC_RETI(mpz_cmp_si(bigint,_iv)<=0);
				break;
		case YAC_OP_CLT:YAC_RETI(mpz_cmp_si(bigint,_iv)<0);
				break;
		case YAC_OP_CGE:YAC_RETI(mpz_cmp_si(bigint,_iv)>=0);
				break;
		case YAC_OP_CGT:YAC_RETI(mpz_cmp_si(bigint,_iv)>0);
				break;
		case YAC_OP_AND:
		case YAC_OP_LAND:
				mpz_init_set_si(tmp,_iv);
				mpz_and(bigint,bigint,tmp);
				mpz_clear(tmp);
				break;

		case YAC_OP_OR:
		case YAC_OP_LOR:
				mpz_init_set_si(tmp,_iv);
				mpz_ior(bigint,bigint,tmp);
				mpz_clear(tmp);
				break;
		case YAC_OP_EOR:
		case YAC_OP_LEOR:
				mpz_init_set_si(tmp,_iv);
				mpz_xor(bigint,bigint,tmp);
				mpz_clear(tmp);
				break;
		case YAC_OP_NEG:
				mpz_neg(bigint,bigint);
				break;
	}
}

void YAC_VCALL BigInt::yacOperatorF(sSI _cmd, sF32 _fv, YAC_Value *_r) {
//	yac_host->printf("yacOperatorF entered\n");
	mpz_t tmp;
	switch (_cmd) {
		case YAC_OP_ASSIGN: mpz_set_d(bigint,_fv);
				    break;
		case YAC_OP_ADD:
				if (_fv>=0.0) {
					mpz_init_set_d(tmp,_fv);
					mpz_add(bigint,bigint,tmp);
				} else {
					mpz_init_set_d(tmp,-_fv);
					mpz_sub(bigint,bigint,tmp);
				}
				mpz_clear(tmp);
				break;
		case YAC_OP_SUB: if (_fv>=0.0) {
					 mpz_init_set_d(tmp,_fv);
					 mpz_sub(bigint,bigint,tmp);
				 } else {
					 mpz_init_set_d(tmp,-_fv);
					 mpz_add(bigint,bigint,tmp);
				 }
				 break;
		case YAC_OP_MUL: mpz_init_set_d(tmp,_fv);
				 mpz_mul(bigint,bigint,tmp);
				 mpz_clear(tmp);
				 break;
		case YAC_OP_DIV: if (_fv>=0.0) {
					 mpz_init_set_d(tmp,_fv);
					 mpz_fdiv_q(bigint,bigint,tmp);
				 } else {
					 mpz_init_set_d(tmp,-_fv);
					 mpz_fdiv_q(bigint,bigint,tmp);
					 mpz_mul_si(bigint,bigint,-1);
				 }
				 mpz_clear(tmp);
				 break;
		case YAC_OP_MOD: mpz_init_set_d(tmp,_fv);
				 mpz_fdiv_r(bigint,bigint,tmp);
				 mpz_clear(tmp);
				 break;
		case YAC_OP_SHL:mpz_mul_2exp(bigint,bigint,(sUI)_fv);
				break;
		case YAC_OP_SHR:mpz_fdiv_q_2exp(bigint,bigint,(sUI)_fv);
				break;
		case YAC_OP_CEQ:YAC_RETI(mpz_cmp_d(bigint,_fv)==0);
				break;
		case YAC_OP_CNE:YAC_RETI(mpz_cmp_d(bigint,_fv)!=0);
				break;
		case YAC_OP_CLE:YAC_RETI(mpz_cmp_d(bigint,_fv)<=0);
				break;
		case YAC_OP_CLT:YAC_RETI(mpz_cmp_d(bigint,_fv)<0);
				break;
		case YAC_OP_CGE:YAC_RETI(mpz_cmp_d(bigint,_fv)>=0);
				break;
		case YAC_OP_CGT:YAC_RETI(mpz_cmp_d(bigint,_fv)>0);
				break;
		case YAC_OP_AND:
		case YAC_OP_LAND:
				mpz_init_set_d(tmp,_fv);
				mpz_and(bigint,bigint,tmp);
				mpz_clear(tmp);
				break;

		case YAC_OP_OR:
		case YAC_OP_LOR:
				mpz_init_set_d(tmp,_fv);
				mpz_ior(bigint,bigint,tmp);
				mpz_clear(tmp);
				break;
		case YAC_OP_EOR:
		case YAC_OP_LEOR:
				mpz_init_set_d(tmp,_fv);
				mpz_xor(bigint,bigint,tmp);
				mpz_clear(tmp);
				break;
		case YAC_OP_NEG:
				mpz_neg(bigint,bigint);
				break;
	}
}

void YAC_VCALL BigInt::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
//	yac_host->printf("yacOperator entered\n");
	if (YAC_VALID(_o)) {
		if (YAC_BCHK(_o,clid_BigInt)) {
			BigInt *o=(BigInt*)_o;
			switch (_cmd) {
				case YAC_OP_ASSIGN:
					mpz_set(bigint,o->bigint);
					break;
				case YAC_OP_ADD:
					mpz_add(bigint,bigint,o->bigint);
					break;
				case YAC_OP_SUB:
					mpz_sub(bigint,bigint,o->bigint);
					break;
				case YAC_OP_MUL:
					mpz_mul(bigint,bigint,o->bigint);
					break;
				case YAC_OP_DIV:
					mpz_fdiv_q(bigint,bigint,o->bigint);
					break;
				case YAC_OP_MOD:
					mpz_fdiv_r(bigint,bigint,o->bigint);
					break;
				case YAC_OP_CEQ:
					YAC_RETI(mpz_cmp(bigint,o->bigint)==0);
					break;
				case YAC_OP_CNE:
					YAC_RETI(mpz_cmp(bigint,o->bigint)!=0);
					break;
				case YAC_OP_CLE:
					YAC_RETI(mpz_cmp(bigint,o->bigint)<=0);
					break;
				case YAC_OP_CLT:
					YAC_RETI(mpz_cmp(bigint,o->bigint)<0);
					break;
				case YAC_OP_CGE:
					YAC_RETI(mpz_cmp(bigint,o->bigint)>=0);
					break;
				case YAC_OP_CGT:
					YAC_RETI(mpz_cmp(bigint,o->bigint)>0);
					break;
				case YAC_OP_AND:
				case YAC_OP_LAND:
					mpz_and(bigint,bigint,o->bigint);
					break;
				case YAC_OP_OR:
				case YAC_OP_LOR:
					mpz_ior(bigint,bigint,o->bigint);
					break;
				case YAC_OP_EOR:
				case YAC_OP_LEOR:
					mpz_xor(bigint,bigint,o->bigint);
					break;
				case YAC_OP_NEG:
					mpz_neg(bigint,bigint);
					break;
				default:
					YAC_Object::yacOperator(_cmd,_o,_r);
			}
		} else if (YAC_BCHK(_o,YAC_CLID_STRING)) {
			YAC_String *o=(YAC_String*)_o;
			switch (_cmd) {
				case YAC_OP_ASSIGN:
					setString(o);
					break;
			}
		} else YAC_Object::yacOperator(_cmd,_o,_r);
	}
}

void BigInt::setInt(sSI val) {
	mpz_set_si(bigint,val);
}

sSI BigInt::getInt(void) {
	//careful!!!!! may break in 64bit compilation attempts, untested
	return (sSI)mpz_get_si(bigint);
}

void BigInt::setBase(sSI val) {
	if (val>2) base=val;
	else base=2;
}

sSI BigInt::getBase(void) {
	return base;
}

sBool BigInt::setString(YAC_String *val) {
	if (mpz_set_str(bigint,(char*)val->chars,base)==0) return 1;
	else return 0;
}

void BigInt::getString(YAC_Value *_r) {
	YAC_String *res=YAC_New_String();
	yacToString(res);
	YAC_RETS(res,1);
}

void BigInt::swap(YAC_Object *_val) {
	if (YAC_CHK(_val,clid_BigInt)) {
		BigInt *val=(BigInt*)_val;
		mpz_swap(bigint,val->bigint);
	}
}
