/// TKS_ScriptClassInstance.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "YAC_HashTable.h"
#include "tks_inc_class.h"
#include "tks_inc_compiler.h"

#include "PTN_DoubleArgExpr.h"

#include "TKS_Mutex.h"


#ifdef TKS_SCI_OBJECTCOUNTER
sSI TKS_ScriptClassInstance::object_counter = 0;
#endif // TKS_SCI_OBJECTCOUNTER


TKS_ScriptClassInstance::TKS_ScriptClassInstance(void) {
#ifdef TKS_SCI_OBJECTCOUNTER
   object_counter++;
#endif // TKS_SCI_OBJECTCOUNTER

   class_ID        = YAC_CLID_CLASS;
   class_decl      = NULL;
   member_data     = NULL;
   custom_data.any = NULL;
   delegates       = NULL;
#ifdef TKS_MT
   mtx_this        = NULL;
#endif //TKS_MT
}

TKS_ScriptClassInstance::~TKS_ScriptClassInstance() {
   clean();

#ifdef TKS_SCI_OBJECTCOUNTER
   object_counter--;
#endif // TKS_SCI_OBJECTCOUNTER
}

void TKS_ScriptClassInstance::clean(void) {
   if(class_decl)
   {
      freeMemberData();
      class_decl = NULL;
   }
   if(tkscript->b_initclasses)
   {
      Dtkscompiler->removeClassInitializer(this);
   }
   if(delegates)
   {
      delete delegates;
      delegates = NULL;
   }
#ifdef TKS_MT
   if(mtx_this != NULL)
   {
      delete mtx_this;
      mtx_this = NULL;
   }
#endif //TKS_MT
}

sBool YAC_VCALL TKS_ScriptClassInstance::yacEquals(YAC_Object *_o) {
   if(YAC_VALID(_o))
   {
      if(YAC_CHK(_o, YAC_CLID_CLASS))
      {
         TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance *) _o;
         if(sci->class_decl->class_id == class_decl->class_id)
         {
            TKS_ClassDeclMember**cdm=class_decl->members.elements;
            sUI i;
            TKS_CachedObject *cc_co;
            yacmem *madr, *omadr;
            for(i = 0u; i < class_decl->members.num_elements; i++)
            {
               /// ---- now store value ----
               cc_co=getMember(&cdm[i]->name);
               madr  = getMemberAdrByID(cc_co->value.int_val);
               omadr = sci->getMemberAdrByID(cc_co->value.int_val);
               switch(cdm[i]->rtti)
               {
               case 1:
                  // ---- compare integer ----
                  if(madr[0].si != omadr[0].si)
                  {
                     return YAC_FALSE;
                  }
                  break;
               case 2:
                  // ---- compare float ----
                  if( (madr[0].f32 >= (omadr[0].f32-YAC_FLOAT_DEVIATION)) && (madr[0].f32 <= (omadr[0].f32+YAC_FLOAT_DEVIATION)) )
                  {
                     return YAC_FALSE;
                  }
                  break;
               default:

                  // ---- compare object ----
                  if(YAC_VALID(madr[0].o))
                  {
                     if(YAC_VALID(omadr[0].o))
                     {
                        if(! madr[0].o->yacEquals(omadr[0].o))
                        {
                           return YAC_FALSE;
                        }
                     }
                     else
                     {
                        return YAC_FALSE;
                     }
                  }
                  else
                  {
                     if(YAC_VALID(omadr[0].o))
                     {
                        return YAC_FALSE;
                     }
                  }
                  break;
               }
            } // for
            return YAC_TRUE;
         }
      }
   }
   return YAC_FALSE;
}

void YAC_VCALL TKS_ScriptClassInstance::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();

   sUI te = 0u;
   TKS_ClassDeclMember **acdms = class_decl->all_members.elements;
   sUI i;
   for(i = 0u; i < class_decl->all_members.max_elements; i++)
   {
      if( (*acdms++)->tagid )
         te++;
   }

   // ---- store number of actually used entries ----
   YAC_SERIALIZE_I32(te);

   // ---- serialize members ----
   yacmem *md = member_data;
   acdms = class_decl->all_members.elements;
   for(i=0; i<class_decl->all_members.max_elements; i++)
   {
      TKS_ClassDeclMember *cdm = *acdms++;
      if(cdm->tagid)
      {
         /// ---- store member name ----
         cdm->name.yacSerialize(_ofs, 0);
         /// ---- store type ----
         YAC_SERIALIZE_I8((sU8)cdm->rtti);
         /// ---- now store value ----
         switch(cdm->rtti)
         {
         case 1:
            // ---- serialize integer ----
            YAC_SERIALIZE_I32( md[0].s32 );
            md++;
            break;
         case 2:
            // ---- serialize float ----
            YAC_SERIALIZE_F32( md[0].f32 );
            md++;
            break;
         default:
            {
               // ---- serialize object, chk deleteme(v) flag ----
               if( md[0].si )
               {
                  // ---- deletable object ----
                  YAC_SERIALIZE_I8(1);
                  YAC_Object *o = md[1].o;
                  o->yacSerialize(_ofs, 1);
               }
               else
               {
                  // ---- non-deletable objects never get serialized ----
                  YAC_SERIALIZE_I8(0);
               }
               md += 2;
            }
            break;
         }
      } // if tagged
      else
      {
         if(cdm->rtti>=YAC_TYPE_OBJECT)
            md+=2;
         else
            md++;
      }
   } // for



   // old code ~~~~~~~~~~~~~~~~~~~~~+~~~~~~~~~~~~~~~~~~~~~+~~~~~~~~~~~~~~~~~~~~~+

   /*TKS_ClassDecl *cd=class_decl;
   TKS_ClassDeclMember**cdm=cd->members.elements;

   sUI te=0; // ---- count number of tagged entries in this class ----
   sUI i;
   for(i=0; i<cd->members.num_elements; i++)
   {
      if(cdm[i]->tagid)
      {
         te++;
      }
   }

   sUI j;    // ---- count number of tagged entries in super classes ----
   for(j=0; j<class_decl->class_dependencies.num_elements; j++)
   {
      TKS_ClassDecl *bcd=class_decl->class_dependencies.elements[j];
      TKS_ClassDeclMember**c=bcd->members.elements;
      sUI ne=bcd->members.num_elements;
      for(i=0; i<ne; i++)
      {
         if((*c)->tagid)
         {
            te++;
         }
         c++;
      }
   }

   // ---- store number of actually used entries ----
   YAC_SERIALIZE_I32(te);

   // ---- serialize tagged members ----
   // ---- in order to be version safe we also store member names ----
   if(te)
   {
      serializeClassMembers(_ofs, cd);
      // ---- serialize super classes ----
      for(j=0; j<class_decl->class_dependencies.num_elements; j++)
      {
         serializeClassMembers(_ofs, class_decl->class_dependencies.elements[j]);
      }
   } // if any tagged
   */
}

/*void TKS_ScriptClassInstance::serializeClassMembers(YAC_Object *_ofs, TKS_ClassDecl *cd) {
   TKS_ClassDeclMember**cdm=cd->members.elements;
   sUI i;
   TKS_CachedObject *cc_co;
   yacmem *madr;
   for(i=0; i<cd->members.num_elements; i++)
   {
      if(cdm[i]->tagid)
      {
         /// ---- store member name ----
         cdm[i]->name.yacSerialize(_ofs, 0);
         /// ---- store type ----
         YAC_SERIALIZE_I8(cdm[i]->rtti);
         /// ---- now store value ----
         cc_co = getMember(&cdm[i]->name);
         madr = getMemberAdrByID(cc_co->value.int_val);
         switch(cdm[i]->rtti)
         {
         case 1:
            // ---- serialize integer ----
            YAC_SERIALIZE_I32(madr[0].si); break;
         case 2:
            // ---- serialize float ----
            YAC_SERIALIZE_F32(madr[0].f4); break;
         default:
            {
               // ---- serialize object, chk deleteme(v) flag ----
               if(madr[-1].si)
               {
                  // ---- deletable object ----
                  YAC_SERIALIZE_I8(1);
                  YAC_Object *o = madr[0].o;
                  o->yacSerialize(_ofs, 1);
               }
               else
               {
                  // ---- read-only pointer ----
                  YAC_SERIALIZE_I8(0);
               }
            }
            break;
         }
      } // if tagged
   } // for
}
*/

sUI YAC_VCALL TKS_ScriptClassInstance::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();

   sUI te=YAC_DESERIALIZE_I32();
   if(te>(TKS_MAX_CLASSMEMBERS*TKS_MAX_CLASSANCESTORS))
   {
      Dprintf("[---] CompactClass::deserialize: insane number of members (%i>%i)\n",
              te, (TKS_MAX_CLASSMEMBERS*TKS_MAX_CLASSANCESTORS));
      return 0u;
   }
   if(te)
   {
      sUI i;
      YAC_String mname;
      for(i=0; i<te; i++)
      {
         // ---- read member name ----
         mname.yacDeserialize(_ifs, 0);
         TKS_ClassDeclMember *cdm=class_decl->findMemberSuper(&mname, 0);
         if(cdm)
         {
            /// ---- member exists, now deserialize value ----
            sU8 rtti=YAC_DESERIALIZE_I8();
            if(rtti!=cdm->rtti)
            {
               Dprintf("[~~~] CompactClass::deserialize: warning: member \"%s\" rtti (%i) != stream member rtti (%i). skipping..\n",
                       (char*)mname.chars, cdm->rtti, rtti
                       );
               // ---- dummy read value ----
               if(rtti>2)
               {
                  // ---- dummy read object value ----
                  sU8 bused=YAC_DESERIALIZE_I8();
                  if(bused)
                  {
                     if(member_data[cdm->offset].si) // is deletable or just reference??
                     {
                        YAC_Object *o = tkscript->deserializeNewObject(_ifs, 0);
                        if(!o)
                        {
                           // ---- failed to allocate object
                           return 0u;
                        }
                        YAC_DELETE(o);
                     }
                  }
               }
               else
               {
                  // ---- dummy read int32/float32 value ----
                  /*sUI du32=*/YAC_DESERIALIZE_I32();
               }
            }
            else
            {
               // ---- rtti seems ok, now deserialize value and store class member ----
               TKS_CachedObject *cc_co=class_decl->getMember(&mname);
               YAC_Value v;

               switch(rtti)
               {
               default:
                  {
                     sU8 bused=YAC_DESERIALIZE_I8();
                     if(bused)
                     {
                        YAC_Object *o_template=0;
                        if(member_data[cdm->offset].si)
                        {
                           if(member_data[cdm->offset+1].o)
                           {
                              o_template = member_data[cdm->offset+1].o;
                           }
                        }
                        YAC_Object *o = tkscript->deserializeNewObject(_ifs, o_template);
                        if(!o)
                           return 0; // ---- failed to deserialize object ----
                        v.initObject(o, 1);
                     }
                     else
                        v.initObject(0, 0); // ---- set "0" pointer ----
                     setClassMemberPointerValueByID(cc_co->value.int_val, &v);
                  }
                  break;
               case 1:
                  v.initInt(YAC_DESERIALIZE_I32());
                  setClassMemberValueByID(cc_co->value.int_val, &v, YAC_OP_ASSIGN);
                  break;
               case 2:
                  v.initFloat(YAC_DESERIALIZE_F32());
                  setClassMemberValueByID(cc_co->value.int_val, &v, YAC_OP_ASSIGN);
                  break;
               }
            }
         }
         else
         {
            // ---- member not found in class ----
            Dprintf("[~~~] CompactClass::deserialize: warning: member \"%s\" not found in class \"%s::%s\" (or its base classes). skipping..\n",
                    (char*)mname.chars,
                    (char*)class_decl->nspace->name.chars,
                    (char*)class_decl->name.chars
                    );
            // ---- dummy read rtti, value ----
            sU8 du8=YAC_DESERIALIZE_I8();
            if(du8>2)
            {
               // ---- dummy read object value ----
               sU8 bused=YAC_DESERIALIZE_I8();
               if(bused)
               {
                  YAC_Object *o=tkscript->deserializeNewObject(_ifs, 0);
                  if(!o)
                  {
                     return 0u;
                  }
                  YAC_DELETE(o);
               }
            }
            else
            {
               // ---- dummy read int32/float32 value ----
               /*sUI du32=*/YAC_DESERIALIZE_I32();
            }
         }
      }
   }
   return 1u;
}

void YAC_VCALL TKS_ScriptClassInstance::yacSerializeClassName(YAC_Object *_ofs) {
   /// ---- special object type TKS user class ----
   YAC_String tcln;
   tcln.copy("@");
   if(class_decl->nspace != &Dtkscompiler->default_namespace)
   {
      tcln.append(&class_decl->nspace->name);
      tcln.append(":");
   }
   tcln.append(&class_decl->name);
   tcln.yacSerialize(_ofs, 0); // e.g. @MyClass
}

sChar *YAC_VCALL TKS_ScriptClassInstance::yacMetaClassName(void) {
   return (sChar*)class_decl->name.chars;
}

sUI YAC_VCALL TKS_ScriptClassInstance::yacMetaClassMemberGetNum(void) {
   return class_decl->template_member_hashtable.num_entries;
}

sUI YAC_VCALL TKS_ScriptClassInstance::yacMetaClassMemberGetAccessKeyByIndex(sUI _idx) {
   return (sUI)class_decl->template_member_hashtable.findIntValueByIndex(_idx);
}

sUI YAC_VCALL TKS_ScriptClassInstance::yacMetaClassMemberGetAccessKeyByName(const sChar *_prop) {
   YAC_String pname; pname.visit(_prop);
   return (sUI)class_decl->template_member_hashtable.findIntValueByName(&pname);
}

sUI YAC_VCALL TKS_ScriptClassInstance::yacMetaClassMemberGetType(sUI _ak) {
   if(~0u != _ak)
   {
      sU16 cortti = (sU16)((_ak >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK);

      switch(cortti & TKS_CLASSDECL_RTTIMASK)
      {
      default:                              return 0;
      case TKS_CLASSDECL_RTTI_INT:	        return 1;
      case TKS_CLASSDECL_RTTI_FLOAT:        return 2;
      case TKS_CLASSDECL_RTTI_STRING:
      case TKS_CLASSDECL_RTTI_OBJECT:
      case TKS_CLASSDECL_RTTI_CLASS:
      case TKS_CLASSDECL_RTTI_POINTER:
      case TKS_CLASSDECL_RTTI_INTARRAY:
      case TKS_CLASSDECL_RTTI_FLOATARRAY:
      case TKS_CLASSDECL_RTTI_STRINGARRAY:
      case TKS_CLASSDECL_RTTI_OBJECTARRAY:
      case TKS_CLASSDECL_RTTI_POINTERARRAY:
      case TKS_CLASSDECL_RTTI_CLASSARRAY:   return 3;
      }
   }
   return 0;
}

void YAC_VCALL TKS_ScriptClassInstance::yacMetaClassMemberGet(sUI _ak, YAC_Value *_ret) {
   if(~0u != _ak)
   {
      sU16 cortti = (sU16)((_ak >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK);
      sU16 cooff = (sU16)((_ak & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_ak >> TKS_CLASSDECL_IDSHIFT) & 0xFFFF]);
      switch(cortti & TKS_CLASSDECL_RTTIMASK)
      {
      default:
         _ret->initVoid();
         break;
      case TKS_CLASSDECL_RTTI_INT:
         _ret->initInt(member_data[cooff].s32);
         break;
      case TKS_CLASSDECL_RTTI_FLOAT:
         _ret->initFloat(member_data[cooff].f32);
         break;
      case TKS_CLASSDECL_RTTI_STRING:
         _ret->initString(member_data[cooff + 1].s, 0);
         break;
      case TKS_CLASSDECL_RTTI_OBJECT:
      case TKS_CLASSDECL_RTTI_CLASS:
      case TKS_CLASSDECL_RTTI_POINTER:
      case TKS_CLASSDECL_RTTI_INTARRAY:
      case TKS_CLASSDECL_RTTI_FLOATARRAY:
      case TKS_CLASSDECL_RTTI_STRINGARRAY:
      case TKS_CLASSDECL_RTTI_OBJECTARRAY:
      case TKS_CLASSDECL_RTTI_POINTERARRAY:
      case TKS_CLASSDECL_RTTI_CLASSARRAY:
         _ret->initObject(member_data[cooff + 1].o, 0);
         break;
      }
   }
   else
   {
      _ret->initVoid();
   }
}

sChar *YAC_VCALL TKS_ScriptClassInstance::yacMetaClassMemberGetName(sUI _ak) {
   if(~0u != _ak)
   {
      if(class_decl->template_member_hashtable.num_entries)
      {
         sUI me = class_decl->template_member_hashtable.max_entries;
         TKS_CachedObject** cl = class_decl->template_member_hashtable.objects;
         TKS_CachedObject* co;
         sUI i;
         for(i = 0; i < me; i++)
         {
            co = *cl++;
            if(co)
               if(((sUI)co->value.int_val) == _ak)
                  return (sChar*)co->name.chars;
         }
      }
   }
   return NULL;
}

void YAC_VCALL TKS_ScriptClassInstance::yacMetaClassMemberSet(sUI _ak, YAC_Value *_v) {
   if(~0u != _ak)
   {
      if(_v->type >= YAC_TYPE_OBJECT)
      {
         setClassMemberPointerValueByID(_ak, _v);
      }
      else
      {
         setClassMemberValueByID(_ak, _v, YAC_OP_ASSIGN);
      }
   }
}

sSI YAC_VCALL TKS_ScriptClassInstance::yacMetaClassInstanceOf(YAC_Object *_o) {
   if(YAC_BCHK(_o, YAC_CLID_CLASS))
   {
      TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance *) _o;

      return class_decl->ancestor_table[sci->class_decl->class_id];
   }
   return 0;
}

void TKS_ScriptClassInstance::freeMemberData(void) {
   // xxx TKS_MT: callDestructors removed (moved to yacFinalizeObject()..)
   if(member_data)
   {
      sUI i;
      for(i=0; i<class_decl->object_indices.num_elements; i++)
      {
         sU16 off = class_decl->object_indices.elements[i];
         if(member_data[off].si) // delete flag
         {
            YAC_Object *t = member_data[off+1].o;
            YAC_DELETE(t);
         }
      }
      delete [] member_data;
      member_data = NULL;
   }
}

void YAC_VCALL TKS_ScriptClassInstance::yacFinalizeObject(void *_context) {
   if(_context != NULL)
   {
      if((!Dtkscompiler->parse_error)&&(!Dtkscompiler->no_destructors))
      {
         PTN_Env env; env.continue_flag =1; env.context = (TKS_Context*)_context ;
         callDestructors( &env );
      }
   }
}

void TKS_ScriptClassInstance::lazyInitMutexAndMemberData(void *_context, TKS_ScriptClassInstance *o) {

   // Lazy create mutex
#ifdef TKS_MT
   if(class_decl->b_this_mutex)
   {
      if(NULL == mtx_this)
      {
         mtx_this = new TKS_Mutex();
      }
   }
#endif //TKS_MT


   // Lazy-init member data
   if(NULL == member_data)
   {
      if(class_decl->member_data_size)
      {
         member_data = new yacmem[class_decl->member_data_size];
         memset((void*)member_data, 0, sizeof(void*) * class_decl->member_data_size);

         sU16 i;
         // Instantiate object members according to template object <o>
         for(i=0; i<class_decl->object_indices.num_elements; i++)
         {
            sU16 off=class_decl->object_indices.elements[i];
            if(o->member_data[off].si) // is deletable (i.e. not just a pointer reference)
            {
               YAC_Object *t = o->member_data[off+1].o;
               YAC_Object *n;
               if(t)
               {
                  n = YAC_CLONE_POOLED(_context, t);
                  n->yacArrayCopySize(t);
                  member_data[off].si  = 1; // delpointer flag
                  member_data[off+1].o = n;
               }
               else
               {
                  // ---- for classes that contain instances of themselves ----
                  member_data[off].si  = 0; // delpointer flag
                  member_data[off+1].o = NULL;
               }
            }
         }

      } // if member_size
      else
      {
         member_data = NULL;
      }
   }
}

void YAC_VCALL TKS_ScriptClassInstance::yacOperatorInit(void *_context/*MUST be nonzero*/, YAC_Object *_arg) {
   if(_arg && YAC_CLID_CLASS == _arg->class_ID)
   {
      TKS_ScriptClassInstance *o = (TKS_ScriptClassInstance *)_arg;  // template object

      class_decl = o->class_decl;

      if(class_decl)
      {
         // (note) if called from static class member initializer, member data may still be NULL
         lazyInitMutexAndMemberData(_context, o);

         // yac_host->printf("xxx SCI::yacOperatorInit: b_runtime=%d b_initializing=%d cd->name=\"%s\" context=%p\n", tkscript->b_runtime, tkscript->b_initializing, class_decl->name.chars, _context);

         if(tkscript->b_runtime)
         {
            if(NULL != _context)
            {
               PTN_Env env; env.continue_flag=1; env.context = (TKS_Context*)_context;
               // yac_host->printf("xxx SCI::yacOperatorInit: 0\n");
               callMemberInitializersPushSelf(&env);

               // yac_host->printf("xxx SCI::yacOperatorInit: 1\n");

               if(!tkscript->b_initializing)
               {
                  callConstructors(&env);
               }

               // yac_host->printf("xxx SCI::yacOperatorInit: 2\n");
            }
            else
            {
#ifdef TKS_DCON
               if(!tkscript->b_initializing)
               {
                  if(class_decl->constructor_list.num_elements)
                  {
                     if(tkscript->yacGetDebugLevel() > 0u)
                     {
                        Dprintf("[~~~] TKS_ScriptClassInstance::yacOperatorInit: _context==NULL (skipping c'tors).\n");
                     }
                  }
               }
#endif
            }
         } // if tkscript->run_time
         else
         {
            TKS_Compiler *c = (TKS_Compiler*)tkscript->compiler;
            c->addClassInitializer(this);
            /////////tkscript->printf("xxx add class ini classname=\"%s\" iniNum=%i\n", class_decl->name.chars, c->class_initializers.num_elements);
         }
         // xxx else instance without template
      } // if classdecl
   }
   else
   {
      Dprintf("[---] TKS_ScriptClassInstance::yacOperatorInit: invalid args.\n");
   }
}

void TKS_ScriptClassInstance::callMemberInitializers(PTN_Env *_env) {
   // Called by callMemberInitializersPushSelf()
   // ---- call initializer expressions
   // sBool bDebug = (class_decl->name.compare("Label"));
   // if(bDebug) yac_host->printf("xxx ScriptClassInstance::callMemberInitializers: --------- cd=\"%s\" addr=%p effinitmember=%p\n", class_decl->name.chars, class_decl, class_decl->effective_init_members);
   if(class_decl->effective_init_members)
   {
      StaticList::Node *c = class_decl->effective_init_members;
      while(c)
      {
         TKS_ClassInitializationMember *im = (TKS_ClassInitializationMember *)c->data;
         // if(bDebug) yac_host->printf("xxx ScriptClassInstance::callMemberInitializers: c=%p cd=\"%s\" im=%p\n", c, class_decl->name.chars, im);
         if(im)
         {
            // if(bDebug) yac_host->printf("xxx ScriptClassInstance::callMemberInitializers: im->member=%p\n", im->member);
            // if(bDebug) yac_host->printf("xxx ScriptClassInstance::callMemberInitializers: im->member->offset=%u\n", im->member->offset);
            // if(bDebug) yac_host->printf("xxx ScriptClassInstance::callMemberInitializers: im->member->name.len=%u\n", im->member->name.length);
            // if(bDebug) yac_host->printf("xxx ScriptClassInstance::callMemberInitializers: im->member->name.chars=\"%s\"\n", im->member->name.chars);
            // if(bDebug) yac_host->printf("xxx ScriptClassInstance::callMemberInitializers: im->member->init_expr=%p\n", im->member->init_expr);
            // if(bDebug) yac_host->printf("xxx ScriptClassInstance::callMemberInitializers: init_expr->getID()=%u\n", im->member->init_expr->getID());
            YAC_Value r; im->member->init_expr->eval(_env, &r);
            // if(bDebug) yac_host->printf("xxx ScriptClassInstance::callMemberInitializers: init_expr OK\n");
            YAC_Value ar;
            sUI rtti = im->member->rtti;
            if(rtti > YAC_TYPE_STRING)
               rtti = YAC_TYPE_OBJECT;
            switch(im->member->rtti)
            {
               case YAC_TYPE_VOID:
                  r.unset();
                  break;
               case YAC_TYPE_INT:
                  member_data[im->shifted_offset].s32 = r.getIntValue();
                  r.unset();
                  break;
               case YAC_TYPE_FLOAT:
                  member_data[im->shifted_offset].f32 = r.getFloatValue();
                  break;
               default:
                  if( (!im->member->deref_init_expr) && member_data[im->shifted_offset+1].si ) // object ptr set??
                  {
                     switch(r.type)
                     {
                        case YAC_TYPE_VOID:
                           break;
                        case YAC_TYPE_INT:
                           member_data[im->shifted_offset+1].o->yacOperatorI(YAC_OP_ASSIGN, r.value.int_val, &ar);
                           ar.unset();
                           break;
                        case YAC_TYPE_FLOAT:
                           member_data[im->shifted_offset+1].o->yacOperatorF32(YAC_OP_ASSIGN, r.value.float_val, &ar);
                           ar.unset();
                           break;
                        default:
                           member_data[im->shifted_offset+1].o->yacOperatorAssign(r.value.object_val);
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
                        if(member_data[im->shifted_offset].b)
                        {
                           YAC_DELETE(member_data[im->shifted_offset+1].o);
                        }
                        member_data[im->shifted_offset].b = r.deleteme;
                        r.deleteme = YAC_FALSE;
                        member_data[im->shifted_offset+1].o = r.value.object_val;
                     }
                     else if(im->member->deref_init_expr)
                     {
                        // ---- unset class member object pointer ----
                        if(member_data[im->shifted_offset].s32)
                        {
                           YAC_DELETE(member_data[im->shifted_offset+1].o);
                        }
                        member_data[im->shifted_offset].b  = YAC_FALSE;
                        member_data[im->shifted_offset+1].o = NULL;
                     }
                  }
                  break;
            }
         }
         c = c->next;
      }
   }
}

void YAC_VCALL TKS_ScriptClassInstance::yacOperatorAssign(YAC_Object *_arg) {

   /*if(!class_decl->b_static_members_initialized)
   {
       class_decl->callRemoveAncestorStaticInitializers( _env );
   }*/

   // (note) when called from a static class member initializer
   ////lazyInitMutexAndMemberData(class_decl->class_template);


   // ---- try to call script class method "yacOperator"
   /////callback.caps.bits.have_yacOperatorAssign=0; // xxx TKS_MT: use *real* thread context (avoid globl lock!)

   // First perform normal assign
   if(YAC_CHK(_arg, YAC_CLID_CLASS))
   {
      if(_arg&&class_decl)
      {
         TKS_ScriptClassInstance *o=(TKS_ScriptClassInstance*)_arg;
         if(o->class_decl)
         {
            yacmem *omd;
            if(class_decl->class_id==o->class_decl->class_id) // same class type?
            {
               omd = o->member_data;
            }
            else if(o->class_decl->ancestor_table[class_decl->class_id]) // is this a base class of o ?
            {
               omd = o->member_data + o->class_decl->class_relocation_table[class_decl->class_id];
            }
            else
            {
               Dprintf("[---] ScriptClass::yacOperatorAssign: incompatible classes (unable to assign \"%s::%s\" to \"%s::%s\").\n",
                  (char*)o->class_decl->nspace->name.chars,
                  (char*)o->class_decl->name.chars,
                  (char*)class_decl->nspace->name.chars,
                  (char*)class_decl->name.chars
                  );
               tkscript->printAllFunctionCallStacks();
               return;
            }

            // Copy members 1:1
            TKS_ClassDeclMember **acdms = class_decl->all_members.elements;
            sUI i;
            yacmem *md  = member_data;
            for(i=0; i<class_decl->all_members.max_elements; i++)
            {
               if((*acdms++)->rtti >= YAC_TYPE_OBJECT)
               {
                  if(md[0].b) // Is deletable?
                  {
                     YAC_DELETE(md[1].o);
                  }
                  if(YAC_VALID(omd[1].o)) // pointer in use?
                  {
                     if(omd[0].b) // Is deletable?
                     {
                        YAC_Object *t = YAC_CLONE_POOLED(NULL, omd[1].o); // xxx TKS_MT: use *real* thread context (no c'tors otherwise!)
                        t->yacOperatorAssign(omd[1].o);
                        md[0].b = 1; // Set deleteme flag
                        md[1].o = t;
                     }
                     else
                     {
                        // Just copy reference
                        md[0].b = YAC_FALSE;
                        md[1].o = omd[1].o;
                     }
                  }
                  else
                  {
                     md[0].b = YAC_FALSE;
                     md[1].o = NULL; // xxx safe to use yac_null ?
                  }
                  md  += 2;
                  omd += 2;
               }
               else
               {
                  // Copy scalar value (int/float)
                  (*md++).s32 = (*omd++).s32;
               }

            } // copy members

         } // if o->class_decl
      } // _arg&&class_decl
   } // if is class
   else if(YAC_CHK(_arg, YAC_CLID_HASHTABLE))
   {
      initFromHashTable((YAC_HashTable*)_arg);
   }
   else if(YAC_VALID(_arg))
   {
      // Try to assign from array
      sUI ne = _arg->yacArrayGetNumElements();
      if(ne>0)
      {
         if(class_decl->all_members.max_elements>0)
         {
            sUI i=0;
            sUI j=0;
            TKS_ClassDeclMember**acdms = class_decl->all_members.elements;
            yacmem *md = member_data;
            while( (i<ne) && (i<class_decl->all_members.max_elements) )
            {
               TKS_ClassDeclMember *cdm = *acdms++;
               YAC_Value v;
               _arg->yacArrayGetDeref(NULL, i, &v); // xxx TKS_MT: pass context (exceptions!)
               // Assign array value to class member
               YAC_Value w;
               w.type = cdm->rtti>YAC_TYPE_STRING ? YAC_TYPE_OBJECT: cdm->rtti;
               if(w.type>=YAC_TYPE_OBJECT)
               {
                  w.deleteme = md[j].b;
                  w.value.object_val = md[j+1].o;
                  ////w.derefPointerAssignValue(&v);
                  w.assignValue(&v); // do not change lvalue pointer
                  md[j].b = w.deleteme;
                  md[j+1].o = w.value.object_val;
                  w.deleteme=0;
                  v.unset();
                  j+=2;
               }
               else
               {
                  w.value.int_val = md[j].s32;
                  w.assignValue(&v);
                  md[j++].s32 = w.value.int_val;
                  v.unset();
               }
               i++;
            }
         }
      }

   }

   // Call scripted '=' operator if available
   {
      PTN_Function *f = class_decl->callback.fun_yacOperatorAssign;
      if(f)
      {
         if(!f->num_local_vars) // xxx TODO: add support for local vars
         {
            PTN_Env env(tkscript->lockGlobalContext());
            env.cret.initObject(_arg, 0);

            f->setArg(env.context, 0, &env.cret);
            env.cret.unset();

            env.context->pushClassStack((TKS_ScriptClassInstance*)this);
            f->evalCall(&env, &env.cret);
            env.context->popClassStack();

            env.cret.unsetFast();
            tkscript->unlockGlobalContext();
            return;
         }
      }
   }
}

sBool YAC_VCALL TKS_ScriptClassInstance::yacToString(YAC_String *_r) const {
   // ---- try to call script class method "yacToString"
   sBool ret=0;
   {
      PTN_Function *f = class_decl->callback.fun_yacToString;
      if(f)
      {
         if(!f->num_local_vars)
         {
            // xxx TKS_MT: hack to make yacToString work again. (context creation is (currently) slow)
            TKS_Context *ctx = tkscript->lockGlobalContext();
            PTN_Env env(ctx);
            PTN_Env *_env = &env;

            Dbeginfuncall(f);

            // xxx yacToString() script method may not have local variables
            env.cret.initString(_r, 0);
            f->setArg(ctx, 0, &env.cret);
            env.cret.unset();

            ctx->pushClassStack((TKS_ScriptClassInstance*)this);
            f->evalCall(&env, &env.cret);
            env.cret.unset();
            ctx->popClassStack();

            ret=1;
            tkscript->unlockGlobalContext();

            Dendfuncall(f);
         }
      }
   }
   // ---- call default yacToString
   if(ret)
   {
      return ret;
   }
   else
   {
      return YAC_Object::yacToString(_r);
   }
}

void YAC_VCALL TKS_ScriptClassInstance::yacOperator(sSI _cmd, YAC_Object *_arg, YAC_Value *_r) {
   if(YAC_OP_ASSIGN == _cmd)
   {
      TKS_ScriptClassInstance::yacOperatorAssign(_arg);
   }
   else
   {
      // ---- try to call script class method "yacOperator"
      {
         PTN_Function *f = class_decl->callback.fun_yacOperator;
         if(f)
         {
            if(!f->num_local_vars)  // xxx TODO: add support for local vars
            {
               TKS_Context *ctx = tkscript->lockGlobalContext();
               PTN_Env env(ctx);

               env.cret.initInt(_cmd);
               f->setArg(ctx, 0, &env.cret);

               env.cret.initObject(_arg, 0);
               f->setArg(env.context, 1, &env.cret);
               env.cret.unset();

               env.context->pushClassStack(this);
               f->evalCall(&env, _r);
               env.context->popClassStack();

               tkscript->unlockGlobalContext();
               return;
            }
         }
      }
      // ---- call default operator
      YAC_Object::yacOperator(_cmd, _arg, _r);
   }
}

void TKS_ScriptClassInstance::callMemberInitializersPushSelf(PTN_Env *_env) {

   if(this == class_decl->class_template)
   {
      return;
   }

   _env->context->pushClassStack(this);

   // ---- initialize members (int i=42)
   callMemberInitializers( _env );

   _env->context->popClassStack();
}

void TKS_ScriptClassInstance::callConstructors(PTN_Env *_env) {

   if(this == class_decl->class_template)
   {
      return;
   }

   sUI ne = class_decl->constructor_list.num_elements;
   sUI i;
   _env->context->pushClassStack(this);

   for(i=0; i<ne; i++)
   {
      TKS_ClassDeclMethod *m = class_decl->constructor_list.elements[i];

      Dbeginfuncall(&m->function);

#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->pushFunctionCall(&m->function, class_decl->src_loc);
#endif

      m->function.evalVoidCall(_env->context);

#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->popFunctionCall();
#endif

      Dendfuncall(&m->function);

   }
   _env->context->popClassStack();
}

void TKS_ScriptClassInstance::callDestructors(PTN_Env *_env) {
   if(this == class_decl->class_template)
   {
      return;
   }

   sUI ne = class_decl->destructor_list.num_elements;
   sUI i;
   _env->context->pushClassStack(this);

   for(i=0; i<ne; i++)
   {
      TKS_ClassDeclMethod *m=class_decl->destructor_list.elements[i];
      sU16 oerrcode = tkscript->ptn_error_code;
      tkscript->ptn_error_code=0;

#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->pushFunctionCall(&m->function, class_decl->src_loc);
#endif

      m->function.evalVoidCall(_env->context);

#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->popFunctionCall();
#endif

      tkscript->ptn_error_code=oerrcode;
   }
   _env->context->popClassStack();
}

TKS_CachedObject *TKS_ScriptClassInstance::getMethod(const char *_name) {
   YAC_String s; s.visit(_name);
   return getMethod(&s);
}

TKS_CachedObject *TKS_ScriptClassInstance::getMethod(YAC_String *_name) {
   return class_decl->template_method_hashtable.findEntry(_name);
}

TKS_CachedObject *TKS_ScriptClassInstance::getMember(YAC_String *_name) {
   return class_decl->template_member_hashtable.findEntry(_name);
}

void TKS_ScriptClassInstance::getPreIncClassMemberValueByID(YAC_Value *r, sUI _id) {
   sU16 cortti = (sU16)( (_id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK );
   sU16 cooff  = (sU16)( (_id & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_id >> TKS_CLASSDECL_IDSHIFT)&0xFFFF] );
   if(cortti<TKS_CLASSDECL_RTTI_OBJECT)
   {
      if(cortti==TKS_CLASSDECL_RTTI_INT)
      {
         member_data[cooff].s32++;
         r->initInt( member_data[cooff].s32 );
      }
      else
      {
         member_data[cooff].f32 += 1.0f;
         r->initFloat( member_data[cooff].f32 );
      }
   }
   else
   {
      YAC_Object *oval = member_data[cooff+1].o;
      if(YAC_VALID(oval))
      {
         YAC_Value vv;
         oval->yacOperatorI(YAC_OP_ADD, 1, &vv);
         r->initObject(oval, 0);
      }
      else
      {
         r->initNull();
      }
   }
}

void TKS_ScriptClassInstance::getPreDecClassMemberValueByID(YAC_Value *r, sUI _id) {
   sU16 cortti = (sU16)( (_id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK );
   sU16 cooff  = (sU16)( (_id & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_id >> TKS_CLASSDECL_IDSHIFT)&0xFFFF] );
   if(cortti < TKS_CLASSDECL_RTTI_OBJECT)
   {
      if(cortti == TKS_CLASSDECL_RTTI_INT)
      {
         member_data[cooff].s32--;
         r->initInt( member_data[cooff].s32 );
      }
      else
      {
         member_data[cooff].f32 -= 1.0f;
         r->initFloat( member_data[cooff].f32 );
      }
   }
   else
   {
      YAC_Object *oval = member_data[cooff+1].o;
      if(YAC_VALID(oval))
      {
         YAC_Value vv;
         oval->yacOperatorI(YAC_OP_SUB, 1, &vv);
         r->initObject(oval, 0);
      }
      else
      {
         r->initNull();
      }
   }
}

void TKS_ScriptClassInstance::getPostIncClassMemberValueByID(YAC_Value *r, sUI _id) {
   sU16 cortti = (sU16)( (_id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK );
   sU16 cooff  = (sU16)( (_id & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_id >> TKS_CLASSDECL_IDSHIFT)&0xFFFF] );
   if(cortti < TKS_CLASSDECL_RTTI_OBJECT)
   {
      if(cortti == TKS_CLASSDECL_RTTI_INT)
      {
         r->initInt( member_data[cooff].s32 );
         member_data[cooff].s32++;
      }
      else
      {
         r->initFloat( member_data[cooff].f32 );
         member_data[cooff].f32 += 1.0f;
      }
   }
   else
   {
      YAC_Object *oval = member_data[cooff+1].o;
      if(YAC_VALID(oval))
      {
         YAC_Object *o = YAC_CLONE_POOLED(NULL, oval); // xxx TKS_MT: should use *real* thread context (c'tors)
         o->yacOperatorAssign(oval); // ^^ overrides initialized members anyway
         {
            YAC_Value vv;
            oval->yacOperatorI(YAC_OP_ADD, 1, &vv);
         }
         r->initObject(o, 1);
      }
      else
      {
         r->initNull();
      }
   }
}

void TKS_ScriptClassInstance::getPostDecClassMemberValueByID(YAC_Value *r, sUI _id) {
   sU16 cortti = (sU16)( (_id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK );
   sU16 cooff  = (sU16)( (_id & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_id >> TKS_CLASSDECL_IDSHIFT)&0xFFFF] );
   if(cortti < TKS_CLASSDECL_RTTI_OBJECT)
   {
      if(cortti == TKS_CLASSDECL_RTTI_INT)
      {
         r->initInt( member_data[cooff].s32 );
         member_data[cooff].s32--;
      }
      else
      {
         r->initFloat( member_data[cooff].f32 );
         member_data[cooff].f32 -= 1.0f;
      }
   }
   else
   {
      YAC_Object *oval = member_data[cooff+1].o;
      if(YAC_VALID(oval))
      {
         YAC_Object *o = YAC_CLONE_POOLED(NULL, oval); // xxx TKS_MT: should use *real* thread context
         o->yacOperatorAssign(oval); // ^^ overrides initialized members anyway
         {
            YAC_Value vv;
            oval->yacOperatorI(YAC_OP_SUB, 1, &vv);
         }
         r->initObject(o, 1);
      }
      else
      {
         r->initNull();
      }
   }
}

void TKS_ScriptClassInstance::postIncClassMemberValueByID(sUI _id) {
   sU16 cortti = (sU16)( (_id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK );
   sU16 cooff  = (sU16)( (_id & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_id >> TKS_CLASSDECL_IDSHIFT)&0xFFFF] );
   if(cortti < TKS_CLASSDECL_RTTI_OBJECT)
   {
      if(cortti == TKS_CLASSDECL_RTTI_INT)
      {
         member_data[cooff].s32++;
      }
      else
      {
         member_data[cooff].f32 += 1.0f;
      }
   }
   else
   {
      YAC_Object *oval = member_data[cooff+1].o;
      if(YAC_VALID(oval))
      {
         YAC_Value vv;
         oval->yacOperatorI(YAC_OP_ADD, 1, &vv);
      }
   }
}

void TKS_ScriptClassInstance::postDecClassMemberValueByID(sUI _id) {
   sU16 cortti = (sU16)( (_id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK );
   sU16 cooff  = (sU16)( (_id & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_id >> TKS_CLASSDECL_IDSHIFT)&0xFFFF] );
   if(cortti < TKS_CLASSDECL_RTTI_OBJECT)
   {
      if(cortti == TKS_CLASSDECL_RTTI_INT)
      {
         member_data[cooff].s32--;
      }
      else
      {
         member_data[cooff].f32 -= 1.0f;
      }
   }
   else
   {
      YAC_Object *oval = member_data[cooff+1].o;
      if(YAC_VALID(oval))
      {
         YAC_Value vv;
         oval->yacOperatorI(YAC_OP_SUB, 1, &vv);
      }
   }
}

yacmem *TKS_ScriptClassInstance::getIntMemberAdrByName(const char *_name) {
   yacmem *r = NULL;
   YAC_String name;
   name.visit(_name);
   TKS_CachedObject *co=getMember(&name);
   if(co)
   {
      sUI id = (sUI)co->value.int_val;
      if( ((id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK) == TKS_CLASSDECL_RTTI_INT )
      {
         r = getMemberAdrByID(id);
      }
   }
   return r;
}

PTN_Function *TKS_ScriptClassInstance::getMethodFunctionByName(const char *_name) {
   YAC_String name;
   name.visit(_name);
   TKS_CachedObject *co=getMethod(&name);
   if(co)
   {
      TKS_ClassDeclMethod *cdm = (TKS_ClassDeclMethod*)co->value.object_val;
      if(cdm)
      {
         return &cdm->function;
      }
   }
   return 0;
}


yacmem *TKS_ScriptClassInstance::getMemberAdrByID(sUI _id) {
   sU16 cooff = (sU16)( (_id & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_id >> TKS_CLASSDECL_IDSHIFT)&0xFFFF] );
   switch((_id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK)
   {
   case TKS_CLASSDECL_RTTI_INT:
   case TKS_CLASSDECL_RTTI_FLOAT:
      return &member_data[cooff];
   case TKS_CLASSDECL_RTTI_STRING:
   case TKS_CLASSDECL_RTTI_OBJECT:
   case TKS_CLASSDECL_RTTI_CLASS:
   case TKS_CLASSDECL_RTTI_POINTER:
   case TKS_CLASSDECL_RTTI_INTARRAY:
   case TKS_CLASSDECL_RTTI_FLOATARRAY:
   case TKS_CLASSDECL_RTTI_STRINGARRAY:
   case TKS_CLASSDECL_RTTI_OBJECTARRAY:
   case TKS_CLASSDECL_RTTI_POINTERARRAY:
   case TKS_CLASSDECL_RTTI_CLASSARRAY:
      return &member_data[cooff+1];
   }
   return NULL;
}

void TKS_ScriptClassInstance::getClassMemberValueByID(YAC_Value *r, sUI _id) {
   sU16 cortti = (sU16)( (_id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK );
   sU16 cooff  = (sU16)( (_id & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_id >> TKS_CLASSDECL_IDSHIFT)&0xFFFF] );
   switch(cortti & TKS_CLASSDECL_RTTIMASK)
   {
   case TKS_CLASSDECL_RTTI_INT:
      r->initInt(member_data[cooff].s32);
      break;
   case TKS_CLASSDECL_RTTI_FLOAT:
      r->initFloat(member_data[cooff].f32);
      break;
   case TKS_CLASSDECL_RTTI_STRING:
      r->initString(member_data[cooff+1].s, 0);
      break;
   case TKS_CLASSDECL_RTTI_OBJECT:
   case TKS_CLASSDECL_RTTI_CLASS:
   case TKS_CLASSDECL_RTTI_POINTER:
   case TKS_CLASSDECL_RTTI_INTARRAY:
   case TKS_CLASSDECL_RTTI_FLOATARRAY:
   case TKS_CLASSDECL_RTTI_STRINGARRAY:
   case TKS_CLASSDECL_RTTI_OBJECTARRAY:
   case TKS_CLASSDECL_RTTI_POINTERARRAY:
   case TKS_CLASSDECL_RTTI_CLASSARRAY:
      r->initObject(member_data[cooff+1].o, 0);
      break;
   }
}


void TKS_ScriptClassInstance::getDerefClassMemberValueByID(YAC_Value *r, sUI _id) {
   sU16 cortti = (sU16)( (_id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK );
   sU16 cooff  = (sU16)( (_id & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_id >> TKS_CLASSDECL_IDSHIFT)&0xFFFF] );
   switch(cortti & TKS_CLASSDECL_RTTIMASK)
   {
   case TKS_CLASSDECL_RTTI_INT:
      r->initInt(member_data[cooff].s32);
      break;
   case TKS_CLASSDECL_RTTI_FLOAT:
      r->initFloat(member_data[cooff].f32);
      break;
   case TKS_CLASSDECL_RTTI_STRING:
      // Unlink object
      r->initString(member_data[cooff+1].s, member_data[cooff+0].b);
      member_data[cooff+0].b = YAC_FALSE;
      break;
   case TKS_CLASSDECL_RTTI_OBJECT:
   case TKS_CLASSDECL_RTTI_CLASS:
   case TKS_CLASSDECL_RTTI_POINTER:
   case TKS_CLASSDECL_RTTI_INTARRAY:
   case TKS_CLASSDECL_RTTI_FLOATARRAY:
   case TKS_CLASSDECL_RTTI_STRINGARRAY:
   case TKS_CLASSDECL_RTTI_OBJECTARRAY:
   case TKS_CLASSDECL_RTTI_POINTERARRAY:
   case TKS_CLASSDECL_RTTI_CLASSARRAY:
      // Unlink object
      r->initObject(member_data[cooff+1].o, member_data[cooff+0].b);
      member_data[cooff+0].b = YAC_FALSE;
      break;
   }
}


void TKS_ScriptClassInstance::setClassMemberValueByID(/*void *_context, */sUI _id, YAC_Value*_v, sU8 _op) {
   sU16 cortti = (sU16)( (_id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK );
   sU16 cooff  = (sU16)( (_id & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_id >> TKS_CLASSDECL_IDSHIFT)&0xFFFF] );
   static const sU8 rtti_map[]={
      0,
         YAC_TYPE_INT,
         YAC_TYPE_FLOAT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_STRING,
         YAC_TYPE_OBJECT, // xxx \|/ superfluous !?!!
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT
   };
   YAC_Value a;
   a.value.any = NULL;
   a.deleteme  = YAC_FALSE;
   a.type      = rtti_map[cortti];

   if(a.type>=YAC_TYPE_OBJECT)
   {
      a.value.any = member_data[cooff+1].any;
   }
   else
   {
      a.value.int_val = member_data[cooff].s32;
   }
   if(_op==YAC_OP_ASSIGN)
   {
      a.assignValue(_v);
#if 0
      if(a.type>=YAC_TYPE_OBJECT) // copy object/int/float.. content
      {
         YAC_Value rt;
         // xxx TKS_MT: MUST use *real* thread context (?)
         PTN_Env env; env.continue_flag=1; env.context=NULL; //_context; /////tkscript->main_context;
         PTN_Env*_env = &env;
         PTN_DoubleArgExpr::EvalOp(_env, &rt, &a, _v, YAC_OP_ASSIGN, NULL/*src_loc*/); // object is directly modified
         rt.unsetFast(); // new
      }
      else
      {
         YAC_Value rt;
         // xxx TKS_MT: MUST use *real* thread context (?)
         PTN_Env env; env.continue_flag=1; env.context=NULL; //_context; /////tkscript->main_context;
         PTN_Env*_env = &env;
         PTN_DoubleArgExpr::EvalOp(_env, &rt, &a, _v, YAC_OP_ASSIGN, NULL/*src_loc*/);
         a=&rt;
      }
#endif // 0
   }
   else
   {
      // ---- a = a + v
      YAC_Value b;
      // xxx TKS_MT: MUST use *real* thread context (?)
      PTN_Env env; env.initDefault();
      PTN_Env*_env = &env;
      YAC_Value ta; ta.assignValue(&a); // EvalOp unsets "a" otherwise!
      PTN_DoubleArgExpr::EvalOp(_env, &b, &ta, _v, _op, NULL/*src_loc*/);
      // ---- b might contain a new object
      if(a.type>=YAC_TYPE_OBJECT)
      {
         YAC_Value rt;
         ta.assignValue(&a);
         PTN_DoubleArgExpr::EvalOp(_env, &rt, &ta, &b, YAC_OP_ASSIGN, NULL/*src_loc*/); // object is directly modified
         rt.unsetFast(); // new
      }
      else
      {
         YAC_Value rt;
         ta.assignValue(&a);
         PTN_DoubleArgExpr::EvalOp(_env, &rt, &ta, &b, YAC_OP_ASSIGN, NULL/*src_loc*/);
         a=&rt;
      }
      b.unsetFast();
   }
   if(a.type<YAC_TYPE_OBJECT)
   {
      member_data[cooff].s32 = a.value.int_val;
   }
   else
   {
      // xxx unnecessary ? (a is never deletable..)
      a.unsetFast(); // object is already modified
   }
}


void TKS_ScriptClassInstance::setClassMemberPointerValueByID(sUI _id, YAC_Value*_v) {
   sU16 cortti= (sU16)( (_id >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK );
   sU16 cooff = (sU16)( (_id & TKS_CLASSDECL_OFFMASK) + class_decl->class_relocation_table[(_id >> TKS_CLASSDECL_IDSHIFT)&0xFFFF] );
   static const sU8 rtti_map[]={
      0,
         YAC_TYPE_INT,
         YAC_TYPE_FLOAT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_STRING,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT,
         YAC_TYPE_OBJECT
   };
   sU8 a_type=rtti_map[cortti];
   if(a_type>=YAC_TYPE_OBJECT)
   {
      YAC_Object *o = member_data[cooff+1].o;
      sBool del = (member_data[cooff].s32!=0);
      if(_v->type>2)
      {
         if(_v->value.object_val!=o) // a<=a
         {
            if(del)
            {
               YAC_DELETE(o);
            }
            member_data[cooff+1].o = _v->value.object_val;
            if(_v->deleteme)
            {
               member_data[cooff].b = YAC_TRUE;
               _v->deleteme = YAC_FALSE;
            }
            else
            {
               member_data[cooff].b = YAC_FALSE;
            }
         }
         else
         {
            // a<=a, same memaddress
            if(_v->deleteme)
            {
               _v->deleteme = YAC_FALSE;
               member_data[cooff].b = YAC_TRUE;
            }
         }
      }
      else
      {
         //if(_v->type==1)
         //   if(_v->value.int_val==0)
         //   {
         if(del)
         {
            YAC_DELETE(o);
         }
         member_data[cooff+1].o = NULL; // unset object reference
         member_data[cooff+0].b = YAC_FALSE; // clear delete flag
         //   }
      }
   }
}

void TKS_ScriptClassInstance::setDelegate(YAC_String *_name, PTN_Function *_method) {
   if(!delegates)
   {
      delegates = (YAC_HashTable*)YAC_New_HashTable();
   }
   YAC_Value v; v.initAny((void *)_method, 0);
   delegates->yacHashSet(NULL, _name, &v);  // xxx TKS_MT: should use *real* thread context (exceptions!)
}

sBool TKS_ScriptClassInstance::evalDelegate(YAC_String *_name, PTN_ArgList *_args,
                                            sUI _srcLoc, PTN_Env *_env, YAC_Value *_r
                                            ) const
{
   if(delegates)
   {
      YAC_Value v;
      delegates->yacHashGet((void*)_env->context, _name, &v);
      if(v.type >= YAC_TYPE_OBJECT)
      {
         PTN_Function *f = (PTN_Function*) v.value.object_val;

         Dbeginfuncall(f);

         YAC_Value *nsf = _env->context->pushStackFrame(f);
         if(nsf)
         {
            YAC_Value *stackc = _env->context->tks_fstackc;

            // Validate arguments
            if(_args)
            {
               if(f->argvarlist)
               {
                  if(_args->num_args == f->argvarlist->num_vars)
                  {
                     f->setArgsByList(_env, _args, nsf);
                  }
                  else
                  {
                     Dprintf("[---] wrong argument count in call to delegate \"%s::%s::%s\" function \"%s\" (have=%d, expect=%d)\n",
                        (char*)class_decl->nspace->name.chars,
                        (char*)class_decl->name.chars,
                        (char*)_name->chars,
                        (char*)f->name.chars,
                        _args->num_args,
                        f->argvarlist->num_vars
                        );
                     _env->context->popStackFrame(f);
                     _env->context->tks_fstackc = stackc;
                     Dendfuncall(f);
                     // XXX TODO: add new exception type for this kind of error ?
                     Drtthrow(NULL, TKS_EXCEPTION_CRITICALERROR, "wrong number of args to delegate function");
                     return 0;
                  }
               }
               else
               {
                  Dprintf("[---] wrong argument count in call to delegate \"%s::%s::%s\" function \"%s\" (have=%d, expect=0)\n",
                        (char*)class_decl->nspace->name.chars,
                        (char*)class_decl->name.chars,
                        (char*)_name->chars,
                        (char*)f->name.chars,
                        _args->num_args
                        );
                  _env->context->popStackFrame(f);
                  _env->context->tks_fstackc = stackc;
                  Dendfuncall(f);
                  // XXX TODO: add new exception type for this kind of error ?
                  Drtthrow(NULL, TKS_EXCEPTION_CRITICALERROR, "wrong number of args to delegate function");
                  return 0;
               }
            }
            else
            {
               if(f->argvarlist)
               {
                  Dprintf("[---] wrong argument count in call to delegate \"%s::%s::%s\" function \"%s\" (have=0, expect=%d)\n",
                     (char*)class_decl->nspace->name.chars,
                     (char*)class_decl->name.chars,
                     (char*)_name->chars,
                     (char*)f->name.chars,
                     f->argvarlist->num_vars
                     );
                  _env->context->popStackFrame(f);
                  _env->context->tks_fstackc = stackc;
                  Dendfuncall(f);
                  // XXX TODO: add new exception type for this kind of error ?
                  Drtthrow(NULL, TKS_EXCEPTION_CRITICALERROR, "wrong number of args to script function");
                  return 0;
               }
            }
            ///////f->setArgsByList(_env, _args, nsf);

#ifdef DX_TRACEFUNCTIONCALLS
            _env->context->pushFunctionCall(f, _srcLoc);
#else
            (void)_srcLoc;
#endif

            _env->context->pushStackFrame2(f);

            _env->context->pushClassStack((TKS_ScriptClassInstance*)this);

            f->evalCall(_env, _r);

#ifdef DX_TRACEFUNCTIONCALLS
            _env->context->popFunctionCall();
#endif

            _env->context->popStackFrame(f);
            _env->context->tks_fstackc=stackc;

            _env->continue_flag = YAC_TRUE;

            _env->context->popClassStack();

            Dendfuncall(f);
            return YAC_TRUE; // method executed OK.

         }  // if nsf

         Dendfuncall(f);

      } // if YAC_TYPE_OBJECT
   } // if delegates
   return YAC_FALSE; // method execution FAILED.
}

sBool TKS_ScriptClassInstance::initFromHashTable(YAC_HashTable *_ht) {
   TKS_CachedObject *cc_co;
   ////yacmem *madr, *omadr;
   sU8 bytes[YAC_MAX_ITERATOR_SIZE];
   YAC_Iterator *it = (YAC_Iterator *)bytes;
   _ht->yacIteratorInit(it);
   it->begin();
   YAC_Value r; it->getNext(&r);
   while(r.type!=0xFF)
   {
      cc_co=getMember(r.value.string_val);
      if(cc_co)
      {
         // ---- Class member found
         sUI ak = cc_co->value.int_val;
         YAC_Value hv; _ht->yacHashGetDeref(NULL, r.value.string_val, &hv); // xxx TKS_MT: should use real thread context (exceptions!)
         TKS_ClassDeclMember *cdm = class_decl->findMemberSuper(r.value.string_val, 0);
         ////madr = getMemberAdrByID(ak);
         ////omadr = getMemberAdrByID(ak);
         // Assign member value from hashtable value
         hv.typecast(cdm->rtti>YAC_TYPE_STRING ? YAC_TYPE_OBJECT : cdm->rtti);
         this->yacMetaClassMemberSet(ak, &hv);
         hv.unset();
      }
      else
      {
         // ---- Class member does not exist
#ifdef TKS_DCON
         if(tkscript->yacGetDebugLevel())
         {
            Dprintf("[---] TKS_ScriptClassInstance::initFromHashTable: class has no member named \"%s\".\n",
                    r.value.string_val
                    );
         }
#endif
         it->end();
         return YAC_FALSE;
      }
      it->getNext(&r);
   } // for

   it->end();
   return YAC_TRUE;
 }

sBool TKS_ScriptClassInstance::writeToHashTable(YAC_HashTable *_ht) {
   _ht->_free();
   sUI numMembers = yacMetaClassMemberGetNum();
   _ht->_alloc(numMembers);
   if( ((sUI)_ht->_getMaxElements()) >= numMembers )
   {
      sUI i;
      yacmem *madr;
      YAC_String key;
      for(i=0; i<numMembers; i++)
      {
         sUI ak = yacMetaClassMemberGetAccessKeyByIndex(i);
         key.visit(yacMetaClassMemberGetName(ak));

         /// ---- now store value ----
         madr = getMemberAdrByID(ak);

         YAC_Value v;
         switch(yacMetaClassMemberGetType(ak))
         {
         case 1:
            // ---- write integer ----
            v.initInt(madr[0].s32);
            break;
         case 2:
            // ---- write float ----
            v.initFloat(madr[0].f32);
            break;
         default:
            // ---- write object ----
            if(YAC_VALID(madr[0].o))
            {
               v.initObject(madr[0].o, YAC_FALSE); // Store reference to Object
               v.fixStringType(); // xxx can skip this ???
            }
            else
            {
               v.initNull();
            }
            break;
         }

         _ht->yacHashSet(NULL, &key, &v); /// xxx TKS_MT: should use *real* thread context (exceptions!)

      } // for
      return YAC_TRUE;
   }
   else
   {
      Dprintf("[---] TKS_ScriptClassInstance::writeToHashTable: hashtable has not enough entries to store all object members (%i<%i).\n",
         _ht->_getMaxElements(), numMembers);
      return YAC_FALSE;
   }
}
