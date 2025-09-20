/// vector4f.cpp
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
#include "matrix4f.h"

#include "ying_Math.h"

_Vector4f::_Vector4f(void) {
   floats[0] = floats[1] = floats[2] = 0.0f;
   floats[3] = 1.0f;
}

_Vector4f::~_Vector4f() {
}

sUI _Vector4f::yacArrayGetNumElements(void) {
   return 4u;
}

sUI _Vector4f::yacArrayGetMaxElements(void) {
   return 4u;
}

sUI _Vector4f::yacArrayGetElementType(void) {
   return YAC_TYPE_FLOAT;
}

sUI _Vector4f::yacArrayGetElementByteSize(void) {
   return sizeof(sF32);
}

void YAC_VCALL _Vector4f::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4u)
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
                  v = 0.0f;
            }
            else
            {
               // Silently cast <null> to 0.0f
               v = 0.0f;
            }
            break;

         default:
            // Should never ever happen!
            Dyac_throw(TypeMismatch, "tkmath::Vector3f::yacArraySet: unknown type");
            v = 0.0f;
            break;
      }
      floats[_idx] = v;
   }
   else
   {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Vector4f::yacArraySet Index too high (%d>3)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Vector4f::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4u)
   {
      _val->initFloat(floats[_idx]);
   }
   else
   {
      char buf[256];
      Dyac_snprintf(buf,  256, "tkmath::Vector4f::yacArrayGet: Index too high (%d>3)", _idx);
      _val->initFloat(0.0f);
      Dyac_throw(ReadArrayOutOfBounds, buf);
   }
}

void *YAC_VCALL _Vector4f::yacArrayGetPointer(void) {
   return (void*)floats;
}

sSI YAC_VCALL _Vector4f::yacTensorRank(void) {
   return YAC_TENSOR_RANK_VECTOR;
}

void YAC_VCALL _Vector4f::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F32(floats[0]);
   YAC_SERIALIZE_F32(floats[1]);
   YAC_SERIALIZE_F32(floats[2]);
   YAC_SERIALIZE_F32(floats[3]);
}

sUI YAC_VCALL _Vector4f::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   floats[0] = YAC_DESERIALIZE_F32();
   floats[1] = YAC_DESERIALIZE_F32();
   floats[2] = YAC_DESERIALIZE_F32();
   floats[3] = YAC_DESERIALIZE_F32();
   return sUI(YAC_TRUE);
}

void YAC_VCALL _Vector4f::yacOperatorAssign(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      floats[0] = o->floats[0];
      floats[1] = o->floats[1];
      floats[2] = o->floats[2];
      floats[3] = o->floats[3];
   }
   else if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      floats[0] = o->floats[0];
      floats[1] = o->floats[1];
      floats[2] = o->floats[2];
      floats[3] = 1.0f;
   }
   else
   {
      YAC_Value r;
      yacOperator(YAC_OP_ASSIGN, _o, &r);
      r.unset();
   }
}

void YAC_VCALL _Vector4f::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector4f))
   {
      if(YAC_BCHK(_max, clid_Vector4f))
      {
         YAC_CAST_ARG(_Vector4f, min, _min);
         YAC_CAST_ARG(_Vector4f, max, _max);
         for(sUI i = 0u; i < 4u; i++)
         {
            if(floats[i] < min->floats[i])
               floats[i] = min->floats[i];
            else if(floats[i] > max->floats[i])
               floats[i] = max->floats[i];
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector4f::yacOperatorClamp: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector4f::yacOperatorClamp: invalid _min object.");
   }
}

void YAC_VCALL _Vector4f::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector4f))
   {
      if(YAC_BCHK(_max, clid_Vector4f))
      {
         YAC_CAST_ARG(_Vector4f, min, _min);
         YAC_CAST_ARG(_Vector4f, max, _max);
         for(sUI i = 0u; i < 4u; i++)
         {
            if(floats[i] < min->floats[i])
               floats[i] += max->floats[i] - min->floats[i];
            else if(floats[i] > max->floats[i])
               floats[i] -= max->floats[i] - min->floats[i];
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector4f::yacOperatorWrap: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector4f::yacOperatorWrap: invalid _min object.");
   }
}

void YAC_VCALL _Vector4f::yacOperator(sSI _cmd,YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      switch(_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI(Dfltequal(floats[0], o->floats[0]) &&
                     Dfltequal(floats[1], o->floats[1]) &&
                     Dfltequal(floats[2], o->floats[2]) &&
                     Dfltequal(floats[3], o->floats[3])
                     );
            break;

         case YAC_OP_CNE:
            YAC_RETI(Dfltnotequal(floats[0], o->floats[0]) ||
                     Dfltnotequal(floats[1], o->floats[1]) ||
                     Dfltnotequal(floats[2], o->floats[2]) ||
                     Dfltnotequal(floats[3], o->floats[3])
                     );
            break;

         case YAC_OP_ASSIGN:
            floats[0] = o->floats[0];
            floats[1] = o->floats[1];
            floats[2] = o->floats[2];
            floats[3] = o->floats[3];
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
            floats[3] = -floats[3];
            break;

         case YAC_OP_INIT:
            floats[0] = 0.0f;
            floats[1] = 0.0f;
            floats[2] = 0.0f;
            floats[3] = 1.0f;
            break;

         default:
         {
            // Should never ever happen !
            char buf[256];
            Dyac_snprintf(buf, 256, "tkmath::Vector4f::yacOperator unsupported cmd %d", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
         }
         return;
      }
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
               // Create temporary Vector4f object on stack
               _Vector4f t;
               t.class_ID = clid_Vector4f;
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
               /* _Vector4f t;
               t.class_ID = clid_Vector4f;
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
            floats[3] = -floats[3];
            break;

         default:
            // Call generic yacOperator, e.g. for <null> comparisons
            YAC_Object::yacOperator(_cmd, _o, _r);
      }
   }
}

void _Vector4f::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF32(_cmd, (sF32)val, _r);
}

void _Vector4f::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_ASSIGN:
         _init(val, 0.0f, 0.0f, 1.0f);
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
         if(Dfltnotequal(val, 0))
         {
            _mulf_YAC_RSELF(1.0f / val);
         }
         else
         {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4f::yacOperatorF32 OP_DIV Cannot divide by zero");
         }
         break;

      default:
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Vector4f::yacOperatorF32 Operator not supported %d", _cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
         break;
   }
}

void _Vector4f::assignFloats(const sF32 *_fv, sUI _ne) {
   if(4u == _ne)
   {
      // Copy four elements
      floats[0] = _fv[0];
      floats[1] = _fv[1];
      floats[2] = _fv[2];
      floats[3] = _fv[3];
   }
   else
   {
      // Copy as much elements from array as possible but never more than 4
      sUI i;
      for(i = 0u; i < _ne; i++)
         floats[i] = _fv[i];

      // Fill the rest with 0.0f / set w to 1.0
      for(; i < 4u; i++)
         floats[i] = (3u == i) ? 1.0f : 0.0f;
   }
}

void _Vector4f::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Vector4f::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
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
            for(i = 0u; i < 4u && i < ne; i++)
            {
               _o->yacArrayGet(_ctx, i, &v);
               floats[i] = v.value.float_val;
            }

            // Fill the rest with 0.0f / set w to 1.0
            for(; i < 4u; i++)
               floats[i] = (3u == i) ? 1.0f : 0.0f;
         }
      }
      else
      {
         // Retrieve individual elements, typecast to float and assign
         YAC_Value v;
         sUI i;
         for(i = 0u; i < 4u && i < ne; i++)
         {
            _o->yacArrayGet(_ctx, i, &v);
            v.typecast(YAC_TYPE_FLOAT);
            floats[i] = v.value.float_val;
            v.unset();
         }
         // Fill the rest with 0.0f / set w to 1.0
         for(; i < 4u; i++)
            floats[i] = (3u == i) ? 1.0f : 0.0f;
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      floats[0] = floats[1] = floats[2] = 0.0f;
      floats[3] = 1.0f;
   }
}

void _Vector4f::_init(sF32 _x, sF32 _y, sF32 _z, sF32 _w) {
   floats[0] = _x;
   floats[1] = _y;
   floats[2] = _z;
   floats[3] = _w;
}

void _Vector4f::_setX(sF32 x) {
   floats[0] = x;
}

void _Vector4f::_setY(sF32 y) {
   floats[1] = y;
}

void _Vector4f::_setZ(sF32 z) {
   floats[2] = z;
}

void _Vector4f::_setW(sF32 w) {
   floats[3] = w;
}

sF32 _Vector4f::_getX(void) {
   return floats[0];
}

sF32 _Vector4f::_getY(void) {
   return floats[1];
}

sF32 _Vector4f::_getZ(void) {
   return floats[2];
}

sF32 _Vector4f::_getW(void) {
   return floats[3];
}

void _Vector4f::copytoobject_helper(YAC_Object *_o, const sUI _numComp, const sUI _idx0, const sUI _idx1, const sUI _idx2, const sUI _idx3) const {
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
            v.value.float_val = (_numComp >= 4u) ? floats[_idx3] : 1.0f;
            _o->yacArraySet(NULL/*context*/, 3u, &v);
         }
      }
   }
}

void _Vector4f::_getXy_YAC_RVAL(YAC_Value *_r) const {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   r->_init(floats[0], floats[1]);
}

void _Vector4f::_getXy_YAC_RARG(YAC_Object *_o) const {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      o->_init(floats[0], floats[1]);
   }
   else if(YAC_VALID(_o))
   {
      copytoobject_helper(_o, 2u, 0u, 1u, 0u, 0u);
   }
}

void _Vector4f::_getXz_YAC_RVAL(YAC_Value *_r) const {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   r->_init(floats[0], floats[1]);
}

void _Vector4f::_getXz_YAC_RARG(YAC_Object *_o) const {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      o->_init(floats[0], floats[2]);
   }
   else if(YAC_VALID(_o))
   {
      copytoobject_helper(_o, 2u, 0u, 2u, 0u, 0u);
   }
}

void _Vector4f::_getYz_YAC_RVAL(YAC_Value *_r) const {
   _Vector2f *r = YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   r->_init(floats[0], floats[2]);
}

void _Vector4f::_getYz_YAC_RARG(YAC_Object *_o) const {
   if(YAC_BCHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      o->_init(floats[1], floats[2]);
   }
   else if(YAC_VALID(_o))
   {
      copytoobject_helper(_o, 2u, 1u, 2u, 0u, 0u);
   }
}

// void _Vector4f::_getXyz_YAC_RSELF(void) const {
// }

void _Vector4f::_getXyz_YAC_RVAL(YAC_Value *_r) const {
   _Vector3f *r = YAC_NEW_POOLED(Vector3f);
   _r->initObject(r, YAC_TRUE);
   r->_init(floats[0], floats[1], floats[2]);
}

void _Vector4f::_getXyz_YAC_RARG(YAC_Object *_o) const {
   if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      o->_init(floats[0], floats[1], floats[2]);
   }
   else if(YAC_VALID(_o))
   {
      copytoobject_helper(_o, 3u, 0u, 1u, 2u, 0u);
   }
}

sBool YAC_VCALL _Vector4f::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(256, 0,0,0);
   s->printf("(%g, %g, %g, %g)", floats[0], floats[1], floats[2], floats[3]);
   s->fixLength();
   return YAC_TRUE;
}

void _Vector4f::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   YAC_RETS(s, YAC_TRUE);
   yacToString(s);
}

void _Vector4f::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      floats[0] += o->floats[0];
      floats[1] += o->floats[1];
      floats[2] += o->floats[2];
      floats[3] += o->floats[3];
   }
   else if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      floats[0] += o->floats[0];
      floats[1] += o->floats[1];
      floats[2] += o->floats[2];
   }
   else if(YAC_VALID(_o))
   {
      YAC_NEW_STACK(Vector4f, o);
      o.assignGeneric(NULL/*context*/, _o);
      floats[0] += o.floats[0];
      floats[1] += o.floats[1];
      floats[2] += o.floats[2];
      floats[3] += o.floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::add_RSELF: invalid arg object");
   }
}

void _Vector4f::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Vector4f *r = YAC_NEW_POOLED(Vector4f);
   _r->initObject(r, YAC_TRUE);
   if(YAC_BCHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      r->floats[0] = floats[0] + o->floats[0];
      r->floats[1] = floats[1] + o->floats[1];
      r->floats[2] = floats[2] + o->floats[2];
      r->floats[3] = floats[3] + o->floats[3];
   }
   else if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      _Vector4f *r = YAC_NEW_POOLED(Vector4f);
      r->floats[0] = floats[0] + o->floats[0];
      r->floats[1] = floats[1] + o->floats[1];
      r->floats[2] = floats[2] + o->floats[2];
      r->floats[3] = floats[3] + 1.0f;
      _r->initObject(r, YAC_TRUE);
   }
   else if(YAC_VALID(_o))
   {
      YAC_NEW_STACK(Vector4f, o);
      o.assignGeneric(NULL/*context*/, _o);
      r->floats[0] = floats[0] + o.floats[0];
      r->floats[1] = floats[1] + o.floats[1];
      r->floats[2] = floats[2] + o.floats[2];
      r->floats[3] = floats[3] + o.floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::add_RVAL: invalid arg object");
   }
}

void _Vector4f::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, r, _r);

      if(YAC_BCHK(_o, clid_Vector4f))
      {
         YAC_CAST_ARG(_Vector4f, o, _o);
         r->floats[0] = floats[0] + o->floats[0];
         r->floats[1] = floats[1] + o->floats[1];
         r->floats[2] = floats[2] + o->floats[2];
         r->floats[3] = floats[3] + o->floats[3];
      }
      else if(YAC_BCHK(_o, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, o, _o);
         YAC_CAST_ARG(_Vector4f, r, _r);
         r->floats[0] = floats[0] + o->floats[0];
         r->floats[1] = floats[1] + o->floats[1];
         r->floats[2] = floats[2] + o->floats[2];
         r->floats[3] = floats[3] + 1.0f;
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector4f, o);
         o.assignGeneric(NULL/*context*/, _o);
         r->floats[0] = floats[0] + o.floats[0];
         r->floats[1] = floats[1] + o.floats[1];
         r->floats[2] = floats[2] + o.floats[2];
         r->floats[3] = floats[3] + o.floats[3];
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::add_RARG: invalid arg object");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::add_RARG: return object is not a Vector4f");
   }
}

void _Vector4f::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      floats[0] -= o->floats[0];
      floats[1] -= o->floats[1];
      floats[2] -= o->floats[2];
      floats[3] -= o->floats[3];
   }
   else if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      floats[0] -= o->floats[0];
      floats[1] -= o->floats[1];
      floats[2] -= o->floats[2];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::sub_RSELF: invalid arg object");
   }
}

void _Vector4f::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      _Vector4f *r = YAC_NEW_POOLED(Vector4f);
      _r->initObject(r, YAC_TRUE);
      r->floats[0] = floats[0] - o->floats[0];
      r->floats[1] = floats[1] - o->floats[1];
      r->floats[2] = floats[2] - o->floats[2];
      r->floats[3] = floats[3] - o->floats[3];
   }
   else if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      _Vector4f *r = YAC_NEW_POOLED(Vector4f);
      _r->initObject(r, YAC_TRUE);
      r->floats[0] = floats[0] - o->floats[0];
      r->floats[1] = floats[1] - o->floats[1];
      r->floats[2] = floats[2] - o->floats[2];
      r->floats[3] = floats[3] - 1.0f;
   }
   else if(YAC_VALID(_o))
   {
      YAC_NEW_STACK(Vector4f, o);
      o.assignGeneric(NULL/*context*/, _o);
      floats[0] -= o.floats[0];
      floats[1] -= o.floats[1];
      floats[2] -= o.floats[2];
      floats[3] -= o.floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::sub_RVAL: invalid arg object");
   }
}

void _Vector4f::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, r, _r);

      if(YAC_BCHK(_o, clid_Vector4f))
      {
         YAC_CAST_ARG(_Vector4f, o, _o);
         YAC_CAST_ARG(_Vector4f, r, _r);
         r->floats[0] = floats[0] - o->floats[0];
         r->floats[1] = floats[1] - o->floats[1];
         r->floats[2] = floats[2] - o->floats[2];
         r->floats[3] = floats[3] - o->floats[3];
      }
      else if(YAC_BCHK(_o, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, o, _o);
         r->floats[0] = floats[0] - o->floats[0];
         r->floats[1] = floats[1] - o->floats[1];
         r->floats[2] = floats[2] - o->floats[2];
         r->floats[3] = floats[3] - 1.0f;
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector4f, o);
         o.assignGeneric(NULL/*context*/, _o);
         r->floats[0] = floats[0] - o.floats[0];
         r->floats[1] = floats[1] - o.floats[1];
         r->floats[2] = floats[2] - o.floats[2];
         r->floats[3] = floats[3] - o.floats[3];
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::sub_RARG: invalid arg object");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::sub_RARG: return object is not a Vector4f");
   }
}

sF32 _Vector4f::_dot(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      return
         floats[0] * o->floats[0] +
         floats[1] * o->floats[1] +
         floats[2] * o->floats[2] +
         floats[3] * o->floats[3] ;
   }
   else if(YAC_VALID(_o))
   {
      YAC_NEW_STACK(Vector4f, o);
      o.assignGeneric(NULL/*context*/, _o);
      return
         floats[0] * o.floats[0] +
         floats[1] * o.floats[1] +
         floats[2] * o.floats[2] +
         floats[3] * o.floats[3] ;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::dot: invalid arg object");
   }
   return 0.0f;
}

void _Vector4f::_tensor_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   // return the tensor product of two vectors
   if(YAC_BCHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
      _r->initObject(m, YAC_TRUE);
      // (todo) review row/column order
      m->floats[ 0] = floats[0] * o->floats[0];
      m->floats[ 1] = floats[0] * o->floats[1];
      m->floats[ 2] = floats[0] * o->floats[2];
      m->floats[ 3] = floats[0] * o->floats[3];
      m->floats[ 4] = floats[1] * o->floats[0];
      m->floats[ 5] = floats[1] * o->floats[1];
      m->floats[ 6] = floats[1] * o->floats[2];
      m->floats[ 7] = floats[1] * o->floats[3];
      m->floats[ 8] = floats[2] * o->floats[0];
      m->floats[ 9] = floats[2] * o->floats[1];
      m->floats[10] = floats[2] * o->floats[2];
      m->floats[11] = floats[2] * o->floats[3];
      m->floats[12] = floats[3] * o->floats[0];
      m->floats[13] = floats[3] * o->floats[1];
      m->floats[14] = floats[3] * o->floats[2];
      m->floats[15] = floats[3] * o->floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4f::tensor_RVAL: invalid Vector4f object");
   }
}

void _Vector4f::_tensor_YAC_RARG(YAC_Object *_o, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      if(YAC_BCHK(_o, clid_Vector4f))
      {
         YAC_CAST_ARG(_Vector4f, o, _o);
         // (todo) review row/column order
         m->floats[ 0] = floats[0] * o->floats[0];
         m->floats[ 1] = floats[0] * o->floats[1];
         m->floats[ 2] = floats[0] * o->floats[2];
         m->floats[ 3] = floats[0] * o->floats[3];
         m->floats[ 4] = floats[1] * o->floats[0];
         m->floats[ 5] = floats[1] * o->floats[1];
         m->floats[ 6] = floats[1] * o->floats[2];
         m->floats[ 7] = floats[1] * o->floats[3];
         m->floats[ 8] = floats[2] * o->floats[0];
         m->floats[ 9] = floats[2] * o->floats[1];
         m->floats[10] = floats[2] * o->floats[2];
         m->floats[11] = floats[2] * o->floats[3];
         m->floats[12] = floats[3] * o->floats[0];
         m->floats[13] = floats[3] * o->floats[1];
         m->floats[14] = floats[3] * o->floats[2];
         m->floats[15] = floats[3] * o->floats[3];
      }
      else
      {
         /* _Vector4f t;
            t.assignGeneric(NULL, _o);
            o = &t;*/
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::tensor_RARG: invalid Vector4f Object");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4f::tensor_RARG: invalid Matrix4f object");
   }
}

void _Vector4f::_addf_YAC_RSELF(sF32 s) {
   floats[0] += s;
   floats[1] += s;
   floats[2] += s;
   floats[3] += s;
}

void _Vector4f::_addf_YAC_RVAL(sF32 s, YAC_Value *_r) {
   _Vector4f *r = YAC_NEW_POOLED(Vector4f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[0] + s;
   r->floats[1] = floats[1] + s;
   r->floats[2] = floats[2] + s;
   r->floats[3] = floats[3] + s;
}

void _Vector4f::_addf_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, r, _r);
      r->floats[0] = floats[0] + s;
      r->floats[1] = floats[1] + s;
      r->floats[2] = floats[2] + s;
      r->floats[3] = floats[3] + s;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4f::addf_RARG: return object is not a Vector4f");
   }
}

void _Vector4f::_mulf_YAC_RSELF(sF32 s) {
   floats[0] *= s;
   floats[1] *= s;
   floats[2] *= s;
   floats[3] *= s;
}

void _Vector4f::_mulf_YAC_RVAL(sF32 s, YAC_Value *_r) {
   _Vector4f *r = YAC_NEW_POOLED(Vector4f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[0] * s;
   r->floats[1] = floats[1] * s;
   r->floats[2] = floats[2] * s;
   r->floats[3] = floats[3] * s;
}

void _Vector4f::_mulf_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, r, _r);
      r->floats[0] = floats[0] * s;
      r->floats[1] = floats[1] * s;
      r->floats[2] = floats[2] * s;
      r->floats[3] = floats[3] * s;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4f::mulf_RARG Return object not of type Vector4f");
   }
}

void _Vector4f::_mul_YAC_RSELF(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      floats[0] *= o->floats[0];
      floats[1] *= o->floats[1];
      floats[2] *= o->floats[2];
      floats[3] *= o->floats[3];
   }
   else if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      floats[0] *= o->floats[0];
      floats[1] *= o->floats[1];
      floats[2] *= o->floats[2];
   }
   else if(YAC_VALID(_o))
   {
      YAC_NEW_STACK(Vector4f, o);
      o.assignGeneric(NULL/*context*/, _o);
      floats[0] *= o.floats[0];
      floats[1] *= o.floats[1];
      floats[2] *= o.floats[2];
      floats[3] *= o.floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::mul_RSELF: invalid arg object");
   }
}

void _Vector4f::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Vector4f *r = YAC_NEW_POOLED(Vector4f);
   _r->initObject(r, YAC_TRUE);

   if(YAC_BCHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      r->floats[0] = floats[0] * o->floats[0];
      r->floats[1] = floats[1] * o->floats[1];
      r->floats[2] = floats[2] * o->floats[2];
      r->floats[3] = floats[3] * o->floats[3];
   }
   else if(YAC_BCHK(_o, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      _Vector4f *r = YAC_NEW_POOLED(Vector4f);
      _r->initObject(r, YAC_TRUE);
      r->floats[0] = floats[0] * o->floats[0];
      r->floats[1] = floats[1] * o->floats[1];
      r->floats[2] = floats[2] * o->floats[2];
      r->floats[3] = floats[3];
   }
   else if(YAC_VALID(_o))
   {
      YAC_NEW_STACK(Vector4f, o);
      o.assignGeneric(NULL/*context*/, _o);
      r->floats[0] = floats[0] * o.floats[0];
      r->floats[1] = floats[1] * o.floats[1];
      r->floats[2] = floats[2] * o.floats[2];
      r->floats[3] = floats[3] * o.floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::mul_RVAL: invalid arg object");
   }
}

void _Vector4f::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, r, _r);

      if(YAC_BCHK(_o, clid_Vector4f))
      {
         YAC_CAST_ARG(_Vector4f, o, _o);
         r->floats[0] = floats[0] * o->floats[0];
         r->floats[1] = floats[1] * o->floats[1];
         r->floats[2] = floats[2] * o->floats[2];
         r->floats[3] = floats[3] * o->floats[3];
      }
      else if(YAC_BCHK(_o, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, o, _o);
         r->floats[0] = floats[0] * o->floats[0];
         r->floats[1] = floats[1] * o->floats[1];
         r->floats[2] = floats[2] * o->floats[2];
         r->floats[3] = floats[3];
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector4f, o);
         o.assignGeneric(NULL/*context*/, _o);
         r->floats[0] = floats[0] * o.floats[0];
         r->floats[1] = floats[1] * o.floats[1];
         r->floats[2] = floats[2] * o.floats[2];
         r->floats[3] = floats[3] * o.floats[3];
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::mul_RARG: invalid arg object");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::mul_RARG: return object is not a Vector4f");
   }
}

void _Vector4f::_transform_YAC_RSELF(YAC_Object *_m) {
   if(YAC_BCHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_mulv_YAC_RARG(this, this);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::transform_RSELF: \'m\' is not a Matrix4f");
   }
}

void _Vector4f::_transform_YAC_RVAL(YAC_Object *_m, YAC_Value *_r) {
   if(YAC_BCHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_mulv_YAC_RVAL(this, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::transform_RSELF: \'m\' is not a Matrix4f");
   }
}

void _Vector4f::_transform_YAC_RARG(YAC_Object *_m, YAC_Object *_r) {
   if(YAC_BCHK(_m, clid_Matrix4f))
   {
      if(YAC_BCHK(_r, clid_Vector4f))
      {
         YAC_CAST_ARG(_Matrix4f, m, _m);
         m->_mulv_YAC_RARG(this, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::transform_RARG: return object is not a Vector4f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4f::transform_RARG: \'m\' is not a Matrix4f");
   }
}

sF32 _Vector4f::_getAbs(void) const{
   // return length of vector
   return sqrtf(floats[0] * floats[0] +
                floats[1] * floats[1] +
                floats[2] * floats[2] +
                floats[3] * floats[3]
                );
}

sF32 _Vector4f::_getAbsSqr(void) const {
   // return squared length of vector
   return
      floats[0] * floats[0] +
      floats[1] * floats[1] +
      floats[2] * floats[2] +
      floats[3] * floats[3] ;
}

void _Vector4f::_unit_YAC_RSELF(void) {
   // scale vector to length 1
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = 1.0f / absval;
      floats[0] *= absval;
      floats[1] *= absval;
      floats[2] *= absval;
      floats[3] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4f::unit_RSELF: zero vector cannot be normalized");
      floats[0] = 0.0f;
      floats[1] = 0.0f;
      floats[2] = 0.0f;
      floats[3] = 1.0f;
   }
}

void _Vector4f::_unit_YAC_RVAL(YAC_Value *_r) {
   _Vector4f *r = YAC_NEW_POOLED(Vector4f);
   _r->initObject(r, YAC_TRUE);
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = 1.0f/absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
      r->floats[2] = floats[2] * absval;
      r->floats[3] = floats[3] * absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4f::unit_RVAL: zero vector cannot be normalized");
      r->floats[0] = 0.0f;
      r->floats[1] = 0.0f;
      r->floats[2] = 0.0f;
      r->floats[3] = 1.0f;
   }
}

void _Vector4f::_unit_YAC_RARG(YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, r, _r);
      sF32 absval = _getAbs();
      if(Dfltnonzero(absval))
      {
         absval = 1.0f / absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
         r->floats[2] = floats[2] * absval;
         r->floats[3] = floats[3] * absval;
      }
      else
      {
         // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4f::unit_RARG: zero vector cannot be normalized");
         r->floats[0] = 0.0f;
         r->floats[1] = 0.0f;
         r->floats[2] = 0.0f;
         r->floats[3] = 1.0f;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4f::unit_RARG: return object is not a Vector4f");
   }
}

void _Vector4f::_unitScale_YAC_RSELF(sF32 s) {
   // scale vector to length s
   sF32 absval = _getAbs();
   if (Dfltnonzero(absval))
   {
      absval = s / absval;
      floats[0] *= absval;
      floats[1] *= absval;
      floats[2] *= absval;
      floats[3] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4f::unitScale_RSELF: zero vector cannot be normalized");
      floats[0] = 0.0f;
      floats[1] = 0.0f;
      floats[2] = 0.0f;
      floats[3] = 1.0f;
   }
}

void _Vector4f::_unitScale_YAC_RVAL(sF32 s, YAC_Value *_r) {
   _Vector4f *r = YAC_NEW_POOLED(Vector4f);
   _r->initObject(r, YAC_TRUE);
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = s / absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
      r->floats[2] = floats[2] * absval;
      r->floats[3] = floats[3] * absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4f::unitScale_RVAL: zero vector cannot be normalized");
      r->floats[0] = 0.0f;
      r->floats[1] = 0.0f;
      r->floats[2] = 0.0f;
      r->floats[3] = 1.0f;
   }
}

void _Vector4f::_unitScale_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if(YAC_BCHK(_r, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, r, _r);
      sF32 absval = _getAbs();
      if(Dfltnonzero(absval))
      {
         absval = s / absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
         r->floats[2] = floats[2] * absval;
         r->floats[3] = floats[3] * absval;
      }
      else
      {
         // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4f::unitScale_RARG: zero vector cannot be normalized");
         r->floats[0] = 0.0f;
         r->floats[1] = 0.0f;
         r->floats[2] = 0.0f;
         r->floats[3] = 1.0f;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4f::unitScale_RARG: return object is not a Vector4f");
   }
}

void _Vector4f::_initScalef(YAC_Object *_o, sF32 _s) {
   if(YAC_BCHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      floats[0] = o->floats[0] * _s;
      floats[1] = o->floats[1] * _s;
      floats[2] = o->floats[2] * _s;
      floats[3] = o->floats[3] * _s;
   }
   else if(YAC_VALID(_o))
   {
      YAC_NEW_STACK(Vector4f, v);
      v.assignGeneric(NULL/*context*/, _o);
      floats[0] = v.floats[0] * _s;
      floats[1] = v.floats[1] * _s;
      floats[2] = v.floats[2] * _s;
      floats[3] = v.floats[3] * _s;
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector4f::initScalef: \'o\' is not a Vector4f");
   }
}

void _Vector4f::_rotate(YAC_Object *_m) {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      YAC_NEW_STACK(Vector4f, t);
      t.yacOperatorAssign(this);
      m->_mulv_YAC_RARG(&t, this);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4f::rotate: arg #1 is not a valid Matrix4f object");
   }
}

sF32 _Vector4f::_distanceToPlane(YAC_Object *_q, YAC_Object *_n) const {
   if(YAC_BCHK(_q, clid_Vector4f))
   {
      if(YAC_BCHK(_n, clid_Vector4f))
      {
         YAC_CAST_ARG(_Vector4f, q, _q);
         YAC_CAST_ARG(_Vector4f, n, _n);
         YAC_NEW_STACK(Vector4f, v);
         _sub_YAC_RARG(q, &v);
         sF32 nAbs = n->_getAbs();
         return (0.0f != nAbs) ? (v._dot(n) / nAbs) : 999999999.0f;
      }
   }
	return 0.0f;
}

sBool _Vector4f::_intersectPlane(YAC_Object *_q, YAC_Object *_n, YAC_Object *_p1, YAC_Object *_p2) {
   // <https://stackoverflow.com/questions/5666222/3d-line-plane-intersection>
   //   double t = (planeNormal.dot(planePoint) - planeNormal.dot(linePoint)) / planeNormal.dot(lineDirection.normalize());
   //   return linePoint.plus(lineDirection.normalize().scale(t));
   if(YAC_BCHK(_q,  clid_Vector4f) &&
      YAC_BCHK(_n,  clid_Vector4f) &&
      YAC_BCHK(_p1, clid_Vector4f) &&
      YAC_BCHK(_p2, clid_Vector4f)
      )
   {
      YAC_CAST_ARG(_Vector4f, q,  _q );
      YAC_CAST_ARG(_Vector4f, n,  _n );
      YAC_CAST_ARG(_Vector4f, p1, _p1);
      YAC_CAST_ARG(_Vector4f, p2, _p2);

      _Vector4f pDir; pDir.diffUnit(p1, p2);  // pDir = normalize(p2 - p1)

      const sF32 dotNQ = Dot(n, q);
      const sF32 dotN1 = Dot(n, p1);
      const sF32 dotND = Dot(n, &pDir);

      if(0.0f != dotND)
      {
         const sF32 t = (dotNQ - dotN1) / dotND;
         floats[0] = p1->floats[0] + pDir.floats[0] * t;
         floats[1] = p1->floats[1] + pDir.floats[1] * t;
         floats[2] = p1->floats[2] + pDir.floats[2] * t;
         floats[3] = p1->floats[3] + pDir.floats[3] * t;
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}


void _Vector4f::_divz_YAC_RSELF(void) {
   if(0.0f != floats[2])
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

void _Vector4f::_divz_YAC_RVAL(YAC_Value *_r) const {
   _Vector2f *r = (_Vector2f*)YAC_NEW_POOLED(Vector2f);
   _r->initObject(r, YAC_TRUE);
   if(0.0f != floats[2])
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

void _Vector4f::_divz_YAC_RARG(YAC_Object *_r) {
   if(YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, r, _r);
      if(0.0f != floats[2])
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

void _Vector4f::_divw_YAC_RSELF(void) {
   if(0.0f != floats[3])
   {
      floats[0] = floats[0] / floats[3];
      floats[1] = floats[1] / floats[3];
      floats[2] = floats[2] / floats[3];
      floats[3] = 1.0f;
   }
   else
   {
      floats[0] = 0.0f;
      floats[1] = 0.0f;
      floats[2] = 0.0f;
      floats[3] = 1.0f;
   }
}

void _Vector4f::_divw_YAC_RVAL(YAC_Value *_r) const {
   _Vector3f *r = (_Vector3f*)YAC_NEW_POOLED(Vector3f);
   _r->initObject(r, YAC_TRUE);
   if(0.0f != floats[3])
   {
      r->floats[0] = floats[0] / floats[3];
      r->floats[1] = floats[1] / floats[3];
      r->floats[2] = floats[2] / floats[3];
   }
   else
   {
      r->floats[0] = 0.0f;
      r->floats[1] = 0.0f;
      r->floats[2] = 0.0f;
   }
}

void _Vector4f::_divw_YAC_RARG(YAC_Object *_r) {
   if(YAC_BCHK(_r, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, r, _r);
      if(0.0f != floats[3])
      {
         r->floats[0] = floats[0] / floats[3];
         r->floats[1] = floats[1] / floats[3];
         r->floats[2] = floats[2] / floats[3];
      }
      else
      {
         r->floats[0] = 0.0f;
         r->floats[1] = 0.0f;
         r->floats[2] = 0.0f;
      }
   }
}

void _Vector4f::_bilinearQuadPos(YAC_Object *_vLT, YAC_Object *_vRT, YAC_Object *_vRB, YAC_Object *_vLB, sF32 _nx, sF32 _ny) {
   if(YAC_BCHK(_vLT, clid_Vector4f) &&
      YAC_BCHK(_vRT, clid_Vector4f) &&
      YAC_BCHK(_vRB, clid_Vector4f) &&
      YAC_BCHK(_vLB, clid_Vector4f)
      )
   {
      YAC_CAST_ARG(_Vector4f, vLT, _vLT);
      YAC_CAST_ARG(_Vector4f, vRT, _vRT);
      YAC_CAST_ARG(_Vector4f, vRB, _vRB);
      YAC_CAST_ARG(_Vector4f, vLB, _vLB);

      const float tx = vLT->floats[0] + (vRT->floats[0] - vLT->floats[0]) * _nx;
      const float ty = vLT->floats[1] + (vRT->floats[1] - vLT->floats[1]) * _nx;
      const float tz = vLT->floats[2] + (vRT->floats[2] - vLT->floats[2]) * _nx;
      const float tw = vLT->floats[3] + (vRT->floats[3] - vLT->floats[3]) * _nx;

      const float bx = vLB->floats[0] + (vRB->floats[0] - vLB->floats[0]) * _nx;
      const float by = vLB->floats[1] + (vRB->floats[1] - vLB->floats[1]) * _nx;
      const float bz = vLB->floats[2] + (vRB->floats[2] - vLB->floats[2]) * _nx;
      const float bw = vLB->floats[3] + (vRB->floats[3] - vLB->floats[3]) * _nx;

      floats[0] = tx + (bx - tx) * _ny;
      floats[1] = ty + (by - ty) * _ny;
      floats[2] = tz + (bz - tz) * _ny;
      floats[3] = tw + (bw - tw) * _ny;
   }
}

void _Vector4f::_project(YAC_Object *_r, sF32 _vpX, sF32 _vpY, sF32 _vpW, sF32 _vpH, sF32 _depthRangeNear, sF32 _depthRangeFar) const {
   // Convert clip coordinates to window coordinates
   if(YAC_BCHK(_r, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, r, _r);
      if(0.0f != floats[3])
      {
         const sF32 sw = 0.5f * _vpW;
         const sF32 sh = 0.5f * _vpH;
         const sF32 obw  = 1.0f / floats[3];

         r->floats[0] = (sw * floats[0]) * obw;
         r->floats[1] = (sh * floats[1]) * obw;

         r->floats[0] = _vpX + sw + r->floats[0];
         r->floats[1] = _vpY + sh + r->floats[1];
      }
      else
      {
         r->floats[0] = 0.0f;
         r->floats[1] = 0.0f;
      }
   }
   else if(YAC_BCHK(_r, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, r, _r);
      if(0.0f != floats[3])
      {
         const sF32 sw = 0.5f * _vpW;
         const sF32 sh = 0.5f * _vpH;
         const sF32 sd = 0.5f * (_depthRangeFar - _depthRangeNear);
         const sF32 obw  = 1.0f / floats[3];

         r->floats[0] = (sw * floats[0]) * obw;
         r->floats[1] = (sh * floats[1]) * obw;
         r->floats[2] = (sd * floats[2]) * obw;

         r->floats[0] = _vpX + sw + r->floats[0];
         r->floats[1] = _vpY + sh + r->floats[1];
         r->floats[2] =        sd + r->floats[2];
      }
      else
      {
         r->floats[0] = 0.0f;
         r->floats[1] = 0.0f;
         r->floats[2] = 0.0f;
      }
   }
   else if(YAC_BCHK(_r, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, r, _r);
      if(0.0f != floats[3])
      {
         const sF32 sw = 0.5f * _vpW;
         const sF32 sh = 0.5f * _vpH;
         const sF32 sd = 0.5f * (_depthRangeFar - _depthRangeNear);
         const sF32 obw  = 1.0f / floats[3];

         r->floats[0] = (sw * floats[0]) * obw;
         r->floats[1] = (sh * floats[1]) * obw;
         r->floats[2] = (sd * floats[2]) * obw;

         r->floats[0] = _vpX + sw + r->floats[0];
         r->floats[1] = _vpY + sh + r->floats[1];
         r->floats[2] =        sd + r->floats[2];
         r->floats[3] = obw;
      }
      else
      {
         r->floats[0] = 0.0f;
         r->floats[1] = 0.0f;
         r->floats[2] = 0.0f;
         r->floats[3] = 1.0f;
      }
   }
}

void _Vector4f::_New(sF32 _x, sF32 _y, sF32 _z, sF32 _w, YAC_Value *_r) {
   _Vector4f *r = YAC_NEW_POOLED(Vector4f);
   _r->initObject(r, YAC_TRUE);
   r->_init(_x, _y, _z, _w);
}

void YAC_CALL _vector4f(sF32 _x, sF32 _y, sF32 _z, sF32 _w, YAC_Value *_r) {
   _Vector4f *r = YAC_NEW_POOLED(Vector4f);
   _r->initObject(r, YAC_TRUE);
   r->_init(_x, _y, _z, _w);
}
