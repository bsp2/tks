/// YAC_ValueArray.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "YAC_ValueArray.h"
#include "TKS_ValueArrayIterator.h"


YAC_ValueArray::YAC_ValueArray(void) {
   class_ID     = YAC_CLID_VALUEARRAY;
   max_elements = 0u;
   num_elements = 0u;
   elements     = NULL;
}

YAC_ValueArray::~YAC_ValueArray() {
   YAC_ValueArray::free();
}

sBool YAC_VCALL YAC_ValueArray::yacToString(YAC_String *_s) const {
   _s->alloc(2u + num_elements * 32u + 1u);
   _s->empty();
   _s->append("[");
   sUI i;
   YAC_String s;
   for(i=0; i<num_elements; i++)
   {
      if(i)
      {
         _s->append(", ");
      }
      switch(elements[i].type)
      {
      case 1:
         {
            s.yacValueOfI(elements[i].value.int_val);
            _s->append(&s);
         }
         break;
      case 2:
         {
            s.yacValueOfF32(elements[i].value.float_val);
            _s->append(&s);
         }
         break;
      case 3:
      case 4:
         if(YAC_VALID(elements[i].value.object_val))
         {
            s.empty();
            elements[i].value.object_val->yacToString(&s);
            _s->append(&s);
         }
         else
         {
            _s->append("<null>");
         }
         break;
      default:
         _s->append("<void>");
         break;
      }
   }
   _s->append("]");
   return 1;
}

void YAC_ValueArray::_getString(YAC_Value *_r) const {
   _r->initEmptyString();
   yacToString(_r->value.string_val);
}

sBool YAC_VCALL YAC_ValueArray::yacIteratorInit(YAC_Iterator *_it) const {
   Diteratorinit(TKS_ValueArrayIterator);
   return 1;
}

void YAC_VCALL YAC_ValueArray::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();

   YAC_SERIALIZE_I32(num_elements);
   YAC_SERIALIZE_I32(max_elements);
   if(num_elements)
   {
      /// ---- serialize all objects that are read-write, read-only pointers become "0" ----
      sUI i;
      for(i=0; i<num_elements; i++)
      {
         YAC_SERIALIZE_I8((sU8)elements[i].type); // type (0,1,2,3,4)

         switch(elements[i].type)
         {
         case YAC_TYPE_VOID:
            break;

         case YAC_TYPE_INT:
            YAC_SERIALIZE_I32(elements[i].value.int_val); // value
            break;

         case YAC_TYPE_FLOAT:
            YAC_SERIALIZE_F32(elements[i].value.float_val); // value
            break;

         case YAC_TYPE_OBJECT:
         case YAC_TYPE_STRING:
            {
               sBool bSerialized = 0;
               if(YAC_VALID(elements[i].value.object_val))
               {
                  if( (!elements[i].value.object_val->yacIsComposite()) || elements[i].deleteme )
                  {
                     YAC_SERIALIZE_I8(1); // tag "in-use"
                     elements[i].value.object_val->yacSerialize(_ofs, 1);
                     bSerialized = 1;
                  }
               }
               if(!bSerialized)
               {
                  YAC_SERIALIZE_I8(0); // unused (NULL pointer)
               }
            }
            break;
         }
      }
   }
}

sUI YAC_VCALL YAC_ValueArray::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();

   YAC_ValueArray::free();

   sUI ne=YAC_DESERIALIZE_I32();
   sUI me=YAC_DESERIALIZE_I32();

   if(sUI(me) >= tkscript->configuration.streamMaxArraySize)
   {
      Dprintf("[---] ValueArray::deserialize: insane array size (maxElements) (%i>%i)\n",
         me, tkscript->configuration.streamMaxArraySize);
      return 0u;
   }

   if(sUI(ne) >= tkscript->configuration.streamMaxArraySize)
   {
      Dprintf("[---] ValueArray::deserialize: insane array size (numElements) (%i>%i)\n",
         ne, tkscript->configuration.streamMaxArraySize);
      return 0u;
   }

   // ---- allocate new array ----
   if(!YAC_ValueArray::alloc(me))
   {
      Dprintf("[---] ValueArray::deserialize: failed to allocate %i elements.\n", me);
      return 0u;
   }

   /// ---- now deserialize elements / add null pointers ----
   if(ne)
   {
      num_elements = ne;
      sUI i;
      for(i = 0u; i < ne; i++)
      {
         sU8 type=YAC_DESERIALIZE_I8();
         switch(type)
         {
         case YAC_TYPE_VOID:
            elements[i].initVoid();
            break;
         case YAC_TYPE_INT:
            elements[i].initInt(YAC_DESERIALIZE_I32());
            break;
         case YAC_TYPE_FLOAT:
            elements[i].initFloat(YAC_DESERIALIZE_F32());
            break;
         case YAC_TYPE_OBJECT:
         case YAC_TYPE_STRING:
            {
               sU8 bused=YAC_DESERIALIZE_I8();
               if(bused)
               {
                  YAC_Object *o = tkscript->deserializeNewObject(_ifs, 0);
                  if(o)
                  {
                     // ---- remember object ----
                     elements[i].initObject(o,1);
                     elements[i].type=type;
                  }
                  else
                  {
                     // ---- failed to create new object ----
                     Dprintf("[---] ValueArray: error deserializing object, index=%i\n", i);
                     return 0;
                  }
               }
               else
               {
                  elements[i].initObject(NULL, 0);
               }
            }
            break;
         }
      }
   }
   // ---- numElements should now equal ne ----
   return 1;
}

void YAC_VCALL YAC_ValueArray::yacArrayCopySize(YAC_Object *_array) {
   YAC_ValueArray *o=(YAC_ValueArray*)_array;
   YAC_ValueArray::alloc(o->max_elements);
}

void YAC_VCALL YAC_ValueArray::yacArraySet(void *_context, sUI _index, YAC_Value *_value) {
   if(((sUI)_index)>=max_elements)
   {
      if(tkscript->configuration.b_autoresizearrays && (((sSI)_index) >= 0))
      {
         if(YAC_ValueArray::realloc(tks_grow_array(_index+1)))
         {
            num_elements = _index + 1;
            goto YAC_ValueArray_yacarrayset_1;
         }
      }

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      msg->printf("ValueArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS, msg);
      return;
   }

   if(((sUI)_index) >= num_elements) // track highest access point
   {
      num_elements = _index + 1u;
   }

YAC_ValueArray_yacarrayset_1:
   elements[_index].copySafe(_value);
}

void YAC_VCALL YAC_ValueArray::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   if(((sUI)_index)<max_elements)
   {
      _r->initValue(&elements[_index]); // return read-only objects
   }
   else
   {
      _r->initVoid();

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      msg->printf("ValueArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

void YAC_VCALL YAC_ValueArray::yacArrayGetDeref(void *_context, sUI _index, YAC_Value *_r) {
   if(sUI(_index) < max_elements)
   {
      _r->fastDerefValue(&elements[_index]); // return read-write objects
   }
   else
   {
      _r->initVoid();

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      //msg->alloc(96);
      msg->printf("ValueArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

void YAC_ValueArray::get(sSI _index, YAC_Value *_r) {
   if(((sUI)_index) < max_elements)
   {
      _r->initValue(&elements[_index]); // return read-only objects
   }
   else
   {
      _r->initVoid();
   }
}

void YAC_ValueArray::getDeref(sSI _index, YAC_Value *_r) {
   if(sUI(_index) < max_elements)
   {
      _r->fastDerefValue(&elements[_index]); // return read-write objects
   }
   else
   {
      _r->initVoid();
   }
}

void YAC_ValueArray::_set(sUI _idx, YAC_Object *_val) {
   if(_idx < max_elements)
   {
      if(YAC_VALID(_val))
      {
         if(YAC_Is_Boolean(_val)         ||
            YAC_Is_UnsignedByte(_val)    ||
            YAC_Is_UnsignedShort(_val)   ||
            YAC_Is_UnsignedInteger(_val) ||
            YAC_Is_Byte(_val)            ||
            YAC_Is_Short(_val)           ||
            YAC_Is_Integer(_val)         ||
            YAC_Is_Long(_val)
            )
         {
            sSI ival;
            if(_val->yacScanI(&ival))
            {
               elements[_idx].safeInitInt(ival);
            }
            else
            {
               elements[_idx].safeInitInt(0);
            }
         }
         else if(YAC_Is_Float(_val) ||
                 YAC_Is_Double(_val)
                 )
         {
            sF32 fval;
            if(_val->yacScanF32(&fval))
            {
               elements[_idx].safeInitFloat(fval);
            }
            else
            {
               elements[_idx].safeInitFloat(0.0f);
            }
         }
         else if(YAC_Is_Value(_val))
         {
            YAC_CAST_ARG(YAC_ValueObject, valObj, _val);
            elements[_idx].copySafe(valObj);  // transfer ownership if possible
         }
         else
         {
            // Store object ref
            elements[_idx].safeInitObject(_val, YAC_FALSE);
         }
      }
      else
      {
         elements[_idx].unset();
      }

      if(_idx >= num_elements)
      {
         num_elements = _idx + 1u;
      }
   }
}

void YAC_ValueArray::_getRev(sSI _index, YAC_Value *_r) {
   sUI idx = ((sUI)(num_elements - 1 - _index));
   if(idx < max_elements)
   {
      _r->initValue(&elements[idx]); // return read-only objects
   }
   else
   {
      _r->initVoid();
   }
}

void YAC_ValueArray::_setRev(sSI _index, YAC_Object *_val) {
   const sUI idx = ((sUI)(num_elements - 1 - _index));
   if(idx < max_elements)
   {
      _set(idx, _val);
   }
}

void YAC_VCALL YAC_ValueArray::yacArraySetWidth(sUI _num) {
   if(_num <= max_elements)
   {
      num_elements = _num;
   }
}

sBool YAC_VCALL YAC_ValueArray::yacArrayAlloc(sUI _maxelements, sUI,sUI,sUI) {
   return YAC_ValueArray::alloc(_maxelements);
}

sBool YAC_ValueArray::alloc(sUI _maxElements) {
   YAC_ValueArray::free();
   if(_maxElements > 0u)
   {
      if(_maxElements > TKS_MAX_ARRAYALLOCSIZE)
      {
         tkscript->printf("[---] ValueArray::insane array size (maxelements=%08x)\n", _maxElements);
         return YAC_FALSE;
      }
      elements = new(std::nothrow) YAC_Value[_maxElements];
      if(elements)
      {
         max_elements = _maxElements;
         num_elements = 0u;
         return YAC_TRUE;
      }
      else
      {
         Dprintf("[---] ValueArray: failed to allocate %i elements\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // alloc(0) -> free()
      return YAC_TRUE;
   }
}

sBool YAC_ValueArray::realloc(sUI _maxElements) {
   if(_maxElements > 0u)
   {
      YAC_Value *ne = new(std::nothrow) YAC_Value[_maxElements];
      if(ne)
      {
         sUI i = 0u;
         if(_maxElements <= max_elements)
         {
            // ---- shrink ----
            for(; i < _maxElements; i++)
            {
               ne[i].copySafe(&elements[i]); // copy elements
            }
            for(; i < max_elements; i++)
            {
               elements[i].unset(); // unset superfluous elements
            }
         }
         else
         {
            // ---- expand ----
            for(; i < max_elements; i++)
            {
               ne[i].copySafe(&elements[i]); // copy existing region
            }
            for(; i < _maxElements; i++) // init new region
            {
               ne[i].initInt(0);
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
         Dprintf("\n[--] failed to realloc ValueArray to %u\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // realloc(0) -> free()
      YAC_ValueArray::free();
      return YAC_TRUE;
   }
}

void YAC_VCALL YAC_ValueArray::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o!=((YAC_Object*)this))
   {
      if(_cmd==YAC_OP_ASSIGN)
      {
         if(YAC_VALID(_o))
         {
            /// ---- generic array copy by references ----
            sUI ne = _o->yacArrayGetNumElements();
            if(YAC_ValueArray::alloc(ne))
            {
               sUI i = 0;
               for(; i<ne; i++)
               {
                  elements[i].unset(); // Do not rely on yacArrayGet to unset the value before re-assignment!
                  _o->yacArrayGet(NULL, i, &elements[i]);  // xxx TKS_MT: should use real thread context (exceptions!)
               }
               num_elements = ne;
               max_elements = ne;
            }
            return;
         }
      }
      else if(YAC_OP_ADD == _cmd)
      {
         _joinCC(this, _o);
      }
   }
   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_ValueArray::free(void) {
   if(elements)
   {
      for(sUI i = 0u; i < max_elements; i++)
      {
         elements[i].unset();
      }
      delete [] elements;
      elements     = NULL;
      num_elements = 0u;
      max_elements = 0u;
   }
}

sUI YAC_ValueArray::getMaxElements(void) {
   return max_elements;
}

sUI YAC_ValueArray::getNumElements(void) {
   return num_elements;
}

void YAC_ValueArray::setNumElements(sUI _nr) {
   if(_nr <= max_elements)
   {
      num_elements = _nr;
   }
}

sSI YAC_ValueArray::indexOfPointer(YAC_Object*_o, sSI _off) {
   sUI i = (sUI) _off;
   for(; i<num_elements; i++)
   {
      if(elements[i].type >= YAC_TYPE_OBJECT)
      {
         if( ((void*)elements[i].value.object_val) == ((void*)_o) )
         {
            // Ok, found object address
            return (sSI) i;
         }
      }
   }
   // Failed, address not found
   return -1;
}

sSI YAC_ValueArray::indexOfObject(YAC_Object *_o, sSI _off) {
   sUI i = (sUI) _off;
   for(; i<num_elements; i++)
   {
      if(elements[i].type >= YAC_TYPE_OBJECT)
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
   }
   return -1;
}


sSI YAC_ValueArray::containsObject(YAC_Object *_o) {
   return (-1 != indexOfObject(_o, 0));
}

sSI YAC_ValueArray::containsPointer(YAC_Object *_o) {
   return (-1 != indexOfPointer(_o, 0));
}

void YAC_ValueArray::empty(void) {
   num_elements = 0u;
}

sBool YAC_ValueArray::isEmpty(void) {
   return (0u == num_elements);
}

sBool YAC_ValueArray::_add(YAC_Object *_valueObject) {
   if(YAC_Is_Value(_valueObject))
   {
      YAC_CAST_ARG(YAC_ValueObject, vo, _valueObject);

      realloc(num_elements + 1u);
      elements[num_elements++] = vo;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_ValueArray::_addVoid(void) {
   return realloc(++num_elements);
}

sBool YAC_ValueArray::_insert(sSI _index, YAC_Object *_valueObject) {
   if(_index >= 0 && sUI(_index) <= num_elements)
   {
      if(YAC_Is_Value(_valueObject))
      {
         YAC_CAST_ARG(YAC_ValueObject, vo, _valueObject);

         realloc(num_elements + 1u);
         num_elements++;

         for(sUI i = num_elements - 1u; i > sUI(_index); i--)
         {
            elements[i] = elements[i - 1];
         }

         elements[_index] = vo;
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

sBool YAC_ValueArray::_delete(sSI _index) {
   if((_index >= 0) && (sUI(_index) < num_elements))
   {
      YAC_Value *d = &elements[_index];
      d->unset();

      sUI i = sUI(_index);
      while(i < (num_elements-1))
      {
         elements[i].copySafe(&elements[i + 1]);
         i++;
      }
      elements[num_elements-1].unset();
      num_elements--;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

void YAC_ValueArray::reverse(void) {
   if(num_elements && elements)
   {
      YAC_Value *s = elements;
      sSI l = (num_elements) >> 1;
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

sSI YAC_ValueArray::_swap(sSI _indexs, sSI _indexd) {
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
            return 1;
         }
      }
   }
   return 0;
}


void YAC_ValueArray::_useAll(void) {
   num_elements = max_elements;
}

void YAC_ValueArray::_getFirst(YAC_Value *_r) {
   if(num_elements > 0u)
   {
      _r->initValue(&elements[0]); // return read-only objects
   }
   else
   {
      _r->initVoid();
   }
}

void YAC_ValueArray::_getLast(YAC_Value *_r) {
   if(num_elements > 0u)
   {
      _r->initValue(&elements[num_elements - 1u]);
   }
   else
   {
      _r->initVoid();
   }
}

YAC_Value *YAC_ValueArray::joinArray(YAC_Value *_d, YAC_Object *_a, sUI _neA, sBool _bCopy, sBool _bUnlink) {
   sUI i = 0u;
   while(i < _neA)
   {
      YAC_Value r;

      if(_bUnlink)
      {
         _a->yacArrayGetDeref(NULL/*context*/, i++, &r);

         *_d = &r;
      }
      else
      {
         _a->yacArrayGet(NULL/*context*/, i++, &r);

         if(r.type < YAC_TYPE_OBJECT)
         {
            *_d = &r;
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

sBool YAC_ValueArray::_joinArrays(YAC_Object *_a, YAC_Object *_b, sBool _bCopyA, sBool _bCopyB, sBool _bUnlinkA, sBool _bUnlinkB) {
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
      // // _bCopyA = 0;
      _bUnlinkA = 1;
   }

   if((void*)this == (void*)_b)
   {
      // // _bCopyB = 0;
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
         printf("[---] YAC_ValueArray::joinArrays: failed to allocate %u elements.\n", neAB);
         return YAC_FALSE;
      }
   }
   else
   {
      YAC_ValueArray::free();
   }

   return YAC_TRUE;
}

sBool YAC_ValueArray::_joinCC(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 1/*bCopyA*/, 1/*bCopyB*/, 0/*bUnlinkA*/, 0/*bUnlinkB*/);
}

sBool YAC_ValueArray::_joinCR(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 1/*bCopyA*/, 0/*bCopyB*/, 0/*bUnlinkA*/, 0/*bUnlinkB*/);
}

sBool YAC_ValueArray::_joinCD(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 1/*bCopyA*/, 0/*bCopyB*/, 0/*bUnlinkA*/, 1/*bUnlinkB*/);
}

sBool YAC_ValueArray::_joinRR(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 0/*bCopyB*/, 0/*bUnlinkA*/, 0/*bUnlinkB*/);
}

sBool YAC_ValueArray::_joinRC(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 1/*bCopyB*/, 0/*bUnlinkA*/, 0/*bUnlinkB*/);
}

sBool YAC_ValueArray::_joinRD(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 0/*bCopyB*/, 0/*bUnlinkA*/, 1/*bUnlinkB*/);
}

sBool YAC_ValueArray::_joinDD(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 0/*bCopyB*/, 1/*bUnlinkA*/, 1/*bUnlinkB*/);
}

sBool YAC_ValueArray::_joinDC(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 1/*bCopyB*/, 1/*bUnlinkA*/, 0/*bUnlinkB*/);
}

sBool YAC_ValueArray::_joinDR(YAC_Object *_a, YAC_Object *_b) {
   return _joinArrays(_a, _b, 0/*bCopyA*/, 0/*bCopyB*/, 1/*bUnlinkA*/, 0/*bUnlinkB*/);
}
