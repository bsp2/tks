#ifdef YAC_WIN32
#include <windows.h>
#endif

#ifdef YAC_MACOS
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
//#include <OpenGL/gl3.h>
#else
// Windows, Linux
#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glext.h>
#endif // YAC_MACOS
