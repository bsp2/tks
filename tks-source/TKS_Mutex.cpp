// TKS_Mutex.cpp
///
/// (c) 2002-2023 by Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///     13Apr2004: thanks to Carsten Busse <car.busse@gmx.de> for help with the
///                  pthread support
//      24Jul2021: add timedLock()
//      22Dec2022: add macos_pthread_mutex_timedlock()

#include "tks.h"

#include "TKS_Mutex.h"

#ifdef YAC_MACOS
#include <errno.h>
#include <time.h>
#endif // YAC_MACOS


#ifdef DX_PTHREADS
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
static pthread_mutex_t tks_pthread_mutex_t_init = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#elif defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
//macOS
static pthread_mutex_t tks_pthread_mutex_t_init = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else
// recursive initializer not available, just use the normal one and hope it works ;)
static pthread_mutex_t tks_pthread_mutex_t_init = PTHREAD_MUTEX_INITIALIZER;
#endif // PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP


#ifdef YAC_MACOS
// see https://www.mail-archive.com/dev@apr.apache.org/msg26846.html
/*
 * A pthread_mutex_timedlock() impl for OSX/macOS, which lacks the real thing.
 * NOTE: Unlike the real McCoy, won't return EOWNERDEAD, EDEADLK or EOWNERDEAD
 */
static int macos_pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abs_timeout) {
   int rv;
   struct timespec remaining, slept, ts;

   remaining = *abs_timeout;
   while ((rv = pthread_mutex_trylock(mutex)) == EBUSY) {
      ts.tv_sec = 0;
      ts.tv_nsec = (remaining.tv_sec > 0 ? 10000000 : 
                    (remaining.tv_nsec < 10000000 ? remaining.tv_nsec : 
                     10000000));
      nanosleep(&ts, &slept);
      ts.tv_nsec -= slept.tv_nsec;
      if (ts.tv_nsec <= remaining.tv_nsec) {
         remaining.tv_nsec -= ts.tv_nsec;
      }
      else {
         remaining.tv_sec--;
         remaining.tv_nsec = (1000000 - (ts.tv_nsec - remaining.tv_nsec));
      }
      if (remaining.tv_sec < 0 || (!remaining.tv_sec && remaining.tv_nsec <= 0)) {
         return ETIMEDOUT;
      }
   }

   return rv;
}
#define pthread_mutex_timedlock macos_pthread_mutex_timedlock
#endif // YAC_MACOS

TKS_Mutex::TKS_Mutex(void) {
   memcpy((void*)&handle, (const void*)&tks_pthread_mutex_t_init, sizeof(pthread_mutex_t));
}

TKS_Mutex::~TKS_Mutex() {
}

void TKS_Mutex::lock(void) {
   pthread_mutex_lock(&handle);
}

sBool TKS_Mutex::timedLock(sUI _timeoutSec) {
   struct timespec to = {0};
   to.tv_sec = ::time(NULL) + _timeoutSec;
   int err = pthread_mutex_timedlock(&handle, &to);
   return (0 == err);
}

void TKS_Mutex::unlock(void) {
   pthread_mutex_unlock(&handle);
}

#else //#ifdef DX_PTHREADS

#ifdef YAC_WIN32
TKS_Mutex::TKS_Mutex(void) {
   ::InitializeCriticalSection( &handle );
}

TKS_Mutex::~TKS_Mutex() {
	::DeleteCriticalSection( &handle );
}

void TKS_Mutex::lock(void) {
	::EnterCriticalSection( &handle );
}

sBool TKS_Mutex::timedLock(sUI _timeoutSec) {
   sUI sec = sUI(GetTickCount64() / 1000u);  // wraps after 497 days
   sUI toSec = sec + _timeoutSec;
   while(!::TryEnterCriticalSection(&handle))
   {
      sec = sUI(GetTickCount64() / 1000u);
      if(sec > toSec)
      {
         // timeout
         return YAC_FALSE;
      }
      else
      {
         // yield
         SwitchToThread();
      }
   }
   return YAC_TRUE;
}

void TKS_Mutex::unlock(void) {
	::LeaveCriticalSection( &handle );
}

#else
// ---- NO THREADS AVAILABLE ----
TKS_Mutex::TKS_Mutex(void) {
	// stub
}

TKS_Mutex::~TKS_Mutex() {
	// stub
}

void TKS_Mutex::lock(void) {
	// stub
}

sBool TKS_Mutex::timedLock(sUI _timeoutSec) {
   (void)_timeoutSec;
   return YAC_TRUE;
}

void TKS_Mutex::unlock(void) {
	// stub
}
#endif // YAC_WIN32
#endif // DX_PTHREADS

void TKS_Mutex::forceUnlock(void) {
   // ugly hack to release a mutex when a thread holding it has crashed
   //  (note) called in Eureka application in case audio stream has crashed
   for(int i=0; i<100; i++)
   {
      unlock();
   }
}
