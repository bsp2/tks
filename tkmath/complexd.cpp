/// complexd.cpp
///
/// (c) 2008-2023 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include "tkmath_yac.h"

#define _USE_MATH_DEFINES defined

/// Define to use cmath/complex C++ includes (please define in makefile)
//#define DX_USE_C99_COMPLEX defined

#ifdef DX_USE_C99_COMPLEX
#include <cmath>
#include <complex>
#else
#include <math.h>
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // M_PI

#include "tkmath.h"
#include "complexd.h"

#include "ying_Math.h"

_Complexd& _Complexd::operator=(_Complexd& obj) {
   doubles[0]=obj.doubles[0];
   doubles[1]=obj.doubles[1];
   return *this;
}

_Complexd::_Complexd(void) {
   doubles[0]=doubles[1]=0.0;
}

_Complexd::~_Complexd() {
}

sUI YAC_VCALL _Complexd::yacArrayGetNumElements(void) {
   return 2;
}

sUI YAC_VCALL _Complexd::yacArrayGetMaxElements(void) {
   return 2;
}

sUI YAC_VCALL _Complexd::yacArrayGetElementType(void) {
   return YAC_TYPE_OBJECT;
}

sUI YAC_VCALL _Complexd::yacArrayGetElementByteSize(void) {
   return sizeof(sF64);
}

void YAC_VCALL _Complexd::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if (_idx<2) {
      sF64 v;
      switch (_val->type) {   
         case YAC_TYPE_VOID:
            v=0.0;
            break;
         case YAC_TYPE_INT:
            v=(sF64)_val->value.int_val;
            break;
         case YAC_TYPE_FLOAT:
            v=(sF64)_val->value.float_val;
            break;
         case YAC_TYPE_OBJECT:
            if (YAC_VALID(_val->value.object_val)) {
               if (!_val->value.object_val->yacScanF64(&v))
                  v=0.0;
            } else v=0.0;
            break;
         default:
            Dyac_throw(TypeMismatch, "tkmath::Complexd::yacArraySet Unknown Type!");
            v=0.0;
            break;
      }
      doubles[_idx]=v;
   } else {
      static char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Complexd::yacArraySet Index too high (%d>1)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Complexd::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if (_idx<2) {
      //_val->initFloat(doubles[_idx]);
      YAC_Double *v=(YAC_Double*) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
      v->value=doubles[_idx];
      _val->initObject(v,1);
   } else {
      static char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Complexd::yacArrayGet Index too high (%d>1)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
      _val->initFloat(0.0f);
   }
}

void *YAC_VCALL _Complexd::yacArrayGetPointer(void) {
   return (void*)doubles;
}

sSI YAC_VCALL _Complexd::yacTensorRank(void) {
   return YAC_TENSOR_RANK_VECTOR;
}

void YAC_VCALL _Complexd::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F64(doubles[0]);
   YAC_SERIALIZE_F64(doubles[1]);
}

sUI YAC_VCALL _Complexd::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   doubles[0]=YAC_DESERIALIZE_F64();
   doubles[1]=YAC_DESERIALIZE_F64();
   return 1;
}

void YAC_VCALL _Complexd::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if (YAC_CHK(_min,clid_Complexd)&&YAC_CHK(_max,clid_Complexd)) {
      _Complexd *min=(_Complexd*)_min;
      _Complexd *max=(_Complexd*)_max;
      if (doubles[0]<min->doubles[0]) doubles[0]=min->doubles[0];
      else if (doubles[0]>max->doubles[0]) doubles[0]=max->doubles[0];
      if (doubles[1]<min->doubles[1]) doubles[1]=min->doubles[1];
      else if (doubles[1]>max->doubles[1]) doubles[1]=max->doubles[1];
   } 
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "expected Complexd object");
   }
}

void YAC_VCALL _Complexd::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if (YAC_CHK(_min,clid_Complexd)&&YAC_CHK(_max,clid_Complexd)) {
      _Complexd *min=(_Complexd*)_min;
      _Complexd *max=(_Complexd*)_max;
      if (doubles[0]<min->doubles[0]) doubles[0]+=(max->doubles[0]-min->doubles[0]);
      else if (doubles[0]>max->doubles[0]) doubles[0]-=(max->doubles[0]-min->doubles[0]);
      if (doubles[1]<min->doubles[1]) doubles[1]+=(max->doubles[1]-min->doubles[1]);
      else if (doubles[1]>max->doubles[1]) doubles[1]-=(max->doubles[1]-min->doubles[1]);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "expected Complexd object");
   }
}

void YAC_VCALL _Complexd::yacOperator(sSI _cmd,YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      switch (_cmd) {
         case YAC_OP_CEQ:
            YAC_RETI( Ddblequal(doubles[0], o->doubles[0]) && 
                      Ddblequal(doubles[1], o->doubles[1]) );
            break;
         case YAC_OP_CNE:
            YAC_RETI( Ddblnotequal(doubles[0], o->doubles[0]) || 
                      Ddblnotequal(doubles[1], o->doubles[1]) );
            break;
         case YAC_OP_ASSIGN:
            doubles[0]=o->doubles[0];
            doubles[1]=o->doubles[1];
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
         case YAC_OP_DIV:
               _div_YAC_RSELF(_o);
            break;
         case YAC_OP_NEG:
            doubles[0]=-doubles[0];
            doubles[1]=-doubles[1];
            break;
         case YAC_OP_INIT:
            doubles[0]=0.0;
            doubles[1]=0.0;
            break;
         default:
         {
            static char buf[256];
            Dyac_snprintf(buf, 256, "tkmath::Complexd::yacOperator: unsupported cmd %d!\n", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
            return;
         }
      }
   } else if (YAC_Is_FloatArray(_o)) {
      YAC_FloatArray *o=(YAC_FloatArray*)_o;
      switch (_cmd) {
         case YAC_OP_ASSIGN:
            if (o->num_elements<2) {
               if (o->num_elements==1) doubles[0]=(sF64)o->elements[0];
               else doubles[0]=0.0;
               doubles[1]=0.0;
            } else {
               doubles[0]=(sF64)o->elements[0];
               doubles[1]=(sF64)o->elements[1];
            }
            break;
         default:
            YAC_NEW_STACK(Complexd, t);
            t.assignFloatArray(o);
            yacOperator(_cmd, &t, _r);
//            Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Complexd::yacOperator Only Assign supported with FloatArray Type");
            break;
      }
   } else if (YAC_VALID(_o)) {
      sF64 v;
      if (_o->yacScanF64(&v)) {
         yacOperatorF64(_cmd, v, _r);
      } else {
         switch (_cmd) {
            case YAC_OP_ASSIGN:
               assignGeneric(NULL,_o);
               break;
	    default:
	       Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Complexd::yacOperator: Only Assign supported with generic Type");
	       break;
         }
      }
   } else {
      switch (_cmd) {
         case YAC_OP_NEG:
	    doubles[0]=-doubles[0];
	    doubles[1]=-doubles[1];
	    break;
	 default:
            YAC_Object::yacOperator(_cmd,_o,_r);
      }
   }
}

void YAC_VCALL _Complexd::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void YAC_VCALL _Complexd::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void YAC_VCALL _Complexd::yacOperatorF64(sSI _cmd, sF64 val, YAC_Value *_r) {
   switch (_cmd) {
      case YAC_OP_ASSIGN:
         doubles[0] = val;
         doubles[1] = 0;
         break;
      case YAC_OP_CEQ:
         YAC_RETI(Ddblequal( doubles[0], val ));
         break;
      case YAC_OP_CNE:
         YAC_RETI(Ddblnotequal( doubles[0], val ));
         break;
      case YAC_OP_CLT:
         YAC_RETI( doubles[0] < val );
         break;
      case YAC_OP_CLE:
         YAC_RETI( doubles[0] <= val );
         break;
      case YAC_OP_CGT:
         YAC_RETI( doubles[0] > val );
         break;
      case YAC_OP_CGE:
         YAC_RETI( doubles[0] >= val );
         break;
      case YAC_OP_ADD:
         doubles[0] += val;
         break;
      case YAC_OP_SUB:
         doubles[0] -= val;
         break;
      case YAC_OP_MUL:
         muld(val);
         break;
      case YAC_OP_DIV:
         if (Ddblnonzero(val)) {
            muld(1.0/val);
         } else {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::yacOperatorF64 OP_DIV Cannot divide by zero");
         }
         break;
      default:
         char buf[256];
         Dyac_snprintf(buf, 256,"tkmath::Complexd::yacOperatorF64 Unsupported Operator %d",_cmd);
         Dyac_throw_def(TkMathUnsupportedOperator, buf);
         break;
   }
}

void YAC_VCALL _Complexd::yacValueOfI(sSI val) {
   doubles[0] = (sF64) val;
}

void YAC_VCALL _Complexd::yacValueOfI64(sS64 val) {
   doubles[0] = (sF64) val;
}

void YAC_VCALL _Complexd::yacValueOfF32(sF32 val) {
   doubles[0] = (sF64) val;
}

void YAC_VCALL _Complexd::yacValueOfF64(sF64 val) {
   doubles[0] = val;
}

sBool YAC_VCALL _Complexd::yacScanI(sSI *val) {
   //lrintf can be 64bit (on gcc 64bit), but can also be 32bit
   //*val = (sSI) lrintf(doubles[0]);
   *val = (sSI) ( (doubles[0]<0) ? (doubles[0] - 0.5) : (doubles[0] + 0.5) );
   return true;
}

sBool YAC_VCALL _Complexd::yacScanI64(sS64 *val) {
   //llrintf is 64bit
   //*val = (sS64) llrintf(doubles[0]);
   *val = (sS64) ( (doubles[0]<0) ? (doubles[0] - 0.5) : (doubles[0] + 0.5) );
   return true;
}

sBool YAC_VCALL _Complexd::yacScanF32(sF32 *val) {
   *val = (sF32) doubles[0];
   return true;
}

sBool YAC_VCALL _Complexd::yacScanF64(sF64 *val) {
   *val = doubles[0];
   return true;
}

void _Complexd::assignFloats(sF32 *_fv, sUI _ne) {
   if(_ne <= 2) {
      // Copy as much elements from array as possible but never more than 2
      sUI i;
      for(i=0; i<_ne; i++) {
         doubles[i] = (sF64) _fv[i];
      }
      // Fill the rest with 0.0f
      for(; i<2; i++) {
         doubles[i] = 0.0;
      }
   } else {
      // Copy two elements
      doubles[0] = (sF64)_fv[0];
      doubles[1] = (sF64) _fv[1];
   }
}

void _Complexd::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Complexd::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
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
            for(i=0; (i<2) && (i<ne); i++)
            {
               _o->yacArrayGet(_ctx, i, &v);
               doubles[i] = (sF32) v.value.float_val;
            }
            // Fill the rest with 0.0f
            for(; i<2; i++)
            {
               doubles[i] = 0.0;
            }
         }
      }
      else
      {
         // Retrieve individual elements, typecast to double and assign
         YAC_Value v;
         sUI i;
         for(i=0; (i<2) && (i<ne); i++)
         {
            _o->yacArrayGet(_ctx, i, &v);
            v.typecast(YAC_TYPE_OBJECT);
            if (!v.value.object_val->yacScanF64(&doubles[i])) doubles[i] = 0;
            v.unset();
         }
         // Fill the rest with 0.0f
	 for (;i<2;i++) 
	    doubles[i] = 0.0;
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      // Get the scalar real value if possible and fill the rest with 0.0f
      if (!_o->yacScanF64(&doubles[0])) 
         doubles[0] = 0.0;
      doubles[1] = 0.0;
   }
}

//direct element access methods
void _Complexd::_setX(YAC_Object *x) {
   if (YAC_VALID(x)) {
      if (!x->yacScanF64(&doubles[0])) doubles[0]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue,"tkmath::Complexd::setX Invalid Object");
   }
}

void _Complexd::_setY(YAC_Object *y) {
   if (YAC_VALID(y)) {
      if (!y->yacScanF64(&doubles[1])) doubles[1]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue,"tkmath::Complexd::setY Invalid Object");
   }
}

void _Complexd::_getX_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r=(YAC_Double*) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   r->value=doubles[0];
   _r->initObject(r,1);
}

void _Complexd::_getX_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[0]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::getX Return object not valid");
   }
}

void _Complexd::_getY_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r=(YAC_Double*) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   r->value=doubles[1];
   _r->initObject(r,1);
}

void _Complexd::_getY_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[1]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::getY Return object not valid");
   }
}

//access polar representation
//radius/length
void _Complexd::_setR(YAC_Object *r) {
   _unitScale_YAC_RSELF(r);
/*   if (YAC_VALID(r)) {
      sF64 val;
      if (!r->yacScanF64(&val)) val=0.0;
      _unitScale_YAC_RSELF(val);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::setR Object not valid");
   }*/
}

//angle
void _Complexd::_setA(YAC_Object *_a) {
   if (YAC_VALID(_a)) {
      sF64 absVal=getAbs();
      sF64 a;
      if (!_a->yacScanF64(&a)) a=0.0;
      doubles[0]=absVal*cos(a);
      doubles[1]=absVal*sin(a);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::setA Object not valid");
   }
}

//radius/length
sF64 _Complexd::getR() {
   return getAbs();
}

void _Complexd::_getR_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r=(YAC_Double*) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _getR_YAC_RARG(r);
   _r->initObject(r,1);
}

void _Complexd::_getR_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getAbs());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::getR Return object not valid");
   }
}

//angle

sF64 _Complexd::getA() {
   return atan2(doubles[1],doubles[0]);
}

void _Complexd::_getA_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r=(YAC_Double*) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _getA_YAC_RARG(r);
   _r->initObject(r,1);
}

void _Complexd::_getA_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getA());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::getA Return object not valid");
   }
}

//generate string representation
sBool YAC_VCALL _Complexd::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(1024,0,0,0);
   s->printf("(%lg+i*%lg)",doubles[0],doubles[1]);
   s->fixLength();
   return 1;
}

void _Complexd::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   yacToString(s);
   YAC_RETS(s,1);
}

void _Complexd::_getPolarString(YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   s->yacArrayAlloc(1024,0,0,0);
   s->printf("%lg*exp(i*%lg)",getR(),getA());
   YAC_RETS(s,1);
}

//add two complex numbers
void _Complexd::_add_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      doubles[0]+=o->doubles[0];
      doubles[1]+=o->doubles[1];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::add_SELF Object not of Type Complexd");
   }
}

void _Complexd::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Complexd *r=YAC_NEW_POOLED(Complexd);
   _add_YAC_RARG(_o,r);
   _r->initObject(r,1);
}

void _Complexd::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      _Complexd *r=(_Complexd*)_r;
      r->doubles[0]=doubles[0]+o->doubles[0];
      r->doubles[1]=doubles[1]+o->doubles[1];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::add_ARG Object not of Type Complexd");
   }
}

//sub two complex numbers
void _Complexd::_sub_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      doubles[0]-=o->doubles[0];
      doubles[1]-=o->doubles[1];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::sub_SELF Object not of Type Complexd");
   }
}

void _Complexd::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Complexd *r=YAC_NEW_POOLED(Complexd);
   _sub_YAC_RARG(_o, r);
   _r->initObject(r,1);
}

void _Complexd::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      _Complexd *r=(_Complexd*)_r;
      r->doubles[0]=doubles[0]-o->doubles[0];
      r->doubles[1]=doubles[1]-o->doubles[1];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::sub_SELF Object not of Type Complexd");
   }
}

//multiply with scalar value

void _Complexd::muld(sF64 s) {
  doubles[0]*=s;
  doubles[1]*=s;
}

void _Complexd::_muld_YAC_RSELF(YAC_Object *_s) {
   if (YAC_VALID(_s)) {
      sF64 s;
      if (!_s->yacScanF64(&s)) s=0.0;
      doubles[0]*=s;
      doubles[1]*=s;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::mulf_RSELF Object not valid");
   }
}

void _Complexd::_muld_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   _Complexd *r=YAC_NEW_POOLED(Complexd);
   _muld_YAC_RARG(_s, r);
   _r->initObject(r,1);
}

void _Complexd::_muld_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if (YAC_VALID(_s)) {
      if (!YAC_CHK(_r, clid_Complexd)) {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::mulf_ARG Return Object not of type Complexd");
      } else {
         sF64 s;
         _Complexd *r=(_Complexd*)_r;
         if (!_s->yacScanF64(&s)) s=0.0;
         r->doubles[0]=doubles[0]*s;
         r->doubles[1]=doubles[1]*s;
      }
   }
}

//complex multiplication
//(a+ib)*(c+id)=(ac-bd)+i*(ad+bc)
void _Complexd::_mul_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      sF64 a,b;
      a=doubles[0]*o->doubles[0]-doubles[1]*o->doubles[1];
      b=doubles[0]*o->doubles[1]+doubles[1]*o->doubles[0];
      doubles[0]=a;
      doubles[1]=b;
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::mul_SELF Object not of type Complexd");
   }
}

void _Complexd::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Complexd *r=YAC_NEW_POOLED(Complexd);
   _mul_YAC_RARG(_o, r);
   _r->initObject(r,1);
}

void _Complexd::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      _Complexd *r=(_Complexd*)_r;
      r->doubles[0]=doubles[0]*o->doubles[0]-doubles[1]*o->doubles[1];
      r->doubles[1]=doubles[0]*o->doubles[1]+doubles[1]*o->doubles[0];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::mul_ARG Object not of type Complexd");
   }
}

//multiply with complex conjugated number x-iy, returns scalar/real number

void _Complexd::_mulConj_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   YAC_Double *r=(YAC_Double*) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _mulConj_YAC_RARG(_o, r);
   _r->initObject(r,1);
}

void _Complexd::_mulConj_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      if (YAC_CHK(_o,clid_Complexd)) {
         _Complexd *o=(_Complexd*)_o;
         _r->yacValueOfF64(doubles[0]*o->doubles[0]+doubles[1]*o->doubles[1]);
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::mulConj Object not of type Complexd");
      }
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::mulConj Return object not valid");
   }
}

//complex division
//(a+ib)/(c+id)=(a+ib)*(c-id)/(c^2+d^2)=((ac+bd)+i*(-ad+bc))/(c^2+d^2)
void _Complexd::_div_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      sF64 a,b,c;
      a=doubles[0]*o->doubles[0]+doubles[1]*o->doubles[1];
      b=-doubles[0]*o->doubles[1]+doubles[1]*o->doubles[0];
      c=o->doubles[0]*o->doubles[0]+o->doubles[1]*o->doubles[1];
      if (c!=0.0) {
         doubles[0]=a/c;
         doubles[1]=b/c;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::div_SELF Division by Zero");
         doubles[0]=0.0;
         doubles[1]=0.0;
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::div_SELF Object not of Type Complexd");
   }
}

void _Complexd::_div_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      sF64 a,b,c;
      a=doubles[0]*o->doubles[0]+doubles[1]*o->doubles[1];
      b=-doubles[0]*o->doubles[1]+doubles[1]*o->doubles[0];
      c=o->doubles[0]*o->doubles[0]+o->doubles[1]*o->doubles[1];
      if (c!=0.0) {
         _Complexd *r=YAC_NEW_POOLED(Complexd);
         r->doubles[0]=a/c;
         r->doubles[1]=b/c;
         _r->initObject(r,1);
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::div_VAL Division by Zero");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::div_VAL Object not of Type Complexd");
   }
}

void _Complexd::_div_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      _Complexd *r=(_Complexd*)_r;
      sF64 a,b,c;
      a=doubles[0]*o->doubles[0]+doubles[1]*o->doubles[1];
      b=-doubles[0]*o->doubles[1]+doubles[1]*o->doubles[0];
      c=o->doubles[0]*o->doubles[0]+o->doubles[1]*o->doubles[1];
      if (c!=0.0) {
         r->doubles[0]=a/c;
         r->doubles[1]=b/c;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::div_ARG Division by Zero");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::div_ARG Object not of Type Complexd");
   }
}

//calcalute 1/z=z/(z*z_conj)
void _Complexd::_invert_YAC_RSELF(void) {
   sF64 denom=getAbsSqr();
   if (denom!=0.0) {
      doubles[0]=doubles[0]/denom;
      doubles[1]=-doubles[1]/denom;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::invert_SELF Division by Zero, cannot invert Zero Vector");
   }
}

void _Complexd::_invert_YAC_RVAL(YAC_Value *_r) {
   sF64 denom=getAbsSqr();
   if (denom!=0.0) {
      _Complexd *r=YAC_NEW_POOLED(Complexd);
      r->doubles[0]=doubles[0]/denom;
      r->doubles[1]=-doubles[1]/denom;
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::invert_VAL Division by Zero, cannot invert Zero Vector");
   }
}

void _Complexd::_invert_YAC_RARG(YAC_Object *_r) {
   if (!YAC_CHK(_r,clid_Complexd)) {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd:invert_ARG Return object not of type Complexd");
   } else {
      _Complexd *r=(_Complexd*)_r;
      sF64 denom=getAbsSqr();
      if (denom!=0.0) {
         r->doubles[0]=doubles[0]/denom;
         r->doubles[1]=-doubles[1]/denom;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::invert_ARG Division by Zero, cannot invert Zero Vector");
      }
   }
}

//return length of complex number/radius
sF64 _Complexd::getAbs(void) {
#ifdef YAC_GCC
   // C99?
   return hypot(doubles[0],doubles[1]);
#else
   // ISO-C++
   return _hypot(doubles[0],doubles[1]);
#endif // YAC_GCC
   //return sqrt(doubles[0]*doubles[0]+doubles[1]*doubles[1]);
}

void _Complexd::_getAbs_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r=(YAC_Double*) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   r->yacValueOfF64(getAbs());
   _r->initObject(r,1);
}

void _Complexd::_getAbs_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getAbs());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::getAbs Return object not valid");
   }
}

//return squared length
sF64 _Complexd::getAbsSqr(void) {
   return (doubles[0]*doubles[0]+doubles[1]*doubles[1]);
}

void _Complexd::_getAbsSqr_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   r->yacValueOfF64(getAbsSqr());
   _r->initObject(r,1);
}

void _Complexd::_getAbsSqr_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getAbsSqr());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::getAbsSqr Return object not valid");
   }
}

//scale complex number to length 1
void _Complexd::_unit_YAC_RSELF(void) {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      doubles[0]/=absval;
      doubles[1]/=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::unit_SELF Zero Vector cannot be normalized");
      doubles[0]=doubles[1]=0.0; //this may change to x=1.0f, y=0.0f
   }
}

void _Complexd::_unit_YAC_RVAL(YAC_Value *_r) {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      _Complexd *r=YAC_NEW_POOLED(Complexd);
      r->doubles[0]=doubles[0]/absval;
      r->doubles[1]=doubles[1]/absval;
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::unit Zero Vector cannot be normalized");
   }
}

void _Complexd::_unit_YAC_RARG(YAC_Object *_r) {
   if (!YAC_CHK(_r,clid_Complexd)) {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::unit_ARG Return object not of type Complexd");
   } else {
      _Complexd *r=(_Complexd*)_r;
      sF64 absval=getAbs();
      if (Ddblnonzero(absval)) {
         r->doubles[0]=doubles[0]/absval;
         r->doubles[1]=doubles[1]/absval;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::unit_ARG Zero Vector cannot be normalized");
      }
   }
}

//scale complex number to length s
void _Complexd::_unitScale_YAC_RSELF(YAC_Object *_s) {
   if (YAC_VALID(_s)) {
      sF64 absval=getAbs();
      sF64 s;
      if (!_s->yacScanF64(&s)) s=0.0;
      if (Ddblnonzero(absval)) {
         absval=s/absval;
         doubles[0]*=absval;
         doubles[1]*=absval;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::unitScale_SELF Zero Vector cannot be normalized");
         doubles[0]=doubles[1]=0.0; //this may change to x=1.0f, y=0.0f
      }
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::unitScale_SELF Object not valid");
   }

//second version
//which is faster?
/*
   sF32 angle=_getA();
   doubles[0]=s*cos(angle);
   doubles[1]=s*sin(angle);*/
}

void _Complexd::_unitScale_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   _Complexd *r=YAC_NEW_POOLED(Complexd);
   _unitScale_YAC_RARG(_s, r);
   _r->initObject(r,1);
}

void _Complexd::_unitScale_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if (YAC_VALID(_s)) {
      if (YAC_CHK(_r,clid_Complexd)) {
         sF64 absval=getAbs();
	 sF64 s;
	 if (!_s->yacScanF64(&s)) s=0.0;
         if (Ddblnonzero(absval)) {
            _Complexd *r=(_Complexd*)_r;
            absval=s/absval;
            r->doubles[0]=doubles[0]*absval;
            r->doubles[1]=doubles[1]*absval;
         } else {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexd::unitScale_ARG Zero Vector cannot be normalized");
         }
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexd::unitScale_ARG Return object not of type Complexd");
      }
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::unitScale_ARG Object not valid");
   }
}

//initialisation methods
void _Complexd::initd(sF64 x, sF64 y) {
   doubles[0] = x;
   doubles[1] = y;
}

void _Complexd::_init(YAC_Object *_x, YAC_Object *_y) {
   if (YAC_VALID(_x)&&YAC_VALID(_y)) {
      if (!_x->yacScanF64(&doubles[0])) doubles[0]=0.0;
      if (!_y->yacScanF64(&doubles[1])) doubles[1]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::init Objects not valid");
   }
}

void _Complexd::_initf(sF32 x, sF32 y) {
   doubles[0] = (sF64) x;
   doubles[1] = (sF64) y;
}

void _Complexd::_initPolar(YAC_Object *_r, YAC_Object *_a) {
   if (YAC_VALID(_r)&&YAC_VALID(_a)) {
      sF64 r,a;
      if (!_r->yacScanF64(&r)) r=0.0;
      if (!_a->yacScanF64(&a)) a=0.0;
      doubles[0] = r * cos(a);
      doubles[1] = r * sin(a);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Complexd::initPolar Objects not valid");
   }
}

void _Complexd::_initPolarf(sF32 _a, sF32 _b) {
   sF64 r,a;
   r=(sF64)_a;
   a=(sF64)_b;
   doubles[0] = r * cos(a);
   doubles[1] = r * sin(a);
}

void _Complexd::_New(YAC_Object *_x, YAC_Object *_y, YAC_Value *_r) {
   _Complexd *r=YAC_NEW_POOLED(Complexd);
   r->_init(_x,_y);
   _r->initObject(r,1);
}

void _Complexd::_NewPolar(YAC_Object *_radius, YAC_Object *_angle, YAC_Value *_r) {
   _Complexd *r=YAC_NEW_POOLED(Complexd);
   r->_initPolar(_radius,_angle);
   _r->initObject(r,1);
}

void YAC_CALL complexd(YAC_Object *_x, YAC_Object *_y, YAC_Value *_r) {
   _Complexd *r=YAC_NEW_POOLED(Complexd);
   r->_init(_x,_y);
   _r->initObject(r,1);
}

void YAC_CALL complexdPolar(YAC_Object *_x, YAC_Object *_y, YAC_Value *_r) {
   _Complexd *r=YAC_NEW_POOLED(Complexd);
   r->_initPolar(_x,_y);
   _r->initObject(r,1);
}

//additional trigonometric and hyperbolic functions
#ifdef DX_USE_C99_COMPLEX
//this version uses native complex data type
//
//use native sqrt
void YAC_CALL _sqrtcd_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(o->doubles[0],o->doubles[1]);
      z=std::sqrt(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sqrtcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _sqrtcd_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *o=(_Complexd*)_o;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(o->doubles[0],o->doubles[1]);
      z=std::sqrt(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sqrtcd_ARG Object is not of type Complexd");
   }
}


//use native pow
void YAC_CALL _powcd_YAC_RVAL(YAC_Object *_b, YAC_Object *_e, YAC_Value *_r) {
   if (YAC_CHK(_b,clid_Complexd)&&YAC_CHK(_e,clid_Complexd)) {
      _Complexd *b=(_Complexd*)_b;
      _Complexd *e=(_Complexd*)_e;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> bc(b->doubles[0],b->doubles[1]),ec(e->doubles[0],e->doubles[1]);
      std::complex<sF64> z;
      z=std::pow(bc,ec);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _powcd_YAC_RARG(YAC_Object *_b, YAC_Object *_e, YAC_Object *_r) {
   if (YAC_CHK(_b,clid_Complexd)&&YAC_CHK(_e,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *b=(_Complexd*)_b;
      _Complexd *e=(_Complexd*)_e;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> bc(b->doubles[0],b->doubles[1]),ec(e->doubles[0],e->doubles[1]);
      std::complex<sF64> z;
      z=std::pow(bc,ec);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powcd_ARG Object is not of type Complexd");
   }
}

//same as before, but you can select the other solutions at wish
//no implementation in stdc complex.h
void YAC_CALL _powncd_YAC_RVAL(YAC_Object *_b, YAC_Object *_e, YAC_Object *_n, YAC_Value *_r) {
   if (YAC_CHK(_b,clid_Complexd)&&YAC_CHK(_e,clid_Complexd)&&YAC_CHK(_n,clid_Complexd)) {
      _Complexd *b=(_Complexd*)_b;
      _Complexd *e=(_Complexd*)_e;
      _Complexd *n=(_Complexd*)_n;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      sF64 r,a;
      sF64 lnra = (sF64) (log(b->getR())-n->doubles[1]*2.0*M_PI);
      sF64 aa = (sF64) (b->getA()+n->doubles[0]*2.0*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      r=exp(e->doubles[0]*lnra-e->doubles[1]*aa);
      a=e->doubles[0]*aa+e->doubles[1]*lnra;
      res->doubles[0]=r*cos(a);
      res->doubles[1]=r*sin(a);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powncd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _powncd_YAC_RARG(YAC_Object *_b, YAC_Object *_e, YAC_Object *_n, YAC_Object *_r) {
   if (YAC_CHK(_b,clid_Complexd)&&YAC_CHK(_e,clid_Complexd)&&YAC_CHK(_n,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *b=(_Complexd*)_b;
      _Complexd *e=(_Complexd*)_e;
      _Complexd *n=(_Complexd*)_n;
      _Complexd *res = (_Complexd*)_r;
      sF64 r,a;
      sF64 lnra = (sF64) (log(b->getR())-n->doubles[1]*2.0*M_PI);
      sF64 aa = (sF64) (b->getA()+n->doubles[0]*2.0*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      r=expf(e->doubles[0]*lnra-e->doubles[1]*aa);
      a=e->doubles[0]*aa+e->doubles[1]*lnra;
      res->doubles[0]=r*cos(a);
      res->doubles[1]=r*sin(a);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powncd_ARG Object is not of type Complexd");
   }
}

//use native exp
void YAC_CALL _expcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::exp(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::expcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _expcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::exp(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::expcd_ARG Object is not of type Complexd");
   }
}

//use native log
void YAC_CALL _logcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::log(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag(); //+2*M_PI*n with n=0,1,2,3,...
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _logcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::log(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag(); //+2*M_PI*n with n=0,1,2,3,...
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logcd_ARG Object is not of type Complexd");
   }
}

//ln(z)=ln(r_z)+i*(a_z+2*pi*n)=ln(r_z)-y_n*2*pi+i*(a_z+x_n*2*pi), this one with n!=0
//no implementation in stdc complex.h
void YAC_CALL _logncd_YAC_RVAL(YAC_Object *_v, YAC_Object *_n,YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_n,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *n=(_Complexd*)_n;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      res->doubles[0] = (sF64) (log(v->getR())-n->doubles[1]*2.0*M_PI);
      res->doubles[1] = (sF64) (v->getA()+n->doubles[0]*2.0*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logncd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _logncd_YAC_RARG(YAC_Object *_v, YAC_Object *_n,YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_n,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *n=(_Complexd*)_n;
      _Complexd *res = (_Complexd*)_r;
      res->doubles[0] = (sF64) (log(v->getR())-n->doubles[1]*2.0*M_PI);
      res->doubles[1] = (sF64) (v->getA()+n->doubles[0]*2.0*M_PI); //+2*M_PI*n with n=0,1,2,3,...
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logncd_ARG Object is not of type Complexd");
   }
}

//use native sin
void YAC_CALL _sincd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::sin(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sincd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _sincd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::sin(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sincd_ARG Object is not of type Complexd");
   }
}

//use native cos
void YAC_CALL _coscd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::cos(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coscd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _coscd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::cos(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coscd_ARG Object is not of type Complexd");
   }
}

//use native tan
void YAC_CALL _tancd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::tan(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tancd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _tancd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::tan(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tancd_ARG Object is not of type Complexd");
   }
}

//use native sinh
void YAC_CALL _sinhcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::sinh(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sinhcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _sinhcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::sinh(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sinhcd_ARG Object is not of type Complexd");
   }
}

//use native cosh
void YAC_CALL _coshcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::cosh(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coshcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _coshcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::cosh(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coshcd_ARG Object is not of type Complexd");
   }
}

//use native tanh
void YAC_CALL _tanhcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::tanh(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tanhcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _tanhcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      z=std::tanh(z);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tanhcd_ARG Object is not of type Complexd");
   }
}

//there is noe native invert method for neither trigonometric nor hyperbolic functions
//
//asinh=log(z+sqrt(z*z+1))
void YAC_CALL _asinhcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      //z=std::asinh(z);  //hooray, this does not exist, so do the calc manually
      //this is true for all arcus functions
      std::complex<sF64> val;
      val=z*z+1.0;
      val=std::sqrt(val);
      val=z+val;
      z=std::log(val);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1); 
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asinhcd_VAL Object is not of type Complexd");
   }   
}

void YAC_CALL _asinhcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      //z=std::asinh(z);  //hooray, this does not exist, so do the calc manually
      //this is true for all arcus functions
      std::complex<sF64> val;
      val=z*z+1.0;
      val=std::sqrt(val);
      val=z+val;
      z=std::log(val);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asinhcd_ARG Object is not of type Complexd");
   }   
}

//acosh=log(z+sqrt(z*z-1))
void YAC_CALL _acoshcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      //z=std::acosh(z); //see asinhcd
      std::complex<sF64> val;
      val=z*z-1.0;
      val=std::sqrt(val);
      val=z+val;
      z=std::log(val);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1); 
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoshcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _acoshcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      //z=std::acosh(z); //see asinhcd
      std::complex<sF64> val;
      val=z*z-1.0;
      val=std::sqrt(val);
      val=z+val;
      z=std::log(val);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoshcd_ARG Object is not of type Complexd");
   }
}

//atanh=0.5*log((z+1)/(z-1))
void YAC_CALL _atanhcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      //z=std::atanh(z); //see asinhcd
      std::complex<sF64> nom,denom;
      nom=1.0+z;
      denom=1.0-z;
      //yac_host->printf("nominator: (%f, %f) denominator: (%f, %f)\n",
      //   nom.real(),nom.imag(),denom.real(),denom.imag());
      nom/=denom;
      //yac_host->printf("nominator/denominator=(%f, %f)\n",nom.real(),nom.imag());
      z=0.5*log(nom);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atanhcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _atanhcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      //z=std::atanh(z); //see asinhcd
      std::complex<sF64> nom,denom;
      nom=1.0+z;
      denom=1.0-z;
      //yac_host->printf("nominator: (%f, %f) denominator: (%f, %f)\n",
      //   nom.real(),nom.imag(),denom.real(),denom.imag());
      nom/=denom;
      //yac_host->printf("nominator/denominator=(%f, %f)\n",nom.real(),nom.imag());
      z=0.5*log(nom);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atanhcd_ARG Object is not of type Complexd");
   }
}

//asin=-i*log(i*z+sqrt(1-z*z))
void YAC_CALL _asincd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      //z=std::asin(z);
      std::complex<sF64> val;
      val=1.0-z*z;
      //yac_host->printf("1-z^2=(%f, %f)\n",val.real(),val.imag());
      val=std::sqrt(val);
      //yac_host->printf("sqrt(1-z^2)=(%f, %f)\n",val.real(),val.imag());
      val=std::complex<sF64>(0.0,1.0)*z+val;
      //std::complex<float> testval=std::complex<float>(0.0f,1.0f)*z;
      //yac_host->printf("i*z=(%f,%f)\n",testval.real(),testval.imag());
      //yac_host->printf("i*z+sqrt(1-z^2)=(%f, %f)\n",val.real(),val.imag());
      z=std::complex<sF64>(0.0,-1.0)*std::log(val);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asincd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _asincd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      //z=std::asin(z);
      std::complex<sF64> val;
      val=1.0-z*z;
      //yac_host->printf("1-z^2=(%f, %f)\n",val.real(),val.imag());
      val=std::sqrt(val);
      //yac_host->printf("sqrt(1-z^2)=(%f, %f)\n",val.real(),val.imag());
      val=std::complex<sF64>(0.0,1.0)*z+val;
      //std::complex<float> testval=std::complex<float>(0.0f,1.0f)*z;
      //yac_host->printf("i*z=(%f,%f)\n",testval.real(),testval.imag());
      //yac_host->printf("i*z+sqrt(1-z^2)=(%f, %f)\n",val.real(),val.imag());
      z=std::complex<sF64>(0.0,-1.0)*std::log(val);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asincd_ARG Object is not of type Complexd");
   }
}

//acos=-i*log(z+i*sqrt(1-z*z))
void YAC_CALL _acoscd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      //z=std::acos(z); //see asinhcd
      std::complex<sF64> val;
      val=1.0-z*z;
      //yac_host->printf("z^2-1=(%f, %f)\n",val.real(),val.imag());
      val=std::complex<sF64>(0.0,1.0)*std::sqrt(val);
      //yac_host->printf("sqrt(z^2-1)=(%f, %f)\n",val.real(),val.imag());
      val=z+val;
      //yac_host->printf("z+sqrt(z^2-1)=(%f, %f)\n",val.real(),val.imag());
      z=std::complex<sF64>(0.0,-1.0)*std::log(val);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoscd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _acoscd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(v->doubles[0],v->doubles[1]);
      //z=std::acos(z); //see asinhcd
      std::complex<sF64> val;
      val=1.0-z*z;
      //yac_host->printf("z^2-1=(%f, %f)\n",val.real(),val.imag());
      val=std::complex<sF64>(0.0,1.0)*std::sqrt(val);
      //yac_host->printf("sqrt(z^2-1)=(%f, %f)\n",val.real(),val.imag());
      val=z+val;
      //yac_host->printf("z+sqrt(z^2-1)=(%f, %f)\n",val.real(),val.imag());
      z=std::complex<sF64>(0.0,-1.0)*std::log(val);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoscd_ARG Object is not of type Complexd");
   }
}

//atan=-(i/2)*log((1+i*z)/(1-i*z))
void YAC_CALL _atancd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      std::complex<sF64> z(-v->doubles[1],v->doubles[0]); //i*z
      //z=std::atan(z); //see asinhcd
      std::complex<sF64> nom,denom;
      nom=1.0+z;
      denom=1.0-z;
      nom/=denom;
      z=std::complex<sF64>(0.0,-0.5)*log(nom);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atancd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _atancd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      std::complex<sF64> z(-v->doubles[1],v->doubles[0]); //i*z
      //z=std::atan(z); //see asinhcd
      std::complex<sF64> nom,denom;
      nom=1.0+z;
      denom=1.0-z;
      nom/=denom;
      z=std::complex<sF64>(0.0,-0.5)*log(nom);
      res->doubles[0]=z.real();
      res->doubles[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atancd_ARG Object is not of type Complexd");
   }
}

#else 
//this version uses the supplied data type from the plugin
//this may be slower but is more portable
//
//sqrt(z)=sqrt(r*exp(i*a))=sqrt(r)*exp(i*a/2.0)
void YAC_CALL _sqrtcd_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Complexd)) {
      sF64 r,a;
      _Complexd *o=(_Complexd*)_o;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      r=sqrt(o->getR());
      a=o->getA()/2.0;
      res->doubles[0]=r*cos(a);
      res->doubles[1]=r*sin(a);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sqrtcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _sqrtcd_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      sF64 r,a;
      _Complexd *o=(_Complexd*)_o;
      _Complexd *res = (_Complexd*)_r;
      r=sqrt(o->getR());
      a=o->getA()/2.0;
      res->doubles[0]=r*cos(a);
      res->doubles[1]=r*sin(a);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sqrtcd_ARG Object is not of type Complexd");
   }
}

//b^a=(r*exp(i*a))^(x+iy)=(r^x+exp(log(r)*iy))*exp(i*a*(x+iy))a
//=exp(x_z*(ln(r_a)+i*a_a))*exp(i*y_z*(ln(r_a)+i*a_a))
//=exp(x_z*ln(r_a))*exp(i*x_z*a_a)*exp(i*y_z*ln(r_a))*exp(-y_z*a_a)
//=exp(x_z*ln(r_a)-y_z*a_a)*exp(i*(x_z*a_a+y_z*ln(r_a))
void YAC_CALL _powcd_YAC_RVAL(YAC_Object *_b, YAC_Object *_e, YAC_Value *_r) {
   if (YAC_CHK(_b,clid_Complexd)&&YAC_CHK(_e,clid_Complexd)) {
      _Complexd *b=(_Complexd*)_b;
      _Complexd *e=(_Complexd*)_e;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      sF64 r,a;
      sF64 lnra=log(b->getR());
      sF64 aa=b->getA(); //+2*M_PI*n with n=0,1,2,3,...
      r=exp(e->doubles[0]*lnra-e->doubles[1]*aa);
      a=e->doubles[0]*aa+e->doubles[1]*lnra;
      res->doubles[0]=r*cos(a);
      res->doubles[1]=r*sin(a);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _powcd_YAC_RARG(YAC_Object *_b, YAC_Object *_e, YAC_Object *_r) {
   if (YAC_CHK(_b,clid_Complexd)&&YAC_CHK(_e,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *b=(_Complexd*)_b;
      _Complexd *e=(_Complexd*)_e;
      _Complexd *res = (_Complexd*)_r;
      sF64 r,a;
      sF64 lnra=log(b->getR());
      sF64 aa=b->getA(); //+2*M_PI*n with n=0,1,2,3,...
      r=exp(e->doubles[0]*lnra-e->doubles[1]*aa);
      a=e->doubles[0]*aa+e->doubles[1]*lnra;
      res->doubles[0]=r*cos(a);
      res->doubles[1]=r*sin(a);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powcd_ARG Object is not of type Complexd");
   }
}

//same as before, but you can select the other solutions at wish
void YAC_CALL _powncd_YAC_RVAL(YAC_Object *_b, YAC_Object *_e, YAC_Object *_n, YAC_Value *_r) {
   if (YAC_CHK(_b,clid_Complexd)&&YAC_CHK(_e,clid_Complexd)&&YAC_CHK(_n,clid_Complexd)) {
      _Complexd *b=(_Complexd*)_b;
      _Complexd *e=(_Complexd*)_e;
      _Complexd *n=(_Complexd*)_n;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      sF64 r,a;
      sF64 lnra = (sF64) (log(b->getR())-n->doubles[1]*2.0*M_PI);
      sF64 aa = (sF64) (b->getA()+n->doubles[0]*2.0*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      r=exp(e->doubles[0]*lnra-e->doubles[1]*aa);
      a=e->doubles[0]*aa+e->doubles[1]*lnra;
      res->doubles[0]=r*cos(a);
      res->doubles[1]=r*sin(a);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powncd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _powncd_YAC_RARG(YAC_Object *_b, YAC_Object *_e, YAC_Object *_n, YAC_Object *_r) {
   if (YAC_CHK(_b,clid_Complexd)&&YAC_CHK(_e,clid_Complexd)&&YAC_CHK(_n,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *b=(_Complexd*)_b;
      _Complexd *e=(_Complexd*)_e;
      _Complexd *n=(_Complexd*)_n;
      _Complexd *res = (_Complexd*)_r;
      sF64 r,a;
      sF64 lnra = (sF64) (log(b->getR())-n->doubles[1]*2.0*M_PI);
      sF64 aa = (sF64) (b->getA()+n->doubles[0]*2.0*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      r=exp(e->doubles[0]*lnra-e->doubles[1]*aa);
      a=e->doubles[0]*aa+e->doubles[1]*lnra;
      res->doubles[0]=r*cos(a);
      res->doubles[1]=r*sin(a);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powncd_ARG Object is not of type Complexd");
   }
}

//exp(z)=exp(x+iy)=exp(x)*exp(iy)
void YAC_CALL _expcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      sF64 r;
      r=exp(v->doubles[0]);
      res->doubles[0]=r*cos(v->doubles[1]);
      res->doubles[1]=r*sin(v->doubles[1]);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::expcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _expcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      sF64 r;
      r=exp(v->doubles[0]);
      res->doubles[0]=r*cos(v->doubles[1]);
      res->doubles[1]=r*sin(v->doubles[1]);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::expcd_ARG Object is not of type Complexd");
   }
}

//ln(z)=ln(r_z)+i*(a_z+2*pi*n)=ln(r_z)-y_n*2*pi+i*(a_z+x_n*2*pi), this one with n=0
void YAC_CALL _logcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      res->doubles[0]=log(v->getR());
      res->doubles[1]=v->getA(); //+2*M_PI*n with n=0,1,2,3,...
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _logcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      res->doubles[0]=log(v->getR());
      res->doubles[1]=v->getA(); //+2*M_PI*n with n=0,1,2,3,...
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logcd_ARG Object is not of type Complexd");
   }
}

//ln(z)=ln(r_z)+i*(a_z+2*pi*n)=ln(r_z)-y_n*2*pi+i*(a_z+x_n*2*pi), this one with n!=0
void YAC_CALL _logncd_YAC_RVAL(YAC_Object *_v, YAC_Object *_n,YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_n,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *n=(_Complexd*)_n;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      res->doubles[0] = (sF64) (log(v->getR())-n->doubles[1]*2.0*M_PI);
      res->doubles[1] = (sF64) (v->getA()+n->doubles[0]*2.0*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logncd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _logncd_YAC_RARG(YAC_Object *_v, YAC_Object *_n,YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_n,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *n=(_Complexd*)_n;
      _Complexd *res = (_Complexd*)_r;
      res->doubles[0] = (sF64) (log(v->getR())-n->doubles[1]*2.0*M_PI);
      res->doubles[1] = (sF64) (v->getA()+n->doubles[0]*2.0*M_PI); //+2*M_PI*n with n=0,1,2,3,...
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logncd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _sincd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      res->doubles[0]=sin(v->doubles[0])*cosh(v->doubles[1]);
      res->doubles[1]=cos(v->doubles[0])*sinh(v->doubles[1]);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sincd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _sincd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      res->doubles[0]=sin(v->doubles[0])*cosh(v->doubles[1]);
      res->doubles[1]=cos(v->doubles[0])*sinh(v->doubles[1]);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sincd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _coscd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      res->doubles[0]=cos(v->doubles[0])*cosh(v->doubles[1]);
      res->doubles[1]=-sin(v->doubles[0])*sinh(v->doubles[1]);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coscd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _coscd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      res->doubles[0]=cos(v->doubles[0])*cosh(v->doubles[1]);
      res->doubles[1]=-sin(v->doubles[0])*sinh(v->doubles[1]);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coscd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _tancd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      sF64 denom=cos(2.0*v->doubles[0])+cosh(2.0*v->doubles[1]);
      if (denom!=0.0) {
         res->doubles[0]=sin(2.0*v->doubles[0])/denom;
         res->doubles[1]=sinh(2.0*v->doubles[1])/denom;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::tancd_VAL Division by Zero");
      }
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tancd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _tancd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      sF64 denom=cos(2.0*v->doubles[0])+cosh(2.0*v->doubles[1]);
      if (denom!=0.0) {
         res->doubles[0]=sin(2.0*v->doubles[0])/denom;
         res->doubles[1]=sinh(2.0*v->doubles[1])/denom;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::tancd_ARG Division by Zero");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tancd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _sinhcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      res->doubles[0]=sinh(v->doubles[0])*cos(v->doubles[1]);
      res->doubles[1]=cosh(v->doubles[0])*sin(v->doubles[1]);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sinhcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _sinhcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      res->doubles[0]=sinh(v->doubles[0])*cos(v->doubles[1]);
      res->doubles[1]=cosh(v->doubles[0])*sin(v->doubles[1]);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sinhcd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _coshcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      res->doubles[0]=cosh(v->doubles[0])*cos(v->doubles[1]);
      res->doubles[1]=sinh(v->doubles[0])*sin(v->doubles[1]);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coshcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _coshcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      res->doubles[0]=cosh(v->doubles[0])*cos(v->doubles[1]);
      res->doubles[1]=sinh(v->doubles[0])*sin(v->doubles[1]);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coshcd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _tanhcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      sF64 denom=cosh(2.0*v->doubles[0])+cos(2.0*v->doubles[1]);
      if (denom!=0.0) {
         res->doubles[0]=sinh(2.0*v->doubles[0])/denom;
         res->doubles[1]=sin(2.0*v->doubles[1])/denom;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::tanhcd_VAL Division by Zero");
      }
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tanhcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _tanhcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      sF64 denom=cosh(2.0*v->doubles[0])+cos(2.0*v->doubles[1]);
      if (denom!=0.0) {
         res->doubles[0]=sinh(2.0*v->doubles[0])/denom;
         res->doubles[1]=sin(2.0*v->doubles[1])/denom;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::tanhcd_ARG Division by Zero");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tanhcd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _asinhcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      YAC_NEW_STACK(Complexd, root);
      YAC_NEW_STACK(Complexd, helper);
      res->doubles[0]=0.0;
      res->doubles[1]=0.0;
      root=*v;
      root._mul_YAC_RSELF(v);
      root.doubles[0]+=1.0;
      _sqrtcd_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *rooted=&helper; //(_Complexd*)helper.value.object_val;
         YAC_NEW_STACK(Complexd, helper2);
         rooted->_add_YAC_RSELF(v);
         _logcd_YAC_RARG(rooted,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexd *logarithm=&helper2; //(_Complexd*)helper2.value.object_val;
            res->doubles[0]=logarithm->doubles[0];
            res->doubles[1]=logarithm->doubles[1];
//            yac_host->yacDelete(logarithm);
//         } //something gone bad - should not happen except memory exhaust
//         yac_host->yacDelete(rooted);
//      } //something gone bad - should not happen except memory exhaust
      _r->initObject(res,1); 
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asinhcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _asinhcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      YAC_NEW_STACK(Complexd, root);
      YAC_NEW_STACK(Complexd, helper);
      res->doubles[0]=0.0;
      res->doubles[1]=0.0;
      root=*v;
      root._mul_YAC_RSELF(v);
      root.doubles[0]+=1.0;
      _sqrtcd_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *rooted=&helper; //(_Complexd*)helper.value.object_val;
         YAC_NEW_STACK(Complexd, helper2);
         rooted->_add_YAC_RSELF(v);
         _logcd_YAC_RARG(rooted,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexd *logarithm=&helper2; //(_Complexd*)helper2.value.object_val;
            res->doubles[0]=logarithm->doubles[0];
            res->doubles[1]=logarithm->doubles[1];
//            yac_host->yacDelete(logarithm);
//         } //something gone bad - should not happen except memory exhaust
//         yac_host->yacDelete(rooted);
//      } //something gone bad - should not happen except memory exhaust
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asinhcd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _acoshcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      YAC_NEW_STACK(Complexd, root);
      YAC_NEW_STACK(Complexd, helper);
      res->doubles[0]=0.0;
      res->doubles[1]=0.0;
      root=*v;
      root._mul_YAC_RSELF(v);
      root.doubles[0]-=1.0;
      _sqrtcd_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *rooted=&helper; //(_Complexd*)helper.value.object_val;
         YAC_NEW_STACK(Complexd, helper2);
         rooted->_add_YAC_RSELF(v);
         _logcd_YAC_RARG(rooted,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexd *logarithm=&helper2; //(_Complexd*)helper2.value.object_val;
            res->doubles[0]=logarithm->doubles[0];
            res->doubles[1]=logarithm->doubles[1];
//            yac_host->yacDelete(logarithm);
//         } //something gone bad - should not happen except memory exhaust
//         yac_host->yacDelete(rooted);
//      } //something gone bad - should not happen except memory exhaust
      _r->initObject(res,1); 
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoshcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _acoshcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      YAC_NEW_STACK(Complexd, root);
      YAC_NEW_STACK(Complexd, helper);
      res->doubles[0]=0.0;
      res->doubles[1]=0.0;
      root=*v;
      root._mul_YAC_RSELF(v);
      root.doubles[0]-=1.0;
      _sqrtcd_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *rooted=&helper; //(_Complexd*)helper.value.object_val;
         YAC_NEW_STACK(Complexd, helper2);
         rooted->_add_YAC_RSELF(v);
         _logcd_YAC_RARG(rooted,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexd *logarithm=&helper2; //(_Complexd*)helper2.value.object_val;
            res->doubles[0]=logarithm->doubles[0];
            res->doubles[1]=logarithm->doubles[1];
//            yac_host->yacDelete(logarithm);
//         } //something gone bad - should not happen except memory exhaust
//         yac_host->yacDelete(rooted);
//      } //something gone bad - should not happen except memory exhaust
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoshcd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _atanhcd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      YAC_NEW_STACK(Complexd, nom);
      YAC_NEW_STACK(Complexd, denom);
      YAC_NEW_STACK(Complexd, helper);
      res->doubles[0]=0.0;
      res->doubles[1]=0.0;
      nom.initd(1.0,0.0);
      nom._add_YAC_RSELF(v);
      denom.initd(1.0,0.0);
      denom._sub_YAC_RSELF(v);
      //yac_host->printf("nominator: (%f, %f) denominator: (%f, %f)\n",
      //   nom.doubles[0],nom.doubles[1],denom.doubles[0],denom.doubles[1]);
      nom._div_YAC_RSELF(&denom);
      //yac_host->printf("nominator/denominator=(%f, %f)\n",nom.doubles[0],nom.doubles[1]);
      _logcd_YAC_RARG(&nom,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *logarithm=&helper; //(_Complexd*)helper.value.object_val;
         logarithm->muld(0.5);
         res->doubles[0]=logarithm->doubles[0];
         res->doubles[1]=logarithm->doubles[1];
//         yac_host->yacDelete(logarithm);
//      }
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atanhcd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _atanhcd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      YAC_NEW_STACK(Complexd, nom);
      YAC_NEW_STACK(Complexd, denom);
      YAC_NEW_STACK(Complexd, helper);
      res->doubles[0]=0.0;
      res->doubles[1]=0.0;
      nom.initd(1.0,0.0);
      nom._add_YAC_RSELF(v);
      denom.initd(1.0,0.0);
      denom._sub_YAC_RSELF(v);
      //yac_host->printf("nominator: (%f, %f) denominator: (%f, %f)\n",
      //   nom.doubles[0],nom.doubles[1],denom.doubles[0],denom.doubles[1]);
      nom._div_YAC_RSELF(&denom);
      //yac_host->printf("nominator/denominator=(%f, %f)\n",nom.doubles[0],nom.doubles[1]);
      _logcd_YAC_RARG(&nom,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *logarithm=&helper; //(_Complexd*)helper.value.object_val;
         logarithm->muld(0.5);
         res->doubles[0]=logarithm->doubles[0];
         res->doubles[1]=logarithm->doubles[1];
//         yac_host->yacDelete(logarithm);
//      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atanhcd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _asincd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      YAC_NEW_STACK(Complexd, root);
      YAC_NEW_STACK(Complexd, vv);
      YAC_NEW_STACK(Complexd, helper);
      res->initd(0.0,0.0);
      root.initd(1.0,0.0);
      vv=*v;
      vv._mul_YAC_RSELF(v);
      root._sub_YAC_RSELF(&vv);
      //yac_host->printf("1-z^2=(%f, %f)\n",root.doubles[0],root.doubles[1]);

      vv.doubles[0]=-v->doubles[1];
      vv.doubles[1]=v->doubles[0];
      _sqrtcd_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *rooted=&helper; //(_Complexd*)helper.value.object_val;
         YAC_NEW_STACK(Complexd, helper2);
         //yac_host->printf("sqrt(1-z^2)=(%f, %f)\n",rooted->doubles[0],rooted->doubles[1]);

         vv._add_YAC_RSELF(rooted);
         //yac_host->printf("i*z+sqrt(1-z^2)=(%f, %f)\n",vv.doubles[0],vv.doubles[1]);

         _logcd_YAC_RARG(&vv,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexd *logarithm=&helper2; //(_Complexd*)helper2.value.object_val;
            res->doubles[0]=logarithm->doubles[1];
            res->doubles[1]=-logarithm->doubles[0];
//            yac_host->yacDelete(logarithm);
//         } //should not happen
//         yac_host->yacDelete(rooted);
//      } //should not happen
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asincd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _asincd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      YAC_NEW_STACK(Complexd, root);
      YAC_NEW_STACK(Complexd, vv);
      YAC_NEW_STACK(Complexd, helper);
      res->initd(0.0,0.0);
      root.initd(1.0,0.0);
      vv=*v;
      vv._mul_YAC_RSELF(v);
      root._sub_YAC_RSELF(&vv);
      //yac_host->printf("1-z^2=(%f, %f)\n",root.doubles[0],root.doubles[1]);

      vv.doubles[0]=-v->doubles[1];
      vv.doubles[1]=v->doubles[0];
      _sqrtcd_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *rooted=&helper; //(_Complexd*)helper.value.object_val;
         YAC_NEW_STACK(Complexd, helper2);
         //yac_host->printf("sqrt(1-z^2)=(%f, %f)\n",rooted->doubles[0],rooted->doubles[1]);

         vv._add_YAC_RSELF(rooted);
         //yac_host->printf("i*z+sqrt(1-z^2)=(%f, %f)\n",vv.doubles[0],vv.doubles[1]);

         _logcd_YAC_RARG(&vv,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexd *logarithm=&helper2; //(_Complexd*)helper2.value.object_val;
            res->doubles[0]=logarithm->doubles[1];
            res->doubles[1]=-logarithm->doubles[0];
//            yac_host->yacDelete(logarithm);
//         } //should not happen
//         yac_host->yacDelete(rooted);
//      } //should not happen
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asincd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _acoscd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      YAC_NEW_STACK(Complexd, root);
      YAC_NEW_STACK(Complexd, vv);
      YAC_NEW_STACK(Complexd, helper);
      res->initd(0.0,0.0);
      root.initd(1.0,0.0);
      vv=*v;
      vv._mul_YAC_RSELF(v);
      root._sub_YAC_RSELF(&vv);
      //yac_host->printf("z^2-1=(%f, %f)\n",root.doubles[0],root.doubles[1]);
      _sqrtcd_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *rooted=&helper; //(_Complexd*)helper.value.object_val;
         YAC_NEW_STACK(Complexd, helper2);
         //yac_host->printf("sqrt(z^2-1)=(%f, %f)\n",rooted->doubles[0],rooted->doubles[1]);
         vv=*v;
         vv.doubles[0]+=-rooted->doubles[1];
         vv.doubles[1]+=rooted->doubles[0];
         //yac_host->printf("z+sqrt(z^2-1)=(%f, %f)\n",rooted->doubles[0],rooted->doubles[1]);
         _logcd_YAC_RARG(&vv,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexd *logarithm=&helper2; //(_Complexd*)helper2.value.object_val;
            res->doubles[0]=logarithm->doubles[1];
            res->doubles[1]=-logarithm->doubles[0];
//            yac_host->yacDelete(logarithm);
//         } //should not happen
//         yac_host->yacDelete(rooted);
//      } //should not happen
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoscd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _acoscd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      YAC_NEW_STACK(Complexd, root);
      YAC_NEW_STACK(Complexd, vv);
      YAC_NEW_STACK(Complexd, helper);
      res->initd(0.0,0.0);
      root.initd(1.0,0.0);
      vv=*v;
      vv._mul_YAC_RSELF(v);
      root._sub_YAC_RSELF(&vv);
      //yac_host->printf("z^2-1=(%f, %f)\n",root.doubles[0],root.doubles[1]);
      _sqrtcd_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *rooted=&helper; //(_Complexd*)helper.value.object_val;
         YAC_NEW_STACK(Complexd, helper2);
         //yac_host->printf("sqrt(z^2-1)=(%f, %f)\n",rooted->doubles[0],rooted->doubles[1]);
         vv=*v;
         vv.doubles[0]+=-rooted->doubles[1];
         vv.doubles[1]+=rooted->doubles[0];
         //yac_host->printf("z+sqrt(z^2-1)=(%f, %f)\n",rooted->doubles[0],rooted->doubles[1]);
         _logcd_YAC_RARG(&vv,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexd *logarithm=&helper2; //(_Complexd*)helper2.value.object_val;
            res->doubles[0]=logarithm->doubles[1];
            res->doubles[1]=-logarithm->doubles[0];
//            yac_host->yacDelete(logarithm);
//         } //should not happen
//         yac_host->yacDelete(rooted);
//      } //should not happen
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoscd_ARG Object is not of type Complexd");
   }
}

void YAC_CALL _atancd_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = YAC_NEW_POOLED(Complexd);
      YAC_NEW_STACK(Complexd, vv);
      YAC_NEW_STACK(Complexd, nom);
      YAC_NEW_STACK(Complexd, denom);
      YAC_NEW_STACK(Complexd, helper);
      res->initd(0.0,0.0);
      vv.initd(-v->doubles[1],v->doubles[0]);
      nom.initd(1.0,0.0);
      nom._add_YAC_RSELF(&vv);
      denom.initd(1.0,0.0);
      denom._sub_YAC_RSELF(&vv);
      nom._div_YAC_RSELF(&denom);
      _logcd_YAC_RARG(&nom,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *logarithm=&helper; //(_Complexd*)helper.value.object_val;
         res->doubles[0]=0.5*logarithm->doubles[1];
         res->doubles[1]=-0.5*logarithm->doubles[0];
//         yac_host->yacDelete(logarithm);
//      } //should not happen
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atancd_VAL Object is not of type Complexd");
   }
}

void YAC_CALL _atancd_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexd)&&YAC_CHK(_r,clid_Complexd)) {
      _Complexd *v=(_Complexd*)_v;
      _Complexd *res = (_Complexd*)_r;
      YAC_NEW_STACK(Complexd, vv);
      YAC_NEW_STACK(Complexd, nom);
      YAC_NEW_STACK(Complexd, denom);
      YAC_NEW_STACK(Complexd, helper);
      res->initd(0.0,0.0);
      vv.initd(-v->doubles[1],v->doubles[0]);
      nom.initd(1.0,0.0);
      nom._add_YAC_RSELF(&vv);
      denom.initd(1.0,0.0);
      denom._sub_YAC_RSELF(&vv);
      nom._div_YAC_RSELF(&denom);
      _logcd_YAC_RARG(&nom,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexd *logarithm=&helper; //(_Complexd*)helper.value.object_val;
         res->doubles[0]=0.5*logarithm->doubles[1];
         res->doubles[1]=-0.5*logarithm->doubles[0];
//         yac_host->yacDelete(logarithm);
//      } //should not happen
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atancd_ARG Object is not of type Complexd");
   }
}

#endif
