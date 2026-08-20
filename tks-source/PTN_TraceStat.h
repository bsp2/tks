/// PTN_TraceStat.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_TRACESTAT_H__
#define PTN_TRACESTAT_H__


class PTN_TraceStat : public PTN_Statement {
public:
   sUI getID (void) const { return PTN_TRACESTAT; }
};


#endif // PTN_TRACESTAT_H__
