/// TKS_FunctionProfiler.cpp
///
/// (c) 2010-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#ifdef TKS_PROFILE_FUNCTIONS

#include "TKS_Mutex.h"
#include "YAC_PointerArray.h"

#include "tks_inc_function.h"
#include "tks_inc_class.h"
#include "tks_inc_compiler.h"

#include "TKS_FunctionProfiler.h"

#define MAX_RESULT_LINES 128

// Only include functions that were run for longer than this period of time
#define MILLISEC_THRESHOLD 1.0

#define MAX_SCRIPT_RESULTS 16384
#define MAX_NATIVE_RESULTS 16384

#define NAME_COLUMN_WIDTH 48


static void AddPaddedString(YAC_String *buf, YAC_String *s, sSI pad) {
   buf->append(s);
   sSI sc = pad - s->length;
   if(sc > 0)
   {
      for(sSI i = 0; i < sc; i++)
      {
         buf->append(" ");
      }
   }
}

void TKS_FunctionProfiler::AppendScriptResults (YAC_String &buf, Array<const PTN_Function*>&a, const char *sortTypeName) {
   sUI i;
   buf.append("\n\n---------------------------------------------------------------------------------------------\n");
   buf.append("Profile result (top script functions/methods by ");
   buf.append(sortTypeName);
   buf.append("):\n");
   buf.append("---------------------------------------------------------------------------------------------\n");
   buf.append("  <rank> <name>                                                 <millisec>\t<callcount>\n");
   YAC_String t;
   t.alloc(256);
   for(i = 0u; i < a.num_elements && i < MAX_RESULT_LINES; i++)
   {
      const PTN_Function *f = a.elements[i];

      // Rank
      t.printf("  %05d.: ", i+1);
      buf.append(&t);

      // Name
      YAC_String name;
      if(f->num_local_vars > 0)
      {
         // Note: profile results for recursive functions are not really correct
         name.append("*");
      }
      if(NULL != f->nspace)
      {
         name.append(&f->nspace->name);
         name.append("::");
      }
      if(NULL != f->parent_class)
      {
         name.append(&f->parent_class->name);
         name.append("::");
      }
      name.append(&f->name);
      AddPaddedString(&buf, &name, NAME_COLUMN_WIDTH);

      // Milliseconds
      t.printf("\t%f\t", f->profile_millisec);
      buf.append(&t);

      // Callcount
      t.printf("%u", f->profile_callcount);
      buf.append(&t);

      buf.append("\n");
   }
}

void TKS_FunctionProfiler::PrintScriptResults(void) {

   Array<const PTN_Function*> a;
   a.alloc(MAX_SCRIPT_RESULTS);

   // Iterate modules
   const StaticList::Node *cn = Dtkscompiler->compiled_modules;
   while(NULL != cn)
   {
      const TKS_CachedScript *cs = (TKS_CachedScript*) cn->data;

      // Iterate module function
      sUI i;
      for(i = 0u; i < cs->functions.max_entries; i++)
      {
         TKS_CachedObject *co = cs->functions.objects[i];
         if(NULL != co)
         {
            const PTN_Function *f = (PTN_Function *) co->value.object_val;
            if(f->profile_millisec > MILLISEC_THRESHOLD)
            {
               a.elements[a.num_elements++] = f;
            }
         }
      }

      // Next module
      cn = cn->next;
   }

   // Iterate script classes
   sUI ci;
   sUI numClasses = Dtkscompiler->class_decls.num_elements;
   for(ci = 0u; ci < numClasses; ci++)
   {
      const TKS_ClassDecl *cd = Dtkscompiler->class_decls.elements[ci];
      if(cd->isUsed())
      {
         // Iterate static class methods
         if(NULL != cd->functions)
         {
            sUI fi;
            for(fi=0; fi<cd->functions->max_entries; fi++)
            {
               const TKS_CachedObject *co = cd->functions->objects[fi];
               if(NULL != co)
               {
                  const PTN_Function *f = (PTN_Function *) co->value.object_val;
                  if(f->profile_millisec > MILLISEC_THRESHOLD)
                  {
                     a.elements[a.num_elements++] = f;
                  }
               }
            }
         }

         // Iterate class methods
         {
            sUI fi;
            for(fi=0; fi<cd->methods.num_elements; fi++)
            {
               const TKS_ClassDeclMethod *cdm = cd->methods.elements[fi];
               const PTN_Function *f = &cdm->function;
               if(f->profile_millisec > MILLISEC_THRESHOLD)
               {
                  a.elements[a.num_elements++] = f;
               }
            }
         }
      }
   }

   YAC_String buf;

   // Sort result array by millisec
   if(a.num_elements > 1u)
   {
      sBool bSwapped;
      do
      {
         bSwapped = YAC_FALSE;

         sUI i;
         for(i=0; i<a.num_elements-1; i++)
         {
            const PTN_Function *fc = a.elements[i];
            const PTN_Function *fn = a.elements[i+1];

            if(fc->profile_millisec < fn->profile_millisec)
            {
               a.elements[i]   = fn;
               a.elements[i+1] = fc;
               bSwapped = YAC_TRUE;
            }
         }
      } while(bSwapped);
   }
   AppendScriptResults(buf, a, "millisec");

   // Sort result array by callcount
   if(a.num_elements > 1u)
   {
      sBool bSwapped;
      do
      {
         bSwapped = YAC_FALSE;

         sUI i;
         for(i=0; i<a.num_elements-1; i++)
         {
            const PTN_Function *fc = a.elements[i];
            const PTN_Function *fn = a.elements[i+1];

            if(fc->profile_callcount < fn->profile_callcount)
            {
               a.elements[i]   = fn;
               a.elements[i+1] = fc;
               bSwapped = YAC_TRUE;
            }
         }
      } while(bSwapped);
   }
   AppendScriptResults(buf, a, "callcount");

   a.free();

   // Print to stderr
   tks_fputs2((const char*) buf.chars);
}

void TKS_FunctionProfiler::AppendNativeResults (YAC_String &buf, Array<const YAC_CommandY*>&a, const char *sortTypeName) {
   buf.append("\n\n---------------------------------------------------------------------------------------------\n");
   buf.append("Profile result (top native functions/methods by ");
   buf.append(sortTypeName);
   buf.append("):\n");
   buf.append("---------------------------------------------------------------------------------------------\n");
   buf.append("  <rank> <name>                                                 <millisec>\t<callcount>\n");
   YAC_String t;
   t.alloc(256);
   sUI i;
   for(i = 0u; i < a.num_elements && i < MAX_RESULT_LINES; i++)
   {
      const YAC_CommandY *cy = a.elements[i];

      // Rank
      t.printf("  %05d.: ", i+1);
      buf.append(&t);

      // Name
      YAC_String name;
      name.append(cy->profile_classname);
      name.append("::");
      name.append(cy->y_name);
      AddPaddedString(&buf, &name, NAME_COLUMN_WIDTH);

      // Millisec
      t.printf("\t%f\t", cy->profile_millisec);
      buf.append(&t);

      // Callcount
      t.printf("%u", cy->profile_callcount);
      buf.append(&t);

      buf.append("\n");
   }
}

void TKS_FunctionProfiler::PrintNativeResults(void) {

   Array<const YAC_CommandY*> a;
   a.alloc(MAX_SCRIPT_RESULTS);

   // Iterate functions
   sUI fi;
   for(fi = 0u; fi < tkscript->yac_functions.max_entries; fi++)
   {
      TKS_CachedObject *co = tkscript->yac_functions.objects[fi];
      if(NULL != co)
      {
         YAC_CommandY *cy = (YAC_CommandY *) co->value.object_val;

         if(cy->profile_millisec > MILLISEC_THRESHOLD)
         {
            a.elements[a.num_elements++] = cy;
         }
      }
   }

   // Iterate classes
   sUI ci;
   for(ci = 0u; ci < YAC_MAX_CLASSES; ci++)
   {
      const TKS_ClassTemplate *ct = TKSCRIPT__ATOM_SPEEDPATH[ci];
      if(NULL != ct)
      {
         const YAC_CommandListY *cmds = &ct->commandsy;
         if(cmds->y_num_commands > 0u)
         {
            // Iterate methods
            sUI mi;
            for(mi=0; mi<cmds->y_num_commands; mi++)
            {
               const YAC_CommandY *cy = &cmds->y_commands[mi];

               if(cy->profile_millisec > MILLISEC_THRESHOLD)
               {
                  a.elements[a.num_elements++] = cy;
               }
            }
         }
      }
   }

   // Sort result array by millisec
   if(a.num_elements > 1u)
   {
      sBool bSwapped;
      do
      {
         bSwapped = YAC_FALSE;

         sUI i;
         for(i = 0u; i < a.num_elements - 1u; i++)
         {
            const YAC_CommandY *cc = a.elements[i];
            const YAC_CommandY *cn = a.elements[i+1];

            if(cc->profile_millisec < cn->profile_millisec)
            {
               a.elements[i]   = cn;
               a.elements[i+1] = cc;
               bSwapped = YAC_TRUE;
            }
         }
      } while(bSwapped);
   }

   YAC_String buf;

   // Print results sorted by milliseconds
   AppendNativeResults(buf, a, "millisec");

   // Sort result array by callcount
   if(a.num_elements > 1u)
   {
      sBool bSwapped;
      do
      {
         bSwapped = YAC_FALSE;

         sUI i;
         for(i = 0u; i < a.num_elements - 1u; i++)
         {
            const YAC_CommandY *cc = a.elements[i];
            const YAC_CommandY *cn = a.elements[i+1];

            if(cc->profile_callcount < cn->profile_callcount)
            {
               a.elements[i]   = cn;
               a.elements[i+1] = cc;
               bSwapped = YAC_TRUE;
            }
         }
      } while(bSwapped);
   }

   // Print sorted results
   AppendNativeResults(buf, a, "callcount");

   a.free();

   // Print to stderr
   tks_fputs2((const char*) buf.chars);
}

void TKS_FunctionProfiler::PrintResults(void) {
   PrintScriptResults();
   PrintNativeResults();
}

#else

static sUI tks_functionprofiler_dummy;

#endif // TKS_PROFILE_FUNCTIONS
