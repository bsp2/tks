/// TKS_ClassDeclMethod.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
///
#ifndef __TKS_CLASSDECLMETHOD_H__
#define __TKS_CLASSDECLMETHOD_H__


class PTN_Function;
class TKS_ClassDecl;


class TKS_ClassDeclMethod {
public:
    enum __types {
        STD         = 0,
        CONSTRUCTOR = 1,
        DESTRUCTOR  = 2,

        PAD
    };
#ifdef TKS_USE_NODEALLOCATOR
   void  operator delete (void*_ptr)    { (void)_ptr; /* do nothing */ }
   void *operator new    (size_t, void *_this) { return _this; }
#endif // TKS_USE_NODEALLOCATOR

public:
    PTN_Function         function;
    sUI                  class_id;         // id of class this method belongs to
    sUI                  type;             // STD, CONTRUCTOR, DESTRUCTOR
    sUI                  protection_flags; // TKS_PROT_x
    TKS_ClassDeclMethod**vtable;           // [TKS_MAX_CLASSDECLS] or NULL if method is not virtual
    TKS_ClassDecl       *class_decl;
    sBool                b_this_mutex;
    sBool                b_virtual;        // 1=extends abstract base class method (=> make sure that a compatible one exists)
    sBool                b_abstract;       // 1=method cannot be called (=> make sure that method is impl. in derived classes)

public:
    TKS_ClassDeclMethod  (void);
    ~TKS_ClassDeclMethod ();

    void  allocVTable       (void);

#ifdef TKS_CHECK_ABSTRACTCALL
    sBool isAbstractInClass (TKS_ClassDecl *_cd);
#endif // TKS_CHECK_ABSTRACTCALL
};


#endif // __TKS_CLASSDECLMETHOD_H__
