// ----
// ---- file   : MinnieDrawable.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2025-2026 by bsp
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
// ---- info   : "minnie" VBO/DisplayList drawable utility class
// ----
// ----
// ----

#ifndef MINNIE_DRAWABLE_H__
#define MINNIE_DRAWABLE_H__

YG("minnie")


#if defined(__cplusplus)
#define MinnieDrawable _MinnieDrawable
YC class _MinnieDrawable
#ifdef SHADERVG_SCRIPT_API
: public YAC_Object
#endif // SHADERVG_SCRIPT_API
{

  public:
//#define MINNIE_PATH_TYPE_DRAW     YCI 0
//#define MINNIE_PATH_TYPE_CONVEX   YCI 1
//#define MINNIE_PATH_TYPE_CONCAVE  YCI 2
//#define MINNIE_PATH_TYPE_EVENODD  YCI 3

//#define MINNIE_LINEJOIN_NONE   YCI 0
//#define MINNIE_LINEJOIN_MITER  YCI 1
//#define MINNIE_LINEJOIN_ROUND  YCI 2
//#define MINNIE_LINEJOIN_BEVEL  YCI 3

//#define MINNIE_LINECAP_NONE    YCI 0
//#define MINNIE_LINECAP_BUTT    YCI 1
//#define MINNIE_LINECAP_ROUND   YCI 2
//#define MINNIE_LINECAP_SQUARE  YCI 3


//#define MINNIE_DRAWOP_END                                  YCI 0x00
//#define MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_32               YCI 0x01
//#define MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_14_2             YCI 0x02
//#define MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_32            YCI 0x03
//#define MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_14_2          YCI 0x04
//#define MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_EDGEAA_32        YCI 0x05
//#define MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_EDGEAA_14_2      YCI 0x06
//#define MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_32     YCI 0x07
//#define MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_14_2   YCI 0x08
//#define MINNIE_DRAWOP_POLYGON_FILL_FLAT_32                 YCI 0x09
//#define MINNIE_DRAWOP_POLYGON_FILL_FLAT_14_2               YCI 0x0A
//#define MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_32              YCI 0x0B
//#define MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_14_2            YCI 0x0C
//#define MINNIE_DRAWOP_RECT_FILL                            YCI 0x0D
//#define MINNIE_DRAWOP_RECT_STROKE                          YCI 0x0E
//#define MINNIE_DRAWOP_RECT_FILL_STROKE                     YCI 0x0F
//#define MINNIE_DRAWOP_ELLIPSE_FILL                         YCI 0x10
//#define MINNIE_DRAWOP_ELLIPSE_STROKE                       YCI 0x11
//#define MINNIE_DRAWOP_ELLIPSE_FILL_STROKE                  YCI 0x12
//#define MINNIE_DRAWOP_ROUNDRECT_FILL                       YCI 0x13
//#define MINNIE_DRAWOP_ROUNDRECT_STROKE                     YCI 0x14
//#define MINNIE_DRAWOP_ROUNDRECT_FILL_STROKE                YCI 0x15
//#define MINNIE_DRAWOP_BIND_TEXTURE                         YCI 0x16
//#define MINNIE_DRAWOP_UNBIND_TEXTURE                       YCI 0x17
//#define MINNIE_DRAWOP_TEXTURE_DECAL_ALPHA                  YCI 0x18
//#define MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_32             YCI 0x19
//#define MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_DECAL_32       YCI 0x1A
//#define MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_32          YCI 0x1B
//#define MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_DECAL_32    YCI 0x1C
//#define MINNIE_DRAWOP_LINE_STRIP_FLAT_14_2                 YCI 0x1D
//#define MINNIE_DRAWOP_LINE_STRIP_FLAT_BEVEL_14_2           YCI 0x1E

#else
   // MINNIE_LIB
struct MinnieDrawable_s;
typedef struct MinnieDrawable_s MinnieDrawable;
struct MinnieDrawable_s {
#endif // defined(__cplusplus)

#define MINNIE_DEFAULT_GL_BUF_SIZE    (128 * 1024)
#define MINNIE_DEFAULT_DRAW_BUF_SIZE  (  2 * 1024)

   // static boolean b_debug_draw_list = 0;

   sBool b_debug;

   YAC_Buffer *buf_gl;
   sUI gl_buf_id;
   sBool b_gl_buf_update_pending;

   // (note)              peak: test069_bluejay  32526 bytes (mixed)
   // (note)              peak: test069_bluejay  12570 bytes (triangles only)
   // (note) usually <<2k, e.g. test040_tiger     6500 bytes (mixed)
   // (note)                    test040_tiger     2480 bytes (triangles only)
   // (note)                    test039_wildboar   450 bytes
   // (note)                    test012_stars       12 bytes
   YAC_Buffer *buf_draw;

   sUI c32_bg_color;   // suggested background color (first palette entry)

   sF32 size_x;  // suggested projR/projB for initTransform()  (object bounding box)
   sF32 size_y;

   sF32 scale_x;
   sF32 scale_y;

   sF32 rot_ang;

   sF32 trans_x;
   sF32 trans_y;


#if defined(__cplusplus)
  public:
	_MinnieDrawable(void);
	~_MinnieDrawable();

#if defined(SHADERVG_SCRIPT_API) && !defined(YAC_NO_HOST)
	YAC(_MinnieDrawable);
#endif

   YM void init (void);

  public:
   YM sBool alloc (sUI _maxGLBufSize, sUI _maxDrawBufSize);
   YM void reset (void);
   YM void setEnableDebug (sBool _bEnable);
   YM sBool getEnableDebug (void);
   YM void freeGL (void);
   YM void free (void);
   YM void lazyAllocGL (void);
   YM YAC_Buffer *getGLBuffer (void);
   YM sUI getGLOffset (void);
   YM YAC_Buffer *getDrawBuffer (void);
   YM sUI getDrawOffset (void);
   YM void onOpen (void);
   YM void setSize2f (sF32 _w, sF32 _h);
   YM void setSizeX (sF32 _w);
   YM void setSizeY (sF32 _h);
   YM sF32 getSizeX (void);
   YM sF32 getSizeY (void);
   YM void setBackgroundColor (sUI _c32);
   YM sUI getBackgroundColor (void);
   YM void setScale2f (sF32 _sx, sF32 _sy);
   YM void setScaleX (sF32 _s);
   YM void setScaleY (sF32 _s);
   YM sF32 getScaleX (void);
   YM sF32 getScaleY (void);
   YM void setRotation (sF32 _a);
   YM sF32 getRotation (void);
   YM void setTranslate2f (sF32 _tx, sF32 _ty);
   YM void setTranslateX (sF32 _tx);
   YM void setTranslateY (sF32 _ty);
   YM sF32 getTranslateX (void);
   YM sF32 getTranslateY (void);
   YM void queueGLBufUpdate (void);
   YM sBool isComplete (void);
   YM void begin (void);
   YM void end (void);
   YM void draw (void);
#endif // __cplusplus

};

#ifndef MINNIE_SKIP_DRAWABLE_C_API
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
MinnieDrawable *minDrawableNew (void);
void minDrawableInit (MinnieDrawable *_drawable);
void minDrawableDelete (MinnieDrawable *_drawable);
sBool minDrawableAlloc (MinnieDrawable *_drawable, sUI _maxGLBufSize, sUI _maxDrawBufSize);
void minDrawableReset (MinnieDrawable *_drawable);
void minDrawableSetEnableDebug (MinnieDrawable *_drawable, sBool _bEnable);
sBool minDrawableGetEnableDebug (MinnieDrawable *_drawable);
void minDrawableFreeGL (MinnieDrawable *_drawable);
void minDrawableFree (MinnieDrawable *_drawable);
void minDrawableLazyAllocGL (MinnieDrawable *_drawable);
YAC_Buffer *minDrawableGetGLBuffer (MinnieDrawable *_drawable);
sUI minDrawableGetGLOffset (MinnieDrawable *_drawable);
YAC_Buffer *minDrawableGetDrawBuffer (MinnieDrawable *_drawable);
sUI minDrawableGetDrawOffset (MinnieDrawable *_drawable);
void minDrawableOnOpen (MinnieDrawable *_drawable);
void minDrawableSetSize2f (MinnieDrawable *_drawable, sF32 _w, sF32 _h);
void minDrawableSetSizeX (MinnieDrawable *_drawable, sF32 _w);
void minDrawableSetSizeY (MinnieDrawable *_drawable, sF32 _h);
sF32 minDrawableGetSizeX (MinnieDrawable *_drawable);
sF32 minDrawableGetSizeY (MinnieDrawable *_drawable);
void minDrawableSetBackgroundColor (MinnieDrawable *_drawable, sUI _c32);
sUI minDrawableGetBackgroundColor (MinnieDrawable *_drawable);
void minDrawableSetScale2f (MinnieDrawable *_drawable, sF32 _sx, sF32 _sy);
void minDrawableSetScaleX (MinnieDrawable *_drawable, sF32 _s);
void minDrawableSetScaleY (MinnieDrawable *_drawable, sF32 _s);
sF32 minDrawableGetScaleX (MinnieDrawable *_drawable);
sF32 minDrawableGetScaleY (MinnieDrawable *_drawable);
void minDrawableSetRotation (MinnieDrawable *_drawable, sF32 _a);
sF32 minDrawableGetRotation (MinnieDrawable *_drawable);
void minDrawableSetTranslate2f (MinnieDrawable *_drawable, sF32 _tx, sF32 _ty);
void minDrawableSetTranslateX (MinnieDrawable *_drawable, sF32 _tx);
void minDrawableSetTranslateY (MinnieDrawable *_drawable, sF32 _ty);
sF32 minDrawableGetTranslateX (MinnieDrawable *_drawable);
sF32 minDrawableGetTranslateY (MinnieDrawable *_drawable);
void minDrawableQueueGLBufUpdate (MinnieDrawable *_drawable);
sBool minDrawableIsComplete (MinnieDrawable *_drawable);
void minDrawableBegin (MinnieDrawable *_drawable);
void minDrawableEnd (MinnieDrawable *_drawable);
void minDrawableDraw (MinnieDrawable *_drawable);
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
#endif // MINNIE_SKIP_DRAWABLE_C_API

#endif // MINNIE_DRAWABLE_H__
