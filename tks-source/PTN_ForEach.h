/// PTN_ForEach.h
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_FOREACH_H__
#define __PTN_FOREACH_H__


class PTN_ForEach : public PTN_BreakableStatement {
public:
	PTN_Statement    *body;
	TKS_CachedObject *cvar;
	PTN_Expr         *obj_expr;
	Feval             obj_expr_opt;
	sBool             b_dynamicvar;
   sBool             break_tag; // 1=body uses break;

public:
	PTN_ForEach  (void);
	void init    (TKS_CachedObject *_cvar, PTN_Expr *_objexpr, PTN_Statement *_body);
	~PTN_ForEach ();

	void    eval           (PTN_Env *_env) const;
	Fevalst getEvalSt      (void) const;
	sUI     getID          (void) const;
	sBool   semanticCheck  (void);
	void    subGenCallList (void);
	void    optimize       (void);
	sBool   resolveXRef    (void);
   void    useBreak       (void);
};


#endif // __PTN_FOREACH_H__
