/// PTN_VarStatement.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_VARSTATEMENT_H__
#define PTN_VARSTATEMENT_H__


class PTN_VarStatement : public PTN_Statement {
public:
   TKS_CachedObject *cached_object;

public:
   PTN_VarStatement  (void);
   PTN_VarStatement  (TKS_CachedObject *_var);
   ~PTN_VarStatement ();

   sBool semanticCheck (void);
};


#endif // PTN_VARSTATEMENT_H__
