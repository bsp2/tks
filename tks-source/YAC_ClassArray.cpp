/// YAC_ClassArray.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_compiler.h"
#include "YAC_ValueObject.h"

#include "YAC_ClassArray.h"
#include "TKS_ScriptClassArrayIterator.h"


YAC_ClassArray::YAC_ClassArray(void) {
   class_ID     = YAC_CLID_CLASSARRAY;
   class_decl   = NULL;
   max_elements = 0u;
   num_elements = 0u;
   elements     = NULL;
}

YAC_ClassArray::YAC_ClassArray(TKS_ClassDecl *_cd) {
   class_ID     = YAC_CLID_CLASSARRAY;
   class_decl   = _cd;
   max_elements = 0u;
   num_elements = 0u;
   elements     = NULL;
}

YAC_ClassArray::~YAC_ClassArray() {
   class_decl = NULL;
   YAC_ClassArray::free();
}

YAC_Object *YAC_ClassArray::getTemplate(void) {
   return class_decl->class_template;
}

void YAC_ClassArray::yacArraySetTemplate(YAC_Object *_templateObject) {
   setTemplate(_templateObject);
}


void YAC_ClassArray::setTemplate(YAC_Object *_sci) {
   if(YAC_CHK(_sci, YAC_CLID_CLASS))
   {
      TKS_ScriptClassInstance *sci=(TKS_ScriptClassInstance *)_sci;

      YAC_ClassArray::free();

      class_decl = sci->class_decl;
   }
   else
   {
      Dprintf("[---] ClassArray::setTemplate: argument is not a script class object.\n");
   }
}

sBool YAC_VCALL YAC_ClassArray::yacIteratorInit(YAC_Iterator *_it) const {
   Diteratorinit(TKS_ScriptClassArrayIterator);
   return 1;
}

void YAC_VCALL YAC_ClassArray::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();


   /// ---- write namespace name ----
   class_decl->nspace->name.yacSerialize(_ofs, 0);

   /// ---- write template class name ----
   class_decl->name.yacSerialize(_ofs, 0);

   /// ---- write number of elements ----
   YAC_SERIALIZE_I32(num_elements);

   if(num_elements)
   {
      sUI i;
      for(i=0; i<num_elements; i++)
      {
         if(elements[i])
         {
            // ---- mark slot used and write single object ----
            YAC_SERIALIZE_I8(1);
            elements[i]->yacSerialize(_ofs, 0);
         }
         else
         {
            // ---- mark slot unused ----
            YAC_SERIALIZE_I8(0);
         }
      }
   }

}

sUI YAC_VCALL YAC_ClassArray::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();

   YAC_ClassArray::free();

   // Read namespace name
   YAC_String nspaceName;
   nspaceName.yacDeserialize(_ifs, 0);
   TKS_Namespace *nspace;
   if(nspaceName.isBlank())
   {
      nspace = NULL; // use default namespace
   }
   else
   {
      nspace = tkscript->findNamespace(&nspaceName);
      if(NULL == nspace)
      {
         Dprintf("[---] ClassArray::yacDeserialize: namespace \"%s\" does not exist.\n",
            (char*)nspaceName.chars
            );
         // Failed
         return 0;
      }
   }

   YAC_String cln; /// ---- read class name and find suitable class declaration object ----
   cln.yacDeserialize(_ifs, 0);
   TKS_ClassDecl *cd = Dtkscompiler->findClassDeclNamespace(&cln, nspace);
   if(!cd)
   {
      Dprintf("[---] ClassArray::deserialize: cannot find template for class \"%s\".\n",
         (char*)cln.chars);
      return 0;
   }
   class_decl=cd;

   sUI ne=YAC_DESERIALIZE_I32();
   if(((sUI)ne)<tkscript->configuration.streamMaxArraySize)
   {
      if(YAC_ClassArray::alloc(ne)) // create new elements according to class_decl->class_template
      {
         num_elements=ne;
         sUI i;
         for(i=0; i<ne; i++)
         {
            if(YAC_DESERIALIZE_I8()) // bool used
            {
               elements[i]->yacDeserialize(_ifs, 0);
            }
         }
         return 1;
      }
   }
   else
   {
      Dprintf("[---] ClassArray::deserialize: insane array size (%i>%i)\n",
         ne, tkscript->configuration.streamMaxArraySize);
   }
   return 0;
}

void YAC_VCALL YAC_ClassArray::yacArrayCopySize(YAC_Object *_array) {
   YAC_ClassArray *o=(YAC_ClassArray*)_array;
   YAC_ClassArray::alloc(o->max_elements);
}


sSI YAC_ClassArray::_add(YAC_Object *_value) {
   if(YAC_CHK(_value, YAC_CLID_CLASS))
   {
      // ---- wrap (read-only) ScriptObject in YAC_Value
      YAC_Value v; v.initObject(_value, 0);
      yacArraySet(NULL, num_elements, &v); // xxx TKS_MT: should use real thread context (exceptions)
      return 1;
   }
   else
   {
      if(YAC_CHK(_value, YAC_CLID_VALUE))
      {
         yacArraySet(NULL, num_elements, (YAC_ValueObject*)_value); /// xxx TKS_MT: should use real thread context (exceptions)
         return 1;
      }
   }
   return 0;
}

sSI YAC_ClassArray::_remove(YAC_Object *_o) {
   sUI i;
   for(i=0; i<num_elements; i++)
   {
      if((void*)elements[i] == (void*)_o)
      {
         return _delete((sSI)i);
      }
   }
   return 0;
}

sSI YAC_ClassArray::_insert(sSI _index, YAC_Object *_val) {
   sUI index = (sUI) _index;
   if(index >= num_elements)
   {
      if(index == num_elements)
      {
         return _add(_val);
      }
      else
      {
         Dprintf("[---] ClassArray::insert: Cannot insert beyond array boundaries (index=%i, numElements=%i).\n", index, num_elements);
         return 0;
      }
   }
   // Check if current array can hold new element
   if(num_elements == max_elements)
   {
      if(! YAC_ClassArray::realloc(tks_grow_array(num_elements+1)) )
      {
         // re-allocation failed
         return 0;
      }
   }
   // Shift elements
   if(elements[num_elements]!=NULL)
   {
      YAC_DELETE(elements[num_elements]);
      elements[num_elements] = NULL;
   }
   for(sUI i=num_elements; i>index; i--)
   {
      elements[i] = elements[i-1];
   }

   num_elements++;
   elements[index] = NULL;

   if(YAC_CHK(_val, YAC_CLID_CLASS))
   {
      TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance*) _val;
      if(sci->class_decl->ancestor_table[class_decl->class_id])
      {
         // ---- create new object and copy _value ----
         TKS_Context *ctx = tkscript->lockGlobalContext(); // xxx TKS_MT: use real script thread
         elements[_index] = (TKS_ScriptClassInstance*) YAC_CLONE_POOLED(ctx, class_decl->class_template);
         tkscript->unlockGlobalContext();
         if(elements[_index])
         {
            elements[_index]->yacOperatorAssign(_val);
         }
         return 1;
      }
      else
      {
         Dprintf("[---] ClassArray::insert: class type mismatch (index=%i).\n", index);
         return 0;
      }
   }
   else if(YAC_CHK(_val, YAC_CLID_VALUE))
   {
      YAC_ValueObject *vo = (YAC_ValueObject*) _val;
      if(vo->type >=YAC_TYPE_OBJECT)
      {
         YAC_Object *o = vo->value.object_val;
         if(YAC_CHK(o, YAC_CLID_CLASS))
         {
            TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance*) o;
            if(sci->class_decl->ancestor_table[class_decl->class_id])
            {
               if(vo->deleteme)
               {
                  elements[index] = sci;
                  vo->deleteme = 0;
               }
               else
               {
                  // ---- create new object and copy _value ----
                  /*TKS_Context *ctx = */tkscript->lockGlobalContext(); // xxx TKS_MT: use real script thread
                  elements[_index] = (TKS_ScriptClassInstance*) YAC_CLONE_POOLED(NULL, class_decl->class_template); // xxx TKS_MT init follows (NULL ok)
                  tkscript->unlockGlobalContext();
                  if(elements[_index])
                  {
                     elements[_index]->yacOperatorAssign(o);
                  }
               }
               return 1;
            }
            else
            {
               Dprintf("[---] ClassArray::insert: class type mismatch (index=%i).\n", index);
               return 0;
            }
         }
         else
         {
            Dprintf("[---] ClassArray::insert: object type mismatch (index=%i).\n", index);
            return 0;
         }
      }
      else
      {
         Dprintf("[---] ClassArray::insert: value type mismatch (index=%i).\n", index);
         return 0;
      }
   }
   else
   {
      Dprintf("[---] ClassArray::insert: object type mismatch (index=%i).\n", index);
      return 0;
   }
}

#if 0
sSI YAC_ClassArray::_delete(sSI _index) {
   sUI index = (sUI) _index;
   if(index < num_elements)
   {
      if(num_elements > 1)
      {
         TKS_ScriptClassInstance **ne=new TKS_ScriptClassInstance*[max_elements - 1];
         if(NULL != ne)
         {
            if(NULL != elements[index]) // should always be allocated ..!
            {
               YAC_DELETE(elements[index]);
               elements[index] = NULL;
            }
            sUI i=0;
            // shrink
            for(i=0; i<index; i++)
            {
               ne[i] = elements[i]; // copy elements
            }
            i++; // skip <index>
            for(; i<max_elements; i++)
            {
               ne[i-1] = elements[i]; // copy elements
            }
            delete [] elements;
            elements = ne;
            max_elements--;
            num_elements--;
            return 1;
         }
         else
         {
            Dprintf("\n[---] YAC_ClassArray::delete: failed to allocate new element pointers (%i)\n", max_elements-1);
            return 0;
         }
      }
      else
      {
         // Deleting last element
         YAC_ClassArray::free();
         return 1;
      }
   }
   return 0;
}
#endif // 0


sSI YAC_ClassArray::_delete(sSI _index) {
   // Fast delete, leaves NULL "holes" in the array (which are then fixed by realloc() later)
   sUI index = (sUI) _index;
   if(index<num_elements)
   {
      if(num_elements>1)
      {
         if(elements[index])
         {
            YAC_DELETE(elements[index]);
            elements[index] = NULL;
         }
         for(sUI i=index; i<(num_elements-1); i++)
         {
            elements[i]=elements[i+1];
         }
         elements[num_elements-1] = NULL;
      }
      num_elements--;
      return 1;
   }
   return 0;
}


void YAC_VCALL YAC_ClassArray::yacArraySet(void *_context, sUI _index, YAC_Value *_value) {
   if(((sUI)_index)>=max_elements)
   {
      if(tkscript->configuration.b_autoresizearrays && (((sSI)_index) >= 0))
      {
         if(YAC_ClassArray::realloc(tks_grow_array(_index+1)))
         {
            num_elements=_index+1;
            goto YAC_ClassArray_yacarrayset_1;
         }
      }

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("ClassArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS, msg);
      return;
   }
   if(((sUI)_index)>=num_elements) // track highest access point
   {
      num_elements=_index+1;
   }
YAC_ClassArray_yacarrayset_1:
   if(_value->type >= YAC_TYPE_OBJECT)
   {
      if(_value->value.object_val->class_ID!=YAC_CLID_CLASS)
      {
         Dprintf("[---] YAC_ClassArray::yacArraySet: object type mismatch. (index=%i, need=\"%s::%s\" have=\"%s\".\n",
            _index,
            (char*)class_decl->nspace->name.chars,
            (char*)class_decl->name.chars,
            (char*)_value->value.object_val->yacClassName()
            );

         YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
         msg->alloc(512);
         msg->printf("[---] YAC_ClassArray::yacArraySet: object type mismatch. (index=%i", _index);
         msg->append(", need=\"");
         msg->append(&class_decl->nspace->name);
         msg->append("::");
         msg->append(&class_decl->name);
         msg->append("\" have=\"");
         msg->append((char*)_value->value.object_val->yacClassName());
         msg->append("\".\n");
         /*msg->printf("[---] YAC_ClassArray::yacArraySet: object type mismatch. (index=%i, need=\"%s::%s\" have=\"%s\".\n",
            _index,
            (char*)class_decl->nspace->name.chars,
            (char*)class_decl->name.chars,
            (char*)_value->value.object_val->yacClassName()
            );*/
         PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
         PTN_Env*_env = &env;
         Drtthrow(NULL, TKS_EXCEPTION_NATIVECLASSTYPEMISMATCH, msg);
         return;
      }
      TKS_ScriptClassInstance *cco=(TKS_ScriptClassInstance*)_value->value.object_val;
      ////if(cco->class_decl->class_id != class_decl->class_id)
      if(!cco->class_decl->ancestor_table[class_decl->class_id]) // Same or derived class ?
      {
         Dprintf("[---] YAC_ClassArray::yacArraySet: script class type mismatch. (index=%i, need=\"%s::%s\" have=\"%s::%s\".\n",
            _index,
            (char*)class_decl->nspace->name.chars,
            (char*)class_decl->name.chars,
            (char*)cco->class_decl->nspace->name.chars,
            (char*)cco->class_decl->name.chars
            );

         YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
         msg->alloc(512);
         msg->printf("[---] YAC_ClassArray::yacArraySet: script class type mismatch. (index=%i", _index);
         msg->append(", need=\"");
         msg->append(&class_decl->nspace->name);
         msg->append("::");
         msg->append(&class_decl->name);
         msg->append("\" have=\"");
         msg->append(&cco->class_decl->nspace->name);
         msg->append("::");
         msg->append(&cco->class_decl->name);
         msg->append("\".\n");
         /*msg->printf("YAC_ClassArray::yacArraySet: script class type mismatch. (index=%i, need=\"%s\" have=\"%s\".\n",
            _index, (char*)class_decl->name.chars, (char*)cco->class_decl->name.chars);*/
         PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
         PTN_Env*_env = &env;
         Drtthrow(NULL, TKS_EXCEPTION_SCRIPTCLASSTYPEMISMATCH, msg);
         return;
      }

      if(_value->deleteme)
      {
         // reference _value (pointer object)
         _value->deleteme=0;
         if(elements[_index])
         {
            YAC_DELETE(elements[_index]);
         }
         elements[_index]=cco; //_value->value.object_val;
      }
      else
      {
         if(elements[_index])
         {
            // ---- copy _value to existing object ----
            elements[_index]->yacOperatorAssign(_value->value.object_val);
         }
         else
         {
            if(class_decl)
            {
               // ---- create new object and copy _value ----
               TKS_Context *ctx = tkscript->lockGlobalContext(); // xxx TKS_MT: use real thread context
               elements[_index] = (TKS_ScriptClassInstance*) YAC_CLONE_POOLED(ctx, class_decl->class_template);
               tkscript->unlockGlobalContext();
               if(elements[_index])
               {
                  elements[_index]->yacOperatorAssign(_value->value.object_val);
               }
            }
            else
            {
               // must not reference, no object to store _value
            }
         }
      }
   }
}

void YAC_VCALL YAC_ClassArray::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   if(((sUI)_index) < max_elements)
   {
      _r->initObject(elements[_index], 0); // return read-only
   }
   else
   {
      _r->initVoid();

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("ClassArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

void YAC_VCALL YAC_ClassArray::yacArrayGetDeref(void *_context, sUI _index, YAC_Value *_r) {
   if(((sUI)_index)<max_elements)
   {
      _r->initObject(elements[_index], (elements[_index]!=0)?1:0 ); // return value
      elements[_index] = 0; // remove element from array
   }
   else
   {
      _r->initVoid();

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("ClassArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

YAC_Object *YAC_ClassArray::get(sSI _index) {
   if( ((sUI)_index) < max_elements )
   {
      return elements[_index];
   }
   else
   {
      return yac_null;
   }
}

void YAC_ClassArray::getDeref(sSI _index, YAC_Value *_r) {
   if( ((sUI)_index)<max_elements )
   {
      _r->initObject(elements[_index], (elements[_index]!=0)?1:0 ); // return value
      elements[_index] = 0; // remove element from array
   }
   else
   {
      _r->initNull();
   }
}

void YAC_ClassArray::unlink(YAC_Object *_o, YAC_Value *_r) {
   sSI idx = indexOfPointer(_o, 0);
   if(-1 != idx)
   {
      getDeref(idx, _r);
      _delete(idx);
   }
   else
   {
      _r->initNull();
   }
}

sBool YAC_VCALL YAC_ClassArray::yacArrayAlloc(sUI _maxelements, sUI,sUI,sUI) {
   return YAC_ClassArray::alloc(_maxelements);
}

void YAC_VCALL YAC_ClassArray::yacArraySetWidth(sUI _num) {
   if(_num <= max_elements)
   {
      num_elements = _num;
   }
}

sBool YAC_ClassArray::alloc(sUI _maxElements) {
   YAC_ClassArray::free();
   if(_maxElements > 0u)
   {
      if(_maxElements > TKS_MAX_ARRAYALLOCSIZE)
      {
         tkscript->printf("[---] ClassArray::insane array size (maxelements=%08x)\n", _maxElements);
         return YAC_FALSE;
      }
      elements = new TKS_ScriptClassInstance*[_maxElements];
      if(elements)
      {
         max_elements = _maxElements;
         num_elements = 0u;

         sUI i = 0u;
         if(class_decl) /// xxx always set?
         {
            if(class_decl->class_template)
            {
               TKS_Context *ctx = tkscript->lockGlobalContext();
               // ---- create new objects based on template type ----
               for(; i < _maxElements; i++)
               {
                  elements[i] = (TKS_ScriptClassInstance*) YAC_CLONE_POOLED(ctx, class_decl->class_template);  // xxx TKS_MT: use *real* thread context (no c'tors otherwise)
               }
               tkscript->unlockGlobalContext();
            }
            else
            {
               for(; i < _maxElements; i++)
               {
                  elements[i] = NULL;
               }
            }
         }
         else
         {
            // reference/pointer array
            for(; i < _maxElements; i++)
            {
               elements[i] = NULL;
            }
         }
         return YAC_TRUE;
      }
      else
      {
         Dprintf("[---] ClassArray: failed to allocate %u elements\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // alloc(0) -> free()
      return YAC_TRUE;
   }
}

void YAC_ClassArray::free(void) {
   if(elements)
   {
      sUI i;
      for(i = 0u; i < max_elements; i++)
      {
         if(elements[i])
         {
            YAC_DELETE(elements[i]);
            elements[i] = NULL;
         }
      }
      delete [] elements;
      elements = NULL;
      num_elements = 0u;
      max_elements = 0u;
   }
}

sBool YAC_ClassArray::realloc(sUI _maxElements) {
   if(_maxElements > 0u)
   {
      TKS_ScriptClassInstance **ne = new TKS_ScriptClassInstance*[_maxElements];
      if(ne)
      {
         sUI i = 0u;
         if(_maxElements <= max_elements)
         {
            // shrink
            for(; i < _maxElements; i++)
            {
               ne[i] = elements[i]; // copy elements
               if(NULL == ne[i])
               {
                  // Preceeding delete() calls might have left "holes" in the array (at the end)
                  if(class_decl)
                  {
                     TKS_Context *ctx = tkscript->lockGlobalContext();
                     ne[i] = (TKS_ScriptClassInstance*) YAC_CLONE_POOLED(ctx, class_decl->class_template); // xxx TKS_MT: use *real* thread context (no c'tors otherwise)
                     tkscript->unlockGlobalContext();
                  }
               }
            }
            for(; i < max_elements; i++)
            {
               YAC_DELETE(elements[i]); // delete superfluous elements
               elements[i] = NULL;
            }
         }
         else
         {
            // expand
            for(; i < max_elements; i++)
            {
               ne[i] = elements[i]; // copy existing region
               if(NULL == ne[i])
               {
                  // Preceeding delete() calls might have left "holes" in the array (at the end)
                  if(class_decl)
                  {
                     TKS_Context *ctx = tkscript->lockGlobalContext();
                     ne[i] = (TKS_ScriptClassInstance*) YAC_CLONE_POOLED(ctx, class_decl->class_template); // xxx TKS_MT: use *real* thread context (no c'tors otherwise)
                     tkscript->unlockGlobalContext();
                  }
               }
            }
            if(class_decl)
            {
               TKS_Context *ctx = tkscript->lockGlobalContext();
               for(; i < _maxElements; i++) // init new region
               {
                  ne[i] = (TKS_ScriptClassInstance*) YAC_CLONE_POOLED(ctx, class_decl->class_template); // xxx TKS_MT: use *real* thread context (no c'tors otherwise)
               }
               tkscript->unlockGlobalContext();
            }
            else
            {
               for(; i < _maxElements; i++) // init new region
               {
                  ne[i] = NULL;
               }
            }
         }
         if(NULL != elements)
         {
            delete [] elements;
         }
         elements     =  ne;
         max_elements = _maxElements;
         if(num_elements > max_elements)
         {
            num_elements = max_elements;
         }
         return YAC_TRUE;
      }
      else
      {
         Dprintf("\n[---] failed to realloc ClassArray to %u\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // realloc(0) -> free()
      YAC_ClassArray::free();
      return YAC_TRUE;
   }
}

void YAC_VCALL YAC_ClassArray::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o!=((YAC_Object*)this))
   {
      if(YAC_OP_ASSIGN == _cmd)
      {
         if(YAC_CHK(_o, YAC_CLID_CLASSARRAY))
         {
            YAC_ClassArray *o=(YAC_ClassArray *)_o;
            if((NULL == class_decl) || (NULL == o->class_decl))
            {
               // can happen when initializing a script class that has a ClassArray member which has not been assigned a class template, yet
               return;
            }
            if(o->class_decl->class_id != class_decl->class_id)
            {
               return; // class type mismatch
            }
            YAC_ClassArray::realloc(o->num_elements);
            sUI i;
            num_elements=o->num_elements;
            for(i=0; i<num_elements; i++)
            {
               if(o->elements[i])
               {
                  if(!elements[i])
                  {
                     elements[i]=(TKS_ScriptClassInstance*) YAC_CLONE_POOLED(NULL, class_decl->class_template);
                  }
                  if(elements[i])
                  {
                     elements[i]->yacOperatorAssign(o->elements[i]);
                  }
               }
               else
               {
                  if(elements[i])
                  {
                     YAC_DELETE(elements[i]);
                     elements[i]=0;
                  }
               }
            }
            return;
         }
         else if(YAC_VALID(_o))
         {
            sUI num=_o->yacArrayGetNumElements();
            YAC_ClassArray::realloc(num);
            num_elements=num;

            sUI i;
            YAC_Value ev;
            for(i=0; i<num_elements; i++)
            {
               _o->yacArrayGet(NULL, i, &ev); // xxx TKS_MT: use *real* thread context (exceptions!)
               sBool b_ok=0;
               if(ev.type>=YAC_TYPE_OBJECT)
               {
                  if(ev.value.object_val)
                  {
                     if(!elements[i]) // xxx is there ever a null pointer?
                     {
                        elements[i]=(TKS_ScriptClassInstance*) YAC_CLONE_POOLED(NULL, class_decl->class_template);
                     }
                     if(elements[i])
                     {
                        elements[i]->yacOperatorAssign(ev.value.object_val);
                     }
                     b_ok=1;
                  }
               }
               if(!b_ok)
               {
                  if(elements[i])
                  {
                     YAC_DELETE(elements[i]);
                     elements[i]=0;
                  }
               }
            }
            return;
         }
      }
   }
   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_ClassArray::yacOperatorInit(void *_context, YAC_Object *_o) {
   (void)_context;

   if(YAC_CHK(_o, YAC_CLID_CLASSARRAY))
   {
      YAC_ClassArray *o=(YAC_ClassArray *)_o;
      class_decl=o->class_decl;
   }
}

sUI YAC_ClassArray::getMaxElements(void) {
   return max_elements;
}

sUI YAC_ClassArray::getNumElements(void) {
   return num_elements;
}

void YAC_ClassArray::empty(void) {
   num_elements = 0u;
}

sBool YAC_ClassArray::isEmpty(void) {
   return (0u == num_elements);
}

void YAC_ClassArray::reverse(void) {
   if(num_elements&&elements)
   {
      TKS_ScriptClassInstance**s=elements;
      sSI l=(num_elements)>>1;
      TKS_ScriptClassInstance**d=&elements[num_elements-1];
      for(; l>=0; l--)
      {
         TKS_ScriptClassInstance*t;
         t=*s;
         *s++=*d;
         *d--=t;
      }
   }
}

YAC_Object *YAC_ClassArray::getNextFree(void) {
   if(num_elements!=max_elements)
   {
      return elements[num_elements++];
   }
   else
   {
      return 0;
   }
}

void YAC_ClassArray::setNumElements(sUI _ne) {
   if(_ne <= max_elements)
   {
      num_elements=_ne;
   }
}

sSI YAC_ClassArray::contains(YAC_Object *_o) {
   sUI i;
   for(i=0; i<num_elements; i++)
   {
      if(elements[i]->yacEquals(_o))
      {
         return 1;
      }
   }
   return 0;
}

sSI YAC_ClassArray::_swap(sSI _indexs, sSI _indexd) {
   sUI idxs = (sUI) _indexs;
   sUI idxd = (sUI) _indexd;
   if(idxs!=idxd)
   {
      if(idxs<num_elements)
      {
         if(idxd<num_elements)
         {
            TKS_ScriptClassInstance *t;
            t = elements[idxd];
            elements[idxd] = elements[idxs];
            elements[idxs] = t;
            return 1;
         }
      }
   }
   return 0;
}

sSI YAC_ClassArray::indexOfPointer(YAC_Object*_o, sSI _off) {
   sUI i = (sUI) _off;
   for(; i<num_elements; i++)
   {
      if( ((void*)elements[i]) == ((void*)_o) )
      {
         // Ok, found object address
         return (sSI) i;
      }
   }
   // Failed, address not found
   return -1;
}

sSI YAC_ClassArray::indexOfObject(YAC_Object*_o, sSI _off) {
   sUI i = (sUI) _off;
   for(; i<num_elements; i++)
   {
      if(YAC_VALID(elements[i]))
      {
         if(elements[i]->yacEquals(_o))
         {
            return i;
         }
      }
      else
      {
         if( ! (YAC_VALID(_o)))
         {
            return i;
         }
      }
   }
   return -1;
}

sSI YAC_ClassArray::containsObject(YAC_Object *_o) {
   return (-1 != indexOfObject(_o, 0));
}

sSI YAC_ClassArray::containsPointer(YAC_Object *_o) {
   return (-1 != indexOfPointer(_o, 0));
}

void YAC_ClassArray::_useAll(void) {
   num_elements = max_elements;
}

YAC_Object *YAC_ClassArray::_getFirst(void) {
   if(num_elements > 0)
   {
      return elements[0];
   }
   else
   {
      return NULL;
   }
}

YAC_Object *YAC_ClassArray::_getLast(void) {
   if(num_elements > 0)
   {
      return elements[num_elements - 1];
   }
   else
   {
      return NULL;
   }
}
