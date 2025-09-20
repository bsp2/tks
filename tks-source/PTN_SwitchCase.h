/// PTN_SwitchCase.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#ifndef PTN_SWITCHCASE_H__
#define PTN_SWITCHCASE_H__


class PTN_SwitchCase : public PTN_Node {
public:
	PTN_SwitchCase *prev;
	PTN_SwitchCase *next;
	PTN_Expr       *expr;      // NULL when default case
	PTN_Statement  *body;
	Feval           expr_opt;
   sBool           do_break;
   sBool           b_beq;     // 'case @(addr):' optimization

public:
	PTN_SwitchCase(void);
	PTN_SwitchCase(PTN_Expr *, PTN_Statement *, sBool _doBreak);
	~PTN_SwitchCase();

	sBool semanticCheck  (void);
	////void  eval           (PTN_Env *_env); // is evaluated in PTN_Switch.cpp
	void  subGenCallList (void);
	void  optimizeBEQ    (void);  // allow 'case @(addr)' optimization after switch(@(addr))
	void  optimize       (void);
	sBool resolveXRef    (void);
};


#endif // PTN_SWITCHCASE_H__
