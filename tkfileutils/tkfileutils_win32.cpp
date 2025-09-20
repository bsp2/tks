#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <yac.h>

#include <windows.h>

#include "tkfileutils.h"


void YAC_CALL tkfileutils_read_directory(YAC_String *_pathname, YAC_Value *_r) {

   static char buf[MAX_PATH];

   _r->initNull();

   if(YAC_Is_String(_pathname))
   {
      YAC_CAST_ARG(YAC_String, pathName, _pathname);

      //printf("xxx tkfileutils_read_directory: pathname=\"%s\".\n", pathName->chars);

      if(pathName->length > 0)
      {
         // Build win32-style pathname (c:\testdir\*)
         sUI i = pathName->length-1;
         memcpy(buf, pathName->chars, i);
         buf[i++] = '\\';
         buf[i++] = '*';
         buf[i++] = 0;

         WIN32_FIND_DATA fdat;
         HANDLE searchHandle = ::FindFirstFile((LPCSTR)buf, &fdat);

         if(INVALID_HANDLE_VALUE != searchHandle)
         {
            YAC_Object *a = YAC_New_StringArray();
            _r->initObject(a, 1);

            YAC_String ts;
            sUI idx = 0;

            sBool bHaveFile = 1;
            do
            {
               ts.empty();

               // Determine type of file record
               if(FILE_ATTRIBUTE_DIRECTORY == (fdat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
               {
                  ts.append("d ");
               }
               else
               {
                  // Regular file
                  ts.append("f ");
               }

               // Append file name
               ts.append("\"");
               ts.append((const char*)fdat.cFileName);
               ts.append("\" ");

               // Append file size (32bit)
               sprintf(buf, "%d", fdat.nFileSizeLow);
               ts.append(buf);

               // Append file modification date
               SYSTEMTIME sysTime;
               if(::FileTimeToSystemTime(&fdat.ftLastWriteTime, &sysTime))
               {
                  sprintf(buf, " %04d.%02d.%02d.%02d.%02d.%02d.%03d ",
                          sysTime.wYear, sysTime.wMonth, sysTime.wDay,
                          sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
                  ts.append(buf);
               }

               YAC_Value v;
               v.initString(&ts, 0);
               a->yacArraySet(NULL, idx, &v);

               // Next file record
               idx++;
               bHaveFile = (sBool) ::FindNextFile(searchHandle, &fdat);
            }
            while(bHaveFile);

            ::FindClose(searchHandle);
         }
         else
         {
            if(yac_host->yacGetDebugLevel() >= 10)
            {
               yac_host->printf("[---] tkfileutils_read_directory: FindFirstFile failed with error 0x%08x\n", ::GetLastError());
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

   GetCurrentDirectory(1024, (LPSTR) s->chars);

   // Generic
   // getcwd((char*) tkscript->configuration.tkp_dir.chars, 1024);

   s->fixLength();

   _r->initString(s, 1);
}

sBool YAC_CALL tkfileutils_setcwd(YAC_String *_pathName) {
   sSI r = 0;
   if(YAC_Is_String(_pathName))
   {
      YAC_String *s = (YAC_String *)_pathName;

      r = ::SetCurrentDirectory((LPSTR)s->chars);
   }
   return r;
}

sBool YAC_CALL tkfileutils_mkdir(YAC_String *_pathName) {
   sBool r = 0;
   if(YAC_Is_String(_pathName))
   {
      YAC_String *s = (YAC_String *)_pathName;

      r = ::CreateDirectory((LPSTR)s->chars, NULL/*lpSecurityAttributes*/);
   }
   return r;
}

sBool YAC_CALL tkfileutils_rm(YAC_String *_pathName) {
   sBool r = 0;
   if(YAC_Is_String(_pathName))
   {
      YAC_String *s = (YAC_String *)_pathName;

      r = ::DeleteFileA((LPSTR)s->chars);
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
