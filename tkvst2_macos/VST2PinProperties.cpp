/// VST2PinProperties.cpp
///
/// (c) 2010-2019 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 02Oct2010
/// changed: 07Mar2011, 26Aug2019
///
///
///

#include "tkvst2.h"

#include "VST2PinProperties.h"


VST2PinProperties::VST2PinProperties(void) {
   memset(&properties, 0, sizeof(VstPinProperties));
}

VST2PinProperties::~VST2PinProperties() {
}

void VST2PinProperties::GetString(const char *_cstring, sSI _maxLen, YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->alloc(_maxLen + 1);

   s->copy(_cstring);
  
   s->fixLength();
   
   _r->initString(s, 1);
}

void VST2PinProperties::getLabel(YAC_Value *_r) {
   GetString(properties.label, kVstMaxLabelLen, _r);
}

sUI VST2PinProperties::getFlags(void) {
   return properties.flags;
}

sUI VST2PinProperties::getArrangementType(void) {
   return properties.arrangementType;
}

void VST2PinProperties::getShortLabel(YAC_Value *_r) {
   GetString(properties.shortLabel, kVstMaxShortLabelLen, _r);
}

sBool VST2PinProperties::isActive(void) {
   return (kVstPinIsActive == (properties.flags & kVstPinIsActive));
}

sBool VST2PinProperties::isStereo(void) {
   return (kVstPinIsStereo == (properties.flags & kVstPinIsStereo));
}

sBool VST2PinProperties::useSpeaker(void) {
   return (kVstPinUseSpeaker == (properties.flags & kVstPinUseSpeaker));
}

void VST2PinProperties::initFallbackPin(sBool _bStereo) {
   memset(&properties, 0, sizeof(VstPinProperties));

   if(_bStereo)
      properties.flags |= kVstPinIsStereo;
}
