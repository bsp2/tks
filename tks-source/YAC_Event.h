/// YAC_Event.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///

#ifndef YAC_EVENT_H__
#define YAC_EVENT_H__


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


#endif // YAC_EVENT_H__
