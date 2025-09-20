/// PTN_VarDecl.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef __PTN_VARDECL_H__
#define __PTN_VARDECL_H__


class PTN_VarDecl : public PTN_VarAssign {
public:
   PTN_VarDecl(void);
   PTN_VarDecl(TKS_CachedObject *_co, PTN_Expr *_expr);
   ~PTN_VarDecl();
   
   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
   sBool   semanticCheck (void);
   void    optimize      (void);
   sBool   resolveXRef   (void);
};


#endif // __PTN_VARDECL_H__
