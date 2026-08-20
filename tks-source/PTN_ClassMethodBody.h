/// PTN_ClassMethodBody.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
#ifndef PTN_CLASSMETHODBODY_H__
#define PTN_CLASSMETHODBODY_H__


class PTN_ClassMethodBody : public PTN_Statement {
public:
    sUI getID (void) const { return PTN_CLASSMETHODBODY; };
};


#endif // PTN_CLASSMETHODBODY_H__
