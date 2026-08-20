/// StaticList.cpp
///
/// (c) 1999-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///

#include <stdlib.h>
#include <stdio.h>

#include "tks.h"

StaticList::StaticList(void) { }

StaticList::~StaticList(void) { }

StaticList::Node *StaticList::Head(StaticList::Node *a) {
  while(a && a->previous) a=a->previous;
  return a;
}

StaticList::Node *StaticList::Tail(StaticList::Node *a) {
  while(a && a->next) a=a->next;
  return a;
}

void StaticList::Free( StaticList::Node **a ) {
  StaticList::Node *b;

  while((b=*a)) {
    *a=b->next;
    b->data=0;
    b->previous=0;
    b->next=0;
    ::free((void *)b);
  }
}

void StaticList::CreateEmpty(StaticList::Node **a) {
  if(a)
  {
    *a = STATICLIST_NEW_NODE;
    if(*a)
    {
	    (*a)->previous = NULL;
	    (*a)->next     = NULL;
	    (*a)->data     = NULL;
    }
  }
}

StaticList::Node *StaticList::Insert( StaticList::Node **a, void *b ) {
  if(a)
  {
    if(!*a)
    {
	    StaticList::CreateEmpty(a);
	    (*a)->data = b;
	    return *a;
    }
    return *a = StaticList::Insert(*a, b);
  }
  return NULL;
}

StaticList::Node *StaticList::Insert(StaticList::Node *a, void *b) {
  StaticList::Node *p;
  p= STATICLIST_NEW_NODE;
  if(!p) return NULL;
  p->data     = b;
  p->previous = a->previous;
  p->next     = a;
  if(a->previous) a->previous->next = p;
  a->previous = p;
  return p;
}

StaticList::Node *StaticList::InsertHead(StaticList::Node **a, void *b) {
	if(a)
	{
		if(!*a)
		{
			StaticList::CreateEmpty(a);
			(*a)->data = b;
			return *a;
		}
		else
			StaticList::Insert(a,b);
	}
	return NULL;
}

void StaticList::Remove( StaticList::Node *a ) {
   if(a)
   {
      if(a->next)
      {
         a->next->previous = a->previous;
      }
      if(a->previous)
      {
         a->previous->next = a->next;
      }
      ::free((void*)a);
   }
}

void StaticList::Remove ( StaticList::Node **l, StaticList::Node *n) {
   if(n->previous)
   {
      n->previous->next = n->next;
   }
   else
   {
      if(n == (*l))
      {
         *l = n->next;
      }
   }
   if(n->next)
   {
      n->next->previous = n->previous;
   }
   n->previous = n->next = NULL;
   ::free((void*)n);
}


StaticList::Node *StaticList::Append( StaticList::Node *a, void *b ) {
   if(a->next)
   {
      return StaticList::Insert(a->next, b);
   }
   a->next = STATICLIST_NEW_NODE;
   if(!a->next)
   {
      Dprintf("[---] StaticList::Append: cannot allocate node.\n");
      return NULL;
   }
   a->next->previous = a;
   a = a->next;
   a->next = 0;
   a->data = b;
   return a;
}

StaticList::Node *StaticList::Append( StaticList::Node **a, void *b ) {
   if(a)
   {
      if(!(*a))
      {
         StaticList::CreateEmpty(a);
         (*a)->data = b;
         return *a;
      }
      return StaticList::Append(*a, b);
   }
   return NULL;
}
