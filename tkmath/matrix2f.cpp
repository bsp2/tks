/// matrix2f.cpp
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
#include "matrix2f.h"

#include "ying_Math.h"


_Matrix2f::_Matrix2f(void) {
   _initIdentity();
}

_Matrix2f::~_Matrix2f() {
}

sUI YAC_VCALL _Matrix2f::yacArrayGetNumElements(void) {
   return 4u;
}

sUI YAC_VCALL _Matrix2f::yacArrayGetMaxElements(void) {
   return 4u;
}

sUI YAC_VCALL _Matrix2f::yacArrayGetElementType(void) {
   return YAC_TYPE_FLOAT;
}

sUI YAC_VCALL _Matrix2f::yacArrayGetElementByteSize(void) {
   return sizeof(sF32);
}

void YAC_VCALL _Matrix2f::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4u)
   {
      sF32 v = 0.0f;

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
               (void)_val->value.object_val->yacScanF32(&v);
            break;

         default:
            Dyac_throw(TypeMismatch, "tkmath::Matrix2f::yacArraySet Unknown Type!");
            break;
      }
      floats[_idx] = v;
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix2f::yacArraySet Index too high (%u > 3)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Matrix2f::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4u)
   {
      _val->initFloat(floats[_idx]);
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix2f::yacArrayGet Index too high (%d>3)", _idx);
      Dyac_throw(ReadArrayOutOfBounds, buf);
      _val->initFloat(0.0f);
   }
}

void *YAC_VCALL _Matrix2f::yacArrayGetPointer(void) {
   return (void*)floats;
}

sUI YAC_VCALL _Matrix2f::yacArrayGetWidth(void) {
   return 2u;
}

sUI YAC_VCALL _Matrix2f::yacArrayGetHeight(void) {
   return 2u;
}

sUI YAC_VCALL _Matrix2f::yacArrayGetStride(void) {
   return 2u * sizeof(sF32);
}

sSI YAC_VCALL _Matrix2f::yacTensorRank(void) {
   return YAC_TENSOR_RANK_MATRIX;
}

void YAC_VCALL _Matrix2f::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F32(floats[0]);
   YAC_SERIALIZE_F32(floats[1]);
   YAC_SERIALIZE_F32(floats[2]);
   YAC_SERIALIZE_F32(floats[3]);
}

sUI YAC_VCALL _Matrix2f::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   floats[0]=YAC_DESERIALIZE_F32();
   floats[1]=YAC_DESERIALIZE_F32();
   floats[2]=YAC_DESERIALIZE_F32();
   floats[3]=YAC_DESERIALIZE_F32();
   return 1;
}

void YAC_VCALL _Matrix2f::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Matrix2f) &&
      YAC_CHK(_max, clid_Matrix2f)
      )
   {
      YAC_CAST_ARG(_Matrix2f, min, _min);
      YAC_CAST_ARG(_Matrix2f, max, _max);

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
      Dyac_throw_def(InvalidPointer, "");
   }
}

void YAC_VCALL _Matrix2f::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Matrix2f) &&
      YAC_CHK(_max, clid_Matrix2f)
      )
   {
      YAC_CAST_ARG(_Matrix2f, min, _min);
      YAC_CAST_ARG(_Matrix2f, max, _max);

      for(sUI i = 0u; i < 4u; i++)
      {
         if(floats[i] < min->floats[i])
            floats[i] += max->floats[i] - min->floats[i];
         else if(floats[i] > max->floats[i])
            floats[i] -= max->floats[i] - min->floats[i];
      }

   } else Dyac_throw_def(InvalidPointer, "");
}

void YAC_VCALL _Matrix2f::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);

      switch(_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI( Dfltequal(floats[0], o->floats[0]) &&
                      Dfltequal(floats[1], o->floats[1]) &&
                      Dfltequal(floats[2], o->floats[2]) &&
                      Dfltequal(floats[3], o->floats[3])
                      );
            break;

         case YAC_OP_CNE:
            YAC_RETI( Dfltnotequal(floats[0], o->floats[0]) ||
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
            break;

         default:
         {
            char buf[128];
            Dyac_snprintf(buf, 128, "tkmath::Matrix2f::yacOperator: unsupported cmd %d!\n", _cmd);
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
            if(o->num_elements < 4u)
            {
               sUI i;
               for(i = 0u; i < o->num_elements; i++)
                  floats[i] = o->elements[i];
               for(;i < 4u; i++)
                  floats[i] = 0.0f;
            }
            else
            {
               floats[0] = o->elements[0];
               floats[1] = o->elements[1];
               floats[2] = o->elements[2];
               floats[3] = o->elements[3];
            }
            break;

         default:
            {
               YAC_NEW_STACK(Matrix2f, t);
               t.assignFloatArray(o);
               yacOperator(_cmd, &t, _r);
               // Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix2f::yacOperator Only Assign supported with FloatArray Type");
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
         switch (_cmd)
         {
            case YAC_OP_ASSIGN:
               assignGeneric(NULL,_o);
               break;

            default:
               Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix2f::yacOperator Only Assign supported with Generic Object Type");
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
            YAC_Object::yacOperator(_cmd,_o,_r);
            break;
      }
   }
}

void _Matrix2f::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF32(_cmd, (sF32)val, _r);
}

void _Matrix2f::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
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
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2f::yacOperatorF32 OP_DIV Cannot divide by zero");
         }
         break;

      default:
         char buf[128];
         Dyac_snprintf(buf, 128, "tkmath::Matrix2f::yacOperatorF32 Operator not supported %d",_cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
         break;
   }
}


void _Matrix2f::assignFloats(sF32 *_fv, sUI _ne) {
   if(4u != _ne)
   {
      Dyac_throw_def(WriteArrayOutOfBounds,"tkmath::Matrix2f Array not of size 4");
   }
   else
   {
      floats[0] = _fv[0];
      floats[1] = _fv[1];
      floats[2] = _fv[2];
      floats[3] = _fv[3];
   }
}

void _Matrix2f::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Matrix2f::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
   sUI ne = _o->yacArrayGetNumElements();
   sUI nH = _o->yacArrayGetHeight();
   sUI nW = _o->yacArrayGetWidth();
   if(0u == nH)
   {
      if(4u == nW)
      {
         //exact number to fill the matrix - we can go on
         nH = 2u;
         nW = 2u;
      }
      else
      {
         //nah, throw an exception
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix2f Array not of size 4");
      }
   }
   if(ne > 0u)
   {
      if(YAC_TYPE_FLOAT == _o->yacArrayGetElementType())
      {
         const sF32 *fv = (const sF32*) _o->yacArrayGetPointer();
         if(NULL != fv)
         {
            // Copy fast
            sUI i,j;
            for (i=0;(i<2)&&(i<nH);i++) {
               for (j=0;(j<2)&&(j<nW);j++) {
                  floats[(i*2)+j]=fv[(i*nW)+j];
               }
               for (;j<2;j++) {
                  if (i!=j) floats[(i*2)+j]=0.0f;
                  else floats[(i*2)+j]=1.0f;
               }
            }
            for (;(i<2);i++) {
               for (j=0;j<2;j++) {
                  if (i!=j) floats[(i*2)+j]=0.0f;
                  else floats[(i*2)+j]=1.0f;
               }
            }

            return;
         }
         else
         {
            // Retrieve individual float elements and assign
            YAC_Value v;
            sUI i,j;
            for (i=0;(i<2)&&(i<nH);i++) {
               for (j=0;(j<2)&&(j<nW);j++) {
                  _o->yacArrayGet(_ctx, (i*nW)+j, &v);
                  floats[(i*2)+j]=v.value.float_val;
               }
               for (;j<2;j++) {
                  if (i!=j) floats[(i*2)+j]=0.0f;
                  else floats[(i*2)+j]=1.0f;
               }
            }
            for (;(i<2);i++) {
               for (j=0;j<2;j++) {
                  if (i!=j) floats[(i*2)+j]=0.0f;
                  else floats[(i*2)+j]=1.0f;
               }
            }
         }
      }
      else
      {
         // Retrieve individual elements, typecast to float and assign
         YAC_Value v;
         sUI i,j;
         for (i=0;(i<2)&&(i<nH);i++) {
            for (j=0;(j<2)&&(j<nW);j++) {
               _o->yacArrayGet(_ctx, (i*nW)+j, &v);
               v.typecast(YAC_TYPE_FLOAT);
               floats[(i*2)+j]=v.value.float_val;
               v.unset();
            }
            for (;j<2;j++) {
               if (i!=j) floats[(i*2)+j]=0.0f;
               else floats[(i*2)+j]=1.0f;
            }
         }
         for (;(i<2);i++) {
            for (j=0;j<2;j++) {
               if (i!=j) floats[(i*2)+j]=0.0f;
               else floats[(i*2)+j]=1.0f;
            }
         }
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      floats[0]=floats[1]=floats[2]=floats[3]=0.0f;
   }
}

void _Matrix2f::_setA(sF32 x) {
   floats[0] = x;
}

void _Matrix2f::_setB(sF32 y) {
   floats[1] = y;
}

void _Matrix2f::_setC(sF32 x) {
   floats[2] = x;
}

void _Matrix2f::_setD(sF32 y) {
   floats[3] = y;
}

sF32 _Matrix2f::_getA(void) {
   return floats[0];
}

sF32 _Matrix2f::_getB(void) {
   return floats[1];
}

sF32 _Matrix2f::_getC(void) {
   return floats[2];
}

sF32 _Matrix2f::_getD(void) {
   return floats[3];
}

sBool YAC_VCALL _Matrix2f::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(128,0,0,0);
   s->printf("((%f, %f), (%f, %f))",floats[0],floats[1],floats[2],floats[3]);
   s->fixLength();
   return 1;
}

void _Matrix2f::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   yacToString(s);
   YAC_RETS(s, 1);
}

void _Matrix2f::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o,clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      floats[0] += o->floats[0];
      floats[1] += o->floats[1];
      floats[2] += o->floats[2];
      floats[3] += o->floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::add_SELF Object not of type Matrix2f");
   }
}

void _Matrix2f::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      _Matrix2f *r = YAC_NEW_POOLED(Matrix2f);
      _r->initObject(r, YAC_TRUE);
      r->floats[0] = floats[0] + o->floats[0];
      r->floats[1] = floats[1] + o->floats[1];
      r->floats[2] = floats[2] + o->floats[2];
      r->floats[3] = floats[3] + o->floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::add_VAL Object not of type Matrix2f");
   }
}

void _Matrix2f::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix2f)&&YAC_CHK(_r, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      YAC_CAST_ARG(_Matrix2f, r, _r);
      r->floats[0] = floats[0] + o->floats[0];
      r->floats[1] = floats[1] + o->floats[1];
      r->floats[2] = floats[2] + o->floats[2];
      r->floats[3] = floats[3] + o->floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::add_ARG Object not of type Matrix2f");
   }
}

void _Matrix2f::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o,clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      floats[0] -= o->floats[0];
      floats[1] -= o->floats[1];
      floats[2] -= o->floats[2];
      floats[3] -= o->floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::sub_SELF Object not of type Matrix2f");
   }
}

void _Matrix2f::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      _Matrix2f *r = YAC_NEW_POOLED(Matrix2f);
      _r->initObject(r, YAC_TRUE);
      r->floats[0] = floats[0] - o->floats[0];
      r->floats[1] = floats[1] - o->floats[1];
      r->floats[2] = floats[2] - o->floats[2];
      r->floats[3] = floats[3] - o->floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::sub_VAL Object not of type Matrix2f");
   }
}

void _Matrix2f::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix2f) &&
      YAC_CHK(_r, clid_Matrix2f)
      )
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      YAC_CAST_ARG(_Matrix2f, r, _r);
      r->floats[0] = floats[0] - o->floats[0];
      r->floats[1] = floats[1] - o->floats[1];
      r->floats[2] = floats[2] - o->floats[2];
      r->floats[3] = floats[3] - o->floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::sub_ARG Object not of type Matrix2f");
   }
}

void _Matrix2f::_mulf_YAC_RSELF(sF32 _s) {
   floats[0] *= _s;
   floats[1] *= _s;
   floats[2] *= _s;
   floats[3] *= _s;
}

void _Matrix2f::_mulf_YAC_RVAL(sF32 _s, YAC_Value *_r) {
   _Matrix2f *r = YAC_NEW_POOLED(Matrix2f);
   r->floats[0] = floats[0] * _s;
   r->floats[1] = floats[1] * _s;
   r->floats[2] = floats[2] * _s;
   r->floats[3] = floats[3] * _s;
   _r->initObject(r, YAC_TRUE);
}

void _Matrix2f::_mulf_YAC_RARG(sF32 _s, YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, r, _r);
      r->floats[0] = floats[0] * _s;
      r->floats[1] = floats[1] * _s;
      r->floats[2] = floats[2] * _s;
      r->floats[3] = floats[3] * _s;
   }
   else
   {
     Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::mulf_ARG Return object not of type Matrix2f");
   }
}

static inline void loc_mul(sF32 *_d, const sF32 *_a, const sF32 *const _b) {
// (note) row major
#define Df(fa,row,col) ((fa)[row*2u + col])
   for(sUI row = 0u; row < 2u; row++)
   {
      for(sUI col = 0u; col < 2u; col++)
      {
         Df(_d,row,col) = Df(_a,row,0) * Df(_b,0,col)  +  Df(_a,row,1) * Df(_b,1,col);
      }
   }
#undef Df
}

void _Matrix2f::_mul_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      float t[2*2];
      loc_mul(t, floats, o->floats);
      floats[0] = t[0];
      floats[1] = t[1];
      floats[2] = t[2];
      floats[3] = t[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::mul_RSELF: Object not of type Matrix2f!\n");
   }
}

void _Matrix2f::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      _Matrix2f *r = YAC_NEW_POOLED(Matrix2f);
      _r->initObject(r, YAC_TRUE);
      loc_mul(r->floats, floats, o->floats);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::mul_RVAL: Object not of type Matrix2f!\n");
   }
}

void _Matrix2f::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix2f) &&
      YAC_CHK(_r, clid_Matrix2f)
      )
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      YAC_CAST_ARG(_Matrix2f, r, _r);
      loc_mul(r->floats, floats, o->floats);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::mul_RARG: Object not of type Matrix2f!\n");
   }
}

void _Matrix2f::_mulRev_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      float t[2*2];
      loc_mul(t, o->floats, floats);
      floats[0] = t[0];
      floats[1] = t[1];
      floats[2] = t[2];
      floats[3] = t[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::mulRev_RSELF: Object not of type Matrix2f!\n");
   }
}

void _Matrix2f::_mulRev_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      _Matrix2f *r = YAC_NEW_POOLED(Matrix2f);
      _r->initObject(r, YAC_TRUE);
      loc_mul(r->floats, o->floats, floats);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::mulRev_RVAL: Object not of type Matrix2f!\n");
   }
}

void _Matrix2f::_mulRev_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix2f) &&
      YAC_CHK(_r, clid_Matrix2f)
      )
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      YAC_CAST_ARG(_Matrix2f, r, _r);
      loc_mul(r->floats, o->floats, floats);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::mulRev_RARG: Object not of type Matrix2f!\n");
   }
}

void _Matrix2f::_mulv_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if(YAC_CHK(_v, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, v, _v);
      _Vector2f *r = YAC_NEW_POOLED(Vector2f);
      _r->initObject(r, YAC_TRUE);
      r->floats[0] = floats[0] * v->floats[0] + floats[1] * v->floats[1];
      r->floats[1] = floats[2] * v->floats[0] + floats[3] * v->floats[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::mulv Object not of type Vector2f");
   }
}

void _Matrix2f::_mulv_YAC_RARG(YAC_Object *_v, YAC_Object *_r) const {
   if(YAC_CHK(_v, clid_Vector2f) &&
      YAC_CHK(_r, clid_Vector2f)
      )
   {
      YAC_CAST_ARG(_Vector2f, v, _v);
      YAC_CAST_ARG(_Vector2f, r, _r);
      if((void*)_v != (void*)_r)
      {
         r->floats[0] = floats[0] * v->floats[0] + floats[1] * v->floats[1];
         r->floats[1] = floats[2] * v->floats[0] + floats[3] * v->floats[1];
      }
      else
      {
         sF32 x = floats[0] * v->floats[0] + floats[1] * v->floats[1];
         sF32 y = floats[2] * v->floats[0] + floats[3] * v->floats[1];
         r->floats[0] = x;
         r->floats[1] = y;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::mulv Object not of type Vector2f");
   }
}

sF32 _Matrix2f::_getAbs(void) const {
   //return "length" of matrix
   return sqrtf(floats[0]*floats[0] + floats[1]*floats[1] + floats[2]*floats[2] + floats[3]*floats[3]);
}

sF32 _Matrix2f::_getAbsSqr(void) const {
   //return squared "length" of matrix
   return (floats[0]*floats[0] + floats[1]*floats[1] + floats[2]*floats[2] + floats[3]*floats[3]);
}

void _Matrix2f::_unit_YAC_RSELF(void) {
   //unify matrix to "length" 1
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
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2f::unit_SELF Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix2f::_unit_YAC_RVAL(YAC_Value *_r) {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      _Matrix2f *r = YAC_NEW_POOLED(Matrix2f);
      _r->initObject(r, YAC_TRUE);
      absval = 1.0f / absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
      r->floats[2] = floats[2] * absval;
      r->floats[3] = floats[3] * absval;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2f::unit_VAL Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix2f::_unit_YAC_RARG(YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Matrix2f))
   {
      sF32 absval = _getAbs();
      if(Dfltnonzero(absval))
      {
         YAC_CAST_ARG(_Matrix2f, r, _r);
         absval = 1.0f / absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
         r->floats[2] = floats[2] * absval;
         r->floats[3] = floats[3] * absval;
      }
      else
      {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2f::unit_ARG Zero Matrix cannot be normalized!\n");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::unit_ARG Return object not of type Matrix2f");
   }
}

void _Matrix2f::_unitScale_YAC_RSELF(sF32 s) {
   //scale matrix to "length" s
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = s / absval;
      floats[0] *= absval;
      floats[1] *= absval;
      floats[2] *= absval;
      floats[3] *= absval;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2f::unitScale_SELF Zero Matrix cannot be normalized");
   }
}

void _Matrix2f::_unitScale_YAC_RVAL(sF32 s, YAC_Value *_r) {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      _Matrix2f *r = YAC_NEW_POOLED(Matrix2f);
      _r->initObject(r, YAC_TRUE);
      absval = s / absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
      r->floats[2] = floats[2] * absval;
      r->floats[3] = floats[3] * absval;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2f::unitScale_VAL Zero Matrix cannot be normalized");
   }
}

void _Matrix2f::_unitScale_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Matrix2f))
   {
      sF32 absval = _getAbs();
      if(Dfltnonzero(absval))
      {
         YAC_CAST_ARG(_Matrix2f, r, _r);
         absval = s / absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
         r->floats[2] = floats[2] * absval;
         r->floats[3] = floats[3] * absval;
      }
      else
      {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2f::unitScale_SELF Zero Matrix cannot be normalized");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::unitScale_SELF Return object not of type Matrix2f");
   }
}

sF32 _Matrix2f::_det(void) const {
   //return determinant of matrix
   return floats[0]*floats[3] - floats[1]*floats[2];
}

void _Matrix2f::_invert_YAC_RSELF(void) {
   sF32 detval = _det();
   if(Dfltnonzero(detval))
   {
      sF32 a = floats[3] / detval;
      sF32 b =-floats[1] / detval;
      sF32 c =-floats[2] / detval;
      sF32 d = floats[0] / detval;
      floats[0] = a;
      floats[1] = b;
      floats[2] = c;
      floats[3] = d;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2f::invert_SELF Invalid Matrix (det==0)");
   }
}

void _Matrix2f::_invert_YAC_RVAL(YAC_Value *_r) {
   sF32 detval = _det();
   if(Dfltnonzero(detval))
   {
      _Matrix2f *r = YAC_NEW_POOLED(Matrix2f);
      _r->initObject(r, YAC_TRUE);
      r->floats[0] =  floats[3] / detval;
      r->floats[1] = -floats[1] / detval;
      r->floats[2] = -floats[2] / detval;
      r->floats[3] =  floats[0] / detval;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2f::invert_VAL Invalid Matrix (det==0)");
   }
}

void _Matrix2f::_invert_YAC_RARG(YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Matrix2f))
   {
      sF32 detval = _det();
      if(Dfltnonzero(detval))
      {
         YAC_CAST_ARG(_Matrix2f, r, _r);
         r->floats[0] =  floats[3] / detval;
         r->floats[1] = -floats[1] / detval;
         r->floats[2] = -floats[2] / detval;
         r->floats[3] =  floats[0] / detval;
      }
      else
      {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2f::invert_ARG Invalid Matrix (det==0)");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::invert_ARG Return object not of type Matrix2f");
   }
}

void _Matrix2f::_transpose_YAC_RSELF(void) {
   //transpose/mirrow matrix
   sF32 a = floats[2];
   floats[2] = floats[1];
   floats[1] = a;
}

void _Matrix2f::_transpose_YAC_RVAL(YAC_Value *_r) {
   _Matrix2f *r = YAC_NEW_POOLED(Matrix2f);
   _r->initObject(r, YAC_TRUE);
   r->floats[0] = floats[0]; r->floats[1] = floats[2];
   r->floats[2] = floats[1]; r->floats[3] = floats[3];
}

void _Matrix2f::_transpose_YAC_RARG(YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, r, _r);
      r->floats[0] = floats[0]; r->floats[1] = floats[2];
      r->floats[2] = floats[1]; r->floats[3] = floats[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::transpose_ARG Return object not of type Matrix2f");
   }
}

void _Matrix2f::_initIdentity(void) {
   TM2F(0,0) = 1.0f;  TM2F(0,1) = 0.0f;
   TM2F(1,0) = 0.0f;  TM2F(1,1) = 1.0f;
}

sBool _Matrix2f::_isIdentity(void) {
   sBool bIdentity = YAC_TRUE;
   for(sUI rowIdx = 0u; rowIdx < 2u; rowIdx++)
   {
      for(sUI colIdx = 0u; colIdx < 2u; colIdx++)
      {
         bIdentity = bIdentity && Dfltequal(TM2F(rowIdx,colIdx), (colIdx == rowIdx) ? 1.0f : 0.0f);
      }
   }
   return bIdentity;
}

void _Matrix2f::_init(sF32 _a, sF32 _b, sF32 _c, sF32 _d) {
   floats[ 0] = _a;
   floats[ 1] = _b;

   floats[ 2] = _c;
   floats[ 3] = _d;
}

void _Matrix2f::_initTranspose(sF32 _a, sF32 _b, sF32 _c, sF32 _d) {
   floats[ 0] = _a;
   floats[ 1] = _c;

   floats[ 2] = _b;
   floats[ 3] = _d;
}
   
void _Matrix2f::_initRowMajor(sF32 a, sF32 b,
                              sF32 c, sF32 d
                              ) {
   // a b
   // c d
   if(b_column_major)
   {
      _initTranspose(a, b, c, d);
   }
   else
   {
      _init(a, b, c, d);
   }
}

void _Matrix2f::_initColumnMajor(sF32 a, sF32 b, sF32 c, sF32 d) {
   // a c
   // b d
   if(b_column_major)
   {
      _init(a, b, c, d);
   }
   else
   {
      _initTranspose(a, b, c, d);
   }
}

void _Matrix2f::_initRowf(sUI _row, sF32 _x, sF32 _y) {
   if(_row < 2u)
   {
      TM2F(_row,0) = _x;
      TM2F(_row,1) = _y;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2f::initRowf: invalid row index (%u > 1)", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2f::_initRowv(sUI _row, YAC_Object *_o) {
   if(_row < 2u)
   {
      if(YAC_CHK(_o, clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, o, _o);
         TM2F(_row, 0) = o->floats[0];
         TM2F(_row, 1) = o->floats[1];
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector2f, t);
         t.assignGeneric(NULL, _o);
         TM2F(_row, 0) = t.floats[0];
         TM2F(_row, 1) = t.floats[1];
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix2f::initRowv: invalid arg object");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2f::initRowv: invalid row index (%u > 1)\n", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2f::_setRow(sUI _row, YAC_Object *_o) {
   _initRowv(_row, _o);
}

void _Matrix2f::_getRow_YAC_RVAL(sUI _row, YAC_Value *_r) {
   if(_row < 2u)
   {
      _Vector2f *r = YAC_NEW_POOLED(Vector2f);
      _r->initObject(r, YAC_TRUE);
      r->_init(TM2F(_row,0), TM2F(_row,1));
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2f::getRow_RVAL: invalid row index (%u > 1)\n", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2f::_getRow_YAC_RARG(sUI _row, YAC_Object *_r) const {
   if(_row < 2u)
   {
      if(YAC_CHK(_r,clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, r, _r);
         r->_init(TM2F(_row,0), TM2F(_row,1));
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix2f::getRow_RARG: return object is not a Vector2f");
      }
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix2f::getRow_RARG: invalid row index (%u > 1)\n", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2f::_initColf(sUI _col, sF32 _x, sF32 _y) {
   if(_col < 2u)
   {
      TM2F(0, _col) = _x;
      TM2F(1, _col) = _y;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2f::initColf: invalid column index (%u > 1)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2f::_initColv(sUI _col, YAC_Object *_o) {
   if(_col < 2u)
   {
      if(YAC_CHK(_o, clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, o, _o);
         TM2F(0, _col) = o->floats[0];
         TM2F(1, _col) = o->floats[1];
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector2f, t);
         t.assignGeneric(NULL, _o);
         TM2F(0, _col) = t.floats[0];
         TM2F(1, _col) = t.floats[1];
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix2f::initColv: invalid arg object");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2f::initColv: invalid column index (%u > 1)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2f::_setCol(sUI _col, YAC_Object *_o) {
   _initColv(_col, _o);
}

void _Matrix2f::_getCol_YAC_RVAL(sUI _col, YAC_Value *_r) {
   if(_col < 2u)
   {
      _Vector2f *r = YAC_NEW_POOLED(Vector2f);
      _r->initObject(r, YAC_TRUE);
      r->_init(TM2F(0, _col),
               TM2F(1, _col)
               );
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2f::getCol_RVAL: invalid column index (%u > 1)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2f::_getCol_YAC_RARG(sUI _col, YAC_Object *_r) const {
   if(_col < 2u)
   {
      if(YAC_CHK(_r,clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, r, _r);
         r->_init(TM2F(0, _col),
                  TM2F(1, _col)
                  );
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix2f::getCol_RARG: return object is not a Vector2f");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2f::getCol_RARG: invalid column index (%u > 1)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2f::_initScalef(sF32 _x, sF32 _y) {
   _init(_x,   0.0f,
         0.0f, _y
         );
}

void _Matrix2f::_initScalev(YAC_Object *_scale) {
   if(YAC_CHK(_scale, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, scale, _scale);
      _init(scale->floats[0], 0.0f,
            0.0f,             scale->floats[1]
            );
   }
   else if(YAC_VALID(_scale))
   {
      YAC_NEW_STACK(Vector2f, v);
      v.assignGeneric(NULL/*context*/, _scale);
      _init(v.floats[0], 0.0f,
            0.0f,        v.floats[1]
            );
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::initScalev: invalid arg object");
   }
}

void _Matrix2f::_initSkewXf(sF32 _a) {
   _initRowMajor(1.0f, tanf(_a),
                 0.0f, 1.0f
                 );
}

void _Matrix2f::_initSkewYf(sF32 _a) {
   _initRowMajor(1.0f,     0.0f,
                 tanf(_a), 1.0f
                 );
}

void _Matrix2f::_initRotatef(sF32 _a) {
   sF32 s = sinf(_a);
   sF32 c = cosf(_a);
   _initRowMajor(c,  -s,
                 s,   c
                 );
}

void _Matrix2f::_scalef_YAC_RSELF(sF32 _x, sF32 _y) {
   YAC_NEW_STACK(Matrix2f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initScalef(_x, _y);
   _mul_YAC_RSELF(&t);
}

void _Matrix2f::_scalef_YAC_RVAL(sF32 _x, sF32 _y, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix2f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initScalef(_x, _y);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix2f::_scalef_YAC_RARG(sF32 _x, sF32 _y, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix2f))
   {
      YAC_NEW_STACK(Matrix2f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(_x, _y);
      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::scalef_RARG: Object not of type Matrix2f");
   }
}

void _Matrix2f::_scalev_YAC_RSELF(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, v, _v);
      YAC_NEW_STACK(Matrix2f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(v->floats[0], v->floats[1]);
      _mul_YAC_RSELF(&t);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix2f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector2f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initScalef(v.floats[0], v.floats[1]);
      _mul_YAC_RSELF(&t);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::scalev_RARG: 'v' is not a Vector2f");
   }
}

void _Matrix2f::_scalev_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if(YAC_CHK(_v, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, v, _v);
      YAC_NEW_STACK(Matrix2f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(v->floats[0], v->floats[1]);
      _mul_YAC_RVAL(&t, _r);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix2f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector2f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initScalef(v.floats[0], v.floats[1]);
      _mul_YAC_RVAL(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::scalev_RVAL: 'v' is not a Vector2f");
   }
}

void _Matrix2f::_scalev_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix2f))
   {
      if(YAC_CHK(_v, clid_Vector2f))
      {
         YAC_CAST_ARG(_Vector2f, v, _v);
         YAC_NEW_STACK(Matrix2f, t);
         t._setColumnMajor(_getColumnMajor());
         t._initScalef(v->floats[0], v->floats[1]);
         _mul_YAC_RARG(&t, _r);
      }
      else if(YAC_VALID(_v))
      {
         YAC_NEW_STACK(Matrix2f, t);
         t._setColumnMajor(_getColumnMajor());
         YAC_NEW_STACK(Vector2f, v);
         v.assignGeneric(NULL/*context*/, _v);
         t._initScalef(v.floats[0], v.floats[1]);
         _mul_YAC_RARG(&t, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::translatev_RARG: 'v' is not a Vector2f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::translatev_RARG: return object is not a Matrix2f");
   }
}

void _Matrix2f::_rotatef_YAC_RSELF(sF32 _ang) {
   YAC_NEW_STACK(Matrix2f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initRotatef(_ang);
   _mul_YAC_RSELF(&t);
}

void _Matrix2f::_rotatef_YAC_RVAL(sF32 _ang, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix2f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initRotatef(_ang);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix2f::_rotatef_YAC_RARG(sF32 _ang, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix2f))
   {
      YAC_NEW_STACK(Matrix2f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initRotatef(_ang);
      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2f::rotatef_RARG: Object not of type Matrix2f");
   }
}

void _Matrix2f::_New(sF32 _a, sF32 _b, sF32 _c, sF32 _d, YAC_Value *_r) {
   _Matrix2f *r = YAC_NEW_POOLED(Matrix2f);
   _r->initObject(r, YAC_TRUE);
   r->_init(_a, _b, _c, _d);
}

void YAC_CALL _matrix2f(sF32 _a, sF32 _b, sF32 _c, sF32 _d, YAC_Value *_r) {
   _Matrix2f *r = YAC_NEW_POOLED(Matrix2f);
   _r->initObject(r, YAC_TRUE);
   r->_init(_a, _b, _c, _d);
}
