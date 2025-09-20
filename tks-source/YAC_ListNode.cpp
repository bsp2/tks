/// YAC_ListNode.cpp
///
/// (c) 2002-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

// #include <stdlib.h>
// #include <string.h>

#include "tks.h"

#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "PTN_Node.h"
#include "YAC_ListNode.h"
#include "YAC_ListNodeIterator.h"


YAC_ListNode::YAC_ListNode(void) {
   class_ID = YAC_CLID_LISTNODE;
   prev = next = NULL; 
   b_immutable = 0;
}

YAC_ListNode::~YAC_ListNode() {
   freeList();
}

void YAC_ListNode::freeList(void) {
   while(next)
   {
      YAC_ListNode *n = next->next;
      next->next = NULL; // Avoid recursive destructor calls!
      YAC_DELETE(next);
      next = n;
   }
   unset();
} 
 
void YAC_ListNode::_freeList(void) { 
   if(!b_immutable) 
   { 
      freeList(); 
   } 
   else 
   { 
      Dprintf("[---] ListNode::freeList(): cannot operate on immutable node.\n"); 
   } 
}

YAC_ListNode *YAC_ListNode::getHead(void) {
   YAC_ListNode *r=this;
   while(r->prev) 
   {
      r=r->prev; 
   }
   return r;
}

YAC_ListNode *YAC_ListNode::getTail(void) {
   YAC_ListNode *r=this;
   while(r->next) 
   {
      r=r->next; 
   }
   return r;
}

YAC_ListNode *YAC_ListNode::getPrev(void) {
   return prev;
}

YAC_ListNode *YAC_ListNode::getNext(void) {
   return next;
}

void YAC_ListNode::appendNode(YAC_ListNode *_n) { 
   if(!next)
   {
      next = _n;
      next->prev = this; 
   }
   else
   {
      next->prev = _n;
      _n->prev = this;
      _n->next = next;
      next = _n;
   }
}

YAC_ListNode *YAC_ListNode::appendEmpty(void) {
   YAC_ListNode *n = (YAC_ListNode*) YAC_NEW_CORE_POOLED(YAC_CLID_LISTNODE);
   n->b_immutable=b_immutable;
   if(n) 
   {
      appendNode(n); 
   }
   return next;
} 
 
YAC_ListNode *YAC_ListNode::_appendEmpty(void) { 
   if(b_immutable) 
   { 
      Dprintf("[---] ListNode::appendEmpty(): cannot operate on immutable node.\n"); 
      return 0; 
   } 
   else 
   { 
      return appendEmpty(); 
   } 
}
 
YAC_ListNode *YAC_ListNode::_appendValue(YAC_Object *_v) { // deprecated 
   return YAC_ListNode::_append(_v); 
} 
 
YAC_ListNode *YAC_ListNode::_append(YAC_Object *_v) { 
   if(b_immutable) 
   { 
      Dprintf("[---] ListNode::append(): cannot operate on immutable node.\n"); 
      return 0; 
   } 
   else 
   { 
      return append(_v); 
   } 
} 

YAC_ListNode *YAC_ListNode::append(YAC_Object *_v) {
   if(YAC_BCHK(_v, YAC_CLID_VALUE))
   {
      YAC_ValueObject *v=(YAC_ValueObject*)_v;
      if(appendEmpty()) 
      {
         next->copySafe(v); 
      }
      return next;
   }
   return 0;
}

YAC_ListNode *YAC_ListNode::concatListRef(YAC_ListNode *_l) {
   YAC_ListNode *c=getTail();
   while(_l&&c->appendEmpty())
   {
      c=c->next;
      c->initValue(_l);
      _l=_l->next;
   }
   return c;
}

YAC_ListNode *YAC_ListNode::concatListGrab(YAC_ListNode *_l) {
   YAC_ListNode *c=getTail();
   while(_l&&c->appendEmpty())
   {
      c=c->next;
      c->copySafe(_l);
      _l=_l->next;
   }
   return c;
}

void YAC_VCALL YAC_ListNode::yacOperatorAssign(YAC_Object *_o) { 
   if(_o==this) 
      return;
   if((!b_immutable)&&YAC_CHK(_o, YAC_CLID_LISTNODE))
   {
      //assignListGrab((YAC_ListNode*)_o);
      YAC_ListNode *c=this;
      if(next)
      {
         YAC_DELETE(next);
         next=0;
      }
      YAC_ListNode *l=(YAC_ListNode*)_o;
      while(l&&c)
      {
         c->copySafe(l);
         l=l->next;
         if(l) 
         {
            c=c->appendEmpty(); 
         }
      }
   }
   else 
   {
      if(b_immutable) 
      { 
         Dprintf("[---] ListNode::operator=: cannot operate on immutable node.\n"); 
      } 
      else 
      { 
         initObject(_o, 0); 
      } 
   }
}

void YAC_VCALL YAC_ListNode::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o == this) 
   { 
      YAC_Object::yacOperator(_cmd, _o, _r);
      return; 
   } 
   switch(_cmd) {
   default:
      YAC_ValueObject::yacOperator(_cmd, _o, _r);
      break;
      
   case YAC_OP_ADD: 
      if(!b_immutable) 
      { 
         if(YAC_CHK(_o, YAC_CLID_LISTNODE)) 
         {
            concatListRef((YAC_ListNode*)_o); 
         }
         else
         {
            YAC_ListNode *c=getTail()->appendEmpty();
            if(c) 
            {
               if(YAC_BCHK(_o, YAC_CLID_VALUE)) 
               {
                  c->initValue((YAC_ValueObject*)_o); 
               }
               else 
               {
                  c->initObject(_o, 0); 
               } 
            }
         } 
      } 
      else 
      { 
         Dprintf("[---] ListNode::operator+: cannot operate on immutable node.\n"); 
      }
      break;
   case YAC_OP_EOR:
      if(!b_immutable) 
      { 
         if(YAC_CHK(_o, YAC_CLID_LISTNODE)) 
         {
            concatListGrab((YAC_ListNode*)_o); 
         }
         else
         {
            YAC_ListNode *c = getTail()->appendEmpty();
            if(c) 
            {
               if(YAC_BCHK(_o, YAC_CLID_VALUE)) 
               {
                  c->copySafe((YAC_ValueObject*)_o); 
               }
               else 
               {
                  c->initObject(_o, 0); 
               } 
            }
         } 
      }
      else 
      { 
         Dprintf("[---] ListNode::operator^: cannot operate on immutable node.\n"); 
      } 
      break; 
      
   }
}

void YAC_ListNode::_getString(YAC_Value *_r) const {
   _r->initEmptyString();
   yacToString(_r->value.string_val);
}

sBool YAC_VCALL YAC_ListNode::yacToString(YAC_String *_s) const {
   YAC_String t;
   t.alloc(192);
   _s->alloc(512);
   _s->copy("{");
   const YAC_ListNode *c=this;
   do  
   {
      switch(c->type)
      {
      case 0:
         _s->append("void");
         break;
      case 1:
         t.printf("%i", c->value.int_val);
         _s->append(&t);
         break;
      case 2:
         t.printf("%g", c->value.float_val);
         _s->append(&t);
         break;
      case 3:
      case 4:
         if(YAC_VALID(c->value.object_val))
         {
            // Note: yacToString adds \"\" if object is already a string
            c->value.object_val->yacToString(&t);
            _s->append(&t);
         }
         else 
         {
            _s->append("<null>"); 
         }
         break;
      ////case 4:
      ////   if(c->value.string_val)
      ////   {
      ////      // xxx c->value.string_val->yacToString(_s)
      ////      _s->append("\""); _s->append(c->value.string_val); _s->append("\"");
      ////   }
      ////   break;
      }
      c=c->next;
      if(c) 
      {
         _s->append(", "); 
      }
   } while(c);
    
   _s->append("}");
   return 1;
}

void YAC_ListNode::toDebugString(YAC_String *_r) {
   YAC_Value sv;
   getStringValue(&sv);
   _r->printf("prev: %p this:<%p> next: %p (type=%s)\n"
      "\tint=%i\n\tfloat=%g\n\ttoString=\"%s\" (del=%s)\n\n",
      (void*)prev, (void*)this, (void*)next,
      (char*)
      ((type==1)?"int":(type==2)?"float":(type==3)?(value.object_val?value.object_val->yacClassName():"Object"):(type==4)?"String":"???"),
      value.int_val, value.float_val, sv.value.string_val->chars, (deleteme?"yes":"no")
      );
   sv.unset();
}

void YAC_ListNode::_getDebugString(YAC_Value *_r) {
   YAC_String *r = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   r->alloc(192);
   toDebugString(r);
   _r->initString(r, 1);
}

void YAC_ListNode::_getDebugStrings(YAC_Value *_r) {
   YAC_String *r = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   YAC_String t;
   t.alloc(192);
   r->alloc(1024);  
   r->empty();
   YAC_ListNode *c = this;
   ////YAC_ListNode *p = c->prev;
   while(c)
   {
      c->toDebugString(&t);
      r->append(&t);
      ////p=c;
      c=c->next;
   }
   _r->initString(r, 1);
}

void YAC_VCALL YAC_ListNode::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   YAC_ListNode *c = this;
   if( ((sSI)_index)>0 )
   {
      sUI i=0;
      while( (i++<_index) && c->next ) 
      {
         c=c->next; 
      }
   }
   if(c != NULL)
   {
      /////_r->initObject(c, 0); // xxx old behaviour was to return ListNode (use get() instead now)
      _r->initValue(c); // Return reference to value
   }
   else
   {
      _r->initVoid(); 

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("ListNode index >>%i<< out of bounds (max=%i)", _index, yacArrayGetNumElements());
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
      ////_v->initNull(); 
   }
}
 
void YAC_VCALL YAC_ListNode::yacArrayGetDeref(void *_context, sUI _index, YAC_Value *_r) {
   YAC_ListNode *c = this;
   if( ((sSI)_index)>0 )
   {
      sUI i=0;
      while( (i++<_index) && c->next ) 
      {
         c=c->next; 
      }
   }
   if(c != NULL)
   {
      /////_r->initObject(c, 0); // xxx old behaviour was to return ListNode (use get() instead now)
      _r->copySafe(c); // Return deletable value
   }
   else
   {
      _r->initVoid(); 

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("ListNode index >>%i<< out of bounds (max=%i)", _index, yacArrayGetNumElements());
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
      ////_v->initNull(); 
   }
}

YAC_Object *YAC_ListNode::getSubList(sSI _index) { 
   if( ((sSI)_index)>0) 
   { 
      sSI i=0; 
      YAC_ListNode *c=this; 
      while( (i++<_index) && c->next ) 
      { 
         c=c->next; 
      } 
      if(i==(_index+1)) 
      { 
         return c; 
      } 
   } 
   return yac_null; 
} 

void YAC_VCALL YAC_ListNode::yacArraySet(void *_context, sUI _index, YAC_Value *_v) {

   (void)_context;

   YAC_ListNode *c=this;
   if(_index)
   {
      sUI i=0;
      while( (i++<_index) && c->next ) 
      {
         c=c->next; 
      }
   }
   c->copySafe(_v);
}


sBool YAC_VCALL YAC_ListNode::yacIteratorInit(YAC_Iterator *_it) const {
   Diteratorinit(YAC_ListNodeIterator);
   return 1;
}

void YAC_ListNode::_getCopy(YAC_Value*_r) {
   YAC_ListNode *c=this;
   YAC_ListNode *r=(YAC_ListNode*)YAC_NEW_CORE_POOLED(YAC_CLID_LISTNODE);
   YAC_ListNode *rc=r;
   while(c)
   {
      rc->initValue(c);
      c=c->next;
      if(c)
      {
         rc->appendEmpty();
         rc=rc->next;		
      }
   }
   YAC_RETO(r, 1);
}

sUI YAC_ListNode::_getSize(void) { 
   return yacArrayGetNumElements();
}
 
sUI YAC_VCALL YAC_ListNode::yacArrayGetNumElements(void) { 
   sUI r=0; 
   YAC_ListNode *c=this; 
   while(c) 
   { 
      r++;  
      c=c->next; 
   } 
   return r; 
} 
 
sUI YAC_VCALL YAC_ListNode::yacArrayGetMaxElements(void) { 
   return yacArrayGetNumElements(); 
} 
