/// PTN_ValueListEntry.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_ValueListEntry.h"


PTN_ValueListEntry::PTN_ValueListEntry(PTN_Expr *_e) {
  expr = _e;
  next = NULL;
}

PTN_ValueListEntry::~PTN_ValueListEntry() {
  delete next;
  next = NULL;
  delete expr;
  expr = NULL;
}
