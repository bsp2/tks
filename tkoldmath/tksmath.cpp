/// tksmath.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <stdio.h>

#define YAC_PRINTF defined
#define YAC_BIGSTRING defined
#include <yac.h>

#include "../tkopengl/inc_opengl.h"
#include "../tkopengl/tkopengl_shared.h"

#include "chaosmath.h"
#include "tkoldmath.h"
#include "tksmath.h"


extern YAC_Host *yac_host;
extern sUI clid_Vector;
extern sUI clid_VectorArray;
extern sUI clid_Vector4;
extern sUI clid_IVector;
extern sUI clid_Matrix;

#define IFYAC_CHK(a,b) if(YAC_CHK(a,b))
//#define IFYAC_CHK(a,b) 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ Vector                                                         ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_Vector::_Vector(void) {
   x=y=z=0.0f;
}

_Vector::~_Vector() {
}

YAC_Object *YAC_VCALL _Vector::yacArrayNew(void) {
   return yac_host->yacNewByID(clid_VectorArray);
}

void YAC_VCALL _Vector::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   
   YAC_SERIALIZE_F32(x);
   YAC_SERIALIZE_F32(y);
   YAC_SERIALIZE_F32(z);
}

sUI YAC_VCALL _Vector::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   
   x=YAC_DESERIALIZE_F32();
   y=YAC_DESERIALIZE_F32();
   z=YAC_DESERIALIZE_F32();
   return 1;
}

void _Vector::_unitScale(sF32 _s) {
   unit();
   scalef(_s);
}

void YAC_VCALL _Vector::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Vector))
   {
      _Vector *o=(_Vector*)_o;
      switch(_cmd)
      {
      case YAC_OP_CEQ:
         YAC_RETI( (x==o->x) && (y==o->y) && (z==o->z) );
         break;
      case YAC_OP_CNE:
         YAC_RETI( (x!=o->x) || (y!=o->y) || (z!=o->z) );
         break;
      case YAC_OP_ASSIGN:
         x=o->x;
         y=o->y;
         z=o->z;
         break;
      case YAC_OP_ADD:
         x+=o->x;
         y+=o->y;
         z+=o->z;
         break;
      case YAC_OP_NEG:
         x=-x;
         y=-y;
         z=-z;
         break;
      case YAC_OP_SUB:
         x-=o->x;
         y-=o->y;
         z-=o->z;
         break;
      case YAC_OP_MUL:
         x*=o->x;
         y*=o->y;
         z*=o->z;
         break;
      case YAC_OP_DIV:
         {
            sFVector t;
            t.init(*this);
            Cross(t, *o);
         }
         break;
      case YAC_OP_MOD:
         {
            sFMatrix m;
            m.init(o->x, o->y, o->z);
            Rotate(&m);
         }
         break;
      }
   }
   else if(_cmd==YAC_OP_ASSIGN)
   {
      if(YAC_VALID(_o))
      {
         sUI ne = _o->yacArrayGetNumElements();
         for(sUI i=0; i<ne; i++)
         {
            YAC_Value r;
            _o->yacArrayGet(NULL, i, &r);
            sF32 v;
            switch(r.type)
            {
            case YAC_TYPE_VOID:
               v = 0.0f;
               break;
            case YAC_TYPE_INT:
               v = (sF32)r.value.int_val;
               break;
            case YAC_TYPE_FLOAT:
               v = r.value.float_val;
               break;
            default:
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&v);
               }
               else
               {
                  v = 0.0f;
               }
               break;
            }
            switch(i)
            {
            case 0:
               x = v;
               break;
            case 1:
               y = v;
               break;
            case 2:
               z = v;
               break;
            }
            r.unset();
         }
      }
   }
   else YAC_Object::yacOperator(_cmd, _o, _r);
}

sUI _Vector::yacArrayGetNumElements(void) {
   return 3;
}

sUI _Vector::yacArrayGetMaxElements(void) {
   return 3;
}

sUI _Vector::yacArrayGetElementType (void) {
   return YAC_TYPE_FLOAT;
}

sUI _Vector::yacArrayGetElementByteSize (void) {
   return sizeof(sF32);
}

void _Vector::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_value) {
   if(_idx<3)
   {
      sF32 v;
      switch(_value->type)
      {
      case YAC_TYPE_VOID:
         v = 0.0f;
         break;
      case YAC_TYPE_INT:
         v = (sF32) _value->value.int_val;
         break;
      case YAC_TYPE_FLOAT:
         v = _value->value.float_val;
         break;
      default:
      case YAC_TYPE_OBJECT:
         if(YAC_VALID(_value->value.object_val))
         {
            if(!_value->value.object_val->yacScanF32(&v))
            {
               v = 0.0f;
            }
         }
         else
         {
            v = 0.0f;
         }
         break;
      }
      switch(_idx)
      {
      case 0:
         x = v;
         break;
      case 1:
         y = v;
         break;
      case 2:
         z = v;
         break;
      }

   }
}

void _Vector::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_r) {
   switch(_idx)
   {
   case 0:
      _r->initFloat(x);
      break;
   case 1:
      _r->initFloat(y);
      break;
   case 2:
      _r->initFloat(z);
      break;
   default:
      _r->initFloat(-9999999.0f);
      break;
   }
}

void _Vector::_initv(YAC_Object*_o) {
   IFYAC_CHK(_o, clid_Vector)
   {
      _Vector *o=(_Vector*)_o;
      x=o->x;
      y=o->y;
      z=o->z;
   }
}

sF32 _Vector::_dot(YAC_Object *_o) {
   IFYAC_CHK(_o, clid_Vector)
   {
      return (Dot(*((_Vector*)_o)));
   }
   return 0.0f;
}

void _Vector::_addScale(YAC_Object *_o, sF32 _v) {
   IFYAC_CHK(_o, clid_Vector)
   {
      AddScale(*((_Vector*)_o), _v);
   }
}

sF32 _Vector::_getAbs(void) {
   return Abs();
}

sF32 _Vector::_getAbsMax(void) {
   return AbsMax();
}

sF32 _Vector::_getAbsAdd(void) {
   return AbsAdd();
}

void _Vector::_cross(YAC_Object *_o1, YAC_Object *_o2) {
   IFYAC_CHK(_o1, clid_Vector)
      IFYAC_CHK(_o2, clid_Vector)
   {
      _Vector *o1=(_Vector*)_o1;
      _Vector *o2=(_Vector*)_o2;
      Cross(*o1, *o2);
   }
}

void _Vector::_rotate(YAC_Object*_o) {
   if(YAC_CHK(_o, clid_Matrix))
   {
      Rotate((_Matrix*)_o);
   }
   else
   {
      IFYAC_CHK(_o, clid_Vector)
      {
         _Vector *v=(_Vector*)_o;
         sFMatrix tm;
         tm.init(v->x, v->y, v->z);
         Rotate(&tm);
      }
   }
}

void _Vector::_rotateInv(YAC_Object*_o) {
   if(YAC_CHK(_o, clid_Matrix))
   {
      RotateInv((_Matrix*)_o);
   }
   else
   {
      IFYAC_CHK(_o, clid_Vector)
      {
         _Vector *v=(_Vector*)_o;
         sFMatrix tm;
         tm.init(v->x, v->y, v->z);
         RotateInv(&tm);
      }
   }
}

void _Vector::_rotateAdd(YAC_Object*_r, YAC_Object*_t) {
   _rotate(_r);
   _add(_t);
}

void _Vector::_rotateInvAdd(YAC_Object*_r, YAC_Object*_t) {
   _rotateInv(_r);
   _add(_t);
}

void _Vector::_sphere(sF32 _x, sF32 _y, sF32 _z, sF32 _s) {
   sFMatrix m;
   m.init(_x, _y, _z);
   init(_s, 0.0f, 0.0f);
   Rotate(&m);
}

sF32 _Vector::_getX(void) {
   return x;
}

sF32 _Vector::_getY(void) {
   return y;
}

sF32 _Vector::_getZ(void) {
   return z;
}

void _Vector::_setX(sF32 _v) {
   x=_v;
}

void _Vector::_setY(sF32 _v) {
   y=_v;
}

void _Vector::_setZ(sF32 _v) {
   z=_v;
}

void _Vector::_print(void) {
   static char buf[256];
   Dyac_snprintf(buf, 256, "Vector: (%g, %g, %g)\n", x, y, z);
   yac_host->yacPrint(buf);
}

void _Vector::_add(YAC_Object *_v) {
   IFYAC_CHK(_v, clid_Vector)
   {
      _Vector *o=(_Vector *)_v;
      x+=o->x;
      y+=o->y;
      z+=o->z;
   }
}

void _Vector::_sub(YAC_Object *_v) {
   IFYAC_CHK(_v, clid_Vector)
   {
      _Vector *o=(_Vector*)_v;
      x-=o->x;
      y-=o->y;
      z-=o->z;
   }
}

void _Vector::_initScale(YAC_Object *_v, sF32 _scl) {
   IFYAC_CHK(_v, clid_Vector)
   {
      _Vector *v=(_Vector*)_v;
      x=v->x*_scl;
      y=v->y*_scl;
      z=v->z*_scl;
   }
}

void _Vector::_scalev(YAC_Object *_v) {
   IFYAC_CHK(_v, clid_Vector)
   {
      _Vector *v=(_Vector*)_v;
      x*=v->x;
      y*=v->y;
      z*=v->z;
   }
}

void _Vector::_blend(YAC_Object *_v, sF32 _amount) {
   IFYAC_CHK(_v, clid_Vector)
   {
      _Vector *v=(_Vector*)_v;
      x+=(v->x-x)*_amount;
      y+=(v->y-y)*_amount;
      z+=(v->z-z)*_amount;
   }
}


void _Vector::_getString(YAC_Value *_r) {
   YAC_String *s=YAC_New_String();
   (void)s->yacArrayAlloc(1024, 0,0,0);
   s->printf("(%g, %g, %g)", x, y, z);
   YAC_RETS(s, 1);
}

void YAC_VCALL _Vector::yacOperatorClamp(YAC_Object*_min,YAC_Object*_max) {
   IFYAC_CHK(_min, clid_Vector)
      IFYAC_CHK(_max, clid_Vector)
   {
      _Vector*min=(_Vector*)_min;
      _Vector*max=(_Vector*)_max;
      if(x<min->x) x=min->x; else if(x>max->x) x=max->x;
      if(y<min->y) y=min->y; else if(y>max->y) y=max->y;
      if(z<min->z) z=min->z; else if(z>max->z) z=max->z;
   }
}

void YAC_VCALL _Vector::yacOperatorWrap(YAC_Object*_min,YAC_Object*_max) {
   IFYAC_CHK(_min, clid_Vector)
      IFYAC_CHK(_max, clid_Vector)
   {
      _Vector*min=(_Vector*)_min;
      _Vector*max=(_Vector*)_max;
      if(x<min->x) x+=(max->x-min->x); else if(x>max->x) x-=(max->x-min->x);
      if(y<min->y) y+=(max->y-min->y); else if(y>max->y) y-=(max->y-min->y);
      if(z<min->z) z+=(max->z-min->z); else if(z>max->z) z-=(max->z-min->z);
   }
}

void _Vector::_glVertex(void) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glVertex3f(x, y, z);
}

void _Vector::_glNormal(void) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glNormal3f(x, y, z);
}

void _Vector::_glScale(void) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glScalef(x, y, z);
}

void _Vector::_glColor(void) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glColor3f(x,y,z);
}

void _Vector::_glColorGamma(sF32 _g) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glColor3f(x*_g, y*_g, z*_g);
}

void _Vector::_faGet(YAC_Object *_fa, sSI _idx) {
   if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
   {
      YAC_FloatArray *fa=(YAC_FloatArray*)_fa;
      if(fa->elements)
      {
         if( ((sUI)_idx) < fa->max_elements)
            if( ((sUI)(_idx+3)) <= fa->max_elements)
            {
               x=fa->elements[_idx];
               y=fa->elements[_idx+1];
               z=fa->elements[_idx+2];
            }
      }
   }
}

void _Vector::_faSet(YAC_Object *_fa, sSI _idx) {
   if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
   {
      YAC_FloatArray *fa=(YAC_FloatArray*)_fa;
      if(fa->elements)
      {
         if( ((sUI)_idx) < fa->max_elements)
            if( ((sUI)(_idx+3)) <= fa->max_elements)
            {
               fa->elements[_idx]=x;
               fa->elements[_idx+1]=y;
               fa->elements[_idx+2]=z;
            }
      }
   }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ Vector4                                                        ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_Vector4::_Vector4(void) {
   x=y=z=w=0.0f;
}

_Vector4::~_Vector4() { }

void YAC_VCALL _Vector4::yacSerialize(YAC_Object*_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   
   YAC_SERIALIZE_F32(x);
   YAC_SERIALIZE_F32(y);
   YAC_SERIALIZE_F32(z);
   YAC_SERIALIZE_F32(w);
}

sUI YAC_VCALL _Vector4::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   
   x=YAC_DESERIALIZE_F32();
   y=YAC_DESERIALIZE_F32();
   z=YAC_DESERIALIZE_F32();
   w=YAC_DESERIALIZE_F32();
   return 1;
}

void YAC_VCALL _Vector4::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Vector4))
   {
      _Vector4 *o=(_Vector4*)_o;
      switch(_cmd)
      {
      case YAC_OP_ASSIGN:
         x=o->x;
         y=o->y;
         z=o->z;
         w=o->w;
         break;
      case YAC_OP_ADD:
         x+=o->x;
         y+=o->y;
         z+=o->z;
         w+=o->w;
         break;
      case YAC_OP_SUB:
         x-=o->x;
         y-=o->y;
         z-=o->z;
         w-=o->w;
         break;
      }
   }
   else
   {
      if(YAC_CHK(_o, clid_Vector))
      {
         _Vector *o=(_Vector*)_o;
         switch(_cmd)
         {
         case YAC_OP_ASSIGN:
            x=o->x;
            y=o->y;
            z=o->z;
            break;
         case YAC_OP_ADD:
            x+=o->x;
            y+=o->y;
            z+=o->z;
            break;
         case YAC_OP_SUB:
            x-=o->x;
            y-=o->y;
            z-=o->z;
            break;
         }
      }
      else if(_cmd==YAC_OP_ASSIGN)
      {
         if(YAC_VALID(_o))
         {
            sUI ne = _o->yacArrayGetNumElements();
            for(sUI i=0; i<ne; i++)
            {
               YAC_Value r;
               _o->yacArrayGet(NULL, i, &r);
               sF32 v;
               switch(r.type)
               {
               case YAC_TYPE_VOID:
                  v = 0.0f;
                  break;
               case YAC_TYPE_INT:
                  v = (sF32)r.value.int_val;
                  break;
               case YAC_TYPE_FLOAT:
                  v = r.value.float_val;
                  break;
               default:
                  if(YAC_VALID(r.value.object_val))
                  {
                     r.value.object_val->yacScanF32(&v);
                  }
                  else
                  {
                     v = 0.0f;
                  }
                  break;
               }
               switch(i)
               {
               case 0:
                  x = v;
                  break;
               case 1:
                  y = v;
                  break;
               case 2:
                  z = v;
                  break;
               case 3:
                  w = v;
                  break;
               }
               r.unset();
            }
         }
      }
      else
         YAC_Object::yacOperator(_cmd, _o, _r);
   }
}

void _Vector4::_setX(sF32 _x) {
   x=_x;
}

void _Vector4::_setY(sF32 _y) {
   y=_y;
}

void _Vector4::_setZ(sF32 _z) {
   z=_z;
}

void _Vector4::_setW(sF32 _w) {
   w=_w;
}

sF32 _Vector4::_getX(void) {
   return x;
}

sF32 _Vector4::_getY(void) {
   return y;
}

sF32 _Vector4::_getZ(void) {
   return z;
}

sF32 _Vector4::_getW(void) {
   return w;
}

void _Vector4::_init(sF32 _x, sF32 _y, sF32 _z, sF32 _w) {
   x=_x;
   y=_y;
   z=_z;
   w=_w;
}

void _Vector4::_getString(YAC_Value *_r) {
   YAC_String *s=YAC_New_String();
   (void)s->yacArrayAlloc(256, 0,0,0);
   s->printf("(%g, %g, %g, %g)", x, y, z, w);
   YAC_RETS(s, 1);
}

void YAC_VCALL _Vector4::yacOperatorClamp(YAC_Object*_min, YAC_Object*_max) {
   IFYAC_CHK(_min, clid_Vector4)
      IFYAC_CHK(_max, clid_Vector4)
   {
      _Vector4*min=(_Vector4*)_min;
      _Vector4*max=(_Vector4*)_max;
      if(x<min->x) x=min->x; else if(x>max->x) x=max->x;
      if(y<min->y) y=min->y; else if(y>max->y) y=max->y;
      if(z<min->z) z=min->z; else if(z>max->z) z=max->z;
      if(w<min->w) w=min->w; else if(w>max->w) w=max->w;
   }
}

void YAC_VCALL _Vector4::yacOperatorWrap(YAC_Object*_min, YAC_Object*_max) {
   IFYAC_CHK(_min, clid_Vector4)
      IFYAC_CHK(_max, clid_Vector4)
   {
      _Vector4*min=(_Vector4*)_min;
      _Vector4*max=(_Vector4*)_max;
      if(x<min->x) x+=(max->x-min->x); else if(x>max->x) x-=(max->x-min->x);
      if(y<min->y) y+=(max->y-min->y); else if(y>max->y) y-=(max->y-min->y);
      if(z<min->z) z+=(max->z-min->z); else if(z>max->z) z-=(max->z-min->z);
      if(w<min->w) w+=(max->w-min->w); else if(w>max->w) w-=(max->w-min->w);
   }
}

sF32 _Vector4::_interpolCubic2D(sF32 _x, sF32 _y) {
   return (x*(1.0f-_x)*(1.0f-_y) + y*(_x)*(1.0f-_y) + z*(_x)*(_y) + w*(1.0f-_x)*_y);
}

void _Vector4::_glVertex(void) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glVertex4f(x, y, z, w);
}

void _Vector4::_glColor(void) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glColor4f(x,y,z,w);
}

void _Vector4::_glColorGamma(sF32 _g) {
   tkopengl_shared_resolve();
   tkopengl_shared->_glColor4f(x*_g, y*_g, z*_g, w);
}

void _Vector4::_faGet(YAC_Object *_fa, sSI _idx) {
   if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
   {
      YAC_FloatArray *fa=(YAC_FloatArray*)_fa;
      if(fa->elements)
      {
         if( ((sUI)_idx) < fa->max_elements)
            if( ((sUI)(_idx+4)) <= fa->max_elements)
            {
               x=fa->elements[_idx];
               y=fa->elements[_idx+1];
               z=fa->elements[_idx+2];
               w=fa->elements[_idx+3];
            }
      }
   }
}

void _Vector4::_faSet(YAC_Object *_fa, sSI _idx) {
   if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
   {
      YAC_FloatArray *fa=(YAC_FloatArray*)_fa;
      if(fa->elements)
      {
         if( ((sUI)_idx) < fa->max_elements)
            if( ((sUI)(_idx+4)) <= fa->max_elements)
            {
               fa->elements[_idx]=x;
               fa->elements[_idx+1]=y;
               fa->elements[_idx+2]=z;
               fa->elements[_idx+3]=w;
            }
      }
   }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ Matrix                                                         ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_Matrix::_Matrix(void) {
   loadIdentity(); 
}

_Matrix::~_Matrix(void) { }

void YAC_VCALL _Matrix::yacSerialize(YAC_Object*_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   
   YAC_SERIALIZE_F32(i.x);
   YAC_SERIALIZE_F32(i.y);
   YAC_SERIALIZE_F32(i.z);
   YAC_SERIALIZE_F32(j.x);
   YAC_SERIALIZE_F32(j.y);
   YAC_SERIALIZE_F32(j.z);
   YAC_SERIALIZE_F32(k.x);
   YAC_SERIALIZE_F32(k.y);
   YAC_SERIALIZE_F32(k.z);
}

sUI YAC_VCALL _Matrix::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   
   i.x=YAC_DESERIALIZE_F32();
   i.y=YAC_DESERIALIZE_F32();
   i.z=YAC_DESERIALIZE_F32();
   j.x=YAC_DESERIALIZE_F32();
   j.y=YAC_DESERIALIZE_F32();
   j.z=YAC_DESERIALIZE_F32();
   k.x=YAC_DESERIALIZE_F32();
   k.y=YAC_DESERIALIZE_F32();
   k.z=YAC_DESERIALIZE_F32();
   return 1;
}

void YAC_VCALL _Matrix::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix))
   {
      _Matrix *o=(_Matrix*)_o;
      switch(_cmd)
      {
      case YAC_OP_ASSIGN:
         i.init(o->i);
         j.init(o->j);
         k.init(o->k);
         break;
      case YAC_OP_ADD:
         Add(*o);
         break;
      case YAC_OP_SUB:
         Sub(*o);
         break;
      case YAC_OP_MUL:
         {
            sFMatrix m;
            m.init(this);
            if(o==this)
            {
               Mul(m, m);
            }
            else
            {
               Mul(m, *o);
            }
         }
         break;
      case YAC_OP_NEG:
         i.x=-i.x;
         i.y=-i.y;
         i.z=-i.z;
         j.x=-j.x;
         j.y=-j.y;
         j.z=-j.z;
         k.x=-k.x;
         k.y=-k.y;
         k.z=-k.z;
         break;
      }
   }
   else if(_cmd==YAC_OP_ASSIGN)
   {
      if(YAC_VALID(_o))
      {
         sUI ne = _o->yacArrayGetNumElements();
         for(sUI i=0; i<ne; i++)
         {
            YAC_Value r;
            _o->yacArrayGet(NULL, i, &r);
            sF32 v;
            switch(r.type)
            {
            case YAC_TYPE_VOID:
               v = 0.0f;
               break;
            case YAC_TYPE_INT:
               v = (sF32)r.value.int_val;
               break;
            case YAC_TYPE_FLOAT:
               v = r.value.float_val;
               break;
            default:
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&v);
               }
               else
               {
                  v = 0.0f;
               }
               break;
            }
            switch(i)
            {
            case 0:
               (_Matrix::i).x = v;
               break;
            case 1:
               (_Matrix::i).y = v;
               break;
            case 2:
               (_Matrix::i).z = v;
               break;
            case 3:
               j.x = v;
               break;
            case 4:
               j.y = v;
               break;
            case 5:
               j.z = v;
               break;
            case 6:
               k.x = v;
               break;
            case 7:
               k.y = v;
               break;
            case 8:
               k.z = v;
               break;
            }
            r.unset();
         }
      }
   }
   else
   {
      YAC_Object::yacOperator(_cmd, _o, _r);
   }
}

void _Matrix::_add(YAC_Object*_o) {
   IFYAC_CHK(_o, clid_Matrix)
   {
      Add(*((_Matrix*)_o));
   }
}

void _Matrix::_mul(YAC_Object*_o1, YAC_Object*_o2) {
   IFYAC_CHK(_o1, clid_Matrix)
      IFYAC_CHK(_o2, clid_Matrix)
   {
      _Matrix *o1=(_Matrix*)_o1;
      _Matrix *o2=(_Matrix*)_o2;
      if(o1==this)
      {
         sFMatrix self;
         self.init(this);
         if(o2==this)
            Mul(self, self);
         else
            Mul(self, *o2);
      }
      else
      {
         if(o2==this)
         {
            sFMatrix self;
            self.init(this);
            Mul(*o1, self);
         }
         else
            Mul(*o1,*o2);
      }
   }
}

void _Matrix::_mulInv(YAC_Object*_o1, YAC_Object*_o2) {
   IFYAC_CHK(_o1, clid_Matrix)
      IFYAC_CHK(_o2, clid_Matrix)
   {
      _Matrix *o1=(_Matrix*)_o1;
      _Matrix *o2=(_Matrix*)_o2;
      if(o1==this)
      {
         sFMatrix self;
         self.init(this);
         if(o2==this)
            MulInv(self, self);
         else
            MulInv(self, *o2);
      }
      else
      {
         if(o2==this)
         {
            sFMatrix self;
            self.init(this);
            MulInv(*o1, self);
         }
         else
            MulInv(*o1,*o2);
      }
   }
}

void _Matrix::_rotate(YAC_Object *_o) {
   static sFMatrix self;
   if(YAC_CHK(_o, clid_Vector))
   {
      self.init(this);
      sFMatrix tm;
      _Vector *v=(_Vector*)_o;
      tm.init(v->x, v->y, v->z);
      Mul(self, tm);
   }
   else
   {
      IFYAC_CHK(_o, clid_Matrix)
      {
         _Matrix *m=(_Matrix*)_o;
         self.init(this);
         Mul(self, *m);
      }
   }
}

void _Matrix::_rotateInv(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Vector))
   {
      sFMatrix self;
      self.init(this);
      sFMatrix tm;
      _Vector *v=(_Vector*)_o;
      tm.init(v->x, v->y, v->z);
      MulInv(self, tm);
   }
   else
   {
      IFYAC_CHK(_o, clid_Matrix)
      {
         _Matrix *m=(_Matrix*)_o;
         sFMatrix self;
         self.init(this);
         MulInv(self, *m);
      }
   }
}

void _Matrix::_sub(YAC_Object*_o) {
   IFYAC_CHK(_o, clid_Matrix)
      Sub(*((_Matrix*)_o));
}

void _Matrix::_getI(YAC_Value *_r) {
   _Vector *r=(_Vector *)yac_host->yacNewByID(clid_Vector);
   r->init(i);
   YAC_RETO(r,1);
}

void _Matrix::_getJ(YAC_Value *_r) {
   _Vector *r=(_Vector *)yac_host->yacNewByID(clid_Vector);
   r->init(j);
   YAC_RETO(r,1);
}

void _Matrix::_getK(YAC_Value *_r) {
   _Vector *r=(_Vector *)yac_host->yacNewByID(clid_Vector);
   r->init(k);
   YAC_RETO(r,1);
} 

void _Matrix::_setI(YAC_Object*_o) {
   IFYAC_CHK(_o, clid_Vector)
      i.init(*((_Vector *)_o));
}

void _Matrix::_setJ(YAC_Object*_o) {
   IFYAC_CHK(_o, clid_Vector)
      j.init(*((_Vector *)_o));
}

void _Matrix::_setK(YAC_Object*_o) {
   IFYAC_CHK(_o, clid_Vector)
      k.init(*((_Vector *)_o));
}

void _Matrix::_print(void) {
   static char buf[384];
   Dyac_snprintf(buf, 384,
                 "Matrix: i=(%g, %g, %g)\n"
                 "         j=(%g, %g, %g)\n"
                 "         k=(%g, %g, %g)\n",
                 i.x, i.y, i.z,
                 j.x, j.y, j.z,
                 k.x, k.y, k.z
                 );
   yac_host->yacPrint(buf);
}

void _Matrix::_initv(YAC_Object *_o) {
   IFYAC_CHK(_o, clid_Vector)
   {
      _Vector*o=(_Vector*)_o;
      init(o->x, o->y, o->z);
   }
}

void _Matrix::_initIJK(YAC_Object *_i, YAC_Object *_j, YAC_Object *_k) {
   IFYAC_CHK(_i, clid_Vector)
      IFYAC_CHK(_j, clid_Vector)
      IFYAC_CHK(_k, clid_Vector)
   {
      _Vector*ri=(_Vector*)_i;
      _Vector*rj=(_Vector*)_j;
      _Vector*rk=(_Vector*)_k;
      i.x=ri->x; i.y=ri->y; i.z=ri->z;
      j.x=rj->x; j.y=rj->y; j.z=rj->z;
      k.x=rk->x; k.y=rk->y; k.z=rk->z;
   }
}

void _Matrix::_initIJKInv(YAC_Object *_i, YAC_Object *_j, YAC_Object *_k) {
   IFYAC_CHK(_i, clid_Vector)
      IFYAC_CHK(_j, clid_Vector)
      IFYAC_CHK(_k, clid_Vector)
   {
      _Vector*ri=(_Vector*)_i;
      _Vector*rj=(_Vector*)_j;
      _Vector*rk=(_Vector*)_k;
      i.x=ri->x; i.y=rj->x; i.z=rk->x;
      j.x=ri->y; j.y=rj->y; j.z=rk->y;
      k.x=ri->z; k.y=rj->z; k.z=rk->z;
   }
}

void _Matrix::_getString(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   (void)s->yacArrayAlloc(384, 0,0,0);
   s->printf(
      "((%g, %g, %g),"
      "(%g, %g, %g),"
      "(%g, %g, %g))",
      i.x, i.y, i.z,
      j.x, j.y, j.z,
      k.x, k.y, k.z
      );
   YAC_RETS(s, 1);
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ _VectorArray                                                   ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_VectorArrayIterator::_VectorArrayIterator(const _VectorArray *_a) {
   array=_a;
   current_index=0;
}

_VectorArrayIterator::~_VectorArrayIterator() {
   array=0;
   current_index=0;
}

void _VectorArrayIterator::getNext(YAC_Value *_r) {
   if(current_index<array->num_elements)
      _r->initObject(&array->elements[current_index++], 0);
   else
      _r->initInt(0xFF);
}




_VectorArray::_VectorArray(void) {
   max_elements=0;
   num_elements=0;
   elements=0;
}

_VectorArray::~_VectorArray() {
   _free();
}

void YAC_VCALL _VectorArray::yacSerialize(YAC_Object*_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   
   YAC_SERIALIZE_I32(num_elements);
   sU32 i;
   for(i=0; i<num_elements; i++)
   {
      elements[i].yacSerialize(_ofs, 0); // do not write class name
   }
}

sUI YAC_VCALL _VectorArray::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   
   _free();
   
   sU32 ne=YAC_DESERIALIZE_I32();
   if(((sU32)ne)<999999) //yac_host->configuration.streamMaxArraySize
   {
      sU32 i;
      if(_alloc(ne))
      {
         for(i=0; i<ne; i++)
            elements[i].yacDeserialize(_ifs, 0);
         num_elements=ne;
      }
      else
      {
         _Vector t;
         /// ---- out of memory, skip stream ----
         for(i=0; i<ne; i++)
            t.yacDeserialize(_ifs, 0);
      }
      return 1;
   }
   else
   {
      yac_host->printf("[---] VectorArray::deserialize: insane array size (%i>%i)\n",
         ne, 999999/*yac_host->configuration.streamMaxArraySize*/);
   }
   return 0;
}

sBool YAC_VCALL _VectorArray::yacIteratorInit(YAC_Iterator *_it) const {
   _VectorArrayIterator *it = (_VectorArrayIterator*)_it;
   ::new (it) _VectorArrayIterator(this);
   return 1;
}

void YAC_VCALL _VectorArray::yacArrayCopySize(YAC_Object *_array) {
   _VectorArray *o=(_VectorArray*)_array;
   _alloc(o->max_elements);
}

void YAC_VCALL _VectorArray::yacArraySet(void *_context, sUI _index, YAC_Value *_value) {
   if(((sU32)_index)<max_elements)
   {
      if(((sU32)_index)>=num_elements) // track highest access point
         num_elements=_index+1;
      if(_value->type==3)
      {
         _Vector *v=(_Vector*)_value->value.object_val;
         IFYAC_CHK(v, clid_Vector)
         {
            elements[_index].init(*v);
         }
      }
   }
   else
   {
      yac_host->printf("\n[!!!] _VectorArray::yacArraySet: index >>%i<< out of bounds (max=%i).\n", _index, max_elements);
      // xxx TKS_MT: raise exception
   }
}

void YAC_VCALL _VectorArray::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   if(((sU32)_index)<max_elements)
   {
      _r->initObject(&elements[_index], 0);
   }
   else
   {
      yac_host->printf("\n[!!!] _VectorArray::yacArrayGet: index >>%i<< out of bounds (max=%i).\n", _index, max_elements);
      _r->initVoid();
      // xxx TKS_MT: raise exception
   }
}

sSI _VectorArray::_alloc(sSI _maxelements) {
   if(_maxelements)
   {
      if(elements) 
      { 
         delete [] elements; 
         elements = NULL;
         max_elements=0;
         num_elements=0;
      }
      if(_maxelements)
      {
         elements=new _Vector [ _maxelements ];
         if(elements)
         {
            max_elements=_maxelements;
            num_elements=0;  
            sUI i;
            for(i=0; i<max_elements; i++) 
            {
               elements[i].class_ID=clid_Vector;
            }
            return 1;
         }
      }
      else
      {
         num_elements=0;
         max_elements=0;
      }
      return 0;
   }
   else
   {
      _free();
      return 1;
   }
}

sBool YAC_VCALL _VectorArray::yacArrayAlloc(sUI _num, sUI,sUI,sUI) {
   return _alloc((sSI)_num);
}

sSI _VectorArray::_realloc(sSI _maxelements) {
   if(_maxelements)
   {
      _Vector *ne=new _Vector[_maxelements];
      if(ne)
      {
         sU32 i=0;
         if(((sU32)_maxelements)<=max_elements)
         {
            for(; i<((sU32)_maxelements); i++) 
            {
               ne[i].class_ID=clid_Vector;
               ne[i].init(elements[i]);
            }
         }
         else
         {
            for(; i<((sU32)max_elements); i++) 
            {
               ne[i].class_ID=clid_Vector;
               ne[i].init(elements[i]);
            }
            for(; i<((sU32)_maxelements); i++) 
            {
               ne[i].class_ID=clid_Vector;
            }
         }
         delete [] elements;
         elements=ne;
         max_elements=_maxelements;
         if(num_elements>max_elements) num_elements=max_elements;
         return 1;
      }
      else
      {
         yac_host->printf("\n[!!!] failed to realloc VectorArray to %i\n", _maxelements);
         // xxx
         return 0;
      }
   }
   else
   {
      _empty();
      return 1;
   }
}

void YAC_VCALL _VectorArray::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o!=((YAC_Object*)this))
   {
      if(_cmd==YAC_OP_ASSIGN)
      {
         if(YAC_CHK(_o, clid_VectorArray))
         {
            _VectorArray*o=(_VectorArray*)_o;
            if( ((void*)o) != ((void*)this) )
            {
               _realloc(o->num_elements);
               sU32 i;
               num_elements = o->num_elements;
               for(i=0; i<num_elements; i++)
               {
                  ///YAC_Value opr;
                  //elements[i].yacOperator(YAC_OP_ASSIGN, &o->elements[i], &opr);
                  ///opr.unsetFast();
                  elements[i].x = o->elements[i].x;
                  elements[i].y = o->elements[i].y;
                  elements[i].z = o->elements[i].z;
               }
            }
         }
      }
      else YAC_Object::yacOperator(_cmd, _o, _r);
   }
}

void _VectorArray::_free(void) {
   //Array<_Vector>::free();
   if(elements)
   {
      sU32 i;
      for(i=0; i<num_elements; i++)
      {
#ifdef YAC_OBJECT_TAGS
         elements[i].validation_tag = YAC_INVALID_TAG;
#endif
      }
      delete [] elements;
      elements     = NULL;
      num_elements = 0;
      max_elements = 0;
   }
}

sUI _VectorArray::_getMaxElements(void) {
   return max_elements;
}

sUI _VectorArray::_getNumElements(void) {
   return num_elements;
}

void _VectorArray::_setNumElements(sUI _num) {
   if(((sU32)_num)<max_elements)
      num_elements=_num;
}

sSI _VectorArray::_add(YAC_Object *_val) {
   if(YAC_CHK(_val, clid_Vector))
   {
      _Vector *val=(_Vector*)_val;
      //YAC_RETI(Array<_Vector>::add(*val));
      if(num_elements<max_elements &&_val)
      {
         elements[num_elements].x=val->x;
         elements[num_elements].y=val->y;
         elements[num_elements++].z=val->z;
         return 1;
      }
   }
   return 0;
}

sSI _VectorArray::_addEmpty(sUI _num) {
   if(num_elements+((sUI)_num)<= max_elements)
   {
      num_elements+=((sUI)_num);
      return num_elements;
   }
   return 0;
}

void _VectorArray::_empty(void) {
   num_elements = 0;
}

sSI _VectorArray::_insert(sSI _index, YAC_Object *_val) {
   /// xxx
   if(YAC_CHK(_val, clid_Vector))
   {
      _Vector *v=(_Vector *)_val;
      if(((sUI)_index)<num_elements)
      {
         num_elements++;
         if((((sUI)_index)+1)<max_elements)
         {
            sUI i;
            for(i=num_elements-1; i>((sUI)_index); i--)	elements[i]=elements[i-1];
         }
         elements[_index].x=v->x;
         elements[_index].y=v->y;
         elements[_index].z=v->z;
         return 1;
      }
   }
   return 0;
}

sSI _VectorArray::_delete(sSI _index) {
   /// xxx
   if(((sUI)_index)<num_elements)
   {
      if(num_elements>1)
      {
         sU32 i;
         for(i=((sUI)_index); i<(num_elements-1); i++)
            elements[i]=elements[i+1];
      }
      num_elements--;
      return 1;
   }
   return 0;
}

void _VectorArray::_zero(void) {
   if(elements&&(num_elements>1))
   {
      sUI i;
      for(i=0; i<max_elements; i++)
         elements[i].init(0.0f, 0.0f, 0.0f);
   }
}

void _VectorArray::_reverse(void) {
   //Array<_Vector>::reverse();
   if(num_elements&&elements)
   {
      _Vector *s=elements;
      sSI l=(num_elements)>>1;
      _Vector *d=&elements[num_elements-1];
      for(; l>=0; l--)
      {
         _Vector t;
         t=*s;
         *s++=*d;
         *d--=t;
      }
   }
}

void _VectorArray::_print(void) {
   if(elements&&max_elements)
   {
      static char buf[256];
      Dyac_snprintf(buf, 256,
                    "[...] _VectorArray::print: num_elements=%i max_elements=%i.\n",
                    num_elements, max_elements
                    );
      yac_host->yacPrint(buf);
      sU32 i;
      for(i=0; i<num_elements; i++)
         elements[i]._print();
      yac_host->yacPrint("\n");
   }
   else
      yac_host->yacPrint("[...] _VectorArray::print: no elements.\n");
}

void _VectorArray::_rotate(YAC_Object *_s, YAC_Object *_m) {
   if(YAC_CHK(_m, clid_Matrix) && YAC_CHK(_s, clid_VectorArray))
   {
      _Matrix *m=(_Matrix *)_m;
      _VectorArray *va=(_VectorArray*)_s;
      if(num_elements>=va->num_elements)
      {
         sU32 i;
         for(i=0; i<va->num_elements; i++)
         {
            _Vector *s=&va->elements[i];
            _Vector *d=&elements[i];
            d->x = s->x*m->i.x + s->y*m->i.y + s->z*m->i.z;
            d->y = s->x*m->j.x + s->y*m->j.y + s->z*m->j.z;
            d->z = s->x*m->k.x + s->y*m->k.y + s->z*m->k.z;
         }
      }
      else
      {
         yac_host->printf("\n[!!!] VectorArray: cannot rotate %i elements (num=%i, max=%i).\n",
            va->num_elements, num_elements, max_elements);
      }
   }
}

void _VectorArray::_rotateInv(YAC_Object *_s, YAC_Object *_m) {
   if(YAC_CHK(_m, clid_Matrix) && YAC_CHK(_s, clid_VectorArray))
   {
      _Matrix *m=(_Matrix *)_m;
      _VectorArray *va=(_VectorArray*)_s;
      if(num_elements>=va->num_elements)
      {
         sU32 i;
         for(i=0; i<va->num_elements; i++)
         {
            _Vector *s=&va->elements[i];
            _Vector *d=&elements[i];
            d->x = s->x*m->i.x + s->y*m->j.x + s->z*m->k.x;
            d->y = s->x*m->i.y + s->y*m->j.y + s->z*m->k.y;
            d->z = s->x*m->i.z + s->y*m->j.z + s->z*m->k.z;
         }
      }
      else
      {
         yac_host->printf("\n[!!!] VectorArray: cannot rotateInv %i elements (num=%i, max=%i).\n",
            va->num_elements, num_elements, max_elements);
      }
   }
}

void _VectorArray::_translatev(YAC_Object *_s, YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector) && YAC_CHK(_s, clid_VectorArray))
   {
      _VectorArray *va=(_VectorArray*)_s;
      if(num_elements>=va->num_elements)
      {
         sU32 i;
         for(i=0; i<va->num_elements; i++)
         {
            _Vector *s=&va->elements[i];
            _Vector *d=&elements[i];
            d->x+=s->x;
            d->y+=s->y;
            d->z+=s->z;
         }
      }
      else
         yac_host->printf("\n[!!!] VectorArray: cannot translate %i elements (num=%i, max=%i).\n",
         va->num_elements, num_elements, max_elements);
   }
}


void _VectorArray::_scalev(YAC_Object *_s, YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector) && YAC_CHK(_s, clid_VectorArray))
   {
      _VectorArray *va=(_VectorArray*)_s;
      if(num_elements>=va->num_elements)
      {
         sU32 i;
         for(i=0; i<va->num_elements; i++)
         {
            _Vector *s=&va->elements[i];
            _Vector *d=&elements[i];
            d->x*=s->x;
            d->y*=s->y;
            d->z*=s->z;
         }
      }
      else
         yac_host->printf("\n[!!!] VectorArray: cannot scale %i elements (num=%i, max=%i).\n",
         va->num_elements, num_elements, max_elements);
   }
}

YAC_Object *_VectorArray::_getNextFree(void) {
   if(num_elements!=max_elements)
      return &elements[num_elements++];
   return 0;
}

sSI _VectorArray::reorderFrom (YAC_Object *_va, YAC_Object *_ia) {
   if(YAC_CHK(_va, clid_VectorArray))
   {
      if(YAC_BCHK(_ia, YAC_CLID_INTARRAY))
      {
         _VectorArray *va= (_VectorArray*)_va;
         YAC_IntArray *ia= (YAC_IntArray*)_ia;
         sUI me = va->num_elements;
         if(ia->num_elements == me)
         {
            if(_VectorArray::_realloc(me))
            {
               sUI i=0;
               for(i=0; i<me; i++)
               {
                  sUI idx = (sUI) ia->elements[i];
                  if(idx>=me)
                  {
                     yac_host->printf("[---] VectorArray::reorderFrom: source index exceeds array boundaries (%i>=%i)\n", idx, me);
                     return 0;
                  }
                  elements[i].x = va->elements[idx].x;
                  elements[i].y = va->elements[idx].y;
                  elements[i].z = va->elements[idx].z;
               }
            }
            else
            {
               yac_host->printf("[---] VectorArray::reorderFrom: failed to reallocate %i vertices\n", me);
            }
         }
         else
         {
            yac_host->printf("[---] VectorArray::reorderFrom: index array element count mismatch (%i != %i)\n",
               ia->num_elements, me);
         }
      }
   }
   return 0;
}

void _VectorArray::sortByX(YAC_Object *_ia) { 
   if(elements&&num_elements)  
   { 
      if(YAC_BCHK(_ia, YAC_CLID_INTARRAY)) 
      { 
         YAC_IntArray *ia=(YAC_IntArray*)_ia;  
         if(ia->max_elements>=num_elements)
         {
            ia->num_elements = num_elements; 
            for(sUI i=0; i<num_elements; i++)  
            { 
               ia->elements[i] = i;   // identity
            } 
            quicksortByX((sSI*)ia->elements, 0, num_elements-1); 
         } 
      } 
   } 
} 
 
void _VectorArray::sortByY(YAC_Object *_ia) { 
   if(elements&&num_elements)  
   { 
      if(YAC_BCHK(_ia, YAC_CLID_INTARRAY)) 
      { 
         YAC_IntArray *ia=(YAC_IntArray*)_ia;  
         if(ia->max_elements>=num_elements)
         {
            ia->num_elements = num_elements; 
            for(sUI i=0; i<num_elements; i++)  
            { 
               ia->elements[i] = i;   // identity
            } 
            quicksortByY((sSI*)ia->elements, 0, num_elements-1); 
         } 
      } 
   } 
} 

void _VectorArray::sortByZ(YAC_Object *_ia) { 
   if(elements&&num_elements)  
   { 
      if(YAC_BCHK(_ia, YAC_CLID_INTARRAY)) 
      { 
         YAC_IntArray *ia=(YAC_IntArray*)_ia;  
         if(ia->max_elements>=num_elements)
         {
            ia->num_elements = num_elements; 
            for(sUI i=0; i<num_elements; i++)  
            { 
               ia->elements[i] = i;   // identity
            } 
            quicksortByZ((sSI*)ia->elements, 0, num_elements-1); 
         } 
      } 
   } 
} 

void _VectorArray::quicksortByX(sSI *_ia, sSI _l, sSI _r) { 
   sF32 m = elements[ _ia[(_l + _r) >>1] ].x; 
   sSI i = _l, j = _r, ti; 
   while(i <= j) 
   {     
      while( elements[_ia[i]].x < m )  
      { 
         i++;  
      } 
      while( m < elements[_ia[j]].x )  
      { 
         j--; 
      } 
      if(i <= j) 
      { 
         ti       = _ia[j]; 
         _ia[j--] = _ia[i]; 
         _ia[i++] = ti; 
      } 
   } 
   if(_l < j) 
   { 
      quicksortByX(_ia, _l, j); 
   } 
   if(i < _r) 
   { 
      quicksortByX(_ia, i, _r); 
   } 
} 


void _VectorArray::quicksortByY(sSI *_ia, sSI _l, sSI _r) { 
   sF32 m = elements[ _ia[(_l + _r) >>1] ].y; 
   sSI i = _l, j = _r, ti; 
   while(i <= j) 
   {     
      while( elements[_ia[i]].y < m )  
      { 
         i++;  
      } 
      while( m < elements[_ia[j]].y )  
      { 
         j--; 
      } 
      if(i <= j) 
      { 
         ti       = _ia[j]; 
         _ia[j--] = _ia[i]; 
         _ia[i++] = ti; 
      } 
   } 
   if(_l < j) 
   { 
      quicksortByY(_ia, _l, j); 
   } 
   if(i < _r) 
   { 
      quicksortByY(_ia, i, _r); 
   } 
} 

void _VectorArray::quicksortByZ(sSI *_ia, sSI _l, sSI _r) { 
   sF32 m = elements[ _ia[(_l + _r) >>1] ].z; 
   sSI i = _l, j = _r, ti; 
   while(i <= j) 
   {     
      while( elements[_ia[i]].z < m )  
      { 
         i++;  
      } 
      while( m < elements[_ia[j]].z )  
      { 
         j--; 
      } 
      if(i <= j) 
      { 
         ti       = _ia[j]; 
         _ia[j--] = _ia[i]; 
         _ia[i++] = ti; 
      } 
   } 
   if(_l < j) 
   { 
      quicksortByZ(_ia, _l, j); 
   } 
   if(i < _r) 
   { 
      quicksortByZ(_ia, i, _r); 
   } 
} 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ IVector                                                        ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_IVector::_IVector(void) { }

_IVector::~_IVector() { }

void YAC_VCALL _IVector::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   yac_host->printf("ivector::serialize\n");
   YAC_BEG_SERIALIZE();
   
   YAC_SERIALIZE_I32(x);
   YAC_SERIALIZE_I32(y);
}

sUI YAC_VCALL _IVector::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   yac_host->printf("ivector::deserialize\n");
   YAC_BEG_DESERIALIZE();
   
   x=YAC_DESERIALIZE_I32();
   y=YAC_DESERIALIZE_I32();
   return 1;
}

void YAC_VCALL _IVector::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_IVector))
   {
      _IVector *o=(_IVector*)_o;
      switch(_cmd)
      {
      case YAC_OP_ASSIGN:
         x=o->x;
         y=o->y;
         break;
      case YAC_OP_ADD:
         x+=o->x;
         y+=o->y;
         break;
      case YAC_OP_NEG:
         x=-x;
         y=-y;
         break;
      case YAC_OP_SUB:
         x-=o->x;
         y-=o->y;
         break;
      case YAC_OP_MUL:
         x*=o->x;
         y*=o->y;
         break;
      }
   }
   else if(_cmd==YAC_OP_ASSIGN)
   {
      if(YAC_VALID(_o))
      {
         sUI ne = _o->yacArrayGetNumElements();
         for(sUI i=0; i<ne; i++)
         {
            YAC_Value r;
            _o->yacArrayGet(NULL, i, &r);
            sSI v;
            switch(r.type)
            {
            case YAC_TYPE_VOID:
               v = 0;
               break;
            case YAC_TYPE_INT:
               v = r.value.int_val;
               break;
            case YAC_TYPE_FLOAT:
               v = (sSI)r.value.float_val;
               break;
            default:
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanI(&v);
               }
               else
               {
                  v = 0;
               }
               break;
            }
            switch(i)
            {
            case 0:
               x = v;
               break;
            case 1:
               y = v;
               break;
            }
            r.unset();
         }
      }
   }
   else YAC_Object::yacOperator(_cmd, _o, _r);
}

void _IVector::_neg(void) {
   x=-x;
   y=-y;
}

void _IVector::_init(sSI _x, sSI _y) {
   x=_x;
   y=_y;
}

void _IVector::_scale(sF32 _v) {
   x*=(sSI)(x*_v);
   y*=(sSI)(y*_v);
}

void _IVector::_scaleXY(sF32 _vx, sF32 _vy) {
   x*=(sSI)(x*_vx);
   y*=(sSI)(y*_vy);
}

void _IVector::_addScale(YAC_Object *_o, sF32 _v) {
   if(YAC_CHK(_o, clid_IVector))
   {
      _IVector *o=(_IVector*)_o;
      x+=(sSI)(o->x*_v);
      y+=(sSI)(o->y*_v);
   }
}

sF32 _IVector::_abs(void) {
   return ((sF32)sFSqrt((sF32)(x*x+y*y)));
}

sSI _IVector::_absMax(void) {
   sSI xx,yy;
   xx = sABS(x);
   yy = sABS(y);
   if(xx>yy)
      return xx;
   else
      return yy;
}

sSI _IVector::_absAdd(void) {
   return sABS(x)+sABS(y);
}

void _IVector::_rotate(YAC_Object*_o) {
   if(YAC_CHK(_o, clid_Matrix))
   {
      _Matrix *o=(_Matrix*)_o;
      sFVector tv;
      tv.x=(sF32)x;
      tv.y=(sF32)y;
      tv.z=0.0f;
      tv.Rotate(o);
      x=(sSI)tv.x;
      y=(sSI)tv.y;
   }
}

void _IVector::_rotateInv(YAC_Object*_o) {
   if(YAC_CHK(_o, clid_Matrix))
   {
      _Matrix *o=(_Matrix*)_o;
      sFVector tv;
      tv.x=(sF32)x;
      tv.y=(sF32)y;
      tv.z=0.0f;
      tv.RotateInv(o);
      x=(sSI)tv.x;
      y=(sSI)tv.y;
   }
}

void _IVector::_sphere(sF32 _x, sF32 _y, sF32 _z, sF32 _s) {
   sFMatrix m;
   m.init(_x, _y, _z);
   sFVector tv;
   tv.init(_s, 0.0f, 0.0f);
   tv.Rotate(&m);
   x=(sSI)tv.x;	
   y=(sSI)tv.y;
}

sSI _IVector::_getX(void) {
   return x;
}

sSI _IVector::_getY(void) {
   return y;
}

void _IVector::_setX(sSI _v) {
   x=_v;
}
void _IVector::_setY(sSI _v) {
   y=_v;
}

void _IVector::_print(void) {
   yac_host->printf("\nIVector: (%i, %i)\n", x, y);
}

void _IVector::_getString(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->yacArrayAlloc(1024, 0,0,0);
   s->printf("(%i, %i)", x, y);
   YAC_RETS(s, 1);
}

void _IVector::_add(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_IVector)) 
      
   {
      _IVector *o=(_IVector *)_v;
      x+=o->x;
      y+=o->y;
   }
}

void _IVector::_sub(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_IVector))
   {
      _IVector *o=(_IVector*)_v;
      x-=o->x;
      y-=o->y;
   }
}


void YAC_VCALL _IVector::yacOperatorClamp(YAC_Object*_min, YAC_Object*_max) {
   if(YAC_CHK(_min, clid_IVector))
      if(YAC_CHK(_max, clid_IVector))
      {
         _IVector*min=(_IVector*)_min;
         _IVector*max=(_IVector*)_max;
         if(x<min->x) x=min->x; else if(x>max->x) x=max->x;
         if(y<min->y) y=min->y; else if(y>max->y) y=max->y;
      }
}

void YAC_VCALL _IVector::yacOperatorWrap(YAC_Object*_min,YAC_Object*_max) {
   if(YAC_CHK(_min, clid_IVector))
      if(YAC_CHK(_max, clid_IVector))
      {
         _IVector*min=(_IVector*)_min;
         _IVector*max=(_IVector*)_max;
         if(x<min->x) x+=(max->x-min->x); else if(x>max->x) x-=(max->x-min->x);
         if(y<min->y) y+=(max->y-min->y); else if(y>max->y) y-=(max->y-min->y);
      }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ _WrappedFloat                                                ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_WrappedFloat::_WrappedFloat(void) {
   value=0.0f;
   min=0.0f;
   max=(sF32)sM_PI*2.0f;
   add=0.0001f;
}

_WrappedFloat::~_WrappedFloat() {}

void YAC_VCALL _WrappedFloat::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   
   YAC_SERIALIZE_F32(value);
   YAC_SERIALIZE_F32(add);
   YAC_SERIALIZE_F32(min);
   YAC_SERIALIZE_F32(max);
}

sUI YAC_VCALL _WrappedFloat::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   
   value=YAC_DESERIALIZE_F32();
   add=YAC_DESERIALIZE_F32();
   min=YAC_DESERIALIZE_F32();
   max=YAC_DESERIALIZE_F32();
   return 1;
}

void YAC_VCALL _WrappedFloat::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o!=((YAC_Object*)this))
   {
      if(YAC_CHK(_o, class_ID))
      {
         if(_cmd==YAC_OP_ASSIGN)
         {
            _WrappedFloat *o=(_WrappedFloat*)_o;
            value=o->value;
            add=o->add;
            min=o->min;
            max=(sF32)o->max;
         }
      }
      else YAC_Object::yacOperator(_cmd, _o, _r);
   }
}

void _WrappedFloat::_setValue(sF32 _v) {
   value=_v;
}

sF32 _WrappedFloat::_getValue(void) {
   return value;
}

void _WrappedFloat::_setAdd(sF32 _add) {
   add=_add;
}

sF32 _WrappedFloat::_getAdd(void) {
   return add;
}

void _WrappedFloat::_setMin(sF32 _v) {
   min=_v;
}

sF32 _WrappedFloat::_getMin(void) {
   return min;
}

void _WrappedFloat::_setMax(sF32 _v) {
   max=_v;
}

sF32 _WrappedFloat::_getMax(void) {
   return max;
}

void _WrappedFloat::_init(sF32 _v, sF32 _a, sF32 _min, sF32 _max) {
   value=_v;
   add=_a;
   min=_min;
   max=_max;
}

void _WrappedFloat::_tick(void) {
   value+=add;
   if(value>=max)
      value=min+(value-max);
   else
      if(value<min)
         value=max-(min-value);
}

void _WrappedFloat::_tickPrecise(sF32 _f) {
   value+=add*_f;
   if(value>=max)
      value=min+(value-max);
   else
      if(value<min)
         value=max-(min-value);
}

