// ----
// ---- file   : inc_yac_native.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2025-2026 by bsp
// ----
// ----          Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// ----          associated documentation files (the "Software"), to deal in the Software without restriction, including
// ----          without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// ----          copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to
// ----          the following conditions:
// ----
// ----          The above copyright notice and this permission notice shall be included in all copies or substantial
// ----          portions of the Software.
// ----
// ----          THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// ----          NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// ----          IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// ----          WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// ----          SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----
// ---- info   :
// ---- note   : must be compiled as C++ code
// ----
// ----
// ----

#include <stdlib.h>  // malloc,free
#include <string.h>  // strlen

#define YAC_NO_EXPORTS
#define YAC_C_STRUCTS  defined
#define YAC_OBJECT_TAGS  0
#define YAC_CUST_OBJECT
#define YAC_CUST_STRING
#define YAC_CUST_EVENT
#define YAC_CUST_VALUE
#define YAC_CUST_NUMBEROBJECTS
#define YAC_CUST_LISTNODE
#define YAC_CUST_TREENODE
#define YAC_CUST_STREAMBASE
#define YAC_CUST_BUFFER
#define YAC_CUST_INTARRAY
#define YAC_CUST_FLOATARRAY

#include <yac.h>

#ifdef MINNIE_LIB
#if defined(__cplusplus)
extern "C" {
typedef void *minnie_allocator_handle_t;
extern void *minnie_alloc (minnie_allocator_handle_t _allocator, sU32 _sz);
extern void minnie_free (minnie_allocator_handle_t _allocator, void *_ptr);
}
#else
typedef void *minnie_allocator_handle_t;
extern void *minnie_alloc (minnie_allocator_handle_t _allocator, sU32 _sz);
extern void minnie_free (minnie_allocator_handle_t _allocator, void *_ptr);
#endif // __cplusplus
#endif // MINNIE_LIB

#if defined(__cplusplus)
struct YAC_String {
#else
typedef struct YAC_String_s {
#endif

#if defined(__cplusplus) && !defined(YAC_C_STRUCTS)
	enum __stringflags {
      YAC_STRING_QUOT   = (1u << 24),  // \"\"
      YAC_STRING_QUOTR  = (1u << 25),  // ~; region quote ~;
      YAC_STRING_QUOT2  = (1u << 26),  // \'\'
      YAC_STRING_DEL    = (1u << 31)   // chars are owned by this string
   };
#else
#define YAC_STRING_QUOT   (1u << 24)  // \"\"
#define YAC_STRING_QUOTR  (1u << 25)  // ~; region quote ~;
#define YAC_STRING_QUOT2  (1u << 26)  // \'\'
#define YAC_STRING_DEL    (1u << 31)
#endif

   sUI    buflen; // ---- total buffer size (0 if chars are not owned by this object)
   sUI    bflags; // ---- stringflags (e.g. DEL flag)
   sUI    length; // ---- number of used chars in buffer
   sUI    key;    // ---- hashkey that is used for fast object comparison
   sU8   *chars;  // ---- pointer to first char in buffer

#if defined(__cplusplus)
   void free(void) {
      if(buflen > 0u)
      {
         if(NULL != chars)
         {
            Dyacfreechars(chars);
            chars = NULL;
         }
         buflen = 0u;
      }
      length = 0u;
      key = YAC_LOSTKEY;
   }

   void visit(const char *_s) {
      free();
      chars  = (sU8*)_s;
      key    = YAC_LOSTKEY;
      length = (sUI)strlen(_s)+1;
      buflen = 0u;
   }

   sBool alloc(sU32 len) {
      if(length != len)
      {
         free();
         if(chars && (bflags & YAC_STRING_DEL))
         {
            Dyacfreechars(chars);
         }
         buflen = len;
         length = len;
         if(len > 0u)
         {
            chars = Dyacallocchars(length);
            if(NULL != chars)
            {
               for(sUI i = 0u; i < length; i++)
                  chars[i] = 0u;
            }
            else
            {
               buflen = 0u;
               bflags = 0u;
               length = 0u;
            }
         }
         else
         {
            chars = NULL;
         }
      }
      key = YAC_LOSTKEY;
      return (NULL != chars);
   }

   sBool realloc(sUI _len) {
      if( NULL == chars || buflen < _len )
      {
         return YAC_String::alloc(_len);
      }
      else
      {
         length = _len;
         key = YAC_LOSTKEY;
         return (0u != length);
      }
   }

   sBool createEmpty(void) {
      sBool ret = YAC_String::alloc(1);
      if(ret)
         *chars = 0u;
      return ret;
   }

   sBool copy(YAC_String *_s) {
      sBool ret;
      if( (ret = (NULL != _s)) )
      {
         if(_s->length)
         {
            ret = YAC_String::realloc(_s->length);
            if(ret)
            {
               for(sUI i = 0u; i < _s->length; i++)
               {
                  chars[i] = _s->chars[i];
               }
               if(_s->bflags & YAC_STRING_QUOT)
               {
                  bflags |= YAC_STRING_QUOT;
               }
            }
         }
         else
         {
            ret = createEmpty();
         }
      }
      return ret;
   }

   sBool copy(const char *_s) {
      if(_s)
      {
         sUI j = (sUI)strlen(_s);
         sBool ret = YAC_String::realloc(j + 1u);
         if(ret)
         {
            sUI i = 0u;
            for(; i < j; i++)
               chars[i] = (sChar)_s[i];
            chars[i] = 0u;
         }
         return ret;
      }
      else
      {
         return createEmpty();
      }
   }

   sBool append(YAC_String *o) {
      if(o && o->chars && o->length)
      {
         sUI k = o->length + length;
         if(buflen < k)
         {
            sUI nl = k;
            sU8 *nc = Dyacallocchars(nl);
            sBool ret = (NULL != nc) ? YAC_TRUE : YAC_FALSE;
            if(ret)
            {
               sUI i = 0u;
               if(chars)
               {
                  if(length > 1u)
                  {
                     for(; i < length - 1u; i++)
                     {
                        nc[i] = chars[i];
                     }
                  }
                  if((bflags & YAC_STRING_DEL))
                  {
                     /* Dyac_host_printf("free old chars 1\n"); */
                     Dyacfreechars(chars);
                     /* Dyac_host_printf("free old chars 2\n"); */
                  }
                  if(length > 0u)
                     length--;
               }
               for(k=0; i < (length/*-1*/+o->length); i++, k++)
               {
                  nc[i] = o->chars[k];
               }
               chars  = nc;
               buflen = nl;
               length = length + o->length;
               key    = YAC_LOSTKEY;
            }
            return ret;
         }
         else
         {
            sU8 *d = chars;
            if(length > 1u)
            {
               d += length - 1u;
               length = k - 1u;
            }
            else
            {
               length = k;
            }
            sUI i = 0u;
            for(; i < o->length; i++)
            {
               /* printf("xxx append i=%u d=\'%c\'\n", i, (char)o->chars[i]); */
               *d++ = o->chars[i];
            }
            if(length == (sU32)-1)
            {
               length = o->length;
            }
            /* printf("xxx append: length=%u\n", length); */
            key = YAC_LOSTKEY;
            return YAC_TRUE;
         }
      }
      return YAC_FALSE;
   }

   sBool append(const char *_s) {
      YAC_String t;
      t.visit(_s);
      return append(&t);
   }

   sSI indexOf(const YAC_String *s, sUI start) const {
      //
      // Only searches substrings char-by-char after first/last character match was found
      //
      if(s && (s->length > 1)) // s!=null and s has at least one character except ASCIIZ ?
      {
         if(length > 1) // not empty / not only ASCIIZ?
         {
            if(s->length == 2) // // search for single character (special case) ?
            {
               const sU8 *cs = s->chars;
               const sU8 fsc = cs[0];
               const sU8 *c = chars + start;
               sUI searchLen = length - 1 - start;

               while(searchLen-- > 0)
               {
                  if(*c++ == fsc)
                  {
                     return (sSI) (c - chars - 1);
                  }
               }

               // Substring not found
               return -1;
            }
            else if( (length - start) >= s->length ) // is this String long enough to possibly contain the substring ?
            {
               const sU8 *cs = s->chars;
               const sU8 *c = chars;
               sUI searchLen = 1 + (length - s->length);
               sU8 fsc = cs[0]; // first character of substring
               sU8 lsc = cs[s->length - 2]; // last character of substring
               sUI lsoff = s->length - 2;
               sUI i = start;

               for(; i<searchLen; i++)
               {
                  if(c[i] == fsc)
                  {
                     // Possible substring start
                     if(c[i + lsoff] == lsc) // last character of substring matches ?
                     {
                        // Compare rest of substring
                        sUI k = 1;
                        const sU8 *t = c + i + 1;

                        while(k < lsoff)
                        {
                           if(*t++ != cs[k++])
                           {
                              k = 0xFFFFFFFFu;
                              break;
                           }
                        }

                        if(k == lsoff)
                        {
                           // Found match
                           return i;
                        }
                     }
                  }
               }
            }
            // else: string is not long enough to contain substring
         }
         // else: string is empty
      }
      // else: substring is empty

      // Substring not found
      return -1;
   }

   void overwriteReplace(YAC_String *_a, YAC_String *_b) {
      // replace 'a' by 'b'. 'a'.length must be >= _b.length
      //  (note) used by FixShaderSourceVert() / FixShaderSourceFrag()
      sUI startIdx = 0u;
      sUI aLen = _a->length;
      sUI bLen = _b->length;
      /* printf("xxx overwriteReplace: aLen=%u \"%s\"  bLen=%u \"%s\"\n", aLen, _a->chars, bLen, _b->chars); */
      if(aLen > 0u && bLen > 0u && aLen >= bLen)
      {
         aLen--;
         bLen--;
         for(;;)
         {
            sSI idx = indexOf(_a, startIdx);
            if(idx >= 0)
            {
               sUI k = sUI(idx);
               sUI i = 0u;
               for(; i < bLen; i++)
               {
                  chars[k++] = _b->chars[i];
               }
               for(; i < aLen; i++)
               {
                  chars[k++] = sChar(' ');
               }
               startIdx += aLen;
            }
            else
            {
               break;
            }
         }
      }
   }

   YAC_String(void) {
      buflen = 0u;
      bflags = 0u;
      length = 0u;
      key    = YAC_LOSTKEY;
      chars  = NULL;
   }

   ~YAC_String() {
   }
#endif // __cplusplus

#if defined(__cplusplus)
};
#else
} YAC_String;
#endif

#if defined(__cplusplus)
struct YAC_Buffer {
#else
typedef struct YAC_Buffer_s {
#endif
	sUI   size;
	sUI   io_offset;
	sU8  *buffer;
   sBool deleteme;

#if defined(__cplusplus)
   void free(void) {
      if(NULL != buffer)
      {
         if(deleteme)
         {
#ifdef MINNIE_LIB
            minnie_free(NULL/*allocator*/, (void*)buffer);
#else
            delete [] buffer;
#endif // MINNIE_LIB
            deleteme = YAC_FALSE;
         }
         buffer = NULL;
      }
      size      = 0u;
      io_offset = 0u;
   }

   YAC_Buffer(void) {
      size      = 0u;
      io_offset = 0u;
      buffer    = NULL;
      deleteme  = YAC_FALSE;
   }

   ~YAC_Buffer() {
      free();
   }

   sBool alloc(sU32 _size) {
      if(!deleteme || size != _size)
      {
         free();
#ifdef MINNIE_LIB
         buffer = (sU8*)minnie_alloc(NULL/*allocator*/, _size);
#else
         buffer = new(std::nothrow)sU8[_size];
#endif // MINNIE_LIB
         if(NULL != buffer)
         {
            deleteme = YAC_TRUE;
            size = _size;
         }
         // else: failed to allocate '_size' bytes
      }
      io_offset = 0u;
      return deleteme;
   }

   void writeI8(sU8 _value) {
      if(io_offset < size)
         buffer[io_offset++] = (sU8)_value;
#ifdef YAC_NATIVE_WARN_BUFFER_OOB
      else
      {
         printf("[---] YAC_Buffer::writeI8: out of bounds (io_offset=%u size=%u)\n", io_offset, size);
      }
#endif // YAC_NATIVE_WARN_BUFFER_OOB
   }

   void writeI16(sU16 _value) {
      if(io_offset + 1u < size)
      {
#ifdef YAC_LITTLE_ENDIAN
         buffer[io_offset++] = (sU8)(_value & 255u);
         buffer[io_offset++] = (sU8)((_value >> 8) & 255u);
#else
         buffer[io_offset++] = (sU8)((_value >> 8) & 255u);
         buffer[io_offset++] = (sU8)(_value & 255u);
#endif // YAC_LITTLE_ENDIAN
      }
#ifdef YAC_NATIVE_WARN_BUFFER_OOB
      else
      {
         printf("[---] YAC_Buffer::writeI16: out of bounds (io_offset=%u size=%u)\n", io_offset, size);
      }
#endif // YAC_NATIVE_WARN_BUFFER_OOB
   }

   void writeI32(sU32 _value) {
      if(io_offset + 3u < size)
      {
#ifdef YAC_LITTLE_ENDIAN
         buffer[io_offset++] = (sU8)(_value & 255u);
         buffer[io_offset++] = (sU8)((_value >> 8) & 255u);
         buffer[io_offset++] = (sU8)((_value >> 16) & 255u);
         buffer[io_offset++] = (sU8)((_value >> 24) & 255u);
#else
         buffer[io_offset++] = (sU8)((_value >> 24) & 255u);
         buffer[io_offset++] = (sU8)((_value >> 16) & 255u);
         buffer[io_offset++] = (sU8)((_value >> 8) & 255u);
         buffer[io_offset++] = (sU8)(_value & 255u);
#endif // YAC_LITTLE_ENDIAN
      }
#ifdef YAC_NATIVE_WARN_BUFFER_OOB
      else
      {
         printf("[---] YAC_Buffer::writeI32: out of bounds (io_offset=%u size=%u)\n", io_offset, size);
      }
#endif // YAC_NATIVE_WARN_BUFFER_OOB
   }

   void writeF32(sF32 _value) {
      if(io_offset + 3u < size)
      {
         yacmem v; v.f32 = _value;
         yacmemptr m; m.u8 = buffer + io_offset;
         m.u8[0] = v.au8[0];
         m.u8[1] = v.au8[1];
         m.u8[2] = v.au8[2];
         m.u8[3] = v.au8[3];
         io_offset += 4u;
      }
#ifdef YAC_NATIVE_WARN_BUFFER_OOB
      else
      {
         printf("[---] YAC_Buffer::writeF32: out of bounds (io_offset=%u size=%u)\n", io_offset, size);
      }
#endif // YAC_NATIVE_WARN_BUFFER_OOB
   }

   sU8 readI8(void) {
      sU8 r;
      if(io_offset < size)
         r = buffer[io_offset++];
      else
         r = 0u;
      return r;
   }

   sU16 readI16(void) {
      sU16 r;
      if(io_offset + 1u < size)
      {
#ifdef YAC_LITTLE_ENDIAN
         r  = (sU16)buffer[io_offset++];
         r |= (sU16)(buffer[io_offset++] << 8);
#else
         r  = (sU16)(buffer[io_offset++] << 8);
         r |= (sU16)buffer[io_offset++];
#endif // YAC_LITTLE_ENDIAN
      }
      else
      {
         r = 0u;
      }
      return r;
   }

   sU32 readI32(void) {
      sU32 r;
      if(io_offset + 3u < size)
      {
#ifdef YAC_LITTLE_ENDIAN
         r  = (sU32)buffer[io_offset++];
         r |= (sU32)(buffer[io_offset++] << 8);
         r |= (sU32)(buffer[io_offset++] << 16);
         r |= (sU32)(buffer[io_offset++] << 24);
#else
         r  = (sU32)(buffer[io_offset++] << 24);
         r |= (sU32)(buffer[io_offset++] << 16);
         r |= (sU32)(buffer[io_offset++] << 8);
         r |= (sU32)buffer[io_offset++];
#endif // YAC_LITTLE_ENDIAN
      }
      else
      {
         r = 0u;
      }
      return r;
   }

   sF32 readF32(void) {
      yacmem r;
      if(io_offset + 3u < size)
      {
         yacmemptr m; m.u8 = buffer + io_offset;
         r.au8[0] = m.u8[0];
         r.au8[1] = m.u8[1];
         r.au8[2] = m.u8[2];
         r.au8[3] = m.u8[3];
         io_offset += 4u;
      }
      else
      {
         r.f32 = 0.0f;
      }
      return r.f32;
   }
#endif // __cplusplus

#if defined(__cplusplus)
};
extern "C" {
extern sBool yac_buffer_alloc (YAC_Buffer *_buffer, sU32 _size);
extern void yac_buffer_free (YAC_Buffer *_buffer);
extern void yac_buffer_write_s16 (YAC_Buffer *_buffer, sS16 _x);
extern void yac_buffer_write_2s16 (YAC_Buffer *_buffer, sS16 _x, sS16 _y);
extern void yac_buffer_write_3s16 (YAC_Buffer *_buffer, sS16 _x, sS16 _y, sS16 _z);
extern void yac_buffer_write_4s16 (YAC_Buffer *_buffer, sS16 _x, sS16 _y, sS16 _z, sS16 _w);
extern void yac_buffer_write_1fx (YAC_Buffer *_buffer, sF32 _x);
extern void yac_buffer_write_2fx (YAC_Buffer *_buffer, sF32 _x, sF32 _y);
extern void yac_buffer_write_3fx (YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z, sF32 _w);
extern void yac_buffer_write_4fx (YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z, sF32 _w);
extern void yac_buffer_write_f32 (YAC_Buffer *_buffer, sF32 _x);
extern void yac_buffer_write_2f (YAC_Buffer *_buffer, sF32 _x, sF32 _y);
extern void yac_buffer_write_3f (YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z);
extern void yac_buffer_write_4f (YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z, sF32 _w);
}
#else
} YAC_Buffer;
extern sBool yac_buffer_alloc (YAC_Buffer *_buffer, sU32 _size);
extern void yac_buffer_free (YAC_Buffer *_buffer);
extern void yac_buffer_write_s16 (YAC_Buffer *_buffer, sS16 _x);
extern void yac_buffer_write_2s16 (YAC_Buffer *_buffer, sS16 _x, sS16 _y);
extern void yac_buffer_write_3s16 (YAC_Buffer *_buffer, sS16 _x, sS16 _y, sS16 _z);
extern void yac_buffer_write_4s16 (YAC_Buffer *_buffer, sS16 _x, sS16 _y, sS16 _z, sS16 _w);
extern void yac_buffer_write_1fx (YAC_Buffer *_buffer, sF32 _x);
extern void yac_buffer_write_2fx (YAC_Buffer *_buffer, sF32 _x, sF32 _y);
extern void yac_buffer_write_3fx (YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z, sF32 _w);
extern void yac_buffer_write_4fx (YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z, sF32 _w);
extern void yac_buffer_write_f32 (YAC_Buffer *_buffer, sF32 _x);
extern void yac_buffer_write_2f (YAC_Buffer *_buffer, sF32 _x, sF32 _y);
extern void yac_buffer_write_3f (YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z);
extern void yac_buffer_write_4f (YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z, sF32 _w);
#endif
