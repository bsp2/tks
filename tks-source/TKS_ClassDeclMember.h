/// TKS_ClassDeclMember.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_CLASSDECLMEMBER_H__
#define __TKS_CLASSDECLMEMBER_H__


class TKS_ClassDecl;
class PTN_Expr; 


class TKS_ClassDeclMember {
public:
#ifdef TKS_USE_NODEALLOCATOR
   void  operator delete (void*_ptr)    { (void)_ptr; /* do nothing */ }
   void *operator new    (size_t, void *_this) { return _this; }
#endif // TKS_USE_NODEALLOCATOR

public:
    YAC_String     name;
    sUI            offset; // num longwords
    sUI            rtti;   // 1=int, 2=float, 3=object, 4=string, 5=pointer, 6=classinstance.. TKS_CLASSDECL_RTTIxxx
    sUI            tagid;  // 0 or tag ID (order of declaration)
    sUI            protection_flags; // TKS_PROT_xxx
    TKS_ClassDecl *class_decl; 
    PTN_Expr      *init_expr; // (constant) initializer expression or null 
    sBool          deref_init_expr; // always grab initialization object (<= instead of =)
    union __my_value {
      sSI          int_val;
      sF32         float_val;
      YAC_Object  *object_val; // used as a template object 
      YAC_String  *string_val; 
      TKS_ScriptClassInstance *sobject_val;
      void        *any;
    } value;
    sUI            b_delete_template_object; // used for classes 
    sUI            b_static; // 1=class member is a static variable in the class namespace. 
    TKS_CachedObject *co_static; // stores the "CachedObject" variable if this member is static
    sBool          b_noinit; // 1=do not initialize member ("ref only")

public:
    TKS_ClassDeclMember(void);
    ~TKS_ClassDeclMember(); 
 
    void createStaticObjectInstance(void); 
    void createStaticInstance(void);
};


#endif // __TKS_CLASSDECLMEMBER_H__
