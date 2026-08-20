/// PTN_Throw.h
///
/// (c) 2006-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
#ifndef PTN_THROW_H__
#define PTN_THROW_H__


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


#endif // PTN_THROW_H__
