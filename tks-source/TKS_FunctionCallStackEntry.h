/// TKS_FunctionCallStackEntry.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_FUNCTIONCALLSTACKENTRY_H__
#define TKS_FUNCTIONCALLSTACKENTRY_H__


// ---- Used for function call backtraces
class TKS_FunctionCallStackEntry {
public:
   const PTN_Function *function;   // what script function was called
   const YAC_CommandY *commandY;   // resp. what C++ function/method was called
   const char   *yclass_name;// the name of the C++ class
   sUI           src_loc;    // where it was called

public:
   void init(TKS_FunctionCallStackEntry *_src);
   void print(sSI _line, sSI _sid);
   void appendString(YAC_String *_s, sSI _line);
};


#endif // TKS_FUNCTIONCALLSTACKENTRY_H__
