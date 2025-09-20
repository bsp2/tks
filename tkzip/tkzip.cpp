// ----
// ---- file   : tkzip.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2020 by Bastian Spiegel. 
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See 
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : libzip interface
// ----
// ---- created: 04Jan2020
// ---- changed: 10Jan2020
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
YAC_Host *yac_host;

#include <zip.h>

#include "tkzip.h"

#include "ying_zip.h"

#include "ying_zip_Zip.cpp"

#include "ying_zip.cpp"


// Implement standard exception ID variables (see yac.h)
Dyac_std_exid_impl;


// ---------------------------------------------------------------------------- Zip
Zip::Zip(void) {
   zip = NULL;
   sources = NULL;
}

Zip::~Zip() {
   close();
}

void Zip::close(void) {

   if(NULL != zip)
   {
      zip_close(zip);
      zip = NULL;
   }

   // yac_host->printf("close 1\n");
   freeSources();
   // yac_host->printf("close 2\n");

}

void Zip::freeSources(void) {
   tkzip_source_t *c = sources;
   while(NULL != c)
   {
      if(c->b_free_source)
         zip_source_free(c->source);
      delete [] c->data.u8;
      c->data.any = NULL;
      tkzip_source_t *p = c->prev;
      delete c;
      c = p;
   }
   sources = NULL;
}

tkzip_source_t *Zip::addSourceFromData(const void *_dataOrNull, sUI _numBytes) {
   // (note) copy data
   tkzip_source_t *ret = NULL;
   if(NULL != zip)
   {
      tkzip_source_t *n = new(std::nothrow) tkzip_source_t;
      if(NULL != n)
      {
         n->b_free_source = YAC_TRUE;
         n->data.u8 = new(std::nothrow)sU8[_numBytes];
         n->num_bytes = _numBytes;
         if(NULL != n->data.any)
         {
            if(NULL != _dataOrNull)
               ::memcpy(n->data.any, _dataOrNull, _numBytes);

            n->source = zip_source_buffer(zip, n->data.any, _numBytes, 0/*freep*/);

            if(NULL != n->source)
            {
               n->prev = sources;
               ret = sources = n;
            }
            else
            {
               zip_source_free(n->source);
               delete [] n->data.u8;
               n->data.any = NULL;
               delete n;
            }
         }
         else
         {
            delete n;
         }
      }
   }
   return ret;
}

sBool Zip::openLocalInt(YAC_String *_pathName, int _flags) {
   close();

   if(YAC_Is_String(_pathName))
   {
      int error;
      zip = zip_open((const char*)_pathName->chars, _flags, &error);
   }

   return (NULL != zip);
}

sBool Zip::openLocalReadOnly(YAC_String *_pathName) {
   return openLocalInt(_pathName, ZIP_RDONLY/*flags*/);
}

sBool Zip::openLocal(YAC_String *_pathName) {
   return openLocalInt(_pathName, 0/*flags*/);
}

sBool Zip::createLocal(YAC_String *_pathName) {
   return openLocalInt(_pathName, ZIP_CREATE | ZIP_TRUNCATE);
}

sUI Zip::getNumEntries(void) {
   sUI r  = 0u;
   if(NULL != zip)
   {
      r = sUI(zip_get_num_entries(zip, 0u/*flags*/));
   }
   return r;
}

void Zip::getNameByIndex(sUI _index, YAC_Value *_r) {
   if(NULL != zip)
   {
      const char *name = zip_get_name(zip, _index, ZIP_FL_ENC_RAW);
      if(NULL != name)
      {
         _r->initEmptyString();
         _r->value.string_val->copy(name);
      }
   }
}

sSI Zip::getIndexByName(YAC_String *_pathName) {
   sSI r = -1;
   if(YAC_Is_String(_pathName))
   {
      if(NULL != zip)
      {
         r = sSI(zip_name_locate(zip, (const char*)_pathName->chars, ZIP_FL_ENC_GUESS));
      }
   }
   return r;
}

sS64 Zip::getSizeByIndexInt(sUI _index) {
   sS64 r = -1;
   if(NULL != zip)
   {
      zip_stat_t st;
      if(0 == zip_stat_index(zip, _index, ZIP_FL_ENC_GUESS, &st))
      {
         if(0u != (st.valid & ZIP_STAT_SIZE))
         {
            r = sS64(st.size);
         }
      }
   }
   return r;
}

void Zip::getSizeByIndex(sUI _index, YAC_Value *_r) {
   sS64 r = getSizeByIndexInt(_index);
   if(r >= 0)
   {
      _r->initObject(YAC_New_UnsignedLong(), YAC_TRUE);
      _r->value.object_val->yacValueOfI64(r);
   }
}

void Zip::getCompressedSizeByIndex(sUI _index, YAC_Value *_r) {
   yacmem64 r; r.u64 = 0u;
   if(NULL != zip)
   {
      zip_stat_t st;
      if(0 == zip_stat_index(zip, _index, ZIP_FL_ENC_GUESS, &st))
      {
         if(0u != (st.valid & ZIP_STAT_COMP_SIZE))
         {
            r.u64 = st.comp_size;
         }
      }
   }
   _r->initObject(YAC_New_UnsignedLong(), YAC_TRUE);
   _r->value.object_val->yacValueOfI64(r.s64);
}

void Zip::loadStringByIndex(sUI _index, YAC_Value *_r) {
   if(NULL != zip)
   {
      zip_file_t *zf = zip_fopen_index(zip, _index, 0u/*flags*/);
      if(NULL != zf)
      {
         sS64 sz = getSizeByIndexInt(_index);
         _r->initEmptyString();
         sBool bDone = YAC_FALSE;
         if(sz > 0)
         {
            if(0 == (sz & ~sS64(sU32_MAX)))
            {
               if(_r->value.string_val->alloc(sUI(sz) + 1))
               {
                  (void)zip_fread(zf, (void*)_r->value.string_val->chars, sz);
                  _r->value.string_val->fixLength();
                  bDone = YAC_TRUE;
               }
            }
            else
            {
               yac_host->printf("[---] Zip::loadString: file is larger than 4GB, skipping\n");
            }
         }
         if(!bDone)
         {
            _r->initNull();
         }
         zip_fclose(zf);
      }
   }
}

void Zip::loadStringByName(YAC_String *_pathName, YAC_Value *_r) {
   sSI idx = getIndexByName(_pathName);
   if(idx >= 0)
   {
      loadStringByIndex(sUI(idx), _r);
   }
}

void Zip::loadBufferByIndex(sUI _index, YAC_Value *_r) {
   if(NULL != zip)
   {
      zip_file_t *zf = zip_fopen_index(zip, _index, 0u/*flags*/);
      if(NULL != zf)
      {
         sS64 sz = getSizeByIndexInt(_index);
         YAC_Buffer *b = YAC_New_Buffer();
         _r->initObject(b, YAC_TRUE);
         sBool bDone = YAC_FALSE;
         if(sz > 0)
         {
            if(0 == (sz & ~sS64(sU32_MAX)))
            {
               if(b->yacArrayAlloc(sUI(sz + 1), 0,0,0))
               {
                  (void)zip_fread(zf, b->buffer, sz);
                  b->io_offset = sUI(sz);
                  bDone = YAC_TRUE;
               }
            }
            else
            {
               yac_host->printf("[---] Zip::loadBuffer: file is larger than 4GB, skipping\n");
            }
         }
         if(!bDone)
         {
            _r->initNull();
         }
         zip_fclose(zf);
      }
   }
}

void Zip::loadBufferByName(YAC_String *_pathName, YAC_Value *_r) {
   sSI idx = getIndexByName(_pathName);
   if(idx >= 0)
   {
      loadBufferByIndex(sUI(idx), _r);
   }
}

sBool Zip::addDir(YAC_String *_pathName) {
   sBool r = YAC_FALSE;

   if(NULL != zip)
   {
      if(YAC_Is_String(_pathName))
      {
         if(0 <= zip_dir_add(zip, (const char*)_pathName->chars, ZIP_FL_ENC_GUESS))
         {
            r = YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] zip_dir_add() failed (pathName=\"%s\").\n", (const char*)_pathName->chars);
         }
      }
   }

   return r;
}

sBool Zip::addFile(YAC_String *_pathName, YAC_Object *_data) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_String(_pathName))
   {
      if(YAC_VALID(_data))
      {
         void *ptr = _data->yacArrayGetPointer();
         if(NULL != ptr)
         {
            // Read from array
            sUI sz = _data->yacArrayGetWidth() * _data->yacArrayGetElementByteSize();
            if(sz > 0)
            {
               tkzip_source_t *s = addSourceFromData(ptr, sz);
               if(NULL != s)
               {
                  ret = (0 <= zip_file_add(zip, (const char*)_pathName->chars, s->source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS));
                  if(ret)
                     s->b_free_source = YAC_FALSE;
               }
               else
               {
                  yac_host->printf("[---] Zip::addSourceFromData() failed (pathName=\"%s\").\n", (const char*)_pathName->chars);
               }
            }
         }
         else
         {
            if(_data->yacIsStream())
            {
               // Read remaining Stream data (offset..size)
               sUI sz = _data->yacStreamGetSize();
               if(sz > 0)
               {
                  sUI off = _data->yacStreamGetOffset();
                  if(off < sz)
                  {
                     sz = sz - off;
                     tkzip_source_t *s = addSourceFromData(NULL/*dataOrNull*/, sz);
                     if(NULL != s)
                     {
                        sSI numRead = _data->yacStreamRead(s->data.u8, sz);
                        if(sUI(numRead) != sz)
                           yac_host->printf("[!!!] Zip::addFile<Stream>: numRead != sz (%d != %u)\n", numRead, sz);

                        ret = (0 <= zip_file_add(zip, (const char*)_pathName->chars, s->source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS));
                        if(ret)
                           s->b_free_source = YAC_FALSE;
                     }
                     else
                     {
                        yac_host->printf("[---] Zip::addSourceFromData<Stream>() failed (pathName=\"%s\").\n", (const char*)_pathName->chars);
                     }
                  }
               }
            } // if yacIsStream()
         }
      }
   }

   return ret;
}



// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

   // Resolve "standard" exception IDs
   Dyac_std_exid_resolve;

   YAC_Init_zip(_host);

   if(yac_host->yacGetDebugLevel() > 0)
      yac_host->printf("tkzip: init done.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_zip(_host);
}


#include <yac_host.cpp>
