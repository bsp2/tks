/// VSTPinProperties.cpp
///
/// (c) 2010 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 02Oct2010
/// changed: 
///
///
///

#include "tkvst2.h"

#include "VST2PinProperties.h"


VST2PinProperties::VST2PinProperties(void) {
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
   //GetString(properties.label, kVstMaxLabelLen, _r);
}

sUI VST2PinProperties::getFlags(void) {
   return 0;//properties.flags;
}

sUI VST2PinProperties::getArrangementType(void) {
   return 0;//properties.arrangementType;
}

void VST2PinProperties::getShortLabel(YAC_Value *_r) {
   //GetString(properties.shortLabel, kVstMaxShortLabelLen, _r);
}

sBool VST2PinProperties::isActive(void) {
   return 0;//(kVstPinIsActive == (properties.flags & kVstPinIsActive));
}

sBool VST2PinProperties::isStereo(void) {
   return 0;//(kVstPinIsStereo == (properties.flags & kVstPinIsStereo));
}

sBool VST2PinProperties::useSpeaker(void) {
   return 0;//(kVstPinUseSpeaker == (properties.flags & kVstPinUseSpeaker));
}


