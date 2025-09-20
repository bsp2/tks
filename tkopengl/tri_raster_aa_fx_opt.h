/// tri_raster_aa_fx_opt.h
///
/// (c) 2024 Bastian Spiegel <bs@tkscript.de>
///     - published under terms of the GNU general public license (GPL)
///
///
#ifndef TKSTRIRASTERAAFX_H__
#define TKSTRIRASTERAAFX_H__

/* (todo) 16c palette

  + find bg color (top left pixel)
  + find most frequently used 15 remaining colors
  ? fb: 0xAI     A=coverage/alpha   I=palette index cur
     - when blending over coverage < 15: 
        - input: source alpha, source color index
        - read from fb:  coverage, dest color index
        x LUT: [16][16][16][16]  (65536 entries)
        ? coverage is implicitely encoded in source color index 

     - when dest coverage is 0:
        - write source coverage + (full coverage) color index
           ! pal entry for coverage+index combo may not exist (e.g. when pixel is overdrawn)
     - when dest coverage is 1..14
        - find final pal entry for [destcov][srccov][destidx][srcidx]

  - final pass: split color index nibble into 4 bitplanes

 */

// When defined, allow anti-aliasing (ignore numSamples parameter otherwise)
#ifndef AAFX_ALLOW_AA
#define AAFX_ALLOW_AA 1
#endif // AAFX_ALLOW_AA

#ifndef AAFX_NOAA_BIAS
#define AAFX_NOAA_BIAS 0
#define AAFX_NOAA_FIXED_BIAS 0
#else
#if 0 == AAFX_NOAA_BIAS
#define AAFX_NOAA_FIXED_BIAS -30
#else
#define AAFX_NOAA_FIXED_BIAS 0
#endif
#endif // AAFX_NOAA_BIAS

#ifndef AAFX_NUM_SAMPLES
#define AAFX_NUM_SAMPLES 4
#endif // AAFX_NUM_SAMPLES

// When defined, use 32/16 div instead of 64/32
//  (note) geometry should be split (see b_split_tris in svg.tks) so that largest bbox does not exceed 127x127
//  (note) but: unsplit test meshes still look okay @320x240/640x480/800x600
//  (note) triangles become a bit larger (precision issues). good enough for Low-Fi applications, though
#ifndef AAFX_USE_DIVSW
#define AAFX_USE_DIVSW  1
#endif // AAFX_USE_DIVSW

// Can be set to 0 when geometry is (pre-)split so that max tri bbox size does not exceed 127x127
/* #define AAFX_DIVS_SHIFT 6 */
#define AAFX_DIVS_SHIFT 5

// Use FPU for denominator division (50% faster on 68060)
#ifndef AAFX_USE_FLOAT_INV
#define AAFX_USE_FLOAT_INV  0
#endif // AAFX_USE_FLOAT_INV

// Debug: Skip RGB blending
#ifndef AAFX_COVERAGE_ONLY
#define AAFX_COVERAGE_ONLY 0
#endif // AAFX_COVERAGE_ONLY

// When defined, store ARGB4444 in fb dwords (instead of ARGB8888)
//  (note) must call applyClearColorAfterDrawTriAA4444() afterwards to expand back to ARGB8888
//  (note) ** don't use, precision is not high enough **
#ifndef AAFX_4444
#define AAFX_4444 0
#endif // AAFX_4444


#ifndef AAFX_DELTA
#if AAFX_USE_FLOAT_INV
#define AAFX_DELTA 0
#else
#define AAFX_DELTA 1
#endif // AAFX_USE_FLOAT_INV
#endif // AAFX_DELTA

// AA only. optimized for 4 samples and 320x240. 4 instead of 6 bit fractional coord bits.

// (note) 68020 has muls.l / divs.l, 68000 and 68060 do not.

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

// (note) 68020 has 256 byte instruction cache
// (note) 68060 has 8kb data and instruction caches (4-way associative)

typedef sS16 sFix4;
typedef sS16 sFix5;
typedef sS32 sFix6;
typedef sS32 sFix;
typedef sS32 sFix26;
typedef float sF32;

// (note) "wild boar" mesh with 6bits on 68020: 14500ms   4bits: 19800ms
#define FR_BITS 6
#define FR_HALF ((1 << FR_BITS)>>1)
#define FR_ONE  (1 << FR_BITS)

#define FX_COORD_SCL_AA 64.0f

#define Dfixmul0(A,B) (mulsw((A), (B)) )
#define Dfixmul1(A,B) (((A) * (B)) >> 1)
#define Dfixmul2(A,B) (((A) * (B)) >> 2)
#define Dfixmul4(A,B) (((A) * (B)) >> 4)
#define Dfixmul5(A,B) (((A) * (B)) >> 5)
/* #define Dfixmul6(A,B) (((A) * (B)) >> 6) */
#define Dfixmul6(A,B) (mulsw((A), (B)) >> 6)
#define Dfixmul18(A,B) (((A) * (B)) >> 18)
/* #define Dfixmul8(A,B) (((A) * (B)) >> 8) */
#define Dfixmul8(A,B) (mulsw((A), (B)) >> 8)
#define Dfixmul10(A,B) (((A) * (B)) >> 10)
/* #define Dfixmul12(A,B) (((A) * (B)) >> 12) */
#define Dfixmul12(A,B) (mulsw((A), (B)) >> 12)
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
#define Df2xr(A) ((sFix)((A) * ((sF32)(1 << FR_BITS))))
#define Df2x8(A) ((sFix)((A) * 256.0f))
#define Df2x10(A) ((sFix)((A) * 1024.0f))
#define Df2x12(A) ((sFix)((A) * 4096.0f))
#define Dx2f(A) ((sF32)((A) * (1.0f / 65536.0f)))
#define Dx2f6(A) ((sF32)((A) * (1.0f / 64.0f)))
#define Dx2fr(A) ((sF32)((A) * (1.0f / (1 << FR_BITS))))


#ifdef AAFX_C

#define Dinlinemethodwithin static inline
/* #define Dinlinemethodwithin  */

#define Dinlinemethodblend Dstatic inline
/* #define Dinlinemethodblend  */


#ifdef AAFX_C_NOSTATIC
#define Dstatic
#define Dfield 
#else
#define Dstatic static
#define Dfield static
#endif // AAFX_C_NOSTATIC


typedef struct s_TriRasterAAVec2x {
   sFix6 x, y;  // 6 fractional bits (FR_BITS)
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

#define Dinlinemethodwithin 
#define Dinlinemethodblend 

#define Dstatic static
#define Dfield

struct TriRasterAAVec2x {
   sFix6 x, y;

   inline void sub(const TriRasterAAVec2x &v) {
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
#if 0
   asm("mulsw %2,%0":"=d"(a): "0"(c),"d"(b): "cc");
#else
   asm("mulsw %2,%0"
       :"=d"(a)
       : "0"(c),"d"(b)
       : "cc"
       );
#endif
   return a;
}
#else
static inline int mulsw(short b, short c) {
   return b * c;
}
#endif // AMIGA

#if 1 && defined(AMIGA)
// https://gcc.gnu.org/onlinedocs/gcc/extensions-to-the-c-language-family/how-to-use-inline-assembly-language-in-c-code.html
static inline short divsw(signed int a, short b) {
   // divide a by b
   asm("divs %[div],%[io]\n\t"
       : [io]  "+d" (a)
       : [div] "d" (b)
       : "cc"
       );
   return (short)a;  // set remainder to 0
}
#else
static inline short divsw(signed int a, short b) {
#ifndef AMIGA_LLVM
   if( (a / b) > 32767 )
      printf("xxx divsw overflow: a=%d b=%d => a/b=%d\n", a, b, (a/b));
#endif // AMIGA_LLVM

   return a / b;
}
#endif // AMIGA

#ifndef AAFX_C
struct TriRasterAA_fx {
#endif // AAFX_C

#if 0 == AAFX_DELTA
   Dfield TriRasterAAVec2x va;
   Dfield TriRasterAAVec2x vb;
   Dfield TriRasterAAVec2x vc;

   Dfield sFix6 v0x; // = vb.x - va.x;
   Dfield sFix6 v0y; // = vb.y - va.y;

   Dfield sFix6 v1x; // = vc.x - va.x;
   Dfield sFix6 v1y; // = vc.y - va.y;
#endif // 0 == AAFX_DELTA

#ifdef AAFX_C
   #define mask_fx 0xffff0000u
#else
   static const sU32 mask_fx = 0xffff0000u;
#endif // AAFX_C

#if AAFX_USE_FLOAT_INV
   sS32 den;
   Dfield sF32 inv_denF;
#elif AAFX_USE_DIVSW
   sS16 den;
#else
   sS32 den;
   Dfield sFix26 inv_den26;
#endif

#if 0
   // barycentric result of last isNotWithin() call
   //  (note) e.g. for gouraud shading or texture mapping
   Dfield t_ui a; // = (v2x * v1y - v1x_mul_v2y) * inv_den;
   Dfield t_ui b; // = (v0x_mul_v2y - v2x * v0y) * inv_den;
   Dfield t_ui c; // = 1 - a - b;
#endif

#if AAFX_NOAA_BIAS
   Dfield sFix fr_bias_a;  // top-left fill-rule bias
   Dfield sFix fr_bias_b;
   Dfield sFix fr_bias_c;  // 65536 - bias
#endif // AAFX_NOAA_BIAS

#if AAFX_ALLOW_AA
   Dfield sFix fr_bias_a_aa;
   Dfield sFix fr_bias_b_aa;
   Dfield sFix fr_bias_c_aa;  // 65536 - bia
#endif // AAFX_ALLOW_AA

/* #define biasShared (((-655 * ((160*65536) / 320/\*FB_W*\/)) >> 16)) */
/* #define fr_bias_a_aa biasShared */
/* #define fr_bias_b_aa biasShared */
/* #define fr_bias_c_aa biasShared */


   Dinlinemethodwithin sU32 isNotWithin(
#if AAFX_DELTA
      const sS32 v2xv1y_minus_v1xv2y,
      const sS32 v0xv2y_minus_v2xv0y
#else
      const sFix v2x, const sFix v2y
#endif // AAFX_DELTA
                    ) {

#if 0
      /* printf("xxx isWithin(x=%f y=%f)\n", x/32.0f, y/32.0f); */
      sFix6 v2x = (x - va.x);
      sFix6 v2y = (y - va.y);
#endif

      t_ui a, b, c;

#if AAFX_USE_FLOAT_INV
      a.s = ((sFix)((Dfixmul0(v2x, v1y) - Dfixmul0(v1x, v2y)) * inv_denF)) << 4;
      b.s = ((sFix)((Dfixmul0(v0x, v2y) - Dfixmul0(v2x, v0y)) * inv_denF)) << 4;
#else

      // very tiny triangles not rendered correctly. good enough for "demo" purposes, though. should enable AA.
#if 6 == FR_BITS
#if 0
      a.s = (sFix)( (((long long)((Dfixmul6(v2x, v1y) - Dfixmul6(v1x, v2y)))) * inv_den26)>>16 );
      b.s = (sFix)( (((long long)((Dfixmul6(v0x, v2y) - Dfixmul6(v2x, v0y)))) * inv_den26)>>16 );
#elif 0
      // bad AA / rendering errors
      a.s = (sFix)( (((sS16)((Dfixmul8(v2x, v1y) - Dfixmul8(v1x, v2y)))) * inv_den26)>>14 );
      b.s = (sFix)( (((sS16)((Dfixmul8(v0x, v2y) - Dfixmul8(v2x, v0y)))) * inv_den26)>>14 );
#elif AAFX_USE_DIVSW
      // divs.w
#if 0
      sS32 m1 = ((sS32)((mulsw(v2x, v1y) - mulsw(v1x, v2y))<<2));
      sS32 m2 = ((sS32)((mulsw(v0x, v2y) - mulsw(v2x, v0y))<<2));
      a.s = (sFix)( divsw(m1, den) << 2 );
      b.s = (sFix)( divsw(m2, den) << 2 );
#elif AAFX_DELTA
      // ******** delta
      const sS32 m1 = ((sS32)((v2xv1y_minus_v1xv2y)>>AAFX_DIVS_SHIFT));
      const sS32 m2 = ((sS32)((v0xv2y_minus_v2xv0y)>>AAFX_DIVS_SHIFT));
      a.s = (sFix)( divsw(m1, den) << (4 + AAFX_DIVS_SHIFT) );
      b.s = (sFix)( divsw(m2, den) << (4 + AAFX_DIVS_SHIFT) );
#else
      const sS32 m1 = ((sS32)((mulsw(v2x, v1y) - mulsw(v1x, v2y))>>AAFX_DIVS_SHIFT));
      const sS32 m2 = ((sS32)((mulsw(v0x, v2y) - mulsw(v2x, v0y))>>AAFX_DIVS_SHIFT));
      a.s = (sFix)( divsw(m1, den) << (4 + AAFX_DIVS_SHIFT) );
      b.s = (sFix)( divsw(m2, den) << (4 + AAFX_DIVS_SHIFT) );
#endif

#elif 1
      // divs.l
      a.s = (sFix)( (((sS32)((mulsw(v2x, v1y) - mulsw(v1x, v2y))<<2)) / den)<<2);
      b.s = (sFix)( (((sS32)((mulsw(v0x, v2y) - mulsw(v2x, v0y))<<2)) / den)<<2);
#endif
#elif 4 == FR_BITS
      a.s = (sFix)( (((long long)((Dfixmul4(v2x, v1y) - Dfixmul4(v1x, v2y)))) * inv_den26)>>14 );
      b.s = (sFix)( (((long long)((Dfixmul4(v0x, v2y) - Dfixmul4(v2x, v0y)))) * inv_den26)>>14 );
#else
#error invalid FR_BITS
#endif

#endif // AAFX_USE_FLOAT_INV

#if 0
      sFix at = (sFix)( (((long long)((Dfixmul6(v2x, v1y) - Dfixmul6(v1x, v2y)))) * inv_den26)>>16 );
      sF32 aF = (Dx2f6(v2x)*Dx2f6(v1y) - Dx2f6(v1x)*Dx2f6(v2y)) * inv_denF;
      sFix aFx = sFix(aF*65536);

      /* if( abs(aFx - a.s) > 255 ) */
      /* if( (a.s < 0) ^ (aFx < 0) ) */
      /* if(0) */
      {
         printf("xxx v0=(%d,%d) v1=(%d,%d) v2=(%d,%d) den=%d  at=0x%08x(%d) a.s=0x%08x(%d) aF=%f(0x%08x)(%d)\n", v0x, v0y, v1x, v1y, v2x, v2y, den, at, at, a.s, a.s, aF, aFx, aFx);
         printf("xxx    v2x*v1y=%d - v1x*v2y=%d = %d  *4/d=%d\n", mulsw(v2x,v1y), mulsw(v1x,v2y), (mulsw(v2x,v1y) - mulsw(v1x,v2y)), ((mulsw(v2x,v1y) - mulsw(v1x,v2y))<<2)/den);
         printf("xxx    Dx2f6(v2x)*Dx2f6(v1y) = %f\n", Dx2f6(v2x)*Dx2f6(v1y));
         printf("xxx    Dx2f6(v1x)*Dx2f6(v2y) = %f  inv_denF=%f\n", Dx2f6(v1x)*Dx2f6(v2y), inv_denF);
         printf("xxx    v0x*v2y=%d - v2x*v0y=%d = %d  *4/d=%d\n", mulsw(v0x,v2y), mulsw(v2x,v0y), (mulsw(v0x,v2y) - mulsw(v2x,v0y)), ((mulsw(v0x,v2y) - mulsw(v2x,v0y))<<2)/den);
      }
#endif

      c.s = 65536 - a.s - b.s;

#if AAFX_NOAA_BIAS
      t_ui ta; ta.s = a.s - fr_bias_a;
      t_ui tb; tb.s = b.s - fr_bias_b;
      t_ui tc; tc.s = c.s - fr_bias_c;
      sU32 uNotWithin = (ta.u & mask_fx) | (tb.u & mask_fx) | (tc.u & mask_fx);
#elif 0 != AAFX_NOAA_FIXED_BIAS
      t_ui ta; ta.s = a.s - AAFX_NOAA_FIXED_BIAS;
      t_ui tb; tb.s = b.s - AAFX_NOAA_FIXED_BIAS;
      t_ui tc; tc.s = c.s - AAFX_NOAA_FIXED_BIAS;
      sU32 uNotWithin = (ta.u & mask_fx) | (tb.u & mask_fx) | (tc.u & mask_fx);
#else
      sU32 uNotWithin = (a.u & mask_fx) | (b.u & mask_fx) | (c.u & mask_fx);
#endif // AAFX_NOAA_BIAS

      return uNotWithin;
   }

#if AAFX_ALLOW_AA
   Dinlinemethodwithin sU32 isNotWithinAA(
#if AAFX_DELTA
      const sS32 v2xv1y_minus_v1xv2y,
      const sS32 v0xv2y_minus_v2xv0y
#else
      const sFix v2x, const sFix v2y
#endif // AAFX_DELTA
                      )
#ifndef AAFX_C
      const
#endif // AAFX_C
      {

      /* sFix6 v2x = (x - va.x); */
      /* sFix6 v2y = (y - va.y); */

      t_ui a,b,c;

#if AAFX_USE_FLOAT_INV
      a.s = ((sFix)((Dfixmul0(v2x, v1y) - Dfixmul0(v1x, v2y)) * inv_denF)) << 4;
      b.s = ((sFix)((Dfixmul0(v0x, v2y) - Dfixmul0(v2x, v0y)) * inv_denF)) << 4;
#else

#if 6 == FR_BITS
#if 0
      a.s = (sFix)( (((long long)((Dfixmul6(v2x, v1y) - Dfixmul6(v1x, v2y)))) * inv_den26)>>16 );
      b.s = (sFix)( (((long long)((Dfixmul6(v0x, v2y) - Dfixmul6(v2x, v0y)))) * inv_den26)>>16 );
#elif 0
      a.s = (sFix)( (((sS16)((Dfixmul8(v2x, v1y) - Dfixmul8(v1x, v2y)))) * inv_den26)>>14 );
      b.s = (sFix)( (((sS16)((Dfixmul8(v0x, v2y) - Dfixmul8(v2x, v0y)))) * inv_den26)>>14 );
#elif AAFX_USE_DIVSW
      // divs.w
#if 0
      sS32 m1 = ((sS32)((mulsw(v2x, v1y) - mulsw(v1x, v2y))<<2));
      sS32 m2 = ((sS32)((mulsw(v0x, v2y) - mulsw(v2x, v0y))<<2));
      a.s = (sFix)( divsw(m1, den) << 2 );
      b.s = (sFix)( divsw(m2, den) << 2 );
#elif AAFX_DELTA
      // ********
      const sS32 m1 = ((sS32)((v2xv1y_minus_v1xv2y)>>AAFX_DIVS_SHIFT));
      const sS32 m2 = ((sS32)((v0xv2y_minus_v2xv0y)>>AAFX_DIVS_SHIFT));
      a.s = (sFix)( divsw(m1, den) << (4 + AAFX_DIVS_SHIFT) );
      b.s = (sFix)( divsw(m2, den) << (4 + AAFX_DIVS_SHIFT) );
#else
      const sS32 m1 = ((sS32)((mulsw(v2x, v1y) - mulsw(v1x, v2y))>>AAFX_DIVS_SHIFT));
      const sS32 m2 = ((sS32)((mulsw(v0x, v2y) - mulsw(v2x, v0y))>>AAFX_DIVS_SHIFT));
      a.s = (sFix)( divsw(m1, den) << (4 + AAFX_DIVS_SHIFT) );
      b.s = (sFix)( divsw(m2, den) << (4 + AAFX_DIVS_SHIFT) );
#endif

#elif 1
      // divs.l
      a.s = (sFix)( (((sS32)((mulsw(v2x, v1y) - mulsw(v1x, v2y))<<2)) / den)<<2);
      b.s = (sFix)( (((sS32)((mulsw(v0x, v2y) - mulsw(v2x, v0y))<<2)) / den)<<2);
#endif
#elif 4 == FR_BITS
      a.s = (sFix)( (((long long)((Dfixmul4(v2x, v1y) - Dfixmul4(v1x, v2y)))) * inv_den26)>>14 );
      b.s = (sFix)( (((long long)((Dfixmul4(v0x, v2y) - Dfixmul4(v2x, v0y)))) * inv_den26)>>14 );
#else
#error invalid FR_BITS
#endif

#endif // AAFX_USE_FLOAT_INV

      c.s = 65536 - a.s - b.s;
      
      a.s -= fr_bias_a_aa;
      b.s -= fr_bias_b_aa;
      c.s -= fr_bias_c_aa;

      return (a.u & mask_fx) | (b.u & mask_fx) | (c.u & mask_fx);
   }

#if AAFX_4444
   Dinlinemethodblend sU32 BlendCoverage4444(const sU32 d32, const sU8 _coverage/*0..15*/, const sU32 c32) {
      // d32/c32 format: #0a0r0g0b
      sU16 as  = (c32 >> 24) & 15u;
      as += (as & 1u);
      as = (_coverage * as) >> 4;
      as += (as & 1u);
      sU16 ad  = (d32 >> 24) & 15u;
      sU16 a = ((15u - ad) * as) >> 4;
      ad = a + ad;
      a += (a & 1u);
#ifdef AAFX_C
      #define rgbMask 0x000F0F0Fu
#else
      static const sU32 rgbMask = 0x000F0F0Fu;
#endif // AAFX_C
      const sU32 cdrgb = (d32 & rgbMask);
      const sU32 csrgb = (c32 & rgbMask);
      return
         (ad << 24)
         | ((cdrgb + ((csrgb * a) >> 4)) & rgbMask)
         ;
   }

#else
      // ARGB8888
   Dinlinemethodblend sU32 BlendCoverage8888(const sU32 d32, const sU8 _coverage, const sU32 c32) {
      /* return c32; */  // 6080 ms w/ blending, 4980 ms w/o  (~1396px blended per frame @50Hz on MC68EC020@14MHz)
      sU16 as  = (c32 >> 24) & 255u;
      as += (as & 1u);
      as = (_coverage * as) >> 8;
      as += (as & 1u);
      sU16 ad  = (d32 >> 24) & 255u;
      sU16 a = ((255u - ad) * as) >> 8;
      ad = a + ad;
#if 0 == AAFX_COVERAGE_ONLY
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
      const sU16 cdg = (d32 & gMask);
      const sU16 csg = (c32 & gMask);
      return
         (ad << 24)
         | ( (cdg + (((csg * a) >> 8))) & gMask )
         | ((cdrb + ((csrb * a) >> 8)) & rbMask)
         ;
#else
      return (ad << 24);
#endif // AAFX_COVERAGE_ONLY
   }
#endif // AAFX_4444

#endif // AAFX_ALLOW_AA

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
      const TriRasterAAVec2x *_v1,  // 6 fractional bits (FR_BITS)  (todo) pass TriRasterAAVec2x[3] array address
      const TriRasterAAVec2x *_v2,
      const TriRasterAAVec2x *_v3,
#else
      const TriRasterAAVec2x &_v1,  // 6 fractional bits (FR_BITS)
      const TriRasterAAVec2x &_v2,
      const TriRasterAAVec2x &_v3,
#endif // AAFX_C
      sU32 _c32,
      Bitmap *_fb,
      const sU32 _numSamples,
      const sU32 _aaFixMode,
      const sFix _aaBias
                          ) {
      // (note) 'numSamples' must be either 0 (AA off), or 2 / 4 / 8 / 16
      // (note) 'aaFixMode' slightly enlarges the triangle to cover the seams at (inner) shared edges (2=outer pixels only)
      // (note) 'aaBias' should be set to Df2x(-0.01f * (160.0f / _fb->s.x))  /  Dfixmul16(-655, ((160*65536) / _fb->s.x));

#if 0 == AAFX_ALLOW_AA
      (void)_numSamples;
      (void)_aaBias;
#endif
      (void)_aaFixMode;

#if AAFX_4444
      _c32 = (_c32 >> 4) & 0x0F0F0F0Fu;
#endif // AAFX_4444

#if AAFX_DELTA
      TriRasterAAVec2x va;
      TriRasterAAVec2x vb;
      TriRasterAAVec2x vc;

      sFix6 v0x; // = vb.x - va.x;
      sFix6 v0y; // = vb.y - va.y;

      sFix6 v1x; // = vc.x - va.x;
      sFix6 v1y; // = vc.y - va.y;
#endif // 0 == AAFX_DELTA

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
      bbMin.x = sMIN(va.x, sMIN(vb.x, vc.x)) & ~((1 << FR_BITS)-1);
      bbMin.y = sMIN(va.y, sMIN(vb.y, vc.y)) & ~((1 << FR_BITS)-1);

      const sS32 bbMaxX = (sMAX(va.x, sMAX(vb.x, vc.x)) + FR_HALF) >> FR_BITS;
      const sS32 bbMaxY = (sMAX(va.y, sMAX(vb.y, vc.y)) + FR_HALF) >> FR_BITS;

      Dvec2x_sub(va, bbMin);
      Dvec2x_sub(vb, bbMin);
      Dvec2x_sub(vc, bbMin);

      sS32 fb_off_x = bbMin.x >> FR_BITS;
      sS32 fb_off_y = bbMin.y >> FR_BITS;

      sS32 bbSizeX = bbMaxX - fb_off_x;
      sS32 bbSizeY = bbMaxY - fb_off_y;

#if AAFX_ALLOW_AA
      const sBool bAA = (0u != _numSamples);

      sS32 geo_shift_x;
      sS32 geo_shift_y;
#endif // AAFX_ALLOW_AA

#if AAFX_ALLOW_AA
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
#endif // AAFX_ALLOW_AA

      if(fb_off_x < 0)
      {
         bbSizeX = sMAX(0, bbSizeX + fb_off_x);
#if AAFX_ALLOW_AA
         geo_shift_x -= fb_off_x;
#endif // AAFX_ALLOW_AA
         fb_off_x = 0;
      }

      if(fb_off_y < 0)
      {
         bbSizeY = sMAX(0, bbSizeY + fb_off_y);
#if AAFX_ALLOW_AA
         geo_shift_y -= fb_off_y;
#endif // AAFX_ALLOW_AA
         fb_off_y = 0;
      }

      if( fb_off_x + bbSizeX > _fb->s.x )
         bbSizeX = _fb->s.x - fb_off_x;

      if( fb_off_y + bbSizeY > _fb->s.y )
         bbSizeY = _fb->s.y - fb_off_y;

      /* printf("xxx bb=(%d; %d; %d; %d)\n", fb_off_x, fb_off_y, bbSizeX, bbSizeY); */

#if AAFX_ALLOW_AA
      if(bAA)
      {
#if AAFX_NOAA_BIAS
         fr_bias_a = 0;
         fr_bias_b = 0;
         fr_bias_c = 0;
#endif // AAFX_NOAA_BIAS

         /* if(_aaFixMode) */
         /* { */
            const sFix biasShared = _aaBias; // Dfixmul16(-655, ((160*65536) / _fb->s.x));
            fr_bias_a_aa = biasShared;
            fr_bias_b_aa = biasShared;
            fr_bias_c_aa = biasShared;
         /* } */
         /* else */
         /* { */
         /*    fr_bias_a_aa = 0; */
         /*    fr_bias_b_aa = 0; */
         /*    fr_bias_c_aa = 0; */
         /* } */
      }
#if AAFX_NOAA_BIAS
#endif // AAFX_NOAA_BIAS
#endif // AAFX_ALLOW_AA
#if AAFX_NOAA_BIAS
      else
      {
         // AA off
#ifdef AAFX_C
         #define biasBR 1
#else
#if 0 == AAFX_USE_DIVSW
         static const sFix biasBR = 1;//1
#else
         static const sFix biasBR = -328;  // enlarge tris to cover holes (320x240)
#endif
#endif // AAFX_C
         fr_bias_a = DIsTopLeftEdge(vc, va) ? 0 : biasBR;
         fr_bias_b = DIsTopLeftEdge(va, vb) ? 0 : biasBR;
         fr_bias_c = DIsTopLeftEdge(vb, vc) ? 0 : biasBR;

         fr_bias_a_aa = 0;  // avoid compiler warning (vars are unused)
         fr_bias_b_aa = 0;
         fr_bias_c_aa = 0;
      }
#endif // AAFX_NOAA_BIAS

      v0x = vb.x - va.x;
      v0y = vb.y - va.y;

      v1x = vc.x - va.x;
      v1y = vc.y - va.y;

#if AAFX_USE_FLOAT_INV
#if 1
      const sFix denX = Dfixmul6(v0x, v1y) - Dfixmul6(v1x, v0y);
      if(0 == denX)
         return;
      inv_denF = 64.0f / denX;
#else
      const float denF = Dx2f6(v0x) * Dx2f6(v1y) - Dx2f6(v1x) * Dx2f6(v0y);
      if(0.0f == denF)
         return;
      inv_denF = 1.0f / denF;
#endif
#else

#if 4 == FR_BITS
      /*const sFix */den = Dfixmul8(v0x, v1y) - Dfixmul8(v1x, v0y);  // 0 fractional bits (Dfixmul<2*FR_BITS>)
#elif 6 == FR_BITS
      /*const sFix */den = Dfixmul12(v0x, v1y) - Dfixmul12(v1x, v0y);  // 0 fractional bits (Dfixmul<2*FR_BITS>)
      /* if( (Dfixmul12(v0x, v1y) - Dfixmul12(v1x, v0y)) >= 32767 ) */
      /*    printf("xxx den overflow den32=%d\n", (Dfixmul12(v0x, v1y) - Dfixmul12(v1x, v0y))); */
#else
#error invalid FR_BITS
#endif
      if(0 == den)
         return;
#if 0 == AAFX_USE_DIVSW
      inv_den26 = (1 << 26) / den;  // (todo) Goldschmidt
#endif // AAFX_USE_DIVSW

      /* const float denF = Dx2f6(v0x) * Dx2f6(v1y) - Dx2f6(v1x) * Dx2f6(v0y); */
      /* inv_denF = 1.0f / denF; */
      /* if(inv_denF < 0.0f) */
      /*    printf("xxx inv_denF=%f den=%d den16=%d\n", inv_denF, den, den16); */

      /* printf("xxx v0x=%f v1y=%f v1x=%f v0y=%f denF=%f den=%f(%d) inv_denF=%f inv_den26=%g\n", Dx2fr(v0x), Dx2fr(v1y), Dx2fr(v1x), Dx2fr(v0y), denF, sF32(den/(1.0f)), den, inv_denF, (inv_den26 / sF64(1ll << 26))); */

      /* printf("xxx v0=(%d; %d) v1=(%d; %d) ta1=0x%08x ta=0x%08x tb=0x%08x den=0x%08x inv_den26=0x%08x\n", v0x, v0y, v1x, v1y, ta1, ta, tb, den, inv_den26); */
#endif // AAFX_USE_FLOAT_INV

#if 2 == AAFX_NUM_SAMPLES
#if AAFX_4444
      #define rgaaAdd (15)  // >> 1
#else
      #define rgaaAdd (255) // >> 1
#endif // AAFX_4444
#else
#if AAFX_4444
      #define rgaaAdd (15)  // >> 2
#else
      #define rgaaAdd (255)  // >> 2
#endif // AAFX_4444
#endif // AAFX_NUM_SAMPLES

      const sS32 dpitch32 = _fb->stride >> 2;
      /* #define dpitch32 320 */
      sU32 *d = _fb->data.u32 + fb_off_y * dpitch32 + fb_off_x;

      /* printf("xxx bbSize=(%d,%d)\n", bbSizeX, bbSizeY); */

#if 0
      {
         for(sUI y = 0u; y < 4; y++)
         {
            for(volatile sUI x = 0u; x < 4; x++)
            {
               d[x] = 0xFFFF0000u;
            }
            d += 320;
         }
         return;
      }
#endif

      /* printf("xxx bbSizeX=%d bbSizeY=%d\n", bbSizeX, bbSizeY); */

#if AAFX_ALLOW_AA
      sFix cy = (geo_shift_y << FR_BITS) + FR_HALF;
#else
      sFix cy = FR_HALF;
#endif // AAFX_ALLOW_AA
      cy -= va.y;
      const sFix yBot = cy + (bbSizeY << FR_BITS);
#if AAFX_ALLOW_AA
      geo_shift_x  = geo_shift_x << FR_BITS;
      geo_shift_x += FR_HALF;
#endif // AAFX_ALLOW_AA

#if AAFX_DELTA
      sFix v1yDeltaX = v1y << FR_BITS;  // cx
      sFix v0yDeltaX = v0y << FR_BITS;

      sFix v1xDeltaY = v1x << FR_BITS;  // cy
      sFix v0xDeltaY = v0x << FR_BITS;
#endif // AAFX_DELTA

      /* if(!isNotWithinAA(cx + Df2x6(0.75f-0.5f), cy + Df2x6(0.75f-0.5f))) aa += rgaaAdd; */
      /* if(!isNotWithinAA(cx + Df2x6(0.25f-0.5f), cy + Df2x6(0.25f-0.5f))) aa += rgaaAdd; */

#if AAFX_DELTA
#if AAFX_ALLOW_AA
      sS32 cx = geo_shift_x;
#else
      sS32 cx = FR_HALF;
#endif // AAFX_ALLOW_AA
      cx -= va.x;

#if 0
      if(70 == bbSizeX && 9 == bbSizeY)
         printf("xxx AAFX_DELTA initial cx=0x%08x\n", cx);
#endif

      /* sS32 v2xv1y_minus_v1xv2y = ((sS32)((mulsw( cx, v1y) - mulsw(v1x,  cy))>>AAFX_DIVS_SHIFT)); */
      /* sS32 v0xv2y_minus_v2xv0y = ((sS32)((mulsw(v0x,  cy) - mulsw( cx, v0y))>>AAFX_DIVS_SHIFT)); */
      sS32 v2xv1y_minus_v1xv2y = ((sS32)((mulsw(cx,  v1y) - mulsw(v1x,  cy))                 ));
      sS32 v0xv2y_minus_v2xv0y = ((sS32)((mulsw(v0x,  cy) - mulsw( cx, v0y))                 ));
      sS32 v2xv1y_minus_v1xv2y_rewindx = /*-*/ (bbSizeX * v1yDeltaX) + v1xDeltaY;
      sS32 v0xv2y_minus_v2xv0y_rewindx = /*+*/ (bbSizeX * v0yDeltaX) + v0xDeltaY;

#if AAFX_ALLOW_AA
#if 2 == AAFX_NUM_SAMPLES
      sS32 aaTbl2[2] = {
         // sample 1:  if(!isNotWithinAA(cx + Df2x6(0.75f-0.5f), cy + Df2x6(0.75f-0.5f))) aa += rgaaAdd;   ==(0.25;0.25)
         /* /\*+*\/ (v1yDeltaX >> 2) - (v1xDeltaY >> 2),   // sample 1 v2xv1y_minus_v1xv2y delta */
         /* /\*+*\/ (v0xDeltaY >> 2) - (v0yDeltaX >> 2),   // sample 1 v0xv2y_minus_v2xv0y delta */
         /*+*/ (v1yDeltaX - v1xDeltaY) >> 2,   // sample 1 v2xv1y_minus_v1xv2y delta
         /*+*/ (v0xDeltaY - v0yDeltaX) >> 2,   // sample 1 v0xv2y_minus_v2xv0y delta

         /* // sample 2:  if(!isNotWithinAA(cx + Df2x6(0.25f-0.5f), cy + Df2x6(0.25f-0.5f))) aa += rgaaAdd;   ==(-0.25;-0.25) (mirror x/y) */
         /* /\*-*\/ (v1yDeltaX + v1xDeltaY) >> 2,   // sample 2  (note) // -term1-term2  = -(term1 + term2) */
         /* /\*-*\/ (v0xDeltaY + v0yDeltaX) >> 2 */
      };
#else
      // 4 samples
      const sFix aaV1y125 = (v1yDeltaX >> 3);      // 0.125
      const sFix aaV1x125 = (v1xDeltaY >> 3);      // 0.125

      const sFix aaV1y375 = (v1yDeltaX * 3) >> 3;  // 0.375
      const sFix aaV1x375 = (v1xDeltaY * 3) >> 3;  // 0.375

      const sFix aaV0x125 = (v0xDeltaY >> 3);      // 0.125
      const sFix aaV0y125 = (v0yDeltaX >> 3);      // 0.125

      const sFix aaV0x375 = (v0xDeltaY * 3) >> 3;  // 0.375
      const sFix aaV0y375 = (v0yDeltaX * 3) >> 3;  // 0.375

      sS32 aaTbl4[8] = {
         // (todo)
         // sample 1: if(!isNotWithinAA(cx + Df2xr(0.375f-0.5f), cy + Df2xr(0.125f-0.5f))) aa += rgaaAdd;  ==(-0.125;-0.375)
         /*+*/ aaV1x375 - aaV1y125,   // -aaV1y125 - -aaV1x375 = -aaV1y125 + aaV1x375 = aaV1x375 - aaV1y125
         /*+*/ aaV0y125 - aaV0x375,   // -aaV0x375 - -aaV0y125 = -aaV0x375 + aaV0y125 = aaV0y125 - aaV0x375

         // sample 2: if(!isNotWithinAA(cx + Df2xr(0.875f-0.5f), cy + Df2xr(0.375f-0.5f))) aa += rgaaAdd;  ==(0.375;-0.125)
         /*+*/ aaV1y375 + aaV1x125,   // t1 - -t2 =  (t1 + t2)
         /*-*/ aaV0x125 + aaV0y375,   // -t1 - t2 = -(t1 + t2)

         // sample 3: if(!isNotWithinAA(cx + Df2xr(0.125f-0.5f), cy + Df2xr(0.625f-0.5f))) aa += rgaaAdd;  ==(-0.375;0.125)
         /*-*/ aaV1y375 + aaV1x125,   // -t1 - t2 = -(t1 + t2)
         /*+*/ aaV0x125 + aaV0y375,   // t1 - -t2 =  (t1 + t2)

         // sample 4: if(!isNotWithinAA(cx + Df2xr(0.625f-0.5f), cy + Df2xr(0.875f-0.5f))) aa += rgaaAdd;  ==(0.125;0.375)
         /*+*/ aaV1y125 - aaV1x375,
         /*+*/ aaV0x375 - aaV0y125,
      };
#endif // 2 == AAFX_NUM_SAMPLES
#endif // AAFX_ALLOW_AA
#endif // AAFX_DELTA

      while(cy < yBot)
      {
#if 0 == AAFX_DELTA
#if AAFX_ALLOW_AA
         sS32 cx = geo_shift_x;
#else
         sS32 cx = FR_HALF;
#endif // AAFX_ALLOW_AA
         cx -= va.x;
#endif // AAFX_DELTA

         /* sS32 v2xv1y_minus_v1xv2y = ((sS32)((mulsw(cx,  v1y) - mulsw(v1x,  cy))                 )); */
         /* sS32 v0xv2y_minus_v2xv0y = ((sS32)((mulsw(v0x,  cy) - mulsw( cx, v0y))                 )); */

#if 0
         if(70 == bbSizeX && 9 == bbSizeY)
            printf("xxx cy=0x%08x cx=0x%08x term1=0x%08x term2=0x%08x v1Dt=(0x%08x, 0x%08x) v0Dt=(0x%08x, 0x%08x) bbSizeX=%d term1_dy=0x%08x\n", cy, cx, v2xv1y_minus_v1xv2y, v0xv2y_minus_v2xv0y, v1xDeltaY, v1yDeltaX, v0xDeltaY, v0yDeltaX, bbSizeX, v2xv1y_minus_v1xv2y_rewindx);
#endif

         for(sU16 xi = 0; xi < (sU16)bbSizeX; xi++)
         {
            const sU32 d32 = d[xi];
#if AAFX_4444
            if((d32 & 0x0F000000u) != 0x0F000000u)
#else
            if((d32 & 0xFF000000u) != 0xFF000000u)
#endif // AAFX_4444
            {
#if AAFX_DELTA
               const sU32 uNotWithin = isNotWithin(v2xv1y_minus_v1xv2y, v0xv2y_minus_v2xv0y);
#else
               const sU32 uNotWithin = isNotWithin(cx, cy);
#endif // AAFX_DELTA

#if AAFX_ALLOW_AA
               if(uNotWithin)  // bAA must be true
               {
                  sU16 aa = 0u;

                  /* if(!uNotWithin) */
                  /*    aa += rgaaAdd; */

#if 2 == AAFX_NUM_SAMPLES
                  // 2 samples
#if AAFX_DELTA
                  if(!isNotWithinAA(v2xv1y_minus_v1xv2y + aaTbl2[0], v0xv2y_minus_v2xv0y + aaTbl2[1])) aa += rgaaAdd;
                  if(!isNotWithinAA(v2xv1y_minus_v1xv2y - aaTbl2[0], v0xv2y_minus_v2xv0y - aaTbl2[1])) aa += rgaaAdd;
#else
                  if(!isNotWithinAA(cx + Df2x6(0.75f-0.5f), cy + Df2x6(0.75f-0.5f))) aa += rgaaAdd;
                  if(!isNotWithinAA(cx + Df2x6(0.25f-0.5f), cy + Df2x6(0.25f-0.5f))) aa += rgaaAdd;
#endif // AAFX_DELTA
#else
                  // 4 samples
#if AAFX_DELTA
                  if(!isNotWithinAA(v2xv1y_minus_v1xv2y + aaTbl4[0], v0xv2y_minus_v2xv0y + aaTbl4[1])) aa += rgaaAdd;
                  if(!isNotWithinAA(v2xv1y_minus_v1xv2y + aaTbl4[2], v0xv2y_minus_v2xv0y - aaTbl4[3])) aa += rgaaAdd;
                  if(!isNotWithinAA(v2xv1y_minus_v1xv2y - aaTbl4[4], v0xv2y_minus_v2xv0y + aaTbl4[5])) aa += rgaaAdd;
                  if(!isNotWithinAA(v2xv1y_minus_v1xv2y + aaTbl4[6], v0xv2y_minus_v2xv0y + aaTbl4[7])) aa += rgaaAdd;
#else
                  if(!isNotWithinAA(cx + Df2xr(0.375f-0.5f), cy + Df2xr(0.125f-0.5f))) aa += rgaaAdd;
                  if(!isNotWithinAA(cx + Df2xr(0.875f-0.5f), cy + Df2xr(0.375f-0.5f))) aa += rgaaAdd;
                  if(!isNotWithinAA(cx + Df2xr(0.125f-0.5f), cy + Df2xr(0.625f-0.5f))) aa += rgaaAdd;
                  if(!isNotWithinAA(cx + Df2xr(0.625f-0.5f), cy + Df2xr(0.875f-0.5f))) aa += rgaaAdd;
#endif // AAFX_DELTA
#endif // AAFX_NUM_SAMPLES

                  if(aa > 4)
                  {
#if AAFX_4444
#if 2 == AAFX_NUM_SAMPLES
                     const sU8 a8 = (sU8)(aa >> 1);  // 2 samples (0..15)
#else
                     const sU8 a8 = (sU8)(aa >> 2);  // 4 samples (0..15)
#endif
                     d[xi] = BlendCoverage4444(d32, a8/*coverage*/, _c32);
#else
#if 2 == AAFX_NUM_SAMPLES
                     const sU8 a8 = (sU8)(aa >> 1);  // 2 samples (0..255)
#else
                     const sU8 a8 = (sU8)(aa >> 2);  // 4 samples (0..255)
#endif
                     d[xi] = BlendCoverage8888(d32, a8/*coverage*/, _c32);
#endif // AAFX_4444
                  }
               }
               else
#endif // AAFX_ALLOW_AA
               {
                  if(!uNotWithin)
                  {
#if AAFX_ALLOW_AA
#if AAFX_4444
                     d[xi] = BlendCoverage4444(d32, 15u/*coverage*/, _c32);
#else
                     d[xi] = BlendCoverage8888(d32, 255u/*coverage*/, _c32);
#endif // AAFX_4444
#else
                     d[xi] = _c32;  // (note) c32.a must be 255 (15 in 4444 mode)
#endif // AAFX_ALLOW_AA
                  }
               }

               /* d[xi] = 0;//(255u << 24) | cy; */

            } // if !full coverage

            // Next pixel
#if AAFX_DELTA
#if 0
            v2xv1y_minus_v1xv2y += v1y << FR_BITS;
            v0xv2y_minus_v2xv0y -= v0y << FR_BITS;
#else
            // ~10% faster on 68EC020
            v2xv1y_minus_v1xv2y += v1yDeltaX;
            v0xv2y_minus_v2xv0y -= v0yDeltaX;
#endif
#else
            cx += FR_ONE;
#endif // AAFX_DELTA
         }

         // Next scanline
#if AAFX_DELTA
         v2xv1y_minus_v1xv2y -= v2xv1y_minus_v1xv2y_rewindx;
         v0xv2y_minus_v2xv0y += v0xv2y_minus_v2xv0y_rewindx;
#endif // AAFX_DELTA
         d += dpitch32;
         cy += FR_ONE;
      }
   }

#ifndef AAFX_C
};
#endif // AAFX_C

#endif // AAFC_C_SKIP_IMPL

#undef DIsTopLeftEdge
#undef Dstatic
#undef Dfield
#undef Dvec2x_sub

#undef FR_BITS
#undef FR_HALF
#undef FR_ONE

#undef Df2x

#endif // TKSTRIRASTERAAFX_H__
