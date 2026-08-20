/// PTN_EmptyStatement.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_EMPTYSTATEMENT_H__
#define PTN_EMPTYSTATEMENT_H__


class PTN_EmptyStatement : public PTN_Statement {
public:
#ifdef TKS_JIT
   sBool forceHybrid(void);
#endif
};

// ---- used for enum
class PTN_EmptyStatementNoTerm : public PTN_EmptyStatement {
public:
   sUI getID (void) const;
};


#endif // PTN_EMPTYSTATEMENT_H__
