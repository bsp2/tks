/// tksopengl.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL)
///
///

#ifndef TKSOPENGL_H__
#define TKSOPENGL_H__


YG("OpenGL");


typedef void      (APIENTRY *glanyfun_t)              (void);

typedef void      (APIENTRY *glBindBuffer_t)          (GLenum, GLuint);
typedef GLboolean (APIENTRY *glIsBuffer_t)            (GLuint);
typedef void      (APIENTRY *glDeleteBuffers_t)       (GLsizei, const GLuint *);
typedef void      (APIENTRY *glGenBuffers_t)          (GLsizei, GLuint *);
typedef void      (APIENTRY *glBufferData_t)          (GLenum, GLsizei, const GLvoid *, GLenum);
typedef void      (APIENTRY *glBufferSubData_t)       (GLenum, GLsizei, GLsizei, const GLvoid *);
typedef void      (APIENTRY *glDrawArraysInstanced_t) (GLenum, GLint, GLsizei, GLsizei);
typedef void      (APIENTRY *glGetBufferParameteriv_t)(GLenum _target, GLenum _value, GLint *);
typedef void *    (APIENTRY *glMapBuffer_t)           (GLenum, GLenum);
typedef void *    (APIENTRY *glMapBufferRange_t)      (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef void      (APIENTRY *glFlushMappedBufferRange_t) (GLenum target, GLintptr offset, GLsizeiptr length);
typedef GLboolean (APIENTRY *glUnmapBuffer_t)         (GLenum);
typedef void      (APIENTRY *glTexImage2DMultisample_t) (GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void      (APIENTRY *glTexImage3D_t)          (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
typedef void      (APIENTRY *glTexImage3DMultisample_t) (GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void      (APIENTRY *glTexStorage2D_t)        (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void      (APIENTRY *glTexStorage2DMultisample_t) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void      (APIENTRY *glTexStorage3D_t)        (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
typedef void      (APIENTRY *glTexStorage3DMultisample_t) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void      (APIENTRY *glCopyTexSubImage3D_t)   (GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
typedef void      (APIENTRY *glActiveTexture_t)       (GLenum);
typedef void      (APIENTRY *glClientActiveTexture_t) (GLenum);
typedef void      (APIENTRY *glMultiTexCoord1f_t)     (GLenum, GLfloat);
typedef void      (APIENTRY *glMultiTexCoord2f_t)     (GLenum, GLfloat, GLfloat);
typedef void      (APIENTRY *glMultiTexCoord3f_t)     (GLenum, GLfloat, GLfloat, GLfloat);
typedef void      (APIENTRY *glBlendFuncSeparate_t)   (GLenum, GLenum, GLenum, GLenum);
typedef void      (APIENTRY *glBlendEquation_t)       (GLenum);
typedef void      (APIENTRY *glBlendColor_t)          (GLfloat, GLfloat, GLfloat, GLfloat);
////typedef void      (APIENTRY *glGenerateMipmap_t)      (GLenum);

// --- Framebuffer object (FBO) extensions ---
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

typedef void      (APIENTRY *glDrawBuffers_t)              (GLsizei n, const GLenum *);

// --- GLSL extensions ---
typedef GLuint    (APIENTRY *glCreateProgram_t)            (void);
typedef void      (APIENTRY *glDeleteProgram_t)            (GLuint program);
typedef GLuint    (APIENTRY *glCreateShader_t)             (GLenum shaderType);
typedef void      (APIENTRY *glDeleteShader_t)             (GLuint shader);
typedef void      (APIENTRY *glShaderSource_t)             (GLuint shader, GLsizei count, const char*const*strings, const GLint*lengths);
typedef void      (APIENTRY *glCompileShader_t)            (GLuint shader);
typedef void      (APIENTRY *glAttachShader_t)             (GLuint program, GLuint shader);
typedef void      (APIENTRY *glDetachShader_t)             (GLuint program, GLuint shader);
typedef void      (APIENTRY *glLinkProgram_t)              (GLuint program);
typedef void      (APIENTRY *glValidateProgram_t)          (GLuint program);
typedef void      (APIENTRY *glGetShaderiv_t)              (GLuint shader, GLenum pname, GLint*params);
typedef void      (APIENTRY *glGetShaderInfoLog_t)         (GLuint shader, GLsizei maxLength, GLsizei*length, char*infoLog);
typedef void      (APIENTRY *glGetProgramInfoLog_t)        (GLuint program, GLsizei maxLength, GLsizei*length, char*infoLog);
typedef void      (APIENTRY *glGetProgramiv_t)             (GLuint program, GLenum pname, GLint*params);
typedef void      (APIENTRY *glUseProgram_t)               (GLuint program);
typedef void      (APIENTRY *glBindAttribLocation_t)       (GLuint program, GLuint index, const char*name);
typedef GLint     (APIENTRY *glGetAttribLocation_t)        (GLuint program, const char*name);
typedef void      (APIENTRY *glBindFragDataLocation_t)     (GLuint program, GLuint colorNumber, const char*name);
typedef void      (APIENTRY *glBindFragDataLocationIndexed_t)(GLuint program, GLuint colorNumber, GLuint index, const char*name);
typedef GLint     (APIENTRY *glGetFragDataLocation_t)      (GLuint program, const char*name);
typedef GLint     (APIENTRY *glGetFragDataIndex_t)         (GLuint program, const char*name);
typedef void      (APIENTRY *glVertexAttrib1f_t)           (GLuint index, GLfloat v0);
typedef void      (APIENTRY *glVertexAttrib2f_t)           (GLuint index, GLfloat v0, GLfloat v1);
typedef void      (APIENTRY *glVertexAttrib2fv_t)          (GLuint index, const GLfloat *v);
typedef void      (APIENTRY *glVertexAttrib3fv_t)          (GLuint index, const GLfloat *v);
typedef void      (APIENTRY *glVertexAttrib4fv_t)          (GLuint index, const GLfloat *v);
typedef void      (APIENTRY *glVertexAttrib3f_t)           (GLuint index, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void      (APIENTRY *glVertexAttrib4f_t)           (GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void      (APIENTRY *glVertexAttrib4Nub_t)         (GLuint index, GLubyte v0, GLubyte v1, GLubyte v2, GLubyte v3);
typedef void      (APIENTRY *glVertexAttribPointer_t)      (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid*pointer);
typedef void      (APIENTRY *glEnableVertexAttribArray_t)  (GLuint index);
typedef void      (APIENTRY *glDisableVertexAttribArray_t) (GLuint index);
typedef GLint     (APIENTRY *glGetUniformLocation_t)       (GLuint program, const char*name);
typedef void      (APIENTRY *glGetActiveUniform_t)         (GLuint program, GLuint index, GLsizei bufSize, GLsizei*length, GLint*size, GLenum*type, char*name);
typedef void      (APIENTRY *glUniform1f_t)                (GLint location, GLfloat v0);
typedef void      (APIENTRY *glUniform2f_t)                (GLint location, GLfloat v0, GLfloat v1);
typedef void      (APIENTRY *glUniform3f_t)                (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void      (APIENTRY *glUniform4f_t)                (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void      (APIENTRY *glUniform2fv_t)               (GLint location, GLsizei count, const GLfloat *values);
typedef void      (APIENTRY *glUniform3fv_t)               (GLint location, GLsizei count, const GLfloat *values);
typedef void      (APIENTRY *glUniform4fv_t)               (GLint location, GLsizei count, const GLfloat *values);
typedef void      (APIENTRY *glUniform1i_t)                (GLint location, GLint v0);
typedef void      (APIENTRY *glUniform2i_t)                (GLint location, GLint v0, GLint v1);
typedef void      (APIENTRY *glUniform3i_t)                (GLint location, GLint v0, GLint v1, GLint v2);
typedef void      (APIENTRY *glUniform4i_t)                (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void      (APIENTRY *glUniformMatrix2fv_t)         (GLint location, GLsizei count, GLboolean transpose, const GLfloat*value);
typedef void      (APIENTRY *glUniformMatrix3fv_t)         (GLint location, GLsizei count, GLboolean transpose, const GLfloat*value);
typedef void      (APIENTRY *glUniformMatrix4fv_t)         (GLint location, GLsizei count, GLboolean transpose, const GLfloat*value);

//
// --- samplers --
//
typedef void      (APIENTRY *glGenSamplers_t)              (GLsizei n, GLuint *samplers);
typedef void      (APIENTRY *glDeleteSamplers_t)           (GLsizei n, const GLuint*ids);
typedef void      (APIENTRY *glBindSampler_t)              (GLuint unit, GLuint sampler);
typedef void      (APIENTRY *glSamplerParameteri_t)        (GLuint sampler, GLenum pname, GLint param);

//
// --- point sprites ---
//
typedef void      (APIENTRY *glPointParameterf_t)  (GLenum _pname, GLfloat _param);
typedef void      (APIENTRY *glPointParameteriv_t) (GLenum _pname, const GLint *_params);
typedef void      (APIENTRY *glPointParameterfv_t) (GLenum _pname, const GLfloat *_params);


//
// --- Vertex Array Objects (VAO) ---
//
typedef void      (APIENTRY *glBindVertexArray_t)    (GLuint array);
typedef void      (APIENTRY *glDeleteVertexArrays_t) (GLsizei n, const GLuint *arrays);
typedef void      (APIENTRY *glGenVertexArrays_t)    (GLsizei n, GLuint *arrays);
typedef GLboolean (APIENTRY *glIsVertexArray_t)      (GLuint array);

typedef void      (APIENTRY *glVertexAttribDivisor_t) (GLuint index, GLuint divisor);


//
// --- glGetStringi ---
//
typedef const GLubyte * (APIENTRY *glGetStringi_t) (GLenum name, GLuint index);



/* @function zglSetEnableGLCore,boolean bEnable
Enable GL 4.x core profile mode (false = compatibility).
Usually called by tksdl plugin during init (after reading TKSDL_GLCORE env var)
*/
extern sBool tkopengl_b_glcore;
YF void YAC_CALL _zglEnableGLCore (sBool _bEnable);


/* @function glActiveTexture,int texture
Select active texture unit


glActiveTexture selects which texture unit subsequent texture state calls will affect. The number of texture units an implementation supports is implementation dependent, but must be at least 2.


Vertex arrays are client-side GL resources, which are selected by the §glClientActiveTexture routine.

@arg texture Specifies which texture unit to make active. The number of texture units is implementation dependent, but must be at least two. texture must be one of GL_TEXTUREi, where i ranges from 0 to the larger of (GL_MAX_TEXTURE_COORDS - 1) and (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1). The initial value is §GL_TEXTURE0.
@see http://www.opengl.org/sdk/docs/man/html/glActiveTexture.xhtml

*/
YF void        YAC_CALL _glActiveTexture        (sSI);



/* @function glAlphaFunc,int func,float ref
Specify the alpha test function

@arg func Specifies the alpha comparison function. Symbolic constants §GL_NEVER, §GL_LESS, §GL_EQUAL, §GL_LEQUAL, §GL_GREATER, §GL_NOTEQUAL, §GL_GEQUAL, and §GL_ALWAYS are accepted. The initial value is §GL_ALWAYS.
@arg ref Specifies the reference	value that incoming alpha values are compared to. This value is clamped to the range 0	through	1, where 0 represents the lowest possible alpha value and 1 the highest possible	value. The initial reference value is 0.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glAlphaFunc.xml
*/
YF void        YAC_CALL _glAlphaFunc            (sSI, sF32);


/* @function glArrayElement,int i
Render a vertex using the specified vertex array element

@arg i Specifies an index into the enabled vertex data arrays.
@see http://www.opengl.org/sdk/docs/man2/xhtml/glArrayElement.xml
*/
YF void        YAC_CALL _glArrayElement         (sSI);


/* @function glBegin,int mode
Delimit the vertices	of a primitive or a group of like primitives

@arg mode Specifies the primitive or primitives that will be created from vertices presented between glBegin and the subsequent glEnd. Ten symbolic constants are accepted: §GL_POINTS, §GL_LINES, §GL_LINE_STRIP, §GL_LINE_LOOP, §GL_TRIANGLES, §GL_TRIANGLE_STRIP, §GL_TRIANGLE_FAN, §GL_QUADS, §GL_QUAD_STRIP, and §GL_POLYGON.
@see http://www.opengl.org/sdk/docs/man2/xhtml/glBegin.xml
@see §glEnd
*/
YF void        YAC_CALL _glBegin                (sSI);


/* @function glBindTexture,int target,int texture
Bind a named texture to a texturing target

@arg target Specifies the target	to which the texture is	bound. Must be either §GL_TEXTURE_1D or §GL_TEXTURE_2D.
@arg texture Specifies the name of a texture.
@see http://www.opengl.org/sdk/docs/man/html/glBindTexture.xhtml
*/
YF void        YAC_CALL _glBindTexture          (sSI, sUI);


/* @function zglDeleteTexture:int texture
Delete texture object.


Please notice that the §Texture class usually handles texture handling.

@arg texture Texture name-id
@see http://www.opengl.org/sdk/docs/man/html/glDeleteTextures.xhtml
*/
YF void        YAC_CALL _zglDeleteTexture      (sUI _texture);


/* @function zglGenTexture:int
Generate texture name.


Please notice that the §Texture class usually handles texture handling.

@return generated texture name
@see http://www.opengl.org/sdk/docs/man/html/glGenTextures.xhtml
*/
YF sUI         YAC_CALL _zglGenTexture        (void);


/* @function glBlendColor,float r,float g,float b,float a
Set the blend color

@arg r Red value
@arg g Green value
@arg b Blue value
@arg a Alpha value
@see http://www.opengl.org/sdk/docs/man/html/glBlendColor.xhtml
*/
YF void        YAC_CALL _glBlendColor(sF32,sF32,sF32,sF32);


/* @function glBlendEquation,int mode
Specify the equation used for both the RGB blend equation and the Alpha blend equation

@arg Specifies how source and destination colors are combined. It must be §GL_FUNC_ADD, §GL_FUNC_SUBTRACT, §GL_FUNC_REVERSE_SUBTRACT, §GL_MIN, §GL_MAX.
@see http://www.opengl.org/sdk/docs/man/html/glBlendEquation.xhtml
*/
YF void        YAC_CALL _glBlendEquation(sSI);


/* @function glBlendFunc,int sfactor,int dfactor
Specify pixel arithmetic

@arg sfactor Specifies how the red, green, blue, and alpha source blending factors are computed.  Nine symbolic constants are accepted: §GL_ZERO, §GL_ONE, §GL_DST_COLOR, §GL_ONE_MINUS_DST_COLOR, §GL_SRC_ALPHA, §GL_ONE_MINUS_SRC_ALPHA, §GL_DST_ALPHA, §GL_ONE_MINUS_DST_ALPHA, and §GL_SRC_ALPHA_SATURATE. The initial value is	§GL_ONE.
@arg dfactor Specifies how the red, green, blue, and alpha destination blending factors are computed. Eight symbolic constants are accepted: §GL_ZERO, §GL_ONE, §GL_SRC_COLOR, §GL_ONE_MINUS_SRC_COLOR, §GL_SRC_ALPHA, §GL_ONE_MINUS_SRC_ALPHA, §GL_DST_ALPHA, and §GL_ONE_MINUS_DST_ALPHA. The initial value is §GL_ZERO.
@see http://www.opengl.org/sdk/docs/man/html/glBlendFunc.xhtml
*/
YF void        YAC_CALL _glBlendFunc            (sSI, sSI);


/* @function glBlendFuncSeparate,int srcRGB,int dstRGB,int srcAlpha,int dstAlpha
Specify pixel arithmetic for RGB and alpha components separately

@arg srcRGB Specifies how the red, green, and blue blending factors are computed. The following symbolic constants are accepted: §GL_ZERO, §GL_ONE, §GL_SRC_COLOR, §GL_ONE_MINUS_SRC_COLOR, §GL_DST_COLOR, §GL_ONE_MINUS_DST_COLOR, §GL_SRC_ALPHA, §GL_ONE_MINUS_SRC_ALPHA, §GL_DST_ALPHA, §GL_ONE_MINUS_DST_ALPHA, §GL_CONSTANT_COLOR, §GL_ONE_MINUS_CONSTANT_COLOR, §GL_CONSTANT_ALPHA, §GL_ONE_MINUS_CONSTANT_ALPHA, and §GL_SRC_ALPHA_SATURATE. The initial value is §GL_ONE.
@arg dstRGB Specifies how the red, green, and blue destination blending factors are computed. The following symbolic constants are accepted: §GL_ZERO, §GL_ONE, §GL_SRC_COLOR, §GL_ONE_MINUS_SRC_COLOR, §GL_DST_COLOR, §GL_ONE_MINUS_DST_COLOR, §GL_SRC_ALPHA, §GL_ONE_MINUS_SRC_ALPHA, §GL_DST_ALPHA, §GL_ONE_MINUS_DST_ALPHA, §GL_CONSTANT_COLOR, §GL_ONE_MINUS_CONSTANT_COLOR, §GL_CONSTANT_ALPHA, and §GL_ONE_MINUS_CONSTANT_ALPHA. The initial value is §GL_ZERO.
@arg srcAlpha Specifies how the alpha source blending factor is computed. The same symbolic constants are accepted as for srcRGB. The initial value is §GL_ONE.
@arg dstAlpha Specifies how the alpha destination blending factor is computed. The same symbolic constants are accepted as for dstRGB. The initial value is §GL_ZERO.
@see http://www.opengl.org/sdk/docs/man/html/glBlendFuncSeparate.xhtml
*/
YF void        YAC_CALL _glBlendFuncSeparate    (sSI, sSI, sSI, sSI);


/* @function glCallList,int list
Execute a display list

@arg list Specifies the integer name of the display list to be executed.
@see http://www.opengl.org/sdk/docs/man2/xhtml/glCallList.xml
*/
YF void        YAC_CALL _glCallList             (sSI);


/* @function glClientActiveTexture,int texture
select active texture unit


glActiveTexture selects which texture unit subsequent texture state calls will affect. The number of texture units an implementation supports is implementation dependent, but must be at least 2.


glClientActiveTexture selects the vertex array client state parameters to be modified by glTexCoordPointer (see §zglTexCoordPointer1f, §zglTexCoordPointer2f, §zglTexCoordPointer3f), and enabled or disabled with §glEnableClientState or glDisableClientState, respectively, when called with a parameter of §GL_TEXTURE_COORD_ARRAY.


@arg texture Specifies which texture unit to make active. The number of texture units is implementation dependent, but must be at least two. texture must be one of GL_TEXTUREi, where i ranges from 0 to the larger of (GL_MAX_TEXTURE_COORDS - 1) and (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1). The initial value is §GL_TEXTURE0.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glClientActiveTexture.xml
*/
YF void        YAC_CALL _glClientActiveTexture  (sSI);


/* @function glCullFace,int mode
Specify whether front- or back-facing facets can be culled

@arg mode Specifies whether front- or back-facing	facets are candidates for culling. Symbolic constants §GL_FRONT, §GL_BACK, and §GL_FRONT_AND_BACK are accepted. The initial value is §GL_BACK.

@see http://www.opengl.org/sdk/docs/man/html/glCullFace.xhtml
*/
YF void        YAC_CALL _glCullFace             (sSI);


/* @function glClearColor,float r,float g,float b,float a
Specify clear values for the color buffers

@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man/html/glClearColor.xhtml
*/
YF void        YAC_CALL _glClearColor           (sF32, sF32, sF32, sF32);


/* @function zglClearColorARGB,int argb
Specify clear values for the color buffers as packed ARGB32 color

@arg argb packed 32bit color

@see http://www.opengl.org/sdk/docs/man/html/glClearColor.xhtml
*/
YF void        YAC_CALL _zglClearColorARGB      (sUI _argb);


/* @function glClear,int mask
Clear buffers to preset values

@arg mask Bitwise	OR of masks that indicate the buffers to be cleared. The four masks are §GL_COLOR_BUFFER_BIT, §GL_DEPTH_BUFFER_BIT, §GL_ACCUM_BUFFER_BIT, and §GL_STENCIL_BUFFER_BIT.

@see http://www.opengl.org/sdk/docs/man/html/glClear.xhtml
*/
YF void        YAC_CALL _glClear                (sSI);


/* @function glClearDepth,float depth
Specify the clear value for the depth buffer

@arg depth Specifies the depth value used when the depth buffer is cleared. The initial value is 1.

@see http://www.opengl.org/sdk/docs/man/html/glClearDepth.xhtml
*/
YF void        YAC_CALL _glClearDepth           (sF32);


/* @function glClearStencil,int s
Specify the clear value for the stencil buffer

@arg s Specifies the index used when the stencil buffer is cleared. The initial value is 0.

@see http://www.opengl.org/sdk/docs/man/html/glClearStencil.xhtml
*/
YF void        YAC_CALL _glClearStencil         (sSI);


/* @function zglColor1f,float lum
Set the current color (grayscale)

@arg lum Luminance component (replicated for red/green/blue/alpha)

@see http://www.opengl.org/sdk/docs/man2/xhtml/glColor.xml
*/
YF void        YAC_CALL _zglColor1f             (sF32);


/* @function glColor3f,float r,float g,float b
Set the current color

@arg r Red component
@arg g Green component
@arg b Blue component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glColor.xml
*/
YF void        YAC_CALL _glColor3f              (sF32, sF32, sF32);


/* @function glColor3ub,byte r,byte g,byte b
Set the current color

@arg r Red component
@arg g Green component
@arg b Blue component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glColor.xml
*/
YF void        YAC_CALL _glColor3ub             (sSI _a, sSI _b, sSI _c);


/* @function glColor4f,float r,float g,float b,float a
Set the current color

@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glColor.xml
*/
YF void        YAC_CALL _glColor4f              (sF32, sF32, sF32, sF32);


/* @function glColor4ub,byte r,byte g,byte b,byte a
Set the current color

@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glColor.xml
*/
YF void        YAC_CALL _glColor4ub             (sSI, sSI, sSI, sSI);


/* @function glColorMask,boolean r,boolean g,boolean b,boolean a
Enable and disable writing of frame buffer color components

@arg r Red mask
@arg g Green mask
@arg b Blue mask
@arg a Alpha mask

@see http://www.opengl.org/sdk/docs/man/html/glColorMask.xhtml
*/
YF void        YAC_CALL _glColorMask            (sSI, sSI, sSI, sSI);


/* @function glCopyPixels,int x,int y,int w,int h,int type
Copy pixels in the frame buffer

@arg x Specify the window x coordinate of the lower left corner of the rectangular region of pixels to be copied
@arg y Specify the window y coordinate of the lower left corner of the rectangular region of pixels to be copied
@arg w Specify the width of the rectangular region of pixels to be copied. Must be nonnegative.
@arg h Specify the height of the rectangular region of pixels to be copied. Must be nonnegative.
@arg type Specifies whether color values, depth values, or stencil values are to be	copied. Symbolic constants §GL_COLOR, §GL_DEPTH, and §GL_STENCIL are accepted.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glCopyPixels.xml
*/
YF void        YAC_CALL _glCopyPixels           (sSI, sSI, sSI, sSI, sSI);


/* @function glCopyTexSubImage2D,int target,int level,int xoffset,int yoffset,int x,int y,int width,int height
Copy a two-dimensional texture subimage

@arg target Specifies the target	texture. Must be §GL_TEXTURE_2D
@arg level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
@arg xoffset Specifies a texel offset in the x direction within the texture array.
@arg yoffset Specifies a texel offset in the y direction within the texture array.
@arg x Specify the window x coordinate of the lower left corner of the rectangular region of pixels to be copied.
@arg y Specify the window y coordinate of the lower left corner of the rectangular region of pixels to be copied.
@arg width Specifies the width of the texture subimage.
@arg height Specifies the height	of the texture subimage.

@see http://www.opengl.org/sdk/docs/man/html/glCopyTexSubImage2D.xhtml
*/
YF void        YAC_CALL _glCopyTexSubImage2D    (sSI, sSI, sSI, sSI, sSI, sSI, sSI, sSI);


/* @function glTexImage1D,int target,int level,int internalformat,int width,int border,int format,int type
Reserve memory for a 1D texture.

The pixels parameter is ignored, for now.

Also see the §Texture class.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexImage1D.xml
*/
YF void        YAC_CALL _glTexImage1D           (sSI, sSI, sSI, sUI, sSI, sSI, sSI, YAC_Object *);


/* @function glTexImage2D,int target,int level,int internalformat,int width,int height,int border,int format,int type
Reserve memory for a 2D texture.

The pixels parameter is ignored, for now.

Also see the §Texture class.

@see http://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml
*/
YF void        YAC_CALL _glTexImage2D           (sSI, sSI, sSI, sUI, sUI, sSI, sSI, sSI, YAC_Object *);


/* @function glTexImage3D,int target,int level,int internalFormat,int width,int height,int depth,int border,int format,int type,Object pixels
Specify a three-dimensional texture image

The pixels parameter is ignored, for now.

@arg target Specifies the target	texture. Must be §GL_TEXTURE_3D or §GL_PROXY_TEXTURE_3D
@arg level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
@arg internalFormat Specifies the number of color components  in  the  texture. Must be 1, 2, 3, or 4, or one of the following symbolic constants: §GL_ALPHA, §GL_ALPHA4, §GL_ALPHA8, §GL_ALPHA12, §GL_ALPHA16, §GL_LUMINANCE, §GL_LUMINANCE4, §GL_LUMINANCE8, §GL_LUMINANCE12, §GL_LUMINANCE16, §GL_LUMINANCE_ALPHA, §GL_LUMINANCE4_ALPHA4, §GL_LUMINANCE6_ALPHA2, §GL_LUMINANCE8_ALPHA8, §GL_LUMINANCE12_ALPHA4, §GL_LUMINANCE12_ALPHA12, §GL_LUMINANCE16_ALPHA16, §GL_INTENSITY, §GL_INTENSITY4, §GL_INTENSITY8, §GL_INTENSITY12, §GL_INTENSITY16, §GL_R3_G3_B2, §GL_RGB, §GL_RGB4, §GL_RGB5, §GL_RGB8, §GL_RGB10, §GL_RGB12, §GL_RGB16, §GL_RGBA, §GL_RGBA2, §GL_RGBA4, §GL_RGB5_A1, §GL_RGBA8, §GL_RGB10_A2, §GL_RGBA12, or §GL_RGBA16.
@arg width Specifies the width of the texture image.
@arg height Specifies the height	of the texture image.
@arg depth Specifies the depth of the texture image
@arg border Specifies the width of the border. Must be either 0 or 1.
@arg format Specifies the format of the pixel data. The following symbolic values are accepted: §GL_COLOR_INDEX, §GL_RED, §GL_GREEN, §GL_BLUE, §GL_ALPHA, §GL_RGB, §GL_BGR, §GL_RGBA, §GL_BGRA, §GL_LUMINANCE, and §GL_LUMINANCE_ALPHA.
@arg type Specifies the data type of the pixel data. The following symbolic  values are accepted: §GL_UNSIGNED_BYTE, §GL_BYTE, §GL_BITMAP, §GL_UNSIGNED_SHORT, §GL_SHORT, §GL_UNSIGNED_INT, §GL_INT, §GL_FLOAT, §GL_UNSIGNED_BYTE_3_3_2, §GL_UNSIGNED_BYTE_2_3_3_REV, §GL_UNSIGNED_SHORT_5_6_5, §GL_UNSIGNED_SHORT_5_6_5_REV, §GL_UNSIGNED_SHORT_4_4_4_4, §GL_UNSIGNED_SHORT_4_4_4_4_REV, §GL_UNSIGNED_SHORT_5_5_5_1, §GL_UNSIGNED_SHORT_1_5_5_5_REV, §GL_UNSIGNED_INT_8_8_8_8, §GL_UNSIGNED_INT_8_8_8_8_REV, §GL_UNSIGNED_INT_10_10_10_2, and §GL_UNSIGNED_INT_2_10_10_10_REV.
@arg pixels Currently ignored. I.e, this call can currently only be used to allocate data for render-to-texture use.

@see http://www.opengl.org/sdk/docs/man/html/glTexImage3D.xhtml
*/
YF void        YAC_CALL _glTexImage3D       (sSI, sSI, sSI, sSI, sSI, sSI, sSI, sSI, sSI, YAC_Object *);


/* @function glTexStorage2D,int target,int levels,int internalformat,int width,int height
Simultaneously specify storage for all levels of a two-dimensional texture

@see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
*/
YF void        YAC_CALL _glTexStorage2D     (sSI, sSI, sSI, sUI, sUI);


/* @function glTexStorage2DMultisample,int target,int levels,int internalformat,int width,int height,boolean fixedsamplelocations
Simultaneously specify storage for all levels of a two-dimensional texture

@see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexStorage2DMultisample.xhtml
*/
YF void        YAC_CALL _glTexStorage2DMultisample (sSI, sSI, sSI, sUI, sUI, sBool);


/* @function glTexStorage3D,int target,int levels,int internalformat,int width,int height,int depth
Simultaneously specify storage for all levels of a three-dimensional, two-dimensional array or cube-map array texture

@see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexStorage3D.xhtml
*/
YF void        YAC_CALL _glTexStorage3D     (sSI, sSI, sSI, sUI, sUI, sUI);


/* @function glTexStorage3DMultisample,int target,int levels,int internalformat,int width,int height,int depth,boolean fixedsamplelocations
Simultaneously specify storage for all levels of a two-dimensional texture

@see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexStorage2DMultisample.xhtml
*/
YF void        YAC_CALL _glTexStorage3DMultisample (sSI, sSI, sSI, sUI, sUI, sUI, sBool);


/* @function glCopyTexSubImage3D,int target,int level,int xoffset,int yoffset,int zoffset,int x,int y,int width,int height
Copy a three-dimensional texture subimage

@arg target Specifies the target	texture. Must be §GL_TEXTURE_3D or §GL_PROXY_TEXTURE_3D.
@arg level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
@arg xoffset Specifies a texel offset in the x direction within the texture array.
@arg yoffset Specifies a texel offset in the y direction within the texture array.
@arg zoffset Specifies a texel offset in the z direction within the texture array.
@arg x Specify the horizontal window coordinate of the lower left corner of the rectangular region of pixels to be copied.
@arg y Specify the vertical window coordinate of the lower left corner of the rectangular region of pixels to be copied.
@arg width Specifies the width of the texture subimage.
@arg height Specifies the height of the texture subimage.

@see http://www.opengl.org/sdk/docs/man/html/glCopyTexSubImage3D.xhtml
*/
YF void        YAC_CALL _glCopyTexSubImage3D      (sSI, sSI, sSI, sSI, sSI, sSI, sSI, sSI, sSI);


/* @function glCopyTexImage2D,int target,int level,int internalFormat,int x,int y,int width,int height,int border
Copy pixels into a 2D texture image

@arg target Specifies the	target texture. Must be GL_TEXTURE_2D.
@arg level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
@arg internalFormat Specifies the internal format of the texture. Must be one of the following symbolic constants:
           §GL_ALPHA, §GL_ALPHA4, §GL_ALPHA8, §GL_ALPHA12, §GL_ALPHA16, §GL_LUMINANCE, §GL_LUMINANCE4, §GL_LUMINANCE8, §GL_LUMINANCE12, §GL_LUMINANCE16, §GL_LUMINANCE_ALPHA, §GL_LUMINANCE4_ALPHA4, §GL_LUMINANCE6_ALPHA2, §GL_LUMINANCE8_ALPHA8, §GL_LUMINANCE12_ALPHA4, §GL_LUMINANCE12_ALPHA12, §GL_LUMINANCE16_ALPHA16, §GL_INTENSITY, §GL_INTENSITY4, §GL_INTENSITY8, §GL_INTENSITY12, §GL_INTENSITY16, §GL_RGB, §GL_R3_G3_B2, §GL_RGB4, §GL_RGB5, §GL_RGB8, §GL_RGB10, §GL_RGB12, §GL_RGB16, §GL_RGBA, §GL_RGBA2, §GL_RGBA4, §GL_RGB5_A1, §GL_RGBA8, §GL_RGB10_A2, §GL_RGBA12, or §GL_RGBA16.
@arg x Specify the window x coordinate of the lower left corner of the rectangular region of pixels to be copied.
@arg y Specify the window y coordinate of the lower left corner of the rectangular region of pixels to be copied.
@arg width Specifies the width of the texture image. Must be 0 or 2**n + 2*border for some integer n.
@arg height Specifies the height of the texture image. Must be 0 or 2**m + 2*border for some integer m.
@arg border Specifies the width of the border. Must be either 0 or 1.

@see http://www.opengl.org/sdk/docs/man/html/glCopyTexImage2D.xhtml
*/
YF void        YAC_CALL _glCopyTexImage2D       (sSI, sSI, sSI, sSI, sSI, sSI, sSI, sSI);


/* @function glDepthFunc,int func
Specify the value used for depth buffer comparisons

@arg func Specifies the depth comparison function. Symbolic constants §GL_NEVER, §GL_LESS, §GL_EQUAL, §GL_LEQUAL, §GL_GREATER, §GL_NOTEQUAL, §GL_GEQUAL, and §GL_ALWAYS are accepted. The initial value is §GL_LESS.

@see http://www.opengl.org/sdk/docs/man/html/glDepthFunc.xhtml
*/
YF void        YAC_CALL _glDepthFunc            (sSI);


/* @function glDepthMask,boolean mask
Enable or disable writing into the depth buffer

@arg mask Specifies whether the depth buffer is enabled for writing. If flag is false, depth buffer writing is disabled. Otherwise, it is enabled. Initially, depth buffer writing is enabled.

@see http://www.opengl.org/sdk/docs/man/html/glDepthMask.xhtml
*/
YF void        YAC_CALL _glDepthMask            (sSI);


/* @function glDisable,int cap
Disable server-side GL capabilities

@arg cap Specifies a symbolic constant indicating a GL capability.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glDisable.xml
*/
YF void        YAC_CALL _glDisable              (sSI);


/* @function glDisableClientState,int cap
Disable client-side capability

@arg Specifies the capability to disable.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glDisableClientState.xml
*/
YF void        YAC_CALL _glDisableClientState   (sSI);


/* @function glDrawArrays,int mode,int first,int count
Render primitives from array data

@arg mode Specifies what kind of primitives to render. Symbolic constants §GL_POINTS, §GL_LINE_STRIP, §GL_LINE_LOOP, §GL_LINES, §GL_TRIANGLE_STRIP, §GL_TRIANGLE_FAN, §GL_TRIANGLES, §GL_QUAD_STRIP, §GL_QUADS, and §GL_POLYGON are accepted.
@arg first Specifies the starting index in the enabled arrays.
@arg count Specifies the number of indices to be rendered.

@see http://www.opengl.org/sdk/docs/man/html/glDrawArrays.xhtml
*/
YF void        YAC_CALL _glDrawArrays           (sSI, sSI, sSI);


/* @function glDrawArraysInstanced,int mode,int first,int count,int instancecount
Draw multiple instances of a range of elements

@arg mode Specifies what kind of primitives to render. Symbolic constants §GL_POINTS, §GL_LINE_STRIP, §GL_LINE_LOOP, §GL_LINES, §GL_TRIANGLE_STRIP, §GL_TRIANGLE_FAN, §GL_TRIANGLES, §GL_QUAD_STRIP, §GL_QUADS, and §GL_POLYGON are accepted.
@arg first Specifies the starting index in the enabled arrays.
@arg count Specifies the number of indices to be rendered.
@arg instancecount Specifies the number of instances of the specified range of indices to be rendered

@see http://www.opengl.org/sdk/docs/man/html/glDrawArrays.xhtml
*/
YF void        YAC_CALL _glDrawArraysInstanced  (sSI, sSI, sSI, sSI);


/* @function glDrawElements,int mode,IntArray indices
Render primitives from array data

@arg mode Specifies what kind of primitives to render. Symbolic constants §GL_POINTS, §GL_LINE_STRIP, §GL_LINE_LOOP, §GL_LINES, §GL_TRIANGLE_STRIP, §GL_TRIANGLE_FAN, §GL_TRIANGLES, §GL_QUAD_STRIP, §GL_QUADS, and §GL_POLYGON are accepted.
@arg indices Specifies a pointer (and count) to the location where the indices are stored

@see http://www.opengl.org/sdk/docs/man/html/glDrawElements.xhtml
*/
YF void        YAC_CALL _glDrawElements         (sSI, YAC_Object *_indices);


/* @function zglDrawElementsOffset,int mode,int count,int type,int offset
Render primitives from currently bound index buffer data

@arg mode Specifies what kind of primitives to render. Symbolic constants §GL_POINTS, §GL_LINE_STRIP, §GL_LINE_LOOP, §GL_LINES, §GL_TRIANGLE_STRIP, §GL_TRIANGLE_FAN, §GL_TRIANGLES, §GL_QUAD_STRIP, §GL_QUADS, and §GL_POLYGON are accepted.
@arg count Number of indices
@arg type Index type (e.g. §GL_UNSIGNED_INT)
@arg offset Buffer offset (bytes)

@see http://www.opengl.org/sdk/docs/man/html/glDrawElements.xhtml
*/
YF void        YAC_CALL _zglDrawElementsOffset  (sSI _mode, sUI _count, sSI _type, sUI _offset);


/* @function glDrawBuffer,int mode
Specify which color buffers are to be drawn into

@arg mode Specifies up to four color buffers to be drawn into. Symbolic constants §GL_NONE, §GL_FRONT_LEFT, §GL_FRONT_RIGHT, §GL_BACK_LEFT, §GL_BACK_RIGHT, §GL_FRONT, §GL_BACK, §GL_LEFT, §GL_RIGHT, §GL_FRONT_AND_BACK, and GL_AUXi, where i is between 0 and GL_AUX_BUFFERS-1, are accepted (GL_AUX_BUFFERS is not the upper limit; use glGet to query the number of	available aux buffers.) The initial value is	§GL_FRONT for single-buffered contexts, and §GL_BACK for double-buffered contexts.

@see http://www.opengl.org/sdk/docs/man/html/glDrawBuffer.xhtml
*/
YF void        YAC_CALL _glDrawBuffer           (sSI);


/* @function glDrawBuffers,int n,IntArray a
Specifies a list of color buffers to be drawn into

@arg n Specifies the number of buffers in bufs.
@arg a Array of symbolic constants specifying the buffers into which fragment colors or data values will be written. The symbolic constants contained in
 bufs may be any of the following §GL_NONE, §GL_FRONT_LEFT, §GL_FRONT_RIGHT, §GL_BACK_LEFT, §GL_BACK_RIGHT, GL_AUXi

@see http://www.opengl.org/sdk/docs/man/html/glDrawBuffers.xhtml
*/
YF void        YAC_CALL _glDrawBuffers          (sUI, YAC_Object *_ia);


/* @function glEdgeFlag,boolean flag
Flag edges as either boundary or nonboundary

@arg flag Specifies the current edge flag value, either true or false. The initial value is true.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glEdgeFlag.xml
*/
YF void        YAC_CALL _glEdgeFlag             (sSI);


/* @function glEnable,int cap
Enable server-side GL capabilities

@arg cap Specifies a symbolic constant indicating a GL capability.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glEnable.xml
*/
YF void        YAC_CALL _glEnable               (sSI);


/* @function glIsEnabled,int cap:int
Test whether a capability is enabled

@arg cap Specifies a symbolic constant indicating a GL capability.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glIsEnabled.xml
*/
YF sSI         YAC_CALL _glIsEnabled            (sSI);


/* @function glEnableClientState,int cap
Enable client-side capability

@arg cap Specifies the capability to enable. Symbolic constants §GL_COLOR_ARRAY, §GL_EDGE_FLAG_ARRAY, §GL_INDEX_ARRAY, §GL_NORMAL_ARRAY, §GL_TEXTURE_COORD_ARRAY, and §GL_VERTEX_ARRAY are accepted.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glEnableClientState.xml
*/
YF void        YAC_CALL _glEnableClientState    (sSI);


/* @function glEnd
Delimit the vertices of a primitive or a group of like primitives

@see §glBegin
@see http://www.opengl.org/sdk/docs/man2/xhtml/glEnd.xml
*/
YF void        YAC_CALL _glEnd                  (void);


/* @function glFinish
Block until all GL execution is complete

@see http://www.opengl.org/sdk/docs/man/html/glFinish.xhtml
*/
YF void        YAC_CALL _glFinish               (void);


/* @function glFlush
Force execution of GL commands in finite time

@see http://www.opengl.org/sdk/docs/man/html/glFlush.xhtml
*/
YF void        YAC_CALL _glFlush                (void);


/* @function glFrontFace,int mode
Define front- and back-facing polygons

@arg mode Specifies the orientation of front-facing polygons. §GL_CW and §GL_CCW are accepted. The initial value is §GL_CCW.

@see http://www.opengl.org/sdk/docs/man/html/glFrontFace.xhtml
*/
YF void        YAC_CALL _glFrontFace            (sSI);


/* @function glFrustum,float left,float right,float bottom,float top,float znear,float zfar
Multiply the current matrix by a perspective matrix

@arg left Specify the coordinate for the left vertical clipping plane.
@arg right Specify the coordinate for the right vertical clipping plane.
@arg bottom Specify the coordinate for the bottom horizontal clipping plane.
@arg top Specify the coordinate for the top horizontal clipping plane.
@arg znear Specify the distances to the near depth clipping plane. The distance must be positive.
@arg zfar Specify the distances to the far depth clipping plane. The distance must be positive.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
*/
YF void        YAC_CALL _glFrustum              (sF32, sF32, sF32, sF32, sF32, sF32);


/* @function glHint,int target,int mode
Specify implementation-specific hints

@arg target Specifies a symbolic constant indicating the behavior to be controlled. §GL_FOG_HINT, §GL_LINE_SMOOTH_HINT, §GL_PERSPECTIVE_CORRECTION_HINT, §GL_POINT_SMOOTH_HINT, and §GL_POLYGON_SMOOTH_HINT are accepted.
@arg mode Specifies a symbolic constant indicating the desired behavior. §GL_FASTEST, §GL_NICEST, and §GL_DONT_CARE are accepted.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glHint.xml
*/
YF void        YAC_CALL _glHint                 (sSI, sSI);


/* @function glLoadIdentity
Replace the current matrix with the identity matrix

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLoadIdentity.xml
*/
YF void        YAC_CALL _glLoadIdentity         (void);


/* @function glLineWidth,float width
Specify the width of rasterized lines

@arg width Specifies the width of rasterized lines. The initial value is 1.

@see http://www.opengl.org/sdk/docs/man/html/glLineWidth.xhtml
*/
YF void        YAC_CALL _glLineWidth            (sF32);


/* @function glLineStipple,int factor,short pattern
Specify the line stipple pattern

@arg factor Specifies a multiplier for each bit in the line stipple pattern. If factor is 3, for example, each bit in the pattern is used three times before the next bit in the pattern is used. factor is clamped to the range	[1, 256] and defaults to 1.
@arg pattern Specifies a 16-bit integer whose bit pattern determines which fragments of a line will be drawn when	the line is rasterized. Bit zero is used first; the default pattern is all 1's.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLineStipple.xml
*/
YF void        YAC_CALL _glLineStipple          (sSI, sSI);


/* @function glLogicOp,int opcode
Specify a logical pixel operation for color index rendering

@arg opcode Specifies a symbolic constant	that selects a logical operation. The following symbols are accepted: §GL_CLEAR, §GL_SET, §GL_COPY, §GL_COPY_INVERTED, §GL_NOOP, §GL_INVERT, §GL_AND, §GL_NAND, §GL_OR, §GL_NOR, §GL_XOR, §GL_EQUIV, §GL_AND_REVERSE, §GL_AND_INVERTED, §GL_OR_REVERSE, and §GL_OR_INVERTED. The initial value is §GL_COPY.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLogicOp.xml
*/
YF void        YAC_CALL _glLogicOp              (sSI);


/* @function glMatrixMode,int mode
Specify which matrix is the current matrix

@arg mode Specifies which matrix stack is	the target for subsequent matrix operations. Three values are accepted: §GL_MODELVIEW, §GL_PROJECTION, and §GL_TEXTURE. The initial value is §GL_MODELVIEW.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMatrixMode.xml
*/
YF void        YAC_CALL _glMatrixMode           (sSI);


/* @function glMultiTexCoord1f,int texture,float s
Set the current texture coordinate

@arg texture Specifies the texture unit whose coordinates should be modified. The number of texture units is implementation dependent, but must be at least two. Symbolic constant must be one of GL_TEXTUREi, where i ranges from 0 to GL_MAX_TEXTURE_COORDS - 1, which is an implementation-dependent value.
@arg s Specify the s texture coordinate

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMultiTexCoord.xml
*/
YF void        YAC_CALL _glMultiTexCoord1f      (sSI, sF32);


/* @function glMultiTexCoord2f,int texture,float s,float t
Set the current texture coordinates

@arg texture Specifies the texture unit whose coordinates should be modified. The number of texture units is implementation dependent, but must be at least two. Symbolic constant must be one of GL_TEXTUREi, where i ranges from 0 to GL_MAX_TEXTURE_COORDS - 1, which is an implementation-dependent value.
@arg s Specify the s texture coordinate
@arg t Specify the t texture coordinate

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMultiTexCoord.xml
*/
YF void        YAC_CALL _glMultiTexCoord2f      (sSI, sF32, sF32);


/* @function glMultiTexCoord3f,int texture,float s,float t,float r
Set the current texture coordinates

@arg texture Specifies the texture unit whose coordinates should be modified. The number of texture units is implementation dependent, but must be at least two. Symbolic constant must be one of GL_TEXTUREi, where i ranges from 0 to GL_MAX_TEXTURE_COORDS - 1, which is an implementation-dependent value.
@arg s Specify the s texture coordinate
@arg t Specify the t texture coordinate
@arg r Specify the r texture coordinate

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMultiTexCoord.xml
*/
YF void        YAC_CALL _glMultiTexCoord3f      (sSI, sF32, sF32, sF32);



/* @function glNormal3f,float nx,float ny,float nz
Set the current normal vector

@arg nx Specify the x coordinate of the new current normal
@arg ny Specify the y coordinate of the new current normal
@arg nz Specify the z coordinate of the new current normal

@see http://www.opengl.org/sdk/docs/man2/xhtml/glNormal.xml
*/
YF void        YAC_CALL _glNormal3f             (sF32, sF32, sF32);


/* @function glOrtho,float left,float right,float bottom,float top,float znear,float zfar
Multiply the current matrix with an orthographic matrix

@arg left Specify the coordinate for the left vertical clipping plane.
@arg right Specify the coordinate for the right vertical clipping plane.
@arg bottom Specify the coordinate for the bottom horizontal clipping plane.
@arg top Specify the coordinate for the top horizontal clipping plane.
@arg znear Specify the distance to the near depth clipping planes. These values are negative if the plane is to be behind the viewer.
@arg zfar Specify the distance to the far depth clipping planes. These values are negative if the plane is to be behind the viewer.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml
*/
YF void        YAC_CALL _glOrtho                (sF32, sF32, sF32, sF32, sF32, sF32);


/* @function glPointSize,float size
Specify the diameter of rasterized points

@arg size Specifies the diameter of rasterized points. The initial value is 1.

@see http://www.opengl.org/sdk/docs/man/html/glPointSize.xhtml
*/
YF void        YAC_CALL _glPointSize            (sF32);


/* @function glPointParameterf,int pname,float param
Specify point parameter

@arg pname
Specifies a single-valued point parameter.
§GL_POINT_SIZE_MIN, §GL_POINT_SIZE_MAX, §GL_POINT_FADE_THRESHOLD_SIZE, and §GL_POINT_SPRITE_COORD_ORIGIN are accepted.

@arg param Specifies the value to be assigned to pname

@see http://www.opengl.org/sdk/docs/man2/xhtml/glPointParameter.xml
*/
YF void        YAC_CALL _glPointParameterf      (sSI, sF32);


/* @function zglPointParameter3f,int pname,float param
Specify point parameters

@arg pname §GL_POINT_DISTANCE_ATTENUATION is accepted

@arg param Specifies the values to be assigned to pname

@see http://www.opengl.org/sdk/docs/man2/xhtml/glPointParameter.xml
*/
YF void        YAC_CALL _zglPointParameter3f    (sSI, sF32, sF32, sF32);


/* @function glPixelZoom,float xfactor,float yfactor
Specify the pixel zoom factors

@arg xfactor Specify the x zoom factor for pixel write operations.
@arg yfactor Specify the y zoom factor for pixel write operations.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glPixelZoom.xml
*/
YF void        YAC_CALL _glPixelZoom            (sF32, sF32);


/* @function glPolygonMode,int face,int mode
Select a polygon rasterization mode

@arg face Specifies the polygons that mode applies to. Must be §GL_FRONT for front-facing polygons, §GL_BACK for back-facing polygons, or §GL_FRONT_AND_BACK for front- and back-facing polygons.
@arg mode Specifies how polygons will be rasterized. Accepted values are §GL_POINT, §GL_LINE, and §GL_FILL. The initial	value is §GL_FILL for both front- and back-facing polygons.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glPolygonMode.xml
*/
YF void        YAC_CALL _glPolygonMode          (sSI, sSI);


/* @function glPolygonOffset,float factor,float units
Set the scale and units used to calculate depth values

@arg factor  Specifies a scale factor that is used to create a variable depth offset for each polygon. The initial value is 0.
@arg units Is multiplied by an implementation-specific value to create a constant depth offset. The initial value is 0.

@see http://www.opengl.org/sdk/docs/man/html/glPolygonOffset.xhtml
*/
YF void        YAC_CALL _glPolygonOffset        (sF32, sF32);


/* @function glPopAttrib
Pop the server attribute stack

@see http://www.opengl.org/sdk/docs/man2/xhtml/glPopAttrib.xml
*/
YF void        YAC_CALL _glPopAttrib            (void);


/* @function glPushAttrib,int mask
Push the server attribute stack

@see http://www.opengl.org/sdk/docs/man2/xhtml/glPushAttrib.xml
*/
YF void        YAC_CALL _glPushAttrib           (sUI);


/* @function glPopClientAttrib
Pop the client attribute stack

@see http://www.opengl.org/sdk/docs/man2/xhtml/glPopClientAttrib.xml
*/
YF void        YAC_CALL _glPopClientAttrib      (void);


/* @function glPushClientAttrib,int mask
Push the client attribute stack

@see http://www.opengl.org/sdk/docs/man2/xhtml/glPushClientAttrib.xml
*/
YF void        YAC_CALL _glPushClientAttrib     (sUI);


/* @function glPopMatrix
Pop the current matrix stack

@see http://www.opengl.org/sdk/docs/man2/xhtml/glPopMatrix.xml
*/
YF void        YAC_CALL _glPopMatrix            (void);


/* @function glPushMatrix
Push the current matrix stack

@see http://www.opengl.org/sdk/docs/man2/xhtml/glPushMatrix.xml
*/
YF void        YAC_CALL _glPushMatrix           (void);


/* @function glRasterPos2i,int x,int y
Specify the raster position for pixel operations

@arg x Specify the x object coordinate for the raster position.
@arg y Specify the y object coordinate for the raster position.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glRasterPos.xml
*/
YF void        YAC_CALL _glRasterPos2i          (sSI, sSI);


/* @function glRasterPos3f,float x,float y,float z
Specify the raster position for pixel operations

@arg x Specify the x object coordinate for the raster position.
@arg y Specify the y object coordinate for the raster position.
@arg z Specify the z object coordinate for the raster position.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glRasterPos.xml
*/
YF void        YAC_CALL _glRasterPos3f          (sF32, sF32, sF32);


/* @function glReadBuffer,int mode
Select a color buffer source for pixels

@arg mode Specifies a color buffer. Accepted values are §GL_FRONT_LEFT, §GL_FRONT_RIGHT, §GL_BACK_LEFT, §GL_BACK_RIGHT, §GL_FRONT, §GL_BACK, §GL_LEFT, §GL_RIGHT, and GL_AUXi, where i is between 0 and §GL_AUX_BUFFERS-1.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glReadBuffer.xml
*/
YF void        YAC_CALL _glReadBuffer           (sSI);


/* @function glRectf,float x1,float y1,float x2,float y2
Draw a rectangle

@arg x1 Specify one vertex of a rectangle.
@arg y1 Specify one vertex of a rectangle.
@arg x2 Specify the opposite vertex of	the rectangle.
@arg y2 Specify the opposite vertex of	the rectangle.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glRect.xml
*/
YF void        YAC_CALL _glRectf                (sF32, sF32, sF32, sF32);


/* @function glRotatef,float angle,float x,float y,float z
Multiply the current matrix by a rotation matrix

@arg angle Specifies the angle of rotation, in degrees.
@arg x Specify the x coordinate of a vector, respectively.
@arg y Specify the y coordinate of a vector, respectively.
@arg z Specify the z coordinate of a vector, respectively.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glRotate.xml
*/
YF void        YAC_CALL _glRotatef              (sF32, sF32, sF32, sF32);


/* @function glScalef,float x,float y,float z
Multiply the current matrix by a general scaling matrix

@arg x Specify scale factor along the x axis
@arg y Specify scale factor along the y axis
@arg z Specify scale factor along the z axis

@see http://www.opengl.org/sdk/docs/man2/xhtml/glScale.xml
*/
YF void        YAC_CALL _glScalef               (sF32, sF32, sF32);


/* @function glScissor,int x,int y,int width,int height
Define the scissor box

@arg x Specify the x coordinate of the lower left corner of the scissor box. Initially (0, 0)
@arg y Specify the y coordinate of the lower left corner of the scissor box. Initially (0, 0)
@arg width Specify the width of the scissor box
@arg height Specify the height of the scissor box

@see http://www.opengl.org/sdk/docs/man/html/glScissor.xhtml
*/
YF void        YAC_CALL _glScissor              (sSI, sSI, sSI, sSI);


/* @function glShadeModel,int mode
Select flat or smooth shading

@arg mode Specifies a symbolic value representing a shading technique. Accepted values are §GL_FLAT and §GL_SMOOTH. The initial value is §GL_SMOOTH.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glShadeModel.xml
*/
YF void        YAC_CALL _glShadeModel           (sSI);


/* @function glStencilFunc,int func,float ref,int mask
Set function and reference value for stencil testing

@arg func Specifies the test function. Eight tokens are valid: §GL_NEVER, §GL_LESS, §GL_LEQUAL, §GL_GREATER, §GL_GEQUAL, §GL_EQUAL, §GL_NOTEQUAL, and §GL_ALWAYS. The initial value is §GL_ALWAYS.
@arg ref Specifies the reference	value for the stencil test. ref is clamped to the range [0,2n-1], where n is the number of bitplanes in the stencil buffer. The initial value is 0.
@arg mask Specifies a mask that is ANDed with both the reference value and the stored stencil value when	the test is done. The initial value	is all 1's.

@see http://www.opengl.org/sdk/docs/man/html/glStencilFunc.xhtml
*/
YF void        YAC_CALL _glStencilFunc          (sSI, sSI, sUI);


/* @function glStencilOp,int fail,int zfail,int zpass
Set stencil test actions

@arg fail Specifies the action to take when the stencil test fails. Six symbolic constants are accepted: §GL_KEEP, §GL_ZERO, §GL_REPLACE, §GL_INCR, §GL_DECR, and §GL_INVERT. The initial value is §GL_KEEP.
@arg zfail Specifies the stencil action when the stencil test passes, but the depth test fails. zfail accepts the same symbolic constants as fail. The initial value is §GL_KEEP.
@arg zpass Specifies the stencil action when both the stencil test and the depth test pass, or when the stencil test passes and either	there is no depth buffer or depth testing is not enabled. zpass accepts the same symbolic constants as fail. The initial value is §GL_KEEP.

@see http://www.opengl.org/sdk/docs/man/html/glStencilOp.xhtml
*/
YF void        YAC_CALL _glStencilOp            (sSI, sSI, sSI);


/* @function glStencilMask,int mask
Control the writing of individual bits in the stencil planes

@arg mask Specifies a bit mask to enable and disable writing of individual bits in the stencil planes. Initially, the mask is	all 1's.

@see http://www.opengl.org/sdk/docs/man/html/glStencilMask.xhtml
*/
YF void        YAC_CALL _glStencilMask          (sUI);


/* @function glTexSubImage2D,int target,int level,int xoffset,int yoffset,int width,int height,int format,int type,int offset
Specify a two-dimensional texture subimage.

Please notice that texture uploads are usually handled in the §Texture class.
This function is merely useful when working with OpenGL 2.1 pixel buffer objects (PBO).

@arg target Specifies the target texture. Must be §GL_TEXTURE_2D.
@arg level Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
@arg xoffset Specifies a texel offset in the x direction within the texture array.
@arg yoffset Specifies a texel offset in the y direction within the texture array.
@arg width Specifies the width of the texture subimage.
@arg height Specifies the height of the texture subimage.
@arg Specifies the format of the pixel data. The following symbolic values are accepted: §GL_COLOR_INDEX, §GL_RED, §GL_GREEN, §GL_BLUE, §GL_ALPHA, §GL_RGB, §GL_BGR, §GL_RGBA, §GL_BGRA, §GL_LUMINANCE, and §GL_LUMINANCE_ALPHA.

@see http://www.opengl.org/sdk/docs/man/html/glTexSubImage2D.xhtml
*/
YF void        YAC_CALL _glTexSubImage2D(sSI, sSI, sSI, sSI, sSI, sSI, sSI, sSI, sSI);


/* @function glTexEnvf,int target,int pname,int param
Set texture environment parameters

@arg target Specifies a texture environment. Must be §GL_TEXTURE_ENV, §GL_TEXTURE_FILTER_CONTROL or §GL_POINT_SPRITE.
@arg pname Specifies the symbolic name of a single-valued texture environment parameter. Must be §GL_TEXTURE_ENV_MODE, §GL_TEXTURE_LOD_BIAS, §GL_COMBINE_RGB, §GL_COMBINE_ALPHA, §GL_SRC0_RGB, §GL_SRC1_RGB, §GL_SRC2_RGB, §GL_SRC0_ALPHA, §GL_SRC1_ALPHA, §GL_SRC2_ALPHA, §GL_OPERAND0_RGB, §GL_OPERAND1_RGB, §GL_OPERAND2_RGB, §GL_OPERAND0_ALPHA, §GL_OPERAND1_ALPHA, §GL_OPERAND2_ALPHA, §GL_RGB_SCALE, §GL_ALPHA_SCALE, or §GL_COORD_REPLACE
@arg param Specifies a single symbolic constant, one of §GL_ADD, §GL_ADD_SIGNED, §GL_INTERPOLATE, §GL_MODULATE, §GL_DECAL, §GL_BLEND, §GL_REPLACE, §GL_SUBTRACT, §GL_COMBINE, §GL_TEXTURE, §GL_CONSTANT, §GL_PRIMARY_COLOR, §GL_PREVIOUS, §GL_SRC_COLOR, §GL_ONE_MINUS_SRC_COLOR, §GL_SRC_ALPHA, §GL_ONE_MINUS_SRC_ALPHA, a single boolean value for the point sprite texture coordinate replacement, a single floating-point value for the texture level-of-detail bias, or 1.0, 2.0, or 4.0 when specifying the §GL_RGB_SCALE or §GL_ALPHA_SCALE.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml
*/
YF void        YAC_CALL _glTexEnvf(sSI, sSI, sF32);


/* @function zglTexEnvColor4f,float r,float g,float b,float a
Set texture environment color

@arg r Red value
@arg g Green value
@arg b Blue value
@arg a Alpha value

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml
*/
YF void        YAC_CALL _zglTexEnvColor4f(sF32, sF32, sF32, sF32);


/* @function glTexEnvi,int target,int pname,int param
Set texture environment parameters

@arg target Specifies a texture environment. Must be §GL_TEXTURE_ENV, §GL_TEXTURE_FILTER_CONTROL or §GL_POINT_SPRITE.
@arg pname Specifies the symbolic name of a single-valued texture environment parameter. Must be §GL_TEXTURE_ENV_MODE, §GL_TEXTURE_LOD_BIAS, §GL_COMBINE_RGB, §GL_COMBINE_ALPHA, §GL_SRC0_RGB, §GL_SRC1_RGB, §GL_SRC2_RGB, §GL_SRC0_ALPHA, §GL_SRC1_ALPHA, §GL_SRC2_ALPHA, §GL_OPERAND0_RGB, §GL_OPERAND1_RGB, §GL_OPERAND2_RGB, §GL_OPERAND0_ALPHA, §GL_OPERAND1_ALPHA, §GL_OPERAND2_ALPHA, §GL_RGB_SCALE, §GL_ALPHA_SCALE, or §GL_COORD_REPLACE
@arg param Specifies a single symbolic constant, one of §GL_ADD, §GL_ADD_SIGNED, §GL_INTERPOLATE, §GL_MODULATE, §GL_DECAL, §GL_BLEND, §GL_REPLACE, §GL_SUBTRACT, §GL_COMBINE, §GL_TEXTURE, §GL_CONSTANT, §GL_PRIMARY_COLOR, §GL_PREVIOUS, §GL_SRC_COLOR, §GL_ONE_MINUS_SRC_COLOR, §GL_SRC_ALPHA, §GL_ONE_MINUS_SRC_ALPHA, a single boolean value for the point sprite texture coordinate replacement, a single floating-point value for the texture level-of-detail bias, or 1.0, 2.0, or 4.0 when specifying the §GL_RGB_SCALE or §GL_ALPHA_SCALE.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml
*/
YF void        YAC_CALL _glTexEnvi(sSI, sSI, sSI);


/* @function glTexParameteri,int target,int pname,int param
Set texture parameters.


Note: For some obscure(?) reason, omitting this call before uploading a texture will cause GL to silently fail and simply render foreground-color filled triangles. (The §Texture class takes care of this automatically).

@arg target Specifies the target texture, which must be either §GL_TEXTURE_1D, §GL_TEXTURE_2D, §GL_TEXTURE_3D or §GL_TEXTURE_CUBE_MAP.
@arg pname Specifies the symbolic name of a single-valued texture parameter. pname can be one of the following: §GL_TEXTURE_MIN_FILTER, §GL_TEXTURE_MAG_FILTER, §GL_TEXTURE_MIN_LOD, §GL_TEXTURE_MAX_LOD, §GL_TEXTURE_BASE_LEVEL, §GL_TEXTURE_MAX_LEVEL, §GL_TEXTURE_WRAP_S, §GL_TEXTURE_WRAP_T, §GL_TEXTURE_WRAP_R, or §GL_TEXTURE_PRIORITY.
@arg Specifies the value of pname.

@see http://www.opengl.org/sdk/docs/man/html/glTexParameter.xhtml
*/
YF void        YAC_CALL _glTexParameteri(sSI, sSI, sSI);



/* @function glTexCoord1f,float s
Set the current texture coordinate

@arg s Specify the s texture coordinate

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoord.xml
*/
YF void        YAC_CALL _glTexCoord1f           (sF32);


/* @function glTexCoord2f,float s,float t
Set the current texture coordinates

@arg s Specify the s texture coordinate
@arg t Specify the t texture coordinate

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoord.xml
*/
YF void        YAC_CALL _glTexCoord2f           (sF32, sF32);


/* @function zglTexCoord2v,Object vec2
Set the current texture coordinates

@arg vec2 Array-type object (e.g. a Vector2f)

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoord.xml
*/
YF void        YAC_CALL _zglTexCoord2v          (YAC_Object *);


/* @function glTexCoord3f,float s,float t,float r
Set the current texture coordinates

@arg s Specify the s texture coordinate
@arg t Specify the t texture coordinate
@arg r Specify the r texture coordinate

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoord.xml
*/
YF void        YAC_CALL _glTexCoord3f           (sF32, sF32, sF32);


/* @function zglTexCoord3v,Object vec3
Set the current texture coordinates

@arg vec3 Array-type object (e.g. a Vector3f)

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoord.xml
*/
YF void        YAC_CALL _zglTexCoord3v          (YAC_Object *);


/* @function glTexCoord4f,float s,float t,float r,float q
Set the current texture coordinates

@arg s Specify the s texture coordinate
@arg t Specify the t texture coordinate
@arg r Specify the r texture coordinate
@arg q Specify the q texture coordinate

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoord.xml
*/
YF void        YAC_CALL _glTexCoord4f           (sF32, sF32, sF32, sF32);


/* @function zglTexCoord4v,Object vec4
Set the current texture coordinates

@arg vec4 Array-type object (e.g. a Vector4f)

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoord.xml
*/
YF void        YAC_CALL _zglTexCoord4v          (YAC_Object *);


/* @function glTexCoord1i,int s
Set the current texture coordinate

@arg s Specify the s texture coordinate

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoord.xml
*/
YF void        YAC_CALL _glTexCoord1i           (sSI); // xxx


/* @function glTexCoord2i,int s,int t
Set the current texture coordinates

@arg s Specify the s texture coordinate
@arg t Specify the t texture coordinate

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoord.xml
*/
YF void        YAC_CALL _glTexCoord2i           (sSI, sSI); // XXX


/* @function glTexGeni,int coord,int pname,int param
Control the generation of texture coordinates

@arg coord Specifies a texture coordinate. Must	be one of GL_S, GL_T, GL_R, or GL_Q.
@arg pname Specifies the symbolic name of the texture-coordinate generation function. Must be §GL_TEXTURE_GEN_MODE.
@arg param Specifies a single-valued texture generation parameter, one of §GL_OBJECT_LINEAR, §GL_EYE_LINEAR, or §GL_SPHERE_MAP.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexGen.xml
*/
YF void        YAC_CALL _glTexGeni              (sSI, sSI, sSI);


/* @function zglTexGen4f,int coord,int pname,float p1,float p2,float p3,float p4
Control the generation of texture coordinates

@arg coord Specifies a texture coordinate. Must	be one of GL_S, GL_T, GL_R, or GL_Q.
@arg pname Specifies the symbolic name of the texture-coordinate generation function. Must be §GL_OBJECT_PLANE, or §GL_EYE_PLANE.
@arg p1 First coefficient for the texture-coordinate generation function specified by pname.
@arg p2 Second coefficient for the texture-coordinate generation function specified by pname.
@arg p3 Third coefficient for the texture-coordinate generation function specified by pname.
@arg p4 Fourth coefficient for the texture-coordinate generation function specified by pname.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexGen.xml
*/
YF void YAC_CALL _zglTexGen4f(sSI _a, sSI _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f);


/* @function glTranslatef,float x,float y,float z
Multiply	the current matrix by a translation matrix

@arg x Specify the x coordinate of a translation vector
@arg y Specify the y coordinate of a translation vector
@arg z Specify the z coordinate of a translation vector

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTranslate.xml
*/
YF void        YAC_CALL _glTranslatef           (sF32, sF32, sF32);


/* @function glVertex2i,int x,int y
Specify a vertex

@arg x Specify x coordinate of a vertex
@arg y Specify y coordinate of a vertex

@see http://www.opengl.org/sdk/docs/man2/xhtml/glVertex.xml
*/
YF void        YAC_CALL _glVertex2i             (sSI, sSI);


/* @function glVertex2f,float x,float y
Specify a vertex

@arg x Specify x coordinate of a vertex
@arg y Specify y coordinate of a vertex

@see http://www.opengl.org/sdk/docs/man2/xhtml/glVertex.xml
*/
YF void        YAC_CALL _glVertex2f             (sF32, sF32);


/* @function zglVertex2v,Object vec2
Specify a vertex

@arg vec2 Array-type object (e.g. a Vector2f)

@see http://www.opengl.org/sdk/docs/man2/xhtml/glVertex.xml
*/
YF void        YAC_CALL _zglVertex2v            (YAC_Object *_v);


/* @function glVertex3f,float x,float y,float z
Specify a vertex

@arg x Specify x coordinate of a vertex
@arg y Specify y coordinate of a vertex
@arg z Specify z coordinate of a vertex

@see http://www.opengl.org/sdk/docs/man2/xhtml/glVertex.xml
*/
YF void        YAC_CALL _glVertex3f             (sF32, sF32, sF32);


/* @function zglVertex3v,Object vec3
Specify a vertex

@arg vec3 Array-type object (e.g. a Vector3f)

@see http://www.opengl.org/sdk/docs/man2/xhtml/glVertex.xml
*/
YF void        YAC_CALL _zglVertex3v            (YAC_Object *_v);


/* @function glVertex4f,float x,float y,float z,float w
Specify a vertex

@arg x Specify x coordinate of a vertex
@arg y Specify y coordinate of a vertex
@arg z Specify z coordinate of a vertex
@arg w Specify w coordinate of a vertex

@see http://www.opengl.org/sdk/docs/man2/xhtml/glVertex.xml
*/
YF void        YAC_CALL _glVertex4f             (sF32, sF32, sF32, sF32);


/* @function zglVertex4v,Object vec4
Specify a vertex

@arg vec4 Array-type object (e.g. a Vector4f)

@see http://www.opengl.org/sdk/docs/man2/xhtml/glVertex.xml
*/
YF void        YAC_CALL _zglVertex4v            (YAC_Object *_v);



/* @function glViewport,int x,int y,int width,int height
Set the viewport

@arg x Specify the x coordinate of the lower left corner of the viewport rectangle, in pixels. The initial value is (0,0).
@arg y Specify the y coordinate of the lower left corner of the viewport rectangle, in pixels. The initial value is (0,0).
@arg width Specify the width of the viewport. When a GL context is first attached to a window, width is set to the dimension of that window.
@arg height Specify the height of the viewport. When a GL context is first attached to a window, height is set to the dimension of that window.

@see http://www.opengl.org/sdk/docs/man/html/glViewport.xhtml
*/
YF void        YAC_CALL _glViewport             (sSI, sSI, sSI, sSI);


/* @function zglInitPerspective,float aspect,float fov,float znear,float zfar
Load perspective projection matrix

@arg aspect Aspect ratio
@arg fov Field of view (1..180)
@arg znear Distance to near clipping plane
@arg zfar Distance to far clipping plane

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMatrixMode.xml
@see http://www.opengl.org/sdk/docs/man2/xhtml/glLoadIdentity.xml
@see http://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
*/
YF void        YAC_CALL _zglInitPerspective     (sF32, sF32, sF32, sF32);


/* @function zglInitOrtho,float w,float h
Load orthogonal projection matrix

(0; 0) is located at the center of the screen

@arg w Quadrant width
@arg h Quadrant height

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMatrixMode.xml
@see http://www.opengl.org/sdk/docs/man2/xhtml/glLoadIdentity.xml
@see http://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml
*/
YF void        YAC_CALL _zglInitOrtho           (sF32, sF32);


/* @function zglInit2D,float w,float h
Load 2D projection matrix

(0; 0) is located in the top/left corner of the screen

@arg w Width
@arg h Height

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMatrixMode.xml
@see http://www.opengl.org/sdk/docs/man2/xhtml/glLoadIdentity.xml
@see http://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml
*/
YF void        YAC_CALL _zglInit2D              (sF32, sF32);


/* @function zgluLookAt,Object eye,Object center,Object up
Define a viewing transformation


You may pass objects that support the yac array interface (e.g. §FloatArray, §ListNode, §tkmath.Vector3f and so on).

@arg eye Eye vector (3 elements)
@arg center Center vector (3 elements)
@arg up Up vector (3 elements)

@see http://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
*/
YF void        YAC_CALL _zgluLookAt             (YAC_Object *, YAC_Object *, YAC_Object *);

////YF void        YAC_CALL _zglScalev              (YAC_Object *);

////YF void        YAC_CALL _zglTranslatefv         (YAC_Object *);
////YF void        YAC_CALL _zglRotatev             (YAC_Object *);
////YF void        YAC_CALL _zglNormalv             (YAC_Object *);
////YF void        YAC_CALL _zglVertexv             (YAC_Object *);
////YF void        YAC_CALL _zglRotate3f            (sF32, sF32, sF32);
////YF void        YAC_CALL _zglLoadMatrix          (YAC_Object *, YAC_Object *);
////YF void        YAC_CALL _zglStoreMatrix         (YAC_Object *, YAC_Object *);


/* @function glLoadMatrixf,Object array
Replace the current matrix with the specified matrix

@arg array Must hold at least 16 elements (column major layout)

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLoadMatrix.xml
*/
YF void        YAC_CALL _glLoadMatrixf          (YAC_Object *_fa);


/* @function zglLoadMatrixTransposef,Object array
Replace the current matrix with the specified matrix (transpose row major to OpenGL column major)

@arg array Must hold at least 16 elements (row major layout)

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLoadMatrix.xml
*/
YF void        YAC_CALL _zglLoadMatrixTransposef (YAC_Object *_fa);


/* @function zglFrustum,float left,float right,float bottom,float top,float znear,float zfar
Multiply the current matrix by a perspective matrix

@arg left Specify the coordinate of the left vertical clipping plane
@arg right Specify the coordinate of the right vertical clipping plane
@arg bottom Specify the coordinate of the bottom horizontal clipping plane
@arg top Specify the coordinate of the top horizontal clipping plane
@arg znear Specify the distance to the near depth clipping plane
@arg zfar Specify the distance to the far depth clipping plane

@see http://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
*/
YF void        YAC_CALL _zglFrustum             (sF32, sF32, sF32, sF32);


/* @function glColorMaterial,int face,int mode
Specify which material parameters track the current color

@arg face GL_FRONT, GL_BACK, or GL_FRONT_AND_BACK
@arg mode GL_EMISSION, GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_AMBIENT_AND_DIFFUSE*

@see http://www.opengl.org/sdk/docs/man2/xhtml/glColorMaterial.xml
*/
YF void        YAC_CALL _glColorMaterial        (sSI, sSI);


/* @function zglLightAmbient4f,int light,float r,float g,float b,float a
Set light source ambient color parameter

@arg light GL_LIGHTi
@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLight.xml
*/
YF void        YAC_CALL _zglLightAmbient4f      (sSI, sF32, sF32, sF32, sF32);


/* @function zglLightDiffuse4f,int light,float r,float g,float b,float a
Set light source diffuse color parameter

@arg light GL_LIGHTi
@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLight.xml
*/
YF void        YAC_CALL _zglLightDiffuse4f      (sSI, sF32, sF32, sF32, sF32);


/* @function zglLightSpecular4f,int light,float r,float g,float b,float a
Set light source specular color parameter

@arg light GL_LIGHTi
@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLight.xml
*/
YF void        YAC_CALL _zglLightSpecular4f     (sSI, sF32, sF32, sF32, sF32);


/* @function zglLightPositionf,int light,float x,float y,float z,float w
Set light source position

@arg light GL_LIGHTi
@arg x X position of light in homogeneous object coordinates
@arg y Y position of light in homogeneous object coordinates
@arg z Z position of light in homogeneous object coordinates
@arg w W position of light in homogeneous object coordinates

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLight.xml
*/
YF void        YAC_CALL _zglLightPositionf      (sSI, sF32, sF32, sF32, sF32);


////YF void        YAC_CALL _zglLightPositionv      (sSI, YAC_Object *, sF32);


/* @function zglLightSpotDirectionf,int light,float x,float y,float z
Set spotlight direction

@arg light GL_LIGHTi
@arg x X direction of spotlight in homogeneous object coordinates
@arg y Y direction of spotlight in homogeneous object coordinates
@arg z Z direction of spotlight in homogeneous object coordinates

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLight.xml
*/
YF void        YAC_CALL _zglLightSpotDirectionf (sSI, sF32, sF32, sF32);


////YF void        YAC_CALL _zglLightSpotDirectionv (sSI, YAC_Object *);


/* @function zglLightSpotExponent,int light,float exp
Set spotlight exponent

@arg light GL_LIGHTi
@arg exp 0..127

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLight.xml
*/
YF void        YAC_CALL _zglLightSpotExponent   (sSI, sF32);


/* @function zglLightSpotCutOff,int light,float cutoff
Set spotlight cutoff

@arg light GL_LIGHTi
@arg cutoff 0..90, 180

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLight.xml
*/
YF void        YAC_CALL _zglLightSpotCutOff     (sSI, sF32);


/* @function zglLightConstantAttenuation,int light,float factor
Set light source constant attenuation

@arg light GL_LIGHTi
@arg factor Constant attenuation factor

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLight.xml
*/
YF void        YAC_CALL _zglLightConstantAttenuation (sSI, sF32);


/* @function zglLightLinearAttenuation,int light,float factor
Set light source linear attenuation

@arg light GL_LIGHTi
@arg factor Linear attenuation factor

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLight.xml
*/
YF void        YAC_CALL _zglLightLinearAttenuation   (sSI, sF32);


/* @function zglLightQuadraticAttenuation,int light,float factor
Set light source quadratic attenuation

@arg light GL_LIGHTi
@arg factor Quadratic attenuation factor

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLight.xml
*/
YF void        YAC_CALL _zglLightQuadraticAttenuation(sSI, sF32);


/* @function zglLightModelAmbient4f,float r,float g,float b,float a
Set ambient scene color

@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLightModel.xml
*/
YF void        YAC_CALL _zglLightModelAmbient4f      (sF32, sF32, sF32, sF32);


/* @function glLightModeli,int pname,int param
Configure light model

@arg pname Specifies a single-valued lighting model parameter. §GL_LIGHT_MODEL_LOCAL_VIEWER, §GL_LIGHT_MODEL_COLOR_CONTROL, and §GL_LIGHT_MODEL_TWO_SIDE are accepted.
@arg param 1(true) or (false) resp. §GL_SINGLE_COLOR or §GL_SEPARATE_SPECULAR_COLOR if pname is §GL_LIGHT_MODEL_COLOR_CONTROL.

@see §zglLightModelLocalViewer, §zglLightModelColorControl, §zglLightModelTwoSide

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLightModel.xml
*/
YF void        YAC_CALL _glLightModeli               (sSI _pname, sSI _param);


/* @function zglLightModelLocalViewer,boolean param
Configure local viewer light model

@arg param If param is 0 (or 0.0), specular reflection angles take the view direction to be parallel to and in the direction of the -z axis, regardless of the location of the vertex in eye coordinates. Otherwise, specular reflections are computed from the origin of the eye coordinate system. The initial value is 0.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLightModel.xml
*/
YF void        YAC_CALL _zglLightModelLocalViewer    (sSI);


/* @function zglLightModelTwoSide,boolean param
Configure two sided light model

@arg param If params is 0 (or 0.0), one-sided lighting is specified, and only the front material parameters are used in the lighting equation. Otherwise, two-sided lighting is specified

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLightModel.xml
*/
YF void        YAC_CALL _zglLightModelTwoSide        (sSI);


/* @function zglLightModelColorControl,int mode
Configure single color / separate specular color lighting mode.

When §GL_SEPARATE_SPECULAR_COLOR mode is selected, the specular component will be applied (added) after texture mapping.

@arg param §GL_SINGLE_COLOR or §GL_SEPARATE_SPECULAR_COLOR

@see http://www.opengl.org/sdk/docs/man2/xhtml/glLightModel.xml
*/
YF void        YAC_CALL _zglLightModelColorControl   (sSI);


/* @function zglLightMaterialAmbient4f,int face,float r,float g,float b,float a
Set ambient material color

@arg face Specifies which face or faces are being updated. Must be one of §GL_FRONT, §GL_BACK, or §GL_FRONT_AND_BACK.
@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMaterial.xml
*/
YF void        YAC_CALL _zglLightMaterialAmbient4f   (sSI, sF32, sF32, sF32, sF32);


/* @function zglLightMaterialDiffuse4f,int face,float r,float g,float b,float a
Set diffuse material color

@arg face Specifies which face or faces are being updated. Must be one of §GL_FRONT, §GL_BACK, or §GL_FRONT_AND_BACK.
@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMaterial.xml
*/
YF void        YAC_CALL _zglLightMaterialDiffuse4f   (sSI, sF32, sF32, sF32, sF32);


/* @function zglLightMaterialSpecular4f,int face,float r,float g,float b,float a
Set specular material color

@arg face Specifies which face or faces are being updated. Must be one of §GL_FRONT, §GL_BACK, or §GL_FRONT_AND_BACK.
@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMaterial.xml
*/
YF void        YAC_CALL _zglLightMaterialSpecular4f  (sSI, sF32, sF32, sF32, sF32);


/* @function zglLightMaterialEmission4f,int face,float r,float g,float b,float a
Set emissive material color

@arg face Specifies which face or faces are being updated. Must be one of §GL_FRONT, §GL_BACK, or §GL_FRONT_AND_BACK.
@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMaterial.xml
*/
YF void        YAC_CALL _zglLightMaterialEmission4f  (sSI, sF32, sF32, sF32, sF32);


/* @function zglLightMaterialShininess,int face,float exp
Set material shininess

@arg face Specifies which face or faces are being updated. Must be one of §GL_FRONT, §GL_BACK, or §GL_FRONT_AND_BACK.
@arg exp 0..127

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMaterial.xml
*/
YF void        YAC_CALL _zglLightMaterialShininess   (sSI, sF32);


/* @function zglLightMaterialAmbientAndDiffuse4f,int face,float r,float g,float b,float a
Set both ambient and diffuse material colors

@arg face Specifies which face or faces are being updated. Must be one of §GL_FRONT, §GL_BACK, or §GL_FRONT_AND_BACK.
@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glMaterial.xml
*/
YF void        YAC_CALL _zglLightMaterialAmbientAndDiffuse4f(sSI, sF32, sF32, sF32, sF32);


/* @function zglFogMode,int mode
Specifies the equation to be used to compute the fog blend factor

@arg mode Three symbolic constants are accepted: §GL_LINEAR, §GL_EXP, and §GL_EXP2. The initial fog mode is §GL_EXP.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glFog.xml
*/
YF void        YAC_CALL _zglFogMode             (sSI);


/* @function zglFogAlphaMode,int mode
Specifies the equation to be used to compute the fog alpha color.

This is an extension to the regular OpenGL fog feature which is only available if tkopengl is compiled in emulation mode (DX_EMU_FOG / DX_EMU_FIXFXN).

When drawing transparent objects, §GL_BLEND can be used to fade-out primitives (if fog alpha is 0.0) resp. §GL_MULT alpha mode can be used to fade to an alpha relative to the primitive alpha (out_alpha = mix(frag_alpha*fog_alpha, frag_alpha, fog_amount)).

@arg mode Three symbolic constants are accepted: §GL_REPLACE (OpenGL behaviour, discard fogcolor.alpha, blend to fogcolor RGB and keep primitive alpha), §GL_BLEND (full fog color+alpha blending), and §GL_MULT (blend to fog alpha multiplied by primitive alpha). The initial fog alpha mode is §GL_REPLACE.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glFog.xml
*/
YF void        YAC_CALL _zglFogAlphaMode        (sSI _mode);


/* @function zglFogDensity,float param
Specify the fog density

@arg param Only nonnegative densities are accepted. The initial fog density is 1.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glFog.xml
*/
YF void        YAC_CALL _zglFogDensity          (sF32);


/* @function zglFogStart,float param
Specify fog near distance

@arg param Fog near distance. The initial near distance is 0.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glFog.xml
*/
YF void        YAC_CALL _zglFogStart            (sF32 _znear);


/* @function zglFogEnd,float param
Specify fog far distance

@arg param Fog far distance. The initial far distance is 1.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glFog.xml
*/
YF void        YAC_CALL _zglFogEnd              (sF32 _zfar);


/* @function zglFogColor4f,float r,float g,float b,float a
Set fog color


Note: OpenGL actually ignores the alpha component which means that fogging can NOT be used to fade out objects.

@arg r Red component
@arg g Green component
@arg b Blue component
@arg a Alpha component

@see http://www.opengl.org/sdk/docs/man2/xhtml/glFog.xml
*/
YF void        YAC_CALL _zglFogColor4f          (sF32, sF32, sF32, sF32);


/* @function zglColorARGB,int argb32
Set the current color

@arg argb32 Packed 32bit ARGB32 color

@see http://www.opengl.org/sdk/docs/man2/xhtml/glColor.xml
*/
YF void        YAC_CALL _zglColorARGB           (sUI);


/* @function zglVertexPointer,Object array
Define an array of vertex data

Note: Stride is assumed to be 0, type is GL_FLOAT and size is 3.

@arg array Specifies the pointer to the vertex data

@see http://www.opengl.org/sdk/docs/man2/xhtml/glVertexPointer.xml
*/
YF void        YAC_CALL _zglVertexPointer       (YAC_Object *);


/* @function zglVertexPointer2f,Object array
Define an array of vertex data

Note: Stride is assumed to be 0, type is GL_FLOAT and size is 2.

@arg array Specifies the pointer to the vertex data

@see http://www.opengl.org/sdk/docs/man2/xhtml/glVertexPointer.xml
*/
YF void        YAC_CALL _zglVertexPointer2f     (YAC_Object *);


/* @function zglVertexOffset,int size,int type,int stride,int offset
Set vertex pointer offset within currently bound buffer object

@arg size Specifies the number of coordinates per vertex; must be 2, 3, or 4.
@arg type Specifies the data type of each coordinate in the array. Symbolic constants §GL_SHORT, §GL_INT, §GL_FLOAT, or §GL_DOUBLE are accepted.
@arg stride Specifies the byte offset between consecutive vertices. If stride is 0, the vertices are understood to be tightly packed in the array.
@arg offset Specifies the offset to the first coordinate of the first vertex in the array.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glVertexPointer.xml
*/
YF void        YAC_CALL _zglVertexOffset        (sUI _size, sSI _type, sUI _stride, sUI _offset);


/* @function zglNormalPointer,Object array
Define an array of normal data

Note: Stride is assumed to be 0 and type is GL_FLOAT.

@arg array Specifies the pointer to the normal data

@see http://www.opengl.org/sdk/docs/man2/xhtml/glNormalPointer.xml
*/
YF void        YAC_CALL _zglNormalPointer       (YAC_Object *);


/* @function zglNormalOffset,int type,int stride,int offset
Set normal pointer offset within currently bound buffer object

@arg type Specifies the data type of each coordinate in the array. Symbolic constants §GL_BYTE, §GL_SHORT, §GL_INT, §GL_FLOAT, or §GL_DOUBLE are accepted.
@arg stride Specifies the byte offset between consecutive normals. If stride is 0, the normals are understood to be tightly packed in the array.
@arg offset Specifies the offset to the first coordinate of the first normal in the array.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glNormalPointer.xml
*/
YF void        YAC_CALL _zglNormalOffset        (sSI _type, sUI _stride, sUI _offset);


/* @function zglColorPointer3f,Object array
Define an array of RGB color data

Note: Stride is assumed to be 0 and type is GL_FLOAT.

@arg array Specifies the pointer to the color data

@see http://www.opengl.org/sdk/docs/man2/xhtml/glColorPointer.xml
*/
YF void        YAC_CALL _zglColorPointer3f      (YAC_Object *);


/* @function zglColorOffset,int size,int type,int stride,int offset
Set color pointer offset within currently bound buffer object

@arg size Specifies the number of components per color; must 3 or 4.
@arg type Specifies the data type of each color component in the array. Symbolic constants §GL_BYTE, §GL_UNSIGNED_BYTE, §GL_SHORT, §GL_UNSIGNED_SHORT, §GL_INT, §GL_UNSIGNED_INT, §GL_FLOAT, or §GL_DOUBLE are accepted.
@arg stride Specifies the byte offset between consecutive colors. If stride is zero (the initial value), the colors are understood to be tightly packed in the array.
@arg offset Specifies the offset to the first component of the first color element in the array.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glColorPointer.xml
*/
YF void        YAC_CALL _zglColorOffset         (sUI _size, sSI _type, sUI _stride, sUI _offset);


/* @function zglColorPointer4f,Object array
Define an array of RGBA color data

Note: Stride is assumed to be 0 and type is GL_FLOAT.

@arg array Specifies the pointer to the color data

@see http://www.opengl.org/sdk/docs/man2/xhtml/glColorPointer.xml
*/
YF void        YAC_CALL _zglColorPointer4f      (YAC_Object *);


/* @function zglColorPointer4b,Object array
Define an array of RGBA color data

Note: Stride is assumed to be 0 and type is GL_UNSIGNED_BYTE.

@arg array Specifies the pointer to the color data

@see http://www.opengl.org/sdk/docs/man2/xhtml/glColorPointer.xml
*/
YF void        YAC_CALL _zglColorPointer4b      (YAC_Object *);


/* @function zglTexCoordPointer1f,Object array
Define an array of texture coordinate data

Note: Stride is assumed to be 0 and type is GL_FLOAT.

@arg array Specifies the pointer to the texture coordinate data

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoordPointer.xml
*/
YF void        YAC_CALL _zglTexCoordPointer1f   (YAC_Object *);


/* @function zglTexCoordPointer2f,Object array
Define an array of texture coordinate data

Note: Stride is assumed to be 0 and type is GL_FLOAT.

@arg array Specifies the pointer to the texture coordinate data

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoordPointer.xml
*/
YF void        YAC_CALL _zglTexCoordPointer2f   (YAC_Object *);


/* @function zglTexCoordOffset,int size,int type,int stride,int offset
Set texture coordinate pointer offset within currently bound buffer object

@arg size Specifies the number of coordinates per array element. Must be 1, 2, 3 or 4
@arg type  Specifies the data type of each texture coordinate. Symbolic constants §GL_SHORT, §GL_INT, §GL_FLOAT, or §GL_DOUBLE are accepted.
@arg stride Specifies the byte offset between consecutive array elements. If stride is 0, the array elements are understood to be tightly packed.
@arg offset Specifies an offset to the first coordinate of the first element in the array.

@see http://www.opengl.org/sdk/docs/man2/xhtml/glTexCoordPointer.xml
*/
YF void        YAC_CALL _zglTexCoordOffset      (sUI _size, sSI _type, sUI _stride, sUI _offset);


/* @function zglReadColorPixel,int x,int y:int
Read a single pixel from the frame buffer

@arg x Specify the window x coordinate of the pixel that is read from the frame buffer.
@arg y Specify the window y coordinate of the pixel that is read from the frame buffer.
@return Pixel (GL_RGBA in native host byte order)

@see http://www.opengl.org/sdk/docs/man/html/glReadPixels.xhtml
*/
YF sUI         YAC_CALL _zglReadColorPixel      (sSI, sSI);


/* @function zglReadColorPixelsX,Object tex,int off,int x, int y,int sx
Read a line of pixels from the frame buffer

@arg tex Specify the target texture (or array)
@arg off Specify the texture pixel write offset
@arg x Specify the window x coordinate of the pixel that is read from the frame buffer.
@arg y Specify the window y coordinate of the pixel that is read from the frame buffer.
@arg sx Specify the number of pixels to read

@see http://www.opengl.org/sdk/docs/man/html/glReadPixels.xhtml
*/
YF void        YAC_CALL _zglReadColorPixelsX  (YAC_Object *_tex, sUI _off, sUI _x, sUI _y, sUI _sx);


/* @function zglReadDepthPixel,int x,int y:float
Read a single depth value from the depth buffer

@arg x Specify the window x coordinate of the pixel that is read from the frame buffer.
@arg y Specify the window y coordinate of the pixel that is read from the frame buffer.
@return Depth value

@see http://www.opengl.org/sdk/docs/man/html/glReadPixels.xhtml
*/
YF sF32        YAC_CALL _zglReadDepthPixel      (sSI, sSI);


/* @function zglClipPlane,int plane,float eq0,float eq1,float eq2,float eq3
Specify a plane against which all geometry is clipped

@arg plane Specifies which clipping plane is being positioned. Symbolic names of the form GL_CLIP_PLANEi, where i is an integer between 0 and GL_MAX_CLIP_PLANES -1, are accepted.
@arg eq0
@arg eq1
@arg eq2
@arg eq3

@see http://www.opengl.org/sdk/docs/man2/xhtml/glClipPlane.xml
*/
YF void        YAC_CALL _zglClipPlane           (sSI _i, sF32 _eq0, sF32 _eq1, sF32 _eq2, sF32 _eq3);


/* @function glGetString,int name:String
Return a string describing the current GL connection

@arg name Specifies a symbolic constant, one of §GL_VENDOR, §GL_RENDERER, §GL_VERSION, or §GL_EXTENSIONS.
@return String

@see http://www.opengl.org/sdk/docs/man/html/glGetString.xhtml
*/
YF void        YAC_CALL _glGetString            (sSI _type, YAC_Value *_r);


/* @function zglGetFloat,int name:float
Return the value of a selected parameter

@arg name Specifies a symbolic constant
@return current parameter value

@see http://www.opengl.org/sdk/docs/man/html/glGet.xhtml
*/
YF sF32        YAC_CALL _zglGetFloat            (sSI _type);


/* @function glGetFloatv,int name,FloatArray ret
Use this for queries that return a float vector.

@see http://www.opengl.org/sdk/docs/man/html/glGet.xhtml
*/
YF void        YAC_CALL _glGetFloatv            (sSI _type, YAC_Object *_faRet);


/* @function zglGetInteger,int name:int
Return the value of a selected parameter

@arg name Specifies a symbolic constant, e.g. §GL_MAX_TEXTURE_UNITS, §GL_MAX_TEXTURE_COORDS, §GL_MAX_ELEMENTS_VERTICES, §GL_MAX_ELEMENTS_INDICES, §GL_ACTIVE_TEXTURE, §GL_CLIENT_ACTIVE_TEXTURE. Note: Not all GL parameter names are exported in the tkopengl plugin but as a workaround you can still pass the raw hex value (see gl.h, glext.h)
@return current parameter value

@see http://www.opengl.org/sdk/docs/man/html/glGet.xhtml
*/
YF sSI         YAC_CALL _zglGetInteger          (sSI _type);


/* @function glGetIntegerv,int name,IntArray ret
Use this for queries that return an integer vector.

@see http://www.opengl.org/sdk/docs/man/html/glGet.xhtml
*/
YF void        YAC_CALL _glGetIntegerv          (sSI _type, YAC_Object *_iaRet);


/* @function glGetError:int
Return last GL error code and reset internal error state to GL_NO_ERROR.

@return OpenGL error code

@see http://www.opengl.org/sdk/docs/man/html/glGetError.xhtml
*/
YF sSI        YAC_CALL _glGetError              (void);


/* @function zglGenBuffer:int
Generate buffer object name id

@return Buffer object name id

@see http://www.opengl.org/sdk/docs/man/html/glGenBuffers.xhtml
*/
YF sUI        YAC_CALL _zglGenBuffer            (void);


/* @function zglDeleteBuffer,int id
Delete buffer object

@arg id Buffer object name id

@see http://www.opengl.org/sdk/docs/man/html/glDeleteBuffers.xhtml
*/
YF void        YAC_CALL _zglDeleteBuffer        (sSI _id);


/* @function zglBufferData,int target,int size,Object data,int usage
Creates and initializes a buffer object's data store

@arg target Specifies the target buffer object. The symbolic constant must be §GL_ARRAY_BUFFER or §GL_ELEMENT_ARRAY_BUFFER.
@arg size Initialize buffer size in bytes. If size is < 0, the size will be set to data.numElements*sizeof(float).
@data Specifies a pointer to data that will be copied into the data store for initialization, or null if no data is to be copied.
@arg usage Specifies the expected usage pattern of the data store. The symbolic constant must be GL_STATIC_DRAW or GL_DYNAMIC_DRAW.

@see http://www.opengl.org/sdk/docs/man/html/glBufferData.xhtml
*/
YF void        YAC_CALL _zglBufferData          (sSI _target, sSI _size, YAC_Object *_data, sSI _usage);
void _glBufferData(GLenum, GLsizei, const GLvoid *, GLenum);


/* @function zglBufferSubData,int target,int offset,int size,Object data
Updates a subset of a buffer object's data store.

@arg target Specifies the target buffer object. The symbolic constant must be §GL_ARRAY_BUFFER or §GL_ELEMENT_ARRAY_BUFFER.
@arg offset Specifies the offset into the buffer object's data store where data replacement will begin, measured in bytes.
@arg size Specifies the size in bytes of the data store region being replaced.
@data Specifies a pointer to data that will be copied into the data store.

@see http://www.opengl.org/sdk/docs/man/html/glBufferSubData.xhtml
*/
YF void        YAC_CALL _zglBufferSubData       (sSI _target, sSI _offset, sSI _size, YAC_Object *_data);
YF void        YAC_CALL _zglBufferSubDataOffset (sSI _target, sSI _offset, sSI _size, YAC_Object *_data, sUI _dataOffset);


/* @function glBindBuffer,int target,int id
Bind a named buffer to a target

While a non-zero buffer object is bound to the §GL_ARRAY_BUFFER target, the vertex array pointer parameter that is traditionally interpreted as a pointer to client-side memory is instead interpreted as an offset within the buffer object measured in basic machine units.

While a non-zero buffer object is bound to the §GL_ELEMENT_ARRAY_BUFFER target, the indices parameter of §glDrawElements that is traditionally interpreted as a pointer to client-side memory is instead interpreted as an offset within the buffer object measured in basic machine units.

@arg target Specifies the target buffer object. The symbolic constant must be §GL_ARRAY_BUFFER, §GL_ELEMENT_ARRAY_BUFFER or §GL_PIXEL_UNPACK_BUFFER.
@arg id Specifies the name id of a buffer object.

@see http://www.opengl.org/sdk/docs/man/html/glBindBuffer.xhtml
*/
YF void        YAC_CALL _glBindBuffer           (sSI _target, sUI _id);


/* @function zglGetBufferParameter,int target,int id:int
Return parameters of a buffer object via glGetBufferParameteriv()
@arg target GL_ARRAY_BUFFER, ..
@arg value  GL_BUFFER_SIZE, ..
@return buffer parameter
*/
YF sSI         YAC_CALL _zglGetBufferParameter  (sSI _target, sSI _value);


/* @function glIsBuffer,int id:boolean
Query whether the given name id is bound to a buffer object

@arg id Specifies the name id of a buffer object.
@return 1(true) if the name id represents a buffer object, 0(false) otherwise.

@see http://www.opengl.org/sdk/docs/man/html/glIsBuffer.xhtml
*/
YF sSI         YAC_CALL _glIsBuffer             (sSI _id);


/* @function zglMapBuffer,int target,int access,Buffer buffer,int byteSize
Map a buffer object's data store

@arg target Specifies the target buffer object being mapped. The symbolic constant must be §GL_ARRAY_BUFFER, §GL_ELEMENT_ARRAY_BUFFER, §GL_PIXEL_PACK_BUFFER, or §GL_PIXEL_UNPACK_BUFFER.
@arg access Specifies the access policy, indicating whether it will be possible to read from, write to, or both read from and write to the buffer object's mapped data store. The symbolic constant must be §GL_READ_ONLY, §GL_WRITE_ONLY, or §GL_READ_WRITE.
@arg buffer Returns the data store address mapped to the client's address space. The old buffer contents,if any, will be freed prior to setting the new buffer pointer and size.
@arg byteSize Number of bytes. This should be the same size that was passed to §zglBufferData. Note: leaving this up to the application can result in access to unmapped memory. Be careful.

@see http://www.opengl.org/sdk/docs/man/html/glMapBuffer.xhtml
*/
YF void        YAC_CALL _zglMapBuffer(sSI _target, sSI _access, YAC_Object *_buffer, sUI _byteSize);


/* @function zglMapBufferRange,int target,int offset,int length,int access,Buffer buffer
Map a buffer object's data store

@arg target Specifies the target to which the buffer object is bound. (e.g. §GL_ARRAY_BUFFER)
@arg offset Specifies the starting offset within the buffer of the range to be mapped.
@arg length Specifies the length of the range to be mapped.
@arg access Specifies a combination of access flags indicating the desired access to the mapped range. GL_MAP_READ_BIT, GL_MAP_WRITE_BIT, GL_MAP_PERSISTENT_BIT, GL_MAP_COHERENT_BIT (if not set, app. must call glFlushMappedBufferRange()), GL_MAP_INVALIDATE_RANGE_BIT, GL_MAP_INVALIDATE_BUFFER_BIT, GL_MAP_FLUSH_EXPLICIT_BIT, GL_MAP_UNSYNCHRONIZED_BIT)
@arg buffer Returns the data store address mapped to the client's address space. The old buffer contents,if any, will be freed prior to setting the new buffer pointer and size.
*/
YF void YAC_CALL _zglMapBufferRange(sSI _target, sUI _offset, sUI _length, sSI _access, YAC_Object *_buffer);
YF void YAC_CALL _glFlushMappedBufferRange (sSI _target, sUI _offset, sUI _length);

/* @function glUnmapBuffer,int target:boolean
Unmap buffer object

@arg target Specifies the target buffer object being unmapped. The symbolic constant must be §GL_ARRAY_BUFFER, §GL_ELEMENT_ARRAY_BUFFER, §GL_PIXEL_PACK_BUFFER, or §GL_PIXEL_UNPACK_BUFFER.
@return true if the buffer has been unmapped, false otherwise

@see http://www.opengl.org/sdk/docs/man/html/glMapBuffer.xhtml
*/
YF sSI         YAC_CALL _glUnmapBuffer(sSI _target);


/* @function zglLoadExtensions
Resolve OpenGL extensions.

Must be called after window/screen has been openend if the application wishes to use >v1.1 GL functions (e.g. buffer objects, see §zglGenBuffer)

(note) tkopengl v0.9.9.10+ auto-loads the extensions when a viewport/window is opened

*/
YF void        YAC_CALL _zglLoadExtensions      (void);



//
//
// Framebuffer objects (FBO)
//
//

/* @function glIsRenderbuffer,int renderbuffer:boolean
Determine if a name corresponds to a renderbuffer object

@arg renderbuffer Specifies a value that may be the name of a renderbuffer object

@see http://www.opengl.org/sdk/docs/man/html/glIsRenderbuffer.xhtml
 */
YF sSI         YAC_CALL _glIsRenderbuffer       (sUI _renderbuffer);


/* @function glBindRenderbuffer,int target,int renderbuffer
Bind a renderbuffer to a renderbuffer target

@arg target Specifies the renderbuffer target of the binding operation. target must be §GL_RENDERBUFFER
@arg renderbuffer Specifies the name of the renderbuffer object to bind

@see http://www.opengl.org/sdk/docs/man/html/glBindRenderbuffer.xhtml
 */
YF void        YAC_CALL _glBindRenderbuffer     (sSI _target, sUI _renderbuffer);


/* @function zglDeleteRenderbuffer,int renderbuffer
Delete renderbuffer objects

@arg renderbuffer Specifies the number of renderbuffer objects to be deleted.

@see http://www.opengl.org/sdk/docs/man/html/glDeleteRenderbuffers.xhtml
 */
YF void        YAC_CALL _zglDeleteRenderbuffer  (sUI _renderbuffer);


/* @function zglGenRenderbuffer:int
Generate renderbuffer object names

@return Renderbuffer object name

@see http://www.opengl.org/sdk/docs/man/html/glGenRenderbuffers.xhtml
 */
YF sUI         YAC_CALL _zglGenRenderbuffer     (void);


/* @function glRenderbufferStorage,int target,int internalformat,int width,int height
Establish data storage, format and dimensions of a renderbuffer object's image

@arg target Specifies a binding to which the target of the allocation and must be §GL_RENDERBUFFER
@arg internalformat Specifies the internal format to use for the renderbuffer object's image
@arg width Specifies the width of the renderbuffer, in pixels
@arg height Specifies the height of the renderbuffer, in pixels

@see http://www.opengl.org/sdk/docs/man/html/glRenderbufferStorage.xhtml
 */
YF void        YAC_CALL _glRenderbufferStorage  (sSI _target, sSI _internalformat, sUI _width, sUI _height);


/* @function glRenderbufferStorageMultisample,int target,int samples,int internalformat,int width,int height
Establish data storage, format, dimensions and sample count of a renderbuffer object's image

@arg target Specifies a binding to which the target of the allocation and must be §GL_RENDERBUFFER
@arg samples Specifies the number of samples to be used for the renderbuffer object's storage. (see §glGet / §GL_MAX_SAMPLES)
@arg internalformat Specifies the internal format to use for the renderbuffer object's image
@arg width Specifies the width of the renderbuffer, in pixels
@arg height Specifies the height of the renderbuffer, in pixels

@see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glRenderbufferStorageMultisample.xhtml
 */
YF void        YAC_CALL _glRenderbufferStorageMultisample (sSI _target, sUI _samples, sSI _internalformat, sUI _width, sUI _height);


/* @function zglGetRenderbufferParameteri,int target,int pname:int

@see http://www.opengl.org/sdk/docs/man/html/glGetRenderbufferParameter.xhtml
 */
YF sSI         YAC_CALL _zglGetRenderbufferParameteri (sSI _target, sSI _pname);


/* @function glIsFramebuffer,int framebuffer:boolean
Determine if a name corresponds to a framebuffer object

@arg framebuffer Specifies a value that may be the name of a framebuffer object

@see http://www.opengl.org/sdk/docs/man/html/glIsFramebuffer.xhtml
 */
YF sSI         YAC_CALL _glIsFramebuffer        (sUI _framebuffer);


/* @function glBindFramebuffer,int target,int framebuffer
Bind a framebuffer to a framebuffer target

@arg target Specifies the framebuffer target of the binding operation
@arg framebuffer Specifies the name of the framebuffer object to bind

@see http://www.opengl.org/sdk/docs/man/html/glBindFramebuffer.xhtml
 */
YF void        YAC_CALL _glBindFramebuffer      (sSI _target, sUI _framebuffer);


/* @function zglDeleteFramebuffer,int framebuffer
Delete framebuffer objects

@arg framebuffer Framebuffer object name

@see http://www.opengl.org/sdk/docs/man/html/glDeleteFramebuffers.xhtml
 */
YF void        YAC_CALL _zglDeleteFramebuffer   (sUI _framebuffer);


/* @function zglGenFramebuffer:int
Generate framebuffer object names

@return Framebuffer object name

@see http://www.opengl.org/sdk/docs/man/html/glGenFramebuffers.xhtml
 */
YF sUI         YAC_CALL _zglGenFramebuffer      (void);


/* @function glCheckFramebufferStatus,int target:int
Check the completeness status of a framebuffer

@arg target Specify the target of the framebuffer completeness check
@return The return value is §GL_FRAMEBUFFER_COMPLETE if the framebuffer bound to target is complete. Other return values are.. §GL_FRAMEBUFFER_UNDEFINED, §GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT, §GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT, §GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER, §GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER, §GL_FRAMEBUFFER_UNSUPPORTED, §GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE, §GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE, §GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS

@see http://www.opengl.org/sdk/docs/man/html/glCheckFramebufferStatus.xhtml
 */
YF sSI         YAC_CALL _glCheckFramebufferStatus (sSI _target);


/* @function glFramebufferTexture1D,int target,int attachment,int textarget,int texture,int level
Attach a level of a texture object as a logical buffer to the currently bound framebuffer object

@arg target Specifies the framebuffer target. target must be §GL_DRAW_FRAMEBUFFER, §GL_READ_FRAMEBUFFER, or §GL_FRAMEBUFFER. §GL_FRAMEBUFFER is equivalent to §GL_DRAW_FRAMEBUFFER
@arg attachment Specifies the attachment point of the framebuffer. attachment must be GL_COLOR_ATTACHMENTi, §GL_DEPTH_ATTACHMENT, §GL_STENCIL_ATTACHMENT or §GL_DEPTH_STENCIL_ATTACHMENT
@arg textarget For §glFramebufferTexture1D, §glFramebufferTexture2D and §glFramebufferTexture3D, specifies what type of texture is expected in the texture parameter, or for cube map textures, which face is to be attached
@arg texture Specifies the texture object to attach to the framebuffer attachment point named by attachment
@arg level Specifies the mipmap level of texture to attach

@see http://www.opengl.org/sdk/docs/man/html/glFramebufferTexture.xhtml
 */
YF void        YAC_CALL _glFramebufferTexture1D (sSI _target, sSI _attachment, sSI _textarget, sUI _texture, sSI _level);


/* @function glFramebufferTexture2D,int target,int attachment,int textarget,int texture,int level
Attach a level of a texture object as a logical buffer to the currently bound framebuffer object

@arg target Specifies the framebuffer target. target must be §GL_DRAW_FRAMEBUFFER, §GL_READ_FRAMEBUFFER, or §GL_FRAMEBUFFER. §GL_FRAMEBUFFER is equivalent to §GL_DRAW_FRAMEBUFFER
@arg attachment Specifies the attachment point of the framebuffer. attachment must be GL_COLOR_ATTACHMENTi, §GL_DEPTH_ATTACHMENT, §GL_STENCIL_ATTACHMENT or §GL_DEPTH_STENCIL_ATTACHMENT
@arg textarget For §glFramebufferTexture1D, §glFramebufferTexture2D and §glFramebufferTexture3D, specifies what type of texture is expected in the texture parameter, or for cube map textures, which face is to be attached
@arg texture Specifies the texture object to attach to the framebuffer attachment point named by attachment
@arg level Specifies the mipmap level of texture to attach


@see http://www.opengl.org/sdk/docs/man/html/glFramebufferTexture.xhtml
 */
YF void        YAC_CALL _glFramebufferTexture2D (sSI _target, sSI _attachment, sSI _textarget, sUI _texture, sSI _level);


/* @function glFramebufferTexture3D,int target,int attachment,int textarget,int texture,int level,int zoffset
Attach a level of a texture object as a logical buffer to the currently bound framebuffer object

@arg target Specifies the framebuffer target. target must be §GL_DRAW_FRAMEBUFFER, §GL_READ_FRAMEBUFFER, or §GL_FRAMEBUFFER. §GL_FRAMEBUFFER is equivalent to §GL_DRAW_FRAMEBUFFER
@arg attachment Specifies the attachment point of the framebuffer. attachment must be GL_COLOR_ATTACHMENTi, §GL_DEPTH_ATTACHMENT, §GL_STENCIL_ATTACHMENT or §GL_DEPTH_STENCIL_ATTACHMENT
@arg textarget For §glFramebufferTexture1D, §glFramebufferTexture2D and §glFramebufferTexture3D, specifies what type of texture is expected in the texture parameter, or for cube map textures, which face is to be attached
@arg texture Specifies the texture object to attach to the framebuffer attachment point named by attachment
@arg level Specifies the mipmap level of texture to attach

@see http://www.opengl.org/sdk/docs/man/html/glFramebufferTexture.xhtml
 */
YF void        YAC_CALL _glFramebufferTexture3D (sSI _target, sSI _attachment, sSI _textarget, sUI _texture, sSI _level, sSI _zoffset);


/* @function glFramebufferRenderbuffer,int target,int attachment,int renderbuffertarget,int renderbuffer
Attach a renderbuffer as a logical buffer to the currently bound framebuffer object

@arg target Specifies the framebuffer target. target must be §GL_DRAW_FRAMEBUFFER, §GL_READ_FRAMEBUFFER, or §GL_FRAMEBUFFER. §GL_FRAMEBUFFER is equivalent to §GL_DRAW_FRAMEBUFFER
@arg attachment Specifies the attachment point of the framebuffer
@arg renderbuffertarget Specifies the renderbuffer target and must be §GL_RENDERBUFFER
@arg renderbuffer Specifies the name of an existing renderbuffer object of type renderbuffertarget to attach

@see http://www.opengl.org/sdk/docs/man/html/glFramebufferRenderbuffer.xhtml
 */
YF void        YAC_CALL _glFramebufferRenderbuffer (sSI _target, sSI _attachment, sSI _renderbuffertarget, sSI _renderbuffer);


/* @function zglGetFramebufferAttachmentParameteri,int target,int attachment,int pname:int
Retrieve information about attachments of a bound framebuffer object

@arg target Specifies the target of the query operation.
@arg attachment Specifies the attachment within target
@arg pname Specifies the parameter of attachment to query
@return The value of pname for attachment

@see http://www.opengl.org/sdk/docs/man/html/glGetFramebufferAttachmentParameter.xhtml
 */
YF sSI         YAC_CALL _zglGetFramebufferAttachmentParameteri (sSI _target, sSI _attachment, sSI _pname);


/* @function glBlitFramebuffer,int srcX0,int srcY0,int srcX1,int srcY1,int dstX0,int dstY0,int dstX1,int dstY1,int mask,int filter
Copy a block of pixels from one framebuffer object to another

@arg srcX0 Specify the bounds of the source rectangle within the read buffer of the read framebuffer.
@arg srcY0
@arg srcX1
@arg srcY1
@arg dstX0 Specify the bounds of the destination rectangle within the write buffer of the write framebuffer.
@arg dstY0
@arg dstX1
@arg dstY1
@arg mask The bitwise OR of the flags indicating which buffers are to be copied. The allowed flags are §GL_COLOR_BUFFER_BIT, §GL_DEPTH_BUFFER_BIT and §GL_STENCIL_BUFFER_BIT.
@arg filter Specifies the interpolation to be applied if the image is stretched. Must be GL_NEAREST or GL_LINEAR.

@see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlitFramebuffer.xhtml
 */
YF void        YAC_CALL _glBlitFramebuffer(sSI _srcX0, sSI _srcY0, sSI _srcX1, sSI _srcY1, sSI _dstX0, sSI _dstY0, sSI _dstX1, sSI _dstY1, sUI _mask, sSI _filter);

/* @function glGenerateMipmap,int target
Generate a complete set of mipmaps for a texture object

@arg target Specifies the texture target of the texture object whose mipmaps will be generated. Must be one of the following symbolic constants: §GL_TEXTURE_2D or §GL_TEXTURE_CUBE_MAP.

@see http://www.opengl.org/sdk/docs/man/html/glGenerateMipmap.xhtml
*/
YF void        YAC_CALL _glGenerateMipmap       (sSI);



//
//
// GLSL
//
//

/* @function glCreateProgram:int
Creates a program object

@return creates an empty program object and returns a non-zero value by which it can be referenced. returns 0 if an error occurs creating the program object.

@see http://www.opengl.org/sdk/docs/man/html/glCreateProgram.xhtml
*/
YF sUI YAC_CALL _glCreateProgram (void);


/* @function glDeleteProgram,int program
Deletes a program object

@arg program Specifies the program object to be deleted

@see http://www.opengl.org/sdk/docs/man/html/glDeleteProgram.xhtml
*/
YF void YAC_CALL _glDeleteProgram (sUI _program);


/* @function glCreateShader,int shaderType:int
Creates a shader object

@arg shaderType Specifies the type of shader to be created. Must be either §GL_VERTEX_SHADER or §GL_FRAGMENT_SHADER.

@return creates an empty shader object and returns a non-zero value by which it can be	referenced. returns 0 if an error occurs creating the	shader object

@see http://www.opengl.org/sdk/docs/man/html/glCreateShader.xhtml
*/
YF sUI YAC_CALL _glCreateShader (sUI shaderType);


/* @function glDeleteShader,int shader
Deletes a shader object

@arg program Specifies the shader object to be deleted

@see http://www.opengl.org/sdk/docs/man/html/glDeleteShader.xhtml
*/
YF void YAC_CALL _glDeleteShader (sUI _shader);


/* @function zglShaderSource,int shader,String source
Replaces the source code in a shader object

@arg shader Specifies the handle of the shader object whose source code is to be replaced

@arg source Specifies the source code to be loaded into the shader

@see http://www.opengl.org/sdk/docs/man/html/glShaderSource.xhtml
*/
YF void YAC_CALL _zglShaderSource (sUI shader, YAC_Object *_source);


/* @function glCompileShader,int shader
Compiles a shader object

@arg shader Specifies the shader object to be compiled.

@see http://www.opengl.org/sdk/docs/man/html/glCompileShader.xhtml
 */
YF void YAC_CALL _glCompileShader (sUI shader);


/* @function zglGetProgrami,int program,int pname:int
Returns a parameter from a program object

@arg shader Specifies the program object to be queried.
@return Returns the requested object parameter

@see http://www.opengl.org/sdk/docs/man/html/glGetProgram.xhtml
*/
YF sSI YAC_CALL _zglGetProgrami (sUI _program, sSI _pname);


/* @function glGetProgramInfoLog,int program,String retLog
Returns the information log for a program object

@arg program Specifies the program object whose information log is to be queried.
@arg retLog Receives the info log. The string is resized/reallocated, if necessary.

@see http://www.opengl.org/sdk/docs/man/html/glGetProgramInfoLog.xhtml
*/
YF void YAC_CALL _glGetProgramInfoLog (sUI program, YAC_Object *_retLog);


/* @function zglGetShaderi,int shader,int pname:int
Returns a parameter from a shader object

@arg shader Specifies the shader object to be queried.
@return Returns the requested object parameter

@see http://www.opengl.org/sdk/docs/man/html/glGetShader.xhtml
*/
YF sSI YAC_CALL _zglGetShaderi (sUI _shader, sSI _pname);


/* @function glGetShaderInfoLog,int shader,String retLog
Returns the information log for a shader object

@arg program Specifies the shader object whose information log is to be queried.
@arg retLog Receives the info log. The string is resized/reallocated, if necessary.

@see http://www.opengl.org/sdk/docs/man/html/glGetShaderInfoLog.xhtml
*/
YF void YAC_CALL _glGetShaderInfoLog (sUI shader, YAC_Object *_retLog);


/* @function glAttachShader,int program,int shader
Attaches a shader object to a program object

@arg program Specifies the program object to which a shader object will be attached
@arg shader Specifies the shader object that is to be attached

@see http://www.opengl.org/sdk/docs/man/html/glAttachShader.xhtml
 */
YF void YAC_CALL _glAttachShader (sUI program, sUI shader);


/* @function glDetachShader,int program,int shader
Detaches a shader object from a program object to which it is attached

@arg program Specifies the program object from which to detach the shader object
@arg shader Specifies the shader object to be detached

@see http://www.opengl.org/sdk/docs/man/html/glDetachShader.xhtml
 */
YF void YAC_CALL _glDetachShader (sUI program, sUI shader);


/* @function glLinkProgram,int program
Links a program object

@arg program Specifies the handle of the program object to be linked.

@see http://www.opengl.org/sdk/docs/man/html/glLinkProgram.xhtml
*/
YF void YAC_CALL _glLinkProgram (sUI program);


/* @function glValidateProgram,int program
Validates a program object

@arg program Specifies the handle of the program object to be validated.

@see http://www.opengl.org/sdk/docs/man/html/glValidateProgram.xhtml
*/
YF void YAC_CALL _glValidateProgram (sUI program);


/* @function glUseProgram,int program
Installs a program object as part of current rendering state

@arg program Specifies the handle of the program object whose executables are to be used as part of current rendering state

@see http://www.opengl.org/sdk/docs/man/html/glUseProgram.xhtml
*/
YF void YAC_CALL _glUseProgram (sUI program);


/* @function glBindAttribLocation,int program,int index,String name
Associates a generic vertex attribute index with a named attribute variable

@arg program Specifies the handle of the program object in which the association is to be made
@arg index Specifies the index of the generic vertex attribute to be bound
@arg name Specifies the name of the vertex shader attribute variable to which index is to be bound

@see http://www.opengl.org/sdk/docs/man/html/glBindAttribLocation.xhtml
*/
YF void YAC_CALL _glBindAttribLocation (sUI _program, sUI _index, YAC_Object *_name);


/* @function glGetAttribLocation,int program,String name
Returns the location of an attribute variable

@arg program Specifies the handle of the program object to be queried
@arg name Specifies the name of the attribute variable whose location is to be queried

@see http://www.opengl.org/sdk/docs/man/html/glGetAttribLocation.xhtml
*/
YF sSI YAC_CALL _glGetAttribLocation (sUI _program, YAC_Object *_name);


/* @function glBindFragDataLocation,int program,int colorNumber,String name
Bind a user-defined varying out variable to a fragment shader color number

@arg program The name of the program containing varying out variable whose binding to modify
@arg colorNumber The color number to bind the user-defined varying out variable to
@arg name The name of the user-defined varying out variable whose binding to modify

@see http://www.opengl.org/sdk/docs/man/html/glBindFragDataLocation.xhtml
*/
YF void YAC_CALL _glBindFragDataLocation (sUI _program, sUI _colorNumber, YAC_Object *_name);


/* @function glBindFragDataLocationIndexed,int program,int colorNumber,int index,String name
Bind a user-defined varying out variable to a fragment shader color number and index

@arg program The name of the program containing varying out variable whose binding to modify
@arg colorNumber The color number to bind the user-defined varying out variable to
@arg index The index of the color input to bind the user-defined varying out variable to
@arg name The name of the user-defined varying out variable whose binding to modify

@see http://www.opengl.org/sdk/docs/man/html/glBindFragDataLocationIndexed.xhtml
*/
YF void YAC_CALL _glBindFragDataLocationIndexed (sUI _program, sUI _colorNumber, sUI _index, YAC_Object *_name);


/* @function glGetFragDataLocation,int program,String name:int
Query the bindings of color numbers to user-defined varying out variables

@arg program The name of the program containing varying out variable whose binding to modify
@arg name The name of the user-defined varying out variable whose binding to modify
@return The assigned color number binding for the user-defined varying out variable name for program program. program must have previously been linked

@see http://www.opengl.org/sdk/docs/man/html/glGetFragDataLocation.xhtml
*/
YF sSI YAC_CALL _glGetFragDataLocation (sUI _program, YAC_Object *_name);


/* @function glGetFragDataIndex,int program,String name:int
Query the bindings of color indices to user-defined varying out variables

@arg program The name of the program containing varying out variable whose binding to modify
@arg name The name of the user-defined varying out variable whose index to query
@return The index of the fragment color to which the variable name was bound when the program object program was last linked. If name is not a varying out variable of program, or if an error occurs, -1 will be returned

@see http://www.opengl.org/sdk/docs/man/html/glGetFragDataIndex.xhtml
*/
YF sSI YAC_CALL _glGetFragDataIndex (sUI _program, YAC_Object *_name);


/* @function glVertexAttrib1f,int index,float v0
Specifies the value of a generic vertex attribute

@arg index Specifies the index of the generic vertex attribute to be modified
@arg v0 Specifies the new value to be used for the specified vertex attribute

@see http://www.opengl.org/sdk/docs/man/html/glVertexAttrib.xhtml
*/
YF void YAC_CALL _glVertexAttrib1f (sUI _index, sF32 _v0);


/* @function glVertexAttrib2f,int index,float v0,float v1
Specifies the values of a generic vertex attribute

@arg index Specifies the index of the generic vertex attribute to be modified
@arg v0 Specifies the new value to be used for the specified vertex attribute
@arg v1 Specifies the new value to be used for the specified vertex attribute

@see http://www.opengl.org/sdk/docs/man/html/glVertexAttrib.xhtml
*/
YF void YAC_CALL _glVertexAttrib2f (sUI _index, sF32 _v0, sF32 _v1);


/* @function glVertexAttrib3f,int index,float v0,float v1,float v2
Specifies the values of a generic vertex attribute

@arg index Specifies the index of the generic vertex attribute to be modified
@arg v0 Specifies the new value to be used for the specified vertex attribute
@arg v1 Specifies the new value to be used for the specified vertex attribute
@arg v2 Specifies the new value to be used for the specified vertex attribute

@see http://www.opengl.org/sdk/docs/man/html/glVertexAttrib.xhtml
*/
YF void YAC_CALL _glVertexAttrib3f (sUI _index, sF32 _v0, sF32 _v1, sF32 _v2);


/* @function glVertexAttrib4f,int index,float v0,float v1,float v2,float v3
Specifies the values of a generic vertex attribute

@arg index Specifies the index of the generic vertex attribute to be modified
@arg v0 Specifies the new value to be used for the specified vertex attribute
@arg v1 Specifies the new value to be used for the specified vertex attribute
@arg v2 Specifies the new value to be used for the specified vertex attribute
@arg v3 Specifies the new value to be used for the specified vertex attribute

@see http://www.opengl.org/sdk/docs/man/html/glVertexAttrib.xhtml
*/
YF void YAC_CALL _glVertexAttrib4f (sUI _index, sF32 _v0, sF32 _v1, sF32 _v2, sF32 _v3);


/* @function glVertexAttrib4Nub,int index,byte v0,byte v1,byte v2,byte v3
Specifies the values of a generic vertex attribute

@arg index Specifies the index of the generic vertex attribute to be modified
@arg v0 Specifies the new value to be used for the specified vertex attribute
@arg v1 Specifies the new value to be used for the specified vertex attribute
@arg v2 Specifies the new value to be used for the specified vertex attribute
@arg v3 Specifies the new value to be used for the specified vertex attribute

@see http://www.opengl.org/sdk/docs/man/html/glVertexAttrib.xhtml
*/
YF void YAC_CALL _glVertexAttrib4Nub (sUI _index, sUI _v0, sUI _v1, sUI _v2, sUI _v3);


/* @function glVertexAttribPointer,int index,int size,int type,boolean normalized,int stride,Object a
Define an array of generic vertex attribute data

@arg index Specifies the index of the generic vertex attribute to be modified
@arg size Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, or 4. The initial value is 4.
@arg type Specifies the data type of each component in the array. Symbolic constants §GL_BYTE, §GL_UNSIGNED_BYTE, §GL_SHORT, §GL_UNSIGNED_SHORT, §GL_INT, §GL_UNSIGNED_INT, §GL_FLOAT, or §GL_DOUBLE are accepted. The initial value is §GL_FLOAT.
@arg normalized Specifies whether fixed-point data values should be normalized (§GL_TRUE) or converted directly as fixed-point values (§GL_FALSE) when they are accessed
@arg stride Specifies the byte offset between consecutive generic vertex attributes. If stride is 0, the generic vertex attributes are understood to be tightly packed in the array. The initial value is 0.
@arg a Specifies the array object holding the vertex attribute data

@see http://www.opengl.org/sdk/docs/man/html/glVertexAttribPointer.xhtml
*/
YF void YAC_CALL _glVertexAttribPointer (sUI _index, sUI _size, sSI _type, sSI _bNormalized, sSI _stride, YAC_Object *_a);


/* @function zglVertexAttribOffset,int index,int size,int type,boolean normalized,int stride,int offset
Define an array of generic vertex attribute data

@arg index Specifies the index of the generic vertex attribute to be modified
@arg size Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, or 4. The initial value is 4.
@arg type Specifies the data type of each component in the array. Symbolic constants §GL_BYTE, §GL_UNSIGNED_BYTE, §GL_SHORT, §GL_UNSIGNED_SHORT, §GL_INT, §GL_UNSIGNED_INT, §GL_FLOAT, or §GL_DOUBLE are accepted. The initial value is §GL_FLOAT.
@arg normalized Specifies whether fixed-point data values should be normalized (§GL_TRUE) or converted directly as fixed-point values (§GL_FALSE) when they are accessed
@arg stride Specifies the byte offset between consecutive generic vertex attributes. If stride is 0, the generic vertex attributes are understood to be tightly packed in the array. The initial value is 0.
@arg offset Specifies the offset into the currently bound buffer object

@see http://www.opengl.org/sdk/docs/man/html/glVertexAttribPointer.xhtml
*/
YF void YAC_CALL _zglVertexAttribOffset (sUI _index, sUI _size, sSI _type, sSI _bNormalized, sSI _stride, sSI _offset);


/* @function glEnableVertexAttribArray,int index
Enable a generic vertex attribute array

@arg index Specifies the index of the generic vertex attribute to be enabled

@see http://www.opengl.org/sdk/docs/man/html/glEnableVertexAttribArray.xhtml
*/
YF void YAC_CALL _glEnableVertexAttribArray (sUI _index);


/* @function glDisableVertexAttribArray,int index
Disable a generic vertex attribute array

@arg index Specifies the index of the generic vertex attribute to be disabled

@see http://www.opengl.org/sdk/docs/man/html/glEnableVertexAttribArray.xhtml
*/
YF void YAC_CALL _glDisableVertexAttribArray (sUI _index);


/* @function glGetUniformLocation,int program,String name:int
Returns the location of a uniform variable

@arg program Specifies the program object to be queried. The program must currently be bound, please see §glUseProgram.
@arg name The name of the uniform variable whose location is to be queried
@return location of the uniform variable

@see http://www.opengl.org/sdk/docs/man/html/glGetUniformLocation.xhtml
*/
YF sSI YAC_CALL _glGetUniformLocation (sUI _program, YAC_Object *_name);


/* @function zglGetActiveUniform,int program,int index,Integer retSize,String retName:int
Returns information about an active uniform variable for the specified program object

@arg program Specifies the program object to be queried.
@arg index Specifies the index of the uniform variable to be queried
@arg retSize Returns the size of the uniform variable (can be null if not needed)
@arg retName Return the name of the uniform variable. The string is resized, if necessary.
@return Data type of the uniform variable

@see http://www.opengl.org/sdk/docs/man/html/glGetActiveUniform.xhtml
*/
YF sSI YAC_CALL _zglGetActiveUniform (sUI _program, sUI _index, YAC_Object *_retSize,YAC_Object *_retName);


/* @function glUniform1f,int location,float v0
Specify the value of a uniform variable for the current program object

@arg location Specifies the location of the uniform variable to be modified.
@arg v0 Specifies the new value to be used for the specified uniform variable

@see http://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
*/
YF void YAC_CALL _glUniform1f (sSI _location, sF32 _v0);


/* @function glUniform2f,int location,float v0,float v1
Specify the values of a uniform variable for the current program object

@arg location Specifies the location of the uniform variable to be modified.
@arg v0 Specifies the new value to be used for the specified uniform variable
@arg v1 Specifies the new value to be used for the specified uniform variable

@see http://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
*/
YF void YAC_CALL _glUniform2f (sSI _location, sF32 _v0, sF32 _v1);


/* @function glUniform3f,int location,float v0,float v1,float v2
Specify the values of a uniform variable for the current program object

@arg location Specifies the location of the uniform variable to be modified.
@arg v0 Specifies the new value to be used for the specified uniform variable
@arg v1 Specifies the new value to be used for the specified uniform variable
@arg v2 Specifies the new value to be used for the specified uniform variable

@see http://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
*/
YF void YAC_CALL _glUniform3f (sSI _location, sF32 _v0, sF32 _v1, sF32 _v2);


/* @function glUniform4f,int location,float v0,float v1,float v2,float v3
Specify the values of a uniform variable for the current program object

@arg location Specifies the location of the uniform variable to be modified.
@arg v0 Specifies the new value to be used for the specified uniform variable
@arg v1 Specifies the new value to be used for the specified uniform variable
@arg v2 Specifies the new value to be used for the specified uniform variable
@arg v3 Specifies the new value to be used for the specified uniform variable

@see http://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
*/
YF void YAC_CALL _glUniform4f (sSI _location, sF32 _v0, sF32 _v1, sF32 _v2, sF32 _v3);


/* @function glUniform1i,int location,int v0
Specify the value of a uniform variable for the current program object

@arg location Specifies the location of the uniform variable to be modified.
@arg v0 Specifies the new value to be used for the specified uniform variable

@see http://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
*/
YF void YAC_CALL _glUniform1i (sSI _location, sSI _v0);


/* @function glUniform2i,int location,int v0,int v1
Specify the values of a uniform variable for the current program object

@arg location Specifies the location of the uniform variable to be modified.
@arg v0 Specifies the new value to be used for the specified uniform variable
@arg v1 Specifies the new value to be used for the specified uniform variable

@see http://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
*/
YF void YAC_CALL _glUniform2i (sSI _location, sSI _v0, sSI _v1);


/* @function glUniform3i,int location,int v0,int v1,int v2
Specify the values of a uniform variable for the current program object

@arg location Specifies the location of the uniform variable to be modified.
@arg v0 Specifies the new value to be used for the specified uniform variable
@arg v1 Specifies the new value to be used for the specified uniform variable
@arg v2 Specifies the new value to be used for the specified uniform variable

@see http://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
*/
YF void YAC_CALL _glUniform3i (sSI _location, sSI _v0, sSI _v1, sSI _v2);


/* @function glUniform4i,int location,int v0,int v1,int v2,int v3
Specify the values of a uniform variable for the current program object

@arg location Specifies the location of the uniform variable to be modified.
@arg v0 Specifies the new value to be used for the specified uniform variable
@arg v1 Specifies the new value to be used for the specified uniform variable
@arg v2 Specifies the new value to be used for the specified uniform variable
@arg v3 Specifies the new value to be used for the specified uniform variable

@see http://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
*/
YF void YAC_CALL _glUniform4i (sSI _location, sSI _v0, sSI _v1, sSI _v2, sSI _v3);


/* @function glUniformMatrix2fv,int location,int count,boolean tranpose,Object a
Specify the values of a uniform 2x2 matrix variable for the current program object

@arg location Specifies the location of the uniform variable to be modified.
@arg count Specifies the number of matrices that are to be modified. This should be 1 if the targeted uniform variable is not an array of matrices, and 1 or more if it is an array of matrices (currently limited to 256 elements, i.e. 64 2x2 matrices)
@arg transpose Specifies whether to transpose the matrix as the values are loaded into the uniform variable
@arg a Array object (non-float array values will be converted to float)

@see http://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
*/
YF void YAC_CALL _glUniformMatrix2fv (sSI _location, sSI _count, sSI _bTranspose, YAC_Object *_fa);


/* @function glUniformMatrix3fv,int location,int count,boolean tranpose,Object a
Specify the values of a uniform 3x3 matrix variable for the current program object

@arg location Specifies the location of the uniform variable to be modified.
@arg count Specifies the number of matrices that are to be modified. This should be 1 if the targeted uniform variable is not an array of matrices, and 1 or more if it is an array of matrices (currently limited to 252 elements, i.e. 28 3x3 matrices)
@arg transpose Specifies whether to transpose the matrix as the values are loaded into the uniform variable
@arg a Array object (non-float array values will be converted to float)

@see http://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
*/
YF void YAC_CALL _glUniformMatrix3fv (sSI _location, sSI _count, sSI _bTranspose, YAC_Object *_fa);


/* @function glUniformMatrix4fv,int location,int count,boolean tranpose,Object a
Specify the values of a uniform 4x4 matrix variable for the current program object

@arg location Specifies the location of the uniform variable to be modified.
@arg count Specifies the number of matrices that are to be modified. This should be 1 if the targeted uniform variable is not an array of matrices, and 1 or more if it is an array of matrices (currently limited to 256 elements, i.e. 16 4x4 matrices)
@arg transpose Specifies whether to transpose the matrix as the values are loaded into the uniform variable
@arg a Array object (non-float array values will be converted to float)

@see http://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
*/
YF void YAC_CALL _glUniformMatrix4fv (sSI _location, sSI _count, sSI _bTranspose, YAC_Object *_fa);


//
//
// --- samplers ---
//
//

/* @function zglGenSampler:int
Generate sampler object name

@return The generated sampler object name

@see http://www.opengl.org/sdk/docs/man/html/glGenSamplers.xhtml
*/
YF sUI YAC_CALL _zglGenSampler (void);


/* @function zglDeleteSampler,int id
Delete named sampler object

@arg Specifies the sampler object to be deleted

@see http://www.opengl.org/sdk/docs/man/html/glDeleteSamplers.xhtml
*/
YF void YAC_CALL _zglDeleteSampler (sUI _id);


/* @function glBindSampler,int unit,int sampler
Bind a named sampler to a texturing target

@arg unit Specifies the index of the texture unit to which the sampler is bound.
@arg sampler Specifies the name of a sampler.

@see http://www.opengl.org/sdk/docs/man/html/glBindSampler.xhtml
*/
YF void YAC_CALL _glBindSampler (sUI _unit, sUI _sampler);


/* @function glSamplerParameteri,int sampler,int pname,int param
Set sampler parameters

@arg sampler Specifies the sampler object whose parameter to modify.
@arg pname Specifies the symbolic name of a single-valued sampler parameter. pname can be one of the following: §GL_TEXTURE_WRAP_S, §GL_TEXTURE_WRAP_T, §GL_TEXTURE_WRAP_R, §GL_TEXTURE_MIN_FILTER, §GL_TEXTURE_MAG_FILTER, §GL_TEXTURE_MIN_LOD, §GL_TEXTURE_MAX_LOD, §GL_TEXTURE_LOD_BIAS, §GL_TEXTURE_COMPARE_MODE, or §GL_TEXTURE_COMPARE_FUNC
@arg param Specifies the value of pname

@see http://www.opengl.org/sdk/docs/man/html/glSamplerParameter.xhtml
*/
YF void YAC_CALL _glSamplerParameteri (sUI _sampler, sSI _pname, sSI _param);



//
//
// --- Vertex Array Objects (VAO) ---
//
//
YF void  YAC_CALL _glBindVertexArray (sUI _array);
YF void  YAC_CALL _zglDeleteVertexArray (sUI _array);
YF sUI   YAC_CALL _zglGenVertexArray (void);
YF sBool YAC_CALL _glIsVertexArray (sUI _array);


// GL3.3+, GLES3.0+
YF void YAC_CALL _glVertexAttribDivisor (sUI index, sUI divisor);



/* @class DisplayList,Object
Represents an OpenGL display list
*/
YC class _DisplayList : public YAC_Object {
// Note: the DisplayList class is "abused" to export all the global GL constants !
public:
	sSI        gl_dlist;
    YAC_String name;

public:
	_DisplayList(void);
	~_DisplayList();

	YAC(_DisplayList);

	// the following lines are not comments but they rather represent a constant export table

// OpenGL 1.1 constants:


/* @constant GL_NONE,int
*/
//#define GL_NONE                           YCI 0


/* @constant GL_TRUE,int
*/
//#define GL_TRUE                           YCI 1


/* @constant GL_FALSE,int
*/
//#define GL_FALSE                          YCI 0


/* @constant GL_BOOL,int
*/
//#define GL_BOOL                           YCI 0x8B56


/* @constant GL_BOOL_VEC2,int
*/
//#define GL_BOOL_VEC2                      YCI 0x8B57


/* @constant GL_BOOL_VEC3,int
*/
//#define GL_BOOL_VEC3                      YCI 0x8B58


/* @constant GL_BOOL_VEC4,int
*/
//#define GL_BOOL_VEC4                      YCI 0x8B59


/* @constant GL_BYTE,int
*/
//#define GL_BYTE                           YCI 0x1400


/* @constant GL_UNSIGNED_BYTE,int
*/
//#define GL_UNSIGNED_BYTE                  YCI 0x1401


/* @constant GL_SHORT,int
*/
//#define GL_SHORT                          YCI 0x1402


/* @constant GL_UNSIGNED_SHORT,int
*/
//#define GL_UNSIGNED_SHORT                 YCI 0x1403


/* @constant GL_UNSIGNED_SHORT_5_6_5,int
*/
//#define GL_UNSIGNED_SHORT_5_6_5           YCI 0x8363


/* @constant GL_UNSIGNED_SHORT_5_6_5_REV,int
*/
//#define GL_UNSIGNED_SHORT_5_6_5_REV       YCI 0x8364


/* @constant GL_UNSIGNED_INT_8_8_8_8,int
*/
//#define GL_UNSIGNED_INT_8_8_8_8           YCI 0x8035


/* @constant GL_UNSIGNED_INT_8_8_8_8_REV,int
*/
//#define GL_UNSIGNED_INT_8_8_8_8_REV       YCI 0x8367


/* @constant GL_INT,int
*/
//#define GL_INT                            YCI 0x1404


/* @constant GL_INT_VEC2,int
*/
//#define GL_INT_VEC2                       YCI 0x8B53


/* @constant GL_INT_VEC3,int
*/
//#define GL_INT_VEC3                       YCI 0x8B54


/* @constant GL_INT_VEC4,int
*/
//#define GL_INT_VEC4                       YCI 0x8B55


/* @constant GL_UNSIGNED_INT,int
*/
//#define GL_UNSIGNED_INT                   YCI 0x1405


/* @constant GL_FLOAT,int
*/
//#define GL_FLOAT                          YCI 0x1406


/* @constant GL_FLOAT_VEC2,int
*/
//#define GL_FLOAT_VEC2                     YCI 0x8B50


/* @constant GL_FLOAT_VEC3,int
*/
//#define GL_FLOAT_VEC3                     YCI 0x8B51


/* @constant GL_FLOAT_VEC4,int
*/
//#define GL_FLOAT_VEC4                     YCI 0x8B52


/* @constant GL_FLOAT_MAT2,int
*/
//#define GL_FLOAT_MAT2                     YCI 0x8B5A


/* @constant GL_FLOAT_MAT3,int
*/
//#define GL_FLOAT_MAT3                     YCI 0x8B5B


/* @constant GL_FLOAT_MAT4,int
*/
//#define GL_FLOAT_MAT4                     YCI 0x8B5C


/* @constant GL_2_BYTES,int
*/
//#define GL_2_BYTES                        YCI 0x1407


/* @constant GL_3_BYTES,int
*/
//#define GL_3_BYTES                        YCI 0x1408


/* @constant GL_4_BYTES,int
*/
//#define GL_4_BYTES                        YCI 0x1409


/* @constant GL_DOUBLE,int
*/
//#define GL_DOUBLE                         YCI 0x140A


/* @constant GL_FRONT_LEFT,int
*/
//#define GL_FRONT_LEFT                     YCI 0x0400


/* @constant GL_FRONT_RIGHT,int
*/
//#define GL_FRONT_RIGHT                    YCI 0x0401


/* @constant GL_BACK_LEFT,int
*/
//#define GL_BACK_LEFT                      YCI 0x0402


/* @constant GL_BACK_RIGHT,int
*/
//#define GL_BACK_RIGHT                     YCI 0x0403


/* @constant GL_LEFT,int
*/
//#define GL_LEFT                           YCI 0x0406


/* @constant GL_RIGHT,int
*/
//#define GL_RIGHT                          YCI 0x0407


/* @constant GL_AUX0,int
*/
//#define GL_AUX0                           YCI 0x0409


/* @constant GL_AUX1,int
*/
//#define GL_AUX1                           YCI 0x040A


/* @constant GL_AUX2,int
*/
//#define GL_AUX2                           YCI 0x040B


/* @constant GL_AUX3,int
*/
//#define GL_AUX3                           YCI 0x040C


/* @constant GL_INDEX_ARRAY,int
*/
//#define GL_INDEX_ARRAY                    YCI 0x8077


/* @constant GL_EDGE_FLAG_ARRAY,int
*/
//#define GL_EDGE_FLAG_ARRAY                YCI 0x8079


/* @constant GL_ALPHA4,int
*/
//#define GL_ALPHA4                         YCI 0x803B


/* @constant GL_ALPHA8,int
*/
//#define GL_ALPHA8                         YCI 0x803c


/* @constant GL_ALPHA12,int
*/
//#define GL_ALPHA12                        YCI 0x803D


/* @constant GL_ALPHA16,int
*/
//#define GL_ALPHA16                        YCI 0x803E


/* @constant GL_LUMINANCE4,int
*/
//#define GL_LUMINANCE4                     YCI 0x803F


/* @constant GL_LUMINANCE8,int
*/
//#define GL_LUMINANCE8                     YCI 0x8040


/* @constant GL_LUMINANCE12,int
*/
//#define GL_LUMINANCE12                    YCI 0x8041


/* @constant GL_LUMINANCE16,int
*/
//#define GL_LUMINANCE16                    YCI 0x8042


/* @constant GL_LUMINANCE4_ALPHA4,int
*/
//#define GL_LUMINANCE4_ALPHA4              YCI 0x8043


/* @constant GL_LUMINANCE6_ALPHA2,int
*/
//#define GL_LUMINANCE6_ALPHA2              YCI 0x8044


/* @constant GL_LUMINANCE8_ALPHA8,int
*/
//#define GL_LUMINANCE8_ALPHA8              YCI 0x8045


/* @constant GL_LUMINANCE12_ALPHA4,int
*/
//#define GL_LUMINANCE12_ALPHA4             YCI 0x8046


/* @constant GL_LUMINANCE12_ALPHA12,int
*/
//#define GL_LUMINANCE12_ALPHA12            YCI 0x8047


/* @constant GL_LUMINANCE16_ALPHA16,int
*/
//#define GL_LUMINANCE16_ALPHA16            YCI 0x8048


/* @constant GL_INTENSITY4,int
*/
//#define GL_INTENSITY4                     YCI 0x804A


/* @constant GL_INTENSITY8,int
*/
//#define GL_INTENSITY8                     YCI 0x804B


/* @constant GL_INTENSITY12,int
*/
//#define GL_INTENSITY12                    YCI 0x804C


/* @constant GL_INTENSITY16,int
*/
//#define GL_INTENSITY16                    YCI 0x804D


/* @constant GL_RGB4,int
*/
//#define GL_RGB4                           YCI 0x804F


/* @constant GL_RGB10,int
*/
//#define GL_RGB10                          YCI 0x8052


/* @constant GL_RGB12,int
*/
//#define GL_RGB12                          YCI 0x8053


/* @constant GL_RGB16,int
*/
//#define GL_RGB16                          YCI 0x8054


/* @constant GL_RGBA2,int
*/
//#define GL_RGBA2                          YCI 0x8055


/* @constant GL_RGBA4,int
*/
//#define GL_RGBA4                          YCI 0x8056


/* @constant GL_RGBA12,int
*/
//#define GL_RGBA12                         YCI 0x805A


/* @constant GL_RGBA16,int
*/
//#define GL_RGBA16                         YCI 0x805B


/* @constant GL_MODELVIEW,int
*/
//#define GL_MODELVIEW                   YCI 0x1700


/* @constant GL_PROJECTION,int
*/
//#define GL_PROJECTION                  YCI 0x1701


/* @constant GL_TEXTURE,int
*/
//#define GL_TEXTURE                     YCI 0x1702


/* @constant GL_FOG_HINT,int
*/
//#define GL_FOG_HINT                    YCI 0x0C54


/* @constant GL_LINE_SMOOTH_HINT,int
*/
//#define GL_LINE_SMOOTH_HINT            YCI 0x0C52


/* @constant GL_PERSPECTIVE_CORRECTION_HINT,int
*/
//#define GL_PERSPECTIVE_CORRECTION_HINT YCI 0x0C50


/* @constant GL_POINT_SMOOTH_HINT,int
*/
//#define GL_POINT_SMOOTH_HINT           YCI 0x0C51


/* @constant GL_POLYGON_SMOOTH_HINT,int
*/
//#define GL_POLYGON_SMOOTH_HINT         YCI 0x0C53


/* @constant GL_FASTEST,int
*/
//#define GL_FASTEST                     YCI 0x1101


/* @constant GL_NICEST,int
*/
//#define GL_NICEST                      YCI 0x1102


/* @constant GL_DONT_CARE,int
*/
//#define GL_DONT_CARE                   YCI 0x1100


/* @constant GL_ALPHA_TEST,int
*/
//#define GL_ALPHA_TEST                  YCI 0x0BC0


/* @constant GL_AUTO_NORMAL,int
*/
//#define GL_AUTO_NORMAL                 YCI 0x0D80


/* @constant GL_BLEND,int
*/
//#define GL_BLEND                       YCI 0x0BE2


/* @constant GL_BLEND_DST_RGB,int
*/
//#define GL_BLEND_DST_RGB               YCI 0x80C8


/* @constant GL_BLEND_SRC_RGB,int
*/
//#define GL_BLEND_SRC_RGB               YCI 0x80C9


/* @constant GL_BLEND_DST_ALPHA,int
*/
//#define GL_BLEND_DST_ALPHA             YCI 0x80CA


/* @constant GL_BLEND_SRC_ALPHA,int
*/
//#define GL_BLEND_SRC_ALPHA             YCI 0x80CB


/* @constant GL_CULL_FACE,int
*/
//#define GL_CULL_FACE                   YCI 0x0B44


/* @constant GL_DEPTH_TEST,int
*/
//#define GL_DEPTH_TEST                  YCI 0x0B71


/* @constant GL_DITHER,int
*/
//#define GL_DITHER                      YCI 0x0BD0


/* @constant GL_FOG,int
*/
//#define GL_FOG                         YCI 0x0B60


/* @constant GL_LIGHT0,int
*/
//#define GL_LIGHT0                      YCI 0x4000


/* @constant GL_LIGHT1,int
*/
//#define GL_LIGHT1                      YCI 0x4001


/* @constant GL_LIGHT2,int
*/
//#define GL_LIGHT2                      YCI 0x4002


/* @constant GL_LIGHT3,int
*/
//#define GL_LIGHT3                      YCI 0x4003


/* @constant GL_LIGHT4,int
*/
//#define GL_LIGHT4                      YCI 0x4004


/* @constant GL_LIGHT5,int
*/
//#define GL_LIGHT5                      YCI 0x4005


/* @constant GL_LIGHT6,int
*/
//#define GL_LIGHT6                      YCI 0x4006


/* @constant GL_LIGHT7,int
*/
//#define GL_LIGHT7                      YCI 0x4007


/* @constant GL_LIGHTING,int
*/
//#define GL_LIGHTING                    YCI 0x0B50


/* @constant GL_AMBIENT,int
*/
//#define GL_AMBIENT                     YCI 0x1200


/* @constant GL_DIFFUSE,int
*/
//#define GL_DIFFUSE                     YCI 0x1201


/* @constant GL_EMISSION,int
*/
//#define GL_EMISSION                    YCI 0x1600


/* @constant GL_SPECULAR,int
*/
//#define GL_SPECULAR                    YCI 0x1202


/* @constant GL_AMBIENT_AND_DIFFUSE,int
*/
//#define GL_AMBIENT_AND_DIFFUSE         YCI 0x1602


/* @constant GL_COLOR_MATERIAL,int
*/
//#define GL_COLOR_MATERIAL              YCI 0x0B57


/* @constant GL_SINGLE_COLOR,int
*/
//#define GL_SINGLE_COLOR                YCI 0x81F9


/* @constant GL_SEPARATE_SPECULAR_COLOR,int
*/
//#define GL_SEPARATE_SPECULAR_COLOR     YCI 0x81FA


/* @constant GL_LINE_SMOOTH,int
*/
//#define GL_LINE_SMOOTH                 YCI 0x0B20


/* @constant GL_LINE_STIPPLE,int
*/
//#define GL_LINE_STIPPLE                YCI 0x0B24


/* @constant GL_NORMALIZE,int
*/
//#define GL_NORMALIZE                   YCI 0x0BA1


/* @constant GL_POINT_SMOOTH,int
*/
//#define GL_POINT_SMOOTH                YCI 0x0B10


/* @constant GL_POLYGON_OFFSET_FILL,int
*/
//#define GL_POLYGON_OFFSET_FILL         YCI 0x8037


/* @constant GL_POLYGON_OFFSET_POINT,int
*/
//#define GL_POLYGON_OFFSET_POINT        YCI 0x2A01


/* @constant GL_POLYGON_SMOOTH,int
*/
//#define GL_POLYGON_SMOOTH              YCI 0x0B41


/* @constant GL_POLYGON_STIPPLE,int
*/
//#define GL_POLYGON_STIPPLE             YCI 0x0B42


/* @constant GL_SCISSOR_TEST,int
*/
//#define GL_SCISSOR_TEST                YCI 0x0C11


/* @constant GL_STENCIL_TEST,int
*/
//#define GL_STENCIL_TEST                YCI 0x0B90


/* @constant GL_MULTISAMPLE,int
*/
//#define GL_MULTISAMPLE                 YCI 0x809D


/* @constant GL_TEXTURE_1D,int
*/
//#define GL_TEXTURE_1D                  YCI 0x0DE0


/* @constant GL_PROXY_TEXTURE_1D,int
Can be used to check if texture fits into memory.
*/
//#define GL_PROXY_TEXTURE_1D            YCI 0x8063


/* @constant GL_TEXTURE_2D,int
*/
//#define GL_TEXTURE_2D                  YCI 0x0DE1


/* @constant GL_PROXY_TEXTURE_2D,int
Can be used to check if texture fits into memory.
*/
//#define GL_PROXY_TEXTURE_2D            YCI 0x8064


/* @constant GL_TEXTURE_2D,int
*/
//#define GL_TEXTURE_2D                  YCI 0x0DE1


/* @constant GL_PROXY_TEXTURE_2D,int
Can be used to check if texture fits into memory.
*/
//#define GL_PROXY_TEXTURE_2D            YCI 0x8064

/* @constant GL_TEXTURE_2D_ARRAY,int
*/
//#define GL_TEXTURE_2D_ARRAY            YCI 0x8C1A

/* @constant GL_PROXY_TEXTURE_2D_ARRAY,int
Can be used to check if texture fits into memory.
*/
//#define GL_PROXY_TEXTURE_2D_ARRAY      YCI 0x8C1B

/* @constant GL_TEXTURE_3D,int
*/
//#define GL_TEXTURE_3D                  YCI 0x806F


/* @constant GL_PROXY_TEXTURE_3D,int
Can be used to check if texture fits into memory.
*/
//#define GL_PROXY_TEXTURE_3D            YCI 0x8070


/* @constant GL_TEXTURE0,int
*/
//#define GL_TEXTURE0                    YCI 0x84c0


/* @constant GL_TEXTURE1,int
*/
//#define GL_TEXTURE1                    YCI 0x84c1


/* @constant GL_TEXTURE2,int
*/
//#define GL_TEXTURE2                    YCI 0x84c2


/* @constant GL_TEXTURE3,int
*/
//#define GL_TEXTURE3                    YCI 0x84c3


/* @constant GL_TEXTURE4,int
*/
//#define GL_TEXTURE4                    YCI 0x84c4


/* @constant GL_TEXTURE5,int
*/
//#define GL_TEXTURE5                    YCI 0x84c5


/* @constant GL_TEXTURE6,int
*/
//#define GL_TEXTURE6                    YCI 0x84c6


/* @constant GL_TEXTURE7,int
*/
//#define GL_TEXTURE7                    YCI 0x84c7


/* @constant GL_TEXTURE8,int
*/
//#define GL_TEXTURE8                    YCI 0x84c8


/* @constant GL_TEXTURE9,int
*/
//#define GL_TEXTURE9                    YCI 0x84c9


/* @constant GL_TEXTURE10,int
*/
//#define GL_TEXTURE10                   YCI 0x84ca


/* @constant GL_TEXTURE11,int
*/
//#define GL_TEXTURE11                   YCI 0x84cb


/* @constant GL_TEXTURE12,int
*/
//#define GL_TEXTURE12                   YCI 0x84cc


/* @constant GL_TEXTURE13,int
*/
//#define GL_TEXTURE13                   YCI 0x84cd


/* @constant GL_TEXTURE14,int
*/
//#define GL_TEXTURE14                   YCI 0x84ce


/* @constant GL_TEXTURE15,int
*/
//#define GL_TEXTURE16                   YCI 0x84cf


/* @constant GL_ACTIVE_TEXTURE,int
*/
//#define GL_ACTIVE_TEXTURE              YCI 0x84e0


/* @constant GL_CLIENT_ACTIVE_TEXTURE,int
*/
//#define GL_CLIENT_ACTIVE_TEXTURE       YCI 0x84e1


/* @constant GL_MAX_TEXTURE_UNITS,int
*/
//#define GL_MAX_TEXTURE_UNITS           YCI 0x84e2


/* @constant GL_MAX_TEXTURE_IMAGE_UNITS,int
*/
//#define GL_MAX_TEXTURE_IMAGE_UNITS     YCI 0x8872


/* @constant GL_MAX_TEXTURE_SIZE,int
*/
//#define GL_MAX_TEXTURE_SIZE            YCI 0x0D33


/* @constant GL_MAX_TEXTURE_COORDS,int
*/
//#define GL_MAX_TEXTURE_COORDS          YCI 0x8871


/* @constant GL_MAX_ELEMENTS_VERTICES,int
*/
//#define GL_MAX_ELEMENTS_VERTICES       YCI 0x80E8


/* @constant GL_MAX_ELEMENTS_INDICES,int
*/
//#define GL_MAX_ELEMENTS_INDICES        YCI 0x80E9


/* @constant GL_TEXTURE_MAG_FILTER,int
 */
//#define GL_TEXTURE_MAG_FILTER          YCI 0x2800


/* @constant GL_TEXTURE_MIN_FILTER,int
 */
//#define GL_TEXTURE_MIN_FILTER          YCI 0x2801


/* @constant GL_TEXTURE_WRAP_S,int
 */
//#define GL_TEXTURE_WRAP_S              YCI 0x2802


/* @constant GL_TEXTURE_WRAP_T,int
 */
//#define GL_TEXTURE_WRAP_T              YCI 0x2803


/* @constant GL_TEXTURE_WRAP_R,int
 */
//#define GL_TEXTURE_WRAP_R              YCI 0x8072


/* @constant GL_TEXTURE_MIN_LOD,int
 */
//#define GL_TEXTURE_MIN_LOD             YCI 0x813A


/* @constant GL_TEXTURE_MAX_LOD,int
 */
//#define GL_TEXTURE_MAX_LOD             YCI 0x813B


/* @constant GL_TEXTURE_BASE_LEVEL,int
 */
//#define GL_TEXTURE_BASE_LEVEL          YCI 0x813C


/* @constant GL_TEXTURE_MAX_LEVEL,int
 */
//#define GL_TEXTURE_MAX_LEVEL           YCI 0x813D


/* @constant GL_TEXTURE_PRIORITY,int
 */
//#define GL_TEXTURE_PRIORITY            YCI 0x8066


/* @constant GL_TEXTURE_RESIDENT,int
 */
//#define GL_TEXTURE_RESIDENT            YCI 0x8067


/* @constant GL_GENERATE_MIPMAP,int
 */
//#define GL_GENERATE_MIPMAP             YCI 0x8191


/* @constant GL_GENERATE_MIPMAP_HINT,int
 */
//#define GL_GENERATE_MIPMAP_HINT        YCI 0x8192


/* @constant GL_DEPTH_COMPONENT16,int
 */
//#define GL_DEPTH_COMPONENT16           YCI 0x81A5


/* @constant GL_DEPTH_COMPONENT24,int
 */
//#define GL_DEPTH_COMPONENT24           YCI 0x81A6


/* @constant GL_DEPTH_COMPONENT32,int
 */
//#define GL_DEPTH_COMPONENT32           YCI 0x81A7


/* @constant GL_DEPTH24_STENCIL8,int
 */
//#define GL_DEPTH24_STENCIL8            YCI 0x88F0


/* @constant GL_DEPTH32F_STENCIL8,int
 */
//#define GL_DEPTH32F_STENCIL8           YCI 0x8CAD


/* @constant GL_MIRRORED_REPEAT,int
 */
//#define GL_MIRRORED_REPEAT             YCI 0x8370


/* @constant GL_FRAGMENT_DEPTH,int
 */
//#define GL_FRAGMENT_DEPTH              YCI 0x8452


/* @constant GL_CLEAR,int
*/
//#define GL_CLEAR                       YCI 0x1500


/* @constant GL_SET,int
*/
//#define GL_SET                         YCI 0x150F


/* @constant GL_COPY,int
*/
//#define GL_COPY                        YCI 0x1503


/* @constant GL_COPY_INVERTED,int
*/
//#define GL_COPY_INVERTED               YCI 0x150c


/* @constant GL_NOOP,int
*/
//#define GL_NOOP                        YCI 0x1505


/* @constant GL_INVERT,int
*/
//#define GL_INVERT                      YCI 0x150a


/* @constant GL_AND,int
*/
//#define GL_AND                         YCI 0x1501


/* @constant GL_NAND,int
*/
//#define GL_NAND                        YCI 0x150e


/* @constant GL_OR,int
*/
//#define GL_OR                          YCI 0x1507


/* @constant GL_NOR,int
*/
//#define GL_NOR                         YCI 0x1508


/* @constant GL_XOR,int
*/
//#define GL_XOR                         YCI 0x1506


/* @constant GL_EQUIV,int
*/
//#define GL_EQUIV                       YCI 0x1509


/* @constant GL_AND_REVERSE,int
*/
//#define GL_AND_REVERSE                 YCI 0x1502


/* @constant GL_AND_INVERTED,int
*/
//#define GL_AND_INVERTED                YCI 0x1504


/* @constant GL_OR_REVERSE,int
*/
//#define GL_OR_REVERSE                  YCI 0x150b


/* @constant GL_OR_INVERTED,int
*/
//#define GL_OR_INVERTED                 YCI 0x150d


/* @constant GL_RENDER,int
*/
//#define GL_RENDER                      YCI 0x1C00


/* @constant GL_FEEDBACK,int
*/
//#define GL_FEEDBACK                    YCI 0x1C01


/* @constant GL_SELECT,int
*/
//#define GL_SELECT                      YCI 0x1C02


/* @constant GL_FLAT,int
*/
//#define GL_FLAT                        YCI 0x1D00


/* @constant GL_SMOOTH,int
*/
//#define GL_SMOOTH                      YCI 0x1D01


/* @constant GL_FRONT,int
*/
//#define GL_FRONT                       YCI 0x0404


/* @constant GL_BACK,int
*/
//#define GL_BACK                        YCI 0x0405


/* @constant GL_FRONT_AND_BACK,int
*/
//#define GL_FRONT_AND_BACK              YCI 0x0408


/* @constant GL_POINT,int
*/
//#define GL_POINT                       YCI 0x1b00


/* @constant GL_LINE,int
*/
//#define GL_LINE                        YCI 0x1b01


/* @constant GL_FILL,int
*/
//#define GL_FILL                        YCI 0x1b02


/* @constant GL_ZERO,int
*/
//#define GL_ZERO                        YCI 0x0


/* @constant GL_ONE,int
*/
//#define GL_ONE                         YCI 0x1


/* @constant GL_SRC_COLOR,int
*/
//#define GL_SRC_COLOR                   YCI 0x0300


/* @constant GL_ONE_MINUS_SRC_COLOR,int
*/
//#define GL_ONE_MINUS_SRC_COLOR         YCI 0x0301


/* @constant GL_SRC_ALPHA,int
*/
//#define GL_SRC_ALPHA                   YCI 0x0302


/* @constant GL_ONE_MINUS_SRC_ALPHA,int
*/
//#define GL_ONE_MINUS_SRC_ALPHA         YCI 0x0303


/* @constant GL_DST_ALPHA,int
*/
//#define GL_DST_ALPHA                   YCI 0x0304


/* @constant GL_ONE_MINUS_DST_ALPHA,int
*/
//#define GL_ONE_MINUS_DST_ALPHA         YCI 0x0305


/* @constant GL_DST_COLOR,int
*/
//#define GL_DST_COLOR                   YCI 0x0306


/* @constant GL_ONE_MINUS_DST_COLOR,int
*/
//#define GL_ONE_MINUS_DST_COLOR         YCI 0x0307


/* @constant GL_SRC_ALPHA_SATURATE,int
*/
//#define GL_SRC_ALPHA_SATURATE          YCI 0x0308


/* @constant GL_CONSTANT_COLOR,int
*/
//#define GL_CONSTANT_COLOR              YCI 0x8001


/* @constant GL_ONE_MINUS_CONSTANT_COLOR,int
*/
//#define GL_ONE_MINUS_CONSTANT_COLOR     YCI 0x8002


/* @constant GL_CONSTANT_ALPHA,int
*/
//#define GL_CONSTANT_ALPHA               YCI 0x8003


/* @constant GL_ONE_MINUS_CONSTANT_ALPHA,int
*/
//#define GL_ONE_MINUS_CONSTANT_ALPHA     YCI 0x8004


/* @constant GL_BLEND_COLOR,int
*/
//#define GL_BLEND_COLOR                  YCI 0x8005


/* @constant GL_CW,int
*/
//#define GL_CW                          YCI 0x0900


/* @constant GL_CCW,int
*/
//#define GL_CCW                         YCI 0x0901


/* @constant GL_KEEP,int
*/
//#define GL_KEEP                        YCI 0x1E00


/* @constant GL_REPLACE,int
*/
//#define GL_REPLACE                     YCI 0x1E01


/* @constant GL_INCR,int
*/
//#define GL_INCR                        YCI 0x1E02


/* @constant GL_INCR_WRAP,int
*/
//#define GL_INCR_WRAP                   YCI 0x8507


/* @constant GL_DECR,int
*/
//#define GL_DECR                        YCI 0x1E03


/* @constant GL_DECR_WRAP,int
*/
//#define GL_DECR_WRAP                   YCI 0x8508


/* @constant GL_NEVER,int
*/
//#define GL_NEVER                       YCI 0x0200


/* @constant GL_LESS,int
*/
//#define GL_LESS                        YCI 0x0201


/* @constant GL_EQUAL,int
*/
//#define GL_EQUAL                       YCI 0x0202


/* @constant GL_LEQUAL,int
*/
//#define GL_LEQUAL                      YCI 0x0203


/* @constant GL_GREATER,int
*/
//#define GL_GREATER                     YCI 0x0204


/* @constant GL_NOTEQUAL,int
*/
//#define GL_NOTEQUAL                    YCI 0x0205


/* @constant GL_GEQUAL,int
*/
//#define GL_GEQUAL                      YCI 0x0206


/* @constant GL_ALWAYS,int
*/
//#define GL_ALWAYS                      YCI 0x0207


/* @constant GL_POLYGON_OFFSET_LINE,int
*/
//#define GL_POLYGON_OFFSET_LINE         YCI 0x2a02


/* @constant GL_VERTEX_ARRAY,int
*/
//#define GL_VERTEX_ARRAY                YCI 0x8074


/* @constant GL_NORMAL_ARRAY,int
*/
//#define GL_NORMAL_ARRAY                YCI 0x8075


/* @constant GL_COLOR_ARRAY,int
*/
//#define GL_COLOR_ARRAY                 YCI 0x8076


/* @constant GL_TEXTURE_COORD_ARRAY,int
*/
//#define GL_TEXTURE_COORD_ARRAY         YCI 0x8078


/* @constant GL_COLOR_LOGIC_OP,int
*/
//#define GL_COLOR_LOGIC_OP              YCI 0x0bf2


/* @constant GL_S,int
*/
//#define GL_S                           YCI 0x2000


/* @constant GL_T,int
*/
//#define GL_T                           YCI 0x2001


/* @constant GL_R,int
*/
//#define GL_R                           YCI 0x2002


/* @constant GL_Q,int
*/
//#define GL_Q                           YCI 0x2003


/* @constant GL_TEXTURE_GEN_S,int
*/
//#define GL_TEXTURE_GEN_S               YCI 0x0c60


/* @constant GL_TEXTURE_GEN_T,int
*/
//#define GL_TEXTURE_GEN_T               YCI 0x0c61


/* @constant GL_TEXTURE_GEN_R,int
*/
//#define GL_TEXTURE_GEN_R               YCI 0x0c62


/* @constant GL_TEXTURE_GEN_Q,int
*/
//#define GL_TEXTURE_GEN_Q               YCI 0x0c63


/* @constant GL_TEXTURE_ENV,int
*/
//#define GL_TEXTURE_ENV                 YCI 0x2300


/* @constant GL_TEXTURE_FILTER_CONTROL,int
*/
//#define GL_TEXTURE_FILTER_CONTROL      YCI 0x8500


/* @constant GL_POINT_SPRITE,int
*/
//#define GL_POINT_SPRITE                YCI 0x8861


/* @constant GL_COORD_REPLACE,int
*/
//#define GL_COORD_REPLACE               YCI 0x8862


/* @constant GL_TEXTURE_ENV_MODE,int
*/
//#define GL_TEXTURE_ENV_MODE            YCI 0x2200


/* @constant GL_TEXTURE_ENV_COLOR,int
*/
//#define GL_TEXTURE_ENV_COLOR           YCI 0x2201


/* @constant GL_TEXTURE_LOD_BIAS,int
*/
//#define GL_TEXTURE_LOD_BIAS            YCI 0x8501


/* @constant GL_TEXTURE_COMPARE_MODE,int
*/
//#define GL_TEXTURE_COMPARE_MODE        YCI 0x884C


/* @constant GL_TEXTURE_COMPARE_FUNC,int
*/
//#define GL_TEXTURE_COMPARE_FUNC        YCI 0x884D


/* @constant GL_TEXTURE_COMPARE_R_TO_TEXTURE,int
*/
//#define GL_TEXTURE_COMPARE_R_TO_TEXTURE YCI 0x884E


/* @constant GL_COMBINE,int
*/
//#define GL_COMBINE                     YCI 0x8570


/* @constant GL_COMBINE_RGB,int
*/
//#define GL_COMBINE_RGB                 YCI 0x8571


/* @constant GL_COMBINE_ALPHA,int
*/
//#define GL_COMBINE_ALPHA               YCI 0x8572


/* @constant GL_SRC0_RGB,int
*/
//#define GL_SRC0_RGB                    YCI 0x8580


/* @constant GL_SRC1_RGB,int
*/
//#define GL_SRC1_RGB                    YCI 0x8581


/* @constant GL_SRC2_RGB,int
*/
//#define GL_SRC2_RGB                    YCI 0x8582


/* @constant GL_SRC0_ALPHA,int
*/
//#define GL_SRC0_ALPHA                  YCI 0x8588


/* @constant GL_SRC1_ALPHA,int
*/
//#define GL_SRC1_ALPHA                  YCI 0x8589


/* @constant GL_SRC2_ALPHA,int
*/
//#define GL_SRC2_ALPHA                  YCI 0x858A


/* @constant GL_OPERAND0_RGB,int
*/
//#define GL_OPERAND0_RGB                YCI 0x8590


/* @constant GL_OPERAND1_RGB,int
*/
//#define GL_OPERAND1_RGB                YCI 0x8591


/* @constant GL_OPERAND2_RGB,int
*/
//#define GL_OPERAND2_RGB                YCI 0x8592


/* @constant GL_OPERAND0_ALPHA,int
*/
//#define GL_OPERAND0_ALPHA              YCI 0x8598


/* @constant GL_OPERAND1_ALPHA,int
*/
//#define GL_OPERAND1_ALPHA              YCI 0x8599


/* @constant GL_OPERAND2_ALPHA,int
*/
//#define GL_OPERAND2_ALPHA              YCI 0x859A


/* @constant GL_RGB_SCALE,int
*/
//#define GL_RGB_SCALE                   YCI 0x8573


/* @constant GL_ALPHA_SCALE,int
*/
//#define GL_ALPHA_SCALE                 YCI 0x0d1c


/* @constant GL_ADD_SIGNED,int
*/
//#define GL_ADD_SIGNED                  YCI 0x8574


/* @constant GL_INTERPOLATE,int
*/
//#define GL_INTERPOLATE                 YCI 0x8575


/* @constant GL_SUBTRACT,int
*/
//#define GL_SUBTRACT                    YCI 0x84E7


/* @constant GL_CONSTANT,int
*/
//#define GL_CONSTANT                    YCI 0x8576


/* @constant GL_PRIMARY_COLOR,int
*/
//#define GL_PRIMARY_COLOR               YCI 0x8577


/* @constant GL_PREVIOUS,int
*/
//#define GL_PREVIOUS                    YCI 0x8578


/* @constant GL_DOT3_RGB,int
*/
//#define GL_DOT3_RGB                    YCI 0x86AE


/* @constant GL_DOT3_RGBA,int
*/
//#define GL_DOT3_RGBA                   YCI 0x86AF


/* @constant GL_DECAL,int
*/
//#define GL_DECAL                       YCI 0x2101


/* @constant GL_MODULATE,int
*/
//#define GL_MODULATE                    YCI 0x2100


/* @constant GL_EYE_LINEAR,int
*/
//#define GL_EYE_LINEAR                  YCI 0x2400


/* @constant GL_OBJECT_LINEAR,int
*/
//#define GL_OBJECT_LINEAR               YCI 0x2401


/* @constant GL_SPHERE_MAP,int
*/
//#define GL_SPHERE_MAP                  YCI 0x2402


/* @constant GL_TEXTURE_GEN_MODE,int
*/
//#define GL_TEXTURE_GEN_MODE            YCI 0x2500


/* @constant GL_OBJECT_PLANE,int
*/
//#define GL_OBJECT_PLANE                YCI 0x2501


/* @constant GL_EYE_PLANE,int
*/
//#define GL_EYE_PLANE                   YCI 0x2502


/* @constant GL_COLOR,int
*/
//#define GL_COLOR                       YCI 0x1800


/* @constant GL_DEPTH,int
*/
//#define GL_DEPTH                       YCI 0x1801


/* @constant GL_STENCIL,int
*/
//#define GL_STENCIL                     YCI 0x1802


/* @constant GL_COLOR_INDEX,int
*/
//#define GL_COLOR_INDEX                 YCI 0x1900


/* @constant GL_STENCIL_INDEX,int
*/
//#define GL_STENCIL_INDEX               YCI 0x1901


/* @constant GL_DEPTH_COMPONENT,int
*/
//#define GL_DEPTH_COMPONENT             YCI 0x1902


/* @constant GL_RED,int
*/
//#define GL_RED                         YCI 0x1903


/* @constant GL_GREEN,int
*/
//#define GL_GREEN                       YCI 0x1904


/* @constant GL_BLUE,int
*/
//#define GL_BLUE                        YCI 0x1905


/* @constant GL_ALPHA,int
*/
//#define GL_ALPHA                       YCI 0x1906


/* @constant GL_TEXTURE_SWIZZLE_R,int
GL_ARB_texture_swizzle (not available on macOS / GL 4.1 core)
*/
//#define GL_TEXTURE_SWIZZLE_R              YCI 0x8E42


/* @constant GL_TEXTURE_SWIZZLE_G,int
GL_ARB_texture_swizzle (not available on macOS / GL 4.1 core)
*/
//#define GL_TEXTURE_SWIZZLE_G              YCI 0x8E43


/* @constant GL_TEXTURE_SWIZZLE_B,int
GL_ARB_texture_swizzle (not available on macOS / GL 4.1 core)
*/
//#define GL_TEXTURE_SWIZZLE_B              YCI 0x8E44


/* @constant GL_TEXTURE_SWIZZLE_A,int
GL_ARB_texture_swizzle (not available on macOS / GL 4.1 core)
*/
//#define GL_TEXTURE_SWIZZLE_A              YCI 0x8E45


/* @constant GL_TEXTURE_SWIZZLE_RGBA,int
GL_ARB_texture_swizzle (not available on macOS / GL 4.1 core)
*/
//#define GL_TEXTURE_SWIZZLE_RGBA           YCI 0x8E46


/* @constant GL_RGB,int
*/
//#define GL_RGB                         YCI 0x1907


/* @constant GL_RGBA,int
*/
//#define GL_RGBA                        YCI 0x1908


/* @constant GL_LUMINANCE,int
*/
//#define GL_LUMINANCE                   YCI 0x1909


/* @constant GL_LUMINANCE8,int
*/
//#define GL_LUMINANCE8                  YCI 0x8040


/* @constant GL_LUMINANCE_ALPHA,int
*/
//#define GL_LUMINANCE_ALPHA             YCI 0x190a


/* @constant GL_LUMINANCE8_ALPHA8,int
*/
//#define GL_LUMINANCE8_ALPHA8           YCI 0x8045


/* @constant GL_INTENSITY,int
*/
//#define GL_INTENSITY                   YCI 0x8049


/* @constant GL_INTENSITY8,int
*/
//#define GL_INTENSITY8                  YCI 0x804b


/* @constant GL_RGB5,int
*/
//#define GL_RGB5                        YCI 0x8050


/* @constant GL_RGB8,int
*/
//#define GL_RGB8                        YCI 0x8051


/* @constant GL_RGB5_A1,int
*/
//#define GL_RGB5_A1                     YCI 0x8057


/* @constant GL_RGBA8,int
*/
//#define GL_RGBA8                       YCI 0x8058


/* @constant GL_RGB10_A2,int
*/
//#define GL_RGB10_A2                    YCI 0x8059


/* @constant GL_R3_G3_B2,int
*/
//#define GL_R3_G3_B2                    YCI 0x2a10


/* @constant GL_ALL_ATTRIB_BITS,int
*/
//#define GL_ALL_ATTRIB_BITS             YCI 0x000fffff


/* @constant GL_CLIENT_ALL_ATTRIB_BITS,int
*/
//#define GL_CLIENT_ALL_ATTRIB_BITS      YCI 0xffffffff


/* @constant GL_CURRENT_BIT,int
*/
//#define GL_CURRENT_BIT                 YCI 0x1


/* @constant GL_ENABLE_BIT,int
*/
//#define GL_ENABLE_BIT                  YCI 0x2000


/* @constant GL_EVAL_BIT,int
*/
//#define GL_EVAL_BIT                    YCI 0x10000


/* @constant GL_FOG_BIT,int
*/
//#define GL_FOG_BIT                     YCI 0x80


/* @constant GL_HINT_BIT,int
*/
//#define GL_HINT_BIT                    YCI 0x8000


/* @constant GL_LIGHTING_BIT,int
*/
//#define GL_LIGHTING_BIT                YCI 0x40


/* @constant GL_LINE_BIT,int
*/
//#define GL_LINE_BIT                    YCI 0x4


/* @constant GL_LIST_BIT,int
*/
//#define GL_LIST_BIT                    YCI 0x20000


/* @constant GL_PIXEL_MODE_BIT,int
*/
//#define GL_PIXEL_MODE_BIT              YCI 0x20


/* @constant GL_POINT_BIT,int
*/
//#define GL_POINT_BIT                   YCI 0x2


/* @constant GL_POLYGON_BIT,int
*/
//#define GL_POLYGON_BIT                 YCI 0x8


/* @constant GL_POLYGON_STIPPLE_BIT,int
*/
//#define GL_POLYGON_STIPPLE_BIT         YCI 0x10


/* @constant GL_SCISSOR_BIT,int
*/
//#define GL_SCISSOR_BIT                 YCI 0x80000


/* @constant GL_TEXTURE_BIT,int
*/
//#define GL_TEXTURE_BIT                 YCI 0x40000


/* @constant GL_TRANSFORM_BIT,int
*/
//#define GL_TRANSFORM_BIT               YCI 0x1000


/* @constant GL_VIEWPORT_BIT,int
*/
//#define GL_VIEWPORT_BIT                YCI 0x800


/* @constant GL_ALL_ATTRIBS_BITS,int
*/
//#define GL_ALL_ATTRIBS_BITS            YCI 0xFFFFF


/* @constant GL_CLIENT_PIXEL_STORE_BIT,int
*/
//#define GL_CLIENT_PIXEL_STORE_BIT      YCI 0x1


/* @constant GL_CLIENT_VERTEX_ARRAY_BIT,int
*/
//#define GL_CLIENT_VERTEX_ARRAY_BIT     YCI 0x2


/* @constant GL_CLIENT_ALL_ATTRIB_BITS,int
*/
//#define GL_CLIENT_ALL_ATTRIB_BITS      YCI 0xFFFFFFFF


/* @constant GL_BGRA,int
*/
//#define GL_BGRA                        YCI 0x80E1
//#define GL_BGRA_EXT                    YCI 0x80E1
//#define GL_BGRA8_EXT                   YCI 0x93A1

/* @constant GL_BGR,int
*/
//#define GL_BGR                         YCI 0x80E0
//#define GL_BGR_EXT                     YCI 0x80E0


/* @constant GL_SPOT_EXPONENT,int
*/
//#define GL_SPOT_EXPONENT               YCI 0x1205


/* @constant GL_SPOT_CUTOFF,int
*/
//#define GL_SPOT_CUTOFF                 YCI 0x1206


/* @constant GL_CONSTANT_ATTENUATION,int
*/
//#define GL_CONSTANT_ATTENUATION        YCI 0x1207


/* @constant GL_LINEAR_ATTENUATION,int
*/
//#define GL_LINEAR_ATTENUATION          YCI 0x1208


/* @constant GL_QUADRATIC_ATTENUATION,int
*/
//#define GL_QUADRATIC_ATTENUATION       YCI 0x1209


/* @constant GL_POINTS,int
*/
//#define GL_POINTS                      YCI 0x0000


/* @constant GL_LINES,int
*/
//#define GL_LINES                       YCI 0x0001


/* @constant GL_LINE_STRIP,int
*/
//#define GL_LINE_STRIP                  YCI 0x0003


/* @constant GL_LINE_LOOP,int
*/
//#define GL_LINE_LOOP                   YCI 0x0002


/* @constant GL_TRIANGLES,int
*/
//#define GL_TRIANGLES                   YCI 0x0004


/* @constant GL_TRIANGLE_STRIP,int
*/
//#define GL_TRIANGLE_STRIP              YCI 0x0005


/* @constant GL_TRIANGLE_FAN,int
*/
//#define GL_TRIANGLE_FAN                YCI 0x0006


/* @constant GL_QUADS,int
*/
//#define GL_QUADS                       YCI 0x0007


/* @constant GL_QUAD_STRIP,int
*/
//#define GL_QUAD_STRIP                  YCI 0x0008


/* @constant GL_POLYGON,int
*/
//#define GL_POLYGON                     YCI 0x0009


/* @constant GL_NEAREST,int
*/
//#define GL_NEAREST                     YCI 0x2600


/* @constant GL_LINEAR,int
*/
//#define GL_LINEAR                      YCI 0x2601


/* @constant GL_CLAMP,int
*/
//#define GL_CLAMP                       YCI 0x2900


/* @constant GL_REPEAT,int
*/
//#define GL_REPEAT                      YCI 0x2901


/* @constant GL_EXP,int
*/
//#define GL_EXP                         YCI 0x0800


/* @constant GL_EXP2,int
*/
//#define GL_EXP2                        YCI 0x0801


/* @constant GL_COLOR_BUFFER_BIT,int
*/
//#define GL_COLOR_BUFFER_BIT            YCI 0x4000


/* @constant GL_DEPTH_BUFFER_BIT,int
*/
//#define GL_DEPTH_BUFFER_BIT            YCI 0x100


/* @constant GL_ACCUM_BUFFER_BIT,int
*/
//#define GL_ACCUM_BUFFER_BIT            YCI 0x200


/* @constant GL_STENCIL_BUFFER_BIT,int
*/
//#define GL_STENCIL_BUFFER_BIT          YCI 0x400


/* @constant GL_CLIP_PLANE0,int
*/
//#define GL_CLIP_PLANE0                 YCI 0x3000


/* @constant GL_CLIP_PLANE1,int
*/
//#define GL_CLIP_PLANE1                 YCI 0x3001


/* @constant GL_CLIP_PLANE2,int
*/
//#define GL_CLIP_PLANE2                 YCI 0x3002


/* @constant GL_CLIP_PLANE3,int
*/
//#define GL_CLIP_PLANE3                 YCI 0x3003


/* @constant GL_CLIP_PLANE4,int
*/
//#define GL_CLIP_PLANE4                 YCI 0x3004


/* @constant GL_CLIP_PLANE5,int
*/
//#define GL_CLIP_PLANE5                 YCI 0x3005


/* @constant GL_VENDOR,int
*/
//#define GL_VENDOR                      YCI 0x1F00


/* @constant GL_RENDERER,int
*/
//#define GL_RENDERER                    YCI 0x1F01


/* @constant GL_VERSION,int
*/
//#define GL_VERSION                     YCI 0x1F02


/* @constant GL_EXTENSIONS,int
*/
//#define GL_EXTENSIONS                  YCI 0x1F03



/* @constant GL_ACCUM,int
*/
//#define GL_ACCUM                       YCI 0x0100


/* @constant GL_LOAD,int
*/
//#define GL_LOAD                          YCI  0x0101


/* @constant GL_RETURN,int
*/
//#define GL_RETURN                        YCI  0x0102


/* @constant GL_MULT,int
*/
//#define GL_MULT                          YCI  0x0103


/* @constant GL_ADD,int
*/
//#define GL_ADD                           YCI  0x0104




// Extension constants:


/* @constant GL_POINT_SIZE_MIN,int
*/
//#define GL_POINT_SIZE_MIN                 YCI 0x8126


/* @constant GL_POINT_SIZE_MAX,int
*/
//#define GL_POINT_SIZE_MAX                 YCI 0x8127


/* @constant GL_POINT_FADE_THRESHOLD_SIZE,int
*/
//#define GL_POINT_FADE_THRESHOLD_SIZE      YCI 0x8128


/* @constant GL_POINT_DISTANCE_ATTENUATION,int
*/
//#define GL_POINT_DISTANCE_ATTENUATION     YCI 0x8129


/* @constant GL_POINT_SPRITE_COORD_ORIGIN,int
*/
//#define GL_POINT_SPRITE_COORD_ORIGIN      YCI 0x8CA0


/* @constant GL_LOWER_LEFT,int
*/
//#define GL_LOWER_LEFT                     YCI 0x8CA1


/* @constant GL_UPPER_LEFT,int
*/
//#define GL_UPPER_LEFT                     YCI 0x8CA2


/* @constant GL_CLAMP_TO_BORDER,int
*/
//#define GL_CLAMP_TO_BORDER                YCI 0x812D


/* @constant GL_CLAMP_TO_EDGE,int
*/
//#define GL_CLAMP_TO_EDGE                  YCI 0x812F


/* @constant GL_NORMAL_MAP,int
*/
//#define GL_NORMAL_MAP                     YCI 0x8511


/* @constant GL_REFLECTION_MAP,int
*/
//#define GL_REFLECTION_MAP                 YCI 0x8512


/* @constant GL_TEXTURE_CUBE_MAP,int
*/
//#define GL_TEXTURE_CUBE_MAP               YCI 0x8513


/* @constant GL_TEXTURE_BINDING_CUBE_MAP,int
*/
//#define GL_TEXTURE_BINDING_CUBE_MAP       YCI 0x8514


/* @constant GL_TEXTURE_CUBE_MAP_POSITIVE_X,int
*/
//#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    YCI 0x8515


/* @constant GL_TEXTURE_CUBE_MAP_NEGATIVE_X,int
*/
//#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    YCI 0x8516


/* @constant GL_TEXTURE_CUBE_MAP_POSITIVE_Y,int
*/
//#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    YCI 0x8517


/* @constant GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,int
*/
//#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    YCI 0x8518


/* @constant GL_TEXTURE_CUBE_MAP_POSITIVE_Z,int
*/
//#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    YCI 0x8519


/* @constant GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,int
*/
//#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    YCI 0x851A


//
//
// draw buffers
//
//

/* @constant GL_MAX_DRAW_BUFFERS,int
*/
//#define GL_MAX_DRAW_BUFFERS               YCI 0x8824


/* @constant GL_DRAW_BUFFER0,int
*/
//#define GL_DRAW_BUFFER0                   YCI 0x8825


/* @constant GL_DRAW_BUFFER1,int
*/
//#define GL_DRAW_BUFFER1                   YCI 0x8826


/* @constant GL_DRAW_BUFFER2,int
*/
//#define GL_DRAW_BUFFER2                   YCI 0x8827


/* @constant GL_DRAW_BUFFER3,int
*/
//#define GL_DRAW_BUFFER3                   YCI 0x8828


/* @constant GL_DRAW_BUFFER4,int
*/
//#define GL_DRAW_BUFFER4                   YCI 0x8829


/* @constant GL_DRAW_BUFFER5,int
*/
//#define GL_DRAW_BUFFER5                   YCI 0x882A


/* @constant GL_DRAW_BUFFER6,int
*/
//#define GL_DRAW_BUFFER6                   YCI 0x882B


/* @constant GL_DRAW_BUFFER7,int
*/
//#define GL_DRAW_BUFFER7                   YCI 0x882C



//
//
// glBlendEquation (1.4) constants:
//
//

/* @constant GL_FUNC_ADD,int
*/
//#define GL_FUNC_ADD                       YCI 0x8006


/* @constant GL_MIN,int
*/
//#define GL_MIN                            YCI 0x8007


/* @constant GL_MAX,int
*/
//#define GL_MAX                            YCI 0x8008


/* @constant GL_BLEND_EQUATION_RGB,int
*/
//#define GL_BLEND_EQUATION_RGB             YCI 0x8009


/* @constant GL_BLEND_EQUATION_ALPHA,int
*/
//#define GL_BLEND_EQUATION_ALPHA           YCI 0x883D


/* @constant GL_FUNC_SUBTRACT,int
*/
//#define GL_FUNC_SUBTRACT                  YCI 0x800A


/* @constant GL_FUNC_REVERSE_SUBTRACT,int
*/
//#define GL_FUNC_REVERSE_SUBTRACT          YCI 0x800B


//
//
// OpenGL 1.5+ buffer objects
//
//


/* @constant GL_ARRAY_BUFFER,int
*/
//#define GL_ARRAY_BUFFER                   YCI 0x8892


/* @constant GL_ELEMENT_ARRAY_BUFFER,int
*/
//#define GL_ELEMENT_ARRAY_BUFFER           YCI 0x8893


/* @constant GL_ARRAY_BUFFER_BINDING,int
*/
//#define GL_ARRAY_BUFFER_BINDING           YCI 0x8894


/* @constant GL_ELEMENT_ARRAY_BUFFER_BINDING,int
*/
//#define GL_ELEMENT_ARRAY_BUFFER_BINDING   YCI 0x8895


/* @constant GL_VERTEX_ARRAY_BUFFER_BINDING,int
*/
//#define GL_VERTEX_ARRAY_BUFFER_BINDING    YCI 0x8896


/* @constant GL_NORMAL_ARRAY_BUFFER_BINDING,int
*/
//#define GL_NORMAL_ARRAY_BUFFER_BINDING    YCI 0x8897


/* @constant GL_COLOR_ARRAY_BUFFER_BINDING,int
*/
//#define GL_COLOR_ARRAY_BUFFER_BINDING     YCI 0x8898


/* @constant GL_INDEX_ARRAY_BUFFER_BINDING,int
*/
//#define GL_INDEX_ARRAY_BUFFER_BINDING     YCI 0x8899


/* @constant GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING,int
*/
//#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING YCI 0x889A


/* @constant GL_EDGE_FLAG_ARRAY_BUFFER_BINDING,int
*/
//#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING YCI 0x889B


/* @constant GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING,int
*/
//#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING YCI 0x889C


/* @constant GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING,int
*/
//#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING YCI 0x889D


/* @constant GL_WEIGHT_ARRAY_BUFFER_BINDING,int
*/
//#define GL_WEIGHT_ARRAY_BUFFER_BINDING    YCI 0x889E


/* @constant GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,int
*/
//#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING YCI 0x889F


/* @constant GL_READ_ONLY,int
*/
//#define GL_READ_ONLY                      YCI 0x88B8


/* @constant GL_WRITE_ONLY,int
*/
//#define GL_WRITE_ONLY                     YCI 0x88B9


/* @constant GL_READ_WRITE,int
*/
//#define GL_READ_WRITE                     YCI 0x88BA


/* @constant GL_BUFFER_ACCESS,int
*/
//#define GL_BUFFER_ACCESS                  YCI 0x88BB


/* @constant GL_BUFFER_MAPPED,int
*/
//#define GL_BUFFER_MAPPED                  YCI 0x88BC


/* @constant GL_BUFFER_MAP_POINTER,int
*/
//#define GL_BUFFER_MAP_POINTER             YCI 0x88BD


/* @constant GL_STREAM_DRAW,int
*/
//#define GL_STREAM_DRAW                    YCI 0x88E0


/* @constant GL_STREAM_READ,int
*/
//#define GL_STREAM_READ                    YCI 0x88E1


/* @constant GL_STREAM_COPY,int
*/
//#define GL_STREAM_COPY                    YCI 0x88E2


/* @constant GL_STATIC_DRAW,int
*/
//#define GL_STATIC_DRAW                    YCI 0x88E4


/* @constant GL_STATIC_READ,int
*/
//#define GL_STATIC_READ                    YCI 0x88E5


/* @constant GL_STATIC_COPY,int
*/
//#define GL_STATIC_COPY                    YCI 0x88E6


/* @constant GL_DYNAMIC_DRAW,int
*/
//#define GL_DYNAMIC_DRAW                   YCI 0x88E8


/* @constant GL_DYNAMIC_READ,int
*/
//#define GL_DYNAMIC_READ                   YCI 0x88E9


/* @constant GL_DYNAMIC_COPY,int
*/
//#define GL_DYNAMIC_COPY                   YCI 0x88EA


/* @constant GL_MAP_READ_BIT,int
*/
//#define GL_MAP_READ_BIT                   YCI 0x0001

/* @constant GL_MAP_WRITE_BIT,int
*/
//#define GL_MAP_WRITE_BIT                  YCI 0x0002

/* @constant GL_MAP_PERSISTENT_BIT,int
*/
//#define GL_MAP_PERSISTENT_BIT             0x????

/* @constant GL_MAP_COHERENT_BIT,int
*/
//#define GL_MAP_COHERENT_BIT               0x????

/* @constant GL_MAP_INVALIDATE_RANGE_BIT,int
*/
//#define GL_MAP_INVALIDATE_RANGE_BIT       YCI 0x0004

/* @constant GL_MAP_INVALIDATE_BUFFER_BIT,int
*/
//#define GL_MAP_INVALIDATE_BUFFER_BIT      YCI 0x0008

/* @constant GL_MAP_FLUSH_EXPLICIT_BIT,int
*/
//#define GL_MAP_FLUSH_EXPLICIT_BIT         YCI 0x0010

/* @constant GL_MAP_UNSYNCHRONIZED_BIT,int
*/
//#define GL_MAP_UNSYNCHRONIZED_BIT         YCI 0x0020





//
//
// Opengl 2.1 extension constants
//
//

/* @constant GL_PIXEL_PACK_BUFFER,int
 */
//#define GL_PIXEL_PACK_BUFFER              YCI 0x88EB


/* @constant GL_PIXEL_UNPACK_BUFFER,int
 */
//#define GL_PIXEL_UNPACK_BUFFER            YCI 0x88EC


/* @constant GL_PIXEL_PACK_BUFFER_BINDING,int
 */
//#define GL_PIXEL_PACK_BUFFER_BINDING      YCI 0x88ED


/* @constant GL_PIXEL_UNPACK_BUFFER_BINDING,int
 */
//#define GL_PIXEL_UNPACK_BUFFER_BINDING    YCI 0x88EF


//
//
// Framebuffer objects (FBO)
//
//

/* @constant GL_FRAMEBUFFER,int
Accepted by the <target> parameter of §glBindFramebuffer, §glCheckFramebufferStatus, §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
 */
//#define GL_FRAMEBUFFER                    YCI 0x8D40


/* @constant GL_RENDERBUFFER,int
Accepted by the <target> parameter of §glBindRenderbuffer, §glRenderbufferStorage, §glGetRenderbufferParameteri, and returned by §zglGetFramebufferAttachmentParameteri
 */
//#define GL_RENDERBUFFER                   YCI 0x8D41


/* @constant GL_STENCIL_INDEX1,int
Accepted by the <internalformat> parameter of §glRenderbufferStorage
*/
//#define GL_STENCIL_INDEX1                 YCI 0x8D46


/* @constant GL_STENCIL_INDEX4,int
Accepted by the <internalformat> parameter of §glRenderbufferStorage
*/
//#define GL_STENCIL_INDEX4                 YCI 0x8D47


/* @constant GL_STENCIL_INDEX8,int
Accepted by the <internalformat> parameter of §glRenderbufferStorage
*/
//#define GL_STENCIL_INDEX8                 YCI 0x8D48


/* @constant GL_STENCIL_INDEX16,int
Accepted by the <internalformat> parameter of §glRenderbufferStorage
*/
//#define GL_STENCIL_INDEX16                YCI 0x8D49


/* @constant GL_RENDERBUFFER_WIDTH,int
Accepted by the <pname> parameter of §zglGetRenderbufferParameteri
 */
//#define GL_RENDERBUFFER_WIDTH             YCI 0x8D42


/* @constant GL_RENDERBUFFER_HEIGHT,int
Accepted by the <pname> parameter of §zglGetRenderbufferParameteri
 */
//#define GL_RENDERBUFFER_HEIGHT            YCI 0x8D43


/* @constant GL_RENDERBUFFER_INTERNAL_FORMAT,int
Accepted by the <pname> parameter of §zglGetRenderbufferParameteri
 */
//#define GL_RENDERBUFFER_INTERNAL_FORMAT   YCI 0x8D44


/* @constant GL_RENDERBUFFER_RED_SIZE,int
Accepted by the <pname> parameter of §zglGetRenderbufferParameteri
 */
//#define GL_RENDERBUFFER_RED_SIZE          YCI 0x8D50


/* @constant GL_RENDERBUFFER_GREEN_SIZE,int
Accepted by the <pname> parameter of §zglGetRenderbufferParameteri
 */
//#define GL_RENDERBUFFER_GREEN_SIZE        YCI 0x8D51


/* @constant GL_RENDERBUFFER_BLUE_SIZE,int
Accepted by the <pname> parameter of §zglGetRenderbufferParameteri
 */
//#define GL_RENDERBUFFER_BLUE_SIZE         YCI 0x8D52


/* @constant GL_RENDERBUFFER_ALPHA_SIZE,int
Accepted by the <pname> parameter of §zglGetRenderbufferParameteri
 */
//#define GL_RENDERBUFFER_ALPHA_SIZE        YCI 0x8D53


/* @constant GL_RENDERBUFFER_DEPTH_SIZE,int
Accepted by the <pname> parameter of §zglGetRenderbufferParameteri
 */
//#define GL_RENDERBUFFER_DEPTH_SIZE        YCI 0x8D54


/* @constant GL_RENDERBUFFER_STENCIL_SIZE,int
Accepted by the <pname> parameter of §zglGetRenderbufferParameteri
 */
//#define GL_RENDERBUFFER_STENCIL_SIZE      YCI 0x8D55


/* @constant GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,int
Accepted by the <pname> parameter of §zglGetFramebufferAttachmentParameteri
 */
//#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE  YCI 0x8CD0


/* @constant GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,int
Accepted by the <pname> parameter of §zglGetFramebufferAttachmentParameteri
 */
//#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME    YCI 0x8CD1


/* @constant GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,int
Accepted by the <pname> parameter of §zglGetFramebufferAttachmentParameteri
 */
//#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL  YCI 0x8CD2


/* @constant GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE,int
Accepted by the <pname> parameter of §zglGetFramebufferAttachmentParameteri
 */
//#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE  YCI 0x8CD3


/* @constant GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET,int
Accepted by the <pname> parameter of §zglGetFramebufferAttachmentParameteri
 */
//#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET  YCI 0x8CD3


/* @constant GL_READ_FRAMEBUFFER,int
 */
//#define GL_READ_FRAMEBUFFER               YCI 0x8CA8


/* @constant GL_DRAW_FRAMEBUFFER,int
 */
//#define GL_DRAW_FRAMEBUFFER               YCI 0x8CA9


/* @constant GL_DEPTH_STENCIL_ATTACHMENT,int
 */
//#define GL_DEPTH_STENCIL_ATTACHMENT       YCI 0x821A


/* @constant GL_COLOR_ATTACHMENT0,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT0              YCI 0x8CE0


/* @constant GL_COLOR_ATTACHMENT1,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT1              YCI 0x8CE1


/* @constant GL_COLOR_ATTACHMENT2,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT2              YCI 0x8CE2


/* @constant GL_COLOR_ATTACHMENT3,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT3              YCI 0x8CE3


/* @constant GL_COLOR_ATTACHMENT4,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT4              YCI 0x8CE4


/* @constant GL_COLOR_ATTACHMENT5,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT5              YCI 0x8CE5


/* @constant GL_COLOR_ATTACHMENT6,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT6              YCI 0x8CE6


/* @constant GL_COLOR_ATTACHMENT7,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT7              YCI 0x8CE7


/* @constant GL_COLOR_ATTACHMENT8,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT8              YCI 0x8CE8


/* @constant GL_COLOR_ATTACHMENT9,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT9              YCI 0x8CE9


/* @constant GL_COLOR_ATTACHMENT10,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT10             YCI 0x8CEA


/* @constant GL_COLOR_ATTACHMENT11,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT11             YCI 0x8CEB


/* @constant GL_COLOR_ATTACHMENT12,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT12             YCI 0x8CEC


/* @constant GL_COLOR_ATTACHMENT13,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT13             YCI 0x8CED


/* @constant GL_COLOR_ATTACHMENT14,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT14             YCI 0x8CEE


/* @constant GL_COLOR_ATTACHMENT15,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_COLOR_ATTACHMENT15             YCI 0x8CEF


/* @constant GL_DEPTH_ATTACHMENT,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_DEPTH_ATTACHMENT               YCI 0x8D00


/* @constant GL_STENCIL_ATTACHMENT,int
Accepted by the <attachment> parameter of §glFramebufferTexture1D, §glFramebufferTexture2D, §glFramebufferTexture3D, §glFramebufferRenderbuffer, and §zglGetFramebufferAttachmentParameteri
*/
//#define GL_STENCIL_ATTACHMENT             YCI 0x8D20


/* @constant GL_FRAMEBUFFER_COMPLETE,int
Returned by §glCheckFramebufferStatus
 */
//#define GL_FRAMEBUFFER_COMPLETE           YCI 0x8CD5


/* @constant GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,int
Returned by §glCheckFramebufferStatus
 */
//#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT             YCI 0x8CD6


/* @constant GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,int
Returned by §glCheckFramebufferStatus
 */
//#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT     YCI 0x8CD7


/* @constant GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS,int
Returned by §glCheckFramebufferStatus
 */
//#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS             YCI 0x8CD9


/* @constant GL_FRAMEBUFFER_INCOMPLETE_FORMATS,int
Returned by §glCheckFramebufferStatus
 */
//#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS                YCI 0x8CDA


/* @constant GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,int
Returned by §glCheckFramebufferStatus
 */
//#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER            YCI 0x8CDB


/* @constant GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,int
Returned by §glCheckFramebufferStatus
 */
//#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER            YCI 0x8CDC


/* @constant GL_FRAMEBUFFER_UNSUPPORTED,int
Returned by §glCheckFramebufferStatus
 */
//#define GL_FRAMEBUFFER_UNSUPPORTED                       YCI 0x8CDD


/* @constant GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,int
Returned by §glCheckFramebufferStatus
 */
//#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE            YCI 0x8D56


/* @constant GL_FRAMEBUFFER_BINDING,int
Accepted by §zglGetInteger
*/
//#define GL_FRAMEBUFFER_BINDING            YCI 0x8CA6


/* @constant GL_RENDERBUFFER_BINDING,int
Accepted by §zglGetInteger
*/
//#define GL_RENDERBUFFER_BINDING           YCI 0x8CA7


/* @constant GL_MAX_COLOR_ATTACHMENTS,int
Accepted by §zglGetInteger
*/
//#define GL_MAX_COLOR_ATTACHMENTS          YCI 0x8CDF


/* @constant GL_MAX_RENDERBUFFER_SIZE,int
Accepted by §glGetIntegerv
*/
//#define GL_MAX_RENDERBUFFER_SIZE          YCI 0x84E8


/* @constant GL_MAX_SAMPLES,int
Accepted by §glGetIntegerv
*/
//#define GL_MAX_SAMPLES                    YCI 0x8D57


/* @constant GL_INVALID_FRAMEBUFFER_OPERATION,int
Returned by §glGetError
*/
//#define GL_INVALID_FRAMEBUFFER_OPERATION  YCI 0x0506


/* @constant GL_BUFFER_SIZE,int
 */
//#define GL_BUFFER_SIZE                    YCI 0x8764


//
//
// GL_ARB_texture_rectangle
//
//

/* @constant GL_TEXTURE_RECTANGLE,int
Accepted by the <cap> parameter of §glEnable, §glDisable and §glIsEnabled, by the <pname> parameter of §glGetBooleanv, §glGetIntegerv, §glGetFloatv and §glGetDoublev, and by the <target> parameter of §glBindTexture, §zglGetTexParameterf, §zglGetTexParameteri, §glTexParameterf, §glTexParameteri, §glTexParameterfv and §glTexParameteriv

Accepted by the <target> parameter of §glGetTexImage, §glGetTexLevelParameteriv, §glGetTexLevelParameterfv, §glTexImage2D, §glCopyTexImage2D, §glTexSubImage2D and §glCopyTexSubImage2D

*/
//#define GL_TEXTURE_RECTANGLE              YCI 0x84F5


/* @constant GL_TEXTURE_BINDING_RECTANGLE,int
Accepted by the <pname> parameter of §glGetBooleanv, §glGetIntegerv, §glGetFloatv and §glGetDoublev
*/
//#define GL_TEXTURE_BINDING_RECTANGLE      YCI 0x84F6


/* @constant GL_PROXY_TEXTURE_RECTANGLE,int
 Accepted by the <target> parameter of §glGetTexLevelParameteriv, §glGetTexLevelParameterfv, §glGetTexParameteriv and §glTexImage2D
Can be used to check if texture fits into memory.
*/
//#define GL_PROXY_TEXTURE_RECTANGLE        YCI 0x84F7


/* @constant GL_MAX_RECTANGLE_TEXTURE_SIZE,int
Accepted by the <pname> parameter of §glGetBooleanv, §glGetDoublev, §glGetIntegerv and §glGetFloatv
*/
//#define GL_MAX_RECTANGLE_TEXTURE_SIZE     YCI 0x84F8


/* @constant GL_SAMPLER_1D,int
*/
//#define GL_SAMPLER_1D                     YCI 0x8B5D


/* @constant GL_SAMPLER_2D,int
*/
//#define GL_SAMPLER_2D                     YCI 0x8B5E


/* @constant GL_SAMPLER_3D,int
*/
//#define GL_SAMPLER_3D                     YCI 0x8B5F


/* @constant GL_SAMPLER_CUBE,int
*/
//#define GL_SAMPLER_CUBE                   YCI 0x8B60


/* @constant GL_SAMPLER_1D_SHADOW,int
*/
//#define GL_SAMPLER_1D_SHADOW              YCI 0x8B61


/* @constant GL_SAMPLER_2D_SHADOW,int
*/
//#define GL_SAMPLER_2D_SHADOW              YCI 0x8B62


/* @constant GL_SAMPLER_2D_RECT,int
Returned by <type> parameter of §glGetActiveUniform when the location <index> for program object <program> is of type sampler2DRect
*/
//#define GL_SAMPLER_2D_RECT                YCI 0x8B63


/* @constant GL_SAMPLER_2D_RECT_SHADOW,int
Returned by <type> parameter of §glGetActiveUniform when the location <index> for program object <program> is of type sampler2DRectShadow
*/
//#define GL_SAMPLER_2D_RECT_SHADOW         YCI 0x8B64


// --- GL_ARB_texture_multisample ---
//#define GL_SAMPLE_POSITION                           YCI 0x8E50
//#define GL_SAMPLE_MASK                               YCI 0x8E51
//#define GL_SAMPLE_MASK_VALUE                         YCI 0x8E52
//#define GL_MAX_SAMPLE_MASK_WORDS                     YCI 0x8E59
//#define GL_TEXTURE_2D_MULTISAMPLE                    YCI 0x9100
//#define GL_PROXY_TEXTURE_2D_MULTISAMPLE              YCI 0x9101
//#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY              YCI 0x9102
//#define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY        YCI 0x9103
//#define GL_TEXTURE_BINDING_2D_MULTISAMPLE            YCI 0x9104
//#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY      YCI 0x9105
//#define GL_TEXTURE_SAMPLES                           YCI 0x9106
//#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS            YCI 0x9107
//#define GL_SAMPLER_2D_MULTISAMPLE                    YCI 0x9108
//#define GL_INT_SAMPLER_2D_MULTISAMPLE                YCI 0x9109
//#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE       YCI 0x910A
//#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY              YCI 0x910B
//#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY          YCI 0x910C
//#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY YCI 0x910D
//#define GL_MAX_COLOR_TEXTURE_SAMPLES                 YCI 0x910E
//#define GL_MAX_DEPTH_TEXTURE_SAMPLES                 YCI 0x910F
//#define GL_MAX_INTEGER_SAMPLES                       YCI 0x9110





// --- GLSL ---

/* @constant GL_PROGRAM_OBJECT,int
 */
//#define GL_PROGRAM_OBJECT                 YCI 0x8B40


/* @constant GL_SHADER_OBJECT,int
 */
//#define GL_SHADER_OBJECT                  YCI 0x8B48


/* @constant GL_OBJECT_TYPE,int
 */
//#define GL_OBJECT_TYPE                    YCI 0x8B4E


/* @constant GL_MAX_VERTEX_ATTRIBS,int
 */
//#define GL_MAX_VERTEX_ATTRIBS             YCI 0x8869


/* @constant GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,int
 */
//#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED YCI 0x886A


/* @constant GL_FRAGMENT_SHADER,int
 */
//#define GL_FRAGMENT_SHADER                YCI 0x8B30


/* @constant GL_VERTEX_SHADER,int
 */
//#define GL_VERTEX_SHADER                  YCI 0x8B31


/* @constant GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,int
 */
//#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS YCI 0x8B49


/* @constant GL_MAX_VERTEX_UNIFORM_COMPONENTS,int
 */
//#define GL_MAX_VERTEX_UNIFORM_COMPONENTS  YCI 0x8B4A


/* @constant GL_MAX_VARYING_FLOATS,int
 */
//#define GL_MAX_VARYING_FLOATS             YCI 0x8B4B


/* @constant GL_SHADER_TYPE,int
 */
//#define GL_SHADER_TYPE                    YCI 0x8B4F


/* @constant GL_DELETE_STATUS,int
 */
//#define GL_DELETE_STATUS                  YCI 0x8B80


/* @constant GL_LINK_STATUS,int
 */
//#define GL_LINK_STATUS                    YCI 0x8B82


/* @constant GL_VALIDATE_STATUS,int
 */
//#define GL_VALIDATE_STATUS                YCI 0x8B83


/* @constant GL_INFO_LOG_LENGTH,int
 */
//#define GL_INFO_LOG_LENGTH                YCI 0x8B84


/* @constant GL_ATTACHED_SHADERS,int
 */
//#define GL_ATTACHED_SHADERS               YCI 0x8B85


/* @constant GL_ACTIVE_UNIFORMS,int
 */
//#define GL_ACTIVE_UNIFORMS                YCI 0x8B86


/* @constant GL_ACTIVE_UNIFORM_MAX_LENGTH,int
 */
//#define GL_ACTIVE_UNIFORM_MAX_LENGTH      YCI 0x8B87


/* @constant GL_SHADER_SOURCE_LENGTH,int
 */
//#define GL_SHADER_SOURCE_LENGTH           YCI 0x8B88


/* @constant GL_ACTIVE_ATTRIBUTES,int
 */
//#define GL_ACTIVE_ATTRIBUTES              YCI 0x8B89


/* @constant GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,int
 */
//#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH    YCI 0x8B8A


/* @constant GL_FRAGMENT_SHADER_DERIVATIVE_HINT,int
 */
//#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT YCI 0x8B8B


/* @constant GL_SHADING_LANGUAGE_VERSION,int
 */
//#define GL_SHADING_LANGUAGE_VERSION       YCI 0x8B8C


/* @constant GL_CURRENT_PROGRAM,int
 */
//#define GL_CURRENT_PROGRAM                YCI 0x8B8D


/* @constant GL_MAJOR_VERSION,int
 Since GL30
 */
//#define GL_MAJOR_VERSION                  YCI 0x821B


/* @constant GL_MINOR_VERSION,int
 Since GL30
 */
//#define GL_MINOR_VERSION                  YCI 0x821C



	void YAC_VCALL yacOperator(sSI _op, YAC_Object *_o, YAC_Value *);

   /* @method alloc
   Generate new display list name id

   This is a wrapper for the glGenLists() native function.
   */
	YM void        _alloc   (void);


   /* @method free
   Free display list

   This is a wrapper for the glDeleteLists() native function.
   */
	YM void        _free    (void);


   /* @method begin
   Begin recording of display list

   This is a wrapper for the glNewList() native function (GL_COMPILE_AND_EXECUTE)
   */
	YM void        _begin   (void);


   /* @method end
   End recording of display list and execute it.

   This is a wrapper for the glEndList() native function.
   */
	YM void        _end     (void);


   /* @method draw
   Execute display list

   This is a wrapper for the glCallList() native function.
   */
	YM void        _draw    (void);


   /* @method setName,String name
   Set display list name

   @arg name String to copy to the name field
   */
	YM void        _setName (YAC_String *);


   /* @method getName:String
   Query the display list name

   @return Reference to name field
   */
	YM YAC_String *_getName (void);


   /* @method getId:int
   Query display list name id

   @return Display list name id or -1 if this object stores no valid display list.
   @see glCallList
   */
	YM sSI         _getId   (void);
};


extern void YAC_CALL _glLoadMatrixf_int(const sF32 *_mat);
extern void YAC_CALL _glMultMatrixf_int(const sF32 *_mat);
extern void YAC_CALL _unproject_int(sF32 _x, sF32 _y,
                                    sF32 *_nearx, sF32 *_neary, sF32 *_nearz,
                                    sF32 *_farx, sF32 *_fary, sF32 *_farz
                                    );
extern void YAC_CALL _zglStoreMatrixf_int(sF32 *_mat);
extern void YAC_CALL _glLightfv_int(GLenum _light, GLenum _pname, const GLfloat *_params);
extern void YAC_CALL _glMaterialfv_int(GLenum _face, GLenum _pname, const GLfloat *_params);
extern void YAC_CALL _project_int(sF32 _x, sF32 _y, sF32 _z,
                                  sF32 *_px, sF32 *_py, sF32 *_pz
                                  );

extern sSI YAC_CALL _glGetUniformLocation_int (sUI _program, const char *_name);
extern void YAC_CALL _glShaderSource_int (GLuint _shader, GLsizei _count, const char *const*_src, const GLint *_lengths);
extern void YAC_CALL _glGetShaderiv_int (GLuint _shader, GLenum _pname, GLint *_r);
extern void YAC_CALL _glGetShaderInfoLog_int (GLuint _shader, GLsizei _maxLength, GLsizei *_length, char *_infoLog);
extern void YAC_CALL _glGetProgramiv_int (GLuint _program, GLenum _pname, GLint *_params);
extern void YAC_CALL _glGetProgramInfoLog_int (GLuint _program, GLsizei _maxLength, GLsizei *_length, char *_infoLog);
extern void YAC_CALL _glVertexAttribPointer_int (GLuint _index, GLint _size, GLenum _type, GLboolean _normalized, GLsizei _stride, const GLvoid *_pointer);
extern void YAC_CALL _glUniformMatrix2fv_int (GLint _location, GLsizei _count, GLboolean _transpose, const GLfloat *_value);
extern void YAC_CALL _glUniformMatrix3fv_int (GLint _location, GLsizei _count, GLboolean _transpose, const GLfloat *_value);
extern void YAC_CALL _glUniformMatrix4fv_int (GLint _location, GLsizei _count, GLboolean _transpose, const GLfloat *_value);
extern void YAC_CALL _glBindAttribLocation_int (GLuint _program, GLuint _index, const char *_name);
extern void YAC_CALL _glUseProgram_int (GLuint _program);
extern GLuint YAC_CALL _glGenLists (GLsizei _range);
extern void YAC_CALL _glDeleteLists (GLuint _list, GLsizei _range);
extern void YAC_CALL _glNewList (GLuint _list, GLenum _mode);
extern void YAC_CALL _glEndList (void);
extern void YAC_CALL _glPixelStorei_int (GLenum _pname, GLint _param);
extern void YAC_CALL _glTexEnvfv_int (GLenum _target, GLenum _pname, const GLfloat *_params);
extern void YAC_CALL _glDrawPixels_int (GLsizei _width, GLsizei _height, GLenum _format, GLenum _type, const GLvoid *_pixels);
extern void YAC_CALL _glTexImage1D_int (GLenum _target, GLint _level, GLint _internalFormat, GLsizei _width, GLint _border, GLenum _format, GLenum _type, const GLvoid *_data);
extern void YAC_CALL _glTexImage2D_int (GLenum _target, GLint _level, GLint _internalFormat, GLsizei _width, GLsizei _height, GLint _border, GLenum _format, GLenum _type, const GLvoid *_data);
extern void YAC_CALL _glTexImage3D_int(GLenum _target, GLint _level, GLint _internalFormat, GLsizei _width, GLsizei _height, GLsizei _depth, GLint _border, GLenum _format, GLenum _type, const GLvoid *_data);
extern void YAC_CALL _glTexSubImage2D_int(GLenum _target, GLint _level, GLint _xoffset, GLint _yoffset, GLsizei _width, GLsizei _height, GLenum _format, GLenum _type, const GLvoid *_pixels);
extern GLenum YAC_CALL _glGetError_int (void);
extern GLint YAC_CALL _glGetAttribLocation_int (GLuint _program, const char *_name);
extern void YAC_CALL _glUniform2fv_int (GLint _location, GLsizei _count, const GLfloat *_values);
extern void YAC_CALL _glUniform3fv_int (GLint _location, GLsizei _count, const GLfloat *_values);
extern void YAC_CALL _glUniform4fv_int (GLint _location, GLsizei _count, const GLfloat *_values);
extern void YAC_CALL _glVertexAttrib2fv_int (GLuint _index, const GLfloat *_v);
extern void YAC_CALL _glVertexAttrib3fv_int (GLuint _index, const GLfloat *_v);
extern void YAC_CALL _glVertexAttrib4fv_int (GLuint _index, const GLfloat *_v);
extern void YAC_CALL _glDrawElements_int(GLenum _mode, GLsizei _count, GLenum _type, const GLvoid *_indices);
extern void YAC_CALL _glDrawArrays_priv(sSI _mode, sSI _first, sSI _count, sBool _bAllowImm);
extern void YAC_CALL _glDrawElements_priv(GLenum _mode, GLsizei _count, GLenum _type, const GLvoid *_indices, sBool _bAllowImm);
extern void YAC_CALL _glPointParameterfv_int(sSI _pname, const sF32 *_params);

#endif // __TKSOPENGL_H__
