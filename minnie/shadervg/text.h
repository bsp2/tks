// ----
// ---- file   : text.h
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
// ---- info   : ShaderVG text utilities
// ----
// ----
// ----

#ifndef SHADERVG_TEXT_H__
#define SHADERVG_TEXT_H__

YG("shadervg");

#ifdef SHADERVG_TEXT
#pragma pack(push, 2)

typedef struct sdvg_font_info_s {  // 24 bytes (ROM)
   sS16 height;
   sS16 ascender;
   sS16 descender;
   sS16 underline_position;   // *64
   sS16 underline_thickness;  // *64
   sU16 first_glyph;
   sU16 num_glyphs;
   sU16 sdf_radius;
   sF32 us;  // 1/orig_tex_w
   sF32 vs;  // 1/orig_tex_h
} sdvg_font_info_t;

typedef struct sdvg_glyph_s {  // 14 bytes (ROM)
   sS16 x;
   sS16 y;
   sS16 w;
   sS16 h;
   sS16 offset_x;
   sS16 offset_y;
   sS16 advance_x;
} sdvg_glyph_t;

#pragma pack(pop)

typedef struct sdvg_font_s {  // RAM
   const sdvg_font_info_t *info;
   const sdvg_glyph_t     *glyphs;
   /* YAC_Buffer data;  // dynamically allocated when loaded from stream / file, static ref when initialized from ROM address */
   sUI         tex_w;
   sUI         tex_h;
   sF32        ob_tex_w;
   sF32        ob_tex_h;
   const void *tex_data;  // ref. 1 byte per pixel (alpha)
   sUI         tex_id;    // GL texture id
} sdvg_font_t;

#ifdef SHADERVG_SCRIPT_API
/* @class ShaderVG_Font
*/
YC class _ShaderVG_Font : public YAC_Object {
  public:
   sdvg_font_t font;

  public:
   _ShaderVG_Font(void);
   ~_ShaderVG_Font(void);

   YAC(_ShaderVG_Font);
};
#endif // SHADERVG_SCRIPT_API

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern void sdvg_int_reset_font (void); // internal

/* @function sdvg_InitFont,ShaderVG_Font,Object data,Object texData,int texW,int texH:boolean
*/
sBool YAC_CALL sdvg_InitFont (sdvg_font_t *_font, const void *_data, sUI _dataSz, const void *_texData, sUI _texW, sUI _texH);

/* @function sdvg_FontOnOpen,ShaderVG_Font:boolean
*/
sBool YAC_CALL sdvg_FontOnOpen (sdvg_font_t *_font);  // create + upload texture. must be called in onOpen()

/* @function sdvg_TextExtents,ShaderVG_Font,String text,Integer retW,Integer retH
*/
void YAC_CALL sdvg_TextExtents (const sdvg_font_t *_font, const char *_text, sSI *_retW, sSI *_retH);

/* @function sdvg_TextWidth,ShaderVG_Font,String text:int
*/
sSI YAC_CALL sdvg_TextWidth (const sdvg_font_t *_font, const char *_text);

/* @function sdvg_TextHeight,ShaderVG_Font,String text:int
*/
sSI YAC_CALL sdvg_TextHeight (const sdvg_font_t *_font, const char *_text);

/* @function sdvg_TextBBox,ShaderVG_Font,String text,Integer retMinX,Integer retMaxX,Integer retMinY,Integer retMaxY
*/
void YAC_CALL sdvg_TextBBox (const sdvg_font_t *_font, const char *_text, sSI *_retMinX, sSI *_retMaxX, sSI *_retMinY, sSI *_retMaxY);

/* @function sdvg_TextIndexAtX,ShaderVG_Font,String text,int x:int
*/
sSI YAC_CALL sdvg_TextIndexAtX (const sdvg_font_t *_font, const char *_text, sSI _x);

/* @function sdvg_BindFont,ShaderVG_Font
*/
void YAC_CALL sdvg_BindFont (const sdvg_font_t *_font, sBool _bFilter);

/* @function sdvg_DrawText,String text,float x,float y
*/
void YAC_CALL sdvg_DrawText (const char *_text, sF32 _x, sF32 _y);

/* @function sdvg_DrawTextClipped,String text,float x,float y,float clipLeft,float clipTop,float clipRight,float clipBottom
*/
void YAC_CALL sdvg_DrawTextClipped (const char *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom);

/* @function sdvg_DrawTextUnderline,String text,float x,float y
*/
void YAC_CALL sdvg_DrawTextUnderline (const char *_text, sF32 _x, sF32 _y);

/* @function sdvg_DrawTextUnderlineClipped,String text,float x,float y,float clipLeft,float clipTop,float clipRight,float clipBottom
*/
void YAC_CALL sdvg_DrawTextUnderlineClipped (const char *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom);

/* @function sdvg_DrawTextAccel,String text,float x,float y
*/
void YAC_CALL sdvg_DrawTextAccel (const char *_text, sF32 _x, sF32 _y);

/* @function sdvg_DrawTextAccelClipped,String text,float x,float y,float clipLeft,float clipTop,float clipRight,float clipBottom
*/
void YAC_CALL sdvg_DrawTextAccelClipped (const char *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom);

/* @function sdvg_UnbindFont
*/
YF void YAC_CALL sdvg_UnbindFont (void);
#ifdef SHADERVG_SCRIPT_API
YF sBool YAC_CALL _sdvg_InitFont (YAC_Object *_font, YAC_Object *_data, YAC_Object *_texData, sUI _texW, sUI _texH);
YF sBool YAC_CALL _sdvg_FontOnOpen (YAC_Object *_font);
YF void YAC_CALL _sdvg_TextExtents (YAC_Object *_font, YAC_String *_text, YAC_Object *_retW, YAC_Object *_retH);
YF sSI YAC_CALL _sdvg_TextWidth (YAC_Object *_font, YAC_String *_text);
YF sSI YAC_CALL _sdvg_TextHeight (YAC_Object *_font, YAC_String *_text);
YF void YAC_CALL _sdvg_TextBBox (YAC_Object *_font, YAC_String *_text, YAC_Object *_retMinX, YAC_Object *_retMaxX, YAC_Object *_retMinY, YAC_Object *_retMaxY);
YF sSI YAC_CALL _sdvg_TextIndexAtX (YAC_Object *_font, YAC_String *_text, sSI _x);
YF void YAC_CALL _sdvg_BindFont (YAC_Object *_font, sBool _bFilter);
YF void YAC_CALL _sdvg_DrawText (YAC_String *_text, sF32 _x, sF32 _y);
YF void YAC_CALL _sdvg_DrawTextClipped (YAC_String *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom);
YF void YAC_CALL _sdvg_DrawTextUnderline (YAC_String *_text, sF32 _x, sF32 _y);
YF void YAC_CALL _sdvg_DrawTextUnderlineClipped (YAC_String *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom);
YF void YAC_CALL _sdvg_DrawTextAccel (YAC_String *_text, sF32 _x, sF32 _y);
YF void YAC_CALL _sdvg_DrawTextAccelClipped (YAC_String *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom);
#endif // SHADERVG_SCRIPT_API

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // SHADERVG_TEXT


#endif // SHADERVG_TEXT_H__
