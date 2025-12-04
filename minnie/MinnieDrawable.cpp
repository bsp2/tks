// ----
// ---- file   : MinnieDrawable.cpp
// ---- author : bsp
// ---- legal  : Distributed under terms of the MIT LICENSE.
// ----
// ---- info   : "minnie" API
// ---- note   :
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "inc_yac.h"

#define MINNIE_SKIP_TYPEDEFS defined
#include "minnie.h"
#include "MinnieDrawable.h"

#include "inc_opengl.h"

#include "shadervg/shadervg.h"
#include "shadervg/shadervg_internal.h"
#include "shadervg/Shader.h"
#include "shadervg/Shape.h"

#ifndef MINNIE_LIB
#include "../tkopengl/tkopengl_shared.h"
#endif


// ---------------------------------------------------------------------------- MinnieDrawable
_MinnieDrawable::_MinnieDrawable(void) {
   buf_gl = NULL;
   gl_buf_id = 0u;
   b_gl_buf_update_pending = YAC_FALSE;

   buf_draw = NULL;
   c32_bg_color = 0u;

   size_x = 640.0f;
   size_y = 480.0f;

   scale_x = 1.0f;
   scale_y = 1.0f;
   rot_ang = 0.0f;
   trans_x = 0.0f;
   trans_y = 0.0f;
}

_MinnieDrawable::~_MinnieDrawable() {
   free();
}

void _MinnieDrawable::init(void) {
}

sBool _MinnieDrawable::alloc(sUI _maxGLBufSize, sUI _maxDrawBufSize) {
   sBool ret = YAC_FALSE;

   if(_maxGLBufSize < 4096)
      _maxGLBufSize = MINNIE_DEFAULT_GL_BUF_SIZE;

   if(_maxDrawBufSize < 1024)
      _maxDrawBufSize = MINNIE_DEFAULT_DRAW_BUF_SIZE;

   if(NULL == buf_gl)
   {
      buf_gl   = YAC_New_Buffer();
      buf_draw = YAC_New_Buffer();
   }

#ifdef SHADERVG_SCRIPT_API
   if(buf_gl->yacArrayAlloc(_maxGLBufSize, 0,0,0))
   {
      if(buf_draw->yacArrayAlloc(_maxDrawBufSize, 0,0,0))
      {
         // Succeeded
         ret = YAC_TRUE;
      }
   }
#else
   // MINNIE_LIB
   if(buf_gl->alloc(_maxGLBufSize))
   {
      if(buf_draw->alloc(_maxDrawBufSize))
      {
         // Succeeded
         ret = YAC_TRUE;
      }
   }
#endif // SHADERVG_SCRIPT_API

   return ret;
}

void _MinnieDrawable::reset(void) {
   Dstream_set_offset(buf_gl, 0u);
   Dstream_set_offset(buf_draw, 0u);
}

void _MinnieDrawable::setEnableDebug(sBool _bEnable) {
   b_debug = _bEnable;
}

sBool _MinnieDrawable::getEnableDebug(void) {
   return b_debug;
}

void _MinnieDrawable::freeGL(void) {
   // must be called by onOpen()
   if(0u != gl_buf_id)
   {
      Dsdvg_glcall(zglDeleteBuffer(gl_buf_id));
      gl_buf_id = 0u;
   }
}

void _MinnieDrawable::free(void) {
   freeGL();

   YAC_DELETE_SAFE(buf_gl);
   YAC_DELETE_SAFE(buf_draw);
}

void _MinnieDrawable::lazyAllocGL(void) {
   // Create vertex attrib buffer
   if(0u == gl_buf_id && NULL != buf_gl && buf_gl->size > 0u)
   {
      gl_buf_id = Dsdvg_glcall(zglGenBuffer());
      Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, gl_buf_id));
      Dsdvg_glcall(zglBufferData(GL_ARRAY_BUFFER, buf_gl->size, NULL, GL_STATIC_DRAW));
      Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, 0)); // remove binding
      Dyac_host_printf("[dbg] MinnieDrawable::lazyAllocGL: VBO last error is %d\n", Dsdvg_glcall(glGetError()));
   }
}

YAC_Buffer *_MinnieDrawable::getGLBuffer(void) {
   return buf_gl;
}

sUI _MinnieDrawable::getGLOffset(void) {
   return (NULL != buf_gl) ? Dstream_get_offset(buf_gl) : 0u;
}

YAC_Buffer *_MinnieDrawable::getDrawBuffer(void) {
   return buf_draw;
}

sUI _MinnieDrawable::getDrawOffset (void) {
   return (NULL != buf_draw) ? Dstream_get_offset(buf_draw) : 0u;
}

void _MinnieDrawable::onOpen(void) {
   freeGL();
}

void _MinnieDrawable::setSize2f(sF32 _w, sF32 _h) {
   size_x = _w;
   size_y = _h;
}

void _MinnieDrawable::setSizeX(sF32 _w) {
   size_x = _w;
}

void _MinnieDrawable::setSizeY(sF32 _h) {
   size_y = _h;
}

sF32 _MinnieDrawable::getSizeX(void) {
   return size_x;
}

sF32 _MinnieDrawable::getSizeY(void) {
   return size_y;
}

void _MinnieDrawable::setBackgroundColor(sUI _c32) {
   c32_bg_color = _c32;
}

sUI _MinnieDrawable::getBackgroundColor(void) {
   return c32_bg_color;
}

void _MinnieDrawable::setScale2f(sF32 _sx, sF32 _sy) {
   scale_x = _sx;
   scale_y = _sy;
}

void _MinnieDrawable::setScaleX(sF32 _s) {
   scale_x = _s;
}

void _MinnieDrawable::setScaleY(sF32 _s) {
   scale_y = _s;
}

sF32 _MinnieDrawable::getScaleX(void) {
   return scale_x;
}

sF32 _MinnieDrawable::getScaleY(void) {
   return scale_y;
}

void _MinnieDrawable::setRotation(sF32 _a) {
   rot_ang = _a;
}

sF32 _MinnieDrawable::getRotation(void) {
   return rot_ang;
}

void _MinnieDrawable::setTranslate2f(sF32 _tx, sF32 _ty) {
   trans_x = _tx;
   trans_y = _ty;
}

void _MinnieDrawable::setTranslateX(sF32 _tx) {
   trans_x = _tx;
}

void _MinnieDrawable::setTranslateY(sF32 _ty) {
   trans_y = _ty;
}

sF32 _MinnieDrawable::getTranslateX(void) {
   return trans_x;
}

sF32 _MinnieDrawable::getTranslateY(void) {
   return trans_y;
}

void _MinnieDrawable::queueGLBufUpdate(void) {
   b_gl_buf_update_pending = YAC_TRUE;
}

sBool _MinnieDrawable::isComplete(void) {
   return (Dstream_get_offset(buf_gl) > 0u) && (Dstream_get_offset(buf_draw) > 0u);
}

void _MinnieDrawable::draw() {
   // Dyac_host_printf("xxx _MinnieDrawable::draw()\n");
   lazyAllocGL();

   if(0 != gl_buf_id && isComplete())
   {
      if(b_gl_buf_update_pending)
      {
         b_gl_buf_update_pending = false;
         Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, gl_buf_id));
         Dsdvg_glcall(zglBufferSubData(GL_ARRAY_BUFFER, 0, Dstream_get_offset(buf_gl), buf_gl));
         Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, 0));
      }

      minExecDrawListEx(buf_draw, gl_buf_id, b_debug);
   }
}
