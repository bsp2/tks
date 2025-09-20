/// PTN_ValueListEntry.cpp
/// (c) 2001-2005 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_ValueListEntry.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                   ~
//~ PTN_ValueListEntry                ~
//~                                   ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PTN_ValueListEntry::PTN_ValueListEntry(PTN_Expr *_e) {
  expr=_e;
  next=0;
}

PTN_ValueListEntry::~PTN_ValueListEntry() {
  delete next;
  next=0;
  delete expr;
  expr=0;
}
