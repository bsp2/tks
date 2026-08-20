/// TKS_Mutex.h
///
/// (c) 2002-2026 by Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///

#ifndef TKS_MUTEX_H__
#define TKS_MUTEX_H__


#ifdef DX_PTHREADS
#include <pthread.h>
#else
#ifdef YAC_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#endif


class TKS_Mutex {
public:

private:
#ifdef DX_PTHREADS
   pthread_mutex_t handle;
#else
#ifdef YAC_WIN32
   CRITICAL_SECTION handle;
#endif
#endif

public:

   TKS_Mutex(void);
   ~TKS_Mutex();

   void  lock        (void);
   sBool timedLock   (sUI _timeoutSec);
   void  unlock      (void);
   void  forceUnlock (void);
};


class TKS_NamedMutex : public TKS_Mutex {
public:
   YAC_String name;
};


#endif // TKS_MUTEX_H__
