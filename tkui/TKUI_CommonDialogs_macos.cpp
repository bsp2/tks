
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>

#define YAC_PRINTF
#include <yac.h>

#include <math.h>
#include <stdlib.h>

#include "ying_ui.h"

#include "TKUI_CommonDialogs.h"

#define TKUI_MAX_PATHNAME 2048
#define TKUI_MAX_FILEFILTERSIZE 4096

extern std::vector<std::string>openFileDialog(char const * const aTitle ,
                                              char const * const aDefaultDirName ,
                                              char const * const aDefaultFileName ,
                                              const std::vector<std::string> & filters,
                                              bool bMultipleSelection,
                                              bool bDirectoriesOnly,
                                              bool bSave
                                              );


#if 0
static char file_filter_pairs[TKUI_MAX_FILEFILTERSIZE];
#endif


static char**ParseFilterNameStringPairs(YAC_Object*_filterNames, YAC_Object*_filterStrings) {
   char **caFilterNameStringPairs = NULL;

#if 0
   file_filter_pairs[0] = 0;
   file_filter_pairs[1] = 0;
#endif

   sUI i;
   if(YAC_VALID(_filterNames))
   {
      if(YAC_VALID(_filterStrings))
      {
         sUI numFilters = _filterNames->yacArrayGetNumElements();
         if(numFilters <= _filterStrings->yacArrayGetNumElements())
         {
            // ---- test whether each array elements is a (valid) String
            // ---- then extract the char pointers of the strings
            caFilterNameStringPairs = new char*[(numFilters << 1) + 2u];
            for(i = 0u; i < numFilters; i++)
            {
               YAC_Value v;
               _filterNames->yacArrayGet(NULL, i, &v); // xxx TKS_MT: should use real thread context (exceptions..)
               if(v.type >= YAC_TYPE_OBJECT)
               {
                  if(!YAC_BCHK(v.value.object_val, YAC_CLID_STRING))
                  {
                     if(!YAC_VALID(v.value.object_val))
                     {
                        yac_host->printf("[---] TKUI_OpenFileDialog_macos: _filterStrings[%i]==*ILL*\n", i);
                     }
                     else
                     {
                        yac_host->printf("[---] TKUI_OpenFileDialog_macos: _filterStrings[%i] is not a String.\n", i);
                     }
                     delete [] caFilterNameStringPairs;
                     return NULL;
                  }
                  YAC_String *s = v.value.string_val;
                  caFilterNameStringPairs[(i << 1) + 0u] = (char*)s->chars;
               }
               else
               {
                  yac_host->printf("[---] TKUI_OpenFileDialog_macos: _filterStrings[%i] is not a String.\n", i);
                  delete [] caFilterNameStringPairs;
                  return NULL;
               }

               v.unset(); // valid to unset this yacArrayGetElement must never return "new" objects

               _filterStrings->yacArrayGet(NULL, i, &v); // xxx TKS_MT: should use real thread context (exceptions..)
               if(v.type >= YAC_TYPE_OBJECT)
               {
                  if(!YAC_BCHK(v.value.object_val, YAC_CLID_STRING))
                  {
                     if(!YAC_VALID(v.value.object_val))
                     {
                        yac_host->printf("[---] TKUI_OpenFileDialog_macos: _filterNames[%i]==*ILL*\n", i);
                     }
                     else
                     {
                        yac_host->printf("[---] TKUI_OpenFileDialog_macos: _filterNames[%i] is not a String.\n", i);
                     }
                     delete [] caFilterNameStringPairs;
                     return NULL;
                  }
                  YAC_String *s = v.value.string_val;
                  if(s->length >= 3 && ('*' == s->chars[0]) && ('.' == s->chars[1]))
                     caFilterNameStringPairs[(i << 1) + 1u] = (char*)s->chars + 2;  // "*.txt" => "txt"
                  else
                     caFilterNameStringPairs[(i << 1) + 1u] = (char*)s->chars;  // should not be reachable
               }
               else
               {
                  yac_host->printf("[---] TKUI_OpenFileDialog_macos: _filterNames[%i] is not a String.\n", i);
                  delete [] caFilterNameStringPairs;
                  return NULL;
               }
            } // for numFilters

#if 0
            // ---- now that we collected all name/filter char pointers build one big string out of it
            // ---- the code above may still be usuable for other GUI toolkits (i.e. those using an array of char*)
            sUI k = 0u;
            for(i = 0u; i < numFilters; i++)
            {
               // append filter name
               char *s = caFilterNameStringPairs[(i << 1) + 0u]; // append filter name, e.g."C sources"
               while(*s)
               {
                  file_filter_pairs[k++] = *s++;
               }
               file_filter_pairs[k++] = 0;
               s = caFilterNameStringPairs[(i << 1) + 1u]; // append filter string "*.h;*.c"
               while(*s)
               {
                  file_filter_pairs[k++] = *s++;
               }
               file_filter_pairs[k++] = 0;
            }
            file_filter_pairs[k++] = 0;
            file_filter_pairs[k++] = 0;
#endif
         }
      }
      else
      {
         if(!YAC_VALID(_filterStrings))
         {
            yac_host->printf("[---] TKUI_OpenFileDialog_macos: _filterStrings==*ILL*\n");
            return NULL;
         }
         else
         {
            // ignore filter settings
         }
      }
   }
   else
   {
      // ---- ignore filter settings
      if(!YAC_VALID(_filterNames))
      {
         yac_host->printf("[---] TKUI_OpenFileDialog_macos: _filterNames==*ILL*\n");
         return NULL;
      }
   }

   return caFilterNameStringPairs;
}


static void loc_FileDialog(YAC_Object *_suggestedPath,
                           YAC_Object *_suggestedFile,
                           YAC_Object *_windowTitle,
                           YAC_Object *_filterNames,
                           YAC_Object *_filterStrings,
                           sBool _bMultipleSelection,
                           sBool _bDirectoriesOnly,
                           sBool _bSave,
                           YAC_Value *_r
                           ) {

   _r->initNull();

   char**caFilterNameStringPairs = ParseFilterNameStringPairs(_filterNames, _filterStrings);

   YAC_String sSuggestedDirName;
   if(YAC_Is_String(_suggestedPath))
   {
      YAC_CAST_ARG(YAC_String, suggestedPath, _suggestedPath);
      if(suggestedPath->length > 1u)
      {
         sSuggestedDirName.visit((const char*)suggestedPath->chars);
      }
   }

   YAC_String sSuggestedFileName;
   if(YAC_Is_String(_suggestedFile))
   {
      YAC_CAST_ARG(YAC_String, suggestedFile, _suggestedFile);
      if(suggestedFile->length > 1u)
      {
         sSuggestedFileName.visit((const char*)suggestedFile->chars);
      }
   }

   // (note) must not set a default file in save dialogs on macOS
   //         (dialogs shows error message otherwise)
   // if(YAC_Is_String(_suggestedFile) && !_bSave)
   // {
   //    YAC_CAST_ARG(YAC_String, suggestedFile, _suggestedFile);
   //    if(sSuggestedPathName.length > 1)
   //       sSuggestedPathName.append("/");
   //    sSuggestedPathName.append(suggestedFile);
   // }

   const char *suggestedDirNameChars = (const char*)sSuggestedDirName.chars;
   const char *suggestedFileNameChars = (const char*)sSuggestedFileName.chars;
#if 0
   yac_host->printf("[trc] TKUI_CommonDialogs_macos: suggestedDirNameChars=\"%s\"\n", suggestedDirNameChars);
   yac_host->printf("[trc] TKUI_CommonDialogs_macos: suggestedFileNameChars=\"%s\"\n", suggestedFileNameChars);
#endif

   const char *windowTitleChars = "OpenFileDialog";
   if(YAC_Is_String(_windowTitle))
   {
      YAC_CAST_ARG(YAC_String, windowTitle, _windowTitle);
      windowTitleChars = (const char*)windowTitle->chars;
   }

   std::vector<std::string> filters;
#if 1
   if(NULL != caFilterNameStringPairs)
   {
      char **s = caFilterNameStringPairs;
      while(NULL != s[1])
      {
         // yac_host->printf("adding filter \"%s\"\n", s[1]);
         filters.push_back(std::string(s[1]));
         s += 2;
      }
   }
// #else
   // filters.push_back(std::string(""));  // => matches files with no suffix
#endif

   std::vector<std::string> files = openFileDialog(windowTitleChars,
                                                   suggestedDirNameChars,
                                                   suggestedFileNameChars,
                                                   filters,
                                                   (0 != _bMultipleSelection),
                                                   (0 != _bDirectoriesOnly),
                                                   (0 != _bSave)
                                                   );

   sUI numFiles = files.size();
   if(numFiles > 0u)
   {
      if(_bSave || !_bMultipleSelection)
      {
         _r->initEmptyString();
         std::string stdStr = files.at(0);
         YAC_String t;
         t.visit(stdStr.c_str());
         _r->value.string_val->yacCopy(&t);
      }
      else
      {
         YAC_StringArray *a = YAC_New_StringArray();
         a->yacArrayAlloc(numFiles,1,0,0);

         for(sUI i = 0u; i < numFiles; i++)
         {
            std::string stdStr = files.at(i);
            YAC_String *s = YAC_New_String();
            YAC_Value v;
            v.initString(s, 1);
            YAC_String t;
            t.visit(stdStr.c_str());
            s->yacCopy(&t);
            a->yacArraySet(NULL/*context*/, i, &v);
            v.unset();
         }
         _r->initObject(a, 1);
      }
   }

   if(NULL != caFilterNameStringPairs)
   {
      delete [] caFilterNameStringPairs;
   }
}


void TKUI_OpenFileDialog(YAC_Object *_suggestedPath,
                         YAC_Object *_suggestedFile,
                         YAC_Object *_windowTitle,
                         YAC_Object *_filterNames,
                         YAC_Object *_filterStrings,
                         sSI _bMultipleSelection,
                         sSI _bDirectoriesOnly,
                         YAC_Value *_r
                         ) {

   loc_FileDialog(_suggestedPath,
                  _suggestedFile,
                  _windowTitle,
                  _filterNames,
                  _filterStrings,
                  _bMultipleSelection,
                  _bDirectoriesOnly,
                  YAC_FALSE/*bSave*/,
                  _r
                  );
}

void TKUI_SaveFileDialog(YAC_Object *_suggestedPath,
                         YAC_Object *_suggestedFile,
                         YAC_Object *_windowTitle,
                         YAC_Object *_filterNames,
                         YAC_Object *_filterStrings,
                         sSI _bDirectoriesOnly,
                         YAC_Value *_r
                         ) {

   loc_FileDialog(_suggestedPath,
                  _suggestedFile,
                  _windowTitle,
                  _filterNames,
                  _filterStrings,
                  YAC_FALSE/*bMultipleSelection*/,
                  _bDirectoriesOnly,
                  YAC_TRUE/*bSave*/,
                  _r
                  );
}
