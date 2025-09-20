/// YAC_StdErrStream.cpp
///
/// (c) 2003-2006 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdlib.h>

#include "tks.h"
#include "TKS_StreamIO.h"
#include "YAC_Buffer.h" 

#include "YAC_StdErrStream.h"


YAC_StdErrStream::YAC_StdErrStream(void) { 
} 

YAC_StdErrStream::~YAC_StdErrStream(void) { 
} 

void YAC_VCALL YAC_StdErrStream::yacOperator(sSI _cmd, YAC_Object *_obj, YAC_Value *_r) { 
   switch(_cmd) 
   { 
   default:
      YAC_Object::yacOperator(_cmd, _obj, _r);
      break;
   case YAC_OP_SHL: 
      if(_obj) _obj->yacSerialize(this, 1); 
      break; 
   } 
} 

sBool YAC_VCALL YAC_StdErrStream::yacIsStream(void) { 
   return 1; 
} 

sUI YAC_VCALL YAC_StdErrStream::yacStreamGetOffset(void) { 
   return ::ftell(stdout); 
} 

sSI YAC_VCALL YAC_StdErrStream::yacStreamGetErrorCode(void) { 
   return ferror(stdout); 
} 

void YAC_VCALL YAC_StdErrStream::yacStreamGetErrorStringByCode(sSI _code, YAC_Value *_r) { 
   YacStreamGetErrorStringByCode(_code,_r); 
} 

sSI YAC_VCALL YAC_StdErrStream::yacStreamWrite(sU8 *_d, sUI _l) { 
   return (sSI)::fwrite((void*)_d, (size_t)_l, 1, stdout); 
} 

void YAC_VCALL YAC_StdErrStream::yacStreamWriteI8(sU8 _i) { 
   ::fputc(_i, stdout); 
} 

void YAC_StdErrStream::flush(void) { 
   ::fflush(stdout); 
} 

sUI YAC_VCALL YAC_StdErrStream::yacStreamGetSize(void) { 
   return ::ftell(stdout); 
} 

sSI YAC_StdErrStream::isOpen(void) { 
   return 1; 
} 

sBool YAC_VCALL YAC_StdErrStream::yacStreamEOF(void) { 
   return 1; 
} 
