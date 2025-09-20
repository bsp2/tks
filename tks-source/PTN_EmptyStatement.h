/// PTN_EmptyStatement.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_EMPTYSTATEMENT_H__
#define __PTN_EMPTYSTATEMENT_H__

 
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
 

#endif // __PTN_EMPTYSTATEMENT_H__
