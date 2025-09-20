/// PTN_ValueListExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_VALUELISTEXPR_H__
#define __PTN_VALUELISTEXPR_H__


class PTN_ValueListExpr : public PTN_Expr {
public:
	YAC_ListNode      *value_list;
	PTN_ValueListEntry *first_entry;
	PTN_ValueListEntry *last_entry;
	sBool              is_const;
   sBool              b_always_new;

public:
	PTN_ValueListExpr  (sBool _bAlwaysNew);
	~PTN_ValueListExpr (void);
	
	void        addExpr           (PTN_Expr *_expr);
	sBool       semanticCheck     (void);
	sBool       resolveXRef       (void);
	void        optimize          (void);
	void        eval              (PTN_Env *_env, YAC_Value *) const;
	Feval       getEval           (void) const;
	void        eval              (PTN_Env *_env, YAC_Value *, sBool _const);
	void        evalConst         (YAC_Value *);
	sBool       isConst           (void);
};


#endif // __PTN_VALUELISTEXPR_H__
