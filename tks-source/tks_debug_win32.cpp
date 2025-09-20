/// tks_debug_win32.cpp 
///
/// (c) 2001-2008 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under the terms of the GNU general public license (GPL). 
/// 
 
 
#include "tks.h" 
 
#ifdef YAC_WIN32 
 
#include <windows.h> 
 
#include "tks_debug.h" 
 
 
static LONG __stdcall tksexhandler(EXCEPTION_POINTERS *exceptionInfo) { 
   switch(exceptionInfo->ExceptionRecord->ExceptionCode) 
   { 
   case EXCEPTION_ACCESS_VIOLATION: 
      printf("\n\n[---] TKS exception ACCESS_VIOLATION caught ");
      if(exceptionInfo->ExceptionRecord->ExceptionInformation[0])
      {
         printf(" (write to "); 
      }
      else
      {
         printf(" (read from ");
      }
      printf("0x%08x)\n", (sUI) exceptionInfo->ExceptionRecord->ExceptionInformation[1]);
      tkscript->printAllFunctionCallStacks();

      // Run script callback if available
      if(tkscript->sig_segv)
      {
         YAC_Value r;
         void *context = tkscript->yacContextGetDefault(); // xxx TKS_MT: use *real* thread context ...?
         yac_host->yacEvalFunctionReturn((void*)context, tkscript->sig_segv, 0, NULL, &r); 
         ////////////////tkscript->yacContextDestroy(context);
         if(r.getIntValue())
         {
            // Modify the X86 PC so it continues with the SegVContinue routine
            // If we do not do this the last failed CPU instruction will be called again
#ifdef DX_X64
            exceptionInfo->ContextRecord->Rip = (DWORD64) TKS_SegVContinue;
#else
            exceptionInfo->ContextRecord->Eip = (DWORD) TKS_SegVContinue;
#endif // DX_X64

            return EXCEPTION_CONTINUE_EXECUTION; // continue at new PC 
         }
      }
      break; 
   case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: 
      printf("\n\n[---] TKS exception ARRAY_BOUNDS_EXCEEDED caught.\n"); 
      break; 
   case EXCEPTION_BREAKPOINT: 
      printf("\n\n[---] TKS exception BREAKPOINT.\n"); 
      break; 
   case EXCEPTION_DATATYPE_MISALIGNMENT: 
      printf("\n\n[---] TKS exception DATATYPE_MISALIGNMENT.\n"); 
      break; 
   case EXCEPTION_FLT_DENORMAL_OPERAND: 
      printf("\n\n[---] TKS exception FLT_DENORMAL_OPERAND.\n"); 
      break; 
   case EXCEPTION_FLT_DIVIDE_BY_ZERO: 
      printf("\n\n[---] TKS exception FLT_DIVIDE_BY_ZERO. \n"); 
      break; 
   case EXCEPTION_FLT_INEXACT_RESULT: 
      printf("\n\n[---] TKS exception FLT_INEXACT_RESULT. \n"); 
      break; 
   case EXCEPTION_FLT_INVALID_OPERATION: 
      printf("\n\n[---] TKS exception FLT_INVALID_OPERATION.\n"); 
      break; 
   case EXCEPTION_FLT_OVERFLOW: 
      printf("\n\n[---] TKS exception FLT_OVERFLOW.\n"); 
      break; 
   case EXCEPTION_FLT_STACK_CHECK: 
      printf("\n\n[---] TKS exception FLT_STACK_CHECK.\n"); 
      break; 
   case EXCEPTION_FLT_UNDERFLOW: 
      printf("\n\n[---] TKS exception FLT_UNDERFLOW.\n"); 
      break; 
   case EXCEPTION_ILLEGAL_INSTRUCTION: 
      printf("\n\n[---] TKS exception ILLEGAL_INSTRUCTION.\n"); 
      break; 
   case EXCEPTION_IN_PAGE_ERROR: 
      printf("\n\n[---] TKS exception IN_PAGE_ERROR.\n"); 
      break; 
   case EXCEPTION_INT_DIVIDE_BY_ZERO: 
      printf("\n\n[---] TKS exception INT_DIVIDE_BY_ZERO.\n"); 
      break; 
   case EXCEPTION_INT_OVERFLOW: 
      printf("\n\n[---] TKS exception INT_OVERFLOW. \n"); 
      break; 
   case EXCEPTION_INVALID_DISPOSITION: 
      printf("\n\n[---] TKS exception INVALID_DISPOSITION.\n"); 
      break; 
   case EXCEPTION_NONCONTINUABLE_EXCEPTION: 
      printf("\n\n[---] TKS exception NONCONTINUABLE_EXCEPTION.\n"); 
      break; 
   case EXCEPTION_PRIV_INSTRUCTION: 
      printf("\n\n[---] TKS exception PRIV_INSTRUCTION.\n"); 
      break; 
   case EXCEPTION_SINGLE_STEP: 
      printf("\n\n[---] TKS exception SINGLE_STEP.\n"); 
      break; 
   case EXCEPTION_STACK_OVERFLOW: 
      printf("\n\n[---] TKS exception STACK_OVERFLOW.\n"); 
      break; 
   } 
   //return EXCEPTION_CONTINUE_SEARCH; 
   return EXCEPTION_EXECUTE_HANDLER; 
} 
 
 
void tks_debug_init(int _argc, char**_argv) { 
   (void) _argc;
   (void) _argv;

   ::SetUnhandledExceptionFilter(tksexhandler);  
} 
 
void tks_debug_exit(void) { 
} 
 
 
#endif // YAC_WIN32 
 
