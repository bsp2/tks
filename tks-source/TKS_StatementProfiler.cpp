/// TKS_StatementProfiler.cpp
///
/// (c) 2006-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#ifdef TKS_PROFILE_STATEMENTS

#include "TKS_Mutex.h"
#include "YAC_IntArray.h"

#include "tks_inc_compiler.h"

#define MAX_RESULT_LINES 128
//#define MAX_RESULT_LINES 10000


TKS_StatementProfiler::TKS_StatementProfiler(void) {
   init();
}

TKS_StatementProfiler::~TKS_StatementProfiler() {
   exit();
}

void TKS_StatementProfiler::init(void) {
   num_modules = 0;
   for(int i=0; i<TKS_MAX_MODULES; i++)
   {
      line_count   [i] = 0;
      call_count   [i] = NULL;
      millisec_sum [i] = NULL;
      avg_millisec [i] = NULL;
   }
#ifdef TKS_MT
   mtx_result = new TKS_Mutex();
#endif // TKS_MT
}

void TKS_StatementProfiler::exit(void) {
#ifdef TKS_MT
   if(mtx_result != NULL)
   {
      delete mtx_result;
      mtx_result = NULL;
   }
#endif // TKS_MT
   for(int i=0; i<TKS_MAX_MODULES; i++)
   {
      if(millisec_sum[i] != NULL)
      {
         delete [] millisec_sum[i];
         millisec_sum[i] = NULL;
      }
      if(avg_millisec[i] != NULL)
      {
         delete [] avg_millisec[i];
         avg_millisec[i] = NULL;
      }
      if(call_count[i] != NULL)
      {
         delete [] call_count[i];
         call_count[i] = NULL;
      }
      line_count[i] = 0;
   }
}

void TKS_StatementProfiler::addModule(sUI _numLines) {
   millisec_sum [num_modules] = new(std::nothrow) sF64[_numLines];
   avg_millisec [num_modules] = new(std::nothrow) sF64[_numLines];
   call_count   [num_modules] = new(std::nothrow) sUI [_numLines];
   for(sUI i=0; i<_numLines; i++)
   {
      millisec_sum[num_modules][i] = 0.0;
      avg_millisec[num_modules][i] = 0.0;
      call_count  [num_modules][i] = 0;
   }
   line_count   [num_modules++] = _numLines;
}

void TKS_StatementProfiler::addResult(sUI _srcLoc, sF64 _deltaMilliSec) {
/*   tkscript->printf("TKS_StatementProfiler::addResult: loc=%i:%i ms=%.64lg\n", 
      (_srcLoc>>16),
      (_srcLoc&0xFFFF),
      _deltaMilliSec
      );*/
   sUI modIdx = (_srcLoc>>16)&0xFFFF;
   if(modIdx>0)
   {
      // Is not temporary module
      sUI lineIdx = (_srcLoc&0xFFFF);
      if(lineIdx>=0)
      {
         modIdx--;

#ifdef TKS_MT
         mtx_result->lock();
#endif // TKS_MT
         
         sUI *cc  = &call_count   [modIdx][lineIdx];
         if(*cc==0)
         {
            *cc = 1;
//            tkscript->printf("xxx first *cc=%d\n", *cc);
            sF64 *avgms = &avg_millisec[modIdx][lineIdx];
            *avgms = _deltaMilliSec;
         }
         else
         {
            *cc = *cc +1;
//            tkscript->printf("xxx *cc=%d\n", *cc);
            sF64 *avgms = &avg_millisec[modIdx][lineIdx];
            *avgms = (*avgms+_deltaMilliSec)*0.5;
         }
         sF64 *ms = &millisec_sum [modIdx][lineIdx];
         *ms = *ms + _deltaMilliSec;

#ifdef TKS_MT
         mtx_result->unlock();
#endif // TKS_MT
      }
   }

}

void TKS_StatementProfiler::writeMilliString(YAC_String *_s) {
   // Traverse modules
   sUI modI;
   YAC_IntArray cc_ia;

   // Collect all called lines in cc_ia
   for(modI=0; modI<num_modules; modI++)
   {
     sUI *cc = call_count[modI];
     if(cc)
     {
         sUI i;
         sUI nl = line_count[modI];
         for(i=0; i<nl; i++)
         {
            if(cc[i])
            {
               // Line was called at least once
               cc_ia.add( (modI<<16) | i ); // Store src_loc
            }
         }
     }
   }

   // Sort all called lines by millisec criterium
   if(cc_ia.num_elements>1)
   {
      sBool bSwapped;
      do 
      {
         bSwapped = 0;
         sUI i;
         sF64 *lp = &millisec_sum[(cc_ia.elements[0]>>16)][(cc_ia.elements[0]&0xFFFF)];
         sF64 l = *lp;
         for(i=1; i<cc_ia.num_elements; i++)
         {
            lp = &millisec_sum[(cc_ia.elements[i-1]>>16)][(cc_ia.elements[i-1]&0xFFFF)];
            sF64 *rp = &millisec_sum[(cc_ia.elements[i]>>16)][(cc_ia.elements[i]&0xFFFF)];
            sF64 r = *rp;
            if(r<l)
            {
               sSI t = cc_ia.elements[i];
               cc_ia.elements[i] = cc_ia.elements[i-1];
               cc_ia.elements[i-1] = t;
               bSwapped = 1;
            }
            else
            {
               l = r;
            }
         }
      }
      while(bSwapped);
   }
   
   {
      // Build ratios
      sUI ne = cc_ia.num_elements;
      if(ne > MAX_RESULT_LINES) 
      {
         ne = MAX_RESULT_LINES;
      }
      sF64 *ra1st = new(std::nothrow) sF64[ne];
      sF64 *raParent = new(std::nothrow) sF64[ne];

      ra1st   [ne-1] = 1.0;
      raParent[ne-1] = 1.0;
      if(ne>1)
      {
         sSI i;
         sF64 lcc = (sF64) millisec_sum[(cc_ia.elements[ne-1]>>16)][(cc_ia.elements[ne-1]&0xFFFF)];
         sF64 fcc = lcc;
         for(i=(sSI)ne-1; i>=0; i--)
         {
            sF64 cc = (sF64) millisec_sum[(cc_ia.elements[i]>>16)][(cc_ia.elements[i]&0xFFFF)];
            if(cc>0)
            {
               ra1st[i] = (fcc/cc);
               raParent[i] = (lcc/cc);
            }
            else
            {
               ra1st[i] = 0.0;
               raParent[i] = 0.0;
            }
            lcc = cc;
         }
      }
      
      _s->append("------------------------------------\n");
      _s->append("Profile result (millisec_sum):\n");
      writeSortedArray(&cc_ia, ra1st, raParent, _s);
      
      delete [] raParent;
      delete [] ra1st;
   }
}

void TKS_StatementProfiler::writeAvgMilliString(YAC_String *_s) {
   // Traverse modules
   sUI modI;
   YAC_IntArray cc_ia;

   // Collect all called lines in cc_ia
   for(modI=0; modI<num_modules; modI++)
   {
     sUI *cc = call_count[modI];
     if(cc)
     {
         sUI i;
         sUI nl = line_count[modI];
         for(i=0; i<nl; i++)
         {
            if(cc[i])
            {
               // Line was called at least once
               cc_ia.add( (modI<<16) | i ); // Store src_loc
            }
         }
     }
   }

   // Sort all called lines by millisec criterium
   if(cc_ia.num_elements>1)
   {
      sBool bSwapped;
      do 
      {
         bSwapped = 0;
         sUI i;
         sF64 *lp = &avg_millisec[(cc_ia.elements[0]>>16)][(cc_ia.elements[0]&0xFFFF)];
         sF64 l = *lp;
         for(i=1; i<cc_ia.num_elements; i++)
         {
            lp = &avg_millisec[(cc_ia.elements[i-1]>>16)][(cc_ia.elements[i-1]&0xFFFF)];
            sF64 *rp = &avg_millisec[(cc_ia.elements[i]>>16)][(cc_ia.elements[i]&0xFFFF)];
            sF64 r = *rp;
            if(r<l)
            {
               sSI t = cc_ia.elements[i];
               cc_ia.elements[i] = cc_ia.elements[i-1];
               cc_ia.elements[i-1] = t;
               bSwapped = 1;
            }
            else
            {
               l = r;
            }
         }
      }
      while(bSwapped);
   }
   
   {
      // Build ratios
      sUI ne = cc_ia.num_elements;
      if(ne > MAX_RESULT_LINES) 
      {
         ne = MAX_RESULT_LINES;
      }
      sF64 *ra1st    = new(std::nothrow) sF64[ne];
      sF64 *raParent = new(std::nothrow) sF64[ne];

      ra1st   [ne-1] = 1.0;
      raParent[ne-1] = 1.0;
      if(ne>1)
      {
         sSI i;
         sF64 lcc = (sF64) avg_millisec[(cc_ia.elements[ne-1]>>16)][(cc_ia.elements[ne-1]&0xFFFF)];
         sF64 fcc = lcc;
         for(i=(sSI)ne-1; i>=0; i--)
         {
            sF64 cc = (sF64) avg_millisec[(cc_ia.elements[i]>>16)][(cc_ia.elements[i]&0xFFFF)];
            if(cc>0)
            {
               ra1st   [i] = (fcc/cc);
               raParent[i] = (lcc/cc);
            }
            else
            {
               ra1st   [i] = 0.0;
               raParent[i] = 0.0;
            }
            lcc = cc;
         }
      }
      
      _s->append("------------------------------------\n");
      _s->append("Profile result (avg_millisec):\n");
      writeSortedArray(&cc_ia, ra1st, raParent, _s);
      
      delete [] raParent;
      delete [] ra1st;
   }
}

void TKS_StatementProfiler::writeCountString(YAC_String *_s) {
   // Traverse modules
   sUI modI;
   YAC_IntArray cc_ia;
   cc_ia.alloc(2000); // Default allocation size
   cc_ia.empty();

   // Collect all called lines in cc_ia
   sUI k=0;
   for(modI=0; modI<num_modules; modI++)
   {
     sUI *cc = call_count[modI];
     if(cc)
     {
         sUI i;
         sUI nl = line_count[modI];
         for(i=0; i<nl; i++)
         {
            if(cc[i])
            {
               // Line was called at least once
               cc_ia.add( (modI<<16) | i ); // Store src_loc
               k++;
            }
         }
     }
   }
   tkscript->printf("[dbg] added %d profile lines\n", k);

   // Sort all called lines by call_count criterium
   if(cc_ia.num_elements>1)
   {
      sBool bSwapped;
      do 
      {
         bSwapped = 0;
         sUI i;
         sUI *lp = &call_count[(cc_ia.elements[0]>>16)][(cc_ia.elements[0]&0xFFFF)];
         sUI l = *lp;
         for(i=1; i<cc_ia.num_elements; i++)
         {
            lp = &call_count[(cc_ia.elements[i-1]>>16)][(cc_ia.elements[i-1]&0xFFFF)];
            sUI *rp = &call_count[(cc_ia.elements[i  ]>>16)][(cc_ia.elements[i  ]&0xFFFF)];
            sUI r = *rp;
            if(r<l)
            {
               sSI t = cc_ia.elements[i];
               cc_ia.elements[i] = cc_ia.elements[i-1];
               cc_ia.elements[i-1] = t;
               bSwapped = 1;
            }
            else
            {
               l = r;
            }
         }
      }
      while(bSwapped);
   }

   {
      // Build ratios
      sUI ne = cc_ia.num_elements;
      if(ne > MAX_RESULT_LINES) 
      {
         ne = MAX_RESULT_LINES;
      }
      sF64 *ra1st    = new(std::nothrow) sF64[ne];
      sF64 *raParent = new(std::nothrow) sF64[ne];

      ra1st   [ne-1] = 1.0;
      raParent[ne-1] = 1.0;
      if(ne>1)
      {
         sSI i;
         sF64 lcc = (sF64) call_count[(cc_ia.elements[ne-1]>>16)][(cc_ia.elements[ne-1]&0xFFFF)];
         sF64 fcc = lcc;
         for(i=(sSI)ne-1; i>=0; i--)
         {
            sF64 cc = (sF64) call_count[(cc_ia.elements[i]>>16)][(cc_ia.elements[i]&0xFFFF)];
            if(cc>0)
            {
               ra1st   [i] = (fcc/cc);
               raParent[i] = (lcc/cc);
            }
            else
            {
               ra1st   [i] = 0.0;
               raParent[i] = 0.0;
            }
            lcc = cc;
         }
      }
      
      _s->append("------------------------------------\n");
      _s->append("Profile result (call_count):\n");
      writeSortedArray(&cc_ia, ra1st, raParent, _s);
      
      delete [] raParent;
      delete [] ra1st;
   }
}

void TKS_StatementProfiler::writeCoverageString(YAC_String *_s) {
   _s->copy("todo...");
}

void TKS_StatementProfiler::writeSortedArray(YAC_IntArray *cc_ia, sF64 *_ratios1st, sF64 *_ratiosParent, YAC_String *_s) {
   if(cc_ia->num_elements)
   {
      sUI totalLoc = Dtkscompiler->getTotalNumLines();

      // Print (sorted) array
      YAC_String t;
      t.alloc(1024);

      t.empty();
      t.printf("  Num statement lines covered: %i/%i\n", cc_ia->num_elements, totalLoc);
      _s->append(&t);
      t.printf("  <rank>   <module_name>:<line_nr> <total_ms> <avg_ms> <call_count> <ratio1st> <ratioParent>\n");
      _s->append(&t);
      sUI j=1;
      for(sSI i=(sSI)cc_ia->num_elements-1; (i>=0) && (j<=MAX_RESULT_LINES); i--, j++) 
      {
         sUI srcLoc = cc_ia->elements[i];
         t.empty();
         ////t.printf("  %05d.: %s:%i %.012lg %lg %d (1/%.012lg) (1/%.012lg) *** ",
         t.printf("  %05d.: %s:%i %g %g %d (1/%g) (1/%g) *** ",
            j,
            ((TKS_Compiler*)tkscript->compiler)->getModuleNameByIndex( ((srcLoc>>16)+1) & 0xFFFF ),  
            (srcLoc&0xFFFF)+1 ,
            millisec_sum[(srcLoc>>16)][(srcLoc&0xFFFF)],
            avg_millisec[(srcLoc>>16)][(srcLoc&0xFFFF)],
            call_count[(srcLoc>>16)][(srcLoc&0xFFFF)],
            _ratios1st[i],
            _ratiosParent[i]
            );  
         _s->append(&t);
         TKS_CachedScript *cs = Dtkscompiler->getModuleByIndex( ((srcLoc>>16)+1) & 0xFFFF );
         if(cs != NULL)
         {
            cs->compiled_script->writeTokenLine((srcLoc&0xFFFF), _s);
         }
         _s->append("\n");
      }
      _s->append("------------------------------------\n");
   }
}

void TKS_StatementProfiler::printResultString(void) {
   YAC_String buf;

   buf.empty();
   writeCountString(&buf);
   tks_fputs2((const char*)buf.chars);

   buf.empty();
   writeMilliString(&buf);
   tks_fputs2((const char*)buf.chars);
   
   buf.empty();
   writeAvgMilliString(&buf);
   tks_fputs2((const char*)buf.chars);
}

#else //TKS_PROFILE_STATEMENTS
typedef int __tks_profile_statements_dummy;
#endif //TKS_PROFILE_STATEMENTS
