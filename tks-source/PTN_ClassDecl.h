/// PTN_ClassDecl.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSDECL_H__
#define __PTN_CLASSDECL_H__


class PTN_ClassDecl : public PTN_Statement {
public:
    TKS_ClassDecl *class_decl;
public:
    PTN_ClassDecl(void);
    PTN_ClassDecl(TKS_ClassDecl*);
    ~PTN_ClassDecl(void); 

    sUI   getID         (void) const { return PTN_CLASSDECL; };
    sBool semanticCheck (void);
    sBool resolveXRef   (void);
    void  optimize      (void);
};


#endif // __PTN_CLASSDECL_H__
