/// PTN_NewExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_NEWEXPR_H__
#define __PTN_NEWEXPR_H__


class PTN_NewExpr : public PTN_Expr {
public:
   YAC_Object *template_object; // read only reference to class template
public:
   PTN_NewExpr(void);
   ~PTN_NewExpr();
   
   void   eval    (PTN_Env *_env, YAC_Value *) const;
   Feval  getEval (void) const;
};


#endif // __PTN_NEWEXPR_H__
