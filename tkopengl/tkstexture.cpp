// tkstexture.cpp
///
/// (c) 1999-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

// #define DEBUG_UPLOAD defined

// old-style glTexParameteri GL_MODULATE etc
#ifndef DX_GL_COMPATIBILITY
#define DX_GL_COMPATIBILITY  1
#endif

#define YAC_PRINTF defined
#include <yac.h>

#include <stdlib.h>
#include <math.h>

#include "inc_opengl.h"

#include "tks-list.h"

#include "bitmap.h"
#include "image.h"
#include "tksopengl.h"
#include "glerror.h"
#include "tkstexture.h"

#include "tri_raster_aa.h"
#if 0
#include "tri_raster_aa_fx.h"
#else
#define AAFX_USE_DIVSW   0
#define AAFX_DELTA       0
#define AAFX_ALLOW_AA    1
#define AAFX_NUM_SAMPLES 4
#define AAFX_NOAA_BIAS   1
#include "tri_raster_aa_fx_opt.h"
#endif

#define HSPAN_DIV_TBL 1
#define HSPAN_TOPLEFT_FILLRULE 1
#define HSPAN_SAFE 1
#define Dprintfmissing if(0);else printf
#include "tri_hspan.h"

#include "tri_tex_fx.h"

extern sUI clid_Texture;
extern YAC_Host *yac_host;


extern glGenerateMipmap_t glGenerateMipmap_xref;


#if 0
// test data:
static const GLubyte tex_data[4* 4*4] = {
   0xFF, 0x88, 0xFF, 0xFF,
   0x00, 0x00, 0xFF, 0xFF,
   0xFF, 0x88, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF,

   0x00, 0xFF, 0xFF, 0xFF,
   0x00, 0x88, 0xFF, 0xFF,
   0x00, 0xFF, 0xFF, 0xFF,
   0x00, 0x88, 0xFF, 0xFF,

   0xFF, 0x88, 0x00, 0xFF,
   0xFF, 0xFF, 0x00, 0xFF,
   0xFF, 0x88, 0x00, 0xFF,
   0xFF, 0xFF, 0x00, 0xFF,

   0x7F, 0xFF, 0x7F, 0xFF,
   0x7F, 0x88, 0x7F, 0xFF,
   0x7F, 0xFF, 0x7F, 0xFF,
   0x7F, 0x88, 0x7F, 0xFF,
};
#endif

_Texture::_Texture(void) {
   gl_nameid    = (sUI)-1;
   flags        = 0;
   priority     = 0.0f;
   target       = 0; // Use flags
   mipmap_level = 0;
   b_autogen_id = 1;
   sz_hint      = 1u;
}

_Texture::~_Texture() {
   // yac_host->printf("[dbg] ::~_Texture 1 (ENTER)\n");
   _free();
   // yac_host->printf("[dbg] ::~_Texture 2 (LEAVE)\n");
}

sBool YAC_VCALL _Texture::yacArrayAlloc(sUI _sx, sUI _sy, sUI, sUI _z) {
   return _alloc((sSI)_sx, (sSI)_sy, (sSI)_z);
}

void YAC_VCALL _Texture::yacArraySet(void *_context, sUI _index, YAC_Value *_value) {
   if(_value->type==YAC_TYPE_FLOAT)
   {
      _value->type = YAC_TYPE_INT;
      _value->value.int_val = (sSI)_value->value.float_val;
   }
   if(_value->type==1)
      if(image.screen&&image.screen->data.any)
      {
         if(((sU32)_index)<((sU32)(image.screen->s.x*image.screen->s.y)))
         {
            switch(image.screen->z)
            {
               case 1:
                  image.screen->data.u8[((sU32)_index)]=(sU8)_value->value.int_val;
                  break;
               case 2:
                  image.screen->data.u16[((sU32)_index)]=(sU16)_value->value.int_val;
                  break;
               case 3:
               {
                  sU32 ti=((sU32)_index)*3;
                  image.screen->data.u8[ti++]=(sU8)(_value->value.int_val>>16);
                  image.screen->data.u8[ti++]=(sU8)(_value->value.int_val>>8);
                  image.screen->data.u8[ti++]=(sU8)(_value->value.int_val);
               }
               break;
               case 4:
                  image.screen->data.u32[((sU32)_index)]=(sU32)_value->value.int_val;
                  break;
            }
         }
      }
}

void YAC_VCALL _Texture::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   _r->deleteme=0;
   _r->type=1;

   /*yac_host->printf("yacArrayGet(%i)=%08x sx=%i sy=%i z=%i\n",_index, _r->value.int_val,
     image.screen->s.x,
     image.screen->s.y,
     image.screen->z);*/

   if(image.screen&&image.screen->z)
   {
      sUI ids=(image.screen->stride/image.screen->z);
      sUI idy=_index/ids;
      sUI idx=_index-(idy*ids);
      //if(((sU32)_index)<((sU32)((image.screen->stride/image.screen->z)*image.screen->s.y)))
      if((idx<((sUI)image.screen->s.x))&&(idy<((sUI)image.screen->s.y)))
      {
         switch(image.screen->z)
         {
            case 1:
               _r->value.int_val=(sU8)image.screen->data.u8[((sU32)_index)];
               return;
            case 2:
               _r->value.int_val=(sU16)image.screen->data.u16[((sU32)_index)];
               return;
            case 3:
            {
               sU32 ti=((sU32)_index)*3;
               _r->value.int_val=
                  (((sU8)image.screen->data.u8[ti+0])<<16)|
                  (((sU8)image.screen->data.u8[ti+1])<<8 )|
                  (((sU8)image.screen->data.u8[ti+2]     ));
            }
            return;
            case 4:
               _r->value.int_val=(sU32)image.screen->data.u32[((sU32)_index)];
               return;
         }
      }
   }
   _r->value.int_val=0;
}

sUI  YAC_VCALL _Texture::yacArrayGetElementByteSize(void) {
   if(image.screen) return image.screen->z;
   else             return 0; // tags JIT support
}

sUI YAC_VCALL _Texture::yacArrayGetWidth(void) {
   if(image.screen) return image.screen->s.x;
   else             return 0;
}

sUI YAC_VCALL _Texture::yacArrayGetHeight(void) {
   if(image.screen) return image.screen->s.y;
   else             return 0;
}

sUI YAC_VCALL _Texture::yacArrayGetElementType(void) {
   return 1;
}

sUI YAC_VCALL _Texture::yacArrayGetStride(void) {
   if(image.screen) return image.screen->stride;
   else             return 0;
}

void *YAC_VCALL _Texture::yacArrayGetPointer(void) {
   if(image.screen) return image.screen->data.any;
   else             return 0;
}

sUI YAC_CALL _Texture::yacArrayGetNumElements(void) {
   if(image.screen)
      return image.screen->s.x * image.screen->s.y;
   else
      return 0u;
}

sUI YAC_CALL _Texture::yacArrayGetMaxElements(void) {
   return yacArrayGetNumElements();
}

sUI _Texture::getGlTextureTarget(void) {
   // Get target for "glBindTexture" and similar operations
   if(0 != target)
   {
      if( (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X) &&
          (target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
          )
      {
         return GL_TEXTURE_CUBE_MAP;
      }
      else
      {
         return target;
      }
   }
   else
   {
      if(0 != (flags & TEX_1D))
         return GL_TEXTURE_1D;
      else
         return GL_TEXTURE_2D;
   }
}

sUI _Texture::getGlTextureUploadTarget(void) {
   // Get target for "glTexImage" and similar operations
   if(0 != target)
   {
      return target;
   }
   else
   {
      if(0 != (flags & TEX_1D))
         return GL_TEXTURE_1D;
      else
         return GL_TEXTURE_2D;
   }
}

void _Texture::fixFlags(void) {
   GLenum texTarget = getGlTextureTarget();

   switch(texTarget)
   {
      default:
         break;

      case GL_TEXTURE_RECTANGLE:
         if(0 != (flags & TEX_REPEAT_S))
         {
            flags &= ~TEX_REPEAT_S;
            yac_host->printf("[~~~] Texture::fixFlags: REPEAT_S not supported for GL_TEXTURE_RECTANGLE, removing..\n");
         }

         if(0 != (flags & TEX_REPEAT_T))
         {
            flags &= ~TEX_REPEAT_T;
            yac_host->printf("[~~~] Texture::fixFlags: REPEAT_T not supported for GL_TEXTURE_RECTANGLE, removing..\n");
         }

         flags |= TEX_CLAMPTOEDGE_S;
         flags |= TEX_CLAMPTOEDGE_T;
         break;
   }
}

void _Texture::_setFlags(sUI _fl) {
   flags = (sUI)_fl;

   fixFlags();
}

sUI _Texture::_getFlags(void) {
   return flags;
}

sUI  _Texture::_getStride(void) {
   if(image.screen)
      return image.screen->stride/image.screen->z;
   else
      return 0;
}

void _Texture::_setScreen(YAC_Object *_t) {
   if(YAC_CHK(_t, clid_Texture))
   {
      _Texture *t=(_Texture*)_t;
      image.setScreen(t->image.screen);
   }
   else
   {
      yac_host->printf("\n[---] Texture::setScreen: _t is not a Texture.\n");
   }
}

////void _Texture::_setScreenArea(YAC_Object *_t, YAC_Object *_ivlt, YAC_Object *_ivsz) {
void _Texture::_setScreenArea(YAC_Object *_t, sSI _ltx, sSI _lty, sSI _sx, sSI _sy) {
   if(YAC_CHK(_t, clid_Texture))
   {
      _Texture *t=(_Texture*)_t;
      image.setScreenArea(t->image.screen, _ltx, _lty, _sx, _sy);
   }
   else
   {
      yac_host->printf("\n[---] Texture::setScreenArea: invalid Texture object.\n");
   }
}

sBool _Texture::_loadImage(YAC_String *_s, sSI _scx, sSI _scy, sSI _z) {
   return image.load(0, (char*)_s->chars, _z, 4/*cmapz*/, _scx, _scy);
}

sBool _Texture::_loadLocalImage(YAC_String *_s, sSI _scx, sSI _scy, sSI _z) {
   return image.load(1, (char*)_s->chars, _z, 4/*cmapz*/, _scx, _scy);
}

sBool _Texture::_loadImageStream(YAC_Object *_ofs, sSI _scx, sSI _scy, sSI _z) {
   return image.loadStream(_ofs, _z, 4/*cmapz*/, _scx, _scy);
}

sBool _Texture::_saveImage(YAC_String *_s) {
   yac_host->printf("[...] Texture::saveImage(\"%s\")\n", (char*)_s->chars);
   return image.savePNG((char*)_s->chars);
}

sBool _Texture::_saveImageStream(YAC_Object *_ofs) {
   return image.savePNGStream(_ofs/*png_io*/);
}

sF32 _Texture::_setPriority(sF32 _p) {
   sF32 r=priority;
   priority=_p;

#ifndef DX_GLES
   if( ((sUI)-1) != gl_nameid )
   {
      // update priority
      ::glPrioritizeTextures(1, (GLuint*)&gl_nameid, &priority);
      Dtraceglerror("glPrioritizeTextures");
   }
#endif // DX_GLES

   return r;
}

sSI _Texture::_getId(void) {
   return gl_nameid;
}

void _Texture::_setId(sSI _glTextureNameId) {

   // Delete old texture id if it was autogenerated
   _unload();

   if(-1 != _glTextureNameId)
   {
      // Use "shared" texture id
      b_autogen_id = 0;
   }
   else
   {
      // Re-enable automatic texture id
      b_autogen_id = 1;
   }
   gl_nameid = (sUI) _glTextureNameId;
}

void _Texture::_lazyGenId() {
   // called when texture is bound as FBO (gledit.GFBO)
   if(1 == b_autogen_id)
   {
      if(((sSI)gl_nameid) == -1)
      {
         gl_nameid = _zglGenTexture();
      }
   }
}

void _Texture::_update(void) {
   upload(0, 0, 0, 0, 0);
}

void _Texture::_updateRegion(sSI _x, sSI _y, sSI _w, sSI _h) {
   if( -1 == ((sSI)gl_nameid) )
   {
      upload(1,0,0,0,0);
   }
   else
   {
      sUI x = (sUI) _x;
      sUI y = (sUI) _y;
      sUI w = (sUI) _w;
      sUI h = (sUI) _h;
      if( (x+w) <= (sUI)image.screen->s.x )
      {
         if( (y+h) <= (sUI)image.screen->s.y )
         {
            upload(YAC_FALSE/*need_newname*/, x, y, w, h);
         }
      }
   }
}

void _Texture::_upload(void) {
   upload(b_autogen_id, 0, 0, 0, 0);
}

void _Texture::upload(sBool need_newname, sUI _x, sUI _y, sUI _w, sUI _h) {
   sBool bSwapRB = YAC_FALSE;

   fixFlags();

   if(1 == b_autogen_id)
   {
      if(need_newname || ( ((sSI)gl_nameid)==-1) )
      {
         // do not gen new name if texture is re-uploaded
         gl_nameid = _zglGenTexture();

         flags |= TEX_UPLOADED;
      }
   }
   else
   {
      // Use "shared" texture name id
   }

   // ---- onboard textures have no system memory screen ----
   if(NULL != image.screen)
   {
      GLenum texTarget = getGlTextureTarget();
      GLenum texUploadTarget = getGlTextureUploadTarget(); // May be different than texTarget (e.g. cube maps)

      _glBindTexture(texTarget, gl_nameid);

      int int_format = 0;
      int pix_format = 0;
      int type = 0;

      switch(image.screen->z)
      {
         default:
         case 0:
            yac_host->printf("\n[---] Texture::upload: invalid color depth.\n");
            break;

         case 1: // alpha
            type = GL_UNSIGNED_BYTE;

            if( (flags & TEX_INTENSITY) || (flags & TEX_ALPHA) )
            {
               /////int_format=GL_INTENSITY;
               /////pix_format=GL_ALPHA;

               // (note) intensity rgba=(v,v,v,v); luminace rgba=(v,v,v,1.0)
#ifdef DX_GLES
               // (note) SGX533/GLES alpha=(0,0,0,v) but shader will fix it to (v,v,v,v)
               int_format = GL_ALPHA;
               pix_format = GL_ALPHA;
#else
               if(tkopengl_b_glcore)
               {
                  int_format = GL_R8;
                  pix_format = GL_RED;
               }
               else
               {
                  int_format = GL_INTENSITY;
                  pix_format = GL_LUMINANCE;
               }
#endif // DX_GLES
            }
            else
            {
#ifdef DX_GLES
               int_format = GL_LUMINANCE;
               pix_format = GL_LUMINANCE;
#else
               if(tkopengl_b_glcore)
               {
                  int_format = GL_R8;
                  pix_format = GL_RED;
               }
               else
               {
                  int_format = GL_LUMINANCE8;
                  pix_format = GL_LUMINANCE;
               }
#endif // DX_GLES
            }
            //yac_host->printf("upload LUMINANCE texture.\n");
            break;

         case 2:
#ifdef DX_GLES
            int_format = GL_RGB;
            pix_format = GL_RGB;
            type = GL_UNSIGNED_SHORT_5_6_5;
#else
            int_format = GL_RGB5;
            pix_format = GL_BGRA;
            type = GL_UNSIGNED_SHORT; //GL_5_6_5;
#endif // DX_GLES
            break;

         case 3:
#ifndef DX_GLES
            if(flags & TEX_16BIT)
            {
               int_format = GL_RGB5;
            }
            else
            {
               //int_format=GL_RGB;
               int_format = GL_RGB8; // xxx equivalent to GL_RGB ?!
            }
#else
            int_format = GL_RGB;
#endif // DX_GLES
            pix_format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
            //yac_host->printf("upload RGB texture.\n");
            break;

         case 4:
            if(0u == (flags & TEX_ALPHA))
            {
#ifdef DX_GLES
               ////int_format = GL_RGB;  // (note) does not work on PVR SGX533 when pix_format is GL_RGBA
               int_format = GL_RGBA;
#else
               if(flags & TEX_16BIT)
               {
                  int_format = GL_RGB5;
               }
               else
               {
                  // // int_format = 3; // xxx 3? (GL_RGB?)
                  int_format = GL_RGB;
               }
#endif // DX_GLES
            }
            else
            {
#ifdef DX_GLES
               int_format = GL_RGBA;
#else
               // // int_format = 4; // xxx 4? (GL_RGBA?)
               int_format = GL_RGBA;
#endif // DX_GLES
            }

            type = GL_UNSIGNED_BYTE;

#ifdef YAC_LITTLE_ENDIAN
            if(tkopengl_b_glcore)
            {
               int_format = GL_RGBA;
               pix_format = GL_BGRA_EXT;
            }
            else if(flags & TEX_FORCEHOSTBYTEORDER)
            {
               pix_format = GL_BGRA;    // ARGB_8
            }
            else
            {
               pix_format = GL_RGBA;
               bSwapRB    = YAC_TRUE;   // ARGB32 => ABGR32 (RGBA_8)
            }
#else
            pix_format = GL_RGBA;
#endif
            break;
      }

      if(0u != (flags & TEX_REPEAT_S))
      {
         _glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
      }
      else if(0u != (flags & TEX_CLAMPTOEDGE_S))
      {
         _glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      }
      else //if(flags&TEX_CLAMP_S)
      {
// // #if defined(DX_GLES) || !DX_GL_COMPATIBILITY
         _glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// // #else
// //          _glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
// // #endif // DX_GLES
      }

      if(0u != (flags & TEX_REPEAT_T))
      {
         _glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
      }
      else if(0u != (flags & TEX_CLAMPTOEDGE_T))
      {
         _glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      }
      else //if(flags&TEX_CLAMP_T)
      {
// // #if defined(DX_GLES) || !DX_GL_COMPATIBILITY
         _glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// // #else
// //          _glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
// // #endif // DX_GLES
      }

      // (note) there's currently [26Nov2018] no flag for TEX_REPEAT_R => use clamp
      _glTexParameteri(texTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


      // (todo) n/a in GL4core profile (-DDX_GL4_CORE)
#if DX_GL_COMPATIBILITY
      if(!tkopengl_b_glcore)
      {
         if(0u != (flags & TEX_MODULATE))
         {
            _glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
         }
         else if(0u != (flags & TEX_BLEND)) // slow (2001)
         {
            _glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
         }
         else if(0u != (flags & TEX_DECAL))
         {
            _glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
         }
         else
         {
            _glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
         }

         //glTexEnviv(GL_TEXTURE_2D, GL_TEXTURE_ENV_COLOR, envcol);
      }
#endif // DX_GL_COMPATIBILITY


      if(bSwapRB)
      {
         yacmemptr s;
         sSI y;
         sSI isx, isy;

         if(_w)
         {
            s.u8 = &image.screen->data.u8[_y*image.screen->stride + _x];
            isx = _w;
            isy = _h;
         }
         else
         {
            s.u32 = image.screen->data.u32;
            isx = image.screen->s.x;
            isy = image.screen->s.y;
         }

         for(y = 0; y < isy; y++)
         {
            sSI x;
            sU32 c32;

            for(x = 0; x < isx; x++)
            {
               // ARGB32 => ABGR32 (R,G,B,A order on little endian)
               c32 = s.u32[x];
               c32 =
                  (((c32>>24)&0xFF)<<24 ) |
                  (((c32>>16)&0xFF)     ) |
                  (((c32>> 8)&0xFF)<<8  ) |
                  (((c32    )&0xFF)<<16 ) ;
               s.u32[x] = c32;
            }

            s.u8 += image.screen->stride;
         }
      }


      // Enable/disable automatic mipmap generation
      GLenum error = _glGetError_int();
      if(GL_NO_ERROR != error)
      {
         yac_host->printf("[~~~] Texture::upload: detected GL error %d before glTexParameteri(..GL_GENERATE_MIPMAP..) call\n", error);
      }

#if !defined(DX_GLES) && DX_GL_COMPATIBILITY
      if(!tkopengl_b_glcore)
      {
         if(flags & TEX_AUTOMIPMAP)
         {
            _glTexParameteri(texTarget, GL_GENERATE_MIPMAP, GL_TRUE);
         }
         else
         {
            _glTexParameteri(texTarget, GL_GENERATE_MIPMAP, GL_FALSE);
         }
      }
#endif // DX_GLES && DX_GL_COMPATIBILITY

      error = _glGetError_int();
      if(GL_NO_ERROR != error)
      {
         // Silently ignore GL error in case GL_GENERATE_MIPMAP is not supported
         if(yac_host->yacGetDebugLevel() > 0)
         {
            yac_host->printf("[~~~] Texture::upload: detected GL error %d after glTexParameteri(..GL_GENERATE_MIPMAP..) call\n", error);
         }
      }


      if(GL_TEXTURE_1D == texTarget)
      {
         _glTexImage1D_int(GL_TEXTURE_1D,
                           mipmap_level,
                           int_format,
                           image.screen->s.x,
                           0, // border
                           pix_format,
                           type,
                           image.screen->data.any
                           );
      }
      else if((GL_TEXTURE_2D == texTarget) ||
              (GL_TEXTURE_CUBE_MAP == texTarget) ||
              (GL_TEXTURE_RECTANGLE == texTarget)
              )
      {
#ifndef DX_GLES
         _glPixelStorei_int(GL_UNPACK_ROW_LENGTH, image.screen->stride/image.screen->z);
#endif // DX_GLES
         _glPixelStorei_int(GL_UNPACK_ALIGNMENT, 1); // tightly packed

#ifdef DEBUG_UPLOAD
         yac_host->printf("[dbg] TKSTexture::upload: target=0x%04x mipmap_level=%d int_format=0x%04x pix_format=0x%04x type=0x%04x\n", (sU16)texUploadTarget, mipmap_level, (sU16)int_format, (sU16)pix_format, (sU16)type);
#endif

         if(_w)
         {
            _glTexSubImage2D_int(texUploadTarget,
                                 mipmap_level,
                                 _x,
                                 _y,
                                 _w,
                                 _h,
                                 pix_format,
                                 type,
                                 image.screen->data.u8 + (image.screen->stride * _y) + (_x * image.screen->z)
                                 );
            //glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

         }
         else
         {
            _glTexImage2D_int(texUploadTarget,
                              mipmap_level,
                              int_format,
                              image.screen->s.x,
                              image.screen->s.y,
                              0, // border
                              pix_format,
                              type,
                              image.screen->data.any
                              );
         }
      }
      else if((GL_TEXTURE_3D == texTarget) ||
              (GL_TEXTURE_2D_ARRAY == texTarget)
              )
      {
#ifndef DX_GLES
         _glPixelStorei_int(GL_UNPACK_ROW_LENGTH, image.screen->stride/image.screen->z);
#endif // DX_GLES
         _glPixelStorei_int(GL_UNPACK_ALIGNMENT, 1); // tightly packed
         _glPixelStorei_int(GL_UNPACK_IMAGE_HEIGHT, 0); // =use glTexImage3d height param

         // (todo) support w/h/x/y params (texsubimage) (+add z param. add updateRegion3d() method ?!)
         sUI uploadSy = image.screen->s.y / sz_hint;
         sUI uploadSz = sz_hint;

         if(uploadSy > 0u)
         {
            sUI uploadSx = image.screen->s.x;

#ifdef DEBUG_UPLOAD
            yac_host->printf("[dbg] TKSTexture::upload: target=0x%04x mipmap_level=%d int_format=0x%04x pix_format=0x%04x w=%u h=%u d=%u (totalH=%u) type=0x%04x\n", (sU16)texUploadTarget, mipmap_level, (sU16)int_format, (sU16)pix_format, uploadSx, uploadSy, uploadSz, (uploadSy * uploadSz), (sU16)type);
#endif

            _glTexImage3D_int(texUploadTarget,
                              mipmap_level,
                              int_format,
                              uploadSx,
                              uploadSy,
                              uploadSz,
                              0, // border
                              pix_format,
                              type,
                              image.screen->data.any
                              );

         }
         else
         {
            yac_host->printf("[---] Texture::upload: array/depth slice height is 0 (sy=%u, sz=%u)\n", image.screen->s.y, sz_hint);
         }
      }
      else
      {
         yac_host->printf("[---] Texture::upload: unsupported GL texture target 0x%08x. Missing glTexImage*() codepath.\n", texTarget);
         return;
      }

      if(bSwapRB)
      {
         yacmemptr s;
         sSI y;
         sSI isx, isy;

         if(_w)
         {
            s.u8 = &image.screen->data.u8[_y*image.screen->stride + _x];
            isx = _w;
            isy = _h;
         }
         else
         {
            s.u32 = image.screen->data.u32;
            isx = image.screen->s.x;
            isy = image.screen->s.y;
         }

         for(y=0; y<isy; y++)
         {
            sSI x;
            sU32 c32;
            for(x=0; x<isx; x++)
            {
               c32 = s.u32[x];
               c32 =
                  (((c32>>24)&0xFF)<<24 ) |
                  (((c32>>16)&0xFF)     ) |
                  (((c32>> 8)&0xFF)<<8  ) |
                  (((c32    )&0xFF)<<16 ) ;
               s.u32[x] = c32;
            }

            s.u8 += image.screen->stride;
         }
      }


      if(flags & TEX_MINFILTERLINEAR)
      {
         _glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      }
      else if(flags & TEX_MINFILTERTRILINEAR)
      {
         _glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      }
      else
      {
         _glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      }

      if(flags & TEX_MAGFILTERLINEAR)
      {
         _glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }
      else if(flags & TEX_MAGFILTERTRILINEAR)
      {
         // Note: Actually, there is no such such as a trilinear magfilter..
         _glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }
      else
      {
         _glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      }

#ifndef DX_GLES
      if(priority > 0.0f)
      {
         ::glPrioritizeTextures(1, (GLuint*)&gl_nameid, &priority);
         Dtraceglerror("glPrioritizeTextures");
      }
#endif // DX_GLES

//       if(flags & TEX_AUTOMIPMAP)
//       {
//          // Note: glGenerateMipmap seems to be broken on ATI cards.
//          //       Rumor has it that it wants to be called twice ?
//          if(glGenerateMipmap_xref)
//          {
//             glGenerateMipmap_xref(texTarget);
//          }
//          else
//          {
//             yac_host->printf("[---] Texture::upload: TEX_AUTOMIPMAP: unresolved extension function glGenerateMipmap called.\n");
//          }
//       }

      if(flags & TEX_DELETE)
      {
         _freeImage();
      }
   }
}

void _Texture::_unbind(void) {
   if(gl_nameid != (sUI)-1)
   {
      GLenum texTarget = getGlTextureTarget();
      _glBindTexture(texTarget, 0);
   }
}

void _Texture::_bind(void) {
   fixFlags();

   if(gl_nameid == (sUI)-1)
   {
      _upload();
   }

   if(gl_nameid != (sUI)-1)
   {
      GLenum texTarget = getGlTextureTarget();

      _glBindTexture(texTarget, gl_nameid);

      if(tkopengl_b_glcore)
      {
#if 0
         // (note) GL_ARB_texture_swizzle not supported on macOS (GL 4.1 core profile) => must handle this in shader
         if(0 != (flags & TEX_ALPHA))
         {
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, GL_RED);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, GL_RED);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, GL_RED);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, GL_RED);
         }
         else
         {
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, GL_RED);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
            glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
         }
#endif
      }
      else
      {
         if(0 != (flags & TEX_MODULATE))
         {
            _glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
         }
         else if(0 != (flags & TEX_BLEND)) // slow (2001)
         {
            _glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
         }
         else if(0 != (flags & TEX_DECAL))
         {
            _glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
         }
         else
         {
            _glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
         }
      }

      // How to filter when texture is scaled down
      if(0 != (flags & TEX_MINFILTERLINEAR))
      {
         _glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      }
      else if(0 != (flags & TEX_MINFILTERTRILINEAR))
      {
         _glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      }
      else
      {
         _glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      }

      // How to filter when texture is scaled up
      if(0 != (flags & TEX_MAGFILTERLINEAR))
      {
         _glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }
      else if(0 != (flags & TEX_MAGFILTERTRILINEAR))
      {
         // Note: Actually, there is no such such as a trilinear magfilter..
         _glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }
      else
      {
         _glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      }

      if(!tkopengl_b_glcore)
      {
         if(0 != (flags & TEX_POINTSPRITE))
         {
            _glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
         }
         else
         {
            _glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE);
         }
      }

   }
}

void _Texture::_enable(void) {
   if((sUI)-1 != gl_nameid)
   {
      if(!tkopengl_b_glcore)
      {
         GLenum texTarget = getGlTextureTarget();
         
         _glEnable(texTarget);
      }
   }
}

void _Texture::_disable(void) {
   if((sUI)-1 != gl_nameid)
   {
      if(!tkopengl_b_glcore)
      {
         GLenum texTarget = getGlTextureTarget();
         
         _glDisable(texTarget);
      }
   }
}

void _Texture::_clear(sUI _c32) {
   if(image.screen)
   {
      image.sel.s = image.screen->s;
      image.sel.p.x = image.sel.p.y=0;
      image.drawFilledRect(0,0,image.screen->s.x, image.screen->s.y, _c32);
   }
}

void _Texture::_clearAlpha(sUI _a8) {
   if(image.screen)
   {
      if(4u == image.screen->z)
      {
         yacmemptr d; d.u32 = image.screen->data.u32;
         if(NULL != d.any)
         {
            const sU32 a8Shifted = (_a8 << 24);

            for(sSI y = 0; y < image.screen->s.y; y++)
            {
               for(sSI x = 0; x < image.screen->s.x; x++)
                  d.u32[x] = (d.u32[x] & 0x00FFFFFFu) | a8Shifted;

               d.u8 += image.screen->stride;
            }
         }
      }
   }
}

void _Texture::Expand2n(sSI *_sx, sSI *_sy) {

   // WTF!! get rid of the size table and replace this with some bit twiddeling magic :P

   //static sU16 size_table[16]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};
   static sU16 size_table[12]={16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};
#define E2N_NUM (sizeof(size_table)/sizeof(sU16))
   if((*_sx)<0||(*_sx)>32768)
      return;
   if((*_sy)<0||(*_sy)>32768)
      return;
   if(*_sx<size_table[0])
      *_sx=size_table[0];
   if(*_sy<size_table[0])
      *_sy=size_table[0];
   sS8 i;
   for(i=0; i<((sS8)E2N_NUM); i++)
   {
      if((*_sx)==size_table[i])
         break;
   }

   if(i==((sS8)E2N_NUM))
   {
      // odd texture size, correct
      for(i=(E2N_NUM-2); i>=0; i--)
      {
         if((*_sx)>size_table[i])
         {
            (*_sx)=size_table[i+1];
            break;
         }
      }
   }

   for(i=0; i<((sS8)E2N_NUM); i++)
   {
      if((*_sy)==size_table[i])
         break;
   }

   if(i==((sS8)E2N_NUM))
   {
      // odd texture size, correct
      for(i=(E2N_NUM-2); i>=0; i--)
      {
         if((*_sy)>size_table[i])
         {
            (*_sy)=size_table[i+1];
            break;
         }
      }
   }

   // Make quadratic:
   /*if(*_sx > *_sy)
    *_sy=*_sx;
    else if(*_sy > *_sx)
    *_sx=*_sy;*/
}

sSI _Texture::_alloc2n(sSI _sx, sSI _sy, sSI _z) {
   tksvec2i s;
   Expand2n(&_sx, &_sy);
   s.x=_sx;
   s.y=_sy;
   return image.allocScreen(s, _z);
}

sBool _Texture::_alloc(sSI _sx, sSI _sy, sSI _z) {
   tksvec2i s;
   s.x=_sx;
   s.y=_sy;
   return image.allocScreen(s, _z);
}

void _Texture::_scaleFrom(YAC_Object *_t) {
   if(YAC_CHK(_t, clid_Texture))
   {
      _Texture *t=(_Texture *)_t;
      image.scaleFrom(&t->image);
   }
   else
   {
      yac_host->printf("\n[---] Texture::scaleFrom: Object is not a Texture.\n");
   }
}

void _Texture::_free(void) {
   _freeImage();
   _unload();
}

void _Texture::_unload(void) {
   if(gl_nameid != (sUI)-1)
   {
      if(1 == b_autogen_id)
      {
         _zglDeleteTexture(gl_nameid);
         gl_nameid = (sUI)-1;
         flags &= ~TEX_UPLOADED;
      }
   }
}

void _Texture::_freeImage(void) {
   image.deleteScreen();
   image.deleteColormap();
}

void _Texture::_setEnvColor(sSI _i) {
   if(!tkopengl_b_glcore)
   {
      sU32 c32=(sU32)_i;
      sF32 envcol[4];
      envcol[0]=((c32>>16)&0xFF)*(1.0f/255.0f);
      envcol[1]=((c32>> 8)&0xFF)*(1.0f/255.0f);
      envcol[2]=((c32    )&0xFF)*(1.0f/255.0f);
      envcol[3]=((c32>>24)&0xFF)*(1.0f/255.0f);
      // update environment color

      _glTexEnvfv_int(getGlTextureTarget(), GL_TEXTURE_ENV_COLOR, envcol);
   }
}


////void _Texture::_copyRegion(YAC_Object *_tex, YAC_Object *_ivecp, YAC_Object *_ivecs, YAC_Object *_ivecd) {
void _Texture::_copyRegion(YAC_Object *_tex, sSI _px, sSI _py, sSI _sx, sSI _sy, sSI _dx, sSI _dy) {
   if(YAC_CHK(_tex, clid_Texture))
   {
      _Texture*t=(_Texture*)_tex;
      image.copyRegion(&t->image, _px, _py, _sx, _sy, _dx, _dy);
   }
}

void _Texture::_setFgcolor(sUI _c32) {
   image.fgcolor.r = (sU8)(_c32>>16);
   image.fgcolor.g = (sU8)(_c32>> 8);
   image.fgcolor.b = (sU8)(_c32    );
   image.fgcolor.a = (sU8)(_c32>>24);
}

sUI _Texture::_getFgcolor(void) {
   return (  ((sU32)(image.fgcolor.r<<16)|(image.fgcolor.g<<8)|(image.fgcolor.b)|(image.fgcolor.a<<24)) );
}

sSI _Texture::_getSx(void) {
   if(NULL != image.screen)
   {
      return image.screen->s.x;
   }
   else
   {
      return 0;
   }
}

sSI _Texture::_getSy(void) {
   if(NULL != image.screen)
   {
      return image.screen->s.y;
   }
   else
   {
      return 0;
   }
}

sSI _Texture::_getZ(void) {
   if(NULL != image.screen)
   {
      return image.screen->z;
   }
   else
   {
      return 0;
   }
}

void _Texture::_setSz(sUI _sz) {
   if((_sz > 1u) && (_sz <= 4096u))  // note: GL must support at least 256 depth slices
   {
      sz_hint = _sz;
   }
}

sUI _Texture::_getSz(void) {
   return sz_hint;
}

void _Texture::_drawPixels2i(sSI _x, sSI _y) {
   _drawPixels3f((sF32)_x, (sF32)_y, 0.0f);
}

void _Texture::_drawPixels2f(sF32 _x, sF32 _y) {
   _drawPixels3f(_x, _y, 0.0f);
}

/*void _Texture::_drawPixels(YAC_Object *_rasterposvector) {
  if(YAC_CHK(_rasterposvector, clid_Vector))
  {
  _Vector *rpos=(_Vector*)_rasterposvector;
  _drawPixels3f(rpos->x, rpos->y, rpos->z);
  }
  else
  {
  yac_host->printf("[---] Texture::drawPixels: invalid vector object.\n");
  }
  }*/

void _Texture::_drawPixels3f(sF32 _x, sF32 _y, sF32 _z) {
   if(image.screen)
   {
      if(image.screen->data.any)
      {
         _glRasterPos3f(_x, _y, _z);

         switch(image.screen->z)
         {
            case 1:
               _glDrawPixels_int(image.screen->s.x, image.screen->s.y, GL_LUMINANCE, GL_UNSIGNED_BYTE, image.screen->data.any);
               break;

            case 3:
               _glDrawPixels_int(image.screen->s.x, image.screen->s.y, GL_RGB, GL_UNSIGNED_BYTE, image.screen->data.any);
               break;

            case 4:
               _glDrawPixels_int(image.screen->s.x, image.screen->s.y, GL_BGRA, GL_UNSIGNED_BYTE, image.screen->data.any);
               break;
         }

         _glRasterPos3f(0.0f, 0.0f, 0.0f);
      }
   }
}

void _Texture::_swapScreen(YAC_Object *_tex) {
   if(YAC_CHK(_tex, clid_Texture))
   {
      _Texture*tex=(_Texture*)_tex;
      Bitmap *scr=image.screen;
      image.screen=tex->image.screen;
      tex->image.screen=scr;
      sBool del=image.ownscreen;
      image.ownscreen=tex->image.ownscreen;
      tex->image.ownscreen=del;
      //-------- update current selection --------
      image.sel.p.x=0;
      image.sel.p.y=0;
      tex->image.sel.p.x=0;
      tex->image.sel.p.y=0;
      if(image.screen)
      {
         image.sel.s.x=image.screen->s.x;
         image.sel.s.y=image.screen->s.y;
      }
      if(tex->image.screen)
      {
         tex->image.sel.s.x=tex->image.screen->s.x;
         tex->image.sel.s.y=tex->image.screen->s.y;
      }
   }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ _TextureModBlur                                              ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define DgetPixel8(a,b) (((a)>=0&&(a)<_i->screen->s.x)&&((b)>=0&&(b)<_i->screen->s.y))?_i->screen->data.u8[((a)*_i->screen->z)+((b)*_i->screen->stride)]:0;
#define DgetPixel32(a,b) (((a)>=0&&(a)<_i->screen->s.x)&&((b)>=0&&(b)<_i->screen->s.y))? *(sU32*)(&_i->screen->data.u8[((a)*_i->screen->z)+((b)*_i->screen->stride)]):0;

void _Texture::simpleBlur(Image *_i, Image *_j) {
   if(_i->screen&&_j->screen)
      if(_i->screen->z==_j->screen->z)
         if(_j->screen->s.x>=_i->screen->s.x)
            if(_j->screen->s.y>=_i->screen->s.y)
               if(_i->screen->data.any&&_j->screen->data.any)
               {
                  yacmemptr s;
                  s.u8=_i->screen->data.u8;
                  yacmemptr d;
                  d.u8=_j->screen->data.u8;
                  //sU16 cr=0;
                  sS16 x,y;
                  //for(; cr<_repeat; cr++)
                  {
                     switch(_i->screen->z)
                     {
                        case 1:
                           for(y=0; y<_i->screen->s.y; y++)
                           {
                              for(x=0; x<_i->screen->s.x; x++)
                              {
                                 sU8 p[4][4];
                                 p[0][0]=DgetPixel8(x-2, y-2);
                                 p[1][0]=DgetPixel8(x-1, y-2);
                                 p[2][0]=DgetPixel8(x-0, y-2);
                                 p[3][0]=DgetPixel8(x+1, y-2);
                                 p[0][1]=DgetPixel8(x-2, y-1);
                                 p[1][1]=DgetPixel8(x-1, y-1);
                                 p[2][1]=DgetPixel8(x-0, y-1);
                                 p[3][1]=DgetPixel8(x+1, y-1);
                                 p[0][2]=DgetPixel8(x-2, y-0);
                                 p[1][2]=DgetPixel8(x-1, y-0);
                                 p[2][2]=DgetPixel8(x-0, y-0);
                                 p[3][2]=DgetPixel8(x+1, y-0);
                                 p[0][3]=DgetPixel8(x-2, y+1);
                                 p[1][3]=DgetPixel8(x-1, y+1);
                                 p[2][3]=DgetPixel8(x-0, y+1);
                                 p[3][3]=DgetPixel8(x+1, y+1);
                                 d.u8[x]=
                                    (
                                       p[0][0]+p[1][0]+p[2][0]+p[3][0]+
                                       p[0][1]+p[1][1]+p[2][1]+p[3][1]+
                                       p[0][2]+p[1][2]+p[2][2]+p[3][2]+
                                       p[0][3]+p[1][3]+p[2][3]+p[3][3])>>4;
                              }
                              s.u8+=_i->screen->stride;
                              d.u8+=_j->screen->stride;
                           }
                           break;
                        case 4:
                        {
                           sU32 p[4][4];
                           sU8 asum;
                           sU8 rsum;
                           sU8 gsum;
                           sU8 bsum;
                           for(y=0; y<_i->screen->s.y; y++)
                           {
                              for(x=0; x<_i->screen->s.x; x++)
                              {
                                 p[0][0]=DgetPixel32(x-2, y-2);
                                 p[1][0]=DgetPixel32(x-1, y-2);
                                 p[2][0]=DgetPixel32(x-0, y-2);
                                 p[3][0]=DgetPixel32(x+1, y-2);
                                 p[0][1]=DgetPixel32(x-2, y-1);
                                 p[1][1]=DgetPixel32(x-1, y-1);
                                 p[2][1]=DgetPixel32(x-0, y-1);
                                 p[3][1]=DgetPixel32(x+1, y-1);
                                 p[0][2]=DgetPixel32(x-2, y-0);
                                 p[1][2]=DgetPixel32(x-1, y-0);
                                 p[2][2]=DgetPixel32(x-0, y-0);
                                 p[3][2]=DgetPixel32(x+1, y-0);
                                 p[0][3]=DgetPixel32(x-2, y+1);
                                 p[1][3]=DgetPixel32(x-1, y+1);
                                 p[2][3]=DgetPixel32(x-0, y+1);
                                 p[3][3]=DgetPixel32(x+1, y+1);
                                 asum=
                                    (
                                       ((p[0][0]>>24)&0xFF)+((p[1][0]>>24)&0xFF)+((p[2][0]>>24)&0xFF)+((p[3][0]>>24)&0xFF)+
                                       ((p[0][1]>>24)&0xFF)+((p[1][1]>>24)&0xFF)+((p[2][1]>>24)&0xFF)+((p[3][1]>>24)&0xFF)+
                                       ((p[0][2]>>24)&0xFF)+((p[1][2]>>24)&0xFF)+((p[2][2]>>24)&0xFF)+((p[3][2]>>24)&0xFF)+
                                       ((p[0][3]>>24)&0xFF)+((p[1][3]>>24)&0xFF)+((p[2][3]>>24)&0xFF)+((p[3][3]>>24)&0xFF)
                                     ) >> 4;
                                 rsum=
                                    (
                                       ((p[0][0]>>16)&0xFF)+((p[1][0]>>16)&0xFF)+((p[2][0]>>16)&0xFF)+((p[3][0]>>16)&0xFF)+
                                       ((p[0][1]>>16)&0xFF)+((p[1][1]>>16)&0xFF)+((p[2][1]>>16)&0xFF)+((p[3][1]>>16)&0xFF)+
                                       ((p[0][2]>>16)&0xFF)+((p[1][2]>>16)&0xFF)+((p[2][2]>>16)&0xFF)+((p[3][2]>>16)&0xFF)+
                                       ((p[0][3]>>16)&0xFF)+((p[1][3]>>16)&0xFF)+((p[2][3]>>16)&0xFF)+((p[3][3]>>16)&0xFF)
                                     ) >> 4;

                                 gsum=
                                    (
                                       ((p[0][0]>> 8)&0xFF)+((p[1][0]>> 8)&0xFF)+((p[2][0]>> 8)&0xFF)+((p[3][0]>> 8)&0xFF)+
                                       ((p[0][1]>> 8)&0xFF)+((p[1][1]>> 8)&0xFF)+((p[2][1]>> 8)&0xFF)+((p[3][1]>> 8)&0xFF)+
                                       ((p[0][2]>> 8)&0xFF)+((p[1][2]>> 8)&0xFF)+((p[2][2]>> 8)&0xFF)+((p[3][2]>> 8)&0xFF)+
                                       ((p[0][3]>> 8)&0xFF)+((p[1][3]>> 8)&0xFF)+((p[2][3]>> 8)&0xFF)+((p[3][3]>> 8)&0xFF)
                                     ) >> 4;
                                 bsum=
                                    (
                                       ((p[0][0]    )&0xFF)+((p[1][0]    )&0xFF)+((p[2][0]    )&0xFF)+((p[3][0]    )&0xFF)+
                                       ((p[0][1]    )&0xFF)+((p[1][1]    )&0xFF)+((p[2][1]    )&0xFF)+((p[3][1]    )&0xFF)+
                                       ((p[0][2]    )&0xFF)+((p[1][2]    )&0xFF)+((p[2][2]    )&0xFF)+((p[3][2]    )&0xFF)+
                                       ((p[0][3]    )&0xFF)+((p[1][3]    )&0xFF)+((p[2][3]    )&0xFF)+((p[3][3]    )&0xFF)
                                     ) >> 4;

                                 asum=( asum+((p[2][2]>>24)&0xFF) )>>1;
                                 rsum=( rsum+((p[2][2]>>16)&0xFF) )>>1;
                                 gsum=( gsum+((p[2][2]>> 8)&0xFF) )>>1;
                                 bsum=( bsum+((p[2][2]    )&0xFF) )>>1;
                                 asum=( asum+((p[2][2]>>24)&0xFF) )>>1;
                                 rsum=( rsum+((p[2][2]>>16)&0xFF) )>>1;
                                 gsum=( gsum+((p[2][2]>> 8)&0xFF) )>>1;
                                 bsum=( bsum+((p[2][2]    )&0xFF) )>>1;
                                 d.u32[x]=(asum<<24)|(rsum<<16)|(gsum<<8)|(bsum);
                                 //d.u32[x]=p[2][2];
                                 //d.u32[x]=s.u32[x];
                              }
                              s.u8+=_i->screen->stride;
                              d.u8+=_j->screen->stride;
                           }
                        }
                        break;
                     }
                  }
               }
}

void _Texture::simpleBlurX(Image *_i, Image *_j) {
   if(_i->screen&&_j->screen)
      if(_i->screen->z==_j->screen->z)
         if(_j->screen->s.x>=_i->screen->s.x)
            if(_j->screen->s.y>=_i->screen->s.y)
               if(_i->screen->data.any&&_j->screen->data.any)
               {
                  yacmemptr s;
                  s.u8=_i->screen->data.u8;
                  yacmemptr d;
                  d.u8=_j->screen->data.u8;
                  sS16 x,y;
                  {
                     switch(_i->screen->z)
                     {
                        case 1:
                        {
                           sU8 p[3];
                           sU8 t;
                           for(y=0; y<_i->screen->s.y; y++)
                           {
                              for(x=0; x<_i->screen->s.x; x++)
                              {
                                 p[0]=DgetPixel8(x-1, y);
                                 p[1]=DgetPixel8(x  , y);
                                 p[2]=DgetPixel8(x+1, y);
                                 t=(p[0]+p[2])>>1;
                                 d.u8[x]=(p[1]+t)>>1;
                              }
                              s.u8+=_i->screen->stride;
                              d.u8+=_j->screen->stride;
                           }
                        }
                        break;
                        case 4:
                        {
                           sU8 p[3];
                           sU8 t;
                           sS16 xil;
                           sS16 xim;
                           sS16 xir;
                           for(y=0; y<_i->screen->s.y; y++)
                           {
                              xil=-4;
                              xim=0;
                              xir=4;
                              for(x=0; x<_i->screen->s.x; x++,xil+=4,xim+=4,xir+=4)
                              {
                                 /** blue **/
                                 p[0]=DgetPixel8( xil+0, y);
                                 p[1]=DgetPixel8( xim+0, y);
                                 p[2]=DgetPixel8( xir+0, y);
                                 t=(p[0]+p[2])>>1;
                                 d.u8[xim+0]=(p[1]+t)>>1;
                                 /** green **/
                                 p[0]=DgetPixel8( xil+1, y);
                                 p[1]=DgetPixel8( xim+1, y);
                                 p[2]=DgetPixel8( xir+1, y);
                                 d.u8[xim+1]=(p[1]+t)>>1;
                                 /** red **/
                                 p[0]=DgetPixel8( xil+2, y);
                                 p[1]=DgetPixel8( xim+2, y);
                                 p[2]=DgetPixel8( xir+2, y);
                                 d.u8[xim+2]=(p[1]+t)>>1;
                                 /** alpha **/
                                 p[0]=DgetPixel8( xil+3, y);
                                 p[1]=DgetPixel8( xim+3, y);
                                 p[2]=DgetPixel8( xir+3, y);
                                 d.u8[xim+3]=(p[1]+t)>>1;
                              }
                              s.u8+=_i->screen->stride;
                              d.u8+=_j->screen->stride;
                           }
                        }
                        break;
                     }
                  }
               }
}

void _Texture::simpleBlurY(Image *_i, Image *_j) {
   if(_i->screen&&_j->screen)
      if(_i->screen->z==_j->screen->z)
         if(_j->screen->s.x>=_i->screen->s.x)
            if(_j->screen->s.y>=_i->screen->s.y)
               if(_i->screen->data.any&&_j->screen->data.any)
               {
                  //yacmemptr s;
                  //s.u8=_i->screen->data.u8;
                  //yacmemptr d;
                  //d.u8=_j->screen->data.u8;
                  sS16 x,y;
                  {
                     switch(_i->screen->z)
                     {
                        case 1:
                           for(y=0; y<_i->screen->s.y; y++)
                           {
                              for(x=0; x<_i->screen->s.x; x++)
                              {
                                 // (todo) implement me
                              }
                           }
                           break;
                        case 4:
                           for(y=0; y<_i->screen->s.y; y++)
                           {
                              for(x=0; x<_i->screen->s.x; x++)
                              {
                                 // (todo) implement me
                              }
                           }
                           break;
                     }
                  }
               }
}

void _Texture::_simpleBlur(YAC_Object *_t) {
   if(YAC_CHK(_t, clid_Texture))
   {
      _Texture *t=(_Texture*)_t;
      simpleBlur(&t->image, &image);
   }
}

void _Texture::_simpleBlurX(YAC_Object *_t) {
   if(YAC_CHK(_t, clid_Texture))
   {
      _Texture *t=(_Texture*)_t;
      simpleBlurX(&t->image, &image);
   }
}

void _Texture::_simpleBlurY(YAC_Object *_t) {
   if(YAC_CHK(_t, clid_Texture))
   {
      _Texture *t=(_Texture*)_t;
      simpleBlurY(&t->image, &image);
   }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ TextureModColor                                                ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void _Texture::_applyColorMap(YAC_Object *_t, YAC_Object *_cm) {
   if(
      YAC_CHK(_t, clid_Texture)&&
      YAC_CHK(_cm, clid_Texture)
      )
   {
      _Texture *t=(_Texture*)_t;
      _Texture *cm=(_Texture*)_cm;
      if(cm->image.screen)
         if(cm->image.screen->data.any)
            if(cm->image.screen->z==4)
               if(cm->image.screen->s.x==256)
                  if(image.screen)
                     if(image.screen->data.any)
                        if(t->image.screen)
                           if(t->image.screen->data.any)
                              if(t->image.screen->z==1)
                                 if(t->image.screen->s.x==image.screen->s.x)
                                    if(t->image.screen->s.y==image.screen->s.y)
                                    {
                                       sU16 x;
                                       sU16 y;
                                       yacmemptr s;
                                       yacmemptr d;
                                       yacmemptr c;
                                       s.u8=t->image.screen->data.u8;
                                       d.u32=image.screen->data.u32;
                                       c.u32=cm->image.screen->data.u32;
                                       switch(image.screen->z)
                                       {
                                          case 4:
                                             for(y=0; y<t->image.screen->s.y; y++)
                                             {
                                                for(x=0; x<t->image.screen->s.x; x++)
                                                {
                                                   d.u32[x]=c.u32[s.u8[x]];
                                                }
                                                d.u8+=image.screen->stride;
                                                s.u8+=t->image.screen->stride;
                                             }
                                             break;
                                          case 3:
                                          {
                                             sU32 c32;
                                             sU16 x2;
                                             for(y=0; y<t->image.screen->s.y; y++)
                                             {
                                                x2=0;
                                                for(x=0; x<t->image.screen->s.x; x++)
                                                {
                                                   c32=c.u32[s.u8[x]];
                                                   d.u8[x2++]=(sU8)(c32>>16);
                                                   d.u8[x2++]=(sU8)(c32>>8);
                                                   d.u8[x2++]=(sU8)c32;
                                                }
                                                d.u8+=image.screen->stride;
                                                s.u8+=t->image.screen->stride;
                                             }
                                          }
                                          break;
                                       }
                                    }
   }
}

void _Texture::_colorKeyToAlpha(sSI _ckey) {
   image.colorKeyToAlpha((sU32)_ckey);
}

sSI _Texture::_grayscale(void) {
   return image.grayscale();
}

void _Texture::_interleaveAlpha(YAC_Object *_t) {
   if(YAC_CHK(_t, clid_Texture))
   {
      _Texture *t=(_Texture *)_t;
      if(t->image.screen)
         image.interleaveAlpha(t->image.screen);
   }
}

void _Texture::_adjustGamma(sF32 _g) {
   if(image.screen)
   {
      if(image.screen->data.any)
      {
         sU16 x,y;
         yacmemptr d;
         switch(image.screen->z)
         {
            case 1:
               for(y=0; y<image.screen->s.y; y++)
               {
                  d.u8=image.screen->data.u8+y*image.screen->stride;
                  for(x=0; x<image.screen->s.x; x++)
                  {
                     sF32 c8=d.u8[x]*_g;
                     if(c8>255.0f)
                        c8=255.0f;
                     d.u8[x]=(sU8)c8;
                  }
               }
               break;
            case 3:
            {
               sF32 c8;
               sU16 x2;
               for(y=0; y<image.screen->s.y; y++)
               {
                  x2=0;
                  d.u8=image.screen->data.u8+y*image.screen->stride;
                  for(x=0; x<image.screen->s.x; x++, x2+=3)
                  {
                     //red
                     c8=d.u8[x2+0]*_g;
                     if(c8>255.0f) c8=255.0f;
                     d.u8[x2+0]=(sU8)c8;
                     //green
                     c8=d.u8[x2+1]*_g;
                     if(c8>255.0f) c8=255.0f;
                     d.u8[x2+1]=(sU8)c8;
                     //blue
                     c8=d.u8[x2+2]*_g;
                     if(c8>255.0f) c8=255.0f;
                     d.u8[x2+2]=(sU8)c8;
                  }
               }
            }
            break;
            case 4:
            {
               sU16 x2;
               sF32 c8;
               for(y=0; y<image.screen->s.y; y++)
               {
                  x2=0;
                  d.u8=image.screen->data.u8+y*image.screen->stride;
                  for(x=0; x<image.screen->s.x; x++, x2+=4)
                  {
                     c8=d.u8[x2+0]*_g;
                     if(c8>255.0f) c8=255.0f;
                     d.u8[x2+0]=(sU8)c8;

                     c8=d.u8[x2+1]*_g;
                     if(c8>255.0f) c8=255.0f;
                     d.u8[x2+1]=(sU8)c8;

                     c8=d.u8[x2+2]*_g;
                     if(c8>255.0f) c8=255.0f;
                     d.u8[x2+2]=(sU8)c8;

                     c8=d.u8[x2+3]*_g;
                     if(c8>255.0f) c8=255.0f;
                     d.u8[x2+3]=(sU8)c8;
                  }
               }
            }
            break;
         }
      }
   }
}


void _Texture::_adjustBrightness(sF32 _g) {

   if(image.screen)
   {
      if(image.screen->data.any)
      {
         if(_g>1.0f) _g=1.0f;
         else if(_g<0.0f) _g=0.0f;

         sU16 x,y;
         yacmemptr d;
         sF32 c8;
         switch(image.screen->z)
         {
            case 1:
               for(y=0; y<image.screen->s.y; y++)
               {
                  d.u8=image.screen->data.u8+y*image.screen->stride;
                  for(x=0; x<image.screen->s.x; x++)
                  {
                     c8=(255-d.u8[x])*_g;
                     d.u8[x]+=(sU8)c8;
                  }
               }
               break;
            case 3:
            {
               sF32 c8;
               sU16 x2;
               for(y=0; y<image.screen->s.y; y++)
               {
                  x2=0;
                  d.u8=image.screen->data.u8+y*image.screen->stride;
                  for(x=0; x<image.screen->s.x; x++, x2+=3)
                  {
                     //red
                     c8=(255-d.u8[x2+0])*_g;
                     d.u8[x2+0]+=(sU8)c8;
                     //green
                     c8=(255-d.u8[x2+1])*_g;
                     d.u8[x2+1]+=(sU8)c8;
                     //blue
                     c8=(255-d.u8[x2+2])*_g;
                     d.u8[x2+2]+=(sU8)c8;
                  }
               }
            }
            break;
            case 4:
            {
               sU16 x2;
               sF32 c8;
               for(y=0; y<image.screen->s.y; y++)
               {
                  x2=0;
                  d.u8=image.screen->data.u8+y*image.screen->stride;
                  for(x=0; x<image.screen->s.x; x++, x2+=4)
                  {
                     c8=(255-d.u8[x2+0])*_g;
                     d.u8[x2+0]+=(sU8)c8;

                     c8=(255-d.u8[x2+1])*_g;
                     d.u8[x2+1]+=(sU8)c8;

                     c8=(255-d.u8[x2+2])*_g;
                     d.u8[x2+2]+=(sU8)c8;

                     c8=(255-d.u8[x2+3])*_g;
                     d.u8[x2+3]+=(sU8)c8;
                  }
               }
            }
            break;
         }
      }
   }
}

void _Texture::_blend(YAC_Object *_srca, YAC_Object *_srcb, sSI _ratio) {
   if(YAC_CHK(_srca, clid_Texture))
      if(YAC_CHK(_srcb, clid_Texture))
      {
         _Texture *dsrca=(_Texture*)_srca;
         _Texture *dsrcb=(_Texture*)_srcb;

         if(_ratio>255)_ratio=255; else if(_ratio<0)_ratio=0;
         sF32 p=(_ratio/128.0f)-1;
         p=((1.0f-p)/2.0f);
         sU16 ra=(sU16)(255*p);
         sU16 rb=(sU16)(255*(1.0f-p));

         if(image.screen)
            if(dsrca->image.screen)
               if(dsrcb->image.screen)
                  if(image.screen->data.any)
                     if(dsrca->image.screen->data.any)
                        if(dsrcb->image.screen->data.any)
                        {
                           if((image.screen->z==dsrca->image.screen->z)&&(image.screen->z==dsrcb->image.screen->z))
                              if((image.screen->s.x==dsrca->image.screen->s.x)&&(image.screen->s.y==dsrca->image.screen->s.y))
                                 if((image.screen->s.x==dsrcb->image.screen->s.x)&&(image.screen->s.y==dsrcb->image.screen->s.y))
                                 {
                                    yacmemptr sa;
                                    yacmemptr sb;
                                    yacmemptr d;
                                    sa.u8=dsrca->image.screen->data.u8;
                                    sb.u8=dsrcb->image.screen->data.u8;
                                    d.u8=image.screen->data.u8;
                                    sU16 x;
                                    sU16 y;
                                    switch(image.screen->z)
                                    {
                                       case 1:
                                       {
                                          sU16 k;
                                          for(y=0; y<image.screen->s.y; y++)
                                          {
                                             k=0;
                                             for(x=0; x<image.screen->s.x>>3; x++)
                                             {
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                             }
                                             d.u8+=image.screen->stride;
                                             sa.u8+=dsrca->image.screen->stride;
                                             sb.u8+=dsrcb->image.screen->stride;
                                          }
                                       }
                                       break;
                                       case 2:
                                          break;
                                       case 3:
                                       {
                                          sU16 k;
                                          for(y=0; y<image.screen->s.y; y++)
                                          {
                                             k=0;
                                             for(x=0; x<image.screen->s.x>>2; x++)
                                             {
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                             }
                                             d.u8+=image.screen->stride;
                                             sa.u8+=dsrca->image.screen->stride;
                                             sb.u8+=dsrcb->image.screen->stride;
                                          }
                                       }
                                       break;
                                       case 4:
                                       {
                                          sU16 k;
                                          for(y=0; y<image.screen->s.y; y++)
                                          {
                                             k=0;
                                             for(x=0; x<image.screen->s.x; x++)
                                             {
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                                d.u8[k]=(sU8)(((((sUI)sa.u8[k])*ra)+(((sUI)sb.u8[k])*rb))>>9);
                                                k++;
                                             }
                                             d.u8+=image.screen->stride;
                                             sa.u8+=dsrca->image.screen->stride;
                                             sb.u8+=dsrcb->image.screen->stride;
                                          }
                                       }
                                       break;
                                    }
                                 }
                        }
      }
}

sF32 _Texture::_countPixels(sSI _ckey) {
   if(image.screen)
      return image.countPixels((sU32)_ckey);
   return 0.0f;
}

void _Texture::_multiply(YAC_Object *_srca, YAC_Object *_srcb) {
   if(YAC_CHK(_srca, clid_Texture))
      if(YAC_CHK(_srcb, clid_Texture))
      {
         _Texture *dsrca=(_Texture*)_srca;
         _Texture *dsrcb=(_Texture*)_srcb;
         if(image.screen)
            if(dsrca->image.screen)
               if(dsrcb->image.screen)
                  if(image.screen->data.any)
                     if(dsrca->image.screen->data.any)
                        if(dsrcb->image.screen->data.any)
                        {
                           if((image.screen->z==dsrca->image.screen->z)&&(image.screen->z==dsrcb->image.screen->z))
                              if((image.screen->s.x==dsrca->image.screen->s.x)&&(image.screen->s.y==dsrca->image.screen->s.y))
                                 if((image.screen->s.x==dsrcb->image.screen->s.x)&&(image.screen->s.y==dsrcb->image.screen->s.y))
                                 {

                                    yacmemptr sa;
                                    yacmemptr sb;
                                    yacmemptr d;
                                    sa.u8=dsrca->image.screen->data.u8;
                                    sb.u8=dsrcb->image.screen->data.u8;
                                    d.u8=image.screen->data.u8;
                                    sU16 x;
                                    sU16 y;
                                    switch(image.screen->z)
                                    {
                                       case 1:
                                       {
                                          // ---- LUMINANCE, ALPHA ----
                                          for(y=0; y<image.screen->s.y; y++)
                                          {
                                             sU16 k=0;
                                             for(x=0; x<image.screen->s.x>>2; x++)
                                             {
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                             }
                                             d.u8+=image.screen->stride;
                                             sa.u8+=dsrca->image.screen->stride;
                                             sb.u8+=dsrcb->image.screen->stride;
                                          }
                                       }
                                       break;
                                       case 2:
                                          break;
                                       case 3:
                                       {
                                          // ---- RGB ----
                                          for(y=0; y<image.screen->s.y; y++)
                                          {
                                             sU16 k=0;
                                             for(x=0; x<image.screen->s.x>>1; x++)
                                             {
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;

                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                             }
                                             d.u8+=image.screen->stride;
                                             sa.u8+=dsrca->image.screen->stride;
                                             sb.u8+=dsrcb->image.screen->stride;
                                          }
                                       }
                                       break;
                                       case 4:
                                       {
                                          // ---- RGBA, ABGR ----
                                          for(y=0; y<image.screen->s.y; y++)
                                          {
                                             sU16 k=0;
                                             for(x=0; x<image.screen->s.x>>1; x++)
                                             {
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;

                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                                d.u8[k]=(((sU16)sa.u8[k])*sb.u8[k])>>8;
                                                k++;
                                             }
                                             d.u8+=image.screen->stride;
                                             sa.u8+=dsrca->image.screen->stride;
                                             sb.u8+=dsrcb->image.screen->stride;
                                          }
                                       }
                                       break;
                                    }
                                 }
                        }
      }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ TextureModPrim                                                 ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sUI _Texture::_getXYRaw(sUI _x, sUI _y) {
   Bitmap *scr = NULL;
   scr = image.screen;
   if(NULL != scr)
   {
      if(_x < sUI(scr->s.x) && _y < sUI(scr->s.y))
      {
         yacmemptr s;
         s.u8 = scr->data.u8+scr->stride*_y+scr->z*_x;
         switch(scr->z)
         {
            case 1:
               // luminance
               return (sUI)s.u8[0];
            case 2:
               // 16bit
            {
               sU16 w565 = (sU16)s.u16[0];
               return (sUI)w565;
            }
            case 3:
               // RGB
               return
                  (s.u8[0]<<16) |
                  (s.u8[1]<< 8) |
                  (s.u8[2]    ) ;
            case 4:
               // ARGB
               return s.u32[0];
         }
      }
   }
   return 0u;
}

void _Texture::_setXYRaw(sUI _x, sUI _y, sUI _v) {
   Bitmap *scr = NULL;
   scr = image.screen;
   if(NULL != scr)
   {
      if(_x < sUI(scr->s.x) && _y < sUI(scr->s.y))
      {
         yacmemptr s;
         s.u8 = scr->data.u8+scr->stride*_y+scr->z*_x;
         switch(scr->z)
         {
            case 1:
               // luminance
               s.u8[0] = (sU8)_v;
               break;

            case 2:
               // 16bit
               s.u16[0] = _v;
               break;

            case 3:
               // RGB
               s.u8[0] = (_v>>16)&0xFF;
               s.u8[1] = (_v>> 8)&0xFF;
               s.u8[2] = (_v    )&0xFF;
               break;

            case 4:
               // ARGB
               s.u32[0] = _v;
               break;
         }
      }
   }
}

sUI _Texture::_getXY32(sSI _x, sSI _y) {
   Bitmap *scr=0;
   scr=image.screen;
   if(scr)
   {
      if(_x>=0&&_x<scr->s.x&&
         _y>=0&&_y<scr->s.y)
      {
         yacmemptr s;
         s.u8=scr->data.u8+scr->stride*_y+scr->z*_x;
         switch(scr->z)
         {
            case 1:
               // luminance
               return (sUI)s.u8[0];
            case 2:
               // 16bit
            {
               sU16 w565=(sU16)s.u16[0];
               return ((((w565>>11))<<3)<<16)|
                  ((((w565>> 5)&63)<<2)<<8)|
                  ((((w565    )&31)<<3)   );
            }
            case 3:
               // RGB
               return (s.u8[0]<<16)|
                  (s.u8[1]<< 8)|
                  (s.u8[2]    );
            case 4:
               // ARGB
               return s.u32[0];
         }
      }
   }
   return 0u;
}

void _Texture::_setXY32(sSI _x, sSI _y, sSI _v) {
   Bitmap *scr=0;
   scr=image.screen;
   if(scr)
   {
      if(_x>=0&&_x<scr->s.x&&
         _y>=0&&_y<scr->s.y)
      {
         yacmemptr s;
         s.u8=scr->data.u8+scr->stride*_y+scr->z*_x;
         switch(scr->z)
         {
            case 1:
               // luminance
               s.u8[0]=(sU8)_v;
               break;
            case 2:
               // 16bit
            {
               s.u16[0]=
                  ((((_v>>16)&0xFF)>>3)<<11)|
                  ((((_v>> 8)&0xFF)>>2)<<5)|
                  ((((_v    )&0xFF)>>3)   );
            }
            break;
            case 3:
               // RGB
               s.u8[0]=(_v>>16)&0xFF;
               s.u8[1]=(_v>> 8)&0xFF;
               s.u8[2]=(_v    )&0xFF;
               break;
            case 4:
               // ARGB
               s.u32[0]=_v;
               break;
         }
      }
   }
}

#define Dc32torgbaf(v,a) \
   (a)[0] = (((v) >> 16) & 255u) * (1.0f / 255.0f); \
   (a)[1] = (((v) >>  8) & 255u) * (1.0f / 255.0f); \
   (a)[2] = (((v)      ) & 255u) * (1.0f / 255.0f); \
   (a)[3] = (((v) >> 24) & 255u) * (1.0f / 255.0f)


void _Texture::_getUVVec4(sF32 _u, sF32 _v, YAC_Object *_retVec4) {
   if(YAC_VALID(_retVec4))
   {
      if((NULL != image.screen) && (image.screen->s.x > 0))
      {
         yacmemptr d; d.any = _retVec4->yacArrayGetPointer();
         if((4u == _retVec4->yacArrayGetElementByteSize()) && (_retVec4->yacArrayGetMaxElements() >= 4u))
         {
            _u = sRANGE(_u, 0.0f, 1.0f);
            _v = sRANGE(_v, 0.0f, 1.0f);
            sSI x = sSI((image.screen->s.x-1) * _u);
            sSI y = sSI((image.screen->s.y-1) * _v);
            sUI c32 = _getXY32(x, y);
            Dc32torgbaf(c32, d.f32);
         }
      }
   }
}

void _Texture::_getUVFilterVec4(sF32 _u, sF32 _v, YAC_Object *_retVec4) {
   if(YAC_VALID(_retVec4))
   {
      if((NULL != image.screen) && (image.screen->s.x > 0))
      {
         yacmemptr d; d.any = _retVec4->yacArrayGetPointer();
         if((4u == _retVec4->yacArrayGetElementByteSize()) && (_retVec4->yacArrayGetMaxElements() >= 4u))
         {
            _u = sRANGE(_u, 0.0f, 1.0f);
            _v = sRANGE(_v, 0.0f, 1.0f);
            sF32 fx = (image.screen->s.x-1) * _u;
            sF32 fy = (image.screen->s.y-1) * _v;
            sSI x = sSI(fx);
            sSI y = sSI(fy);
            sF32 fracX = fx - sF32(x);
            sF32 fracY = fy - sF32(y);
            // yac_host->printf("xxx Texture::getUVFilterVec4: uv=(%f; %f) => i=(%d; %d) frac=(%f; %f)\n", _u, _v, x, y, fracX, fracY);
            sUI c32LT = _getXY32(x, y);
            sUI c32RT = ((x+1) < image.screen->s.x) ? _getXY32(x+1, y) : c32LT;
            sUI c32LB;
            sUI c32RB;
            if((y+1) < image.screen->s.y)
            {
               c32LB = _getXY32(x, y+1);
               c32RB = ((x+1) < image.screen->s.x) ? _getXY32(x+1, y+1) : c32LB;
            }
            else
            {
               c32LB = c32LT;
               c32RB = c32RT;
            }
            sF32 lt[4];
            sF32 rt[4];
            sF32 rb[4];
            sF32 lb[4];
            Dc32torgbaf(c32LT, lt);
            Dc32torgbaf(c32RT, rt);
            Dc32torgbaf(c32RB, rb);
            Dc32torgbaf(c32LB, lb);
            sF32 tr = lt[0] + (rt[0] - lt[0]) * fracX;
            sF32 br = lb[0] + (rb[0] - lb[0]) * fracX;
            sF32 tg = lt[1] + (rt[1] - lt[1]) * fracX;
            sF32 bg = lb[1] + (rb[1] - lb[1]) * fracX;
            sF32 tb = lt[2] + (rt[2] - lt[2]) * fracX;
            sF32 bb = lb[2] + (rb[2] - lb[2]) * fracX;
            sF32 ta = lt[3] + (rt[3] - lt[3]) * fracX;
            sF32 ba = lb[3] + (rb[3] - lb[3]) * fracX;
            d.f32[0] = tr + (br - tr) * fracY;
            d.f32[1] = tg + (bg - tg) * fracY;
            d.f32[2] = tb + (bb - tb) * fracY;
            d.f32[3] = ta + (ba - ta) * fracY;
         }
      }
   }
}

void _Texture::_getUVRepeatVec4(sF32 _u, sF32 _v, YAC_Object *_retVec4) {
   if(YAC_VALID(_retVec4))
   {
      if((NULL != image.screen) && (image.screen->s.x > 0))
      {
         yacmemptr d; d.any = _retVec4->yacArrayGetPointer();
         if((4u == _retVec4->yacArrayGetElementByteSize()) && (_retVec4->yacArrayGetMaxElements() >= 4u))
         {
            sSI x = sSI((image.screen->s.x) * _u);
            sSI y = sSI((image.screen->s.y) * _v);
            x = x % image.screen->s.x;
            y = y % image.screen->s.y;
            sUI c32 = _getXY32(x, y);
            Dc32torgbaf(c32, d.f32);
         }
      }
   }
}

void _Texture::_getUVFilterRepeatVec4(sF32 _u, sF32 _v, YAC_Object *_retVec4) {
   if(YAC_VALID(_retVec4))
   {
      if((NULL != image.screen) && (image.screen->s.x > 0))
      {
         yacmemptr d; d.any = _retVec4->yacArrayGetPointer();
         if((4u == _retVec4->yacArrayGetElementByteSize()) && (_retVec4->yacArrayGetMaxElements() >= 4u))
         {
            sF32 fx = (image.screen->s.x) * _u;
            sF32 fy = (image.screen->s.y) * _v;
            sSI x = sSI(fx);
            sSI y = sSI(fy);
            sF32 fracX = fx - sF32(x);
            sF32 fracY = fy - sF32(y);
            x = x % image.screen->s.x;
            y = y % image.screen->s.y;
            // yac_host->printf("xxx Texture::getUVFilterRepeatVec4: uv=(%f; %f) => i=(%d; %d) frac=(%f; %f)\n", _u, _v, x, y, fracX, fracY);
            sUI c32LT = _getXY32(x, y);
            sUI c32RT = _getXY32((x+1) % image.screen->s.x, y);
            sUI c32LB = _getXY32(x, (y+1) % image.screen->s.y);
            sUI c32RB = _getXY32((x+1) % image.screen->s.x, (y+1) % image.screen->s.y);
            sF32 lt[4];
            sF32 rt[4];
            sF32 rb[4];
            sF32 lb[4];
            Dc32torgbaf(c32LT, lt);
            Dc32torgbaf(c32RT, rt);
            Dc32torgbaf(c32RB, rb);
            Dc32torgbaf(c32LB, lb);
            sF32 tr = lt[0] + (rt[0] - lt[0]) * fracX;
            sF32 br = lb[0] + (rb[0] - lb[0]) * fracX;
            sF32 tg = lt[1] + (rt[1] - lt[1]) * fracX;
            sF32 bg = lb[1] + (rb[1] - lb[1]) * fracX;
            sF32 tb = lt[2] + (rt[2] - lt[2]) * fracX;
            sF32 bb = lb[2] + (rb[2] - lb[2]) * fracX;
            sF32 ta = lt[3] + (rt[3] - lt[3]) * fracX;
            sF32 ba = lb[3] + (rb[3] - lb[3]) * fracX;
            d.f32[0] = tr + (br - tr) * fracY;
            d.f32[1] = tg + (bg - tg) * fracY;
            d.f32[2] = tb + (bb - tb) * fracY;
            d.f32[3] = ta + (ba - ta) * fracY;
         }
      }
   }
}

static sF32 loc_read_float_from_array(YAC_Object *_v, sUI _idx) {
   YAC_Value r;
   _v->yacArrayGet(NULL/*context*/, _idx, &r);
   switch(r.type)
   {
      case YAC_TYPE_INT:
         return (sF32)r.value.int_val;

      case YAC_TYPE_FLOAT:
         return r.value.float_val;

      case YAC_TYPE_OBJECT:
      case YAC_TYPE_STRING:
      {
         // e.g. number object (or a string)
         sF32 f = 0.0f;
         (void)_v->yacScanF32(&f);
         r.unsetFast();
         return f;
      }
   }
   return 0.0f;
}

void _Texture::_drawTriAA(YAC_Object *_v1, YAC_Object *_v2, YAC_Object *_v3, sUI _c32, sUI _numSamples, sUI _aaFixMode, sF32 _aaBias) {
   if((NULL != image.screen) && (4u == image.screen->z) && (image.screen->s.x > 0))
   {
      if(YAC_VALID(_v1) &&
         YAC_VALID(_v2) &&
         YAC_VALID(_v3)
         )
      {
         TriRasterAAVec2f v1;
         v1.x = loc_read_float_from_array(_v1, 0u);
         v1.y = loc_read_float_from_array(_v1, 1u);

         TriRasterAAVec2f v2;
         v2.x = loc_read_float_from_array(_v2, 0u);
         v2.y = loc_read_float_from_array(_v2, 1u);

         TriRasterAAVec2f v3;
         v3.x = loc_read_float_from_array(_v3, 0u);
         v3.y = loc_read_float_from_array(_v3, 1u);

         TriRasterAA raster;
         raster.renderTriToBitmap(v1, v2, v3, _c32, image.screen, _numSamples, _aaFixMode, _aaBias);
      }
   }
}

void _Texture::_drawTriAA_fx(YAC_Object *_v1, YAC_Object *_v2, YAC_Object *_v3, sUI _c32, sUI _numSamples, sUI _aaFixMode, sS32 _aaBias) {
   // fixpoint rasterizer test
   if((NULL != image.screen) && (4u == image.screen->z) && (image.screen->s.x > 0))
   {
      if(YAC_VALID(_v1) &&
         YAC_VALID(_v2) &&
         YAC_VALID(_v3)
         )
      {
         TriRasterAAVec2x v1;
         v1.x = sSI(loc_read_float_from_array(_v1, 0u) * FX_COORD_SCL_AA);
         v1.y = sSI(loc_read_float_from_array(_v1, 1u) * FX_COORD_SCL_AA);

         TriRasterAAVec2x v2;
         v2.x = sSI(loc_read_float_from_array(_v2, 0u) * FX_COORD_SCL_AA);
         v2.y = sSI(loc_read_float_from_array(_v2, 1u) * FX_COORD_SCL_AA);

         TriRasterAAVec2x v3;
         v3.x = sSI(loc_read_float_from_array(_v3, 0u) * FX_COORD_SCL_AA);
         v3.y = sSI(loc_read_float_from_array(_v3, 1u) * FX_COORD_SCL_AA);

         TriRasterAA_fx raster;
         raster.renderTriToBitmap(v1, v2, v3, _c32, image.screen, _numSamples, _aaFixMode, _aaBias);
      }
   }
}

void _Texture::applyClearColorAfterDrawTriAA(sUI _c24) {
   // Apply clear color after drawTriAA()
   //   (note) fb byte order: B,G,R,A (ARGB32)

// #define CLR_DEBUG defined

   if((NULL != image.screen) && (4u == image.screen->z) && (image.screen->s.x > 0))
   {
      const sU8 dr = (_c24 >> 16) & 255u;
      const sU8 dg = (_c24 >>  8) & 255u;
      const sU8 db = (_c24      ) & 255u;
      sU8 *dRow = image.screen->data.u8;
#ifdef CLR_DEBUG
      int debugLeft = 30;
#endif // CLR_DEBUG
      for(sSI y = 0; y < image.screen->s.y; y++)
      {
         sU8 *d = dRow;
         for(sSI x = 0; x < image.screen->s.x; x++)
         {
            const sU8 as = d[3];
#if 1
            // (GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
            const sU8 ad = 255u - as;
#endif
            if(as < 255u)
            {
               const sU8 rs = d[2];
               const sU8 gs = d[1];
               const sU8 bs = d[0];
               d[3] = 255u;
#if 1
               // (GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
               d[2] = rs + ((dr * ad) >> 8);
               d[1] = gs + ((dg * ad) >> 8);
               d[0] = bs + ((db * ad) >> 8);
#else
               // old (bad AA): (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
               d[2] = dr + (((rs - dr) * as) >> 8);
               d[1] = dg + (((gs - dg) * as) >> 8);
               d[0] = db + (((bs - db) * as) >> 8);
#endif // 1
#ifdef CLR_DEBUG
               if(debugLeft && as > 0)
               {
                  debugLeft--;
                  printf("xxx src=#%02x%02x%02x d=#%02x%02x%02x as=%u => #%02x%02x%02x\n", rs,gs,bs, dr,dg,db, as, d[2],d[1],d[0]);
               }
#endif // CLR_DEBUG
            }
            d += sizeof(sU32);
         }
         dRow += image.screen->stride;
      }
   }
#undef CLR_DEBUG
}

void _Texture::applyClearColorAfterDrawTriAA4444(sUI _c24) {
   // Apply clear color after drawTriAA() (in AAFX_4444 mode)
   //   (note) fb byte order: B,G,R,A (#0A0R0G0B stored in ARGB32 dwords)
   //   (note) c32 has #00RRGGBB format

   if((NULL != image.screen) && (4u == image.screen->z) && (image.screen->s.x > 0))
   {
      const sU8 dr = (_c24 >> 16) & 255u;
      const sU8 dg = (_c24 >>  8) & 255u;
      const sU8 db = (_c24      ) & 255u;
      sU8 *dRow = image.screen->data.u8;
      for(sSI y = 0; y < image.screen->s.y; y++)
      {
         sU8 *d = dRow;
         for(sSI x = 0; x < image.screen->s.x; x++)
         {
            sU8 as = d[3];
            d[3] = 255u;
            // expand 4444 to 8888
            as = (as | (as << 4));
            sU8 rs = d[2];
            rs = (rs | (rs << 4));
            sU8 gs = d[1];
            gs = (gs | (gs << 4));
            sU8 bs = d[0];
            bs = (bs | (bs << 4));
            if(as < 15u)
            {
               d[2] = dr + (((rs - dr) * as) >> 8);
               d[1] = dg + (((gs - dg) * as) >> 8);
               d[0] = db + (((bs - db) * as) >> 8);
            }
            else
            {
               d[2] = rs;
               d[1] = gs;
               d[0] = bs;
            }
            d += sizeof(sU32);
         }
         dRow += image.screen->stride;
      }
   }
}

void _Texture::_drawTri_fx(YAC_Object *_v1, YAC_Object *_v2, YAC_Object *_v3, sUI _c32, sUI _c32Mask) {
   // non-antialised fixed point rasterizer (see tri_hspan.h)
   if((NULL != image.screen) && (4u == image.screen->z) && (image.screen->s.x > 0))
   {
      if(YAC_VALID(_v1) &&
         YAC_VALID(_v2) &&
         YAC_VALID(_v3)
         )
      {
         sU32 va[2*3];

         va[0*2+0] = sSI(loc_read_float_from_array(_v1, 0u) * FX_COORD_SCL_HSPAN);
         va[0*2+1] = sSI(loc_read_float_from_array(_v1, 1u) * FX_COORD_SCL_HSPAN);

         va[1*2+0] = sSI(loc_read_float_from_array(_v2, 0u) * FX_COORD_SCL_HSPAN);
         va[1*2+1] = sSI(loc_read_float_from_array(_v2, 1u) * FX_COORD_SCL_HSPAN);

         va[2*2+0] = sSI(loc_read_float_from_array(_v3, 0u) * FX_COORD_SCL_HSPAN);
         va[2*2+1] = sSI(loc_read_float_from_array(_v3, 1u) * FX_COORD_SCL_HSPAN);

#if 0
         // simulate 2 fractional bits (instead of 4)
         for(sUI idx = 0u; idx < (2*3); idx++)
            va[idx] = va[idx] & ~3u;
#endif

         draw_tri_hspan(image.screen->data.u32,
                        image.screen->stride>>2/*fbW=bytes to c32 px*/,
                        image.screen->s.y/*fbH*/,
                        va,
                        _c32,
                        _c32Mask
                        );
      }
   }
}

void _Texture::_drawTriTex_fx(YAC_Object *_tex,
                              YAC_Object *_v1, YAC_Object *_v2, YAC_Object *_v3,
                              YAC_Object *_uv1, YAC_Object *_uv2, YAC_Object *_uv3,
                              sF32 _a
                              ) {
   // non-antialised, texture-mapped fixed point rasterizer (see tri_tex_fx.h)
   if( (NULL != image.screen) && (4u == image.screen->z) && (image.screen->s.x > 0) )
   {
      if(YAC_VALID(_tex) &&
         YAC_VALID(_v1) && YAC_VALID(_v2) && YAC_VALID(_v3) &&
         YAC_VALID(_uv1) && YAC_VALID(_uv2) && YAC_VALID(_uv3)
         )
      {
         const sU32 *tex = (const sU32 *)_tex->yacArrayGetPointer();
         if(NULL != tex && (4 == _tex->yacArrayGetElementByteSize()))
         {
            const sUI texW = _tex->yacArrayGetWidth();
            const sUI texH = _tex->yacArrayGetHeight();

            tritexvec2_t va[3];
            tritexvec2_t uv[3];

            // (note) will be converted to fixed point in the rasterizer prologue
            va[0].x = loc_read_float_from_array(_v1, 0u);
            va[0].y = loc_read_float_from_array(_v1, 1u);

            va[1].x = loc_read_float_from_array(_v2, 0u);
            va[1].y = loc_read_float_from_array(_v2, 1u);

            va[2].x = loc_read_float_from_array(_v3, 0u);
            va[2].y = loc_read_float_from_array(_v3, 1u);

            // UV
            uv[0].x = loc_read_float_from_array(_uv1, 0u);
            uv[0].y = loc_read_float_from_array(_uv1, 1u);

            uv[1].x = loc_read_float_from_array(_uv2, 0u);
            uv[1].y = loc_read_float_from_array(_uv2, 1u);

            uv[2].x = loc_read_float_from_array(_uv3, 0u);
            uv[2].y = loc_read_float_from_array(_uv3, 1u);

            TriTexFx tt;
            tritexvec2_t vaSorted[3];
            TriTexFx::SortVertices(va[0], va[1], va[2],
                                   vaSorted[0], vaSorted[1], vaSorted[2]
                                   );

            tt.tritex_dda_fix_opt(image.screen->data.u32, image.screen->s.x, image.screen->s.y,
                                  tex, texW, texH,
                                  vaSorted[0], vaSorted[1], vaSorted[2],
                                  va[0], va[1], va[2],
                                  uv[0], uv[1], uv[2],
                                  sU32(_a * 255)
                                  );
         } // if tex data
      } // if tex+vertices
   } // if fb
}

void _Texture::_drawPolygon(YAC_Object *_intarray, sSI _color) {
   if(YAC_BCHK(_intarray, YAC_CLID_INTARRAY))
   {
      YAC_IntArray *ia=(YAC_IntArray*)_intarray;
      image.drawFilledConvexPoly(ia->num_elements/2, ia->elements, (sU32)_color);
   }
}

void _Texture::_drawLine(sSI _qx, sSI _qy, sSI _px, sSI _py, sUI _color) {
   image.drawLine(_qx, _qy, _px, _py, _color);
}

void _Texture::_drawFilledBox(sSI _px, sSI _py, sSI _sx, sSI _sy, sUI _color) {
   image.drawFilledRect(_px, _py, _sx, _sy, _color);
}

void _Texture::_createMask(sSI _colorkey) {
   sUI ckey=(sUI)(_colorkey&0xFFFFFF);
   if(image.screen)
      if(image.screen->data.any)
      {
         sSI x,y;
         sUI str=image.screen->stride;
         yacmemptr s; s.any=image.screen->data.any;
         switch(image.screen->z)
         {
            case 1:
               for(y=0; y<image.screen->s.y; y++)
               {

                  for(x=0; x<image.screen->s.x; x++)
                  {
                     s.u8[x]=(s.u8[x]==(sU8)ckey)?0xFF:0;
                  }
                  s.u8+=str;
               }
               break;
            case 4:
               for(y=0; y<image.screen->s.y; y++)
               {
                  for(x=0; x<image.screen->s.x; x++)
                     s.u32[x]=((s.u32[x]&0xFFFFFF)==ckey)?0xFFFFFFff:0;
                  s.u8+=str;
               }
               break;
         }
      }
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                                                ~
//~ TextureModTransform                                            ~
//~                                                                ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void _Texture::_scroll(sSI _dx, sSI _dy) {
   image.scroll(_dx, _dy);
}

void _Texture::_expand2n(void) {
   Bitmap *scr=0;
   scr=image.screen;
   if(scr)
   {
      tksvec2i s;
      s.x=scr->s.x;
      s.y=scr->s.y;
      _Texture::Expand2n(&s.x, &s.y);
      if((s.x!=scr->s.x)||(s.y!=scr->s.y))
      {
         /** resize window to match GL 2^n texture size requirements **/
         Image tmp;
         tmp.allocScreen(s, scr->z);
         tmp.clearScreen();
         tmp.copyRegion(&image, 0, 0, scr->s.x, scr->s.y, 0, 0);
         tmp.ownscreen=0;
         image.setScreen(tmp.screen);
         image.ownscreen=1;
         image.sel.s.x=image.screen->s.x;
         image.sel.s.y=image.screen->s.y;
         image.sel.p.x=image.sel.p.y=0;
      }
   }
}


static sU32 loc_blend_32_8(const sU32 _d, const sU32 _s, const sU8 _a) {
   // Drgb = Drgb*(1-a) + Srgb*a
   // keep dest alpha
   sU16 a = _a + (_a & 1u);

   // sU8 da = (_d >> 24) & 255u;
   sU8 dr = (_d >> 16) & 255u;
   sU8 dg = (_d >>  8) & 255u;
   sU8 db = (_d      ) & 255u;

   // sU8 sa = (_s >> 24) & 255u;
   // // sa = sa + (sa & 1u)
   sU8 sr = (_s >> 16) & 255u;
   sU8 sg = (_s >>  8) & 255u;
   sU8 sb = (_s      ) & 255u;

   sU16 oma = (256u - a);

   // da = (da * oma) >> 8;
   dr = (dr * oma) >> 8;
   dg = (dg * oma) >> 8;
   db = (db * oma) >> 8;

   sr = (sr * a) >> 8;
   sg = (sg * a) >> 8;
   sb = (sb * a) >> 8;

   dr += sr;
   dg += sg;
   db += sb;

   return (_d&0xFF000000u) | (dr << 16) | (dg << 8) | db;
}

// [05Oct2023] add source over blending
void _Texture::_renderFixedText(YAC_Object *_fontTex, sSI _charWidth, YAC_Object *_text, sSI _x, sSI _y,
                                YAC_Object *_colorIntArray, YAC_Object *_attrIntArray) {
   // Attr:
   //    ----Bit--+---Description-----
   //        0    | inverse
   //        1    | underline
   //
   if(YAC_CHK(_fontTex, clid_Texture))
   {
      if(YAC_BCHK(_colorIntArray, YAC_CLID_INTARRAY))
      {
         if(YAC_BCHK(_attrIntArray, YAC_CLID_INTARRAY))
         {
            if(YAC_BCHK(_text, YAC_CLID_STRING))
            {
               _Texture *fontTex = (_Texture *)_fontTex;
               if( (fontTex->image.screen->z == 1) && (image.screen->z == 4) )
               {
                  YAC_IntArray *colors = (YAC_IntArray *) _colorIntArray;
                  YAC_IntArray *attrs  = (YAC_IntArray *) _attrIntArray;
                  YAC_String *text = (YAC_String *) _text;
                  sUI cw = (sUI) _charWidth;
                  //sUI ch = (sUI) fontTex->image.screen->s.y;
                  sUI sw = image.screen->s.x;
                  sUI sh = image.screen->s.y;
                  sUI fw = fontTex->image.screen->s.x;
                  sUI fh = fontTex->image.screen->s.y;
                  sUI dx = (sUI) _x;
                  sUI dy = (sUI) _y;
                  sUI sstride = fontTex->image.screen->stride - cw;
                  sUI dstride = (image.screen->stride>>2) - cw;
                  if( (dy+fh) <= sh )
                  {
                     sUI i = (sUI) 0;
                     while( (i<text->length) && ((dx+cw)<=sw) &&
                            (i<colors->num_elements) && (i<attrs->num_elements)
                            )
                     {
                        sU8 c = text->chars[i];
                        //yac_host->printf("xxx tkstexture dx=%i\n",dx);
                        if(c > 0)
                        {
                           sU32 *d = &image.screen->data.u32[ dx + _y*(image.screen->stride>>2) ];
                           sUI c32 = (sUI) colors->elements[ i ];
                           sUI attr = (sUI) attrs->elements[ i ];

                           if(c > 32)
                           {
                              if( (((c-33)*cw)+cw) <= fw )
                              {
                                 const sU8 *s = &fontTex->image.screen->data.u8[ (c-33)*cw ];

                                 switch(attr)
                                 {
                                    default:
                                    case 0:
                                    {
                                       // Copy + modulate by color
                                       for(sUI y = 0u; y < fh; y++)
                                       {
                                          for(sUI x = 0u; x < cw; x++)
                                          {
                                             const sU8 a = *s++;
                                             if(a)
                                             {
                                                // *d++ = c32;
                                                // *d++ = (c32&0x00FFFFFFu) | (a << 24);
                                                // *d++ = 0xFF000000u | ((a << 16) | (a << 8) | a);
                                                // *d++ = (0 << 24) | (a << 16) | (a << 8) | a;
                                                *d = loc_blend_32_8(*d, c32, a); d++;
                                             }
                                             else
                                             {
                                                d++;
                                             }
                                             //sUI t = (sUI) *s++;
                                             //*d++ = ((t<<24)&c32) | ((t<<16)&c32) | ((t<<8)&c32) | (t&c32);
                                          }
                                          s += sstride;
                                          d += dstride;
                                       }
                                    }
                                    break;

                                    case 1:
                                    {
                                       // Copy inverse + modulate by color
                                       for(sUI y = 0u; y < fh; y++)
                                       {
                                          for(sUI x = 0u; x < cw; x++)
                                          {
                                             const sU8 a = *s++;
                                             // if(a)
                                             // {
                                             //    d++;
                                             // }
                                             // else
                                             // {
                                                // *d++ = c32;
                                                // *d++ = (c32&0x00FFFFFFu) | (a << 24);
                                              *d = loc_blend_32_8(*d, c32, 255u - a); d++;
                                             // }
                                             //sUI t = (sUI) 255-(*s++);
                                             //*d++ = ((t<<24)&c32) | ((t<<16)&c32) | ((t<<8)&c32) | (t&c32);
                                          }
                                          s += sstride;
                                          d += dstride;
                                       }
                                    }
                                    break;

                                    case 2:
                                    {
                                       // Copy + underline + modulate by color
                                       for(sUI y = 0u; y < (fh - 1u); y++)
                                       {
                                          for(sUI x = 0u; x < cw; x++)
                                          {
                                             const sU8 a = *s++;
                                             if(a)
                                             {
                                                // *d++ = c32;
                                                // *d++ = (c32&0x00FFFFFFu) | (a << 24);
                                                *d = loc_blend_32_8(*d, c32, a); d++;
                                             }
                                             else
                                             {
                                                d++;
                                             }
                                          }
                                          s += sstride;
                                          d += dstride;
                                       }
                                       for(sUI x = 0u; x < cw; x++)
                                       {
                                          *d++ = c32;
                                       }
                                    }
                                    break;

                                    case 3:
                                    {
                                       // Copy inverse + underline + modulate by color
                                       for(sUI y = 0u; y < (fh - 1u); y++)
                                       {
                                          for(sUI x = 0u; x < cw; x++)
                                          {
                                             const sU8 a = *s++;
                                             // if(a)
                                             // {
                                             //    d++;
                                             // }
                                             // else
                                             // {
                                                // *d++ = c32;
                                                // *d++ = (c32&0x00FFFFFFu) | (a << 24);
                                                *d = loc_blend_32_8(*d, c32, 255u - a); d++;
                                             // }
                                          }
                                          s += sstride;
                                          d += dstride;
                                       }
                                       // /*for(x=0; x<cw; x++)
                                       //   {
                                       //   *d++ = c32;
                                       //   }*/
                                    }
                                    break;
                                 } // switch attr

                              }
                           }
                           else
                           {
                              // whitespace
                              switch(attr)
                              {
                                 default:
                                 case 0:
                                 {
                                    // Copy + modulate by color
                                    // (intentionally left blank)
                                 }
                                 break;

                                 case 1: // Copy inverse + modulate by color
                                 case 3: // Copy inverse + underline + modulate by color
                                 {
                                    for(sUI y = 0u; y < fh; y++)
                                    {
                                       for(sUI x = 0u; x < cw; x++)
                                       {
                                          *d++ = c32;
                                       }
                                       d += dstride;
                                    }
                                 }
                                 break;

                                 case 2:
                                 {
                                    // Copy + underline + modulate by color
                                    d += dstride * (fh - 1u);
                                    for(sUI x = 0u; x < cw; x++)
                                    {
                                       *d++ = c32;
                                    }
                                 }
                                 break;

                              } // switch attr

                           } // else whitespace

                        } // c > 0
                        else
                        {
                           break;
                        }
                        i++;
                        dx += cw;
                     }
                  }
               }
               else
               {
                  yac_host->printf("[---] Texture::renderFixedText: bpp mismatch.\n");
               }
            }
            else
            {
               yac_host->printf("[---] Texture::renderFixedText: text missing.\n");
            }
         }
         else
         {
            yac_host->printf("[---] Texture::renderFixedText: attributes missing.\n");
         }
      }
      else
      {
         yac_host->printf("[---] Texture::renderFixedText: colors missing.\n");
      }
   }
   else
   {
      yac_host->printf("[---] Texture::renderFixedText: font missing.\n");
   }
}


void _Texture::_setMipMapLevel(sSI _level) {
   mipmap_level = (sUI) _level;
}

sSI _Texture::_getMipMapLevel(void) {
   return (sSI) mipmap_level;
}

void _Texture::_setTarget(sSI _glTextureTarget) {
   target = (sUI) _glTextureTarget;

   fixFlags(); // add CLAMPTOEDGE_*
}

sSI _Texture::_getTarget(void) {
   return (sSI) target;
}

void _Texture::_flipY(void) {
   image.flipY();
}

void _Texture::_invert(void) {
   image.invert();
}

void _Texture::_visitBuffer(YAC_Object *_buffer, sUI _w, sUI _h, sUI _bpp) {

   _freeImage();

   if(YAC_BCHK(_buffer, YAC_CLID_BUFFER))
   {
      YAC_Buffer *b = (YAC_Buffer *)_buffer;

      sUI reqSize = (_w * _h * _bpp);
      sUI availSize = b->size - b->io_offset;

      if( reqSize <= availSize)
      {
         image.visitBuffer(b->buffer + b->io_offset, _w, _h, _bpp);
      }
      else
      {
         yac_host->printf("[---] Texture::visitBuffer: buffer size is exceeded (%u * %u * %u (%u) > (%u - %u (%u)))\n",
                          _w, _h, _bpp, reqSize, b->size, b->io_offset, (b->size - b->io_offset));
      }
   }
}

void _Texture::calcAlphaSDF(_Texture *_texSDF, sSI _gbitmapx, sSI _gbitmapy, sSI _gwidth, sSI _gheight, sSI _sdfRadius, sF32 _sdfMaxDist) {
#define Dargb(a,r,g,b) ((sU8(a)<<24)|(sU8(r)<<16)|(sU8(g)<<8)|sU8(b))
   // (todo) check texture format and bounds
   sF32 y = 0.5;
   for(sSI yloopIdx = 0; yloopIdx < (_gheight + _sdfRadius*2); yloopIdx++)
   {
      sF32 x = 0.5;
      for(sSI xloopIdx = 0; xloopIdx < (_gwidth + _sdfRadius*2); xloopIdx++)
      {
         int c8 = _texSDF->_getXY32(_gbitmapx - _sdfRadius + (x),
                                    _gbitmapy - _sdfRadius + (y)
                                    );
         if(255 != c8)
         {
            sF32 minDist = 999999;
            sF32 iy = 0;
            for(sSI iyloopIdx = 0; iyloopIdx < _gheight; iyloopIdx++)
            {
               sF32 ix = 0;
               for(sSI ixloopIdx = 0; ixloopIdx < _gwidth; ixloopIdx++)
               {
                  sF32 dx = 999999, dy = 99999;
                  sSI bx = _gbitmapx + sSI(ix);
                  sSI by = _gbitmapy + sSI(iy);
                  // int c8i = texSDF.getXY32(bx, by);
                  // if(255 == c8i)
                  // {
                     int lt = (255 == _texSDF->_getXY32(bx-1, by-1));
                     int t  = (255 == _texSDF->_getXY32(bx,   by-1));
                     int rt = (255 == _texSDF->_getXY32(bx+1, by-1));
                     int l  = (255 == _texSDF->_getXY32(bx-1, by));
                     int c  = (255 == _texSDF->_getXY32(bx,   by));
                     int r  = (255 == _texSDF->_getXY32(bx+1, by));
                     int lb = (255 == _texSDF->_getXY32(bx-1, by+1));
                     int b  = (255 == _texSDF->_getXY32(bx,   by+1));
                     int rb = (255 == _texSDF->_getXY32(bx+1, by+1));

                     // 876
                     // 5x3
                     // 210
                     int m = (lt << 8) | (t << 7) | (rt << 6) | (l << 5) | (c << 4) | (r << 3) | (lb << 2) || (b << 1) | (rb);
                     sF32 aix = ix + _sdfRadius;
                     sF32 aiy = iy + _sdfRadius;

                     switch(m)
                     {
                        // 0x0
                        // 00x
                        // 000
                        //
                        // 0x0
                        // x00
                        // 000
                        //
                        // 000
                        // x00
                        // 0x0
                        //
                        // 000
                        // 00x
                        // 0x0
                        case 0b010001000:
                        case 0b010100000:
                        case 0b000100010:
                        case 0b000001010:
                           dx = (ix + 0.5);
                           dy = (iy + 0.5);
                           break;

                           // 0x0
                           // 0x0
                           // 0x0
                        case 0b010010010:
                           if(x < aix)
                           {
                              dx = (ix      );
                              dy = (iy + 0.5);
                           }
                           else
                           {
                              dx = ix + 1.0;
                              dy = iy + 0.5;
                           }  
                           break;

                           // 000
                           // xxx
                           // 000
                        case 0b000111000:
                           if(y < aiy)
                           {
                              dx = (ix + 0.5);
                              dy = (iy      );
                           }
                           else
                           {
                              dx = ix + 0.5;
                              dy = iy + 1.0;
                           }  
                           break;

                        default:
                           if(c)
                           {
                              dx = ix + 0.5;
                              dy = iy + 0.5;
                           }
                           break;
                     }

                     dx = dx + _sdfRadius - x - 0.5;
                     dy = dy + _sdfRadius - y - 0.5;
                     // trace "xxx d=("+dx+";"+dy+")";
                     float d = sqrtf(dx*dx + dy*dy);
                     if(d < minDist)
                     {
                        minDist = d;
                        // minDist = mathMaxf(dx, dy);
                     }
                  // }
                  ix++;
               }
               iy++;
            }
            if(minDist < 999)
            {
               // trace "xxx p=("+x+","+y+") minDist="+minDist+"/"+sdfMaxDist;
               minDist = 254 - ( (minDist * 254) / _sdfMaxDist );
               minDist = sRANGE(minDist, 0, 254);
               _texSDF->_setXY32(_gbitmapx - _sdfRadius + int(x),
                                 _gbitmapy - _sdfRadius + int(y),
                                 Dargb(minDist, minDist, minDist, minDist)
                                 );
            }
            else
            {
               _texSDF->_setXY32(_gbitmapx - _sdfRadius + (x),
                                 _gbitmapy - _sdfRadius + (y),
                                 Dargb(minDist, minDist, minDist, 0)
                                 );
            }
         }
         x++;
      }
      y++;
   }
}

