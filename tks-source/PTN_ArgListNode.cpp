/// PTN_ArgListNode.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_ArgListNode.h"


PTN_ArgListNode::PTN_ArgListNode(PTN_Expr *_expr) {
   expr         = _expr;
   next         = NULL;
   previous     = NULL;
#ifndef TKS_MT
   cv.value.any = NULL;
   cv.deleteme  = 0;
#endif
   rtti_hint    = 0; // xxx always 0 ?
   expr_opt     = expr->getEval();
}

PTN_ArgListNode::~PTN_ArgListNode() {
#ifndef TKS_MT
   cv.unset();
#endif

   if(NULL != expr)
   { 
      delete expr;
      expr = NULL;
   }

   if(NULL != next)
   {	
      delete next;
      next = NULL; 
   }
}
