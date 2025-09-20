/// vector3f.cpp
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>,
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
#include "matrix3f.h"
#include "matrix4f.h"

#include "ying_Math.h"

// _Vector3f& _Vector3f::operator=(const _Vector3f& obj) {
//    floats[0] = obj.floats[0];
//    floats[1] = obj.floats[1];
//    floats[2] = obj.floats[2];
//    return *this;
// }

_Vector3f::_Vector3f(void) {
   floats[0] = floats[1] = floats[2] = 0.0f;
}

_Vector3f::~_Vector3f() {
}

sUI _Vector3f::yacArrayGetNumElements(void) {
   return 3u;
}

sUI _Vector3f::yacArrayGetMaxElements(void) {
   return 3u;
}

sUI _Vector3f::yacArrayGetElementType(void) {
   return YAC_TYPE_FLOAT;
}

sUI _Vector3f::yacArrayGetElementByteSize(void) {
   return sizeof(sF32);
}

void _Vector3f::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 3u)
   {
      sF32 v;
      switch(_val->type)
      {
         case YAC_TYPE_VOID:
            v = 0.0f;
            break;

         case YAC_TYPE_INT:
            v = (sF32) _val->value.int_val;
            break;

         case YAC_TYPE_FLOAT:
            v = _val->value.float_val;
            break;

         case YAC_TYPE_OBJECT:
            if(YAC_VALID(_val->value.object_val))
            {
               if(!_val->value.object_val->yacScanF32(&v))
                  v = 0.0f;
            }
            else
            {
               // silently cast <null> to 0.0f
               v = 0.0f;
            }
            break;

         default:
            // should not be reachable
            Dyac_throw(TypeMismatch, "tkmath::Vector3f::yacArraySet: unknown type");
            v = 0.0f;
            break;
      }
      floats[_idx] = v;
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Vector3f::yacArraySet: index out of bounds (%u > 2)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Vector3f::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 3u)
   {
      _val->initFloat(floats[_idx]);
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Vector3f::yacArrayGet: index out of bounds (%u > 2)", _idx);
      _val->initFloat(0.0f);
      Dyac_throw(ReadArrayOutOfBounds, buf);
   }
}

void *YAC_VCALL _Vector3f::yacArrayGetPointer(void) {
   return (void*)floats;
}

sSI YAC_VCALL _Vector3f::yacTensorRank(void) {
   return YAC_TENSOR_RANK_VECTOR;
}

void YAC_VCALL _Vector3f::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F32(floats[0]);
   YAC_SERIALIZE_F32(floats[1]);
   YAC_SERIALIZE_F32(floats[2]);
}

sUI YAC_VCALL _Vector3f::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   floats[0] = YAC_DESERIALIZE_F32();
   floats[1] = YAC_DESERIALIZE_F32();
   floats[2] = YAC_DESERIALIZE_F32();
   return 1u;
}

void YAC_VCALL _Vector3f::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector3f))
   {
      if(YAC_BCHK(_max, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, min, _min);
         YAC_CAST_ARG(_Vector3f, max, _max);

         for(sUI i = 0u; i < 3u; i++)
         {
            if(floats[i] < min->floats[i])
               floats[i] = min->floats[i];
            else if(floats[i] > max->floats[i])
               floats[i] = max->floats[i];
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector3f::yacOperatorClamp: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector3f::yacOperatorClamp: invalid _min object.");
   }
}

void YAC_VCALL _Vector3f::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector3f))
   {
      if(YAC_BCHK(_max, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, min, _min);
         YAC_CAST_ARG(_Vector3f, max, _max);

         for(sUI i = 0u; i < 3u; i++)
         {
            if(floats[i] < min->floats[i])
               floats[i] += (max->floats[i] - min->floats[i]);
            else if(floats[i] > max->floats[i])
               floats[i] -= (max->floats[i] - min->floats[i]);
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector3f::yacOperatorWrap: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector3f::yacOperatorWrap: invalid _min object.");
   }
}

void YAC_VCALL _Vector3f::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      switch(_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI(Dfltequal(floats[0], o->floats[0]) &&
                     Dfltequal(floats[1], o->floats[1]) &&
                     Dfltequal(floats[2], o->floats[2])
                     );
            break;

         case YAC_OP_CNE:
            YAC_RETI(Dfltnotequal(floats[0], o->floats[0]) ||
                     Dfltnotequal(floats[1], o->floats[1]) ||
                     Dfltnotequal(floats[2], o->floats[2])
                     );
            break;

         case YAC_OP_ASSIGN:
            floats[0] = o->floats[0];
            floats[1] = o->floats[1];
            floats[2] = o->floats[2];
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
            floats[2] = -floats[2];
            break;

         case YAC_OP_INIT:
            floats[0] = 0.0f;
            floats[1] = 0.0f;
            floats[2] = 0.0f;
            break;

         default:
         {
            // Should never ever happen !
            char buf[128];
            Dyac_snprintf(buf, 128, "tkmath::Vector3f::yacOperator unsupported cmd %d", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
         }
         return;
      } // switch _cmd
   }
   else if(YAC_VALID(_o))
   {
      sF32 val;
      if(_o->yacScanF32(&val))
      {
         yacOperatorF32(_cmd, val, _r);
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
               // Create temporary Vector3f object on stack
               YAC_NEW_STACK(Vector3f, t);
               t.assignFloatArray(o);

               // Recursively call yacOperator again
               yacOperator(_cmd, &t, _r);
            }
            break;
         }
      }
      else
      {
         // Handle generic float container object
         switch(_cmd)
         {
            case YAC_OP_ASSIGN:
               assignGeneric(NULL, _o);
               break;

            default:
            {
/*               _Vector3f t;
               t.class_ID = clid_Vector3f;
               t.assignGeneric(NULL, _o);*/

               // Recursively call yacOperator again
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
            floats[2] = -floats[2];
            break;

         default:
            // Call generic yacOperator, e.g. for <null> comparisons
            YAC_Object::yacOperator(_cmd, _o, _r);
            break;
      }
   }
}

void _Vector3f::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF32(_cmd, (sF32)val, _r);
}

void _Vector3f::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_ASSIGN:
         _init(val, 0.0f, 0.0f);
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
            _mulf_YAC_RSELF(1.0f/val);
         }
         else
         {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3f::yacOperatorF32 OP_DIV Cannot divide by zero");
         }
         break;

      default:
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Vector3f::yacOperatorF32 Operator not supported %d", _cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
         break;
   }
}

void _Vector3f::assignFloats(const sF32 *_fv, sUI _ne) {
   if(_ne <= 3)
   {
      // Copy as much elements from array as possible but never more than 3
      sUI i;
      for(i = 0u; i < _ne; i++)
         floats[i] = _fv[i];

      // Fill the rest with 0.0f
      for(; i < 3u; i++)
         floats[i] = 0.0f;
   }
   else
   {
      // Copy three elements
      floats[0] = _fv[0];
      floats[1] = _fv[1];
      floats[2] = _fv[2];
   }
}

void _Vector3f::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Vector3f::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
   sUI ne = _o->yacArrayGetNumElements();
   if(ne > 0)
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
            for(i = 0u; i < 3u && i < ne; i++)
            {
               _o->yacArrayGet(_ctx, i, &v);
               floats[i] = v.value.float_val;
            }
            // Fill the rest with 0.0f
            for(; i < 3u; i++)
               floats[i] = 0.0f;
         }
      }
      else
      {
         // Retrieve individual elements, typecast to float and assign
         YAC_Value v;
         sUI i;
         for(i = 0u; i < 3u && i < ne; i++)
         {
            _o->yacArrayGet(_ctx, i, &v);
            v.typecast(YAC_TYPE_FLOAT);
            floats[i] = v.value.float_val;
            v.unset();
         }
         // Fill the rest with 0.0f
         for(; i < 3u; i++)
            floats[i] = 0.0f;
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      floats[0] = floats[1] = floats[2] = 0.0f;
   }
}

void _Vector3f::_setX(sF32 x) {
   floats[0] = x;
}

void _Vector3f::_setY(sF32 y) {
   floats[1] = y;
}

void _Vector3f::_setZ(sF32 z) {
   floats[2] = z;
}

sF32 _Vector3f::_getX(void) const {
   return floats[0];
}

sF32 _Vector3f::_getY(void) const {
   return floats[1];
}

sF32 _Vector3f::_getZ(void) const {
   return floats[2];
}

void _Vector3f::copytoobject_helper(YAC_Object *_o, const sUI _numComp, const sUI _idx0, const sUI _idx1, const sUI _idx2) const {
   const sUI ne = _o->yacArrayGetNumElements();
   if(ne >= 2u)
   {
      YAC_Value v; v.initFloat(floats[_idx0]);
      _o->yacArraySet(NULL/*context*/, 0u, &v);

      v.value.float_val = floats[_idx1];
      _o->yacArraySet(NULL/*context*/, 1u, &v);

      if(ne >= 3u)
      {
         v.value.float_val = (_numComp >= 3u) ? floats[_idx2] : 0.0f;
         _o->yacArraySet(NULL/*context*/, 2u, &v);

         if(ne >= 4u)
         {
            v.value.float_val = 1.0f;
            _o->yacArraySet(NULL/*context*/, 3u, &v);
         }
      }
   }
}

// void _Vector3f::_getXy_YAC_RSELF(void) const {
// }

void _Vector3f::_getXy_YAC_RVAL(YAC_Value *_r) const {
   _Vector2f *o = YAC_NEW_POOLED(Vector2f);
   o->_init(floats[0], floats[1]);
   _r->initObject(o, 1);
}

void _Vector3f::_getXy_YAC_RARG(YAC_Object *_o) const {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      o->_init(floats[0], floats[1]);
   }
}

void _Vector3f::_getXz_YAC_RVAL(YAC_Value *_r) const {
   _Vector2f *o = YAC_NEW_POOLED(Vector2f);
   o->_init(floats[0], floats[2]);
   _r->initObject(o, 1);
}

void _Vector3f::_getXz_YAC_RARG(YAC_Object *_o) const {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      o->_init(floats[0], floats[2]);
   }
}

void _Vector3f::_getYx_YAC_RVAL(YAC_Value *_r) const {
   _Vector2f *o = YAC_NEW_POOLED(Vector2f);
   o->_init(floats[1], floats[0]);
   _r->initObject(o, 1);
}

void _Vector3f::_getYx_YAC_RARG(YAC_Object *_o) const {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      o->_init(floats[1], floats[0]);
   }
}

sBool YAC_VCALL _Vector3f::yacToString(YAC_String *s) const {
#ifdef YAC_BIGSTRING
   s->yacArrayAlloc(256,0,0,0);
   s->printf("(%g, %g, %g)", floats[0], floats[1], floats[2]);
   s->fixLength();
   return YAC_TRUE;
#endif
   return YAC_FALSE;
}

void _Vector3f::_getString(YAC_Value *_r) const {
   YAC_String *s = YAC_New_String();
   yacToString(s);
   YAC_RETS(s, YAC_TRUE);
}

void _Vector3f::_init(sF32 x, sF32 y, sF32 z) {
   floats[0] = x;
   floats[1] = y;
   floats[2] = z;
}

void _Vector3f::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      floats[0] += o->floats[0];
      floats[1] += o->floats[1];
      floats[2] += o->floats[2];
   }
   else
   {
      /* // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::add_SELF Object not of Type Vector3f");
   }
}

void _Vector3f::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      _Vector3f *r = YAC_NEW_POOLED(Vector3f);
      _r->initObject(r, YAC_TRUE);
      r->floats[0] = floats[0] + o->floats[0];
      r->floats[1] = floats[1] + o->floats[1];
      r->floats[2] = floats[2] + o->floats[2];
   }
   else
   {
      /* // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::add_VAL Object not of Type Vector3f");
   }
}

void _Vector3f::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_BCHK(_o,clid_Vector3f) && YAC_BCHK(_r,clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      YAC_CAST_ARG(_Vector3f, r, _r);
      r->floats[0] = floats[0] + o->floats[0];
      r->floats[1] = floats[1] + o->floats[1];
      r->floats[2] = floats[2] + o->floats[2];
   }
   else
   {
      /* // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::add_ARG Object not of Type Vector3f");
   }
}

void _Vector3f::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      floats[0] -= o->floats[0];
      floats[1] -= o->floats[1];
      floats[2] -= o->floats[2];
   }
   else
   {
      /* // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::sub_SELF Object not of Type Vector3f");
   }
}

void _Vector3f::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      _Vector3f *r = YAC_NEW_POOLED(Vector3f);
      _r->initObject(r, YAC_TRUE);
      r->floats[0] = floats[0] - o->floats[0];
      r->floats[1] = floats[1] - o->floats[1];
      r->floats[2] = floats[2] - o->floats[2];
   }
   else
   {
      /* // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::sub_VAL Object not of Type Vector3f");
   }
}

void _Vector3f::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_BCHK(_o, clid_Vector3f) && YAC_BCHK(_r,clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      _Vector3f *r = (_Vector3f*)_r;
      r->floats[0] = floats[0] - o->floats[0];
      r->floats[1] = floats[1] - o->floats[1];
      r->floats[2] = floats[2] - o->floats[2];
   }
   else
   {
      /* // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::sub_ARG Object not of Type Vector3f");
   }
}

sF32 _Vector3f::_dot(YAC_Object *_o) {
   // returns scalar dot product
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      return floats[0]*o->floats[0] + floats[1]*o->floats[1] + floats[2]*o->floats[2];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::dot Object not of Type Vector3f");
   }
   return 0.0f;
}

void _Vector3f::_cross_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   // returns the orthogonal vector of the cross product
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      _Vector3f *res = YAC_NEW_POOLED(Vector3f);
      _r->initObject(res, YAC_TRUE);
      res->floats[0] = floats[1]*o->floats[2] - floats[2]*o->floats[1];
      res->floats[1] = floats[2]*o->floats[0] - floats[0]*o->floats[2];
      res->floats[2] = floats[0]*o->floats[1] - floats[1]*o->floats[0];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::cross Object not of type Vector3f");
   }
}

void _Vector3f::_cross_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   // returns the orthogonal vector of the cross product
   if(YAC_BCHK(_o, clid_Vector3f) && YAC_BCHK(_r, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      YAC_CAST_ARG(_Vector3f, r, _r);
      r->floats[0] = floats[1]*o->floats[2] - floats[2]*o->floats[1];
      r->floats[1] = floats[2]*o->floats[0] - floats[0]*o->floats[2];
      r->floats[2] = floats[0]*o->floats[1] - floats[1]*o->floats[0];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::cross Object not of type Vector3f");
   }
}

sF32 _Vector3f::_triple(YAC_Object *_b, YAC_Object *_c) {
   // return the scalar triple product (a*(bxc))
   if(YAC_BCHK(_b, clid_Vector3f) && YAC_BCHK(_c, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, b, _b);
      YAC_CAST_ARG(_Vector3f, c, _c);
      _Matrix3f m;
      memcpy(m.floats+0/*(0*sizeof(sF32))*/,    floats,sizeof(sF32)*3); // we copy as row vector instead of col
      memcpy(m.floats+3/*(3*sizeof(sF32))*/, b->floats,sizeof(sF32)*3); // because the determinant of a
      memcpy(m.floats+6/*(6*sizeof(sF32))*/, c->floats,sizeof(sF32)*3); // transposed matrix is the same
      return m._det();
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::triple Object not of type Vector3f");
   }
   return 0.0f;
}

void _Vector3f::_tensor_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   // return the matrix of the tensor product
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      _Matrix3f *m = YAC_NEW_POOLED(Matrix3f);
      _r->initObject(m, YAC_TRUE);
      m->floats[0] = floats[0] * o->floats[0];
      m->floats[1] = floats[0] * o->floats[1];
      m->floats[2] = floats[0] * o->floats[2];
      m->floats[3] = floats[1] * o->floats[0];
      m->floats[4] = floats[1] * o->floats[1];
      m->floats[5] = floats[1] * o->floats[2];
      m->floats[6] = floats[2] * o->floats[0];
      m->floats[7] = floats[2] * o->floats[1];
      m->floats[8] = floats[2] * o->floats[2];
   }
   else
   {
      // _Vector3f t;
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::tensor: invalid Vector3f object");
   }
}

void _Vector3f::_tensor_YAC_RARG(YAC_Object *_o, YAC_Object *_m) const {
   // return the matrix of the tensor product
   if(YAC_CHK(_m, clid_Matrix3f))
   {
      if(YAC_BCHK(_o, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, o, _o);
         YAC_CAST_ARG(_Matrix3f, m, _m);
         m->floats[0] = floats[0] * o->floats[0];
         m->floats[1] = floats[0] * o->floats[1];
         m->floats[2] = floats[0] * o->floats[2];
         m->floats[3] = floats[1] * o->floats[0];
         m->floats[4] = floats[1] * o->floats[1];
         m->floats[5] = floats[1] * o->floats[2];
         m->floats[6] = floats[2] * o->floats[0];
         m->floats[7] = floats[2] * o->floats[1];
         m->floats[8] = floats[2] * o->floats[2];
      }
      else
      {
         /* // _Vector3f t;
         t.assignGeneric(NULL, _o);
         o = &t;*/
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::tensor_: invalid Vector3f Object");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::tensor_: invalid Matrix3f object");
   }
}

void _Vector3f::_addf_YAC_RSELF(sF32 _s) {
   floats[0] += _s;
   floats[1] += _s;
   floats[2] += _s;
}

void _Vector3f::_addf_YAC_RVAL(sF32 _s, YAC_Value *_r) {
   _Vector3f *r = YAC_NEW_POOLED(Vector3f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[0] + _s;
   r->floats[1] = floats[1] + _s;
   r->floats[2] = floats[2] + _s;
}

void _Vector3f::_addf_YAC_RARG(sF32 _s, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, r, _r);
      r->floats[0] = floats[0] + _s;
      r->floats[1] = floats[1] + _s;
      r->floats[2] = floats[2] + _s;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::addf_ARG Return object not of type Vector3f");
   }
}

void _Vector3f::_add3f_YAC_RSELF(sF32 _sx, sF32 _sy, sF32 _sz) {
   floats[0] += _sx;
   floats[1] += _sy;
   floats[2] += _sz;
}

void _Vector3f::_add3f_YAC_RVAL(sF32 _sx, sF32 _sy, sF32 _sz, YAC_Value *_r) {
   _Vector3f *r = YAC_NEW_POOLED(Vector3f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[0] + _sx;
   r->floats[1] = floats[1] + _sy;
   r->floats[2] = floats[2] + _sz;
}

void _Vector3f::_add3f_YAC_RARG(sF32 _sx, sF32 _sy, sF32 _sz, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, r, _r);
      r->floats[0] = floats[0] + _sx;
      r->floats[1] = floats[1] + _sy;
      r->floats[2] = floats[2] + _sz;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::add3f_ARG Return object not of type Vector3f");
   }
}

void _Vector3f::_mulf_YAC_RSELF(sF32 _s) {
   floats[0] *= _s;
   floats[1] *= _s;
   floats[2] *= _s;
}

void _Vector3f::_mulf_YAC_RVAL(sF32 _s, YAC_Value *_r) {
   _Vector3f *r = YAC_NEW_POOLED(Vector3f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[0] * _s;
   r->floats[1] = floats[1] * _s;
   r->floats[2] = floats[2] * _s;
}

void _Vector3f::_mulf_YAC_RARG(sF32 _s, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, r, _r);
      r->floats[0] = floats[0] * _s;
      r->floats[1] = floats[1] * _s;
      r->floats[2] = floats[2] * _s;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::mulf_ARG Return type not of type Vector3f");
   }
}

void _Vector3f::_mul3f_YAC_RSELF(sF32 _sx, sF32 _sy, sF32 _sz) {
   floats[0] *= _sx;
   floats[1] *= _sy;
   floats[2] *= _sz;
}

void _Vector3f::_mul3f_YAC_RVAL(sF32 _sx, sF32 _sy, sF32 _sz, YAC_Value *_r) {
   _Vector3f *r = YAC_NEW_POOLED(Vector3f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[0] * _sx;
   r->floats[1] = floats[1] * _sy;
   r->floats[2] = floats[2] * _sz;
}

void _Vector3f::_mul3f_YAC_RARG(sF32 _sx, sF32 _sy, sF32 _sz, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, r, _r);
      r->floats[0] = floats[0] * _sx;
      r->floats[1] = floats[1] * _sy;
      r->floats[2] = floats[2] * _sz;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::mul3f_ARG Return type not of type Vector3f");
   }
}

void _Vector3f::_mul_YAC_RSELF(YAC_Object *_o) {
   // multiply each element
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      floats[0] *= o->floats[0];
      floats[1] *= o->floats[1];
      floats[2] *= o->floats[2];
   }
   else
   {
      /* // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::mul_SELF Invalid Vector3f object");

   }
}

void _Vector3f::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   // multiply each element
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      _Vector3f *r = YAC_NEW_POOLED(Vector3f);
      _r->initObject(r, YAC_TRUE);
      r->floats[0] = floats[0] * o->floats[0];
      r->floats[1] = floats[1] * o->floats[1];
      r->floats[2] = floats[2] * o->floats[2];
   }
   else
   {
      /* // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::mul_VAL Invalid Vector3f object");
   }
}

void _Vector3f::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_BCHK(_o, clid_Vector3f) && YAC_BCHK(_r, clid_Vector3f))
   {
      // multiply each element
      YAC_CAST_ARG(_Vector3f, o, _o);
      YAC_CAST_ARG(_Vector3f, r, _r);
      r->floats[0] = floats[0] * o->floats[0];
      r->floats[1] = floats[1] * o->floats[1];
      r->floats[2] = floats[2] * o->floats[2];
   }
   else
   {
      /* // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::mul_ARG Invalid Vector3f object");

   }
}

void _Vector3f::_transform_YAC_RSELF(YAC_Object *_m) {
   if(YAC_BCHK(_m, clid_Matrix3f))
   {
      YAC_CAST_ARG(_Matrix3f, m, _m);
      m->_mulv_YAC_RARG(this, this);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::transform_RSELF: \'m\' is not a Matrix3f");
   }
}

void _Vector3f::_transform_YAC_RVAL(YAC_Object *_m, YAC_Value *_r) {
   if(YAC_BCHK(_m, clid_Matrix3f))
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
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::transform_RSELF: \'m\' is not a Matrix[34]f");
   }
}

void _Vector3f::_transform_YAC_RARG(YAC_Object *_m, YAC_Object *_r) {
   if(YAC_BCHK(_m, clid_Matrix3f))
   {
      YAC_CAST_ARG(_Matrix3f, m, _m);

      if(YAC_BCHK(_r, clid_Vector3f))
      {
         m->_mulv_YAC_RARG(this, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::transform_RARG: return object is not a Vector3f");
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
         v.floats[2] = floats[2];
         v.floats[3] = 1.0f;
         m->_mulv_YAC_RARG(&v, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::transform_RARG: return object is not a Vector4f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3f::transform_RARG: \'m\' is not a Matrix[34]f");
   }
}

sF32 _Vector3f::_getAbs(void) const {
   return sqrtf(floats[0]*floats[0] + floats[1]*floats[1] + floats[2]*floats[2]);
}

sF32 _Vector3f::_getAbsSqr(void) const {
   return floats[0]*floats[0] + floats[1]*floats[1] + floats[2]*floats[2];
}

void _Vector3f::_unit_YAC_RSELF(void) {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = 1.0f / absval;
      floats[0] *= absval;
      floats[1] *= absval;
      floats[2] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3f::unit_SELF Zero Vector cannot be normalized");
      floats[0] = 0.0f;
      floats[1] = 0.0f;
      floats[2] = 0.0f;
   }
}

void _Vector3f::_unit_YAC_RVAL(YAC_Value *_r) {
   _Vector3f *r = YAC_NEW_POOLED(Vector3f);
   _r->initObject(r, YAC_TRUE);
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = 1.0f / absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
      r->floats[2] = floats[2] * absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3f::unit_VAL Zero Vector cannot be normalized");
   }
}

void _Vector3f::_unit_YAC_RARG(YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, r, _r);
      sF32 absval = _getAbs();
      if(Dfltnonzero(absval))
      {
         absval = 1.0f / absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
         r->floats[2] = floats[2] * absval;
      }
      else
      {
         // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3f::unit_ARG Zero Vector cannot be normalized");
         r->floats[0] = 0.0f;
         r->floats[1] = 0.0f;
         r->floats[2] = 0.0f;
      }
   }
   else
   {
     Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::unit_ARG Return object not of type Vector3f");
   }
}

void _Vector3f::_unitScale_YAC_RSELF(sF32 s) {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = s / absval;

      floats[0] *= absval;
      floats[1] *= absval;
      floats[2] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3f::unitScale_SELF Zero Vector cannot be normalized");
      floats[0] = 0.0f;
      floats[1] = 0.0f;
      floats[2] = 0.0f;
   }
}

void _Vector3f::_unitScale_YAC_RVAL(sF32 s, YAC_Value *_r) {
   _Vector3f *r = YAC_NEW_POOLED(Vector3f);
   _r->initObject(r, YAC_TRUE);
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = s / absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
      r->floats[2] = floats[2] * absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3f::unitScale_VAL Zero Vector cannot be normalized");
   }
}

void _Vector3f::_unitScale_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, r, _r);
      sF32 absval = _getAbs();
      if(Dfltnonzero(absval))
      {
         absval = s / absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
         r->floats[2] = floats[2] * absval;
      }
      else
      {
         // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3f::unitScale_ARG Zero Vector cannot be normalized");
         r->floats[0] = 0.0f;
         r->floats[1] = 0.0f;
         r->floats[2] = 0.0f;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3f::unitScale_ARG Return object not of type Vector3f");
   }
}

void _Vector3f::_initScalef(YAC_Object *_o, sF32 _s) {
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      floats[0] = o->floats[0] * _s;
      floats[1] = o->floats[1] * _s;
      floats[2] = o->floats[2] * _s;
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector3f::initScalef: invalid object parameter");
   }
}

sF32 _Vector3f::_distanceToPlane(YAC_Object *_q, YAC_Object *_n) const {
   if(YAC_BCHK(_q, clid_Vector3f))
   {
      if(YAC_BCHK(_n, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, q, _q);
         YAC_CAST_ARG(_Vector3f, n, _n);
         YAC_NEW_STACK(Vector3f, v);
         _sub_YAC_RARG(q, &v);
         sF32 nAbs = n->_getAbs();
         return (0.0f != nAbs) ? (v._dot(n) / nAbs) : 999999999.0f;
      }
   }
	return 0.0f;
}

sBool _Vector3f::_intersectPlane(YAC_Object *_q, YAC_Object *_n, YAC_Object *_p1, YAC_Object *_p2) {
   // <https://stackoverflow.com/questions/5666222/3d-line-plane-intersection>
   //   double t = (planeNormal.dot(planePoint) - planeNormal.dot(linePoint)) / planeNormal.dot(lineDirection.normalize());
   //   return linePoint.plus(lineDirection.normalize().scale(t));
   if(YAC_BCHK(_q,  clid_Vector3f) &&
      YAC_BCHK(_n,  clid_Vector3f) &&
      YAC_BCHK(_p1, clid_Vector3f) &&
      YAC_BCHK(_p2, clid_Vector3f)
      )
   {
      YAC_CAST_ARG(_Vector3f, q,  _q );
      YAC_CAST_ARG(_Vector3f, n,  _n );
      YAC_CAST_ARG(_Vector3f, p1, _p1);
      YAC_CAST_ARG(_Vector3f, p2, _p2);

      _Vector3f pDir; pDir.diffUnit(p1, p2);  // pDir = normalize(p2 - p1)

      const sF32 dotNQ = Dot(n, q);
      const sF32 dotN1 = Dot(n, p1);
      const sF32 dotND = Dot(n, &pDir);

      if(0.0f != dotND)
      {
         const sF32 t = (dotNQ - dotN1) / dotND;
         floats[0] = p1->floats[0] + pDir.floats[0] * t;
         floats[1] = p1->floats[1] + pDir.floats[1] * t;
         floats[2] = p1->floats[2] + pDir.floats[2] * t;
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

void _Vector3f::_divz_YAC_RSELF(void) {
   if(floats[2] > 0.0f)
   {
      floats[0] = floats[0] / floats[2];
      floats[1] = floats[1] / floats[2];
      floats[2] = 1.0f;
   }
   else
   {
      floats[0] = 0.0f;
      floats[1] = 0.0f;
      floats[2] = 1.0f;
   }
}

void _Vector3f::_divz_YAC_RVAL(YAC_Value *_r) const {
   _Vector2f *r = (_Vector2f*)YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   if(floats[2] > 0.0f)
   {
      r->floats[0] = floats[0] / floats[2];
      r->floats[1] = floats[1] / floats[2];
   }
   else
   {
      r->floats[0] = 0.0f;
      r->floats[1] = 0.0f;
   }
}

void _Vector3f::_divz_YAC_RARG(YAC_Object *_r) {
   if(YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, r, _r);
      if(floats[2] > 0.0f)
      {
         r->floats[0] = floats[0] / floats[2];
         r->floats[1] = floats[1] / floats[2];
      }
      else
      {
         r->floats[0] = 0.0f;
         r->floats[1] = 0.0f;
      }
   }
}

void _Vector3f::_bilinearQuadPos(YAC_Object *_vLT, YAC_Object *_vRT, YAC_Object *_vRB, YAC_Object *_vLB, sF32 _nx, sF32 _ny) {
   if(YAC_BCHK(_vLT, clid_Vector3f) &&
      YAC_BCHK(_vRT, clid_Vector3f) &&
      YAC_BCHK(_vRB, clid_Vector3f) &&
      YAC_BCHK(_vLB, clid_Vector3f)
      )
   {
      YAC_CAST_ARG(_Vector3f, vLT, _vLT);
      YAC_CAST_ARG(_Vector3f, vRT, _vRT);
      YAC_CAST_ARG(_Vector3f, vRB, _vRB);
      YAC_CAST_ARG(_Vector3f, vLB, _vLB);

      const float tx = vLT->floats[0] + (vRT->floats[0] - vLT->floats[0]) * _nx;
      const float ty = vLT->floats[1] + (vRT->floats[1] - vLT->floats[1]) * _nx;
      const float tz = vLT->floats[2] + (vRT->floats[2] - vLT->floats[2]) * _nx;

      const float bx = vLB->floats[0] + (vRB->floats[0] - vLB->floats[0]) * _nx;
      const float by = vLB->floats[1] + (vRB->floats[1] - vLB->floats[1]) * _nx;
      const float bz = vLB->floats[2] + (vRB->floats[2] - vLB->floats[2]) * _nx;

      floats[0] = tx + (bx - tx) * _ny;
      floats[1] = ty + (by - ty) * _ny;
      floats[2] = tz + (bz - tz) * _ny;
   }      
}

// void _Vector3f::_project(YAC_Object *_r, sF32 _vpX, sF32 _vpY, sF32 _vpW, sF32 _vpH, sF32 _depthRangeNear, sF32 _depthRangeFar) const {
//    // Convert clip coordinates to window coordinates
//    if(YAC_BCHK(_r, clid_Vector2f))
//    {
//       YAC_CAST_ARG(_Vector2f, r, _r);
//       if(0.0f != floats[3])
//       {
//          const sF32 sw = 0.5f * _vpW;
//          const sF32 sh = 0.5f * _vpH;
//          const sF32 obw  = 1.0f / floats[3];

//          r->floats[0] = (sw * floats[0]) * obw;
//          r->floats[1] = (sh * floats[1]) * obw;

//          r->floats[0] = _vpX + sw + r->floats[0];
//          r->floats[1] = _vpY + sh + r->floats[1];
//       }
//       else
//       {
//          r->floats[0] = 0.0f;
//          r->floats[1] = 0.0f;
//       }
//    }
//    else if(YAC_BCHK(_r, clid_Vector3f))
//    {
//       YAC_CAST_ARG(_Vector3f, r, _r);
//       if(0.0f != floats[3])
//       {
//          const sF32 sw = 0.5f * _vpW;
//          const sF32 sh = 0.5f * _vpH;
//          const sF32 sd = 0.5f * (_depthRangeFar - _depthRangeNear);
//          const sF32 obw  = 1.0f / floats[3];

//          r->floats[0] = (sw * floats[0]) * obw;
//          r->floats[1] = (sh * floats[1]) * obw;
//          r->floats[2] = (sd * floats[2]) * obw;

//          r->floats[0] = _vpX + sw + r->floats[0];
//          r->floats[1] = _vpY + sh + r->floats[1];
//          r->floats[2] =        sd + r->floats[2];
//       }
//       else
//       {
//          r->floats[0] = 0.0f;
//          r->floats[1] = 0.0f;
//          r->floats[2] = 0.0f;
//       }
//    }
// }

void _Vector3f::_New(sF32 x, sF32 y, sF32 z, YAC_Value *_r) {
   _Vector3f *r = YAC_NEW_POOLED(Vector3f);
   _r->initObject(r, YAC_TRUE);
   r->_init(x, y, z);
}

void YAC_CALL _vector3f(sF32 x, sF32 y, sF32 z,YAC_Value *_r) {
   _Vector3f *r = YAC_NEW_POOLED(Vector3f);
   _r->initObject(r, YAC_TRUE);
   r->_init(x, y, z);
}
