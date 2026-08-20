/// TKS_SignalEntry.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_SIGNALENTRY_H__
#define TKS_SIGNALENTRY_H__


class TKS_SignalEntry {
   //
   // see YAC_Object::yacGetSignalStringList()
   //
public:
   YAC_String name;
   sUI        rtti;
};


#endif // TKS_SIGNALENTRY_H__
