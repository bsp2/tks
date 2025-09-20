/// tksmathstatic.cpp
///
/// (c) 2002-2014 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <stdio.h>
#include <math.h>

#include <yac.h>
#include "chaosmath.h"
#include "tksmath.h"

extern sUI clid_Vector;


_Math::_Math(void) {}

_Math::~_Math() {}

sF32 _Math::_powerXY(sF32 _x, sF32 _y) {
    return (sF32)exp(_y*log(_x));
}

sF32 _Math::_max(sF32 _x, sF32 _y) {
    return _x>_y?_x:_y;
}

sF32 _Math::_min(sF32 _x, sF32 _y) {
    return _x<_y?_x:_y;
}

sF32 _Math::_absmax(sF32 _x, sF32 _y) {
    return (_x<0?-_x:_x)>(_y<0?-_y:_y)?_x:_y;
}

sF32 _Math::_absmin(sF32 _x, sF32 _y) {
    return (_x<0?-_x:_x)<(_y<0?-_y:_y)?_x:_y;
}

YAC_Object *_Math::_normal(YAC_Object *_v1, YAC_Object *_v2, YAC_Object *_v3) {
   if(YAC_CHK(_v1, clid_Vector))
   {
      if(YAC_CHK(_v2, clid_Vector))
      {
         if(YAC_CHK(_v3, clid_Vector))
         {
            _Vector *v1=(_Vector*)_v1;
            _Vector *v2=(_Vector*)_v2;
            _Vector *v3=(_Vector*)_v3;
            sFVector a;
            sFVector b;
            a.x= v1->x - v2->x;
            a.y= v1->y - v2->y;
            a.z= v1->z - v2->z;                   
            b.x= v2->x - v3->x;
            b.y= v2->y - v3->y;
            b.z= v2->z - v3->z;
            ret_vector.x=a.y*b.z - a.z*b.y;
            ret_vector.y=a.z*b.x - a.x*b.z;
            ret_vector.z=a.x*b.y - a.y*b.x;
            return &ret_vector;
         }
      }
   }
	return 0;
    //C_RETO(0, 0);
}

sUI _Math::_c32Multiply(sUI c32a, sUI c32b, sF32 _bscale) {
	sUI r32=0;
	sU8 ca;
	sU8 cb;
	// ---- alpha ----
	ca=(c32a>>24)&0xFF;
	cb=(c32b>>24)&0xFF;
	r32|=((sU32)(ca+ (ca*cb*1.0f/256.0f-ca)*_bscale))<<24;
	// ---- red ----
	ca=(c32a>>16)&0xFF;
	cb=(c32b>>16)&0xFF;
	r32|=((sU32)(ca+ (ca*cb*1.0f/256.0f-ca)*_bscale))<<16;
	// ---- green ----
	ca=(c32a>>8)&0xFF;
	cb=(c32b>>8)&0xFF;
	r32|=((sU32)(ca+ (ca*cb*1.0f/256.0f-ca)*_bscale))<<8;
	// ---- green ----
	ca=c32a&0xFF;
	cb=c32b&0xFF;
	r32|=((sU32)(ca+ (ca*cb*1.0f/256.0f-ca)*_bscale));

	return r32;
}

sUI _Math::_c32Blend(sUI c32a, sUI c32b, sF32 _bscale) {
	sU32 r32=0;
	sU8 ca;
	sU8 cb;
	// ---- alpha ----
	ca=(c32a>>24)&0xFF;
	cb=(c32b>>24)&0xFF;
	r32|=((sU32)(ca+ (cb-ca)*_bscale))<<24;
	// ---- red ----
	ca=(c32a>>16)&0xFF;
	cb=(c32b>>16)&0xFF;
	r32|=((sU32)(ca+ (cb-ca)*_bscale))<<16;
	// ---- green ----
	ca=(c32a>>8)&0xFF;
	cb=(c32b>>8)&0xFF;
	r32|=((sU32)(ca+ (cb-ca)*_bscale))<<8;
	// ---- green ----
	ca=c32a&0xFF;
	cb=c32b&0xFF;
	r32|=((sU32)(ca+ (cb-ca)*_bscale));

	return r32;
}

void _Math::_reflect(YAC_Object *_in, YAC_Object *_n, YAC_Object *_ret) {
	if(YAC_CHK(_in, clid_Vector))
		if(YAC_CHK(_n, clid_Vector))
			if(YAC_CHK(_ret, clid_Vector))
			{
				_Vector *vi=(_Vector*)_in;
				_Vector *n=(_Vector*)_n;
				_Vector *r=(_Vector*)_ret;
				// ---- calc reflection vector ----
				// ---- I: incoming ray        ----
				// ---- S: surface normal      ----
				// ---- L= unit(I)             ----
				// ---- N= unit(S)             ----
				// ---- L'=(L*N)*N             ----
				// ---- R'=-L + 2*L'           ----
				// ---- R= R'*|I|              ----
				// ---- R: reflected ray       ----
				sFVector vt;
				sFVector vt1;
				sFVector vt2;
				vt2.x=n->x; vt2.y=n->y; vt2.z=n->z;
				vt2.unit();                             // ---- N= unit(S)
				vt1.x=vi->x; vt1.y=vi->y; vt1.z=vi->z;  // incoming ray
				sF32 l=vt1.Abs();                       // ---- |I|           
				vt1.unit();                             // ---- L= unit(I)
				// ---- L'=(L*N)*N; ----
				sF32 f_2=vt1.Dot(vt2);  
				vt.x=vt2.x; vt.y=vt2.y; vt.z=vt2.z;
				vt.scalef(f_2);
				r->x=vt.x; r->y=vt.y; r->z=vt.z;
				r->Add(*r);
				r->Sub(vt1);
				// ---- R= R'*|M|; ----
				r->scalef(l);
				//return vt2;
			}
}

void _Math::_vectorMin(YAC_Object *_d, YAC_Object *_v1, YAC_Object *_v2) {
	if(YAC_CHK(_d, clid_Vector))
		if(YAC_CHK(_v1, clid_Vector))
			if(YAC_CHK(_v2, clid_Vector))
			{
				_Vector *d=(_Vector*)_d;
				_Vector *v1=(_Vector*)_v1;
				_Vector *v2=(_Vector*)_v2;
				d->x=v1->x<v2->x?v1->x:v2->x;
				d->y=v1->y<v2->y?v1->y:v2->y;
				d->z=v1->z<v2->z?v1->z:v2->z;
			}
}

void _Math::_vectorMax(YAC_Object *_d, YAC_Object *_v1, YAC_Object *_v2) {
	if(YAC_CHK(_d, clid_Vector))
		if(YAC_CHK(_v1, clid_Vector))
			if(YAC_CHK(_v2, clid_Vector))
			{
				_Vector *d=(_Vector*)_d;
				_Vector *v1=(_Vector*)_v1;
				_Vector *v2=(_Vector*)_v2; 
				d->x=v1->x>=v2->x?v1->x:v2->x;
				d->y=v1->y>=v2->y?v1->y:v2->y;
				d->z=v1->z>=v2->z?v1->z:v2->z;
			}
}

sF32 _Math::_distancePointPlane(YAC_Object *_p, YAC_Object *_q, YAC_Object *_n) {
	if(YAC_CHK(_p, clid_Vector))
		if(YAC_CHK(_q, clid_Vector))
			if(YAC_CHK(_n, clid_Vector))
			{
				_Vector *p=(_Vector*)_p;
				_Vector *q=(_Vector*)_q;
				_Vector *n=(_Vector*)_n;
				sFVector v;
				v.x=p->x-q->x; v.y=p->y-q->y; v.z=p->z-q->z;
				return (v.Dot(*n)/n->Abs());
			}
	return 0.0f;
}
