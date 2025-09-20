/// YAC_IntArray.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "YAC_IntArray.h"
#include "TKS_IntArrayIterator.h"


YAC_IntArray::YAC_IntArray(void) {
   class_ID     = YAC_CLID_INTARRAY;
   max_elements = 0u;
   num_elements = 0u;
   elements     = NULL;
   own_data     = YAC_FALSE;
}

YAC_IntArray::~YAC_IntArray() {
   YAC_IntArray::free();
}

sBool YAC_VCALL YAC_IntArray::yacToString(YAC_String *_s) const {
   _s->alloc(2+num_elements*7+1);
   _s->empty();
   _s->append("[");
   sUI i;
   if(num_elements)
   {
      YAC_String s; s.yacValueOfI(elements[0]);
      _s->append(&s);
   }
   for(i=1; i<num_elements; i++)
   {
      _s->append(", ");
      YAC_String s; s.yacValueOfI(elements[i]);
      _s->append(&s);
   }
   _s->append("]");
   return 1;
}

void YAC_IntArray::_getString(YAC_Value *_r) const {
   _r->initEmptyString();
   yacToString(_r->value.string_val);
}

void YAC_IntArray::_getString2(YAC_Value *_r) const {
   _r->initEmptyString();
   YAC_String *s = _r->value.string_val;
   s->append("[");
   char buf[64];
   sUI ne = num_elements >> 1;
   sUI j = 0u;
   for(sUI i = 0u; i < ne; i++)
   {
      if(i > 0u)
         s->append(", ");
      Dyac_snprintf(buf, 64, "(%d;%d)", elements[j+0u], elements[j+1u]);
      s->append(buf);
      j += 2u;
   }
   s->append("]");
}

void YAC_IntArray::_getString3(YAC_Value *_r) const {
   _r->initEmptyString();
   YAC_String *s = _r->value.string_val;
   s->append("[");
   char buf[96];
   sUI ne = (num_elements / 3u);
   sUI j = 0u;
   for(sUI i = 0u; i < ne; i++)
   {
      if(i > 0u)
         s->append(", ");
      Dyac_snprintf(buf, 64, "(%d;%d;%d)", elements[j+0u], elements[j+1u], elements[j+2u]);
      s->append(buf);
      j += 3u;
   }
   s->append("]");
}

void YAC_VCALL YAC_IntArray::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();

   YAC_SERIALIZE_I32(num_elements);
   sUI i;
   for(i=0; i<num_elements; i++)
   {
      YAC_SERIALIZE_I32(elements[i]);
   }
}

sUI YAC_VCALL YAC_IntArray::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();

   sUI ne = YAC_DESERIALIZE_I32();

   if(ne > 0)
   {
      if(alloc(ne))
      {
         num_elements=ne;
         sUI i;
         for(i=0; i<num_elements; i++)
         {
            elements[i]=YAC_DESERIALIZE_I32();
         }
      }
      else
      {
         /// ---- out of memory, skip stream ----
         _ifs->yacStreamSeek(ne*4, YAC_CUR);
      }
   }
   else
   {
      free();
   }

   return 1;
}

void YAC_IntArray::free(void) {
   if(elements)
   {
      if(own_data)
      {
         delete [] elements;
         own_data = YAC_FALSE;
      }
      elements     = NULL;
      num_elements = 0u;
      max_elements = 0u;
   }
}

sBool YAC_IntArray::alloc(sUI _maxElements) {
   YAC_IntArray::free();
   if(_maxElements > 0u)
   {
      if(_maxElements > TKS_MAX_INTARRAYALLOCSIZE)
      {
         Dprintf("[---] IntArray::insane array size (maxelements=%08x)\n", _maxElements);
         return YAC_FALSE;
      }
      elements = new(std::nothrow) sS32[_maxElements];
      if(elements)
      {
         max_elements = _maxElements;
         num_elements = 0u;
         own_data     = YAC_TRUE;
         return YAC_TRUE;
      }
      else
      {
         Dprintf("\n[---] IntArray: failed to allocate %u elements\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // alloc(0) -> free()
      return YAC_TRUE;
   }
}

sBool YAC_VCALL YAC_IntArray::yacIteratorInit(YAC_Iterator *_it) const {
   Diteratorinit(TKS_IntArrayIterator);
   return YAC_TRUE;
}

void YAC_VCALL YAC_IntArray::yacArrayCopySize(YAC_Object *_intarray) {
   YAC_IntArray::alloc(((YAC_IntArray*)_intarray)->max_elements);
}

void YAC_VCALL YAC_IntArray::yacArraySetWidth(sUI _num) {
   if(_num <= max_elements)
   {
      num_elements = _num;
   }
}

void YAC_VCALL YAC_IntArray::yacArraySet(void *_context, sUI _index, YAC_Value *_value) {
   if(_value->type != YAC_TYPE_INT)
   {
      _value->typecast(YAC_TYPE_INT);
   }
   if(((sUI)_index) < max_elements)
   {
      if(((sUI)_index) >= num_elements) // track highest access point
      {
         num_elements = _index + 1;
      }
      elements[_index] = _value->value.int_val;
   }
   else
   {
      if(tkscript->configuration.b_autoresizearrays && (((sSI)_index) >= 0))
      {
         if(YAC_IntArray::realloc(tks_grow_array(_index+1)))
         {
            elements[_index] = _value->value.int_val;
            num_elements = _index + 1;
            return;
         }
      }
      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("IntArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS, msg);
   }
}

void YAC_VCALL YAC_IntArray::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   if(((sUI)_index)<max_elements)
   {
      _r->initInt(elements[_index]);
   }
   else
   {
      _r->initVoid();
      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ///msg->alloc(96);
      msg->printf("IntArray index >>%i<< out of bounds (max=%i)", _index, max_elements);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

sSI YAC_IntArray::get(sSI _index) {
   if( ((sUI)_index) < num_elements )
   {
      return elements[_index];
   }
   else
   {
      return 0;
   }
}

void YAC_IntArray::_set(sUI _idx, sSI _val) {
   if(_idx < max_elements)
   {
      elements[_idx] = _val;

      if(_idx >= num_elements)
         num_elements = _idx + 1u;
   }
}

sSI YAC_IntArray::_getRev(sSI _index) {
   sUI idx = ((sUI)(num_elements - 1 - _index));
   if(idx < max_elements)
   {
      return elements[idx];
   }
   else
   {
      return 0;
   }
}

void YAC_IntArray::_setRev(sSI _index, sSI _val) {
   const sUI idx = ((sUI)(num_elements - 1 - _index));
   if(idx < max_elements)
   {
      elements[idx] = _val;
   }
}

sSI YAC_IntArray::_getDefault(sUI _index, sSI _def) {
   return (_index < num_elements) ? elements[_index] : _def;
}

sBool YAC_VCALL YAC_IntArray::yacArrayAlloc(sUI _maxelements, sUI,sUI,sUI) {
   if(alloc(_maxelements))
      return (NULL != elements);
   return YAC_FALSE;
}

sBool YAC_IntArray::realloc(sUI _maxElements) {
   if(_maxElements > 0u)
   {
      sSI *ne = new(std::nothrow) sSI[_maxElements];
      if(ne)
      {
         sUI i = 0u;
         if(_maxElements <= max_elements)
         {
            for(; i < _maxElements; i++)
               ne[i] = elements[i];
         }
         else
         {
            for(; i < max_elements; i++)
               ne[i] = elements[i];
         }
         if(own_data)
         {
            delete [] elements;
         }
         elements     = ne;
         own_data     = YAC_TRUE;
         max_elements = _maxElements;
         if(num_elements > max_elements)
            num_elements = max_elements;
         return YAC_TRUE;
      }
      else
      {
         Dprintf("\n[---] failed to realloc IntArray to %u\n", _maxElements);
         return YAC_FALSE;
      }
   }
   else
   {
      // realloc(0) -> free()
      YAC_IntArray::free();
      return YAC_TRUE;
   }
}

sBool YAC_VCALL YAC_IntArray::yacArrayRealloc(sUI _maxelements, sUI,sUI,sUI) {
   if(realloc(_maxelements))
      return (NULL != elements);
   return YAC_FALSE;
}

sBool YAC_IntArray::_isEqual(YAC_Object *_o) {
   if(YAC_BCHK(_o, YAC_CLID_INTARRAY))
   {
      YAC_CAST_ARG(YAC_IntArray, ia, _o);

      if(ia->num_elements == num_elements)
      {
         sUI i;
         for(i = 0u; i < num_elements; i++)
         {
            if(ia->elements[i] != elements[i])
               return YAC_FALSE;
         }
         return YAC_TRUE;
      }
   }
   else if(YAC_VALID(_o))
   {
      // Compare to any array object (cast elements to int)
      sUI ne = _o->yacArrayGetNumElements();
      if(ne == num_elements)
      {
         for(sUI i = 0u; i < ne; i++)
         {
            YAC_Value t;
            yacArrayGet(NULL/*ctx*/, i, &t);
            t.typecast(YAC_TYPE_INT);
            if(t.value.int_val != elements[i])
            {
               t.unsetFast();
               return YAC_FALSE;
            }
            t.unsetFast();
         }
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

void YAC_VCALL YAC_IntArray::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o!=((YAC_Object*)this))
   {
      if(YAC_OP_ASSIGN == _cmd)
      {
         if(YAC_BCHK(_o, YAC_CLID_INTARRAY))
         {
            YAC_IntArray *o=(YAC_IntArray*)_o;

            // Only reallocate if array does not own data or new data does not fit
            sBool bRealloc = !own_data || (max_elements < o->num_elements);

            if(bRealloc)
               YAC_IntArray::realloc(o->num_elements);

            num_elements = o->num_elements;
            for(sUI i = 0u; i < num_elements; i++)
               elements[i] = o->elements[i];

            return;
         }
         else if(YAC_VALID(_o))
         {
            if(_o->yacArrayGetNumElements())
            {
               sUI ne = _o->yacArrayGetNumElements();
               if(YAC_IntArray::realloc(ne)) // XXX optimize no keep elements
               {
                  num_elements = ne;
                  for(sUI i = 0u; i < ne; i++)
                  {
                     YAC_Value r;
                     _o->yacArrayGet(NULL, i, &r); // xxx TKS_MT use *real* thread context (exceptions!)
                     r.typecast(YAC_TYPE_INT);
                     elements[i] = r.value.int_val;
                  }
               }
               // xxx print OOM error
            }
            return;
         }
      }
      else if(YAC_OP_CEQ == _cmd)
      {
         _r->initInt(_isEqual(_o));
         return;
      }
      else if(YAC_OP_CNE == _cmd)
      {
         _r->initInt(!_isEqual(_o));
         return;
      }
      else if(YAC_OP_ADD == _cmd)
      {
         _join(this, _o);
         return;
      }
      else if(YAC_OP_AND == _cmd)
      {
         _r->initInt(-1 != _findSubArrayOffset(_o));
         return;
      }
   }
   YAC_Object::yacOperator(_cmd, _o, _r);
}

sSI YAC_IntArray::getMaxElements(void) {
   return max_elements;
}

sSI YAC_IntArray::getNumElements(void) {
   return num_elements;
}

sBool YAC_IntArray::add(sSI _intval) {
   if(num_elements < max_elements)
   {
      elements[num_elements++] = _intval;
      return YAC_TRUE;
   }
   else
   {
      sUI oldNumElements = num_elements;
      YAC_Value v;
      v.initInt(_intval);
      yacArraySet(NULL, num_elements, &v); // xxx TKS_MT: should use *real* thread context (exceptions)
      return (num_elements != oldNumElements);
   }
}

sBool YAC_IntArray::add2(sSI _val1, sSI _val2) {
   if((num_elements + 1u) < max_elements)
   {
      elements[num_elements++] = _val1;
      elements[num_elements++] = _val2;
      return YAC_TRUE;
   }
   else
   {
      sSI r = add(_val1);
      if(r)
         r = add(_val2);
      return r;
   }
}

sBool YAC_IntArray::add3(sSI _val1, sSI _val2, sSI _val3) {
   if((num_elements + 2u) < max_elements)
   {
      elements[num_elements++] = _val1;
      elements[num_elements++] = _val2;
      elements[num_elements++] = _val3;
      return YAC_TRUE;
   }
   else
   {
      sSI r = add(_val1);
      if(r)
         r = add(_val2);
      if(r)
         r = add(_val3);
      return r;
   }
}

sBool YAC_IntArray::add4(sSI _val1, sSI _val2, sSI _val3, sSI _val4) {
   if((num_elements + 3u) < max_elements)
   {
      elements[num_elements++] = _val1;
      elements[num_elements++] = _val2;
      elements[num_elements++] = _val3;
      elements[num_elements++] = _val4;
      return YAC_TRUE;
   }
   else
   {
      sSI r = add(_val1);
      if(r)
         r = add(_val2);
      if(r)
         r = add(_val3);
      if(r)
         r = add(_val4);
      return r;
   }
}

sBool YAC_IntArray::addArray(YAC_Object *_a, sUI _off, sUI _len) {
   if(TKS_VALID(_a))
   {
      sUI ne = _a->yacArrayGetNumElements();
      if(_off < ne)
      {
         if( (_off + _len) > ne )
            _len = (ne - _off);
         while(_len-- > 0)
         {
            YAC_Value r;
            _a->yacArrayGet(NULL, _off++, &r);
            r.typecast(YAC_TYPE_INT);
            add(r.value.int_val);
         }
         return YAC_TRUE;
      }
   }
   else
   {
      Dyac_throw_def(INVALIDPOINTER, "_array is not a valid Object");
   }
   return YAC_FALSE;
}

sSI YAC_IntArray::addEmpty(sUI _num) {
   if( (num_elements+_num) <= max_elements)
   {
      num_elements += _num;
      return (num_elements);
   }
   else
   {
      return 0;
   }
}

void YAC_IntArray::empty(void) {
   num_elements = 0u;
}

sBool YAC_IntArray::isEmpty(void) {
   return (0u == num_elements);
}

sSI YAC_IntArray::insert(sSI _index, sSI _val) {
   sUI index = (sUI) _index;
   if(index >= num_elements)
   {
      if(index == num_elements)
      {
         return add(_val);
      }
      else
      {
         Dprintf("[---] IntArray::insert: Cannot insert beyond array boundaries (index=%i, numElements=%i).\n", index, num_elements);
         return YAC_FALSE;
      }
   }
   // Check if current array can hold new element
   if(num_elements == max_elements)
   {
      if(! YAC_IntArray::realloc(tks_grow_array(num_elements+1)) )
      {
         // re-allocation failed
         return YAC_FALSE;
      }
   }
   // Shift elements
   for(sUI i = num_elements; i > index; i--)
      elements[i] = elements[i - 1u];
   elements[index] = _val;
   num_elements++;
   return YAC_TRUE;
}

sBool YAC_IntArray::_delete(sSI _index) {
   if( ((sUI)_index) < num_elements )
   {
      if(num_elements > 1u)
      {
         for(sUI i = ((sUI)_index); i < (num_elements - 1u); i++)
            elements[i] = elements[i + 1u];
      }
      num_elements--;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_IntArray::_delete2(sSI _index) {
   if(((sUI)_index + 1u) < num_elements)
   {
      if(num_elements > 2u)
      {
         for(sUI i = ((sUI)_index); i < (num_elements - 2u); i++)
            elements[i] = elements[i + 2u];
      }
      num_elements -= 2u;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sSI YAC_IntArray::_delete3(sSI _index) {
   if(((sUI)_index + 2u) < num_elements)
   {
      if(num_elements > 3u)
      {
         for(sUI i = sUI(_index); i < (num_elements - 3u); i++)
            elements[i] = elements[i + 3u];
      }
      num_elements -= 3u;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_IntArray::_delete4(sSI _index) {
   if((sUI(_index) + 3u) < num_elements)
   {
      if(num_elements > 4u)
      {
         for(sUI i = ((sUI)_index); i < (num_elements - 4u); i++)
            elements[i] = elements[i + 4u];
      }
      num_elements -= 4u;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

void YAC_IntArray::fill(sSI _c) {
   if(elements && num_elements)
   {
      if(0 == _c)
      {
         ::memset((void*)elements, 0, num_elements * sizeof(sSI));
      }
      else
      {
         for(sUI i = 0u; i < num_elements; i++)
            elements[i] = _c;
      }
   }
}

sBool YAC_IntArray::allocAndFill(sUI _num, sSI _c) {
   sBool r = (alloc(_num) > 0);
   if(r)
   {
      num_elements = max_elements;
      fill(_c);
   }
   return r;
}

void YAC_IntArray::fillRegion(sSI _off, sSI _len, sSI _val) {
   // Clip start
   if(_off < 0)
   {
      _len += _off;
      _off = 0;
   }

   // Clip end
   if(sUI(_off) < num_elements)
   {
      if(sUI(_len) <= num_elements)
      {
         if( (_off + _len) > sSI(num_elements) )
            _len = sSI(num_elements) - _off;

         // Fill
         if(_len > 0)
         {
            sSI *d = elements + _off;
            const sUI len = sUI(_len);
            for(sUI i = 0u; i < len; i++)
               *d++ = _val;
         }
      }
   }
}

void YAC_IntArray::copyFrom(YAC_Object *_ia, sSI _off, sSI _len, sSI _doff) {
   if(YAC_BCHK(_ia,YAC_CLID_INTARRAY))
   {
      sUI off  = (sUI)_off;
      sUI doff = (sUI)_doff;
      sUI len  = (sUI)_len;

      YAC_IntArray *ia = (YAC_IntArray*)_ia;
      sUI opl = (off + len);
      if( (doff+len) <= max_elements )
      {
         if(opl <= ia->max_elements)
         {
            sUI i = doff;
            sUI j = off;
            while(j < opl)
               elements[i++] = ia->elements[j++];
         }
      }
   }
}

void YAC_IntArray::reverse(void) {
   if(num_elements && elements)
   {
      sS32 *s = elements;
      sSI   l = num_elements >> 1;
      sS32 *d = &elements[num_elements - 1u];
      sSI t;
      for(; l >= 0; l--)
      {
         t    = *s;
         *s++ = *d;
         *d-- = t;
      }
   }
}

void YAC_IntArray::setNumElements(sSI _ne) {
   if(sUI(_ne) <= max_elements)
      num_elements = _ne;
}

void YAC_IntArray::swapByteOrder(void) {
   if(elements)
   {
      // xxxx 4 byte ints !!
      sUI *e = (sUI*)elements;
      for(sUI i = 0u; i < num_elements; i++)
      {
         sUI c = *e;
         *e++=
            ((c&255u)<<24)     |
            (((c>>8)&255u)<<16)|
            (((c>>16)&255u)<<8)|
            (((c>>24)&255u))   ;
      }
   }
}

sBool YAC_IntArray::visitBytes(YAC_Object *_a, sSI _offbytes, sSI _numbytes) {
   if(_a)
   {
      if(1 == _a->yacArrayGetElementType())
      {
         sUI  ne = _a->yacArrayGetWidth();
         sUI   z = _a->yacArrayGetElementByteSize();
         sUI max = ne * z * _a->yacArrayGetHeight();

         if(1 == z || 4 == z) // 8bit / 32 bit
         {	// ---- stride must equal width*z (linear memory access) ----
            if( (ne*z) == _a->yacArrayGetStride())
            {
               sU8 *dat = (sU8*)_a->yacArrayGetPointer();
               if(dat && ( ( sUI(_numbytes) + sUI(_offbytes) ) <= max ) )
               {
                  YAC_IntArray::free();
                  elements = (sSI*) (&dat[_offbytes]); /// xxx can create "odd" pointers
                  num_elements = max_elements = _numbytes >> 2;
                  return YAC_TRUE;
               }
            }
         }
      }
   }
   return YAC_FALSE;
}

sSI YAC_IntArray::elementExists(sSI _v) {
   // deprecated
   return _contains(_v);
}

sBool YAC_IntArray::_contains(sSI _v) {
   return (-1 != indexOf(_v, 0));
}

sBool YAC_IntArray::_remove(sSI _v) {
   sSI idx = indexOf(_v, 0);
   if(-1 != idx)
   {
      _delete(idx);
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sSI YAC_IntArray::indexOf(sSI _v, sSI _off) {
   sUI i = (sUI)_off;
   for(; i < num_elements; i++)
   {
      if(elements[i] == _v)
         return (sSI)i;
   }
   return -1;
}

sSI YAC_IntArray::indexOfNot(sSI _v, sSI _off) {
   sUI i = (sUI)_off;
   for(; i < num_elements; i++)
   {
      if(elements[i] != _v)
         return (sSI)i;
   }
   return -1;
}

sUI YAC_IntArray::_read8(YAC_Object *_ifs, sUI _num, sUI _doff) {
   sUI num;
   if(_ifs)
   {
      if(!_num)
      {
         sUI streamOff = _ifs->yacStreamGetOffset();
         num = _ifs->yacStreamGetSize() - streamOff;
      }
      else
      {
         num = _num;
      }

      if( (_doff + num) > max_elements )
      {
         if(!YAC_IntArray::realloc(_doff+num))
            return 0u;
      }
      sUI  i = 0u;
      sSI *e = elements + _doff;
      do
      {
         *e++ = _ifs->yacStreamReadI8();
      }
      while( (++i < num) && !_ifs->yacStreamEOF() );
      if( (_doff + i) > num_elements )
      {
         num_elements = _doff + i;
      }
      return i;
   }
   return 0u;
}

sUI YAC_IntArray::_read16(YAC_Object *_ifs, sUI _num, sUI _doff) {
   sUI num;
   if(_ifs)
   {
      if(!_num)
      {
         sUI streamOff = _ifs->yacStreamGetOffset();
         sUI bytesleft = _ifs->yacStreamGetSize() - streamOff;
         num = bytesleft >> 1;
      }
      else
      {
         num = _num;
      }

      if( (_doff + num) > max_elements )
      {
         if(!YAC_IntArray::realloc(_doff + num))
            return 0u;
      }
      sUI  i = 0u;
      sSI *e = elements + _doff;
      do
      {
         *e++ = _ifs->yacStreamReadI16();
      }
      while( (++i < num) && !_ifs->yacStreamEOF() );
      if( (_doff + i) > num_elements )
      {
         num_elements = _doff + i;
      }
      return i;
   }
   return 0u;
}

sUI YAC_IntArray::_read32(YAC_Object *_ifs, sUI _num, sUI _doff) {
   sUI num;
   if(_ifs)
   {
      if(!_num)
      {
         sUI streamOff = _ifs->yacStreamGetOffset();
         sUI bytesleft = _ifs->yacStreamGetSize() - streamOff;
         num = bytesleft >> 2;
      }
      else
      {
         num = _num;
      }
      if( (_doff + num) > max_elements )
      {
         if(!YAC_IntArray::realloc(_doff + num))
            return 0u;
      }
      sUI  i = 0u;
      sSI *e = elements + _doff;
      do
      {
         *e++ = _ifs->yacStreamReadI32();
      }
      while ((++i < num) && !_ifs->yacStreamEOF() );
      if( (_doff + i) > num_elements )
         num_elements = _doff + i;
      return i;
   }
   return 0u;
}

sUI YAC_IntArray::_write8(YAC_Object *_ofs, sUI _num, sUI _soff) {
   if(_ofs)
   {
      sUI num;
      if(!_num)
         num = num_elements;
      else
         num = _num;

      if( (_soff + num) > max_elements )
         num = max_elements;

      sUI  i = 0u;
      sSI *e = elements + _soff;
      do
      {
         _ofs->yacStreamWriteI8((sU8)*e++);
      }
      while( (++i < num) && !_ofs->yacStreamGetErrorCode() );
      return i;
   }
   return 0u;
}

sUI YAC_IntArray::_write16(YAC_Object *_ofs, sUI _num, sUI _soff) {
   if(_ofs)
   {
      sUI num;
      if(!_num)
         num = num_elements;
      else
         num = _num;
      if( (_soff + num) > max_elements )
         num = max_elements;
      sUI  i = 0u;
      sSI *e = elements + _soff;
      do
      {
         _ofs->yacStreamWriteI16((sU16)*e++);
      }
      while( (++i < num) && !_ofs->yacStreamGetErrorCode() );
      return i;
   }
   return 0u;
}

sUI YAC_IntArray::_write32(YAC_Object *_ofs, sUI _num, sUI _soff) {
   if(_ofs)
   {
      sUI num;
      if(!_num)
         num = num_elements;
      else
         num = _num;
      if( (_soff + num) > max_elements )
         num = max_elements;
      sUI  i = 0u;
      sSI *e = elements + _soff;
      do
      {
         _ofs->yacStreamWriteI32(*e++);
      }
      while( (++i < num) && !_ofs->yacStreamGetErrorCode() );
      return i;
   }
   return 0u;
}

sSI YAC_IntArray::getMin(void) {
   if(num_elements)
   {
      sSI min = 2147483647; // INT_MAX
      for(sUI i = 0u; i < num_elements; i++)
      {
         sSI t = elements[i];
         if(t < min)
            min = t;
      }
      return min;
   }
   else
   {
      return 0;
   }
}

sSI YAC_IntArray::getMax(void) {
   if(num_elements)
   {
      sSI max = (-2147483647-1); // INT_MIN
      for(sUI i = 0u; i < num_elements; i++)
      {
         sSI t = elements[i];
         if(t > max)
            max = t;
      }
      return max;
   }
   else
   {
      return 0;
   }
}

sSI YAC_IntArray::getAbsMin(void) {
   if(num_elements)
   {
      sSI min = 2147483647; // INT_MAX
      for(sUI i = 0u; i < num_elements; i++)
      {
         sSI t = elements[i];
         if(t >= 0)
         {
            if(t < min)
               min = t;
         }
         else
         {
            if((-t) < min)
               min = -t;
         }
      }
      return min;
   }
   else
   {
      return 0;
   }
}

sSI YAC_IntArray::getAbsMax(void) {
   if(num_elements)
   {
      sSI max = (-2147483647-1); // INT_MIN
      for(sUI i = 0u; i < num_elements; i++)
      {
         sSI t = elements[i];
         if(t >= 0)
         {
            if(t > max)
               max = t;
         }
         else
         {
            if((-t) > max)
               max = -t;
         }
      }
      return max;
   }
   else
   {
      return 0;
   }
}

sF32 YAC_IntArray::getMean(void) {
   if(num_elements)
   {
      sF64 avg = 0.0;
      for(sUI i = 0; i < num_elements; i++)
         avg += (sF64)elements[i];
      return (sF32)(avg / (sF64)num_elements);
   }
   else
   {
      return 0.0f;
   }
}

sF32 YAC_IntArray::getAbsMean(void) {
   if(num_elements)
   {
      sF64 avg = 0.0;
      for(sUI i = 0u; i < num_elements; i++)
      {
         sSI t = elements[i];
         if(t >= 0)
            avg += (sF64)t;
         else
            avg -= (sF64)t;
      }
      return (sF32)(avg / (sF64)num_elements);
   }
   else
   {
      return 0.0f;
   }
}

void YAC_IntArray::identity(sSI _num) {
   if(realloc((sUI)_num))
   {
      num_elements = (sUI) _num;
      for(sUI i = 0u; i < num_elements; i++)
         elements[i] = i;
   }
}

sBool YAC_IntArray::rearrange(YAC_Object *_ia) {
   if(YAC_VALID(_ia))
   {
      if(YAC_Is_IntArray(_ia))
      {
         YAC_IntArray *ia = (YAC_IntArray*) _ia;
         if( (ia->num_elements == num_elements) && (ia->num_elements > 1u) && (num_elements > 1u) )
         {
            sSI *ti = new(std::nothrow) sSI[num_elements];
            if(ti)
            {
               for(sUI i = 0u; i < num_elements; i++)
               {
                  sUI srcId = ia->elements[i];
                  if(srcId > num_elements)
                  {
                     Dprintf("[---] IntArray::rearrange: source index (%i) out of bounds.\n", (sSI) srcId);
                     return YAC_FALSE;
                  }
                  ti[i] = elements[srcId];
               }

               for(sUI i = 0u; i < num_elements; i++)
                  elements[i] = ti[i];

               delete [] ti;

               return YAC_TRUE;
            }
            else
            {
               Dprintf("[---] IntArray::rearrange: failed to allocate %i indices.\n", num_elements);
            }
         }
      }
   }
   return YAC_FALSE;
}

void YAC_IntArray::sortByValue(YAC_Object *_ia) {
   if(YAC_BCHK(_ia, YAC_CLID_INTARRAY))
   {
      YAC_IntArray *ia = (YAC_IntArray*)_ia;
      if(elements && num_elements)
      {
         if(ia->realloc(num_elements))
         {
            ia->num_elements = num_elements;
            for(sUI i = 0u; i < num_elements; i++)
               ia->elements[i] = i;
            quicksortByValue((sSI*)ia->elements, 0/*l*/, sSI(num_elements - 1u)/*r*/);
         }
      }
      else
      {
         ia->empty();
      }
   }
}

void YAC_IntArray::quicksortByValue(sSI *_ia, sSI _l, sSI _r) {
   sSI m = elements[ _ia[(_l + _r) >>1] ];
   sSI i = _l, j = _r, ti;
   while(i <= j)
   {
      while(elements[_ia[i]] < m)
         i++;

      while(m < elements[_ia[j]])
         j--;

      if(i <= j)
      {
         ti       = _ia[j];
         _ia[j--] = _ia[i];
         _ia[i++] = ti;
      }
   }

   if(_l < j)
      quicksortByValue(_ia, _l, j);

   if(i < _r)
      quicksortByValue(_ia, i, _r);
}

sBool YAC_IntArray::_swap(sSI _indexs, sSI _indexd) {
   sUI idxs = (sUI) _indexs;
   sUI idxd = (sUI) _indexd;
   if(idxs != idxd)
   {
      if(idxs < num_elements)
      {
         if(idxd < num_elements)
         {
            sSI t;
            t = elements[idxd];
            elements[idxd] = elements[idxs];
            elements[idxs] = t;
            return YAC_TRUE;
         }
      }
   }
   return YAC_FALSE;
}

void YAC_IntArray::_visit(YAC_Object *_ia, sSI _off, sSI _len) {
   YAC_IntArray::free();
   if(YAC_VALID(_ia))
   {
      if(YAC_BCHK(_ia, YAC_CLID_INTARRAY))
      {
         YAC_IntArray *ia = (YAC_IntArray *)_ia;
         sUI off = (sUI) _off;
         sUI len = (sUI) _len;
         if(off < ia->num_elements)
         {
            own_data = YAC_FALSE;
            elements = ia->elements + off;
            if( (off+len) <= ia->num_elements )
               num_elements = max_elements = len;
            else
               num_elements = max_elements = ia->num_elements - off;
         }
      }
   }
}

void YAC_IntArray::_useAll(void) {
   num_elements = max_elements;
}

sSI YAC_IntArray::_getFirst(void) {
   if(num_elements > 0u)
      return elements[0];
   return 0;
}

sSI YAC_IntArray::_getLast(void) {
   if(num_elements > 0u)
      return elements[num_elements - 1u];
   return 0;
}

sBool YAC_IntArray::_join(YAC_Object *_a, YAC_Object *_b) {
   if(YAC_Is_IntArray(_a) && YAC_Is_IntArray(_b))
   {
      // Join IntArrays
      YAC_CAST_ARG(YAC_IntArray, a, _a);
      YAC_CAST_ARG(YAC_IntArray, b, _b);

      sUI neAB = (a->num_elements + b->num_elements);

      if(neAB > 0u)
      {
         sSI *d = new(std::nothrow) sSI[neAB];

         if(NULL != d)
         {
            sUI k = 0u;
            sUI i;

            i = 0u;
            while(i < a->num_elements)
               d[k++] = a->elements[i++];

            i = 0u;
            while(i < b->num_elements)
               d[k++] = b->elements[i++];

            free();
            elements = d;
            own_data = YAC_TRUE;
            num_elements = max_elements = neAB;
         }
         else
         {
            Dprintf("[---] YAC_IntArray::join: failed to allocate %u elements.\n", neAB);
            return YAC_FALSE;
         }
      }
      else
      {
         YAC_IntArray::free();
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
         sSI *d = new(std::nothrow) sSI[neAB];

         if(NULL != d)
         {
            sUI k = 0u;
            sUI i;

            i = 0u;
            while(i < neA)
            {
               YAC_Value r;
               _a->yacArrayGet(NULL/*context*/, i++, &r);
               r.typecast(YAC_TYPE_INT);
               d[k++] = r.value.int_val;
               r.unsetFast();
            }

            i = 0u;
            while(i < neB)
            {
               YAC_Value r;
               _b->yacArrayGet(NULL/*context*/, i++, &r);
               r.typecast(YAC_TYPE_INT);
               d[k++] = r.value.int_val;
               r.unsetFast();
            }

            free();
            elements = d;
            own_data = YAC_TRUE;
            num_elements = max_elements = neAB;
         }
         else
         {
            Dprintf("[---] YAC_IntArray::join: failed to allocate %u elements.\n", neAB);
            return YAC_FALSE;
         }
      }
      else
      {
         YAC_IntArray::free();
      }

   }

   return YAC_TRUE;
}

sSI YAC_IntArray::_replace(sSI _a, sSI _b) {
   sUI i = 0u;
   sUI r = 0u;
   for(; i<num_elements; i++)
   {
      if(elements[i] == _a)
      {
         elements[i] = _b;
         r++;
      }
   }
   return r;
}

sSI YAC_IntArray::_replaceNeg(sSI _b) {
   sUI i = 0u;
   sUI r = 0u;
   for(; i < num_elements; i++)
   {
      if(elements[i] < 0)
      {
         elements[i] = _b;
         r++;
      }
   }
   return r;
}

void YAC_IntArray::_inc(sUI _idx) {
   if(_idx < num_elements)
      elements[_idx]++;
}

void YAC_IntArray::_dec(sUI _idx) {
   if(_idx < num_elements)
      elements[_idx]--;
}

sSI YAC_IntArray::_getNumUniqueValues(void) {
   sSI ret = 0;

   if(num_elements > 0u)
   {
      if(num_elements > 1u)
      {
         for(sUI idx = 0u; idx < num_elements; idx++)
         {
            sSI val = elements[idx];

            sBool bFound = YAC_FALSE;

            for(sUI innerIdx = (idx + 1u); innerIdx < num_elements; innerIdx++)
            {
               if(val == elements[innerIdx])
               {
                  bFound = YAC_TRUE;
                  break;
               }
            }

            if(!bFound)
               ret++;
         }
      }
      else
      {
         ret = 1;
      }
   }

   return ret;
}

sSI YAC_IntArray::_findSubArrayOffset(YAC_Object *_ia) {
   // e.g. [1,2,3,4,5].findSubArrayOffset([3,4]) == 2
   if(YAC_Is_IntArray(_ia))
   {
      YAC_CAST_ARG(YAC_IntArray, ia, _ia);

      if(ia->num_elements <= num_elements)
      {
         if(num_elements > 0u)
         {
            if(ia->num_elements > 0u)
            {
               sSI oFirst = ia->elements[0];
               sUI startOff = 0u;
               for(;;)
               {
                  sSI idxFirst = indexOf(oFirst, startOff);
                  if(-1 != idxFirst)
                  {
                     sUI i = sUI(idxFirst);
                     if( (i + ia->num_elements) <= num_elements )
                     {
                        sBool bMatch = YAC_TRUE;
                        sUI j = 0u;
                        while(j < ia->num_elements)
                        {
                           if(elements[i++] != ia->elements[j++])
                           {
                              bMatch = YAC_FALSE;
                              break;
                           }
                        }
                        if(bMatch)
                           return sSI(idxFirst);
                        startOff++;
                     }
                     else
                     {
                        return -1;
                     }
                  }
                  else
                  {
                     return -1;
                  }
               }
            }
         }
      }
   }
   return -1;
}
