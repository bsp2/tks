/**********************************************************************
 *    c-Source:            edgeflags.c
 *    Author:              Nils Pipenbrinck
 *    Creation Date:       30.11.2006
 *    Project Name:        OpenVG 1.0 Implementation 
 *    Description:             
 *
 *  Implementation of an edge-flag generator. Takes an index buffer
 *  and generates edge flags.
 *
 *  Uses a splay-tree for edge id storage.. 
 *
 **********************************************************************
 *     History report
 *     
 *     Reason of changes:
 *
 *      - initial implementation
 *
 **********************************************************************/
#include "../common.h"
#include "edgeflags.h"

#define GROWSIZE 256  // grow by 256 elements each.

static EdgeTree_Entry * loc_TreeLocate (VGuint value, EdgeTree_Entry * t) 
/////////////////////////////////////////////////////////////////////////
// locate a node inside the splay tree (SPLAY-Operation).
{
  EdgeTree_Entry  N,	*l,	*r,	*y;
  if (!t) return	t;
  N.left = N.right = 0;
  l	=	r	=	&N;

  for	(;;) 
  {
    if (value < t->value) 
    {
      if (!t->left) break;
      if (value	<	t->left->value) 
      {
        //	rotate  
        y	=	t->left;													 
        t->left	=	y->right;
        y->right = t;
        t	=	y;
        if (!t->left) break;
      }
      // link 
      r->left	=	t;
      r	=	t;
      t	=	t->left;
    }	else if	(value > t->value)
    {
      if (!t->right)	break;
      if (value > t->right->value)
      {
        // rotate 
        y	=	t->right;													
        t->right = y->left;
        y->left	=	t;
        t	=	y;
        if (!t->right)	break;
      }
      // link
      l->right = t;
      l	=	t;
      t	=	t->right;
    }	else break;
  }
  l->right = t->left;											
  r->left	=	t->right;
  t->left	=	N.right;
  t->right = N.left;
  return t;
}

static VGint loc_Tree_Relocate (VGES_EdgeTree * tab)
///////////////////////////////////////////////
// relocates the symbol table (in case we run out of memory)
{
  VGint i;
  EdgeTree_Entry * entries;
  EdgeTree_Entry * old;

  assert (tab->m_FreeList == 0);

  // allocate memory for additional entries:
  entries = (EdgeTree_Entry*) vgtesselator_alloc ((tab->m_RepositorySize + GROWSIZE) * sizeof (EdgeTree_Entry));
  if (!entries) 
  {
    tab->m_AllocationError = 1;
    return 0;
  }

  // relocate the tree structure:
  old = tab->m_Repository;
  for (i=0; i<tab->m_AllocationTop; i++)
  {
    entries[i].value  = old[i].value;
    if (old[i].left)  entries[i].left   = old[i].left  - old + entries; else entries[i].left  = 0;
    if (old[i].right) entries[i].right  = old[i].right - old + entries; else entries[i].right = 0;
  }

  // relocate the root node:
  if (tab->m_SplayTree)
    tab->m_SplayTree = tab->m_SplayTree -  tab->m_Repository + entries;

  // Free old repository
  if (tab->m_Repository)
    vgtesselator_free (tab->m_Repository);

  // Set new repository:
  tab->m_Repository      = entries;
  tab->m_RepositorySize += GROWSIZE;
  return 1;
}


void VGES_Init_EdgeTree (VGES_EdgeTree * tab)
/////////////////////////////////////////
{
  tab->m_Repository      = 0;
  tab->m_RepositorySize  = 0;
  tab->m_FreeList        = 0;
  tab->m_AllocationTop   = 0;
  tab->m_SplayTree       = 0;
  tab->m_AllocationError = 0;
}


void VGES_Close_EdgeTree (VGES_EdgeTree * tab)
/////////////////////////////////////////////
// frees allocated resources.
{
  if (tab->m_Repository)
    vgtesselator_free (tab->m_Repository);
}


static void loc_Tree_ProcessEdge (VGES_EdgeTree * tab, VGuint e1, VGuint e2, VGuint e3)
//////////////////////////////////////////////////////////////////////////////////////
{
  VGuint edgecodes[3];
  VGint i;

  edgecodes[0] = e1<e2 ? (e1<<16)|(e2) : (e2<<16)|(e1);
  edgecodes[1] = e2<e3 ? (e2<<16)|(e3) : (e3<<16)|(e2);
  edgecodes[2] = e1<e3 ? (e1<<16)|(e3) : (e3<<16)|(e1);

  for (i=0; i<3; i++)
  {
    VGuint value = edgecodes[i];

    tab->m_SplayTree = loc_TreeLocate (value,tab->m_SplayTree);

    if ((tab->m_SplayTree) && (tab->m_SplayTree->value == value))
    {
      ////////////////////////////////////////////
      // This edge exists: Remove it from the tree
      ////////////////////////////////////////////
      EdgeTree_Entry * x = tab->m_SplayTree;

      if (!tab->m_SplayTree->left) 
      {
        x	=	tab->m_SplayTree->right;
      }	else {
        x	=	loc_TreeLocate(value, tab->m_SplayTree->left);
        x->right = tab->m_SplayTree->right;
      }
      // free the root splaytree-node. link it into the FreeList via the left pointer.
      tab->m_SplayTree->left = tab->m_FreeList;
      tab->m_FreeList        = tab->m_SplayTree;
      tab->m_SplayTree       = x;
    } 
    else 
    {
      EdgeTree_Entry * newnode;
      ///////////////////////////////////////////////
      // This edge does not exist: Add it to the tree
      ///////////////////////////////////////////////

      // allocate a node: Take if from the freelist if we have one entry,
      // otherwise take it from the repository heap:
      if (tab->m_FreeList)
      {
        // we have a node in the freelist:
        newnode = tab->m_FreeList;
        tab->m_FreeList = newnode->left;
      } else {
        // need to grow and relocate? 
        if (tab->m_AllocationTop >= tab->m_RepositorySize)
          if (!loc_Tree_Relocate(tab)) return;
        newnode = &tab->m_Repository[tab->m_AllocationTop++];
      }

      newnode->value  = value;

      if (!tab->m_SplayTree) 
      {
        newnode->left	   = 0;
        newnode->right   = 0;
        tab->m_SplayTree = newnode;
      } else 
      if (value < tab->m_SplayTree->value)
      {
        newnode->left	 = tab->m_SplayTree->left;
        newnode->right = tab->m_SplayTree;
        tab->m_SplayTree->left	=	0;
        tab->m_SplayTree = newnode;
      }	else if (value > tab->m_SplayTree->value)
      {
        newnode->right = tab->m_SplayTree->right;
        newnode->left	 =	tab->m_SplayTree;
        tab->m_SplayTree->right = 0;
        tab->m_SplayTree = newnode;
      }	
    }
  }
}



VGboolean VGES_Generate_Edgeflags (VGES_EdgeTree * tree, VGushort * a_ib, VGubyte * a_eb, VGint a_NumFaces)
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Build edge flags from a index buffer structure.
{
  VGint i;
  VGushort * ib = a_ib;
  EdgeTree_Entry * t;

  // Initialize the tree: 
  tree->m_AllocationError = 0;
  tree->m_AllocationTop   = 0;
  tree->m_FreeList        = 0;
  tree->m_SplayTree       = 0;

  // Put all Edges into the tree. If an edge does not exist, 
  // add it to the tree, otherwise remove it. this way all
  // outer edges remain:
  for (i=0; (i<a_NumFaces) && (!tree->m_AllocationError); i++, ib+=3)
    loc_Tree_ProcessEdge (tree, ib[0], ib[1], ib[2]);

  // Check allocation errors:
  if (tree->m_AllocationError)
    return VG_FALSE;

  // now lookup all edges again and generate edge-flags
  ib = a_ib;
  for (i=0; i<a_NumFaces; i++)
  {
    VGuint value;
    VGuint a = *(ib++);
    VGuint b = *(ib++);
    VGuint c = *(ib++);
    VGubyte edgeflag = 0;

    // traverses the tree and find the outer edges.
    // every 1 out of 8 operations will use the splay-tree
    // traversal which balances but is a bit slower.
    if ((i&7) == 7)
    {
      ////////////////////////////
      // traversal with balancing
      ////////////////////////////
      value = ((a < b) ? (a<<16)|(b) : (b<<16)|(a));
      tree->m_SplayTree = loc_TreeLocate (value,tree->m_SplayTree);
      if ((tree->m_SplayTree) && (tree->m_SplayTree->value == value)) edgeflag |= 1;

      value = ((b < c) ? (b<<16)|(c) : (c<<16)|(b));
      tree->m_SplayTree = loc_TreeLocate (value,tree->m_SplayTree);
      if ((tree->m_SplayTree) && (tree->m_SplayTree->value == value)) edgeflag |= 2;

      value = ((a < c) ? (a<<16)|(c) : (c<<16)|(a));
      tree->m_SplayTree = loc_TreeLocate (value,tree->m_SplayTree);
      if ((tree->m_SplayTree) && (tree->m_SplayTree->value == value)) edgeflag |= 4;
    } 
    else 
    {
      ////////////////////////////
      // traversal without balancing
      ////////////////////////////

      value = ((a < b) ? (a<<16)|(b) : (b<<16)|(a));
      t = tree->m_SplayTree;
      for	(;;) 
      {
        if (!t) break;
        if (value < t->value)      t = t->left;
        else if (value > t->value) t = t->right;
        else { edgeflag |= 1; break; }
      }
      value = ((b < c) ? (b<<16)|(c) : (c<<16)|(b));
      t = tree->m_SplayTree;
      for	(;;) 
      {
        if (!t) break;
        if (value < t->value)      t = t->left;
        else if (value > t->value) t = t->right;
        else { edgeflag |= 2; break; }
      }
      value = ((a < c) ? (a<<16)|(c) : (c<<16)|(a));
      t = tree->m_SplayTree;
      for	(;;) 
      {
        if (!t) break;
        if (value < t->value)      t = t->left;
        else if (value > t->value) t = t->right;
        else { edgeflag |= 4; break; }
      }
    }

    a_eb[i] = edgeflag;
  }

  return VG_TRUE;
}


