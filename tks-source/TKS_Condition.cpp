// TKS_Condition.cpp 
/// 
/// (c) 2009-2023 by Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 

#include "tks.h" 

// Note: pthread implementation is untested


#include "TKS_Condition.h" 
  
 
#ifdef DX_PTHREADS

#include <sys/time.h>
#include <errno.h>

#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
static pthread_mutex_t tks_pthread_mutex_t_init = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#elif defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
//macOS
static pthread_mutex_t tks_pthread_mutex_t_init = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else
// recursive initializer not available, just use the normal one and hope it works ;)
static pthread_mutex_t tks_pthread_mutex_t_init = PTHREAD_MUTEX_INITIALIZER;
#endif // PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP

static pthread_cond_t tks_pthread_cond_t_init = PTHREAD_COND_INITIALIZER;

TKS_Condition::TKS_Condition(void) { 
   memcpy((void*)&mutex, (const void*)&tks_pthread_mutex_t_init, sizeof(pthread_mutex_t));
   memcpy((void*)&cond, (const void*)&tks_pthread_cond_t_init, sizeof(pthread_cond_t));
   b_condition    = YAC_FALSE;
   b_manual_reset = YAC_FALSE;
   b_created      = YAC_FALSE;
} 

TKS_Condition::~TKS_Condition() {
   if(b_created)
   {
      b_created = YAC_FALSE;
      pthread_cond_destroy(&cond);
      pthread_mutex_destroy(&mutex);
   }
}

sSI TKS_Condition::create(sSI _bManualReset) {
   sSI r;

   if(!b_created)
   {
      b_manual_reset = _bManualReset;
      b_created = YAC_TRUE;
      r = 0;
   }
   else
   {
      r = -1;
   }

   return r;
}

sSI TKS_Condition::raise(void) {
   sSI r;

   if(b_created)
   {
      pthread_mutex_lock(&mutex);

      b_condition = YAC_TRUE;

      if(b_manual_reset)
      {
         // unblock *all* waiting threads
         r = pthread_cond_broadcast(&cond);
      }
      else
      {
         // unblock one waiting thread (+..spurious wakeup?)
         r = pthread_cond_signal(&cond);
      }

      pthread_mutex_unlock(&mutex);
   }
   else
   {
      r = -1;
   }

   return r;
}

sSI TKS_Condition::wait(sSI _timeout) {
   // (note) 'timeout' in milliseconds
   sSI r;

   if(b_created)
   {
      r = 0;

      struct timeval tv;
      struct timespec ts = {0, 0};

      if(_timeout > 0)
      {
         // query current time
         gettimeofday(&tv, NULL);

         // add delta time (millisec)
         ts.tv_sec = tv.tv_sec;
         sU64 usec = tv.tv_usec;
         usec += (_timeout * 1000);
         ts.tv_sec  += (usec / 1000000);
         ts.tv_nsec  = (usec % 1000000) * 1000;
      }

      pthread_mutex_lock(&mutex);

      // Note: the while loop is used to handle spurious wakeup, see http://en.wikipedia.org/wiki/Spurious_wakeup
      while(!b_condition) 
      {
         if(_timeout)
         {
            //
            // release mutex, wait for condition/signal, then acquire mutex again
            //
            r = pthread_cond_timedwait(&cond, &mutex, &ts);
            // tkscript->printf("xxx pthread_cond_timedwait returned r=%d\n", r);
            if(ETIMEDOUT == r)
               break;
         }
         else
         {
            r = pthread_cond_wait(&cond, &mutex);
         }

         // Note: because of spurious wakeup, b_condition may still be 0.
         //       TODO: correctly handle timeout? (calc timeout *before* while loop..)

         // r => ETIMEDOUT, 0..
      }

      if(ETIMEDOUT != r)
      {
         if(!b_manual_reset)
         {
            // auto-reset: allow only one thread to start running after waiting for the condition to be raised.
            //             other threads need to wait for the next signal.
            b_condition = YAC_FALSE;
         }
         // else: let all threads "pass" until flag is manually reset (see reset())
      }

      pthread_mutex_unlock(&mutex);
   }
   else
   {
      r = -1;
   }

   return r;
}

sSI TKS_Condition::reset(void) {
   sSI r;

   if(b_created && b_manual_reset)
   {
      pthread_mutex_lock(&mutex);
      b_condition = YAC_FALSE;
      pthread_mutex_unlock(&mutex);
      r = 0;
   }
   else
   {
      r = -1;
   }

   return r;
}

// end DX_PTHREADS
#else


#ifdef YAC_WIN32
TKS_Condition::TKS_Condition(void) {
   event     = NULL;
   b_created = YAC_FALSE;
}

TKS_Condition::~TKS_Condition() {
   if(b_created)
   {
      b_created = YAC_FALSE;
      ::CloseHandle(event);
      event = NULL;
   }
}

sSI TKS_Condition::create(sSI _bManualReset) {
   sSI r;

   if(!b_created)
   {
      event = ::CreateEvent(NULL/*lpEventAttributes*/, _bManualReset, 0/*bInitialState*/, NULL/*lpName*/);
      r = GetLastError();
      b_created = YAC_TRUE;
   }
   else
   {
      Dprintf("[---] TKS_Condition::create: already created.\n");
      r = -1;
   }

   return r;
}

sSI TKS_Condition::raise(void) {
   sSI r;

   if(b_created)
   {
      if(::SetEvent(event))
      {
         r = 0;
      }
      else
      {
         r = GetLastError();
      }
   }
   else
   {
      Dprintf("[---] TKS_Condition::raise: not created.\n");
      r = -1;
   }

   return r;
}

sSI TKS_Condition::wait(sSI _timeout) {
   sSI r;

   if(b_created)
   {
      if(_timeout)
      {
         r = ::WaitForSingleObject(event, _timeout);
      }
      else
      {
         r = ::WaitForSingleObject(event, INFINITE);
      }

      // r => WAIT_ABANDONED (0x80), WAIT_OBJECT_0 (0), WAIT_TIMEOUT (0x102)
   }
   else
   {
      Dprintf("[---] TKS_Condition::wait: not created.\n");
      r = -1;
   }

   return r;
}

sSI TKS_Condition::reset(void) {
   sSI r;

   if(b_created)
   {
      if(::ResetEvent(event))
      {
         r = 0;
      }
      else
      {
         r = ::GetLastError();
      }
   }
   else
   {
      Dprintf("[---] TKS_Condition::reset: not created.\n");
      r = -1;
   }

   return r;
}


#endif // YAC_WIN32


#endif // DX_PTHREADS
