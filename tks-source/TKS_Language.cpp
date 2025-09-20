/// TKS_Language.cpp
///
/// (c) 2002-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include "tks.h"

#ifdef DX_PTHREADS
#include <sched.h>
#endif

#include "tks_inc_compiler.h"
#include "tks_inc_class.h"

#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "YAC_ListNode.h"
#include "YAC_StringArray.h"
#include "YAC_PointerArray.h"
#include "YAC_HashTable.h"

#include "TKS_Mutex.h"
#include "TKS_Condition.h"
#include "TKS_Mailbox.h"
#include "TKS_Thread.h"
#include "TKS_ScriptModule.h"

#include "TKS_Language.h"

#ifdef YAC_WIN32
#include <windows.h>

#include "tks_debug.h"
#endif // YAC_WIN32

#ifdef YAC_POSIX
#include <unistd.h>
#endif // YAC_POSIX


extern void tks_get_extrafile_stringarray (YAC_Value *_r);  // see main.cpp


TKS_Language::TKS_Language(void) {
   s_versionString.visit(TKS_VERSION_STR);
}

TKS_Language::~TKS_Language() { }

void YAC_VCALL TKS_Language::yacGetSignalStringList(YAC_String *_sig) {
   _sig->visit("onSignal:1 onSegV:0 onSegVContinue:0"); // TKS_SIG_SIGNAL, TKS_SIG_SEGV, TKS_SIG_SEGVCONTINUE
}

void YAC_VCALL TKS_Language::yacRegisterSignal(sUI _id, YAC_FunctionHandle _fun) {
   switch(_id)
   {
   case TKS_SIG_SIGNAL:       tkscript->sig_signal       = _fun; break;
   case TKS_SIG_SEGV:         tkscript->sig_segv         = _fun; break;
   case TKS_SIG_SEGVCONTINUE: tkscript->sig_segvcontinue = _fun; break;
   default:
      Dprintf("[---] TKS::yacRegisterSignal: unknown signal id=%i.\n", _id);
      break;
   }
}

YAC_String *TKS_Language::constantToString(sSI _id, YAC_String *ps) {
   sBool psblk=ps->isBlank();
   TKS_CachedObject **o=tkscript->global_constant_namespace.objects;
   if(o)
   {
      sUI i;
      for(i=0; i<tkscript->global_constant_namespace.max_entries; i++)
      {
         if(o[i])
         {
            if(o[i]->value.int_val==_id)
            {
               if((psblk)||(o[i]->name.indexOf(ps, 0)==0))
               {
                  return &o[i]->name;
               }
            }
         }

      }
   }
   o=tkscript->uconst_table.objects;
   if(o)
   {
      sUI i;
      for(i=0; i<tkscript->uconst_table.max_entries; i++)
      {
         if(o[i])
         {
            switch(o[i]->type)
            {
            default:
               break;
            case 1:
               {
                  sSI iv=o[i]->value.int_val;
                  if(iv==_id)
                  {
                     if((psblk)||(o[i]->name.indexOf(ps, 0)==0))
                     {
                        return &o[i]->name;
                     }
                  }
               }
               break;
            case 2:
               {
                  sF32 fv=o[i]->value.float_val;
                  if(Dfltequal(fv, (sF32)_id))
                  {
                     if((psblk)||(o[i]->name.indexOf(ps, 0)==0))
                     {
                        return &o[i]->name;
                     }
                  }
               }
               break;
            case 4:
               {
                  YAC_String *s=o[i]->value.string_val;
                  sSI iv;
                  s->yacScanI(&iv);
                  if(iv==_id)
                  {
                     if((psblk)||(o[i]->name.indexOf(ps, 0)==0))
                     {
                        return &o[i]->name;
                     }
                  }
               }
               break;
            }
         }
      }
   }
   return 0;
}

void TKS_Language::stringToConstant(YAC_Object *_s, YAC_Value*_r) {
   if(YAC_BCHK(_s, YAC_CLID_STRING))
   {
      YAC_String *s=(YAC_String*)_s;
      TKS_CachedObject *co=tkscript->global_constant_namespace.findEntry(s);
      if(!co)
         co=tkscript->uconst_table.findEntry(s);
      if(co)
      {
         _r->initValue(co);
         return;
      }
   }
   _r->initInt(-1);
}

YAC_String *TKS_Language::scriptClassConstantToString (YAC_Object *_vo, YAC_Object *_sci, YAC_String *_strmatch) {

   if(YAC_BCHK(_vo, YAC_CLID_VALUE))
   {
      YAC_CAST_ARG(YAC_ValueObject, vo, _vo);

      if(YAC_Is_String(_strmatch))
      {
         if(YAC_BCHK(_sci, YAC_CLID_CLASS))
         {
            TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance *) _sci;

            TKS_ClassDecl *cd = sci->class_decl;

            if(NULL != cd->constants)
            {
               sUI i;
               for(i=0; i<cd->constants->max_entries; i++)
               {
                  TKS_CachedObject *co = cd->constants->objects[i];
                  if(NULL != co)
                  {
                     if(co->compareValue((YAC_Value*)vo))
                     {
                        if(co->name.startsWith(_strmatch))
                        {
                           return &co->name;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return NULL;
}

void TKS_Language::getClassName(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_VALID(_o))
   {
      YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
      rs->copy((char*)_o->yacClassName());
      _r->initString(rs, 1);
   }
   else
   {
      _r->initString(NULL, 0);
   }
}

void TKS_Language::getNamespaceName(YAC_Object *_o, YAC_Value *_r) {
   if(YAC_VALID(_o))
   {
      YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
      if(YAC_BCHK(_o, YAC_CLID_CLASS))
      {
         // Script class
         TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance*) _o;
         rs->visit((const char*)sci->class_decl->nspace->name.chars);
      }
      else
      {
         // C++ class
         rs->visit("yac");
      }
      _r->initString(rs, 1);
   }
   else
   {
      _r->initString(NULL, 0);
   }
}

sSI TKS_Language::getVersion(void) {
   return TKS_VERSION;
}

YAC_String *TKS_Language::getVersionString(void) {
   return &s_versionString;
}

void TKS_Language::setIntPropertyByName(YAC_Object *_o, YAC_String *_name, sSI _value) { // deprecated
   if(YAC_VALID(_o))
   {
      sUI ak=_o->yacMetaClassMemberGetAccessKeyByName((sChar*)_name->chars);
      YAC_Value val; val.initInt(_value);
      _o->yacMetaClassMemberSet(ak, &val);
   }
}

void TKS_Language::setFloatPropertyByName(YAC_Object *_o, YAC_String *_name, sF32 _value)  { // deprecated
   if(YAC_VALID(_o))
   {
      sUI ak=_o->yacMetaClassMemberGetAccessKeyByName((sChar*)_name->chars);
      YAC_Value val; val.initFloat(_value);
      _o->yacMetaClassMemberSet(ak, &val);
   }
}

void TKS_Language::setObjectPropertyByName(YAC_Object *_o, YAC_String *_name, YAC_Object *_value)  { // deprecated
   if(YAC_VALID(_o))
   {
      sUI ak=_o->yacMetaClassMemberGetAccessKeyByName((sChar*)_name->chars);
      YAC_Value val; val.initObject(_value, 0);
      _o->yacMetaClassMemberSet(ak, &val);
   }
}

sSI TKS_Language::getPropertyAccessKeyByName(YAC_Object *_o, YAC_Object *_name) { // deprecated
   if(YAC_VALID(_o) && YAC_BCHK(_name, YAC_CLID_STRING))
   {
      YAC_String *name=(YAC_String*)_name;
      return (sSI)_o->yacMetaClassMemberGetAccessKeyByName((sChar*)name->chars);
   }
   return -1;
}

sSI TKS_Language::getPropertyAccessKeyByIndex(YAC_Object *_o, sSI _index) { // deprecated
   return YAC_VALID(_o)?((sSI)_o->yacMetaClassMemberGetAccessKeyByIndex((sUI)_index)):-1;
}

void TKS_Language::getPropertyByAccessKey(YAC_Object *_o, sSI _ak, YAC_Value *_r)  {
   if(YAC_VALID(_o))
   {
      sUI ak = (sUI)_ak;
      _o->yacMetaClassMemberGet(ak, _r);
   }
}

void TKS_Language::getPropertyNameByAccessKey(YAC_Object *_o, sSI _ak, YAC_Value *_r) {
   if(YAC_VALID(_o))
   {
      YAC_String *name = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
      if(name)
      {
         name->visit(_o->yacMetaClassMemberGetName(_ak));
         YAC_RETS(name, 1);
         return;
      }
   }
   YAC_RETS(0,0);
}

void TKS_Language::getPropertyByName(YAC_Object *_o, YAC_String *_name, YAC_Value *_r)  {
   if(YAC_VALID(_o))
   {
      sUI ak=_o->yacMetaClassMemberGetAccessKeyByName((sChar*)_name->chars);
      _o->yacMetaClassMemberGet(ak, _r);
   }
}

sSI TKS_Language::getNumProperties(YAC_Object *_o)  {
   return YAC_VALID(_o) ? _o->yacMetaClassMemberGetNum():0;
}

void TKS_Language::getPropertyClassName(YAC_Object *_o, YAC_Value *_r)  {
   if(YAC_VALID(_o))
   {
      _r->initEmptyString();
      _r->value.string_val->visit(_o ? (char*)_o->yacMetaClassName() : (char*)"");
   }
}

void TKS_Language::newObjectByName(YAC_Object *_nspaceName, YAC_Object *_name, YAC_Value *_r)  {
   TKS_Namespace *nspace;
   if(YAC_BCHK(_nspaceName, YAC_CLID_STRING))
   {
      nspace = tkscript->findNamespace((YAC_String*)_nspaceName);
   }
   else
   {
      nspace = NULL;
   }
   if(YAC_BCHK(_name, YAC_CLID_STRING))
   {
      YAC_Object *o = tkscript->newObjectByName(nspace, (YAC_String*)_name);
      _r->initObject(o, (NULL != o));
   }
   else
   {
      _r->initObject(0, 0);
   }
}

void TKS_Language::newObjectArrayByName(YAC_Object *_nspaceName, YAC_Object *_name, YAC_Value *_r)  {
   TKS_Namespace *nspace;
   if(YAC_BCHK(_nspaceName, YAC_CLID_STRING))
   {
      nspace = tkscript->findNamespace((YAC_String*)_nspaceName);
   }
   else
   {
      nspace = NULL;
   }
   if(YAC_BCHK(_name, YAC_CLID_STRING))
   {
      YAC_Object *o = tkscript->newObjectArrayByName(nspace, (YAC_String*)_name);
      _r->initObject(o, (o!=NULL));
   }
   else
   {
      _r->initObject(0, 0);
   }
}

YAC_Object *TKS_Language::findScriptClass(YAC_Object *_nspaceName, YAC_Object *_name) {
   TKS_Namespace *nspace = NULL;
   if(YAC_Is_String(_nspaceName))
   {
      YAC_String *nspaceName = (YAC_String*) _nspaceName;
      nspace = tkscript->findNamespace(nspaceName);
      if(NULL == nspace)
      {
         Dprintf("[---] TKS::findScriptClass: namespace \"%s\" does not exist.\n",
            (char*)nspaceName->chars
            );
         return NULL;
      }
   }
   if(YAC_Is_String(_name))
   {
      YAC_String *name = (YAC_String*)_name;
      TKS_ClassDecl *cd = Dtkscompiler->findClassDeclNamespace(name, nspace);
      if(cd)
      {
         return cd->class_template;
      }
   }
   return yac_null;
}

void TKS_Language::findClassesByPrefix_YAC_RARG(YAC_Object *_nspaceName, YAC_Object *_prefix, YAC_Object *_return) {

   sBool bMatchAllNames;

   YAC_String *prefix;

   if(YAC_Is_String(_prefix))
   {
      bMatchAllNames = 0;
      prefix = (YAC_String*) _prefix;
   }
   else
   {
      bMatchAllNames = 1;
      prefix = NULL;
   }

   TKS_Namespace *nspace;
   if(YAC_Is_String(_nspaceName))
   {
      nspace = tkscript->findNamespace((YAC_String*)_nspaceName);
   }
   else
   {
      nspace = &Dtkscompiler->default_namespace; // default=find any (script or C++)
   }

   _return->yacArrayRealloc(0, 0, 0, 0);

   if(nspace != &Dtkscompiler->yac_namespace)
   {
      // Try to find script class
      sUI i;
      for(i = 1u; i < Dtkscompiler->class_decls.num_elements; i++)
      {
         TKS_ClassDecl *cd = Dtkscompiler->class_decls.elements[i];
         if(cd->isUsed())
         {
            if(cd->nspace == nspace)
            {
               if(bMatchAllNames || cd->name.startsWith(prefix))
               {
                  YAC_Value v;
                  v.initObject(cd->class_template, 0);
                  _return->yacArraySet(NULL, _return->yacArrayGetNumElements(), &v);
               }
            }
         }
      }
   }

   // Try to find c++ classes
   if((nspace == &Dtkscompiler->default_namespace) || (nspace == &Dtkscompiler->yac_namespace))
   {
      sUI i;
      for(i = 1u; i < YAC_MAX_CLASSES; i++)
      {
         TKS_ClassTemplate *clt = TKSCRIPT__ATOM_SPEEDPATH[i];
         if(NULL != clt)
         {
            if(bMatchAllNames || clt->class_name.startsWith(prefix))
            {
               YAC_Value v;
               v.initObject(clt->template_object, 0);
               _return->yacArraySet(NULL, _return->yacArrayGetNumElements(), &v);
            }
         }
      }
   }
}

void TKS_Language::findClassesByPrefix_YAC_RVAL(YAC_Object *_nspaceName, YAC_Object *_prefix, YAC_Value *_r) {
   YAC_PointerArray *r = (YAC_PointerArray*) YAC_NEW_CORE_POOLED(YAC_CLID_POINTERARRAY);
   findClassesByPrefix_YAC_RARG(_nspaceName, _prefix, r);
   _r->initObject(r, 1);
}

void TKS_Language::findClassesByBaseClass(YAC_Object *_nspaceName, YAC_Object *_baseClass, YAC_Value *_r) {
   YAC_PointerArray *r = (YAC_PointerArray*) YAC_NEW_CORE_POOLED(YAC_CLID_POINTERARRAY);
   _r->initObject(r, 1);
   if(YAC_Is_Class(_baseClass))
   {
      YAC_CAST_ARG(TKS_ScriptClassInstance, baseClass, _baseClass);
      TKS_ClassDecl *cdBase = baseClass->class_decl;

      TKS_Namespace *nspace;
      if(YAC_Is_String(_nspaceName))
      {
         nspace = tkscript->findNamespace((YAC_String*)_nspaceName);
         if(NULL == nspace)
            return; // namespace not found
      }
      else
      {
         nspace = &Dtkscompiler->default_namespace; // default=find any (script or C++)
      }

      if(nspace != &Dtkscompiler->yac_namespace)
      {
         // Find script classes
         for(sUI i = 1u; i < Dtkscompiler->class_decls.num_elements; i++)
         {
            TKS_ClassDecl *cd = Dtkscompiler->class_decls.elements[i];
            if(cd->isUsed())
            {
               if(cd->nspace == nspace)
               {
                  if(cd->hasBaseClass(cdBase))
                  {
                     r->_add(cd->class_template);
                  }
               }
            }
         }
      }
   }  // if baseClass is class
}

void TKS_Language::newObjectByID(sSI _class_ID, YAC_Value *_r)  {
   _r->initObject(YAC_NEW_CORE_POOLED(_class_ID), 1);
}

sSI TKS_Language::getClassID(YAC_Object *_o)  {
   return (sSI)(_o?(sSI)_o->class_ID:-1);
}

sSI TKS_Language::evalMethodByName(YAC_Object *_o, YAC_String *_name, YAC_Object *_arglist, YAC_Object *_retvalue) {
   // Evaluate C++ method ny name
   // Script class support is (currently) not available since the thread context is not available here.
   if(YAC_VALID(_o))
   {
      // Convert arguments (if any) to array
      if(YAC_CHK(_arglist, YAC_CLID_LISTNODE))
      {
         YAC_Value args[16]; // xxx numargs hardcoded
         sUI i = 0;

         ////if(YAC_CHK(_retvalue, YAC_CLID_VALUE))
         {
            YAC_ListNode *c = (YAC_ListNode*)_arglist;
            while(c)
            {
               if(i<16)
               {
                  args[i++].initValue(c);
               }
               else
               {
                  Dprintf("[---] Language::evalMethodByName(%s): maximum number of arguments exceeded (%d >= 16).\n",
                     _name->chars, i);
               }
               c = c->next;
            }
         }

         // Eval with arguments
         return tkscript->yacEvalMethodByName(_o, (const char*)_name->chars, args, i, (YAC_ValueObject*)_retvalue);
      }
      else
      {
         // Eval with no arguments
         return tkscript->yacEvalMethodByName(_o, (const char*)_name->chars, NULL, 0, (YAC_ValueObject*)_retvalue);
      }


////#ifdef TKS_DCON
////      tkscript->printf("[---] Language::evalMethodByName(%s): _arglist (0x%p) or _retvalue (0x%p) pointer is not valid.\n",
////         (char*)_name->chars, (void*) _arglist, (void*) _retvalue);
////#endif
   }
   else
   {
      Dprintf("[---] Language::evalMethodByName(%s): invalid <this> object..\n",
         _name->chars);
   }
   return 0;
}

sSI TKS_Language::classToHashTable(YAC_Object *_class, YAC_Object *_ht) {
   if(YAC_Is_HashTable(_ht))
   {
      YAC_HashTable *ht = (YAC_HashTable*) _ht;

      if(YAC_VALID(_class))
      {
         if(YAC_CHK(_class, YAC_CLID_CLASS))
         {
            TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance*)_class;
            return sci->writeToHashTable(ht);
         }
      }
      // Failed, free hashtable elements
      ht->varcache.freeCache();
   }
   return 0;
}

sSI TKS_Language::hashTableToClass(YAC_Object *_ht, YAC_Object *_o) {
   if(YAC_Is_HashTable(_ht))
   {
      if(YAC_CHK(_o, YAC_CLID_CLASS))
      {
         return ((TKS_ScriptClassInstance*)_o)->initFromHashTable((YAC_HashTable*)_ht);
      }
   }
   return 0;
}

void TKS_Language::getMethodArgumentNames(YAC_Object *_nspaceName, YAC_Object *_classname, YAC_Object *_methodname, YAC_Value *_r) {
   if(YAC_Is_String(_classname))
   {
      if(YAC_Is_String(_methodname))
      {
         YAC_String *classname  = (YAC_String*)_classname;
         YAC_String *methodname = (YAC_String*)_methodname;

         // Resolve namespace
         TKS_Namespace *nspace;
         if(YAC_Is_String(_nspaceName))
         {
            YAC_String *nspaceName = (YAC_String*) _nspaceName;
            nspace = tkscript->findNamespace(nspaceName);
            if(NULL == nspace)
            {
               Dprintf("[---] TKS::getMethodArgumentNames: namespace \"%s\" does not exist.\n",
                  (char*)nspaceName->chars
                  );
               _r->initNull();
               return;
            }
         }
         else
         {
            nspace = NULL;
         }

         TKS_ClassDecl *cd = Dtkscompiler->findClassDeclNamespace(classname, nspace);
         if(cd)
         {
            TKS_ClassDeclMethod *cdm = cd->findMethod(methodname);
            if(cdm)
            {
               PTN_Function *f = &cdm->function;
               YAC_StringArray *sa=(YAC_StringArray*)YAC_NEW_CORE_POOLED(YAC_CLID_STRINGARRAY);
               _r->initObject(sa, 1);
               TKS_FormalArgument *fa=f->formal_args;
               sUI numargs=0;
               while(fa)
               {
                  numargs++;
                  fa=fa->next;
               }
               if(numargs>0)
               {
                  if(sa->yacArrayAlloc(numargs, 0,0,0))
                  {
                     sa->num_elements=numargs;
                     fa=f->formal_args;
                     int i=0;
                     while(fa)
                     {
                        sa->elements[i++].visit((sChar*)fa->name.chars);
                        fa=fa->next;
                     }
                  }
               }
               return;

            }
         }
      }
   }
   _r->initNull();
}

void TKS_Language::sleep(sUI _ms) {
   tkscript->sleep(_ms);
}

void TKS_Language::fork(YAC_String *_chapterName, YAC_Value *_r) {
   (void)_chapterName;
   (void)_r;
   // - create SharedBuffer for IPC
   // - launch new tks process

   // xxx TODO: remove this call
}

void TKS_Language::_yield(void) {
#ifdef DX_PTHREADS
   sched_yield();
#else
#ifdef YAC_WIN32
#ifdef _MSC_VER
#ifdef DX_WIN32_YIELD
   //#if _MSC_VER >= 1300
   SwitchToThread();
#else
   //#warn "Thread::yield() not available in VC6"
   ::Sleep(1);
#endif // have vc7+ ?
#endif // _MSC_VER
#else
   usleep(1);
#endif // YAC_WIN32
#endif // DX_PTHREADS
}

void TKS_Language::getMethodNames(YAC_Object *_class, YAC_Value *_r) {
   if(YAC_VALID(_class))
   {
      if(YAC_CLID_CLASS == _class->class_ID)
      {
         // Script class
         TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance*) _class;

         YAC_StringArray *r = (YAC_StringArray*) YAC_NEW_CORE_POOLED(YAC_CLID_STRINGARRAY);
         r->alloc(16);

         // Iterate template_method_hashtable
         TKS_ObjectCache *oc = &sci->class_decl->template_method_hashtable;
         TKS_CachedObject **co = oc->objects;
         sUI i;
         for(i=0; i<oc->max_entries; i++)
         {
            if(NULL != co[i])
            {
               if(r->num_elements == r->max_elements)
               {
                  r->realloc(r->max_elements + 16);
               }
               YAC_String *n = (YAC_String*) r->getNextFree();
               n->yacCopy(&co[i]->name);
            }
         }

         _r->initObject(r, 1);
      }
      else
      {
         // C++ class
         TKS_ClassTemplate *clt = TKSCRIPT__ATOM_SPEEDPATH[_class->class_ID];
         if(NULL != clt)
         {
            sUI i;
            sUI numCommands = clt->commandsy.y_num_commands;
            YAC_StringArray *r = (YAC_StringArray*) YAC_NEW_CORE_POOLED(YAC_CLID_STRINGARRAY);
            r->alloc(numCommands);
            for(i=0; i<numCommands; i++)
            {
               r->elements[i].copy(clt->commandsy.y_commands[i].y_name);
            }
            r->num_elements = i;
            _r->initObject(r, 1);
         }
         else
         {
            _r->initNull();
         }
      }
   }
   else
   {
      // xxx TODO raise invalid pointer exception..
      _r->initNull();
   }
}

sSI TKS_Language::_isBigEndian(void) {
#ifdef YAC_LITTLE_ENDIAN
   return 0;
#else
   return 1;
#endif // YAC_LITTLE_ENDIAN
}

sBool TKS_Language::is64Bit(void) {
   return (8 == sizeof(void*));
}

void TKS_Language::_reinstallDebugHandler(void) {
#ifdef YAC_WIN32
   tks_debug_init(0, NULL);
#endif // YAC_WIN32
}

void TKS_Language::getTksExePath(YAC_Value *_r) {
   _r->initNewString(&tkscript->configuration.tks_exe_path);
}

void TKS_Language::getTksExeDir(YAC_Value *_r) {
   _r->initNewString(&tkscript->configuration.tks_exe_dir);
}

void TKS_Language::getTksPluginPath(YAC_Value *_r) {
   _r->initNewString(&tkscript->configuration.plugin_path);
}

void TKS_Language::getTksModulePath(YAC_Value *_r) {
   _r->initNewString(&tkscript->configuration.module_path);
}

void TKS_Language::getTksLibraryPath(YAC_Value *_r) {
   _r->initNewString(&tkscript->configuration.library_path);
}

void TKS_Language::getTksApplicationPath(YAC_Value *_r) {
   _r->initNewString(&tkscript->configuration.application_path);
}

void TKS_Language::getDllPrefix(YAC_Value *_r) {
   _r->initEmptyString();
#ifdef YAC_LINUX
   _r->value.string_val->copy("lib");
#endif // YAC_LINUX
}

void TKS_Language::getDllSuffix(YAC_Value *_r) {
   _r->initEmptyString();
#ifdef YAC_WIN32
   _r->value.string_val->copy(".dll");
#elif defined(YAC_LINUX)
   _r->value.string_val->copy(".so");
#elif defined(YAC_MACOS)
   _r->value.string_val->copy(".dylib");
#endif
}

void TKS_Language::newThreadFromCurrent(YAC_Value *_r) {
   TKS_Thread *th = (TKS_Thread*)tkscript->yacThreadNewFromCurrent();
   _r->initObject(th, YAC_TRUE);
}

void TKS_Language::forceUnlockNamedMutex(YAC_String *_name) {
   if(YAC_Is_String(_name))
   {
      TKS_NamedMutex *mtx = tkscript->findNamedMutex(_name);
      if(NULL != mtx)
      {
         tkscript->printf("[dbg] TKS::forceUnlockNamedMutex(name=\"%s\") BEGIN\n", (const char*)_name->chars);
         mtx->forceUnlock();
         tkscript->printf("[dbg] TKS::forceUnlockNamedMutex(name=\"%s\") END\n", (const char*)_name->chars);
      }
   }
}

void TKS_Language::unlockNamedMutex(YAC_String *_name) {
   if(YAC_Is_String(_name))
   {
      TKS_NamedMutex *mtx = tkscript->findNamedMutex(_name);
      if(NULL != mtx)
      {
         mtx->unlock();
      }
   }
}

void TKS_Language::lockNamedMutex(YAC_String *_name) {
   if(YAC_Is_String(_name))
   {
      TKS_NamedMutex *mtx = tkscript->findNamedMutex(_name);
      if(NULL != mtx)
      {
         mtx->lock();
      }
   }
}

void TKS_Language::getHomeDir(YAC_Value *_r) {
   _r->initNewString(&tkscript->configuration.home_dir);
}

void TKS_Language::getCurrentDir(YAC_Value *_r) {
   _r->initEmptyString();
   if(_r->value.string_val->alloc(1024))
   {
#ifdef YAC_WIN32
      ::GetCurrentDirectory(1024, (LPSTR)_r->value.string_val->chars);
#else
      char *r = ::getcwd((char*)_r->value.string_val->chars, 1024);
      (void)r;
#endif // YAC_WIN32
      _r->value.string_val->fixLength();
   }
}

void TKS_Language::getExtraFiles(YAC_Value *_r) {
   tks_get_extrafile_stringarray(_r);
}

sBool TKS_Language::loadLocalTSL(YAC_String *_libName) {
   sBool r = YAC_FALSE;
   // yac_host->printf("xxx loadLocalTSL 1\n");
   if(YAC_Is_String(_libName))
   {
      // yac_host->printf("xxx loadLocalTSL 1b\n");
      r = tkscript->loadLocalTSL(_libName);
   }
   // yac_host->printf("xxx loadLocalTSL LEAVE\n");
   return r;
}

void TKS_Language::evalScript(YAC_String *_src, YAC_Value *_r) {
   TKS_ScriptModule *script = (TKS_ScriptModule*) YAC_NEW_CORE_POOLED(TKS_CLID_SCRIPT);
   script->installPrintHook();
   if(script->load(_src))
      script->eval();
   script->getPrintHookOutputAndClear(_r);
   // tkscript->printf("xxx evalScript: r.chars=\"%s\"\n", (const char*)_r->value.string_val->chars);
   YAC_DELETE(script);
}
