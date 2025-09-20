#ifndef ___MATH_HPP__
#define ___MATH_HPP__



/****************************************************************************/
/***                                                                      ***/
/***   Copyright (C) 2001 by Dierk Ohlerich                               ***/
/***                                                                      ***/
/***   Do what you want but don't remove this notice                      ***/
/***                                                                      ***/
/***   chaos@vcc.de                                                       ***/
/***                                                                      ***/
/****************************************************************************/

class sFVector;
class sFMatrix;
class sFVector2d;

/****************************************************************************/
/****************************************************************************/


/************************************************************************/

/* floating-point functions */

sF32 sFSin(sF32);
sF32 sFCos(sF32);
sF32 sFExp(sF32);
sF32 sFPower(sF32,sF32);
sF32 sFAsin(sF32);
sF32 sFAcos(sF32);
sF32 sFAtan(sF32);
sF32 sFAtan2(sF32 a,sF32 b);
sF32 sFAbs(sF32 a);
sF32 sFSqrt(sF32 a);
sF32 sFInvSqrt(sF32 a);
//inline sF32 sFSign(sF32 a) {return ((a)>=0) ? 1 : -1;}

/************************************************************************/
/************************************************************************/

class sFVector : public YAC_Object
{
public:
  sF32 x,y,z;

	void copyFrom(sFVector*);
	void init(void);
	void init(sF32 _x, sF32 _y, sF32 _z);
	void init(const sFVector &v) {x=v.x;y=v.y;z=v.z;}

	//
  //void Init(const sIVector &v);

	void neg()
	{x=-x; y=-y; z=-z;}
  void Add(const sFVector &v) {x+=v.x;y+=v.y;z+=v.z;}
  void Sub(const sFVector &v) {x-=v.x;y-=v.y;z-=v.z;}
  sF32 Dot(const sFVector &v) {return x*v.x+y*v.y+z*v.z;}
  void scalef(sF32 s) {x=(sF32)(x*s);y=(sF32)(y*s);z=(sF32)(z*s);}
  void AddScale(const sFVector &v,sF32 s) {x+=(sF32)(v.x*s);y+=(sF32)(v.y*s);z+=(sF32)(v.z*s);}

  sF32 Abs();
  sF32 AbsMax();
  sF32 AbsAdd();
  void unit();
  //void Unit(const sFVector &);
  void Cross(const sFVector &a,const sFVector &b) {x=(a.y*b.z-a.z*b.y);y=(a.z*b.x-a.x*b.z);z=(a.x*b.y-a.y*b.x);}
  void Rotate(sFMatrix *);
  void RotateInv(sFMatrix *);

  void rotateXCW(void);
  void rotateXCCW(void);
  void rotateYCW(void);
  void rotateYCCW(void);
  void rotateZCW(void);
  void rotateZCCW(void);
};

/************************************************************************/

class sFMatrix : public YAC_Object
{
public:
  sFVector i,j,k;

  void init();
  void init(const sFVector &);
  void init(sF32 a,sF32 b,sF32 c);
  void initDeg(sF32 a,sF32 b,sF32 c);
  void loadIdentity(void);
  void init(sFMatrix *);

  void Mul(const sFMatrix &,const sFMatrix &);
  void MulInv(const sFMatrix &,const sFMatrix &);
  void invert();

  void Sub(const sFMatrix &);
  void Add(const sFMatrix &);
  void scale(sF32);
};

/****************************************************************************/
/****************************************************************************/

#endif
