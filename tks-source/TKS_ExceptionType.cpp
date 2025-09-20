/// TKS_ExceptionType.cpp
///
/// (c) 2006-2007 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
 

#include "tks.h"

#include "TKS_ExceptionType.h"

TKS_ExceptionType::TKS_ExceptionType(void) {
   parent_module   = NULL;
   parent_class    = NULL;
   ////suggested_class = NULL;
   next_exception  = NULL;
   is_declared     = 0;
   exception_id    = 0xFFFFffff;
}

TKS_ExceptionType::TKS_ExceptionType(sUI _baseId, sUI _id, const char *_name) {
   parent_module    = NULL;
   parent_class     = NULL;
   ////suggested_class  = NULL;
   next_exception   = NULL;
   parent_exception = tkscript->getExceptionTypeById(_baseId); // xxx check fail 
   exception_id     = _id; // assign absolute id
   name.copy((char*)_name);
   is_declared      = 1;
}

TKS_ExceptionType::TKS_ExceptionType(TKS_ExceptionType *_baseType, YAC_String *_name) {
   parent_module    = NULL;
   parent_class     = NULL;
   ////suggested_class  = NULL;
   next_exception   = NULL;
   parent_exception = _baseType;
   exception_id     = 0xFFFFffff; // dynamically assign id
   name.yacCopy(_name);
   is_declared      = 1;
}

TKS_ExceptionType::~TKS_ExceptionType() {
   if(next_exception != NULL)
   {
      delete next_exception;
      next_exception = NULL;
   }
}

