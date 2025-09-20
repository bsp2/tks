/// YAC_HashTable.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#include "tks.h"

#include "tks_inc_class.h"

#include "YAC_HashTable.h"
#include "TKS_HashTableIterator.h" 
#include "YAC_ValueObject.h"

 
YAC_HashTable::YAC_HashTable(void) {
   class_ID = YAC_CLID_HASHTABLE; 
}

YAC_HashTable::~YAC_HashTable() {
}

sBool YAC_VCALL YAC_HashTable::yacIteratorInit(YAC_Iterator *_it) const {
   Diteratorinit(TKS_HashTableIterator);
   return 1;
}

sBool YAC_VCALL YAC_HashTable::yacToString(YAC_String *_s) const {
   return yacToString2(_s, 0);
}

sBool YAC_HashTable::yacToString2(YAC_String *_s, sBool _bReparsable) const {
   // (todo) bReparsable does not work recursively => add yacToReparsableString()
   _s->alloc(2+varcache.num_entries*32+1);
   _s->empty();
   _s->append("#[");
   sUI i;
   YAC_String s;
   TKS_CachedObject **cp = varcache.objects;
   sSI j=0;

   for(i=0; i<varcache.max_entries; i++)
   {
      TKS_CachedObject *c = *cp++;
      if(NULL != c)
      {
         // (note) if used as variable storage, c->type can be 0 (void/dynamic). c->flags contains TKS_CachedObject::ISDYNAMIC in that case
         //         since this method is meant for HashTable type vars, void always flags an unused/deleted entry
         if(YAC_TYPE_VOID != c->type)
         {
            if(j++) 
            {
               _s->append(", "); 
            }

            _s->append("\"");

            if(c->name.indexOfControlCharacter(0) != -1) // \"\" (?)
            {
               ////s.empty();
               ////c->name.yacToString(&s);
               ////_s->append(&s);
               _s->append(&c->name);
            }
            else 
            {
               if(_bReparsable)
               {
                  c->name._escapeControlChars_YAC_RARG(&s);
                  _s->append(&s); 
               }
               else
               {
                  _s->append(&c->name); 
               }
            }

            _s->append("\"");

            _s->append("=");

            switch(c->type)
            {
               case YAC_TYPE_INT:
               {
                  s.empty();
                  s.yacValueOfI(c->value.int_val);
                  _s->append(&s);
               }
               break;

               case YAC_TYPE_FLOAT:
               {
                  s.empty();
                  s.yacValueOfF32(c->value.float_val);
                  _s->append(&s);
               }
               break;

               case YAC_TYPE_OBJECT:
               case YAC_TYPE_STRING:
                  if(YAC_VALID(c->value.object_val)) 
                  {
                     s.empty(); 
                     if(_bReparsable)
                     {
                        if(YAC_TYPE_STRING == c->type)
                        {
                           YAC_String t;
                           c->value.string_val->_escapeControlChars_YAC_RARG(&t);
                           t.yacToString(&s);  // restore surrounding quotes, if any
                        }
                        else
                        {
                           c->value.object_val->yacToString(&s);  // restore surrounding quotes, if any
                        }
                     }
                     else
                     {
                        c->value.object_val->yacToString(&s);  // restore surrounding quotes, if any
                     }
                     _s->append(&s); 
                  } 
                  else 
                  { 
                     ////if(c->isNullOrIF0()) 
                     if(c->isNull())
                     { 
                        if(_bReparsable)
                        {
                           _s->append("null"); 
                        }
                        else
                        {
                           _s->append("<null>"); 
                        }
                     } 
                     else 
                     { 
                        _s->append("*ILL*"); 
                     } 
                  }
                  break;

               default:
                  //_s->append("");
                  break;
            }
         }
      }
   }

   _s->append("]");
   return 1;
}

void YAC_HashTable::_getString(YAC_Value *_r) const {
   _r->initEmptyString();
   yacToString2(_r->value.string_val, 0);
}

void YAC_HashTable::_getReparsableString(YAC_Value *_r) const {
   _r->initEmptyString();
   yacToString2(_r->value.string_val, 1);
}

void YAC_VCALL YAC_HashTable::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   
   YAC_SERIALIZE_I32(varcache.max_entries);
   YAC_SERIALIZE_I32(varcache.num_entries);
   
   sUI j=0;
   
   if(varcache.num_entries)
   {
      TKS_CachedObject **cop=varcache.objects;
      sUI i;
      for(i=0; i<varcache.max_entries; i++)
      {
         /// ---- is this cache entry allocated ? ----
         if(cop[i])
         {
            /// ---- is this cache entry actually in use ? ----
            //if(cop[i]->cache_index)
            {
               if(j>=varcache.num_entries)
               {
                  Dprintf("[---] HashTable::serialize: cache has more linked entries than previously indicated (%i>%i).\n",
                     j, varcache.num_entries);
                  return;
               }
               switch(cop[i]->type) 
               { 
               default:
               case YAC_TYPE_INT:
                  YAC_SERIALIZE_I8(YAC_TYPE_INT);         // store type
                  cop[i]->name.yacSerialize(_ofs, 0);            // store name 
                  YAC_SERIALIZE_I32(cop[i]->value.int_val); // store value 
                  j++;
                  break;
               case YAC_TYPE_FLOAT:
                  YAC_SERIALIZE_I8(YAC_TYPE_FLOAT);         // store type
                  cop[i]->name.yacSerialize(_ofs, 0);              // store name 
                  YAC_SERIALIZE_F32(cop[i]->value.float_val); // store value
                  j++; 
                  break;
               case YAC_TYPE_OBJECT:
               case YAC_TYPE_STRING:
                  {
                     YAC_SERIALIZE_I8((sU8)cop[i]->type);        // store type
                     cop[i]->name.yacSerialize(_ofs, 0);            // store name 
                     sBool serialized=0;
                     /// ---- only store deletable objects to avoid recursion ----
                     if(YAC_VALID(cop[i]->value.object_val))
                     {
                        if( (!cop[i]->value.object_val->yacIsComposite()) || cop[i]->deleteme)
                        {
                           /// ---- used object tag ----
                           YAC_SERIALIZE_I8(1); // tag used
                           cop[i]->value.object_val->yacSerialize(_ofs, 1); // store value
                           serialized=1;
                        }
                     }
                     if(!serialized)
                     {
                        YAC_SERIALIZE_I8(0);  // mark unused
                     }
                  }
                  j++; 
                  break;
               }
            }
         }
      }
      if(j!=varcache.num_entries)
      {
         Dprintf("[---] HashTable::serialize: cache has less linked entries than previously indicated. (corrupted stream) (%i<%i)\n",
            j, varcache.num_entries);
      }
   }
}

sUI YAC_VCALL YAC_HashTable::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   
   varcache.freeCache();
   
   /// ---- get max_entries ----
   sUI me = YAC_DESERIALIZE_I32();
   if(me >= tkscript->configuration.streamMaxArraySize)
   {
      Dprintf("[---] HashTable::deserialize: insane max_entries (%i>%i)\n",
         me, tkscript->configuration.streamMaxArraySize);
      return 0u;
   }
   
   /// ---- get num_entries ----
   sUI ne = YAC_DESERIALIZE_I32();
   if(ne >= tkscript->configuration.streamMaxArraySize)
   {
      Dprintf("[---] HashTable::deserialize: insane num_entries (%i>%i)\n",
         me, tkscript->configuration.streamMaxArraySize);
      return 0u;
   }
   
   /// ---- if allocation fails just perform a dummy read ----
   sBool ac = varcache.allocCache(me);
   if(!ac)
   {
      Dprintf("[---] HashTable::deserialize: failed to allocate cache with %i entries. using dummy read.\n", me);
   }
   
   sUI i;
   YAC_String coname;
   //coname.setEngine(tkscript);
   TKS_CachedObject *co;
   YAC_Object *o;
   for(i = 0u; i < ne; i++)
   {
      sU8 user_rtti = YAC_DESERIALIZE_I8();
      switch(user_rtti)
      {
      default:
         {
            Dprintf("[---] HashTable::deserialize: cache entry has illegal user_rtti %i.\n",
               user_rtti);
            return 0u;
         }

      case YAC_TYPE_INT:
         coname.yacDeserialize(_ifs, 0);
         if(ac)
         {
            co=varcache.createIntegerEntry(&coname, 1);
            co->value.int_val=YAC_DESERIALIZE_I32();
         }
         else
         {
            YAC_DESERIALIZE_I32(); // dummy read
         }
         break;

      case YAC_TYPE_FLOAT:
         coname.yacDeserialize(_ifs, 0);
         if(ac)
         {
            co=varcache.createFloatEntry(&coname, 1);
            co->value.float_val=YAC_DESERIALIZE_F32();
         }
         else
         {
            YAC_DESERIALIZE_F32(); // dummy read
         }
         break;

      case YAC_TYPE_OBJECT:
         coname.yacDeserialize(_ifs, 0);
         if(YAC_DESERIALIZE_I8()) /// use tag
         {
            o=tkscript->deserializeNewObject(_ifs, 0);
            if(ac)
            {
               varcache.createObjectEntry(&coname, 1, o, 1);
            }
            else
            {
               if(o) 
               {
                  YAC_DELETE(o);
               }
            }
         }
         else
         {
            varcache.createObjectEntry(&coname, 1, NULL, 0);
         }
         break;

      case YAC_TYPE_STRING:
         {
            coname.yacDeserialize(_ifs, 0);
            if(YAC_DESERIALIZE_I8()) /// use tag
            {
               YAC_String *ts = (YAC_String*) YAC_NEW_CORE(YAC_CLID_STRING);
               //ts->setEngine(tkscript);
               ts->yacDeserialize(_ifs, 1);
               if(ac)
               {
                  varcache.createStringEntry(&coname, 1, ts, 1);
               }
               else
               {
                  YAC_DELETE(ts);
               }
            }
            else
            {
               varcache.createObjectEntry(&coname, 1, NULL, 0);
            }
         }
         break;
      }
   }
   
   // // varcache.max_entries=me; // note: max_entries was already set by allocCache()!!
   varcache.num_entries = ne;
   return 1u;
}

sBool YAC_HashTable::_alloc(sUI _cachesize) {
   return varcache.allocCache(_cachesize); // xxx hmm , add +1 for NULL element here ?
}

sBool YAC_HashTable::_realloc(sUI _cachesize) {
   YAC_HashTable tmp;

   if(tmp._alloc(_cachesize))
   {
      tmp.addEntriesFrom(this);

      varcache.freeCache();

      varcache.objects     = tmp.varcache.objects;
      varcache.max_entries = tmp.varcache.max_entries;
      varcache.num_entries = tmp.varcache.num_entries;

      tmp.varcache.objects = NULL;
      tmp.varcache.num_entries = 0u;
      tmp.varcache.max_entries = 0u;
   }

   return YAC_FALSE;
}

void YAC_VCALL YAC_HashTable::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_OP_ASSIGN == _cmd)
   {
      if(_o != ((YAC_Object*)this))
      {
         if(YAC_CHK(_o, YAC_CLID_HASHTABLE))
         {
            YAC_HashTable *o = (YAC_HashTable*)_o;

            varcache.freeCache();

            if(o->varcache.num_entries >= varcache.max_entries) 
            {
               varcache.allocCache(o->varcache.num_entries + 1); 
            }

            addEntriesFrom(o);

            return;
         }
      } 
   }
   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_HashTable::addEntriesFrom(YAC_HashTable *_o) {

   for(sUI i=0; i<_o->varcache.max_entries; i++)
   {
      TKS_CachedObject *co = _o->varcache.objects[i];

      if(NULL != co)
      {
         switch(co->type)
         {
            case YAC_TYPE_VOID:
               break;

            case YAC_TYPE_INT:
               _addInt(&co->name, co->value.int_val);
               break;

            case YAC_TYPE_FLOAT:
               _addFloat(&co->name, co->value.float_val);
               break;

            case YAC_TYPE_STRING:
               _addString(&co->name, co->value.string_val);
               break;

            case YAC_TYPE_OBJECT:
               _addObject(&co->name, co->value.object_val);
               break;
         }
      }
   }
}

sSI YAC_HashTable::_addInt(YAC_String *_s, sSI _val) {
   TKS_CachedObject *co=varcache.createIntegerEntry(_s, 1);
   if(co)
   {
      co->value.int_val=_val;
      return 1;
   }
   else 
   {
      return 0; 
   }
}

sSI YAC_HashTable::_addFloat(YAC_String *_s, sF32 _val) {
   TKS_CachedObject *co=varcache.createFloatEntry(_s, 1);
   if(co)
   {
      co->value.float_val=_val;
      return 1;
   }
   else 
   {
      return 0; 
   }
}

sSI YAC_HashTable::_addString(YAC_String *_s, YAC_String *_v) {
   TKS_CachedObject *co = varcache.createStringInstance(_s, 1);
   if(co)
	{
      co->value.string_val->yacCopy(_v);
      return 1;
	}
   else 
   {
      return 0; 
   }
}

sSI YAC_HashTable::_addObject(YAC_String *_s, YAC_Object *_v) {
   TKS_CachedObject *co=0;
   if(_v)
   {
      YAC_Object *no;
      if(YAC_CHK(_v, YAC_CLID_CLASS)) 
      {
         no= YAC_CLONE_POOLED(NULL, ((TKS_ScriptClassInstance*)_v)->class_decl->class_template); 
      }
      else 
      {
         no = YAC_CLONE_POOLED(NULL, _v);
      }
      if(no)  
      { 
         no->yacOperatorAssign(_v); 
      }
      co=varcache.createObjectEntry(_s, 1, no, 1);
   }
   return (co!=0);
}

sSI YAC_HashTable::_addObjectRef(YAC_String *_s, YAC_Object *_v) {
   return _v ? (varcache.createObjectEntry((YAC_String*)_s, 1, _v, 0) != 0) : 0;
}

void YAC_HashTable::_get(YAC_Object *_s, YAC_Value *_r) {
   TKS_CachedObject *co = NULL;
   if(YAC_BCHK(_s, YAC_CLID_STRING))
   {
      YAC_String *s = (YAC_String*)_s;
      co = varcache.findEntry(s);
   }
   if(co)
   {
      _r->initValue(co); 
   }
   else
   {
      _r->initVoid(); 
   }
}

void YAC_VCALL YAC_HashTable::yacHashGetDeref(void *_context, YAC_String *_key, YAC_Value *_r) {
   (void)_context;

   TKS_CachedObject *co = varcache.findEntry(_key); 
   if(co)
   { 
      _r->fastDerefValue(co); 
   } 
   else
   {
      _r->initVoid();
   }
}
 
void YAC_HashTable::_getDeref(YAC_Object *_s, YAC_Value *_r) { 
   if(YAC_BCHK(_s, YAC_CLID_STRING)) 
   { 
      yacHashGetDeref(NULL, (YAC_String*)_s, _r); /// xxx TKS_MT: should use *real* thread context (exceptions)
      return;
   } 
   else
   {
      _r->initVoid(); 
   }
} 

sSI YAC_HashTable::_exists(YAC_String *_k) {
   return (NULL != varcache.findEntry(_k));
}

void YAC_HashTable::_delete(YAC_String *_k) {
   varcache.removeEntry(_k);
}

void YAC_HashTable::_free(void) {
   varcache.freeCache();
}

sSI YAC_HashTable::_getNumElements(void) {
   return varcache.num_entries;
}

sSI YAC_HashTable::_getMaxElements(void) {
   return varcache.max_entries;
}

void YAC_VCALL YAC_HashTable::yacHashSet(void *_context, YAC_String *_key, YAC_Value *_value) { 
   (void)_context;

   TKS_CachedObject *co = varcache.createEntry(_key, 1/*bCopyName*/);
   if(co)  
   { 
      co->initScriptValCO(_value); 
   } 
}

void YAC_VCALL YAC_HashTable::yacHashGet(void *_context, YAC_String *_key, YAC_Value *_ret) {
   (void)_context;

   _ret->initValue(varcache.findEntry(_key));
} 
 
void YAC_VCALL YAC_HashTable::yacArraySet(void *_context, sUI _index, YAC_Value *_value) { 
   YAC_String s;
   char buf[12];
   Dyac_snprintf(buf, 12, "%d", (sSI)_index);
   s.visit(buf);
   ////s.yacValueOfI((sSI)_index);
   yacHashSet(_context, &s, _value);
}

void YAC_VCALL YAC_HashTable::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) { 
   YAC_String s; 
   char buf[12];
   Dyac_snprintf(buf, 12, "%d", (sSI)_index);
   s.visit(buf);
   ////s.yacValueOfI((sSI)_index); 
   yacHashGet(_context, &s, _r); 
} 

void YAC_VCALL YAC_HashTable::yacArrayGetDeref(void *_context, sUI _index, YAC_Value *_r) { 
   YAC_String s; 
   s.yacValueOfI((sSI)_index); 
   yacHashGetDeref(_context, &s, _r); 
} 


sBool YAC_VCALL YAC_HashTable::yacArrayAlloc(sUI _num, sUI,sUI,sUI) {
   varcache.freeCache();
   return varcache.allocCache(_num);
} 
 
void YAC_HashTable::_set(YAC_Object *_key, YAC_Object *_value) { 
   if(YAC_Is_String(_key)) 
   { 
      if(YAC_BCHK(_value, YAC_CLID_VALUE)) 
      { 
         yacHashSet(NULL, (YAC_String *)_key, (YAC_ValueObject*)_value);  /// xxx TKS_MT: should use *real* thread context (exceptions)
      } 
      else 
      { 
         Dprintf("[---] YAC_HashTable::set: value is not a ValueObject.\n"); 
      } 
   } 
   else 
   { 
      Dprintf("[---] YAC_HashTable::set: key is not a String.\n"); 
   } 
}
 
void YAC_HashTable::_setInt(YAC_Object *_key, sSI _i) { 
   if(YAC_Is_String(_key)) 
   { 
      YAC_Value v; v.initInt(_i); 
      yacHashSet(NULL, (YAC_String*)_key, &v); // xxx TKS_MT: should use *real* thread context (exceptions)
   } 
   else 
   { 
      Dprintf("[---] YAC_HashTable::setInt: key is not a String.\n"); 
   } 
} 
 
void YAC_HashTable::_setFloat(YAC_Object *_key, sF32 _f) { 
   if(YAC_Is_String(_key)) 
   { 
      YAC_Value v; v.initFloat(_f); 
      yacHashSet(NULL, (YAC_String*)_key, &v); // xxx TKS_MT: should use *real* thread context (exceptions)
   } 
   else 
   { 
      Dprintf("[---] YAC_HashTable::setFloat: key is not a String.\n"); 
   } 
} 
 
void YAC_HashTable::_setObject(YAC_Object *_key, YAC_Object *_value) { 
   if(YAC_Is_String(_key)) 
   { 
      YAC_Value v; v.initObject(_value, 0 /* reference only */ ); 
      yacHashSet(NULL, (YAC_String*)_key, &v); // xxx TKS_MT: should use *real* thread context (exceptions)
   } 
   else 
   { 
      Dprintf("[---] YAC_HashTable::setObject: key is not a String.\n"); 
   } 
} 
 
void YAC_HashTable::_setCopy(YAC_Object *_key, YAC_Object *_orig) { 
   YAC_Value v; 
   if(YAC_VALID(_orig)) 
   { 
      YAC_Object *no = YAC_CLONE_POOLED(NULL, _orig);
      no->yacOperatorAssign(_orig); 
      v.initObject(no, 1); 
      v.fixStringType(); // xxx can skip this?
   } 
   else 
   { 
      v.initNull(); 
   } 
   yacHashSet(NULL, (YAC_String*)_key, &v); // xxx TKS_MT: should use *real* thread context (exceptions)
} 
 
void YAC_HashTable::_setString(YAC_Object *_key, YAC_Object *_s) { 
   if(YAC_Is_String(_key)) 
   { 
      if(YAC_BCHK(_s, YAC_CLID_STRING)) 
      { 
         YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
         s->yacCopy(_s); 
         YAC_Value v; v.initString(s, 1); 
         yacHashSet(NULL, (YAC_String*)_key, &v); // xxx TKS_MT: should use *real* thread context (exceptions)
      } 
      else 
      { 
         Dprintf("[---] YAC_HashTable::set: value is not a String.\n"); 
      } 
   } 
   else 
   { 
      Dprintf("[---] YAC_HashTable::setObject: key is not a String.\n"); 
   } 
} 
 
sSI YAC_HashTable::_keysToArray(YAC_Object *_array) { 
   if(YAC_VALID(_array)) 
   { 
      sUI l = varcache.num_entries; 
      if(_array->yacArrayAlloc(l, 0,0,0)) 
      { 
         sUI i=0, j=0; 
         while(i<varcache.max_entries) 
         { 
            TKS_CachedObject *co=varcache.objects[i++]; 
            if(co) 
            { 
               YAC_Value v; v.initString(&co->name, 0); 
               _array->yacArraySet(NULL, j++, &v); // xxx TKS_MT: should use *real* thread context (exceptions)
            } 
         } 
         return (sSI)l; 
      } 
      else 
      { 
         Dprintf("[---] YAC_HashTable::keysToArray: failed to realloc index array.\n"); 
      } 
   } 
   return 0; 
} 
 
sSI YAC_HashTable::_valuesToArray(YAC_Object *_array) { 
   if(YAC_VALID(_array)) 
   { 
      sUI l = varcache.num_entries; 
      if(_array->yacArrayAlloc(l, 0,0,0)) 
      { 
         sUI i=0, j=0; 
         while(i<varcache.max_entries) 
         { 
            TKS_CachedObject *co=varcache.objects[i++]; 
            if(co) 
            { 
               _array->yacArraySet(NULL, j++, co); // xxx TKS_MT: should use *real* thread context (exceptions)
            } 
         } 
         return (sSI)l; 
      } 
      else 
      { 
         Dprintf("[---] YAC_HashTable::valuesToArray: failed to realloc index array.\n"); 
      } 
   } 
   return 0; 
} 

sBool YAC_HashTable::_replaceKey(YAC_Object *_oldKey, YAC_Object *_newKey) {
   if(YAC_Is_String(_oldKey))
   {
      if(YAC_Is_String(_newKey))
      {
         YAC_CAST_ARG(YAC_String, oldKey, _oldKey);
         YAC_CAST_ARG(YAC_String, newKey, _newKey);

         if(_exists(oldKey))
         {
            YAC_Value r;
            yacHashGetDeref(NULL/*context*/, oldKey, &r);
            _delete(oldKey);
            yacHashSet(NULL/*context*/, newKey, &r);

            return YAC_TRUE;
         }
      }
   }
   return YAC_FALSE;
}
