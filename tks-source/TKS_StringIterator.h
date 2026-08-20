/// TKS_StringIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_STRINGITERATOR_h__
#define TKS_STRINGITERATOR_h__


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


#endif // TKS_STRINGITERATOR_h__
