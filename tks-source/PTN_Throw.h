/// PTN_Throw.h
///
/// (c) 2006-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_THROW_H__
#define __PTN_THROW_H__


class PTN_Throw : public PTN_Statement {
public:
   TKS_ExceptionType *exception_type; // NULL == re-throw current exception
   PTN_Expr          *msg_expr;
   Feval              msg_expr_opt;
   
public:
   PTN_Throw(void);
   ~PTN_Throw();
   
   sBool   semanticCheck (void);
   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
   void    optimize      (void);
   sBool   resolveXRef   (void);
};


#endif // __PTN_THROW_H__
