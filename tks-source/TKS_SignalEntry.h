/// TKS_SignalEntry.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_SIGNALENTRY_H__
#define __TKS_SIGNALENTRY_H__


class TKS_SignalEntry {
   //
   // see YAC_Object::yacGetSignalStringList()
   //
public:
   YAC_String name;
   sUI        rtti;
};


#endif // __TKS_SIGNALENTRY_H__
