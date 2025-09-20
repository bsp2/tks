/// PTN_TraceStat.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef __PTN_TRACESTAT_H__
#define __PTN_TRACESTAT_H__


class PTN_TraceStat : public PTN_Statement {
public:
   sUI getID (void) const { return PTN_TRACESTAT; }
};


#endif // __PTN_TRACESTAT_H__
