/// VSTParameterProperties.cpp
///
/// (c) 2010 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 10Oct2010
/// changed: 
///
///
///

#include "tkvst2.h"

#include "VST2ParameterProperties.h"


VST2ParameterProperties::VST2ParameterProperties(void) {
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
}

void VST2ParameterProperties::getShortLabel(YAC_Value *_r) {
}

sUI VST2ParameterProperties::getFlags(void) {
   return 0;
}

sBool VST2ParameterProperties::isSwitch(void) {
   return 0;
}

sBool VST2ParameterProperties::usesIntegerMinMax(void) {
   return 0;
}

sBool VST2ParameterProperties::usesFloatStep(void) {
   return 0;
}

sBool VST2ParameterProperties::usesIntStep(void) {
   return 0;
}

sBool VST2ParameterProperties::supportsDisplayIndex(void) {
   return 0;
}

sBool VST2ParameterProperties::supportsDisplayCategory(void) {
   return 0;
}

sBool VST2ParameterProperties::canRamp(void) {
   return 0;
}

sF32 VST2ParameterProperties::getStepFloat(void) {
   return 0.1f;
}

sF32 VST2ParameterProperties::getSmallStepFloat(void) {
   return 0.01f;
}

sF32 VST2ParameterProperties::getLargeStepFloat(void) {
   return 0.2f;
}

sSI VST2ParameterProperties::getMinInteger(void) {
   return 0;
}

sSI VST2ParameterProperties::getMaxInteger(void) {
   return 0;
}

sSI VST2ParameterProperties::getStepInteger(void) {
   return 0;
}

sSI VST2ParameterProperties::getLargeStepInteger(void) {
   return 0;
}

sSI VST2ParameterProperties::getDisplayIndex(void) {
   return 0;
}

sSI  VST2ParameterProperties::getCategory(void) {
   return 0;
}

sSI VST2ParameterProperties::getNumParametersInCategory(void) {
   return 0;
}

void VST2ParameterProperties::getCategoryLabel(YAC_Value *_r) {
}

sF32 VST2ParameterProperties::convertNormalizedValue(sF32 _norm) {
   return 0.0f;
}

