// ----
// ---- file   : StSampleMutexGroup.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2010 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ---- changed: 21Jan2010
// ----
// ----
// ----

#include "tksampler.h"


StSampleMutexGroup::StSampleMutexGroup(void) {
   next = NULL;
   name = NULL;
}

StSampleMutexGroup::~StSampleMutexGroup() {
   YAC_DELETE_SAFE(name);
}

YAC_Object *StSampleMutexGroup::_getNext(void) {
   return next;
}

void StSampleMutexGroup::_setNext(YAC_Object *_mtxgrp) {
   if(YAC_CHK(_mtxgrp, clid_StSampleMutexGroup))
   {
      next = (StSampleMutexGroup*) _mtxgrp;
   }
   else
   {
      next = NULL;
   }
}

YAC_Object *StSampleMutexGroup::_getName(void) {
   return name;
}

void StSampleMutexGroup::_setName(YAC_Object *_str) {
   if(NULL == name)
   {
      name = YAC_New_String();
   }
   name->yacCopy(_str);
}
