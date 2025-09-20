#ifndef VGES_TESS_BINDING_H
#define VGES_TESS_BINDING_H
/**********************************************************************
 *    C Source:            tess_binding.h
 *    Author:              Nils Pipenbrinck
 *    Creation Date:       12.06.2006
 *    Project Name:        OpenVG 1.0 Implementation
 *    Description:
 *
 *    Api Wrapper for the Tesselator. Converts multiple elements into a
 *    single indexed one.
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

typedef void * VGTesselator;
// opaque handle

typedef struct
//////////////
// 2D Vector structure
{
  VGfloat x,y;
} VGES_Vector2;


typedef struct tagElement
/////////////////////////
// Structure to store outlines and meshes
{
  VGint                  m_NumVertices;           // number of vertices
  VGshort              * m_IndexBuffer;           // ptr to index-list (might be 0 for fans)
  VGubyte              * m_EdgeBuffer;
  VGuint                 m_IndexBufferSize;       // number of entries in index-list (must be multiple of 3, less than 32768)
  struct tagElement    * m_Next;                  // ptr to next element in chain
  VGES_Vector2           m_Vertices[1];           // array of vertices.
} Element;


typedef enum
/////////////
// fillrule definition
{
  VG_EVEN_ODD = 0x1900,
  VG_NON_ZERO = 0x1901
} VGFillRule;


VGTesselator VGES_NewTesselator ( void );
// create a tesselator instance.

void VGES_DeleteTesselator (VGTesselator tess);
// delete a tesselator instance.

Element * VGES_TesselateElements ( VGTesselator tess, Element * e, VGFillRule WindingRule, VGboolean CreateEdgeFlags );
// tesselate a list of elements accoding to a windingrule.

#endif
