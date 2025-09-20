/// TKS_Thread.h
///
/// (c) 2002-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the GNU General Public License (GPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#GPL> for further information.
///
///
#ifndef TKS_THREAD_H__
#define TKS_THREAD_H__


#ifdef DX_PTHREADS 
#include <pthread.h> 
#else
#ifdef YAC_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef struct win32_yac_threadentry_userdata_s {
   yac_thread_fxn_t fxn;
   void *userdata;
} win32_yac_threadentry_userdata_t;
#endif 
#endif

#ifdef YAC_LINUX
#include <errno.h>
#include <syscall.h>
#endif

#ifdef YAC_MACOS
#include <mach/thread_policy.h>
#endif // YAC_MACOS

class TKS_FunctionObject;

   // See http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/setthreadpriority.asp
#define TKS_THREAD_PRIORITY_ABOVE_NORMAL 1
#define TKS_THREAD_PRIORITY_BELOW_NORMAL -1
#define TKS_THREAD_PRIORITY_HIGHEST 2
#define TKS_THREAD_PRIORITY_IDLE -15
#define TKS_THREAD_PRIORITY_LOWEST -2
#define TKS_THREAD_PRIORITY_NORMAL 0
#define TKS_THREAD_PRIORITY_TIME_CRITICAL 15

#define TKS_PROCESS_PRIORITY_CLASS_IDLE     0
#define TKS_PROCESS_PRIORITY_CLASS_NORMAL   1
#define TKS_PROCESS_PRIORITY_CLASS_HIGH     2
#define TKS_PROCESS_PRIORITY_CLASS_REALTIME 3

YC class TKS_Thread : public YAC_Object {
public:

#ifdef DX_PTHREADS
   pthread_t pthread_id;
#ifdef YAC_MACOS
   thread_t mach_thread_id;
#endif // YAC_MACOS
   volatile sBool b_pthread_started;
#else
#ifdef YAC_WIN32
   HANDLE hThread;
   DWORD  dwThreadId;
   win32_yac_threadentry_userdata_t win32_yac_threadentry_userdata;
#endif
#endif

#ifdef YAC_LINUX
   pid_t lwp_id;
#endif

   int                  priority;
   sBool                b_running;     // 1 if thread has not been canceled yet
   sBool                b_clean_exit;  // 1 if thread has exited normally
   sBool                b_created;
   sBool                b_killable;    // 1 for regular script threads, 0 for main thread
   sBool                b_killed;
   TKS_Context *        context;       // script exec. context
	TKS_FunctionObject * tks_entry_fun; // (global) script function for thread startup
	YAC_Object *         user_data;     // passed to thread startup
   YAC_String           name;

   TKS_Mailbox mbox;

   sU32 rand_seed;

public:
	TKS_Thread(void);
	~TKS_Thread();

   void initFromCurrent2(); // helper
   void initFromCurrent(void); // Used for on-the-fly "unknown" (pooled) thread context initialization. allocates script context.
   void initMain(void);        // Usually only called for the main thread, use static script context.

   sBool create2 (yac_thread_fxn_t _threadFxn, void *_userData);
   sSI sendEvent2 (YAC_Event *_ev);
   static sBool Yield2 (void);
   static sBool Delay2 (void);

#ifdef DX_PTHREADS
   void setSchedFIFOPriority(sUI _prio);
#endif // DX_PTHREADS

   void lazyCleanUpTerminatedThread (void);

#ifdef YAC_WIN32
   static void LazyResolveWindows10ProcessThreadsAPIFxns (void);
#endif // YAC_WIN32

   static void LazyGetProcessorInformation (void);

   static sBool ParseAffinityMaskString (YAC_Object *_s, sU64 &_mask);

public:
	sSI         _create            (YAC_Object *_functionObject);
	void        _kill              (void);
	sSI         _wait              (void);
	sSI         _getId             (void);

	void        _setUserdata       (YAC_Object*);
	YAC_Object *_getUserdata       (void);
   sSI         _isRunning         (void);
   void        _setName           (YAC_String *_name);
   void        _getName           (YAC_Value *_r);

   void        _setPriority       (sSI _priority);
   sSI         _getPriority       (void);

   void        _SetProcessPriorityClass  (sSI _priorityClass);
   sBool       _SetProcessAffinityMask32 (sUI _mask);
   sBool       _SetProcessAffinityMaskByString (YAC_Object *_s);

   sBool       _IsAffinityMaskStringBitSet (YAC_Object *_s, sUI _bit);

   sUI         _GetNumCPUCores       (void);
   sBool       _IsCPUCoreShared      (sUI _coreIdx);
   sBool       _IsPerformanceCPUCore (sUI _coreIdx);
   sBool       _IsEfficiencyCPUCore  (sUI _coreIdx);

   // (note) setAffinityMask32 fails on Windows when mask contains invalid bits => must query MaxAffinityMask32
   //         see <https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-setthreadaffinitymask>
   sUI         _GetMaxAffinityMask32 (void);
   void        _setAffinityMask32 (sUI _mask);
   void        _setAffinityMaskByString (YAC_Object *_s);
   // (note) -1=any core
   void        _setCPUCore        (sSI _coreIdx);

   void        _allocEventQueue (sSI _size);
   sSI         _sendEvent       (YAC_Object *_ev);
   YAC_Object *_peekEvent       (void);
   YAC_Object *_peekEventById   (sSI _id);
   void        _waitEvent       (sUI _timeoutMS, YAC_Value *_r);
   void        _waitEventById   (sSI _id, sUI _timeoutMS, YAC_Value *_r);

   sUI         _rand            (void);
   void        _srand           (sUI _seed);

   // see https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-switchtothread
   sBool _SwitchToThread (void);
   sBool _Yield (void);  // same as SwitchToThread(). High CPU load when no other threads can be scheduled.
   sBool _Delay (void);  // SwitchToThread() on Windows, usleep(500) on Linux/macOS
};


#ifdef HAVE_TLS
extern YAC_TLS TKS_Thread *tks_current_thread;

extern YAC_TLS YAC_ContextHandle tks_current_context; // see TKS_Thread.cpp for explanation
#endif // HAVE_TLS


#endif // TKS_THREAD_H__
