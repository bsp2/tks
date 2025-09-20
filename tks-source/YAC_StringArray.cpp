/// YAC_StringArray.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "YAC_ValueObject.h"
#include "YAC_StringArray.h"
#include "TKS_StringArrayIterator.h"

#include "YAC_IntArray.h"
#include "YAC_FloatArray.h"
#include "YAC_ObjectArray.h"
#include "YAC_PointerArray.h"


YAC_StringArray::YAC_StringArray(void) {
   class_ID     = YAC_CLID_STRINGARRAY;
   max_elements = 0u;
   num_elements = 0u;
   elements     = NULL;
}

YAC_StringArray::~YAC_StringArray() {
   YAC_StringArray::free();
}

sBool YAC_VCALL YAC_StringArray::yacIteratorInit(YAC_Iterator *_it) const {
   Diteratorinit(TKS_StringArrayIterator);
   return 1;
}

sBool YAC_VCALL YAC_StringArray::yacToString(YAC_String *_s) const {
   _s->alloc(2+_s->getLength()*32+1);
   _s->empty();
   _s->append("[");
   for(sUI i = 0u; i < num_elements; i++)
   {
      if(i)
         _s->append(", ");

      YAC_String s;
      elements[i].yacToString(&s);
      _s->append(&s);
   }
   _s->append("]");
   return 1;
}

void YAC_StringArray::_getString(YAC_Value *_r) const {
   _r->initEmptyString();
   yacToString(_r->value.string_val);
}

void YAC_VCALL YAC_StringArray::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();

   YAC_SERIALIZE_I32(num_elements);

   for(sUI i = 0u; i < num_elements; i++)
      elements[i].yacSerialize(_ofs, 0); // do not write class name
}

sUI YAC_VCALL YAC_StringArray::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();

   sUI ne = YAC_DESERIALIZE_I32();

   if(sUI(ne) < tkscript->configuration.streamMaxArraySize)
   {
      sUI i;
      if(alloc(ne))
      {
         num_elements=ne;
         for(i = 0u; i < num_elements; i++)
            elements[i].yacDeserialize(_ifs, 0);
      }
      else
      {
         YAC_String t;
         /// ---- out of memory, skip stream ----
         for(i = 0u; i < ne; i++)
            t.yacDeserialize(_ifs, 0);
         t.free();
      }
      return YAC_TRUE;
   }
   else
   {
      free();
      Dprintf("[---] StringArray::deserialize: insane array size (%i>%i)\n",
         ne, tkscript->configuration.streamMaxArraySize);
   }
   return YAC_FALSE;
}

void YAC_VCALL YAC_StringArray::yacArrayCopySize(YAC_Object *_array) {
   YAC_StringArray *o=(YAC_StringArray*)_array;
   alloc(o->max_elements);
}

void YAC_VCALL YAC_StringArray::yacArraySet(void *_context, sUI _index, YAC_Value *_value) {
   if(_index >= max_elements)
   {
      if(tkscript->configuration.b_autoresizearrays && (((sSI)_index) >= 0))
      {
         if(YAC_StringArray::realloc(tks_grow_array(_index+1)))
         {
            num_elements = _index + 1u;
            goto YAC_StringArray_yacarrayset_1;
         }
      }

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("StringArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS, msg);
      return;
   }

   if(_index >= num_elements)
      num_elements = _index + 1u;

YAC_StringArray_yacarrayset_1:
   if(_value->deleteme)
   {
      // move chars array to this string (copy pointers)
      elements[_index].refCopy(_value->value.string_val);
   }
   else
   {
      // copy chars array
      elements[_index].yacCopy(_value->value.string_val);
   }
}

void YAC_VCALL YAC_StringArray::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   if(_index < max_elements)
   {
      _r->initString(&elements[_index], 0);
   }
   else
   {
      _r->initVoid();

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("StringArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

YAC_Object *YAC_StringArray::get(sSI _index) {
   if(sUI(_index) < num_elements)
   {
      return &elements[_index];
   }
   else
   {
      return yac_null;
   }
}

void YAC_StringArray::_set(sUI _idx, YAC_Object *_val) {
   if(_idx < max_elements)
   {
      elements[_idx].yacCopy(_val);

      if(_idx >= num_elements)
         num_elements = _idx + 1u;
   }
}

YAC_Object *YAC_StringArray::_getRev(sSI _index) {
   const sUI idx = ((sUI)(num_elements - 1 - _index));
   if(idx < num_elements)
   {
      return &elements[_index];
   }
   else
   {
      return yac_null;
   }
}

void YAC_StringArray::_setRev(sUI _idx, YAC_Object *_val) {
   const sUI idx = ((sUI)(num_elements - 1 - _idx));
   if(idx < num_elements)
   {
      elements[_idx].yacCopy(_val);
   }
}

void YAC_StringArray::getCopy(sSI _index, YAC_Value *_r) {
   if(sUI(_index) < num_elements)
   {
      _r->initNewString(&elements[_index]); // return copy of String at _index
   }
   else
   {
      _r->initNull();
   }
}

void YAC_StringArray::_getDefault(sUI _index, YAC_Object *_valOrVO, YAC_Value *_r) {
   if(_index < num_elements)
   {
      _r->initString(&elements[_index], 0);
   }
   else if(YAC_Is_Value(_valOrVO))
   {
      YAC_CAST_ARG(YAC_ValueObject *, vo, _valOrVO);
      _r->copySafe(*vo);
   }
   else
   {
      _r->initObject(_valOrVO, 0);
   }
}

sBool YAC_VCALL YAC_StringArray::yacArrayAlloc(sUI _maxelements, sUI,sUI,sUI) {
   return alloc(_maxelements);
}

sBool YAC_StringArray::alloc(sUI _maxElements) {
   if(_maxElements > 0u)
   {
      if(_maxElements > TKS_MAX_ARRAYALLOCSIZE)
      {
         tkscript->printf("[---] StringArray::insane array size (maxelements=%08x)\n", _maxElements);
         return YAC_FALSE;
      }
      if(NULL != elements)
      {
         delete [] elements;
      }
      elements = new(std::nothrow)YAC_String[_maxElements];
      if(NULL != elements)
      {
         max_elements = _maxElements;
         num_elements = 0u;
         return YAC_TRUE;
      }
      else
      {
         // Allocation failed
         Dprintf("[---] StringArray: failed to allocate %u elements\n", _maxElements);
         num_elements = 0u;
         max_elements = 0u;
         return YAC_FALSE;
      }
   }
   else
   {
      // alloc(0) -> free()
      YAC_StringArray::free();
      return YAC_TRUE;
   }
}

sBool YAC_StringArray::realloc(sUI _maxElements) {
   if(_maxElements > 0u)
   {
      YAC_String *ne = new(std::nothrow)YAC_String[_maxElements];
      if(ne)
      {
         sUI i = 0u;
         if(_maxElements <= max_elements) // shrink
         {
            for(; i < _maxElements; i++)
            {
               ne[i].init();
               ne[i].refCopy(&elements[i]); // copy string
            }
         }
         else
         {
            for(; i < max_elements; i++) // expand, copy existing
            {
               ne[i].init();
               ne[i].refCopy(&elements[i]);
            }
            for(; i < _maxElements; i++) // init remaining new strings
               ne[i].init();
         }

         if(NULL != elements)
            delete [] elements;

         elements     = ne;
         max_elements = _maxElements;
         if(num_elements > max_elements)
            num_elements = max_elements;

         return YAC_TRUE;
      }
      else
      {
         Dprintf("[---] StringArray: failed to realloc to %u elements\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // realloc(0) -> free()
      YAC_StringArray::free();
      return YAC_TRUE;
   }
}

void YAC_VCALL YAC_StringArray::yacArraySetWidth(sUI _num) {
   if(_num <= max_elements)
      num_elements = _num;
}

void YAC_VCALL YAC_StringArray::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o != ((YAC_Object*)this))
   {
      if(YAC_OP_ASSIGN == _cmd)
      {
         if(YAC_CHK(_o, YAC_CLID_STRINGARRAY))
         {
            YAC_StringArray *o = (YAC_StringArray*)_o;
            realloc(o->num_elements);
            num_elements = o->num_elements;
            for(sUI i = 0u; i < num_elements; i++)
               elements[i].yacCopy(&o->elements[i]);
            return;
         }
         else if(YAC_CHK(_o, YAC_CLID_POINTERARRAY))
         {
            YAC_PointerArray *o = (YAC_PointerArray*)_o;
            realloc(o->num_elements);
            num_elements = o->num_elements;
            for(sUI i = 0u; i < num_elements; i++)
            {
               switch(o->elements[i].type)
               {
                  default:
                     elements[i].visit("");
                     break;

                  case YAC_TYPE_OBJECT:
                  {
                     YAC_Object *ov = o->elements[i].value.object_val;
                     if(YAC_VALID(ov))
                     {
                        if(!ov->yacToString(&elements[i]))
                        {
                           elements[i].visit("");
                        }
                     }
                     else
                     {
                        elements[i].visit("");
                     }
                  }
                  break;

               case YAC_TYPE_STRING:
                  {
                     YAC_String *sv = o->elements[i].value.string_val;
                     if(YAC_VALID(sv))
                        elements[i].yacCopy(sv);
                     else
                        elements[i].visit("");
                  }
                  break;
               }
            }
            return;
         }
         else
         {
            // any array (for example a ValueArray)
            if(YAC_VALID(_o))
            {
               realloc(_o->yacArrayGetNumElements());
               num_elements = _o->yacArrayGetNumElements();
               for(sUI i = 0u; i < num_elements; i++)
               {
                  YAC_Value r;
                  _o->yacArrayGet(NULL, i, &r);
                  switch(r.type)
                  {
                     default:
                        elements[i].visit("");
                        break;

                     case YAC_TYPE_INT:
                        elements[i].yacValueOfI(r.value.int_val);
                        break;

                     case YAC_TYPE_FLOAT:
                        elements[i].yacValueOfF32(r.value.float_val);
                        break;

                     case YAC_TYPE_OBJECT:
                     case YAC_TYPE_STRING:
                        if(YAC_VALID(r.value.object_val))
                           r.value.object_val->yacToString(&elements[i]);
                        else
                           elements[i].visit("");
                        break;
                  }

                  r.unsetFast();
               }
            } // if valid o
            else
            {
               free();
            }
            return;
         }
      }
      else if(YAC_OP_ADD == _cmd)
      {
         _join(this, _o);
         return;
      }
      else if(YAC_OP_AND == _cmd)
      {
         YAC_RETI(containsObject(_o));
         return;
      }
   }
   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_StringArray::free(void) {
   if(elements)
   {
      delete [] elements;
      elements = NULL;
      num_elements = 0u;
      max_elements = 0u;
   }
}

sUI YAC_StringArray::getMaxElements(void) {
   return max_elements;
}

sUI YAC_StringArray::getNumElements(void) {
   return num_elements;
}

void YAC_StringArray::setNumElements(sUI _num) {
   if(_num <= max_elements)
      num_elements = _num;
}

sBool YAC_StringArray::add(YAC_Object *_s) {
   sUI one = num_elements;
   if(YAC_BCHK(_s, YAC_CLID_STRING))
   {
      if(num_elements < max_elements)
      {
         elements[num_elements++].yacCopy(_s);
         return YAC_TRUE;
      }
      else
      {
         YAC_Value v;
         v.initString((YAC_String*)_s, 0);
         yacArraySet(NULL, num_elements, &v); // xxx TKS_MT: should use real thread context (exceptions)
      }
   }
   else
   {
      // Add empty string
      return addEmpty(1);
   }
   return (num_elements != one);
}

sUI YAC_StringArray::addEmpty(sUI _numElements) {
   if(num_elements + _numElements <= max_elements)
   {
      for(sUI i = 0u; i < _numElements; i++)
         elements[num_elements++].empty();

      return num_elements;
   }
   return 0u;
}

sBool YAC_StringArray::remove(YAC_Object *_s) {
   sSI idx = indexOfObject(_s, 0/*startOff*/);
   if(idx >= 0)
   {
      _delete(sUI(idx));
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

void YAC_StringArray::empty(void) {
   num_elements = 0u;
}

sBool YAC_StringArray::isEmpty(void) {
   return (0u == num_elements);
}

sBool YAC_StringArray::insert(sSI _index, YAC_Object *_s) {
   if(YAC_BCHK(_s, YAC_CLID_STRING))
   {
      sUI index = (sUI) _index;
      if(index >= num_elements)
      {
         if(index == num_elements)
         {
            return add(_s);
         }
         else
         {
            Dprintf("[---] StringArray::insert: Cannot insert beyond array boundaries (index=%i, numElements=%i).\n", index, num_elements);
            return YAC_FALSE;
         }
      }
      // Check if current array can hold new element
      if(num_elements == max_elements)
      {
         if(! YAC_StringArray::realloc(tks_grow_array(num_elements + 1u)) )
         {
            // re-allocation failed
            return YAC_FALSE;
         }
      }
      // Shift elements
      for(sUI i = num_elements; i > index; i--)
         elements[i].refCopy(&elements[i - 1u]);
      elements[index].yacCopy(_s);
      num_elements++;

      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_StringArray::_delete(sUI _index) {
   if(_index < num_elements)
   {
      if(--num_elements)
      {
         for(sUI i = _index; i < num_elements; i++)
            elements[i].refCopy(&elements[i + 1u]);
      }
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

void YAC_StringArray::unset(void) {
   if(elements)
   {
      for(sUI i = 0u; i < max_elements; i++)
      {
         elements[i].freeStack();
         elements[i].empty();
      }
   }
}

void YAC_StringArray::reverse(void) {
   //XXX optimize char*
   if(num_elements && elements)
   {
      YAC_String *s = elements;
      sSI l = sSI(num_elements >> 1);
      YAC_String *d = &elements[num_elements - 1u];
      for(; l >= 0; l--)
      {
         YAC_String t;
         t.refCopy(s);
         (*s++).refCopy(d);
         (*d--).refCopy(&t);
      }
   }
}

sBool YAC_StringArray::rearrange(YAC_Object *_ia) {
   if(YAC_VALID(_ia))
   {
      if(YAC_Is_IntArray(_ia))
      {
         YAC_IntArray *ia = (YAC_IntArray*) _ia;
         if( ia->num_elements == num_elements && ia->num_elements > 1u && num_elements > 1u )
         {
            YAC_String *ts = new YAC_String[num_elements];
            if(ts)
            {
               sUI i = 0u;
               for(; i < num_elements; i++)
               {
                  sUI srcId = ia->elements[i];
                  if(srcId > num_elements)
                  {
                     Dprintf("[---] StringArray::rearrange: source index (%i) out of bounds.\n", (sSI) srcId);
                     return YAC_FALSE;
                  }
                  ts[i].refCopy(&elements[srcId]);
               }
               for(i = 0u; i < num_elements; i++)
                  elements[i].refCopy(&ts[i]);

               delete [] ts;

               return YAC_TRUE;
            }
            else
            {
               Dprintf("[---] StringArray::rearrange: failed to allocate %i indices.\n", num_elements);
            }
         }
      }
   }
   return YAC_FALSE;
}

void YAC_StringArray::sortByValue(YAC_Object *_ia, sSI _cs) {
   if(YAC_BCHK(_ia, YAC_CLID_INTARRAY))
   {
      YAC_IntArray *ia = (YAC_IntArray*)_ia;
      if(elements && num_elements)
      {
         ia->identity(num_elements);
         quicksortByValue((sSI*)ia->elements, 0, num_elements - 1u, _cs);
      }
      else
      {
         ia->empty();
      }
   }
}

void YAC_StringArray::sortByLength(YAC_Object *_ia) {
   if(YAC_BCHK(_ia, YAC_CLID_INTARRAY))
   {
      YAC_IntArray *ia = (YAC_IntArray*)_ia;
      if(elements && num_elements)
      {
         ia->identity(num_elements);
         quicksortByLength((sSI*)ia->elements, 0, num_elements - 1u);
      }
      else
      {
         ia->empty();
      }
   }
}

void YAC_StringArray::quicksortByValue(sSI *_ia, sSI _l, sSI _r, sSI _cs) {
   YAC_String *m = &elements[ _ia[(_l + _r) >>1] ];
   sSI i = _l, j = _r, ti;
   while(i <= j)
   {
      while( elements[_ia[i]].isPredecessor(m, _cs) )
         i++;

      while( m->isPredecessor(&elements[_ia[j]], _cs) )
         j--;

      if(i <= j)
      {
         ti       = _ia[j];
         _ia[j--] = _ia[i];
         _ia[i++] = ti;
      }
   }
   if(_l < j)
      quicksortByValue(_ia, _l, j, _cs);

   if(i < _r)
      quicksortByValue(_ia, i, _r, _cs);
}

void YAC_StringArray::quicksortByLength(sSI *_ia, sSI _l, sSI _r) {
   sUI m = elements[ _ia[(_l + _r) >>1] ] .length;
   sSI i = _l, j = _r, ti;
   while(i <= j)
   {
      while( elements[_ia[i]].length < m )
         i++;

      while( m < elements[_ia[j]].length )
         j--;

      if(i <= j)
      {
         ti       = _ia[j];
         _ia[j--] = _ia[i];
         _ia[i++] = ti;
      }
   }

   if(_l < j)
      quicksortByLength(_ia, _l, j);

   if(i < _r)
      quicksortByLength(_ia, i, _r);
}

YAC_Object *YAC_StringArray::getNextFree(void) {
   if(num_elements != max_elements)
      return &elements[num_elements++];
   else
      return NULL;
}

sSI YAC_StringArray::indexOfPointer(YAC_Object *_o, sSI _startOff) {
   if(YAC_VALID(_o)) // StringArray never stores null pointers!
   {
      sUI i = (sUI) _startOff;
      for(; i < num_elements; i++)
      {
         if( ((void*)&elements[i]) == (void*)_o)
         {
            // Ok, found matching object
            return (sSI) i;
         }
      }
   }
   // Failed, object address was not found in this array
   return -1;
}

sSI YAC_StringArray::indexOfObject(YAC_Object *_s, sSI _startOff) {
   sUI i = (sUI) _startOff;
   for(; i < num_elements; i++)
   {
      if(elements[i].yacEquals(_s))
      {
         // Ok, found matching object
         return (sSI) i;
      }
   }
   // Failed, object was not found in this array
   return -1;
}

sSI YAC_StringArray::containsObject(YAC_Object *_s) {
   return ( -1 != indexOfObject(_s, 0) );
}

sSI YAC_StringArray::containsPointer(YAC_Object *_s) {
   return ( -1 != indexOfPointer(_s, 0) );
}

sSI YAC_StringArray::_swap(sSI _indexs, sSI _indexd) {
   sUI idxs = (sUI) _indexs;
   sUI idxd = (sUI) _indexd;
   if(idxs != idxd)
   {
      if(idxs < num_elements)
      {
         if(idxd < num_elements)
         {
            YAC_String *s = &elements[idxs];
            YAC_String *d = &elements[idxd];
            sUI    t_buflen = d->buflen;
            sUI    t_bflags = d->bflags;
            sUI    t_length = d->length;
            sUI    t_key    = d->key;
            sU8   *t_chars  = d->chars;
            StaticList::Node *t_clones = d->clones;

            d->buflen = s->buflen;
            d->bflags = s->bflags;
            d->length = s->length;
            d->key    = s->key;
            d->chars  = s->chars;
            d->clones = s->clones;
            s->buflen = t_buflen;
            s->bflags = t_bflags;
            s->length = t_length;
            s->key    = t_key;
            s->chars  = t_chars;
            s->clones = t_clones;
            return YAC_TRUE;
         }
      }
   }
   return YAC_FALSE;
}

void YAC_StringArray::_useAll(void) {
   num_elements = max_elements;
}

YAC_Object *YAC_StringArray::_getFirst(void) {
   if(num_elements > 0u)
      return &elements[0];
   else
      return NULL;
}

YAC_Object *YAC_StringArray::_getLast(void) {
   if(num_elements > 0u)
      return &elements[num_elements - 1u];
   else
      return NULL;
}

sBool YAC_StringArray::_join(YAC_Object *_a, YAC_Object *_b) {
   if(YAC_Is_StringArray(_a) && YAC_Is_StringArray(_b))
   {
      // Join StringArrays
      YAC_CAST_ARG(YAC_StringArray, a, _a);
      YAC_CAST_ARG(YAC_StringArray, b, _b);

      sUI neAB = (a->num_elements + b->num_elements);
      if(neAB > 0u)
      {
         YAC_String *d = new YAC_String[neAB];

         if(NULL != d)
         {
            sUI k = 0u;
            sUI i;

            i = 0u;
            if((void*)a == (void*)this)
            {
               while(i < a->num_elements)
               {
                  YAC_String *as = &a->elements[i++];
                  d[k].key    = as->key;
                  d[k].buflen = as->buflen;
                  d[k].bflags = as->bflags; // take ownership
                  d[k].chars  = as->chars;
                  d[k].length = as->length;
                  as->buflen = 0u;
                  as->bflags = 0u;
                  k++;
               }
            }
            else
            {
               while(i < a->num_elements)
               {
                  d[k++].yacCopy(&a->elements[i++]);
               }
            }

            i = 0u;
            if((void*)b == (void*)this)
            {
               while(i < b->num_elements)
               {
                  YAC_String *bs = &b->elements[i++];
                  d[k].key    = bs->key;
                  d[k].buflen = bs->buflen;
                  d[k].bflags = bs->bflags; // take ownership
                  d[k].chars  = bs->chars;
                  d[k].length = bs->length;
                  bs->buflen = 0u;
                  bs->bflags = 0u;
                  k++;
               }
            }
            else
            {
               while(i < b->num_elements)
                  d[k++].yacCopy(&b->elements[i++]);
            }

            free();

            elements = d;
            num_elements = max_elements = neAB;
         }
         else
         {
            printf("[---] YAC_StringArray::join: failed to allocate %u elements.\n", neAB);
            return YAC_FALSE;
         }
      }
      else
      {
         YAC_StringArray::free();
      }
   }
   else
   {
      // Generic
      sUI neA;
      if(YAC_VALID(_a))
         neA = _a->yacArrayGetNumElements();
      else
         neA = 0u;

      sUI neB;
      if(YAC_VALID(_b))
         neB = _b->yacArrayGetNumElements();
      else
         neB = 0u;

      sUI neAB = (neA + neB);
      if(neAB > 0u)
      {
         YAC_String *d = new YAC_String[neAB];

         if(NULL != d)
         {
            sUI k = 0u;
            sUI i;

            i = 0u;
            if((void*)_a == (void*)this)
            {
               YAC_CAST_ARG(YAC_StringArray, a, _a);

               while(i < neA)
               {
                  YAC_String *as = &a->elements[i++];
                  d[k].key    = as->key;
                  d[k].buflen = as->buflen;
                  d[k].bflags = as->bflags; // take ownership
                  d[k].chars  = as->chars;
                  d[k].length = as->length;
                  as->buflen = 0u;
                  as->bflags = 0u;
                  k++;
               }
            }
            else
            {
               while(i < neA)
               {
                  YAC_Value r;
                  _a->yacArrayGet(NULL/*context*/, i++, &r);
                  r.typecast(YAC_TYPE_STRING);
                  d[k++] = r.value.string_val;
                  r.unsetFast();
               }
            }

            i = 0u;
            if((void*)_b == (void*)this)
            {
               YAC_CAST_ARG(YAC_StringArray, b, _b);

               while(i < neB)
               {
                  YAC_String *bs = &b->elements[i++];
                  d[k].key    = bs->key;
                  d[k].buflen = bs->buflen;
                  d[k].bflags = bs->bflags; // take ownership
                  d[k].chars  = bs->chars;
                  d[k].length = bs->length;
                  bs->buflen = 0u;
                  bs->bflags = 0u;
                  k++;
               }
            }
            else
            {
               while(i < neB)
               {
                  YAC_Value r;
                  _b->yacArrayGet(NULL/*context*/, i++, &r);
                  r.typecast(YAC_TYPE_STRING);
                  d[k++] = r.value.string_val;
                  r.unsetFast();
               }
            }

            free();

            elements = d;
            num_elements = max_elements = neAB;
         }
         else
         {
            printf("[---] YAC_StringArray::join: failed to allocate %u elements.\n", neAB);
            return YAC_FALSE;
         }
      }
      else
      {
         YAC_StringArray::free();
      }
   }

   return YAC_TRUE;
}

sUI YAC_StringArray::_getMaxChars(void) const {
   // Return max length _without_ ASCIIZ
   sUI r = 0u;
   for(sUI i = 0u; i < num_elements; i++)
   {
      sUI l = elements[i].length;
      if(l > 0u)
         l--;  // asciiz
      if(l > r)
         r = l;
   }
   return r;
}

void YAC_StringArray::_mergeToString(YAC_String *_separatorOrNull, YAC_Value *_r) const {
   _r->initEmptyString();
   YAC_String *sep = NULL;
   if(YAC_Is_String(_separatorOrNull))
   {
      if(_separatorOrNull->getNumChars() > 0u)
         sep = _separatorOrNull;
   }
   for(sUI i = 0u; i < num_elements; i++)
   {
      if( i > 0u && NULL != sep )
         _r->value.string_val->append(sep);
      _r->value.string_val->append(&elements[i]);
   }
}
