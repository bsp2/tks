/// PTN_ArgListNode.h
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#ifndef __PTN_ARGLISTNODE_H__
#define __PTN_ARGLISTNODE_H__

class PTN_ArgListNode : public PTN_Node {
   //
   // Actual argument list entry
   //
public: 
   PTN_Expr        *expr;    
   Feval            expr_opt;
   PTN_ArgListNode *next;
   PTN_ArgListNode *previous;
#ifndef TKS_MT
   YAC_Value        cv; // xxx TKS_MT not thread safe (re-entrance)
#endif
   sU8              rtti_hint; // xxx always 0 ?
   
public:
   PTN_ArgListNode(PTN_Expr *_expr);
   ~PTN_ArgListNode();
};


#endif // __PTN_ARGLISTNODE_H__
