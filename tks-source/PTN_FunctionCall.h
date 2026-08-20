// PTN_FunctionCall.h
//
// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
//     - distributed under terms of the Lesser GNU General Public License (LGPL)
//

#ifndef PTN_FUNCTIONCALL_H__
#define PTN_FUNCTIONCALL_H__


class PTN_FunctionCall : public PTN_Call {
public:
	PTN_FunctionCall(PTN_CallableExpr *_expr);

	void    eval      (PTN_Env *_env) const;
	Fevalst getEvalSt (void) const;
};


#endif // PTN_FUNCTIONCALL_H__
