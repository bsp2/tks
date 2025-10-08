// ----
// ---- file   : text.cpp
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
// ---- note   :
// ----
// ----
// ----

#ifdef SHADERVG_TEXT

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "../inc_yac.h"

#define MINNIE_SKIP_TYPEDEFS  defined
#include "../minnie.h"

#include "../inc_opengl.h"

#include "shadervg.h"
#include "shadervg_internal.h"
#include "Shader.h"
#include "Shape.h"
#include "text.h"

#ifdef SHADERVG_SCRIPT_API
#include "../tkminnie/ying_shadervg.h"
#endif // SHADERVG_SCRIPT_API

static const sdvg_font_t *shadervg_cur_font;

void sdvg_int_reset_font(void) {
   shadervg_cur_font = NULL;
}


#ifndef SHADERVG_TEXT_BATCH_SIZE
// (note) num char quads
#define SHADERVG_TEXT_BATCH_SIZE 128
#endif // SHADERVG_TEXT_BATCH_SIZE

#ifndef SHADERVG_TEXT_UNDERLINE_BATCH_SIZE
// (note) num lines
#define SHADERVG_TEXT_UNDERLINE_BATCH_SIZE 16
#endif // SHADERVG_TEXT_UNDERLINE_BATCH_SIZE


#ifdef SHADERVG_SCRIPT_API
_ShaderVG_Font::_ShaderVG_Font(void) {
   ::memset(&font, 0, sizeof(sdvg_font_t));
}

_ShaderVG_Font::~_ShaderVG_Font() {
}
#endif // SHADERVG_SCRIPT_API

sBool YAC_CALL sdvg_InitFont(sdvg_font_t *_font,
                             const void *_binData, sUI _binDataSz,
                             const void *_texData, sUI _texW, sUI _texH
                             ) {
   sBool r = YAC_FALSE;

   if(NULL != _font &&
      (_binDataSz >= (sizeof(sdvg_font_info_t) + (64u * sizeof(sdvg_glyph_t)))) &&
      (_texW > 0u) &&
      (_texH > 0u)
      )
   {
      ::memset((void*)_font, 0, sizeof(sdvg_font_t));
      _font->info = (sdvg_font_info_t*)_binData;
      _font->glyphs = (sdvg_glyph_t*)(_font->info + 1);
      // _font->data.buffer   = (sU8*)_binData;
      // _font->data.size     = _binDataSz;
      // _font->data.deleteme = YAC_FALSE;
      _font->tex_w    = _texW;
      _font->tex_h    = _texH;
      _font->tex_data = _texData;

      // Succeeded
      r = YAC_TRUE;
   }

   return r;
}

sBool YAC_CALL sdvg_FontOnOpen(sdvg_font_t *_font) {
   sBool r = YAC_FALSE;
   if(NULL != _font && NULL != _font->info)
   {
      _font->tex_id = sdvg_CreateTexture2D(SDVG_TEXFMT_ALPHA8,
                                           _font->tex_w, _font->tex_h,
                                           _font->tex_data,
                                           _font->tex_w * _font->tex_h
                                           );
      r = (0u != _font->tex_id);
   }
   return r;
}

void YAC_CALL sdvg_TextExtents(const sdvg_font_t *_font, const char *_text, sSI *_retW, sSI *_retH) {
   sSI retW = 0;
   sSI retH = 0;
   if(NULL != _font && NULL != _font->info)
   {
      if(NULL != _text)
      {
         sUI cx = 0u;
         sSI curW = 0;
         sUI charIdx = 0u;
         for(;;)
         {
            sUI c = sUI(_text[charIdx++]);
            if(c > 0u)
            {
               if(sUI('\n') == c)
               {
                  if(0u == cx)
                  {
                     retH += _font->info->height;
                  }
                  else
                  {
                     if(curW > retW)
                        retW = curW;
                     cx = 0u;
                     curW = 0;
                  }
               }
               else if(c >= _font->info->first_glyph)
               {
                  c -= _font->info->first_glyph;
                  if(c < _font->info->num_glyphs)
                  {
                     const sdvg_glyph_t *g = &_font->glyphs[c];
                     curW += g->advance_x;
                     if(0u == cx++)
                        retH += _font->info->height;
                  }
                  // else: invalid glyph (skip)
               }
               // else: before first glyph (skip)
            }
            else
            {
               // ASCIIZ / end of string
               if(curW > retW)
                  retW = curW;
               break;
            }
         } // iterate chars
      }
      // else: NULL text (skip)
   }
   else
   {
      Dsdvg_errorprintf("[---] shadervg:sdvg_TextExtents: font is NULL\n");
   }
   *_retW = retW;
   *_retH = retH;
}

sSI YAC_CALL sdvg_TextWidth(const sdvg_font_t *_font, const char *_text) {
   sSI retW;
   sSI retH;
   sdvg_TextExtents(_font, _text, &retW, &retH);
   return retW;
}

sSI YAC_CALL sdvg_TextHeight(const sdvg_font_t *_font, const char *_text) {
   sSI retW;
   sSI retH;
   sdvg_TextExtents(_font, _text, &retW, &retH);
   return retH;
}

void YAC_CALL sdvg_TextBBox(const sdvg_font_t *_font, const char *_text, sSI *_retMinX, sSI *_retMaxX, sSI *_retMinY, sSI *_retMaxY) {
   sSI minX = 999999;
   sSI maxX = -1;
   sSI minY = 999999;
   sSI maxY = -1;
   if(NULL != _font && NULL != _font->info)
   {
      if(NULL != _text)
      {
         sSI cx = 0;
         sSI cy = _font->info->ascender - _font->info->height;
         sUI charIdx = 0u;
         for(;;)
         {
            sUI c = sUI(_text[charIdx++]);
            if(c > 0u)
            {
               if(sUI('\n') == c)
               {
                  if(0 == cx)
                  {
                     cy += _font->info->height;
                  }
                  else
                  {
                     cx = 0;
                  }
               }
               else if(c >= _font->info->first_glyph)
               {
                  c -= _font->info->first_glyph;
                  if(c < _font->info->num_glyphs)
                  {
                     const sdvg_glyph_t *g = &_font->glyphs[c];

                     if(0 == cx)
                        cy += _font->info->height;

                     const sSI xl = cx + g->offset_x;
                     const sSI xr = xl + g->w;
                     const sSI yt = cy - g->offset_y;
                     const sSI yb = yt + g->h;

                     if(xl < minX)
                        minX = xl;
                     if(xr > maxX)
                        maxX = xr;

                     if(yt < minY)
                        minY = yt;
                     if(yb > maxY)
                        maxY = yb;

                     // Dprintf("xxx char=\'%c\' yt=%d yb=%d maxY=%d w=%d h=%d off=(%d, %d)\n", char(c+shadervg_cur_font->info->first_glyph), yt, yb, maxY, g->w, g->h, g->offset_x, g->offset_y);

                     cx += g->advance_x;
                  }
                  // else: invalid glyph (skip)
               }
               // else: before first glyph (skip)
            }
            else
            {
               // ASCIIZ / end of string
               break;
            }
         } // iterate chars
      }
      // else: NULL text (skip)
   }
   else
   {
      Dsdvg_errorprintf("[---] shadervg:sdvg_TextWidth: font is NULL\n");
   }

   if(minX < 999999)
      *_retMinX = minX;
   else
      *_retMinX = 0;

   if(maxX > 0)
      *_retMaxX = maxX;
   else
      *_retMaxX = 0;

   if(minY < 999999)
      *_retMinY = minY;
   else
      *_retMinY = 0;

   if(maxY > 0)
      *_retMaxY = maxY;
   else
      *_retMaxY = 0;
}

sSI YAC_CALL sdvg_TextIndexAtX(const sdvg_font_t *_font, const char *_text, sSI _x) {
   if(NULL != _text)
   {
      sSI w = 0;
      sUI charIdx = 0u;
      for(;;)
      {
         sUI c = sUI(_text[charIdx++]);
         if(c > 0u)
         {
            if(c > _font->info->first_glyph)
            {
               c -= _font->info->first_glyph;
               const sdvg_glyph_t *g = &_font->glyphs[c];
               w += g->advance_x;
               if(w > (_x + sSI(sUI(g->advance_x)/2u)))
                  return sSI(charIdx - 1u);
            }
         }
         else
         {
            return sSI(charIdx - 1u);
         }
      }
   }
   return -1;
}

void YAC_CALL sdvg_BindFont(const sdvg_font_t *_font, sBool _bFilter) {
   shadervg_cur_font = _font;
   sdvg_BindTexture2D(shadervg_cur_font->tex_id, YAC_FALSE/*bRepeat*/, _bFilter);
}

void YAC_CALL sdvg_DrawText(const char *_text, sF32 _x, sF32 _y) {
   if(NULL != shadervg_cur_font && NULL != shadervg_cur_font->info)
   {
      if(NULL != _text)
      {
         _x += 0.375f;
         _y += 0.375f;
         sF32 cx = _x;
         sF32 cy = _y + shadervg_cur_font->info->ascender;
         sUI charIdx = 0u;
         sUI quadIdx = 0u;
         const sF32 us = shadervg_cur_font->info->us;
         const sF32 vs = shadervg_cur_font->info->vs;
         const sF32 sdfRadius = sF32(shadervg_cur_font->info->sdf_radius);
         if(sdfRadius > 0.0f)
            (void)sdvg_BeginTexturedTrianglesAlphaSDF(SHADERVG_TEXT_BATCH_SIZE * (2u * 3u));
         else
            (void)sdvg_BeginTexturedTrianglesAlpha(SHADERVG_TEXT_BATCH_SIZE * (2u * 3u));
         for(;;)
         {
            sUI c = sUI(_text[charIdx++]);
            if(c > 0u)
            {
               if(sUI('\n') == c)
               {
                  cx = _x;
                  cy += shadervg_cur_font->info->height;
               }
               else if(c >= shadervg_cur_font->info->first_glyph)
               {
                  c -= shadervg_cur_font->info->first_glyph;
                  if(c < shadervg_cur_font->info->num_glyphs)
                  {
                     if(quadIdx == SHADERVG_TEXT_BATCH_SIZE)
                     {
                        sdvg_End();

                        if(sdfRadius > 0.0f)
                           (void)sdvg_BeginTexturedTrianglesAlphaSDF(SHADERVG_TEXT_BATCH_SIZE * (2u * 3u));
                        else
                           (void)sdvg_BeginTexturedTrianglesAlpha(SHADERVG_TEXT_BATCH_SIZE * (2u * 3u));
                        quadIdx = 0u;
                     }

                     const sdvg_glyph_t *g = &shadervg_cur_font->glyphs[c];

                     sF32 ul = (g->x          - sdfRadius) * us;
                     sF32 ur = ((g->x + g->w) + sdfRadius) * us;
                     sF32 vt = (g->y          - sdfRadius) * vs;
                     sF32 vb = ((g->y + g->h) + sdfRadius) * vs;

                     sF32 xl = cx + g->offset_x - sdfRadius;
                     sF32 xr = xl + g->w        + sdfRadius*2;
                     sF32 yt = cy - g->offset_y - sdfRadius;
                     sF32 yb = yt + g->h        + sdfRadius*2;

                     // Dprintf("xxx char=\'%c\' yt=%f yb=%f w=%d h=%d off=(%d, %d)\n", char(c+shadervg_cur_font->info->first_glyph), yt, yb, g->w, g->h, g->offset_x, g->offset_y);

                     // 1
                     sdvg_TexCoord2f(ul, vt);
                     sdvg_Vertex2f(xl, yt);

                     sdvg_TexCoord2f(ur, vt);
                     sdvg_Vertex2f(xr, yt);

                     sdvg_TexCoord2f(ur, vb);
                     sdvg_Vertex2f(xr, yb);

                     // 2
                     sdvg_TexCoord2f(ul, vt);
                     sdvg_Vertex2f(xl, yt);

                     sdvg_TexCoord2f(ur, vb);
                     sdvg_Vertex2f(xr, yb);

                     sdvg_TexCoord2f(ul, vb);
                     sdvg_Vertex2f(xl, yb);

                     // Next char
                     cx += g->advance_x;
                     quadIdx++;
                  }
                  // else: invalid glyph (skip)
               }
               // else: before first_glyph (skip)
            }
            else
            {
               // ASCIIZ / end of string
               break;
            }
         } // iterate chars
         sdvg_End();
      }
      // else: NULL text (skip)
   }
   else
   {
      Dsdvg_errorprintf("[---] shadervg:sdvg_DrawText: font is NULL\n");
   }
}

void YAC_CALL sdvg_DrawTextClipped(const char *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom) {
   if(NULL != shadervg_cur_font && NULL != shadervg_cur_font->info)
   {
      if(NULL != _text)
      {
         _x += 0.375f;
         _y += 0.375f;
         sUI charIdx = 0u;
         sUI quadIdx = 0u;
         const sF32 us = shadervg_cur_font->info->us;
         const sF32 vs = shadervg_cur_font->info->vs;
         const sF32 sdfRadius = sF32(shadervg_cur_font->info->sdf_radius);
         if(sdfRadius > 0.0f)
            (void)sdvg_BeginTexturedTrianglesAlphaSDF(SHADERVG_TEXT_BATCH_SIZE * (2u * 3u));
         else
            (void)sdvg_BeginTexturedTrianglesAlpha(SHADERVG_TEXT_BATCH_SIZE * (2u * 3u));
         sF32 cx = _x;
         sF32 cy = _y + shadervg_cur_font->info->ascender;
         for(;;)
         {
            sUI c = sUI(_text[charIdx++]);
            if(c > 0u)
            {
               if(sUI('\n') == c)
               {
                  cx = _x;
                  cy += shadervg_cur_font->info->height;
               }
               else if(c >= shadervg_cur_font->info->first_glyph)
               {
                  c -= shadervg_cur_font->info->first_glyph;
                  if(c < shadervg_cur_font->info->num_glyphs)
                  {
                     if(quadIdx == SHADERVG_TEXT_BATCH_SIZE)
                     {
                        sdvg_End();

                        if(sdfRadius > 0.0f)
                           (void)sdvg_BeginTexturedTrianglesAlphaSDF(SHADERVG_TEXT_BATCH_SIZE * (2u * 3u));
                        else
                           (void)sdvg_BeginTexturedTrianglesAlpha(SHADERVG_TEXT_BATCH_SIZE * (2u * 3u));
                        quadIdx = 0u;
                     }

                     const sdvg_glyph_t *g = &shadervg_cur_font->glyphs[c];

                     sF32 ul = g->x          - sdfRadius;
                     sF32 ur = (g->x + g->w) + sdfRadius;
                     sF32 vt = g->y          - sdfRadius;
                     sF32 vb = (g->y + g->h) + sdfRadius;

                     sF32 xl = cx + g->offset_x - sdfRadius;
                     sF32 xr = xl + g->w        + sdfRadius*2;
                     sF32 yt = cy - g->offset_y - sdfRadius;
                     sF32 yb = yt + g->h        + sdfRadius*2;

                     if(xr >= _clipLeft && xl < _clipRight &&
                        yb >= _clipTop  && yt < _clipBottom
                        )
                     {
                        if(xl < _clipLeft)
                        {
                           ul += (_clipLeft - xl);
                           xl = _clipLeft;
                        }

                        if(yt < _clipTop)
                        {
                           vt += (_clipTop - yt);
                           yt = _clipTop;
                        }

                        if(xr > _clipRight)
                        {
                           ur -= (xr - _clipRight);
                           xr = _clipRight;
                        }

                        if(yb > _clipBottom)
                        {
                           vb -= (yb - _clipBottom);
                           yb = _clipBottom;
                        }

                        ul *= us;
                        ur *= us;
                        vt *= vs;
                        vb *= vs;

                        // Dprintf("xxx char=\'%c\' yt=%f yb=%f w=%d h=%d off=(%d, %d)\n", char(c+shadervg_cur_font->info->first_glyph), yt, yb, g->w, g->h, g->offset_x, g->offset_y);

                        // 1
                        sdvg_TexCoord2f(ul, vt);
                        sdvg_Vertex2f(xl, yt);

                        sdvg_TexCoord2f(ur, vt);
                        sdvg_Vertex2f(xr, yt);

                        sdvg_TexCoord2f(ur, vb);
                        sdvg_Vertex2f(xr, yb);

                        // 2
                        sdvg_TexCoord2f(ul, vt);
                        sdvg_Vertex2f(xl, yt);

                        sdvg_TexCoord2f(ur, vb);
                        sdvg_Vertex2f(xr, yb);

                        sdvg_TexCoord2f(ul, vb);
                        sdvg_Vertex2f(xl, yb);

                        quadIdx++;
                     }

                     // Next char
                     cx += g->advance_x;
                  }
                  // else: invalid glyph (skip)
               }
               // else: before first_glyph (skip)
            }
            else
            {
               // ASCIIZ / end of string
               break;
            }
         } // iterate chars
         sdvg_End();
      }
      // else: NULL text (skip)
   }
   else
   {
      Dsdvg_errorprintf("[---] shadervg:sdvg_DrawTextClipped: font is NULL\n");
   }
}

void YAC_CALL sdvg_DrawTextUnderline(const char *_text, sF32 _x, sF32 _y) {
   if(NULL != shadervg_cur_font && NULL != shadervg_cur_font->info)
   {
      if(NULL != _text)
      {
         // // _x += 0.375f;
         // // _y += 0.375f;
         sF32 cx = _x;
         sF32 cy = _y + shadervg_cur_font->info->ascender - (shadervg_cur_font->info->underline_position * (1.0f / 64.0f));
         sUI charIdx = 0u;
         sUI lineIdx = 0u;
         sdvg_SetStrokeWidth(shadervg_cur_font->info->underline_thickness * (0.5f / 64.0f));
         (void)sdvg_BeginLinesAA(SHADERVG_TEXT_BATCH_SIZE * 2u);
         for(;;)
         {
            sUI c = sUI(_text[charIdx++]);
            if(c > 0u)
            {
               if(sUI('\n') == c)
               {
                  if(cx != _x)
                  {
                     sdvg_Vertex2f(_x, cy);
                     sdvg_Vertex2f(cx, cy);
                  }
                  cx = _x;
                  cy += shadervg_cur_font->info->height;
               }
               else if(c >= shadervg_cur_font->info->first_glyph)
               {
                  c -= shadervg_cur_font->info->first_glyph;
                  if(c < shadervg_cur_font->info->num_glyphs)
                  {
                     if(lineIdx == SHADERVG_TEXT_UNDERLINE_BATCH_SIZE)
                     {
                        sdvg_End();

                        (void)sdvg_BeginLinesAA(SHADERVG_TEXT_UNDERLINE_BATCH_SIZE * 2u);
                        lineIdx = 0u;
                     }

                     const sdvg_glyph_t *g = &shadervg_cur_font->glyphs[c];
                     cx += g->advance_x;

                     // Next char
                     lineIdx++;
                  }
                  // else: invalid glyph (skip)
               }
               // else: before first_glyph (skip)
            }
            else
            {
               // ASCIIZ / end of string
               if(cx != _x)
               {
                  sdvg_Vertex2f(_x, cy);
                  sdvg_Vertex2f(cx, cy);
               }
               break;
            }
         } // iterate chars
         sdvg_End();
      }
      // else: NULL text (skip)
   }
   else
   {
      Dsdvg_errorprintf("[---] shadervg:sdvg_DrawTextUnderline: font is NULL\n");
   }
}

void YAC_CALL sdvg_DrawTextUnderlineClipped(const char *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom) {
   if(NULL != shadervg_cur_font && NULL != shadervg_cur_font->info)
   {
      if(NULL != _text)
      {
         // // _x += 0.375f;
         // // _y += 0.375f;
         sF32 cx = _x;
         sF32 cy = _y + shadervg_cur_font->info->ascender - (shadervg_cur_font->info->underline_position * (1.0f / 64.0f));
         sUI charIdx = 0u;
         sUI lineIdx = 0u;
         const sF32 strokeW = shadervg_cur_font->info->underline_thickness * (0.5f / 64.0f);
         sdvg_SetStrokeWidth(strokeW);
         (void)sdvg_BeginLinesAA(SHADERVG_TEXT_BATCH_SIZE * 2u);
         for(;;)
         {
            sUI c = sUI(_text[charIdx++]);
            if(c > 0u)
            {
               if(sUI('\n') == c)
               {
                  if(cx != _x)
                  {
                     sF32 x1 = _x;
                     sF32 x2 = cx;
                     if(x2 >= _clipLeft && x1 < _clipRight &&
                        ((cy - strokeW) >= _clipTop) &&
                        ((cy + strokeW)  < _clipBottom)
                        )
                     {
                        if(x1 < _clipLeft)
                           x1 = _clipLeft;
                        if(x2 > _clipRight)
                           x2 = _clipRight;

                        sdvg_Vertex2f(x1, cy);
                        sdvg_Vertex2f(x2, cy);
                     }
                  }
                  cx = _x;
                  cy += shadervg_cur_font->info->height;
               }
               else if(c >= shadervg_cur_font->info->first_glyph)
               {
                  c -= shadervg_cur_font->info->first_glyph;
                  if(c < shadervg_cur_font->info->num_glyphs)
                  {
                     if(lineIdx == SHADERVG_TEXT_UNDERLINE_BATCH_SIZE)
                     {
                        sdvg_End();

                        (void)sdvg_BeginLinesAA(SHADERVG_TEXT_UNDERLINE_BATCH_SIZE * 2u);
                        lineIdx = 0u;
                     }

                     const sdvg_glyph_t *g = &shadervg_cur_font->glyphs[c];
                     cx += g->advance_x;

                     // Next char
                     lineIdx++;
                  }
                  // else: invalid glyph (skip)
               }
               // else: before first_glyph (skip)
            }
            else
            {
               // ASCIIZ / end of string
               if(cx != _x)
               {
                  sF32 x1 = _x;
                  sF32 x2 = cx;
                  if(x2 >= _clipLeft && x1 < _clipRight &&
                     ((cy - strokeW) >= _clipTop) &&
                     ((cy + strokeW)  < _clipBottom)
                     )
                  {
                     if(x1 < _clipLeft)
                        x1 = _clipLeft;
                     if(x2 > _clipRight)
                        x2 = _clipRight;

                     sdvg_Vertex2f(x1, cy);
                     sdvg_Vertex2f(x2, cy);
                  }
               }
               break;
            }
         } // iterate chars
         sdvg_End();
      }
      // else: NULL text (skip)
   }
   else
   {
      Dsdvg_errorprintf("[---] shadervg:sdvg_DrawTextUnderlineClipped: font is NULL\n");
   }
}

void YAC_CALL sdvg_DrawTextAccel(const char *_text, sF32 _x, sF32 _y) {
   if(NULL != shadervg_cur_font && NULL != shadervg_cur_font->info)
   {
      if(NULL != _text)
      {
         // // _x += 0.375f;
         // // _y += 0.375f;
         sF32 cx = _x;
         sF32 cy = _y + shadervg_cur_font->info->ascender - (shadervg_cur_font->info->underline_position * (1.0f / 64.0f));
         sUI charIdx = 0u;
         sUI lineIdx = 0u;
         sBool bNextIsAccel = YAC_FALSE;
         sdvg_SetStrokeWidth(shadervg_cur_font->info->underline_thickness * (0.5f / 64.0f));
         (void)sdvg_BeginLinesAA(SHADERVG_TEXT_BATCH_SIZE * 2u);
         for(;;)
         {
            sUI c = sUI(_text[charIdx++]);
            if(c > 0u)
            {
               if(sUI('\n') == c)
               {
                  bNextIsAccel = YAC_FALSE;
                  cx = _x;
                  cy += shadervg_cur_font->info->height;
               }
               else if(sUI('\a') == c)
               {
                  bNextIsAccel = YAC_TRUE;
               }
               else if(c >= shadervg_cur_font->info->first_glyph)
               {
                  c -= shadervg_cur_font->info->first_glyph;
                  if(c < shadervg_cur_font->info->num_glyphs)
                  {
                     if(lineIdx == SHADERVG_TEXT_UNDERLINE_BATCH_SIZE)
                     {
                        sdvg_End();

                        (void)sdvg_BeginLinesAA(SHADERVG_TEXT_UNDERLINE_BATCH_SIZE * 2u);
                        lineIdx = 0u;
                     }

                     const sdvg_glyph_t *g = &shadervg_cur_font->glyphs[c];

                     if(bNextIsAccel)
                     {
                        bNextIsAccel = YAC_FALSE;

                        sdvg_Vertex2f(cx, cy);
                        sdvg_Vertex2f(cx + g->advance_x, cy);
                     }

                     cx += g->advance_x;

                     // Next char
                     lineIdx++;
                  }
                  // else: invalid glyph (skip)
               }
               // else: before first_glyph (skip)
            }
            else
            {
               // ASCIIZ / end of string
               break;
            }
         } // iterate chars
         sdvg_End();
      }
      // else: NULL text (skip)
   }
   else
   {
      Dsdvg_errorprintf("[---] shadervg:sdvg_DrawTextAccel: font is NULL\n");
   }
}

void YAC_CALL sdvg_DrawTextAccelClipped(const char *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom) {
   if(NULL != shadervg_cur_font && NULL != shadervg_cur_font->info)
   {
      if(NULL != _text)
      {
         // // _x += 0.375f;
         // // _y += 0.375f;
         sF32 cx = _x;
         sF32 cy = _y + shadervg_cur_font->info->ascender - (shadervg_cur_font->info->underline_position * (1.0f / 64.0f));
         sUI charIdx = 0u;
         sUI lineIdx = 0u;
         sBool bNextIsAccel = YAC_FALSE;
         const sF32 strokeW = shadervg_cur_font->info->underline_thickness * (0.5f / 64.0f);
         sdvg_SetStrokeWidth(strokeW);
         (void)sdvg_BeginLinesAA(SHADERVG_TEXT_BATCH_SIZE * 2u);
         for(;;)
         {
            sUI c = sUI(_text[charIdx++]);
            if(c > 0u)
            {
               if(sUI('\n') == c)
               {
                  bNextIsAccel = YAC_FALSE;
                  cx = _x;
                  cy += shadervg_cur_font->info->height;
               }
               else if(sUI('\a') == c)
               {
                  bNextIsAccel = YAC_TRUE;
               }
               else if(c >= shadervg_cur_font->info->first_glyph)
               {
                  c -= shadervg_cur_font->info->first_glyph;
                  if(c < shadervg_cur_font->info->num_glyphs)
                  {
                     if(lineIdx == SHADERVG_TEXT_UNDERLINE_BATCH_SIZE)
                     {
                        sdvg_End();

                        (void)sdvg_BeginLinesAA(SHADERVG_TEXT_UNDERLINE_BATCH_SIZE * 2u);
                        lineIdx = 0u;
                     }

                     const sdvg_glyph_t *g = &shadervg_cur_font->glyphs[c];

                     if(bNextIsAccel)
                     {
                        bNextIsAccel = YAC_FALSE;

                        sF32 x1 = cx;
                        sF32 x2 = cx + g->advance_x;
                        if(x2 >= _clipLeft && x1 < _clipRight &&
                           ((cy - strokeW) >= _clipTop) &&
                           ((cy + strokeW)  < _clipBottom)
                           )
                        {
                           if(x1 < _clipLeft)
                              x1 = _clipLeft;
                           if(x2 > _clipRight)
                              x2 = _clipRight;

                           sdvg_Vertex2f(x1, cy);
                           sdvg_Vertex2f(x2, cy);
                        }
                     }

                     cx += g->advance_x;

                     // Next char
                     lineIdx++;
                  }
                  // else: invalid glyph (skip)
               }
               // else: before first_glyph (skip)
            }
            else
            {
               // ASCIIZ / end of string
               break;
            }
         } // iterate chars
         sdvg_End();
      }
      // else: NULL text (skip)
   }
   else
   {
      Dsdvg_errorprintf("[---] shadervg:sdvg_DrawTextUnderlineClipped: font is NULL\n");
   }
}

void YAC_CALL sdvg_UnbindFont(void) {
   shadervg_cur_font = NULL;
   sdvg_UnbindTexture2D();
}

#ifdef SHADERVG_SCRIPT_API
sBool YAC_CALL _sdvg_InitFont(YAC_Object *_font, YAC_Object *_data, YAC_Object *_texData, sUI _texW, sUI _texH) {
   sBool r = YAC_FALSE;
   if(YAC_CHK(_font, clid_ShaderVG_Font))
   {
      YAC_CAST_ARG(_ShaderVG_Font, font, _font);

      if(YAC_VALID(_data))
      {
         const void *data = _data->yacArrayGetPointer();
         const sUI dataSz = _data->yacArrayGetElementByteSize() * _data->yacArrayGetNumElements();

         if(NULL != data && dataSz > 0u)
         {
            if(YAC_VALID(_texData))
            {
               const void *texData = _texData->yacArrayGetPointer();
               const sUI texDataSz = _texData->yacArrayGetElementByteSize() * _texData->yacArrayGetNumElements();
               if(texDataSz >= (_texW * _texH))
               {
                  r = sdvg_InitFont(&font->font, data, dataSz, texData, _texW, _texH);

                  if(!r)
                  {
                     Dyac_host_printf("[---] sdvg_InitFont: native sdvg_InitFont() failed\n");
                  }
               }
               else
               {
                  Dyac_host_printf("[---] sdvg_InitFont: invalid texDataSz (expect=%u have=%u)\n", (_texW*_texH), texDataSz);
               }
            }
            else
            {
               Dyac_host_printf("[---] sdvg_InitFont: texData pointer is NULL\n");
            }
         }
         else
         {
            Dyac_host_printf("[---] sdvg_InitFont: invalid data (dataPtr=%p dataSz=%u)\n", data, dataSz);
         }
      }
      else
      {
         Dyac_host_printf("[---] sdvg_InitFont: invalid data object\n");
      }
   }
   else
   {
      Dyac_host_printf("[---] sdvg_InitFont: invalid font object\n");
   }
   return r;
}

sBool YAC_CALL _sdvg_FontOnOpen(YAC_Object *_font) {
   sBool r = YAC_FALSE;
   if(YAC_CHK(_font, clid_ShaderVG_Font))
   {
      YAC_CAST_ARG(_ShaderVG_Font, font, _font);

      r = sdvg_FontOnOpen(&font->font);
   }
   return r;
}

void YAC_CALL _sdvg_TextExtents(YAC_Object *_font, YAC_String *_text, YAC_Object *_retW, YAC_Object *_retH) {
   sSI retW = 0;
   sSI retH = 0;
   if(YAC_CHK(_font, clid_ShaderVG_Font))
   {
      YAC_CAST_ARG(_ShaderVG_Font, font, _font);

      if(YAC_Is_String(_text))
      {
         YAC_CAST_STRING(text, _text);

         if(NULL != _text->chars)
         {
            sdvg_TextExtents(&font->font, (const char*)text->chars, &retW, &retH);
         }
      }
   }

   if(YAC_VALID(_retW))
      _retW->yacValueOfI(retW);

   if(YAC_VALID(_retH))
      _retH->yacValueOfI(retH);
}

sSI YAC_CALL _sdvg_TextWidth(YAC_Object *_font, YAC_String *_text) {
   sSI r = 0;
   if(YAC_CHK(_font, clid_ShaderVG_Font))
   {
      YAC_CAST_ARG(_ShaderVG_Font, font, _font);

      if(YAC_Is_String(_text))
      {
         YAC_CAST_STRING(text, _text);

         if(NULL != _text->chars)
         {
            r = sdvg_TextWidth(&font->font, (const char*)text->chars);
         }
      }
   }
   return r;
}

sSI YAC_CALL _sdvg_TextHeight(YAC_Object *_font, YAC_String *_text) {
   sSI r = 0;
   if(YAC_CHK(_font, clid_ShaderVG_Font))
   {
      YAC_CAST_ARG(_ShaderVG_Font, font, _font);

      if(YAC_Is_String(_text))
      {
         YAC_CAST_STRING(text, _text);
         if(NULL != _text->chars)
         {
            r = sdvg_TextHeight(&font->font, (const char*)text->chars);
         }
      }
   }
   return r;
}

void YAC_CALL _sdvg_TextBBox(YAC_Object *_font, YAC_String *_text, YAC_Object *_retMinX, YAC_Object *_retMaxX, YAC_Object *_retMinY, YAC_Object *_retMaxY) {
   sSI retMinX = 0;
   sSI retMaxX = 0;
   sSI retMinY = 0;
   sSI retMaxY = 0;
   if(YAC_CHK(_font, clid_ShaderVG_Font))
   {
      YAC_CAST_ARG(_ShaderVG_Font, font, _font);

      if(YAC_Is_String(_text))
      {
         YAC_CAST_STRING(text, _text);
         if(NULL != _text->chars)
         {
            sdvg_TextBBox(&font->font, (const char*)text->chars, &retMinX, &retMaxX, &retMinY, &retMaxY);
         }
      }
   }

   if(YAC_VALID(_retMinX))
      _retMinX->yacValueOfI(retMinX);

   if(YAC_VALID(_retMaxX))
      _retMaxX->yacValueOfI(retMaxX);

   if(YAC_VALID(_retMinY))
      _retMinY->yacValueOfI(retMinY);

   if(YAC_VALID(_retMaxY))
      _retMaxY->yacValueOfI(retMaxY);
}

sSI YAC_CALL _sdvg_TextIndexAtX(YAC_Object *_font, YAC_String *_text, sSI _x) {
   sSI r = -1;
   if(YAC_CHK(_font, clid_ShaderVG_Font))
   {
      YAC_CAST_ARG(_ShaderVG_Font, font, _font);

      if(YAC_Is_String(_text))
      {
         YAC_CAST_STRING(text, _text);
         if(NULL != text->chars)
         {
            r = sdvg_TextIndexAtX(&font->font, (const char*)text->chars, _x);
         }
      }
   }
   return r;
}

void YAC_CALL _sdvg_BindFont(YAC_Object *_font, sBool _bFilter) {
   if(YAC_CHK(_font, clid_ShaderVG_Font))
   {
      YAC_CAST_ARG(_ShaderVG_Font, font, _font);

      if(NULL != font->font.info)
         sdvg_BindFont(&font->font, _bFilter);
   }
}

void YAC_CALL _sdvg_DrawText(YAC_String *_text, sF32 _x, sF32 _y) {
   if(YAC_Is_String(_text))
   {
      YAC_CAST_STRING(text, _text);
      if(NULL != _text->chars)
      {
         sdvg_DrawText((const char*)text->chars, _x, _y);
      }
   }
}

void YAC_CALL _sdvg_DrawTextClipped(YAC_String *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom) {
   if(YAC_Is_String(_text))
   {
      YAC_CAST_STRING(text, _text);
      if(NULL != _text->chars)
      {
         sdvg_DrawTextClipped((const char*)text->chars, _x, _y, _clipLeft, _clipTop, _clipRight, _clipBottom);
      }
   }
}

void YAC_CALL _sdvg_DrawTextUnderline(YAC_String *_text, sF32 _x, sF32 _y) {
   if(YAC_Is_String(_text))
   {
      YAC_CAST_STRING(text, _text);
      if(NULL != _text->chars)
      {
         sdvg_DrawTextUnderline((const char*)text->chars, _x, _y);
      }
   }
}

void YAC_CALL _sdvg_DrawTextUnderlineClipped(YAC_String *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom) {
   if(YAC_Is_String(_text))
   {
      YAC_CAST_STRING(text, _text);
      if(NULL != _text->chars)
      {
         sdvg_DrawTextUnderlineClipped((const char*)text->chars, _x, _y, _clipLeft, _clipTop, _clipRight, _clipBottom);
      }
   }
}

void YAC_CALL _sdvg_DrawTextAccel(YAC_String *_text, sF32 _x, sF32 _y) {
   if(YAC_Is_String(_text))
   {
      YAC_CAST_STRING(text, _text);
      if(NULL != _text->chars)
      {
         sdvg_DrawTextAccel((const char*)text->chars, _x, _y);
      }
   }
}

void YAC_CALL _sdvg_DrawTextAccelClipped(YAC_String *_text, sF32 _x, sF32 _y, sF32 _clipLeft, sF32 _clipTop, sF32 _clipRight, sF32 _clipBottom) {
   if(YAC_Is_String(_text))
   {
      YAC_CAST_STRING(text, _text);
      if(NULL != _text->chars)
      {
         sdvg_DrawTextAccelClipped((const char*)text->chars, _x, _y, _clipLeft, _clipTop, _clipRight, _clipBottom);
      }
   }
}

#endif // SHADERVG_SCRIPT_API

#endif // SHADERVG_TEXT
