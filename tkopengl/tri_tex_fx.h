/// tri_tex_fx.h
///
/// (c) 2024 Bastian Spiegel <bs@tkscript.de>
///     - published under terms of the GNU general public license (GPL)
///
///
#ifndef TKS_TRI_TEX_FX_H__
#define TKS_TRI_TEX_FX_H__

// When defined, enable bilinear filtering
#ifndef TRITEX_BILINEAR
#define TRITEX_BILINEAR  1
#endif // TRITEX_BILINEAR


typedef struct tritexvec2_s {
   sF32 x;
   sF32 y;
} tritexvec2_t;

#define Df2x(f) sS32(f*65536)

struct TriTexFx {
   sU32 *fb;         // ARGB32 framebuffer
   sU32  fbW, fbH;

   const sU32 *tex;  // ARGB32 texels
   sU32  texW, texH;

   sU32 a8;          // global alpha


   static inline void SortVertices(const tritexvec2_t &vtx1, const tritexvec2_t &vtx2, const tritexvec2_t &vtx3,
                                   tritexvec2_t &v1, tritexvec2_t &v2, tritexvec2_t &v3
                                   ) {
      // returns sorted vertices in v1..v3
      if(vtx1.y < vtx3.y)
      {
         if(vtx1.y < vtx2.y)
         {
            // v1 above v3 and v2
            if(vtx2.y < vtx3.y)
            {
               // v1 => v2 => v3
               v1 = vtx1;
               v2 = vtx2;
               v3 = vtx3;
            }
            else
            {
               // v1 => v3 => v2
               v1 = vtx1;
               v2 = vtx3;
               v3 = vtx2;
            }
         }
         else
         {
            // v1 above v3 and below v2
            // v2 => v1 => v3
            v1 = vtx2;
            v2 = vtx1;
            v3 = vtx3;
         }
      }
      else
      {
         // v3 above v1
         if(vtx3.y < vtx2.y)
         {
            // v3 above v1 and v2
            if(vtx2.y < vtx1.y)
            {
               // v3 => v2 => v1
               v1 = vtx3;
               v2 = vtx2;
               v3 = vtx1;
            }
            else
            {
               // v3 => v1 => v2
               v1 = vtx3;
               v2 = vtx1;
               v3 = vtx2;
            }
         }
         else
         {
            // v3 above v1 and below v2
            // v2 => v3 => v1
            v1 = vtx2;
            v2 = vtx3;
            v3 = vtx1;
         }
      }
   }

   inline sU32 fetchTexelClamp(sS32 texX, sS32 texY) const {
      if(texX < 0)
         texX = 0;
      else if(texX >= (sS32)texW)
         texX = (sS32)texW - 1;

      if(texY < 0)
         texY = 0;
      else if(texY >= (sS32)texH)
         texY = (sS32)texH - 1;

      return tex[texY * texW + texX];
   }

   static inline sU32 BlendC32(const sU32 _c32A, const sU32 _c32B, const sU8 _a8) {

      const sU32 agA = (_c32A >> 8) & 0x00FF00FFu;
      const sU32 rbA = _c32A & 0x00FF00FFu;

      const sU32 agB = (_c32B >> 8) & 0x00FF00FFu;
      const sU32 rbB = _c32B & 0x00FF00FFu;

      sU32 ag = (agA + (((sU32)((sU32)(agB - agA))*_a8) >> 8)) & 0x00FF00FFu;
      sU32 rb = (rbA + (((sU32)((sU32)(rbB - rbA))*_a8) >> 8)) & 0x00FF00FFu;

      return (ag << 8) | rb;
   }

   inline sU32 fetchTexelBilinear(const sS32 uFix, const sS32 vFix) const {

      const sS32 texX = uFix >> 16;
      const sS32 texY = vFix >> 16;

      const sU32 tex32LT = fetchTexelClamp(texX,   texY  );
      const sU32 tex32RT = fetchTexelClamp(texX+1, texY  );
      const sU32 tex32RB = fetchTexelClamp(texX+1, texY+1);
      const sU32 tex32LB = fetchTexelClamp(texX,   texY+1);

      const sU32 ag_lt = (tex32LT >> 8) & 0x00FF00FFu;
      const sU32 rb_lt = tex32LT & 0x00FF00FFu;

      const sU32 ag_rt = (tex32RT >> 8) & 0x00FF00FFu;
      const sU32 rb_rt = tex32RT & 0x00FF00FFu;

      const sU32 ag_rb = (tex32RB >> 8) & 0x00FF00FFu;
      const sU32 rb_rb = tex32RB & 0x00FF00FFu;

      const sU32 ag_lb = (tex32LB >> 8) & 0x00FF00FFu;
      const sU32 rb_lb = tex32LB & 0x00FF00FFu;

      const sU8 fracU = sU8(uFix >> 8) & 255u;
      const sU8 fracV = sU8(vFix >> 8) & 255u;

      const sU32 ag_t = (ag_lt + (((sU32)((sU32)(ag_rt - ag_lt))*fracU) >> 8)) & 0x00FF00FFu;
      const sU32 rb_t = (rb_lt + (((sU32)((sU32)(rb_rt - rb_lt))*fracU) >> 8)) & 0x00FF00FFu;

      const sU32 ag_b = (ag_lb + (((sU32)((sU32)(ag_rb - ag_lb))*fracU) >> 8)) & 0x00FF00FFu;
      const sU32 rb_b = (rb_lb + (((sU32)((sU32)(rb_rb - rb_lb))*fracU) >> 8)) & 0x00FF00FFu;

      const sU32 ag_d = (ag_t + (((sU32)((sU32)(ag_b - ag_t))*fracV) >> 8)) & 0x00FF00FFu;
      const sU32 rb_d = (rb_t + (((sU32)((sU32)(rb_b - rb_t))*fracV) >> 8)) & 0x00FF00FFu;

      return (ag_d << 8) | rb_d;
   }

   inline sU32 fetchTexelNearest(const sS32 uFix, const sS32 vFix) const {
      const sS32 texX = uFix >> 16;
      const sS32 texY = vFix >> 16;

      if(texX < 0 || texX >= (sS32)texW)
         return 0u;

      if(texY < 0 || texY >= (sS32)texH)
         return 0u;

      return tex[texY * (sS32)texW + texX];
   }

   inline void setPixel(const sS32 ix, const sS32 iy,
                        const sS32 uFix, const sS32 vFix
                        ) {

#if TRITEX_BILINEAR
      const sU32 c32 = fetchTexelBilinear(uFix, vFix);
#else
      const sU32 c32 = fetchTexelNearest(uFix, vFix);
#endif // TRITEX_BILINEAR

      if(ix >= 0 && ix < (sS32)fbW &&
         iy >= 0 && iy < (sS32)fbH
         )
      {
         const sU32 fb32 = fb[iy * fbW + ix];
         const sU32 a = (a8 * (c32 >> 24)) >> 8;
         fb[iy * fbW + ix] = BlendC32(fb32, c32, a);
      }
   }

   inline void tritex_dda_fix_opt(sU32 *_fb, const sU32 _fbW, const sU32 _fbH,
                                  const sU32 *_tex, const sU32 _texW, const sU32 _texH,
                                  const tritexvec2_t v1, tritexvec2_t v2, tritexvec2_t v3,
                                  const tritexvec2_t v1Orig, const tritexvec2_t v2Orig, const tritexvec2_t v3Orig,
                                  const tritexvec2_t uv1Orig, const tritexvec2_t uv2Orig, const tritexvec2_t uv3Orig,
                                  const sU32 _a8
                                  ) {
      //       v1                      v1        //
      //       /\                      /\        // 
      //      /  \                    /  \       //
      //   v2/____\                  /____\v2    //
      //     \     \                /     /      //
      //       \    \              /    /        //
      //         \   \            /   /          //
      //           \  \          /  /            //
      //             \ \        / /              //
      //               \\v3    //v3              //

      fb  = _fb;
      fbW = _fbW;
      fbH = _fbH;

      tex  = _tex;
      texW = _texW;
      texH = _texH;

      a8 = _a8 + (_a8 >> 7);

      const sS32 v1x = Df2x(v1.x);
      const sS32 v1y = Df2x(v1.y);
      const sS32 v2x = Df2x(v2.x);
      const sS32 v2y = Df2x(v2.y);
      const sS32 v3y = Df2x(v3.y);

      // used for UV setup:
      //  (todo) dvx tilted in upside-down rotation (180°)
      //          (v3Origy == v1Origy) test fails since values are slightly off
      const sS32 v1Origx = Df2x(v1Orig.x);
      const sS32 v1Origy = Df2x(v1Orig.y);
      const sS32 v2Origx = Df2x(v2Orig.x);
      const sS32 v2Origy = Df2x(v2Orig.y);
      const sS32 v3Origx = Df2x(v3Orig.x);
      const sS32 v3Origy = Df2x(v3Orig.y);

      const sSI topRow    = (v1y + 1) >> 16;
      const sSI centerRow = (v2y + 1) >> 16;
      const sSI bottomRow = (v3y + 1) >> 16;

      sSI topNumRows    = centerRow - topRow + 1;
      sSI totalNumRows  = bottomRow - topRow + 1;
      sSI bottomNumRows = bottomRow - centerRow;

      // UV setup
      tritexvec2_t uv1Tex; uv1Tex.x = uv1Orig.x * texW; uv1Tex.y = uv1Orig.y * texH;
      tritexvec2_t uv2Tex; uv2Tex.x = uv2Orig.x * texW; uv2Tex.y = uv2Orig.y * texH;
      tritexvec2_t uv3Tex; uv3Tex.x = uv3Orig.x * texW; uv3Tex.y = uv3Orig.y * texH;
      sF32 dx, dy;
      sF32 my;
      sF32 dux, duy, dvx, dvy;
      tritexvec2_t uv4Tex;
      sF32 p4x;
      sF32 p4y;
      sF32 p4xNorm;
      // Set up dux / dvx
      if(v2Origy == v1Origy)
      {
         dx = v2Orig.x - v1Orig.x;
         if(0.0f != dx)
            dux = (uv2Tex.x - uv1Tex.x) / dx;
         else
            dux = 0.0f;
         if(0.0f != dx)
            dvx = (uv2Tex.y - uv1Tex.y) / dx;
         else
            dvx = 0.0f;
      }
      else if(v3Origy == v1Origy)
      {
         dx = v3Orig.x - v1Orig.x;
         if(0.0f != dx)
            dux = (uv3Tex.x - uv1Tex.x) / dx;
         else
            dux = 0.0f;
         if(0.0f != dx)
            dvx = (uv3Tex.y - uv1Tex.y) / dx;
         else
            dvx = 0.0f;
      }
      else if(v3Origy == v2Origy)
      {
         dx = v3Orig.x - v2Orig.x;
         if(0.0f != dx)
            dux = (uv3Tex.x - uv2Tex.x) / dx;
         else
            dux = 0.0f;
         if(0.0f != dx)
            dvx = (uv3Tex.y - uv2Tex.y) / dx;
         else
            dvx = 0.0f;
      }
      else
      {
         // Construct new vertex p4 on edge v2<>v3 that is located on the same y as v1
         if(0 == (v2Origx - v3Origx))
         {
            p4x = v3Orig.x;
            uv4Tex.x = uv3Tex.x;
            const sF32 d = (v3Orig.y - v2Orig.y);
            if(0.0f != d)
               p4xNorm = ((v1Orig.y - v2Orig.y) / d);
            else
               p4xNorm = 0.0f;
            uv4Tex.y = uv2Tex.y + ( (uv3Tex.y - uv2Tex.y) * p4xNorm );
         }
         else
         {
            const sF32 d = (v2Orig.x - v3Orig.x);
            if(0.0f != d)
            {
               my = (v2Orig.y - v3Orig.y) / d;
               if(0.0f != my)
                  p4x = v3Orig.x + ((v1Orig.y - v3Orig.y) / my);
               else
                  p4x = v3Orig.x;
            }
            else
            {
               my = 0.0f;
               p4x = v3Orig.x;
            }
            const sF32 d2 = (v2Orig.x - v3Orig.x);
            if(0.0f != d2)
               p4xNorm = ((v1Orig.y - v3Orig.y) / my) / d2;
            else
               p4xNorm = 0.0f;
            uv4Tex.x = uv3Tex.x + (uv2Tex.x - uv3Tex.x) * p4xNorm;
            uv4Tex.y = uv3Tex.y + (uv2Tex.y - uv3Tex.y) * p4xNorm;
         }
         dx = p4x - v1Orig.x;
         if(0.0f != dx)
            dux = (uv4Tex.x - uv1Tex.x) / dx;
         else
            dux = 0.0f;
         if(0.0f != dx)
            dvx = (uv4Tex.y - uv1Tex.y) / dx;
         else
            dvx = 0.0f;
      }

      if(v2Origx == v1Origx)
      {
         dy = v2Orig.y - v1Orig.y;
         if(0.0f != dy)
            duy = (uv2Tex.x - uv1Tex.x) / dy;
         else
            duy = 0.0f;
         if(0.0f != dy)
            dvy = (uv2Tex.y - uv1Tex.y) / dy;
         else
            dvy = 0.0f;
      }
      else if(v3Origx == v1Origx)
      {
         dy = v3Orig.y - v1Orig.y;
         if(0.0f != dy)
            duy = (uv3Tex.x - uv1Tex.x) / dy;
         else
            duy = 0.0f;
         if(0.0f != dy)
            dvy = (uv3Tex.y - uv1Tex.y) / dy;
         else
            dvy = 0.0f;
      }
      else if(v3Origx == v2Origx)
      {
         dy = v3Orig.y - v2Orig.y;
         if(0.0f != dy)
            duy = (uv3Tex.x - uv2Tex.x) / dy;
         else
            duy = 0.0f;
         if(0.0f != dy)
            dvy = (uv3Tex.y - uv2Tex.y) / dy;
         else
            dvy = 0.0f;
      }
      else
      {
         // Construct new vertex p4 on edge v1<>v2 that is located on the same x as v3
         if(0 == (v2Origx - v3Origx))
            my = 0;
         else
         {
            const sF32 d = (v2Orig.x - v1Orig.x);
            if(0.0f != d)
               my = (v2Orig.y - v1Orig.y) / d;
            else
               my = 0.0f;
         }
         p4y = v1Orig.y + (v3Orig.x - v1Orig.x) * my;
         const sF32 d2 = (v2Orig.x - v1Orig.x);
         if(0.0f != d2)
            p4xNorm = (v3Orig.x-v1Orig.x) / d2;
         else
            p4xNorm = 0.0f;
         uv4Tex.x = uv1Tex.x + (uv2Tex.x - uv1Tex.x) * p4xNorm;
         uv4Tex.y = uv1Tex.y + (uv2Tex.y - uv1Tex.y) * p4xNorm;
         dy = v3Orig.y - p4y;
         if(0.0f != dy)
         {
            duy = (uv3Tex.x - uv4Tex.x) / dy;
            dvy = (uv3Tex.y - uv4Tex.y) / dy;
         }
         else
         {
            duy = 0.0f;
            dvy = 0.0f;
         }
      }

      const sF32 v1TopDeltaY = (topRow - v1Orig.y);
            sS32 uYFix  = Df2x(v1TopDeltaY * duy);
            sS32 vYFix  = Df2x(v1TopDeltaY * dvy);
      const sS32 duyFix = Df2x(duy);
      const sS32 dvyFix = Df2x(dvy);
      const sS32 duxFix = Df2x(dux);
      const sS32 dvxFix = Df2x(dvx);
      const sS32 uv1TexXFix = Df2x(uv1Tex.x);
      const sS32 uv1TexYFix = Df2x(uv1Tex.y);

      // Set up edge functions
      sS32 v2EdgeX = v1x;
      sS32 v3EdgeX = v1x;

      sS32 v2StepX = (topNumRows > 1) ? Df2x( (v2.x - v1.x) / (topNumRows-1) ) : 0;
      sS32 v3StepX = (totalNumRows > 1) ? Df2x( (v3.x - v1.x) / (totalNumRows-1) ) : 0;

      sSI iy = topRow;

      if(topRow == centerRow)
      {
         // Corner case: 1 pixel top row (e.g. axis aligned rectangles)
         v2EdgeX = v2x;

         // top/left fill rule: draw top row only when frac(v2.y) < 0.5
         if(0 != (v2y & 0x8000))
         {
            iy++;
            v2EdgeX += v2StepX;
            v3EdgeX += v3StepX;
            uYFix += duyFix;
            vYFix += dvyFix;
            topNumRows--;
         }
      }

      if(centerRow == bottomRow)
      {
         // top/left fill rule: draw bottom row only when frac(v3.y) >= 0.5
         //  (note) bottomNumRows is 0
         if(0 == (v3y & 0x8000))
            topNumRows--;
      }

      sS32 iRightX;
      sS32 leftX;
      sS32 rightX;
      sS32 ix;
      sSI u,v;

      for(sSI topRowIdx = 0; topRowIdx < topNumRows; topRowIdx++)
      {
         if(v2EdgeX > v3EdgeX)
         {
            leftX = v3EdgeX;
            rightX = v2EdgeX;
         }
         else
         {
            leftX = v2EdgeX;
            rightX = v3EdgeX;
         }

         // top/left fill rule: draw first pixel only if leftX < 0.5, otherwise proceed to next pixel
         //  (note) top rule "cy < 0.5" only applies to horizontal top edges
         leftX = leftX + (leftX & 0x8000);

         // top/left fill rule: draw last pixel only if rightX >= 0.5, otherwise proceed to next pixel
         //  (note) top rule "cy >= 0.5" only applies to horizontal bottom edges
         rightX = rightX + (rightX & 0x8000);

         ix      = leftX >> 16;
         iRightX = rightX >> 16;

         u = uv1TexXFix + (sSI)((ix - v1Orig.x) * duxFix) + uYFix;
         v = uv1TexYFix + (sSI)((ix - v1Orig.x) * dvxFix) + vYFix;

         while(ix < iRightX)
         {
            setPixel(ix, iy, u, v);

            ix++;
            u += duxFix;
            v += dvxFix;
         }

         v2EdgeX += v2StepX;
         v3EdgeX += v3StepX;
         uYFix += duyFix;
         vYFix += dvyFix;
         iy++;
      }

      // Bottom area
      if(centerRow == bottomRow)
         return;

      v2EdgeX = v2x;
      v2StepX = (0 != bottomNumRows) ? Df2x( (v3.x - v2.x) / bottomNumRows ) : 0;
      v2EdgeX += v2StepX;

      // top/left fill rule: draw bottom row only when frac(v3.y) >= 0.5
      if(0 == (v3y & 0x8000))
         bottomNumRows--;

      for(sSI bottomRowIdx = 0; bottomRowIdx < bottomNumRows; bottomRowIdx++)
      {
         if(v2EdgeX > v3EdgeX)
         {
            leftX = v3EdgeX;
            rightX = v2EdgeX;
         }
         else
         {
            leftX = v2EdgeX;
            rightX = v3EdgeX;
         }

         // top/left fill rule: draw first pixel only if leftX < 0.5, otherwise proceed to next pixel
         //  (note) top rule "cy < 0.5" only applies to horizontal top edges
         leftX = leftX + (leftX & 0x8000);

         // top/left fill rule: draw last pixel only if rightX >= 0.5, otherwise proceed to next pixel
         //  (note) top rule "cy >= 0.5" only applies to horizontal bottom edges
         rightX = rightX + (rightX & 0x8000);

         ix  = leftX >> 16;
         iRightX = rightX >> 16;

         u = uv1TexXFix + (sSI)((ix - v1Orig.x) * duxFix) + uYFix;
         v = uv1TexYFix + (sSI)((ix - v1Orig.x) * dvxFix) + vYFix;

         while(ix < iRightX)
         {
            setPixel(ix, iy, u, v);

            u += duxFix;
            v += dvxFix;
            ix++;
         }

         v2EdgeX += v2StepX;
         v3EdgeX += v3StepX;
         uYFix += duyFix;
         vYFix += dvyFix;
         iy++;
      }

   }

};

#undef Df2x


#endif // TKS_TRI_TEX_FX_H__
