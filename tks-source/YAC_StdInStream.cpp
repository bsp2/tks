/// YAC_StdInStream.cpp
///
/// (c) 2003-2020 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdlib.h>

#include "tks.h"
#include "TKS_StreamIO.h"
#include "YAC_Buffer.h"
 
#include "YAC_StdInStream.h"


YAC_StdInStream::YAC_StdInStream(void) { 
   b_eof = YAC_FALSE;
} 

YAC_StdInStream::~YAC_StdInStream(void) { 
} 

void YAC_VCALL YAC_StdInStream::yacOperator(sSI _cmd, YAC_Object *_obj, YAC_Value *_r) { 
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

sBool YAC_VCALL YAC_StdInStream::yacIsStream(void) { 
   return 1; 
} 

sUI YAC_VCALL YAC_StdInStream::yacStreamGetOffset(void) { 
   return ::ftell(stdin); 
} 

sSI YAC_VCALL YAC_StdInStream::yacStreamGetErrorCode(void) { 
   return ferror(stdin); 
} 

void YAC_VCALL YAC_StdInStream::yacStreamGetErrorStringByCode(sSI _code, YAC_Value *_r) { 
   YacStreamGetErrorStringByCode(_code,_r); 
} 

void YAC_VCALL YAC_StdInStream::yacStreamSetOffset(sUI _off) { 
   ::fseek(stdin, _off, SEEK_SET); 
} 

void YAC_VCALL YAC_StdInStream::yacStreamSeek(sSI _offset, sUI _mode) { 
   ::fseek(stdin, _offset, _mode==0?SEEK_SET:_mode==1?SEEK_CUR:SEEK_END); 
} 

sSI YAC_VCALL YAC_StdInStream::yacStreamRead(sU8 *_d, sUI _l) { 
   return (sSI)::fread((void*)_d, 1, (size_t)_l, stdin); 
} 

sU8 YAC_VCALL YAC_StdInStream::yacStreamReadI8(void) { 
   if(b_eof)
      return 0u;
   int r=::fgetc(stdin); 
   if(r==EOF)  
   { 
      b_eof = YAC_TRUE;
      // yac_host->printf("xxx YAC_StdInStream:.yacStreamReadI8: found EOF\n");
      return 0; 
   }
   // yac_host->printf("xxx YAC_StdInStream:.yacStreamReadI8: read r=%u (%c)\n", r, char(r));
   // yac_host->printf("xxx YAC_StdInStream:.yacStreamReadI8: read r=%u\n", r);
   return (sU8) r; 
} 

void YAC_StdInStream::flush(void) { 
   // xxx flushing stdin not portable ?
   ::fflush(stdin); 
} 

sUI YAC_VCALL YAC_StdInStream::yacStreamGetSize(void) { 
   return 0; 
   /*long fpos=::ftell(stdin); 
   long e=::fseek(stdin, 0, SEEK_END); 
   ::fseek(stdin, fpos, SEEK_SET); 
   return e;*/ 
} 

sSI YAC_StdInStream::isOpen(void) { 
   return 1; 
} 

sBool YAC_VCALL YAC_StdInStream::yacStreamEOF(void) { 
   // yac_host->printf("YAC_StdInStream::yacStreamEOF: b_eof=%d\n", b_eof);
   return b_eof; 
} 
