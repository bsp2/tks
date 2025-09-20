/// PTN_Node.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>. 
///     - distributed under terms of the GNU general public license (GPL).
///
///
///          07062001 <bs> opened file. 
//

#include "tks.h"
#include "PTN_Node.h"


YAC_Value PTN_Node::zero_val;
sSI PTN_Node::object_counter = 0;


PTN_Node::PTN_Node(void) { 
   PTN_Node::object_counter++;
}

PTN_Node::~PTN_Node() { 
   PTN_Node::object_counter--;
}

const char *PTN_Node::GetErrorByCode(sU16 _ec) {
   switch(_ec)
   {
   case PTNERR_NOERROR:                 return "+++noerror+++";
   case PTNERR_ANYERR:                  return " ";
   case PTNERR_UFUNMISSINGBODY:         return "missing function body";
   case PTNERR_CLASSSTACKOVERFLOW:      return "class stack overflow";
   case PTNERR_MODULENOTFOUND:          return "module not found";
   case PTNERR_MODULEMEMBERNOTFOUND:    return "module member not found";
   case PTNERR_MODULEMETHODNOTFOUND:    return "module method not found";
   case PTNERR_CANTCOMPILE:             return "node type is not supported by JIT compiler.";
   case PTNERR_ERRCOMPILE:              return "error compiling node (unsupported argument/return type ?).";
   case PTNERR_FSTACKOVERFLOW:          return "function stack overflow";
   case PTNERR_BREAKSTACKOVERFLOW:      return "break stack overflow";
   case PTNERR_SWITCHSTACKOVERFLOW:     return "switch stack overflow";
   case PTNERR_CONSTRAINTFAILED:        return "check constraint failed"; 
   default:                             return "**ILL.ERRCODE**";
   }
}

sUI PTN_Node::getID(void) const {
   return PTN_NODE;
}

#ifdef TKS_JIT
sBool PTN_Node::forceHybrid(void) { 
   return 1; 
}
#endif

sBool PTN_Node::semanticCheck(void) {
   return 1;
}

sBool PTN_Node::isConst(void) {
   return 0;
}

void PTN_Node::optimize(void) {
}

sBool PTN_Node::resolveXRef(void) {
   return 1;
}

#ifdef TKS_JIT
sU8 PTN_Node::compile(VMCore *) {
   Dcterror(PTNERR_CANTCOMPILE);
   return 0xFF; // not supported
} 
#endif

sBool PTN_Node::needSemicolon(void) const {
   sUI pid = getID();
   return
      (
      pid==PTN_FOR             ||
      pid==PTN_IFELSE          ||
      pid==PTN_WHILE           ||
      pid==PTN_DOWHILE         ||
      pid==PTN_SWITCH          ||
      pid==PTN_LOOP            ||
      pid==PTN_FUNCTION        ||
      pid==PTN_FUNCTIONBODY    ||
      pid==PTN_FOREACH         ||
      pid==PTN_DEFINE          ||
      pid==PTN_CLASSDECL       ||
      pid==PTN_CLASSMETHODBODY ||
      pid==PTN_TRACESTAT       ||
      pid==PTN_PREPARE         ||
      pid==PTN_TRYCATCHFINALLY ||
      pid==PTN_EMPTY
      );
}

