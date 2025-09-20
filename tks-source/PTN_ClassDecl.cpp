/// PTN_ClassDecl.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "tks_inc_class.h"

#include "PTN_ClassDecl.h"


PTN_ClassDecl::PTN_ClassDecl(void) {
   class_decl = 0;
}

PTN_ClassDecl::PTN_ClassDecl(TKS_ClassDecl *_classdecl) {
   class_decl = _classdecl;
}

PTN_ClassDecl::~PTN_ClassDecl() {
}

sBool PTN_ClassDecl::semanticCheck(void) {
    sBool r=(class_decl!=0);
    sUI i;
    for(i=0; r&&(i<class_decl->methods.num_elements); i++)
    {
        TKS_ClassDeclMethod *m = class_decl->methods.elements[i];
        r = r && m->function.semanticCheck(); 
    }
    return r;
}

sBool PTN_ClassDecl::resolveXRef(void) {
   sBool r=1;
   sUI i;
   for(i=0; r&&(i<class_decl->methods.num_elements); i++)
   {
      TKS_ClassDeclMethod *m = class_decl->methods.elements[i];
      r = r && m->function.resolveXRef();
      if(!r)
      {
         Dprintf("[---] PTN_ClassDecl::resolveXRef: failed in method \"%s\"\n",
            (char*)m->function.name.chars);
      }
   }

   if(!r)
   {
      Dprintf("[---] PTN_ClassDecl::resolveXRef: failed.\n");
   }
   
   return r;
}

void PTN_ClassDecl::optimize(void) {
   ////tkscript->printf("xxx optimize class \"%s\".\n", class_decl->name.chars);
   class_decl->optimize();
}
