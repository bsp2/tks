/// PTN_OpObjectMemberAssignY.h
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_OPOBJECTMEMBERASSIGNY_H__
#define __PTN_OPOBJECTMEMBERASSIGNY_H__


class PTN_OpObjectMemberAssignY : public PTN_Statement { 
public: 
   TKS_CachedObject   *var;             // ro reference to object variable 
   PTN_Expr           *expr; 
   sU8                 assign_operator; // YAC_OP_xxx
   const YAC_CommandY *get_command; 
   const YAC_CommandY *set_command; 
   sUI                 clid;            // class id at compile time 
   sBool               b_indirect;
   
   Feval             expr_opt; 
   
public: 
   PTN_OpObjectMemberAssignY(TKS_CachedObject   * _var,  
                             const YAC_CommandY * _get_command,  
                             const YAC_CommandY * _set_command,  
                             sU8                  _assign_operator,  
                             PTN_Expr           * _expr,  
                             sUI                  _clid 
                             ); 
   ~PTN_OpObjectMemberAssignY(); 
   
   void initIndirect(void) { var = NULL; b_indirect = YAC_TRUE; }

   sBool   semanticCheck (void);
   void    optimize      (void);
   sBool   resolveXRef   (void);
   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
#ifdef TKS_JIT
   sU8     compile       (VMCore *_vm);
   sBool   forceHybrid   (void);
#endif
};


#endif // __PTN_OPOBJECTMEMBERASSIGNY_H__
