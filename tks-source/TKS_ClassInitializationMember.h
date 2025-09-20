/// TKS_ClassInitializationMember.h  
///  
/// (c) 2005-2009 Bastian Spiegel <bs@tkscript.de>  
///    - distributed under terms of the GNU general public license (GPL).
/// 
/// 
#ifndef __TKS_CLASSINITIALIZATION_MEMBER_H__
#define __TKS_CLASSINITIALIZATION_MEMBER_H__
 

class TKS_ClassInitializationMember { 

#ifdef TKS_USE_NODEALLOCATOR
public:
   void  operator delete (void*_ptr)    { (void)_ptr; /* do nothing */ }
   void *operator new    (size_t, void *_this) { return _this; }
#endif // TKS_USE_NODEALLOCATOR

public: 
     TKS_ClassDeclMember *member; 
     sUI                  shifted_offset; 
}; 


#endif // __TKS_CLASSINITIALIZATION_MEMBER_H__
