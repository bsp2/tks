
#ifdef MINNIE_LIB
#ifndef YAC_MACOS
/* #define GL_GLEXT_PROTOTYPES defined */
/* #include "../tkopengl/GL_includes/glcorearb.h" */
#ifdef YAC_WIN32
#include <windows.h>
#endif // YAC_WIN32
#include <GL/gl.h>
#include "../tkopengl/GL_includes/glext.h" // need OpenGL 1.2+ for cube mapping and buffer objects
#include "native/gl_ext.h"
#else
#define GL3_PROTOTYPES defined
#include <OpenGL/gl3.h>
#endif // !YAC_MACOS
#define TKOPENGL_SKIP_GL_HEADERS defined
#include "../tkopengl/inc_opengl.h"
#include "native/zgl.h"
#else
#include "../tkopengl/inc_opengl.h"
#include "../tkopengl/tkopengl_shared.h"
#endif // MINNIE_LIB

#ifndef GL_BGRA8_EXT
#define GL_BGRA_EXT                       0x80E1
#endif

#ifndef GL_BGRA8_EXT
#define GL_BGRA8_EXT                      0x93A1
#endif
