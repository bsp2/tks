// ----
// ---- file   : gl_ext.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2001-2025 by bsp
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
// ---- note   :
// ----
// ----
// ----

#ifndef GL_EXT_H__
#define GL_EXT_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
typedef void      (APIENTRY *glActiveTexture_t)              (GLenum);
typedef void      (APIENTRY *glBlendFuncSeparate_t)          (GLenum, GLenum, GLenum, GLenum);
typedef GLboolean (APIENTRY *glIsBuffer_t)                   (GLuint);
typedef void      (APIENTRY *glBindBuffer_t)                 (GLenum, GLuint);
typedef void      (APIENTRY *glDeleteBuffers_t)              (GLsizei, const GLuint *);
typedef void      (APIENTRY *glGenBuffers_t)                 (GLsizei, GLuint *);
typedef void      (APIENTRY *glBufferData_t)                 (GLenum, GLsizei, const GLvoid *, GLenum);
typedef void      (APIENTRY *glBufferSubData_t)              (GLenum, GLsizei, GLsizei, const GLvoid *);
typedef void      (APIENTRY *glDrawArraysInstanced_t)        (GLenum, GLint, GLsizei, GLsizei);
typedef void *    (APIENTRY *glMapBuffer_t)                  (GLenum, GLenum);
typedef void *    (APIENTRY *glMapBufferRange_t)             (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef void      (APIENTRY *glFlushMappedBufferRange_t)     (GLenum target, GLintptr offset, GLsizeiptr length);
typedef GLboolean (APIENTRY *glUnmapBuffer_t)                (GLenum);
typedef GLboolean (APIENTRY *glIsRenderbuffer_t)             (GLuint renderbuffer);
typedef void      (APIENTRY *glBindRenderbuffer_t)           (GLenum target, GLuint renderbuffer);
typedef void      (APIENTRY *glDeleteRenderbuffers_t)        (GLsizei n, const GLuint *renderbuffers);
typedef void      (APIENTRY *glGenRenderbuffers_t)           (GLsizei n, GLuint *renderbuffers);
typedef void      (APIENTRY *glRenderbufferStorage_t)        (GLenum target, GLenum internalFormat, GLsizei width, GLsizei height);
typedef void      (APIENTRY *glRenderbufferStorageMultisample_t) (GLenum target, GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height);
typedef void      (APIENTRY *glGetRenderbufferParameteriv_t) (GLenum target, GLenum pname, GLint *params);
typedef GLboolean (APIENTRY *glIsFramebuffer_t)              (GLuint framebuffer);
typedef void      (APIENTRY *glBindFramebuffer_t)            (GLenum target, GLuint framebuffer);
typedef void      (APIENTRY *glDeleteFramebuffers_t)         (GLsizei n, const GLuint *framebuffers);
typedef void      (APIENTRY *glGenFramebuffers_t)            (GLsizei n, GLuint *framebuffers);
typedef GLenum    (APIENTRY *glCheckFramebufferStatus_t)     (GLenum target);
typedef void      (APIENTRY *glFramebufferTexture1D_t)       (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void      (APIENTRY *glFramebufferTexture2D_t)       (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void      (APIENTRY *glFramebufferTexture3D_t)       (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
typedef void      (APIENTRY *glFramebufferRenderbuffer_t)    (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void      (APIENTRY *glGetFramebufferAttachmentParameteriv_t) (GLenum target, GLenum attachment, GLenum pname, GLint *params);
typedef void      (APIENTRY *glBlitFramebuffer_t)            (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void      (APIENTRY *glGenerateMipmap_t)             (GLenum target);

typedef void      (APIENTRY *glDrawBuffers_t)                (GLsizei n, const GLenum *);
typedef void      (APIENTRY *glGetBufferParameteriv_t)       (GLenum _target, GLenum _value, GLint *);
typedef void      (APIENTRY *glVertexAttrib1f_t)             (GLuint index, GLfloat v0);
typedef void      (APIENTRY *glVertexAttrib2f_t)             (GLuint index, GLfloat v0, GLfloat v1);
typedef void      (APIENTRY *glVertexAttrib2fv_t)            (GLuint index, const GLfloat *v);
typedef void      (APIENTRY *glVertexAttrib3fv_t)            (GLuint index, const GLfloat *v);
typedef void      (APIENTRY *glVertexAttrib4fv_t)            (GLuint index, const GLfloat *v);
typedef void      (APIENTRY *glVertexAttrib3f_t)             (GLuint index, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void      (APIENTRY *glVertexAttrib4f_t)             (GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void      (APIENTRY *glVertexAttrib4Nub_t)           (GLuint index, GLubyte v0, GLubyte v1, GLubyte v2, GLubyte v3);
typedef void      (APIENTRY *glVertexAttribPointer_t)        (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid*pointer);
typedef void      (APIENTRY *glEnableVertexAttribArray_t)    (GLuint index);
typedef void      (APIENTRY *glDisableVertexAttribArray_t)   (GLuint index);
typedef GLuint    (APIENTRY *glCreateProgram_t)              (void);
typedef void      (APIENTRY *glDeleteProgram_t)              (GLuint program);
typedef GLuint    (APIENTRY *glCreateShader_t)               (GLenum shaderType);
typedef void      (APIENTRY *glDeleteShader_t)               (GLuint shader);
typedef void      (APIENTRY *glShaderSource_t)               (GLuint shader, GLsizei count, const char*const*strings, const GLint*lengths);
typedef void      (APIENTRY *glCompileShader_t)              (GLuint shader);
typedef void      (APIENTRY *glAttachShader_t)               (GLuint program, GLuint shader);
typedef void      (APIENTRY *glDetachShader_t)               (GLuint program, GLuint shader);
typedef void      (APIENTRY *glLinkProgram_t)                (GLuint program);
typedef void      (APIENTRY *glValidateProgram_t)            (GLuint program);
typedef void      (APIENTRY *glGetShaderiv_t)                (GLuint shader, GLenum pname, GLint*params);
typedef void      (APIENTRY *glGetShaderInfoLog_t)           (GLuint shader, GLsizei maxLength, GLsizei*length, char*infoLog);
typedef void      (APIENTRY *glGetProgramInfoLog_t)          (GLuint program, GLsizei maxLength, GLsizei*length, char*infoLog);
typedef void      (APIENTRY *glGetProgramiv_t)               (GLuint program, GLenum pname, GLint*params);
typedef void      (APIENTRY *glUseProgram_t)                 (GLuint program);
typedef void      (APIENTRY *glBindAttribLocation_t)         (GLuint program, GLuint index, const char*name);
typedef GLint     (APIENTRY *glGetAttribLocation_t)          (GLuint program, const char*name);
typedef void      (APIENTRY *glBindFragDataLocation_t)       (GLuint program, GLuint colorNumber, const char*name);
typedef void      (APIENTRY *glBindFragDataLocationIndexed_t)(GLuint program, GLuint colorNumber, GLuint index, const char*name);
typedef GLint     (APIENTRY *glGetFragDataLocation_t)        (GLuint program, const char*name);
typedef GLint     (APIENTRY *glGetFragDataIndex_t)           (GLuint program, const char*name);
typedef void      (APIENTRY *glEnableVertexAttribArray_t)    (GLuint index);
typedef void      (APIENTRY *glDisableVertexAttribArray_t)   (GLuint index);
typedef GLint     (APIENTRY *glGetUniformLocation_t)         (GLuint program, const char*name);
typedef void      (APIENTRY *glGetActiveUniform_t)           (GLuint program, GLuint index, GLsizei bufSize, GLsizei*length, GLint*size, GLenum*type, char*name);
typedef void      (APIENTRY *glUniform1f_t)                  (GLint location, GLfloat v0);
typedef void      (APIENTRY *glUniform2f_t)                  (GLint location, GLfloat v0, GLfloat v1);
typedef void      (APIENTRY *glUniform3f_t)                  (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void      (APIENTRY *glUniform4f_t)                  (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void      (APIENTRY *glUniform2fv_t)                 (GLint location, GLsizei count, const GLfloat *values);
typedef void      (APIENTRY *glUniform3fv_t)                 (GLint location, GLsizei count, const GLfloat *values);
typedef void      (APIENTRY *glUniform4fv_t)                 (GLint location, GLsizei count, const GLfloat *values);
typedef void      (APIENTRY *glUniform1i_t)                  (GLint location, GLint v0);
typedef void      (APIENTRY *glUniform2i_t)                  (GLint location, GLint v0, GLint v1);
typedef void      (APIENTRY *glUniform3i_t)                  (GLint location, GLint v0, GLint v1, GLint v2);
typedef void      (APIENTRY *glUniform4i_t)                  (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void      (APIENTRY *glUniformMatrix2fv_t)           (GLint location, GLsizei count, GLboolean transpose, const GLfloat*value);
typedef void      (APIENTRY *glUniformMatrix3fv_t)           (GLint location, GLsizei count, GLboolean transpose, const GLfloat*value);
typedef void      (APIENTRY *glUniformMatrix4fv_t)           (GLint location, GLsizei count, GLboolean transpose, const GLfloat*value);
typedef void      (APIENTRY *glBindVertexArray_t)            (GLuint array);
typedef void      (APIENTRY *glDeleteVertexArrays_t)         (GLsizei n, const GLuint *arrays);
typedef void      (APIENTRY *glGenVertexArrays_t)            (GLsizei n, GLuint *arrays);
typedef GLboolean (APIENTRY *glIsVertexArray_t)              (GLuint array);
typedef void      (APIENTRY *glVertexAttribDivisor_t)        (GLuint index, GLuint divisor);

extern glBindBuffer_t                     glBindBuffer;
extern glIsBuffer_t                       glIsBuffer;
extern glDeleteBuffers_t                  glDeleteBuffers;
extern glGenBuffers_t                     glGenBuffers;
extern glBufferData_t                     glBufferData;
extern glBufferSubData_t                  glBufferSubData;
extern glDrawArraysInstanced_t            glDrawArraysInstanced;
extern glGetBufferParameteriv_t           glGetBufferParameteriv;
extern glMapBuffer_t                      glMapBuffer;
extern glMapBufferRange_t                 glMapBufferRange;
extern glFlushMappedBufferRange_t         glFlushMappedBufferRange;
extern glUnmapBuffer_t                    glUnmapBuffer;
extern glActiveTexture_t                  glActiveTexture;
extern glBlendFuncSeparate_t              glBlendFuncSeparate;
extern glBindRenderbuffer_t               glBindRenderbuffer;
extern glDeleteRenderbuffers_t            glDeleteRenderbuffers;
extern glGenRenderbuffers_t               glGenRenderbuffers;
extern glBindFramebuffer_t                glBindFramebuffer;
extern glDeleteFramebuffers_t             glDeleteFramebuffers;
extern glGenFramebuffers_t                glGenFramebuffers;
extern glCheckFramebufferStatus_t         glCheckFramebufferStatus;
extern glFramebufferTexture2D_t           glFramebufferTexture2D;
extern glBlitFramebuffer_t                glBlitFramebuffer;
extern glCreateProgram_t                  glCreateProgram;
extern glDeleteProgram_t                  glDeleteProgram;
extern glCreateShader_t                   glCreateShader;
extern glDeleteShader_t                   glDeleteShader;
extern glShaderSource_t                   glShaderSource;
extern glCompileShader_t                  glCompileShader;
extern glAttachShader_t                   glAttachShader;
extern glDetachShader_t                   glDetachShader;
extern glLinkProgram_t                    glLinkProgram;
extern glValidateProgram_t                glValidateProgram;
extern glGetShaderiv_t                    glGetShaderiv;
extern glGetShaderInfoLog_t               glGetShaderInfoLog;
extern glGetProgramInfoLog_t              glGetProgramInfoLog;
extern glGetProgramiv_t                   glGetProgramiv;
extern glUseProgram_t                     glUseProgram;
extern glBindAttribLocation_t             glBindAttribLocation;
extern glGetAttribLocation_t              glGetAttribLocation;
extern glBindFragDataLocation_t           glBindFragDataLocation;
extern glBindFragDataLocationIndexed_t    glBindFragDataLocationIndexed;
extern glGetFragDataLocation_t            glGetFragDataLocation;
extern glGetFragDataIndex_t               glGetFragDataIndex;
extern glVertexAttrib1f_t                 glVertexAttrib1f;
extern glVertexAttrib2f_t                 glVertexAttrib2f;
extern glVertexAttrib3f_t                 glVertexAttrib3f;
extern glVertexAttrib4f_t                 glVertexAttrib4f;
extern glVertexAttrib4Nub_t               glVertexAttrib4Nub;
extern glVertexAttrib2fv_t                glVertexAttrib2fv;
extern glVertexAttrib3fv_t                glVertexAttrib3fv;
extern glVertexAttrib4fv_t                glVertexAttrib4fv;
extern glVertexAttribPointer_t            glVertexAttribPointer;
extern glEnableVertexAttribArray_t        glEnableVertexAttribArray;
extern glDisableVertexAttribArray_t       glDisableVertexAttribArray;
extern glBindVertexArray_t                glBindVertexArray;
extern glDeleteVertexArrays_t             glDeleteVertexArrays;
extern glGenVertexArrays_t                glGenVertexArrays;
extern glIsVertexArray_t                  glIsVertexArray;
extern glVertexAttribDivisor_t            glVertexAttribDivisor;
extern glGetUniformLocation_t             glGetUniformLocation;
extern glGetActiveUniform_t               glGetActiveUniform;
extern glUniform1f_t                      glUniform1f;
extern glUniform2f_t                      glUniform2f;
extern glUniform3f_t                      glUniform3f;
extern glUniform4f_t                      glUniform4f;
extern glUniform2fv_t                     glUniform2fv;
extern glUniform3fv_t                     glUniform3fv;
extern glUniform4fv_t                     glUniform4fv;
extern glUniform1i_t                      glUniform1i;
extern glUniform2i_t                      glUniform2i;
extern glUniform3i_t                      glUniform3i;
extern glUniform4i_t                      glUniform4i;
extern glUniformMatrix2fv_t               glUniformMatrix2fv;
extern glUniformMatrix3fv_t               glUniformMatrix3fv;
extern glUniformMatrix4fv_t               glUniformMatrix4fv;
extern glRenderbufferStorage_t            glRenderbufferStorage;
extern glFramebufferRenderbuffer_t        glFramebufferRenderbuffer;
extern glRenderbufferStorageMultisample_t glRenderbufferStorageMultisample;
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // GL_EXT_H__
