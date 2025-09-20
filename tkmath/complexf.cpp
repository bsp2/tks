/// complexf.cpp
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
#include "vector2f.h"
#include "complexf.h"

#include "ying_Math.h"

_Complexf& _Complexf::operator=(_Complexf& obj) {
   floats[0]=obj.floats[0];
   floats[1]=obj.floats[1];
   return *this;
}

_Complexf::_Complexf(void) {
   floats[0]=floats[1]=0.0f;
}

_Complexf::~_Complexf() {
}

sUI YAC_VCALL _Complexf::yacArrayGetNumElements(void) {
   return 2;
}

sUI YAC_VCALL _Complexf::yacArrayGetMaxElements(void) {
   return 2;
}

sUI YAC_VCALL _Complexf::yacArrayGetElementType(void) {
   return YAC_TYPE_FLOAT;
}

sUI YAC_VCALL _Complexf::yacArrayGetElementByteSize(void) {
   return sizeof(sF32);
}

void YAC_VCALL _Complexf::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if (_idx<2) {
      sF32 v;
      switch (_val->type) {   
         case YAC_TYPE_VOID:
            v=0.0f;
            break;
         case YAC_TYPE_INT:
            v=(sF32)_val->value.int_val;
            break;
         case YAC_TYPE_FLOAT:
            v=_val->value.float_val;
            break;
         case YAC_TYPE_OBJECT:
            if (YAC_VALID(_val->value.object_val)) {
               if (!_val->value.object_val->yacScanF32(&v))
                  v=0.0f;
            } else v=0.0f;
            break;
         default:
            Dyac_throw(TypeMismatch, "tkmath::Complexf::yacArraySet Unknown Type!");
            v=0.0f;
            break;
      }
      floats[_idx]=v;
   } else {
      static char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Complexf::yacArraySet Index too high (%d>1)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Complexf::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if (_idx<2) {
      _val->initFloat(floats[_idx]);
   } else {
      static char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Complexf::yacArrayGet Index too high (%d>1)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
      _val->initFloat(0.0f);
   }
}

void *YAC_VCALL _Complexf::yacArrayGetPointer(void) {
   return (void*)floats;
}

sSI YAC_VCALL _Complexf::yacTensorRank(void) {
   return YAC_TENSOR_RANK_VECTOR;
}

void YAC_VCALL _Complexf::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F32(floats[0]);
   YAC_SERIALIZE_F32(floats[1]);
}

sUI YAC_VCALL _Complexf::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   floats[0]=YAC_DESERIALIZE_F32();
   floats[1]=YAC_DESERIALIZE_F32();
   return 1;
}

void YAC_VCALL _Complexf::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if (YAC_CHK(_min,clid_Complexf)&&YAC_CHK(_max,clid_Complexf)) {
      _Complexf *min=(_Complexf*)_min;
      _Complexf *max=(_Complexf*)_max;
      if (floats[0]<min->floats[0]) floats[0]=min->floats[0];
      else if (floats[0]>max->floats[0]) floats[0]=max->floats[0];
      if (floats[1]<min->floats[1]) floats[1]=min->floats[1];
      else if (floats[1]>max->floats[1]) floats[1]=max->floats[1];
   } 
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "expected Complexf object");
   }
}

void YAC_VCALL _Complexf::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if (YAC_CHK(_min,clid_Complexf)&&YAC_CHK(_max,clid_Complexf)) {
      _Complexf *min=(_Complexf*)_min;
      _Complexf *max=(_Complexf*)_max;
      if (floats[0]<min->floats[0]) floats[0]+=(max->floats[0]-min->floats[0]);
      else if (floats[0]>max->floats[0]) floats[0]-=(max->floats[0]-min->floats[0]);
      if (floats[1]<min->floats[1]) floats[1]+=(max->floats[1]-min->floats[1]);
      else if (floats[1]>max->floats[1]) floats[1]-=(max->floats[1]-min->floats[1]);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "expected Complexf object");
   }
}

void YAC_VCALL _Complexf::yacOperator(sSI _cmd,YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      switch (_cmd) {
         case YAC_OP_CEQ:
            YAC_RETI( Dfltequal(floats[0], o->floats[0]) && 
                      Dfltequal(floats[1], o->floats[1]) );
            break;
         case YAC_OP_CNE:
            YAC_RETI( Dfltnotequal(floats[0], o->floats[0]) || 
                      Dfltnotequal(floats[1], o->floats[1]) );
            break;
         case YAC_OP_ASSIGN:
            floats[0]=o->floats[0];
            floats[1]=o->floats[1];
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
            floats[0]=-floats[0];
            floats[1]=-floats[1];
            break;
         case YAC_OP_INIT:
            floats[0]=0.0f;
            floats[1]=0.0f;
            break;
         default:
         {
            static char buf[256];
            Dyac_snprintf(buf, 256, "tkmath::Complexf::yacOperator: unsupported cmd %d!\n", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
            return;
         }
      }
   } else if (YAC_Is_FloatArray(_o)) {
      YAC_FloatArray *o=(YAC_FloatArray*)_o;
      switch (_cmd) {
         case YAC_OP_ASSIGN:
            if (o->num_elements<2) {
               if (o->num_elements==1) floats[0]=o->elements[0];
               else floats[0]=0.0f;
               floats[1]=0.0f;
            } else {
               floats[0]=o->elements[0];
               floats[1]=o->elements[1];
            }
            break;
         default:
            YAC_NEW_STACK(Complexf, t);
            t.assignFloatArray(o);
            yacOperator(_cmd, &t, _r);
//            Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Complexf::yacOperator Only Assign supported with FloatArray Type");
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
	    Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Complexf::yacOperator: Only Assign supported with generic Type");
	    break;
      }
   } else {
      switch (_cmd) {
         case YAC_OP_NEG:
	    floats[0]=-floats[0];
	    floats[1]=-floats[1];
	    break;
	 default:
            YAC_Object::yacOperator(_cmd,_o,_r);
      }
   }
}

void YAC_VCALL _Complexf::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF32(_cmd, (sF32)val, _r);
}

void YAC_VCALL _Complexf::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   switch (_cmd) {
      case YAC_OP_ASSIGN:
         floats[0] = val;
         floats[1] = 0;
         break;
      case YAC_OP_CEQ:
         YAC_RETI(Dfltequal( floats[0], val ));
         break;
      case YAC_OP_CNE:
         YAC_RETI(Dfltnotequal( floats[0], val ));
         break;
      case YAC_OP_CLT:
         YAC_RETI( floats[0] < val );
         break;
      case YAC_OP_CLE:
         YAC_RETI( floats[0] <= val );
         break;
      case YAC_OP_CGT:
         YAC_RETI( floats[0] > val );
         break;
      case YAC_OP_CGE:
         YAC_RETI( floats[0] >= val );
         break;
      case YAC_OP_ADD:
         floats[0] += val;
         break;
      case YAC_OP_SUB:
         floats[0] -= val;
         break;
      case YAC_OP_MUL:
         _mulf_YAC_RSELF(val);
         break;
      case YAC_OP_DIV:
         if (Dfltnonzero(val)) {
            _mulf_YAC_RSELF(1.0f/val);
         } else {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::yacOperatorF32 OP_DIV Cannot divide by zero");
         }
         break;
      default:
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Complexf::yacOperatorF32 Unsupported Operator %d",_cmd);
         Dyac_throw_def(TkMathUnsupportedOperator, buf);
         break;
   }
}

void YAC_VCALL _Complexf::yacValueOfI(sSI val) {
   floats[0] = (sF32) val;
}

void YAC_VCALL _Complexf::yacValueOfI64(sS64 val) {
   floats[0] = (sF32) val;
}

void YAC_VCALL _Complexf::yacValueOfF32(sF32 val) {
   floats[0] = val;
}

void YAC_VCALL _Complexf::yacValueOfF64(sF64 val) {
   floats[0] = (sF32) val;
}

sBool YAC_VCALL _Complexf::yacScanI(sSI *val) {
   //lrintf can be 64bit (on gcc 64bit), but can also be 32bit
   //*val = (sSI) lrintf(floats[0]);
   *val = (sSI) ( (floats[0]<0) ? (floats[0] - 0.5f) : (floats[0] + 0.5f) );
   return true;
}

sBool YAC_VCALL _Complexf::yacScanI64(sS64 *val) {
   //llrintf is 64bit
   //*val = (sS64) llrintf(floats[0]);
   *val = (sS64) ( (floats[0]<0) ? (floats[0] - 0.5f) : (floats[0] + 0.5f) );
   return true;
}

sBool YAC_VCALL _Complexf::yacScanF32(sF32 *val) {
   *val = floats[0];
   return true;
}

sBool YAC_VCALL _Complexf::yacScanF64(sF64 *val) {
   *val = (sF64) floats[0];
   return true;
}

void _Complexf::assignFloats(sF32 *_fv, sUI _ne) {
   if(_ne <= 2) {
      // Copy as much elements from array as possible but never more than 2
      sUI i;
      for(i=0; i<_ne; i++) {
         floats[i] = _fv[i];
      }
      // Fill the rest with 0.0f
      for(; i<2; i++) {
         floats[i] = 0.0f;
      }
   } else {
      // Copy two elements
      floats[0] = _fv[0];
      floats[1] = _fv[1];
   }
}

void _Complexf::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Complexf::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
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
               floats[i] = v.value.float_val;
            }
            // Fill the rest with 0.0f
            for(; i<2; i++)
            {
               floats[i] = 0.0f;
            }
         }
      }
      else
      {
         // Retrieve individual elements, typecast to float and assign
         YAC_Value v;
         sUI i;
         for(i=0; (i<2) && (i<ne); i++)
         {
            _o->yacArrayGet(_ctx, i, &v);
            v.typecast(YAC_TYPE_FLOAT);
            floats[i] = v.value.float_val;
            v.unset();
         }
         // Fill the rest with 0.0f
	 for (;i<2;i++) 
	    floats[i] = 0.0f;
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      // Get the scalar real value if possible and fill the rest with 0.0f
      if (!_o->yacScanF32(&floats[0])) 
         floats[0] = 0.0f;
      floats[1] = 0.0f;
   }
}

//direct element access methods
void _Complexf::_setX(sF32 x) {
   floats[0]=x;
}

void _Complexf::_setY(sF32 y) {
   floats[1]=y;
}

sF32 _Complexf::_getX(void) {
   return floats[0];
}

sF32 _Complexf::_getY(void) {
   return floats[1];
}

//access polar representation
//radius/length
void _Complexf::_setR(sF32 r) {
   _unitScale_YAC_RSELF(r);
}

//angle
void _Complexf::_setA(sF32 a) {
   sF32 absVal=_getAbs();
   floats[0]=absVal*cosf(a);
   floats[1]=absVal*sinf(a);
}

//radius/length
sF32 _Complexf::_getR(void) {
   return _getAbs();
}

//angle
sF32 _Complexf::_getA(void) {
   return atan2f(floats[1],floats[0]);
}

//generate string representation
sBool YAC_VCALL _Complexf::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(1024,0,0,0);
   s->printf("(%g+i*%g)",floats[0],floats[1]);
   s->fixLength();
   return 1;
}

void _Complexf::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   yacToString(s);
   YAC_RETS(s,1);
}

void _Complexf::_getPolarString(YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   s->yacArrayAlloc(1024,0,0,0);
   s->printf("%g*exp(i*%g)",_getR(),_getA());
   YAC_RETS(s,1);
}

//add two complex numbers
void _Complexf::_add_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      floats[0]+=o->floats[0];
      floats[1]+=o->floats[1];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::add_SELF Object not of Type Complexf");
   }
}

void _Complexf::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      _Complexf *r=YAC_NEW_POOLED(Complexf);
      r->floats[0]=floats[0]+o->floats[0];
      r->floats[1]=floats[1]+o->floats[1];
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::add_VAL Object not of Type Complexf");
   }
}

void _Complexf::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      _Complexf *r=(_Complexf*)_r;
      r->floats[0]=floats[0]+o->floats[0];
      r->floats[1]=floats[1]+o->floats[1];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::add_ARG Object not of Type Complexf");
   }
}

//sub two complex numbers
void _Complexf::_sub_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      floats[0]-=o->floats[0];
      floats[1]-=o->floats[1];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::sub_SELF Object not of Type Complexf");
   }
}

void _Complexf::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      _Complexf *r=YAC_NEW_POOLED(Complexf);
      r->floats[0]=floats[0]-o->floats[0];
      r->floats[1]=floats[1]-o->floats[1];
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::sub_VAL Object not of Type Complexf");
   }
}

void _Complexf::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      _Complexf *r=(_Complexf*)_r;
      r->floats[0]=floats[0]-o->floats[0];
      r->floats[1]=floats[1]-o->floats[1];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::sub_SELF Object not of Type Complexf");
   }
}

//multiply with scalar value
void _Complexf::_mulf_YAC_RSELF(sF32 s) {
   floats[0]*=s;
   floats[1]*=s;
}

void _Complexf::_mulf_YAC_RVAL(sF32 s, YAC_Value *_r) {
   _Complexf *r=YAC_NEW_POOLED(Complexf);
   r->floats[0]=floats[0]*s;
   r->floats[1]=floats[1]*s;
   _r->initObject(r,1);
}

void _Complexf::_mulf_YAC_RARG(sF32 s, YAC_Object *_r) {
   if (!YAC_CHK(_r, clid_Complexf)) {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::mulf_ARG Return Object not of type Complexf");
   }
   _Complexf *r=(_Complexf*)_r;
   r->floats[0]=floats[0]*s;
   r->floats[1]=floats[1]*s;
}

//complex multiplication
//(a+ib)*(c+id)=(ac-bd)+i*(ad+bc)
void _Complexf::_mul_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      sF32 a,b;
      a=floats[0]*o->floats[0]-floats[1]*o->floats[1];
      b=floats[0]*o->floats[1]+floats[1]*o->floats[0];
      floats[0]=a;
      floats[1]=b;
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::mul_SELF Object not of type Complexf");
   }
}

void _Complexf::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      _Complexf *r=YAC_NEW_POOLED(Complexf);
      r->floats[0]=floats[0]*o->floats[0]-floats[1]*o->floats[1];
      r->floats[1]=floats[0]*o->floats[1]+floats[1]*o->floats[0];
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::mul_VAL Object not of type Complexf");
   }
}

void _Complexf::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      _Complexf *r=(_Complexf*)_r;
      r->floats[0]=floats[0]*o->floats[0]-floats[1]*o->floats[1];
      r->floats[1]=floats[0]*o->floats[1]+floats[1]*o->floats[0];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::mul_ARG Object not of type Complexf");
   }
}

//multiply with complex conjugated number x-iy, returns scalar/real number
sF32 _Complexf::_mulConj(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      return (floats[0]*o->floats[0]+floats[1]*o->floats[1]);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::mulConj Object not of type Complexf");
   }
   return 0.0f;
}

//complex division
//(a+ib)/(c+id)=(a+ib)*(c-id)/(c^2+d^2)=((ac+bd)+i*(-ad+bc))/(c^2+d^2)
void _Complexf::_div_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      sF32 a,b,c;
      a=floats[0]*o->floats[0]+floats[1]*o->floats[1];
      b=-floats[0]*o->floats[1]+floats[1]*o->floats[0];
      c=o->floats[0]*o->floats[0]+o->floats[1]*o->floats[1];
      if (c!=0.0f) {
         floats[0]=a/c;
         floats[1]=b/c;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::div_SELF Division by Zero");
         floats[0]=0.0f;
         floats[1]=0.0f;
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::div_SELF Object not of Type Complexf");
   }
}

void _Complexf::_div_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      sF32 a,b,c;
      a=floats[0]*o->floats[0]+floats[1]*o->floats[1];
      b=-floats[0]*o->floats[1]+floats[1]*o->floats[0];
      c=o->floats[0]*o->floats[0]+o->floats[1]*o->floats[1];
      if (c!=0.0f) {
         _Complexf *r=YAC_NEW_POOLED(Complexf);
         r->floats[0]=a/c;
         r->floats[1]=b/c;
         _r->initObject(r,1);
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::div_VAL Division by Zero");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::div_VAL Object not of Type Complexf");
   }
}

void _Complexf::_div_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      _Complexf *r=(_Complexf*)_r;
      sF32 a,b,c;
      a=floats[0]*o->floats[0]+floats[1]*o->floats[1];
      b=-floats[0]*o->floats[1]+floats[1]*o->floats[0];
      c=o->floats[0]*o->floats[0]+o->floats[1]*o->floats[1];
      if (c!=0.0f) {
         r->floats[0]=a/c;
         r->floats[1]=b/c;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::div_ARG Division by Zero");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::div_ARG Object not of Type Complexf");
   }
}

//calcalute 1/z=z/(z*z_conj)
void _Complexf::_invert_YAC_RSELF(void) {
   sF32 denom=_getAbsSqr();
   if (denom!=0.0f) {
      floats[0]=floats[0]/denom;
      floats[1]=-floats[1]/denom;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::invert_SELF Division by Zero, cannot invert Zero Vector");
   }
}

void _Complexf::_invert_YAC_RVAL(YAC_Value *_r) {
   sF32 denom=_getAbsSqr();
   if (denom!=0.0f) {
      _Complexf *r=YAC_NEW_POOLED(Complexf);
      r->floats[0]=floats[0]/denom;
      r->floats[1]=-floats[1]/denom;
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::invert_VAL Division by Zero, cannot invert Zero Vector");
   }
}

void _Complexf::_invert_YAC_RARG(YAC_Object *_r) {
   if (!YAC_CHK(_r,clid_Complexf)) {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf:invert_ARG Return object not of type Complexf");
   } else {
      _Complexf *r=(_Complexf*)_r;
      sF32 denom=_getAbsSqr();
      if (denom!=0.0f) {
         r->floats[0]=floats[0]/denom;
         r->floats[1]=-floats[1]/denom;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::invert_ARG Division by Zero, cannot invert Zero Vector");
      }
   }
}

//return length of complex number/radius
sF32 _Complexf::_getAbs(void) {
#ifdef YAC_GCC
   // C99?
   return hypotf(floats[0],floats[1]);
#else
   // ISO-C++
#ifdef HAVE_HYPOTF
   return _hypotf(floats[0],floats[1]);
#else
   return hypotf_emul(floats[0],floats[1]);
#endif // HAVE_HYPOTF
#endif // YAC_GCC
   //return sqrtf(floats[0]*floats[0]+floats[1]*floats[1]);
}

//return squared length
sF32 _Complexf::_getAbsSqr(void) {
   return (floats[0]*floats[0]+floats[1]*floats[1]);
}

//scale complex number to length 1
void _Complexf::_unit_YAC_RSELF(void) {
   sF32 absval=_getAbs();
   if (Dfltnonzero(absval)) {
      floats[0]/=absval;
      floats[1]/=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::unit_SELF Zero Vector cannot be normalized");
      floats[0]=floats[1]=0.0f; //this may change to x=1.0f, y=0.0f
   }
}

void _Complexf::_unit_YAC_RVAL(YAC_Value *_r) {
   sF32 absval=_getAbs();
   if (Dfltnonzero(absval)) {
      _Complexf *r=YAC_NEW_POOLED(Complexf);
      r->floats[0]=floats[0]/absval;
      r->floats[1]=floats[1]/absval;
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::unit Zero Vector cannot be normalized");
   }
}

void _Complexf::_unit_YAC_RARG(YAC_Object *_r) {
   if (!YAC_CHK(_r,clid_Complexf)) {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::unit_ARG Return object not of type Complexf");
   } else {
      _Complexf *r=(_Complexf*)_r;
      sF32 absval=_getAbs();
      if (Dfltnonzero(absval)) {
         r->floats[0]=floats[0]/absval;
         r->floats[1]=floats[1]/absval;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::unit_ARG Zero Vector cannot be normalized");
      }
   }
}

//scale complex number to length s
void _Complexf::_unitScale_YAC_RSELF(sF32 s) {
   sF32 absval=_getAbs();
   if (Dfltnonzero(absval)) {
      absval=s/absval;
      floats[0]*=absval;
      floats[1]*=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::unitScale_SELF Zero Vector cannot be normalized");
      floats[0]=floats[1]=0.0f; //this may change to x=1.0f, y=0.0f
   }

//second version
//which is faster?
/*
   sF32 angle=_getA();
   floats[0]=s*cos(angle);
   floats[1]=s*sin(angle);*/
}

void _Complexf::_unitScale_YAC_RVAL(sF32 s, YAC_Value *_r) {
   sF32 absval=_getAbs();
   if (Dfltnonzero(absval)) {
      _Complexf *r=YAC_NEW_POOLED(Complexf);
      absval=s/absval;
      r->floats[0]=floats[0]*absval;
      r->floats[1]=floats[1]*absval;
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::unitScale_VAL Zero Vector cannot be normalized");
   }

//second version
//which is faster?
/*
   sF32 angle=_getA();
   floats[0]=s*cos(angle);
   floats[1]=s*sin(angle);*/
}

void _Complexf::_unitScale_YAC_RARG(sF32 s, YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Complexf)) {
      sF32 absval=_getAbs();
      if (Dfltnonzero(absval)) {
         _Complexf *r=(_Complexf*)_r;
         absval=s/absval;
         r->floats[0]=floats[0]*absval;
         r->floats[1]=floats[1]*absval;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Complexf::unitScale_ARG Zero Vector cannot be normalized");
      }

//second version
//which is faster?
/*
   sF32 angle=_getA();
   floats[0]=s*cos(angle);
   floats[1]=s*sin(angle);*/
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Complexf::unitScale_ARG Return object not of type Complexf");
   }
}

//initialisation methods
void _Complexf::_init(sF32 x, sF32 y) {
   floats[0]=x;
   floats[1]=y;
}

void _Complexf::_initPolar(sF32 r, sF32 a) {
   floats[0]=r*cosf(a);
   floats[1]=r*sinf(a);
}

void _Complexf::_New(sF32 x, sF32 y, YAC_Value *_r) {
   _Complexf *r=YAC_NEW_POOLED(Complexf);
   r->_init(x,y);
   _r->initObject(r,1);
}

void _Complexf::_NewPolar(sF32 radius, sF32 angle, YAC_Value *_r) {
   _Complexf *r=YAC_NEW_POOLED(Complexf);
   r->_initPolar(radius,angle);
   _r->initObject(r,1);
}

void YAC_CALL complexf(sF32 x, sF32 y, YAC_Value *_r) {
   _Complexf *r=YAC_NEW_POOLED(Complexf);
   r->_init(x,y);
   _r->initObject(r,1);
}

void YAC_CALL complexfPolar(sF32 x, sF32 y, YAC_Value *_r) {
   _Complexf *r=YAC_NEW_POOLED(Complexf);
   r->_initPolar(x,y);
   _r->initObject(r,1);
}

//additional trigonometric and hyperbolic functions
#ifdef DX_USE_C99_COMPLEX
//this version uses native complex data type
//
//use native sqrt
void YAC_CALL _sqrtcf_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(o->floats[0],o->floats[1]);
      z=std::sqrt(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sqrtcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _sqrtcf_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *o=(_Complexf*)_o;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(o->floats[0],o->floats[1]);
      z=std::sqrt(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sqrtcf_ARG Object is not of type Complexf");
   }
}


//use native pow
void YAC_CALL _powcf_YAC_RVAL(YAC_Object *_b, YAC_Object *_e, YAC_Value *_r) {
   if (YAC_CHK(_b,clid_Complexf)&&YAC_CHK(_e,clid_Complexf)) {
      _Complexf *b=(_Complexf*)_b;
      _Complexf *e=(_Complexf*)_e;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> bc(b->floats[0],b->floats[1]),ec(e->floats[0],e->floats[1]);
      std::complex<float> z;
      z=std::pow(bc,ec);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _powcf_YAC_RARG(YAC_Object *_b, YAC_Object *_e, YAC_Object *_r) {
   if (YAC_CHK(_b,clid_Complexf)&&YAC_CHK(_e,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *b=(_Complexf*)_b;
      _Complexf *e=(_Complexf*)_e;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> bc(b->floats[0],b->floats[1]),ec(e->floats[0],e->floats[1]);
      std::complex<float> z;
      z=std::pow(bc,ec);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powcf_ARG Object is not of type Complexf");
   }
}

//same as before, but you can select the other solutions at wish
//no implementation in stdc complex.h
void YAC_CALL _powncf_YAC_RVAL(YAC_Object *_b, YAC_Object *_e, YAC_Object *_n, YAC_Value *_r) {
   if (YAC_CHK(_b,clid_Complexf)&&YAC_CHK(_e,clid_Complexf)&&YAC_CHK(_n,clid_Complexf)) {
      _Complexf *b=(_Complexf*)_b;
      _Complexf *e=(_Complexf*)_e;
      _Complexf *n=(_Complexf*)_n;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      sF32 r,a;
      sF32 lnra = (sF32) (logf(b->_getR())-n->floats[1]*2.0f*M_PI);
      sF32 aa = (sF32) (b->_getA()+n->floats[0]*2.0f*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      r=expf(e->floats[0]*lnra-e->floats[1]*aa);
      a=e->floats[0]*aa+e->floats[1]*lnra;
      res->floats[0]=r*cos(a);
      res->floats[1]=r*sin(a);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powncf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _powncf_YAC_RARG(YAC_Object *_b, YAC_Object *_e, YAC_Object *_n, YAC_Object *_r) {
   if (YAC_CHK(_b,clid_Complexf)&&YAC_CHK(_e,clid_Complexf)&&YAC_CHK(_n,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *b=(_Complexf*)_b;
      _Complexf *e=(_Complexf*)_e;
      _Complexf *n=(_Complexf*)_n;
      _Complexf *res = (_Complexf*)_r;
      sF32 r,a;
      sF32 lnra = (sF32) (logf(b->_getR())-n->floats[1]*2.0f*M_PI);
      sF32 aa = (sF32) (b->_getA()+n->floats[0]*2.0f*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      r=expf(e->floats[0]*lnra-e->floats[1]*aa);
      a=e->floats[0]*aa+e->floats[1]*lnra;
      res->floats[0]=r*cos(a);
      res->floats[1]=r*sin(a);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powncf_ARG Object is not of type Complexf");
   }
}

//use native exp
void YAC_CALL _expcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::exp(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::expcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _expcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::exp(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::expcf_ARG Object is not of type Complexf");
   }
}

//use native log
void YAC_CALL _logcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::log(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag(); //+2*M_PI*n with n=0,1,2,3,...
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _logcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::log(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag(); //+2*M_PI*n with n=0,1,2,3,...
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logcf_ARG Object is not of type Complexf");
   }
}

//ln(z)=ln(r_z)+i*(a_z+2*pi*n)=ln(r_z)-y_n*2*pi+i*(a_z+x_n*2*pi), this one with n!=0
//no implementation in stdc complex.h
void YAC_CALL _logncf_YAC_RVAL(YAC_Object *_v, YAC_Object *_n,YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_n,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *n=(_Complexf*)_n;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      res->floats[0] = (sF32) (logf(v->_getR())-n->floats[1]*2.0f*M_PI);
      res->floats[1] = (sF32) (v->_getA()+n->floats[0]*2.0f*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logncf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _logncf_YAC_RARG(YAC_Object *_v, YAC_Object *_n,YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_n,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *n=(_Complexf*)_n;
      _Complexf *res = (_Complexf*)_r;
      res->floats[0] = (sF32) (logf(v->_getR())-n->floats[1]*2.0f*M_PI);
      res->floats[1] = (sF32) (v->_getA()+n->floats[0]*2.0f*M_PI); //+2*M_PI*n with n=0,1,2,3,...
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logncf_ARG Object is not of type Complexf");
   }
}

//use native sin
void YAC_CALL _sincf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::sin(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sincf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _sincf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::sin(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sincf_ARG Object is not of type Complexf");
   }
}

//use native cos
void YAC_CALL _coscf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::cos(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coscf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _coscf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::cos(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coscf_ARG Object is not of type Complexf");
   }
}

//use native tan
void YAC_CALL _tancf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::tan(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tancf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _tancf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::tan(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tancf_ARG Object is not of type Complexf");
   }
}

//use native sinh
void YAC_CALL _sinhcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::sinh(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sinhcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _sinhcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::sinh(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sinhcf_ARG Object is not of type Complexf");
   }
}

//use native cosh
void YAC_CALL _coshcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::cosh(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coshcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _coshcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::cosh(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coshcf_ARG Object is not of type Complexf");
   }
}

//use native tanh
void YAC_CALL _tanhcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::tanh(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tanhcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _tanhcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      z=std::tanh(z);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tanhcf_ARG Object is not of type Complexf");
   }
}

//there is noe native invert method for the neither trigonometric nor hyperbolic functions
//
//asinh=log(z+sqrt(z*z+1))
void YAC_CALL _asinhcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      //z=std::asinh(z);  //hooray, this does not exist, so do the calc manually
      //this is true for all arcus functions
      std::complex<float> val;
      val=z*z+1.0f;
      val=sqrt(val);
      val=z+val;
      z=log(val);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1); 
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asinhcf_VAL Object is not of type Complexf");
   }   
}

void YAC_CALL _asinhcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      //z=std::asinh(z);  //hooray, this does not exist, so do the calc manually
      //this is true for all arcus functions
      std::complex<float> val;
      val=z*z+1.0f;
      val=sqrt(val);
      val=z+val;
      z=log(val);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asinhcf_ARG Object is not of type Complexf");
   }   
}

//acosh=log(z+sqrt(z*z-1))
void YAC_CALL _acoshcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      //z=std::acosh(z); //see asinhcf
      std::complex<float> val;
      val=z*z-1.0f;
      val=sqrt(val);
      val=z+val;
      z=log(val);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1); 
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoshcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _acoshcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      //z=std::acosh(z); //see asinhcf
      std::complex<float> val;
      val=z*z-1.0f;
      val=sqrt(val);
      val=z+val;
      z=log(val);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoshcf_ARG Object is not of type Complexf");
   }
}

//atanh=0.5*log((z+1)/(z-1))
void YAC_CALL _atanhcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      //z=std::atanh(z); //see asinhcf
      std::complex<float> nom,denom;
      nom=1.0f+z;
      denom=1.0f-z;
      //yac_host->printf("nominator: (%f, %f) denominator: (%f, %f)\n",
      //   nom.real(),nom.imag(),denom.real(),denom.imag());
      nom/=denom;
      //yac_host->printf("nominator/denominator=(%f, %f)\n",nom.real(),nom.imag());
      z=0.5f*log(nom);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atanhcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _atanhcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      //z=std::atanh(z); //see asinhcf
      std::complex<float> nom,denom;
      nom=1.0f+z;
      denom=1.0f-z;
      //yac_host->printf("nominator: (%f, %f) denominator: (%f, %f)\n",
      //   nom.real(),nom.imag(),denom.real(),denom.imag());
      nom/=denom;
      //yac_host->printf("nominator/denominator=(%f, %f)\n",nom.real(),nom.imag());
      z=0.5f*log(nom);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atanhcf_ARG Object is not of type Complexf");
   }
}

//asin=-i*log(i*z+sqrt(1-z*z))
void YAC_CALL _asincf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      //z=std::asin(z);
      std::complex<float> val;
      val=1.0f-z*z;
      //yac_host->printf("1-z^2=(%f, %f)\n",val.real(),val.imag());
      val=sqrt(val);
      //yac_host->printf("sqrt(1-z^2)=(%f, %f)\n",val.real(),val.imag());
      val=std::complex<float>(0.0f,1.0f)*z+val;
      //std::complex<float> testval=std::complex<float>(0.0f,1.0f)*z;
      //yac_host->printf("i*z=(%f,%f)\n",testval.real(),testval.imag());
      //yac_host->printf("i*z+sqrt(1-z^2)=(%f, %f)\n",val.real(),val.imag());
      z=std::complex<float>(0.0f,-1.0f)*log(val);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asincf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _asincf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      //z=std::asin(z);
      std::complex<float> val;
      val=1.0f-z*z;
      //yac_host->printf("1-z^2=(%f, %f)\n",val.real(),val.imag());
      val=sqrt(val);
      //yac_host->printf("sqrt(1-z^2)=(%f, %f)\n",val.real(),val.imag());
      val=std::complex<float>(0.0f,1.0f)*z+val;
      //std::complex<float> testval=std::complex<float>(0.0f,1.0f)*z;
      //yac_host->printf("i*z=(%f,%f)\n",testval.real(),testval.imag());
      //yac_host->printf("i*z+sqrt(1-z^2)=(%f, %f)\n",val.real(),val.imag());
      z=std::complex<float>(0.0f,-1.0f)*log(val);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asincf_ARG Object is not of type Complexf");
   }
}

//acos=-i*log(z+i*sqrt(1-z*z))
void YAC_CALL _acoscf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(v->floats[0],v->floats[1]);
      //z=std::acos(z); //see asinhcf
      std::complex<float> val;
      val=1.0f-z*z;
      //yac_host->printf("z^2-1=(%f, %f)\n",val.real(),val.imag());
      val=std::complex<float>(0.0f,1.0f)*sqrt(val);
      //yac_host->printf("sqrt(z^2-1)=(%f, %f)\n",val.real(),val.imag());
      val=z+val;
      //yac_host->printf("z+sqrt(z^2-1)=(%f, %f)\n",val.real(),val.imag());
      z=std::complex<float>(0.0f,-1.0f)*log(val);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoscf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _acoscf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(v->floats[0],v->floats[1]);
      //z=std::acos(z); //see asinhcf
      std::complex<float> val;
      val=1.0f-z*z;
      //yac_host->printf("z^2-1=(%f, %f)\n",val.real(),val.imag());
      val=std::complex<float>(0.0f,1.0f)*sqrt(val);
      //yac_host->printf("sqrt(z^2-1)=(%f, %f)\n",val.real(),val.imag());
      val=z+val;
      //yac_host->printf("z+sqrt(z^2-1)=(%f, %f)\n",val.real(),val.imag());
      z=std::complex<float>(0.0f,-1.0f)*log(val);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoscf_ARG Object is not of type Complexf");
   }
}

//atan=-(i/2)*log((1+i*z)/(1-i*z))
void YAC_CALL _atancf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      std::complex<float> z(-v->floats[1],v->floats[0]); //i*z
      //z=std::atan(z); //see asinhcf
      std::complex<float> nom,denom;
      nom=1.0f+z;
      denom=1.0f-z;
      nom/=denom;
      z=std::complex<float>(0.0f,-0.5f)*log(nom);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atancf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _atancf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      std::complex<float> z(-v->floats[1],v->floats[0]); //i*z
      //z=std::atan(z); //see asinhcf
      std::complex<float> nom,denom;
      nom=1.0f+z;
      denom=1.0f-z;
      nom/=denom;
      z=std::complex<float>(0.0f,-0.5f)*log(nom);
      res->floats[0]=z.real();
      res->floats[1]=z.imag();
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atancf_ARG Object is not of type Complexf");
   }
}

#else 
//this version uses the supplied data type from the plugin
//this may be slower but is more portable
//
//sqrt(z)=sqrt(r*exp(i*a))=sqrt(r)*exp(i*a/2.0)
void YAC_CALL _sqrtcf_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if (YAC_CHK(_o,clid_Complexf)) {
      sF32 r,a;
      _Complexf *o=(_Complexf*)_o;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      r=sqrtf(o->_getR());
      a=o->_getA()/2.0f;
      res->floats[0]=r*cosf(a);
      res->floats[1]=r*sinf(a);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sqrtcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _sqrtcf_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      sF32 r,a;
      _Complexf *o=(_Complexf*)_o;
      _Complexf *res = (_Complexf*)_r;
      r=sqrtf(o->_getR());
      a=o->_getA()/2.0f;
      res->floats[0]=r*cosf(a);
      res->floats[1]=r*sinf(a);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sqrtcf_ARG Object is not of type Complexf");
   }
}

//b^a=(r*exp(i*a))^(x+iy)=(r^x+exp(log(r)*iy))*exp(i*a*(x+iy))a
//=exp(x_z*(ln(r_a)+i*a_a))*exp(i*y_z*(ln(r_a)+i*a_a))
//=exp(x_z*ln(r_a))*exp(i*x_z*a_a)*exp(i*y_z*ln(r_a))*exp(-y_z*a_a)
//=exp(x_z*ln(r_a)-y_z*a_a)*exp(i*(x_z*a_a+y_z*ln(r_a))
void YAC_CALL _powcf_YAC_RVAL(YAC_Object *_b, YAC_Object *_e, YAC_Value *_r) {
   if (YAC_CHK(_b,clid_Complexf)&&YAC_CHK(_e,clid_Complexf)) {
      _Complexf *b=(_Complexf*)_b;
      _Complexf *e=(_Complexf*)_e;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      sF32 r,a;
      sF32 lnra=logf(b->_getR());
      sF32 aa=b->_getA(); //+2*M_PI*n with n=0,1,2,3,...
      r=expf(e->floats[0]*lnra-e->floats[1]*aa);
      a=e->floats[0]*aa+e->floats[1]*lnra;
      res->floats[0]=r*cosf(a);
      res->floats[1]=r*sinf(a);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _powcf_YAC_RARG(YAC_Object *_b, YAC_Object *_e, YAC_Object *_r) {
   if (YAC_CHK(_b,clid_Complexf)&&YAC_CHK(_e,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *b=(_Complexf*)_b;
      _Complexf *e=(_Complexf*)_e;
      _Complexf *res = (_Complexf*)_r;
      sF32 r,a;
      sF32 lnra=logf(b->_getR());
      sF32 aa=b->_getA(); //+2*M_PI*n with n=0,1,2,3,...
      r=expf(e->floats[0]*lnra-e->floats[1]*aa);
      a=e->floats[0]*aa+e->floats[1]*lnra;
      res->floats[0]=r*cosf(a);
      res->floats[1]=r*sinf(a);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powcf_ARG Object is not of type Complexf");
   }
}

//same as before, but you can select the other solutions at wish
void YAC_CALL _powncf_YAC_RVAL(YAC_Object *_b, YAC_Object *_e, YAC_Object *_n, YAC_Value *_r) {
   if (YAC_CHK(_b,clid_Complexf)&&YAC_CHK(_e,clid_Complexf)&&YAC_CHK(_n,clid_Complexf)) {
      _Complexf *b=(_Complexf*)_b;
      _Complexf *e=(_Complexf*)_e;
      _Complexf *n=(_Complexf*)_n;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      sF32 r,a;
      sF32 lnra = (sF32) (logf(b->_getR())-n->floats[1]*2.0f*M_PI);
      sF32 aa = (sF32) (b->_getA()+n->floats[0]*2.0f*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      r=expf(e->floats[0]*lnra-e->floats[1]*aa);
      a=e->floats[0]*aa+e->floats[1]*lnra;
      res->floats[0]=r*cosf(a);
      res->floats[1]=r*sinf(a);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powncf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _powncf_YAC_RARG(YAC_Object *_b, YAC_Object *_e, YAC_Object *_n, YAC_Object *_r) {
   if (YAC_CHK(_b,clid_Complexf)&&YAC_CHK(_e,clid_Complexf)&&YAC_CHK(_n,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *b=(_Complexf*)_b;
      _Complexf *e=(_Complexf*)_e;
      _Complexf *n=(_Complexf*)_n;
      _Complexf *res = (_Complexf*)_r;
      sF32 r,a;
      sF32 lnra = (sF32) (logf(b->_getR())-n->floats[1]*2.0f*M_PI);
      sF32 aa = (sF32) (b->_getA()+n->floats[0]*2.0f*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      r=expf(e->floats[0]*lnra-e->floats[1]*aa);
      a=e->floats[0]*aa+e->floats[1]*lnra;
      res->floats[0]=r*cosf(a);
      res->floats[1]=r*sinf(a);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::powncf_ARG Object is not of type Complexf");
   }
}

//exp(z)=exp(x+iy)=exp(x)*exp(iy)
void YAC_CALL _expcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      sF32 r;
      r=expf(v->floats[0]);
      res->floats[0]=r*cosf(v->floats[1]);
      res->floats[1]=r*sinf(v->floats[1]);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::expcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _expcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      sF32 r;
      r=expf(v->floats[0]);
      res->floats[0]=r*cosf(v->floats[1]);
      res->floats[1]=r*sinf(v->floats[1]);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::expcf_ARG Object is not of type Complexf");
   }
}

//ln(z)=ln(r_z)+i*(a_z+2*pi*n)=ln(r_z)-y_n*2*pi+i*(a_z+x_n*2*pi), this one with n=0
void YAC_CALL _logcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      res->floats[0]=logf(v->_getR());
      res->floats[1]=v->_getA(); //+2*M_PI*n with n=0,1,2,3,...
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _logcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      res->floats[0]=logf(v->_getR());
      res->floats[1]=v->_getA(); //+2*M_PI*n with n=0,1,2,3,...
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logcf_ARG Object is not of type Complexf");
   }
}

//ln(z)=ln(r_z)+i*(a_z+2*pi*n)=ln(r_z)-y_n*2*pi+i*(a_z+x_n*2*pi), this one with n!=0
void YAC_CALL _logncf_YAC_RVAL(YAC_Object *_v, YAC_Object *_n,YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_n,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *n=(_Complexf*)_n;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      res->floats[0] = (sF32) (logf(v->_getR())-n->floats[1]*2.0f*M_PI);
      res->floats[1] = (sF32) (v->_getA()+n->floats[0]*2.0f*M_PI); //+2*M_PI*n with n=0,1,2,3,...
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logncf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _logncf_YAC_RARG(YAC_Object *_v, YAC_Object *_n,YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_n,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *n=(_Complexf*)_n;
      _Complexf *res = (_Complexf*)_r;
      res->floats[0] = (sF32) (logf(v->_getR())-n->floats[1]*2.0f*M_PI);
      res->floats[1] = (sF32) (v->_getA()+n->floats[0]*2.0f*M_PI); //+2*M_PI*n with n=0,1,2,3,...
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::logncf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _sincf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      res->floats[0]=sinf(v->floats[0])*coshf(v->floats[1]);
      res->floats[1]=cosf(v->floats[0])*sinhf(v->floats[1]);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sincf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _sincf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      res->floats[0]=sinf(v->floats[0])*coshf(v->floats[1]);
      res->floats[1]=cosf(v->floats[0])*sinhf(v->floats[1]);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sincf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _coscf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      res->floats[0]=cosf(v->floats[0])*coshf(v->floats[1]);
      res->floats[1]=-sinf(v->floats[0])*sinhf(v->floats[1]);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coscf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _coscf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      res->floats[0]=cosf(v->floats[0])*coshf(v->floats[1]);
      res->floats[1]=-sinf(v->floats[0])*sinhf(v->floats[1]);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coscf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _tancf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      sF32 denom=cosf(2.0f*v->floats[0])+coshf(2.0f*v->floats[1]);
      if (denom!=0.0f) {
         res->floats[0]=sinf(2.0f*v->floats[0])/denom;
         res->floats[1]=sinhf(2.0f*v->floats[1])/denom;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::tancf_VAL Division by Zero");
      }
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tancf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _tancf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      sF32 denom=cosf(2.0f*v->floats[0])+coshf(2.0f*v->floats[1]);
      if (denom!=0.0f) {
         res->floats[0]=sinf(2.0f*v->floats[0])/denom;
         res->floats[1]=sinhf(2.0f*v->floats[1])/denom;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::tancf_ARG Division by Zero");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tancf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _sinhcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      res->floats[0]=sinhf(v->floats[0])*cosf(v->floats[1]);
      res->floats[1]=coshf(v->floats[0])*sinf(v->floats[1]);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sinhcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _sinhcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      res->floats[0]=sinhf(v->floats[0])*cosf(v->floats[1]);
      res->floats[1]=coshf(v->floats[0])*sinf(v->floats[1]);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::sinhcf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _coshcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      res->floats[0]=coshf(v->floats[0])*cosf(v->floats[1]);
      res->floats[1]=sinhf(v->floats[0])*sinf(v->floats[1]);
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coshcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _coshcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      res->floats[0]=coshf(v->floats[0])*cosf(v->floats[1]);
      res->floats[1]=sinhf(v->floats[0])*sinf(v->floats[1]);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::coshcf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _tanhcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      sF32 denom=coshf(2.0f*v->floats[0])+cosf(2.0f*v->floats[1]);
      if (denom!=0.0f) {
         res->floats[0]=sinhf(2.0f*v->floats[0])/denom;
         res->floats[1]=sinf(2.0f*v->floats[1])/denom;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::tanhcf_VAL Division by Zero");
      }
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tanhcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _tanhcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      sF32 denom=coshf(2.0f*v->floats[0])+cosf(2.0f*v->floats[1]);
      if (denom!=0.0f) {
         res->floats[0]=sinhf(2.0f*v->floats[0])/denom;
         res->floats[1]=sinf(2.0f*v->floats[1])/denom;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::tanhcf_ARG Division by Zero");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::tanhcf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _asinhcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      YAC_NEW_STACK(Complexf, root);
      YAC_NEW_STACK(Complexf, helper);
      res->floats[0]=0.0f;
      res->floats[1]=0.0f;
      root=*v;
      root._mul_YAC_RSELF(v);
      root.floats[0]+=1.0f;
      _sqrtcf_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *rooted=&helper; //(_Complexf*)helper.value.object_val;
         YAC_NEW_STACK(Complexf, helper2);
         rooted->_add_YAC_RSELF(v);
         _logcf_YAC_RARG(rooted,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexf *logarithm=&helper2; //(_Complexf*)helper2.value.object_val;
            res->floats[0]=logarithm->floats[0];
            res->floats[1]=logarithm->floats[1];
//            yac_host->yacDelete(logarithm);
//         } //something gone bad - should not happen except memory exhaust
//         yac_host->yacDelete(rooted);
//      } //something gone bad - should not happen except memory exhaust
      _r->initObject(res,1); 
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asinhcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _asinhcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      YAC_NEW_STACK(Complexf, root);
      YAC_NEW_STACK(Complexf, helper);
      res->floats[0]=0.0f;
      res->floats[1]=0.0f;
      root=*v;
      root._mul_YAC_RSELF(v);
      root.floats[0]+=1.0f;
      _sqrtcf_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *rooted=&helper; //(_Complexf*)helper.value.object_val;
         YAC_NEW_STACK(Complexf, helper2);
         rooted->_add_YAC_RSELF(v);
         _logcf_YAC_RARG(rooted,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexf *logarithm=&helper2; //(_Complexf*)helper2.value.object_val;
            res->floats[0]=logarithm->floats[0];
            res->floats[1]=logarithm->floats[1];
//            yac_host->yacDelete(logarithm);
//         } //something gone bad - should not happen except memory exhaust
//         yac_host->yacDelete(rooted);
//      } //something gone bad - should not happen except memory exhaust
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asinhcf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _acoshcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      YAC_NEW_STACK(Complexf, root);
      YAC_NEW_STACK(Complexf, helper);
      res->floats[0]=0.0f;
      res->floats[1]=0.0f;
      root=*v;
      root._mul_YAC_RSELF(v);
      root.floats[0]-=1.0f;
      _sqrtcf_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *rooted=&helper; //(_Complexf*)helper.value.object_val;
         YAC_NEW_STACK(Complexf, helper2);
         rooted->_add_YAC_RSELF(v);
         _logcf_YAC_RARG(rooted,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexf *logarithm=&helper2; //(_Complexf*)helper2.value.object_val;
            res->floats[0]=logarithm->floats[0];
            res->floats[1]=logarithm->floats[1];
//            yac_host->yacDelete(logarithm);
//         } //something gone bad - should not happen except memory exhaust
//         yac_host->yacDelete(rooted);
//      } //something gone bad - should not happen except memory exhaust
      _r->initObject(res,1); 
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoshcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _acoshcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      YAC_NEW_STACK(Complexf, root);
      YAC_NEW_STACK(Complexf, helper);
      res->floats[0]=0.0f;
      res->floats[1]=0.0f;
      root=*v;
      root._mul_YAC_RSELF(v);
      root.floats[0]-=1.0f;
      _sqrtcf_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *rooted=&helper; //(_Complexf*)helper.value.object_val;
         YAC_NEW_STACK(Complexf, helper2);
         rooted->_add_YAC_RSELF(v);
         _logcf_YAC_RARG(rooted,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexf *logarithm=&helper2; //(_Complexf*)helper2.value.object_val;
            res->floats[0]=logarithm->floats[0];
            res->floats[1]=logarithm->floats[1];
//            yac_host->yacDelete(logarithm);
//         } //something gone bad - should not happen except memory exhaust
//         yac_host->yacDelete(rooted);
//      } //something gone bad - should not happen except memory exhaust
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoshcf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _atanhcf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      YAC_NEW_STACK(Complexf, nom);
      YAC_NEW_STACK(Complexf, denom);
      YAC_NEW_STACK(Complexf, helper);
      res->floats[0]=0.0f;
      res->floats[1]=0.0f;
      nom._init(1.0f,0.0f);
      nom._add_YAC_RSELF(v);
      denom._init(1.0f,0.0f);
      denom._sub_YAC_RSELF(v);
      //yac_host->printf("nominator: (%f, %f) denominator: (%f, %f)\n",
      //   nom.floats[0],nom.floats[1],denom.floats[0],denom.floats[1]);
      nom._div_YAC_RSELF(&denom);
      //yac_host->printf("nominator/denominator=(%f, %f)\n",nom.floats[0],nom.floats[1]);
      _logcf_YAC_RARG(&nom,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *logarithm=&helper; //(_Complexf*)helper.value.object_val;
         logarithm->_mulf_YAC_RSELF(0.5f);
         res->floats[0]=logarithm->floats[0];
         res->floats[1]=logarithm->floats[1];
//         yac_host->yacDelete(logarithm);
//      }
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atanhcf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _atanhcf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      YAC_NEW_STACK(Complexf, nom);
      YAC_NEW_STACK(Complexf, denom);
      YAC_NEW_STACK(Complexf, helper);
      res->floats[0]=0.0f;
      res->floats[1]=0.0f;
      nom._init(1.0f,0.0f);
      nom._add_YAC_RSELF(v);
      denom._init(1.0f,0.0f);
      denom._sub_YAC_RSELF(v);
      //yac_host->printf("nominator: (%f, %f) denominator: (%f, %f)\n",
      //   nom.floats[0],nom.floats[1],denom.floats[0],denom.floats[1]);
      nom._div_YAC_RSELF(&denom);
      //yac_host->printf("nominator/denominator=(%f, %f)\n",nom.floats[0],nom.floats[1]);
      _logcf_YAC_RARG(&nom,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *logarithm=&helper; //(_Complexf*)helper.value.object_val;
         logarithm->_mulf_YAC_RSELF(0.5f);
         res->floats[0]=logarithm->floats[0];
         res->floats[1]=logarithm->floats[1];
//         yac_host->yacDelete(logarithm);
//      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atanhcf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _asincf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      YAC_NEW_STACK(Complexf, root);
      YAC_NEW_STACK(Complexf, vv);
      YAC_NEW_STACK(Complexf, helper);
      res->_init(0.0f,0.0f);
      root._init(1.0f,0.0f);
      vv=*v;
      vv._mul_YAC_RSELF(v);
      root._sub_YAC_RSELF(&vv);
      //yac_host->printf("1-z^2=(%f, %f)\n",root.floats[0],root.floats[1]);

      vv.floats[0]=-v->floats[1];
      vv.floats[1]=v->floats[0];
      _sqrtcf_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *rooted=&helper; //(_Complexf*)helper.value.object_val;
         YAC_NEW_STACK(Complexf, helper2);
         //yac_host->printf("sqrt(1-z^2)=(%f, %f)\n",rooted->floats[0],rooted->floats[1]);

         vv._add_YAC_RSELF(rooted);
         //yac_host->printf("i*z+sqrt(1-z^2)=(%f, %f)\n",vv.floats[0],vv.floats[1]);

         _logcf_YAC_RARG(&vv,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexf *logarithm=&helper2; //(_Complexf*)helper2.value.object_val;
            res->floats[0]=logarithm->floats[1];
            res->floats[1]=-logarithm->floats[0];
//            yac_host->yacDelete(logarithm);
//         } //should not happen
//         yac_host->yacDelete(rooted);
//      } //should not happen
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asincf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _asincf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      YAC_NEW_STACK(Complexf, root);
      YAC_NEW_STACK(Complexf, vv);
      YAC_NEW_STACK(Complexf, helper);
      res->_init(0.0f,0.0f);
      root._init(1.0f,0.0f);
      vv=*v;
      vv._mul_YAC_RSELF(v);
      root._sub_YAC_RSELF(&vv);
      //yac_host->printf("1-z^2=(%f, %f)\n",root.floats[0],root.floats[1]);

      vv.floats[0]=-v->floats[1];
      vv.floats[1]=v->floats[0];
      _sqrtcf_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *rooted=&helper; //(_Complexf*)helper.value.object_val;
         YAC_NEW_STACK(Complexf, helper2);
         //yac_host->printf("sqrt(1-z^2)=(%f, %f)\n",rooted->floats[0],rooted->floats[1]);

         vv._add_YAC_RSELF(rooted);
         //yac_host->printf("i*z+sqrt(1-z^2)=(%f, %f)\n",vv.floats[0],vv.floats[1]);

         _logcf_YAC_RARG(&vv,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexf *logarithm=&helper2; //(_Complexf*)helper2.value.object_val;
            res->floats[0]=logarithm->floats[1];
            res->floats[1]=-logarithm->floats[0];
//            yac_host->yacDelete(logarithm);
//         } //should not happen
//         yac_host->yacDelete(rooted);
//      } //should not happen
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::asincf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _acoscf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      YAC_NEW_STACK(Complexf, root);
      YAC_NEW_STACK(Complexf, vv);
      YAC_NEW_STACK(Complexf, helper);
      res->_init(0.0f,0.0f);
      root._init(1.0f,0.0f);
      vv=*v;
      vv._mul_YAC_RSELF(v);
      root._sub_YAC_RSELF(&vv);
      //yac_host->printf("z^2-1=(%f, %f)\n",root.floats[0],root.floats[1]);
      _sqrtcf_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *rooted=&helper; //(_Complexf*)helper.value.object_val;
         YAC_NEW_STACK(Complexf, helper2);
         //yac_host->printf("sqrt(z^2-1)=(%f, %f)\n",rooted->floats[0],rooted->floats[1]);
         vv=*v;
         vv.floats[0]+=-rooted->floats[1];
         vv.floats[1]+=rooted->floats[0];
         //yac_host->printf("z+sqrt(z^2-1)=(%f, %f)\n",rooted->floats[0],rooted->floats[1]);
         _logcf_YAC_RARG(&vv,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexf *logarithm=&helper2; //(_Complexf*)helper2.value.object_val;
            res->floats[0]=logarithm->floats[1];
            res->floats[1]=-logarithm->floats[0];
//            yac_host->yacDelete(logarithm);
//         } //should not happen
//         yac_host->yacDelete(rooted);
//      } //should not happen
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoscf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _acoscf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      YAC_NEW_STACK(Complexf, root);
      YAC_NEW_STACK(Complexf, vv);
      YAC_NEW_STACK(Complexf, helper);
      res->_init(0.0f,0.0f);
      root._init(1.0f,0.0f);
      vv=*v;
      vv._mul_YAC_RSELF(v);
      root._sub_YAC_RSELF(&vv);
      //yac_host->printf("z^2-1=(%f, %f)\n",root.floats[0],root.floats[1]);
      _sqrtcf_YAC_RARG(&root,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *rooted=&helper; //(_Complexf*)helper.value.object_val;
         YAC_NEW_STACK(Complexf, helper2);
         //yac_host->printf("sqrt(z^2-1)=(%f, %f)\n",rooted->floats[0],rooted->floats[1]);
         vv=*v;
         vv.floats[0]+=-rooted->floats[1];
         vv.floats[1]+=rooted->floats[0];
         //yac_host->printf("z+sqrt(z^2-1)=(%f, %f)\n",rooted->floats[0],rooted->floats[1]);
         _logcf_YAC_RARG(&vv,&helper2);
//         if (helper2.type==YAC_TYPE_OBJECT) {
            _Complexf *logarithm=&helper2; //(_Complexf*)helper2.value.object_val;
            res->floats[0]=logarithm->floats[1];
            res->floats[1]=-logarithm->floats[0];
//            yac_host->yacDelete(logarithm);
//         } //should not happen
//         yac_host->yacDelete(rooted);
//      } //should not happen
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::acoscf_ARG Object is not of type Complexf");
   }
}

void YAC_CALL _atancf_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if (YAC_CHK(_v,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = YAC_NEW_POOLED(Complexf);
      YAC_NEW_STACK(Complexf, vv);
      YAC_NEW_STACK(Complexf, nom);
      YAC_NEW_STACK(Complexf, denom);
      YAC_NEW_STACK(Complexf, helper);
      res->_init(0.0f,0.0f);
      vv._init(-v->floats[1],v->floats[0]);
      nom._init(1.0f,0.0f);
      nom._add_YAC_RSELF(&vv);
      denom._init(1.0f,0.0f);
      denom._sub_YAC_RSELF(&vv);
      nom._div_YAC_RSELF(&denom);
      _logcf_YAC_RARG(&nom,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *logarithm=&helper; //(_Complexf*)helper.value.object_val;
         res->floats[0]=0.5f*logarithm->floats[1];
         res->floats[1]=-0.5f*logarithm->floats[0];
//         yac_host->yacDelete(logarithm);
//      } //should not happen
      _r->initObject(res,1);
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atancf_VAL Object is not of type Complexf");
   }
}

void YAC_CALL _atancf_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if (YAC_CHK(_v,clid_Complexf)&&YAC_CHK(_r,clid_Complexf)) {
      _Complexf *v=(_Complexf*)_v;
      _Complexf *res = (_Complexf*)_r;
      YAC_NEW_STACK(Complexf, vv);
      YAC_NEW_STACK(Complexf, nom);
      YAC_NEW_STACK(Complexf, denom);
      YAC_NEW_STACK(Complexf, helper);
      res->_init(0.0f,0.0f);
      vv._init(-v->floats[1],v->floats[0]);
      nom._init(1.0f,0.0f);
      nom._add_YAC_RSELF(&vv);
      denom._init(1.0f,0.0f);
      denom._sub_YAC_RSELF(&vv);
      nom._div_YAC_RSELF(&denom);
      _logcf_YAC_RARG(&nom,&helper);
//      if (helper.type==YAC_TYPE_OBJECT) {
         _Complexf *logarithm=&helper; //(_Complexf*)helper.value.object_val;
         res->floats[0]=0.5f*logarithm->floats[1];
         res->floats[1]=-0.5f*logarithm->floats[0];
//         yac_host->yacDelete(logarithm);
//      } //should not happen
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::atancf_ARG Object is not of type Complexf");
   }
}

#endif
