/// PTN_VarDecl.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_VARDECL_H__
#define PTN_VARDECL_H__


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


#endif // PTN_VARDECL_H__
