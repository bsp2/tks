/// TKS_StringIterator.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"
#include "YAC_String.h"
#include "TKS_StringIterator.h"


TKS_StringIterator::TKS_StringIterator(void) {
   root       = NULL;
   sn_current = NULL;
}

TKS_StringIterator::~TKS_StringIterator() {
   root       = NULL;
   sn_current = NULL;
}

void TKS_StringIterator::begin(const YAC_String *_r) {
   if(_r)
   {
      root = _r;
      if(root->clones)
      {
         sn_current = root->clones;
         if(sn_current)
         {
            current = (YAC_String*)sn_current->data;
         }
         else
         {
            current = NULL;
         }
      }
      else
      {
         sn_current = NULL;
         current = NULL;
      }
   }
   else
   {
      root = NULL;
      sn_current = NULL;
      current = NULL;
   }
}

void TKS_StringIterator::end(void) {
   root = NULL;
   sn_current = NULL;
   current = NULL;
}

YAC_String *TKS_StringIterator::head(void) {
   if(sn_current)
   {
      sn_current = StaticList::Head(sn_current);
      current = (YAC_String*)sn_current->data;
      return current;
   }
   return NULL;
}

YAC_String *TKS_StringIterator::tail(void) {
   if(sn_current)
   {
      sn_current = StaticList::Tail(sn_current);
      current = (YAC_String*)sn_current->data;
      return current;
   }
   return NULL;
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
   return NULL;
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
   return NULL;
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
      return NULL;
   }
}
