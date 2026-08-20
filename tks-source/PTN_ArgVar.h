/// PTN_ArgVar.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_ARGVAR_H__
#define PTN_ARGVAR_H__


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


#endif // PTN_ARGVAR_H__
