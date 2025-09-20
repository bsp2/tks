/// vector2d.cpp
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

// _Vector2d& _Vector2d::operator=(const _Vector2d& obj) {
//    doubles[0]=obj.doubles[0];
//    doubles[1]=obj.doubles[1];
//    return *this;
// }

_Vector2d::_Vector2d(void) {
   doubles[0] = doubles[1] = 0.0f;
}

_Vector2d::~_Vector2d() {
}

sUI _Vector2d::yacArrayGetNumElements(void) {
   return 2;
}

sUI _Vector2d::yacArrayGetMaxElements(void) {
   return 2;
}

sUI _Vector2d::yacArrayGetElementType(void) {
   return YAC_TYPE_OBJECT;
}

sUI _Vector2d::yacArrayGetElementByteSize(void) {
   return sizeof(sF64);
}

void _Vector2d::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 2)
   {
      sF64 v;
      switch(_val->type)
      {
         case YAC_TYPE_VOID:
            v = 0.0;
            break;

         case YAC_TYPE_INT:
            v = (sF64) _val->value.int_val;
            break;

         case YAC_TYPE_FLOAT:
            v = (sF64) _val->value.float_val;
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
            Dyac_throw(TypeMismatch, "tkmath::Vector2d::yacArraySet: unknown type");
            v = 0.0;
            break;
      }
      doubles[_idx] = v;
   }
   else
   {
      static char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Vector2d::yacArraySet: Index too high (%u>1)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Vector2d::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 2u)
   {
      YAC_Double *v = (YAC_Double *) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
      v->value = doubles[_idx];
      _val->initObject(v,1);
   }
   else
   {
      static char buf[256];
      Dyac_snprintf(buf, 256, "tkmath::Vector2d::yacArrayGet: Index too high (%u>1)", _idx);
      _val->initFloat(0.0f);
      Dyac_throw(ReadArrayOutOfBounds, buf);
   }
}

void *YAC_VCALL _Vector2d::yacArrayGetPointer(void) {
   return (void*)doubles;
}

sSI YAC_VCALL _Vector2d::yacTensorRank(void) {
   return YAC_TENSOR_RANK_VECTOR;
}

void YAC_VCALL _Vector2d::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_F64(doubles[0]);
   YAC_SERIALIZE_F64(doubles[1]);
}

sUI YAC_VCALL _Vector2d::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   doubles[0] = YAC_DESERIALIZE_F64();
   doubles[1] = YAC_DESERIALIZE_F64();
   return 1u;
}

void YAC_VCALL _Vector2d::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector2d))
   {
      if(YAC_BCHK(_max, clid_Vector2d))
      {
         YAC_CAST_ARG(_Vector2d, min, _min);
         YAC_CAST_ARG(_Vector2d, max, _max);

         if(doubles[0] < min->doubles[0])
            doubles[0] = min->doubles[0];
         else if(doubles[0] > max->doubles[0])
            doubles[0] = max->doubles[0];

         if(doubles[1] < min->doubles[1])
            doubles[1] = min->doubles[1];
         else if(doubles[1] > max->doubles[1])
            doubles[1] = max->doubles[1];
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector2d::yacOperatorClamp: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector2d::yacOperatorClamp: invalid _min object.");
   }
}

void YAC_VCALL _Vector2d::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_BCHK(_min, clid_Vector2d))
   {
      if(YAC_BCHK(_max, clid_Vector2d))
      {
         YAC_CAST_ARG(_Vector2d, min, _min);
         YAC_CAST_ARG(_Vector2d, max, _max);

         if(doubles[0] < min->doubles[0])
            doubles[0] += (max->doubles[0] - min->doubles[0]);
         else if(doubles[0] > max->doubles[0])
            doubles[0] -= (max->doubles[0] - min->doubles[0]);

         if(doubles[1] < min->doubles[1])
            doubles[1] += (max->doubles[1] - min->doubles[1]);
         else if(doubles[1] > max->doubles[1])
            doubles[1] -= (max->doubles[1] - min->doubles[1]);
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "tkmath::Vector2d::yacOperatorWrap: invalid _max object.");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector2d::yacOperatorWrap: invalid _min object.");
   }
}

void YAC_VCALL _Vector2d::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      switch(_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI(Ddblequal(doubles[0], o->doubles[0]) &&
                     Ddblequal(doubles[1], o->doubles[1])
                     );
            break;

         case YAC_OP_CNE:
            YAC_RETI(Ddblnotequal(doubles[0], o->doubles[0]) ||
                     Ddblnotequal(doubles[1], o->doubles[1])
                     );
            break;

         case YAC_OP_ASSIGN:
            doubles[0] = o->doubles[0];
            doubles[1] = o->doubles[1];
            break;

         case YAC_OP_ADD:
            _add_YAC_RSELF(_o);
            break;

         case YAC_OP_SUB:
            _sub_YAC_RSELF(_o);
            break;

            //individual multiplication of elements at this place would only be
            //irritating, so it is currently disabled
            /*case YAC_OP_MUL:
           _mul(_o);
           break;*/

         case YAC_OP_NEG:
            doubles[0] = -doubles[0];
            doubles[1] = -doubles[1];
            break;

         case YAC_OP_INIT:
            doubles[0] = 0.0;
            doubles[1] = 0.0;
            break;

         default:
         {
            char buf[128];
            Dyac_snprintf(buf, 128, "tkmath::Vector2d::yacOperator unsupported cmd %d", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
         }
         return;
      }
   }
   else if(YAC_Is_FloatArray(_o))
   {
      YAC_CAST_ARG(YAC_FloatArray, o, _o);
      switch(_cmd)
      {
         case YAC_OP_ASSIGN:
            assignFloatArray(o);
            break;

         default:
         {
            // Construct temporary Vector2d object
            _Vector2d t;
            t.class_ID = clid_Vector2d;
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
      sF64 v;
      if(_o->yacScanF64(&v))
      {
         yacOperatorF64(_cmd, v, _r);
      }
      else
      {
         // Handle generic array object
         switch(_cmd)
         {
            case YAC_OP_ASSIGN:
               assignGeneric(NULL, _o);
               break;

            default:
            {
               /* // Construct temporary Vector2d object
              _Vector2d t;
              t.class_ID = clid_Vector2d;
              t.assignGeneric(NULL, _o);*/

               // Recursively call yacOperator
               YAC_Object::yacOperator(_cmd, _o, _r);
            }
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
            break;

         default:
            // Not a valid Object (e.g. <null>)
            YAC_Object::yacOperator(_cmd,_o,_r);
            break;
      }
   }
}

void _Vector2d::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void _Vector2d::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   yacOperatorF64(_cmd, (sF64)val, _r);
}

void _Vector2d::yacOperatorF64(sSI _cmd, sF64 val, YAC_Value *_r) {
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
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2d::yacOperatorF64 OP_DIV Cannot divide by zero");
         }
         break;

      default:
         char buf[128];
         Dyac_snprintf(buf, 128, "tkmath::Vector2d::yacOperatorF64 Operator not supported %d",_cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
         break;
   }
}

void _Vector2d::assignFloats(const sF32 *_fv, sUI _ne) {
   if(_ne <= 2u)
   {
      // Copy as much elements from array as possible but never more than 2
      sUI i;
      for(i = 0u; i < _ne; i++)
         doubles[i] = (sF64) _fv[i];

      // Fill the rest with 0.0f
      for(; i < 2u; i++)
         doubles[i] = 0.0;
   }
   else
   {
      // Copy two elements
      doubles[0] = (sF64) _fv[0];
      doubles[1] = (sF64) _fv[1];
   }
}

void _Vector2d::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Vector2d::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
   sUI ne = _o->yacArrayGetNumElements();
   if(ne > 0u)
   {
      if(YAC_TYPE_FLOAT == _o->yacArrayGetElementType())
      {
         const sF32 *fv = (const sF32*) _o->yacArrayGetPointer();
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
            for(i = 0u; i < 2u && i < ne; i++)
            {
               _o->yacArrayGet(_ctx, i, &v);
               doubles[i] = (sF64) v.value.float_val;
            }
            // Fill the rest with 0.0f
            for(; i < 2u; i++)
               doubles[i] = 0.0;
         }
      }
      else
      {
         // Retrieve individual elements, typecast to float and assign
         YAC_Value v;
         sUI i;
         for(i = 0u; i < 2u && i < ne; i++)
         {
            _o->yacArrayGet(_ctx, i, &v);
            v.typecast(YAC_TYPE_OBJECT);
            if (!v.value.object_val->yacScanF64(&doubles[i])) doubles[i]=0.0;
            v.unset();
         }
         // Fill the rest with 0.0f
         for(; i < 2u; i++)
            doubles[i] = 0.0;
      }
   }
   else
   {
      // Other array is empty or no array. Silently assign 0.0f
      doubles[0] = doubles[1] = 0.0;
   }
}


//direct access to elements methods
void _Vector2d::_setX(YAC_Object *x) {
   if(YAC_VALID(x))
   {
      if(!x->yacScanF64(&doubles[0]))
         doubles[0] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::setX Object not valid");
   }
}

void _Vector2d::_setY(YAC_Object *y) {
   if(YAC_VALID(y))
   {
      if(!y->yacScanF64(&doubles[1]))
         doubles[1] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::setY Object not valid");
   }
}

void _Vector2d::_getX_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(r, YAC_TRUE);
   r->value = doubles[0];
}

void _Vector2d::_getX_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[0]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::getX Return object not valid");
   }
}

void _Vector2d::_getY_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(r,1);
   r->value = doubles[1];
}

void _Vector2d::_getY_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(doubles[1]);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::getY Return object not valid");
   }
}

sBool YAC_VCALL _Vector2d::yacToString(YAC_String *s) const {
   // generate string representation
   s->yacArrayAlloc(128, 0,0,0);
   s->printf("(%g, %g)", doubles[0], doubles[1]);
   s->fixLength();
   return YAC_TRUE;
}

//create string object and fill with string representation
void _Vector2d::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
   YAC_RETS(s, YAC_TRUE);
   yacToString(s);
}

void _Vector2d::_add_YAC_RSELF(YAC_Object *_o) {
   // add two vector2f
   if(YAC_BCHK(_o, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      doubles[0] += o->doubles[0];
      doubles[1] += o->doubles[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2d::add_SELF: invalid Vector2d object");
   }
   /* else
   {
      // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);
   }*/
}

void _Vector2d::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   // add two vector2f
   if(YAC_BCHK(_o, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      _Vector2d *r = YAC_NEW_POOLED(Vector2d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[0] + o->doubles[0];
      r->doubles[1] = doubles[1] + o->doubles[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2d::add_VAL: invalid Vector2d object");
   }
   /* else
   {
      // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);
   }*/
}

void _Vector2d::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   // add two vector2f
   if(YAC_BCHK(_o, clid_Vector2d) && YAC_BCHK(_r, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      YAC_CAST_ARG(_Vector2d, r, _r);
      r->doubles[0] = doubles[0] + o->doubles[0];
      r->doubles[1] = doubles[1] + o->doubles[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2d::add_ARG: invalid Vector2d object");
   }
   /* else
   {
      // Add generic Object
      YAC_Value r;
      yacOperator(YAC_OP_ADD, _o, &r);
   }*/
}

void _Vector2d::_sub_YAC_RSELF(YAC_Object *_o) {
   // sub two vector2f
   if(YAC_BCHK(_o, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      doubles[0] -= o->doubles[0];
      doubles[1] -= o->doubles[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::sub_SELF Invalid Vector2d object");
   }
   /* else
   {
      // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_SUB, _o, &r);
   }*/
}

void _Vector2d::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   // sub two vector2f
   if(YAC_BCHK(_o, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      _Vector2d *r = YAC_NEW_POOLED(Vector2d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[0] - o->doubles[0];
      r->doubles[1] = doubles[1] - o->doubles[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::sub_VAL invalid Vector2d object");
   }
   /* else
   {
      // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_SUB, _o, &r);
   }*/
}

void _Vector2d::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   // sub two vector2f
   if(YAC_BCHK(_o, clid_Vector2d) && YAC_BCHK(_r, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      YAC_CAST_ARG(_Vector2d, r, _r);
      r->doubles[0] = doubles[0] - o->doubles[0];
      r->doubles[1] = doubles[1] - o->doubles[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2f::sub: invalid Vector2d object");
   }
   /* else
   {
      // Subtract generic Object
      YAC_Value r;
      yacOperator(YAC_OP_SUB, _o, &r);
   }*/
}

sF64 _Vector2d::dot(YAC_Object *_o) {
   // We assume that this is already check before this gets called
   YAC_CAST_ARG(_Vector2d, o, _o);
   return (doubles[0] * o->doubles[0] + doubles[1] * o->doubles[1]);
}

void _Vector2d::_dot_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector2d))
   {
      YAC_Double *r = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
      _r->initObject(r, YAC_TRUE);
      r->value = dot(_o);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2d::dot_RVAL Object not of type Vector2d");
   }
}

void _Vector2d::_dot_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      if(YAC_BCHK(_o, clid_Vector2d))
      {
         _r->yacValueOfF64(dot(_o));
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2d::dot_RVAL Object not of type Vector2d");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2d::dot_RARG Return object not valid");
   }
}

void _Vector2d::_tensor_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      _Matrix2d *m = YAC_NEW_POOLED(Matrix2d);
      _r->initObject(m, YAC_TRUE);
      m->doubles[0] = doubles[0] * o->doubles[0];
      m->doubles[1] = doubles[0] * o->doubles[1];
      m->doubles[2] = doubles[1] * o->doubles[0];
      m->doubles[3] = doubles[1] * o->doubles[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2d::tensor_VAL Invalid Vector2d object");
   }
}

void _Vector2d::_tensor_YAC_RARG(YAC_Object *_o, YAC_Object *_m) {
   if(YAC_CHK(_m, clid_Matrix2d))
   {
      _Matrix2d *m = (_Matrix2d*) _m;
      if(YAC_BCHK(_o, clid_Vector2d))
      {
         YAC_CAST_ARG(_Vector2d, o, _o);
         m->doubles[0] = doubles[0] * o->doubles[0];
         m->doubles[1] = doubles[0] * o->doubles[1];
         m->doubles[2] = doubles[1] * o->doubles[0];
         m->doubles[3] = doubles[1] * o->doubles[1];
      }
      /* else
      {
         _Vector2d t;
         t.assignGeneric(NULL, _o);
         o = &t;
      }*/
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2d::tensor_ARG Invalid Vector2d object");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2d::tensor_ARG: Invalid Matrix2d return object");
   }
}

void _Vector2d::muld(sF64 s) {
   // multiply vector2f with scalar
   doubles[0] *= s;
   doubles[1] *= s;
}

void _Vector2d::_muld_YAC_RSELF(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      doubles[0] *= s;
      doubles[1] *= s;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::muld_SELF Object not valid");
   }
}

void _Vector2d::_muld_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   _Vector2d *r = YAC_NEW_POOLED(Vector2d);
   _r->initObject(r, YAC_TRUE);
   if(YAC_VALID(_s))
   {
      sF64 s;
      if (!_s->yacScanF64(&s))
         s = 0.0;
      r->doubles[0] = doubles[0] * s;
      r->doubles[1] = doubles[1] * s;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::muld_VAL Object not valid");
   }
}

void _Vector2d::_muld_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_VALID(_s))
   {
      if(YAC_BCHK(_r, clid_Vector2d))
      {
         YAC_CAST_ARG(_Vector2d, r, _r);
         sF64 s;
         if(!_s->yacScanF64(&s))
            s = 0.0;
         r->doubles[0] = doubles[0] * s;
         r->doubles[1] = doubles[1] * s;
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2d::muld_ARG Return object not of type Vector2d");
      }
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::muld_ARG Object not valid");
   }
}

void _Vector2d::_mul_YAC_RSELF(YAC_Object *_o) {
   // multiply both elements of each vector2f
   if(YAC_BCHK(_o, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      doubles[0] *= o->doubles[0];
      doubles[1] *= o->doubles[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2d::mul_SELF Invalid Vector2d object");
   }
   /* else
   {
      // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);
   }*/
}

void _Vector2d::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_BCHK(_o, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      _Vector2d *r = YAC_NEW_POOLED(Vector2d);
      _r->initObject(r, YAC_TRUE);
      r->doubles[0] = doubles[0] * o->doubles[0];
      r->doubles[1] = doubles[1] * o->doubles[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2d::mul_VAL Invalid Vector2d object");
   }
   /* else
   {
      // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);
   }*/
}

void _Vector2d::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_BCHK(_o, clid_Vector2d) && YAC_BCHK(_r, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, o, _o);
      YAC_CAST_ARG(_Vector2d, r, _r);
      r->doubles[0] = doubles[0] * o->doubles[0];
      r->doubles[1] = doubles[1] * o->doubles[1];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Vector2d::mul_ARG Invalid Vector2d object");
   }
   /* else
   {
      // Multiply by generic Object
      YAC_Value r;
      yacOperator(YAC_OP_MUL, _o, &r);
   }*/
}

sF64 _Vector2d::getAbs(void) {
   // return length of vector2f
   // could use math.h hypoth function instead
   return sqrt(doubles[0]*doubles[0] + doubles[1]*doubles[1]);
}

void _Vector2d::_getAbs_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   _r->initObject(r, YAC_TRUE);
   r->value = getAbs();
}

void _Vector2d::_getAbs_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(getAbs());
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::getAbs_ARG Return object not valid");
   }
}

sF64 _Vector2d::getAbsSqr(void) {
   // return squared length of vector2f - faster and does the job as well as getAbs
   // in many circumstances
   return doubles[0] * doubles[0] + doubles[1] * doubles[1];
}

void _Vector2d::_getAbsSqr_YAC_RVAL(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double*)YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   r->value = getAbsSqr();
   _r->initObject(r, YAC_TRUE);
}

void _Vector2d::_getAbsSqr_YAC_RARG(YAC_Object *_r) {
   if(YAC_VALID(_r))
   {
      _r->yacValueOfF64(getAbsSqr());
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::getAbsSqr_ARG Return object not valid");
   }
}

void _Vector2d::_unit_YAC_RSELF(void) {
   // scale vector2f to length 1.0f
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      absval = 1.0 / absval;
      doubles[0] *= absval;
      doubles[1] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2d::unit_SELF Zero Vector cannot be normalized");
      doubles[0] = 0.0f;
      doubles[1] = 0.0f;
   }
}

void _Vector2d::_unit_YAC_RVAL(YAC_Value *_r) {
   _Vector2d *r = YAC_NEW_POOLED(Vector2d);
   _r->initObject(r, YAC_TRUE);
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      absval = 1.0 / absval;
      r->doubles[0] = doubles[0] * absval;
      r->doubles[1] = doubles[1] * absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2d::unit_VAL Zero Vector cannot be normalized");
   }
}

void _Vector2d::_unit_YAC_RARG(YAC_Object *_r) {
   if(YAC_BCHK(_r, clid_Vector2d))
   {
      _Vector2d *r = (_Vector2d*)_r;
      sF64 absval = getAbs();
      if(Ddblnonzero(absval))
      {
         absval = 1.0 / absval;
         r->doubles[0] = doubles[0] * absval;
         r->doubles[1] = doubles[1] * absval;
      }
      else
      {
         // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2d::unit_ARG Zero Vector cannot be normalized");
         r->doubles[0] = 0.0f;
         r->doubles[1] = 0.0f;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2d::unit_ARG Return object not of type Vector2d");
   }
}

void _Vector2d::unitScale(sF64 s) {
   // scale vector2f to length s
   sF64 absval = getAbs();
   if(Ddblnonzero(absval))
   {
      absval = s / absval;
      doubles[0] *= absval;
      doubles[1] *= absval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Vector2d::unitScale Zero Vector cannot be normalized");
      doubles[0] = 0.0f;
      doubles[1] = 0.0f;
   }
}

void _Vector2d::_unitScale_YAC_RSELF(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      unitScale(s);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::unitScale_SELF Object not valid");
   }
}

void _Vector2d::_unitScale_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   if(YAC_VALID(_s))
   {
      _Vector2d *r = YAC_NEW_POOLED(Vector2d);
      _r->initObject(r, YAC_TRUE);
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      r->doubles[0] = doubles[0];
      r->doubles[1] = doubles[1];
      r->unitScale(s);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::unitScale_VAL Object not valid");
   }
}

void _Vector2d::_unitScale_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_BCHK(_r, clid_Vector2d))
   {
      YAC_CAST_ARG(_Vector2d, r, _r);
      sF64 s;
      if(!_s->yacScanF64(&s))
         s = 0.0;
      r->doubles[0] = doubles[0];
      r->doubles[1] = doubles[1];
      r->unitScale(s);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Vector2d::unitScale_ARG Return object not of type Vector2d");
   }
}

// initialise vector2f value methods

void _Vector2d::_init(YAC_Object *_x, YAC_Object *_y) {
   // init both elements with individual values
   if(YAC_VALID(_x) && YAC_VALID(_y))
   {
      if(!_x->yacScanF64(&doubles[0]))
         doubles[0] = 0.0;

      if(!_y->yacScanF64(&doubles[1]))
         doubles[1] = 0.0;
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::init Objects not valid");
   }
}

void _Vector2d::_initf(sF32 x, sF32 y) {
   doubles[0] = x;
   doubles[1] = y;
}

void _Vector2d::_initScaled(YAC_Object *_o, YAC_Object *_s) {
   // init with a directional vector _o and length _s
   if(YAC_BCHK(_o, clid_Vector2d))
   {
      if(YAC_VALID(_s))
      {
         YAC_CAST_ARG(_Vector2d, o, _o);
         sF64 s;
         if (!_s->yacScanF64(&s))
            s = 0.0;
         doubles[0] = o->doubles[0] * s;
         doubles[1] = o->doubles[1] * s;
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue, "tkmath::Vector2d::initScaled Object not valid");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Vector2d::initScaled: invalid object parameter");
   }
}

void _Vector2d::_New(YAC_Object *x, YAC_Object *y, YAC_Value *_r) {
   _Vector2d *r = YAC_NEW_POOLED(Vector2d);
   _r->initObject(r, YAC_TRUE);
   r->_init(x,y);
}

void YAC_CALL _vector2d(YAC_Object *x, YAC_Object *y,YAC_Value *_r) {
   _Vector2d *r = YAC_NEW_POOLED(Vector2d);
   _r->initObject(r, YAC_TRUE);
   r->_init(x,y);
}
