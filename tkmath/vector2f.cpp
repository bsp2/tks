/// vector2f.cpp
///
/// (c) 2008-2025 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <math.h>

#include "tkmath_yac.h"
#include "tkmath.h"
#include "matrix.h"
#include "vector2f.h"
#include "vector3f.h"
#include "vector4f.h"
#include "complexf.h"
#include "matrix2f.h"
#include "matrix2x3f.h"
#include "matrix3f.h"
#include "matrix4f.h"

#include "ying_Math.h"


_Vector2f::_Vector2f(void) {
   floats[0] = floats[1] = 0.0f;
}

_Vector2f::~_Vector2f() {
}

sUI _Vector2f::yacArrayGetNumElements(void) {
   return 2u;
}

sUI _Vector2f::yacArrayGetMaxElements(void) {
   return 2u;
}

sUI _Vector2f::yacArrayGetElementType(void) {
   return YAC_TYPE_FLOAT;
}

sUI _Vector2f::yacArrayGetElementByteSize(void) {
   return sizeof(sF32);
}

void _Vector2f::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 2)
   {
      sF32 v;
      switch(_val->type)
      {
         case YAC_TYPE_VOID:
            v = 0.0f;
            break;

         case YAC_TYPE_INT:
            v = (sF32)_val->value.int_val;
            break;

         case YAC_TYPE_FLOAT:
            v = _val->value.float_val;
            break;

         case YAC_TYPE_OBJECT:
            if(YAC_VALID(_val->value.object_val))
            {
               if(!_val->value.object_val->yacScanF32(&v))
               {
                  // Scan failed, silently assign 0.0f
                  v = 0.0f;
               }
            }
            else
            {
               // Invalid or <null> object, silently assign 0.0f
               v = 0.0f;
            }
            break;

         default:
            // Should never ever happen!
            Dyac_throw(TypeMismatch, "tkmath::Vector2f::yacArraySet: unknown type");
            v = 0.0f;
            break;
      }
      floats[_idx] = v;
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Vector2f::yacArraySet: Index too high (%u > 1)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Vector2f::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 2u)
   {
      _val->initFloat(floats[_idx]);
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Vector2f::yacArrayGet: Index too high (%u > 1)", _idx);
      _val->initFloat(0.0f);
      Dyac_throw(ReadArrayOutOfBounds, buf);
   }
}

void *YAC_VCALL _Vector2f::yacArrayGetPointer(void) {
   return (void*)floats;
}

sSI YAC_VCALL _Vector2f::yacTensorRank(void) {
   return YAC_TENSOR_RANK_VECTOR;
}

void YAC_VCALL _Vector2f::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F32(floats[0]);
   YAC_SERIALIZE_F32(floats[1]);
}

sUI YAC_VCALL _Vector2f::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   floats[0] = YAC_DESERIALIZE_F32();
   floats[1] = YAC_DESERIALIZE_F32();
   return sUI(YAC_TRUE);
}

void YAC_VCALL _Vector2f::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector2f))
   {
      if(YAC_BCHK(_max, clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, min, _min);
         YAC_CAST_ARG(_Vector2f, max, _max);

         if(floats[0] < min->floats[0])
            floats[0] = min->floats[0];
         else if(floats[0] > max->floats[0])
            floats[0] = max->floats[0];

         if(floats[1] < min->floats[1])
            floats[1] = min->floats[1];
         else if(floats[1] > max->floats[1])
            floats[1] = max->floats[1];
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector2f::yacOperatorClamp: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector2f::yacOperatorClamp: invalid _min object.");
   }
}

void YAC_VCALL _Vector2f::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector2f))
   {
      if(YAC_BCHK(_max, clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, min, _min);
         YAC_CAST_ARG(_Vector2f, max, _max);

         if(floats[0] < min->floats[0])
            floats[0] += (max->floats[0] - min->floats[0]);
         else if(floats[0] > max->floats[0])
            floats[0] -= (max->floats[0] - min->floats[0]);

         if(floats[1] < min->floats[1])
            floats[1] += (max->floats[1] - min->floats[1]);
         else if(floats[1] > max->floats[1])
            floats[1] -= (max->floats[1] - min->floats[1]);
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector2f::yacOperatorWrap: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector2f::yacOperatorWrap: invalid _min object.");
   }
}


void YAC_VCALL _Vector2f::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      switch(_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI(Dfltequal(floats[0], o->floats[0]) &&
                     Dfltequal(floats[1], o->floats[1])
                     );
            break;

         case YAC_OP_CNE:
            YAC_RETI(Dfltnotequal(floats[0], o->floats[0]) ||
                     Dfltnotequal(floats[1], o->floats[1])
                     );
            break;

         case YAC_OP_ASSIGN:
            floats[0] = o->floats[0];
            floats[1] = o->floats[1];
            break;

         case YAC_OP_ADD:
            _add_YAC_RSELF(_o);
            break;

         case YAC_OP_SUB:
            _sub_YAC_RSELF(_o);
            break;

        case YAC_OP_MUL:
            _mul_YAC_RSELF(_o);
            break;

         case YAC_OP_NEG:
            floats[0] = -floats[0];
            floats[1] = -floats[1];
            break;

         case YAC_OP_INIT:
            floats[0] = 0.0f;
            floats[1] = 0.0f;
            break;

         default:
         {
            char buf[128];
            Dyac_snprintf(buf, 128, "tkmath::Vector2f::yacOperator unsupported cmd %d", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
         }
         return;
      }
   }
   else if(YAC_Is_FloatArray(_o))
   {
      YAC_CAST_ARG(YAC_FloatArray, o, _o);
      switch(_cmd)
      {
         case YAC_OP_ASSIGN:
            assignFloatArray(o);
            break;

         default:
         {
            // Construct temporary Vector2f object
            _Vector2f t;
            t.class_ID = clid_Vector2f;
            t.assignFloatArray(o);
            yacOperator(_cmd, &t, _r);
         }
         break;
      }
   }
   else if(YAC_VALID(_o))
   {
      sF32 val;
      if(_o->yacScanF32(&val))
      {
         yacOperatorF32(_cmd, val, _r);
      }
      else
      {
         // Handle generic array object
         switch(_cmd)
         {
            case YAC_OP_ASSIGN:
               assignGeneric(NULL, _o);
               break;

            default:
            {
               /* // Construct temporary Vector2f object
               _Vector2f t;
               t.class_ID = clid_Vector2f;
               t.assignGeneric(NULL, _o);*/
               YAC_Object::yacOperator(_cmd, _o, _r);
            }
            break;
         }
      }
   }
   else
   {
      switch(_cmd)
      {
         case YAC_OP_NEG:
            floats[0] = -floats[0];
            floats[1] = -floats[1];
            break;

         default:
            // Not a valid Object (e.g. <null>)
            YAC_Object::yacOperator(_cmd,_o,_r);
      }
   }
}

void _Vector2f::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF32(_cmd, (sF32)val, _r);
}

void _Vector2f::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_ASSIGN:
         _init(val, 0.0f);
         break;

      case YAC_OP_ADD:
         _addf_YAC_RSELF(val);
         break;

      case YAC_OP_SUB:
         _addf_YAC_RSELF(-val);
         break;

      case YAC_OP_MUL:
         _mulf_YAC_RSELF(val);
         break;

      case YAC_OP_DIV:
         if(Dfltnonzero(val))
         {
            _mulf_YAC_RSELF(1.0f / val);
         }
         else
         {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2f::yacOperatorF32 OP_DIV Cannot divide by zero");
         }
         break;

      default:
         char buf[128];
         Dyac_snprintf(buf, 128, "tkmath::Vector2f::yacOperatorF32 Operator not supported %d", _cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
         break;
   }
}

void _Vector2f::assignFloats(const sF32 *_fv, sUI _ne) {
   if(_ne <= 2u)
   {
      // Copy as much elements from array as possible but never more than 2
      sUI i;
      for(i = 0u; i < _ne; i++)
         floats[i] = _fv[i];

      // Fill the rest with 0.0f
      for(; i < 2u; i++)
         floats[i] = 0.0f;

   }
   else
   {
      // Copy two elements
      floats[0] = _fv[0];
      floats[1] = _fv[1];
   }
}

void _Vector2f::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Vector2f::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
   sUI ne = _o->yacArrayGetNumElements();
   if(ne > 0u)
   {
      if(YAC_TYPE_FLOAT == _o->yacArrayGetElementType())
      {
         const sF32 *fv = (const sF32*) _o->yacArrayGetPointer();
         if(NULL != fv)
         {
            // Copy fast
            assignFloats(fv, ne);
            return;
         }
         else
         {
            // Retrieve individual float elements and assign
            YAC_Value v;
            sUI i;
            for(i = 0u; i < 2u && i < ne; i++)
            {
               _o->yacArrayGet(_ctx, i, &v);
               floats[i] = v.value.float_val;
            }
            // Fill the rest with 0.0f
            for(; i < 2u; i++)
               floats[i] = 0.0f;
         }
      }
      else
      {
         // Retrieve individual elements, typecast to float and assign
         YAC_Value v;
         sUI i;
         for(i = 0u; i < 2u && i < ne; i++)
         {
            _o->yacArrayGet(_ctx, i, &v);
            v.typecast(YAC_TYPE_FLOAT);
            floats[i] = v.value.float_val;
            v.unset();
         }
         // Fill the rest with 0.0f
         for(; i < 2u; i++)
            floats[i] = 0.0f;
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      floats[0] = floats[1] = 0.0f;
   }
}

void _Vector2f::_init(sF32 _x, sF32 _y) {
   floats[0] = _x;
   floats[1] = _y;
}

void _Vector2f::_setX(sF32 x) {
   floats[0] = x;
}

void _Vector2f::_setY(sF32 y) {
   floats[1] = y;
}

sF32 _Vector2f::_getX(void) const {
   return floats[0];
}

sF32 _Vector2f::_getY(void) const {
   return floats[1];
}

void _Vector2f::_getYx_YAC_RSELF(void) const {
}

void _Vector2f::_getYx_YAC_RVAL(YAC_Value *_r) const {
   _Vector2f *o = YAC_NEW_POOLED(Vector2f);
   o->_init(floats[1], floats[0]);
   _r->initObject(o, 1);
}

void _Vector2f::_getYx_YAC_RARG(YAC_Object *_o) const {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      o->_init(floats[1], floats[0]);
   }
}

sBool YAC_VCALL _Vector2f::yacToString(YAC_String *s) const {
   // generate string representation
   s->yacArrayAlloc(64, 0,0,0);
   s->printf("(%g, %g)", floats[0], floats[1]);
   s->fixLength();
   return YAC_TRUE;
}

void _Vector2f::_getString(YAC_Value *_r) const {
   // create string object and fill with string representation
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING64);
   yacToString(s);
   YAC_RETS(s, YAC_TRUE);
}

void _Vector2f::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      // add two vector2f
      YAC_CAST_ARG(_Vector2f, o, _o);
      floats[0] += o->floats[0];
      floats[1] += o->floats[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::add_SELF: invalid Vector2f object");
   }
   /* else
   {
      // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);
   }*/
}

void _Vector2f::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   // add two vector2f
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      _Vector2f *r = YAC_NEW_POOLED(Vector2f);
      r->floats[0] = floats[0] + o->floats[0];
      r->floats[1] = floats[1] + o->floats[1];
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::add_VAL: invalid Vector2f object");
   }
   /* else
     {
     // Add generic Object
     YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);
   }*/
}

void _Vector2f::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   // add two vector2f
   if(YAC_BCHK(_o, clid_Vector2f) && YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      YAC_CAST_ARG(_Vector2f, r, _r);
      r->floats[0] = floats[0] + o->floats[0];
      r->floats[1] = floats[1] + o->floats[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::add_ARG: invalid Vector2f object");
   }
   /* else
   {
      // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);
   }*/
}

void _Vector2f::_sub_YAC_RSELF(YAC_Object *_o) {
   // sub two vector2f
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      floats[0] -= o->floats[0];
      floats[1] -= o->floats[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::sub_SELF Invalid Vector2f object");
   }
   /* else
   {
      // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_SUB, _o, &r);
   }*/
}

void _Vector2f::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   // sub two vector2f
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      _Vector2f *r = YAC_NEW_POOLED(Vector2f);
      r->floats[0] = floats[0] - o->floats[0];
      r->floats[1] = floats[1] - o->floats[1];
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::sub_VAL invalid Vector2f object");
   }
   /* else
   {
      // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_SUB, _o, &r);
   }*/
}

void _Vector2f::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   // sub two vector2f
   if(YAC_BCHK(_o, clid_Vector2f) && YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      YAC_CAST_ARG(_Vector2f, r, _r);
      r->floats[0] = floats[0] - o->floats[0];
      r->floats[1] = floats[1] - o->floats[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::sub: invalid Vector2f object");
   }
   /*   else
   {
      // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_SUB, _o, &r);
   }*/
}

sF32 _Vector2f::_dot(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      return floats[0] * o->floats[0] + floats[1] * o->floats[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::dot: invalid Vector2f object");
      return 0.0f;
   }
   // there is no such thing like a dot product from a vector2f with a generic object!
   /*   else
   {
      // Dot product with generic Object
      _Vector2f t;
      t.assignGeneric(NULL, _o);
      return (floats[0] * t.floats[0] + floats[1] * t.floats[1]);
   }*/
}

void _Vector2f::_tensor_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      _Matrix2f *m = YAC_NEW_POOLED(Matrix2f);
      _r->initObject(m, YAC_TRUE);
      m->floats[0] = floats[0] * o->floats[0];
      m->floats[1] = floats[0] * o->floats[1];
      m->floats[2] = floats[1] * o->floats[0];
      m->floats[3] = floats[1] * o->floats[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::tensor_VAL Invalid Vector2f object");
   }
}

void _Vector2f::_tensor_YAC_RARG(YAC_Object *_o, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, m, _m);
      if(YAC_BCHK(_o, clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, o, _o);
         m->floats[0] = floats[0] * o->floats[0];
         m->floats[1] = floats[0] * o->floats[1];
         m->floats[2] = floats[1] * o->floats[0];
         m->floats[3] = floats[1] * o->floats[1];
      }
      /* else
      {
         _Vector2f t;
         t.assignGeneric(NULL, _o);
         o = &t;
      }*/
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::tensor_ARG Invalid Vector2f object");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::tensor_ARG: Invalid Matrix2f return object");
   }
}

void _Vector2f::_addf_YAC_RSELF(sF32 s) {
   floats[0] += s;
   floats[1] += s;
}

void _Vector2f::_addf_YAC_RVAL(sF32 s, YAC_Value *_r) {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[0] + s;
   r->floats[1] = floats[1] + s;
}

void _Vector2f::_addf_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, r, _r);
      r->floats[0] = floats[0] + s;
      r->floats[1] = floats[1] + s;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::addf_ARG Return object not of type Vector2f");
   }
}

void _Vector2f::_add2f_YAC_RSELF(sF32 _tx, sF32 _ty) {
   floats[0] += _tx;
   floats[1] += _ty;
}

void _Vector2f::_add2f_YAC_RVAL(sF32 _tx, sF32 _ty, YAC_Value *_r) {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[0] + _tx;
   r->floats[1] = floats[1] + _ty;
}

void _Vector2f::_add2f_YAC_RARG(sF32 _tx, sF32 _ty, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, r, _r);
      r->floats[0] = floats[0] + _tx;
      r->floats[1] = floats[1] + _ty;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::addf_ARG Return object not of type Vector2f");
   }
}

void _Vector2f::_mulf_YAC_RSELF(sF32 _s) {
   floats[0] *= _s;
   floats[1] *= _s;
}

void _Vector2f::_mulf_YAC_RVAL(sF32 _s, YAC_Value *_r) {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[0] * _s;
   r->floats[1] = floats[1] * _s;
}

void _Vector2f::_mulf_YAC_RARG(sF32 _s, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, r, _r);
      r->floats[0] = floats[0] * _s;
      r->floats[1] = floats[1] * _s;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::mulf_ARG Return object not of type Vector2f");
   }
}

void _Vector2f::_mul2f_YAC_RSELF(sF32 _sx, sF32 _sy) {
   floats[0] *= _sx;
   floats[1] *= _sy;
}

void _Vector2f::_mul2f_YAC_RVAL(sF32 _sx, sF32 _sy, YAC_Value *_r) {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[0] * _sx;
   r->floats[1] = floats[1] * _sy;
}

void _Vector2f::_mul2f_YAC_RARG(sF32 _sx, sF32 _sy, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, r, _r);
      r->floats[0] = floats[0] * _sx;
      r->floats[1] = floats[1] * _sy;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::mul2f_ARG Return object not of type Vector2f");
   }
}

void _Vector2f::_mul_YAC_RSELF(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      floats[0] *= o->floats[0];
      floats[1] *= o->floats[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::vector2f::mul_SELF Invalid Vector2f object");
   }
   /* else
   {
      // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);
   }*/
}

void _Vector2f::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      _Vector2f *r = YAC_NEW_POOLED(Vector2f);
      _r->initObject(r, YAC_TRUE);
      r->floats[0] = floats[0] * o->floats[0];
      r->floats[1] = floats[1] * o->floats[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::vector2f::mul_VAL Invalid Vector2f object");
   }
   /* else
   {
      // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);
   }*/
}

void _Vector2f::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_BCHK(_o, clid_Vector2f) && YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      YAC_CAST_ARG(_Vector2f, r, _r);
      r->floats[0] = floats[0] * o->floats[0];
      r->floats[1] = floats[1] * o->floats[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::vector2f::mul_ARG Invalid Vector2f object");
   }
   /* else
   {
      // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);
   }*/
}

void _Vector2f::_transform_YAC_RSELF(YAC_Object *_m) {
   if(YAC_BCHK(_m, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, m, _m);
      m->_mulv_YAC_RARG(this, this);
   }
   else if(YAC_BCHK(_m, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, m, _m);
      m->_mulv_YAC_RARG(this, this);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::transform_RSELF: \'m\' is not a Matrix2f or Matrix2x3f");
   }
}

void _Vector2f::_transform_YAC_RVAL(YAC_Object *_m, YAC_Value *_r) {
   if(YAC_BCHK(_m, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, m, _m);
      m->_mulv_YAC_RVAL(this, _r);
   }
   else if(YAC_BCHK(_m, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, m, _m);
      m->_mulv_YAC_RVAL(this, _r);
   }
   else if(YAC_BCHK(_m, clid_Matrix3f))
   {
      YAC_CAST_ARG(_Matrix3f, m, _m);
      m->_mulv_YAC_RVAL(this, _r);
   }
   else if(YAC_BCHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_mulv_YAC_RVAL(this, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::transform_RSELF: \'m\' is not a Matrix[234]f");
   }
}

void _Vector2f::_transform_YAC_RARG(YAC_Object *_m, YAC_Object *_r) {
   if(YAC_BCHK(_m, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, m, _m);

      if(YAC_BCHK(_r, clid_Vector2f))
      {
         m->_mulv_YAC_RARG(this, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::transform_RARG: return object is not a Vector2f");
      }
   }
   else if(YAC_BCHK(_m, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, m, _m);

      if(YAC_BCHK(_r, clid_Vector2f))
      {
         m->_mulv_YAC_RARG(this, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::transform_RARG: return object is not a Vector2f");
      }
   }
   else if(YAC_BCHK(_m, clid_Matrix3f))
   {
      YAC_CAST_ARG(_Matrix3f, m, _m);

      if(YAC_BCHK(_r, clid_Vector3f))
      {
         YAC_NEW_STACK(Vector3f, v);
         v.floats[0] = floats[0];
         v.floats[1] = floats[1];
         v.floats[2] = 0.0f;

         m->_mulv_YAC_RARG(this, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::transform_RARG: return object is not a Vector3f");
      }
   }
   else if(YAC_BCHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);

      if(YAC_BCHK(_r, clid_Vector4f))
      {
         YAC_NEW_STACK(Vector4f, v);
         v.floats[0] = floats[0];
         v.floats[1] = floats[1];
         v.floats[2] = 0.0f;
         v.floats[3] = 1.0f;

         m->_mulv_YAC_RARG(&v, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::transform_RARG: return object is not a Vector4f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::transform_RARG: \'m\' is not a Matrix[234]f");
   }
}

sF32 _Vector2f::_getAbs(void) const {
   // return length of vector2f
   // could use math.h hypoth function instead
   return sqrtf(floats[0]*floats[0] + floats[1]*floats[1]);
}

sF32 _Vector2f::_getAbsSqr(void) const {
   // return squared length of vector2f - faster and does the job as well as getAbs
   // in many circumstances
   return (floats[0] * floats[0] + floats[1] * floats[1]);
}

void _Vector2f::_unit_YAC_RSELF(void) {
   // scale vector2f to length 1.0f
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = 1.0f / absval;
      floats[0] *= absval;
      floats[1] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2f::unit_SELF Zero Vector cannot be normalized");
      floats[0] = 0.0f;
      floats[1] = 0.0f;
   }
}

void _Vector2f::_unit_YAC_RVAL(YAC_Value *_r) {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = 1.0f / absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2f::unit_VAL Zero Vector cannot be normalized");
   }
}

void _Vector2f::_unit_YAC_RARG(YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, r, _r);
      sF32 absval = _getAbs();
      if(Dfltnonzero(absval))
      {
         absval = 1.0f / absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
      }
      else
      {
         // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2f::unit_ARG Zero Vector cannot be normalized");
         r->floats[0] = 0.0f;
         r->floats[1] = 0.0f;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::unit_ARG Return object not of type Vector2f");
   }
}

void _Vector2f::_unitScale_YAC_RSELF(sF32 s) {
   // scale vector2f to length s
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = s / absval;
      floats[0] *= absval;
      floats[1] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2f::unitScale_SELF Zero Vector cannot be normalized");
      floats[0] = 0.0f;
      floats[1] = 0.0f;
   }
}

void _Vector2f::_unitScale_YAC_RVAL(sF32 s, YAC_Value *_r) {
   sF32 absval = _getAbs();
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   if(Dfltnonzero(absval))
   {
      absval = s / absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2f::unitScale_VAL Zero Vector cannot be normalized");
   }
}

void _Vector2f::_unitScale_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, r, _r);
      sF32 absval = _getAbs();
      if(Dfltnonzero(absval))
      {
         absval = s / absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
      }
      else
      {
         // // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2f::unitScale_ARG Zero Vector cannot be normalized");
         r->floats[0] = 0.0f;
         r->floats[1] = 0.0f;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::unitScale_ARG Return object not of type Vector2f");
   }
}

void _Vector2f::_initScalef(YAC_Object *_o, sF32 _s) {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      floats[0] = o->floats[0] * _s;
      floats[1] = o->floats[1] * _s;
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector2f::initScalef: invalid object parameter");
   }
}

sF32 _Vector2f::_distanceToPlane(YAC_Object *_q, YAC_Object *_n) const {
   if(YAC_BCHK(_q, clid_Vector2f))
   {
      if(YAC_BCHK(_n, clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, q, _q);
         YAC_CAST_ARG(_Vector2f, n, _n);
         YAC_NEW_STACK(Vector2f, v);
         _sub_YAC_RARG(q, &v);
         sF32 nAbs = n->_getAbs();
         return (0.0f != nAbs) ? (v._dot(n) / nAbs) : 999999999.0f;
      }
   }
	return 0.0f;
}

sF32 _Vector2f::intersect(YAC_Object *_v1s, YAC_Object *_v1e,
                          YAC_Object *_v2s, YAC_Object *_v2e,
                          sBool _bExtrapolate
                          ) {
      // returns intersection point in 'this'
      // returns normalized position on edge v2s<>v2e

   if(YAC_BCHK(_v1s,  clid_Vector2f) &&
      YAC_BCHK(_v1e,  clid_Vector2f) &&
      YAC_BCHK(_v2s,  clid_Vector2f) &&
      YAC_BCHK(_v2e,  clid_Vector2f)
      )
   {
      YAC_CAST_ARG(_Vector2f, v1s,  _v1s);
      YAC_CAST_ARG(_Vector2f, v1e,  _v1e);
      YAC_CAST_ARG(_Vector2f, v2s,  _v2s);
      YAC_CAST_ARG(_Vector2f, v2e,  _v2e);

      // if(b_debug)
      // {
      //    float mx;
      //    mx = (v1e.y - v1s.y) / (v1e.x - v1s.x);
      //    trace "xxx intersect: v1s="+v1s+" v1e="+v1e+" mx="+mx;
      //    mx = (v2e.y - v2s.y) / (v2e.x - v2s.x);
      //    trace "xxx intersect: v2s="+v2s+" v2e="+v2e+" mx="+mx;

      // }

      // <https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect>
      // E = B-A = ( Bx-Ax, By-Ay )
      // F = D-C = ( Dx-Cx, Dy-Cy )
      // P = ( -Ey, Ex )
      // h = ( (A-C) * P ) / ( F * P )        // * = dot product
      YAC_NEW_STACK(Vector2f, vE); vE.subFrom(v1e, v1s);
      YAC_NEW_STACK(Vector2f, vF); vF.subFrom(v2e, v2s);
      YAC_NEW_STACK(Vector2f, vP); vP._init(-vE.floats[1], vE.floats[0]);
      YAC_NEW_STACK(Vector2f, vG); vG._init(v1s->floats[0] - v2s->floats[0], v1s->floats[1] - v2s->floats[1]);
      sF32 h = vF.dot(&vP);
      if(0.0f != h)
         h = vG.dot(&vP) / h;
      // if(b_debug)
      //    trace "xxx intersect: h="+h+" bExtrapolate="+bExtrapolate;
      if( _bExtrapolate || ((0.0 <= h) && (h <= 1.0)) )
      {
         mulfFrom(&vF, h);
         add(v2s);
      }
      else
      {
         copyFrom(v2s);
      }
      return h;
   }
   return 0.0f;
}

void _Vector2f::_bilinearQuadPos(YAC_Object *_vLT, YAC_Object *_vRT, YAC_Object *_vRB, YAC_Object *_vLB, sF32 _nx, sF32 _ny) {
   if(YAC_BCHK(_vLT, clid_Vector2f) &&
      YAC_BCHK(_vRT, clid_Vector2f) &&
      YAC_BCHK(_vRB, clid_Vector2f) &&
      YAC_BCHK(_vLB, clid_Vector2f)
      )
   {
      YAC_CAST_ARG(_Vector2f, vLT, _vLT);
      YAC_CAST_ARG(_Vector2f, vRT, _vRT);
      YAC_CAST_ARG(_Vector2f, vRB, _vRB);
      YAC_CAST_ARG(_Vector2f, vLB, _vLB);

      const float tx = vLT->floats[0] + (vRT->floats[0] - vLT->floats[0]) * _nx;
      const float ty = vLT->floats[1] + (vRT->floats[1] - vLT->floats[1]) * _nx;

      const float bx = vLB->floats[0] + (vRB->floats[0] - vLB->floats[0]) * _nx;
      const float by = vLB->floats[1] + (vRB->floats[1] - vLB->floats[1]) * _nx;

      floats[0] = tx + (bx - tx) * _ny;
      floats[1] = ty + (by - ty) * _ny;
   }      
}

void _Vector2f::_rotateCW90_YAC_RSELF(void) {
   // y = -x , x = y
   const sF32 t = floats[0];
   floats[0] = floats[1];
   floats[1] = -t;
}

void _Vector2f::_rotateCW90_YAC_RVAL(YAC_Value *_r) {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[1];
   r->floats[1] = -floats[0];
}

void _Vector2f::_rotateCW90_YAC_RARG(YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, r, _r);
      r->floats[0] = floats[1];
      r->floats[1] = -floats[0];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::rotateCW90_ARG Return object not of type Vector2f");
   }
}

void _Vector2f::_rotateCCW90_YAC_RSELF(void) {
   const sF32 t = floats[0];
   floats[0] = -floats[1];
   floats[1] = t;
}

void _Vector2f::_rotateCCW90_YAC_RVAL(YAC_Value *_r) {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = -floats[1];
   r->floats[1] = floats[0];
}

void _Vector2f::_rotateCCW90_YAC_RARG(YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, r, _r);
      r->floats[0] = -floats[1];
      r->floats[1] = floats[0];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2f::rotateCCW90_ARG Return object not of type Vector2f");
   }
}

void _Vector2f::_New(sF32 _x, sF32 _y, YAC_Value *_r) {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   r->_init(_x, _y);
}

void YAC_CALL _vector2f(sF32 _x, sF32 _y, YAC_Value *_r) {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   r->_init(_x, _y);
}
