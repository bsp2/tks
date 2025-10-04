
#ifdef MINNIE_LIB
#define GL3_PROTOTYPES defined
#include <OpenGL/gl3.h>
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
