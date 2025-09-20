/// PTN_Switch.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef PTN_SWITCH_H__
#define PTN_SWITCH_H__


class PTN_Switch : public PTN_BreakableStatement {
public:
   PTN_Expr              *expr;
   Feval                  expr_opt;
   PTN_SwitchCase        *switch_case;  // double linked list of switch cases
   PTN_SwitchDefaultCase *default_case; // default switch case or NULL (non-deletable reference into switch_case list)
   sBool                  b_beq;        // 1=expr is @() type PTN_BuiltinFun (compare bit patterns / pointer addresses)

public:
   PTN_Switch(void);
   PTN_Switch(PTN_Expr *);
   ~PTN_Switch();

   sUI     getID           (void) const;
   sBool   semanticCheck   (void);
   void    eval            (PTN_Env *_env) const;
   Fevalst getEvalSt       (void) const;
   void    subGenCallList  (void);
   sBool   optimizeBEQ     (void);  // switch(@(expr))
   void    optimize        (void);
   sBool   resolveXRef     (void);
};


#endif // PTN_SWITCH_H__
