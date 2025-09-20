/// color.h
///
/// (c) 1999-2006 Bastian Spiegel <bs@tkscript.de> 
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///

#ifndef __TKS_COLOR_H__
#define __TKS_COLOR_H__

YG("OldMath");

YC class _Color : public YAC_Object {
public:
    sF32  rf; 
    sF32  gf;
    sF32  bf;
    sF32  af;
    sF32  gamma;
    sUI   pi;
    sBool recalcpi;

public:
    _Color(void);
    ~_Color();
    YAC(_Color);

	void YAC_VCALL yacSerialize(YAC_Object *, sUI);
	sUI  YAC_VCALL yacDeserialize(YAC_Object *, sUI);

	sUI  recalcPi(void);
/* 	sUI  getColor32(void); */

    void YAC_VCALL yacOperator(sSI, YAC_Object*, YAC_Value *);

    YM sUI  _getColori     (void);
    YM sUI  _setColori     (sSI _pi);
    YM sUI  _getColor4b    (void);
    YM sUI  _setRgbpi      (sSI _pi);
    YM sUI  _init4i        (sSI _a, sSI _r, sSI _g, sSI _b);
    YM sUI  _init4f        (sF32 _a, sF32 _r, sF32 _g, sF32 _b);
    YM sUI  _init3f        (sF32 _r, sF32 _g, sF32 _b);
    YM sUI  _initGamma3f   (sF32 _r, sF32 _g, sF32 _b, sF32 _gamma);
    YM sUI  _getRi         (void);
    YM sUI  _getGi         (void);
    YM sUI  _getBi         (void);
    YM sUI  _getAi         (void);
    YM sF32 _getRf         (void);
    YM sF32 _getGf         (void);
    YM sF32 _getBf         (void);
    YM sF32 _getAf         (void);
    YM void _setAi         (sSI _a);
    YM void _setAf         (sF32 _a);
    YM void _setRi         (sSI _ri);
    YM void _setRf         (sF32 _r);
    YM void _setGi         (sSI _g);
    YM void _setGf         (sF32 _g);
    YM void _getString     (YAC_Value*);
    YM void _setBi         (sSI _b);
    YM void _setBf         (sF32 _b);
    YM sF32 _getGamma      (void);
    YM void _setGamma      (sF32 _gamma);
    YM void _glColor       (void);
    YM sUI  _getBlendf     (sF32 _blend);
    YM sUI  _getGammaf     (sF32 _gamma);
    YM sUI  _mixf          (YAC_Object *_c, sF32 _camount);
    YM void _glColorGamma  (sF32);
	YM void _glLightMaterialAmbient           (sSI);
	YM void _glLightMaterialDiffuse           (sSI);
	YM void _glLightMaterialAmbientAndDiffuse (sSI);
	YM void _glLightMaterialSpecular          (sSI);
	YM void _glLightMaterialEmission          (sSI);

	sF32 *getRGBAfv(void);
};

#endif
