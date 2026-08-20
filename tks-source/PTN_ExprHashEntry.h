/// PTN_ExprHashEntry.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_EXPRHASHENTRY_H__
#define PTN_EXPRHASHENTRY_H__


class PTN_ExprHashEntry : public PTN_Node {
public:
   PTN_Expr          *expr;
   YAC_String         name;
   PTN_ExprHashEntry *next;
   Feval              expr_opt;

public:
   PTN_ExprHashEntry  (PTN_Expr *_expr, YAC_String *_name);
   ~PTN_ExprHashEntry ();

   sBool semanticCheck     (void);
   sBool resolveXRef       (void);
   void  optimize          (void);
};


#endif // PTN_EXPRHASHENTRY_H__
