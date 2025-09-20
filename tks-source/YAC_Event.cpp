/// YAC_Event.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
///
#include "tks.h"
#include "YAC_String.h"
#include "YAC_ValueObject.h"
#include "YAC_Event.h"
 
YAC_Event::YAC_Event(void) {
  class_ID = YAC_CLID_EVENT;
  id     = 0;
  sub_id = 0;
}

YAC_Event::~YAC_Event() { 
}

void YAC_VCALL YAC_Event::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   YAC_Object::yacOperator(_cmd, _o, _r);  

   if(YAC_OP_ASSIGN == _cmd)
   {
      YAC_Event::yacOperatorAssign(_o);
   }
}

void YAC_Event::yacOperatorAssign(YAC_Object *_o) {
   if(YAC_CHK(_o, YAC_CLID_EVENT))
   {
      YAC_Event *o = (YAC_Event*)_o;
      id = o->id;
      sub_id = o->id;
   }
}

void YAC_Event::setId(sSI _id) {
   id = _id;
}

sSI YAC_Event::getId(void) {
   return id;
}

void YAC_Event::setSubId(sSI _subId) {
   sub_id = _subId;
}

sSI YAC_Event::getSubId(void) {
   return sub_id;
}
