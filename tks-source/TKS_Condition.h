/// TKS_Condition.h
///
/// (c) 2009-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///

#ifndef TKS_CONDITION_H__
#define TKS_CONDITION_H__


#ifdef DX_PTHREADS
#include <pthread.h>
#else
#ifdef YAC_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#endif


class TKS_Condition : public YAC_Object {
public:
   sBool b_created;

#ifdef DX_PTHREADS
   pthread_mutex_t mutex;
   volatile sBool  b_condition;
   pthread_cond_t  cond;
   sBool           b_manual_reset;
#else
#ifdef YAC_WIN32
   HANDLE          event;
#endif // YAC_WIN32
#endif // DX_PTHREADS


public:
   TKS_Condition(void);
   ~TKS_Condition();


public:
   sSI  create (sSI _bManualReset);
   sSI  raise  (void);
   sSI  wait   (sSI _timeout);
   sSI  reset  (void);
};


#endif // TKS_CONDITION_H__
