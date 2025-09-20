/// TKS_FormalArgument.cpp 
/// 
/// (c) 2001-2020 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 
 
#include "tks.h" 
#include "TKS_ScriptConstraint.h" 
#include "TKS_FormalArgument.h" 
 
#include "PTN_Node.h" 
#include "PTN_Expr.h" 
#include "PTN_Statement.h" 
#include "vmopenum.h" 
#include "VMCore.h" 
#include "PTN_ArgVar.h" 
#include "PTN_ArgVarList.h" 
#include "PTN_ArgListNode.h" 
#include "PTN_ArgList.h" 
#include "TKS_ScriptObjectID.h"
#include "TKS_FunctionObject.h"
#include "PTN_Function.h" 
 
#include "tks_inc_class.h" 
#include "tks_inc_compiler.h" 
 
 
TKS_FormalArgument::TKS_FormalArgument(void) {
   is_pointerarray    = 0;
   is_array           = 0; 
   is_local           = 0; 
   var                = NULL; 
   script_constraint  = NULL; 
   constraint_type    = TKS_CONSTRAINTTYPE_NONE; 
   next               = NULL; 
   cached_meta_class  = NULL;
   cached_cpp_class   = NULL;
   class_type         = 0;
   freezed_namespaces = NULL;
   forced_nspace      = NULL;
} 
 
TKS_FormalArgument::TKS_FormalArgument(YAC_String * _name,  
                                       sSI          _type,  
                                       YAC_String * _className, 
                                       sBool        _bPointerArray,
                                       sBool        _array,  
                                       sBool        _local 
                                       )  
{ 
   name.yacCopy(_name); 
   class_name.yacCopy(_className); 
   type               = _type; 
   is_pointerarray    = _bPointerArray;
   is_array           = _array; 
   is_local           = _local; 
   var                = NULL; 
   next               = 0; 
   script_constraint  = NULL; 
   constraint_type    = TKS_CONSTRAINTTYPE_NONE; 
   cached_meta_class  = NULL;
   cached_cpp_class   = NULL;
   class_type         = 0; 
   freezed_namespaces = NULL;
   forced_nspace      = NULL;
} 
 
TKS_FormalArgument::~TKS_FormalArgument() { 
   if(next) 
   { 
      delete next; // xxx do not delete recursively..
      next = NULL; 
   } 

   if(NULL != freezed_namespaces)
   {
      delete [] freezed_namespaces;
      freezed_namespaces = NULL;
   }

} 
 
// ---- Resolve class_name to class template object 
TKS_CachedObject *TKS_FormalArgument::createVariable(TKS_ObjectCache *_vc, PTN_Function *_cfun) { 
   if(_vc->findEntry(&name)) 
   { 
      Dprintf("[---] FormalArgument: variable \"%s\" is already declared.\n", (char*)name.chars); 
      return 0; 
   } 
   if(is_pointerarray)
   {
      var=_vc->createObjectEntry( 
         &name, 0, TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_POINTERARRAY]->template_object, 0);
      class_type = YAC_CLID_POINTERARRAY; 
   }
   else if(is_array) 
   {
      switch(type) 
      { 
         case YAC_TYPE_VARIANT: 
            var=_vc->createObjectEntry( 
               &name, 0, TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_VALUEARRAY]->template_object, 0);  
            class_type = YAC_CLID_VALUEARRAY; 
            break; 
 
         case YAC_TYPE_INT: 
            var=_vc->createObjectEntry( 
               &name, 0, TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_INTARRAY]->template_object, 0);
            class_type = YAC_CLID_INTARRAY; 
            break; 
 
         case YAC_TYPE_FLOAT: 
            var=_vc->createObjectEntry(
               &name, 0, TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_FLOATARRAY]->template_object, 0);
            class_type = YAC_CLID_FLOATARRAY; 
            break; 
 
         case YAC_TYPE_OBJECT: 
            if(class_name.isBlank()) // 'Object' pointer type
            { 
               var=_vc->createObjectEntry( 
                  &name, 0, TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_POINTERARRAY]->template_object, 0);
               class_type = YAC_CLID_POINTERARRAY; 
            }
            else if(!resolveClassType())
            {
               Dprintf("[---] FormalArgument::createVariable: failed to resolve class \"%s\".\n", 
                       (char*)class_name.chars); 
            }
            else
            {
               if(cached_meta_class)
               {
                  // Script class
                  var=_vc->createObjectEntry( 
                     &name, 0, TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_CLASSARRAY]->template_object, 0);
                  class_type = YAC_CLID_CLASSARRAY; 
               }
               else
               {
                  // C++ class
                  var=_vc->createObjectEntry( 
                     &name, 0, TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_OBJECTARRAY]->template_object, 0);
                  class_type = YAC_CLID_OBJECTARRAY; 
               }
            }
            break; 
 
         case YAC_TYPE_STRING: 
            var=_vc->createObjectEntry( 
               &name, 0, TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_STRINGARRAY]->template_object, 0);
            class_type = YAC_CLID_STRINGARRAY; 
            break; 
      } 
   }
   else 
   {
      // Scalar
      switch(type) 
      { 
         case YAC_TYPE_VARIANT: 
            var = _vc->createVariantEntry(&name, 0);
            break; 
         case YAC_TYPE_INT: 
            var = _vc->createIntegerEntry(&name, 0);
            break; 
         case YAC_TYPE_FLOAT: 
            var = _vc->createFloatEntry(&name, 0);
            break; 
         case YAC_TYPE_OBJECT: 
            if(class_name.isBlank()) 
            { 
               var=_vc->createObjectEntry(
                  &name, 0, TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_OBJECT]->template_object, 0); 
            }
            else if(!resolveClassType())
            {
               Dprintf("[---] FormalArgument::createVariable: failed to resolve class \"%s\".\n", 
                       (char*)class_name.chars); 
            }
            else
            {
               if(cached_meta_class) 
               { 
                  // Script class
                  var = _vc->createObjectEntry(&name, 0, cached_meta_class, 0); 
                  class_type = cached_meta_class->class_decl->class_id; 
               }
               else
               {
                  // C++ class
                  var = _vc->createObjectEntry(&name, 0, cached_cpp_class, 0); 
                  class_type = cached_cpp_class->class_ID; 
               }
            }
            break; 

         case YAC_TYPE_STRING: 
            var=_vc->createObjectEntry(
               &name, 0, TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_STRING]->template_object, 0);
            var->type  = YAC_TYPE_STRING; 
            class_type = YAC_CLID_STRING; 
            break; 
      } 
   } 
   if(var) 
   { 
      if(is_local) 
      { 
         var->flags |= TKS_CachedObject::ISLOCAL; 
         var->local_index = _cfun->num_local_vars++; 
      } 
      return var; 
   } 
   return 0; 
} 

sBool TKS_FormalArgument::resolveClassType(void) {
   TKS_ClassDecl *cd;
   TKS_ClassTemplate *clt;
   
   if(class_name.isBlank()) // "Object" pointer type
   {
      cached_cpp_class = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_OBJECT]->template_object;
      return 1;
   }
   
   // Has the class already been resolved ?
   if( (NULL == cached_meta_class) && (NULL == cached_cpp_class) )
   {
      if(!tkscript->findDeclaredClassType(&class_name, freezed_namespaces, forced_nspace, &clt, &cd))
      {
#ifdef TKS_DCON
         if(NULL != forced_nspace)
         {
            Dprintf("[---] TKS_FormalArgument::resolveClassType: unable to resolve class \"%s::%s\".\n",
                    (char*) forced_nspace->name.chars,
                    (char*) class_name.chars
                    );
         }
         else
         {
            Dprintf("[---] TKS_FormalArgument::resolveClassType: unable to resolve class \"%s\".\n",
                    (char*) class_name.chars
                    );
         }
#endif 
         return 0;
      }
      else
      {
         if(NULL != clt)
         {
            cached_cpp_class = clt->template_object;
         }
         else
         {
            cached_cpp_class = NULL;
         }
         if(NULL != cd)
         {
            cached_meta_class = cd->class_template;
         }
         else
         {
            cached_meta_class = NULL;
         }
      }
   }
   // Found or already resolved
   return 1;
}

