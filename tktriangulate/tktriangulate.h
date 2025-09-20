/// tktriangulate.h
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

#ifndef __TKTRIANGULATE_H__
#define __TKTRIANGULATE_H__


YG("tktriangulate")


YC class _Triangulate  : public YAC_Object {
  protected:
   sSI engine_type;  // see TRIANGULATE_xxx

  protected:
   // polypartition:
   //  - input must be in clock-wise winding order
   //  - does not support self-intersecting polygons
#ifdef HAVE_POLYPARTITION
   struct {
      TPPLPartition pp;
      list<TPPLPoly> input;
      list<TPPLPoly> result_poly;
      list<TPPLPoly> result;
   } polypartition;
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
   struct {
      Element *input;  // single-linked list of polygons (or NULL). filled by addPolygon*(), deleted in freeInput()
      Element *output;
   } vgtesselate;
#endif // HAVE_VGTESSELATE

  public:
   YAC(_Triangulate);

   _Triangulate(void);
   ~_Triangulate();

  public:
   /* void YAC_VCALL yacValueOfI(sSI _i) { i = _i; } */
   // Triangulation by ear clipping
   //  - Time/Space complexity: O(n^2)/O(n)
   //  - Supports holes: Yes, by calling TPPLPartition::RemoveHoles
   //  - Quality of solution: Satisfactory in most cases
   #define TRIANGULATE_POLYPARTITION_TRIANGULATE_EC       YCI  0

   // Optimal triangulation in terms of edge length using dynamic programming algorithm
   //  - Method: TPPLPartition::Triangulate_OPT
   //  - Time/Space complexity: O(n^3)/O(n^2)
   //  - Supports holes: No. You could call TPPLPartition::RemoveHoles prior to calling TPPLPartition::Triangulate_OPT, 
   //     but the solution would no longer be optimal, thus defeating the purpose
   //  - Quality of solution: Optimal in terms of minimal edge length
   #define TRIANGULATE_POLYPARTITION_TRIANGULATE_OPT      YCI  1

   // Triangulation by partition into monotone polygons
   //  - Method: TPPLPartition::Triangulate_MONO
   //  - Time/Space complexity: O(n*log(n))/O(n)
   //  - Supports holes: Yes, by design
   //  - Quality of solution: Poor. Many thin triangles are created in most cases
   #define TRIANGULATE_POLYPARTITION_TRIANGULATE_MONO     YCI  2

   // Convex partition using Hertel-Mehlhorn algorithm
   //  - Method: TPPLPartition::ConvexPartition_HM
   //  - Time/Space complexity: O(n^2)/O(n)
   //  - Supports holes: Yes, by calling TPPLPartition::RemoveHoles
   //  - Quality of solution: At most four times the minimum number of convex polygons is created. 
   //     However, in practice it works much better than that and often gives optimal partition.
   #define TRIANGULATE_POLYPARTITION_CONVEXPARTITION_HM   YCI  3

   // - Optimal convex partition using dynamic programming algorithm by Keil and Snoeyink
   // - Method: TPPLPartition::ConvexPartition_OPT
   // - Time/Space complexity: O(n^3)/O(n^3)
   // - Supports holes: No. You could call TPPLPartition::RemoveHoles prior to calling TPPLPartition::Triangulate_OPT, 
   //    but the solution would no longer be optimal, thus defeating the purpose
   // - Quality of solution: Optimal. A minimum number of convex polygons is produced};
   #define TRIANGULATE_POLYPARTITION_CONVEXPARTITION_OPT  YCI  4

   // TES' OpenVG triangulator. Based on SGI's triangulator. Supports self-intersecting polygons (for creating holes).
   #define TRIANGULATE_VGTESSELATE_DEFAULT                YCI  5

#ifdef HAVE_POLYPARTITION
  protected:
   void polypartition_triangulate_EC (void);
   void polypartition_triangulate_OPT (void);
   void polypartition_triangulate_MONO (void);
   void polypartition_convexPartition_HM (void);
   void polypartition_convexPartition_OPT (void);
#endif // HAVE_POLYPARTITION

#ifdef HAVE_VGTESSELATE
   void vgtesselate_default (void);
#endif // HAVE_VGTESSELATE

  public:
   // Remove all polygons
   YM void free (void);

   // Remove all polygons
   YM void freeInput (void);

   // Remove result triangles
   YM void freeResult (void);

   // Select triangulator engine (TRIANGULATE_xxx)
   YM void setEngine (sSI _engineType);   

   YM sBool usePolyPartition (void);
   YM sBool useVGTesselate (void);

   // x/y pairs
   YM void addPolygonRegionEx (YAC_Object *_fa, sUI _startOff, sUI _numElements, sBool _bHole);

   // x/y pairs
   YM void addPolygonEx (YAC_Object *_fa, sBool _bHole);

   // x/y pairs
   YM void addPolygon (YAC_Object *_fa);

   // x/y pairs
   YM void addPolygonHole (YAC_Object *_fa);

   // x/y pairs
   YM void addPolygonRegion (YAC_Object *_fa, sUI _startOff, sUI _numElements);

   // x/y pairs
   YM void addPolygonRegionHole (YAC_Object *_fa, sUI _startOff, sUI _numElements);

   // Calculate result triangles
   YM void triangulate (void);

   // Reallocate and copy result triangles vertices to array object
   YM sBool storeResult (YAC_Object *_fa);

   // Reallocate and copy result triangles vertices and indices to array objects
   YM sBool storeResultIndexed (YAC_Object *_fa, YAC_Object *_ia);

};


#endif // __TKTRIANGULATE_H__
