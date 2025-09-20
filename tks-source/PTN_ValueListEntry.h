/// PTN_ValueListEntry.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_VALUELISTENTRY_H__
#define __PTN_VALUELISTENTRY_H__


class PTN_ValueListEntry : public PTN_Node {
public: 
	PTN_Expr *          expr;
	PTN_ValueListEntry *next;
	Feval               expr_opt;

	PTN_ValueListEntry  (PTN_Expr *_expr);
	~PTN_ValueListEntry ();
};


#endif // __PTN_VALUELISTENTRY_H__
