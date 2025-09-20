/// TKS_MutexObject.h 
/// 
/// (c) 2006-2021 by Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 
/// 
#ifndef __TKS_MUTEXOBJECT_H__ 
#define __TKS_MUTEXOBJECT_H__ 

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

#endif 
