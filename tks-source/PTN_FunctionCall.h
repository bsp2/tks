// PTN_FunctionCall.h
//
// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
//     - distributed under the terms of the GNU general public license (GPL).
//
#ifndef __PTN_FUNCTIONCALL_H__
#define __PTN_FUNCTIONCALL_H__


class PTN_FunctionCall : public PTN_Call {
public:
	PTN_FunctionCall(PTN_CallableExpr *_expr);

	void    eval      (PTN_Env *_env) const;
	Fevalst getEvalSt (void) const;
};


#endif // __PTN_FUNCTIONCALL_H__
