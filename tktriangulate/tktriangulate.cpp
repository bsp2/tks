/// tktriangulate.cpp
///
/// (c) 2018-2024 by Bastian Spiegel <bs@tkscript.de>
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
///

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <list>
#include <limits>
using namespace std;

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
YAC_Host *yac_host;

#include "tktriangulate_config.h"

#ifdef HAVE_POLYPARTITION
#include "polypartition-master/src/polypartition.h"
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
extern "C" {
#include "VG_tesselator/tesselator/tessbinding.h"
}
#endif // HAVE_VGTESSELATE

#include "tktriangulate.h"

#include "ying_tktriangulate.h"
#include "ying_tktriangulate_Triangulate.cpp"
#include "ying_tktriangulate.cpp"

// Implement standard exception ID variables (see yac.h)
Dyac_std_exid_impl;


// ---------------------------------------------------------------------------- Triangulate
_Triangulate::_Triangulate(void) {
#ifdef HAVE_POLYPARTITION
   engine_type = TRIANGULATE_POLYPARTITION_TRIANGULATE_EC;
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
   engine_type = TRIANGULATE_VGTESSELATE_DEFAULT;
   vgtesselate.input = NULL;
   // // vgtesselate.input_verts = YAC_New_FloatArray();
   vgtesselate.output = NULL;
#endif // HAVE_VGTESSELATE
}

_Triangulate::~_Triangulate() {
   free();
}

void _Triangulate::free(void) {
   freeInput();

#ifdef HAVE_POLYPARTITION
   freeResult();
#endif // HAVE_POLYPARTITION
}

void _Triangulate::freeInput(void) {
#ifdef HAVE_POLYPARTITION
   polypartition.input.clear();
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
   Element *e = vgtesselate.input;
   while(NULL != e)
   {
      Element *eNext = e->m_Next;
      ::free((void*)e);
      e = eNext;
   }
   vgtesselate.input = NULL;
#endif // HAVE_VGTESSELATE
}

void _Triangulate::freeResult(void) {
#ifdef HAVE_POLYPARTITION
   polypartition.result.clear();
   polypartition.result_poly.clear();
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
   if(NULL != vgtesselate.output)
   {
      // yac_host->printf("xxx free vgtesselate.output=%p\n", vgtesselate.output);
      ::free(vgtesselate.output);
      // yac_host->printf("xxx free vgtesselate.output DONE\n");
      vgtesselate.output = NULL;
   }
#endif // HAVE_VGTESSELATE
}

void _Triangulate::setEngine(sSI _engineType) {
   engine_type = _engineType;
}

void _Triangulate::addPolygonRegionEx(YAC_Object *_fa, sUI _startOff, sUI _numElements, sBool _bHole) {
   if(YAC_VALID(_fa))
   {
      sUI availNumElements = _fa->yacArrayGetNumElements();

      if(0u == _numElements)
         _numElements = availNumElements;

      if((_startOff + _numElements) <= availNumElements)
      {
         sUI numVertices = _numElements / 2u;
         // yac_host->printf("xxx Triangulate::addPolygonRegion: startOff=%u numElements=%u => numVertices=%u\n", _startOff, _numElements, numVertices);

         sBool bPP = usePolyPartition();
         sBool bVG = useVGTesselate();

#ifdef HAVE_POLYPARTITION
         TPPLPoly poly;
         if(bPP)
         {
            poly.Init(numVertices);
            poly.SetHole(YAC_FALSE != _bHole);
         }
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
         VGES_Vector2 *vgInputVerts;
         if(bVG)
         {
            Element *e = (Element *) ::malloc( sizeof(Element) + (numVertices - 1u) * sizeof(VGES_Vector2) );
            memset((void*)e, 0, sizeof(Element));
            e->m_NumVertices     = numVertices;
            e->m_IndexBufferSize = 0u;
            e->m_IndexBuffer     = NULL;
      
            if(NULL == vgtesselate.input)
            {
               vgtesselate.input = e;
            }
            else
            {
               Element *eLast = vgtesselate.input;
               while(NULL != eLast->m_Next)
                  eLast = eLast->m_Next;
               eLast->m_Next = e;
            }

            vgInputVerts = e->m_Vertices;
         }
#endif // HAVE_VGTESSELATE

         sUI idx = 0;
         sUI j = _startOff;

         for(idx = 0u; idx < numVertices; idx++)
         {
            YAC_Value vx;
            YAC_Value vy;

            _fa->yacArrayGet(NULL/*context*/, j++, &vx);
            vx.typecast(YAC_TYPE_FLOAT);

            _fa->yacArrayGet(NULL/*context*/, j++, &vy);
            vy.typecast(YAC_TYPE_FLOAT);

#ifdef HAVE_POLYPARTITION
            if(bPP)
            {
               poly[idx].x = vx.value.float_val;
               poly[idx].y = vy.value.float_val;
            }
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
            if(bVG)
            {
               // printf("xxx add vg vertex[%u] (%f; %f)\n", idx, vx.value.float_val, vy.value.float_val);
               vgInputVerts[idx].x = vx.value.float_val;
               vgInputVerts[idx].y = vy.value.float_val;
            }
#endif // HAVE_VGTESSELATE
         }

#ifdef HAVE_POLYPARTITION
         polypartition.input.push_back(poly);
#endif // HAVE_POLYPARTITION
      }
      else
      {
         yac_host->printf("[---] Triangulate::addPolygon: invalid region (startOff+numElements > array size) (%u + %u > %u)\n", _startOff, _numElements, availNumElements);
      }
   }
   else
   {
      freeInput();
   }
}

void _Triangulate::addPolygonRegion(YAC_Object *_fa, sUI _startOff, sUI _numElements) {
   return addPolygonRegionEx(_fa, _startOff, _numElements, YAC_FALSE/*bHole*/);
}

void _Triangulate::addPolygonRegionHole(YAC_Object *_fa, sUI _startOff, sUI _numElements) {
   return addPolygonRegionEx(_fa, _startOff, _numElements, YAC_TRUE/*bHole*/);
}

void _Triangulate::addPolygonEx(YAC_Object *_fa, sBool _bHole) {
   addPolygonRegionEx(_fa, 0u, 0u, _bHole);
}

void _Triangulate::addPolygon(YAC_Object *_fa) {
   addPolygonRegionEx(_fa, 0u, 0u, YAC_FALSE/*bHole*/);
}

void _Triangulate::addPolygonHole(YAC_Object *_fa) {
   addPolygonRegionEx(_fa, 0u, 0u, YAC_TRUE/*bHole*/);
}

#ifdef HAVE_POLYPARTITION

void _Triangulate::polypartition_triangulate_EC(void) {
   // Triangulation by ear clipping
   //  - Time/Space complexity: O(n^2)/O(n)
   //  - Supports holes: Yes, by calling TPPLPartition::RemoveHoles
   //  - Quality of solution: Satisfactory in most cases
   polypartition.pp.Triangulate_EC(&polypartition.input, &polypartition.result);
}

void _Triangulate::polypartition_triangulate_OPT(void) {
   // Optimal triangulation in terms of edge length using dynamic programming algorithm
   //  - Method: TPPLPartition::Triangulate_OPT
   //  - Time/Space complexity: O(n^3)/O(n^2)
   //  - Supports holes: No. You could call TPPLPartition::RemoveHoles prior to calling TPPLPartition::Triangulate_OPT,
   //     but the solution would no longer be optimal, thus defeating the purpose
   //  - Quality of solution: Optimal in terms of minimal edge length
   polypartition.pp.Triangulate_OPT(&(*polypartition.input.begin()), &polypartition.result);
}

void _Triangulate::polypartition_triangulate_MONO(void) {
   // Triangulation by partition into monotone polygons
   //  - Method: TPPLPartition::Triangulate_MONO
   //  - Time/Space complexity: O(n*log(n))/O(n)
   //  - Supports holes: Yes, by design
   //  - Quality of solution: Poor. Many thin triangles are created in most cases
   polypartition.pp.Triangulate_MONO(&polypartition.input, &polypartition.result);
}

void _Triangulate::polypartition_convexPartition_HM(void) {
   // Convex partition using Hertel-Mehlhorn algorithm
   //  - Method: TPPLPartition::ConvexPartition_HM
   //  - Time/Space complexity: O(n^2)/O(n)
   //  - Supports holes: Yes, by calling TPPLPartition::RemoveHoles
   //  - Quality of solution: At most four times the minimum number of convex polygons is created.
   //     However, in practice it works much better than that and often gives optimal partition.
   polypartition.result_poly.clear();
   polypartition.pp.ConvexPartition_HM(&polypartition.input, &polypartition.result_poly);
   // yac_host->printf("xxx Triangulate::convexPartition_HM: result_poly has %d polys\n", polypartition.result_poly.size());
   polypartition.pp.Triangulate_OPT(&(*polypartition.result_poly.begin()), &polypartition.result);
}

void _Triangulate::polypartition_convexPartition_OPT(void) {
   // - Optimal convex partition using dynamic programming algorithm by Keil and Snoeyink
   // - Method: TPPLPartition::ConvexPartition_OPT
   // - Time/Space complexity: O(n^3)/O(n^3)
   // - Supports holes: No. You could call TPPLPartition::RemoveHoles prior to calling TPPLPartition::Triangulate_OPT,
   //    but the solution would no longer be optimal, thus defeating the purpose
   // - Quality of solution: Optimal. A minimum number of convex polygons is produced};
   polypartition.result_poly.clear();
   polypartition.pp.ConvexPartition_OPT(&(*polypartition.input.begin()), &polypartition.result_poly);
   // yac_host->printf("xxx Triangulate::convexPartition_OPT: result_poly has %d polys\n", polypartition.result_poly.size());
   polypartition.pp.Triangulate_OPT(&(*polypartition.result_poly.begin()), &polypartition.result);
}

#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
void _Triangulate::vgtesselate_default(void) {

   if(NULL != vgtesselate.input)
   {
      VGTesselator tesselator;

      freeResult();

      // start tesselation
      tesselator = VGES_NewTesselator();
      vgtesselate.output = VGES_TesselatateElements(tesselator, vgtesselate.input, VG_EVEN_ODD, VG_FALSE);
      // yac_host->printf("xxx vgtesselate.output=%p\n", vgtesselate.output);
      // // if(NULL != vgtesselate.output)
      {
         // yac_host->printf("xxx vgtesselate.output: NumVertices=%u\n", vgtesselate.output->m_NumVertices);
         VGES_DeleteTesselator(tesselator);
      }
   }

}
#endif // HAVE_VGTESSELATE

sBool _Triangulate::usePolyPartition(void) {
   return (engine_type < TRIANGULATE_VGTESSELATE_DEFAULT);
}

sBool _Triangulate::useVGTesselate(void) {
   return (engine_type >= TRIANGULATE_VGTESSELATE_DEFAULT);
}

void _Triangulate::triangulate(void) {
   switch(engine_type)
   {
      default:
         yac_host->printf("[---] Triangulate::triangulate: unsupported/unavailable engine type %d\n", engine_type);
         break;

#ifdef HAVE_POLYPARTITION
      case TRIANGULATE_POLYPARTITION_TRIANGULATE_EC:
         polypartition_triangulate_EC();
         break;

      case TRIANGULATE_POLYPARTITION_TRIANGULATE_OPT:
         polypartition_triangulate_OPT();
         break;

      case TRIANGULATE_POLYPARTITION_TRIANGULATE_MONO:
         polypartition_triangulate_MONO();
         break;

      case TRIANGULATE_POLYPARTITION_CONVEXPARTITION_HM:
         polypartition_convexPartition_HM();
         break;

      case TRIANGULATE_POLYPARTITION_CONVEXPARTITION_OPT:
         polypartition_convexPartition_OPT();
         break;
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
      case TRIANGULATE_VGTESSELATE_DEFAULT:
         vgtesselate_default();
         break;
#endif // HAVE_VGTESSELATE
   }
}

sBool _Triangulate::storeResult(YAC_Object *_fa) {
   if(YAC_VALID(_fa))
   {
      sUI numTriangles = 0u;

#ifdef HAVE_POLYPARTITION
      if(usePolyPartition())
      {
         numTriangles = polypartition.result.size();
      }
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
      if(useVGTesselate())
      {
         numTriangles = (NULL != vgtesselate.output) ? vgtesselate.output->m_NumVertices / 3u : 0u;
      }
#endif // HAVE_VGTESSELATE

      if(numTriangles > 0)
      {
         if(YAC_TRUE == _fa->yacArrayAlloc(numTriangles * 3u * 2u, 1, YAC_TYPE_FLOAT))
         {
            sUI k = 0u;
            YAC_Value vf;
            vf.initFloat(0.0f);

#ifdef HAVE_POLYPARTITION
            if(usePolyPartition())
            {
               list<TPPLPoly>::iterator iter;

               // Iterate polygons / triangles
               for(iter = polypartition.result.begin(); iter != polypartition.result.end(); iter++)
               {
                  const TPPLPoly *poly = &(*iter);
                  sSI numPoints = poly->GetNumPoints();

                  if(3 == numPoints)
                  {
                     // Iterate triangle vertices
                     for(sSI pointIdx = 0; pointIdx < 3; pointIdx++)
                     {
                        vf.value.float_val = (*poly)[pointIdx].x;
                        _fa->yacArraySet(NULL/*context*/, k++, &vf);

                        vf.value.float_val = (*poly)[pointIdx].y;
                        _fa->yacArraySet(NULL/*context*/, k++, &vf);
                     }
                  }
                  else
                  {
                     yac_host->printf("[~~~] Triangulate::storeResult: unexpected number of polygon vertices (%d != 3)\n", numPoints);
                     return YAC_FALSE;
                  }
               }
            }
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
            if(useVGTesselate())
            {
               // yac_host->printf("xxx store vg res\n");
               // Iterate triangle vertices
               if(NULL != vgtesselate.output->m_IndexBuffer)
               {
                  const VGshort *triIndices = vgtesselate.output->m_IndexBuffer;
                  numTriangles = vgtesselate.output->m_IndexBufferSize / 3u;
                  // yac_host->printf("xxx store vg res<indexed>: numTriangles=%u\n", numTriangles);
                  sUI triJ = 0u;
                  for(sUI triIdx = 0u; triIdx < numTriangles; triIdx++)
                  {
                     for(sUI triVtxIdx = 0u; triVtxIdx < 3u; triVtxIdx++)
                     {
                        VGshort vtxIdx = triIndices[triJ++];
                        // yac_host->printf("xxx store vg res (%f; %f) triIdx=%u vtxIdx=%d\n", vgtesselate.output->m_Vertices[vtxIdx].x, vgtesselate.output->m_Vertices[vtxIdx].y, triIdx, vtxIdx);

                        vf.value.float_val = vgtesselate.output->m_Vertices[vtxIdx].x;
                        _fa->yacArraySet(NULL/*context*/, k++, &vf);

                        vf.value.float_val = vgtesselate.output->m_Vertices[vtxIdx].y;
                        _fa->yacArraySet(NULL/*context*/, k++, &vf);
                     }
                  }
               }
               else
               {
                  for(sUI vtxIdx = 0u; vtxIdx < vgtesselate.output->m_NumVertices; vtxIdx++)
                  {
                     // yac_host->printf("xxx store vg res (%f; %f)\n", vgtesselate.output->m_Vertices[idx].x, vgtesselate.output->m_Vertices[idx].y);
                     vf.value.float_val = vgtesselate.output->m_Vertices[vtxIdx].x;
                     _fa->yacArraySet(NULL/*context*/, k++, &vf);

                     vf.value.float_val = vgtesselate.output->m_Vertices[vtxIdx].y;
                     _fa->yacArraySet(NULL/*context*/, k++, &vf);
                  }
               }
            }
#endif // HAVE_VGTESSELATE

            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] Triangulate::storeResult: failed to allocate destination array (%u triangles / %u vertices / %u elements)\n", numTriangles, numTriangles*3u, numTriangles*3u*2u);
         }
      }
      else
      {
         yac_host->printf("[---] Triangulate::storeResult: number of result triangles is ZERO.\n");
      }
   }
   return YAC_FALSE;
}


sBool _Triangulate::storeResultIndexed(YAC_Object *_fa, YAC_Object *_ia) {
   if(YAC_VALID(_fa) && YAC_VALID(_ia))
   {
      sUI numVertices = 0u;
      sUI numIndices  = 0u;

#ifdef HAVE_POLYPARTITION
      if(usePolyPartition())
      {
         numVertices = polypartition.result.size();
         numIndices  = numVertices * 3u;
      }
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
      if(useVGTesselate())
      {
         numVertices = (NULL != vgtesselate.output) ? vgtesselate.output->m_NumVertices     : 0u;
         numIndices  = (NULL != vgtesselate.output) ? vgtesselate.output->m_IndexBufferSize : 0u;
      }
#endif // HAVE_VGTESSELATE

      if(numVertices > 0 && numIndices > 0)
      {
         if( (YAC_TRUE == _fa->yacArrayAlloc(numVertices * 2u, 1, YAC_TYPE_FLOAT)) &&
             (YAC_TRUE == _ia->yacArrayAlloc(numIndices,       1, YAC_TYPE_INT))
             )
         {
            sUI vtxOut = 0u;
            YAC_Value vf;
            vf.initFloat(0.0f);

            YAC_Value vi;
            vi.initInt(0);
            sSI idxOut = 0;

#ifdef HAVE_POLYPARTITION
            if(usePolyPartition())
            {
               list<TPPLPoly>::iterator iter;

               // Iterate polygons / triangles
               for(iter = polypartition.result.begin(); iter != polypartition.result.end(); iter++)
               {
                  const TPPLPoly *poly = &(*iter);
                  sSI numPoints = poly->GetNumPoints();

                  if(3 == numPoints)
                  {
                     // Iterate triangle vertices
                     for(sSI pointIdx = 0; pointIdx < 3; pointIdx++)
                     {
                        vf.value.float_val = (*poly)[pointIdx].x;
                        _fa->yacArraySet(NULL/*context*/, vtxOut++, &vf);

                        vf.value.float_val = (*poly)[pointIdx].y;
                        _fa->yacArraySet(NULL/*context*/, vtxOut++, &vf);

                        vi.value.int_val = idxOut;
                        _ia->yacArraySet(NULL/*context*/, idxOut++, &vi);
                     }
                  }
                  else
                  {
                     yac_host->printf("[~~~] Triangulate::storeResult: unexpected number of polygon vertices (%d != 3)\n", numPoints);
                     return YAC_FALSE;
                  }
               }
            }
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
            if(useVGTesselate())
            {
               // yac_host->printf("xxx store vg res\n");
               // Iterate triangle vertices
               if(NULL != vgtesselate.output->m_IndexBuffer)
               {
                  // yac_host->printf("xxx store vg res<indexed>: numTriangles=%u\n", numIndices/3u);

                  // Store vertices
                  for(sUI vtxIdx = 0u; vtxIdx < numVertices; vtxIdx++)
                  {
                     // yac_host->printf("xxx store vg res<indexed> (%f; %f) vtxIdx=%un", vgtesselate.output->m_Vertices[vtxIdx].x, vgtesselate.output->m_Vertices[vtxIdx].y, vtxIdx);

                     vf.value.float_val = vgtesselate.output->m_Vertices[vtxIdx].x;
                     _fa->yacArraySet(NULL/*context*/, vtxOut++, &vf);

                     vf.value.float_val = vgtesselate.output->m_Vertices[vtxIdx].y;
                     _fa->yacArraySet(NULL/*context*/, vtxOut++, &vf);
                  }

                  // Store indices
                  const VGshort *triIndices = vgtesselate.output->m_IndexBuffer;
                  for(sUI idx = 0u; idx < numIndices; idx++)
                  {
                     VGshort vtxIdx = triIndices[idx];
                     // yac_host->printf("xxx store vg res<indexed> idx=%u vtxIdx=%d\n", idx, vtxIdx);

                     vi.value.int_val = vtxIdx;
                     _ia->yacArraySet(NULL/*context*/, idxOut++, &vi);                    
                  }
               }
               else
               {
                  for(sUI vtxIdx = 0u; vtxIdx < numVertices; vtxIdx++)
                  {
                     // yac_host->printf("xxx store vg res (%f; %f)\n", vgtesselate.output->m_Vertices[idx].x, vgtesselate.output->m_Vertices[idx].y);
                     vf.value.float_val = vgtesselate.output->m_Vertices[vtxIdx].x;
                     _fa->yacArraySet(NULL/*context*/, vtxOut++, &vf);

                     vf.value.float_val = vgtesselate.output->m_Vertices[vtxIdx].y;
                     _fa->yacArraySet(NULL/*context*/, vtxOut++, &vf);

                     vi.value.int_val = idxOut;
                     _ia->yacArraySet(NULL/*context*/, idxOut++, &vi);
                  }
               }
            }
#endif // HAVE_VGTESSELATE

            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] Triangulate::storeResultIndexed: failed to allocate destination array (%u vertices / %u indices)\n", numVertices, numIndices);
         }
      }
      else
      {
         yac_host->printf("[---] Triangulate::storeResultIndexed: number of result triangles is ZERO.\n");
      }
   }
   return YAC_FALSE;
}


// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

   // Resolve "standard" exception IDs
   Dyac_std_exid_resolve;

   YAC_Init_tktriangulate(_host);
   yac_host->printf("init tktriangulate done.\n");
}


// ---------------------------------------------------------------------------- YAC_Exit
void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_tktriangulate(_host);
}


#include <yac_host.cpp>
