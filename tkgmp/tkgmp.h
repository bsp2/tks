#ifndef __TKGMP_H_
#define __TKGMP_H_

YG("tkgmp")

#include <gmp.h>

#define YAC_OP_PRIO_BIGINT YAC_OP_PRIO_DOUBLE+1
#define YAC_OP_PRIO_BIGRAT YAC_OP_PRIO_DOUBLE+2
#define YAC_OP_PRIO_BIGFLOAT YAC_OP_PRIO_DOUBLE+3

class BigInt;
class BigRat;
class BigFloat;

YC class BigInt : public YAC_Object {
private:
	friend class BigRat;
	friend class BigFloat;
//	friend void YAC_VCALL BigRat::yacOperator(sSI,YAC_Object*,YAC_Value*);

	mpz_t bigint;
	static sSI base;
public:
	BigInt(void);
	~BigInt(void);

	sBool YAC_VCALL yacToString(YAC_String *_s) const;

	sBool YAC_VCALL yacScanI(sSI *);
	sBool YAC_VCALL yacScanF32(sF32 *);
	sBool YAC_VCALL yacScanF64(sF64 *);
	sBool YAC_VCALL yacScanI64(sS64 *);

	void YAC_VCALL yacValueOfI(sSI);
	void YAC_VCALL yacValueOfF32(sF32);
	void YAC_VCALL yacValueOfF64(sF64);
	void YAC_VCALL yacValueOfI64(sS64);

	void YAC_VCALL yacSerialize(YAC_Object *, sUI);
	sUI YAC_VCALL yacDeserialize(YAC_Object *,sUI);
	void YAC_VCALL yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r);
	void YAC_VCALL yacOperatorF(sSI _cmd, sF32 _fv, YAC_Value *_r);
	void YAC_VCALL yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r);
	sUI YAC_VCALL yacOperatorPriority(void) {return YAC_OP_PRIO_BIGINT;}
	
	YM void setInt(sSI val);
	YM sSI getInt(void);
	YM void setBase(sSI val);
	YM sSI getBase(void);

	YM sBool setString(YAC_String *val);
	YM void getString(YAC_Value *_r);


	YM void swap(YAC_Object *_val);

	YAC(BigInt);
};

YC class BigRat : public YAC_Object {
private:
	friend class BigInt;
	friend class BigFloat;
	mpq_t bigrat,tmp;
	static sSI base;
public:
	BigRat(void);
	~BigRat(void);

	sBool YAC_CALL yacToString(YAC_String *_s) const;

	sBool YAC_VCALL yacScanI(sSI *);
	sBool YAC_VCALL yacScanF32(sF32 *);
	sBool YAC_VCALL yacScanF64(sF64 *);
	sBool YAC_VCALL yacScanI64(sS64 *);


	void YAC_VCALL yacValueOfI(sSI);
	void YAC_VCALL yacValueOfF32(sF32);
	void YAC_VCALL yacValueOfF64(sF64);
	void YAC_VCALL yacValueOfI64(sS64);

	void YAC_VCALL yacSerialize(YAC_Object *, sUI);
	sUI YAC_VCALL yacDeserialize(YAC_Object *,sUI);
	void YAC_VCALL yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r);
	void YAC_VCALL yacOperatorF(sSI _cmd, sF32 _fv, YAC_Value *_r);
	void YAC_VCALL yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r);
	sUI YAC_VCALL yacOperatorPriority(void) {return YAC_OP_PRIO_BIGRAT;}

	
	YM void setBase(sSI val);
	YM sSI getBase(void);

	YM sBool setString(YAC_String *val);
	YM void getString(YAC_Value *_r);

	YM sBool setNumerator(YAC_Object *val);
	YM void getNumerator(YAC_Value *_r);
 
	YM sBool setDenumerator(YAC_Object *val);
	YM void getDenumerator(YAC_Value *_r);

	YM void canonicalize(void);

	YAC(BigRat);
};

YC class BigFloat : public YAC_Object {
private:
	friend class BigInt;
	friend class BigRat;
	mpf_t bigfloat,tmp;
	sSI digits,precision;
	static sSI base;
public:
	BigFloat(void);
	~BigFloat(void);

	sBool YAC_CALL yacToString(YAC_String *_s) const;

	sBool YAC_VCALL yacScanI(sSI *);
	sBool YAC_VCALL yacScanF32(sF32 *);
	sBool YAC_VCALL yacScanF64(sF64 *);
	sBool YAC_VCALL yacScanI64(sS64 *);
	
	void YAC_VCALL yacValueOfI(sSI);
	void YAC_VCALL yacValueOfF32(sF32);
	void YAC_VCALL yacValueOfF64(sF64);
	void YAC_VCALL yacValueOfI64(sS64);

	void YAC_VCALL yacSerialize(YAC_Object *, sUI);
	sUI YAC_VCALL yacDeserialize(YAC_Object *,sUI);
	void YAC_VCALL yacOperatorI(sSI _cmd, sSI _iv, YAC_Value *_r);
	void YAC_VCALL yacOperatorF(sSI _cmd, sF32 _fv, YAC_Value *_r);
	void YAC_VCALL yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r);
	sUI YAC_VCALL yacOperatorPriority(void) {return YAC_OP_PRIO_BIGFLOAT;}

	YM void setBase(sSI val);
	YM sSI getBase(void);

	YM void setDigits(sSI val);
	YM sSI getDigits(void);

	YM void setPrecision(sSI val);
	YM sSI getPrecision(void);

	YM sBool setString(YAC_String *val);
	YM void getString(YAC_Value *_r);

	YAC(BigFloat);
};




#endif
