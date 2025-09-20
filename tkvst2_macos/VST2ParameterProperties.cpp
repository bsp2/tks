/// VST2ParameterProperties.cpp
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

#include "tkvst2.h"

#include "VST2ParameterProperties.h"


VST2ParameterProperties::VST2ParameterProperties(void) {
   memset(&properties, 0, sizeof(VstParameterProperties));
}

VST2ParameterProperties::~VST2ParameterProperties() {
}

void VST2ParameterProperties::GetString(const char *_cstring, sSI _maxLen, YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->alloc(_maxLen + 1);

   s->copy(_cstring);
  
   s->fixLength();
   
   _r->initString(s, 1);
}

void VST2ParameterProperties::getLabel(YAC_Value *_r) {
   GetString(properties.label, kVstMaxLabelLen, _r);
}

void VST2ParameterProperties::getShortLabel(YAC_Value *_r) {
   GetString(properties.shortLabel, kVstMaxShortLabelLen, _r);
}

sUI VST2ParameterProperties::getFlags(void) {
   return properties.flags;
}

sBool VST2ParameterProperties::isSwitch(void) {
   return ((properties.flags & kVstParameterIsSwitch) == kVstParameterIsSwitch);
}

sBool VST2ParameterProperties::usesIntegerMinMax(void) {
   return ((properties.flags & kVstParameterUsesIntegerMinMax) == kVstParameterUsesIntegerMinMax);
}

sBool VST2ParameterProperties::usesFloatStep(void) {
   return ((properties.flags & kVstParameterUsesFloatStep) == kVstParameterUsesFloatStep);
}

sBool VST2ParameterProperties::usesIntStep(void) {
   return ((properties.flags & kVstParameterUsesIntStep) == kVstParameterUsesIntStep);
}

sBool VST2ParameterProperties::supportsDisplayIndex(void) {
   return ((properties.flags & kVstParameterSupportsDisplayIndex) == kVstParameterSupportsDisplayIndex);
}

sBool VST2ParameterProperties::supportsDisplayCategory(void) {
   return ((properties.flags & kVstParameterSupportsDisplayCategory) == kVstParameterSupportsDisplayCategory);
}

sBool VST2ParameterProperties::canRamp(void) {
   return ((properties.flags & kVstParameterCanRamp) == kVstParameterCanRamp);
}

sF32 VST2ParameterProperties::getStepFloat(void) {
   return properties.stepFloat;
}

sF32 VST2ParameterProperties::getSmallStepFloat(void) {
   return properties.smallStepFloat;
}

sF32 VST2ParameterProperties::getLargeStepFloat(void) {
   return properties.largeStepFloat;
}

sSI VST2ParameterProperties::getMinInteger(void) {
   return properties.minInteger;
}

sSI VST2ParameterProperties::getMaxInteger(void) {
   return properties.maxInteger;
}

sSI VST2ParameterProperties::getStepInteger(void) {
   return properties.stepInteger;
}

sSI VST2ParameterProperties::getLargeStepInteger(void) {
   return properties.largeStepInteger;
}

sSI VST2ParameterProperties::getDisplayIndex(void) {
   return properties.displayIndex;
}

sSI  VST2ParameterProperties::getCategory(void) {
   return properties.category;
}

sSI VST2ParameterProperties::getNumParametersInCategory(void) {
   return properties.numParametersInCategory;
}

void VST2ParameterProperties::getCategoryLabel(YAC_Value *_r) {
   GetString(properties.categoryLabel, kVstMaxCategLabelLen, _r);
}

sF32 VST2ParameterProperties::convertNormalizedValue(sF32 _norm) {
   if(isSwitch())
   {
      return (_norm < 0.5f) ? 0.0f : 1.0f;
   }
   else if(usesIntegerMinMax())
   {
      return properties.minInteger + _norm * (properties.maxInteger - properties.minInteger);
   }
   else
   {
      // 0..1 float range
      return _norm;
   }
}
