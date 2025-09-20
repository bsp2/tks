/// PTN_Expr.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_compiler.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_ConstVal.h" 
#include "YAC_Integer.h" 
#include "YAC_Float.h"


PTN_Expr::PTN_Expr(void) {  
}

PTN_Expr::~PTN_Expr() {  
}

void PTN_Expr::eval(PTN_Env *_env, YAC_Value *_r) const {  
   (void)_env;
   (void)_r;
} 
 
Feval PTN_Expr::getEval(void) const { 
	return 0; 
} 
 
void PTN_Expr::evalConst(YAC_Value *_r) { 
   PTN_Env env; env.initDefault(); /// xxx TKS_MT: use *real* thread context (temp scripts!)
	eval(&env, _r); 
} 
 
void PTN_Expr::evalTC(PTN_Env *_env, sU8 _rtti, YAC_Value *_r) const { 
   //
   // Target _rtti must be int/float or Object. NOT String !
   //
	eval(_env, _r); 
	sU8 atc = (sU8) ( ((((sU8)_r->type))<<2) | (_rtti&3) ); 
	switch(atc) 
	{  
	//default: 
	//case 0:	 // void->void 
	//case 5:  // int->int 
	//case 10: // int->int 
	//case 15: // object->object 
	//case 19: // string->object 
	//	break; 
	case 1:	 // void->int 
		_r->initInt(0); 
		break; 
	case 2:	 // void->float 
		_r->initFloat(0.0f); 
		break; 
	case 3:  // void->object 
		_r->initObject(0, 0); 
		break; 
	case 4:  // int->void 
	case 8:  // float->void  
		_r->initVoid(); 
		break; 
	case 6:  // int->float 
		_r->initFloat((sF32)_r->value.int_val);
		break; 
	case 7:  // int->object  
      { 
         YAC_Integer *no = (YAC_Integer*) YAC_NEW_CORE_POOLED(YAC_CLID_INTEGER);
         no->value=_r->value.int_val; 
         _r->initObject(no, 1); 
      } 
      break;
	case 11: // float->object  
      { 
         YAC_Float *no = (YAC_Float*) YAC_NEW_CORE_POOLED(YAC_CLID_FLOAT);
         no->value=_r->value.float_val; 
         _r->initObject(no, 1); 
      } 
		break; 
	case 9:  // float->int 
      _r->initInt((sSI)_r->value.float_val);
		break; 
	case 12: // object->void 
	case 16: // string->void 
		_r->unset(); 
		break; 
	case 13: // object->int 
	case 17: // string->int 
		{ 
			if(_r->value.object_val) 
			{ 
				sSI i; 
				_r->value.object_val->yacScanI(&i); 
				_r->safeInitInt(i); 
			} 
			else  
         { 
            _r->initInt(0); // error  
         }
		} 
		break; 
	case 14: // object->float  
	case 18: // string->float 
		{ 
			if(_r->value.object_val) 
			{ 
				sF32 f; 
				_r->value.object_val->yacScanF32(&f); 
				_r->safeInitFloat(f); 
			} 
			else  
         { 
            _r->initFloat(0.0f); // error  
         }
		} 
		break; 
	} 
} 

sUI PTN_Expr::getType(void) const {
	return 0; 
}

		// int-int-int
		// int-int-float
		// int-float-int
		// int-float-float
		// float-int-int
		// float-int-float
		// float-float-int
		// float-float-float


void tks_optimize_expr(PTN_Expr **_e, sU8 _rtti/**hint**/) {
   if(*_e)
   {
      (*_e)->optimize();

      if((*_e)->isConst())
      {
         if(PTN_CONSTVAL != (*_e)->getID())
         {
            YAC_Value r;
            (*_e)->evalConst(&r);

				PTN_ConstVal *cv;
				PTN_NEW_STATIC_NODE(cv, PTN_ConstVal)();

				if(r.type >= YAC_TYPE_OBJECT && !r.deleteme)
				{
               if(YAC_VALID(r.value.object_val))
               {
                  // Create copy of const object before it is deleted by its parent
                  //   e.g.  (0 ? "A" : "B")
                  cv->const_value.duplicate(NULL/*ctx,no c'tors*/, &r);  // duplicate base object + array size
                  cv->const_value.value.object_val->yacOperatorAssign(r.value.object_val);
               }
               else
               {
                  // e.g. {} empty list (null) in myfuncobj.eval({})
                  cv->const_value.initNull();
               }
				}
				else
				{
					cv->const_value.fastDerefValue(&r);
				}

				if(_rtti)
				{
					cv->const_value.typecast(_rtti);
				}

				delete *_e;
            *_e = cv;
         }
         else
         {
            // ---- is const , check for rtti hint
            if(_rtti)
            {
               ((PTN_ConstVal*)(*_e))->const_value.typecast(_rtti);
            }
         }
      }
   }
}
