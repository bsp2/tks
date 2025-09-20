/// PTN_ClassMethodBody.h
///
/// (c) 2001-2005 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMETHODBODY_H__
#define __PTN_CLASSMETHODBODY_H__

 
class PTN_ClassMethodBody : public PTN_Statement {
public:
    sUI getID (void) const { return PTN_CLASSMETHODBODY; };
};
 

#endif // __PTN_CLASSMETHODBODY_H__
