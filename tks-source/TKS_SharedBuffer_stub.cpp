/// TKS_SharedBuffer_stub.cpp
///
/// (c) 2005-2006 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include <string.h>
#include "tks.h"

#include "PTN_Node.h"
#include "YAC_Buffer.h"
#include "TKS_SharedBuffer.h"

void TKS_SharedBuffer::freeSharedMemory(void) {
#ifdef TKS_DCON
   tkscript->printf("[---] TKS_SharedBuffer::freeSharedMemory: stub.\n");
#endif
}

sSI TKS_SharedBuffer::allocSharedMemoryByKey(YAC_String *_shmKey, sSI _size) {
#ifdef TKS_DCON
   tkscript->printf("[---] TKS_SharedBuffer::allocSharedMemory: stub.\n");
#endif
   return 0;
}


sSI TKS_SharedBuffer::visitSharedMemory (YAC_String *_shmKey, sSI _size) {
#ifdef TKS_DCON
   tkscript->printf("[---] TKS_SharedBuffer::visitSharedMemory: stub.\n");
#endif
   return 0;
}


