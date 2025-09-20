/// VST2PinProperties.h
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

#ifndef __VST2PINPROPERTIES_H__
#define __VST2PINPROPERTIES_H__


YG("vst2");


YC class VST2PinProperties : public YAC_Object {
public:
   VstPinProperties properties;

public:
   YAC(VST2PinProperties);

   VST2PinProperties(void);
   ~VST2PinProperties();

  protected:
   static void GetString(const char *_cstring, sSI _maxLen, YAC_Value *_r);

  public:
   void initFallbackPin(sBool _bStereo);

public:

   YM void getLabel           (YAC_Value *_r);
   YM sUI  getFlags           (void);
   YM sUI  getArrangementType (void);
   YM void getShortLabel      (YAC_Value *_r);

   YM sBool isActive   (void);
   YM sBool isStereo   (void);
   YM sBool useSpeaker (void);
};


#endif // __VST2PINPROPERTIES_H__
