/// PTN_SwitchDefaultCase.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_SWITCHDEFAULTCASE_H__
#define __PTN_SWITCHDEFAULTCASE_H__


class PTN_SwitchDefaultCase : public PTN_SwitchCase {
public:
   PTN_SwitchDefaultCase(void);
   PTN_SwitchDefaultCase(PTN_Expr *, PTN_Statement *, sBool _doBreak);
   
   sUI   getID         (void) const;
   sBool semanticCheck (void); // The default case is valid without an expr
};


#endif // __PTN_SWITCHDEFAULTCASE_H__
