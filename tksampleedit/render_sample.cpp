// ----
// ---- file   : render_sample.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   :
// ----
// ---- created: 25Oct2009
// ---- changed: 29Apr2010, 11May2010, 25Feb2011, 04May2013, 07Mar2014, 18Dec2018, 15Jan2019
// ----          06May2019, 15Sep2023, 16Sep2023, 10Nov2023, 22Dec2023, 13Sep2025, 16Sep2025
// ----          18Sep2025
// ----
// ----
// ----

#include "tksampleedit.h"

#ifdef USE_TKMINNIE
#include "../minnie/tkminnie/tkminnie_shared.h"
#else
#include "../tkopengl/tkopengl_shared.h"
#endif // USE_TKMINNIE

#define RS_DOTS         0
#define RS_LINES        1
#define RS_DELTA        2
#define RS_GRADIENT     3
#define RS_LINES_OUTER  4

#define IS_SH          0
#define IS_LINEAR      1
#define IS_SMOOTHSTEP  2
#define IS_AVG8        3

#define FFT_EXP 6.0f

typedef sF32 (*get_sample_fxn_t)(const sF32 *s, const sUI maxSamples, const sUI numCh, const sF64 off);



static sF32 get_sample_sh(const sF32 *s, const sUI maxSamples, const sUI numCh, const sF64 off) {
   (void)maxSamples;

   sUI i = ((sUI) off) * numCh;

   if(i < maxSamples)
   {
      return s[i];
   }
   else
   {
      return 0.0f;
   }
}

static sF32 get_sample_linear(const sF32 *s, const sUI maxSamples, const sUI numCh, const sF64 off) {
   sUI i = ((sUI) off) * numCh;
   sF64 f = off - ((sUI)off);

   if(i < (maxSamples-numCh))
   {
      return sF32(s[i]*(1.0f-f) + s[i+numCh]*f);
   }
   else
   {
      return sF32(s[maxSamples-numCh]);
   }
}

static sF32 get_sample_smoothstep(const sF32 *s, const sUI maxSamples, const sUI numCh, const sF64 off) {
   sUI i = ((sUI) off) * numCh;
   sF64 f = off - ((sUI)off);

   if(i < (maxSamples-numCh))
   {
      f = (f*f) * (3.0f - 2.0f*f);

      return sF32(s[i]*(1.0f-f) + s[i+numCh]*f);
   }
   else
   {
      return 0.0f;
   }
}

static sF32 get_sample_avg8(const sF32 *s, const sUI maxSamples, const sUI numCh, const sF64 off) {
   sUI i = ((sUI) off);
   sF64 sum = 0.0f;
   sSI x;

   for(x=0; x<8; x++)
   {
      sUI k = ((i + x) - 3) * numCh;

      if(k < maxSamples)
      {
         sum += s[k];
      }
   }
   return sF32(sum / 8.0);
}



static void render_sample_dots(const sF32 *s,
                               const sUI _startOffset,
                               const sUI _numCh,
                               const sUI _numFrames,
                               const sUI _maxSamples,
                               sF32 _w, sF32 _h,
                               sF32 _min, sF32 _max,
                               get_sample_fxn_t _readFxn,
                               sF32 _offY
                               )
{
   sF32 scly = (_h / (_max - _min));

   ////yac_host->printf("xxx render_sample_skip_sh #ch=%u #samples=%u w=%f h=%f\n", numCh, numFrames, _w, _h);

#ifdef USE_TKMINNIE
   tkminnie_shared->sdvg_BeginPointsRoundAA(TKMINNIE_MAX_POINTS);
#else
   tkopengl_shared->_glBegin(GL_POINTS);
#endif // USE_TKMINNIE

   sF64 off = (sF32)_startOffset;
   sF64 spd = _numFrames / _w;
   sF64 maxFrames = (sF32) (_maxSamples / _numCh);

   sF32 cx = 0.0f;

   while(cx < _w)
   {
      cx += 1.0f;
      sF32 cy = _h - ( ((_readFxn)(s, _maxSamples, _numCh, off) - _min) * scly ) + _offY;

#ifdef USE_TKMINNIE
      tkminnie_shared->sdvg_Vertex2f(cx, cy);
#else
      tkopengl_shared->_glVertex2f(cx, cy);
#endif // USE_TKMINNIE

      off += spd;

      if(off >= maxFrames)
      {
         off -= maxFrames;
      }
   }

#ifdef USE_TKMINNIE
   tkminnie_shared->sdvg_End();
#else
   tkopengl_shared->_glEnd();
#endif // USE_TKMINNIE
}

static void render_sample_lines(sF32 *s,
                                sUI _startOffset, sUI _numCh, sUI _numFrames, sUI _maxSamples,
                                sF32 _w, sF32 _h,
                                sF32 _min, sF32 _max,
                                sUI _c32Skip,
                                get_sample_fxn_t _readFxn,
                                sF32 _offY
                                )
{
#if 0
   sUI tStart = yac_host->yacMilliSeconds();
#endif

   sF32 scly = (_h / (_max - _min));

   // yac_host->printf("xxx render_sample_lines: #ch=%u #samples=%u w=%f h=%f\n", numCh, numFrames, _w, _h);

   sF64 off = (sF32)_startOffset;
   sF64 spd = _numFrames / _w;
   sSI num = (sSI) spd;
   sF64 maxFrames = (sF32) (_maxSamples / _numCh);

   // first sample

   sF32 cx = 0.0;
   sUI totalNumVerts = 0u;

   if(spd < 1.0)
   {
#ifdef USE_TKMINNIE
      tkminnie_shared->sdvg_BeginLineStripAA(TKMINNIE_MAX_POINTS);
      sF32 lx = 0.0f;
      sF32 ly = _h - ( (s[0] - _min) * scly ) + _offY;
      tkminnie_shared->sdvg_Vertex2f(lx, ly);
      sUI vtxIdx = 1u;
#else
      tkopengl_shared->_glBegin(GL_LINE_STRIP);
      tkopengl_shared->_glVertex2f(0.0f,
                                   _h - ( (s[0] - _min) * scly ) + _offY
                                   );
#endif // USE_TKMINNIE

      // yac_host->printf("xxx draw samples 1\n");

      while(cx < _w)
      {
         // Read windowed sample
         sF32 cy = _h - ( ((_readFxn)(s, _maxSamples, _numCh, off) - _min) * scly ) + _offY;

#ifdef USE_TKMINNIE
         // Dyac_host_printf("xxx render_lines: vtxIdx=%u\n", vtxIdx);
         if(vtxIdx == TKMINNIE_MAX_POINTS)
         {
            totalNumVerts += vtxIdx;
            tkminnie_shared->sdvg_End();
            tkminnie_shared->sdvg_BeginLineStripAA(TKMINNIE_MAX_POINTS);
            tkminnie_shared->sdvg_Vertex2f(lx, ly);
            vtxIdx = 1u;
         }
         tkminnie_shared->sdvg_Vertex2f(cx, cy);
         lx = cx;
         ly = cy;
         vtxIdx++;
#else
         tkopengl_shared->_glVertex2f(cx, cy);
#endif // USE_TKMINNIE

         off += spd;

         if(off >= maxFrames)
            off -= maxFrames;

         cx += 1.0f;
      }

#ifdef USE_TKMINNIE
      totalNumVerts += vtxIdx;
      tkminnie_shared->sdvg_End();
#else
      tkopengl_shared->_glEnd();
#endif // USE_TKMINNIE
   }
   else
   {
      // yac_host->printf("xxx draw samples 2\n");

      num++;
      if(1)
      {
#ifndef NO_SHADE
         // Shade
         if(num > 128) // 128
            num = 128;
#else
         num = 1;
#endif // NO_SHADE

         float red   = ((_c32Skip >> 16)&255) / 255.0f;
         float green = ((_c32Skip >>  8)&255) / 255.0f;
         float blue  = ((_c32Skip      )&255) / 255.0f;
         float maxAlpha = (((_c32Skip >> 24)&255) / 255.0f);
         float alpha = (sF32)( maxAlpha / spd );
         // // alpha = powf(alpha, 1.7f) * 0.75f;
         // Dyac_host_printf("xxx render_samples-lines spd >= 1.0 alpha_pre_clip=%f\n", alpha);
         if(alpha < 0.03f) // 0.03f
            alpha = 0.03f;
         else if(alpha > maxAlpha)
            alpha = maxAlpha;

         // Dyac_host_printf("xxx render_samples-lines spd >= 1.0 alpha=%f\n", alpha);
         sF32 zeroy = _h - ( (0.0f - _min) * scly ) + _offY;

#ifdef USE_TKMINNIE
         sUI vtxIdx = 0u;
         tkminnie_shared->sdvg_SetStrokeColor4f(red, green, blue, alpha);
         tkminnie_shared->sdvg_BeginLinesAA(TKMINNIE_MAX_POINTS*2);
#else
         tkopengl_shared->_glColor4f(red, green, blue, alpha);
         tkopengl_shared->_glBegin(GL_LINES);
#endif // USE_TKMINNIE

         while(cx < _w)
         {
            sF64 cOff = off - (spd*0.5);
            sSI k;
            sF64 cw = spd / num;

            for(k = 0; k < num; k++)
            {
               // Read windowed sample
               sF32 cy = _h - ( ((_readFxn)(s, _maxSamples, _numCh, cOff) - _min) * scly ) + _offY;

#ifdef USE_TKMINNIE
               if(vtxIdx == TKMINNIE_MAX_POINTS)
               {
                  totalNumVerts += vtxIdx;
                  tkminnie_shared->sdvg_End();
                  tkminnie_shared->sdvg_BeginLinesAA(TKMINNIE_MAX_POINTS*2);
                  vtxIdx = 0u;
               }
               tkminnie_shared->sdvg_Vertex2f(cx, zeroy);
               tkminnie_shared->sdvg_Vertex2f(cx, cy);
               vtxIdx++;
#else
               tkopengl_shared->_glVertex2f(cx, zeroy);
               tkopengl_shared->_glVertex2f(cx, cy);
#endif // USE_TKMINNIE

               cOff += cw;

               if(cOff >= maxFrames)
                  cOff -= maxFrames;
            }

            off += spd;

            if(off >= maxFrames)
               off -= maxFrames;

            cx += 1.0f;
         }

#ifdef USE_TKMINNIE
         totalNumVerts += vtxIdx;
         tkminnie_shared->sdvg_End();
#else
         tkopengl_shared->_glEnd();
#endif // USE_TKMINNIE
      }

   }

#if 0
   tStart = yac_host->yacMilliSeconds() - tStart;
   Dyac_host_printf("xxx render_sample_lines: totalNumVerts=%u t=%ums\n", totalNumVerts*2u, tStart);
#else
   (void)totalNumVerts;
#endif
}

static void render_sample_gradient(sF32 *s,
                                   sUI _startOffset, sUI _numCh, sUI _numFrames, sUI _maxSamples,
                                   sF32 _w, sF32 _h,
                                   sF32 _min, sF32 _max,
                                   sUI _c32Skip,
                                   sUI _c32Rep,
                                   get_sample_fxn_t _readFxn,
                                   sF32 _offY
                                   )
{
   // yac_host->printf("xxx render_sample_gradient: #ch=%u #samples=%u w=%f h=%f\n", numCh, numFrames, _w, _h);

   sF64 off = (sF32)_startOffset;
   sF64 spd = _numFrames / _w;
   sSI num = (sSI) spd;
   sF64 maxFrames = (sF32) (_maxSamples / _numCh);

   sF32 scly  = (_h / (_max - _min));
   sF32 zeroy = _h - ( (0.0f - _min) * scly ) + _offY;

   // first sample
   sF32 lx = 0;
   sF32 ly = _h - ( (s[0] - _min) * scly ) + _offY;

   sF32 cx = 0.0;

   static const float uMax = 1.0f;

   if(spd < 1.0)
   {
      // float red   = ((_c32Skip >> 16)&255) / 255.0f;
      // float green = ((_c32Skip >>  8)&255) / 255.0f;
      // float blue  = ((_c32Skip      )&255) / 255.0f;
      // float alphaO = 1.0f;
      // if(alphaO < 0.03f)
      //    alphaO = 0.03f;
      // float alphaI = alphaO * 0.1f;
      // if(alphaI < 0.005f)
      //    alphaI = 0.005f;

#ifdef USE_TKMINNIE
      sUI vtxIdx = 0u;
      tkminnie_shared->sdvg_BeginTexturedTriangles(TKMINNIE_MAX_POINTS*6);
#else
      tkopengl_shared->_glBegin(GL_TRIANGLES);
#endif // USE_TKMINNIE

      while(cx < _w)
      {
         // Read windowed sample
         const sF32 smp = (_readFxn)(s, _maxSamples, _numCh, off);
         sF32 cy = _h - ( (smp - _min) * scly ) + _offY;
         const sF32 v = (smp < 0.0f) ? -smp : smp;

#ifdef USE_TKMINNIE
         if(vtxIdx == TKMINNIE_MAX_POINTS)
         {
            tkminnie_shared->sdvg_End();
            tkminnie_shared->sdvg_BeginTexturedTriangles(TKMINNIE_MAX_POINTS*6);
            vtxIdx = 0u;
         }
         // 1
         tkminnie_shared->sdvg_TexCoord2f(uMax, v);  // outer
         tkminnie_shared->sdvg_Vertex2f(lx, ly);  // outer

         tkminnie_shared->sdvg_TexCoord2f(uMax, v);
         tkminnie_shared->sdvg_Vertex2f(cx, cy);

         tkminnie_shared->sdvg_TexCoord2f(0.0f, v); // inner
         tkminnie_shared->sdvg_Vertex2f(cx, zeroy); // inner

         // 2
         tkminnie_shared->sdvg_TexCoord2f(uMax, v);  // outer
         tkminnie_shared->sdvg_Vertex2f(lx, ly);  // outer

         tkminnie_shared->sdvg_TexCoord2f(0.0f, v); // inner
         tkminnie_shared->sdvg_Vertex2f(cx, zeroy); // inner

         tkminnie_shared->sdvg_TexCoord2f(0.0f, v); // inner
         tkminnie_shared->sdvg_Vertex2f(lx, zeroy); // inner

         vtxIdx++;
#else
         // 1
         tkopengl_shared->_glTexCoord2f(uMax, v);  // outer
         tkopengl_shared->_glVertex2f(lx, ly);  // outer

         tkopengl_shared->_glTexCoord2f(uMax, v);
         tkopengl_shared->_glVertex2f(cx, cy);

         tkopengl_shared->_glTexCoord2f(0.0f, v); // inner
         tkopengl_shared->_glVertex2f(cx, zeroy); // inner

         // 2
         tkopengl_shared->_glTexCoord2f(uMax, v);  // outer
         tkopengl_shared->_glVertex2f(lx, ly);  // outer

         tkopengl_shared->_glTexCoord2f(0.0f, v); // inner
         tkopengl_shared->_glVertex2f(cx, zeroy); // inner

         tkopengl_shared->_glTexCoord2f(0.0f, v); // inner
         tkopengl_shared->_glVertex2f(lx, zeroy); // inner
#endif // USE_TKMINNIE

         lx = cx;
         ly = cy;
         off += spd;

         if(off >= maxFrames)
            off -= maxFrames;

         cx += 1.0f;
      }

#ifdef USE_TKMINNIE
      tkminnie_shared->sdvg_End();
#else
      tkopengl_shared->_glEnd();
#endif // USE_TKMINNIE
   }
   else
   {
      // (note) quite slow in VBO-mode on macOS (fast in immediate-mode)

      // yac_host->printf("xxx draw samples 2\n");
      // float red   = ((_c32Skip >> 16)&255) / 255.0f;
      // float green = ((_c32Skip >>  8)&255) / 255.0f;
      // float blue  = ((_c32Skip      )&255) / 255.0f;
      // float alphaO = 1.0f / num;
      // if(alphaO < 0.03f)
      //    alphaO = 0.03f;
      // float alphaI = alphaO * 0.1f;
      // if(alphaI < 0.005f)
      //    alphaI = 0.005f;

      num++;
      if(1)
      {
#ifndef NO_SHADE
         // Shade
         if(num > 128) // 128
            num = 128;
#else
         num = 1;
#endif // NO_SHADE

         float red   = ((_c32Skip >> 16)&255) / 255.0f;
         float green = ((_c32Skip >>  8)&255) / 255.0f;
         float blue  = ((_c32Skip      )&255) / 255.0f;
         float alpha = (sF32)( (((_c32Skip >> 24)&255) / 255.0f) / spd );
         // // alpha = powf(alpha, 1.7f) * 0.75f;
         if(alpha < 0.3f) //0.03f
            alpha = 0.3f;
         else if(alpha > 1.0f)
            alpha = 1.0f;

#ifdef USE_TKMINNIE
         tkminnie_shared->sdvg_SetFillColor4f(red, green, blue, alpha);
         tkminnie_shared->sdvg_BeginTexturedTriangles(TKMINNIE_MAX_POINTS*6);
         sUI vtxIdx = 0u;
#else
         tkopengl_shared->_glColor4f(red, green, blue, alpha);
         tkopengl_shared->_glBegin(GL_TRIANGLES);
#endif // USE_TKMINNIE

         while(cx < _w)
         {
            sF64 cOff = off - (spd*0.5);
            sSI k;
            sF64 cw = spd / num;

            for(k = 0; k < num; k++)
            {
               const sF32 smp = (_readFxn)(s, _maxSamples, _numCh, cOff);
               sF32 cy = _h - ( (smp - _min) * scly ) + _offY;
               const sF32 v = (smp < 0.0f) ? -smp : smp;

#ifdef USE_TKMINNIE
               if(vtxIdx == TKMINNIE_MAX_POINTS)
               {
                  tkminnie_shared->sdvg_End();
                  tkminnie_shared->sdvg_BeginTexturedTriangles(TKMINNIE_MAX_POINTS*6);
                  vtxIdx = 0u;
               }
               // 1
               tkminnie_shared->sdvg_TexCoord2f(uMax, v);       // outer
               tkminnie_shared->sdvg_Vertex2f(cx,      cy);     // outer

               tkminnie_shared->sdvg_TexCoord2f(uMax, v);       // outer
               tkminnie_shared->sdvg_Vertex2f(cx+1.0f, cy);     // outer

               tkminnie_shared->sdvg_TexCoord2f(0.0f, v);       // inner
               tkminnie_shared->sdvg_Vertex2f(cx+1.0f, zeroy);  // inner

               // 2
               tkminnie_shared->sdvg_TexCoord2f(uMax, v);       // outer
               tkminnie_shared->sdvg_Vertex2f(cx,      cy);     // outer

               tkminnie_shared->sdvg_TexCoord2f(0.0f, v);       // inner
               tkminnie_shared->sdvg_Vertex2f(cx+1.0f, zeroy);  // inner

               tkminnie_shared->sdvg_TexCoord2f(0.0f, v);       // inner
               tkminnie_shared->sdvg_Vertex2f(cx,      zeroy);  // inner

               vtxIdx++;
#else
               // 1
               tkopengl_shared->_glTexCoord2f(uMax, v);       // outer
               tkopengl_shared->_glVertex2f(cx,      cy);     // outer

               tkopengl_shared->_glTexCoord2f(uMax, v);       // outer
               tkopengl_shared->_glVertex2f(cx+1.0f, cy);     // outer

               tkopengl_shared->_glTexCoord2f(0.0f, v);       // inner
               tkopengl_shared->_glVertex2f(cx+1.0f, zeroy);  // inner

               // 2
               tkopengl_shared->_glTexCoord2f(uMax, v);       // outer
               tkopengl_shared->_glVertex2f(cx,      cy);     // outer

               tkopengl_shared->_glTexCoord2f(0.0f, v);       // inner
               tkopengl_shared->_glVertex2f(cx+1.0f, zeroy);  // inner

               tkopengl_shared->_glTexCoord2f(0.0f, v);       // inner
               tkopengl_shared->_glVertex2f(cx,      zeroy);  // inner
#endif // USE_TKMINNIE

               cOff += cw;

               if(cOff >= maxFrames)
                  cOff -= maxFrames;
            }

            off += spd;

            if(off >= maxFrames)
               off -= maxFrames;

            cx += 1.0f;
         }

#ifdef USE_TKMINNIE
         tkminnie_shared->sdvg_End();
#else
         tkopengl_shared->_glEnd();
#endif // USE_TKMINNIE
      }

   }
}

static void render_sample_lines_outer(sF32 *s,
                                      sUI _startOffset, sUI _numCh, sUI _numFrames, sUI _maxSamples,
                                      sF32 _w, sF32 _h,
                                      sF32 _min, sF32 _max,
                                      sUI _c32Skip,
                                      get_sample_fxn_t _readFxn,
                                      sF32 _offY
                                      )
{
   sF32 scly = (_h / (_max - _min));

   // yac_host->printf("xxx render_sample_lines_outer: #ch=%u #samples=%u w=%f h=%f\n", numCh, numFrames, _w, _h);

   sF64 off = (sF32)_startOffset;
   sF64 spd = _numFrames / _w;
   sSI num = (sSI) spd;
   sF64 maxFrames = (sF32) (_maxSamples / _numCh);

   sF32 cx = 0.0;

   float red   = ((_c32Skip >> 16)&255) / 255.0f;
   float green = ((_c32Skip >>  8)&255) / 255.0f;
   float blue  = ((_c32Skip      )&255) / 255.0f;
   float alpha;
   if(num > 0)
      alpha = (((_c32Skip >> 24)&255) / 255.0f) / num;
   else
      alpha = 1.0f;
   // Dyac_host_printf("xxx render_sample_lines_outer: numFrames=%u off=%f spd=%f alpha=%f num=%u\n", _numFrames, off, spd, alpha, num);
   
   if(alpha < 0.03f)
      alpha = 0.03f;

#ifdef USE_TKMINNIE
   tkminnie_shared->sdvg_SetStrokeColor4f(red, green, blue, alpha);
   tkminnie_shared->sdvg_BeginLineStripAA(TKMINNIE_MAX_POINTS);
   sF32 lx = 0.0f;
   sF32 ly = _h - ( (s[0] - _min) * scly ) + _offY;
   tkminnie_shared->sdvg_Vertex2f(lx, ly);
   sUI vtxIdx = 1u;
#else
   tkopengl_shared->_glColor4f(red, green, blue, alpha);
   tkopengl_shared->_glBegin(GL_LINES);
   tkopengl_shared->_glVertex2f(0.0f,
                                _h - ( (s[0] - _min) * scly ) + _offY
                                );
#endif // USE_TKMINNIE

   // yac_host->printf("xxx draw samples 1\n");

   while(cx < _w)
   {
      // Read windowed sample
      sF32 cy = _h - ( ((_readFxn)(s, _maxSamples, _numCh, off) - _min) * scly ) + _offY;

#ifdef USE_TKMINNIE
      if(vtxIdx == TKMINNIE_MAX_POINTS)
      {
         tkminnie_shared->sdvg_End();
         tkminnie_shared->sdvg_BeginLineStripAA(TKMINNIE_MAX_POINTS);
         vtxIdx = 1u;
         tkminnie_shared->sdvg_Vertex2f(lx, ly);
      }
      tkminnie_shared->sdvg_Vertex2f(cx, cy);
      lx = cx;
      ly = cy;
      vtxIdx++;
#else
      tkopengl_shared->_glVertex2f(cx, cy);
#endif // USE_TKMINNIE

      off += spd;

      if(off >= maxFrames)
         off -= maxFrames;

      cx += 1.0f;
   }

#ifdef USE_TKMINNIE
   tkminnie_shared->sdvg_End();
#else
   tkopengl_shared->_glEnd();
#endif // USE_TKMINNIE
}

static void render_sample_delta(sF32 *s, sUI _startOffset, sUI _numCh, sUI _numFrames, sUI _maxSamples,
                                sF32 _w, sF32 _h, sF32 _min, sF32 _max,
                                get_sample_fxn_t _readFxn,
                                sF32 _offY
                                )
{
   // (todo) REMOVE ? (or re-add as interpolation-style)

   sF32 scly = (_h / (_max - _min));

   ////yac_host->printf("xxx render_sample_delta #ch=%u #samples=%u w=%f h=%f\n", numCh, numFrames, _w, _h);

   sF64 off = (sF32)_startOffset;
   sF64 spd = _numFrames / _w;
   sF64 maxFrames = (sF32) (_maxSamples / _numCh);

   sF32 cx = 0.0f;
   sF32 lsmp = s[0];

#ifdef USE_TKMINNIE
   sUI vtxIdx = 0u;
   tkminnie_shared->sdvg_BeginLinesAA(TKMINNIE_MAX_POINTS*2);
#else
   tkopengl_shared->_glBegin(GL_LINES);
#endif // USE_TKMINNIE

   while(cx < _w)
   {
      cx += 1.0f;
      sF32 csmp = (_readFxn)(s, _maxSamples, _numCh, off);
      sF32 cy = _h - ( ((csmp - lsmp) - _min) * scly ) + _offY;

#ifdef USE_TKMINNIE
      if(vtxIdx == TKMINNIE_MAX_POINTS)
      {
         tkminnie_shared->sdvg_End();
         tkminnie_shared->sdvg_BeginLinesAA(TKMINNIE_MAX_POINTS*2);
         vtxIdx = 0u;
      }
      tkminnie_shared->sdvg_Vertex2f(cx-1, cy);
      tkminnie_shared->sdvg_Vertex2f(cx, cy);
      vtxIdx++;
#else
      tkopengl_shared->_glVertex2f(cx-1, cy);
      tkopengl_shared->_glVertex2f(cx, cy);
#endif // USE_TKMINNIE

      lsmp = csmp;
      off += spd;

      if(off >= maxFrames)
      {
         off -= maxFrames;
      }
   }

#ifdef USE_TKMINNIE
   tkminnie_shared->sdvg_End();
#else
   tkopengl_shared->_glEnd();
#endif // USE_TKMINNIE
}

void YAC_CALL tksampleedit_render_sample(YAC_Object *_data, sUI _numCh,
                                         sUI _frameOffset, sUI _numFrames,
                                         sF32 _w, sF32 _h,
                                         sF32 _min, sF32 _max,
                                         sSI _renderStyle,
                                         sSI _interpolStyle,
                                         sUI _c32Skip/*lines*/,
                                         sUI _c32Rep/*gradient*/
                                         )
{
   //
   // Render sample (range) at the given zoom level, using OpenGL.
   //

   ////yac_host->printf("xxx tksampleedit_render_sample\n");

   if(YAC_Is_FloatArray(_data))
   {
      YAC_CAST_ARG(YAC_FloatArray, a, _data);

      if( (_numCh > 0) && (_numCh <= 2/*max interleaved channels*/) )
      {
         sUI maxSamples = a->num_elements; // do not divide by numCh!

         sUI numFramesAvail = maxSamples / _numCh;

         if(_frameOffset >= numFramesAvail)
            _frameOffset -= numFramesAvail;

         // // if(_frameOffset < numFramesAvail)
         {
            if(_numFrames <= numFramesAvail)
            {
               // (note) allow wrap-around (ringbuffer)
               // if( (_frameOffset + _numFrames) <= numFramesAvail)
               {
                  // if( (_frameOffset + _numFrames) >= _frameOffset) // 32bit overflow
                  {
                     get_sample_fxn_t readFxn;

#ifdef USE_TKMINNIE
                     tkminnie_shared_resolve();
#else
                     tkopengl_shared_resolve();
#endif // USE_TKMINNIE

                     switch(_interpolStyle)
                     {
                        default:
                        case IS_SH:
                           readFxn = get_sample_sh;
                           break;

                        case IS_LINEAR:
                           readFxn = get_sample_linear;
                           break;

                        case IS_SMOOTHSTEP:
                           readFxn = get_sample_smoothstep;
                           break;

                        case IS_AVG8:
                           readFxn = get_sample_avg8;
                           break;
                     }

                     // Render samples
                     {
                        sF32 h = _h / _numCh;

                        sF32 offY = 0.0f;
                        for(sUI ch = 0u; ch < _numCh; ch++)
                        {
                           switch(_renderStyle)
                           {
                              default:
                              case RS_DOTS:
                                 render_sample_dots(a->elements + ch, _frameOffset,
                                                    _numCh, _numFrames, maxSamples,
                                                    _w, h,
                                                    _min, _max,
                                                    readFxn,
                                                    offY
                                                    );
                                 break;

                              case RS_LINES:
                                 render_sample_lines(a->elements + ch, _frameOffset,
                                                     _numCh, _numFrames, maxSamples,
                                                     _w, h,
                                                     _min, _max,
                                                     _c32Skip,
                                                     readFxn,
                                                     offY
                                                     );
                                 break;

                              case RS_DELTA:
                                 render_sample_delta(a->elements + ch, _frameOffset,
                                                     _numCh, _numFrames, maxSamples,
                                                     _w, h,
                                                     _min, _max,
                                                     readFxn,
                                                     offY
                                                     );
                                 break;

                              case RS_GRADIENT:
                                 render_sample_gradient(a->elements + ch, _frameOffset,
                                                        _numCh, _numFrames, maxSamples,
                                                        _w, h,
                                                        _min, _max,
                                                        _c32Skip,
                                                        _c32Rep,
                                                        readFxn,
                                                        offY
                                                        );
                                 break;

                              case RS_LINES_OUTER:
                                 render_sample_lines_outer(a->elements + ch, _frameOffset,
                                                           _numCh, _numFrames, maxSamples,
                                                           _w, h,
                                                           _min, _max,
                                                           _c32Skip,
                                                           readFxn,
                                                           offY
                                                           );
                                 break;
                           }

                           offY += h;
                        }

                     }

                  }
                  // else
                  // {
                  //    yac_host->printf("[---] tksampleedit_render_sample: (frameOffset + numFrames) < frameOffset ((%u+%u)=%u < %u)\n",
                  //                     _frameOffset, _numFrames, (_frameOffset+_numFrames), _frameOffset);
                  // }
               }
               // else
               // {
               //    yac_host->printf("[---] tksampleedit_render_sample: (frameOffset + numFrames) > numFramesAvail ((%u+%u)=%u >= %u)\n",
               //                     _frameOffset, _numFrames, (_frameOffset+_numFrames), numFramesAvail);
               // }
            }
            else
            {
               if(yac_host->yacGetDebugLevel() >= 2u)
                  yac_host->printf("[~~~] tksampleedit_render_sample: numFrames > numFramesAvail (%u >= %u)\n", _numFrames, numFramesAvail);
            }
         }
         // else
         // {
         //    yac_host->printf("[---] tksampleedit_render_sample: frameOffset >= numFramesAvail (%u >= %u)\n", _frameOffset, numFramesAvail);
         // }
      }

   }
}

void YAC_CALL tksampleedit_render_mono_sample_1d_8bit(YAC_Object *_byteArray, sUI _width, YAC_Object *_fa, sUI _frameOffset, sUI _numFrames, sF32 _lumScl) {
   YAC_CAST_ARG(YAC_IntArray, ba, _byteArray);
   YAC_CAST_ARG(YAC_FloatArray, fa, _fa);

   if(_numFrames > 0)
   {
      if((_frameOffset + _numFrames) <= fa->max_elements)
      {
         const sF32 *s = fa->elements + _frameOffset;
         sU8 *d = (sU8*)ba->yacArrayGetPointer();

         sF64 xStep = sF64(_width) / _numFrames;
         sF64 x = 0.0;
         // // memset((void*)ba, 0, _width); // done by caller

         // sF32 smpPerPixScl = sF32(255.0f * _width * 8.0f) / _numFrames;
         sF32 smpPerPixScl = _lumScl;//255.0f;

         for(sUI i = 0u; i < _numFrames; i++)
         {
            sUI di = sUI(x);
            sF32 f = s[i];
            f = sABS(f) * smpPerPixScl;
            sSI pixOld = sSI(d[di]);
            sSI pix = sSI(f);
            // yac_host->printf("xxx s[%u]=%f pix=%u\n", i, s[i], pix);
            if(pix > 255)
               pix = 255;
            d[di] = sU8(sMAX(pix,pixOld));

            // Next frame
            x += xStep;
         }
      }
   }
}

static void render_magnitudes_lines(const sF32 *s,
                                    sUI              _num,
                                    sUI              _numAvail,
                                    sF32             _w,
                                    sF32             _h,
                                    sF32             _zoomX,
                                    sF32             _max,
                                    sUI              _baseColor,
                                    get_sample_fxn_t _readFxn,
                                    float            _a
                                    )
{
   sF32 scly = (_h / (_max - 0.0f/*_min*/));

   ////yac_host->printf("xxx render_sample_lin #ch=%u #samples=%u w=%f h=%f\n", numCh, numFrames, _w, _h);

   sF64 cOff = (sF32)0.0f;
   sF64 spd = 1.0f;///_num / _w;

   // // first sample
   // sF32 lx = 0;
   // sF32 ly = _h - ( (s[0] - 0.0f/*_min*/) * scly );

   float alpha = 1.0f;
   alpha *= _a;
   if(alpha < 0.02f)
      alpha = 0.02f;
   // printf(" num=%d alpha=%f\n", num, alpha);
   float red   = ((_baseColor >> 16)&255) / 255.0f; // 0x00
   float green = ((_baseColor >>  8)&255) / 255.0f; // 0x11
   float blue  = ((_baseColor      )&255) / 255.0f; // 0x45

   if(alpha < 0.01f)
      alpha = 0.01f;

#ifdef USE_TKMINNIE
   tkminnie_shared->sdvg_SetStrokeColor4f(red, green, blue, alpha);
   tkminnie_shared->sdvg_BeginLineStripAA(TKMINNIE_MAX_POINTS);
   sF32 lx = -10.0f;
   sF32 ly =   0.0f;
   tkminnie_shared->sdvg_Vertex2f(lx, ly);
   sUI vtxIdx = 1u;
#else
   tkopengl_shared->_glColor4f(red, green, blue, alpha);
   tkopengl_shared->_glBegin(GL_LINE_STRIP);
   tkopengl_shared->_glVertex2f(-10.0f, 0.0f);
#endif // USE_TKMINNIE

   // sF32 zeroy = _h - ( (0.0f - 0.0f/*_min*/) * scly );

   while(cOff < _num)
   {
      sF64 cx = cOff / _num;
      cx = pow(cx, 1.0 / FFT_EXP);
      cx = -1.0*_zoomX + cx * (1.0 + 1.0*_zoomX);
      cx *= _w;

      sF32 cy = _h - ( ((_readFxn)(s, _numAvail, 1u/*_numCh*/, cOff) - 0.0f/*_min*/) * scly );

      // tkopengl_shared->_glVertex2f(cx, zeroy);
      // tkopengl_shared->_glVertex2f(cx, cy);

#ifdef USE_TKMINNIE
      if(vtxIdx == TKMINNIE_MAX_POINTS)
      {
         tkminnie_shared->sdvg_End();
         tkminnie_shared->sdvg_BeginLineStripAA(TKMINNIE_MAX_POINTS);
         tkminnie_shared->sdvg_Vertex2f(lx, ly);
         vtxIdx = 1u;
      }
      tkminnie_shared->sdvg_Vertex2f(sF32(cx), sF32(cy));
      lx = sF32(cx);
      ly = sF32(cy);
      vtxIdx++;
#else
      tkopengl_shared->_glVertex2f((sF32)cx, (sF32)cy);
#endif // USE_TKMINNIE

      cOff += spd;
   }

#ifdef USE_TKMINNIE
   tkminnie_shared->sdvg_End();
#else
   tkopengl_shared->_glEnd();
#endif // USE_TKMINNIE
}

void YAC_CALL tksampleedit_render_fft(YAC_Object *_magnitudes,
                                      sUI         _num,
                                      sF32        _w,
                                      sF32        _h,
                                      sF32        _zoomX,
                                      sF32        _max,
                                      sUI         _baseColor
                                      )
{
   //
   // Render magnitudes at the given zoom level, using OpenGL.
   //

   ////yac_host->printf("xxx tksampleedit_render_fft\n");

   if(YAC_Is_FloatArray(_magnitudes))
   {
      YAC_CAST_ARG(YAC_FloatArray, a, _magnitudes);

      sUI numAvail = a->num_elements;

      if(_num <= numAvail)
      {
         // (note) allow wrap-around (ringbuffer)

#ifdef USE_TKMINNIE
         tkminnie_shared_resolve();
#else
         tkopengl_shared_resolve();
#endif // USE_TKMINNIE

         get_sample_fxn_t readFxn = &get_sample_linear;

         // Render magnitudes
         {
            sF32 h = _h;

            render_magnitudes_lines(a->elements,
                                    _num, numAvail,
                                    _w, h,
                                    _zoomX,
                                    _max,
                                    _baseColor,
                                    readFxn,
                                    0.5f/*a*/
                                    );
         }
      }
      else
      {
         yac_host->printf("[~~~] tksampleedit_render_fft: num > numAvail (%u >= %u)\n", _num, numAvail);
      }
   }
}
