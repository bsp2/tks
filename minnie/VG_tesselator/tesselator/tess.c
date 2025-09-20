/**********************************************************************
 *    C Source:            tess.c
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
#include <setjmp.h>
#include <stdio.h>
#include "tess.h"
#include "mesh.h"
#include "sweep.h"
#include "tessmono.h"
#include "tessbinding.h"

// macro to find out if a float is near some small value
#define VGES_NEARZERO(A) (((A)> -FLT_EPSILON) && ((A)< FLT_EPSILON))


VGTesselator VGES_NewTesselator ( void )
/////////////////////////////////////////
{
  Tesselator *tess;
  tess = (Tesselator *)vgtesselator_alloc(sizeof(Tesselator));
  if (tess == NULL)  return 0;	
  memset (tess, 0, sizeof (Tesselator));
  tess->mesh          = 0;
  VGES_Pool_Init (&tess->VertexPool, sizeof (GLUvertex),    512);
  VGES_Pool_Init (&tess->FacePool,   sizeof (GLUface),      512);
  VGES_Pool_Init (&tess->EdgePool,   sizeof (EdgePair),     1024);
  VGES_Pool_Init (&tess->RegionPool, sizeof (ActiveRegion), 256);
  VGES_Init_EdgeTree  (&tess->m_EdgeTree);
 
  return (VGTesselator) tess;
}


void VGES_DeleteTesselator ( VGTesselator  a_tess)
//////////////////////////////////////////////////
{
  Tesselator * tess = (Tesselator *) a_tess;
  VGES_Pool_Done (&tess->VertexPool);
  VGES_Pool_Done (&tess->FacePool);
  VGES_Pool_Done (&tess->EdgePool);
  VGES_Pool_Done (&tess->RegionPool);
  if (tess->dict)
    dictDeleteDict(tess->dict);

  VGES_Close_EdgeTree (&tess->m_EdgeTree);
  vgtesselator_free( tess);
}


static int AddVertex( Tesselator *tess, VGfloat coords[2], void *data )
////////////////////////////////////////////////////////////////////////
{
  GLUhalfEdge *e;

  e = tess->lastEdge;
  if( e == NULL ) 
  {
    /* Make a self-loop (one vertex, one edge). */

    e = __gl_meshMakeEdge( tess->mesh );
    if (e == NULL) return 0;
    if ( !__gl_meshSplice( e, e->Sym, tess->mesh ) ) return 0;
  } else {
    /* Create a new vertex and edge which immediately follow e
     * in the ordering around the left face.
     */
    if (__gl_meshSplitEdge( e , tess->mesh) == NULL) return 0;
    e = e->Lnext;
  }

  /* The new vertex is now e->Org. */
  e->Org->coords[0] = coords[1];
  e->Org->coords[1] = coords[0];
  e->Org->s         = coords[1];
  e->Org->t         = coords[0];
  
  /* The winding of an edge says how the winding number changes as we
   * cross from the edge''s right face to its left face.  We add the
   * vertices in such an order that a CCW contour will add +1 to
   * the winding number of the region inside the contour.
   */
  e->winding      = 1;
  e->Sym->winding = -1;
  tess->lastEdge  = e;
  return 1;
}


static void loc_SetElements ( VGTesselator a_tess, Element * e)
////////////////////////////////////////////////////////////
{
  Element * run;
  VGint i;

  Tesselator * tess = (Tesselator *) a_tess;

  VGES_POOL_CLEAR (tess->FacePool);
  VGES_POOL_CLEAR (tess->VertexPool);
  VGES_POOL_CLEAR (tess->EdgePool);

  tess->mesh = (GLUmesh *) &tess->m_MeshMem;

  __gl_meshNewMesh(&tess->m_MeshMem, sizeof (tess->m_MeshMem));

  // set pool buffer:
  tess->mesh->VertexPool = &tess->VertexPool;
  tess->mesh->FacePool   = &tess->FacePool;
  tess->mesh->EdgePool   = &tess->EdgePool;

  for (run = e; run; run = run->m_Next)
  {
    tess->lastEdge = 0;

    for (i=0; i<run->m_NumVertices; i++)
    {
      if (!AddVertex( tess, &run->m_Vertices[i].x, 0)) 
        longjmp(tess->env,1); 
    }
  }
}


static Element * loc_Generate_SingleElement (Tesselator *tess, VGint FaceCount, VGint VertexCount, VGboolean CreateEdgeFlags)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Turns the processed tesselator structure into a single VGES Element.
{
  Element * out = 0;
  GLUface *f;

  VGint WriteCount = 0;
  VGint size1;
  VGint size2;
  VGint size3;

  // calculate size for element, vertices and index buffer:
  size1 = sizeof (Element) + ((VertexCount-1) * sizeof (VGES_Vector2));
  size2 = FaceCount * 3 * sizeof (VGushort);

  if (CreateEdgeFlags)
    size3 = FaceCount *  sizeof (VGubyte);
  else 
    size3 = 0;

  // allocate all parts in one chunk:
  out = (Element *) vgtesselator_alloc (size1 + size2 + size3);
  if (!out) return 0;

  // set index buffer and note that we must not delete it:
  out->m_IndexBuffer     = (VGshort*) (((VGubyte *) out) + size1);
  out->m_EdgeBuffer      = (VGubyte*) (((VGubyte *) out) + size1 + size2);
  out->m_NumVertices     = VertexCount;
  out->m_IndexBufferSize = FaceCount * 3;
  out->m_Next            = 0;

  if (!CreateEdgeFlags)
    out->m_EdgeBuffer = 0;

  // Write the faces (index list)
  for( f = tess->mesh->fHead.next; f != &tess->mesh->fHead; f = f->next ) 
  {
    if (f->inside) 
    {
      GLUhalfEdge *e = f->anEdge;
      do {
        out->m_IndexBuffer[WriteCount++] = (VGshort) e->Org->pqHandle;
        e = e->Lnext;
      } while( e != f->anEdge );
    }
  }

  // Write the vertices:
  for( f = tess->mesh->fHead.next; f != &tess->mesh->fHead; f = f->next ) 
  {
    if (f->inside) 
    {
      GLUhalfEdge *e = f->anEdge;
      do 
      {
        VGint VertexId = e->Org->pqHandle;
        if (VertexId != -1)
        {
          out->m_Vertices[VertexId].x = (VGfloat)e->Org->t;
          out->m_Vertices[VertexId].y = (VGfloat)e->Org->s;
          e->Org->pqHandle = -1;
        }
        e = e->Lnext;
      } while( e != f->anEdge );
    }
  }

  if (CreateEdgeFlags)
  {
    if (!VGES_Generate_Edgeflags (&tess->m_EdgeTree, (VGushort*)out->m_IndexBuffer, out->m_EdgeBuffer, out->m_IndexBufferSize/3))
    {
      // Handle out of memory error:
      vgtesselator_free (out);
      return 0;
    }
  }

  return out;
}



static Element * loc_Generate_MultipleElements (Tesselator *tess, VGint FaceCount, VGint VertexCount)
/////////////////////////////////////////////////////////////////////////////////////////////////
// Turns the processed tesselator structure into a multiple VGES Elements.
// this version does not try to build an optimized index buffer, it just stores the faces as they appear
{
  Element * out   = 0;
  Element * chain = 0;
  GLUface *f;
  VGint id = 0;

  // Write the faces (index list)
  for( f = tess->mesh->fHead.next; f != &tess->mesh->fHead; f = f->next ) 
  {
    GLUhalfEdge *e = f->anEdge;

    if (f->inside) 
    {
      if (out && (out->m_IndexBufferSize == id))
      {
        chain = out;
        out = 0;
      }
      if (!out)
      {
        // Generate a new element */
        VGint size1;
        VGint size2;
        VGint  FaceCnt = FaceCount;
        if (FaceCnt > (32768/3)) FaceCnt = (32768/3);
        FaceCount -= FaceCnt;

        // calculate size for element, vertices and index buffer:
        size1 = sizeof (Element) + ((FaceCnt*3-1) * sizeof (VGES_Vector2));
        size2 = FaceCnt * 3 * sizeof (VGushort);

        // allocate all parts in one chunk:
        out = (Element *) vgtesselator_alloc (size1 + size2);
        if (!out) return 0;

        // set index buffer and note that we must not delete it:
        out->m_IndexBuffer     = (VGshort*) (((VGubyte *) out) + size1);
        out->m_NumVertices     = FaceCnt * 3;
        out->m_IndexBufferSize = FaceCnt * 3;
        out->m_Next            = chain;
        out->m_EdgeBuffer      = 0;
        id = 0;
      }

      // Add a face:
      do {
        out->m_IndexBuffer[id] = id;
        out->m_Vertices[id].x = (VGfloat)e->Org->t;
        out->m_Vertices[id].y = (VGfloat)e->Org->s;
        id++;
        e = e->Lnext;
      } while( e != f->anEdge );
    }
  }

  if (out)
  {
    out->m_Next = chain;
    chain = out;
  }

  return chain;
}

static Element * loc_ConvexTesselator (VGES_EdgeTree * tree, Element * src, VGboolean CreateEdgeFlags)
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Try to tesseleate the single element using a recursive subdivision method. Non-Convexity is detected 
// by checking the signs of the generated triangles. If anything goes wrong the process is stopped and 
// the tesselation is handled by "the big tesselator".
//
// This algorithm can handles all concave and some convex geometries  as well. 
// The topology is much better than using simple fans/strips.
{
  VGint left[32];     // enough for 2^32 vertices
  VGint right[32];    // enough for 2^32 vertices 
  VGint SignFlag = 0;
  VGint stacktop;

  left[0]  = 0;
  right[0] = src->m_NumVertices-1;
  stacktop = 1;
    
  while (stacktop)
  {
    VGint l,r,m;
    stacktop--;
    l = left[stacktop];
    r = right[stacktop];
    m = (l+r)>>1;

    if ((m != l) && (m != r))
    {
      VGfloat Area;
      const VGES_Vector2 * a = &src->m_Vertices[l];
      const VGES_Vector2 * b = &src->m_Vertices[m];
      const VGES_Vector2 * c = &src->m_Vertices[r];

      Area = (a->x - c->x) * (b->y - c->y) - 
             (a->y - c->y) * (b->x - c->x);

      // do not touch degeneracies
      if (!VGES_NEARZERO (Area))
      {
        // if we don't yet have a valid sign-flag:
        if (SignFlag == 0) 
        {
          // Get area-sign of first triangle.
          SignFlag = (Area < 0) ? -1 : 1;
        } else {
          // Check triangle against first Area-Sign.
          // return if we have a sign-flip.
          if (SignFlag != ((Area < 0) ? -1 : 1)) 
            return 0;
        }
      }

      // recursive subdivide:
      if (m-l > 1) { left[stacktop] = l; right[stacktop] = m; stacktop++; } 
      if (r-m > 1) { left[stacktop] = m; right[stacktop] = r; stacktop++; }
    } 
  }

  // everything went well?
  if (SignFlag)
  {
    Element * out;
    VGint size1;
    VGint size2;
    VGint size3;
    VGint n;
    // VGint e;
    VGint FaceCnt = src->m_NumVertices-2;

    // calculate size for element, vertices and index buffer:
    size1 = sizeof (Element) + ((FaceCnt*3-1) * sizeof (VGES_Vector2));
    size2 = FaceCnt * 3 * sizeof (VGushort);

    if (CreateEdgeFlags)
      size3 = FaceCnt * sizeof (VGubyte);
    else 
      size3 =0;

    // allocate all parts in one chunk:
    out = (Element *) vgtesselator_alloc (size1 + size2 + size3);
    if (!out) return 0;

    // set index buffer and note that we must not delete it:
    out->m_IndexBuffer     = (VGshort*) (((VGubyte *) out) + size1);
    out->m_EdgeBuffer      = (VGubyte*) (((VGubyte *) out) + size1 + size2);
    out->m_NumVertices     = FaceCnt * 3;
    out->m_IndexBufferSize = FaceCnt * 3;
    out->m_Next            = 0;
    if (!CreateEdgeFlags) 
      out->m_EdgeBuffer = 0;

    // copy vertex data:
    memcpy (out->m_Vertices, src->m_Vertices, src->m_NumVertices * sizeof (VGES_Vector2));

    left[0]  = 0;
    right[0] = src->m_NumVertices-1;
    stacktop = 1;
    n = 0;
    // e = 0;
      
    // Tesselate once more, this time write the index and edge data:
    while (stacktop)
    {
      int l,r,m;
      stacktop--;
      l = left[stacktop];
      r = right[stacktop];
      m = (l+r)>>1;

      if ((m != l) && (m != r))
      {
        out->m_IndexBuffer[n++] = (VGushort)l;
        out->m_IndexBuffer[n++] = (VGushort)m;
        out->m_IndexBuffer[n++] = (VGushort)r;
        if (m-l > 1) { left[stacktop] = l; right[stacktop] = m; stacktop++; } 
        if (r-m > 1) { left[stacktop] = m; right[stacktop] = r; stacktop++;  }
      } 
    }

    if (CreateEdgeFlags)
    {
      if (!VGES_Generate_Edgeflags (tree, (VGushort*)out->m_IndexBuffer, out->m_EdgeBuffer, out->m_IndexBufferSize/3))
      {
        // handle out of memory error:
        vgtesselator_free (out);
        return 0;
      }
    }
    return out;
  }
  return 0;
}

Element * VGES_TesselateElements (VGTesselator a_tess, Element * src, VGFillRule WindingRule, VGboolean CreateEdgeFlags)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  Element    * out  = 0;
  Tesselator * tess = (Tesselator *) a_tess;
  VGint FaceCount   = 0;
  VGint VertexCount = 0;
  GLUface *f;

  // printf("xxx VGES try convex\n");

#if 1
  if ((src->m_Next == 0) && (src->m_NumVertices < 32768))
  {
    // Give the convex tesselator a try.
    out = loc_ConvexTesselator(&tess->m_EdgeTree, src, CreateEdgeFlags);
    if (out) 
      return out;
  }
#endif

  // printf("xxx VGES big WindingRule=0x%08x\n", WindingRule);

  loc_SetElements (a_tess, src);
  tess->windingRule = WindingRule;

  // come back here if out of memory.
  if (setjmp(tess->env) != 0) return 0;

  // split the contours into interiours (regions)
  if (!__gl_computeInterior(tess))
    return 0;

  // split all regions (based on the winding) into monotone sections.
  if (!__gl_meshTessellateInterior( tess->mesh ))
    return 0;

  // Walk the Face-List, count all active faces and "mark out"
  // the handle (used to find distinct vertices later)
  for( f = tess->mesh->fHead.next; f != &tess->mesh->fHead; f = f->next ) 
  {
    if (f->inside) 
    {
      GLUhalfEdge *e = f->anEdge;
      FaceCount++;
      e->Org->pqHandle               = -1;
      e->Lnext->Org->pqHandle        = -1;
      e->Lnext->Lnext->Org->pqHandle = -1;
    }
  }

  // Walk the Face-List, assign unique vertex id's.
  for( f = tess->mesh->fHead.next; f != &tess->mesh->fHead; f = f->next ) 
  {
    GLUhalfEdge *e = f->anEdge;
    if (f->inside) 
    do {
      if (e->Org->pqHandle == -1)
        e->Org->pqHandle = VertexCount++;
      e = e->Lnext;
    } while( e != f->anEdge );
  }

  if (VertexCount < 32768)
     return loc_Generate_SingleElement (tess, FaceCount, VertexCount, CreateEdgeFlags);
  else
    return loc_Generate_MultipleElements (tess, FaceCount, VertexCount);
}


