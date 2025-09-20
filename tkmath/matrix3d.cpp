/// matrix3d.cpp
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
#include "vector3d.h"
#include "matrix3d.h"

#include "ying_Math.h"

#define TM3D(r,c)  doubles[TM3(r,c)]
#define OM3D(r,c)  o->doubles[OM3(r,c)]
#define M3D(a,r,c) (a)->doubles[M3(a,r,c)]

_Matrix3d::_Matrix3d(void) {
   _initIdentity();
}

_Matrix3d::~_Matrix3d() {
}

sUI YAC_VCALL _Matrix3d::yacArrayGetNumElements(void) {
   return 9;
}

sUI YAC_VCALL _Matrix3d::yacArrayGetMaxElements(void) {
   return 9;
}

sUI YAC_VCALL _Matrix3d::yacArrayGetElementType(void) {
   return YAC_TYPE_OBJECT;
}

sUI YAC_VCALL _Matrix3d::yacArrayGetElementByteSize(void) {
   return sizeof(sF64);
}

void YAC_VCALL _Matrix3d::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 9u)
   {
      sF64 v = 0;
      switch (_val->type)
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
            if(YAC_VALID(_val->value.object_val))
            {
               if (!_val->value.object_val->yacScanF64(&v))
                  v = 0.0;
            }
            break;

         default:
            Dyac_throw(TypeMismatch, "tkmath::Matrix3d::yacArraySet: invalid type");
            break;
      }
      doubles[_idx] = v;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3d::yacArraySet; index out of bounds (%u > 8)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Matrix3d::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 9u)
   {
      YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
      _val->initObject(d, YAC_TRUE);
      d->value = doubles[_idx];
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3d::yacArrayGet: index out of bounds (%u > 8)", _idx);
      Dyac_throw(ReadArrayOutOfBounds, buf);
      _val->initFloat(0.0);
   }
}

void *YAC_VCALL _Matrix3d::yacArrayGetPointer(void) {
   return (void*)doubles;
}

sUI YAC_VCALL _Matrix3d::yacArrayGetWidth(void) {
   return 3u;
}

sUI YAC_VCALL _Matrix3d::yacArrayGetHeight(void) {
   return 3u;
}

sUI YAC_VCALL _Matrix3d::yacArrayGetStride(void) {
   return 3u * sizeof(sF64);
}

sSI YAC_VCALL _Matrix3d::yacTensorRank(void) {
   return YAC_TENSOR_RANK_MATRIX;
}

void YAC_VCALL _Matrix3d::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
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
}

sUI YAC_VCALL _Matrix3d::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
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
   return 1;
}

void YAC_VCALL _Matrix3d::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if (YAC_CHK(_min,clid_Matrix3d)&&YAC_CHK(_max,clid_Matrix3d)) {
      _Matrix3d *min=(_Matrix3d*)_min;
      _Matrix3d *max=(_Matrix3d*)_max;
      if (doubles[0]<min->doubles[0]) doubles[0]=min->doubles[0];
      else if (doubles[0]>max->doubles[0]) doubles[0]=max->doubles[0];
      if (doubles[1]<min->doubles[1]) doubles[1]=min->doubles[1];
      else if (doubles[1]>max->doubles[1]) doubles[1]=max->doubles[1];
      if (doubles[2]<min->doubles[2]) doubles[2]=min->doubles[2];
      else if (doubles[2]>max->doubles[2]) doubles[2]=max->doubles[2];
      if (doubles[3]<min->doubles[3]) doubles[3]=min->doubles[3];
      else if (doubles[3]>max->doubles[3]) doubles[3]=max->doubles[3];
      if (doubles[4]<min->doubles[4]) doubles[4]=min->doubles[4];
      else if (doubles[4]>max->doubles[4]) doubles[4]=max->doubles[4];
      if (doubles[5]<min->doubles[5]) doubles[5]=min->doubles[5];
      else if (doubles[5]>max->doubles[5]) doubles[5]=max->doubles[5];
      if (doubles[6]<min->doubles[6]) doubles[6]=min->doubles[6];
      else if (doubles[6]>max->doubles[6]) doubles[6]=max->doubles[6];
      if (doubles[7]<min->doubles[7]) doubles[7]=min->doubles[7];
      else if (doubles[7]>max->doubles[7]) doubles[7]=max->doubles[7];
      if (doubles[8]<min->doubles[8]) doubles[8]=min->doubles[8];
      else if (doubles[8]>max->doubles[8]) doubles[8]=max->doubles[8];
   } else Dyac_throw_def(InvalidPointer, "");
}

void YAC_VCALL _Matrix3d::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if (YAC_CHK(_min,clid_Matrix3d)&&YAC_CHK(_max,clid_Matrix3d)) {
      _Matrix3d *min=(_Matrix3d*)_min;
      _Matrix3d *max=(_Matrix3d*)_max;
      if (doubles[0]<min->doubles[0]) doubles[0]+=(max->doubles[0]-min->doubles[0]);
      else if (doubles[0]>max->doubles[0]) doubles[0]-=(max->doubles[0]-min->doubles[0]);
      if (doubles[1]<min->doubles[1]) doubles[1]+=(max->doubles[1]-min->doubles[1]);
      else if (doubles[1]>max->doubles[1]) doubles[1]-=(max->doubles[1]-min->doubles[1]);
      if (doubles[2]<min->doubles[2]) doubles[2]+=(max->doubles[2]-min->doubles[2]);
      else if (doubles[2]>max->doubles[2]) doubles[2]-=(max->doubles[2]-min->doubles[2]);
      if (doubles[3]<min->doubles[3]) doubles[3]+=(max->doubles[3]-min->doubles[3]);
      else if (doubles[3]>max->doubles[3]) doubles[3]-=(max->doubles[3]-min->doubles[3]);
      if (doubles[4]<min->doubles[4]) doubles[4]+=(max->doubles[4]-min->doubles[4]);
      else if (doubles[4]>max->doubles[4]) doubles[4]-=(max->doubles[4]-min->doubles[4]);
      if (doubles[5]<min->doubles[5]) doubles[5]+=(max->doubles[5]-min->doubles[5]);
      else if (doubles[5]>max->doubles[5]) doubles[5]-=(max->doubles[5]-min->doubles[5]);
      if (doubles[6]<min->doubles[6]) doubles[6]+=(max->doubles[6]-min->doubles[6]);
      else if (doubles[6]>max->doubles[6]) doubles[6]-=(max->doubles[6]-min->doubles[6]);
      if (doubles[7]<min->doubles[7]) doubles[7]+=(max->doubles[7]-min->doubles[7]);
      else if (doubles[7]>max->doubles[7]) doubles[7]-=(max->doubles[7]-min->doubles[7]);
      if (doubles[8]<min->doubles[8]) doubles[8]+=(max->doubles[8]-min->doubles[8]);
      else if (doubles[8]>max->doubles[8]) doubles[8]-=(max->doubles[8]-min->doubles[8]);
   } else Dyac_throw_def(InvalidPointer, "");
}

void YAC_VCALL _Matrix3d::yacOperator(sSI _cmd,YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
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
                      Ddblequal(doubles[8], o->doubles[8])
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
                      Ddblnotequal(doubles[8], o->doubles[8])
                      );
            break;

         case YAC_OP_ASSIGN:
            for(sUI i = 0u; i < 9u; i++)
               doubles[i] = o->doubles[i];
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
            for(sUI i = 0u; i < 9u; i++)
               doubles[i] = -doubles[i];
            break;

         case YAC_OP_INIT:
            break;

         default:
         {
            static char buf[256];
            Dyac_snprintf(buf, 256, "tkmath::Matrix3d::yacOperator: unsupported cmd %d!\n", _cmd);
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
                  doubles[i] = (sF64)o->elements[i];
               for(; i < 9u; i++)
                  doubles[i]=0.0;
            }
            else
            {
               for(sUI i = 0u; i < 9u; i++)
                  doubles[i] = (sF64)o->elements[i];
            }
            break;

         default:
            _Matrix3d t;
            t.class_ID=clid_Matrix3d;
            t.assignFloatArray(o);
            yacOperator(_cmd,&t,_r);
            //Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix3d::yacOperator Only Assign supported with FloatArray Type");
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
         switch(_cmd)
         {
            case YAC_OP_ASSIGN:
               assignGeneric(NULL,_o);
               break;

            default:
               Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix3d::yacOperator Only Assign supported with Generic Type");
               break;
         }
      }
   }
   else
   {
      switch(_cmd)
      {
         case YAC_OP_NEG:
            for(sUI i = 0u; i < 9u; i++)
               doubles[i] = -doubles[i];
            break;

         default:
            YAC_Object::yacOperator(_cmd,_o,_r);
            break;
      }
   }
}

void _Matrix3d::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void _Matrix3d::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void _Matrix3d::yacOperatorF64(sSI _cmd, sF64 val, YAC_Value *_r) {
   switch(_cmd)
   {
      case YAC_OP_MUL:
         muld(val);
         break;

      case YAC_OP_DIV:
         if(Dfltnonzero(val))
         {
            muld(1.0 / val);
         }
         else
         {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3d::yacOperatorF64 OP_DIV Cannot divide by zero");
         }
         break;

      default:
         char buf[80];
         Dyac_snprintf(buf, 80, "tkmath::Matrix3d::yacOperatorF64 Operator not supported %d",_cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
         break;
   }
}

void _Matrix3d::assignFloats(const sF32 *_fv, sUI _ne) {
   if(9u == _ne)
   {
      for(sUI i = 0u; i < 9u; i++)
         doubles[i] = (sF64)_fv[i];
   }
   else
   {
      Dyac_throw_def(WriteArrayOutOfBounds,"tkmath::Matrix3d Array not of size 9");
   }
}

void _Matrix3d::assignFloatArray(const YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Matrix3d::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
   sUI ne = _o->yacArrayGetNumElements();
   sUI nH = _o->yacArrayGetHeight();
   sUI nW = _o->yacArrayGetWidth();
   if (nH==0) {
      if (nW==9) { //exact number to fill the matrix - we con go on
         nH=3;
         nW=3;
      } else { //nah, throw an exception
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix3d Array not of size 9");
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
            for (i=0;(i<3)&&(i<nH);i++) {
               for (j=0;(j<3)&&(j<nW);j++) {
                  doubles[(i*3)+j]=(sF64)fv[(i*nW)+j];
               }
               for (;j<3;j++) {
                  if (i!=j) doubles[(i*3)+j]=0.0;
		  else doubles[(i*3)+j]=1.0;
               }
            }
            for (;(i<3);i++) {
               for (j=0;j<3;j++) {
                  if (i!=j) doubles[(i*3)+j]=0.0;
		  else doubles[(i*3)+j]=1.0;
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
                  doubles[(i*3)+j]=(sF64)v.value.float_val;
               }
               for (;j<3;j++) {
                  if (i!=j) doubles[(i*3)+j]=0.0;
		  else doubles[(i*3)+j]=1.0;
               }
            }
            for (;(i<3);i++) {
               for (j=0;j<3;j++) {
                  if (i!=j) doubles[(i*3)+j]=0.0;
		  else doubles[(i*3)+j]=1.0;
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
               v.typecast(YAC_TYPE_OBJECT);
	       if (!v.value.object_val->yacScanF64(&doubles[(i*3)+j])) doubles[(i*3)+j]=0.0;
               v.unset();
            }
            for (;j<3;j++) {
               if (i!=j) doubles[(i*3)+j]=0.0;
	       else doubles[(i*3)+j]=1.0;
            }
         }
         for (;(i<3);i++) {
            for (j=0;j<3;j++) {
               if (i!=j) doubles[(i*3)+j]=0.0;
	       else doubles[(i*3)+j]=1.0;
            }
         }
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      ::memset((void*)doubles, 0, sizeof(doubles));
   }
}

void _Matrix3d::_setA(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[0])) doubles[0]=0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::setA Object not valid");
   }
}

void _Matrix3d::_setB(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[1])) doubles[1]=0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::setB Object not valid");
   }
}

void _Matrix3d::_setC(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[2])) doubles[2]=0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::setC Object not valid");
   }
}

void _Matrix3d::_setD(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if (!x->yacScanF64(&doubles[3])) doubles[3]=0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::setD Object not valid");
   }
}

void _Matrix3d::_setE(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[4])) doubles[4]=0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::setE Object not valid");
   }
}

void _Matrix3d::_setF(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[5])) doubles[5]=0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::setF Object not valid");
   }
}

void _Matrix3d::_setG(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[6])) doubles[6]=0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::setG Object not valid");
   }
}

void _Matrix3d::_setH(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[7])) doubles[7]=0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::setH Object not valid");
   }
}

void _Matrix3d::_setI(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if (!x->yacScanF64(&doubles[8])) doubles[8]=0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::setI Object not valid");
   }
}


void _Matrix3d::_getA_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[0];
   _r->initObject(d,1);
}

void _Matrix3d::_getA_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[0]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::getA Return object not valid");
   }
}

void _Matrix3d::_getB_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[1];
   _r->initObject(d,1);
}

void _Matrix3d::_getB_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[1]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::getB Return object not valid");
   }
}

void _Matrix3d::_getC_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[2];
   _r->initObject(d,1);
}

void _Matrix3d::_getC_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[2]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::getC Return object not valid");
   }
}

void _Matrix3d::_getD_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[3];
   _r->initObject(d,1);
}

void _Matrix3d::_getD_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[3]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::getD Return object not valid");
   }
}
void _Matrix3d::_getE_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[4];
   _r->initObject(d,1);
}

void _Matrix3d::_getE_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[4]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::getE Return object not valid");
   }
}
void _Matrix3d::_getF_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[5];
   _r->initObject(d,1);
}

void _Matrix3d::_getF_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[5]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::getF Return object not valid");
   }
}

void _Matrix3d::_getG_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[6];
   _r->initObject(d,1);
}

void _Matrix3d::_getG_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[6]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::getG Return object not valid");
   }
}

void _Matrix3d::_getH_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[7];
   _r->initObject(d,1);
}

void _Matrix3d::_getH_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[7]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::getH Return object not valid");
   }
}

void _Matrix3d::_getI_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value = doubles[8];
   _r->initObject(d,1);
}

void _Matrix3d::_getI_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[8]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::getI Return object not valid");
   }
}

sBool YAC_VCALL _Matrix3d::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(1024,0,0,0);
   s->printf("((%lg, %lg, %lg), (%lg, %lg, %lg), (%lg, %lg, %lg))",
             doubles[0], doubles[1], doubles[2],
             doubles[3], doubles[4], doubles[5],
             doubles[6], doubles[7], doubles[8]
             );
   s->fixLength();
   return YAC_TRUE;
}

void _Matrix3d::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*)YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   yacToString(s);
   YAC_RETS(s, YAC_TRUE);
}

void _Matrix3d::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o,clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      for(sUI i = 0u; i < 9u; i++)
         doubles[i] += o->doubles[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::add_RSELF: Object not of type Matrix3d");
   }
}

void _Matrix3d::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o,clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      _Matrix3d *r = YAC_NEW_POOLED(Matrix3d);
      _r->initObject(r, YAC_TRUE);
      for(sUI i = 0u; i < 9u; i++)
         r->doubles[i] = doubles[i] + o->doubles[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::add_RVAL: Object not of type Matrix3d");
   }
}

void _Matrix3d::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Matrix3d) &&
      YAC_CHK(_r, clid_Matrix3d)
      )
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      YAC_CAST_ARG(_Matrix3d, r, _r);
      for(sUI i = 0u; i < 9u; i++)
         r->doubles[i] = doubles[i] + o->doubles[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::add_RARG: Object not of type Matrix3d");
   }
}

void _Matrix3d::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o,clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      for(sUI i = 0u; i < 9u; i++)
         doubles[i] -= o->doubles[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::sub_RSELF: Object not of type Matrix3d");
   }
}

void _Matrix3d::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o,clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      _Matrix3d *r = YAC_NEW_POOLED(Matrix3d);
      _r->initObject(r, YAC_TRUE);
      for(sUI i = 0u; i < 9u; i++)
         r->doubles[i] = doubles[i] - o->doubles[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::sub_RVAL: Object not of type Matrix3d");
   }
}

void _Matrix3d::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o,clid_Matrix3d)&&YAC_CHK(_r,clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      YAC_CAST_ARG(_Matrix3d, r, _r);
      for(sUI i = 0u; i < 9u; i++)
         r->doubles[i] = doubles[i] - o->doubles[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::sub_RARG: Object not of type Matrix3d");
   }
}

void _Matrix3d::muld(const sF64 s) {
   for(sUI i = 0u; i < 9u; i++)
      doubles[i] *= s;
}

void _Matrix3d::_muld_YAC_RSELF(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      sF64 s;
      if(!_s->yacScanF64(&s))
         s=0.0;
      muld(s);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::muld Object not valid");
   }
}

void _Matrix3d::_muld_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   if(YAC_VALID(_s))
   {
      _Matrix3d *r = YAC_NEW_POOLED(Matrix3d);
      _r->initObject(r, YAC_TRUE);
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      for(sUI i = 0u; i < 9u; i++)
         r->doubles[i] = doubles[i] * s;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::muld Object not valid");
   }
}

void _Matrix3d::_muld_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix3d))
   {
      if(YAC_VALID(_s))
      {
         YAC_CAST_ARG(_Matrix3d, r, _r);
         sF64 s;
         if(!_s->yacScanF64(&s))
            s = 0.0;
         for(sUI i = 0u; i < 9u; i++)
            r->doubles[i] = doubles[i] * s;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::muld Object not valid");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::mulf_ARG Return object not of type Matrix3d");
   }
}

void _Matrix3d::_mul_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      sF64 t[3][3];
      for(sUI y = 0u; y < 3u; y++)
      {
         for(sUI x = 0u; x < 3u; x++)
         {
            t[y][x] =
               TM3D(y,0) * OM3D(0,x) +
               TM3D(y,1) * OM3D(1,x) +
               TM3D(y,2) * OM3D(2,x) ;
         }
      }
      ::memcpy((void*)doubles, (const void*)t, sizeof(doubles));
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::mul_SELF Object not of type Matrix3d!\n");
   }
}

void _Matrix3d::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      _Matrix3d *r = YAC_NEW_POOLED(Matrix3d);
      _r->initObject(r, YAC_TRUE);

      for(sUI y = 0u; y < 3u; y++)
      {
         for(sUI x = 0u; x < 3u; x++)
         {
            M3D(r,y,x) =
               TM3D(y,0) * OM3D(0,x) +
               TM3D(y,1) * OM3D(1,x) +
               TM3D(y,2) * OM3D(2,x) ;
         }
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::mul_RVAL: Object not of type Matrix3d!\n");
   }
}

void _Matrix3d::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      YAC_CAST_ARG(_Matrix3d, r, _r);

      if((void*)o != (void*)this)
      {
         for(sUI y = 0u; y < 3u; y++)
         {
            for(sUI x = 0u; x < 3u; x++)
            {
               M3D(r,y,x) =
                  TM3D(y,0) * OM3D(0,x) +
                  TM3D(y,1) * OM3D(1,x) +
                  TM3D(y,2) * OM3D(2,x) ;
            }
         }
      }
      else
      {
         sF64 t[4][4];
         for(sUI y = 0u; y < 3u; y++)
         {
            for(sUI x = 0u; x < 3u; x++)
            {
               t[y][x] =
                  TM3D(y,0) * OM3D(0,x) +
                  TM3D(y,1) * OM3D(1,x) +
                  TM3D(y,2) * OM3D(2,x) ;
            }
         }
         ::memcpy((void*)doubles, (const void*)t, sizeof(doubles));
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::mul_RARG: Object not of type Matrix3d!\n");
   }
}

void _Matrix3d::_mulRev_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      sF64 t[3][3];
      for(sUI y = 0u; y < 3u; y++)
      {
         for(sUI x = 0u; x < 3u; x++)
         {
            t[y][x] =
               OM3D(y,0) * TM3D(0,x) +
               OM3D(y,1) * TM3D(1,x) +
               OM3D(y,2) * TM3D(2,x) ;
         }
      }
      ::memcpy((void*)doubles, (const void*)t, sizeof(doubles));
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::mulRev_RSELF Object not of type Matrix3d!\n");
   }
}

void _Matrix3d::_mulRev_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      _Matrix3d *r = YAC_NEW_POOLED(Matrix3d);
      _r->initObject(r, YAC_TRUE);

      for(sUI y = 0u; y < 3u; y++)
      {
         for(sUI x = 0u; x < 3u; x++)
         {
            M3D(r,y,x) =
               OM3D(y,0) * TM3D(0,x) +
               OM3D(y,1) * TM3D(1,x) +
               OM3D(y,2) * TM3D(2,x) ;
         }
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::mulRev_RVAL: Object not of type Matrix3d!\n");
   }
}

void _Matrix3d::_mulRev_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, o, _o);
      YAC_CAST_ARG(_Matrix3d, r, _r);

      if((void*)o != (void*)this)
      {
         for(sUI y = 0u; y < 3u; y++)
         {
            for(sUI x = 0u; x < 3u; x++)
            {
               M3D(r,y,x) =
                  OM3D(y,0) * TM3D(0,x) +
                  OM3D(y,1) * TM3D(1,x) +
                  OM3D(y,2) * TM3D(2,x) ;
            }
         }
      }
      else
      {
         sF64 t[3][3];
         for(sUI y = 0u; y < 3u; y++)
         {
            for(sUI x = 0u; x < 3u; x++)
            {
               t[y][x] =
                  OM3D(y,0) * TM3D(0,x) +
                  OM3D(y,1) * TM3D(1,x) +
                  OM3D(y,2) * TM3D(2,x) ;
            }
         }
         ::memcpy((void*)doubles, (const void*)t, sizeof(doubles));
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::mulRev_RARG: Object not of type Matrix3d!\n");
   }
}

void _Matrix3d::_mulv_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o,clid_Vector3d))
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      _Vector3d *r = YAC_NEW_POOLED(Vector3d);
      _r->initObject(r, YAC_TRUE);

      r->doubles[0] = doubles[0]*o->doubles[0] + doubles[1]*o->doubles[1] + doubles[2]*o->doubles[2];
      r->doubles[1] = doubles[3]*o->doubles[0] + doubles[4]*o->doubles[1] + doubles[5]*o->doubles[2];
      r->doubles[2] = doubles[6]*o->doubles[0] + doubles[7]*o->doubles[1] + doubles[8]*o->doubles[2];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::mulv Object not of type Vector3d");
   }
}

void _Matrix3d::_mulv_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Vector3d) &&
      YAC_CHK(_r, clid_Vector3d)
      )
   {
      YAC_CAST_ARG(_Vector3d, o, _o);
      YAC_CAST_ARG(_Vector3d, r, _r);

      if((void*)o != (void*)r)
      {
         r->doubles[0] = doubles[0]*o->doubles[0] + doubles[1]*o->doubles[1] + doubles[2]*o->doubles[2];
         r->doubles[1] = doubles[3]*o->doubles[0] + doubles[4]*o->doubles[1] + doubles[5]*o->doubles[2];
         r->doubles[2] = doubles[6]*o->doubles[0] + doubles[7]*o->doubles[1] + doubles[8]*o->doubles[2];
      }
      else
      {
         sF64 x = doubles[0]*o->doubles[0] + doubles[1]*o->doubles[1] + doubles[2]*o->doubles[2];
         sF64 y = doubles[3]*o->doubles[0] + doubles[4]*o->doubles[1] + doubles[5]*o->doubles[2];
         sF64 z = doubles[6]*o->doubles[0] + doubles[7]*o->doubles[1] + doubles[8]*o->doubles[2];

         r->doubles[0] = x;
         r->doubles[1] = y;
         r->doubles[2] = z;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::mulv Object not of type Vector3d");
   }
}

sF64 _Matrix3d::getAbs(void) {
   return
      sqrt(doubles[0]*doubles[0] + doubles[1]*doubles[1] + doubles[2]*doubles[2] +
           doubles[3]*doubles[3] + doubles[4]*doubles[4] + doubles[5]*doubles[5] +
           doubles[6]*doubles[6] + doubles[7]*doubles[7] + doubles[8]*doubles[8]
           );
}

void _Matrix3d::_getAbs_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = getAbs();
}

void _Matrix3d::_getAbs_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(getAbs());
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::getAbs Object not valid");
   }
}

sF64 _Matrix3d::getAbsSqr(void) {
   return
      doubles[0]*doubles[0] + doubles[1]*doubles[1] + doubles[2]*doubles[2] +
      doubles[3]*doubles[3] + doubles[4]*doubles[4] + doubles[5]*doubles[5] +
      doubles[6]*doubles[6] + doubles[7]*doubles[7] + doubles[8]*doubles[8] ;
}

void _Matrix3d::_getAbsSqr_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = getAbsSqr();
}

void _Matrix3d::_getAbsSqr_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(getAbsSqr());
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::getAbsSqr Object not valid");
   }
}

void _Matrix3d::_unit_YAC_RSELF(void) {
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      absval = 1.0 / absval;
      for(sUI i = 0u; i < 9u; i++)
         doubles[i] *= absval;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3d::unit_SELF Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix3d::_unit_YAC_RVAL(YAC_Value *_r) {
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      _Matrix3d *r = YAC_NEW_POOLED(Matrix3d);
      _r->initObject(r, YAC_TRUE);
      absval = 1.0 / absval;
      for(sUI i = 0u; i < 9u; i++)
         r->doubles[i] = doubles[i] * absval;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3d::unit_RVAL: Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix3d::_unit_YAC_RARG(YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix3d))
   {
      sF64 absval = getAbs();
      if(Ddblnonzero(absval))
      {
         YAC_CAST_ARG(_Matrix3d, r, _r);
         absval = 1.0 / absval;
         for(sUI i = 0u; i < 9u; i++)
            r->doubles[i] = doubles[i] * absval;
      }
      else
      {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3d::unit_RSELF: Zero Matrix cannot be normalized!\n");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::unit_RARG: Return object not of type Matrix3d");
   }
}

void _Matrix3d::unitScale(sF64 s) {
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      absval = s / absval;
      for(sUI i = 0u; i < 9u; i++)
         doubles[i] *= absval;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3d::unitScale: Zero Matrix cannot be normalized");
   }
}

void _Matrix3d::_unitScale_YAC_RSELF(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      unitScale(s);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::unitScale: Object not valid");
   }
}

void _Matrix3d::_unitScale_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      if(YAC_VALID(_s))
      {
         _Matrix3d *r = YAC_NEW_POOLED(Matrix3d);
         _r->initObject(r, YAC_TRUE);
         sF64 s;
         if(!_s->yacScanF64(&s))
            s = 0.0;
         absval = s / absval;
         for(sUI i = 0u; i < 9u; i++)
            r->doubles[i] = doubles[i] * absval;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::unitScale Object not valid");
      }
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3d::unitScale_VAL Zero Matrix cannot be normalized");
   }
}

void _Matrix3d::_unitScale_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix3d))
   {
      sF64 absval = getAbs();
      if(Ddblnonzero(absval))
      {
         if(YAC_VALID(_s))
         {
            YAC_CAST_ARG(_Matrix3d, r, _r);
            sF64 s;
            if(!_s->yacScanF64(&s))
               s = 0.0;
            absval = s / absval;
            for(sUI i = 0u; i < 9u; i++)
               r->doubles[i] = doubles[i] * absval;
         }
         else
         {
            Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::unitScale_RARG: Object not valid");
         }
      }
      else
      {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3d::unitScale_RARG: Zero Matrix cannot be normalized");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::unitScale_RARG: Return object not of type Matrix3d");
   }
}

sF64 _Matrix3d::det(void) {
   return
        doubles[0]*doubles[4]*doubles[8]
      + doubles[1]*doubles[5]*doubles[6]
      + doubles[2]*doubles[3]*doubles[7]
      - doubles[2]*doubles[4]*doubles[6]
      - doubles[0]*doubles[5]*doubles[7]
      - doubles[1]*doubles[3]*doubles[8];
}

void _Matrix3d::_det_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(d, YAC_TRUE);
   d->value = det();
}

void _Matrix3d::_det_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(det());
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::det: Return object not valid");
   }
}

void _Matrix3d::_invert_YAC_RSELF(void) {
   sF64 detval = det();
   if(Ddblnonzero(detval))
   {
      const sF64 a = (doubles[4]*doubles[8] - doubles[5]*doubles[7]) / detval;
      const sF64 b = (doubles[2]*doubles[7] - doubles[1]*doubles[8]) / detval;
      const sF64 c = (doubles[1]*doubles[5] - doubles[2]*doubles[4]) / detval;
      const sF64 d = (doubles[5]*doubles[6] - doubles[3]*doubles[8]) / detval;
      const sF64 e = (doubles[0]*doubles[8] - doubles[2]*doubles[6]) / detval;
      const sF64 f = (doubles[2]*doubles[3] - doubles[0]*doubles[5]) / detval;
      const sF64 g = (doubles[3]*doubles[7] - doubles[4]*doubles[6]) / detval;
      const sF64 h = (doubles[1]*doubles[6] - doubles[0]*doubles[7]) / detval;
      const sF64 i = (doubles[0]*doubles[4] - doubles[1]*doubles[3]) / detval;
      doubles[0] = a;
      doubles[1] = b;
      doubles[2] = c;
      doubles[3] = d;
      doubles[4] = e;
      doubles[5] = f;
      doubles[6] = g;
      doubles[7] = h;
      doubles[8] = i;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3d::invert_SELF Invalid Matrix (det==0)");
   }
}

void _Matrix3d::_invert_YAC_RVAL(YAC_Value *_r) {
   sF64 detval = det();
   if(Ddblnonzero(detval))
   {
      _Matrix3d *r = YAC_NEW_POOLED(Matrix3d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = (doubles[4]*doubles[8] - doubles[5]*doubles[7]) / detval;
      r->doubles[1] = (doubles[2]*doubles[7] - doubles[1]*doubles[8]) / detval;
      r->doubles[2] = (doubles[1]*doubles[5] - doubles[2]*doubles[4]) / detval;
      r->doubles[3] = (doubles[5]*doubles[6] - doubles[3]*doubles[8]) / detval;
      r->doubles[4] = (doubles[0]*doubles[8] - doubles[2]*doubles[6]) / detval;
      r->doubles[5] = (doubles[2]*doubles[3] - doubles[0]*doubles[5]) / detval;
      r->doubles[6] = (doubles[3]*doubles[7] - doubles[4]*doubles[6]) / detval;
      r->doubles[7] = (doubles[1]*doubles[6] - doubles[0]*doubles[7]) / detval;
      r->doubles[8] = (doubles[0]*doubles[4] - doubles[1]*doubles[3]) / detval;
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3d::invert_VAL Invalid Matrix (det==0)");
   }
}

void _Matrix3d::_invert_YAC_RARG(YAC_Object *_r) {
   if(YAC_CHK(_r,clid_Matrix3d))
   {
      sF64 detval = det();
      if(Ddblnonzero(detval))
      {
         YAC_CAST_ARG(_Matrix3d, r, _r);
         r->doubles[0] = (doubles[4]*doubles[8] - doubles[5]*doubles[7]) / detval;
         r->doubles[1] = (doubles[2]*doubles[7] - doubles[1]*doubles[8]) / detval;
         r->doubles[2] = (doubles[1]*doubles[5] - doubles[2]*doubles[4]) / detval;
         r->doubles[3] = (doubles[5]*doubles[6] - doubles[3]*doubles[8]) / detval;
         r->doubles[4] = (doubles[0]*doubles[8] - doubles[2]*doubles[6]) / detval;
         r->doubles[5] = (doubles[2]*doubles[3] - doubles[0]*doubles[5]) / detval;
         r->doubles[6] = (doubles[3]*doubles[7] - doubles[4]*doubles[6]) / detval;
         r->doubles[7] = (doubles[1]*doubles[6] - doubles[0]*doubles[7]) / detval;
         r->doubles[8] = (doubles[0]*doubles[4] - doubles[1]*doubles[3]) / detval;
      }
      else
      {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix3d::invert_ARG Invalid Matrix (det==0)");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::invert_ARG Return object not of type Matrix3d");
   }
}

void _Matrix3d::_transpose_YAC_RSELF(void) {
   sF64 a = doubles[1];
   sF64 b = doubles[2];
   sF64 c = doubles[5];
   doubles[1] = doubles[3];
   doubles[2] = doubles[6];
   doubles[5] = doubles[7];
   doubles[3] = a;
   doubles[6] = b;
   doubles[7] = c;
}

void _Matrix3d::_transpose_YAC_RVAL(YAC_Value *_r) {
   _Matrix3d *r = YAC_NEW_POOLED(Matrix3d);
   _r->initObject(r, YAC_TRUE);
   r->doubles[0] = doubles[0];  r->doubles[1] = doubles[3];  r->doubles[2] = doubles[6];
   r->doubles[3] = doubles[1];  r->doubles[4] = doubles[4];  r->doubles[5] = doubles[7];
   r->doubles[6] = doubles[2];  r->doubles[7] = doubles[5];  r->doubles[8] = doubles[8];
}

void _Matrix3d::_transpose_YAC_RARG(YAC_Object *_r) {
   if(YAC_CHK(_r,clid_Matrix3d))
   {
      YAC_CAST_ARG(_Matrix3d, r, _r);
      r->doubles[0] = doubles[0];  r->doubles[1] = doubles[3];  r->doubles[2] = doubles[6];
      r->doubles[3] = doubles[1];  r->doubles[4] = doubles[4];  r->doubles[5] = doubles[7];
      r->doubles[6] = doubles[2];  r->doubles[7] = doubles[5];  r->doubles[8] = doubles[8];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix3d::tranpose_ARG Return object not of type Matrix3d");
   }
}

void _Matrix3d::_setRow(sUI row, YAC_Object *_o) {
   _initRowV(row,_o);
}

void _Matrix3d::_getRow_YAC_RVAL(sUI row, YAC_Value *_r) {
   if(row < 3u)
   {
      _Vector3d *r = YAC_NEW_POOLED(Vector3d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[(row*3)+0];
      r->doubles[1] = doubles[(row*3)+1];
      r->doubles[2] = doubles[(row*3)+2];
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3d::getRow_VAL Wrong Row Number! (%u > 2)\n", row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3d::_getRow_YAC_RARG(sUI row, YAC_Object *_r) {
   if(row < 3u)
   {
      if(YAC_CHK(_r,clid_Vector3d))
      {
         YAC_CAST_ARG(_Vector3d, r, _r);
         r->doubles[0] = doubles[(row*3)+0];
         r->doubles[1] = doubles[(row*3)+1];
         r->doubles[2] = doubles[(row*3)+2];
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix3d::getRow_ARG Return object not of type Vector3d");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3d::getRow_RARG: invalid row (%u > 2)\n", row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3d::_setCol(sUI col, YAC_Object *_o) {
   _initColV(col,_o);
}

void _Matrix3d::_getCol_YAC_RVAL(sUI col, YAC_Value *_r) {
   if(col < 3u)
   {
      _Vector3d *r = YAC_NEW_POOLED(Vector3d);
      r->doubles[0] = doubles[0+col];
      r->doubles[1] = doubles[3+col];
      r->doubles[2] = doubles[6+col];
      _r->initObject(r, YAC_TRUE);
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3d::getCol_RVAL: invalid row (%u > 2)\n", col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3d::_getCol_YAC_RARG(sUI col, YAC_Object *_r) {
   if(col < 3u)
   {
      if(YAC_CHK(_r,clid_Vector3d))
      {
         YAC_CAST_ARG(_Vector3d, r, _r);
         r->doubles[0] = doubles[0+col];
         r->doubles[1] = doubles[3+col];
         r->doubles[2] = doubles[6+col];
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix3d::getCol_RARG: Return object not of type Vector3d");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3d::getCol_RARG: invalid row (%u > 2)\n", col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3d::_initf(sF32 a, sF32 b, sF32 c,
                       sF32 d, sF32 e, sF32 f,
                       sF32 g, sF32 h, sF32 i
                       ) {
   TM3D(0,0) = a;  TM3D(0,1) = b;  TM3D(0,2) = c;
   TM3D(1,0) = d;  TM3D(1,1) = e;  TM3D(1,2) = f;
   TM3D(2,0) = g;  TM3D(2,1) = h;  TM3D(2,2) = i;
}

void _Matrix3d::_init(YAC_Object * a, YAC_Object * b, YAC_Object * c,
                      YAC_Object * d, YAC_Object * e, YAC_Object * f,
                      YAC_Object * g, YAC_Object * h, YAC_Object * i
                      ) {
   if(YAC_VALID(a) &&
      YAC_VALID(b) &&
      YAC_VALID(c) &&
      YAC_VALID(d) &&
      YAC_VALID(e) &&
      YAC_VALID(f) &&
      YAC_VALID(g) &&
      YAC_VALID(h) &&
      YAC_VALID(i)
      )
   {
      if(!a->yacScanF64(&doubles[0])) doubles[0] = 0.0;
      if(!b->yacScanF64(&doubles[1])) doubles[1] = 0.0;
      if(!c->yacScanF64(&doubles[2])) doubles[2] = 0.0;
      if(!d->yacScanF64(&doubles[3])) doubles[3] = 0.0;
      if(!e->yacScanF64(&doubles[4])) doubles[4] = 0.0;
      if(!f->yacScanF64(&doubles[5])) doubles[5] = 0.0;
      if(!g->yacScanF64(&doubles[6])) doubles[6] = 0.0;
      if(!h->yacScanF64(&doubles[7])) doubles[7] = 0.0;
      if(!i->yacScanF64(&doubles[8])) doubles[8] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::init Objects not valid");
   }
}

void _Matrix3d::_initRow(sUI row, YAC_Object * x, YAC_Object * y, YAC_Object * z) {
   if(row < 3u)
   {
      if(YAC_VALID(x) && YAC_VALID(y) && YAC_VALID(z))
      {
         if(!x->yacScanF64(&doubles[row*3+0])) doubles[row*3+0] = 0.0;
         if(!y->yacScanF64(&doubles[row*3+1])) doubles[row*3+1] = 0.0;
         if(!z->yacScanF64(&doubles[row*3+2])) doubles[row*3+2] = 0.0;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::initRow: invalid object(s)");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3d::initRow: invalid row (%u > 2)", row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3d::_initCol(sUI col, YAC_Object * x, YAC_Object * y, YAC_Object * z) {
   if(col < 3u)
   {
      if(YAC_VALID(x) && YAC_VALID(y) && YAC_VALID(z))
      {
         if(!x->yacScanF64(&doubles[col+0])) doubles[col+0] = 0.0;
         if(!y->yacScanF64(&doubles[col+3])) doubles[col+3] = 0.0;
         if(!z->yacScanF64(&doubles[col+6])) doubles[col+6] = 0.0;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Matrix3d::initCol: invalid object(s)");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix3d::initCol: invalid column (%u > 2)\n", col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix3d::_initRowV(sUI row, YAC_Object *_o) {
   if(YAC_VALID(_o))
   {
      _Vector3d t; t.class_ID = clid_Vector3d;
      t.assignGeneric(NULL,_o);
      if(row < 3u)
      {
         doubles[(row*3)+0] = t.doubles[0];
         doubles[(row*3)+1] = t.doubles[1];
         doubles[(row*3)+2] = t.doubles[2];
      }
      else
      {
         char buf[80];
         Dyac_snprintf(buf, 80, "tkmath::Matrix3d::initRowV: invalid row (%u > 2)\n", row);
         Dyac_throw_def(WriteArrayOutOfBounds, buf);
      }
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix3d::initRowV: Invalid Object Value");
   }
}

void _Matrix3d::_initColV(sUI col, YAC_Object *_o) {
   if(YAC_VALID(_o))
   {
      _Vector3d t; t.class_ID=clid_Vector3d;
      t.assignGeneric(NULL,_o);
      if(col < 3u)
      {
         doubles[0+col] = t.doubles[0];
         doubles[3+col] = t.doubles[1];
         doubles[6+col] = t.doubles[2];
      }
      else
      {
         char buf[80];
         Dyac_snprintf(buf, 80, "tkmath::Matrix3d::initColV Wrong Col Number! (%d>2)\n", col);
         Dyac_throw_def(WriteArrayOutOfBounds, buf);
      }
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix3d::initColV: Invalid Object Value");
   }
}

void _Matrix3d::_initIdentity(void) {
   TM3D(0,0) = 1.0;  TM3D(0,1) = 0.0;  TM3D(0,2) = 0.0;
   TM3D(1,0) = 0.0;  TM3D(1,1) = 1.0;  TM3D(1,2) = 0.0;
   TM3D(2,0) = 0.0;  TM3D(2,1) = 0.0;  TM3D(2,2) = 1.0;
}

void _Matrix3d::_New(YAC_Object * a, YAC_Object * b, YAC_Object * c,
                     YAC_Object * d, YAC_Object * e, YAC_Object * f,
                     YAC_Object * g, YAC_Object * h, YAC_Object * i,
                     YAC_Value *_r) {
   _Matrix3d *r = YAC_NEW_POOLED(Matrix3d);
   _r->initObject(r, YAC_TRUE);
   r->_init(a, b, c,
            d, e, f,
            g, h, i
            );
}

void YAC_CALL _matrix3d(YAC_Object * a, YAC_Object * b, YAC_Object * c,
                        YAC_Object * d, YAC_Object * e, YAC_Object * f,
                        YAC_Object * g, YAC_Object * h, YAC_Object * i,
                        YAC_Value *_r) {
   _Matrix3d *r=YAC_NEW_POOLED(Matrix3d);
   _r->initObject(r, YAC_TRUE);
   r->_init(a, b, c,
            d, e, f,
            g, h, i
            );
}
