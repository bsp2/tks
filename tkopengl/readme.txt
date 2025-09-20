
compile time options:

DONT_DRAW - Skip glBegin() / glEnd() / glDrawArrays() / glDrawElements() / glDrawBuffer() / glDrawBuffers()

DX_GIF - Compile with libgif support (not tested in a while, may require some build fixes)
DX_TIFF - Compile with tiffio support  (not tested in a while, may require some build fixes)
DX_PNG - Compile with libpng support (default)
DX_JPEG - Compile with jpeglib support (not tested in a while, may require some build fixes)

TRACE_GL_ERROR - Call glError() after each GL function and report / print any errors to console
///   Note: A drawback to this automatic error reporting feature is that glGetError()
///         resets the current GL error code so scripts cannot read it.
//    Note: OGL specs that it is NOT allowed to call glGetError() in between glBegin()..glEnd()

TRACE_GL_DRAW_ERROR - Define to trace GL errors after calls to glVertex*, glTexCoord*, glNormal*, glMultiTexCoord* outside of glBegin/glEnd.
/// (can that ever happen??)
/// TRACE_GL_ERROR must be defined, too.


DX_CONSTRUCTOR_EXPRESSIONS - GNU-style construction of size/point/selection objects (used by Image class)

DX_GLES_NOEMU - Don't enable Desktop OpenGL API emulation in GLES builds

DX_EMU_MATRIX - Emulate matrix ops (identity/rotate/scale/translate/GLU/..)  [required for GLES2]

DX_EMU_IMM - Emulate immediate mode (glBegin/glEnd)  [required for GLES2]
              (note) does not emulate glEdgeFlag()

DX_EMU_ATTRIB - Use attribute stack to track state changes (required by DX_EMU_FIXFXN)
                 (note) this feature only works in combination with DX_EMU_FIXFXN

DX_EMU_ATTRIB_TEXTURE_ID_MAP - If not defined, assume that glGenTextures() assigns texture ids in ascending order from 0..n. If defined, assume that texture ids are "completely arbitrary" and a mapping must be used.

DX_EMU_PRIM - Emulate legacy primitive types (GL_POLYGON/GL_QUADS/GL_QUAD_STRIP)  [required for GLES2]
               (note) GL_LINE polygons are drawn via GL_LINES. As a consequence, backface culling does not work.

DX_EMU_PRIM_TRI_LINE - Emulate GL_LINE polygon mode for GL_TRIANGLES
                        (note) increases memory usage by up to several hundred KBytes (depending on TKOPENGL_IMM_ARRAY_SIZE)

DX_EMU_PRIM_QUAD_LINE - Emulate GL_LINE polygon mode for GL_QUADS
                         (note) increases memory usage by up to several hundred KBytes (depending on TKOPENGL_IMM_ARRAY_SIZE)

DX_EMU_PRIM_TRISTRIP_LINE - Emulate GL_LINE polygon mode for GL_TRIANGLE_STRIP
                             (note) increases memory usage by up to several hundred KBytes (depending on TKOPENGL_IMM_ARRAY_SIZE)

DX_EMU_PRIM_QUADSTRIP_LINE - Emulate GL_LINE polygon mode for GL_QUAD_STRIP
                              (note) increases memory usage by up to several hundred KBytes (depending on TKOPENGL_IMM_ARRAY_SIZE)
                              (note) drawArrays() 'first' arg can only be set in increments of two

DX_EMU_FIXFXN - Emulate fixed function pipeline using shaders  [required for GLES2]
                 (note) DX_EMU_MATRIX and DX_EMU_IMM must be defined as well
                 (note) emulates glVertexPointer/glNormalPointer/.. via glVertexAttribPointer
                 (note) does not emulate texture combiners, just basic GLES 1.x functionality
                 (note) does not emulate glRasterPos*() / glDrawPixels() / glCopyPixels()
                 (note) does not emulate glLogicOp()
                 (note) does not emulate glLineStipple()
                 (note) does not emulate glPolygonStipple()
                 (note) does not emulate glPolygonMode() GL_FRONT/GL_BACK (front and back use the same settings)
                 (note) does not emulate point sprite GL_POINT_FADE_THRESHOLD_SIZE (just parsed)
                 (note) does not emulate indexed color mode
                 (note) does not emulate desktop-GL builtin shader attributes/uniforms
		 (note) does not emulate glClipPlane() (todo? slow via fragshader!)
                 (note) does not emulate glTexEnv(.., GL_TEXTURE_LOD_BIAS, ..)
                 (note) the maximum number of array elements is restricted to TKOPENGL_IMM_ARRAY_SIZE (def=32768)

DX_TEXTURE_3D - Enable support for 3D textures (sampler3D / GL_TEXTURE_3D)

DX_EMU_TEXTURE_1D - Emulate 1D textures via 2D textures  [required for GLES2]

DX_EMU_TEXTURE_RECTANGLE - (todo) Emulate GL_TEXTURE_RECTANGLE / sampler2DRect

DX_EMU_FOG - Emulate GL_FOG  [required for GLES2]
              (note) requires DX_EMU_FIXFXN
              (note) adds non-standard zglFogAlphaMode() (GL_REPLACE (OpenGL) / GL_BLEND (ext) / GL_MULT (ext))

DX_EMU_LIGHT - Emulate GL_LIGHTn  [required for GLES2]
                (note) requires DX_EMU_FIXFXN
                (note) does not emulate two-sided lighting
                (note) does not emulate different front/back materials
                (note) color material only supports GL_AMBIENT_AND_DIFFUSE (same as OGLES 1.x)
                (note) color material tracking is not permanent (i.e. does not update material colors after rendering)

DX_EMU_LIGHT_SKIPDIVW - Skip perspective eyeVtx division when positional lighting or local viewer is enabled
                (note) shader speed hack, assumes that modelview matrices are non-projective

DX_EMU_MULTITEX - (stub) Emulate glMultiTex*, glClientActiveTexture(), ..

DX_EMU_DLIST - (stub) Emulate display lists  [required for GLES2]

DX_GLES - Compile with OpenGL ES 2.x+ support
           (note) auto-enables all of the DX_EMU_* options (unless DX_GLES_NOEMU is defined)
           (note) glTexImage2D() internal formats GL_RGB8, GL_RGBA8/GL_BGRA will silently be converted to GL_RGB resp. GL_RGBA
           (note) glRenderbufferStorage() internal formats GL_DEPTH_COMPONENT24/32 will silently be converted to *_COMPONENT16

DX_GLES_CUSTOM_CONFIG - If defined, do not auto-enable _all_ DX_EMU_* options (just the basic ones). See "inc_opengl.h"
