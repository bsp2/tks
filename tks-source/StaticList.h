/// staticlist.h 
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 

#ifndef __TKS_STATICLIST_H__
#define __TKS_STATICLIST_H__
 
class StaticList {
public:
  struct Node {
        struct Node *previous;
        struct Node *next;
        void        *data; /// pointer to data 
  };

 public:
                           StaticList  ( void );
                          ~StaticList  ( void );

  static StaticList::Node *Head        ( StaticList::Node  *a );
  static StaticList::Node *Tail        ( StaticList::Node  *a );
  static void              Free        ( StaticList::Node **a );
  static void              CreateEmpty ( StaticList::Node **a );
  static StaticList::Node *Insert      ( StaticList::Node  *a, void *b );
  static StaticList::Node *Insert      ( StaticList::Node **a, void *b );
  static StaticList::Node *InsertHead  ( StaticList::Node **a, void *b ); 
  static void              Remove      ( StaticList::Node *a );
  static void              Remove      ( StaticList::Node **l, StaticList::Node *n);
  static StaticList::Node *Append      ( StaticList::Node *a, void *b );
  static StaticList::Node *Append      ( StaticList::Node **a, void *b );
};
 
#define STATICLIST_NEW_NODE (StaticList::Node*)(::malloc( sizeof(StaticList::Node) )) 


#endif // __TKS_STATICLIST_H__
