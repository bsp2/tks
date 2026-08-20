/// PTN_ExprArrayEntry.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_EXPRARRAYENTRY_H__
#define PTN_EXPRARRAYENTRY_H__


class PTN_ExprArrayEntry : public PTN_Node {
public:
   PTN_Expr           *expr;
   PTN_ExprArrayEntry *next;
   Feval               expr_opt;

public:
   PTN_ExprArrayEntry  (PTN_Expr *_expr);
   ~PTN_ExprArrayEntry ();

   sBool semanticCheck      (void);
   void  optimize           (void);
   sBool resolveXRef        (void);
};


#endif // PTN_EXPRARRAYENTRY_H__
