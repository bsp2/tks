/// tkoldmath.h
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL)
///
///   Note: These APIs are kind of deprecated. Originally, they were part of the tkopengl plugin.
///

#ifndef __TKOLDMATH_H__
#define __TKOLDMATH_H__


YG("OldMath");


YF YAC_Object *YAC_CALL _ivector                (sSI, sSI);
YF YAC_Object *YAC_CALL _vector                 (sF32, sF32, sF32);
YF YAC_Object *YAC_CALL _vector4                (sF32, sF32, sF32, sF32);
YF sUI         YAC_CALL _mathC32Multiply        (sUI, sUI, sF32 _bscale);
YF sUI         YAC_CALL _mathC32Blend           (sUI, sUI, sF32 _bscale);
YF void        YAC_CALL _mathNormal             (YAC_Object *, YAC_Object *, YAC_Object *, YAC_Object *);
YF void        YAC_CALL _mathUnproject          (YAC_Object *, YAC_Object *, YAC_Object *);
YF void        YAC_CALL _mathProject            (YAC_Object *, YAC_Object *);
YF void        YAC_CALL _mathReflect            (YAC_Object *, YAC_Object *, YAC_Object *);
YF sF32        YAC_CALL _mathDistancePointPlane (YAC_Object *, YAC_Object *, YAC_Object *);
YF void        YAC_CALL _mathVectorMin          (YAC_Object *, YAC_Object *, YAC_Object *);
YF void        YAC_CALL _mathVectorMax          (YAC_Object *, YAC_Object *, YAC_Object *);
YF sUI         YAC_CALL _mathC32ToC8            (sUI);

YF void        YAC_CALL _zglLoadMatrix          (YAC_Object *, YAC_Object *);
YF void        YAC_CALL _zglLightPositionv      (sSI, YAC_Object *, sF32);
YF void        YAC_CALL _zglLightSpotDirectionv (sSI, YAC_Object *);
YF void        YAC_CALL _zglNormalv             (YAC_Object *);
YF void        YAC_CALL _zglRotatev             (YAC_Object *);
YF void        YAC_CALL _zglRotate3f            (sF32, sF32, sF32);
YF void        YAC_CALL _zglScalev              (YAC_Object *);
YF void        YAC_CALL _zglStoreMatrix         (YAC_Object *, YAC_Object *);
YF void        YAC_CALL _zglTranslatev          (YAC_Object *);
////YF void        YAC_CALL _zgluLookAt             (YAC_Object *, YAC_Object *, YAC_Object *);
YF void        YAC_CALL _zglVertexv             (YAC_Object *);


extern tkopengl_shared_t *tkopengl_shared;
extern void tkopengl_shared_lazy_resolve(void);

#endif // __TKOLDMATH_H__
