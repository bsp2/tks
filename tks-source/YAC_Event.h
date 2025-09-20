/// YAC_Event.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
///
#ifndef __YAC_EVENT_H__
#define __YAC_EVENT_H__


class YAC_Event : public YAC_ValueObject {
public:
   sSI id;
   sSI sub_id;
   
public:
   YAC_Event             (void);
   ~YAC_Event            ();
   
   void        YAC_VCALL yacOperatorAssign (YAC_Object*);
   void        YAC_VCALL yacOperator       (sSI, YAC_Object *, YAC_Value *_r);
   
   void setId    (sSI _id);
   sSI  getId    (void);

   void setSubId (sSI _subId);
   sSI  getSubId (void);

};


#endif // YAC_EVENT
