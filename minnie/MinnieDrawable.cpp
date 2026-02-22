// ----
// ---- file   : MinnieDrawable.cpp
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
// ---- info   : "minnie" VBO/DisplayList drawable utility class
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#if defined(__cplusplus)
#include <new>
#endif

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

void _MinnieDrawable::begin(void) {
   minFreeDynamic();
   // minDebugPrintAllocStats();
   minResetAllocStats();
   reset();
   minSetVertexBufferExportOFS(getGLBuffer());
   minSetDrawListExportOFS(getDrawBuffer());
   minBegin();
}

void _MinnieDrawable::end(void) {
   minEnd();
   setSize2f(minGetWidth(), minGetHeight());
   setBackgroundColor(minGetColorByIndex(0));
   queueGLBufUpdate();
}

void _MinnieDrawable::draw() {
   // Dyac_host_printf("xxx _MinnieDrawable::draw()\n");
   lazyAllocGL();

   if(0u != gl_buf_id && isComplete())
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

// ---------------------------------------------------------------------------- C API
#ifndef MINNIE_SKIP_DRAWABLE_C_API

MinnieDrawable *minDrawableNew(void) {
   return (MinnieDrawable*)new(std::nothrow)_MinnieDrawable();
}

void minDrawableInit(MinnieDrawable *_drawable) {
   _drawable->init();
}

void minDrawableDelete(MinnieDrawable *_drawable) {
   if(NULL != _drawable)
   {
      delete _drawable;
   }
}

sBool minDrawableAlloc(MinnieDrawable *_drawable, sUI _maxGLBufSize, sUI _maxDrawBufSize) {
   return _drawable->alloc(_maxGLBufSize, _maxDrawBufSize);
}

void minDrawableReset(MinnieDrawable *_drawable) {
   _drawable->reset();
}

void minDrawableSetEnableDebug(MinnieDrawable *_drawable, sBool _bEnable) {
   _drawable->setEnableDebug(_bEnable);
}

sBool minDrawableGetEnableDebug(MinnieDrawable *_drawable) {
   return _drawable->getEnableDebug();
}

void minDrawableFreeGL(MinnieDrawable *_drawable) {
   _drawable->freeGL();
}

void minDrawableFree(MinnieDrawable *_drawable) {
   _drawable->free();
}

void minDrawableLazyAllocGL(MinnieDrawable *_drawable) {
   _drawable->lazyAllocGL();
}

YAC_Buffer *minDrawableGetGLBuffer(MinnieDrawable *_drawable) {
   return _drawable->getGLBuffer();
}

sUI minDrawableGetGLOffset(MinnieDrawable *_drawable) {
   return _drawable->getGLOffset();
}

YAC_Buffer *minDrawableGetDrawBuffer(MinnieDrawable *_drawable) {
   return _drawable->getDrawBuffer();
}

sUI minDrawableGetDrawOffset(MinnieDrawable *_drawable) {
   return _drawable->getDrawOffset();
}

void minDrawableOnOpen(MinnieDrawable *_drawable) {
   _drawable->onOpen();
}

void minDrawableSetSize2f(MinnieDrawable *_drawable, sF32 _w, sF32 _h) {
   _drawable->setSize2f(_w, _h);
}

void minDrawableSetSizeX(MinnieDrawable *_drawable, sF32 _w) {
   _drawable->setSizeX(_w);
}

void minDrawableSetSizeY(MinnieDrawable *_drawable, sF32 _h) {
   _drawable->setSizeY(_h);
}

sF32 minDrawableGetSizeX(MinnieDrawable *_drawable) {
   return _drawable->getSizeX();
}

sF32 minDrawableGetSizeY(MinnieDrawable *_drawable) {
   return _drawable->getSizeY();
}

void minDrawableSetBackgroundColor(MinnieDrawable *_drawable, sUI _c32) {
   return _drawable->setBackgroundColor(_c32);
}

sUI minDrawableGetBackgroundColor(MinnieDrawable *_drawable) {
   return _drawable->getBackgroundColor();
}

void minDrawableSetScale2f(MinnieDrawable *_drawable, sF32 _sx, sF32 _sy) {
   _drawable->setScale2f(_sx, _sy);
}

void minDrawableSetScaleX(MinnieDrawable *_drawable, sF32 _s) {
   _drawable->setScaleX(_s);
}

void minDrawableSetScaleY(MinnieDrawable *_drawable, sF32 _s) {
   _drawable->setScaleY(_s);
}

sF32 minDrawableGetScaleX(MinnieDrawable *_drawable) {
   return _drawable->getScaleX();
}

sF32 minDrawableGetScaleY(MinnieDrawable *_drawable) {
   return _drawable->getScaleY();
}

void minDrawableSetRotation(MinnieDrawable *_drawable, sF32 _a) {
   _drawable->setRotation(_a);
}

sF32 minDrawableGetRotation(MinnieDrawable *_drawable) {
   return _drawable->getRotation();
}

void minDrawableSetTranslate2f(MinnieDrawable *_drawable, sF32 _tx, sF32 _ty) {
   _drawable->setTranslate2f(_tx, _ty);
}

void minDrawableSetTranslateX(MinnieDrawable *_drawable, sF32 _tx) {
   _drawable->setTranslateX(_tx);
}

void minDrawableSetTranslateY(MinnieDrawable *_drawable, sF32 _ty) {
   _drawable->setTranslateY(_ty);
}

sF32 minDrawableGetTranslateX(MinnieDrawable *_drawable) {
   return _drawable->getTranslateX();
}

sF32 minDrawableGetTranslateY(MinnieDrawable *_drawable) {
   return _drawable->getTranslateY();
}

void minDrawableQueueGLBufUpdate(MinnieDrawable *_drawable) {
   _drawable->queueGLBufUpdate();
}

sBool minDrawableIsComplete(MinnieDrawable *_drawable) {
   return _drawable->isComplete();
}

void minDrawableBegin(MinnieDrawable *_drawable) {
   _drawable->begin();
}

void minDrawableEnd(MinnieDrawable *_drawable) {
   _drawable->end();
}

void minDrawableDraw(MinnieDrawable *_drawable) {
   _drawable->draw();
}

#endif // MINNIE_SKIP_DRAWABLE_C_API
