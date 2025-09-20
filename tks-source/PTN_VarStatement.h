/// PTN_VarStatement.h
/// (c) 2001-2005 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_VARSTATEMENT_H__
#define __PTN_VARSTATEMENT_H__


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                   ~
//~ PTN_VarStatement                  ~
//~                                   ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class PTN_VarStatement : public PTN_Statement {
public:
   TKS_CachedObject *cached_object; 

public:
   PTN_VarStatement  (void);
   PTN_VarStatement  (TKS_CachedObject *_var);
   ~PTN_VarStatement ();
   
   sBool semanticCheck (void);
};



#endif
