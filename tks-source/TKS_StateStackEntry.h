/// TKS_StateStackEntry.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_STATESTACKENTRY_H__
#define __TKS_STATESTACKENTRY_H__


class TKS_StateStackEntry {
public:
   sU16 state;       // return/parent state
   sU16 state_index; // return/parent state_index

   union _param {
      PTAnyNodePtr node;
      YAC_String  *str;
      sUI          value;
      void        *any;
   } param[4];
   
   void initParams(void) { param[3].any=param[2].any=param[1].any = NULL; };
};


#endif // __TKS_STATESTACKENTRY_H__
