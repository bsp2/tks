/// TKX_File.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
///
#include "tks.h"
#include "TKX_File.h"


TKX_File::TKX_File(void) {
   next                 = NULL;
   file_offset          = 0u;
   file_size            = 0u;
   file_data            = NULL;
   type                 = CFE_TYPE_UNKNOWN;
   tokenized_source.any = NULL;
   b_done               = YAC_FALSE;
}

TKX_File::~TKX_File() { 
   freeTokenizedSource();
} 

sBool TKX_File::initLibFile(const char *_libName) {
   type = CFE_TYPE_LIB;
   if(source_name.alloc(256))
   {
      source_name.printf("lib:%s", _libName);
      // yac_host->printf("xxx TKX_File::initLibFile: libName=\"%s\" source_name.chars=\"%s\"\n", _libName, source_name.chars);
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

void TKX_File::freeTokenizedSource(void) {
   if(NULL != tokenized_source.any)
   {
      delete [] tokenized_source.u8;
      tokenized_source.any = NULL;
   }
}

