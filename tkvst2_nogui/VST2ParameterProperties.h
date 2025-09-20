/// VST2ParameterProperties.h
///
/// (c) 2010-2019 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 10Oct2010
/// changed: 26Aug2019
///
///
///

#ifndef __VST2PARAMETERPROPERTIES_H__
#define __VST2PARAMETERPROPERTIES_H__


YG("vst2");


YC class VST2ParameterProperties : public YAC_Object {
public:
   VstParameterProperties properties;

public:
   YAC(VST2ParameterProperties);

   VST2ParameterProperties(void);
   ~VST2ParameterProperties();

  protected:
   static void GetString(const char *_cstring, sSI _maxLen, YAC_Value *_r);

public:

   YM void getLabel           (YAC_Value *_r);
   YM void getShortLabel      (YAC_Value *_r);

   YM sUI   getFlags                (void);
   YM sBool isSwitch                (void);
   YM sBool usesIntegerMinMax       (void);
   YM sBool usesFloatStep           (void);
   YM sBool usesIntStep             (void);
   YM sBool supportsDisplayIndex    (void);
   YM sBool supportsDisplayCategory (void);
   YM sBool canRamp                 (void);

   YM sF32  getStepFloat      (void);
   YM sF32  getSmallStepFloat (void);
   YM sF32  getLargeStepFloat (void);

   YM sSI  getMinInteger       (void);
   YM sSI  getMaxInteger       (void);
   YM sSI  getStepInteger      (void);
   YM sSI  getLargeStepInteger (void);

   YM sSI getDisplayIndex (void);

   YM sSI  getCategory                (void);
   YM sSI  getNumParametersInCategory (void);
   YM void getCategoryLabel           (YAC_Value *_r);

   YM sF32 convertNormalizedValue (sF32 _norm);
};


#endif // __VST2PARAMETERPROPERTIES_H__
