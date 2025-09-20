/// YAC_Buffer.cpp
///
/// (c) 2002-2025 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include <string.h>
#include "tks.h"
#include "PTN_Node.h"
#include "YAC_Buffer.h"

#ifdef DX_Z
extern "C" {
#include <zlib.h>
}
#endif


YAC_Buffer::YAC_Buffer(void) { 
   class_ID  = YAC_CLID_BUFFER; 
   size      = 0u; 
   io_offset = 0u; 
   buffer    = NULL; 
   deleteme  = YAC_FALSE;
} 
 
YAC_Buffer::~YAC_Buffer(void) { 
   free(); 
} 

sBool YAC_VCALL YAC_Buffer::yacEquals(YAC_Object *_o) { 
   if(YAC_Is_Buffer(_o)) 
   { 
      YAC_CAST_ARG(YAC_Buffer, o, _o);
      if(io_offset == o->io_offset)
      {
         for(sUI i = 0u; i < io_offset; i++)
         {
            if(buffer[i] != o->buffer[i])
               return YAC_FALSE;
         }
         return YAC_TRUE;
      }
   } 
   return YAC_FALSE; 
} 
 
sBool YAC_VCALL YAC_Buffer::yacArrayRealloc(sUI _sx, sUI _sy, sUI _type, sUI _elementbytesize) {

   (void)_sy;
   (void)_type;
   (void)_elementbytesize;

   if(_sx > 0u)
   {
      if(_sx > size)
      {
         sU8 *d = new(std::nothrow) sU8[_sx];

         if(NULL != d)
         {
            if(buffer)
            {
               if(_sx>size)
               {
                  sUI i;
                  for(i = 0u; i < size; i++)
                     d[i] = buffer[i];
               }

               if(YAC_FALSE != deleteme)
               {
                  delete [] buffer;
               }
            }
            buffer   =  d;
            deleteme =  YAC_TRUE;
            size     = _sx;
            if(io_offset > size)
            {
               io_offset = size;
            }
            return YAC_TRUE;
         }
      }
      else
      {
         // No re-allocation required
         size = _sx;
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

void YAC_VCALL YAC_Buffer::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o)
   {
      if(_o != ((YAC_Object*)this))
      {
         switch(_cmd)
         {
            case YAC_OP_SHL:
               _o->yacSerialize(this, YAC_TRUE/*bUseTypeInfo*/);
               return;

            case YAC_OP_ASSIGN:
               yacOperatorAssign(_o);
               return;

            case YAC_OP_CEQ:
               _r->initInt(yacEquals(_o));
               return;

            case YAC_OP_CNE:
               _r->initInt(!yacEquals(_o));
               return;
         }
      }
      else 
      {
         if(YAC_OP_CEQ == _cmd)
         {
            _r->initInt(1);
            return;
         }
         if(YAC_OP_CNE == _cmd)
         {
            _r->initInt(0);
            return;
         }
      }
   }
   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_VCALL YAC_Buffer::yacOperatorAssign(YAC_Object *_robj) {
   if(YAC_BCHK(_robj, YAC_CLID_BUFFER))
   {
      YAC_CAST_ARG(YAC_Buffer, obuf, _robj);

      if(size != obuf->size)
      {
         setSize(obuf->size);
      }

      io_offset = obuf->io_offset;

      memcpy(buffer, obuf->buffer, obuf->size);
   }
}

void YAC_VCALL YAC_Buffer::yacGetConstantStringList(YAC_String *_s) {
   _s->visit("LITTLE_ENDIAN:0 BIG_ENDIAN:1 ");
}

void YAC_VCALL YAC_Buffer::yacArraySet(void *_context, sUI _index, YAC_Value *_value) {
   if(((sUI)_index) >= size)
   { 
      if(tkscript->configuration.b_autoresizearrays && (((sSI)_index) >= 0))
      { 
         if(YAC_Buffer::resize(tks_grow_array(_index+1))) 
         { 
            io_offset = _index + 1u; 
            goto yacbufferstream_yacarrayset_1; 
         } 
      } 

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      //msg->alloc(96);
      msg->printf("Buffer index >>%i<< out of bounds (max=%i)", _index, size);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS, msg);
      return; 
   }
   if(((sUI)_index) >= io_offset) // track highest access point
   {
      io_offset = _index + 1u; 
   }

yacbufferstream_yacarrayset_1:

   if(_value->type != YAC_TYPE_INT) 
   { 
      _value->typecast(YAC_TYPE_INT); 
   } 
   buffer[_index] = (sU8) _value->value.int_val;
}

void YAC_VCALL YAC_Buffer::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   if(((sUI)_index)<size) 
   {
      _r->initInt(buffer[_index]);
   }
   else
   {
      _r->initVoid(); 
      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("Buffer index >>%i<< out of bounds (max=%i)", _index, size);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

void YAC_VCALL YAC_Buffer::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   YAC_SERIALIZE_I32(size);
   if(size > 0u)
   {
      for(sUI i = 0u; i < size; i++)
      {
         YAC_SERIALIZE_I8(buffer[i]);
      }
   }
}

sUI YAC_VCALL YAC_Buffer::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   sUI nsize = YAC_DESERIALIZE_I32();
   if(nsize > 0u)
   {
      // ---- allocate new buffer ----
      if(setSize(nsize))
      {
         for(sUI i = 0u; i < nsize; i++)
         {
            // ---- deserialize a byte ;) ----
            buffer[i]=YAC_DESERIALIZE_I8();
         }
      }
      else
      {
         // ---- out of mem, skip data -----
         _ifs->yacStreamSeek(nsize, YAC_CUR);
      }
   }
   else
   {
      YAC_Buffer::free();
   }
   return 1u;
}
 
sBool YAC_VCALL YAC_Buffer::yacArrayAlloc(sUI _sx, sUI, sUI, sUI) { 
   return setSize((sSI)_sx); 
}

sBool YAC_Buffer::setSize(sUI _num) {
   if(!deleteme || size != _num)
   {
      YAC_Buffer::free();

      if(_num > 0u)
      {
         buffer = new(std::nothrow) sU8[_num];
         if(buffer)
         {
            deleteme  = YAC_TRUE;
            io_offset = 0u;
            size      = _num;
            return YAC_TRUE;
         }
      }
   }
   else
   {
      // size already allocated
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_Buffer::resize(sUI _num) {
   if(!deleteme || size != _num)
   {
      if(_num > 0u)
      {
         if(buffer)
         {
            /** buffer already exists, realloc **/
            sU8 *nb = new(std::nothrow) sU8[_num];
            if(nb)
            {
               sUI i = 0u;
               if(_num >= size)
               {
                  /** new buffer is >= old buffer **/
                  /** copy old buffer content **/
                  for(; i < size; i++) nb[i] = buffer[i];
                  /** fill remaining bytes with 0 **/
                  for(; i < _num; i++) nb[i] = 0;
               }
               else
               {
                  /** new buffer is < old buffer, discard bytes **/
                  if(io_offset >= _num) io_offset = _num - 1u;
                  for(; i < _num; i++) nb[i] = buffer[i];
               }

               if(YAC_FALSE != deleteme)
               {
                  delete [] buffer;
               }

               buffer   = nb;
               deleteme = YAC_TRUE;
               size     = _num;

               return YAC_TRUE;
            }
         }
         else
         {
            /** realloc=alloc+zero **/
            if(setSize(_num))
            {
               fillZero();
               return YAC_TRUE;
            }
         }
      }
   }
   else 
   {
      // size already allocated
      return YAC_TRUE;
   }

   return YAC_FALSE;
}

void YAC_Buffer::truncate(sUI _newSize) {
   // (note) this is for API methods that write 'size' bytes from this buffer (and the initial buffer size is larger than needed)
   if(_newSize <= size)
   {
      size = _newSize;
      if(io_offset > size)
         io_offset = size;
   }
   else
   {
      yac_host->printf("[---] YAC_Buffer::truncate: failed (newSize=%u, size=%u)\n", _newSize, size);
   }
}

void YAC_Buffer::free(void) {
   if(buffer)
   {
      if(YAC_FALSE != deleteme)
      {
         delete [] buffer; 
      }

      buffer = NULL;
   }

   deleteme  = YAC_FALSE;
   size      = 0u;
   io_offset = 0u;
}

sBool YAC_VCALL YAC_Buffer::yacIsStream(void) {
   return 1;
}

sUI YAC_VCALL YAC_Buffer::yacStreamGetSize(void) {
   return size;
}

void YAC_VCALL YAC_Buffer::yacStreamWriteI8(sU8 _value) {
   // bit_offset = 0u;
   if(io_offset < size)
      buffer[((sUI)io_offset++)]=(sU8)_value;
}

void YAC_Buffer::pokeI8(sSI _offset, sSI _value) {
   if( ((sUI)_offset)<size ) buffer[((sUI)_offset)]=(sU8)_value;
   else
   {
      Dprintf("[---] Buffer::pokeI8: index out of bounds!! (offset=%i, size=%i)\n",
         _offset, size);
   }
}

void YAC_VCALL YAC_Buffer::yacStreamWriteI16(sU16 _value) {
   // bit_offset = 0u;
   if((io_offset+1) < size)
   {
      pokeI16(io_offset, _value);
      io_offset += 2;
   }
}

void YAC_Buffer::pokeI16(sSI _offset, sSI _value) {
   if(size>1)
   {
      if( ((sUI)_offset)<(size-1) )
      {
#ifdef YAC_LITTLE_ENDIAN
         if(byteOrder)
         {
            /** little->big **/
            // ---- store high byte
            buffer[((sUI)_offset)]=(sU8)(_value>>8);
            // ---- store low byte
            buffer[((sUI)_offset)+1]=(sU8)(_value);
         }
         else
         {
            /** little->little **/
            *(sU16*)&buffer[((sUI)_offset)]=(sU16)_value;
         }
#else
         if(byteOrder)
         {
            /** big->big **/
            *(sU16*)&buffer[((sUI)_offset)]=(sU16)_value;
         }
         else
         {
            /** big->little **/
            // ---- store low byte
            buffer[((sUI)_offset)]=(sU8)(_value);
            // ---- store high byte
            buffer[((sUI)_offset)+1]=(sU8)(_value>>8);
         }
#endif
         return;
      }
   }
   Dprintf("[---] Buffer::pokeI16: index out of bounds!! (offset=%i, size=%i)\n",
      _offset, size);
   
}

void YAC_Buffer::pokeI14(sSI _offset, sSI _value) {
   pokeI16(_offset,
           (((_value >> 7)&127)<<8) | (_value & 127)
           );
}


void YAC_VCALL YAC_Buffer::yacStreamWriteI32(sSI _value) {
   // bit_offset = 0u;
   if((io_offset + 3u) < size)
   {
      pokeI32(io_offset, _value);
      io_offset += 4u;
   }
}

void YAC_Buffer::pokeI32(sSI _offset, sSI _value) {
   if(size > 3u)
   {
      if(sUI(_offset) < (size - 3u))
      {
#ifdef YAC_LITTLE_ENDIAN
         if(byteOrder)
         {
            /** little->big **/
            // ---- store high byte of high word
            buffer[((sUI)_offset)]=(sU8)(_value>>24);
            // ---- store low byte of high word
            buffer[((sUI)_offset)+1]=(sU8)(_value>>16);
            // ---- store high byte of low word
            buffer[((sUI)_offset)+2]=(sU8)(_value>>8);
            // ---- store low byte of low word
            buffer[((sUI)_offset)+3]=(sU8)(_value);
         }
         else
         {
            /** little->little **/
            *(sU32*)&buffer[((sUI)_offset)]=(sU32)_value;
         }
#else
         if(byteOrder)
         {
            /** big->big **/
            *(sU32*)&buffer[((sUI)_offset)]=(sU32)_value;
         }
         else
         {
            // ---- store high byte of high word
            buffer[((sUI)_offset)+3]=(sU8)(_value>>24);
            // ---- store low byte of high word
            buffer[((sUI)_offset)+2]=(sU8)(_value>>16);
            // ---- store high byte of low word
            buffer[((sUI)_offset)+1]=(sU8)(_value>>8);
            // ---- store low byte of low word
            buffer[((sUI)_offset)+0]=(sU8)(_value);
         }
#endif
         return;
      }
   }
   Dprintf("[---] Buffer::pokeI32: index out of bounds!! (offset=%i, size=%i)\n",
           _offset, size
           );
}

void YAC_VCALL YAC_Buffer::yacStreamWriteF32(sF32 _value) {
   // bit_offset = 0u;
   if((io_offset+3)<size)
   {
      // Handle endianess conversion and write 4 bytes
      YAC_StreamBase::yacStreamWriteF32(_value);
      // // *(sF32*)&buffer[((sUI)io_offset)]=_value;
      // // io_offset+=4;
   }
}

void YAC_Buffer::pokeF32(sSI _offset, sF32 _value) {
   if(size>3)
   {
      if( ((sUI)_offset) < (size-3) )
      {
         // // tkscript->printf("xxx YAC_Buffer::pokeF32: offset=%d value=%f\n", _offset, _value);
         // (note) peek/poke is always in host byte order. use yacStream*() methods for automatic endian conversion instead.
         *(sF32*)&buffer[((sUI)_offset)] = _value;
         return;
      }
   }
   Dprintf("[---] Buffer::pokeF32: index out of bounds!! (offset=%i, size=%i)\n",
      _offset, size);
}

sU8 YAC_VCALL YAC_Buffer::yacStreamReadI8(void) {
   // bit_offset = 0u;
   if(io_offset < size)
      return buffer[((sUI)io_offset++)];
   return 0u;
}

sU8 YAC_Buffer::getI8OF(sSI _offset) {
   if( ((sUI)_offset)<size ) return buffer[((sUI)_offset)];
   return 0u;
}

sSI YAC_Buffer::peekI8(sSI _offset) {
   return (sSI) getI8OF(_offset);
}

sU16 YAC_VCALL YAC_Buffer::yacStreamReadI16(void) {
   // bit_offset = 0u;
   if((io_offset + 1u) < size)
   {
      io_offset += 2u;
      return getI16OF(io_offset - 2u);
   }
   return 0u;
}

sU16 YAC_Buffer::getI16OF(sSI _offset) {
   if(size>1)
   {
      if( ((sUI)_offset)<(size-1) )
      {
         sUI r;
#ifdef YAC_LITTLE_ENDIAN
         if(byteOrder)
         {
            /** big->little **/
            // ---- get high byte
            r=(sUI) (buffer[((sUI)_offset)]<<8);
            // ---- get low byte
            r|=(sUI) (buffer[((sUI)_offset)+1]);
         }
         else
         {
            /** little->little **/
            r=(sUI) *(sU16*)&buffer[((sUI)_offset)];
         }
#else
         if(byteOrder)
         {
            /** big->big **/
            r=(sUI) *(sU16*)&buffer[((sUI)_offset)];
         }
         else
         {
            /** little->big **/
            // ---- get high byte
            r=(sUI) (buffer[((sUI)_offset)+1]<<8);
            // ---- get low byte
            r|=(sUI) (buffer[((sUI)_offset)]);
         }
#endif
         return (sU16) r;
      }
   }
   return 0;
}

sSI YAC_Buffer::peekI14(sSI _offset) {
   sSI r = peekI16(_offset);
   r = ((r>>1)&0x3F80) | (r&127);
   return r;
}

sSI YAC_Buffer::peekI16(sSI _offset) {
   return (sSI)getI16OF(_offset);
}

sU32 YAC_VCALL YAC_Buffer::yacStreamReadI32(void) {
   // bit_offset = 0u;
   if((io_offset + 3u) < size)
   {
      io_offset += 4u;
      return getI32OF(io_offset - 4u);
   }
   return 0u;
}

sU32 YAC_Buffer::getI32OF(sSI _offset) {
   if(size>3)
   {
      if( ((sUI)_offset)<(size-3) )
      {
         sU32 r;
#ifdef YAC_LITTLE_ENDIAN
         if(byteOrder)
         {
            /** big->little **/
            // ---- get high byte of high word
            r=(sU32) (buffer[((sUI)_offset)]<<24);
            // ---- get low byte of high word
            r|=(sU32) (buffer[((sUI)_offset)+1]<<16);
            // ---- get high byte of low word
            r|=(sU32) (buffer[((sUI)_offset)+2]<<8);
            // ---- get low byte of low word
            r|=(sU32) (buffer[((sUI)_offset)+3]);
         }
         else
         {
            /** little->little **/
            r=(sU32) *(sU32*)&buffer[((sUI)_offset)];
         }
#else
         if(byteOrder)
         {
            /** big->big **/
            r=(sU32) *(sU32*)&buffer[((sUI)_offset)];
         }
         else
         {
            /** little->big **/
            // ---- get high byte of high word
            r=(sU32) (buffer[((sUI)_offset)+3]<<24);
            // ---- get low byte of high word
            r|=(sU32) (buffer[((sUI)_offset)+2]<<16);
            // ---- get high byte of low word
            r|=(sU32) (buffer[((sUI)_offset)+1]<<8);
            // ---- get low byte of low word
            r|=(sU32) (buffer[((sUI)_offset)+0]);
         }
#endif
         return r;
      }
   }
   return 0;
}

sSI YAC_Buffer::peekI32(sSI _offset) {
   return getI32OF(_offset);
}

sF32 YAC_VCALL YAC_Buffer::yacStreamReadF32(void) {
   // bit_offset = 0u;
   if((io_offset + 3u) < size)
   {
      // Read 4 bytes and handle endianess conversion
      return YAC_StreamBase::yacStreamReadF32();
      // // io_offset+=4;
      // // return *(sF32*)&buffer[((sUI)io_offset-4)];
   }
   return 0.0f;
}

sF32 YAC_Buffer::getF32OF(sSI _offset) {
   if(size>3)
   {
      if( ((sUI)_offset)<(size-3) )
      {
         return *(sF32*)&buffer[((sUI)_offset)];
      }
   }
#ifdef TKS_DCON
   Dprintf("[---] Buffer::getF32OF: index out of bounds!! (offset=%i, size=%i)\n",
      _offset, size);
#endif
   return 0.0f;
}

sF32 YAC_Buffer::peekF32(sSI _offset) {
   return getF32OF(_offset);
}


void YAC_Buffer::fillZero(void) {
   if(size&&buffer) ::memset((void*)buffer, 0, (size_t)size);
}

void YAC_Buffer::fillI8(sUI _c) {
   if(size && buffer)
   {
      ::memset((void*)buffer, (sU8)_c, (size_t)size);
   }
}

sBool YAC_Buffer::substring(YAC_String *ds, sUI _off, sUI _num) {
   if(_off < size)
   {
      if( ((_off + _num) > size) || !_num )
         _num = size - _off;
      ds->alloc(_num + 1u);
      sU8 *d = ds->chars;
      sU8 *s = buffer;
      sUI i;
      for(i = 0u; i < _num && *s; i++)
      {
         *d++ = *s++;
      }
      *d++ = 0u;
      ds->length = _num + 1u;
      return YAC_TRUE;
   }
   // ---- out of bounds ----
   return YAC_FALSE;
}

void YAC_VCALL YAC_Buffer::yacStreamSetOffset(sUI _off) {
   // bit_offset = 0u;
   if(_off <= size )
   {
      io_offset = _off;
   }
}

sUI YAC_VCALL YAC_Buffer::yacStreamGetOffset(void) {
   return io_offset;
}

void YAC_Buffer::getString(sUI _off, sUI _len, YAC_Value *_r) {
   if(_len > 0u)
   {
      if( (_off + _len) <= size )
      {
         YAC_String *ns = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
         if(ns->alloc(_len + 1u))
         {
            sU8 *s = buffer + _off;
            sU8 *d = ns->chars;
            sUI i = 0u;
            while(i < _len && s[i])
            {
               *d++ = s[i++];
            }
            *d++ = 0u;
            ns->length = i + 1u;
            YAC_RETS(ns, 1);
            return;
         }
         else
         {
            tkscript->printf("[---] Buffer::getString: failed to alloc %u bytes.\n", _len + 1u);
         }
      }
   }
   else
   {
      if(_off < size)
      {
         YAC_String *ns = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
         sU8 *s = buffer + _off;
         // find ASCIIZ
         sUI l = 0u;
         while(l < size && *s++) 
         {
            l++; 
         }
         if(ns->alloc(l + 1u))
         {
            s = buffer + _off;
            sU8 *d = ns->chars;
            for(sUI i = 0u; i < l; i++)
               *d++ = *s++;
            *d++ = 0u; 
            ns->length = l + 1u;
            YAC_RETS(ns, 1);
            return;
         }
         else 
         { 
            Dprintf("[---] Buffer::getString: failed to alloc %u bytes.\n", l + 1u); 
         }
      }
   } 
   YAC_RETS(0, 0); 
}

sBool YAC_Buffer::setString(sUI _off, YAC_String *_st) {
   if(buffer)
   {
      YAC_String *st = (YAC_String*)_st;
      if(st->chars && st->length)
      {
         if((_off + st->length) <= size)
         {
            sU8 *d = buffer + _off;
            sU8 *s = st->chars;
            sUI i;
            for(i = 0u; i < st->length; i++)
            {
               *d++=*s++;
            } 
            if( (_off + st->length) > io_offset )   
            {
               io_offset = (_off + st->length); 
            }
            return YAC_TRUE;
         }
      }
   }
   return YAC_FALSE;
}

sSI YAC_Buffer::gzip(YAC_Object *_src, sSI _off, sSI _len, sSI _level) {
#ifdef DX_Z
   if(YAC_BCHK(_src, YAC_CLID_BUFFER)) 
   {
      YAC_Buffer *src=(YAC_Buffer*)_src;
      if( ((sUI)(_off+_len))<=src->size )
      {
         unsigned long compr_size=(int)(_len*0.12)+_len+120;
         if(yacArrayRealloc(compr_size,0,0,0))
         {
            if(::compress2(buffer, &compr_size, (unsigned char*)src->buffer+_off, _len, _level)==Z_OK)
            {
               size = compr_size;
               io_offset = 0u;
               return YAC_TRUE;
            }
         }
      }
   }
#endif
   return YAC_FALSE;
}

sSI YAC_Buffer::gunzip(YAC_Object *_src, sSI _off, sSI _len, sSI _dlen) {
#ifdef DX_Z
   if(YAC_BCHK(_src, YAC_CLID_BUFFER))
   {
      YAC_Buffer *src=(YAC_Buffer*)_src;
      if( ((sUI)(_off+_len))<=src->size )
      {
         unsigned long uncompr_size=(sUI)_dlen;
         if(yacArrayRealloc(uncompr_size,0,0,0))
         {
            if(::uncompress((unsigned char*)buffer, &uncompr_size, (unsigned char*)src->buffer+_off, (sUI)_len)==Z_OK)
            {
               size = uncompr_size;
               io_offset = 0u;
               return YAC_TRUE;
            }
         }
      }
   }
#endif
   return YAC_FALSE;
}

sSI YAC_Buffer::_getChecksum(void) {
   //
   // TODO: cache checksum..
   //
   sUI key = 0u;
   sUI i;
   sUI j = 1;
   for(i = 0u; i < io_offset; i++)
   {
      key = 53 * key + j * buffer[i]; 
      j += 128;
   }
   return (sSI) key;
}

sBool YAC_Buffer::_compareBuffer(YAC_Buffer *_o) const {
   sBool r = YAC_FALSE;
   if(YAC_Is_Buffer(_o))
   {
      if(_o->io_offset == io_offset)
      {
         r = YAC_TRUE;
         for(sUI i = 0u; i < io_offset; i++)
         {
            if(_o->buffer[i] != buffer[i])
            {
               r = YAC_FALSE;
               break;
            }
         }
      }
   }
   return r;
}

sBool YAC_Buffer::visit(YAC_Buffer *_buffer, sUI _off, sUI _len) {
   free();

   if(YAC_Is_Buffer(_buffer))
   {
      if(_off < _buffer->size)
      {
         if(_len <= _buffer->size)
         {
            if( (_off + _len) <= _buffer->size)
            {
               buffer = _buffer->buffer + _off;
               size   = _len;
               io_offset = 0u;

               return YAC_TRUE;
            }
         }
      }
   }

   return YAC_FALSE;
}

static char base64_enc_table[64] = {
   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',  // 0..15
   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',  // 16..31
   'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',  // 32..47
   'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'   // 48..63
};

static sU8 base64_dec_table[256] = {  // see tks-examples/base64/gen_base64_dec_tbl.tks
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
   0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0,
   0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

sUI YAC_Buffer::base64Encode(YAC_String *_out) {
   sUI r = 0u;
   if(io_offset > 0)
   {
      sUI encSize = ((io_offset+2u)/3u) * 4u;

      if(_out->_realloc(encSize+1u))
      {
         sUI off = 0u;
         sUI tripNum = (io_offset / 3u) * 3u;
         sUI outOff = 0u;
         sU8 *d = _out->chars;

         for(; off <tripNum; off += 3u)
         {
            const sU8 b1 = buffer[off + 0u];
            const sU8 b2 = buffer[off + 1u];
            const sU8 b3 = buffer[off + 2u];
            
            d[outOff++] = base64_enc_table[(b1>>2) & 63u];
            d[outOff++] = base64_enc_table[((b1&3u)<<4) | ((b2>>4)&15u)];
            d[outOff++] = base64_enc_table[((b2&15u)<<2) | ((b3>>6)&3u)];
            d[outOff++] = base64_enc_table[(b3&63u)];
         }

         if(off < io_offset)
         {
            const sU8 b1 = buffer[off++];
            d[outOff++] = base64_enc_table[(b1>>2) & 0x3fu];
            if(off < io_offset)
            {
               const sU8 b2 = buffer[off++];
               d[outOff++] = base64_enc_table[((b1&3u)<<4) | ((b2>>4)&15u)];
               d[outOff++] = base64_enc_table[((b2&15u)<<2)];
               d[outOff++] = '=';
            }
            else
            {
               d[outOff++] = base64_enc_table[((b1&3u)<<4)];
               d[outOff++] = '=';
               d[outOff++] = '=';
            }
         }

         r = outOff;

         _out->length = ++outOff;
         _out->key = YAC_LOSTKEY;
      }
   }
   return r;
}

sUI YAC_Buffer::base64Decode(YAC_String *_in) {
   sUI r = 0u;
   if(_in->length > 1)
   {
      sUI numChars = _in->length-1/*asciiz*/;
      sUI decSize = ((numChars+3)/4) * 3;
      sUI outOff = 0u;

      if(yacArrayRealloc(decSize, 0,0,0))
      {
         sUI quadNum = ((_in->length-1) / 4u) * 4u;
         sUI off = 0u;
         const sU8 *s = _in->chars;

         for(; off < quadNum; off += 4u)
         {
            const sU8 c1 = base64_dec_table[s[off + 0u]];
            const sU8 c2 = base64_dec_table[s[off + 1u]];
            const sU8 c3 = base64_dec_table[s[off + 2u]];
            const sU8 c4 = base64_dec_table[s[off + 3u]];

            buffer[outOff++] = (c1 << 2) | ((c2>>4)&3u);
            if('=' != s[off + 2u])
            {
               buffer[outOff++] = ((c2&15u)<<4) | ((c3>>2)&15u);
               if('=' != s[off + 3u])
               {
                  buffer[outOff++] = ((c3&3u)<<6) | c4;
               }
            }
         }

         r = off;
         io_offset = outOff;
      }
   }
   return r;
}

sUI YAC_Buffer::readBufferBits(sUI _startBit, sUI _numBits) {
   sUI endBit = _startBit + _numBits;
   sUI r = 0u;
   if( ((endBit +7u) >> 3) <= size )
   {
      sUI startByte = _startBit >> 3;
      sUI bitOff = (_startBit - (startByte << 3));  // 0..7
      sUI outShift = _numBits;
      sUI byteOff = startByte;
      sUI bitsLeft = _numBits;
      // yac_host->printf("xxx readBufferBits: startBit=%u numBits=%u startByte=%u bitOff=%u\n", _startBit, _numBits, startByte, bitOff);
      while(bitsLeft > 0u)
      {
         sUI bitsAvail = (8u - bitOff);
         // yac_host->printf("xxx readBufferBits: byteOff=%u bitOff=%u outShift=%u bitsLeft=%u bitsAvail=%u buffer[byteOff]=0x%02x r=0x%04x\n", byteOff, bitOff, outShift, bitsLeft, bitsAvail, buffer[byteOff], r);
         if(bitsLeft < bitsAvail)
         {
            // Last byte
            outShift -= bitsLeft;
            r |= ( (buffer[byteOff] >> (bitsAvail - bitsLeft)) & ((1u << bitsLeft) - 1u) ) << outShift;
            // // outShift += bitsLeft;
            bitsLeft = 0u;
            // yac_host->printf("xxx readBufferBits: r=0x%04x\n", r);
         }
         else
         {
            // 01111111 11111110
            outShift -= bitsAvail;
            r |= (buffer[byteOff] & ((1u << bitsAvail) - 1u)) << outShift;
            bitsLeft -= bitsAvail;
            bitOff = 0u;
            byteOff++;
         }
      }
   }
   return r;
}

void YAC_Buffer::writeBufferBits(sUI _startBit, sUI _numBits, sUI _val) {
   sUI endBit = _startBit + _numBits;
   if( ((endBit +7u) >> 3) <= size )
   {
      sUI startByte = _startBit >> 3;
      sUI bitOff = (_startBit - (startByte << 3));  // 0..7
      sUI outShift = _numBits;
      sUI byteOff = startByte;
      sUI bitsLeft = _numBits;
      // yac_host->printf("xxx writeBufferBits: startBit=%u numBits=%u startByte=%u bitOff=%u\n", _startBit, _numBits, startByte, bitOff);
      while(bitsLeft > 0u)
      {
         sUI bitsAvail = (8u - bitOff);
         // yac_host->printf("xxx writeBufferBits: byteOff=%u bitOff=%u outShift=%u bitsLeft=%u bitsAvail=%u buffer[byteOff]=0x%02x val=0x%04x\n", byteOff, bitOff, outShift, bitsLeft, bitsAvail, buffer[byteOff], _val);
         if(bitsLeft < bitsAvail)
         {
            // Last byte (lowest bits of 'val')
            outShift -= bitsLeft;
            sU32 v = (_val & ((1u << bitsLeft) - 1));
            // yac_host->printf("xxx writeBufferBits:  keep last byte=0x%02x mask=0x%02x\n", ((buffer[byteOff] & ~((1u << (8u - bitsLeft)) - 1u))), ((1u << (8u - bitsLeft)) - 1u));
            buffer[byteOff] = (buffer[byteOff] & ((1u << (8u - bitsLeft)) - 1u)) | (v << (8u - bitsLeft));
            bitsLeft = 0u;
         }
         else
         {
            // 01111111 11111110
            outShift -= bitsAvail;
            const sU8 mask = ~((1u << bitsAvail) - 1u);
            const sU8 keep = (buffer[byteOff] & mask);
            const sU8 rep  = (_val >> outShift);
            // yac_host->printf("xxx writeBufferBits:  keep=0x%02x mask=0x%02x rep=0x%02x\n", keep, mask, rep);
            buffer[byteOff] = keep | rep;
            bitsLeft -= bitsAvail;
            bitOff = 0u;
            byteOff++;
         }
      }
   }
}

void YAC_Buffer::hexdump(sUI _start, sUI _num) {
   if( (_start < size) && (_num <= size) && ((_start + _num) <= size) )
   {
      sUI HEXDUMP_COLS = 16u;
      sUI j, i = _start;
      sU8 c;
      sUI x = 0u;
      sUI y = _start;

      yac_host->printf("%04x: ", (i - _start));

      for(sUI byteIdx = 0u; byteIdx < _num; byteIdx++)
      {
         c = buffer[i++];
         yac_host->printf("%02x ", c);
         if(0u == (++x % HEXDUMP_COLS))
         {
            j = y;
            for(sUI asciiIdx = 0u; asciiIdx < HEXDUMP_COLS; asciiIdx++)
            {
               c = buffer[j++];
               if(c < 32u)
                  c = '.';
               else if(c > 126u)
                  c = '.';
               yac_host->printf("%c", c);
            }
            yac_host->printf("\n");

            if(i < (_start + _num))
            {
               yac_host->printf("%04x: ", (i - _start));
            }
            x = 0u;
            y += HEXDUMP_COLS;
         }
      }

      if(x > 0u)
      {
         for(sUI asciiIdx = 0u; asciiIdx < (3u*(HEXDUMP_COLS-x)); asciiIdx++)
         {
            yac_host->printf(" ");
         }
         j = y;
         for(sUI asciiIdx = 0u; asciiIdx < x; asciiIdx++)
         {
            c = buffer[j++];
            if(c < 32u)
               c = '.';
            else if(c > 126u)
               c = '.';
            yac_host->printf("%c", c);
         }
         yac_host->printf("\n");
      }
   }
}

sUI YAC_Buffer::ringWriteBuffer(sUI _ringStartOff, sUI _ringSz, sUI _writeOff, YAC_Buffer *_src, sUI _srcOff, sUI _numBytes) {
   sUI r = _writeOff;
   if(YAC_Is_Buffer(_src)) 
   { 
      if(_ringSz > 0u && _ringSz <= size)
      {
         if( (_ringStartOff + _ringSz) <= size )
         {
            if(_writeOff < _ringSz)
            {
               if(_srcOff < _src->size)
               {
                  sUI j = _srcOff;
                  sUI k = _writeOff;
                  for(sUI i = 0u; i < _numBytes; i++)
                  {
                     buffer[_ringStartOff + k++] = _src->buffer[j++];
                     if(k >= _ringSz)
                        k = 0u;
                  }
                  r = k;
               }
            }
         }
      }
   }
   return r;
}

sUI YAC_Buffer::ringReadBuffer(sUI _ringStartOff, sUI _ringSz, sUI _readOff, YAC_Buffer *_dst, sUI _dstOff, sUI _numBytes) {
   sUI r = _readOff;
   if(YAC_Is_Buffer(_dst)) 
   { 
      if(_ringSz > 0u && _ringSz <= size)
      {
         if( (_ringStartOff + _ringSz) <= size )
         {
            if(_readOff < _ringSz)
            {
               if(_dstOff < _dst->size)
               {
                  sUI k = _dstOff;
                  sUI j = _readOff;
                  for(sUI i = 0u; i < _numBytes; i++)
                  {
                     _dst->buffer[k++] = buffer[_ringStartOff + j++];
                     if(j >= _ringSz)
                        j = 0u;
                  }
                  r = j;
               }
            }
         }
      }
   }
   return r;
}
