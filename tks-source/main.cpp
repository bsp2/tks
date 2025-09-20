/// main.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

/// Just for testing/debugging purposes
//#define TKS_PRINT_SIZEOFS defined


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "tks.h"

#ifdef YAC_WIN32
#include <windows.h>
#endif // YAC_WIN32

#ifdef YAC_POSIX
#include <unistd.h>
#endif // YAC_POSIX

#ifdef DX_STATIC_TKSDL
#include <SDL/SDL.h>
#endif

#include "YAC_ValueObject.h"
#include "YAC_Event.h"
#include "YAC_IntArray.h"
#include "YAC_FloatArray.h"
#include "YAC_StringArray.h"
#include "YAC_ListNode.h"
#include "YAC_TreeNode.h"
#include "Pool.h"
#include "ObjectPool.h"

#include "YAC_StreamBase.h"
#include "YAC_StdInStream.h"
#include "YAC_StdOutStream.h"
#include "YAC_StdErrStream.h"
#include "YAC_Buffer.h"

#include "TKS_StreamIO.h"
#include "YAC_PakFile.h"

#include "tks_inc_class.h"

#include "TKX_File.h"
#include "TKX_Chapter.h"
#include "TKX_PakFile.h"

#include "TKS_Token.h"
#include "TKS_TokenTable.h"
#include "TKS_TokenizedScript.h"

#include "PTN_Node.h"
#include "PTAnyNodePtr.h"
#include "TKS_StateStackEntry.h"
#include "TKS_CachedScript.h"
#include "TKS_Compiler.h"
#include "TKS_XMLParser.h"

#include "tks_debug.h"

#include "PTN_Statement.h"

#ifdef TKS_PRINT_SIZEOFS
#include "PTN_Expr.h"
#include "PTN_DoubleArgExpr.h"
#endif // TKS_PRINT_SIZEOFS


static sBool b_interactive;
static sBool b_continue;
static sBool b_forcenorewritescripts = YAC_TRUE; // 1=store source (often compresses better) 0=store tokenized form (sometimes compresses better)

static sUI i_return;

static void tks_sigint         (int _sig);
static int  print_usage        (void);
static void NewEngine          (int _argc, char **_argv, sBool _bRunInBG = 0);
static void DeleteEngine       (void);
static void exit_clean         (int _r);
static void exit_print_usage   (void);
static void exit_print_version (void);
static int  scan_ac            (int _argc, char **_argv);
static int  ac_compile         (const char *_name, const char *_takname);
static int  ac_list            (const char *_name);
static int  ac_run             (const char *_name, sBool _bRunInBG = 0);
static int  print_version      (void);
static int  tks_main           (int _argc, char **_argv);
static int  StartDefaultPakfile(void);

TKS_ScriptEngine *tkscript;
YAC_Host *yac_host;


class TKS_ExtraFileEntry {
public:
   TKS_ExtraFileEntry *next;
   const char *name;
   sBool b_pre;  // 1=prepend before other files (-ep)  0=append after current file (-ef)
};

static TKS_ExtraFileEntry *extra_files = NULL;

static void tks_delete_extrafile_list(void) {
   // Delete extra file list
   TKS_ExtraFileEntry *e = extra_files;

   while(NULL != e)
   {
      TKS_ExtraFileEntry *n = e->next;
      e->next = NULL;
      delete e;
      e = n;
   }

   extra_files = NULL;
}

void tks_get_extrafile_stringarray(YAC_Value *_r) {
   // called by TKS.getExtraFiles
   YAC_StringArray *a = YAC_New_StringArray();
   if(NULL != a)
   {
      TKS_ExtraFileEntry *e = extra_files;
      while(NULL != e)
      {
         YAC_String s;
         s.visit(e->name);
         YAC_Value r;
         r.initString(&s, 0);
         a->yacArraySet(NULL/*context*/, a->num_elements, &r);

         // Next extra file
         e = e->next;
      }
      _r->initObject(a, 1);
   }
   else
   {
      // Failed to allocate array (should not be reachable)
      _r->initNull();
   }
}

static void tks_addextrafile(const char *_name, sBool _bPre = YAC_FALSE) {
   TKS_ExtraFileEntry *e = new(std::nothrow) TKS_ExtraFileEntry();
   e->next  = NULL;
   e->name  = _name;
   e->b_pre = _bPre;

   // printf("xxx tks_addextrafile: name=%s\n", _name);

   if(NULL == extra_files)
   {
      extra_files = e;
   }
   else
   {
      TKS_ExtraFileEntry *p = extra_files;
      while(NULL != p->next)
         p = p->next;
      p->next = e;
   }
}

static YAC_String extrafile_buf;

static void tks_addextrafiles_from_file(YAC_String *_fileName) {

   // printf("[...] adding extra files from \"%s\".\n", _fileName->chars);

   if(extrafile_buf.readRawFile(_fileName))
   {
      sUI i;

      for(i=0; i<extrafile_buf.length; i++)
      {
         if( ('\n' == extrafile_buf.chars[i]) || ('\r' == extrafile_buf.chars[i]) )
         {
            extrafile_buf.chars[i] = 0;
         }
      }

      sUI off = 0;

      while(off < extrafile_buf.length)
      {
         sSI idx = extrafile_buf.indexOf((char)0, off);

         if(-1 != idx)
         {
            if(idx != (sSI)off)
            {
               if('#' != extrafile_buf.chars[off])
               {
                  tks_addextrafile((const char*) (extrafile_buf.chars + off));
               }
            }

            off = (sUI) idx + 1;
         }
         else
         {
            break; // ??
         }
      }
   }
}


void tks_sigint(int _sig) {
   if(NULL != tkscript)
   {
      void *f;
      YAC_Value r; r.initInt(YAC_FALSE);

      if( (f=tkscript->sig_signal) )
      {
         YAC_Value args[1]; args[0].initInt(_sig);
         tkscript->main_context->b_running = 1;
         tkscript->yacEvalFunctionReturn(tkscript->main_context, f, 1, args, &r);
      }

      if((SIGINT == _sig) && (YAC_FALSE == r.value.int_val))
      {
         tkscript->printf("\n\n[...] SIGINT caught. calling exit().\n");
         APIC_exit(tkscript->i_return);
      }

      ::signal(SIGINT, tks_sigint);
   }
}

#if 0
void tks_sigusr1(int _sig) {
#if 0
   tkscript->printf("xxx tks_sigusr1: recvd sig=%d\n", _sig);
#else
   (void)_sig;
#endif
}
#endif

static YAC_String loc_exe_name;



int print_version(void) {
   ::printf(TKS_VERSION_STR);
   return 0/*exit code*/;
}


int print_usage(void) {

   ::printf("TKS - an extensible c++ glue script language %s. (C) 2001-2025 Bastian Spiegel.\n\n", TKS_VERSION_STR);
   ::printf("          -- visit http://tkscript.de for further information.\n");
   ::printf("\n USAGE:\n");
   ::printf("        %s source [args]\n", (char*)loc_exe_name.chars);
   ::printf("        %s source.tks [args]\n", (char*)loc_exe_name.chars);
   ::printf("        %s source.tkx [args]\n", (char*)loc_exe_name.chars);
   ::printf("        %s source.tkp [args]\n", (char*)loc_exe_name.chars);
   ::printf("        %s -c    --compile proj\n", (char*)loc_exe_name.chars);
   ::printf("        %s -c    --compile proj.tkp [outfile.tkx]\n", (char*)loc_exe_name.chars);
   ::printf("        %s -cl   --compile library\n", (char*)loc_exe_name.chars);
   ::printf("        %s -cl   --compile library.tkp [outfile.tsl]\n", (char*)loc_exe_name.chars);
   ::printf("        %s -d    --debuglevel nr\n", (char*)loc_exe_name.chars);
#ifdef TKS_JIT
   ::printf("        %s -da   --disassemble\n", (char*)loc_exe_name.chars);
   ::printf("        %s -fi   --forceint\n", (char*)loc_exe_name.chars);
   ::printf("        %s -njo  --nojitopt\n", (char*)loc_exe_name.chars);
   ::printf("        %s -nbc  --noboundarycheck\n", (char*)loc_exe_name.chars);
#endif
   ::printf("        %s -nra  --noarrayautoresize\n", (char*)loc_exe_name.chars);
   ::printf("        %s -ps   --packedscripts\n", (char*)loc_exe_name.chars);
   ::printf("        %s -nur  --nounresolveds\n", (char*)loc_exe_name.chars);
   ::printf("        %s -num  --nounresolvedmembers\n", (char*)loc_exe_name.chars);
   ::printf("        %s -nuf  --nounresolvedfunctions\n", (char*)loc_exe_name.chars);
   ::printf("        %s -ncs  --noconstraints\n", (char*)loc_exe_name.chars);
   ::printf("        %s -nln  --nolinenumbers\n", (char*)loc_exe_name.chars);
   ::printf("        %s -ndo  --nodeleteobjects\n", (char*)loc_exe_name.chars);
   ::printf("        %s -ecs  --enableconstraints\n", (char*)loc_exe_name.chars);
   ::printf("        %s -fss  --functionstacksize <size>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -fcs  --functioncallstacksize <size>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -esp  --enablestatementprofiling %s\n", (char*)loc_exe_name.chars,
#ifdef TKS_PROFILE_STATEMENTS
   ""
#else
   "(n/a)"
#endif
   );
   ::printf("        %s -efp  --enablefunctionprofiling %s\n", (char*)loc_exe_name.chars,
#ifdef TKS_PROFILE_FUNCTIONS
   ""
#else
   "(n/a)"
#endif
   );
   ::printf("        %s -ee   --emitexplanations\n", (char*)loc_exe_name.chars);
   ::printf("        %s -h    --help\n", (char*)loc_exe_name.chars);
   ::printf("        %s -l    --list proj.tkx\n", (char*)loc_exe_name.chars);
   ::printf("        %s -lp   --librarypath </path/to/dir/>|<drive:\\path\\to\\dir\\>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -ch   --chapter <name>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -mp   --modulepath </path/to/dir/>|<drive:\\path\\to\\dir\\>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -nlf  --nolocalfiles\n", (char*)loc_exe_name.chars);
   ::printf("        %s -np   --noplugins\n", (char*)loc_exe_name.chars);
   ::printf("        %s -pp   --pluginpath </path/to/dir/>|<drive:\\path\\to\\dir\\>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -ap   --applicationpath </path/to/dir/>|<drive:\\path\\to\\dir\\>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -ef   --extrafile <sourcefile>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -pl   --poollimit <prio> <kbytes>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -pla  --poollimitall <kbytes>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -pfr  --poolfreeratio <prio> <ratio>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -pfra --poolfreeratioall <ratio>\n", (char*)loc_exe_name.chars);
   ::printf("        %s -sp   --showprogress\n", (char*)loc_exe_name.chars);
   ::printf("        %s -q    --quiet\n", (char*)loc_exe_name.chars);
   ::printf("        %s -v    --version\n", (char*)loc_exe_name.chars);
   // secret printf("        %s -nt, --nothreads\n", (char*)loc_exe_name.chars);
   return 0;
}

void NewEngine(int _argc, char **_argv, sBool _bRunInBG) {
   DeleteEngine();

   tkscript = new TKS_ScriptEngine();
   yac_host = tkscript;

   // // loc_exe_name.visit("tks");

   tkscript->init();

   // Remember runtime directory
   {
      tkscript->configuration.tks_exe_path.visit(_argv[0]);
      // // printf("xxx _argv[0]=\"%s\"\n", _argv[0]);
      YAC_String s;
      s.visit(_argv[0]);
      s.getDirName(&tkscript->configuration.tks_exe_dir);
      loc_exe_name.copy(_argv[0]);
      loc_exe_name.replace(&tkscript->configuration.tks_exe_dir, "");
      loc_exe_name.replace(".bin", "");
      loc_exe_name.replace(".exe", "");
      // // if(loc_exe_name.startsWith("/"))
      // //    loc_exe_name._substring_YAC_RSELF(1, 99);
   }

   // Set app/mod/lib dirs to exe path subdirs when blank
   tkscript->configuration.lazySetFallbackPaths();

   // Query temp/home dir and set tks_local_pathname
   tks_init_streamio();

   if(!_bRunInBG)
   {
      // Install exception handler
      tks_debug_init(_argc, _argv);
   }

#ifdef TKS_EXPORT_ENTITY_SCANNER
   TKS_TokenizedScript::InitEntityTree();
   TKS_XMLParser      ::InitEntityTree();
#endif
}

void DeleteEngine(void) {
   sBool dbg = YAC_FALSE;

   if(NULL != tkscript)
   {
      i_return = tkscript->i_return;
      dbg = (0 != tkscript->configuration.debug_level);
      tks_free_streamio();
      delete tkscript;
      tkscript = NULL;
      yac_host = NULL;
   }

#ifdef TKS_DCON
   if(dbg)
   {
#ifdef YAC_OBJECT_COUNTER
      ::printf("[...] DeleteEngine():    Object::object_counter = %i\n", (int) (YAC_Object::object_counter-TKS_NUM_STATIC_OBJECTS));
#endif // YAC_OBJECT_COUNTER
      ::printf("[...] DeleteEngine(): Statement::object_counter = %i\n", (int) (PTN_Statement::object_counter));
   }
#endif

   tks_debug_exit();

   tks_delete_extrafile_list();
}

void exit_clean(int _r) {
   DeleteEngine();
   ::fflush(stdout);
   ::exit(_r);
}

void exit_print_usage(void) {
   exit_clean(!print_usage());
}

void exit_print_version(void) {
   exit_clean(print_version());
}

static void extract_tkp_dirname_from(YAC_String *_s) {
   //
   // Extract directory name from .tkp (.tks) filename.
   // If the dirname is empty (== relative filename)
   // make it absolute.
   //
   _s->getDirName(&tkscript->configuration.tkp_dir);

   if(tkscript->configuration.tkp_dir.isBlank())
   {
      tkscript->configuration.tkp_dir.alloc(1024);
#ifdef YAC_WIN32
      GetCurrentDirectory(1024, (LPSTR) tkscript->configuration.tkp_dir.chars);
#else
      {
         char *r = getcwd((char*) tkscript->configuration.tkp_dir.chars, 1024);
         (void)r;
      }
#endif // YAC_WIN32
      tkscript->configuration.tkp_dir.fixLength();
      tkscript->configuration.tkp_dir.append("/");
   }
}

int ac_compile(const char *_name, const char *_takname) {
   // compile project and create .tkx file
   tkscript->tkx.str_tkpfilename.copy(_name);

   if(tkscript->tkx.str_tkpfilename.endsWith(".tkp"))
   {
      // -c file.tkp takfile
      if(NULL == _takname)
      {
         return print_usage();
      }

      tkscript->tkx.str_takfilename.copy(_takname);
   }
   else
   {
      // -c file
      if(NULL != _takname)
      {
         return print_usage();
      }

      tkscript->tkx.str_tkpfilename.append(".tkp");
      tkscript->tkx.str_takfilename.copy(_name);

      if(tkscript->configuration.b_tsl_mode)
      {
         tkscript->tkx.str_takfilename.append(".tsl");
      }
      else
      {
         tkscript->tkx.str_takfilename.append(".tkx");
      }
   }

   /*if(!tkscript->tkx.str_takfilename.endsWith(".tkx"))
   {
      ::printf("\n[---] target file is not a TKS pak file (.tkx). aborting.\n");
      return print_usage();
   }*/

   ::printf("[...] compile: source: %s\n", (char*)tkscript->tkx.str_tkpfilename.chars);
   ::printf("[...] compile: target: %s\n", (char*)tkscript->tkx.str_takfilename.chars);

   if(tkscript->tkx.str_tkpfile.readRawFile(&tkscript->tkx.str_tkpfilename))
   {
      tkscript->tkx.str_tkpfile.removeCR();
      tkscript->configuration.b_rewrite_scripts = !b_forcenorewritescripts; // Store scripts as seen by the scanner
      tkscript->tkx.scanProject();

      if(0 != tkscript->tkx.tak_error_code)
      {
         ::printf("\n[---] failed to process .tkp file (err=%i).\n", tkscript->tkx.tak_error_code);
         return 0;
      }

      tkscript->tkx.writeTKX();
      return 1;
   }
   else
   {
      ::printf("\n[---] error opening project (.tkp) file %s\n",(char*)tkscript->tkx.str_tkpfilename.chars);
      return 0;
   }
}


int ac_list(const char *_name) {
   // Display contents of .tkx file

   tkscript->tkx.str_takfilename.copy(_name);
   ::printf("[...] VFS Directory of \"%s\".\n", _name);

   if(tkscript->tkx.str_takfile.readRawFile(&tkscript->tkx.str_takfilename))
   {
      if(tkscript->tkx.scanTKX())
      {
         tkscript->tkx.continueChapterLoading();
         tkscript->tkx.listContent();
         return 1;
      }
   }

   return 0;
}


int ac_run(const char *_name, sBool _bRunInBG) {
   // Execute selected input file (.tks, .tkp, .tkx)

   if(!tkscript->init2())
   {
      ::printf("[---] TKS_ScriptEngine::init2() failed.\n");
      return 0;
   }

   YAC_String cachename;
   YAC_String t;
   t.copy(_name);

   // Handle "# tks app:ying" style program name arguments
   t.replace("app:", &tkscript->configuration.application_path);

#ifdef HAVE_CYGWIN
   t.replace("/usr/lib", &tkscript->configuration.cygwin_usrlib);
#endif // HAVE_CYGWIN

   sBool endswithtks = t.endsWith(".tks");

   if(!endswithtks)
   {
      endswithtks = t.endsWith(".TKS"); // (note) should be removed, uppercase .TKS file suffix is not really supported
   }

   if(endswithtks)
   {
      YAC_String proj;
      proj.alloc(512);
      proj.printf("[project]\nname=\"n/a\"\n[chapter]\nname=\"main\"\n\"code/main.tks\" =\"%s\"\n",
                  (char*)t.chars
                  );
      tkscript->tkx.str_tkpfile.yacCopy(&proj);
      tkscript->configuration.b_simulatevfs = YAC_TRUE;
      extract_tkp_dirname_from(&t);
      tkscript->tkx.scanProject();

      if(0 != tkscript->tkx.tak_error_code)
      {
         ::printf("\n[---] failed to process .tkp file (err=%i).\n", tkscript->tkx.tak_error_code);
         return 0;
      }
   }
   else
   {
      sBool endswithtkp = t.endsWith(".tkp");

      if(!endswithtkp)
      {
         // (todo) should be removed, uppercase .TKP file suffix is not really supported
         endswithtkp = t.endsWith(".TKP");
      }

      sBool endswithtkx = t.endsWith(".tkx");

      if(!endswithtkx)
      {
         // (todo) should be removed, uppercase .TKX file suffix is not really supported
         endswithtkx = t.endsWith(".TKX");
      }

      if(!endswithtkx)
      {
         endswithtkx = t.endsWith(".tsl");
      }

      if(!endswithtkx)
      {
         // (todo) should be removed, uppercase .TSL file suffix is not really supported
         endswithtkx = t.endsWith(".TSL");
      }

      sBool trynxs = endswithtkx;
      sBool trytks = YAC_TRUE;

      if(!(endswithtkp || endswithtkx))
      {
         // support tab-completion. auto-expand to .tkp if multiple selections are available
         if(t.endsWith(".tk"))
         {
            t.append("p");
         }
         else
         {
            t.append(".tkp");
         }

         endswithtkp = YAC_TRUE;
         trynxs      = YAC_TRUE;
         trytks      = YAC_TRUE;
      }

      if(endswithtkp)
      {
         // treat argument as project file name. run as design-time exe
         extract_tkp_dirname_from(&t);
         tkscript->tkx.str_tkpfilename.yacCopy(&t);

         if(tkscript->configuration.debug_level > 0)
         {
            printf("[...] source: %s\n", (char*)tkscript->tkx.str_tkpfilename.chars);
         }

         if(tkscript->tkx.str_tkpfile.readRawFile(&tkscript->tkx.str_tkpfilename))
         {
            YAC_String efName = &tkscript->tkx.str_tkpfilename;
            efName.append(".ef");
            tks_addextrafiles_from_file(&efName);

            tkscript->configuration.b_simulatevfs = YAC_TRUE;
            tkscript->tkx.str_tkpfile.removeCR();
            tkscript->tkx.scanProject();

            if(0 != tkscript->tkx.tak_error_code)
            {
               ::printf("\n[---] failed to process .tkp file (err=%i).\n", tkscript->tkx.tak_error_code);
               return 0;
            }

            trynxs = YAC_FALSE;
         }
         else
         {
            if(trynxs)
            {
               t.replace(".tkp", ".tkx");
               endswithtkx = YAC_TRUE;
            }
            else
            {
               ::printf("\n[---] error opening project file %s\n",(char*)tkscript->tkx.str_tkpfilename.chars);
               return 0;
            }
         }
      }

      if(trynxs)
      {
         if(endswithtkx)
         {
#ifdef YAC_WIN32
#ifdef HAVE_URLMON
            if(t.startsWith("http:") || t.startsWith("ftp:") || t.startsWith("https:"))
            {
               cachename.alloc(MAX_PATH);
               // Download to cache file
               ::URLDownloadToCacheFile(0, (const char*)t.chars, (char*)cachename.chars, MAX_PATH-1, 0, 0);
               t.copy((char*)cachename.chars);
            }
#endif
#endif
            // Treat argument as pak file name. simulate runtime
            tkscript->configuration.b_simulatevfs = YAC_FALSE;
            extract_tkp_dirname_from(&t);
            tkscript->tkx.str_takfilename.yacCopy(&t);

            if(tkscript->tkx.str_takfile.readRawFile(&tkscript->tkx.str_takfilename))
            {
               YAC_String efName = &tkscript->tkx.str_takfilename;
               efName.append(".ef");
               tks_addextrafiles_from_file(&efName);

               tkscript->tkx.scanTKX();
               tkscript->tkx.continueChapterLoading();
            }
            else
            {
               if(trytks)
               {
                  t.replace(".tkx", ".tks");
                  YAC_String proj;
                  proj.alloc(512);
                  proj.printf("[project]\nname=\"na\"\n[chapter]\nname=\"main\"\n\"code/main.tks\" =\"%s\"\n",
                              (char*)t.chars
                              );
                  tkscript->tkx.str_tkpfile.yacCopy(&proj);
                  extract_tkp_dirname_from(&t);
                  tkscript->configuration.b_simulatevfs = YAC_TRUE;
                  tkscript->tkx.scanProject();

                  if(0 != tkscript->tkx.tak_error_code)
                  {
                     //::printf("\n[---] failed to process .tkp file (err=%i).\n", tkscript->tkx.tak_error_code);
                     return 0;
                  }
               }
               else
               {
                  ::printf("[---] failed to load .tkx file \"%s\".\n",
                     (char*)tkscript->tkx.str_takfilename.chars);
                  return 0;
               }
            }
            // execute..
         }
         else
         {
            ::printf("[---] unknown fileformat (neither .tkp nor .tkx). cannot execute.\n");
            return 0;
         }
      }
   }


#ifdef YAC_POSIX
   // Disallow SIGIO signal since some linux drivers use this to notify applications
   //  (e.g. ALSA async callback). If we would not block the SIGIO by default, the ALSA callback
   //  would be run in the context of whatever thread is currently active.
   sigset_t ss;
   sigemptyset(&ss);
   sigaddset(&ss, SIGIO);
   sigaddset(&ss, SIGUSR1);  // (note) librs (optionally) uses this for SHM ringbuffer notifications
   pthread_sigmask(SIG_BLOCK, &ss, NULL);
#endif // YAC_POSIX

   ::signal(SIGINT, &tks_sigint);
#ifdef YAC_POSIX
   ::signal(SIGPIPE, &tks_sigint);
#if 0
   ::signal(SIGUSR1, &tks_sigusr1);
#endif
#endif

   if(tkscript->configuration.debug_level > 0)
   {
      Dprintf("[...] using pluginpath \"%s\".\n", (sChar*)tkscript->configuration.plugin_path.chars);
   }

   // Add extra files to project
   {
      TKS_ExtraFileEntry *e = extra_files;
      while(NULL != e)
      {
         YAC_String sName; sName.visit(e->name);
         if(sName.startsWith("lib:") && !sName.endsWith(".tsl"))
            sName.append(".tsl");
         tkscript->tkx.addExtraFile((const char*)sName.chars, e->b_pre); // copies e->name

         // printf("xxx tkscript->tkx.addExtraFile(e->name=%s)\n", e->name);

         if(0 != tkscript->tkx.tak_error_code)
         {
            // Failed to add extra file
            // ::printf("xxx ac_run: failed to add extra file!!\n");
            return 0;
         }

         // Next extra file
         e = e->next;
      }

      tks_delete_extrafile_list();
   }

   tkscript->compileRun(_bRunInBG);
   if(!_bRunInBG)
      ::signal(SIGINT, SIG_DFL);
   return 1;
}

int StartDefaultPakfile(void) {
   // ---- treat argument as pak file name. simulate runtime
   tkscript->configuration.b_simulatevfs=0;
   tkscript->tkx.str_takfilename.copy("data.tkx");

   return ac_run("data.tkx", YAC_FALSE/*bRunInBG*/);
}

int scan_ac(int _argc, char **_argv) {
   // Parse command line arguments
   int i;
   for(i=1; i<_argc; i++)
   {
      YAC_String ca;
      ca.visit(_argv[i]);
      if(ca.compare("-i") || ca.compare("--interactive"))
      {
         // ---- enable interactive mode ----
         b_interactive = YAC_TRUE;
         printf("[...] interactive mode\n");
      }
      else if(ca.compare("-c") || ca.compare("--compile"))
      {
         // ---- compile TKX mode ----
         if(i+2 < _argc)
         {
            return (ac_compile(_argv[i+1], _argv[i+2]));
         }
         else
         {
            return(ac_compile(_argv[i+1], 0));
         }
      }
      else if(ca.compare("-cl") || ca.compare("--compilelibrary"))
      {
         // ---- compile TSL mode ----
         tkscript->configuration.b_tsl_mode = YAC_TRUE;

         if(i+2 < _argc)
         {
            return (ac_compile(_argv[i+1], _argv[i+2]));
         }
         else
         {
            return(ac_compile(_argv[i+1], 0));
         }
      }
      else if(ca.compare("-ps") || ca.compare("--packedscripts"))
      {
         // ---- store scripts in tokenized form in tkx/tsl files ----
         b_forcenorewritescripts = YAC_FALSE;
         Dprintf("[...] tkx/tsl script packing enabled.\n");
      }
      else if(ca.compare("-fi")||ca.compare("--forceint"))
      {
#ifdef TKS_JIT
         // ---- force interpreted mode ----
         tkscript->configuration.b_forceint = YAC_TRUE;
#endif
      }
#ifdef TKS_JIT
      else if(ca.compare("-njo") || ca.compare("--nojitopt"))
      {
         // ---- force interpreted mode ----
         tkscript->configuration.b_jitopt = YAC_FALSE;
         Dprintf("[...] disabling JIT optimization.\n");
      }
#endif // TKS_JIT
      else if(ca.compare("-nbc") || ca.compare("--noboundarycheck"))
      {
         // ---- disable boundary checks ----
         tkscript->configuration.b_checkbounds = YAC_FALSE;
      }
      else if(ca.compare("-nra") || ca.compare("--noarrayautoresize"))
      {
         // ---- disable array auto-resizing (interpreted mode only) ----
         tkscript->configuration.b_autoresizearrays = YAC_FALSE;
      }
      else if(ca.compare("-nur") || ca.compare("--nounresolveds"))
      {
         // ---- disable PTN_UnresolvedModule* nodes ----
         tkscript->configuration.b_enable_unresolved_members   = YAC_FALSE;
         tkscript->configuration.b_enable_unresolved_functions = YAC_FALSE;
      }
      else if(ca.compare("-num") || ca.compare("--nounresolvedmembers"))
      {
         // ---- disable PTN_UnresolvedModuleMember* nodes ----
         tkscript->configuration.b_enable_unresolved_members = YAC_FALSE;
      }
      else if(ca.compare("-nuf") || ca.compare("--nounresolvedfunctions"))
      {
         // ---- disable PTN_UnresolvedModuleMethod* nodes ----
         tkscript->configuration.b_enable_unresolved_functions = YAC_FALSE;
      }
      else if(ca.compare("-ncs") || ca.compare("--noconstraints"))
      {
         // ---- disable evaluation of script constraints ----
         tkscript->configuration.b_enable_constraints = YAC_FALSE;
      }
      else if(ca.compare("-ecs") || ca.compare("--enableconstraints"))
      {
         // ---- (re-)enable evaluation of script constraints ----
         tkscript->configuration.b_enable_constraints = YAC_TRUE;
      }
      else if(ca.compare("-ndo") || ca.compare("--nodeleteobjects"))
      {
#ifdef TKS_ALLOWNODELETEOBJECTS
         // ---- never delete objects, just mark invalid (debugging) ----
         tkscript->configuration.b_enable_delete_objects = YAC_FALSE;
#endif // TKS_ALLOWNODELETEOBJECTS
         // Disable object pooling to avoid error messages at exit
         for(sUI k = 0u; k < YAC_NUM_POOL_PRIORITIES; k++)
         {
            ObjectPool::UpdateTotalSizeLimit(k, 0);
         }
      }
      else if(ca.compare("-esp") || ca.compare("--enablestatementprofiling"))
      {
#ifdef TKS_PROFILE_STATEMENTS
         // ---- enable script statement profiling ----
         tkscript->configuration.b_enable_statement_profiling = YAC_TRUE;
         tkscript->configuration.b_keep_source_after_compile  = YAC_TRUE;
#endif
      }
      else if(ca.compare("-efp")||ca.compare("--enablefunctionprofiling"))
      {
#ifdef TKS_PROFILE_FUNCTIONS
         // ---- enable function/method call profiling ----
         tkscript->configuration.b_enable_function_profiling = YAC_TRUE;
#endif
      }
      else if(ca.compare("-fss")||ca.compare("--functionstacksize"))
      {
         if(++i < _argc)
         {
            ca.visit(_argv[i]);
            // ---- set size of function stack ----
            sSI vali;
            ca.yacScanI(&vali);

            if(((sUI)vali) > TKS_MAX_FUNCTION_STACK_SIZE)
            {
               Dprintf("[---] illegal function stack size (%i>%i)\n",
                       vali, TKS_MAX_FUNCTION_STACK_SIZE
                       );
               exit_print_usage();
            }
            else
            {
               tkscript->configuration.function_stack_size = (sUI)vali;
            }
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-fcs") || ca.compare("--functioncallstacksize"))
      {
         if(++i < _argc)
         {
            ca.visit(_argv[i]);
            // ---- set size of function-call stack ----
            sSI vali;
            ca.yacScanI(&vali);

            if(((sUI)vali) > TKS_MAX_FUNCTIONCALL_STACK_SIZE)
            {
               Dprintf("[---] illegal functioncall stack size (%i>%i)\n",
                       vali, TKS_MAX_FUNCTIONCALL_STACK_SIZE
                       );
               exit_print_usage();
            }
            else
            {
               tkscript->configuration.functioncall_stack_size = (sUI)vali;
            }
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-mp") || ca.compare("--modulepath"))
      {
         // ---- set module path ----
         if(++i < _argc)
         {
            tkscript->configuration.module_path.copy(_argv[i]);
            ::printf("[...] module path set to \"%s\"\n", _argv[i]);
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-np") || ca.compare("--noplugins"))
      {
         // ---- quiet operation ----
         tkscript->configuration.b_enable_plugins = YAC_FALSE;
      }
      else if(ca.compare("-pp") || ca.compare("--pluginpath"))
      {
         // ---- set plugin path ----
         if(++i < _argc)
         {
            tkscript->configuration.plugin_path.copy(_argv[i]);
            ::printf("[...] plugin path set to \"%s\"\n", _argv[i]);
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-lp") || ca.compare("--librarypath"))
      {
         // ---- set library path ----
         if(++i < _argc)
         {
            tkscript->configuration.library_path.copy(_argv[i]);
            ::printf("[...] library path set to \"%s\"\n", _argv[i]);
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-ap") || ca.compare("--applicationpath"))
      {
         // ---- set application path ----
         if(++i < _argc)
         {
            tkscript->configuration.application_path.copy(_argv[i]);
            ::printf("[...] application path set to \"%s\"\n", _argv[i]);
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-ef") || ca.compare("--extrafile"))
      {
         // ---- Append extra file to already loaded project ----
         if(++i < _argc)
         {
            tks_addextrafile(_argv[i], YAC_FALSE/*bPre*/);
            if(0 != tkscript->configuration.debug_level)
               ::printf("[...] append extra file \"%s\".\n", _argv[i]);
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-ep") || ca.compare("--extrafile-pre"))
      {
         // ---- Prepend extra file to already loaded project ----
         if(++i < _argc)
         {
            tks_addextrafile(_argv[i], YAC_TRUE/*bPre*/);
            if(0 != tkscript->configuration.debug_level)
               ::printf("[...] prepend extra file \"%s\".\n", _argv[i]);
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-pl") || ca.compare("--poollimit"))
      {
         // ---- Set pool limit in kbytes ----
         if(i+2 < _argc)
         {
            ca.visit(_argv[++i]);
            // ---- scan priority level (0..2) ----
            sSI vali;
            ca.yacScanI(&vali);

            if(((sUI)vali) >= YAC_NUM_POOL_PRIORITIES)
            {
               Dprintf("[---] illegal pool priority (%i>=%i)\n",
                       vali, YAC_NUM_POOL_PRIORITIES
                       );
               exit_print_usage();
            }
            else
            {
               sSI prio = vali;
               ca.visit(_argv[++i]);
               // ---- scan pool size (kbytes) ----
               ca.yacScanI(&vali);

               if(((sUI)vali) > ObjectPool::MAX_POOL_LIMIT)
               {
                  Dprintf("[---] illegal pool limit (%i>=%i)\n",
                          vali, ObjectPool::MAX_POOL_LIMIT
                          );
                  exit_print_usage();
               }
               else
               {
                  ObjectPool::UpdateTotalSizeLimit(prio, vali * 1024);
                  if(tkscript->configuration.debug_level > 0)
                  {
                     Dprintf("[...] pool limit for priority class %d set to %d kbytes.\n", prio, vali);
                  }
               }
            }
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-pla") || ca.compare("--poollimitall"))
      {
         // ---- Set pool limit in kbytes for all priority classes ----
         if(++i < _argc)
         {
            ca.visit(_argv[i]);
            // ---- scan pool size (kbytes) ----
            sSI vali;
            ca.yacScanI(&vali);

            if(((sUI)vali) > ObjectPool::MAX_POOL_LIMIT)
            {
               Dprintf("[---] illegal pool limit (%i>=%i)\n",
                       vali, ObjectPool::MAX_POOL_LIMIT
                       );
               exit_print_usage();
            }
            else
            {
               sUI k;

               for(k=0; k<YAC_NUM_POOL_PRIORITIES; k++)
               {
                  ObjectPool::UpdateTotalSizeLimit(k, vali * 1024);
               }

               if(tkscript->configuration.debug_level > 0)
               {
                  Dprintf("[...] pool limit for all priority class set to %d kbytes.\n", vali);
               }
            }
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-pfr") || ca.compare("--poolfreeratio"))
      {
         // ---- Set pool free threshold ratio (float) ----
         if(i+2 < _argc)
         {
            ca.visit(_argv[++i]);
            // ---- scan priority level (0..2) ----
            sSI vali;
            ca.yacScanI(&vali);

            if(((sUI)vali) >= YAC_NUM_POOL_PRIORITIES)
            {
               Dprintf("[---] illegal pool priority (%i>=%i)\n",
                       vali, YAC_NUM_POOL_PRIORITIES
                       );
               exit_print_usage();
            }
            else
            {
               sSI prio = vali;
               ca.visit(_argv[++i]);
               // ---- scan ratio (float) ----
               sF32 valf;
               ca.yacScanF32(&valf);
               {
                  ObjectPool::UpdateFreeThresholdRatio(prio, valf);

                  if(tkscript->configuration.debug_level > 0)
                  {
                     Dprintf("[...] pool free threshold ratio for priority class %d set to %1.3f.\n", prio, valf);
                  }
               }
            }
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-pfra") || ca.compare("--poolfreeratioall"))
      {
         // ---- Set pool free ratio for all priority classes ----
         if(++i < _argc)
         {
            ca.visit(_argv[i]);
            // ---- scan ratio (float) ----
            sF32 valf;
            ca.yacScanF32(&valf);
            {
               sUI k;

               for(k=0; k<YAC_NUM_POOL_PRIORITIES; k++)
               {
                  ObjectPool::UpdateFreeThresholdRatio(k, valf);
               }

               if(tkscript->configuration.debug_level > 0)
               {
                  Dprintf("[...] pool free threshold ratio for all priority class set to %1.3f .\n", valf);
               }
            }
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-sp") || ca.compare("--showprogress"))
      {
         // ---- show scan/compile progress ----
         tkscript->configuration.b_compile_show_progress = YAC_TRUE;
      }
      else if(ca.compare("-da") || ca.compare("--disassemble"))
      {
#ifdef TKS_JIT
         // ---- show disassembly ----
         tkscript->configuration.b_showasm = YAC_TRUE;
#endif
      }
      else if(ca.compare("-q") || ca.compare("--quiet"))
      {
         // ---- quiet operation ----
         tkscript->configuration.b_enableprint = YAC_FALSE;
      }
      else if(ca.compare("-v") || ca.compare("--version"))
      {
         // ---- quiet operation ----
         exit_print_version();
      }
      else if(ca.compare("-h") || ca.compare("--help"))
      {
         // ---- show help ----
         exit_print_usage();
      }
      else if(ca.compare("-ee") || ca.compare("--emitexplanations"))
      {
         // ---- dump documentation strings to stdout after parsing the script ----
         tkscript->configuration.b_emit_explanations = YAC_TRUE;
      }
      else if(ca.compare("-nlf") || ca.compare("--nolocalfiles"))
      {
         // ---- no local file access allowed in scripts ----
         tkscript->configuration.b_enablelocalfiles = YAC_FALSE;
      }
      else if(ca.compare("-nln") || ca.compare("--nolinenumbers"))
      {
         // ---- disable line numbers in .tkx/.tsl files ----
         tkscript->configuration.b_tkx_linenumbers = YAC_FALSE;
      }
      else if(ca.compare("-d") || ca.compare("--debuglevel"))
      {
         // ---- set debug level ----
         if((++i) < _argc)
         {
            sSI dlev;
            ////sscanf(_argv[i], "%d", &dlev);
            dlev = atoi(_argv[i]);
            tkscript->configuration.debug_level = (sU8)dlev;
            printf("[...] debug level set to %i\n", (sU8)dlev);
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-l") || ca.compare("--list"))
      {
         // ---- list pak file contents ----
         if((++i) < _argc)
         {
            return(ac_list(_argv[i]));
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-ch") || ca.compare("--chapter"))
      {
         // ---- select default chapter  ----
         if(++i < _argc)
         {
            tkscript->configuration.default_chapter.copy(_argv[i]);
            ::printf("[...] default chapter set to \"%s\"\n", _argv[i]);
         }
         else
         {
            exit_print_usage();
         }
      }
      else if(ca.compare("-smk") || ca.compare("--sharedmemorykey"))
      {
         // ---- set shared memory key for "script-thread" IPC ----
         if(++i < _argc)
         {
            tkscript->configuration.sharedmemory_key.copy(_argv[i]);
            ::printf("[...] shared memory key set to \"%s\"\n", _argv[i]);
         }
         else
         {
            exit_print_usage();
         }
      }
      else
      {
         // ---- treat argument as source/project/pak file name/prefix ----
         if((_argc - i) > 0)
         {
            if((_argc - i) > 1)
            {
               // pass all following args to script engine
               tkscript->setArgs(_argc - i - 1, _argv + i + 1);
            }
            return ac_run(_argv[i], YAC_FALSE/*bRunInBG*/);
         }
      }
    }
    return 0;
}

#ifdef TKS_PRINT_SIZEOFS
#include "YAC_Byte.h"
#include "YAC_UnsignedByte.h"
#include "YAC_Short.h"
#include "YAC_UnsignedShort.h"
#include "YAC_Integer.h"
#include "YAC_UnsignedInteger.h"
#include "YAC_Long.h"
#include "YAC_UnsignedLong.h"
#include "YAC_Double.h"
#include "YAC_Float.h"
#include "YAC_Integer.h"
#include "YAC_Long.h"
#include "YAC_ValueObject.h"

#include "YAC_HashTable.h"
#include "tks_inc_class.h"
#include "tks_inc_compiler.h"
#include "YAC_ClassArray.h"
#include "YAC_List.h"
#include "YAC_ListNode.h"
#include "YAC_File.h"
#include "YAC_StringArray.h"
#endif // TKS_PRINT_SIZEOFS


#define Dprintsizeof(a) ::printf("[...] sizeof(%s)=%i\n", #a, sizeof(a))


int tks_main(int _argc, char **_argv) {

   //::printf("[...] initial (static) Object::object_counter=%i (should be %i)\n", YAC_Object::object_counter, TKS_NUM_STATIC_OBJECTS);

#ifdef TKS_PRINT_SIZEOFS
   // Debug: print common object byte sizes
   Dprintsizeof(PTN_Function);
   Dprintsizeof(TKS_ClassDecl);
   Dprintsizeof(TKS_CachedObject);
   Dprintsizeof(TKS_ScriptClassInstance);

   Dprintsizeof(YAC_String);
   Dprintsizeof(YAC_Byte);
   Dprintsizeof(YAC_UnsignedByte);
   Dprintsizeof(YAC_Short);
   Dprintsizeof(YAC_UnsignedShort);
   Dprintsizeof(YAC_Integer);
   Dprintsizeof(YAC_UnsignedInteger);
   Dprintsizeof(YAC_Long);
   Dprintsizeof(YAC_UnsignedLong);
   Dprintsizeof(YAC_Float);
   Dprintsizeof(YAC_Double);
   Dprintsizeof(YAC_Buffer);
   Dprintsizeof(TKS_ScriptClassInstance);
   Dprintsizeof(YAC_ClassArray);
   Dprintsizeof(TKS_Context);
   Dprintsizeof(YAC_HashTable);
   Dprintsizeof(YAC_List);
   Dprintsizeof(YAC_ListNode);
   Dprintsizeof(YAC_File);
   Dprintsizeof(YAC_StringArray);
   Dprintsizeof(PTN_DoubleArgExpr);
#endif // TKS_PRINT_SIZEOFS

   Dfixfpuflags;

   i_return      = 0;
   tkscript      = NULL;
   yac_host      = NULL;
   b_interactive = YAC_FALSE;

   if(1 == _argc)
   {
      // ---- no args, run "data.tkx" file ----
      NewEngine(_argc, _argv);

      if(!StartDefaultPakfile())
      {
         print_usage();
      }

      DeleteEngine();
      return 0;
   }
   else do
   {
      b_continue = YAC_FALSE;

      // ---- called with arguments ----
      NewEngine(_argc, _argv);

      /*int r=*/scan_ac(_argc, _argv);

      DeleteEngine();

      if(b_interactive)
      {
         char s_in[2];
         ::printf("\n<'q':quit|'r':run> : ");
         {
            int r = ::fscanf(stdin, "%1s", s_in);
            (void) r;
         }

         switch(s_in[0])
         {
         case 'q':
            b_continue = YAC_FALSE;
            break;

         case 'r':
            b_continue = YAC_TRUE;
            break;
         }
      }


   }
   while(b_continue);

   ::fflush(stdout);
   ::fflush(stderr);

   return i_return;
}

// Only use SDL_main for macOS platform
#ifndef YAC_MACOS
#ifdef DX_STATIC_TKSDL
#undef main
#endif
#endif

#ifdef TKS_LIB
void tks_lib_init(int _argc, char **_argv) {
   NewEngine(_argc, _argv, 1/*bRunInBG*/);
}
int tks_lib_run(const char *_pathName) {
   return ac_run(_pathName, YAC_TRUE/*bRunInBG*/);
}
void tks_lib_stop(void) {
   tkscript->stop();
   ::signal(SIGINT, SIG_DFL);
}
void tks_lib_exit(void) {
   DeleteEngine();
}
#ifdef TKS_VST
void tks_lib_vst_open_editor(YAC_ContextHandle _context) {
   tkscript->vstOpenEditor(_context);
}
void tks_lib_vst_close_editor(YAC_ContextHandle _context) {
   tkscript->vstCloseEditor(_context);
}
void tks_lib_vst_set_sample_rate(YAC_ContextHandle _context, sF32 _rate) {
   tkscript->vstSetSampleRate(_context, _rate);
}
void tks_lib_vst_set_bpm(YAC_ContextHandle _context, sF32 _bpm) {
   tkscript->vstSetBPM(_context, _bpm);
}
void tks_lib_vst_process_replacing(YAC_ContextHandle _context, YAC_Object *_inputs, YAC_Object *_outputs, sUI _numSampleFrames) {
   tkscript->vstProcessReplacing(_context, _inputs, _outputs, _numSampleFrames);
}
void tks_lib_vst_get_bank_chunk(YAC_ContextHandle _context, YAC_Object *_buffer) {
   tkscript->vstGetBankChunk(_context, _buffer);
}
void tks_lib_vst_get_program_chunk(YAC_ContextHandle _context, YAC_Object *_buffer) {
   tkscript->vstGetProgramChunk(_context, _buffer);
}
void tks_lib_vst_set_bank_chunk(YAC_ContextHandle _context, YAC_Object *_buffer) {
   tkscript->vstSetBankChunk(_context, _buffer);
}
void tks_lib_vst_set_program_chunk(YAC_ContextHandle _context, YAC_Object *_buffer) {
   tkscript->vstSetProgramChunk(_context, _buffer);
}
#endif // TKS_VST
#else
extern "C" {

   int main(int _argc, char **_argv) {

      int ret = tks_main(_argc, _argv);

      return ret;
   }
}
#endif // TKS_LIB
