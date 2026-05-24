// ----
// ---- file   : StSampleMutexGroup.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2009-2026 by bsp
// ----
// ----          Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// ----          associated documentation files (the "Software"), to deal in the Software without restriction, including
// ----          without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// ----          copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to
// ----          the following conditions:
// ----
// ----          The above copyright notice and this permission notice shall be included in all copies or substantial
// ----          portions of the Software.
// ----
// ----          THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// ----          NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// ----          IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// ----          WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// ----          SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ---- changed: 21Jan2010, 24May2026
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
