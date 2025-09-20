/// tkstexture.h
///
/// (c) 1999-2025 Bastian Spiegel <bs@tkscript.de>
///     - published under terms of the GNU general public license (GPL)
///
///
#ifndef TKSTEXTURE_H__
#define TKSTEXTURE_H__

YG("Texture");


/* @class Texture
Represents an OpenGL texture
*/
YC class _Texture : public YAC_Object {
public:


/* @constant TEX_16BIT,int
Hint OpenGL to store that texture in a 16bit per pixel format
*/
#define TEX_16BIT              YCI (1<<0)


/* @constant TEX_REPEAT_S,int
Repeat texels along the x axis
*/
#define TEX_REPEAT_S           YCI (1<<1)


/* @constant TEX_REPEAT_T,int
Repeat texels along the y axis
*/
#define TEX_REPEAT_T           YCI (1<<2)


/* @constant TEX_DELETE,int
Delete bitmap after texture has been uploaded to OpenGL
*/
#define TEX_DELETE             YCI (1<<3)


/* @constant TEX_AUTOMIPMAP,int
Automatically generate mip maps
*/
#define TEX_AUTOMIPMAP         YCI (1<<4)


/* @constant TEX_MODULATE,int
Allow the texels to be modulated by the current OpenGL color
*/
#define TEX_MODULATE           YCI (1<<5)


/* @constant TEX_BLEND,int
*/
#define TEX_BLEND              YCI (1<<6)


/* @constant TEX_DECAL,int
*/
#define TEX_DECAL              YCI (1<<7)


/* @constant TEX_REPLACE,int
*/
#define TEX_REPLACE            YCI (1<<8)


/* @constant TEX_MINFILTERLINEAR,int
*/
#define TEX_MINFILTERLINEAR    YCI (1<<9)


/* @constant TEX_MINFILTERTRILINEAR,int
*/
#define TEX_MINFILTERTRILINEAR YCI (1<<10)


/* @constant TEX_MAGFILTERLINEAR,int
*/
#define TEX_MAGFILTERLINEAR    YCI (1<<11)


/* @constant TEX_MAGFILTERTRILINEAR,int

@deprecated Same as TEX_MAGFILTERLINEAR
*/
#define TEX_MAGFILTERTRILINEAR YCI (1<<12)


/* @constant TEX_ALPHA,int
Use alpha channel
*/
#define TEX_ALPHA              YCI (1<<13)


/* @constant TEX_UPLOADED,int
This flag is set when the texture has been uploaded to OpenGL
*/
#define TEX_UPLOADED           YCI (1<<14)


/* @constant TEX_1D,int
Use GL_TEXTURE_1D texture target
*/
#define TEX_1D                 YCI (1<<15)


/* @constant TEX_FORCEHOSTBYTEORDER,int
Hint that the bitmap uses the host byte order (e.g. A,B,G,R on little endian)
*/
#define TEX_FORCEHOSTBYTEORDER YCI (1<<16)


/* @constant TEX_INTENSITY,int
Upload 8bit texture in INTENSITY format
*/
#define TEX_INTENSITY          YCI (1<<17)


/* @constant TEX_CLAMPTOEDGE_S,int
*/
#define TEX_CLAMPTOEDGE_S      YCI (1<<18)


/* @constant TEX_CLAMPTOEDGE_T,int
*/
#define TEX_CLAMPTOEDGE_T      YCI (1<<19)


/* @constant TEX_REPEATMASK,int
*/
#define TEX_REPEATMASK         YCI (TEX_REPEAT_S|TEX_REPEAT_T|TEX_CLAMPTOEDGE_S|TEX_CLAMPTOEDGE_T)


/* @constant TEX_ENVMASK,int
*/
#define TEX_ENVMASK            YCI (TEX_MODULATE|TEX_BLEND|TEX_DECAL|TEX_REPLACE)


/* @constant TEX_FILTERMASK,int
*/
#define TEX_FILTERMASK         YCI (TEX_MINFILTERLINEAR|TEX_MINFILTERTRILINEAR|TEX_MAGFILTERLINEAR|TEX_MAGFILTERTRILINEAR)


/* @constant TEX_POINTSPRITE,int
*/
#define TEX_POINTSPRITE        YCI (1<<20)


public:
	Image image;
	sUI   gl_nameid;
	sUI   flags;
	sF32  priority;
   sUI   target; // 0=use flags(legacy), !=0 = GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_2D_ARRAY, ...
   sUI   mipmap_level; // Default = 0
   sBool b_autogen_id; // Default = 1 (true)
   sUI   sz_hint;      // 3d/array texture number of slices (glTexImage3D depth)

public:
	 _Texture(void);
	 ~_Texture();
	 YAC(_Texture);

     static void Expand2n(sSI *_sx, sSI *_sy);

	 sBool YAC_VCALL yacArrayAlloc (sUI _num, sUI,sUI,sUI);
	 sUI   YAC_VCALL yacArrayGetWidth (void);
	 sUI   YAC_VCALL yacArrayGetHeight (void);
	 sUI   YAC_VCALL yacArrayGetElementType (void);
	 sUI   YAC_VCALL yacArrayGetElementByteSize (void);
	 sUI   YAC_VCALL yacArrayGetStride (void);
	 void *YAC_VCALL yacArrayGetPointer (void);
	 void  YAC_VCALL yacArraySet (void *_context, sUI _index, YAC_Value *_value);
    void  YAC_VCALL yacArrayGet (void *_context, sUI _index, YAC_Value *_ret);
    sUI   YAC_CALL  yacArrayGetNumElements (void);
    sUI   YAC_CALL  yacArrayGetMaxElements (void);

    void fixFlags(void);

	 void upload(sBool _newnameid, sUI _x, sUI _y, sUI _w, sUI _h);
	 void simpleBlur            (Image *_t, Image *_j);
	 void simpleBlurX           (Image *_t, Image *_j);
	 void simpleBlurY           (Image *_t, Image *_j);

    sUI getGlTextureTarget(void); // Use flags(TEX_1D, TEX_2D) or "target" (if it is != 0)
    sUI getGlTextureUploadTarget(void); // Use flags(TEX_1D, TEX_2D) or "target" (if it is != 0)


    // YAC interface


    /* @method setFlags,int flags
    Set texture flags.

    These flags should usually be set before uploading the texture to OpenGL.
    */
	 YM void _setFlags             (sUI _flags);


    /* @method getFlags:int
    */
	 YM sUI  _getFlags             (void);


    /* @method loadImage,String pakFileName,int newWidth,int newHeight,int newNumBytes:boolean
    Load pakfile image and scale to the given geometry and depth.

    @arg pakFileName Logic PAK file name
    @arg newWidth If 0, keep original image width. Otherwise, scale loaded image to this width
    @arg newHeight If 0, keep original image height. Otherwise, scale loaded image to this height
    @arg newNumBytes If 0, keep original image depth. Otherwise, scale loaded image to the given number of bytes per pixel
    */
	 YM sBool _loadImage           (YAC_String *, sSI _scx, sSI _scy, sSI _z);


    /* @method loadLocalImage,String localFileName,int newWidth,int newHeight,int newNumBytes:boolean
    Load local image and scale to the given geometry and depth.

    @arg localFileName Local file name
    @arg newWidth If 0, keep original image width. Otherwise, scale loaded image to this width
    @arg newHeight If 0, keep original image height. Otherwise, scale loaded image to this height
    @arg newNumBytes If 0, keep original image depth. Otherwise, scale loaded image to the given number of bytes per pixel
    */
	 YM sBool  _loadLocalImage     (YAC_String *_s, sSI _scx, sSI _scy, sSI _z);


    /* @method loadImageStream,Stream ifs,int newWidth,int newHeight,int newNumBytes:boolean
    Load image from §Stream and scale to the given geometry and depth.

    @arg ifs Input Stream
    @arg newWidth If 0, keep original image width. Otherwise, scale loaded image to this width
    @arg newHeight If 0, keep original image height. Otherwise, scale loaded image to this height
    @arg newNumBytes If 0, keep original image depth. Otherwise, scale loaded image to the given number of bytes per pixel
    */
	 YM sBool _loadImageStream     (YAC_Object *_ifs, sSI _scx, sSI _scy, sSI _z);


    /* @method saveImage,String localFileName:boolean
    */
	 YM sBool _saveImage           (YAC_String *);


    /* @method saveImageStream,Stream ofs:boolean
    */
	 YM sBool _saveImageStream     (YAC_Object *_ofs);


    /* @method upload
    */
	 YM void _upload               (void);


    /* @method setScreen,Texture srctex
    Share bitmap data with other Texture
    */
	 YM void _setScreen            (YAC_Object *_srctex);


    /* @method alloc,int width,int height,int numBytesPerPixel:boolean
    Allocate bitmap data
    */
	 YM sBool _alloc                (sSI _sx, sSI _sy, sSI _z);


    /* @method scaleFrom,Texture src
    */
	 YM void _scaleFrom            (YAC_Object *);


    /* @method bind
    Bind texture to associated OpenGL texture target
    */
	 YM void _bind                 (void);


    /* @method unbind
    Unbind texture target (bind name 0)
    */
	 YM void _unbind               (void);


    /* @method enable
    Enable texture target (glEnable())
    */
	 YM void _enable               (void);


    /* @method disable
    Disable texture target (glDisable())
    */
	 YM void _disable              (void);


    /* @method free
    Free bitmap data and OpenGL texture handle
    */
	 YM void _free                 (void);


    /* @method freeImage
    Free bitmap data
    */
	 YM void _freeImage            (void);


    /* @method setFlags
    */


	 ////YM void setScreenArea        (YAC_Object *_srctex, YAC_Object *_lt, YAC_Object *_sz);


    /* @method setScreenArea,Texture srctex,int ltx,int lty,int width,int height
    */
    YM void _setScreenArea        (YAC_Object *_srctex, sSI _ltx, sSI _lty, sSI _sx, sSI _sy);


    /* @method setPriority,float prio:float
    Set OpenGL texture priority
    */
	 YM sF32 _setPriority          (sF32);


    /* @method setEnvColor,int argb32
    */
	 YM void _setEnvColor          (sSI);


    /* @method setFlags
    */


    ////YM void _copyRegion           (YAC_Object *_texsource, YAC_Object *_ivecp, YAC_Object *_ivecs, YAC_Object *_ivecd);


    /* @method copyRegion,Texture srctex,int px,int py,int w,int h,int dx,int dy
    */
    YM void _copyRegion           (YAC_Object *_texsource, sSI _px, sSI _py, sSI _sx, sSI _sy, sSI _dx, sSI _dy);


    /* @method clear,int argb32
    Fill pixels with given clear color
    */
 	 YM void _clear                (sUI _c32);


    /* @method clearAlpha,int a
    Fill alpha channel (of ARGB32 texture) with constant value (0..255)
    */
 	 YM void _clearAlpha           (sUI _a8);


    /* @method unload
    Free OpenGL texture handle
    */
	 YM void _unload               (void);


    /* @method getSx:int
    Query texture width
    */
    YM sSI  _getSx                (void);


    /* @method getSy:int
    Query texture height
    */
    YM sSI  _getSy                (void);


    /* @method getZ:int
    Query number of bytes per pixel
    */
    YM sSI  _getZ                 (void);


    /* @method setSz,int sz
    Set OpenGL texture depth (3d textures, 2d texture arrays)
    Note: When texture target is GL_TEXTURE_3D, the texture height (sy) will be split into 'sz' depth slices during upload()
    @arg sz 1..256 (number of slices)
    */
    YM void _setSz                (sUI _sz);


    /* @method getSz:int
    Query OpenGL texture depth (3d texture, 2d texture arrays)
    */
    YM sUI  _getSz                (void);


    ////YM void _drawPixels           (YAC_Object *_rasterposvector);


    /* @method drawPixels2i,int x,int y
    Draw texture via glDrawPixels
    */
    YM void _drawPixels2i         (sSI _x, sSI _y);


    /* @method drawPixels2f,float x,float y
    Draw texture via glDrawPixels
    */
    YM void _drawPixels2f         (sF32 _x, sF32 _y);


    /* @method drawPixels3f,float x,float y,float z
    Draw texture via glDrawPixels
    */
    YM void _drawPixels3f         (sF32 _x, sF32 _y, sF32 _z);


    /* @method swapScreen,Texture tex
    Swap bitmap data
    */
	 YM void _swapScreen           (YAC_Object *_tex);


    /* @method setFgcolor,int argb32
    Set foreground color for image manipulation helpers
    */
	 YM void _setFgcolor           (sUI);


    /* @method getFgcolor:int
    */
	 YM sUI  _getFgcolor           (void);


    /* @method getId:int
    Query OpenGL texture name id
    */
	 YM sSI  _getId                (void);


    /* @method setId,int name
    Set OpenGL texture name id
    */
    YM void _setId                (sSI _glTextureNameId);


    /* @method lazyGenId
       Create OpenGL texture name id (if pending)
    */
    YM void _lazyGenId            (void);


    /* @method applyColorMap,Texture tex,Texture cmap
    Apply colormap to tex and store result in this texture
    */
	 YM void _applyColorMap        (YAC_Object *_t1, YAC_Object *_cm);


    /* @method colorKeyToAlpha,int argb32
    */
	 YM void _colorKeyToAlpha      (sSI _ckey);


    /* @method grayscale:boolean
    */
	 YM sSI  _grayscale            (void);


    /* @method interleaveAlpha,Texture alphaTex
    */
	 YM void _interleaveAlpha      (YAC_Object *_alphatex);


    /* @method adjustGamma,float gamma
    */
	 YM void _adjustGamma          (sF32);


    /* @method adjustBrightness,float brightness
    */
	 YM void _adjustBrightness     (sF32);


    /* @method blend,Texture srca,Texture srcb,byte ratio
    */
 	 YM void _blend                (YAC_Object *_srca, YAC_Object *_srcb, sSI _ratio255);


    /* @method countPixels,int argb32:float

    */
    YM sF32 _countPixels          (sSI _colorkey);


    /* @method multiply,Texture srca,Texture srcb
    */
    YM void _multiply             (YAC_Object *_stexa, YAC_Object *_stexb);


    /* @method getXYRaw,int x,int y:int
    Query pixel (8, 16, 24, or 32 bit)
    */
	 YM sUI  _getXYRaw             (sUI _x, sUI _y);


    /* @method setXY32,int x,int y,int argb32
    Put raw pixel (, 16, 24, or 32 bit)
    */
	 YM void  _setXYRaw             (sUI _x, sUI _y, sUI _v);


    /* @method setXY32,int x,int y,int argb32
    Put pixel
    */
	 YM void _setXY32              (sSI _x, sSI _y, sSI _c32);


    /* @method getXY32,int x,int y:int
    Query pixel
    */
	 YM sUI  _getXY32              (sSI _x, sSI _y);


    /* @method getUVVec4,float u,float v,Object retVec4
    Query pixel via normalized u/v coordinates and store (normalized) result in 'retVec4' array/vector. Clamp u/v to (0;0)..(1;1) range.
    Texel center is left/top and u/v (0;0) refers to top/left texel.
    @arg u X-Position (0..1)
    @arg v Y-Position (0..1)
    @arg retVec4 Array-like Object that receives the texel rgba vector.
    */
	 YM void _getUVVec4            (sF32 _u, sF32 _v, YAC_Object *_retVec4);


    /* @method getUVFilterVec4,float u,float v,Object retVec4
    Query pixel via normalized u/v coordinates and store (normalized) result in 'retVec4' array/vector. Clamp u/v to (0;0)..(1;1) range.
    Texel center is left/top and u/v (0;0) refers to top/left texel.
    @arg u X-Position (0..1)
    @arg v Y-Position (0..1)
    @arg retVec4 Array-like Object that receives the texel rgba vector.
    */
	 YM void _getUVFilterVec4      (sF32 _u, sF32 _v, YAC_Object *_retVec4);


    /* @method getUVRepeatVec4,float u,float v,Object retVec4
    Query pixel via normalized u/v coordinates and store (normalized) result in 'retVec4' array/vector. Repeat u/v outside of (0;0)..(1;1) range.
    Texel center is left/top and u/v (0;0) refers to top/left texel.
    @arg u X-Position (0..1)
    @arg v Y-Position (0..1)
    @arg retVec4 Array-like Object that receives the texel rgba vector.
    */
	 YM void _getUVRepeatVec4      (sF32 _u, sF32 _v, YAC_Object *_retVec4);

    /* @method getUVFilterRepeatVec4,float u,float v,Object retVec4
    Query pixel via normalized u/v coordinates and store (normalized) result in 'retVec4' array/vector. Repeat u/v outside of (0;0)..(1;1) range.
    Texel center is left/top and u/v (0;0) refers to top/left texel.
    @arg u X-Position (0..1)
    @arg v Y-Position (0..1)
    @arg retVec4 Array-like Object that receives the texel rgba vector.
    */
	 YM void _getUVFilterRepeatVec4 (sF32 _u, sF32 _v, YAC_Object *_retVec4);


    /* @method drawTriAA,Object v1,Object v2,Object v3,int c32,int numSamples,boolean bFixSharedEdge
        - renders a premultiplied image / triangle into an ARGB32 bitmap
        - supports clipping
        - supports sub-pixel precision
        - supports alpha blending
        - implements the top-left fill rule (input must be a clock-wise triangle)
        - uses barycentric coordinates
        - implements n-rook multisample-like anti-aliasing without the need for a resolve pass
           (0, 2, 4, 8, or 16 samples)
        - uses the framebuffer texture's alpha channel as a coverage buffer

        (note) v1..v3 should be Vector2f refs (or any other array-like object. [0]=x, [1]=y)
        (note) the fixedpoint version (_fx) uses 6 fractional coord bits
        (note) 'aaFixMode' should be set to 2 when numSamples > 0  (0 otherwise)  (1=inside+outside AA, 2=outside AA only(small tris))
        (note) 'aaBias' should be set to (-0.01f * (160.0f / _fb->s.x)) (float)  /  Dfixmul16(-655, ((160*65536) / _fb->s.x)) (fx)
    */
    YM void _drawTriAA            (YAC_Object *_v1, YAC_Object *_v2, YAC_Object *_v3, sUI _c32, sUI _numSamples, sUI _aaFixMode, sF32 _aaBias);
    YM void _drawTriAA_fx         (YAC_Object *_v1, YAC_Object *_v2, YAC_Object *_v3, sUI _c32, sUI _numSamples, sUI _aaFixMode, sS32 _aaBias);


    /* @method applyClearColorAfterDrawTriAA,int rgb24
    Apply clear color after drawTriAA*() calls.

    Alpha channel will be set to 255.
    */
    YM void applyClearColorAfterDrawTriAA (sUI _c24);


    /* @method applyClearColorAfterDrawTriAA4444,int rgb24
    Apply clear color after drawTriAA*() calls.

    To be used when triangle rasterizer is built in AAFX_4444 mode.

    Expands #0A0R0G0B to #AARRGGBB. Alpha channel will be set to 255.
    */
    YM void applyClearColorAfterDrawTriAA4444 (sUI _c24);


    /* @method drawTri_fx,Object v1,Object v2,Object v3,int c32
        Renders a non-antialiased triangle into an ARGB32 bitmap
        (note) originally written for debugging the Amiga 68k version of this
        (note) the rasterizer uses fixed point math
                (v1..v3 in this script wrapper are Vector2f or FloatArray objects, though)
        (note) unlike drawTriAA*(), triangles need to be rendered back-to-front
        (note) no masking when c32Mask==0
    */
    YM void _drawTri_fx (YAC_Object *_v1, YAC_Object *_v2, YAC_Object *_v3, sUI _c32, sUI _c32Mask);


    /* @method drawTriTex_fx,Object v1,Object v2,Object v3,Objecvt uv1,Object uv2,Object uv3
       Renders a texture-mapped (ARGB32) triangle into an ARGB32 bitmap
        (note) the rasterizer loops use fixed point math
                (v1..v3 / uv1..uv3 are Vector2f or FloatArray objects, though)
        (note) implements bilinear filtering
        (note) the texels are rendered via source-over blending
        (note) 'v' vertices are screen / framebuffer coordinates (scaled to 16 subpixel bits)
        (note) 'uv' coords are in the range 0..1 (scaled to 16 subpixel bits)
        (note) 'a' alpha is in the range 0..1
    */
    YM void _drawTriTex_fx (YAC_Object *_tex, YAC_Object *_v1, YAC_Object *_v2, YAC_Object *_v3, YAC_Object *_uv1, YAC_Object *_uv2, YAC_Object *_uv3, sF32 _a);


    /* @method drawPolygon,Object array,int argb32
    Simple (convex) polygon renderer

    Was once used as a collision detection helper
    */
    YM void _drawPolygon          (YAC_Object *_intarray, sSI _color);


    /* @method drawLine,int qx,int qy,int px,int py,int argb32
    Simple bresenham line renderer

    Was once used as a collision detection helper
    */
    YM void _drawLine             (sSI _qx, sSI _qy, sSI _px, sSI _py, sUI _color);


    /* @method drawFilledBox,int px,int py,int sx,int sy,int argb32
    */
    YM void _drawFilledBox        (sSI _px, sSI _py, sSI _sx, sSI _sy, sUI _color);


    /* @method simpleBlur,Texture src
    */
	 YM void _simpleBlur           (YAC_Object *_j);


    /* @method simpleBlurX,Texture src
    */
	 YM void _simpleBlurX          (YAC_Object *_j);


    /* @method simpleBlurY,Texture src
    */
	 YM void _simpleBlurY          (YAC_Object *_j);


    /* @method scroll,int dx,int dy
    */
    YM void _scroll               (sSI _dx, sSI _dy);


    /* @method expand2n
    Automatically resize image and add black borders if width or height is not a power of two.
    */
    YM void _expand2n             (void);


    /* @method alloc2n,int width,int height,int bytesPerPixel:boolean
    Allocate bitmap
    */
	 YM sSI  _alloc2n              (sSI _sx, sSI _sy, sSI _z);


    /* @method createMask,int colorKey
    */
	 YM void _createMask           (sSI _ckey);


    /* @method getStride:int
    Query number of bytes per scanline
    */
	 YM sUI  _getStride            (void);


    /* @method update
    (Re-)Upload to OpenGL
    */
	 YM void _update               (void);


    /* @method updateRegion,int x,int y,int w,int h
    (Re-) upload region to OpenGL
    */
    YM void _updateRegion         (sSI _x, sSI _y, sSI _w, sSI _h);


    /* @method renderFixedText,Texture fontTex,int charWidth,String text,int x,int y,IntArray colors,IntArray attributes
    Simple fixed font text renderer
    */
    YM void _renderFixedText      (YAC_Object *_fontTex, sSI _charWidth, YAC_Object *_text, sSI _x, sSI _y, YAC_Object *_colorIntArray, YAC_Object *_attrIntArray);


    /* @method setMipMapLevel,int level
    Set OpenGL mip map level
    */
    YM void _setMipMapLevel       (sSI _level);


    /* @method getMipMapLevel:int
    Query OpenGL mip map level
    */
    YM sSI  _getMipMapLevel       (void);


    /* @method setTarget,int target
    Set OpenGL texture target
    */
    YM void _setTarget            (sSI _glTextureTarget);


    /* @method getTarget:int
    Query OpenGL texture target
    */
    YM sSI  _getTarget            (void);


    /* @method flipY
    Rotate bitmap 180 degrees about the x axis

    Remember that (0; 0) is the left/bottom corner in OpenGL..
    */
    YM void _flipY                (void);


    /* @method invert
       Invert r,g,b,a values (linear)
    */
    YM void _invert               (void);


    /* @method visitBuffer,Buffer buffer,int w,int h,int bpp
    Share (non-deletable) buffer view.

    @arg buffer Buffer object that holds the pixel data. Please notice that the buffer "offset" is taken into account when determing the effective buffer pointer and available size.
    @arg w Width
    @arg h Height
    @arg bpp Bytes per pixel
    */
    YM void _visitBuffer(YAC_Object *_buffer, sUI _w, sUI _h, sUI _bpp);
};


#endif // TKSTEXTURE_H__
