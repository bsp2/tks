/// PTN_PrepareStat.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Statement.h"
#include "PTN_PrepareStat.h"


PTN_PrepareStat::PTN_PrepareStat(PTN_Statement *_body) {
   body         = _body;
   not_prepared = 1;
}

PTN_PrepareStat::~PTN_PrepareStat() {
   TKS_DELETE_SAFE(body);
}

sUI PTN_PrepareStat::getID(void) const {
   return PTN_PREPARE;
}

sBool PTN_PrepareStat::semanticCheck(void) {
   return 
      (body ? body->semanticCheckAll() : 0) 
      ;
}

sBool PTN_PrepareStat::resolveXRef(void) {
   return body->resolveXRefAll();
}

void PTN_PrepareStat::optimize(void) {
   if(body)
   {
      body->optimizeAll();
   }
}

void PTN_PrepareStat::subGenCallList(void) {
   body->genCallList();
}

static void PTN_PrepareStat__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   PTN_PrepareStat *st = (PTN_PrepareStat*)_st; // xxx discards const qualifier

   if(st->not_prepared)
   {
      st->not_prepared=0;
      st->body->evalFirst(_env);
   }
}

Fevalst PTN_PrepareStat::getEvalSt(void) const { 
   return PTN_PrepareStat__eval; 
} 

void PTN_PrepareStat::eval(PTN_Env *_env) const { 
   PTN_PrepareStat__eval(_env, this); 
} 


