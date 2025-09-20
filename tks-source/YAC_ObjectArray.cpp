/// YAC_ObjectArray.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "PTN_Node.h"
#include "YAC_ValueObject.h"

#include "YAC_ObjectArray.h"
#include "TKS_ObjectArrayIterator.h"


YAC_ObjectArray::YAC_ObjectArray(void) {
   class_ID        = YAC_CLID_OBJECTARRAY;
   template_object = yac_null;
   max_elements    = 0u;
   num_elements    = 0u;
   elements        = NULL;
}

YAC_ObjectArray::~YAC_ObjectArray() {
   YAC_ObjectArray::free();
   template_object = NULL;
}

sBool YAC_VCALL YAC_ObjectArray::yacToString(YAC_String *_s) const {
   _s->alloc(2+num_elements*32+1);
   _s->empty();
   _s->append("[");
   sUI i;
   YAC_String s;
   if(num_elements)
   {
      if(elements[0] != NULL)
      {
         elements[0]->yacToString(&s);
         _s->append(&s);
      }
      else
      {
         _s->append("<null>");
      }
   }
   for(i=1; i<num_elements; i++)
   {
      if(elements[i] != NULL)
      {
         _s->append(", ");
         elements[i]->yacToString(&s);
         _s->append(&s);
      }
      else
      {
         _s->append(", <null>");
      }
   }
   _s->append("]");
   return 1;
}

void YAC_ObjectArray::_getString(YAC_Value *_r) const {
   _r->initEmptyString();
   yacToString(_r->value.string_val);
}

void YAC_VCALL YAC_ObjectArray::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   
   /// ---- first write template class name ----
   if(template_object)
   {
      /// ---- TKS classes start with '@' ----
      template_object->yacSerializeClassName(_ofs);
   }
   else
   {
      YAC_String tcln; 
      //tcln.setEngine(tkscript);
      tcln.yacSerialize(_ofs, 0); // serialize empty string		
      return;
   }
   
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

sUI YAC_VCALL YAC_ObjectArray::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   
   /// ---- first read class name and find suitable template object ----
   YAC_String cln;
   //cln.setEngine(tkscript);
   cln.yacDeserialize(_ifs, 0);
   if(cln.isBlank())
      return 1; // ---- error in serialization, no template was set ----
   template_object=tkscript->findTemplateObjectByUniqueClassName(&cln);
   if(!template_object)
   {
      Dprintf("[---] ObjectArray::deserialize: cannot find template for class \"%s\".\n",
         cln.chars);
      return 0; // ---- cannot find class ----
   }
   
   sUI ne = YAC_DESERIALIZE_I32();

   if(((sUI)ne) < tkscript->configuration.streamMaxArraySize)
   {
      if(YAC_ObjectArray::alloc(ne)) // create new elements according to template_object
      {
         sUI i; 
         num_elements=ne;
         for(i=0; i<ne; i++)
         {
            sU8 used=YAC_DESERIALIZE_I8();
            if(used)
            {
               // elements[i] must be !=null after alloc()
               elements[i]->yacDeserialize(_ifs, 0);
            }
         }
         return 1;
      }
   }
   else
   {
      free();
      Dprintf("[---] ObjectArray::deserialize: insane array size (%i>%i)\n",
         ne, tkscript->configuration.streamMaxArraySize);
   }
   return 0;
}


sBool YAC_VCALL YAC_ObjectArray::yacIteratorInit(YAC_Iterator *_it) const {
   Diteratorinit(TKS_ObjectArrayIterator);
   return 1;
}

void YAC_VCALL YAC_ObjectArray::yacArrayCopySize(YAC_Object *_array) {
   YAC_ObjectArray *o=(YAC_ObjectArray*)_array;
   YAC_ObjectArray::alloc(o->max_elements);
}


void YAC_VCALL YAC_ObjectArray::yacArraySet(void *_context, sUI _index, YAC_Value *_value) {
   if(((sUI)_index)>=max_elements)
   { 
      if(tkscript->configuration.b_autoresizearrays && (((sSI)_index) >= 0))
      { 
         if(YAC_ObjectArray::realloc(tks_grow_array(_index+1))) 
         { 
            num_elements=_index+1; 
            goto YAC_ObjectArray_yacarrayset_1; 
         } 
      } 

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("ObjectArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS, msg);
      return; 
   } 

   if(((sUI)_index) >= num_elements) // track highest access point 
   {
      num_elements = _index + 1; 
   }

YAC_ObjectArray_yacarrayset_1: 

   YAC_Value value; value = _value; // Grabs deleteme flag from _value

   if(value.type < YAC_TYPE_OBJECT)
   {
      value.typecast(YAC_TYPE_OBJECT);
   }

   if(YAC_VALID(_value->value.object_val))
   {
      if(_value->deleteme)
      {
         // Is the template class a base class of the given _value object ?
         if(tkscript->cpp_typecast_map[_value->value.object_val->class_ID][template_object->class_ID])
         {
            // Then grab reference; don't copy
            _value->deleteme = 0;
            if(elements[_index] != NULL)
            {
               YAC_DELETE(elements[_index]);
            }
            elements[_index] = _value->value.object_val;
         }
         else
         {
            // Assign "any" object value
            if(elements[_index] == NULL)
            {
               elements[_index] = YAC_CLONE_POOLED(NULL, template_object);
            }
            elements[_index]->yacOperatorAssign(_value->value.object_val); 
         }
      }
      else
      {
         // Assign "any" object reference
         if(elements[_index] == NULL)
         {
            elements[_index] = YAC_CLONE_POOLED(NULL, template_object);
         }
         // ---- copy _value to existing object ---- 
         elements[_index]->yacOperatorAssign(_value->value.object_val);
      }
      
   } // if valid _value.object
   else
   {
      if(elements[_index] != NULL)
      {
         YAC_DELETE(elements[_index]);
         elements[_index] = NULL;
      }
   }
   value.unsetFast();
}

sSI YAC_ObjectArray::_add(YAC_Object *_value) { 
   if(YAC_CHK(_value, YAC_CLID_VALUE)) 
   { 
      yacArraySet(NULL, num_elements, (YAC_ValueObject*)_value); // xxx TKS_MT: should use *real* thread context (exceptions)
      return 1; 
   } 
   else 
   { 
      // ---- wrap (read-only) ScriptObject in YAC_Value 
      YAC_Value v; v.initObject(_value, 0); 
      yacArraySet(NULL, num_elements, &v); // xxx TKS_MT: should use *real* thread context (exceptions)
      return 1; 
   } 
} 

sSI YAC_ObjectArray::_remove(YAC_Object *_o) {
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

sSI YAC_ObjectArray::_insert(sSI _index, YAC_Object *_val) {
   sUI index = (sUI) _index;
   if(index >= num_elements)
   {
      if(index == num_elements)
      {
         return _add(_val);
      }
      else
      {
         Dprintf("[---] ObjectArray::insert: Cannot insert beyond array boundaries (index=%i, numElements=%i).\n", index, num_elements);
         return 0;
      }
   }
   // Check if current array can hold new element
   if(num_elements == max_elements)
   {
      if(! YAC_ObjectArray::realloc(tks_grow_array(num_elements+1)) )
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

   if(YAC_CHK(_val, YAC_CLID_VALUE))
   {
      YAC_ValueObject *vo = (YAC_ValueObject*) _val;
      if(vo->type >=YAC_TYPE_OBJECT)
      {
         YAC_Object *o = vo->value.object_val;
         if(tkscript->cpp_typecast_map[o->class_ID][template_object->class_ID])
         {
            if(vo->deleteme)
            {
               elements[index] = o;
               vo->deleteme = 0;
            }
            else
            {
               // ---- create new object and copy _value ----
               elements[_index] = YAC_CLONE_POOLED(NULL, template_object);
               if(elements[_index]) 
               {
                  elements[_index]->yacOperatorAssign(o); 
               }
            }
            return 1;
         }
         else
         {
            Dprintf("[---] ObjectArray::insert: object type mismatch (index=%i).\n", index);
            return 0;
         }
      }
      else
      {
         Dprintf("[---] ObjectArray::insert: value type mismatch (index=%i).\n", index);
         return 0;
      }
   }
   else if(YAC_VALID(_val))
   {
      if(tkscript->cpp_typecast_map[_val->class_ID][template_object->class_ID])
      {
         // ---- create new object and copy _value ----
         elements[_index] = YAC_CLONE_POOLED(NULL, template_object);
         if(elements[_index]) 
         {
            elements[_index]->yacOperatorAssign(_val); 
         }
         return 1;
      }
      else
      {
         Dprintf("[---] ObjectArray::insert: object type mismatch (index=%i).\n", index);
         return 0;
      }
   }
   else
   {
      // Just insert null
      return 1;
   }
}

sSI YAC_ObjectArray::_delete(sSI _index) {
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

void YAC_VCALL YAC_ObjectArray::yacArraySetWidth(sUI _num) {
   if(_num<=max_elements) 
   {
      num_elements=_num; 
   }
}

void YAC_VCALL YAC_ObjectArray::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   if(((sUI)_index) < max_elements)
   {
      _r->initObject(elements[_index], 0); // return read-only
   }
   else
   {
      _r->initVoid(); 

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("ObjectArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}
 
void YAC_VCALL YAC_ObjectArray::yacArrayGetDeref(void *_context, sUI _index, YAC_Value *_r) {
   if(((sUI)_index)<max_elements)
   {
      if(elements[_index])
      {
         _r->initObject(elements[_index], (elements[_index]!=NULL)?1:0 ); // return (deletable) value
         elements[_index] = NULL; // remove element from array
      }
      else
      {
         _r->initNull();
      }
   }
   else
   {
      _r->initVoid(); 

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("ObjectArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

YAC_Object *YAC_ObjectArray::get(sSI _index) { 
   if(((sUI)_index) < max_elements) 
   { 
      return elements[_index]; // return read-only 
   } 
   else 
   { 
      return yac_null; 
   } 
} 

void YAC_ObjectArray::getDeref(sSI _index, YAC_Value *_r) { 
   if(((sUI)_index) < max_elements)
   {
      if(elements[_index])
      {
         _r->initObject(elements[_index], (elements[_index]!=0)?1:0 ); // return (deletable) value
         elements[_index] = 0; // remove element from array
      }
   }
   _r->initNull(); // silently return null in case an error has occured
} 

void YAC_ObjectArray::unlink(YAC_Object *_o, YAC_Value *_r) {
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

sBool YAC_VCALL YAC_ObjectArray::yacArrayAlloc(sUI _maxelements, sUI,sUI,sUI) {
   if(YAC_ObjectArray::alloc(_maxelements))  
   {
      return (elements != NULL); 
   }
   else   
   {
      return YAC_FALSE; 
   }
}

sBool YAC_ObjectArray::yacArrayRealloc(sUI _num, sUI,sUI,sUI) {
   return YAC_ObjectArray::realloc(_num);
}

sBool YAC_ObjectArray::alloc(sUI _maxElements) {
   YAC_ObjectArray::free();
   if(_maxElements > 0u)
   {
      if(_maxElements > TKS_MAX_ARRAYALLOCSIZE)
      {
         tkscript->printf("[---] ObjectArray::insane array size (maxelements=%08x)\n", _maxElements);
         return YAC_FALSE;
      }
      elements = new YAC_Object* [ _maxElements ];
      if(elements)
      {
         max_elements = _maxElements;
         num_elements = 0u;
         sUI i = 0u;
         if(template_object)
         {
            // ---- create new objects based on template type ----
            TKS_Context *ctx = tkscript->lockGlobalContext();
            for(; i < max_elements; i++) 
            {
               elements[i] = YAC_CLONE_POOLED(ctx, template_object); // xxx TKS_MT: use *real* thread context (no c'tors otherwise)
            }
            tkscript->unlockGlobalContext();
         }
         else
         {
            // ---- reference/pointer array ----
            for(; i < max_elements; i++)  
            {
               elements[i] = NULL; 
            }
         }
         return YAC_TRUE;
      }
      else
      {
         Dprintf("\n[---] ObjectArray: failed to allocate %u elements\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // alloc(0) -> free()
      return YAC_TRUE;
   }
}

void YAC_ObjectArray::free(void) {
   sUI i;
   for(i = 0u; i < max_elements; i++)
   {
      if(elements[i] != NULL)
      {
         YAC_DELETE(elements[i]);
         elements[i] = NULL;
      }
   }
   if(elements)
   {
      delete [] elements;
      elements = NULL;
      num_elements = 0u;
      max_elements = 0u;
   }
}

sBool YAC_ObjectArray::realloc(sUI _maxElements) {
   if(_maxElements > 0u)
   {
      YAC_Object **ne = new YAC_Object*[_maxElements];
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
                  // Fix holes left by preceeding delete() calls
                  if(template_object)
                  {
                     ne[i] = YAC_CLONE_POOLED(NULL, template_object); // xxx TKS_MT: use *real* thread context (no c'tors otherwise)
                  }
               }
            }
            for(; i < max_elements; i++)
            {
               if(elements[i] != NULL) // delete superfluous elements
               {
                  YAC_DELETE(elements[i]);
                  elements[i] = NULL;
               }
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
                  // Fix holes left by preceeding delete() calls
                  if(template_object)
                  {
                     ne[i] = YAC_CLONE_POOLED(NULL, template_object); // xxx TKS_MT: use *real* thread context (no c'tors otherwise)
                  }
               }
            }
            if(template_object)
            {
               for(; i < _maxElements; i++) // init new region 
               {
                  ne[i] = YAC_CLONE_POOLED(NULL, template_object); // xxx TKS_MT: use *real* thread context (no c'tors otherwise)
               }
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
         elements = ne;
         max_elements = _maxElements;
         if(num_elements > max_elements) 
         {
            num_elements = max_elements; 
         }
         return YAC_TRUE;
      }
      else
      {
         Dprintf("\n[---] failed to realloc ObjectArray to %u\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // realloc(0) -> free()
      YAC_ObjectArray::free();
      return YAC_TRUE;
   }
}

void YAC_ObjectArray::yacArraySetTemplate(YAC_Object *_templateObject) {
   if(YAC_VALID(_templateObject))
   {
      setTemplate(_templateObject);
   }
   else
   {
      Dprintf("[---] YAC_ObjectArray::yacArraySetTemplate: invalid template object.\n");
   }
}

void YAC_ObjectArray::setTemplate(YAC_Object *_o) {
   template_object=_o;
   YAC_ObjectArray::free();
}

YAC_Object *YAC_ObjectArray::getTemplate(void) {
   return template_object;
}


void YAC_VCALL YAC_ObjectArray::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o!=((YAC_Object*)this))
   {
      if(_cmd==YAC_OP_ASSIGN)
      {
         if(YAC_CHK(_o, YAC_CLID_OBJECTARRAY))
         {
            YAC_ObjectArray *o=(YAC_ObjectArray*)_o;
            YAC_ObjectArray::realloc(o->num_elements);
            num_elements=o->num_elements;
            sUI i;
            for(i=0; i<num_elements; i++)
            {
               if(o->elements[i])
               {
                  if(!elements[i]) 
                  {
                     elements[i] = YAC_CLONE_POOLED(NULL, o->elements[i]); 
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
            // Try generic array assign
            sUI ne = _o->yacArrayGetNumElements();
            sUI i;
            YAC_ObjectArray::realloc(ne);
            num_elements = ne;
            for(i=0; (i<ne) && (i<num_elements); i++)
            {
               YAC_Value r;
               _o->yacArrayGet(NULL, i, &r);
               r.typecast(YAC_TYPE_OBJECT);
               if(r.type >= YAC_TYPE_OBJECT)
               {
                  if(YAC_VALID(r.value.object_val))
                  {
                     if(r.deleteme && tkscript->cpp_typecast_map[r.value.object_val->class_ID][template_object->class_ID])
                     {
                        r.deleteme = 0;
                        elements[i] = r.value.object_val;
                     }
                     else
                     {
                        // Assign not directly compatible object
                        if(elements[i] == NULL)
                        {
                           elements[i] = YAC_CLONE_POOLED(NULL, template_object);
                        }
                        elements[i]->yacOperatorAssign(r.value.object_val);
                        r.unset();
                     }
                  }
                  else
                  {
                     if(elements[i] != NULL)
                     {
                        YAC_DELETE(elements[i]);
                        elements[i] = NULL;
                     }
                  }
               }
               else
               {
                  if(elements[i] != NULL)
                  {
                     YAC_DELETE(elements[i]);
                     elements[i] = NULL;
                  }
               }
            }
         }
      }
   }
   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_VCALL YAC_ObjectArray::yacOperatorInit(void *_context,  YAC_Object *_o) {

   (void)_context;

   if(YAC_CHK(_o, YAC_CLID_OBJECTARRAY))
   {
      YAC_ObjectArray *o=(YAC_ObjectArray*)_o;
      template_object=o->template_object;
   }
   return;
}


sUI YAC_ObjectArray::getMaxElements(void) {
   return max_elements;
}

sUI YAC_ObjectArray::getNumElements(void) {
   return num_elements;
}

void YAC_ObjectArray::empty(void) { 
   num_elements = 0u; 
}

sBool YAC_ObjectArray::isEmpty(void) {
   return (0u == num_elements);
}

void YAC_ObjectArray::setNumElements(sSI _i) {
   if(((sUI)_i) <= max_elements) 
   {
      num_elements = _i; 
   }
}


void YAC_ObjectArray::reverse(void) {
   if(num_elements&&elements)
   {
      YAC_Object**s=elements;
      sSI l=(num_elements)>>1;
      YAC_Object**d=&elements[num_elements-1];
      for(; l>=0; l--)
      {
         YAC_Object*t;
         t=*s;
         *s++=*d;
         *d--=t;
      }
   }
}

YAC_Object *YAC_ObjectArray::getNextFree(void) {
   if(num_elements!=max_elements) 
   {
      return elements[num_elements++]; 
   }
   else 
   {
      return 0; 
   }
}
 
sSI YAC_ObjectArray::indexOfPointer(YAC_Object *_o, sSI _startOff) { 
   if(YAC_VALID(_o)) // StringArray never stores null pointers!
   {
      sUI i = (sUI) _startOff; 
      for(i=0; i<num_elements; i++) 
      { 
         if( ((void*)elements[i]) == (void*)_o) 
         { 
            // Ok, found matching object
            return (sSI) i; 
         } 
      } 
   }
   // Failed, object address was not found in this array
   return -1; 
} 

sSI YAC_ObjectArray::indexOfObject(YAC_Object *_s, sSI _startOff) { 
   sUI i = (sUI) _startOff; 
   for(i=0; i<num_elements; i++) 
   { 
      if(elements[i]->yacEquals(_s)) 
      { 
         // Ok, found matching object
         return (sSI) i; 
      } 
   } 
   // Failed, object was not found in this array
   return -1; 
} 

sSI YAC_ObjectArray::containsObject(YAC_Object *_s) { 
   return ( -1 != indexOfObject(_s, 0) );
} 

sSI YAC_ObjectArray::containsPointer(YAC_Object *_s) { 
   return ( -1 != indexOfPointer(_s, 0) );
} 


sSI YAC_ObjectArray::_swap(sSI _indexs, sSI _indexd) {
   sUI idxs = (sUI) _indexs;
   sUI idxd = (sUI) _indexd;
   if(idxs!=idxd)
   {
      if(idxs<num_elements)
      {
         if(idxd<num_elements)
         {
            YAC_Object *t;
            t = elements[idxd];
            elements[idxd] = elements[idxs];
            elements[idxs] = t;
            return 1;
         }
      }
   }
   return 0;
}

void YAC_ObjectArray::_useAll(void) {
   num_elements = max_elements;
}

YAC_Object *YAC_ObjectArray::_getFirst(void) {
   if(num_elements > 0)
   {
      return elements[0];
   }
   else
   {
      return NULL;
   }
}

YAC_Object *YAC_ObjectArray::_getLast(void) {
   if(num_elements > 0)
   {
      return elements[num_elements - 1];
   }
   else
   {
      return NULL;
   }
}
