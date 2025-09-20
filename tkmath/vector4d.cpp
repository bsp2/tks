/// vector4d.cpp
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
#include "vector4d.h"
#include "matrix4d.h"

#include "ying_Math.h"

_Vector4d::_Vector4d(void) {
   doubles[0] = doubles[1] = doubles[2] = 0.0;
   doubles[3] = 1.0;
}

_Vector4d::~_Vector4d() {
}

sUI _Vector4d::yacArrayGetNumElements(void) {
   return 4u;
}

sUI _Vector4d::yacArrayGetMaxElements(void) {
   return 4u;
}

sUI _Vector4d::yacArrayGetElementType(void) {
   return YAC_TYPE_OBJECT;
}

sUI _Vector4d::yacArrayGetElementByteSize(void) {
   return sizeof(sF64);
}

void YAC_VCALL _Vector4d::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4u)
   {
      sF64 v;
      switch (_val->type)
      {
         case YAC_TYPE_VOID:
            v = 0.0;
            break;

         case YAC_TYPE_INT:
            v = (sF64) _val->value.int_val;
            break;

         case YAC_TYPE_FLOAT:
            v = (sF64) _val->value.float_val;
            break;

         case YAC_TYPE_OBJECT:
            if(YAC_VALID(_val->value.object_val))
            {
               if(!_val->value.object_val->yacScanF64(&v))
                  v = 0.0;
            }
            else
            {
               // Silently cast <null> to 0.0f
               v = 0.0;
            }
            break;

         default:
            // Should never ever happen!
            Dyac_throw(TypeMismatch, "tkmath::Vector3f::yacArraySet: unknown type");
            v = 0.0;
            break;
      }
      doubles[_idx] = v;
   }
   else
   {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Vector4d::yacArraySet Index too high (%u>3)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Vector4d::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4u)
   {
      YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
      _val->initObject(d, YAC_TRUE);
      d->value = doubles[_idx];
   }
   else
   {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Vector4d::yacArrayGet: Index too high (%u>3)", _idx);
      _val->initFloat(0.0f);
      Dyac_throw(ReadArrayOutOfBounds, buf);
   }
}

void *YAC_VCALL _Vector4d::yacArrayGetPointer(void) {
   return (void*)doubles;
}

sSI YAC_VCALL _Vector4d::yacTensorRank(void) {
   return YAC_TENSOR_RANK_VECTOR;
}

void YAC_VCALL _Vector4d::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F64(doubles[0]);
   YAC_SERIALIZE_F64(doubles[1]);
   YAC_SERIALIZE_F64(doubles[2]);
   YAC_SERIALIZE_F64(doubles[3]);
}

sUI YAC_VCALL _Vector4d::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   doubles[0] = YAC_DESERIALIZE_F64();
   doubles[1] = YAC_DESERIALIZE_F64();
   doubles[2] = YAC_DESERIALIZE_F64();
   doubles[3] = YAC_DESERIALIZE_F64();
   return sUI(YAC_TRUE);
}

void YAC_VCALL _Vector4d::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector4d))
   {
      if(YAC_BCHK(_max, clid_Vector4d))
      {
         YAC_CAST_ARG(_Vector4d, min, _min);
         YAC_CAST_ARG(_Vector4d, max, _max);
         for(sUI i = 0u; i < 4u; i++)
         {
            if(doubles[i] < min->doubles[i])
               doubles[i] = min->doubles[i];
            else if(doubles[i] > max->doubles[i])
               doubles[i] = max->doubles[i];
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector4d::yacOperatorClamp: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector4d::yacOperatorClamp: invalid _min object.");
   }
}

void YAC_VCALL _Vector4d::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector4d))
   {
      if(YAC_BCHK(_max, clid_Vector4d))
      {
         YAC_CAST_ARG(_Vector4d, min, _min);
         YAC_CAST_ARG(_Vector4d, max, _max);
         for(sUI i = 0u; i < 4u; i++)
         {
            if(doubles[0] < min->doubles[0])
               doubles[0] += max->doubles[0] - min->doubles[0];
            else if(doubles[0] > max->doubles[0])
               doubles[0] -= max->doubles[0] - min->doubles[0];
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector4d::yacOperatorWrap: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector4d::yacOperatorWrap: invalid _min object.");
   }
}

void YAC_VCALL _Vector4d::yacOperator(sSI _cmd,YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      switch (_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI(Ddblequal(doubles[0], o->doubles[0]) &&
                     Ddblequal(doubles[1], o->doubles[1]) &&
                     Ddblequal(doubles[2], o->doubles[2]) &&
                     Ddblequal(doubles[3], o->doubles[3])
                     );
            break;

         case YAC_OP_CNE:
            YAC_RETI(Ddblnotequal(doubles[0], o->doubles[0]) ||
                     Ddblnotequal(doubles[1], o->doubles[1]) ||
                     Ddblnotequal(doubles[2], o->doubles[2]) ||
                     Ddblnotequal(doubles[3], o->doubles[3])
                     );
            break;

         case YAC_OP_ASSIGN:
            doubles[0] = o->doubles[0];
            doubles[1] = o->doubles[1];
            doubles[2] = o->doubles[2];
            doubles[3] = o->doubles[3];
            break;

         case YAC_OP_ADD:
            _add_YAC_RSELF(_o);
            break;

         case YAC_OP_SUB:
            _sub_YAC_RSELF(_o);
            break;

         // case YAC_OP_MUL:
         //    _mul(_o);
         //    break;

         case YAC_OP_NEG:
            doubles[0] = -doubles[0];
            doubles[1] = -doubles[1];
            doubles[2] = -doubles[2];
            doubles[3] = -doubles[3];
            break;

         case YAC_OP_INIT:
            doubles[0] = 0.0;
            doubles[1] = 0.0;
            doubles[2] = 0.0;
            doubles[3] = 0.0;
            break;

         default:
         {
            // Should not be reachable
            char buf[256];
            Dyac_snprintf(buf, 256, "tkmath::Vector4d::yacOperator unsupported cmd %d", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
         }
         return;
      }
   }
   else if(YAC_VALID(_o))
   {
      sF64 val;
      if(_o->yacScanF64(&val))
      {
         yacOperatorF64(_cmd, val, _r);
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
               // Create temporary Vector4d object on stack
               _Vector4d t;
               t.class_ID = clid_Vector4d;
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
               /* _Vector4d t;
               t.class_ID = clid_Vector4d;
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
            doubles[0] = -doubles[0];
            doubles[1] = -doubles[1];
            doubles[2] = -doubles[2];
            doubles[3] = -doubles[3];
            break;

         default:
            // Call generic yacOperator, e.g. for <null> comparisons
            YAC_Object::yacOperator(_cmd, _o, _r);
            break;
      }
   }
}

void _Vector4d::yacOperatorI(sSI _cmd, sSI _val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)_val, _r);
}

void _Vector4d::yacOperatorF32(sSI _cmd, sF32 _val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)_val, _r);
}

void _Vector4d::yacOperatorF64(sSI _cmd, sF64 _val, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_MUL:
         muld(_val);
         break;

      case YAC_OP_DIV:
         if(Ddblnonzero(_val)) {
            muld(1.0 / _val);
         }
         else
         {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4d::yacOperatorF64 OP_DIV Cannot divide by zero");
         }
         break;

      default:
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Vector4d::yacOperatorF64 Operator not supported %d", _cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
   }
}

void _Vector4d::assignFloats(const sF32 *_fv, sUI _ne) {
   if(_ne <= 4u)
   {
      // Copy as much elements from array as possible but never more than 4
      sUI i;
      for(i = 0u; i < _ne; i++)
         doubles[i] = (sF64)_fv[i];

      // Fill the rest with 0.0f
      for(; i < 4u; i++)
         doubles[i] = 0.0;
   }
   else
   {
      // Copy four elements
      doubles[0] = (sF64)_fv[0];
      doubles[1] = (sF64)_fv[1];
      doubles[2] = (sF64)_fv[2];
      doubles[3] = (sF64)_fv[3];
   }
}

void _Vector4d::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Vector4d::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
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
               doubles[i] = (sF64) v.value.float_val;
            }

            // Fill the rest with 0.0f
            for(; i < 4u; i++)
               doubles[i] = 0.0;
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
            v.typecast(YAC_TYPE_OBJECT);
            if(!v.value.object_val->yacScanF64(&doubles[i]))
               doubles[i] = 0.0;
            v.unset();
         }

         // Fill the rest with 0.0f
         for(; i < 4u; i++)
            doubles[i] = 0.0;
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      doubles[0] = doubles[1] = doubles[2] = doubles[3] = 0.0;
   }
}

void _Vector4d::_setX(YAC_Object *_x) {
   if(YAC_VALID(_x))
   {
      if(!_x->yacScanF64(&doubles[0]))
         doubles[0] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::setX Object not valid");
   }
}

void _Vector4d::_setY(YAC_Object *_y) {
   if(YAC_VALID(_y))
   {
      if(!_y->yacScanF64(&doubles[1]))
         doubles[1] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::setY Object not valid");
   }
}

void _Vector4d::_setZ(YAC_Object *_z) {
   if(YAC_VALID(_z))
   {
      if(!_z->yacScanF64(&doubles[2]))
         doubles[2] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::setZ Object not valid");
   }
}

void _Vector4d::_setW(YAC_Object *_w) {
   if(YAC_VALID(_w)) {
      if(!_w->yacScanF64(&doubles[3]))
         doubles[3] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::setW Object not valid");
   }
}

void _Vector4d::_getX_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = doubles[0];
}

void _Vector4d::_getX_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[0]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::getX Return object not valid");
   }
}

void _Vector4d::_getY_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = doubles[1];
}

void _Vector4d::_getY_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[1]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::getY Return object not valid");
   }
}

void _Vector4d::_getZ_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = doubles[2];
}

void _Vector4d::_getZ_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[2]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::getZ Return object not valid");
   }
}

void _Vector4d::_getW_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = doubles[3];
}

void _Vector4d::_getW_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[3]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::getW Return object not valid");
   }
}

sBool YAC_VCALL _Vector4d::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(256, 0,0,0);
   s->printf("(%lg, %lg, %lg, %lg)", doubles[0], doubles[1], doubles[2], doubles[3]);
   s->fixLength();
   return YAC_TRUE;
}

void _Vector4d::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   YAC_RETS(s, YAC_TRUE);
   yacToString(s);
}

void _Vector4d::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      doubles[0] += o->doubles[0];
      doubles[1] += o->doubles[1];
      doubles[2] += o->doubles[2];
      doubles[3] += o->doubles[3];
   }
   else
   {
      /* // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4d::add_SELF Invalid Vector4d Object");
   }
}

void _Vector4d::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      _Vector4d *r = YAC_NEW_POOLED(Vector4d);
      r->doubles[0] = doubles[0] + o->doubles[0];
      r->doubles[1] = doubles[1] + o->doubles[1];
      r->doubles[2] = doubles[2] + o->doubles[2];
      r->doubles[3] = doubles[3] + o->doubles[3];
      _r->initObject(r,1);
   }
   else
   {
      /* // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4d::add_VAL Invalid Vector4d Object");
   }
}

void _Vector4d::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_BCHK(_o, clid_Vector4d) && YAC_BCHK(_r, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      YAC_CAST_ARG(_Vector4d, r, _r);
      r->doubles[0] = doubles[0] + o->doubles[0];
      r->doubles[1] = doubles[1] + o->doubles[1];
      r->doubles[2] = doubles[2] + o->doubles[2];
      r->doubles[3] = doubles[3] + o->doubles[3];
   }
   else
   {
      /* // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4d::add_ARG Invalid Vector4d Object");
   }
}

void _Vector4d::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      doubles[0] -= o->doubles[0];
      doubles[1] -= o->doubles[1];
      doubles[2] -= o->doubles[2];
      doubles[3] -= o->doubles[3];
   }
   else
   {
      /* // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_SUB, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4d::sub_SELF Invalid Vector4d Object");
   }
}

void _Vector4d::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      _Vector4d *r = YAC_NEW_POOLED(Vector4d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[0] - o->doubles[0];
      r->doubles[1] = doubles[1] - o->doubles[1];
      r->doubles[2] = doubles[2] - o->doubles[2];
      r->doubles[3] = doubles[3] - o->doubles[3];
   }
   else
   {
      /* // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_SUB, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4d::sub_VAL Invalid Vector4d Object");
   }
}

void _Vector4d::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_BCHK(_o, clid_Vector4d) && YAC_BCHK(_r, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      YAC_CAST_ARG(_Vector4d, r, _r);
      r->doubles[0] = doubles[0] - o->doubles[0];
      r->doubles[1] = doubles[1] - o->doubles[1];
      r->doubles[2] = doubles[2] - o->doubles[2];
      r->doubles[3] = doubles[3] - o->doubles[3];
   }
   else
   {
      /* // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_SUB, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4d::sub_ARG Invalid Vector4d Object");
   }
}

sF64 _Vector4d::dot(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      return
         doubles[0] * o->doubles[0] +
         doubles[1] * o->doubles[1] +
         doubles[2] * o->doubles[2] +
         doubles[3] * o->doubles[3] ;
   }
   else
   {
      /* // Dot product with generic Object
      _Vector4d t;
      t.assignGeneric(NULL, _o);
      return (doubles[0]*t.doubles[0] + doubles[1]*t.doubles[1] + doubles[2]*t.doubles[2] + doubles[3]*t.doubles[3]);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4d::dot: invalid Vector4d Object");
   }
   return 0.0;
}

void _Vector4d::_dot_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = dot(_o);
}

void _Vector4d::_dot_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(dot(_o));
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::dot Return object not valid");
   }
}

void _Vector4d::_tensor_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   // return the tensor product of two vectors
   if(YAC_CHK(_o, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      _Matrix4d *m = YAC_NEW_POOLED(Matrix4d);
      _r->initObject(m, YAC_TRUE);
      m->doubles[ 0] = doubles[0] * o->doubles[0];
      m->doubles[ 1] = doubles[0] * o->doubles[1];
      m->doubles[ 2] = doubles[0] * o->doubles[2];
      m->doubles[ 3] = doubles[0] * o->doubles[3];
      m->doubles[ 4] = doubles[1] * o->doubles[0];
      m->doubles[ 5] = doubles[1] * o->doubles[1];
      m->doubles[ 6] = doubles[1] * o->doubles[2];
      m->doubles[ 7] = doubles[1] * o->doubles[3];
      m->doubles[ 8] = doubles[2] * o->doubles[0];
      m->doubles[ 9] = doubles[2] * o->doubles[1];
      m->doubles[10] = doubles[2] * o->doubles[2];
      m->doubles[11] = doubles[2] * o->doubles[3];
      m->doubles[12] = doubles[3] * o->doubles[0];
      m->doubles[13] = doubles[3] * o->doubles[1];
      m->doubles[14] = doubles[3] * o->doubles[2];
      m->doubles[15] = doubles[3] * o->doubles[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4d::tensor: invalid Vector4d object");
   }
}

void _Vector4d::_tensor_YAC_RARG(YAC_Object *_o, YAC_Object *_m) {
   if(YAC_CHK(_m, clid_Matrix4d))
   {
      YAC_CAST_ARG(_Matrix4d, m, _m);
      if(YAC_BCHK(_o, clid_Vector4d))
      {
         YAC_CAST_ARG(_Vector4d, o, _o);
         m->doubles[ 0] = doubles[0] * o->doubles[0];
         m->doubles[ 1] = doubles[0] * o->doubles[1];
         m->doubles[ 2] = doubles[0] * o->doubles[2];
         m->doubles[ 3] = doubles[0] * o->doubles[3];
         m->doubles[ 4] = doubles[1] * o->doubles[0];
         m->doubles[ 5] = doubles[1] * o->doubles[1];
         m->doubles[ 6] = doubles[1] * o->doubles[2];
         m->doubles[ 7] = doubles[1] * o->doubles[3];
         m->doubles[ 8] = doubles[2] * o->doubles[0];
         m->doubles[ 9] = doubles[2] * o->doubles[1];
         m->doubles[10] = doubles[2] * o->doubles[2];
         m->doubles[11] = doubles[2] * o->doubles[3];
         m->doubles[12] = doubles[3] * o->doubles[0];
         m->doubles[13] = doubles[3] * o->doubles[1];
         m->doubles[14] = doubles[3] * o->doubles[2];
         m->doubles[15] = doubles[3] * o->doubles[3];
      }
      else
      {
         /* YAC_Vector4d t; t.assignGeneric(NULL, _o);
           o = &t;*/
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4d::tensor_: invalid Vector4d Object");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4d::tensor_: invalid Matrix4d object");
   }
}

void _Vector4d::muld(sF64 _s) {
   doubles[0] *= _s;
   doubles[1] *= _s;
   doubles[2] *= _s;
   doubles[3] *= _s;
}

void _Vector4d::_muld_YAC_RSELF(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      muld(s);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::muld Object not valid");
   }
}

void _Vector4d::_muld_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   if(YAC_VALID(_s))
   {
      _Vector4d *r = YAC_NEW_POOLED(Vector4d);
      _r->initObject(r, YAC_TRUE);
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      r->doubles[0] = doubles[0] * s;
      r->doubles[1] = doubles[1] * s;
      r->doubles[2] = doubles[2] * s;
      r->doubles[3] = doubles[3] * s;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::muld Object not valid");
   }
}

void _Vector4d::_muld_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, clid_Vector4d))
   {
      if(YAC_VALID(_s))
      {
         YAC_CAST_ARG(_Vector4d, r, _r);
         sF64 s;
         if(!_s->yacScanF64(&s))
            s = 0.0;
         r->doubles[0] = doubles[0] * s;
         r->doubles[1] = doubles[1] * s;
         r->doubles[2] = doubles[2] * s;
         r->doubles[3] = doubles[3] * s;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::muld Object not valid");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4d::muld_ARG Return object not of type Vector4d");
   }
}

void _Vector4d::_mul_YAC_RSELF(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      doubles[0] *= o->doubles[0];
      doubles[1] *= o->doubles[1];
      doubles[2] *= o->doubles[2];
      doubles[3] *= o->doubles[3];
   }
   else
   {
      /* // Multiply by generic Object
         YAC_Value r;
         yacOperator(YAC_OP_MUL, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4d::mul_SELF invalid Vector4d object");
   }
}

void _Vector4d::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      _Vector4d *r = YAC_NEW_POOLED(Vector4d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[0] * o->doubles[0];
      r->doubles[1] = doubles[1] * o->doubles[1];
      r->doubles[2] = doubles[2] * o->doubles[2];
      r->doubles[3] = doubles[3] * o->doubles[3];
   }
   else
   {
      /* // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4d::mul_VAL invalid Vector4d object");
   }
}

void _Vector4d::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_BCHK(_o, clid_Vector4d) &&
      YAC_BCHK(_r, clid_Vector4d)
      )
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      YAC_CAST_ARG(_Vector4d, r, _r);
      r->doubles[0] = doubles[0] * o->doubles[0];
      r->doubles[1] = doubles[1] * o->doubles[1];
      r->doubles[2] = doubles[2] * o->doubles[2];
      r->doubles[3] = doubles[3] * o->doubles[3];
   }
   else
   {
      /* // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector4d::mul_ARG invalid Vector4d object");
   }
}

sF64 _Vector4d::getAbs(void) {
   // return length of vector
   return sqrt(doubles[0] * doubles[0] +
               doubles[1] * doubles[1] +
               doubles[2] * doubles[2] +
               doubles[3] * doubles[3]
               );
}

void _Vector4d::_getAbs_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = getAbs();
}

void _Vector4d::_getAbs_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
     _r->yacValueOfF64(getAbs());
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::getAbs Return object not valid");
   }
}

sF64 _Vector4d::getAbsSqr(void) {
   // return squared length of vector
   return
      doubles[0] * doubles[0] +
      doubles[1] * doubles[1] +
      doubles[2] * doubles[2] +
      doubles[3] * doubles[3] ;
}

void _Vector4d::_getAbsSqr_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = getAbsSqr();
}

void _Vector4d::_getAbsSqr_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(getAbsSqr());
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::getAbs Return object not valid");
   }
}

void _Vector4d::_unit_YAC_RSELF(void) {
   // scale vector to length 1
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      absval = 1.0 / absval;
      doubles[0] *= absval;
      doubles[1] *= absval;
      doubles[2] *= absval;
      doubles[3] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4d::unit_SELF Zero Vector cannot be normalized");
      doubles[0] = 0.0f;
      doubles[1] = 0.0f;
      doubles[2] = 0.0f;
      doubles[3] = 0.0f;
   }
}

void _Vector4d::_unit_YAC_RVAL(YAC_Value *_r) {
   _Vector4d *r = YAC_NEW_POOLED(Vector4d);
   _r->initObject(r, YAC_TRUE);
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      absval = 1.0 / absval;
      r->doubles[0] = doubles[0] * absval;
      r->doubles[1] = doubles[1] * absval;
      r->doubles[2] = doubles[2] * absval;
      r->doubles[3] = doubles[3] * absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4d::unit_VAL Zero Vector cannot be normalized");
      r->doubles[0] = 0.0f;
      r->doubles[1] = 0.0f;
      r->doubles[2] = 0.0f;
      r->doubles[3] = 0.0f;
   }
}

void _Vector4d::_unit_YAC_RARG(YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, r, _r);
      sF64 absval = getAbs();
      if(Ddblnonzero(absval))
      {
         absval = 1.0 / absval;
         r->doubles[0] = doubles[0] * absval;
         r->doubles[1] = doubles[1] * absval;
         r->doubles[2] = doubles[2] * absval;
         r->doubles[3] = doubles[3] * absval;
      }
      else
      {
         // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4d::unit_ARG Zero Vector cannot be normalized");
         r->doubles[0] = 0.0f;
         r->doubles[1] = 0.0f;
         r->doubles[2] = 0.0f;
         r->doubles[3] = 0.0f;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4d::unit_ARG Return object not of type Vector4d");
   }
}

void _Vector4d::unitScale(sF64 s) {
   // scale vector to length s
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      absval = s / absval;
      doubles[0] *= absval;
      doubles[1] *= absval;
      doubles[2] *= absval;
      doubles[3] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4d::unitScale_SELF Zero Vector cannot be normalized");
      doubles[0] = 0.0f;
      doubles[1] = 0.0f;
      doubles[2] = 0.0f;
      doubles[3] = 0.0f;
   }
}

void _Vector4d::_unitScale_YAC_RSELF(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      unitScale(s);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::unitScale Object not valid");
   }
}

void _Vector4d::_unitScale_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   _Vector4d *r = YAC_NEW_POOLED(Vector4d);
   _r->initObject(r, YAC_TRUE);
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      if(YAC_VALID(_s))
      {
         sF64 s;
         if(!_s->yacScanF64(&s))
            s = 0.0;
         absval = s / absval;
         r->doubles[0] = doubles[0] * absval;
         r->doubles[1] = doubles[1] * absval;
         r->doubles[2] = doubles[2] * absval;
         r->doubles[3] = doubles[3] * absval;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::unitScale Object not valid");
      }
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4d::unitScale_VAL Zero Vector cannot be normalized");
      r->doubles[0] = 0.0f;
      r->doubles[1] = 0.0f;
      r->doubles[2] = 0.0f;
      r->doubles[3] = 0.0f;
   }
}

void _Vector4d::_unitScale_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, r, _r);
      sF64 absval = getAbs();
      if(Ddblnonzero(absval))
      {
         if(YAC_VALID(_s))
         {
            sF64 s;
            if(!_s->yacScanF64(&s))
               s = 0.0;
            absval = s / absval;
            r->doubles[0] = doubles[0] * absval;
            r->doubles[1] = doubles[1] * absval;
            r->doubles[2] = doubles[2] * absval;
            r->doubles[3] = doubles[3] * absval;
         }
         else
         {
            Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::unitScale Object not valid");
         }
      }
      else
      {
         // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector4d::unitScale_SELF Zero Vector cannot be normalized");
         r->doubles[0] = 0.0f;
         r->doubles[1] = 0.0f;
         r->doubles[2] = 0.0f;
         r->doubles[3] = 0.0f;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector4d::unitScale_ARG Return object not of type Vector4d");
   }
}

void _Vector4d::_initf(sF32 _x, sF32 _y, sF32 _z, sF32 _w) {
   doubles[0] = _x;
   doubles[1] = _y;
   doubles[2] = _z;
   doubles[3] = _w;
}

void _Vector4d::_init(YAC_Object *_x, YAC_Object *_y, YAC_Object *_z, YAC_Object *_w) {
   if(YAC_VALID(_x) &&
      YAC_VALID(_y) &&
      YAC_VALID(_z) &&
      YAC_VALID(_w)
      )
   {
      if(!_x->yacScanF64(&doubles[0])) doubles[0] = 0.0;
      if(!_y->yacScanF64(&doubles[1])) doubles[1] = 0.0;
      if(!_z->yacScanF64(&doubles[2])) doubles[2] = 0.0;
      if(!_w->yacScanF64(&doubles[3])) doubles[3] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::init Object not valid");
   }
}

void _Vector4d::_initScaled(YAC_Object *_o, YAC_Object *_s) {
   if(YAC_CHK(_o, clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      if(YAC_VALID(_s))
      {
         sF64 s;
         if(!_s->yacScanF64(&s))
            s = 0.0;
         doubles[0] = o->doubles[0] * s;
         doubles[1] = o->doubles[1] * s;
         doubles[2] = o->doubles[2] * s;
         doubles[3] = o->doubles[3] * s;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector4d::initScaled Object not valid");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector4d::initScalef: invalid object parameter");
   }
}

void _Vector4d::_New(YAC_Object *_x, YAC_Object *_y, YAC_Object *_z, YAC_Object *_w, YAC_Value *_r) {
   _Vector4d *r = YAC_NEW_POOLED(Vector4d);
   _r->initObject(r, YAC_TRUE);
   r->_init(_x, _y, _z, _w);
}

void YAC_CALL _vector4d(YAC_Object *_x, YAC_Object *_y, YAC_Object *_z, YAC_Object *_w, YAC_Value *_r) {
   _Vector4d *r = YAC_NEW_POOLED(Vector4d);
   _r->initObject(r, YAC_TRUE);
   r->_init(_x, _y, _z, _w);
}
