/// PTN_ExprArrayEntry.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_EXPRARRAYENTRY_H__
#define __PTN_EXPRARRAYENTRY_H__

 
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


#endif // __PTN_EXPRARRAYENTRY_H__
