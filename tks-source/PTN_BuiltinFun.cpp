/// PTN_BuiltinFun.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdlib.h>
#include <math.h>

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "YAC_Float.h"
#include "YAC_Integer.h"
#include "YAC_Long.h"
#include "PTN_ConstVal.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_BuiltinFun.h"


PTN_BuiltinFun::PTN_BuiltinFun(PTN_Expr *_expr,
                               sU8       _funselector
                               ) {
	fun_selector = _funselector;
	expr         = _expr;
	expr_opt     = expr->getEval();
}

PTN_BuiltinFun::~PTN_BuiltinFun() {
	if(expr) { delete expr; expr = NULL;	}
}

sUI PTN_BuiltinFun::getID(void) const {
   return PTN_BUILTINFUN;
}

sBool PTN_BuiltinFun::isConst(void) {
	switch(fun_selector)
	{
	default:
        return expr?expr->isConst():0;
	case TKS_BFUN_RND:
	case TKS_BFUN_TYPENAME:
	case TKS_BFUN_TCOBJECT:
		return 0;
	case TKS_BFUN_TCSTR:
	case TKS_BFUN_VALUE:
   case TKS_BFUN_ADDRESS:
		return expr->isConst()&&!tks_isobjectconstval(expr);
	}
}

sBool PTN_BuiltinFun::semanticCheck(void) {
    return expr?expr->semanticCheck():0;
}

sUI PTN_BuiltinFun::getType(void) const {
	switch(fun_selector)
	{
	default:
		return 0;

	case TKS_BFUN_TCINT:
	case TKS_BFUN_2N:
	case TKS_BFUN_TYPEID:
	case TKS_BFUN_SIGN:
		return YAC_TYPE_INT;

   case TKS_BFUN_ADDRESS:
#ifdef YAC_64
		return YAC_TYPE_OBJECT; // return address has to be wrapped in Long object
#else
		return YAC_TYPE_INT;
#endif

	case TKS_BFUN_SIN:
	case TKS_BFUN_COS:
	case TKS_BFUN_TAN:
   case TKS_BFUN_TANH:
	case TKS_BFUN_ASIN:
	case TKS_BFUN_ACOS:
	case TKS_BFUN_SQRT:
	case TKS_BFUN_RAD:
	case TKS_BFUN_DEG:
	case TKS_BFUN_TCFLOAT:
	case TKS_BFUN_ROUND:
	case TKS_BFUN_FLOOR:
	case TKS_BFUN_CEIL:
	case TKS_BFUN_FRAC:
	case TKS_BFUN_LOG:
   case TKS_BFUN_LOG2:
	case TKS_BFUN_EXP:
		return YAC_TYPE_FLOAT;

	case TKS_BFUN_VALUE:
	case TKS_BFUN_TCOBJECT:
	case TKS_BFUN_TCPOINTER:
		return YAC_TYPE_OBJECT;

	case TKS_BFUN_TCCHR:
	case TKS_BFUN_TCSTR:
	case TKS_BFUN_TYPENAME:
		return YAC_TYPE_STRING;

	case TKS_BFUN_ABS:
	case TKS_BFUN_RND:
		return expr->getType();
	}
}

static void PTN_BuiltinFun__eval(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
	const PTN_BuiltinFun*st = (const PTN_BuiltinFun*)_st;

   Dtracest;

   YAC_Value r;
   st->expr_opt(_env, &r, st->expr);
   if(Dhaveexception)
   {
      r.unsetFast();
      Dhandleexception(st->expr);
      return;
   }

	switch(r.type)
	{
      case YAC_TYPE_VOID:
         switch(st->fun_selector)
         {
            case TKS_BFUN_ADDRESS:
               ret->initInt(0);
               break;
            case TKS_BFUN_2N:
               ret->initInt(1);
               break;
            case TKS_BFUN_TCINT:
            case TKS_BFUN_TYPEID:
               ret->initInt(YAC_TYPE_VOID);
               break;
            case TKS_BFUN_TCOBJECT:
            case TKS_BFUN_TCPOINTER:
               ret->initObject(NULL, 0);
               break;
            case TKS_BFUN_TCSTR:
            case TKS_BFUN_TCCHR:
               ret->initEmptyString();
               break;
            case TKS_BFUN_SIN:
            case TKS_BFUN_COS:
            case TKS_BFUN_TAN:
            case TKS_BFUN_TANH:
            case TKS_BFUN_ASIN:
            case TKS_BFUN_ACOS:
            case TKS_BFUN_SQRT:
            case TKS_BFUN_RAD:
            case TKS_BFUN_DEG:
            case TKS_BFUN_TCFLOAT:
            case TKS_BFUN_ROUND:
            case TKS_BFUN_FLOOR:
            case TKS_BFUN_CEIL:
            case TKS_BFUN_SIGN:
            case TKS_BFUN_FRAC:
            case TKS_BFUN_LOG:
            case TKS_BFUN_LOG2:
               ret->initFloat(0.0f);
               break;
            case TKS_BFUN_EXP:
               ret->initFloat(1.0f);
               break;
            case TKS_BFUN_ABS:
            case TKS_BFUN_RND:
               ret->initInt(0);
               break;
            case TKS_BFUN_TYPENAME:
               ret->initEmptyString();
               ret->value.string_val->visit("void");
               break;
            case TKS_BFUN_VALUE:
            {
               // Return Value object with type void
               YAC_ValueObject *v = (YAC_ValueObject *)YAC_NEW_CORE_POOLED(YAC_CLID_VALUE);
               v->deleteme = 0;
               v->value.any = NULL;
               v->type = YAC_TYPE_VOID;
               ret->initObject(v, 1);
            }
            break;
         }
         break;

      case YAC_TYPE_STRING:
         switch(st->fun_selector)
         {
            case TKS_BFUN_ADDRESS:
#ifdef YAC_64
               {
                  YAC_Long *lo = (YAC_Long*) YAC_NEW_CORE_POOLED(YAC_CLID_LONG);
                  lo->value = (sS64) r.value.string_val;
                  ret->initObject(lo, 1);
               }
#else
               ret->initInt(r.value.int_val);
#endif
               r.unsetFast();
               break;
            case TKS_BFUN_TYPEID:
               ret->initInt(YAC_TYPE_STRING);
               r.unsetFast();
               break;
            case TKS_BFUN_TYPENAME:
               ret->initEmptyString();
               ret->value.string_val->visit("String");
               r.unsetFast();
               break;
            case TKS_BFUN_TCPOINTER:
               ret->copySafe(&r);
               ret->type = YAC_TYPE_OBJECT; // this will force strings to be passed "by reference"  xxx  ?
               break;
            case TKS_BFUN_TCOBJECT:
            case TKS_BFUN_TCSTR:
               // ---- it is sometimes *very* important to have 'deletable' strings (and other objects)
               // ---- so we can grab words from string stacks and store them without
               // ---- getting into any double-delete trouble..
               if(r.deleteme)
               {
                  ret->copySafe(&r);
               }
               else
               {
                  if(YAC_VALID(r.value.object_val))
                  {
                     YAC_Object *o = YAC_CLONE_POOLED(_env->context, r.value.object_val);
                     o->yacOperatorAssign(r.value.object_val);
                     ret->type = r.type;
                     ret->deleteme = 1;
                     ret->value.object_val = o;
                  }
                  else
                  {
                     ret->type = r.type;
                     ret->deleteme = 0;
                     ret->value.object_val = 0;
                  }
               }
               break;
            case TKS_BFUN_TCFLOAT:
            {
					sF32 f = 0.0f;
					if(YAC_VALID(r.value.object_val))
               {
						r.value.object_val->yacScanF32(&f);
               }
               r.unsetFast();
               ret->initFloat(f);
            }
            break;
            case TKS_BFUN_TCINT:
            {
               sSI i = 0;
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanI(&i);
               }
               r.unsetFast();
               ret->initInt(i);
            }
            break;
            case TKS_BFUN_VALUE:
            {
               YAC_ValueObject *v=(YAC_ValueObject *)YAC_NEW_CORE_POOLED(YAC_CLID_VALUE);
               if(YAC_VALID(r.value.object_val))
               {
                  // Returned object is neither null nor already deleted
                  v->initObject(r.value.object_val, r.deleteme);
                  if(r.value.object_val->class_ID == YAC_CLID_STRING)
                  {
                     v->type=YAC_TYPE_STRING;
                  }
               }
               else
               {
                  // Silently replace null or invalid object with null
                  v->initNull();
               }
               r.deleteme = 0;
               ret->initObject(v, 1);
            }
            break;
         }
         break;

      case YAC_TYPE_FLOAT:
         switch(st->fun_selector)
         {
            case TKS_BFUN_ADDRESS:
               ret->initInt(0);
               break;
            case TKS_BFUN_LOG:
               ret->initFloat((sF32)log(r.value.float_val));
               break;
            case TKS_BFUN_LOG2:
               ret->initFloat((sF32)log2(r.value.float_val));
               break;
            case TKS_BFUN_EXP:
               ret->initFloat((sF32)exp(r.value.float_val));
               break;
            case TKS_BFUN_TYPEID:
               ret->initInt(YAC_TYPE_FLOAT);
               break;
            case TKS_BFUN_TYPENAME:
               ret->initEmptyString();
               ret->value.string_val->visit("float");
               break;

            default:
            case 0:
               // ILLEGAL FUN SELECTOR
               break;
            case TKS_BFUN_SIN:
               ret->initFloat(sinf(r.value.float_val));
               break;
            case TKS_BFUN_COS:
               ret->initFloat(cosf(r.value.float_val));
               break;
            case TKS_BFUN_TAN:
               ret->initFloat(tanf(r.value.float_val));
               break;
            case TKS_BFUN_TANH:
               ret->initFloat(tanhf(r.value.float_val));
               break;
            case TKS_BFUN_ASIN:
               ret->initFloat(asinf(r.value.float_val));
               break;
            case TKS_BFUN_ACOS:
               ret->initFloat(acosf(r.value.float_val));
               break;
            case TKS_BFUN_SQRT:
               if(r.value.float_val > 0.0f)
               {
                  ret->initFloat(sqrtf(r.value.float_val));
               }
               else
               {
                  ret->initFloat(0.0f); // ILLEGAL SQRT(0)
               }
               break;
            case TKS_BFUN_RAD:
               ret->initFloat( (sF32)((r.value.float_val*2.0f*sM_PI)/360.0f) );
               break;
            case TKS_BFUN_DEG:
               ret->initFloat( (sF32)(r.value.float_val*360.0f/(2.0f*sM_PI)) );
               break;
            case TKS_BFUN_ABS:
               ret->initFloat( r.value.float_val<0.0f?-r.value.float_val:r.value.float_val );
               break;
            case TKS_BFUN_FRAC:
               ret->initFloat( r.value.float_val- ((sSI)r.value.float_val) );
               break;
            case TKS_BFUN_TCINT:
               ret->initInt( (sSI) r.value.float_val );
               break;
            case TKS_BFUN_TCFLOAT:
               ret->initFloat( r.value.float_val );
               break;
            case TKS_BFUN_TCSTR:
            {
               YAC_String *ns = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING32);
               ////ns->alloc(32);
               ns->printf("%g", r.value.float_val);
               ret->initString(ns, 1);
            }
            break;
            case TKS_BFUN_ROUND:
               if(r.value.float_val >= 0.0f)
               {
                  ret->initFloat( (sF32) ((sSI)(r.value.float_val+0.5f)) );
               }
               else
               {
                  ret->initFloat( (sF32) ((sSI)(r.value.float_val-0.5f)) );
               }
               break;
            case TKS_BFUN_FLOOR:
               ret->initFloat(floorf(r.value.float_val));
               break;
            case TKS_BFUN_CEIL:
               ret->initFloat(ceilf(r.value.float_val));
               break;
            case TKS_BFUN_SIGN:
               ret->initInt(r.value.float_val < 0.0f ? -1 : r.value.float_val > 0.0f ? 1 : 0);
               break;
            case TKS_BFUN_RND:
               // (todo) 0x3f800000 | (sU32(rand())&((1u<<23)-1u)) ..
               ret->initFloat((sF32) ((rand()*(0.999999999999f/((sF32)RAND_MAX)))*r.value.float_val) );
               break;
            case TKS_BFUN_2N:
            {
               // See <http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2Float>
               sUI u = (sUI) r.value.float_val;
               if(u > 1)
               {
                  yacmem f; // Note: MUST use union to comply with new GCC strict aliasing code
                            // (this code breaks if you use *(unsigned int*)&f instead of f.ui !)
                  f.f32 = (sF32) u;
                  unsigned int const t = 1 << ((f.u32 >> 23) - 0x7f);
                  u = t << ((t < u)?1:0);
               }
               else
               {
                  u = 1;
               }
               ret->initFloat((sF32)u);
            }
            break;
            case TKS_BFUN_TCCHR:
            {
               ret->initString((YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING8), 1);
               ret->value.string_val->chars[0]=(sU8)r.value.float_val;
               ret->value.string_val->chars[1]=(sU8)0;
               ret->value.string_val->length = 2;
            }
            break;
            case TKS_BFUN_TCOBJECT:
            {
               YAC_Float *fo=(YAC_Float*)YAC_NEW_CORE_POOLED(YAC_CLID_FLOAT);
               fo->value = r.value.float_val;
               ret->initObject(fo, 1);
            }
            break;
            case TKS_BFUN_VALUE:
            {
               YAC_ValueObject *v = (YAC_ValueObject *)YAC_NEW_CORE_POOLED(YAC_CLID_VALUE);
               v->initFloat(r.value.float_val);
               ret->initObject(v, 1);
            }
            break;
         }
         break;


      case YAC_TYPE_INT:
         switch(st->fun_selector)
         {
            default:
            case 0:
               // ILLEGAL FUN SELECTOR
               break;
            case TKS_BFUN_ADDRESS:
               ret->initInt(0);
               break;
            case TKS_BFUN_LOG:
               ret->initFloat((sF32)log((sF32)r.value.int_val));
               break;
            case TKS_BFUN_LOG2:
               ret->initFloat((sF32)log2((sF32)r.value.int_val));
               break;
            case TKS_BFUN_EXP:
               ret->initFloat((sF32)exp((sF32)r.value.int_val));
               break;
            case TKS_BFUN_TYPEID:
               ret->initInt(YAC_TYPE_INT);
               break;
            case TKS_BFUN_TYPENAME:
               ret->initEmptyString();
               ret->value.string_val->visit("int");
               break;
            case TKS_BFUN_SIN:
               ret->initFloat((sF32)sin(((sF32)r.value.int_val)));
               break;
            case TKS_BFUN_COS:
               ret->initFloat((sF32)cos(((sF32)r.value.int_val)));
               break;
            case TKS_BFUN_TAN:
               ret->initFloat((sF32)tan(((sF32)r.value.int_val)));
               break;
            case TKS_BFUN_TANH:
               ret->initFloat(tanhf(((sF32)r.value.int_val)));
               break;
            case TKS_BFUN_ASIN:
               ret->initFloat((sF32)asin(((sF32)r.value.int_val)));
               break;
            case TKS_BFUN_ACOS:
               ret->initFloat((sF32)acos(((sF32)r.value.int_val)));
               break;
            case TKS_BFUN_SQRT:
               if(r.value.int_val>0)
               {
                  ret->initFloat((sF32)sqrt(((sF32)r.value.int_val)));
               }
               else
               {
                  ret->initFloat(0.0f); // ILLEGAL SQRT(0)
               }
               break;
            case TKS_BFUN_RAD:
               ret->initFloat( (sF32)(((sF32)r.value.int_val)*2.0f*sM_PI/360.0f) );
               break;
            case TKS_BFUN_DEG:
               ret->initFloat( (sF32)(((sF32)r.value.int_val)*360.0f/2.0f*sM_PI) );
               break;
            case TKS_BFUN_ABS:
               ret->initInt( r.value.int_val<0?-r.value.int_val:r.value.int_val );
               break;
            case TKS_BFUN_FRAC:
               ret->initFloat( 0.0f );
               break;
            case TKS_BFUN_TCINT:
               ret->initInt( r.value.int_val );
               break;
            case TKS_BFUN_TCSTR:
            {
               YAC_String *ns = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING16);
               ns->printf("%i", r.value.int_val);
               ret->initString(ns, 1);
            }
            break;
            case TKS_BFUN_TCFLOAT:
            case TKS_BFUN_ROUND:
            case TKS_BFUN_FLOOR:
            case TKS_BFUN_CEIL:
               ret->initFloat( (sF32) r.value.int_val );
               break;
            case TKS_BFUN_SIGN:
               ret->initInt(r.value.int_val < 0 ? -1 : r.value.int_val > 0 ? 1 : 0);
               break;
            case TKS_BFUN_RND:
               if(r.value.int_val)
               {
                  ret->initInt(rand() % r.value.int_val);
               }
               else
               {
                  ret->initInt(0);
               }
               break;
            case TKS_BFUN_2N:
            {
               // See <http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2Float>
               sUI u = (sUI) r.value.int_val;
               if(u > 1)
               {
                  yacmem f; // Note: MUST use union to comply with new GCC strict aliasing code
                            // (this code breaks if you use *(unsigned int*)&f instead of f.ui !)
                  f.f32 = (sF32) u;
                  unsigned int const t = 1 << ((f.u32 >> 23) - 0x7f);
                  u = t << ((t < u)?1:0);
               }
               else
               {
                  u = 1;
               }
               ret->initInt((sSI)u);
            }
            break;

            case TKS_BFUN_TCCHR:
            {
               ret->initString((YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING8), 1);
               ret->value.string_val->chars[0] = (sU8)r.value.int_val;
               ret->value.string_val->chars[1] = (sU8)0;
               ret->value.string_val->length = 2;
            }
            break;

            case TKS_BFUN_TCOBJECT:
            {
               YAC_Integer *fo = (YAC_Integer*)YAC_NEW_CORE_POOLED(YAC_CLID_INTEGER);
               fo->value = r.value.int_val;
               ret->initObject(fo, 1);
            }
            break;
            case TKS_BFUN_VALUE:
            {
               YAC_ValueObject *v = (YAC_ValueObject *)YAC_NEW_CORE_POOLED(YAC_CLID_VALUE);
               v->initInt(r.value.int_val);
               ret->initObject(v, 1);
            }
            break;
         }
         break;

      case YAC_TYPE_OBJECT:
         switch(st->fun_selector)
         {
            case TKS_BFUN_SIN:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = sinf(ret->value.float_val);
               }
               break;
            case TKS_BFUN_COS:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = cosf(ret->value.float_val);
               }
               break;
            case TKS_BFUN_TAN:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = tanf(ret->value.float_val);
               }
               break;
            case TKS_BFUN_TANH:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = tanhf(ret->value.float_val);
               }
               break;
            case TKS_BFUN_ASIN:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = asinf(ret->value.float_val);
               }
               break;
            case TKS_BFUN_ACOS:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = acosf(ret->value.float_val);
               }
               break;
            case TKS_BFUN_SQRT:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  if(ret->value.float_val > 0.0f)
                     ret->value.float_val = sqrtf(ret->value.float_val);
                  else
                     ret->value.float_val = 0.0f;
               }
               break;
            case TKS_BFUN_RAD:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = (sF32)((ret->value.float_val*2.0f*sM_PI)/360.0f);
               }
               break;
            case TKS_BFUN_DEG:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = (sF32)(ret->value.float_val*360.0f/(2.0f*sM_PI));
               }
               break;
            case TKS_BFUN_ABS:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.int_val &= 0x7FFFffff;
               }
               break;
            case TKS_BFUN_FRAC:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = ret->value.float_val- ((sSI)ret->value.float_val);
               }
               break;
            case TKS_BFUN_LOG:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = logf(ret->value.float_val);
               }
               break;
            case TKS_BFUN_LOG2:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = log2f(ret->value.float_val);
               }
               break;
            case TKS_BFUN_ROUND:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  if(ret->value.float_val >= 0.0f)
                  {
                     ret->value.float_val = ( (sF32) ((sSI)(ret->value.float_val+0.5f)) );
                  }
                  else
                  {
                     ret->value.float_val = ( (sF32) ((sSI)(ret->value.float_val-0.5f)) );
                  }
               }
               break;
            case TKS_BFUN_FLOOR:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = floorf(ret->value.float_val);
               }
               break;
            case TKS_BFUN_CEIL:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = ceilf(ret->value.float_val);
               }
               break;
            case TKS_BFUN_SIGN:
               r.value.object_val->yacScanI(&ret->value.int_val);
               r.unsetFast();
               ret->initInt(ret->value.int_val < 0 ? -1 : ret->value.int_val > 0 ? 1 : 0);
               break;
            case TKS_BFUN_RND:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
                  ret->value.float_val = ((sF32) ((rand()*(0.999999999999f/((sF32)RAND_MAX)))*ret->value.float_val) );
               }
               break;
            case TKS_BFUN_ADDRESS:
#ifdef YAC_64
               {
                  YAC_Long *lo = (YAC_Long*) YAC_NEW_CORE_POOLED(YAC_CLID_LONG);
                  lo->value = (sS64) r.value.object_val;
                  ret->initObject(lo, 1);
               }
#else
               ret->initInt(r.value.int_val);
#endif
               r.unsetFast();
               break;
            case TKS_BFUN_TYPEID:
               ret->initInt(YAC_TYPE_OBJECT);
               r.unsetFast();
               break;
            case TKS_BFUN_TCINT:
               ret->initInt(0);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanI(&ret->value.int_val);
                  r.unsetFast();
               }
               break;
            case TKS_BFUN_TCFLOAT:
               ret->initFloat(0.0f);
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacScanF32(&ret->value.float_val);
                  r.unsetFast();
               }
               break;
            case TKS_BFUN_TCSTR:
               ret->initEmptyString();
               if(YAC_VALID(r.value.object_val))
               {
                  r.value.object_val->yacToString(ret->value.string_val);
                  r.unsetFast();
               }
               else
               {
                  ret->value.string_val->visit("<null>");
               }
               break;
            case TKS_BFUN_TYPENAME:
               ret->initEmptyString();
               if(YAC_VALID(r.value.object_val))
               {
                  ret->value.string_val->visit((char*)r.value.object_val->yacClassName());
                  r.unsetFast();
               }
               else
               {
                  ret->value.string_val->visit("Object");
               }
               break;

            case TKS_BFUN_TCPOINTER:
               ret->copySafe(&r);
               break;
            case TKS_BFUN_TCOBJECT:
               if(r.deleteme)
               {
                  ret->copySafe(&r);
               }
               else
               {
                  if(YAC_VALID(r.value.object_val))
                  {
                     /////YAC_Object *o=yac_host->yacNewByID(r.value.object_val->class_ID);
                     // Do not use yacNewByID so this also works for script classes
                     ////YAC_Object *o = tkscript->atom_speedpath[r.value.object_val->class_ID]->template_object->yacNewObject();
                     ////o->yacOperatorInit(_env->context, r.value.object_val);
                     YAC_Object *o = YAC_CLONE_POOLED(_env->context, r.value.object_val);
                     o->yacOperatorAssign(r.value.object_val);
                     ret->type = r.type;
                     ret->deleteme = 1;
                     ret->value.object_val = o;
                  }
                  else
                  {
                     ret->type = r.type;
                     ret->deleteme = 0;
                     ret->value.object_val = NULL;
                  }
               }
               break;
            case TKS_BFUN_VALUE:
            {
               YAC_ValueObject *v=(YAC_ValueObject *)YAC_NEW_CORE_POOLED(YAC_CLID_VALUE);
               if(YAC_VALID(r.value.object_val))
               {
                  // Returned object is neither null nor already deleted
                  v->initObject(r.value.object_val, r.deleteme);
                  if(r.value.object_val->class_ID == YAC_CLID_STRING)
                  {
                     v->type=YAC_TYPE_STRING;
                  }
               }
               else
               {
                  // Silently replace null or invalid object with null.
                  v->initNull();
               }
               r.deleteme = 0;
               ret->initObject(v, 1);
            }
            break;
         }
         break;
	}
}

void PTN_BuiltinFun::eval(PTN_Env *_env, YAC_Value *_r) const {
	PTN_BuiltinFun__eval(_env, _r, this);
}

Feval PTN_BuiltinFun::getEval(void) const {
	return PTN_BuiltinFun__eval;
}

sBool PTN_BuiltinFun::isFunAddress(void) const {
   // (note) called by PTN_DoubleArgExpr during YAC_OP_BEQ optimization
   return (TKS_BFUN_ADDRESS == fun_selector);
}

void PTN_BuiltinFun::optimize(void) {
	tks_optimize_expr(&expr, 0);
	expr_opt = expr->getEval();
}

sBool PTN_BuiltinFun::resolveXRef(void) {
    return expr->resolveXRef();
}

#ifdef TKS_JIT
sBool PTN_BuiltinFun::forceHybrid(void) {
	switch(fun_selector)
	{
	case TKS_BFUN_SIN:
	case TKS_BFUN_COS:
	case TKS_BFUN_TAN:
	case TKS_BFUN_TANH:
	case TKS_BFUN_SQRT:
	case TKS_BFUN_RAD:
	case TKS_BFUN_DEG:
	case TKS_BFUN_ABS:
	case TKS_BFUN_FRAC:
	case TKS_BFUN_TCINT:
   case TKS_BFUN_TCFLOAT:
	case TKS_BFUN_ROUND:
	case TKS_BFUN_FLOOR:
	case TKS_BFUN_CEIL:
	case TKS_BFUN_SIGN:
	case TKS_BFUN_RND:
	////case TKS_BFUN_TYPEID:
	////case TKS_BFUN_TYPENAME:
   case TKS_BFUN_ADDRESS:
		return expr->forceHybrid();
	}
	return 1;
}

sU8 PTN_BuiltinFun::compile(VMCore *_vm) {
	sU8 r=expr->compile(_vm);
	/*if(r>2)
	{
		Dprintf("[---] Object/variable type not supported by BuiltinFun::compile\n");
		return 0xFF;
	}*/
	switch(fun_selector)
	{
	default:
	case 0:
		// ILLEGAL FUN SELECTOR
		break;
   case TKS_BFUN_ADDRESS: // do nothing, just change the type to int
      return 1;

	case TKS_BFUN_SIN:
		if(r!=1)
		{
			_vm->typecastStack(r, 2);
			Dasmop(VMOP_SFSIN);
		}
		else
		{
			// int? convert from rad to deg
			_vm->pushcf((sF32)(sM_PI*2.0f/360.0f));
			Dasmop(VMOP_SFMUL);
			Dasmop(VMOP_SFSIN);
		}
		return 2;

	case TKS_BFUN_COS:
		if(r!=1)
		{
			_vm->typecastStack(r, 2);
			Dasmop(VMOP_SFCOS);
		}
		else
		{
			// int? convert from rad to deg
			_vm->pushcf((sF32)(sM_PI*2.0f/360.0f));
			Dasmop(VMOP_SFMUL);
			Dasmop(VMOP_SFCOS);
		}
		return 2;

	case TKS_BFUN_TAN:
		if(r!=1)
		{
			_vm->typecastStack(r, 2);
			Dasmop(VMOP_SFTAN);
		}
		else
		{
			// int? convert from rad to deg
			_vm->pushcf((sF32)(sM_PI*2.0f/360.0f));
			Dasmop(VMOP_SFMUL);
			Dasmop(VMOP_SFTAN);
		}
		return 2;

   case TKS_BFUN_TANH:
      // (todo) implement me
      return 2;

	case TKS_BFUN_ASIN:
		return 0xFF;

	case TKS_BFUN_ACOS:
		return 0xFF;

	case TKS_BFUN_SQRT:
		_vm->typecastStack(r, 2);
		Dasmop(VMOP_SFSQRT);
		return 2;

	case TKS_BFUN_RAD:
		_vm->typecastStack(r, 2);
		_vm->pushcf((sF32)((2.0f*sM_PI)/360.0f));
		Dasmop(VMOP_SFMUL);
		return 2;

	case TKS_BFUN_DEG:
		_vm->typecastStack(r, 2);
		_vm->pushcf((sF32)(360.0f/(2.0f*sM_PI)));
		Dasmop(VMOP_SFMUL);
		return 2;

	case TKS_BFUN_ABS:
		if(r==1)
		{
			Dasmop(VMOP_SIABS);
			return 1;
		}
		else
		{
			_vm->typecastStack(r, 2);
			Dasmop(VMOP_SFABS);
			return 2;
		}
		break;
	case TKS_BFUN_FRAC:
		_vm->typecastStack(r, 2);
		Dasmop(VMOP_SFFRAC);
		return 2;

	case TKS_BFUN_TCINT:
		_vm->typecastStack(r, 1);
		return 1;

	case TKS_BFUN_TCFLOAT:
		_vm->typecastStack(r, 2);
		return 2;

	case TKS_BFUN_ROUND:
		_vm->typecastStack(r, 2);
		Dasmop(VMOP_SFROUND);
		return 2;

	case TKS_BFUN_FLOOR:
      // (todo)
      return 1;

	case TKS_BFUN_CEIL:
      // (todo)
      return 1;

	case TKS_BFUN_SIGN:
      // (todo)
      return 1;

	case TKS_BFUN_RND:
		if(r==1)
		{
			_vm->addAPICall1((sUI)tksvm_irnd);
         Dasmop(VMOP_SRETI);
			return 1;
		}
		else
		{
			_vm->addAPICall1((sUI)tksvm_frnd);
         Dasmop(VMOP_SRETF);
			return 2;
		}

	}
	return 0xFF;
}
#endif // TKS_JIT
