
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YAC_PRINTF defined
#include <yac.h>

//#include <tks-list.h>
//extern YAC_Host *yac_host;

#include <ft2build.h>
#include FT_FREETYPE_H
//#include <freetype/freetype.h>

#include "NativeFT2Face.h"

NativeFT2Face::NativeFT2Face(void) {
   ft_face = NULL;

   for(sSI i=0; i<256; i++)
   {
      glyphs_advancex[i] = 0.0f;
   }

   b_monochrome = YAC_TRUE;
}

NativeFT2Face::~NativeFT2Face() {
   freeFace();
}

void NativeFT2Face::freeFace(void) {
   if(ft_face)
   {
      FT_Done_Face(ft_face);
      ft_face = NULL;
   }
}

sSI NativeFT2Face::openMemoryFace(YAC_Object *_buffer, sF32 _w, sF32 _h) {
   if(_buffer)
   {
      freeFace();

      FT_Byte *bytes = (FT_Byte*)_buffer->yacArrayGetPointer();
      sUI numbytes = _buffer->yacArrayGetMaxElements();
      //yac_host->printf("bytes=%08x numbytes=%i\n", (sUI)bytes, numbytes);

      FT_Error code = FT_New_Memory_Face(ft_library, bytes, numbytes, 0 /* face index */, &ft_face);

      if(code)
      {
         yac_host->printf("[---] NativeFT2Face::openMemoryFace(): FT_New_Memory_Face() failed with error code %i\n", code);
      }
      else
      {
         return openFace2(_w, _h);
      }
   }
   return 0;
}

sSI NativeFT2Face::openFileFace(YAC_Object *_pathname, sF32 _w, sF32 _h) {
   if(_pathname)
   {
      if(YAC_BCHK(_pathname, YAC_CLID_STRING))
      {
         YAC_String *pathname=(YAC_String*)_pathname;
         freeFace();
         FT_Error code=FT_New_Face(ft_library, (const char*)pathname->chars, 0, &ft_face);
         if(code)
         {
            yac_host->printf("[---] NativeFT2Face::openFileFace(): FT_New_Face() failed with error code %i\n", code);
         }
         else
         {
            return openFace2(_w, _h);
         }
      }
   }
   return 0;
}

sSI NativeFT2Face::openFace2(sF32 _w, sF32 _h) {
   if(ft_face)
   {
      // ---- set the desired font size
      FT_Error code=FT_Set_Char_Size(ft_face, (int)(_w*64.0f), (int)(_h*64.0f), 72, 72);
      if(code)
      {
         yac_host->printf("[---] NativeFT2Face::openFace2(): FT_Set_Char_Size() failed with error code %i\n", code);
      }
      else
      {
         /*
         if(ft_face->face_flags&FT_FACE_FLAG_SCALABLE)
         {
            yac_host->printf("[...] NativeFT2Face::openFace2(): font face is scalable.\n");
         }
         */
         if(ft_face->face_flags&FT_FACE_FLAG_KERNING)
         {
            if(yac_host->yacGetDebugLevel() > 1)
               yac_host->printf("[...] NativeFT2Face::openFace2(): font uses kerning (not rendered, yet).\n");
         }
         return 1;
      }
   }
   return 0;
}

sF32 NativeFT2Face::getHeight(void) {
   if(ft_face)
   {
      return ft_face->size->metrics.height/64.0f;
   }
   else
      return 0.0f;
}

sF32 NativeFT2Face::getAscender(void) {
   if(ft_face)
   {
      return ft_face->size->metrics.ascender/64.0f;
   }
   else
      return 0.0f;
}

sF32 NativeFT2Face::getDescender(void) {
   if(ft_face)
   {
      return ft_face->size->metrics.descender/64.0f;
   }
   else
      return 0.0f;
}

sF32 NativeFT2Face::getUnderlinePosition(void) {
   if(ft_face)
   {
//       yac_host->printf("size->metrics.y_scale=%i", ft_face->size->metrics.y_scale);
//       yac_host->printf("ft_face->underline_position==%i", ft_face->underline_position);
      return (ft_face->size->metrics.y_scale/65536.0f)*(ft_face->underline_position/64.0f);
   }
   else
      return 0.0f;
}

sF32 NativeFT2Face::getUnderlineThickness(void) {
   if(ft_face)
   {
      return (ft_face->size->metrics.y_scale/65536.0f)*(ft_face->underline_thickness/64.0f);
   }
   else
      return 0.0f;
}

void NativeFT2Face::loadChar(YAC_Object *_so_ft2face, sSI _char) {

   if(ft_face&&_so_ft2face)
   {
      sUI ak_width    = _so_ft2face->yacMetaClassMemberGetAccessKeyByName("width"); // bitmap_width
      sUI ak_height   = _so_ft2face->yacMetaClassMemberGetAccessKeyByName("height"); // bitmap_height
      sUI ak_offsetx  = _so_ft2face->yacMetaClassMemberGetAccessKeyByName("offsetx");
      sUI ak_offsety  = _so_ft2face->yacMetaClassMemberGetAccessKeyByName("offsety");
      sUI ak_advancex = _so_ft2face->yacMetaClassMemberGetAccessKeyByName("advancex");

      YAC_Value v;

      FT_Error code=FT_Load_Char(ft_face, _char, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | (b_monochrome?FT_LOAD_MONOCHROME:0) /*| FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH*//*| FT_LOAD_FORCE_AUTOHINT*/ /**/ /*load_flags*/);
      if(code)
      {
         yac_host->printf("[---] NativeFT2Face::loadChar(): FT_Load_Char(_char=%i) failed with error code %i\n", _char, code);
         v.initFloat(0.0f);
         _so_ft2face->yacMetaClassMemberSet(ak_width, &v);

         sF32 advx = (sF32)(ft_face->glyph->metrics.horiAdvance/64.0f);
         v.initFloat(advx);
         _so_ft2face->yacMetaClassMemberSet(ak_advancex, &v);
         glyphs_advancex[_char&0xFF]=v.value.float_val;
      }
      else
      {
         // ---- glyph is loaded and converted to a bitmap
         v.initFloat((sF32)ft_face->glyph->bitmap.width);
         _so_ft2face->yacMetaClassMemberSet(ak_width, &v);

         v.initFloat((sF32)ft_face->glyph->bitmap.rows);
         _so_ft2face->yacMetaClassMemberSet(ak_height, &v);

         v.initFloat((sF32)ft_face->glyph->bitmap_left);
         _so_ft2face->yacMetaClassMemberSet(ak_offsetx, &v);

         v.initFloat((sF32)ft_face->glyph->bitmap_top);
         _so_ft2face->yacMetaClassMemberSet(ak_offsety, &v);

         sF32 advx;
         //if(_char==' ')
         //{
            advx=(sF32)(ft_face->glyph->metrics.horiAdvance/64.0f);
         //}
         //else
         //{
           // advx=(sF32)(ft_face->glyph->advance.x/64.0f);
         //}
         v.initFloat(advx);
         _so_ft2face->yacMetaClassMemberSet(ak_advancex, &v);
         glyphs_advancex[_char&0xFF]=v.value.float_val;
      }
   }
}

void NativeFT2Face::renderGlyph(YAC_Object *_texture) {
   if(_texture&&ft_face)
   if(ft_face->glyph)
   {
      // ---- copy bitmap data
      sUI bmw    = ft_face->glyph->bitmap.width;
      sUI bmh    = ft_face->glyph->bitmap.rows;
      sUI stride = _texture->yacArrayGetStride();
      union {
         void *any;
         sU8  *p8;
         sU32 *p32;
      } texp;
      texp.any  = _texture->yacArrayGetPointer();
      sUI texw   = _texture->yacArrayGetWidth();
      sUI texh   = _texture->yacArrayGetHeight();

      if((bmw>texw)||(bmh>texh))
      {
         yac_host->printf("[---] NativeFT2Face::renderGlyph(): glyph size (%i, %i) does not match texture size (%i, %i).\n",
            bmw, bmh, texw, texh);
         return;
      }

      sBool b32 = (4 == _texture->yacArrayGetElementByteSize());

      sU8 *srcp=ft_face->glyph->bitmap.buffer;
      //yac_host->printf("renderGLyph() stride=%i bmh=%i bmw=%i texw=%i texh=%i texp=%08x srcp=%08x\n", stride, bmh, bmw, texw, texh, (sUI)texp, (sUI)srcp);
      sUI pitch = (sUI)ft_face->glyph->bitmap.pitch;

      if(texp.any && srcp)
      {
         sUI x,y;

         for(y=0; y<bmh; y++)
         {
            for(x=0; x<bmw; x++)
            {
               if(b_monochrome)
               {
                  // 1bit monochrome
                  sU8 t = 7-(x&7);
                  sU8 bits = srcp[x>>3];
                  sU8 a = ((bits>>t)&1) *255;
                  if(b32)
                  {
                     texp.p32[x] = (a << 24) | 0x00FFFFFF;
                  }
                  else
                  {
                     texp.p8[x] = a;
                  }
               }
               else
               {
                  // 8bit anti-aliased
                  if(b32)
                  {
                     texp.p32[x] = (srcp[x] << 24) | 0x00FFFFFF;
                  }
                  else
                  {
                     texp.p8[x] = srcp[x];
                  }
               }
            }

            texp.p8 += stride;
            srcp += pitch;
         }
      }
      // ---- glyph loading finished
   }
}

/*sF32 NativeFT2Face::stringWidth(YAC_Object *_string) {
   if(YAC_CHK(_string, YAC_CLID_STRING))
   {
      YAC_String *s=(YAC_String*)_string;
      sUI i;
      sF32 r=0.0f;
      for(i=0; i<s->length; i++)
      {
         sU8 c=s->chars[i];
         r+=glyphs_advancex[c];
      }
      return r;
   }
   return 0.0f;
}*/

void NativeFT2Face::storeGlyphsAdvanceX(YAC_Object *_floatArray) {
   if(YAC_BCHK(_floatArray, YAC_CLID_FLOATARRAY))
   {
      YAC_FloatArray *fa=(YAC_FloatArray*)_floatArray;
      if(fa->max_elements>=256)
      {
         sF32 *d=(sF32*)fa->yacArrayGetPointer();
         if(d)
         {
            for(sUI i=0; i<256; i++)
            {
               *d++=glyphs_advancex[i];
            }
         }
      }
   }
}


void NativeFT2Face::storeFamilyName(YAC_Object *_string) {
   if(YAC_BCHK(_string, YAC_CLID_STRING))
   {
      YAC_String *s=(YAC_String *)_string;
      YAC_String t;
      if(ft_face)
      {
         t.visit((char*)ft_face->family_name);
      }
      s->yacCopy(&t);
   }
}

void NativeFT2Face::storeStyleName(YAC_Object *_string) {
   if(YAC_BCHK(_string, YAC_CLID_STRING))
   {
      YAC_String *s=(YAC_String *)_string;
      YAC_String t;
      if(ft_face)
      {
         t.visit((char*)ft_face->style_name);
      }
      s->yacCopy(&t);
   }
}

sSI NativeFT2Face::getStyleFlags(void) {
   if(ft_face)
   {
      return (sSI)ft_face->style_flags;
   }
   else
      return 0;
}

void NativeFT2Face::setMonochrome(sBool _bEnabled) {
   b_monochrome = _bEnabled;
}

sBool NativeFT2Face::getMonochrome(void) {
   return b_monochrome;
}
