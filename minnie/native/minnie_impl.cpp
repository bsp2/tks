// ----
// ---- file   : minnie_impl.cpp
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
// ---- info   : "minnie" implementation
// ---- note   : must be compiled as C++ code
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#ifdef USE_MINNIE_MIB_SETUP
#include "../inc_minnie_main.h"
#else
#include "../inc_minnie.h"
#endif // USE_MINNIE_MIB_SETUP

#include "../minnie_exec.h"


sUI yac_host_yacGetDebugLevel(void) {
   // return 20u;
   return 4u;
}

sBool yac_buffer_alloc(YAC_Buffer *_buffer, sU32 _size) {
   return _buffer->alloc(_size);
}

void yac_buffer_free(YAC_Buffer *_buffer) {
   _buffer->free();
}

void yac_buffer_write_s16(YAC_Buffer *_buffer, sS16 _x) {
   _buffer->writeI16(_x);
}

void yac_buffer_write_2s16(YAC_Buffer *_buffer, sS16 _x, sS16 _y) {
   _buffer->writeI16(_x);
   _buffer->writeI16(_y);
}

void yac_buffer_write_3s16(YAC_Buffer *_buffer, sS16 _x, sS16 _y, sS16 _z) {
   _buffer->writeI16(_x);
   _buffer->writeI16(_y);
   _buffer->writeI16(_z);
}

void yac_buffer_write_4s16(YAC_Buffer *_buffer, sS16 _x, sS16 _y, sS16 _z, sS16 _w) {
   _buffer->writeI16(_x);
   _buffer->writeI16(_y);
   _buffer->writeI16(_z);
   _buffer->writeI16(_w);
}

void yac_buffer_write_1fx(YAC_Buffer *_buffer, sF32 _x) {
   _buffer->writeI16(sS16(_x*4.0f));
}

void yac_buffer_write_2fx(YAC_Buffer *_buffer, sF32 _x, sF32 _y) {
   _buffer->writeI16(sS16(_x*4.0f));
   _buffer->writeI16(sS16(_y*4.0f));
}

void yac_buffer_write_3fx(YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z) {
   _buffer->writeI16(sS16(_x*4.0f));
   _buffer->writeI16(sS16(_y*4.0f));
   _buffer->writeI16(sS16(_z*4.0f));
}

void yac_buffer_write_4fx(YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z, sF32 _w) {
   _buffer->writeI16(sS16(_x*4.0f));
   _buffer->writeI16(sS16(_y*4.0f));
   _buffer->writeI16(sS16(_z*4.0f));
   _buffer->writeI16(sS16(_w*4.0f));
}

void yac_buffer_write_f32(YAC_Buffer *_buffer, sF32 _x) {
   _buffer->writeF32(_x);
}

void yac_buffer_write_2f(YAC_Buffer *_buffer, sF32 _x, sF32 _y) {
   _buffer->writeF32(_x);
   _buffer->writeF32(_y);
}

void yac_buffer_write_3f(YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z) {
   _buffer->writeF32(_x);
   _buffer->writeF32(_y);
   _buffer->writeF32(_z);
}

void yac_buffer_write_4f(YAC_Buffer *_buffer, sF32 _x, sF32 _y, sF32 _z, sF32 _w) {
   _buffer->writeF32(_x);
   _buffer->writeF32(_y);
   _buffer->writeF32(_z);
   _buffer->writeF32(_w);
}

#ifdef USE_MINNIE_MIB_SETUP
void minnie_impl_init(void) {
   minnie::setup::Init();
}

void minnie_impl_exit(void) {
   minnie::setup::Exit();
}
#endif // USE_MINNIE_MIB_SETUP

void minnie_matrix4f_initIdentity(Matrix4f *_this) {
   _this->initIdentity();
}

void minnie_matrix4f_initOrtho(Matrix4f *_this,
                               sF32 _left,   sF32 _right,
                               sF32 _bottom, sF32 _top,
                               sF32 _znear,  sF32 _zfar
                               ) {
   _this->initOrtho(_left,   _right,
                    _bottom, _top,
                    _znear,  _zfar
                    );
}

void minnie_matrix4f_mulr(const Matrix4f *_this, const Matrix4f *_o, Matrix4f *_r) {
   _this->mulr(_o, _r);
}

void minnie_matrix4f_scalef(Matrix4f *_this, sF32 _x, sF32 _y, sF32 _z) {
   _this->scalef(_x, _y, _z);
}

void minnie_matrix4f_translatef(Matrix4f *_this, sF32 _x, sF32 _y, sF32 _z) {
   _this->translatef(_x, _y, _z);
}

void minnie_matrix4f_rotatef(Matrix4f *_this, sF32 _x, sF32 _y, sF32 _z) {
   _this->rotatef(_x, _y, _z);
}
