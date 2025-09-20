/// YAC_String.cpp
///
/// (c) 1998-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU General Public License (GPL).
///
///

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "tks.h"

#include "TKS_StreamIO.h"
#include "TKS_Token.h"
#include "TKS_TokenTable.h"
#include "TKS_TokenizedScript.h"
#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "YAC_ListNode.h"
#include "YAC_TreeNode.h"
#include "YAC_HashTable.h"
#include "TKS_StringIterator.h"
#include "TKS_StringStackIterator.h"
#include "YAC_StringArray.h"
#include "YAC_Long.h"
#include "TKS_XMLParser.h"
#include "TKS_Mutex.h"

#include "PTN_Node.h"

YAC_C_CORE_POOLED(YAC_String, "String", YAC_CLID_STRING);

#ifdef TKS_STRING_OBJECTCOUNTER
sSI YAC_String::string_object_counter = 0;
#endif // TKS_STRING_OBJECTCOUNTER


YAC_String::YAC_String(void) {
#ifdef TKS_STRING_OBJECTCOUNTER
   YAC_String::string_object_counter++;
#endif // TKS_STRING_OBJECTCOUNTER

	init();
}

YAC_String::YAC_String(YAC_String *o) {
#ifdef TKS_STRING_OBJECTCOUNTER
   YAC_String::string_object_counter++;
#endif // TKS_STRING_OBJECTCOUNTER

   class_ID = YAC_CLID_STRING;
   buflen   = 0u;
   bflags   = 0u;
   length   = 0u;
   chars    = NULL;
   key      = YAC_LOSTKEY;
   yacCopy(o);
   clones   = NULL;
}

YAC_String::YAC_String(sSI) {
#ifdef TKS_STRING_OBJECTCOUNTER
   YAC_String::string_object_counter++;
#endif // TKS_STRING_OBJECTCOUNTER

   class_ID = YAC_CLID_STRING;
   clones   = NULL;
}

YAC_String::YAC_String(const sChar *e) {
#ifdef TKS_STRING_OBJECTCOUNTER
   YAC_String::string_object_counter++;
#endif // TKS_STRING_OBJECTCOUNTER

   class_ID = YAC_CLID_STRING;
   buflen   = 0u;
   bflags   = 0u;
   length   = 0u;
   chars    = NULL;
   clones   = NULL;
   key      = YAC_LOSTKEY;
   copy(e);
}

YAC_String::~YAC_String(void) {
#ifdef TKS_STRING_OBJECTCOUNTER
   YAC_String::string_object_counter--;
#endif // TKS_STRING_OBJECTCOUNTER

   YAC_String::free();
}

void YAC_String::init(void) {
   class_ID = YAC_CLID_STRING;
   buflen   = 0u;
   bflags   = 0u;
   chars    = NULL;
   length   = 0u;
   key      = YAC_LOSTKEY;
   clones   = NULL;
}

YAC_String& YAC_String::operator =(YAC_String *o) {
	yacCopy(o);
	return *this;
}

YAC_String& YAC_String::operator =(const sChar *e) {
	copy(e);
	return *this;
}

sU8 YAC_String::operator [](sUI i) const {
	if(i >= length)
	{
	  // // throw ArraySubscriptException();
	  return 0u;
	}
	return chars[i];
}

void YAC_String::free(void) {
   freeStack();

   if(NULL != chars)
   {
      safeFreeChars();
      chars  = NULL;
      length = 0u;
      buflen = 0u;
      bflags = 0u;
      key    = YAC_LOSTKEY;
   }
}

sU8 *YAC_String::AllocChars(sUI n, sUI *_retBuflen, sUI *_retBFlags) {
#ifdef TKS_USE_CHARALLOCATOR
   if(n <= TKS_CHARALLOCATOR_THRESHOLD)
   {
      if(TKS_ScriptEngine::b_allow_char_allocator)
      {
         *_retBuflen = n;
         *_retBFlags = TKS_STRFL_DEL | TKS_STRFL_LA;
         return TKS_ScriptEngine::char_allocator->alloc(n);
      }
   }
#endif // TKS_USE_CHARALLOCATOR

   *_retBuflen = n;
   *_retBFlags = TKS_STRFL_DEL;

   return Dyacallocchars(n);
}

void YAC_String::safeFreeChars(void) {
   if(bflags & TKS_STRFL_DEL)
   {
      if(!(bflags & TKS_STRFL_LA))
      {
         Dyacfreechars(chars);
      }
   }
}

YAC_String *YAC_String::clone(void) {
   YAC_String *a = new YAC_String(); // do not use Pooled String
   if(NULL != a)
   {
      YAC_String::pushStack(a);
   }
   return a;
}

sUI YAC_String::getKey(void) {
   if(YAC_LOSTKEY == key)
   {
      genKey();
   }
   return key;
}


void YAC_String::genKey(void) {
   //
   // Generate hash key
   //
   key = 0u;
   sU8 *c = chars;
   sUI i = 1u;

   if(NULL != c)
   {
      for(; *c; c++,i += 128u)
      {
         key = 53u * key + i * *c;
      }
   }
}

void YAC_String::pushStack(YAC_String *a) {
   StaticList::Insert(&clones, (void *)a);
}

void YAC_String::freeStack(void) {
   YAC_Object *cl;
   StaticList::Node *c=clones;

   while(NULL != c)
   {
      cl = ((YAC_Object*)c->data);

      if(NULL != cl)
      {
         YAC_DELETE(cl);
         c->data = NULL;
      }

      c = c->next;
   }

   StaticList::Free(&clones);
}

void YAC_VCALL YAC_String::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
	YAC_BEG_SERIALIZE();
	_ofs->yacStreamWriteI32(length);
	_ofs->yacStreamWriteString(this, 0, length);
}

sUI YAC_VCALL YAC_String::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
	YAC_BEG_DESERIALIZE();
	/// ---- pascal style string ----
	sSI l = _ifs->yacStreamReadI32();

	if(sUI(l) < tkscript->configuration.streamMaxPascalStringSize)
	{
		_ifs->yacStreamReadString(this, l);
		return 1u;
	}
	else
	{
		Dprintf("[---] String::deserialize: maximum stream string size exceeded (%i>%i)\n",
              ((sUI)l), tkscript->configuration.streamMaxPascalStringSize);
		return 0u;
	}
}

void YAC_String::refCopy(YAC_String *_o) {
   if(YAC_VALID(_o))
	{
      if(_o->bflags & TKS_STRFL_DEL)
      {
		   YAC_String::free();
		   chars  = (sU8*)_o->chars;
         key    = _o->key;
		   buflen = _o->buflen;
         bflags = _o->bflags;
		   length = _o->length;
		   _o->buflen = 0u;
         _o->bflags = 0u;
      }
      else
      {
         // If the other string buffer is not deletable we must not grab the reference since
         // this will cause trouble e.g. with pooled strings..
         yacCopy(_o);
      }
	}
}

void YAC_String::visit(YAC_String *_o) {
   YAC_String::free();
   chars  = (sU8*)_o->chars;
   key    = _o->key;
   buflen = 0u;
   bflags = (_o->bflags & ~TKS_STRFL_DEL);
   length = _o->length;
}

void YAC_String::visit(const char *_cstring) {
   YAC_String::free();

	if(NULL != _cstring)
	{
		chars  = (sU8 *)_cstring;
		length = sUI(strlen(_cstring) + 1);
      buflen = 0u;
      bflags = 0u;
      key    = YAC_LOSTKEY;
	}
}

void YAC_String::_visit(const YAC_Object *_s, sSI _off, sSI _len) {
   YAC_String::free();

   if(YAC_VALID(_s))
   {
      if(YAC_Is_String(_s))
      {
         const YAC_String *s = (const YAC_String *)_s;
         sUI off = (sUI) _off;
         sUI len = (sUI) _len;

         if(off < s->length )
         {
            key    = YAC_LOSTKEY;
            buflen = 0u;
            bflags = 0u;
            chars  = s->chars + off;

            if( (off+len) < s->length )
            {
               length = len;
            }
            else
            {
               length = (s->length - off);
            }
         }
      }
   }
}

sBool YAC_String::alloc(sUI len) {

   key = YAC_LOSTKEY;

   if( (bflags & TKS_STRFL_IMM) || (buflen < len) )
   {
      if(0u == len)
      {
         length = 0u;
         return YAC_TRUE;
      }

      safeFreeChars();

      sUI newBuflen;
      sUI newBFlags;
      chars = AllocChars(++len, &newBuflen, &newBFlags);

      if(NULL != chars)
      {
         buflen = newBuflen;
         bflags = (bflags & TKS_STRFLQMASK) | newBFlags; // keep old stringflags
         chars[len-1] = 0;
         *chars = 0u;
         length = 1u;
      }
      else
      {
         buflen = 0u;
         bflags = 0u;
         length = 0u;

         return YAC_FALSE;
      }
   }
	else
	{
      // Clear string attributes
      bflags &= ~(TKS_STRFLQMASK);

      if(0u != (bflags & TKS_STRFL_DEL))
      {
         chars[buflen - 1u] = 0u;
         *chars = 0u;
         length = 1u;
      }
      else
      {
         length = 0u;
      }
   }

   return YAC_TRUE;
}

void YAC_String::setQuotFlag(sSI _quotFlag) {
   bflags &= ~(TKS_STRFL_QUOT | TKS_STRFL_QUOTR | TKS_STRFL_QUOT2);

   if(1 == _quotFlag) // \"\"
   {
	   bflags |= TKS_STRFL_QUOT;
   }
   else if(2 == _quotFlag) // \'\'
   {
      bflags |= TKS_STRFL_QUOT2;
   }
   else if(3 == _quotFlag) // ;;region;;
   {
      bflags |= TKS_STRFL_QUOTR;
   }
}

sSI YAC_String::getQuotFlag(void) const {
   return
      ((bflags & TKS_STRFL_QUOT)  == TKS_STRFL_QUOT ) ? 1 :
      ((bflags & TKS_STRFL_QUOT2) == TKS_STRFL_QUOT2) ? 2 :
      ((bflags & TKS_STRFL_QUOTR) == TKS_STRFL_QUOTR) ? 3 :
      0
      ;
}

sBool YAC_String::empty(void) {
   length = 0u;
   key = YAC_LOSTKEY;
   return YAC_TRUE;
	// // return YAC_String::alloc(0);
}

sUI YAC_String::getLength(void) const {
   // including ASCIIz
	return (NULL != chars) ? length : 0u;
}

sUI YAC_String::getBufLen(void) const {
	return (NULL != chars) ? buflen : 0u;
}

sUI YAC_String::getNumChars(void) const {
   // don't count ASCIIZ
   return (length > 0u) ? (length - 1u) : 0u;
}

void YAC_String::ToLower(sChar *e, sUI e_length) {
	sChar c;
	for(; c = *e, e_length > 0u; e_length--)
   {
		*e++ = LowerCase(c);
   }
}

void YAC_String::toLower(void) {
   if(length > 0u)
   {
	   ToLower((sChar *)chars, length - 1u);
      key = YAC_LOSTKEY;
   }
}

void YAC_String::_toLower_YAC_RARG(YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_String *s = (YAC_String*) _r;

      s->yacCopy(this);
      s->toLower();
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_toLower_YAC_RVAL(YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);
   s->yacCopy(this);
   s->toLower();
}

void YAC_String::_toLower_YAC_RSELF(void) {
   toLower();
}

void YAC_String::ToUpper(sChar *e, sUI e_length) {
	sChar c;
	for(; c = *e, e_length > 0u; e_length--)
   {
		*e++ = UpperCase(c);
   }
}

void YAC_String::toUpper(void) {
   if(length > 0u)
   {
	   ToUpper((sChar *)chars, length - 1u);
      key = YAC_LOSTKEY;
   }
}

void YAC_String::_toUpper_YAC_RARG(YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_String *s = (YAC_String*) _r;

      s->yacCopy(this);
      s->toUpper();
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_toUpper_YAC_RVAL(YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);
   s->yacCopy(this);
   s->toUpper();
}


void YAC_String::_toUpper_YAC_RSELF(void) {
   toUpper();
}

sUI YAC_String::Replace(sChar *s, sUI length, sChar a, sChar b) {
   sUI ret = 0u;

   if( (0u != a) && (0u != b) )
   {
      sUI i = 0u;
      for(; i < length; i++)
      {
         if(s[i] == a)
         {
            ret++;
            s[i] = b;
         }
      }
   }

   return ret;
}

sUI YAC_String::replace(sChar a, sChar b) {
	key = YAC_LOSTKEY;

	return Replace((sChar *)chars, length, a, b);
}

void YAC_String::_replaceChar_YAC_RARG(sSI a, sSI b, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_String *s = (YAC_String*) _r;

      s->yacCopy(this);
      s->replaceChar(a, b);
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_replaceChar_YAC_RVAL(sSI a, sSI b, YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);
   s->yacCopy(this);
   s->replaceChar(a, b);
}

void YAC_String::_replaceChar_YAC_RSELF(sSI a, sSI b) {
   replaceChar(a, b);
}

sSI YAC_String::_replaceCharReturnNum(sSI a, sSI b) {
   return replaceChar(a, b);
}

sSI YAC_String::replaceChar(sSI a, sSI b) {
	sUI ret = 0u;

	if( (0u != a) && (0u != b) )
	{
		sUI i = 0u;
		for(; i < length; i++)
      {
			if(chars[i] == a)
			{
				ret++;
				chars[i] = (sChar) b;
            key = YAC_LOSTKEY;
			}
      }
	}
   else if(0u != a)
   {
      // ---- delete char a
      sU8 *d = chars;
      sUI l = 0u;
      sUI i = 0u;

      if(length > 1u)
      {
         sUI lm = length - 1u;

         for(; i < lm; i++)
         {
            if(chars[i] != a)
            {
               *d++ = chars[i];
               l++;
            }
            else
            {
               key = YAC_LOSTKEY;
               ret++;
            }
         }

         *d++ = 0u;
         length = (l + 1u);
      }
   }

	return ret;
}

sUI YAC_String::replace(const YAC_String *a, const YAC_String *b) {
	sUI ret = 0u;

	if(NULL != a && NULL != b)
	{
      if(0u == b->length)  // [18Feb2019] fix unterminated-string-after-replace issue
      {
         YAC_String t;
         t.visit("");
         return replace(a, &t);
      }

      if(a->length < 1u)
         return 0u;

#ifdef TKS_MT
      // (todo) place replace table in TLS to get rid of global lock
      (void)tkscript->lockGlobalContext();
#endif //TKS_MT

		if(a->length != b->length)
		{
			sUI i = 0u; // i holds offset to substring to be processed next
			sSI j = 0u; // j holds last indexOf result
			sUI k = 0u; // k holds the number of current replacements to be done.

			static sUI offsets[256]; // Replacement offset table. Can cache k=256 replacements.
			// Returns the total number of replacements done
			j = indexOf(a,i);
			// n holds offset of last 'a' string found
			sUI n = j;

			while(-1 != j)
			{
				n = (sUI)j;
				offsets[k++] = n;
				k = k & 255u;

				if(0u == k)
				{
					i = replaceTable(offsets, (sUI) 256, a, b);
               n = n + ( (b->length - 1u) - (a->length - 1u) ) * 256u;
               i = n + 1u;
					ret += 256u;
				}

				j = indexOf(a, n + (a->length - 1));
			}

			if(k > 0u)
			{
				replaceTable(offsets, k, a, b);
				ret += k;
			}
		}
		else
		{
			// special shortpath: equal length strings
			sU8 *bc = b->chars;
			sSI l = (a->length - 1u);
			sSI j = indexOf(a, 0);
			sSI k;

			if(l > 0)
			{
				while(-1 != j)
				{
					for(k = 0; k < l; k++)
               {
                  chars[j + k] = bc[k];
               }

					j = indexOf(a, j + 1);
				}
			}
		}

		key = YAC_LOSTKEY;

#ifdef TKS_MT
      tkscript->unlockGlobalContext();
#endif //TKS_MT
	}

   return ret;
}

sUI YAC_String::replace(const sChar *e, const sChar *f) {
	YAC_String a;
	YAC_String b;
   a.visit(e);
   b.visit(f);

   return replace(&a, &b);
}

sUI YAC_String::replace(const sChar *e, const YAC_String *f) {
	YAC_String a;
   a.visit(e);

   return replace(&a, f);
}

sUI YAC_String::replace(const YAC_String *a, const sChar *f) {
   YAC_String b;
   b.visit(f);

   return replace(a, &b);
}

sUI YAC_String::replaceTable(const sUI *offsets, sUI k,
                             const YAC_String *a, const YAC_String *b
                             )
{
	// newlen is the new string len if a has been replaced by b k times
	sUI newlen = length - (a->length-1)*k + (b->length-1)*k;

	// New character array
	sU8 *nc = Dyacallocchars(newlen);

	// new_i should return newlen
	sUI i = 0; // Source offset

   sUI new_i = 0; // Destination offset

	if(NULL != nc)
	{
		sUI j = 0;
		sUI r;

		for(; j<k; j++)
		{
			// Copy from source until next occurence of b
			for(; i<offsets[j];)
         {
				nc[new_i++] = chars[i++];
         }

			if(NULL != b->chars)
			{
				// Paste string b
				for(r=0; r<(b->length-1); r++)
            {
					nc[new_i++] = b->chars[r];
            }
			}

         // Skip a
			i += (a->length - 1);
		}

		// All occurences replaced, copy source until EOL
		if(i < length)
		{
			for(; new_i<newlen;)
         {
            nc[new_i++] = chars[i++];
         }
		}

		// Setup string
		length = newlen;
      safeFreeChars();
		chars  = nc;
		buflen = newlen;
      bflags = TKS_STRFL_DEL | (bflags & TKS_STRFLQMASK); // keep string flags
	}

	return i;
}

sBool YAC_String::compare(const sChar *e) const {
//    tkscript->printf("YAC_String::compare(e=\"%s\" this=\"%s\").\n", e, chars);
   if(NULL != e)
   {
      if(0 != *e)
      {
         if(NULL != chars)
         {
            if(length > 1)
            {
               // length includes ASCIIZ
               for(sUI i=0; i < length; i++)
               {
                  if(e[i] != chars[i])
                  {
                     return 0;
                  }
               }

               return 1;
            }
         }
      }
   }

   // empty string
   if( (NULL == chars) || (length <= 1) )
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

sBool YAC_String::compare(YAC_String& s) {
	return YAC_String::yacEquals(&s);
}

sBool YAC_String::compare(YAC_String *s) {
	return YAC_String::yacEquals(s);
}

sBool YAC_String::_compare(YAC_Object *s) {
	return YAC_String::yacEquals(s);
}

sBool YAC_String::_compareIgnoreCase(const YAC_Object *_s) const {
   if(YAC_Is_String(_s))
   {
      YAC_String *s = (YAC_String *)_s;

      if(s->length == length)
      {
         sU8 *s1 = (sU8*)s->chars;

         for(sUI i = 0u; i < length; i++)
         {
            sU8 ca = chars[i];

            if( (ca >= 'a') && (ca <= 'z') )
            {
               ca &= ~32u;
            }

            sU8 cb = s1[i];

            if( (cb >= 'a') && (cb <= 'z') )
            {
               cb &= ~32u;
            }

            if(ca != cb)
            {
               return YAC_FALSE;
            }
         }

         return YAC_TRUE;
      }
   }

   return YAC_FALSE;
}

sBool YAC_String::_compareIgnoreWhitespace(const YAC_Object *_s) const {
   if(YAC_Is_String(_s))
   {
      YAC_String *s = (YAC_String *)_s;

      if(NULL != chars && NULL != s->chars)
      {
         sUI i = 0u;
         sUI j = 0u;
         for(;;)
         {
            const sU8 ca =    chars[i];
            const sU8 cb = s->chars[j];

            // yac_host->printf("xxx ca=%u cb=%u  i=%u j=%u\n", ca, cb, i, j);

            if(0u == ca && 0u == cb)
               break;

            if(ca > 32u)
            {
               if(cb > 32u)
               {
                  if(ca != cb)
                     return YAC_FALSE;

                  // Common character, advance
                  i++;
                  j++;
               }
               else
               {
                  // Skip other whitespace
                  if(0u != cb)
                     j++;
                  else
                     return YAC_FALSE;
               }
            }
            else
            {
               // Skip this whitespace
               if(0u != ca)
                  i++;
               else if(cb > 32u)
                  return YAC_FALSE;
            }
         }
         return YAC_TRUE;
      }

      // Equal when both strings are NULL or empty
      return
         ((NULL == s->chars) || (0u == s->chars[0])) &&
         ((NULL ==    chars) || (0u ==    chars[0])) ;
   }

   return YAC_FALSE;
}

void string_debug(void) {
   printf("xxx string_debug\n");
}

sBool YAC_VCALL YAC_String::yacCopy(YAC_Object *u) {
   if(u != this)
   {
      YAC_String::empty();

      if(YAC_VALID(u))
      {
         if(YAC_Is_String(u))
         {
            YAC_String *o = (YAC_String *)u;

            if(o->length > 0)
            {
               // Copy chars
               if(YAC_String::alloc(o->length))
               {
                  sUI i;
                  for(i=0; i<o->length; i++) // Also copy ASCIIZ
                  {
                     chars[i] = o->chars[i];
                  }

                  length = i;
               }

               // Copy quotation flag
               if(o->bflags & TKS_STRFL_QUOT)
                  bflags |= TKS_STRFL_QUOT;
               else if(o->bflags & TKS_STRFL_QUOTR)
                  bflags |= TKS_STRFL_QUOTR;
               else if(o->bflags & TKS_STRFL_QUOT2)
                  bflags |= TKS_STRFL_QUOT2;

               // Copy hash key
               key = o->key;

               return YAC_TRUE;
            }
            else
            {
               return empty();
            }
         }
         else if(YAC_VALID(u))
         {
            return u->yacToString(this);
         }
      }
   } // if(u != this)

	return YAC_FALSE;
}

void YAC_String::copyBuffered(YAC_String *_o, sChar *_buf) {
   length = _o->length;
   buflen = _o->buflen;
   bflags = _o->bflags & ~TKS_STRFL_DEL;
   key    = _o->key;
   chars  = (sU8*)_buf;
   memcpy(chars, _o->chars, length);
}

sBool YAC_String::CopyTo(const sChar *a, sUI a_length, sUI a_offset,
					          sChar *b, sUI b_length, sUI b_offset,
					          sUI num_chars
                         )
{
	sBool ret = ((a_offset + num_chars) <= a_length) && ((b_offset + num_chars) <= b_length);

	if(ret)
	{
		a += a_offset;
		b += b_offset;

		for(; num_chars>0; num_chars--)
		{
			b[num_chars-1] = (sChar)a[num_chars-1];
		}
	}

	return ret;
}

sBool YAC_String::copy(const sChar *e) {
	if(NULL != e)
	{
		sUI j = (sUI) strlen(e);
		sBool ret = YAC_String::alloc(j+1);

		if(ret)
		{
			sUI i = 0;
			for(; i<j; i++)
         {
            *(sChar*)&chars[i] = e[i];
         }

			chars[i] = 0;
         length = j + 1;
		}

		return ret;
	}
	else
   {
      return empty();
   }
}

void YAC_String::insert_INT(sSI _i, const YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      if(! YAC_Is_String(_s))
      {
         if(_i >= 0)
         {
            YAC_String t;

            if(_s->yacToString(&t))
            {
               insert((sUI)_i, &t);
            }
         }
      }
      else
      {
         if(_i >= 0)
         {
            insert((sUI)_i, (YAC_String *) _s);
         }
      }
   }
}


void YAC_String::_insert_YAC_RARG(sSI _index, const YAC_Object *_s, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_String *s = (YAC_String *) _r;

      s->insert_INT(_index, _s);
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}


void YAC_String::_insert_YAC_RVAL(sSI _index, const YAC_Object *_s, YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);

   s->insert_INT(_index, _s);
}

void YAC_String::_insert_YAC_RSELF(sSI _index, const YAC_Object *_s) {
   insert_INT(_index, _s);
}

sBool YAC_String::insert(sUI _i, const char *_s) {
	if( (_i < length) && (NULL != _s) )
	{
		YAC_String t,u;

		if(_i > 0)
      {
         substring(&t, 0, _i);
      }
		else
      {
         t.empty();
      }

		t.append(_s);
		substring(&u, _i, length - _i);
		YAC_String::yacCopy(&t);
		append(&u);

		return YAC_TRUE;
	}

	return YAC_FALSE;
}

sBool YAC_String::insert(sUI _i, const YAC_String *_s) {
   if(NULL != _s)
	{
		YAC_String t,u;
		substring(&t, 0, _i);
		t.append(_s);
		substring(&u, _i, length - _i);
		YAC_String::yacCopy(&t);
		append(&u);

		return YAC_TRUE;
	}

	return YAC_FALSE;
}

sBool YAC_String::append(const YAC_String *o) {
   if( (NULL != o) && (NULL != o->chars) && (o->length > 0) )
   {
      sUI k = (o->length + length); // (todo) handle potential 32bit overflow

      if(buflen < k)
      {
         k *= 2; // (todo) excessive growth rate ?!

         sUI newBuflen;
         sUI newBFlags;
         sU8 *nc = AllocChars(k, &newBuflen, &newBFlags);
         sBool ret = (NULL != nc) ? 1 : 0;

         if(ret)
         {
            sUI i = 0;

            if(NULL != chars)
            {
               if(length > 0)
               {
                  length--;

                  for(; i < length; i++)
                  {
                     nc[i] = chars[i];
                  }
               }

               safeFreeChars();
            }

            buflen = newBuflen;
            bflags = newBFlags | (bflags&TKS_STRFLQMASK);

            for(k=0; k<o->length; i++, k++)
            {
               nc[i] = o->chars[k];
            }

            chars  = nc;
            length = length + o->length;
            key    = YAC_LOSTKEY;
         }

         return ret;
      }
      else
      {
         sU8 *d = chars + getNumChars();
         sUI i = 0;

         for(; i< o->length;i++)
         {
            *d++ = o->chars[i];
         }

         length = (sUI) (d-chars);
         key    = YAC_LOSTKEY;

         return YAC_TRUE;
      }
   }

   return YAC_FALSE;
}

sBool YAC_String::append(const sChar _c) {
   sChar s[2];
   s[0] = _c;
   s[1] = 0;
   return append(s);
}

sBool YAC_String::append(const sChar *e) {
	if(NULL != e)
	{
		sUI k = sUI(::strlen(e));
		sUI i;

		if(buflen < (length+k))
		{
			sU8 *nc = NULL;
			sUI j = length;
			sUI nbuflen = 0;

			if(j > 0)
			{
				if(NULL != chars)
            {
					if(0 == chars[j-1])
					{
						nbuflen = j + k;
						nc = Dyacallocchars(nbuflen); // use previous '\000'
						j--;
					}
					else
					{
						nbuflen = j + k + 1;
						nc = Dyacallocchars(nbuflen); // no previous '\000'???
					}
            }
			}
			else
			{
				nbuflen = k + 1;
				nc = Dyacallocchars(nbuflen);
			}

			sBool ret = (NULL != nc) ? YAC_TRUE : YAC_FALSE; // sgi cc

			if(ret)
			{
				if(NULL != chars)
				{
					for(i=0; i<j; i++)
               {
                  nc[i] = chars[i];
               }

               safeFreeChars();
				}

				for(i=0; i<k; i++)
            {
               nc[i+j] = e[i];
            }

				nc[i + j] = 0;
				length = j + k + 1;
				chars  = nc;
				buflen = nbuflen;
            bflags = (bflags & TKS_STRFLQMASK) | TKS_STRFL_DEL;
				key    = YAC_LOSTKEY;
			}

			return ret;
		}
		else
		{
			sU8 *d = &chars[getNumChars()];
         if(0u == length)
            length = 1u;
			length += k;

			for(i=0; i<k; i++)
         {
            *d++ = e[i];
         }

			*d++ = 0;
			key = YAC_LOSTKEY;
		}
	}

	return YAC_TRUE;
}

sBool YAC_String::startsWith(const sChar *e) const {
	sBool ret = (NULL != e) && (NULL != chars);

	if(ret)
	{
		sUI k = sUI(::strlen(e));
		ret = (k <= length);

		if(ret)
		{
			sUI i = 0;

			for(; i<k && e[i]==chars[i]; i++)
            ;

			ret = (i == k) ? YAC_TRUE : YAC_FALSE;
		}
	}

	return ret;
}

sBool YAC_String::startsWith(const YAC_String *s) const {
	if( (NULL != s) && (NULL != chars) && (s->length <= length) )
	{
		sSI l = s->length;

		if(l)
		{
			sSI i;
			l--;

			for(i=0; i<l && s->chars[i]==chars[i]; i++)
            ;

			return (i == l) ? YAC_TRUE : YAC_FALSE;
		}
	}

	return YAC_FALSE;
}


sBool YAC_String::endsWith(const sChar *e) const {
	sBool ret = (NULL != e) && (NULL != chars);

	if(ret)
	{
		sSI k = (sSI)strlen(e);

      if(k > 0)
      {
   		ret = (k <= (sSI)(length-1));

		   if(ret)
		   {
   			/*register*/ sU8 *es = &chars[length];

			   for(; k>=0 && *--es==e[k]; k--)
               ;

			   ret = (-1 == k) ? YAC_TRUE : YAC_FALSE;
		   }
      }
      else
      {
         return YAC_FALSE; // endswith empty string? sure..
      }
	}

	return ret;
}


sBool YAC_String::endsWith(const YAC_String *s) const {
	if( (NULL == s) || (NULL == chars) )
   {
      return YAC_FALSE;
   }

	sSI k = (sSI)s->length - 1;

   if(k > 0)
   {
	   if(k <= (sSI)(length - 1))
	   {
   		sU8 *es = &chars[length];

		   for(; k>=0 && *--es==s->chars[k]; k--)
            ;

		   return (-1 == k) ? YAC_TRUE : YAC_FALSE;
	   }
   }

	return YAC_FALSE;
}

void YAC_String::substring_INT(sUI start, sUI len, YAC_String *s) const {
   //
   // len is actually endbyteoff!
   //
   if(start >= length)
   {
      s->empty();
   }
   else
   {
      if(len > length)
      {
         len = (length - start);
      }

      if((start + len) > length )
      {
         len = (length - start);
      }

      substring(s, start,len);
   }
}

void YAC_String::_substring_YAC_RARG (sUI _startbyteoff, sUI _endbyteoff, YAC_Object *_r) const {
   //
   // Write result to return object
   //
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      substring_INT(_startbyteoff, _endbyteoff, (YAC_String*) _r);
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_substring_YAC_RVAL (sUI _startbyteoff, sUI _endbyteoff, YAC_Value *_r) const {
   //
   // Return new String object
   //
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);

   substring_INT(_startbyteoff, _endbyteoff, s);
}

void YAC_String::_substring_YAC_RSELF(sUI _startbyteoff, sUI _endbyteoff) {
   //
   // Modify self
   //
   YAC_String s;
   substring_INT(_startbyteoff, _endbyteoff, &s);
   yacCopy(&s);
}

sBool YAC_String::substring(YAC_String *s, sUI start, sUI len) const {
   if(s != this)
   {
      if(len > 0)
      {
         if(NULL != chars)
         {
            sU8 *a = chars + start;
            sUI k = (0 != a[len-1]) ? (len + 1) : len;

            if(s->alloc(k))
            {
               sUI i = 0;

               for(; i < len; i++)
               {
                  s->chars[i] = *a++;
               }

               if(k != len)
               {
                  s->chars[i++] = 0;
               }

               s->length = i;

               return YAC_TRUE;
            }
         }
         else
         {
            s->empty();
         }
      }
      else
      {
         s->empty();
      }
   }

   return YAC_FALSE;
}

sBool YAC_String::substring(YAC_String& s, sUI start, sUI len) const {
	return substring(&s, start, len);
}

sBool YAC_String::substring(YAC_String *s, sUI start) const {
	return substring(s, start, length - start);
}

sBool YAC_String::substring(YAC_String& s, sUI start) const {
	return substring(&s, start, length - start);
}

sSI YAC_String::split(sSI _delim) {
	sSI ret = ((length > 1) && (_delim > 0)) ? 1 : 0;

	if(ret > 0)
	{
      ret = 0;
		sUI i = 0; // current index in string
		sUI j = 0; // length of current substring
		sUI k = 0; // start index

		for(; i < length; i++)
		{
			if(chars[i] == _delim)
			{
				if(!j)
            {
					k++; // skip delimiter
            }
				else
				{
               YAC_String *cl = new YAC_String(); // write substring, do not use Pooled String!

					if(NULL != cl)
               {
                  ret++;
                  pushStack(cl);
                  substring(cl, k, j);
                  k = (i + 1);
						j = 0;
					}
					else
               {
                  return 0; // out of mem?
               }
				}
			}
			else
         {
            j++;
         }
		}

		// write tail of string
		if(j > 0)
		{
			YAC_String *cl = new YAC_String(); // do not use Pooled String!

			if(NULL != cl)
			{
            ret++;
				pushStack(cl);
				substring(cl, k, j);
			}
			else
         {
            return 0; // out of mem?
         }
		}
	}

	return ret;
}

sSI YAC_String::words(sSI _use_quot) {
	sSI ret = (length > 1) ? 1 : 0;

	if(ret)
	{
      ret = 0;
		sUI i = 0;
		sUI j = 0;
		sBool spaced = YAC_TRUE;
		sSI quot = 0; // >>"<< quotation mark..
		sSI sqm = 0;  // >>'<< quotation mark
		YAC_String *cl = NULL;

		for(; i < length; i++)
		{
			switch(chars[i])
			{
			case '\n':
			case '\014':
			case '\t':
			case ' ':
			case '\000':
				//check_hyphen:
				if(!spaced && (!(quot&1)) && (!(sqm&1)) )
				{
					cl = (YAC_String*)clone();

					if(NULL != cl)
					{
    					ret++;

						if(quot > 0)
						{
							substring(cl, j, i - j - 1);
							quot -= 2;
						}
						else if(sqm > 0)
                  {
                     substring(cl, j, i - j - 1);
                     sqm -= 2;
                  }
                  else
                  {
                     substring(cl, j, i - j);
                  }

						spaced = YAC_TRUE;
					}
				}
				break;

			case '\'':
				if(_use_quot && (!(quot&1)) ) // ignore with "..."
            {
					sqm++;
            }
				else
				{
					if(spaced)
               {
                  j = i;
               }

					spaced = YAC_FALSE;
				}
				break;

			case '\"':
				if(_use_quot && (!(sqm&1)) ) // ignore within '...'
				{
					quot++;
					break;
				}
				// run into default
			default:
				if(spaced)
            {
					j = i;
            }

				spaced = YAC_FALSE;
				break;
			}
		}
	}

	return ret;
}

sBool YAC_String::isPredecessor(const YAC_String *o, sBool case_sensitive) const {
	if(NULL != o)
	{
		if( (NULL != chars) && (NULL != o->chars) )
		{
			sSI n = (o->length > length) ? (length - 1) : (o->length - 1);
			sSI i = 0;

			if(0 == length)
         {
            return (o->length > 0);
         }

			if(case_sensitive)
			{
				for(; i<n && chars[i]==o->chars[i]; i++)
               ;

				if(i == n)
            {
               return (length < o->length);
            }
				else
            {
               return (chars[i] < o->chars[i]);
            }
			}
			else
			{
				sChar a;
				sChar b;
				i = 0;

				do
				{
					a = chars[i];
					b = o->chars[i];

					if( (a >= 'a') && (a <= 'z') )
               {
                  a &= ~32;
               }

					if( (b >= 'a') && (b <= 'z') )
               {
                  b &= ~32;
               }

					i++;
				}
            while( (i < n) && (a == b) );

				if( (a == b) && (i == n) )
            {
               return (length < o->length);
            }

				return (a < b);
			}
		}
	}

	return YAC_FALSE;
}

sBool YAC_String::_isPredecessor(YAC_String *_o, sBool _bCaseSensitive) {
   if(YAC_BCHK(_o, YAC_CLID_STRING))
   {
      YAC_String *o = (YAC_String*)_o;

      return isPredecessor(o, _bCaseSensitive);
   }
   return YAC_TRUE;
}

sSI YAC_String::indexOf(sChar _c, sUI _start) const {
	sUI i = _start;

	for(; i < length; i++)
   {
      if(chars[i] == ((sU8)_c))
      {
         return (sSI)i;
      }
   }

	return -1;
}

sSI YAC_String::indexOf(const sChar *e, sUI start) const {
	YAC_String s;
	s.visit(e);

	return indexOf(&s, start);
}

sSI YAC_String::_indexOfChar(sSI _c, sSI _start) const {
   return indexOf((sChar)_c, (sUI)_start);
}

sSI YAC_String::_revIndexOfChar(sSI _c, sSI _start) const {
	sSI i = _start;
// //    sChar c = (sChar) _c;

   while(i >= 0)
   {
	   if(chars[i] == _c)
      {
         return i;
      }
      i--;
   }

	return -1;
}

sSI YAC_String::_indexOfFirstDifference(YAC_Object *_s) const {
   if(YAC_Is_String(_s))
   {
      const YAC_String *s = (YAC_String*) _s;

      sUI len = sMIN(length, s->length);
      if(len > 1u)
      {
         for(sUI i = 0u; i < len; i++)
         {
            if(chars[i] != s->chars[i])
               return i;
         }
      }
   }
   return -1;
}

void YAC_String::_charsetBlacklist_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_CAST_ARG(YAC_String, r, _r);
      r->realloc(length, YAC_FALSE/*bCopy*/);

      if(YAC_CHK(_s, YAC_CLID_STRING))
      {
         YAC_CAST_ARG(YAC_String, s, _s);

         sUI newLen = 0u;
         sU8 *d = r->chars;

         for(sUI i = 0u; i < length; i++)
         {
            const sU8 tc = chars[i];

            if(0u != tc)
            {
               sBool bKeep = YAC_TRUE;

               for(sUI j = 0u; bKeep && (j < s->length); j++)
                  bKeep = ( (0u != s->chars[j]) ? (s->chars[j] != tc) : YAC_TRUE );

               if(bKeep)
                  d[newLen++] = tc;
            }
            else
            {
               d[newLen++] = 0u;
               break;
            }
         }

         r->length = newLen;
      }
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_charsetBlacklist_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *r = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(r, 1);
   _charsetBlacklist_YAC_RARG(_s, r);
}

void YAC_String::_charsetBlacklist_YAC_RSELF(YAC_Object *_s) {
   if(YAC_CHK(_s, YAC_CLID_STRING))
   {
      YAC_CAST_ARG(YAC_String, s, _s);

      sUI newLen = 0u;
      sU8 *d = chars;

      for(sUI i = 0u; i < length; i++)
      {
         const sU8 tc = chars[i];

         if(0u != tc)
         {
            sBool bKeep = YAC_TRUE;

            for(sUI j = 0u; bKeep && (j < s->length); j++)
               bKeep = ( (0u != s->chars[j]) ? (s->chars[j] != tc) : YAC_TRUE );

            if(bKeep)
               d[newLen++] = tc;
         }
         else
         {
            d[newLen++] = 0u;
            break;
         }
      }

      length = newLen;
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_charsetWhitelist_YAC_RARG(YAC_Object *_s, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_CAST_ARG(YAC_String, r, _r);
      r->realloc(length, YAC_FALSE/*bCopy*/);

      if(YAC_CHK(_s, YAC_CLID_STRING))
      {
         YAC_CAST_ARG(YAC_String, s, _s);

         sUI newLen = 0u;
         sU8 *d = r->chars;

         for(sUI i = 0u; i < length; i++)
         {
            const sU8 tc = chars[i];

            if(0u != tc)
            {
               sBool bKeep = YAC_FALSE;

               for(sUI j = 0u; !bKeep && (j < s->length); j++)
                  bKeep = (s->chars[j] == tc);

               if(bKeep)
                  d[newLen++] = tc;
            }
            else
            {
               d[newLen++] = 0u;
               break;
            }
         }

         r->length = newLen;
      }
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_charsetWhitelist_YAC_RVAL(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *r = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(r, 1);
   _charsetWhitelist_YAC_RARG(_s, r);
}

void YAC_String::_charsetWhitelist_YAC_RSELF(YAC_Object *_s) {
   if(YAC_CHK(_s, YAC_CLID_STRING))
   {
      YAC_CAST_ARG(YAC_String, s, _s);

      sUI newLen = 0u;
      sU8 *d = chars;

      for(sUI i = 0u; i < length; i++)
      {
         const sU8 tc = chars[i];

         if(0u != tc)
         {
            sBool bKeep = YAC_FALSE;

            for(sUI j = 0u; !bKeep && (j < s->length); j++)
               bKeep = (s->chars[j] == tc);

            if(bKeep)
               d[newLen++] = tc;
         }
         else
         {
            d[newLen++] = 0u;
            break;
         }
      }

      length = newLen;
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

sUI YAC_String::sum(void) const {
   //
   // Generate (alternate) hash key
   //
   // (todo) still used?
   //
	sUI i = 0;
	sUI ret = 0;
	sU8 cc;
	sU8 lc = 0;

	for(; i < length; i++)
	{
		cc = chars[i];
		ret += ((sUI)cc ^ lc) + (i << 2);
		lc = cc;
	}

	return ret;
}


sUI YAC_String::sumChars(void) const {
	sUI i = 0;
	sUI ret = 0;

	for(; i < length; i++)
   {
      ret += (sUI)chars[i];
   }

	return ret;
}


#if 0
sSI YAC_String::indexOf_OLD(YAC_String *s, sUI start) {
   //
   // Uses rolling hash to find possible matches.
   // ..probably only good for long keys ?!
   //
   // TODO: Boye-Moore algorithm may be faster for even longer keys ?
   //
   ////if(s&&chars&&s->length&&s->chars)
   if(s && (s->length > 1))
   {
      ////if(start<=length)
      {
         if((s->length+start)<=length)
         {
            sUI ckey = 0;
            sUI dkey = s->sumChars(); // Calc substring hash
            sUI i = start;
            sUI slen = s->length - 1;
            const sU8 *c = chars;

            // Calc hash until end of first possible occurence of substring
            while(i < slen+start)
            {
               ckey += (sUI) c[i++];
            }

            sUI l = start;
            sUI searchLen = (length - s->length);
            const sU8 *sc = s->chars;
            //YAC_String sub;
            do
            {
               if(ckey == dkey) // hash values match ?
               {
                  //substring(&sub, l, s->length-1); // WTF
                  //if(sub.compare(s)) return l;

                  // Compare substring
                  sUI k = 0;
                  const sU8 *t = c + l;
                  while(k < slen)
                  {
                     if(*t++ != sc[k++])
                     {
                        break;
                     }
                  }
                  if(k == slen)
                  {
                     // Found substring
                     return l;
                  }
               }

               // Update hash
               ckey += (sUI)c[i++] - (sUI)c[l++];

            } while(l <= searchLen);
         }
      }
   }
   return -1;
}
#endif


sSI YAC_String::indexOf(const YAC_String *s, sUI start) const {
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

sUI YAC_String::numIndicesOf(const YAC_String *s) const {
	sUI i;
	sSI k = -1;
	sSI j = -((sSI)(s->length) - 1);

	do
	{
		i = j + s->length - 1;
		k++;
		j = indexOf(s,i);
	}
   while(-1 != j);

	return (sUI)k;
}

sUI YAC_String::numIndicesOf(const sChar *e) const {
	YAC_String s;
   s.visit(e);

   return numIndicesOf(&s);
}

sSI YAC_String::lastIndexOf(const YAC_String *s, sUI start) const {
	sUI i = start;
	sSI k = -1;
	sSI j = i;

	for(;;)
	{
		j = indexOf(s,i);

		if(-1 == j)
      {
         break;
      }
		else
		{
			k = j;
			i = k + s->length - 1;
		}
	}

	return k;
}


sSI YAC_String::lastIndexOf(sChar _c, sUI _start) const {
   sSI li = -1;

   if(NULL != chars)
   {
      sUI i = _start;

      for(; i < length; i++)
      {
         if(chars[i] == ((sChar)_c))
         {
            li = i;
         }
      }
   }

   return li;
}

sBool YAC_String::isSubstring(const YAC_String *s, sUI start) const {
	return (-1 != indexOf(s, start));
}


void YAC_VCALL YAC_String::yacValueOfI(sSI i) {
	sChar buf[32];
	Dyac_snprintf(buf, 32, "%i", i);
	copy(buf);
}

void YAC_VCALL YAC_String::yacValueOfF32(sF32 f) {
	sChar buf[64];
	Dyac_snprintf(buf, 64, "%g", f);
	// sprintf(buf, "%e", f);
	copy(buf);
}

void YAC_VCALL YAC_String::yacValueOfF64(sF64 f) {
	sChar buf[96];
	Dyac_snprintf(buf, 96, "%.64lg", f);
	copy(buf);
}

void YAC_VCALL YAC_String::yacValueOfI64(sS64 i) {
   // ---- msvc6 does not support printf for 64bit ints
   YAC_Long l;
   l.value = i;
   l.yacToString(this);
}

void YAC_String::valueOf(YAC_Object *o) {
	if(NULL != o)
   {
      o->yacToString(this);
   }
}

sBool YAC_VCALL YAC_String::yacScanI(sSI *_i) {
	sBool ret = YAC_FALSE;
	*_i = 0;

	if( (NULL != chars) && (length > 0) )
	{
		sBool ishex;
		sBool hex0x = YAC_FALSE;
		sUI i = 0;

		if( ('0' == chars[0]) && ('x' == chars[1]) )
		{
			i++;
			ishex = YAC_TRUE;
			hex0x = YAC_TRUE;
		}
		else
      {
         ishex = ('$' == ((sChar*)chars)[0]) || ('#' == ((sChar*)chars)[0]);
      }

		if(ishex)
		{
			i++;
			// hexadecimal mode
			sUI nc = 0;
			ret = YAC_TRUE;

			for(; i < (length - 1) && ret; i++)
			{
				switch(((sChar*)chars)[i])
				{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
					nc++;
					break;

				default:
					ret = YAC_FALSE;
					break;
				}
			}

			*_i = 0;

			if(ret)
			{
				for(i = (1 + hex0x); i<(length-1) && ret; i++)
				{
					nc--;

					switch(((sChar*)chars)[i])
					{
					case '0':
						break;

					case '1':
						*_i |= (1 << (nc << 2));
						break;

					case '2':
						*_i |= (2 << (nc << 2));
						break;

					case '3':
						*_i |= (3 << (nc << 2));
						break;

					case '4':
						*_i |= (4 << (nc << 2));
						break;

					case '5':
						*_i |= (5 << (nc << 2));
						break;

					case '6':
						*_i |= (6 << (nc << 2));
						break;

					case '7':
						*_i |= (7 << (nc << 2));
						break;

					case '8':
						*_i |= (8 << (nc << 2));
						break;

					case '9':
						*_i |= (9 << (nc << 2));
						break;

					case 'A':
					case 'a':
						*_i |= (10 << (nc << 2));
						break;

					case 'B':
					case 'b':
						*_i |= (11 << (nc << 2));
						break;

					case 'C':
					case 'c':
						*_i |= (12 << (nc << 2));
						break;

					case 'D':
					case 'd':
						*_i |= (13 << (nc << 2));
						break;

					case 'E':
					case 'e':
						*_i |= (14 << (nc << 2));
						break;

					case 'F':
					case 'f':
						*_i |= (15 << (nc << 2));
						break;
					}
				} // for..
			} // for..
      } // if <hexmode>..
      else if( ('0' == ((char*)chars)[0]) && ('b' == ((char*)chars)[1]) )
		{
			// binary mode
			ret = YAC_TRUE;
			sUI nc = 0;

			for(i=2; (i < (length - 1)) && ret; i++)
			{
				if( ('0' != ((sChar*)chars)[i]) && ('1' != ((sChar*)chars)[i]) )
            {
					ret = YAC_FALSE;
            }
				else
            {
					nc++;
            }
			}

			if(ret)
			{
				*_i = 0;

				for(i=2; i < (length - 1); i++)
				{
					nc--;
					*_i |= ('1' == ((sChar*)chars)[i]) ? (1 << nc) : 0;
				}
			}
		}
		else
		{
			// decimal mode
			ret = YAC_TRUE;

         if('-' == ((sChar*)chars)[0])
         {
            for(i=1; i < (length - 1) && ret; i++)
            {
               if('.' == chars[i])
               {
                  break; // float to int
               }

               if( (((sChar*)chars)[i] < '0') || (((sChar*)chars)[i] > '9') )
               {
                  ret = YAC_FALSE;
               }
            }

            if(ret)
            {
               ////ret = (0 != sscanf((sChar *)chars+1, "%d", _i));
               *_i = atoi((sChar*)chars+1);
               ret = YAC_TRUE;
               *_i = -*_i;
            }
            else
            {
               *_i = 0;
            }
         }
         else
         {
            for(i=0; i < (length - 1) && ret; i++)
            {
               if('.' == chars[i])
               {
                  break; // float to int
               }

               if( (((sChar*)chars)[i] < '0') || (((sChar*)chars)[i] > '9') )
               {
                  ret = YAC_FALSE;
               }
            }

            if(ret)
            {
               ////ret = (0 != sscanf((sChar *)chars, "%d", _i));
               *_i = atoi((sChar*)chars);
               ret = YAC_TRUE;
            }
            else
            {
               *_i = 0;
            }
         }
		} // else..
   } // if chars..

   return ret;
}

sBool YAC_String::checkIntConversion(void) const {
   return (YAC_TYPE_INT == checkConversions(YAC_FALSE/*bInternal*/));
}

sBool YAC_String::checkFloatConversion(void) const {
	if( (NULL != chars) && (length > 0) )
	{
		sUI i;
      sUI nc = 0;
		sUI pc = 0;
		sUI mc = 0;
      sUI ec = 0;
      sUI pmc = 0;

		for(i=0; (i < (length - 1)); i++)
		{
			switch(((sChar*)chars)[i])
			{
            case 'f':
               if(!nc)
               {
                  return YAC_FALSE;
               }
               else
               {
                  break;
               }

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               nc++;
               break;

            case 'e':
            case 'E':
                ec++;
                break;

            case '+':
               if(1 == ec)
               {
                  pmc++;
               }
               else
               {
                  return YAC_FALSE;
               }
               break;

            case '.':
               pc++;
               break;

            case '-':
               if(1 == ec)
               {
                  pmc++;
               }
               else
               {
                  mc++;
               }
               break;

            default:
               return YAC_FALSE;
			}
      }

		return
         (nc > 0) &&
         (mc < 2) &&
         (pc < 2) &&
         (ec < 2) &&
         (pmc < 2) ;
	}

	return YAC_FALSE;
}


sBool YAC_VCALL YAC_String::yacScanF32(sF32 *_f) {

   switch(checkConversions())
   {
      case 1:
      {
         // Try int-conversion (e.g. float f = "0xff";)
         sSI i = 0;
         yacScanI(&i);
         *_f = (sF32) i;
      }
      return YAC_TRUE;

      case 2:
         return (0 != sscanf((const char *)chars, "%g", _f));
   }

   return YAC_FALSE;
}

sBool YAC_VCALL YAC_String::yacScanF64(sF64 *_g) {

   if(checkFloatConversion())
   {
      return (0 != sscanf((sChar *)chars, "%lg", _g));
   }
   else
   {
      *_g = 0;

      return YAC_FALSE;
   }
}

sBool YAC_VCALL YAC_String::yacScanI64(sS64 *_i) {
	sBool ret = YAC_FALSE;
	*_i = 0;

	if( (NULL != chars) && (length > 0) )
	{
		sBool ishex;
		sBool hex0x = YAC_FALSE;
		sUI i = 0;

		if( ('0' == chars[0]) && ('x' == chars[1]) )
		{
			i++;
			ishex = YAC_TRUE;
			hex0x = YAC_TRUE;
		}
		else
      {
         ishex = ('$' == ((sChar*)chars)[0]) || ('#' == ((sChar*)chars)[0]);
      }

		if(ishex)
		{
			i++;
			// hexadecimal mode
			sUI nc = 0;
			ret = 1;

			for(; i < (length - 1) && ret; i++)
			{
				switch(((sChar*)chars)[i])
				{
               case '0':
               case '1':
               case '2':
               case '3':
               case '4':
               case '5':
               case '6':
               case '7':
               case '8':
               case '9':
               case 'a':
               case 'b':
               case 'c':
               case 'd':
               case 'e':
               case 'f':
               case 'A':
               case 'B':
               case 'C':
               case 'D':
               case 'E':
               case 'F':
                  nc++;
                  break;

               default:
                  ret = YAC_FALSE;
                  break;
				}
			}

			*_i = 0;

			if(ret)
			{
				for(i = (1 + hex0x); i < (length - 1) && ret; i++)
				{
					nc--;

					switch(((sChar*)chars)[i])
					{
                  case '0':
                     break;

                  case '1':
                     *_i |= ((sS64)1) << (nc << 2);
                     break;

                  case '2':
                     *_i |= ((sS64)2) << (nc << 2);
                     break;

                  case '3':
                     *_i |= ((sS64)3) << (nc << 2);
                     break;

                  case '4':
                     *_i |= ((sS64)4) << (nc << 2);
                     break;

                  case '5':
                     *_i |= ((sS64)5) << (nc << 2);
                     break;

                  case '6':
                     *_i |= ((sS64)6) << (nc << 2);
                     break;

                  case '7':
                     *_i |= ((sS64)7) << (nc << 2);
                     break;

                  case '8':
                     *_i |= ((sS64)8) << (nc << 2);
                     break;

                  case '9':
                     *_i |= ((sS64)9) << (nc << 2);
                     break;

                  case 'A':
                  case 'a':
                     *_i |= ((sS64)10) << (nc << 2);
                     break;

                  case 'B':
                  case 'b':
                     *_i |= ((sS64)11) << (nc << 2);
                     break;

                  case 'C':
                  case 'c':
                     *_i |= ((sS64)12) << (nc << 2);
                     break;

                  case 'D':
                  case 'd':
                     *_i |= ((sS64)13) << (nc << 2);
                     break;

                  case 'E':
                  case 'e':
                     *_i |= ((sS64)14) << (nc << 2);
                     break;

                  case 'F':
                  case 'f':
                     *_i |= ((sS64)15) << (nc << 2);
                     break;
					}
				} // for..
			} // for..
      } // if <hexmode>..
      else if( ('0' == ((char*)chars)[0]) && ('b' == ((char*)chars)[1]) )
		{
			// binary mode
			ret = YAC_TRUE;
			sUI nc = 0;

			for(i=2; (i < (length - 1)) && ret; i++)
			{
				if( ('0' != ((sChar*)chars)[i]) && ('1' != ((sChar*)chars)[i]) )
            {
					ret = YAC_FALSE;
            }
				else
            {
					nc++;
            }
			}

			if(ret)
			{
				*_i = 0;

				for(i=2; i < (length - 1); i++)
				{
					nc--;
					*_i |= ('1' == ((sChar*)chars)[i]) ? (((sS64)1) << nc) : 0;
				}
			}
		}
		else
		{
			// decimal mode
			ret = YAC_TRUE;
			*_i = 0;
			sS64 imul = 1;

			if(length > 0)
			{
				for(i = (length - 2); (((sSI)i) >= 0); i--)
				{
					if( (((sChar*)chars)[i] < '0') || (((sChar*)chars)[i] > '9') )
					{
						if( (!i) && ('-' == chars[0]) )
						{
							*_i = -*_i;

							return YAC_TRUE;
						}

						*_i = 0;

						return YAC_FALSE;
					}

					(*_i) += imul * (chars[i] - 48);
					imul *= 10;
				}

				return YAC_TRUE;
			}

			*_i = 0;

			return YAC_FALSE;
		} // else..
   } // if chars..

   return ret;
}

sBool YAC_String::scan(sChar *c) const {
	sBool ret = (0 != length) && (NULL != chars);

	if(ret)
   {
      *c = chars[0];
   }
	else
   {
      *c = 0;
   }

	return ret;
}

sBool YAC_VCALL YAC_String::yacEquals(YAC_Object *o) {
   if(YAC_VALID(o))
   {
      if(YAC_Is_String(o))
      {
         YAC_String *s = (YAC_String*)o;

         if( (NULL == chars) || (length <= 1) )
         {
            if( (NULL == s->chars) || (s->length <= 1) )
            {
               // Both strings are empty
               return YAC_TRUE;
            }
         }
         else if( (NULL == s->chars) || (s->length <= 1) )
         {
            if( (NULL == chars) || (length <= 1) )
            {
               return YAC_TRUE;
            }
         }
         else if( (s->length == length) && (NULL != s->chars) && (NULL != chars) )
         {
            sUI oKey = s->getKey();

            if(oKey == getKey())
            {
               sUI i = 0;

               for(; i < length && (s->chars[i] == chars[i]); i++)
                  ;

               return (i == length);
            }
         }
      }
      else
      {
         YAC_String s;

         if(o->yacToString(&s))
         {
            return yacEquals(&s);
         }
      }
   }

   return YAC_FALSE;
}

sBool YAC_VCALL YAC_String::yacToString(YAC_String *_s) const {
   _s->alloc(length + 2);
   _s->append("\"");
   _s->append(this); // (todo) add char replacements \"\'\n\r..
   _s->append("\"");

   return YAC_TRUE;
}

sBool YAC_String::sio_readFile(sBool _local, char *_fname) {
   YAC_String s;
   s.visit(_fname);

   return sio_readFile(_local, &s);
}

sBool YAC_String::sio_readFile(sBool _local, YAC_String *_fname) {
   sSI ret = 0;
   TKS_StreamIO sio(tkscript);

   if(_local)
   {
      sio.openLocal(_fname, 0);
   }
   else
   {
      sio.openLogic(_fname);
   }

   if(sio.isOpen())
   {
      sSI len = sio.getFileSize();

      if(len > 0)
      {
         if(YAC_String::alloc(len+1))
         {
            sio.read(chars, len);
            chars[len] = 0;
            fixLength();
            ret = !sio.fail();
         }
      }
      else
      {
         // empty fail
         empty();
         ret = !sio.fail();
      }

      sio.close();
   }

   return ret;
}

sBool YAC_String::readRawFile(YAC_String*_fname) {
	return (NULL != _fname) ? readRawFile((char*)_fname->chars) : YAC_FALSE;
}

sBool YAC_String::readRawFile(sChar *fname) {
   sSI ret = 0;
   FILE *fh = fopen(fname, "rb");

   if(NULL != fh)
   {
      fseek(fh, 0, SEEK_END);
      sSI len = ftell(fh);
      fseek(fh, 0, SEEK_SET);

      if(len > 0)
      {
         if(YAC_String::alloc(len + 1))
         {
            length = len + 1;
            int nr = int(::fread((char*)chars, 1, len, fh));
            chars[len] = 0;
            ret = (nr > 0);
         }
      }

      fclose(fh);
   }

   key = YAC_LOSTKEY;

   return (0 != ret);
}

sBool YAC_String::sio_writeFile(char *_fname) {
   YAC_String s;
   s.visit(_fname);

   return sio_writeFile(&s);
}

sBool YAC_String::sio_writeFile(YAC_String *_fname) {
   if(length > 1)
   {
      TKS_StreamIO sio(tkscript);

      if(sio.openLocal(_fname, TKS_IOS_OUT))
      {
         sio.write(chars, length - 1);
         sBool r = !sio.fail();
         sio.close();

         return r;
      }
   }

   return YAC_FALSE;
}

sBool YAC_String::writeFile(YAC_String *_fname) {
    return writeFile((char*)_fname->chars);
}

sBool YAC_String::writeFile(sChar *fname) {
   FILE *fh;
   sSI ret = 0;
   fh = ::fopen(fname, "wb");

   if(NULL != fh)
   {
      int nw = int(::fwrite((char*)chars,length?(length-1):length,1,fh));
      ret = (nw > 0);
      ::fclose(fh);
   }

   return (0 != ret);
}

void YAC_String::_trim_YAC_RARG(YAC_Object *_s) {
   if(YAC_CHK(_s, YAC_CLID_STRING))
   {
      YAC_String *s = (YAC_String*) _s;

      s->trim(this);
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_trim_YAC_RVAL(YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);
   s->trim(this);
}

void YAC_String::_trim_YAC_RSELF(void) {
	YAC_String t;
	t.trim(this);
	refCopy(&t);
}

void YAC_String::trim(YAC_String *_src) {
   empty();

	if(NULL != _src)
	{
		if( (_src->length > 0) && (NULL != _src->chars) )
		{
			sSI _src_off = 0;
			sSI _src_end = 0;
			sSI i = 0;
			sSI k = 0;

			for(; i<(sSI)_src->length ; i++)
			{
				switch((sChar)_src->chars[i])
				{
               case ' ':
               case '\t':
               case '\n':
               case '\014':
                  break;

               default:
                  _src_off = i;

                  for(i=(sSI)_src->length-1; i>=0; i--)
                  {
                     switch((sChar)_src->chars[i])
                     {
                        case ' ':
                        case '\t':
                        case '\r':
                        case '\014':
                        case '\n':
                        case 0:
                           break;

                        default:
                           _src_end = i;
                           k = _src_end - _src_off;

                           if(k >= 0)
                           {
                              if(alloc(k + 2))
                              {
                                 k = 0;

                                 for(i=_src_off; i<=_src_end ;)
                                 {
                                    chars[k++]=_src->chars[i++];
                                 }

                                 if(k > 0)
                                 {
                                    chars[k++] = 0;
                                 }

                                 length = k;
                                 key = YAC_LOSTKEY;

                                 return;
                              }
                           }
                     } // switch..
                  } // for..

                  return;
				} // switch..
			} // for..

			return;
		} // if <srclength>..
	} // if <src>..
}

sBool YAC_String::scanProtected(const YAC_String *_src, sSI _mode) {
   // (note) not used (and probably not working since allocation size is wrong)

	if( (NULL != _src) && (NULL != _src->chars) )
	{
		if(YAC_String::alloc(_src->length))
		{
			sUI nl = 0;
			sUI i = 0;
			sBool esc_mode = YAC_FALSE;
			sSI quot_mode = 0;
			sSI quots_mode = 0;

			for(i=0; i<_src->length; i++)
			{
				switch((sChar)_src->chars[i])
				{
               case 0:
                  chars[nl++] = 0;
                  length = nl;
                  return !quots_mode && !(!quots_mode && quot_mode);

               case '\\':
                  if(esc_mode)
                  {
                     chars[nl++] = (sU8)'\\';
                     esc_mode = YAC_FALSE;
                  }
                  else
                  {
                     esc_mode = YAC_TRUE;
                  }
                  break;

               case 'n':
                  if(esc_mode)
                  {
                     chars[nl++] = (sU8)'\n';
                     esc_mode = YAC_FALSE;
                  }
                  else
                  {
                     chars[nl++] = (sU8)'n';
                  }
                  break;

               case 't':
                  if(esc_mode)
                  {
                     chars[nl++] = (sU8)'\t';
                     esc_mode = YAC_FALSE;
                  }
                  else
                  {
                     chars[nl++] = (sU8)'t';
                  }
                  break;

               case '\"':
                  if(quots_mode)
                  {
                     if(esc_mode)
                     {
                        chars[nl++] = (sU8)'\\';
                        chars[nl++] = (sU8)'\"';
                        esc_mode = YAC_FALSE;
                     }
                     else
                     {
                        chars[nl++] = (sU8)'\"';
                     }
                  }
                  else
                  {
                     if(esc_mode)
                     {
                        chars[nl++] = (sU8)'\"';
                        esc_mode = YAC_FALSE;
                     }
                     else
                     {
                        if(quot_mode)
                        {
                           /// end parsing???
                           if(1 == _mode)
                           {
                              chars[nl++] = 0;
                              length = nl;

                              return !quots_mode;
                           }
                           else
                           {
                              quot_mode = 0;
                           }
                        }
                        else
                        {
                           quot_mode = 1;
                        }
                     }
                  }
                  break;

               case '\'':
                  if(esc_mode)
                  {
                     chars[nl++] = (sU8)'\'';
                     esc_mode = YAC_FALSE;
                  }
                  else
                  {
                     quots_mode = (1 - quots_mode);
                  }
                  break;

               default:
                  chars[nl++] = _src->chars[i];
                  esc_mode = YAC_FALSE;
                  break;
				} // switch..
			} // for..
      } // if <realloc>..
   } // if <src>..

   return YAC_FALSE;
}

sU16 YAC_String::checkConversions(sBool _bInternal) const {
   // Used to determine if the string can be scanned for ints/floats
   // and also char constants
   // ret 0=empty, 1=int, 2=float, >2=text (>=256)
   // 4=newline
   // 5=tab
   //
	if(0 == length)
   {
      return 0;
   }

	sU16 i = 0;
	sU16 r = 0;
	char *s = (char*)chars;

	if( ('\'' == *s) && (4 == length) )
	{
      if('\'' == s[2])
      {
         return (sU16)s[1];
      }
      else
      {
         // never called any longer?
         s++;
         i++;
         sBool cesc = YAC_FALSE;

         for(; i < length; i++,s++)
         {
            switch(*s)
            {
               case '\\':
                  cesc = YAC_TRUE; // escape-mode
                  break;

               case 't':
                  if(cesc)
                  {
                     return 5; // tab xxx obsolete
                  }
                  else
                  {
                     return (sU16)'t';
                  }

               case 'n':
                  if(cesc)
                  {
                     return 4; // newline xxx obsolete
                  }
                  else
                  {
                     return (sU16)'n';
                  }

               default:
                  return (sU16)(*s);
            }
         }
      }
	}
	else
	{
		sBool ishex;

		if( ('0' == s[0]) && ('x' == s[1]) )
		{
			// 0xff style
			i++;
			s++;
			ishex = YAC_TRUE;
		}
		else
      {
         ishex = ('$' == *s) || ('#' == *s);
      }

		if(ishex)
		{
			i++;
			s++;

			// try hex
			for(; i < length; i++)
			{
				switch(*s++)
				{
               default:
                  r = 3;
                  break;

               case 0:
               case ' ':
               case '\n':
               case '\t':
                  break;

               case '0':
               case '1':
               case '2':
               case '3':
               case '4':
               case '5':
               case '6':
               case '7':
               case '8':
               case '9':
               case 'a':
               case 'b':
               case 'c':
               case 'd':
               case 'e':
               case 'f':
               case 'A':
               case 'B':
               case 'C':
               case 'D':
               case 'E':
               case 'F':
                  if(0 == r)
                  {
                     r++;
                  }
                  break;
				}
			}
		}
		else
		{
         if(length > 2)
         {
            if( ('0' == s[0]) && ('b' == s[1]) )
            {
               // binary mode
               for(i=2; i < (length - 2); i++)
               {
                  switch(s[i])
                  {
                     case '0':
                     case '1':
                        break;

                     default:
                        return 256 + 1; // isstring
                  }
               }

               return YAC_TYPE_INT;
            }
         }

         sBool bHaveDigit = YAC_FALSE;

         // skip sign
         if('-' == s[0])
            i++, s++;

			for(; i < length; i++)
			{
				switch(*s++)
				{
               case 0:
                  break;

               case ' ':
               case '\n':
               case '\t':
                  return 256 + 3; // isText

               default:
                  r += 256 + 4;
                  return r;

               case 'f': // "3f", "3.1f"
                  if(!r)
                  {
                     return 256 + 1; // isstring
                  }
                  else
                  {
                     return YAC_TYPE_FLOAT;
                  }

               case '0':
               case '1':
               case '2':
               case '3':
               case '4':
               case '5':
               case '6':
               case '7':
               case '8':
               case '9':
                  if(0 == r)
                  {
                     r++; // hmm.. r = 1 ? :)
                     bHaveDigit = YAC_TRUE;
                  }
                  break;

               case '+':
               case '-':
                  if(0 == r)
                  {
                     r++;
                  }
                  else if(2 != r)
                  {
                     return 256 + 3;
                  }
                  break;

                case 'e':
                case 'E':
                   if(r > 2) // must be preceeded by '+'
                   {
                      return 256 + 3;
                   }
                   else if(1 == r) // must be preceeded by number or [+-] number
                   {
                      r = 2;
                   }
                   else if(2 != r)
                   {
                      return 256 + 3;
                   }
                   break;

                case '.':
                   if(0 == i)
                   {
                      r = 2;  // .5
                   }
                   else if(1 == r)
                   {
                      r++;  // 0.5
                   }
                   else
                   {
                      return 256 + 3; // isText
                   }
                   break;
				}
			}

         if(!_bInternal)
         {
            if(!bHaveDigit)
               return 256+3; // isText;
         }
		}

		return r;
	}

	return 256 + 3;  // isText
}

sSI YAC_String::_checkConversions(void) {
   return sSI(checkConversions(YAC_FALSE/*bInternal*/));
}

void YAC_String::fixLength(void) {
   if( (NULL != chars) && (buflen > 0) )
   {
      length = 0;
      char *s = (char*)chars;

      while((length < buflen) && *s++)
      {
         length++;
      }
      length++; // count EOF
      key = YAC_LOSTKEY;
   }
   else
   {
      length = 0;
   }
}

sSI YAC_String::isBlank(void) const {
	if( (NULL != chars) && (length > 0) )
	{
		for(sUI i = 0u; i < length; i++)
		{
			switch(chars[i])
			{
            case 0:
               // should not be reachable
               return YAC_TRUE;

            case ' ':
            case '\n':
            case '\t':
               break;

            default:
               return YAC_FALSE;
			}
		}
	}

	return YAC_TRUE;
}

void YAC_String::removeCR(void) {
	char *s = (char*)chars;

	for(sUI i = 0u; i < length; i++)
	{
		if(13 == s[i])
      {
			s[i]=' ';
      }
	}

   key = YAC_LOSTKEY;
}

void YAC_String::substEscapeSeq(void) {
	sUI i;
	char *s = (char*)chars;
	sBool esc = YAC_FALSE;

	for(i=0; i < length; i++)
	{
		if('\\' == *s)
		{
			if(esc)
			{
				s[-1] = 15;
				*s = 20;
				esc = YAC_FALSE;
			}
			else
         {
            esc = YAC_TRUE;
         }
		}
		else
		{
			if(esc)
			{
				switch(*s)
				{
               case 't':    // tab
                  s[-1] = 15;
                  *s = 16;
                  break;

               case 'n':    // line feed
                  s[-1] = 15;
                  *s = 17;
                  break;

               case '\"':
                  s[-1] = 15;
                  *s = 18;
                  break;

               case '\'':
                  s[-1] = 15;
                  *s = 19;
                  break;

               case 'r':    // carriage return
                  s[-1] = 15;
                  *s = 21;
                  break;

               case 'b':    // backspace
                  s[-1] = 15;
                  *s = 22;
                  break;

               case 'a':    // alarm
                  s[-1] = 15;
                  *s = 23;
                  break;

               case 'f':    // form feed
                  s[-1] = 15;
                  *s = 24;
                  break;

               case 'v':    // vertical tab
                  s[-1] = 15;
                  *s = 25;
                  break;

               case 'e':    // escape
                  s[-1] = 15;
                  *s = 26;
                  break;
				}

				esc = YAC_FALSE;
			}
		}

		s++;
	}

   key = YAC_LOSTKEY;
}


void YAC_String::resubstEscapeSeq(void) {
   // string constants
   // todo: optimize for regular char sequences
	replace("\017\022", "\"");
	replace("\017\020", "\t");
	replace("\017\021", "\n");
	replace("\017\022", "\"");
	replace("\017\023", "\'");
	replace("\017\024", "\\");
	replace("\017\025", "\r");
	replace("\017\026", "\b");
	replace("\017\027", "\a");
	replace("\017\030", "\f");
	replace("\017\031", "\v");
	replace("\017\032", "\033");
}

// scripts from strings
void YAC_String::resubstEscapeSeq2(void) {
	if( (length > 0) && (NULL != chars) )
	{
		sUI blen = (length << 1);
		sU8 *nc = Dyacallocchars(blen);
		sU8 *s = chars;
		sU8 *se = chars + (length - 1);
		sU8 *d = nc;
		sBool quote_mode  = YAC_FALSE;   // '
		sBool dquote_mode = YAC_FALSE;  // "
		sBool anyquote    = YAC_FALSE;
		sBool esc_close   = YAC_FALSE;

		while(s < se)
		{
			switch(*s)
			{
            default:
               *d++ = *s++;
               break;

            case '\017': // start escape sequence?
               switch(s[1])
               {
                  default:
                     *d++ = *s++; // regular ASCII control character
                     break;

                  case '\020': // '\t'
                     if(anyquote)
                     {
                        *d++ = '\\';
                        *d++ = 't';
                     }
                     else
                     {
                        *d++ = '\t';
                     }

                     s += 2;
                     break;

                  case '\021': // '\n'
                     if(anyquote)
                     {
                        *d++ = '\\';
                        *d++ = 'n';
                     }
                     else
                     {
                        *d++ = '\n';
                     }

                     s += 2;
                     break;

                  case '\022': // '\"'
                     if(esc_close)
                     {
                        esc_close = YAC_FALSE;
                        *d++ = '\"';
                        dquote_mode = YAC_FALSE;
                        anyquote = quote_mode;
                     }
                     else if(anyquote)
                     {
                        *d++ = '\\';
                        *d++ = '\"';
                     }
                     else
                     {
                        esc_close = YAC_TRUE;
                        *d++ = '\"';
                        dquote_mode = anyquote = YAC_TRUE;
                     }

                     s += 2;
                     break;

                  case '\023': // '\''
                     if(dquote_mode)
                     {
                        *d++ = '\\';
                        *d++ = '\'';
                     }
                     else
                     {
                        *d++ = '\'';
                     }

                     s += 2;
                     break;

                  case '\024': // '\\'
                     if(anyquote)
                     {
                        *d++ = '\\';
                        *d++ = '\\';
                     }
                     else
                     {
                        *d++ = '\\';
                     }

                     s += 2;
                     break;

                  case '\025': // '\r'
                     if(anyquote)
                     {
                        *d++ = '\\';
                        *d++ = 'r';
                     }
                     else
                     {
                        *d++ = '\r';
                     }

                     s += 2;
                     break;

                  case '\026': // '\b'
                     if(anyquote)
                     {
                        *d++ = '\\';
                        *d++ = 'b';
                     }
                     else
                     {
                        *d++ = '\b';
                     }

                     s += 2;
                     break;

                  case '\027': // '\a'
                     if(anyquote)
                     {
                        *d++ = '\\';
                        *d++ = 'a';
                     }
                     else
                     {
                        *d++ = '\a';
                     }

                     s += 2;
                     break;

                  case '\030': // '\f'
                     if(anyquote)
                     {
                        *d++ = '\\';
                        *d++ = 'f';
                     }
                     else
                     {
                        *d++ = '\f';
                     }

                     s += 2;
                     break;

                  case '\031': // '\v'
                     if(anyquote)
                     {
                        *d++ = '\\';
                        *d++ = 'v';
                     }
                     else
                     {
                        *d++ = '\v';
                     }

                     s += 2;
                     break;

                  case '\032': // '\033'
                     if(anyquote)
                     {
                        *d++ = '\\';
                        *d++ = 'e';
                     }
                     else
                     {
                        *d++ = '\033';
                     }

                     s += 2;
                     break;
               }
               break;

            case '\"':
               if(!quote_mode)
               {
                  dquote_mode = 1 - dquote_mode;
                  anyquote = dquote_mode;
               }

               *d++ = '\"';
               s++;
               break;

            case '\'':
               if( (0x0F == s[1]) && ('\'' == s[3]) )
               {
                  *d++ = '\'';
                  *d++ = '\\';

                  // "int i='\n'"
                  switch(s[2])
                  {
                     case '\020':
                        *d++ = 't';
                        break;

                     case '\021':
                        *d++ = 'n';
                        break;

                     case '\022':
                        *d++ = '\"';
                        break;

                     case '\023':
                        *d++ = '\'';
                        break;

                     case '\024':
                        *d++ = '\\';
                        break;

                     case '\025':
                        *d++ = 'r';
                        break;

                     case '\026':
                        *d++ = 'b';
                        break;

                     case '\027':
                        *d++ = 'a';
                        break;

                     case '\030':
                        *d++ = 'f';
                        break;

                     case '\031':
                        *d++ = 'v';
                        break;

                     case '\032':
                        *d++ = 'e';
                        break;
                  }

                  *d++ = '\'';
                  s += 4;
               }
               else
               {
                  if(!dquote_mode)
                  {
                     quote_mode = 1 - quote_mode;
                     anyquote = quote_mode;
                  }

                  *d++ = '\'';
                  s++;
               }
               break;
			}
		}

		*d++ = 0;
		length = sUI(d - nc);
      safeFreeChars();
		chars  = nc;
		buflen = blen;
      bflags = TKS_STRFL_DEL | (bflags&TKS_STRFLQMASK);
	}

   key = YAC_LOSTKEY;
}

void YAC_String::printf(const char *_fmt, ...) {
   va_list va;
   va_start(va, _fmt);
#ifdef YAC_VC
   _vsnprintf((char*)chars, buflen, _fmt, va);
#else
#ifdef HAVE_VSNPRINTF
   vsnprintf((char*)chars, buflen, _fmt, va);
#else
#warning "compiling without VSNPRINTF() support\n"
   vsprintf((char*)chars, _fmt, va);
#endif
#endif
   va_end(va);
   fixLength();
}

sBool YAC_VCALL YAC_String::yacArrayAlloc(sUI _len, sUI, sUI, sUI) {
   return YAC_String::alloc(_len);
}

sBool YAC_VCALL YAC_String::yacArrayRealloc(sUI _len, sUI, sUI, sUI) {
   return YAC_String::_realloc(_len);
}

void YAC_VCALL YAC_String::yacArraySet(void *_context, sUI _index, YAC_Value*_value) {
	if(_index < buflen)
	{
      sU8 c;
		switch(_value->type)
		{
         default:
            c = '?';
            break;

         case 1:
            c = (sU8) _value->value.int_val;
            break;

         case 2:
            c = (sU8) _value->value.float_val;
            break;

         case 3:
			{
				sSI ti;

				if(YAC_VALID(_value->value.object_val))
				{
					if(YAC_BCHK(_value->value.object_val, YAC_CLID_STRING))
					{
						YAC_String *ts = _value->value.string_val;
						ts->yacScanI(&ti);
						c = (sU8)ti;
					}
               else
               {
                  c = '?';
               }
				}
            else
            {
               c = '?';
            }
			}
			break;
		}

      if(0 == length)
      {
         chars[0] = 0;
         length = 1;
      }

      if(_index == ((sUI)length - 1))
      {
         if(c > 0)
         {
            if((_index + 1) < buflen)
            {
               chars[_index] = c;
               chars[_index+1] = 0;
               length++;
               key = YAC_LOSTKEY;
            }
            else
            {
               YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
               msg->printf("String index >>%i<< out of bounds (max=%i)", _index, buflen);
               PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
               PTN_Env*_env = &env;
               Drtthrow(NULL, TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS, msg);
            }
         }
      }
      else
      {
         chars[_index] = c;
         chars[length-1] = 0;
         key = YAC_LOSTKEY;
      }
	}
   else
   {
      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      msg->printf("String index >>%i<< out of bounds (max=%i)", _index, buflen);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS, msg);
	}
}

void YAC_VCALL YAC_String::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
	if(((sUI)_index) < ((sUI)length))
   {
      _r->initInt((sSI)chars[_index]);
   }
	else
   {
      _r->initVoid();

      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      msg->printf("String index >>%i<< out of bounds (num=%i max=%i)", _index, ((sUI)length), buflen);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
}

void YAC_VCALL YAC_String::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   ////Dprintf("xxx String::yacOperator: _cmd=%i o=%p\n", _cmd, _o);

   if(YAC_BCHK(_o, YAC_CLID_STRING))
   {
      YAC_String *o = (YAC_String*)_o;

      switch(_cmd)
      {
         case YAC_OP_ASSIGN:
            yacCopy(o);
            return;

         case YAC_OP_ADD:
            append(o);
            return;

         case YAC_OP_AND:
         {
            sSI r = indexOf(o, 0);
            YAC_RETI(-1 != r);
         }
         return;

         case YAC_OP_CEQ:
            YAC_RETI(compare(o));
            return;

         case YAC_OP_CNE:
            YAC_RETI(!compare(o));
            return;

         case YAC_OP_CLE:
            YAC_RETI(startsWith(o));
            return;

         case YAC_OP_CGE:
            YAC_RETI(endsWith(o));
            return;

         case YAC_OP_CLT:
            YAC_RETI(isPredecessor(o, 1));
            return;

         case YAC_OP_CGT:
            YAC_RETI(!isPredecessor(o, 1));
            return;

         case YAC_OP_LAND:
            YAC_RETI( (!isBlank()) && (!o->isBlank()) );
            return;

         case YAC_OP_LOR:
            YAC_RETI( (!isBlank()) || (!o->isBlank()) );
            return;
      }
   }
   else
   {
      if(YAC_OP_ADD == _cmd)
      {
         if(YAC_VALID(_o))
         {
            YAC_String s;

            if(!_o->yacToString(&s))
            {
               s.alloc(256);
               s.printf("<Object \"%s\" @" YAC_PRINTF_PTRPREFIX "%p>", (char*)_o->yacClassName(), (void *)_o);
            }

            append(&s);
         }
         return;
      }
      else if(YAC_OP_ASSIGN == _cmd)
      {
         if(YAC_VALID(_o))
         {
            _o->yacToString(this);

            return;
         }
         else
         {
            //Dprintf("\n\n\nxxx String::yacOperator ASSIGN: invalid rhs object (o=%p)\n", _o);
            empty();
         }
      }
   }

   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_VCALL YAC_String::yacOperatorI(sSI _cmd, sSI _intval, YAC_Value*) {
   switch(_cmd)
   {
      case YAC_OP_ADD:
      {
         YAC_String s;
         s.yacValueOfI(_intval);
         append(&s);
      }
      break;

      case YAC_OP_MUL:
         if(_intval > 0)
         {
            // (note) string copy could be avoided
            // (note) however, in a doubleargexpr a new string needs to be returned and a different code path is used, anyway
            //         (see PTN_DoubleArgExpr string-int, int-string)
            YAC_String s;
            s.yacCopy(this);
            empty();
            _appendRepeat_YAC_RSELF(&s, _intval);
         }
         break;

      case YAC_OP_ASSIGN:
         yacValueOfI(_intval);
         break;
   }
}

void YAC_VCALL YAC_String::yacOperatorF32(sSI _cmd, sF32 _floatval, YAC_Value*) {
   switch(_cmd)
   {
      case YAC_OP_ADD:
      {
         YAC_String s;
         s.yacValueOfF32(_floatval);
         append(&s);
      }
      break;

      case YAC_OP_ASSIGN:
         yacValueOfF32(_floatval);
         break;
   }
}

void YAC_VCALL YAC_String::yacOperatorF64(sSI _cmd, sF64 _doubleval, YAC_Value*) {
   switch(_cmd)
   {
      case YAC_OP_ADD:
      {
         YAC_String s;
         s.yacValueOfF64(_doubleval);
         append(&s);
      }
      break;

      case YAC_OP_ASSIGN:
         yacValueOfF64(_doubleval);
         break;
   }
}

sSI YAC_String::_numIndicesOf(const YAC_String *_s) const {
   return numIndicesOf(_s);
}

void YAC_String::_replace_YAC_RARG(const YAC_String *_a, const YAC_String *_b, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_String *s = (YAC_String *) _r;

      s->yacCopy(this);
      s->replace(_a, _b);
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_replace_YAC_RVAL(const YAC_String *_a, const YAC_String *_b, YAC_Value *_r) {
   YAC_String *s = (YAC_String *) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);
   s->yacCopy(this);
   s->replace(_a, _b);
}


void YAC_String::_replace_YAC_RSELF(const YAC_String *_a, const YAC_String *_b) {
   replace(_a, _b);
}

sSI YAC_String::_replaceReturnNum(const YAC_String *_s1, const YAC_String *_s2) {
   return replace(_s1, _s2);
}

sSI YAC_String::_indexOf(const YAC_Object *_s, sSI _i) const {
   if(YAC_BCHK(_s, YAC_CLID_STRING))
   {
      return indexOf((const YAC_String*)_s, _i);
   }
   else
   {
      return -1;
   }
}

sSI YAC_String::_lastIndexOf(const YAC_Object *_s) const {
   if(YAC_BCHK(_s, YAC_CLID_STRING))
   {
      return lastIndexOf((const YAC_String*)_s, 0);
   }
   else
   {
      return -1;
   }
}

sSI YAC_String::_load(YAC_String *_s, sSI _ascii) {
   sBool r = sio_readFile(0, (char*)_s->chars);

   if(r)
   {
      if(_ascii)
      {
         removeCR();
      }

      return YAC_TRUE;
   }

   return YAC_FALSE;
}

sSI YAC_String::_loadLocal(YAC_String *_s, sSI _ascii) {
   sBool r = sio_readFile(1, (char*)_s->chars);

   if(_ascii)
   {
      removeCR();
   }

   return r;
}

sSI YAC_String::_saveLocal(YAC_String *_s) {
   return sio_writeFile((char*)_s->chars);
}


sBool YAC_String::_alloc(sUI _numbytes) {
   return YAC_String::alloc(_numbytes);
}

sSI YAC_String::realloc(sUI _numBytes, sBool _bCopy) {
   // (note) realloc 'numBytes' chars (excluding ASCIIZ)

   // Force alloc if a very big string is about to shrink a lot or there is simply not enough space available
   if( ((buflen >= 1024) && ( (_numBytes<<3) < buflen )) ||
       (NULL == chars) ||
       (buflen < (_numBytes+1u))
       ) // xxx (!chars) hack added 180702
   {
      if(NULL != chars)
      {
         sUI l = length;

         if(_numBytes < l)
         {
            l = _numBytes;
         }

         sU8 *nc = Dyacallocchars(_numBytes + 1);
         sUI i = 0u;

         if(_bCopy)
         {
            // Copy chars except for ASCIIz
            sUI numChars = getNumChars();
            sUI lCopy = sMIN(numChars, _numBytes);
            for(; i < lCopy; i++)
            {
               nc[i] = chars[i];
            }
         }

         nc[i] = 0u;  // ASCIIZ

         safeFreeChars();
		   buflen = (_numBytes + 1);
         bflags = TKS_STRFL_DEL | (bflags & TKS_STRFLQMASK); // keep old stringflags
         length = i + 1;
         chars  = nc;
         key    = YAC_LOSTKEY;

         return YAC_TRUE;
      }
      else
      {
		   return YAC_String::alloc(_numBytes + 1);
      }
   }
	else
	{
      key = YAC_LOSTKEY; // new 010208

      if(length > 0u)
      {
         if(_numBytes < (length - 1u))
         {
            length = _numBytes;
            if(length > 0u)
               chars[length-1] = 0;
            else
               chars[0] = 0;
         }
      }

		return YAC_TRUE;
	}
}

sBool YAC_String::_realloc(sSI _numBytes) {
   return (_numBytes >= 0) ? realloc(_numBytes, YAC_TRUE/*bCopy*/) : YAC_FALSE;
}

sSI YAC_String::_getc(sSI _byteoffset) const {
   return (int)((*this)[_byteoffset]);
}


void YAC_String::_putc(int _byteoffset, int _char) {
   if(((sUI)_byteoffset) < buflen)
   {
      if(0 != chars[_byteoffset]) // do not overwrite ASCIIZ
      {
         chars[_byteoffset] = (sU8)_char;
         key = YAC_LOSTKEY;
      }
   }
}

void YAC_String::_append_YAC_RSELF(const YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      if(!YAC_Is_String(_s))
      {
         YAC_String t;

         if(_s->yacToString(&t))
         {
            append(&t);
         }
      }
      else
      {
         append((const YAC_String *) _s);
      }
   }
}

void YAC_String::_append_YAC_RARG(const YAC_Object *_s, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_String *r = (YAC_String*) _r;

      if(YAC_VALID(_s))
      {
         if(! YAC_Is_String(_s))
         {
            YAC_String t;

            if(_s->yacToString(&t))
            {
               r->_copy(this);
               r->append(&t);
            }
         }
         else
         {
            r->_copy(this);
            r->append((YAC_String *) _s);
         }
      }
   }
}

void YAC_String::_append_YAC_RVAL(const YAC_Object *_s, YAC_Value *_r) {
   YAC_String *r = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   r->_copy(this);
   _r->initString(r, 1);
   r->_append_YAC_RSELF(_s);
}

void YAC_String::_appendRepeat_YAC_RSELF(const YAC_String *_s, sSI _numRepeats) {
   if(_numRepeats >= 1)
   {
      if(YAC_Is_String(_s))
      {
         if(_s->length > 0u)
         {
            if(2u == _s->length)
            {
               sUI k = getNumChars();
               // Special (but common) case: repeat single character
               if(realloc(length + sUI(_numRepeats), YAC_TRUE/*bCopy*/))
               {
                  sU8 c = _s->chars[0];
                  ::memset((void*)(chars + k), c, sUI(_numRepeats));
                  length = k + _numRepeats;
                  chars[length++] = 0u;
                  key = YAC_LOSTKEY;
               }
               // else: allocation failed
            }
            else
            {
               // Append arbitrary length string
               sUI sl = _s->getNumChars();
               if(realloc(length + (sl * sUI(_numRepeats)), YAC_TRUE/*bCopy*/))
               {
                  for(sUI i = 0u; i < sUI(_numRepeats); i++)
                  {
                     _append_YAC_RSELF(_s);
                  }
               }
               // else: allocation failed
            }
         }
         else
         {
            // Nothing to append
         }
      }
   }
}

void YAC_String::_appendRepeat_YAC_RARG (const YAC_String *_s, sSI _numRepeats, YAC_Object *_r) {
   if(YAC_Is_String(_r))
   {
      YAC_CAST_ARG(YAC_String, r, _r);
      r->yacCopy(this);
      r->_appendRepeat_YAC_RSELF(_s, _numRepeats);
   }
}

void YAC_String::_appendRepeat_YAC_RVAL(const YAC_String *_s, sSI _numRepeats, YAC_Value *_r) {
   _r->initNewString(this);
   _r->value.string_val->_appendRepeat_YAC_RSELF(_s, _numRepeats);
}

sBool YAC_VCALL YAC_String::yacIteratorInit(YAC_Iterator *_it) const {
	Diteratorinit(TKS_StringStackIterator);
   return YAC_TRUE;
}

// pattern matching (note: this is a very old routine I wrote 8 years ago [1998], pcre should be used instead..)

/** small tool func that matches two chars, allowing wildcards [*?] for char b */
#define char_match(a,b) ( (a==b) || (b=='*') || (b=='?') )

/** Check if string s matches f. No wildcard substitution will be done in this
 * routine, it will return if it encounters [*?] in f.
 */
struct sfmatch {
  int match;
  int off;
};
static struct sfmatch wildcard_submatch(char *s, char *f) {
  struct sfmatch ret={1,0};
  char c;
  char d;

  /** run loop as long as f matches s */
  while( ret.match ) {
    d=*f;
    c=*s;
    /** Examine char from f */
    switch(d) {
    case '?':
    case '*':
      /** Break on wildcards [*?] */
      return ret;
    case 0:
      /** Break on string end */
      return ret;
    default:
      /** Compare char from s with char from f */
      if(c==d) {
	/** If they match, advance to next char from s/f */
	s++;
	f++;
	/** Mark the offset for the match */
	ret.off++;
      } else
	/** If they DONT match, cause the while() loop to exit */
	ret.match=0;
      break;
    }
  }
  /** Return match-result (True/False) and number of matched chars (offset) */
  return ret;
}


/** Examine string s and return offset of first non-wildcard character and
 * the number of jokers (?) in between..
 */
struct nmatch {
  /** Offset for first non-wildcard char */
  int off;
  /** number of jokers left */
  int joker;
};
static struct nmatch wildcard_nextmatch(char *s) {
    /** Initalize return to 0 jokers and offset 0. */
    struct nmatch ret={0,0};
    /** Get first char from string */
    char c=*s;
    /** Examine first char */
    if(c=='?')
        return ret.joker=1,ret;
    /** Until end of string */
    while((c=*s++)) {
        /*     printf("nm/c:%c\n",c); */
        switch(c) {
            /** Is this a joker? */
        case '?':
            /** Increase joker-count */
            ret.joker++;
            if(!*s)
                return ret;
        case '*':
            /** run into [*] label / increase offset */
            ret.off++;
            break;
        default:
            /** a non-wildcard character! return.. */
            return ret;
        }
    }
    /** EOF caught, return! (unexpected end of string) */
    /*   ret.off=0;  */
    /*   ret.joker=0;  */
    return ret;
}


static int wildcard_match(char *s, char *ws) {

    /** work string pointer a (source) ->inner loop */
    char *sa=  s;
    /** work string pointer b (wildcard) ->inner loop */
    char *sb=  ws;
    /** current char from source string s */
    char ca = *sa++;
    /** current char from wildcard string ws */
    char cb ;
    /** next char from wildcard string ws to be matched (sb[1]) */
    char nb;
    /** holds offset to next match and number of jokers in between */
    struct nmatch nm;
    /** holds return values from submatch() (checks a given string against
    * source string and counts how many non-wildcard characters are matched )
    */
    struct sfmatch sub_ret;

    /** current offset in string s */
    int i = 0;

    /** Initalize work string pointer b (sb) to wildcard string (ws) */
    sb=ws;

    /** Get first non-wildcard character to match */
    nm=wildcard_nextmatch(sb);

    /** Go ! */
    while( 1 ) {
        /** Get current byte from work string pointer b */
        cb=*sb;
        /** End of wildcard string ? leave loop */
        if(!cb)
            break;
        /** Subsequent char from wildcard string */
        nb=sb[1];
        //TRACE( fprintf(stderr, "nm=%c(%i) cb=%c nb=%c ca=%c jokerleft=%i\n",
        //	  sb[nm.off], nm.off,  cb, nb, ca, nm.joker )
        //   );
        /** increase current offset in string */
        i++;
        /** end of source string? break. */
        if(!ca)
            break;

        /** Examine char from wildcard string */
        switch(cb) {
        case '*':
            /** Any char? */
            if(!nb)
            /** Endof wildcard string? any data can follow -> we can
            * return now !
            */
            return 0;
            /** Does the current char from source string match the next non-wildcard
            * char in wildcard-string ? (calc'd by nextmatch())
            */
            if(!nm.joker&&ca==sb[nm.off]) {
            /** Single char seems to match, now check if current sequence of
            * non-wildcard character from source string matches the char sequence
            * in wildcard string at the suspected offset nm.off
                */
                if( (sub_ret=wildcard_submatch(sa-1, &sb[nm.off])).match ) {
                /** Increase wildcard pointer by offset of matched char sequence
                * + length of matched char sequence
                    */
                    sb+= sub_ret.off + nm.off ;
                    /** Increase source pointer by number of matched chars */
                    sa+= sub_ret.off -1;
                    /** Get new current source char from pointer */
                    ca=*sa++;
                    /** Get new next matching wildcard-char from wildcard string */
                    nm=wildcard_nextmatch(sb);
                    break;
                }
            }
            /** The first char does NOT seem the match -> the suspected char
            * sequence is different and we need to continue parsing the
            * source string from the current position on..
            */
            ca=*sa++;

            /** Special case in which the pattern string ends with *? */
            if(sb[nm.off]=='?'&&nm.joker==1) {
                return 0;
            }
            /** Since we just used one of our wildcards we decrease the
            * number of jokers left.
            */
            if(nm.joker)
                nm.joker--;
            break;
        case '?':
        /** Any char must match the current one
        * ca is always valid here, the checking is done above!
            */
            ca=*sa++;
            /** Increase wildcard string work pointer */
            sb++;
            /** Get index of next matching char.. */
            nm=wildcard_nextmatch(sb);
            break;
        default:
        /** A regular char from wildcard-string will be directly compared
        * with the current char from the source string..
            */
            if(ca!=cb) {
                /** No match? Then return current offset in source string (ERROR!) */
                return(int) (sa-s);
            }
            /** They do match! */
            ca=*sa++;
            /** Increase wildcard string work pointer */
            sb++;
            /** Get index of next matching char.. */
            nm=wildcard_nextmatch(sb);
            break;
        }
    }

    /** Get index of next matching char.. (if any!) */
    nm=wildcard_nextmatch(sb);

    /*   TRACE( printf("nm=%c Ecb=%c ca=\'%c\' jokerleft=%i\n", sb[nm.off], cb, ca, nm.joker ) ); */

    /** The final check! Do the current chars from source/wildcard match (0!),
    * does the current char match the next char from wildcard string (if any!)
    * and finally: Have we forgotten some jokers?
    */
    if( char_match(ca,cb) && char_match(ca,sb[nm.off]) && !nm.joker )
        /** everything is OK */
        return 0;
    else
    /** This might be considered a problem. The return value will
    * in this case contain the position of the first char NOT matched
    */
    return i+1;
}

sSI YAC_String::_patternMatch(YAC_Object *_pattern) {
   if(YAC_BCHK(_pattern, YAC_CLID_STRING))
   {
      return wildcard_match((char*)chars, (char*)((YAC_String*)_pattern)->chars) ? 0 : 1;
   }
   else
   {
      return 0xFFFF;
   }
}

void YAC_String::_copy(YAC_Object *_s) {
   YAC_String::yacCopy(_s);
}

YAC_String *YAC_String::getWord(sSI _i) const {
   StaticList::Node *cl = clones;
   int ncl = 0;

   if(NULL != cl)
   {
      ncl++;

      while(NULL != cl->next)
      {
         cl = cl->next;
         ncl++;
      }

      if(_i < ncl)
      {
         int i = 0;

         for(; i<_i; i++)
         {
            cl=cl->previous;
         }

         return (YAC_String*)cl->data;
      }
      else
      {
         return tks_empty_string;
      }
   }
   else
   {
      return tks_empty_string;
   }
}

sSI YAC_String::_startsWith(const YAC_String *_s) const {
   return startsWith(_s);
}

sSI YAC_String::_endsWith(const YAC_String *_s) const {
   return endsWith(_s);
}

void YAC_String::_parseXML(YAC_Value *_r) {
   TKS_XMLParser parser;
   parser.parse(this, 0, _r);
}

// xxx TKS_MT: not thread safe (requires global interpreter lock)
#define MAX_STATIC_SUBSTRINGS  (2048)
static sUI YAC_String_coffsets[MAX_STATIC_SUBSTRINGS*2];
static sUI YAC_String_cquot[MAX_STATIC_SUBSTRINGS];

void YAC_String::_splitSpace_YAC_RVAL(sSI _use_quot, YAC_Value *_r) {
	YAC_StringArray *al = (YAC_StringArray*)YAC_NEW_CORE_POOLED(YAC_CLID_STRINGARRAY);
   _r->initObject(al, 1);

   _splitSpace_YAC_RARG(_use_quot, al);
}

void YAC_String::_splitSpace_YAC_RARG(sSI _use_quot, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRINGARRAY))
   {
      YAC_StringArray *al = (YAC_StringArray*) _r;
      al->empty();

#ifdef TKS_MT
      (void)tkscript->lockGlobalContext();
#endif //TKS_MT

      sSI ret = (length>1) ? 1 : 0;

      if(ret)
      {
         ret = 0;
         sUI i = 0, j = 0, quot = 0, sqm = 0; // \" and \' quotation marks
         sBool spaced = YAC_TRUE;
         sUI *offsets;

         sUI maxSubStrings = ((length >> 1) + 1);

         sUI *quotFlags = NULL;

         if(maxSubStrings < MAX_STATIC_SUBSTRINGS)
         {
            offsets = YAC_String_coffsets;
            quotFlags = YAC_String_cquot;
         }
         else
         {
            offsets = new(std::nothrow) sUI[maxSubStrings << 1];

            if(_use_quot)
            {
               quotFlags = new(std::nothrow) sUI[maxSubStrings];
            }
         }

         sUI *op = offsets;
         sUI *cq = quotFlags;

         // ---- determine number of occurences ----
         for(; i < length; i++)
         {
            switch(chars[i])
            {
               case '\n':
               case '\014':
               case '\t':
               case ' ':
               case '\000':
                  if(!spaced && (!(quot&1)) && (!(sqm&1)) )
                  {
                     ret++;

                     if(quot > 0)
                     {
                        *op++ = j;
                        *op++ = i - j - 1;
                        if(_use_quot && (quot > 0))
                           *cq++ = TKS_STRFL_QUOT;
                        quot -= 2;
                     }
                     else if(sqm > 0)
                     {
                        *op++ = j;
                        *op++ = i - j - 1;
                        if(_use_quot && (sqm > 0))
                           *cq++ = TKS_STRFL_QUOT2;
                        sqm -= 2;
                     }
                     else
                     {
                        *op++ = j;
                        *op++ = i - j;
                        if(_use_quot)
                           *cq++ = 0;
                     }

                     spaced = YAC_TRUE;
                  }
                  break;

               case '\'':
                  if(_use_quot&& (!(quot&1)) ) // ignore with "..."
                  {
                     if(!spaced && (!(quot&1)) && (!(sqm&1)) )
                     {
                        ret++;

                        if(quot > 0)
                        {
                           *op++ = j;
                           *op++ = i - j - 1;
                           if(_use_quot && (quot > 0))
                              *cq++ = TKS_STRFL_QUOT;
                           quot -= 2;
                        }
                        else if(sqm > 0)
                        {
                           *op++ = j;
                           *op++ = i - j - 1;
                           if(_use_quot && (sqm > 0))
                              *cq++ = TKS_STRFL_QUOT2;
                           sqm -= 2;
                        }
                        else
                        {
                           *op++ = j;
                           *op++ = i - j;
                           if(_use_quot)
                              *cq++ = 0;
                        }

                        spaced = YAC_TRUE;
                     }

                     sqm++;
                  }
                  else
                  {
                     if(spaced)
                     {
                        j = i;
                     }

                     spaced = YAC_FALSE;
                  }
                  break;

               case '\"':
                  // yac_host->printf("xxx \", sqm=%d quot=%d\n", sqm, quot);
                  if(_use_quot&& (!(sqm & 1)) ) // ignore within '...'
                  {
                     quot++;
                     if(!(quot&1))
                     {
                        // End of "str"
                        ret++;

                        // yac_host->printf("xxx end of \"str\", j=%d i=%d\n", j, i);

                        *op++ = j;
                        *op++ = i - j;
                        if(_use_quot)
                           *cq++ = TKS_STRFL_QUOT;
                        quot -= 2;

                        spaced = YAC_TRUE;
                     }
                     else
                     {
                        // Start of "str", emit prev substring
                        if(!spaced)
                        {
                           ret++;
                           // yac_host->printf("xxx start of \"str\", j=%d i=%d\n", j, i);
                           *op++ = j;
                           *op++ = i - j;
                           if(_use_quot)
                              *cq++ = 0;

                           spaced = YAC_TRUE;
                        }
                        else
                        {
                           // yac_host->printf("xxx start of \"str\" (spaced) i=%d\n", i);
                        }
                     }
                     break;
                  }
                  else
                  {
                     if(spaced)
                     {
                        j = i;
                     }

                     spaced = YAC_FALSE;
                  }
                  break;

               default:
                  // yac_host->printf("xxx defchar \'%c\' i=%d spaced=%d\n", chars[i], i, spaced);
                  if(spaced)
                  {
                     j = i;
                  }

                  spaced = YAC_FALSE;
                  break;
            }
         }

         if(op != offsets)
         {
            if(al->alloc(ret))
            {
               al->num_elements = ret;
               *op++ = 0;
               *op++ = 0;
               YAC_String *cs = al->elements;
               op = offsets;
               sUI *op2 = offsets + 1;
               sUI cop;

               cq = quotFlags;

               while(0 != (cop = *op2))
               {
                  substring(cs, *op, cop);
                  if(_use_quot)
                     cs->bflags |= *cq++;
                  cs++;
                  op  += 2;
                  op2 += 2;
               }
            }
         }

         if(length >= MAX_STATIC_SUBSTRINGS)
         {
            delete [] offsets;

            if(_use_quot)
            {
               delete [] quotFlags;
            }
         }

      }

#ifdef TKS_MT
      tkscript->unlockGlobalContext();
#endif //TKS_MT

   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected StringArray return object");
   }
}


void YAC_String::_splitCharset_YAC_RVAL(const YAC_Object *_cs, YAC_Value *_r) {
	YAC_StringArray *al = (YAC_StringArray*)YAC_NEW_CORE_POOLED(YAC_CLID_STRINGARRAY);
   _r->initObject(al, 1);

   _splitCharset_YAC_RARG(_cs, al);
}

void YAC_String::_splitCharset_YAC_RARG(const YAC_Object *_cs, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRINGARRAY))
   {
      YAC_StringArray *al = (YAC_StringArray*) _r;
      al->empty();

      if(YAC_VALID(_cs))
      {
         if(YAC_Is_String(_cs))
         {
            YAC_String *cs = (YAC_String*)_cs;

            if(cs->length > 1)
            {
               sSI ret = (length > 1) ? 1 : 0;

               if(ret)
               {
                  ret = 0;
                  sUI i = 0, j = 0, quot = 0, sqm = 0; // \" and \' quotation marks
                  sBool spaced = YAC_TRUE;
                  sUI *offsets;

                  if(length <= MAX_STATIC_SUBSTRINGS)
                  {
                     offsets = YAC_String_coffsets;
                  }
                  else
                  {
                     offsets = new(std::nothrow) sUI[((length >> 1) + 1) << 1];
                  }

                  sUI *op = offsets;

                  // ---- determine number of occurences ----
                  for(; i < length; i++)
                  {
                     sU8 c = chars[i];
                     sUI csi=0;

                     for(; csi < cs->length; csi++) // also compare asciiz
                     {
                        if(c == cs->chars[csi])
                        {
                           break;
                        }
                     }

                     if(csi != cs->length)
                     {
                        if(!spaced && (!(quot&1)) && (!(sqm&1)) )
                        {
                           ret++;

                           if(quot > 0)
                           {
                              // (note) never reached ?!! use tokenize() instead
                              *op++ = j;
                              *op++ = i - j - 1;
                              quot -= 2;
                           }
                           else
                           {
                              if(sqm > 0)
                              {
                                 // (note) never reached ?!! use tokenize() instead
                                 *op++ = j;
                                 *op++ = i - j - 1;
                                 sqm -= 2;
                              }
                              else
                              {
                                 *op++ = j;
                                 *op++ = i - j;
                              }
                           }

                           spaced = YAC_TRUE;
                        }
                     }
                     else
                     {
                        if(spaced)
                        {
                           j = i;
                        }

                        spaced = 0;
                     }
                  } // for each char

                  if(op != offsets)
                  {
                     if(al->alloc(ret))
                     {
                        al->num_elements = ret;
                        *op++ = 0;
                        *op++ = 0;
                        YAC_String *cs2 = al->elements;
                        op = offsets;
                        sUI *op2 = offsets + 1;
                        sUI cop;

                        while(0 != (cop = *op2))
                        {
                           substring(cs2++, *op, cop);
                           op  += 2;
                           op2 += 2;
                        }
                     }
                  }

                  if(length > MAX_STATIC_SUBSTRINGS)
                  {
                     delete [] offsets;
                  }
               }

               return;
            }
            else
            {
               Dprintf("[---] YAC_String::splitCharset: charset needs to hold at least 1 char.\n");
            }
         }
         else
         {
            // Charset object is not a string
            Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "_charset is not a String object");
         }
      }
      else
      {
         // Not a valid charset object
         Dyac_throw_def(INVALIDPOINTER, "_charset is not a valid Object");
      }
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected StringArray return object");
   }
}

sSI YAC_String::_wordIndexAtCharIndex(sUI _charIdx, sSI _use_quot) {
   // (note) used in StatusBar widget for tab autocompletion
   // (note) returnd wordIdx will be out of bounds if cursor is at end of string and string ends with blank char
   sSI ret = -1;

   if(length > 1)
   {
      sUI i = 0, quot = 0, sqm = 0; // \" and \' quotation marks
      // sUI j = 0;
      sBool spaced = YAC_TRUE;
      sBool bIncRetAfterSpace = YAC_FALSE;

      ret = 0;

      // sUI *op = offsets;
      // sUI *cq = quotFlags;

      // ---- determine number of occurences ----
      for(; i < length; i++)
      {
         if(bIncRetAfterSpace)
         {
            ret++;
            bIncRetAfterSpace = YAC_FALSE;
         }

         switch(chars[i])
         {
            case '\n':
            case '\014':
            case '\t':
            case ' ':
            case '\000':
               if(!spaced && (!(quot&1)) && (!(sqm&1)) )
               {
                  if(quot > 0)
                  {
                     // // *op++ = j;
                     // // *op++ = i - j - 1;
                     // // if(_use_quot && (quot > 0))
                     // //    *cq++ = TKS_STRFL_QUOT;
                     quot -= 2;
                  }
                  else if(sqm > 0)
                  {
                     // // *op++ = j;
                     // // *op++ = i - j - 1;
                     // // if(_use_quot && (sqm > 0))
                     // //    *cq++ = TKS_STRFL_QUOT2;
                     sqm -= 2;
                  }
                  else
                  {
                     // // *op++ = j;
                     // // *op++ = i - j;
                     // // if(_use_quot)
                     // //    *cq++ = 0;
                  }

                  spaced = YAC_TRUE;
                  bIncRetAfterSpace = YAC_TRUE;
               }
               break;

            case '\'':
               if(_use_quot&& (!(quot&1)) ) // ignore with "..."
               {
                  if(!spaced && (!(quot&1)) && (!(sqm&1)) )
                  {
                     if(quot > 0)
                     {
                        // // *op++ = j;
                        // // *op++ = i - j - 1;
                        // // if(_use_quot && (quot > 0))
                        // //    *cq++ = TKS_STRFL_QUOT;
                        quot -= 2;
                     }
                     else if(sqm > 0)
                     {
                        // // *op++ = j;
                        // // *op++ = i - j - 1;
                        // // if(_use_quot && (sqm > 0))
                        // //    *cq++ = TKS_STRFL_QUOT2;
                        sqm -= 2;
                     }
                     else
                     {
                        // // *op++ = j;
                        // // *op++ = i - j;
                        // // if(_use_quot)
                        // //    *cq++ = 0;
                     }

                     spaced = YAC_TRUE;
                     ret++;
                  }

                  sqm++;

                  // // if(1 == sqm)
                  // // {
                  // //    ret++;
                  // //    bIgnoreLast = YAC_TRUE;
                  // // }
               }
               else
               {
                  if(spaced)
                  {
                     // j = i;

                     // // ret++;
                  }

                  spaced = YAC_FALSE;
               }
               break;

            case '\"':
               // yac_host->printf("xxx \", sqm=%d quot=%d\n", sqm, quot);
               if(_use_quot&& (!(sqm & 1)) ) // ignore within '...'
               {
                  quot++;
                  if(!(quot&1))
                  {
                     // End of "str"

                     // yac_host->printf("xxx end of \"str\", j=%d i=%d\n", j, i);

                     // // *op++ = j;
                     // // *op++ = i - j;
                     // // if(_use_quot)
                     // //    *cq++ = TKS_STRFL_QUOT;
                     quot -= 2;

                     spaced = YAC_TRUE;
                     ret++;
                  }
                  else
                  {
                     // Start of "str", emit prev substring
                     // // ret++;
                     // // bIgnoreLast = YAC_TRUE;
                     if(!spaced)
                     {

                        // // yac_host->printf("xxx start of \"str\", j=%d i=%d\n", j, i);
                        // // *op++ = j;
                        // // *op++ = i - j;
                        // // if(_use_quot)
                        // //    *cq++ = 0;

                        spaced = YAC_TRUE;
                        ret++;
                     }
                     else
                     {
                        // yac_host->printf("xxx start of \"str\" (spaced) i=%d\n", i);
                     }
                  }
                  break;
               }
               else
               {
                  if(spaced)
                  {
                     // j = i;

                     // // ret++;
                  }

                  spaced = YAC_FALSE;
               }
               break;

            default:
               // yac_host->printf("xxx defchar \'%c\' i=%d spaced=%d\n", chars[i], i, spaced);
               if(spaced)
               {
                  // j = i;

                  // // if(!bIgnoreLast)
                  // //    ret++;
                  // // bIgnoreLast = YAC_FALSE;
               }

               spaced = YAC_FALSE;
               break;
         }

         if(i >= _charIdx)
            return ret;

      } // for i < length

   } // if length > 1
   return ret;
}

void YAC_String::tokenizeEx(const YAC_Object *_cs, YAC_Object *_retRanges, YAC_Object *_r) {

   YAC_IntArray *retRanges = NULL;

   if(YAC_Is_IntArray(_retRanges))
   {
      retRanges = (YAC_IntArray*)_retRanges;
      retRanges->num_elements = 0u;
   }

   if(YAC_CHK(_r, YAC_CLID_STRINGARRAY))
   {
      YAC_StringArray *al = (YAC_StringArray*) _r;
      al->empty();

      if(YAC_VALID(_cs))
      {
         if(YAC_Is_String(_cs))
         {
            YAC_String *cs = (YAC_String*)_cs;

            if(cs->length > 1u)
            {
               sSI ret = (length > 1u) ? 1 : 0;

               if(ret)
               {
                  ret = 0;
                  sUI i = 0;
                  sUI j = 0;
                  sBool bQuot = YAC_FALSE;   // \"
                  sBool bSQuot = YAC_FALSE;  // \'
                  sBool bBlank = YAC_TRUE;
                  sUI *offsets;
                  sUI *quotFlags;

#if 1
                  const sUI maxRangeElements = ((length     ) + 1u) << 1;
#else
                  // (todo) this does not cover the (unlikely) worst case where each char is a token
                  //         (but it does save some memory)
                  const sUI maxRangeElements = ((length >> 1) + 1u) << 1;
#endif

                  if(NULL != retRanges)
                  {
                     if(!retRanges->yacArrayRealloc(maxRangeElements, 0,0,0))
                     {
                        char msg[128];
                        Dyac_snprintf(msg, 128, "failed to alloc %u retRanges elements\n", maxRangeElements);
                        Dyac_throw_def(CRITICALERROR, msg);
                        return;
                     }
                     offsets = (sUI*)retRanges->elements;
                  }
                  
                  if(length >= MAX_STATIC_SUBSTRINGS)
                  {
                     if(NULL == retRanges)
                        offsets   = new(std::nothrow) sUI[maxRangeElements];
                     quotFlags = new(std::nothrow) sUI[maxRangeElements];
                  }
                  else
                  {
                     if(NULL == retRanges)
                        offsets   = YAC_String_coffsets;
                     quotFlags = YAC_String_cquot;
                  }

                  sUI *op = offsets;
                  sUI *qf = quotFlags;

                  // Find substring start offsets / lengths
                  for(; i < length; i++)
                  {
                     sU8 c = chars[i];
                     sUI csi = 0u;

                     switch(c)
                     {
                        case ' ':
                        case '\t':
                        case '\n':
                        case '\r':
                        case 0:
                           if(!bQuot && !bSQuot)
                           {
                              if(!bBlank)
                              {
                                 // Implicit whitespace delimiter
                                 // Add new split point (chars until start of whitespace)
                                 ret++;
                                 *op++ = j;
                                 *op++ = i - j;
                                 *qf++ = 0u;
                              }

                              // Move next split start (consume whitespace)
                              j = i;
                              bBlank = YAC_TRUE;
                           }
                           // else: skip over whitespace within quoted string
                           break;

                        case '\'':
                           if(!bQuot) // ignore within "..."
                           {
                              if(!bSQuot)
                              {
                                 if(!bBlank)
                                 {
                                    // Add new split point (chars until start of quote string)
                                    ret++;
                                    *op++ = j;
                                    *op++ = i - j;
                                    *qf++ = 0u;
                                 }

                                 // Start quoted string
                                 j = i + 1;
                              }
                              else if(bSQuot)
                              {
                                 // End of quoted string, add new split point
                                 ret++;
                                 *op++ = j;
                                 *op++ = i - j;
                                 *qf++ = TKS_STRFL_QUOT2;
                                 bBlank = YAC_TRUE;
                              }

                              bSQuot ^= 1;
                              break;
                           }
                           else
                           {
                              // Within quoted '' string, continue search
                           }
                           break;

                        case '\"':
                           if(!bSQuot) // ignore within '...'
                           {
                              if(!bQuot)
                              {
                                 if(!bBlank)
                                 {
                                    // Add new split point (chars until start of quote string)
                                    ret++;
                                    *op++ = j;
                                    *op++ = i - j;
                                    *qf++ = 0u;
                                 }

                                 // Start quoted string
                                 j = i + 1;
                              }
                              else if(bQuot)
                              {
                                 // End of quoted string, add new split point
                                 ret++;
                                 *op++ = j;
                                 *op++ = i - j;
                                 *qf++ = TKS_STRFL_QUOT;
                                 bBlank = YAC_TRUE;
                              }

                              bQuot ^= 1;
                              break;
                           }
                           else
                           {
                              // Within quoted '' string, continue search
                           }
                           break;

                           // run into default
                        default:
                           if(!bQuot && !bSQuot)
                           {
                              for(csi = 0; csi < cs->length; csi++) // also compare asciiz
                              {
                                 if(c == cs->chars[csi])
                                 {
                                    // Found new delimiter char
                                    if(!bBlank)
                                    {
                                       // Add new split point (chars until delimiter)
                                       ret++;
                                       *op++ = j;
                                       *op++ = i - j;
                                       *qf++ = 0u;

                                       // Add new split point (delimiter char)
                                       ret++;
                                       *op++ = i;
                                       *op++ = 1;
                                       *qf++ = 0u;
                                    }
                                    else
                                    {
                                       // Delimiter char after whitespace, add new split point
                                       ret++;
                                       *op++ = i;
                                       *op++ = 1;
                                       *qf++ = 0u;
                                    }
                                    bBlank = YAC_TRUE;
                                    break;
                                 }
                              }

                              if(csi == cs->length)
                              {
                                 // Not a delimiter char
                                 if(bBlank)
                                 {
                                    // Found new split start after whitespace
                                    j = i;
                                    bBlank = YAC_FALSE;
                                 }
                                 // else: continue to look for end of token
                              }
                           }
                           // else: continue to look for end of quoted string
                           break;
                     }

                  } // for each char

                  if(op != offsets)
                  {
                     // Extract substrings to return array
                     if(al->alloc(ret))
                     {
                        al->num_elements = ret;
                        if(NULL != retRanges)
                           retRanges->num_elements = (ret << 1);
                        *op++ = 0;
                        *op++ = 0;
                        *qf++ = 0u;
                        YAC_String *cs2 = al->elements;
                        op = offsets;
                        sUI *op2 = offsets + 1;
                        sUI cop;
                        qf = quotFlags;

                        while(0 != (cop = *op2))
                        {
                           substring(cs2, *op, cop);
                           cs2->bflags |= *qf++;
                           cs2++;
                           op  += 2;
                           op2 += 2;
                        }
                     }
                  }

                  if(length >= MAX_STATIC_SUBSTRINGS)
                  {
                     if(NULL == retRanges)
                        delete [] offsets;
                     delete [] quotFlags;
                  }
               }

               return;
            }
            else
            {
               Dprintf("[---] YAC_String::tokenize: charset needs to hold at least 1 char.\n");
            }
         }
         else
         {
            // Charset object is not a string
            Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "_charset is not a String object");
         }
      }
      else
      {
         // Not a valid charset object
         Dyac_throw_def(INVALIDPOINTER, "_charset is not a valid Object");
      }
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected StringArray return object");
   }
}

void YAC_String::_tokenize_YAC_RARG(const YAC_Object *_cs, YAC_Object *_r) {
   tokenizeEx(_cs, NULL/*retOffsets*/, _r);
}

void YAC_String::_tokenize_YAC_RVAL(const YAC_Object *_cs, YAC_Value *_r) {
	YAC_StringArray *al = (YAC_StringArray*)YAC_NEW_CORE_POOLED(YAC_CLID_STRINGARRAY);
   if(NULL != al)
   {
      _r->initObject(al, 1);
      tokenizeEx(_cs, NULL/*retOffsets*/, al);
   }
   else
   {
      _r->initNull();
   }
}

void YAC_String::_tokenizeWithRanges_YAC_RARG(const YAC_Object *_cs, YAC_Object *_retRanges, YAC_Object *_r) {
   tokenizeEx(_cs, _retRanges, _r);
}

void YAC_String::_tokenizeWithRanges_YAC_RVAL(const YAC_Object *_cs, YAC_Object *_retRanges, YAC_Value *_r) {
	YAC_StringArray *al = (YAC_StringArray*)YAC_NEW_CORE_POOLED(YAC_CLID_STRINGARRAY);
   if(NULL != al)
   {
      _r->initObject(al, 1);

      tokenizeEx(_cs, _retRanges, al);
   }
   else
   {
      _r->initNull();
   }
}

void YAC_String::_splitChar_YAC_RVAL(sSI _delim, YAC_Value *_r) {
	YAC_StringArray *al = (YAC_StringArray*)YAC_NEW_CORE_POOLED(YAC_CLID_STRINGARRAY);
   _r->initObject(al, 1);

   _splitChar_YAC_RARG(_delim, al);
}

void YAC_String::_splitChar_YAC_RARG(sSI _delim, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRINGARRAY))
   {
      YAC_StringArray *al = (YAC_StringArray*) _r;
      al->empty();

      sSI ret = ((length > 1) && _delim) ? 1 : 0;

      if(ret)
      {
         ret = 0;
         sUI i = 0; // current index in string
         sUI j = 0; // length of current substring
         sUI k = 0; // start index
         sUI *offsets;

         if(length <= MAX_STATIC_SUBSTRINGS)
         {
#ifdef TKS_MT
            (void)tkscript->lockGlobalContext();
#endif //TKS_MT
            offsets = YAC_String_coffsets;
         }
         else
         {
            offsets = new(std::nothrow) sUI[(length + 1) << 1];
         }

         sUI *op = offsets;
         sSI quot = 0;
         sSI anyQuot = 0;

         for(; i < length; i++)
         {
            // Dprintf("xxx i=%u c=\'%c\' k=%u j=%u quot=%d\n", i, chars[i], k, j, quot);
            const sChar c = chars[i];
            if('\"' == c)
            {
               if(quot)
               {
                  // End of "" quoted string
                  quot = 0;
               }
               else
               {
                  // Begin quoted string
                  quot = 1;
                  anyQuot = 1;
               }
               j++;
            }
            else if(!quot && c == _delim)
            {
               if(i != (length - 2)) // length -1 is 0 byte
               {
                  // Dprintf("xxx any ret=%u i=%u k=%u j=%u length=%u s=\"%s\"\n", ret, i, k, j, length, chars+k);
                  // // if(0 == j)
                  // // {
                  // //    // *op++ = 0;
                  // //    // *op++ = 0;
                  // //    //k++; // skip delimiter
                  // //    *op++ = k;
                  // //    *op++ = j;
                  // //    k = i + 1;
                  // // }
                  // // else
                  // // {
                     *op++ = k;
                     *op++ = j;
                     k = i + 1;
                     j = 0;
                  // // }

                  ret++;
               }
               else
               {
                  // Dprintf("xxx last ret=%u i=%u k=%u j=%u length=%u s=\"%s\"\n", ret, i, k, j, length, chars+k);
                  // // if(j >= 0)
                  // // {
                     ret++;
                     *op++ = k;
                     *op++ = j; // 0
                     k = i + 1;
                     j = 0;
                  // // }
                  
                  if(chars[length-2] == _delim)
                  {
                     // Last char before ASCIIZ is delimiter
                     ret++;
                     *op++ = chars[length-1];  // points to ASCIIZ
                     *op++ = j; // 0
                     k = i + 1;
                     j = 0;
                  }
                  i = length;
               }
            }
            else
            {
               j++;
            }
         }

         // write tail of string
         if(j > 0)
         {
            if(0 == chars[k + j - 1])
               j--;
            // Dprintf("xxx tail ret=%u i=%u k=%u j=%u length=%u s=\"%s\"\n", ret, i, k, j, length, chars+k);
            ret++;
            *op++ = k;
            *op++ = j;
         }

         if(ret > 0)
         {
            if(al->alloc(ret))
            {
               al->num_elements = ret;
               *op++ = 0;
               *op++ = (sUI)-1;
               YAC_String *cs = al->elements;
               sUI *op2 = offsets + 1;
               op = offsets;
               sUI cop;

               while( (cop = *op2) != ((sUI)-1))
               {
                  if(0 != cop)
                  {
                     if(anyQuot && cop >= 2)
                     {
                        // Exclude quotes
                        // Dprintf("xxx exclude cop=%u startc=\'%c\' endc=\'%c\'\n", cop, chars[*op], chars[*op + cop - 1]);
                        if(chars[*op] == '\"' && chars[*op + cop - 1] == '\"')
                        {
                           substring(cs, *op + 1, cop - 2);
                           cs->bflags |= TKS_STRFL_QUOT;
                        }
                        else
                        {
                           substring(cs, *op, cop);
                        }
                     }
                     else
                     {
                        substring(cs, *op, cop);
                     }
                     cs++;
                  }
                  else
                  {
                     cs++;
                  }

                  op  += 2;
                  op2 += 2;
               }

               if(length > MAX_STATIC_SUBSTRINGS) // XXX thread-safe ??
               {
                  delete [] offsets;
               }
               else
               {
#ifdef TKS_MT
                  tkscript->unlockGlobalContext();
#endif //TKS_MT
               }

               return;
            }
         }

         if(length > MAX_STATIC_SUBSTRINGS)
         {
            delete [] offsets;
         }
         else
         {
#ifdef TKS_MT
            tkscript->unlockGlobalContext();
#endif //TKS_MT
         }
      }
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected StringArray return object");
   }
}

sSI YAC_String::indexOfControlCharacter(sSI _off) const {
   sUI i = (sUI)_off;
   sUI l = length ? (length - 1) : 0;

   for(; i<l; i++)
   {
      if(chars[i] <= ' ')
      {
         return i;
      }
   }

   return -1;
}

sSI YAC_String::charsetIndexOf(const YAC_String *_charset, sSI _off) const {
   if(_off < 0)
      return -1;

   if(NULL != chars && NULL != _charset)
   {
      if(NULL != _charset->chars)
      {
         const sSI csl = (sSI)_charset->length;
         sSI l = (sSI)length;
         const sU8 *oc = _charset->chars;

         sSI i = _off;

         while(i < l)
         {
            sSI j = 0;
            sU8 c = chars[i++];

            if(c > 0)
            {
               while(j < csl)
               {
                  if(c == oc[j++])
                  {
                     return i - 1;
                  }
               }
            }
         }
      }
   }

   return -1;
}

void YAC_String::_abbrev_YAC_RARG(sSI _length, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_String *s = (YAC_String*) _r;

      s->yacCopy(this);
      s->abbrev(_length);
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_abbrev_YAC_RVAL(sSI _length, YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);

   s->yacCopy(this);
   s->abbrev(_length);
}

void YAC_String::_abbrev_YAC_RSELF(sSI _length) {
   abbrev(_length);
}

void YAC_String::abbrev(sSI _length) {
   if( (NULL != chars) && (length > 0) )
   {
      sUI l = length - 1;

      if(l <= 1)
      {
         return;
      }

      sUI nl = (sUI)_length;

      if(nl < 1) // no space for ".."
      {
         return;
      }

      if( (((sSI)l)) <= ((sSI)nl)) // nothing to abbreviate
      {
         return;
      }

      sU8 *d = chars;

      if(1 == nl)
      {
         *d++ = '.';
         *d++ = 0;
         length = 2;
         key = YAC_LOSTKEY;
      }
      else if(nl < 8)
      {
         sSI i = (sSI)(nl - 2);

         if(i > 0)
         {
            d = chars + (nl - 2);
            *d++ = '.';
            *d++ = '.';
            *d++ = 0;
            length = (sUI) (d-chars);
            key = YAC_LOSTKEY;
         }
         else
         {
            d[2] = 0;
            length = 3;
         }
      }
      else if(((sSI)nl) < ((sSI)(l - 3))) // "..text.." or "text.." ??
      {
         sUI offl = (l - nl) / 2;
         sSI i;

         if(offl < 2)
         {
            i = (sSI)offl;
            l -= i + 2;
         }
         else
         {
            i = 2;
            l -= (l - nl) + 2; //(nl&1)
         }

         while(i-- > 0)
         {
            *d++ = '.';
         }

         sU8 *s = chars + offl;
         i = (sSI)(l - 2);

         while(i-- > 0)
         {
            *d++ = *s++;
         }

         *d++ = '.';
         *d++ = '.';
         *d++ = 0;
         length = (sUI)(d - chars);
         key = YAC_LOSTKEY;
      }
      else
      {
         if(l != nl)
         {
            sUI offr = nl - 2;
            d += offr;
            *d++ = '.';
            *d++ = '.';
            *d++ = 0;
            length = (sUI)(d - chars);
            key = YAC_LOSTKEY;
         }
      }
   }
}

void YAC_String::_deleteRegion_YAC_RARG(sSI _off, sSI _length, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_String *s = (YAC_String*) _r;

      s->yacCopy(this);
      s->deleteRegion(_off, _length);
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_deleteRegion_YAC_RVAL(sSI _off, sSI _length, YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);
   s->yacCopy(this);
   s->deleteRegion(_off, _length);
}

void YAC_String::_deleteRegion_YAC_RSELF(sSI _off, sSI _length) {
   deleteRegion(_off, _length);
}

void YAC_String::deleteRegion(sSI _off, sSI _len) {
   if(_len < 1)
   {
      return;
   }

   sUI off = (sUI)_off;
   sUI len = (sUI)_len;

   if(length < 2)
   {
      return;
   }

   if(off >= (length - 1))
   {
      return;
   }

   if((off + len) >= (length - 1))
   {
      len = (length - 1) - off;
   }

   // Move chars
   sU8 *d = chars + off;
   sU8 *s = chars + off + len;
   sUI l = (length - 1 - off - len);

   while(l-- > 0)
   {
      *d++ = *s++;
   }

   chars[(length - len - 1)] = 0;
   length -= len;
   key = YAC_LOSTKEY;
}

sSI YAC_String::_indexOfWordStart(sSI _off) const {
   // (note) used by tkui TextEdit
   if(NULL != chars)
   {
      if(sUI(_off) < length)
      {
         sSI i = _off;
         sBool bSkipSpaces = YAC_TRUE;

         while(--i >= 0)
         {
            switch(chars[i])
            {
               case ' ':
               case '\n':
               case '\t':
                  if(bSkipSpaces)
                  {
                     _off--;
                  }
                  else
                  {
                     if(_off < 0)
                        _off = 0;
                     return _off;
                  }
                  break;

               // // case '.':
               case ',':
               case ':':
               case ';':
               case ')':
               case '(':
               case '=':
               case '+':
               // // case '-':
               case '*':
               case '/':
               case '[':
               case ']':
               case '{':
               case '}':
               case '_':
               case '<':
               case '>':
               case '\"':
               case '\'':
               case '\\':
               case '|':
               case '#':
                  if(bSkipSpaces)
                  {
                     _off--;
                     bSkipSpaces = YAC_FALSE;
                  }
                  else
                  {
                     if(_off < 0)
                        _off = 0;
                     return _off;
                  }
                  break;

               default:
                  _off--;
                  bSkipSpaces = YAC_FALSE;
                  break;
            }
         }

         return 0;
      }
   }

   return -1;
}

sSI YAC_String::_charsetIndexOfWordStart(YAC_String *_charset, sSI _off) const {
   // (note) used by tkui TextEdit
   // (note) e.g. ".,:;)(=+*/[]{}_<>\"\'\\|#"
   if(NULL != chars && NULL != _charset)
   {
      if(sUI(_off) < length)
      {
         if(NULL != _charset->chars)
         {
            const sSI csl = (sSI)_charset->length;
            const sU8 *oc = _charset->chars;

            sSI i = _off;
            sBool bSkipSpaces = YAC_TRUE;

            while(--i >= 0)
            {
               const sU8 c = chars[i];
               switch(c)
               {
                  case ' ':
                  case '\n':
                  case '\t':
                     if(bSkipSpaces)
                     {
                        _off--;
                     }
                     else
                     {
                        if(_off < 0)
                           _off = 0;
                        return _off;
                     }
                     break;

                  default:
                  {
                     sSI j = 0;
                     for(; j < csl; j++)
                     {
                        if(oc[j] == c)
                           break;
                     }
                     if(j != csl)
                     {
                        if(bSkipSpaces)
                        {
                           _off--;
                           bSkipSpaces = YAC_FALSE;
                        }
                        else
                        {
                           if(_off < 0)
                              _off = 0;
                           return _off;
                        }
                     }
                     else
                     {
                        _off--;
                        bSkipSpaces = YAC_FALSE;
                     }
                  }
                  break;
               }
            }

            return 0;
         }
      }
   }

   return -1;
}

sSI YAC_String::_indexOfWordEnd(sSI _off) const {
   // (note) used by tkui TextEdit
   if(NULL != chars)
   {
      if(sUI(_off) < length)
      {
         sSI i = _off;
         const sSI l = (sSI)length;
         sBool bSkipSpaces = YAC_TRUE;
         sBool bFirst = YAC_TRUE;

         while(++i < l)
         {
            _off++;

            switch(chars[i])
            {
               case 0:
                  return sSI(length) - 1;

               case ' ':
               case '\n':
               case '\t':
                  if(!bSkipSpaces || bFirst)
                  {
                     if(sUI(_off) >= length)
                     {
                        _off = sSI(length) - 1;
                     }

                     return _off;
                  }
                  break;

               // // case '.':
               case ',':
               case ':':
               case ';':
               case ')':
               case '(':
               case '=':
               case '+':
               // // case '-':
               case '*':
               case '/':
               case '[':
               case ']':
               case '{':
               case '}':
               case '_':
               case '<':
               case '>':
               case '\"':
               case '\'':
               case '\\':
               case '|':
               case '#':
                  if(bSkipSpaces)
                  {
                     bSkipSpaces = YAC_FALSE;
                  }
                  else
                  {
                     if(sUI(_off) >= length)
                        _off = length - 1;

                     return _off;
                  }
                  break;

               default:
                  bSkipSpaces = YAC_FALSE;
                  break;
            }

            bFirst = YAC_FALSE;
         }

         return sSI(length) - 1;
      }
   }

   return -1;
}

sSI YAC_String::_charsetIndexOfWordEnd(YAC_String *_charset, sSI _off) const {
   // (note) used by tkui TextEdit
   // (note) e.g. ".,:;)(=+*/[]{}_<>\"\'\\|#"
   if(NULL != chars && NULL != _charset)
   {
      if(sUI(_off) < length)
      {
         if(NULL != _charset->chars)
         {
            const sSI csl = (sSI)_charset->length;
            const sU8 *oc = _charset->chars;

            sSI i = _off;
            const sSI l = (sSI)length;
            sBool bSkipSpaces = YAC_TRUE;
            sBool bFirst = YAC_TRUE;

            while(++i < l)
            {
               _off++;

               const char c = chars[i];
               switch(c)
               {
                  case 0:
                     return sSI(length) - 1;

                  case ' ':
                  case '\n':
                  case '\t':
                     if(!bSkipSpaces || bFirst)
                     {
                        if(sUI(_off) >= length)
                        {
                           _off = sSI(length) - 1;
                        }

                        return _off;
                     }
                     break;

                  default:
                  {
                     sSI j = 0;
                     for(; j < csl; j++)
                     {
                        if(oc[j] == c)
                           break;
                     }
                     if(j != csl)
                     {
                        if(bSkipSpaces)
                        {
                           bSkipSpaces = YAC_FALSE;
                        }
                        else
                        {
                           if(sUI(_off) >= length)
                              _off = length - 1;

                           return _off;
                        }
                     }
                     else
                     {
                        bSkipSpaces = YAC_FALSE;
                     }
                  }
                  break;
               }

               bFirst = YAC_FALSE;
            }

            return sSI(length) - 1;
         }
      }
   }
   
   return -1;
}

void YAC_String::_New(YAC_Object *_s, YAC_Value *_r) {
   YAC_String *s = (YAC_String*)YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   s->yacOperatorAssign(_s);
   _r->initString(s, 1);
}

void YAC_String::_replaceRegion_YAC_RARG(sSI _startOff, sSI _endOff, YAC_Object *_repl, YAC_Object *_r) {
   if(YAC_Is_String(_r))
   {
      YAC_String *s = (YAC_String*) _r;

      s->yacCopy(this);
      s->replaceRegion(_startOff, _endOff, _repl);
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_replaceRegion_YAC_RVAL(sSI _startOff, sSI _endOff, YAC_Object *_repl, YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);
   s->yacCopy(this);
   s->replaceRegion(_startOff, _endOff, _repl);
}

void YAC_String::_replaceRegion_YAC_RSELF(sSI _startOff, sSI _endOff, YAC_Object *_repl) {
   replaceRegion(_startOff, _endOff, _repl);
}

void YAC_String::replaceRegion(sSI _startOff, sSI _endOff, YAC_Object *_repl) {
   if(YAC_VALID(_repl))
   {
      if(YAC_Is_String(_repl))
      {
         YAC_String *repl = (YAC_String*) _repl;

         sUI startOff = (sUI) _startOff;
         sUI endOff   = (sUI) _endOff;

         sUI thisLen = length;

         if(thisLen > 0)
         {
            thisLen--; // Do not count ASCIIZ
         }

         if(0 == thisLen)
         {
            yacCopy(repl);

            return;
         }

         if(startOff > thisLen)
         {
            startOff = 0;
         }

         if(endOff > thisLen)
         {
            endOff = thisLen;
         }

         if(endOff < startOff)
         {
            sUI t = endOff;
            endOff = startOff;
            startOff = t;
         }

         sUI replLen = repl->length;

         if(replLen > 0)
         {
            replLen--; // Do not count ASCIIZ
         }

         if(0 == replLen)
         {
            // Delete range
            if(startOff == endOff)
            {
               // Replace empty range with empty string ==> no-op
               return;
            }

            if(endOff == thisLen)
            {
               // Delete until end of string
               chars[startOff] = 0;
               key = YAC_LOSTKEY;
               length = startOff + 1;

               return;
            }

            // Delete range startOff..endOff, shift endOff..thisLen to startOff
            while(endOff < thisLen)
            {
               chars[startOff++] = chars[endOff++];
            }

            chars[startOff++] = 0;
            key = YAC_LOSTKEY;
            length = startOff;

            return;
         }

         if(replLen <= (endOff-startOff))
         {
            // Replacement string length is <= region length to be replaced -> no realloc needed
            sUI i = 0;
            while(i < replLen)
            {
               chars[startOff++] = repl->chars[i++];
            }

            if(endOff == startOff)
            {
               // length has not changed
               key = YAC_LOSTKEY;
               return;
            }
            else
            {
               // Add original trailing region
               while(endOff < thisLen)
               {
                  chars[startOff++] = chars[endOff++];
               }

               chars[startOff++] = 0;
               length = startOff;
               key = YAC_LOSTKEY;

               return;
            }
         }

         // Replacement string is larger than region to be replaced, check if we need to realloc
         sUI newLen = ( 1 + startOff + replLen + thisLen - endOff );

         if(buflen >= newLen)
         {
            // No realloc needed, string still fits into buffer, shift original trailing region
            sUI i = thisLen + replLen - (endOff-startOff), j = thisLen;
            chars[i] = 0;

            while(--j >= endOff)
            {
               chars[--i] = chars[j];
            }

            // Now replace region with replStr
            i = 0;

            while(i < replLen)
            {
               chars[startOff++] = repl->chars[i++];
            }

            length = newLen;
            key = YAC_LOSTKEY;

            return;
         }
         else
         {
            // Worst case: chars need to be re-allocated
            sU8 *nc = Dyacallocchars(newLen);
            sUI i = 0;

            // First copy region until startOff
            while(i < startOff)
            {
               nc[i] = chars[i];
               i++;
            }

            // Append replacement string
            sUI j = 0;

            while(j < replLen)
            {
               nc[i++] = repl->chars[j++];
            }

            // Append original trailing region
            while(endOff < thisLen)
            {
               nc[i++] = chars[endOff++];
            }

            nc[i++] = 0;

            sUI origFlags = (bflags & TKS_STRFLQMASK);
            safeFreeChars();
            chars  = nc;
            buflen = newLen;
            bflags = origFlags | TKS_STRFL_DEL;
            length = newLen;
            key    = YAC_LOSTKEY;

            return;
         }
      } // is string
   } // is valid object
}


sSI YAC_String::_replaceIncludes(YAC_String *_filePrefix) {
   // Preprocess include directives (`some/file`)
   sSI loff = 0;
   sSI istart = -1;
   sSI iend;
   sSI li;
   sSI numRepl = 0;

   for(;;)
   {
      li = indexOf('`', loff);

      if(-1 != li)
      {
         if(-1 == istart)
         {
            iend = -1;
            istart = li + 1;
            loff = istart;
         }
         else
         {
            iend = li;

            // Parse filename
            YAC_String t;
            substring(&t, istart, iend-istart);

            if(YAC_VALID(_filePrefix))
            {
               t.replaceRegion(0, 0, _filePrefix);
            }

            YAC_String buf;

            if(! buf._loadLocal(&t, 1))
            {
               Dprintf("[---] unable to open include file \"%s\".", (char*) t.chars);
               return -1;
            }

            // Replace "`some/file`" by file contents
            replaceRegion(istart-1, iend+1, &buf);

            loff = iend + 1;

            numRepl++;
            istart = -1;
         }
      }
      else
      {
         break;
      }
   }

   return numRepl;
}

void YAC_String::setRawBuffer(sU8 *_data, sUI _len) {
   // visit buffer
   free();
   buflen = _len;
   bflags = 0;
   chars  = _data;
   length = _len;
   key    = YAC_LOSTKEY;
}

sSI YAC_String::_getChecksum(void) {
   return (sSI) getKey();
}

sSI YAC_String::_getChecksumIgnoreWhitespace(void) {
   sUI r = 0u;
   if(NULL != chars)
   {
      sUI i = 1u;
      const sU8 *c = chars;
      while(*c)
      {
         if(*c > 32u)
         {
            r = 53u * r + i * *c;
            i += 128u;
         }
         c++;
      }
   }
   return r;
}

void YAC_String::_getDirName_YAC_RVAL(YAC_Value *_r) const {
   YAC_String *rs = YAC_New_String();
   getDirName(rs);
   _r->initString(rs, 1);
}

void YAC_String::_getDirName_YAC_RARG(YAC_Object *_r) const {
   if(YAC_BCHK(_r, YAC_CLID_STRING))
   {
      getDirName( (YAC_String*) _r );
   }
}

void YAC_String::getDirName(YAC_String *_r) const {
   sSI idxSlash     = lastIndexOf('/', 0);
   sSI idxBackSlash = lastIndexOf('\\', 0);
   sSI idxDrive     = lastIndexOf(':', 0);
   sSI idx = -1;

   if(idxSlash > idxBackSlash)
   {
      idx = idxSlash;
   }
   else
   {
      idx = idxBackSlash;
   }

   if(idxDrive > idx)
   {
      idx = idxDrive;
   }

   if(-1 != idx)
   {
      _r->_realloc(idx + 2);
      _r->length = idx + 2;

      sSI i;
      for(i=0; i<=idx; i++)
      {
         _r->chars[i] = chars[i];
      }

      _r->chars[i++] = 0;
      _r->key = YAC_LOSTKEY;
   }
   else
   {
      _r->empty();
   }
}

void YAC_String::_escapeControlChars_YAC_RSELF (void) {
   YAC_String *rs = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _escapeControlChars_YAC_RARG(rs);
   refCopy(rs);
   YAC_DELETE(rs);
}

void YAC_String::_escapeControlChars_YAC_RVAL (YAC_Value *_r) const {
   _r->initEmptyString();
   _escapeControlChars_YAC_RARG(_r->value.string_val);
}

void YAC_String::_escapeControlChars_YAC_RARG (YAC_Object *_r) const {
   if(YAC_Is_String(_r))
   {
      YAC_CAST_ARG(YAC_String, r, _r);

      // (todo) optimize me

      r->yacCopy((YAC_String*)this);

      r->replace("\\", "\\\\");
      r->replace("\"", "\\\"");
      r->replace("\'", "\\\'");
      r->replace("\t", "\\\t");
      // r->replace("\n", "\\\n");  // [18Aug2024]
      r->replace("\r", "\\\r");
      //replace("\d", "\\\d");
      r->replace("\b", "\\\b");
      r->replace("\a", "\\\a");
      r->replace("\f", "\\\f");
      r->replace("\v", "\\\v");
      //replace("\e", "\\\e");

	} // if is string
}

void YAC_String::_getDebugString(YAC_Value *_r) {

   _r->initEmptyString();

   char buf[256];

   sUI curKey = key;
   genKey();

   Dyac_snprintf(buf, 256, "<addr=" YAC_PRINTF_PTRPREFIX "%p buflen=0x%08x length=0x%08x key=0x%08x (cur=0x%08x)>",
                 this,
                 buflen,
                 length,
                 getKey(),
                 curKey
                 );

   key = curKey;

   _r->value.string_val->copy(buf);
}

sUI YAC_String::_getMetaFlags(void) const {
   return getQuotFlag();
}

sBool YAC_String::_wasQuoted(void) const {
   return (0 != getQuotFlag());
}

sBool YAC_String::_wasQuotedSingle(void) const {
   return (2 == getQuotFlag());
}

sBool YAC_String::_wasQuotedDouble(void) const {
   return (1 == getQuotFlag());
}

sUI YAC_String::_numCharsAt(sSI _char, sUI _off) const {
   // Count number of consecutive char occurences
   sUI ret = 0;
   sUI i = _off;
   for(; i < length; i++)
   {
      if(chars[i] == sU8(_char))
      {
         ret++;
      }
      else
         break;
   }
   return ret;
}

void YAC_String::_utf8ToASCII_YAC_RARG(YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_String *r = (YAC_String*) _r;
      r->yacCopy(this);
      r->_utf8ToASCII_YAC_RSELF();
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_utf8ToASCII_YAC_RVAL(YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);
   s->yacCopy(this);
   s->_utf8ToASCII_YAC_RSELF();
}

void YAC_String::_utf8ToASCII_YAC_RSELF(void) {
   // Replace non-ASCII characters by SUBstitute (26) char
   if(length > 0u)
   {
      sUI k = 0u;
      sUI skip = 0u;
      sUI len = length - 1u;  // ASCIIz
      sU8 *d = chars;

      sUI i;

      if(_hasUTF8BOM())
      {
         i = 3u;
      }
      else
      {
         i = 0u;
      }

      for(; i < len; i++)
      {
         sU8 c = chars[i];
         if(skip > 0u)
            skip--;
         else
         {
            if(0xF0u == (c & 0xF0u))       // 4 byte sequence 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            {
               skip = 3u;
               d[k++] = 26u;  // SUB
            }
            else if(0xE0u == (c & 0xE0u))  // 3 byte sequence 1110xxxx 10xxxxxx 10xxxxxx
            {
               skip = 2u;
               d[k++] = 26u;  // SUB
            }
            else if(0xC0u == (c & 0xC0u))  // 2 byte sequence 110xxxxx 10xxxxxx
            {
               skip = 1u;
               d[k++] = 26u;  // SUB
            }
            else if(0x80u == (c & 0x80u))  // error: out of sequence extended character (ignore)
               skip = 0u;
            else
               d[k++] = c;    // 7bit ASCII
         }
      }
      d[k++] = 0u;
      length = k;
      key = YAC_LOSTKEY;
   }
}

void YAC_String::_utf8ToCP1252_YAC_RARG(YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_String *r = (YAC_String*) _r;
      r->yacCopy(this);
      r->_utf8ToCP1252_YAC_RSELF();
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_utf8ToCP1252_YAC_RVAL(YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);
   s->yacCopy(this);
   s->_utf8ToCP1252_YAC_RSELF();
}

void YAC_String::_utf8ToCP1252_YAC_RSELF(void) {

   // (note) <https://cs.stanford.edu/people/miles/iso8859.html>
   // (note) <https://en.wikipedia.org/wiki/Latin-1_Supplement_(Unicode_block)>
   // (note) <https://de.wikipedia.org/wiki/Windows-1252>

   // (note) CP1252 is the same as ISO8859-1 except for the 0x80..0x9F area (ctl codes in ISO)
   // (note) CP1252 is the HTML5 default encoding for text labeled as ISO8859-1

   // (note) "CHCP 1252" in DOS shell switches to 1252 code page

   // Replace non-convertable characters by SUBstitute (26) char
   if(length > 0u)
   {
      sUI k = 0u;
      sUI skip = 0u;
      sUI skipBit = 0u;
      sUI len = length - 1u;  // ASCIIz
      sU8 *d = chars;
      sUI cUni = 0u;

      sUI i;

      if(_hasUTF8BOM())
      {
         i = 3u;
      }
      else
      {
         i = 0u;
      }

      for(; i < len; i++)
      {
         sU8 c = chars[i];
         if(skip > 0u)
         {
            cUni |= (c & 0x3Fu) << skipBit;
            skipBit -= 6u;
            if(0u == --skip)
            {
               sU8 cOut;
               switch(cUni)
               {
                  default:
                     if(cUni < 256u)
                        cOut = sU8(cUni);
                     else
                        cOut = 26/*SUB*/;
                     break;

                  case 0x20ACu:  // euro
                     cOut = 0x80;
                     break;

                  case 0x0201Au:  // ,
                     cOut = 0x82;
                     break;

                  case 0x0192u:  // f
                     cOut = 0x83;
                     break;

                  case 0x201Eu:  // "
                     cOut = 0x84;
                     break;

                  case 0x2026u:  // ...
                     cOut = 0x85;
                     break;

                  case 0x2020u:  // +
                     cOut = 0x86;
                     break;

                  case 0x2021u:  // ++
                     cOut = 0x87;
                     break;

                  case 0x02C6u:  // ^
                     cOut = 0x88;
                     break;

                  case 0x2030u:  // %o
                     cOut = 0x89;
                     break;

                  case 0x0160u:  // S
                     cOut = 0x8A;
                     break;

                  case 0x2039u:  // <
                     cOut = 0x8B;
                     break;

                  case 0x0152u:  // OE
                     cOut = 0x8C;
                     break;

                  case 0x17Du:  // Z
                     cOut = 0x8E;
                     break;

                  case 0x2018u:  // '
                     cOut = 0x91;
                     break;

                  case 0x2019u:  // '
                     cOut = 0x92;
                     break;

                  case 0x201Cu:  // "
                     cOut = 0x93;
                     break;

                  case 0x201Du:  // "
                     cOut = 0x94;
                     break;

                  case 0x2022u:  // * (bullet point)
                     cOut = 0x95;
                     break;

                  case 0x2013u:  // -
                     cOut = 0x96;
                     break;

                  case 0x2014u:  // --
                     cOut = 0x97;
                     break;

                  case 0x02DCu:  // ~ (superscript)
                     cOut = 0x98;
                     break;

                  case 0x2122u:  // tm (superscript)
                     cOut = 0x99;
                     break;

                  case 0x0161u:  // s
                     cOut = 0x9A;
                     break;

                  case 0x0203u:  // >
                     cOut = 0x9B;
                     break;

                  case 0x0153u:  // oe
                     cOut = 0x9C;
                     break;

                  case 0x017Eu:  // z
                     cOut = 0x9E;
                     break;

                  case 0x0178u:  // Y
                     cOut = 0x9F;
                     break;
               }

               d[k++] = cOut;
            }
         }
         else
         {
            if(0xF0u == (c & 0xF8u))       // 4 byte sequence 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            {
               skip = 3u;
               cUni = (c & ~0xF0u) << 18;
               skipBit = 12u;
            }
            else if(0xE0u == (c & 0xF0u))  // 3 byte sequence 1110xxxx 10xxxxxx 10xxxxxx
            {
               skip = 2u;
               cUni = (c & ~0xE0u) << 12;
               skipBit = 6u;
            }
            else if(0xC0u == (c & 0xE0u))  // 2 byte sequence 110xxxxx 10xxxxxx
            {
               skip = 1u;
               cUni = (c & ~0xC0u) << 6;
               skipBit = 0u;
            }
            else if(0x80u == (c & 0xC0u))  // error: out of sequence extended character (ignore)
               skip = 0u;
            else if(0x80 == (c & 0x80u))   // error: invalid UTF-8 char (ignore)
               skip = 0u;
            else
               d[k++] = c;    // 7bit ASCII
         }
      }
      d[k++] = 0u;
      length = k;
      key = YAC_LOSTKEY;
   }
}

sUI YAC_String::writeUTF8(sUI _unicode, sUI _byteOffset) {
   sUI r = _byteOffset;
   if(_unicode < 0x80u)
   {
      chars[r++] = sU8(_unicode);  // ASCII
   }
   else if(_unicode < 0x800u)
   {
      // 2 byte sequence 110xxxxx 10xxxxxx
      chars[r++] = sU8(0xC0u) | (_unicode >> 6);
      chars[r++] = sU8(0x80u) | (_unicode & 0x3Fu);
   }
   else if(_unicode < 0x10000u)
   {
      // 3 byte sequence 1110xxxx 10xxxxxx 10xxxxxx
      chars[r++] = sU8(0xE0u) | (_unicode >> 12);
      chars[r++] = sU8(0x80u) | ((_unicode >>  6) & 0x3Fu);
      chars[r++] = sU8(0x80u) | ((_unicode      ) & 0x3Fu);
   }
   else
   {
      // 4 byte sequence 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
      chars[r++] = sU8(0xF0u) | (_unicode >> 18);
      chars[r++] = sU8(0x80u) | ((_unicode >> 12) & 0x3Fu);
      chars[r++] = sU8(0x80u) | ((_unicode >>  6) & 0x3Fu);
      chars[r++] = sU8(0x80u) | ((_unicode      ) & 0x3Fu);
   }
   return r;
}

void YAC_String::_cp1252ToUTF8_YAC_RARG(sBool _bBOM, YAC_Object *_r) {
   if(YAC_CHK(_r, YAC_CLID_STRING))
   {
      YAC_String *r = (YAC_String*) _r;

      if(length > 1u)
      {
         r->realloc((length + 1u) * 3u, YAC_FALSE/*bCopy*/);  // worst case allocation size

         sUI k = 0u;
         sUI len = length - 1u;  // ASCIIz
         const sU8 *s = chars;
         sU8 *d = r->chars;

         if(_bBOM)
         {
            k = r->writeUTF8(0xFEFFu, 0u);
         }

         for(sUI i = 0u; i < len; i++)
         {
            sU8 c = s[i];

            switch(c)
            {
               default:
                  if(c < 0x80u)
                  {
                     // ASCII
                     d[k++] = c;
                  }
                  else
                  {
                     k = r->writeUTF8(c, k);
                  }
                  break;

               case 0x80:  // euro
                  k = r->writeUTF8(0x20ACu, k);
                  break;

               case 0x82:  // ,
                  k = r->writeUTF8(0x0201Au, k);
                  break;

               case 0x83:  // f
                  k = r->writeUTF8(0x0192u, k);
                  break;

               case 0x84:  // "
                  k = r->writeUTF8(0x201Eu, k);
                  break;

               case 0x85:  // ...
                  k = r->writeUTF8(0x2026u, k);
                  break;

               case 0x86:  // +
                  k = r->writeUTF8(0x2020u, k);
                  break;

               case 0x87:  // ++
                  k = r->writeUTF8(0x2021u, k);
                  break;

               case 0x88:  // ^
                  k = r->writeUTF8(0x02C6u, k);
                  break;

               case 0x89:  // %o
                  k = r->writeUTF8(0x2030u, k);
                  break;

               case 0x8A:  // S
                  k = r->writeUTF8(0x0160u, k);
                  break;

               case 0x8B:  // <
                  k = r->writeUTF8(0x2039u, k);
                  break;

               case 0x8C:  // OE
                  k = r->writeUTF8(0x0152u, k);
                  break;

               case 0x8E:  // Z
                  k = r->writeUTF8(0x017Du, k);
                  break;

               case 0x91:  // '
                  k = r->writeUTF8(0x2018u, k);
                  break;

               case 0x92:  // '
                  k = r->writeUTF8(0x2019u, k);
                  break;

               case 0x93:  // "
                  k = r->writeUTF8(0x201Cu, k);
                  break;

               case 0x94:  // "
                  k = r->writeUTF8(0x201Du, k);
                  break;

               case 0x95:  // * (bullet point)
                  k = r->writeUTF8(0x2022u, k);
                  break;

               case 0x96:  // -
                  k = r->writeUTF8(0x2013u, k);
                  break;

               case 0x97:  // --
                  k = r->writeUTF8(0x2014u, k);
                  break;

               case 0x98:  // ~ (superscript)
                  k = r->writeUTF8(0x02DCu, k);
                  break;

               case 0x99:  // tm (superscript)
                  k = r->writeUTF8(0x2122u, k);
                  break;

               case 0x9A:  // s
                  k = r->writeUTF8(0x0161u, k);
                  break;

               case 0x9B:  // >
                  k = r->writeUTF8(0x0203u, k);
                  break;

               case 0x9C:  // oe
                  k = r->writeUTF8(0x0153u, k);
                  break;

               case 0x9E:  // z
                  k = r->writeUTF8(0x017Eu, k);
                  break;

               case 0x9F:  // Y
                  k = r->writeUTF8(0x0178u, k);
                  break;
            }
         } // loop chars
         d[k++] = 0u;
         r->length = k;
         r->key = YAC_LOSTKEY;
      }
      else
      {
         r->free();
      }
   }
   else
   {
      Dyac_throw_def(NATIVECLASSTYPEMISMATCH, "expected String return object");
   }
}

void YAC_String::_cp1252ToUTF8_YAC_RVAL(sBool _bBOM, YAC_Value *_r) {
   YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
   _r->initString(s, 1);
   _cp1252ToUTF8_YAC_RARG(_bBOM, s);
}


void YAC_String::_cp1252ToUTF8_YAC_RSELF(sBool _bBOM) {
   if(length > 1u)
   {
      YAC_String t;
      _cp1252ToUTF8_YAC_RARG(_bBOM, &t);
      refCopy(&t);
   }
}

sBool YAC_String::_hasUTF8BOM (void) const {
   sBool ret = YAC_FALSE;

   if(length > 1u)
   {
      sUI len = length - 1u;
      const sU8 *s = chars;

      // BOM ?
      if(0xEFu == s[0])
      {
         if(0xBBu == s[1])
         {
            if(2 < len)
            {
               if(0xBFu == s[2])
                  ret = YAC_TRUE;
            }
         }
      }
   }

   return ret;
}

sBool YAC_String::_isUTF8(void) const {

   if(_hasUTF8BOM())
      return YAC_TRUE;

   sBool ret = YAC_FALSE;

   // guess
   //  - check if all multi-byte sequences are valid
   if(length > 1u)
   {
      sUI len = length - 1u;
      const sU8 *s = chars;
      char c = s[0];

      for(sUI i = 1u; i < len; i++)
      {
         sU8 n = s[i];

         if(0xF0u == (c & 0xF8u))       // 4 byte sequence 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
         {
            ret = YAC_FALSE;
            if(0x80u == (n & 0xC0u))
            {
               if((i+1) < len)
               {
                  if(0x80u == (s[i+1] & 0xC0u))
                  {
                     if((i+2) < len)
                     {
                        if(0x80u == (s[i+2] & 0xC0u))
                        {
                           ret = YAC_TRUE;
                           i += 3;
                           n = chars[i];
                        }
                     }
                  }
               }
            }
            if(!ret)
               break;
         }
         else if(0xE0u == (c & 0xF0u))  // 3 byte sequence 1110xxxx 10xxxxxx 10xxxxxx
         {
            ret = YAC_FALSE;
            if(0x80u == (n & 0xC0u))
            {
               if((i+1) < len)
               {
                  if(0x80u == (s[i+1] & 0xC0u))
                  {
                     ret = YAC_TRUE;
                     i += 2;
                     n = s[i];
                  }
               }
            }
            if(!ret)
               break;
         }
         else if(0xC0u == (c & 0xE0u))  // 2 byte sequence 110xxxxx 10xxxxxx
         {
            ret = (0x80u == (n & 0xC0u));
            if(ret)
            {
               i++;
               n = s[i];
            }
            else
               break;
         }
         else if(0x80u == (c & 0xC0u))  // error: out of sequence extended character
         {
            ret = YAC_FALSE;
            break;
         }
         else if(0x80u == (c & 0x80u))  // invalid UTF-8 character
         {
            ret = YAC_FALSE;
            break;
         }

         // Next char
         c = n;
      }
   }

   return ret;
}
