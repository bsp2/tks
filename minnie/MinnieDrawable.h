// ----
// ---- file   : MinnieDrawable.h
// ---- author : bsp
// ---- legal  : Distributed under terms of the MIT LICENSE.
// ----
// ---- info   : "minnie" API
// ---- note   :
// ----
// ----
// ----

#ifndef MINNIE_DRAWABLE_H__
#define MINNIE_DRAWABLE_H__

YG("minnie")


YC class _MinnieDrawable : public YAC_Object {

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

  public:
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


  public:
	_MinnieDrawable(void);
	~_MinnieDrawable();

	YAC(_MinnieDrawable);

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
   YM sBool isComplete(void);
   YM void draw (void);

};


#endif // MINNIE_DRAWABLE_H__
