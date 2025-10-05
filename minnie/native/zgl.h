// ----
// ---- file   : zgl.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2014-2025 by bsp
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
// ---- info   : zgl utility functions
// ---- note   :
// ----
// ----
// ----

#ifndef ZGL_H__
#define ZGL_H__


inline void zglVertexAttribOffset(sUI _index, sUI _size, sSI _type, sSI _bNormalized, sSI _stride, sSI _offset) {
   glVertexAttribPointer(_index, (GLint)_size, (GLenum)_type, (GLboolean)_bNormalized, _stride,
#ifdef YAC_64
                         (void*)(sU64)_offset
#else
                         (void*)_offset
#endif // YAC_64
                         );
}

inline sUI zglGenBuffer(void) {
   GLuint r = 0u;
   glGenBuffers(1, &r);
   return (sUI)r;
}

inline void zglBufferData(sSI _target, sSI _size, YAC_Buffer *_data, sSI _usage) {
   if(_size <= 0 && NULL != _data)
      _size = (sSI)_data->size;
   glBufferData(_target, _size, (const void *)((NULL != _data)?_data->buffer:NULL), _usage);
}

inline void zglBufferSubData(sSI _target, sSI _offset, sSI _size, YAC_Buffer *_data) {
   if(_size <= 0 && NULL != _data)
      _size = (sSI)_data->size;
   glBufferSubData(_target, _offset, _size, (const void*)_data->buffer);
}

inline sSI zglGetBufferParameter(sSI _target, sSI _value) {
   sSI r = 0;
   glGetBufferParameteriv(_target, _value, &r);
   return r;
}

inline void zglMapBuffer(sSI _target, sSI _access, YAC_Buffer *_buffer, sUI _byteSize) {
   // Map buffer object into client address space
   void *data = glMapBuffer(_target, _access);

   // Free previous buffer data (if any)
   yac_buffer_free(_buffer);

   if(NULL != data)
   {
      _buffer->buffer    = (sU8*)data;
      _buffer->deleteme  = YAC_FALSE;
      _buffer->size      = _byteSize;
      _buffer->io_offset = 0u;
   }
}

inline void zglMapBufferRange(sSI _target, sUI _offset, sUI _length, sSI _access, YAC_Buffer *_buffer) {
   void *data = glMapBufferRange(_target, _offset, _length, _access);

   // Free previous buffer data (if any)
   yac_buffer_free(_buffer);

   // Dyac_host_printf("xxx call zglMapBufferRange: offset=%u length=%u => data=%p\n", _offset, _length, data);
   if(NULL != data)
   {
      _buffer->buffer    = (sU8*)data;
      _buffer->deleteme  = YAC_FALSE;
      _buffer->size      = _length;
      _buffer->io_offset = 0u;
   }
}

inline void zglDeleteBuffer (sSI _id) {
   glDeleteBuffers(1, (GLuint*) &_id);
}

inline sUI zglGenFramebuffer(void) {
   sUI r;
   glGenFramebuffers(1, &r);
   return r;
}

inline void zglDeleteFramebuffer(sUI _framebuffer) {
   glDeleteFramebuffers(1, &_framebuffer);
}

inline sUI zglGenTexture(void) {
   sUI r;
   glGenTextures(1, &r);
   return r;
}

inline void zglDeleteTexture(sUI _tex) {
   glDeleteTextures(1, &_tex);
}

inline sUI zglGenRenderbuffer(void) {
   sUI r;
   glGenRenderbuffers(1, &r);
   return r;
}

inline void zglDeleteRenderbuffer(sUI _tex) {
   glDeleteRenderbuffers(1, &_tex);
}

inline void zglDeleteVertexArray(sUI _array) {
   glDeleteVertexArrays(1, &_array);
}

inline sUI zglGenVertexArray(void) {
   sUI ret = 0u;
   glGenVertexArrays(1, &ret);
   return ret;
}

#endif // ZGL_H__
