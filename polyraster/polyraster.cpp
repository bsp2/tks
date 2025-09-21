#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
YAC_Host *yac_host;

#include "polyraster.h"

#include "ying_polyraster.h"
#include "ying_polyraster.cpp"

// Implement standard exception ID variables (see yac.h)
Dyac_std_exid_impl;


static void loc_calc_bbox(const sF32 *verts, sUI numVerts, sF32 &minX, sF32 &minY, sF32 &maxX, sF32 &maxY) {
   minX =  9999999.0f;
   minY =  9999999.0f;
   maxX = -9999999.0f;
   maxY = -9999999.0f;
   sUI k = 0u;
   for(sUI i = 0u; i < numVerts; i++, k += 2u)
   {
      const sF32 x = verts[k + 0u];
      const sF32 y = verts[k + 1u];
      if(x < minX)
         minX = x;
      if(x > maxX)
         maxX = x;
      if(y < minY)
         minY = y;
      if(y > maxY)
         maxY = y;
   }
}

static void loc_edge(sU8 *pixels, sUI w, sUI h, sSI x1, sSI y1, sSI x2, sSI y2) {
   // TODO: optimize for fixpoint

   // // Dyac_host_printf("loc_edge: 1=(%d; %d) 2=(%d; %d)\n", x1, y1, x2, y2);
   if(y2 < y1)
   {
      sSI t = y2;
      y2 = y1;
      y1 = t;
      t = x2;
      x2 = x1;
      x1 = t;
   }

   sUI numLines = (sUI) abs(y2 - y1);
   if(0u == numLines)
      return;
   sF32 dx = (x2 - x1) / ((sF32)numLines);
   // // Dyac_host_printf("numLines=%d dx=%f y1=%d x1=%d\n", numLines, dx, y1, x1);

//    sUI foff = ((sUI)y1) * w + (sUI)x1;
//    pixels[foff] = 128;

//    sUI loff = ((sUI)y2) * w + (sUI)x2;
//    pixels[loff] = 192;

   sF32 x = x1;
   pixels += y1 * w;
   for(sUI i = 0u; i < numLines; i++)
   {
      // // Dyac_host_printf("x =%f = %d\n", x, (sSI)x);
      pixels[((sSI)x)] ^= 255;
      x += dx;
      pixels += w;
   }
}

static void loc_xor_fill(sU8 *pixels, sUI w, sUI h) {
   sUI k = 0u;
   for(sUI y = 0u; y < h; y++)
   {
      sU8 c = 0x00u;
      for(sUI x = 0u; x < w; x++)
      {
         sU8 v = pixels[k];
         c ^= v;
         pixels[k++] = c;
      }
   }
}

static void loc_edges(sU8 *pixels, sUI w, sUI h,
                      const sF32 *verts, sUI numVerts
                      ) {

   // Render edge outlines
   sSI fgx = (sSI) verts[0];
   sSI fgy = (sSI) verts[1];
   sSI lgx = fgx;
   sSI lgy = fgy;
   sUI k = 2u;
   for(sUI i = 1u; i < numVerts; i++, k += 2u)
   {
      sSI cgx = (sSI) verts[k + 0u];
      sSI cgy = (sSI) verts[k + 1u];
      loc_edge(pixels, w, h, lgx, lgy, cgx, cgy);
      lgx = cgx;
      lgy = cgy;
   }
   loc_edge(pixels, w, h, lgx, lgy, fgx, fgy);
}

static void loc_polyraster(sU8 *pixels, sUI w, sUI h, const sF32 *verts, sUI numVerts) {

#if 0
   sF32 minX, minY;
   sF32 maxX, maxY;
   loc_calc_bbox(verts, numVerts, minX, minY, maxX, maxY);
#endif

   loc_edges(pixels, w, h, verts, numVerts);

   loc_xor_fill(pixels, w, h);
}

void YAC_CALL polyraster_calc_bbox(YAC_Object *_fa,
                                   YAC_Object *_retMinX,
                                   YAC_Object *_retMinY,
                                   YAC_Object *_retMaxX,
                                   YAC_Object *_retMaxY
                                   ) {
   sF32 minX = 0.0f, minY = 0.0f;
   sF32 maxX = 0.0f, maxY = 0.0f;

   if(YAC_Is_FloatArray(_fa))
   {
      YAC_CAST_ARG(YAC_FloatArray, fa, _fa);

      loc_calc_bbox((const sF32*)fa->elements, fa->num_elements / 2u, minX, minY, maxX, maxY);
   }

   if(YAC_VALID(_retMinX))
      _retMinX->yacValueOfI(minX);

   if(YAC_VALID(_retMinY))
      _retMinY->yacValueOfI(minY);

   if(YAC_VALID(_retMaxX))
      _retMaxX->yacValueOfI(maxX);

   if(YAC_VALID(_retMaxY))
      _retMaxY->yacValueOfI(maxY);
}

void YAC_CALL polyraster_edges(YAC_Object *_tex, YAC_Object *_fa) {
   if(YAC_VALID(_tex))
   {
      if(1u == _tex->yacArrayGetElementByteSize())
      {
         sU8 *pixels = (sU8*) _tex->yacArrayGetPointer();

         const sUI w = _tex->yacArrayGetWidth();
         const sUI h = _tex->yacArrayGetHeight();

         if(YAC_Is_FloatArray(_fa))
         {
            YAC_CAST_ARG(YAC_FloatArray, fa, _fa);

            loc_edges(pixels, w, h,
                      (const sF32*)fa->elements,
                      fa->num_elements / 2u
                      );
         }
      }
   }
}

void YAC_CALL polyraster_fill(YAC_Object *_tex) {
   if(YAC_VALID(_tex))
   {
      if(1u == _tex->yacArrayGetElementByteSize())
      {
         sU8 *pixels = (sU8*) _tex->yacArrayGetPointer();

         const sUI w = _tex->yacArrayGetWidth();
         const sUI h = _tex->yacArrayGetHeight();

         loc_xor_fill(pixels, w, h);
      }
   }
}

void YAC_CALL polyraster(YAC_Object *_tex, YAC_Object *_fa) {
   if(YAC_VALID(_tex))
   {
      if(1u == _tex->yacArrayGetElementByteSize())
      {
         sU8 *pixels = (sU8*) _tex->yacArrayGetPointer();

         const sUI w = _tex->yacArrayGetWidth();
         const sUI h = _tex->yacArrayGetHeight();

         if(YAC_Is_FloatArray(_fa))
         {
            YAC_CAST_ARG(YAC_FloatArray, fa, _fa);

            loc_polyraster(pixels, w, h, (sF32*)fa->yacArrayGetPointer(), fa->num_elements / 2u);
         }
      }
   }
}


// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

   // Resolve "standard" exception IDs
   Dyac_std_exid_resolve;

   YAC_Init_polyraster(_host);
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_polyraster(_host);
}


#include <yac_host.cpp>
