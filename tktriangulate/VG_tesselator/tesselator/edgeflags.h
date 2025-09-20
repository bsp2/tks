#ifndef __VGES_EDGEFLAGS_H
#define __VGES_EDGEFLAGS_H
/**********************************************************************
 *    h-Source:            edgeflags.h
 *    Author:              Nils Pipenbrinck
 *    Creation Date:       30.11.2006
 *    Project Name:        OpenVG 1.0 Implementation 
 *    Description:             
 *
 *  Implementation of an edge-flag generator. Takes an index buffer
 *  and generates edge flags.
 *
 **********************************************************************
 *     History report
 *     
 *     Reason of changes:
 *
 *      - initial implementation
 *
 **********************************************************************/

typedef struct et_entry
///////////////////////
{
  VGuint             value;    // associated flags
  struct et_entry  * left;     // binary tree left node
  struct et_entry  * right;    // binary tree right node
} EdgeTree_Entry;

typedef struct 
//////////////
{
  EdgeTree_Entry  * m_Repository;      
  VGint             m_RepositorySize;
  VGint             m_AllocationTop;
  VGint             m_AllocationError;
  EdgeTree_Entry  * m_FreeList;
  EdgeTree_Entry  * m_SplayTree;
} VGES_EdgeTree;


void VGES_Init_EdgeTree  (VGES_EdgeTree * tab);

void VGES_Close_EdgeTree (VGES_EdgeTree * tab);

VGboolean VGES_Generate_Edgeflags (VGES_EdgeTree * tree, VGushort * a_ib, VGubyte * a_eb, VGint a_NumFaces);



#endif
