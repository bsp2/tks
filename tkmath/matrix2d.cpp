/// matrix2d.cpp
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <math.h>

#include "tkmath_yac.h"
#include "tkmath.h"
#include "vector2d.h"
#include "matrix2d.h"

#include "ying_Math.h"

#define TM2D(r,c)  doubles[TM2(r,c)]
#define OM2D(r,c)  o->doubles[OM2(r,c)]
#define M2D(a,r,c) (a)->doubles[M2(a,r,c)]

_Matrix2d::_Matrix2d(void) {
   _initIdentity();
}

_Matrix2d::~_Matrix2d() {
}

sUI YAC_VCALL _Matrix2d::yacArrayGetNumElements(void) {
   return 4u;
}

sUI YAC_VCALL _Matrix2d::yacArrayGetMaxElements(void) {
   return 4u;
}

sUI YAC_VCALL _Matrix2d::yacArrayGetElementType(void) {
   return YAC_TYPE_OBJECT;
}

sUI YAC_VCALL _Matrix2d::yacArrayGetElementByteSize(void) {
   return sizeof(sF64);
}

void YAC_VCALL _Matrix2d::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4u)
   {
      sF64 v = 0;
      switch(_val->type)
      {
         case YAC_TYPE_VOID:
            break;

         case YAC_TYPE_INT:
            v = (sF64) _val->value.int_val;
            break;

         case YAC_TYPE_FLOAT:
            v = (sF64) _val->value.float_val;
            break;

         case YAC_TYPE_OBJECT:
         case YAC_TYPE_STRING:
            if(YAC_VALID(_val->value.object_val))
            {
               if(!_val->value.object_val->yacScanF64(&v))
                  v = 0.0;
            }
            break;

         default:
            Dyac_throw(TypeMismatch, "tkmath::Matrix2d::yacArraySet: invalid type");
            break;
      }
      doubles[_idx]=v;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2d::yacArraySet: index out of bounds (%u > 3)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Matrix2d::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4u)
   {
      YAC_Double *r = (YAC_Double*) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
      r->value = doubles[_idx];
      _val->initObject(r,1);
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix2d::yacArrayGet: index out of bounds (%u > 3)", _idx);
      Dyac_throw(ReadArrayOutOfBounds, buf);
   }
}

void *YAC_VCALL _Matrix2d::yacArrayGetPointer(void) {
   return (void*)doubles;
}

sUI YAC_VCALL _Matrix2d::yacArrayGetWidth(void) {
   return 2u;
}

sUI YAC_VCALL _Matrix2d::yacArrayGetHeight(void) {
   return 2u;
}

sUI YAC_VCALL _Matrix2d::yacArrayGetStride(void) {
   return 2u * sizeof(sF64);
}

sSI YAC_VCALL _Matrix2d::yacTensorRank(void) {
   return YAC_TENSOR_RANK_MATRIX;
}

void YAC_VCALL _Matrix2d::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F64(doubles[0]);
   YAC_SERIALIZE_F64(doubles[1]);
   YAC_SERIALIZE_F64(doubles[2]);
   YAC_SERIALIZE_F64(doubles[3]);
}

sUI YAC_VCALL _Matrix2d::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   doubles[0] = YAC_DESERIALIZE_F64();
   doubles[1] = YAC_DESERIALIZE_F64();
   doubles[2] = YAC_DESERIALIZE_F64();
   doubles[3] = YAC_DESERIALIZE_F64();
   return 1u;
}

void YAC_VCALL _Matrix2d::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Matrix2d) &&
      YAC_CHK(_max, clid_Matrix2d)
      )
   {
      YAC_CAST_ARG(_Matrix2d, min, _min);
      YAC_CAST_ARG(_Matrix2d, max, _max);

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
      Dyac_throw_def(InvalidPointer, "tkmath::Matrix2d::yacOperatorClamp: invalid object(s)");
   }
}

void YAC_VCALL _Matrix2d::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Matrix2d) &&
      YAC_CHK(_max, clid_Matrix2d)
      )
   {
      YAC_CAST_ARG(_Matrix2d, min, _min);
      YAC_CAST_ARG(_Matrix2d, max, _max);

      for(sUI i = 0u; i < 4u; i++)
      {
         if(doubles[i] < min->doubles[i])
            doubles[i] += max->doubles[i] - min->doubles[i];
         else if(doubles[i] > max->doubles[i])
            doubles[i] -= max->doubles[i] - min->doubles[i];
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Matrix2d::yacOperatorWrap: invalid object(s)");
   }
}

void YAC_VCALL _Matrix2d::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o,clid_Matrix2d))
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);

      switch(_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI( Ddblequal(doubles[0], o->doubles[0]) &&
                      Ddblequal(doubles[1], o->doubles[1]) &&
                      Ddblequal(doubles[2], o->doubles[2]) &&
                      Ddblequal(doubles[3], o->doubles[3]) );
            break;

         case YAC_OP_CNE:
            YAC_RETI( Ddblnotequal(doubles[0], o->doubles[0]) ||
                      Ddblnotequal(doubles[1], o->doubles[1]) ||
                      Ddblnotequal(doubles[2], o->doubles[2]) ||
                      Ddblnotequal(doubles[3], o->doubles[3]) );
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

         case YAC_OP_MUL:
            _mul_YAC_RSELF(_o);
            break;

         case YAC_OP_NEG:
            doubles[0] = -doubles[0];
            doubles[1] = -doubles[1];
            doubles[2] = -doubles[2];
            doubles[3] = -doubles[3];
            break;

         case YAC_OP_INIT:
            break;

         default:
         {
            char buf[80];
            Dyac_snprintf(buf, 80, "tkmath::Matrix2d::yacOperator: unsupported cmd %d!\n", _cmd);
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
                  doubles[i] = (sF64)o->elements[i];
               for(; i < 4u; i++)
                  doubles[i]=0.0;
            }
            else
            {
               doubles[0] = (sF64)o->elements[0];
               doubles[1] = (sF64)o->elements[1];
               doubles[2] = (sF64)o->elements[2];
               doubles[3] = (sF64)o->elements[3];
            }
            break;

         default:
            _Matrix2d t; t.class_ID=clid_Matrix2d;
            t.assignFloatArray(o);
            yacOperator(_cmd, &t, _r);
            //Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix2d::yacOperator Only Assign supported with FloatArray Type");
            break;
      }
   }
   else if(YAC_VALID(_o))
   {
      sF64 v;
      if(_o->yacScanF64(&v))
      {
         yacOperatorF64(_cmd, v, _r);
      }
      else
      {
         switch(_cmd)
         {
            case YAC_OP_ASSIGN:
               assignGeneric(NULL,_o);
               break;

            default:
               Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix2d::yacOperator Only Assign supported with Generic Object Type");
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
            YAC_Object::yacOperator(_cmd,_o,_r);
            break;
      }
   }
}

void _Matrix2d::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void _Matrix2d::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void _Matrix2d::yacOperatorF64(sSI _cmd, sF64 val, YAC_Value *_r) {
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
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2d::yacOperatorF64<OP_DIV>: cannot divide by zero");
         }
         break;

      default:
         char buf[80];
         Dyac_snprintf(buf, 80, "tkmath::Matrix2d::yacOperatorF64: unsupported operator %d",_cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
         break;
   }
}


void _Matrix2d::assignFloats(sF32 *_fv, sUI _ne) {
   if(4u == _ne)
   {
      doubles[0] = (sF64) _fv[0];
      doubles[1] = (sF64) _fv[1];
      doubles[2] = (sF64) _fv[2];
      doubles[3] = (sF64) _fv[3];
   }
   else
   {
      Dyac_throw_def(WriteArrayOutOfBounds,"tkmath::Matrix2d Array not of size 4");
   }
}

void _Matrix2d::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Matrix2d::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
   sUI ne = _o->yacArrayGetNumElements();
   sUI nH = _o->yacArrayGetHeight();
   sUI nW = _o->yacArrayGetWidth();
   if(0u == nH)
   {
      if(4u == nW)
      {
         nH = 2u;
         nW = 2u;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix2d::assignGeneric: array not of size 4");
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
            sUI i, j;
            for(i = 0u; i < 2u && i < nH; i++)
            {
               for(j = 0u; j < 2u && j < nW; j++)
               {
                  doubles[(i*2)+j] = (sF64) fv[(i*nW)+j];
               }
               for (;j < 2u; j++)
               {
                  if(i != j)
                     doubles[(i*2)+j] = 0.0;
                  else
                     doubles[(i*2)+j] = 1.0;
               }
            }
            for(;i < 2u; i++)
            {
               for(j = 0u; j < 2u; j++)
               {
                  if(i != j)
                     doubles[(i*2)+j] = 0.0;
                  else
                     doubles[(i*2)+j] = 1.0;
               }
            }
            return;
         }
         else
         {
            // Retrieve individual float elements and assign
            YAC_Value v;
            sUI i,j;
            for(i = 0u; i < 2u && i < nH; i++)
            {
               for(j = 0u; j < 2u && j < nW; j++)
               {
                  _o->yacArrayGet(_ctx, (i*nW)+j, &v);
                  doubles[(i*2)+j] = (sF64) v.value.float_val;
               }
               for(; j < 2u; j++)
               {
                  if(i != j)
                     doubles[(i*2)+j] = 0.0;
                  else
                     doubles[(i*2)+j] = 1.0;
               }
            }
            for(; i < 2u; i++)
            {
               for(j = 0u; j < 2u; j++)
               {
                  if(i != j)
                     doubles[(i*2)+j] = 0.0;
                  else
                     doubles[(i*2)+j] = 1.0;
               }
            }
         }
      }
      else
      {
         // Retrieve individual elements, typecast to float and assign
         YAC_Value v;
         sUI i,j;
         for(i = 0u; i < 2u && i < nH; i++)
         {
            for(j = 0u; j < 2u && j < nW; j++)
            {
               _o->yacArrayGet(_ctx, (i*nW)+j, &v);
               v.typecast(YAC_TYPE_OBJECT);
               if(!v.value.object_val->yacScanF64(&doubles[(i*2)+j]))
                  doubles[(i*2)+j] = 0.0;
               v.unset();
            }
            for(; j < 2u; j++)
            {
               if(i != j)
                  doubles[(i*2)+j] = 0.0;
               else
                  doubles[(i*2)+j] = 1.0;
            }
         }
         for(; i < 2u; i++)
         {
            for(j = 0u; j < 2u; j++)
            {
               if(i != j)
                  doubles[(i*2)+j] = 0.0;
               else
                  doubles[(i*2)+j] = 1.0;
            }
         }
      }
   }
   else
   {
      // Other array is empty or no array. Quitely assign 0.0f.
      doubles[0] = doubles[1] = doubles[2] = doubles[3] = 0.0;
   }
}

void _Matrix2d::_setA(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[0])) doubles[0] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::setA: invalid object");
   }

}

void _Matrix2d::_setB(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[1])) doubles[1] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::setB: invalid object");
   }
}

void _Matrix2d::_setC(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[2])) doubles[2] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::setC: invalid object");
   }
}

void _Matrix2d::_setD(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[3])) doubles[3] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::setD: invalid object");
   }
}

void _Matrix2d::_getA_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double *) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   r->value = doubles[0];
   _r->initObject(r, YAC_TRUE);
}

void _Matrix2d::_getA_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[0]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::getA: invalid return object");
   }
}

void _Matrix2d::_getB_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double *) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   r->value = doubles[1];
   _r->initObject(r, YAC_TRUE);
}

void _Matrix2d::_getB_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[1]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::getB: invalid return object");
   }
}
void _Matrix2d::_getC_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double *) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(r, YAC_TRUE);
   r->value = doubles[2];
}

void _Matrix2d::_getC_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[2]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::getC: invalid return object");
   }
}
void _Matrix2d::_getD_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double *) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(r, YAC_TRUE);
   r->value = doubles[3];
}

void _Matrix2d::_getD_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[3]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::getD: invalid return object");
   }
}

//generate string representation
sBool YAC_VCALL _Matrix2d::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(96,0,0,0);
   s->printf("((%lg, %lg), (%lg, %lg))", doubles[0], doubles[1], doubles[2], doubles[3]);
   s->fixLength();
   return YAC_TRUE;
}

void _Matrix2d::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   yacToString(s);
   YAC_RETS(s, YAC_TRUE);
}

void _Matrix2d::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o,clid_Matrix2d))
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);
      doubles[0] += o->doubles[0];
      doubles[1] += o->doubles[1];
      doubles[2] += o->doubles[2];
      doubles[3] += o->doubles[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::add_RSELF: object not of type Matrix2d");
   }
}

void _Matrix2d::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o,clid_Matrix2d))
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);
      _Matrix2d *r = YAC_NEW_POOLED(Matrix2d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[0] + o->doubles[0];
      r->doubles[1] = doubles[1] + o->doubles[1];
      r->doubles[2] = doubles[2] + o->doubles[2];
      r->doubles[3] = doubles[3] + o->doubles[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::add_RVAL: object not of type Matrix2d");
   }
}

void _Matrix2d::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Matrix2d) &&
      YAC_CHK(_r, clid_Matrix2d)
      )
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);
      YAC_CAST_ARG(_Matrix2d, r, _r);
      r->doubles[0] = doubles[0] + o->doubles[0];
      r->doubles[1] = doubles[1] + o->doubles[1];
      r->doubles[2] = doubles[2] + o->doubles[2];
      r->doubles[3] = doubles[3] + o->doubles[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::add_RARG: object not of type Matrix2d");
   }
}

void _Matrix2d::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix2d))
   {
      _Matrix2d *o = (_Matrix2d*)_o;
      doubles[0] -= o->doubles[0];
      doubles[1] -= o->doubles[1];
      doubles[2] -= o->doubles[2];
      doubles[3] -= o->doubles[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::sub_RSELF: object not of type Matrix2d");
   }
}

void _Matrix2d::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2d))
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);
      _Matrix2d *r = YAC_NEW_POOLED(Matrix2d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[0] - o->doubles[0];
      r->doubles[1] = doubles[1] - o->doubles[1];
      r->doubles[2] = doubles[2] - o->doubles[2];
      r->doubles[3] = doubles[3] - o->doubles[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::sub_RVAL: object not of type Matrix2d");
   }
}

void _Matrix2d::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Matrix2d) &&
      YAC_CHK(_r, clid_Matrix2d)
      )
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);
      YAC_CAST_ARG(_Matrix2d, r, _r);
      r->doubles[0] = doubles[0] - o->doubles[0];
      r->doubles[1] = doubles[1] - o->doubles[1];
      r->doubles[2] = doubles[2] - o->doubles[2];
      r->doubles[3] = doubles[3] - o->doubles[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::sub_RARG: object not of type Matrix2d");
   }
}

void _Matrix2d::muld(const sF64 s) {
   doubles[0] *= s;
   doubles[1] *= s;
   doubles[2] *= s;
   doubles[3] *= s;
}

void _Matrix2d::_muld_YAC_RSELF(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      sF64 s;
      if(!_s->yacScanF64(&s)) s = 0.0;
      muld(s);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::muld_RSELF: invalid object");
   }
}

void _Matrix2d::_muld_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   if(YAC_VALID(_s))
   {
      _Matrix2d *r = YAC_NEW_POOLED(Matrix2d);
      _r->initObject(r, YAC_TRUE);
      sF64 s;
      if(!_s->yacScanF64(&s)) s=0.0;
      r->doubles[0] = doubles[0] * s;
      r->doubles[1] = doubles[1] * s;
      r->doubles[2] = doubles[2] * s;
      r->doubles[3] = doubles[3] * s;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::muld_RVAL: invalid object");
   }
}

void _Matrix2d::_muld_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix2d))
   {
      if(YAC_VALID(_s))
      {
         YAC_CAST_ARG(_Matrix2d, r, _r);
         sF64 s;
         if(!_s->yacScanF64(&s)) s = 0.0;
         r->doubles[0] = doubles[0] * s;
         r->doubles[1] = doubles[1] * s;
         r->doubles[2] = doubles[2] * s;
         r->doubles[3] = doubles[3] * s;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::muld_RARG: invalid object");
      }
   }
   else
   {
     Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::mulf_RARG: return object not of type Matrix2d");
   }
}

void _Matrix2d::_mul_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix2d))
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);
      const sF64 a = doubles[0]*o->doubles[0] + doubles[1]*o->doubles[2];
      const sF64 b = doubles[0]*o->doubles[1] + doubles[1]*o->doubles[3];
      const sF64 c = doubles[2]*o->doubles[0] + doubles[3]*o->doubles[2];
      const sF64 d = doubles[2]*o->doubles[1] + doubles[3]*o->doubles[3];
      doubles[0] = a;
      doubles[1] = b;
      doubles[2] = c;
      doubles[3] = d;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::mul_RSELF: object not of type Matrix2d!\n");
   }
}

void _Matrix2d::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2d))
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);
      _Matrix2d *r = YAC_NEW_POOLED(Matrix2d);
      _r->initObject(r, YAC_TRUE);

      r->doubles[0] = doubles[0]*o->doubles[0] + doubles[1]*o->doubles[2];
      r->doubles[1] = doubles[0]*o->doubles[1] + doubles[1]*o->doubles[3];
      r->doubles[2] = doubles[2]*o->doubles[0] + doubles[3]*o->doubles[2];
      r->doubles[3] = doubles[2]*o->doubles[1] + doubles[3]*o->doubles[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::mul_RVAL: object not of type Matrix2d!\n");
   }
}

void _Matrix2d::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Matrix2d) &&
      YAC_CHK(_r, clid_Matrix2d)
      )
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);
      YAC_CAST_ARG(_Matrix2d, r, _r);

      if((void*)r != (void*)this)
      {
         r->doubles[0] = doubles[0]*o->doubles[0] + doubles[1]*o->doubles[2];
         r->doubles[1] = doubles[0]*o->doubles[1] + doubles[1]*o->doubles[3];
         r->doubles[2] = doubles[2]*o->doubles[0] + doubles[3]*o->doubles[2];
         r->doubles[3] = doubles[2]*o->doubles[1] + doubles[3]*o->doubles[3];
      }
      else
      {
         const sF64 a = doubles[0]*o->doubles[0] + doubles[1]*o->doubles[2];
         const sF64 b = doubles[0]*o->doubles[1] + doubles[1]*o->doubles[3];
         const sF64 c = doubles[2]*o->doubles[0] + doubles[3]*o->doubles[2];
         const sF64 d = doubles[2]*o->doubles[1] + doubles[3]*o->doubles[3];

         r->doubles[0] = a;
         r->doubles[1] = b;
         r->doubles[2] = c;
         r->doubles[3] = d;
      }

   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::mul_RARG: object not of type Matrix2d!\n");
   }
}

void _Matrix2d::_mulRev_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix2d))
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);
      const sF64 a = o->doubles[0]*doubles[0] + o->doubles[1]*doubles[2];
      const sF64 b = o->doubles[0]*doubles[1] + o->doubles[1]*doubles[3];
      const sF64 c = o->doubles[2]*doubles[0] + o->doubles[3]*doubles[2];
      const sF64 d = o->doubles[2]*doubles[1] + o->doubles[3]*doubles[3];
      doubles[0] = a;
      doubles[1] = b;
      doubles[2] = c;
      doubles[3] = d;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::mulRev_RSELF: object not of type Matrix2d!\n");
   }
}

void _Matrix2d::_mulRev_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix2d))
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);
      _Matrix2d *r = YAC_NEW_POOLED(Matrix2d);
      _r->initObject(r, YAC_TRUE);

      r->doubles[0] = o->doubles[0]*doubles[0] + o->doubles[1]*doubles[2];
      r->doubles[1] = o->doubles[0]*doubles[1] + o->doubles[1]*doubles[3];
      r->doubles[2] = o->doubles[2]*doubles[0] + o->doubles[3]*doubles[2];
      r->doubles[3] = o->doubles[2]*doubles[1] + o->doubles[3]*doubles[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::mulRev_RVAL: object not of type Matrix2d!\n");
   }
}

void _Matrix2d::_mulRev_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Matrix2d) &&
      YAC_CHK(_r, clid_Matrix2d)
      )
   {
      YAC_CAST_ARG(_Matrix2d, o, _o);
      YAC_CAST_ARG(_Matrix2d, r, _r);

      if((void*)r != (void*)this)
      {
         r->doubles[0] = o->doubles[0]*doubles[0] + o->doubles[1]*doubles[2];
         r->doubles[1] = o->doubles[0]*doubles[1] + o->doubles[1]*doubles[3];
         r->doubles[2] = o->doubles[2]*doubles[0] + o->doubles[3]*doubles[2];
         r->doubles[3] = o->doubles[2]*doubles[1] + o->doubles[3]*doubles[3];
      }
      else
      {
         const sF64 a = o->doubles[0]*doubles[0] + o->doubles[1]*doubles[2];
         const sF64 b = o->doubles[0]*doubles[1] + o->doubles[1]*doubles[3];
         const sF64 c = o->doubles[2]*doubles[0] + o->doubles[3]*doubles[2];
         const sF64 d = o->doubles[2]*doubles[1] + o->doubles[3]*doubles[3];

         r->doubles[0] = a;
         r->doubles[1] = b;
         r->doubles[2] = c;
         r->doubles[3] = d;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::mulRev_RARG: object not of type Matrix2d!\n");
   }
}

void _Matrix2d::_mulv_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      _Vector2d *r = YAC_NEW_POOLED(Vector2d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[0]*o->doubles[0] + doubles[1]*o->doubles[1];
      r->doubles[1] = doubles[2]*o->doubles[0] + doubles[3]*o->doubles[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::mulv Object not of type Vector2d");
   }
}

void _Matrix2d::_mulv_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Vector2d) &&
      YAC_CHK(_r, clid_Vector2d)
      )
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      YAC_CAST_ARG(_Vector2d, r, _r);

      if((void*)o != (void*)r)
      {
         r->doubles[0] = doubles[0]*o->doubles[0] + doubles[1]*o->doubles[1];
         r->doubles[1] = doubles[2]*o->doubles[0] + doubles[3]*o->doubles[1];
      }
      else
      {
         sF64 x = doubles[0]*o->doubles[0] + doubles[1]*o->doubles[1];
         sF64 y = doubles[2]*o->doubles[0] + doubles[3]*o->doubles[1];
         r->doubles[0] = x;
         r->doubles[1] = y;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::mulv Object not of type Vector2d");
   }
}

sF64 _Matrix2d::getAbs(void) {
   // return "length" of matrix
   return sqrt(doubles[0]*doubles[0] + doubles[1]*doubles[1] + doubles[2]*doubles[2] + doubles[3]*doubles[3]);
}

void _Matrix2d::_getAbs_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double *) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   r->value=getAbs();
   _r->initObject(r,1);
}

void _Matrix2d::_getAbs_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getAbs());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::getAbs Return object not valid");
   }
}

//return squared "length" of matrix
sF64 _Matrix2d::getAbsSqr(void) {
   return (doubles[0]*doubles[0]+doubles[1]*doubles[1]+doubles[2]*doubles[2]+doubles[3]*doubles[3]);
}

void _Matrix2d::_getAbsSqr_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double *) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   r->value=getAbsSqr();
   _r->initObject(r,1);
}

void _Matrix2d::_getAbsSqr_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getAbsSqr());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::getAbs Return object not valid");
   }
}

//unify matrix to "length" 1
void _Matrix2d::_unit_YAC_RSELF(void) {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      absval=1.0/absval;
      doubles[0]*=absval;
      doubles[1]*=absval;
      doubles[2]*=absval;
      doubles[3]*=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2d::unit_SELF Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix2d::_unit_YAC_RVAL(YAC_Value *_r) {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      _Matrix2d *r=YAC_NEW_POOLED(Matrix2d);
      absval=1.0/absval;
      r->doubles[0] = doubles[0] * absval;
      r->doubles[1] = doubles[1] * absval;
      r->doubles[2] = doubles[2] * absval;
      r->doubles[3] = doubles[3] * absval;
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2d::unit_VAL Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix2d::_unit_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r, clid_Matrix2d)) {
      sF64 absval=getAbs();
      if (Ddblnonzero(absval)) {
         _Matrix2d *r=(_Matrix2d*)_r;
	 absval=1.0/absval;
         r->doubles[0] = doubles[0] * absval;
         r->doubles[1] = doubles[1] * absval;
         r->doubles[2] = doubles[2] * absval;
         r->doubles[3] = doubles[3] * absval;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2d::unit_ARG Zero Matrix cannot be normalized!\n");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::unit_ARG Return object not of type Matrix2d");
   }
}

//scale matrix to "length" s
void _Matrix2d::unitScale(sF64 s) {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      absval=s/absval;
      doubles[0]*=absval;
      doubles[1]*=absval;
      doubles[2]*=absval;
      doubles[3]*=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2d::unitScale Zero Matrix cannot be normalized");
   }
}

void _Matrix2d::_unitScale_YAC_RSELF(YAC_Object *_s) {
   if (YAC_VALID(_s)) {
      sF64 s;
      if (!_s->yacScanF64(&s)) s=0.0;
      unitScale(s);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::unitScale_SELF Object not valid");
   }
}

void _Matrix2d::_unitScale_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   if (YAC_VALID(_s)) {
      sF64 absval=getAbs();
      if (Ddblnonzero(absval)) {
         _Matrix2d *r=YAC_NEW_POOLED(Matrix2d);
	 sF64 s;
	 if (!_s->yacScanF64(&s)) s=0.0;
         absval=s/absval;
         r->doubles[0] = doubles[0] * absval;
         r->doubles[1] = doubles[1] * absval;
         r->doubles[2] = doubles[2] * absval;
         r->doubles[3] = doubles[3] * absval;
         _r->initObject(r,1);
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2d::unitScale_VAL Zero Matrix cannot be normalized");
      }
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::unitScale_VAL Object not valid");
   }
}

void _Matrix2d::_unitScale_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if (YAC_VALID(_s)) {
      if (YAC_CHK(_r, clid_Matrix2d)) {
         sF64 absval=getAbs();
         if (Ddblnonzero(absval)) {
            _Matrix2d *r=(_Matrix2d*)_r;
	    sF64 s;
	    if (!_s->yacScanF64(&s)) s=0.0;
            absval=s/absval;
            r->doubles[0] = doubles[0] * absval;
            r->doubles[1] = doubles[1] * absval;
            r->doubles[2] = doubles[2] * absval;
            r->doubles[3] = doubles[3] * absval;
         } else {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2d::unitScale_SELF Zero Matrix cannot be normalized");
         }
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::unitScale_SELF Return object not of type Matrix2d");
      }
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::unitScale_SELF Object not valid");
   }
}

//return determinant of matrix
sF64 _Matrix2d::det(void) {
   return (doubles[0]*doubles[3]-doubles[1]*doubles[2]);
}

void _Matrix2d::_det_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double *) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   r->value = det();
   _r->initObject(r,1);
}

void _Matrix2d::_det_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(det());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::det_ARG Return object not valid");
   }
}

//invert matrix
void _Matrix2d::_invert_YAC_RSELF(void) {
   sF64 detval=det();
   if (Ddblnonzero(detval)) {
      sF64 a,b,c,d;
      a=doubles[3]/detval;
      b=-doubles[1]/detval;
      c=-doubles[2]/detval;
      d=doubles[0]/detval;
      doubles[0]=a;
      doubles[1]=b;
      doubles[2]=c;
      doubles[3]=d;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2d::invert_SELF Invalid Matrix (det==0)");
   }
}

void _Matrix2d::_invert_YAC_RVAL(YAC_Value *_r) {
   sF64 detval=det();
   if (Ddblnonzero(detval)) {
      _Matrix2d *r=YAC_NEW_POOLED(Matrix2d);
      r->doubles[0] =  doubles[3] / detval;
      r->doubles[1] = -doubles[1] / detval;
      r->doubles[2] = -doubles[2] / detval;
      r->doubles[3] =  doubles[0] / detval;
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2d::invert_VAL Invalid Matrix (det==0)");
   }
}

void _Matrix2d::_invert_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r, clid_Matrix2d)) {
      sF64 detval=det();
      if (Ddblnonzero(detval)) {
         _Matrix2d *r=(_Matrix2d*)_r;
         r->doubles[0] =  doubles[3] / detval;
         r->doubles[1] = -doubles[1] / detval;
         r->doubles[2] = -doubles[2] / detval;
         r->doubles[3] =  doubles[0] / detval;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix2d::invert_ARG Invalid Matrix (det==0)");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::invert_ARG Return object not of type Matrix2d");
   }
}

//transpose/mirrow matrix
void _Matrix2d::_transpose_YAC_RSELF(void) {
   sF64 a;
   a=doubles[2];
   doubles[2]=doubles[1];
   doubles[1]=a;
}

void _Matrix2d::_transpose_YAC_RVAL(YAC_Value *_r) {
   _Matrix2d *r=YAC_NEW_POOLED(Matrix2d);
   r->doubles[0]=doubles[0]; r->doubles[1]=doubles[2];
   r->doubles[2]=doubles[1]; r->doubles[3]=doubles[3];
   _r->initObject(r,1);
}

void _Matrix2d::_transpose_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r, clid_Matrix2d)) {
      _Matrix2d *r=(_Matrix2d*)_r;
      r->doubles[0]=doubles[0]; r->doubles[1]=doubles[2];
      r->doubles[2]=doubles[1]; r->doubles[3]=doubles[3];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix2d::transpose_ARG Return object not of type Matrix2d");
   }
}

void _Matrix2d::_init(YAC_Object *_a, YAC_Object *_b,
                      YAC_Object *_c, YAC_Object *_d
                      ) {
   if(YAC_VALID(_a) && YAC_VALID(_b) && YAC_VALID(_c) && YAC_VALID(_d))
   {
      sF64 a, b, c, d;
      if (!_a->yacScanF64(&a)) a = 0.0;
      if (!_b->yacScanF64(&b)) b = 0.0;
      if (!_c->yacScanF64(&c)) c = 0.0;
      if (!_d->yacScanF64(&d)) d = 0.0;
      TM2D(0,0) = a;  TM2D(0,1) = b;
      TM2D(1,0) = c;  TM2D(1,1) = d; 

   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::init Objects not valid");
   }
}

void _Matrix2d::_initf(sF32 a, sF32 b, sF32 c, sF32 d) {
   TM2D(0,0) = (sF64)a;  TM2D(0,1) = (sF64)b;
   TM2D(1,0) = (sF64)c;  TM2D(1,1) = (sF64)d;
}

void _Matrix2d::_initRow(sUI row, YAC_Object *x, YAC_Object *y) {
   if (row<2) {
      if (YAC_VALID(x)&&YAC_VALID(y)) {
         if (!x->yacScanF64(&doubles[row*2])) doubles[row*2]=0.0;
	 if (!y->yacScanF64(&doubles[row*2+1])) doubles[row*2+1]=0.0;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::initRow Objects not valid");
      }
   } else {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix2d::initRow Wrong Row Number! (%d>1)", row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2d::_initCol(sUI col, YAC_Object *x, YAC_Object *y) {
   if (col<2) {
      if (YAC_VALID(x)&&YAC_VALID(y)) {
         if (!x->yacScanF64(&doubles[col])) doubles[col]=0.0;
	 if (!y->yacScanF64(&doubles[col+2])) doubles[col+2]=0.0;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix2d::initCol Objects not valid");
      }
   } else {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix2d::initCol Wrong Col Number! (%d>1)\n", col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix2d::_setRow(sUI row, YAC_Object *_o) {
   _initRowV(row,_o);
}

void _Matrix2d::_getRow_YAC_RVAL(sUI row, YAC_Value *_r) {
   if(row > 1u)
   {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix2d::getRow_VAL Wrong Row Number! (%d>1)\n", row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
   _Vector2d *r=YAC_NEW_POOLED(Vector2d);
   r->doubles[0]=doubles[(row*2)+0];
   r->doubles[1]=doubles[(row*2)+1];
   _r->initObject(r,1);
}

void _Matrix2d::_getRow_YAC_RARG(sUI row, YAC_Object *_r) {
   if(row > 1u) {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix2d::getRow_ARG Wrong Row Number! (%d>1)\n", row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
   if (!YAC_CHK(_r,clid_Vector2d)) {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix2d::getRow_ARG Return object not of type Vector2d");
   }
   _Vector2d *r=(_Vector2d*)_r;
   r->doubles[0]=doubles[(row*2)+0];
   r->doubles[1]=doubles[(row*2)+1];
}

void _Matrix2d::_setCol(sUI col, YAC_Object *_o) {
   _initColV(col,_o);
}

void _Matrix2d::_getCol_YAC_RVAL(sUI col, YAC_Value *_r) {
   if(col > 1u) {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix2d::getCol_VAL Wrong Row Number! (%d>1)\n", col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
   _Vector2d *r=YAC_NEW_POOLED(Vector2d);
   r->doubles[0]=doubles[0+col];
   r->doubles[1]=doubles[2+col];
   _r->initObject(r,1);
}

void _Matrix2d::_getCol_YAC_RARG(sUI col, YAC_Object *_r) {
   if(col > 1u) {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix2d::getCol_ARG Wrong Row Number! (%d>1)\n", col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
   if (!YAC_CHK(_r,clid_Vector2d)) {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix2d::getCol_ARG Return object not of type Vector2d");
   }
   _Vector2d *r=(_Vector2d*)_r;
   r->doubles[0]=doubles[0+col];
   r->doubles[1]=doubles[2+col];
}

void _Matrix2d::_initRowV(sUI row, YAC_Object *_o) {
   if (YAC_VALID(_o)) {
      _Vector2d t;
      t.class_ID=clid_Vector2d;
      t.assignGeneric(NULL,_o);
      if(row > 1u)
      {
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Matrix2d::initRowV Wrong Row Number! (%d>1)\n", row);
         Dyac_throw_def(WriteArrayOutOfBounds, buf);
      }
      doubles[(row*2)+0]=t.doubles[0];
      doubles[(row*2)+1]=t.doubles[1];
   } else {
      Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix2d::initRowV: Invalid Object Value");
   }

}

void _Matrix2d::_initColV(sUI col, YAC_Object *_o) {
   if (YAC_VALID(_o)) {
      _Vector2d t;
      t.class_ID=clid_Vector2d;
      t.assignGeneric(NULL,_o);
      if(col > 1u)
      {
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Matrix2d::initColV Wrong Col Number! (%d>1)\n", col);
         Dyac_throw_def(WriteArrayOutOfBounds, buf);
      }
      doubles[0+col]=t.doubles[0];
      doubles[2+col]=t.doubles[1];
   } else {
      Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix2d::initColV: Invalid Object Value");
   }
}

void _Matrix2d::_initIdentity(void) {
   TM2D(0,0) = 1.0;  TM2D(0,1) = 0.0;
   TM2D(1,0) = 0.0;  TM2D(1,1) = 1.0;
}

void _Matrix2d::_initScalef(sF32 _x, sF32 _y) {
   _initf(_x,   0.0f,
          0.0f, _y
          );
}

void _Matrix2d::_initRotatef(sF32 _a) {
   sF64 s = sin(_a);
   sF64 c = cos(_a);

   TM2D(0,0) = c;  TM2D(0,1) = -s;
   TM2D(1,0) = s;  TM2D(1,1) =  c;
}

void _Matrix2d::_New(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Object *d, YAC_Value *_r) {
   _Matrix2d *r=YAC_NEW_POOLED(Matrix2d);
   _r->initObject(r, YAC_TRUE);
   r->_init(a, b,
            c, d
            );
}

void YAC_CALL _matrix2d(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Object *d, YAC_Value *_r) {
   _Matrix2d *r=YAC_NEW_POOLED(Matrix2d);
   _r->initObject(r, YAC_TRUE);
   r->_init(a, b,
            c, d
            );
}
