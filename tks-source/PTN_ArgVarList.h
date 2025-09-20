/// PTN_ArgVarList.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#ifndef PTN_ARGVARLIST_H__
#define PTN_ARGVARLIST_H__


class PTN_ArgVarList : public PTN_Node {
   //
   // Formal argument list
   //
public:
   PTN_ArgVar       *first_var;  // used during initialization, linked list holds references to function arg hash entries
   sUI               num_vars;
   TKS_CachedObject**cached_vars; // linear variable list

public:
   PTN_ArgVarList(void);
   ~PTN_ArgVarList();

   void       addVar (TKS_CachedObject *);
   void       freeVarLinkedList(void);
   YAC_Value *getVar (TKS_Context *_context, sUI);
   void       init   (void);
};


#endif // PTN_ARGVARLIST_H__
