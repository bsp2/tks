/// PTN_TryCatchFinally.h
///
/// (c) 2006-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_TRYCATCHFINALLY_H__
#define __PTN_TRYCATCHFINALLY_H__


class PTN_CatchBlock : public PTN_Node {
public:
   PTN_CatchBlock    *next;
   TKS_ExceptionType *exception_type;
   PTN_Statement     *body;
public:
   PTN_CatchBlock(TKS_ExceptionType *_t, PTN_Statement *_body);
   ~PTN_CatchBlock();

   sBool isExceptionTypeDerivedFrom(TKS_ExceptionType *_t) const;

   sBool semanticCheck  (void);
   void  optimize       (void);
   sBool resolveXRef    (void);
   void  subGenCallList (void);
};

class PTN_TryCatchFinally : public PTN_Statement {
public:
   PTN_Statement  *try_body;
   PTN_CatchBlock *first_catch;
   PTN_Statement  *finally_body;
   PTN_TryCatchFinally *parent;

public:
   PTN_TryCatchFinally(PTN_TryCatchFinally *_parent);
   ~PTN_TryCatchFinally();

   void addCatchSorted(TKS_ExceptionType *_t, PTN_Statement *_st);
   sBool canCatchType(sUI _exceptionTypeId) const;

   sUI     getID          (void) const;
   void    eval           (PTN_Env *_env) const;
   Fevalst getEvalSt      (void) const;
   sBool   semanticCheck  (void);
   void    optimize       (void);
   sBool   resolveXRef    (void);
   void    subGenCallList (void);
};


#endif // __PTN_TRYCATCHFINALLY_H__
