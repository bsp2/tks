/// TKS_StringIterator.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_STRINGITERATOR_h__
#define __TKS_STRINGITERATOR_h__

 
class TKS_StringIterator : public YAC_Object {
public:
   const YAC_String  *root;
   StaticList::Node  *sn_current;
   YAC_String        *current;
   
public:
   void        begin               (const YAC_String *_r);
   void        end                 (void);
   YAC_String *head                (void);
   YAC_String *tail                (void);
   YAC_String *next                (void);
   YAC_String *previous            (void);
   
public:
   TKS_StringIterator(void);
   ~TKS_StringIterator();
   
   sSI         _head               (void);
   sSI         _tail               (void);
   sSI         _next               (void);
   sSI         _previous           (void);
   YAC_String *_getCurrent         (void);
};


#endif // __TKS_STRINGITERATOR_h__

