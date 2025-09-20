/// PTN_ExprHashEntry.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_EXPRHASHENTRY_H__
#define __PTN_EXPRHASHENTRY_H__
 
 
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


#endif // __PTN_EXPRHASHENTRY_H__

