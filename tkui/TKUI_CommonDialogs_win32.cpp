// ----
// ----    file: TKUI_CommonDialogs_win32.cpp
// ----  author: (C) 2005-2020 Bastian Spiegel <bs@tkscript.de>
// ----    date: 13Jun2005, 16Jun2005, 13Nov2009, 19Jun2020
// ---- license: Distributed under terms of the Lesser General Public License (LGPL). 
// ----          See <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
// ----
// ----    note: 
// ----       13Nov2009: OpenFileDialog: since a recent Vista update, the suggestedFile name must not contain directory separator chars
// ----       19Jun2020: don't delete suggestedPath tmp string until dialog has closed
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>

#define YAC_PRINTF
#include <yac.h>

#include <math.h>
#include <stdlib.h>


#include <windows.h>

#include "ying_ui.h"

#include "TKUI_CommonDialogs.h"

#define TKUI_MAX_PATHNAME 2048
// #define TKUI_DIR_SEPARATOR '/'
#define TKUI_DIR_SEPARATOR '\\'
#define TKUI_MAX_FILEFILTERSIZE 4096

static char file_name[TKUI_MAX_PATHNAME];
static int file_name_offset;

static char file_filter_pairs[TKUI_MAX_FILEFILTERSIZE];


static void BuildPathName(YAC_Object *_path, YAC_Object *_file) {
   sUI i, k=0;
   if(YAC_BCHK(_path, YAC_CLID_STRING))
   {
      YAC_String *s=(YAC_String*)_path;
      if(s->chars&&(s->length>0))
      {
         for(i=0; i<(s->length-1); i++)
         {
            if(s->chars[i] == '/')
               file_name[k++] = '\\';
            else
               file_name[k++] = s->chars[i];
         }
      }
      if(k>0)
      {
         file_name[k++]=TKUI_DIR_SEPARATOR;
      }
   }
   file_name_offset=k;
   if(YAC_BCHK(_file, YAC_CLID_STRING))
   {
      YAC_String *s=(YAC_String*)_file;
      if(s->chars&&(s->length>0))
      {
         for(i=0; i<(s->length-1); i++)
         {
            if(s->chars[i] == '/')
               file_name[k++] = '\\';
            else
               file_name[k++] = s->chars[i];
         }
      }
   }
   file_name[k++]=0;
   yac_host->printf("[trc] TKUI_CommonDialog_win32: BuildPathName: file_name=\"%s\"\n", file_name);
}

static char**ParseFilterNameStringPairs(YAC_Object*_filterNames, YAC_Object*_filterStrings) {
   char **caFilterNameStringPairs=NULL;

   file_filter_pairs[0]=0;
   file_filter_pairs[1]=0;

   sUI i;
   if(YAC_VALID(_filterNames))
   {
      if(YAC_VALID(_filterStrings))
      {
         sUI numFilters=_filterNames->yacArrayGetNumElements();
         if(numFilters <= _filterStrings->yacArrayGetNumElements())
         {
            // ---- test whether each array elements is a (valid) String
            // ---- then extract the char pointers of the strings
            caFilterNameStringPairs=new char*[numFilters*2+2];
            for(i=0; i<numFilters; i++)
            {
               YAC_Value v;
               _filterNames->yacArrayGet(NULL, i, &v); // xxx TKS_MT: should use real thread context (exceptions..)
               if(v.type>=YAC_TYPE_OBJECT)
               {
                  if( ! YAC_BCHK(v.value.object_val, YAC_CLID_STRING))
                  {
                     if(! YAC_VALID(v.value.object_val))
                     {
                        yac_host->printf("[---] TKUI_OpenFileDialog_win32: _filterStrings[%i]==*ILL*\n", i);
                     }
                     else
                     {
                        yac_host->printf("[---] TKUI_OpenFileDialog_win32: _filterStrings[%i] is not a String.\n", i);
                     }
                     delete [] caFilterNameStringPairs;
                     return 0;
                  }
                  YAC_String *s=v.value.string_val;
                  caFilterNameStringPairs[(i<<1)+0]=(char*)s->chars;
               }
               else
               {
                  yac_host->printf("[---] TKUI_OpenFileDialog_win32: _filterStrings[%i] is not a String.\n", i);
                  delete [] caFilterNameStringPairs;
                  return 0;
               }
               
               v.unset(); // valid to unset this yacArrayGetElement must never return "new" objects
               
               _filterStrings->yacArrayGet(NULL, i, &v); // xxx TKS_MT: should use real thread context (exceptions..)
               if(v.type>=YAC_TYPE_OBJECT)
               {
                  if( ! YAC_BCHK(v.value.object_val, YAC_CLID_STRING))
                  {
                     if( !YAC_VALID(v.value.object_val))
                     {
                        yac_host->printf("[---] TKUI_OpenFileDialog_win32: _filterNames[%i]==*ILL*\n", i);
                     }
                     else
                     {
                        yac_host->printf("[---] TKUI_OpenFileDialog_win32: _filterNames[%i] is not a String.\n", i);
                     }
                     delete [] caFilterNameStringPairs;
                     return 0;
                  }
                  YAC_String *s=v.value.string_val;
                  caFilterNameStringPairs[(i<<1)+1]=(char*)s->chars;
               }
               else
               {
                  yac_host->printf("[---] TKUI_OpenFileDialog_win32: _filterNames[%i] is not a String.\n", i);
                  delete [] caFilterNameStringPairs;
                  return 0;
               }
            } // for numFilters

            // ---- now that we collected all name/filter char pointers build one big string out of it
            // ---- the code above may still be usuable for other GUI toolkits (i.e. those using an array of char*)
            sUI k=0;
            for(i=0; i<numFilters; i++)
            {
               // append filter name
               char *s=caFilterNameStringPairs[(i<<1)+0]; // append filter name, e.g."C sources"
               while(*s)
               {
                  file_filter_pairs[k++]=*s++;  
               }
               file_filter_pairs[k++]=0;
               s=caFilterNameStringPairs[(i<<1)+1]; // append filter string "*.h;*.c"
               while(*s)
               {
                  file_filter_pairs[k++]=*s++;
               }
               file_filter_pairs[k++]=0;
            }
            file_filter_pairs[k++]=0;
            file_filter_pairs[k++]=0;
         }
      }
      else
      {
         if(!YAC_VALID(_filterStrings))
         {
            yac_host->printf("[---] TKUI_OpenFileDialog_win32: _filterStrings==*ILL*\n");
            return 0;;
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
         yac_host->printf("[---] TKUI_OpenFileDialog_win32: _filterNames==*ILL*\n");
         return 0;
      }
   }

   return caFilterNameStringPairs;
}


void TKUI_OpenFileDialog(
                         YAC_Object *_suggestedPath, // initial directory
                         YAC_Object *_suggestedFile, // name of last selected file 
                         YAC_Object *_windowTitle,   // caption of the (modal) dialog window
                         YAC_Object *_filterNames,   // array of filter names. each array index must have a corresponding element in the _filterStrings array.
                         YAC_Object *_filterStrings, // array of filter strings. Each String may contain a ';' separated list of file filters, e.g. "*.h;*.cpp;*.cc".
                         sSI         _bMultipleSelection, // 0=single selection, 1=multiple selection
                         sSI         _bDirectoriesOnly,   // 0=also allow file selections, 1=only select directories
                         YAC_Value  *_r                   // stores the returned StringArray of selected file(s) (including full pathnames)
                         ) 
{
   // ---- Open the standard win32 "old-style" file dialog. This should work on virtually any win32 platform.

   // yac_host->printf("xxx TKUI_OpenFileDialog: debug 1\n");

   _r->initObject(0,0); // todo: init with yac_null (initNull()) once it is available in plugins

   OPENFILENAME ofn;
   ::memset((void*)&ofn, 0, sizeof(OPENFILENAME));
   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner   = NULL; // no parent
   ofn.hInstance   = 0; //  OFN_ENABLETEMPLATEHANDLE|OFN_ENABLETEMPLATE|OFN_EXPLORER 
   
   // ---- handle filter settings
   char**caFilterNameStringPairs=ParseFilterNameStringPairs(_filterNames, _filterStrings);
   ofn.lpstrFilter=(const char*)file_filter_pairs;

   // ---- custom user filters
   ofn.lpstrCustomFilter = NULL; // don't remember user defined filters
   ofn.nMaxCustFilter    = 0;

   // ---- initial file
   BuildPathName(0, _suggestedFile);

   ofn.lpstrFile      = file_name; // returns full pathname on success
   ofn.nMaxFile       = TKUI_MAX_PATHNAME;
   ofn.lpstrFileTitle = NULL;
   ofn.nMaxFileTitle  = 0;

   // ---- initial directory
   ofn.lpstrInitialDir=NULL;
   YAC_String *sSuggestedPath = NULL;
   if(YAC_Is_String(_suggestedPath))
   {
      sSuggestedPath = YAC_New_String();
      sSuggestedPath->yacCopy(_suggestedPath);
      if(sSuggestedPath->length > 0u)
      {
         for(sUI i = 0u; i < sSuggestedPath->length; i++)
         {
            ////yac_host->printf("chars[%i]=%c\n", i, s->chars[i]);
            if(sSuggestedPath->chars[i] == '/')
               sSuggestedPath->chars[i] = '\\';
         }
         ofn.lpstrInitialDir = (char*)sSuggestedPath->chars;
         yac_host->printf("[trc] TKUI_CommonDialog_win32: suggestedDir=\"%s\"\n", ofn.lpstrInitialDir);
      }
   }

   // ---- dialog title string
   ofn.lpstrTitle = NULL; // default title, "Open", "Save as"..
   if(YAC_Is_String(_windowTitle))
   {
      YAC_String *s = (YAC_String *)_windowTitle;
      if(s->length > 0)
      {
         ofn.lpstrTitle = (char*) s->chars;
      }
   }

   // ---- Additional flags
   ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER  | OFN_LONGNAMES /*|OFN_NOCHANGEDIR*/ | OFN_PATHMUSTEXIST;
   ////ofn.Flags=OFN_ENABLESIZING|OFN_EXPLORER|OFN_LONGNAMES; 
   if(_bMultipleSelection)
   {
      ofn.Flags |= OFN_ALLOWMULTISELECT;
   }
   if(!_bDirectoriesOnly)
   {
      ofn.Flags |= OFN_FILEMUSTEXIST;
   }
      
   // ---- Fill out remaining fields (according to http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/userinput/commondialogboxlibrary/commondialogboxreference/commondialogboxstructures/openfilename.asp)
   ofn.nFileOffset    = file_name_offset;
   ofn.nFileExtension = 0; // ..?
   ofn.lpstrDefExt    = NULL;
   ofn.lCustData      = 0;
   ofn.lpfnHook       = NULL;
   ofn.lpTemplateName = NULL;

   // ---- Actually open the file dialog
   if(GetOpenFileName(&ofn))
   {
      ////yac_host->printf("GetOpenFileName() ok\n");
      if(!_bMultipleSelection)
      {
         YAC_String *s=YAC_New_String();
         // ---- return String
         YAC_String t; t.visit(file_name);
         s->yacCopy(&t);
         _r->initString(s, 1);
      }
      else
      {
         ////yac_host->printf("GetOpenFileName() multiplesel. ok\n");
         // ---- return StringArray
         // ---- count the number of returned file names
         sUI i=0;
         sUI k=0;
         char *d=file_name;
         sBool bcont=1;
         while(bcont)
         {
            if(d[0]=='\\')
               d[0]='/';
            if(d[0]==0)
            {
               if(k==0)
                  k=sUI(size_t(d-file_name));
               i++;
               bcont=(d[1]!=0);
            }
            d++;
         }
         if(i>1)
         {
            d=file_name+k+1;
            YAC_StringArray *a=YAC_New_StringArray();
            sUI l=i-1;
            a->yacArrayAlloc(l,1,0,0);
            for(i=0; i<l; i++)
            {
               YAC_Value v;
               YAC_String *s=YAC_New_String();
               char *f=file_name, *e=d;
               sUI j=0;
               while(*e++) // determine current file name length
                  j++;
               s->yacArrayAlloc(k+j+1+1); // plus '/' + '\0'

               e = (char*)s->chars;
               // ---- append path name
               for(sUI x=0; x<k; x++)
                  *e++=*f++;
               *e++='/';
               // ---- append current file name
               while(j-->0)
               {
                  *e++=*d++;
               }
               d++;
               *e++=0;
               s->length = (sUI) (e-(char*)s->chars);
               s->key = YAC_LOSTKEY;
               v.initString(s, 1);
               a->yacArraySet(NULL, i, &v); // xxx TKS_MT: should use real thread context (exceptions..)
               v.unset();
            }
            _r->initObject(a, 1);
         }
         else
         {
            YAC_StringArray *a=YAC_New_StringArray();
            a->yacArrayAlloc(1,1,0,0);
            YAC_String *s=YAC_New_String();
            YAC_String t; t.visit(file_name);
            s->yacCopy(&t);
            YAC_Value v; v.initString(s, 1); a->yacArraySet(NULL, 0, &v); // xxx TKS_MT: should use real thread context (exceptions..)
            _r->initObject(a, 1);
         }
      }
   }
   else
   {
      yac_host->printf("[---] tkui::OpenFileDialog: native GetOpenFileName() call failed with CommDlgExtendedError=%d\n", CommDlgExtendedError());
   }

   YAC_DELETE(sSuggestedPath);

   if(caFilterNameStringPairs)
   {
      delete [] caFilterNameStringPairs;
   }
   
}


void TKUI_SaveFileDialog(
                         YAC_Object *_suggestedPath, // initial directory
                         YAC_Object *_suggestedFile, // suggested name for saved file
                         YAC_Object *_windowTitle,   // caption of the (modal) dialog window
                         YAC_Object *_filterNames,   // array of filter names. each array index must have a corresponding element in the _filterStrings array.
                         YAC_Object *_filterStrings, // array of filter strings. Each String may contain a ';' separated list of file filters, e.g. "*.h;*.cpp;*.cc".
                         sSI         _bDirectoriesOnly,   // 0=save to a file, 1=save to a directory/create directory
                         YAC_Value  *_r                   // returns the selected file name (String) or null indicating that the user cancelled the dialog
)
{
   // ---- Open the standard win32 "old-style" file dialog. This should work on virtually any win32 platform.
   _r->initObject(0,0); // todo: init with yac_null (initNull()) once it is available in plugins

   OPENFILENAME ofn;
   ::memset((void*)&ofn, 0, sizeof(OPENFILENAME));
   ofn.lStructSize=sizeof(OPENFILENAME);
   ofn.hwndOwner=NULL; // no parent
   ofn.hInstance=0; //  OFN_ENABLETEMPLATEHANDLE|OFN_ENABLETEMPLATE|OFN_EXPLORER 
   
   // ---- handle filter settings
   char**caFilterNameStringPairs=ParseFilterNameStringPairs(_filterNames, _filterStrings);
   ofn.lpstrFilter=(const char*)file_filter_pairs;

   // ---- custom user filters
   ofn.lpstrCustomFilter=NULL; // don't remember user defined filters
   ofn.nMaxCustFilter=0;

   // ---- initial file
   BuildPathName(0, _suggestedFile);
   ofn.lpstrFile=file_name; // returns full pathname on success
   ofn.nMaxFile=TKUI_MAX_PATHNAME;
   ofn.lpstrFileTitle=NULL;
   ofn.nMaxFileTitle=0;

   // ---- initial directory
   ofn.lpstrInitialDir=NULL;
   if(YAC_Is_String(_suggestedPath))
   {
      YAC_String *s=YAC_New_String();
      s->yacCopy(_suggestedPath);
      if(s->length>0)
      {
         for(sUI i=0; i<s->length; i++)
         {
            ////yac_host->printf("chars[%i]=%c\n", i, s->chars[i]);
            if(s->chars[i]=='/')
               s->chars[i]='\\';
         }
         ofn.lpstrInitialDir=(char*)s->chars;
         ////yac_host->printf("_suggestedPath=%s\n", ofn.lpstrInitialDir);
      }
      delete s;
   }

   // ---- dialog title string
   ofn.lpstrTitle=NULL; // default title, "Open", "Save as"..
   if(YAC_Is_String(_windowTitle))
   {
      YAC_String *s=(YAC_String *)_windowTitle;
      if(s->length>0)
      {
         ofn.lpstrTitle = (char*) s->chars;
      }
   }

   // ---- Additional flags
   ofn.Flags=OFN_ENABLESIZING|OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_LONGNAMES/*|OFN_NOCHANGEDIR*/|OFN_PATHMUSTEXIST;
   ////if(_bDirectoriesOnly)
      
   // ---- Fill out remaining fields (according to http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/userinput/commondialogboxlibrary/commondialogboxreference/commondialogboxstructures/openfilename.asp)
   ofn.nFileOffset=file_name_offset;
   ofn.nFileExtension=0; // ..?
   ofn.lpstrDefExt=NULL;
   ofn.lCustData=0;
   ofn.lpfnHook=NULL;
   ofn.lpTemplateName=NULL;

   // ---- Actually open the file dialog
   if(GetSaveFileName(&ofn))
   {
      ////yac_host->printf("GetSaveFileName() ok\n");
      YAC_String *s=YAC_New_String();
      // ---- return String
      YAC_String t; t.visit(file_name);
      s->yacCopy(&t);
      _r->initString(s, 1);
   }

   if(caFilterNameStringPairs)
   {
      delete [] caFilterNameStringPairs;
   }
}

