/// EntityTree.h
///
/// (c) 2005-2013 by Bastian Spiegel <bs@tkscript.de>
///     - published under the terms of the GNU general public license (GPL).
///
///

#ifndef TKS_ENTITYTREE_H__
#define TKS_ENTITYTREE_H__


#define EntityTree_MAX_LENGTH    16

#ifdef TKS_EXPORT_ENTITY_SCANNER

#define EntityTree_ASCII_HI     128
#define EntityTree_ASCII_MASK   127

class EntityTreeNode
// ---- a single treenode in the entity name tree. stores a character<>id
// ---- and a character<>treenode mapping.
{
  public:
   sUI            *entries;  // stores entity IDs (1..MAX_ENTITIES) or 0  (=unused)
   EntityTreeNode *nodes   [EntityTree_ASCII_HI]; // if a char is not unique, this leads to a subtable for the next char

  public:
    EntityTreeNode ();
   ~EntityTreeNode ();
};


class EntityTree
// ---- pre-sorts entity by length of name, then looks them up in subtree (nodes)
{
private:
   EntityTreeNode *nodes[EntityTree_MAX_LENGTH];

   void  cleanup             (void);

public:
    EntityTree ();
   ~EntityTree ();

   sBool addEntity           (const char *_name, sUI _id);
   sUI   getEntityIDByString (YAC_String *_s);
   void  exportToC           (const char *_filename, const char *_funname);
};


#endif // TKS_EXPORT_ENTITY_SCANNER

#endif // TKS_ENTITYTREE_H__
