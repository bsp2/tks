/// tkopengl_shared.h
///
/// (c) 2013-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef TKOPENGL_SHARED_H__
#define TKOPENGL_SHARED_H__

// tkopengl functions shared with other plugins (e.g. tksdl)

#define TKOPENGL_SHARED_NAME  "TKOPENGL_SHARED"

extern "C" {
   typedef struct {
      void   (YAC_CALL *_glActiveTexture)            (sSI _texture);
      void   (YAC_CALL *_glAttachShader)             (sUI _program, sUI _shader);
      void   (YAC_CALL *_glBegin)                    (sSI _mode);
      void   (YAC_CALL *_glBindAttribLocation)       (GLuint _program, GLuint _index, const char *_name);
      void   (YAC_CALL *_glBindBuffer)               (sSI _target, sUI _id);
      void   (YAC_CALL *_glBindFramebuffer)          (sSI _target, sUI _framebuffer);
      void   (YAC_CALL *_glBindRenderbuffer)         (sSI _target, sUI _renderbuffer);
      void   (YAC_CALL *_glBindTexture)              (sSI _target, sUI _texture);
      void   (YAC_CALL *_glBindVertexArray)          (sUI _array);
      void   (YAC_CALL *_glBlendColor)               (sF32 _a, sF32 _b, sF32 _c, sF32 _d);
      void   (YAC_CALL *_glBlendEquation)            (sSI _a);
      void   (YAC_CALL *_glBlendFunc)                (sSI _a, sSI _b);
      void   (YAC_CALL *_glBlendFuncSeparate)        (sSI _a, sSI _b, sSI _c, sSI _d);
      void   (YAC_CALL *_glBlitFramebuffer)          (sSI _srcX0, sSI _srcY0, sSI _srcX1, sSI _srcY1, sSI _dstX0, sSI _dstY0, sSI _dstX1, sSI _dstY1, sUI _mask, sSI _filter);
      void   (YAC_CALL *_glBufferData)               (GLenum, GLsizei, const GLvoid *, GLenum);
      sSI    (YAC_CALL *_glCheckFramebufferStatus)   (sSI _target);
      void   (YAC_CALL *_glClear)                    (sSI _a);
      void   (YAC_CALL *_glClearColor)               (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_glClearDepth)               (sF32);
      void   (YAC_CALL *_glClearStencil)             (sSI _s);
      void   (YAC_CALL *_glColor3f)                  (sF32 _r, sF32 _g, sF32 _b);
      void   (YAC_CALL *_glColor4f)                  (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_glColorMask)                (sSI, sSI, sSI, sSI);
      void   (YAC_CALL *_glCompileShader)            (sUI _shader);
      sUI    (YAC_CALL *_glCreateProgram)            (void);
      sUI    (YAC_CALL *_glCreateShader)             (sUI _shaderType);
      void   (YAC_CALL *_glCullFace)                 (sSI _a);
      void   (YAC_CALL *_glDeleteProgram)            (sUI _program);
      void   (YAC_CALL *_glDeleteShader)             (sUI _shader);
      void   (YAC_CALL *_glDepthFunc)                (sSI);
      void   (YAC_CALL *_glDepthMask)                (sSI);
      void   (YAC_CALL *_glDetachShader)             (sUI _program, sUI _shader);
      void   (YAC_CALL *_glDisable)                  (sSI);
      void   (YAC_CALL *_glDisableVertexAttribArray) (sUI _index);
      void   (YAC_CALL *_glDrawArrays)               (sSI _mode, sSI _first, sSI _count);
      void   (YAC_CALL *_glDrawArraysInstanced)      (sSI _mode, sSI _first, sSI _count, sSI _instancecount);
      void   (YAC_CALL *_glDrawBuffer)               (sSI _mode);
      void   (YAC_CALL *_glDrawElements)             (GLenum _mode, GLsizei _count, GLenum _type, const GLvoid *_indices);
      void   (YAC_CALL *_glEnable)                   (sSI);
      void   (YAC_CALL *_glEnableVertexAttribArray)  (sUI _index);
      void   (YAC_CALL *_glEnd)                      (void);
      void   (YAC_CALL *_glFinish)                   (void);
      void   (YAC_CALL *_glFlush)                    (void);
      void   (YAC_CALL *_glFlushMappedBufferRange)   (sSI _target, sUI _offset, sUI _length);
      void   (YAC_CALL *_glFramebufferRenderbuffer)  (sSI _target, sSI _attachment, sSI _renderbuffertarget, sSI _renderbuffer);
      void   (YAC_CALL *_glFramebufferTexture2D)     (sSI _target, sSI _attachment, sSI _textarget, sUI _texture, sSI _level);
      void   (YAC_CALL *_glFrontFace)                (sSI _a);
      GLint  (YAC_CALL *_glGetAttribLocation)        (GLuint _program, const char *_name);
      GLenum (YAC_CALL *_glGetError)                 (void);
      void   (YAC_CALL *_glGetProgramiv)             (GLuint _program, GLenum _pname, GLint *_params);
      void   (YAC_CALL *_glGetProgramInfoLog)        (GLuint _program, GLsizei _maxLength, GLsizei *_length, char *_infoLog);
      void   (YAC_CALL *_glGetShaderiv)              (GLuint _shader, GLenum _pname, GLint *_r);
      void   (YAC_CALL *_glGetShaderInfoLog)         (GLuint _shader, GLsizei _maxLength, GLsizei *_length, char *_infoLog);
      sSI    (YAC_CALL *_glGetUniformLocation)       (sUI _program, const char *_name);
      void   (YAC_CALL *_glHint)                     (sSI, sSI);
      void   (YAC_CALL *_glLightfv)                  (GLenum _light, GLenum _pname, const GLfloat *_params);
      void   (YAC_CALL *_glLightModeli)              (sSI pname, sSI _param);
      void   (YAC_CALL *_glLinkProgram)              (sUI _program);
      void   (YAC_CALL *_glLoadIdentity)             (void);
      void   (YAC_CALL *_glLoadMatrixf)              (const sF32 *_mat);
      void   (YAC_CALL *_glMaterialfv)               (GLenum _face, GLenum _pname, const GLfloat *_params);
      void   (YAC_CALL *_glMatrixMode)               (sSI _mode);
      void   (YAC_CALL *_glMultMatrixf)              (const sF32 *_mat);
      void   (YAC_CALL *_glNormal3f)                 (sF32 _x, sF32 _y, sF32 _z);
      void   (YAC_CALL *_glPixelStorei)              (GLenum _pname, GLint _param);
      void   (YAC_CALL *_glPointParameterf)          (sSI _pname, sF32 _param);
      void   (YAC_CALL *_glPointParameterfv)         (sSI _pname, const sF32 *_params);
      void   (YAC_CALL *_glPointSize)                (sF32 _size);
      void   (YAC_CALL *_glPopAttrib)                (void);
      void   (YAC_CALL *_glPopMatrix)                (void);
      void   (YAC_CALL *_glPushAttrib)               (sUI _mask);
      void   (YAC_CALL *_glPushMatrix)               (void);
      void   (YAC_CALL *_glRenderbufferStorage)      (sSI _target, sSI _internalFormat, sUI _width, sUI _height);
      void   (YAC_CALL *_glRenderbufferStorageMultisample) (sSI _target, sUI _samples, sSI _internalFormat, sUI _width, sUI _height);
      void   (YAC_CALL *_glScalef)                   (sF32 _x, sF32 _y, sF32 _z);
      void   (YAC_CALL *_glScissor)                  (sSI _x, sSI _y, sSI _w, sSI _h);
      void   (YAC_CALL *_glShadeModel)               (sSI);
      void   (YAC_CALL *_glShaderSource)             (GLuint _shader, GLsizei _count, const char *const*_src, const GLint *_lengths);
      void   (YAC_CALL *_glStencilFunc)              (sSI _func, sSI _ref, sUI _mask);
      void   (YAC_CALL *_glStencilMask)              (sUI _mask);
      void   (YAC_CALL *_glStencilOp)                (sSI _sfail, sSI _dpfail, sSI _dppass);
      void   (YAC_CALL *_glTexCoord1f)               (sF32 _v0);
      void   (YAC_CALL *_glTexCoord2f)               (sF32 _v0, sF32 _v1);
      void   (YAC_CALL *_glTexCoord3f)               (sF32 _v0, sF32 _v1, sF32 _v2);
      void   (YAC_CALL *_glTexEnvfv)                 (GLenum _target, GLenum _pname, const GLfloat *_params);
      void   (YAC_CALL *_glTexImage1D)        (GLenum _target, GLint _level, GLint _internalFormat,
                                               GLsizei _width, GLint _border, GLenum _format, GLenum _type, const GLvoid *_data);
      void   (YAC_CALL *_glTexImage2D)        (GLenum _target, GLint _level, GLint _internalFormat,
                                               GLsizei _width, GLsizei _height,
                                               GLint _border, GLenum _format, GLenum _type, const GLvoid *_data);
      void   (YAC_CALL *_glTexImage3D)        (GLenum _target, GLint _level, GLint _internalFormat,
                                               GLsizei _width, GLsizei _height, GLsizei _depth,
                                               GLint _border, GLenum _format, GLenum _type, const GLvoid *_data);
      void   (YAC_CALL *_glTexParameteri)     (sSI _target, sSI _pname, sSI _param);
      void   (YAC_CALL *_glTexSubImage2D)     (GLenum _target, GLint _level,
                                               GLint _xoffset, GLint _yoffset,
                                               GLsizei _width, GLsizei _height,
                                               GLenum _format, GLenum _type, const GLvoid *_pixels);
      void   (YAC_CALL *_glTranslatef)        (sF32 _x, sF32 _y, sF32 _z);
      void   (YAC_CALL *_glUniform1i)         (sSI _location, sSI _v0);
      void   (YAC_CALL *_glUniform2i)         (sSI _location, sSI _v0, sSI _v1);
      void   (YAC_CALL *_glUniform3i)         (sSI _location, sSI _v0, sSI _v1, sSI _v2);
      void   (YAC_CALL *_glUniform4i)         (sSI _location, sSI _v0, sSI _v1, sSI _v2, sSI _v3);
      void   (YAC_CALL *_glUniform1f)         (sSI _location, sF32 _v0);
      void   (YAC_CALL *_glUniform2f)         (sSI _location, sF32 _v0, sF32 _v1);
      void   (YAC_CALL *_glUniform3f)         (sSI _location, sF32 _v0, sF32 _v1, sF32 _v2);
      void   (YAC_CALL *_glUniform4f)         (sSI _location, sF32 _v0, sF32 _v1, sF32 _v2, sF32 _v3);
      void   (YAC_CALL *_glUniform2fv)        (GLint _location, GLsizei _count, const GLfloat *_values);
      void   (YAC_CALL *_glUniform3fv)        (GLint _location, GLsizei _count, const GLfloat *_values);
      void   (YAC_CALL *_glUniform4fv)        (GLint _location, GLsizei _count, const GLfloat *_values);
      void   (YAC_CALL *_glUniformMatrix2fv)  (GLint _location, GLsizei _count, GLboolean _transpose, const GLfloat *_value);
      void   (YAC_CALL *_glUniformMatrix3fv)  (GLint _location, GLsizei _count, GLboolean _transpose, const GLfloat *_value);
      void   (YAC_CALL *_glUniformMatrix4fv)  (GLint _location, GLsizei _count, GLboolean _transpose, const GLfloat *_value);
      sSI    (YAC_CALL *_glUnmapBuffer)       (sSI _target);
      void   (YAC_CALL *_glUseProgram)        (GLuint _program);
      void   (YAC_CALL *_glValidateProgram)   (sUI _program);
      void   (YAC_CALL *_glVertex2f)          (sF32 _x, sF32 _y);
      void   (YAC_CALL *_glVertex3f)          (sF32 _x, sF32 _y, sF32 _z);
      void   (YAC_CALL *_glVertex4f)          (sF32 _x, sF32 _y, sF32 _z, sF32 _w);
      void   (YAC_CALL *_glVertexAttrib1f)    (sUI _index, sF32 _v0);
      void   (YAC_CALL *_glVertexAttrib2f)    (sUI _index, sF32 _v0, sF32 _v1);
      void   (YAC_CALL *_glVertexAttrib3f)    (sUI _index, sF32 _v0, sF32 _v1, sF32 _v2);
      void   (YAC_CALL *_glVertexAttrib4f)    (sUI _index, sF32 _v0, sF32 _v1, sF32 _v2, sF32 _v3);
      void   (YAC_CALL *_glVertexAttrib2fv)   (GLuint _index, const GLfloat *_v);
      void   (YAC_CALL *_glVertexAttrib3fv)   (GLuint _index, const GLfloat *_v);
      void   (YAC_CALL *_glVertexAttrib4fv)   (GLuint _index, const GLfloat *_v);
      void   (YAC_CALL *_glVertexAttribPointer)    (GLuint _index, GLint _size, GLenum _type,
                                                    GLboolean _normalized, GLsizei _stride, const GLvoid *_pointer);
      void   (YAC_CALL *_glVertexAttribDivisor)    (sUI index, sUI divisor);
      void   (YAC_CALL *_glViewport)               (sSI _x, sSI _y, sSI _w, sSI _h);
      void   (YAC_CALL *_project)                  (sF32 _x, sF32 _y, sF32 _z,
                                                    sF32 *_px, sF32 *_py, sF32 *_pz
                                                    );
      void   (YAC_CALL *_unproject)                (sF32 _x, sF32 _y,
                                                    sF32 *_nearx, sF32 *_neary, sF32 *_nearz,
                                                    sF32 *_farx, sF32 *_fary, sF32 *_farz
                                                    );
      void   (YAC_CALL *_zglBufferData)            (sSI _target, sSI _size, YAC_Object *_data, sSI _usage);
      void   (YAC_CALL *_zglBufferSubData)         (sSI _target, sSI _offset, sSI _size, YAC_Object *_data);
      void   (YAC_CALL *_zglBufferSubDataOffset)   (sSI _target, sSI _offset, sSI _size, YAC_Object *_data, sUI _dataOffset);
      void   (YAC_CALL *_zglClearColorARGB)        (sUI _argb);
      void   (YAC_CALL *_zglColorARGB)             (sUI _argb);
      void   (YAC_CALL *_zglDeleteBuffer)          (sSI _id);
      void   (YAC_CALL *_zglDeleteFramebuffer)     (sUI _id);
      void   (YAC_CALL *_zglDeleteRenderbuffer)    (sUI _id);
      void   (YAC_CALL *_zglDeleteTexture)         (sUI _id);
      void   (YAC_CALL *_zglDeleteVertexArray)     (sUI _array);
      void   (YAC_CALL *_zglEnableGLCore)          (sBool _bEnable);
      void   (YAC_CALL *_zglFogAlphaMode)          (sSI _mode);
      void   (YAC_CALL *_zglFogColor4f)            (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_zglFogDensity)            (sF32 _density);
      void   (YAC_CALL *_zglFogEnd)                (sF32 _zfar);
      void   (YAC_CALL *_zglFogMode)               (sSI _mode);
      void   (YAC_CALL *_zglFogStart)              (sF32 _znear);
      sUI    (YAC_CALL *_zglGenBuffer)             (void);
      sUI    (YAC_CALL *_zglGenFramebuffer)        (void);
      sUI    (YAC_CALL *_zglGenRenderbuffer)       (void);
      sUI    (YAC_CALL *_zglGenTexture)            (void);
      sUI    (YAC_CALL *_zglGenVertexArray)        (void);
      sSI    (YAC_CALL *_zglGetBufferParameter)    (sSI _target, sSI _value);
      void   (YAC_CALL *_zglLightAmbient4f)        (sSI _lightnr, sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_zglLightDiffuse4f)        (sSI _lightnr, sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_zglLightSpecular4f)       (sSI _lightnr, sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_zglLightPositionf)        (sSI _lightnr, sF32 _x, sF32 _y, sF32 _z, sF32 _w);
      void   (YAC_CALL *_zglLightSpotDirectionf)       (sSI _lightnr, sF32 _x, sF32 _y, sF32 _z);
      void   (YAC_CALL *_zglLightSpotExponent)         (sSI _lightnr, sF32 _exponent);
      void   (YAC_CALL *_zglLightSpotCutOff)           (sSI _lightnr, sF32 _cutoff);
      void   (YAC_CALL *_zglLightConstantAttenuation)  (sSI _lightnr, sF32 _v);
      void   (YAC_CALL *_zglLightLinearAttenuation)    (sSI _lightnr, sF32 _v);
      void   (YAC_CALL *_zglLightQuadraticAttenuation) (sSI _lightnr, sF32 _v);
      void   (YAC_CALL *_zglLightModelAmbient4f)       (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_zglLightModelLocalViewer)     (sSI _eyeorigin);
      void   (YAC_CALL *_zglLightModelTwoSide)         (sSI _enabled);
      void   (YAC_CALL *_zglLightModelColorControl)    (sSI _mode);
      void   (YAC_CALL *_zglLightMaterialAmbient4f)    (sSI _face, sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_zglLightMaterialDiffuse4f)    (sSI _face, sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_zglLightMaterialSpecular4f)   (sSI _face, sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_zglLightMaterialEmission4f)   (sSI _face, sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_zglLightMaterialShininess)    (sSI _face, sF32 _exp);
      void   (YAC_CALL *_zglLightMaterialAmbientAndDiffuse4f) (sSI _face, sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void   (YAC_CALL *_zglLoadExtensions)     (void);
      void   (YAC_CALL *_zglMapBuffer)          (sSI _target, sSI _access, YAC_Object *_buffer, sUI _byteSize);
      void   (YAC_CALL *_zglMapBufferRange)     (sSI _target, sUI _offset, sUI _length, sSI _access, YAC_Object *_buffer);
      void   (YAC_CALL *_zglPointParameter3f)   (sSI _pname, sF32 _v0, sF32 _v1, sF32 _v2);
      void   (YAC_CALL *_zglShaderSource)       (sUI _shader, YAC_Object *_source);
      void   (YAC_CALL *_zglStoreMatrixf)       (sF32 *_mat);
      void   (YAC_CALL *_zglVertexAttribOffset) (sUI _index, sUI _size, sSI _type, sSI _bNormalized, sSI _stride, sSI _offset);
   } tkopengl_shared_t;
}

#ifndef TKOPENGL_BUILD
extern tkopengl_shared_t *tkopengl_shared;
extern void tkopengl_shared_resolve(void);
#endif

#endif // TKOPENGL_SHARED_H__
