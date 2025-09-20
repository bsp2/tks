/// TKS_ActiveNamespace.h
///
/// (c) 2008-2025 Bastian Spiegel <bs@tkscript.de>
///    - distributed under terms of the GNU general public license (GPL).
///

#ifndef TKS_ACTIVENAMESPACE_H__
#define TKS_ACTIVENAMESPACE_H__


class TKS_ActiveNamespace {
public:
   TKS_ActiveNamespace *next;
   TKS_Namespace *      nspace;

public:
   TKS_FreezedNamespaceArray freeze(void) const; // Convert list to NULL terminated array
};


#endif // TKS_ACTIVENAMESPACE_H__
