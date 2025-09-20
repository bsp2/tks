#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include <yac.h>

#ifdef YAC_MACOS
#include <sys/syslimits.h>
#endif // YAC_MACOS

#include "tkfileutils.h"


void YAC_CALL tkfileutils_read_directory(YAC_String *_pathname, YAC_Value *_r) {

   static char buf[PATH_MAX];

   static char absName[PATH_MAX];

   _r->initNull();

   if(YAC_Is_String(_pathname))
   {
      YAC_CAST_ARG(YAC_String, pathName, _pathname);

      //printf("xxx tkfileutils_read_directory: pathname=\"%s\".\n", pathName->chars);

      if(pathName->length > 0)
      {
         DIR *dirHandle = ::opendir((const char*)pathName->chars);

         if(NULL != dirHandle)
         {
            strcpy(absName, (const char*)pathName->chars);
            absName[pathName->length-1] = '/';

            YAC_Object *a = YAC_New_StringArray();
            _r->initObject(a, 1);

            YAC_String ts;
            sUI idx = 0;

            struct dirent *dirEntry = readdir(dirHandle);

            while(NULL != dirEntry)
            {
               ts.empty();

               sBool bSymLink = YAC_FALSE;

               // Determine type of file record
               if(DT_DIR == dirEntry->d_type)
               {
                  ts.append("d ");
               }
               else if(DT_LNK == dirEntry->d_type)
               {
                  ts.append("l ");
                  bSymLink = YAC_TRUE;
               }
               else
               {
                  ts.append("f ");
               }


               // Append file name
               ts.append("\"");
               ts.append((const char*)dirEntry->d_name);
               ts.append("\" ");

               // Build absolute pathname and query file attributes
               strcpy(absName+pathName->length-1+1, dirEntry->d_name);

               if(bSymLink)
               {
                  char symLinkTarget[PATH_MAX];
                  int numChars = ::readlink(absName, symLinkTarget, PATH_MAX);
                  if(numChars > 0)
                  {
                     symLinkTarget[numChars] = 0;
                     ts.append("\"");
                     ts.append(symLinkTarget);
                     ts.append("\" ");
                  }
               }

               // Append file size (32bit)
               struct stat st;
               ::lstat(absName, &st);
#ifdef YAC_MACOS
               snprintf(buf, PATH_MAX, "%lld", st.st_size);
#else
               snprintf(buf, PATH_MAX, "%lu", st.st_size);
#endif // YAC_MACOS
               ts.append(buf);

               // Append file modification date
               struct tm mtime;
               ::localtime_r(&st.st_mtime, &mtime);
               snprintf(buf, PATH_MAX, " %04d.%02d.%02d.%02d.%02d.%02d.%03d ",
                        1900+mtime.tm_year, 1+mtime.tm_mon, mtime.tm_mday,
                        mtime.tm_hour, mtime.tm_min, mtime.tm_sec,
                        0 // sysTime.wMilliseconds
                        );
               ts.append(buf);

               // Add entry to return array
               YAC_Value v;
               v.initString(&ts, 0);
               a->yacArraySet(NULL, idx, &v);

               // Next file record
               idx++;

               dirEntry = readdir(dirHandle);
            }

            ::closedir(dirHandle);
         }
         else
         {
            if(yac_host->yacGetDebugLevel() >= 10)
            {
               yac_host->printf("[~~~] tkfileutils_read_directory: opendir() failed with error %d(\"%s\")\n", errno, strerror(errno));
            }
         }
      }
      else
      {
         yac_host->printf("[---] tkfileutils_read_directory: pathname.length is 0.\n");
      }
   } // if pathname is string
}

void YAC_CALL tkfileutils_getcwd(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->alloc(1024);

   getcwd((char*)s->chars, 1024);

   s->fixLength();

   _r->initString(s, 1);
}

sBool YAC_CALL tkfileutils_setcwd(YAC_String *_pathName) {
   sSI r = 0;
   if(YAC_Is_String(_pathName))
   {
      YAC_String *s = (YAC_String *)_pathName;

      r = (0 != chdir((char*) s->chars));
   }
   return r;
}

sBool YAC_CALL tkfileutils_mkdir(YAC_String *_pathName) {
   sBool r = 0;
   if(YAC_Is_String(_pathName))
   {
      YAC_String *s = (YAC_String *)_pathName;

      r = ::mkdir((char*)s->chars, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
   }
   return r;
}

sBool YAC_CALL tkfileutils_rm(YAC_String *_pathName) {
   sBool r = 0;
   if(YAC_Is_String(_pathName))
   {
      YAC_String *s = (YAC_String *)_pathName;

      r = ::remove((const char*)s->chars);
   }
   return r;
}

sBool YAC_CALL tkfileutils_rename(YAC_String *_pathNameOld, YAC_String *_pathNameNew) {
   sBool r = 0;
   if(YAC_Is_String(_pathNameOld) && YAC_Is_String(_pathNameNew))
   {
      YAC_String *sOld = (YAC_String *)_pathNameOld;
      YAC_String *sNew = (YAC_String *)_pathNameNew;

      r = ::rename((const char*)sOld->chars,
                   (const char*)sNew->chars
                   );
   }
   return r;
}
