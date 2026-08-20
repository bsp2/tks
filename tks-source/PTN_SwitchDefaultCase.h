/// PTN_SwitchDefaultCase.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_SWITCHDEFAULTCASE_H__
#define PTN_SWITCHDEFAULTCASE_H__


class PTN_SwitchDefaultCase : public PTN_SwitchCase {
public:
   PTN_SwitchDefaultCase(void);
   PTN_SwitchDefaultCase(PTN_Expr *, PTN_Statement *, sBool _doBreak);

   sUI   getID         (void) const;
   sBool semanticCheck (void); // The default case is valid without an expr
};


#endif // PTN_SWITCHDEFAULTCASE_H__
