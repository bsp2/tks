/// PTN_FunctionSignalStatement.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_FUNCTIONSIGNALSTATEMENT_H__
#define __PTN_FUNCTIONSIGNALSTATEMENT_H__


class PTN_FunctionSignalStatement : public PTN_Statement {
   //
   // Note: if both function_object and cached_object are NULL, autobind mode is in effect ("use callbacks;")
   //
public:
   TKS_FunctionObject  *function_object;  // ro reference to function wrapped in YAC_Object
   TKS_CachedObject    *cached_object;    // ro reference to cached_object that stores object
   sU8                  signal_id;

public:
   PTN_FunctionSignalStatement(void);
   PTN_FunctionSignalStatement(TKS_FunctionObject *_fun, sU8 _sigid, TKS_CachedObject *_co);
   ~PTN_FunctionSignalStatement();
   
   sBool   semanticCheck (void);
   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
};


#endif // __PTN_FUNCTIONSIGNALSTATEMENT_H__
