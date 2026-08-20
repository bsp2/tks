/// PTN_ClassDecl.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_CLASSDECL_H__
#define PTN_CLASSDECL_H__


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


#endif // PTN_CLASSDECL_H__
