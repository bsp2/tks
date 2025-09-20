/// Thread.cpp
///
/// (c) 2002-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the GNU General Public License (GPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#GPL> for further information.
///
///
/// changelog:
///      04Jul2002 <fli> file created
///      11Mar2005 source cleanup
///      15Mai2006 moved from sdl plugin to tks source, rewrote to use pthreads / win32api
///

// Define DX_WIN32_YIELD to include yield() support for win32 (default = off)
// Define DX_PTHREADP to include pthread_setaffinity support (needs glibc, default=off)
// Define DX_GETTID to include gettid() syscall support (linux, default = off)

// (todo) macOS <https://developer.apple.com/library/archive/releasenotes/Performance/RN-AffinityAPI/index.html>
//         (thread affinity tags instead of masks)
//         <http://www.hybridkernel.com/2015/01/18/binding_threads_to_cores_osx.html>

#ifdef HAVE_WIN10
#include <windows.h>
#include <Processthreadsapi.h>
#endif // HAVE_WIN10

#include "tks.h"

#ifdef DX_PTHREADS

#include <errno.h>
#include <sched.h>
#ifdef DX_PTHREADP
#include <pthreadP.h>
#endif
#endif

#ifdef YAC_LINUX
extern "C" {
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <linux/unistd.h>
#include <errno.h>
#include <unistd.h>
#include <syscall.h>
#ifdef DX_GETTID
_syscall0(pid_t, gettid)
 pid_t gettid(void);
#endif
}
#endif

#ifdef YAC_MACOS
#include <unistd.h>
#include <mach/thread_policy.h>
#include <mach/thread_act.h>

#include <mach/mach_init.h>
#include <mach/thread_policy.h>
// #include <mach/sched.h>

#include <sys/types.h>
#include <sys/sysctl.h>
#endif // YAC_MACOS

#include "TKS_Mutex.h"
#include "TKS_Condition.h"

#include <stdio.h>
#include <stdlib.h>

#include "YAC_ValueObject.h"
#include "YAC_Event.h"
#include "TKS_Mailbox.h"
#include "TKS_Thread.h"
#include "TKS_ScriptObjectID.h"
#include "TKS_FunctionObject.h"


// // #if defined(YAC_WIN32) && defined(HAVE_WIN10)
// // #include <Processthreadsapi.h>
// // #if 0
// // YAC_APIC BOOL SetThreadSelectedCpuSets(
// //    _In_       HANDLE Thread,
// //    _In_ const ULONG  *CpuSetIds,
// //    _In_       ULONG  CpuSetIdCount
// //                                             );
// // #endif
// // #endif

#ifdef YAC_WIN32
typedef BOOL (*SetThreadSelectedCpuSets_fxn_t) (
   _In_       HANDLE Thread,
   _In_ const ULONG  *CpuSetIds,
   _In_       ULONG  CpuSetIdCount
                                                );
static SetThreadSelectedCpuSets_fxn_t   SetThreadSelectedCpuSets_fxn   = NULL;

#ifdef HAVE_WIN10
// // #include <Processthreadsapi.h>

typedef BOOL (*GetSystemCpuSetInformation_fxn_t) (
   PSYSTEM_CPU_SET_INFORMATION Information,
   ULONG                       BufferLength,
   PULONG                      ReturnedLength,
   HANDLE                      Process,
   ULONG                       Flags
                                                  );
static GetSystemCpuSetInformation_fxn_t GetSystemCpuSetInformation_fxn = NULL;
#endif // HAVE_WIN10


void TKS_Thread::LazyResolveWindows10ProcessThreadsAPIFxns(void) {
   // (note) resolve fxn dynamically to maintain Windows 7/8 compatibility

#ifdef TKS_MT
   (void)tkscript->lockGlobalContext();
#endif //TKS_MT

   static sBool bFxnCheck = YAC_FALSE;

   if(YAC_FALSE == bFxnCheck)
   {
      bFxnCheck = YAC_TRUE;

      HMODULE hDLL = ::LoadLibrary("Kernel32.dll");
      if(NULL != hDLL)
      {
         SetThreadSelectedCpuSets_fxn = (SetThreadSelectedCpuSets_fxn_t)
            ::GetProcAddress(hDLL, "SetThreadSelectedCpuSets");

#ifdef HAVE_WIN10
         GetSystemCpuSetInformation_fxn = (GetSystemCpuSetInformation_fxn_t)
            ::GetProcAddress(hDLL, "GetSystemCpuSetInformation");
#endif // HAVE_WIN10
      }
   }

#ifdef TKS_MT
   tkscript->unlockGlobalContext();
#endif //TKS_MT
}


DWORD
#ifndef YAC_64
__stdcall
#endif // YAC_64
win32_yac_threadentry_fxn(LPVOID _ud) {
   // (note) win32 API uses DWORD (32bit) return type, yac_threadentry_fxn_t (and pthreads) uses 64bit void*
   const win32_yac_threadentry_userdata_t *ud = (win32_yac_threadentry_userdata_t*)_ud;
   const void *r = ud->fxn(ud->userdata);
   return (DWORD)size_t(r);  // truncate to 32bit
}
#endif // YAC_WIN32


#ifndef YAC_FORCE_NO_TLS
YAC_TLS TKS_Thread *tks_current_thread = NULL;

// This is used by plugins that create their own threads (i.e. NOT by using a Thread object)
//  and want to call script functions from within that thread.
//  After the plugin has created the thread, it must create a context and set this as the
//   default context (yacContextCreate(), yacContextSetDefault())
//
// If a thread was created using a Thread object, this pointer is NULL (tks_current_thread->context is used instead)
//
// Also see yacContextGetDefault()
YAC_TLS YAC_ContextHandle tks_current_context = NULL;
#endif // YAC_FORCE_NO_TLS


static void *tksvm__threadentry(void *_TKS_Thread) {
   TKS_Thread *thread = (TKS_Thread *)_TKS_Thread;

   // yac_host->printf("xxx tksvm__threadentry\n");

#ifndef YAC_FORCE_NO_TLS
   tks_current_thread = thread;
#endif // YAC_FORCE_NO_TLS

#ifdef YAC_LINUX
   thread->lwp_id = syscall(SYS_gettid);
#endif

#ifdef DX_PTHREADS
   {
      // "The thread can be canceled at any time. (Typically, it will be canceled immediately upon receiving a cancellation request, but the system doesn't guarantee this.)"
      //   -- https://linux.die.net/man/3/pthread_setcanceltype
      int oldState;
      pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldState);
   }
   thread->b_pthread_started = YAC_TRUE;
#endif // DX_PTHREADS

   yac_host->yacContextSetDefault(thread->context); // [25Jan2018] added

   if(thread->context->setJmpThreadExit())
   {
      YAC_Value args[1];
      args[0].initObject(thread, 0);
      yac_host->yacEvalFunction(thread->context, (void*)thread->tks_entry_fun, 1, args);

      if(!thread->b_killed)
      {
         // Print exception (if any)
         if(thread->context->b_exception_raised)
            thread->context->printException();
         else
            thread->b_clean_exit = 1;
      }

      // printf("xxx tksvm__threadentry: leaving thread id=%d\n", thread->_getId());
      return 0;
   }
   else
   {
      // Fatal error occured, terminate thread immediately
      //  (note) longJmpThreadExit() called by TKS_Context::raiseRuntimeException()
      return 0;
   }

}


static sBool loc_b_have_procinfo = YAC_FALSE;
static sUI loc_num_proc = 0u;

#define TKS_MAX_LOGICAL_CORES 64u

#define TKS_PROCINFO_FLAG_PRESENT  (1u << 0)  // is available / present
#define TKS_PROCINFO_FLAG_HT       (1u << 1)  // set for first shared logical processor, e.g. CPU1=PRESENT, CPU2=PRESENT,HT
#define TKS_PROCINFO_FLAG_P        (1u << 2)  // is performance core
#define TKS_PROCINFO_FLAG_E        (1u << 3)  // is efficiency core
typedef struct tks_procinfo_s {
   sU32 flags;
   sU64 processor_mask;
} tks_procinfo_t;

static tks_procinfo_t loc_tks_procinfo[TKS_MAX_LOGICAL_CORES];

#ifdef YAC_WIN32
static SYSTEM_LOGICAL_PROCESSOR_INFORMATION loc_procinfo_win32[TKS_MAX_LOGICAL_CORES];

// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation
static DWORD CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}

#endif // YAC_WIN32

void TKS_Thread::LazyGetProcessorInformation(void) {
   if(!loc_b_have_procinfo)
   {
      loc_b_have_procinfo = YAC_TRUE;
      memset((void*)loc_tks_procinfo, 0, sizeof(loc_tks_procinfo));

      sU8 numCoresPerInfo[TKS_MAX_LOGICAL_CORES];
      memset((void*)numCoresPerInfo, 0, sizeof(numCoresPerInfo));

#ifdef YAC_WIN32
      DWORD retLen = sizeof(loc_procinfo_win32);////sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
      // ::GetLogicalProcessorInformation(NULL, &retLen);
      // // yac_host->printf("xxx retLen=%u\n", retLen);
      // if(ERROR_INSUFFICIENT_BUFFER == GetLastError())
      // {
      //    procInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)::malloc(retLen);
      //    if(NULL != procInfo)
      // {
      if(::GetLogicalProcessorInformation(loc_procinfo_win32, &retLen))
      {
         loc_num_proc = 0u;
         sUI numInfos = retLen / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
         sUI procIdx = 0u;
         sUI relInfoIdx = 0u;

         if(tkscript->configuration.debug_level > 1)
            tkscript->printf("[trc] TKS_Thread::LazyGetProcessorInformation: retLen=%u numInfos=%u\n", retLen, numInfos);

         for(sUI infoIdx = 0u; infoIdx < numInfos; infoIdx++)
         {
            const SYSTEM_LOGICAL_PROCESSOR_INFORMATION *lpi = &loc_procinfo_win32[infoIdx];

            if(RelationProcessorCore == lpi->Relationship)
            {
               loc_tks_procinfo[procIdx].flags          |= TKS_PROCINFO_FLAG_PRESENT;
               loc_tks_procinfo[procIdx].processor_mask  = lpi->ProcessorMask;
               procIdx++;

               sUI numCores = CountSetBits(lpi->ProcessorMask);
               numCoresPerInfo[relInfoIdx] = numCores;

               if(tkscript->configuration.debug_level > 2)
                  tkscript->printf("[trc] TKS_Thread::LazyGetProcessorInformation: infoIdx=%u relInfoIdx=%u numCores=%u. retLen=%u numInfos=%u\n", infoIdx, relInfoIdx, numCores, retLen, numInfos);

               if(2u == numCores)
               {
                  // Hyperthreaded
                  if(procIdx < TKS_MAX_LOGICAL_CORES)
                  {
                     loc_tks_procinfo[procIdx].flags          |= TKS_PROCINFO_FLAG_PRESENT | TKS_PROCINFO_FLAG_HT;
                     loc_tks_procinfo[procIdx].processor_mask  = lpi->ProcessorMask;
                     procIdx++;
                  }
                  else
                  {
                     tkscript->printf("[~~~] TKS_Thread::LazyGetProcessorInformation: infoIdx=%u numCores=%u TKS_MAX_LOGICAL_CORES exceeded. retLen=%u numInfos=%u\n", infoIdx, numCores, retLen, numInfos);
                  }
               }
               else if(1u != numCores)
               {
                  tkscript->printf("[~~~] TKS_Thread::LazyGetProcessorInformation: infoIdx=%u unexpected numCores=%u\n", infoIdx, numCores);
               }

               relInfoIdx++;
            } // if RelationProcessorCore
         }

         loc_num_proc = procIdx;

#if 1 && defined(HAVE_WIN10)  // WIN7, actually
         {
            // https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformationex
            // https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-processor_relationship
            // https://www.intel.com/content/www/us/en/developer/articles/guide/12th-gen-intel-core-processor-gamedev-guide.html
            DWORD retLength = 0;
            GetLogicalProcessorInformationEx(RelationProcessorCore/*RelationshipType*/,
                                                NULL/*Buffer*/,
                                                &retLength
                                             );
            if(ERROR_INSUFFICIENT_BUFFER == GetLastError())
            {
               // (note) variable sized structs
               SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *buf = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)malloc(retLength);
               if(NULL != buf)
               {
                  if(GetLogicalProcessorInformationEx(RelationProcessorCore/*RelationshipType*/,
                                                      buf,
                                                      &retLength
                                                      )
                     )
                  {
                     DWORD off = 0u;
                     sUI infoIdx = 0u;
                     sUI coreIdx = 0u;  // logical CPU core index
                     const SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *inf = buf;
                     while(off < retLength)
                     {
                        // "A core with a higher value for the efficiency class has intrinsically greater performance and
                        //   less efficiency than a core with a lower value for the efficiency class.
                        //   EfficiencyClass is only nonzero on systems with a heterogeneous set of cores"
                        //
                        //  in practice: EfficiencyClass = 1 for p-cores, EfficencyClass = 0 for e-cores
                        //
                        if(tkscript->configuration.debug_level > 2)
                           tkscript->printf("[trc] TKS_Thread::LazyGetProcessorInformation: infoIdx=%u off=%u inf->Size=%u logicalCoreIdx=%u #logical=%u EfficiencyClass=%d\n", infoIdx, off, inf->Size, coreIdx, numCoresPerInfo[infoIdx], inf->Processor.EfficiencyClass);
                        sBool bPerf = (0 != inf->Processor.EfficiencyClass);
                        for(sUI logicalCoreIdx = 0u; logicalCoreIdx < numCoresPerInfo[infoIdx]; logicalCoreIdx++)
                        {
                           loc_tks_procinfo[coreIdx + logicalCoreIdx].flags |= bPerf ? TKS_PROCINFO_FLAG_P : TKS_PROCINFO_FLAG_E;
                        }

                        // Next info
                        coreIdx += numCoresPerInfo[infoIdx];
                        off += inf->Size;
                        inf = (const SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *) ( ((const sU8*)buf) + off );
                        infoIdx++;
                     }
                  }
                  else
                  {
                     tkscript->printf("[~~~] TKS_Thread::LazyGetProcessorInformation: GetLogicalProcessorInformationEx() failed. LastError=%d\n", GetLastError());
                  }

                  free(buf);
               }
               else
               {
                  tkscript->printf("[~~~] TKS_Thread::LazyGetProcessorInformation: malloc(%u) failed\n", retLength);
               }
            }
            else
            {
               tkscript->printf("[~~~] TKS_Thread::LazyGetProcessorInformation: GetLogicalProcessorInformationEx() failed (query buf size). LastError=%d\n", GetLastError());
            }
         }
#endif // HAVE_WIN10

// #ifdef HAVE_WIN10
//          if(NULL != GetSystemCpuSetInformation_fxn)
//          {
//             GetSystemCpuSetInformation_fxn
//          }
// #endif // HAVE_WIN10

      }
      else
      {
         yac_host->printf("[~~~] Thread::LazyGetProcessorInformation: GetLogicalProcessorInformation() failed.\n");
      }
      // }
      // }
#elif defined(YAC_LINUX)
      int n = get_nprocs();
      if(n < 1)
         n = 1;
      loc_num_proc = sUI(n);
      for(sUI procIdx = 0u; procIdx < loc_num_proc; procIdx++)
      {
         loc_tks_procinfo[procIdx].flags          = TKS_PROCINFO_FLAG_PRESENT;
         loc_tks_procinfo[procIdx].processor_mask = (1ull << procIdx);
      }
#else
      // macOS
      {
         sSI intbuf;
         size_t oldlen = sizeof(intbuf);
         if(0 == sysctlbyname("hw.logicalcpu", &intbuf, &oldlen, NULL/*newp*/, 0/*newlen*/))
         {
            if(tkscript->configuration.debug_level > 1)
               tkscript->printf("[trc] TKS_Thread::LazyGetProcessorInformation: hw.logicalcpu=%d\n", intbuf);
            if(intbuf < 1)
               intbuf = 1;
            loc_num_proc = sUI(intbuf);
         }
      }

      for(sUI procIdx = 0u; procIdx < loc_num_proc; procIdx++)
      {
         loc_tks_procinfo[procIdx].flags          = TKS_PROCINFO_FLAG_PRESENT;
         loc_tks_procinfo[procIdx].processor_mask = (1ull << procIdx);
      }

      // (note) assumes that performance cores are enumerated first (e.g. M2pro)
      sUI coreIdx = 0u;
      {
         sSI intbuf;
         size_t oldlen = sizeof(intbuf);
         if(0 == sysctlbyname("hw.perflevel0.logicalcpu", &intbuf, &oldlen, NULL/*newp*/, 0/*newlen*/))
         {
            if(tkscript->configuration.debug_level > 1)
               tkscript->printf("[trc] TKS_Thread::LazyGetProcessorInformation: hw.perflevel0.logicalcpu=%d\n", intbuf);

            if(intbuf > 0)
            {
               sUI numPerf = sUI(intbuf);
               for(; coreIdx < numPerf; coreIdx++)
               {
                  loc_tks_procinfo[coreIdx].flags |= TKS_PROCINFO_FLAG_P;
               }
            }
         }
      }

      // (note) assumes that efficiency cores are enumerated after performance cores (e.g. M2pro)
      {
         sSI intbuf;
         size_t oldlen = sizeof(intbuf);
         if(0 == sysctlbyname("hw.perflevel1.physicalcpu", &intbuf, &oldlen, NULL/*newp*/, 0/*newlen*/))
         {
            if(tkscript->configuration.debug_level > 1)
               tkscript->printf("[trc] TKS_Thread::LazyGetProcessorInformation: hw.perflevel1.logicalcpu=%d\n", intbuf);

            if(intbuf > 0)
            {
               sUI numEff = sUI(intbuf);
               for(sUI effIdx = 0u; effIdx < numEff; effIdx++)
               {
                  loc_tks_procinfo[coreIdx + effIdx].flags |= TKS_PROCINFO_FLAG_E;
               }
            }
         }
      }

#endif
   } // if !loc_b_have_procinfo
}

sBool TKS_Thread::ParseAffinityMaskString(YAC_Object *_s, sU64 &_mask) {
   sBool r = YAC_FALSE;
   if(YAC_Is_String(_s))
   {
      YAC_CAST_ARG(YAC_String, sMask, _s);
      if(sMask->length > 1)
      {
         _mask = 0ull;
         const char *s = (const char*)sMask->chars;
         char c;
         sU32 bit = 0u;
         do
         {
            c = *s++;
            switch(c)
            {
               case 0:
               case ' ':
               case '\t':
               case '\n':
               case '\r':
               default:
                  // skip char (can be used for grouping logical processors)
                  break;

               case '*':
               case '+':
               case 'x':
               case '#':
               case 'y':
                  _mask |= (1ull << bit++);
                  r = YAC_TRUE;
                  break;

               case '.':
               case '-':
               case '_':
               case 'n':
                  bit++;
                  r = YAC_TRUE;
                  break;
            }
         }
         while(c);
      }
   }
   return r;
}

sBool TKS_Thread::_IsAffinityMaskStringBitSet(YAC_Object *_s, sUI _bit) {
   sU64 mask64 = ~0ull;
   if(ParseAffinityMaskString(_s, mask64))
   {
      // Dprintf("xxx IsAffinityMaskStringBitSet: mask64=0x%08x bit=%u\n", mask64, _bit);
      if(mask64 & (1ull << _bit))
         return YAC_TRUE;
   }
   return YAC_FALSE;
}


TKS_Thread::TKS_Thread(void) {
   b_created        = YAC_FALSE;
   b_clean_exit     = YAC_FALSE;
   b_running        = YAC_FALSE;
   b_killable       = YAC_TRUE;
   b_killed         = YAC_FALSE;
   tks_entry_fun    = NULL;
   user_data        = NULL;
   priority         = TKS_THREAD_PRIORITY_NORMAL;
   context          = NULL;

#ifdef DX_PTHREADS
   b_pthread_started = YAC_FALSE;
#ifdef YAC_LINUX
   lwp_id = -1;
#endif
#endif

   yacmem thisAddr;
   thisAddr.any = this;
   rand_seed = 0x4489abcdu ^ thisAddr.ui;
}

TKS_Thread::~TKS_Thread() {
   _kill();
}

void TKS_Thread::initFromCurrent2(void) {
   //
   // Helper fxn to initialize Thread object from current thread environment
   //
   b_created  = YAC_TRUE;
   b_running  = YAC_TRUE;
   b_killable = YAC_FALSE;
   b_killed   = YAC_FALSE;
   context = NULL;

#ifdef DX_PTHREADS
   pthread_id = pthread_self();
#else
#ifdef YAC_WIN32
   dwThreadId = GetCurrentThreadId();
   hThread = GetCurrentThread();
#endif // YAC_WIN32
#endif // DX_PTHREAD
}

void TKS_Thread::initMain(void) {
   initFromCurrent2();
   context = (TKS_Context*) tkscript->main_context;
}

void TKS_Thread::initFromCurrent(void) {
   initFromCurrent2();
   context = (TKS_Context*) yac_host->yacContextCreate();
   // (note) context->b_thread = YAC_FALSE;
}

sSI TKS_Thread::_create(YAC_Object *_fun) {
   if((!b_created) && YAC_CHK(_fun, TKS_CLID_FUNCTION))
   {
      context = (TKS_Context*) yac_host->yacContextCreate();
      context->b_thread = YAC_TRUE;
      context->tks_fstackc--;
      context->tks_fstackc->initObject(this, 0); // First local var

      tks_entry_fun = (TKS_FunctionObject*) _fun;

      return create2(tksvm__threadentry, (void*)this);
   }

   return YAC_FALSE;
}

sBool TKS_Thread::create2(yac_thread_fxn_t _threadFxn, void *_userData) {
   b_clean_exit = YAC_FALSE;
   b_killed     = YAC_FALSE;
   b_running    = YAC_TRUE;

#ifdef DX_PTHREADS
   pthread_attr_t attr;
   pthread_attr_init(&attr);

#if 1 && defined(YAC_MACOS)
   if(TKS_THREAD_PRIORITY_TIME_CRITICAL == priority)
   {
#ifdef HAVE_APPLE_QOS
      if(tkscript->configuration.debug_level >= 2)
      {
         Dprintf("[dbg] Thread::create2: prio=TIME_CRITICAL HAVE_APPLE_QOS\n");
      }
      pthread_attr_set_qos_class_np(&attr, 
                                    QOS_CLASS_USER_INTERACTIVE, 0
                                    );
#else
      if(tkscript->configuration.debug_level >= 2)
      {
         Dprintf("[dbg] Thread::create2: prio=TIME_CRITICAL SCHED_FIFO\n");
      }
      int policy = SCHED_FIFO;
      pthread_attr_setschedpolicy(&attr, policy);
      struct sched_param schedParam;
      schedParam.sched_priority = sched_get_priority_max(policy);
      pthread_attr_setschedparam(&attr, &schedParam);
      pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
#endif
   }
#endif // YAC_MACOS
   
   b_created = (0 == pthread_create(&pthread_id, &attr, _threadFxn, _userData));

#ifdef YAC_MACOS
   if(b_created)
   {
#if 0  // xxxxxxxxxxxxxxxxx
      mach_thread_id = pthread_mach_thread_np(pthread_id);
      thread_affinity_policy_data_t policy = { THREAD_AFFINITY_TAG_NULL };
      kern_return_t kret = thread_policy_set(mach_thread_id,
                                             THREAD_AFFINITY_POLICY,
                                             (thread_policy_t)&policy,
                                             1/*count*/
                                             );
      (void)kret;
#endif
   }
   else
   {
   }
#endif // YAC_MACOS

#ifdef YAC_LINUX
   if(b_created)
   {
      /* wait for lwp_id field to become valid */
      while(!b_pthread_started)
      {
         // pthread_yield();
         sched_yield();
      }
   }
#endif // YAC_LINUX

#else

#ifdef YAC_WIN32
   // (adapted from msdn example)
   win32_yac_threadentry_userdata.fxn      = _threadFxn;
   win32_yac_threadentry_userdata.userdata = _userData;
   hThread = CreateThread(NULL,              // default security attributes
                          0,                 // use default stack size
                          &win32_yac_threadentry_fxn, // thread function
                          (LPVOID)&win32_yac_threadentry_userdata, // argument to thread function
                          0,                 // use default creation flags
                          &dwThreadId        // returns the thread identifier
                          );
   b_created = (NULL != hThread);
#endif

#endif // DX_PTHREADS

   if(!b_created)
   {
      yac_host->yacContextDestroy(context);
      context = NULL;
   }

   b_running = b_created;
   return b_running;
}

void TKS_Thread::_kill(void) {
   if(b_created && b_killable)
   {
      if(YAC_FALSE == b_clean_exit)
      {
         sSI killedId = _getId();
         Dprintf("[---] killing Thread id=%d leftovers..\n", killedId);
         b_killed = YAC_TRUE;

#ifdef DX_PTHREADS
         ///pthread_join( pthread_id, NULL);
         pthread_cancel( pthread_id );
         pthread_detach( pthread_id );
         pthread_id = 0;
#else
#ifdef YAC_WIN32
         SuspendThread( hThread );
         TerminateThread( hThread, 10 ); // 10 = exit code
         WaitForMultipleObjects(1, &hThread, TRUE, 5000 /*INFINITE*/); // wait max. 5sec
         CloseHandle(hThread);
         hThread = NULL;
#endif
#endif
         Dprintf("[---] Thread id=%d killed.\n", killedId);
      }
      else
      {
#ifdef DX_PTHREADS
         ///pthread_join( pthread_id, NULL);
         pthread_cancel( pthread_id );
         pthread_detach( pthread_id );
         pthread_id = 0;
#else
#ifdef YAC_WIN32
         SuspendThread( hThread );
         TerminateThread( hThread, 10 ); // 10 = exit code
         CloseHandle(hThread);
         hThread = NULL;
#endif
#endif
      }

      // Delete script execution context
      if(NULL != context)
      {
         yac_host->yacContextDestroy(context);
         context = NULL;
      }

      b_running = YAC_FALSE;
      b_created = YAC_FALSE;

#ifdef DX_PTHREADS
      b_pthread_started = YAC_FALSE;
#ifdef YAC_LINUX
      lwp_id = -1;
#endif
#endif
   }

}

sSI TKS_Thread::_wait(void) {
   if(b_created && b_killable)
   {
      if(1 || (YAC_FALSE == b_clean_exit))
      // // // if(YAC_FALSE == b_clean_exit)  // [05Sep2019]
      {
         b_running = 0; // signalize thread cancellation
#ifdef DX_PTHREADS
         pthread_join( pthread_id, NULL ); // wait for thread to exit
         pthread_id = 0;
#else
#ifdef YAC_WIN32
         WaitForMultipleObjects(1, &hThread, TRUE, INFINITE); // wait for thread to exit
         CloseHandle(hThread);
         hThread = NULL;
#endif
#endif
      }
      else
      {
#ifdef DX_PTHREADS
         pthread_detach( pthread_id );
         pthread_cancel( pthread_id );
         pthread_id = 0;
#else
#ifdef YAC_WIN32
         SuspendThread( hThread );
         TerminateThread( hThread, 10 ); // 10 = exit code
         CloseHandle(hThread);
         hThread = NULL;
#endif
#endif
      }

      // Delete script execution context
      if(NULL != context)
      {
         yac_host->yacContextDestroy(context);
         context = NULL;
      }

      b_running = YAC_FALSE;
      b_created = YAC_FALSE;

#ifdef DX_PTHREADS
      b_pthread_started = YAC_FALSE;
#ifdef YAC_LINUX
      lwp_id = -1;
#endif
#endif
      return b_clean_exit;
   }

   return -1;
}

sSI TKS_Thread::_getId(void) {
   if(b_created)
   {
#ifdef YAC_LINUX
      return (sSI) lwp_id;
#else

#ifdef DX_PTHREADS
#ifdef YAC_MACOS
      {
         yacmem m; m.any = (void*)pthread_id;
         return m.s32;  // discards upper 32bit of pthread_id but this is only used for debugging purposes, anyway.
      }
#else
      return (sSI) pthread_id;
#endif // YAC_MACOS
#else
#ifdef YAC_WIN32
      return (sSI) dwThreadId;
#endif
#endif

#endif // YAC_LINUX

   }
   return -1;
}

void TKS_Thread::lazyCleanUpTerminatedThread(void) {
   if(b_created && b_running && b_killable)
   {
#ifdef YAC_WIN32
      DWORD result = WaitForSingleObject( hThread, 0/*timeout*/);

      if(WAIT_OBJECT_0 == result)
      {
         // the thread handle is signaled - the thread has terminated
         _kill();
      }
      else
      {
         // the thread handle is not signaled - the thread is still alive
      }
#else
      b_running = !b_clean_exit;
      if(!b_running)
      {
         _wait();
      }
#endif // YAC_WIN32
   }
}

sSI TKS_Thread::_isRunning(void) {

   // Clean up thread leftovers in case the thread has terminated (and/or crashed) and is not actually running anymore
   lazyCleanUpTerminatedThread();

   return b_running;
}

void TKS_Thread::_setName (YAC_String *_name) {
   name.yacCopy(_name);
}

void TKS_Thread::_getName (YAC_Value *_r) {
   _r->initNewString(&name);
}

void TKS_Thread::_setUserdata(YAC_Object *_o) {
   user_data = _o;
}

YAC_Object *TKS_Thread::_getUserdata(void) {
   return user_data;
}

#ifdef DX_PTHREADS
void TKS_Thread::setSchedFIFOPriority(sUI _prio) {
   struct sched_param param;
   memset((void*)&param, 0, sizeof(struct sched_param));

   param.sched_priority = _prio;

   int err = pthread_setschedparam(pthread_id, SCHED_FIFO, &param);

   if(0 != err)
   {
      Dprintf("[---] Thread<%d>::setPriority: pthread_setschedparam(SCHED_FIFO, %u) failed\n",
              _getId(),
              _prio
              );
   }
}
#endif // DX_PTHREADS

void TKS_Thread::_setPriority(sSI _priority) {
   if(_priority != priority)
   {
      sSI newPriority = _priority;
      if(b_created)
      {
#ifdef DX_PTHREADS

         int policy = SCHED_OTHER;
         int schedPriority = _priority;

#if 1 && defined(YAC_MACOS)
         switch(_priority)
         {
            default:
               break;

            case TKS_THREAD_PRIORITY_TIME_CRITICAL:
            {
               // https://developer.apple.com/library/archive/documentation/Darwin/Conceptual/KernelProgramming/scheduler/scheduler.html#//apple_ref/doc/uid/TP30000905-CH211-BABHGEFA
               // https://github.com/apple-oss-distributions/xnu/blob/main/osfmk/mach/thread_policy.h
               struct thread_time_constraint_policy ttcpolicy;
               thread_port_t threadport = pthread_mach_thread_np(pthread_id);
 
               ttcpolicy.period      = 0; // HZ / 1000;
               ttcpolicy.computation = 0; // HZ / 2000; // HZ/3300;
               ttcpolicy.constraint  = 0; // HZ / 1000; // HZ/2200;
               ttcpolicy.preemptible = 1; // ignored, see thread_policy.h
 
               // => fails with err=4
               int err = thread_policy_set(threadport,
                                           THREAD_TIME_CONSTRAINT_POLICY, (thread_policy_t)&ttcpolicy,
                                           THREAD_TIME_CONSTRAINT_POLICY_COUNT
                                           );
               if(KERN_SUCCESS == err)
               {
                  priority = _priority;
               }
               else
               {
                  Dprintf("[---] Thread<%d>::setPriority: thread_policy_set() (prio=TIME_CRITICAL) failed with err=%d\n", _getId(), err);
               }
               return;
            }
            break;

            case TKS_THREAD_PRIORITY_HIGHEST:
               schedPriority = sched_get_priority_max(policy);  // => 47
               // schedPriority = 45;
               policy = SCHED_OTHER;
               newPriority = TKS_THREAD_PRIORITY_ABOVE_NORMAL;
               break;

            case TKS_THREAD_PRIORITY_LOWEST:
               schedPriority = sched_get_priority_min(policy); // => 15
               break;
         }
#endif // YAC_MACOS

         switch(newPriority)
         {
            case TKS_THREAD_PRIORITY_HIGHEST:
               setSchedFIFOPriority(40);
               break;

            case TKS_THREAD_PRIORITY_TIME_CRITICAL:
               setSchedFIFOPriority(80);
               break;

            case TKS_THREAD_PRIORITY_BELOW_NORMAL:
            case TKS_THREAD_PRIORITY_IDLE:
            case TKS_THREAD_PRIORITY_LOWEST:
            case TKS_THREAD_PRIORITY_NORMAL:
            case TKS_THREAD_PRIORITY_ABOVE_NORMAL:
            {
               int policyOld = 0;
               struct sched_param paramOld;
               memset((void*)&paramOld, 0, sizeof(struct sched_param));
               struct sched_param param;
               memset((void*)&param, 0, sizeof(struct sched_param));
               param.sched_priority = schedPriority;
               (void)pthread_getschedparam(pthread_id, &policyOld, &paramOld);
               int err = pthread_setschedparam(pthread_id, policy, &param);
               if(0 == err)
               {
                  priority = _priority;
               }
               else
               {
                  const char *sMsg = "";
                  switch(err)
                  {
                     default:
                        break;

                     case EINVAL:
                        sMsg = "(EINVAL)";
                        break;

                     case ENOTSUP:
                        sMsg = "(ENOTSUP)";
                        break;

                     case ESRCH:
                        sMsg = "(ESRCH)";
                        break;

                     case EPERM:
                        sMsg = "(EPERM)";
                        break;
                  }
                  Dprintf("[---] Thread<%d>::setPriority: pthread_setschedparam(policy=%d prio=%d) failed with err=%d%s (policyOld=%d paramOld.sched_priority=%d minPrio=%d maxPrio=%d)\n", _getId(), policy, schedPriority, err, sMsg, policyOld, paramOld.sched_priority, sched_get_priority_min(policy), sched_get_priority_max(policy));
               }
            }
            break;
            default:
               Dprintf("[---] Thread<%d>::setPriority: illegal priority (%i).\n", _getId(), _priority);
               break;
         }
#else
#ifdef YAC_WIN32
         switch(_priority)
         {
            case TKS_THREAD_PRIORITY_ABOVE_NORMAL:
            case TKS_THREAD_PRIORITY_BELOW_NORMAL:
            case TKS_THREAD_PRIORITY_HIGHEST:
            case TKS_THREAD_PRIORITY_IDLE:
            case TKS_THREAD_PRIORITY_LOWEST:
            case TKS_THREAD_PRIORITY_NORMAL:
            case TKS_THREAD_PRIORITY_TIME_CRITICAL:
               SetThreadPriority(hThread, _priority);
               priority = _priority;
               break;
            default:
               Dprintf("[---] Thread<%d>::setPriority: illegal priority (%i).\n", _getId(), _priority);
               break;
         }
#endif // YAC_WIN32
#endif // DX_PTHREADS
      } // if b_created
      else
      {
         // Remember priority for create
         priority = _priority;
      }
   }
}

sSI TKS_Thread::_getPriority(void) {
   return priority;
}

void TKS_Thread::_SetProcessPriorityClass(sSI _priorityClass) {
#ifdef YAC_WIN32
   BOOL r = 0;
   DWORD dwExpectClass = 0;
   switch(_priorityClass)
   {
      case TKS_PROCESS_PRIORITY_CLASS_IDLE:
         dwExpectClass = IDLE_PRIORITY_CLASS;
         r = ::SetPriorityClass(GetCurrentProcess(), dwExpectClass);
         break;

      default:
      case TKS_PROCESS_PRIORITY_CLASS_NORMAL:
         dwExpectClass = NORMAL_PRIORITY_CLASS;
         r = ::SetPriorityClass(GetCurrentProcess(), dwExpectClass);
         break;

      case TKS_PROCESS_PRIORITY_CLASS_HIGH:
         dwExpectClass = HIGH_PRIORITY_CLASS;
         r = ::SetPriorityClass(GetCurrentProcess(), dwExpectClass);
         break;

      case TKS_PROCESS_PRIORITY_CLASS_REALTIME:
         // (note) won't when called with regular user privileges
         // (note) workaround: use taskmanager to manually set realtime priority
         // (note) OR: run process as administrator
         dwExpectClass = REALTIME_PRIORITY_CLASS;
         r = ::SetPriorityClass(GetCurrentProcess(), dwExpectClass);
         break;
   }

   if(0 == r)
   {
      Dprintf("[---] Thread::SetProcessPriorityClass<win32>: FAILED. r=%d GetLastError()=%d\n", r, GetLastError());
   }
   else
   {
      Dprintf("[...] Thread::SetProcessPriorityClass<win32>: ok, windows priority class is now 0x%08x (expect 0x%08x)\n", ::GetPriorityClass(GetCurrentProcess()), dwExpectClass);
   }

#else
   (void)_priorityClass;
#endif // YAC_WIN32
}

sBool TKS_Thread::_SetProcessAffinityMask32(sUI _mask) {
#ifdef YAC_WIN32
   DWORD_PTR procMask = 0;
   DWORD_PTR sysMask = 0;
   ::GetProcessAffinityMask(::GetCurrentProcess(), &procMask, &sysMask);
   DWORD_PTR newMask = ((DWORD_PTR)_mask) & sysMask;
   BOOL ret = ::SetProcessAffinityMask(::GetCurrentProcess(), newMask);
   if(!ret)
   {
      Dprintf("[~~~] Thread::SetProcessAffinityMask32: SetProcessAffinityMask() failed. GetLastError()=%d\n", GetLastError());
   }
   else
   {
      return YAC_TRUE;
   }
#endif // YAC_WIN32
   return YAC_FALSE;
}

sBool TKS_Thread::_SetProcessAffinityMaskByString(YAC_Object *_s) {
   sU64 mask64 = ~0ull;
   if(ParseAffinityMaskString(_s, mask64))
   {
#ifdef YAC_WIN32
      DWORD_PTR procMask = 0;
      DWORD_PTR sysMask = 0;
      ::GetProcessAffinityMask(::GetCurrentProcess(), &procMask, &sysMask);
      DWORD_PTR newMask = ((DWORD_PTR)mask64) & sysMask;
      BOOL ret = ::SetProcessAffinityMask(::GetCurrentProcess(), newMask);
      if(ret)
      {
         return YAC_TRUE;
      }
      else
      {
         Dprintf("[~~~] Thread::SetProcessAffinityMaskByString: SetProcessAffinityMask() failed. GetLastError()=%d\n", GetLastError());
      }
#endif // YAC_WIN32
   }
   return YAC_FALSE;
}

sUI TKS_Thread::_GetNumCPUCores(void) {
   LazyGetProcessorInformation();
   return loc_num_proc;
}

sBool TKS_Thread::_IsCPUCoreShared(sUI _coreIdx) {
   LazyGetProcessorInformation();
   if(_coreIdx < TKS_MAX_LOGICAL_CORES)
   {
      return (TKS_PROCINFO_FLAG_HT == (loc_tks_procinfo[_coreIdx].flags & TKS_PROCINFO_FLAG_HT));
   }
   return YAC_FALSE;
}

sBool TKS_Thread::_IsPerformanceCPUCore(sUI _coreIdx) {
   LazyGetProcessorInformation();
   if(_coreIdx < TKS_MAX_LOGICAL_CORES)
   {
      return (TKS_PROCINFO_FLAG_P == (loc_tks_procinfo[_coreIdx].flags & TKS_PROCINFO_FLAG_P));
   }
   return YAC_FALSE;
}

sBool TKS_Thread::_IsEfficiencyCPUCore(sUI _coreIdx) {
   LazyGetProcessorInformation();
   if(_coreIdx < TKS_MAX_LOGICAL_CORES)
   {
      return (TKS_PROCINFO_FLAG_E == (loc_tks_procinfo[_coreIdx].flags & TKS_PROCINFO_FLAG_E));
   }
   return YAC_FALSE;
}

sUI TKS_Thread::_GetMaxAffinityMask32(void) {
   sU64 r = 0u;

   LazyGetProcessorInformation();

   for(sUI procIdx = 0u; procIdx < loc_num_proc; procIdx++)
   {
      r |= loc_tks_procinfo[procIdx].processor_mask;
   }

   // truncate to 32 bit
   return (r & 0xFFFFffffu);
}

void TKS_Thread::_setAffinityMask32(sUI _mask) {
   if(b_created)
   {
#ifdef DX_PTHREADS

#ifdef DX_PTHREADP
      sUI mask = (sUI) _mask;
      pthread_setaffinity_np(pthread_id, (const cpu_set_t*) &mask);
#else
#ifdef YAC_LINUX
      ///struct pthread *pd = (struct pthread *) pthread_id;
#ifdef DX_GETTID
      sched_setaffinity(gettid()/*pd->tid*/, sizeof(sSI), (sUI*) &_mask);
#endif
#endif
#endif // if DX_PTHREADP

#else // if DX_PTHREADS
#ifdef YAC_WIN32
      DWORD_PTR dwp = DWORD_PTR(_mask);
      SetThreadAffinityMask(hThread, dwp);
#endif // YAC_WIN32
#endif // DX_PTHREADS
   } // if b_created
}

void TKS_Thread::_setAffinityMaskByString(YAC_Object *_s) {
   if(b_created)
   {
      sU64 mask64 = ~0ull;
      if(ParseAffinityMaskString(_s, mask64))
      {
#ifdef YAC_MACOS
         // Fallback to setCPUCore() (mask should contain only a single set bit)
         {
            // Find first set bit
            for(sU32 bit = 0u; bit < 64u; bit++)
            {
               if(mask64 & (1ull << bit))
               {
                  _setCPUCore(bit);
                  break;
               }
            }
         }
#else
#ifdef DX_PTHREADS
#ifdef DX_PTHREADP
         pthread_setaffinity_np(pthread_id, (const cpu_set_t*) &mask64);
#else
#ifdef YAC_LINUX
         ///struct pthread *pd = (struct pthread *) pthread_id;
#ifdef DX_GETTID
         sched_setaffinity(gettid()/*pd->tid*/, sizeof(sU64), &mask64);
#endif // DX_GETTID
#endif // YAC_LINUX
#endif // if DX_PTHREADP

#else // if DX_PTHREADS
#ifdef YAC_WIN32
         DWORD_PTR dwp = DWORD_PTR(mask64);
         SetThreadAffinityMask(hThread, dwp);
#endif // YAC_WIN32
#endif // DX_PTHREADS
#endif // YAC_MACOS
      } // if ParseAffinityMaskByString
   } // if b_created
}

void TKS_Thread::_setCPUCore(sSI _coreIdx) {

#ifdef YAC_MACOS
   // see <https://developer.apple.com/library/archive/releasenotes/Performance/RN-AffinityAPI/>
   if(_coreIdx < 0)
   {
      thread_affinity_policy_data_t policy = { THREAD_AFFINITY_TAG_NULL };
      kern_return_t kret = thread_policy_set(mach_thread_id,
                                             THREAD_AFFINITY_POLICY,
                                             (thread_policy_t)&policy,
                                             1/*count*/
                                             );
      (void)kret;
   }
   else
   {
      thread_affinity_policy_data_t policy = { _coreIdx };
      kern_return_t kret = thread_policy_set(mach_thread_id,
                                             THREAD_AFFINITY_POLICY,
                                             (thread_policy_t)&policy,
                                             1/*count*/
                                             );
      (void)kret;
   }
#else
   // Windows, Linux
   if(_coreIdx < 0)
   {
      _setAffinityMask32(~0u);  // any
   }
   else
   {
      _setAffinityMask32(1u << sUI(_coreIdx));
   }
#endif // YAC_MACOS

#ifdef YAC_WIN32

#if 0
   // https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadpriorityboost
   //  (note) disabling priority boost does not look like a good idea (=> Eureka multi-processing underruns)
   ::SetThreadPriorityBoost(hThread, TRUE/*disable*/);
#endif

   (void)LazyResolveWindows10ProcessThreadsAPIFxns();

   // tkscript->printf("xxx call SetThreadSelectedCpuSets_fxn=%p\n", SetThreadSelectedCpuSets_fxn);

   if(NULL != SetThreadSelectedCpuSets_fxn)
   {
      if(_coreIdx >= 0)
      {
         // "Sets the selected CPU Sets assignment for the specified thread.
         //  This assignment overrides the process default assignment, if one is set."
         //   -- https://msdn.microsoft.com/en-us/library/windows/desktop/mt186428(v=vs.85).aspx
         ULONG cpuSetIds = sUI(_coreIdx);
         SetThreadSelectedCpuSets_fxn(hThread,
                                      &cpuSetIds,
                                      1
                                      );
      }
      else
      {
         // Reset
         SetThreadSelectedCpuSets_fxn(hThread, NULL, 0);
      }
   }

   // // (todo) SetThreadIdealProcessor() ?
   // // https://learn.microsoft.com/de-de/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadidealprocessor
   ///     (==> sets _number_ of preferred processors per thread)

#endif // YAC_WIN32
}

void TKS_Thread::_allocEventQueue(sSI _size) {
   mbox.allocEventQueue((sUI)_size);
}

sSI TKS_Thread::sendEvent2(YAC_Event *_ev) {
   return mbox.sendEvent(_ev);
}

sSI TKS_Thread::_sendEvent(YAC_Object *_ev) {
   if(YAC_BCHK(_ev, YAC_CLID_EVENT))
   {
      return mbox.sendEvent((YAC_Event*)_ev);
   }
   else
   {
      Dprintf("[---] Thread::sendEvent: not an Event object.\n");
      return ~0;
   }
}

YAC_Object *TKS_Thread::_peekEvent(void) {
   return mbox.peekEvent();
}

YAC_Object *TKS_Thread::_peekEventById(sSI _id) {
   return mbox.peekEventById(_id);
}

void TKS_Thread::_waitEvent(sUI _timeoutMS, YAC_Value *_r) {
   mbox.waitEvent(_timeoutMS, _r);
}

void TKS_Thread::_waitEventById(sSI _id, sUI _timeoutMS, YAC_Value *_r) {
   mbox.waitEventById(_id, _timeoutMS, _r);
}

sUI TKS_Thread::_rand(void) {
   // yeah, this is a pretty lousy random fxn but I only needed it for testing
   //  (todo) replace by lfsr
   rand_seed *= 16807u;
   return rand_seed;
}

void TKS_Thread::_srand(sUI _seed) {
   rand_seed = _seed;
}

sBool TKS_Thread::Yield2(void) {
#ifdef YAC_WIN32
   return SwitchToThread();
#else
#ifdef DX_PTHREADS
#ifdef YAC_MACOS
   pthread_yield_np();
   return YAC_TRUE;
#else
   // (note) high CPU usage when there are no other waiting threads
   return (0 == sched_yield());
#endif // YAC_MACOS
#endif // DX_PTHREADS
#endif // YAC_WIN32
}

sBool TKS_Thread::_SwitchToThread(void) {
   return TKS_Thread::Yield2();
}

sBool TKS_Thread::_Yield(void) {
   return TKS_Thread::Yield2();
}

sBool TKS_Thread::Delay2(void) {
#ifdef YAC_WIN32
   return SwitchToThread();
#else
   // Linux, macOS
   usleep(500);
   return YAC_TRUE;
#endif
}
sBool TKS_Thread::_Delay(void) {
   return TKS_Thread::Delay2();
}
