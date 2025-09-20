/// YAC_TreeNode.h
///
/// (c) 2001-2015 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __YAC_TREENODE_H__
#define __YAC_TREENODE_H__


class YAC_TreeNode : public YAC_ValueObject {
public:
   YAC_TreeNode *left;
   YAC_TreeNode *right;
   YAC_TreeNode *parent;
   YAC_String    name;
   YAC_String    id;
   
public:
   YAC_TreeNode  (void);
   ~YAC_TreeNode ();
   
   void          YAC_VCALL yacOperator     (sSI, YAC_Object *, YAC_Value *);
   sBool         YAC_VCALL yacIsComposite  (void) {return 1;}

   YAC_TreeNode *seekRoot       (void);
   sUI           getNumNodes    (void);
   YAC_TreeNode *findByName2    (YAC_String *_s, sSI _depth);
   YAC_TreeNode *findById2      (YAC_String *_s, sSI _depth);
   void          addToHashTable (YAC_String *_name, YAC_Object *_hash);
  
   YAC_Object *_insertLeft       (YAC_Object *_templ);
   YAC_Object *_insertRight      (YAC_Object *_templ);
   YAC_Object *_findByName       (YAC_String *_s, sSI _depth);
   YAC_Object *_findById         (YAC_String *_s, sSI _depth);
   void        _free             (void);
   YAC_Object *getLeft           (void);
   YAC_String *getName           (void);
   YAC_Object *getParent         (void);
   YAC_Object *getRight          (void);
   YAC_Object *getRoot           (void);
   YAC_Object *_seekByPathName   (YAC_String *_s);
   void        _setName          (YAC_String *_s);
   void        _setLeft          (YAC_Object *_o);
   void        _setRight         (YAC_Object *_o);
   void        _writeToHashTable (YAC_Object *_o);
   YAC_String *getId             (void);
   void        _setId            (YAC_String *_s);
};


#endif // __YAC_TREENODE_H__
