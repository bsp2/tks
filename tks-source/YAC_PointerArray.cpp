/// YAC_PointerArray.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"

#include "YAC_PointerArray.h"
#include "TKS_PointerArrayIterator.h"

#include "YAC_ObjectArray.h"
#include "tks_inc_class.h"
#include "YAC_ClassArray.h"

#include "YAC_ValueObject.h"


YAC_PointerArray::YAC_PointerArray(void) {
   class_ID     = YAC_CLID_POINTERARRAY;
   max_elements = 0u;
   num_elements = 0u;
   elements     = NULL;
}

YAC_PointerArray::~YAC_PointerArray() {
   YAC_PointerArray::free();
}

sBool YAC_PointerArray::yacToString(YAC_String *_s) const {
   _s->alloc(2+num_elements*32+1);
   _s->empty();
   _s->append("[");
   sUI i;
   YAC_String s;
   for(i = 0u; i < num_elements; i++)
   {
      if(i)
      {
         _s->append(", ");
      }
      switch(elements[i].type)
      {
      case 3:
      case 4:
         if(YAC_VALID(elements[i].value.object_val))
         {
            s.empty();
            if(!elements[i].value.object_val->yacToString(&s))
            {
               if(YAC_VALID(elements[i].value.object_val))
               {
                  s.alloc(128);
                  s.printf("<%s" YAC_PRINTF_PTRPREFIX "%p>", (elements[i].deleteme ? "@" : "#"), elements[i].value.object_val);
               }
               else
               {
                  s.visit("<null>");
               }
            }
            _s->append(&s);
            break;
         }
         else
         {
            ////if(elements[i].isNullOrIF0())
            if(elements[i].isNull())
            {
               _s->append("<null>");
            }
            else
            {
               _s->append("*ILL*");
            }
         }
      default:
         //_s->append("");
         break;
      }
   }
   _s->append("]");
   return 1;
}

void YAC_PointerArray::_getString(YAC_Value *_r) const {
   _r->initEmptyString();
   yacToString(_r->value.string_val);
}

sBool YAC_VCALL YAC_PointerArray::yacIteratorInit(YAC_Iterator *_it) const {
   Diteratorinit(TKS_PointerArrayIterator);
   return 1;
}

void YAC_VCALL YAC_PointerArray::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();

   YAC_SERIALIZE_I32(num_elements);
   YAC_SERIALIZE_I32(max_elements);
   if(num_elements)
   {
      /// ---- serialize all objects that are read-write, read-only pointers become "0" ----
      sUI i;
      for(i = 0u; i < num_elements; i++)
      {
         sBool bSerialized = YAC_FALSE;
         if(YAC_VALID(elements[i].value.object_val)) // used?
         {
            if( (!elements[i].value.object_val->yacIsComposite()) || elements[i].deleteme )
            {
               // ---- tag used ----
               YAC_SERIALIZE_I8(1);
               elements[i].value.object_val->yacSerialize(_ofs, 1);
               bSerialized = YAC_TRUE;
            }
         }
         if(!bSerialized)
         {
            // ---- tag unused ----
            YAC_SERIALIZE_I8(0);
         }
      }
   }

}

sUI YAC_VCALL YAC_PointerArray::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();

   YAC_PointerArray::free();

   sUI ne = YAC_DESERIALIZE_I32();
   sUI me = YAC_DESERIALIZE_I32();

   if(me >= tkscript->configuration.streamMaxArraySize)
   {
      Dprintf("[---] PointerArray::deserialize: insane array size (maxElements) (%i>%i)\n",
         me, tkscript->configuration.streamMaxArraySize);
      return 0u;
   }

   if(ne >= tkscript->configuration.streamMaxArraySize)
   {
      Dprintf("[---] PointerArray::deserialize: insane array size (numElements) (%i>%i)\n",
         ne, tkscript->configuration.streamMaxArraySize);
      return YAC_FALSE;
   }

   // ---- allocate new array ----
   if(!YAC_PointerArray::alloc(me))
   {
      Dprintf("[---] PointerArray::deserialize: failed to allocate %i elements.\n", me);
      return YAC_FALSE;
   }

   /// ---- now deserialize elements / add null pointers ----
   if(ne)
   {
      num_elements = ne;
      for(sUI i = 0u; i < ne; i++)
      {
         sU8 bUsed = YAC_DESERIALIZE_I8();
         if(bUsed)
         {
            /// ---- deserialize new object ----
            YAC_Object *o = tkscript->deserializeNewObject(_ifs, 0);
            if(o)
            {
               // ---- remember object ----
               YAC_Value sv;
               sv.initObject(o, 1);
               yacArraySet(NULL, i, &sv);// xxx TKS_MT: should use *real* thread context (exceptions)
            }
            else
            {
               // ---- failed to create new object ----
               return 0;
            }
         }
         else
         {
            /// ---- add null pointer ----
            elements[i].type             = YAC_TYPE_OBJECT;
            elements[i].deleteme         = YAC_FALSE;
            elements[i].value.object_val = NULL;
         }
      }
   }
   // ---- numElements should now equal ne ----
   return 1u;
}

void YAC_VCALL YAC_PointerArray::yacArrayCopySize(YAC_Object *_array) {
   YAC_PointerArray *o = (YAC_PointerArray*)_array;
   YAC_PointerArray::alloc(o->max_elements);
}

sBool YAC_PointerArray::_add(YAC_Object *_value) {
   if(YAC_VALID(_value))
   {
      if(YAC_BCHK(_value, YAC_CLID_VALUE))
      {
         yacArraySet(NULL, num_elements, (YAC_ValueObject*)_value); // xxx TKS_MT: should use *real* thread context (exceptions)
         return YAC_TRUE;
      }
      else
      {
         // ---- wrap (read-only) ScriptObject in YAC_Value
         YAC_Value v; v.initObject(_value, 0);
         yacArraySet(NULL, num_elements, &v); // xxx TKS_MT: should use *real* thread context (exceptions)
         return YAC_TRUE;
      }
   }
   else
   {
      // Add <null>
      YAC_Value v; v.initNull();
      yacArraySet(NULL, num_elements, &v);
   }
   return YAC_FALSE;
}

sBool YAC_PointerArray::_remove(YAC_Object *_o) {
   sUI i;
   void *o;
   if(YAC_VALID(_o))
   {
      if(YAC_BCHK(_o, YAC_CLID_VALUE))
      {
         YAC_ValueObject *vo = (YAC_ValueObject*) _o;
         if(vo->type >= YAC_TYPE_OBJECT)
            o = (void*) vo->value.object_val;
         else
            o = NULL;
      }
      else
      {
         o = (void*)_o;
      }
   }
   else
   {
      o = NULL;
   }

   for(i = 0u; i < num_elements; i++)
   {
      if((void*)elements[i].value.object_val == o)
      {
         return _delete(i);
      }
   }
   return YAC_FALSE;
}


sBool YAC_PointerArray::_insert(sSI _index, YAC_Object *_val) {
   sUI index = (sUI) _index;
   if(index >= num_elements)
   {
      if(index == num_elements)
      {
         return _add(_val);
      }
      else
      {
         Dprintf("[---] PointerArray::insert: Cannot insert beyond array boundaries (index=%i, numElements=%i).\n", index, num_elements);
         return YAC_FALSE;
      }
   }
   // Check if current array can hold new element
   if(num_elements == max_elements)
   {
      if(! YAC_PointerArray::realloc(tks_grow_array(num_elements+1)) )
      {
         // re-allocation failed
         return YAC_FALSE;
      }
   }
   // Shift elements
   for(sUI i = num_elements; i > index; i--)
      elements[i].copySafe(&elements[i - 1u]);

   if(YAC_VALID(_val))
   {
      if(YAC_BCHK(_val, YAC_CLID_VALUE))
      {
         YAC_ValueObject *vo = (YAC_ValueObject*) _val;
         if(vo->type < YAC_TYPE_OBJECT)
            vo->typecast(YAC_TYPE_OBJECT);
         elements[index].copySafe(vo);
      }
      else
      {
         elements[index].initObject(_val, 0);
      }
   }
   else
   {
      // Insert <null>
      elements[index].initNull();
   }
   num_elements++;
   return YAC_TRUE;
}

sBool YAC_PointerArray::_delete(sUI _index) {
   if(_index < num_elements)
   {
      if(num_elements > 1u)
      {
         for(sUI i = _index; i < (num_elements - 1u); i++)
            elements[i].copySafe(&elements[i + 1u]);
         elements[num_elements - 1u].unset();
      }
      else
      {
         elements[0].unset();
      }
      num_elements--;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_PointerArray::_swap(sUI _indexs, sUI _indexd) {
   sUI idxs = (sUI) _indexs;
   sUI idxd = (sUI) _indexd;
   if(idxs != idxd)
   {
      if(idxs < num_elements)
      {
         if(idxd < num_elements)
         {
            YAC_Value t;
            t.fastDerefValue(&elements[idxd]);
            elements[idxd].fastDerefValue(&elements[idxs]);
            elements[idxs].fastDerefValue(&t);
            return YAC_TRUE;
         }
      }
   }
   return YAC_FALSE;
}

void YAC_VCALL YAC_PointerArray::yacArraySet(void *_context, sUI _index, YAC_Value *_value) {
   if(_index >= max_elements)
   {
      if(tkscript->configuration.b_autoresizearrays && (((sSI)_index) >= 0))
      {
         if(YAC_PointerArray::realloc(tks_grow_array(_index+1)))
         {
            num_elements = _index + 1u;
            goto YAC_PointerArray_yacarrayset_1;
         }
      }

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("PointerArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS, msg);
      return;
   }
   if(_index >= num_elements) // track highest access point
   {
      num_elements = _index + 1u;
   }
YAC_PointerArray_yacarrayset_1:
   if(_value->type < YAC_TYPE_OBJECT)
      _value->typecast(YAC_TYPE_OBJECT);
   elements[_index].copySafe(_value);
}

void YAC_VCALL YAC_PointerArray::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   if(_index < max_elements)
   {
      _r->initObject(elements[_index].value.object_val, 0); // return read-only
   }
   else
   {
      _r->initVoid();

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("PointerArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

void YAC_VCALL YAC_PointerArray::yacArrayGetDeref(void *_context, sUI _index, YAC_Value *_r) {
   if(_index < max_elements)
   {
      _r->fastDerefValue(&elements[_index]);
   }
   else
   {
      _r->initVoid();

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("PointerArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

void YAC_PointerArray::getDeref(sSI _index, YAC_Value *_r) {
   if(sUI(_index) < max_elements)
      _r->fastDerefValue(&elements[_index]);
   else
      _r->initNull();
}

void YAC_PointerArray::unlink(YAC_Object *_o, YAC_Value *_r) {
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

YAC_Object *YAC_PointerArray::get(sSI _index) {
   if( ((sUI)_index) < num_elements )  // 19Oct2015
   {
      return elements[_index].value.object_val;
   }
   return NULL;
}

YAC_Object *YAC_PointerArray::_getRev(sSI _index) {
   sUI idx = ((sUI)(num_elements - 1u - _index));
   if(idx < max_elements)
   {
      return elements[idx].value.object_val;
   }
   return NULL;
}

void YAC_VCALL YAC_PointerArray::yacArraySetWidth(sUI _num) {
   if(_num<=max_elements)
   {
      num_elements=_num;
   }
}

sBool YAC_VCALL YAC_PointerArray::yacArrayAlloc(sUI _maxelements, sUI,sUI,sUI) {
   return YAC_PointerArray::alloc(_maxelements);
}

sBool YAC_VCALL YAC_PointerArray::yacArrayRealloc(sUI _sx, sUI, sUI, sUI) {
   return YAC_PointerArray::realloc((sSI)_sx);
}

sBool YAC_PointerArray::alloc(sUI _maxElements) {

   if(_maxElements > TKS_MAX_ARRAYALLOCSIZE)
   {
      tkscript->printf("[---] PointerArray::insane array size (maxelements=%08x)\n", _maxElements);
      return YAC_FALSE;
   }

   YAC_PointerArray::free();

   if(_maxElements)
   {
      elements = new(std::nothrow) YAC_Value[_maxElements];
      if(elements)
      {
         max_elements = _maxElements;
         num_elements = 0u;
         sUI i;
         for(i = 0u; i<max_elements; i++)
         {
            elements[i].initObject(0, 0);
         }
         return max_elements;
      }
      else
      {
         Dprintf("[---] PointerArray: failed to allocate %u elements\n", _maxElements);
         num_elements = 0u;
         max_elements = 0u;
         return YAC_FALSE;
      }
   }
   else
   {
      // alloc(0) -> free()
      return YAC_TRUE;
   }
}

sBool YAC_PointerArray::realloc(sUI _maxElements) {
   if(_maxElements > 0u)
   {
      YAC_Value *ne = new(std::nothrow)YAC_Value[_maxElements];
      if(ne)
      {
         sUI i = 0u;
         if(_maxElements <= max_elements)
         {
            // shrink
            for(i = 0u; i < _maxElements; i++)
            {
               ne[i] = &elements[i]; // copy elements
            }
            for(; ((sUI)i) < max_elements; i++)
            {
               elements[i].unset(); // unset superfluous elements
            }
         }
         else
         {
            // expand
            for(; i < max_elements; i++)
            {
               ne[i] = &elements[i]; // copy existing region
            }
            for(; i < _maxElements; i++) // init new region
            {
               ne[i].initObject(0,0);
            }
         }
         if(NULL != elements)
         {
            delete [] elements;
         }
         elements = ne;
         max_elements = _maxElements;
         if(num_elements > max_elements)
            num_elements = max_elements;
         return YAC_TRUE;
      }
      else
      {
         Dprintf("\n[---] failed to realloc PointerArray to %u\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // realloc(0) -> free()
      YAC_PointerArray::free();
      return YAC_TRUE;
   }
}

void YAC_VCALL YAC_PointerArray::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o!=((YAC_Object*)this))
   {
      if(YAC_OP_ASSIGN == _cmd)
      {
         if(YAC_CHK(_o, YAC_CLID_POINTERARRAY))
         {
            YAC_PointerArray *o = (YAC_PointerArray*)_o;
            YAC_PointerArray::realloc(o->num_elements);
            sUI i;
            num_elements=o->num_elements;
            for(i = 0u; i < num_elements; i++)
            {
               elements[i].unset();
               elements[i].type      = o->elements[i].type;
               elements[i].value.any = o->elements[i].value.any;
            }
            return;
         }
         else if(YAC_CHK(_o, YAC_CLID_OBJECTARRAY))
         {
            YAC_ObjectArray *o = (YAC_ObjectArray*)_o;
            YAC_PointerArray::realloc(o->num_elements);
            sUI i;
            for(i = 0u; i < num_elements; i++)
            {
               elements[i].initObject(o->elements[i], 0);
               if(elements[i].value.object_val->class_ID==YAC_CLID_STRING)
                  elements[i].type=4; // fix string type
            }
            return;
         }
         else if(YAC_CHK(_o, YAC_CLID_CLASSARRAY))
         {
            YAC_ClassArray *o = (YAC_ClassArray*)_o;
            YAC_PointerArray::realloc(o->num_elements);
            sUI i;
            for(i = 0u; i < num_elements; i++)
            {
               elements[i].initObject(o->elements[i], 0);
            }
            return;
         }
         else if(YAC_VALID(_o))
         {
            // Try generic array assign
            sUI ne = _o->yacArrayGetNumElements();
            sUI i;
            YAC_PointerArray::realloc(ne);
            num_elements = ne;
            for(i = 0u; (i < ne) && (i < num_elements); i++)
            {
               YAC_Value r;
               _o->yacArrayGet(NULL, i, &r);
               r.typecast(YAC_TYPE_OBJECT);
               if(r.type >= YAC_TYPE_OBJECT)
               {
                  elements[i].copySafe(&r); // Grab pointer
               }
               else
               {
                  elements[i].safeInitNull();
               }
            }
         }
      }
      else if(YAC_OP_ADD == _cmd)
      {
         _joinCC(this, _o);
      }
   }
   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_PointerArray::free(void) {
   if(elements)
   {
      unsetAll();

      delete [] elements;
      elements = NULL;
      num_elements = 0u;
      max_elements = 0u;
   }
}

void YAC_PointerArray::unsetAll(void) {
   for(sUI i = 0u; i < max_elements; i++)
   {
      elements[i].unset();
   }
}

sUI YAC_PointerArray::getMaxElements(void) {
   return max_elements;
}

sUI YAC_PointerArray::getNumElements(void) {
   return num_elements;
}

void YAC_PointerArray::setNumElements(sUI _nr) {
   if(_nr <= max_elements)
   {
      num_elements = _nr;
   }
}

void YAC_PointerArray::empty(void) {
   num_elements = 0u;
}

sBool YAC_PointerArray::isEmpty(void) {
   return (0u == num_elements);
}

void YAC_PointerArray::reverse(void) {
   if(num_elements&&elements)
   {
      YAC_Value *s=elements;
      sSI l = (num_elements >> 1);
      YAC_Value *d = &elements[num_elements - 1u];
      for(; l >= 0; l--)
      {
         YAC_Value t;
         t = s;
         *s++=*d;
         *d--=&t;
      }
   }
}

sSI YAC_PointerArray::indexOfPointer(YAC_Object *_o, sSI _off) {
   sUI i = (sUI) _off;

   if(YAC_VALID(_o))
   {
      for(; i<num_elements; i++)
      {
         if( ((void*)elements[i].value.object_val) == ((void*)_o) )
         {
            // Ok, found object address
            return (sSI) i;
         }
      }
   }
   else
   {
      for(; i<num_elements; i++)
      {
         if(!YAC_VALID(elements[i].value.object_val))
         {
            // Ok, found object address
            return (sSI) i;
         }
      }
   }
   // Failed, address not found
   return -1;
}

sSI YAC_PointerArray::indexOfObject(YAC_Object *_o, sSI _off) {
   sUI i = (sUI) _off;
   for(; i < num_elements; i++)
   {
      if(YAC_VALID(elements[i].value.object_val))
      {
         if(elements[i].value.object_val->yacEquals(_o))
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


sSI YAC_PointerArray::containsObject(YAC_Object *_o) {
   return (-1 != indexOfObject(_o, 0));
}

sSI YAC_PointerArray::containsPointer(YAC_Object *_o) {
   return (-1 != indexOfPointer(_o, 0));
}

YAC_Object *YAC_PointerArray::getNextFree(void) {
   if(num_elements < max_elements)
   {
      return elements[num_elements++].value.object_val;
   }
   else
   {
      return yac_null;
   }
}

void YAC_PointerArray::_useAll(void) {
   num_elements = max_elements;
}

YAC_Object *YAC_PointerArray::_getFirst(void) {
   if(num_elements > 0u)
   {
      return elements[0].value.object_val;
   }
   else
   {
      return NULL;
   }
}

YAC_Object *YAC_PointerArray::_getLast(void) {
   if(num_elements > 0u)
   {
      return elements[num_elements - 1].value.object_val;
   }
   else
   {
      return NULL;
   }
}

sSI YAC_PointerArray::rearrange(YAC_Object *_ia) {
   if(YAC_VALID(_ia))
   {
      if(YAC_Is_IntArray(_ia))
      {
         YAC_IntArray *ia = (YAC_IntArray*) _ia;
         if( (ia->num_elements == num_elements) && (ia->num_elements > 1u) && (num_elements > 1u) )
         {
            YAC_Value *ne = new(std::nothrow) YAC_Value[num_elements];
            for(sUI i = 0u; i < num_elements; i++)
            {
               ne[i] = &elements[ia->elements[i]];
            }
            delete [] elements;
            elements = ne;
            max_elements = num_elements;
            return 1;
         }
      }
   }
   return 0;
}

YAC_Value *YAC_PointerArray::joinArray(YAC_Value *_d, YAC_Object *_a, sUI _neA, sBool _bCopy, sBool _bUnlink) {
   sUI i = 0;
   while(i < _neA)
   {
      YAC_Value r;

      if(_bUnlink)
      {
         _a->yacArrayGetDeref(NULL/*context*/, i++, &r);

         if(r.type < YAC_TYPE_OBJECT)
         {
            _d->initNull();
         }
         else
         {
            _d->fastDerefValue(&r);
         }
      }
      else
      {
         _a->yacArrayGet(NULL/*context*/, i++, &r);

         if(r.type < YAC_TYPE_OBJECT)
         {
            _d->initNull();
         }
         else
         {
            if(_bCopy)
            {
               if(YAC_VALID(r.value.object_val))
               {
                  _d->duplicate(NULL/*context*/, &r);
                  _d->value.object_val->yacOperatorAssign(r.value.object_val);
               }
               else
               {
                  _d->initNull();
               }
            }
            else
            {
               _d->fastDerefValue(&r);
            }
         }
      }

      r.unsetFast();

      _d++;
   }

   return _d;
}

sBool YAC_PointerArray::_joinArrays(YAC_Object *_a, YAC_Object *_b, sBool _bCopyA, sBool _bCopyB, sBool _bUnlinkA, sBool _bUnlinkB) {
   // Generic
   sUI neA;
   if(YAC_VALID(_a))
   {
      neA = _a->yacArrayGetNumElements();
   }
   else
   {
      neA = 0;
   }

   sUI neB;
   if(YAC_VALID(_b))
   {
      neB = _b->yacArrayGetNumElements();
   }
   else
   {
      neB = 0;
   }

   if((void*)this == (void*)_a)
   {
      // _bCopyA = 0;
      _bUnlinkA = 1;
   }

   if((void*)this == (void*)_b)
   {
      // _bCopyB = 0;
      _bUnlinkB = 1;
   }

   sUI neAB = (neA + neB);
   if(neAB > 0u)
   {
      YAC_Value *d = new(std::nothrow) YAC_Value[neAB];

      if(NULL != d)
      {
         YAC_Value *dStart = d;

         d = joinArray(d, _a, neA, _bCopyA, _bUnlinkA);

         d = joinArray(d, _b, neB, _bCopyB, _bUnlinkB);

         free();

         elements = dStart;
         num_elements = max_elements = neAB;
      }
      else
      {
         printf("[---] YAC_PointerArray::joinArrays: failed to allocate %u elements.\n", neAB);
         return 0;
      }
   }
   else
   {
      YAC_PointerArray::free();
   }

   return 1;
}

sBool YAC_PointerArray::_joinCC(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 1/*bCopyA*/, 1/*bCopyB*/, 0/*bUnlinkA*/, 0/*bUnlinkB*/);
}

sBool YAC_PointerArray::_joinCR(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 1/*bCopyA*/, 0/*bCopyB*/, 0/*bUnlinkA*/, 0/*bUnlinkB*/);
}

sBool YAC_PointerArray::_joinCD(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 1/*bCopyA*/, 0/*bCopyB*/, 0/*bUnlinkA*/, 1/*bUnlinkB*/);
}

sBool YAC_PointerArray::_joinRR(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 0/*bCopyB*/, 0/*bUnlinkA*/, 0/*bUnlinkB*/);
}

sBool YAC_PointerArray::_joinRC(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 1/*bCopyB*/, 0/*bUnlinkA*/, 0/*bUnlinkB*/);
}

sBool YAC_PointerArray::_joinRD(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 0/*bCopyB*/, 0/*bUnlinkA*/, 1/*bUnlinkB*/);
}

sBool YAC_PointerArray::_joinDD(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 0/*bCopyB*/, 1/*bUnlinkA*/, 1/*bUnlinkB*/);
}

sBool YAC_PointerArray::_joinDC(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 1/*bCopyB*/, 1/*bUnlinkA*/, 0/*bUnlinkB*/);
}

sBool YAC_PointerArray::_joinDR(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 0/*bCopyB*/, 1/*bUnlinkA*/, 0/*bUnlinkB*/);
}

sBool YAC_PointerArray::isFirst(YAC_Object *_o) {
   return (num_elements > 0u) && (indexOfPointer(_o, 0) == 0);
}

sBool YAC_PointerArray::isLast(YAC_Object *_o) {
   return (num_elements > 0u) && (indexOfPointer(_o, 0) == sSI(num_elements - 1u));
}
