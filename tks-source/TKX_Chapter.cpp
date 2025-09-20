/// TKX_Chapter.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
///
///

#include "tks.h"
#include "TKX_File.h"
#include "TKX_Chapter.h"

#ifdef DX_Z
extern "C" {
#include <zlib.h>
}
#endif


TKX_Chapter::TKX_Chapter(TKX_PakFile *_parent) {
   parent                  = _parent;
   next                    = NULL;
   chapter_files           = NULL;
   num_files               = 0u;
   total_chapter_filesize  = 0u;
   total_chapter_indexsize = 0u;
   orig_data               = NULL;
   compr_data              = NULL;
   compr_size              = 0u;
   orig_size               = 0u;
   is_loaded               = YAC_FALSE;
}

TKX_Chapter::~TKX_Chapter() {
   if(NULL != compr_data) 
   { 
      delete [] compr_data; 
      compr_data = NULL; 
   }
}

void TKX_Chapter::createCompressedChapter(void) { 
#ifdef DX_Z
   /// indextablesize+indextable+files
   sU32 total_chapter_size = 4 + total_chapter_filesize + total_chapter_indexsize;
   sSI i;
   orig_data = new(std::nothrow) unsigned char[total_chapter_size];

   if(NULL == orig_data)
   {
      parent->tak_error_code = TKX_PakFile::TKXERROUTOFMEM;
      return;
   }

   compr_size = 0;
   orig_size  = total_chapter_size;
   TKX_set32((sU32*)orig_data, total_chapter_indexsize); // chapter file table size
   sU8 *cfte = orig_data + 4; // +indextablesize

   // Load files
   sSI current_fileoffset = total_chapter_indexsize + 4;
   TKX_File *cfe = chapter_files;

   while(NULL != cfe)
   {
      // Copy logical name to file table
      sSI log_l = cfe->logic_name.length;
      cfte[8] = (sU8) log_l; // including ASCIIZ

      for(i=0; i<log_l; i++)
      {
         cfte[9+i] = cfe->logic_name.chars[i];
      }

      // copy source name to file table
      sSI src_l = cfe->source_name.length;
      cfte[9+log_l] = (sU8) src_l;

      for(i = 0; i < src_l; i++)
      {
         cfte[10+log_l+i] = cfe->source_name.chars[i];
      }
      
      if(TKX_File::CFE_TYPE_FILE == cfe->type)
      {
         // Local file (to be directly included in pak file)
         TKX_set32((sU32*)cfte, current_fileoffset);
         TKX_set32((sU32*)(cfte+4), cfe->file_size);

         // Copy file contents
         if(NULL != cfe->tokenized_source.any)
         {
            // Copy preprocessed source file (tokenized form)
            memcpy((void*)(orig_data+current_fileoffset), cfe->tokenized_source.any, cfe->file_size);
         }
         else
         {
            // Copy regular bin/ascii file
            FILE *fh = fopen((char*)cfe->source_name.chars, "rb");

            if(NULL != fh)
            {
               int r = (int)(size_t)fread((char*)(orig_data+current_fileoffset), 1, cfe->file_size, fh);
               (void)r;
               fclose(fh);
            }
            else
            {
               // Failed to read input file
               tkscript->printf("[---] failed to read input file \"%s\".\n", (char*)cfe->source_name.chars);
               parent->tak_error_code = TKX_PakFile::TKXERROPENINPUTFILE;
               delete [] orig_data;
               orig_data = NULL;
               return;
            }
         }

         //printf("[...] file header=%08x\n", *(sU32*)(orig_data+current_fileoffset));

         current_fileoffset += cfe->file_size;
      }
      else
      {
         // external file (local,http,ftp,lib..)
         TKX_set32((sU32*)cfte, cfe->type); // type is saved since v0.9.12.16 (instead of 0)
         TKX_set32((sU32*)(cfte+4), 0); // filesize
      }

      sU32 fesize = 4+4+1+1 + cfe->source_name.length + cfe->logic_name.length;

      //printf("[...] wrote file entry. size=%i source_name=\"%s\".\n",
      //    fesize, (char*)cfe->source_name.chars);

      // Align to word boundary
      if(fesize & 1)
      {
         fesize++; 
      }

      cfte += fesize;
      
      // Next file
      cfe = cfe->next;
   }

   //printf("[...] current_fileoffset=%i total_chapter_size=%i\n",
   //      current_fileoffset, total_chapter_size);

   // estimate compressed size and zip data
   compr_size = (int)(total_chapter_size*0.12) + total_chapter_size + 120;
   compr_data = new(std::nothrow) unsigned char[compr_size];

   if(NULL == compr_data)
   {
      parent->tak_error_code = TKX_PakFile::TKXERROUTOFMEM;
      delete [] orig_data;
      orig_data = NULL;
      return;
   }
   
   if(::compress2((unsigned char*)compr_data, &compr_size, (unsigned char*)orig_data, orig_size, 9)!=Z_OK)
   {
      parent->tak_error_code = TKX_PakFile::TKXERRCOMPRESS;
      delete [] orig_data;
      orig_data = NULL;
      delete [] compr_data;
      compr_data = NULL;
      return;
   }
   
   //printf("[...] chapter orig=%i compr=%i ratio=%f header=%08x\n",
   //    orig_size, compr_size, ((sF32)compr_size)/((sF32)orig_size),
   //    *(sU32*)compr_data);
   
   delete [] orig_data;
   orig_data = NULL;
#else 
   // [...] z-lib support not compiled in; disabling tkx writer. 
#endif
}

void TKX_Chapter::loadCompressedChapter(void) { 
   // yac_host->printf("xxx TKX_Chapter::loadCompressedChapter: compr_data=%p\n", compr_data);
#ifdef DX_Z
   if(NULL == compr_data)
   {
      return;
   }

   compr_size = TKX_get32((sU32*)(&compr_data[0]));
   orig_size  = TKX_get32((sU32*)(&compr_data[4]));

   //printf("[...] compr_size=%i orig_size=%i header=%08x\n", compr_size, orig_size, *(sU32*)&compr_data[8]);

   // Allocate memory for uncompressed data
   orig_data = new(std::nothrow) unsigned char[orig_size];

   if(NULL == orig_data)
   { 
#ifdef TKS_DCON
      ::printf("[---] loadCompressedChapter: failed to allocate %u bytes.\n", orig_size); 
#endif
      return;
   }

   // Unpack data
   unsigned long dec_orig_size = orig_size;
   int uncret = uncompress((unsigned char*)orig_data, &dec_orig_size, (unsigned char*)(compr_data+8), compr_size);

   if(uncret != Z_OK)
   {
#ifdef TKS_DCON
      ::printf("[---] error decompressing chapter ERRCODE=%i dec_orig_size=%u orig_size=%u\n", 
         uncret,
         (sUI)dec_orig_size,
         orig_size);
#endif
      return;
   }

   if(dec_orig_size != orig_size)
   {
#ifdef TKS_DCON
      printf("[...] internal error: dec_orig_size(%u) and orig_size(%u) differ.\n",
         (sUI)dec_orig_size, orig_size);
#endif
   }

#ifdef TKS_DCON
   //tkscript->printf("[...] chapter uncompressed.\n");
#endif

   compr_data = NULL;

   /// scan chapter file table
   sU16 fesize;
   sU16 ftcsize = 0u;
   sU16 ftsize = (sU16) TKX_get32((sU32*)(orig_data));
   sU8 *ftp = orig_data+4;
   TKX_File *lf = NULL;
   num_files = 0u;

   while(ftcsize < ftsize)
   {
      num_files++;

      // Allocate new file entry and add to SLL
      TKX_File *tf = new(std::nothrow) TKX_File();

      if(NULL != lf)
      {
         lf->next = tf;
         lf = tf;
      }
      else
      {
         chapter_files = tf;
         lf = tf;
      }

      tf->file_offset = TKX_get32((sU32*)(ftp + 0));
      tf->file_size   = TKX_get32((sU32*)(ftp + 4));
      tf->file_data   = orig_data + tf->file_offset;

      sS16 namelength =*(ftp + 8);
      tf->logic_name.alloc(namelength);
      sU8 *d = ftp + 9;
      sU8 ni = 0;

      for(; ni<namelength; ni++)
      {
         tf->logic_name.chars[ni]=*d++;
      }

      tf->logic_name.length = namelength;
      
      namelength = *d++;
      tf->source_name.alloc(namelength);

      for(ni=0; ni<namelength; ni++)
      {
         tf->source_name.chars[ni] = *d++;
      }

      tf->source_name.length = namelength;
      fesize = ((sU16)(d - ftp));

      // Align to word boundary
      if(fesize & 1)
      {
         fesize++;
      }

      //printf("[...] found file. entrysize=%i logic_name=\"%s\" source_name=\"%s\" offset=%i size=%i.\n",
      //    fesize,
      //    tf->logical_name.chars,
      //    tf->source_name.chars,
      //    tf->file_offset,
      //    tf->file_size);

      //printf("[...] file header=%08x\n",
      //    *(sU32*)(orig_data+tf->file_offset));
      
      ftp += fesize;
      ftcsize = (sU16)(ftcsize + fesize);
   }

   is_loaded = YAC_TRUE;

#else 
   tkscript->printf("[...] z-lib support not compiled in; unable to load compressed chapter from TKX file.\n");
#endif
}

TKX_File *TKX_Chapter::findFileByLogicName(YAC_String *_logicName) const {
   TKX_File *c = chapter_files;
   while(NULL != c)
   {
      if(c->logic_name.compare(_logicName))
         return c;
      c = c->next;
   }
   return NULL;
}

sBool TKX_Chapter::mergeTSL(TKX_File *f, sBool _bTSL) {
   //
   // 'f' is a .tsl file node in the application project
   //
   //  - load and scan the .tsl pakfile
   //  - merge .tsl content (file nodes) with the application project
   //     - prefix logic .tks source file names with library name so there
   //        is no name clash between application source file names / other
   //        library file names
   //        (note) the application must still ensure that the names of other files
   //                (e.g. images, bin files, ..) are unique within a project
   //
   // yac_host->printf("xxx TKX_Chapter::mergeTSL: ENTER\n");
   TKX_PakFile *pf = new(std::nothrow) TKX_PakFile();

   if( (f->file_offset < TKX_File::CFE_NUMTYPES) || (NULL == f->file_data) )
   {
      // yac_host->printf("xxx TKX_Chapter::mergeTSL: 2  f->source_name=\"%s\"\n", f->source_name.chars);

      if(f->source_name.startsWith("lib:"))
      {
         // yac_host->printf("xxx TKX_Chapter::mergeTSL: 3\n");

         // Load tokenized script library
         YAC_String sEmpty;
         YAC_String t; 
         sBool bLibOpened;
         
         // Try CWD
         if(1)
         {
            t.yacCopy(&f->source_name);
            t.replaceRegion(0, 4, &sEmpty);
            bLibOpened = pf->str_takfile.readRawFile(&t);
         }

         // Try CWD/libraries/
         if(!bLibOpened)
         {
            t.yacCopy(&f->source_name);
            t.replaceRegion(0, 4, &sEmpty);
            t.insert(0, "libraries/");
            bLibOpened = pf->str_takfile.readRawFile(&t);
         }

         // Try STARTDIR
         if(!bLibOpened)
         {
            t.yacCopy(&f->source_name);
            t.replaceRegion(0, 4, &sEmpty);
            t.insert(0, &tkscript->configuration.tks_exe_dir);
            bLibOpened = pf->str_takfile.readRawFile(&t);
         }

         // Try STARTDIR/libraries/
         if(!bLibOpened)
         {
            t.yacCopy(&f->source_name);
            t.replaceRegion(0, 4, &sEmpty);
            t.insert(0, "libraries/");
            t.insert(0, &tkscript->configuration.tks_exe_dir);
            bLibOpened = pf->str_takfile.readRawFile(&t);
         }

         // Try library repository
         if(!bLibOpened)
         {
            t.yacCopy(&f->source_name);
            t.replaceRegion(0, 4, &tkscript->configuration.library_path);

#ifdef HAVE_CYGWIN
            if('/' == t[0])
            {
               t.replace("/usr/lib", &tkscript->configuration.cygwin_usrlib);
            }
#endif
            bLibOpened = pf->str_takfile.readRawFile(&t);
         }

         // All failed ?
         if(!bLibOpened)
         {
            Dprintf("[---] unable to load script library \"%s\".\n", (char*)t.chars);
            delete pf;

            return YAC_FALSE;
         }

         if(tkscript->configuration.debug_level > 0)
         {
            Dprintf("[...] loaded script library \"%s\".\n", (char*)t.chars);
         }
      }
      else
      {
         pf->str_takfile.readRawFile(&f->source_name);
      }
   }
   else
   {
      pf->str_takfile.setRawBuffer(f->file_data, f->file_size);
   }

   if(!pf->scanTKX())
   {
      Dprintf("[---] failed to scan TSL file logicName=\"%s\" localName=\"%s\".\n", 
              (char*)f->logic_name.chars,
              (char*)f->source_name.chars
              );

      delete pf;

      return YAC_FALSE;
   }

   // calls TKX_Chapter::loadCompressedChapter()
   pf->continueChapterLoading();
   
   // Merge files into this chapter
   {
      pf->selectFirstChapter();
      TKX_File *f2 = pf->current_chapter->chapter_files;
      
      if(NULL != f2)
      {
         TKX_File *c = f2;

         // Prefix with library name
         do
         {
            // Add unique files
            //  (i.e. skip common/shared files if they've already been added by another lib)
            // yac_host->printf("xxx TKX_Chapter::mergeTSL: bTSL=%d merge file logic_name=\"%s\" into chapter=%p\n", _bTSL, c->logic_name.chars, this);
            if(!_bTSL || NULL == findFileByLogicName(&c->logic_name))
            {
               if(c->logic_name.endsWith(".tks"))
               {
                  c->logic_name.insert(0, ":");
                  c->logic_name.insert(0, &f->logic_name);
                  c->logic_name.replace("lib:", "");
               }
            }
            else
            {
               // Already exists, remove from (temp) chapter
               // yac_host->printf("xxx TKX_Chapter::mergeTSL: remove duplicate file logic_name=\"%s\"\n", c->logic_name.chars);
               c->logic_name.empty();
               c->source_name.empty();
               c->type = TKX_File::CFE_TYPE_UNKNOWN;
            }

            if(NULL != c->next)
            {
               c = c->next;
            }
         }
         while(NULL != c->next);
         
         // Merge with this file list
         c->next = f->next;
      }
      
      f->next = f2;
 
      pf->current_chapter->chapter_files = NULL;
   }

   if(0 != pf->tak_error_code)
   {
      Dprintf("[---] failed to merge TSL file logicName=\"%s\" localName=\"%s\" (errcode=%i).\n", 
              (char*)f->logic_name.chars,
              (char*)f->source_name.chars,
              pf->tak_error_code
              );

      delete pf;

      return YAC_FALSE;
   }

   delete pf;

   // yac_host->printf("xxx TKX_Chapter::mergeTSL: LEAVE (ok)\n");

   return YAC_TRUE;
}

#ifndef YAC_LITTLE_ENDIAN
sUI TKX_get32(sU32*_s) {
   sU32 r;
   sU8 *s = (sU8*)_s;
   sU8 *d = (sU8*)&r;
   d[0] = s[3];
   d[1] = s[2];
   d[2] = s[1];
   d[3] = s[0];
   return r;
}
void TKX_set32(sU32*_d, sU32 _b) {
   sU8 *s = (sU8*)&_b;
   sU8 *d = (sU8*)_d;
   d[0] = s[3];
   d[1] = s[2];
   d[2] = s[1];
   d[3] = s[0];
}
#endif
