/// TKS_ClassDeclMethod.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_compiler.h"
#include "tks_inc_class.h"


TKS_ClassDeclMethod::TKS_ClassDeclMethod(void) {
   vtable           = NULL;
   protection_flags = TKS_PROT_NONE;
   b_this_mutex     = YAC_FALSE;
   b_abstract       = YAC_FALSE;
   b_virtual        = YAC_FALSE;
}

TKS_ClassDeclMethod::~TKS_ClassDeclMethod() {
   if(NULL != vtable)
   {
      // Note: node allocator batch-frees all vtables during cleanup
#ifndef TKS_USE_NODEALLOCATOR
      delete [] vtable;
#endif // TKS_USE_NODEALLOCATOR
      vtable = NULL;
   }
}

void TKS_ClassDeclMethod::allocVTable(void) {
#ifdef TKS_USE_NODEALLOCATOR
   sUI sz = sizeof(TKS_ClassDeclMethod*)
#if 1
      // (note) [24Sep2023] must reserve space for future, dynamically loaded TSL classes
      * TKS_MAX_CLASSDECLS
#else
      * Dtkscompiler->class_decls.num_elements
#endif
      ;
   vtable = (TKS_ClassDeclMethod**) Dtkscompiler->static_node_allocator->alloc(sz);
#else
   sz = sizeof(TKS_ClassDeclMethod) * TKS_MAX_CLASSDECLS;
   vtable = new(std::nothrow) TKS_ClassDeclMethod*[TKS_MAX_CLASSDECLS];
   // (todo) handle OOM
#endif // TKS_USE_NODEALLOCATOR
   memset(vtable, 0, sz);
   vtable[function.parent_class->class_id] = this; // Add reference to self
}

#ifdef TKS_CHECK_ABSTRACTCALL
sBool TKS_ClassDeclMethod::isAbstractInClass(TKS_ClassDecl *_cd) {
   //
   // Check whether this method was declared in _cd or one of its baseclasses
   //
   if(_cd != function.parent_class)
   {
      if(vtable)
      {
         if(vtable[_cd->class_id])
         {
            // OK: was implemented in given class
            return vtable[_cd->class_id]->b_abstract;
         }
      }
   }
   return 1;
}
#endif // TKS_CHECK_ABSTRACTCALL

