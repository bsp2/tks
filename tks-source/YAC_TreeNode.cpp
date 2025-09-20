/// YAC_TreeNode.cpp
///
/// (c) 2001-2015 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "YAC_TreeNode.h"
#include "YAC_HashTable.h"

#include "TKS_StringIterator.h"


YAC_TreeNode::YAC_TreeNode(void) {
   class_ID   = YAC_CLID_TREENODE;
   parent     = NULL;
   left       = NULL;
   right      = NULL;
   deleteme   = 0;
   value.any  = NULL;
}

YAC_TreeNode::~YAC_TreeNode() {
   YAC_TreeNode::_free();
}

void YAC_TreeNode::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   YAC_Object::yacOperator(_cmd, _o, _r);
}

YAC_Object *YAC_TreeNode::_insertLeft(YAC_Object *_templ) {
   YAC_TreeNode *pleft=left;
   left=(YAC_TreeNode*)YAC_NEW_CORE_POOLED(YAC_CLID_TREENODE);
   if(_templ)  
   { 
      left->initObject(YAC_CLONE_POOLED(NULL, _templ), 1); // xxx TKS_MT: use *real* thread context
      //left->value.object_val->yacOperatorAssign(_templ); // xxx yacOperatorAssign?
   }
   else 
   { 
      left->initObject(0, 0); 
   }
   left->parent=this;
   if(pleft)  
   { 
      pleft->parent=left; 
   }
   return left;
}

YAC_Object *YAC_TreeNode::_insertRight(YAC_Object *_templ) {
   YAC_TreeNode *pright=right;
   right=(YAC_TreeNode*)YAC_NEW_CORE_POOLED(YAC_CLID_TREENODE);
   if(_templ)  
   {
      right->initObject(YAC_CLONE_POOLED(NULL, _templ), 1); // xxx TKS_MT: use *real* thread context
      // xxx yacOperatorAssign
   }
   else 
   {
      right->initObject(0, 0); 
   }
   right->parent=this;
   if(pright) 
   {
      pright->parent=right; 
   }
   return right;
}

YAC_TreeNode*YAC_TreeNode::findByName2(YAC_String *_s, sSI _depth) {
   if(name.compare(_s)) 
   {
      return this; 
   }
   else
   {
      if(left)
      {
         YAC_TreeNode *r=left->findByName2(_s, _depth);
         if(r)
            return r;
      }
      if(_depth>0) 
      {
         if(right) 
         {
            return right->findByName2(_s, _depth-1); 
         } 
      }
      return 0;
   }
}

YAC_Object *YAC_TreeNode::_findByName(YAC_String *_s, sSI _depth) {
   return findByName2(_s, _depth);
}

YAC_TreeNode*YAC_TreeNode::findById2(YAC_String *_s, sSI _depth) {
   if(id.compare(_s)) 
   {
      return this; 
   }
   else
   {
      if(left)
      {
         YAC_TreeNode *r=left->findById2(_s, _depth);
         if(r)
            return r;
      }
      if(_depth>0) 
      {
         if(right) 
         {
            return right->findById2(_s, _depth-1); 
         } 
      }
      return 0;
   }
}

YAC_Object *YAC_TreeNode::_findById(YAC_String *_s, sSI _depth) {
   return findById2(_s, _depth);
}

void YAC_TreeNode::_free(void) {
   if(left)
   {
      YAC_DELETE(left); left=0;
   }
   if(right)
   {
      YAC_DELETE(right); right=0;
   }
   unset();
}

YAC_Object *YAC_TreeNode::getLeft(void) {
   return left;
}

YAC_String *YAC_TreeNode::getName(void) {
   return &name;
}

YAC_Object *YAC_TreeNode::getParent(void) {
   return parent;
}

YAC_Object *YAC_TreeNode::getRight(void) {
   return right;
}

YAC_Object *YAC_TreeNode::getRoot(void) {
   return seekRoot();
}

YAC_TreeNode *YAC_TreeNode::seekRoot(void) {
   YAC_TreeNode *c=this;
   while(c->parent) c=c->parent;
   return c;
}

YAC_Object *YAC_TreeNode::_seekByPathName(YAC_String *_s) {
   // myroot.myleafa.myleafb.myobject
   YAC_String s;
   s.yacCopy(_s);
   s.split('.');
   TKS_StringIterator si;
   si.begin(&s);
   YAC_TreeNode *c=this;
   if(si.tail())
   {
      do  
      {
         YAC_String *cl=	si.current;
         YAC_TreeNode *t=c;
         if(c->left)
         {
            if(c->left->name.compare(cl)) t=c->left;
         }
         if(c->right)
         {
            if(c->right->name.compare(cl)) t=c->right;
         }
         c=t;
      } while(si.previous());
   }
   si.end();
   s.freeStack();
   if(c==this)  
   {
      return 0; 
   }
   else  
   {
      return c; 
   }
}

void YAC_TreeNode::_setName(YAC_String *_s) {
   name.yacCopy(_s);
}

void YAC_TreeNode::_setLeft(YAC_Object *_tn) { // xxx
   if(YAC_VALID(_tn))
   {
      if(YAC_CHK(_tn, YAC_CLID_TREENODE))
      {
         if(left)  
         { 
            YAC_DELETE(left); 
         }
         left=(YAC_TreeNode*)_tn;
      }
   }
   else
   {
      // ---- unset left ----
      if(left)  
      { 
         YAC_DELETE(left); left=0;  
      }
   }
}

void YAC_TreeNode::_setRight(YAC_Object *_tn) { // XXX
   if(YAC_VALID(_tn))
   {
      if(YAC_CHK(_tn, YAC_CLID_TREENODE))
      {
         if(right)  
         { 
            YAC_DELETE(right); 
         }
         right=(YAC_TreeNode*)_tn;
      }
   }
   else
   {
      // ---- unset right ----
      if(right)  
      { 
         YAC_DELETE(right); right=0; 
      }
   }
}

void YAC_TreeNode::_writeToHashTable(YAC_Object *_ht) {
   if(YAC_CHK(_ht, YAC_CLID_HASHTABLE))
   {
      YAC_HashTable *ht=(YAC_HashTable*)_ht;
      sUI num_nodes=getNumNodes();
      sSI r;
      if(num_nodes*5<1024) 
      {
         r=ht->_alloc(num_nodes*5); 
      }
      else 
      {
         r=ht->_alloc(num_nodes); 
      }
      if(r)
      {
         YAC_String tname;
         tname.alloc(1024);
         addToHashTable(&tname, ht);
      }
   }
}

void YAC_TreeNode::addToHashTable(YAC_String *_name, YAC_Object *_hash) {
   YAC_String tname; tname.yacCopy(_name);
   if(!tname.isBlank())  
   { 
      tname.append("."); 
   }
   if(id.isBlank())  
   { 
      tname.append(&name);  
   } 
   else  
   { 
      tname.append(&id); 
   }
   YAC_Value val; val.initObject(this, 0);
   _hash->yacHashSet(NULL, &tname, &val); // xxx TKS_MT: should use real thread context (exceptions)
   if(right)  
   {  
      YAC_String ts;  
      ts.yacCopy(&tname);  
      right->addToHashTable(&ts, _hash);  
   }
   if(left)  
   {  
      YAC_String ts;  
      ts.yacCopy(_name);  
      left->addToHashTable(&ts, _hash);  
   }
}

sUI YAC_TreeNode::getNumNodes(void) {
   sUI r = 1;
   if(left)
   {
      r = r + left->getNumNodes();
   }
   if(right)
   {
      r = r + right->getNumNodes();
   }
   return r;
}

YAC_String *YAC_TreeNode::getId(void) {
   return &id;
}

void YAC_TreeNode::_setId(YAC_String *_s) {
   id.yacCopy(_s);
}
