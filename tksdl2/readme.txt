
*** this version of tksdl uses libSDL2, see https://github.com/libsdl-org ***

- set env.var TKSDL_FULLFRAME to force FPS precision to 1.0
   (workaround for buggy gettimeofday(), should usually not be needed)
 
- set env.var TKSDL_GLERROR=1 to enable GL error trace (call glError() after each GL call)

- set env.var TKSDL_CORE=1 to force OpenGL 3.2+ core profile

- set env.var TKSDL_VIEWSIZE="640;480" to override initial window size

- set env.var TKSDL_VIEWPOS="30;20" to override initial window position
   (note) on macOS, negative x positions may be required to move window to other display

compile time options:

DX_GLES - Compile with OpenGL ES support

DX_GLES_GLEMU - Compile with Desktop OpenGL emulation support (must also be enabled in tkopengl)
