/// matrix3f.cpp
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
#include "matrix3f.h"

#include "ying_Math.h"


#ifdef USE_DYNAMIC_ORDER
const sU8 matrix3_element_order_table[2][3][3] = {
   // Row major
   {
      { 0,  1, 2 },
      { 3,  4, 5 },
      { 6,  7, 8 }
   },

   // Column major
   {
      { 0,  3,  6 },
      { 1,  4,  7 },
      { 2,  5,  8 },
   }
};
#endif // USE_DYNAMIC_ORDER

_Matrix3f::_Matrix3f(void) {
   _initIdentity();
}

_Matrix3f::~_Matrix3f() {
}

sUI YAC_VCALL _Matrix3f::yacArrayGetNumElements(void) {
   return 9;
}

sUI YAC_VCALL _Matrix3f::yacArrayGetMaxElements(void) {
   return 9;
}

sUI YAC_VCALL _Matrix3f::yacArrayGetElementType(void) {
   return YAC_TYPE_FLOAT;
}

sUI YAC_VCALL _Matrix3f::yacArrayGetElementByteSize(void) {
   return sizeof(sF32);
}

void YAC_VCALL _Matrix3f::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 9u)
   {
      sF32 v = 0.0f;
      switch(_val->type)
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
         case YAC_TYPE_STRING:
            if(YAC_VALID(_val->value.object_val))
            {
               if(!_val->value.object_val->yacScanF32(&v))
                  v = 0.0f;
            }
            break;

         default:
            Dyac_throw(TypeMismatch, "tkmath::Matrix3f::yacArraySet: invalid type");
            break;
      }
      floats[_idx]=v;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3f::yacArraySet: index out of bounds (%u > 8)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Matrix3f::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 9u)
   {
      _val->initFloat(floats[_idx]);
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3f::yacArrayGet: index out of bounds (%u > 8)", _idx);
      Dyac_throw(ReadArrayOutOfBounds, buf);
      _val->initFloat(0.0f);
   }
}

void *YAC_VCALL _Matrix3f::yacArrayGetPointer(void) {
   return (void*)floats;
}

sUI YAC_VCALL _Matrix3f::yacArrayGetWidth(void) {
   return 3u;
}

sUI YAC_VCALL _Matrix3f::yacArrayGetHeight(void) {
   return 3u;
}

sUI YAC_VCALL _Matrix3f::yacArrayGetStride(void) {
   return 3u * sizeof(sF32);
}

sSI YAC_VCALL _Matrix3f::yacTensorRank(void) {
   return YAC_TENSOR_RANK_MATRIX;
}

void YAC_VCALL _Matrix3f::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F32(floats[0]);
   YAC_SERIALIZE_F32(floats[1]);
   YAC_SERIALIZE_F32(floats[2]);
   YAC_SERIALIZE_F32(floats[3]);
   YAC_SERIALIZE_F32(floats[4]);
   YAC_SERIALIZE_F32(floats[5]);
   YAC_SERIALIZE_F32(floats[6]);
   YAC_SERIALIZE_F32(floats[7]);
   YAC_SERIALIZE_F32(floats[8]);
}

sUI YAC_VCALL _Matrix3f::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   floats[0] = YAC_DESERIALIZE_F32();
   floats[1] = YAC_DESERIALIZE_F32();
   floats[2] = YAC_DESERIALIZE_F32();
   floats[3] = YAC_DESERIALIZE_F32();
   floats[4] = YAC_DESERIALIZE_F32();
   floats[5] = YAC_DESERIALIZE_F32();
   floats[6] = YAC_DESERIALIZE_F32();
   floats[7] = YAC_DESERIALIZE_F32();
   floats[8] = YAC_DESERIALIZE_F32();
   return 1u;
}

void YAC_VCALL _Matrix3f::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Matrix3f) &&
      YAC_CHK(_max, clid_Matrix3f)
      )
   {
      YAC_CAST_ARG(_Matrix3f, min, _min);
      YAC_CAST_ARG(_Matrix3f, max, _max);

      for(sUI i = 0u; i < 9u; i++)
      {
         if(floats[i] < min->floats[i])
            floats[i] = min->floats[i];
         else if(floats[i] > max->floats[i])
            floats[i] = max->floats[i];
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Matrix3f::yacOperatorClamp: invalid object(s)");
   }
}

void YAC_VCALL _Matrix3f::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Matrix3f) &&
      YAC_CHK(_max, clid_Matrix3f)
      )
   {
      YAC_CAST_ARG(_Matrix3f, min, _min);
      YAC_CAST_ARG(_Matrix3f, max, _max);

      for(sUI i = 0u; i < 9u; i++)
      {
         if(floats[i] < min->floats[i])
            floats[i] += (max->floats[i] - min->floats[i]);
         else if(floats[i] > max->floats[i])
            floats[i] -= (max->floats[i] - min->floats[i]);
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Matrix3f::yacOperatorWrap: invalid object(s)");
   }
}

void YAC_VCALL _Matrix3f::yacOperator(sSI _cmd,YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Matrix3f)) {
      _Matrix3f *o=(_Matrix3f*)_o;
      switch (_cmd) {
         case YAC_OP_CEQ:
            YAC_RETI( Dfltequal(floats[0], o->floats[0]) &&
                      Dfltequal(floats[1], o->floats[1]) &&
                      Dfltequal(floats[2], o->floats[2]) &&
                      Dfltequal(floats[3], o->floats[3]) &&
                      Dfltequal(floats[4], o->floats[4]) &&
                      Dfltequal(floats[5], o->floats[5]) &&
                      Dfltequal(floats[6], o->floats[6]) &&
                      Dfltequal(floats[7], o->floats[7]) &&
                      Dfltequal(floats[8], o->floats[8]) );
            break;
         case YAC_OP_CNE:
            YAC_RETI( Dfltnotequal(floats[0], o->floats[0]) ||
                      Dfltnotequal(floats[1], o->floats[1]) ||
                      Dfltnotequal(floats[2], o->floats[2]) ||
                      Dfltnotequal(floats[3], o->floats[3]) ||
                      Dfltnotequal(floats[4], o->floats[4]) ||
                      Dfltnotequal(floats[5], o->floats[5]) ||
                      Dfltnotequal(floats[6], o->floats[6]) ||
                      Dfltnotequal(floats[7], o->floats[7]) ||
                      Dfltnotequal(floats[8], o->floats[8]) );
            break;
         case YAC_OP_ASSIGN:
            floats[0] = o->floats[0];
            floats[1] = o->floats[1];
            floats[2] = o->floats[2];
            floats[3] = o->floats[3];
            floats[4] = o->floats[4];
            floats[5] = o->floats[5];
            floats[6] = o->floats[6];
            floats[7] = o->floats[7];
            floats[8] = o->floats[8];
            break;
         case YAC_OP_ADD:
            {
               //_Matrix3f *r=YAC_NEW_POOLED(Matrix3f);
               //*r=*this;
               //r->_add(_o);
               //_r->initObject(r,1);
               _add_YAC_RSELF(_o);
            }
            break;
         case YAC_OP_SUB:
            {
               //_Matrix3f *r=YAC_NEW_POOLED(Matrix3f);
               //*r=*this;
               //r->_sub(_o);
               //_r->initObject(r,1);
               _sub_YAC_RSELF(_o);
            }
            break;
         case YAC_OP_MUL:
            {
               //_Matrix3f *r=YAC_NEW_POOLED(Matrix3f);
               //*r=*this;
               //r->_mul(_o);
               //_r->initObject(r,1);
               _mul_YAC_RSELF(_o);
            }
            break;

         case YAC_OP_NEG:
            floats[0] = -floats[0];
            floats[1] = -floats[1];
            floats[2] = -floats[2];
            floats[3] = -floats[3];
            floats[4] = -floats[4];
            floats[5] = -floats[5];
            floats[6] = -floats[6];
            floats[7] = -floats[7];
            floats[8] = -floats[8];
            break;

         case YAC_OP_INIT:
            break;

         default:
         {
            char buf[128];
            Dyac_snprintf(buf, 128, "tkmath::Matrix3f::yacOperator: unsupported cmd %d!\n", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
            return;
         }

      }
   }
   else if(YAC_Is_FloatArray(_o))
   {
      YAC_FloatArray *o = (YAC_FloatArray*)_o;
      switch(_cmd)
      {
         case YAC_OP_ASSIGN:
            if(o->num_elements < 9u)
            {
               sUI i;
               for(i = 0u; i < o->num_elements; i++)
                  floats[i] = o->elements[i];
               for(; i < 9u; i++)
                  floats[i] = 0.0f;  // (todo) diagonal = 1
            }
            else
            {
               floats[0] = o->elements[0];
               floats[1] = o->elements[1];
               floats[2] = o->elements[2];
               floats[3] = o->elements[3];
               floats[4] = o->elements[4];
               floats[5] = o->elements[5];
               floats[6] = o->elements[6];
               floats[7] = o->elements[7];
               floats[8] = o->elements[8];
            }
            break;
         default:
            {
               YAC_NEW_STACK(Matrix3f, t);
               t.assignFloatArray(o);
               yacOperator(_cmd,&t,_r);
               //Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix3f::yacOperator Only Assign supported with FloatArray Type");
            }
            break;
      }
   } else if (YAC_VALID(_o)) {
      sF32 val;
      if (_o->yacScanF32(&val)) {
         yacOperatorF32(_cmd, val, _r);
      } else
      switch (_cmd) {
         case YAC_OP_ASSIGN:
	    assignGeneric(NULL,_o);
	    break;
	 default:
	    Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix3f::yacOperator Only Assign supported with Generic Type");
	    break;
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
            floats[6] = -floats[6];
            floats[7] = -floats[7];
            floats[8] = -floats[8];
            break;
         default:
            YAC_Object::yacOperator(_cmd,_o,_r);
            break;
      }
   }
}

void _Matrix3f::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF32(_cmd, (sF32)val, _r);
}

void _Matrix3f::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
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
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3f::yacOperatorF32 OP_DIV Cannot divide by zero");
         }
         break;
      default:
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Matrix3f::yacOperatorF32 Operator not supported %d",_cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
         break;
   }
}

void _Matrix3f::assignFloats(sF32 *_fv, sUI _ne) {
   if(9u != _ne)
   {
      Dyac_throw_def(WriteArrayOutOfBounds,"tkmath::Matrix3f Array not of size 9");
   }
   else
   {
      // Copy two elements
      floats[0] = _fv[0];
      floats[1] = _fv[1];
      floats[2] = _fv[2];
      floats[3] = _fv[3];
      floats[4] = _fv[4];
      floats[5] = _fv[5];
      floats[6] = _fv[6];
      floats[7] = _fv[7];
      floats[8] = _fv[8];
   }
}

void _Matrix3f::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Matrix3f::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
   sUI ne = _o->yacArrayGetNumElements();
   sUI nH = _o->yacArrayGetHeight();
   sUI nW = _o->yacArrayGetWidth();
   if(0u == nH)
   {
      if(9u == nW)
      {
         nH = 3u;
         nW = 3u;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix3f Array not of size 9");
      }
   }
   if(ne > 0u)
   {
      if(YAC_TYPE_FLOAT == _o->yacArrayGetElementType())
      {
         sF32 *fv = (sF32*) _o->yacArrayGetPointer();
         if(NULL != fv)
         {
            // Copy fast
            sUI i,j;
            for (i=0;(i<3)&&(i<nH);i++) {
               for (j=0;(j<3)&&(j<nW);j++) {
                  floats[(i*3)+j]=fv[(i*nW)+j];
               }
               for (;j<3;j++) {
                  if (i!=j) floats[(i*3)+j]=0.0f;
                  else floats[(i*3)+j]=1.0f;
               }
            }
            for (;(i<3);i++) {
               for (j=0;j<3;j++) {
                  if (i!=j) floats[(i*3)+j]=0.0f;
                  else floats[(i*3)+j]=1.0f;
               }
            }

            return;
         }
         else
         {
            // Retrieve individual float elements and assign
            YAC_Value v;
            sUI i,j;
            for (i=0;(i<3)&&(i<nH);i++) {
               for (j=0;(j<3)&&(j<nW);j++) {
                  _o->yacArrayGet(_ctx, (i*nW)+j, &v);
                  floats[(i*3)+j]=v.value.float_val;
               }
               for (;j<3;j++) {
                  if (i!=j) floats[(i*3)+j]=0.0f;
                  else floats[(i*3)+j]=1.0f;
               }
            }
            for (;(i<3);i++) {
               for (j=0;j<3;j++) {
                  if (i!=j) floats[(i*3)+j]=0.0f;
                  else floats[(i*3)+j]=1.0f;
               }
            }
         }
      }
      else
      {
         // Retrieve individual elements, typecast to float and assign
         YAC_Value v;
         sUI i,j;
         for (i=0;(i<3)&&(i<nH);i++) {
            for (j=0;(j<3)&&(j<nW);j++) {
               _o->yacArrayGet(_ctx, (i*nW)+j, &v);
               v.typecast(YAC_TYPE_FLOAT);
               floats[(i*3)+j]=v.value.float_val;
               v.unset();
            }
            for (;j<3;j++) {
               if (i!=j) floats[(i*3)+j]=0.0f;
	       else floats[(i*3)+j]=1.0f;
            }
         }
         for (;(i<3);i++) {
            for (j=0;j<3;j++) {
               if (i!=j) floats[(i*3)+j]=0.0f;
	       else floats[(i*3)+j]=1.0f;
            }
         }
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      ::memset((void*)floats, 0, sizeof(floats));
   }
}

void _Matrix3f::_setA(sF32 x) {
   floats[0] = x;
}

void _Matrix3f::_setB(sF32 y) {
   floats[1] = y;
}

void _Matrix3f::_setC(sF32 x) {
   floats[2] = x;
}

void _Matrix3f::_setD(sF32 y) {
   floats[3] = y;
}

void _Matrix3f::_setE(sF32 y) {
   floats[4] = y;
}

void _Matrix3f::_setF(sF32 y) {
   floats[5] = y;
}

void _Matrix3f::_setG(sF32 y) {
   floats[6] = y;
}

void _Matrix3f::_setH(sF32 y) {
   floats[7] = y;
}

void _Matrix3f::_setI(sF32 y) {
   floats[8] = y;
}

sF32 _Matrix3f::_getA(void) {
   return floats[0];
}

sF32 _Matrix3f::_getB(void) {
   return floats[1];
}

sF32 _Matrix3f::_getC(void) {
   return floats[2];
}

sF32 _Matrix3f::_getD(void) {
   return floats[3];
}

sF32 _Matrix3f::_getE(void) {
   return floats[4];
}

sF32 _Matrix3f::_getF(void) {
   return floats[5];
}

sF32 _Matrix3f::_getG(void) {
   return floats[6];
}

sF32 _Matrix3f::_getH(void) {
   return floats[7];
}

sF32 _Matrix3f::_getI(void) {
   return floats[8];
}

sBool YAC_VCALL _Matrix3f::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(1024,0,0,0);
   s->printf("((%g, %g, %g), (%g, %g, %g), (%g, %g, %g))",
             floats[0],floats[1],floats[2],
             floats[3],floats[4],floats[5],
             floats[6],floats[7],floats[8]
             );
   s->fixLength();
   return YAC_TRUE;
}

void _Matrix3f::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   yacToString(s);
   YAC_RETS(s,1);
}

void _Matrix3f::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix3f))
   {
      _Matrix3f *o = (_Matrix3f*)_o;
      for(sUI i = 0u; i < 9u; i++)
         floats[i] = floats[i] + o->floats[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::add_SELF Object not of type Matrix3f");
   }
}

void _Matrix3f::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix3f))
   {
      _Matrix3f *o = (_Matrix3f*)_o;
      _Matrix3f *r = YAC_NEW_POOLED(Matrix3f);
      for(sUI i = 0u; i < 9u; i++)
         r->floats[i] = floats[i] + o->floats[i];
      _r->initObject(r,1);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::add_VAL Object not of type Matrix3f");
   }
}

void _Matrix3f::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix3f) && YAC_CHK(_r, clid_Matrix3f))
   {
      _Matrix3f *o = (_Matrix3f*)_o;
      _Matrix3f *r = (_Matrix3f*)_r;
      for(sUI i = 0u; i < 9u; i++)
         r->floats[i] = floats[i] + o->floats[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::add_ARG Object not of type Matrix3f");
   }
}

void _Matrix3f::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o,clid_Matrix3f))
   {
      _Matrix3f *o = (_Matrix3f*)_o;
      for(sUI i = 0u; i < 9u; i++)
         floats[i] -= o->floats[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::sub_SELF Object not of type Matrix3f");
   }
}

void _Matrix3f::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o,clid_Matrix3f))
   {
      _Matrix3f *o = (_Matrix3f*)_o;
      _Matrix3f *r = YAC_NEW_POOLED(Matrix3f);
      for(sUI i = 0u; i < 9u; i++)
         r->floats[i] = floats[i] - o->floats[i];
      _r->initObject(r,1);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::sub_VAL Object not of type Matrix3f");
   }
}

void _Matrix3f::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix3f) &&
      YAC_CHK(_r, clid_Matrix3f)
      )
   {
      _Matrix3f *o = (_Matrix3f*)_o;
      _Matrix3f *r = (_Matrix3f*)_r;
      for(sUI i = 0u; i < 9u; i++)
         r->floats[i] = floats[i] - o->floats[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::sub_ARG Object not of type Matrix3f");
   }
}

void _Matrix3f::_mulf_YAC_RSELF(sF32 s) {
   for(sUI i = 0u; i < 9u; i++)
      floats[i] *= s;
}

void _Matrix3f::_mulf_YAC_RVAL(sF32 s, YAC_Value *_r) {
   _Matrix3f *r=YAC_NEW_POOLED(Matrix3f);
   r->floats[0] = floats[0] * s;
   r->floats[1] = floats[1] * s;
   r->floats[2] = floats[2] * s;
   r->floats[3] = floats[3] * s;
   r->floats[4] = floats[4] * s;
   r->floats[5] = floats[5] * s;
   r->floats[6] = floats[6] * s;
   r->floats[7] = floats[7] * s;
   r->floats[8] = floats[8] * s;
   _r->initObject(r,1);
}

void _Matrix3f::_mulf_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if (YAC_CHK(_r, clid_Matrix3f)) {
      _Matrix3f *r=(_Matrix3f*)_r;
      r->floats[0] = floats[0] * s;
      r->floats[1] = floats[1] * s;
      r->floats[2] = floats[2] * s;
      r->floats[3] = floats[3] * s;
      r->floats[4] = floats[4] * s;
      r->floats[5] = floats[5] * s;
      r->floats[6] = floats[6] * s;
      r->floats[7] = floats[7] * s;
      r->floats[8] = floats[8] * s;
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::mulf_ARG Return object not of type Matrix3f");
   }
}

void _Matrix3f::_mul_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o,clid_Matrix3f))
   {
      YAC_CAST_ARG(_Matrix3f, o, _o);
      const sF32 a = floats[0]*o->floats[0] + floats[1]*o->floats[3] + floats[2]*o->floats[6];
      const sF32 b = floats[0]*o->floats[1] + floats[1]*o->floats[4] + floats[2]*o->floats[7];
      const sF32 c = floats[0]*o->floats[2] + floats[1]*o->floats[5] + floats[2]*o->floats[8];
      const sF32 d = floats[3]*o->floats[0] + floats[4]*o->floats[3] + floats[5]*o->floats[6];
      const sF32 e = floats[3]*o->floats[1] + floats[4]*o->floats[4] + floats[5]*o->floats[7];
      const sF32 f = floats[3]*o->floats[2] + floats[4]*o->floats[5] + floats[5]*o->floats[8];
      const sF32 g = floats[6]*o->floats[0] + floats[7]*o->floats[3] + floats[8]*o->floats[6];
      const sF32 h = floats[6]*o->floats[1] + floats[7]*o->floats[4] + floats[8]*o->floats[7];
      const sF32 i = floats[6]*o->floats[2] + floats[7]*o->floats[5] + floats[8]*o->floats[8];
      floats[0] = a;
      floats[1] = b;
      floats[2] = c;
      floats[3] = d;
      floats[4] = e;
      floats[5] = f;
      floats[6] = g;
      floats[7] = h;
      floats[8] = i;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::mul_SELF Object not of type Matrix3f!\n");
   }
}

void _Matrix3f::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o,clid_Matrix3f))
   {
      _Matrix3f *o = (_Matrix3f*)_o;
      _Matrix3f *r = YAC_NEW_POOLED(Matrix3f);
      r->floats[0] = floats[0]*o->floats[0] + floats[1]*o->floats[3] + floats[2]*o->floats[6];
      r->floats[1] = floats[0]*o->floats[1] + floats[1]*o->floats[4] + floats[2]*o->floats[7];
      r->floats[2] = floats[0]*o->floats[2] + floats[1]*o->floats[5] + floats[2]*o->floats[8];
      r->floats[3] = floats[3]*o->floats[0] + floats[4]*o->floats[3] + floats[5]*o->floats[6];
      r->floats[4] = floats[3]*o->floats[1] + floats[4]*o->floats[4] + floats[5]*o->floats[7];
      r->floats[5] = floats[3]*o->floats[2] + floats[4]*o->floats[5] + floats[5]*o->floats[8];
      r->floats[6] = floats[6]*o->floats[0] + floats[7]*o->floats[3] + floats[8]*o->floats[6];
      r->floats[7] = floats[6]*o->floats[1] + floats[7]*o->floats[4] + floats[8]*o->floats[7];
      r->floats[8] = floats[6]*o->floats[2] + floats[7]*o->floats[5] + floats[8]*o->floats[8];
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::mul_VAL Object not of type Matrix3f!\n");
   }
}

void _Matrix3f::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o,clid_Matrix3f)&&
      YAC_CHK(_r,clid_Matrix3f)
      )
   {
      _Matrix3f *o = (_Matrix3f*)_o;
      _Matrix3f *r = (_Matrix3f*)_r;
      r->floats[0] = floats[0]*o->floats[0] + floats[1]*o->floats[3] + floats[2]*o->floats[6];
      r->floats[1] = floats[0]*o->floats[1] + floats[1]*o->floats[4] + floats[2]*o->floats[7];
      r->floats[2] = floats[0]*o->floats[2] + floats[1]*o->floats[5] + floats[2]*o->floats[8];
      r->floats[3] = floats[3]*o->floats[0] + floats[4]*o->floats[3] + floats[5]*o->floats[6];
      r->floats[4] = floats[3]*o->floats[1] + floats[4]*o->floats[4] + floats[5]*o->floats[7];
      r->floats[5] = floats[3]*o->floats[2] + floats[4]*o->floats[5] + floats[5]*o->floats[8];
      r->floats[6] = floats[6]*o->floats[0] + floats[7]*o->floats[3] + floats[8]*o->floats[6];
      r->floats[7] = floats[6]*o->floats[1] + floats[7]*o->floats[4] + floats[8]*o->floats[7];
      r->floats[8] = floats[6]*o->floats[2] + floats[7]*o->floats[5] + floats[8]*o->floats[8];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::mul_ARG Object not of type Matrix3f!\n");
   }
}

void _Matrix3f::_mulv_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Vector3f))
   {
      _Vector3f *o = (_Vector3f*)_o;
      _Vector3f *r = YAC_NEW_POOLED(Vector3f);
      r->floats[0] = floats[0]*o->floats[0] + floats[1]*o->floats[1] + floats[2]*o->floats[2];
      r->floats[1] = floats[3]*o->floats[0] + floats[4]*o->floats[1] + floats[5]*o->floats[2];
      r->floats[2] = floats[6]*o->floats[0] + floats[7]*o->floats[1] + floats[8]*o->floats[2];
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::mulv Object not of type Vector3f");
   }
}

void _Matrix3f::_mulv_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Vector3f) &&
      YAC_CHK(_r, clid_Vector3f)
      )
   {
      YAC_CAST_ARG(_Vector3f, o, _o);
      YAC_CAST_ARG(_Vector3f, r, _r);

      if((void*)o != (void*)r)
      {
         r->floats[0] = floats[0]*o->floats[0] + floats[1]*o->floats[1] + floats[2]*o->floats[2];
         r->floats[1] = floats[3]*o->floats[0] + floats[4]*o->floats[1] + floats[5]*o->floats[2];
         r->floats[2] = floats[6]*o->floats[0] + floats[7]*o->floats[1] + floats[8]*o->floats[2];
      }
      else
      {
         sF32 x = floats[0]*o->floats[0] + floats[1]*o->floats[1] + floats[2]*o->floats[2];
         sF32 y = floats[3]*o->floats[0] + floats[4]*o->floats[1] + floats[5]*o->floats[2];
         sF32 z = floats[6]*o->floats[0] + floats[7]*o->floats[1] + floats[8]*o->floats[2];

         r->floats[0] = x;
         r->floats[1] = y;
         r->floats[2] = z;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::mulv Object not of type Vector3f");
   }
}

//get "length" of matrix
sF32 _Matrix3f::_getAbs(void) const {
   return
      sqrtf(floats[0]*floats[0] + floats[1]*floats[1] + floats[2]*floats[2] +
            floats[3]*floats[3] + floats[4]*floats[4] + floats[5]*floats[5] +
            floats[6]*floats[6] + floats[7]*floats[7] + floats[8]*floats[8]
            );
}

//get squared "length" of matrix
sF32 _Matrix3f::_getAbsSqr(void) const {
   return (floats[0]*floats[0]+floats[1]*floats[1]+floats[2]*floats[2]+
      floats[3]*floats[3]+floats[4]*floats[4]+floats[5]*floats[5]+
      floats[6]*floats[6]+floats[7]*floats[7]+floats[8]*floats[8]);
}

//scale "length" of matrix to 1
void _Matrix3f::_unit_YAC_RSELF(void) {
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
      floats[6] *= absval;
      floats[7] *= absval;
      floats[8] *= absval;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3f::unit_SELF Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix3f::_unit_YAC_RVAL(YAC_Value *_r) {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      _Matrix3f *r = YAC_NEW_POOLED(Matrix3f);
      absval = 1.0f / absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
      r->floats[2] = floats[2] * absval;
      r->floats[3] = floats[3] * absval;
      r->floats[4] = floats[4] * absval;
      r->floats[5] = floats[5] * absval;
      r->floats[6] = floats[6] * absval;
      r->floats[7] = floats[7] * absval;
      r->floats[8] = floats[8] * absval;
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3f::unit_VAL Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix3f::_unit_YAC_RARG(YAC_Object *_r) const {
   if (YAC_CHK(_r, clid_Matrix3f)) {
      sF32 absval=_getAbs();
      if (Dfltnonzero(absval)) {
         _Matrix3f *r=(_Matrix3f*)_r;
         absval=1.0f/absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
         r->floats[2] = floats[2] * absval;
         r->floats[3] = floats[3] * absval;
         r->floats[4] = floats[4] * absval;
         r->floats[5] = floats[5] * absval;
         r->floats[6] = floats[6] * absval;
         r->floats[7] = floats[7] * absval;
         r->floats[8] = floats[8] * absval;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3f::unit_SELF Zero Matrix cannot be normalized!\n");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::unit_ARG Return object not of type Matrix3f");
   }
}

//scale "length" of matrix to s
void _Matrix3f::_unitScale_YAC_RSELF(sF32 s) {
   sF32 absval=_getAbs();
   if (Dfltnonzero(absval)) {
      absval=s/absval;
      floats[0]*=absval;
      floats[1]*=absval;
      floats[2]*=absval;
      floats[3]*=absval;
      floats[4]*=absval;
      floats[5]*=absval;
      floats[6]*=absval;
      floats[7]*=absval;
      floats[8]*=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3f::unitScale_SELF Zero Matrix cannot be normalized");
   }
}

void _Matrix3f::_unitScale_YAC_RVAL(sF32 s, YAC_Value *_r) {
   sF32 absval=_getAbs();
   if (Dfltnonzero(absval)) {
      _Matrix3f *r=YAC_NEW_POOLED(Matrix3f);
      absval=s/absval;
      r->floats[0] = floats[0] * absval;
      r->floats[1] = floats[1] * absval;
      r->floats[2] = floats[2] * absval;
      r->floats[3] = floats[3] * absval;
      r->floats[4] = floats[4] * absval;
      r->floats[5] = floats[5] * absval;
      r->floats[6] = floats[6] * absval;
      r->floats[7] = floats[7] * absval;
      r->floats[8] = floats[8] * absval;
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3f::unitScale_VAL Zero Matrix cannot be normalized");
   }
}

void _Matrix3f::_unitScale_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if (YAC_CHK(_r,clid_Matrix3f)) {
      sF32 absval=_getAbs();
      if (Dfltnonzero(absval)) {
         _Matrix3f *r=(_Matrix3f*)_r;
         absval=s/absval;
         r->floats[0] = floats[0] * absval;
         r->floats[1] = floats[1] * absval;
         r->floats[2] = floats[2] * absval;
         r->floats[3] = floats[3] * absval;
         r->floats[4] = floats[4] * absval;
         r->floats[5] = floats[5] * absval;
         r->floats[6] = floats[6] * absval;
         r->floats[7] = floats[7] * absval;
         r->floats[8] = floats[8] * absval;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3f::unitScale_ARG Zero Matrix cannot be normalized");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::unitScale_ARG Return object not of type Matrix3f");
   }
}

sF32 _Matrix3f::_det(void) const {
   // return determinant of matrix
   return (floats[0]*floats[4]*floats[8]
      +floats[1]*floats[5]*floats[6]
      +floats[2]*floats[3]*floats[7]
      -floats[2]*floats[4]*floats[6]
      -floats[0]*floats[5]*floats[7]
      -floats[1]*floats[3]*floats[8]);
}

void _Matrix3f::_invert_YAC_RSELF(void) {
   // invert matrix
   sF32 detval=_det();
   if (Dfltnonzero(detval)) {
      sF32 a,b,c,d,e,f,g,h,i;
      a=(floats[4]*floats[8]-floats[5]*floats[7])/detval;
      b=(floats[2]*floats[7]-floats[1]*floats[8])/detval;
      c=(floats[1]*floats[5]-floats[2]*floats[4])/detval;
      d=(floats[5]*floats[6]-floats[3]*floats[8])/detval;
      e=(floats[0]*floats[8]-floats[2]*floats[6])/detval;
      f=(floats[2]*floats[3]-floats[0]*floats[5])/detval;
      g=(floats[3]*floats[7]-floats[4]*floats[6])/detval;
      h=(floats[1]*floats[6]-floats[0]*floats[7])/detval;
      i=(floats[0]*floats[4]-floats[1]*floats[3])/detval;
      floats[0]=a;
      floats[1]=b;
      floats[2]=c;
      floats[3]=d;
      floats[4]=e;
      floats[5]=f;
      floats[6]=g;
      floats[7]=h;
      floats[8]=i;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3f::invert_SELF Invalid Matrix (det==0)");
   }
}

void _Matrix3f::_invert_YAC_RVAL(YAC_Value *_r) {
   sF32 detval=_det();
   if (Dfltnonzero(detval)) {
      _Matrix3f *r=YAC_NEW_POOLED(Matrix3f);
      r->floats[0]=(floats[4]*floats[8]-floats[5]*floats[7])/detval;
      r->floats[1]=(floats[2]*floats[7]-floats[1]*floats[8])/detval;
      r->floats[2]=(floats[1]*floats[5]-floats[2]*floats[4])/detval;
      r->floats[3]=(floats[5]*floats[6]-floats[3]*floats[8])/detval;
      r->floats[4]=(floats[0]*floats[8]-floats[2]*floats[6])/detval;
      r->floats[5]=(floats[2]*floats[3]-floats[0]*floats[5])/detval;
      r->floats[6]=(floats[3]*floats[7]-floats[4]*floats[6])/detval;
      r->floats[7]=(floats[1]*floats[6]-floats[0]*floats[7])/detval;
      r->floats[8]=(floats[0]*floats[4]-floats[1]*floats[3])/detval;
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3f::invert_VAL Invalid Matrix (det==0)");
   }
}

void _Matrix3f::_invert_YAC_RARG(YAC_Object *_r) const {
   if (YAC_CHK(_r,clid_Matrix3f)) {
      sF32 detval=_det();
      if (Dfltnonzero(detval)) {
         _Matrix3f *r=(_Matrix3f*)_r;
         r->floats[0]=(floats[4]*floats[8]-floats[5]*floats[7])/detval;
         r->floats[1]=(floats[2]*floats[7]-floats[1]*floats[8])/detval;
         r->floats[2]=(floats[1]*floats[5]-floats[2]*floats[4])/detval;
         r->floats[3]=(floats[5]*floats[6]-floats[3]*floats[8])/detval;
         r->floats[4]=(floats[0]*floats[8]-floats[2]*floats[6])/detval;
         r->floats[5]=(floats[2]*floats[3]-floats[0]*floats[5])/detval;
         r->floats[6]=(floats[3]*floats[7]-floats[4]*floats[6])/detval;
         r->floats[7]=(floats[1]*floats[6]-floats[0]*floats[7])/detval;
         r->floats[8]=(floats[0]*floats[4]-floats[1]*floats[3])/detval;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3f::invert_ARG Invalid Matrix (det==0)");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::invert_ARG Return object not of type Matrix3f");
   }
}

void _Matrix3f::_transpose_YAC_RSELF(void) {
   sF32 a,b,c;
   a=floats[1];
   b=floats[2];
   c=floats[5];
   floats[1]=floats[3];
   floats[2]=floats[6];
   floats[5]=floats[7];
   floats[3]=a;
   floats[6]=b;
   floats[7]=c;
}

void _Matrix3f::_transpose_YAC_RVAL(YAC_Value *_r) {
   _Matrix3f *r=YAC_NEW_POOLED(Matrix3f);
   r->floats[0]=floats[0]; r->floats[1]=floats[3]; r->floats[2]=floats[6];
   r->floats[3]=floats[1]; r->floats[4]=floats[4]; r->floats[5]=floats[7];
   r->floats[6]=floats[2]; r->floats[7]=floats[5]; r->floats[8]=floats[8];
   _r->initObject(r,1);
}

void _Matrix3f::_transpose_YAC_RARG(YAC_Object *_r) const {
   if (YAC_CHK(_r,clid_Matrix3f)) {
      _Matrix3f *r=(_Matrix3f*)_r;
      r->floats[0]=floats[0]; r->floats[1]=floats[3]; r->floats[2]=floats[6];
      r->floats[3]=floats[1]; r->floats[4]=floats[4]; r->floats[5]=floats[7];
      r->floats[6]=floats[2]; r->floats[7]=floats[5]; r->floats[8]=floats[8];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::tranpose_ARG Return object not of type Matrix3f");
   }
}

void _Matrix3f::_setRow(sUI _row, YAC_Object *_o) {
   _initRowv(_row, _o);
}

void _Matrix3f::_getRow_YAC_RVAL(sUI _row, YAC_Value *_r) {
   if(_row < 3u)
   {
      _Vector3f *r = YAC_NEW_POOLED(Vector3f);
      r->_init(TM3F(_row, 0), TM3F(_row, 1), TM3F(_row, 2));
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix3f::getRow_VAL: invalid row index (%u > 2)\n", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3f::_getRow_YAC_RARG(sUI _row, YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Vector3f))
   {
      if(_row < 3u)
      {
         YAC_CAST_ARG(_Vector3f, r, _r);
         r->_init(TM3F(_row, 0),
                  TM3F(_row, 1),
                  TM3F(_row, 2)
                  );
      }
      else
      {
         char buf[128];
         Dyac_snprintf(buf, 128, "tkmath::Matrix3f::getRow_RARG: invalid row index (%u > 2)\n", _row);
         Dyac_throw_def(WriteArrayOutOfBounds, buf);
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix3f::getRow_RARG: return object is not a Vector3f");
   }
}

void _Matrix3f::_setCol(sUI _col, YAC_Object *_v) {
   _initColv(_col, _v);
}

void _Matrix3f::_getCol_YAC_RVAL(sUI _col, YAC_Value *_r) {
   if(_col < 3u)
   {
      _Vector3f *r = YAC_NEW_POOLED(Vector3f);
      r->_init(TM3F(0, _col),
               TM3F(1, _col),
               TM3F(2, _col)
               );
      _r->initObject(r, 1);
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix3f::getCol_VAL: invalid column index (%u > 2)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3f::_getCol_YAC_RARG(sUI _col, YAC_Object *_r) const {
   if(_col < 3u)
   {
      if(YAC_CHK(_r, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, r, _r);
         r->_init(TM3F(0, _col),
                  TM3F(1, _col),
                  TM3F(2, _col)
                  );
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix3f::getCol_RARG: return object is not a Vector3f");
      }
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix3f::getCol_RARG: invalid column index (%u > 2)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3f::_initIdentity(void) {
   TM3F(0,0) = 1.0f;  TM3F(0,1) = 0.0f;  TM3F(0,2) = 0.0f;
   TM3F(1,0) = 0.0f;  TM3F(1,1) = 1.0f;  TM3F(1,2) = 0.0f;
   TM3F(2,0) = 0.0f;  TM3F(2,1) = 0.0f;  TM3F(2,2) = 1.0f;
}

sBool _Matrix3f::_isIdentity(void) {
   sBool bIdentity = YAC_TRUE;
   for(sUI rowIdx = 0u; rowIdx < 3u; rowIdx++)
   {
      for(sUI colIdx = 0u; colIdx < 3u; colIdx++)
      {
         bIdentity = bIdentity && Dfltequal(TM3F(rowIdx,colIdx), (colIdx == rowIdx) ? 1.0f : 0.0f);
      }
   }
   return bIdentity;
}

void _Matrix3f::_init(sF32 a, sF32 b, sF32 c,
                      sF32 d, sF32 e, sF32 f,
                      sF32 g, sF32 h, sF32 i
                      ) {
   // a b c  (row-major)
   // d e f
   // g h i

   floats[0] = a;
   floats[1] = b;
   floats[2] = c;

   floats[3] = d;
   floats[4] = e;
   floats[5] = f;

   floats[6] = g;
   floats[7] = h;
   floats[8] = i;
}

void _Matrix3f::_initTranspose(sF32 a, sF32 b, sF32 c,
                               sF32 d, sF32 e, sF32 f,
                               sF32 g, sF32 h, sF32 i
                               ) {
   floats[ 0] = a;
   floats[ 1] = d;
   floats[ 2] = g;

   floats[ 3] = b;
   floats[ 4] = e;
   floats[ 5] = h;

   floats[ 6] = c;
   floats[ 7] = f;
   floats[ 8] = i;
}
   
void _Matrix3f::_initRowMajor(sF32 a, sF32 b, sF32 c,
                              sF32 d, sF32 e, sF32 f,
                              sF32 g, sF32 h, sF32 i
                              ) {
   // a b c   C-Array / Direct3D order
   // d e f
   // g h i
   if(b_column_major)
   {
      _initTranspose(a, b, c, d, e, f, g, h, i);
   }
   else
   {
      _init(a, b, c, d, e, f, g, h, i);
   }
}

void _Matrix3f::_initColumnMajor(sF32 a, sF32 b, sF32 c,
                                 sF32 d, sF32 e, sF32 f,
                                 sF32 g, sF32 h, sF32 i
                                 ) {
   // a d g   OpenGL order
   // b e h
   // c f i
   if(b_column_major)
   {
      _init(a, b, c, d, e, f, g, h, i);
   }
   else
   {
      _initTranspose(a, b, c, d, e, f, g, h, i);
   }
}

void _Matrix3f::_initRowf(sUI _row, sF32 _x, sF32 _y, sF32 _z) {
   if(_row < 3u)
   {
      TM3F(_row, 0) = _x;
      TM3F(_row, 1) = _y;
      TM3F(_row, 2) = _z;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3f::initRowf: invalid row index (%u > 2)", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3f::_initRowv(sUI _row, YAC_Object *_o) {
   if(_row < 3u)
   {
      if(YAC_CHK(_o, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, o, _o);
         TM3F(_row, 0) = o->floats[0];
         TM3F(_row, 1) = o->floats[1];
         TM3F(_row, 2) = o->floats[2];
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector3f, t);
         t.assignGeneric(NULL,_o);
         TM3F(_row, 0) = t.floats[0];
         TM3F(_row, 1) = t.floats[1];
         TM3F(_row, 2) = t.floats[2];
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix3f::initRowv: invalid arg object");
      }
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix3f::initRowv: invalid row index (%u > 2)\n", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3f::_initColf(sUI _col, sF32 _x, sF32 _y, sF32 _z) {
   if(_col < 3u)
   {
      TM3F(0, _col) = _x;
      TM3F(1, _col) = _y;
      TM3F(2, _col) = _z;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3f::initColf: invalid column index (%u > 2)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3f::_initColv(sUI _col, YAC_Object *_o) {
   if(_col < 3u)
   {
      if(YAC_CHK(_o, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, o, _o);
         TM3F(0,_col) = o->floats[0];
         TM3F(1,_col) = o->floats[1];
         TM3F(2,_col) = o->floats[2];
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector3f, t);
         t.assignGeneric(NULL, _o);
         TM3F(0,_col) = t.floats[0];
         TM3F(1,_col) = t.floats[1];
         TM3F(2,_col) = t.floats[2];
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix3f::initColv: invalid arg object");
      }
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix3f::initColv: invalid column index (%u > 2)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3f::_initScalef(sF32 _x, sF32 _y, sF32 _z) {
   _init(_x,  0,  0,
         0,  _y,  0,
         0,   0, _z
         );
}

void _Matrix3f::_initScalev(YAC_Object *_scale) {
   if(YAC_CHK(_scale, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, scale, _scale);
      _init(scale->floats[0], 0,                0,
            0,                scale->floats[1], 0,
            0,                0,                scale->floats[2]
            );
   }
   else if(YAC_CHK(_scale, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, scale, _scale);
      _init(scale->floats[0], 0,                0,
            0,                scale->floats[1], 0,
            0,                0,                1
            );
   }
   else if(YAC_VALID(_scale))
   {
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _scale);
      _init(v.floats[0], 0,           0,
            0,           v.floats[1], 0,
            0,           0,           1
            );
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::initScalev: invalid arg object");
   }
}

void _Matrix3f::_initTranslatef(sF32 _x, sF32 _y) {
   _initRowMajor(1, 0, _x,
                 0, 1, _y,
                 0, 0, 1
                 );
}

void _Matrix3f::_initTranslatev(YAC_Object *_translate) {
   if(YAC_CHK(_translate, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, translate, _translate);
      _initRowMajor(1, 0, translate->floats[0],
                    0, 1, translate->floats[1],
                    0, 0, 1
                    );
   }
   else if(YAC_VALID(_translate))
   {
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _translate);
      _initRowMajor(1, 0, v.floats[0],
                    0, 1, v.floats[1],
                    0, 0, 1
                    );
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::initTranslatev: \'translate\' is not a Vector2f");
   }
}

void _Matrix3f::_initRotatef(sF32 _x, sF32 _y, sF32 _z) {
   _initEulerXYZ(_x, _y, _z);
}

void _Matrix3f::_initRotatev(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      _initEulerXYZ(v->floats[0], v->floats[1], v->floats[2]);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _v);
      _initEulerXYZ(v.floats[0], v.floats[1], v.floats[2]);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::initRotatev: \'v\' is not a Vector3f");
   }
}

void _Matrix3f::_initEulerXYX(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c2;
   TM3F(0,1) = s1*s2;
   TM3F(0,2) = c1*s2;

   TM3F(1,0) = s2*s3;
   TM3F(1,1) = c1*c3-c2*s1*s3;
   TM3F(1,2) = -c3*s1-c1*c2*s3;

   TM3F(2,0) = -c3*s2;
   TM3F(2,1) = c2*c3*s1+c1*s3;
   TM3F(2,2) = c1*c2*c3-s1*s3;
}

void _Matrix3f::_initEulerXZX(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c2;
   TM3F(0,1) = -c1*s2;
   TM3F(0,2) = s1*s2;

   TM3F(1,0) = c3*s2;
   TM3F(1,1) = c1*c2*c3-s1*s3;
   TM3F(1,2) = -c2*c3*s1-c1*s3;

   TM3F(2,0) = s2*s3;
   TM3F(2,1) = c3*s1+c1*c2*s3;
   TM3F(2,2) = c1*c3-c2*s1*s3;
}

void _Matrix3f::_initEulerYXY(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c1*c3-c2*s1*s3;
   TM3F(0,1) = s2*s3;
   TM3F(0,2) = c3*s1+c1*c2*s3;

   TM3F(1,0) = s1*s2;
   TM3F(1,1) = c2;
   TM3F(1,2) = -c1*s2;

   TM3F(2,0) = -c2*c3*s1-c1*s3;
   TM3F(2,1) = c3*s2;
   TM3F(2,2) = c1*c2*c3-s1*s3;
}

void _Matrix3f::_initEulerYZY(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c1*c2*c3-s1*s3;
   TM3F(0,1) = -c3*s2;
   TM3F(0,2) = c2*c3*s1+c1*s3;

   TM3F(1,0) = c1*s2;
   TM3F(1,1) = c2;
   TM3F(1,2) = s1*s2;

   TM3F(2,0) = -c3*s1-c1*c2*s3;
   TM3F(2,1) = s2*s3;
   TM3F(2,2) = c1*c3-c2*s1*s3;
}

void _Matrix3f::_initEulerZXZ(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c1*c3-c2*s1*s3;
   TM3F(0,1) = -c3*s1-c1*c2*s3;
   TM3F(0,2) = s2*s3;

   TM3F(1,0) = c2*c3*s1+c1*s3;
   TM3F(1,1) = c1*c2*c3-s1*s3;
   TM3F(1,2) = -c3*s2;

   TM3F(2,0) = s1*s2;
   TM3F(2,1) = c1*s2;
   TM3F(2,2) = c2;
}

void _Matrix3f::_initEulerZYZ(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c1*c2*c3-s1*s3;
   TM3F(0,1) = -c2*c3*s1-c1*s3;
   TM3F(0,2) = c3*s2;

   TM3F(1,0) = c3*s1+c1*c2*s3;
   TM3F(1,1) = c1*c3-c2*s1*s3;
   TM3F(1,2) = s2*s3;

   TM3F(2,0) = -c1*s2;
   TM3F(2,1) = s1*s2;
   TM3F(2,2) = c2;
}

void _Matrix3f::_initEulerXYZ(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c2*c3;
   TM3F(0,1) = c3*s1*s2-c1*s3;
   TM3F(0,2) = c1*c3*s2+s1*s3;

   TM3F(1,0) = c2*s3;
   TM3F(1,1) = c1*c3+s1*s2*s3;
   TM3F(1,2) = c1*s2*s3-c3*s1;

   TM3F(2,0) = -s2;
   TM3F(2,1) = c2*s1;
   TM3F(2,2) = c1*c2;
}

void _Matrix3f::_initEulerXZY(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c2*c3;
   TM3F(0,1) = s1*s3-c1*c3*s2;
   TM3F(0,2) = c3*s1*s2+c1*s3;

   TM3F(1,0) = s2;
   TM3F(1,1) = c1*c2;
   TM3F(1,2) = -c2*s1;

   TM3F(2,0) = -c2*s3;
   TM3F(2,1) = c3*s1+c1*s2*s3;
   TM3F(2,2) = c1*c3-s1*s2*s3;
}

void _Matrix3f::_initEulerYXZ(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c1*c3-s1*s2*s3;
   TM3F(0,1) = -c2*s3;
   TM3F(0,2) = c3*s1+c1*s2*s3;

   TM3F(1,0) = c3*s1*s2+c1*s3;
   TM3F(1,1) = c2*c3;
   TM3F(1,2) = s1*s3-c1*c3*s2;

   TM3F(2,0) = -c2*s1;
   TM3F(2,1) = s2;
   TM3F(2,2) = c1*c2;
}

void _Matrix3f::_initEulerYZX(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c1*c2;
   TM3F(0,1) = -s2;
   TM3F(0,2) = c2*s1;

   TM3F(1,0) = c1*c3*s2+s1*s3;
   TM3F(1,1) = c2*c3;
   TM3F(1,2) = c3*s1*s2-c1*s3;

   TM3F(2,0) = c1*s2*s3-c3*s1;
   TM3F(2,1) = c2*s3;
   TM3F(2,2) = c1*c3+s1*s2*s3;
}

void _Matrix3f::_initEulerZXY(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c1*c3+s1*s2*s3;
   TM3F(0,1) = c1*s2*s3-c3*s1;
   TM3F(0,2) = c2*s3;

   TM3F(1,0) = c2*s1;
   TM3F(1,1) = c1*c2;
   TM3F(1,2) = -s2;

   TM3F(2,0) = c3*s1*s2-c1*s3;
   TM3F(2,1) = c1*c3*s2+s1*s3;
   TM3F(2,2) = c2*c3;
}

void _Matrix3f::_initEulerZYX(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM3F(0,0) = c1*c2;
   TM3F(0,1) = -c2*s1;
   TM3F(0,2) = s2;

   TM3F(1,0) = c3*s1+c1*s2*s3;
   TM3F(1,1) = c1*c3-s1*s2*s3;
   TM3F(1,2) = -c2*s3;

   TM3F(2,0) = s1*s3-c1*c3*s2;
   TM3F(2,1) = c3*s1*s2+c1*s3;
   TM3F(2,2) = c2*c3;
}

void _Matrix3f::_translatef_YAC_RSELF(sF32 _x, sF32 _y) {
   YAC_NEW_STACK(Matrix3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initTranslatef(_x, _y);

   _mul_YAC_RSELF(&t);
}

void _Matrix3f::_translatef_YAC_RVAL(sF32 _x, sF32 _y, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initTranslatef(_x, _y);

   _mul_YAC_RVAL(&t, _r);
}

void _Matrix3f::_translatef_YAC_RARG(sF32 _x, sF32 _y, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix3f))
   {
      YAC_NEW_STACK(Matrix3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initTranslatef(_x, _y);

      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::translatef_RARG: Object not of type Matrix3f");
   }
}

void _Matrix3f::_translatev_YAC_RSELF(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, v, _v);
      YAC_NEW_STACK(Matrix3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initTranslatef(v->floats[0], v->floats[1]);
      _mul_YAC_RSELF(&t);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::translatev_RARG: 'v' is not a Vector2f");
   }
}

void _Matrix3f::_translatev_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if(YAC_CHK(_v, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, v, _v);
      YAC_NEW_STACK(Matrix3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initTranslatef(v->floats[0], v->floats[1]);
      _mul_YAC_RVAL(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::translatev_RVAL: 'v' is not a Vector2f");
   }
}

void _Matrix3f::_translatev_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if(YAC_CHK(_v, clid_Vector2f))
   {
      if(YAC_CHK(_r, clid_Matrix3f))
      {
         YAC_CAST_ARG(_Vector2f, v, _v);
         YAC_NEW_STACK(Matrix3f, t);
         t._setColumnMajor(_getColumnMajor());
         t._initTranslatef(v->floats[0], v->floats[1]);
         _mul_YAC_RARG(&t, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::translatev_RARG: return object is not a Matrix3f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::translatev_RARG: 'v' is not a Vector2f");
   }
}

void _Matrix3f::_scalef_YAC_RSELF(sF32 _x, sF32 _y, sF32 _z) {
   YAC_NEW_STACK(Matrix3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initScalef(_x, _y, _z);

   _mul_YAC_RSELF(&t);
}

void _Matrix3f::_scalef_YAC_RVAL(sF32 _x, sF32 _y, sF32 _z, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initScalef(_x, _y, _z);

   _mul_YAC_RVAL(&t, _r);
}

void _Matrix3f::_scalef_YAC_RARG(sF32 _x, sF32 _y, sF32 _z, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix3f))
   {
      YAC_NEW_STACK(Matrix3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(_x, _y, _z);

      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::scalef_RARG: Object not of type Matrix3f");
   }
}

void _Matrix3f::_scalev_YAC_RSELF(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      YAC_NEW_STACK(Matrix3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(v->floats[0], v->floats[1], v->floats[2]);
      _mul_YAC_RSELF(&t);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::scalev_RARG: 'v' is not a Vector3f");
   }
}

void _Matrix3f::_scalev_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      YAC_NEW_STACK(Matrix3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(v->floats[0], v->floats[1], v->floats[2]);
      _mul_YAC_RVAL(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::scalev_RVAL: 'v' is not a Vector3f");
   }
}

void _Matrix3f::_scalev_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      if(YAC_CHK(_r, clid_Matrix3f))
      {
         YAC_CAST_ARG(_Vector3f, v, _v);
         YAC_NEW_STACK(Matrix3f, t);
         t._setColumnMajor(_getColumnMajor());
         t._initScalef(v->floats[0], v->floats[1], v->floats[2]);
         _mul_YAC_RARG(&t, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::scalev_RARG: return object is not a Matrix3f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::scalev_RARG: 'v' is not a Vector3f");
   }
}

void _Matrix3f::_rotatef_YAC_RSELF(sF32 _x, sF32 _y, sF32 _z) {
   YAC_NEW_STACK(Matrix3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initEulerXYZ(_x, _y, _z);
   _mul_YAC_RSELF(&t);
}

void _Matrix3f::_rotatef_YAC_RVAL(sF32 _x, sF32 _y, sF32 _z, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix3f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initEulerXYZ(_x, _y, _z);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix3f::_rotatef_YAC_RARG(sF32 _x, sF32 _y, sF32 _z, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix3f))
   {
      YAC_NEW_STACK(Matrix3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initEulerXYZ(_x, _y, _z);
      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::rotatef_RARG: Object not of type Matrix3f");
   }
}

void _Matrix3f::_rotatev_YAC_RSELF(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      YAC_NEW_STACK(Matrix3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initEulerXYZ(v->floats[0], v->floats[1], v->floats[2]);
      _mul_YAC_RSELF(&t);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::rotatev_RARG: \'v\' is not a Vector3f");
   }
}

void _Matrix3f::_rotatev_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      YAC_NEW_STACK(Matrix3f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initEulerXYZ(v->floats[0], v->floats[1], v->floats[2]);
      _mul_YAC_RVAL(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::rotatev_RVAL: \'v\' is not a Vector3f");
   }
}

void _Matrix3f::_rotatev_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      if(YAC_CHK(_r, clid_Matrix3f))
      {
         YAC_CAST_ARG(_Vector3f, v, _v);
         YAC_NEW_STACK(Matrix3f, t);
         t._setColumnMajor(_getColumnMajor());
         t._initEulerXYZ(v->floats[0], v->floats[1], v->floats[2]);
         _mul_YAC_RARG(&t, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::rotatev_RARG: return object is not a Matrix3f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3f::scalev_RARG: 'v' is not a Vector3f");
   }
}

void _Matrix3f::_New(sF32 a, sF32 b, sF32 c, sF32 d, sF32 e, sF32 f, sF32 g, sF32 h, sF32 i, YAC_Value *_r) {
   _Matrix3f *r = YAC_NEW_POOLED(Matrix3f);
   _r->initObject(r, YAC_TRUE);
   r->_init(a, b, c,  // row-major(default)    (args should be transposed in column-major mode)
            d, e, f,
            g, h, i
            );
}

void YAC_CALL _matrix3f(sF32 a, sF32 b, sF32 c, sF32 d, sF32 e, sF32 f, sF32 g, sF32 h, sF32 i, YAC_Value *_r) {
   _Matrix3f *r=YAC_NEW_POOLED(Matrix3f);
   _r->initObject(r, YAC_TRUE);
   r->_init(a, b, c,
            d, e, f,
            g, h, i
            );
}
