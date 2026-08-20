/// PTN_ValueListEntry.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_VALUELISTENTRY_H__
#define PTN_VALUELISTENTRY_H__


class PTN_ValueListEntry : public PTN_Node {
public:
	PTN_Expr *          expr;
	PTN_ValueListEntry *next;
	Feval               expr_opt;

	PTN_ValueListEntry  (PTN_Expr *_expr);
	~PTN_ValueListEntry ();
};


#endif // PTN_VALUELISTENTRY_H__
