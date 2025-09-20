/// matrix4f.cpp
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
#include "vector4f.h"
#include "matrix4f.h"
#include "matrix3f.h"
#include "matrix2x3f.h"
#include "matrix2f.h"
#include "quaternionf.h"

#include "ying_Math.h"


#ifdef USE_DYNAMIC_ORDER
const sU8 matrix4_element_order_table[2][4][4] = {
   // Row major (default)
   {
      { 0,  1,  2,  3},
      { 4,  5,  6,  7},
      { 8,  9, 10, 11},
      {12, 13, 14, 15}
   },

   // Column major (OpenGL)
   {
      { 0,  4,  8, 12},
      { 1,  5,  9, 13},
      { 2,  6, 10, 14},
      { 3,  7, 11, 15}
   }
};
#endif // USE_DYNAMIC_ORDER


_Matrix4f::_Matrix4f(void) {
   _initIdentity();
}

_Matrix4f::~_Matrix4f() {
}


// void _Matrix4f::_setColumnMajor(sSI _bEnabled) {
// }

// sSI _Matrix4f::_getColumnMajor(void) {
// }


sUI YAC_VCALL _Matrix4f::yacArrayGetNumElements(void) {
   return 16u;
}

sUI YAC_VCALL _Matrix4f::yacArrayGetMaxElements(void) {
   return 16u;
}

sUI YAC_VCALL _Matrix4f::yacArrayGetElementType(void) {
   return YAC_TYPE_FLOAT;
}

sUI YAC_VCALL _Matrix4f::yacArrayGetElementByteSize(void) {
   return sizeof(sF32);
}

void YAC_VCALL _Matrix4f::yacArraySet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 16u)
   {
      sF32 v = 0.0f;
      switch (_val->type)
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
            if(YAC_VALID(_val->value.object_val))
            {
               (void)_val->value.object_val->yacScanF32(&v);
            }
            break;

         default:
            Dyac_throw(TypeMismatch, "tkmath::Matrix4f::yacArraySet: invalid type");
            break;
      }
      floats[_idx] = v;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix4f::yacArraySet: index out of bounds (%u > 15)", _idx);
      Dyac_throw(WriteArrayOutOfBounds, buf);
   }
}

void YAC_VCALL _Matrix4f::yacArrayGet(YAC_ContextHandle _ctx, sUI _idx, YAC_Value *_val) {
   if(_idx < 16u)
   {
      _val->initFloat(floats[_idx]);
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix4f::yacArrayGet index out of bounds (%u > 15)", _idx);
      Dyac_throw(ReadArrayOutOfBounds, buf);
      _val->initFloat(0.0f);
   }
}

void *YAC_VCALL _Matrix4f::yacArrayGetPointer(void) {
   return (void*)floats;
}

sUI YAC_VCALL _Matrix4f::yacArrayGetWidth(void) {
   return 4u;
}

sUI YAC_VCALL _Matrix4f::yacArrayGetHeight(void) {
   return 4u;
}

sUI YAC_VCALL _Matrix4f::yacArrayGetStride(void) {
   return 4u * sizeof(sF32);
}

sSI YAC_VCALL _Matrix4f::yacTensorRank(void) {
   return YAC_TENSOR_RANK_MATRIX;
}

void YAC_VCALL _Matrix4f::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_I32(b_column_major);
   YAC_SERIALIZE_F32(floats[ 0]);
   YAC_SERIALIZE_F32(floats[ 1]);
   YAC_SERIALIZE_F32(floats[ 2]);
   YAC_SERIALIZE_F32(floats[ 3]);
   YAC_SERIALIZE_F32(floats[ 4]);
   YAC_SERIALIZE_F32(floats[ 5]);
   YAC_SERIALIZE_F32(floats[ 6]);
   YAC_SERIALIZE_F32(floats[ 7]);
   YAC_SERIALIZE_F32(floats[ 8]);
   YAC_SERIALIZE_F32(floats[ 9]);
   YAC_SERIALIZE_F32(floats[10]);
   YAC_SERIALIZE_F32(floats[11]);
   YAC_SERIALIZE_F32(floats[12]);
   YAC_SERIALIZE_F32(floats[13]);
   YAC_SERIALIZE_F32(floats[14]);
   YAC_SERIALIZE_F32(floats[15]);
}

sUI YAC_VCALL _Matrix4f::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   sBool bCM;

   YAC_BEG_DESERIALIZE();

   // Deserialize column/row format, 1=column major, 0=row major
   bCM = YAC_DESERIALIZE_I32();

#ifdef USE_DYNAMIC_ORDER
   b_column_major = bCM;
   floats[ 0] = YAC_DESERIALIZE_F32();
   floats[ 1] = YAC_DESERIALIZE_F32();
   floats[ 2] = YAC_DESERIALIZE_F32();
   floats[ 3] = YAC_DESERIALIZE_F32();
   floats[ 4] = YAC_DESERIALIZE_F32();
   floats[ 5] = YAC_DESERIALIZE_F32();
   floats[ 6] = YAC_DESERIALIZE_F32();
   floats[ 7] = YAC_DESERIALIZE_F32();
   floats[ 8] = YAC_DESERIALIZE_F32();
   floats[ 9] = YAC_DESERIALIZE_F32();
   floats[10] = YAC_DESERIALIZE_F32();
   floats[11] = YAC_DESERIALIZE_F32();
   floats[12] = YAC_DESERIALIZE_F32();
   floats[13] = YAC_DESERIALIZE_F32();
   floats[14] = YAC_DESERIALIZE_F32();
   floats[15] = YAC_DESERIALIZE_F32();
#else
   if(bCM)
   {
      // Column major
      floats[ 0] = YAC_DESERIALIZE_F32();
      floats[ 1] = YAC_DESERIALIZE_F32();
      floats[ 2] = YAC_DESERIALIZE_F32();
      floats[ 3] = YAC_DESERIALIZE_F32();
      floats[ 4] = YAC_DESERIALIZE_F32();
      floats[ 5] = YAC_DESERIALIZE_F32();
      floats[ 6] = YAC_DESERIALIZE_F32();
      floats[ 7] = YAC_DESERIALIZE_F32();
      floats[ 8] = YAC_DESERIALIZE_F32();
      floats[ 9] = YAC_DESERIALIZE_F32();
      floats[10] = YAC_DESERIALIZE_F32();
      floats[11] = YAC_DESERIALIZE_F32();
      floats[12] = YAC_DESERIALIZE_F32();
      floats[13] = YAC_DESERIALIZE_F32();
      floats[14] = YAC_DESERIALIZE_F32();
      floats[15] = YAC_DESERIALIZE_F32();
   }
   else
   {
      // Convert row to column major
      floats[ 0] = YAC_DESERIALIZE_F32();
      floats[ 4] = YAC_DESERIALIZE_F32();
      floats[ 8] = YAC_DESERIALIZE_F32();
      floats[12] = YAC_DESERIALIZE_F32();
      floats[ 1] = YAC_DESERIALIZE_F32();
      floats[ 5] = YAC_DESERIALIZE_F32();
      floats[ 9] = YAC_DESERIALIZE_F32();
      floats[13] = YAC_DESERIALIZE_F32();
      floats[ 2] = YAC_DESERIALIZE_F32();
      floats[ 6] = YAC_DESERIALIZE_F32();
      floats[10] = YAC_DESERIALIZE_F32();
      floats[14] = YAC_DESERIALIZE_F32();
      floats[ 3] = YAC_DESERIALIZE_F32();
      floats[ 7] = YAC_DESERIALIZE_F32();
      floats[11] = YAC_DESERIALIZE_F32();
      floats[15] = YAC_DESERIALIZE_F32();
   }
#endif // USE_DYNAMIC_ORDER
   return 1u;
}

void YAC_VCALL _Matrix4f::yacOperatorClamp(YAC_Object *_min, YAC_Object *_max) {
   if(YAC_CHK(_min,clid_Matrix4f) && YAC_CHK(_max,clid_Matrix4f))
   {
      _Matrix4f *min = (_Matrix4f*)_min;
      _Matrix4f *max = (_Matrix4f*)_max;

#define Dclamp(r,c)                      \
      if(TM4F(r,c) < M4F(min,r,c))       \
      {                                  \
         TM4F(r,c) = M4F(min,r,c);       \
      }                                  \
      else if(TM4F(r,c) > M4F(max,r,c))  \
      {                                  \
         TM4F(r,c) = M4F(max,r,c);       \
      }

      Dclamp(0,0) Dclamp(0,1) Dclamp(0,2) Dclamp(0,3)
      Dclamp(1,0) Dclamp(1,1) Dclamp(1,2) Dclamp(1,3)
      Dclamp(2,0) Dclamp(2,1) Dclamp(2,2) Dclamp(2,3)
      Dclamp(3,0) Dclamp(3,1) Dclamp(3,2) Dclamp(3,3)

#undef Dclamp
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Matrix4f::yacOperatorClamp: invalid object(s)");
   }
}

void YAC_VCALL _Matrix4f::yacOperatorWrap(YAC_Object *_min, YAC_Object *_max) {
   if (YAC_CHK(_min,clid_Matrix4f)&&YAC_CHK(_max,clid_Matrix4f)) {
      _Matrix4f *min = (_Matrix4f*)_min;
      _Matrix4f *max = (_Matrix4f*)_max;

#define Dwrap(r,c)\
      if(TM4F(r,c) < M4F(min,r,c))                 \
      {                                            \
         TM4F(r,c) += M4F(max,r,c) - M4F(min,r,c); \
      }                                            \
      else if(TM4F(r,c) > M4F(max,r,c))            \
      {                                            \
         TM4F(r,c) -= M4F(max,r,c) - M4F(min,r,c); \
      }

      Dwrap(0,0) Dwrap(0,1) Dwrap(0,2) Dwrap(0,3)
      Dwrap(1,0) Dwrap(1,1) Dwrap(1,2) Dwrap(1,3)
      Dwrap(2,0) Dwrap(2,1) Dwrap(2,2) Dwrap(2,3)
      Dwrap(3,0) Dwrap(3,1) Dwrap(3,2) Dwrap(3,3)

#undef Dwrap
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "tkmath::Matrix4f::yacOperatorWrap: invalid object(s)");
   }
}

void YAC_VCALL _Matrix4f::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      switch (_cmd)
      {
         case YAC_OP_CEQ:
            YAC_RETI( Dfltequal(TM4F(0,0), OM4F(0,0)) &&
                      Dfltequal(TM4F(1,0), OM4F(1,0)) &&
                      Dfltequal(TM4F(2,0), OM4F(2,0)) &&
                      Dfltequal(TM4F(3,0), OM4F(3,0)) &&
                      Dfltequal(TM4F(0,1), OM4F(0,1)) &&
                      Dfltequal(TM4F(1,1), OM4F(1,1)) &&
                      Dfltequal(TM4F(2,1), OM4F(2,1)) &&
                      Dfltequal(TM4F(3,1), OM4F(3,1)) &&
                      Dfltequal(TM4F(0,2), OM4F(0,2)) &&
                      Dfltequal(TM4F(1,2), OM4F(1,2)) &&
                      Dfltequal(TM4F(2,2), OM4F(2,2)) &&
                      Dfltequal(TM4F(3,2), OM4F(3,2)) &&
                      Dfltequal(TM4F(0,3), OM4F(0,3)) &&
                      Dfltequal(TM4F(1,3), OM4F(1,3)) &&
                      Dfltequal(TM4F(2,3), OM4F(2,3)) &&
                      Dfltequal(TM4F(3,3), OM4F(3,3))
                      );
            break;

         case YAC_OP_CNE:
            YAC_RETI( Dfltnotequal(TM4F(0,0), OM4F(0,0)) ||
                      Dfltnotequal(TM4F(1,0), OM4F(1,0)) ||
                      Dfltnotequal(TM4F(2,0), OM4F(2,0)) ||
                      Dfltnotequal(TM4F(3,0), OM4F(3,0)) ||
                      Dfltnotequal(TM4F(0,1), OM4F(0,1)) ||
                      Dfltnotequal(TM4F(1,1), OM4F(1,1)) ||
                      Dfltnotequal(TM4F(2,1), OM4F(2,1)) ||
                      Dfltnotequal(TM4F(3,1), OM4F(3,1)) ||
                      Dfltnotequal(TM4F(0,2), OM4F(0,2)) ||
                      Dfltnotequal(TM4F(1,2), OM4F(1,2)) ||
                      Dfltnotequal(TM4F(2,2), OM4F(2,2)) ||
                      Dfltnotequal(TM4F(3,2), OM4F(3,2)) ||
                      Dfltnotequal(TM4F(0,3), OM4F(0,3)) ||
                      Dfltnotequal(TM4F(1,3), OM4F(1,3)) ||
                      Dfltnotequal(TM4F(2,3), OM4F(2,3)) ||
                      Dfltnotequal(TM4F(3,3), OM4F(3,3))
                      );
            break;

         case YAC_OP_ASSIGN:
            TM4F(0,0) = OM4F(0,0);
            TM4F(1,0) = OM4F(1,0);
            TM4F(2,0) = OM4F(2,0);
            TM4F(3,0) = OM4F(3,0);
            TM4F(0,1) = OM4F(0,1);
            TM4F(1,1) = OM4F(1,1);
            TM4F(2,1) = OM4F(2,1);
            TM4F(3,1) = OM4F(3,1);
            TM4F(0,2) = OM4F(0,2);
            TM4F(1,2) = OM4F(1,2);
            TM4F(2,2) = OM4F(2,2);
            TM4F(3,2) = OM4F(3,2);
            TM4F(0,3) = OM4F(0,3);
            TM4F(1,3) = OM4F(1,3);
            TM4F(2,3) = OM4F(2,3);
            TM4F(3,3) = OM4F(3,3);
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
            for(sUI i = 0u; i < 16u; i++)
               floats[i] = -floats[i];
            break;

         case YAC_OP_INIT:
            break;

         default:
         {
            char buf[128];
            Dyac_snprintf(buf, 128, "tkmath::Matrix4f::yacOperator: unsupported cmd %d!\n", _cmd);
            Dyac_throw_def(TkMathUnsupportedOperator, buf);
            return;
         }

      }
   }
   else if(YAC_CHK(_o, clid_Matrix3f))
   {
      YAC_CAST_ARG(_Matrix3f, o, _o);
      switch (_cmd)
      {
         case YAC_OP_ASSIGN:
            TM4F(0,0) = OM3F(0,0);
            TM4F(0,1) = OM3F(0,1);
            TM4F(0,2) = OM3F(0,2);
            TM4F(0,3) = 0.0f;
            TM4F(1,0) = OM3F(1,0);
            TM4F(1,1) = OM3F(1,1);
            TM4F(1,2) = OM3F(1,2);
            TM4F(1,3) = 0.0f;
            TM4F(2,0) = OM3F(2,0);
            TM4F(2,1) = OM3F(2,1);
            TM4F(2,2) = OM3F(2,2);
            TM4F(2,3) = 0.0f;
            TM4F(3,0) = 0.0f;
            TM4F(3,1) = 0.0f;
            TM4F(3,2) = 0.0f;
            TM4F(3,3) = 1.0f;
            return;
      }
   }
   else if(YAC_CHK(_o, clid_Matrix2x3f))
   {
      YAC_CAST_ARG(_Matrix2x3f, o, _o);
      switch (_cmd)
      {
         case YAC_OP_ASSIGN:
            TM4F(0,0) = OM2x3F(0,0);
            TM4F(0,1) = OM2x3F(0,1);
            TM4F(0,2) = 0.0f;
            TM4F(0,3) = OM2x3F(0,2);
            TM4F(1,0) = OM2x3F(1,0);
            TM4F(1,1) = OM2x3F(1,1);
            TM4F(1,2) = 1.0f;
            TM4F(1,3) = OM2x3F(1,2);
            TM4F(2,0) = 0.0f;
            TM4F(2,1) = 0.0f;
            TM4F(2,2) = 1.0f;
            TM4F(2,3) = 0.0f;
            TM4F(3,0) = 0.0f;
            TM4F(3,1) = 0.0f;
            TM4F(3,2) = 0.0f;
            TM4F(3,3) = 1.0f;
            return;
      }
   }
   else if(YAC_CHK(_o, clid_Matrix2f))
   {
      YAC_CAST_ARG(_Matrix2f, o, _o);
      switch (_cmd)
      {
         case YAC_OP_ASSIGN:
            TM4F(0,0) = OM2F(0,0);
            TM4F(0,1) = OM2F(0,1);
            TM4F(0,2) = 0.0f;
            TM4F(0,3) = 0.0f;
            TM4F(1,0) = OM2F(1,0);
            TM4F(1,1) = OM2F(1,1);
            TM4F(1,2) = 0.0f;
            TM4F(1,3) = 0.0f;
            TM4F(2,0) = 0.0f;
            TM4F(2,1) = 0.0f;
            TM4F(2,2) = 1.0f;
            TM4F(2,3) = 0.0f;
            TM4F(3,0) = 0.0f;
            TM4F(3,1) = 0.0f;
            TM4F(3,2) = 0.0f;
            TM4F(3,3) = 1.0f;
            return;
      }
   }
   else if(YAC_CHK(_o, clid_Vector4f))
   {
      if(YAC_OP_MUL == _cmd)
      {
         YAC_CAST_ARG(_Vector4f, o, _o);
         _Vector4f *r = YAC_NEW_POOLED(Vector4f);
         _mulv_YAC_RARG(o, r);
         _r->initObject(r, YAC_TRUE);
      }
      else
      {
         // Unsupported mat4 <cmd> vec4
      }
      return;
   }
   else if(YAC_Is_FloatArray(_o))
   {
      // Assume that FloatArray uses same element order
      YAC_CAST_ARG(YAC_FloatArray, o, _o);
      switch(_cmd)
      {
         case YAC_OP_ASSIGN:
            if(o->num_elements < 16u)
            {
               sUI i;
               for(i = 0u; i < o->num_elements; i++)
                  floats[i] = o->elements[i];
               for(; i < 16u; i++)
                  floats[i] = 0.0f; // (todo) diagonal = 1
            }
            else
            {
               memcpy((void*)floats, (const void*)o->elements, sizeof(floats));
            }
            break;

         default:
         {
            YAC_NEW_STACK(Matrix4f, t);
            t._setColumnMajor(_getColumnMajor());
            t.assignFloatArray(o);
            yacOperator(_cmd, &t, _r);
            //Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix4f::yacOperator Only Assign supported with FloatArray Type");
         }
         break;
      } // switch _cmd
   } // if FloatArray
   else if(YAC_VALID(_o))
   {
      // Generic right hand side object
      sF32 val;
      if(_o->yacScanF32(&val))
      {
         yacOperatorF32(_cmd, val, _r);
      }
      else
      {
         switch(_cmd)
         {
            case YAC_OP_ASSIGN:
               assignGeneric(NULL, _o);
               break;

            default:
               /////Dyac_throw_def(TkMathUnsupportedOperator, "tkmath::Matrix4f::yacOperator: Only Assign supported with Generic Object Type");
               YAC_Object::yacOperator(_cmd,_o,_r);
               break;
         }
      }
   }
   else
   {
      switch(_cmd)
      {
         case YAC_OP_NEG:
            for(sUI i = 0u; i < 16u; i++)
               floats[i] = -floats[i];
            break;

         case YAC_OP_INIT:
            break;

         default:
            YAC_Object::yacOperator(_cmd,_o,_r);
            break;
      }
   }
}

void _Matrix4f::yacOperatorI(sSI _cmd, sSI val, YAC_Value *_r) {
   yacOperatorF32(_cmd, (sF32)val, _r);
}

void _Matrix4f::yacOperatorF32(sSI _cmd, sF32 val, YAC_Value *_r) {
   switch (_cmd)
   {
      case YAC_OP_MUL:
         _mulf_YAC_RSELF(val);
         break;

      case YAC_OP_DIV:
         if(Dfltnonzero(val))
         {
            _mulf_YAC_RSELF(1.0f/val);
         }
         else
         {
            Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::yacOperatorF32: OP_DIV Cannot divide by zero");
         }
         break;

      default:
         char buf[256];
         Dyac_snprintf(buf, 256, "tkmath::Matrix4f::yacOperatorF32: Operator not supported %d",_cmd);
         Dyac_throw_def(TkMathUnsupportedOperator,buf);
         break;
   }
}

void _Matrix4f::assignFloats(const sF32 *_fv, const sUI _ne) {
   if(16u != _ne)
   {
      Dyac_throw_def(WriteArrayOutOfBounds,"tkmath::Matrix4f: Array not of size 16");
   }
   else
   {
      memcpy((void*)floats, (const void*)_fv, sizeof(floats));
   }
}

void _Matrix4f::assignFloatArray(YAC_FloatArray *_fa) {
   assignFloats(_fa->elements, _fa->num_elements);
}

void _Matrix4f::assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o) {
   sUI ne = _o->yacArrayGetNumElements();
   sUI nH = _o->yacArrayGetHeight();
   sUI nW = _o->yacArrayGetWidth();

   if(nH <= 1u)
   {
      if(16u == nW)
      { //exact number to fill the matrix - we con go on
         nH = 4;
         nW = 4;
      }
      else
      { //nah, throw an exception
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix4f: Array not of size 16");
      }
   }
   if(ne > 0u)
   {
      sF32 *fv = (sF32*) _o->yacArrayGetPointer();

      if((NULL != fv) && (YAC_TYPE_FLOAT == _o->yacArrayGetElementType()))
      {
         // Copy fast, assume same element order
         sUI y, x;

         if(b_column_major)
         {
            sUI ySkip;
            if(nH > 4)
            {
               ySkip = nH - 4;
               nH = 4;
            }
            else
            {
               ySkip = 0;
            }
            if(nW > 4)
            {
               nW = 4;
            }

            sUI k = 0;
            sUI l = 0;
            for(x = 0u; x < nW; x++)
            {
               for(y = 0u; y < nH; y++)
               {
                  floats[k++] = fv[l++];
               }
               l += ySkip;
               for(; y < 4u; y++)
               {
                  floats[k++] = (x==y) ? 1.0f : 0.0f;
               }
            }
            for(; x < 4u; x++)
            {
               for(y = 0u; y < 4u; y++)
               {
                  floats[k++] = (x == y) ? 1.0f : 0.0f;
               }
            }
         }
         else
         {
            // Row major
            sUI xSkip;
            if(nH > 4u)
            {
               nH = 4u;
            }
            if(nW > 4u)
            {
               xSkip = nW - 4u;
               nW = 4u;
            }
            else
            {
               xSkip = 0u;
            }

            sUI k = 0u;
            sUI l = 0u;
            for(y = 0u; y < nH; y++)
            {
               for(x = 0u; x < nW; x++)
               {
                  floats[k++] = fv[l++];
               }
               l += xSkip;
               for(; x < 4u; x++)
               {
                  floats[k++] = (x == y) ? 1.0f : 0.0f;
               }
            }
            for(; y < 4u; y++)
            {
               for(x = 0u; x < 4u; x++)
               {
                  floats[k++] = (x == y) ? 1.0f : 0.0f;
               }
            }
         }
      }
      else
      {
         // Copy slow, typecast elements to float, assume same element order
         sUI y, x;

         if(b_column_major)
         {
            sUI ySkip;
            if(nH > 4u)
            {
               ySkip = nH - 4u;
               nH = 4u;
            }
            else
            {
               ySkip = 0u;
            }

            if(nW > 4u)
               nW = 4u;

            sUI k = 0u;
            sUI l = 0u;
            for(x = 0u; x < nW; x++)
            {
               for(y = 0u; y < nH; y++)
               {
                  YAC_Value v;
                  _o->yacArrayGet(_ctx, l++, &v);
                  v.typecast(YAC_TYPE_FLOAT);
                  floats[k++] = v.value.float_val;
               }
               l += ySkip;
               for(; y < 4u; y++)
               {
                  floats[k++] = (x == y) ? 1.0f : 0.0f;
               }
            }
            for(; x < 4u; x++)
            {
               for(y = 0u; y < 4u; y++)
               {
                  floats[k++] = (x == y) ? 1.0f : 0.0f;
               }
            }
         }
         else
         {
            // Row major
            sUI xSkip;
            if(nH > 4u)
               nH = 4u;
            if(nW > 4u)
            {
               xSkip = nW - 4u;
               nW = 4u;
            }
            else
            {
               xSkip = 0u;
            }

            sUI k = 0u;
            sUI l = 0u;
            for(y = 0u; y < nH; y++)
            {
               for(x = 0u; x < nW; x++)
               {
                  YAC_Value v;
                  _o->yacArrayGet(_ctx, l++, &v);
                  v.typecast(YAC_TYPE_FLOAT);
                  floats[k++] = v.value.float_val;
               }
               l += xSkip;
               for(; x < 4u; x++)
               {
                  floats[k++] = (x == y) ? 1.0f : 0.0f;
               }
            }
            for(; y < 4u; y++)
            {
               for(x = 0u; x < 4u; x++)
               {
                  floats[k++] = (x==y) ? 1.0f : 0.0f;
               }
            }
         }
      }
   }
   else
   {
      // Other array is empty or no array. Silently load identity
      _initIdentity();
   }
}

void _Matrix4f::_set(sUI _row, sUI _col, sF32 _v) {
   if(_row < 4u && _col < 4u)
   {
      TM4F(_row, _col) = _v;
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix4f::setRC: col / row out of bounds! (row=%u, col=%u)\n", _row, _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

sF32 _Matrix4f::_get(sUI _row, sUI _col) {
   if(_row < 4u && _col < 4u)
   {
      return TM4F(_row, _col);
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix4f::getRC: col / Row out of bounds! (row=%u, col=%u)\n", _row, _col);
      Dyac_throw_def(ReadArrayOutOfBounds, buf);
      return 0.0f;
   }
}

void _Matrix4f::_setA(sF32 x) {
   TM4F(0,0) = x;
}

void _Matrix4f::_setB(sF32 y) {
   TM4F(0,1) = y;
}

void _Matrix4f::_setC(sF32 x) {
   TM4F(0,2) = x;
}

void _Matrix4f::_setD(sF32 y) {
   TM4F(0,3) = y;
}

void _Matrix4f::_setE(sF32 y) {
   TM4F(1,0) = y;
}

void _Matrix4f::_setF(sF32 y) {
   TM4F(1,1) = y;
}

void _Matrix4f::_setG(sF32 y) {
   TM4F(1,2) = y;
}

void _Matrix4f::_setH(sF32 y) {
   TM4F(1,3) = y;
}


void _Matrix4f::_setI(sF32 y) {
   TM4F(2,0) = y;
}

void _Matrix4f::_setJ(sF32 y) {
   TM4F(2,1) = y;
}

void _Matrix4f::_setK(sF32 y) {
   TM4F(2,2) = y;
}

void _Matrix4f::_setL(sF32 y) {
   TM4F(2,3) = y;
}

void _Matrix4f::_setM(sF32 y) {
   TM4F(3,0) = y;
}

void _Matrix4f::_setN(sF32 y) {
   TM4F(3,1) = y;
}

void _Matrix4f::_setO(sF32 y) {
   TM4F(3,2) = y;
}

void _Matrix4f::_setP(sF32 y) {
   TM4F(3,3) = y;
}

sF32 _Matrix4f::_getA(void) {
   return TM4F(0,0);
}

sF32 _Matrix4f::_getB(void) {
   return TM4F(0,1);
}

sF32 _Matrix4f::_getC(void) {
   return TM4F(0,2);
}

sF32 _Matrix4f::_getD(void) {
   return TM4F(0,3);
}

sF32 _Matrix4f::_getE(void) {
   return TM4F(1,0);
}

sF32 _Matrix4f::_getF(void) {
   return TM4F(1,1);
}

sF32 _Matrix4f::_getG(void) {
   return TM4F(1,2);
}

sF32 _Matrix4f::_getH(void) {
   return TM4F(1,3);
}

sF32 _Matrix4f::_getI(void) {
   return TM4F(2,0);
}

sF32 _Matrix4f::_getJ(void) {
   return TM4F(2,1);
}

sF32 _Matrix4f::_getK(void) {
   return TM4F(2,2);
}

sF32 _Matrix4f::_getL(void) {
   return TM4F(2,3);
}

sF32 _Matrix4f::_getM(void) {
   return TM4F(3,0);
}

sF32 _Matrix4f::_getN(void) {
   return TM4F(3,1);
}

sF32 _Matrix4f::_getO(void) {
   return TM4F(3,2);
}

sF32 _Matrix4f::_getP(void) {
   return TM4F(3,3);
}

sBool YAC_VCALL _Matrix4f::yacToString(YAC_String *s) const {
   //
   // Generate string representation
   //
   s->yacArrayAlloc(512,0,0,0);
   s->printf("((%g, %g, %g, %g), (%g, %g, %g, %g), (%g, %g, %g, %g), (%g, %g, %g, %g))",
             TM4F(0,0), TM4F(0,1), TM4F(0,2), TM4F(0,3),
             TM4F(1,0), TM4F(1,1), TM4F(1,2), TM4F(1,3),
             TM4F(2,0), TM4F(2,1), TM4F(2,2), TM4F(2,3),
             TM4F(3,0), TM4F(3,1), TM4F(3,2), TM4F(3,3)
             );
   s->fixLength();
   return YAC_TRUE;
}

void _Matrix4f::_getString(YAC_Value *_r) const {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   yacToString(s);
   YAC_RETS(s,1);
}

void _Matrix4f::_add_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      for(sUI i = 0u; i < 16u; i++)
         floats[i] += o->floats[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::add_SELF: arg is not a Matrix4f");
   }
}

void _Matrix4f::_add_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      _Matrix4f *r = YAC_NEW_POOLED(Matrix4f);
      r->_setColumnMajor(_getColumnMajor());
      _r->initObject(r, YAC_TRUE);
      for(sUI i = 0u; i < 16u; i++)
         r->floats[i] = floats[i] + o->floats[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::add_RVAL: arg is not a Matrix4f");
   }
}

void _Matrix4f::_add_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix4f) && YAC_CHK(_r, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      YAC_CAST_ARG(_Matrix4f, r, _r);
      for(sUI i = 0u; i < 16u; i++)
         r->floats[i] = floats[i] + o->floats[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::add_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_sub_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      for(sUI i = 0u; i < 16u; i++)
         floats[i] -= o->floats[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::sub_SELF: arg is not a Matrix4f");
   }
}

void _Matrix4f::_sub_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      _Matrix4f *r = YAC_NEW_POOLED(Matrix4f);
      r->_setColumnMajor(_getColumnMajor());
      _r->initObject(r, YAC_TRUE);
      for(sUI i = 0u; i < 16u; i++)
         r->floats[i] = floats[i] - o->floats[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::sub_RVAL: arg is not a Matrix4f");
   }
}

void _Matrix4f::_sub_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_o, clid_Matrix4f) && YAC_CHK(_r, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      YAC_CAST_ARG(_Matrix4f, r, _r);
      for(sUI i = 0u; i < 16u; i++)
         r->floats[i] = floats[i] - o->floats[i];
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::sub_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_mulf_YAC_RSELF(sF32 s) {
   for(sUI i = 0u; i < 16u; i++)
      floats[i] *= s;
}

void _Matrix4f::_mulf_YAC_RVAL(sF32 s, YAC_Value *_r) {
   _Matrix4f *r = YAC_NEW_POOLED(Matrix4f);
   r->_setColumnMajor(_getColumnMajor());
   _r->initObject(r, YAC_TRUE);
   for(sUI i = 0u; i < 16u; i++)
      r->floats[i] = floats[i] * s;
}

void _Matrix4f::_mulf_YAC_RARG(sF32 s, YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, r, _r);
      for(sUI i = 0u; i < 16u; i++)
         r->floats[i] = floats[i] * s;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mulf_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_mul_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      sF32 t[4][4];

      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            t[y][x] =
               TM4F(y,0) * OM4F(0,x) +
               TM4F(y,1) * OM4F(1,x) +
               TM4F(y,2) * OM4F(2,x) +
               TM4F(y,3) * OM4F(3,x) ;
         }
      }
      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            TM4F(y,x) = t[y][x];
         }
      }
      // // memcpy((void*)floats, (const void*)t, sizeof(floats));
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mul_RSELF: arg is not a Matrix4f!\n");
   }
}

void _Matrix4f::_mul_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      _Matrix4f *r = YAC_NEW_POOLED(Matrix4f);
      r->_setColumnMajor(_getColumnMajor());

      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            M4F(r,y,x) =
               TM4F(y,0) * OM4F(0,x) +
               TM4F(y,1) * OM4F(1,x) +
               TM4F(y,2) * OM4F(2,x) +
               TM4F(y,3) * OM4F(3,x) ;
         }
      }

      _r->initObject(r, 1);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mul_RVAL: arg is not a Matrix4f!\n");
   }
}

void _Matrix4f::_mul_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      YAC_CAST_ARG(_Matrix4f, r, _r);

      if((void*)o != (void*)this)
      {
         for(sUI y = 0u; y < 4u; y++)
         {
            for(sUI x = 0u; x < 4u; x++)
            {
               M4F(r,y,x) =
                  TM4F(y,0) * OM4F(0,x) +
                  TM4F(y,1) * OM4F(1,x) +
                  TM4F(y,2) * OM4F(2,x) +
                  TM4F(y,3) * OM4F(3,x) ;
            }
         }
      }
      else
      {
         sF32 t[4][4];
         for(sUI y = 0u; y < 4u; y++)
         {
            for(sUI x = 0u; x < 4u; x++)
            {
               t[y][x] =
                  TM4F(y,0) * OM4F(0,x) +
                  TM4F(y,1) * OM4F(1,x) +
                  TM4F(y,2) * OM4F(2,x) +
                  TM4F(y,3) * OM4F(3,x) ;
            }
         }
         for(sUI y = 0u; y < 4u; y++)
         {
            for(sUI x = 0u; x < 4u; x++)
            {
               TM4F(y,x) = t[y][x];
            }
         }
         // // memcpy((void*)floats, (const void*)t, sizeof(floats));
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mul_RARG: return object is not a Matrix4f!\n");
   }
}

void _Matrix4f::_mulRev_YAC_RSELF(YAC_Object *_o) {
   if(YAC_CHK(_o, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      sF32 t[4][4];

      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            t[y][x] =
               OM4F(y,0) * TM4F(0,x) +
               OM4F(y,1) * TM4F(1,x) +
               OM4F(y,2) * TM4F(2,x) +
               OM4F(y,3) * TM4F(3,x) ;
         }
      }
      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            TM4F(y,x) = t[y][x];
         }
      }
      // // memcpy((void*)floats, (const void*)t, sizeof(floats));
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mulRev_SELF: arg is not a Matrix4f!\n");
   }
}

void _Matrix4f::_mulRev_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      _Matrix4f *r = YAC_NEW_POOLED(Matrix4f);
      r->_setColumnMajor(_getColumnMajor());
      _r->initObject(r, YAC_TRUE);

      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            M4F(r,y,x) =
               OM4F(y,0) * TM4F(0,x) +
               OM4F(y,1) * TM4F(1,x) +
               OM4F(y,2) * TM4F(2,x) +
               OM4F(y,3) * TM4F(3,x) ;
         }
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mul_RVAL: arg is not a Matrix4f!\n");
   }
}

void _Matrix4f::_mulRev_YAC_RARG(YAC_Object *_o, YAC_Object *_r) {
   if(YAC_CHK(_o, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, o, _o);
      YAC_CAST_ARG(_Matrix4f, r, _r);

      if((void*)o != (void*)this)
      {
         for(sUI y = 0u; y < 4u; y++)
         {
            for(sUI x = 0u; x < 4u; x++)
            {
               M4F(r,y,x) =
                  OM4F(y,0) * TM4F(0,x) +
                  OM4F(y,1) * TM4F(1,x) +
                  OM4F(y,2) * TM4F(2,x) +
                  OM4F(y,3) * TM4F(3,x) ;
            }
         }
      }
      else
      {
         sF32 t[4][4];
         for(sUI y = 0u; y < 4u; y++)
         {
            for(sUI x = 0u; x < 4u; x++)
            {
               t[y][x] =
                  OM4F(y,0) * TM4F(0,x) +
                  OM4F(y,1) * TM4F(1,x) +
                  OM4F(y,2) * TM4F(2,x) +
                  OM4F(y,3) * TM4F(3,x) ;
            }
         }
         for(sUI y = 0u; y < 4u; y++)
         {
            for(sUI x = 0u; x < 4u; x++)
            {
               TM4F(y,x) = t[y][x];
            }
         }
         // // memcpy((void*)floats, (const void*)t, sizeof(floats));
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mulRev_RARG: return object is not a Matrix4f\n");
   }
}

void _Matrix4f::mulv_helper(const _Vector4f *o, _Vector4f *r) const {
   if((void*)o != (void*)r)
   {
      for(sUI y = 0u; y < 4u; y++)
      {
         r->floats[y] =
            TM4F(y,0) * o->floats[0] +
            TM4F(y,1) * o->floats[1] +
            TM4F(y,2) * o->floats[2] +
            TM4F(y,3) * o->floats[3] ;
      }
   }
   else
   {
      sF32 t[4];
      for(sUI y = 0u; y < 4u; y++)
      {
         t[y] =
            TM4F(y,0) * o->floats[0] +
            TM4F(y,1) * o->floats[1] +
            TM4F(y,2) * o->floats[2] +
            TM4F(y,3) * o->floats[3] ;
      }

      r->floats[0] = t[0];
      r->floats[1] = t[1];
      r->floats[2] = t[2];
      r->floats[3] = t[3];
   }
}

void _Matrix4f::_mulv_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) const {
   //
   // Multiply matrix by (column-) vector and return new Vector4f
   //
   if(YAC_CHK(_o, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, o, _o);
      _Vector4f *r = YAC_NEW_POOLED(Vector4f);
      _r->initObject(r, YAC_TRUE);
      mulv_helper(o, r);
   }
   else if(YAC_VALID(_o))
   {
      YAC_NEW_STACK(Vector4f, v);
      v.assignGeneric(NULL/*context*/, _o);
      _Vector4f *r = YAC_NEW_POOLED(Vector4f);
      _r->initObject(r, YAC_TRUE);
      mulv_helper(&v, r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mulv: invalid arg object");
   }
}

void _Matrix4f::_mulv_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, r, _r);

      if(YAC_CHK(_o, clid_Vector4f))
      {
         YAC_CAST_ARG(_Vector4f, o, _o);
         mulv_helper(o, r);
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector4f, v);
         v.assignGeneric(NULL/*context*/, _o);
         mulv_helper(&v, r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mulv: invalid arg object");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mulv: return object is not a Vector4f");
   }
}

void _Matrix4f::mulvrev_helper(const _Vector4f *o, _Vector4f *r) const {
   if((void*)o != (void*)r)
   {
      for(sUI y = 0u; y < 4u; y++)
      {
         r->floats[y] =
            TM4F(0,y) * o->floats[0] +
            TM4F(1,y) * o->floats[1] +
            TM4F(2,y) * o->floats[2] +
            TM4F(3,y) * o->floats[3] ;
      }
   }
   else
   {
      sF32 t[4];
      for(sUI y = 0u; y < 4u; y++)
      {
         t[y] =
            TM4F(0,y) * o->floats[0] +
            TM4F(1,y) * o->floats[1] +
            TM4F(2,y) * o->floats[2] +
            TM4F(3,y) * o->floats[3] ;
      }

      r->floats[0] = t[0];
      r->floats[1] = t[1];
      r->floats[2] = t[2];
      r->floats[3] = t[3];
   }
}

void _Matrix4f::_mulvRev_YAC_RVAL(YAC_Object *_o, YAC_Value *_r) const {
   //
   // Multiply matrix by (column-) vector and return new Vector4f
   //
   if(YAC_CHK(_o, clid_Vector4f))
   {
      _Vector4f *o = (_Vector4f*)_o;
      _Vector4f *r = YAC_NEW_POOLED(Vector4f);
      _r->initObject(r, YAC_TRUE);
      mulvrev_helper(o, r);
   }
   else if(YAC_VALID(_o))
   {
      YAC_NEW_STACK(Vector4f, v);
      v.assignGeneric(NULL/*context*/, _o);
      _Vector4f *r = YAC_NEW_POOLED(Vector4f);
      _r->initObject(r, YAC_TRUE);
      mulvrev_helper(&v, r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mulvRev: invalid arg object");
   }
}

void _Matrix4f::_mulvRev_YAC_RARG(YAC_Object *_o, YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, r, _r);

      if(YAC_CHK(_o, clid_Vector4f))
      {
         YAC_CAST_ARG(_Vector4f, o, _o);
         mulvrev_helper(o, r);
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector4f, v);
         v.assignGeneric(NULL/*context*/, _o);
         mulvrev_helper(&v, r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mulvRev: invalid arg object");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::mulvRev: return object is not a Vector4f");
   }
}

sF32 _Matrix4f::_getAbs(void) const {
   //
   // Return "length" of matrix
   //
   return sqrtf(floats[ 0]*floats[ 0] + floats[ 1]*floats[ 1] + floats[ 2]*floats[ 2] + floats[ 3]*floats[ 3]+
                floats[ 4]*floats[ 4] + floats[ 5]*floats[ 5] + floats[ 6]*floats[ 6] + floats[ 7]*floats[ 7]+
                floats[ 8]*floats[ 8] + floats[ 9]*floats[ 9] + floats[10]*floats[10] + floats[11]*floats[11]+
                floats[12]*floats[12] + floats[13]*floats[13] + floats[14]*floats[14] + floats[15]*floats[15]
                );
}

sF32 _Matrix4f::_getAbsSqr(void) const {
   //
   // Return squared "length" of matrix
   //
   return (floats[ 0]*floats[ 0] + floats[ 1]*floats[ 1] + floats[ 2]*floats[ 2] + floats[ 3]*floats[ 3]+
           floats[ 4]*floats[ 4] + floats[ 5]*floats[ 5] + floats[ 6]*floats[ 6] + floats[ 7]*floats[ 7]+
           floats[ 8]*floats[ 8] + floats[ 9]*floats[ 9] + floats[10]*floats[10] + floats[11]*floats[11]+
           floats[12]*floats[12] + floats[13]*floats[13] + floats[14]*floats[14] + floats[15]*floats[15]
           );
}

void _Matrix4f::_unit_YAC_RSELF(void) {
   //
   // Scale matrix to "length" 1
   //
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      absval = 1.0f / absval;
      _mulf_YAC_RSELF(absval);
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::unit_SELF: Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix4f::_unit_YAC_RVAL(YAC_Value *_r) {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      _Matrix4f *r = YAC_NEW_POOLED(Matrix4f);
      r->_setColumnMajor(_getColumnMajor());
      _mulf_YAC_RARG(absval, r);
      _r->initObject(r, 1);
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::unit_RVAL: Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix4f::_unit_YAC_RARG(YAC_Object *_r) const {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      _mulf_YAC_RARG(absval, _r);
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::unit_RARG: Zero Matrix cannot be normalized!\n");
   }
}

void _Matrix4f::_unitScale_YAC_RSELF(sF32 s) {
   //
   // Scale matrix to "length" s
   //
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      _mulf_YAC_RSELF(s / absval);
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::unitScale_SELF: Zero Matrix cannot be normalized");
   }
}

void _Matrix4f::_unitScale_YAC_RVAL(sF32 s, YAC_Value *_r) {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      _Matrix4f *r = YAC_NEW_POOLED(Matrix4f);
      r->_setColumnMajor(_getColumnMajor());
      _mulf_YAC_RARG(s / absval, r);
      _r->initObject(r, 1);
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::unitScale_RVAL: Zero Matrix cannot be normalized");
   }
}

void _Matrix4f::_unitScale_YAC_RARG(sF32 s, YAC_Object *_r) const {
   sF32 absval = _getAbs();
   if(Dfltnonzero(absval))
   {
      _mulf_YAC_RARG(s / absval, _r);
   }
   else
   {
      Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::unitScale_RARG: Zero Matrix cannot be normalized");
   }
}

sF32 _Matrix4f::_det(void) const {
   //
   // Returns determinant of matrix
   //
   // Calculate determinant with leibniz' formula
   //
#define PDET(a,b,c,d) (TM4F(0,a) * TM4F(1,b) * TM4F(2,c) * TM4F(3, d))

   return (+PDET(0,1,2,3) - PDET(0,1,3,2) - PDET(0,2,1,3) + PDET(0,2,3,1) + PDET(0,3,1,2) - PDET(0,3,2,1)
           -PDET(1,0,2,3) + PDET(1,0,3,2) + PDET(1,2,0,3) - PDET(1,2,3,0) - PDET(1,3,0,2) + PDET(1,3,2,0)
           +PDET(2,0,1,3) - PDET(2,0,3,1) - PDET(2,1,0,3) + PDET(2,1,3,0) + PDET(2,3,0,1) - PDET(2,3,1,0)
           -PDET(3,0,1,2) + PDET(3,0,2,1) + PDET(3,1,0,2) - PDET(3,1,2,0) - PDET(3,2,0,1) + PDET(3,2,1,0)
           );
}


#ifdef INVERT_DET
void _Matrix4f::invertHelper(_Matrix4f *_t) const {
   //inverts matrix if possible, determinant version
#define DET3(_a,_b,_c,_d,_e,_f,_g,_h,_i) ((_a*_e*_i)+(_b*_f*_g)+(_c*_d*_h)-(_c*_e*_g)-(_a*_f*_h)-(_b*_d*_i))
#define A TM4F(0,0)
#define B TM4F(0,1)
#define C TM4F(0,2)
#define D TM4F(0,3)
#define E TM4F(1,0)
#define F TM4F(1,1)
#define G TM4F(1,2)
#define H TM4F(1,3)
#define I TM4F(2,0)
#define J TM4F(2,1)
#define K TM4F(2,2)
#define L TM4F(2,3)
#define M TM4F(3,0)
#define N TM4F(3,1)
#define O TM4F(3,2)
#define P TM4F(3,3)
   sF32 detval = _det();
   if(Dfltnonzero(detval))
   {
      detval = 1.0f / detval;

      M4F(_t,0,0) =  DET3(F,G,H,J,K,L,N,O,P) * detval;
      M4F(_t,0,1) = -DET3(B,C,D,J,K,L,N,O,P) * detval;
      M4F(_t,0,2) =  DET3(B,C,D,F,G,H,N,O,P) * detval;
      M4F(_t,0,3) = -DET3(B,C,D,F,G,H,J,K,L) * detval;

      M4F(_t,1,0) = -DET3(E,G,H,I,K,L,M,O,P) * detval;
      M4F(_t,1,1) =  DET3(A,C,D,I,K,L,M,O,P) * detval;
      M4F(_t,1,2) = -DET3(A,C,D,E,G,H,M,O,P) * detval;
      M4F(_t,1,3) =  DET3(A,C,D,E,G,H,I,K,L) * detval;

      M4F(_t,2,0) =  DET3(E,F,H,I,J,L,M,N,P) * detval;
      M4F(_t,2,1) = -DET3(A,B,D,I,J,L,M,N,P) * detval;
      M4F(_t,2,2) =  DET3(A,B,D,E,F,H,M,N,P) * detval;
      M4F(_t,2,3) = -DET3(A,B,D,E,F,H,I,J,L) * detval;

      M4F(_t,3,0) = -DET3(E,F,G,I,J,K,M,N,O) * detval;
      M4F(_t,3,1) =  DET3(A,B,C,I,J,K,M,N,O) * detval;
      M4F(_t,3,2) = -DET3(A,B,C,E,F,G,M,N,O) * detval;
      M4F(_t,3,3) =  DET3(A,B,C,E,F,G,I,J,K) * detval;
   }
   else
   {
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::invert_SELF: Invalid Matrix (det==0)");
      Dyac_host_printf("tkmath::Matrix4f::invertHelper: invalid Matrix (det==0)");
      _t->_initIdentity();
      return;
   }
#undef P
#undef O
#undef N
#undef M
#undef L
#undef K
#undef J
#undef I
#undef H
#undef G
#undef F
#undef E
#undef D
#undef C
#undef B
#undef A
#undef DET3
}

#else //INVERT_DET

//an alternative implementation of invert matrix
//this should theoretically be faster, as it uses less multiplications
//but in some compiler/os/architecture combinations the determinant version
//is faster
//
//maybe decide at compiletime?
void _Matrix4f::invertHelper(_Matrix4f *o) const {
   sF32 m0,m1,m2,m3,s;
   sF32 rd0[8],rd1[8],rd2[8],rd3[8],*r0=rd0, *r1=rd1, *r2=rd2, *r3=rd3, *tmp;

   r0[0] = TM4F(0,0);
   r0[1] = TM4F(0,1);
   r0[2] = TM4F(0,2);
   r0[3] = TM4F(0,3);
   r0[4] = 1.0f;
   r0[5] = 0.0f;
   r0[6] = 0.0f;
   r0[7] = 0.0f;

   r1[0] = TM4F(1,0);
   r1[1] = TM4F(1,1);
   r1[2] = TM4F(1,2);
   r1[3] = TM4F(1,3);
   r1[5] = 1.0f;
   r1[4] = 0.0f;
   r1[6] = 0.0f;
   r1[7] = 0.0f;

   r2[0] = TM4F(2,0);
   r2[1] = TM4F(2,1);
   r2[2] = TM4F(2,2);
   r2[3] = TM4F(2,3);
   r2[6] = 1.0f;
   r2[4] = 0.0f;
   r2[5] = 0.0f;
   r2[7] = 0.0f;

   r3[0] = TM4F(3,0);
   r3[1] = TM4F(3,1);
   r3[2] = TM4F(3,2);
   r3[3] = TM4F(3,3);
   r3[7] = 1.0f;
   r3[4] = 0.0f;
   r3[5] = 0.0f;
   r3[6] = 0.0f;

   /* Choose myPivot, or die. */
   if (fabsf(r3[0])>fabsf(r2[0])) {tmp = r3;r3 = r2;r2 = tmp;};
   if (fabsf(r2[0])>fabsf(r1[0])) {tmp = r2;r2 = r1;r1 = tmp;};
   if (fabsf(r1[0])>fabsf(r0[0])) {tmp = r1;r1 = r0;r0 = tmp;};
   if (Dfltzero(r0[0]))
   {
      //assert(!"could not invert matrix");
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::invert2: Invalid Matrix (det==0)");
      Dyac_host_printf("[!!!] tkmath::Matrix4f::invertHelper: invalid Matrix (det==0)");
      o->_initIdentity();
      return;
   }
   /* Eliminate first variable. */
   m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
   s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
   s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
   s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
   s = r0[4];
   if (Dfltnonzero(s)) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
   s = r0[5];
   if (Dfltnonzero(s)) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
   s = r0[6];
   if (Dfltnonzero(s)) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
   s = r0[7];
   if (Dfltnonzero(s)) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

   /* Choose myPivot, or die. */
   if (fabsf(r3[1])>fabsf(r2[1])) {tmp = r3;r3 = r2;r2 = tmp;};
   if (fabsf(r2[1])>fabsf(r1[1])) {tmp = r2;r2 = r1;r1 = tmp;};
   if (Dfltzero(r1[1]))
   {
      //assert(!"could not invert matrix");
      //Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::invert2: Invalid Matrix (det==0)");
      Dyac_host_printf("[!!!] tkmath::Matrix4f::invertHelper: invalid Matrix (det==0)");
      o->_initIdentity();
      return;
   }
   /* Eliminate second variable. */
   m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
   r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
   r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
   s = r1[4]; if (Dfltnonzero(s)) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
   s = r1[5]; if (Dfltnonzero(s)) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
   s = r1[6]; if (Dfltnonzero(s)) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
   s = r1[7]; if (Dfltnonzero(s)) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

   /* Choose myPivot, or die. */
   if (fabsf(r3[2])>fabsf(r2[2])) {tmp=r3;r3 = r2;r2 = tmp;};
   if (Dfltzero(r2[2]))
   {
      // assert(!"could not invert matrix");
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::invert2: Invalid Matrix (det==0)");
      Dyac_host_printf("[!!!] tkmath::Matrix4f::invertHelper: invalid Matrix (det==0)");
      o->_initIdentity();
      return;
   }
   /* Eliminate third variable. */
   m3 = r3[2]/r2[2];
   r3[3] -= m3 * r2[3];
   r3[4] -= m3 * r2[4];
   r3[5] -= m3 * r2[5];
   r3[6] -= m3 * r2[6];
   r3[7] -= m3 * r2[7];
   /* Last check. */
   if (Dfltzero(r3[3]))
   {
      //assert(!"could not invert matrix");
      // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::invert2: Invalid Matrix (det==0)");
      Dyac_host_printf("[!!!] tkmath::Matrix4f::invertHelper: invalid Matrix (det==0)");
      o->_initIdentity();
      return;
   }

   s = 1.0f/r3[3];              /* Now back substitute row 3. */
   r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

   m2 = r2[3];                 /* Now back substitute row 2. */
   s  = 1.0f/r2[2];
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
   s  = 1.0f/r1[1];
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
   s  = 1.0f/r0[0];
   r0[4] = s * (r0[4] - r1[4] * m0);
   r0[5] = s * (r0[5] - r1[5] * m0);
   r0[6] = s * (r0[6] - r1[6] * m0);
   r0[7] = s * (r0[7] - r1[7] * m0);

   OM4F(0,0) = r0[4];  OM4F(0,1) = r0[5];
   OM4F(0,2) = r0[6];  OM4F(0,3) = r0[7];
   OM4F(1,0) = r1[4];  OM4F(1,1) = r1[5];
   OM4F(1,2) = r1[6];  OM4F(1,3) = r1[7];
   OM4F(2,0) = r2[4];  OM4F(2,1) = r2[5];
   OM4F(2,2) = r2[6];  OM4F(2,3) = r2[7];
   OM4F(3,0) = r3[4];  OM4F(3,1) = r3[5];
   OM4F(3,2) = r3[6];  OM4F(3,3) = r3[7];

}


#endif // INVERT_DET

void _Matrix4f::_invert_YAC_RSELF(void) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());

   invertHelper(&t);

   // Copy result
   for(sUI k = 0u; k < 16u; k++)
      floats[k] = t.floats[k];
}

void _Matrix4f::_invert_YAC_RVAL(YAC_Value *_r) {
   _Matrix4f *r = YAC_NEW_POOLED(Matrix4f);
   r->_setColumnMajor(_getColumnMajor());
   invertHelper(r);
   _r->initObject(r, 1);
}

void _Matrix4f::_invert_YAC_RARG(YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      _Matrix4f *r = (_Matrix4f*)_r;
      sF32 detval = _det();
      if(Dfltnonzero(detval))
      {
         invertHelper(r);
      }
      else
      {
         // Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::invert_RARG: Invalid Matrix (det==0)");
         Dyac_host_printf("[!!!] tkmath::Matrix4f::invert: invalid Matrix (det==0)");
         r->_initIdentity();
         return;
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::invert_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_transpose_YAC_RSELF(void) {
   sF32 a = floats[ 1];
   sF32 b = floats[ 2];
   sF32 c = floats[ 3];
   sF32 d = floats[ 6];
   sF32 e = floats[ 7];
   sF32 f = floats[11];
   floats[ 1] = floats[ 4];
   floats[ 2] = floats[ 8];
   floats[ 3] = floats[12];
   floats[ 6] = floats[ 9];
   floats[ 7] = floats[13];
   floats[11] = floats[14];
   floats[ 4] = a;
   floats[ 8] = b;
   floats[12] = c;
   floats[ 9] = d;
   floats[13] = e;
   floats[14] = f;
}

void _Matrix4f::_transpose_YAC_RVAL(YAC_Value *_r) {
   _Matrix4f *r = YAC_NEW_POOLED(Matrix4f);
   r->_setColumnMajor(_getColumnMajor());
   r->floats[ 0] = floats[ 0]; r->floats[ 1] = floats[4]; r->floats[ 2] = floats[ 8]; r->floats[ 3] = floats[12];
   r->floats[ 4] = floats[ 1]; r->floats[ 5] = floats[5]; r->floats[ 6] = floats[ 9]; r->floats[ 7] = floats[13];
   r->floats[ 8] = floats[ 2]; r->floats[ 9] = floats[6]; r->floats[10] = floats[10]; r->floats[11] = floats[14];
   r->floats[12] = floats[ 3]; r->floats[13] = floats[7]; r->floats[14] = floats[11]; r->floats[15] = floats[15];

   _r->initObject(r,1);
}

void _Matrix4f::_transpose_YAC_RARG(YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      _Matrix4f *r = (_Matrix4f*)_r;
      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            M4F(r,x,y) = TM4F(y,x);
         }
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::transpose_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_setRow(sUI _row, YAC_Object *_v) {
   _initRowv(_row, _v);
}

void _Matrix4f::_getRow_YAC_RVAL(sUI _row, YAC_Value *_r) {
   if(_row < 4u)
   {
      _Vector4f *r = YAC_NEW_POOLED(Vector4f);
      _r->initObject(r, YAC_TRUE);
      r->_init(TM4F(_row,0), TM4F(_row,1), TM4F(_row,2), TM4F(_row,3));
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix4f::getRow_RVAL: invalid row index (%u > 3)\n", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix4f::_getRow_YAC_RARG(sUI _row, YAC_Object *_r) const {
   if(_row < 4u)
   {
      if(YAC_CHK(_r, clid_Vector4f))
      {
         _Vector4f *r = (_Vector4f*)_r;
         r->_init(TM4F(_row,0), TM4F(_row,1), TM4F(_row,2), TM4F(_row,3));
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::getRow_RARG: return object is not a Vector4f");
      }
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix4f::getRow_RARG: invalid row index (%u > 3)\n", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix4f::_setCol(sUI _col, YAC_Object *_o) {
   _initColv(_col, _o);
}

void _Matrix4f::_getCol_YAC_RVAL(sUI _col, YAC_Value *_r) {
   if(_col < 4u)
   {
      _Vector4f *r = YAC_NEW_POOLED(Vector4f);
      r->_init(TM4F(0,_col), TM4F(1,_col), TM4F(2,_col), TM4F(3,_col));
      _r->initObject(r, 1);
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix4f::getCol_RVAL: invalid column index (%u > 3)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix4f::_getCol_YAC_RARG(sUI _col, YAC_Object *_r) const {
   if(_col < 4u)
   {
      if(YAC_CHK(_r, clid_Vector4f))
      {
         _Vector4f *r = (_Vector4f*)_r;
         r->_init(TM4F(0,_col), TM4F(1,_col), TM4F(2,_col), TM4F(3,_col));
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::getCol_RARG: return object is not a Vector4f");
      }
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix4f::getCol_RARG: invalid column index (%u > 3)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix4f::_getQuaternionf_YAC_RVAL(YAC_Value *_r) {
   _Quaternionf *q = YAC_NEW_POOLED(Quaternionf);
   _r->initObject(q, YAC_TRUE);
   _getQuaternionf_YAC_RARG(q);
}

void _Matrix4f::_getQuaternionf_YAC_RARG(YAC_Object *_r) const {
   if(YAC_CHK(_r, clid_Quaternionf))
   {
      YAC_CAST_ARG(_Quaternionf, q, _r);
      sF32 s0, s1, s2;
      sSI k0, k1, k2, k3;
      if((TM4F(0,0) + TM4F(1,1) + TM4F(2,2)) > 0.0f)
      {
         k0 = 0;
         k1 = 3;
         k2 = 2;
         k3 = 1;
         s0 = 1.0f;
         s1 = 1.0f;
         s2 = 1.0f;
      }
      else if((TM4F(0,0) > TM4F(1,1)) && (TM4F(0,0) > TM4F(2,2)))
      {
         k0 = 1;
         k1 = 2;
         k2 = 3;
         k3 = 0;
         s0 =  1.0f;
         s1 = -1.0f;
         s2 = -1.0f;
      }
      else if(TM4F(1,1) > TM4F(2,2))
      {
         k0 = 2;
         k1 = 1;
         k2 = 0;
         k3 = 3;
         s0 = -1.0f;
         s1 =  1.0f;
         s2 = -1.0f;
      }
      else
      {
         k0 = 3;
         k1 = 0;
         k2 = 1;
         k3 = 2;
         s0 = -1.0f;
         s1 = -1.0f;
         s2 =  1.0f;
      }
      sF32 t = s0 * TM4F(0,0) + s1 * TM4F(1,1) + s2 * TM4F(2,2) + 1.0f;
      sF32 s = 0.5f / sqrtf(t);
      //sF32 s = ReciprocalSqrt( t ) * 0.5f;
      q->floats[k0] = s * t;
      q->floats[k1] = ( TM4F(0,1) - s2 * TM4F(1,0) ) * s;
      q->floats[k2] = ( TM4F(2,0) - s1 * TM4F(0,2) ) * s;
      q->floats[k3] = ( TM4F(1,2) - s0 * TM4F(2,1) ) * s;
      //translational part of matrix - we omit it
      //q[4] = m[0 * 4 + 3];
      //q[5] = m[1 * 4 + 3];
      //q[6] = m[2 * 4 + 3];
      //q[7] = 0.0f;

/*      sF32 a,b,c,d,e,f,g,h,i;
      a=(floats[1]+floats[4])/4.0f;
      b=(floats[2]+floats[8])/4.0f;
      c=(floats[6]+floats[9])/4.0f;
      d=(floats[4]-floats[1])/4.0f;
      e=(floats[2]-floats[8])/4.0f;
      f=(floats[9]-floats[6])/4.0f;
      g=1.0f-floats[0];
      h=1.0f-floats[5];
      i=1.0f-floats[10];
      if (Dfltnonzero(g)&&Dfltnonzero(h)&&Dfltnonzero(i)) {
         q->floats[0]=sqrtf((f*f+e*e)/i);
	 q->floats[1]=sqrtf((a*a+b*b)/g);
	 q->floats[2]=sqrtf((a*a+c*c)/h);
	 q->floats[3]=sqrtf((b*b+c*c)/i);
      } else {
         Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::Quaternionf Cannot determine Quaternion representation");
      }*/
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::getQuaternionf_RARG: return object is not a Quaternionf");
   }
}

void _Matrix4f::_initIdentity(void) {
   TM4F(0,0) = 1.0f;  TM4F(0,1) = 0.0f;  TM4F(0,2) = 0.0f;  TM4F(0,3) = 0.0f;
   TM4F(1,0) = 0.0f;  TM4F(1,1) = 1.0f;  TM4F(1,2) = 0.0f;  TM4F(1,3) = 0.0f;
   TM4F(2,0) = 0.0f;  TM4F(2,1) = 0.0f;  TM4F(2,2) = 1.0f;  TM4F(2,3) = 0.0f;
   TM4F(3,0) = 0.0f;  TM4F(3,1) = 0.0f;  TM4F(3,2) = 0.0f;  TM4F(3,3) = 1.0f;
}

sBool _Matrix4f::_isIdentity(void) {
   sBool bIdentity = YAC_TRUE;
   for(sUI rowIdx = 0u; rowIdx < 4u; rowIdx++)
   {
      for(sUI colIdx = 0u; colIdx < 4u; colIdx++)
      {
         // yac_host->printf("xxx isIdentity: TM4F(rowIdx=%u,colIdx=%u)=%f\n", rowIdx, colIdx, TM4F(rowIdx,colIdx));
         bIdentity = bIdentity && Dfltequal(TM4F(rowIdx,colIdx), (colIdx == rowIdx) ? 1.0f : 0.0f);
      }
   }
   return bIdentity;
}

void _Matrix4f::_init(sF32 a, sF32 b, sF32 c, sF32 d,
                      sF32 e, sF32 f, sF32 g, sF32 h,
                      sF32 i, sF32 j, sF32 k, sF32 l,
                      sF32 m, sF32 n, sF32 o, sF32 p
                      ) {
   // a b c d  (row-major)
   // e f g h
   // i j k l
   // m n o p

   floats[ 0] = a;
   floats[ 1] = b;
   floats[ 2] = c;
   floats[ 3] = d;

   floats[ 4] = e;
   floats[ 5] = f;
   floats[ 6] = g;
   floats[ 7] = h;

   floats[ 8] = i;
   floats[ 9] = j;
   floats[10] = k;
   floats[11] = l;

   floats[12] = m;
   floats[13] = n;
   floats[14] = o;
   floats[15] = p;
}

void _Matrix4f::_initTranspose(sF32 a, sF32 b, sF32 c, sF32 d,
                               sF32 e, sF32 f, sF32 g, sF32 h,
                               sF32 i, sF32 j, sF32 k, sF32 l,
                               sF32 m, sF32 n, sF32 o, sF32 p
                               ) {
   floats[ 0] = a;
   floats[ 1] = e;
   floats[ 2] = i;
   floats[ 3] = m;

   floats[ 4] = b;
   floats[ 5] = f;
   floats[ 6] = j;
   floats[ 7] = n;

   floats[ 8] = c;
   floats[ 9] = g;
   floats[10] = k;
   floats[11] = o;

   floats[12] = d;
   floats[13] = h;
   floats[14] = l;
   floats[15] = p;
}

void _Matrix4f::_initRowMajor(sF32 a, sF32 b, sF32 c, sF32 d,
                              sF32 e, sF32 f, sF32 g, sF32 h,
                              sF32 i, sF32 j, sF32 k, sF32 l,
                              sF32 m, sF32 n, sF32 o, sF32 p
                              ) {
   // a b c d   C-array / Direct3D order (default)
   // e f g h
   // i j k l
   // m n o p
   if(b_column_major)
   {
      _initTranspose(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
   }
   else
   {
      _init(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
   }
}

void _Matrix4f::_initColumnMajor(sF32 a, sF32 b, sF32 c, sF32 d,
                                 sF32 e, sF32 f, sF32 g, sF32 h,
                                 sF32 i, sF32 j, sF32 k, sF32 l,
                                 sF32 m, sF32 n, sF32 o, sF32 p) {
   // a e i m   OpenGL order
   // b f j n
   // c g k o
   // d h l p
   if(b_column_major)
   {
      _init(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
   }
   else
   {
      _initTranspose(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
   }
}

void _Matrix4f::_initRowf(sUI _row, sF32 _p, sF32 _q, sF32 _r, sF32 _s) {
   if(_row < 4u)
   {
      TM4F(_row, 0) = _p;
      TM4F(_row, 1) = _q;
      TM4F(_row, 2) = _r;
      TM4F(_row, 3) = _s;
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix4f::initRow: invaliud row index (%u > 3)", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix4f::_initRowv(sUI _row, YAC_Object *_o) {
   if(_row < 4u)
   {
      if(YAC_CHK(_o, clid_Vector4f))
      {
         YAC_CAST_ARG(_Vector4f, o, _o);
         TM4F(_row, 0) = o->floats[0];
         TM4F(_row, 1) = o->floats[1];
         TM4F(_row, 2) = o->floats[2];
         TM4F(_row, 3) = o->floats[3];
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector4f, t);
         t.assignGeneric(NULL, _o);
         TM4F(_row, 0) = t.floats[0];
         TM4F(_row, 1) = t.floats[1];
         TM4F(_row, 2) = t.floats[2];
         TM4F(_row, 3) = t.floats[3];
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix4f::initRowv: invalid arg object");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix4f::initRowv: invalid row index (%u > 3)\n", _row);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix4f::_initColf(sUI _col, sF32 _p, sF32 _q, sF32 _r, sF32 _s) {
   if(_col < 4u)
   {
      TM4F(0, _col) = _p;
      TM4F(1, _col) = _q;
      TM4F(2, _col) = _r;
      TM4F(3, _col) = _s;
   }
   else
   {
      char buf[128];
      Dyac_snprintf(buf, 128, "tkmath::Matrix4f::initColf: invalid column index (%u > 3)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix4f::_initColv(sUI _col, YAC_Object *_o) {
   if(_col < 4u)
   {
      if(YAC_CHK(_o, clid_Vector4f))
      {
         YAC_CAST_ARG(_Vector4f, o, _o);
         TM4F(0, _col) = o->floats[0];
         TM4F(1, _col) = o->floats[1];
         TM4F(2, _col) = o->floats[2];
         TM4F(3, _col) = o->floats[3];
      }
      else if(YAC_VALID(_o))
      {
         YAC_NEW_STACK(Vector4f, t);
         t.assignGeneric(NULL,_o);
         TM4F(0, _col) = t.floats[0];
         TM4F(1, _col) = t.floats[1];
         TM4F(2, _col) = t.floats[2];
         TM4F(3, _col) = t.floats[3];
      }
      else
      {
         Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix4f::initColv: invalid arg object");
      }
   }
   else
   {
      char buf[80];
      Dyac_snprintf(buf, 80, "tkmath::Matrix4f::initColv: invalid column index (%u > 3)\n", _col);
      Dyac_throw_def(WriteArrayOutOfBounds, buf);
   }
}

void _Matrix4f::_initRotatef(sF32 _x, sF32 _y, sF32 _z) {
   _initEulerXYZ(_x, _y, _z);
}

void _Matrix4f::_initRotatev(YAC_Object *_v) {
   if(YAC_BCHK(_v, clid_Vector3f))
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
      Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix4f::initRotatev: invalid arg object");
   }
}

void _Matrix4f::rotate_helper(const sF32 _angle, const _Vector3f *_dir) {
   sF32 x = _dir->floats[0];
   sF32 y = _dir->floats[1];
   sF32 z = _dir->floats[2];

   sF32 s = sinf(_angle);
   sF32 c = cosf(_angle);

   TM4F(0, 0) = x*x*(1.0f-c)+c;
   TM4F(0, 1) = x*y*(1.0f-c)-z*s;
   TM4F(0, 2) = x*z*(1.0f-c)+y*s;
   TM4F(0, 3) = 0.0f;

   TM4F(1, 0) = y*x*(1.0f-c)+z*s;
   TM4F(1, 1) = y*y*(1.0f-c)+c;
   TM4F(1, 2) = y*z*(1.0f-c)-x*s;
   TM4F(1, 3) = 0.0f;

   TM4F(2, 0) = x*z*(1.0f-c)-y*s;
   TM4F(2, 1) = y*z*(1.0f-c)+x*s;
   TM4F(2, 2) = z*z*(1.0f-c)+c;
   TM4F(2, 3) = 0.0f;

   TM4F(3, 3) = 0.0f;
   TM4F(3, 3) = 0.0f;
   TM4F(3, 3) = 0.0f;
   TM4F(3, 3) = 1.0f;
}

void _Matrix4f::_initRotate(sF32 _angle, YAC_Object *_dir) {
   if(YAC_BCHK(_dir, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, dir, _dir);
      rotate_helper(_angle, dir);
   }
   else if(YAC_VALID(_dir))
   {
      YAC_NEW_STACK(Vector3f, dir);
      dir.assignGeneric(NULL/*context*/, _dir);
      rotate_helper(_angle, &dir);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::initRotate: invalid \'dir\' object");
   }
}

void _Matrix4f::_initEulerXYX(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c2;
   TM4F(0,1) = s1*s2;
   TM4F(0,2) = c1*s2;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = s2*s3;
   TM4F(1,1) = c1*c3-c2*s1*s3;
   TM4F(1,2) = -c3*s1-c1*c2*s3;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = -c3*s2;
   TM4F(2,1) = c2*c3*s1+c1*s3;
   TM4F(2,2) = c1*c2*c3-s1*s3;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initEulerXZX(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c2;
   TM4F(0,1) = -c1*s2;
   TM4F(0,2) = s1*s2;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = c3*s2;
   TM4F(1,1) = c1*c2*c3-s1*s3;
   TM4F(1,2) = -c2*c3*s1-c1*s3;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = s2*s3;
   TM4F(2,1) = c3*s1+c1*c2*s3;
   TM4F(2,2) = c1*c3-c2*s1*s3;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initEulerYXY(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c1*c3-c2*s1*s3;
   TM4F(0,1) = s2*s3;
   TM4F(0,2) = c3*s1+c1*c2*s3;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = s1*s2;
   TM4F(1,1) = c2;
   TM4F(1,2) = -c1*s2;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = -c2*c3*s1-c1*s3;
   TM4F(2,1) = c3*s2;
   TM4F(2,2) = c1*c2*c3-s1*s3;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initEulerYZY(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c1*c2*c3-s1*s3;
   TM4F(0,1) = -c3*s2;
   TM4F(0,2) = c2*c3*s1+c1*s3;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = c1*s2;
   TM4F(1,1) = c2;
   TM4F(1,2) = s1*s2;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = -c3*s1-c1*c2*s3;
   TM4F(2,1) = s2*s3;
   TM4F(2,2) = c1*c3-c2*s1*s3;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initEulerZXZ(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c1*c3-c2*s1*s3;
   TM4F(0,1) = -c3*s1-c1*c2*s3;
   TM4F(0,2) = s2*s3;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = c2*c3*s1+c1*s3;
   TM4F(1,1) = c1*c2*c3-s1*s3;
   TM4F(1,2) = -c3*s2;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = s1*s2;
   TM4F(2,1) = c1*s2;
   TM4F(2,2) = c2;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initEulerZYZ(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c1*c2*c3-s1*s3;
   TM4F(0,1) = -c2*c3*s1-c1*s3;
   TM4F(0,2) = c3*s2;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = c3*s1+c1*c2*s3;
   TM4F(1,1) = c1*c3-c2*s1*s3;
   TM4F(1,2) = s2*s3;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = -c1*s2;
   TM4F(2,1) = s1*s2;
   TM4F(2,2) = c2;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initEulerXYZ(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c2*c3;
   TM4F(0,1) = c3*s1*s2-c1*s3;
   TM4F(0,2) = c1*c3*s2+s1*s3;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = c2*s3;
   TM4F(1,1) = c1*c3+s1*s2*s3;
   TM4F(1,2) = c1*s2*s3-c3*s1;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = -s2;
   TM4F(2,1) = c2*s1;
   TM4F(2,2) = c1*c2;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initEulerXZY(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c2*c3;
   TM4F(0,1) = s1*s3-c1*c3*s2;
   TM4F(0,2) = c3*s1*s2+c1*s3;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = s2;
   TM4F(1,1) = c1*c2;
   TM4F(1,2) = -c2*s1;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = -c2*s3;
   TM4F(2,1) = c3*s1+c1*s2*s3;
   TM4F(2,2) = c1*c3-s1*s2*s3;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initEulerYXZ(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c1*c3-s1*s2*s3;
   TM4F(0,1) = -c2*s3;
   TM4F(0,2) = c3*s1+c1*s2*s3;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = c3*s1*s2+c1*s3;
   TM4F(1,1) = c2*c3;
   TM4F(1,2) = s1*s3-c1*c3*s2;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = -c2*s1;
   TM4F(2,1) = s2;
   TM4F(2,2) = c1*c2;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initEulerYZX(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c1*c2;
   TM4F(0,1) = -s2;
   TM4F(0,2) = c2*s1;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = c1*c3*s2+s1*s3;
   TM4F(1,1) = c2*c3;
   TM4F(1,2) = c3*s1*s2-c1*s3;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = c1*s2*s3-c3*s1;
   TM4F(2,1) = c2*s3;
   TM4F(2,2) = c1*c3+s1*s2*s3;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initEulerZXY(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c1*c3+s1*s2*s3;
   TM4F(0,1) = c1*s2*s3-c3*s1;
   TM4F(0,2) = c2*s3;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = c2*s1;
   TM4F(1,1) = c1*c2;
   TM4F(1,2) = -s2;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = c3*s1*s2-c1*s3;
   TM4F(2,1) = c1*c3*s2+s1*s3;
   TM4F(2,2) = c2*c3;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initEulerZYX(sF32 a, sF32 b, sF32 c) {
   sF32 s1 = sinf(a), c1 = cosf(a);
   sF32 s2 = sinf(b), c2 = cosf(b);
   sF32 s3 = sinf(c), c3 = cosf(c);

   TM4F(0,0) = c1*c2;
   TM4F(0,1) = -c2*s1;
   TM4F(0,2) = s2;
   TM4F(0,3) = 0.0f;

   TM4F(1,0) = c3*s1+c1*s2*s3;
   TM4F(1,1) = c1*c3-s1*s2*s3;
   TM4F(1,2) = -c2*s3;
   TM4F(1,3) = 0.0f;

   TM4F(2,0) = s1*s3-c1*c3*s2;
   TM4F(2,1) = c3*s1*s2+c1*s3;
   TM4F(2,2) = c2*c3;
   TM4F(2,3) = 0.0f;

   TM4F(3,0) = 0.0f;
   TM4F(3,1) = 0.0f;
   TM4F(3,2) = 0.0f;
   TM4F(3,3) = 1.0f;
}

void _Matrix4f::_initOrtho(sF32 _left, sF32 _right,
                           sF32 _bottom, sF32 _top,
                           sF32 _znear, sF32 _zfar
                           ) {
   /*
    *  2/(r-l)   0        0         -(r+l)/(r-l)
    *  0         2/(t-b)  0         -(t+b)/(t-b)
    *  0         0        -2/(f-n)  -(f+n)/(f-n)
    *  0         0        0         1
    *
    */
   float rml = _right - _left;
   float tmb = _top   - _bottom;
   float fmn = _zfar  - _znear;

   if(0.0f != rml && tmb != 0.0f && fmn != 0.0f)
   {
      TM4F(0, 0) = 2.0f / rml;
      TM4F(0, 1) = 0.0f;
      TM4F(0, 2) = 0.0f;
      TM4F(0, 3) = -(_right + _left) / rml;

      TM4F(1, 0) = 0.0f;
      TM4F(1, 1) = 2.0f / tmb;
      TM4F(1, 2) = 0.0f;
      TM4F(1, 3) = -(_top + _bottom) / tmb;

      TM4F(2, 0) = 0.0f;
      TM4F(2, 1) = 0.0f;
      TM4F(2, 2) = -2.0f / fmn;
      TM4F(2, 3) = -(_zfar + _znear) / fmn;

      TM4F(3, 0) = 0.0f;
      TM4F(3, 1) = 0.0f;
      TM4F(3, 2) = 0.0f;
      TM4F(3, 3) = 1.0f;
   }
}

void _Matrix4f::_initFrustum(sF32 _left, sF32 _right,
                             sF32 _bottom, sF32 _top,
                             sF32 _znear, sF32 _zfar
                             ) {
   /*
    *  (a e i m)   (A 0  C 0)   (a*A, e*B, a*C+e*D+i*E-m, i*F)
    *  (b f j n) * (0 B  D 0) = (b*A, f*B, b*C+f*D+j*E-n, j*F)
    *  (c g k o)   (0 0  E F)   (c*A, g*B, c*C+g*D+k*E-o, k*F)
    *  (d h l p)   (0 0 -1 0)   (d*A, h*B, d*C+h*D+l*E-p, l*F)
    *
    *    (note) comment (a,b,c,d,..) differs from actual matrix element order (row major)
    *
    *  A = (2*near) / (right-left)
    *  B = (2*near) / (top-bottom)
    *
    *  C = (right+left) / (right-left)
    *  D = (top+bottom) / (top-bottom)
    *  E = - ( (far+near) / (far-near) )
    *
    *  F = - ( (2*far*near) / (far-near) )
    */

   // _bottom = -_bottom;
   // _top = -_top;

   sF32 near2 = (2.0f * _znear);

   sF32 A = near2 / (_right - _left);
   sF32 B = near2 / (_top - _bottom);

   sF32 C = (_right + _left) / (_right - _left);
   sF32 D = (_top + _bottom) / (_top - _bottom);
   sF32 E = -(_zfar + _znear) / (_zfar - _znear);

   sF32 F = -(2.0f * _zfar * _znear) / (_zfar - _znear);

   sF32 tm[16];

   // for(int i=0; i<16; i++)
   // {
   //    tm[i] = TM4F(0, i);
   // }
   tm[0+0] = 1.0f;  // column major  (todo) change to row major
   tm[0+1] = 0.0f;
   tm[0+2] = 0.0f;
   tm[0+3] = 0.0f;

   tm[4+0] = 0.0f;
   tm[4+1] = 1.0f;
   tm[4+2] = 0.0f;
   tm[4+3] = 0.0f;

   tm[8+0] = 0.0f;
   tm[8+1] = 0.0f;
   tm[8+2] = 1.0f;
   tm[8+3] = 0.0f;

   tm[12+0] = 0.0f;
   tm[12+1] = 0.0f;
   tm[12+2] = 0.0f;
   tm[12+3] = 1.0f;

   TM4F(0, 0) = tm[ 0] * A;
   TM4F(1, 0) = tm[ 1] * A;
   TM4F(2, 0) = tm[ 2] * A;
   TM4F(3, 0) = tm[ 3] * A;

   TM4F(0, 1) = tm[ 4] * B;
   TM4F(1, 1) = tm[ 5] * B;
   TM4F(2, 1) = tm[ 6] * B;
   TM4F(3, 1) = tm[ 7] * B;

   TM4F(0, 2) = (tm[ 0] * C) + (tm[ 4] * D) + (tm[ 8] * E) - tm[12];
   TM4F(1, 2) = (tm[ 1] * C) + (tm[ 5] * D) + (tm[ 9] * E) - tm[13];
   TM4F(2, 2) = (tm[ 2] * C) + (tm[ 6] * D) + (tm[10] * E) - tm[14];
   TM4F(3, 2) = (tm[ 3] * C) + (tm[ 7] * D) + (tm[11] * E) - tm[15];

   TM4F(0, 3) = (tm[ 8] * F);
   TM4F(1, 3) = (tm[ 9] * F);
   TM4F(2, 3) = (tm[10] * F);
   TM4F(3, 3) = (tm[11] * F);
}

void _Matrix4f::_initFrustumFov(sF32 _aspect, sF32 _fov, sF32 _znear, sF32 _zfar) {
	sF32 top = ::tanf(_fov * sF32(sM_PI / 360.0f)) * _znear;
	_initFrustum(_aspect*-top, _aspect*top, -top, top, _znear, _zfar);
}

void _Matrix4f::_initFrustumFovXY(sF32 _left, sF32 _right, sF32 _bottom, sF32 _top,
                                  sF32 _aspect, sF32 _fovX, sF32 _fovY,
                                  sF32 _znear, sF32 _zfar
                                  ) {
	sF32 h = ::tanf(_fovX * sF32(sM_PI / 360.0f)) * _znear * _aspect;
   sF32 v = ::tanf(_fovY * sF32(sM_PI / 360.0f)) * _znear;
   _initFrustum(_left*h, _right*h, _bottom*v, _top*v, _znear, _zfar);
}

void _Matrix4f::_BuildPerspectiveMatrix4f_YAC_RVAL(sF32 _fov, sF32 _aspect, sF32 _znear, sF32 _zfar, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initPerspective(_fov, _aspect, _znear, _zfar);
   // // _BuildPerspectiveMatrix4f_YAC_RARG(fov, aspect, zNear, zFar, m);
}

void _Matrix4f::_BuildPerspectiveMatrix4f_YAC_RARG(sF32 _fov, sF32 _aspect, sF32 _znear, sF32 _zfar, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initPerspective(_fov, _aspect, _znear, _zfar);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildPerspectiveMatrix4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_initPerspective(sF32 _fov, sF32 _aspect, sF32 _zNear, sF32 _zFar) {
   sF32 sine, cotangent, deltaZ;
   sF32 radians = _fov * ((sF32)sM_PI / 360.0f);
   sine = sinf(radians);
   /* Should be non-zero to avoid division by zero. */
   //assert(deltaZ);
   //assert(sine);
   //assert(aspectRatio);
   cotangent = cosf(radians) / sine;
   deltaZ = _zNear - _zFar;
   _initRowf(0, cotangent / _aspect, 0, 0, 0);
   _initRowf(1, 0, cotangent, 0, 0);
   _initRowf(2, 0, 0, (_zFar+_zNear)/deltaZ, 2*_zNear*_zFar/deltaZ);
   _initRowf(3, 0, 0, -1, 0);
}

void _Matrix4f::_BuildLookAtMatrix4f_YAC_RVAL(YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initLookAt(_eye, _center, _up);
}

void _Matrix4f::_BuildLookAtMatrix4f_YAC_RARG(YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Object *_m) const {
   if(YAC_CHK(_eye,    clid_Vector3f) &&
      YAC_CHK(_center, clid_Vector3f) &&
      YAC_CHK(_up,     clid_Vector3f)
      )
   {
      if(YAC_CHK(_m, clid_Matrix4f))
      {
         YAC_CAST_ARG(_Matrix4f, m, _m);
         m->_initLookAt(_eye, _center, _up);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildLookAtMatrix4f: return object is not a Matrix4f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildLookAtMatrix4f: eye/center/up is not a Vector3f");
   }
}

void _Matrix4f::_initLookAt(YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up) {
   if(YAC_CHK(_eye,    clid_Vector3f) &&
      YAC_CHK(_center, clid_Vector3f) &&
      YAC_CHK(_up,     clid_Vector3f)
      )
   {
      // (note) rewrite 20Jul2020
      // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
      YAC_CAST_ARG(_Vector3f, eye,    _eye);
      YAC_CAST_ARG(_Vector3f, center, _center);
      YAC_CAST_ARG(_Vector3f, up,     _up);

      // f = normalize(center - eye)
      YAC_NEW_STACK(Vector3f, f);
      center->_sub_YAC_RARG(eye, &f);
      f._unit_YAC_RSELF();

      // upn = normalize(up);
      YAC_NEW_STACK(Vector3f, upn);
      up->_unit_YAC_RARG(&upn);

      // s = f X upn
      YAC_NEW_STACK(Vector3f, s);
      f._cross_YAC_RARG(&upn, &s);

      // u = normalize(s) X f
      // YAC_NEW_STACK(Vector3f, sn);
      // s._unit_YAC_RARG(&sn);
      s._unit_YAC_RSELF();
      YAC_NEW_STACK(Vector3f, u);
      s._cross_YAC_RARG(&f, &u);

      // col1
      TM4F(0,0) = s.floats[0];
      TM4F(1,0) = u.floats[0];
      TM4F(2,0) = -f.floats[0];
      TM4F(3,0) = 0.0f;

      // col2
      TM4F(0,1) = s.floats[1];
      TM4F(1,1) = u.floats[1];
      TM4F(2,1) = -f.floats[1];
      TM4F(3,1) = 0.0f;

      // col3
      TM4F(0,2) = s.floats[2];
      TM4F(1,2) = u.floats[2];
      TM4F(2,2) = -f.floats[2];
      TM4F(3,2) = 0.0f;

      // col4
      //  (note) m * translatemat(-eye)  (<https://www.dcode.fr/matrix-multiplication>)
      TM4F(0,3) = (s.floats[0] * -eye->floats[0]) + (s.floats[1] * -eye->floats[1]) + (s.floats[2] * -eye->floats[2]);  // s.dot(-eye)
      TM4F(1,3) = (u.floats[0] * -eye->floats[0]) + (u.floats[1] * -eye->floats[1]) + (u.floats[2] * -eye->floats[2]);  // u.dot(-eye)
      TM4F(2,3) = (f.floats[0] *  eye->floats[0]) + (f.floats[1] *  eye->floats[1]) + (f.floats[2] *  eye->floats[2]);  // -f.dot(-eye)=f.dot(eye)
      TM4F(3,3) = 1.0f;
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::initLookAt: eye/center/up is not a Vector3f");
   }
}

void _Matrix4f::_BuildRotateMatrix4f_YAC_RVAL(sF32 angle, YAC_Object *_dir, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   _BuildRotateMatrix4f_YAC_RARG(angle,_dir,m);
}

void _Matrix4f::_BuildRotateMatrix4f_YAC_RARG(sF32 _radians, YAC_Object *_dir, YAC_Object *_m) const {
   if(YAC_CHK(_dir, clid_Vector3f))
   {
      if(YAC_CHK(_m, clid_Matrix4f))
      {
         YAC_CAST_ARG(_Matrix4f, m, _m);
         m->_initRotate(_radians, _dir);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildRotateMatrix4f: return object is not a Matrix4f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildRotateMatrix4f: 'dir' is not a Vector3f");
   }
}

// void _Matrix4f::_initRotate(sF32 _radians, YAC_Object *_dir) {
//    if(YAC_CHK(_dir, clid_Vector3f))
//    {
//       sF32 sine, cosine, ab, bc, ca, tx, ty, tz;
//       YAC_CAST_ARG(_Vector3f, dir, _dir);
//       YAC_NEW_STACK(Vector3f, axis);

//       dir->_unit_YAC_RARG(&axis);

//       sine = sinf(radians);
//       cosine = cosf(radians);
//       ab = axis.floats[0] * axis.floats[1] * (1 - cosine);
//       bc = axis.floats[1] * axis.floats[2] * (1 - cosine);
//       ca = axis.floats[2] * axis.floats[0] * (1 - cosine);
//       tx = axis.floats[0] * axis.floats[0];
//       ty = axis.floats[1] * axis.floats[1];
//       tz = axis.floats[2] * axis.floats[2];
//       TM4F(0,0) = tx + cosine * (1 - tx);
//       TM4F(0,1) = ab + axis.floats[2] * sine;
//       TM4F(0,2) = ca - axis.floats[1] * sine;
//       TM4F(0,3) = 0.0f;
//       TM4F(1,0) = ab - axis.floats[2] * sine;
//       TM4F(1,1) = ty + cosine * (1 - ty);
//       TM4F(1,2) = bc + axis.floats[0] * sine;
//       TM4F(1,3) = 0.0f;
//       TM4F(2,0) = ca + axis.floats[1] * sine;
//       TM4F(2,1) = bc - axis.floats[0] * sine;
//       TM4F(2,2) = tz + cosine * (1 - tz);
//       TM4F(2,3) = 0.0f;
//       TM4F(3,0) = 0.0f;
//       TM4F(3,1) = 0.0f;
//       TM4F(3,2) = 0.0f;
//       TM4F(3,3) = 1.0f;
//    }
//    else
//    {
//       Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::initRotate: 'dir' is not a Vector3f");
//    }
// }

void _Matrix4f::_BuildTranslateMatrix4f_YAC_RVAL(YAC_Object *_move, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   _BuildTranslateMatrix4f_YAC_RARG(_move, m);
}

void _Matrix4f::_BuildTranslateMatrix4f_YAC_RARG(YAC_Object *_move, YAC_Object *_m) const {
   if(YAC_CHK(_move, clid_Vector3f))
   {
      if(YAC_CHK(_m, clid_Matrix4f))
      {
         YAC_CAST_ARG(_Matrix4f, m, _m);
         YAC_CAST_ARG(_Vector3f, move, _move);
         m->_initRowMajor(1.0f, 0.0f, 0.0f, move->floats[0],
                          0.0f, 1.0f, 0.0f, move->floats[1],
                          0.0f, 0.0f, 1.0f, move->floats[2],
                          0.0f, 0.0f, 0.0f, 1.0f
                          );
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildTranslateMatrix4f: return object is not a Matrix4f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildTranslateMatrix4f: object is not a Vector3f");
   }
}

void _Matrix4f::_BuildScaleMatrix4f_YAC_RVAL(YAC_Object *_scale, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   _BuildScaleMatrix4f_YAC_RARG(_scale, m);
}

void _Matrix4f::_BuildScaleMatrix4f_YAC_RARG(YAC_Object *_scale, YAC_Object *_m) const {
   if(YAC_CHK(_scale, clid_Vector3f))
   {
      if(YAC_CHK(_m, clid_Matrix4f))
      {
         YAC_CAST_ARG(_Matrix4f, m, _m);
         YAC_CAST_ARG(_Vector3f, scale, _scale);
         m->_init(scale->floats[0], 0.0f,             0.0f,             0.0f,
                  0.0f,             scale->floats[1], 0.0f,             0.0f,
                  0.0f,             0.0f,             scale->floats[2], 0.0f,
                  0.0f,             0.0f,             0.0f,             1.0f
                  );
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildScaleMatrix4f: return object is not a Matrix4f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildScaleMatrix4f: 'scale' is not a Vector3f");
   }
}

void _Matrix4f::_initTranslatef(sF32 _x, sF32 _y, sF32 _z) {
   _initRowMajor(1, 0, 0, _x,
                 0, 1, 0, _y,
                 0, 0, 1, _z,
                 0, 0, 0,  1
                 );
}

void _Matrix4f::_initTranslatev(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      _initRowMajor(1, 0, 0, v->floats[0],
                    0, 1, 0, v->floats[1],
                    0, 0, 1, v->floats[2],
                    0, 0, 0, 1
                    );
   }
   else if(YAC_CHK(_v, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, v, _v);
      _initRowMajor(1, 0, 0, v->floats[0],
                    0, 1, 0, v->floats[1],
                    0, 0, 1, 0,
                    0, 0, 0, 1
                    );
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _v);
      _initRowMajor(1, 0, 0, v.floats[0],
                    0, 1, 0, v.floats[1],
                    0, 0, 1, v.floats[2],
                    0, 0, 0, 1
                    );
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::initTranslatev: invalid arg object");
   }
}

void _Matrix4f::_initScalef(sF32 _x, sF32 _y, sF32 _z) {
   _init(_x,    0.0f,  0.0f, 0.0f,
         0.0f, _y,     0.0f, 0.0f,
         0.0f,  0.0f, _z,    0.0f,
         0.0f,  0.0f,  0.0f, 1.0f
         );
}

void _Matrix4f::_initScalev(YAC_Object *_scale) {
   if(YAC_CHK(_scale, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, scale, _scale);
      _init(scale->floats[0], 0,                0,                0,
            0,                scale->floats[1], 0,                0,
            0,                0,                scale->floats[2], 0,
            0,                0,                0,                1
            );
   }
   else if(YAC_CHK(_scale, clid_Vector2f))
   {
      YAC_CAST_ARG(_Vector2f, scale, _scale);
      _init(scale->floats[0], 0,                0,  0,
            0,                scale->floats[1], 0,  0,
            0,                0,                1,  0,
            0,                0,                0,  1
            );
   }
   else if(YAC_CHK(_scale, clid_Vector4f))
   {
      YAC_CAST_ARG(_Vector4f, scale, _scale);
      _init(scale->floats[0], 0,                0,                0,
            0,                scale->floats[1], 0,                0,
            0,                0,                scale->floats[2], 0,
            0,                0,                0,                scale->floats[3]
            );
   }
   else if(YAC_VALID(_scale))
   {
      YAC_NEW_STACK(Vector4f, v);
      v.assignGeneric(NULL/*context*/, _scale);
      _init(v.floats[0], 0,           0,           0,
            0,           v.floats[1], 0,           0,
            0,           0,           v.floats[2], 0,
            0,           0,           0,           v.floats[3]
            );
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::initScalev: invalid arg object");
   }
}

void _Matrix4f::_initSkewXf(sF32 _a) {
   _initRowMajor(1.0f, tanf(_a), 0.0f, 0.0f,
                 0.0f, 1.0f,     0.0f, 0.0f,
                 0.0f, 0.0f,     1.0f, 0.0f,
                 0.0f, 0.0f,     0.0f, 1.0f
                 );
}

void _Matrix4f::_initSkewYf(sF32 _a) {
   _initRowMajor(1.0f,     0.0f, 0.0f, 0.0f,
                 tanf(_a), 1.0f, 0.0f, 0.0f,
                 0.0f,     0.0f, 1.0f, 0.0f,
                 0.0f,     0.0f, 0.0f, 1.0f
                 );
}

void _Matrix4f::_BuildEulerXZX4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerXZX(a, b, c);
}

void _Matrix4f::_BuildEulerXZX4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerXZX(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerXZX4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_BuildEulerXYX4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerXYX(a, b, c);
}

void _Matrix4f::_BuildEulerXYX4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerXYX(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerXYX4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_BuildEulerYXY4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerYXY(a, b, c);
}

void _Matrix4f::_BuildEulerYXY4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerYXY(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerYXY4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_BuildEulerYZY4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerYZY(a, b, c);
}

void _Matrix4f::_BuildEulerYZY4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerYZY(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerYZY4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_BuildEulerZYZ4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerZYZ(a, b, c);
}

void _Matrix4f::_BuildEulerZYZ4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerZYZ(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerZYZ4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_BuildEulerZXZ4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerZXZ(a, b, c);
}

void _Matrix4f::_BuildEulerZXZ4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerZXZ(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerZXZ4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_BuildEulerXZY4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerXZY(a, b, c);
}

void _Matrix4f::_BuildEulerXZY4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerXZY(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerXZY4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_BuildEulerXYZ4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerXYZ(a, b, c);
}

void _Matrix4f::_BuildEulerXYZ4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerXYZ(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerXYZ4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_BuildEulerYXZ4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerYXZ(a, b, c);
}

void _Matrix4f::_BuildEulerYXZ4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerYXZ(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerYXZ4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_BuildEulerYZX4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerYZX(a, b, c);
}

void _Matrix4f::_BuildEulerYZX4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerYZX(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerYZX4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_BuildEulerZYX4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerZYX(a, b, c);
}

void _Matrix4f::_BuildEulerZYX4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerZYX(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerZYX4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_BuildEulerZXY4f_YAC_RVAL(sF32 a, sF32 b, sF32 c, YAC_Value *_r) {
   _Matrix4f *m = YAC_NEW_POOLED(Matrix4f);
   _r->initObject(m, YAC_TRUE);
   m->_setColumnMajor(_getColumnMajor());
   m->_initEulerZXY(a, b, c);
}

void _Matrix4f::_BuildEulerZXY4f_YAC_RARG(sF32 a, sF32 b, sF32 c, YAC_Object *_m) const {
   if(YAC_CHK(_m, clid_Matrix4f))
   {
      YAC_CAST_ARG(_Matrix4f, m, _m);
      m->_initEulerZXY(a, b, c);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch,"tkmath::Matrix4f::BuildEulerZXY4f: return object is not a Matrix4f");
   }
}

void _Matrix4f::_translatef_YAC_RSELF(sF32 _x, sF32 _y, sF32 _z) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initTranslatef(_x, _y, _z);
   _mul_YAC_RSELF(&t);
}

void _Matrix4f::_translatef_YAC_RVAL(sF32 _x, sF32 _y, sF32 _z, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initTranslatef(_x, _y, _z);

   _mul_YAC_RVAL(&t, _r);
}

void _Matrix4f::_translatef_YAC_RARG(sF32 _x, sF32 _y, sF32 _z, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initTranslatef(_x, _y, _z);

      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::translatef_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_translatev_YAC_RSELF(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initTranslatef(v->floats[0], v->floats[1], v->floats[2]);
      _mul_YAC_RSELF(&t);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initTranslatef(v.floats[0], v.floats[1], v.floats[2]);
      _mul_YAC_RSELF(&t);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::translatev_RARG: 'v' is not a Vector3f");
   }
}

void _Matrix4f::_translatev_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initTranslatef(v->floats[0], v->floats[1], v->floats[2]);
      _mul_YAC_RVAL(&t, _r);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initTranslatef(v.floats[0], v.floats[1], v.floats[2]);
      _mul_YAC_RVAL(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::translatev_RVAL: 'v' is not a Vector3f");
   }
}

void _Matrix4f::_translatev_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      if(YAC_CHK(_v, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, v, _v);
         YAC_NEW_STACK(Matrix4f, t);
         t._setColumnMajor(_getColumnMajor());
         t._initTranslatef(v->floats[0], v->floats[1], v->floats[2]);
         _mul_YAC_RARG(&t, _r);
      }
      else if(YAC_VALID(_v))
      {
         YAC_NEW_STACK(Matrix4f, t);
         t._setColumnMajor(_getColumnMajor());
         YAC_NEW_STACK(Vector3f, v);
         v.assignGeneric(NULL/*context*/, _v);
         t._initTranslatef(v.floats[0], v.floats[1], v.floats[2]);
         _mul_YAC_RARG(&t, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::translatev_RARG: 'v' is not a Vector3f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::translatev_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_scalef_YAC_RSELF(sF32 _x, sF32 _y, sF32 _z) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initScalef(_x, _y, _z);
   _mul_YAC_RSELF(&t);
}

void _Matrix4f::_scalef_YAC_RVAL(sF32 _x, sF32 _y, sF32 _z, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initScalef(_x, _y, _z);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix4f::_scalef_YAC_RARG(sF32 _x, sF32 _y, sF32 _z, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(_x, _y, _z);
      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::scalef_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_scalev_YAC_RSELF(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(v->floats[0], v->floats[1], v->floats[2]);
      _mul_YAC_RSELF(&t);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initScalef(v.floats[0], v.floats[1], v.floats[2]);
      _mul_YAC_RSELF(&t);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::scalev_RARG: 'v' is not a Vector3f");
   }
}

void _Matrix4f::_scalev_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initScalef(v->floats[0], v->floats[1], v->floats[2]);
      _mul_YAC_RVAL(&t, _r);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initScalef(v.floats[0], v.floats[1], v.floats[2]);
      _mul_YAC_RVAL(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::scalev_RVAL: 'v' is not a Vector3f");
   }
}

void _Matrix4f::_scalev_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      if(YAC_CHK(_v, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, v, _v);
         YAC_NEW_STACK(Matrix4f, t);
         t._setColumnMajor(_getColumnMajor());
         t._initScalef(v->floats[0], v->floats[1], v->floats[2]);
         _mul_YAC_RARG(&t, _r);
      }
      else if(YAC_VALID(_v))
      {
         YAC_NEW_STACK(Matrix4f, t);
         t._setColumnMajor(_getColumnMajor());
         YAC_NEW_STACK(Vector3f, v);
         v.assignGeneric(NULL/*context*/, _v);
         t._initScalef(v.floats[0], v.floats[1], v.floats[2]);
         _mul_YAC_RARG(&t, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::scalev_RARG: 'v' is not a Vector3f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::scalev_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_rotatef_YAC_RSELF(sF32 _x, sF32 _y, sF32 _z) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initEulerXYZ(_x, _y, _z);
   _mul_YAC_RSELF(&t);
}

void _Matrix4f::_rotatef_YAC_RVAL(sF32 _x, sF32 _y, sF32 _z, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initEulerXYZ(_x, _y, _z);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix4f::_rotatef_YAC_RARG(sF32 _x, sF32 _y, sF32 _z, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initEulerXYZ(_x, _y, _z);
      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::rotatef_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_rotatev_YAC_RSELF(YAC_Object *_v) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initEulerXYZ(v->floats[0], v->floats[1], v->floats[2]);
      _mul_YAC_RSELF(&t);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initEulerXYZ(v.floats[0], v.floats[1], v.floats[2]);
      _mul_YAC_RSELF(&t);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::rotatev_RARG: 'v' is not a Vector3f");
   }
}

void _Matrix4f::_rotatev_YAC_RVAL(YAC_Object *_v, YAC_Value *_r) {
   if(YAC_CHK(_v, clid_Vector3f))
   {
      YAC_CAST_ARG(_Vector3f, v, _v);
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initEulerXYZ(v->floats[0], v->floats[1], v->floats[2]);
      _mul_YAC_RVAL(&t, _r);
   }
   else if(YAC_VALID(_v))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _v);
      t._initEulerXYZ(v.floats[0], v.floats[1], v.floats[2]);
      _mul_YAC_RVAL(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::rotatev_RVAL: 'v' is not a Vector3f");
   }
}

void _Matrix4f::_rotatev_YAC_RARG(YAC_Object *_v, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      if(YAC_CHK(_v, clid_Vector3f))
      {
         YAC_CAST_ARG(_Vector3f, v, _v);
         YAC_NEW_STACK(Matrix4f, t);
         t._setColumnMajor(_getColumnMajor());
         t._initEulerXYZ(v->floats[0], v->floats[1], v->floats[2]);
         _mul_YAC_RARG(&t, _r);
      }
      else if(YAC_VALID(_v))
      {
         YAC_NEW_STACK(Matrix4f, t);
         t._setColumnMajor(_getColumnMajor());
         YAC_NEW_STACK(Vector3f, v);
         v.assignGeneric(NULL/*context*/, _v);
         t._initEulerXYZ(v.floats[0], v.floats[1], v.floats[2]);
         _mul_YAC_RARG(&t, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::rotatev_RARG: 'v' is not a Vector3f");
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::rotatev_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_rotate_YAC_RSELF(sF32 _angle, YAC_Object *_dir) {
   if(YAC_BCHK(_dir, clid_Vector3f))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initRotate(_angle, _dir);
      _mul_YAC_RSELF(&t);
   }
   else if(YAC_VALID(_dir))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _dir);
      t._initRotate(_angle, &v);
      _mul_YAC_RSELF(&t);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "\'dir\' is not a Vector3f");
   }
}

void _Matrix4f::_rotate_YAC_RVAL(sF32 _angle, YAC_Object *_dir, YAC_Value *_r) {
   if(YAC_BCHK(_dir, clid_Vector3f))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initRotate(_angle, _dir);
      _mul_YAC_RVAL(&t, _r);
   }
   else if(YAC_VALID(_dir))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      YAC_NEW_STACK(Vector3f, v);
      v.assignGeneric(NULL/*context*/, _dir);
      t._initRotate(_angle, &v);
      _mul_YAC_RVAL(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "\'dir\' is not a Vector3f");
   }
}

void _Matrix4f::_rotate_YAC_RARG(sF32 _angle, YAC_Object *_dir, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      if(YAC_BCHK(_dir, clid_Vector3f))
      {
         YAC_NEW_STACK(Matrix4f, t);
         t._setColumnMajor(_getColumnMajor());
         t._initRotate(_angle, _dir);
         _mul_YAC_RARG(&t, _r);
      }
      else if(YAC_VALID(_dir))
      {
         YAC_NEW_STACK(Matrix4f, t);
         t._setColumnMajor(_getColumnMajor());
         YAC_NEW_STACK(Vector4f, v);
         v.assignGeneric(NULL/*context*/, _dir);
         t._initRotate(_angle, _dir);
         _mul_YAC_RARG(&t, _r);
      }
      else
      {
         Dyac_throw_def(NativeClassTypeMismatch, "\'dir\' arg is not a Vector3f");
      }
   }
   else
   {
         Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::rotate_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_ortho_YAC_RSELF(sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initOrtho(_left, _right, _bottom, _top, _znear, _zfar);
   _mul_YAC_RSELF(&t);
}

void _Matrix4f::_ortho_YAC_RVAL(sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initOrtho(_left, _right, _bottom, _top, _znear, _zfar);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix4f::_ortho_YAC_RARG(sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initOrtho(_left, _right, _bottom, _top, _znear, _zfar);
      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::ortho_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_frustum_YAC_RSELF(sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initFrustum(_left, _right, _bottom, _top, _znear, _zfar);
   _mul_YAC_RSELF(&t);
}

void _Matrix4f::_frustum_YAC_RVAL(sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initFrustum(_left, _right, _bottom, _top, _znear, _zfar);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix4f::_frustum_YAC_RARG(sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initFrustum(_left, _right, _bottom, _top, _znear, _zfar);
      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::frustum_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_frustumFovXY_YAC_RSELF(sF32 _left, sF32 _right, sF32 _bottom, sF32 _top,
                                        sF32 _aspect, sF32 _fovX, sF32 _fovY,
                                        sF32 _znear, sF32 _zfar
                                        ) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initFrustumFovXY(_left, _right, _bottom, _top, _aspect, _fovX, _fovY, _znear, _zfar);
   _mul_YAC_RSELF(&t);
}

void _Matrix4f::_frustumFovXY_YAC_RVAL(sF32 _left, sF32 _right, sF32 _bottom, sF32 _top,
                                       sF32 _aspect, sF32 _fovX, sF32 _fovY,
                                       sF32 _znear, sF32 _zfar,
                                       YAC_Value *_r
                                       ) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initFrustumFovXY(_left, _right, _bottom, _top, _aspect, _fovX, _fovY, _znear, _zfar);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix4f::_frustumFovXY_YAC_RARG(sF32 _left, sF32 _right, sF32 _bottom, sF32 _top,
                                       sF32 _aspect, sF32 _fovX, sF32 _fovY,
                                       sF32 _znear, sF32 _zfar,
                                       YAC_Object *_r
                                       ) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initFrustumFovXY(_left, _right, _bottom, _top, _aspect, _fovX, _fovY, _znear, _zfar);
   _mul_YAC_RARG(&t, _r);
}

void _Matrix4f::_perspective_YAC_RSELF(sF32 _fov, sF32 _aspect, sF32 _znear, sF32 _zfar) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initPerspective(_fov, _aspect, _znear, _zfar);
   _mul_YAC_RSELF(&t);
}

void _Matrix4f::_perspective_YAC_RVAL(sF32 _fov, sF32 _aspect, sF32 _znear, sF32 _zfar, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initPerspective(_fov, _aspect, _znear, _zfar);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix4f::_perspective_YAC_RARG(sF32 _fov, sF32 _aspect, sF32 _znear, sF32 _zfar, YAC_Object *_r) {
   if(YAC_CHK(_r, clid_Matrix4f))
   {
      YAC_NEW_STACK(Matrix4f, t);
      t._setColumnMajor(_getColumnMajor());
      t._initPerspective(_fov, _aspect, _znear, _zfar);
      _mul_YAC_RARG(&t, _r);
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "tkmath::Matrix4f::perspective_RARG: return object is not a Matrix4f");
   }
}

void _Matrix4f::_lookAt_YAC_RSELF(YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initLookAt(_eye, _center, _up);
   _mul_YAC_RSELF(&t);
}

void _Matrix4f::_lookAt_YAC_RVAL(YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Value *_r) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initLookAt(_eye, _center, _up);
   _mul_YAC_RVAL(&t, _r);
}

void _Matrix4f::_lookAt_YAC_RARG(YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Object *_r) {
   YAC_NEW_STACK(Matrix4f, t);
   t._setColumnMajor(_getColumnMajor());
   t._initLookAt(_eye, _center, _up);
   _mul_YAC_RARG(&t, _r);
}

void _Matrix4f::_extractPlanes(YAC_Object *_retN,
                               YAC_Object *_retF,
                               YAC_Object *_retL,
                               YAC_Object *_retR,
                               YAC_Object *_retT,
                               YAC_Object *_retB
                               ) const {
   if(YAC_CHK(_retN, clid_Vector4f))
   {
      if(YAC_CHK(_retF, clid_Vector4f))
      {
         if(YAC_CHK(_retL, clid_Vector4f))
         {
            if(YAC_CHK(_retR, clid_Vector4f))
            {
               if(YAC_CHK(_retT, clid_Vector4f))
               {
                  if(YAC_CHK(_retB, clid_Vector4f))
                  {
                     YAC_CAST_ARG(_Vector4f, retN, _retN);
                     YAC_CAST_ARG(_Vector4f, retF, _retF);
                     YAC_CAST_ARG(_Vector4f, retL, _retL);
                     YAC_CAST_ARG(_Vector4f, retR, _retR);
                     YAC_CAST_ARG(_Vector4f, retT, _retT);
                     YAC_CAST_ARG(_Vector4f, retB, _retB);

                     // Hartmann / Gribbs
// #define SWAP_CR

                     // near
                     for(sUI i = 0u; i < 4u; i++)
                     {
#ifdef SWAP_CR
                        retN->floats[i] = TM4F(i, 3) + TM4F(i, 2);
#else
                        retN->floats[i] = TM4F(3, i) + TM4F(2, i);
#endif
                     }
                     retN->_unit_YAC_RSELF();

                     // far
                     for(sUI i = 0u; i < 4u; i++)
                     {
#ifdef SWAP_CR
                        retF->floats[i] = TM4F(i, 3) - TM4F(i, 2);
#else
                        retF->floats[i] = TM4F(3, i) - TM4F(2, i);
#endif
                     }
                     retF->_unit_YAC_RSELF();

                     // left
                     for(sUI i = 0u; i < 4u; i++)
                     {
#ifdef SWAP_CR
                        retL->floats[i] = TM4F(i, 3) + TM4F(i, 0);
#else
                        retL->floats[i] = TM4F(3, i) + TM4F(0, i);
#endif
                     }
                     retL->_unit_YAC_RSELF();

                     // right
                     for(sUI i = 0u; i < 4u; i++)
                     {
#ifdef SWAP_CR
                        retR->floats[i] = TM4F(i, 3) - TM4F(i, 0);
#else
                        retR->floats[i] = TM4F(3, i) - TM4F(0, i);
#endif
                     }
                     retR->_unit_YAC_RSELF();

                     // top
                     for(sUI i = 0u; i < 4u; i++)
                     {
#ifdef SWAP_CR
                        retT->floats[i] = TM4F(i, 3) - TM4F(i, 1);
#else
                        retT->floats[i] = TM4F(3, i) - TM4F(1, i);
#endif
                     }
                     retT->_unit_YAC_RSELF();

                     // bottom
                     for(sUI i = 0u; i < 4u; i++)
                     {
#ifdef SWAP_CR
                        retB->floats[i] = TM4F(i, 3) + TM4F(i, 1);
#else
                        retB->floats[i] = TM4F(3, i) + TM4F(1, i);
#endif
                     }
                     retB->_unit_YAC_RSELF();

                  }
               }
            }
         }
      }
   }
}

void _Matrix4f::_New(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_VALID(_o))
   {
      _Matrix4f *r = YAC_NEW_POOLED(Matrix4f);
      _r->initObject(r, YAC_TRUE);
      r->_setColumnMajor(_getColumnMajor());
      r->assignGeneric(NULL, _o);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue,"tkmath::Matrix4f::New: invalid Object");
   }
}

void YAC_CALL _matrix4f(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_VALID(_o))
   {
      _Matrix4f *r = YAC_NEW_POOLED(Matrix4f);
      _r->initObject(r, YAC_TRUE);
      r->assignGeneric(NULL, _o);
   }
   else
   {
      Dyac_throw_def(TkMathInvalidValue,"tkmath::matrix4f: invalid Object");
   }
}
