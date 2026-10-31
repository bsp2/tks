/// TKS_ConfigurationInter.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"
#include "TKS_Configuration.h"
#include "TKS_ConfigurationInter.h"


TKS_ConfigurationInter::TKS_ConfigurationInter(void) {
}

TKS_ConfigurationInter::~TKS_ConfigurationInter() {
}

void TKS_ConfigurationInter::setLogLevel(sSI _d) {
   tkscript->configuration.log_level = _d;
}

sSI TKS_ConfigurationInter::getLogLevel(void) {
  return tkscript->configuration.log_level;
}

sBool TKS_ConfigurationInter::getEnableLogInfo(void) {
  return tkscript->configuration.b_log_info;
}

sBool TKS_ConfigurationInter::getEnableLogVerbose(void) {
  return tkscript->configuration.b_log_verbose;
}

sBool TKS_ConfigurationInter::getEnableLogSuccess(void) {
  return tkscript->configuration.b_log_success;
}

sBool TKS_ConfigurationInter::getEnableLogWarning(void) {
  return tkscript->configuration.b_log_warning;
}

sBool TKS_ConfigurationInter::getEnableLogError(void) {
  return tkscript->configuration.b_log_error;
}

sBool TKS_ConfigurationInter::getEnableLogFatal(void) {
  return tkscript->configuration.b_log_fatal;
}

sBool TKS_ConfigurationInter::getEnableLogProfile(void) {
  return tkscript->configuration.b_log_profile;
}

sBool TKS_ConfigurationInter::getEnableLogDebug(void) {
  return tkscript->configuration.b_log_debug;
}

void TKS_ConfigurationInter::setDebugLevel(sUI _d) {
   tkscript->configuration.debug_level = _d;
}

sUI TKS_ConfigurationInter::getDebugLevel(void) {
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
