/// vector3d.cpp
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
#include "vector3d.h"
#include "matrix3d.h"

#include "ying_Math.h"

_Vector3d::_Vector3d(void) {
   doubles[0] = doubles[1] = doubles[2] = 0.0;
}

_Vector3d::~_Vector3d() {
}

sUI _Vector3d::yacArrayGetNumElements(void) {
   return 3u;
}

sUI _Vector3d::yacArrayGetMaxElements(void) {
   return 3u;
}

sUI _Vector3d::yacArrayGetElementType(void) {
   return YAC_TYPE_OBJECT;
}

sUI _Vector3d::yacArrayGetElementByteSize(void) {
   return sizeof(sF64);
}

void _Vector3d::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 3)
   {
      sF64 v;
      switch(_val->type)
      {
         case YAC_TYPE_VOID:
            v = 0.0;
            break;

         case YAC_TYPE_INT:
            v = (sF64)_val->value.int_val;
            break;

         case YAC_TYPE_FLOAT:
            v = (sF64)_val->value.float_val;
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
            Dyac_throw(TypeMismatch, "tkmath::Vector3d::yacArraySet: unknown type");
            v = 0.0;
            break;
      }
      doubles[_idx] = v;
   }
   else
   {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Vector3d::yacArraySet Index too high (%d>2)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Vector3d::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 3u)
   {
      YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
      _val->initObject(d, YAC_TRUE);
      d->value = doubles[_idx];
   }
   else
   {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Vector3d::yacArrayGet: Index too high (%d>2)", _idx);
      _val->initFloat(0.0f);
      Dyac_throw(ReadArrayOutOfBounds, buf);
   }
}

void *YAC_VCALL _Vector3d::yacArrayGetPointer(void) {
   return (void*)doubles;
}

sSI YAC_VCALL _Vector3d::yacTensorRank(void) {
   return YAC_TENSOR_RANK_VECTOR;
}

void YAC_VCALL _Vector3d::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F64(doubles[0]);
   YAC_SERIALIZE_F64(doubles[1]);
   YAC_SERIALIZE_F64(doubles[2]);
}

sUI YAC_VCALL _Vector3d::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   doubles[0] = YAC_DESERIALIZE_F64();
   doubles[1] = YAC_DESERIALIZE_F64();
   doubles[2] = YAC_DESERIALIZE_F64();
   return sUI(YAC_TRUE);
}

void YAC_VCALL _Vector3d::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector3d))
   {
      if(YAC_BCHK(_max, clid_Vector3d))
      {
         YAC_CAST_ARG(_Vector3d, min, _min);
         YAC_CAST_ARG(_Vector3d, max, _max);

         if(doubles[0] < min->doubles[0])
            doubles[0] = min->doubles[0];
         else if(doubles[0] > max->doubles[0])
            doubles[0] = max->doubles[0];

         if(doubles[1] < min->doubles[1])
            doubles[1] = min->doubles[1];
         else if(doubles[1] > max->doubles[1])
            doubles[1] = max->doubles[1];

         if(doubles[2] < min->doubles[2])
            doubles[2] = min->doubles[2];
         else if(doubles[2] > max->doubles[2])
            doubles[2] = max->doubles[2];
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector3d::yacOperatorClamp: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector3d::yacOperatorClamp: invalid _min object.");
   }
}

void YAC_VCALL _Vector3d::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector3d))
   {
      if(YAC_BCHK(_max, clid_Vector3d))
      {
         YAC_CAST_ARG(_Vector3d, min, _min);
         YAC_CAST_ARG(_Vector3d, max, _max);

         if (doubles[0] < min->doubles[0])
            doubles[0] += max->doubles[0] - min->doubles[0];
         else if(doubles[0] > max->doubles[0])
            doubles[0] -= max->doubles[0] - min->doubles[0];

         if(doubles[1] < min->doubles[1])
            doubles[1] += max->doubles[1] - min->doubles[1];
         else if(doubles[1] > max->doubles[1])
            doubles[1] -= max->doubles[1] - min->doubles[1];

         if(doubles[2] < min->doubles[2])
            doubles[2] += max->doubles[2] - min->doubles[2];
         else if(doubles[2] > max->doubles[2])
            doubles[2] -= max->doubles[2] - min->doubles[2];
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector3d::yacOperatorWrap: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector3d::yacOperatorWrap: invalid _min object.");
   }
}

void YAC_VCALL _Vector3d::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      switch(_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI(Ddblequal(doubles[0], o->doubles[0]) &&
                     Ddblequal(doubles[1], o->doubles[1]) &&
                     Ddblequal(doubles[2], o->doubles[2])
                     );
            break;

         case YAC_OP_CNE:
            YAC_RETI(Ddblnotequal(doubles[0], o->doubles[0]) ||
                     Ddblnotequal(doubles[1], o->doubles[1]) ||
                     Ddblnotequal(doubles[2], o->doubles[2])
                     );
            break;

         case YAC_OP_ASSIGN:
            doubles[0] = o->doubles[0];
            doubles[1] = o->doubles[1];
            doubles[2] = o->doubles[2];
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
            break;

         case YAC_OP_INIT:
            doubles[0] = 0.0;
            doubles[1] = 0.0;
            doubles[2] = 0.0;
            break;

         default:
         {
            // Should never ever happen !
            char buf[256];
            Dyac_snprintf(buf, 256, "tkmath::Vector3d::yacOperator unsupported cmd %d", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
         }
         return;
      } // switch _cmd
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
         switch (_cmd)
         {
            case YAC_OP_ASSIGN:
               assignFloatArray(o);
               break;

            default:
            {
               // Create temporary Vector3d object on stack
               _Vector3d t;
               t.class_ID = clid_Vector3d;
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
               /* _Vector3d t;
               t.class_ID = clid_Vector3d;
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
            break;

         default:
            // Call generic yacOperator, e.g. for <null> comparisons
            YAC_Object::yacOperator(_cmd, _o, _r);
            break;
      }
   }

}

void _Vector3d::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void _Vector3d::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void _Vector3d::yacOperatorF64(sSI _cmd, sF64 val, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_MUL:
         muld(val);
         break;

      case YAC_OP_DIV:
         if(Ddblnonzero(val))
         {
            muld(1.0 / val);
         }
         else
         {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3d::yacOperatorF64 OP_DIV Cannot divide by zero");
         }
         break;

      default:
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Vector3d::yacOperatorF64 Operator not supported %d", _cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
         break;
   }
}

void _Vector3d::assignFloats(const sF32 *_fv, sUI _ne) {
   if(_ne <= 3u)
   {
      // Copy as much elements from array as possible but never more than 3
      sUI i;
      for(i = 0u; i < _ne; i++)
         doubles[i] = (sF64)_fv[i];

      // Fill the rest with 0.0f
      for(; i < 3u; i++)
         doubles[i] = 0.0;
   }
   else
   {
      // Copy three elements
      doubles[0] = (sF64) _fv[0];
      doubles[1] = (sF64) _fv[1];
      doubles[2] = (sF64) _fv[2];
   }
}

void _Vector3d::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Vector3d::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
   sUI ne = _o->yacArrayGetNumElements();
   if(ne > 0)
   {
      if(YAC_TYPE_FLOAT == _o->yacArrayGetElementType())
      {
         sF32 *fv = (sF32*) _o->yacArrayGetPointer();
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
            for(i=0; (i<3) && (i<ne); i++)
            {
               _o->yacArrayGet(_ctx, i, &v);
               doubles[i] = (sF32) v.value.float_val;
            }
            // Fill the rest with 0.0f
            for(; i<3; i++)
            {
               doubles[i] = 0.0;
            }
         }
      }
      else
      {
         // Retrieve individual elements, typecast to float and assign
         YAC_Value v;
         sUI i;
         for(i=0; (i<3) && (i<ne); i++)
         {
            _o->yacArrayGet(_ctx, i, &v);
            v.typecast(YAC_TYPE_OBJECT);
	    if (!v.value.object_val->yacScanF64(&doubles[i])) doubles[i] = 0;
            v.unset();
         }
         // Fill the rest with 0.0f
         for(; i<3; i++)
         {
            doubles[i] = 0.0;
         }
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      doubles[0] = doubles[1] = doubles[2] = 0.0;
   }
}

void _Vector3d::_setX(YAC_Object *x) {
   if (YAC_VALID(x)) {
      if (!x->yacScanF64(&doubles[0])) doubles[0] = 0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::setX Object not valid");
   }
}

void _Vector3d::_setY(YAC_Object *y) {
   if (YAC_VALID(y)) {
      if (!y->yacScanF64(&doubles[1])) doubles[1] = 0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::setY Object not valid");
   }
}

void _Vector3d::_setZ(YAC_Object *z) {
   if (YAC_VALID(z)) {
      if (!z->yacScanF64(&doubles[2])) doubles[2] = 0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::setZ Object not valid");
   }
}

void _Vector3d::_getX_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[0];
   _r->initObject(d,1);
}

void _Vector3d::_getX_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[0]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::getX Return object not valid");
   }
}

void _Vector3d::_getY_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[1];
   _r->initObject(d,1);
}

void _Vector3d::_getY_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[1]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::getY Return object not valid");
   }
}

void _Vector3d::_getZ_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[2];
   _r->initObject(d,1);
}

void _Vector3d::_getZ_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[2]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::getZ Return object not valid");
   }
}

sBool YAC_VCALL _Vector3d::yacToString(YAC_String *s) const {
#ifdef YAC_BIGSTRING
   s->yacArrayAlloc(1024,0,0,0);
   s->printf("(%lg, %lg, %lg)",doubles[0],doubles[1],doubles[2]);
   s->fixLength();
   return 1;
#endif
   return 0;
}

void _Vector3d::_getString(YAC_Value *_r) const {
   YAC_String *s=YAC_New_String();
   yacToString(s);
   YAC_RETS(s,1);
}

void _Vector3d::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      doubles[0] += o->doubles[0];
      doubles[1] += o->doubles[1];
      doubles[2] += o->doubles[2];
   }
   else
   {
      /* // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3d::add_SELF Object not of Type Vector3d");
   }
}

void _Vector3d::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      _Vector3d *r = YAC_NEW_POOLED(Vector3d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[0] + o->doubles[0];
      r->doubles[1] = doubles[1] + o->doubles[1];
      r->doubles[2] = doubles[2] + o->doubles[2];
   }
   else
   {
      /* // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3d::add_VAL Object not of Type Vector3d");
   }
}

void _Vector3d::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_BCHK(_o, clid_Vector3d) && YAC_BCHK(_r, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      YAC_CAST_ARG(_Vector3d, r, _r);
      r->doubles[0] = doubles[0] + o->doubles[0];
      r->doubles[1] = doubles[1] + o->doubles[1];
      r->doubles[2] = doubles[2] + o->doubles[2];
   }
   else
   {
      /* // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3d::add_ARG Object not of Type Vector3d");
   }
}

void _Vector3d::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      doubles[0] -= o->doubles[0];
      doubles[1] -= o->doubles[1];
      doubles[2] -= o->doubles[2];
   }
   else
   {
      /* // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3d::sub_SELF Object not of Type Vector3d");
   }
}

void _Vector3d::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      _Vector3d *r = YAC_NEW_POOLED(Vector3d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[0] - o->doubles[0];
      r->doubles[1] = doubles[1] - o->doubles[1];
      r->doubles[2] = doubles[2] - o->doubles[2];
   }
   else
   {
      /* // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3d::sub_VAL Object not of Type Vector3d");
   }
}

void _Vector3d::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_BCHK(_o, clid_Vector3d) && YAC_CHK(_r, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      YAC_CAST_ARG(_Vector3d, r, _r);
      r->doubles[0] = doubles[0] - o->doubles[0];
      r->doubles[1] = doubles[1] - o->doubles[1];
      r->doubles[2] = doubles[2] - o->doubles[2];
   }
   else
   {
      /* // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3d::sub_ARG Object not of Type Vector3d");
   }
}

sF64 _Vector3d::dot(YAC_Object *_o) {
   // returns scalar dot product
   if(YAC_BCHK(_o, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      return
         doubles[0] * o->doubles[0] +
         doubles[1] * o->doubles[1] +
         doubles[2] * o->doubles[2] ;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3d::dot Object not of Type Vector3d");
   }
   return 0.0;
}

void _Vector3d::_dot_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = dot(_o);
}

void _Vector3d::_dot_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(dot(_o));
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::dot Return object not valid");
   }
}

void _Vector3d::_cross_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   // returns the orthogonal vector of the cross product
   if(YAC_BCHK(_o, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      _Vector3d *r = YAC_NEW_POOLED(Vector3d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[1] * o->doubles[2] - doubles[2] * o->doubles[1];
      r->doubles[1] = doubles[2] * o->doubles[0] - doubles[0] * o->doubles[2];
      r->doubles[2] = doubles[0] * o->doubles[1] - doubles[1] * o->doubles[0];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3d::cross Object not of type Vector3d");
   }
}

void _Vector3d::_cross_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   // returns the orthogonal vector of the cross product
   if(YAC_BCHK(_o, clid_Vector3d) && YAC_BCHK(_r, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      YAC_CAST_ARG(_Vector3d, r, _r);
      r->doubles[0] = doubles[1] * o->doubles[2] - doubles[2] * o->doubles[1];
      r->doubles[1] = doubles[2] * o->doubles[0] - doubles[0] * o->doubles[2];
      r->doubles[2] = doubles[0] * o->doubles[1] - doubles[1] * o->doubles[0];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3d::cross Object not of type Vector3d");
   }
}

sF64 _Vector3d::triple(YAC_Object *_b, YAC_Object *_c) {
   // return the scalar triple product (a*(bxc))
   if(YAC_BCHK(_b, clid_Vector3d) && YAC_BCHK(_c,clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, b, _b);
      YAC_CAST_ARG(_Vector3d, c, _c);
      _Matrix3d m;
      memcpy(m.doubles+0/*(0*sizeof(sF32))*/,    doubles, sizeof(sF64)*3); // we copy as row vector instead of col
      memcpy(m.doubles+3/*(3*sizeof(sF32))*/, b->doubles, sizeof(sF64)*3); // because the determinant of a
      memcpy(m.doubles+6/*(6*sizeof(sF32))*/, c->doubles, sizeof(sF64)*3); // transposed matrix is the same
      return m.det();
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3d::triple Object not of type Vector3d");
   }
   return 0.0;
}

void _Vector3d::_triple_YAC_RVAL(YAC_Object *_b, YAC_Object *_c, YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = triple(_b,_c);
}

void _Vector3d::_triple_YAC_RARG(YAC_Object *_b, YAC_Object *_c, YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(triple(_b,_c));
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::triple Return object not valid");
   }
}

void _Vector3d::_tensor_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   // return the matrix of the tensor product
   if(YAC_BCHK(_o, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      _Matrix3d *m = YAC_NEW_POOLED(Matrix3d);
      _r->initObject(m, YAC_TRUE);
      m->doubles[0] = doubles[0] * o->doubles[0];
      m->doubles[1] = doubles[0] * o->doubles[1];
      m->doubles[2] = doubles[0] * o->doubles[2];
      m->doubles[3] = doubles[1] * o->doubles[0];
      m->doubles[4] = doubles[1] * o->doubles[1];
      m->doubles[5] = doubles[1] * o->doubles[2];
      m->doubles[6] = doubles[2] * o->doubles[0];
      m->doubles[7] = doubles[2] * o->doubles[1];
      m->doubles[8] = doubles[2] * o->doubles[2];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3d::tensor: invalid Vector3d object");
   }
}

void _Vector3d::_tensor_YAC_RARG(YAC_Object *_o, YAC_Object *_m) {
   // return the matrix of the tensor product
   if(YAC_CHK(_m, clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, m, _m);
      if(YAC_BCHK(_o, clid_Vector3d))
      {
         YAC_CAST_ARG(_Vector3d, o, _o);
         m->doubles[0] = doubles[0] * o->doubles[0];
         m->doubles[1] = doubles[0] * o->doubles[1];
         m->doubles[2] = doubles[0] * o->doubles[2];
         m->doubles[3] = doubles[1] * o->doubles[0];
         m->doubles[4] = doubles[1] * o->doubles[1];
         m->doubles[5] = doubles[1] * o->doubles[2];
         m->doubles[6] = doubles[2] * o->doubles[0];
         m->doubles[7] = doubles[2] * o->doubles[1];
         m->doubles[8] = doubles[2] * o->doubles[2];
      }
      else
      {
         /* t.assignGeneric(NULL, _o);
           o = &t;*/
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3d::tensor_: invalid Vector3d Object");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3d::tensor_: invalid Matrix3f object");
   }
}

void _Vector3d::muld(sF64 s) {
   doubles[0] *= s;
   doubles[1] *= s;
   doubles[2] *= s;
}

void _Vector3d::_muld_YAC_RSELF(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      muld(s);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::muld Object not valid");
   }
}

void _Vector3d::_muld_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   // multiply elements by scalar value
   if(YAC_VALID(_s))
   {
      _Vector3d *r = YAC_NEW_POOLED(Vector3d);
      _r->initObject(r, YAC_TRUE);
      sF64 s;
      if (!_s->yacScanF64(&s)) s=0.0;
      r->doubles[0] = doubles[0] * s;
      r->doubles[1] = doubles[1] * s;
      r->doubles[2] = doubles[2] * s;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::muld Object not valid");
   }
}

void _Vector3d::_muld_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   // multiply elements by scalar value
   if(YAC_BCHK(_r, clid_Vector3d))
   {
      if(YAC_VALID(_s))
      {
         YAC_CAST_ARG(_Vector3d, r, _r);
         sF64 s;
         if(!_s->yacScanF64(&s))
            s = 0.0;
         r->doubles[0] = doubles[0] * s;
         r->doubles[1] = doubles[1] * s;
         r->doubles[2] = doubles[2] * s;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::muld Object not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3d::mulf_ARG Return type not of type Vector3d");
   }
}

void _Vector3d::_mul_YAC_RSELF(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_Vector3d))
   {
      _Vector3d *o=(_Vector3d*)_o;
      doubles[0] *= o->doubles[0];
      doubles[1] *= o->doubles[1];
      doubles[2] *= o->doubles[2];
   }
   else
   {
      /* // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3d::mul_SELF Invalid Vector3d object");
   }
}

void _Vector3d::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      _Vector3d *r = YAC_NEW_POOLED(Vector3d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[0] * o->doubles[0];
      r->doubles[1] = doubles[1] * o->doubles[1];
      r->doubles[2] = doubles[2] * o->doubles[2];
   }
   else
   {
      /* // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3d::mul_VAL Invalid Vector3d object");
   }
}

void _Vector3d::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_BCHK(_o, clid_Vector3d) && YAC_BCHK(_r, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      YAC_CAST_ARG(_Vector3d, r, _r);
      r->doubles[0] = doubles[0] * o->doubles[0];
      r->doubles[1] = doubles[1] * o->doubles[1];
      r->doubles[2] = doubles[2] * o->doubles[2];
   }
   else
   {
      /* // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);*/
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector3d::mul_ARG Invalid Vector3d object");

   }
}

sF64 _Vector3d::getAbs(void) {
   return sqrt(doubles[0] * doubles[0] +
               doubles[1] * doubles[1] +
               doubles[2] * doubles[2]
               );
}

void _Vector3d::_getAbs_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = getAbs();
}

void _Vector3d::_getAbs_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(getAbs());
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::getAbs Object not valid");
   }
}

sF64 _Vector3d::getAbsSqr(void) {
   return
      doubles[0] * doubles[0] +
      doubles[1] * doubles[1] +
      doubles[2] * doubles[2]
      ;
}

void _Vector3d::_getAbsSqr_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = getAbsSqr();
}

void _Vector3d::_getAbsSqr_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(getAbsSqr());
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::getAbsSqr Object not valid");
   }
}

void _Vector3d::_unit_YAC_RSELF(void) {
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      absval = 1.0 / absval;
      doubles[0] *= absval;
      doubles[1] *= absval;
      doubles[2] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3d::unit_SELF Zero Vector cannot be normalized");
      doubles[0] = 0.0f;
      doubles[1] = 0.0f;
      doubles[2] = 0.0f;
   }
}

void _Vector3d::_unit_YAC_RVAL(YAC_Value *_r) {
   _Vector3d *r = YAC_NEW_POOLED(Vector3d);
   _r->initObject(r, YAC_TRUE);
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      absval = 1.0 / absval;
      r->doubles[0] = doubles[0] * absval;
      r->doubles[1] = doubles[1] * absval;
      r->doubles[2] = doubles[2] * absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3d::unit_VAL Zero Vector cannot be normalized");
      r->doubles[0] = 0.0f;
      r->doubles[1] = 0.0f;
      r->doubles[2] = 0.0f;
   }
}

void _Vector3d::_unit_YAC_RARG(YAC_Object *_r) {
   if(YAC_BCHK(_r, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, r, _r);
      sF64 absval = getAbs();
      if(absval != 0.0)
      {
         absval = 1.0 / absval;
         r->doubles[0] = doubles[0] * absval;
         r->doubles[1] = doubles[1] * absval;
         r->doubles[2] = doubles[2] * absval;
      }
      else
      {
         // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3d::unit_ARG Zero Vector cannot be normalized");
         r->doubles[0] = 0.0f;
         r->doubles[1] = 0.0f;
         r->doubles[2] = 0.0f;
      }
   }
   else
   {
     Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3d::unit_ARG Return object not of type Vector3d");
   }
}

void _Vector3d::unitScale(sF64 s) {
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      absval = s / absval;

      doubles[0] *= absval;
      doubles[1] *= absval;
      doubles[2] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3d::unitScale_SELF Zero Vector cannot be normalized");
      doubles[0] = 0.0f;
      doubles[1] = 0.0f;
      doubles[2] = 0.0f;
   }
}

void _Vector3d::_unitScale_YAC_RSELF(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      unitScale(s);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::unitScale Object not valid");
   }
}

void _Vector3d::_unitScale_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   _Vector3d *r = YAC_NEW_POOLED(Vector3d);
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
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::unitScale Object not valid");
      }
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3d::unitScale_VAL Zero Vector cannot be normalized");
   }
}

void _Vector3d::_unitScale_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, r, _r);
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
         }
         else
         {
            Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::unitScale Object not valid");
         }
      }
      else
      {
         // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector3d::unitScale_ARG Zero Vector cannot be normalized");
         r->doubles[0] = 0.0f;
         r->doubles[1] = 0.0f;
         r->doubles[2] = 0.0f;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector3d::unitScale_ARG Return object not of type Vector3d");
   }
}

void _Vector3d::_initf(sF32 x, sF32 y, sF32 z) {
   doubles[0] = x;
   doubles[1] = y;
   doubles[2] = z;
}

void _Vector3d::_init(YAC_Object *_x, YAC_Object *_y, YAC_Object *_z) {
   if(YAC_VALID(_x) && YAC_VALID(_y) && YAC_VALID(_z))
   {
      if(!_x->yacScanF64(&doubles[0])) doubles[0]=0.0;
      if(!_y->yacScanF64(&doubles[1])) doubles[1]=0.0;
      if(!_z->yacScanF64(&doubles[2])) doubles[2]=0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::init Objects not valid");
   }
}

void _Vector3d::_initScaled(YAC_Object *_o, YAC_Object *_s) {
   if(YAC_BCHK(_o, clid_Vector3d))
   {
      if(YAC_VALID(_s))
      {
         YAC_CAST_ARG(_Vector3d, o, _o);
         sF64 s;
         if(!_s->yacScanF64(&s))
            s = 0.0;
         doubles[0] = o->doubles[0] * s;
         doubles[1] = o->doubles[1] * s;
         doubles[2] = o->doubles[2] * s;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector3d::initScaled Object not valid");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector3d::initScaled invalid object parameter");
   }
}

void _Vector3d::_New(YAC_Object *x, YAC_Object *y, YAC_Object *z, YAC_Value *_r) {
   _Vector3d *r = YAC_NEW_POOLED(Vector3d);
   _r->initObject(r, YAC_TRUE);
   r->_init(x, y, z);
}

void YAC_CALL _vector3d(YAC_Object *x, YAC_Object *y, YAC_Object *z,YAC_Value *_r) {
   _Vector3d *r = YAC_NEW_POOLED(Vector3d);
   _r->initObject(r, YAC_TRUE);
   r->_init(x, y, z);
}
