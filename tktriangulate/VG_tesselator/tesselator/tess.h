#ifndef VGES_TESS_INTERNAL_H
#define VGES_TESS_INTERNAL_H
/**********************************************************************
 *     Source:            tess.h
 *    Author:              Nils Pipenbrinck
 *    Creation Date:       12.06.2006
 *    Project Name:        OpenVG 1.0 Implementation 
 *    Description:             
 *
 *
 *   Simple structures and interface to the low level SGI Tesselator 
 *   (Free OpenGL Sample implementation) 
 *
 *   Almost all capabilites have been removed, so it now just does
 *   what it's supposed to do.
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
#include <setjmp.h>
#include "mesh.h"
#include "newdict.h"
#include "priorityq.h"
#include "edgeflags.h"
#include "poolalloc.h"
#include "tessbinding.h"

struct Tesselator 
///////////////////
{
  // memory pools:
  VGES_Pool     VertexPool;
  VGES_Pool     FacePool;
  VGES_Pool     EdgePool;
  VGES_Pool     RegionPool;

  // buffer for the mesh object:
  VGubyte       m_MeshMem[sizeof (GLUmesh)];

  // Edge-Tree (for edge flag detection)
  VGES_EdgeTree m_EdgeTree;  

  // states:
  VGFillRule    windingRule;	  // rule for determining polygon interior
  Dict		      * dict;		      // edge dictionary for sweep line 
  PriorityQ 	  * pq;		        // priority queue of vertex events 
  GLUvertex	    * event;		    // current sweep event being processed 
  GLUhalfEdge	  * lastEdge;	    // lastEdge->Org is the most recent vertex 
  GLUmesh	      * mesh;		      // stores the input contours, and eventually the tessellation itself 
  jmp_buf        env;			      // place to jump to when VGES_mallocs fail 
};

#endif
