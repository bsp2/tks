/// matrix2x3f.cpp
///
/// (c) 2024-2025 by Bastian Spiegel <bs@tkscript.de>
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
#include "matrix2x3f.h"

#include "ying_Math.h"


#ifdef USE_DYNAMIC_ORDER
const sU8 matrix3_element_order_table[2][3][3] = {
   // Row major
   {
      { 0,  1, 2 },
      { 3,  4, 5 },
   },

   // Column major
   {
      { 0,  3, },
      { 1,  4, },
      { 2,  5, },
   }
};
#endif // USE_DYNAMIC_ORDER

_Matrix2x3f::_Matrix2x3f(void) {
   _initIdentity();
}

_Matrix2x3f::~_Matrix2x3f() {
}

sUI YAC_VCALL _Matrix2x3f::yacArrayGetNumElements(void) {
   return 6u;
}

sUI YAC_VCALL _Matrix2x3f::yacArrayGetMaxElements(void) {
   return 6u;
}

sUI YAC_VCALL _Matrix2x3f::yacArrayGetElementType(void) {
   return YAC_TYPE_FLOAT;
}

sUI YAC_VCALL _Matrix2x3f::yacArrayGetElementByteSize(void) {
   return sizeof(sF32);
}

void YAC_VCALL _Matrix2x3f::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 6u)
   {
      sF32 v = 0.0f;
      switch (_val->type)
      {
         case YAC_TYPE_VOID:
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
               (void)_val->value.object_val->yacScanF32(&v);
            }
            break;

         default:
            Dyac_throw(TypeMismatch, "tkmath::Matrix2x3f::yacArraySet Unknown Type!");
            break;
      }
      floats[_idx] = v;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::yacArraySet index out of bounds (%u > 5)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Matrix2x3f::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 6u)
   {
      _val->initFloat(floats[_idx]);
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::yacArrayGet index out of bounds (%u > 5)", _idx);
      Dyac_throw(ReadArrayOutOfBounds, buf);
      _val->initFloat(0.0f);
   }
}

void *YAC_VCALL _Matrix2x3f::yacArrayGetPointer(void) {
   return (void*)floats;
}

sUI YAC_VCALL _Matrix2x3f::yacArrayGetWidth(void) {
   return 3;
}

sUI YAC_VCALL _Matrix2x3f::yacArrayGetHeight(void) {
   return 2;
}

sUI YAC_VCALL _Matrix2x3f::yacArrayGetStride(void) {
   return 3u * sizeof(sF32);
}

sSI YAC_VCALL _Matrix2x3f::yacTensorRank(void) {
   return YAC_TENSOR_RANK_MATRIX;
}

void YAC_VCALL _Matrix2x3f::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F32(floats[0]);
   YAC_SERIALIZE_F32(floats[1]);
   YAC_SERIALIZE_F32(floats[2]);
   YAC_SERIALIZE_F32(floats[3]);
   YAC_SERIALIZE_F32(floats[4]);
   YAC_SERIALIZE_F32(floats[5]);
}

sUI YAC_VCALL _Matrix2x3f::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   floats[0] = YAC_DESERIALIZE_F32();
   floats[1] = YAC_DESERIALIZE_F32();
   floats[2] = YAC_DESERIALIZE_F32();
   floats[3] = YAC_DESERIALIZE_F32();
   floats[4] = YAC_DESERIALIZE_F32();
   floats[5] = YAC_DESERIALIZE_F32();
   return 1u;
}

void YAC_VCALL _Matrix2x3f::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Matrix2x3f) &&
      YAC_CHK(_max, clid_Matrix2x3f)
      )
   {
      YAC_CAST_ARG(_Matrix2x3f, min, _min);
      YAC_CAST_ARG(_Matrix2x3f, max, _max);

      for(sUI i = 0u; i < 6u; i++)
      {
         if(floats[i] < min->floats[i])
            floats[i] = min->floats[i];
         else if(floats[i] > max->floats[i])
            floats[i] = max->floats[i];
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "");
   }
}

void YAC_VCALL _Matrix2x3f::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Matrix2x3f) &&
      YAC_CHK(_max, clid_Matrix2x3f)
      )
   {
      YAC_CAST_ARG(_Matrix2x3f, min, _min);
      YAC_CAST_ARG(_Matrix2x3f, max, _max);

      for(sUI i = 0u; i < 6u; i++)
      {
         if(floats[i] < min->floats[i])
            floats[i] += (max->floats[i] - min->floats[i]);
         else if(floats[i] > max->floats[i])
            floats[i] -= (max->floats[i] - min->floats[i]);
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "");
   }
}

void YAC_VCALL _Matrix2x3f::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      switch(_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI( Dfltequal(floats[0], o->floats[0]) &&
                      Dfltequal(floats[1], o->floats[1]) &&
                      Dfltequal(floats[2], o->floats[2]) &&
                      Dfltequal(floats[3], o->floats[3]) &&
                      Dfltequal(floats[4], o->floats[4]) &&
                      Dfltequal(floats[5], o->floats[5])
                      );
            break;

         case YAC_OP_CNE:
            YAC_RETI( Dfltnotequal(floats[0], o->floats[0]) ||
                      Dfltnotequal(floats[1], o->floats[1]) ||
                      Dfltnotequal(floats[2], o->floats[2]) ||
                      Dfltnotequal(floats[3], o->floats[3]) ||
                      Dfltnotequal(floats[4], o->floats[4]) ||
                      Dfltnotequal(floats[5], o->floats[5])
                      );
            break;

         case YAC_OP_ASSIGN:
            floats[0] = o->floats[0];
            floats[1] = o->floats[1];
            floats[2] = o->floats[2];
            floats[3] = o->floats[3];
            floats[4] = o->floats[4];
            floats[5] = o->floats[5];
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
            floats[4] = -floats[4];
            floats[5] = -floats[5];
            break;

         case YAC_OP_INIT:
            break;

         default:
         {
            char buf[80];
            Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::yacOperator: unsupported cmd %d!\n", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
            return;
         }
      }
   }
   else if(YAC_Is_FloatArray(_o))
   {
      YAC_CAST_ARG(YAC_FloatArray, o, _o);
      switch(_cmd)
      {
         case YAC_OP_ASSIGN:
            if(o->num_elements < 6u)
            {
               sUI i;
               for(i = 0u; i < o->num_elements; i++)
                  floats[i] = o->elements[i];
               for(;i < 6u; i++)
                  floats[i] = 0.0f;
            }
            else
            {
               floats[0] = o->elements[0];
               floats[1] = o->elements[1];
               floats[2] = o->elements[2];
               floats[3] = o->elements[3];
               floats[4] = o->elements[4];
               floats[5] = o->elements[5];
            }
            break;

         default:
            {
               YAC_NEW_STACK(Matrix2x3f, t);
               t.assignFloatArray(o);
               yacOperator(_cmd, &t, _r);
               //Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix2x3f::yacOperator Only Assign supported with FloatArray Type");
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
         switch(_cmd)
         {
            case YAC_OP_ASSIGN:
               assignGeneric(NULL, _o);
               break;

            default:
               Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix2x3f::yacOperator Only Assign supported with Generic Type");
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
            floats[4] = -floats[4];
            floats[5] = -floats[5];
            break;

         default:
            YAC_Object::yacOperator(_cmd,_o,_r);
            break;
      }
   }
}

void _Matrix2x3f::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF32(_cmd, (sF32)val, _r);
}

void _Matrix2x3f::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   switch(_cmd)
   {
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
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2x3f::yacOperatorF32 OP_DIV Cannot divide by zero");
         }
         break;

      default:
         char buf[80];
         Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::yacOperatorF32 Operator not supported %d", _cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
         break;
   }
}

void _Matrix2x3f::assignFloats(sF32 *_fv, sUI _ne) {
   if(6u == _ne)
   {
      floats[0] = _fv[0];
      floats[1] = _fv[1];
      floats[2] = _fv[2];
      floats[3] = _fv[3];
      floats[4] = _fv[4];
      floats[5] = _fv[5];
   }
   else
   {
      Dyac_throw_def(WriteArrayOutOfBounds,"tkmath::Matrix2x3f Array not of size 6");
   }
}

void _Matrix2x3f::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Matrix2x3f::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
   sUI ne = _o->yacArrayGetNumElements();
   sUI nH = _o->yacArrayGetHeight();
   sUI nW = _o->yacArrayGetWidth();
   if(0u == nH)
   {
      if(6u == nW)
      {
         nH = 2u;
         nW = 3u;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix2x3f Array not of size 6");
      }
   }
   if(ne > 0u)
   {
      if(YAC_TYPE_FLOAT == _o->yacArrayGetElementType())
      {
         const sF32 *fv = (sF32*) _o->yacArrayGetPointer();
         if(NULL != fv)
         {
            // Copy fast
            sUI i,j;
            for(i = 0u; (i < 2u) && (i < nH); i++)
            {
               for(j = 0u; (j < 3u) && (j < nW); j++)
               {
                  floats[(i*3u) + j] = fv[(i*nW) + j];
               }
               for(;j < 3u; j++)
               {
                  if(i != j)
                     floats[(i*3u) + j] = 0.0f;
                  else
                     floats[(i*3u) + j] = 1.0f;
               }
            }
            for(; (i < 2u); i++)
            {
               for(j = 0u; j < 3u; j++)
               {
                  if(i != j)
                     floats[(i*3u) + j] = 0.0f;
                  else
                     floats[(i*3u) + j] = 1.0f;
               }
            }

            return;
         }
         else
         {
            // Retrieve individual float elements and assign
            YAC_Value v;
            sUI i,j;
            for(i = 0u; (i < 2u) && (i < nH); i++)
            {
               for(j = 0u; (j < 3u) && (j < nW); j++)
               {
                  _o->yacArrayGet(_ctx, (i*nW)+j, &v);
                  floats[(i*3u) + j] = v.value.float_val;
               }
               for(; j < 3u; j++)
               {
                  if(i != j)
                     floats[(i*3u) + j] = 0.0f;
                  else
                     floats[(i*3u) + j] = 1.0f;
               }
            }
            for(;(i < 2u); i++)
            {
               for(j = 0u; j < 3u; j++)
               {
                  if(i != j)
                     floats[(i*3u) + j] = 0.0f;
                  else
                     floats[(i*3u) + j] = 1.0f;
               }
            }
         }
      }
      else
      {
         // Retrieve individual elements, typecast to float and assign
         YAC_Value v;
         sUI i,j;
         for(i = 0u; (i < 2u) && (i < nH); i++)
         {
            for(j = 0u; (j < 3u) && (j < nW); j++)
            {
               _o->yacArrayGet(_ctx, (i*nW)+j, &v);
               v.typecast(YAC_TYPE_FLOAT);
               floats[(i*3u) + j] = v.value.float_val;
               v.unset();
            }
            for(; j < 3u; j++)
            {
               if(i != j)
                  floats[(i*3u) + j] = 0.0f;
               else
                  floats[(i*3u) + j] = 1.0f;
            }
         }
         for(;(i < 2u); i++)
         {
            for(j = 0u; j < 3u; j++)
            {
               if(i != j)
                  floats[(i*3u) + j] = 0.0f;
               else
                  floats[(i*3u) + j] = 1.0f;
            }
         }
      }
   }
   else
   {
      // Other array is empty or no array. Quietly assign 0.0f
      floats[0] = floats[1] = floats[2] = floats[3] = floats[4] = floats[5] = 0.0f;
   }
}

void _Matrix2x3f::_setA(sF32 _m11) {
   floats[0] = _m11;
}

void _Matrix2x3f::_setB(sF32 _m12) {
   floats[1] = _m12;
}

void _Matrix2x3f::_setC(sF32 _m13) {
   floats[2] = _m13;
}

void _Matrix2x3f::_setD(sF32 _m21) {
   floats[3] = _m21;
}

void _Matrix2x3f::_setE(sF32 _m22) {
   floats[4] = _m22;
}

void _Matrix2x3f::_setF(sF32 _m23) {
   floats[5] = _m23;
}

sF32 _Matrix2x3f::_getA(void) {
   return floats[0];
}

sF32 _Matrix2x3f::_getB(void) {
   return floats[1];
}

sF32 _Matrix2x3f::_getC(void) {
   return floats[2];
}

sF32 _Matrix2x3f::_getD(void) {
   return floats[3];
}

sF32 _Matrix2x3f::_getE(void) {
   return floats[4];
}

sF32 _Matrix2x3f::_getF(void) {
   return floats[5];
}

sBool YAC_VCALL _Matrix2x3f::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(256,0,0,0);
   s->printf("((%g, %g, %g), (%g, %g, %g))",
             floats[0],floats[1],floats[2],
             floats[3],floats[4],floats[5]
             );
   s->fixLength();
   return YAC_TRUE;
}

void _Matrix2x3f::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   yacToString(s);
   YAC_RETS(s, YAC_TRUE);
}

void _Matrix2x3f::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      floats[0] += o->floats[0];
      floats[1] += o->floats[1];
      floats[2] += o->floats[2];
      floats[3] += o->floats[3];
      floats[4] += o->floats[4];
      floats[5] += o->floats[5];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::add_SELF Object not of type Matrix2x3f");
   }
}

void _Matrix2x3f::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      _Matrix2x3f *r = YAC_NEW_POOLED(Matrix2x3f);
      r->floats[0] = floats[0] + o->floats[0];
      r->floats[1] = floats[1] + o->floats[1];
      r->floats[2] = floats[2] + o->floats[2];
      r->floats[3] = floats[3] + o->floats[3];
      r->floats[4] = floats[4] + o->floats[4];
      r->floats[5] = floats[5] + o->floats[5];
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::add_VAL Object not of type Matrix2x3f");
   }
}

void _Matrix2x3f::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix2x3f) &&
      YAC_CHK(_r, clid_Matrix2x3f)
      ) {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      YAC_CAST_ARG(_Matrix2x3f, r, _r);
      r->floats[0] = floats[0] + o->floats[0];
      r->floats[1] = floats[1] + o->floats[1];
      r->floats[2] = floats[2] + o->floats[2];
      r->floats[3] = floats[3] + o->floats[3];
      r->floats[4] = floats[4] + o->floats[4];
      r->floats[5] = floats[5] + o->floats[5];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::add_ARG Object not of type Matrix2x3f");
   }
}

void _Matrix2x3f::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      floats[0] -= o->floats[0];
      floats[1] -= o->floats[1];
      floats[2] -= o->floats[2];
      floats[3] -= o->floats[3];
      floats[4] -= o->floats[4];
      floats[5] -= o->floats[5];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::sub_SELF Object not of type Matrix2x3f");
   }
}

void _Matrix2x3f::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      _Matrix2x3f *r = YAC_NEW_POOLED(Matrix2x3f);
      r->floats[0] = floats[0] - o->floats[0];
      r->floats[1] = floats[1] - o->floats[1];
      r->floats[2] = floats[2] - o->floats[2];
      r->floats[3] = floats[3] - o->floats[3];
      r->floats[4] = floats[4] - o->floats[4];
      r->floats[5] = floats[5] - o->floats[5];
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::sub_VAL Object not of type Matrix2x3f");
   }
}

void _Matrix2x3f::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix2x3f) &&
      YAC_CHK(_r, clid_Matrix2x3f)
      ) {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      YAC_CAST_ARG(_Matrix2x3f, r, _r);
      r->floats[0] = floats[0] - o->floats[0];
      r->floats[1] = floats[1] - o->floats[1];
      r->floats[2] = floats[2] - o->floats[2];
      r->floats[3] = floats[3] - o->floats[3];
      r->floats[4] = floats[4] - o->floats[4];
      r->floats[5] = floats[5] - o->floats[5];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::sub_ARG Object not of type Matrix2x3f");
   }
}

void _Matrix2x3f::_mulf_YAC_RSELF(sF32 s) {
   floats[0] *= s;
   floats[1] *= s;
   floats[2] *= s;
   floats[3] *= s;
   floats[4] *= s;
   floats[5] *= s;
}

void _Matrix2x3f::_mulf_YAC_RVAL(sF32 s, YAC_Value *_r) {
   _Matrix2x3f *r = YAC_NEW_POOLED(Matrix2x3f);
   r->floats[0] = floats[0] * s;
   r->floats[1] = floats[1] * s;
   r->floats[2] = floats[2] * s;
   r->floats[3] = floats[3] * s;
   r->floats[4] = floats[4] * s;
   r->floats[5] = floats[5] * s;
   _r->initObject(r, YAC_TRUE);
}

void _Matrix2x3f::_mulf_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, r, _r);
      r->floats[0] = floats[0] * s;
      r->floats[1] = floats[1] * s;
      r->floats[2] = floats[2] * s;
      r->floats[3] = floats[3] * s;
      r->floats[4] = floats[4] * s;
      r->floats[5] = floats[5] * s;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::mulf_ARG Return object not of type Matrix2x3f");
   }
}

void _Matrix2x3f::_mul_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      const sF32 a = floats[0]*o->floats[0] + floats[1]*o->floats[3] + floats[2]*0.0f/*o->floats[6]*/;
      const sF32 b = floats[0]*o->floats[1] + floats[1]*o->floats[4] + floats[2]*0.0f/*o->floats[7]*/;
      const sF32 c = floats[0]*o->floats[2] + floats[1]*o->floats[5] + floats[2]*1.0f/*o->floats[8]*/;
      const sF32 d = floats[3]*o->floats[0] + floats[4]*o->floats[3] + floats[5]*0.0f/*o->floats[6]*/;
      const sF32 e = floats[3]*o->floats[1] + floats[4]*o->floats[4] + floats[5]*0.0f/*o->floats[7]*/;
      const sF32 f = floats[3]*o->floats[2] + floats[4]*o->floats[5] + floats[5]*1.0f/*o->floats[8]*/;
      floats[0] = a;
      floats[1] = b;
      floats[2] = c;
      floats[3] = d;
      floats[4] = e;
      floats[5] = f;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::mul_SELF Object not of type Matrix2x3f!\n");
   }
}

void _Matrix2x3f::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      _Matrix2x3f *r = YAC_NEW_POOLED(Matrix2x3f);
      r->floats[0] = floats[0]*o->floats[0] + floats[1]*o->floats[3] + floats[2]*0.0f/*o->floats[6]*/;
      r->floats[1] = floats[0]*o->floats[1] + floats[1]*o->floats[4] + floats[2]*0.0f/*o->floats[7]*/;
      r->floats[2] = floats[0]*o->floats[2] + floats[1]*o->floats[5] + floats[2]*1.0f/*o->floats[8]*/;
      r->floats[3] = floats[3]*o->floats[0] + floats[4]*o->floats[3] + floats[5]*0.0f/*o->floats[6]*/;
      r->floats[4] = floats[3]*o->floats[1] + floats[4]*o->floats[4] + floats[5]*0.0f/*o->floats[7]*/;
      r->floats[5] = floats[3]*o->floats[2] + floats[4]*o->floats[5] + floats[5]*1.0f/*o->floats[8]*/;
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::mul_VAL Object not of type Matrix2x3f!\n");
   }
}

void _Matrix2x3f::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix2x3f) &&
      YAC_CHK(_r, clid_Matrix2x3f)
      )
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      YAC_CAST_ARG(_Matrix2x3f, r, _r);

      if((void*)_r != (void*)this)
      {
         r->floats[0] = floats[0]*o->floats[0] + floats[1]*o->floats[3] + floats[2]*0.0f/*o->floats[6]*/;
         r->floats[1] = floats[0]*o->floats[1] + floats[1]*o->floats[4] + floats[2]*0.0f/*o->floats[7]*/;
         r->floats[2] = floats[0]*o->floats[2] + floats[1]*o->floats[5] + floats[2]*1.0f/*o->floats[8]*/;
         r->floats[3] = floats[3]*o->floats[0] + floats[4]*o->floats[3] + floats[5]*0.0f/*o->floats[6]*/;
         r->floats[4] = floats[3]*o->floats[1] + floats[4]*o->floats[4] + floats[5]*0.0f/*o->floats[7]*/;
         r->floats[5] = floats[3]*o->floats[2] + floats[4]*o->floats[5] + floats[5]*1.0f/*o->floats[8]*/;
      }
      else
      {
         const sF32 a = floats[0]*o->floats[0] + floats[1]*o->floats[3] + floats[2]*0.0f/*o->floats[6]*/;
         const sF32 b = floats[0]*o->floats[1] + floats[1]*o->floats[4] + floats[2]*0.0f/*o->floats[7]*/;
         const sF32 c = floats[0]*o->floats[2] + floats[1]*o->floats[5] + floats[2]*1.0f/*o->floats[8]*/;
         const sF32 d = floats[3]*o->floats[0] + floats[4]*o->floats[3] + floats[5]*0.0f/*o->floats[6]*/;
         const sF32 e = floats[3]*o->floats[1] + floats[4]*o->floats[4] + floats[5]*0.0f/*o->floats[7]*/;
         const sF32 f = floats[3]*o->floats[2] + floats[4]*o->floats[5] + floats[5]*1.0f/*o->floats[8]*/;

         r->floats[0] = a;
         r->floats[1] = b;
         r->floats[2] = c;
         r->floats[3] = d;
         r->floats[4] = e;
         r->floats[5] = f;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::mul_ARG Object not of type Matrix2x3f!\n");
   }
}

void _Matrix2x3f::_mulRev_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      const sF32 a = o->floats[0]*floats[0] + o->floats[1]*floats[3] + o->floats[2]*0.0f/*floats[6]*/;
      const sF32 b = o->floats[0]*floats[1] + o->floats[1]*floats[4] + o->floats[2]*0.0f/*floats[7]*/;
      const sF32 c = o->floats[0]*floats[2] + o->floats[1]*floats[5] + o->floats[2]*1.0f/*floats[8]*/;
      const sF32 d = o->floats[3]*floats[0] + o->floats[4]*floats[3] + o->floats[5]*0.0f/*floats[6]*/;
      const sF32 e = o->floats[3]*floats[1] + o->floats[4]*floats[4] + o->floats[5]*0.0f/*floats[7]*/;
      const sF32 f = o->floats[3]*floats[2] + o->floats[4]*floats[5] + o->floats[5]*1.0f/*floats[8]*/;
      floats[0] = a;
      floats[1] = b;
      floats[2] = c;
      floats[3] = d;
      floats[4] = e;
      floats[5] = f;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::mulRev_SELF Object not of type Matrix2x3f!\n");
   }
}

void _Matrix2x3f::_mulRev_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      _Matrix2x3f *r = YAC_NEW_POOLED(Matrix2x3f);
      r->floats[0] = o->floats[0]*floats[0] + o->floats[1]*floats[3] + o->floats[2]*0.0f/*floats[6]*/;
      r->floats[1] = o->floats[0]*floats[1] + o->floats[1]*floats[4] + o->floats[2]*0.0f/*floats[7]*/;
      r->floats[2] = o->floats[0]*floats[2] + o->floats[1]*floats[5] + o->floats[2]*1.0f/*floats[8]*/;
      r->floats[3] = o->floats[3]*floats[0] + o->floats[4]*floats[3] + o->floats[5]*0.0f/*floats[6]*/;
      r->floats[4] = o->floats[3]*floats[1] + o->floats[4]*floats[4] + o->floats[5]*0.0f/*floats[7]*/;
      r->floats[5] = o->floats[3]*floats[2] + o->floats[4]*floats[5] + o->floats[5]*1.0f/*floats[8]*/;
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::mulRev_VAL Object not of type Matrix2x3f!\n");
   }
}

void _Matrix2x3f::_mulRev_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix2x3f) &&
      YAC_CHK(_r, clid_Matrix2x3f)
      )
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      YAC_CAST_ARG(_Matrix2x3f, r, _r);

      if((void*)_r != (void*)this)
      {
         r->floats[0] = o->floats[0]*floats[0] + o->floats[1]*floats[3] + o->floats[2]*0.0f/*floats[6]*/;
         r->floats[1] = o->floats[0]*floats[1] + o->floats[1]*floats[4] + o->floats[2]*0.0f/*floats[7]*/;
         r->floats[2] = o->floats[0]*floats[2] + o->floats[1]*floats[5] + o->floats[2]*1.0f/*floats[8]*/;
         r->floats[3] = o->floats[3]*floats[0] + o->floats[4]*floats[3] + o->floats[5]*0.0f/*floats[6]*/;
         r->floats[4] = o->floats[3]*floats[1] + o->floats[4]*floats[4] + o->floats[5]*0.0f/*floats[7]*/;
         r->floats[5] = o->floats[3]*floats[2] + o->floats[4]*floats[5] + o->floats[5]*1.0f/*floats[8]*/;
      }
      else
      {
         const sF32 a = o->floats[0]*floats[0] + o->floats[1]*floats[3] + o->floats[2]*0.0f/*floats[6]*/;
         const sF32 b = o->floats[0]*floats[1] + o->floats[1]*floats[4] + o->floats[2]*0.0f/*floats[7]*/;
         const sF32 c = o->floats[0]*floats[2] + o->floats[1]*floats[5] + o->floats[2]*1.0f/*floats[8]*/;
         const sF32 d = o->floats[3]*floats[0] + o->floats[4]*floats[3] + o->floats[5]*0.0f/*floats[6]*/;
         const sF32 e = o->floats[3]*floats[1] + o->floats[4]*floats[4] + o->floats[5]*0.0f/*floats[7]*/;
         const sF32 f = o->floats[3]*floats[2] + o->floats[4]*floats[5] + o->floats[5]*1.0f/*floats[8]*/;
         r->floats[0] = a;
         r->floats[1] = b;
         r->floats[2] = c;
         r->floats[3] = d;
         r->floats[4] = e;
         r->floats[5] = f;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::mulRev_ARG Object not of type Matrix2x3f!\n");
   }
}

void _Matrix2x3f::_mulv_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      _Vector2f *r = YAC_NEW_POOLED(Vector2f);
      // row major
      r->floats[0] = floats[0]*o->floats[0] + floats[1]*o->floats[1] + floats[2]*1.0f/*o->floats[2]*/;
      r->floats[1] = floats[3]*o->floats[0] + floats[4]*o->floats[1] + floats[5]*1.0f/*o->floats[2]*/;
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::mulv Object not of type Vector2f");
   }
}

void _Matrix2x3f::_mulv_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Vector2f) &&
      YAC_CHK(_r, clid_Vector2f)
      )
   {
      YAC_CAST_ARG(_Vector2f, o, _o);
      YAC_CAST_ARG(_Vector2f, r, _r);

      if((void*)o != (void*)r)
      {
         r->floats[0] = floats[0]*o->floats[0] + floats[1]*o->floats[1] + floats[2]*1.0f/*o->floats[2]*/;
         r->floats[1] = floats[3]*o->floats[0] + floats[4]*o->floats[1] + floats[5]*1.0f/*o->floats[2]*/;
      }
      else
      {
         const sF32 x = floats[0]*o->floats[0] + floats[1]*o->floats[1] + floats[2]*1.0f/*o->floats[2]*/;
         const sF32 y = floats[3]*o->floats[0] + floats[4]*o->floats[1] + floats[5]*1.0f/*o->floats[2]*/;
         r->floats[0] = x;
         r->floats[1] = y;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::mulv Object not of type Vector2f");
   }
}

sF32 _Matrix2x3f::_getAbs(void) const {
   return
      sqrtf(floats[0]*floats[0] + floats[1]*floats[1] + floats[2]*floats[2] +
            floats[3]*floats[3] + floats[4]*floats[4] + floats[5]*floats[5]
            );
}

sF32 _Matrix2x3f::_getAbsSqr(void) const {
   return
      floats[0]*floats[0] + floats[1]*floats[1] + floats[2]*floats[2] +
      floats[3]*floats[3] + floats[4]*floats[4] + floats[5]*floats[5] ;
}

void _Matrix2x3f::_unit_YAC_RSELF(void) {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = 1.0f / absval;
      floats[0] *= absval;
      floats[1] *= absval;
      floats[2] *= absval;
      floats[3] *= absval;
      floats[4] *= absval;
      floats[5] *= absval;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2x3f::unit_SELF Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix2x3f::_unit_YAC_RVAL(YAC_Value *_r) {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      _Matrix2x3f *r = YAC_NEW_POOLED(Matrix2x3f);
      absval = 1.0f / absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
      r->floats[2] = floats[2] * absval;
      r->floats[3] = floats[3] * absval;
      r->floats[4] = floats[4] * absval;
      r->floats[5] = floats[5] * absval;
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2x3f::unit_VAL Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix2x3f::_unit_YAC_RARG(YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Matrix2x3f))
   {
      sF32 absval = _getAbs();
      if(Dfltnonzero(absval))
      {
         YAC_CAST_ARG(_Matrix2x3f, r, _r);
         absval = 1.0f / absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
         r->floats[2] = floats[2] * absval;
         r->floats[3] = floats[3] * absval;
         r->floats[4] = floats[4] * absval;
         r->floats[5] = floats[5] * absval;
      }
      else
      {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2x3f::unit_SELF Zero Matrix cannot be normalized!\n");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::unit_ARG Return object not of type Matrix2x3f");
   }
}

void _Matrix2x3f::_unitScale_YAC_RSELF(sF32 s) {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = s / absval;
      floats[0] *= absval;
      floats[1] *= absval;
      floats[2] *= absval;
      floats[3] *= absval;
      floats[4] *= absval;
      floats[5] *= absval;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2x3f::unitScale_SELF Zero Matrix cannot be normalized");
   }
}

void _Matrix2x3f::_unitScale_YAC_RVAL(sF32 s, YAC_Value *_r) {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      _Matrix2x3f *r = YAC_NEW_POOLED(Matrix2x3f);
      absval = s / absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
      r->floats[2] = floats[2] * absval;
      r->floats[3] = floats[3] * absval;
      r->floats[4] = floats[4] * absval;
      r->floats[5] = floats[5] * absval;
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2x3f::unitScale_VAL Zero Matrix cannot be normalized");
   }
}

void _Matrix2x3f::_unitScale_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Matrix2x3f))
   {
      sF32 absval = _getAbs();
      if(Dfltnonzero(absval))
      {
         YAC_CAST_ARG(_Matrix2x3f, r, _r);
         absval = s / absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
         r->floats[2] = floats[2] * absval;
         r->floats[3] = floats[3] * absval;
         r->floats[4] = floats[4] * absval;
         r->floats[5] = floats[5] * absval;
      }
      else
      {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2x3f::unitScale_RARG: zero matrix cannot be normalized");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::unitScale_RARG: return object is not a Matrix2x3f");
   }
}

void _Matrix2x3f::_setRow(sUI _row, YAC_Object *_o) {
   _initRowv(_row, _o);
}

void _Matrix2x3f::_getRow_YAC_RVAL(sUI _row, YAC_Value *_r) {
   if(_row < 2u)
   {
      _Vector3f *r = YAC_NEW_POOLED(Vector3f);
      r->_init(TM2x3F(_row, 0), TM2x3F(_row, 1), TM2x3F(_row, 2));
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::getRow_VAL: invalid row index (%u > 1)\n", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2x3f::_getRow_YAC_RARG(sUI _row, YAC_Object *_r) const {
   if(_row < 2u)
   {
      if(YAC_CHK(_r, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, r, _r);
         r->_init(TM2x3F(_row, 0), TM2x3F(_row, 1), TM2x3F(_row, 2));
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix2x3f::getRow_RARG: return object is not a Vector3f");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::getRow_RARG: invalid row index (%u > 1)\n", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2x3f::_setCol(sUI _col, YAC_Object *_o) {
   _initColv(_col, _o);
}

void _Matrix2x3f::_getCol_YAC_RVAL(sUI _col, YAC_Value *_r) {
   if(_col < 3u)
   {
      _Vector2f *r = YAC_NEW_POOLED(Vector2f);
      r->_init(TM2x3F(0, _col),
               TM2x3F(1, _col)
               );
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::getCol_VAL: invalid row index (%u > 2)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2x3f::_getCol_YAC_RARG(sUI _col, YAC_Object *_r) const {
   if(_col < 3u)
   {
      if(YAC_CHK(_r, clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, r, _r);
         r->_init(TM2x3F(0, _col),
                  TM2x3F(1, _col)
                  );
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix2x3f::getCol_RARG: return object is not a Vector2f");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::getCol_RARG: invalid row index (%u > 2)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2x3f::_initIdentity(void) {
   TM2x3F(0,0) = 1.0f;  TM2x3F(0,1) = 0.0f;  TM2x3F(0,2) = 0.0f;
   TM2x3F(1,0) = 0.0f;  TM2x3F(1,1) = 1.0f;  TM2x3F(1,2) = 0.0f;
}

void _Matrix2x3f::_init(sF32 a, sF32 b, sF32 c, sF32 d, sF32 e, sF32 f) {
   TM2x3F(0,0) = a;  TM2x3F(0,1) = b;  TM2x3F(0,2) = c;
   TM2x3F(1,0) = d;  TM2x3F(1,1) = e;  TM2x3F(1,2) = f;
}

void _Matrix2x3f::_initRowf(sUI _row, sF32 _x, sF32 _y, sF32 _z) {
   if(_row < 2u)
   {
      TM2x3F(_row,0) = _x;  TM2x3F(_row,1) = _y;  TM2x3F(_row,2) = _z;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::initRowf: invalid row index (%u > 1)", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2x3f::_initRowv(sUI _row, YAC_Object *_o) {
   if(_row < 2u)
   {
      if(YAC_CHK(_o, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, o, _o);
         TM2x3F(_row, 0) = o->floats[0];  TM2x3F(_row, 1) = o->floats[1];  TM2x3F(_row, 2) = o->floats[2];
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector3f, t);
         t.assignGeneric(NULL, _o);
         TM2x3F(_row, 0) = t.floats[0];  TM2x3F(_row, 1) = t.floats[1];  TM2x3F(_row, 2) = t.floats[2];
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix2x3f::initRowv: invalid arg object");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::initRowv: invalid row index (%u > 1)\n", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2x3f::_initColf(sUI _col, sF32 _x, sF32 _y) {
   if(_col < 3u)
   {
      TM2x3F(0, _col) = _x;
      TM2x3F(1, _col) = _y;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::initColf: invalid column index (%u > 2)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2x3f::_initColv(sUI _col, YAC_Object *_o) {
   if(_col < 3u)
   {
      if(YAC_CHK(_o, clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, o, _o);
         TM2x3F(0, _col) = o->floats[0];
         TM2x3F(1, _col) = o->floats[1];
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector2f, t);
         t.assignGeneric(NULL, _o);
         TM2x3F(0, _col) = t.floats[0];
         TM2x3F(1, _col) = t.floats[1];
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix2x3f::initColv: invalid arg object");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2x3f::initColv: invalid column index (%u > 2)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2x3f::_initOrtho(sF32 _left, sF32 _right,
                             sF32 _bottom, sF32 _top
                             ) {
   /*
    *  2/(r-l)   0        -(r+l)/(r-l)
    *  0         2/(t-b)  -(t+b)/(t-b)
    *
    */
   sF32 rml = _right - _left;
   sF32 tmb = _top   - _bottom;

   if(0.0f != rml && tmb != 0.0f)
   {
      TM2x3F(0, 0) = 2.0f / rml;
      TM2x3F(0, 1) = 0.0f;
      TM2x3F(0, 2) = -(_right + _left) / rml;

      TM2x3F(1, 0) = 0.0f;
      TM2x3F(1, 1) = 2.0f / tmb;
      TM2x3F(1, 2) = -(_top + _bottom) / tmb;
   }
}

void _Matrix2x3f::_initTranslatef(sF32 _x, sF32 _y) {
   _init(1, 0, _x,
         0, 1, _y
         );
}

void _Matrix2x3f::_initTranslatev(YAC_Object *_translate) {
   if(YAC_CHK(_translate, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, translate, _translate);
      _init(1, 0, translate->floats[0],
            0, 1, translate->floats[1]
            );
   }
   else if(YAC_VALID(_translate))
   {
      YAC_NEW_STACK(Vector2f, v);
      v.assignGeneric(NULL/*context*/, _translate);
      _init(1, 0, v.floats[0],
            0, 1, v.floats[1]
            );
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::initTranslatev: invalid arg object (not a Vector2f)");
   }
}

void _Matrix2x3f::_initScalef(sF32 _x, sF32 _y) {
   _init(_x,  0,  0,
         0,  _y,  0
         );
}

void _Matrix2x3f::_initScalev(YAC_Object *_scale) {
   if(YAC_CHK(_scale, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, scale, _scale);
      _init(scale->floats[0], 0,                0,
            0,                scale->floats[1], 0
            );
   }
   else if(YAC_VALID(_scale))
   {
      YAC_NEW_STACK(Vector2f, v);
      v.assignGeneric(NULL/*context*/, _scale);
      _init(v.floats[0], 0,           0,
            0,           v.floats[1], 0
            );
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::initScalev: invalid arg object (not a Vector2f)");
   }
}

void _Matrix2x3f::_initSkewXf(sF32 _a) {
   _init(1.0f, tanf(_a), 0.0f,
         0.0f, 1.0f,     0.0f
         );
}

void _Matrix2x3f::_initSkewYf(sF32 _a) {
   _init(1.0f,     0.0f, 0.0f,
         tanf(_a), 1.0f, 0.0f
         );
}

void _Matrix2x3f::_initRotatef(sF32 _a) {
   const sF32 s = sinf(_a);
   const sF32 c = cosf(_a);
   _init(c,  -s, 0.0f,
         s,   c, 0.0f
         );
}

void _Matrix2x3f::_translatef_YAC_RSELF(sF32 _x, sF32 _y) {
   YAC_NEW_STACK(Matrix2x3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initTranslatef(_x, _y);
   _mul_YAC_RSELF(&t);
}

void _Matrix2x3f::_translatef_YAC_RVAL(sF32 _x, sF32 _y, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix2x3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initTranslatef(_x, _y);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix2x3f::_translatef_YAC_RARG(sF32 _x, sF32 _y, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix2x3f))
   {
      YAC_NEW_STACK(Matrix2x3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initTranslatef(_x, _y);
      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::translatef_RARG: Object not of type Matrix2x3f");
   }
}

void _Matrix2x3f::_translatev_YAC_RSELF(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, v, _v);
      YAC_NEW_STACK(Matrix2x3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initTranslatef(v->floats[0], v->floats[1]);
      _mul_YAC_RSELF(&t);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix2x3f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector2f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initTranslatef(v.floats[0], v.floats[1]);
      _mul_YAC_RSELF(&t);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::translatev_RARG: 'v' is not a Vector2f");
   }
}

void _Matrix2x3f::_translatev_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if(YAC_CHK(_v, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, v, _v);
      YAC_NEW_STACK(Matrix2x3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initTranslatef(v->floats[0], v->floats[1]);
      _mul_YAC_RVAL(&t, _r);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix2x3f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector2f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initTranslatef(v.floats[0], v.floats[1]);
      _mul_YAC_RVAL(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::translatev_RVAL: 'v' is not a Vector2f");
   }
}

void _Matrix2x3f::_translatev_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix2x3f))
   {
      if(YAC_CHK(_v, clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, v, _v);
         YAC_NEW_STACK(Matrix2x3f, t);
         t._setColumnMajor(_getColumnMajor());
         t._initTranslatef(v->floats[0], v->floats[1]);
         _mul_YAC_RARG(&t, _r);
      }
      else if(YAC_VALID(_v))
      {
         YAC_NEW_STACK(Matrix2x3f, t);
         t._setColumnMajor(_getColumnMajor());
         YAC_NEW_STACK(Vector2f, v);
         v.assignGeneric(NULL/*context*/, _v);
         t._initTranslatef(v.floats[0], v.floats[1]);
         _mul_YAC_RARG(&t, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::translatev_RARG: 'v' is not a Vector2f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::translatev_RARG: return object is not a Matrix3f");
   }
}

void _Matrix2x3f::_scalef_YAC_RSELF(sF32 _x, sF32 _y) {
   YAC_NEW_STACK(Matrix2x3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initScalef(_x, _y);
   _mul_YAC_RSELF(&t);
}

void _Matrix2x3f::_scalef_YAC_RVAL(sF32 _x, sF32 _y, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix2x3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initScalef(_x, _y);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix2x3f::_scalef_YAC_RARG(sF32 _x, sF32 _y, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix2x3f))
   {
      YAC_NEW_STACK(Matrix2x3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(_x, _y);
      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::scalef_RARG: Object not of type Matrix2x3f");
   }
}

void _Matrix2x3f::_scalev_YAC_RSELF(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, v, _v);
      YAC_NEW_STACK(Matrix2x3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(v->floats[0], v->floats[1]);
      _mul_YAC_RSELF(&t);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix2x3f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector2f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initScalef(v.floats[0], v.floats[1]);
      _mul_YAC_RSELF(&t);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::scalev_RARG: 'v' is not a Vector2f");
   }
}

void _Matrix2x3f::_scalev_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if(YAC_CHK(_v, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, v, _v);
      YAC_NEW_STACK(Matrix2x3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(v->floats[0], v->floats[1]);
      _mul_YAC_RVAL(&t, _r);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix2x3f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector2f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initScalef(v.floats[0], v.floats[1]);
      _mul_YAC_RVAL(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::scalev_RVAL: 'v' is not a Vector2f");
   }
}

void _Matrix2x3f::_scalev_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix2x3f))
   {
      if(YAC_CHK(_v, clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, v, _v);
         YAC_NEW_STACK(Matrix2x3f, t);
         t._setColumnMajor(_getColumnMajor());
         t._initScalef(v->floats[0], v->floats[1]);
         _mul_YAC_RARG(&t, _r);
      }
      else if(YAC_VALID(_v))
      {
         YAC_NEW_STACK(Matrix2x3f, t);
         t._setColumnMajor(_getColumnMajor());
         YAC_NEW_STACK(Vector2f, v);
         v.assignGeneric(NULL/*context*/, _v);
         t._initScalef(v.floats[0], v.floats[1]);
         _mul_YAC_RARG(&t, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::scalev_RARG: 'v' is not a Vector2f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::scalev_RARG: return object is not a Matrix2x3f");
   }
}

void _Matrix2x3f::_rotatef_YAC_RSELF(sF32 _ang) {
   YAC_NEW_STACK(Matrix2x3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initRotatef(_ang);
   _mul_YAC_RSELF(&t);
}

void _Matrix2x3f::_rotatef_YAC_RVAL(sF32 _ang, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix2x3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initRotatef(_ang);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix2x3f::_rotatef_YAC_RARG(sF32 _ang, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix2x3f))
   {
      YAC_NEW_STACK(Matrix2x3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initRotatef(_ang);
      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2x3f::rotatef_RARG: Object not of type Matrix2x3f");
   }
}

void _Matrix2x3f::_New(sF32 a, sF32 b, sF32 c, sF32 d, sF32 e, sF32 f, YAC_Value *_r) {
   _Matrix2x3f *r = YAC_NEW_POOLED(Matrix2x3f);
   r->_init(a, b, c, d, e, f);
   _r->initObject(r, YAC_TRUE);
}

void YAC_CALL _matrix2x3f(sF32 a, sF32 b, sF32 c, sF32 d, sF32 e, sF32 f, YAC_Value *_r) {
   _Matrix2x3f *r = YAC_NEW_POOLED(Matrix2x3f);
   r->_init(a, b, c, d, e, f);
   _r->initObject(r, YAC_TRUE);
}
