/// TKS_ConfigurationInter.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "TKS_Configuration.h"
#include "TKS_ConfigurationInter.h"


TKS_ConfigurationInter::TKS_ConfigurationInter(void) {
}

TKS_ConfigurationInter::~TKS_ConfigurationInter() {
}

void TKS_ConfigurationInter::setDebugLevel(sSI _d) {
    tkscript->configuration.debug_level = (sU8) _d;
}

sSI TKS_ConfigurationInter::getDebugLevel(void) {
  return tkscript->configuration.debug_level;
}

void TKS_ConfigurationInter::setForceInt(sSI _i) {
  tkscript->configuration.b_forceint = (0 != _i);
}

sSI TKS_ConfigurationInter::getForceInt(void) {
  return tkscript->configuration.b_forceint;
}

void TKS_ConfigurationInter::setThreadTerminateOnException(sBool _bEnable) {
   tkscript->configuration.b_thread_terminate_on_exception = _bEnable;
}

void TKS_ConfigurationInter::setLockGlobalContext(sBool _bEnable) {
   tkscript->configuration.b_lock_global_context = _bEnable;
}
