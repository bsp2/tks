/// quaternionf.cpp
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
#include "matrix.h"
#include "vector3f.h"
#include "vector4f.h"
#include "matrix4f.h"
#include "quaternionf.h"

#include "ying_Math.h"


_Quaternionf::_Quaternionf() {
   floats[0]=floats[1]=floats[2]=floats[3]=0.0f;
}

_Quaternionf::~_Quaternionf() {
}

sUI YAC_VCALL _Quaternionf::yacArrayGetNumElements() {
   return 4;
}

sUI YAC_VCALL _Quaternionf::yacArrayGetMaxElements() {
   return 4;
}

sUI YAC_VCALL _Quaternionf::yacArrayGetElementType() {
   return YAC_TYPE_FLOAT;
}

sUI YAC_VCALL _Quaternionf::yacArrayGetElementByteSize() {
   return sizeof(sF32);
}

void YAC_VCALL _Quaternionf::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4)
   {
      sF32 v;
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
            {
               if (!_val->value.object_val->yacScanF32(&v))
               {
                  // Scan failed, silently assign 0.0f
                  v = 0.0f;
               }
            }
            else
            {
               // Invalid or <null> object, silently assign 0.0f
               v = 0.0f;
            }
            break;
         default:
            // Should never ever happen!
            Dyac_throw(TypeMismatch, "tkmath::Quaternionf::yacArraySet: unknown type");
            v = 0.0f;
            break;
      }
      floats[_idx] = v;
   }
   else
   {
      static char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Quaternionf::yacArraySet: Index too high (%d>3)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Quaternionf::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 4)
   {
      _val->initFloat(floats[_idx]);
   }
   else
   {
      static char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Quaternionf::yacArrayGet: Index too high (%d>3)", _idx);
      _val->initFloat(0.0f);
      Dyac_throw(ReadArrayOutOfBounds, buf);
   }
}

void *YAC_VCALL _Quaternionf::yacArrayGetPointer() {
   return (void*)floats;
}

sSI YAC_VCALL _Quaternionf::yacTensorRank() {
   return YAC_TENSOR_RANK_VECTOR;
}

void YAC_VCALL _Quaternionf::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F32(floats[0]);
   YAC_SERIALIZE_F32(floats[1]);
   YAC_SERIALIZE_F32(floats[2]);
   YAC_SERIALIZE_F32(floats[3]);
}

sUI YAC_VCALL _Quaternionf::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
  YAC_BEG_DESERIALIZE();
  floats[0]=YAC_DESERIALIZE_F32();
  floats[1]=YAC_DESERIALIZE_F32();
  floats[2]=YAC_DESERIALIZE_F32();
  floats[3]=YAC_DESERIALIZE_F32();
  return 1;
}

void YAC_VCALL _Quaternionf::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min,clid_Quaternionf))
   {
      if(YAC_CHK(_max, clid_Quaternionf))
      {
         _Quaternionf *min = (_Quaternionf*) _min;
         _Quaternionf *max = (_Quaternionf*) _max;
         if(floats[0] < min->floats[0])
         {
            floats[0] = min->floats[0];
         }
         else if(floats[0] > max->floats[0])
         {
            floats[0] = max->floats[0];
         }

         if(floats[1] < min->floats[1])
         {
            floats[1] = min->floats[1];
         }
         else if(floats[1] > max->floats[1])
         {
            floats[1] = max->floats[1];
         }

         if(floats[2] < min->floats[2])
         {
            floats[2] = min->floats[2];
         }
         else if(floats[2] > max->floats[2])
         {
            floats[2] = max->floats[2];
         }

         if(floats[3] < min->floats[3])
         {
            floats[3] = min->floats[3];
         }
         else if(floats[3] > max->floats[3])
         {
            floats[3] = max->floats[3];
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Quaternionf::yacOperatorClamp: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Quaternionf::yacOperatorClamp: invalid _min object.");
   }
}

void YAC_VCALL _Quaternionf::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min, clid_Quaternionf))
   {
      if(YAC_CHK(_max, clid_Quaternionf))
      {
         _Quaternionf *min = (_Quaternionf*)_min;
         _Quaternionf *max = (_Quaternionf*)_max;
         if(floats[0] < min->floats[0])
         {
            floats[0] += (max->floats[0] - min->floats[0]);
         }
         else if(floats[0] > max->floats[0])
         {
            floats[0] -= (max->floats[0] - min->floats[0]);
         }

         if(floats[1] < min->floats[1])
         {
            floats[1] += (max->floats[1] - min->floats[1]);
         }
         else if(floats[1] > max->floats[1])
         {
            floats[1] -= (max->floats[1] - min->floats[1]);
         }

         if(floats[2] < min->floats[2])
         {
            floats[2] += (max->floats[2] - min->floats[2]);
         }
         else if(floats[2] > max->floats[2])
         {
            floats[2] -= (max->floats[2] - min->floats[2]);
	 }

         if(floats[3] < min->floats[3])
         {
            floats[3] += (max->floats[3] - min->floats[3]);
         }
         else if(floats[3] > max->floats[3])
         {
            floats[3] -= (max->floats[3] - min->floats[3]);
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Quaternionf::yacOperatorWrap: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Quaternionf::yacOperatorWrap: invalid _min object.");
   }
}

void YAC_VCALL _Quaternionf::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   //yac_host->printf("yacOp called with cmd=%d\n",_cmd);
   if(YAC_CHK(_o, clid_Quaternionf))
   {
      _Quaternionf *o = (_Quaternionf*) _o;
      switch (_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI( Dfltequal(floats[0], o->floats[0]) &&
                      Dfltequal(floats[1], o->floats[1]) &&
		      Dfltequal(floats[2], o->floats[2]) &&
                      Dfltequal(floats[3], o->floats[3]) );
            break;
         case YAC_OP_CNE:
            YAC_RETI( Dfltnotequal(floats[0], o->floats[0]) ||
	              Dfltnotequal(floats[1], o->floats[1]) ||
		      Dfltnotequal(floats[2], o->floats[2]) ||
                      Dfltnotequal(floats[3], o->floats[3]) );
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
            floats[0] = 0.0f;
            floats[1] = 0.0f;
	    floats[2] = 0.0f;
	    floats[3] = 0.0f;
            break;
         default:
         {
            static char buf[256];
            Dyac_snprintf(buf, 256, "tkmath::Quaternionf::yacOperator unsupported cmd %d", _cmd);
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
            // Construct temporary Quaternionf object
            YAC_NEW_STACK(Quaternionf,t);
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
      sF32 val;
      if (_o->yacScanF32(&val)) {
         yacOperatorF32(_cmd, val, _r);
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
	   floats[0]=-floats[0];
	   floats[1]=-floats[1];
	   floats[2]=-floats[2];
	   floats[3]=-floats[3];
	   break;
      // Not a valid Object (e.g. <null>)
        default:
           YAC_Object::yacOperator(_cmd,_o,_r);
      }
   }
}

void _Quaternionf::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF32(_cmd, (sF32)val, _r);
}

void _Quaternionf::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
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
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaternionf::yacOperatorF32 OP_DIV Cannot divide by zero");
         }
         break;
      default:
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Quaternionf::yacOperatorF32 Unsupported Operator %d",_cmd);
         Dyac_throw_def(TkMathUnsupportedOperator, buf);
         break;
   }
}

void YAC_VCALL _Quaternionf::yacValueOfI(sSI val) {
   floats[0] = (sF32) val;
}

void YAC_VCALL _Quaternionf::yacValueOfI64(sS64 val) {
   floats[0] = (sF32) val;
}

void YAC_VCALL _Quaternionf::yacValueOfF32(sF32 val) {
   floats[0] = val;
}

void YAC_VCALL _Quaternionf::yacValueOfF64(sF64 val) {
   floats[0] = (sF32) val;
}

sBool YAC_VCALL _Quaternionf::yacScanI(sSI *val) {
   //lrintf can be 64bit (on gcc 64bit), but can also be 32bit
   //*val = (sSI) lrintf(floats[0]);
   *val = (sSI) ( (floats[0]<0) ? (floats[0] - 0.5f) : (floats[0] + 0.5f) );
   return true;
}

sBool YAC_VCALL _Quaternionf::yacScanI64(sS64 *val) {
   //llrintf is 64bit
   //*val = (sS64) llrintf(floats[0]);
   *val = (sS64) ( (floats[0]<0) ? (floats[0] - 0.5f) : (floats[0] + 0.5f) );
   return true;
}

sBool YAC_VCALL _Quaternionf::yacScanF32(sF32 *val) {
   *val = floats[0];
   return true;
}

sBool YAC_VCALL _Quaternionf::yacScanF64(sF64 *val) {
   *val = (sF64) floats[0];
   return true;
}

void _Quaternionf::assignFloats(sF32 *_fv, sUI _ne) {
   if(_ne <= 4) {
      // Copy as much elements from array as possible but never more than 2
      sUI i;
      for(i=0; i<_ne; i++) {
         floats[i] = _fv[i];
      }
      // Fill the rest with 0.0f
      for(; i<4; i++) {
         floats[i] = 0.0f;
      }
   } else {
      // Copy four elements
      floats[0] = _fv[0];
      floats[1] = _fv[1];
      floats[2] = _fv[2];
      floats[3] = _fv[3];
   }
}

void _Quaternionf::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Quaternionf::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
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
               floats[i] = v.value.float_val;
            }
            // Fill the rest with 0.0f
            for(; i<4; i++)
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
         for(i=0; (i<4) && (i<ne); i++)
         {
            _o->yacArrayGet(_ctx, i, &v);
            v.typecast(YAC_TYPE_FLOAT);
            floats[i] = v.value.float_val;
            v.unset();
         }
         // Fill the rest with 0.0f
         for(; i<4; i++)
         {
            floats[i] = 0.0f;
         }
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign real part if it exists and rest with 0.0f
      if (!_o->yacScanF32(&floats[0])) floats[0]=0;
      floats[1] = floats[2] = floats[3] = 0.0f;
   }
}

//generate string representation
sBool YAC_VCALL _Quaternionf::yacToString(YAC_String *s) const {
   s->yacArrayAlloc(128, 0,0,0);
   s->printf("(%g+i*%g+j*%g+k*%g)", floats[0], floats[1], floats[2], floats[3]);
   s->fixLength();
   return 1;
}

void _Quaternionf::_setR(sF32 a) {
   floats[0]=a;
}

void _Quaternionf::_setI(sF32 a) {
   floats[1]=a;
}

void _Quaternionf::_setJ(sF32 a) {
   floats[2]=a;
}

void _Quaternionf::_setK(sF32 a) {
   floats[3]=a;
}

void _Quaternionf::_setV(YAC_Object *_v) {
  if (YAC_CHK(_v,clid_Vector3f)) {
     _Vector3f *v=(_Vector3f*)_v;
     floats[1] = v->floats[0];
     floats[2] = v->floats[1];
     floats[3] = v->floats[2];
  } else {
     Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::setV Object not of type Vector3f");
  }
}

sF32 _Quaternionf::_getR() {
   return floats[0];
}

sF32 _Quaternionf::_getI() {
   return floats[1];
}

sF32 _Quaternionf::_getJ() {
   return floats[2];
}

sF32 _Quaternionf::_getK() {
   return floats[3];
}

void _Quaternionf::_getV_YAC_RVAL(YAC_Value *_r) {
   _Vector3f *v=YAC_NEW_POOLED(Vector3f);
   _getV_YAC_RARG(v);
   _r->initObject(v,1);
}

void _Quaternionf::_getV_YAC_RARG(YAC_Object *_v) {
   if (YAC_CHK(_v,clid_Vector3f)) {
      _Vector3f *v = (_Vector3f*) _v;
      v->floats[0] = floats[1];
      v->floats[1] = floats[2];
      v->floats[2] = floats[3];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::getV_ARG Return object not of type Vector3f");
   }
}

//create string object and fill with string representation
void _Quaternionf::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
   yacToString(s);
   YAC_RETS(s, 1);
}

void _Quaternionf::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Quaternionf))
   {
//add two vector2f
      _Quaternionf *o = (_Quaternionf*) _o;
      floats[0] += o->floats[0];
      floats[1] += o->floats[1];
      floats[2] += o->floats[2];
      floats[3] += o->floats[3];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Quaternionf::add_SELF: invalid Quaternionf object");
   }
}

void _Quaternionf::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   _add_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaternionf::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Quaternionf)&&YAC_CHK(_r, clid_Quaternionf))
   {
      _Quaternionf *o = (_Quaternionf*) _o;
      _Quaternionf *r = (_Quaternionf*) _r;
      r->floats[0] = floats[0] + o->floats[0];
      r->floats[1] = floats[1] + o->floats[1];
      r->floats[2] = floats[2] + o->floats[2];
      r->floats[3] = floats[3] + o->floats[3];

   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Quaternionf::add_ARG: invalid Quaternionf object");
   }
}


void _Quaternionf::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Quaternionf))
   {
//add two vector2f
      _Quaternionf *o = (_Quaternionf*) _o;
      floats[0] -= o->floats[0];
      floats[1] -= o->floats[1];
      floats[2] -= o->floats[2];
      floats[3] -= o->floats[3];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Quaternionf::sub_SELF: invalid Quaternionf object");
   }
}

void _Quaternionf::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   _sub_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaternionf::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Quaternionf)&&YAC_CHK(_r, clid_Quaternionf))
   {
      _Quaternionf *o = (_Quaternionf*) _o;
      _Quaternionf *r = (_Quaternionf*) _r;
      r->floats[0] = floats[0] - o->floats[0];
      r->floats[1] = floats[1] - o->floats[1];
      r->floats[2] = floats[2] - o->floats[2];
      r->floats[3] = floats[3] - o->floats[3];

   } else {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Quaternionf::sub_ARG: invalid Quaternionf object");
   }
}

void _Quaternionf::_mulf_YAC_RSELF(sF32 s) {
   floats[0] *= s;
   floats[1] *= s;
   floats[2] *= s;
   floats[3] *= s;
}

void _Quaternionf::_mulf_YAC_RVAL(sF32 s, YAC_Value *_r) {
   _Quaternionf *r=YAC_NEW_POOLED(Quaternionf);
   r->floats[0] = floats[0] * s;
   r->floats[1] = floats[1] * s;
   r->floats[2] = floats[2] * s;
   r->floats[3] = floats[3] * s;
   _r->initObject(r,1);
}

void _Quaternionf::_mulf_YAC_RARG(sF32 s, YAC_Object *_r) {
   if (YAC_CHK(_r, clid_Quaternionf)) {
      _Quaternionf *r=(_Quaternionf*)_r;
      r->floats[0] = floats[0] * s;
      r->floats[1] = floats[1] * s;
      r->floats[2] = floats[2] * s;
      r->floats[3] = floats[3] * s;
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::mulf_ARG Return object not of type Quaternionf");
   }
}

void _Quaternionf::_mul_YAC_RSELF(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Quaternionf)) {
      _Quaternionf *q=(_Quaternionf*)_o;
      sF32 t,x,y,z;
      t=floats[0]*q->floats[0]-floats[1]*q->floats[1]-floats[2]*q->floats[2]-floats[3]*q->floats[3];
      x=floats[0]*q->floats[1]+floats[1]*q->floats[0]+floats[2]*q->floats[3]-floats[3]*q->floats[2];
      y=floats[0]*q->floats[2]-floats[1]*q->floats[3]+floats[2]*q->floats[0]+floats[3]*q->floats[1];
      z=floats[0]*q->floats[3]+floats[1]*q->floats[2]-floats[2]*q->floats[1]+floats[3]*q->floats[0];

      floats[0]=t;
      floats[1]=x;
      floats[2]=y;
      floats[3]=z;
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::mul_SELF Object not of type Quaternionf");
   }
}

void _Quaternionf::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   _mul_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaternionf::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Quaternionf)) {
      if (YAC_CHK(_r,clid_Quaternionf)) {
         _Quaternionf *q=(_Quaternionf*)_o;
	 _Quaternionf *r=(_Quaternionf*)_r;
         r->floats[0]=floats[0]*q->floats[0]-floats[1]*q->floats[1]-floats[2]*q->floats[2]-floats[3]*q->floats[3];
         r->floats[1]=floats[0]*q->floats[1]+floats[1]*q->floats[0]+floats[2]*q->floats[3]-floats[3]*q->floats[2];
         r->floats[2]=floats[0]*q->floats[2]-floats[1]*q->floats[3]+floats[2]*q->floats[0]+floats[3]*q->floats[1];
         r->floats[3]=floats[0]*q->floats[3]+floats[1]*q->floats[2]-floats[2]*q->floats[1]+floats[3]*q->floats[0];
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::mul_ARG Return object not of type Quaternionf");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::mul_ARG Object not of type Quaternionf");
   }
}

void _Quaternionf::_mulConj_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   _mulConj_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaternionf::_mulConj_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Quaternionf)) {
      if (YAC_CHK(_r,clid_Quaternionf)) {
         _Quaternionf *q=(_Quaternionf*)_o;
	 _Quaternionf *r=(_Quaternionf*)_r;
         r->floats[0]=floats[0]*q->floats[0]+floats[1]*q->floats[1]+floats[2]*q->floats[2]+floats[3]*q->floats[3];

         r->floats[1]=-floats[0]*q->floats[1]+floats[1]*q->floats[0]-floats[2]*q->floats[3]+floats[3]*q->floats[2];
         r->floats[2]=-floats[0]*q->floats[2]+floats[1]*q->floats[3]+floats[2]*q->floats[0]-floats[3]*q->floats[1];
         r->floats[3]=-floats[0]*q->floats[3]-floats[1]*q->floats[2]+floats[2]*q->floats[1]+floats[3]*q->floats[0];
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::mulConj_ARG Return object not of type Quaternionf");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::mulConj_ARG Object not of type Quaternionf");
   }
}

void _Quaternionf::_conjMul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   _conjMul_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaternionf::_conjMul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Quaternionf)) {
      if (YAC_CHK(_r,clid_Quaternionf)) {
         _Quaternionf *q=(_Quaternionf*)_o;
	 _Quaternionf *r=(_Quaternionf*)_r;
         r->floats[0]=floats[0]*q->floats[0]+floats[1]*q->floats[1]+floats[2]*q->floats[2]+floats[3]*q->floats[3];

         r->floats[1]=floats[0]*q->floats[1]-floats[1]*q->floats[0]-floats[2]*q->floats[3]+floats[3]*q->floats[2];
         r->floats[2]=floats[0]*q->floats[2]+floats[1]*q->floats[3]-floats[2]*q->floats[0]-floats[3]*q->floats[1];
         r->floats[3]=floats[0]*q->floats[3]-floats[1]*q->floats[2]+floats[2]*q->floats[1]-floats[3]*q->floats[0];
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::conjMul_ARG Return object not of type Quaternionf");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::conjMul_ARG Object not of type Quaternionf");
   }
}

sF32 _Quaternionf::_dot(YAC_Object *_o) {
   if (YAC_CHK(_o,clid_Quaternionf)) {
      _Quaternionf *q=(_Quaternionf*)_o;
      return floats[0]*q->floats[0]+floats[1]*q->floats[1]+floats[2]*q->floats[2]+floats[3]*q->floats[3];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::dot Object not of type Quaternionf");
   }
   return 0.0f;
}

void _Quaternionf::_outer_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   _outer_YAC_RARG(_o,q);
   _r->initObject(q,1);
}

void _Quaternionf::_outer_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if (YAC_CHK(_o,clid_Quaternionf)) {
      if (YAC_CHK(_r,clid_Quaternionf)) {
         _Quaternionf *q=(_Quaternionf*)_o;
	 _Quaternionf *r=(_Quaternionf*)_r;
	 r->floats[0]=0.0f;
	 r->floats[1]=floats[0]*q->floats[1]-floats[1]*q->floats[0]+floats[3]*q->floats[2]-floats[2]*q->floats[3];
	 r->floats[2]=floats[0]*q->floats[2]-floats[2]*q->floats[0]+floats[1]*q->floats[3]-floats[3]*q->floats[1];
	 r->floats[3]=floats[0]*q->floats[3]-floats[3]*q->floats[0]+floats[2]*q->floats[1]-floats[1]*q->floats[2];
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::outer Return object not of type Quaternionf");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::outer Object not of type Quaternionf");
   }
}

void _Quaternionf::_invert_YAC_RSELF() {
   sF32 absvalsqr=_getAbsSqr();
   if (absvalsqr != 0.0f) {
      absvalsqr = 1.0f / absvalsqr;
      floats[0] *= absvalsqr;
      floats[1] *= -absvalsqr;
      floats[2] *= -absvalsqr;
      floats[3] *= -absvalsqr;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaternionf::invert_SELF Cannot invert zero quaternion");
   }
}

void _Quaternionf::_invert_YAC_RVAL(YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   _invert_YAC_RARG(q);
   _r->initObject(q,1);
}

void _Quaternionf::_invert_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Quaternionf)) {
      _Quaternionf *r = (_Quaternionf*) _r;
      sF32 absvalsqr=_getAbsSqr();
      if (absvalsqr != 0.0f) {
         absvalsqr = 1.0f / absvalsqr;
         r->floats[0] =  floats[0] * absvalsqr;
         r->floats[1] = -floats[1] * absvalsqr;
         r->floats[2] = -floats[2] * absvalsqr;
         r->floats[3] = -floats[3] * absvalsqr;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaternionf::invert_ARG Cannot invert zero quaternion");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::invert_ARG Return object not of type Quaternionf");
   }
}

sF32 _Quaternionf::_getAbs() {
   return sqrtf(floats[0]*floats[0]+floats[1]*floats[1]+floats[2]*floats[2]+floats[3]*floats[3]);

}

sF32 _Quaternionf::_getAbsSqr() {
   return floats[0]*floats[0]+floats[1]*floats[1]+floats[2]*floats[2]+floats[3]*floats[3];
}

void _Quaternionf::_unit_YAC_RSELF() {
   sF32 absval=_getAbs();
   if (Dfltnonzero(absval)) {
      absval=1.0f/absval;
      floats[0]*=absval;
      floats[1]*=absval;
      floats[2]*=absval;
      floats[3]*=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaternionf::unit_SELF Zero quaternion cannot be normalized");
   }
}

void _Quaternionf::_unit_YAC_RVAL(YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   _unit_YAC_RARG(q);
   _r->initObject(q,1);
}

void _Quaternionf::_unit_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Quaternionf)) {
      sF32 absval=_getAbs();
      if (Dfltnonzero(absval)) {
         _Quaternionf *r=(_Quaternionf*)_r;
         absval=1.0f/absval;
	 r->floats[0] = floats[0] * absval;
	 r->floats[1] = floats[1] * absval;
	 r->floats[2] = floats[2] * absval;
	 r->floats[3] = floats[3] * absval;
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::unit_ARG Zero quaternion cannot be normalized");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::unit_ARG Return object not of type Quaternionf");
   }
}

void _Quaternionf::_unitScale_YAC_RSELF(sF32 s) {
   sF32 absval=_getAbs();
   if (Dfltnonzero(absval)) {
      absval=s/absval;
      floats[0]*=absval;
      floats[1]*=absval;
      floats[2]*=absval;
      floats[3]*=absval;
   } else {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaternionf::unitScale_SELF Zero quaternion cannot be scaled");
   }
}

void _Quaternionf::_unitScale_YAC_RVAL(sF32 s, YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   _unitScale_YAC_RARG(s,q);
   _r->initObject(q,1);
}

void _Quaternionf::_unitScale_YAC_RARG(sF32 s, YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Quaternionf)) {
      sF32 absval=_getAbs();
      if (Dfltnonzero(absval)) {
         _Quaternionf *r=(_Quaternionf*)_r;
         absval=s/absval;
	 r->floats[0] = floats[0] * absval;
	 r->floats[1] = floats[1] * absval;
	 r->floats[2] = floats[2] * absval;
	 r->floats[3] = floats[3] * absval;
      } else {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::unitScale_ARG Zero quaternion cannot be scaled");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::unitScale_ARG Return object not of type Quaternionf");
   }
}

void _Quaternionf::_setAngle(sF32 a) {
   YAC_NEW_STACK(Quaternionf,v);
   sF32 radius=_getAbs();
   sF32 sa=sinf(a);
   _getPureUnit_YAC_RARG(&v);
   floats[0]=radius*cosf(a);
   floats[1]=radius*v.floats[1]*sa;
   floats[2]=radius*v.floats[2]*sa;
   floats[3]=radius*v.floats[3]*sa;
}

void _Quaternionf::_setRadius(sF32 r) {
   _unitScale_YAC_RSELF(r);
}

void _Quaternionf::_setPureUnit(YAC_Object *_v) {
   if (YAC_CHK(_v, clid_Quaternionf)) {
      _Quaternionf *v=(_Quaternionf*)_v;
      if (Dfltnonzero(v->floats[0])&&(Dfltequal(v->_getAbs(),1.0f))) {
         sF32 radius=_getAbs();
	 if (Dfltnonzero(radius)) {
	    sF32 angle = acosf(floats[0]/radius);
	    sF32 rsa = radius * sinf(angle);
	    floats[1] = v->floats[1] * rsa;
	    floats[2] = v->floats[2] * rsa;
	    floats[3] = v->floats[3] * rsa;
	 } else {
	    Dyac_throw_def(TkMathDivisionByZero, "Cannot set pure unit vector for zero quaternion");
	 }
      } else {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Quaternionf::setPureUnit Object not a pure unit quaternion (real==0 and length==1)");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::setPureUnit Object not of type Quaternionf");
   }
}

sF32 _Quaternionf::_getAngle() {
   sF32 absval=_getAbs();
   if (Dfltnonzero(absval)) {
      return acosf(floats[0]/absval);
   } else {
     Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaternionf::getAngle Cannot determine of zero quaternion");
   }
   return 0.0f;
}

sF32 _Quaternionf::_getRadius() {
  return _getAbs();
}

void _Quaternionf::_getPureUnit_YAC_RVAL(YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   _getPureUnit_YAC_RARG(q);
   _r->initObject(q,1);
}

void _Quaternionf::_getPureUnit_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Quaternionf)) {
      _Quaternionf *r=(_Quaternionf*)_r;
      sF32 radius=_getAbs();
      if (Dfltnonzero(radius)) {
         sF32 angle=acosf(floats[0]/radius);
         sF32 scale=1.0f/(radius*sinf(angle));
         r->floats[0]=0.0f; //this makes it pure
         r->floats[1]=floats[1]*scale;
         r->floats[2]=floats[2]*scale;
         r->floats[3]=floats[3]*scale;
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Quaternionf::getPureUnit_ARG Cannot get pure unit vector from zero quaternion");
      }
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::getPureUnit_ARG Return object not of type Quaternionf");
   }
}

void _Quaternionf::_getMatrix4f_YAC_RVAL(YAC_Value *_r) {
   _Matrix4f *m=YAC_NEW_POOLED(Matrix4f);
   _getMatrix4f_YAC_RARG(m);
   _r->initObject(m,1);
}

void _Quaternionf::_getMatrix4f_YAC_RARG(YAC_Object *_r) {
   if (YAC_CHK(_r,clid_Matrix4f)) {
      _Matrix4f *m = (_Matrix4f*) _r;
      m->floats[ 0] = floats[0]; m->floats[ 1] = floats[1]; m->floats[ 2] = floats[2]; m->floats[ 3] = floats[3];
      m->floats[ 4] =-floats[1]; m->floats[ 5] = floats[0]; m->floats[ 6] =-floats[3]; m->floats[ 7] = floats[2];
      m->floats[ 8] =-floats[2]; m->floats[ 9] = floats[3]; m->floats[10] = floats[0]; m->floats[11] =-floats[1];
      m->floats[12] =-floats[3]; m->floats[13] =-floats[2]; m->floats[14] = floats[1]; m->floats[15] = floats[0];
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::getMatrix4f Return object not of type Matrix4f");
   }
}

void _Quaternionf::_getRotateMatrix4f_YAC_RVAL(YAC_Value *_r) {
   _Matrix4f *m=YAC_NEW_POOLED(Matrix4f);
   _getRotateMatrix4f_YAC_RARG(m);
   _r->initObject(m,1);
}

void _Quaternionf::_getRotateMatrix4f_YAC_RARG(YAC_Object *_m) {
   if (YAC_CHK(_m,clid_Matrix4f)) {
      _Matrix4f *m=(_Matrix4f*)_m;
      sF32 xx,yy,zz;
      sF32 wx2,wy2,wz2,xy2,xz2,yz2;
      xx=2.0f*floats[1]*floats[1];
      yy=2.0f*floats[2]*floats[2];
      zz=2.0f*floats[3]*floats[3];
      wx2=2.0f*floats[0]*floats[1];
      wy2=2.0f*floats[0]*floats[2];
      wz2=2.0f*floats[0]*floats[3];
      xy2=2.0f*floats[1]*floats[2];
      xz2=2.0f*floats[1]*floats[3];
      yz2=2.0f*floats[2]*floats[3];
      m->floats[0] = 1.0f - yy - zz; m->floats[1] = xy2 - wz2;      m->floats[2] = xz2 + wy2;      m->floats[3] = 0.0f;
      m->floats[4] = xy2 + wz2;      m->floats[5] = 1.0f - xx - zz; m->floats[6] = yz2 - wx2;      m->floats[7] = 0.0f;
      m->floats[8] = xz2 - wy2;      m->floats[9] = yz2 + wx2;      m->floats[10]= 1.0f - xx - yy; m->floats[11]= 0.0f;
      m->floats[12]= 0.0f;           m->floats[13]= 0.0f;           m->floats[14]= 0.0f;           m->floats[15]= 1.0f;
   } else {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Quaternionf::getRotateMatrix4f Return object not of type Matrix4f");
   }
}

void _Quaternionf::_init(sF32 a, sF32 b, sF32 c, sF32 d) {
   floats[0]=a;
   floats[1]=b;
   floats[2]=c;
   floats[3]=d;
}

void _Quaternionf::_New(sF32 a, sF32 b, sF32 c, sF32 d, YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   q->_init(a,b,c,d);
   _r->initObject(q,1);
}

void YAC_CALL _quaternionf(sF32 a, sF32 b, sF32 c, sF32 d, YAC_Value *_r) {
   _Quaternionf *q=YAC_NEW_POOLED(Quaternionf);
   q->_init(a,b,c,d);
   _r->initObject(q,1);
}
