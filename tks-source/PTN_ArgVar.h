/// PTN_ArgVar.h
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#ifndef __PTN_ARGVAR_H__
#define __PTN_ARGVAR_H__


class PTN_ArgVar : public PTN_Node {
   //
   // Formal argument list entry
   //
public:
	TKS_CachedObject *var; // reference to function variable hash entries
	PTN_ArgVar       *next;

public:
	PTN_ArgVar(TKS_CachedObject *);
	~PTN_ArgVar();
};


#endif // __PTN_ARGVAR_H__
