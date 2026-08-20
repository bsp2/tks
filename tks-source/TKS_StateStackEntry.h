/// TKS_StateStackEntry.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_STATESTACKENTRY_H__
#define TKS_STATESTACKENTRY_H__


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


#endif // TKS_STATESTACKENTRY_H__
