/// tkoldmath.cpp
///
/// (c) 2001-2014 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///   Note: These APIs are more or less deprecated. Originally, they were part of the "tkopengl" plugin.
///

#define TKS_NUM_TMP_VECTOR   32
#define TKS_NUM_TMP_IVECTOR  32
#define TKS_NUM_TMP_VECTOR4  32

#define TKS_MASK_TMP_VECTOR  31
#define TKS_MASK_TMP_IVECTOR 31
#define TKS_MASK_TMP_VECTOR4 31

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YAC_PRINTF defined
#define YAC_BIGSTRING
#include <yac.h>

#include <yac_host.cpp>

#include "../tkopengl/inc_opengl.h"

#include "../tkopengl/tkopengl_shared.h"
#include "../tkopengl/tkopengl_shared.cpp"

// // (todo) remove GLU
// #ifdef YAC_MACOS
// #include <OpenGL/glu.h>
// #else
// // linux, windows
// #include <GL/glu.h>
// #endif

#include "chaosmath.h"
#include "tksmath.h"
#include "color.h"
#include "tkoldmath.h"

extern sUI clid_Color;
extern sUI clid_IVector;
extern sUI clid_Matrix;
extern sUI clid_Vector;
extern sUI clid_Vector4;
extern sUI clid_VectorArray;

YAC_Host *yac_host;

#include "ying_OldMath.h"




// ---- to emulate the (old) ivector(), vector(), vector4() expressions using functions
// ---- ...these were faster once because of constant expr optimizations...
static sUI      tmp_next_ivector;
static _IVector*tmp_ivector;
static sUI      tmp_next_vector;
static _Vector* tmp_vector;
static sUI      tmp_next_vector4;
static _Vector4*tmp_vector4;
//static _IVector tmp_ivector[TKS_NUM_TMP_IVECTOR];
//static _Vector  tmp_vector[TKS_NUM_TMP_VECTOR];
//static _Vector4 tmp_vector4[TKS_NUM_TMP_VECTOR4];

YAC_Object *YAC_CALL _ivector(sSI _x, sSI _y) {
	_IVector *v=&tmp_ivector[tmp_next_ivector++];
	tmp_next_ivector&=TKS_MASK_TMP_IVECTOR;
	v->x=_x; v->y=_y;
	return v;
}

YAC_Object *YAC_CALL _vector(sF32 _x, sF32 _y, sF32 _z) {
	_Vector *v=&tmp_vector[tmp_next_vector++];
	tmp_next_vector&=TKS_MASK_TMP_VECTOR;
	v->x=_x; v->y=_y; v->z=_z;
	return v;
}

YAC_Object *YAC_CALL _vector4(sF32 _x, sF32 _y, sF32 _z, sF32 _w) {
	_Vector4 *v=&tmp_vector4[tmp_next_vector4++];
	tmp_next_vector4&=TKS_MASK_TMP_VECTOR4;
	v->x=_x; v->y=_y; v->z=_z; v->w=_w;
	return v;
}

sUI YAC_CALL _mathC32Multiply(sUI c32a, sUI c32b, sF32 _bscale) {
	sUI r32=0;
	sU8 ca;
	sU8 cb;
	// ---- alpha ----
	ca=(c32a>>24)&0xFF;
	cb=(c32b>>24)&0xFF;
	r32|=((sUI)(ca+ (ca*cb*1.0f/256.0f-ca)*_bscale))<<24;
	// ---- red ----
	ca=(c32a>>16)&0xFF;
	cb=(c32b>>16)&0xFF;
	r32|=((sUI)(ca+ (ca*cb*1.0f/256.0f-ca)*_bscale))<<16;
	// ---- green ----
	ca=(c32a>>8)&0xFF;
	cb=(c32b>>8)&0xFF;
	r32|=((sUI)(ca+ (ca*cb*1.0f/256.0f-ca)*_bscale))<<8;
	// ---- green ----
	ca=c32a&0xFF;
	cb=c32b&0xFF;
	r32|=((sUI)(ca+ (ca*cb*1.0f/256.0f-ca)*_bscale));

	return r32;
}

sUI YAC_CALL _mathC32Blend(sUI c32a, sUI c32b, sF32 _bscale) {
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

void YAC_CALL _mathNormal(YAC_Object *_v1, YAC_Object *_v2, YAC_Object*_v3, YAC_Object*_ret){
    if(YAC_CHK(_v1, clid_Vector))
        if(YAC_CHK(_v2, clid_Vector))
            if(YAC_CHK(_v3, clid_Vector))
				if(YAC_CHK(_ret, clid_Vector))
				{
					_Vector *v1=(_Vector*)_v1;
					_Vector *v2=(_Vector*)_v2;
					_Vector *v3=(_Vector*)_v3;
					_Vector *r=(_Vector*)_ret;
					sFVector a;
					sFVector b;
					a.x= v1->x - v2->x;
		            a.y= v1->y - v2->y;
	                a.z= v1->z - v2->z;                   
					b.x= v2->x - v3->x;
					b.y= v2->y - v3->y;
					b.z= v2->z - v3->z;
					r->x=a.y*b.z - a.z*b.y;
					r->y=a.z*b.x - a.x*b.z;
					r->z=a.x*b.y - a.y*b.x;
				}
}

void YAC_CALL _mathUnproject(YAC_Object *_v, YAC_Object *_rnear, YAC_Object *_rfar) {
	if(YAC_CHK(_v, clid_Vector))
   {
      if(YAC_CHK(_rnear, clid_Vector))
      {
         if(YAC_CHK(_rfar, clid_Vector))
         {
            _Vector *vnear=(_Vector*)_rnear;
            _Vector *vfar=(_Vector*)_rfar;
            _Vector *v=(_Vector*)_v;
            
            tkopengl_shared_resolve();
            tkopengl_shared->_unproject(v->x, v->y,
                                        &vnear->x, &vnear->y, &vnear->z,
                                        &vfar->x, &vfar->y, &vfar->z
                                        );
         }
      }
   }
}

void YAC_CALL _mathProject(YAC_Object *_v, YAC_Object *_vret) {
	if(YAC_CHK(_v, clid_Vector))
   {
      if(YAC_CHK(_vret, clid_Vector))
      {
         _Vector *v=(_Vector *)_v;
         _Vector *vret=(_Vector *)_vret;

         tkopengl_shared_resolve();
         tkopengl_shared->_project(v->x, v->y, v->z, &vret->x, &vret->y, &vret->z);
         
#if 0
			GLdouble prjmatrix[4*4];
			GLdouble mvmatrix[4*4];
			GLint view[4];
			::glGetDoublev(GL_PROJECTION_MATRIX, prjmatrix);
			::glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
			::glGetIntegerv(GL_VIEWPORT, view);
			// project mmm
			GLdouble rx;
			GLdouble ry;
			GLdouble rz;
			::gluProject((GLdouble)v->x, (GLdouble)v->y, (GLdouble)v->z, mvmatrix, prjmatrix, view, &rx, &ry, &rz);
			vret->x=(sF32)rx;
			vret->y=(sF32)ry;
			vret->z=(sF32)rz;
#endif
      }
   }
}

void YAC_CALL _mathReflect(YAC_Object *_in, YAC_Object *_n, YAC_Object *_ret) {
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
			}
}

sF32 YAC_CALL _mathDistancePointPlane(YAC_Object *_p, YAC_Object *_q, YAC_Object *_n) {
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

void YAC_CALL _mathVectorMin(YAC_Object *_v1, YAC_Object *_v2, YAC_Object *_r) {
	if(YAC_CHK(_r, clid_Vector))
		if(YAC_CHK(_v1, clid_Vector))
			if(YAC_CHK(_v2, clid_Vector))
			{
				_Vector *v1=(_Vector*)_v1;
				_Vector *v2=(_Vector*)_v2;
				_Vector *r=(_Vector*)_r;
				r->x=v1->x<v2->x?v1->x:v2->x;
				r->y=v1->y<v2->y?v1->y:v2->y;
				r->z=v1->z<v2->z?v1->z:v2->z;
			}
}

void YAC_CALL _mathVectorMax(YAC_Object *_v1, YAC_Object *_v2, YAC_Object *_r) {
	if(YAC_CHK(_r, clid_Vector))
		if(YAC_CHK(_v1, clid_Vector))
			if(YAC_CHK(_v2, clid_Vector))
			{
				_Vector *v1=(_Vector*)_v1;
				_Vector *v2=(_Vector*)_v2;
				_Vector *r=(_Vector*)_r;
				r->x=v1->x>=v2->x?v1->x:v2->x;
				r->y=v1->y>=v2->y?v1->y:v2->y;
				r->z=v1->z>=v2->z?v1->z:v2->z;
			}
}

sUI YAC_CALL _mathC32ToC8(sUI _c32) {
	return (
		((((_c32>>16)&0xFF)*80)>>8) + 
		(((_c32>> 8 )&0xFF)>>1) + 
		((((_c32    )&0xFF)*47)>>8)
		);
}


void YAC_CALL _zglLightPositionv(sSI _lightnr, YAC_Object *_v, sF32 _w) {
   if(YAC_CHK(_v, clid_Vector))
   {
      _Vector *v=(_Vector*)_v;
      GLfloat xyz[4];
      xyz[0]=v->x;
      xyz[1]=v->y;
      xyz[2]=v->z;
      xyz[3]=_w;
      
      tkopengl_shared_resolve();
      tkopengl_shared->_glLightfv(_lightnr, GL_POSITION, xyz);
   }
}

void YAC_CALL _zglLightSpotDirectionv(sSI _lightnr, YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector))
   {
      _Vector *v=(_Vector*)_v;
      sF32 xyz[3]={v->x, v->y, v->z};

      tkopengl_shared_resolve();
      tkopengl_shared->_glLightfv(_lightnr, GL_SPOT_DIRECTION, xyz);
   }
}

void YAC_CALL _zglLoadMatrix(YAC_Object *_m, YAC_Object *_v) {
   if(YAC_CHK(_m, clid_Matrix))
   {
      if(YAC_CHK(_v, clid_Vector))
      {
         _Matrix *m=(_Matrix*)_m;
			_Vector *v=(_Vector*)_v;
			GLfloat ma[16];
         
			ma[0] =m->i.x;
			ma[4] =m->i.y;
			ma[8] =m->i.z;

			ma[1] =m->j.x;
			ma[5] =m->j.y;
			ma[9] =m->j.z;

			ma[2] =m->k.x;
			ma[6] =m->k.y;
			ma[10]=m->k.z; 

			ma[3] =0.0f;
			ma[7] =0.0f;
			ma[11] =0.0f;

			ma[12]=v->x;
			ma[13]=v->y;
			ma[14]=v->z;
			ma[15]=1.0f;

         tkopengl_shared_resolve();
         tkopengl_shared->_glLoadMatrixf((const GLfloat*)ma);
      }
   }
}	

void YAC_CALL _zglNormalv(YAC_Object *_v) {
	if(YAC_CHK(_v, clid_Vector))
	{
		_Vector *v=(_Vector*)_v;

		tkopengl_shared_resolve();
      tkopengl_shared->_glNormal3f(v->x, v->y, v->z);
	}
}

void YAC_CALL _zglRotate3f(sF32 _a, sF32 _b, sF32 _c) {
	_Matrix m;
	m.class_ID=clid_Matrix;
	double sc=2.0*sM_PI/360.0;
	m.init((sF32)(_a*sc), (sF32)(_b*sc), (sF32)(_c*sc));
	_zglRotatev(&m);
}

void YAC_CALL _zglRotatev(YAC_Object *_v) {
	if(YAC_CHK(_v, clid_Matrix))
	{
		_Matrix *m=(_Matrix *)_v;
		GLfloat tm[16];
		
		tm[0]=m->i.x;
		tm[4]=m->i.y;
		tm[8]=m->i.z;
		tm[12]=0;
		
		tm[1]=m->j.x;
		tm[5]=m->j.y;
		tm[9]=m->j.z;
		tm[13]=0;
		
		tm[2]=m->k.x;
		tm[6]=m->k.y;
		tm[10]=m->k.z;
		tm[14]=tm[3]=tm[7]=tm[11]=0;
		tm[15]=1.0f;

      tkopengl_shared_resolve();
      tkopengl_shared->_glMultMatrixf(tm);
	}
	else
	{
		if(YAC_CHK(_v, clid_Vector))
		{
			_Vector *v=(_Vector*)_v;
			_Matrix m;
			m.class_ID=clid_Matrix;
			sF32 sc=(sF32)(2.0f*sM_PI/360.0f);
			m.init((sF32)(v->x*sc), (sF32)(v->y*sc), (sF32)(v->z*sc));
			_zglRotatev(&m);
		}
	}
}

void YAC_CALL _zglScalev(YAC_Object *_v) {
	if(YAC_CHK(_v, clid_Vector))
	{
		_Vector *v=(_Vector*)_v;

      tkopengl_shared_resolve();
      tkopengl_shared->_glScalef(v->x, v->y, v->z);
	}
}

void YAC_CALL _zglStoreMatrix(YAC_Object *_m, YAC_Object *_v) {
   if(YAC_CHK(_m, clid_Matrix))
   {
      if(YAC_CHK(_v, clid_Vector))
      {
         _Matrix *m=(_Matrix*)_m;
			_Vector *v=(_Vector*)_v;
			sF32 ma[16];

         tkopengl_shared_resolve();
         tkopengl_shared->_zglStoreMatrixf(ma);

#if 0
         yac_host->printf("xxx tkoldmath::zglStoreMatrixf: ma={\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t}\n",
                          ma[0], ma[4], ma[8], ma[12],
                          ma[1], ma[5], ma[9], ma[13],
                          ma[2], ma[6], ma[10], ma[14],
                          ma[3], ma[7], ma[11], ma[15]
                          );
#endif

			m->i.x=ma[0];
			m->i.y=ma[4];
			m->i.z=ma[8];

			m->j.x=ma[1];
			m->j.y=ma[5];
			m->j.z=ma[9];

			m->k.x=ma[2];
			m->k.y=ma[6];
			m->k.z=ma[10]; 

			v->x=ma[12];
			v->y=ma[13];
			v->z=ma[14];
		}
   }
}

void YAC_CALL _zglTranslatev(YAC_Object *_v) {
    if(YAC_CHK(_v, clid_Vector))
    {
		_Vector *v=(_Vector*)_v;

      tkopengl_shared_resolve();
      tkopengl_shared->_glTranslatef(v->x, v->y, v->z);
    }
}

// void YAC_CALL _zgluLookAt(YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up) {
// 	if(YAC_CHK(_eye, clid_Vector))
// 	if(YAC_CHK(_center, clid_Vector))
// 	if(YAC_CHK(_up, clid_Vector))
// 	{
// 		_Vector *eye=(_Vector*)_eye;
// 		_Vector *center=(_Vector*)_center;
// 		_Vector *up=(_Vector*)_up;
// 		::gluLookAt(eye->x, eye->y, eye->z,
// 			  center->x, center->y, center->z,
// 			  up->x, up->y, up->z
// 		);
// 	}
// }

void YAC_CALL _zglVertexv(YAC_Object *_v) {
	if(YAC_CHK(_v, clid_Vector))
	{
		_Vector *v=(_Vector*)_v;

      tkopengl_shared_resolve();
      tkopengl_shared->_glVertex3f(v->x, v->y, v->z);
	}
}


/// ---------------------------------------------------------------------------
#include "ying_OldMath.cpp"
#include "ying_OldMath_Color.cpp"
#include "ying_OldMath_IVector.cpp"
#include "ying_OldMath_Math.cpp"
#include "ying_OldMath_Matrix.cpp"
#include "ying_OldMath_Vector.cpp"
#include "ying_OldMath_Vector4.cpp"
#include "ying_OldMath_VectorArray.cpp"
#include "ying_OldMath_WrappedFloat.cpp"


// ---------------------------------------------------------------------------
void YAC_CALL YAC_Init(YAC_Host *_host) {
	if(_host->yacGetDebugLevel())
		_host->printf("[dbg] tkoldmath::YAC_Init called.\n");
	yac_host=_host;

	YAC_Init_OldMath (_host);

	// ---- volatile vectors ----
	tmp_next_ivector=0;
	tmp_next_vector=0;
	tmp_next_vector4=0;

   tmp_ivector = new _IVector[TKS_NUM_TMP_IVECTOR];
   tmp_vector  = new _Vector [TKS_NUM_TMP_VECTOR ];
   tmp_vector4 = new _Vector4[TKS_NUM_TMP_VECTOR4];

	sUI i; 
	for(i=0; i<TKS_NUM_TMP_IVECTOR; i++) tmp_ivector[i].class_ID=clid_IVector;
	for(i=0; i<TKS_NUM_TMP_VECTOR;  i++) tmp_vector [i].class_ID=clid_Vector;
	for(i=0; i<TKS_NUM_TMP_VECTOR4; i++) tmp_vector4[i].class_ID=clid_Vector4;

	if(yac_host->yacGetDebugLevel())
		yac_host->yacPrint("[dbg] tkoldmath::YAC_Init() finished.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

   delete [] tmp_ivector; tmp_ivector = 0;
   delete [] tmp_vector;  tmp_vector  = 0;
   delete [] tmp_vector4; tmp_vector4 = 0;

	YAC_Exit_OldMath (_host);

	if(yac_host->yacGetDebugLevel())
		yac_host->yacPrint("[dbg] tkoldmath::YAC_Exit() finished.\n");
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00090830;
}

