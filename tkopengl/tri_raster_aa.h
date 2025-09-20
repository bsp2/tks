/// tri_raster_aa.h
///
/// (c) 2024-2025 Bastian Spiegel <bs@tkscript.de>
///     - published under terms of the MIT LICENSE.
///
///
#ifndef TKSTRIRASTERAA_H__
#define TKSTRIRASTERAA_H__

//
// a simple (and experimental) solid-filled 2D triangle rasterizer that:
//  - renders a premultiplied image / triangle into an ARGB32 bitmap
//  - supports clipping
//  - supports sub-pixel precision
//  - supports alpha blending
//  - implements the top-left fill rule (input must be a clock-wise triangle)
//  - uses barycentric coordinates
//  - implements n-rook multisample-like anti-aliasing without the need for a resolve pass
//     (0, 2, 4, 8, or 16 samples)
//  - uses the framebuffer texture's alpha channel as a coverage buffer
//

#define Dargb(a,r,g,b) ((a << 24) | (r << 16) | (g << 8) | b)

#define Dstatic static

#ifdef AA_C

#define Dfield static

typedef struct s_TriRasterAAVec2f {
   sF32 x, y;
} TriRasterAAVec2f;

#ifndef AA_C_SKIP_IMPL
Dstatic inline void vec2f_sub(TriRasterAAVec2f *thiz, const TriRasterAAVec2f *v) {
   thiz->x -= v->x;
   thiz->y -= v->y;
}
#endif // AAFX_C_SKIP_IMPL
#define Dvec2f_sub(A,B) vec2f_sub(&(A), &(B))

typedef struct s_Bitmap {
   struct {
      sS32 x;
      sS32 y;
   } s;

   union {
      void *any;
      sU8  *u8;
      sU32 *u32;
   } data;

   sS32 stride;
} Bitmap;

#define DIsTopLeftEdge(A,B) loc_is_topleft_edge(&(A), &(B))

#else
// C++
#define Dfield
struct TriRasterAAVec2f {
   sF32 x, y;

   void sub(const TriRasterAAVec2f &v) {
      x -= v.x;
      y -= v.y;
   }
};

#define Dvec2f_sub(A,B) (A).sub(B)

#define DIsTopLeftEdge(A,B) IsTopLeftEdge((A), (B))

#endif // AA_C

#ifndef AA_C_SKIP_IMPL

#ifndef AA_C
struct TriRasterAA {
#endif // AA_C

   Dfield TriRasterAAVec2f va;
   Dfield TriRasterAAVec2f vb;
   Dfield TriRasterAAVec2f vc;

   Dfield sF32 v0x; // = vb.x - va.x;
   Dfield sF32 v0y; // = vb.y - va.y;

   Dfield sF32 v1x; // = vc.x - va.x;
   Dfield sF32 v1y; // = vc.y - va.y;

   Dfield sF32 inv_den; // = 1.0f / den;

   // barycentric result of last isWithin() call
   Dfield sF32 a; // = (v2x * v1y - v1x_mul_v2y) * inv_den;
   Dfield sF32 b; // = (v0x_mul_v2y - v2x * v0y) * inv_den;
   Dfield sF32 c; // = 1 - a - b;

   Dfield sF32 fr_bias_a;  // top-left fill-rule bias
   Dfield sF32 fr_bias_b;
   Dfield sF32 fr_bias_c;

   Dfield sF32 fr_bias_a_aa;
   Dfield sF32 fr_bias_b_aa;
   Dfield sF32 fr_bias_c_aa;

   sBool isWithin(const sF32 x, const sF32 y) {

      const sF32 v2x = x - va.x;
      const sF32 v2y = y - va.y;

      a = (v2x * v1y - v1x * v2y) * inv_den;
      b = (v0x * v2y - v2x * v0y) * inv_den;
      c = 1.0f - a - b;

#if 0
      if( sSI(y) == 4 && sSI(x) == 5 )
      {
         float mulA = (v2x * v1y - v1x * v2y);
         float mulB = (v0x * v2y - v2x * v0y);

         printf("xxx isWithin(x=%f y=%f) => mulA=%f mulB=%f a=%f b=%f c=%f inv_den=%f v0=(%f;%f) v1=(%f;%f) v2=(%f;%f)\n", x, y, mulA, mulB, a, b, c, inv_den, v0x, v0y, v1x, v1y, v2x, v2y);
      }
#endif

      return ( a >= fr_bias_a && b >= fr_bias_b && c >= fr_bias_c );
   }

   sBool isWithinAA(const sF32 x, const sF32 y)
#ifndef AA_C
      const
#endif // AA_C
      {

      const sF32 v2x = x - va.x;
      const sF32 v2y = y - va.y;

      const sF32 a = (v2x * v1y - v1x * v2y) * inv_den;
      const sF32 b = (v0x * v2y - v2x * v0y) * inv_den;
      const sF32 c = 1.0f - a - b;

      return ( a >= fr_bias_a_aa && b >= fr_bias_b_aa && c >= fr_bias_c_aa );
   }

#if 0
   static sU32 BlendSrcOver(const sUI d32, const sU8 _coverage, const sUI c32) {

      sUI ad  = (d32 >> 24) & 255u;
      sUI cdr = (d32 >> 16) & 255u;
      sUI cdg = (d32 >>  8) & 255u;
      sUI cdb = (d32      ) & 255u;

      sUI as  = (c32 >> 24) & 255u;
      sUI csr = (c32 >> 16) & 255u;
      sUI csg = (c32 >>  8) & 255u;
      sUI csb = (c32      ) & 255u;

      as += (as & 1u);
      as = (_coverage * as) >> 8;
      as += (as & 1u);

      cdr = cdr + (((csr - cdr) * as) >> 8);
      cdg = cdg + (((csg - cdg) * as) >> 8);
      cdb = cdb + (((csb - cdb) * as) >> 8);
      ad = 255;

      return Dargb(ad, cdr, cdg, cdb);
   }
#endif

   static inline sU32 BlendCoverage(const sU32 d32, const sU8 _coverage, const sU32 c32) {
      sU16 as  = (c32 >> 24) & 255u;
      as += (as & 1u);
      as = (_coverage * as) >> 8;
      as += (as & 1u);
      sU16 ad  = (d32 >> 24) & 255u;
      sU16 a = ((255u - ad) * as) >> 8;
      ad = a + ad;
      a += (a & 1u);
#ifdef AA_C
      #define rbMask 0x00FF00FFu
      #define gMask  0x0000FF00u
#else
      static const sU32 rbMask = 0x00FF00FFu;
      static const sU32 gMask  = 0x0000FF00u;
#endif // AA_C
      const sU32 cdrb = (d32 & rbMask);
      const sU32 csrb = (c32 & rbMask);
#if 1
      const sU16 cdg = (d32 & gMask);
      const sU16 csg = (c32 & gMask);
      return
         (ad << 24)
         | ( (cdg + (((csg * a) >> 8))) & gMask )
         | ((cdrb + ((csrb * a) >> 8)) & rbMask)
         ;
#else
      const sU16 cdg = (d32 >> 8) & 255u;
      const sU16 csg = (c32 >> 8) & 255u;
      return
         (ad << 24)
         | ( (cdg + ((csg * a) >> 8)) << 8 )
         | ((cdrb + ((csrb * a) >> 8)) & rbMask)
         ;
#endif
   }

#ifdef AA_C
Dstatic inline sBool loc_is_topleft_edge(const TriRasterAAVec2f *a, const TriRasterAAVec2f *b) {
   if(b->y >= a->y)
      return (b->x >= a->x);
   return (b->x >= a->x);
}
#else
   static inline sBool IsTopLeftEdge(const TriRasterAAVec2f &a, const TriRasterAAVec2f &b) {
      if(b.y >= a.y)
         return (b.x >= a.x);
      return (b.x >= a.x);
   }
#endif // AA_C

   void renderTriToBitmap(
#ifdef AA_C
      const TriRasterAAVec2f *_v1,
      const TriRasterAAVec2f *_v2,
      const TriRasterAAVec2f *_v3,
#else
      const TriRasterAAVec2f &_v1,
      const TriRasterAAVec2f &_v2,
      const TriRasterAAVec2f &_v3,
#endif // AA_C
      const sU32 _c32,
      Bitmap *_fb,
      const sUI _numSamples,
      const sUI _aaFixMode,
      const sF32 _aaBias
                          ) {
      // (note) 'numSamples' must be either 0 (AA off), or 2 / 4 / 8 / 16
      // (note) 'bFixSharedEdge' slightly enlarges the triangle to cover the seams at (inner) shared edges
      // (note) 'aaBias' should be set to (-0.01f * (160.0f / _fb->s.x))

#ifdef AA_C
      va = *_v1;
      vb = *_v2;
      vc = *_v3;
#else
      va = _v1;
      vb = _v2;
      vc = _v3;
#endif // AA_C

      TriRasterAAVec2f bbMin;
      bbMin.x = (sF32)((sSI)(sMIN(va.x, sMIN(vb.x, vc.x))));
      bbMin.y = (sF32)((sSI)(sMIN(va.y, sMIN(vb.y, vc.y))));

      const sSI bbMaxX = (sSI)(sMAX(va.x, sMAX(vb.x, vc.x)) + 0.5f);
      const sSI bbMaxY = (sSI)(sMAX(va.y, sMAX(vb.y, vc.y)) + 0.5f);

      Dvec2f_sub(va, bbMin);
      Dvec2f_sub(vb, bbMin);
      Dvec2f_sub(vc, bbMin);

      sSI fb_off_x = (sSI)bbMin.x;
      sSI fb_off_y = (sSI)bbMin.y;

      sSI bbSizeX = bbMaxX - fb_off_x;
      sSI bbSizeY = bbMaxY - fb_off_y;

      const sBool bAA = (0u != _numSamples);
      sSI geo_shift_x;
      sSI geo_shift_y;

      if(bAA)
      {
         fb_off_x -= 1;
         fb_off_y -= 1;
         geo_shift_x = -1;
         geo_shift_y = -1;
         bbSizeX += 2;
         bbSizeY += 2;
      }
      else
      {
         geo_shift_x = 0;
         geo_shift_y = 0;
      }

      if(fb_off_x < 0)
      {
         bbSizeX = sMAX(0, bbSizeX + fb_off_x);
         geo_shift_x -= fb_off_x;
         fb_off_x = 0;
      }

      if(fb_off_y < 0)
      {
         bbSizeY = sMAX(0, bbSizeY + fb_off_y);
         geo_shift_y -= fb_off_y;
         fb_off_y = 0;
      }

      if( fb_off_x + bbSizeX > _fb->s.x )
         bbSizeX = _fb->s.x - fb_off_x;

      if( fb_off_y + bbSizeY > _fb->s.y )
         bbSizeY = _fb->s.y - fb_off_y;

      if(bAA)
      {
         fr_bias_a = 0.0f;
         fr_bias_b = 0.0f;
         fr_bias_c = 0.0f;

         if(_aaFixMode)
         {
            const sF32 biasShared = _aaBias; // e.g. -0.01f * (160.0f / _fb->s.x);
            fr_bias_a_aa = biasShared;
            fr_bias_b_aa = biasShared;
            fr_bias_c_aa = biasShared;
         }
         else
         {
            fr_bias_a_aa = 0.0f;
            fr_bias_b_aa = 0.0f;
            fr_bias_c_aa = 0.0f;
         }
      }
      else
      {
         // AA off
         static const sF32 biasBR = 0.0000001f;
         fr_bias_a = DIsTopLeftEdge(vc, va) ? 0.0f : biasBR;
         fr_bias_b = DIsTopLeftEdge(va, vb) ? 0.0f : biasBR;
         fr_bias_c = DIsTopLeftEdge(vb, vc) ? 0.0f : biasBR;

         fr_bias_a_aa = 0.0f;  // avoid compiler warning (vars are unused)
         fr_bias_b_aa = 0.0f;
         fr_bias_c_aa = 0.0f;
      }

      v0x = vb.x - va.x;
      v0y = vb.y - va.y;

      v1x = vc.x - va.x;
      v1y = vc.y - va.y;

      const sF32 den = v0x * v1y - v1x * v0y;
      /* printf("xxx den=%f\n", den); */
      if(0.0f == den)
         return;
      inv_den = 1.0f / den;

      /* printf("xxx v0x=%f v1y=%f v1x=%f v0y=%f den=%f inv_den=%f\n", v0x, v1y, v1x, v0y, den, inv_den); */

      const sF32 rgaaAdd = 1.0f / (_numSamples + 1u);

      const sS32 dpitch32 = _fb->stride >> 2;
      sU32 *d = _fb->data.u32 + fb_off_y * dpitch32;

      sSI y = geo_shift_y;
      for(sSI iy = 0; iy < bbSizeY; iy++)
      {
         const sF32 cy = y + 0.5f;
         sSI x = geo_shift_x;
         for(sSI ix = 0; ix < bbSizeX; ix++)
         {
            const sSI dx = x + fb_off_x - geo_shift_x;
            const sU32 d32 = d[dx];
            if((d32 & 0xFF000000u) != 0xFF000000u)
            {
               const sBool iWithin = isWithin(x + 0.5f, cy);
#if 0
               sF32 curA = mathClampf(a, 0, 1);  // save barycentrics, e.g. for texture fetch
               sF32 curB = mathClampf(b, 0, 1);
               sF32 curC = mathClampf(c, 0, 1);
#endif

               if( (2u == _aaFixMode) ? !iWithin : bAA )
               {
                  sF32 aa = 0.0f;

                  if(iWithin)
                     aa += rgaaAdd;

                  switch(_numSamples)
                  {
                     case 2:
                        if(isWithinAA(x + 0.75f, y + 0.75f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.25f, y + 0.25f)) aa += rgaaAdd;
                        break;

                     default:
                     case 4:
                        if(isWithinAA(x + 0.375f, y + 0.125f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.875f, y + 0.375f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.125f, y + 0.625f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.625f, y + 0.875f)) aa += rgaaAdd;
                        break;

                     case 8:
                        if(isWithinAA(x + 0.5625f, y + 0.3125f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.4375f, y + 0.6875f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.8125f, y + 0.5625f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.3125f, y + 0.1875f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.1875f, y + 0.8125f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.0625f, y + 0.4375f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.6875f, y + 0.9375f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.9375f, y + 0.0625f)) aa += rgaaAdd;
                        break;

                     case 16:
                        // (note) ~7% faster than using __builtin_popcount()
                        if(isWithinAA(x + 0.5625f, y + 0.5625f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.4375f, y + 0.3125f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.3125f, y + 0.625f))  aa += rgaaAdd;
                        if(isWithinAA(x + 0.75f,   y + 0.4375f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.1875f, y + 0.375f))  aa += rgaaAdd;
                        if(isWithinAA(x + 0.625f,  y + 0.8125f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.8125f, y + 0.6875f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.6875f, y + 0.1875f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.375f,  y + 0.875f))  aa += rgaaAdd;
                        if(isWithinAA(x + 0.5f,    y + 0.0625f)) aa += rgaaAdd;
                        if(isWithinAA(x + 0.25f,   y + 0.125f))  aa += rgaaAdd;
                        if(isWithinAA(x + 0.125f,  y + 0.75f))   aa += rgaaAdd;
                        if(isWithinAA(x + 0.0f,    y + 0.5f))    aa += rgaaAdd;
                        if(isWithinAA(x + 0.9375f, y + 0.25f))   aa += rgaaAdd;
                        if(isWithinAA(x + 0.875f,  y + 0.25f))   aa += rgaaAdd;
                        if(isWithinAA(x + 0.0625f, y + 0.0f))    aa += rgaaAdd;
                        break;
                  }

                  if(aa > 0.0039f)
                  {
                     const sU8 a8 = (sU8)(aa * 255.0f);
                     d[dx] = BlendCoverage(d32, a8/*coverage*/, _c32);
                  }
               }
               else if(iWithin)
               {
                  d[dx] = BlendCoverage(d32, 255u/*coverage*/, _c32);
               }

            } // if !full coverage

            // Next pixel
            x++;
         }

         // Next scanline
         d += dpitch32;
         y++;
      }
   }

#ifndef AA_C
};
#endif // AA_C

#undef Dargb
#undef DIsTopLeftEdge
#undef Dstatic
#undef Dfield
#undef Dvec2f_sub

#endif // AA_C_SKIP_IMPL

#endif // TKSTRIRASTERAA_H__
