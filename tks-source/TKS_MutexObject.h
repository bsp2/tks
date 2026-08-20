/// TKS_MutexObject.h
///
/// (c) 2006-2026 by Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///

#ifndef TKS_MUTEXOBJECT_H__
#define TKS_MUTEXOBJECT_H__


class TKS_MutexObject : public YAC_Object {
public:
   TKS_Mutex mutex;

public:

   TKS_MutexObject(void);
   ~TKS_MutexObject();

   void  lock      (void);
   sBool timedLock (sUI _timeoutSec);
   void  unlock    (void);
};


#endif // TKS_MUTEXOBJECT_H__
