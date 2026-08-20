/// PTN_NewExpr.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_NEWEXPR_H__
#define PTN_NEWEXPR_H__


class PTN_NewExpr : public PTN_Expr {
public:
   YAC_Object *template_object; // read only reference to class template
public:
   PTN_NewExpr(void);
   ~PTN_NewExpr();

   void   eval    (PTN_Env *_env, YAC_Value *) const;
   Feval  getEval (void) const;
};


#endif // PTN_NEWEXPR_H__
