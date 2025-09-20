/// tri_raster_aa_fx_opt.h
///
/// (c) 2024 Bastian Spiegel <bs@tkscript.de>
///     - published under terms of the GNU general public license (GPL)
///
///
#ifndef TKSTRIRASTERAAFX_H__
#define TKSTRIRASTERAAFX_H__

// AA only. optimized for 4 samples and 320x240. 4 instead of 6 bit fractional coord bits.

// (note) 68020 has muls.l

// (note) define AAFX_C to switch to "C" mode (skip classes, members become static vars)

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

// (note) the fixed point version is ~12% slower than the float version
// (note) it's meant for machines that do not have an FPU.

// (note) known issue: in non-AA mode at low resolutions (<640x480) the geometry may have (a few) holes (when AAFX_USE_FLOAT_INV=0)
// (note) known issue: *very* small triangles may not be rendered correctly (imaging) (when AAFX_USE_FLOAT_INV=0)

// set to 1 to use float arithmetic for 'inv_den' multiplication (fixes holes and tiny-triangle-imaging)
//  (note) when enabled, code will include 4 32bit int*FP multiplications and 1 32bit FP/int division
//  (note) meant for targets which have an FPU but where fixed point math is still faster in general

typedef sS16 sFix4;
typedef sS16 sFix5;
typedef sS32 sFix;
typedef sS32 sFix26;

#define Dfixmul0(A,B) (((A) * (B))     )
#define Dfixmul1(A,B) (((A) * (B)) >> 1)
#define Dfixmul2(A,B) (((A) * (B)) >> 2)
#define Dfixmul4(A,B) (((A) * (B)) >> 4)
#define Dfixmul5(A,B) (((A) * (B)) >> 5)
#define Dfixmul6(A,B) (((A) * (B)) >> 6)
#define Dfixmul18(A,B) (((A) * (B)) >> 18)
#define Dfixmul8(A,B) (((A) * (B)) >> 8)
#define Dfixmul10(A,B) (((A) * (B)) >> 10)
#define Dfixmul12(A,B) (((A) * (B)) >> 12)
#define Dfixmul16(A,B) (((A) * (B)) >> 16)
#define Dfixmul20(A,B) (((A) * (B)) >> 20)
#define Dfixmul22(A,B) (((A) * (B)) >> 22)
#define Dfixmul23(A,B) (((A) * (B)) >> 23)
#define Dfixmul24(A,B) (((A) * (B)) >> 24)
#define Dfixmul26(A,B) (((A) * (B)) >> 26)
#define Dfixmul28(A,B28) (((A) * (B28)) >> 28)
#define Df2x(A) ((sFix)((A) * 65536.0f))
#define Df2x4(A) ((sFix)((A) * 16.0f))
#define Df2x5(A) ((sFix)((A) * 32.0f))
#define Df2x6(A) ((sFix)((A) * 64.0f))
#define Df2x8(A) ((sFix)((A) * 256.0f))
#define Df2x10(A) ((sFix)((A) * 1024.0f))
#define Df2x12(A) ((sFix)((A) * 4096.0f))
#define Dx2f(A) ((sF32)((A) * (1.0f / 65536.0f)))
#define Dx2f6(A) ((sF32)((A) * (1.0f / 64.0f)))


#ifdef AAFX_C

#ifdef AAFX_C_NOSTATIC
#define Dstatic
#define Dfield 
#else
#define Dstatic static
#define Dfield static
#endif // AAFX_C_NOSTATIC


typedef struct s_TriRasterAAVec2x {
   sFix5 x, y;  // 5 fractional bits
} TriRasterAAVec2x;

#ifndef AAFX_C_SKIP_IMPL
Dstatic inline void vec2x_sub(TriRasterAAVec2x *thiz, const TriRasterAAVec2x *v) {
   thiz->x -= v->x;
   thiz->y -= v->y;
}
#endif // AAFX_C_SKIP_IMPL
#define Dvec2x_sub(A,B) vec2x_sub(&(A), &(B))

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
#define Dstatic static
#define Dfield

struct TriRasterAAVec2x {
   sFix5 x, y;

   void sub(const TriRasterAAVec2x &v) {
      x -= v.x;
      y -= v.y;
   }
};

#define Dvec2x_sub(A,B) (A).sub(B)

#define DIsTopLeftEdge(A,B) IsTopLeftEdge(A, B)

#endif // AAFX_C


typedef union u_ui {
   sFix s;
   sU32 u;
} t_ui;

#ifndef AAFX_C_SKIP_IMPL

#ifdef AMIGA
static inline int mulsw(short b, short c) {
   int a;
   asm("mulsw %2,%0":"=d"(a): "0"(c),"d"(b): "cc");
   return a;
}
#else
static inline int mulsw(short b, short c) {
   return b * c;
}
#endif // AMIGA

#ifndef AAFX_C
struct TriRasterAA_fx {
#endif // AAFX_C
   Dfield TriRasterAAVec2x va;
   Dfield TriRasterAAVec2x vb;
   Dfield TriRasterAAVec2x vc;

#ifdef AAFX_C
   #define mask_fx 0xffff0000u
#else
   static const sU32 mask_fx = 0xffff0000u;
#endif // AAFX_C

   Dfield sFix5 v0x; // = vb.x - va.x;
   Dfield sFix5 v0y; // = vb.y - va.y;

   Dfield sFix5 v1x; // = vc.x - va.x;
   Dfield sFix5 v1y; // = vc.y - va.y;

   Dfield sFix26 inv_den26;

   // barycentric result of last isWithin() call
   Dfield t_ui a; // = (v2x * v1y - v1x_mul_v2y) * inv_den;
   Dfield t_ui b; // = (v0x_mul_v2y - v2x * v0y) * inv_den;
   Dfield t_ui c; // = 1 - a - b;

   Dfield sFix fr_bias_a;  // top-left fill-rule bias
   Dfield sFix fr_bias_b;
   Dfield sFix fr_bias_c;  // 65536 - bias

   Dfield sFix fr_bias_a_aa;
   Dfield sFix fr_bias_b_aa;
   Dfield sFix fr_bias_c_aa;  // 65536 - bia

   /* Dstatic const sU16 rgaa_add_tbl[17]; */

   sU32 isNotWithin(const sFix x, const sFix y) {

      /* printf("xxx isWithin(x=%f y=%f)\n", x/32.0f, y/32.0f); */
      sFix5 v2x = (x - va.x);
      sFix5 v2y = (y - va.y);

      // very tiny triangles not rendered correctly. good enough for "demo" purposes, though. should enable AA.
#if 1
      a.s = (sFix)( (((long long)((Dfixmul5(v2x, v1y) - Dfixmul5(v1x, v2y)))) * inv_den26)>>15 );
      b.s = (sFix)( (((long long)((Dfixmul5(v0x, v2y) - Dfixmul5(v2x, v0y)))) * inv_den26)>>15 );
#else
      a.s = (sFix)( (((sS32)(( (sS32) (mulsw(v2x, v1y) - mulsw(v1x, v2y)) >> 9 ))) * inv_den26)>>11 );
      b.s = (sFix)( (((sS32)(( (sS32) (mulsw(v0x, v2y) - mulsw(v2x, v0y)) >> 9 ))) * inv_den26)>>11 );
#endif

      c.s = fr_bias_c - a.s - b.s;

      t_ui ta; ta.s = a.s - fr_bias_a;
      t_ui tb; tb.s = b.s - fr_bias_b;

      sU32 uNotWithin = (ta.u & mask_fx) | (tb.u & mask_fx) | (c.u & mask_fx);
      return uNotWithin;
   }

   sU32 isNotWithinAA(const sFix x, const sFix y)
#ifndef AAFX_C
      const
#endif // AAFX_C
      {

      sFix5 v2x = (x - va.x);
      sFix5 v2y = (y - va.y);

#if 1
      t_ui a; a.s = (sFix)( (((long long)((Dfixmul5(v2x, v1y) - Dfixmul5(v1x, v2y)))) * inv_den26)>>15 );
      t_ui b; b.s = (sFix)( (((long long)((Dfixmul5(v0x, v2y) - Dfixmul5(v2x, v0y)))) * inv_den26)>>15 );
#else
      t_ui a; a.s = (sFix)( (sS32) (((sS32)(( (mulsw(v2x, v1y) - mulsw(v1x, v2y)) >> 9 ))) * inv_den26)>>11  );
      t_ui b; b.s = (sFix)( (sS32) (((sS32)(( (mulsw(v0x, v2y) - mulsw(v2x, v0y)) >> 9 ))) * inv_den26)>>11  );
#endif
      t_ui c; c.s = fr_bias_c_aa - a.s - b.s;

      a.s -= fr_bias_a_aa;
      b.s -= fr_bias_b_aa;

      return (a.u & mask_fx) | (b.u & mask_fx) | (c.u & mask_fx);
   }

   Dstatic inline sU32 BlendCoverage(const sU32 d32, const sU8 _coverage, const sU32 c32) {
      sU16 as  = (c32 >> 24) & 255u;
      as += (as & 1u);
      as = (_coverage * as) >> 8;
      as += (as & 1u);
      sU16 ad  = (d32 >> 24) & 255u;
      sU16 a = ((255u - ad) * as) >> 8;
      ad = a + ad;
      a += (a & 1u);
#ifdef AAFX_C
      #define rbMask 0x00FF00FFu
      #define gMask  0x0000FF00u
#else
      static const sU32 rbMask = 0x00FF00FFu;
      static const sU32 gMask  = 0x0000FF00u;
#endif // AAFX_C_SKIP_IMPL
      const sU32 cdrb = (d32 & rbMask);
      const sU32 csrb = (c32 & rbMask);
      const sU16 cdg = (d32 & gMask);
      const sU16 csg = (c32 & gMask);
      return
         (ad << 24)
         | ( (cdg + (((csg * a) >> 8))) & gMask )
         | ((cdrb + ((csrb * a) >> 8)) & rbMask)
         ;
   }

#ifdef AAFX_C
Dstatic inline sBool loc_is_topleft_edge(const TriRasterAAVec2x *a, const TriRasterAAVec2x *b) {
   if(b->y >= a->y)
      return (b->x >= a->x);
   return (b->x >= a->x);
}
#else
   static inline sBool IsTopLeftEdge(const TriRasterAAVec2x &a, const TriRasterAAVec2x &b) {
      if(b.y >= a.y)
         return (b.x >= a.x);
      return (b.x >= a.x);
   }
#endif // AAFX_C

   void renderTriToBitmap(
#ifdef AAFX_C
      const TriRasterAAVec2x *_v1,  // 5 fractional bits
      const TriRasterAAVec2x *_v2,
      const TriRasterAAVec2x *_v3,
#else
      const TriRasterAAVec2x &_v1,  // 5 fractional bits
      const TriRasterAAVec2x &_v2,
      const TriRasterAAVec2x &_v3,
#endif // AAFX_C
      const sU32 _c32,
      Bitmap *_fb,
      const sU32 _numSamples,
      const sU32 _aaFixMode,
      const sFix _aaBias
                          ) {
      // (note) 'numSamples' must be either 0 (AA off), or 2 / 4 / 8 / 16
      // (note) 'aaFixMode' slightly enlarges the triangle to cover the seams at (inner) shared edges (2=outer pixels only)
      // (note) 'aaBias' should be set to Df2x(-0.01f * (160.0f / _fb->s.x))  /  Dfixmul16(-655, ((160*65536) / _fb->s.x));

#ifdef AAFX_C
      va = *_v1;
      vb = *_v2;
      vc = *_v3;
#else
      va = _v1;
      vb = _v2;
      vc = _v3;
#endif // AAFX_C

      TriRasterAAVec2x bbMin;
      bbMin.x = sMIN(va.x, sMIN(vb.x, vc.x)) & ~((1 << 5)-1);
      bbMin.y = sMIN(va.y, sMIN(vb.y, vc.y)) & ~((1 << 5)-1);

      const sS16 bbMaxX = (sMAX(va.x, sMAX(vb.x, vc.x)) + 16) >> 5;
      const sS16 bbMaxY = (sMAX(va.y, sMAX(vb.y, vc.y)) + 16) >> 5;

      Dvec2x_sub(va, bbMin);
      Dvec2x_sub(vb, bbMin);
      Dvec2x_sub(vc, bbMin);

      sS16 fb_off_x = bbMin.x >> 5;
      sS16 fb_off_y = bbMin.y >> 5;

      sS16 bbSizeX = bbMaxX - fb_off_x;
      sS16 bbSizeY = bbMaxY - fb_off_y;

      sS16 geo_shift_x;
      sS16 geo_shift_y;

      /* if(bAA) */
      {
         fb_off_x -= 1;
         fb_off_y -= 1;
         geo_shift_x = -1;
         geo_shift_y = -1;
         bbSizeX += 2;
         bbSizeY += 2;
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

      /* printf("xxx bb=(%d; %d; %d; %d)\n", fb_off_x, fb_off_y, bbSizeX, bbSizeY); */

      /* if(bAA) */
      {
         fr_bias_a = 0;
         fr_bias_b = 0;
         fr_bias_c = 65536 - 0;

         const sFix biasShared = _aaBias; // Dfixmul16(-655, ((160*65536) / _fb->s.x));
         fr_bias_a_aa = biasShared;
         fr_bias_b_aa = biasShared;
         fr_bias_c_aa = 65536 - biasShared;
      }

      v0x = vb.x - va.x;
      v0y = vb.y - va.y;

      v1x = vc.x - va.x;
      v1y = vc.y - va.y;

      /* const sFix den2 = ((sS16)Dfixmul8(v0x, v1y)) - ((sS16)Dfixmul8(v1x, v0y));  // 0 fractional bits */
      /* const sFix den2 = (mulsw(v0x, v1y) - mulsw(v1x, v0y)) >> 10;  // 0 fractional bits */
      const sFix den = Dfixmul10(v0x, v1y) - Dfixmul10(v1x, v0y);  // 0 fractional bits
      /* const sFix den = (Dfixmul5(v0x, v1y) - Dfixmul5(v1x, v0y)) >> 5;  // 0 fractional bits */
      /* if(den != den2) */
      /*    printf("xxx v0=(%d;%d) v1=(%d;%d) den=0x%08x den2=0x%08x\n", v0x, v0y, v1x, v1y, den, den2); */
      /* printf("xxx v0=(%d;%d) v1=(%d;%d) den=0x%08x den2=0x%08x v0x*v1y_fixmul8=%d v0x*v1y_mulsw=%d\n", v0x, v0y, v1x, v1y, den, den2, ((sS16)Dfixmul8(v0x,v1y)), mulsw(v0x>>4, v1y>>4)); */
      /* printf("xxx v0=(%d;%d) v1=(%d;%d) den=0x%08x den2=0x%08x v0x*v1y_fixmul8=%d v0x*v1y_mulsw=%d\n", v0x, v0y, v1x, v1y, den, den2, ((sS16)((v0x*v1y)>>8)), (mulsw(v0x, v1y)>>8)); */
      if(0 == den)
         return;
      inv_den26 = (1 << 26) / den;  // (todo) Goldschmidt

      /* const sFix rgaaAdd = rgaa_add_tbl[_numSamples];  // (note) caller must clamp numSamples to 0..16 range */
#define rgaaAdd (255)    // (1023/4)

      /* const sS32 dpitch32 = _fb->stride >> 2; */
      #define dpitch32 320
      sU32 *d = _fb->data.u32 + fb_off_y * dpitch32 + fb_off_x;

      /* printf("xxx bbSize=(%d,%d)\n", bbSizeX, bbSizeY); */

      sFix5 cy = (geo_shift_y << 5) + 16;
      const sFix5 yBot = cy + (bbSizeY << 5);
      geo_shift_x  = (geo_shift_x << 5) + 16;
      while(cy < yBot)
      {
         sFix5 cx = geo_shift_x;
         for(sU16 xi = 0; xi < (sU16)bbSizeX; xi++)
         {
            const sU32 d32 = d[xi];
            if((d32 & 0xFF000000u) != 0xFF000000u)
            {
               const sU32 uNotWithin = isNotWithin(cx, cy);

               if(uNotWithin)  // assume aaFixMode==2 (and bAA==true)
               {
                  sU16 aa = 0u;

                  if(!isNotWithinAA(cx + Df2x4(0.375f-0.5f), cy + Df2x4(0.125f-0.5f))) aa += rgaaAdd;
                  if(!isNotWithinAA(cx + Df2x4(0.875f-0.5f), cy + Df2x4(0.375f-0.5f))) aa += rgaaAdd;
                  if(!isNotWithinAA(cx + Df2x4(0.125f-0.5f), cy + Df2x4(0.625f-0.5f))) aa += rgaaAdd;
                  if(!isNotWithinAA(cx + Df2x4(0.625f-0.5f), cy + Df2x4(0.875f-0.5f))) aa += rgaaAdd;

                  if(aa > 4)
                  {
                     const sU8 a8 = (sU8)(aa >> 2);
                     d[xi] = BlendCoverage(d32, a8/*coverage*/, _c32);
                  }
               }
               else if(!uNotWithin)
               {
                  d[xi] = BlendCoverage(d32, 255u/*coverage*/, _c32);
               }

            } // if !full coverage

            // Next pixel
            cx += 32;
         }

         // Next scanline
         d += dpitch32;
         cy += 32;
      }
   }

#ifndef AAFX_C
};
#endif // AAFX_C


#undef Dargb

#if 0
#ifdef AAFX_C
Dstatic
const sFix
#else
const sFix TriRasterAA_fx::
#endif // AAFX_C
rgaa_add_tbl[17] = {
   /*  0 */ 1023,  // AA off (0 samples)
   /*  1 */ 1023,  // - unused
   /*  2 */  510,  // 2 samples
   /*  3 */  510,  // - unused
   /*  4 */  255,  // 4 samples
   /*  5 */  255,  // - unused
   /*  6 */  255,  // - unused
   /*  7 */  255,  // - unused
   /*  8 */  127,  // 8 samples
   /*  9 */  127,  // - unused
   /* 10 */  127,  // - unused
   /* 11 */  127,  // - unused
   /* 12 */  127,  // - unused
   /* 13 */  127,  // - unused
   /* 14 */  127,  // - unused
   /* 15 */  127,  // - unused
   /* 16 */   63,  // 16 samples
};
#endif

#endif // AAFX_C_SKIP_IMPL

#endif // TKSTRIRASTERAAFX_H__
