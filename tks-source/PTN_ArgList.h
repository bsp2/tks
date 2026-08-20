/// PTN_ArgList.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_ARGLIST_H__
#define PTN_ARGLIST_H__


class PTN_ArgList : public PTN_Node {
   //
   // Actual argument list
   //
public:
   PTN_ArgListNode *first_arg;
   sUI              num_args;

public:
   PTN_ArgList(void);
   ~PTN_ArgList();

   void  addArg        (PTN_Expr *_expr);
   sBool semanticCheck (void);
   void  optimize      (void);
   sBool resolveXRef   (void);
#ifdef TKS_JIT
   //sU8   compile       (VMCore *_vm);
   sUI   compileY      (const YAC_CommandY *_com, VMCore *_vm);
   sBool forceHybrid   (void);
#endif
};


#endif // PTN_ARGLIST_H__
