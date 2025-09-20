/// VSTPinProperties.h
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

#ifndef __VST2PINPROPERTIES_H__
#define __VST2PINPROPERTIES_H__


YG("vst2");


YC class VST2PinProperties : public YAC_Object {
public:

public:
   YAC(VST2PinProperties);

   VST2PinProperties(void);
   ~VST2PinProperties();

  protected:
   static void GetString(const char *_cstring, sSI _maxLen, YAC_Value *_r);

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
