// ----
// ---- file   : minnie.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2014-2025 by bsp
// ----
// ----          Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// ----          associated documentation files (the "Software"), to deal in the Software without restriction, including
// ----          without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// ----          copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to
// ----          the following conditions:
// ----
// ----          The above copyright notice and this permission notice shall be included in all copies or substantial
// ----          portions of the Software.
// ----
// ----          THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// ----          NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// ----          IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// ----          WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// ----          SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----
// ---- info   : "minnie" binary loader + renderer / OpenGL attrib buffer exporter
// ----
// ----
// ----

#pragma once

#define YG(a)
YG("minnie");

#include <stdlib.h>

#if defined(__cplusplus)
#include <new>
#endif

#ifdef HAVE_VGTESSELATE
#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus
#include "VG_tesselator/tesselator/tessbinding.h"
#if defined(__cplusplus)
}
#endif // __cplusplus
#endif // HAVE_VGTESSELATE

#ifndef MINNIE_SW_RENDER
#define MINNIE_SW_RENDER 1
#endif // MINNIE_SW_RENDER

#ifndef MINNIE_BIG_ENDIAN
#define MINNIE_BIG_ENDIAN 0
#endif // MINNIE_BIG_ENDIAN

#ifndef MINNIE_ALLOC_DEBUG
#define MINNIE_ALLOC_DEBUG 1
#endif // MINNIE_ALLOC_DEBUG

#ifndef MINNIE_ARRAY_BOUNDARY_CHECK
#define MINNIE_ARRAY_BOUNDARY_CHECK 1
#endif // MINNIE_ARRAY_BOUNDARY_CHECK

#ifndef MINNIE_MAX_FRAMEBUFFERS
#define MINNIE_MAX_FRAMEBUFFERS 8u
#endif // MINNIE_MAX_FRAMEBUFFERS

#ifndef MINNIE_MAX_PATHS
/* #define MINNIE_MAX_PATHS 512u */
/* #define MINNIE_MAX_PATHS 1024u */
#define MINNIE_MAX_PATHS 4096u
#endif // MINNIE_MAX_PATHS

#ifndef MINNIE_MAX_SUB_PATHS
/* #define MINNIE_MAX_SUB_PATHS 16u */
/* #define MINNIE_MAX_SUB_PATHS 192u */
#define MINNIE_MAX_SUB_PATHS 256u
#endif // MINNIE_MAX_SUB_PATHS

#ifndef MINNIE_MAX_COLORS
#define MINNIE_MAX_COLORS 256u
#endif // MINNIE_MAX_COLORS

#ifndef MINNIE_MAX_MAT_2D
#define MINNIE_MAX_MAT_2D 32u
#endif // MINNIE_MAX_MAT_2D

#ifndef MINNIE_MAX_MAT_3D
#define MINNIE_MAX_MAT_3D 32u
#endif // MINNIE_MAX_MAT_3D

// when 1, export vertices and edge-AA scaling factors as signed 16bit integers (scaled by 4.0)
// when 0, export as regular 32bit floats
#ifndef MINNIE_EXPORT_VERTEX_16BIT
#define MINNIE_EXPORT_VERTEX_16BIT   1
#endif // MINNIE_EXPORT_VERTEX_16BIT

#ifndef MINNIE_EXPORT_TRIS_SIMPLE
// debug. export as triangle array (u8 r,g,b,a followed by f32 x,y. 12 bytes/vertex) (MSAA)
#define MINNIE_EXPORT_TRIS_SIMPLE  1
#endif // MINNIE_EXPORT_TRIS_SIMPLE

#ifndef MINNIE_EXPORT_TRIS_EDGEAA
// debug. export as triangle array (pos,color,edgeflags,s/t/u scaling. 28 bytes/vertex)
#define MINNIE_EXPORT_TRIS_EDGEAA              1
#define MINNIE_EXPORT_TRIS_EDGEAA_BC           1    // 1=export barycentric coordinates (30 bytes/vertex)
#define MINNIE_EXPORT_TRIS_EDGEAA_SCALE_16BIT  0    // 1=export scale factors as 16bit ints (*1024)  BREAKS RENDERING
#define MINNIE_EDGEAA_FIND_SHARED              1    // high CPU load (e.g. test076_hamster: 121ms with, 1ms without)
#define MINNIE_EDGEAA_CALC_FACTORS             1
/* // turn off 'simple' mesh export in edgeAA mode: */
/* /\* #if MINNIE_EXPORT_TRIS_EDGEAA *\/ */
/* /\* #undef MINNIE_EXPORT_TRIS_SIMPLE *\/ */
/* /\* #define MINNIE_EXPORT_TRIS_SIMPLE 0 *\/ */
/* /\* #endif // MINNIE_EXPORT_TRIS_EDGEAA *\/ */
#endif // MINNIE_EXPORT_TRIS_EDGEAA

#ifndef MINNIE_PROCEDURAL_API
// include procedural API (api*() methods)
#define MINNIE_PROCEDURAL_API  1
#endif // MINNIE_PROCEDURAL_API

#ifndef MINNIE_SKIP_TYPEDEFS  // (note) when not including "yac.h"
typedef char            sChar;
typedef unsigned char   sU8;
typedef signed char     sS8;
typedef unsigned short  sU16;
typedef signed short    sS16;
typedef unsigned int    sU32;
typedef signed int      sS32;
typedef float           sF32;
typedef int             sBool;

#ifndef NULL
#define NULL 0
#endif // NULL

#define sABS(x) (((x)>0)?(x):-(x))
#define sMIN(a,b) (((a)>(b))?(b):(a))
#define sMAX(a,b) (((a)>(b))?(a):(b))

#define sM_PI  3.14159265358f
#define sM_2PI 6.28318530718f

#endif // MINNIE_SKIP_TYPEDEFS

#define Dgrowarray(a) (sMAX(256u,(((a)+1u) + ((a)>>1))))

// helper macros for writing to a stream
#ifdef SHADERVG_SCRIPT_API
#define Dstream_write_i8(s,v)  ((YAC_Object*)(s))->yacStreamWriteI8(v)
#define Dstream_write_i16(s,v) ((YAC_Object*)(s))->yacStreamWriteI16(v)
#define Dstream_write_s16(s,v) ((YAC_Object*)(s))->yacStreamWriteI16(v)
#define Dstream_write_i32(s,v) ((YAC_Object*)(s))->yacStreamWriteI32(v)
#define Dstream_write_f32(s,v) ((YAC_Object*)(s))->yacStreamWriteF32(v)
#define Dstream_write_2f(s,v1,v2) ((YAC_Object*)(s))->yacStreamWriteF32(v1); ((YAC_Object*)(s))->yacStreamWriteF32(v2)
#define Dstream_get_offset(s)  ((YAC_Object*)(s))->yacStreamGetOffset()
#define Dstream_set_offset(s,o) ((YAC_Object*)(s))->yacStreamSetOffset(o)

// helper macros for reading from a stream
#define Dstream_read_i8(s)  ((YAC_Object*)(s))->yacStreamReadI8()
#define Dstream_read_i16(s) ((YAC_Object*)(s))->yacStreamReadI16()
#define Dstream_read_s16(s) ((YAC_Object*)(s))->yacStreamReadI16()
#define Dstream_read_i32(s) ((YAC_Object*)(s))->yacStreamReadI32()
#define Dstream_read_f32(s) ((YAC_Object*)(s))->yacStreamReadF32()

#else
// MINNIE_LIB
#define Dstream_write_i8(s,v)  ((YAC_Buffer*)(s))->writeI8(v)
#define Dstream_write_i16(s,v) ((YAC_Buffer*)(s))->writeI16(v)
#define Dstream_write_s16(s,v) ((YAC_Buffer*)(s))->writeI16(v)
#define Dstream_write_i32(s,v) ((YAC_Buffer*)(s))->writeI32(v)
#define Dstream_write_f32(s,v) ((YAC_Buffer*)(s))->writeF32(v)
#define Dstream_write_2f(s,v1,v2) ((YAC_Buffer*)(s))->writeF32(v1); ((YAC_Buffer*)(s))->writeF32(v2)
#define Dstream_get_offset(s)  ((YAC_Buffer*)(s))->io_offset
#define Dstream_set_offset(s,o) ((YAC_Buffer*)(s))->io_offset = (o)

// helper macros for reading from a stream
#define Dstream_read_i8(s)  ((YAC_Buffer*)(s))->readI8()
#define Dstream_read_i16(s) ((YAC_Buffer*)(s))->readI16()
#define Dstream_read_s16(s) ((YAC_Buffer*)(s))->readI16()
#define Dstream_read_i32(s) ((YAC_Buffer*)(s))->readI32()
#define Dstream_read_f32(s) ((YAC_Buffer*)(s))->readF32()
#endif // SHADERVG_SCRIPT_API


// helper macros for writing to vertex buffer stream
#define Dexport_vb_i8(a)  Dstream_write_i8(minnie::setup::loc_vb_export_ofs, a)
#define Dexport_vb_i16(a) Dstream_write_i16(minnie::setup::loc_vb_export_ofs, a)
#define Dexport_vb_i32(a) Dstream_write_i32(minnie::setup::loc_vb_export_ofs, a)
#define Dexport_vb_f32(a) Dstream_write_f32(minnie::setup::loc_vb_export_ofs, a)
#define Dexport_vb_add2f(a, b)       Dexport_vb_f32(a); Dexport_vb_f32(b)
#define Dexport_vb_add3f(a, b, c)    Dexport_vb_f32(a); Dexport_vb_f32(b); Dexport_vb_f32(c)
#define Dexport_vb_add4f(a, b, c, d) Dexport_vb_f32(a); Dexport_vb_f32(b); Dexport_vb_f32(c); Dexport_vb_f32(d)
#define Dexport_vb_get_offset() Dstream_get_offset(minnie::setup::loc_vb_export_ofs)

// helper macros for writing to display list buffer stream
#define Dexport_dl_i8(a)  Dstream_write_i8(minnie::setup::loc_dl_export_ofs, a)
#define Dexport_dl_i16(a) Dstream_write_i16(minnie::setup::loc_dl_export_ofs, a)
#define Dexport_dl_i32(a) Dstream_write_i32(minnie::setup::loc_dl_export_ofs, a)
#define Dexport_dl_f32(a) Dstream_write_f32(minnie::setup::loc_dl_export_ofs, a)
#define Dexport_dl_vboffset32() Dstream_write_i32(minnie::setup::loc_dl_export_ofs, Dexport_vb_get_offset())

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ draw ops
#define MINNIE_DRAWOP_END                                  0x00
#define MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_32               0x01
#define MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_14_2             0x02
#define MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_32            0x03
#define MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_14_2          0x04
#define MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_EDGEAA_32        0x05
#define MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_EDGEAA_14_2      0x06
#define MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_32     0x07
#define MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_14_2   0x08
#define MINNIE_DRAWOP_POLYGON_FILL_FLAT_32                 0x09
#define MINNIE_DRAWOP_POLYGON_FILL_FLAT_14_2               0x0A
#define MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_32              0x0B
#define MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_14_2            0x0C
#define MINNIE_DRAWOP_RECT_FILL                            0x0D
#define MINNIE_DRAWOP_RECT_STROKE                          0x0E
#define MINNIE_DRAWOP_RECT_FILL_STROKE                     0x0F
#define MINNIE_DRAWOP_ELLIPSE_FILL                         0x10
#define MINNIE_DRAWOP_ELLIPSE_STROKE                       0x11
#define MINNIE_DRAWOP_ELLIPSE_FILL_STROKE                  0x12
#define MINNIE_DRAWOP_ROUNDRECT_FILL                       0x13
#define MINNIE_DRAWOP_ROUNDRECT_STROKE                     0x14
#define MINNIE_DRAWOP_ROUNDRECT_FILL_STROKE                0x15
#define MINNIE_DRAWOP_BIND_TEXTURE                         0x16
#define MINNIE_DRAWOP_UNBIND_TEXTURE                       0x17
#define MINNIE_DRAWOP_TEXTURE_DECAL_ALPHA                  0x18
#define MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_32             0x19
#define MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_DECAL_32       0x1A
#define MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_32          0x1B
#define MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_DECAL_32    0x1C
#define MINNIE_DRAWOP_LINE_STRIP_FLAT_14_2                 0x1D
#define MINNIE_DRAWOP_LINE_STRIP_FLAT_BEVEL_14_2           0x1E

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// (note) must match shadervg settings
#define MINNIE_LINE_STROKE_W_OFFSET           0.75f
#define MINNIE_RECT_FILL_AA_SIZE_OFFSET       1.0f
#define MINNIE_RECT_AA_SIZE_OFFSET            0.75f
#define MINNIE_RECT_AA_STROKE_OFFSET          0.25f
#define MINNIE_ELLIPSE_FILL_AA_SIZE_OFFSET    1.0f
#define MINNIE_ELLIPSE_AA_SIZE_OFFSET         0.75f
#define MINNIE_ELLIPSE_AA_STROKE_OFFSET       0.25f
#define MINNIE_ROUNDRECT_FILL_AA_SIZE_OFFSET  1.0f
#define MINNIE_ROUNDRECT_AA_SIZE_OFFSET       0.75f
#define MINNIE_ROUNDRECT_AA_STROKE_OFFSET     0.25f

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ rasterizer
#if defined(MINNIE_IMPLEMENTATION) && defined(MINNIE_SW_RENDER)
#define Dprintf if(0);else printf
#define AA_C defined
#include "../tkopengl/tri_raster_aa.h"
// #if 0
// #include "../tkopengl/tri_raster_aa_fx.h"
// #else
// #define AAFX_C defined
// #define AAFX_USE_DIVSW   0
// #define AAFX_DELTA       0
// #define AAFX_ALLOW_AA    1
// #define AAFX_NUM_SAMPLES 4
// #define AAFX_NOAA_BIAS   1
// #include "../tkopengl/tri_raster_aa_fx_opt.h"
// #endif

#define HSPAN_DIV_TBL 1
#define HSPAN_TOPLEFT_FILLRULE 1
#define HSPAN_SAFE 1
#define Dprintfmissing if(0);else printf
#include "../tkopengl/tri_hspan.h"
#undef Dprintf
#undef Dprintfmissing
#endif // MINNIE_IMPLEMENTATION && MINNIE_SW_RENDER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ /rasterizer

#define Dprintf        if(0);else printf
#define Derrorprintf   if(0);else printf
#define Dtorprintf     if(1);else printf  // Constructor / Destructor debug
#define Dinitprintf    if(1);else printf  // object init debug
#define Dallocprintf   if(1);else printf  // object alloc debug
#define Dedgeaaprintfv if(1);else printf
#define Dedgeaaprintf  if(1);else printf
#define Dexportprintf  if(1);else printf
#define Dexportprintfv if(1);else printf
#define Dapiprintf     if(0);else printf
#define Dapierror      if(0);else printf

#define Dkbytes(a)  (sSI(a*(100.0f/1024.0f))/100.0f)

// row major matrix access helpers
//   0 1 2
//   3 4 5
#define M2x3(a, row, col) (((row)*3)+(col))
#define M3(a, row, col)   (((row)*3)+(col))
#define M4(a, row, col)   (((row)<<2)+(col))

#define TM2x3(row, col)  M2x3(this,row,col)
#define TM3(row, col)    M3(this,row,col)
#define TM4(row, col)    M4(this,row,col)

#define OM2x3(row, col)  M2x3(o,row,col)
#define OM3(row, col)    M3(o,row,col)
#define OM4(row, col)    M4(o,row,col)

#define TM2x3F(r,c)  floats[TM2x3(r,c)]
#define OM2x3F(r,c)  o->floats[OM2x3(r,c)]
#define M2x3F(a,r,c) (a)->floats[M2x3(a,r,c)]

#define TM3F(r,c)  floats[TM3(r,c)]
#define OM3F(r,c)  o->floats[OM3(r,c)]
#define M3F(a,r,c) (a)->floats[M3(a,r,c)]

#define TM4F(r,c)  floats[TM4(r,c)]
#define OM4F(r,c)  o->floats[OM4(r,c)]
#define M4F(a,r,c) (a)->floats[M4(a,r,c)]

// <class.png>
struct Vector2f {
   sF32 x, y;

#ifdef __cplusplus
   void operator =(const Vector2f *_o) {
      x = _o->x;
      y = _o->y;
   }

   void operator =(const Vector2f &_o) {
      x = _o.x;
      y = _o.y;
   }

   bool operator ==(const Vector2f *_o) {
      return (x == _o->x) && (y == _o->y);
   }

   bool operator ==(const Vector2f &_o) {
      return (x == _o.x) && (y == _o.y);
   }

   void copyFrom(const Vector2f *_o) {
      x = _o->x;
      y = _o->y;
   }

   void init(sF32 _x, sF32 _y) {
      x = _x;
      y = _y;
   }

   void add(const Vector2f *_o) {
      x += _o->x;
      y += _o->y;
   }

   void addFrom(const Vector2f *_a, const Vector2f *_b) {
      x = _a->x + _b->x;
      y = _a->y + _b->y;
   }

   void sub(const Vector2f *_o) {
      x -= _o->x;
      y -= _o->y;
   }

   void subFrom(const Vector2f *_a, const Vector2f *_b) {
      x = _a->x - _b->x;
      y = _a->y - _b->y;
   }

   sF32 dot(const Vector2f *_o) const {
      return x * _o->x + y * _o->y;
   }

   inline sF32 crossZ(const Vector2f *_o) const {
      return x * _o->y - y * _o->x;
   }

   void mulf(const sF32 _s) {
      x *= _s;
      y *= _s;
   }

   void mulfFrom(const Vector2f *_o, const sF32 _s) {
      x = _o->x * _s;
      y = _o->y * _s;
   }

   sF32 getAbs(void) const {
      return ::sqrtf(x*x + y*y);
   }

   void unit(void) {
      sF32 absval = getAbs();
      if(0.0f != absval)
      {
         absval = 1.0f / absval;
         x *= absval;
         y *= absval;
      }
      else
      {
         x = 0.0f;
         y = 0.0f;
      }
   }

   void unitScale(const sF32 _s) {
      sF32 absval = getAbs();
      if(0.0f != absval)
      {
         absval = _s / absval;
         x *= absval;
         y *= absval;
      }
      else
      {
         x = 0.0f;
         y = 0.0f;
      }
   }

#if 0
   void unitScale2f(const sF32 _sx, const sF32 _sy) {
      sF32 absval = getAbs();
      if(0.0f != absval)
      {
         absval = 1.0f / absval;
         x *= _sx * absval;
         y *= _sy * absval;
      }
      else
      {
         x = 0.0f;
         y = 0.0f;
      }
   }
#endif

   void unitScaleFrom(const Vector2f *_v, const sF32 _s) {
      sF32 absval = _v->getAbs();
      if(0.0f != absval)
      {
         absval = _s / absval;
         x = _v->x * absval;
         y = _v->y * absval;
      }
      else
      {
         x = 0.0f;
         y = 0.0f;
      }
   }

   inline void rotateCW90(void) {
      const sF32 t = x;
      x = y;
      y = -t;
   }

   inline void rotateCCW90(void) {
      const sF32 t = x;
      x = -y;
      y = t;
   }

   /* method distanceToPlane,Vector2f q,Vector2f n:float
      Calc distance of vertex to plane defined by Vector2f q (on plane) and plane normal n
      @arg q Point on plane
      @arg n Plane normal
      @return distance
    */
   sF32 distanceToPlane(const Vector2f *_q, const Vector2f *_n) const {
      Vector2f v; v.subFrom(this, _q);
      const sF32 nAbs = _n->getAbs();
      return (0.0f != nAbs) ? (v.dot(_n) / nAbs) : 999999999.0f;
   }

   sF32 intersect(const Vector2f *_v1s, const Vector2f *_v1e,
                  const Vector2f *_v2s, const Vector2f *_v2e,
                  const sBool _bExtrapolate
                  ) {
      // returns intersection point in 'this'
      // returns normalized position on edge v2s<>v2e

      Vector2f vE; vE.subFrom(_v1e, _v1s);
      Vector2f vF; vF.subFrom(_v2e, _v2s);
      Vector2f vP; vP.init(-vE.y, vE.x);
      Vector2f vG; vG.init(_v1s->x - _v2s->x, _v1s->y - _v2s->y);
      sF32 h = vF.dot(&vP);
      if(0.0f != h)
         h = vG.dot(&vP) / h;
      if( _bExtrapolate || ((0.0 <= h) && (h <= 1.0)) )
      {
         mulfFrom(&vF, h);  // (todo) fmaFrom()
         add(_v2s);
      }
      else
      {
         copyFrom(_v2s);
      }
      return h;
   }
#endif // __cplusplus

};

// <class.png>
#if defined(__cplusplus)
struct Vector4f {
#else
typedef struct Vector4f_s {
#endif
   sF32 x, y, z, w;

#ifdef __cplusplus
   void init(sF32 _x, sF32 _y, sF32 _z, sF32 _w) {
      x = _x;
      y = _y;
      z = _z;
      w = _w;
   }

   void divzTo(Vector2f *_v2) {
      if(z > 0.0f)
      {
         _v2->x = x / z;
         _v2->y = y / z;
      }
      else
      {
         _v2->x = 0.0f;
         _v2->y = 0.0f;
      }
   }
#endif // __cplusplus

#if defined(__cplusplus)
};
#else
} Vector4f;
#endif


// <class.png>
#if defined(__cplusplus)
struct Matrix4f {
#else
typedef struct Matrix4f_s {
#endif
   // row major
   sF32 floats[4*4];

#ifdef __cplusplus
   void copyFrom(const Matrix4f *_o) {
      ::memcpy(floats, _o->floats, sizeof(floats));
   }

   void init(sF32 a, sF32 b, sF32 c, sF32 d,
             sF32 e, sF32 f, sF32 g, sF32 h,
             sF32 i, sF32 j, sF32 k, sF32 l,
             sF32 m, sF32 n, sF32 o, sF32 p
             ) {
      // a b c d  (row-major)
      // e f g h
      // i j k l
      // m n o p
      floats[ 0] = a;  floats[ 1] = b;  floats[ 2] = c;  floats[ 3] = d;
      floats[ 4] = e;  floats[ 5] = f;  floats[ 6] = g;  floats[ 7] = h;
      floats[ 8] = i;  floats[ 9] = j;  floats[10] = k;  floats[11] = l;
      floats[12] = m;  floats[13] = n;  floats[14] = o;  floats[15] = p;
   }

   void initIdentity(void) {
      TM4F(0,0) = 1.0f;  TM4F(0,1) = 0.0f;  TM4F(0,2) = 0.0f;  TM4F(0,3) = 0.0f;
      TM4F(1,0) = 0.0f;  TM4F(1,1) = 1.0f;  TM4F(1,2) = 0.0f;  TM4F(1,3) = 0.0f;
      TM4F(2,0) = 0.0f;  TM4F(2,1) = 0.0f;  TM4F(2,2) = 1.0f;  TM4F(2,3) = 0.0f;
      TM4F(3,0) = 0.0f;  TM4F(3,1) = 0.0f;  TM4F(3,2) = 0.0f;  TM4F(3,3) = 1.0f;
   }

   void initOrtho(sF32 _left,   sF32 _right,
                  sF32 _bottom, sF32 _top,
                  sF32 _znear,  sF32 _zfar
                  ) {
      /*
       *  2/(r-l)   0        0         -(r+l)/(r-l)
       *  0         2/(t-b)  0         -(t+b)/(t-b)
       *  0         0        -2/(f-n)  -(f+n)/(f-n)
       *  0         0        0         1
       *
       */
      const sF32 rml = _right - _left;
      const sF32 tmb = _top   - _bottom;
      const sF32 fmn = _zfar  - _znear;

      if(0.0f != rml && tmb != 0.0f && fmn != 0.0f)
      {
         TM4F(0, 0) = 2.0f / rml;
         TM4F(0, 1) = 0.0f;
         TM4F(0, 2) = 0.0f;
         TM4F(0, 3) = -(_right + _left) / rml;

         TM4F(1, 0) = 0.0f;
         TM4F(1, 1) = 2.0f / tmb;
         TM4F(1, 2) = 0.0f;
         TM4F(1, 3) = -(_top + _bottom) / tmb;

         TM4F(2, 0) = 0.0f;
         TM4F(2, 1) = 0.0f;
         TM4F(2, 2) = -2.0f / fmn;
         TM4F(2, 3) = -(_zfar + _znear) / fmn;

         TM4F(3, 0) = 0.0f;
         TM4F(3, 1) = 0.0f;
         TM4F(3, 2) = 0.0f;
         TM4F(3, 3) = 1.0f;
      }
   }

   void initScalef(sF32 _x, sF32 _y, sF32 _z) {
      init(_x,    0.0f,  0.0f, 0.0f,
           0.0f, _y,     0.0f, 0.0f,
           0.0f,  0.0f, _z,    0.0f,
           0.0f,  0.0f,  0.0f, 1.0f
           );
   }

   void initTranslatef(sF32 _x, sF32 _y, sF32 _z) {
      init(1, 0, 0, _x,
           0, 1, 0, _y,
           0, 0, 1, _z,
           0, 0, 0,  1
           );
   }

   void initRotatef(sF32 a, sF32 b, sF32 c) {
      sF32 s1 = sinf(a), c1 = cosf(a);
      sF32 s2 = sinf(b), c2 = cosf(b);
      sF32 s3 = sinf(c), c3 = cosf(c);

      TM4F(0,0) = c2*c3;
      TM4F(0,1) = c3*s1*s2-c1*s3;
      TM4F(0,2) = c1*c3*s2+s1*s3;
      TM4F(0,3) = 0.0f;

      TM4F(1,0) = c2*s3;
      TM4F(1,1) = c1*c3+s1*s2*s3;
      TM4F(1,2) = c1*s2*s3-c3*s1;
      TM4F(1,3) = 0.0f;

      TM4F(2,0) = -s2;
      TM4F(2,1) = c2*s1;
      TM4F(2,2) = c1*c2;
      TM4F(2,3) = 0.0f;

      TM4F(3,0) = 0.0f;
      TM4F(3,1) = 0.0f;
      TM4F(3,2) = 0.0f;
      TM4F(3,3) = 1.0f;
   }

   void mulv(Vector4f *_v) const {
      sF32 t[4];
      for(sUI y = 0u; y < 4u; y++)
      {
         t[y] =
            TM4F(y,0) * _v->x +
            TM4F(y,1) * _v->y +
            TM4F(y,2) * _v->z +
            TM4F(y,3) * _v->w ;
      }
      _v->x = t[0];
      _v->y = t[1];
      _v->z = t[2];
      _v->w = t[3];
   }

   void mulr(const Matrix4f *o, Matrix4f *r) const {
      // Multiply by matrix o (this * o) and store in r
      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            M4F(r,y,x) =
               TM4F(y,0) * OM4F(0,x) +
               TM4F(y,1) * OM4F(1,x) +
               TM4F(y,2) * OM4F(2,x) +
               TM4F(y,3) * OM4F(3,x) ;
         }
      }
   }

   void mul(const Matrix4f *o) {
      // Multiply by matrix o (this * o)
      sF32 t[4][4];

      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            t[y][x] =
               TM4F(y,0) * OM4F(0,x) +
               TM4F(y,1) * OM4F(1,x) +
               TM4F(y,2) * OM4F(2,x) +
               TM4F(y,3) * OM4F(3,x) ;
         }
      }
      for(sUI y = 0u; y < 4u; y++)
      {
         for(sUI x = 0u; x < 4u; x++)
         {
            TM4F(y,x) = t[y][x];
         }
      }
   }

   void scalef(sF32 _x, sF32 _y, sF32 _z) {
      Matrix4f t;
      t.initScalef(_x, _y, _z);
      mul(&t);
   }

   void translatef(sF32 _x, sF32 _y, sF32 _z) {
      Matrix4f t;
      t.initTranslatef(_x, _y, _z);
      mul(&t);
   }

   void rotatef(sF32 _x, sF32 _y, sF32 _z) {
      Matrix4f t;
      t.initRotatef(_x, _y, _z);
      mul(&t);
   }
#endif // __cplusplus

#if defined(__cplusplus)
};
extern "C" {
extern void minnie_matrix4f_initIdentity (Matrix4f *_this);
extern void minnie_matrix4f_initOrtho (Matrix4f *_this, sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear,  sF32 _zfar);
extern void minnie_matrix4f_mulr (const Matrix4f *_this, const Matrix4f *_o, Matrix4f *_r);
extern void minnie_matrix4f_scalef (Matrix4f *_this, sF32 _x, sF32 _y, sF32 _z);
extern void minnie_matrix4f_translatef (Matrix4f *_this, sF32 _x, sF32 _y, sF32 _z);
extern void minnie_matrix4f_rotatef (Matrix4f *_this, sF32 _x, sF32 _y, sF32 _z);
}
#else
} Matrix4f;
extern void minnie_matrix4f_initIdentity (Matrix4f *_this);
extern void minnie_matrix4f_initOrtho (Matrix4f *_this, sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear,  sF32 _zfar);
extern void minnie_matrix4f_mulr (const Matrix4f *_this, const Matrix4f *_o, Matrix4f *_r);
extern void minnie_matrix4f_scalef (Matrix4f *_this, sF32 _x, sF32 _y, sF32 _z);
extern void minnie_matrix4f_translatef (Matrix4f *_this, sF32 _x, sF32 _y, sF32 _z);
extern void minnie_matrix4f_rotatef (Matrix4f *_this, sF32 _x, sF32 _y, sF32 _z);
#endif


#ifdef MINNIE_IMPLEMENTATION  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(__cplusplus)
#error MINNIE_IMPLEMENTATION must be defined in a .cpp file
#endif

namespace minnie {

static sF32 fb_scale      = 1.0f;      // (todo) REMOVE (must be 1.0)
static sF32 geo_scale     = fb_scale;  // (todo) REMOVE (must be 1.0)

static sBool b_draw_tri  = YAC_TRUE;
static sBool b_draw_line = YAC_TRUE;
static sBool b_draw_fill = YAC_TRUE;

static sBool b_draw_sw_tri  = YAC_TRUE;
static sBool b_draw_sw_line = YAC_TRUE;
static sBool b_draw_sw_fill = YAC_TRUE;

static sBool b_debug_op         = 0;  // echo parsed ops  2=echo curve/line/blit
static sBool b_debug_pal        = 0;
static sBool b_debug_fb         = 0;
static sBool b_debug_cubic      = 0;
static sBool b_debug_arc        = 0;
static sBool b_debug_clip2d     = 0;
static sBool b_debug_clip3d     = 0;
static sBool b_debug_fill       = 0;  // 1=show batch draw calls  2=also show individual tri draw calls   3=debug 3d transform
static sBool b_debug_extrude    = 0;  // 2=verbose
static sBool b_debug_line_strip = 0;  // 2=verbose
static sBool b_debug_line       = 0;
static sBool b_debug_tess       = 0;  // 2=verbose
// static sBool b_debug_prims   = 1;  // show #points, #lines and #tris at end of file (moved to test.tks)
static sBool b_allow_line_opt   = 0;  // 1=draw narrow strokes as lines   0=allow tesselate
static sBool b_debug_export     = 1;

static sUI total_num_points      = 0u;
static sUI total_num_lines       = 0u;
static sUI total_num_line_strips = 0u;
static sUI total_num_tris        = 0u;
static sUI total_num_tris_tex    = 0u;
static sUI total_num_rects       = 0u;
static sUI total_num_roundrects  = 0u;
static sUI total_num_ellipses    = 0u;

// Path types
#define MINNIE_PATH_TYPE_DRAW       0  // 0x84: no points / no tesselator (main draw list)
#define MINNIE_PATH_TYPE_CONVEX     1  // 0x84: no tesselator
#define MINNIE_PATH_TYPE_CONCAVE    2  // 0x85: (simple) ear-clipping tesselator
#define MINNIE_PATH_TYPE_EVENODD    3  // 0x87: SGI/VG tesselator (can add holes with sub-paths)
#define MINNIE_PATH_TYPE_IMMEDIATE  4  // 0x8D: specialized render functions (rect, roundrect, ellipse, circle, ..)

// https://www.w3.org/TR/SVG11/painting.html#StrokeLinecapProperty
#define MINNIE_LINEJOIN_NONE   0
#define MINNIE_LINEJOIN_MITER  1
#define MINNIE_LINEJOIN_ROUND  2
#define MINNIE_LINEJOIN_BEVEL  3

#define MINNIE_LINECAP_NONE    0
#define MINNIE_LINECAP_BUTT    1
#define MINNIE_LINECAP_ROUND   2
#define MINNIE_LINECAP_SQUARE  3


typedef union mem32_u {
   sU32 u32;
   sS32 s32;
   sF32 f32;
} mem32_t;

typedef union cmemptr_u {
   const void *any;
   const sU8  *u8;
   const sS8  *s8;
   const sU16 *u16;
   const sS16 *s16;
   const sU32 *u32;
   const sS32 *s32;
   const sF32 *f32;
} cmemptr_t;

typedef union memptr_u {
   void *any;
   sU8  *u8;
   sS8  *s8;
   sU16 *u16;
   sS16 *s16;
   sU32 *u32;
   sS32 *s32;
   sF32 *f32;
} memptr_t;

typedef void *minnie_allocator_handle_t;

#if MINNIE_ALLOC_DEBUG
static sSI g_minnie_alloc_num;
static sSI g_minnie_alloc_cur;
static sSI g_minnie_alloc_max;
static sSI g_minnie_free_num;

static sSI g_vgtesselator_alloc_num;
static sSI g_vgtesselator_alloc_cur;
static sSI g_vgtesselator_alloc_max;
static sSI g_vgtesselator_free_num;
#endif // MINNIE_ALLOC_DEBUG

static sUI debug_num_paths;
static sUI debug_num_swtess_paths;
static sUI debug_num_small_paths;
static sUI debug_num_small_path_points;
static sUI debug_num_large_paths;
static sUI debug_num_large_path_points;
static sUI debug_num_stroke_open_paths;
static sUI debug_num_stroke_open_path_points;
static sUI debug_num_stroke_closed_paths;
static sUI debug_num_stroke_closed_path_points;

#ifndef MINNIE_CUSTOM_ALLOC
void *minnie_alloc(minnie_allocator_handle_t _allocator, sU32 _sz) {
   void *ret = NULL;
   (void)_allocator;

#if MINNIE_ALLOC_DEBUG
   // (note) VG tesselator crashes when memory is not aligned to 16 bytes
   sU32 allocSz = _sz + 4*sizeof(sU32);
   sU32 *hdr = (sU32*)::malloc(allocSz);
   if(NULL != hdr)
   {
      hdr[0] = sUI(g_minnie_alloc_num);
      hdr[1] = _sz;

      g_minnie_alloc_num++;
      g_minnie_alloc_cur += sSI(_sz);
      if(g_minnie_alloc_cur > g_minnie_alloc_max)
      {
         g_minnie_alloc_max = g_minnie_alloc_cur;
      }

      Dallocprintf("[trc] minnie_alloc: serial=%d live=%d size=%u(%3.2fk) cur_total=%3.2fk\n", g_minnie_alloc_num-1, (g_minnie_alloc_num - g_minnie_free_num), _sz, Dkbytes(_sz), Dkbytes(g_minnie_alloc_cur));

      ret = (void*)(hdr + 4);
   }
#else
   ret = ::malloc(_sz);
#endif // MINNIE_ALLOC_DEBUG
   Dallocprintf("[trc] minnie_alloc: sz=%u => ret=%p\n", _sz, ret);
   return ret;
}

void minnie_free(minnie_allocator_handle_t _allocator, void *_ptr) {
   (void)_allocator;
#if MINNIE_ALLOC_DEBUG
   /* Dallocprintf("[trc] minnie_free: ptr=%p\n", _ptr); */
   if(NULL != _ptr)
   {
      sU32 *hdr = ((sU32*)_ptr) - 4;
      g_minnie_free_num++;
      g_minnie_alloc_cur -= (sSI)hdr[1];
      Dallocprintf("[trc] minnie_free: ptr=%p serial=%d sz=%u => new alloc_cur=%d\n", _ptr, hdr[0], hdr[1], g_minnie_alloc_cur);
      ::free((void*)hdr);
   }
#else
   ::free(_ptr);
#endif // MINNIE_ALLOC_DEBUG
   fflush(stdout);
}

void *minnie_realloc(minnie_allocator_handle_t _allocator, void *_ptr, sU32 _sz) {
   Dallocprintf("[trc] minnie_realloc: ptr=%p sz=%u\n", _ptr, _sz);
#if MINNIE_ALLOC_DEBUG
   void *addrNew = minnie_alloc(_allocator, _sz);
   if(NULL != addrNew)
   {
      if(NULL != _ptr)
      {
         const sU32 *hdrOld = ((const sU32*)_ptr) - 4;
         unsigned int copySz = sMIN(hdrOld[1], _sz);
         if(copySz > 0u)
         {
            ::memcpy(addrNew, _ptr, copySz);
         }
         minnie_free(_allocator, _ptr);
      }
   }
   return addrNew;
#else
   return ::realloc(_ptr, _sz);
#endif // MINNIE_ALLOC_DEBUG
}

#endif // MINNIE_CUSTOM_ALLOC


#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus
void *vgtesselator_alloc(unsigned int _sz) {
   void *ret = NULL;
   /* (void)_allocator; */

#if MINNIE_ALLOC_DEBUG
   // (note) / (todo) VG tesselator crashes when memory is not aligned to 16 bytes
   sU32 allocSz = _sz + 4*sizeof(sU32);
   sU32 *hdr = (sU32*)::malloc(allocSz);
   if(NULL != hdr)
   {
      hdr[0] = sUI(g_vgtesselator_alloc_num);
      hdr[1] = _sz;

      g_vgtesselator_alloc_num++;
      g_vgtesselator_alloc_cur += sSI(_sz);
      if(g_vgtesselator_alloc_cur > g_vgtesselator_alloc_max)
      {
         g_vgtesselator_alloc_max = g_vgtesselator_alloc_cur;
      }

      Dallocprintf("[trc] vgtesselator_alloc: serial=%d live=%d size=%u(%3.2fk) cur_total=%3.2fk\n", g_vgtesselator_alloc_num-1, (g_vgtesselator_alloc_num - g_vgtesselator_free_num), _sz, Dkbytes(_sz), Dkbytes(g_vgtesselator_alloc_cur));

      ret = (void*)(hdr + 4);
   }
#else
   ret = ::malloc(_sz);
#endif // MINNIE_ALLOC_DEBUG
   Dallocprintf("[trc] vgtesselator_alloc: sz=%u => ret=%p\n", _sz, ret);
   return ret;
}

void vgtesselator_free(void *_ptr) {
   /* (void)_allocator; */
#if MINNIE_ALLOC_DEBUG
   /* Dallocprintf("[trc] vgtesselator_free: ptr=%p\n", _ptr); */
   if(NULL != _ptr)
   {
      sU32 *hdr = ((sU32*)_ptr) - 4;
      g_vgtesselator_free_num++;
      g_vgtesselator_alloc_cur -= (sSI)hdr[1];
      Dallocprintf("[trc] vgtesselator_free: ptr=%p serial=%d sz=%u => new alloc_cur=%d\n", _ptr, hdr[0], hdr[1], g_vgtesselator_alloc_cur);
      ::free((void*)hdr);
   }
#else
   ::free(_ptr);
#endif // MINNIE_ALLOC_DEBUG
   fflush(stdout);
}

void *vgtesselator_realloc(void *_ptr, unsigned int _sz) {
   Dallocprintf("[trc] vgtesselator_realloc: ptr=%p sz=%u\n", _ptr, _sz);
#if MINNIE_ALLOC_DEBUG
   void *addrNew = vgtesselator_alloc(_sz);
   if(NULL != addrNew)
   {
      if(NULL != _ptr)
      {
         const sU32 *hdrOld = ((const sU32*)_ptr) - 4;
         unsigned int copySz = sMIN(hdrOld[1], _sz);
         if(copySz > 0u)
         {
            ::memcpy(addrNew, _ptr, copySz);
         }
         vgtesselator_free(_ptr);
      }
   }
   return addrNew;
#else
   return ::realloc(_ptr, _sz);
#endif // MINNIE_ALLOC_DEBUG
}

#if defined(__cplusplus)
} // extern "C"
#endif // __cplusplus


void minnie_alloc_debug_print_stats(void) {
#if MINNIE_ALLOC_DEBUG
   Dprintf("[...] minnie_alloc_debug_print_stats: #alloc=%d #free=%d live=%d max=%3.2fk cur=%3.2fk\n", g_minnie_alloc_num, g_minnie_free_num, (g_minnie_alloc_num - g_minnie_free_num), Dkbytes(g_minnie_alloc_max), Dkbytes(g_minnie_alloc_cur));
   Dprintf("[...] vgtesselator_alloc_debug_print_stats: #alloc=%d #free=%d live=%d max=%3.2fk cur=%3.2fk\n", g_vgtesselator_alloc_num, g_vgtesselator_free_num, (g_vgtesselator_alloc_num - g_vgtesselator_free_num), Dkbytes(g_vgtesselator_alloc_max), Dkbytes(g_vgtesselator_alloc_cur));
#endif // MINNIE_ALLOC_DEBUG
}

void minnie_reset_alloc_stats(void) {
#if MINNIE_ALLOC_DEBUG
   if(0 == g_minnie_alloc_cur)
   {
      g_minnie_alloc_num = 0;
      g_minnie_free_num  = 0;
   }

   if(0 == g_vgtesselator_alloc_cur)
   {
      g_vgtesselator_alloc_num = 0;
      g_vgtesselator_free_num  = 0;
   }
#endif // MINNIE_ALLOC_DEBUG
}

sU32 i32rgba8_from_argb32(const sU32 _c32) {
   // little endian:
   //   c32: ARGB32 = b,g,r,a in memory => swap b<=>r => ABGR32 = r,g,b,a in memory
   return
      ( (_c32 & 0xFF00FFFFu) | ((_c32 & 0x000000FFu) << 16) ) |
      ( (_c32 & 0xFFFFFF00u) | ((_c32 & 0x00FF0000u) >> 16) ) ;
   // (todo) big endian
}

static inline sF32 mathLerpf(const sF32 _a, const sF32 _b, const sF32 _t) {
   return _a + (_b - _a) * _t;
}

static inline sSI mathWrapi(sSI _a, sSI _b, sSI _c) {
   if(_a < _b)
      return (_a - _b) + _c;
   else if(_a >= _c)
      return _b + (_a - _c);
   else
      return _a;
}

static sF32 f32_from_f16(sSI _v) {
   mem32_t r; r.u32 = 0;

   // Convert exponent from 5 bits (bias = 15) to 8 bits (bias = 127)
   r.u32 = (0 != _v) ? (sUI( ( (sS8(_v >> 10) & 31) -  15) + 127 ) << 23) : 0;

   // Convert mantissa bits from 10 bits to 23 bits
   r.u32 |= (_v & 1023u) << (23 - 10);

   // Add sign bit
   r.u32 |= (_v >> 15) << 31;

   return r.f32;
}


#if MINNIE_EXPORT_TRIS_EDGEAA
// #define MINNIE_EDGEAA_MAX_VERTICES_PER_PATH  4096
/* #define MINNIE_EDGEAA_MAX_VERTICES_PER_PATH  65536 */
#define MINNIE_EDGEAA_MAX_VERTICES_PER_PATH  131072

struct VertexAA {
   sS32  x;   // float*1024
   sS32  y;   // float*1024
   sU8   r,g,b,a;
   sBool b_edge_bc;  // opposite from A  ('s')   0=AA  1=noAA(shared)
   sBool b_edge_ca;  // opposite from B  ('u')   ..
   sBool b_edge_ab;  // opposite from C  ('t')   ..
   /* sBool b_all_shared;  // valid for first vertex in a triangle. 1=(b_edge_bc && b_edge_ca && b_edge_ab) */
   sF32  scale_s;
   sF32  scale_t;
   sF32  scale_u;
};
#endif // MINNIE_EXPORT_TRIS_EDGEAA


// <class.png>
struct StreamReader {
   cmemptr_t data;
   cmemptr_t data_end;

  public:

   void init(const void *_data, sU32 _sz) {
      data.any = _data;
      data_end.u8 = data.u8 + _sz;
   }

   sS8 getS8(void) {
      return *data.s8++;
   }

   sU8 getU8(void) {
      return *data.u8++;
   }

   sS16 getS16(void) {
      return *data.s16++;
   }

   sU16 getU16(void) {
      return *data.u16++;
   }

   sF32 getF32(void) {
      /* yac_host->printf("xxx getF32: data.u8=%p\n", data.u8); */
#if 0
      union {
         sF32 f32;
         sU8  u8[4];
      } u;
      u.u8[0] = *data.u8++;
      u.u8[1] = *data.u8++;
      u.u8[2] = *data.u8++;
      u.u8[3] = *data.u8++;
      return u.f32;
#else
      return *data.f32++;
#endif
   }

};


// <class.png>
struct StreamReader4 {
   StreamReader *ifs;
   sU8 cur_byte;
   sUI bit_off;

  public:

   void begin(StreamReader *_ifs) {
      ifs      = _ifs;
      cur_byte = 0u;
      bit_off  = 8u;
   }

   sU8 read4(void) {
      if(8u == bit_off)
      {
         cur_byte = ifs->getU8();
         bit_off = 0;
      }
      sU8 r = (cur_byte >> bit_off) & 15u;
      bit_off += 4u;
      return r;
   }

   sU16 read12u() {
      // unsigned
      sU16 r = read4();
      r |= (read4() << 4);
      r |= (read4() << 8);
      return r;
   }

   sS32 read12s() {
      // sign-extend
      mem32_t r; r.u32 = read4();
      r.u32 |= (read4() << 4);
      r.u32 |= (read4() << 8);
      if(r.u32 & 2048u)
         r.u32 |= 0xFFFFF000u;
      return r.s32;
   }

   void end(void) { }

};


// <class.png>
class Framebuffer {
  public:
   minnie_allocator_handle_t allocator;

   sU32     fb_idx;
   sU32     w;
   sU32     h;

#if MINNIE_SW_RENDER
   sU32     pitch;  // bytes per scanline
   memptr_t pixels;
   sBool    b_own_pixels;

   // tri_raster_aa Bitmap
   Bitmap tri_aa_bitmap;
#endif // MINNIE_SW_RENDER

  public:
   Framebuffer(void) {
      allocator    = NULL;
      fb_idx       = 0u;
      w            = 0u;
      h            = 0u;
#if MINNIE_SW_RENDER
      pitch        = 0u;
      pixels.any   = NULL;
      b_own_pixels = YAC_FALSE;
#endif // MINNIE_SW_RENDER
   }

   void free(void) {
#if MINNIE_SW_RENDER
      if(b_own_pixels)
      {
         minnie_free(allocator, pixels.any);
         b_own_pixels = YAC_FALSE;
      }
      pixels.any = NULL;
      pitch = 0u;
#endif // MINNIE_SW_RENDER
      w = h = 0u;
   }

   ~Framebuffer() {
      free();
   }

   sBool init(minnie_allocator_handle_t _allocator, sU32 _fbIdx, sU32 _w, sU32 _h, sU32 _pitch, void *_pixelsOrNull) {
      free();
      allocator = _allocator;
      w = _w;
      h = _h;
#if MINNIE_SW_RENDER
      if(NULL == _pixelsOrNull)
      {
         pitch        = w * sizeof(sU32);
         pixels.any   = minnie_alloc(allocator, sizeof(h * pitch));
         b_own_pixels = (NULL != pixels.any);
         Dinitprintf("[dbg] minnie::Framebuffer::init: alloc sz=(%u,%u) pitch=%u => pixels=%p\n", w, h, pitch, pixels.any);
      }
      else
      {
         pixels.any   = _pixelsOrNull;
         pitch        = _pitch;
         b_own_pixels = YAC_FALSE;
         Dinitprintf("[dbg] minnie::Framebuffer::init: ref sz=(%u,%u) pitch=%u => pixels=%p\n", w, h, pitch, pixels.any);
      }

      // Copy to Bitmap struct for renderTriToBitmap()
      tri_aa_bitmap.s.x = (sS32)w;
      tri_aa_bitmap.s.y = (sS32)h;
      tri_aa_bitmap.data.any = pixels.any;
      tri_aa_bitmap.stride = (sS32)pitch;

      return (NULL != pixels.any);
#else
      return YAC_TRUE;
#endif // MINNIE_SW_RENDER
   }

};


// <class.png>
struct FramebufferOverride {
   memptr_t pixels;
   sU32     w;
   sU32     h;
   sU32     pitch;  // bytes per scanline
};


// <class.png>
class IntArray {
  public:
   minnie_allocator_handle_t allocator;
   memptr_t elements;
   sBool    b_own_elements;
   sU32     num_elements;
   sU32     max_elements;

  public:

   IntArray(void) {
      allocator      = NULL;
      elements.any   = NULL;
      b_own_elements = YAC_FALSE;
      num_elements   = 0u;
      max_elements   = 0u;
   }

   void free(void) {
      if(b_own_elements)
      {
         Dtorprintf("~IntArray this=%p (max=%u)\n", this, max_elements);
         minnie_free(allocator, elements.any);
         b_own_elements = YAC_FALSE;
      }
      elements.any = NULL;
      max_elements = num_elements = 0u;
   }

   ~IntArray() {
      free();
   }

   sBool init(minnie_allocator_handle_t _allocator, sU32 _maxElements, void *_elementsOrNull) {
      free();
      allocator = _allocator;
      num_elements = max_elements = 0u;
      b_own_elements = YAC_FALSE;
      if(NULL == _elementsOrNull)
      {
         elements.any = minnie_alloc(allocator, sizeof(sS32) * _maxElements);
         if(NULL != elements.any)
         {
            b_own_elements = YAC_TRUE;
            max_elements = _maxElements;
         }
      }
      else
      {
         elements.any = _elementsOrNull;
         max_elements = _maxElements;
      }
      return (NULL != elements.any);
   }

   void empty(void) {
      num_elements  = 0u;
   }

   void add(sS32 _i) {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(num_elements >= max_elements)
      {
         Derrorprintf("[---] Minnie::IntArray::add: out of bounds (max=%u)\n", max_elements);
         return;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      elements.s32[num_elements++] = _i;
   }

   void addU32(sU32 _i) {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(num_elements >= max_elements)
      {
         Derrorprintf("[---] Minnie::IntArray::addU32: out of bounds (max=%u)\n", max_elements);
         return;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      elements.u32[num_elements++] = _i;
   }

   void add3(sS32 _a, sS32 _b, sS32 _c) {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(num_elements + 3u > max_elements)
      {
         Derrorprintf("[---] Minnie::IntArray::add3: out of bounds (max=%u)\n", max_elements);
         return;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      elements.s32[num_elements++] = _a;
      elements.s32[num_elements++] = _b;
      elements.s32[num_elements++] = _c;
   }

   sS32 get(sUI _idx) const {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(_idx >= num_elements)
      {
         Derrorprintf("[---] Minnie::IntArray::get: idx %u out of bounds (num=%u)\n", _idx, num_elements);
         return 0;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      return elements.s32[_idx];
   }

   sU32 getU32(sUI _idx) const {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(_idx >= num_elements)
      {
         Derrorprintf("[---] Minnie::IntArray::getU32: idx %u out of bounds (num=%u)\n", _idx, num_elements);
         return 0;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      return elements.u32[_idx];
   }

   void copyFrom(const IntArray *_o) {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(_o->num_elements > max_elements)
      {
         Derrorprintf("[---] Minnie::IntArray::copyFrom: out of bounds (need=%u max=%u)\n", _o->num_elements, max_elements);
         return;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      ::memcpy(elements.any, _o->elements.any, sizeof(sS32) * _o->num_elements);
      num_elements = _o->num_elements;
   }

   sBool alloc(sUI _max) {
      free();
      elements.any = minnie_alloc(allocator, sizeof(sS32) * _max);
      if(NULL != elements.any)
      {
         b_own_elements = YAC_TRUE;
         max_elements = _max;
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   sBool identity(const sUI _num) {
      if(max_elements < _num)
      {
         if(!alloc(_num))
            return YAC_FALSE;
      }
      num_elements = _num;
      for(sUI i = 0u; i < num_elements; i++)
         elements.s32[i] = i;
      return YAC_TRUE;
   }

};


// <class.png>
class FloatArray {
  public:
   minnie_allocator_handle_t allocator;
   memptr_t elements;
   sBool    b_own_elements;
   sU32     num_elements;
   sU32     max_elements;

  public:

   FloatArray(void) {
      allocator      = NULL;
      elements.any   = NULL;
      num_elements   = 0u;
      max_elements   = 0u;
      elements.any   = NULL;
      b_own_elements = YAC_FALSE;
   }

   void free(void) {
      if(b_own_elements)
      {
         Dtorprintf("~FloatArray this=%p (elements=%p max=%u)\n", this, elements.any, max_elements);
         minnie_free(allocator, elements.any);
         b_own_elements = YAC_FALSE;
      }
      elements.any = NULL;
      max_elements = num_elements = 0u;
   }

   ~FloatArray() {
      free();
   }

   sBool init(minnie_allocator_handle_t _allocator, sU32 _maxElements, void *_elementsOrNull) {
      free();
      allocator = _allocator;
      num_elements = max_elements = 0u;
      b_own_elements = YAC_FALSE;
      if(NULL == _elementsOrNull)
      {
         elements.any = minnie_alloc(allocator, sizeof(sF32) * _maxElements);
         Dinitprintf("xxx FloatArray::init: this=%p max=%u => alloc elements=%p\n", this, _maxElements, elements.any);
         if(NULL != elements.any)
         {
            b_own_elements = YAC_TRUE;
            max_elements = _maxElements;
         }
      }
      else
      {
         elements.any = _elementsOrNull;
         max_elements = _maxElements;
         Dinitprintf("xxx FloatArray::init: this=%p max=%u => ref elements=%p\n", this, _maxElements, elements.any);
      }
      return (NULL != elements.any);
   }

   void empty(void) {
      num_elements  = 0u;
   }

   sBool isEmpty(void) {
      return (0u == num_elements);
   }

   sBool resize(sUI _num) {
      if(b_own_elements)
      {
         sF32 *ne = (sF32*)minnie_alloc(allocator, sizeof(sF32) * _num);
         if(NULL != ne)
         {
            sUI copyNum = sMIN(num_elements, _num);
            ::memcpy((void*)ne, elements.any, sizeof(sF32) * copyNum);
            minnie_free(allocator, elements.any);
            elements.f32 = ne;
            return YAC_TRUE;
         }
         return YAC_FALSE;
      }
      return YAC_FALSE;
   }

   void add2(sF32 _x, sF32 _y) {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(num_elements + 2u > max_elements)
      {
         if(!resize(Dgrowarray(num_elements + 2u)))
         {
            Derrorprintf("[---] Minnie::FloatArray::add2: out of bounds (max=%u)\n", max_elements);
            return;
         }
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      elements.f32[num_elements++] = _x;
      elements.f32[num_elements++] = _y;
   }

   void add2ui(sUI _x, sUI _y) {
      add2((sF32)_x, (sF32)_y);
   }

   void add3(sF32 _x, sF32 _y, sF32 _z) {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(num_elements + 3u > max_elements)
      {
         if(!resize(Dgrowarray(num_elements + 3u)))
         {
            Derrorprintf("[---] Minnie::FloatArray::add3: out of bounds (max=%u)\n", max_elements);
            return;
         }
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      elements.f32[num_elements++] = _x;
      elements.f32[num_elements++] = _y;
      elements.f32[num_elements++] = _z;
   }

   sF32 get(sUI _idx) const {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(_idx >= num_elements)
      {
         Derrorprintf("[---] Minnie::FloatArray::get: out of bounds (num=%u)\n", num_elements);
         return 0.0f;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      return elements.f32[_idx];
   }

   sF32 getRev(sUI _idx) const {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(_idx >= num_elements)
      {
         Derrorprintf("[---] Minnie::FloatArray::getRev: revIdx=%u out of bounds (num=%u)\n", _idx, num_elements);
         return 0.0f;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      return elements.f32[num_elements - 1u - _idx];
   }

   void copyFrom(const FloatArray *_o) {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(_o->num_elements > max_elements)
      {
         Derrorprintf("[---] Minnie::FloatArray::copyFrom: out of bounds (need=%u max=%u)\n", _o->num_elements, max_elements);
         return;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      ::memcpy(elements.any, _o->elements.any, sizeof(sF32) * _o->num_elements);
      num_elements = _o->num_elements;
   }

   void debugPrint2(const char *_msg) const {
      Dprintf("%s",_msg);
      for(sUI i = 0u; (i+1u) < num_elements; i+=2u)
      {
         if(i > 0u)
         {
            Dprintf(", ");
         }
         Dprintf("(%f;%f)", elements.f32[i], elements.f32[i+1]);
      }
      Dprintf("\n");
   }

};


// <class.png>
template <class T> class PointerArray {
  public:
   minnie_allocator_handle_t allocator;
   T **elements;
   sBool b_own_elements;
   sU32  num_elements;
   sU32  max_elements;

  public:

   PointerArray(void) {
      allocator      = NULL;
      elements       = NULL;
      num_elements   = 0u;
      max_elements   = 0u;
      b_own_elements = YAC_FALSE;
   }

   void free(void) {
      if(b_own_elements)
      {
         Dtorprintf("~PointerArray this=%p (num=%u max=%u)\n", this, num_elements, max_elements);
      }
      for(sUI i = 0u; i < num_elements; i++)
      {
         if(NULL != elements[i])
         {
            /* Dprintf("xxx PointerArray: begin destruct element %u\n", i); */
            elements[i]->~T();
            /* Dprintf("xxx PointerArray: end destruct element %u\n", i); */
            minnie_free(allocator, (void*)elements[i]);
            /* Dprintf("xxx PointerArray: element %u freed\n", i); */
            elements[i] = NULL;
         }
      }
      if(b_own_elements)
      {
         minnie_free(allocator, (void*)elements);
         b_own_elements = YAC_FALSE;
      }
      elements = NULL;
      max_elements = num_elements = 0u;
   }

   ~PointerArray() {
      free();
   }

   sBool init(minnie_allocator_handle_t _allocator, sU32 _maxElements, void *_elementsOrNull) {
      free();
      allocator = _allocator;
      num_elements = max_elements = 0u;
      b_own_elements = YAC_FALSE;
      if(NULL == _elementsOrNull)
      {
         const sUI allocSz = sizeof(T*) * _maxElements;
         elements = (T**)minnie_alloc(allocator, allocSz);
         Dallocprintf("[>>>] alloc PointerArray max=%u (%u bytes) => elements=%p\n", _maxElements, allocSz, elements);
         if(NULL != elements)
         {
            b_own_elements = YAC_TRUE;
            max_elements = _maxElements;
         }
      }
      else
      {
         elements = (T**)_elementsOrNull;
         max_elements = _maxElements;
      }
      Dinitprintf("xxx Minnie::PointerArray::init: LEAVE this=%p num=%u max=%u\n", this, num_elements, max_elements);
      return (NULL != elements);
   }

   void empty(void) {
      num_elements  = 0u;
   }

   T *addNew(void) {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      // Dprintf("xxx Minnie::PointerArray::addNew: this=%p num=%u max=%u\n", this, num_elements, max_elements);
      if(num_elements >= max_elements)
      {
         Derrorprintf("[---] Minnie::PointerArray::addNew: out of bounds (max=%u)\n", max_elements);
         return NULL;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      T *o = (T*)minnie_alloc(allocator, sizeof(T));
      // Dprintf("xxx Minnie::PointerArray::addNew: this=%p => o=%p (num is now %u)\n", this, o, (num_elements+1u));
      new(o)T();
      elements[num_elements++] = o;
      return o;
   }

   T *get(sUI _idx) const {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(_idx >= num_elements)
      {
         Derrorprintf("[---] Minnie::PointerArray::get: idx=%u out of bounds (num=%u)\n", _idx, num_elements);
         return NULL;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      return elements[_idx];
   }

   T *getRev(sUI _idx) const {
#if MINNIE_ARRAY_BOUNDARY_CHECK
      if(_idx >= num_elements)
      {
         Derrorprintf("[---] Minnie::PointerArray::getRev: revIdx=%u out of bounds (num=%u)\n", _idx, num_elements);
         return NULL;
      }
#endif // MINNIE_ARRAY_BOUNDARY_CHECK
      return elements[num_elements - 1u - _idx];
   }

};

// <class.png>
struct Vector3f {
  public:
   sF32 x, y, z;

   void init(sF32 _x, sF32 _y, sF32 _z) {
      x = _x;
      y = _y;
      z = _z;
   }

   void subFrom(const Vector3f *_a, const Vector3f *_b) {
      x = _a->x - _b->x;
      y = _a->y - _b->y;
      z = _a->z - _b->z;
   }

   inline sF32 dot(const Vector3f *_o) const {
      return x*_o->x + y*_o->y + z*_o->z;
   }

   static inline sF32 Dot(const Vector3f *_a, const Vector3f *_b) {
      return _a->x * _b->x + _a->y * _b->y + _a->z * _b->z ;
   }

   sF32 getAbs(void) const {
      return ::sqrtf(x*x + y*y + z*z);
   }

   void diffUnit(const Vector3f *_a, const Vector3f *_b) {
      // b - a
      x = _b->x - _a->x;
      y = _b->y - _a->y;
      z = _b->z - _a->z;

      // calc length
      sF32 l =  ::sqrtf(x * x + y * y + z * z);

      // normalize
      if(0.0f != l)
      {
         l = 1.0f / l;
         x *= l;
         y *= l;
         z *= l;
      }
      else
      {
         x = 0.0f;
         y = 0.0f;
         z = 0.0f;
      }
   }

   sF32 distanceToPlane(const Vector3f *_q, const Vector3f *_n) const {
      Vector3f v;
      v.subFrom(this, _q);
      const sF32 nAbs = _n->getAbs();
      return (0.0f != nAbs) ? (v.dot(_n) / nAbs) : 999999999.0f;
   }

   sBool intersectPlane(const Vector3f *_q, const Vector3f *_n, const Vector3f *_p1, const Vector3f *_p2) {
      Vector3f pDir; pDir.diffUnit(_p1, _p2);  // pDir = normalize(p2 - p1)

      const sF32 dotNQ = Dot(_n, _q);
      const sF32 dotN1 = Dot(_n, _p1);
      const sF32 dotND = Dot(_n, &pDir);

      if(0.0f != dotND)
      {
         const sF32 t = (dotNQ - dotN1) / dotND;
         x = _p1->x + pDir.x * t;
         y = _p1->y + pDir.y * t;
         z = _p1->z + pDir.z * t;
         return YAC_TRUE;
      }

      // colinear
      return YAC_FALSE;
   }

   void divzTo(Vector2f *_v2) {
      if(z > 0.0f)
      {
         _v2->x = x / z;
         _v2->y = y / z;
      }
      else
      {
         _v2->x = 0.0f;
         _v2->y = 0.0f;
      }
   }

};


// <class.png>
struct Matrix2x3f {
   // row major
  public:
   sF32 floats[2*3];

   void initIdentity(void) {
      TM2x3F(0, 0) = 1.0f;  TM2x3F(0, 1) = 0.0f;  TM2x3F(0, 2) = 0.0f;
      TM2x3F(1, 0) = 0.0f;  TM2x3F(1, 1) = 1.0f;  TM2x3F(1, 2) = 0.0f;
   }

   void mulv(Vector2f *_v) const {
      sF32 t[2]; t[0] = _v->x; t[1] = _v->y;
      _v->x = floats[0]*t[0] + floats[1]*t[1] + floats[2];
      _v->y = floats[3]*t[0] + floats[4]*t[1] + floats[5];
   }

   void copyFrom(const Matrix2x3f *_o) {
      ::memcpy(floats, _o->floats, sizeof(floats));
   }

};



// <class.png>
class ShapePolyline {

  protected:
   const FloatArray *line_vertices;
   FloatArray *v_tri;  // triangle mesh vertices (out)

   sF32  const_line_width;
   sBool b_line_closed;
   sF32  line_miter_limit;           // [def=32.0]
#define LINE_BEVEL_FB_DIST  4.0f
   sUI   line_joint_type;            // 1=miter, 2=round, 3=bevel  [def=bevel]
   sUI   line_cap_type;              // 1=butt, 2=round, 3=square  [def=butt]
   sUI   num_div_round_line_joint;   // [def=11]
   sU16  line_pattern;               // [def=#ffff(solid)]
#define LINE_VIS_MIN_ANG 0.2f        // [def=0.2]


  public:

   void setLineVerticesRef(FloatArray *_a) {
      line_vertices = _a;
   }

   void setTriVerticesRef(FloatArray *_a) {
      v_tri = _a;
   }

   void setLineWidth(sF32 _w) {
      // (note) the total effective line width is (w * 2)
      // Dprintf("xxx setLineWidth w=%f\n", _w);
      const_line_width = _w;
   }

   void setEnableClosed(sBool _bEnable) {
      b_line_closed = _bEnable;
   }

   void setLineJointType(sUI _type) {
      /* Dprintf("xxx setLineJointType(%u)\n", _type); */
      line_joint_type = _type;
   }

   void setLineCapType(sUI _type) {
      line_cap_type = _type;
   }

   void setMiterLimit(sF32 _f) {
      line_miter_limit = _f;
   }

   void setNumDivRoundLineJoint(sUI _num) {
      num_div_round_line_joint = _num;
   }

   void setLinePattern (sU16 _pattern) {
      line_pattern = _pattern;
   }

  protected:
   void addTri(const Vector2f *_vLT, const Vector2f *_vRT, const Vector2f *_vRB) {
      v_tri->add2(_vLT->x, _vLT->y);
      v_tri->add2(_vRT->x, _vRT->y);
      v_tri->add2(_vRB->x, _vRB->y);
   }

   void addQuad(const Vector2f *_vLT, const Vector2f *_vRT, const Vector2f *_vRB, const Vector2f *_vLB) {
      v_tri->add2(_vLT->x, _vLT->y);
      v_tri->add2(_vRT->x, _vRT->y);
      v_tri->add2(_vRB->x, _vRB->y);

      v_tri->add2(_vLT->x, _vLT->y);
      v_tri->add2(_vRB->x, _vRB->y);
      v_tri->add2(_vLB->x, _vLB->y);
   }

   void addRoundLineJoint_bezier(const Vector2f *_vCtr,
                                 const Vector2f *_vL,
                                 const Vector2f *_vMiter,
                                 const Vector2f *_vR,
                                 const sUI _numDiv
                                 ) {

      sF32 aStep = 1.0f / (_numDiv - 1u);
      sF32 a = aStep;

      Vector2f vPrev; vPrev = _vL;
      Vector2f vCur;
      Vector2f vCtr; vCtr = _vCtr;

      Vector2f vDirL; vDirL.subFrom(_vMiter, _vL);
      Vector2f vDirR; vDirR.subFrom(_vMiter, _vR);
      Vector2f vDir;
      Vector2f vMidL;
      Vector2f vMidR;

      for(sUI divIdx = 0u; divIdx < _numDiv - 1u; divIdx++)
      {
         vMidL.mulfFrom(&vDirL, a);
         vMidR.mulfFrom(&vDirR, 1.0f - a);
         vMidL.add(_vL);
         vMidR.add(_vR);
         vDir.subFrom(&vMidR, &vMidL);
         vCur.mulfFrom(&vDir, a);
         vCur.add(&vMidL);

         addTri(&vCtr, &vPrev, &vCur);

         // Next vertex
         vPrev = vCur;
         a += aStep;
      }
   }

   void addRoundLineCap_bezier(const Vector2f *_vL, const Vector2f *_vMiter, const Vector2f *_vR,
                               const sUI _numDiv
                               ) {

      const sF32 aStep = 1.0f / (_numDiv - 1u);
      sF32 a = aStep;

      Vector2f vPrev; vPrev = _vL;
      Vector2f vCur;

      Vector2f vDirL; vDirL.subFrom(_vMiter, _vL);
      Vector2f vDirR; vDirR.subFrom(_vMiter, _vR);
      Vector2f vDir;
      Vector2f vMidL;
      Vector2f vMidR;

      for(sUI divIdx = 0u; divIdx < _numDiv - 1u; divIdx++)
      {
         vMidL.mulfFrom(&vDirL, a);
         vMidR.mulfFrom(&vDirR, 1.0f - a);
         vMidL.add(_vL);
         vMidR.add(_vR);
         vDir.subFrom(&vMidR, &vMidL);
         vCur.mulfFrom(&vDir, a);
         vCur.add(&vMidL);

         addTri(&vPrev, &vCur, _vR);

         // Next vertex
         vPrev = vCur;
         a += aStep;
      }
   }

   // (todo) remove pattern ?
   void tesselatePolyline(const FloatArray *_vertices,
                          const sBool       _bClosed,
                          const sU16        _pattern
                          ) {
      /* allocTriVertices(_vertices.numElements * (12/2)); */
      /* queueVBOUpdate(); */

      const sUI numVertices = _vertices->num_elements / 2u;

      Vector2f vStart;
      Vector2f vStartLast;
      /* sF32 wStart; */
      /* sF32 wStartLast; */

      Vector2f vEnd;
      /* sF32 wEnd; */
      /* sF32 wFirst; */

      /* sF32 wStartLastSclL; */
      /* sF32 wStartLastSclR; */
      /* sF32 wEndLastSclL; */
      /* sF32 wEndLastSclR; */
      /* sF32 wStartSclL; */
      /* sF32 wStartSclR; */
      /* sF32 wEndSclL; */
      /* sF32 wEndSclR; */
      /* sF32 wEndSclLFirst; */
      /* sF32 wEndSclRFirst; */

      Vector2f vTmp;
      Vector2f vTmp2;

      Vector2f nLine;

      Vector2f vStartL;
      Vector2f vStartR;
      Vector2f vEndL;
      Vector2f vEndR;

      Vector2f vStartLastL;
      Vector2f vStartLastR;
      Vector2f vEndLastL;
      Vector2f vEndLastR;

      Vector2f vDirLast;
      int skipQuad = 0;
      /* int segIdx = 0; */

      if(0 == _pattern)
         return;
      sU16 patternBit = _bClosed ? (1 << 0) : (1 << (15-(numVertices & 15)));
      /* sBool bDrawQuadPrev = YAC_FALSE; */

      if(_bClosed)
      {
         skipQuad = 1;
         vStart.init(_vertices->get( (numVertices-1u) * 2u +0u),
                     _vertices->get( (numVertices-1u) * 2u +1u)
                     );
         /* wStart = const_line_width; */
      }
      else
      {
         vStart.init(_vertices->get(0),
                     _vertices->get(1)
                     );
         /* wStart = const_line_width; */
      }

      /* wFirst = wStart; */


      /* /\* float wStartSclLFirst = wStartSclL; *\/ */
      /* /\* float wStartSclRFirst = wStartSclR; *\/ */

      // if(null != left_widths)
      // {
      //    trace "xxx wFirst="+wFirst+" const_line_width="+const_line_width+" factor="+((wFirst / const_line_width))+" shape_left.const_line_width="+shape_left.const_line_width;
      //    left_widths.add(mathMaxf(0.5f, (wFirst / const_line_width) * shape_left.const_line_width));
      // }

      // if(null != right_widths)
      //    right_widths.add(mathMaxf(0.5f, (wFirst / const_line_width) * shape_right.const_line_width));

      if(2u == numVertices)
      {
         // (todo) special case: single segment polyline
      }

      /* int aaEdgeFlags; */

      sUI vertexIdx = _bClosed ? 0u : 1u;  // current line segment end vertex
      sUI vertexIterIdx = 0u;

      Vector2f vISFirst;
      /* sF32 wISSclFirst; */

      sBool bFirstQuad = YAC_TRUE;
      Vector2f vCapCtr;
      Vector2f vCapOuter;

      const sBool bMiter = (MINNIE_LINEJOIN_MITER == line_joint_type);
      const sBool bRound = (MINNIE_LINEJOIN_ROUND == line_joint_type);

      for(sUI loopVtxIdx = 0u; loopVtxIdx < numVertices + (_bClosed*2); loopVtxIdx++)
      {
         vEnd.init(_vertices->get(vertexIdx*2u + 0u),
                   _vertices->get(vertexIdx*2u + 1u)
                   );

         /* wEnd = const_line_width; */

         // Line start / end normals scaled to line start / end width
         nLine.init(vStart.y - vEnd.y, vEnd.x - vStart.x);
         nLine.unitScale(const_line_width);
         // nLine.unitScale2f(const_line_width*1.5f, const_line_width*0.5f);

         // Extrude start and end
         vStartL.init(vStart.x - nLine.x, vStart.y - nLine.y);
         vStartR.init(vStart.x + nLine.x, vStart.y + nLine.y);
         vEndL.init(vEnd.x - nLine.x, vEnd.y - nLine.y);
         vEndR.init(vEnd.x + nLine.x, vEnd.y + nLine.y);

         Vector2f vDir; vDir.init(vEnd.x - vStart.x, vEnd.y - vStart.y);

         if(0u == vertexIdx)
            patternBit = 1u;

         sBool bDrawQuad = (0u != (_pattern & patternBit));
         sU16 patternBitNext;
         if( (vertexIdx+1u) == numVertices )
            patternBitNext = 1u;
         else
            patternBitNext = mathWrapi(patternBit << 1, 0, 65535);
         sBool bDrawQuadNext = (0u != (_pattern & patternBitNext));
         sBool bAllowJoint = bDrawQuad && bDrawQuadNext;

         if(vertexIterIdx > 0u)
         {
            sBool bDrawSeg = bDrawQuad && !skipQuad && (vertexIterIdx >= (0u + _bClosed));

            // Calc winding
            sBool bLastVertex  = (vertexIterIdx == (numVertices - 1u + _bClosed*2u));
            if(!bLastVertex || _bClosed)
            {
               Vector2f vIS;
               Vector2f vLT;
               Vector2f vRT;
               /* float    wISScl; */

               Vector2f vMiterOrig;
               Vector2f vMiter;
               Vector2f vMiterL;
               Vector2f vMiterR;
               Vector2f vMiterStart;
               Vector2f vMiterEnd;
               /* float    wMiterScl; */
               /* float    wMiterSclL; */
               /* float    wMiterSclR; */

               Vector2f vLTDraw;
               Vector2f vRTDraw;
               Vector2f vISDraw;

               int winding = int( (vDirLast.x*vDir.y - vDirLast.y * vDir.x) * 16.0 );

               sF32 t;
               sF32 h;

               sBool bBevel = (MINNIE_LINEJOIN_BEVEL == line_joint_type);

               if(winding > 0)
               {
                  // Right turn joint

                  // Calc intersection between vStartLastR<>vEndLastR and vStartR<>vEndR
                  if(1 && _bClosed && (vertexIterIdx == numVertices+1))
                  {
                     /* if(b_debug) */
                     /*    trace "xxx <right> load vIS="+vIS+" vertexIdx="+vertexIdx; */
                     vIS = vISFirst;
                     /* wISScl = wISSclFirst; */
                  }
                  else
                  {
                     h = 0.0f;
                     vTmp.subFrom(&vStartR, &vEndLastR);
                     if(vTmp.getAbs() < 2.0f)
                     {
                        vIS.addFrom(&vStartR, &vEndLastR);
                        vIS.mulf(0.5f);
                        /* wISScl = wEndLastSclR;  // [02Apr2024] */
                        h = -999999.0f;
                     }
                     else
                     {
                        vTmp.subFrom(&vEndLastR, &vStartLastR);
                        vTmp2.subFrom(&vEndR, &vStartR);
                        vTmp.unit();
                        vTmp2.unit();
                        t = (vTmp.x*vTmp2.y - vTmp.y*vTmp2.x);

                        if(sABS(t) < LINE_VIS_MIN_ANG)
                        {
                           vIS.addFrom(&vStartR, &vEndLastR);
                           vIS.mulf(0.5f);
                           /* wISScl = wEndLastSclR;  // [02Apr2024] */
                           h = -999999.0f;
                        }

                        // trace "xxx vTmp="+vTmp+" vTmp2="+vTmp2+" vTmp X vTmp2="+t+" => h="+h;
                     }

                     if(h > -999999.0f)
                     {
                        h = vIS.intersect(&vStartLastR, &vEndLastR,
                                          &vStartR, &vEndR,
                                          YAC_FALSE/*bExtrapolate*/
                                          );
                        /* if!(0.0f <= h <= 1.0) */
                        if(h < 0.0f || h > 1.0f)
                        {
                           vIS = vStartR;
                           /* wISScl = wStartSclR; */
                        }
                        else
                        {
                           /* wISScl = mathLerpf(wStartLastSclR, wEndLastSclR, h); */
                        }
                     }
                  }

                  // if(b_debug)
                  //    trace "xxx <right> vIS="+vIS+" h="+h;

                  vEndLastR = vIS;
                  vStartR   = vIS;

                  /* wEndLastSclR = wISScl; */
                  /* wStartSclR   = wISScl; */

                  // trace "xxx vertexIdx="+vertexIdx+" vertexIterIdx="+vertexIterIdx+" vIS="+vIS;

                  if(_bClosed && (vertexIterIdx == 1u))
                  {
                     /* if(b_debug) */
                     /*    trace "xxx save vIS="+vIS+" vertexIdx="+vertexIdx; */
                     vISFirst = vIS;
                     /* wISSclFirst = wISScl; */
                  }

                  /* if(b_debug) */
                  /*    trace "xxx <right> h="+h+" vIS="+vIS+"  vIS_int=("+int(vIS.x)+";"+int(vIS.y)+")"; */

                  // Fall back to Bevel joints ?
                  vTmp.subFrom(&vStartL, &vEndLastL);
                  if(bMiter)
                     bBevel |= (vTmp.getAbs() <= LINE_BEVEL_FB_DIST);

                  ////if(!bBevel)
                  if(bMiter || bRound)
                  {
                     // Calc miter intersection (right turn)
                     h = vMiterOrig.intersect(&vStartLastL, &vEndLastL,
                                              &vEndL, &vStartL,
                                              YAC_TRUE/*bExtrapolate*/
                                              );

                     vTmp.subFrom(&vMiterOrig, &vStart);

                     /* wMiterScl = mathLerpf(wStartLastSclL, wEndLastSclL, h); */
                     /* wMiterSclL = wMiterScl; */
                     /* wMiterSclR = wMiterScl; */

                     /* if(b_debug) */
                     /*    trace "xxx miterLen="+vTmp.abs+" line_miter_limit="+line_miter_limit+" h="+h; */

                     if(vTmp.getAbs() > line_miter_limit)
                     {
                        vMiter.unitScaleFrom(&vTmp, line_miter_limit);
                        vMiter.add(&vStart);

                        vTmp.subFrom(&vMiterOrig, &vEndLastL);

                        vMiterStart.subFrom(&vEndLastL, &vEndLastL); // (todo) 0!??
                        vMiterStart.add(&vMiter);
                        vMiterEnd.subFrom(&vStartL, &vEndLastL);
                        vMiterEnd.add(&vMiter);

                        vMiterL.intersect(&vEndLastL, &vMiterOrig, &vMiterStart, &vMiterEnd,   YAC_TRUE/*bExtrapolate*/);
                        vMiterR.intersect(&vStartL,   &vMiterOrig, &vMiterEnd,   &vMiterStart, YAC_TRUE/*bExtrapolate*/);

                        /* if(b_debug) */
                        /*    trace "xxx vMiterOrig="+vMiterOrig+" vMiter="+vMiter+" vMiterStart="+vMiterStart+" vMiterEnd="+vMiterEnd+" vMiterL="+vMiterL+" vMiterR="+vMiterR; */
                     }
                     else
                     {
                        vMiter = vMiterOrig;
                        vMiterL = vMiter;
                        vMiterR = vMiter;

                        /* wMiterSclL = wMiterScl; */
                        /* wMiterSclR = wMiterScl; */
                     }


                     // Draw line joint (right turn)
                     if( (vertexIterIdx >= (1/*+_bClosed*/)))
                     {
                        if(MINNIE_LINEJOIN_MITER == line_joint_type)
                        {
                           if(vMiter == vMiterOrig)
                           {
                              vEndLastL = vMiter;
                              vStartL   = vMiter;

                              /* wEndLastSclL = wMiterScl; */
                              /* wStartSclL   = wMiterScl; */
                           }
                           else
                           {
                              vEndLastL = vMiterL;
                              vStartL   = vMiterR;

                              /* wEndLastSclL = wMiterSclL; */
                              /* wStartSclL   = wMiterSclR; */

                              bBevel = YAC_TRUE;
                           }
                        }
                     }

                     if(bAllowJoint && (vertexIterIdx >= (1u+_bClosed)))
                     {
                        if(MINNIE_LINEJOIN_ROUND == line_joint_type)
                        {
                           addRoundLineJoint_bezier(&vIS, &vEndLastL, &vMiter, &vStartL,
                                                    num_div_round_line_joint
                                                    );
                        }
                     }

                  } // if !bBevel

                  if(bBevel && bAllowJoint && (vertexIterIdx >= (1u+_bClosed)))
                  {
                     // bevel line joint (right turn)

                     vLT = vEndLastL;
                     vRT = vStartL;

                     // trace "xxx bevelLineJoint: bottom vRB="+vRB+" vAA3="+vAA3+" nAA3="+nAA3+" ilAA3="+ilAA3;

                     if(1)
                     {
                        vLTDraw = vLT;
                        vRTDraw = vRT;
                        vISDraw = vIS;

                        addTri(&vLTDraw, &vRTDraw, &vISDraw);
                     }
                  } // if bBevel

               }
               else if(winding < 0)
               {
                  // Left turn joint
                  /* if(b_debug) */
                  /*    trace "xxx left turn joint"; */

                  // Calc intersection between vStartLastL<>vEndLastL and vStartL<>vEndL
                  if(_bClosed && (vertexIterIdx == numVertices+1u))
                  {
                     /* if(b_debug) */
                     /*    trace "xxx <left> load vIS="+vIS+" vertexIdx="+vertexIdx; */
                     vIS = vISFirst;
                     /* wISScl = wISSclFirst; */
                  }
                  else
                  {
                     h = 0.0f;
                     vTmp.subFrom(&vStartL, &vEndLastL);
                     if(vTmp.getAbs() < 2.0f)
                     {
                        vIS.addFrom(&vStartL, &vEndLastL);
                        vIS.mulf(0.5f);
                        /* wISScl = wEndLastSclL;  // [02Apr2024] */
                        h = -999999.0f;
                     }
                     else
                     {
                        vTmp.subFrom(&vEndLastL, &vStartLastL);
                        vTmp2.subFrom(&vEndL, &vStartL);
                        vTmp.unit();
                        vTmp2.unit();
                        t = (vTmp.x*vTmp2.y - vTmp.y*vTmp2.x);

                        if(sABS(t) < LINE_VIS_MIN_ANG)
                        {
                           vIS.addFrom(&vStartL, &vEndLastL);
                           vIS.mulf(0.5f);
                           h = -999999.0f;
                           /* wISScl = wEndLastSclL;  // [02Apr2024] */
                        }

                        // trace "xxx vTmp="+vTmp+" vTmp2="+vTmp2+" vTmp X vTmp2="+t+" => h="+h;
                     }

                     // trace "xxx h="+h;
                     if(h > -999999.0f)
                     {
                        h = vIS.intersect(&vStartLastL, &vEndLastL,
                                          &vEndL, &vStartL,
                                          YAC_FALSE/*bExtrapolate*/
                                          );
                        /* if!(0.0f <= h <= 1.0) */
                        if(h < 0.0f || h > 1.0f)
                        {
                           vIS = vStartL;
                           /* wISScl = wStartSclL; */
                        }
                        else
                        {
                           /* wISScl = mathLerpf(wStartLastSclL, wEndLastSclL, h); */
                        }
                     }

                  }

                  vEndLastL = vIS;
                  vStartL   = vIS;

                  /* wEndLastSclL = wISScl; */
                  /* wStartSclL   = wISScl; */

                  if(_bClosed && (vertexIterIdx == 1u))
                  {
                     /* if(b_debug) */
                     /*    trace "xxx save vIS="+vIS+" vertexIdx="+vertexIdx; */
                     vISFirst = vIS;
                     /* wISSclFirst = wISScl; */
                  }

                  /* if(b_debug) */
                  /*    trace "xxx <left> vIS="+vIS; */

                  // Fall back to Bevel joints ?
                  vTmp.subFrom(&vStartR, &vEndLastR);
                  if(bMiter)
                     bBevel |= (vTmp.getAbs() < LINE_BEVEL_FB_DIST);

                  /* if(b_debug) */
                  /*    trace "xxx <left> bBevel="+bBevel+" vTmp="+vTmp+" vTmp.abs="+vTmp.abs+" vEndLastR="+vEndLastR+" vStartR="+vStartR+" vStartLast="+vStartLast+" vStart="+vStart+" vEnd="+vEnd; */

                  // Dprintf("xxx bBevel=%d bMiter=%d\n", bBevel, bMiter);

                  // // if(!bBevel)
                  if(bMiter || bRound)
                  {
                     // Calc miter intersection (left turn)
                     h = vMiterOrig.intersect(&vStartLastR, &vEndLastR,
                                              &vEndR, &vStartR,
                                              YAC_TRUE/*bExtrapolate*/
                                              );

                     vTmp.subFrom(&vMiterOrig, &vStart);

                     /* wMiterScl = mathLerpf(wStartLastSclR, wEndLastSclR, h); */
                     /* wMiterSclL = wMiterScl; */
                     /* wMiterSclR = wMiterScl; */

                     /* if(b_debug) */
                     /*    trace "xxx miterLen="+vTmp.abs+" line_miter_limit="+line_miter_limit; */

                     if(vTmp.getAbs() > line_miter_limit)
                     {
                        vMiter.unitScaleFrom(&vTmp, line_miter_limit);
                        vMiter.add(&vStart);

                        vTmp.subFrom(&vMiterOrig, &vEndLastR);

                        vMiterStart.subFrom(&vEndLastR, &vEndLastR);  // (todo) 0!??
                        vMiterStart.add(&vMiter);
                        vMiterEnd.subFrom(&vStartR, &vEndLastR);
                        vMiterEnd.add(&vMiter);

                        vMiterR.intersect(&vEndLastR, &vMiterOrig, &vMiterStart, &vMiterEnd,   YAC_TRUE/*bExtrapolate*/);
                        vMiterL.intersect(&vStartR,   &vMiterOrig, &vMiterEnd,   &vMiterStart, YAC_TRUE/*bExtrapolate*/);

                        /* if(b_debug) */
                        /*    trace "xxx vMiterOrig="+vMiterOrig+" vMiter="+vMiter+" vMiterStart="+vMiterStart+" vMiterEnd="+vMiterEnd+" vMiterL="+vMiterL+" vMiterR="+vMiterR; */
                     }
                     else
                     {
                        vMiter = vMiterOrig;
                        vMiterL = vMiter;
                        vMiterR = vMiter;

                        /* wMiterSclL = wMiterScl; */
                        /* wMiterSclR = wMiterScl; */
                     }

                     // Draw line joint (left turn)
                     if(bAllowJoint && (vertexIterIdx >= (1u/*+_bClosed*/)))
                     {
                        if(MINNIE_LINEJOIN_MITER == line_joint_type)
                        {
                           if(vMiter == vMiterOrig)
                           {
                              vEndLastR = vMiter;
                              vStartR   = vMiter;

                              /* wEndLastSclR = wMiterScl; */
                              /* wStartSclR   = wMiterScl; */
                           }
                           else
                           {
                              vEndLastR = vMiterR;
                              vStartR   = vMiterL;

                              /* wEndLastSclR = wMiterSclR; */
                              /* wStartSclR   = wMiterSclL; */

                              bBevel = YAC_TRUE;
                           }
                        }
                     }

                     if(bAllowJoint && (vertexIterIdx >= (1u+_bClosed)))
                     {
                        if(MINNIE_LINEJOIN_ROUND == line_joint_type)
                        {
                           addRoundLineJoint_bezier(&vIS, &vEndLastR, &vMiter, &vStartR,
                                                    num_div_round_line_joint
                                                    );
                        }
                     }
                  } // if !bBevel

                  if(bBevel && bAllowJoint && (vertexIterIdx >= (1u+_bClosed)))
                  {
                     // bevel line joint (left turn)

                     // top-side AA edge:
                     vLT = vStartR;
                     vRT = vEndLastR;

                     // // // bottom-side AA edge: shift top-side AA edge to vIS edge
                     // // vLB = vStartR;   vLB.sub(vEndLastR); vLB.add(vIS);  // (todo) remove
                     // // vRB = vEndLastR; vRB.sub(vEndLastR); vRB.add(vIS);  // (todo) remove
                     // // if(b_debug)
                     // //    trace "xxx bevelLineJoint: <left> vertexIterIdx="+vertexIterIdx+" bottom vRB="+vRB;

                     if(1)
                     {
                        vLTDraw = vLT;
                        vRTDraw = vRT;
                        vISDraw = vIS;

                        // trace "xxx ########### <left> draw Bevel joint vertexIterIdx="+vertexIterIdx+" vLT="+vLTDraw+" vRT="+vRTDraw+" vIS="+vISDraw;

                        addTri(&vLTDraw, &vRTDraw, &vISDraw);
                     }
                  } // if bBevel

               }
               else
               {
                  // Straight (no joint)
                  vEndLastL = vStartL;
                  vEndLastR = vStartR;

                  /* wEndLastSclL = wStartSclL; */
                  /* wEndLastSclR = wStartSclR; */

               } // winding
            } // if !bLastVertex

            // Draw (previous) line segment
            // trace "xxx skipQuad="+skipQuad;
            if(!skipQuad)
            {
               // trace "xxx drawSeg: vertexIterIdx="+vertexIterIdx;
               // // boolean bDrawSeg = (vertexIterIdx >= (0 + _bClosed));
               if(bDrawSeg)
               {
                  if(!b_line_closed && (MINNIE_LINECAP_ROUND == line_cap_type))
                  {
                     if(bFirstQuad)
                     {
                        bFirstQuad = YAC_FALSE;
                        vCapCtr.init( (vStartLastL.x + vStartLastR.x) * 0.5f,
                                      (vStartLastL.y + vStartLastR.y) * 0.5f
                                      );
                        vCapOuter = vEndLastL;
                        vCapOuter.sub(&vStartLastL);
                        vCapOuter.unitScale(-const_line_width*2.0f);
                        // vCapOuter.unitScale2f(-const_line_width*(2.0f*1.5f), -const_line_width*(2.0f*0.5f));
                        vCapOuter.add(&vCapCtr);

                        // trace "xxx first vCapCtr="+vCapCtr;
                        addRoundLineCap_bezier(&vStartLastL, &vCapOuter, &vStartLastR,
                                               num_div_round_line_joint
                                               );
                     }
                     else if(vertexIdx == numVertices-1u)
                     {
                        vCapCtr.init( (vEndL.x + vEndR.x) * 0.5f,
                                      (vEndL.y + vEndR.y) * 0.5f
                                      );
                        // vDir ?
                        vCapOuter = vEndL;
                        vCapOuter.sub(&vStartL);
                        vCapOuter.unitScale(const_line_width*2.0f);
                        // vCapOuter.unitScale2f(const_line_width*(2.0f*1.5f), const_line_width*(2.0f*0.5f));
                        vCapOuter.add(&vCapCtr);

                        addRoundLineCap_bezier(&vEndL, &vCapOuter, &vEndR,
                                               num_div_round_line_joint
                                               );
                     }
                  }

                  // trace "xxx drawSeg: vertexIdx="+vertexIdx;
                  addQuad(&vStartLastL, &vEndLastL, &vEndLastR, &vStartLastR);
               } // if bDrawSeg
            }
            else
               skipQuad--;

            /* segIdx++; */

         } // if vertexIterIdx > 0

         // Next line
         vStartLast  = vStart;
         vStart      = vEnd;
         vStartLastL = vStartL;
         vStartLastR = vStartR;
         vEndLastL   = vEndL;
         vEndLastR   = vEndR;

         /* wStartLast  = wStart; */
         /* wStart      = wEnd; */

         /* wStartLastSclL = wStartSclL; */
         /* wStartLastSclR = wStartSclR; */
         /* wEndLastSclL   = wEndSclL; */
         /* wEndLastSclR   = wEndSclR; */
         /* wStartSclL     = wEndSclL; */
         /* wStartSclR     = wEndSclR; */

         vDirLast = vDir;
         /* vertexIdx = (vertexIdx + 1) % numVertices; */
         vertexIdx++; if(vertexIdx >= numVertices) vertexIdx = 0u;
         vertexIterIdx++;
         /* bDrawQuadPrev = bDrawQuad; */
         patternBit = patternBitNext;

         // if(null != left_widths)
         //    trace "xxx vertexIterIdx="+vertexIterIdx+" left_widths="+#(left_widths);
      }

      /* if(b_debug) trace "[dbg] ShapePolyline::tesselatePolyline: mesh has "+(v_tri.numElements/(2*3))+" triangles"; */
   }

  public:
   void tesselate(void) {
      tesselatePolyline(line_vertices, b_line_closed, line_pattern);
   }

};


// <class.png>
class Path {
  public:
   minnie_allocator_handle_t allocator;
   sUI        path_idx;
   FloatArray points;  // vec2
   sBool      b_closed;
   sUI        type;  // see MINNIE_PATH_TYPE_xxx

   IntArray ia_fill;  // tesselated triangle indices

   FloatArray     va_extrude;   // extruded polyline vertices ('v_tri')
   IntArray       ia_extrude;   // (todo)
   ShapePolyline  shape_extrude;

   Path *parent_path;
   PointerArray<Path> sub_paths;

#ifdef HAVE_VGTESSELATE
   struct {
      Element *input;  // single-linked list of polygons (or NULL)
      Element *output;
   } vgtesselate;
#endif // HAVE_VGTESSELATE

  public:
   Path() {
      path_idx = 0u;
      parent_path = NULL;
   }

   void free(void) {
      points.free();
      va_extrude.free();
      sub_paths.free();
   }

   ~Path() {
      Dtorprintf("~Path this=%p\n", this);
      Dtorprintf("~Path this=%p  points.elements=%p ia_fill.elements=%p va_extrude.elements=%p ia_extrude.elements=%p\n", this, points.elements.any, ia_fill.elements.any, va_extrude.elements.any, ia_extrude.elements.any);
      free();
      Dtorprintf("~Path LEAVE this=%p\n", this);
   }

   void init(minnie_allocator_handle_t _allocator,
             const sUI _pathIdx, const sU8 _type,
             void *_pointsDataOrNull, const sU32 _pointsMaxElements,
             void *_vaExtrudeData, const sU32 _maxExtrudedElements
             ) {
      Dinitprintf("xxx Path::init: this=%p pathIdx=%u pointsMax=%u\n", this, _pathIdx, _pointsMaxElements);
      allocator = _allocator;
      points.init(_allocator, _pointsMaxElements, _pointsDataOrNull);
      va_extrude.init(_allocator, _maxExtrudedElements, _vaExtrudeData);
      path_idx  = _pathIdx;
      type      = _type;
      sub_paths.init(_allocator, MINNIE_MAX_SUB_PATHS, NULL/*elementsOrNull*/);
   }

   void updateFromFbRect(const sUI _w, const sUI _h) {
      points.empty();
      points.add2ui( 0,  0);
      points.add2ui(_w,  0);
      points.add2ui(_w, _h);
      points.add2ui( 0, _h);
      b_closed = YAC_TRUE;
   }

   void lazyBegin(const sF32 _lx, const sF32 _ly) {
      if(points.isEmpty())
         points.add2(_lx, _ly);
   }

   void lineTo(const sF32 _px, const sF32 _py) {
      points.add2(_px, _py);
   }

   void rect(const sF32 _x, const sF32 _y, const sF32 _w, const sF32 _h) {
      points.add2(_x,      _y);
      points.add2(_x + _w, _y);
      points.add2(_x + _w, _y + _h);
      points.add2(_x,      _y + _h);
   }

   void roundRect(const sF32 _x, const sF32 _y,
                  const sF32 _w, const sF32 _h,
                  const sF32 _rx, const sF32 _ry,
                  const sUI _numSeg,
                  const sF32 _minDistSqr
                  ) {
      if(_rx < 0.001f && _ry < 0.001f)
      {
         rect(_x, _y, _w, _h);
      }
      else
      {
         // top
         points.add2(_x + _rx,      _y);
         points.add2(_x + _w - _rx, _y);

         // right-top
         cubicTo(_x + _w - _rx,      _y,             // p
                 _x + _w - _rx*0.5f, _y,             // c1
                 _x + _w,            _y + _ry*0.5f,  // c2
                 _x + _w,            _y + _ry,       // q
                 _numSeg,
                 _minDistSqr
                 );

         // right
         points.add2(_x + _w, _y + _h - _ry);

         // right-bottom
         cubicTo(_x + _w,            _y + _h - _ry,       // p
                 _x + _w,            _y + _h - _ry*0.5f,  // c1
                 _x + _w - _rx*0.5f, _y + _h,             // c2
                 _x + _w - _rx,      _y + _h,             // q
                 _numSeg,
                 _minDistSqr
                 );

         // bottom
         points.add2(_x + _rx, _y + _h);

         // left-bottom
         cubicTo(_x + _rx,      _y + _h,             // p
                 _x + _rx*0.5f, _y + _h,             // c1
                 _x,            _y + _h - _ry*0.5f,  // c2
                 _x,            _y + _h - _ry,       // q
                 _numSeg,
                 _minDistSqr
                 );

         // left
         points.add2(_x, _y + _ry);

         // left-top
         cubicTo(_x,            _y + _ry,       // p
                 _x,            _y + _ry*0.5f,  // c1
                 _x + _rx*0.5f, _y,             // c2
                 _x + _rx,      _y,             // q
                 _numSeg,
                 _minDistSqr
                 );
      }
   }

   void ellipse(const sF32 _cx, const sF32 _cy, const sF32 _rx, const sF32 _ry, const sUI _numSeg) {
      const sF32 step = (sF32)(sM_2PI / _numSeg);
      sF32 a = 0.0f;
      for(sUI segIdx = 0u; segIdx < _numSeg; segIdx++)
      {
         const sF32 x = _cx + cosf(a) * _rx;
         const sF32 y = _cy + sinf(a) * _ry;
         points.add2(x, y);
         a += step;
      }
      b_closed = YAC_TRUE;
   }

   void circle(const sF32 _cx, const sF32 _cy, const sF32 _r, const sUI _numSeg) {
      ellipse(_cx, _cy, _r, _r, _numSeg);
   }

   void cubicTo(const sF32 _px,  const sF32 _py,
                const sF32 _c1x, const sF32 _c1y,
                const sF32 _c2x, const sF32 _c2y,
                const sF32 _qx,  const sF32 _qy,
                const sUI _numSeg,
                const sF32 _minDistSqr
                ) {
      /* if(b_debug_cubic) trace "[trc] Path::cubicTo: p=("+_px+";"+_py+") c1=("+_c1x+";"+_c1y+") c2=("+_c2x+";"+_c2y+") q=("+_qx+";"+_qy+") #seg="+_numSeg; */
      const sF32 dsqr = (_qx-_px)*(_qx-_px) + (_qy-_py)*(_qy-_py);
      /* sF32 dist = ::sqrtf( dsqr ); */
      if(dsqr >= _minDistSqr)  // workaround / fix for test066_cat
      {
         const sF32 step = 1.0f / _numSeg;
         sF32 t = step;
         for(sUI segIdx = 0u; segIdx < _numSeg - 1u; segIdx++)
         {
            const sF32 pc1x = _px + (_c1x - _px) * t;
            const sF32 pc1y = _py + (_c1y - _py) * t;

            const sF32 c2qx = _c2x + (_qx - _c2x) * t;
            const sF32 c2qy = _c2y + (_qy - _c2y) * t;

            const sF32 c12x = _c1x + (_c2x - _c1x) * t;
            const sF32 c12y = _c1y + (_c2y - _c1y) * t;

            const sF32 m1x = pc1x + (c12x - pc1x) * t;
            const sF32 m1y = pc1y + (c12y - pc1y) * t;

            const sF32 m2x = c12x + (c2qx - c12x) * t;
            const sF32 m2y = c12y + (c2qy - c12y) * t;

            const sF32 x = m1x + (m2x - m1x) * t;
            const sF32 y = m1y + (m2y - m1y) * t;

            points.add2(x, y);

            t += step;
         }
      }
      else
      {
         // Dprintf("xxx cubicTo: skip dist=%f < minDist=%f  q=(%f;%f) p=(%f;%f)\n", dist, _minDist, _qx, _qy, _px, _py);
      }
      points.add2(_qx, _qy);
   }

   void arcTo(sF32 _px, sF32 _py,
              sF32 _rx, sF32 _ry,
              sF32 _rot,
              sBool _bLargeArc,
              sBool _bArcSweep,
              sF32 _qx, sF32 _qy,
              sUI _numSeg
              ) {
      if(b_debug_arc) { Dprintf("[trc] arcTo p=(%f;%f) r=(%f;%f) rot=%f bLargeArc=%d bArcSweep=%d q=(%f;%f) #seg=%u\n", _px, _py, _rx, _ry, _rot, _bLargeArc, _bArcSweep, _qx, _qy, _numSeg); }

      if(_rx > 0.0f && _ry > 0.0f)
      {
         // correct out of range radii
         sF32 bx = (_px - _qx) * 0.5f;
         sF32 by = (_py - _qy) * 0.5f;

         sF32 lambda = (bx*bx) / (_rx*_rx) + (by*by) / (_ry*_ry);
         if(b_debug_arc >= 2) { Dprintf("[trc] arcTo: lambda=%f r=(%f;%f) bLargeArc=%d bArcSweep=%d\n", lambda, _rx, _ry, _bLargeArc, _bArcSweep); }
         sF32 pxd;
         sF32 pyd;
         sF32 cxd;
         sF32 cyd;
         sF32 cx;
         sF32 cy;
         sF32 prx;  // relative to center
         sF32 pry;
         sF32 qrx;
         sF32 qry;
         sF32 angP;
         sF32 angQ;

         if(lambda > 1.0f)
         {
            // radii are smaller than distance between start/end points => start/end angles are opposite from eachother
            //  - largeArcFlag has no effect
            //  - ellipse center is halfway between start/end
            lambda = ::sqrtf(lambda);
            _rx *= lambda;
            _ry *= lambda;
            if(b_debug_arc >= 2) { Dprintf("[trc] arcTo: => fixed lambda=%f new r=(%f; %f)\n", lambda, _rx, _ry); }
         }

         // x' = x*cos(a) - y*sin(a)
         // y' = x*sin(a) + y*cos(a)
         const sF32 cr = ::cosf(_rot);
         const sF32 sr = ::sinf(_rot);

         // compute (x1',y1')
         const sF32 pxd1 = (_px - _qx) * 0.5f;  // x1'  == bx
         const sF32 pyd1 = (_py - _qy) * 0.5f;  // y1'

         pxd = pxd1*cr - pyd1*sr;
         pyd = pxd1*sr + pyd1*cr;

         // compute (cx',cy')
         const sF32 rxs = _rx * _rx;
         const sF32 rys = _ry * _ry;
         const sF32 pxds = pxd * pxd;
         const sF32 pyds = pyd * pyd;

         const sF32 cfacDiv1 = (rxs*pyds + rys*pxds);
         sF32 cfac = 0.0f;
         if(0.0f != cfacDiv1)
         {
            sF32 cfacDiv2 = (rxs*rys - rxs*pyds - rys*pxds) / cfacDiv1;
            if(cfacDiv2 > 0.0f)
               cfac = ::sqrtf( cfacDiv2 );  // +-
         }

         if( (_bLargeArc == _bArcSweep) ^ 0 )
         {
            cfac = -cfac;
         }
         const sF32 cxd1 = (cfac *  (_rx * pyd)) / _ry;
         const sF32 cyd1 = (cfac * -(_ry * pxd)) / _rx;

         // compute (cx,cy)
         cxd = cxd1*cr + cyd1*sr;
         cyd = -cxd1*sr + cyd1*cr;

         cx = cxd + (_px + _qx)*0.5f;
         cy = cyd + (_py + _qy)*0.5f;

         if(1)
         {
            if(b_debug_arc >= 2) { Dprintf("[trc] arcTo: p=(%f;%f) c=(%f;%f) r=(%f; %f) cfac=%f  dist p-c=%f\n", _px, _py, cx, cy, _rx, _ry, cfac, ::sqrtf( (_px-cx)*(_px-cx) + (_py-cy)*(_py-cy) )); }
         }

         // calc start / end angles
         prx = (_px - cx) / _rx;  // relative to center + normalize
         pry = (_py - cy) / _ry;
         angP = (prx > 1.0f) ? 0.0f : (prx < -1.0f) ? (sF32)sM_PI : acosf(prx);  // >1 workaround for micro-overflows (which would result in NaN)
         if(pry < 0.0f)
            angP = (sF32)(sM_2PI - angP);

         qrx = (_qx - cx) / _rx;
         qry = (_qy - cy) / _ry;
         angQ = (qrx > 1.0f) ? 0.0f : ::acosf(qrx);  // >1 workaround for micro-overflows (which would result in NaN)
         if(qry < 0.0f)
            angQ = (sF32)(sM_2PI - angQ);

         if(b_debug_arc >= 2) { Dprintf("[trc] arcTo: p=(%f; %f) q=(%f; %f) => c=(%f; %f)  pr=(%f; %f)  qr=(%f; %f)  angP=%f  angQ=%f\n", _px, _py, _qx, _qy, cx, cy, prx, pry, qrx, qry, (angP*(360.0f/sM_2PI)), (angQ*(360.0f/sM_2PI))); }

         sF32 w;
         sF32 aDelta;

         if(angP > angQ)
         {
            if(b_debug_arc >= 2) { Dprintf("[trc] arcTo: angP > angQ\n"); }
            if(lambda < 1.0f)
            {
               if( (angP - angQ) >= (sF32)sM_PI )
               {
                  if(_bLargeArc)
                     aDelta = angQ - angP;
                  else
                  {
                     angP -= (sF32)sM_2PI;
                     aDelta = angQ - angP;
                  }
               }
               else
               {
                  if(_bLargeArc)
                     aDelta = angQ + (sF32)sM_2PI - angP;
                  else
                     aDelta = angQ - angP;
               }
            }
            else
            {
               if(_bLargeArc)
                  aDelta = angP - angQ;
               else
                  aDelta = angQ - angP;
               if(_bArcSweep != _bLargeArc)
                  aDelta = -aDelta;
            }
         }
         else
         {
            if(b_debug_arc >= 2) { Dprintf("[trc] arcTo: angP <= angQ\n"); }
            if(lambda < 1.0f)
            {
               if( (angQ - angP) >= (sF32)sM_PI)
               {
                  if(_bLargeArc)
                     aDelta = angQ - angP;
                  else
                  {
                     angQ -= (sF32)sM_2PI;
                     aDelta = angQ - angP;
                  }
               }
               else
               {
                  if(_bLargeArc)
                     aDelta = angQ - angP - (sF32)sM_2PI;
                  else
                     aDelta = angQ - angP;
               }
            }
            else
            {
               if(_bLargeArc)
                  aDelta = angQ - angP;
               else
                  aDelta = angP - angQ;
               if(_bArcSweep != _bLargeArc)
                  aDelta = -aDelta;
            }
         }

         w = aDelta / _numSeg;

         if(b_debug_arc >= 2) { Dprintf("[trc] arcTo: interp  angP=%f  angQ=%f  aDelta=%f  w=%f\n", (angP*(360.0f/sM_2PI)), (angQ*(360.0f/sM_2PI)), (aDelta*(360.0f/sM_2PI)), (w*(360.0f/sM_2PI))); }

         sF32 a = angP + w;
         for(sUI segIdx = 0u; segIdx < _numSeg - 1u; segIdx++)
         {
            const sF32 x = cosf(a) * _rx + cx;
            const sF32 y = sinf(a) * _ry + cy;

            points.add2(x, y);
            a += w;
         }

         points.add2(_qx, _qy);
      }
      else
      {
         // invalid radius => straight line
         //  (note) should not be reachable when .mib has been generated with 'minnie'
         points.add2(_qx, _qy);
      }
   }

   void end(const sBool _bClosed) {
      /* if(b_debug_paths && path_idx > 0) trace "[dbg] path idx="+path_idx+" has "+(points.numElements/2)+" points"; */
      /* Dprintf("xxx end(bClosed=%d)\n", _bClosed); */
      total_num_points += (points.num_elements / 2u);
      b_closed = _bClosed;
   }

   void extrudeShape(FloatArray *_points, const sF32 _strokeW, const sU8 _joinCapMode, const sUI _roundNumSeg, const sF32 _miterLimit) {
      /* shape_extrude.initShapePolyline(); */
      /* Dprintf("xxx extrudeShape: va_extrude.max_elements=%u joinCapMode=$%02x\n", va_extrude.max_elements, _joinCapMode); */

      if(_points->get(0) == _points->getRev(1) &&  // first equals last point ?
         _points->get(1) == _points->getRev(0))
         _points->num_elements -= 2u;

      shape_extrude.setLineVerticesRef(_points);
      shape_extrude.setTriVerticesRef(&va_extrude);
      shape_extrude.setLineWidth(_strokeW);  // radius
      shape_extrude.setEnableClosed(b_closed);
      shape_extrude.setMiterLimit(_miterLimit);
      shape_extrude.setLineJointType(_joinCapMode & 15u);
      shape_extrude.setLineCapType((_joinCapMode>>4) & 15u);
      /* if(b_debug_extrude >= 2) trace "[trc] shape_extrude: b_closed="+b_closed; */
      shape_extrude.setNumDivRoundLineJoint(_roundNumSeg);
      shape_extrude.setLinePattern(0xFFFFu);

      // (todo) emit indices
      va_extrude.empty();
      shape_extrude.tesselate();

      (void)ia_extrude.identity(va_extrude.num_elements / 2u);

      /* extrude_stroke_w = _strokeW; */
      /* extrude_join_cap = _joinCapMode; */
      /* extrude_num_seg  = _roundNumSeg; */
   }

   // <method.png>
   void exportConcaveVertices(void *_ofs, const FloatArray *_va, const sU32 _c32,
                              const sF32 _tx, const sF32 _ty, const sF32 _geoScale,
                              const sBool _bUniformColors
                              ) {
      // export path points (will later be tesselated by GPU)
      const sUI numVerts = _va->num_elements >> 1u;
      if(numVerts >= 3u)
      {
         const sF32 *va = _va->elements.f32;
         Dexportprintfv("[trc] minnie::Path::exportConcaveVertices: write %u concave vertices at vboffset=%u 16bit=%d bUniformColors=%d c32=#%08x\n", numVerts, Dstream_get_offset(_ofs), MINNIE_EXPORT_VERTEX_16BIT, _bUniformColors, _c32);
         for(sUI vtxIdx = 0u; vtxIdx < numVerts; vtxIdx++)
         {
            sF32 x = (va[0] + _tx) * _geoScale;
            sF32 y = (va[1] + _ty) * _geoScale;

#if MINNIE_EXPORT_VERTEX_16BIT
            Dstream_write_i16(_ofs, sS16(x*4.0f));
            Dstream_write_i16(_ofs, sS16(y*4.0f));
#else
            Dstream_write_f32(_ofs, x);
            Dstream_write_f32(_ofs, y);
#endif // MINNIE_EXPORT_VERTEX_16BIT

            if(!_bUniformColors)
            {
               Dstream_write_i8(_ofs, (_c32 >> 16) & 255 );  // r
               Dstream_write_i8(_ofs, (_c32 >>  8) & 255 );  // g
               Dstream_write_i8(_ofs, (_c32      ) & 255 );  // b
               Dstream_write_i8(_ofs, (_c32 >> 24) & 255 );  // a
            }

            // Next vertex
            va += 2;
         }
      }
   }

   // <method.png>
   void exportConcaveVerticesTransform2d(void *_ofs, const FloatArray *_va, const sU32 _c32,
                                         const Matrix2x3f *_mat2d, const sF32 _tx, const sF32 _ty, const sF32 _geoScale,
                                         const sBool _bUniformColors
                                         ) {
      // export path points (will later be tesselated by GPU)
      const sUI numVerts = _va->num_elements >> 1u;
      if(numVerts >= 3u)
      {
         const sF32 *va = _va->elements.f32;
         Dexportprintfv("[trc] minnie::Path::exportConcaveVerticesTransform2d: write %u concave vertices at vboffset=%u 16bit=%d bUniformColors=%d c32=#%08x\n", numVerts, Dstream_get_offset(_ofs), MINNIE_EXPORT_VERTEX_16BIT, _bUniformColors, _c32);
         for(sUI vtxIdx = 0u; vtxIdx < numVerts; vtxIdx++)
         {
            Vector2f v; v.init(va[0], va[1]);
            _mat2d->mulv(&v);
            v.x += _tx;
            v.y += _ty;
            v.mulf(_geoScale);

#if MINNIE_EXPORT_VERTEX_16BIT
            Dstream_write_i16(_ofs, sS16(v.x*4.0f));
            Dstream_write_i16(_ofs, sS16(v.y*4.0f));
#else
            Dstream_write_f32(_ofs, v.x);
            Dstream_write_f32(_ofs, v.y);
#endif // MINNIE_EXPORT_VERTEX_16BIT

            if(!_bUniformColors)
            {
               Dstream_write_i8(_ofs, (_c32 >> 16) & 255 );  // r
               Dstream_write_i8(_ofs, (_c32 >>  8) & 255 );  // g
               Dstream_write_i8(_ofs, (_c32      ) & 255 );  // b
               Dstream_write_i8(_ofs, (_c32 >> 24) & 255 );  // a
            }

            // Next vertex
            va += 2;
         }
      }
   }

   // <method.png>
   sBool tesselateConcave(const FloatArray *_va, IntArray *indices/*==tmpia_tesselate_concave*/) {
      // ear clipping
      //  - expects clock-wise vertices

      if(b_debug_tess) _va->debugPrint2("[trc] tesselateConcave: va=");

      sUI numVerts = _va->num_elements / 2u;
      if(numVerts < 3u)
         return YAC_TRUE;
      indices->identity(numVerts);

      ia_fill.alloc( (numVerts-2u) * 3u );
      /* Dprintf("xxx tesselateConcave: numVerts=%u indices->num=%u/max=%u ia_fill.num=%u/max=%u\n", numVerts, indices->num_elements, indices->max_elements, ia_fill.num_elements, ia_fill.max_elements); */

      sSI firstIndex = 0;
      sSI lastIndex = indices->num_elements - 1;
      sSI numIndices = (sSI)indices->num_elements;
      sSI numIndicesLeft = numIndices;

      sSI idxCI;

      for(;;)
      {
         /* if(b_debug_tess) trace "[trc] Path::tesselateConcave: remove START ---------------------------"; */
         /* if(b_debug_tess) trace "[trc] ~~~~~~~~~~~ numIndicesLeft="+numIndicesLeft+" indices="+indices+" first="+firstIndex+" last="+lastIndex; */

         sSI idxAI = lastIndex;
         while(-1 == indices->elements.s32[lastIndex])
            lastIndex--;
         sSI idxBI = firstIndex;
         if(-1 == indices->elements.s32[firstIndex])
            firstIndex++;
         // trace "xxx tesselateConcave numIndicesLeft="+numIndicesLeft+" numIndices="+numIndices+" first="+firstIndex+" last="+lastIndex;
         sSI idxA;
         sSI idxB;
         sSI idxC;
         sBool bRemoved;
         for(sSI loopIndicesLeftIdx = 0; loopIndicesLeftIdx < numIndicesLeft; loopIndicesLeftIdx++)
         {
            bRemoved = YAC_FALSE;

            idxCI = idxBI + 1;
            if(idxCI > lastIndex)
               idxCI = firstIndex;

            while(-1 == indices->elements.s32[idxCI])
            {
               if(++ idxCI > lastIndex)
                  idxCI = firstIndex;
            }

            idxA = indices->get(idxAI);
            idxB = indices->get(idxBI);
            idxC = indices->get(idxCI);

            if(b_debug_tess)
            {
               Dprintf("xxx try I(%d,%d,%d) = (%d,%d,%d)\n", idxAI, idxBI, idxCI, idxA, idxB, idxC);
            }

            if(b_debug_tess >= 2)
            {
               Dprintf("xxx CW check:\n");
               Dprintf("    vA=(%f;%f)\n", _va->get(idxA*2+0), _va->get(idxA*2+1));
               Dprintf("    vB=(%f;%f)\n", _va->get(idxB*2+0), _va->get(idxB*2+1));
               Dprintf("    vC=(%f;%f)\n", _va->get(idxC*2+0), _va->get(idxC*2+1));
            }

            sF32 vABx = (_va->get(idxB*2+0) - _va->get(idxA*2+0));
            sF32 vABy = (_va->get(idxB*2+1) - _va->get(idxA*2+1));
            sF32 vACx = (_va->get(idxC*2+0) - _va->get(idxA*2+0));
            sF32 vACy = (_va->get(idxC*2+1) - _va->get(idxA*2+1));
            sF32 crossZ = vABx * vACy - vABy * vACx;
            if(b_debug_tess) { Dprintf("[trc] Path::tesselateConcave: crossZ=%f\n", +crossZ); }

            if(crossZ >= 0.0f)
            {
               // clock-wise triangle (idxA, idxB, idxC)
               /* if(b_debug_tess) trace "[trc] Path::tesselateConcave: candidate triI=("+idxAI+","+idxBI+","+idxCI+") tri=("+idxA+","+idxB+","+idxC+")"; */

               // Check if any other vertex lies inside of triangle
               //  (todo) cache results ? (numVerts * ( (numVerts)*3 - 2 )) (lots of bits for e.g. 1000 vertices)

               sF32 ax = _va->get(idxA*2+0);
               sF32 ay = _va->get(idxA*2+1);

               sF32 bx = _va->get(idxB*2+0);
               sF32 by = _va->get(idxB*2+1);

               sF32 cx = _va->get(idxC*2+0);
               sF32 cy = _va->get(idxC*2+1);

               sF32 bxmax = bx - ax;
               sF32 bymay = by - ay;

               sF32 cxmax = cx - ax;
               sF32 cymay = cy - ay;

               sF32 cxmbx = cx - bx;
               sF32 cymby = cy - by;

               int otherIdx = 0;
               for(sUI loopNumVertsIdx = 0u; loopNumVertsIdx < numVerts; loopNumVertsIdx++)
               {
                  if( (otherIdx != idxA) && (otherIdx != idxB) && (otherIdx != idxC) )
                  {
                     sF32 sx = _va->get(otherIdx*2+0);
                     sF32 sy = _va->get(otherIdx*2+1);

                     sF32 as_x = sx - ax;
                     sF32 as_y = sy - ay;

                     sBool s_ab = bxmax * as_y - bymay * as_x > 0;

                     if( (cxmax * as_y - cymay * as_x > 0) != s_ab )
                     {
                        if( (cxmbx * (sy - by) - cymby * (sx - bx) > 0) == s_ab )
                        {
                           // within tri
                           /* if(b_debug_tess) trace "[trc] Path::tesselateConcave: otherIdx="+otherIdx+" within tri ("+idxA+","+idxB+","+idxC+")"; */
                           break;
                        }
                     }
                  }

                  otherIdx++;
               }

               if(otherIdx == numVerts)
               {
                  // no other vertex inside of tri => add triangle and remove center vertex
                  // if(9 == idxA && 1 == idxB && 2 == idxC)
                  // {
                  ia_fill.add3(idxA, idxB, idxC);
                  //    trace "xxx remove points="+points.string2;
                  //    trace "    remove addTri vA=("+points.get(idxA*2+0)+";"+points.get(idxA*2+1)+")";
                  //    trace "    remove addTri vB=("+points.get(idxB*2+0)+";"+points.get(idxB*2+1)+")";
                  //    trace "    remove addTri vC=("+points.get(idxC*2+0)+";"+points.get(idxC*2+1)+")";
                  // }
                  if(idxBI == firstIndex)
                  {
                     while(-1 == indices->get(++firstIndex))
                        ;
                  }
                  else if(idxBI == lastIndex)
                  {
                     while(-1 == indices->get(--lastIndex))
                        ;
                  }
                  indices->elements.s32[idxBI] = -1;
                  if(b_debug_tess) { Dprintf("[trc] Path::tesselateConcave: addTri (%d,%d,%d) #indices=%u firstIndex=%d lastIndex=%d\n", idxA, idxB, idxC, indices->num_elements, firstIndex, lastIndex); }
                  bRemoved = YAC_TRUE;
                  numIndicesLeft--;

                  // Restart from first index
                  break;
               }
            } // if is CW

            // else: skip and try next vertex
            idxAI = idxBI;
            idxBI = idxCI;
            /* /\* numIndicesLeft--; *\/ */

         } // loop numIndicesLeft

         if(!bRemoved)
         {
            Dprintf("[!!!] Path::tesselateConcave: failed (no removable triangles left) (numIndicesLeft=%u)\n", numIndicesLeft);
            break;
         }

         if(b_debug_tess) { Dprintf("[trc] Path::tesselateConcave: numIndicesLeft=%u\n", numIndicesLeft); }
         if(numIndicesLeft <= 3)
            break;

      } // for(;;)

      // Add remaining triangle
      if(indices->num_elements >= 3u)
      {
         /* if(b_debug_tess) */
         /* { */
         /*    trace "[trc] remaining tri: indices="+#(indices); */
         /*    trace "[trc] remaining tri: firstIndex="+firstIndex+" ("+indices.get(firstIndex)+")"; */
         /*    trace "[trc] remaining tri: lastIndex="+lastIndex+" ("+indices.get(lastIndex)+")"; */
         /* } */
         if(1)
         {
            ia_fill.add(indices->elements.s32[lastIndex]);
            ia_fill.add(indices->elements.s32[firstIndex]);
            idxCI = firstIndex + 1;
            if(idxCI > lastIndex)
               idxCI = firstIndex;
            while(-1 == indices->elements.s32[idxCI])
            {
               idxCI++;
               if(idxCI > lastIndex)
                  idxCI = firstIndex;
            }
            /* if(b_debug_tess) trace "[trc] remaining tri: CIndex="+idxCI+" ("+indices.get(idxCI)+")"; */
            ia_fill.add(indices->elements.s32[idxCI]);
         }
      }

      if(b_debug_tess) { Dprintf("[trc] Path::tesselateConcave: #ia_fill=%u numVerts=%u(*3=%u)\n", ia_fill.num_elements, numVerts, (numVerts*3u)); }
      return YAC_TRUE;
   }

   // <method.png>
#ifdef HAVE_VGTESSELATE
   sBool vgtesselateAddPolygonRegion(const sF32 *_va, const sUI _numElements) {
      const sUI numVertices = _numElements / 2u;
      const size_t allocSz = sizeof(Element) + (numVertices - 1u) * sizeof(VGES_Vector2);
      Dallocprintf("[trc] minnie::Path::vgtesselateAddPolygonRegion: numVertices=%u allocSz=%3.2fk\n", numVertices, Dkbytes(allocSz));

      Element *e = (Element *) minnie_alloc(allocator, (sUI)allocSz);
      if(NULL != e)
      {
         ::memset((void*)e, 0, sizeof(Element));
         e->m_NumVertices     = numVertices;
         e->m_IndexBufferSize = 0u;
         e->m_IndexBuffer     = NULL;

         if(NULL == vgtesselate.input)
         {
            vgtesselate.input = e;
         }
         else
         {
            // (todo) remember last
            Element *eLast = vgtesselate.input;
            while(NULL != eLast->m_Next)
               eLast = eLast->m_Next;
            eLast->m_Next = e;
         }

         VGES_Vector2 *vgInputVerts = e->m_Vertices;
#if 0
         for(sUI debugIdx = 0u; debugIdx < numVertices; debugIdx++)
         {
            Dprintf("xxx vgtesselateAddPolygonRegion: va[%u] = (%f; %f)\n", debugIdx, _va[debugIdx*2+0u], _va[debugIdx*2+1u]);
         }
#endif
         ::memcpy((void*)vgInputVerts, (const void*)_va, sizeof(VGES_Vector2) * numVertices);

         // Succeeded
         return YAC_TRUE;
      }
      else
      {
         Dprintf("[~~~] minnie::Path::vgtesselateAddPolygonRegion: failed to alloc input element (%u vertices)\n", numVertices);
      }
      return YAC_FALSE;
   }

   // <method.png>
   void vgtesselateFreeInput(void) {
      // Free tesselator input
      Element *e = vgtesselate.input;
      while(NULL != e)
      {
         Element *eNext = e->m_Next;
         minnie_free(allocator, (void*)e);
         e = eNext;
      }
      vgtesselate.input = NULL;
   }
#endif // HAVE_VGTESSELATE

   // <method_get.png>
   static sUI CalcBBoxNumPixels(const FloatArray *_va) {
      sF32 bbMinX =  9999999.0f;
      sF32 bbMinY =  9999999.0f;
      sF32 bbMaxX = -9999999.0f;
      sF32 bbMaxY = -9999999.0f;
      for(sUI vaOff = 0u; vaOff < _va->num_elements; vaOff += 2u)
      {
         sF32 x = _va->elements.f32[vaOff + 0u];
         sF32 y = _va->elements.f32[vaOff + 1u];
         if(x < bbMinX)
            bbMinX = x;
         if(x > bbMaxX)
            bbMaxX = x;
         if(y < bbMinY)
            bbMinY = y;
         if(y > bbMaxY)
            bbMaxY = y;
      }
      sUI w = (sUI)(bbMaxX - bbMinX);
      sUI h = (sUI)(bbMaxY - bbMinY);
      return w * h;
   }

   // <method.png>
   sBool tesselateEvenOdd(const FloatArray *_vaIn, FloatArray *_vaOut) {
      if(b_debug_tess >= 2)
      {
         Dprintf("xxx tesselateEvenOdd: ENTER\n");
      }
      /* return YAC_FALSE;//xxxxxxxxxxxxxxxxxxxxxxx */
#ifdef HAVE_VGTESSELATE

      debug_num_swtess_paths++;

      vgtesselate.input  = NULL;
      vgtesselate.output = NULL;

      sUI numElem = _vaIn->num_elements;
      if(numElem > 0u)
      {
         /* Dprintf("xxx tesselateEvenOdd: 1\n"); */
         if(1)
         {
            if(_vaIn->get(0) == _vaIn->getRev(1) &&  // first equals last point ?
               _vaIn->get(1) == _vaIn->getRev(0))
               numElem -= 2u;
            if(numElem >= (2u*3u))
            {
               if(b_debug_tess >= 2) { Dprintf("[trc] minnie::Path::tesselateEvenOdd: add numVert=%u origNumVert=%u\n", (numElem/2u), (_vaIn->num_elements/2u)); }
               if(!vgtesselateAddPolygonRegion(_vaIn->elements.f32, numElem))
               {
                  vgtesselateFreeInput();
                  return YAC_FALSE;
               }
            }
         }

         /* Dprintf("xxx tesselateEvenOdd: 2\n"); */

#if 1 // xxxxxxxxxxxx
         if(_vaIn == &points)  // (todo) support 2d/3d transformed sub-paths
         {
            for(sUI spIdx = 0u; spIdx < sub_paths.num_elements; spIdx++)
            {
               Path *sp = sub_paths.elements[spIdx];
               numElem = sp->points.num_elements;
               if(numElem > 0u)
               {
                  if(sp->points.get(0) == sp->points.getRev(1) &&  // first equals last point ?
                     sp->points.get(1) == sp->points.getRev(0))
                     numElem -= 2u;
                  if(numElem >= (2u*3u))
                  {
                     if(b_debug_tess >= 2) { Dprintf("[trc] minnie::Path::tesselateEvenodd: add sp numVert=%u origNumVert=%u\n", (numElem/2), (sp->points.num_elements/2u)); }
                     if(!vgtesselateAddPolygonRegion(sp->points.elements.f32, numElem))
                     {
                        vgtesselateFreeInput();
                        return YAC_FALSE;
                     }
                  }
               }
            }
         }
         else if(sub_paths.num_elements > 0u)
         {
            Dprintf("[~~~] minnie::Path::tesselateEvenOdd: skipping 2d/3d transformed sub-paths (missing implementation)\n");
         }
#endif

         /* Dprintf("xxx tesselateEvenOdd: 3\n"); */

#if 0
         {
            Element *e = vgtesselate.input;
            while(NULL != e)
            {
               printf("xxx tesselate e=%p e.numVerts=%u next=%p\n", e, e->m_NumVertices, e->m_Next);
               Element *eNext = e->m_Next;
               e = eNext;
            }
         }
#endif

         /* Dprintf("xxx tesselateEvenOdd: 4\n"); */

         sBool bOk = YAC_FALSE;

         if(b_debug_tess >= 1)
         {
            Dprintf("xxx VGES_NewTesselator: start  vgtesselate.input=%p elem1.#vertices=%u\n", vgtesselate.input, (NULL != vgtesselate.input) ? vgtesselate.input->m_NumVertices : 0u);
         }

         if(NULL != vgtesselate.input)
         {
            VGTesselator tesselator = VGES_NewTesselator();
            if(b_debug_tess >= 2)
            {
               Dprintf("xxx VGES_NewTesselator: tesselator=%p\n", tesselator);
            }
            vgtesselate.output = VGES_TesselateElements(tesselator, vgtesselate.input, VG_EVEN_ODD, VG_FALSE/*CreateEdgeFlags*/);
            if(b_debug_tess >= 2)
            {
               Dprintf("xxx VGES_NewTesselator: end\n");
            }
            VGES_DeleteTesselator(tesselator);
            /* printf("xxx VGES_NewTesselator: end 2\n"); */

            /* Dprintf("xxx tesselateEvenOdd: 5\n"); */

            // Store (indexed) result in 'points' / 'ia_fill'
            //
            //  (todo) return ref to vgtesselate.output and free after rendering
            //
#if 1  // xxxxxxxxxxxxxxxxxxx
            ia_fill.empty();
            {
               sUI numVertices = (NULL != vgtesselate.output) ? vgtesselate.output->m_NumVertices     : 0u;
               sUI numIndices  = (NULL != vgtesselate.output) ? vgtesselate.output->m_IndexBufferSize : 0u;

               _vaOut->num_elements  = 0u;
               ia_fill.num_elements = 0u;

               if(numVertices > 0u && numIndices > 0u)
               {
                  if((numVertices*2u) <= _vaOut->max_elements)
                  {
                     Dallocprintf("xxx minnie::Path::tesselateEvenOdd: ia_fill.alloc(numIndices=%u)\n", numIndices);
                     if(ia_fill.alloc(numIndices))
                     {
                        // Copy vertices
                        ::memcpy(_vaOut->elements.any, (const void*)vgtesselate.output->m_Vertices, sizeof(sF32)*2u*numVertices);
                        _vaOut->num_elements = numVertices * 2u;

                        // Copy indices
                        const VGshort *triIndices = vgtesselate.output->m_IndexBuffer;
                        for(sUI idx = 0u; idx < numIndices; idx++)
                           ia_fill.elements.s32[idx] = (sS32)triIndices[idx];
                        ia_fill.num_elements = numIndices;

                        if(b_debug_tess) { Dprintf("[trc] Path::tesselateEvenOdd: #vertices=%u #indices=%u\n", (_vaOut->num_elements/2u), ia_fill.num_elements); }
                        // Succeeded
                        bOk = YAC_TRUE;
                        /* bOk = YAC_FALSE; */
                     }
                     else
                     {
                        Derrorprintf("[---] Path::tesselateEvenOdd: failed to allocate %u result indices\n", numIndices);
                     }
                  }
                  else
                  {
                     Derrorprintf("[---] Path::tesselateEvenOdd: points array too small for result (%u < %u)\n", _vaOut->max_elements, numVertices*2u);
                  }
               }
               else
               {
                  if(b_debug_tess) { Derrorprintf("[---] minnie:Path::tesselateEvenOdd: number of result triangles is ZERO.\n"); }
               }
            }
#endif

            vgtesselateFreeInput();

         } // if vgtesselate.input

         // Free tesselator output
         //  (note) allocated via vgtesselator_alloc()
         if(NULL != vgtesselate.output)
         {
            vgtesselator_free((void*)vgtesselate.output);
            vgtesselate.output = NULL;
         }

         if(b_debug_tess >= 2)
         {
            Dprintf("xxx tesselateEvenOdd: LEAVE bOk=%d\n", bOk);
         }

         return bOk;
      }
#endif // HAVE_VGTESSELATE

      if(b_debug_tess >= 2)
      {
         Dprintf("xxx tesselateEvenOdd: LEAVE YAC_FALSE\n");
      }
      return YAC_FALSE;
   }

};


// <class.png>
/* @class MinnieSetup
 */
/* @constant MINNIE_PATH_TYPE_CONVEX
 */
/* @constant MINNIE_PATH_TYPE_CONCAVE
 */
/* @constant MINNIE_PATH_TYPE_EVENODD
 */
/* @constant MINNIE_PATH_TYPE_IMMEDIATE
 */

/* @constant MINNIE_LINEJOIN_NONE
 */
/* @constant MINNIE_LINEJOIN_MITER
 */
/* @constant MINNIE_LINEJOIN_ROUND
 */
/* @constant MINNIE_LINEJOIN_BEVEL
 */

/* @constant MINNIE_LINECAP_NONE
 */
/* @constant MINNIE_LINECAP_BUTT
 */
/* @constant MINNIE_LINECAP_ROUND
 */
/* @constant MINNIE_LINECAP_SQUARE
 */
namespace setup {
   static minnie_allocator_handle_t allocator;

   static void *loc_vb_export_ofs;
   static void *loc_dl_export_ofs;

   static sF32 stroke_scale;
   static sF32 stroke_offset;

// (note) see shadervg::Shape
#define MINNIE_SHAPE_AA_RANGE  1.5f

#define MINNIE_RECT_SINGLE_AREA_THRESHOLD  256
#define MINNIE_ROUNDRECT_SINGLE_AREA_THRESHOLD  256

#define MINNIE_ELLIPSE_EXTRUDE_I  2.0f
#define MINNIE_ELLIPSE_EXTRUDE_O  6.0f
#define MINNIE_ELLIPSE_NUM_SEG    16
#define MINNIE_ELLIPSE_SINGLE_RADIUS_THRESHOLD  10.0f


   static sUI  active_dl_op;            // MINNIE_DRAWOP_xxx
   static sUI  active_dl_num_tris;
   static sUI  active_dl_num_verts;
   static sUI  active_dl_start_offset;  // VB byte offset
   static sUI  active_dl_c32;
   static sUI  active_dl_c32_stroke;
   static sF32 active_dl_stroke_w;
   static sF32 active_dl_cx;
   static sF32 active_dl_cy;
   static sF32 active_dl_rx;
   static sF32 active_dl_ry;
   static sF32 active_dl_sx;
   static sF32 active_dl_sy;

   static sSI   dl_tex_id;
   static sBool dl_tex_repeat;
   static sBool dl_tex_filter;
   static sF32  dl_tex_decal_alpha;

   static sUI geo_w;
   static sUI geo_h;
   static sF32 aspect;  // geo_h/geo_w, e.g. (600/800)=0.75
   static IntArray palette;  // c32 initialized from RGB888 or RGB444
   static PointerArray<Framebuffer> framebuffers;
   static FramebufferOverride framebuffer_overrides[MINNIE_MAX_FRAMEBUFFERS];
   static sF32 aa_bias;
   static PointerArray<Matrix2x3f> matrices_2d;
   static PointerArray<Matrix4f>   matrices_3d;  // read from 3x3 matrix
   static PointerArray<Path>       paths;  // in reverse order
   static Path *cur_path;
   static Path *last_parent_path;
   static Path *last_drawn_path;
   static Path  path_fb;
   static sF32  path_fb_data[4*2];
   static sF32 last_x;
   static sF32 last_y;

   static sU32         cur_pal_idx;  // 8bit
   static sU32         cur_c32;
   static Matrix2x3f   cur_mat_2d;
   static Matrix4f     cur_mat_3d;
   static sBool        b_aa;
   static Framebuffer *cur_fb;
   static Framebuffer *cur_src_fb;
   static sF32         cur_x;
   static sF32         cur_y;
   static sF32         cur_mirror_x;
   static sF32         cur_mirror_y;
   static sS16         cur_mask_idx;  // -1=none
   static sF32         cur_stroke_w;
   static sUI          cur_join_cap;  // (cap<<4)|join
   static sUI          cur_num_seg;
   static sF32         cur_miter_limit;
   static Path        *cur_clip2d_path;
   static sBool        b_cur_clip2d_pre;
   static sBool        b_cur_clip_fb;
   static sBool        b_cur_clip_3d;
   static sF32         cubic_min_dist_sqr;

   static sUI def_points_per_path;  // see initScratchBuffers()
   static sUI max_extruded_vertices_per_path;

   static FloatArray tmpfa_extrude;            // maxExtrudedVerticesPerPath*2
   static FloatArray tmpfa_points2;            // maxPointsPerPath*2. translateAndScalePoints(), transform2DAndTranslateAndScale(), p->tesselateEvenOdd()
   static IntArray   tmpia_tesselate_concave;  // maxPointsPerPath*1
   static FloatArray tmpfa_clip2;              // maxClippedPointsPerPath*2
   static FloatArray tmpfa_clip2d_tmp1;        // maxClippedPointsPerPath*2
   static FloatArray tmpfa_va3;                // maxClippedPointsPerPath*3
   static FloatArray tmpfa_clip3;              // maxClippedPointsPerPath*3
   static IntArray   tmpia_clip3;              // maxClippedTrisPerPath*3
   static IntArray   tmpia_clip2;              // maxClippedTrisPerPath*3
   static IntArray   tmpia_clip_ia_tri_in;     // 3..6 indices ?
   static IntArray   tmpia_clip_ia_tri_out;    // 3..6 indices ?
   static FloatArray tmpfa_proj2;              // maxClippedTrisPerPath*2*3

#if MINNIE_EXPORT_TRIS_EDGEAA
   // temporary during path export
   static VertexAA edgeaa_vertices[MINNIE_EDGEAA_MAX_VERTICES_PER_PATH];
   static sUI edgeaa_vertices_num;
#endif // MINNIE_EXPORT_TRIS_EDGEAA

   static sBool b_edge_aa;              // def=1  can be enabled/disabled per-path
   static sBool b_tesselate_concave;    // def=1 (tesselate to triangles).  0=export as-is (and later tesselate via GPU)
   static sBool b_force_concave_evenodd;  // def=0  (1=treat concave paths as evenodd paths via SGI tesselator)
   static sUI   sw_tesselate_size_threshold;
   static sBool b_render_strokes;
   static sBool b_render_join_cap;
   static sBool b_render_fill_concave;
   static sBool b_render_fill_evenodd;
   static sBool b_uniform_colors;       // 1=set path colors via uniforms    0=store colors in vertex attribs [default]
   static sF32  stroke_w_line_strip_threshold;  // use line strips if stroke_w <= threshold  (0.0f == off)
   static sF32  stroke_w_line_join_threshold;   // disable BEVEL/ROUND line joins if stroke_w <= threshold  (0.0f == off)

  // public:
   static void Init(void) {
      def_points_per_path = 0u;
      ::memset(framebuffer_overrides, 0, sizeof(framebuffer_overrides));
      loc_vb_export_ofs = NULL;
      loc_dl_export_ofs = NULL;
      b_edge_aa                     = YAC_FALSE;
      b_tesselate_concave           = YAC_TRUE;
      b_force_concave_evenodd       = YAC_FALSE;
      sw_tesselate_size_threshold   = 0u;
      b_render_strokes              = YAC_TRUE;
      b_render_join_cap             = YAC_TRUE;
      b_render_fill_concave         = YAC_TRUE;
      b_render_fill_evenodd         = YAC_TRUE;
      b_uniform_colors              = YAC_FALSE;
      stroke_w_line_strip_threshold = 0.0f;
      stroke_w_line_join_threshold  = 0.0f;
      stroke_scale                  = fb_scale/*1.0f*/;
      stroke_offset                 = 0.0f;  // e.g. for GL edge_aa renderer
   }

   static void freeDynamic(void) {
      path_fb     .free();
      framebuffers.free();
      matrices_2d .free();
      matrices_3d .free();
      paths       .free();
      palette     .free();
   }

   static void Exit(void) {
      // Dtorprintf("~Minnie this=%p tmpfa_extrude=%p tmpfa_extrude.elements=%p\n", this, &tmpfa_extrude, tmpfa_extrude.elements.any);
      freeDynamic();
      tmpfa_extrude          .free();
      tmpfa_points2          .free();
      tmpia_tesselate_concave.free();
      tmpfa_clip2            .free();
      tmpfa_clip2d_tmp1      .free();
      tmpfa_va3              .free();
      tmpfa_clip3            .free();
      tmpia_clip3            .free();
      tmpia_clip2            .free();
      tmpia_clip_ia_tri_in   .free();
      tmpia_clip_ia_tri_out  .free();
      tmpfa_proj2            .free();
      Dtorprintf("~Minnie: LEAVE\n");
   }

   // <method.png>
   static void reset(void) {

      freeDynamic();

      last_x       = 0.0f;
      last_y       = 0.0f;

      last_parent_path = NULL;
      last_drawn_path = NULL;

      cur_mat_2d.initIdentity();
      cur_mat_3d.initIdentity();

      b_aa         = YAC_FALSE;

      aspect = (600.0f / 800.0f);

      cur_fb             = NULL;
      cur_src_fb         = NULL;
      cur_path           = NULL;
      cur_x              = 0.0f;
      cur_y              = 0.0f;
      cur_mask_idx       = -1;
      cur_stroke_w       = 0.0f;
      cur_num_seg        = 6u;
      cur_pal_idx        = 1u;
      cur_c32            = 0xFFffffffu;
      cur_join_cap       = MINNIE_LINEJOIN_BEVEL | (MINNIE_LINECAP_ROUND << 4);
      cur_miter_limit    = 32.0f;
      cur_clip2d_path    = NULL;
      b_cur_clip2d_pre   = YAC_FALSE;
      b_cur_clip_fb      = YAC_FALSE;
      b_cur_clip_3d      = YAC_FALSE;
      cubic_min_dist_sqr = 4.0f;

      total_num_points      = 0u;
      total_num_lines       = 0u;
      total_num_line_strips = 0u;
      total_num_tris        = 0u;
      total_num_tris_tex    = 0u;
      total_num_rects       = 0u;
      total_num_roundrects  = 0u;
      total_num_ellipses    = 0u;

      active_dl_op           = 0u;
      active_dl_num_tris     = 0u;
      active_dl_num_verts    = 0u;
      active_dl_start_offset = 0u;
      active_dl_c32          = 0x12CD34EFu;
      active_dl_c32_stroke   = 0x12CD34EFu;
      active_dl_stroke_w     = 0.0f;
      active_dl_cx           = 0.0f;
      active_dl_cy           = 0.0f;
      active_dl_rx           = 0.0f;
      active_dl_ry           = 0.0f;
      active_dl_sx           = 0.0f;
      active_dl_sy           = 0.0f;

      dl_tex_id          = 0;
      dl_tex_repeat      = YAC_FALSE;
      dl_tex_filter      = YAC_FALSE;
      dl_tex_decal_alpha = 1.0f;

      debug_num_paths                     = 0u;
      debug_num_swtess_paths              = 0u;
      debug_num_small_paths               = 0u;
      debug_num_small_path_points         = 0u;
      debug_num_large_paths               = 0u;
      debug_num_large_path_points         = 0u;
      debug_num_stroke_open_paths         = 0u;
      debug_num_stroke_open_path_points   = 0u;
      debug_num_stroke_closed_paths       = 0u;
      debug_num_stroke_closed_path_points = 0u;
   }

   // <method_set.png>
   static void setEnableDraw(sBool _bEnable) {
      b_draw_tri  = _bEnable;
      b_draw_line = _bEnable;
      b_draw_fill = _bEnable;
   }

   // <method_set.png>
   static void setEnableDrawSW(sBool _bEnable) {
      b_draw_sw_tri  = _bEnable;
      b_draw_sw_line = _bEnable;
      b_draw_sw_fill = _bEnable;
   }

   // <method_set.png>
   static void setVertexBufferExportOFS(void *_ofs) {
      loc_vb_export_ofs = _ofs;
   }

   // <method_set.png>
   static void setDrawListExportOFS(void *_ofs) {
      loc_dl_export_ofs = _ofs;
   }

   // <method_set.png>
   static void setEnableRenderStrokes(sBool _bEnable) {
      b_render_strokes = _bEnable;
   }

   // <method_get.png>
   static sBool getEnableRenderStrokes(void) {
      return b_render_strokes;
   }

   // <method_set.png>
   static void setEnableRenderJoinCap(sBool _bEnable) {
      b_render_join_cap = _bEnable;
   }

   // <method_get.png>
   static sBool getEnableRenderJoinCap(void) {
      return b_render_join_cap;
   }

   // <method_set.png>
   static void setEnableRenderFillConcave(sBool _bEnable) {
      b_render_fill_concave = _bEnable;
   }

   // <method_get.png>
   static sBool getEnableRenderFillConcave(void) {
      return b_render_fill_concave;
   }

   // <method_set.png>
   static void setEnableRenderFillEvenOdd(sBool _bEnable) {
      b_render_fill_evenodd = _bEnable;
   }

   // <method_get.png>
   static sBool getEnableRenderFillEvenOdd(void) {
      return b_render_fill_evenodd;
   }

   // <method_set.png>
   static void setEnableUniformColors(sBool _bEnable) {
      b_uniform_colors = _bEnable;
   }

   // <method_get.png>
   static sBool getEnableUniformColors(void) {
      return b_uniform_colors;
   }

   // <method_set.png>
   static void setStrokeWLineStripThreshold(sF32 _threshold) {
      stroke_w_line_strip_threshold = _threshold;
   }

   // <method_get.png>
   static sF32 getStrokeWLineStripThreshold(void) {
      return stroke_w_line_strip_threshold;
   }

   // <method_set.png>
   static void setStrokeWLineJoinThreshold(sF32 _threshold) {
      stroke_w_line_join_threshold = _threshold;
   }

   // <method_get.png>
   static sF32 getStrokeWLineJoinThreshold(void) {
      return stroke_w_line_join_threshold;
   }

   // <method_set.png>
   static void setEnableEdgeAA(sBool _bEnable) {
#if MINNIE_EXPORT_TRIS_EDGEAA
      b_edge_aa = _bEnable;
#endif // MINNIE_EXPORT_TRIS_EDGEAA
   }

   // <method_get.png>
   static sBool getEnableEdgeAA(void) {
      return b_edge_aa;
   }

   // <method_set.png>
   static void setEnableTesselateConcave(sBool _bEnable) {
      b_tesselate_concave = _bEnable;
   }

   // <method_get.png>
   static sBool getEnableTesselateConcave(void) {
      return b_tesselate_concave;
   }

   // <method_set.png>
   static void setSwTesselateSizeThreshold(sUI _sizeThreshold) {
      sw_tesselate_size_threshold = _sizeThreshold;
   }

   // <method_get.png>
   static sUI getSwTesselateSizeThreshold(void) {
      return sw_tesselate_size_threshold;
   }

   // <method_set.png>
   static void setEnableForceConcaveEvenOdd(sBool _bEnable) {
      b_force_concave_evenodd = _bEnable;
   }

   // <method_get.png>
   static sBool getEnableForceConcaveEvenOdd(void) {
      return b_force_concave_evenodd;
   }

   // <method.png>
   static void translateAndScalePoints(FloatArray *_vaPoints, FloatArray *_vaOut) {
      _vaOut->empty();
      for(sUI vaOff = 0u; vaOff < _vaPoints->num_elements; vaOff += 2u)
      {
         _vaOut->add2((_vaPoints->get(vaOff+0u) + cur_x) * geo_scale,
                     (_vaPoints->get(vaOff+1u) + cur_y) * geo_scale
                     );
      }
   }

   // <method.png>
   static Path *updateAndGetPathFb(void) {
      path_fb.updateFromFbRect(cur_fb->w, cur_fb->h);
      return &path_fb;
   }

   // <method.png>
   static void Clip2D(const FloatArray *_vaIn, const FloatArray *_clipPath, FloatArray *_vaOut) {
      // <https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm>
      if(b_debug_clip2d) { Dprintf("[dbg] Clip2D ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); }

      // if(b_debug_clip2d) _vaIn->debugPrint2("[trc] Clip2D: vaIn=");

      _vaOut->empty();

      FloatArray *vaIn = &tmpfa_clip2d_tmp1;
      vaIn->copyFrom(_vaIn);

      Vector2f e1; e1.init(_clipPath->get(0) * geo_scale, _clipPath->get(1) * fb_scale);
      sUI clipOff = 2u;

      /* if(b_debug_clip2d) trace "[dbg] Clip2D: loop "+(_clipPath.numElements/2)+" clip path vertices"; */
      for(sUI loopClipPathIdx = 0u; loopClipPathIdx < _clipPath->num_elements / 2u; loopClipPathIdx++)
      {
         _vaOut->empty();

         Vector2f e2; e2.init(_clipPath->get(clipOff+0u) * geo_scale, _clipPath->get(clipOff+1u) * fb_scale);

         Vector2f vN; vN.init( (e2.y - e1.y),
                               -(e2.x - e1.x)
                               );

         Vector2f vPrev; vPrev.init(vaIn->get(0), vaIn->get(1));
         /* if(b_debug_clip2d) trace "[dbg] Clip2D: loop "+(vaIn.numElements/2)+" input path vertices  ------- e1="+e1+" e2="+e2+" vN="+vN; */
         sUI inOff = 2u;
         for(sUI loopVaInIdx = 0u; loopVaInIdx < vaIn->num_elements / 2u; loopVaInIdx++)
         {
            Vector2f vCur; vCur.init(vaIn->get(inOff+0u), vaIn->get(inOff+1u));
            Vector2f vIS;
            sF32 dPrev = vPrev.distanceToPlane(&e1, &vN);
            sF32 dCur  = vCur.distanceToPlane(&e1, &vN);
            /* if(b_debug_clip2d) trace "[dbg] Clip2D: inOff="+inOff+" v1="+vPrev+" v2="+vCur+" e1="+e1+" e2="+e2+" vN="+vN+" dPrev="+dPrev+" dCur="+dCur; */

            if(dCur < 0.0f)
            {
               if(dPrev >= 0.0f)
               {
                  vIS.intersect(&vPrev, &vCur, &e1, &e2, YAC_TRUE/*bExtrapolate*/);
                  _vaOut->add2(vIS.x, vIS.y);
               }
               _vaOut->add2(vCur.x, vCur.y);
            }
            else
            {
               if(dPrev < 0.0f)
               {
                  vIS.intersect(&vPrev, &vCur, &e1, &e2, YAC_TRUE/*bExtrapolate*/);
                  _vaOut->add2(vIS.x, vIS.y);
               }
            }

            // Next vertex
            vPrev = vCur;
            inOff += 2u;
            if(inOff >= vaIn->num_elements)
               inOff = 0u;
         }

         // trace "xxx Clip2D e1="+e1+" e2="+e2;

         if(0u == _vaOut->num_elements)
            break;

         // Next clip edge
         e1 = e2;
         clipOff += 2u;
         if(clipOff >= _clipPath->num_elements)
            clipOff = 0u;

         if(2u != clipOff)
            vaIn->copyFrom(_vaOut);
      }

      if(b_debug_clip2d) { Dprintf("[dbg] Clip2D: #vaOut=%u\n", _vaOut->num_elements/2u); }
   }

   // <method.png>
   static void Clip3DZNear(const FloatArray *_vaIn3, const sF32 _zNear, FloatArray *_vaOut3) {
      // <https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm>
      /* if(b_debug_clip3d) trace "[dbg] Clip3DZNear ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"; */

      _vaOut3->empty();
      Vector3f vQ; vQ.init(0.0f, 0.0f, _zNear);
      Vector3f vN; vN.init(0.0f, 0.0f, 1.0f);

      Vector3f vPrev; vPrev.init(_vaIn3->get(0), _vaIn3->get(1), _vaIn3->get(2));
      /* if(b_debug_clip3d) trace "[dbg] Clip3DZNear: loop "+(_vaIn3.numElements/2)+" input path vertices  ------- vN="+vN; */
      sUI inOff = 3u;
      const sUI vaIn3NumVerts = (_vaIn3->num_elements / 3u);
      for(sUI loopVaIn3Idx = 0u; loopVaIn3Idx < vaIn3NumVerts; loopVaIn3Idx++)
      {
         Vector3f vCur; vCur.init(_vaIn3->get(inOff+0), _vaIn3->get(inOff+1), _vaIn3->get(inOff+2));
         Vector3f vIS;
         sF32 dPrev = vPrev.distanceToPlane(&vQ, &vN);
         sF32 dCur  = vCur .distanceToPlane(&vQ, &vN);
         /* if(b_debug_clip3d) trace "[trc] Clip3DZNear: inOff="+inOff+" v1="+vPrev+" v2="+vCur+" dPrev="+dPrev+" dCur="+dCur; */

         if(dCur >= 0.0f)
         {
            if(dPrev < 0.0f)
            {
               vIS.intersectPlane(&vQ, &vN, &vPrev, &vCur);
               /* if(b_debug_clip3d) trace "[trc] Clip3DZNear: dCur>=0 dPrev<0 vPrev="+vPrev+" vCur="+vCur+" => vIS="+vIS; */
               _vaOut3->add3(vIS.x, vIS.y, vIS.z);
            }
            /* if(b_debug_clip3d) trace "[trc] Clip3DZNear: add vCur="+vCur; */
            _vaOut3->add3(vCur.x, vCur.y, vCur.z);
         }
         else
         {
            if(dPrev >= 0.0f)
            {
               vIS.intersectPlane(&vQ, &vN, &vPrev, &vCur);
               /* if(b_debug_clip3d) trace "[trc] Clip3DZNear: dCur<0 dPrev>=0 vPrev="+vPrev+" vCur="+vCur+" => vIS="+vIS; */
               _vaOut3->add3(vIS.x, vIS.y, vIS.z);
            }
         }

         // Next vertex
         vPrev = vCur;
         inOff += 3;
         if(inOff >= _vaIn3->num_elements)
            inOff = 0u;
      }

      // trace "xxx Clip3D e1="+e1+" e2="+e2;

      /* if(b_debug_clip3d) trace "[dbg] Clip3DZNear: vaOut3="+_vaOut3.string3; */
   }

   static void ClipIndexedTris2D(FloatArray *_va, const IntArray *_iaIn, const FloatArray *_clipPath, IntArray *_iaOut) {
      // <https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm>
      if(b_debug_clip2d) { Dprintf("xxx ClipIndexedTris2D ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); }
      /* if(b_debug_clip2d) trace "[dbg]   #vert="+(_va.numElements/2)+" #ind="+_iaIn.numElements+" #tris="+(_iaIn.numElements/3); */

      _iaOut->empty();

      sUI triOff = 0u;
      const sUI numTris = (_iaIn->num_elements / 3u);
      IntArray *iaTriIn = &tmpia_clip_ia_tri_in;
      IntArray *iaTriOut = &tmpia_clip_ia_tri_out;
      for(sUI loopIaInIdx = 0u; loopIaInIdx < numTris; loopIaInIdx++)
      {
         // Dprintf("xxx loopIaInIdx=%u iaIn->num_elements=%u va->num_elements=%u\n", loopIaInIdx, _iaIn->num_elements, _va->num_elements);
         iaTriIn->empty();
         iaTriIn->add(_iaIn->get(triOff + 0u));
         iaTriIn->add(_iaIn->get(triOff + 1u));
         iaTriIn->add(_iaIn->get(triOff + 2u));

         Vector2f e1; e1.init(_clipPath->get(0) * geo_scale, _clipPath->get(1) * fb_scale);

         /* if(b_debug_clip2d >= 2) trace "[trc] ClipIndexedTris2D: loop "+(_clipPath.numElements/2)+" clip path vertices"; */
         sUI clipOff = 2u;
         sUI iaOff;
         const sUI clipPathNumVerts = _clipPath->num_elements / 2u;
         for(sUI loopClipPathIdx = 0u; loopClipPathIdx < clipPathNumVerts; loopClipPathIdx++)
         {
            // Dprintf("xxx 1 iaTriIn->num_elements=%u iaTriOut->num=%u/max=%u\n", iaTriIn->num_elements, iaTriOut->num_elements, iaTriOut->max_elements);
            iaTriOut->empty();

            Vector2f e2; e2.init(_clipPath->get(clipOff+0u) * geo_scale, _clipPath->get(clipOff+1u) * fb_scale);
            Vector2f vN; vN.init( (e2.y - e1.y),
                                  -(e2.x - e1.x)
                                  );
            /* if(b_debug_clip2d >= 2) trace "[trc] ClipIndexedTris2D: loop "+(iaTriIn.numElements)+" tri indices   ------- e1="+e1+" e2="+e2+" vN="+vN; */
            // Dprintf("xxx 2 iaTriIn->num_elements=%u\n", iaTriIn->num_elements);

            Vector2f vPrev; vPrev.init(_va->get(iaTriIn->get(0)*2u+0u), _va->get(iaTriIn->get(0)*2u+1u));

            iaOff = 1u;
            for(sUI loopIaTriInIdx = 0u; loopIaTriInIdx < iaTriIn->num_elements; loopIaTriInIdx++)
            {
               Vector2f vCur; vCur.init(_va->get(iaTriIn->get(iaOff)*2u+0u), _va->get(iaTriIn->get(iaOff)*2u+1u));
               Vector2f vIS;
               const sF32 dPrev = vPrev.distanceToPlane(&e1, &vN);
               const sF32 dCur  = vCur.distanceToPlane(&e1, &vN);
               if(b_debug_clip2d >= 2)
               {
                  Dprintf("[trc] ClipIndexedTris2D: iaOff=%u v1=(%f;%f) v2=(%f;%f) e1=(%f;%f) e2=(%f;%f) vN=(%f;%f) dPrev=%f dCur=%f\n", iaOff, vPrev.x, vPrev.y, vCur.x, vCur.y, e1.x, e1.y, e2.x, e2.y, vN.x, vN.y, dPrev, dCur);
               }

               if(dCur < 0.0f)
               {
                  if(dPrev >= 0.0f)
                  {
                     vIS.intersect(&vPrev, &vCur, &e1, &e2, YAC_TRUE/*bExtrapolate*/);
                     // trace "xxx add vIS idx="+(_va.numElements/2);
                     iaTriOut->add(_va->num_elements / 2u);
                     _va->add2(vIS.x, vIS.y);
                  }
                  iaTriOut->add(iaTriIn->get(iaOff));
               }
               else
               {
                  if(dPrev < 0.0f)
                  {
                     vIS.intersect(&vPrev, &vCur, &e1, &e2, YAC_TRUE/*bExtrapolate*/);
                     /* if(b_debug_clip2d >= 2) trace "[dbg] add vIS idx="+(_va.numElements/2); */
                     iaTriOut->add(_va->num_elements / 2u);
                     _va->add2(vIS.x, vIS.y);
                  }
               }

               // Next vertex
               vPrev = vCur;
               if(++iaOff >= iaTriIn->num_elements)
                  iaOff = 0u;
            } // loop tri indices

            // trace "xxx Clip2D e1="+e1+" e2="+e2;

            if(0u == iaTriOut->num_elements)
               break;

            // Next clip edge
            e1 = e2;
            clipOff += 2u;
            if(clipOff >= _clipPath->num_elements)
               clipOff = 0u;

            if(2u != clipOff)
               iaTriIn->copyFrom(iaTriOut);

         } // loop clip path

         /* if(b_debug_clip2d) trace "[trc] triIdx="+(triOff/3)+" iaTriOut="+iaTriOut; */
         if(iaTriOut->num_elements >= 3u)
         {
            iaOff = 2u;
            for(sUI loopIaTriOutIdx = 0u; loopIaTriOutIdx < iaTriOut->num_elements - 2u; loopIaTriOutIdx++)
            {
               _iaOut->add(iaTriOut->get(0));
               _iaOut->add(iaTriOut->get(iaOff - 1u));
               _iaOut->add(iaTriOut->get(iaOff));
               iaOff++;
            }
         }

         triOff += 3u;
      } // loop triangles

      /* if(b_debug_clip2d) trace "[dbg] ClipIndexedTris2D: iaOut="+_iaOut; */
   }

   static void ClipIndexedTris3DZNear(FloatArray *_va3, const IntArray *_iaIn, const sF32 _zNear, IntArray *_iaOut) {
      // <https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm>
      /* if(b_debug_clip3d) trace "[dbg] ClipIndexedTris3DZNear ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"; */
      /* if(b_debug_clip3d) trace "[dbg]   #vert="+(_va3.numElements/2)+" #ind="+_iaIn.numElements+" #tris="+(_iaIn.numElements/3); */

      _iaOut->empty();

      Vector3f vQ; vQ.init(0.0f, 0.0f, _zNear);
      Vector3f vN; vN.init(0.0f, 0.0f, 1.0f);

      sUI triOff = 0u;
      const sUI numTris = _iaIn->num_elements / 3u;
      IntArray *iaTriIn = &tmpia_clip_ia_tri_in;
      IntArray *iaTriOut = &tmpia_clip_ia_tri_out;
      for(sUI loopIaInIdx = 0u; loopIaInIdx < numTris; loopIaInIdx++)
      {
         iaTriIn->empty();
         iaTriIn->add(_iaIn->get(triOff + 0u));
         iaTriIn->add(_iaIn->get(triOff + 1u));
         iaTriIn->add(_iaIn->get(triOff + 2u));

         iaTriOut->empty();

         /* if(b_debug_clip3d) trace "[trc] ClipIndexedTris3DZNear: loop "+(iaTriIn.numElements)+" tri indices   ------- znear="+_zNear+" vN="+vN; */

         Vector3f vPrev; vPrev.init(_va3->get(iaTriIn->get(0)*3u+0u),
                                    _va3->get(iaTriIn->get(0)*3u+1u),
                                    _va3->get(iaTriIn->get(0)*3u+2u)
                                    );

         sUI iaOff = 1u;
         for(sUI loopIaTriInIdx = 0u; loopIaTriInIdx < iaTriIn->num_elements; loopIaTriInIdx++)
         {
            Vector3f vCur; vCur.init(_va3->get(iaTriIn->get(iaOff)*3u+0u),
                                     _va3->get(iaTriIn->get(iaOff)*3u+1u),
                                     _va3->get(iaTriIn->get(iaOff)*3u+2u)
                                     );
            Vector3f vIS;
            const sF32 dPrev = vPrev.distanceToPlane(&vQ, &vN);
            const sF32 dCur  = vCur .distanceToPlane(&vQ, &vN);
            /* if(b_debug_clip3d >= 2) trace "[trc] ClipIndexedTris3DZNear: iaOff="+iaOff+" v1="+vPrev+" v2="+vCur+" dPrev="+dPrev+" dCur="+dCur; */

            if(dCur >= 0.0f)
            {
               if(dPrev < 0.0f)
               {
                  vIS.intersectPlane(&vQ, &vN, &vPrev, &vCur);
                  /* if(b_debug_clip3d) trace "[trc] ClipIndexedTris3DZNear: dCur>=0 dPrev<0 vPrev="+vPrev+" vCur="+vCur+" => vIS="+vIS; */
                  // trace "xxx add vIS idx="+(_va3.numElements/2);
                  iaTriOut->add(_va3->num_elements / 2u);
                  _va3->add3(vIS.x, vIS.y, vIS.z);
               }
               iaTriOut->add(iaTriIn->get(iaOff));
            }
            else
            {
               if(dPrev >= 0.0f)
               {
                  vIS.intersectPlane(&vQ, &vN, &vPrev, &vCur);
                  /* if(b_debug_clip3d) trace "[trc] ClipIndexedTris3DZNear: dCur<0 dPrev>=0 vPrev="+vPrev+" vCur="+vCur+" => vIS="+vIS; */
                  iaTriOut->add(_va3->num_elements / 2u);
                  _va3->add3(vIS.x, vIS.y, vIS.z);
               }
            }

            // Next vertex
            vPrev = vCur;
            iaOff++;
            if(iaOff >= iaTriIn->num_elements)
               iaOff = 0u;
         } // loop tri indices

         /* if(b_debug_clip3d) trace "[trc] triIdx="+(triOff/3)+" iaTriOut="+iaTriOut; */
         if(iaTriOut->num_elements >= 3u)
         {
            iaOff = 2;
            for(sUI loopIaTriOutIdx = 0u; loopIaTriOutIdx < iaTriOut->num_elements - 2u; loopIaTriOutIdx++)
            {
               _iaOut->add(iaTriOut->get(0));
               _iaOut->add(iaTriOut->get(iaOff - 1u));
               _iaOut->add(iaTriOut->get(iaOff));
               iaOff++;
            }
         }

         triOff += 3u;
      } // loop triangles

      /* if(b_debug_clip3d) trace "[dbg] ClipIndexedTris3DZNear: iaOut="+_iaOut; */
   }

   // <method.png>
   static void transform2DAndTranslateAndScale(FloatArray *_vaPoints, FloatArray *_vaOut) {
      _vaOut->empty();
      sUI vaOff = 0u;
      const sUI numVerts = _vaPoints->num_elements / 2u;
      for(sUI loopVaPointsIdx = 0u; loopVaPointsIdx < numVerts; loopVaPointsIdx++)
      {
         Vector2f v;
         v.init(_vaPoints->get(vaOff+0u),
                _vaPoints->get(vaOff+1u)
                );
         cur_mat_2d.mulv(&v);

         _vaOut->add2( (v.x + cur_x) * geo_scale,
                       (v.y + cur_y) * geo_scale
                       );
         vaOff += 2u;
      }
   }

   // <method.png>
#if MINNIE_SW_RENDER
   static void opFillCoverageAA() {
      const sUI numPix = cur_fb->w * cur_fb->h;
      if(numPix > 0u)
         ::memset(cur_fb->pixels.any, 0, cur_fb->h * cur_fb->pitch);
   }
#endif // MINNIE_SW_RENDER

   // <method.png>
#if MINNIE_SW_RENDER
   static void applyClearColorAfterDrawTriAA(const sUI _c24) {
      // Apply clear color after drawTriAA()
      //   (note) fb byte order: B,G,R,A (ARGB32)

// #define CLR_DEBUG defined

      const sU8 dr = (_c24 >> 16) & 255u;
      const sU8 dg = (_c24 >>  8) & 255u;
      const sU8 db = (_c24      ) & 255u;
      sU8 *dRow = cur_fb->pixels.u8;
#ifdef CLR_DEBUG
      int debugLeft = 30;
#endif // CLR_DEBUG
      for(sUI y = 0u; y < cur_fb->h; y++)
      {
         sU8 *d = dRow;
         for(sUI x = 0u; x < cur_fb->w; x++)
         {
            const sU8 as = d[3];
#if 1
            // (GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
            const sU8 ad = 255u - as;
#endif
            if(as < 255u)
            {
               const sU8 rs = d[2];
               const sU8 gs = d[1];
               const sU8 bs = d[0];
               d[3] = 255u;
#if 1
               // (GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
               d[2] = rs + ((dr * ad) >> 8);
               d[1] = gs + ((dg * ad) >> 8);
               d[0] = bs + ((db * ad) >> 8);
#else
               // old (bad AA): (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
               d[2] = dr + (((rs - dr) * as) >> 8);
               d[1] = dg + (((gs - dg) * as) >> 8);
               d[0] = db + (((bs - db) * as) >> 8);
#endif // 1
#ifdef CLR_DEBUG
               if(debugLeft && as > 0)
               {
                  debugLeft--;
                  Dprintf("xxx src=#%02x%02x%02x d=#%02x%02x%02x as=%u => #%02x%02x%02x\n", rs,gs,bs, dr,dg,db, as, d[2],d[1],d[0]);
               }
#endif // CLR_DEBUG
            }
            d += sizeof(sU32);
         }
         dRow += cur_fb->pitch;
      }
#undef CLR_DEBUG
   }
#endif // MINNIE_SW_RENDER

   // <method.png>
#if MINNIE_SW_RENDER
   static void opFill(const sU8 _palIdx) {
      const sU32 c32 = palette.getU32(_palIdx);
      if(b_aa)
      {
         if(b_debug_fill)
         {
            Dprintf("[trc] opFill: applyClearColorAfterDrawTriAA(c32=#0x%08x)\n", c32);
         }
         applyClearColorAfterDrawTriAA(c32);
      }
      else
      {
         memptr_t d = cur_fb->pixels;
         if(b_debug_fill)
         {
            Dprintf("[trc] opFill: c32=#0x%08x cur_fb->sz=(%u;%u) pitch=%u pixels=%p\n", c32, cur_fb->w, cur_fb->h, cur_fb->pitch, d.any);
         }
         for(sUI y = 0u; y < cur_fb->h; y++)
         {
            for(sUI x = 0u; x < cur_fb->w; x++)
               d.u32[x] = c32;
            d.u8 += cur_fb->pitch;
         }
      }
   }
#endif // MINNIE_SW_RENDER

   // <method.png>
#if MINNIE_SW_RENDER
   static void drawLine(sS16 _px, sS16 _py, sS16 _qx, sS16 _qy, sU32 _c) {
      // TKSTexture::drawLine()

      // 2d clipping
      if(_px<0 || _px>=(sS16)cur_fb->w || _py<0 || _py>=(sS16)cur_fb->h ||
         _qx<0 || _qx>=(sS16)cur_fb->w || _qy<0 || _qy>=(sS16)cur_fb->h
         )
         return; // only accept lines that are completely visible

      // prepare ~bresenham
      sS16 dx=_qx-_px;
      sS16 dy=_qy-_py;
      sS16 adx=(dx<0)?-dx:dx;
      sS16 ady=(dy<0)?-dy:dy;
      if(!adx)
         adx=1; // catch divbyzero
      if(!ady)
         ady=1; // catch divbyzero
      sS16 cadv;
      sU32 iadv;
      if(adx>ady)
      {
         // constant advance x
         double ax=(((double)ady)/((double)adx))*65536;
         iadv=(sU32)ax;
         cadv=(dx<0)?-1:1;
         if(dy<0)
         {
            sS16 t; // swap coords
            t=_px;
            _px=_qx;
            _qx=t;
            t=_py;
            _py=_qy;
            _qy=t;
            cadv=-cadv;
         }
         //cadv*=screen->z;
      }
      else
      {
         // constant advance y
         double ay=(((double)adx)/((double)ady))*65536;
         iadv=(sU32)ay;
         cadv=(dy<0)?-1:1;
         if(dx<0)
         {
            sS16 t; // swap coords
            t=_px;
            _px=_qx;
            _qx=t;
            t=_py;
            _py=_qy;
            _qy=t;
            cadv=-cadv;
         }
         cadv*=cur_fb->w; // xxx assumes s.x*z==stride
      }
      memptr_t d; d.u8 = cur_fb->pixels.u8 + _py*cur_fb->pitch + sizeof(sU32)*_px;
      sU32 count32=0;
      switch(4u/*screen->z*/)
      {
         case 1:
         {
            if(adx>ady)
            {
               // const advance x
               for(; adx>=0; adx--)
               {
                  d.u8[(count32>>16) * cur_fb->pitch] = _c;
                  d.u8+=cadv;
                  count32+=iadv;
               }
            }
            else
            {
               // const advance y
               for(; ady>=0; ady--)
               {
                  d.u8[(count32>>16)]=_c;
                  d.u8+=cadv;
                  count32+=iadv;
               }
            }
         }
         break;
         case 2:
            // line 16bit not implemented yet
            break;
         case 3:
            // line 24bit not implemented yet
            break;
         case 4:
            // line 32bit
         {
            if(adx>ady)
            {
               // const advance x
               if(b_aa)
               {
                  for(; adx >= 0; adx--)
                  {
                     const sUI off = (count32>>16) * cur_fb->w;
                     if( sU8(d.u32[off] >> 24) < 255u )
                        d.u32[off] = _c;
                     d.u32 += cadv;
                     count32 += iadv;
                  }
               }
               else
               {
                  for(; adx >= 0; adx--)
                  {
                     d.u32[(count32>>16)*cur_fb->w] = _c;
                     d.u32+=cadv;
                     count32+=iadv;
                  }
               }
            }
            else
            {
               // const advance y
               if(b_aa)
               {
                  for(; ady >= 0; ady--)
                  {
                     const sUI off = (count32>>16);
                     if( sU8(d.u32[off] >> 24) < 255u )
                        d.u32[off] = _c;
                     d.u32 += cadv;
                     count32 += iadv;
                  }
               }
               else
               {
                  for(; ady>=0; ady--)
                  {
                     d.u32[(count32>>16)]=_c;
                     d.u32+=cadv;
                     count32+=iadv;
                  }
               }
            }
         }
         break;
      }
   }
#endif // MINNIE_SW_RENDER

   // <method.png>
   static void opLine(sF32 _lx, sF32 _ly, sF32 _x, sF32 _y) {
      if(b_debug_line) { Dprintf("[trc] opLine(palIdx=%u c32=#%08x l=(%f;%f) c=(%f;%f))\n", cur_pal_idx, cur_c32, _lx, _ly, _x, _y); }
#if MINNIE_SW_RENDER
      if(b_draw_sw_line)
         drawLine((sS16)_lx, (sS16)_ly, (sS16)_x, (sS16)_y, cur_c32);
#endif // MINNIE_SW_RENDER

      total_num_lines++;
   }

   // <method.png>
   static void emitQuadVertices(const sF32 _x, const sF32 _y, const sF32 _w, const sF32 _h) {
      // (note) GL_TRIANGLE_FAN
      Dexport_vb_add2f(_x,      _y     );
      Dexport_vb_add2f(_x + _w, _y     );
      Dexport_vb_add2f(_x + _w, _y + _h);
      Dexport_vb_add2f(_x,      _y + _h);
   }

   // <method.png>
   static void emitRectBorderVertices(const sF32 _centerX, const sF32 _centerY,
                                      const sF32 _sizeX, const sF32 _sizeY,
                                      const sF32 _strokeW,
                                      const sF32 _aaRange
                                      ) {
      // A_______________________B
      // | C___________________D |
      // | |                   | |
      // | |                   | |
      // | |                   | |
      // | E-------------------F |
      // G_______________________H

      const sF32 ax = _centerX - _sizeX - _strokeW;
      const sF32 ay = _centerY - _sizeY - _strokeW;

      const sF32 bx = _centerX + _sizeX + _strokeW;
      const sF32 by = ay;

      const sF32 cx = _centerX - _sizeX + _strokeW + _aaRange;
      const sF32 cy = _centerY - _sizeY + _strokeW + _aaRange;

      const sF32 dx = _centerX + _sizeX - _strokeW - _aaRange;
      const sF32 dy = cy;

      const sF32 ex = cx;
      const sF32 ey = _centerY + _sizeY - _strokeW - _aaRange;

      const sF32 fx = dx;
      const sF32 fy = ey;

      const sF32 gx = ax;
      const sF32 gy = _centerY + _sizeY + _strokeW;

      const sF32 hx = bx;
      const sF32 hy = gy;

      Dexport_vb_add2f(ax, ay);  Dexport_vb_add2f(bx, by);  Dexport_vb_add2f(cx, cy);
      Dexport_vb_add2f(cx, cy);  Dexport_vb_add2f(bx, by);  Dexport_vb_add2f(dx, dy);
      Dexport_vb_add2f(ax, ay);  Dexport_vb_add2f(cx, cy);  Dexport_vb_add2f(ex, ey);
      Dexport_vb_add2f(ax, ay);  Dexport_vb_add2f(ex, ey);  Dexport_vb_add2f(gx, gy);
      Dexport_vb_add2f(dx, dy);  Dexport_vb_add2f(bx, by);  Dexport_vb_add2f(fx, fy);
      Dexport_vb_add2f(fx, fy);  Dexport_vb_add2f(bx, by);  Dexport_vb_add2f(hx, hy);
      Dexport_vb_add2f(fx, fy);  Dexport_vb_add2f(hx, hy);  Dexport_vb_add2f(ex, ey);
      Dexport_vb_add2f(ex, ey);  Dexport_vb_add2f(hx, hy);  Dexport_vb_add2f(gx, gy);
   }

   // <method.png>
   static void setupRectFillVBO32(sF32 _centerX, sF32 _centerY,
                                  sF32 _sizeX,   sF32 _sizeY
                                  ) {
      //  +0  u16 aaRange * 256
      //  +2  i32 vbOffInner
      //  +6  u16 numVertsInner  (0 or 6. GL_TRIANGLES)
      //  +8  i32 vbOffBorder
      //  +12 u16 numVertsBorder
      //  +14 u16 primTypeBorder

      const sF32 aaRange = MINNIE_SHAPE_AA_RANGE;   // (todo) make configurable
      Dexport_dl_i16(sU16(aaRange * 256));

      const sBool bSingle = ((_sizeX*_sizeY) <= MINNIE_RECT_SINGLE_AREA_THRESHOLD);
      sUI numVerts;

      // Inner
#if 1
      const sBool bInner = !bSingle;
#else
      const sBool bInner = YAC_FALSE;  // debug
#endif
      if(bInner)
      {
         // (note) GL_TRIANGLES
         numVerts = 6u;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);

         // left/top
         Dexport_vb_f32(_centerX - _sizeX + aaRange);
         Dexport_vb_f32(_centerY - _sizeY + aaRange);

         Dexport_vb_f32(_centerX + _sizeX - aaRange);
         Dexport_vb_f32(_centerY - _sizeY + aaRange);

         Dexport_vb_f32(_centerX - _sizeX + aaRange);
         Dexport_vb_f32(_centerY + _sizeY - aaRange);

         // right/bottom
         Dexport_vb_f32(_centerX - _sizeX + aaRange);
         Dexport_vb_f32(_centerY + _sizeY - aaRange);

         Dexport_vb_f32(_centerX + _sizeX - aaRange);
         Dexport_vb_f32(_centerY - _sizeY + aaRange);

         Dexport_vb_f32(_centerX + _sizeX - aaRange);
         Dexport_vb_f32(_centerY + _sizeY - aaRange);
      }
      else
      {
         Dexport_dl_vboffset32();
         Dexport_dl_i16(0/*numVerts*/);
      }

      if(bSingle)
      {
         numVerts = 4u;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0006/*GL_TRIANGLE_FAN*/);

         emitQuadVertices(_centerX - _sizeX,
                          _centerY - _sizeY,
                          _sizeX * 2.0f,
                          _sizeY * 2.0f
                          );
      }
      else if(YAC_TRUE/*b_draw_border*/)
      {
         numVerts = 8u * 3u;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0004/*GL_TRIANGLES*/);

         emitRectBorderVertices(_centerX, _centerY,
                                _sizeX, _sizeY,
                                0.0f/*_strokeW*/,
                                aaRange
                                );
      }

   }

   // <method.png>
   static void setupRectFillStrokeVBO32(sF32 _centerX, sF32 _centerY,
                                        sF32 _sizeX,   sF32 _sizeY,
                                        sF32 _strokeW
                                        ) {
      //  +0  u16 aaRange * 256
      //  +2  i32 vbOffInner
      //  +6  u16 numVertsInner  (0 or 6. GL_TRIANGLES)
      //  +8  i32 vbOffBorder
      //  +12 u16 numVertsBorder
      //  +14 u16 primTypeBorder

      const sF32 aaRange = MINNIE_SHAPE_AA_RANGE;   // (todo) make configurable
      Dexport_dl_i16(sU16(aaRange * 256));

      const sBool bSingle = ((_sizeX*_sizeY) <= MINNIE_RECT_SINGLE_AREA_THRESHOLD);
      sSI numVerts;

      // Inner
#if 1
      const sBool bInner = !bSingle;
#else
      const sBool bInner = 0;  // debug
#endif
      if(bInner)
      {
         // (note) GL_TRIANGLES
         numVerts = 6u;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);

         // left/top
         Dexport_vb_f32(_centerX - _sizeX + _strokeW + aaRange);
         Dexport_vb_f32(_centerY - _sizeY + _strokeW + aaRange);

         Dexport_vb_f32(_centerX + _sizeX - _strokeW - aaRange);
         Dexport_vb_f32(_centerY - _sizeY + _strokeW + aaRange);

         Dexport_vb_f32(_centerX - _sizeX + _strokeW + aaRange);
         Dexport_vb_f32(_centerY + _sizeY - _strokeW - aaRange);

         // right/bottom
         Dexport_vb_f32(_centerX - _sizeX + _strokeW + aaRange);
         Dexport_vb_f32(_centerY + _sizeY - _strokeW - aaRange);

         Dexport_vb_f32(_centerX + _sizeX - _strokeW - aaRange);
         Dexport_vb_f32(_centerY - _sizeY + _strokeW + aaRange);

         Dexport_vb_f32(_centerX + _sizeX - _strokeW - aaRange);
         Dexport_vb_f32(_centerY + _sizeY - _strokeW - aaRange);
      }
      else
      {
         Dexport_dl_vboffset32();
         Dexport_dl_i16(0/*numVerts*/);
      }

      if(bSingle)
      {
         numVerts = 4u;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0006/*GL_TRIANGLE_FAN*/);

         emitQuadVertices(_centerX - _sizeX - _strokeW,
                          _centerY - _sizeY - _strokeW,
                          (_sizeX + _strokeW) * 2.0f,
                          (_sizeY + _strokeW) * 2.0f
                          );
      }
      else if(YAC_TRUE/*b_draw_border*/)
      {
         numVerts = 8u * 3u;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0004/*GL_TRIANGLES*/);

         emitRectBorderVertices(_centerX, _centerY,
                                _sizeX, _sizeY,
                                _strokeW,
                                aaRange
                                );
      }

   }

   // <method.png>
   static void setupRectStrokeVBO32(sF32 _centerX, sF32 _centerY,
                                    sF32 _sizeX,   sF32 _sizeY,
                                    sF32 _strokeW
                                    ) {
      //  +0  u16 aaRange * 256
      //  +2  i32 vbOffBorder
      //  +6  u16 numVertsBorder
      //  +8  u16 primTypeBorder

      const sF32 aaRange = MINNIE_SHAPE_AA_RANGE;   // (todo) make configurable
      Dexport_dl_i16(sU16(aaRange * 256));

      const sBool bSingle = ((_sizeX*_sizeY) <= MINNIE_RECT_SINGLE_AREA_THRESHOLD);
      sUI numVerts;

      if(bSingle)
      {
         numVerts = 4u;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0006/*GL_TRIANGLE_FAN*/);

         emitQuadVertices(_centerX - _sizeX - _strokeW,
                          _centerY - _sizeY - _strokeW,
                          (_sizeX + _strokeW) * 2.0f,
                          (_sizeY + _strokeW) * 2.0f
                          );
      }
      else if(YAC_TRUE/*b_draw_border*/)
      {
         numVerts = 8u * 3u;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0004/*GL_TRIANGLES*/);

         emitRectBorderVertices(_centerX, _centerY,
                                _sizeX, _sizeY,
                                _strokeW,
                                aaRange
                                );
      }

   }

   // <method.png>
   static void setupEllipseFillVBO32(sF32 _centerX, sF32 _centerY,
                                     sF32 _radiusX, sF32 _radiusY
                                     ) {
      //  +0  i32 vbOffInner
      //  +4  i16 numVertsInner
      //  +6  i32 vbOffBorder
      //  +10 i16 numVertsBorder
      //  +12 u16 primTypeBorder
      //
      const sSI numSeg = MINNIE_ELLIPSE_NUM_SEG;
      const sF32 rxI = _radiusX - MINNIE_ELLIPSE_EXTRUDE_I;
      const sF32 rxO = _radiusX + MINNIE_ELLIPSE_EXTRUDE_O;
      const sF32 ryI = _radiusY - MINNIE_ELLIPSE_EXTRUDE_I;
      const sF32 ryO = _radiusY + MINNIE_ELLIPSE_EXTRUDE_O;
      const sF32 aStep = sF32(sM_2PI / numSeg);
      sF32 a;
      const sBool bSingle =
         (_radiusX < MINNIE_ELLIPSE_SINGLE_RADIUS_THRESHOLD) ||
         (_radiusY < MINNIE_ELLIPSE_SINGLE_RADIUS_THRESHOLD)
         ;
      sSI numVerts;

      // Inner
      if(!bSingle)
      {
         // Calc inner mesh
         a = aStep;
         numVerts = (2 + numSeg);

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);

         Dexport_vb_f32(_centerX);
         Dexport_vb_f32(_centerY);
         Dexport_vb_f32(_centerX + rxI);
         Dexport_vb_f32(_centerY);

         for(sSI segIdx = 0; segIdx < numSeg; segIdx++)
         {
            Dexport_vb_f32(_centerX + rxI * ::cosf(a));
            Dexport_vb_f32(_centerY + ryI * ::sinf(a));
            a += aStep;
         }

         // // glDrawArrays(GL_TRIANGLE_FAN, 0, numVerts);
      }
      else
      {
         Dexport_dl_vboffset32();
         Dexport_dl_i16(0/*numVerts*/);
      }

      // Outer border
      if(bSingle)
      {
         numVerts = 4;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0006/*GL_TRIANGLE_FAN*/);

         const sF32 x = _centerX - _radiusX;
         const sF32 y = _centerY - _radiusY;
         const sF32 w = _radiusX * 2.0f;
         const sF32 h = _radiusY * 2.0f;

         Dexport_vb_f32(x);
         Dexport_vb_f32(y);

         Dexport_vb_f32(x + w);
         Dexport_vb_f32(y);

         Dexport_vb_f32(x + w);
         Dexport_vb_f32(y + h);

         Dexport_vb_f32(x);
         Dexport_vb_f32(y + h);

         // // glDrawArrays(GL_TRIANGLE_FAN, 0, numVerts);
      }
      else
      {
         a = aStep;
         numVerts = (2 + numSeg*2);

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0005/*GL_TRIANGLE_STRIP*/);

         Dexport_vb_f32(_centerX + rxI);
         Dexport_vb_f32(_centerY);

         Dexport_vb_f32(_centerX + rxO);
         Dexport_vb_f32(_centerY);

         for(sSI segIdx = 0; segIdx < numSeg; segIdx++)
         {
            Dexport_vb_f32( _centerX + rxI * ::cosf(a) );
            Dexport_vb_f32( _centerY + ryI * ::sinf(a) );

            Dexport_vb_f32( _centerX + rxO * ::cosf(a) );
            Dexport_vb_f32( _centerY + ryO * ::sinf(a) );

            a += aStep;
         }

         // // glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerts);
      }
   }

   // <method.png>
   static void setupEllipseStrokeVBO32(sF32 _centerX, sF32 _centerY,
                                       sF32 _radiusX, sF32 _radiusY,
                                       sF32 _strokeW
                                       ) {
      //  +0 i32 vbOffBorder
      //  +4 i16 numVertsBorder
      //  +6 u16 primTypeBorder
      //
      const sSI numSeg = MINNIE_ELLIPSE_NUM_SEG;
      const sF32 rxI = _radiusX - _strokeW - MINNIE_ELLIPSE_EXTRUDE_I;
      const sF32 rxO = _radiusX + _strokeW + MINNIE_ELLIPSE_EXTRUDE_O;
      const sF32 ryI = _radiusY - _strokeW - MINNIE_ELLIPSE_EXTRUDE_I;
      const sF32 ryO = _radiusY + _strokeW + MINNIE_ELLIPSE_EXTRUDE_O;
      const sF32 aStep = sF32(sM_2PI / numSeg);
      sF32 a;
      sBool bSingle =
         (_radiusX < MINNIE_ELLIPSE_SINGLE_RADIUS_THRESHOLD) ||
         (_radiusY < MINNIE_ELLIPSE_SINGLE_RADIUS_THRESHOLD)
         ;
      sSI numVerts;

      // Outer border
      if(bSingle)
      {
         numVerts = 4;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0006/*GL_TRIANGLE_FAN*/);

         sF32 x = _centerX - _radiusX - _strokeW;
         sF32 y = _centerY - _radiusY - _strokeW;
         sF32 w = (_radiusX + _strokeW) * 2.0f;
         sF32 h = (_radiusY + _strokeW) * 2.0f;

         Dexport_vb_f32(x);
         Dexport_vb_f32(y);

         Dexport_vb_f32(x + w);
         Dexport_vb_f32(y);

         Dexport_vb_f32(x + w);
         Dexport_vb_f32(y + h);

         Dexport_vb_f32(x);
         Dexport_vb_f32(y + h);
      }
      else
      {
         a = aStep;
         numVerts = (2 + numSeg*2);

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0005/*GL_TRIANGLE_STRIP*/);

         Dexport_vb_f32(_centerX + rxI);
         Dexport_vb_f32(_centerY);

         Dexport_vb_f32(_centerX + rxO);
         Dexport_vb_f32(_centerY);

         for(sSI segIdx = 0; segIdx < numSeg; segIdx++)
         {
            Dexport_vb_f32( _centerX + rxI * ::cosf(a) );
            Dexport_vb_f32( _centerY + ryI * ::sinf(a) );

            Dexport_vb_f32( _centerX + rxO * ::cosf(a) );
            Dexport_vb_f32( _centerY + ryO * ::sinf(a) );

            a += aStep;
         }
      }
   }

   // <method.png>
   static void setupEllipseFillStrokeVBO32(sF32 _centerX, sF32 _centerY,
                                           sF32 _radiusX, sF32 _radiusY,
                                           sF32 _strokeW
                                           ) {
      //  +0  i32 vbOffInner
      //  +4  i16 numVertsInner
      //  +6  i32 vbOffBorder
      //  +10 i16 numVertsBorder
      //  +12 u16 primTypeBorder
      //
      const sSI numSeg = MINNIE_ELLIPSE_NUM_SEG;
      const sF32 rxI = _radiusX - _strokeW - MINNIE_ELLIPSE_EXTRUDE_I;
      const sF32 rxO = _radiusX + _strokeW + MINNIE_ELLIPSE_EXTRUDE_O;
      const sF32 ryI = _radiusY - _strokeW - MINNIE_ELLIPSE_EXTRUDE_I;
      const sF32 ryO = _radiusY + _strokeW + MINNIE_ELLIPSE_EXTRUDE_O;
      const sF32 aStep = sF32(sM_2PI / numSeg);
      sF32 a;
      sBool bSingle =
         (_radiusX < MINNIE_ELLIPSE_SINGLE_RADIUS_THRESHOLD) ||
         (_radiusY < MINNIE_ELLIPSE_SINGLE_RADIUS_THRESHOLD)
         ;
      sSI numVerts;

      // Inner
      Dexport_dl_vboffset32();

      if(!bSingle)
      {
         // Calc inner mesh
         a = aStep;
         numVerts = (2 + numSeg);

         Dexport_dl_i16(numVerts);

         Dexport_vb_f32(_centerX);
         Dexport_vb_f32(_centerY);
         Dexport_vb_f32(_centerX + rxI);
         Dexport_vb_f32(_centerY);

         for(sSI segIdx = 0; segIdx < numSeg; segIdx++)
         {
            Dexport_vb_f32(_centerX + rxI * ::cosf(a));
            Dexport_vb_f32(_centerY + ryI * ::sinf(a));
            a += aStep;
         }
      }
      else
      {
         Dexport_dl_i16(0/*numVerts*/);
      }

      // Outer border
      if(bSingle)
      {
         numVerts = 4;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0006/*GL_TRIANGLE_FAN*/);

         const sF32 x = _centerX - _radiusX - _strokeW;
         const sF32 y = _centerY - _radiusY - _strokeW;
         const sF32 w = (_radiusX + _strokeW) * 2.0f;
         const sF32 h = (_radiusY + _strokeW) * 2.0f;

         Dexport_vb_f32(x);
         Dexport_vb_f32(y);

         Dexport_vb_f32(x + w);
         Dexport_vb_f32(y);

         Dexport_vb_f32(x + w);
         Dexport_vb_f32(y + h);

         Dexport_vb_f32(x);
         Dexport_vb_f32(y + h);
      }
      else
      {
         a = aStep;
         numVerts = (2 + numSeg*2);

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0005/*GL_TRIANGLE_STRIP*/);

         Dexport_vb_f32(_centerX + rxI);
         Dexport_vb_f32(_centerY);

         Dexport_vb_f32(_centerX + rxO);
         Dexport_vb_f32(_centerY);

         for(sSI segIdx = 0; segIdx < numSeg; segIdx++)
         {
            Dexport_vb_f32( _centerX + rxI * ::cosf(a) );
            Dexport_vb_f32( _centerY + ryI * ::sinf(a) );

            Dexport_vb_f32( _centerX + rxO * ::cosf(a) );
            Dexport_vb_f32( _centerY + ryO * ::sinf(a) );

            a += aStep;
         }
      }
   }

   // <method.png>
   static sUI emitRoundRectInnerVertices(const sF32 _centerX, const sF32 _centerY,
                                         const sF32 _sizeX,   const sF32 _sizeY,
                                         const sF32 _radiusX, const sF32 _radiusY,
                                         const sF32 _strokeW,
                                         const sF32 _aaRange
                                         ) {
      sUI retNumTris = 8u;  // 8..14 tris

      //  A_______B________C______D
      //  | E_____F________G____H |
      //  | |    /|      / |\   | |
      //  | |   / |    /   | \  | |
      //  | |  /  |  /     |  \ | |
      //  | | /   |/       |   \| |
      // I1 J/----K--------L----M I2
      //  | |     |  <var> |    | |  center area is omitted when (_sizeY <= _radiusY)
      // N1 O-----P--------Q----R N2
      //  | |\    |      / |   /| |
      //  | | \   |    /   |  / | |
      //  | |  \  |  /     | /  | |
      //  | |   \ |/       |/   | |
      //  | S----\T--------U----V |
      //  W_______X________Y______Z
      //

      const sF32 ex = _centerX - _sizeX + _strokeW + _aaRange;
      const sF32 ey = _centerY - _sizeY + _strokeW + _aaRange;

      const sF32 fx = _centerX - _sizeX + _radiusX;
      const sF32 fy = ey;

      const sF32 gx = _centerX + _sizeX - _radiusX;
      const sF32 gy = ey;

      const sF32 hx = _centerX + _sizeX - _strokeW - _aaRange;
      // const sF32 hy = ey;

      const sF32 jx = ex;
      const sF32 jy = _centerY - _sizeY + _radiusY;

      const sF32 kx = fx;
      const sF32 ky = jy;

      const sF32 lx = gx;
      const sF32 ly = jy;

      const sF32 mx = hx;
      const sF32 my = jy;

      const sF32 ox = jx;
      const sF32 oy = _centerY + _sizeY - _radiusY;

      const sF32 px = kx;
      const sF32 py = oy;

      const sF32 qx = lx;
      const sF32 qy = oy;

      const sF32 rx = mx;
      const sF32 ry = oy;

      // const sF32 sx = ex;
      const sF32 sy = _centerY + _sizeY - _strokeW - _aaRange;

      const sF32 tx = px;
      const sF32 ty = sy;

      const sF32 ux = qx;
      const sF32 uy = sy;

      // const sF32 vx = rx;
      // const sF32 vy = sy;

      // emit triangles
      Dexport_vb_add2f(jx, jy);  Dexport_vb_add2f(fx, fy);  Dexport_vb_add2f(kx, ky);
      Dexport_vb_add2f(kx, ky);  Dexport_vb_add2f(fx, fy);  Dexport_vb_add2f(gx, gy);
      Dexport_vb_add2f(gx, gy);  Dexport_vb_add2f(lx, ly);  Dexport_vb_add2f(kx, ky);
      Dexport_vb_add2f(gx, gy);  Dexport_vb_add2f(mx, my);  Dexport_vb_add2f(lx, ly);

      if(_sizeY > _radiusY)
      {
         // center area
         Dexport_vb_add2f(jx, jy);  Dexport_vb_add2f(kx, ky);  Dexport_vb_add2f(ox, oy);
         Dexport_vb_add2f(ox, oy);  Dexport_vb_add2f(kx, ky);  Dexport_vb_add2f(px, py);
         Dexport_vb_add2f(kx, ky);  Dexport_vb_add2f(lx, ly);  Dexport_vb_add2f(px, py);
         Dexport_vb_add2f(px, py);  Dexport_vb_add2f(lx, ly);  Dexport_vb_add2f(qx, qy);
         Dexport_vb_add2f(qx, qy);  Dexport_vb_add2f(lx, ly);  Dexport_vb_add2f(mx, my);
         Dexport_vb_add2f(mx, my);  Dexport_vb_add2f(rx, ry);  Dexport_vb_add2f(qx, qy);

         retNumTris += 6u;
      }

      Dexport_vb_add2f(ox, oy);  Dexport_vb_add2f(px, py);  Dexport_vb_add2f(tx, ty);
      Dexport_vb_add2f(tx, ty);  Dexport_vb_add2f(px, py);  Dexport_vb_add2f(qx, qy);
      Dexport_vb_add2f(qx, qy);  Dexport_vb_add2f(ux, uy);  Dexport_vb_add2f(tx, ty);
      Dexport_vb_add2f(qx, qy);  Dexport_vb_add2f(rx, ry);  Dexport_vb_add2f(ux, uy);

      return retNumTris;
   }

   // <method.png>
   static sUI emitRoundRectBorderVertices(const sF32 _centerX, const sF32 _centerY,
                                          const sF32 _sizeX,   const sF32 _sizeY,
                                          const sF32 _radiusX, const sF32 _radiusY,
                                          const sF32 _strokeW,
                                          const sF32 _aaRange
                                          ) {
      sUI retNumTris = 24u;  // 24..28 tris

      //  A_______B________C______D
      //  | E_____F________G____H |
      //  | |    /|      / |\   | |
      //  | |   / |    /   | \  | |
      //  | |  /  |  /     |  \ | |
      //  | | /   |/       |   \| |
      // I1 J/----K--------L----M I2
      //  | |     |  <var> |    | |  center area is omitted when (_sizeY - _radiusY) <= 0.0f)
      // N1 O-----P--------Q----R N2
      //  | |\    |      / |   /| |
      //  | | \   |    /   |  / | |
      //  | |  \  |  /     | /  | |
      //  | |   \ |/       |/   | |
      //  | S----\T--------U----V |
      //  W_______X________Y______Z
      //

      const sF32 ax = _centerX - _sizeX - _strokeW;
      const sF32 ay = _centerY - _sizeY - _strokeW;

      const sF32 bx = _centerX - _sizeX + _radiusX;
      const sF32 by = ay;

      const sF32 cx = _centerX + _sizeX - _radiusX;
      const sF32 cy = ay;

      const sF32 dx = _centerX + _sizeX + _strokeW;
      const sF32 dy = ay;

      const sF32 ex = _centerX - _sizeX + _strokeW + _aaRange;
      const sF32 ey = _centerY - _sizeY + _strokeW + _aaRange;

      const sF32 fx = _centerX - _sizeX + _radiusX;
      const sF32 fy = ey;

      const sF32 gx = _centerX + _sizeX - _radiusX;
      const sF32 gy = ey;

      const sF32 hx = _centerX + _sizeX - _strokeW - _aaRange;
      const sF32 hy = ey;

      const sF32 i1x = ax;
      const sF32 i1y = _centerY - _sizeY + _radiusY;

      const sF32 jx = ex;
      const sF32 jy = i1y;

      const sF32 kx = fx;
      // const sF32 ky = jy;

      const sF32 lx = gx;
      // const sF32 ly = jy;

      const sF32 mx = hx;
      const sF32 my = jy;

      const sF32 i2x = dx;
      const sF32 i2y = my;

      const sF32 n1x = ax;
      const sF32 n1y = _centerY + _sizeY - _radiusY;

      const sF32 ox = jx;
      const sF32 oy = n1y;

      const sF32 px = kx;
      // const sF32 py = oy;

      const sF32 qx = lx;
      // const sF32 qy = oy;

      const sF32 rx = mx;
      const sF32 ry = oy;

      const sF32 n2x = i2x;
      const sF32 n2y = ry;

      const sF32 sx = ex;
      const sF32 sy = _centerY + _sizeY - _strokeW - _aaRange;

      const sF32 tx = px;
      const sF32 ty = sy;

      const sF32 ux = qx;
      const sF32 uy = sy;

      const sF32 vx = rx;
      const sF32 vy = sy;

      const sF32 wx = ax;
      const sF32 wy = _centerY + _sizeY + _strokeW;

      const sF32 xx = tx;
      const sF32 xy = wy;

      const sF32 yx = ux;
      const sF32 yy = wy;

      const sF32 zx = dx;
      const sF32 zy = wy;

      // top
      Dexport_vb_add2f(ax, ay);  Dexport_vb_add2f(bx, by);  Dexport_vb_add2f(ex, ey);
      Dexport_vb_add2f(ex, ey);  Dexport_vb_add2f(bx, by);  Dexport_vb_add2f(fx, fy);

      Dexport_vb_add2f(fx, fy);  Dexport_vb_add2f(bx, by);  Dexport_vb_add2f(cx, cy);
      Dexport_vb_add2f(cx, cy);  Dexport_vb_add2f(gx, gy);  Dexport_vb_add2f(fx, fy);

      Dexport_vb_add2f(cx, cy);  Dexport_vb_add2f(dx, dy);  Dexport_vb_add2f(gx, gy);
      Dexport_vb_add2f(gx, gy);  Dexport_vb_add2f(dx, dy);  Dexport_vb_add2f(hx, hy);

      // left top
      Dexport_vb_add2f(ax,  ay);  Dexport_vb_add2f(ex, ey);  Dexport_vb_add2f(i1x, i1y);
      Dexport_vb_add2f(i1x, i1y); Dexport_vb_add2f(ex, ey);  Dexport_vb_add2f(jx,  jy);

      Dexport_vb_add2f(hx,  hy);  Dexport_vb_add2f(dx, dy);  Dexport_vb_add2f(i2x, i2y);
      Dexport_vb_add2f(i2x, i2y); Dexport_vb_add2f(mx, my);  Dexport_vb_add2f(hx,  hy);

      if(_sizeY > _radiusY)
      {
         // center area borders
         Dexport_vb_add2f(i1x, i1y);  Dexport_vb_add2f(jx, jy);  Dexport_vb_add2f(n1x, n1y);
         Dexport_vb_add2f(n1x, n1y);  Dexport_vb_add2f(jx, jy);  Dexport_vb_add2f(ox,  oy);

         Dexport_vb_add2f(rx,  ry);   Dexport_vb_add2f(mx,  my);  Dexport_vb_add2f(i2x, i2y);
         Dexport_vb_add2f(i2x, i2y);  Dexport_vb_add2f(n2x, n2y); Dexport_vb_add2f(rx,  ry);

         retNumTris += 4u;
      }

      Dexport_vb_add2f(n1x, n1y);  Dexport_vb_add2f(ox, oy);  Dexport_vb_add2f(wx, wy);
      Dexport_vb_add2f(wx,  wy);   Dexport_vb_add2f(ox, oy);  Dexport_vb_add2f(sx, sy);

      Dexport_vb_add2f(wx,  wy);   Dexport_vb_add2f(sx, sy);  Dexport_vb_add2f(xx, xy);
      Dexport_vb_add2f(xx,  xy);   Dexport_vb_add2f(sx, sy);  Dexport_vb_add2f(tx, ty);

      Dexport_vb_add2f(tx,  ty);   Dexport_vb_add2f(ux, uy);  Dexport_vb_add2f(xx, xy);
      Dexport_vb_add2f(xx,  xy);   Dexport_vb_add2f(ux, uy);  Dexport_vb_add2f(yx, yy);

      Dexport_vb_add2f(ux,  uy);   Dexport_vb_add2f(vx, vy);  Dexport_vb_add2f(zx, zy);
      Dexport_vb_add2f(zx,  zy);   Dexport_vb_add2f(yx, yy);  Dexport_vb_add2f(ux, uy);

      Dexport_vb_add2f(vx,  vy);   Dexport_vb_add2f(rx, ry);  Dexport_vb_add2f(n2x, n2y);
      Dexport_vb_add2f(n2x, n2y);  Dexport_vb_add2f(zx, zy);  Dexport_vb_add2f(vx, vy);

      // corners / diagonals
      Dexport_vb_add2f(ex, ey);  Dexport_vb_add2f(fx, fy);  Dexport_vb_add2f(jx, jy);
      Dexport_vb_add2f(gx, gy);  Dexport_vb_add2f(hx, hy);  Dexport_vb_add2f(mx, my);
      Dexport_vb_add2f(ox, oy);  Dexport_vb_add2f(tx, ty);  Dexport_vb_add2f(sx, sy);
      Dexport_vb_add2f(rx, ry);  Dexport_vb_add2f(vx, vy);  Dexport_vb_add2f(ux, uy);

      return retNumTris;
   }

   static void setupRoundRectFillVBO32(sF32 _centerX, sF32 _centerY,
                                       sF32 _sizeX,   sF32 _sizeY,
                                       sF32 _radiusX, sF32 _radiusY
                                       ) {
      //  +0  u16 aaRange * 256
      //  +2  i32 vbOffInner
      //  +6  u16 numVertsInner  (GL_TRIANGLES)
      //  +8  i32 vbOffBorder
      //  +12 u16 numVertsBorder
      //  +14 u16 primTypeBorder (GL_TRIANGLE_FAN(0x0006) or GL_TRIANGLES(0x0004))

      const sF32 aaRange = MINNIE_SHAPE_AA_RANGE;   // (todo) make configurable
      Dexport_dl_i16(sU16(aaRange * 256));

      if(_radiusX > _sizeX)
         _radiusX = _sizeX;

      if(_radiusY > _sizeY)
         _radiusY = _sizeY;

      sUI numVerts;
      sUI numTris;

      const sBool bSingle = ((_sizeX*_sizeY) <= MINNIE_ROUNDRECT_SINGLE_AREA_THRESHOLD);

      // Inner
      const sBool bInner = !bSingle;
      if(bInner)
      {
         numTris = 14u;

         const sU32 vbOff = Dexport_vb_get_offset();

         numTris = emitRoundRectInnerVertices(_centerX, _centerY,
                                              _sizeX,   _sizeY,
                                              _radiusX, _radiusY,
                                              0.0f/*strokeW*/,
                                              aaRange
                                              );

         numVerts = numTris * 3u;

         Dexport_dl_i32(vbOff);
         Dexport_dl_i16(numVerts);
      }
      else
      {
         Dexport_dl_vboffset32();
         Dexport_dl_i16(0/*numVerts*/);
      }

      if(bSingle)
      {
         numVerts = 4u;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0006/*GL_TRIANGLE_FAN*/);

         emitQuadVertices(_centerX - _sizeX,
                          _centerY - _sizeY,
                          _sizeX * 2.0f,
                          _sizeY * 2.0f
                          );
      }
      else
      {
         // (note) 24..28 tris

         const sU32 vbOff = Dexport_vb_get_offset();

         numTris = emitRoundRectBorderVertices(_centerX, _centerY,
                                               _sizeX,   _sizeY,
                                               _radiusX, _radiusY,
                                               0.0f/*strokeW*/,
                                               aaRange
                                               );
         numVerts = numTris * 3u;

         Dexport_dl_i32(vbOff);
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0004/*GL_TRIANGLES*/);
      }
   }

   static void setupRoundRectStrokeVBO32(sF32 _centerX, sF32 _centerY,
                                         sF32 _sizeX,   sF32 _sizeY,
                                         sF32 _radiusX, sF32 _radiusY,
                                         sF32 _strokeW
                                         ) {
      //  +0  u16 aaRange * 256
      //  +2  i32 vbOffBorder
      //  +6  u16 numVertsBorder
      //  +8  u16 primTypeBorder (GL_TRIANGLE_FAN(0x0006) or GL_TRIANGLES(0x0004))

      const sF32 aaRange = MINNIE_SHAPE_AA_RANGE;   // (todo) make configurable
      Dexport_dl_i16(sU16(aaRange * 256));

      if(_radiusX > _sizeX)
         _radiusX = _sizeX;

      if(_radiusY > _sizeY)
         _radiusY = _sizeY;

      sUI numVerts;
      sUI numTris;

      const sBool bSingle = ((_sizeX*_sizeY) <= MINNIE_ROUNDRECT_SINGLE_AREA_THRESHOLD);

      if(bSingle)
      {
         numVerts = 4u;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0006/*GL_TRIANGLE_FAN*/);

         emitQuadVertices(_centerX - _sizeX - _strokeW,
                          _centerY - _sizeY - _strokeW,
                          (_sizeX + _strokeW) * 2.0f,
                          (_sizeY + _strokeW) * 2.0f
                          );
      }
      else
      {
         // (note) 24..28 tris

         const sU32 vbOff = Dexport_vb_get_offset();

         numTris = emitRoundRectBorderVertices(_centerX, _centerY,
                                               _sizeX,   _sizeY,
                                               _radiusX, _radiusY,
                                               _strokeW,
                                               aaRange
                                               );
         numVerts = numTris * 3u;

         Dexport_dl_i32(vbOff);
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0004/*GL_TRIANGLES*/);
      }
   }

   static void setupRoundRectFillStrokeVBO32(sF32 _centerX, sF32 _centerY,
                                             sF32 _sizeX,   sF32 _sizeY,
                                             sF32 _radiusX, sF32 _radiusY,
                                             sF32 _strokeW
                                             ) {
      //  +0  u16 aaRange * 256
      //  +2  i32 vbOffInner
      //  +6  u16 numVertsInner  (GL_TRIANGLES)
      //  +8  i32 vbOffBorder
      //  +12 u16 numVertsBorder
      //  +14 u16 primTypeBorder (GL_TRIANGLE_FAN(0x0006) or GL_TRIANGLES(0x0004))

      const sF32 aaRange = MINNIE_SHAPE_AA_RANGE;   // (todo) make configurable
      Dexport_dl_i16(sU16(aaRange * 256));

      if(_radiusX > _sizeX)
         _radiusX = _sizeX;

      if(_radiusY > _sizeY)
         _radiusY = _sizeY;

      sUI numVerts;
      sUI numTris;

      const sBool bSingle = ((_sizeX*_sizeY) <= MINNIE_ROUNDRECT_SINGLE_AREA_THRESHOLD);

      // Inner
      const sBool bInner = !bSingle;
      if(bInner)
      {
         numTris = 14u;

         const sU32 vbOff = Dexport_vb_get_offset();

         numTris = emitRoundRectInnerVertices(_centerX, _centerY,
                                              _sizeX,   _sizeY,
                                              _radiusX, _radiusY,
                                              _strokeW,
                                              aaRange
                                              );

         numVerts = numTris * 3u;

         Dexport_dl_i32(vbOff);
         Dexport_dl_i16(numVerts);
      }
      else
      {
         Dexport_dl_vboffset32();
         Dexport_dl_i16(0/*numVerts*/);
      }

      if(bSingle)
      {
         numVerts = 4u;

         Dexport_dl_vboffset32();
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0006/*GL_TRIANGLE_FAN*/);

         emitQuadVertices(_centerX - _sizeX - _strokeW,
                          _centerY - _sizeY - _strokeW,
                          (_sizeX + _strokeW) * 2.0f,
                          (_sizeY + _strokeW) * 2.0f
                          );
      }
      else
      {
         // (note) 24..28 tris

         const sU32 vbOff = Dexport_vb_get_offset();

         numTris = emitRoundRectBorderVertices(_centerX, _centerY,
                                               _sizeX,   _sizeY,
                                               _radiusX, _radiusY,
                                               _strokeW,
                                               aaRange
                                               );
         numVerts = numTris * 3u;

         Dexport_dl_i32(vbOff);
         Dexport_dl_i16(numVerts);
         Dexport_dl_i16(0x0004/*GL_TRIANGLES*/);
      }
   }

   // <method.png>
   static void finishActiveDrawListOp(void) {
      switch(active_dl_op)
      {
         default:
         case MINNIE_DRAWOP_END:
            break;

         case MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_32:
         case MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_14_2:
         case MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_EDGEAA_32:
         case MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_EDGEAA_14_2:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%02x<tri_flat> start_offset=%u num_tris=%u c32=#%08x\n", active_dl_op, active_dl_start_offset, active_dl_num_tris, active_dl_c32);
            if(active_dl_num_tris >= 1u)
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_i32(active_dl_start_offset);
               Dexport_dl_i32(active_dl_num_tris * 3u);
               Dexport_dl_f32( ((active_dl_c32 >> 16) & 255) * (1.0f / 255.0f) );  // r
               Dexport_dl_f32( ((active_dl_c32 >>  8) & 255) * (1.0f / 255.0f) );  // g
               Dexport_dl_f32( ((active_dl_c32      ) & 255) * (1.0f / 255.0f) );  // b
               Dexport_dl_f32( ((active_dl_c32 >> 24) & 255) * (1.0f / 255.0f) );  // a
            }
            break;

         case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_32:
         case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_14_2:
         case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_32:
         case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_14_2:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%02x<tri> start_offset=%u num_tris=%u\n", active_dl_op, active_dl_start_offset, active_dl_num_tris);
            if(active_dl_num_tris >= 1u)
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_i32(active_dl_start_offset);
               Dexport_dl_i32(active_dl_num_tris * 3u);
            }
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_32:
         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_14_2:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<poly_flat> start_offset=%u num_verts=%u c32=#%08x\n", active_dl_op, active_dl_start_offset, active_dl_num_verts, active_dl_c32);
            if(active_dl_num_verts >= 3u)
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_i32(active_dl_start_offset);
               Dexport_dl_i32(active_dl_num_verts);
               Dexport_dl_f32( ((active_dl_c32 >> 16) & 255) * (1.0f / 255.0f) );  // r
               Dexport_dl_f32( ((active_dl_c32 >>  8) & 255) * (1.0f / 255.0f) );  // g
               Dexport_dl_f32( ((active_dl_c32      ) & 255) * (1.0f / 255.0f) );  // b
               Dexport_dl_f32( ((active_dl_c32 >> 24) & 255) * (1.0f / 255.0f) );  // a
            }
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_32:
         case MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_14_2:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<poly> start_offset=%u num_verts=%u\n", active_dl_op, active_dl_start_offset, active_dl_num_verts);
            if(active_dl_num_verts >= 3u)
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_i32(active_dl_start_offset);
               Dexport_dl_i32(active_dl_num_verts);
            }
            break;

         case MINNIE_DRAWOP_RECT_FILL:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<rect_fill> start_offset=%u c=(%f;%f) s=(%f;%f) c32=#%08x\n", active_dl_op, active_dl_start_offset, active_dl_cx, active_dl_cy, active_dl_sx, active_dl_sy, active_dl_c32);
            {
               Dexport_dl_i16(active_dl_op);
               // (note) ShaderVG renderer treats cx/cy as center and sx/sy as radii
               const sF32 sxh = active_dl_sx * 0.5f;
               const sF32 syh = active_dl_sy * 0.5f;
               const sF32 cxc = active_dl_cx + sxh;
               const sF32 cyc = active_dl_cy + syh;
               Dexport_dl_f32(cxc);
               Dexport_dl_f32(cyc);
               Dexport_dl_f32(sxh);
               Dexport_dl_f32(syh);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               const sF32 aaOff = 1/*b_aa*/ ? MINNIE_RECT_FILL_AA_SIZE_OFFSET : 0.0f;
               setupRectFillVBO32(cxc, cyc,
                                  sxh + aaOff, syh + aaOff
                                  );
            }
            total_num_rects++;
            break;

         case MINNIE_DRAWOP_RECT_STROKE:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<rect_stroke> start_offset=%u c=(%f;%f) s=(%f;%f) c32Stroke=#%08x strokeW=%f\n", active_dl_op, active_dl_start_offset, active_dl_cx, active_dl_cy, active_dl_sx, active_dl_sy, active_dl_c32_stroke, active_dl_stroke_w);
            {
               Dexport_dl_i16(active_dl_op);
               // (note) ShaderVG renderer treats cx/cy as center and sx/sy as radii
               const sF32 sxh = active_dl_sx * 0.5f;
               const sF32 syh = active_dl_sy * 0.5f;
               const sF32 cxc = active_dl_cx + sxh;
               const sF32 cyc = active_dl_cy + syh;
               Dexport_dl_f32(cxc);
               Dexport_dl_f32(cyc);
               Dexport_dl_f32(sxh);
               Dexport_dl_f32(syh);
               Dexport_dl_i32(active_dl_c32_stroke);  // ARGB32
               Dexport_dl_f32(active_dl_stroke_w);
               const sF32 aaOffSize   = 1/*b_aa*/ ? MINNIE_RECT_AA_SIZE_OFFSET   : 0.0f;
               const sF32 aaOffStroke = 1/*b_aa*/ ? MINNIE_RECT_AA_STROKE_OFFSET : 0.0f;
               setupRectStrokeVBO32(cxc, cyc,
                                    sxh + aaOffSize, syh + aaOffSize,
                                    active_dl_stroke_w + aaOffStroke
                                    );
            }
            total_num_rects++;
            break;

         case MINNIE_DRAWOP_RECT_FILL_STROKE:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<rect_fill_stroke> start_offset=%u c=(%f;%f) s=(%f;%f) c32=#%08x c32Stroke=#%08x strokeW=%f\n", active_dl_op, active_dl_start_offset, active_dl_cx, active_dl_cy, active_dl_sx, active_dl_sy, active_dl_c32, active_dl_c32_stroke, active_dl_stroke_w);
            {
               Dexport_dl_i16(active_dl_op);
               // (note) ShaderVG renderer treats cx/cy as center and sx/sy as radii
               const sF32 sxh = active_dl_sx * 0.5f;
               const sF32 syh = active_dl_sy * 0.5f;
               const sF32 cxc = active_dl_cx + sxh;
               const sF32 cyc = active_dl_cy + syh;
               Dexport_dl_f32(cxc);
               Dexport_dl_f32(cyc);
               Dexport_dl_f32(sxh);
               Dexport_dl_f32(syh);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               Dexport_dl_i32(active_dl_c32_stroke);  // ARGB32
               Dexport_dl_f32(active_dl_stroke_w);
               const sF32 aaOffSize   = 1/*b_aa*/ ? MINNIE_RECT_AA_SIZE_OFFSET   : 0.0f;
               const sF32 aaOffStroke = 1/*b_aa*/ ? MINNIE_RECT_AA_STROKE_OFFSET : 0.0f;
               setupRectFillStrokeVBO32(cxc, cyc,
                                        sxh + aaOffSize, syh + aaOffSize,
                                        active_dl_stroke_w + aaOffStroke
                                        );
            }
            total_num_rects++;
            break;

         case MINNIE_DRAWOP_ELLIPSE_FILL:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<ellipse_fill> start_offset=%u c=(%f;%f) r=(%f;%f) c32=#%08x\n", active_dl_op, active_dl_start_offset, active_dl_cx, active_dl_cy, active_dl_rx, active_dl_ry, active_dl_c32);
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_f32(active_dl_cx);
               Dexport_dl_f32(active_dl_cy);
               Dexport_dl_f32(active_dl_rx);
               Dexport_dl_f32(active_dl_ry);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               const sF32 aaOff = 1/*b_aa*/ ? MINNIE_ELLIPSE_FILL_AA_SIZE_OFFSET : 0.0f;
               setupEllipseFillVBO32(active_dl_cx, active_dl_cy,
                                     active_dl_rx + aaOff, active_dl_ry + aaOff
                                     );
            }
            total_num_ellipses++;
            break;

         case MINNIE_DRAWOP_ELLIPSE_STROKE:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<ellipse_stroke> start_offset=%u c=(%f;%f) r=(%f;%f) strokeW=%f c32Stroke=#%08x\n", active_dl_op, active_dl_start_offset, active_dl_cx, active_dl_cy, active_dl_rx, active_dl_ry, active_dl_stroke_w, active_dl_c32_stroke);
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_f32(active_dl_cx);
               Dexport_dl_f32(active_dl_cy);
               Dexport_dl_f32(active_dl_rx);
               Dexport_dl_f32(active_dl_ry);
               Dexport_dl_i32(active_dl_c32_stroke);  // ARGB32
               Dexport_dl_f32(active_dl_stroke_w);
               const sF32 aaOffSize   = 1/*b_aa*/ ? MINNIE_ELLIPSE_AA_SIZE_OFFSET   : 0.0f;
               const sF32 aaOffStroke = 1/*b_aa*/ ? MINNIE_ELLIPSE_AA_STROKE_OFFSET : 0.0f;
               setupEllipseStrokeVBO32(active_dl_cx, active_dl_cy,
                                       active_dl_rx + aaOffSize, active_dl_ry + aaOffSize,
                                       active_dl_stroke_w + aaOffStroke
                                       );
            }
            total_num_ellipses++;
            break;

         case MINNIE_DRAWOP_ELLIPSE_FILL_STROKE:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<ellipse_fill_stroke> start_offset=%u c=(%f;%f) r=(%f;%f) c32Fill=#%08x strokeW=%f c32Stroke=#%08x\n", active_dl_op, active_dl_start_offset, active_dl_cx, active_dl_cy, active_dl_rx, active_dl_ry, active_dl_c32, active_dl_stroke_w, active_dl_c32_stroke);
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_f32(active_dl_cx);
               Dexport_dl_f32(active_dl_cy);
               Dexport_dl_f32(active_dl_rx);
               Dexport_dl_f32(active_dl_ry);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               Dexport_dl_i32(active_dl_c32_stroke);  // ARGB32
               Dexport_dl_f32(active_dl_stroke_w);
               const sF32 aaOffSize   = 1/*b_aa*/ ? MINNIE_ELLIPSE_AA_SIZE_OFFSET   : 0.0f;
               const sF32 aaOffStroke = 1/*b_aa*/ ? MINNIE_ELLIPSE_AA_STROKE_OFFSET : 0.0f;
               setupEllipseFillStrokeVBO32(active_dl_cx, active_dl_cy,
                                           active_dl_rx + aaOffSize, active_dl_ry + aaOffSize,
                                           active_dl_stroke_w + aaOffStroke
                                           );
            }
            total_num_ellipses++;
            break;

         case MINNIE_DRAWOP_ROUNDRECT_FILL:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<roundrect_fill> start_offset=%u c=(%f;%f) s=(%f;%f) r=(%f;%f) c32=#%08x\n", active_dl_op, active_dl_start_offset, active_dl_cx, active_dl_cy, active_dl_sx, active_dl_sy, active_dl_rx, active_dl_ry, active_dl_c32);
            {
               Dexport_dl_i16(active_dl_op);
               // (note) ShaderVG renderer treats cx/cy as center and sx/sy as radii
               const sF32 sxh = active_dl_sx * 0.5f;
               const sF32 syh = active_dl_sy * 0.5f;
               const sF32 cxc = active_dl_cx + sxh;
               const sF32 cyc = active_dl_cy + syh;
               Dexport_dl_f32(cxc);
               Dexport_dl_f32(cyc);
               Dexport_dl_f32(sxh);
               Dexport_dl_f32(syh);
               Dexport_dl_f32(active_dl_rx);
               Dexport_dl_f32(active_dl_ry);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               const sF32 aaOff = 1/*b_aa*/ ? MINNIE_ROUNDRECT_FILL_AA_SIZE_OFFSET : 0.0f;
               setupRoundRectFillVBO32(cxc, cyc,
                                       sxh + aaOff, syh + aaOff,
                                       active_dl_rx, active_dl_ry
                                       );
            }
            total_num_roundrects++;
            break;

         case MINNIE_DRAWOP_ROUNDRECT_STROKE:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<roundrect_stroke> start_offset=%u c=(%f;%f) s=(%f;%f) r=(%f;%f) c32Stroke=#%08x strokeW=%f\n", active_dl_op, active_dl_start_offset, active_dl_cx, active_dl_cy, active_dl_sx, active_dl_sy, active_dl_rx, active_dl_ry, active_dl_c32_stroke, active_dl_stroke_w);
            {
               Dexport_dl_i16(active_dl_op);
               // (note) ShaderVG renderer treats cx/cy as center and sx/sy as radii
               const sF32 sxh = active_dl_sx * 0.5f;
               const sF32 syh = active_dl_sy * 0.5f;
               const sF32 cxc = active_dl_cx + sxh;
               const sF32 cyc = active_dl_cy + syh;
               Dexport_dl_f32(cxc);
               Dexport_dl_f32(cyc);
               Dexport_dl_f32(sxh);
               Dexport_dl_f32(syh);
               Dexport_dl_f32(active_dl_rx);
               Dexport_dl_f32(active_dl_ry);
               Dexport_dl_i32(active_dl_c32_stroke);  // ARGB32
               Dexport_dl_f32(active_dl_stroke_w);
               const sF32 aaOffSize   = 1/*b_aa*/ ? MINNIE_ROUNDRECT_AA_SIZE_OFFSET   : 0.0f;
               const sF32 aaOffStroke = 1/*b_aa*/ ? MINNIE_ROUNDRECT_AA_STROKE_OFFSET : 0.0f;
               setupRoundRectStrokeVBO32(cxc, cyc,
                                         sxh + aaOffSize, syh + aaOffSize,
                                         active_dl_rx, active_dl_ry,
                                         active_dl_stroke_w + aaOffStroke
                                         );
            }
            total_num_roundrects++;
            break;

         case MINNIE_DRAWOP_ROUNDRECT_FILL_STROKE:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<roundrect_fill_stroke> start_offset=%u c=(%f;%f) s=(%f;%f) r=(%f;%f) c32=#%08x c32Stroke=#%08x strokeW=%f\n", active_dl_op, active_dl_start_offset, active_dl_cx, active_dl_cy, active_dl_sx, active_dl_sy, active_dl_rx, active_dl_ry, active_dl_c32, active_dl_c32_stroke, active_dl_stroke_w);
            {
               Dexport_dl_i16(active_dl_op);
               // (note) ShaderVG renderer treats cx/cy as center and sx/sy as radii
               const sF32 sxh = active_dl_sx * 0.5f;
               const sF32 syh = active_dl_sy * 0.5f;
               const sF32 cxc = active_dl_cx + sxh;
               const sF32 cyc = active_dl_cy + syh;
               Dexport_dl_f32(cxc);
               Dexport_dl_f32(cyc);
               Dexport_dl_f32(sxh);
               Dexport_dl_f32(syh);
               Dexport_dl_f32(active_dl_rx);
               Dexport_dl_f32(active_dl_ry);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               Dexport_dl_i32(active_dl_c32_stroke);  // ARGB32
               Dexport_dl_f32(active_dl_stroke_w);
               const sF32 aaOffSize   = 1/*b_aa*/ ? MINNIE_ROUNDRECT_AA_SIZE_OFFSET   : 0.0f;
               const sF32 aaOffStroke = 1/*b_aa*/ ? MINNIE_ROUNDRECT_AA_STROKE_OFFSET : 0.0f;
               setupRoundRectFillStrokeVBO32(cxc, cyc,
                                             sxh + aaOffSize, syh + aaOffSize,
                                             active_dl_rx, active_dl_ry,
                                             active_dl_stroke_w + aaOffStroke
                                             );
            }
            total_num_roundrects++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_32:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<tri_tex_uv_flat> start_offset=%u num_tris=%u c32=#%08x texId=%d texRep=%d texFlt=%d\n", active_dl_op, active_dl_start_offset, active_dl_num_tris, active_dl_c32, dl_tex_id, dl_tex_repeat, dl_tex_filter);
            if(active_dl_num_tris >= 1u)
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_i32(active_dl_start_offset);
               Dexport_dl_i32(active_dl_num_tris * 3u);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               total_num_tris_tex += active_dl_num_tris;
            }
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_DECAL_32:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<tri_tex_uv_flat_decal> start_offset=%u num_tris=%u c32=#%08x texId=%d texRep=%d texFlt=%d texDecalAlpha=%f\n", active_dl_op, active_dl_start_offset, active_dl_num_tris, active_dl_c32, dl_tex_id, dl_tex_repeat, dl_tex_filter, dl_tex_decal_alpha);
            if(active_dl_num_tris >= 1u)
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_i32(active_dl_start_offset);
               Dexport_dl_i32(active_dl_num_tris * 3u);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               total_num_tris_tex += active_dl_num_tris;
            }
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_32:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<tri_tex_uv_gouraud> start_offset=%u num_tris=%u c32=#%08x texId=%d texRep=%d texFlt=%d\n", active_dl_op, active_dl_start_offset, active_dl_num_tris, active_dl_c32, dl_tex_id, dl_tex_repeat, dl_tex_filter);
            if(active_dl_num_tris >= 1u)
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_i32(active_dl_start_offset);
               Dexport_dl_i32(active_dl_num_tris * 3u);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               total_num_tris_tex += active_dl_num_tris;
            }
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_DECAL_32:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<tri_tex_uv_gouraud_decal> start_offset=%u num_tris=%u c32=#%08x texId=%d texRep=%d texFlt=%d texDecalAlpha=%f\n", active_dl_op, active_dl_start_offset, active_dl_num_tris, active_dl_c32, dl_tex_id, dl_tex_repeat, dl_tex_filter, dl_tex_decal_alpha);
            if(active_dl_num_tris >= 1u)
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_i32(active_dl_start_offset);
               Dexport_dl_i32(active_dl_num_tris * 3u);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               total_num_tris_tex += active_dl_num_tris;
            }
            break;

         case MINNIE_DRAWOP_LINE_STRIP_FLAT_14_2:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<line_strip_flat_14_2> start_offset=%u num_verts=%u c32=#%08x\n", active_dl_op, active_dl_start_offset, active_dl_num_verts, active_dl_c32);
            if(active_dl_num_verts >= 2u)
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_i32(active_dl_start_offset);
               Dexport_dl_i32(active_dl_num_verts);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               Dexport_dl_f32(active_dl_stroke_w);
               total_num_line_strips++;
            }
            break;

         case MINNIE_DRAWOP_LINE_STRIP_FLAT_BEVEL_14_2:
            Dexportprintf("[trc] Minnie::finishActiveDrawListOp: op=0x%03x<line_strip_flat_bevel_14_2> start_offset=%u num_verts=%u c32=#%08x\n", active_dl_op, active_dl_start_offset, active_dl_num_verts, active_dl_c32);
            if(active_dl_num_verts >= 2u)
            {
               Dexport_dl_i16(active_dl_op);
               Dexport_dl_i32(active_dl_start_offset);
               Dexport_dl_i32(active_dl_num_verts);
               Dexport_dl_i32(active_dl_c32);  // ARGB32
               Dexport_dl_f32(active_dl_stroke_w);
               total_num_line_strips++;
            }
            break;

      }
      active_dl_op = 0x00u;
   }

   // <method.png>
   static sBool beginDrawListOp(sUI _op) {
      if(NULL != loc_dl_export_ofs && NULL != loc_vb_export_ofs)
      {
         if( (!b_uniform_colors || (active_dl_c32 == cur_c32)) &&
             ((active_dl_op <= MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_14_2) ||
              (active_dl_op == MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_32)
              ) &&
             (active_dl_op == _op)
             )
         {
            // continue previous draw-triangles[-edgeAA] op
            return YAC_TRUE;
         }

         finishActiveDrawListOp();

         active_dl_op           = _op;
         active_dl_start_offset = Dexport_vb_get_offset();
         active_dl_num_tris     = 0u;
         active_dl_num_verts    = 0u;
         active_dl_c32          = cur_c32;
         active_dl_stroke_w     = cur_stroke_w;

         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   static void beginDrawListOpTri(void) {
      sUI op = 0u;

      if(b_uniform_colors)
      {
#if MINNIE_EXPORT_VERTEX_16BIT
         op = MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_14_2;
#else
         op = MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_32;
#endif // MINNIE_EXPORT_VERTEX_16BIT
      }
      else
      {
#if MINNIE_EXPORT_VERTEX_16BIT
         op = MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_14_2;
#else
         op = MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_32;
#endif // MINNIE_EXPORT_VERTEX_16BIT
      }

      beginDrawListOp(op);
   }

   static void beginDrawListOpTriEdgeAA(void) {
      sUI op;

#if MINNIE_EXPORT_VERTEX_16BIT
      op = MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_14_2;
#else
      op = MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_32;
#endif // MINNIE_EXPORT_VERTEX_16BIT

      beginDrawListOp(op);
   }

   static void beginDrawListOpPolygon(void) {
      sUI op = 0u;

      if(b_uniform_colors)
      {
#if MINNIE_EXPORT_VERTEX_16BIT
         op = MINNIE_DRAWOP_POLYGON_FILL_FLAT_14_2;
#else
         op = MINNIE_DRAWOP_POLYGON_FILL_FLAT_32;
#endif // MINNIE_EXPORT_VERTEX_16BIT
      }
      else
      {
#if MINNIE_EXPORT_VERTEX_16BIT
         op = MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_14_2;
#else
         op = MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_32;
#endif // MINNIE_EXPORT_VERTEX_16BIT
      }

      beginDrawListOp(op);
   }

   // <method.png>
   static sUI calcCurJoinCap(void) {
      if(cur_stroke_w <= stroke_w_line_join_threshold)
      {
         const sUI jointType = (cur_join_cap & 15u);
         if(MINNIE_LINEJOIN_ROUND == jointType ||
            MINNIE_LINEJOIN_BEVEL == jointType
            )
            return (cur_join_cap & 0xF0u);
      }
      return cur_join_cap;
   }

   // <method.png>
   static void beginDrawListOpLineStrip(void) {
      const sUI curJoinCap = calcCurJoinCap();
      // Dprintf("xxx beginDrawListOpLineStrip: cur_stroke_w=%f\n", cur_stroke_w);
      beginDrawListOp( (MINNIE_LINEJOIN_NONE != (curJoinCap & 15u))
                       ? MINNIE_DRAWOP_LINE_STRIP_FLAT_BEVEL_14_2
                       : MINNIE_DRAWOP_LINE_STRIP_FLAT_14_2
                       );
   }

   // <method.png>
   static sBool beginDrawListOpRect(const sF32 _sx, const sF32 _sy) {
      if(NULL != loc_dl_export_ofs && NULL != loc_vb_export_ofs)
      {
         if( (active_dl_op  == MINNIE_DRAWOP_RECT_FILL) &&
             (cur_x         == active_dl_cx) &&
             (cur_y         == active_dl_cy) &&
             (_sx           == active_dl_sx) &&
             (_sy           == active_dl_sy) &&
             (cur_stroke_w > 0.0f)
             )
         {
            // merge fill+stroke
            active_dl_c32_stroke = cur_c32;
            active_dl_stroke_w   = cur_stroke_w;
            active_dl_op = MINNIE_DRAWOP_RECT_FILL_STROKE;
            return YAC_TRUE;
         }

         finishActiveDrawListOp();

         active_dl_op           = (cur_stroke_w > 0.0f) ? MINNIE_DRAWOP_RECT_STROKE : MINNIE_DRAWOP_RECT_FILL;
         active_dl_start_offset = Dexport_vb_get_offset();
         active_dl_c32          = cur_c32;
         active_dl_c32_stroke   = cur_c32;
         active_dl_stroke_w     = cur_stroke_w;
         active_dl_cx           = cur_x;
         active_dl_cy           = cur_y;
         active_dl_sx           = _sx;
         active_dl_sy           = _sy;

         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   // <method.png>
   static sBool beginDrawListOpEllipse(const sF32 _rx, const sF32 _ry) {
      if(NULL != loc_dl_export_ofs && NULL != loc_vb_export_ofs)
      {
         if( (active_dl_op  == MINNIE_DRAWOP_ELLIPSE_FILL) &&
             (cur_x         == active_dl_cx) &&
             (cur_y         == active_dl_cy) &&
             (_rx           == active_dl_rx) &&
             (_ry           == active_dl_ry) &&
             (cur_stroke_w > 0.0f)
             )
         {
            // merge fill+stroke
            active_dl_c32_stroke = cur_c32;
            active_dl_stroke_w   = cur_stroke_w;
            active_dl_op = MINNIE_DRAWOP_ELLIPSE_FILL_STROKE;
            return YAC_TRUE;
         }

         finishActiveDrawListOp();

         active_dl_op           = (cur_stroke_w > 0.0f) ? MINNIE_DRAWOP_ELLIPSE_STROKE : MINNIE_DRAWOP_ELLIPSE_FILL;
         active_dl_start_offset = Dexport_vb_get_offset();
         active_dl_c32          = cur_c32;
         active_dl_c32_stroke   = cur_c32;
         active_dl_stroke_w     = cur_stroke_w;
         active_dl_cx           = cur_x;
         active_dl_cy           = cur_y;
         active_dl_rx           = _rx;
         active_dl_ry           = _ry;

         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   // <method.png>
   static sBool beginDrawListOpRoundRect(const sF32 _sx, const sF32 _sy, const sF32 _rx, const sF32 _ry) {
      if(NULL != loc_dl_export_ofs && NULL != loc_vb_export_ofs)
      {
         if( (active_dl_op  == MINNIE_DRAWOP_ROUNDRECT_FILL) &&
             (cur_x         == active_dl_cx) &&
             (cur_y         == active_dl_cy) &&
             (_sx           == active_dl_sx) &&
             (_sy           == active_dl_sy) &&
             (_rx           == active_dl_rx) &&
             (_ry           == active_dl_ry) &&
             (cur_stroke_w > 0.0f)
             )
         {
            // merge fill+stroke
            active_dl_c32_stroke = cur_c32;
            active_dl_stroke_w   = cur_stroke_w;
            active_dl_op = MINNIE_DRAWOP_ROUNDRECT_FILL_STROKE;
            return YAC_TRUE;
         }

         finishActiveDrawListOp();

         active_dl_op           = (cur_stroke_w > 0.0f) ? MINNIE_DRAWOP_ROUNDRECT_STROKE : MINNIE_DRAWOP_ROUNDRECT_FILL;
         active_dl_start_offset = Dexport_vb_get_offset();
         active_dl_c32          = cur_c32;
         active_dl_c32_stroke   = cur_c32;
         active_dl_stroke_w     = cur_stroke_w;
         active_dl_cx           = cur_x;
         active_dl_cy           = cur_y;
         active_dl_sx           = _sx;
         active_dl_sy           = _sy;
         active_dl_rx           = _rx;
         active_dl_ry           = _ry;

         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   // <method.png>
#if MINNIE_EXPORT_TRIS_EDGEAA
   static void edgeAABeginPath(void) {
      edgeaa_vertices_num = 0u;
   }

   static void edgeAAAddVertex(sF32 _x, sF32 _y, sU32 _c32) {
      if(edgeaa_vertices_num < MINNIE_EDGEAA_MAX_VERTICES_PER_PATH)
      {
         //
         VertexAA *v = &edgeaa_vertices[edgeaa_vertices_num++];
         v->x = sS32(_x * 1024.0f);
         v->y = sS32(_y * 1024.0f);
         v->r = (_c32 >> 16) & 255u;  // r
         v->g = (_c32 >>  8) & 255u;  // g
         v->b = (_c32      ) & 255u;  // b
         v->a = (_c32 >> 24) & 255u;  // a
         v->b_edge_bc = YAC_FALSE;
         v->b_edge_ca = YAC_FALSE;
         v->b_edge_ab = YAC_FALSE;
         /* v->b_all_shared = YAC_FALSE; */
#if 0
         v->scale_s = 1.0f;
         v->scale_t = 1.0f;
         v->scale_u = 1.0f;
#else
         // (note) plane dist calculated/updated later on in edgeAAEndPath()
         /* #define DMIN 0.005  */
         #define DMIN 0.02
         v->scale_s = (1.0f / DMIN);
         v->scale_t = (1.0f / DMIN);
         v->scale_u = (1.0f / DMIN);
#endif
         /* yac_host->printf("xxx edgeAAAddVertex: p=(%f; %f) c32=#%08x\n", _x, _y, _c32); */
      }
      else
      {
         Dprintf("[~~~] MINNIE_EDGEAA_MAX_VERTICES_PER_PATH(%u) exceeded\n", MINNIE_EDGEAA_MAX_VERTICES_PER_PATH);
      }
   }

   static inline sBool edgeAAIsSharedEdge(const VertexAA *vC1, const VertexAA *vC2,
                                          const VertexAA *vO1, const VertexAA *vO2
                                          ) {
      return
         (vC1->x == vO1->x) && (vC1->y == vO1->y) &&
         (vC2->x == vO2->x) && (vC2->y == vO2->y)
         ;
   }

   static void edgeAAEndPath(void) {
      if(edgeaa_vertices_num >= 3u)
      {
         if(NULL != loc_vb_export_ofs)
         {
            // Find shared edges
#if MINNIE_EDGEAA_FIND_SHARED
            const sUI numTris = edgeaa_vertices_num / 3u;
            VertexAA *vC = edgeaa_vertices;
            for(sUI triIdxC = 0u; triIdxC < numTris; triIdxC++)
            {
               /* vC->b_edge_bc = 0; */
               /* vC->b_edge_ca = 0; */
               /* vC->b_edge_ab = 0; */
               /* vC->b_all_shared = 0; */

               VertexAA *vO = edgeaa_vertices;
               for(sUI triIdxO = 0u; triIdxO < numTris; triIdxO++)
               {
                  /* if(vO->b_all_shared) */
                  /* { */
                  /*    Dprintf("xxx vO->b_all_shared=%d\n", vO->b_all_shared); */
                  /* } */
                  if(triIdxO != triIdxC)
                  /* if(triIdxO != triIdxC && !vO->b_all_shared) */
                  {
                     // opposite from vtx A ('s')
                     if(!vC->b_edge_bc)
                     {
                        /* vC->b_edge_bc = */
                        /*    edgeAAIsSharedEdge(vC+1, vC+2, vO+0, vO+1) || */
                        /*    edgeAAIsSharedEdge(vC+1, vC+2, vO+1, vO+0) || */
                        /*    edgeAAIsSharedEdge(vC+1, vC+2, vO+1, vO+2) || */
                        /*    edgeAAIsSharedEdge(vC+1, vC+2, vO+2, vO+1) || */
                        /*    edgeAAIsSharedEdge(vC+1, vC+2, vO+2, vO+0) || */
                        /*    edgeAAIsSharedEdge(vC+1, vC+2, vO+0, vO+2) ; */

                        if(edgeAAIsSharedEdge(vC+1, vC+2, vO+0, vO+1))
                        {
                           vC->b_edge_bc = 1;
                           vO->b_edge_ab = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+1, vC+2, vO+1, vO+0))
                        {
                           vC->b_edge_bc = 1;
                           vO->b_edge_ab = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+1, vC+2, vO+1, vO+2))
                        {
                           vC->b_edge_bc = 1;
                           vO->b_edge_bc = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+1, vC+2, vO+2, vO+1))
                        {
                           vC->b_edge_bc = 1;
                           vO->b_edge_bc = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+1, vC+2, vO+2, vO+0))
                        {
                           vC->b_edge_bc = 1;
                           vO->b_edge_ca = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+1, vC+2, vO+0, vO+2))
                        {
                           vC->b_edge_bc = 1;
                           vO->b_edge_ca = 1;
                        }
                     }

                     // opposite from vtx B ('u')
                     if(!vC->b_edge_ca)
                     {
                        /* vC->b_edge_ca = */
                        /*    edgeAAIsSharedEdge(vC+2, vC+0, vO+0, vO+1) || */
                        /*    edgeAAIsSharedEdge(vC+2, vC+0, vO+1, vO+0) || */
                        /*    edgeAAIsSharedEdge(vC+2, vC+0, vO+1, vO+2) || */
                        /*    edgeAAIsSharedEdge(vC+2, vC+0, vO+2, vO+1) || */
                        /*    edgeAAIsSharedEdge(vC+2, vC+0, vO+2, vO+0) || */
                        /*    edgeAAIsSharedEdge(vC+2, vC+0, vO+0, vO+2) ; */
                        if(edgeAAIsSharedEdge(vC+2, vC+0, vO+0, vO+1))
                        {
                           vC->b_edge_ca = 1;
                           vO->b_edge_ab = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+2, vC+0, vO+1, vO+0))
                        {
                           vC->b_edge_ca = 1;
                           vO->b_edge_ab = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+2, vC+0, vO+1, vO+2))
                        {
                           vC->b_edge_ca = 1;
                           vO->b_edge_bc = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+2, vC+0, vO+2, vO+1))
                        {
                           vC->b_edge_ca = 1;
                           vO->b_edge_bc = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+2, vC+0, vO+2, vO+0))
                        {
                           vC->b_edge_ca = 1;
                           vO->b_edge_ca = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+2, vC+0, vO+0, vO+2))
                        {
                           vC->b_edge_ca = 1;
                           vO->b_edge_ca = 1;
                        }
                     }

                     // opposite from vtx C ('t')
                     if(!vC->b_edge_ab)
                     {
                        /* vC->b_edge_ab = */
                        /*    edgeAAIsSharedEdge(vC+0, vC+1, vO+0, vO+1) || */
                        /*    edgeAAIsSharedEdge(vC+0, vC+1, vO+1, vO+0) || */
                        /*    edgeAAIsSharedEdge(vC+0, vC+1, vO+1, vO+2) || */
                        /*    edgeAAIsSharedEdge(vC+0, vC+1, vO+2, vO+1) || */
                        /*    edgeAAIsSharedEdge(vC+0, vC+1, vO+2, vO+0) || */
                        /*    edgeAAIsSharedEdge(vC+0, vC+1, vO+0, vO+2) ; */
                        if(edgeAAIsSharedEdge(vC+0, vC+1, vO+0, vO+1))
                        {
                           vC->b_edge_ab = 1;
                           vO->b_edge_ab = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+0, vC+1, vO+1, vO+0))
                        {
                           vC->b_edge_ab = 1;
                           vO->b_edge_ab = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+0, vC+1, vO+1, vO+2))
                        {
                           vC->b_edge_ab = 1;
                           vO->b_edge_bc = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+0, vC+1, vO+2, vO+1))
                        {
                           vC->b_edge_ab = 1;
                           vO->b_edge_bc = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+0, vC+1, vO+2, vO+0))
                        {
                           vC->b_edge_ab = 1;
                           vO->b_edge_ca = 1;
                        }
                        else if(edgeAAIsSharedEdge(vC+0, vC+1, vO+0, vO+2))
                        {
                           vC->b_edge_ab = 1;
                           vO->b_edge_ca = 1;
                        }
                     }

                  }
                  // Next triangle
                  vO += 3;
               }

               // Replicate edge-flags (skip ?!)
               vC[1].b_edge_bc = vC[0].b_edge_bc;
               vC[2].b_edge_bc = vC[0].b_edge_bc;
               vC[1].b_edge_ca = vC[0].b_edge_ca;
               vC[2].b_edge_ca = vC[0].b_edge_ca;
               vC[1].b_edge_ab = vC[0].b_edge_ab;
               vC[2].b_edge_ab = vC[0].b_edge_ab;

               /* vC->b_all_shared = vC->b_edge_bc && vC->b_edge_ca && vC->b_edge_ab; */

               // Next triangle
               vC += 3;
            }
#endif // MINNIE_EDGEAA_FIND_SHARED

#if MINNIE_EDGEAA_CALC_FACTORS
            // Calc barycentric correction factors ('scale s|t|u')
            //  (note) based on 'edge_aa_planedist.tks'
            {
               VertexAA *vAA = edgeaa_vertices;
               const sF32 scaleY = aspect * (1.0f / 1024.0f);
#if 0
               const sF32 aaBorder = 0.5f;  // (todo) make configurable
#else
               const sF32 aaBorder = 0.5f;  // (todo) make configurable
#endif
               const sF32 aaBorderInv = (1.0f / aaBorder);
               for(sUI vtxIdx = 0u; vtxIdx < edgeaa_vertices_num; vtxIdx += 3u)
               {
                  Vector2f vA; vA.init(vAA[0].x*(1.0f/1024.0f), vAA[0].y*scaleY);
                  Vector2f vB; vB.init(vAA[1].x*(1.0f/1024.0f), vAA[1].y*scaleY);
                  Vector2f vC; vC.init(vAA[2].x*(1.0f/1024.0f), vAA[2].y*scaleY);

#if 0
                  vA.init(-80+200, -10+200); // xxxxxxxxxxx
                  vB.init( 60+200, -90+200);
                  vC.init( 90+200,  70+200);
#endif

                  // Scale triangle by aaBorder width
                  //  (todo) REMOVE
                  if(0)
                  {
                     Vector2f vABP = vB; vABP.sub(&vA); vABP.rotateCW90(); vABP.unitScale(aaBorder);
                     Vector2f vBCP = vC; vBCP.sub(&vB); vBCP.rotateCW90(); vBCP.unitScale(aaBorder);
                     Vector2f vCAP = vA; vCAP.sub(&vC); vCAP.rotateCW90(); vCAP.unitScale(aaBorder);

                     Vector2f vA1 = vA; vA1.add(&vABP);
                     Vector2f vA2 = vB; vA2.add(&vABP);
                     Vector2f vB1 = vB; vB1.add(&vBCP);
                     Vector2f vB2 = vC; vB2.add(&vBCP);
                     Vector2f vC1 = vC; vC1.add(&vCAP);
                     Vector2f vC2 = vA; vC2.add(&vCAP);

                     sF32 is;
                     Vector2f vISAB;
                     Vector2f vISBC;
                     Vector2f vISCA;

                     is = vISAB.intersect(&vA1, &vA2, &vB1, &vB2, true/*bExtrapolate*/);
                     // Dprintf("xxx is vISAB = %f\n", is);
                     is = vISBC.intersect(&vB1, &vB2, &vC1, &vC2, true/*bExtrapolate*/);
                     // Dprintf("xxx is vISBC = %f\n", is);
                     is = vISCA.intersect(&vC1, &vC2, &vA1, &vA2, true/*bExtrapolate*/);
                     // Dprintf("xxx is vISCA = %f\n", is);
                     (void)is;

                     // Dprintf("xxx vA=(%f;%f) vB=(%f;%f) vC=(%f;%f)\n", vA.x, vA.y, vB.x, vB.y, vC.x, vC.y);
                     // Dprintf("xxx   => vA'=(%f;%f) vB'=(%f;%f) vC'=(%f;%f)\n", vISCA.x, vISCA.y, vISAB.x, vISAB.y, vISBC.x, vISBC.y);

                     vA = vISCA;
                     vB = vISAB;
                     vC = vISBC;
                  } // if(0)

                  if(0)
                  {
                     vAA[0].x = sS32(vA.x * 1024.0f);
                     vAA[0].y = sS32(vA.y * 1024.0f);

                     vAA[1].x = sS32(vB.x * 1024.0f);
                     vAA[1].y = sS32(vB.y * 1024.0f);

                     vAA[2].x = sS32(vC.x * 1024.0f);
                     vAA[2].y = sS32(vC.y * 1024.0f);
                  }

                  // Calc distances to edge planes
                  //  (todo) optimize/remove sABS() (winding)
                  Vector2f vN;

                  // dist A<>BC ('s')
                  vN.init(vC.x, vC.y);
                  vN.x -= vB.x;
                  vN.y -= vB.y;
                  vN.rotateCW90();
                  sF32 dBC = vA.distanceToPlane(&vB, &vN);
                  dBC = sABS(dBC);

                  // dist B<>CA ('u')
                  vN.init(vA.x, vA.y);
                  vN.x -= vC.x;
                  vN.y -= vC.y;
                  vN.rotateCW90();
                  sF32 dCA = vB.distanceToPlane(&vC, &vN);
                  dCA = sABS(dCA);

                  // dist C<>AB ('t')
                  vN.init(vB.x, vB.y);
                  vN.x -= vA.x;
                  vN.y -= vA.y;
                  vN.rotateCW90();
                  sF32 dAB = vC.distanceToPlane(&vB, &vN);
                  dAB = sABS(dAB);

                  Dedgeaaprintfv("xxx vA=(%f;%f) vB=(%f;%f) vC=(%f;%f) => dBC=%f dCA=%f dAB=%f\n", vA.x, vA.y, vB.x, vB.y, vC.x, vC.y, dBC, dCA, dAB);

                  vAA[0].scale_s = dBC * aaBorderInv;
                  vAA[0].scale_t = dAB * aaBorderInv;
                  vAA[0].scale_u = dCA * aaBorderInv;

                  // (todo) skip ?
                  vAA[1].scale_s = vAA[0].scale_s;
                  vAA[1].scale_t = vAA[0].scale_t;
                  vAA[1].scale_u = vAA[0].scale_u;
                  vAA[2].scale_s = vAA[0].scale_s;
                  vAA[2].scale_t = vAA[0].scale_t;
                  vAA[2].scale_u = vAA[0].scale_u;

                  // Next triangle
                  vAA += 3;
               }
            }
#endif // MINNIE_EDGEAA_CALC_FACTORS

            // Export to stream
            sUI numSharedEdges = 0u;
            {
               const VertexAA *v = edgeaa_vertices;
#if MINNIE_EXPORT_TRIS_EDGEAA_BC
               // 30 bytes per vertex (with barycentric coordinates)
               static const sU8 aBarycentric[2*3] = {
                  0u, 0u,     // A  glPointCoord.st = (0.0, 0.0)  => stu=(1.0, 0.0, 0.0)  (u=a.s-a.t; t=a.t; s=1-a.s;)
                  255u, 0u,   // B                    (1.0, 0.0)         (0.0, 0.0, 1.0)
                  255u, 255u  // C                    (1.0, 1.0)         (0.0, 1.0, 0.0)
               };
#else
               // 28 bytes per vertex (procedural barycentric coordinates, requires OpenGL ES 3.x / gl_VertexID)
#endif // MINNIE_EXPORT_TRIS_EDGEAA_BC

               active_dl_num_tris  += edgeaa_vertices_num / 3u;
               active_dl_num_verts += edgeaa_vertices_num;  // not used by draw-triangles ops, counting it anyway

               for(sUI vtxIdx = 0u; vtxIdx < edgeaa_vertices_num; vtxIdx++)
               {
#if MINNIE_EXPORT_VERTEX_16BIT
                  Dexport_vb_i16( sS16(v->x * (4.0f/1024.0f)) );
                  Dexport_vb_i16( sS16(v->y * (4.0f/1024.0f)) );
#else
                  Dexport_vb_f32(v->x * (1.0f/1024.0f));
                  Dexport_vb_f32(v->y * (1.0f/1024.0f));
#endif // MINNIE_EXPORT_VERTEX_16BIT
                  Dexport_vb_i8(v->r);
                  Dexport_vb_i8(v->g);
                  Dexport_vb_i8(v->b);
                  Dexport_vb_i8(v->a);
                  Dexport_vb_i8(v->b_edge_bc/*s*/ ? 255u/*noAA*/ : 0u/*AA*/);
                  Dexport_vb_i8(v->b_edge_ca/*u*/ ? 255u/*noAA*/ : 0u/*AA*/);
                  Dexport_vb_i8(v->b_edge_ab/*t*/ ? 255u/*noAA*/ : 0u/*AA*/);
                  Dexport_vb_i8(0u);  // __pad1
#if MINNIE_EXPORT_VERTEX_16BIT && MINNIE_EXPORT_TRIS_EDGEAA_SCALE_16BIT
                  /* Dprintf("xxx v_scale s=%f t=%f u=%f\n", v->scale_s, v->scale_t, v->scale_u); */
                  Dexport_vb_i16( sS16(v->scale_s*1024.0f) );
                  Dexport_vb_i16( sS16(v->scale_t*1024.0f) );
                  Dexport_vb_i16( sS16(v->scale_u*1024.0f) );
#else
                  Dexport_vb_f32(v->scale_s);
                  Dexport_vb_f32(v->scale_t);
                  Dexport_vb_f32(v->scale_u);
#endif // MINNIE_EXPORT_VERTEX_16BIT
#if MINNIE_EXPORT_TRIS_EDGEAA_BC
                  sUI bcOff = (vtxIdx % 3u) << 1;
                  Dexport_vb_i8(aBarycentric[bcOff + 0u]);
                  Dexport_vb_i8(aBarycentric[bcOff + 1u]);
#endif // MINNIE_EXPORT_TRIS_EDGEAA_BC
                  if(b_debug_export)
                  {
                     if(v->b_edge_bc)
                        numSharedEdges++;
                     if(v->b_edge_ca)
                        numSharedEdges++;
                     if(v->b_edge_ab)
                        numSharedEdges++;
                  }

                  // Next vertex
                  v++;
               }
            }

            if(b_debug_export)
            {
               numSharedEdges /= 3u;
               Dedgeaaprintf("[trc] edgeAAEndPath: edgeaa_vertices_num=%u numSharedEdges=%u\n", edgeaa_vertices_num, numSharedEdges);
            }

         } // if loc_vb_export_ofs
      }
   }
#endif // MINNIE_EXPORT_TRIS_EDGEAA

   // <method.png>
   static void drawTri(const Vector2f *_v1, const Vector2f *_v2, const Vector2f *_v3, sU32 _c32, sU32 _c32Mask) {
      if(b_draw_tri)
      {
         if(b_debug_fill >= 2)
         {
            Dprintf("[trc] drawTri: v1=(%f;%f) v2=(%f; %f) v3=(%f; %f) c32=#%08x c32Mask=#%08x b_aa=%d\n", _v1->x, _v1->y, _v2->x, _v2->y, _v3->x, _v3->y, _c32, _c32Mask, b_aa);
         }
         /* return; // xxxxxxxxxx */

         /* Dprintf("xxx drawTri: b_edge_aa=%d\n", b_edge_aa); */
         if(!b_edge_aa)
         {
#if MINNIE_EXPORT_TRIS_SIMPLE
            active_dl_num_tris  += 1u;
            active_dl_num_verts += 3u;  // not used by draw-triangles ops, counting it anyway

#if MINNIE_EXPORT_VERTEX_16BIT
            // 8 bytes per vertex, 24 bytes per triangle
            Dexport_vb_i16(sS16(_v1->x * 4.0f));
            Dexport_vb_i16(sS16(_v1->y * 4.0f));
#else
            Dexport_vb_f32(_v1->x);
            Dexport_vb_f32(_v1->y);
#endif // MINNIE_EXPORT_VERTEX_16BIT

            if(!b_uniform_colors)
            {
               Dexport_vb_i8( (_c32 >> 16) & 255 );  // r
               Dexport_vb_i8( (_c32 >>  8) & 255 );  // g
               Dexport_vb_i8( (_c32      ) & 255 );  // b
               Dexport_vb_i8( (_c32 >> 24) & 255 );  // a
            }

#if MINNIE_EXPORT_VERTEX_16BIT
            Dexport_vb_i16(sS16(_v2->x * 4.0f));
            Dexport_vb_i16(sS16(_v2->y * 4.0f));
#else
            Dexport_vb_f32(_v2->x);
            Dexport_vb_f32(_v2->y);
#endif // MINNIE_EXPORT_VERTEX_16BIT

            if(!b_uniform_colors)
            {
               Dexport_vb_i8( (_c32 >> 16) & 255 );  // r
               Dexport_vb_i8( (_c32 >>  8) & 255 );  // g
               Dexport_vb_i8( (_c32      ) & 255 );  // b
               Dexport_vb_i8( (_c32 >> 24) & 255 );  // a
            }

#if MINNIE_EXPORT_VERTEX_16BIT
            Dexport_vb_i16( sS16(_v3->x * 4.0f) );
            Dexport_vb_i16( sS16(_v3->y * 4.0f) );
#else
            Dexport_vb_f32(_v3->x);
            Dexport_vb_f32(_v3->y);
#endif // MINNIE_EXPORT_VERTEX_16BIT

            if(!b_uniform_colors)
            {
               Dexport_vb_i8( (_c32 >> 16) & 255 );  // r
               Dexport_vb_i8( (_c32 >>  8) & 255 );  // g
               Dexport_vb_i8( (_c32      ) & 255 );  // b
               Dexport_vb_i8( (_c32 >> 24) & 255 );  // a
            }
         }
#endif // MINNIE_EXPORT_TRIS_SIMPLE
#if MINNIE_EXPORT_TRIS_EDGEAA
         else
         {
            // Check winding and flip if CCW
            Vector2f vAB; vAB.init(_v2->x - _v1->x, _v2->y - _v1->y);
            Vector2f vBC; vBC.init(_v3->x - _v1->x, _v3->y - _v1->y);
            sF32 z = vAB.crossZ(&vBC);
            if(z >= 0.0f)
            /* if(z < 0.0f) */
            {
               edgeAAAddVertex(_v1->x, _v1->y, _c32);
               edgeAAAddVertex(_v2->x, _v2->y, _c32);
               edgeAAAddVertex(_v3->x, _v3->y, _c32);
            }
            else
            {
               /* Dprintf("[trc] flipCCW vA=(%f;%f) vB=(%f;%f) vC=(%f;%f) => dot=%f\n", vA.x, vA.y, vB.x, vB.y, vC.x, vC.y, z); */
               edgeAAAddVertex(_v1->x, _v1->y, _c32);
               edgeAAAddVertex(_v3->x, _v3->y, _c32);
               edgeAAAddVertex(_v2->x, _v2->y, _c32);
            }
         }
#endif // MINNIE_EXPORT_TRIS_SIMPLE

#if MINNIE_SW_RENDER
         if(b_draw_sw_tri)
         {
            if(b_aa)
            {
               TriRasterAAVec2f v1, v2, v3;
               v1.x = _v1->x; v1.y = _v1->y;
               v2.x = _v2->x; v2.y = _v2->y;
               v3.x = _v3->x; v3.y = _v3->y;

               const sUI numSamples = 16u;
               // (todo) support pitch
               renderTriToBitmap(&v1, &v2, &v3, _c32, &cur_fb->tri_aa_bitmap, numSamples, (numSamples>0)?2:0/*aaFixMode*/, aa_bias);
            }
            else
            {
               // Convert Vector2f to fixed point
               sU32 va[2*3];

               va[0*2+0] = sUI(_v1->x * FX_COORD_SCL_HSPAN);
               va[0*2+1] = sUI(_v1->y * FX_COORD_SCL_HSPAN);

               va[1*2+0] = sUI(_v2->x * FX_COORD_SCL_HSPAN);
               va[1*2+1] = sUI(_v2->y * FX_COORD_SCL_HSPAN);

               va[2*2+0] = sUI(_v3->x * FX_COORD_SCL_HSPAN);
               va[2*2+1] = sUI(_v3->y * FX_COORD_SCL_HSPAN);

               draw_tri_hspan(cur_fb->pixels.u32,
                              cur_fb->pitch>>2/*fbW=bytes to c32 px*/,
                              cur_fb->h,
                              va,
                              _c32,
                              _c32Mask
                              );
            }
         } // b_draw_sw_tri
#endif // MINNIE_SW_RENDER
      }
      total_num_tris++;
   }

   // <method.png>
   static void drawPathFillConvex(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConvex: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      /* if(b_debug_fill) trace "[trc] drawPathFillConvex: vertices:"; */
      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      FloatArray *va = &p->points;
      if(va->num_elements >= (3u*2u))
      {
         v1.init(va->get(0) + cur_x, va->get(1) + cur_y);
         v2.init(va->get(2) + cur_x, va->get(3) + cur_y);
         v1.mulf(geo_scale);
         v2.mulf(geo_scale);

         sUI vaOff = 4u;
         while(vaOff < va->num_elements)
         {
            v3.init(va->get(vaOff+0u) + cur_x, va->get(vaOff+1u) + cur_y);
            v3.mulf(geo_scale);

            drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

            v2 = v3;
            vaOff += 2u;
         }
      }
   }

   // <method.png>
   static void drawPathFillConvexClip(Path *p, const FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConvexClip: path_idx=%u #points=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, p->points.num_elements/2u, cur_pal_idx, cur_c32, cur_mask_idx); }

      // if(b_debug_fill >= 2) p->points.debugPrint2("[trc] drawPathFillConvexClip: points=");

      FloatArray *vaPoints = &tmpfa_points2;
      translateAndScalePoints(&p->points, vaPoints/*out*/);

      FloatArray *vaClip = &tmpfa_clip2;
      Clip2D(vaPoints, _vaClip, vaClip/*out*/);

      // Dprintf("xxx vaClip->num_elements=%u\n", vaClip->num_elements);

      /* if(b_debug_fill) trace "[trc] drawPathFillConvexClip: vertices:"; */
      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      if(vaClip->num_elements >= (3u*2u))
      {
         const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

         v1.init(vaClip->get(0), vaClip->get(1));
         v2.init(vaClip->get(2), vaClip->get(3));

         sUI vaOff = 4u;
         while(vaOff < vaClip->num_elements)
         {
            v3.init(vaClip->get(vaOff+0u),
                    vaClip->get(vaOff+1u)
                    );

            drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

            v2 = v3;
            vaOff += 2u;
         }
      }

   }

   // <method.png>
   static void drawPathFillConvexTransform2d(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConvexTransform2d: path_idx=%u #points=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, p->points.num_elements/2u, cur_pal_idx, cur_c32, cur_mask_idx); }

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      /* if(b_debug_fill) trace "[trc] drawPathFillConvexTransform2d: vertices:"; */
      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      const FloatArray *va = &p->points;
      if(va->num_elements >= (3u*2u))
      {
         v1.init(va->get(0), va->get(1));
         cur_mat_2d.mulv(&v1);
         v1.x = v1.x + cur_x;
         v1.y = v1.y + cur_y;
         v1.mulf(geo_scale);

         v2.init(va->get(2), va->get(3));
         cur_mat_2d.mulv(&v2);
         v2.x = v2.x + cur_x;
         v2.y = v2.y + cur_y;
         v2.mulf(geo_scale);

         sUI vaOff = 4u;
         while(vaOff < va->num_elements)
         {
            v3.init(va->get(vaOff+0u), va->get(vaOff+1u));
            cur_mat_2d.mulv(&v3);
            v3.x = v3.x + cur_x;
            v3.y = v3.y + cur_y;
            v3.mulf(geo_scale);

            drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

            v2 = v3;
            vaOff += 2u;
         }
      }

   }

   // <method.png>
   static void drawPathFillConvexTransform2dClip(Path *p, const FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConvexTransform2dClip: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      transform2DAndTranslateAndScale(&p->points, vaPoints);

      FloatArray *vaClip = &tmpfa_clip2;
      Clip2D(vaPoints, _vaClip, vaClip/*out*/);

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      /* if(b_debug_fill) trace "[trc] drawPathFillConvexTransform2d: vertices:"; */
      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      if(vaClip->num_elements >= (3u*2u))
      {
         v1.init(vaClip->get(0), vaClip->get(1));
         v2.init(vaClip->get(2), vaClip->get(3));

         sUI vaClipOff = 4u;
         while(vaClipOff < vaClip->num_elements)
         {
            v3.init(vaClip->get(vaClipOff+0u), vaClip->get(vaClipOff+1u));

            drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

            v2 = v3;
            vaClipOff += 2u;
         }
      }
   }

   // <method.png>
   static void drawPathFillConvexTransform3d(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConvexTransform3d: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      /* if(b_debug_fill) trace "[trc] drawPathFillConvexTransform3d: vertices:"; */
      Vector4f v1;
      Vector4f v2;
      Vector4f v3;
      Vector2f v1s;
      Vector2f v2s;
      Vector2f v3s;
      const FloatArray *va = &p->points;
      if(va->num_elements >= (3u*2u))
      {
         v1.init(va->get(0), va->get(1), 0.0f, 1.0f);
         cur_mat_3d.mulv(&v1);
         v1.divzTo(&v1s);
         v1s.x = v1s.x + cur_x;
         v1s.y = v1s.y + cur_y;
         v1s.mulf(geo_scale);

         v2.init(va->get(2), va->get(3), 0.0f, 1.0f);
         cur_mat_3d.mulv(&v2);
         v2.divzTo(&v2s);
         v2s.x = v2s.x + cur_x;
         v2s.y = v2s.y + cur_y;
         v2s.mulf(geo_scale);

         sUI vaOff = 4u;
         while(vaOff < va->num_elements)
         {
            v3.init(va->get(vaOff+0u), va->get(vaOff+1u), 0.0f, 1.0f);
            cur_mat_3d.mulv(&v3);
            v3.divzTo(&v3s);
            v3s.x = v3s.x + cur_x;
            v3s.y = v3s.y + cur_y;
            v3s.mulf(geo_scale);

            drawTri(&v1s, &v2s, &v3s, cur_c32, c32Mask);

            v2s = v3s;
            vaOff += 2u;
         }
      }
   }

   // <method.png>
   static void drawPathFillConvexTransform3dClip(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConvexTransform3dClip: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      // Transform points from 2D to 3D
      const FloatArray *vaPoints = &p->points;
      FloatArray *va3 = &tmpfa_va3;
      va3->empty();
      if(vaPoints->num_elements >= (3u*2u))
      {
         Vector4f v1;
         Vector4f v2;
         Vector4f v3;

         v1.init(vaPoints->get(0), vaPoints->get(1), 0.0f, 1.0f);
         v2.init(vaPoints->get(2), vaPoints->get(3), 0.0f, 1.0f);

         cur_mat_3d.mulv(&v1);
         cur_mat_3d.mulv(&v2);

         va3->add3(v1.x, v1.y, v1.z);
         va3->add3(v2.x, v2.y, v2.z);

         /* if(b_debug_fill >= 3) trace "xxx drawPathFillConvexTransform3dClip: v1="+v1+" cur_mat_3d="+cur_mat_3d; */
         /* if(b_debug_fill >= 3) trace "xxx drawPathFillConvexTransform3dClip: v1_3d="+v1; */

         sUI vaOff = 4u;
         while(vaOff < vaPoints->num_elements)
         {
            v3.init(vaPoints->get(vaOff+0u), vaPoints->get(vaOff+1u), 0.0f, 1.0f);
            cur_mat_3d.mulv(&v3);
            va3->add3(v3.x, v3.y, v3.z);

            // Next 2D point
            vaOff += 2u;
         }

         // Clip 3D points against near plane
         FloatArray *vaClip3 = &tmpfa_clip3;
         Clip3DZNear(va3, 0.01f/*znear*/, vaClip3);

         // Project to 2D
         FloatArray *vaProj = &tmpfa_proj2;
         vaProj->empty();
         vaOff = 0u;
         const sUI vaClip3Num = vaClip3->num_elements / 3;
         for(sUI loopVaClip3Idx = 0u; loopVaClip3Idx < vaClip3Num; loopVaClip3Idx++)
         {
            Vector3f vProj;
            vProj.init(vaClip3->get(vaOff+0u),
                       vaClip3->get(vaOff+1u),
                       vaClip3->get(vaOff+2u)
                       );

            Vector2f vProj2;
            vProj.divzTo(&vProj2);

            vaProj->add2( (vProj2.x + cur_x) * geo_scale,
                          (vProj2.y + cur_y) * geo_scale
                          );

            // Next clipped 3D vertex
            vaOff += 3u;
         }

         // Clip 2D
         const Path *pFB = updateAndGetPathFb();
         FloatArray *va = &tmpfa_clip2;
         va->empty();
         Clip2D(vaProj, &pFB->points, va);

         if(va->num_elements >= (3u*2u))
         {
            const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

            Vector2f v1s;
            Vector2f v2s;
            v1s.init(va->get(0), va->get(1));
            v2s.init(va->get(2), va->get(3));

            vaOff = 4u;
            /* sUI triIdx = 0u; */
            while(vaOff < va->num_elements)
            {
               Vector2f v3s;
               v3s.init(va->get(vaOff+0u), va->get(vaOff+1u));

               /* if(b_debug_fill >= 2) */
               /*    c32 = palette.get( 1 + (triIdx & 3) ); */

               drawTri(&v1s, &v2s, &v3s, cur_c32, c32Mask);

               v2s = v3s;
               vaOff += 2u;
               /* triIdx++; */
            }
         } // if #va >= 3

      } // if #points >= 3

   }

   // <method.png>
   static void drawPathFillConcave(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConcave: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      /* Dprintf("xxx drawPathFillConcave: b_tesselate_concave=%d b_edge_aa=%d\n", b_tesselate_concave, b_edge_aa); */
      if( !(b_tesselate_concave || b_edge_aa) && (NULL != loc_vb_export_ofs) )
      {
         if( !(active_dl_op >= MINNIE_DRAWOP_POLYGON_FILL_FLAT_32) || !(active_dl_op <= MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_14_2) )
         {
            Dprintf("[!!!] Minnie::drawPathFillConcave: INTERNAL ERROR: active_dl_op(0x%02x) != MINNIE_DRAWOP_POLYGON_FILL_*\n", active_dl_op);
         }
         p->exportConcaveVertices(loc_vb_export_ofs, &p->points, cur_c32, cur_x, cur_y, geo_scale, b_uniform_colors);
         active_dl_num_verts += p->points.num_elements >> 1u;
         return;
      }

      // (note) vertices remain unchanged, generates p.ia_fill
      if(!p->tesselateConcave(&p->points, &tmpia_tesselate_concave))
      {
         Derrorprintf("[---] drawPathFillConcave: tesselateConcave() failed\n");
         return;
      }

      const FloatArray *va = &p->points;
      const IntArray *ia = &p->ia_fill;

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         /* if(-1 == debug_tri_idx || debug_tri_idx == triIdx) */
         {
            sSI vaOff1 = ia->get(idxOff+0u)*2;
            sSI vaOff2 = ia->get(idxOff+1u)*2;
            sSI vaOff3 = ia->get(idxOff+2u)*2;
            v1.init(va->get(vaOff1+0) + cur_x, va->get(vaOff1+1) + cur_y);
            v2.init(va->get(vaOff2+0) + cur_x, va->get(vaOff2+1) + cur_y);
            v3.init(va->get(vaOff3+0) + cur_x, va->get(vaOff3+1) + cur_y);
            v1.mulf(geo_scale);
            v2.mulf(geo_scale);
            v3.mulf(geo_scale);

            /* if(b_debug_fill >= 2) */
            /*    c32 = palette.get( 1 + (triIdx & 3) ); */

            drawTri(&v1, &v2, &v3, cur_c32, c32Mask);
         }
         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathFillEvenOdd(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillEvenOdd: path_idx=%u #points=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, p->points.num_elements/2u, cur_pal_idx, cur_c32, cur_mask_idx); }

      // (note) iterates parent+sub paths, generate vaPoints and p.ia_fill
      FloatArray *vaPoints = &tmpfa_points2;
      if(!p->tesselateEvenOdd(&p->points/*in*/, vaPoints/*out*/))
      {
         if(b_debug_tess) { Derrorprintf("[---] drawPathFillEvenOdd: tesselateEvenOdd() failed\n"); }
         return;
      }

      const FloatArray *va = vaPoints;
      const IntArray   *ia = &p->ia_fill;

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         /* if(-1 == debug_tri_idx || debug_tri_idx == triIdx) */
         {
            sSI vaOff1 = ia->get(idxOff+0u)*2;
            sSI vaOff2 = ia->get(idxOff+1u)*2;
            sSI vaOff3 = ia->get(idxOff+2u)*2;
            /* printf("xxx vaOff1=%d vaOff2=%d vaOff3=%d va->num_elements=%u\n", vaOff1, vaOff2, vaOff3, va->max_elements); */

            v1.init(va->get(vaOff1+0) + cur_x, va->get(vaOff1+1) + cur_y);
            v2.init(va->get(vaOff2+0) + cur_x, va->get(vaOff2+1) + cur_y);
            v3.init(va->get(vaOff3+0) + cur_x, va->get(vaOff3+1) + cur_y);
            v1.mulf(geo_scale);
            v2.mulf(geo_scale);
            v3.mulf(geo_scale);

            /* if(b_debug_fill >= 2) */
            /*    c32 = palette.get( 1 + (triIdx & 3) ); */

            drawTri(&v1, &v2, &v3, cur_c32, c32Mask);
         }
         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
      /* printf("xxx drawPathFillEvenodd: LEAVE\n"); */
   }

   // <method.png>
   static void drawPathFillConcaveClipPre(Path *p, FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConcaveClipPre: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      translateAndScalePoints(&p->points, vaPoints/*out*/);

      FloatArray *va = &tmpfa_clip2;
      Clip2D(vaPoints, _vaClip, va);

      if( !(b_tesselate_concave || b_edge_aa) && (NULL != loc_vb_export_ofs) )
      {
         if( !(active_dl_op >= MINNIE_DRAWOP_POLYGON_FILL_FLAT_32) || !(active_dl_op <= MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_14_2) )
         {
            Dprintf("[!!!] Minnie::drawPathFillConcaveClipPre: INTERNAL ERROR: active_dl_op(0x%02x) != MINNIE_DRAWOP_POLYGON_FILL_*\n", active_dl_op);
         }
         // (note) translation + scaling already applied in translateAndScalePoints()
         p->exportConcaveVertices(loc_vb_export_ofs, va, cur_c32, 0.0f/*tx*/, 0.0f/*ty*/, 1.0f/*geoScale*/, b_uniform_colors);
         active_dl_num_verts += va->num_elements >> 1u;
         return;
      }

      // (note) vertices remain unchanged, generates p.ia_fill
      if(!p->tesselateConcave(va, &tmpia_tesselate_concave))
      {
         Derrorprintf("[---] drawPathFillConcaveClipPre: tesselateConcave() failed\n");
         return;
      }

      IntArray *ia = &p->ia_fill;

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         /* if(-1 == debug_tri_idx || debug_tri_idx == triIdx) */
         {
            sUI vaOff1 = sUI(ia->get(idxOff+0))*2u;
            sUI vaOff2 = sUI(ia->get(idxOff+1))*2u;
            sUI vaOff3 = sUI(ia->get(idxOff+2))*2u;
            v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u));
            v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u));
            v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u));

            /* if(b_debug_fill >= 2) */
            /*    c32 = palette.get( 1 + (triIdx & 3) ); */

            drawTri(&v1, &v2, &v3, cur_c32, c32Mask);
         }
         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathFillEvenOddClipPre(Path *p, FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillEvenOddClipPre: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      translateAndScalePoints(&p->points, vaPoints/*out*/);

      FloatArray *va = &tmpfa_clip2;
      Clip2D(vaPoints, _vaClip, va);

      // (note) iterates parent+sub paths, replaces p->points, generates p->ia_fill
      if(!p->tesselateEvenOdd(va/*in*/, va/*out*/))
      {
         if(b_debug_tess) { Derrorprintf("[---] drawPathFillEvenOddClipPre: tesselateEvenOdd() failed\n"); }
         return;
      }

      IntArray *ia = &p->ia_fill;

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         /* if(-1 == debug_tri_idx || debug_tri_idx == triIdx) */
         {
            sUI vaOff1 = sUI(ia->get(idxOff+0))*2u;
            sUI vaOff2 = sUI(ia->get(idxOff+1))*2u;
            sUI vaOff3 = sUI(ia->get(idxOff+2))*2u;
            v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u));
            v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u));
            v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u));

            /* if(b_debug_fill >= 2) */
            /*    c32 = palette.get( 1 + (triIdx & 3) ); */

            drawTri(&v1, &v2, &v3, cur_c32, c32Mask);
         }
         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathIndexedTris2D(const FloatArray *va, const IntArray *ia) {
      /* if(b_debug_fill) { Dprintf("[dbg] drawPathIndexedTris2D: path_idx=%u pal_idx=%u cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_mask_idx); } */

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0u))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1u))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2u))*2u;
         v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u));
         v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u));
         v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u));
         /* if(b_debug_extrude >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathFillConcaveClip(Path *p, FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConcaveClip: path_idx=%u pal_idx=%u cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      translateAndScalePoints(&p->points, vaPoints/*out*/);

      // (note) vertices remain unchanged, generates p.ia_fill
      if(!p->tesselateConcave(vaPoints, &tmpia_tesselate_concave))
      {
         Derrorprintf("[---] drawPathFillConcaveClip: tesselateConcave() failed\n");
         return;
      }

      // (note) appends helper indices/vertices to vaPoints
      IntArray *ia = &tmpia_clip2; ia->empty();
      ClipIndexedTris2D(vaPoints, &p->ia_fill, _vaClip, ia/*out*/);

      drawPathIndexedTris2D(vaPoints, ia);
   }

   // <method.png>
   static void drawPathFillEvenOddClip(Path *p, FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillEvenOddClip: path_idx=%u pal_idx=%u cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      translateAndScalePoints(&p->points, vaPoints/*out*/);

      // (note) vertices remain unchanged, generates p.ia_fill
      if(!p->tesselateEvenOdd(vaPoints/*in*/, vaPoints/*out*/))
      {
         if(b_debug_tess) { Derrorprintf("[---] drawPathFillEvenOddClip: tesselateEvenOdd() failed\n"); }
         return;
      }

      // (note) appends helper indices/vertices to vaPoints
      IntArray *ia = &tmpia_clip2; ia->empty();
      ClipIndexedTris2D(vaPoints, &p->ia_fill, _vaClip, ia/*out*/);

      drawPathIndexedTris2D(vaPoints, ia);
   }

   // <method.png>
   static void drawPathFillConcaveTransform2d(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConcaveTransform2d: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      if( !(b_tesselate_concave || b_edge_aa) && (NULL != loc_vb_export_ofs) )
      {
         if( !(active_dl_op >= MINNIE_DRAWOP_POLYGON_FILL_FLAT_32) || !(active_dl_op <= MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_14_2) )
         {
            Dprintf("[!!!] Minnie::drawPathFillConcaveTransform2d: INTERNAL ERROR: active_dl_op(0x%02x) != MINNIE_DRAWOP_POLYGON_FILL_*\n", active_dl_op);
         }
         p->exportConcaveVerticesTransform2d(loc_vb_export_ofs, &p->points, cur_c32, &cur_mat_2d, cur_x, cur_y, geo_scale, b_uniform_colors);
         active_dl_num_verts += p->points.num_elements >> 1u;
         return;
      }

      // (note) vertices remain unchanged, generates p.ia_fill
      if(!p->tesselateConcave(&p->points, &tmpia_tesselate_concave))
      {
         Derrorprintf("[---] drawPathFillConcaveTransform2d: tesselateConcave() failed\n");
         return;
      }

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      FloatArray *va = &p->points;
      IntArray *ia = &p->ia_fill;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0u))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1u))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2u))*2u;
         v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u));
         v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u));
         v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u));
         cur_mat_2d.mulv(&v1);
         cur_mat_2d.mulv(&v2);
         cur_mat_2d.mulv(&v3);
         v1.x = v1.x + cur_x;
         v1.y = v1.y + cur_y;
         v2.x = v2.x + cur_x;
         v2.y = v2.y + cur_y;
         v3.x = v3.x + cur_x;
         v3.y = v3.y + cur_y;
         v1.mulf(geo_scale);
         v2.mulf(geo_scale);
         v3.mulf(geo_scale);

         /* if(b_debug_fill >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathFillEvenOddTransform2d(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillEvenOddTransform2d: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      // (note) iterates parent+sub paths, replaces p->points, generates p->ia_fill
      FloatArray *vaPoints = &tmpfa_points2;
      if(!p->tesselateEvenOdd(&p->points/*in*/, vaPoints/*out*/))
      {
         if(b_debug_tess) { Derrorprintf("[---] drawPathFillEvenOddTransform2d: tesselateEvenOdd() failed\n"); }
         return;
      }

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      FloatArray *va = vaPoints;
      IntArray   *ia = &p->ia_fill;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0u))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1u))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2u))*2u;
         v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u));
         v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u));
         v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u));
         cur_mat_2d.mulv(&v1);
         cur_mat_2d.mulv(&v2);
         cur_mat_2d.mulv(&v3);
         v1.x = v1.x + cur_x;
         v1.y = v1.y + cur_y;
         v2.x = v2.x + cur_x;
         v2.y = v2.y + cur_y;
         v3.x = v3.x + cur_x;
         v3.y = v3.y + cur_y;
         v1.mulf(geo_scale);
         v2.mulf(geo_scale);
         v3.mulf(geo_scale);

         /* if(b_debug_fill >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathFillConcaveTransform2dClipPre(Path *p, const FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConcaveTransform2dClipPre: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      transform2DAndTranslateAndScale(&p->points, vaPoints);

      FloatArray *vaClip = &tmpfa_clip2;
      Clip2D(vaPoints, _vaClip, vaClip/*out*/);

      if( !(b_tesselate_concave || b_edge_aa) && (NULL != loc_vb_export_ofs) )
      {
         if( !(active_dl_op >= MINNIE_DRAWOP_POLYGON_FILL_FLAT_32) || !(active_dl_op <= MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_14_2) )
         {
            Dprintf("[!!!] Minnie::drawPathFillConcaveTransform2dClipPre: INTERNAL ERROR: active_dl_op(0x%02x) != MINNIE_DRAWOP_POLYGON_FILL_*\n", active_dl_op);
         }
         // (note) translation and scaling already applied in transform2DAndTranslateAndScale()
         p->exportConcaveVertices(loc_vb_export_ofs, vaClip, cur_c32, cur_x, cur_y, geo_scale, b_uniform_colors);
         active_dl_num_verts += vaClip->num_elements >> 1u;
         return;
      }

      // (note) vertices remain unchanged, generates p.ia_fill
      if(!p->tesselateConcave(vaClip, &tmpia_tesselate_concave))
      {
         Derrorprintf("[---] drawPathFillConcaveTransform2dClipPre: tesselateConcave() failed\n");
         return;
      }

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      FloatArray *va = vaClip;
      IntArray *ia = &p->ia_fill;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0u))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1u))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2u))*2u;
         v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u));
         v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u));
         v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u));

         /* if(b_debug_fill >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathFillEvenOddTransform2dClipPre(Path *p, const FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillEvenOddTransform2dClipPre: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      transform2DAndTranslateAndScale(&p->points, vaPoints);

      FloatArray *vaClip = &tmpfa_clip2;
      Clip2D(vaPoints, _vaClip, vaClip/*out*/);

      // (note) iterates parent+sub paths, replaces p->points, generates p->ia_fill
      if(!p->tesselateEvenOdd(vaClip/*in*/, vaClip/*out*/))
      {
         if(b_debug_tess) { Derrorprintf("[---] drawPathFillEvenOddTransform2dClipPre: tesselateEvenOdd() failed\n"); }
         return;
      }

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      FloatArray *va = vaClip;
      IntArray *ia = &p->ia_fill;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0u))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1u))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2u))*2u;
         v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u));
         v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u));
         v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u));

         /* if(b_debug_fill >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathFillConcaveTransform2dClip(Path *p, FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConcaveTransform2dClip: path_idx=%u pal_idx=%u cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      transform2DAndTranslateAndScale(&p->points, vaPoints/*out*/);

      // (note) vertices remain unchanged, generates p.ia_fill
      if(!p->tesselateConcave(vaPoints, &tmpia_tesselate_concave))
      {
         Derrorprintf("[---] drawPathFillConcaveTransform2dClip: tesselateConcave() failed\n");
         return;
      }

      // (note) appends helper indices/vertices to vaPoints
      IntArray *ia = &tmpia_clip2; ia->empty();
      ClipIndexedTris2D(vaPoints, &p->ia_fill, _vaClip, ia/*out*/);

      drawPathIndexedTris2D(vaPoints, ia);
   }

   // <method.png>
   static void drawPathFillEvenOddTransform2dClip(Path *p, FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillEvenOddTransform2dClip: path_idx=%u pal_idx=%u cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      transform2DAndTranslateAndScale(&p->points, vaPoints/*out*/);

      // (note) iterates parent+sub paths, replaces p->points, generates p->ia_fill
      if(!p->tesselateEvenOdd(vaPoints/*in*/, vaPoints/*out*/))
      {
         if(b_debug_tess) { Derrorprintf("[---] drawPathFillEvenOddTransform2dClip: tesselateEvenOdd() failed\n"); }
         return;
      }

      // (note) appends helper indices/vertices to vaPoints
      IntArray *ia = &tmpia_clip2; ia->empty();
      ClipIndexedTris2D(vaPoints, &p->ia_fill, _vaClip, ia/*out*/);

      drawPathIndexedTris2D(vaPoints, ia);
   }

   // <method.png>
   static void drawPathFillConcaveTransform3d(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConcaveTransform3d: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      // (note) vertices remain unchanged, generates p.ia_fill
      if(!p->tesselateConcave(&p->points, &tmpia_tesselate_concave))
      {
         Derrorprintf("[---] drawPathFillConcaveTransform3d: tesselateConcave() failed\n");
         return;
      }

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector4f v1;
      Vector4f v2;
      Vector4f v3;
      Vector2f v1s;
      Vector2f v2s;
      Vector2f v3s;
      FloatArray *va = &p->points;
      IntArray *ia = &p->ia_fill;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2))*2u;
         v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u), 0.0f, 1.0f);
         v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u), 0.0f, 1.0f);
         v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u), 0.0f, 1.0f);
         cur_mat_3d.mulv(&v1);
         cur_mat_3d.mulv(&v2);
         cur_mat_3d.mulv(&v3);

         v1.divzTo(&v1s);
         v1s.x = v1s.x + cur_x;
         v1s.y = v1s.y + cur_y;
         v1s.mulf(geo_scale);

         v2.divzTo(&v2s);
         v2s.x = v2s.x + cur_x;
         v2s.y = v2s.y + cur_y;
         v2s.mulf(geo_scale);

         v3.divzTo(&v3s);
         v3s.x = v3s.x + cur_x;
         v3s.y = v3s.y + cur_y;
         v3s.mulf(geo_scale);

         /* if(b_debug_fill >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1s, &v2s, &v3s, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathFillEvenOddTransform3d(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillEvenOddTransform3d: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      // (note) iterates parent+sub paths, replaces p->points, generates p->ia_fill
      FloatArray *vaPoints = &tmpfa_points2;
      if(!p->tesselateEvenOdd(&p->points/*in*/, vaPoints/*out*/))
      {
         if(b_debug_tess) { Derrorprintf("[---] drawPathFillEvenOddTransform3d: tesselateEvenOdd() failed\n"); }
         return;
      }

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector4f v1;
      Vector4f v2;
      Vector4f v3;
      Vector2f v1s;
      Vector2f v2s;
      Vector2f v3s;
      FloatArray *va = vaPoints;
      IntArray   *ia = &p->ia_fill;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2))*2u;
         v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u), 0.0f, 1.0f);
         v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u), 0.0f, 1.0f);
         v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u), 0.0f, 1.0f);
         cur_mat_3d.mulv(&v1);
         cur_mat_3d.mulv(&v2);
         cur_mat_3d.mulv(&v3);

         v1.divzTo(&v1s);
         v1s.x = v1s.x + cur_x;
         v1s.y = v1s.y + cur_y;
         v1s.mulf(geo_scale);

         v2.divzTo(&v2s);
         v2s.x = v2s.x + cur_x;
         v2s.y = v2s.y + cur_y;
         v2s.mulf(geo_scale);

         v3.divzTo(&v3s);
         v3s.x = v3s.x + cur_x;
         v3s.y = v3s.y + cur_y;
         v3s.mulf(geo_scale);

         /* if(b_debug_fill >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1s, &v2s, &v3s, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathFillConcaveTransform3dClip(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillConcaveTransform3dClip: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      // Transform points from 2D to 3D
      FloatArray *vaPoints = &p->points;
      FloatArray *va3 = &tmpfa_va3;
      va3->empty();
      if(vaPoints->num_elements >= (3u*2u))
      {
         Vector4f v1;
         Vector4f v2;
         Vector4f v3;

         v1.init(vaPoints->get(0), vaPoints->get(1), 0, 1);
         v2.init(vaPoints->get(2), vaPoints->get(3), 0, 1);

         cur_mat_3d.mulv(&v1);
         cur_mat_3d.mulv(&v2);

         va3->add3(v1.x, v1.y, v1.z);
         va3->add3(v2.x, v2.y, v2.z);

         /* if(b_debug_fill >= 3) trace "xxx drawPathFillConcaveTransform3dClip: v1="+v1+" cur_mat_3d="+cur_mat_3d; */
         /* if(b_debug_fill >= 3) trace "xxx drawPathFillConcaveTransform3dClip: v1_3d="+v1; */

         sUI vaOff = 4u;
         while(vaOff < vaPoints->num_elements)
         {
            v3.init(vaPoints->get(vaOff+0u), vaPoints->get(vaOff+1u), 0.0f, 1.0f);
            cur_mat_3d.mulv(&v3);
            va3->add3(v3.x, v3.y, v3.z);

            // Next 2D point
            vaOff += 2u;
         }

         // Clip 3D points against near plane
         FloatArray *vaClip3 = &tmpfa_clip3;
         vaClip3->empty();
         /* if(b_debug_clip3d >= 3) trace "xxx drawPathFillConcaveTransform3dClip: va3="+va3.string3; */
         Clip3DZNear(va3, 0.01f/*znear*/, vaClip3);
         /* if(b_debug_clip3d >= 3) trace "xxx drawPathFillConcaveTransform3dClip: vaClip3="+vaClip3.string3; */

         // Project to 2D
         FloatArray *vaProj = &tmpfa_proj2;
         vaProj->empty();
         vaOff = 0u;
         const sUI vaClip3NumVerts = vaClip3->num_elements / 3u;
         for(sUI loopVaClip3Idx = 0u; loopVaClip3Idx < vaClip3NumVerts; loopVaClip3Idx++)
         {
            Vector3f vProj;
            Vector2f vProj2;
            vProj.init(vaClip3->get(vaOff+0u),
                       vaClip3->get(vaOff+1u),
                       vaClip3->get(vaOff+2u)
                       );

            vProj.divzTo(&vProj2);
            // // trace "xxx va["+(vaOff/2)+"] = "+v3+" (xform) mat_2d="+cur_mat_2d;

            vaProj->add2( (vProj2.x + cur_x) * geo_scale,
                          (vProj2.y + cur_y) * geo_scale
                          );

            // Next clipped 3D vertex
            vaOff += 3u;
         }

         /* if(b_debug_clip3d >= 3) trace "xxx drawPathFillConcaveTransform3dClip: cur=("+cur_x+";"+cur_y+") geo_scale="+geo_scale+" vaProj="+vaProj.string2; */

         // Clip 2D
         Path *pFB = updateAndGetPathFb();
         FloatArray *va = &tmpfa_clip2;
         va->empty();
         // trace "xxx vaProj="+vaProj+" pFB.points="+(pFB.points.getString2());
         Clip2D(vaProj, &pFB->points, va);

         /* if(b_debug_clip3d >= 2) trace "xxx drawPathFillConcaveTransform3dClip: va="+va; */

         // (note) vertices remain unchanged, generates p.ia_fill
         if(!p->tesselateConcave(va, &tmpia_tesselate_concave))
         {
            Derrorprintf("[---] drawPathFillConcaveTransform3dClip: tesselateConcave() failed\n");
            return;
         }

         if(va->num_elements >= (3u*2u))
         {
            IntArray *ia = &p->ia_fill;

            const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

            sUI iaOff = 0u;
            /* sUI triIdx = 0u; */
            const sUI numTris = ia->num_elements / 3u;
            for(sUI loopTriIdx = 0u; loopTriIdx < numTris; loopTriIdx++)
            {
               Vector2f v1s;
               Vector2f v2s;
               Vector2f v3s;
               v1s.init(va->get(sUI(ia->get(iaOff+0u))*2u+0u),
                        va->get(sUI(ia->get(iaOff+0u))*2u+1u)
                        );
               v2s.init(va->get(sUI(ia->get(iaOff+1u))*2u+0u),
                        va->get(sUI(ia->get(iaOff+1u))*2u+1u)
                        );
               v3s.init(va->get(sUI(ia->get(iaOff+2u))*2u+0u),
                        va->get(sUI(ia->get(iaOff+2u))*2u+1u)
                        );

               /* if(b_debug_fill >= 2) */
               /*    c32 = palette.get( 1 + (triIdx & 3) ); */

               drawTri(&v1s, &v2s, &v3s, cur_c32, c32Mask);

               // Next triangle
               iaOff += 3u;
               /* triIdx++; */
            }

         } // if #va >= 3

      } // if #points >= 3

   }

   // <method.png>
   static void drawPathFillEvenOddTransform3dClip(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathFillEvenOddTransform3dClip: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      // Transform points from 2D to 3D
      FloatArray *vaPoints = &p->points;
      FloatArray *va3 = &tmpfa_va3;
      va3->empty();
      if(vaPoints->num_elements >= (3u*2u))
      {
         Vector4f v1;
         Vector4f v2;
         Vector4f v3;

         v1.init(vaPoints->get(0), vaPoints->get(1), 0, 1);
         v2.init(vaPoints->get(2), vaPoints->get(3), 0, 1);

         cur_mat_3d.mulv(&v1);
         cur_mat_3d.mulv(&v2);

         va3->add3(v1.x, v1.y, v1.z);
         va3->add3(v2.x, v2.y, v2.z);

         /* if(b_debug_fill >= 3) trace "xxx drawPathFillEvenOddTransform3dClip: v1="+v1+" cur_mat_3d="+cur_mat_3d; */
         /* if(b_debug_fill >= 3) trace "xxx drawPathFillEvenOddTransform3dClip: v1_3d="+v1; */

         sUI vaOff = 4u;
         while(vaOff < vaPoints->num_elements)
         {
            v3.init(vaPoints->get(vaOff+0u), vaPoints->get(vaOff+1u), 0.0f, 1.0f);
            cur_mat_3d.mulv(&v3);
            va3->add3(v3.x, v3.y, v3.z);

            // Next 2D point
            vaOff += 2u;
         }

         // Clip 3D points against near plane
         FloatArray *vaClip3 = &tmpfa_clip3;
         vaClip3->empty();
         /* if(b_debug_clip3d >= 3) trace "xxx drawPathFillEvenOddTransform3dClip: va3="+va3.string3; */
         Clip3DZNear(va3, 0.01f/*znear*/, vaClip3);
         /* if(b_debug_clip3d >= 3) trace "xxx drawPathFillConcaveTransform3dClip: vaClip3="+vaClip3.string3; */

         // Project to 2D
         FloatArray *vaProj = &tmpfa_proj2;
         vaProj->empty();
         vaOff = 0u;
         const sUI vaClip3NumVerts = vaClip3->num_elements / 3u;
         for(sUI loopVaClip3Idx = 0u; loopVaClip3Idx < vaClip3NumVerts; loopVaClip3Idx++)
         {
            Vector3f vProj;
            Vector2f vProj2;
            vProj.init(vaClip3->get(vaOff+0u),
                       vaClip3->get(vaOff+1u),
                       vaClip3->get(vaOff+2u)
                       );

            vProj.divzTo(&vProj2);
            // // trace "xxx va["+(vaOff/2)+"] = "+v3+" (xform) mat_2d="+cur_mat_2d;

            vaProj->add2( (vProj2.x + cur_x) * geo_scale,
                          (vProj2.y + cur_y) * geo_scale
                          );

            // Next clipped 3D vertex
            vaOff += 3u;
         }

         /* if(b_debug_clip3d >= 3) trace "xxx drawPathFillConcaveTransform3dClip: cur=("+cur_x+";"+cur_y+") geo_scale="+geo_scale+" vaProj="+vaProj.string2; */

         // Clip 2D
         Path *pFB = updateAndGetPathFb();
         FloatArray *va = &tmpfa_clip2;
         va->empty();
         // trace "xxx vaProj="+vaProj+" pFB.points="+(pFB.points.getString2());
         Clip2D(vaProj, &pFB->points, va);

         /* if(b_debug_clip3d >= 2) trace "xxx drawPathFillConcaveTransform3dClip: va="+va; */

         // (note) iterates parent+sub paths, replaces p->points, generates p->ia_fill
         if(!p->tesselateEvenOdd(va/*in*/, va/*out*/))
         {
            if(b_debug_tess) { Derrorprintf("[---] drawPathFillEvenOddTransform3dClip: tesselateEvenOdd() failed\n"); }
            return;
         }

         if(va->num_elements >= (3u*2u))
         {
            IntArray *ia = &p->ia_fill;

            const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

            sUI iaOff = 0u;
            /* sUI triIdx = 0u; */
            const sUI numTris = ia->num_elements / 3u;
            for(sUI loopTriIdx = 0u; loopTriIdx < numTris; loopTriIdx++)
            {
               Vector2f v1s;
               Vector2f v2s;
               Vector2f v3s;
               v1s.init(va->get(sUI(ia->get(iaOff+0u))*2u+0u),
                        va->get(sUI(ia->get(iaOff+0u))*2u+1u)
                        );
               v2s.init(va->get(sUI(ia->get(iaOff+1u))*2u+0u),
                        va->get(sUI(ia->get(iaOff+1u))*2u+1u)
                        );
               v3s.init(va->get(sUI(ia->get(iaOff+2u))*2u+0u),
                        va->get(sUI(ia->get(iaOff+2u))*2u+1u)
                        );

               /* if(b_debug_fill >= 2) */
               /*    c32 = palette.get( 1 + (triIdx & 3) ); */

               drawTri(&v1s, &v2s, &v3s, cur_c32, c32Mask);

               // Next triangle
               iaOff += 3u;
               /* triIdx++; */
            }

         } // if #va >= 3

      } // if #points >= 3

   }

   // <method.png>
   static void drawPathLine(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathLine: path_idx=%u #points=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, p->points.num_elements, cur_pal_idx, cur_c32, cur_mask_idx); }

      const FloatArray *va = &p->points;
      if(va->num_elements >= 4u)
      {
         /* if(b_debug_line >= 2) trace "[trc] drawPathLine: points="+va.string2; */
         sF32 lx = va->get(0) + cur_x;
         sF32 ly = va->get(1) + cur_y;
         lx *= geo_scale;
         ly *= geo_scale;
         sF32 firstX = lx;
         sF32 firstY = ly;
         sUI vaOff = 2u;
         const sUI numVertsM1 = (va->num_elements / 2u) - 1u;
         for(sUI loopVaIdx = 0u; loopVaIdx < numVertsM1; loopVaIdx++)
         {
            sF32 cx = va->get(vaOff+0u) + cur_x;
            sF32 cy = va->get(vaOff+1u) + cur_y;
            cx *= geo_scale;
            cy *= geo_scale;
            /* if(b_debug_line >= 2) */
            /*    trace "[trc] drawPathLine: l=("+lx+";"+ly+") c=("+cx+";"+cy+")"; */
            opLine(lx, ly, cx, cy);
            lx = cx;
            ly = cy;
            vaOff += 2u;
         }
         if(p->b_closed)
            opLine(lx, ly, firstX, firstY);
      }
   }

   // <method.png>
   static void drawPathLineClip(Path *p, FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathLineClip: path_idx=%u pal_idx=%u cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_mask_idx); }

      if(p->points.num_elements >= 4u)
      {
         FloatArray *vaPoints = &tmpfa_points2;
         translateAndScalePoints(&p->points, vaPoints/*out*/);

         FloatArray *vaClip = &tmpfa_clip2;
         Clip2D(vaPoints, _vaClip, vaClip);

         /* if(b_debug_line >= 2) trace "[trc] drawPathLineClip: points="+vaClip.string2; */
         sF32 lx = vaClip->get(0);
         sF32 ly = vaClip->get(1);
         sF32 firstX = lx;
         sF32 firstY = ly;
         sUI vaOff = 2u;
         const sUI vaClipNumVerts = (vaClip->num_elements / 2u) - 1u;
         for(sUI loopVaClipIdx = 0u; loopVaClipIdx < vaClipNumVerts; loopVaClipIdx++)
         {
            sF32 cx = vaClip->get(vaOff+0u);
            sF32 cy = vaClip->get(vaOff+1u);
            /* if(b_debug_line >= 2) */
            /*    trace "[trc] drawPathLineClip: l=("+lx+";"+ly+") c=("+cx+";"+cy+")"; */
            opLine(lx, ly, cx, cy);
            lx = cx;
            ly = cy;
            vaOff += 2u;
         }
         if(p->b_closed)
            opLine(lx, ly, firstX, firstY);
      }
   }

   // <method.png>
   static void drawPathLineTransform2d(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathLineTransform2d: path_idx=%u pal_idx=%u cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_mask_idx); }

      FloatArray *va = &p->points;
      /* if(b_debug_line >= 2) trace "[trc] drawPathLineTransform2d: points="+va.string2; */
      Vector2f vl;
      Vector2f vFirst;
      vl.init(va->get(0) + cur_x, va->get(1) + cur_y);
      /* if(b_debug_line >= 3) */
      /*    trace "[trc] drawPathLineTransform2d: vfirst raw="+vl; */
      cur_mat_2d.mulv(&vl);
      vl.mulf(geo_scale);
      vFirst = vl;
      sUI vaOff = 2u;
      const sUI vaNumVertsM1 = (va->num_elements / 2u) - 1u;
      for(sUI loopVaIdx = 0u; loopVaIdx < vaNumVertsM1; loopVaIdx++)
      {
         Vector2f vc; vc.init(va->get(vaOff+0), va->get(vaOff+1));
         /* if(b_debug_line >= 3) */
         /*    trace "[trc] drawPathLineTransform2d: vc raw="+vc; */
         cur_mat_2d.mulv(&vc);
         vc.x = vc.x + cur_x;
         vc.y = vc.y + cur_y;
         vc.mulf(geo_scale);
         /* if(b_debug_line >= 2) */
         /*    trace "[trc] drawPathLineTransform2d: l="+vl+" c="+vc; */
         opLine(vl.x, vl.y, vc.x, vc.y);
         vl = vc;
         vaOff += 2u;
      }
      if(p->b_closed)
      {
         /* if(b_debug_line >= 2) */
         /*    trace "[trc] drawPathLineTransform2d: close l="+vl+" first="+vFirst; */
         opLine(vl.x, vl.y, vFirst.x, vFirst.y);
      }
   }

   // <method.png>
   static void drawPathLineTransform2dClip(Path *p, const FloatArray *_vaClip) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathLineTransform2dClip: path_idx=%u pal_idx=%u cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      transform2DAndTranslateAndScale(&p->points, vaPoints);

      FloatArray *vaClip = &tmpfa_clip2;
      Clip2D(vaPoints, _vaClip, vaClip/*out*/);

      /* if(b_debug_line >= 2) trace "[trc] drawPathLineTransform2dClip: vaClip="+vaClip.string2; */
      Vector2f vl;
      Vector2f vFirst;
      vl.init(vaClip->get(0), vaClip->get(1));
      /* if(b_debug_line >= 3) */
      /*    trace "[trc] drawPathLineTransform2dClip: vfirst raw="+vl; */
      vFirst = vl;
      sUI vaOff = 2u;
      const sUI vaClipNumVertsM1 = (vaClip->num_elements / 2u) - 1u;
      for(sUI loopVaIdx = 0u; loopVaIdx < vaClipNumVertsM1; loopVaIdx++)
      {
         Vector2f vc; vc.init(vaClip->get(vaOff+0u), vaClip->get(vaOff+1u));
         /* if(b_debug_line >= 3) */
         /*    trace "[trc] drawPathLineTransform2dClip: vc raw="+vc; */
         /* if(b_debug_line >= 2) */
         /*    trace "[trc] drawPathLineTransform2dClip: l="+vl+" c="+vc; */
         opLine(vl.x, vl.y, vc.x, vc.y);
         vl = vc;
         vaOff += 2u;
      }
      if(p->b_closed)
      {
         /* if(b_debug_line >= 2) */
         /*    trace "[trc] drawPathLineTransform2dClip: close l="+vl+" first="+vFirst; */
         opLine(vl.x, vl.y, vFirst.x, vFirst.y);
      }
   }

   // <method.png>
   static void drawPathLineTransform3d(Path *p) {
      if(b_debug_fill) { Dprintf("[dbg] drawPathLineTransform3d: path_idx=%u pal_idx=%u cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_mask_idx); }

      FloatArray *va = &p->points;
      /* if(b_debug_line >= 2) trace "[trc] drawPathLineTransform3d: points="+va.string2; */
      Vector4f vFirst;
      Vector2f vFirsts;
      Vector2f vls;
      vFirst.init(va->get(0) + cur_x, va->get(1) + cur_y, 0.0f, 1.0f);
      /* if(b_debug_line >= 3) */
      /*    trace "[trc] drawPathLineTransform3d: vfirst raw="+vFirst; */
      cur_mat_3d.mulv(&vFirst);
      vFirst.divzTo(&vFirsts);
      vFirsts.x = vFirsts.x + cur_x;
      vFirsts.y = vFirsts.y + cur_y;
      vFirsts.mulf(geo_scale);
      vls = vFirsts;
      sUI vaOff = 2u;
      const sUI vaNumVertsM1 = (va->num_elements / 2u) - 1u;
      for(sUI loopVaIdx = 0u; loopVaIdx < vaNumVertsM1; loopVaIdx++)
      {
         Vector4f vc; vc.init(va->get(vaOff+0u), va->get(vaOff+1u), 0.0f, 1.0f);
         /* if(b_debug_line >= 3) */
         /*    trace "[trc] drawPathLineTransform3d: vc raw="+vc; */
         cur_mat_3d.mulv(&vc);
         Vector2f vcs;
         vc.divzTo(&vcs);
         vcs.x = vcs.x + cur_x;
         vcs.y = vcs.y + cur_y;
         vcs.mulf(geo_scale);
         /* if(b_debug_line >= 2) */
         /*    trace "[trc] drawPathLineTransform3d: l="+vls+" c="+vcs; */
         opLine(vls.x, vls.y, vcs.x, vcs.y);
         vls = vcs;
         vaOff += 2u;
      }
      if(p->b_closed)
      {
         /* if(b_debug_line >= 2) */
         /*    trace "[trc] drawPathLineTransform3d: close l="+vls+" first="+vFirsts; */
         opLine(vls.x, vls.y, vFirsts.x, vFirsts.y);
      }
   }

   // <method.png>
   static void exportLineStripPoints(const FloatArray *_va, sBool _bClosed, sBool _bBevel) {
      // (note) when path is closed, last vertex may equal first vertex (=> skip it)
      // (todo) this is the case for regular paths but currently not for ellipses
      sUI vaNum = (_va->num_elements >> 1);
      if(_bClosed)
      {
         if(_va->elements.f32[_va->num_elements-2u] == _va->elements.f32[0u] &&
            _va->elements.f32[_va->num_elements-1u] == _va->elements.f32[1u]
            )
         {
            vaNum--;
         }
      }
      const sUI vaWrap = (vaNum << 1);
      sUI num = vaNum + sUI(_bClosed) + (sUI(_bClosed && _bBevel));
      active_dl_num_verts += num;
      num = (num << 1);
      for(sUI idxOffM = 0u; idxOffM < num; idxOffM += 2u)
      {
         const sUI idxOff = (idxOffM >= vaWrap) ? (idxOffM - vaWrap) : idxOffM;
         const sF32 x = _va->elements.f32[idxOff + 0u];
         const sF32 y = _va->elements.f32[idxOff + 1u];
#ifndef TKMINNIE_DUPLICATE_POINT_VERTICES
         const sSI dupNum = 1;
#else
         const sSI dupNum = _bBevel ? 9 : 6;
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
         for(sSI vtxIdx = 0; vtxIdx < dupNum; vtxIdx++)
         {
            Dexport_vb_i16(sSI(x * 4.0f));
            Dexport_vb_i16(sSI(y * 4.0f));
#ifdef TKMINNIE_DUPLICATE_POINT_VERTICES
            Dexport_vb_i16(vtxIdx);
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
         }
      }
   }

   // <method.png>
   static void exportLineStripPointsTranslateScale(const FloatArray *_va, sBool _bClosed, sBool _bBevel) {
      // (note) when path is closed, last vertex may equal first vertex (=> skip it)
      // (todo) this is the case for regular paths but currently not for ellipses
      sUI vaNum = (_va->num_elements >> 1);
      if(_bClosed)
      {
         if(_va->elements.f32[_va->num_elements-2u] == _va->elements.f32[0u] &&
            _va->elements.f32[_va->num_elements-1u] == _va->elements.f32[1u]
            )
         {
            vaNum--;
         }
      }
      const sUI vaWrap = (vaNum << 1);
      sUI num = vaNum + sUI(_bClosed) + (sUI(_bClosed && _bBevel));
      // Dprintf("xxx exportLineStripPointsTranslateScale: bClosed=%d bBevel=%d vaNum=%u => num=%u\n", _bClosed, _bBevel, vaNum, num);
      active_dl_num_verts += num;
      num = (num << 1);
      for(sUI idxOffM = 0u; idxOffM < num; idxOffM += 2u)
      {
         const sUI idxOff = (idxOffM >= vaWrap) ? (idxOffM - vaWrap) : idxOffM;
         const sF32 x = (_va->elements.f32[idxOff + 0u] + cur_x) * geo_scale;
         const sF32 y = (_va->elements.f32[idxOff + 1u] + cur_y) * geo_scale;
         /* Dprintf("xxx exportLineStripPointsTranslateScale: idxOff=%u p=(%f; %f)\n", idxOff, x, y); */
#ifndef TKMINNIE_DUPLICATE_POINT_VERTICES
         const sSI dupNum = 1;
#else
         const sSI dupNum = _bBevel ? 9 : 6;
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
         for(sSI vtxIdx = 0; vtxIdx < dupNum; vtxIdx++)
         {
            Dexport_vb_i16(sSI(x * 4.0f));
            Dexport_vb_i16(sSI(y * 4.0f));
#ifdef TKMINNIE_DUPLICATE_POINT_VERTICES
            Dexport_vb_i16(vtxIdx);
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
         }
      }
   }

   // <method.png>
   static void drawPathLineStrip(Path *p) {
      if(b_debug_line_strip) { Dprintf("[dbg] drawPathLineStrip: path_idx=%u #points=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, p->points.num_elements/2u, cur_pal_idx, cur_c32, cur_mask_idx); }

      const sUI curJoinCap = calcCurJoinCap();
      exportLineStripPointsTranslateScale(&p->points, p->b_closed, (MINNIE_LINEJOIN_NONE != (curJoinCap & 15u)));
   }

   // <method.png>
   static void drawPathExtrudeShape(Path *p) {
      if(b_debug_extrude) { Dprintf("[dbg] drawPathExtrudeShape: path_idx=%u #points=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, p->points.num_elements/2u, cur_pal_idx, cur_c32, cur_mask_idx); }

      // Dprintf("xxx drawPathExtrudeShape: cur_stroke_w=%f stroke_w_line_join_threshold=%f\n", cur_stroke_w, stroke_w_line_join_threshold);

      const sUI curJoinCap = calcCurJoinCap();
      p->extrudeShape(&p->points, cur_stroke_w, curJoinCap, cur_num_seg, cur_miter_limit);

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      FloatArray *va = &p->va_extrude;
      IntArray *ia = &p->ia_extrude;
      if(b_debug_extrude) { Dprintf("[dbg] drawPathExtrudeShape: va_extrude.num_elements=%u ia_extrude.num_elements=%u\n", va->num_elements, ia->num_elements); }
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0u))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1u))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2u))*2u;
         v1.init(va->get(vaOff1+0u) + cur_x, va->get(vaOff1+1u) + cur_y);
         v2.init(va->get(vaOff2+0u) + cur_x, va->get(vaOff2+1u) + cur_y);
         v3.init(va->get(vaOff3+0u) + cur_x, va->get(vaOff3+1u) + cur_y);
         v1.mulf(geo_scale);
         v2.mulf(geo_scale);
         v3.mulf(geo_scale);
         /* if(b_debug_extrude >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathLineStripClipPre(Path *p, const FloatArray *_vaClip) {
      if(b_debug_line_strip) { Dprintf("[dbg] drawPathLineStripClipPre: path_idx=%u #points=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, p->points.num_elements/2u, cur_pal_idx, cur_c32, cur_mask_idx); }

      if(NULL != loc_vb_export_ofs)
      {
         FloatArray *vaPoints = &tmpfa_points2;
         translateAndScalePoints(&p->points, vaPoints/*out*/);

         FloatArray *vaOut = &tmpfa_clip2;
         Clip2D(vaPoints, _vaClip, vaOut);

         const sUI curJoinCap = calcCurJoinCap();
         exportLineStripPoints(vaOut, p->b_closed, (MINNIE_LINEJOIN_NONE != (curJoinCap & 15u)));
      }
   }

   // <method.png>
   static void drawPathExtrudeShapeClipPre(Path *p, const FloatArray *_vaClip) {
      if(b_debug_extrude) { Dprintf("[dbg] drawPathExtrudeShapeClipPre: path_idx=%u #points=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, p->points.num_elements/2u, cur_pal_idx, cur_c32, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      translateAndScalePoints(&p->points, vaPoints/*out*/);

      FloatArray *vaOut = &tmpfa_clip2;
      Clip2D(vaPoints, _vaClip, vaOut);

      const sUI curJoinCap = calcCurJoinCap();
      p->extrudeShape(vaOut, cur_stroke_w, curJoinCap, cur_num_seg, cur_miter_limit);

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      FloatArray *va = &p->va_extrude;
      IntArray *ia = &p->ia_extrude;
      // Dprintf("xxx drawPathExtrudeShapeClipPre: va->num_elements=%u ia->num_elements=%u\n", va->num_elements, ia->num_elements);
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2))*2u;
         v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u));
         v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u));
         v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u));
         /* if(b_debug_extrude >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathExtrudeShapeClip(Path *p, const FloatArray *_vaClip) {
      if(b_debug_extrude) { Dprintf("[dbg] drawPathExtrudeShapeClip: path_idx=%u #points=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, p->points.num_elements/2u, cur_pal_idx, cur_c32, cur_mask_idx); }

      IntArray *ia = &tmpia_clip2;
      ia->empty();

      FloatArray *vaPoints = &tmpfa_points2;
      translateAndScalePoints(&p->points, vaPoints/*out*/);

      // Dprintf("xxx vaPoints->num_elements=%u\n", vaPoints->num_elements);

      const sUI curJoinCap = calcCurJoinCap();
      p->extrudeShape(vaPoints, cur_stroke_w * geo_scale, curJoinCap, cur_num_seg, cur_miter_limit);

      // Dprintf("xxx ia_extrude.num_elements=%u\n", p->ia_extrude.num_elements);
      // Dprintf("xxx _vaClip->num_elements=%u\n", _vaClip->num_elements);

      // (note) appends helper indices/vertices to p.va_extrude
      ClipIndexedTris2D(&p->va_extrude, &p->ia_extrude, _vaClip, ia/*out*/);

      // Dprintf("xxx ia->num_elements=%u\n", ia->num_elements);

      drawPathIndexedTris2D(&p->va_extrude, ia);
   }

   // <method.png>
   static void drawPathLineStripTransform2d(Path *p) {
      if(b_debug_line_strip) { Dprintf("[dbg] drawPathLineStripTransform2d: path_idx=%u #points=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, p->points.num_elements/2u, cur_pal_idx, cur_c32, cur_mask_idx); }

      if(NULL != loc_vb_export_ofs)
      {
         FloatArray *vaPoints = &tmpfa_points2;
         transform2DAndTranslateAndScale(&p->points, vaPoints);

         const sUI curJoinCap = calcCurJoinCap();
         exportLineStripPoints(vaPoints, p->b_closed, (MINNIE_LINEJOIN_NONE != (curJoinCap & 15u)));
      }
   }

   // <method.png>
   static void drawPathExtrudeShapeTransform2d(Path *p) {
      if(b_debug_extrude) { Dprintf("[dbg] drawPathExtrudeShapeTransform2d: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      const sUI curJoinCap = calcCurJoinCap();
      p->extrudeShape(&p->points, cur_stroke_w, curJoinCap, cur_num_seg, cur_miter_limit);

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      FloatArray *va = &p->va_extrude;
      IntArray *ia = &p->ia_extrude;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0u))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1u))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2u))*2u;
         v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u));
         v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u));
         v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u));
         cur_mat_2d.mulv(&v1);
         cur_mat_2d.mulv(&v2);
         cur_mat_2d.mulv(&v3);
         v1.x = v1.x + cur_x;
         v1.y = v1.y + cur_y;
         v2.x = v2.x + cur_x;
         v2.y = v2.y + cur_y;
         v3.x = v3.x + cur_x;
         v3.y = v3.y + cur_y;
         v1.mulf(geo_scale);
         v2.mulf(geo_scale);
         v3.mulf(geo_scale);

         /* if(b_debug_extrude >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathLineStripTransform2dClipPre(Path *p, const FloatArray *_vaClip) {
      if(b_debug_line_strip) { Dprintf("[dbg] drawPathLineStripTransform2dClipPre: path_idx=%u #points=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, p->points.num_elements/2u, cur_pal_idx, cur_c32, cur_mask_idx); }

      if(NULL != loc_vb_export_ofs)
      {
         FloatArray *vaPoints = &tmpfa_points2;
         transform2DAndTranslateAndScale(&p->points, vaPoints);

         FloatArray *vaClip = &tmpfa_clip2;
         Clip2D(vaPoints, _vaClip, vaClip/*out*/);

         const sUI curJoinCap = calcCurJoinCap();
         exportLineStripPoints(vaClip, p->b_closed, (MINNIE_LINEJOIN_NONE != (curJoinCap & 15u)));
      }
   }

   // <method.png>
   static void drawPathExtrudeShapeTransform2dClipPre(Path *p, const FloatArray *_vaClip) {
      if(b_debug_extrude) { Dprintf("[dbg] drawPathExtrudeShapeTransform2dClipPre: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      FloatArray *vaPoints = &tmpfa_points2;
      transform2DAndTranslateAndScale(&p->points, vaPoints);

      FloatArray *vaClip = &tmpfa_clip2;
      Clip2D(vaPoints, _vaClip, vaClip/*out*/);

      const sUI curJoinCap = calcCurJoinCap();
      p->extrudeShape(vaClip, cur_stroke_w * geo_scale, curJoinCap, cur_num_seg, cur_miter_limit);

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1;
      Vector2f v2;
      Vector2f v3;
      FloatArray *va = &p->va_extrude;
      const IntArray *ia = &p->ia_extrude;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0u))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1u))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2u))*2u;
         v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u));
         v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u));
         v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u));

         /* if(b_debug_extrude >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1, &v2, &v3, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathExtrudeShapeTransform2dClip(Path *p, const FloatArray *_vaClip) {
      if(b_debug_extrude) { Dprintf("[dbg] drawPathExtrudeShapeTransform2dClip: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      // Create va_extrude / ia_extrude
      const sUI curJoinCap = calcCurJoinCap();
      p->extrudeShape(&p->points, cur_stroke_w, curJoinCap, cur_num_seg, cur_miter_limit);

      // Transform points from 2D to 3D
      FloatArray *va = &p->va_extrude;
      const IntArray *iaIn = &p->ia_extrude;

      // (note) appends helper vertices to 'va'
      IntArray *ia = &tmpia_clip2;
      ClipIndexedTris2D(va, iaIn, _vaClip, ia/*out*/);

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector2f v1s;
      Vector2f v2s;
      Vector2f v3s;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0u))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1u))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2u))*2u;
         v1s.init(va->get(vaOff1+0u), va->get(vaOff1+1u));
         v2s.init(va->get(vaOff2+0u), va->get(vaOff2+1u));
         v3s.init(va->get(vaOff3+0u), va->get(vaOff3+1u));

         /* if(b_debug_extrude >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1s, &v2s, &v3s, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathExtrudeShapeTransform3d(Path *p) {
      if(b_debug_extrude) { Dprintf("[dbg] drawPathExtrudeShapeTransform3d: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      const sUI curJoinCap = calcCurJoinCap();
      p->extrudeShape(&p->points, cur_stroke_w, curJoinCap, cur_num_seg, cur_miter_limit);

      const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0u;

      Vector4f v1;
      Vector4f v2;
      Vector4f v3;
      Vector2f v1s;
      Vector2f v2s;
      Vector2f v3s;
      FloatArray *va = &p->va_extrude;
      IntArray *ia = &p->ia_extrude;
      sUI idxOff = 0u;
      /* sUI triIdx = 0u;  // debug color */
      while(idxOff < ia->num_elements)
      {
         sUI vaOff1 = sUI(ia->get(idxOff+0u))*2u;
         sUI vaOff2 = sUI(ia->get(idxOff+1u))*2u;
         sUI vaOff3 = sUI(ia->get(idxOff+2u))*2u;
         v1.init(va->get(vaOff1+0u), va->get(vaOff1+1u), 0.0f, 1.0f);
         v2.init(va->get(vaOff2+0u), va->get(vaOff2+1u), 0.0f, 1.0f);
         v3.init(va->get(vaOff3+0u), va->get(vaOff3+1u), 0.0f, 1.0f);
         cur_mat_3d.mulv(&v1);
         cur_mat_3d.mulv(&v2);
         cur_mat_3d.mulv(&v3);

         v1.divzTo(&v1s);
         v2.divzTo(&v2s);
         v3.divzTo(&v3s);

         v1s.x = v1s.x + cur_x;
         v2s.x = v2s.x + cur_x;
         v3s.x = v3s.x + cur_x;

         v1s.y = v1s.y + cur_y;
         v2s.y = v2s.y + cur_y;
         v3s.y = v3s.y + cur_y;

         v1s.mulf(geo_scale);
         v2s.mulf(geo_scale);
         v3s.mulf(geo_scale);

         /* if(b_debug_extrude >= 2) */
         /*    c32 = palette.get( (1 + triIdx) & 7 ); */

         drawTri(&v1s, &v2s, &v3s, cur_c32, c32Mask);

         idxOff += 3u;
         /* triIdx++;  // debugColor */
      }
   }

   // <method.png>
   static void drawPathExtrudeShapeTransform3dClip(Path *p) {
      if(b_debug_extrude) { Dprintf("[dbg] drawPathExtrudeShapeTransform3dClip: path_idx=%u pal_idx=%u (c32=#%08x) cur_mask_idx=%d\n", p->path_idx, cur_pal_idx, cur_c32, cur_mask_idx); }

      // Create va_extrude / ia_extrude
      const sUI curJoinCap = calcCurJoinCap();
      p->extrudeShape(&p->points, cur_stroke_w, curJoinCap, cur_num_seg, cur_miter_limit);

      // Transform points from 2D to 3D
      FloatArray *vaPoints = &p->va_extrude;
      IntArray *iaPoints = &p->ia_extrude;
      FloatArray *va3 = &tmpfa_va3;
      va3->empty();
      sUI vaOff;
      Vector4f v;
      if(vaPoints->num_elements >= (3u*2u))
      {
         vaOff = 0u;
         while(vaOff < vaPoints->num_elements)
         {
            v.init(vaPoints->get(vaOff+0u),
                   vaPoints->get(vaOff+1u),
                   0.0f,
                   1.0f
                   );

            cur_mat_3d.mulv(&v);
            va3->add3(v.x, v.y, v.z);

            /* if(b_debug_fill >= 3) trace "xxx drawPathExtrudeShapeTransform3dClip: v="+v+" cur_mat_3d="+cur_mat_3d; */
            /* if(b_debug_fill >= 3) trace "xxx drawPathExtrudeShapeTransform3dClip: v_3d="+v; */

            // Next 2D point
            vaOff += 2u;
         }

         // Clip 3D points against near plane
         //  (note) appends intersection vertices to 'va3'
         IntArray *iaClip3 = &tmpia_clip3;
         iaClip3->empty();
         ClipIndexedTris3DZNear(va3, iaPoints, 0.01f/*znear*/, iaClip3);

         // Project to 2D
         FloatArray *vaProj = &tmpfa_proj2;
         vaProj->empty();
         vaOff = 0u;
         const sUI va3NumVerts = va3->num_elements / 3u;
         for(sUI loopVa3Idx = 0u; loopVa3Idx < va3NumVerts; loopVa3Idx++)
         {
            Vector3f vProj;
            Vector2f vProj2;
            vProj.init(va3->get(vaOff+0u),
                       va3->get(vaOff+1u),
                       va3->get(vaOff+2u)
                       );

            vProj.divzTo(&vProj2);
            // // trace "xxx va["+(vaOff/2)+"] = "+v3+" (xform) mat_2d="+cur_mat_2d;

            vaProj->add2( (vProj2.x + cur_x) * geo_scale,
                          (vProj2.y + cur_y) * geo_scale
                          );

            // Next clipped 3D vertex
            vaOff += 3u;
         }

         // (note) appends helper vertices to 'vaProj'
         IntArray *ia = &tmpia_clip2;
         ia->empty();
         Path *pFB = updateAndGetPathFb();
         ClipIndexedTris2D(vaProj, iaClip3, &pFB->points, ia/*out*/);

         const sU32 c32Mask = (cur_mask_idx >= 0) ? palette.getU32(cur_mask_idx) : 0;

         Vector2f v1s;
         Vector2f v2s;
         Vector2f v3s;
         sUI idxOff = 0u;
         /* sUI triIdx = 0u;  // debug color */
         while(idxOff < ia->num_elements)
         {
            sUI vaOff1 = sUI(ia->get(idxOff+0))*2u;
            sUI vaOff2 = sUI(ia->get(idxOff+1))*2u;
            sUI vaOff3 = sUI(ia->get(idxOff+2))*2u;
            v1s.init(vaProj->get(vaOff1+0u), vaProj->get(vaOff1+1u));
            v2s.init(vaProj->get(vaOff2+0u), vaProj->get(vaOff2+1u));
            v3s.init(vaProj->get(vaOff3+0u), vaProj->get(vaOff3+1u));

            /* if(b_debug_extrude >= 2) */
            /*    c32 = palette.get( (1 + triIdx) & 7 ); */

            drawTri(&v1s, &v2s, &v3s, cur_c32, c32Mask);

            idxOff += 3u;
            /* triIdx++;  // debugColor */
         }
      }
   }

   // <method.png>
   static void drawPath(Path *p, int mode, const sUI _forcedPathTypeOr0) {
      if(b_debug_fill) { Dprintf("[dbg] drawPath: mode=%d p->path_idx=%u p->type=%u cur_stroke_w=%f\n", mode, p->path_idx, p->type, cur_stroke_w); }

      if(p->points.num_elements >= (2u*2u))
      {
         // Start or continue draw-triangles op
#if MINNIE_EXPORT_TRIS_EDGEAA
         /* Dprintf("xxx b_edge_aa=%d\n", b_edge_aa); */
         if(b_edge_aa)
         {
            // edgeAA
            edgeAABeginPath();

            beginDrawListOpTriEdgeAA();
         }
         else
#endif // MINNIE_EXPORT_TRIS_EDGEAA
         {
            // MSAA / noAA
            beginDrawListOpTri();
         }

         const sUI pathType = (0u != _forcedPathTypeOr0) ? _forcedPathTypeOr0 : p->type;

         /* Dprintf("xxx pathType=%u (forced=%u) mode=%u cur_stroke_w=%f\n", pathType, _forcedPathTypeOr0, mode, cur_stroke_w); */

         switch(mode)
         {
            case 0:
               // no transform
               if(0.0f == cur_stroke_w)
               {
                  // fill
                  if(MINNIE_PATH_TYPE_EVENODD == pathType)
                  {
                     if(b_render_fill_evenodd)
                     {
                        if(NULL != cur_clip2d_path)
                        {
                           if(b_cur_clip2d_pre)
                              drawPathFillEvenOddClipPre(p, &cur_clip2d_path->points);
                           else
                              drawPathFillEvenOddClip(p, &cur_clip2d_path->points);
                        }
                        else
                        {
                           drawPathFillEvenOdd(p);
                        }
                     }
                  }
                  else if(MINNIE_PATH_TYPE_CONCAVE == pathType)
                  {
                     if(b_render_fill_concave)
                     {
                        if(NULL != cur_clip2d_path)
                        {
                           if(b_cur_clip2d_pre)
                           {
                              if( !(b_tesselate_concave || b_edge_aa) )
                              {
                                 // Start draw-concave op
                                 beginDrawListOpPolygon();
                              }
                              drawPathFillConcaveClipPre(p, &cur_clip2d_path->points);
                           }
                           else
                           {
                              drawPathFillConcaveClip(p, &cur_clip2d_path->points);
                           }
                        }
                        else
                        {
                           if( !(b_tesselate_concave || b_edge_aa) )
                           {
                              // Start draw-concave op
                              beginDrawListOpPolygon();
                           }
                           drawPathFillConcave(p);
                        }
                     }
                  }
                  else
                  {  // MINNIE_PATH_TYPE CONVEX
                     if(NULL != cur_clip2d_path)
                        drawPathFillConvexClip(p, &cur_clip2d_path->points);  // (note) same for pre/post clip
                     else
                        drawPathFillConvex(p);
                  }
               }
               else if(b_render_strokes)
               {
                  if(p->b_closed)
                  {
                     debug_num_stroke_closed_paths++;
                     debug_num_stroke_closed_path_points += (p->points.num_elements >> 1);
                  }
                  else
                  {
                     debug_num_stroke_open_paths++;
                     debug_num_stroke_open_path_points += (p->points.num_elements >> 1);
                  }

                  if(b_allow_line_opt && cur_stroke_w <= 0.25f)
                  {
                     if(NULL != cur_clip2d_path)
                        drawPathLineClip(p, &cur_clip2d_path->points);
                     else
                        drawPathLine(p);
                  }
                  else
                  {
                     if(NULL != cur_clip2d_path)
                     {
                        if(b_cur_clip2d_pre)
                        {
                           if(cur_stroke_w < stroke_w_line_strip_threshold)
                           {
                              beginDrawListOpLineStrip();
                              drawPathLineStripClipPre(p, &cur_clip2d_path->points);
                              /* finishActiveDrawListOp(); */
                           }
                           else
                           {
                              drawPathExtrudeShapeClipPre(p, &cur_clip2d_path->points);
                           }
                        }
                        else
                        {
                           drawPathExtrudeShapeClip(p, &cur_clip2d_path->points);
                        }
                     }
                     else
                     {
                        if(cur_stroke_w < stroke_w_line_strip_threshold)
                        {
                           beginDrawListOpLineStrip();
                           drawPathLineStrip(p);
                           /* finishActiveDrawListOp(); */
                        }
                        else
                        {
                           drawPathExtrudeShape(p);
                        }
                     }
                  }
               }
               break;

            case 1:
               // transform 2d
               if(0.0f == cur_stroke_w)
               {
                  // fill
                  if(MINNIE_PATH_TYPE_EVENODD == pathType)
                  {
                     if(b_render_fill_evenodd)
                     {
                        if(NULL != cur_clip2d_path)
                        {
                           if(b_cur_clip2d_pre)
                              drawPathFillEvenOddTransform2dClipPre(p, &cur_clip2d_path->points);
                           else
                              drawPathFillEvenOddTransform2dClip(p, &cur_clip2d_path->points);
                        }
                        else
                           drawPathFillEvenOddTransform2d(p);
                     }
                  }
                  else if(MINNIE_PATH_TYPE_CONCAVE == pathType)
                  {
                     if(b_render_fill_concave)
                     {
                        if(NULL != cur_clip2d_path)
                        {
                           if(b_cur_clip2d_pre)
                           {
                              // clip path points
                              if( !(b_tesselate_concave || b_edge_aa) )
                              {
                                 // Start draw-concave op
                                 beginDrawListOpPolygon();
                              }
                              drawPathFillConcaveTransform2dClipPre(p, &cur_clip2d_path->points);
                           }
                           else
                           {
                              // clip triangles
                              drawPathFillConcaveTransform2dClip(p, &cur_clip2d_path->points);
                           }
                        }
                        else
                        {
                           if( !(b_tesselate_concave || b_edge_aa) )
                           {
                              // Start draw-concave op
                              beginDrawListOpPolygon();
                           }
                           drawPathFillConcaveTransform2d(p);
                        }
                     }
                  }
                  else
                  {
                     if(NULL != cur_clip2d_path)
                     {
                        drawPathFillConvexTransform2dClip(p, &cur_clip2d_path->points);  // (note) same for pre/post
                     }
                     else
                        drawPathFillConvexTransform2d(p);
                  }
               }
               else if(b_render_strokes)
               {
                  if(p->b_closed)
                  {
                     debug_num_stroke_closed_paths++;
                     debug_num_stroke_closed_path_points += (p->points.num_elements >> 1);
                  }
                  else
                  {
                     debug_num_stroke_open_paths++;
                     debug_num_stroke_open_path_points += (p->points.num_elements >> 1);
                  }

                  if(b_allow_line_opt && cur_stroke_w <= 0.25f)
                  {
                     if(NULL != cur_clip2d_path)
                     {
                        drawPathLineTransform2dClip(p, &cur_clip2d_path->points);
                     }
                     else
                        drawPathLineTransform2d(p);
                  }
                  else
                  {
                     if(NULL != cur_clip2d_path)
                     {
                        if(b_cur_clip2d_pre)
                        {
                           if(cur_stroke_w < stroke_w_line_strip_threshold)
                           {
                              beginDrawListOpLineStrip();
                              drawPathLineStripTransform2dClipPre(p, &cur_clip2d_path->points);
                              /* finishActiveDrawListOp(); */
                           }
                           else
                           {
                              drawPathExtrudeShapeTransform2dClipPre(p, &cur_clip2d_path->points);
                           }
                        }
                        else
                        {
                           drawPathExtrudeShapeTransform2dClip(p, &cur_clip2d_path->points);
                        }
                     }
                     else
                     {
                        if(cur_stroke_w < stroke_w_line_strip_threshold)
                        {
                           beginDrawListOpLineStrip();
                           drawPathLineStripTransform2d(p);
                           /* finishActiveDrawListOp(); */
                        }
                        else
                        {
                           drawPathExtrudeShapeTransform2d(p);
                        }
                     }
                  }
               }
               break;

            case 2:
               // transform 3d
               if(0.0f == cur_stroke_w)
               {
                  if(MINNIE_PATH_TYPE_EVENODD == pathType)
                  {
                     if(b_render_fill_evenodd)
                     {
                        if(b_cur_clip_3d)
                        {
                           drawPathFillEvenOddTransform3dClip(p);
                        }
                        else
                        {
                           drawPathFillEvenOddTransform3d(p);
                        }
                     }
                  }
                  else if(MINNIE_PATH_TYPE_CONCAVE == pathType)
                  {
                     if(b_render_fill_concave)
                     {
                        // triangles
                        if(b_cur_clip_3d)
                        {
                           drawPathFillConcaveTransform3dClip(p);
                        }
                        else
                        {
                           drawPathFillConcaveTransform3d(p);
                        }
                     }
                  }
                  else
                  {
                     if(b_cur_clip_3d)
                     {
                        drawPathFillConvexTransform3dClip(p);
                     }
                     else
                     {
                        drawPathFillConvexTransform3d(p);
                     }
                  }
               }
               else if(b_render_strokes)
               {
                  if(p->b_closed)
                  {
                     debug_num_stroke_closed_paths++;
                     debug_num_stroke_closed_path_points += (p->points.num_elements >> 1);
                  }
                  else
                  {
                     debug_num_stroke_open_paths++;
                     debug_num_stroke_open_path_points += (p->points.num_elements >> 1);
                  }

                  if(b_allow_line_opt && cur_stroke_w <= 0.25f)
                  {
                     if(b_cur_clip_3d)
                     {
                        // (todo) drawPathLineTransform3dClip(p);
                     }
                     else
                     {
                        drawPathLineTransform3d(p);
                     }
                  }
                  else
                  {
                     if(b_cur_clip_3d)
                     {
                        drawPathExtrudeShapeTransform3dClip(p);
                     }
                     else
                     {
                        drawPathExtrudeShapeTransform3d(p);
                     }
                  }
               }
               break;
         }

#if MINNIE_EXPORT_TRIS_EDGEAA
         if(b_edge_aa)
         {
            edgeAAEndPath();
         }
#endif // MINNIE_EXPORT_TRIS_EDGEAA

      }  // if #points >= 2
   }

   // <method.png>
   static void drawMultiPath(Path *p, int mode) {

      debug_num_paths++;

      // concave (n-polygon) GPU tesselation enabled and auto-redirect
      //  PATH_TYPE_EVENODD(without subpaths) to PATH_TYPE_CONCAVE ?
      if( !(b_tesselate_concave || b_edge_aa) && sw_tesselate_size_threshold > 0u )
      {
         if(cur_stroke_w < 0.001f) // fill ?
         {
            if(0u == p->sub_paths.num_elements)
            {
               if(MINNIE_PATH_TYPE_EVENODD == p->type || MINNIE_PATH_TYPE_CONCAVE == p->type)
               {
                  const sUI bbNumPixels = Path::CalcBBoxNumPixels(&p->points);
                  if(bbNumPixels <= sw_tesselate_size_threshold)
                  {
                     debug_num_small_paths++;
                     debug_num_small_path_points += (p->points.num_elements >> 1);
                     drawPath(p, mode, MINNIE_PATH_TYPE_CONCAVE);
                     return;
                  }
                  else
                  {
                     debug_num_large_paths++;
                     debug_num_large_path_points += (p->points.num_elements >> 1);
                     drawPath(p, mode, MINNIE_PATH_TYPE_EVENODD);
                     return;
                  }
               }
            }
         }
      }

      drawPath(p, mode, 0u/*forcedPathTypeOr0*/);

      if(MINNIE_PATH_TYPE_EVENODD != p->type || cur_stroke_w > 0.0f)
      {
         if(b_debug_fill && (p->sub_paths.num_elements > 0u))
         {
            Dprintf("[trc] drawMultiPath: #sub=%u mode=%d\n", p->sub_paths.num_elements, mode);
         }

         for(sUI spIdx = 0u; spIdx < p->sub_paths.num_elements; spIdx++)
         {
            Path *sp = p->sub_paths.elements[spIdx];
            drawPath(sp, mode, 0u/*forcedPathTypeOr0*/);
         }
      }
   }

   // <method.png>
   static void readPalette12(StreamReader *ifs) {
      const sUI numColors = ifs->getU8() + 1u;
      if(b_debug_pal)
      {
         Dprintf("[dbg] MinnieBin::readPalette12: numColors=%u\n", numColors);
      }
      StreamReader4 ifs4; ifs4.begin(ifs);
      palette.empty();
      for(sUI colorIdx = 0u; colorIdx < numColors; colorIdx++)
      {
         sU8 r = ifs4.read4();
         sU8 g = ifs4.read4();
         sU8 b = ifs4.read4();
         if(b_debug_pal)
         {
            Dprintf("[dbg] readPalette12: [%3u]=(#%02x, #%02x, #%02x)\n", palette.num_elements, r, g, b);
         }
         r |= (r << 4);
         g |= (g << 4);
         b |= (b << 4);
         palette.addU32( 0xff000000u | ((r << 16) | (g << 8) | b) );
      }
      ifs4.end();
      cur_c32 = palette.get(cur_pal_idx);
   }

   // <method.png>
   static void readPalette24(StreamReader *ifs) {
      const sUI numColors = ifs->getU8() + 1u;
      if(b_debug_pal)
      {
         Dprintf("[dbg] MinnieBin::readPalette24: numColors=%u\n", numColors);
      }
      palette.empty();
      for(sUI colorIdx = 0u; colorIdx < numColors; colorIdx++)
      {
         const sU8 r = ifs->getU8();
         const sU8 g = ifs->getU8();
         const sU8 b = ifs->getU8();
         if(b_debug_pal)
         {
            Dprintf("[dbg] readPalette24: [%u] = (#%02x,#%02x,#%02x)\n", palette.num_elements, r, g, b);
         }
         palette.add( 0xff000000u | ((r << 16) | (g << 8) | b) );
      }
      cur_c32 = palette.get(cur_pal_idx);
   }

   // <method.png>
   static void readFramebuffers(StreamReader *ifs) {
      sUI numFb = ifs->getU8();
      for(sUI fbIdx = 1u; fbIdx <= numFb; fbIdx++)
      {
         const sUI w = sUI((ifs->getU8() * 8u) * fb_scale + 0.5f);
         const sUI h = sUI((ifs->getU8() * 8u) * fb_scale + 0.5f);

         if(fbIdx < MINNIE_MAX_FRAMEBUFFERS)
         {
            Framebuffer *fb = framebuffers.addNew();
            FramebufferOverride *fbOver = &framebuffer_overrides[fbIdx];
            if(NULL != fbOver->pixels.any)
            {
               if(b_debug_fb)
               {
                  Dprintf("[dbg] add framebuffer override sz=%ux%u pitch=%u ptr=%p\n", fbOver->w, fbOver->h, fbOver->pitch, fbOver->pixels.any);
               }
               fb->init(allocator, framebuffers.num_elements - 1u, fbOver->w, fbOver->h, fbOver->pitch, fbOver->pixels.any);
            }
            else
            {
               if(b_debug_fb)
               {
                  Dprintf("[dbg] add framebuffer %ux%u (fb_scale=%f)\n", w, h, fb_scale);
               }
               fb->init(allocator, framebuffers.num_elements - 1u, w, h, 0u/*pitch=auto*/, NULL/*pixelsOrNull*/);
            }
         }
         else
         {
            Derrorprintf("[---] Minnie::readFramebuffers: fbIdx=%u out of bounds (max=%u), skipping..\n", fbIdx, MINNIE_MAX_FRAMEBUFFERS);
         }
      }
   }

   // <method.png>
   static void readMatrices2D(StreamReader *ifs) {
      const sUI numMat = ifs->getU16() + 1u;
      for(sUI matIdx = 0u; matIdx < numMat; matIdx++)
      {
         Matrix2x3f *mat = matrices_2d.addNew();
         for(sUI i = 0u; i < 2u*3u; i++)
            mat->floats[i] = f32_from_f16(ifs->getU16());

         if(b_debug_fb)
         {
            Dprintf("[dbg] add mat_2d idx=%u mat=([%f %f %f] [%f %f %f])\n", matIdx, mat->floats[0], mat->floats[1], mat->floats[2], mat->floats[3], mat->floats[4], mat->floats[5]);
         }
      }
   }

   // <method.png>
   static void readMatrices3D(StreamReader *ifs) {
      const sUI numMat = ifs->getU16() + 1u;
      for(sUI matIdx = 0u; matIdx < numMat; matIdx++)
      {
         Matrix4f *mat = matrices_3d.addNew();
         for(sUI i = 0u; i < 4u*4u; i++)
            mat->floats[i] = f32_from_f16(ifs->getU16());

         if(b_debug_fb)
         {
            Dprintf("[dbg] add mat_3d idx=%u mat=([%f %f %f %f] [%f %f %f %f] [%f %f %f %f] [%f %f %f %f])\n", matIdx, mat->floats[0], mat->floats[1], mat->floats[2], mat->floats[3], mat->floats[4], mat->floats[5], mat->floats[6], mat->floats[7], mat->floats[8], mat->floats[9], mat->floats[10], mat->floats[11], mat->floats[12], mat->floats[13], mat->floats[14], mat->floats[15]);
         }
      }
   }

   // <method.png>
   static void begin(void) {
      /* Dprintf("xxx parseBuffer call path_fb.init\n"); */

      path_fb.init(NULL/*_allocator*/, 0u/*pathIdx*/, MINNIE_PATH_TYPE_CONVEX/*type*/,
                   (void*)path_fb_data/*pointsData*/, 4u*2u/*pointsMaxElements*/,
                   NULL/*_vaExtrudeData*/, 0u/*maxExtrudedElements*/
                   );

      /* Dprintf("xxx parseBuffer path_fb.init done\n"); */

      palette     .init(allocator, MINNIE_MAX_COLORS,       NULL/*elementsOrNull*/);
      framebuffers.init(allocator, MINNIE_MAX_FRAMEBUFFERS, NULL/*elementsOrNull*/);
      paths       .init(allocator, MINNIE_MAX_PATHS,        NULL/*elementsOrNull*/);
      matrices_2d .init(allocator, MINNIE_MAX_MAT_2D,       NULL/*elementsOrNull*/);
      matrices_3d .init(allocator, MINNIE_MAX_MAT_3D,       NULL/*elementsOrNull*/);

      /* Dprintf("xxx parseBuffer: after pal/fb/paths/mat init\n"); */

      // Create main path (no points but sets renderstates and draws other paths)
      cur_path = paths.addNew();

      // Dprintf("xxx call path->init cur_path=%p\n", cur_path);
      cur_path->init(allocator, 0u/*pathIdx*/, MINNIE_PATH_TYPE_DRAW/*type*/,
                     NULL/*pointsData*/, 0u/*pointsMaxElements*/,
                     NULL/*vaExtrudeData*/, 0u/*maxExtrudedElements*/
                     );

      cur_mirror_x = 0.0f;
      cur_mirror_y = 0.0f;
   }

   // <method.png>
   static void end(void) {
      finishActiveDrawListOp();
   }

   // <method.png>
   static void beginGeo(void) {
      // Add default framebuffer
      cur_fb = framebuffers.addNew();
      {
         FramebufferOverride *fbOver = &framebuffer_overrides[0];
         if(NULL != fbOver->pixels.any)
         {
            if(b_debug_fb)
            {
               Dprintf("[dbg] main framebuffer override sz=%ux%u pitch=%u ptr=%p\n", fbOver->w, fbOver->h, fbOver->pitch, fbOver->pixels.any);
               geo_w = fbOver->w;
               geo_h = fbOver->h;
            }
            cur_fb->init(allocator, 0u/*fbIdx*/, fbOver->w, fbOver->h, fbOver->pitch, fbOver->pixels.any);
         }
         else
         {
            cur_fb->init(allocator, 0u/*fbIdx*/, geo_w, geo_h, 0u/*pitch=auto*/, NULL/*pixelsOrNull*/);
         }
      }

      aspect = (geo_w > 0u) ? (geo_h / sF32(geo_w)) : 1.0f;

      // (note) workaround for test059_abstractbike / test066_cat
      sF32 whMax = (sF32)sMAX(geo_w, geo_h);
      if(whMax > 0.0f)
      {
         cubic_min_dist_sqr = 4.0f * (640.0f / whMax);
         /* Dprintf("xxx cubic_min_dist_sqr=%f\n", cubic_min_dist_sqr); */
      }

      cur_src_fb = cur_fb;
      aa_bias = -0.01f * (160.0f / cur_fb->w);
      /* zglInit2D(geo_w, geo_h); */
   }

   // <method.png>
   static sBool newPath(sUI _type, const char *_debugCmd, const char *_debugName) {
      // type: MINNIE_PATH_TYPE_CONVEX|CONCAVE|EVENODD
      cur_path = paths.addNew();
      if(NULL != cur_path)
      {
         /* Dprintf("xxx 0x84: def_points_per_path=%u\n", def_points_per_path); */
         cur_path->init(allocator, paths.num_elements/*path_idx*/, _type/*type*/,
                        NULL/*pointsData*/, def_points_per_path*2u/*pointsMaxElements*/,
                        tmpfa_extrude.elements.any, max_extruded_vertices_per_path*2u/*maxExtrudedElements*/
                        );
         last_parent_path = cur_path;
         cur_x = 0.0f;
         cur_y = 0.0f;
         cur_mirror_x = 0.0f;
         cur_mirror_y = 0.0f;
         if(b_debug_op) { Dprintf("[dbg] Minnie::newPath: %s %u  (%s)\n", _debugCmd, cur_path->path_idx, _debugName); }
         return YAC_TRUE;
      }
      else
      {
         Derrorprintf("[---] Minnie::newPath: max paths(%u) exceeded (%s)\n", paths.max_elements, _debugCmd);
         return YAC_FALSE;
      }
   }

   // <method.png>
   static void endPath(sBool _bClosed) {
      if(NULL != cur_path)
      {
         cur_path->end(_bClosed);
         cur_path = paths.getRev(0);
         cur_x = 0.0f;
         cur_y = 0.0f;
      }
   }

   // <method.png>
   static sBool parseBuffer(minnie_allocator_handle_t _allocator, void *_data, sU32 _dataSz) {
      /* Dprintf("xxx parseBuffer dataSz=%u\n", _dataSz); */
      allocator = _allocator;

      reset();
      begin();

      /* Dprintf("xxx parseBuffer: after reset\n"); */

      StreamReader ifs;
      ifs.init(_data, _dataSz);

      sU8 palIdx;
      sS16 blitDstX;
      sS16 blitDstY;
      sU16 blitW;
      sU16 blitH;
      sU16 blitSrcX;
      sU16 blitSrcY;
      sF32 dstX;
      sF32 dstY;
      sF32 w;
      sF32 h;
      sF32 c1x;
      sF32 c1y;
      sF32 c2x;
      sF32 c2y;
      sF32 rx;
      sF32 ry;
      sF32 r;
      sF32 xrot;
      sU16 rotAndFlags;
      sBool bLargeArc;
      sBool bArcSweep;
      Path *oPath;  // tmp insert / draw path
      /* sF32 sx; */
      /* sF32 sy; */
      /* sF32 ang; */
      /* Framebuffer *fb; */
      /* Texture *tex; */
      sUI pathIdx;
      StreamReader4 ifs4;
      /* sUI i; */
      sUI matIdx;

      // Read header (flags)
      sU8 flags = ifs.getU8();  // ver MSB6, endianness bit7, AA bit6
      sU8 ver   = (flags & 0x3Fu);
      if(1u != ver)
      {
         Derrorprintf("[---] Minnie::parseBuffer: invalid ver=%u\n", ver);
         return YAC_FALSE;
      }
      sBool bBE = (flags & 0x80u) ? YAC_TRUE : YAC_FALSE;
      b_aa      = (flags & 0x40u) ? YAC_TRUE : YAC_FALSE;

      // (todo) check that data byte order equals host byte order
      if(bBE && !MINNIE_BIG_ENDIAN)
      {
         Derrorprintf("[---] Minnie::parseBuffer: byte order mismatch (data is big endian, host is little endian)\n");
         return YAC_FALSE;
      }
      /* ifs.byteOrder = bBE ? YAC_BIG_ENDIAN : YAC_LITTLE_ENDIAN; */

      for(;;)
      {
         sU8 op = ifs.getU8();

#if 0
         if(b_debug_op)
         {
            Dprintf("[trc] op=0x%02x\n", op);
         }
#endif

         switch(op)
         {
            default:
               Derrorprintf("[---] Minnie::parseBuffer: unhandled op 0x%02x\n", op);
               return YAC_FALSE;

            case 0xE0u:  // end
               if(b_debug_op) { Dprintf("[dbg] end\n"); }
               // if(b_debug_prims) { Dprintf("[dbg] #points=%u #lines=%u #tris=%u\n", total_num_points, total_num_lines, total_num_tris); }
               end();

               // Succeeded
               return YAC_TRUE;

            case 0xE1:  // geo (u16)
               if(0u == framebuffers.num_elements)
               {
                  geo_w = (sUI)( ifs.getU16() * fb_scale + 0.5f );
                  geo_h = (sUI)( ifs.getU16() * fb_scale + 0.5f );
                  beginGeo();
                  if(b_debug_op) { Dprintf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n[dbg] geo=(%u,%u) (u16)\n", geo_w, geo_h); }
               }
               else
               {
                  Dprintf("[~~~] Minnie::parseBuffer: multiple GEO attributes (0xE1), ignoring..\n");
               }
               break;

            case 0xE2u:  // geo (u8)
               if(0u == framebuffers.num_elements)
               {
                  geo_w = (sUI)( (ifs.getU8() * 8) * fb_scale + 0.5f );
                  geo_h = (sUI)( (ifs.getU8() * 8) * fb_scale + 0.5f );
                  beginGeo();
                  if(b_debug_op) { Dprintf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n[dbg] geo=(%u,%u) (u8)\n", geo_w, geo_h); }
               }
               else
               {
                  Dprintf("[~~~] Minnie::parseBuffer: multiple GEO attributes (0xE2), ignoring..\n");
               }
               break;

            case 0xE3u:  // pal12
               readPalette12(&ifs);
               break;

            case 0xE4u:  // pal16 (RESVD)
               /* die "(todo) implement pal16"; */
               break;

            case 0xE5u:  // pal24
               readPalette24(&ifs);
               break;

            case 0xE7u:  // additional draw buffers
               readFramebuffers(&ifs);
               break;

            case 0xE8u:
               readMatrices2D(&ifs);
               break;

            case 0xE9u:
               readMatrices3D(&ifs);
               break;

            case 0xEAu:
               /* die "(todo) implement VM num_consts"; */
               break;

            case 0xEBu:
               /* die "(todo) implement VM num_vars"; */
               break;

            case 0xECu:
               /* die "(todo) implement VM stack"; */
               break;

            case 0xEDu:
               /* die "(todo) implement call_stack"; */
               break;

            case 0xEEu:
               /* die "(todo) implement VM max_ops"; */
               break;

            case 0x01u:
               /* die "(todo) implement VM x"; */
               break;

            case 0x02u:  // n <mat2x3>
               matIdx = ifs.getU8();
               cur_mat_2d.copyFrom(matrices_2d.get(matIdx));
               if(b_debug_op) { Dprintf("[dbg] n %u m=([%f %f %f] [%f %f %f])  (u8)\n", matIdx, cur_mat_2d.floats[0], cur_mat_2d.floats[1], cur_mat_2d.floats[2], cur_mat_2d.floats[3], cur_mat_2d.floats[4], cur_mat_2d.floats[5]); }
               break;

            case 0x03u:  // q <mat4x4>
               matIdx = ifs.getU8();
               cur_mat_3d.copyFrom(matrices_3d.get(matIdx));
               if(b_debug_op) { Dprintf("[dbg] q %u m=[(%f %f %f %f) (%f %f %f %f) (%f %f %f %f) (%f %f %f %f)]  (u8)\n", matIdx, cur_mat_3d.floats[0], cur_mat_3d.floats[1], cur_mat_3d.floats[2], cur_mat_3d.floats[3], cur_mat_3d.floats[4], cur_mat_3d.floats[5], cur_mat_3d.floats[6], cur_mat_3d.floats[7], cur_mat_3d.floats[8], cur_mat_3d.floats[9], cur_mat_3d.floats[10], cur_mat_3d.floats[11], cur_mat_3d.floats[12], cur_mat_3d.floats[13], cur_mat_3d.floats[14], cur_mat_3d.floats[15]); }
               break;

            case 0x04u:  // n <mat2x3>
               matIdx = ifs.getU16();
               cur_mat_2d.copyFrom(matrices_2d.get(matIdx));
               if(b_debug_op) { Dprintf("[dbg] n %u m=([%f %f %f] [%f %f %f])  (u16)\n", matIdx, cur_mat_2d.floats[0], cur_mat_2d.floats[1], cur_mat_2d.floats[2], cur_mat_2d.floats[3], cur_mat_2d.floats[4], cur_mat_2d.floats[5]); }
               break;

            case 0x05u:  // q <mat4x4>
               matIdx = ifs.getU16();
               cur_mat_3d.copyFrom(matrices_3d.get(matIdx));
               if(b_debug_op) { Dprintf("[dbg] q %u m=[(%f %f %f %f) (%f %f %f %f) (%f %f %f %f) (%f %f %f %f)]  (u16)\n", matIdx, cur_mat_3d.floats[0], cur_mat_3d.floats[1], cur_mat_3d.floats[2], cur_mat_3d.floats[3], cur_mat_3d.floats[4], cur_mat_3d.floats[5], cur_mat_3d.floats[6], cur_mat_3d.floats[7], cur_mat_3d.floats[8], cur_mat_3d.floats[9], cur_mat_3d.floats[10], cur_mat_3d.floats[11], cur_mat_3d.floats[12], cur_mat_3d.floats[13], cur_mat_3d.floats[14], cur_mat_3d.floats[15]); }
               break;

            case 0x0Eu:  // h <join> <cap>
               cur_join_cap = ifs.getU8();
               if(!b_render_join_cap)
                  cur_join_cap = 0u;
               {
                  const char *sJoin;
                  switch(cur_join_cap & 15u)
                  {
                     case MINNIE_LINEJOIN_NONE:  sJoin = "none";  break;
                     case MINNIE_LINEJOIN_MITER: sJoin = "miter"; break;
                     case MINNIE_LINEJOIN_ROUND: sJoin = "round"; break;
                     case MINNIE_LINEJOIN_BEVEL: sJoin = "bevel"; break;
                     default:                    sJoin = "?";     break;
                  }
                  const char *sCap;
                  switch( (cur_join_cap>>4) & 15u )
                  {
                     case MINNIE_LINECAP_NONE:   sCap = "none";   break;
                     case MINNIE_LINECAP_BUTT:   sCap = "butt";   break;
                     case MINNIE_LINECAP_ROUND:  sCap = "round";  break;
                     case MINNIE_LINECAP_SQUARE: sCap = "square"; break;
                     default:                    sCap = "?";      break;
                  }
                  if(b_debug_op) { Dprintf("[dbg] h %s %s\n", sJoin, sCap); }
               }
               break;

            case 0x10u:
            case 0x11u:
            case 0x12u:
            case 0x13u:
            case 0x14u:
            case 0x15u:
            case 0x16u:
            case 0x17u:
            case 0x18u:
            case 0x19u:
            case 0x1Au:
            case 0x1Bu:
            case 0x1Cu:
            case 0x1Du:
            case 0x1Eu:
            case 0x1Fu:
               cur_pal_idx = (op & 15u);
               cur_c32 = palette.getU32(cur_pal_idx);
               if(b_debug_op) { Dprintf("[dbg] c %u  (IMM)  (c32=#%08x)\n", cur_pal_idx, cur_c32); }
               break;

            case 0x20u:
               cur_pal_idx = ifs.getU8();
               cur_c32 = palette.getU32(cur_pal_idx);
               if(b_debug_op) { Dprintf("[dbg] c %u  (U8)  (c32=#%08x)\n", cur_pal_idx, cur_c32); }
               break;

            case 0x21u:  // m f32
               cur_x += ifs.getF32();
               cur_y += ifs.getF32();
               if(b_debug_op) { Dprintf("[dbg] m %f;%f  (f32)\n", cur_x, cur_y); }
               break;

            case 0x22u:  // M f32
               cur_x = ifs.getF32();
               cur_y = ifs.getF32();
               if(b_debug_op) { Dprintf("[dbg] M %f;%f  (f32)\n", cur_x, cur_y); }
               break;

            case 0x23u:  // cubic spline (f32)
               c1x     = cur_x + ifs.getF32();
               c1y     = cur_y + ifs.getF32();
               cur_mirror_x = ifs.getF32();
               cur_mirror_y = ifs.getF32();
               dstX    = cur_x + ifs.getF32();
               dstY    = cur_y + ifs.getF32();
               c2x     = dstX - cur_mirror_x;
               c2y     = dstY - cur_mirror_y;
               if(b_debug_op >= 2) { Dprintf("[dbg] c %f %f %f %f %f %f  (f32)\n", c1x, c1y, c2x, c2y, dstX, dstY); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->cubicTo(cur_x, cur_y,
                                 c1x,   c1y,
                                 c2x,   c2y,
                                 dstX,  dstY,
                                 cur_num_seg,
                                 cubic_min_dist_sqr
                                 );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0x24u: {
                 // (todo) remove ?
                 sU8 logicOp = ifs.getU8();
                 if(b_debug_op) { Dprintf("[dbg] g %u\n", logicOp); }
               }
               break;

            case 0x25u:  // cubic spline mirror (f32)
               c1x     = cur_x + cur_mirror_x;
               c1y     = cur_y + cur_mirror_y;
               cur_mirror_x = ifs.getF32();
               cur_mirror_y = ifs.getF32();
               dstX    = cur_x + ifs.getF32();
               dstY    = cur_y + ifs.getF32();
               c2x     = dstX - cur_mirror_x;
               c2y     = dstY - cur_mirror_y;
               if(b_debug_op >= 2) { Dprintf("[dbg] s %f %f %f %f %f %f  (f32)\n", c1x, c1y, c2x, c2y, dstX, dstY); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->cubicTo(cur_x, cur_y,
                                 c1x,   c1y,
                                 c2x,   c2y,
                                 dstX,  dstY,
                                 cur_num_seg,
                                 cubic_min_dist_sqr
                                 );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0x26u:  // l dx dy (f32)
               last_x = cur_x;
               last_y = cur_y;
               cur_x += ifs.getF32();
               cur_y += ifs.getF32();
               if(b_debug_op >= 2) { Dprintf("[dbg] l %f %f  (f32)\n", cur_x, cur_y); }
               cur_path->lazyBegin(last_x, last_y);
               cur_path->lineTo(cur_x, cur_y);
               cur_mirror_x = 0.0f;
               cur_mirror_y = 0.0f;
               break;

            case 0x27u:  // miter limit (u8)
               cur_miter_limit = (sF32)ifs.getU8();
               if(b_debug_op) { Dprintf("[dbg] ml %f\n", cur_miter_limit); }
               break;

            case 0x28u:  // m signed 6.2
               cur_x += ifs.getS8() * (1.0f / 4.0f);
               cur_y += ifs.getS8() * (1.0f / 4.0f);
               if(b_debug_op) { Dprintf("[dbg] m %f;%f  (s6.2)\n", cur_x, cur_y); }
               break;

            case 0x29u:  // m signed 14.2
               cur_x += ifs.getS16() * (1.0f / 4.0f);
               cur_y += ifs.getS16() * (1.0f / 4.0f);
               if(b_debug_op) { Dprintf("[dbg] m %f;%f  (s14.2)\n", cur_x, cur_y); }
               break;

            case 0x2Au:  // m signed 8.0
               cur_x += ifs.getS8();
               cur_y += ifs.getS8();
               if(b_debug_op) { Dprintf("[dbg] m %f;%f  (s8.0)\n", cur_x, cur_y); }
               break;

            case 0x2Bu:  // m unsigned 8.0
               cur_x += ifs.getU8();
               cur_y += ifs.getU8();
               if(b_debug_op) { Dprintf("[dbg] m %f;%f  (u8.0)\n", cur_x, cur_y); }
               break;

            case 0x2Cu:  // M unsigned 8.0
               cur_x = ifs.getU8();
               cur_y = ifs.getU8();
               if(b_debug_op) { Dprintf("[dbg] M %f;%f  (u8.0)\n", cur_x, cur_y); }
               break;

            case 0x2Du:  // M unsigned 8.0 even
               cur_x = (sF32)( (ifs.getU8() << 1) );
               cur_y = (sF32)( (ifs.getU8() << 1) );
               if(b_debug_op) { Dprintf("[dbg] M %f;%f  (u8.0*2)\n", cur_x, cur_y); }
               break;

            case 0x2Eu:  // M unsigned 8.0 mul4
               cur_x = (sF32)( (ifs.getU8() << 2) );
               cur_y = (sF32)( (ifs.getU8() << 2) );
               if(b_debug_op) { Dprintf("[dbg] M %f;%f  (u8.0*4)\n", cur_x, cur_y); }
               break;

            case 0x2Fu:  // M signed 14.2
               cur_x = ifs.getS16() * (1.0f / 4.0f);
               cur_y = ifs.getS16() * (1.0f / 4.0f);
               if(b_debug_op) { Dprintf("[dbg] M %f;%f  (s14.2)\n", cur_x, cur_y); }
               break;

            case 0x30u:  // t 0
            case 0x31u:  // t 1
            case 0x32u:  // t 2
            case 0x33u:  // t 3
            case 0x34u:  // t 4
            case 0x35u:  // t 5
            case 0x36u:  // t 6
               cur_fb = framebuffers.get(op - 0x30u);
               aa_bias = -0.01f * (160.0f / cur_fb->w);
               if(b_debug_op) { Dprintf("[dbg] t %u  (=> %ux%u)  (IMM)\n", cur_fb->fb_idx, cur_fb->w, cur_fb->h); }
               break;

            case 0x37u:  // t ubyte
               cur_fb = framebuffers.get(ifs.getU8());
               aa_bias = -0.01f * (160.0f / cur_fb->w);
               if(b_debug_op) { Dprintf("[dbg] t %u  (=> %ux%u)  (u8)\n", cur_fb->fb_idx, cur_fb->w, cur_fb->h); }
               break;

            case 0x38u:  // u 0
            case 0x39u:  // u 1
            case 0x3Au:  // u 2
            case 0x3Bu:  // u 3
            case 0x3Cu:  // u 4
            case 0x3Du:  // u 5
            case 0x3Eu:  // u 6
               cur_src_fb = framebuffers.get(op - 0x38u);
               if(b_debug_op) { Dprintf("[dbg] u %u  (=> %ux%u)  (IMM)\n", cur_src_fb->fb_idx, cur_src_fb->w, cur_src_fb->h); }
               break;

            case 0x3Fu:  // u ubyte
               cur_src_fb = framebuffers.get(ifs.getU8());
               if(b_debug_op) { Dprintf("[dbg] u %u  (=> %ux%u)  (u8)\n", cur_src_fb->fb_idx, cur_src_fb->w, cur_src_fb->h); }
               break;

            case 0x40u:  // f 0
            case 0x41u:  // f 1
            case 0x42u:  // f 2
            case 0x43u:  // f 3
            case 0x44u:  // f 4
            case 0x45u:  // f 5
            case 0x46u:  // f 6
            case 0x47u:  // f 7
            case 0x48u:  // f 8
            case 0x49u:  // f 9
            case 0x4Au:  // f 10
            case 0x4Bu:  // f 11
            case 0x4Cu:  // f 12
            case 0x4Du:  // f 13
               palIdx = (op - 0x40u);
               if(b_debug_op) { Dprintf("[dbg] f %u  (IMM)\n", palIdx); }
#if MINNIE_SW_RENDER
               if(b_draw_sw_fill)
                  opFill(palIdx);
#endif // MINNIE_SW_RENDER
               break;

            case 0x4Eu:  // f ubyte
               palIdx = ifs.getU8();
               if(b_debug_op) { Dprintf("[dbg] f %u  (u8)\n", palIdx); }
#if MINNIE_SW_RENDER
               if(b_draw_sw_fill)
                  opFill(palIdx);
#endif // MINNIE_SW_RENDER
               break;

            case 0x4Fu:  // clear coverage alpha (AA)
               if(b_debug_op) { Dprintf("[dbg] f aa\n"); }
#if MINNIE_SW_RENDER
               if(b_draw_sw_fill)
                  opFillCoverageAA();
#endif // MINNIE_SW_RENDER
               break;

            case 0x50u:  // w2off
               cur_clip2d_path  = NULL;
               b_cur_clip2d_pre = YAC_FALSE;
               b_cur_clip_3d    = YAC_FALSE;
               if(b_debug_op) { Dprintf("[dbg] w2off\n"); }
               break;

            case 0x51u:  // w2fb
               cur_clip2d_path  = updateAndGetPathFb();
               b_cur_clip2d_pre = YAC_FALSE;
               b_cur_clip_3d    = YAC_FALSE;
               if(b_debug_op) { Dprintf("[dbg] w2fb\n"); }
               break;

            case 0x52u:  // w2p <pathIdx.u8>
               cur_clip2d_path = paths.getRev(ifs.getU8() + 1);
               b_cur_clip2d_pre = YAC_TRUE;
               b_cur_clip_3d    = YAC_FALSE;
               if(b_debug_op) { Dprintf("[dbg] w2p %u  (u8)\n", cur_clip2d_path->path_idx); }
               break;

            case 0x53u:  // w2p <pathIdx.u16>
               cur_clip2d_path  = paths.getRev(ifs.getU16() + 1u);
               b_cur_clip2d_pre = YAC_TRUE;
               b_cur_clip_3d    = YAC_FALSE;
               if(b_debug_op) { Dprintf("[dbg] w2p %u  (u16)\n", cur_clip2d_path->path_idx); }
               break;

            case 0x54u:  // w2 <pathIdx.u8>
               cur_clip2d_path  = paths.getRev(ifs.getU8() + 1u);
               b_cur_clip2d_pre = YAC_FALSE;
               b_cur_clip_3d    = YAC_FALSE;
               if(b_debug_op) { Dprintf("[dbg] w2 %u  (u8)\n", cur_clip2d_path->path_idx); }
               break;

            case 0x55u:  // w2 <pathIdx.u16>
               cur_clip2d_path  = paths.getRev(ifs.getU16() + 1u);
               b_cur_clip2d_pre = YAC_FALSE;
               b_cur_clip_3d    = YAC_FALSE;
               if(b_debug_op) { Dprintf("[dbg] w2 %u  (u16)\n", cur_clip2d_path->path_idx); }
               break;

            case 0x56u:  // w3fb
               cur_clip2d_path  = NULL;
               b_cur_clip2d_pre = YAC_FALSE;
               b_cur_clip_3d    = YAC_TRUE;
               if(b_debug_op) { Dprintf("[dbg] w3fb\n"); }
               break;

            case 0x5Fu:  // fp
               if(b_debug_op >= 2) { Dprintf("[dbg] fp\n"); }
               if(NULL != last_drawn_path)
               {
                  last_drawn_path->free();
               }
               break;

            case 0x60u:  // b dstX dstY
               blitDstX = sS16((ifs.getS16() * (1.0f / 4.0f)) * geo_scale);
               blitDstY = sS16((ifs.getS16() * (1.0f / 4.0f)) * geo_scale);
               if(b_debug_op >= 2) { Dprintf("[dbg] b %d %d [%u %u 0 0]\n", blitDstX, blitDstY, cur_src_fb->w, cur_src_fb->h); }
               break;

            case 0x61u:  // b dstX dstY w h srcX srcY (u8)
               blitDstX = sS16((ifs.getS16() * (1.0f / 4.0f)) * geo_scale);
               blitDstY = sS16((ifs.getS16() * (1.0f / 4.0f)) * geo_scale);
               blitW = sU16(ifs.getU8() * geo_scale);
               blitH = sU16(ifs.getU8() * geo_scale);
               blitSrcX = sU16(ifs.getU8() * geo_scale);
               blitSrcY = sU16(ifs.getU8() * geo_scale);
               if(b_debug_op >= 2) { Dprintf("[dbg] b %d %d %u %u %u %u  (u8)\n", blitDstX, blitDstY, blitW, blitH, blitSrcX, blitSrcY); }
               break;

            case 0x62u:  // b dstX dstY w h srcX srcY (u16)
               blitDstX = sS16((ifs.getS16() * (1.0f / 4.0f)) * geo_scale);
               blitDstY = sS16((ifs.getS16() * (1.0f / 4.0f)) * geo_scale);
               blitW = sU16(ifs.getU16() * geo_scale);
               blitH = sU16(ifs.getU16() * geo_scale);
               blitSrcX = sU16(ifs.getU16() * geo_scale);
               blitSrcY = sU16(ifs.getU16() * geo_scale);
               if(b_debug_op >= 2) { Dprintf("[dbg] b %d %d %u %u %u %u  (u16)\n", blitDstX, blitDstY, blitW, blitH, blitSrcX, blitSrcY); }
               break;

            // 0x63..0x6F: RESVD

            case 0x70u:  // mask 0
            case 0x71u:  // mask 1
            case 0x72u:  // mask 2
            case 0x73u:  // mask 3
            case 0x74u:  // mask 4
            case 0x75u:  // mask 5
            case 0x76u:  // mask 6
            case 0x77u:  // mask 7
            case 0x78u:  // mask 8
            case 0x79u:  // mask 9
            case 0x7Au:  // mask 10
            case 0x7Bu:  // mask 11
            case 0x7Cu:  // mask 12
            case 0x7Du:  // mask 13
            case 0x7Eu:  // mask 14
            case 0x7Fu:  // mask 15
               cur_mask_idx = (op - 0x70u);
               if(b_debug_op) { Dprintf("[dbg] a %d  (IMM)\n", cur_mask_idx); }
               break;

            case 0x80u:  // mask <u8>
               cur_mask_idx = ifs.getU8();
               if(b_debug_op) { Dprintf("[dbg] a %d  (u8)\n", cur_mask_idx); }
               break;

            case 0x81u:  // mask off
               cur_mask_idx = -1;
               if(b_debug_op) { Dprintf("[dbg] a off\n"); }
               break;

            case 0x82u:  // k / stroke_w
               cur_stroke_w = (ifs.getU8() * (1.0f / 4.0f)) * stroke_scale + stroke_offset;
               // Dprintf("xxx minnie: set cur_stroke_w=%f  stroke_scale=%f stroke_offset=%f\n", cur_stroke_w, stroke_scale, stroke_offset);
               if(b_debug_op)
               {
                  if(cur_stroke_w <= 0.001f)
                  {
                     Dprintf("[dbg] k fill  (u8)\n");
                  }
                  else if(cur_stroke_w <= 0.25f)
                  {
                     Dprintf("[dbg] k line  (u8)\n");
                  }
                  else
                  {
                     Dprintf("[dbg] k %f\n", cur_stroke_w);
                  }
               }
               break;

            case 0x83u:  // k 0    /    k fill
               cur_stroke_w = 0.0f;
               if(b_debug_op) { Dprintf("[dbg] k fill\n"); }
               break;

            case 0x84u:  // p <idx>  (convex)
               if(!newPath(MINNIE_PATH_TYPE_CONVEX, "p", "convex"))
                  return YAC_FALSE;
               break;

            case 0x85u:  // pt <idx>  (concave)
               if(b_force_concave_evenodd)
               {
                  if(!newPath(MINNIE_PATH_TYPE_EVENODD, "ph", "evenodd"))
                     return YAC_FALSE;
               }
               else
               {
                  if(!newPath(MINNIE_PATH_TYPE_CONCAVE, "pt", "concave"))
                     return YAC_FALSE;
               }
               break;

            case 0x86u: // psub
               if(NULL != last_parent_path)
               {
                  cur_path = last_parent_path->sub_paths.addNew();
                  if(NULL != cur_path)
                  {
                     cur_path->init(allocator, 999999u/*pathIdx*/, last_parent_path->type,
                                    NULL/*pointsData*/, def_points_per_path*2u/*pointsMaxElements*/,
                                    tmpfa_extrude.elements.any, max_extruded_vertices_per_path*2u/*maxExtrudedElements*/
                                    );
                     cur_path->parent_path = last_parent_path;
                     cur_x = 0.0f;
                     cur_y = 0.0f;
                     cur_mirror_x = 0.0f;
                     cur_mirror_y = 0.0f;
                     if(b_debug_op) { Dprintf("[dbg] psub   (parent_path_idx=%u)\n", last_parent_path->path_idx); }
                  }
                  else
                  {
                     Derrorprintf("[---] Minnie::parseBuffer: max sub paths(%u) exceeded\n", last_parent_path->sub_paths.max_elements);
                     return YAC_FALSE;
                  }
               }
               else
               {
                  Derrorprintf("[---] Minnie::parseBuffer: 'psub' op w/o parent path\n");
                  return YAC_FALSE;
               }
               break;

            case 0x87u:  // ph <idx>  (evenodd)
               if(!newPath(MINNIE_PATH_TYPE_EVENODD, "ph", "evenodd"))
                  return YAC_FALSE;
               break;

            case 0x88u:  // l x y (unsigned 6.2)
               last_x = cur_x;
               last_y = cur_y;
               cur_x += ifs.getU8() * (1.0f / 4.0f);
               cur_y += ifs.getU8() * (1.0f / 4.0f);
               if(b_debug_op >= 2) { Dprintf("[dbg] l %f %f  (u6.2)\n", cur_x, cur_y); }
               cur_path->lazyBegin(last_x, last_y);
               cur_path->lineTo(cur_x, cur_y);
               cur_mirror_x = 0.0f;
               cur_mirror_y = 0.0f;
               break;

            case 0x89u:  // l x y (signed 14.2 shorts)
               last_x = cur_x;
               last_y = cur_y;
               cur_x += ifs.getS16() * (1.0f / 4.0f);
               cur_y += ifs.getS16() * (1.0f / 4.0f);
               if(b_debug_op >= 2) { Dprintf("[dbg] l %f %f  (s14.2)\n", cur_x, cur_y); }
               cur_path->lazyBegin(last_x, last_y);
               cur_path->lineTo(cur_x, cur_y);
               cur_mirror_x = 0.0f;
               cur_mirror_y = 0.0f;
               break;

            case 0x8Au:  // l x y (u8)
               last_x = cur_x;
               last_y = cur_y;
               // Dprintf("xxx 0x8A: l  last=(%f; %f)\n", last_x, last_y);
               cur_x += ifs.getU8();
               cur_y += ifs.getU8();
               if(b_debug_op >= 2) { Dprintf("[dbg] l %f %f  (u8)\n", cur_x, cur_y); }
               cur_path->lazyBegin(last_x, last_y);
               cur_path->lineTo(cur_x, cur_y);
               cur_mirror_x = 0.0f;
               cur_mirror_y = 0.0f;
               break;

            case 0x8Bu:  // l x y (s6.2)
               last_x = cur_x;
               last_y = cur_y;
               cur_x += ifs.getS8() * (1.0f / 4.0f);
               cur_y += ifs.getS8() * (1.0f / 4.0f);
               if(b_debug_op >= 2) { Dprintf("[dbg] l %f %f  (s6.2)\n", cur_x, cur_y); }
               cur_path->lazyBegin(last_x, last_y);
               cur_path->lineTo(cur_x, cur_y);
               cur_mirror_x = 0.0f;
               cur_mirror_y = 0.0f;
               break;

            case 0x8Cu:  // l x y (s10.2)
               ifs4.begin(&ifs);
               last_x = cur_x;
               last_y = cur_y;
               cur_x += ifs4.read12s() * (1.0f / 4.0f);
               cur_y += ifs4.read12s() * (1.0f / 4.0f);
               ifs4.end();
               if(b_debug_op >= 2) { Dprintf("[dbg] l %f %f  (s10.2)\n", cur_x, cur_y); }
               cur_path->lazyBegin(last_x, last_y);
               cur_path->lineTo(cur_x, cur_y);
               break;

            case 0x8Du:  // pi  (immediate)
               if(!newPath(MINNIE_PATH_TYPE_IMMEDIATE, "pi", "immediate"))
                  return YAC_FALSE;
               break;

            case 0x8Eu:  // num_seg (u8)
               cur_num_seg = ifs.getU8();
               if(b_debug_op) { Dprintf("[dbg] j %u\n", cur_num_seg); }
               break;

            case 0x8Fu:  // num_seg (u16)
               cur_num_seg = ifs.getU16();
               if(b_debug_op) { Dprintf("[dbg] j %u\n", cur_num_seg); }
               break;

            case 0x90u:  // cubic spline (s6.2)
               c1x     = cur_x + (ifs.getS8() * (1.0f / 4.0f));
               c1y     = cur_y + (ifs.getS8() * (1.0f / 4.0f));
               cur_mirror_x = (ifs.getS8() * (1.0f / 4.0f));
               cur_mirror_y = (ifs.getS8() * (1.0f / 4.0f));
               dstX    = cur_x + (ifs.getS8() * (1.0f / 4.0f));
               dstY    = cur_y + (ifs.getS8() * (1.0f / 4.0f));
               c2x     = dstX - cur_mirror_x;
               c2y     = dstY - cur_mirror_y;
               if(b_debug_op >= 2) { Dprintf("[dbg] c %f %f %f %f %f %f  (s6.2)\n", c1x, c1y, c2x, c2y, dstX, dstY); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->cubicTo(cur_x, cur_y,
                                 c1x,   c1y,
                                 c2x,   c2y,
                                 dstX,  dstY,
                                 cur_num_seg,
                                 cubic_min_dist_sqr
                                 );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0x91u:  // cubic spline (s8.0)
               c1x     = cur_x + ifs.getS8();
               c1y     = cur_y + ifs.getS8();
               cur_mirror_x = ifs.getS8();
               cur_mirror_y = ifs.getS8();
               dstX    = cur_x + ifs.getS8();
               dstY    = cur_y + ifs.getS8();
               c2x     = dstX - cur_mirror_x;
               c2y     = dstY - cur_mirror_y;
               if(b_debug_op >= 2) { Dprintf("[dbg] c %f %f %f %f %f %f  (s8.0)\n", c1x, c1y, c2x, c2y, dstX, dstY); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->cubicTo(cur_x, cur_y,
                                 c1x,   c1y,
                                 c2x,   c2y,
                                 dstX,  dstY,
                                 cur_num_seg,
                                 cubic_min_dist_sqr
                                 );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0x92u:  // cubic spline (u8.0)
               c1x     = cur_x + ifs.getU8();
               c1y     = cur_y + ifs.getU8();
               cur_mirror_x = ifs.getU8();
               cur_mirror_y = ifs.getU8();
               dstX    = cur_x + ifs.getU8();
               dstY    = cur_y + ifs.getU8();
               c2x     = dstX - cur_mirror_x;
               c2y     = dstY - cur_mirror_y;
               if(b_debug_op >= 2) { Dprintf("[dbg] c %f %f %f %f %f %f  (u8.0)\n", c1x, c1y, c2x, c2y, dstX, dstY); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->cubicTo(cur_x, cur_y,
                                 c1x,   c1y,
                                 c2x,   c2y,
                                 dstX,  dstY,
                                 cur_num_seg,
                                 cubic_min_dist_sqr
                                 );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0x93u:  // cubic spline (s14.2)
               c1x     = cur_x + (ifs.getS16() * (1.0f / 4.0f));
               c1y     = cur_y + (ifs.getS16() * (1.0f / 4.0f));
               cur_mirror_x = (ifs.getS16() * (1.0f / 4.0f));
               cur_mirror_y = (ifs.getS16() * (1.0f / 4.0f));
               if(b_debug_cubic) { Dprintf("[trc] cur=(%f;%f) mirror=(%f;%f)", cur_x, cur_y, cur_mirror_x, cur_mirror_y); }
               dstX    = cur_x + (ifs.getS16() * (1.0f / 4.0f));
               dstY    = cur_y + (ifs.getS16() * (1.0f / 4.0f));
               c2x     = dstX - cur_mirror_x;
               c2y     = dstY - cur_mirror_y;
               if(b_debug_op >= 2) { Dprintf("[dbg] c %f %f %f %f %f %f  (s14.2)\n", c1x, c1y, c2x, c2y, dstX, dstY); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->cubicTo(cur_x, cur_y,
                                 c1x,   c1y,
                                 c2x,   c2y,
                                 dstX,  dstY,
                                 cur_num_seg,
                                 cubic_min_dist_sqr
                                 );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0x94u:  // cubic spline (s10.2)
               ifs4.begin(&ifs);
               c1x     = cur_x + (ifs4.read12s() * (1.0f / 4.0f));
               c1y     = cur_y + (ifs4.read12s() * (1.0f / 4.0f));
               cur_mirror_x = (ifs4.read12s() * (1.0f / 4.0f));
               cur_mirror_y = (ifs4.read12s() * (1.0f / 4.0f));
               if(b_debug_cubic) { Dprintf("[trc] cur=(%f;%f) mirror=(%f;%f)", cur_x, cur_y, cur_mirror_x, cur_mirror_y); }
               dstX    = cur_x + (ifs4.read12s() * (1.0f / 4.0f));
               dstY    = cur_y + (ifs4.read12s() * (1.0f / 4.0f));
               ifs4.end();
               c2x     = dstX - cur_mirror_x;
               c2y     = dstY - cur_mirror_y;
               if(b_debug_op >= 2) { Dprintf("[dbg] c %f %f %f %f %f %f  (s10.2)\n", c1x, c1y, c2x, c2y, dstX, dstY); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->cubicTo(cur_x, cur_y,
                                 c1x,   c1y,
                                 c2x,   c2y,
                                 dstX,  dstY,
                                 cur_num_seg,
                                 cubic_min_dist_sqr
                                 );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0x95u:  // m -(unsigned 8.0)
               cur_x -= ifs.getU8();
               cur_y -= ifs.getU8();
               if(b_debug_op) { Dprintf("[dbg] m %f;%f  (-u8.0)\n", cur_x, cur_y); }
               break;

            case 0x96u:  // m (unsigned 8.0 * 2)
               cur_x += (ifs.getU8() + 1u) * 2.0f;
               cur_y += (ifs.getU8() + 1u) * 2.0f;
               if(b_debug_op) { Dprintf("[dbg] m %f;%f  (u8.0*2)\n", cur_x, cur_y); }
               break;

            // 0x95..0x97: RESVD

            case 0x98u:  // cubic spline mirror (s6.2)
               c1x     = cur_x + cur_mirror_x;
               c1y     = cur_y + cur_mirror_y;
               cur_mirror_x = (ifs.getS8() * (1.0f / 4.0f));
               cur_mirror_y = (ifs.getS8() * (1.0f / 4.0f));
               dstX    = cur_x + (ifs.getS8() * (1.0f / 4.0f));
               dstY    = cur_y + (ifs.getS8() * (1.0f / 4.0f));
               c2x     = dstX - cur_mirror_x;
               c2y     = dstY - cur_mirror_y;
               if(b_debug_op >= 2) { Dprintf("[dbg] s %f %f %f %f %f %f  (s6.2)\n", c1x, c1y, c2x, c2y, dstX, dstY); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->cubicTo(cur_x, cur_y,
                                 c1x,   c1y,
                                 c2x,   c2y,
                                 dstX,  dstY,
                                 cur_num_seg,
                                 cubic_min_dist_sqr
                                 );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0x99u:  // cubic spline mirror (s8.0)
               c1x     = cur_x + cur_mirror_x;
               c1y     = cur_y + cur_mirror_y;
               cur_mirror_x = ifs.getS8();
               cur_mirror_y = ifs.getS8();
               dstX    = cur_x + ifs.getS8();
               dstY    = cur_y + ifs.getS8();
               c2x     = dstX - cur_mirror_x;
               c2y     = dstY - cur_mirror_y;
               if(b_debug_op >= 2) { Dprintf("[dbg] s %f %f %f %f %f %f  (s8.0)\n", c1x, c1y, c2x, c2y, dstX, dstY); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->cubicTo(cur_x, cur_y,
                                 c1x,   c1y,
                                 c2x,   c2y,
                                 dstX,  dstY,
                                 cur_num_seg,
                                 cubic_min_dist_sqr
                                 );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0x9Au:  // cubic spline mirror (u8.0)
               c1x     = cur_x + cur_mirror_x;
               c1y     = cur_y + cur_mirror_y;
               cur_mirror_x = ifs.getU8();
               cur_mirror_y = ifs.getU8();
               dstX    = cur_x + ifs.getU8();
               dstY    = cur_y + ifs.getU8();
               c2x     = dstX - cur_mirror_x;
               c2y     = dstY - cur_mirror_y;
               if(b_debug_op >= 2) { Dprintf("[dbg] s %f %f %f %f %f %f  (u8.0)\n", c1x, c1y, c2x, c2y, dstX, dstY); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->cubicTo(cur_x, cur_y,
                                 c1x,   c1y,
                                 c2x,   c2y,
                                 dstX,  dstY,
                                 cur_num_seg,
                                 cubic_min_dist_sqr
                                 );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0x9Bu:  // cubic spline mirror (s14.2)
               c1x     = cur_x + cur_mirror_x;
               c1y     = cur_y + cur_mirror_y;
               cur_mirror_x = (ifs.getS16() * (1.0f / 4.0f));
               cur_mirror_y = (ifs.getS16() * (1.0f / 4.0f));
               dstX    = cur_x + (ifs.getS16() * (1.0f / 4.0f));
               dstY    = cur_y + (ifs.getS16() * (1.0f / 4.0f));
               c2x     = dstX - cur_mirror_x;
               c2y     = dstY - cur_mirror_y;
               if(b_debug_op >= 2) { Dprintf("[dbg] s %f %f %f %f %f %f  (s14.2)\n", c1x, c1y, c2x, c2y, dstX, dstY); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->cubicTo(cur_x, cur_y,
                                 c1x,   c1y,
                                 c2x,   c2y,
                                 dstX,  dstY,
                                 cur_num_seg,
                                 cubic_min_dist_sqr
                                 );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            // 0x9C..0x9D: RESVD

            case 0x9Eu:  // arc (s14.2)
               rx        = ifs.getS16() * (1.0f / 4.0f);
               ry        = ifs.getS16() * (1.0f / 4.0f);
               rotAndFlags = ifs.getU16();
               xrot      = ((rotAndFlags >> 2)&16383u) * ((sF32)sM_2PI / (512.0f*32));
               if(b_debug_arc >= 2 && (xrot != 0.0f)) { Dprintf("[trc] arc rotAndFlags=%u xrot=%f\n", rotAndFlags, (xrot*(360.0f/sM_2PI))); }
               bLargeArc = rotAndFlags & (1u << 0);
               bArcSweep = (rotAndFlags & (1u << 1)) >> 1;
               dstX      = cur_x + (ifs.getS16() * (1.0f / 4.0f));
               dstY      = cur_y + (ifs.getS16() * (1.0f / 4.0f));
               if(b_debug_op) { Dprintf("[dbg] arc %f %f %f %d %d %f %f (#seg=%u) (s14.2)\n", rx, ry, xrot, bLargeArc, bArcSweep, dstX, dstY, cur_num_seg); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->arcTo(cur_x, cur_y,
                               rx, ry,
                               xrot,
                               bLargeArc,
                               bArcSweep,
                               dstX, dstY,
                               cur_num_seg
                               );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0x9Fu:  // arc (f32)
               rx        = ifs.getF32();
               ry        = ifs.getF32();
               rotAndFlags = ifs.getU16();
               xrot      = ((rotAndFlags >> 2)&16383u) * ((sF32)sM_2PI / (512.0f*32));
               if(b_debug_arc >= 2 && (xrot != 0.0f)) { Dprintf("[trc] arc rotAndFlags=%u xrot=%f\n", rotAndFlags, (xrot*(360.0f/sM_2PI))); }
               bLargeArc = rotAndFlags & (1u << 0);
               bArcSweep = (rotAndFlags & (1u << 1)) >> 1;
               dstX      = cur_x + ifs.getF32();
               dstY      = cur_y + ifs.getF32();
               if(b_debug_op) { Dprintf("[dbg] arc %f %f %f %d %d %f %f (#seg=%u) (f32)\n", rx, ry, xrot, bLargeArc, bArcSweep, dstX, dstY, cur_num_seg); }
               cur_path->lazyBegin(cur_x, cur_y);
               cur_path->arcTo(cur_x, cur_y,
                               rx, ry,
                               xrot,
                               bLargeArc,
                               bArcSweep,
                               dstX, dstY,
                               cur_num_seg
                               );
               cur_x   = dstX;
               cur_y   = dstY;
               break;

            case 0xA0u:  // r (u6.2)
               w = ifs.getU8() * (1.0f / 4.0f);
               h = ifs.getU8() * (1.0f / 4.0f);
               if(b_debug_op) { Dprintf("[dbg] r %f %f  (u6.2)\n", w, h); }
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpRect(w, h);
               }
               else
               {
                  cur_path->rect(cur_x, cur_y, w, h);
               }
               break;

            case 0xA1u:  // r (u14.2)
               w = ifs.getU16() * (1.0f / 4.0f);
               h = ifs.getU16() * (1.0f / 4.0f);
               if(b_debug_op) { Dprintf("[dbg] r %f %f  (u14.2)\n", w, h); }
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpRect(w, h);
               }
               else
               {
                  cur_path->rect(cur_x, cur_y, w, h);
               }
               break;

            case 0xA2u:  // r (u8.0)
               w = ifs.getU8();
               h = ifs.getU8();
               if(b_debug_op) { Dprintf("[dbg] r %f %f  (u8.0)\n", w, h); }
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpRect(w, h);
               }
               else
               {
                  cur_path->rect(cur_x, cur_y, w, h);
               }
               break;

            case 0xA3u:  // r (u8.0 even)
               w = (sF32)( (ifs.getU8() << 1) );
               h = (sF32)( (ifs.getU8() << 1) );
               if(b_debug_op) { Dprintf("[dbg] r %f %f  (u8.0*2)\n", w, h); }
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpRect(w, h);
               }
               else
               {
                  cur_path->rect(cur_x, cur_y, w, h);
               }
               break;

            case 0xA4:  // r (f32)
               w = ifs.getF32();
               h = ifs.getF32();
               rx = ifs.getF32();
               ry = ifs.getF32();
               if(b_debug_op) { Dprintf("[dbg] r %f %f %f %f  (f32)\n", w, h, rx, ry); }
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpRoundRect(w, h, rx, ry);
               }
               else
               {
                  cur_path->roundRect(cur_x, cur_y, w, h, rx, ry, cur_num_seg, cubic_min_dist_sqr);
               }
               break;

            case 0xA5:  // r (u14.2)
               w = ifs.getU16() * (1.0f / 4.0f);
               h = ifs.getU16() * (1.0f / 4.0f);
               rx = ifs.getU16() * (1.0f / 4.0f);
               ry = ifs.getU16() * (1.0f / 4.0f);
               if(b_debug_op) { Dprintf("[dbg] r %f %f %f %f  (u14.2)\n", w, h, rx, ry); }
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpRoundRect(w, h, rx, ry);
               }
               else
               {
                  cur_path->roundRect(cur_x, cur_y, w, h, rx, ry, cur_num_seg, cubic_min_dist_sqr);
               }
               break;

            case 0xA6:  // r (u8*2)
               w = ifs.getU8() * 2.0f;
               h = ifs.getU8() * 2.0f;
               rx = ifs.getU8() * 2.0f;
               ry = ifs.getU8() * 2.0f;
               if(b_debug_op) { Dprintf("[dbg] r %f %f %f %f  (u8*2)\n", w, h, rx, ry); }
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpRoundRect(w, h, rx, ry);
               }
               else
               {
                  cur_path->roundRect(cur_x, cur_y, w, h, rx, ry, cur_num_seg, cubic_min_dist_sqr);
               }
               break;

            case 0xB0u:  // e (u6.2)
               rx = ifs.getU8() * (1.0f / 4.0f);
               ry = ifs.getU8() * (1.0f / 4.0f);
               if(b_debug_op) { Dprintf("[dbg] e %f %f  (u6.2)\n", rx, ry); }
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpEllipse(rx, ry);
               }
               else
               {
                  cur_path->ellipse(cur_x, cur_y, rx, ry, cur_num_seg);
               }
               break;

            case 0xB1u:  // e (u14.2)
               rx = ifs.getU16() * (1.0f / 4.0f);
               ry = ifs.getU16() * (1.0f / 4.0f);
               if(b_debug_op) { Dprintf("[dbg] e %f %f  (u14.2)\n", rx, ry); }
               /* Dprintf("xxx ellipse cur_path->type=%d\n", cur_path->type); */
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpEllipse(rx, ry);
               }
               else
               {
                  cur_path->ellipse(cur_x, cur_y, rx, ry, cur_num_seg);
               }
               break;

            case 0xB2u:  // e (u8.0)
               rx = (sF32)( ifs.getU8() );
               ry = (sF32)( ifs.getU8() );
               if(b_debug_op) { Dprintf("[dbg] e %f %f  (u8.0)\n", rx, ry); }
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpEllipse(rx, ry);
               }
               else
               {
                  cur_path->ellipse(cur_x, cur_y, rx, ry, cur_num_seg);
               }
               break;

            case 0xB3u:  // e (u8.0 even)
               rx = (sF32)( (ifs.getU8() << 1) );
               ry = (sF32)( (ifs.getU8() << 1) );
               if(b_debug_op) { Dprintf("[dbg] e %f %f  (u8.0*2)\n", rx, ry); }
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpEllipse(rx, ry);
               }
               else
               {
                  cur_path->ellipse(cur_x, cur_y, rx, ry, cur_num_seg);
               }
               break;

            case 0xB4u:  // circle (u14.2)
               r = ifs.getU16() * (1.0f / 4.0f);
               if(b_debug_op) { Dprintf("[dbg] circle %f  (u14.2)\n", r); }
               if(MINNIE_PATH_TYPE_IMMEDIATE == cur_path->type)
               {
                  (void)beginDrawListOpEllipse(r, r);
               }
               else
               {
                  cur_path->circle(cur_x, cur_y, r, cur_num_seg);
               }
               break;

            case 0xC0u: // v <pathIdx> <sx> <sy> <ang> (scale+rotate)
               // (todo) remove
               /* oPath <= paths.get(ifs.getU8()); */
               /* sx  =  ifs.getS16() / 4096.0f; */
               /* sy  =  ifs.getS16() / 4096.0f; */
               /* ang = (ifs.getU16() & 16383) * (360.0f / 16384.0f); */
               /* if(b_debug_op) trace "[dbg] v "+opath->path_idx+" "+sx+" "+sy+" "+ang+"  (u8)"; */
               break;

            case 0xC1u: // v <pathIdx> <sx> <sy> <ang> (scale+rotate)
               // (todo) remove
               /* oPath <= paths.get(ifs.getU16()); */
               /* sx  =  ifs.getS16() / 4096.0f; */
               /* sy  =  ifs.getS16() / 4096.0f; */
               /* ang = (ifs.getU16() & 16383) * (360.0f / 16384.0f); */
               /* if(b_debug_op) trace "[dbg] v "+opath->path_idx+" "+sx+" "+sy+" "+ang+"  (u16)"; */
               break;

            case 0xC2u: // v <pathIdx>
               // (todo) remove
               /* oPath <= paths.get(ifs.getU8()); */
               /* if(b_debug_op) trace "[dbg] v "+opath->path_idx+" "+sx+" "+sy+" "+ang+"  (u8)"; */
               break;

            case 0xC3u: // v <pathIdx>
               // (todo) remove
               /* oPath <= paths.get(ifs.getU16()); */
               /* if(b_debug_op) trace "[dbg] v "+opath->path_idx+" "+sx+" "+sy+" "+ang+"  (u16)"; */
               break;

            case 0xD0u: // d <pathIdx>
               pathIdx = ifs.getU8();
               oPath = paths.getRev(pathIdx);
               last_drawn_path = oPath;
               if(NULL != oPath)
               {
                  if(b_debug_op) { Dprintf("[dbg] d %u  (u8)\n", pathIdx); }
                  drawMultiPath(oPath, 0/*mode=notransform*/);
               }
               else
               {
                  Derrorprintf("[---] 0xD0: pathIdx=%u oPath=NULL!!\n", pathIdx);
               }
               break;

            case 0xD1u: // d <pathIdx>
               pathIdx = ifs.getU16();
               oPath = paths.getRev(pathIdx);
               last_drawn_path = oPath;
               if(NULL != oPath)
               {
                  if(b_debug_op) { Dprintf("[dbg] d %u  (u16)\n", pathIdx); }
                  drawMultiPath(oPath, 0/*mode=no transform*/);
               }
               else
               {
                  Derrorprintf("[---] 0xD1: pathIdx=%u oPath=NULL!!\n", pathIdx);
               }
               break;

            case 0xD2u: // d2 <pathIdx>
               pathIdx = ifs.getU8();
               oPath = paths.getRev(pathIdx);
               last_drawn_path = oPath;
               if(NULL != oPath)
               {
                  if(b_debug_op) { Dprintf("[dbg] d2 %u  (u8)\n", pathIdx); }
                  drawMultiPath(oPath, 1/*mode=transform2d*/);
               }
               else
               {
                  Derrorprintf("[---] 0xD2: pathIdx=%u oPath=NULL!!\n", pathIdx);
               }
               break;

            case 0xD3u: // d3 <pathIdx>
               pathIdx = ifs.getU8();
               oPath = paths.getRev(pathIdx);
               last_drawn_path = oPath;
               if(NULL != oPath)
               {
                  if(b_debug_op) { Dprintf("[dbg] d3 %u  (u8)\n", pathIdx); }
                  drawMultiPath(oPath, 2/*mode=transform3d*/);
               }
               else
               {
                  Derrorprintf("[---] 0xD3: pathIdx=%u oPath=NULL!!\n", pathIdx);
               }
               break;

            case 0xD4: // d2 <pathIdx>
               pathIdx = ifs.getU16();
               oPath = paths.getRev(pathIdx);
               last_drawn_path = oPath;
               if(NULL != oPath)
               {
                  if(b_debug_op) { Dprintf("[dbg] d2 %u  (u16)\n", pathIdx); }
                  drawMultiPath(oPath, 1/*mode=transform2d*/);
               }
               else
               {
                  Derrorprintf("[---] 0xD4: pathIdx=%u oPath=NULL!!\n", pathIdx);
               }
               break;

            case 0xD5: // d3 <pathIdx>
               pathIdx = ifs.getU16();
               oPath = paths.getRev(pathIdx);
               last_drawn_path = oPath;
               if(NULL != oPath)
               {
                  if(b_debug_op) { Dprintf("[dbg] d3 %u  (u16)\n", pathIdx); }
                  drawMultiPath(oPath, 2/*mode=transform3d*/);
               }
               else
               {
                  Derrorprintf("[---] 0xD5: pathIdx=%u oPath=NULL!!\n", pathIdx);
               }
               break;

            case 0xFEu: // y
               if(b_debug_op) { Dprintf("[dbg] y\n"); }
               endPath(YAC_FALSE/*bClosed*/);
               break;

            case 0xFFu: // z
               if(b_debug_op) { Dprintf("[dbg] z\n"); }
               endPath(YAC_TRUE/*bClosed*/);
               break;

         }
      }

      return YAC_FALSE;
   }

   // <method.png>
#ifdef SHADERVG_SCRIPT_API
   static void setFramebufferOverride(const sUI _fbIdx, void *_pixelData, const sUI _w, const sUI _h, const sUI _pitch) {
      Dprintf("[dbg] Minnie::setFramebufferOverride: fbIdx=%u pixels=%p w=%u h=%u pitch=%u\n", _fbIdx, _pixelData, _w, _h, _pitch);
      FramebufferOverride *fbOver = &framebuffer_overrides[_fbIdx];
      fbOver->pixels.any = _pixelData;
      fbOver->w     = _w;
      fbOver->h     = _h;
      fbOver->pitch = _pitch;
   }
#endif // SHADERVG_SCRIPT_API

   // <method.png>
   static sU32 initScratchBuffers(void *_data, sU32 _dataSz,
                                  sU32 _defPointsPerPath           =  200u,
                                  sU32 _maxPointsPerPath           =  200u,
                                  sU32 _maxClippedPointsPerPath    =  260u,
                                  sU32 _maxExtrudedVerticesPerPath =  768u,
                                  sU32 _maxClippedTrisPerPath      =  768u
                                  ) {
      memptr_t m; m.any = _data;
      memptr_t mStart = m;
      Dprintf("[trc] Minnie::initScratchBuffers: data=%p dataSz=%u\n", _data, _dataSz);

      def_points_per_path            = _defPointsPerPath;
      max_extruded_vertices_per_path = _maxExtrudedVerticesPerPath;

      // (todo) identify scratchbuffers that can be shared (i.e. not needed simultaneously)

      tmpfa_extrude.init(NULL/*allocator*/, _maxExtrudedVerticesPerPath * 2u, m.any);
      m.f32 += tmpfa_extrude.max_elements;
      /* Dprintf("xxx tmpfa_extrude=%p elements=%p max=%u\n", &tmpfa_extrude, tmpfa_extrude.elements.any, tmpfa_extrude.max_elements); */

      tmpfa_points2.init(NULL/*allocator*/, _maxPointsPerPath * 2u, m.any);
      m.f32 += tmpfa_points2.max_elements;

      tmpia_tesselate_concave.init(NULL/*allocator*/, _maxPointsPerPath, m.any);
      m.s32 += tmpia_tesselate_concave.max_elements;

      tmpfa_clip2.init(NULL/*allocator*/, _maxClippedPointsPerPath * 2u, m.any);
      m.f32 += tmpfa_clip2.max_elements;

      tmpfa_clip2d_tmp1.init(NULL/*allocator*/, _maxClippedPointsPerPath * 2u, m.any);
      m.f32 += tmpfa_clip2d_tmp1.max_elements;

      tmpfa_va3.init(NULL/*allocator*/, _maxClippedPointsPerPath * 3u, m.any);
      m.f32 += tmpfa_va3.max_elements;

      tmpfa_clip3.init(NULL/*allocator*/, _maxClippedPointsPerPath * 3u, m.any);
      m.f32 += tmpfa_clip3.max_elements;

      tmpia_clip3.init(NULL/*allocator*/, _maxClippedTrisPerPath * 3u, m.any);
      m.s32 += tmpia_clip3.max_elements;

      tmpia_clip2.init(NULL/*allocator*/, _maxClippedTrisPerPath * 3u, m.any);
      m.s32 += tmpia_clip2.max_elements;

      tmpia_clip_ia_tri_in.init(NULL/*allocator*/, 3*4, m.any);
      m.s32 += tmpia_clip_ia_tri_in.max_elements;

      tmpia_clip_ia_tri_out.init(NULL/*allocator*/, 3*4, m.any);
      m.s32 += tmpia_clip_ia_tri_out.max_elements;

      tmpfa_proj2.init(NULL/*allocator*/, _maxClippedTrisPerPath * 2u*3u, m.any);
      m.f32 += tmpfa_proj2.max_elements;

      /* Dprintf("xxx m.any=%p mStart.any=%p => reqSz=%u\n", m.any, mStart.any, (sU32)(m.u8 - mStart.u8)); */

#if MINNIE_ALLOC_DEBUG
      if(NULL != _data) // not probing ?
         minnie_alloc_debug_print_stats();
#endif // MINNIE_ALLOC_DEBUG

      // (note) caller must check that scratch buffer is large enough
      return (sU32)(m.u8 - mStart.u8);
   }

} // end namespace minniesetup

} // end namespace minnie

#endif // MINNIE_IMPLEMENTATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#if MINNIE_PROCEDURAL_API

// --------------------------------------------------------------------------
// -                                                                        -
// -                            procedural API                              -
// -                                                                        -
// --------------------------------------------------------------------------

// forward declarations
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
YF void YAC_CALL minBegin (void);
YF void YAC_CALL minEnd (void);
YF void YAC_CALL minFreeDynamic (void);
YF void YAC_CALL minSetStrokeScale (sF32 _scale);
YF void YAC_CALL minSetStrokeOffset (sF32 _offset);
YF sUI YAC_CALL minGetWidth (void);
YF sUI YAC_CALL minGetHeight (void);
YF sUI YAC_CALL minGetColorByIndex (sUI _idx);
#ifdef SHADERVG_SCRIPT_API
YF sBool YAC_CALL minSetFramebufferOverride (sUI _fbIdx, YAC_Object *_pixels, sUI _w, sUI _h, sUI _pitch);  // MINNIE_SW_RENDER
#endif // SHADERVG_SCRIPT_API
YF sUI YAC_CALL minInitScratchBuffers (YAC_Buffer *_buf, sUI _defPointsPerPath, sUI _maxPointsPerPath, sUI _maxClippedPointsPerPath, sUI _maxExtrudedVerticesPerPath, sUI _maxClippedTrisPerPath);
YF sBool YAC_CALL minParseBuffer (YAC_Buffer *_buf);
YF void YAC_CALL minDebugPrintAllocStats (void);
YF void YAC_CALL minDebugPrintPathStats (void);
YF void YAC_CALL minResetAllocStats (void);
YF sUI YAC_CALL minGetTotalNumPoints (void);
YF sUI YAC_CALL minGetTotalNumLines (void);
YF sUI YAC_CALL minGetTotalNumLineStrips (void);
YF sUI YAC_CALL minGetTotalNumTris (void);
YF sUI YAC_CALL minGetTotalNumTrisTex (void);
YF sUI YAC_CALL minGetTotalNumRects (void);
YF sUI YAC_CALL minGetTotalNumEllipses (void);
YF void YAC_CALL minSetEnableDraw (sBool _bEnable);
YF void YAC_CALL minSetEnableDrawSW (sBool _bEnable);
YF void YAC_CALL minSetEnableRenderStrokes (sBool _bEnable);
YF sBool YAC_CALL minGetEnableRenderStrokes (void);
YF void YAC_CALL minSetEnableRenderJoinCap (sBool _bEnable);
YF sBool YAC_CALL minGetEnableRenderJoinCap (void);
YF void YAC_CALL minSetEnableRenderFillConcave (sBool _bEnable);
YF sBool YAC_CALL minGetEnableRenderFillConcave (void);
YF void YAC_CALL minSetEnableRenderFillEvenOdd (sBool _bEnable);
YF sBool YAC_CALL minGetEnableRenderFillEvenOdd (void);
YF void YAC_CALL minSetEnableEdgeAA (sBool _bEnable);
YF sBool YAC_CALL minGetEnableEdgeAA (void);
YF void YAC_CALL minSetEnableTesselateConcave (sBool _bEnable);
YF sBool YAC_CALL minGetEnableTesselateConcave (void);
YF void YAC_CALL minSetSwTesselateSizeThreshold (sUI _sizeThreshold);
YF sUI YAC_CALL minGetSwTesselateSizeThreshold (void);
YF void YAC_CALL minSetEnableForceConcaveEvenOdd (sBool _bEnable);
YF sBool YAC_CALL minGetEnableForceConcaveEvenOdd (void);
YF void YAC_CALL minSetEnableUniformColors (sBool _bEnable);
YF sBool YAC_CALL minGetEnableUniformColors (void);
YF void YAC_CALL minSetStrokeWLineStripThreshold (sF32 _threshold);
YF sF32 YAC_CALL minGetStrokeWLineStripThreshold (void);
YF void YAC_CALL minSetStrokeWLineJoinThreshold (sF32 _threshold);
YF void YAC_CALL minSetVertexBufferExportOFS (YAC_Buffer *_ofs);
YF void YAC_CALL minSetDrawListExportOFS (YAC_Buffer *_ofs);
YF sBool YAC_CALL minIsEdgeAA (void);
YF sBool YAC_CALL minIsVertexFix16 (void);
YF sBool YAC_CALL minIsEdgeAAScaleFix16 (void);
YF void YAC_CALL minSeg (sUI _numSeg);
YF void YAC_CALL minMoveTo (sF32 _x, sF32 _y);
YF void YAC_CALL minLineTo (sF32 _dstX, sF32 _dstY);
YF void YAC_CALL minCubicTo (sF32 _c1x, sF32 _c1y, sF32 _c2x, sF32 _c2y, sF32 _dstX, sF32 _dstY);
YF void YAC_CALL minCubicMirrorTo (sF32 _c2x, sF32 _c2y, sF32 _dstX, sF32 _dstY);
YF void YAC_CALL minArcTo (sF32 _rx, sF32 _ry, sF32 _rot, sBool _bLargeArc, sBool _bArcSweep, sF32 _dstX, sF32 _dstY);
YF void YAC_CALL minRect (sF32 _w, sF32 _h);
YF void YAC_CALL minRoundRect (sF32 _w, sF32 _h, sF32 _rx, sF32 _ry);
YF void YAC_CALL minEllipse (sF32 _rx, sF32 _ry);
YF void YAC_CALL minCircle (sF32 _r);
YF sSI YAC_CALL minBeginPathEx (sSI _pathType);
YF sSI YAC_CALL minBeginPath (void);
YF sSI YAC_CALL minBeginPathConvex (void);
YF sSI YAC_CALL minBeginPathConcave (void);
YF sSI YAC_CALL minBeginPathEvenOdd (void);
YF void YAC_CALL minBeginImmediate (void);
YF void YAC_CALL minEndImmediate (void);
YF void YAC_CALL minEndPath (sBool _bClosed);
YF void YAC_CALL minEndPathOpen (void);
YF void YAC_CALL minEndPathClosed (void);
YF sSI YAC_CALL minBeginSubEx (sSI _pathType);
YF sSI YAC_CALL minBeginSub (void);
YF sSI YAC_CALL minBeginSubConvex (void);
YF sSI YAC_CALL minBeginSubConcave (void);
YF sSI YAC_CALL minBeginSubEvenOdd (void);
YF void YAC_CALL minEndSub (sBool _bClosed);
YF void YAC_CALL minEndSubOpen (void);
YF void YAC_CALL minEndSubClosed (void);
YF void YAC_CALL minStrokeWidth (sF32 _w);
YF void YAC_CALL minFill (void);
YF void YAC_CALL minMiterLimit (sF32 _l);
YF void YAC_CALL minColor (sUI _c32);
YF void YAC_CALL minJoinCap (sUI _joinCap);
YF void YAC_CALL minJoin (sUI _joinType);
YF void YAC_CALL minJoinNone (void);
YF void YAC_CALL minJoinMiter (void);
YF void YAC_CALL minJoinRound (void);
YF void YAC_CALL minJoinBevel (void);
YF void YAC_CALL minCap (sUI _capType);
YF void YAC_CALL minCapNone (void);
YF void YAC_CALL minCapRound (void);
YF void YAC_CALL minCapButt (void);
YF void YAC_CALL minDrawPath (sSI _pathIdx);
YF void YAC_CALL minBindTexture (sSI _texId, sBool _bRepeat, sBool _bFilter);
YF void YAC_CALL minUnbindTexture (void);
YF void YAC_CALL minTextureDecalAlpha (sF32 _decalAlpha);
YF void YAC_CALL minTriangleTexUVFlat (sF32 _x1, sF32 _y1, sF32 _u1, sF32 _v1, sF32 _x2, sF32 _y2, sF32 _u2, sF32 _v2, sF32 _x3, sF32 _y3, sF32 _u3, sF32 _v3);
YF void YAC_CALL minTriangleTexUVFlatDecal (sF32 _x1, sF32 _y1, sF32 _u1, sF32 _v1, sF32 _x2, sF32 _y2, sF32 _u2, sF32 _v2, sF32 _x3, sF32 _y3, sF32 _u3, sF32 _v3);
YF void YAC_CALL minTriangleTexUVGouraud (sF32 _x1, sF32 _y1, sF32 _u1, sF32 _v1, sU32 _c32v1, sF32 _x2, sF32 _y2, sF32 _u2, sF32 _v2, sU32 _c32v2, sF32 _x3, sF32 _y3, sF32 _u3, sF32 _v3, sU32 _c32v3);
YF void YAC_CALL minTriangleTexUVGouraudDecal (sF32 _x1, sF32 _y1, sF32 _u1, sF32 _v1, sU32 _c32v1, sF32 _x2, sF32 _y2, sF32 _u2, sF32 _v2, sU32 _c32v2, sF32 _x3, sF32 _y3, sF32 _u3, sF32 _v3, sU32 _c32v3);
YF void YAC_CALL minRectTexUVFlat (sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb);
YF void YAC_CALL minRectTexUVFlatDecal (sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb);
YF void YAC_CALL minRectTexUVGouraud (sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb, sU32 _c32lt, sU32 _c32rt, sU32 _c32rb, sU32 _c32lb);
YF void YAC_CALL minRectTexUVGouraudDecal (sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb, sU32 _c32lt, sU32 _c32rt, sU32 _c32rb, sU32 _c32lb);
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef MINNIE_IMPLEMENTATION  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* @function minBegin
Free paths, reset state and start new drawing.

@see minEnd
*/
void minBegin(void) {
   minnie::setup::reset();
   minnie::setup::begin();
}

/* @function minEnd
End drawing

@see minBegin
*/
void minEnd(void) {
   minnie::setup::end();
}

/* @function minFreeDynamic
Free dynamically allocated memory (paths)

@see minBegin
*/
void minFreeDynamic(void) {
   minnie::setup::freeDynamic();
}

/* @function minSetStrokeScale,float scale
Set stroke width scaling factor. Applied to next path draw calls.
*/
void minSetStrokeScale(sF32 _scale) {
   minnie::setup::stroke_scale = _scale;
}

/* @function minSetStrokeOffset,float offset
Set stroke width offset. Applied to next path draw calls.
*/
void minSetStrokeOffset(sF32 _offset) {
   minnie::setup::stroke_offset = _offset;
}

/* @function minGetWidth:int
Get canvas width (pixels)
*/
sUI minGetWidth(void) {
   return minnie::setup::geo_w;
}

/* @function minGetHeight:int
Get canvas height (pixels)
*/
sUI minGetHeight(void) {
   return minnie::setup::geo_h;
}

/* @function minGetColorByIndex,int idx:int
Get ARGB32 color from current palette.

Can e.g. be used to query the background color of an imported SVG object.
*/
sUI minGetColorByIndex(sUI _idx) {
   if(_idx < minnie::setup::palette.num_elements)
      return minnie::setup::palette.elements.u32[_idx];
   return 0u;
}

/* @function minSetFramebufferOverride,int fbIdx,Texture tex,int w,int h,int pitch:boolean
Override framebuffer geometry and storage. Only applies to SW render mode.
*/
#ifdef SHADERVG_SCRIPT_API
sBool minSetFramebufferOverride(sUI _fbIdx, YAC_Object *_pixels, sUI _w, sUI _h, sUI _pitch) {
   // override framebuffer
   if(_fbIdx < MINNIE_MAX_FRAMEBUFFERS)
   {
      if(YAC_VALID(_pixels))
      {
         void *pixelData = _pixels->yacArrayGetPointer();
         if(NULL != pixelData)
         {
            const sUI elemByteSz = _pixels->yacArrayGetElementByteSize();
            const sUI pixelW = _pixels->yacArrayGetWidth();
            sUI pixelH = _pixels->yacArrayGetHeight();
            if(pixelH < 1u)
               pixelH = 1u;
            const sUI pixelDataAvail = elemByteSz * pixelW * pixelH;
            if(0 == _pitch)
               _pitch = (_w * sizeof(sU32));
            const sUI pixelDataReq = (_h * _pitch);
            if(pixelDataReq <= pixelDataAvail)
            {
               minnie::setup::setFramebufferOverride(_fbIdx, pixelData, _w, _h, _pitch);
               return YAC_TRUE;
            }
            else
            {
               Dprintf("[---] Minnie::setFramebuffer: fb geometry(%ux%u pitch=%u) out of bounds (pixelDataAvail=%u need=%u)\n", _w, _h, _pitch, pixelDataAvail, pixelDataReq);
            }
         }
         else
         {
            Dprintf("[---] Minnie::setFramebuffer: fbIdx=%u out of bounds (max=%u)\n", _fbIdx, MINNIE_MAX_FRAMEBUFFERS);
         }
      }
      else
      {
         Dprintf("[---] Minnie::setFramebuffer: invalid pixels object\n");
      }
   }
   else
   {
      Dprintf("[---] Minnie::setFramebuffer: fbIdx=%u out of bounds (max=%u)\n", _fbIdx, MINNIE_MAX_FRAMEBUFFERS);
   }
   return YAC_FALSE;
}
#endif // SHADERVG_SCRIPT_API

//   int _defPointsPerPath           =  200u,
//   int _maxPointsPerPath           =  200u,
//   int _maxClippedPointsPerPath    =  300u,
//   int _maxExtrudedVerticesPerPath = 1024u,
//   int _maxClippedTrisPerPath      = 1024u
// returns required scratch buffer size (must be <= buf.size)
/* @function minInitScratchBuffers,Buffer buf,int defPointsPerPath,int maxPointsPerPath,int maxClippedPointsPerPath,int maxExtrudedVerticesPerPath,int maxClippedTrisPerPath:int
Initialize scratchbuffer or calculate required scratch buffer memory.
@arg Buffer Scratch buffer storage. null=don't initialize and simply return required buffer size
@arg defPointsPerPath Typical number of control points per path
@arg maxPointsPerPath Maximum number of control points per path
@arg maxClippedPointsPerPath Maximum number of clipped points per path
@arg maxExtrudedVerticesPerPath Maximum number of extruded (line-)vertices per path
@arg maxClippedTrisPerPath Maximum number of clipped triangles per path
*/
sUI minInitScratchBuffers(YAC_Buffer *_buf,
                          sUI _defPointsPerPath,
                          sUI _maxPointsPerPath,
                          sUI _maxClippedPointsPerPath,
                          sUI _maxExtrudedVerticesPerPath,
                          sUI _maxClippedTrisPerPath
                          ) {
   void *bufPtr;
   sUI bufSz;
   if(YAC_Is_Buffer(_buf))
   {
      YAC_CAST_ARG(YAC_Buffer, buf, _buf);
      bufPtr = (void*)buf->buffer;
      bufSz  = buf->size;
   }
   else
   {
      // Probe
      bufPtr = NULL;
      bufSz  = 0u;
   }
   return minnie::setup::initScratchBuffers(bufPtr, bufSz,
                                            _defPointsPerPath,
                                            _maxPointsPerPath,
                                            _maxClippedPointsPerPath,
                                            _maxExtrudedVerticesPerPath,
                                            _maxClippedTrisPerPath
                                            );
}

/* @function minParseBuffer,Buffer buf:boolean
Parse MIB stream.

The MIB data can either be generated by the application, or be converted from e.g. an SVG file via an offline preprocessor.

@arg buf §File or §Buffer stream
*/
sBool minParseBuffer(YAC_Buffer *_buf) {
   if(YAC_Is_Buffer(_buf))
   {
      YAC_CAST_ARG(YAC_Buffer, buf, _buf);
      return minnie::setup::parseBuffer(NULL/*allocator*/, (void*)buf->buffer, buf->size);
   }
   return YAC_FALSE;
}

/* @function minDebugPrintAllocStats
Debug-print allocation statistics to the console
*/
void minDebugPrintAllocStats(void) {
#if MINNIE_ALLOC_DEBUG
   minnie::minnie_alloc_debug_print_stats();
#endif // MINNIE_ALLOC_DEBUG
}

/* @function minDebugPrintPathStats
Debug-print path statistics to the console
*/
void minDebugPrintPathStats(void) {
   Dyac_host_printf("[dbg] Minnie::debugPrintPathStats: ~~~~~~~~~~~~~ #paths=%u #strokeC=%u(%u pts) #strokeO=%u(%u pts) #fillSmall=%u(%u pts) #fillLarge=%u(%u pts) #swTess=%u\n", minnie::debug_num_paths, minnie::debug_num_stroke_closed_paths, minnie::debug_num_stroke_closed_path_points, minnie::debug_num_stroke_open_paths, minnie::debug_num_stroke_open_path_points, minnie::debug_num_small_paths, minnie::debug_num_small_path_points, minnie::debug_num_large_paths, minnie::debug_num_large_path_points, minnie::debug_num_swtess_paths);
}

/* @function minResetAllocStats
Reset allocation debug statistics
*/
void minResetAllocStats(void) {
#if MINNIE_ALLOC_DEBUG
   minnie::minnie_reset_alloc_stats();
#endif // MINNIE_ALLOC_DEBUG
}

/* @function minGetTotalNumPoints:int
Query total number of points (debug)
*/
sUI minGetTotalNumPoints(void) {
   return minnie::total_num_points;
}

/* @function minGetTotalNumLines:int
Query total number of lines (debug)
*/
sUI minGetTotalNumLines(void) {
   return minnie::total_num_lines;
}

/* @function minGetTotalNumLineStrips:int
Query total number of line strips (debug)
*/
sUI minGetTotalNumLineStrips(void) {
   return minnie::total_num_line_strips;
}

/* @function minGetTotalNumTris:int
Query total number of triangles (debug)
*/
sUI minGetTotalNumTris(void) {
   return minnie::total_num_tris;
}

/* @function minGetTotalNumTrisTex:int
Query total number of textured triangles (debug)
*/
sUI minGetTotalNumTrisTex(void) {
   return minnie::total_num_tris_tex;
}

/* @function minGetTotalNumRects:int
Query total number of rectangles (debug)
*/
sUI minGetTotalNumRects(void) {
   return minnie::total_num_rects;
}

/* @function minGetTotalNumEllipses:int
Query total number of ellipses and circles (debug)
*/
sUI minGetTotalNumEllipses(void) {
   return minnie::total_num_ellipses;
}

/* @function minSetEnableDraw,boolean bEnable
Enable or disable path drawing (debug)
*/
void minSetEnableDraw(sBool _bEnable) {
   minnie::setup::setEnableDraw(_bEnable);
}

/* @function minSetEnableDrawSW,boolean bEnable
Enable or disable software rasterizer (debug)
*/
void minSetEnableDrawSW(sBool _bEnable) {
   minnie::setup::setEnableDrawSW(_bEnable);
}

/* @function minSetEnableRenderStrokes,boolean bEnable
Enable or disable stroked path rendering (debug)

@see minGetEnableRenderStrokes
*/
void minSetEnableRenderStrokes(sBool _bEnable) {
   minnie::setup::setEnableRenderStrokes(_bEnable);
}

/* @function minGetEnableRenderStrokes:boolean
Query stroked path rendering enable-state (debug)

@see minSetEnableRenderStrokes
*/
sBool minGetEnableRenderStrokes(void) {
   return minnie::setup::getEnableRenderStrokes();
}

/* @function minSetEnableRenderJoinCap,boolean bEnable
Enable or disable line joints and caps (debug)

@see minGetEnableRenderJoinCap
*/
void minSetEnableRenderJoinCap(sBool _bEnable) {
   minnie::setup::setEnableRenderJoinCap(_bEnable);
}

/* @function minGetEnableRenderJoinCap:boolean
Query line join and cap enable-state (debug)

@see minSetEnableRenderJoinCap
*/
sBool minGetEnableRenderJoinCap(void) {
   return minnie::setup::getEnableRenderJoinCap();
}

/* @function minSetEnableRenderFillConcave,boolean bEnable
Enable or disable concave path rendering (debug)

@see minGetEnableRenderFillConcave
*/
void minSetEnableRenderFillConcave(sBool _bEnable) {
   minnie::setup::setEnableRenderFillConcave(_bEnable);
}

/* @function minGetEnableRenderFillConcave:boolean
Query concave path rendering enable-state (debug)

@see minSetEnableRenderFillConcave
*/
sBool minGetEnableRenderFillConcave(void) {
   return minnie::setup::getEnableRenderFillConcave();
}

/* @function minSetEnableRenderFillEvenOdd,boolean bEnable
Enable or disable even-odd path rendering (debug)

@see minGetEnableRenderFillEvenOdd
*/
void minSetEnableRenderFillEvenOdd(sBool _bEnable) {
   minnie::setup::setEnableRenderFillEvenOdd(_bEnable);
}

/* @function minGetEnableRenderFillEvenOdd:boolean
Query even-odd path rendering enable-state (debug)

@see minSetEnableRenderFillEvenOdd
*/
sBool minGetEnableRenderFillEvenOdd(void) {
   return minnie::setup::getEnableRenderFillEvenOdd();
}

/* @function minSetEnableEdgeAA,boolean bEnable
Enable or disable edge AA triangle rendering (experimental)

@see minGetEnableEdgeAA
*/
void minSetEnableEdgeAA(sBool _bEnable) {
   minnie::setup::setEnableEdgeAA(_bEnable);
}

/* @function minGetEnableEdgeAA:boolean
Query edge AA triangle rendering enable-state (experimental)

@see minSetEnableEdgeAA
*/
sBool minGetEnableEdgeAA(void) {
   return minnie::setup::getEnableEdgeAA();
}

/* @function minSetEnableTesselateConcave,boolean bEnable
Enable or disable concave polygon software tesselation (false=use HW accelerated polygons)

@see minGetEnableTesselateConcave
*/
void minSetEnableTesselateConcave(sBool _bEnable) {
   minnie::setup::setEnableTesselateConcave(_bEnable);
}

/* @function minGetEnableTesselateConcave:boolean
Query concave polygon software tesselation enable-state

@see minSetEnableTesselateConcave
*/
sBool minGetEnableTesselateConcave(void) {
   return minnie::setup::getEnableTesselateConcave();
}

/* @function minSetSwTesselateSizeThreshold,int sizeThreshold
Bounding box area threshold for polygon software tesselation. 0=disable SW tesselation

@see minGetSwTesselateSizeThreshold
*/
void minSetSwTesselateSizeThreshold(sUI _sizeThreshold) {
   minnie::setup::setSwTesselateSizeThreshold(_sizeThreshold);
}

/* @function minGetSwTesselateSizeThreshold:int
Query software tesselation polygon bounding box area threshold.

@see minSetSwTesselateSizeThreshold
*/
sUI minGetSwTesselateSizeThreshold(void) {
   return minnie::setup::getSwTesselateSizeThreshold();
}

/* @function minSetEnableForceConcaveEvenOdd,boolean bEnable
Force concave polygons to be rendered using the even-odd code paths

@see minGetEnableForceConcaveEvenOdd
*/
void minSetEnableForceConcaveEvenOdd(sBool _bEnable) {
   minnie::setup::setEnableForceConcaveEvenOdd(_bEnable);
}

/* @function minGetEnableForceConcaveEvenOdd:boolean
Query concave-polygons-via-even-odd-code-path enable-state

@see minSetEnableForceConcaveEvenOdd
*/
sBool minGetEnableForceConcaveEvenOdd(void) {
   return minnie::setup::getEnableForceConcaveEvenOdd();
}

/* @function minSetEnableUniformColors,boolean bEnable
Enable or disable uniform colors.

When enabled, colors are passed via shader uniforms (constant for the duration of a draw call, decreases memory usage).
When disabled, colors are passed via shader attributes, which allows multiple (triangle) draw calls to be merged (but increases memory usage).

@see minGetEnableUniformColors
*/
void minSetEnableUniformColors(sBool _bEnable) {
   minnie::setup::setEnableUniformColors(_bEnable);
}

/* @function minGetEnableUniformColors:boolean
Query uniform-colors enable-state.

@see minSetEnableUniformColors
*/
sBool minGetEnableUniformColors(void) {
   return minnie::setup::getEnableUniformColors();
}

/* @function minSetStrokeWLineStripThreshold,float threshold
Set line stroke width software tesselation threshold (999999=disable SW tesselation).

Fall back to software tesselation if the stroke width exceeds the thresold.

@see minGetStrokeWLineStripThreshold
*/
void minSetStrokeWLineStripThreshold(sF32 _threshold) {
   minnie::setup::setStrokeWLineStripThreshold(_threshold);
}

/* @function minGetStrokeWLineStripThreshold:float
Query line stroke width software tesselation threshold

@see minSetStrokeWLineStripThreshold
*/
sF32 minGetStrokeWLineStripThreshold(void) {
   return minnie::setup::getStrokeWLineStripThreshold();
}

/* @function minSetStrokeWLineJoinThreshold,float threshold
Set stroke width line-join-auto-disable threshold.

Allows line joints to be disabled for narrow lines.

@see minGetStrokeWLineJoinThreshold
*/
void minSetStrokeWLineJoinThreshold(sF32 _threshold) {
   minnie::setup::setStrokeWLineJoinThreshold(_threshold);
}

/* @function minGetStrokeWLineJoinThreshold:float
Query line-join-auto-disable threshold.

@see minSetStrokeWLineJoinThreshold
*/
sF32 minGetStrokeWLineJoinThreshold(void) {
   return minnie::setup::getStrokeWLineJoinThreshold();
}

/* @function minSetVertexBufferExportOFS,Stream ofs
Set vertex attribute output stream.

@arg ofs Output stream (usually a §Buffer)

@see minSetDrawListExportOFS
*/
void minSetVertexBufferExportOFS(YAC_Buffer *_ofs) {
   if(YAC_VALID(_ofs))
      minnie::setup::setVertexBufferExportOFS((void*)_ofs);
   else
      minnie::setup::setVertexBufferExportOFS(NULL);
}

/* @function minSetDrawListExportOFS,Stream ofs
Set draw list output stream.

@arg ofs Output stream (usually a §Buffer)

@see minSetVertexBufferExportOFS
*/
void minSetDrawListExportOFS(YAC_Buffer *_ofs) {
   if(YAC_VALID(_ofs))
      minnie::setup::setDrawListExportOFS((void*)_ofs);
   else
      minnie::setup::setDrawListExportOFS(NULL);
}

/* @function minIsEdgeAA:boolean
experimental
*/
sBool minIsEdgeAA(void) {
#if MINNIE_EXPORT_TRIS_EDGEAA
   return YAC_TRUE;
#else
   return YAC_FALSE;
#endif // MINNIE_EXPORT_TRIS_EDGEAA
}

/* @function minIsVertexFix16:boolean
Query whether 14:2 fixed point vertex attribute mode was enabled at compile time
*/
sBool minIsVertexFix16(void) {
#if MINNIE_EXPORT_VERTEX_16BIT
   return YAC_TRUE;
#else
   return YAC_FALSE;
#endif // MINNIE_EXPORT_VERTEX_16BIT
}

/* @function minIsEdgeAAScaleFix16:boolean
experimental
*/
sBool minIsEdgeAAScaleFix16(void) {
#if MINNIE_EXPORT_VERTEX_16BIT && MINNIE_EXPORT_TRIS_EDGEAA_SCALE_16BIT
   return YAC_TRUE;
#else
   return YAC_FALSE;
#endif // MINNIE_EXPORT_VERTEX_16BIT && MINNIE_EXPORT_TRIS_EDGEAA_SCALE_16BIT
}

/* @function minSeg,int numSeg
Set number of subdivisions for next bezier curve / arc path segments
*/
void minSeg(sUI _numSeg) {
   minnie::setup::cur_num_seg = _numSeg;
}

/* @function minMoveTo,float x,float y
Move path cursor to absolute position

@arg x X position (abs)
@arg y Y position (abs)
*/
void minMoveTo(sF32 _x, sF32 _y) {
   // (note) WIP
   // 0x22  M f32
   minnie::setup::cur_x = _x;
   minnie::setup::cur_y = _y;
}

/* @function minLineTo,float dstX,float dstY
Add line segment to current path

@arg dstX Destination X (abs)
@arg dstY Destination Y (abs)
*/
void minLineTo(sF32 _dstX, sF32 _dstY) {
   if(NULL != minnie::setup::cur_path)
   {
      minnie::setup::last_x = minnie::setup::cur_x;
      minnie::setup::last_y = minnie::setup::cur_y;
      minnie::setup::cur_x = _dstX;
      minnie::setup::cur_y = _dstY;
      minnie::setup::cur_path->lazyBegin(minnie::setup::last_x, minnie::setup::last_y);
      minnie::setup::cur_path->lineTo(minnie::setup::cur_x, minnie::setup::cur_y);
      minnie::setup::cur_mirror_x = 0.0f;
      minnie::setup::cur_mirror_y = 0.0f;
   }
   else
   {
      Dapierror("[---] Minnie::apiLineTo: cur_path is NULL\n");
   }
}

/* @function minCubicTo,float c1x,float c1y,float c2x,float c2y,float dstX,float dstY
Add cubic spline segment to current path

@arg c1x Control point 1 X (abs)
@arg c1y Control point 1 Y (abs)
@arg c2x Control point 2 X (abs)
@arg c2y Control point 2 Y (abs)
@arg dstX Destination X (abs)
@arg dstY Destination Y (abs)

@see minCubicMirrorTo
*/
void minCubicTo(sF32 _c1x, sF32 _c1y, sF32 _c2x, sF32 _c2y, sF32 _dstX, sF32 _dstY) {
   if(NULL != minnie::setup::cur_path)
   {
      minnie::setup::cur_mirror_x = _dstX - _c2x;
      minnie::setup::cur_mirror_y = _dstY - _c2y;
      minnie::setup::cur_path->lazyBegin(minnie::setup::cur_x, minnie::setup::cur_y);
      minnie::setup::cur_path->cubicTo(minnie::setup::cur_x, minnie::setup::cur_y,
                                       _c1x,   _c1y,
                                       _c2x,   _c2y,
                                       _dstX,  _dstY,
                                       minnie::setup::cur_num_seg,
                                       minnie::setup::cubic_min_dist_sqr
                                       );
      minnie::setup::cur_x = _dstX;
      minnie::setup::cur_y = _dstY;
   }
   else
   {
      Dapierror("[---] Minnie::apiCubicTo: cur_path is NULL\n");
   }
}

/* @function minCubicMirrorTo,float c2x,float c2y,float dstX,float dstY
Add smoothed / mirrored cubic spline segment to current path.

The implicite control point 1 is calculated by mirroring the previous segment's control point 2.

@arg c2x Control point 2 X (abs)
@arg c2y Control point 2 Y (abs)
@arg dstX Destination X (abs)
@arg dstY Destination Y (abs)

@see minCubicTo
*/
void minCubicMirrorTo(sF32 _c2x, sF32 _c2y, sF32 _dstX, sF32 _dstY) {
   if(NULL != minnie::setup::cur_path)
   {
      sF32 c1x     = minnie::setup::cur_x + minnie::setup::cur_mirror_x;
      sF32 c1y     = minnie::setup::cur_y + minnie::setup::cur_mirror_y;
      minnie::setup::cur_mirror_x = _dstX - _c2x;
      minnie::setup::cur_mirror_y = _dstY - _c2y;
      minnie::setup::cur_path->lazyBegin(minnie::setup::cur_x, minnie::setup::cur_y);
      minnie::setup::cur_path->cubicTo(minnie::setup::cur_x, minnie::setup::cur_y,
                                       c1x,   c1y,
                                       _c2x,  _c2y,
                                       _dstX, _dstY,
                                       minnie::setup::cur_num_seg,
                                       minnie::setup::cubic_min_dist_sqr
                                       );
      minnie::setup::cur_x   = _dstX;
      minnie::setup::cur_y   = _dstY;
   }
   else
   {
      Dapierror("[---] Minnie::apiCubicMirrorTo: cur_path is NULL\n");
   }
}

/* @function minArcTo,float rx,float ry,float rot,boolean bLargeArc,boolean bArcSweep,float dstX,float dstY
Add elliptic arc segment to current path

@arg rx Radius x
@arg ry Radius y
@arg rot Rotation (radian measure)
@arg bLargeArc Large arc flag
@arg bArcSweep Arc sweep flag
@arg dstX Destination X (abs)
@arg dstY Destination Y (abs)
*/
void minArcTo(sF32 _rx, sF32 _ry,
              sF32 _rot,
              sBool _bLargeArc,
              sBool _bArcSweep,
              sF32 _dstX, sF32 _dstY
              ) {
   if(NULL != minnie::setup::cur_path)
   {
      minnie::setup::last_x = minnie::setup::cur_x;
      minnie::setup::last_y = minnie::setup::cur_y;
      minnie::setup::cur_path->lazyBegin(minnie::setup::last_x, minnie::setup::last_y);
      minnie::setup::cur_path->arcTo(minnie::setup::cur_x, minnie::setup::cur_y,
                                     _rx, _ry,
                                     _rot,
                                     _bLargeArc, _bArcSweep,
                                     _dstX, _dstY,
                                     minnie::setup::cur_num_seg
                                     );
      minnie::setup::cur_x = _dstX;
      minnie::setup::cur_y = _dstY;
   }
   else
   {
      Dapierror("[---] Minnie::apiArcTo: cur_path is NULL\n");
   }
}

/* @function minRect,float w,float h
Add rectangle segment to current path (cursor position denotes left / top corner)
@arg w Width
@arg h Height

@see minRoundRect
*/
void minRect(sF32 _w, sF32 _h) {
   if(NULL != minnie::setup::cur_path)
   {
      if(MINNIE_PATH_TYPE_IMMEDIATE == minnie::setup::cur_path->type)
      {
         (void)minnie::setup::beginDrawListOpRect(_w, _h);
      }
      else
      {
         minnie::setup::cur_path->rect(minnie::setup::cur_x, minnie::setup::cur_y, _w, _h);
      }
   }
   else
   {
      Dapierror("[---] Minnie::apiRect: cur_path is NULL\n");
   }
}

/* @function minRoundRect,float w,float h,float rx,float ry
Add rounded rectangle segment to current path (cursor position denotes left / top corner).
@arg w Width
@arg h Height
@arg rx Corner radius x
@arg ry Corner radius y

@see minRect
*/
void minRoundRect(sF32 _w, sF32 _h, sF32 _rx, sF32 _ry) {
   if(NULL != minnie::setup::cur_path)
   {
      if(MINNIE_PATH_TYPE_IMMEDIATE == minnie::setup::cur_path->type)
      {
         (void)minnie::setup::beginDrawListOpRoundRect(_w, _h, _rx, _ry);
      }
      else
      {
         minnie::setup::cur_path->roundRect(minnie::setup::cur_x, minnie::setup::cur_y,
                                          _w, _h, _rx, _ry,
                                          minnie::setup::cur_num_seg,
                                          minnie::setup::cubic_min_dist_sqr
                                          );
      }
   }
   else
   {
      Dapierror("[---] Minnie::apiRoundRect: cur_path is NULL\n");
   }
}

/* @function minEllipse,float rx,float ry
Add ellipse segment to current path (cursor position denotes center)
@arg rx Radius x
@arg ry Radius y

@see minCircle
*/
void minEllipse(sF32 _rx, sF32 _ry) {
   if(NULL != minnie::setup::cur_path)
   {
      if(MINNIE_PATH_TYPE_IMMEDIATE == minnie::setup::cur_path->type)
      {
         (void)minnie::setup::beginDrawListOpEllipse(_rx, _ry);
      }
      else
      {
         minnie::setup::cur_path->ellipse(minnie::setup::cur_x, minnie::setup::cur_y,
                                           _rx, _ry,
                                           minnie::setup::cur_num_seg
                                           );
      }
   }
   else
   {
      Dapierror("[---] Minnie::apiEllipse: cur_path is NULL\n");
   }
}

/* @function minCircle
Add circle segment to current path (cursor position denotes center)
@arg r Radius

@see minEllipse
*/
void minCircle(sF32 _r) {
   if(NULL != minnie::setup::cur_path)
   {
      if(MINNIE_PATH_TYPE_IMMEDIATE == minnie::setup::cur_path->type)
      {
         (void)minnie::setup::beginDrawListOpEllipse(_r, _r);
      }
      else
      {
         minnie::setup::cur_path->ellipse(minnie::setup::cur_x, minnie::setup::cur_y,
                                        _r, _r,
                                        minnie::setup::cur_num_seg
                                        );
      }
   }
   else
   {
      Dapierror("[---] Minnie::apiCircle: cur_path is NULL\n");
   }
}

/* @function minBeginPathEx,int pathType:int
Begin new path definition.

@arg pathType See §MINNIE_PATH_TYPE_CONVEX, §MINNIE_PATH_TYPE_CONCAVE, §MINNIE_PATH_TYPE_EVENODD, §MINNIE_PATH_TYPE_IMMEDIATE

@return Path id

@see minBeginPath, minBeginPathConvex, minBeginPathConcave, minBeginPathEvenOdd, minEndPath
*/
sSI minBeginPathEx(sSI _pathType) {
   // begin new path declaration. returns path_idx or -1 (error)
   //   pathType: MINNIE_PATH_TYPE_xxx (CONVEX|CONCAVE|EVENODD|IMMEDIATE)
   if(minnie::setup::newPath(_pathType, "api"/*debugCmd*/, "apiPathBegin"/*debugName*/))
   {
      return sSI(minnie::setup::cur_path->path_idx);
   }
   return -1;
}

/* @function minBeginPath:int
Begin new Even-Odd path definition
@return Path id

@see minEndPath
*/
sSI minBeginPath(void) {
   return minBeginPathEx(MINNIE_PATH_TYPE_EVENODD);
}

/* @function minBeginPathConvex:int
Begin new Convex path definition
@return Path id

@see minEndPath
*/
sSI minBeginPathConvex(void) {
   return minBeginPathEx(MINNIE_PATH_TYPE_CONVEX);
}

/* @function minBeginPathConcave:int
Begin new Concave path definition
@return Path id

@see minEndPath
*/
sSI minBeginPathConcave(void) {
   return minBeginPathEx(MINNIE_PATH_TYPE_CONCAVE);
}

/* @function minBeginPathEvenOdd:int
Begin new Even-Odd path definition
@return Path id

@see minEndPath
*/
sSI minBeginPathEvenOdd(void) {
   return minBeginPathEx(MINNIE_PATH_TYPE_EVENODD);
}

/* @function minBeginPathImmediate:int
Begin new Immediate path definition.

Graphics elements within immediate paths are rendered immediately, i.e. they do not require separate draw-path calls.

Immediate paths are particularly useful for graphics elements like UV-textured triangles and rectangles, which cannot be added to regular paths.

@return Path id

@see minEndImmediate
*/
void minBeginImmediate(void) {
   (void)minBeginPathEx(MINNIE_PATH_TYPE_IMMEDIATE);
}

/* @function minEndImmediate
End Immediate path definition
*/
void minEndImmediate(void) {
   minnie::setup::endPath(YAC_FALSE/*bClosed*/);
}

/* @function minEndPath,boolean bClosed
End path definition
*/
void minEndPath(sBool _bClosed) {
   minnie::setup::endPath(_bClosed);
}

/* @function minEndPathOpen
End path definition and keep path open (e.g. polylines)
*/
void minEndPathOpen(void) {
   minnie::setup::endPath(YAC_FALSE/*bClosed*/);
}

/* @function minEndPathClosed
End path definition and close path (connect last to first, e.g. polygons)
*/
void minEndPathClosed(void) {
   minnie::setup::endPath(YAC_TRUE/*bClosed*/);
}

/* @function minBeginSubEx,int pathType:int
Begin new sub-path definition.

Sub paths can e.g. be used to cut holes into the parent (even-odd type) path.
@return Sub-path id
*/
sSI minBeginSubEx(sSI _pathType) {
   // (note) return value: >=0 if subpath was created, -1 on error
   if(NULL != minnie::setup::last_parent_path)
   {
      minnie::setup::cur_path = minnie::setup::last_parent_path->sub_paths.addNew();
      if(NULL != minnie::setup::cur_path)
      {
         minnie::setup::cur_path->init(minnie::setup::allocator,
                                     999999u/*pathIdx*/,
                                     minnie::setup::last_parent_path->type,
                                     NULL/*pointsData*/, minnie::setup::def_points_per_path*2u/*pointsMaxElements*/,
                                     minnie::setup::tmpfa_extrude.elements.any, minnie::setup::max_extruded_vertices_per_path*2u/*maxExtrudedElements*/
                                     );
         minnie::setup::cur_path->parent_path = minnie::setup::last_parent_path;
         minnie::setup::cur_x = 0.0f;
         minnie::setup::cur_y = 0.0f;
         minnie::setup::cur_mirror_x = 0.0f;
         minnie::setup::cur_mirror_y = 0.0f;
         return minnie::setup::cur_path->path_idx;
      }
      else
      {
         Dapierror("[---] Minnie::apiPathBeginSub: max sub paths(%u) exceeded\n", minnie::setup::last_parent_path->sub_paths.max_elements);
         return -1;
      }
   }
   else
   {
      Dapierror("[---] Minnie::apiPathBeginSub: no parent path\n");
      return -1;
   }
}

/* @function minBeginSub:int
Begin new Even-Odd sub-path definition
@return Sub-path id
*/
sSI minBeginSub(void) {
   return minBeginSubEx(MINNIE_PATH_TYPE_EVENODD);
}

/* @function minBeginSubConvex:int
Begin new Convex sub-path definition
@return Sub-path id
*/
sSI minBeginSubConvex(void) {
   return minBeginSubEx(MINNIE_PATH_TYPE_CONVEX);
}

/* @function minBeginSubConcave:int
Begin new Concave sub-path definition
@return Sub-path id
*/
sSI minBeginSubConcave(void) {
   return minBeginSubEx(MINNIE_PATH_TYPE_CONCAVE);
}

/* @function minBeginSubEvenOdd:int
Begin new Even-Odd sub-path definition
@return Sub-path id
*/
sSI minBeginSubEvenOdd(void) {
   return minBeginSubEx(MINNIE_PATH_TYPE_EVENODD);
}

/* @function minEndSub,boolean bClosed
End sub-path definition
*/
void minEndSub(sBool _bClosed) {
   minEndPath(_bClosed);
}

/* @function minEndSubOpen
End sub-path definition and keep sub-path open
*/
void minEndSubOpen(void) {
   minEndSub(YAC_FALSE/*bClosed*/);
}

/* @function minEndSubClosed
End sub-path definition and close sub-path (connect last to first)
*/
void minEndSubClosed(void) {
   minEndSub(YAC_TRUE/*bClosed*/);
}

/* @function minStrokeWidth,float w
Set line stroke width.

The total line width is (2 * strokeW).
*/
void minStrokeWidth(sF32 _w) {
   // (note) <0.001: enable fill mode  (see minFill())
   minnie::setup::cur_stroke_w = _w * minnie::setup::stroke_scale + minnie::setup::stroke_offset;
}

/* @function minFill
Select fill mode for next path draw call
*/
void minFill(void) {
   minnie::setup::cur_stroke_w = 0.0f;
}

/* @function minMiterLimit,float l
Set miter limit.

When the miter line joint distance exceeds the miter limit, a bevel line joint will be rendered instead.
*/
void minMiterLimit(sF32 _l) {
   minnie::setup::cur_miter_limit = _l;
}

/* @function minColor
Set ARGB32 color for next path draw call
*/
void minColor(sUI _c32) {
   minnie::setup::cur_c32 = _c32;
}

/* @function minJoinCap,int joinCap
Set line joint and cap type (short form).
*/
void minJoinCap(sUI _joinCap) {
   // MINNIE_LINEJOIN_xxx  (NONE|MITER|ROUND|BEVEL)
   // MINNIE_LINECAP_xxx   (NONE|BUTT|ROUND|SQUARE)
   //  (note) def=0x23 (round/bevel)
   minnie::setup::cur_join_cap = sU8(_joinCap);
}

/* @function minJoin,int joinType
Select line joint type (short form)
*/
void minJoin(sUI _joinType) {
   // MINNIE_LINEJOIN_xxx  (NONE|MITER|ROUND|BEVEL)
   minnie::setup::cur_join_cap = (minnie::setup::cur_join_cap & ~15u) | sU8(_joinType & 15u);
}

/* @function minJoinNone
Disable line joints
*/
void minJoinNone(void) {
   minJoin(MINNIE_LINEJOIN_NONE);
}

/* @function minJoinMiter
Select Miter line joints
*/
void minJoinMiter(void) {
   minJoin(MINNIE_LINEJOIN_MITER);
}

/* @function minJoinRound
Select Round line joints
*/
void minJoinRound(void) {
   minJoin(MINNIE_LINEJOIN_ROUND);
}

/* @function minJoinBevel
Select Bevel line joints
*/
void minJoinBevel(void) {
   minJoin(MINNIE_LINEJOIN_BEVEL);
}

/* @function minCap,int capType
Select line cap type (short form)
*/
void minCap(sUI _capType) {
   // MINNIE_LINECAP_xxx   (NONE|BUTT|ROUND|SQUARE)
   minnie::setup::cur_join_cap = (minnie::setup::cur_join_cap & ~0xF0u) | (sU8(_capType & 15u)<<4);
}

/* @function minCapNone
Disable line caps
*/
void minCapNone(void) {
   minCap(MINNIE_LINECAP_NONE);
}

/* @function minCapRound
Select Round line caps
*/
void minCapRound(void) {
   minCap(MINNIE_LINECAP_ROUND);
}

/* @function minCapButt
Select Butt line caps
*/
void minCapButt(void) {
   minCap(MINNIE_LINECAP_BUTT);
}

/* @function minDrawPath,int pathId
Draw previously defined path
*/
void minDrawPath(sSI _pathId) {
   /* yac_host->printf("xxx apiDraw: pathId=%u #paths=%u\n", _pathId, paths.num_elements); */
   minnie::Path *oPath = minnie::setup::paths.get(sUI(_pathId - 1));
   if(NULL != oPath)
   {
      minnie::setup::drawMultiPath(oPath, 0/*mode=notransform*/);
   }
   else
   {
      Dapierror("[---] Minnie::apiDrawPath: invalid pathId=%d\n", _pathId);
   }
}

/* @function minBindTexture,int texId,boolean bRepeat,boolean bFilter
Bind 2D texture.

Applies to texture triangle and rectangle draw calls.

@arg texId OpenGL texture id
@arg bRepeat true=repeat texture false=clamp to edge
@arg bFilter true=enable bilinear filter false=use nearest neighbour sampling

@see minTriangleTexUVFlat
@see minTriangleTexUVGouraud
@see minRectTexUVFlat
@see minRectTexUVGouraud
@see minTriangleTexUVFlatDecal
@see minTriangleTexUVGouraudDecal
@see minRectTexUVFlatDecal
@see minRectTexUVGouraudDecal
@see minUnbindTexture
*/
void minBindTexture(sSI _texId, sBool _bRepeat, sBool _bFilter) {
   if(NULL != minnie::setup::loc_dl_export_ofs)
   {
      if(minnie::setup::dl_tex_id     != _texId    ||
         minnie::setup::dl_tex_repeat != _bRepeat  ||
         minnie::setup::dl_tex_filter != _bFilter
         )
      {
         minnie::setup::finishActiveDrawListOp();
         Dexport_dl_i16(MINNIE_DRAWOP_BIND_TEXTURE);
         Dexport_dl_i32(_texId);
         Dexport_dl_i8(_bRepeat);
         Dexport_dl_i8(_bFilter);
         minnie::setup::dl_tex_id     = _texId;
         minnie::setup::dl_tex_repeat = _bRepeat;
         minnie::setup::dl_tex_filter = _bFilter;
      }
   }
}

/* @function minUnbindTexture
Unbind texture

@see minBindTexture
*/
void minUnbindTexture(void) {
   if(NULL != minnie::setup::loc_dl_export_ofs)
   {
      if(0 != minnie::setup::dl_tex_id)
      {
         minnie::setup::finishActiveDrawListOp();
         Dexport_dl_i16(MINNIE_DRAWOP_UNBIND_TEXTURE);
         minnie::setup::dl_tex_id = 0;
      }
   }
}

/* @function minTextureDecalAlpha,float decalAlpha
Select alpha for 'decal' texture draw calls

@see minTriangleTexUVFlatDecal
@see minTriangleTexUVGouraudDecal
@see minRectTexUVFlatDecal
@see minRectTexUVGouraudDecal
@see minBindTexture
@see minUnbindTexture
*/
void minTextureDecalAlpha(sF32 _decalAlpha) {
   if(NULL != minnie::setup::loc_dl_export_ofs)
   {
      if(minnie::setup::dl_tex_decal_alpha != _decalAlpha)
      {
         minnie::setup::finishActiveDrawListOp();
         Dexport_dl_i16(MINNIE_DRAWOP_TEXTURE_DECAL_ALPHA);
         Dexport_dl_f32(_decalAlpha);
         minnie::setup::dl_tex_decal_alpha = _decalAlpha;
      }
   }
}

/* @function minTriangleTexUVFlat,float x1,float y1,float u1,float v1,float x2,float y2,float u2,float v2,float x3,float y3,float u3,float v3
Draw a texture mapped triangle.

Multiple consecutive draw calls will be merged automatically.

@arg x1 Vertex 1 x
@arg y1 Vertex 1 y
@arg u1 Vertex 1 u texture coordinate (0..1)
@arg v1 Vertex 1 v texture coordinate (0..1)
@arg x2 Vertex 2 x
@arg y2 Vertex 2 y
@arg u2 Vertex 2 u texture coordinate (0..1)
@arg v2 Vertex 2 v texture coordinate (0..1)
@arg x3 Vertex 3 x
@arg y3 Vertex 3 y
@arg u3 Vertex 3 u texture coordinate (0..1)
@arg v3 Vertex 3 v texture coordinate (0..1)

@see minTriangleTexUVGouraud
@see minRectTexUVFlat
@see minRectTexUVGouraud
@see minTriangleTexUVFlatDecal
@see minTriangleTexUVGouraudDecal
@see minRectTexUVFlatDecal
@see minRectTexUVGouraudDecal
@see minBindTexture
@see minUnbindTexture
*/
void minTriangleTexUVFlat(sF32 _x1, sF32 _y1, sF32 _u1, sF32 _v1,
                          sF32 _x2, sF32 _y2, sF32 _u2, sF32 _v2,
                          sF32 _x3, sF32 _y3, sF32 _u3, sF32 _v3
                          ) {
   if(minnie::setup::beginDrawListOp(MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_32))
   {
      Dexport_vb_add2f(_x1, _y1);
      Dexport_vb_add2f(_u1, _v1);

      Dexport_vb_add2f(_x2, _y2);
      Dexport_vb_add2f(_u2, _v2);

      Dexport_vb_add2f(_x3, _y3);
      Dexport_vb_add2f(_u3, _v3);

      minnie::setup::active_dl_num_tris++;
   }
}

/* @function minTriangleTexUVFlatDecal,float x1,float y1,float u1,float v1,float x2,float y2,float u2,float v2,float x3,float y3,float u3,float v3
Draw a texture mapped triangle. The texture is crossfaded with the current fill color according to §minTextureDecalAlpha.

Multiple consecutive draw calls will be merged automatically.

@arg x1 Vertex 1 x
@arg y1 Vertex 1 y
@arg u1 Vertex 1 u texture coordinate (0..1)
@arg v1 Vertex 1 v texture coordinate (0..1)
@arg x2 Vertex 2 x
@arg y2 Vertex 2 y
@arg u2 Vertex 2 u texture coordinate (0..1)
@arg v2 Vertex 2 v texture coordinate (0..1)
@arg x3 Vertex 3 x
@arg y3 Vertex 3 y
@arg u3 Vertex 3 u texture coordinate (0..1)
@arg v3 Vertex 3 v texture coordinate (0..1)

@see minTriangleTexUVFlat
@see minTriangleTexUVGouraud
@see minRectTexUVFlat
@see minRectTexUVGouraud
@see minTriangleTexUVGouraudDecal
@see minRectTexUVFlatDecal
@see minRectTexUVGouraudDecal
@see minBindTexture
@see minUnbindTexture
*/
void minTriangleTexUVFlatDecal(sF32 _x1, sF32 _y1, sF32 _u1, sF32 _v1,
                               sF32 _x2, sF32 _y2, sF32 _u2, sF32 _v2,
                               sF32 _x3, sF32 _y3, sF32 _u3, sF32 _v3
                               ) {
   if(minnie::setup::beginDrawListOp(MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_DECAL_32))
   {
      Dexport_vb_add2f(_x1, _y1);
      Dexport_vb_add2f(_u1, _v1);

      Dexport_vb_add2f(_x2, _y2);
      Dexport_vb_add2f(_u2, _v2);

      Dexport_vb_add2f(_x3, _y3);
      Dexport_vb_add2f(_u3, _v3);

      minnie::setup::active_dl_num_tris++;
   }
}

/* @function minTriangleTexUVGouraud,float x1,float y1,float u1,float v1,int c32v1,float x2,float y2,float u2,float v2,int c32v2,float x3,float y3,float u3,float v3,int c32v3
Draw a texture mapped, gouraud shaded triangle.

Multiple consecutive draw calls will be merged automatically.

@arg x1 Vertex 1 x
@arg y1 Vertex 1 y
@arg u1 Vertex 1 u texture coordinate (0..1)
@arg v1 Vertex 1 v texture coordinate (0..1)
@arg c32v1 Vertex 1 ARGB32 color
@arg x2 Vertex 2 x
@arg y2 Vertex 2 y
@arg u2 Vertex 2 u texture coordinate (0..1)
@arg v2 Vertex 2 v texture coordinate (0..1)
@arg c32v2 Vertex 2 ARGB32 color
@arg x3 Vertex 3 x
@arg y3 Vertex 3 y
@arg u3 Vertex 3 u texture coordinate (0..1)
@arg v3 Vertex 3 v texture coordinate (0..1)
@arg c32v3 Vertex 3 ARGB32 color

@see minTriangleTexUVFlat
@see minRectTexUVFlat
@see minRectTexUVGouraud
@see minTriangleTexUVFlatDecal
@see minTriangleTexUVGouraudDecal
@see minRectTexUVFlatDecal
@see minRectTexUVGouraudDecal
@see minBindTexture
@see minUnbindTexture
*/
void minTriangleTexUVGouraud(sF32 _x1, sF32 _y1, sF32 _u1, sF32 _v1, sU32 _c32v1,
                             sF32 _x2, sF32 _y2, sF32 _u2, sF32 _v2, sU32 _c32v2,
                             sF32 _x3, sF32 _y3, sF32 _u3, sF32 _v3, sU32 _c32v3
                             ) {
   if(minnie::setup::beginDrawListOp(MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_32))
   {
      Dexport_vb_add2f(_x1, _y1);
      Dexport_vb_add2f(_u1, _v1);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32v1));

      Dexport_vb_add2f(_x2, _y2);
      Dexport_vb_add2f(_u2, _v2);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32v2));

      Dexport_vb_add2f(_x3, _y3);
      Dexport_vb_add2f(_u3, _v3);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32v3));

      minnie::setup::active_dl_num_tris++;
   }
}

/* @function minTriangleTexUVGouraudDecal,float x1,float y1,float u1,float v1,int c32v1,float x2,float y2,float u2,float v2,int c32v2,float x3,float y3,float u3,float v3,int c32v3
Draw a texture mapped, gouraud shaded triangle. The texture is crossfaded with the current fill color according to §minTextureDecalAlpha.

Multiple consecutive draw calls will be merged automatically.

@arg x1 Vertex 1 x
@arg y1 Vertex 1 y
@arg u1 Vertex 1 u texture coordinate (0..1)
@arg v1 Vertex 1 v texture coordinate (0..1)
@arg c32v1 Vertex 1 ARGB32 color
@arg x2 Vertex 2 x
@arg y2 Vertex 2 y
@arg u2 Vertex 2 u texture coordinate (0..1)
@arg v2 Vertex 2 v texture coordinate (0..1)
@arg c32v2 Vertex 2 ARGB32 color
@arg x3 Vertex 3 x
@arg y3 Vertex 3 y
@arg u3 Vertex 3 u texture coordinate (0..1)
@arg v3 Vertex 3 v texture coordinate (0..1)
@arg c32v3 Vertex 3 ARGB32 color

@see minTriangleTexUVFlat
@see minTriangleTexUVGouraud
@see minRectTexUVFlat
@see minRectTexUVGouraud
@see minTriangleTexUVFlatDecal
@see minRectTexUVFlatDecal
@see minRectTexUVGouraudDecal
@see minBindTexture
@see minUnbindTexture
*/
void minTriangleTexUVGouraudDecal(sF32 _x1, sF32 _y1, sF32 _u1, sF32 _v1, sU32 _c32v1,
                                  sF32 _x2, sF32 _y2, sF32 _u2, sF32 _v2, sU32 _c32v2,
                                  sF32 _x3, sF32 _y3, sF32 _u3, sF32 _v3, sU32 _c32v3
                                  ) {
   if(minnie::setup::beginDrawListOp(MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_DECAL_32))
   {
      Dexport_vb_add2f(_x1, _y1);
      Dexport_vb_add2f(_u1, _v1);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32v1));

      Dexport_vb_add2f(_x2, _y2);
      Dexport_vb_add2f(_u2, _v2);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32v2));

      Dexport_vb_add2f(_x3, _y3);
      Dexport_vb_add2f(_u3, _v3);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32v3));

      minnie::setup::active_dl_num_tris++;
   }
}

/* @function minRectTexUVFlat,float x,float y,float w,float h,float ul,float vt,float ur,float vb
Draw a texture mapped rectangle

Multiple consecutive draw calls will be merged automatically.

@arg x X position
@arg y Y position
@arg w Width
@arg h Height
@arg ul Left u texture coordinate
@arg vt Top v texture coordinate
@arg ur Right u texture coordinate
@arg vb Bottom v texture coordinate

@see minTriangleTexUVFlat
@see minTriangleTexUVGouraud
@see minRectTexUVGouraud
@see minTriangleTexUVFlatDecal
@see minTriangleTexUVGouraudDecal
@see minRectTexUVFlatDecal
@see minRectTexUVGouraudDecal
@see minBindTexture
@see minUnbindTexture
*/
void minRectTexUVFlat(sF32 _x, sF32 _y, sF32 _w, sF32 _h,
                      sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb
                      ) {
   if(minnie::setup::beginDrawListOp(MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_32))
   {
      // tri 1
      Dexport_vb_add2f(_x,      _y );
      Dexport_vb_add2f(_ul,     _vt);

      Dexport_vb_add2f(_x + _w, _y );
      Dexport_vb_add2f(_ur,     _vt);

      Dexport_vb_add2f(_x + _w, _y + _h);
      Dexport_vb_add2f(_ur,     _vb    );

      // tri 2
      Dexport_vb_add2f(_x,      _y );
      Dexport_vb_add2f(_ul,     _vt);

      Dexport_vb_add2f(_x + _w, _y + _h);
      Dexport_vb_add2f(_ur,     _vb    );

      Dexport_vb_add2f(_x,      _y + _h);
      Dexport_vb_add2f(_ul,     _vb    );

      minnie::setup::active_dl_num_tris += 2u;
   }
}

/* @function minRectTexUVFlatDecal,float x,float y,float w,float h,float ul,float vt,float ur,float vb
Draw a texture mapped rectangle. The texture is crossfaded with the current fill color according to §minTextureDecalAlpha.

Multiple consecutive draw calls will be merged automatically.

@arg x X position
@arg y Y position
@arg w Width
@arg h Height
@arg ul Left u texture coordinate
@arg vt Top v texture coordinate
@arg ur Right u texture coordinate
@arg vb Bottom v texture coordinate

@see minTriangleTexUVFlat
@see minTriangleTexUVGouraud
@see minRectTexUVFlat
@see minRectTexUVGouraud
@see minTriangleTexUVFlatDecal
@see minTriangleTexUVGouraudDecal
@see minRectTexUVGouraudDecal
@see minBindTexture
@see minUnbindTexture
*/
void minRectTexUVFlatDecal(sF32 _x, sF32 _y, sF32 _w, sF32 _h,
                           sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb
                           ) {
   if(minnie::setup::beginDrawListOp(MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_DECAL_32))
   {
      // tri 1
      Dexport_vb_add2f(_x,      _y );
      Dexport_vb_add2f(_ul,     _vt);

      Dexport_vb_add2f(_x + _w, _y );
      Dexport_vb_add2f(_ur,     _vt);

      Dexport_vb_add2f(_x + _w, _y + _h);
      Dexport_vb_add2f(_ur,     _vb    );

      // tri 2
      Dexport_vb_add2f(_x,      _y );
      Dexport_vb_add2f(_ul,     _vt);

      Dexport_vb_add2f(_x + _w, _y + _h);
      Dexport_vb_add2f(_ur,     _vb    );

      Dexport_vb_add2f(_x,      _y + _h);
      Dexport_vb_add2f(_ul,     _vb    );

      minnie::setup::active_dl_num_tris += 2u;
   }
}

/* @function minRectTexUVGouraud,float x,float y,float w,float h,float ul,float vt,float ur,float vb,int c32lt,int c32rt,int c32rb,int c32lb
Draw a texture mapped, gouraud shaded rectangle.

Multiple consecutive draw calls will be merged automatically.

@arg x X position
@arg y Y position
@arg w Width
@arg h Height
@arg ul Left u texture coordinate
@arg vt Top v texture coordinate
@arg ur Right u texture coordinate
@arg vb Bottom v texture coordinate
@arg c32lt Left/Top ARGB32 color
@arg c32rt Right/Top ARGB32 color
@arg c32rb Right/Bottom ARGB32 color
@arg c32lb Left/Bottom ARGB32 color

@see minTriangleTexUVFlat
@see minTriangleTexUVGouraud
@see minRectTexUVFlat
@see minTriangleTexUVFlatDecal
@see minTriangleTexUVGouraudDecal
@see minRectTexUVFlatDecal
@see minRectTexUVGouraudDecal
@see minBindTexture
@see minUnbindTexture
*/
void minRectTexUVGouraud(sF32 _x, sF32 _y, sF32 _w, sF32 _h,
                         sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb,
                         sU32 _c32lt, sU32 _c32rt, sU32 _c32rb, sU32 _c32lb
                         ) {
   if(minnie::setup::beginDrawListOp(MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_32))
   {
      // tri 1
      Dexport_vb_add2f(_x,      _y );
      Dexport_vb_add2f(_ul,     _vt);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32lt));

      Dexport_vb_add2f(_x + _w, _y );
      Dexport_vb_add2f(_ur,     _vt);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32rt));

      Dexport_vb_add2f(_x + _w, _y + _h);
      Dexport_vb_add2f(_ur,     _vb    );
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32rb));

      // tri 2
      Dexport_vb_add2f(_x,      _y );
      Dexport_vb_add2f(_ul,     _vt);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32lt));

      Dexport_vb_add2f(_x + _w, _y + _h);
      Dexport_vb_add2f(_ur,     _vb    );
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32rb));

      Dexport_vb_add2f(_x,      _y + _h);
      Dexport_vb_add2f(_ul,     _vb    );
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32lb));

      minnie::setup::active_dl_num_tris += 2u;
   }
}

/* @function minRectTexUVGouraudDecal,float x,float y,float w,float h,float ul,float vt,float ur,float vb,int c32lt,int c32rt,int c32rb,int c32lb
Draw a texture mapped, gouraud shaded rectangle. The texture is crossfaded with the current fill color according to §minTextureDecalAlpha.

Multiple consecutive draw calls will be merged automatically.

@arg x X position
@arg y Y position
@arg w Width
@arg h Height
@arg ul Left u texture coordinate
@arg vt Top v texture coordinate
@arg ur Right u texture coordinate
@arg vb Bottom v texture coordinate
@arg c32lt Left/Top ARGB32 color
@arg c32rt Right/Top ARGB32 color
@arg c32rb Right/Bottom ARGB32 color
@arg c32lb Left/Bottom ARGB32 color

@see minTriangleTexUVFlat
@see minTriangleTexUVGouraud
@see minRectTexUVFlat
@see minRectTexUVGouraud
@see minTriangleTexUVFlatDecal
@see minTriangleTexUVGouraudDecal
@see minRectTexUVFlatDecal
@see minBindTexture
@see minUnbindTexture
*/
void minRectTexUVGouraudDecal(sF32 _x, sF32 _y, sF32 _w, sF32 _h,
                              sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb,
                              sU32 _c32lt, sU32 _c32rt, sU32 _c32rb, sU32 _c32lb
                              ) {
   if(minnie::setup::beginDrawListOp(MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_DECAL_32))
   {
      // tri 1
      Dexport_vb_add2f(_x,      _y );
      Dexport_vb_add2f(_ul,     _vt);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32lt));

      Dexport_vb_add2f(_x + _w, _y );
      Dexport_vb_add2f(_ur,     _vt);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32rt));

      Dexport_vb_add2f(_x + _w, _y + _h);
      Dexport_vb_add2f(_ur,     _vb    );
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32rb));

      // tri 2
      Dexport_vb_add2f(_x,      _y );
      Dexport_vb_add2f(_ul,     _vt);
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32lt));

      Dexport_vb_add2f(_x + _w, _y + _h);
      Dexport_vb_add2f(_ur,     _vb    );
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32rb));

      Dexport_vb_add2f(_x,      _y + _h);
      Dexport_vb_add2f(_ul,     _vb    );
      Dexport_vb_i32  (minnie::i32rgba8_from_argb32(_c32lb));

      minnie::setup::active_dl_num_tris += 2u;
   }
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
#endif // MINNIE_IMPLEMENTATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif // MINNIE_PROCEDURAL_API
