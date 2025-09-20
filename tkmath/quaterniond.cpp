/// quaterniond.cpp
///
/// (c) 2008-2023 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <math.h>

#include "tkmath_yac.h"
#include "tkmath.h"
#include "vector3d.h"
#include "matrix4d.h"
#include "quaterniond.h"

#include "ying_Math.h"


_Quaterniond::_Quaterniond() {
   doubles[0]=doubles[1]=doubles[2]=doubles[3]=0.0;
}

_Quaterniond::~_Quaterniond() {
}

sUI YAC_VCALL _Quaterniond::yacArrayGetNumElements() {
   return 4;
}

sUI YAC_VCALL _Quaterniond::yacArrayGetMaxElements() {
   return 4;
}

sUI YAC_VCALL _Quaterniond::yacArrayGetElementType() {
   return YAC_TYPE_OBJECT;
}

sUI YAC_VCALL _Quaterniond::yacArrayGetElementByteSize() {
   return sizeof(sF64);
}

void YAC_VCALL _Quaterniond::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4)
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
               if (!_val->value.object_val->yacScanF64(&v))
               {
                  // Scan failed, silently assign 0.0f
                  v = 0.0;
               }
            }
            else
            {
               // Invalid or <null> object, silently assign 0.0f
               v = 0.0;
            }
            break;
         default:
            // Should never ever happen!
            Dyac_throw(TypeMismatch, "tkmath::Quaterniond::yacArraySet: unknown type");
            v = 0.0;
            break;
      }
      doubles[_idx] = v;
   }
   else
   {
      static char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Quaterniond::yacArraySet: Index too high (%d>3)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Quaterniond::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4)
   {
      YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
      d->value=doubles[_idx];
      _val->initObject(d,1);
   }
   else
   {
      static char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Quaterniond::yacArrayGet: Index too high (%d>3)", _idx);
      _val->initFloat(0.0f);
      Dyac_throw(ReadArrayOutOfBounds, buf);
   }
}

void *YAC_VCALL _Quaterniond::yacArrayGetPointer() {
   return (void*)doubles;
}

sSI YAC_VCALL _Quaterniond::yacTensorRank() {
   return YAC_TENSOR_RANK_VECTOR;
}

void YAC_VCALL _Quaterniond::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F64(doubles[0]);
   YAC_SERIALIZE_F64(doubles[1]);
   YAC_SERIALIZE_F64(doubles[2]);
   YAC_SERIALIZE_F64(doubles[3]);
}

sUI YAC_VCALL _Quaterniond::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
  YAC_BEG_DESERIALIZE();
  doubles[0]=YAC_DESERIALIZE_F64();
  doubles[1]=YAC_DESERIALIZE_F64();
  doubles[2]=YAC_DESERIALIZE_F64();
  doubles[3]=YAC_DESERIALIZE_F64();
  return 1;
}

void YAC_VCALL _Quaterniond::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min,clid_Quaterniond))
   {
      if(YAC_CHK(_max, clid_Quaterniond))
      {
         _Quaterniond *min = (_Quaterniond*) _min;
         _Quaterniond *max = (_Quaterniond*) _max;
         if(doubles[0] < min->doubles[0])
         {
            doubles[0] = min->doubles[0];
         }
         else if(doubles[0] > max->doubles[0])
         {
            doubles[0] = max->doubles[0];
         }

         if(doubles[1] < min->doubles[1])
         {
            doubles[1] = min->doubles[1];
         }
         else if(doubles[1] > max->doubles[1])
         {
            doubles[1] = max->doubles[1];
         }

         if(doubles[2] < min->doubles[2])
         {
            doubles[2] = min->doubles[2];
         }
         else if(doubles[2] > max->doubles[2])
         {
            doubles[2] = max->doubles[2];
         }

         if(doubles[3] < min->doubles[3])
         {
            doubles[3] = min->doubles[3];
         }
         else if(doubles[3] > max->doubles[3])
         {
            doubles[3] = max->doubles[3];
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Quaterniond::yacOperatorClamp: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Quaterniond::yacOperatorClamp: invalid _min object.");
   }
}

void YAC_VCALL _Quaterniond::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Quaterniond))
   {
      if(YAC_CHK(_max, clid_Quaterniond))
      {
         _Quaterniond *min = (_Quaterniond*)_min;
         _Quaterniond *max = (_Quaterniond*)_max;
         if(doubles[0] < min->doubles[0])
         {
            doubles[0] += (max->doubles[0] - min->doubles[0]);
         }
         else if(doubles[0] > max->doubles[0])
         {
            doubles[0] -= (max->doubles[0] - min->doubles[0]);
         }

         if(doubles[1] < min->doubles[1])
         {
            doubles[1] += (max->doubles[1] - min->doubles[1]);
         }
         else if(doubles[1] > max->doubles[1])
         {
            doubles[1] -= (max->doubles[1] - min->doubles[1]);
         }

         if(doubles[2] < min->doubles[2])
         {
            doubles[2] += (max->doubles[2] - min->doubles[2]);
         }
         else if(doubles[2] > max->doubles[2])
         {
            doubles[2] -= (max->doubles[2] - min->doubles[2]);
	 }

         if(doubles[3] < min->doubles[3])
         {
            doubles[3] += (max->doubles[3] - min->doubles[3]);
         }
         else if(doubles[3] > max->doubles[3])
         {
            doubles[3] -= (max->doubles[3] - min->doubles[3]);
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Quaterniond::yacOperatorWrap: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Quaterniond::yacOperatorWrap: invalid _min object.");
   }
}

void YAC_VCALL _Quaterniond::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   //yac_host->printf("yacOp called with cmd=%d\n",_cmd);
   if(YAC_CHK(_o, clid_Quaterniond))
   {
      _Quaterniond *o = (_Quaterniond*) _o;
      switch (_cmd)
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
            doubles[0] = 0.0;
            doubles[1] = 0.0;
            doubles[2] = 0.0;
            doubles[3] = 0.0;
            break;
         default:
         {
            static char buf[256];
            Dyac_snprintf(buf, 256, "tkmath::Quaterniond::yacOperator unsupported cmd %d", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
         }
         return;
      }
   }
   else if(YAC_Is_FloatArray(_o))
   {
      YAC_FloatArray *o = (YAC_FloatArray*) _o;
      switch (_cmd)
      {
         case YAC_OP_ASSIGN:
            assignFloatArray(o);
            break;
         default:
         {
            // Construct temporary Quaterniond object
            YAC_NEW_STACK(Quaterniond,t);
            t.assignFloatArray(o);

            // Recursively call yacOperator
            //just handle it like a vector2f
            yacOperator(_cmd, &t, _r);
         }
         break;
      }
   }
   else if(YAC_VALID(_o))
   {
      sF64 val;
      if (_o->yacScanF64(&val)) {
         yacOperatorF64(_cmd, val, _r);
      } else
         // Handle generic array object
         switch(_cmd)
         {
            case YAC_OP_ASSIGN:
               assignGeneric(NULL, _o);
               break;

            default:
            {
               YAC_Object::yacOperator(_cmd, _o, _r);
            }
            break;
         }
   }
   else
   {
      switch (_cmd) {
         case YAC_OP_NEG:
            doubles[0]=-doubles[0];
            doubles[1]=-doubles[1];
            doubles[2]=-doubles[2];
            doubles[3]=-doubles[3];
            break;
            // Not a valid Object (e.g. <null>)
         default:
            YAC_Object::yacOperator(_cmd,_o,_r);
      }
   }
}

void _Quaterniond::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void _Quaterniond::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void _Quaterniond::yacOperatorF64(sSI _cmd, sF64 val, YAC_Value *_r) {
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
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaterniond::yacOperatorF64 OP_DIV Cannot divide by zero");
         }
         break;
      default:
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Quaterniond::yacOperatorF64 Unsupported Operator %d",_cmd);
         Dyac_throw_def(TkMathUnsupportedOperator, buf);
         break;
   }
}

void YAC_VCALL _Quaterniond::yacValueOfI(sSI val) {
   doubles[0] = (sF64) val;
}

void YAC_VCALL _Quaterniond::yacValueOfI64(sS64 val) {
   doubles[0] = (sF64) val;
}

void YAC_VCALL _Quaterniond::yacValueOfF32(sF32 val) {
   doubles[0] = (sF64) val;
}

void YAC_VCALL _Quaterniond::yacValueOfF64(sF64 val) {
   doubles[0] = (sF64) val;
}

sBool YAC_VCALL _Quaterniond::yacScanI(sSI *val) {
   //lrintf can be 64bit (on gcc 64bit), but can also be 32bit
   //*val = (sSI) lrintf(doubles[0]);
   *val = (sSI) ( (doubles[0]<0) ? (doubles[0] - 0.5) : (doubles[0] + 0.5) );
   return true;
}

sBool YAC_VCALL _Quaterniond::yacScanI64(sS64 *val) {
   //llrintf is 64bit
   //*val = (sS64) llrintf(doubles[0]);
   *val = (sS64) ( (doubles[0]<0) ? (doubles[0] - 0.5) : (doubles[0] + 0.5) );
   return true;
}

sBool YAC_VCALL _Quaterniond::yacScanF32(sF32 *val) {
   *val = (sF32) doubles[0];
   return true;
}

sBool YAC_VCALL _Quaterniond::yacScanF64(sF64 *val) {
   *val = doubles[0];
   return true;
}

void _Quaterniond::assignFloats(sF32 *_fv, sUI _ne) {
   if(_ne <= 4) {
      // Copy as much elements from array as possible but never more than 2
      sUI i;
      for(i=0; i<_ne; i++) {
         doubles[i] = (sF64) _fv[i];
      }
      // Fill the rest with 0.0f
      for(; i<4; i++) {
         doubles[i] = 0.0;
      }
   } else {
      // Copy four elements
      doubles[0] = (sF64) _fv[0];
      doubles[1] = (sF64) _fv[1];
      doubles[2] = (sF64) _fv[2];
      doubles[3] = (sF64) _fv[3];
   }
}

void _Quaterniond::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Quaterniond::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
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
            for(i=0; (i<4) && (i<ne); i++)
            {
               _o->yacArrayGet(_ctx, i, &v);
               doubles[i] = (sF64) v.value.float_val;
            }
            // Fill the rest with 0.0f
            for(; i<4; i++)
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
         for(i=0; (i<4) && (i<ne); i++)
         {
            _o->yacArrayGet(_ctx, i, &v);
            v.typecast(YAC_TYPE_OBJECT);
	    if (!v.value.object_val->yacScanF64(&doubles[i])) doubles[i]=0.0;
            v.unset();
         }
         // Fill the rest with 0.0f
         for(; i<4; i++)
         {
            doubles[i] = 0.0;
         }
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign real part if it exists and rest with 0.0f
      if (!_o->yacScanF64(&doubles[0])) doubles[0]=0.0;
      doubles[1] = doubles[2] = doubles[3] = 0.0;
   }
}

//generate string representation
sBool YAC_VCALL _Quaterniond::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(128, 0,0,0);
   s->printf("(%lg+i*%lg+j*%lg+k*%lg)", doubles[0], doubles[1], doubles[2], doubles[3]);
   s->fixLength();
   return 1;
}


void _Quaterniond::_setR(YAC_Object *a) {
   if (YAC_VALID(a)) {
      if (!a->yacScanF64(&doubles[0])) doubles[0]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::setR Object not valid");
   }
}

void _Quaterniond::_setI(YAC_Object *a) {
   if (YAC_VALID(a)) {
      if (!a->yacScanF64(&doubles[1])) doubles[1]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::setI Object not valid");
   }
}

void _Quaterniond::_setJ(YAC_Object *a) {
   if (YAC_VALID(a)) {
      if (!a->yacScanF64(&doubles[2])) doubles[2]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::setJ Object not valid");
   }
}

void _Quaterniond::_setK(YAC_Object *a) {
   if (YAC_VALID(a)) {
      if (!a->yacScanF64(&doubles[3])) doubles[3]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::setK Object not valid");
   }
}

void _Quaterniond::_setV(YAC_Object *_v) {
  if (YAC_CHK(_v,clid_Vector3d)) {
     _Vector3d *v=(_Vector3d*)_v;
     doubles[1] = v->doubles[0];
     doubles[2] = v->doubles[1];
     doubles[3] = v->doubles[2];
  } else {
     Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::setV Object not of type Vector3d");
  }
}


void _Quaterniond::_getR_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[0];
   _r->initObject(d,1);
}

void _Quaterniond::_getR_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[0]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::getR Object not valid");
   }
}

void _Quaterniond::_getI_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[1];
   _r->initObject(d,1);
}

void _Quaterniond::_getI_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[1]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::getI Object not valid");
   }
}
void _Quaterniond::_getJ_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[2];
   _r->initObject(d,1);
}

void _Quaterniond::_getJ_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[2]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::getJ Object not valid");
   }
}
void _Quaterniond::_getK_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=doubles[3];
   _r->initObject(d,1);
}

void _Quaterniond::_getK_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(doubles[3]);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::getK Object not valid");
   }
}

void _Quaterniond::_getV_YAC_RVAL(YAC_Value *_r) {
   _Vector3d *v=YAC_NEW_POOLED(Vector3d);
   _getV_YAC_RARG(v);
   _r->initObject(v,1);
}

void _Quaterniond::_getV_YAC_RARG(YAC_Object *_v) {
   if (YAC_CHK(_v,clid_Vector3d)) {
      _Vector3d *v = (_Vector3d*) _v;
      v->doubles[0] = doubles[1];
      v->doubles[1] = doubles[2];
      v->doubles[2] = doubles[3];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::getV_ARG Return object not of type Vector3d");
   }
}

//create string object and fill with string representation
void _Quaterniond::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
   yacToString(s);
   YAC_RETS(s, 1);
}

void _Quaterniond::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Quaterniond))
   {
//add two vector2f
      _Quaterniond *o = (_Quaterniond*) _o;
      doubles[0] += o->doubles[0];
      doubles[1] += o->doubles[1];
      doubles[2] += o->doubles[2];
      doubles[3] += o->doubles[3];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Quaterniond::add_SELF: invalid Quaterniond object");
   }
}

void _Quaterniond::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   _add_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaterniond::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Quaterniond)&&YAC_CHK(_r, clid_Quaterniond))
   {
      _Quaterniond *o = (_Quaterniond*) _o;
      _Quaterniond *r = (_Quaterniond*) _r;
      r->doubles[0] = doubles[0] + o->doubles[0];
      r->doubles[1] = doubles[1] + o->doubles[1];
      r->doubles[2] = doubles[2] + o->doubles[2];
      r->doubles[3] = doubles[3] + o->doubles[3];

   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Quaterniond::add_ARG: invalid Quaterniond object");
   }
}


void _Quaterniond::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Quaterniond))
   {
//add two vector2f
      _Quaterniond *o = (_Quaterniond*) _o;
      doubles[0] -= o->doubles[0];
      doubles[1] -= o->doubles[1];
      doubles[2] -= o->doubles[2];
      doubles[3] -= o->doubles[3];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Quaterniond::sub_SELF: invalid Quaterniond object");
   }
}

void _Quaterniond::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   _sub_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaterniond::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Quaterniond)&&YAC_CHK(_r, clid_Quaterniond))
   {
      _Quaterniond *o = (_Quaterniond*) _o;
      _Quaterniond *r = (_Quaterniond*) _r;
      r->doubles[0] = doubles[0] - o->doubles[0];
      r->doubles[1] = doubles[1] - o->doubles[1];
      r->doubles[2] = doubles[2] - o->doubles[2];
      r->doubles[3] = doubles[3] - o->doubles[3];

   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Quaterniond::sub_ARG: invalid Quaterniond object");
   }
}

void _Quaterniond::muld(sF64 s) {
   doubles[0] *= s;
   doubles[1] *= s;
   doubles[2] *= s;
   doubles[3] *= s;
}

void _Quaterniond::_muld_YAC_RSELF(YAC_Object *_s) {
   if (YAC_VALID(_s)) {
      sF64 s;
      if (!_s->yacScanF64(&s)) s=0.0;
      muld(s);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::muld Object not valid");
   }
}

void _Quaterniond::_muld_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   if (YAC_VALID(_s)) {
      _Quaterniond *r=YAC_NEW_POOLED(Quaterniond);
      sF64 s;
      if (!_s->yacScanF64(&s)) s=0.0;
      r->doubles[0] = doubles[0] * s;
      r->doubles[1] = doubles[1] * s;
      r->doubles[2] = doubles[2] * s;
      r->doubles[3] = doubles[3] * s;
      _r->initObject(r,1);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::muld Object not valid");
   }
}

void _Quaterniond::_muld_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if (YAC_CHK(_r, clid_Quaterniond)) {
      if (YAC_VALID(_s)) {
         _Quaterniond *r=(_Quaterniond*)_r;
	 sF64 s;
	 if (!_s->yacScanF64(&s)) s=0.0;
         r->doubles[0] = doubles[0] * s;
         r->doubles[1] = doubles[1] * s;
         r->doubles[2] = doubles[2] * s;
         r->doubles[3] = doubles[3] * s;
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::muld Object not valid");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::mulf_ARG Return object not of type Quaterniond");
   }
}

void _Quaterniond::_mul_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Quaterniond)) {
      _Quaterniond *q=(_Quaterniond*)_o;
      sF64 t,x,y,z;
      t=doubles[0]*q->doubles[0]-doubles[1]*q->doubles[1]-doubles[2]*q->doubles[2]-doubles[3]*q->doubles[3];
      x=doubles[0]*q->doubles[1]+doubles[1]*q->doubles[0]+doubles[2]*q->doubles[3]-doubles[3]*q->doubles[2];
      y=doubles[0]*q->doubles[2]-doubles[1]*q->doubles[3]+doubles[2]*q->doubles[0]+doubles[3]*q->doubles[1];
      z=doubles[0]*q->doubles[3]+doubles[1]*q->doubles[2]-doubles[2]*q->doubles[1]+doubles[3]*q->doubles[0];

      doubles[0]=t;
      doubles[1]=x;
      doubles[2]=y;
      doubles[3]=z;
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::mul_SELF Object not of type Quaterniond");
   }
}

void _Quaterniond::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   _mul_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaterniond::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Quaterniond)) {
      if (YAC_CHK(_r,clid_Quaterniond)) {
         _Quaterniond *q=(_Quaterniond*)_o;
	 _Quaterniond *r=(_Quaterniond*)_r;
         r->doubles[0]=doubles[0]*q->doubles[0]-doubles[1]*q->doubles[1]-doubles[2]*q->doubles[2]-doubles[3]*q->doubles[3];
         r->doubles[1]=doubles[0]*q->doubles[1]+doubles[1]*q->doubles[0]+doubles[2]*q->doubles[3]-doubles[3]*q->doubles[2];
         r->doubles[2]=doubles[0]*q->doubles[2]-doubles[1]*q->doubles[3]+doubles[2]*q->doubles[0]+doubles[3]*q->doubles[1];
         r->doubles[3]=doubles[0]*q->doubles[3]+doubles[1]*q->doubles[2]-doubles[2]*q->doubles[1]+doubles[3]*q->doubles[0];
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::mul_ARG Return object not of type Quaterniond");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::mul_ARG Object not of type Quaterniond");
   }
}

void _Quaterniond::_mulConj_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   _mulConj_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaterniond::_mulConj_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Quaterniond)) {
      if (YAC_CHK(_r,clid_Quaterniond)) {
         _Quaterniond *q=(_Quaterniond*)_o;
	 _Quaterniond *r=(_Quaterniond*)_r;
         r->doubles[0]=doubles[0]*q->doubles[0]+doubles[1]*q->doubles[1]+doubles[2]*q->doubles[2]+doubles[3]*q->doubles[3];

         r->doubles[1]=-doubles[0]*q->doubles[1]+doubles[1]*q->doubles[0]-doubles[2]*q->doubles[3]+doubles[3]*q->doubles[2];
         r->doubles[2]=-doubles[0]*q->doubles[2]+doubles[1]*q->doubles[3]+doubles[2]*q->doubles[0]-doubles[3]*q->doubles[1];
         r->doubles[3]=-doubles[0]*q->doubles[3]-doubles[1]*q->doubles[2]+doubles[2]*q->doubles[1]+doubles[3]*q->doubles[0];
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::mulConj_ARG Return object not of type Quaterniond");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::mulConj_ARG Object not of type Quaterniond");
   }
}

void _Quaterniond::_conjMul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   _conjMul_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaterniond::_conjMul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Quaterniond)) {
      if (YAC_CHK(_r,clid_Quaterniond)) {
         _Quaterniond *q=(_Quaterniond*)_o;
	 _Quaterniond *r=(_Quaterniond*)_r;
         r->doubles[0]=doubles[0]*q->doubles[0]+doubles[1]*q->doubles[1]+doubles[2]*q->doubles[2]+doubles[3]*q->doubles[3];

         r->doubles[1]=doubles[0]*q->doubles[1]-doubles[1]*q->doubles[0]-doubles[2]*q->doubles[3]+doubles[3]*q->doubles[2];
         r->doubles[2]=doubles[0]*q->doubles[2]+doubles[1]*q->doubles[3]-doubles[2]*q->doubles[0]-doubles[3]*q->doubles[1];
         r->doubles[3]=doubles[0]*q->doubles[3]-doubles[1]*q->doubles[2]+doubles[2]*q->doubles[1]-doubles[3]*q->doubles[0];
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::conjMul_ARG Return object not of type Quaterniond");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::conjMul_ARG Object not of type Quaterniond");
   }
}

sF64 _Quaterniond::dot(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Quaterniond)) {
      _Quaterniond *q=(_Quaterniond*)_o;
      return doubles[0]*q->doubles[0]+doubles[1]*q->doubles[1]+doubles[2]*q->doubles[2]+doubles[3]*q->doubles[3];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::dot Object not of type Quaterniond");
   }
   return 0.0;
}

void _Quaterniond::_dot_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=dot(_o);
   _r->initObject(d,1);
}

void _Quaterniond::_dot_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(dot(_o));
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::dot Return object not valid");
   }
}

void _Quaterniond::_outer_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   _outer_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaterniond::_outer_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Quaterniond)) {
      if (YAC_CHK(_r,clid_Quaterniond)) {
         _Quaterniond *q=(_Quaterniond*)_o;
	 _Quaterniond *r=(_Quaterniond*)_r;
	 r->doubles[0]=0.0;
	 r->doubles[1]=doubles[0]*q->doubles[1]-doubles[1]*q->doubles[0]+doubles[3]*q->doubles[2]-doubles[2]*q->doubles[3];
	 r->doubles[2]=doubles[0]*q->doubles[2]-doubles[2]*q->doubles[0]+doubles[1]*q->doubles[3]-doubles[3]*q->doubles[1];
	 r->doubles[3]=doubles[0]*q->doubles[3]-doubles[3]*q->doubles[0]+doubles[2]*q->doubles[1]-doubles[1]*q->doubles[2];
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::outer Return object not of type Quaterniond");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::outer Object not of type Quaterniond");
   }
}

void _Quaterniond::_invert_YAC_RSELF() {
   sF64 absvalsqr=getAbsSqr();
   if (Ddblnonzero(absvalsqr)) {
      absvalsqr = 1.0 / absvalsqr;
      doubles[0] *= absvalsqr;
      doubles[1] *= -absvalsqr;
      doubles[2] *= -absvalsqr;
      doubles[3] *= -absvalsqr;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaterniond::invert_SELF Cannot invert zero quaternion");
   }
}

void _Quaterniond::_invert_YAC_RVAL(YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   _invert_YAC_RARG(q);
   _r->initObject(q,1);
}

void _Quaterniond::_invert_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Quaterniond)) {
      _Quaterniond *r = (_Quaterniond*) _r;
      sF64 absvalsqr=getAbsSqr();
      if (Ddblnonzero(absvalsqr)) {
         absvalsqr = 1.0 / absvalsqr;
         r->doubles[0] =  doubles[0] * absvalsqr;
         r->doubles[1] = -doubles[1] * absvalsqr;
         r->doubles[2] = -doubles[2] * absvalsqr;
         r->doubles[3] = -doubles[3] * absvalsqr;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaterniond::invert_ARG Cannot invert zero quaternion");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::invert_ARG Return object not of type Quaterniond");
   }
}

sF64 _Quaterniond::getAbs() {
   return sqrt(doubles[0]*doubles[0]+doubles[1]*doubles[1]+doubles[2]*doubles[2]+doubles[3]*doubles[3]);
}

void _Quaterniond::_getAbs_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=getAbs();
   _r->initObject(d,1);
}

void _Quaterniond::_getAbs_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getAbs());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::getAbs Return object not valid");
   }
}

sF64 _Quaterniond::getAbsSqr() {
   return doubles[0]*doubles[0]+doubles[1]*doubles[1]+doubles[2]*doubles[2]+doubles[3]*doubles[3];
}

void _Quaterniond::_getAbsSqr_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=getAbsSqr();
   _r->initObject(d,1);
}

void _Quaterniond::_getAbsSqr_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getAbsSqr());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::getAbsSqr Return object not valid");
   }
}

void _Quaterniond::_unit_YAC_RSELF() {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      absval=1.0/absval;
      doubles[0]*=absval;
      doubles[1]*=absval;
      doubles[2]*=absval;
      doubles[3]*=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaterniond::unit_SELF Zero quaternion cannot be normalized");
   }
}

void _Quaterniond::_unit_YAC_RVAL(YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   _unit_YAC_RARG(q);
   _r->initObject(q,1);
}

void _Quaterniond::_unit_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Quaterniond)) {
      sF64 absval=getAbs();
      if (Ddblnonzero(absval)) {
         _Quaterniond *r=(_Quaterniond*)_r;
         absval=1.0/absval;
	 r->doubles[0] = doubles[0] * absval;
	 r->doubles[1] = doubles[1] * absval;
	 r->doubles[2] = doubles[2] * absval;
	 r->doubles[3] = doubles[3] * absval;
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::unit_ARG Zero quaternion cannot be normalized");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::unit_ARG Return object not of type Quaterniond");
   }
}

void _Quaterniond::unitScale(sF64 s) {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      absval=s/absval;
      doubles[0]*=absval;
      doubles[1]*=absval;
      doubles[2]*=absval;
      doubles[3]*=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaterniond::unitScale_SELF Zero quaternion cannot be scaled");
   }
}

void _Quaterniond::_unitScale_YAC_RSELF(YAC_Object *_s) {
   if (YAC_VALID(_s)) {
      sF64 s;
      if (!_s->yacScanF64(&s)) s=0.0;
      unitScale(s);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::unitScale Object not valid");
   }
}

void _Quaterniond::_unitScale_YAC_RVAL(YAC_Object *s, YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   _unitScale_YAC_RARG(s,q);
   _r->initObject(q,1);
}

void _Quaterniond::_unitScale_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Quaterniond)) {
      sF64 absval=getAbs();
      if (Ddblnonzero(absval)) {
         if (YAC_VALID(_s)) {
            _Quaterniond *r=(_Quaterniond*)_r;
   	    sF64 s;
	    if (!_s->yacScanF64(&s)) s=0.0;
            absval=s/absval;
	    r->doubles[0] = doubles[0] * absval;
	    r->doubles[1] = doubles[1] * absval;
	    r->doubles[2] = doubles[2] * absval;
            r->doubles[3] = doubles[3] * absval;
	 } else {
	    Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::unitScale Object not valid");
	 }
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::unitScale_ARG Zero quaternion cannot be scaled");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::unitScale_ARG Return object not of type Quaterniond");
   }
}

void _Quaterniond::_setAngle(YAC_Object *_a) {
   if (YAC_VALID(_a)) {
      sF64 a;
      if (!_a->yacScanF64(&a)) a=0.0;
      YAC_NEW_STACK(Quaterniond,v);
      sF64 radius=getAbs();
      sF64 sa=sin(a);
      _getPureUnit_YAC_RARG(&v);
      doubles[0]=radius*cos(a);
      doubles[1]=radius*v.doubles[1]*sa;
      doubles[2]=radius*v.doubles[2]*sa;
      doubles[3]=radius*v.doubles[3]*sa;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::setAngle Object not valid");
   }
}

void _Quaterniond::_setRadius(YAC_Object *r) {
   _unitScale_YAC_RSELF(r);
}

void _Quaterniond::_setPureUnit(YAC_Object *_v) {
   if (YAC_CHK(_v, clid_Quaterniond)) {
      _Quaterniond *v=(_Quaterniond*)_v;
      if (Ddblnonzero(v->doubles[0])&&(Ddblequal(v->getAbs(),1.0))) {
         sF64 radius=getAbs();
	 if (Ddblnonzero(radius)) {
	    sF64 angle = acos(doubles[0]/radius);
	    sF64 rsa = radius * sin(angle);
	    doubles[1] = v->doubles[1] * rsa;
	    doubles[2] = v->doubles[2] * rsa;
	    doubles[3] = v->doubles[3] * rsa;
	 } else {
	    Dyac_throw_def(TkMathDivisionByZero, "Cannot set pure unit vector for zero quaternion");
	 }
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::setPureUnit Object not a pure unit quaternion (real==0 and length==1)");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::setPureUnit Object not of type Quaterniond");
   }
}

sF64 _Quaterniond::getAngle() {
   sF64 absval=getAbs();
   if (Ddblnonzero(absval)) {
      return acos(doubles[0]/absval);
   } else {
     Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaterniond::getAngle Cannot determine of zero quaternion");
   }
   return 0.0;
}

void _Quaterniond::_getAngle_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=getAngle();
   _r->initObject(d,1);
}

void _Quaterniond::_getAngle_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getAngle());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::getAngle Object not valid");
   }
}

sF64 _Quaterniond::getRadius() {
  return getAbs();
}

void _Quaterniond::_getRadius_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *d=(YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   d->value=getRadius();
   _r->initObject(d,1);
}

void _Quaterniond::_getRadius_YAC_RARG(YAC_Object *_r) {
   if (YAC_VALID(_r)) {
      _r->yacValueOfF64(getRadius());
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::getRadius Object not valid");
   }
}

void _Quaterniond::_getPureUnit_YAC_RVAL(YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   _getPureUnit_YAC_RARG(q);
   _r->initObject(q,1);
}

void _Quaterniond::_getPureUnit_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Quaterniond)) {
      _Quaterniond *r=(_Quaterniond*)_r;
      sF64 radius=getAbs();
      if (Ddblnonzero(radius)) {
         sF64 angle=acos(doubles[0]/radius);
         sF64 scale=1.0/(radius*sin(angle));
         r->doubles[0]=0.0; //this makes it pure
         r->doubles[1]=doubles[1]*scale;
         r->doubles[2]=doubles[2]*scale;
         r->doubles[3]=doubles[3]*scale;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaterniond::getPureUnit_ARG Cannot get pure unit vector from zero quaternion");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::getPureUnit_ARG Return object not of type Quaterniond");
   }
}

void _Quaterniond::_getMatrix4d_YAC_RVAL(YAC_Value *_r) {
   _Matrix4d *m=YAC_NEW_POOLED(Matrix4d);
   _getMatrix4d_YAC_RARG(m);
   _r->initObject(m,1);
}

void _Quaterniond::_getMatrix4d_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Matrix4d)) {
      _Matrix4d *m = (_Matrix4d*) _r;
      m->doubles[ 0] = doubles[0]; m->doubles[ 1] = doubles[1]; m->doubles[ 2] = doubles[2]; m->doubles[ 3] = doubles[3];
      m->doubles[ 4] =-doubles[1]; m->doubles[ 5] = doubles[0]; m->doubles[ 6] =-doubles[3]; m->doubles[ 7] = doubles[2];
      m->doubles[ 8] =-doubles[2]; m->doubles[ 9] = doubles[3]; m->doubles[10] = doubles[0]; m->doubles[11] =-doubles[1];
      m->doubles[12] =-doubles[3]; m->doubles[13] =-doubles[2]; m->doubles[14] = doubles[1]; m->doubles[15] = doubles[0];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaterniond::getMatrix4d Return object not of type Matrix4d");
   }
}

void _Quaterniond::_initf(sF32 a, sF32 b, sF32 c, sF32 d) {
   doubles[0]=a;
   doubles[1]=b;
   doubles[2]=c;
   doubles[3]=d;
}

void _Quaterniond::_init(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Object *d) {
   if (YAC_VALID(a)&&YAC_VALID(b)&&YAC_VALID(c)&&YAC_VALID(d)) {
      if (!a->yacScanF64(&doubles[0])) doubles[0]=0.0;
      if (!b->yacScanF64(&doubles[1])) doubles[1]=0.0;
      if (!c->yacScanF64(&doubles[2])) doubles[2]=0.0;
      if (!d->yacScanF64(&doubles[3])) doubles[3]=0.0;
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaterniond::init Object not valid");
   }
}

void _Quaterniond::_New(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Object *d, YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   q->_init(a,b,c,d);
   _r->initObject(q,1);
}

void YAC_CALL _quaterniond(YAC_Object *a, YAC_Object *b, YAC_Object *c, YAC_Object *d, YAC_Value *_r) {
   _Quaterniond *q=YAC_NEW_POOLED(Quaterniond);
   q->_init(a,b,c,d);
   _r->initObject(q,1);
}
