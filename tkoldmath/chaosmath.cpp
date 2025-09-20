/****************************************************************************/
/***                                                                      ***/
/***   Copyright (C) 2001 by Dierk Ohlerich                               ***/
/***                                                                      ***/
/***   Do what you want but don't remove this notice                      ***/
/***                                                                      ***/
/***   chaos@vcc.de                                                       ***/
/***                                                                      ***/
/****************************************************************************/

// changelog:
//              14Feb03 <fli> fixed Vector::rotate, Matrix::init(a,b,c) for OpenGL compatibility
//                      <fli> unit now also cals the length of very small vectors
//                      <fli> clean up for source release, stripped off unneeded functions/methods 

#include <math.h>
#include <yac.h>

#include "chaosmath.h"


/****************************************************************************/
/****************************************************************************/
/***                                                                      ***/
/***   Float Math                                                         ***/
/***                                                                      ***/
/****************************************************************************/
/****************************************************************************/

sF32 sFSin(sF32 a)
{
  return (sF32)sin((double)a);
}

/****************************************************************************/

sF32 sFCos(sF32 a)
{
  return (sF32)cos((double)a);
}

/****************************************************************************/

sF32 sFExp(sF32 a)
{
  return (sF32)exp((double)a);
}

/****************************************************************************/

sF32 sFPower(sF32 a,sF32 x)
{
  return (sF32)exp((double)x*log(a));
}

/****************************************************************************/

sF32 sFAsin(sF32 a)
{
	return (sF32)asin((double)a);
}

/****************************************************************************/

sF32 sFAcos(sF32 a)
{
	return (sF32)acos((double)a);
}

/****************************************************************************/

sF32 sFAtan(sF32 a)
{
  return (sF32)atan((double)a);
}

/****************************************************************************/

sF32 sFAtan2(sF32 a,sF32 b)	
{ 
  return (sF32)atan2((double)a,(double)b); 
}

/****************************************************************************/

sF32 sFAbs(sF32 a)							
{ 
  return (sF32)fabs((double)a); 
}

/****************************************************************************/

sF32 sFSqrt(sF32 a)						
{ 
  return (sF32)sqrt((double)a); 
}

/****************************************************************************/

sF32 sFInvSqrt(sF32 a)					
{ 
  return (sF32)(1.0/sqrt((double)a)); 
}

/****************************************************************************/
/****************************************************************************/
/***                                                                      ***/
/***   Float Vector                                                       ***/
/***                                                                      ***/
/****************************************************************************/
/****************************************************************************/

void sFVector::copyFrom(sFVector *_o) {
	x=_o->x;
	y=_o->y;
	z=_o->z;
}

void sFVector::init(void) {
	x=y=z=0.0f;
}

void sFVector::init(sF32 _x, sF32 _y, sF32 _z) {
	x=_x; y=_y; z=_z;
}

sF32 sFVector::Abs()
{
  return (sF32)sFSqrt(x*x+y*y+z*z);
}

sF32 sFVector::AbsMax()
{
  sF32 xx,yy,zz;

  xx = sABS(x);
  yy = sABS(y);
  zz = sABS(z);
  if(xx>yy && xx>zz) return xx;
  if(yy>zz) return yy;
  return zz;
}

sF32 sFVector::AbsAdd()
{
  return (sF32)(sABS(x)+sABS(y)+sABS(z));
}

void sFVector::unit()
{
  sF64 e;
  
  e=x*x+y*y+z*z;

  if(e!=0) //(e>0.000000001f)
  {
    e=sFInvSqrt((sF32)e);
    x*=(sF32)e;
    y*=(sF32)e;
    z*=(sF32)e;
  }
  else
  {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
  }
}

/*void sFVector::Unit(const sFVector &v)
{
  sF32 e;

  e=v.x*v.x+v.y*v.y+v.z*v.z;
  if(e>0.000000001f)
  {
    e=(sF32)sFInvSqrt(e);
    x=(sF32)(v.x*e);
    y=(sF32)(v.y*e);
    z=(sF32)(v.z*e);
  }
  else
  {
    x=1.0f;
    y=0.0f;
    z=0.0f;
  }
}*/

void sFVector::Rotate(sFMatrix *m)
{
  sF32 tx=x;
  sF32 ty=y;
  x= tx*m->i.x + ty*m->i.y + z*m->i.z;
  y= tx*m->j.x + ty*m->j.y + z*m->j.z;
  z= tx*m->k.x + ty*m->k.y + z*m->k.z;
}

void sFVector::RotateInv(sFMatrix *m)
{
  sF32 xx,yy;
  xx=x;
  yy=y;
  x = xx*m->i.x + yy*m->j.x + z*m->k.x;
  y = xx*m->i.y + yy*m->j.y + z*m->k.y;
  z = xx*m->i.z + yy*m->j.z + z*m->k.z;
}


void sFVector::rotateXCW(void) {
	sF32 yy=y;
	y=-z;
	z=yy;
}

void sFVector::rotateXCCW(void) {
	sF32 yy=y;
	y=z;
	z=-yy;
}

void sFVector::rotateYCW(void) {
	sF32 xx=x;
	x=z;
	z=-xx;
}

void sFVector::rotateYCCW(void) {
	sF32 xx=x;
	x=-z;
	z=xx;
}

void sFVector::rotateZCW(void) {
	sF32 xx=x;
	x=-y;
	y=xx;
}

void sFVector::rotateZCCW(void) {
	sF32 xx=x;
	x=y;
	y=-xx;
}

/****************************************************************************/
/****************************************************************************/
/***                                                                      ***/
/***   Float Matrix                                                       ***/
/***                                                                      ***/
/****************************************************************************/
/****************************************************************************/

void sFMatrix::init()
{
  i.init(1.0f, 0.0f, 0.0f);
  j.init(0.0f, 1.0f, 0.0f);
  k.init(0.0f, 0.0f, 1.0f);
}

void sFMatrix::init(sFMatrix *_m) {
	i.x=_m->i.x; i.y=_m->i.y; i.z=_m->i.z;
	j.x=_m->j.x; j.y=_m->j.y; j.z=_m->j.z;
	k.x=_m->k.x; k.y=_m->k.y; k.z=_m->k.z;
}

void sFMatrix::init(const sFVector &v)
{
  j.init(0,1,0);
  k = v;
  k.unit();
  i.Cross(j,k);
  i.unit();
  j.Cross(k,i);
  j.unit();
}


void sFMatrix::init(sF32 ry,sF32 rx,sF32 rz) // euler rotation
{
  sF32 cx,cy,cz;
  sF32 sx,sy,sz;

  cx=(sF32)sFCos(rx);
  cy=(sF32)sFCos(ry);
  cz=(sF32)sFCos(rz);
  sx=(sF32)sFSin(rx);
  sy=(sF32)sFSin(ry);
  sz=(sF32)sFSin(rz);
  
  i.x =  cx*cz - sx*sy*sz;
  i.y = -cy*sz;
  i.z =  sx*cz + cx*sy*sz;
  j.x =  cx*sz + sx*sy*cz;
  j.y =  cy*cz;
  j.z =  sx*sz - cx*sy*cz;
  k.x = -sx*cy;
  k.y =  sy;
  k.z =  cx*cy;
}

void sFMatrix::initDeg(sF32 ry,sF32 rx,sF32 rz)
{
  sF32 cx,cy,cz;
  sF32 sx,sy,sz;

  rx*=(sF32)(2.0f*sM_PI/360.0f);
  ry*=(sF32)(2.0f*sM_PI/360.0f);
  rz*=(sF32)(2.0f*sM_PI/360.0f);
  cx=(sF32)sFCos(rx);
  cy=(sF32)sFCos(ry);
  cz=(sF32)sFCos(rz);
  sx=(sF32)sFSin(rx);
  sy=(sF32)sFSin(ry);
  sz=(sF32)sFSin(rz);
  
  i.x =  cx*cz - sx*sy*sz;
  i.y = -cy*sz;
  i.z =  sx*cz + cx*sy*sz;
  j.x =  cx*sz + sx*sy*cz;
  j.y =  cy*cz;
  j.z =  sx*sz - cx*sy*cz;
  k.x = -sx*cy;
  k.y =  sy;
  k.z =  cx*cy;
}

void sFMatrix::loadIdentity(void) {
	i.x=1.0f;
	j.y=1.0f;
	k.z=1.0f;
	i.y=i.z=j.x=j.z=k.x=k.y=0.0f;
}


/************************************************************************/

void sFMatrix::Mul(const sFMatrix &a,const sFMatrix &b)
{
  i.x= a.i.x*b.i.x + a.i.y*b.j.x + a.i.z*b.k.x;
  j.x= a.j.x*b.i.x + a.j.y*b.j.x + a.j.z*b.k.x;
  k.x= a.k.x*b.i.x + a.k.y*b.j.x + a.k.z*b.k.x;

  i.y= a.i.x*b.i.y + a.i.y*b.j.y + a.i.z*b.k.y;
  j.y= a.j.x*b.i.y + a.j.y*b.j.y + a.j.z*b.k.y;
  k.y= a.k.x*b.i.y + a.k.y*b.j.y + a.k.z*b.k.y;

  i.z= a.i.x*b.i.z + a.i.y*b.j.z + a.i.z*b.k.z;
  j.z= a.j.x*b.i.z + a.j.y*b.j.z + a.j.z*b.k.z;
  k.z= a.k.x*b.i.z + a.k.y*b.j.z + a.k.z*b.k.z;
} 


void sFMatrix::MulInv(const sFMatrix &a,const sFMatrix &b)
{
  i.x= a.i.x*b.i.x + a.i.y*b.i.y + a.i.z*b.i.z;
  j.x= a.j.x*b.i.x + a.j.y*b.i.y + a.j.z*b.i.z;
  k.x= a.k.x*b.i.x + a.k.y*b.i.y + a.k.z*b.i.z;

  i.y= a.i.x*b.j.x + a.i.y*b.j.y + a.i.z*b.j.z;
  j.y= a.j.x*b.j.x + a.j.y*b.j.y + a.j.z*b.j.z;
  k.y= a.k.x*b.j.x + a.k.y*b.j.y + a.k.z*b.j.z;

  i.z= a.i.x*b.k.x + a.i.y*b.k.y + a.i.z*b.k.z;
  j.z= a.j.x*b.k.x + a.j.y*b.k.y + a.j.z*b.k.z;
  k.z= a.k.x*b.k.x + a.k.y*b.k.y + a.k.z*b.k.z;
} 

void sFMatrix::invert()
{
  sF32 s;

  s   = i.y;
  i.y = j.x;
  j.x = s;
  s   = i.z;
  i.z = k.x;
  k.x = s;
  s   = j.z;
  j.z = k.y;
  k.y = s;
}


/************************************************************************/

void sFMatrix::Add(const sFMatrix &m)
{
  i.Add(m.i);
  j.Add(m.j);
  k.Add(m.k);
}

void sFMatrix::Sub(const sFMatrix &m)
{
  i.Sub(m.i);
  j.Sub(m.j);
  k.Sub(m.k);
}

void sFMatrix::scale(sF32 s)
{
  i.scalef(s);
  j.scalef(s);
  k.scalef(s);
}

