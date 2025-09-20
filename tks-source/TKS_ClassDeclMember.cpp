/// TKS_ClassDeclMember.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_compiler.h"
#include "TKS_ClassDeclMember.h"


TKS_ClassDeclMember::TKS_ClassDeclMember(void) {
   offset                   = 0u;
   rtti                     = 0u;
   value.any                = NULL;
   b_delete_template_object = YAC_FALSE;
   protection_flags         = TKS_PROT_NONE;
   init_expr                = NULL;
   deref_init_expr          = YAC_FALSE;
   b_static                 = YAC_FALSE;
   co_static                = NULL;
   b_noinit                 = YAC_FALSE;
}

TKS_ClassDeclMember::~TKS_ClassDeclMember() {
   // yac_host->printf("xxx ~TKS_ClassDeclMember this=%p\n", this);

   if(b_delete_template_object)
   {
      YAC_DELETE(value.object_val);
      value.object_val = NULL;
   }

   offset    = 0u;
   rtti      = 0u;
   value.any = NULL;

   if(init_expr)
   {
      delete init_expr;
      init_expr = NULL;
   }

   if(co_static)
   {
      co_static->unset();
      delete co_static;
      co_static = NULL;
   }
}

void TKS_ClassDeclMember::createStaticInstance(void) {
   switch(rtti)
   {
      case YAC_TYPE_VARIANT:
         break;

      case YAC_TYPE_INT:
         co_static->initInt(0);
         break;

      case YAC_TYPE_FLOAT:
         co_static->initFloat(0.0f);
         break;

      default:
         createStaticObjectInstance();
         break;
   }
}

// ---- instantiate static class member
void TKS_ClassDeclMember::createStaticObjectInstance(void) {
   if(TKS_CLASSDECL_RTTI_CLASS == rtti)
   {
      if(b_noinit)
      {
         co_static->initNull();
      }
      else
      {
         if(value.sobject_val)
         {
            co_static->value.object_val = YAC_CLONE_POOLED(NULL, value.object_val);
            co_static->deleteme = YAC_TRUE;
         }
      }
   }
   else
   {
      if(value.object_val)
      {
         if(b_delete_template_object)
         {
            co_static->value.object_val = value.object_val;
            co_static->deleteme = YAC_FALSE;
            // ---- template object is deleted when class is deleted
         }
         else
         {
            // ---- create object instance
            if(b_noinit)
            {
               co_static->initNull();
            }
            else
            {
               co_static->value.object_val = YAC_CLONE_POOLED(NULL, value.object_val);
               co_static->deleteme = YAC_TRUE;
            }
         }
      }
   }
}
