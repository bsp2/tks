/// YAC_PakFile.cpp
///
/// (c) 2001-2008 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdlib.h>

#include "tks.h"
#include "TKS_StreamIO.h"
#include "YAC_Buffer.h"
#include "YAC_PakFile.h"



YAC_PakFile::YAC_PakFile(void) { 
   sio = NULL; 
} 

YAC_PakFile::~YAC_PakFile(void) { 
   yacStreamClose(); 
} 

void YAC_VCALL YAC_PakFile::yacOperator(sSI _cmd, YAC_Object *_obj, YAC_Value *_r) { 
   switch(_cmd) 
   { 
   default:
      YAC_Object::yacOperator(_cmd, _obj, _r);
      break;

   case YAC_OP_SHL: 
      if(_obj != NULL) 
      {
         _obj->yacSerialize(this, 1); 
      }
      break; 
   } 
} 

sBool YAC_VCALL YAC_PakFile::yacIsStream(void) { 
   return 1; 
} 

sUI YAC_VCALL YAC_PakFile::yacStreamGetOffset(void) { 
   if(sio != NULL) 
   {
      return sio->io_offset; 
   }
   return 0; 
} 

void YAC_VCALL YAC_PakFile::yacStreamSetOffset(sUI _off) { 
   if(sio != NULL) 
   {
      sio->seek(_off, YAC_BEG); 
   }
} 

void YAC_VCALL YAC_PakFile::yacStreamSeek(sSI _offset, sUI _mode) { 
   if(sio != NULL)
   {
      sio->seek(_offset, (sU8)_mode); 
   }
} 

sSI YAC_VCALL YAC_PakFile::yacStreamGetErrorCode(void) { 
   if(sio) 
   {
      return sio->error_code; 
   }
   else
   {
      return -1;
   }
} 

sSI YAC_PakFile::openLocal(YAC_String *_name, sSI _access) { 
#ifdef TKS_PAKFILE_OPENLOCAL
   return yacStreamOpenLocal((sChar*)_name->chars, _access); 
#else
   Dprintf("[---] Pakfile::openLocal: METHOD NOT COMPILED in.\n");
   return 0;
#endif // TKS_PAKFILE_OPENLOCAL
} 

sBool YAC_VCALL YAC_PakFile::yacStreamOpenLocal(sChar *_local_pathname, sSI _access) { 
#ifdef TKS_PAKFILE_OPENLOCAL
   yacStreamClose(); 

   sio = new TKS_StreamIO(tkscript); 

   if(sio != NULL) 
   { 
      if(sio->openLocal(_local_pathname, _access)) 
      {
         return 1; 
      }
      delete sio; 
      sio = NULL; 
   } 
   return 0; 
#else
   Dprintf("[---] Pakfile::yacStreamOpenLocal: METHOD NOT COMPILED in.\n");
   return 0;
#endif // TKS_PAKFILE_OPENLOCAL
}  

sBool YAC_VCALL YAC_PakFile::yacStreamOpenLogic(sChar *_logic_name) { 
   YAC_String s; 
   s.visit(_logic_name); 
   return YAC_PakFile::_open(&s); 
} 

sSI YAC_PakFile::_open(YAC_String *_name) { 
   yacStreamClose(); 

   sio = new TKS_StreamIO(tkscript); 

   if(sio != NULL) 
   { 
      if(sio->openLogic(_name)) 
      {
         return 1; 
      }
      delete sio; 
      sio = NULL; 
   } 
   return 0; 
} 

sSI YAC_VCALL YAC_PakFile::yacStreamRead(sU8 *_d, sUI _l) { 
   // bit_offset = 0u;
   if(sio != NULL)  
   {
      return sio->read(_d, _l); 
   }
   else  
   {
      return 0; 
   }
} 


sU8 YAC_VCALL YAC_PakFile::yacStreamReadI8(void) { 
   // bit_offset = 0u;
   if(sio != NULL) 
   { 
      sU8 r; 
      sio->read(&r, 1); 
      return r; 
   } 
   else 
   {
      return 0; 
   }
} 

void YAC_VCALL YAC_PakFile::yacStreamClose(void) { 
   if(sio) 
   { 
      delete sio; 
      sio = NULL; 
   } 
} 

sUI YAC_VCALL YAC_PakFile::yacStreamGetSize(void) { 
   if(sio != NULL)	
   {
      return sio->getFileSize(); 
   }
   return 0; 
} 

sSI YAC_PakFile::isOpen(void) { 
   return (sio != NULL); 
} 

sBool YAC_VCALL YAC_PakFile::yacStreamEOF(void) { 
   if(sio != NULL)
   {
      return (sio->io_offset==sio->io_size);
   }
   else
   {
      return 1;
   }
} 

