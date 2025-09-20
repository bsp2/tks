/// YAC_StdOutStream.cpp
///
/// (c) 2003-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdlib.h>

#include "tks.h"
#include "TKS_StreamIO.h"
#include "YAC_Buffer.h"
 
#include "YAC_StdOutStream.h"


YAC_StdOutStream::YAC_StdOutStream(void) { 
} 

YAC_StdOutStream::~YAC_StdOutStream(void) { 
} 

void YAC_VCALL YAC_StdOutStream::yacOperator(sSI _cmd, YAC_Object *_obj, YAC_Value *_r) { 
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

sBool YAC_VCALL YAC_StdOutStream::yacIsStream(void) { 
   return 1; 
} 

sUI YAC_VCALL YAC_StdOutStream::yacStreamGetOffset(void) { 
   return ::ftell(stdout); 
} 

sSI YAC_VCALL YAC_StdOutStream::yacStreamGetErrorCode(void) { 
   return ferror(stdout); 
} 

void YAC_VCALL YAC_StdOutStream::yacStreamGetErrorStringByCode(sSI _code, YAC_Value *_r) { 
   YacStreamGetErrorStringByCode(_code,_r); 
} 

sSI YAC_VCALL YAC_StdOutStream::yacStreamWrite(sU8 *_d, sUI _l) { 
   if(tkscript->configuration.b_enableprint)
   {
      return (sSI)::fwrite((void*)_d, (size_t)_l, 1, stdout); 
   }
   else
   {
      return 1;
   }
} 

void YAC_VCALL YAC_StdOutStream::yacStreamWriteI8(sU8 _i) { 
   if(tkscript->configuration.b_enableprint)
   {
      ::fputc(_i, stdout); 
   }
} 

void YAC_StdOutStream::flush(void) { 
   ::fflush(stdout); 
} 

sUI YAC_VCALL YAC_StdOutStream::yacStreamGetSize(void) { 
   return ::ftell(stdout); 
} 

sSI YAC_StdOutStream::isOpen(void) { 
   return 1; 
} 

sBool YAC_VCALL YAC_StdOutStream::yacStreamEOF(void) { 
   return 1; 
} 
