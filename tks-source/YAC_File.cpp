/// YAC_File.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>
#include <stdlib.h>

#include "tks.h"
#include "TKS_StreamIO.h"
#include "YAC_Buffer.h"
#include "YAC_File.h"


YAC_File::YAC_File(void) {
   handle  = NULL;
   tmpName = NULL;
}

YAC_File::~YAC_File(void) {
   yacStreamClose();
   removeTemp();
}

void YAC_VCALL YAC_File::yacGetConstantStringList(YAC_String *_s) {
   _s->copy(
      "SEEK_SET:0 "
      "SEEK_CUR:1 "
      "SEEK_END:2 "
      "ERRINVALIDSEEK:1 "
      "ERRIO:2 "
      "ERRCREATEFILE:3 "
      "ERROPENFILE:4 "
      "IOS_OUT:1 "
      "IOS_IN:0 "
      "IOS_INOUT:2 "
      );
}

void YAC_VCALL YAC_File::yacOperator(sSI _cmd, YAC_Object *_obj, YAC_Value *_r) {
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

sBool YAC_VCALL YAC_File::yacIsStream(void) {
   return 1;
}

sUI YAC_VCALL YAC_File::yacStreamGetOffset(void) {
   if(handle)
      return ::ftell(handle);
   return 0;
}

void YAC_VCALL YAC_File::yacStreamSetOffset(sUI _off) {
   if(handle)
      ::fseek(handle, _off, SEEK_SET);
}

void YAC_VCALL YAC_File::yacStreamSeek(sSI _offset, sUI _mode) {
   if(handle)
      ::fseek(handle, _offset, _mode==0?SEEK_SET:_mode==1?SEEK_CUR:SEEK_END);
}

sSI YAC_VCALL YAC_File::yacStreamGetErrorCode(void) {
   if(handle)
      return ferror(handle);
   else
      return -1;
}

void YAC_VCALL YAC_File::yacStreamGetErrorStringByCode(sSI _code, YAC_Value *_r) {
   YacStreamGetErrorStringByCode(_code,_r);
}

void YAC_File::_openTemp(YAC_String *_dir, YAC_String *_pfx, YAC_Value *_r) {
#ifdef TKS_OLD_TEMP_FILES
   if(tkscript->configuration.b_enablelocalfiles)
   {
      const char *nd;
      const char *npfx;
      if(!_dir->isBlank())
         nd=(const char*)_dir->chars;
      else
         nd=0;
      if(!_pfx->isBlank())
         npfx=(const char*)_pfx->chars;
      else
         npfx=0;
#ifdef YAC_GCC
      char *n = tempnam((char*)nd, (char*)npfx); // returned char* is allocated with malloc()
#else
      char *n = _tempnam((char*)nd, (char*)npfx); // returned char* is allocated with malloc()
#endif // YAC_GCC
      if(n)
      {
         if(yacStreamOpenLocal(n, 1))
         {
            YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
            if(s)
               s->copy(n);
            _r->initString(s,(s!=0));
            tmpName=n; // free tmpName and unlink file if it's closed
         }
         else
            ::free((void*)n);
      }
   }
   else
   {
      Dprintf("[---] File::openTempFile(\"%s\", \"%s\"): local file access is not enabled.\n", (char*)_dir->chars, (char*)_pfx->chars);
   }
#else
   Dprintf("[---] File::openTempFile(\"%s\", \"%s\"): METHOD NOT COMPILED IN.\n", (char*)_dir->chars, (char*)_pfx->chars);
#endif // TKS_OLD_TEMP_FILES
}

sSI YAC_File::openLocal(YAC_String *_name, sSI _access) {
   return yacStreamOpenLocal((sChar*)_name->chars, _access);
}

sSI YAC_File::_open(YAC_String *_name, sSI _access) {
   return yacStreamOpenLocal((sChar*)_name->chars, _access);
}

sBool YAC_VCALL YAC_File::yacStreamOpenLocal(sChar *_name, sSI _access) {
   yacStreamClose();
   if(_access&&!tkscript->configuration.b_enablelocalfiles)
   {
      Dprintf("[---] File: access restrictions forbid access to local file \"%s\".\n",
         (char*)_name);
      return 0;
   }
   //tkscript->printf("openLocal name=%s access=%i\n", _name->chars, _access);
   switch(_access)
   {
   default:
      Dprintf("[---] File::open(\"%s\", %i): unsupported access mode.\n", _name, _access);
      break;
   case 0:
      // IOS_IN
      handle=::fopen((const char*)_name, "rb");
      break;
   case 1:
      // IOS_OUT
      handle=::fopen((const char*)_name, "wb");
      break;
   case 2:
      // IOS_INOUT
      handle=::fopen((const char*)_name, "rb+");
      break;
   }
   return (handle!=0);
}


sSI YAC_VCALL YAC_File::yacStreamRead(sU8 *_d, sUI _l) {
   // bit_offset = 0u;
   if(handle)
   {
      return (sSI)::fread((void*)_d, 1, (size_t)_l, handle);
   }
   else
      return 0;
}


sSI YAC_VCALL YAC_File::yacStreamWrite(sU8 *_d, sUI _l) {
   // bit_offset = 0u;
   if(handle)
   {
      return (sSI)::fwrite((void*)_d, (size_t)_l, 1, handle);
   }
   else
      return 0;
}

void YAC_VCALL YAC_File::yacStreamWriteI8(sU8 _i) {
   // bit_offset = 0u;
   if(handle)
   {
      ::fputc(_i, handle); //::fwrite((void*)&_i, (size_t)_1, 1, handle);
   }
}

sU8 YAC_VCALL YAC_File::yacStreamReadI8(void) {
   // bit_offset = 0u;
   if(handle)
   {
      int r=::fgetc(handle);
      if(r==EOF) return 0;
      return (sU8) r;
   }
   else
   {
      return 0;
   }
}

void YAC_File::flush(void) {
   if(handle)
      ::fflush(handle);
}

void YAC_File::removeTemp(void) {
   if(tmpName)
   {
#ifdef TKS_OLD_TEMP_FILES
      if(::remove((const char*)tmpName))
      {
         Dprintf("[---] File: failed to remove tmp file \"%s\".\n", (char*)tmpName);
      }
      ::free((void*)tmpName);
      tmpName=0;
#else
   Dprintf("[---] File::removeTemp: METHOD NOT COMPILED IN.\n");
#endif // TKS_OLD_TEMP_FILES
   }

}

void YAC_VCALL YAC_File::yacStreamClose(void) {
   if(handle)
   {
      ::fclose(handle);
      handle = NULL;
   }
   // bit_offset = 0u;
}

sUI YAC_VCALL YAC_File::yacStreamGetSize(void) {
   if(handle)
   {
      long e,fpos=ftell(handle);
      if(!fseek(handle, 0, SEEK_END))
         e=ftell(handle);
      else
         e=0;
      if(fpos>=0)
         fseek(handle, fpos, SEEK_SET);
      return e;
   }
   return 0;
}

sSI YAC_File::isOpen(void) {
   return (NULL != handle);
}

sBool YAC_VCALL YAC_File::yacStreamEOF(void) {
   if(handle)
   {
      long cPos = ftell(handle);
      fseek(handle, 0, SEEK_END);
      long endPos = ftell(handle);
      fseek(handle, cPos, SEEK_SET);
      return (cPos >= endPos);
      ////sBool bEOF = (0 != feof(handle)); // note: feof(handle) return 0 if filepos is exactly at the EOF (bug, feature?)
      ////return bEOF;
   }
   else
   {
      return 1;
   }
}
