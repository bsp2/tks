/// TKX_PakFile.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

//
// (note) TKX files are always stored in LITTLE_ENDIAN format. (auto-byteswap'd on big endian systems)
//
// (note) pak files may contain multiple chapters but usually it's just one
//         (originally (back in 2001) tks was a browser plugin which raised the need for a "loader" chapter
//           which was run as soon as the data was available / while the "main" chapter data was still downloading)
//

extern "C" {
#include <stdio.h>
}

#include "tks.h"
#include "TKX_File.h"
#include "TKX_Chapter.h"
#include "TKX_PakFile.h"

#include "TKS_StringIterator.h"

#include "tks_inc_class.h"
#include "tks_inc_compiler.h"



TKX_PakFile::TKX_PakFile(void) {
   chapters        = NULL;
   current_chapter = NULL;
   current_file    = NULL;
   pak_size        = 0;
   tak_error_code  = 0;
}

TKX_PakFile::~TKX_PakFile(void) {
   cleanUp();
}

void TKX_PakFile::addChapter(void) {

   TKX_Chapter *cie = new TKX_Chapter(this);

#ifdef TKS_DCON
   //tkscript->printf("[...] adding chapter \"%s\"\n", (char*)str_chaptername.chars);
#endif

   if(NULL != cie)
   {
      cie->chapter_name.yacCopy(&str_chaptername);
      cie->chapter_files = NULL;
      current_file = NULL;

      if(NULL != current_chapter)
      {
         current_chapter->next = cie;
         current_chapter = cie;
      }
      else
      {
         chapters = current_chapter = cie;
      }
   }
}

sBool TKX_PakFile::prepareTKXDownload(sU8 *_s) {
   if(('T' == _s[0]) &&
      ('P' == _s[1]) &&
      ('A' == _s[2]) &&
      ('K' == _s[3])
      )
   {
      pak_size = TKX_get32((sU32*)(&_s[4]));

#ifdef TKS_DCON
      //tkscript->printf("[...] prepareTakDownload paksize=%i.\n",
      //    pak_size);
#endif

      if(str_takfile.alloc(pak_size))
      {
         str_takfile.length = pak_size;
         return YAC_TRUE;
      }
      else
      {
         tak_error_code = TKXERROUTOFMEM;
      }
   }

   tak_error_code = TKXERRINVALIDHEADER;

   return YAC_FALSE;
}

void TKX_PakFile::addExtraFile(const char *_name, sBool _bPre) {
   str_sourcefilename.copy(_name);

   if(!str_sourcefilename.isBlank())
   {
      str_logfilename.yacCopy(&str_sourcefilename);
      addFile(_bPre);
   }
}

void TKX_PakFile::addFile(sBool _bPre) {

   // printf("xxx TKX_PakFile::addFile: str_sourcefilename=\"%s\"\n", str_sourcefilename.chars);

   if(NULL != current_chapter)
   {
      TKX_File *cfe = new(std::nothrow) TKX_File();
      if(NULL != cfe)
      {
         sBool bSetCurrentFile = YAC_TRUE;

         if(_bPre)
         {
            // e.g. when preprending lib file via "-efp" option
            //  (=> run global module statements before main file enters a UI event loop outside of main())
            TKX_File *oldHead = current_chapter->chapter_files;
            current_chapter->chapter_files = cfe;
            cfe->next = oldHead;
            bSetCurrentFile = YAC_FALSE;  // keep current 'tail' file
         }
         else if(NULL != current_file)
         {
            current_file->next = cfe;
         }
         else
         {
            if(NULL == current_chapter->chapter_files)
            {
               // First file
               current_chapter->chapter_files = cfe;
            }
            else
            {
               // Adding extrafile to .tkx (not .tkp! this would take the if(current_file) codepath)
               TKX_File *c = current_chapter->chapter_files;

               while(NULL != c->next)
                  c = c->next;

               c->next = cfe; // Append to end of file list
               bSetCurrentFile = YAC_FALSE;
            }
         }

         sBool is_mod   = YAC_FALSE;
         sBool is_local = YAC_FALSE;
         sBool is_local_home = YAC_FALSE;

         if(bSetCurrentFile)
         {
            current_file = cfe;
         }

         cfe->logic_name.yacCopy(&str_logfilename);

         if(str_sourcefilename.startsWith("http:"))
         {
            cfe->type = TKX_File::CFE_TYPE_HTTP;
         }
         else if(str_sourcefilename.startsWith("https:"))
         {
            cfe->type = TKX_File::CFE_TYPE_HTTPS;
         }
         else if(str_sourcefilename.startsWith("ftp:"))
         {
            cfe->type = TKX_File::CFE_TYPE_FTP;
         }
         else if(str_sourcefilename.startsWith("sftp:"))
         {
            cfe->type = TKX_File::CFE_TYPE_SFTP;
         }
         else if(str_sourcefilename.startsWith("mod:"))
         {   // ---- re-usable tks script module ----
            //cfe->type = CFE_TYPE_FILE;
            cfe->type = TKX_File::CFE_TYPE_LOCAL;
            is_mod = YAC_TRUE;
         }
         else if(str_sourcefilename.startsWith("local:"))
         {   // ---- local file, never embed ----
            cfe->type = TKX_File::CFE_TYPE_LOCAL;
            is_local = YAC_TRUE;
         }
         else if(str_sourcefilename.startsWith("home:"))
         {   // ---- local file in home directory, never embed ----
            cfe->type = TKX_File::CFE_TYPE_LOCAL_HOME;
            is_local_home = YAC_TRUE;
         }
         else if(str_sourcefilename.startsWith("lib:"))
         {
            // ---- dynamically loaded script library (TKS_LIBRARY_PATH)
            cfe->type = TKX_File::CFE_TYPE_LIB;
         }
         else
         {
            // ---- test if local file exists (to avoid runtime errors) ----
            cfe->type = TKX_File::CFE_TYPE_FILE;
         }

         cfe->source_name.yacCopy(&str_sourcefilename);

         if(is_local)
         {
            // local .tks source (e.g. for config files)
            cfe->source_name.replace("local:", "");
            cfe->logic_name.replace("local:", "");
         }
         else if(is_local_home)
         {
            // .tks source in home directory (e.g. for config files)
            cfe->source_name.replace("home:", "");
            cfe->logic_name.replace("home:", "");
         }

         if(is_mod)
         {
            // ---- search module in current working directory ----
            cfe->source_name.yacCopy(&str_sourcefilename);
            cfe->source_name.replace("mod:","");
         }
         else
         {
            // Build absolute pathname (so that the application can still change the CWD)
            if(!(
                  cfe->source_name.startsWith("/")         ||
                  cfe->source_name.startsWith("\\")        ||
                  (-1 != cfe->source_name.indexOf(':', 0)) ||
                  (cfe->source_name.startsWith("..")
                   )
                 )
               )
            {
               cfe->source_name.insert(0, &tkscript->configuration.tkp_dir);
               ////printf("xxx TKX_PakFile: absolute cfe->source_name=\"%s\" tkp_dir=\"%s\".\n", cfe->source_name.chars, tkscript->configuration.tkp_dir.chars);
            }

         }

         if(TKX_File::CFE_TYPE_FILE == cfe->type)
         {
            // ---- determine file size ----
            FILE *fh;
pakfile_addfile_reopen:

            if(tkscript->configuration.b_rewrite_scripts && cfe->source_name.endsWith(".tks"))
            {
               // Input file is tkscript, Store tokenized form.
               YAC_String buf;
               sBool bSrcOk = buf._loadLocal(&cfe->source_name, 1);

               if(bSrcOk)
               {
                  TKS_TokenizedScript *compiled_script = new TKS_TokenizedScript();
                  sUI num_tokens = compiled_script->scan(&buf, 1, 0 /*, default params*/);

                  if(num_tokens >= ERRTKS)
                  {
                     bSrcOk = YAC_FALSE;
                  }
                  else
                  {
                     compiled_script->optimize();
                     cfe->file_size = compiled_script->calcSize(tkscript->configuration.b_tkx_linenumbers);
                     cfe->tokenized_source.u8 = new sU8[cfe->file_size];

                     if(NULL == cfe->tokenized_source.any)
                     {
                        Dprintf("[---] TKX_Pakfile::addFile: unable to alloc tokenized data (file=\"%s\", %i bytes).\n",
                                (char*) cfe->source_name.chars, cfe->file_size);
                        tak_error_code = TKXERROPENFILE;
                        return;
                     }

                     compiled_script->writeTokens(cfe->tokenized_source);
                  }

                  delete compiled_script;
                  compiled_script = NULL;
               }

               if(!bSrcOk)
               {
                  Dprintf("[---] TKX_Pakfile::addFile: unable to open source file \"%s\".\n",
                          (char*) cfe->source_name.chars);
                  tak_error_code = TKXERROPENFILE;
                  return;
               }
            }
            else
            {
               // Regular file
               fh = fopen((char*)cfe->source_name.chars, "rb");

               if(NULL == fh)
               {
                  if(is_mod)
                  {
                     // ---- search module in archive directory ----
                     cfe->source_name.insert(0, &tkscript->configuration.module_path);
#ifdef HAVE_CYGWIN
                     if('/' == cfe->source_name[0])
                     {
                        cfe->source_name.replace("/usr/lib", &tkscript->configuration.cygwin_usrlib);
                     }
#endif
                     is_mod = YAC_FALSE;
                     goto pakfile_addfile_reopen;
                  }

                  if(!str_sourcefilename.compare("data.tks"))
                  {
                     Dprintf("\n[---] PakFile: cannot open file \"%s\".\n", (char*)str_sourcefilename.chars);
                  }
                  tak_error_code = TKXERROPENFILE;
                  return;
               }
               else
               {
                  // Regular file
                  fseek(fh, 0, SEEK_END);
                  cfe->file_size = ftell(fh);
                  fclose(fh);

                  // Prefix logic name with dataprefix (if set)
                  if(!cfe->logic_name.endsWith(".tks") &&
                     !cfe->logic_name.endsWith(".tsl")
                     )
                  {
                     if(str_dataprefix.length > 1)
                     {
                        cfe->logic_name.insert(0, &str_dataprefix);
                     }
                  }
               }
            }

            current_chapter->total_chapter_filesize += cfe->file_size;
         }

#ifdef TKS_DCON
         /*
         tkscript->printf("[...] adding file (type=%i, size=%i) \"%s\" from \"%s\"\n",
         cfe->type,
         cfe->file_size,
         (char*)str_logfilename.chars,
         (char*)str_sourcefilename.chars);
         */
#endif

         current_chapter->num_files++;
         ////sU32 fesize = (4 + 4 + 1 + 1) + (str_sourcefilename.length + str_logfilename.length);
         sU32 fesize = (4 + 4 + 1 + 1) + (str_sourcefilename.length + cfe->logic_name.length);

         // Align to word boundary
         if(fesize & 1)
         {
            fesize++;
         }

         current_chapter->total_chapter_indexsize += fesize;
      }
   }
}

void TKX_PakFile::cleanUp(void) {
   if(NULL != chapters)
   {
      TKX_Chapter *cie = chapters;

      while(NULL != cie)
      {
         TKX_Chapter *nie = cie->next;
         TKX_File *cfe = cie->chapter_files;

         while(NULL != cfe)
         {
            TKX_File *nfe = cfe->next;
            delete cfe;
            cfe = nfe;
         }

         delete cie;
         cie = nie;
      }

      current_chapter = NULL;
      chapters        = NULL;
      current_file    = NULL;
   }
}

void TKX_PakFile::writeTKX(void) {

   FILE *fh = fopen((char*)str_takfilename.chars, "wb");

   if(NULL == fh)
   {
      Dprintf("\n[---] unable to create output file \"%s\"\n",
              (char*)str_takfilename.chars);
      tak_error_code = TKXERRCREATEFILE;
      return;
   }

   // Compress chapters
   TKX_Chapter *cie = chapters;
   sU32 l_head = (4 + 4 + 4);
   sU32 pak_size = 0;

   while(NULL != cie)
   {
      cie->createCompressedChapter();

      if(0 != tak_error_code)
      {
         Dprintf("[---] createCompressedChapter() failed, output file=\"%s\"\n",
                 (char*)str_takfilename.chars
                 );
         fclose(fh);
         return;
      }

      pak_size += cie->compr_size + (4 + 4);
      sU16 l_name = (sU16) (cie->chapter_name.length + 1);

      // Align name length to word size
      if(l_name & 1)
      {
         l_name++;
      }

      l_head += 4 + l_name;

      // Next chapter
      cie = cie->next;
   }

   // Write pak file header
   pak_size += l_head;
   tkx_header[0] = 'T';
   tkx_header[1] = 'P';
   tkx_header[2] = 'A';
   tkx_header[3] = 'K';
   TKX_set32((sU32*)(tkx_header+4), pak_size);
   Dprintf("[...] TKX_PakFile::writeTak: total_pak_size=%i\n", pak_size);

   // gen index table
   cie = chapters;
   char *cta = tkx_header + 8;
   TKX_set32((sU32*)cta, l_head-8); // store chapter index table size
   //printf("[...] chapter index table size=%i\n", l_head-8);
   cta += 4;
   sU32 choffset = l_head;

   while(NULL != cie)
   {
      // Store chapter offset
      TKX_set32((sU32*)cta, choffset);
      choffset += cie->compr_size + (4 + 4);
      cta += 4;

      // Store chapter name
      sSI i;

      for(i=0; i<((sSI)cie->chapter_name.length); i++)
      {
         cta[i] = (char)cie->chapter_name.chars[i];
      }

      cta[i++] = 0;

      // Align to word boundary
      if(i & 1)
      {
         i++;
      }

      cta += i;

      // Next chapter
      cie = cie->next;
   }

   // Write compressed chapters
   cie = chapters;
   fwrite((const char*)tkx_header, l_head, 1, fh);

   struct _chapter_header {
      sU32 compr_size;
      sU32 orig_size;
   } chapter_header;

   while(NULL != cie)
   {
      TKX_set32((sU32*)&chapter_header.compr_size, cie->compr_size);
      TKX_set32((sU32*)&chapter_header.orig_size, cie->orig_size);
      fwrite((const char*)&chapter_header, 8, 1, fh);
      fwrite((const char*)cie->compr_data, cie->compr_size, 1, fh);
      cie = cie->next;
   }

   fclose(fh);
}

sBool TKX_PakFile::scanTKX(void) {
   char buf[1024];      // for chapter name
   sU8 *s = str_takfile.chars;

   if(('T' == s[0]) &&
      ('P' == s[1]) &&
      ('A' == s[2]) &&
      ('K' == s[3])
      )
   {
      sU32 pak_size = TKX_get32((sU32*)(&s[4]));
      sSI  toc_size = TKX_get32((sU32*)(&s[8]));

      /// ---- parse table of contents ----
      sU8 *tp = &s[12];
      sSI  ti = 0;
      sU8 chapter_index = 0;
      (void)chapter_index;
      toc_size -= 4;
      sU32 last_chp_offset = 0;
      TKX_Chapter *last_tc = NULL;

      while(ti < toc_size)
      {
         sU32 chp_offset = TKX_get32((sU32*)(&tp[ti]));
         //printf("[...] chapter[%i] offset=%i\n", chapter_index, chp_offset);

         // Read chapter name
         {
            char *chpname = (char*)&tp[ti + 4];
            sU16 chpnamelength = 0;

            while(*chpname)
            {
               buf[chpnamelength++] = *chpname++;

               if(chpnamelength >= 1024)
               {
                  // chapter name buffer overflow
                  return YAC_FALSE;
               }
            }

            buf[chpnamelength++] = 0;

            // Align to word boundary
            if(chpnamelength & 1)
            {
               chpnamelength++;
            }

            // Add chapter
            str_chaptername.copy(buf);
            addChapter();
            current_chapter->compr_data = &str_takfile.chars[chp_offset];

            if(NULL != last_tc)
            {
               last_tc->compr_size = (chp_offset - last_chp_offset);
               last_chp_offset = chp_offset;
               last_tc = current_chapter;
            }
            else
            {
               last_chp_offset = chp_offset;
               last_tc = current_chapter;
            }

            ti += chpnamelength + 4;
            chapter_index++;
         }

         if(NULL != last_tc)
         {
            last_tc->compr_size = (pak_size - last_chp_offset);
         }
      }
      // toc read, now decompress chapters
   }

   return YAC_TRUE;
}

void TKX_PakFile::continueChapterLoading(void) {
   TKX_Chapter *cc = chapters;

   while(NULL != cc)
   {
      if(!cc->is_loaded)
      {
         if(str_takfile.length >= (((sU32)(cc->compr_data - str_takfile.chars)) + cc->compr_size))
         {
            cc->loadCompressedChapter();
         }
      }

      // Next chapter
      cc = cc->next;
   }
}

enum __states {
   ST_UNKNOWN = 0,
   ST_PROJECT,
   ST_AUTHORS,
   ST_CHAPTER,

   ST_PAD
};

void TKX_PakFile::scanProject(void) {
   str_prjname.visit("unknown");

   // Process `/some/file` include directives
   if(-1 == str_tkpfile._replaceIncludes(NULL))
   {
      Dprintf("[---] failed to process project file include directives.\n");
      tak_error_code = TKXERROPENINCLUDEFILE;
      return;
   }

   str_tkpfile.split('\n');
   TKS_StringIterator is;
   is.begin(&str_tkpfile);
   sBool bGlobalChapter = YAC_TRUE;

   str_chaptername.visit("main");
   addChapter();

   sSI state = ST_CHAPTER;

   if(is.tail())
   {
      do
      {
         YAC_String sv;
         YAC_String *t = is.current;

         if('[' == (*t)[0])
         {
            state = ST_UNKNOWN;
         }

         if('#' != (*t)[0]) // Not a comment line ?
         {
            switch(state)
            {
            case ST_UNKNOWN:
               if(t->startsWith("[project]"))
               {
                  state = ST_PROJECT;
               }
               else if(t->startsWith("[authors]"))
               {
                  state = ST_AUTHORS;
               }
               else if(t->startsWith("[chapter]"))
               {
                  state = ST_CHAPTER;
               }
               break;

            case ST_PROJECT:
               if(t->startsWith("name"))
               {
                  sSI di = t->indexOf("=", 0);

                  if(-1 != di)
                  {
                     // ---- get project name ----
                     t->substring(&sv, (di + 1), (t->length - di - 1));
                     sv.replaceChar('\"',0);
                     str_prjname.trim(&sv);
                     ////printf("xxx str_prjname=\"%s\".\n", str_prjname.chars);
                  }
               }
               else if(t->startsWith("created"))
               {
                  sSI di = t->indexOf("=", 0);

                  if(-1 != di)
                  {
                     // get project creation date
                     t->substring(&sv, (di + 1), (t->length - di - 1));
                     str_prjdate.trim(&sv);
                     str_prjdate.substring(&sv, 1, (str_prjdate.length - 3));
                     str_prjdate.yacCopy(&sv);
                  }
               }
               else if(t->startsWith("version"))
               {
                  sSI di = t->indexOf("=", 0);

                  if(-1 != di)
                  {
                     // ---- get project version ----
                     t->substring(&sv, (di + 1), (t->length - di - 1));
                     str_prjversion.trim(&sv);
                     str_prjversion.substring(&sv, 1, (str_prjversion.length - 3));
                     str_prjversion.yacCopy(&sv);
                  }
               }
               else if(t->startsWith("dataprefix"))
               {
                  sSI di = t->indexOf("=", 0);

                  if(-1 != di)
                  {
                     // ---- get non-tks logic file name prefix ----
                     t->substring(&sv, (di + 1), (t->length - di - 1));
                     str_dataprefix.trim(&sv);
                     str_dataprefix.substring(&sv, 1, (str_dataprefix.length - 3));
                     str_dataprefix.yacCopy(&sv);
                  }
               }
               break;

            case ST_AUTHORS:
               break;

            case ST_CHAPTER:
               if(t->startsWith("name"))
               {
                  if(bGlobalChapter)
                  {
                     // Use first chapter as global chapter
                     bGlobalChapter = YAC_FALSE;
                  }
                  else
                  {
                     // Begin next chapter
                     sSI di = t->indexOf("=");

                     if(-1 != di)
                     {
                        t->substring(&sv, (di + 1), (t->length - di - 1));
                        str_chaptername.trim(&sv);
                        str_chaptername.substring(&sv, 1, (str_chaptername.length - 3));
                        str_chaptername.yacCopy(&sv);
                        addChapter();
                     }
                  }
               }
               else
               {
                  // Add new file
                  sSI di = t->indexOf("=");

                  if(-1 != di)
                  {
                     YAC_String s, s2;

                     // Parse logic filename
                     t->substring(&s2, 0, di);
                     s.trim(&s2);

                     if('\"' == s[0])
                     {
                        sSI lqi = s.lastIndexOf('\"', 1);

                        if(-1 == lqi)
                        {
                           tak_error_code = TKXERRPARSELOGICNAME;
                           ////continue;
                           return;
                        }
                        else
                        {
                           s.substring(&str_logfilename, 1, (s.length - 3));
                        }
                     }
                     else
                     {
                        str_logfilename.yacCopy(&s);
                     }

                     // Parse local filename
                     t->substring(&s2, (di + 1), (t->length - (di + 1)));
                     s.trim(&s2);

                     if('\"' == s[0])
                     {
                        sSI lqi = s.lastIndexOf('\"', 1);

                        if(-1 == lqi)
                        {
                           tak_error_code = TKXERRPARSELOCALNAME;
                           ////continue;
                           return;
                        }
                        else
                        {
                           s.substring(&str_sourcefilename, 1, (s.length - 3));
                        }
                     }
                     else
                     {
                        str_sourcefilename.yacCopy(&s);
                     }

                     addFile();

                     if(0 != tak_error_code)
                     {
                        return;
                     }
                  }
                  else
                  {
                     // Logic filename == local filename
                     str_sourcefilename.trim(t);

                     if(!str_sourcefilename.isBlank())
                     {
                        str_logfilename.yacCopy(&str_sourcefilename);
                        addFile();

                        if(0 != tak_error_code)
                        {
                           return;
                        }
                     }
                  }
               }
               break;

            default:
               break;
            }
         } // if not #comment
      }
      while(is.previous() && (0 == tak_error_code));
   }

   str_tkpfile.freeStack();
}

void TKX_PakFile::selectFirstChapter(void) {
   if(NULL != chapters)
   {
      current_chapter = chapters;
   }
}

void TKX_PakFile::selectChapterByName(YAC_String *_name) {
   TKX_Chapter *c = chapters;

   while(NULL != c)
   {
      if(c->chapter_name.compare(_name))
      {
         // Found chapter
         current_chapter = c;
         return;
      }
      else
      {
         c = c->next;
      }
   }
}

TKX_File *TKX_PakFile::findLogic(char *_name) {
   YAC_String s;
   s.visit(_name);
   return findLogic(&s);
}

TKX_File *TKX_PakFile::findLogic(YAC_String *_name) {

   // printf("xxx ENTER TKX_PakFile::findLogic name=%08x current_chapter=%08x\n", _name, current_chapter);
   // printf("xxx ENTER TKX_PakFile::findLogic name=%08x\n", _name);

   if(
      (NULL == _name)           ||
      (NULL == _name->chars)    ||
      (NULL == current_chapter)
      )
   {
      return NULL;
   }

   // printf("xxx ENTER TKX_PakFile::findLogic name=\"%s\" current_chapter=%08x\n", _name->chars, current_chapter);

   TKX_File *cf = current_chapter->chapter_files;

   while(NULL != cf)
   {
      if(cf->logic_name.compare(_name))
      {
         // Found file
         return cf;
      }

      cf = cf->next;
   }

   return NULL;
}

void TKX_PakFile::listContent(void) {
#ifdef TKS_DCON
   TKX_Chapter *cc = chapters;
   Dprintf("[...] tak size=%i.\n", str_takfile.length);
   Dprintf("[...] pak TOC:\n");
   sU8 cci = 1;

   while(NULL != cc)
   {
      Dprintf("\n[...] %02i: chapter name=\"%s\" #files=%i origsize=%i comprsize=%i ratio=%3.2f.\n",
              cci,
              (char*)cc->chapter_name.chars,
              cc->num_files,
              cc->orig_size,
              cc->compr_size,
              ( ((sF32)cc->compr_size) / ((sF32)cc->orig_size)*100.0f )
              );
      Dprintf("[...] %02i: chapter TOC:\n", cci);

      sU16 cfi = 1;
      TKX_File *cf = cc->chapter_files;

      while(NULL != cf)
      {
         Dprintf("[...] %02i:%02i: logic=\"%s\" local=\"%s\" off=%u size=%u type=%u.\n",
                 cci,
                 cfi,
                 (char*)cf->logic_name.chars,
                 (char*)cf->source_name.chars,
                 (sUI)cf->file_offset,
                 (sUI)cf->file_size,
                 (sUI)cf->type
                 );

         // Next file
         cfi++;
         cf = cf->next;
      }

      // Next chapter
      cci++;
      cc = cc->next;
   }
#endif // TKS_DCON
}

sBool TKX_PakFile::mergeLocalTSL(YAC_String *_libName) {
   // printf("xxx mergeLocalTSL libName=\"%s\"\n", (const char*)_libName->chars);
   sBool r;
   TKX_File *f = new(std::nothrow) TKX_File;
   r = (NULL != f);
   if(r)
   {
      r = f->initLibFile((const char*)_libName->chars);
      if(r)
      {
         // Append file to SLL
         TKX_File *p = current_chapter->chapter_files;
         while(p->next)
            p = p->next;
         p->next = f;
         current_chapter->num_files++;

         // yac_host->printf("xxx mergeLocalTSL: call mergeTSL  current_chapter=%p f->next=%p\n", current_chapter, f->next);
         // (note) appends files from pakfile to f->next
         r = current_chapter->mergeTSL(f, YAC_TRUE/*bTSL*/);
         // yac_host->printf("xxx mergeLocalTSL: LEAVE f->next=%p\n", f->next);
      }
   }
   return r;
}
