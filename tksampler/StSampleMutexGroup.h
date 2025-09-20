// ----
// ---- file   : StSampleMutexGroup.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ---- changed: 21Jan2010, 03Oct2024
// ----
// ----
// ----

#ifndef ST_SAMPLEMUTEXGROUP_H__
#define ST_SAMPLEMUTEXGROUP_H__

YG("tksampler")


YC class StSampleMutexGroup : public YAC_Object {
  public:
   StSampleMutexGroup *next;
   YAC_String         *name;

  public:
   StSampleMutexGroup(void);
   ~StSampleMutexGroup();

   YAC(StSampleMutexGroup);

   YM YAC_Object *_getNext(void);
   YM void        _setNext(YAC_Object *_mtxgrp);

   YM YAC_Object *_getName     (void);
   YM void        _setName     (YAC_Object *_str); // copy
};


#endif // ST_SAMPLEMUTEXGROUP_H__
