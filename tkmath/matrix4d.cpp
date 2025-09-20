/// matrix4d.cpp
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
#include "vector3d.h"
#include "vector4d.h"
#include "matrix4d.h"

#include "ying_Math.h"

// Helper macros to access elements
#define TM4D(r,c)  doubles[TM4(r,c)]
#define OM4D(r,c)  o->doubles[OM4(r,c)]
#define M4D(a,r,c) (a)->doubles[M4(a,r,c)]


_Matrix4d::_Matrix4d(void) {
   _initIdentity();
}

_Matrix4d::~_Matrix4d() {
}

sUI YAC_VCALL _Matrix4d::yacArrayGetNumElements(void) {
   return 16u;
}

sUI YAC_VCALL _Matrix4d::yacArrayGetMaxElements(void) {
   return 16u;
}

sUI YAC_VCALL _Matrix4d::yacArrayGetElementType(void) {
   return YAC_TYPE_OBJECT;
}

sUI YAC_VCALL _Matrix4d::yacArrayGetElementByteSize(void) {
   return sizeof(sF64);
}

void YAC_VCALL _Matrix4d::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 16u)
   {
      sF64 v = 0;
      switch(_val->type)
      {
         case YAC_TYPE_VOID:
            break;

         case YAC_TYPE_INT:
            v = (sF64)_val->value.int_val;
            break;

         case YAC_TYPE_FLOAT:
            v = (sF64)_val->value.float_val;
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
            Dyac_throw(TypeMismatch, "tkmath::Matrix4d::yacArraySet: invalid type");
            break;
      }
      doubles[_idx] = v;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix4d::yacArraySet: index out of bounds (%u > 15)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Matrix4d::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 16u)
   {
      YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
      _val->initObject(d, YAC_TRUE);
      d->value = doubles[_idx];
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix4d::yacArrayGet: index out of bounds (%u > 15)", _idx);
      Dyac_throw(ReadArrayOutOfBounds, buf);
      _val->initFloat(0.0f);
   }
}

void *YAC_VCALL _Matrix4d::yacArrayGetPointer(void) {
   return (void*)doubles;
}

sUI YAC_VCALL _Matrix4d::yacArrayGetWidth(void) {
   return 4u;
}

sUI YAC_VCALL _Matrix4d::yacArrayGetHeight(void) {
   return 4u;
}

sUI YAC_VCALL _Matrix4d::yacArrayGetStride(void) {
   return 4u * sizeof(sF64);
}

sSI YAC_VCALL _Matrix4d::yacTensorRank(void) {
   return YAC_TENSOR_RANK_MATRIX;
}

void YAC_VCALL _Matrix4d::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F64(doubles[0]);
   YAC_SERIALIZE_F64(doubles[1]);
   YAC_SERIALIZE_F64(doubles[2]);
   YAC_SERIALIZE_F64(doubles[3]);
   YAC_SERIALIZE_F64(doubles[4]);
   YAC_SERIALIZE_F64(doubles[5]);
   YAC_SERIALIZE_F64(doubles[6]);
   YAC_SERIALIZE_F64(doubles[7]);
   YAC_SERIALIZE_F64(doubles[8]);
   YAC_SERIALIZE_F64(doubles[9]);
   YAC_SERIALIZE_F64(doubles[10]);
   YAC_SERIALIZE_F64(doubles[11]);
   YAC_SERIALIZE_F64(doubles[12]);
   YAC_SERIALIZE_F64(doubles[13]);
   YAC_SERIALIZE_F64(doubles[14]);
   YAC_SERIALIZE_F64(doubles[15]);
}

sUI YAC_VCALL _Matrix4d::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   doubles[0]=YAC_DESERIALIZE_F64();
   doubles[1]=YAC_DESERIALIZE_F64();
   doubles[2]=YAC_DESERIALIZE_F64();
   doubles[3]=YAC_DESERIALIZE_F64();
   doubles[4]=YAC_DESERIALIZE_F64();
   doubles[5]=YAC_DESERIALIZE_F64();
   doubles[6]=YAC_DESERIALIZE_F64();
   doubles[7]=YAC_DESERIALIZE_F64();
   doubles[8]=YAC_DESERIALIZE_F64();
   doubles[9]=YAC_DESERIALIZE_F64();
   doubles[10]=YAC_DESERIALIZE_F64();
   doubles[11]=YAC_DESERIALIZE_F64();
   doubles[12]=YAC_DESERIALIZE_F64();
   doubles[13]=YAC_DESERIALIZE_F64();
   doubles[14]=YAC_DESERIALIZE_F64();
   doubles[15]=YAC_DESERIALIZE_F64();
   return 1;
}

void YAC_VCALL _Matrix4d::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Matrix4d) && YAC_CHK(_max,clid_Matrix4d))
   {
      YAC_CAST_ARG(_Matrix4d, min, _min);
      YAC_CAST_ARG(_Matrix4d, max, _max);

      for(sUI i = 0u; i < 16u; i++)
      {
         if (doubles[i] < min->doubles[i])
            doubles[i] = min->doubles[i];
         else if(doubles[i] > max->doubles[i])
            doubles[i] = max->doubles[i];
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Matrix4d::yacOperatorClamp: invalid object(s)");
   }
}

void YAC_VCALL _Matrix4d::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Matrix4d) && YAC_CHK(_max,clid_Matrix4d))
   {
      YAC_CAST_ARG(_Matrix4d, min, _min);
      YAC_CAST_ARG(_Matrix4d, max, _max);

      for(sUI i = 0u; i < 16u; i++)
      {
         if(doubles[i] < min->doubles[i])
            doubles[i] += max->doubles[i] - min->doubles[i];
         else if(doubles[i] > max->doubles[i])
            doubles[i] -= max->doubles[i] - min->doubles[i];
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Matrix4d::yacOperatorWrap: invalid object(s)");
   }
}

void YAC_VCALL _Matrix4d::yacOperator(sSI _cmd,YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix4d))
   {
      YAC_CAST_ARG(_Matrix4d, o, _o);
      switch(_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI( Ddblequal(doubles[0], o->doubles[0]) &&
                      Ddblequal(doubles[1], o->doubles[1]) &&
                      Ddblequal(doubles[2], o->doubles[2]) &&
                      Ddblequal(doubles[3], o->doubles[3]) &&
                      Ddblequal(doubles[4], o->doubles[4]) &&
                      Ddblequal(doubles[5], o->doubles[5]) &&
                      Ddblequal(doubles[6], o->doubles[6]) &&
                      Ddblequal(doubles[7], o->doubles[7]) &&
                      Ddblequal(doubles[8], o->doubles[8]) &&
                      Ddblequal(doubles[9], o->doubles[9]) &&
                      Ddblequal(doubles[10], o->doubles[10]) &&
                      Ddblequal(doubles[11], o->doubles[11]) &&
                      Ddblequal(doubles[12], o->doubles[12]) &&
                      Ddblequal(doubles[13], o->doubles[13]) &&
                      Ddblequal(doubles[14], o->doubles[14]) &&
                      Ddblequal(doubles[15], o->doubles[15])
                      );
            break;

         case YAC_OP_CNE:
            YAC_RETI( Ddblnotequal(doubles[0], o->doubles[0]) || 
                      Ddblnotequal(doubles[1], o->doubles[1]) ||
                      Ddblnotequal(doubles[2], o->doubles[2]) ||
                      Ddblnotequal(doubles[3], o->doubles[3]) ||
                      Ddblnotequal(doubles[4], o->doubles[4]) ||
                      Ddblnotequal(doubles[5], o->doubles[5]) ||
                      Ddblnotequal(doubles[6], o->doubles[6]) ||
                      Ddblnotequal(doubles[7], o->doubles[7]) ||
                      Ddblnotequal(doubles[8], o->doubles[8]) ||
                      Ddblnotequal(doubles[9], o->doubles[9]) ||
                      Ddblnotequal(doubles[10], o->doubles[10]) ||
                      Ddblnotequal(doubles[11], o->doubles[11]) ||
                      Ddblnotequal(doubles[12], o->doubles[12]) ||
                      Ddblnotequal(doubles[13], o->doubles[13]) ||
                      Ddblnotequal(doubles[14], o->doubles[14]) ||
                      Ddblnotequal(doubles[15], o->doubles[15])
                      );
            break;

         case YAC_OP_ASSIGN:
            doubles[ 0] = o->doubles[0];
            doubles[ 1] = o->doubles[1];
            doubles[ 2] = o->doubles[2];
            doubles[ 3] = o->doubles[3];
            doubles[ 4] = o->doubles[4];
            doubles[ 5] = o->doubles[5];
            doubles[ 6] = o->doubles[6];
            doubles[ 7] = o->doubles[7];
            doubles[ 8] = o->doubles[8];
            doubles[ 9] = o->doubles[9];
            doubles[10] = o->doubles[10];
            doubles[11] = o->doubles[11];
            doubles[12] = o->doubles[12];
            doubles[13] = o->doubles[13];
            doubles[14] = o->doubles[14];
            doubles[15] = o->doubles[15];
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
            doubles[ 0] = -doubles[0];
            doubles[ 1] = -doubles[1];
            doubles[ 2] = -doubles[2];
            doubles[ 3] = -doubles[3];
            doubles[ 4] = -doubles[4];
            doubles[ 5] = -doubles[5];
            doubles[ 6] = -doubles[6];
            doubles[ 7] = -doubles[7];
            doubles[ 8] = -doubles[8];
            doubles[ 9] = -doubles[9];
            doubles[10] = -doubles[10];
            doubles[11] = -doubles[11];
            doubles[12] = -doubles[12];
            doubles[13] = -doubles[13];
            doubles[14] = -doubles[14];
            doubles[15] = -doubles[15];
            break;

         case YAC_OP_INIT:
            break;

         default:
         {
            char buf[256];
            Dyac_snprintf(buf, 256, "tkmath::Matrix4d::yacOperator: unsupported cmd %d!\n", _cmd);
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
            if(o->num_elements < 16u)
            {
               sUI i;
               for(i = 0u; i < o->num_elements;i++)
                  doubles[i] = (sF64)o->elements[i];
               for(; i < 16u; i++)
                  doubles[i] = 0.0;  // (todo) diagonal = 1
            }
            else
            {
               doubles[ 0] = o->elements[ 0];
               doubles[ 1] = o->elements[ 1];
               doubles[ 2] = o->elements[ 2];
               doubles[ 3] = o->elements[ 3];
               doubles[ 4] = o->elements[ 4];
               doubles[ 5] = o->elements[ 5];
               doubles[ 6] = o->elements[ 6];
               doubles[ 7] = o->elements[ 7];
               doubles[ 8] = o->elements[ 8];
               doubles[ 9] = o->elements[ 9];
               doubles[10] = o->elements[10];
               doubles[11] = o->elements[11];
               doubles[12] = o->elements[12];
               doubles[13] = o->elements[13];
               doubles[14] = o->elements[14];
               doubles[15] = o->elements[15];
            }
            break;

         default:
            _Matrix4d t;
            t.class_ID=clid_Matrix4d;
            t.assignFloatArray(o);
            yacOperator(_cmd, &t, _r);

            //Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix4d::yacOperator Only Assign supported with FloatArray Type");
            break;
      }
   }
   else if(YAC_VALID(_o))
   {
      sF64 val;
      if(_o->yacScanF64(&val))
      {
         yacOperatorF64(_cmd, val, _r);
      }
      else
      {
         switch (_cmd) {
            case YAC_OP_ASSIGN:
               assignGeneric(NULL,_o);
               break;
            default:
               Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix4d::yacOperator Only Assign supported with Generic Object Type");
               break;
         }
      }
   }
   else
   {
      switch(_cmd)
      {
         case YAC_OP_NEG:
            doubles[ 0] = -doubles[ 0];
            doubles[ 1] = -doubles[ 1];
            doubles[ 2] = -doubles[ 2];
            doubles[ 3] = -doubles[ 3];
            doubles[ 4] = -doubles[ 4];
            doubles[ 5] = -doubles[ 5];
            doubles[ 6] = -doubles[ 6];
            doubles[ 7] = -doubles[ 7];
            doubles[ 8] = -doubles[ 8];
            doubles[ 9] = -doubles[ 9];
            doubles[10] = -doubles[10];
            doubles[11] = -doubles[11];
            doubles[12] = -doubles[12];
            doubles[13] = -doubles[13];
            doubles[14] = -doubles[14];
            doubles[15] = -doubles[15];
            break;

         default:
            YAC_Object::yacOperator(_cmd,_o,_r);
      }
   }
}

void _Matrix4d::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF32)val, _r);
}

void _Matrix4d::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void _Matrix4d::yacOperatorF64(sSI _cmd, sF64 val, YAC_Value *_r) {
   switch (_cmd) {
      case YAC_OP_MUL:
         muld(val);
         break;
      case YAC_OP_DIV:
         if (Ddblnonzero(val)) {
            muld(1.0/val);
         } else {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::yacOperatorF64 OP_DIV Cannot divide by zero");
         }
         break;
      default:
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Matrix4d::yacOperatorF64 Operator not supported %d",_cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
   }
}

void _Matrix4d::assignFloats(sF32 *_fv, sUI _ne) {
   if(_ne != 16) {
      Dyac_throw_def(WriteArrayOutOfBounds,"tkmath::Matrix4d Array not of size 16");
   } else {
      // Copy two elements
      doubles[0] = _fv[0];
      doubles[1] = _fv[1];
      doubles[2] = _fv[2];
      doubles[3] = _fv[3];
      doubles[4] = _fv[4];
      doubles[5] = _fv[5];
      doubles[6] = _fv[6];
      doubles[7] = _fv[7];
      doubles[8] = _fv[8];
      doubles[9] = _fv[9];
      doubles[10]= _fv[10];
      doubles[11]= _fv[11];
      doubles[12]= _fv[12];
      doubles[13]= _fv[13];
      doubles[14]= _fv[14];
      doubles[15]= _fv[15];
   }
}

void _Matrix4d::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Matrix4d::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
   sUI ne = _o->yacArrayGetNumElements();
   sUI nH = _o->yacArrayGetHeight();
   sUI nW = _o->yacArrayGetWidth();
   if (nH==0) {
      if (nW==16) { //exact number to fill the matrix - we con go on
         nH=4;
         nW=4;
      } else { //nah, throw an exception
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix4d Array not of size 16");
      }
   }
   if(ne > 0)
   {
      if(YAC_TYPE_FLOAT == _o->yacArrayGetElementType())
      {
         sF32 *fv = (sF32*) _o->yacArrayGetPointer();
         if(NULL != fv)
         {
            // Copy fast
            sUI i,j;
            for (i=0;(i<4)&&(i<nH);i++) {
               for (j=0;(j<4)&&(j<nW);j++) {
                  doubles[(i*4)+j]=(sF64)fv[(i*nW)+j];
               }
               for (;j<4;j++) {
                  if (i!=j) doubles[(i*4)+j]=0.0;
                  else doubles[(i*4)+j]=1.0;
               }
            }
            for (;(i<4);i++) {
               for (j=0;j<4;j++) {
                  if (i!=j) doubles[(i*4)+j]=0.0;
                  else doubles[(i*4)+j]=1.0;
               }
            }

            return;
         }
         else
         {
            // Retrieve individual float elements and assign
            YAC_Value v;
            sUI i,j;
            for (i=0;(i<4)&&(i<nH);i++) {
               for (j=0;(j<4)&&(j<nW);j++) {
                  _o->yacArrayGet(_ctx, (i*nW)+j, &v);
                  doubles[(i*4)+j]=(sF64)v.value.float_val;
               }
               for (;j<4;j++) {
                  if (i!=j) doubles[(i*4)+j]=0.0;
                  else doubles[(i*4)+j]=1.0;
               }
            }
            for (;(i<4);i++) {
               for (j=0;j<4;j++) {
                  if (i!=j) doubles[(i*4)+j]=0.0;
                  else doubles[(i*4)+j]=1.0;
               }
            }
         }
      }
      else
      {
         // Retrieve individual elements, typecast to float and assign
         YAC_Value v;
         sUI i,j;
         for (i=0;(i<4)&&(i<nH);i++) {
            for (j=0;(j<4)&&(j<nW);j++) {
               _o->yacArrayGet(_ctx, (i*nW)+j, &v);
               v.typecast(YAC_TYPE_OBJECT);
	       if (!v.value.object_val->yacScanF64(&doubles[(i*4)+j])) doubles[(i*4)+j]=0.0;
               v.unset();
            }
            for (;j<4;j++) {
               if (i!=j) doubles[(i*4)+j]=0.0;
               else doubles[(i*4)+j]=1.0;
            }
         }
         for (;(i<4);i++) {
            for (j=0;j<4;j++) {
               if (i!=j) doubles[(i*4)+j]=0.0;
               else doubles[(i*4)+j]=1.0;
            }
         }
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      doubles[0] = doubles[1] = doubles[2]=doubles[3]=doubles[4]=doubles[5]=doubles[6]=doubles[7]=doubles[8]=doubles[9]=doubles[10]=doubles[11]=doubles[12]=doubles[13]=doubles[14]=doubles[15]=0.0;
   }
}

//direct element access
void _Matrix4d::_setA(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[0])) doubles[0]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setA Object not valid");
   }
}

void _Matrix4d::_setB(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[1])) doubles[1]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setB Object not valid");
   }
}

void _Matrix4d::_setC(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[2])) doubles[2]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setC Object not valid");
   }
}

void _Matrix4d::_setD(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[3])) doubles[3]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setD Object not valid");
   }
}

void _Matrix4d::_setE(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[4])) doubles[4]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setE Object not valid");
   }
}

void _Matrix4d::_setF(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[5])) doubles[5]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setF Object not valid");
   }
}

void _Matrix4d::_setG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[6])) doubles[6]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setG Object not valid");
   }
}

void _Matrix4d::_setH(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[7])) doubles[7]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setH Object not valid");
   }
}

void _Matrix4d::_setI(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[8])) doubles[8]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setI Object not valid");
   }
}

void _Matrix4d::_setJ(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[9])) doubles[9]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setJ Object not valid");
   }
}

void _Matrix4d::_setK(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[10])) doubles[10]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setK Object not valid");
   }
}

void _Matrix4d::_setL(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[11])) doubles[11]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setL Object not valid");
   }
}

void _Matrix4d::_setM(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[12])) doubles[12]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setM Object not valid");
   }
}

void _Matrix4d::_setN(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[13])) doubles[13]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setN Object not valid");
   }
}

void _Matrix4d::_setO(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[14])) doubles[14]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setO Object not valid");
   }
}

void _Matrix4d::_setP(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (!_r->yacScanF64(&doubles[15])) doubles[15]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::setP Object not valid");
   }
}


void _Matrix4d::_getA_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[0];
   _r->initObject(d,1);
}

void _Matrix4d::_getA_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[0]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getA Return object not valid");
   }
}

void _Matrix4d::_getB_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[1];
   _r->initObject(d,1);
}

void _Matrix4d::_getB_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[1]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getB Return object not valid");
   }
}
void _Matrix4d::_getC_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[2];
   _r->initObject(d,1);
}

void _Matrix4d::_getC_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[2]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getC Return object not valid");
   }
}
void _Matrix4d::_getD_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[3];
   _r->initObject(d,1);
}

void _Matrix4d::_getD_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[3]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getD Return object not valid");
   }
}
void _Matrix4d::_getE_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[4];
   _r->initObject(d,1);
}

void _Matrix4d::_getE_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[4]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getE Return object not valid");
   }
}
void _Matrix4d::_getF_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[5];
   _r->initObject(d,1);
}

void _Matrix4d::_getF_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[5]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getF Return object not valid");
   }
}
void _Matrix4d::_getG_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[6];
   _r->initObject(d,1);
}

void _Matrix4d::_getG_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[6]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getG Return object not valid");
   }
}
void _Matrix4d::_getH_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[7];
   _r->initObject(d,1);
}

void _Matrix4d::_getH_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[7]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getH Return object not valid");
   }
}
void _Matrix4d::_getI_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[8];
   _r->initObject(d,1);
}

void _Matrix4d::_getI_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[8]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getI Return object not valid");
   }
}
void _Matrix4d::_getJ_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[9];
   _r->initObject(d,1);
}

void _Matrix4d::_getJ_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[9]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getJ Return object not valid");
   }
}
void _Matrix4d::_getK_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[10];
   _r->initObject(d,1);
}

void _Matrix4d::_getK_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[10]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getK Return object not valid");
   }
}
void _Matrix4d::_getL_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[11];
   _r->initObject(d,1);
}

void _Matrix4d::_getL_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[11]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getL Return object not valid");
   }
}
void _Matrix4d::_getM_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[12];
   _r->initObject(d,1);
}

void _Matrix4d::_getM_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[12]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getM Return object not valid");
   }
}
void _Matrix4d::_getN_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[13];
   _r->initObject(d,1);
}

void _Matrix4d::_getN_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[13]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getN Return object not valid");
   }
}
void _Matrix4d::_getO_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[14];
   _r->initObject(d,1);
}

void _Matrix4d::_getO_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[14]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getO Return object not valid");
   }
}
void _Matrix4d::_getP_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[15];
   _r->initObject(d,1);
}

void _Matrix4d::_getP_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[15]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getP Return object not valid");
   }
}

//generate string representation
sBool YAC_VCALL _Matrix4d::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(1024,0,0,0);
   s->printf("((%lg, %lg, %lg, %lg), (%lg, %lg, %lg, %lg), (%lg, %lg, %lg, %lg), (%lg, %lg, %lg, %lg))",
      doubles[0],doubles[1],doubles[2],doubles[3],
      doubles[4],doubles[5],doubles[6],doubles[7],
      doubles[8],doubles[9],doubles[10],doubles[11],
      doubles[12],doubles[13],doubles[14],doubles[15]);
   s->fixLength();
   return 1;
}

void _Matrix4d::_getString(YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   yacToString(s);
   YAC_RETS(s,1);
}

//add two matrices
void _Matrix4d::_add_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Matrix4d)) {
      _Matrix4d *o=(_Matrix4d*)_o;
      doubles[0]+=o->doubles[0];
      doubles[1]+=o->doubles[1];
      doubles[2]+=o->doubles[2];
      doubles[3]+=o->doubles[3];
      doubles[4]+=o->doubles[4];
      doubles[5]+=o->doubles[5];
      doubles[6]+=o->doubles[6];
      doubles[7]+=o->doubles[7];
      doubles[8]+=o->doubles[8];
      doubles[9]+=o->doubles[9];
      doubles[10]+=o->doubles[10];
      doubles[11]+=o->doubles[11];
      doubles[12]+=o->doubles[12];
      doubles[13]+=o->doubles[13];
      doubles[14]+=o->doubles[14];
      doubles[15]+=o->doubles[15];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::add_SELF Object not of type Matrix3f");
   }
}

void _Matrix4d::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Matrix4d)) {
      _Matrix4d *o=(_Matrix4d*)_o;
      _Matrix4d *r=YAC_NEW_POOLED(Matrix4d);
      r->doubles[0] =  doubles[0]  + o->doubles[0];
      r->doubles[1] =  doubles[1]  + o->doubles[1];
      r->doubles[2] =  doubles[2]  + o->doubles[2];
      r->doubles[3] =  doubles[3]  + o->doubles[3];
      r->doubles[4] =  doubles[4]  + o->doubles[4];
      r->doubles[5] =  doubles[5]  + o->doubles[5];
      r->doubles[6] =  doubles[6]  + o->doubles[6];
      r->doubles[7] =  doubles[7]  + o->doubles[7];
      r->doubles[8] =  doubles[8]  + o->doubles[8];
      r->doubles[9] =  doubles[9]  + o->doubles[9];
      r->doubles[10] = doubles[10] + o->doubles[10];
      r->doubles[11] = doubles[11] + o->doubles[11];
      r->doubles[12] = doubles[12] + o->doubles[12];
      r->doubles[13] = doubles[13] + o->doubles[13];
      r->doubles[14] = doubles[14] + o->doubles[14];
      r->doubles[15] = doubles[15] + o->doubles[15];
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::add_VAL Object not of type Matrix3f");
   }
}

void _Matrix4d::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Matrix4d)&&YAC_CHK(_r, clid_Matrix4d)) {
      _Matrix4d *o=(_Matrix4d*)_o;
      _Matrix4d *r=(_Matrix4d*)_r;
      r->doubles[0] =  doubles[0]  + o->doubles[0];
      r->doubles[1] =  doubles[1]  + o->doubles[1];
      r->doubles[2] =  doubles[2]  + o->doubles[2];
      r->doubles[3] =  doubles[3]  + o->doubles[3];
      r->doubles[4] =  doubles[4]  + o->doubles[4];
      r->doubles[5] =  doubles[5]  + o->doubles[5];
      r->doubles[6] =  doubles[6]  + o->doubles[6];
      r->doubles[7] =  doubles[7]  + o->doubles[7];
      r->doubles[8] =  doubles[8]  + o->doubles[8];
      r->doubles[9] =  doubles[9]  + o->doubles[9];
      r->doubles[10] = doubles[10] + o->doubles[10];
      r->doubles[11] = doubles[11] + o->doubles[11];
      r->doubles[12] = doubles[12] + o->doubles[12];
      r->doubles[13] = doubles[13] + o->doubles[13];
      r->doubles[14] = doubles[14] + o->doubles[14];
      r->doubles[15] = doubles[15] + o->doubles[15];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::add_ARG Object not of type Matrix3f");
   }
}

//sub two matrices
void _Matrix4d::_sub_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Matrix4d)) {
      _Matrix4d *o=(_Matrix4d*)_o;
      doubles[0]-=o->doubles[0];
      doubles[1]-=o->doubles[1];
      doubles[2]-=o->doubles[2];
      doubles[3]-=o->doubles[3];
      doubles[4]-=o->doubles[4];
      doubles[5]-=o->doubles[5];
      doubles[6]-=o->doubles[6];
      doubles[7]-=o->doubles[7];
      doubles[8]-=o->doubles[8];
      doubles[9]-=o->doubles[9];
      doubles[10]-=o->doubles[10];
      doubles[11]-=o->doubles[11];
      doubles[12]-=o->doubles[12];
      doubles[13]-=o->doubles[13];
      doubles[14]-=o->doubles[14];
      doubles[15]-=o->doubles[15];

   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::sub_SELF Object not of type Matrix4d");
   }
}

void _Matrix4d::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Matrix4d)) {
      _Matrix4d *o=(_Matrix4d*)_o;
      _Matrix4d *r=YAC_NEW_POOLED(Matrix4d);
      r->doubles[0]  = doubles[0]  - o->doubles[0];
      r->doubles[1]  = doubles[1]  - o->doubles[1];
      r->doubles[2]  = doubles[2]  - o->doubles[2];
      r->doubles[3]  = doubles[3]  - o->doubles[3];
      r->doubles[4]  = doubles[4]  - o->doubles[4];
      r->doubles[5]  = doubles[5]  - o->doubles[5];
      r->doubles[6]  = doubles[6]  - o->doubles[6];
      r->doubles[7]  = doubles[7]  - o->doubles[7];
      r->doubles[8]  = doubles[8]  - o->doubles[8];
      r->doubles[9]  = doubles[9]  - o->doubles[9];
      r->doubles[10] = doubles[10] - o->doubles[10];
      r->doubles[11] = doubles[11] - o->doubles[11];
      r->doubles[12] = doubles[12] - o->doubles[12];
      r->doubles[13] = doubles[13] - o->doubles[13];
      r->doubles[14] = doubles[14] - o->doubles[14];
      r->doubles[15] = doubles[15] - o->doubles[15];
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::sub_VAL Object not of type Matrix4d");
   }
}

void _Matrix4d::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Matrix4d)&&YAC_CHK(_r, clid_Matrix4d)) {
      _Matrix4d *o=(_Matrix4d*)_o;
      _Matrix4d *r=(_Matrix4d*)_r;
      r->doubles[0]  = doubles[0]  - o->doubles[0];
      r->doubles[1]  = doubles[1]  - o->doubles[1];
      r->doubles[2]  = doubles[2]  - o->doubles[2];
      r->doubles[3]  = doubles[3]  - o->doubles[3];
      r->doubles[4]  = doubles[4]  - o->doubles[4];
      r->doubles[5]  = doubles[5]  - o->doubles[5];
      r->doubles[6]  = doubles[6]  - o->doubles[6];
      r->doubles[7]  = doubles[7]  - o->doubles[7];
      r->doubles[8]  = doubles[8]  - o->doubles[8];
      r->doubles[9]  = doubles[9]  - o->doubles[9];
      r->doubles[10] = doubles[10] - o->doubles[10];
      r->doubles[11] = doubles[11] - o->doubles[11];
      r->doubles[12] = doubles[12] - o->doubles[12];
      r->doubles[13] = doubles[13] - o->doubles[13];
      r->doubles[14] = doubles[14] - o->doubles[14];
      r->doubles[15] = doubles[15] - o->doubles[15];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::sub_ARG Object not of type Matrix4d");
   }
}

void _Matrix4d::muld(const sF64 s) {
   for(sUI i = 0u; i < 16u; i++)
      doubles[i] *= s;
}

void _Matrix4d::_muld_YAC_RSELF(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      muld(s);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::muld Object not valid");
   }
}

void _Matrix4d::_muld_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   if(YAC_VALID(_s))
   {
      _Matrix4d *r = YAC_NEW_POOLED(Matrix4d);
      _r->initObject(r, YAC_TRUE);
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      for(sUI i = 0u; i < 16u; i++)
         r->doubles[i] = doubles[i] * s;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::muld Object not valid");
   }
}

void _Matrix4d::_muld_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_CHK(_r,clid_Matrix4d))
   {
      if(YAC_VALID(_s))
      {
         YAC_CAST_ARG(_Matrix4d, r, _r);
         sF64 s;
         if(!_s->yacScanF64(&s))
            s = 0.0;
         for(sUI i = 0u; i < 16u; i++)
            r->doubles[i] = doubles[0] * s;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::muld Object not valid");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::muld_ARG Return object not of type Matrix4d");
   }
}

void _Matrix4d::_mul_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix4d))
   {
      YAC_CAST_ARG(_Matrix4d, o, _o);
      sF64 t[4][4];
      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            t[y][x] =
               TM4D(y,0) * OM4D(0,x) +
               TM4D(y,1) * OM4D(1,x) +
               TM4D(y,2) * OM4D(2,x) +
               TM4D(y,3) * OM4D(3,x) ;
         }
      }
      ::memcpy((void*)doubles, (const void*)t, sizeof(doubles));
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::mul_SELF Object not of type Matrix4d!\n");
   }
}

void _Matrix4d::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix4d))
   {
      YAC_CAST_ARG(_Matrix4d, o, _o);
      _Matrix4d *r = YAC_NEW_POOLED(Matrix4d);
      _r->initObject(r, YAC_TRUE);

      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            M4D(r,y,x) =
               TM4D(y,0) * OM4D(0,x) +
               TM4D(y,1) * OM4D(1,x) +
               TM4D(y,2) * OM4D(2,x) +
               TM4D(y,3) * OM4D(3,x) ;
         }
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::mul_RVAL: Object not of type Matrix4d!\n");
   }
}

void _Matrix4d::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Matrix4d))
   {
      YAC_CAST_ARG(_Matrix4d, o, _o);
      YAC_CAST_ARG(_Matrix4d, r, _r);

      if((void*)o != (void*)this)
      {
         for(sUI y = 0u; y < 4u; y++)
         {
            for(sUI x = 0u; x < 4u; x++)
            {
               M4D(r,y,x) =
                  TM4D(y,0) * OM4D(0,x) +
                  TM4D(y,1) * OM4D(1,x) +
                  TM4D(y,2) * OM4D(2,x) +
                  TM4D(y,3) * OM4D(3,x) ;
            }
         }
      }
      else
      {
         sF64 t[4][4];
         for(sUI y = 0u; y < 4u; y++)
         {
            for(sUI x = 0u; x < 4u; x++)
            {
               t[y][x] =
                  TM4D(y,0) * OM4D(0,x) +
                  TM4D(y,1) * OM4D(1,x) +
                  TM4D(y,2) * OM4D(2,x) +
                  TM4D(y,3) * OM4D(3,x) ;
            }
         }
         ::memcpy((void*)doubles, (const void*)t, sizeof(doubles));
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::mul_RARG: Object not of type Matrix4d!\n");
   }
}

void _Matrix4d::_mulRev_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix4d))
   {
      YAC_CAST_ARG(_Matrix4d, o, _o);
      sF64 t[4][4];
      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            t[y][x] =
               OM4D(y,0) * TM4D(0,x) +
               OM4D(y,1) * TM4D(1,x) +
               OM4D(y,2) * TM4D(2,x) +
               OM4D(y,3) * TM4D(3,x) ;
         }
      }
      ::memcpy((void*)doubles, (const void*)t, sizeof(doubles));
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::mulRev_RSELF Object not of type Matrix4d!\n");
   }
}

void _Matrix4d::_mulRev_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix4d))
   {
      YAC_CAST_ARG(_Matrix4d, o, _o);
      _Matrix4d *r = YAC_NEW_POOLED(Matrix4d);
      _r->initObject(r, YAC_TRUE);

      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            M4D(r,y,x) =
               OM4D(y,0) * TM4D(0,x) +
               OM4D(y,1) * TM4D(1,x) +
               OM4D(y,2) * TM4D(2,x) +
               OM4D(y,3) * TM4D(3,x) ;
         }
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::mulRev_RVAL: Object not of type Matrix4d!\n");
   }
}

void _Matrix4d::_mulRev_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix4d))
   {
      YAC_CAST_ARG(_Matrix4d, o, _o);
      YAC_CAST_ARG(_Matrix4d, r, _r);

      if((void*)o != (void*)this)
      {
         for(sUI y = 0u; y < 4u; y++)
         {
            for(sUI x = 0u; x < 4u; x++)
            {
               M4D(r,y,x) =
                  OM4D(y,0) * TM4D(0,x) +
                  OM4D(y,1) * TM4D(1,x) +
                  OM4D(y,2) * TM4D(2,x) +
                  OM4D(y,3) * TM4D(3,x) ;
            }
         }
      }
      else
      {
         sF64 t[4][4];
         for(sUI y = 0u; y < 4u; y++)
         {
            for(sUI x = 0u; x < 4u; x++)
            {
               t[y][x] =
                  OM4D(y,0) * TM4D(0,x) +
                  OM4D(y,1) * TM4D(1,x) +
                  OM4D(y,2) * TM4D(2,x) +
                  OM4D(y,3) * TM4D(3,x) ;
            }
         }
         ::memcpy((void*)doubles, (const void*)t, sizeof(doubles));
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::mulRev_RARG: Object not of type Matrix4d!\n");
   }
}

void _Matrix4d::_mulv_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o,clid_Vector4d))
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      _Vector4d *r = YAC_NEW_POOLED(Vector4d);
      _r->initObject(r, YAC_TRUE);

      r->doubles[0] = doubles[ 0]*o->doubles[0] + doubles[ 1]*o->doubles[1] + doubles[ 2]*o->doubles[2] + doubles[ 3]*o->doubles[3];
      r->doubles[1] = doubles[ 4]*o->doubles[0] + doubles[ 5]*o->doubles[1] + doubles[ 6]*o->doubles[2] + doubles[ 7]*o->doubles[3];
      r->doubles[2] = doubles[ 8]*o->doubles[0] + doubles[ 9]*o->doubles[1] + doubles[10]*o->doubles[2] + doubles[11]*o->doubles[3];
      r->doubles[3] = doubles[12]*o->doubles[0] + doubles[13]*o->doubles[1] + doubles[14]*o->doubles[2] + doubles[15]*o->doubles[3];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::mulv_RVAL: Object not of type Vector4d");
   }
}

void _Matrix4d::_mulv_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Vector4d) &&
      YAC_CHK(_r, clid_Vector4d)
      )
   {
      YAC_CAST_ARG(_Vector4d, o, _o);
      YAC_CAST_ARG(_Vector4d, r, _r);

      if((void*)o != (void*)r)
      {
         for(sUI y = 0u; y < 4u; y++)
         {
            r->doubles[y] =
               TM4D(y,0) * o->doubles[0] + 
               TM4D(y,1) * o->doubles[1] + 
               TM4D(y,2) * o->doubles[2] +
               TM4D(y,3) * o->doubles[3] ;
         }
      }
      else
      {
         sF64 t[4];
         for(sUI y = 0u; y < 4u; y++)
         {
            t[y] =
               TM4D(y,0) * o->doubles[0] + 
               TM4D(y,1) * o->doubles[1] + 
               TM4D(y,2) * o->doubles[2] +
               TM4D(y,3) * o->doubles[3] ;
         }

         r->doubles[0] = t[0];
         r->doubles[1] = t[1];
         r->doubles[2] = t[2];
         r->doubles[3] = t[3];
      }
   } 
   else 
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::mulv: Object not of type Vector4d");
   }
   // // if (YAC_CHK(_o,clid_Vector4d)&&(YAC_CHK(_r,clid_Vector4d))) {
   // //    _Vector4d *o=(_Vector4d*)_o;
   // //    _Vector4d *r=(_Vector4d*)_r;
   // //    r->doubles[0]=doubles[0]*o->doubles[0]+doubles[1]*o->doubles[1]+doubles[2]*o->doubles[2]+doubles[3]*o->doubles[3];
   // //    r->doubles[1]=doubles[4]*o->doubles[0]+doubles[5]*o->doubles[1]+doubles[6]*o->doubles[2]+doubles[7]*o->doubles[3];
   // //    r->doubles[2]=doubles[8]*o->doubles[0]+doubles[9]*o->doubles[1]+doubles[10]*o->doubles[2]+doubles[11]*o->doubles[3];
   // //    r->doubles[3]=doubles[12]*o->doubles[0]+doubles[13]*o->doubles[1]+doubles[14]*o->doubles[2]+doubles[15]*o->doubles[3];
   // // } else {
   // //    Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::mulv Object not of type Vector4d");
   // // }
}

//return "length" of matrix
sF64 _Matrix4d::getAbs(void) {
   return sqrt(doubles[0]*doubles[0]+doubles[1]*doubles[1]+doubles[2]*doubles[2]+doubles[3]*doubles[3]+
         doubles[4]*doubles[4]+doubles[5]*doubles[5]+doubles[6]*doubles[6]+doubles[7]*doubles[7]+
	 doubles[8]*doubles[8]+doubles[9]*doubles[9]+doubles[10]*doubles[10]+doubles[11]*doubles[11]+
	 doubles[12]*doubles[12]+doubles[13]*doubles[13]+doubles[14]*doubles[14]+doubles[15]*doubles[15]);
}

void _Matrix4d::_getAbs_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=getAbs();
   _r->initObject(d,1);
}

void _Matrix4d::_getAbs_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getAbs());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getAbs Return object not valid");
   }
}

//return squared "length" of matrix
sF64 _Matrix4d::getAbsSqr(void) {
   return (doubles[0]*doubles[0]+doubles[1]*doubles[1]+doubles[2]*doubles[2]+doubles[3]*doubles[3]+
         doubles[4]*doubles[4]+doubles[5]*doubles[5]+doubles[6]*doubles[6]+doubles[7]*doubles[7]+
	 doubles[8]*doubles[8]+doubles[9]*doubles[9]+doubles[10]*doubles[10]+doubles[11]*doubles[11]+
	 doubles[12]*doubles[12]+doubles[13]*doubles[13]+doubles[14]*doubles[14]+doubles[15]*doubles[15]);
}

void _Matrix4d::_getAbsSqr_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=getAbsSqr();
   _r->initObject(d,1);
}

void _Matrix4d::_getAbsSqr_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getAbsSqr());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::getAbsSqr Return object not valid");
   }
}

//scale matrix to "length" 1
void _Matrix4d::_unit_YAC_RSELF(void) {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      absval=1.0/absval;
      doubles[0]*=absval;
      doubles[1]*=absval;
      doubles[2]*=absval;
      doubles[3]*=absval;
      doubles[4]*=absval;
      doubles[5]*=absval;
      doubles[6]*=absval;
      doubles[7]*=absval;
      doubles[8]*=absval;
      doubles[9]*=absval;
      doubles[10]*=absval;
      doubles[11]*=absval;
      doubles[12]*=absval;
      doubles[13]*=absval;
      doubles[14]*=absval;
      doubles[15]*=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::unit_SELF Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix4d::_unit_YAC_RVAL(YAC_Value *_r) {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      _Matrix4d *r=YAC_NEW_POOLED(Matrix4d);
      absval=1.0/absval;
      r->doubles[0]=doubles[0]*absval;
      r->doubles[1]=doubles[1]*absval;
      r->doubles[2]=doubles[2]*absval;
      r->doubles[3]=doubles[3]*absval;
      r->doubles[4]=doubles[4]*absval;
      r->doubles[5]=doubles[5]*absval;
      r->doubles[6]=doubles[6]*absval;
      r->doubles[7]=doubles[7]*absval;
      r->doubles[8]=doubles[8]*absval;
      r->doubles[9]=doubles[9]*absval;
      r->doubles[10]=doubles[10]*absval;
      r->doubles[11]=doubles[11]*absval;
      r->doubles[12]=doubles[12]*absval;
      r->doubles[13]=doubles[13]*absval;
      r->doubles[14]=doubles[14]*absval;
      r->doubles[15]=doubles[15]*absval;
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::unit_VAL Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix4d::_unit_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Matrix4d)) {
      sF64 absval=getAbs();
      if (Ddblnonzero(absval)) {
         _Matrix4d *r=(_Matrix4d*)_r;
         absval=1.0/absval;
         r->doubles[0]=doubles[0]*absval;
         r->doubles[1]=doubles[1]*absval;
         r->doubles[2]=doubles[2]*absval;
         r->doubles[3]=doubles[3]*absval;
         r->doubles[4]=doubles[4]*absval;
         r->doubles[5]=doubles[5]*absval;
         r->doubles[6]=doubles[6]*absval;
         r->doubles[7]=doubles[7]*absval;
         r->doubles[8]=doubles[8]*absval;
         r->doubles[9]=doubles[9]*absval;
         r->doubles[10]=doubles[10]*absval;
         r->doubles[11]=doubles[11]*absval;
         r->doubles[12]=doubles[12]*absval;
         r->doubles[13]=doubles[13]*absval;
         r->doubles[14]=doubles[14]*absval;
         r->doubles[15]=doubles[15]*absval;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::unit_ARG Zero Matrix cannot be normalized!\n");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::unit_ARG Return object not of type Matrix4d");
   }
}

//sale matrix to "length" s
void _Matrix4d::unitScale(sF64 s) {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      absval=s/absval;
      doubles[0]*=absval;
      doubles[1]*=absval;
      doubles[2]*=absval;
      doubles[3]*=absval;
      doubles[4]*=absval;
      doubles[5]*=absval;
      doubles[6]*=absval;
      doubles[7]*=absval;
      doubles[8]*=absval;
      doubles[9]*=absval;
      doubles[10]*=absval;
      doubles[11]*=absval;
      doubles[12]*=absval;
      doubles[13]*=absval;
      doubles[14]*=absval;
      doubles[15]*=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::unitScale_SELF Zero Matrix cannot be normalized");
   }
}

void _Matrix4d::_unitScale_YAC_RSELF(YAC_Object *_s) {
   if (YAC_VALID(_s)) {
      sF64 s;
      if (!_s->yacScanF64(&s)) s=0.0;
      unitScale(s);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::unitScale Object not valid");
   }
}

void _Matrix4d::_unitScale_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      if (YAC_VALID(_s)) {
         sF64 s;
	 if (!_s->yacScanF64(&s)) s=0.0;
         absval=s/absval;
         _Matrix4d *r=YAC_NEW_POOLED(Matrix4d);
         r->doubles[0]=doubles[0]*absval;
         r->doubles[1]=doubles[1]*absval;
         r->doubles[2]=doubles[2]*absval;
         r->doubles[3]=doubles[3]*absval;
         r->doubles[4]=doubles[4]*absval;
         r->doubles[5]=doubles[5]*absval;
         r->doubles[6]=doubles[6]*absval;
         r->doubles[7]=doubles[7]*absval;
         r->doubles[8]=doubles[8]*absval;
         r->doubles[9]=doubles[9]*absval;
         r->doubles[10]=doubles[10]*absval;
         r->doubles[11]=doubles[11]*absval;
         r->doubles[12]=doubles[12]*absval;
         r->doubles[13]=doubles[13]*absval;
         r->doubles[14]=doubles[14]*absval;
         r->doubles[15]=doubles[15]*absval;
         _r->initObject(r,1);
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::unitScale Object not valid");
      }
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::unitScale_VAL Zero Matrix cannot be normalized");
   }
}

void _Matrix4d::_unitScale_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if (YAC_CHK(_r, clid_Matrix4d)) {
      sF64 absval=getAbs();
      if (Ddblnonzero(absval)) {
         if (YAC_VALID(_s)) {
	    sF64 s;
	    if (!_s->yacScanF64(&s)) s=0.0;
            _Matrix4d *r=(_Matrix4d*)_r;
            absval=s/absval;
            r->doubles[0]=doubles[0]*absval;
            r->doubles[1]=doubles[1]*absval;
            r->doubles[2]=doubles[2]*absval;
            r->doubles[3]=doubles[3]*absval;
            r->doubles[4]=doubles[4]*absval;
            r->doubles[5]=doubles[5]*absval;
            r->doubles[6]=doubles[6]*absval;
            r->doubles[7]=doubles[7]*absval;
            r->doubles[8]=doubles[8]*absval;
            r->doubles[9]=doubles[9]*absval;
            r->doubles[10]=doubles[10]*absval;
            r->doubles[11]=doubles[11]*absval;
            r->doubles[12]=doubles[12]*absval;
            r->doubles[13]=doubles[13]*absval;
            r->doubles[14]=doubles[14]*absval;
            r->doubles[15]=doubles[15]*absval;
	 } else {
	    Dyac_throw_def(TkMathInvalidValue, "tkmath::unitScale Object not valid");
	 }
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::unitScale_ARG Zero Matrix cannot be normalized");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::unitScale_ARG Return object not of type Matrix4d");
   }
}

//returns determinant of matrix
sF64 _Matrix4d::det(void) {
//calculate determinant with leibniz' formula
#define PDET(a,b,c,d) (doubles[0*4+a]*doubles[1*4+b]*doubles[2*4+c]*doubles[3*4+d])
   return (PDET(0,1,2,3)-PDET(0,1,3,2)-PDET(0,2,1,3)+PDET(0,2,3,1)+PDET(0,3,1,2)-PDET(0,3,2,1)
          -PDET(1,0,2,3)+PDET(1,0,3,2)+PDET(1,2,0,3)-PDET(1,2,3,0)-PDET(1,3,0,2)+PDET(1,3,2,0)
	  +PDET(2,0,1,3)-PDET(2,0,3,1)-PDET(2,1,0,3)+PDET(2,1,3,0)+PDET(2,3,0,1)-PDET(2,3,1,0)
	  -PDET(3,0,1,2)+PDET(3,0,2,1)+PDET(3,1,0,2)-PDET(3,1,2,0)-PDET(3,2,0,1)+PDET(3,2,1,0));
}

void _Matrix4d::_det_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=det();
   _r->initObject(d,1);
}

void _Matrix4d::_det_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(det());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::det Return object not valid");
   }
}

#ifdef INVERT_DET
//inverts matrix if possible, determinant version
void _Matrix4d::_invert_YAC_RSELF(void) {
#define DET3(_a,_b,_c,_d,_e,_f,_g,_h,_i) ((_a*_e*_i)+(_b*_f*_g)+(_c*_d*_h)-(_c*_e*_g)-(_a*_f*_h)-(_b*_d*_i))
#define A doubles[0]
#define B doubles[1]
#define C doubles[2]
#define D doubles[3]
#define E doubles[4]
#define F doubles[5]
#define G doubles[6]
#define H doubles[7]
#define I doubles[8]
#define J doubles[9]
#define K doubles[10]
#define L doubles[11]
#define M doubles[12]
#define N doubles[13]
#define O doubles[14]
#define P doubles[15]
   sF64 detval=det();
   if (Ddblnonzero(detval)) {
      sF64 a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;

      a=DET3(F,G,H,J,K,L,N,O,P)/detval;
      b=-DET3(B,C,D,J,K,L,N,O,P)/detval;
      c=DET3(B,C,D,F,G,H,N,O,P)/detval;
      d=-DET3(B,C,D,F,G,H,J,K,L)/detval;

      e=-DET3(E,G,H,I,K,L,M,O,P)/detval;
      f=DET3(A,C,D,I,K,L,M,O,P)/detval;
      g=-DET3(A,C,D,E,G,H,M,O,P)/detval;
      h=DET3(A,C,D,E,G,H,I,K,L)/detval;

      i=DET3(E,F,H,I,J,L,M,N,P)/detval;
      j=-DET3(A,B,D,I,J,L,M,N,P)/detval;
      k=DET3(A,B,D,E,F,H,M,N,P)/detval;
      l=-DET3(A,B,D,E,F,H,I,J,L)/detval;

      m=-DET3(E,F,G,I,J,K,M,N,O)/detval;
      n=DET3(A,B,C,I,J,K,M,N,O)/detval;
      o=-DET3(A,B,C,E,F,G,M,N,O)/detval;
      p=DET3(A,B,C,E,F,G,I,J,K)/detval;

      doubles[0]=a;
      doubles[1]=b;
      doubles[2]=c;
      doubles[3]=d;
      doubles[4]=e;
      doubles[5]=f;
      doubles[6]=g;
      doubles[7]=h;
      doubles[8]=i;
      doubles[9]=j;
      doubles[10]=k;
      doubles[11]=l;
      doubles[12]=m;
      doubles[13]=n;
      doubles[14]=o;
      doubles[15]=p;

   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert_SELF Invalid Matrix (det==0)");

   }
}

void _Matrix4d::_invert_YAC_RVAL(YAC_Value *_r) {
#define DET3(_a,_b,_c,_d,_e,_f,_g,_h,_i) ((_a*_e*_i)+(_b*_f*_g)+(_c*_d*_h)-(_c*_e*_g)-(_a*_f*_h)-(_b*_d*_i))
#define A doubles[0]
#define B doubles[1]
#define C doubles[2]
#define D doubles[3]
#define E doubles[4]
#define F doubles[5]
#define G doubles[6]
#define H doubles[7]
#define I doubles[8]
#define J doubles[9]
#define K doubles[10]
#define L doubles[11]
#define M doubles[12]
#define N doubles[13]
#define O doubles[14]
#define P doubles[15]
   sF64 detval=det();
   if (Ddblnonzero(detval)) {
      _Matrix4d *r=YAC_NEW_POOLED(Matrix4d);

      r->A=DET3(F,G,H,J,K,L,N,O,P)/detval;
      r->B=-DET3(B,C,D,J,K,L,N,O,P)/detval;
      r->C=DET3(B,C,D,F,G,H,N,O,P)/detval;
      r->D=-DET3(B,C,D,F,G,H,J,K,L)/detval;

      r->E=-DET3(E,G,H,I,K,L,M,O,P)/detval;
      r->F=DET3(A,C,D,I,K,L,M,O,P)/detval;
      r->G=-DET3(A,C,D,E,G,H,M,O,P)/detval;
      r->H=DET3(A,C,D,E,G,H,I,K,L)/detval;

      r->I=DET3(E,F,H,I,J,L,M,N,P)/detval;
      r->J=-DET3(A,B,D,I,J,L,M,N,P)/detval;
      r->K=DET3(A,B,D,E,F,H,M,N,P)/detval;
      r->L=-DET3(A,B,D,E,F,H,I,J,L)/detval;

      r->M=-DET3(E,F,G,I,J,K,M,N,O)/detval;
      r->N=DET3(A,B,C,I,J,K,M,N,O)/detval;
      r->O=-DET3(A,B,C,E,F,G,M,N,O)/detval;
      r->P=DET3(A,B,C,E,F,G,I,J,K)/detval;


      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert_VAL Invalid Matrix (det==0)");
   }
}

void _Matrix4d::_invert_YAC_RARG(YAC_Object *_r) {
#define DET3(_a,_b,_c,_d,_e,_f,_g,_h,_i) ((_a*_e*_i)+(_b*_f*_g)+(_c*_d*_h)-(_c*_e*_g)-(_a*_f*_h)-(_b*_d*_i))
#define A doubles[0]
#define B doubles[1]
#define C doubles[2]
#define D doubles[3]
#define E doubles[4]
#define F doubles[5]
#define G doubles[6]
#define H doubles[7]
#define I doubles[8]
#define J doubles[9]
#define K doubles[10]
#define L doubles[11]
#define M doubles[12]
#define N doubles[13]
#define O doubles[14]
#define P doubles[15]
   if (YAC_CHK(_r,clid_Matrix4d)) {
      sF64 detval=det();
      if (Ddblnonzero(detval)) {
         _Matrix4d *r=(_Matrix4d*)_r;
         r->A=DET3(F,G,H,J,K,L,N,O,P)/detval;
         r->B=-DET3(B,C,D,J,K,L,N,O,P)/detval;
         r->C=DET3(B,C,D,F,G,H,N,O,P)/detval;
         r->D=-DET3(B,C,D,F,G,H,J,K,L)/detval;

         r->E=-DET3(E,G,H,I,K,L,M,O,P)/detval;
         r->F=DET3(A,C,D,I,K,L,M,O,P)/detval;
         r->G=-DET3(A,C,D,E,G,H,M,O,P)/detval;
         r->H=DET3(A,C,D,E,G,H,I,K,L)/detval;

         r->I=DET3(E,F,H,I,J,L,M,N,P)/detval;
         r->J=-DET3(A,B,D,I,J,L,M,N,P)/detval;
         r->K=DET3(A,B,D,E,F,H,M,N,P)/detval;
         r->L=-DET3(A,B,D,E,F,H,I,J,L)/detval;

         r->M=-DET3(E,F,G,I,J,K,M,N,O)/detval;
         r->N=DET3(A,B,C,I,J,K,M,N,O)/detval;
         r->O=-DET3(A,B,C,E,F,G,M,N,O)/detval;
         r->P=DET3(A,B,C,E,F,G,I,J,K)/detval;

      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert_ARG Invalid Matrix (det==0)");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::invert_ARG Return object not of type Matrix4d");
   }
}

#else //INVERT_DET

//an alternative implementation of invert matrix
//this should theoretically be faster, as it uses less multiplications
//but in some compiler/os/architecture combinations the determinant version
//is faster
//
//maybe decide at compiletime?
void _Matrix4d::_invert_YAC_RSELF(void) {
   sF64 m0,m1,m2,m3,s;
   sF64 rd0[8],rd1[8],rd2[8],rd3[8],*r0=rd0, *r1=rd1, *r2=rd2, *r3=rd3, *tmp;

  r0[0] = doubles[0];
  r0[1] = doubles[0*4+1];
  r0[2] = doubles[0*4+2];
  r0[3] = doubles[0*4+3];
  r0[4] = 1.0;
  r0[5] = 0;
  r0[6] = 0;
  r0[7] = 0.0;

  r1[0] = doubles[(1)*4+(0)];
  r1[1] = doubles[(1)*4+(1)];
  r1[2] = doubles[(1)*4+(2)];
  r1[3] = doubles[(1)*4+(3)];
  r1[5] = 1.0;
  r1[4] = 0;
  r1[6] = 0;
  r1[7] = 0.0;

  r2[0] = doubles[(2)*4+(0)];
  r2[1] = doubles[(2)*4+(1)];
  r2[2] = doubles[(2)*4+(2)];
  r2[3] = doubles[(2)*4+(3)];
  r2[6] = 1.0;
  r2[4] = 0;
  r2[5] = 0;
  r2[7] = 0.0;

  r3[0] = doubles[(3)*4+(0)];
  r3[1] = doubles[(3)*4+(1)];
  r3[2] = doubles[(3)*4+(2)];
  r3[3] = doubles[(3)*4+(3)];
  r3[7] = 1.0;
  r3[4] = 0;
  r3[5] = 0;
  r3[6] = 0.0;

  /* Choose myPivot, or die. */
  if (fabs(r3[0])>fabs(r2[0])) {tmp=r3;r3=r2;r2=tmp;};
  if (fabs(r2[0])>fabs(r1[0])) {tmp=r2;r2=r1;r1=tmp;};
  if (fabs(r1[0])>fabs(r0[0])) {tmp=r1;r1=r0;r0=tmp;};
  if (Ddblzero(r0[0])) {
    //assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");

  }
  /* Eliminate first variable. */
  m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
  s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
  s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
  s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
  s = r0[4];
  if (Ddblnonzero(s)) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r0[5];
  if (Ddblnonzero(s)) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r0[6];
  if (Ddblnonzero(s)) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r0[7];
  if (Ddblnonzero(s)) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* Choose myPivot, or die. */
  if (fabs(r3[1])>fabs(r2[1])) {tmp=r3;r3=r2;r2=tmp;};
  if (fabs(r2[1])>fabs(r1[1])) {tmp=r2;r2=r1;r1=tmp;};
  if (Ddblzero(r1[1])) {
    //assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");
  }
  /* Eliminate second variable. */
  m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
  r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
  r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
  s = r1[4]; if (Ddblnonzero(s)) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r1[5]; if (Ddblnonzero(s)) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r1[6]; if (Ddblnonzero(s)) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r1[7]; if (Ddblnonzero(s)) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* Choose myPivot, or die. */
  if (fabs(r3[2])>fabs(r2[2])) {tmp=r3;r3=r2;r2=tmp;};
  if (Ddblzero(r2[2])) {
   // assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");
  }
  /* Eliminate third variable. */
  m3 = r3[2]/r2[2];
  r3[3] -= m3 * r2[3];
  r3[4] -= m3 * r2[4];
  r3[5] -= m3 * r2[5];
  r3[6] -= m3 * r2[6];
  r3[7] -= m3 * r2[7];
  /* Last check. */
  if (Ddblzero(r3[3])) {
    //assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");
  }

  s = 1.0/r3[3];              /* Now back substitute row 3. */
  r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

  m2 = r2[3];                 /* Now back substitute row 2. */
  s  = 1.0/r2[2];
  r2[4] = s * (r2[4] - r3[4] * m2);
  r2[5] = s * (r2[5] - r3[5] * m2);
  r2[6] = s * (r2[6] - r3[6] * m2);
  r2[7] = s * (r2[7] - r3[7] * m2);
  m1 = r1[3];
  r1[4] -= r3[4] * m1;
  r1[5] -= r3[5] * m1;
  r1[6] -= r3[6] * m1;
  r1[7] -= r3[7] * m1;
  m0 = r0[3];
  r0[4] -= r3[4] * m0;
  r0[5] -= r3[5] * m0;
  r0[6] -= r3[6] * m0;
  r0[7] -= r3[7] * m0;

  m1 = r1[2];                 /* Now back substitute row 1. */
  s  = 1.0/r1[1];
  r1[4] = s * (r1[4] - r2[4] * m1);
  r1[5] = s * (r1[5] - r2[5] * m1);
  r1[6] = s * (r1[6] - r2[6] * m1);
  r1[7] = s * (r1[7] - r2[7] * m1);
  m0 = r0[2];
  r0[4] -= r2[4] * m0;
  r0[5] -= r2[5] * m0;
  r0[6] -= r2[6] * m0;
  r0[7] -= r2[7] * m0;

  m0 = r0[1];                 /* Now back substitute row 0. */
  s  = 1.0/r0[0];
  r0[4] = s * (r0[4] - r1[4] * m0);
  r0[5] = s * (r0[5] - r1[5] * m0);
  r0[6] = s * (r0[6] - r1[6] * m0);
  r0[7] = s * (r0[7] - r1[7] * m0);

  doubles[(0)*4+(0)] = r0[4]; doubles[(0)*4+(1)] = r0[5];
  doubles[(0)*4+(2)] = r0[6]; doubles[(0)*4+(3)] = r0[7];
  doubles[(1)*4+(0)] = r1[4]; doubles[(1)*4+(1)] = r1[5];
  doubles[(1)*4+(2)] = r1[6]; doubles[(1)*4+(3)] = r1[7];
  doubles[(2)*4+(0)] = r2[4]; doubles[(2)*4+(1)] = r2[5];
  doubles[(2)*4+(2)] = r2[6]; doubles[(2)*4+(3)] = r2[7];
  doubles[(3)*4+(0)] = r3[4]; doubles[(3)*4+(1)] = r3[5];
  doubles[(3)*4+(2)] = r3[6]; doubles[(3)*4+(3)] = r3[7];

}

void _Matrix4d::_invert_YAC_RVAL(YAC_Value *_r) {
   sF64 m0,m1,m2,m3,s;
   sF64 rd0[8],rd1[8],rd2[8],rd3[8],*r0=rd0, *r1=rd1, *r2=rd2, *r3=rd3, *tmp;

  r0[0] = doubles[0];
  r0[1] = doubles[0*4+1];
  r0[2] = doubles[0*4+2];
  r0[3] = doubles[0*4+3];
  r0[4] = 1.0;
  r0[5] = 0;
  r0[6] = 0;
  r0[7] = 0.0;

  r1[0] = doubles[(1)*4+(0)];
  r1[1] = doubles[(1)*4+(1)];
  r1[2] = doubles[(1)*4+(2)];
  r1[3] = doubles[(1)*4+(3)];
  r1[5] = 1.0;
  r1[4] = 0;
  r1[6] = 0;
  r1[7] = 0.0;

  r2[0] = doubles[(2)*4+(0)];
  r2[1] = doubles[(2)*4+(1)];
  r2[2] = doubles[(2)*4+(2)];
  r2[3] = doubles[(2)*4+(3)];
  r2[6] = 1.0;
  r2[4] = 0;
  r2[5] = 0;
  r2[7] = 0.0;

  r3[0] = doubles[(3)*4+(0)];
  r3[1] = doubles[(3)*4+(1)];
  r3[2] = doubles[(3)*4+(2)];
  r3[3] = doubles[(3)*4+(3)];
  r3[7] = 1.0;
  r3[4] = 0;
  r3[5] = 0;
  r3[6] = 0.0;

  /* Choose myPivot, or die. */
  if (fabs(r3[0])>fabs(r2[0])) {tmp=r3;r3=r2;r2=tmp;};
  if (fabs(r2[0])>fabs(r1[0])) {tmp=r2;r2=r1;r1=tmp;};
  if (fabs(r1[0])>fabs(r0[0])) {tmp=r1;r1=r0;r0=tmp;};
  if (Ddblzero(r0[0])) {
    //assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");

  }
  /* Eliminate first variable. */
  m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
  s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
  s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
  s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
  s = r0[4];
  if (Ddblnonzero(s)) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r0[5];
  if (Ddblnonzero(s)) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r0[6];
  if (Ddblnonzero(s)) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r0[7];
  if (Ddblnonzero(s)) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* Choose myPivot, or die. */
  if (fabs(r3[1])>fabs(r2[1])) {tmp=r3;r3=r2;r2=tmp;};
  if (fabs(r2[1])>fabs(r1[1])) {tmp=r2;r2=r1;r1=tmp;};
  if (Ddblzero(r1[1])) {
    //assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");
  }
  /* Eliminate second variable. */
  m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
  r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
  r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
  s = r1[4]; if (Ddblnonzero(s)) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r1[5]; if (Ddblnonzero(s)) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r1[6]; if (Ddblnonzero(s)) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r1[7]; if (Ddblnonzero(s)) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* Choose myPivot, or die. */
  if (fabs(r3[2])>fabs(r2[2])) {tmp=r3;r3=r2;r2=tmp;};
  if (Ddblzero(r2[2])) {
   // assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");
  }
  /* Eliminate third variable. */
  m3 = r3[2]/r2[2];
  r3[3] -= m3 * r2[3];
  r3[4] -= m3 * r2[4];
  r3[5] -= m3 * r2[5];
  r3[6] -= m3 * r2[6];
  r3[7] -= m3 * r2[7];
  /* Last check. */
  if (Ddblzero(r3[3])) {
    //assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");
  }

  s = 1.0/r3[3];              /* Now back substitute row 3. */
  r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

  m2 = r2[3];                 /* Now back substitute row 2. */
  s  = 1.0/r2[2];
  r2[4] = s * (r2[4] - r3[4] * m2);
  r2[5] = s * (r2[5] - r3[5] * m2);
  r2[6] = s * (r2[6] - r3[6] * m2);
  r2[7] = s * (r2[7] - r3[7] * m2);
  m1 = r1[3];
  r1[4] -= r3[4] * m1;
  r1[5] -= r3[5] * m1;
  r1[6] -= r3[6] * m1;
  r1[7] -= r3[7] * m1;
  m0 = r0[3];
  r0[4] -= r3[4] * m0;
  r0[5] -= r3[5] * m0;
  r0[6] -= r3[6] * m0;
  r0[7] -= r3[7] * m0;

  m1 = r1[2];                 /* Now back substitute row 1. */
  s  = 1.0/r1[1];
  r1[4] = s * (r1[4] - r2[4] * m1);
  r1[5] = s * (r1[5] - r2[5] * m1);
  r1[6] = s * (r1[6] - r2[6] * m1);
  r1[7] = s * (r1[7] - r2[7] * m1);
  m0 = r0[2];
  r0[4] -= r2[4] * m0;
  r0[5] -= r2[5] * m0;
  r0[6] -= r2[6] * m0;
  r0[7] -= r2[7] * m0;

  m0 = r0[1];                 /* Now back substitute row 0. */
  s  = 1.0/r0[0];
  r0[4] = s * (r0[4] - r1[4] * m0);
  r0[5] = s * (r0[5] - r1[5] * m0);
  r0[6] = s * (r0[6] - r1[6] * m0);
  r0[7] = s * (r0[7] - r1[7] * m0);

  _Matrix4d *r=YAC_NEW_POOLED(Matrix4d);
  r->doubles[(0)*4+(0)] = r0[4]; r->doubles[(0)*4+(1)] = r0[5];
  r->doubles[(0)*4+(2)] = r0[6]; r->doubles[(0)*4+(3)] = r0[7];
  r->doubles[(1)*4+(0)] = r1[4]; r->doubles[(1)*4+(1)] = r1[5];
  r->doubles[(1)*4+(2)] = r1[6]; r->doubles[(1)*4+(3)] = r1[7];
  r->doubles[(2)*4+(0)] = r2[4]; r->doubles[(2)*4+(1)] = r2[5];
  r->doubles[(2)*4+(2)] = r2[6]; r->doubles[(2)*4+(3)] = r2[7];
  r->doubles[(3)*4+(0)] = r3[4]; r->doubles[(3)*4+(1)] = r3[5];
  r->doubles[(3)*4+(2)] = r3[6]; r->doubles[(3)*4+(3)] = r3[7];

  _r->initObject(r,1);

}

void _Matrix4d::_invert_YAC_RARG(YAC_Object *_r) {
   sF64 m0,m1,m2,m3,s;
   sF64 rd0[8],rd1[8],rd2[8],rd3[8],*r0=rd0, *r1=rd1, *r2=rd2, *r3=rd3, *tmp;

  r0[0] = doubles[0];
  r0[1] = doubles[0*4+1];
  r0[2] = doubles[0*4+2];
  r0[3] = doubles[0*4+3];
  r0[4] = 1.0;
  r0[5] = 0;
  r0[6] = 0;
  r0[7] = 0.0;

  r1[0] = doubles[(1)*4+(0)];
  r1[1] = doubles[(1)*4+(1)];
  r1[2] = doubles[(1)*4+(2)];
  r1[3] = doubles[(1)*4+(3)];
  r1[5] = 1.0;
  r1[4] = 0;
  r1[6] = 0;
  r1[7] = 0.0;

  r2[0] = doubles[(2)*4+(0)];
  r2[1] = doubles[(2)*4+(1)];
  r2[2] = doubles[(2)*4+(2)];
  r2[3] = doubles[(2)*4+(3)];
  r2[6] = 1.0;
  r2[4] = 0;
  r2[5] = 0;
  r2[7] = 0.0;

  r3[0] = doubles[(3)*4+(0)];
  r3[1] = doubles[(3)*4+(1)];
  r3[2] = doubles[(3)*4+(2)];
  r3[3] = doubles[(3)*4+(3)];
  r3[7] = 1.0;
  r3[4] = 0;
  r3[5] = 0;
  r3[6] = 0.0;

  /* Choose myPivot, or die. */
  if (fabs(r3[0])>fabs(r2[0])) {tmp=r3;r3=r2;r2=tmp;};
  if (fabs(r2[0])>fabs(r1[0])) {tmp=r2;r2=r1;r1=tmp;};
  if (fabs(r1[0])>fabs(r0[0])) {tmp=r1;r1=r0;r0=tmp;};
  if (Ddblzero(r0[0])) {
    //assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");

  }
  /* Eliminate first variable. */
  m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
  s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
  s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
  s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
  s = r0[4];
  if (Ddblnonzero(s)) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r0[5];
  if (Ddblnonzero(s)) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r0[6];
  if (Ddblnonzero(s)) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r0[7];
  if (Ddblnonzero(s)) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* Choose myPivot, or die. */
  if (fabs(r3[1])>fabs(r2[1])) {tmp=r3;r3=r2;r2=tmp;};
  if (fabs(r2[1])>fabs(r1[1])) {tmp=r2;r2=r1;r1=tmp;};
  if (Ddblzero(r1[1])) {
    //assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");
  }
  /* Eliminate second variable. */
  m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
  r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
  r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
  s = r1[4]; if (Ddblnonzero(s)) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r1[5]; if (Ddblnonzero(s)) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r1[6]; if (Ddblnonzero(s)) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r1[7]; if (Ddblnonzero(s)) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* Choose myPivot, or die. */
  if (fabs(r3[2])>fabs(r2[2])) {tmp=r3;r3=r2;r2=tmp;};
  if (Ddblzero(r2[2])) {
   // assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");
  }
  /* Eliminate third variable. */
  m3 = r3[2]/r2[2];
  r3[3] -= m3 * r2[3];
  r3[4] -= m3 * r2[4];
  r3[5] -= m3 * r2[5];
  r3[6] -= m3 * r2[6];
  r3[7] -= m3 * r2[7];
  /* Last check. */
  if (Ddblzero(r3[3])) {
    //assert(!"could not invert matrix");
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4d::invert2 Invalid Matrix (det==0)");
  }

  s = 1.0/r3[3];              /* Now back substitute row 3. */
  r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

  m2 = r2[3];                 /* Now back substitute row 2. */
  s  = 1.0/r2[2];
  r2[4] = s * (r2[4] - r3[4] * m2);
  r2[5] = s * (r2[5] - r3[5] * m2);
  r2[6] = s * (r2[6] - r3[6] * m2);
  r2[7] = s * (r2[7] - r3[7] * m2);
  m1 = r1[3];
  r1[4] -= r3[4] * m1;
  r1[5] -= r3[5] * m1;
  r1[6] -= r3[6] * m1;
  r1[7] -= r3[7] * m1;
  m0 = r0[3];
  r0[4] -= r3[4] * m0;
  r0[5] -= r3[5] * m0;
  r0[6] -= r3[6] * m0;
  r0[7] -= r3[7] * m0;

  m1 = r1[2];                 /* Now back substitute row 1. */
  s  = 1.0/r1[1];
  r1[4] = s * (r1[4] - r2[4] * m1);
  r1[5] = s * (r1[5] - r2[5] * m1);
  r1[6] = s * (r1[6] - r2[6] * m1);
  r1[7] = s * (r1[7] - r2[7] * m1);
  m0 = r0[2];
  r0[4] -= r2[4] * m0;
  r0[5] -= r2[5] * m0;
  r0[6] -= r2[6] * m0;
  r0[7] -= r2[7] * m0;

  m0 = r0[1];                 /* Now back substitute row 0. */
  s  = 1.0/r0[0];
  r0[4] = s * (r0[4] - r1[4] * m0);
  r0[5] = s * (r0[5] - r1[5] * m0);
  r0[6] = s * (r0[6] - r1[6] * m0);
  r0[7] = s * (r0[7] - r1[7] * m0);

  if (YAC_CHK(_r,clid_Matrix4d)) {
     _Matrix4d *r=(_Matrix4d*)_r;

     r->doubles[(0)*4+(0)] = r0[4]; r->doubles[(0)*4+(1)] = r0[5];
     r->doubles[(0)*4+(2)] = r0[6]; r->doubles[(0)*4+(3)] = r0[7];
     r->doubles[(1)*4+(0)] = r1[4]; r->doubles[(1)*4+(1)] = r1[5];
     r->doubles[(1)*4+(2)] = r1[6]; r->doubles[(1)*4+(3)] = r1[7];
     r->doubles[(2)*4+(0)] = r2[4]; r->doubles[(2)*4+(1)] = r2[5];
     r->doubles[(2)*4+(2)] = r2[6]; r->doubles[(2)*4+(3)] = r2[7];
     r->doubles[(3)*4+(0)] = r3[4]; r->doubles[(3)*4+(1)] = r3[5];
     r->doubles[(3)*4+(2)] = r3[6]; r->doubles[(3)*4+(3)] = r3[7];
  } else {
     Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::invert2_ARG Return object not of type Matrix4d");
  }

}
#endif

//transpose/mirrow matrix
void _Matrix4d::_transpose_YAC_RSELF(void) {
   sF64 a,b,c,d,e,f;
   a=doubles[1];
   b=doubles[2];
   c=doubles[3];
   d=doubles[6];
   e=doubles[7];
   f=doubles[11];
   doubles[1]=doubles[4];
   doubles[2]=doubles[8];
   doubles[3]=doubles[12];
   doubles[6]=doubles[9];
   doubles[7]=doubles[13];
   doubles[11]=doubles[14];
   doubles[4]=a;
   doubles[8]=b;
   doubles[12]=c;
   doubles[9]=d;
   doubles[13]=e;
   doubles[14]=f;
}

void _Matrix4d::_transpose_YAC_RVAL(YAC_Value *_r) {
   _Matrix4d *r=YAC_NEW_POOLED(Matrix4d);
   r->doubles[0]=doubles[0]; r->doubles[1]=doubles[4]; r->doubles[2]=doubles[8]; r->doubles[3]=doubles[12];
   r->doubles[4]=doubles[1]; r->doubles[5]=doubles[5]; r->doubles[6]=doubles[9]; r->doubles[7]=doubles[13];
   r->doubles[8]=doubles[2]; r->doubles[9]=doubles[6]; r->doubles[10]=doubles[10]; r->doubles[11]=doubles[14];
   r->doubles[12]=doubles[3]; r->doubles[13]=doubles[7]; r->doubles[14]=doubles[11]; r->doubles[15]=doubles[15];

   _r->initObject(r,1);
}

void _Matrix4d::_transpose_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Matrix4d)) {
      _Matrix4d *r=(_Matrix4d*)_r;
      r->doubles[0]=doubles[0]; r->doubles[1]=doubles[4]; r->doubles[2]=doubles[8]; r->doubles[3]=doubles[12];
      r->doubles[4]=doubles[1]; r->doubles[5]=doubles[5]; r->doubles[6]=doubles[9]; r->doubles[7]=doubles[13];
      r->doubles[8]=doubles[2]; r->doubles[9]=doubles[6]; r->doubles[10]=doubles[10]; r->doubles[11]=doubles[14];
      r->doubles[12]=doubles[3]; r->doubles[13]=doubles[7]; r->doubles[14]=doubles[11]; r->doubles[15]=doubles[15];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4d::transpose_ARG Return object not of type Matrix4d");
   }
}

void _Matrix4d::_setRow(sUI row, YAC_Object *_o) {
   _initRowV(row,_o);
}

void _Matrix4d::_getRow_YAC_RVAL(sUI row, YAC_Value *_r) {
   if (row>3) {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix4d::getRow_VAL Wrong Row Number! (%d>3)\n", row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
      return;
   }
   _Vector4d *r=YAC_NEW_POOLED(Vector4d);
   r->doubles[0]=doubles[(row*4)+0];
   r->doubles[1]=doubles[(row*4)+1];
   r->doubles[2]=doubles[(row*4)+2];
   r->doubles[3]=doubles[(row*4)+3];
   _r->initObject(r,1);
}

void _Matrix4d::_getRow_YAC_RARG(sUI row, YAC_Object *_r) {
   if (row>3) {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix4d::getRow_VAL Wrong Row Number! (%d>3)\n", row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
      return;
   }
   if (!YAC_CHK(_r,clid_Vector4d)) {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::getRow_VAL Return object not of type Vector4d");
      return;
   }
   _Vector4d *r=(_Vector4d*)_r;
   r->doubles[0]=doubles[(row*4)+0];
   r->doubles[1]=doubles[(row*4)+1];
   r->doubles[2]=doubles[(row*4)+2];
   r->doubles[3]=doubles[(row*4)+3];
}

void _Matrix4d::_setCol(sUI col, YAC_Object *_o) {
   _initColV(col,_o);
}

void _Matrix4d::_getCol_YAC_RVAL(sUI col, YAC_Value *_r) {
   if (col>3) {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix4d::getCol_VAL Wrong Row Number! (%d>3)\n", col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
      return;
   }
   _Vector4d *r=YAC_NEW_POOLED(Vector4d);
   r->doubles[0]=doubles[0+col];
   r->doubles[1]=doubles[4+col];
   r->doubles[2]=doubles[8+col];
   r->doubles[3]=doubles[12+col];
   _r->initObject(r,1);
}

void _Matrix4d::_getCol_YAC_RARG(sUI col, YAC_Object *_r) {
   if (col>3) {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix4d::getCol_ARG Wrong Row Number! (%d>3)\n", col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
      return;
   }
   if (!YAC_CHK(_r,clid_Vector4d)) {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::getCol_ARG Return object not of type Vector4d");
      return;
   }
   _Vector4d *r=(_Vector4d*)_r;
   r->doubles[0]=doubles[0+col];
   r->doubles[1]=doubles[4+col];
   r->doubles[2]=doubles[8+col];
   r->doubles[3]=doubles[12+col];
}

//initialisation methods
void _Matrix4d::_initf(sF32 a, sF32 b, sF32 c, sF32 d, sF32 e, sF32 f, sF32 g, sF32 h, sF32 i, sF32 j, sF32 k, sF32 l, sF32 m, sF32 n, sF32 o, sF32 p) {
   doubles[0]=a;
   doubles[1]=b;
   doubles[2]=c;
   doubles[3]=d;
   doubles[4]=e;
   doubles[5]=f;
   doubles[6]=g;
   doubles[7]=h;
   doubles[8]=i;
   doubles[9]=j;
   doubles[10]=k;
   doubles[11]=l;
   doubles[12]=m;
   doubles[13]=n;
   doubles[14]=o;
   doubles[15]=p;
}

void _Matrix4d::_init(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Object *d, YAC_Object *e, YAC_Object *f, YAC_Object *g, YAC_Object *h, YAC_Object *i, YAC_Object *j, YAC_Object *k, YAC_Object *l, YAC_Object *m, YAC_Object *n, YAC_Object *o, YAC_Object *p) {
   if (YAC_VALID(a)&&YAC_VALID(b)&&YAC_VALID(c)&&YAC_VALID(d)&&YAC_VALID(e)&&YAC_VALID(f)&&YAC_VALID(g)&&YAC_VALID(h)&&YAC_VALID(i)&&YAC_VALID(j)&&YAC_VALID(k)&&YAC_VALID(l)&&YAC_VALID(m)&&YAC_VALID(n)&&YAC_VALID(o)&&YAC_VALID(p)) {
      if (!a->yacScanF64(&doubles[0])) doubles[0]=0.0;
      if (!b->yacScanF64(&doubles[1])) doubles[1]=0.0;
      if (!c->yacScanF64(&doubles[2])) doubles[2]=0.0;
      if (!d->yacScanF64(&doubles[3])) doubles[3]=0.0;
      if (!e->yacScanF64(&doubles[4])) doubles[4]=0.0;
      if (!f->yacScanF64(&doubles[5])) doubles[5]=0.0;
      if (!g->yacScanF64(&doubles[6])) doubles[6]=0.0;
      if (!h->yacScanF64(&doubles[7])) doubles[7]=0.0;
      if (!i->yacScanF64(&doubles[8])) doubles[8]=0.0;
      if (!j->yacScanF64(&doubles[9])) doubles[9]=0.0;
      if (!k->yacScanF64(&doubles[10])) doubles[10]=0.0;
      if (!l->yacScanF64(&doubles[11])) doubles[11]=0.0;
      if (!m->yacScanF64(&doubles[12])) doubles[12]=0.0;
      if (!n->yacScanF64(&doubles[13])) doubles[13]=0.0;
      if (!o->yacScanF64(&doubles[14])) doubles[14]=0.0;
      if (!p->yacScanF64(&doubles[15])) doubles[15]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::init Objects not valid");
   }
}

void _Matrix4d::_initRow(sUI row, YAC_Object *p, YAC_Object *q, YAC_Object *r, YAC_Object *s) {
   if (row<4) {
      if (YAC_VALID(p)&&YAC_VALID(q)&&YAC_VALID(r)&&YAC_VALID(s)) {
         if (!p->yacScanF64(&doubles[row*4])) doubles[row*4]=0.0;
         if (!q->yacScanF64(&doubles[row*4+1])) doubles[row*4+1]=0.0;
         if (!r->yacScanF64(&doubles[row*4+2])) doubles[row*4+2]=0.0;
         if (!s->yacScanF64(&doubles[row*4+3])) doubles[row*4+3]=0.0;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::initRow Objects not valid");
      }
   } else {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix3f::initRow Wrong Row Number! (%d>3)", row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix4d::_initCol(sUI col, YAC_Object *p, YAC_Object *q, YAC_Object *r, YAC_Object *s) {
   if (col<4) {
      if (YAC_VALID(p)&&YAC_VALID(q)&&YAC_VALID(r)&&YAC_VALID(s)) {
         if (!p->yacScanF64(&doubles[col])) doubles[col]=0.0;
         if (!q->yacScanF64(&doubles[col+4])) doubles[col+4]=0.0;
         if (!r->yacScanF64(&doubles[col+8])) doubles[col+8]=0.0;
         if (!s->yacScanF64(&doubles[col+12])) doubles[col+12]=0.0;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::initCol Objects not valid");
      }
   } else {
      char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Matrix4d::initCol Wrong Col Number! (%d>3)\n", col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix4d::_initRowV(sUI row, YAC_Object *_o) {
   if (YAC_VALID(_o)) {
      _Vector4d t;
      t.class_ID=clid_Vector4d;
      t.assignGeneric(NULL,_o);
      if (row>3) {
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Matrix4d::initRowV Wrong Row Number! (%d>3)\n", row);
         Dyac_throw_def(WriteArrayOutOfBounds, buf);
      }
      doubles[(row*4)+0]=t.doubles[0];
      doubles[(row*4)+1]=t.doubles[1];
      doubles[(row*4)+2]=t.doubles[2];
      doubles[(row*4)+3]=t.doubles[3];
   } else {
      Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix4d::initRowV: Invalid Object Value");
   }
}

void _Matrix4d::_initColV(sUI col, YAC_Object *_o) {
   if (YAC_VALID(_o)) {
      _Vector4d t;
      t.class_ID=clid_Vector4d;
      t.assignGeneric(NULL,_o);
      if (col>3) {
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Matrix4d::initColV Wrong Col Number! (%d>3)\n", col);
         Dyac_throw_def(WriteArrayOutOfBounds, buf);
      }
      doubles[0+col]=t.doubles[0];
      doubles[4+col]=t.doubles[1];
      doubles[8+col]=t.doubles[2];
      doubles[12+col]=t.doubles[3];
   } else {
      Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix4d::initColV: Invalid Object Value");
   }
}

void _Matrix4d::_initEulerXYX(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerXYX4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_initEulerXZX(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerXZX4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_initEulerYXY(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerYXY4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_initEulerYZY(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerYZY4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_initEulerZXZ(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerZXZ4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_initEulerZYZ(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerZYZ4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_initEulerXYZ(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerXYZ4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_initEulerXZY(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerXZY4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_initEulerYXZ(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerYXZ4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_initEulerYZX(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerYZX4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_initEulerZXY(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerZXY4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_initEulerZYX(YAC_Object * a, YAC_Object * b, YAC_Object * c) {
   _BuildEulerZYX4d_YAC_RARG(a,b,c,this);
}

void _Matrix4d::_BuildPerspectiveMatrix4d_YAC_RVAL(YAC_Object *fov, YAC_Object *aspect, YAC_Object *zNear, YAC_Object *zFar, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildPerspectiveMatrix4d_YAC_RARG(fov, aspect, zNear, zFar, m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildPerspectiveMatrix4d_YAC_RARG(YAC_Object *_fov, YAC_Object *_aspectRatio, YAC_Object *_zNear, YAC_Object *_zFar, YAC_Object *_m) {
   if (YAC_CHK(_m,clid_Matrix4d)) {
      if (YAC_VALID(_fov)&&YAC_VALID(_aspectRatio)&&YAC_VALID(_zNear)&&YAC_VALID(_zFar)) {
      sF64 fov, aspectRatio, zNear, zFar;
      if (!_fov->yacScanF64(&fov)) fov=0.0;
      if (!_aspectRatio->yacScanF64(&aspectRatio)) aspectRatio=0.0;
      if (!_zNear->yacScanF64(&zNear)) zNear=0.0;
      if (!_zFar->yacScanF64(&zFar)) zFar=0.0;
      sF64 sine, cotangent, deltaZ;
      sF64 radians = (fov / 2.0) * ((sF64)sM_PI / 180.0);
      _Matrix4d *m=(_Matrix4d*)_m;

      deltaZ = zFar - zNear;
      sine = sin(radians);
      /* Should be non-zero to avoid division by zero. */
      //assert(deltaZ);
      //assert(sine);
      //assert(aspectRatio);
      cotangent = cos(radians) / sine;
      m->doubles[0] = cotangent / aspectRatio;
      m->doubles[1] = 0;
      m->doubles[2] = 0;
      m->doubles[3] = 0;

      m->doubles[4] = 0;
      m->doubles[5] = cotangent;
      m->doubles[6] = 0;
      m->doubles[7] = 0;

      m->doubles[8] = 0;
      m->doubles[9] = 0;
      m->doubles[10] = -(zFar+zNear)/deltaZ;
      m->doubles[11] = -1;

      m->doubles[12] = 0;
      m->doubles[13] = 0;
      m->doubles[14] = -2.0*zNear*zFar/deltaZ;
      m->doubles[15] = 0;

//      m->_initCol(0,cotangent / aspectRatio,0,0,0);
//      m->_initCol(1,0,cotangent,0,0);
//      m->_initCol(2,0,0,-(zFar+zNear)/deltaZ,-2*zNear*zFar/deltaZ);
//      m->_initCol(3,0,0,-1,0);
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildPerspectiveMatrix4d Objects not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildPerspectiveMatrix4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildLookAtMatrix4d_YAC_RVAL(YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildLookAtMatrix4d_YAC_RARG(_eye,_center,_up,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildLookAtMatrix4d_YAC_RARG(YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Object *_m) {
   if (YAC_CHK(_eye,clid_Vector3d)&&YAC_CHK(_center,clid_Vector3d)&&YAC_CHK(_up,clid_Vector3d)) {
      if (YAC_CHK(_m,clid_Matrix4d)) {
         _Matrix4d *m=(_Matrix4d*)_m;
         _Vector3d *eye=(_Vector3d*)_eye;
	 _Vector3d *center=(_Vector3d*)_center;
	 _Vector3d *up=(_Vector3d*)_up;
         YAC_NEW_STACK(Vector3d,x);
         YAC_NEW_STACK(Vector3d,y);
         YAC_NEW_STACK(Vector3d,z);
	 YAC_NEW_STACK(Vector3d,neye);
	 //z=eye-center
	 eye->_sub_YAC_RARG(center,&z);
	 //z.unit()
	 z._unit_YAC_RSELF();
	 //x=up x x
	 up->_cross_YAC_RARG(&z,&x);
	 //y=zxy
	 z._cross_YAC_RARG(&x,&y);
	 //x.unit()
	 x._unit_YAC_RSELF();
	 //y.unit()
	 y._unit_YAC_RSELF();
	 //neye=-eye
	 neye.doubles[0]=-eye->doubles[0];
	 neye.doubles[1]=-eye->doubles[1];
	 neye.doubles[2]=-eye->doubles[2];
	 //eye->_mulf_YAC_RARG(-1.0f,&neye);


	 m->doubles[0] = x.doubles[0];
	 m->doubles[1] = x.doubles[1];
	 m->doubles[2] = x.doubles[2];
	 m->doubles[3] = x.dot(&neye);

	 m->doubles[4] = y.doubles[0];
	 m->doubles[5] = y.doubles[1];
	 m->doubles[6] = y.doubles[2];
	 m->doubles[7] = y.dot(&neye);

	 m->doubles[8] = z.doubles[0];
	 m->doubles[9] = z.doubles[1];
	 m->doubles[10] = z.doubles[2];
	 m->doubles[11] = z.dot(&neye);

	 m->doubles[12] = 0;
	 m->doubles[13] = 0;
	 m->doubles[14] = 0;
	 m->doubles[15] = 1.0;

	 //m->_initRow(0,x._getX(),x._getY(),x._getZ(),x._dot(&neye));
	 //m->_initRow(1,y._getX(),y._getY(),y._getZ(),y._dot(&neye));
	 //m->_initRow(2,z._getX(),z._getY(),z._getZ(),z._dot(&neye));
	 //m->_initRow(3,0,0,0,1.0f);

      } else {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildLookAtMatrix4d Return object not of type Matrix4d");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildLookAtMatrix4d Objects not of type Vector3d"); 
   }
}

void _Matrix4d::_BuildRotateMatrix4d_YAC_RVAL(YAC_Object *_angle, YAC_Object *_dir, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildRotateMatrix4d_YAC_RARG(_angle,_dir,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildRotateMatrix4d_YAC_RARG(YAC_Object *_radians, YAC_Object *_dir, YAC_Object *_m) {
   if (YAC_CHK(_dir,clid_Vector3d)) {
      if (YAC_CHK(_m,clid_Matrix4d)) {
         if (YAC_VALID(_radians)) {
            sF64 radians;
       	    sF64 sine, cosine, ab, bc, ca, tx, ty, tz;
            _Vector3d *dir=(_Vector3d*)_dir;
            _Matrix4d *m=(_Matrix4d*)_m;
	    YAC_NEW_STACK(Vector3d,axis);
	    if (!_radians->yacScanF64(&radians)) radians=0.0;
  
            dir->_unit_YAC_RARG(&axis);
  
//         radians = angle * M_PI / 180.0;
            sine = sin(radians);
            cosine = cos(radians);
            ab = axis.doubles[0] * axis.doubles[1] * (1 - cosine);
            bc = axis.doubles[1] * axis.doubles[2] * (1 - cosine);
            ca = axis.doubles[2] * axis.doubles[0] * (1 - cosine);
            tx = axis.doubles[0] * axis.doubles[0];
            ty = axis.doubles[1] * axis.doubles[1];
            tz = axis.doubles[2] * axis.doubles[2];
            m->doubles[0]  = tx + cosine * (1 - tx);
            m->doubles[1]  = ab + axis.doubles[2] * sine;
            m->doubles[2]  = ca - axis.doubles[1] * sine;
            m->doubles[3]  = 0.0;
            m->doubles[4]  = ab - axis.doubles[2] * sine;
            m->doubles[5]  = ty + cosine * (1 - ty);
            m->doubles[6]  = bc + axis.doubles[0] * sine;
            m->doubles[7]  = 0.0;
            m->doubles[8]  = ca + axis.doubles[1] * sine;
            m->doubles[9]  = bc - axis.doubles[0] * sine;
            m->doubles[10] = tz + cosine * (1 - tz);
            m->doubles[11] = 0;
            m->doubles[12] = 0;
            m->doubles[13] = 0;
            m->doubles[14] = 0;
            m->doubles[15] = 1;
	 } else {
	    Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildRotateMatrix4d Object not valid");
	 }
      } else {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildRotateMatrix4d Return object not of type Matrix4d");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildRotateMatrix4d Object not of type Vector3d");
   }
}

void _Matrix4d::_BuildTranslateMatrix4d_YAC_RVAL(YAC_Object *_move, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildTranslateMatrix4d_YAC_RARG(_move,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildTranslateMatrix4d_YAC_RARG(YAC_Object *_move, YAC_Object *_m) {
   if (YAC_CHK(_move,clid_Vector3d)) {
      if (YAC_CHK(_m,clid_Matrix4d)) {
         _Matrix4d *m=(_Matrix4d*)_m;
	 _Vector3d *move=(_Vector3d*)_move;

	 m->doubles[0]=1;
	 m->doubles[1]=0;
	 m->doubles[2]=0;
	 m->doubles[3]=move->doubles[0];

	 m->doubles[4]=0;
	 m->doubles[5]=1;
	 m->doubles[6]=0;
	 m->doubles[7]=move->doubles[1];

	 m->doubles[8]=0;
	 m->doubles[9]=0;
	 m->doubles[10]=1;
	 m->doubles[11]=move->doubles[2];

	 m->doubles[12]=0;
	 m->doubles[13]=0;
	 m->doubles[14]=0;
	 m->doubles[15]=1;

/*	 m->_init(1,0,0,move->doubles[0],
	          0,1,0,move->doubles[1],
		  0,0,1,move->doubles[2],
		  0,0,0,1);*/
      } else {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildTranslateMatrix4d Return object not of type Matrix4d");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildTranslateMatrix4d Object not of type Vector3d");
   }
}

void _Matrix4d::_BuildEulerXZX4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerXZX4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerXZX4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      sF64 a,b,c;
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         if (!_a->yacScanF64(&a)) a=0.0;
         if (!_b->yacScanF64(&b)) b=0.0;
         if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c2;
         m->doubles[1]=-c1*s2;
         m->doubles[2]=s1*s2;
         m->doubles[3]=0;

         m->doubles[4]=c3*s2;
         m->doubles[5]=c1*c2*c3-s1*s3;
         m->doubles[6]=-c2*c3*s1-c1*s3;
         m->doubles[7]=0;

         m->doubles[8]=s2*s3;
         m->doubles[9]=c3*s1+c1*c2*s3;
         m->doubles[10]=c1*c3-c2*s1*s3;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerXZX4d Object not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerXZX4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildEulerXYX4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerXYX4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerXYX4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         sF64 a,b,c;
	 if (!_a->yacScanF64(&a)) a=0.0;
	 if (!_b->yacScanF64(&b)) b=0.0;
	 if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c2;
         m->doubles[1]=s1*s2;
         m->doubles[2]=c1*s2;
         m->doubles[3]=0;

         m->doubles[4]=s2*s3;
         m->doubles[5]=c1*c3-c2*s1*s3;
         m->doubles[6]=-c3*s1-c1*c2*s3;
         m->doubles[7]=0;

         m->doubles[8]=-c3*s2;
         m->doubles[9]=c2*c3*s1+c1*s3;
         m->doubles[10]=c1*c2*c3-s1*s3;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerXYX4d Object not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerXYX4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildEulerYXY4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerYXY4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerYXY4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         sF64 a,b,c;
	 if (!_a->yacScanF64(&a)) a=0.0;
	 if (!_b->yacScanF64(&b)) b=0.0;
	 if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c1*c3-c2*s1*s3;
         m->doubles[1]=s2*s3;
         m->doubles[2]=c3*s1+c1*c2*s3;
         m->doubles[3]=0;

         m->doubles[4]=s1*s2;
         m->doubles[5]=c2;
         m->doubles[6]=-c1*s2;
         m->doubles[7]=0;

         m->doubles[8]=-c2*c3*s1-c1*s3;
         m->doubles[9]=c3*s2;
         m->doubles[10]=c1*c2*c3-s1*s3;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerYXY4d Object not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerYXY4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildEulerYZY4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerYZY4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerYZY4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         sF64 a,b,c;
         if (!_a->yacScanF64(&a)) a=0.0;
         if (!_b->yacScanF64(&b)) b=0.0;
         if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c1*c2*c3-s1*s3;
         m->doubles[1]=-c3*s2;
         m->doubles[2]=c2*c3*s1+c1*s3;
         m->doubles[3]=0;

         m->doubles[4]=c1*s2;
         m->doubles[5]=c2;
         m->doubles[6]=s1*s2;
         m->doubles[7]=0;

         m->doubles[8]=-c3*s1-c1*c2*s3;
         m->doubles[9]=s2*s3;
         m->doubles[10]=c1*c3-c2*s1*s3;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerYZY4d Object not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerYZY4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildEulerZYZ4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerZYZ4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerZYZ4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         sF64 a,b,c;
         if (!_a->yacScanF64(&a)) a=0.0;
         if (!_b->yacScanF64(&b)) b=0.0;
         if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c1*c2*c3-s1*s3;
         m->doubles[1]=-c2*c3*s1-c1*s3;
         m->doubles[2]=c3*s2;
         m->doubles[3]=0;

         m->doubles[4]=c3*s1+c1*c2*s3;
         m->doubles[5]=c1*c3-c2*s1*s3;
         m->doubles[6]=s2*s3;
         m->doubles[7]=0;

         m->doubles[8]=-c1*s2;
         m->doubles[9]=s1*s2;
         m->doubles[10]=c2;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerZYZ4d Objects not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerZYZ4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildEulerZXZ4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerZXZ4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerZXZ4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         sF64 a,b,c;
	 if (!_a->yacScanF64(&a)) a=0.0;
	 if (!_b->yacScanF64(&b)) b=0.0;
	 if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c1*c3-c2*s1*s3;
         m->doubles[1]=-c3*s1-c1*c2*s3;
         m->doubles[2]=s2*s3;
         m->doubles[3]=0;

         m->doubles[4]=c2*c3*s1+c1*s3;
         m->doubles[5]=c1*c2*c3-s1*s3;
         m->doubles[6]=-c3*s2;
         m->doubles[7]=0;

         m->doubles[8]=s1*s2;
         m->doubles[9]=c1*s2;
         m->doubles[10]=c2;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerZXZ4d Objects not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerZXZ4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildEulerXZY4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerXZY4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerXZY4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         sF64 a,b,c;
	 if (!_a->yacScanF64(&a)) a=0.0;
	 if (!_b->yacScanF64(&b)) b=0.0;
	 if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c2*c3;
         m->doubles[1]=s1*s3-c1*c3*s2;
         m->doubles[2]=c3*s1*s2+c1*s3;
         m->doubles[3]=0;

         m->doubles[4]=s2;
         m->doubles[5]=c1*c2;
         m->doubles[6]=-c2*s1;
         m->doubles[7]=0;

         m->doubles[8]=-c2*s3;
         m->doubles[9]=c3*s1+c1*s2*s3;
         m->doubles[10]=c1*c3-s1*s2*s3;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerXZY4d Objects not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerXZY4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildEulerXYZ4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerXYZ4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerXYZ4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         sF64 a,b,c;
	 if (!_a->yacScanF64(&a)) a=0.0;
	 if (!_b->yacScanF64(&b)) b=0.0;
	 if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c2*c3;
         m->doubles[1]=c3*s1*s2-c1*s3;
         m->doubles[2]=c1*c3*s2+s1*s3;
         m->doubles[3]=0;

         m->doubles[4]=c2*s3;
         m->doubles[5]=c1*c3+s1*s2*s3;
         m->doubles[6]=c1*s2*s3-c3*s1;
         m->doubles[7]=0;

         m->doubles[8]=-s2;
         m->doubles[9]=c2*s1;
         m->doubles[10]=c1*c2;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerXYZ4d Objects not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerXYZ4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildEulerYXZ4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerYXZ4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerYXZ4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         sF64 a,b,c;
	 if (!_a->yacScanF64(&a)) a=0.0;
	 if (!_b->yacScanF64(&b)) b=0.0;
	 if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c1*c3-s1*s2*s3;
         m->doubles[1]=-c2*s3;
         m->doubles[2]=c3*s1+c1*s2*s3;
         m->doubles[3]=0;

         m->doubles[4]=c3*s1*s2+c1*s3;
         m->doubles[5]=c2*c3;
         m->doubles[6]=s1*s3-c1*c3*s2;
         m->doubles[7]=0;

         m->doubles[8]=-c2*s1;
         m->doubles[9]=s2;
         m->doubles[10]=c1*c2;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerYXZ4d Objects not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerYXZ4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildEulerYZX4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerYZX4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerYZX4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         sF64 a,b,c;
	 if (!_a->yacScanF64(&a)) a=0.0;
	 if (!_b->yacScanF64(&b)) b=0.0;
	 if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c1*c2;
         m->doubles[1]=-s2;
         m->doubles[2]=c2*s1;
         m->doubles[3]=0;

         m->doubles[4]=c1*c3*s2+s1*s3;
         m->doubles[5]=c2*c3;
         m->doubles[6]=c3*s1*s2-c1*s3;
         m->doubles[7]=0;

         m->doubles[8]=c1*s2*s3-c3*s1;
         m->doubles[9]=c2*s3;
         m->doubles[10]=c1*c3+s1*s2*s3;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerYZX4d Objects not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerYZX4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildEulerZYX4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerZYX4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerZYX4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         sF64 a,b,c;
	 if (!_a->yacScanF64(&a)) a=0.0;
	 if (!_b->yacScanF64(&b)) b=0.0;
	 if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c1*c2;
         m->doubles[1]=-c2*s1;
         m->doubles[2]=s2;
         m->doubles[3]=0;

         m->doubles[4]=c3*s1+c1*s2*s3;
         m->doubles[5]=c1*c3-s1*s2*s3;
         m->doubles[6]=-c2*s3;
         m->doubles[7]=0;

         m->doubles[8]=s1*s3-c1*c3*s2;
         m->doubles[9]=c3*s1*s2+c1*s3;
         m->doubles[10]=c2*c3;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerZYX4d Objects not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerZYX4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_BuildEulerZXY4d_YAC_RVAL(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _BuildEulerZXY4d_YAC_RARG(a,b,c,m);
   _r->initObject(m,1);
}

void _Matrix4d::_BuildEulerZXY4d_YAC_RARG(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_m) {
   if (YAC_CHK(_m, clid_Matrix4d)) {
      if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_c)) {
         sF64 a,b,c;
         if (!_a->yacScanF64(&a)) a=0.0;
         if (!_b->yacScanF64(&b)) b=0.0;
         if (!_c->yacScanF64(&c)) c=0.0;
         _Matrix4d *m=(_Matrix4d*)_m;
         sF64 s1=sin(a), c1=cos(a);
         sF64 s2=sin(b), c2=cos(b);
         sF64 s3=sin(c), c3=cos(c);

         m->doubles[0]=c1*c3+s1*s2*s3;
         m->doubles[1]=c1*s2*s3-c3*s1;
         m->doubles[2]=c2*s3;
         m->doubles[3]=0;

         m->doubles[4]=c2*s1;
         m->doubles[5]=c1*c2;
         m->doubles[6]=-s2;
         m->doubles[7]=0;

         m->doubles[8]=c3*s1*s2-c1*s3;
         m->doubles[9]=c1*c3*s2+s1*s3;
         m->doubles[10]=c2*c3;
         m->doubles[11]=0;

         m->doubles[12]=0;
         m->doubles[13]=0;
         m->doubles[14]=0;
         m->doubles[15]=1;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix4d::BuildEulerZXY4d Objects not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4d::BuildEulerZXY4d Return object not of type Matrix4d");
   }
}

void _Matrix4d::_initIdentity(void) {
   TM4D(0,0) = 1.0;  TM4D(0,1) = 0.0;  TM4D(0,2) = 0.0;  TM4D(0,3) = 0.0;
   TM4D(1,0) = 0.0;  TM4D(1,1) = 1.0;  TM4D(1,2) = 0.0;  TM4D(1,3) = 0.0;
   TM4D(2,0) = 0.0;  TM4D(2,1) = 0.0;  TM4D(2,2) = 1.0;  TM4D(2,3) = 0.0;
   TM4D(3,0) = 0.0;  TM4D(3,1) = 0.0;  TM4D(3,2) = 0.0;  TM4D(3,3) = 1.0;
}

void _Matrix4d::_New(YAC_Object *_o, YAC_Value *_r) {
  if(YAC_VALID(_o))
  {
     _Matrix4d *r = YAC_NEW_POOLED(Matrix4d);
     _r->initObject(r, YAC_TRUE);
     r->assignGeneric(NULL,_o);
  }
  else
  {
     Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix4d::New: invalid Object");
  }
}

void YAC_CALL _matrix4d(YAC_Object *_o, YAC_Value *_r) {
  if(YAC_VALID(_o))
  {
     _Matrix4d *r = YAC_NEW_POOLED(Matrix4d);
     r->assignGeneric(NULL, _o);
     _r->initObject(r, YAC_TRUE);
  }
  else
  {
     Dyac_throw_def(TkMathInvalidValue,"tkmath::matrix4d: invalid Object");
  }
}
