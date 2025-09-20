/// color.cpp
///
/// (c) 2001-2014 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#define YAC_BIGSTRING
#include <yac.h>

#include "../tkopengl/inc_opengl.h"
#include "../tkopengl/tkopengl_shared.h"

#include "chaosmath.h"
#include "tksmath.h"
#include "color.h"

extern YAC_Host *yac_host;
extern sUI       clid_Color;
extern sUI       clid_Vector;
extern sUI       clid_Vector4;

#define IFYAC_CHK(a,b) if(YAC_CHK(a,b))
//#define IFYAC_CHK(a,b) 


_Color::_Color(void) {
   rf=0; 
   gf=0;
   bf=0;
   af=1.0f;
   gamma=1.0f;
   pi=0;
   recalcpi=0;
}

_Color::~_Color() {
}

void YAC_VCALL _Color::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   
   YAC_SERIALIZE_F32(rf);
   YAC_SERIALIZE_F32(gf);
   YAC_SERIALIZE_F32(bf);
   YAC_SERIALIZE_F32(af);
   YAC_SERIALIZE_F32(gamma);
}

sUI YAC_VCALL _Color::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   
   rf=YAC_DESERIALIZE_F32();
   gf=YAC_DESERIALIZE_F32();
   bf=YAC_DESERIALIZE_F32();
   af=YAC_DESERIALIZE_F32();
   gamma=YAC_DESERIALIZE_F32();
   
   recalcpi=1;
   return 1;
}

void YAC_VCALL _Color::yacOperator(sSI _cmd, YAC_Object*_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Color))
   {
      _Color *o=(_Color*)_o;
      switch(_cmd)
      {
      case YAC_OP_ASSIGN:
         rf=o->rf;
         gf=o->gf;
         bf=o->bf;
         af=o->af;
         gamma=o->gamma;
         pi=o->pi;
         recalcpi=o->recalcpi;
         break;
      case YAC_OP_ADD:
         {
            _Color *r=new _Color();
            r->gamma=gamma;
            r->recalcpi=1;
            r->rf=rf+o->rf;
            r->gf=gf+o->gf;
            r->bf=bf+o->bf;
            r->af=af;
            YAC_RETO(r, 1);
         }
         break;
      case YAC_OP_SUB:
         {
            _Color *r=new _Color();
            r->gamma=gamma;
            r->recalcpi=1;
            r->rf=rf-o->rf;
            r->gf=gf-o->gf;
            r->bf=bf-o->bf;
            r->af=af;
            if(r->rf<0.0)  r->rf=0.0;
            if(r->gf<0.0)  r->gf=0.0;
            if(r->bf<0.0)  r->bf=0.0;
            YAC_RETO(r, 1);
         }
         break;
      case YAC_OP_MUL:
         {
            _Color *r=new _Color();
            r->gamma=gamma;
            r->recalcpi=1;
            r->rf=rf*o->rf;
            r->gf=gf*o->gf;
            r->bf=bf*o->bf;
            r->af=af;
            YAC_RETO(r, 1);
         }
         break;
      case YAC_OP_DIV:
         {
            _Color *r=new _Color();
            r->gamma=gamma;
            r->recalcpi=1;
            r->rf=rf+(o->rf-rf)*0.5f;
            r->gf=gf+(o->gf-gf)*0.5f;
            r->bf=bf+(o->bf-bf)*0.5f;
            r->af=af;
            YAC_RETO(r, 1);
         }
         break;
      }
   }
   else
   {
      if(YAC_CHK(_o, clid_Vector4))
      {
         _Vector4 *v4=(_Vector4*)_o;
         rf=v4->x; gf=v4->y; bf=v4->z; af=v4->w;
         recalcpi=1;
      }
      else
      {
         if(YAC_CHK(_o, clid_Vector))
         {
            _Vector *v=(_Vector*)_o;
            rf=v->x; gf=v->y; bf=v->z;
            recalcpi=1;
         }
         else
         {
            YAC_Object::yacOperator(_cmd, _o, _r);
         }
      }
   }
}

#define GETPI recalcpi?recalcPi():pi

sUI _Color::recalcPi(void) {
   recalcpi=0;
   sUI cr=(sUI)(rf*gamma*255);
   if(cr>255)
      cr=255<<16;
   else
      cr=cr<<16;
   sUI cg=(sUI)(gf*gamma*255);
   if(cg>255)
      cg=255<<8;
   else
      cg=cg<<8;
   sUI cb=(sUI)(bf*gamma*255);
   if(cb>255)
      cb=255;
   return pi=(((sU16)(af*255))<<24)|cr|cg|cb;
}


// sUI _Color::getColor32(void) {
// 	return GETPI;
// }

sUI _Color::_getColori(void) {
   return GETPI;
}

sUI _Color::_getColor4b(void) {
   sUI i=GETPI;
#ifdef YAC_LITTLE_ENDIAN
   return 
      (((i>>16)&0xFF)<<0 )|
      (((i>> 8)&0xFF)<<8 )|
      (((i    )&0xFF)<<16)|
      (((i>>24)&0xFF)<<24);
#else
   return //argb -> #rgba
      (((i>>24)&0xFF)<<0 )|
      (((i>> 8)&0xFF)<<8 )|
      (((i    )&0xFF)<<16)|
      (((i>>16)&0xFF)<<24);
#endif
}

sUI _Color::_setColori(sSI _pi) {
   af=(sF32)(((((sUI)_pi)>>24)&0xFF)/255.0f);
   rf=(sF32)(((((sUI)_pi)>>16)&0xFF)/255.0f);
   gf=(sF32)(((((sUI)_pi)>> 8)&0xFF)/255.0f);
   bf=(sF32)(((((sUI)_pi)    )&0xFF)/255.0f);
   return recalcPi();
}

sUI _Color::_setRgbpi(sSI _pi) { // deprecated
   af=(sF32)(((((sUI)_pi)>>24)&0xFF)/255.0f);
   rf=(sF32)(((((sUI)_pi)>>16)&0xFF)/255.0f);
   gf=(sF32)(((((sUI)_pi)>> 8)&0xFF)/255.0f);
   bf=(sF32)(((((sUI)_pi)    )&0xFF)/255.0f);
   return recalcPi();
}

sUI _Color::_init4i(sSI _r, sSI _g, sSI _b, sSI _a) {
   rf=(sF32)((_r&0xFF)/255.0f);
   gf=(sF32)((_g&0xFF)/255.0f);
   bf=(sF32)((_b&0xFF)/255.0f);
   af=(sF32)((_a&0xFF)/255.0f);
   gamma=1.0f;
   return recalcPi();
}

sUI _Color::_init4f(sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
   af=_a;
   rf=_r;
   gf=_g;
   bf=_b;
   gamma=1.0f;
   return recalcPi();
}

sUI _Color::_init3f(sF32 _r, sF32 _g, sF32 _b) {
   rf=_r;
   gf=_g;
   bf=_b;
   gamma=1.0f;
   return recalcPi();
}

sUI _Color::_initGamma3f(sF32 _r, sF32 _g, sF32 _b, sF32 _gamma) {
   rf=_r;
   gf=_g;
   bf=_b; 
   gamma=_gamma;
   return recalcPi();
}

sUI _Color::_getRi(void) {
   sUI r=(sUI)(255.0f*gamma*rf);
   if(r>255) r=255;
   return r;
}

sUI _Color::_getGi(void) {
   sUI g=(sUI)(255.0f*gamma*gf);
   if(g>255) g=255;
   return g;
}

sUI _Color::_getBi(void) {
   sUI b=(sUI)(255.0f*gamma*bf);
   if(b>255) b=255;
   return b;
}

sUI _Color::_getAi(void) {
   sUI a=(sUI)(255.0f*gamma*af);
   if(a>255) a=255;
   return a;
}

sF32 _Color::_getRf(void) {
   sF32 r=gamma*rf;
   if(r>1.0f) r=1.0f;
   return r;
}

sF32 _Color::_getGf(void) {
   sF32 g=gamma*gf;
   if(g>1.0f) g=1.0f;
   return g;
}

sF32 _Color::_getBf(void) {
   sF32 b=gamma*bf;
   if(b>1.0f) b=1.0f;
   return b;
}

sF32 _Color::_getAf(void) {
   sF32 a=gamma*af;
   if(a>1.0f) a=1.0f;
   return a;
}

void _Color::_setAi(sSI _a) {
   af=(sF32)((_a&255)/255.0f);
   recalcpi=1;
}

void _Color::_setAf(sF32 _a) {
   af=_a;
   recalcpi=1;
}

void _Color::_setRi(sSI _r) {
   rf=(sF32)((_r&255)/255.0f);
   recalcpi=1;
}

void _Color::_setRf(sF32 _r) {
   rf=_r;
   recalcpi=1;
}

void _Color::_setGi(sSI _g) {
   gf=(sF32)((_g&255)/255.0f);
   recalcpi=1;
}

void _Color::_setGf(sF32 _g) {
   gf=_g;
   recalcpi=1;
}

void _Color::_setBi(sSI _b) {
   bf=(_b&255)/255.0f;
   recalcpi=1;
}

void _Color::_setBf(sF32 _b) {
   bf=_b;
   recalcpi=1;
}

sF32 _Color::_getGamma(void) {
   return gamma;
}

void _Color::_setGamma(sF32 _gamma) {
   gamma=_gamma;
   recalcpi=1;
}

sUI _Color::_getBlendf(sF32 _blend) {
   sUI cr=(sUI)((rf+_blend)*gamma*255);
   if(cr>255) cr=255<<16;
   else cr=cr<<16;
   sUI cg=(sUI)((gf+_blend)*gamma*255);
   if(cg>255) cg=255<<8;
   else cg=cg<<8;
   sUI cb=(sUI)((bf+_blend)*gamma*255);
   if(cb>255) cb=255;
   return (((sU16)(af*255))<<24)|cr|cg|cb;
}

sUI _Color::_getGammaf(sF32 _gamma) {
   sUI cr=(sUI)(rf*_gamma*255);
   if(cr>255) cr=255<<16;
   else cr=cr<<16;
   sUI cg=(sUI)(gf*_gamma*255);
   if(cg>255) cg=255<<8;
   else cg=cg<<8;
   sUI cb=(sUI)(bf*_gamma*255);
   if(cb>255) cb=255;
   return (((sU16)(af*255))<<24)|cr|cg|cb;
}

void _Color::_getString(YAC_Value *_r) {
   sUI v=GETPI;
   YAC_String *rs= YAC_New_String();
   rs->yacArrayAlloc(16, 0,0,0);
   rs->printf("#%08x",v);
   _r->initString(rs, 1);
}

sUI _Color::_mixf(YAC_Object *_o, sF32 _a) {
   IFYAC_CHK(_o, clid_Color)
   {
      _Color *o=(_Color *)_o;
      sF32 ar;
      sF32 ag;
      sF32 ab;
      ar=rf*af;
      ag=gf*af;
      ab=bf*af;
      sUI r;
      sUI g;
      sUI b;
      r=(sUI)(255*(ar+(o->af*o->rf-ar)*_a));
      g=(sUI)(255*(ag+(o->af*o->gf-ag)*_a));
      b=(sUI)(255*(ab+(o->af*o->bf-ab)*_a));
      if(r>255) r=255;
      if(g>255) g=255;
      if(b>255) b=255;
      return 0xFF000000|(r<<16)|(g<<8)|b;
   }
   else
   {
      return 0;
   }
}

void _Color::_glColor(void) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glColor4f(rf*gamma, gf*gamma, bf*gamma, af);
}

void _Color::_glColorGamma(sF32 _g) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glColor4f(rf*_g, gf*_g, bf*_g, af);
}

sF32 *_Color::getRGBAfv(void) {
   static GLfloat rgba[4];
   rgba[0]=rf*gamma;
   rgba[1]=gf*gamma;
   rgba[2]=bf*gamma;
   rgba[3]=af*gamma;
   return rgba;
}

void _Color::_glLightMaterialAmbient(sSI _face) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glMaterialfv(_face, GL_AMBIENT, getRGBAfv());
}

void _Color::_glLightMaterialDiffuse(sSI _face) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glMaterialfv(_face,GL_DIFFUSE, getRGBAfv());
}

void _Color::_glLightMaterialAmbientAndDiffuse(sSI _face) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glMaterialfv(_face, GL_AMBIENT_AND_DIFFUSE, getRGBAfv());
}


void _Color::_glLightMaterialSpecular(sSI _face) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glMaterialfv(_face, GL_SPECULAR, getRGBAfv());
}

void _Color::_glLightMaterialEmission(sSI _face) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glMaterialfv(_face, GL_EMISSION, getRGBAfv());
}
