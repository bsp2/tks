/// TKS_StringIterator.cpp
/// (c) 2001-2005 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "YAC_String.h"
#include "TKS_StringIterator.h"


TKS_StringIterator::TKS_StringIterator(void) {
   root       = 0;
   sn_current = 0;
}

TKS_StringIterator::~TKS_StringIterator() {
   root       = 0;
   sn_current = 0;
}

void TKS_StringIterator::begin(const YAC_String *_r) {
   if(_r)
   {
      root=_r;
      if(root->clones)
      {
         sn_current=root->clones;
         if(sn_current) 
         {
            current=(YAC_String*)sn_current->data; 
         }
         else 
         {
            current=0; 
         }
      }
      else
      {
         sn_current=0;
         current=0;
      }
   }
   else
   {
      root=0;
      sn_current=0;
      current=0;
   }
}

void TKS_StringIterator::end(void) {
   root=0;
   sn_current=0;
   current=0;
}

YAC_String *TKS_StringIterator::head(void) {
   if(sn_current)
   {
      sn_current=StaticList::Head(sn_current);
      current=(YAC_String*)sn_current->data;
      return current;
   }
   return 0;
}

YAC_String *TKS_StringIterator::tail(void) {
   if(sn_current)
   {
      sn_current=StaticList::Tail(sn_current);
      current=(YAC_String*)sn_current->data;
      return current;
   }
   return 0;
}

YAC_String *TKS_StringIterator::next(void) {
   if(sn_current)
   {
      if(sn_current->next)
      {
         sn_current=sn_current->next;
         current=(YAC_String*)sn_current->data;
         return current;
      }
   }
   return 0;
}

YAC_String *TKS_StringIterator::previous(void) {
   if(sn_current)
   {
      if(sn_current->previous)
      {
         sn_current=sn_current->previous;
         current=(YAC_String*)sn_current->data;
         return current;
      }
   }
   return 0;
}

sSI TKS_StringIterator::_head(void) {
   return (sSI)(head()!=0);
}

sSI TKS_StringIterator::_tail(void) {
   return (tail()!=0);
}

sSI TKS_StringIterator::_next(void) {
   return (next()!=0);
}

sSI TKS_StringIterator::_previous(void) {
   return (previous()!=0);
}

YAC_String *TKS_StringIterator::_getCurrent(void) {
   if(sn_current->data) 
   {
      return (YAC_String*)sn_current->data; 
   }
   else 
   {
      return 0; 
   }
}
