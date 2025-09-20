/// TKS_ClassDecl.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdlib.h>

#include "tks.h"

#include "tks_inc_class.h"
#include "tks_inc_compiler.h"
#include "TKS_CachedScript.h"

#include "YAC_ObjectArray.h"
#include "YAC_ClassArray.h"

#include "StaticList.h"

static const char *tks_prot_typenames[TKS_PROT_NUM] = {"unprotected", "public", "protected", "private", "module", "namespace"};


TKS_ClassDecl::TKS_ClassDecl(void) {
   is_declared             = YAC_FALSE;
   class_id                = 0u;
   module                  = NULL;
   nspace                  = NULL;
   src_loc                 = 0u;
   member_data_size        = 0u;
   current_offset          = 0u;
   class_template          = NULL;
   template_initialized    = YAC_FALSE;
   functions               = NULL;
   constants               = NULL;
   constants_documentation = NULL;

   initialization_members       = NULL;
   effective_init_members       = NULL;
   b_static_members_initialized = YAC_FALSE;

   members.elements     = NULL;
   members.max_elements = members.num_elements = 0u;

   methods.elements     = NULL;
   methods.max_elements = methods.num_elements = 0u;

   ancestors.elements     = NULL;
   ancestors.num_elements = ancestors.max_elements = 0u;

   class_dependencies.elements     = NULL;
   class_dependencies.num_elements = class_dependencies.max_elements = 0u;

   all_members.elements     = NULL;
   all_members.max_elements = all_members.num_elements = 0u;

   num_object_members = 0u;
   num_ctors = 0u;
   num_dtors = 0u;

   b_this_mutex          = YAC_FALSE;
   b_abstract            = YAC_FALSE;
   b_args_resolved       = YAC_FALSE;
   b_init_functions_done = YAC_FALSE;
   b_done                = YAC_FALSE;

   class_field_mask = new(std::nothrow) sU8[TKS_MAX_CLASSDECLS];

   resetAncestorAndRelocationTables();  // also zeros class_field_mask[]

   callback.fun_yacOperator       = NULL;
   callback.fun_yacToString       = NULL;
   callback.fun_yacOperatorAssign = NULL;
}

TKS_ClassDecl::~TKS_ClassDecl() {
   clean();
}

void TKS_ClassDecl::resetAncestorAndRelocationTables(void) {
   // (todo) use memset
   for(sUI i = 0u; i < TKS_MAX_CLASSDECLS; i++)
   {
      ancestor_table         [i] = 0u;
      class_relocation_table [i] = 0u;
      class_field_mask       [i] = 0u;
   }
}

sBool TKS_ClassDecl::isUsed(void) const {
   return (class_id > 0u) && (NULL != module);
}

void TKS_ClassDecl::clean(void) {
   // Dprintf("xxx TKS_ClassDecl::clean: id=%u name=\"%s\"\n", class_id, name.chars);

   template_member_hashtable.freeCache();
   template_method_hashtable.freeCache();

   freeFunctions();

   if(NULL != constants)
   {
      delete constants;
      constants = NULL;
   }
   if(NULL != constants_documentation)
   {
      delete constants_documentation;
      constants_documentation = NULL;
   }

   if(NULL != all_members.elements)
   {
      delete [] all_members.elements;
      all_members.elements = NULL;
      all_members.num_elements = 0u;
      all_members.max_elements = 0u;
   }

   if(NULL != class_dependencies.elements)
   {
      delete [] class_dependencies.elements;
      class_dependencies.elements = NULL;
      class_dependencies.num_elements = 0u;
      class_dependencies.max_elements = 0u;
   }

   if(ancestors.elements)
   {
      delete [] ancestors.elements;
      ancestors.elements = NULL;
      ancestors.num_elements = 0u;
      ancestors.max_elements = 0u;
   }

   if(members.elements)
   {
      for(sUI i = 0u; i < members.num_elements; i++)
      {
         if(members.elements[i])
         {
            delete members.elements[i];
         }
      }
      delete [] members.elements;
      members.elements = NULL;
      members.num_elements = 0u;
      members.max_elements = 0u;
   }

   if(methods.elements) // new..
   {
      for(sUI i = 0u; i < methods.num_elements; i++)
      {
         if(methods.elements[i])
         {
            delete methods.elements[i];
         }
      }
      delete [] methods.elements;
      methods.elements = NULL;
      methods.num_elements = 0u;
      methods.max_elements = 0u;
   }

   freeInitializationMembers();

   if(NULL != class_field_mask)
   {
      delete[] class_field_mask;
      class_field_mask = NULL;
   }

   constructor_list.free();
   destructor_list.free();

   template_initialized  = YAC_FALSE;
   is_declared           = YAC_FALSE;
   b_args_resolved       = YAC_FALSE;
   b_abstract            = YAC_FALSE;
   b_init_functions_done = YAC_FALSE;
   b_done                = YAC_FALSE;
   member_data_size      = 0u;
   current_offset        = 0u;

   // [29Mar2025] (temp-)class clean-up / revert to unused state
   //  (note) do NOT unset class_id
   module               = NULL;
   nspace               = NULL;
   src_loc              = 0u;
   name.free();
   documentation.free();
}

void TKS_ClassDecl::freeFunctions(void) {
   if(NULL != functions)
   {
      for(sUI i = 0u; i < functions->max_entries; i++)
      {
         TKS_CachedObject *co = functions->objects[i];
         if(co)
         {
            if(YAC_TYPE_VOID != co->type)
            {
               PTN_Function *fun = (PTN_Function *)co->value.any;
               delete fun;
            }
         }
      }
      delete functions;
      functions = NULL;
   }
}

void TKS_ClassDecl::freeInitializationMembers(void) {
   if(initialization_members)
   {
      freeInitializationMembers2(&initialization_members);
   }
   if(effective_init_members)
   {
      freeInitializationMembers2(&effective_init_members);
   }
}

void TKS_ClassDecl::freeInitializationMembers2(StaticList::Node **_list) {
   StaticList::Node *c = *_list;
   while(c)
   {
      TKS_ClassInitializationMember *im = (TKS_ClassInitializationMember *)c->data;
      if(im)
      {
         delete im;
         c->data = NULL;
      }
      c = c->next;
   }
   StaticList::Free(_list);
}

void TKS_ClassDecl::addInitializationMember(TKS_ClassDeclMember*_m, sBool _deref) {
   TKS_ClassInitializationMember *m;
   PTN_NEW_STATIC_NODE(m, TKS_ClassInitializationMember);
   m->member = _m;
   m->member->deref_init_expr = _deref;
   m->shifted_offset = (sUI)-1;
   StaticList::Node *c;
   if(initialization_members)
   {
      // Append to end of list
      c = StaticList::Tail(initialization_members);
      c->next = STATICLIST_NEW_NODE;
      c->next->previous = c;
      c->next->next     = NULL;
      c->next->data     = (void *)m;
   }
   else
   {
      StaticList::InsertHead(&initialization_members, (void *)m);
   }

   // ---- static members are removed before class instanciation
}

void TKS_ClassDecl::init(void) {
   if(NULL == class_template)
   {
      class_template = (TKS_ScriptClassInstance*)
         TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_CLASS]->template_object->yacNewObject();
   }
   else
   {
      // temp module class reinit
      class_template->clean();
   }
   class_template->class_decl = this;

   if(NULL == class_field_mask)
   {
      // reinit
      class_field_mask = new(std::nothrow) sU8[TKS_MAX_CLASSDECLS];

      resetAncestorAndRelocationTables();  // also zeros class_field_mask[]
   }

   members.elements = new(std::nothrow) TKS_ClassDeclMember*[ TKS_MAX_CLASSMEMBERS ];
   if(members.elements)
   {
      members.max_elements = TKS_MAX_CLASSMEMBERS;
   }
   else
   {
      members.max_elements = 0u; // XXX panic
   }
   members.num_elements = 0u;

   methods.elements = new(std::nothrow) TKS_ClassDeclMethod*[ TKS_MAX_CLASSMETHODS ];
   if(methods.elements)
   {
      methods.max_elements = TKS_MAX_CLASSMETHODS;
   }
   else
   {
      methods.max_elements = 0u; // XXX panic
   }
   methods.num_elements = 0u;

   ancestors.elements = new(std::nothrow) TKS_ClassDecl *[ TKS_MAX_CLASSANCESTORS];
   if(ancestors.elements)
   {
      ancestors.max_elements = TKS_MAX_CLASSANCESTORS;
   }
   else
   {
      ancestors.max_elements = 0u; // XXX panic
   }
   ancestors.num_elements = 0u;

   class_dependencies.elements = new(std::nothrow) TKS_ClassDecl *[TKS_MAX_CLASSDEPENDENCIES];
   if(class_dependencies.elements)
   {
      class_dependencies.max_elements = TKS_MAX_CLASSDEPENDENCIES;
   }
   else
   {
      class_dependencies.max_elements = 0u; // XXX panic
   }
   class_dependencies.num_elements = 0u;

   class_field_mask[class_id] = 1u;

   // constants_documentation hash is allocated at first call to setConstantDocumentation()
}

TKS_ClassDecl *TKS_ClassDecl::findAncestor(YAC_String *_name) const {
   for(sUI i = 0u; i < ancestors.num_elements; i++)
   {
      if(ancestors.elements[i]->name.compare(_name))
      {
         return ancestors.elements[i];
      }
   }
   return NULL;
}

TKS_ClassDecl *TKS_ClassDecl::findAncestorDep(TKS_ClassDecl *_cd) const { // ancestor of ancestors
   for(sUI i = 0u; i < class_dependencies.num_elements; i++)
   {
      if(class_dependencies.elements[i] == _cd) ////->name.compare(_name))
      {
         return class_dependencies.elements[i];
      }
   }
   return NULL;
}

sBool TKS_ClassDecl::hasBaseClass(TKS_ClassDecl *_cd) const {
   return ancestor_table[_cd->class_id];
}

TKS_ClassDeclMember *TKS_ClassDecl::findMemberSuper(YAC_String *_name, sUI _static) const {
   TKS_ClassDeclMember**c=members.elements;
   sUI i;
   for(i = 0u; i < members.num_elements; i++)
   {
      if((*c)->name.compare(_name))
      {
         if((*c)->b_static == _static)
         {
            return (*c);
         }
      }
      c++;
   }
   // ---- search super classes
   sUI j;
   for(j = 0u; j < class_dependencies.num_elements; j++)
   {
      TKS_ClassDecl *cd=class_dependencies.elements[j];
      c = cd->members.elements;
      for(i = 0u; i < cd->members.num_elements; i++)
      {
         if((*c)->name.compare(_name))
         {
            if((*c)->b_static == _static)
            {
               return (*c);
            }
         }
         c++;
      }
   }
   return NULL;
}

TKS_CachedObject *TKS_ClassDecl::getMember(YAC_String *_name) const {
   return template_member_hashtable.findEntry(_name);
}

TKS_CachedObject *TKS_ClassDecl::getStaticMember(YAC_String *_name) const {
   TKS_ClassDeclMember**m = members.elements;
   sUI i;
   for(i = 0u; i < members.num_elements; i++)
   {
      if((*m)->b_static)
      {
         if((*m)->name.compare(_name))
         {
            return (*m)->co_static;
         }
      }
      m++;
   }
   for(i = 0u; i < ancestors.num_elements; i++)
   {
      TKS_CachedObject *co=ancestors.elements[i]->getStaticMember(_name);
      if(co)
      {
         return co;
      }
   }
   return NULL;
}

PTN_Function *TKS_ClassDecl::findFunction(YAC_String *_name) const {
   if(NULL != functions)
   {
      TKS_CachedObject *co = functions->findEntry(_name);
      if(co)
      {
         return (PTN_Function*)co->value.object_val;
      }
   }
   return NULL;
}

PTN_Function *TKS_ClassDecl::findFunctionSuper(YAC_String *_name) const {
   PTN_Function *fun = findFunction(_name);
   if(!fun)
   {
      for(sUI j = 0u; (!fun) && (j < class_dependencies.num_elements); j++)
      {
         TKS_ClassDecl *cd = class_dependencies.elements[j];
         fun = cd->findFunction(_name);
      }
   }
   return fun;
}

TKS_ClassDeclMethod *TKS_ClassDecl::findMethod(YAC_String *_name) const {
   TKS_ClassDeclMethod**c = methods.elements;
   sUI i;
   for(i = 0u; i < methods.num_elements; i++)
   {
      if((*c)->function.name.compare(_name))
      {
         return (*c);
      }
      c++;
   }
   return NULL;
}

TKS_ClassDeclMethod *TKS_ClassDecl::findMethodSuper(YAC_String *_name) const {
   TKS_ClassDeclMethod**c = methods.elements;
   sUI i;
   for(i = 0u; i < methods.num_elements; i++)
   {
      if((*c)->function.name.compare(_name))
      {
         return (*c);
      }
      c++;
   }
   // ---- search super classes
   sUI j;
   for(j = 0u; j < class_dependencies.num_elements; j++)
   {
      TKS_ClassDecl *cd = class_dependencies.elements[j];
      c = cd->methods.elements;
      for(i = 0u; i < cd->methods.num_elements; i++)
      {
         if((*c)->function.name.compare(_name))
         {
            return (*c);
         }
         c++;
      }
   }
   return NULL;
}


TKS_ClassDeclMethod *TKS_ClassDecl::getMethod(YAC_String *_name) const {
   TKS_CachedObject *co = template_method_hashtable.findEntry(_name);
   if(co)
   {
      return (TKS_ClassDeclMethod*)co->value.object_val;
   }
   return NULL;
}

void TKS_ClassDecl::setLastArraySize(sSI _size) {
   if(members.num_elements)
   {
      TKS_ClassDeclMember *m=members.elements[members.num_elements-1];
      if(!m->b_delete_template_object)
      {
         m->value.object_val=m->value.object_val->yacNew((void*)tkscript->main_context);
         m->b_delete_template_object = YAC_TRUE;
      }
      (void)m->value.object_val->yacArrayAlloc(_size, 1, 0, 0);
   }
}

TKS_ClassDeclMember *TKS_ClassDecl::addScalarMember(YAC_String *_name, sBool _bObject, sU8 _tag, sU8 _tagcount, sUI _prot_flags, sUI _static) {
   if(members.num_elements != members.max_elements)
   {
      TKS_ClassDeclMember *m;
      PTN_NEW_STATIC_NODE(m, TKS_ClassDeclMember)();
      members.elements[members.num_elements++]=m;
      m->class_decl = this;
      m->offset     = (sU8)current_offset;
      m->name.yacCopy(_name);
      if(_static)
      {
         m->b_static = YAC_TRUE;
#ifdef TKS_USE_NODEALLOCATOR
         PTN_NEW_STATIC_NODE(m->co_static, TKS_CachedObject_PN)();
#else
         m->co_static = new(std::nothrow) TKS_CachedObject();
#endif // TKS_USE_NODEALLOCATOR
         m->co_static->value.object_val=0;
      }
      else
      {
         current_offset = (sU16)( current_offset + ((sU16) (1+(_bObject&1))) ); // arrays are objects which use a pointer+deleteme flag structure, hence 2 sUIs
         all_members.num_elements++;
         num_object_members += (_bObject & 1);
      }
      if(_tag)
      {
         m->tagid = _tagcount;
      }
      else
      {
         m->tagid = 0;
      }
      m->protection_flags=_prot_flags;
      return m;
   }
   else
   {
      Dprintf("\n[---] ClassDecl: internal error: member table is full (limit=%i).\n",  members.max_elements);
   }
   return NULL;
}

TKS_ClassDeclMember *TKS_ClassDecl::addADTMember (YAC_String *_name, sBool /*_array*/, sU8 _tag, sU8 _tagcount, sUI _prot_flags, sUI _static) {
   return addScalarMember(_name, 1, _tag, _tagcount, _prot_flags, _static);
}

void TKS_ClassDecl::addInt(YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagcount, sUI _prot_flags, sUI _static, sBool _bRefOnly) {
   TKS_ClassDeclMember *m = addScalarMember(_name, _array, _tag, _tagcount, _prot_flags, _static);
   if(m)
   {
      m->b_noinit = _bRefOnly;
      if(_array)
      {
         m->rtti=TKS_CLASSDECL_RTTI_INTARRAY;
         m->value.object_val=TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_INTARRAY]->template_object;
         if(m->co_static)
         {
            m->co_static->initObject(m->value.object_val, 0);
         }
      }
      else
      {
         m->rtti=TKS_CLASSDECL_RTTI_INT;
         if(m->co_static)
         {
            m->co_static->initInt(0);
         }
      }
   }
}

void TKS_ClassDecl::addFloat(YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagcount, sUI _prot_flags, sUI _static, sBool _bRefOnly) {
   TKS_ClassDeclMember *m = addScalarMember(_name, _array, _tag, _tagcount, _prot_flags, _static);
   if(m)
   {
      m->b_noinit = _bRefOnly;
      if(_array)
      {
         m->rtti=_array ? TKS_CLASSDECL_RTTI_FLOATARRAY : TKS_CLASSDECL_RTTI_FLOAT;
         m->value.object_val = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_FLOATARRAY]->template_object;
         if(m->co_static)
         {
            m->co_static->initObject(m->value.object_val, 0);
         }
      }
      else
      {
         m->rtti=_array ? TKS_CLASSDECL_RTTI_FLOATARRAY : TKS_CLASSDECL_RTTI_FLOAT;
         if(m->co_static)
         {
            m->co_static->initFloat(0.0f);
         }
      }
   }
}

void TKS_ClassDecl::addString(YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagcount, sUI _prot_flags, sUI _static, sBool _bRefOnly) {
   TKS_ClassDeclMember *m = addADTMember(_name, _array, _tag, _tagcount, _prot_flags, _static);
   if(m)
   {
      m->b_noinit = _bRefOnly;
      if(_array)
      {
         m->rtti=TKS_CLASSDECL_RTTI_STRINGARRAY;
         m->value.object_val=TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_STRINGARRAY]->template_object;
         if(m->co_static)
         {
            m->co_static->initObject(m->value.object_val, 0);
         }
      }
      else
      {
         m->rtti=TKS_CLASSDECL_RTTI_STRING;
         m->value.object_val=TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_STRING]->template_object;
         if(m->co_static)
         {
            m->co_static->initString(m->value.string_val, 0);
         }
      }
   }
}

void TKS_ClassDecl::addPointer(YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagcount, sUI _prot_flags, sUI _static, sBool _bRefOnly) {
   TKS_ClassDeclMember *m = addADTMember(_name, _array, _tag, _tagcount, _prot_flags, _static);
   if(m)
   {
      m->b_noinit = _bRefOnly;
      if(_array)
      {
         m->rtti=TKS_CLASSDECL_RTTI_POINTERARRAY;
         m->value.object_val=TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_POINTERARRAY]->template_object;
      }
      else
      {
         m->rtti=TKS_CLASSDECL_RTTI_POINTER;
         m->value.object_val=0;
      }
      if(m->co_static)
      {
         m->co_static->initObject(m->value.object_val, 0);
      }
   }
}

void TKS_ClassDecl::addObject(TKS_ClassTemplate *_tmp, YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagcount, sUI _prot_flags, sUI _static, sBool _bRefOnly) {
   TKS_ClassDeclMember *m = addADTMember(_name, _array, _tag, _tagcount, _prot_flags, _static);
   if(m)
   {
      m->b_noinit = _bRefOnly;
      if(_array)
      {
         // Check whether there is a special array object for the respective template_object
#if 0
         if(m->b_noinit)
         {
            m->rtti = TKS_CLASSDECL_RTTI_OBJECT;
            m->value.object_val = NULL;
         }
         else
#endif
         {
            YAC_Object *a = _tmp->template_object->yacArrayNew();
            if(a != NULL)
            {
               // Use specialized array
               m->rtti = TKS_CLASSDECL_RTTI_OBJECT;
               m->value.object_val = a;
               m->b_delete_template_object=1;
            }
            else
            {
               // Use generic object array
               m->rtti = TKS_CLASSDECL_RTTI_OBJECTARRAY;
               m->value.object_val = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_OBJECTARRAY]->template_object;
               ((YAC_ObjectArray*)m->value.object_val)->template_object = _tmp->template_object;
            }
         }
      }
      else
      {
         m->rtti = TKS_CLASSDECL_RTTI_OBJECT;
         m->value.object_val = _tmp->template_object;
      }
      if(m->co_static)
      {
         m->co_static->initObject(m->value.object_val, 0);
         if(m->b_noinit)
         {
            m->co_static->flags |= TKS_CachedObject::PTRASSIGN | TKS_CachedObject::NOINIT;
         }
      }
   }
}

void TKS_ClassDecl::addClass(TKS_ClassDecl *_cd,
                             YAC_String *   _name,
                             sBool          _array,
                             sU8            _tag,
                             sU8            _tagcount,
                             sUI            _prot_flags,
                             sUI            _static,
                             sBool          _bRefOnly
                             ) {
   TKS_ClassDeclMember *m = addADTMember(_name, _array, _tag, _tagcount, _prot_flags, _static);
   if(m)
   {
      if(_array)
      {
         m->rtti = TKS_CLASSDECL_RTTI_CLASSARRAY;
         if(_bRefOnly)
         {
            m->value.object_val = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_CLASSARRAY];
         }
         else
         {
            YAC_ClassArray *no=(YAC_ClassArray*)YAC_NEW_CORE(YAC_CLID_CLASSARRAY);
            no->class_decl = _cd;
            m->value.object_val = no;
            m->b_delete_template_object = 1;
         }
      }
      else
      {
         m->rtti = TKS_CLASSDECL_RTTI_CLASS;
         m->value.sobject_val = _cd->class_template;
         m->b_noinit = _bRefOnly;
      }
      if(m->co_static)
      {
         m->co_static->initObject(m->value.object_val, 0);
         if(m->b_noinit)
         {
            m->co_static->flags |= TKS_CachedObject::PTRASSIGN | TKS_CachedObject::NOINIT;
         }
      }

      class_field_mask[_cd->class_id] = 1;
   }
}

TKS_ObjectCache *TKS_ClassDecl::getCreateConstants() {
   if(NULL == constants)
   {
      PTN_NEW_STATIC_NODE(constants, TKS_ObjectCache_PN)();
      constants->allocCache(TKS_MAX_CLASSCONSTANTS);
   }
   return constants;
}

TKS_CachedObject *TKS_ClassDecl::findConstant(YAC_String *_name) const {
   if(NULL != constants)
   {
      return constants->findEntry(_name);
   }
   else
   {
      return NULL;
   }
}

TKS_CachedObject *TKS_ClassDecl::findConstantSuper(YAC_String *_name) const {
   TKS_CachedObject *co = findConstant(_name);
   if(NULL == co)
   {
      for(sUI j = 0u; (NULL == co) && (j < class_dependencies.num_elements); j++)
      {
         TKS_ClassDecl *cd = class_dependencies.elements[j];
         co = cd->findConstant(_name);
      }
   }
   return co;
}

TKS_CachedObject *TKS_ClassDecl::addIntConstant(YAC_String *_name) {
   TKS_CachedObject *co = getCreateConstants()->createIntegerEntry(_name, 1);
   co->flags |= TKS_CachedObject::ISCLASSCONSTUNINITIALIZED;
   return co;
}

TKS_CachedObject *TKS_ClassDecl::addFloatConstant(YAC_String *_name) {
   TKS_CachedObject *co = getCreateConstants()->createFloatEntry(_name, 1);
   co->flags |= TKS_CachedObject::ISCLASSCONSTUNINITIALIZED;
   return co;
}

TKS_CachedObject *TKS_ClassDecl::addObjectConstant(YAC_String *_name) {
   TKS_CachedObject *co = getCreateConstants()->createObjectEntry(_name, 1, NULL, 0/*delpointer*/);
   co->flags |= TKS_CachedObject::ISCLASSCONSTUNINITIALIZED;
   return co;
}

TKS_CachedObject *TKS_ClassDecl::addStringConstant(YAC_String *_name) {
   TKS_CachedObject *co = getCreateConstants()->createStringInstance(_name, 1);
   co->flags |= TKS_CachedObject::ISCLASSCONSTUNINITIALIZED;
   return co;
}

sBool TKS_ClassDecl::setConstantDocumentation (YAC_String *_constantName, YAC_String *_doc) {
   //
   // xxx TODO: only copy documentation if emitexplanations is enabled ?!
   //
   TKS_CachedObject * co;
   if(NULL == constants_documentation)
   {
      PTN_NEW_STATIC_NODE(constants_documentation, TKS_ObjectCache_PN)();
      constants_documentation->allocCache(TKS_MAX_CLASSCONSTANTS);
   }
   if(0 == constants_documentation->max_entries)
   {
      co = NULL;
   }
   else
   {
      co = constants_documentation->findEntry(_constantName);
   }
   if(NULL == co)
   {
      YAC_String *t = (YAC_String*) YAC_NEW_CORE(YAC_CLID_STRING);
      t->yacCopy(_doc);
      constants_documentation->createStringEntry(_constantName, 1, t, 1); // xxx re-use keyname from constant..
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

PTN_Function *TKS_ClassDecl::addFunction(YAC_String *_name, sUI _prot_flags, sBool _bAnonymousMutex, YAC_String *_nextMutexName, sUI _srcLoc) {
   PTN_Function *nn;
   PTN_NEW_STATIC_NODE(nn, PTN_Function)();
   PTN_Function *fun      = nn;
   fun->parent_class      = this;
   fun->src_loc           = _srcLoc;
   fun->protection_flags  = _prot_flags;
   fun->is_class_function = YAC_TRUE;
   fun->name.yacCopy(_name);

   if(!fun->initMutex(_bAnonymousMutex, _nextMutexName) )
   {
      Dprintf("[---] TKS_ClassDecl::addFunction: failed to init mutex (class=%s).\n", (char*)name.chars);
      delete fun;
      return NULL;
   }

   if(NULL == functions)
   {
      PTN_NEW_STATIC_NODE(functions, TKS_ObjectCache_PN)();
      functions->allocCache(TKS_MAX_CLASSFUNCTIONS);
   }

   TKS_CachedObject *co = functions->createAnyEntry(&fun->name, 0, fun);
   if(!co)
   {
      delete fun; fun = NULL;
      Dprintf("[---] failed to addFunction(\"%s\").\n", (char*)_name->chars);
   }
   return fun;
}

PTN_Function *TKS_ClassDecl::addMethod(YAC_String *_name,
                                       sUI         _prot_flags,
                                       sBool       _bThisMutex,
                                       sBool       _bAnonymousMutex,
                                       YAC_String *_nextMutexName,
                                       sBool       _bAbstract,
                                       sBool       _bVirtual,
                                       sUI         _srcLoc
                                       ) {
   if(methods.num_elements != methods.max_elements)
   {
      TKS_ClassDeclMethod *m;
      PTN_NEW_STATIC_NODE(m, TKS_ClassDeclMethod)();

      // Initialize function object
      if(!m->function.initMutex(_bAnonymousMutex, _nextMutexName) )
      {
         Dprintf("[---] TKS_ClassDecl::addMethod: failed to init mutex (class=%s).\n", (char*)name.chars);
         delete m;
         return NULL;
      }
      m->function.src_loc            = _srcLoc;
      m->function.parent_class       = this;
      m->function.is_class_function  = YAC_FALSE;
      m->function.name.yacCopy(_name);

      m->class_id                    = class_id; // for Class1.method()
      m->class_decl                  = this;
      m->type                        = TKS_ClassDeclMethod::STD;
      m->protection_flags            = _prot_flags;
      m->b_this_mutex                = _bThisMutex;
      m->b_abstract                  = _bAbstract;
      m->b_virtual                   = _bVirtual;

      if(_bAbstract)
      {
         b_abstract = YAC_TRUE;
      }

      methods.elements[methods.num_elements++] = m;

      if(_name->compare(&name))
      {
         // ---- found constructor ----
         m->type = TKS_ClassDeclMethod::CONSTRUCTOR;
         num_ctors++;
      }
      else
      {
         if('~' == (*_name)[0])
         {
            YAC_String t;
            t.copy("~");
            t.append(&name);
            if(t.compare(_name)) // xxx must be destructor (already validated by parser)
            {
               // ---- found destructor ----
               m->type = TKS_ClassDeclMethod::DESTRUCTOR;
               num_dtors++;
            }
         }
         else
         {
            if('C' == (*_name)[0])
            {
               if(_name->compare("Constructor"))
               {
                  m->type = TKS_ClassDeclMethod::CONSTRUCTOR;
                  num_ctors++;
               }
            }
            else
            {
               if('D' == (*_name)[0])
               {
                  if(_name->compare("Destructor"))
                  {
                     m->type = TKS_ClassDeclMethod::DESTRUCTOR;
                     num_dtors++;
                  }
               }
            }
         }
      }
      return &m->function;
   }
   else
   {
      Dprintf("\n[---] ClassDecl: internal error: method table is full (class name = \"%s\").\n", (char*)name.chars);
      return NULL;
   }
}

sBool TKS_ClassDecl::addAncestor(TKS_ClassDecl *_ancestor) {
   // ---- make sure that this class is not an ancestor of _ancestor
   // ---- otherwise we end up in a stack-overflow trap
   if(NULL != _ancestor->findAncestorDep(this))
   {
      // ---- this class is already baseclass of _ancestor
      return YAC_FALSE;
   }
   else
   {
      ancestor_table[_ancestor->class_id] = 1; // compatible class tag

      if(ancestors.num_elements<ancestors.max_elements)
      {
         ancestors.elements[ancestors.num_elements++]=_ancestor;
         addClassDependency(_ancestor);
         return YAC_TRUE;
      }
      else
      {
         Dprintf("\n[---] ClassDecl: internal error: ancestor table is full.\n");
         return YAC_FALSE;
      }
   }
}

void TKS_ClassDecl::addClassDependency(TKS_ClassDecl *_cd) {
   if(_cd != this)
   {
      for(sUI i = 0u; i < class_dependencies.num_elements; i++)
      {
         if(class_dependencies.elements[i] == _cd)
         {
            // ---- is already a dependency ----
            return;
         }
      }
      // ---- add new dependency
      if(class_dependencies.num_elements < class_dependencies.max_elements)
      {
         class_dependencies.elements[class_dependencies.num_elements++] = _cd;
      }
      else
      {
         Dprintf("\n[---] ClassDecl: internal error: dependency table is full.\n");
      }
   }
}

sU16 TKS_ClassDecl::getNumDependencies(TKS_ClassDecl *_d) {
   sUI r = 0u;
   if(class_dependencies.num_elements)
   {
      sUI num = class_dependencies.num_elements;
      for(sUI i = 0u; i < num; i++)
      {
         TKS_ClassDecl *c = class_dependencies.elements[i];
         if(_d != this)
         {
            _d->addClassDependency(c);
         }
         r += c->getNumDependencies(_d);
      }
   }
   return (sU16)r;
}

void TKS_ClassDecl::calcMemberDataSize(sUI *_mds, sU8 *_mdstable, sU16 *_crt,
                                       sUI *_totalNumMembers, sUI *_totalNumObjectIndices,
                                       sUI *_totalNumMethods, sUI *_totalNumCtors, sUI *_totalNumDtors
                                       )
{
   *_totalNumMembers       += all_members.num_elements;
   *_totalNumObjectIndices += num_object_members;
   *_totalNumMethods       += methods.num_elements;
   *_totalNumCtors         += num_ctors;
   *_totalNumDtors         += num_dtors;
   if(ancestors.num_elements)
   {
      // Traverse ancestors right (base) to left (this)
      sSI i = (sSI) ancestors.num_elements-1;
      while(i >= 0)
      {
         TKS_ClassDecl *dep = ancestors.elements[i--];
         if(!_mdstable[dep->class_id])
         {
            _crt[dep->class_id] = (sU16) *_mds;
            (*_mds) += dep->current_offset;  // add total member data size of dep class (w/o its ancestors)
            _mdstable[dep->class_id] = 1;

            dep->calcMemberDataSize(
               _mds, _mdstable, _crt,
               _totalNumMembers,
               _totalNumObjectIndices,
               _totalNumMethods,
               _totalNumCtors,
               _totalNumDtors
               );
         }
      }
   }
}

void TKS_ClassDecl::initializeStaticMembers(void) {
   for(sUI i=0; i<members.num_elements; i++)
   {
      TKS_ClassDeclMember *m=members.elements[i];
      if(m->b_static)
      {
         if(!m->init_expr)
         {
            m->createStaticInstance();
         }
      }
   }
}

void TKS_ClassDecl::callRemoveStaticInitializers(PTN_Env *_env) {
   // ---- determine/init static member values
   // ---- this is called once for each class

   initializeStaticMembers();

   if(initialization_members)
   {
      StaticList::Node *c = initialization_members;
      while(c)
      {
         if(c->data)
         {
            TKS_ClassInitializationMember *im = (TKS_ClassInitializationMember*)c->data;
            if(im->member)
            {
               if(im->member->b_static)
               {
                  TKS_ClassDeclMember *m = im->member;
                  TKS_CachedObject *co = m->co_static;

                  sBool br = _env->context->b_running;  // xxx ?
                  _env->context->b_running = YAC_TRUE;  // xxx ?

                  YAC_Value r; m->init_expr->eval(_env, &r);

                  _env->context->b_running = br; // xxx ?

                  YAC_Value ar;
                  sUI rtti = m->rtti;
                  if(rtti > YAC_TYPE_STRING)
                  {
                     rtti = YAC_TYPE_OBJECT;
                  }
                  co->type = rtti;

                  switch(m->rtti)
                  {
                     case YAC_TYPE_VOID:
                        r.unset();
                        break;

                     case YAC_TYPE_INT:
                        co->value.int_val = r.getIntValue();
                        r.unset();
                        break;

                     case YAC_TYPE_FLOAT:
                        co->value.float_val = r.getFloatValue();
                        break;

                     default:
                        if(!m->deref_init_expr)
                        {
                           m->createStaticObjectInstance();
                           switch(r.type)
                           {
                              case YAC_TYPE_VOID:
                                 break;

                              case YAC_TYPE_INT:
                                 co->value.object_val->yacOperatorI(YAC_OP_ASSIGN, r.value.int_val, &ar);
                                 ar.unset();
                                 break;

                              case YAC_TYPE_FLOAT:
                                 co->value.object_val->yacOperatorF32(YAC_OP_ASSIGN, r.value.float_val, &ar);
                                 ar.unset();
                                 break;

                              default:
                                 co->value.object_val->yacOperatorAssign(r.value.object_val);
                                 ar.unset();
                                 r.unset();
                                 break;
                           }
                        }
                        else
                        {
                           // ---- just a Pointer, grab object from expression ----
                           // ---- e.g. class MyClass { Pointer p=[1,2,3,4]; }
                           if(r.type >= YAC_TYPE_OBJECT)
                           {
                              // ---- grab object pointer ----
                              co->deleteme=r.deleteme;
                              r.deleteme = YAC_FALSE;
                              co->value.object_val = r.value.object_val;
                           }
                           else
                           {
                              // ---- unset class member object pointer ----
                              co->value.object_val = NULL;
                              co->deleteme = YAC_FALSE;
                           }
                        }
                        break;
                  } // switch member.rtti

               } // if static

            } // if member (paranoia?)

            delete im;

         } // if c->data (paranoia?)
         StaticList::Node *t = c->next;
         StaticList::Remove(&initialization_members, c);
         c = t;
      } // while c
   } // if members

}

void TKS_ClassDecl::callRemoveAncestorStaticInitializers(PTN_Env *_env) {
   // ----
   // ---- call static member initializers of ancestor/base classes
   // ----
   for(sUI k = 0u; k < ancestors.num_elements; k++)
   {
      TKS_ClassDecl *cd = ancestors.elements[k];
      if(!cd->b_static_members_initialized) // already initialized??
      {
         cd->callRemoveAncestorStaticInitializers( _env );
      }
   }
   callRemoveStaticInitializers( _env );
   b_static_members_initialized = YAC_TRUE;
}

sBool TKS_ClassDecl::createClassTemplate(void) {
   // ----
   // ---- create map of effective class members
   // ----
   /*printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nxxx createClassTemplate name=\"%s\" class_id=%d\n",
      name.chars,
      class_id
      );*/
   member_data_size = current_offset;
   // yac_host->printf("xxx TKS_ClassDecl::createClassTemplate: this=%p id=%u name=\"%s\" member_data_size=%u\n", this, class_id, name.chars, member_data_size);
   sBool ancestorTagTable[TKS_MAX_CLASSDECLS];
   for(sUI i = 0u; i < TKS_MAX_CLASSDECLS; i++)
   {
      ancestor_table         [i] = 0u;
      class_relocation_table [i] = 0u;
      ancestorTagTable       [i] = 0u;
   }
   ancestor_table[0]        = 1u; // Class
   ancestor_table[class_id] = 1u;

   sUI totalNumObjectIndices = 0u;
   sUI totalNumMethods = 0u;
   sUI totalNumCtors = 0u;
   sUI totalNumDtors = 0u;

   calcMemberDataSize(
      &member_data_size, ancestor_table, class_relocation_table,
      &all_members.max_elements, &totalNumObjectIndices,
      &totalNumMethods, &totalNumCtors, &totalNumDtors
      );
   // // Dprintf("xxx member_data_size=%d\n", member_data_size);
   // Note: tkui classes avg. member_data_size = 32, peak=95

   // yac_host->printf("xxx TKS_ClassDecl::createClassTemplate: 2\n");

   template_member_hashtable.allocCache(all_members.max_elements + 1);
   object_indices.alloc(totalNumObjectIndices);
   template_method_hashtable.allocCache(totalNumMethods + 1);
   if(all_members.max_elements)
   {
      all_members.elements = new(std::nothrow) TKS_ClassDeclMember*[all_members.max_elements];
   }
   TKS_ScriptClassInstance *cc = class_template;
   if(member_data_size)
   {
      cc->member_data = new(std::nothrow) yacmem[member_data_size];
      ::memset((void*)cc->member_data, 0, sizeof(void*)*member_data_size);
   }
   else
   {
      cc->member_data = NULL;
   }
   sUI off = 0u;
   yacmem *cmd = cc->member_data;

   // Dprintf("xxx TKS_ClassDecl::createClassTemplate: 3\n");

   // Add members
   TKS_ClassDeclMember**acdms = all_members.elements;
   if(!addMembersToTemplate(&cmd, &off, &acdms, &object_indices, &template_member_hashtable, class_id, ancestorTagTable))
   {
      return YAC_FALSE;
   }
   if(off != member_data_size)
   {
      Dprintf("\n[---] off(%i) and mdsize(%i) differ\n", off, member_data_size);
      return YAC_FALSE;
   }

   // Dprintf("xxx TKS_ClassDecl::createClassTemplate: 4\n");

   // Add methods
   memset(ancestorTagTable, 0, sizeof(sBool)*TKS_MAX_CLASSDECLS);
   if(!addMethodsToTemplate(&template_method_hashtable, class_id, ancestorTagTable))
   {
      return YAC_FALSE;
   }

   // ----
   // ---- resolved shifted_offset in TKClassInitializationMember list ----
   // ----
   addAncestorInitializationMembers(this);


   if(effective_init_members)
   {
      StaticList::Node *c = effective_init_members;
      while(c)
      {
         if(c->data)
         {
            TKS_ClassInitializationMember *im = (TKS_ClassInitializationMember*)c->data;
            TKS_CachedObject *co = template_member_hashtable.findEntry(&im->member->name);
            im->shifted_offset = (co->value.int_val & TKS_CLASSDECL_OFFMASK);
            im->shifted_offset += class_relocation_table[(co->value.int_val >> TKS_CLASSDECL_IDSHIFT) & 0xFFFF];
         }
         c = c->next;
      }
   }

   // Dprintf("xxx TKS_ClassDecl::createClassTemplate: 5\n");

   if(totalNumCtors)
   {
      constructor_list.alloc(totalNumCtors);
      memset(ancestorTagTable, 0, sizeof(sBool)*TKS_MAX_CLASSDECLS);
      addConstructorsToTemplate(&constructor_list, ancestorTagTable);
   }

   // Dprintf("xxx TKS_ClassDecl::createClassTemplate: 6\n");

   if(totalNumDtors)
   {
      destructor_list.alloc(totalNumDtors);
      memset(ancestorTagTable, 0, sizeof(sBool)*TKS_MAX_CLASSDECLS);
      addDestructorsToTemplate(&destructor_list, ancestorTagTable);
   }

   // Dprintf("xxx TKS_ClassDecl::createClassTemplate: 7\n");

   template_initialized = YAC_TRUE;
   return YAC_TRUE;
}

void TKS_ClassDecl::addAncestorInitializationMembers(TKS_ClassDecl *_target) {
   // ----
   // ---- add effective initialization members
   // ----
   for(sUI ai = 0u; ai < ancestors.num_elements; ai++)
   {
      ancestors.elements[ai]->addAncestorInitializationMembers(_target);
   }
   addInitializationMembers(_target);
}

void TKS_ClassDecl::addInitializationMembers(TKS_ClassDecl *_target) {
   if(initialization_members)
   {
      StaticList::Node *c = initialization_members;
      while(c)
      {
         TKS_ClassInitializationMember *o = (TKS_ClassInitializationMember *)c->data;
         if(YAC_FALSE == o->member->b_static)
         {
            TKS_ClassInitializationMember *m;
            PTN_NEW_STATIC_NODE(m, TKS_ClassInitializationMember);
            m->member = o->member;
            m->shifted_offset = (sUI)-1;
            if(NULL != _target->effective_init_members)
            {
               // Append to end of list (TODO: speed this up, avoid Tail() call..)
               StaticList::Node *t = StaticList::Tail(_target->effective_init_members);
               t->next           = STATICLIST_NEW_NODE;
               t->next->previous = t;
               t->next->next     = NULL;
               t->next->data     = (void *)m;
            }
            else
            {
               StaticList::InsertHead(&_target->effective_init_members, (void *)m);
            }
         }
         c = c->next;
      }
   }
}

sBool TKS_ClassDecl::addMembersToTemplate(yacmem**              _cmd,
                                          sUI *                 _off,
                                          TKS_ClassDeclMember***_allMembers,
                                          Array<sU16>*          _objectindices,
                                          TKS_ObjectCache *     _memberhash,
                                          sU16                  _class_id,
                                          sBool *               _classTags
                                          )
{
   // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u _classID=%u\n", class_id, _class_id);
   if(_classTags[class_id])
   {
      // Members already added, nothing to do here
      return YAC_TRUE;
   }
   _classTags[class_id] = YAC_TRUE;

   yacmem *cmd = *_cmd;
   sUI off = 0u;
   sUI k;
   for(k = 0u; k < members.num_elements; k++)
   {
      TKS_ClassDeclMember *m = members.elements[k];
      // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u members[%u]=%p\n", class_id, k, m);
      // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u   m->b_static=%d\n", class_id, m->b_static);
      if(m->b_static)
      {
         continue;
      }

      *(*_allMembers)++ = m;
      TKS_CachedObject *co = _memberhash->createIntegerEntry(&m->name, 0);
      if(!co)
      {
         Dprintf("\n[---] createClassTemplate: failed to insert member into hashtable.\n");
         return YAC_FALSE;
      }

      if((off + *_off) >= TKS_CLASSDECL_OFFMASK)
      {
         Dprintf("\n[---] createClassTemplate: shifted base class member offset (%d) exceeds %d\n", (off + *_off), TKS_CLASSDECL_OFFMASK-1);
         return YAC_FALSE;
      }

      // (note) 'off' is relative to current class. it will be relocated to the actual offset via the 'class_relocation_table' array
      co->value.int_val = (off) |(m->rtti << TKS_CLASSDECL_RTTISHIFT) | (class_id << TKS_CLASSDECL_IDSHIFT);
      YAC_Object *to;
      // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u   m->rtti=%d\n", class_id, m->rtti & TKS_CLASSDECL_RTTIMASK);

      switch(m->rtti & TKS_CLASSDECL_RTTIMASK)
      {
         case TKS_CLASSDECL_RTTI_INT:
         case TKS_CLASSDECL_RTTI_FLOAT:
            cmd++;
            off++;
            break;

         case TKS_CLASSDECL_RTTI_INTARRAY:
            if(m->b_noinit)
            {
               cmd += 2;
            }
            else
            {
               cmd++->si = 1;
               to = YAC_NEW_CORE(YAC_CLID_INTARRAY);
               to->yacArrayCopySize(m->value.object_val);
               cmd++->o = to;
            }
            _objectindices->elements[_objectindices->num_elements++]=(sU16)(off + *_off);
            off += 2;
            break;

         case TKS_CLASSDECL_RTTI_FLOATARRAY:
            if(m->b_noinit)
            {
               cmd += 2;
            }
            else
            {
               cmd++->si = 1;
               to = YAC_NEW_CORE(YAC_CLID_FLOATARRAY);
               to->yacArrayCopySize(m->value.object_val);
               cmd++->o = to;
            }
            _objectindices->elements[_objectindices->num_elements++] = (sU16)(off + *_off);
            off += 2;
            break;

         case TKS_CLASSDECL_RTTI_STRING:
            if(m->b_noinit)
            {
               cmd += 2;
            }
            else
            {
               cmd++->si = 1;
               to  = YAC_NEW_CORE(YAC_CLID_STRING);
               cmd++->o = to;
            }
            _objectindices->elements[_objectindices->num_elements++] = (sU16)(off + *_off);
            off += 2;
            break;

         case TKS_CLASSDECL_RTTI_STRINGARRAY:
            if(m->b_noinit)
            {
               cmd += 2;
            }
            else
            {
               cmd++->si = 1;
               to = YAC_NEW_CORE(YAC_CLID_STRINGARRAY);
               to->yacArrayCopySize(m->value.object_val);
               cmd++->o = to;
            }
            _objectindices->elements[_objectindices->num_elements++] = (sU16)(off + *_off);
            off += 2;
            break;

         case TKS_CLASSDECL_RTTI_OBJECT:
            if(m->b_noinit)
            {
               cmd += 2;
            }
            else
            {
               cmd++->si = 1;
               cmd++->o  = YAC_CLONE_POOLED(NULL, m->value.object_val);
            }
            _objectindices->elements[_objectindices->num_elements++] = (sU16)(off + *_off);
            off += 2;
            break;

         case TKS_CLASSDECL_RTTI_OBJECTARRAY:
            if(m->b_noinit)
            {
               cmd += 2;
            }
            else
            {
               cmd++->si = 1;
               to = YAC_CLONE_POOLED(NULL, m->value.object_val);
               to->yacArrayCopySize(m->value.object_val);
               cmd++->o = to;
            }
            _objectindices->elements[_objectindices->num_elements++] = (sU16)(off + *_off);
            off += 2;
            break;

         case TKS_CLASSDECL_RTTI_CLASS:
            // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u   m->value.sobject_val=%p\n", class_id, m->value.sobject_val);
            // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u   m->value.sobject_val->class_decl=%p\n", class_id, m->value.sobject_val->class_decl);
            // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u   m->value.sobject_val->class_decl=%p id=%u name=\"%s\"\n", class_id, m->value.sobject_val->class_decl, m->value.sobject_val->class_decl->class_id, m->value.sobject_val->class_decl->name.chars);
            if(m->b_noinit || m->value.sobject_val->class_decl->b_abstract)
            {
               // Do not instantiate abstract class
               cmd += 2;
            }
            else
            {
               cmd++->si = 1;
               {
                  YAC_Object *t;
                  t = NULL;

                  // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u   m->value.sobject_val->class_decl->class_id=%u\n", class_id, m->value.sobject_val->class_decl->class_id);
                  // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u   ancestor_table[%u]=%d\n", class_id, m->value.sobject_val->class_decl->class_id, ancestor_table[m->value.sobject_val->class_decl->class_id]);
                  // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u m->value.sobject_val->class_decl->class_field_mask=%p\n", class_id, m->value.sobject_val->class_decl->class_field_mask);

                  if(1 == ancestor_table[m->value.sobject_val->class_decl->class_id])  // 19Nov2015: allow instanceof baseclass as member in derived class, even if baseclass references back to derived class
                  {
                     // Member is base class of this
                     m->value.sobject_val->class_decl->class_field_mask[class_id] = 0; // allow instantiation
                     // (note) the issue at hand is that the class member may have been added before the derived class was declared, this fixes the instantiation flag when all class decls are known
                  }


                  if(
                     (m->value.sobject_val->class_decl->ancestor_table[class_id]) ||
                     (m->value.sobject_val->class_decl->class_field_mask[class_id]) // member is same or baseclass of this
                     )
                  {
                     ////t=0;//t=class_template; avoid recursion, no instanciation
                     cmd[-1].si = 0; // dont delete self
                  }
                  else
                  {
                     // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u   m->value.sobject_val->class_decl->template_initialized=%d\n", class_id, m->value.sobject_val->class_decl->template_initialized);
                     if(m->value.sobject_val->class_decl->template_initialized)
                     {
                        t = YAC_CLONE_POOLED(NULL, m->value.object_val);
                     }
                     else
                     {
                        t = NULL;
                        if(!m->value.sobject_val->class_decl->createClassTemplate())
                        {
                           return YAC_FALSE;
                        }
                        t = YAC_CLONE_POOLED(NULL, m->value.object_val);
                        ////m->value.sobject_val->class_decl->addForwardEntry(((TKS_ScriptClassInstance**)cmd));
                        ////t=0;
                     }
                  }
                  cmd++->o = t;
               }
            }
            _objectindices->elements[_objectindices->num_elements++] = (sU16)(off + *_off);
            off += 2;
            break;

         case TKS_CLASSDECL_RTTI_CLASSARRAY:
            if(m->b_noinit)
            {
               cmd += 2;
            }
            else
            {
               cmd++->si = 1;
               to = YAC_CLONE_POOLED(NULL, m->value.object_val);
               to->yacArrayCopySize(m->value.object_val);
               cmd++->o = to;
            }
            _objectindices->elements[_objectindices->num_elements++] = (sU16)(off + *_off);
            off += 2;
            break;

         case TKS_CLASSDECL_RTTI_POINTER:
            cmd += 2;
            _objectindices->elements[_objectindices->num_elements++] = (sU16)(off + *_off);
            off += 2;
            break;

         case TKS_CLASSDECL_RTTI_POINTERARRAY:
            if(m->b_noinit)
            {
               cmd += 2;
            }
            else
            {
               cmd++->si = 1;
               to = YAC_NEW_CORE(YAC_CLID_POINTERARRAY);
               to->yacArrayCopySize(m->value.object_val);
               cmd++->o = to;
            }
            _objectindices->elements[_objectindices->num_elements++] = (sU16)(off + *_off);
            off += 2;
            break;
      }

      // yac_host->printf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u   m[%u] OK\n", class_id, k);
   }
   *_cmd = cmd;
   *_off += off;

   // Dprintf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u 2\n", class_id);

   if(ancestors.num_elements)
   {
      // Traverse ancestors right (base) to left (this)
      sSI i = (sSI) ancestors.num_elements-1;
      while(i >= 0)
      {
         TKS_ClassDecl *dep=ancestors.elements[i--];
         if(!dep->addMembersToTemplate(_cmd, _off, _allMembers, _objectindices, _memberhash, _class_id, _classTags))
         {
            return YAC_FALSE;
         }
      }
   }

   // Dprintf("xxx TKS_ClassDecl::addMembersToTemplate: this_class_id=%u LEAVE (ok)\n", class_id);

   return YAC_TRUE;
}

sBool TKS_ClassDecl::addMethodsToTemplate(TKS_ObjectCache *_methodhash, sU16 _rclid, sBool *_ancestorTagTable) {
   sUI i;
   // Dprintf("xxx .... addMethodsToTemplate: this=%p rclid=%u\n", this, _rclid);

   if(_ancestorTagTable[class_id])
   {
      return YAC_TRUE;
   }
   _ancestorTagTable[class_id] = 1;

   TKS_ClassDecl *rclass = Dtkscompiler->class_decls.elements[_rclid];

   /*printf("\n........addAncestorMethods: name=\"%s\" class_id=%d rclid=%d.............\n",
      name.chars,
      class_id,
      _rclid
      );*/
   //
   // Add methods from last ancestor first so the search order is left to right (e.g. CTest : CBase2, CBase1)
   //

   sUI j = ancestors.num_elements;
   for(i = 0u; i < ancestors.num_elements; i++)
   {
      TKS_ClassDecl *dep = ancestors.elements[--j];
      if(!dep->addMethodsToTemplate(_methodhash, _rclid, _ancestorTagTable))
      {
         return YAC_FALSE;
      }
   }

   // // Dprintf("xxx addMethodsToTemplate: class=\"%s\" class_id=%d rclid=%d\n", name.chars, class_id, _rclid);

   //
   // loop "own" methods and make sure that formal argument list matches the base
   // class method, if any.
   //
   for(i = 0u; i < methods.num_elements; i++)
   {
      TKS_ClassDeclMethod *cm = methods.elements[i];
      TKS_CachedObject *co = _methodhash->createIntegerEntry(&cm->function.name, 0); // xxx do not duplicate key string!!
      co->value.object_val = (YAC_Object*)(cm);
      sUI k; // ---- scan all ancestors and find method, mk entry in vtable
      sBool bExtendsCheck = !cm->b_virtual; //// || (class_id==_rclid);

      // Note: need to iterate all ancestor classes of real class so that inheritance "networks" like
      //         in class_virtualdispatch_network.tks work
      for(k = (rclass->class_dependencies.num_elements - 1u); sSI(k) >= 0; k--)
      {
         TKS_ClassDecl *bc = rclass->class_dependencies.elements[k];
         // // Dprintf("xxx addMethodsToTemplate: bc=\"%s\" ##################### this=\"%s\"\n", bc->name.chars, name.chars);
         TKS_ClassDeclMethod *bcm = bc->findMethod(&cm->function.name);
         // // Dprintf("xxx      find bcm \"%s\" = %p\n", cm->function.name.chars, bcm);
         if(bcm)
         {
            if(cm->b_abstract && (bc != this) && (ancestor_table[bc->class_id]))
            {
#ifdef TKS_DCON
               Dprintf("[---] abstract method \"%s::%s::%s\" already declared in base class \"%s::%s\".\n",
                       (char*)nspace->name.chars,
                       (char*)name.chars,
                       (char*)cm->function.name.chars,
                       (char*)bc->nspace->name.chars,
                       (char*)bc->name.chars,
                       (char*)bcm->function.name.chars
                       );
#endif
               return YAC_FALSE;
            }

            // Verify that both methods use the same parameter signature
            if(ancestor_table[bc->class_id])
            {
               if(!cm->function.compareFormalArgs(&bcm->function))
               {
#ifdef TKS_DCON
                  YAC_String s_cm_args; cm->function.formalArgsToString(&s_cm_args);
                  YAC_String s_bcm_args; bcm->function.formalArgsToString(&s_bcm_args);

                  Dprintf("[---] formal argument type mismatch in declaration of method %s::%s::%s:\n[---]    have %s(%s)\n[---]    expected %s(%s) (in base class %s::%s)\n",
                          (char*)nspace->name.chars,       // method
                          (char*)name.chars,
                          (char*)cm->function.name.chars,
                          (char*)cm->function.name.chars,  // have
                          (char*)s_cm_args.chars,
                          (char*)bcm->function.name.chars, // expected
                          (char*)s_bcm_args.chars,
                          (char*)bc->nspace->name.chars,
                          (char*)bc->name.chars
                          );
#endif
                  return YAC_FALSE;
               }
            }

            bExtendsCheck = YAC_TRUE;

            if(NULL == bcm->vtable)
            {
               // Dprintf("xxx allocVTable k=%u i=%u\n", k, i);
               bcm->allocVTable();
            }

            /*printf("xxx addMethodsToTemplate: force set bc \"%s\" vtable[%d] (curr=%p) to \"%s::%s\".\n",
                  bc->name.chars,
                  class_id,
                  bcm->vtable[class_id],
                  name.chars, cm->function.name.chars
                  );*/
            bcm->vtable[class_id] = cm;
            // // bcm->vtable[bcm->class_id] = cm;

            /*printf("xxx addMethodsToTemplate: set bc \"%s\" vtable[%d] (curr=%p) to \"%s::%s\".\n",
                  bc->name.chars,
                  _rclid,
                  bcm->vtable[_rclid],
                  name.chars, cm->function.name.chars
                  );*/
            ////if(!bcm->vtable[_rclid]) // DerivedClass inherits method
            {
               bcm->vtable[_rclid] = cm;
            }

         } // if bcm

      }

      if(!bExtendsCheck)
      {
         //
         // no base class method found for virtual override ("public method extends mymethod()")
         //
         Dprintf("[---] base class implementation required but not found for method \"%s::%s::%s\".\n",
                 nspace ? (sChar*) nspace->name.chars : "",
                 (sChar*) name.chars,
                 (sChar*) cm->function.name.chars
                 );
         return YAC_FALSE;
      }

      if(_rclid != class_id)
      {
         if(NULL == cm->vtable)
         {
            cm->allocVTable();
         }
         /*printf("xxx addMethodsToTemplate: set this \"%s\" vtable[%d/%d] (curr=%p) to \"%s::%s\".\n",
            name.chars,
            _rclid, class_id,
            cm->vtable[_rclid],
            name.chars, cm->function.name.chars
            );*/
         ////if(!cm->vtable[_rclid])
         {
            cm->vtable[_rclid] = cm;
            /////cm->vtable[class_id] = cm;
         }
      }

   } // loop "own" methods

   return YAC_TRUE;
}


void TKS_ClassDecl::addConstructorsToTemplate(Array<TKS_ClassDeclMethod*>*_l, sBool *_ancestorTagTable) {
   if(_ancestorTagTable[class_id])
   {
      // Already added
      return;
   }
   _ancestorTagTable[class_id] = 1;

   sSI i;

   i = (sSI) ancestors.num_elements-1;
   for(; i >= 0; i--)
   {
      TKS_ClassDecl *ac = ancestors.elements[i];
      ac->addConstructorsToTemplate(_l, _ancestorTagTable);
   }

   sSI ne = (sSI) methods.num_elements;
   for(i = 0; i < ne; i++)
   {
      TKS_ClassDeclMethod *m=methods.elements[i];
      if(TKS_ClassDeclMethod::CONSTRUCTOR == m->type)
      {
         _l->elements[_l->num_elements++] = m;
      }
   }
}

void TKS_ClassDecl::addDestructorsToTemplate(Array<TKS_ClassDeclMethod*>*_l, sBool *_ancestorTagTable) {
   if(_ancestorTagTable[class_id])
   {
      // Already added
      return;
   }
   _ancestorTagTable[class_id] = 1;

   sUI i;
   sUI ne = methods.num_elements;
   for(i = 0u; i < ne; i++)
   {
      TKS_ClassDeclMethod *m = methods.elements[i];
      if(TKS_ClassDeclMethod::DESTRUCTOR == m->type)
      {
         _l->elements[_l->num_elements++] = m;
      }
   }
   ne = ancestors.num_elements;
   for(i = 0u; i < ne; i++)
   {
      TKS_ClassDecl *ac = ancestors.elements[i];
      ac->addDestructorsToTemplate(_l, _ancestorTagTable);
   }
}

sBool TKS_ClassDecl::allowFunctionCall(
                                       PTN_Function *          _f,
                                       const TKS_CachedScript *_caller_mod,
                                       const TKS_ClassDecl *   _caller_class,
                                       TKS_Namespace *         _caller_nspace
                                       ) const {
   sBool ret = YAC_FALSE;

   switch(_f->protection_flags)
   {
      case TKS_PROT_NONE:
      case TKS_PROT_PUBLIC:
         ret = YAC_TRUE;
         break;

      case TKS_PROT_PROTECTED:
         if(_caller_class)
         {
            ret = (_caller_class == _f->parent_class) || (NULL != _caller_class->findAncestorDep(_f->parent_class));
         }
         else
         {
            ret = YAC_FALSE;
         }
         break;

      case TKS_PROT_PRIVATE:
         ret = (_caller_class == _f->parent_class);
         break;

      case TKS_PROT_MODULE:
         ret = (_caller_mod == _f->parent_class->module);
         if(!ret)
         {
            Dprintf("[---] call to module function \"%s\" of class \"%s::%s\" in module \"%s\" from module \"%s\" is prohibited due to access restrictions.\n",
                    (char*)_f->name.chars,
                    (nspace ? (char*) nspace->name.chars : ""),
                    (char*)name.chars,
                    (char*)_f->parent_class->module->name.chars,
                    (char*)_caller_mod->name.chars
                    );
         }
         return ret;

      case TKS_PROT_NSPACE:
         ret = (_f->parent_class->nspace == _caller_nspace);
         if(!ret)
         {
            Dprintf("[---] call to namespace function \"%s\" of class \"%s::%s\" from namespace \"%s\" is prohibited due to access restrictions.\n",
                    (char*)_f->name.chars,
                    (nspace? (char*)nspace->name.chars : ""),
                    (char*)name.chars,
                    (char*)_caller_nspace->name.chars
                    );
         }
         return ret;
   }

   if(!ret)
   {
      Dprintf("[---] call to %s function \"%s\" of class \"%s::%s\" is prohibited due to access restrictions.\n",
         tks_prot_typenames[_f->protection_flags],
         (char*)_f->name.chars,
         (nspace? (char*)nspace->name.chars : ""),
         (char*)name.chars
         );
   }
   return ret;
}

sBool TKS_ClassDecl::allowMethodCall(YAC_String *            _mn,
                                     const TKS_CachedScript *_caller_mod,
                                     const TKS_ClassDecl *   _caller_class,
                                     TKS_Namespace *         _caller_nspace
                                     ) const {
   TKS_ClassDeclMethod *method = findMethodSuper(_mn);
   sBool ret = YAC_FALSE;
   switch(method->protection_flags)
   {
      case TKS_PROT_NONE:
      case TKS_PROT_PUBLIC:
         ret = YAC_TRUE;
         break;

      case TKS_PROT_PROTECTED:
         if(_caller_class)
         {
            ret = (_caller_class == method->class_decl) || (NULL != _caller_class->findAncestorDep(method->class_decl));
         }
         else
         {
            ret = YAC_FALSE;
         }
         break;

      case TKS_PROT_PRIVATE:
         ret = (_caller_class == method->class_decl);
         break;

      case TKS_PROT_MODULE:
         ret = (_caller_mod == method->class_decl->module);
         if(!ret)
         {
            Dprintf("[---] call to module member \"%s\" of class \"%s::%s\" in module \"%s\" from module \"%s\" is prohibited due to access restrictions.\n",
                    (char*)_mn->chars,
                    (nspace ? (char*) nspace->name.chars : ""),
                    (char*)name.chars,
                    (char*)method->class_decl->module->name.chars,
                    (char*)_caller_mod->name.chars
                    );
         }
         return ret;

      case TKS_PROT_NSPACE:
         ret = (method->class_decl->nspace == _caller_nspace);
         if(!ret)
         {
            Dprintf("[---] call to namespace method \"%s\" of class \"%s::%s\" from namespace \"%s\" is prohibited due to access restrictions.\n",
                    (char*)_mn->chars,
                    (method->class_decl->nspace ? (char*)method->class_decl->nspace->name.chars : ""),
                    (char*)name.chars,
                    (char*)_caller_nspace->name.chars
                    );
         }
         return ret;
   }

   if(!ret)
   {
      Dprintf("[---] call to %s method \"%s\" of class \"%s::%s\" is prohibited due to access restrictions.\n",
              tks_prot_typenames[method->protection_flags],
              (char*)_mn->chars,
              (method->class_decl->nspace ? (char*)method->class_decl->nspace->name.chars : ""),
              (char*)name.chars
              );
   }
   return ret;
}

sBool TKS_ClassDecl::allowMemberAccess(YAC_String     *        _mn,
                                       const TKS_CachedScript *_caller_mod,
                                       const TKS_ClassDecl    *_caller_class,
                                       TKS_Namespace *         _caller_nspace
                                       ) const {
   sBool ret = YAC_FALSE;
   TKS_ClassDeclMember *member = findMemberSuper(_mn, 0/*non-static*/);
   if(!member)
   {
      member = findMemberSuper(_mn, 1/*static*/);
   }
   switch(member->protection_flags)
   {
      case TKS_PROT_NONE:
      case TKS_PROT_PUBLIC:
         ret = YAC_TRUE;
         break;

      case TKS_PROT_PROTECTED:
         if(_caller_class)
         {
            ret = (_caller_class == member->class_decl) || (NULL != _caller_class->findAncestorDep(member->class_decl));
         }
         else
         {
            ret = YAC_FALSE;
         }
         break;

      case TKS_PROT_PRIVATE:
         ret= (_caller_class == member->class_decl);
         break;

      case TKS_PROT_MODULE:
         ret = (_caller_mod == member->class_decl->module);
         if(!ret)
         {
            Dprintf("[---] access to module member \"%s\" of class \"%s::%s\" in module \"%s\" from module \"%s\" is prohibited due to access restrictions.\n",
                    (char*)_mn->chars,
                    (nspace ? (char*) nspace->name.chars : ""),
                    (char*)name.chars,
                    (char*)member->class_decl->module->name.chars,
                    (char*)_caller_mod->name.chars
                    );
         }
         return ret;

      case TKS_PROT_NSPACE:
         ret = (_caller_nspace == member->class_decl->nspace);
         if(!ret)
         {
            Dprintf("[---] access to namespace member \"%s\" of class \"%s::%s\" from namespace \"%s\" is prohibited due to access restrictions.\n",
                    (char*)_mn->chars,
                    (member->class_decl->nspace ? (char*)member->class_decl->nspace->name.chars : ""),
                    (char*)name.chars,
                    (char*)_caller_nspace->name.chars
                    );
         }
         return ret;
   }

   if(!ret)
   {
      Dprintf("[---] access to %s member \"%s\" of class \"%s::%s\" is prohibited due to access restrictions.\n",
              tks_prot_typenames[member->protection_flags],
              (char*)_mn->chars,
              (member->class_decl->nspace ? (char*)member->class_decl->nspace->name.chars : ""),
              (char*)name.chars
              );
   }
   return ret;
}

sBool TKS_ClassDecl::resolveFunctionArguments(void) {
   sBool ret = YAC_TRUE;
   // Dprintf("xxx TKS_ClassDecl::resolveFunctionArguments: cd->name=\"%s\" functions=%p\n", (const char*)name.chars, functions);
   if(NULL != functions)
   {
      for(sUI i = 0u; ret && i < functions->max_entries; i++)
      {
         TKS_CachedObject *co = functions->objects[i];
         if(co)
         {
            PTN_Function *f = (PTN_Function *)co->value.object_val;
            // Dprintf("xxx TKS_ClassDecl::resolveFunctionArguments: cd->name=\"%s\" f->name=\"%s\"\n", (const char*)name.chars, (const char*)f->name.chars);
            ret = ret && f->resolveFormalArgs();
            // ---- error output is in function::resolveFormalArgs
         }
      }
   }
   return ret;
}

sBool TKS_ClassDecl::resolveMethodArguments(void) {
   sBool ret = YAC_TRUE;
   for(sUI i = 0u; ret && (i < methods.num_elements); i++)
   {
      TKS_ClassDeclMethod *cdm = methods.elements[i];
      ret = ret && cdm->function.resolveFormalArgs();
   }
   return ret;
}

sBool TKS_ClassDecl::initializeFunctions(void) {
   // Dprintf("xxx TKS_ClassDecl::initializeFunctions: name=\"%s\" functions=%p\n", (const char*)name.chars, functions);
   b_init_functions_done = YAC_TRUE;
   if(NULL != functions)
   {
      for(sUI i = 0u; (i < functions->max_entries); i++)
      {
         TKS_CachedObject *co = functions->objects[i];
         if(co)
         {
            PTN_Function *f = (PTN_Function *)co->value.object_val;
            if(!f->semanticCheck())
            {
               Dprintf("[---] semanticCheck failed for \"%s::%s\".\n",
                       (char*)name.chars, (char*)f->name.chars
                       );
               return YAC_FALSE;
            }

            if(!f->resolveXRef())
            {
               Dprintf("[---] resolveXRef failed for \"%s::%s\".\n",
                       (char*)name.chars,
                       (char*)f->name.chars);
               return YAC_FALSE;
            }
            f->optimize();
         }
      }
   }
   return YAC_TRUE;
}

void TKS_ClassDecl::deleteStaticMembers(void) {
   TKS_ClassDeclMember**m = members.elements;
   for(sUI i = 0u; i < members.num_elements; i++)
   {
      if((*m)->co_static)
      {
         (*m)->co_static->unset();
      }
   }
}

PTN_Function *TKS_ClassDecl::resolveCallback(const char *_name) const {
   YAC_String t;
   t.visit(_name);
   TKS_ClassDeclMethod *cdm = getMethod(&t);
   if(cdm)
   {
      return &cdm->function;
   }
   else
   {
      return NULL;
   }
}

void TKS_ClassDecl::optimize(void) {
#define KEEP_INFO_FOR_DYNAMIC_TSLS  defined

   // ---- optimize method pointer array
   if(methods.num_elements < methods.max_elements)
   {
      TKS_ClassDeclMethod**ma;
      if(methods.num_elements)
      {
         ma = new(std::nothrow) TKS_ClassDeclMethod*[methods.num_elements];
         // (todo) handle OOM
         for(sUI i = 0u; i < methods.num_elements; i++)
         {
            ma[i] = methods.elements[i];
         }
      }
      else
      {
         ma = NULL;
      }
      delete [] methods.elements;
      methods.elements     = ma;
      methods.max_elements = methods.num_elements;
   }

   // ---- optimize member pointer array
   if(members.num_elements < members.max_elements)
   {
      TKS_ClassDeclMember**ma;
      if(members.num_elements)
      {
         ma = new(std::nothrow) TKS_ClassDeclMember*[members.num_elements];
         // (todo) handle OOM
         // (todo) use memcpy
         for(sUI i = 0u; i < members.num_elements; i++)
         {
            ma[i] = members.elements[i];
         }
      }
      else
      {
         ma = NULL;
      }
      delete [] members.elements;
      members.elements     = ma;
      members.max_elements = members.num_elements;
   }

   // ---- optimize method functions
   for(sUI i = 0u; i < methods.num_elements; i++)
   {
      TKS_ClassDeclMethod *m = methods.elements[i];
      m->function.optimize();
   }

#ifndef KEEP_INFO_FOR_DYNAMIC_TSLS
   // Delete member ancestor table
   //   (note) [24Sep2023] keep it, may be needed for dynamically loaded TSLs later on
   if(NULL != class_field_mask)
   {
      delete [] class_field_mask;
      class_field_mask = NULL;
   }
#endif // KEEP_INFO_FOR_DYNAMIC_TSLS

   // Resolve yacOperatorAssign callback fxn
   callback.fun_yacOperatorAssign = resolveCallback("yacOperatorAssign");

   // Resolve yacToString callback fxn
   callback.fun_yacToString = resolveCallback("yacToString");

   // Resolve yacOperator callback fxn
   callback.fun_yacOperator = resolveCallback("yacOperator");
}

#if 0
// Note: already done by TKS_ClassDeclMember::createStaticObjectInstance()
void TKS_ClassDecl::fixAbstractVars(void) {
   sUI i;
   for(i=0; i<members.num_elements; i++)
   {
      TKS_ClassDeclMember *m = members.elements[i];
      if(m->b_static && m->b_noinit)
      {
         m->co_static->initNull();
      }
   }
}
#endif // 0
