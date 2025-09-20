/// tri_raster_aa_fx.h
///
/// (c) 2024 Bastian Spiegel <bs@tkscript.de>
///     - published under terms of the GNU general public license (GPL)
///
///
#ifndef TKSTRIRASTERAAFX_H__
#define TKSTRIRASTERAAFX_H__

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
#ifndef AAFX_USE_FLOAT_INV
#define AAFX_USE_FLOAT_INV  0
#endif // AAFX_USE_FLOAT_INV

#define FX_COORD_SCL_AA 64.0f

typedef sS32 sFix;
typedef sS32 sFix26;

#define Dfixmul0(A,B) (((A) * (B))     )
#define Dfixmul1(A,B) (((A) * (B)) >> 1)
#define Dfixmul2(A,B) (((A) * (B)) >> 2)
#define Dfixmul4(A,B) (((A) * (B)) >> 4)
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
   sFix x, y;
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
   sFix x, y;

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

   Dfield sFix v0x; // = vb.x - va.x;
   Dfield sFix v0y; // = vb.y - va.y;

   Dfield sFix v1x; // = vc.x - va.x;
   Dfield sFix v1y; // = vc.y - va.y;

#if AAFX_USE_FLOAT_INV
   Dfield sF32 inv_denF;
#else
   Dfield sFix26 inv_den26;
#endif // AAFX_USE_FLOAT_INV

   // barycentric result of last isWithin() call
   Dfield t_ui a; // = (v2x * v1y - v1x_mul_v2y) * inv_den;
   Dfield t_ui b; // = (v0x_mul_v2y - v2x * v0y) * inv_den;
   Dfield t_ui c; // = 1 - a - b;

   Dfield sFix fr_bias_a;  // top-left fill-rule bias
   Dfield sFix fr_bias_b;
   Dfield sFix fr_bias_c;

   Dfield sFix fr_bias_a_aa;
   Dfield sFix fr_bias_b_aa;
   Dfield sFix fr_bias_c_aa;

   Dstatic const sFix rgaa_add_tbl[17];

   sU32 isNotWithin(const sFix x, const sFix y) {

      /* printf("xxx isWithin(x=%f y=%f)\n", x/64.0f, y/64.0f); */
      sFix v2x = (x - va.x);
      sFix v2y = (y - va.y);

      /* sF32 aF = (Dx2f6(v2x)*Dx2f6(v1y) - Dx2f6(v1x)*Dx2f6(v2y)) * inv_denF; */
      /* sF32 bF = (Dx2f6(v0x)*Dx2f6(v2y) - Dx2f6(v2x)*Dx2f6(v0y)) * inv_denF; */
      /* sF32 cF = 1.0f - aF - bF; */
#if AAFX_USE_FLOAT_INV
      a.s = sFix((Dfixmul0(v2x, v1y) - Dfixmul0(v1x, v2y)) * inv_denF) << 4;
      b.s = sFix((Dfixmul0(v0x, v2y) - Dfixmul0(v2x, v0y)) * inv_denF) << 4;
#else

#if 0
      a.s = Dfixmul16((long long)((Dfixmul6(v2x, v1y) - Dfixmul6(v1x, v2y))), inv_den26>>1)>>1;
      b.s = Dfixmul16((long long)((Dfixmul6(v0x, v2y) - Dfixmul6(v2x, v0y))), inv_den26>>1)>>1;
#elif 1
      // very tiny triangles not rendered correctly. good enough for "demo" purposes, though. should enable AA.
      a.s = (sFix)( (((long long)((Dfixmul6(v2x, v1y) - Dfixmul6(v1x, v2y)))) * inv_den26)>>16 );
      b.s = (sFix)( (((long long)((Dfixmul6(v0x, v2y) - Dfixmul6(v2x, v0y)))) * inv_den26)>>16 );
#elif 0
      a.s = sFix( ((((Dfixmul6(v2x, v1y) - Dfixmul6(v1x, v2y)))) * inv_den26>>1)>>15 );  // not working (images)
      b.s = sFix( ((((Dfixmul6(v0x, v2y) - Dfixmul6(v2x, v0y)))) * inv_den26>>1)>>15 );
#elif 0
      a.s = (((long long)(Dfixmul6(v2x, v1y) - Dfixmul6(v1x, v2y))) << 10) / den;
      b.s = (((long long)(Dfixmul6(v0x, v2y) - Dfixmul6(v2x, v0y))) << 10) / den;
#elif 0
      a.s = sFix((Dfixmul0(v2x>>1, v1y>>1) - Dfixmul0(v1x>>1, v2y>>1)) * inv_denF) << 6;
      b.s = sFix((Dfixmul0(v0x>>1, v2y>>1) - Dfixmul0(v2x>>1, v0y>>1)) * inv_denF) << 6;
#elif 0
      a.s = sFix((Dfixmul0((long long)v2x, v1y) - Dfixmul0((long long)v1x, v2y)) * inv_denF) << 4;
      b.s = sFix((Dfixmul0((long long)v0x, v2y) - Dfixmul0((long long)v2x, v0y)) * inv_denF) << 4;
#elif 0
      a.s = sFix((Dfixmul0(v2x, v1y) - Dfixmul0(v1x, v2y)) * inv_denF) << 4;
      b.s = sFix((Dfixmul0(v0x, v2y) - Dfixmul0(v2x, v0y)) * inv_denF) << 4;
#else
      a.s = sFix((Dfixmul0(v2x>>1, v1y>>1) - Dfixmul0(v1x>>1, v2y>>1)) / den) << 6;
      b.s = sFix((Dfixmul0(v0x>>1, v2y>>1) - Dfixmul0(v2x>>1, v0y>>1)) / den) << 6;
#endif

#endif // AAFX_USE_FLOAT_INV

#if 0
      a.s = aF * 65536;
      b.s = bF * 65536;
#endif

      c.s = 65536 - a.s - b.s;

      /* float mulA = (Dx2f6(v2x)*Dx2f6(v1y) - Dx2f6(v1x)*Dx2f6(v2y)); */
      /* float mulAx = (Dfixmul6(v2x, v1y) - Dfixmul6(v1x, v2y)) / 64.0f; */
      /* float mulBx = (Dfixmul6(v0x, v2y) - Dfixmul6(v2x, v0y)) / 64.0f; */

      /* aF = mulAx * inv_denF; */
      /* bF = mulBx * inv_denF; */

      /* float aF = a * 65536.0f; */
      /* float bF = b * 65536.0f; */

      /* if( sSI(y/64.0f) == 4 && sSI(x/64.0f) == 5 ) */
      /* { */
      /*    printf("xxx isWithin(x=%f y=%f) => mulA=%f mulAx=%f mulBx=%f a=%f aF=%f b=%f bF=%f c=%f cF=%f inv_den=%f v0(%f;%f) v1=(%f;%f) v2=(%f;%f)\n", x/64.0f, y/64.0f, mulA, mulAx, mulBx, Dx2f(a.s), aF, Dx2f(b.s), bF, Dx2f(c.s), cF, inv_denF, Dx2f6(v0x), Dx2f6(v0y), Dx2f6(v1x), Dx2f6(v1y), Dx2f6(v2x), Dx2f6(v2y)); */
      /* } */

#if 1
      a.s -= fr_bias_a;
      b.s -= fr_bias_b;
      c.s -= fr_bias_c;
#endif

#if 0
      aF = Dx2f(a.s);
      bF = Dx2f(b.s);
      cF = Dx2f(c.s);
#endif

#if 0
      aF -= fr_bias_a ? 0.0000001f : 0;
      bF -= fr_bias_b ? 0.0000001f : 0;
      cF -= fr_bias_c ? 0.0000001f : 0;
#endif

      sU32 uNotWithin = (a.u & mask_fx) | (b.u & mask_fx) | (c.u & mask_fx);

      /* /\* if( sSI(y/64.0f) == 4 && sSI(x/64.0f) == 5 ) *\/ */
      /* if( !(aF >= 0.0f && bF >= 0.0f && cF >= 0.0f) && (0u == uNotWithin) ) */
      /* { */
      /*    printf("xxx isWithin(x=%f y=%f) => a=%f(0x%08x) aF=%f b=%f(0x%08x) bF=%f c=%f(0x%08x) cF=%f uNotWithin=0x%08x\n", x/64.0f, y/64.0f, Dx2f(a.s), a.u, aF, Dx2f(b.s), b.u, bF, Dx2f(c.s), c.u, cF, uNotWithin); */
      /* } */

      return uNotWithin;
      /* return !( aF >= 0.0f && bF >= 0.0f && cF >= 0.0f ); */
   }

   sU32 isNotWithinAA(const sFix x, const sFix y)
#ifndef AAFX_C
      const
#endif // AAFX_C
      {

      sFix v2x = (x - va.x);
      sFix v2y = (y - va.y);

#if AAFX_USE_FLOAT_INV
      t_ui a; a.s = sFix((Dfixmul0(v2x, v1y) - Dfixmul0(v1x, v2y)) * inv_denF) << 4;
      t_ui b; b.s = sFix((Dfixmul0(v0x, v2y) - Dfixmul0(v2x, v0y)) * inv_denF) << 4;
#else
      t_ui a; a.s = (sFix)( (((long long)((Dfixmul6(v2x, v1y) - Dfixmul6(v1x, v2y)))) * inv_den26)>>16 );
      t_ui b; b.s = (sFix)( (((long long)((Dfixmul6(v0x, v2y) - Dfixmul6(v2x, v0y)))) * inv_den26)>>16 );
#endif // AAFX_USE_FLOAT_INV
      t_ui c; c.s = 65536 - a.s - b.s;

      a.s -= fr_bias_a_aa;
      b.s -= fr_bias_b_aa;
      c.s -= fr_bias_c_aa;

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
#endif // AAFX_C
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
      const TriRasterAAVec2x *_v1,  // 6 fractional bits
      const TriRasterAAVec2x *_v2,
      const TriRasterAAVec2x *_v3,
#else
      const TriRasterAAVec2x &_v1,  // 6 fractional bits
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
      bbMin.x = sMIN(va.x, sMIN(vb.x, vc.x)) & ~((1 << 6)-1);
      bbMin.y = sMIN(va.y, sMIN(vb.y, vc.y)) & ~((1 << 6)-1);

      const sS32 bbMaxX = (sMAX(va.x, sMAX(vb.x, vc.x)) + 32) >> 6;
      const sS32 bbMaxY = (sMAX(va.y, sMAX(vb.y, vc.y)) + 32) >> 6;

      Dvec2x_sub(va, bbMin);
      Dvec2x_sub(vb, bbMin);
      Dvec2x_sub(vc, bbMin);

      sS32 fb_off_x = bbMin.x >> 6;
      sS32 fb_off_y = bbMin.y >> 6;

      sS32 bbSizeX = bbMaxX - fb_off_x;
      sS32 bbSizeY = bbMaxY - fb_off_y;

      const sBool bAA = (0u != _numSamples);
      sS32 geo_shift_x;
      sS32 geo_shift_y;

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

      /* printf("xxx bb=(%d; %d; %d; %d)\n", fb_off_x, fb_off_y, bbSizeX, bbSizeY); */

      if(bAA)
      {
         fr_bias_a = 0;
         fr_bias_b = 0;
         fr_bias_c = 0;

         if(_aaFixMode)
         {
            const sFix biasShared = _aaBias; // Dfixmul16(-655, ((160*65536) / _fb->s.x));
            fr_bias_a_aa = biasShared;
            fr_bias_b_aa = biasShared;
            fr_bias_c_aa = biasShared;
         }
         else
         {
            fr_bias_a_aa = 0;
            fr_bias_b_aa = 0;
            fr_bias_c_aa = 0;
         }
      }
      else
      {
         // AA off
#ifdef AAFX_C
         #define biasBR 1
#else
         static const sFix biasBR = 1;//1
#endif // AAFX_C
         fr_bias_a = DIsTopLeftEdge(vc, va) ? 0 : biasBR;
         fr_bias_b = DIsTopLeftEdge(va, vb) ? 0 : biasBR;
         fr_bias_c = DIsTopLeftEdge(vb, vc) ? 0 : biasBR;
      }

      v0x = vb.x - va.x;
      v0y = vb.y - va.y;

      v1x = vc.x - va.x;
      v1y = vc.y - va.y;

#if AAFX_USE_FLOAT_INV
#if 1
      const sFix denX = Dfixmul6(v0x, v1y) - Dfixmul6(v1x, v0y);
      inv_denF = 64.0f / denX;
#else
      const float denF = Dx2f6(v0x) * Dx2f6(v1y) - Dx2f6(v1x) * Dx2f6(v0y);
      inv_denF = 1.0f / denF;
#endif
#else
      /* const sFix ta1 = v0x * v1y; */
      /* const sFix ta = Dfixmul12(v0x, v1y); */
      /* const sFix tb = Dfixmul12(v1x, v0y); */
      const sFix den = Dfixmul12(v0x, v1y) - Dfixmul12(v1x, v0y);  // 0 fractional bits
      if(0 == den)
         return;
      inv_den26 = (1 << 26) / den;  // (todo) Goldschmidt
#endif // AAFX_USE_FLOAT_INV

      /* printf("xxx v0x=%f v1y=%f v1x=%f v0y=%f denF=%f den=%f(%d) inv_denF=%f inv_den26=%g\n", Dx2f6(v0x), Dx2f6(v1y), Dx2f6(v1x), Dx2f6(v0y), denF, sF32(den/(1.0f)), den, inv_denF, (inv_den26 / sF64(1ll << 26))); */

      /* printf("xxx v0=(%d; %d) v1=(%d; %d) ta1=0x%08x ta=0x%08x tb=0x%08x den=0x%08x inv_den26=0x%08x\n", v0x, v0y, v1x, v1y, ta1, ta, tb, den, inv_den26); */

#if 1
      const sFix rgaaAdd = rgaa_add_tbl[_numSamples];  // (note) caller must clamp numSamples to 0..16 range
#else
      const sFix rgaaAdd = sFix(65536 / (_numSamples + 1u));
#endif

      const sS32 dpitch32 = _fb->stride >> 2;
      sU32 *d = _fb->data.u32 + fb_off_y * dpitch32 + fb_off_x;

      /* printf("xxx bbSize=(%d,%d)\n", bbSizeX, bbSizeY); */

      sFix cy = (geo_shift_y << 6) + 32;
      const sFix yBot = cy + (bbSizeY << 6);
      geo_shift_x  = geo_shift_x << 6;
      geo_shift_x += 32;
      while(cy < yBot)
      {
         /* const sFix cy = y + 32; */
         sS32 cx = geo_shift_x;
         for(sU16 xi = 0; xi < (sU16)bbSizeX; xi++)
         {
            /* const sS32 dx = (x >> 6) + (fb_off_x - geo_shift_x); */
            const sU32 d32 = d[xi];
            if((d32 & 0xFF000000u) != 0xFF000000u)
            {
               const sU32 uNotWithin = isNotWithin(cx, cy);
#if 0
               sFix curA = mathClampi(a, 0, 65536);  // save barycentrics, e.g. for texture fetch
               sFix curB = mathClampi(b, 0, 65536);
               sFix curC = mathClampi(c, 0, 65536);
#endif

#if 1
               if( (2u == _aaFixMode) ? uNotWithin : bAA )
#else
               if(uNotWithin)  // bAA must be true
#endif
               {
                  sFix aa = 0;

                  if(!uNotWithin)
                     aa += rgaaAdd;

                  switch(_numSamples)
                  {
                     case 2:
                        if(!isNotWithinAA(cx + Df2x6(0.75f-0.5f), cy + Df2x6(0.75f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.25f-0.5f), cy + Df2x6(0.25f-0.5f))) aa += rgaaAdd;
                        break;

                     default:
                     case 4:
                        if(!isNotWithinAA(cx + Df2x6(0.375f-0.5f), cy + Df2x6(0.125f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.875f-0.5f), cy + Df2x6(0.375f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.125f-0.5f), cy + Df2x6(0.625f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.625f-0.5f), cy + Df2x6(0.875f-0.5f))) aa += rgaaAdd;
                        break;

                     case 8:
                        if(!isNotWithinAA(cx + Df2x6(0.5625f-0.5f), cy + Df2x6(0.3125f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.4375f-0.5f), cy + Df2x6(0.6875f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.8125f-0.5f), cy + Df2x6(0.5625f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.3125f-0.5f), cy + Df2x6(0.1875f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.1875f-0.5f), cy + Df2x6(0.8125f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.0625f-0.5f), cy + Df2x6(0.4375f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.6875f-0.5f), cy + Df2x6(0.9375f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.9375f-0.5f), cy + Df2x6(0.0625f-0.5f))) aa += rgaaAdd;
                        break;

                     case 16:
                        // (note) ~7% faster than using __builtin_popcount()
                        if(!isNotWithinAA(cx + Df2x6(0.5625f-0.5f), cy + Df2x6(0.5625f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.4375f-0.5f), cy + Df2x6(0.3125f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.3125f-0.5f), cy + Df2x6(0.625f-0.5f)))  aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.75f-0.5f),   cy + Df2x6(0.4375f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.1875f-0.5f), cy + Df2x6(0.375f-0.5f)))  aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.625f-0.5f),  cy + Df2x6(0.8125f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.8125f-0.5f), cy + Df2x6(0.6875f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.6875f-0.5f), cy + Df2x6(0.1875f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.375f-0.5f),  cy + Df2x6(0.875f-0.5f)))  aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.5f-0.5f),    cy + Df2x6(0.0625f-0.5f))) aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.25f-0.5f),   cy + Df2x6(0.125f-0.5f)))  aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.125f-0.5f),  cy + Df2x6(0.75f-0.5f)))   aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.0f-0.5f),    cy + Df2x6(0.5f-0.5f)))    aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.9375f-0.5f), cy + Df2x6(0.25f-0.5f)))   aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.875f-0.5f),  cy + Df2x6(0.25f-0.5f)))   aa += rgaaAdd;
                        if(!isNotWithinAA(cx + Df2x6(0.0625f-0.5f), cy + Df2x6(0.0f-0.5f)))    aa += rgaaAdd;
                        break;
                  }

                  if(aa > Df2x(0.0039f))
                  {
                     const sU8 a8 = (sU8)(aa >> 8);//* 255.0f);
                     d[xi] = BlendCoverage(d32, a8/*coverage*/, _c32);
                  }
               }
               else if(!uNotWithin)
               {
#if 1
                  d[xi] = BlendCoverage(d32, 255u/*coverage*/, _c32);
#else
                  d[xi] = _c32; // debug
#endif
               }

            } // if !full coverage

            // Next pixel
            cx += 64;
         }

         // Next scanline
         d += dpitch32;
         cy += 64;
      }
   }

#ifndef AAFX_C
};
#endif // AAFX_C


#undef Dargb

#ifdef AAFX_C
Dstatic
const sFix
#else
const sFix TriRasterAA_fx::
#endif // AAFX_C
rgaa_add_tbl[17] = {
   /*  0 */ 65536,  // AA off (0 samples)
   /*  1 */ 65536,  // - unused
   /*  2 */ 21845,  // 2 samples
   /*  3 */ 21845,  // - unused
   /*  4 */ 13107,  // 4 samples
   /*  5 */ 13107,  // - unused
   /*  6 */ 13107,  // - unused
   /*  7 */ 13107,  // - unused
   /*  8 */  7281,  // 8 samples
   /*  9 */  7281,  // - unused
   /* 10 */  7281,  // - unused
   /* 11 */  7281,  // - unused
   /* 12 */  7281,  // - unused
   /* 13 */  7281,  // - unused
   /* 14 */  7281,  // - unused
   /* 15 */  7281,  // - unused
   /* 16 */  3855,  // 16 samples
};

#endif // AAFX_C_SKIP_IMPL

#undef DIsTopLeftEdge
#undef Dstatic
#undef Dfield
#undef Dvec2x_sub

#endif // TKSTRIRASTERAAFX_H__
