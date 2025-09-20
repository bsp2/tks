/// TKS_MutexObject.cpp 
/// 
/// (c) 2006-2021 by Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 
/// 
#include "tks.h" 
 
#include "TKS_Mutex.h" 
#include "TKS_MutexObject.h" 
 
 
 
TKS_MutexObject::TKS_MutexObject(void) { 
} 
 
TKS_MutexObject::~TKS_MutexObject() { 
} 
 
void TKS_MutexObject::lock(void) { 
   mutex.lock();
} 

sBool TKS_MutexObject::timedLock(sUI _timeoutSec) { 
   return mutex.timedLock(_timeoutSec);
} 
 
void TKS_MutexObject::unlock(void) { 
   mutex.unlock();
} 
