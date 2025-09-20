/// YAC_List.cpp
///
/// (c) 2003-2008 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "YAC_ListNode.h"
#include "StaticList.h"
#include "PTN_Node.h"
#include "YAC_List.h"
#include "TKS_ListIterator.h"

YAC_List::YAC_List(void) {
   head = NULL;
   tail = NULL;
}

YAC_List::~YAC_List() {
   freeList();
}

void YAC_List::freeList(void) {
   if(head)
   {
      YAC_DELETE(head);
      head = tail = NULL;
   }
}

void YAC_VCALL YAC_List::yacArrayGet(void *_context, sUI _index, YAC_Value *_v) {
   if(head)
   {
      head->yacArrayGet(_context, _index, _v);
   }
   else
   {
      _v->initVoid();

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("List index >>%i<< out of bounds (max=%i)", _index, yacArrayGetNumElements());
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
      ////_v->initNull();
   }
}

YAC_Object *YAC_List::getSubList(sSI _index) {
   if(_index > 0)
   {
      sSI i = 0;
      YAC_ListNode *c = head;
      if(head)
      {
         while( (i++ < _index) && c->next )
            c = c->next;

         if(i == (_index+1))
         {
            return c;
         }
      }
   }
   return yac_null;
}

void YAC_VCALL YAC_List::yacArraySet(void *_context, sUI _index, YAC_Value *_v) {
   if(head)
      head->yacArraySet(_context, _index, _v);
}

void YAC_VCALL YAC_List::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   switch(_cmd)
   {
   case YAC_OP_ASSIGN:
      {
         YAC_ListNode *ol = NULL;
         freeList();
         if(YAC_CHK(_o, YAC_CLID_LIST))
         {
            YAC_List *o = (YAC_List*)_o;
            ol = o->head;
         }
         else if(YAC_CHK(_o, YAC_CLID_LISTNODE))
         {
            ol = (YAC_ListNode*)_o;
         }
         if(ol)
         {
            ol->_getCopy(_r);
            head = (YAC_ListNode*)_r->value.object_val;
            tail = head->getTail();
            // ---- add immutable tag
            ol = head;
            while(YAC_VALID(ol))
            {
               ol->b_immutable = YAC_TRUE;
               ol = ol->next;
            }
            _r->initVoid();
         }
         else
         {
            // simply put _o in a one element list
            _addFirst(_o);
         }
      }
      break;
   case YAC_OP_ADD:
      _addLast(_o);
      break;
   case YAC_OP_EOR:
      {
         YAC_ListNode *ol = NULL;
         if(YAC_CHK(_o, YAC_CLID_LIST))
         {
            YAC_List *o = (YAC_List*)_o;
            ol = o->head;
         }
         else if(YAC_CHK(_o, YAC_CLID_LISTNODE))
         {
            ol = (YAC_ListNode*)_o;
         }
         if(ol)
         {
            if(head)
            {
               tail = head->concatListGrab(ol);
            }
            else
            {
               head = (YAC_ListNode*) YAC_NEW_CORE_POOLED(YAC_CLID_LISTNODE);
               head->b_immutable = YAC_TRUE;
               ((YAC_ValueObject*)head)->copySafe(ol);
               tail=head->concatListGrab(ol->next);
            }
         }
      }
      break;
   case YAC_OP_CEQ:
      _r->initInt(compareToObject(_o));
      break;
   case YAC_OP_CNE:
      _r->initInt(!compareToObject(_o));
      break;
   default:
      YAC_Object::yacOperator(_cmd, _o, _r);
      break;
   }
}

sBool YAC_List::compareToObject(YAC_Object *_o) {
   YAC_ListNode *ol = NULL;
   if(YAC_CHK(_o, YAC_CLID_LIST))
   {
      YAC_List *o = (YAC_List*)_o;
      ol = o->head;
   }
   else if(YAC_CHK(_o, YAC_CLID_LISTNODE))
   {
      ol = (YAC_ListNode*)_o;
   }

   if(ol)
   {
      YAC_ListNode *l = head;
      sBool bsame = YAC_TRUE;
      while(bsame && ol && l)
      {
         bsame = ol->compareValue(l);
         ol = ol->next;
         l = l->next;
      }
      return bsame && (NULL == ol) && (NULL == l);
   }
   return YAC_FALSE;
}

sBool YAC_VCALL YAC_List::yacIteratorInit(YAC_Iterator *_it) const {
   Diteratorinit(TKS_ListIterator);
   return YAC_TRUE;
}

void YAC_List::_getCopy(YAC_Value *_r) {
   YAC_List *r = (YAC_List*)YAC_NEW_CORE_POOLED(YAC_CLID_LIST);
   if(head)
   {
      head->_getCopy(_r);
      r->head = (YAC_ListNode*)_r->value.object_val;
   }
   _r->initObject(r, 1);
}

sUI YAC_List::_getSize(void) {
   return head?head->_getSize():0;
}

sSI YAC_List::isEmpty(void) {
   return (NULL == head);
}

void YAC_List::_getString(YAC_Value *_r) const {
   if(head)
   {
      head->_getString(_r);
   }
   else
   {
      YAC_String *r = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
      r->visit("{}");
      _r->initString(r, 1);
   }
}

sBool YAC_VCALL YAC_List::yacToString(YAC_String *_s) const {
   if(head)
   {
      head->yacToString(_s);
   }
   else
   {
      _s->visit("{}");
   }
   return 1;
}

void YAC_List::_removeAll(YAC_Value *_r) {
   if(head)
   {
      _r->initObject(head, 1);
      head = tail = NULL;
   }
   else
   {
      _r->initObject(0,0);
   }
}

void YAC_List::_removeFirst(YAC_Value *_r) {
   if(head)
   {
      YAC_ListNode *t = head;
      head = head->next;
      if(head)
      {
         head->prev = NULL;
      }
      else
      {
         tail = NULL;
      }
      t->next = t->prev = NULL;
      _r->initObject(t, 1);
   }
   else
   {
      _r->initObject(0,0);
   }
}

void YAC_List::_removeLast(YAC_Value *_r) {
   if(tail)
   {
      YAC_ListNode *t = tail;
      tail = t->prev;
      if(tail)
      {
         tail->next = NULL;
         t->prev = NULL;
      }
      else
      {
         head = NULL;
      }
      _r->initObject(t, 1);
   }
   else
   {
      _r->initObject(0,0);
   }
}

YAC_Object *YAC_List::_addFirst(YAC_Object *_value) {
   YAC_ListNode *t = (YAC_ListNode*) YAC_NEW_CORE_POOLED(YAC_CLID_LISTNODE);
   t->b_immutable = YAC_TRUE;
   if(YAC_BCHK(_value, YAC_CLID_VALUE))
   {
      t->copySafe((YAC_ValueObject*)_value);
   }
   else
   {
      if(_value)
      {
         if(YAC_CLID_STRING == _value->class_ID)
         {
            t->initEmptyString();
            t->value.string_val->yacCopy(_value);
         }
         else
         {
            YAC_Object *no = YAC_CLONE_POOLED(NULL, _value);
            no->yacOperatorAssign(_value);
            t->initObject(no, 1);
         }
      }
      else
      {
         t->initObject(yac_null, 0);
      }
   }
   if(head)
   {
      head->prev = t;
      t->next = head;
   }
   else
   {
      tail = t;
   }
   head = t;
   return t;
}

YAC_Object *YAC_List::_addLast(YAC_Object *_value) {
   YAC_ListNode *ol = NULL;
   if(YAC_CHK(_value, YAC_CLID_LIST))
   {
      YAC_List *o = (YAC_List*)_value;
      ol = o->head;
   }
   else if(YAC_CHK(_value, YAC_CLID_LISTNODE))
   {
      ol = (YAC_ListNode*)_value;
   }
   if(ol)
   {
      if(NULL == ol->next) // don't use concat for single node lists
         ol = NULL;
   }
   if(ol)
   {
      if(head)
      {
         tail = head->concatListRef(ol);
      }
      else
      {
         head = (YAC_ListNode*) YAC_NEW_CORE_POOLED(YAC_CLID_LISTNODE);
         head->b_immutable = YAC_TRUE;
         ((YAC_ValueObject*)head)->initValue(ol);
         tail = head->concatListRef(ol->next);
      }
   }
   else
   {
      YAC_ListNode *t = (YAC_ListNode*) YAC_NEW_CORE_POOLED(YAC_CLID_LISTNODE);
      t->b_immutable = YAC_TRUE;
      if(YAC_BCHK(_value, YAC_CLID_VALUE))
      {
         t->copySafe((YAC_ValueObject*)_value);
      }
      else
      {
         if(_value)
         {
            if(YAC_CLID_STRING == _value->class_ID)
            {
               t->initEmptyString();
               t->value.string_val->yacCopy(_value);
            }
            else
            {
               YAC_Object *no = YAC_CLONE_POOLED(NULL, _value);
               no->yacOperatorAssign(_value);
               t->initObject(no, 1);
            }
         }
         else
         {
            t->initObject(yac_null, 0);
         }
      }
      if(tail)
      {
         tail->next = t;
         t->prev = tail;
         tail = t;
      }
      else
      {
         head = tail = t;
      }
   }
   return tail;
}

YAC_ListNode *YAC_List::addLastNew(void) {
   YAC_ListNode *t = (YAC_ListNode*) YAC_NEW_CORE_POOLED(YAC_CLID_LISTNODE);
   t->b_immutable = YAC_TRUE;
   if(tail)
   {
      tail->next = t;
      t->prev = tail;
      tail = t;
   }
   else
   {
      head = tail = t;
   }
   return t;
}

YAC_ListNode *YAC_List::addFirstNew(void) {
   YAC_ListNode *t = (YAC_ListNode*) YAC_NEW_CORE_POOLED(YAC_CLID_LISTNODE);
   t->b_immutable = YAC_TRUE;
   if(head)
   {
      head->prev = t;
      t->next = head;
      head = t;
   }
   else
   {
      head = tail = t;
   }
   return t;
}

YAC_Object *YAC_List::_addFirstInt(sSI _v) {
   YAC_ListNode *t = addFirstNew();
   t->initInt(_v);
   return t;
}

YAC_Object *YAC_List::_addLastInt(sSI _v) {
   YAC_ListNode *t = addLastNew();
   t->initInt(_v);
   return t;
}

YAC_Object *YAC_List::_addFirstFloat(sF32 _v) {
   YAC_ListNode *t = addFirstNew();
   t->initFloat(_v);
   return t;
}

YAC_Object *YAC_List::_addLastFloat(sF32 _v) {
   YAC_ListNode *t = addLastNew();
   t->initFloat(_v);
   return t;
}

YAC_Object *YAC_List::_addFirstObject(YAC_Object *_oref) {
   YAC_ListNode *t = addFirstNew();
   t->initObject(_oref, 0);
   return t;
}

YAC_Object *YAC_List::_addLastObject(YAC_Object *_oref) {
   YAC_ListNode *t = addLastNew();
   t->initObject(_oref, 0);
   return t;
}

YAC_Object *YAC_List::_addFirstCopy(YAC_Object *_oref) {
   if(YAC_VALID(_oref))
   {
      YAC_ListNode *t = addFirstNew();
      YAC_Object *o = YAC_CLONE_POOLED(NULL, _oref);
      o->yacCopy(_oref); // xxx yacOperatorAssign
      t->initObject(_oref, 1);
      return t;
   }
   return NULL;
}

YAC_Object *YAC_List::_addLastCopy(YAC_Object *_oref) {
   if(YAC_VALID(_oref))
   {
      YAC_ListNode *t = addLastNew();
      YAC_Object *o = YAC_CLONE_POOLED(NULL, _oref);
      o->yacCopy(_oref); /// xxx yacOperatorAssign?
      t->initObject(_oref, 1);
      return t;
   }
   return NULL;
}

YAC_Object *YAC_List::_addFirstString(YAC_Object *_sval) {
   if(YAC_VALID(_sval))
   {
      YAC_ListNode *t = addFirstNew();
      YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
      s->yacCopy(_sval);
      t->initString(s, 1);
      return t;
   }
   return NULL;
}

YAC_Object *YAC_List::_addLastString(YAC_Object *_sval) {
   if(YAC_VALID(_sval))
   {
      YAC_ListNode *t = addLastNew();
      YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
      s->yacCopy(_sval);
      t->initString(s, 1);
      return t;
   }
   return NULL;
}

YAC_Object *YAC_List::_insert(YAC_Object *_v, YAC_Object *_ln) {
   if( (void*)_ln == (void*)tail )
   {
      return _addLast(_v);
   }
   YAC_ListNode *c = head;
   while(c)
   {
      if( (void*)c == (void*)_ln )
      {
         // ---- found node
         YAC_ListNode *r = NULL;

         if(YAC_VALID(_v))
         {
            YAC_ListNode *ol=0;
            if(YAC_BCHK(_v, YAC_CLID_LIST))
            {
               ol = ((YAC_List*)_v)->head;
            }
            if(YAC_BCHK(_v, YAC_CLID_LISTNODE))
            {
               ol = (YAC_ListNode*)_v;
            }
            if(ol)
            {
               YAC_ListNode *t = ol;
               YAC_ListNode *n;
               YAC_ListNode *c_orig = c;
               do
               {
                  n = c->appendEmpty();
                  if(n)
                  {
                     if(!r)
                     {
                        r = n;
                     }
                     n->copySafe(t);
                  }
                  c = c->next;
                  t = t->next;
               } while(t && n);
               if( (void*)c_orig == (void*)tail )
               {
                  tail = n;
               }
            }
            else
            {
               if(YAC_BCHK(_v, YAC_CLID_VALUE))
               {
                  r = c->appendEmpty();
                  if(r)
                  {
                     YAC_ValueObject *v = (YAC_ValueObject*)_v;
                     r->copySafe(v);
                     if(c == tail)
                     {
                        tail = r;
                     }
                  }
               }

            }
         } // if valid _v
         return r;

      } // if c==_ln
      c = c->next;
   }
   return NULL;
}

YAC_Object *YAC_List::getHead(void) {
   return head;
}

YAC_Object *YAC_List::getTail(void) {
   return tail;
}

void YAC_List::_reverse(void) {
   if(head)
   {
      if( (void*)head != (void*)tail )
      {
         YAC_ListNode *l = head;
         YAC_ListNode *n;
         YAC_ListNode *t;
         while(l)
         {
            n = l->next;
            t = l->prev;
            l->prev = l->next;
            l->next = t;
            l = n;
         }
         t = tail; tail = head; head = t;
      }
   }
}

YAC_Object *YAC_List::_findPointer(YAC_Object *_o) {
   YAC_ListNode *l = head;
   while(l)
   {
      if(l->type >= YAC_TYPE_OBJECT)
      {
         if(l->value.object_val == (YAC_Object*)(_o))
         {
            return l;
         }
      }
      l = l->next;
   }
   return NULL;
}

void YAC_List::_remove(YAC_Object *_l, YAC_Value *_r) {
   if(YAC_CHK(_l, YAC_CLID_LISTNODE))
   {
      YAC_ListNode *l = head;
      while(l)
      {
         if( (void*)l == (void*)_l )
         {
            if( (void*)l == (void*)head )
            {
               head = head->next;
               if(head)
               {
                  head->prev = NULL;
               }
               else
               {
                  tail = NULL;
               }
               l->next = NULL;
               _r->initObject(l, 1);
               return;
            }
            if( (void*)l == (void*)tail ) // tail but not head?
            {
               tail = tail->prev;
               tail->next = NULL;
               l->prev = NULL;
               _r->initObject(l, 1);
               return;
            }
            // ---- neither head nor tail
            l->prev->next = l->next;
            l->next->prev = l->prev;
            l->prev = NULL;
            l->next = NULL;
            _r->initObject(l, 1);
            return;
         }
         l = l->next;
      }
   }
   _r->initVoid();
}

sUI YAC_VCALL YAC_List::yacArrayGetNumElements(void) {
   return (NULL != head) ? head->yacArrayGetNumElements() : 0u;
}

sUI YAC_VCALL YAC_List::yacArrayGetMaxElements(void) {
   return yacArrayGetNumElements();
}
