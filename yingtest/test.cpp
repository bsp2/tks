#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
YAC_Host *yac_host;

#include "test.h"

#include "ying_test.h"

#include "ying_test_MyClass.cpp"
#include "ying_test_MyVector.cpp"
#include "ying_test_MyEmptyClass.cpp"
#include "ying_test_MyPooledClass.cpp"

#include "ying_test.cpp"


// Implement standard exception ID variables (see yac.h)
Dyac_std_exid_impl;


sF32 MyVector::getAbs(void) {
   return (sF32) sqrt(x*x + y*y + z*z);
}


void YAC_CALL Say_Hello(void) {
	yac_host->printf("hello, world.\n");
}

void YAC_CALL _Say_Hello2(void) {
	yac_host->printf("hello, world 2.\n");
}

void YAC_CALL Fun_ReturnNewMyClass(YAC_Value *_r) {
	_r->initObject(yac_host->yacNewByID(clid_MyClass),1);
}

void MyClass::returnNewMyClass(YAC_Value *_r) {
	Fun_ReturnNewMyClass(_r);
}

void YAC_CALL RaiseSegV(void) {
   *(volatile sUI*)0 = 0;
}

void YAC_CALL _TestVarRetMangled(YAC_Object *_o, YAC_Value *_r) {
   _r->initObject(_o, 0);
}

void YAC_CALL Raise_InvalidPointer(void) {
   YAC_ExceptionId exid = yac_host->yacExceptionGetIdByName("InvalidPointer");

   // Raise exception
   yac_host->yacExceptionRaise(yac_host->yacContextGetDefault(), exid, "just a test!", __FILE__, __LINE__);
}

YF void YAC_CALL TestObjRet0_YAC_RARG(YAC_Object *_ret42) {
   // Returns "42" in "return argument"
   // Note: _ret42 is always valid at this point

   // Check whether return object is a simple scalar or a has a variant tensor rank (e.g. String, ListNode)
   sSI tr = _ret42->yacTensorRank();
   if( (YAC_TENSOR_RANK_SCALAR == tr) || (YAC_TENSOR_RANK_VAR == tr) )
   {
      _ret42->yacValueOfI(42);
   }
   else
   {
      Dyac_throw_def(TypeMismatch, "expected scalar return object");
   }
}

YF void YAC_CALL TestObjRet1_YAC_RARG(int _i, YAC_Object *_ret) {
   // Returns _i in "return argument"
   // Note: _ret is always valid at this point

   // Check whether return object is a simple scalar or a has a variant tensor rank (e.g. String, ListNode)
   sSI tr = _ret->yacTensorRank();
   if( (YAC_TENSOR_RANK_SCALAR == tr) || (YAC_TENSOR_RANK_VAR == tr) )
   {
      _ret->yacValueOfI(_i);
   }
   else
   {
      Dyac_throw_def(TypeMismatch, "expected scalar return object");
   }
}

void YAC_CALL TestObjRet0_YAC_RVAL(YAC_Value *_r) {
   // Returns new Integer object (int. value= 42)
   YAC_Integer *io = (YAC_Integer*) YAC_NEW_CORE_POOLED(YAC_CLID_INTEGER);
   io->value = 42;
   _r->initObject(io, 1);
}

void YAC_CALL TestObjRet1_YAC_RVAL(int _i, YAC_Value *_r) {
   // Returns new Integer object (int.value = _i)
   YAC_Integer *io = (YAC_Integer*) YAC_NEW_CORE_POOLED(YAC_CLID_INTEGER);
   io->value = _i;
   _r->initObject(io, 1);
}

void MyClass::testObjRet0_YAC_RARG(YAC_Object *_ret42) {
   // Returns "42" in "return argument"
   // Note: _ret42 is always valid at this point

   // Check whether return object is a simple scalar or a has a variant tensor rank (e.g. String, ListNode)
   sSI tr = _ret42->yacTensorRank();
   if( (YAC_TENSOR_RANK_SCALAR == tr) || (YAC_TENSOR_RANK_VAR == tr) )
   {
      _ret42->yacValueOfI(42);
   }
   else
   {
      Dyac_throw_def(TypeMismatch, "expected scalar return object");
   }
}

void MyClass::testObjRet1_YAC_RARG(int _i, YAC_Object *_ret) {
   // Returns _i in "return argument"
   // Note: _ret is always valid at this point

   // Check whether return object is a simple scalar or a has a variant tensor rank (e.g. String, ListNode)
   sSI tr = _ret->yacTensorRank();
   if( (YAC_TENSOR_RANK_SCALAR == tr) || (YAC_TENSOR_RANK_VAR == tr) )
   {
      _ret->yacValueOfI(_i);
   }
   else
   {
      Dyac_throw_def(TypeMismatch, "expected scalar return object");
   }
}

void MyClass::testObjRet0_YAC_RVAL(YAC_Value *_r) {
   // Returns new Integer object (int. value= 42)
   YAC_Integer *io = (YAC_Integer*) YAC_NEW_CORE_POOLED(YAC_CLID_INTEGER);
   io->value = 42;
   _r->initObject(io, 1);
}

void MyClass::testObjRet1_YAC_RVAL(int _i, YAC_Value *_r) {
   // Returns new Integer object (int.value = _i)
   YAC_Integer *io = (YAC_Integer*) YAC_NEW_CORE_POOLED(YAC_CLID_INTEGER);
   io->value = _i;
   _r->initObject(io, 1);
}

void MyClass::testObjRet1_YAC_RSELF(sSI _i) {
   // Modify self
   // Note: this is indeed a very simple example :-)
   i = _i;
}



// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;

   // Resolve "standard" exception IDs
   Dyac_std_exid_resolve;

   YAC_Init_test(_host);
   yac_host->printf("init done.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_test(_host);
}


#include <yac_host.cpp>
