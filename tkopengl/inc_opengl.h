/// inc_opengl.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

/*
 * OpenGL include helper
 *
 */


#ifdef DX_GLES

#ifndef DX_GLES_NOEMU

#ifndef DX_EMU_ATTRIB
#define DX_EMU_ATTRIB defined
#endif

#ifndef DX_EMU_MATRIX
#define DX_EMU_MATRIX defined
#endif

#ifndef DX_EMU_IMM
#define DX_EMU_IMM defined
#endif

#ifndef DX_EMU_DLIST
#define DX_EMU_DLIST defined
#endif

#ifndef DX_EMU_TEXTURE_1D
#define DX_EMU_TEXTURE_1D defined
#endif

#ifndef DX_EMU_TEXTURE_RECTANGLE
#define DX_EMU_TEXTURE_RECTANGLE defined
#endif

#ifndef DX_GLES_CUSTOM_CONFIG

#ifndef DX_EMU_PRIM
#define DX_EMU_PRIM defined
#endif

#ifndef DX_EMU_PRIM_TRI_LINE
#define DX_EMU_PRIM_TRI_LINE defined
#endif

#ifndef DX_EMU_PRIM_QUAD_LINE
#define DX_EMU_PRIM_QUAD_LINE defined
#endif

#ifndef DX_EMU_PRIM_TRISTRIP_LINE
#define DX_EMU_PRIM_TRISTRIP_LINE defined
#endif

#ifndef DX_EMU_PRIM_QUADSTRIP_LINE
#define DX_EMU_PRIM_QUADSTRIP_LINE defined
#endif

#ifndef DX_EMU_FIXFXN
#define DX_EMU_FIXFXN defined
#endif

#ifndef DX_EMU_FOG
#define DX_EMU_FOG defined
#endif

#ifndef DX_EMU_LIGHT
#define DX_EMU_LIGHT defined
#endif

#ifndef DX_EMU_MULTITEX
#define DX_EMU_MULTITEX defined
#endif

#endif // DX_GLES_CUSTOM_CONFIG

#endif // !DX_GLES_NOEMU

#endif // DX_GLES


#ifdef YAC_WIN32
#include <windows.h>
#include <GL/gl.h>
#include "GL_includes/glext.h" // need OpenGL 1.2+ for cube mapping and buffer objects
#endif

#ifdef YAC_MACOS
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
//#include <OpenGL/gl3.h>
#endif

#ifdef DX_GLES
#define GL_GLEXT_PROTOTYPES defined
#include <EGL/egl.h>
#ifdef DX_GLES_3
#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif // DX_GLES_3
#else

#if defined(YAC_LINUX) && defined(HAVE_X11)
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glxext.h>
#endif // YAC_LINUX (X11)

#endif // DX_GLES

#ifndef APIENTRY
#define APIENTRY
#endif

#ifndef DX_EMU_MATRIX
#ifndef DX_GLES

// for gluLookAt:
#ifdef YAC_MACOS
#include <OpenGL/glu.h>
#else
// linux, windows
#include <GL/glu.h>
#endif // YAC_MACOS

#endif // DX_GLES
#endif // DX_EMU_MATRIX



#ifndef GL_BGRA
#define GL_BGRA               0x80E1
#endif

#ifndef GL_RGB8
#define GL_RGB8               0x8051
#endif

#ifndef GL_RGBA8
#define GL_RGBA8              0x8058
#endif

#ifndef GL_DEPTH_COMPONENT16
#define GL_DEPTH_COMPONENT16              0x81A5
#endif

#ifndef GL_DEPTH_COMPONENT24
#define GL_DEPTH_COMPONENT24              0x81A6
#endif

#ifndef GL_DEPTH_COMPONENT32
#define GL_DEPTH_COMPONENT32              0x81A7
#endif


#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP    0x8191
#endif // GL_GENERATE_MIPMAP

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE      0x812F
#endif // GL_CLAMP_TO_EDGE

#ifndef GL_TEXTURE_CUBE_MAP
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#endif // GL_TEXTURE_CUBE_MAP

#ifndef GL_PROGRAM_POINT_SIZE
#define GL_PROGRAM_POINT_SIZE             0x8642
#endif

////#ifdef DX_GLES

#ifndef GL_STACK_OVERFLOW
#define GL_STACK_OVERFLOW     0x0503
#endif

#ifndef GL_STACK_UNDERFLOW
#define GL_STACK_UNDERFLOW    0x0504
#endif

#ifndef GL_MODELVIEW
#define GL_MODELVIEW          0x1700
#endif

#ifndef GL_MODELVIEW_MATRIX
#define GL_MODELVIEW_MATRIX   0x0BA6
#endif

#ifndef GL_PROJECTION_MATRIX
#define GL_PROJECTION_MATRIX  0x0BA7
#endif

#ifndef GL_TEXTURE_MATRIX
#define GL_TEXTURE_MATRIX     0x0BA8
#endif

#ifndef GL_PROJECTION
#define GL_PROJECTION         0x1701
#endif

#ifndef GL_TEXTURE
#define GL_TEXTURE            0x1702
#endif

#ifndef GL_TEXTURE_GEN_Q
#define GL_TEXTURE_GEN_Q      0x0C63
#endif

#ifndef GL_TEXTURE_GEN_S
#define GL_TEXTURE_GEN_S      0x0C60
#endif

#ifndef GL_TEXTURE_GEN_T
#define GL_TEXTURE_GEN_T      0x0C61
#endif

#ifndef GL_TEXTURE_GEN_R
#define GL_TEXTURE_GEN_R      0x0C62
#endif

#ifndef GL_EYE_LINEAR
#define GL_EYE_LINEAR         0x2400
#endif

#ifndef GL_OBJECT_LINEAR
#define GL_OBJECT_LINEAR      0x2401
#endif

#ifndef GL_SPHERE_MAP
#define GL_SPHERE_MAP         0x2402
#endif

#ifndef GL_TEXTURE_GEN_MODE
#define GL_TEXTURE_GEN_MODE   0x2500
#endif

#ifndef GL_OBJECT_PLANE
#define GL_OBJECT_PLANE       0x2501
#endif

#ifndef GL_EYE_PLANE
#define GL_EYE_PLANE          0x2502
#endif

#ifndef GL_S
#define GL_S                  0x2000
#endif

#ifndef GL_T
#define GL_T                  0x2001
#endif

#ifndef GL_R
#define GL_R                  0x2002
#endif

#ifndef GL_Q
#define GL_Q                  0x2003
#endif

#ifndef GL_MULT
#define GL_MULT               0x0103
#endif

#ifndef GL_VERTEX_ARRAY
#define GL_VERTEX_ARRAY       0x8074
#endif

#ifndef GL_NORMAL_ARRAY
#define GL_NORMAL_ARRAY       0x8075
#endif

#ifndef GL_COLOR_ARRAY
#define GL_COLOR_ARRAY        0x8076
#endif

#ifndef GL_INDEX_ARRAY
#define GL_INDEX_ARRAY        0x8077
#endif

#ifndef GL_TEXTURE_COORD_ARRAY
#define GL_TEXTURE_COORD_ARRAY  0x8078
#endif

#ifndef GL_EDGE_FLAG_ARRAY
#define GL_EDGE_FLAG_ARRAY    0x8079
#endif

#ifndef GL_DONT_CARE
#define GL_DONT_CARE          0x1100
#endif

#ifndef GL_FASTEST
#define GL_FASTEST            0x1101
#endif

#ifndef GL_NICEST
#define GL_NICEST             0x1102
#endif

#ifndef GL_POLYGON
#define GL_POLYGON            0x0009
#endif

#ifndef GL_FRONT
#define GL_FRONT              0x0404
#endif

#ifndef GL_BACK
#define GL_BACK               0x0405
#endif

#ifndef GL_FRONT_AND_BACK
#define GL_FRONT_AND_BACK     0x0408
#endif

#ifndef GL_POINT
#define GL_POINT              0x1b00
#endif

#ifndef GL_LINE
#define GL_LINE               0x1b01
#endif

#ifndef GL_FILL
#define GL_FILL               0x1b02
#endif

#ifndef GL_QUADS
#define GL_QUADS              0x0007
#endif

#ifndef GL_QUAD_STRIP
#define GL_QUAD_STRIP         0x0008
#endif

#ifndef GL_TEXTURE_1D
#define GL_TEXTURE_1D         0x0DE0
#endif

#ifndef GL_TEXTURE_2D
#define GL_TEXTURE_2D         0x0DE1
#endif

#ifndef GL_TEXTURE_3D
#define GL_TEXTURE_3D         0x806F
#endif

#ifndef GL_TEXTURE_RECTANGLE
#define GL_TEXTURE_RECTANGLE  0x84F5
#endif

#ifndef GL_POINT_SMOOTH
#define GL_POINT_SMOOTH       0x0B10
#endif

#ifndef GL_LINE_SMOOTH
#define GL_LINE_SMOOTH        0x0B20
#endif

#ifndef GL_LINE_STIPPLE
#define GL_LINE_STIPPLE       0x0B24
#endif

#ifndef GL_POLYGON_SMOOTH
#define GL_POLYGON_SMOOTH     0x0B41
#endif

#ifndef GL_POLYGON_STIPPLE
#define GL_POLYGON_STIPPLE    0x0B42
#endif

#ifndef GL_TEXTURE_ENV
#define GL_TEXTURE_ENV        0x2300
#endif

#ifndef GL_TEXTURE_ENV_MODE
#define GL_TEXTURE_ENV_MODE   0x2200
#endif

#ifndef GL_TEXTURE_ENV_COLOR
#define GL_TEXTURE_ENV_COLOR  0x2201
#endif

#ifndef GL_REPLACE
#define GL_REPLACE            0x1E01
#endif

#ifndef GL_MODULATE
#define GL_MODULATE           0x2100
#endif

#ifndef GL_DECAL
#define GL_DECAL              0x2101
#endif

#ifndef GL_BLEND
#define GL_BLEND              0x0BE2
#endif

#ifndef GL_ADD
#define GL_ADD                0x0104
#endif

#ifndef GL_ALPHA_TEST
#define GL_ALPHA_TEST         0x0BC0
#endif

#ifndef GL_NEVER
#define GL_NEVER              0x0200
#endif

#ifndef GL_LESS
#define GL_LESS               0x0201
#endif

#ifndef GL_EQUAL
#define GL_EQUAL              0x0202
#endif

#ifndef GL_LEQUAL
#define GL_LEQUAL             0x0203
#endif

#ifndef GL_GREATER
#define GL_GREATER            0x0204
#endif

#ifndef GL_NOTEQUAL
#define GL_NOTEQUAL           0x0205
#endif

#ifndef GL_GEQUAL
#define GL_GEQUAL             0x0206
#endif

#ifndef GL_ALWAYS
#define GL_ALWAYS             0x0207
#endif

#ifndef GL_FOG
#define GL_FOG                0x0B60
#endif

#ifndef GL_FOG_MODE
#define GL_FOG_MODE           0x0B65
#endif

#ifndef GL_FOG_COLOR
#define GL_FOG_COLOR          0x0B66
#endif

#ifndef GL_LINEAR
#define GL_LINEAR             0x2601
#endif

#ifndef GL_EXP
#define GL_EXP                0x0800
#endif

#ifndef GL_EXP2
#define GL_EXP2               0x0801
#endif

#ifndef GL_FLAT
#define GL_FLAT               0x1D00
#endif

#ifndef GL_SMOOTH
#define GL_SMOOTH             0x1D01
#endif

#ifndef GL_MAX_LIGHTS
#define GL_MAX_LIGHTS                0x0D31
#endif

#ifndef GL_LIGHTING
#define GL_LIGHTING                  0x0B50
#endif

#ifndef GL_LIGHT0
#define GL_LIGHT0                    0x4000
#endif

#ifndef GL_LIGHT1
#define GL_LIGHT1                    0x4001
#endif

#ifndef GL_LIGHT2
#define GL_LIGHT2                    0x4002
#endif

#ifndef GL_LIGHT3
#define GL_LIGHT3                    0x4003
#endif

#ifndef GL_LIGHT4
#define GL_LIGHT4                    0x4004
#endif

#ifndef GL_LIGHT5
#define GL_LIGHT5                    0x4005
#endif

#ifndef GL_LIGHT6
#define GL_LIGHT6                    0x4006
#endif

#ifndef GL_LIGHT7
#define GL_LIGHT7                    0x4007
#endif

#ifndef GL_COLOR_MATERIAL
#define GL_COLOR_MATERIAL            0x0B57
#endif

#ifndef GL_LIGHT_MODEL_LOCAL_VIEWER
#define GL_LIGHT_MODEL_LOCAL_VIEWER  0x0B51
#endif

#ifndef GL_LIGHT_MODEL_TWO_SIDE
#define GL_LIGHT_MODEL_TWO_SIDE      0x0B52
#endif

#ifndef GL_LIGHT_MODEL_COLOR_CONTROL
#define GL_LIGHT_MODEL_COLOR_CONTROL 0x81F8
#endif

#ifndef GL_SINGLE_COLOR
#define GL_SINGLE_COLOR              0x81F9
#endif

#ifndef GL_SEPARATE_SPECULAR_COLOR
#define GL_SEPARATE_SPECULAR_COLOR   0x81FA
#endif

#ifndef GL_LIGHT_MODEL_AMBIENT
#define GL_LIGHT_MODEL_AMBIENT       0x0B53
#endif

#ifndef GL_LIGHT0
#define GL_LIGHT0                    0x4000
#endif

#ifndef GL_LIGHT1
#define GL_LIGHT1                    0x4001
#endif

#ifndef GL_LIGHT2
#define GL_LIGHT2                    0x4002
#endif

#ifndef GL_LIGHT3
#define GL_LIGHT3                    0x4003
#endif

#ifndef GL_LIGHT4
#define GL_LIGHT4                    0x4004
#endif

#ifndef GL_LIGHT5
#define GL_LIGHT5                    0x4005
#endif

#ifndef GL_LIGHT6
#define GL_LIGHT6                    0x4006
#endif

#ifndef GL_LIGHT7
#define GL_LIGHT7                    0x4007
#endif

#ifndef GL_AMBIENT
#define GL_AMBIENT                   0x1200
#endif

#ifndef GL_DIFFUSE
#define GL_DIFFUSE                   0x1201
#endif

#ifndef GL_SPECULAR
#define GL_SPECULAR                  0x1202
#endif

#ifndef GL_POSITION
#define GL_POSITION                  0x1203
#endif

#ifndef GL_SPOT_DIRECTION
#define GL_SPOT_DIRECTION            0x1204
#endif

#ifndef GL_SPOT_EXPONENT
#define GL_SPOT_EXPONENT             0x1205
#endif

#ifndef GL_SPOT_CUTOFF
#define GL_SPOT_CUTOFF               0x1206
#endif

#ifndef GL_CONSTANT_ATTENUATION
#define GL_CONSTANT_ATTENUATION      0x1207
#endif

#ifndef GL_LINEAR_ATTENUATION
#define GL_LINEAR_ATTENUATION        0x1208
#endif

#ifndef GL_QUADRATIC_ATTENUATION
#define GL_QUADRATIC_ATTENUATION     0x1209
#endif

#ifndef GL_EMISSION
#define GL_EMISSION                  0x1600
#endif

#ifndef GL_SHININESS
#define GL_SHININESS                 0x1601
#endif

#ifndef GL_AMBIENT_AND_DIFFUSE
#define GL_AMBIENT_AND_DIFFUSE       0x1602
#endif

#ifndef GL_POINT_SPRITE
#define GL_POINT_SPRITE              0x8861
#endif

#ifndef GL_COORD_REPLACE
#define GL_COORD_REPLACE             0x8862
#endif

#ifndef GL_POINT_SIZE_MIN
#define GL_POINT_SIZE_MIN            0x8126
#endif

#ifndef GL_POINT_SIZE_MAX
#define GL_POINT_SIZE_MAX            0x8127
#endif

#ifndef GL_POINT_FADE_THRESHOLD_SIZE
#define GL_POINT_FADE_THRESHOLD_SIZE 0x8128
#endif

#ifndef GL_POINT_DISTANCE_ATTENUATION
#define GL_POINT_DISTANCE_ATTENUATION 0x8129
#endif

#ifndef GL_POINT_SPRITE_COORD_ORIGIN
#define GL_POINT_SPRITE_COORD_ORIGIN 0x8CA0
#endif

#ifndef GL_LOWER_LEFT
#define GL_LOWER_LEFT                0x8CA1
#endif

#ifndef GL_UPPER_LEFT
#define GL_UPPER_LEFT                0x8CA2
#endif

#ifndef GL_COMPILE
#define GL_COMPILE                   0x1300
#endif

#ifndef GL_COMPILE_AND_EXECUTE
#define GL_COMPILE_AND_EXECUTE       0x1301
#endif

#ifndef GL_PERSPECTIVE_CORRECTION_HINT
#define GL_PERSPECTIVE_CORRECTION_HINT    0x0C50
#endif

#ifndef GL_POINT_SMOOTH_HINT
#define GL_POINT_SMOOTH_HINT              0x0C51
#endif

#ifndef GL_LINE_SMOOTH_HINT
#define GL_LINE_SMOOTH_HINT               0x0C52
#endif

#ifndef GL_POLYGON_SMOOTH_HINT
#define GL_POLYGON_SMOOTH_HINT            0x0C53
#endif

#ifndef GL_FOG_HINT
#define GL_FOG_HINT                       0x0C54
#endif

#ifndef GL_PROGRAM_BINARY_LENGTH
#define GL_PROGRAM_BINARY_LENGTH          0x8741
#endif

#ifndef GL_TEXTURE_2D_ARRAY
#define GL_TEXTURE_2D_ARRAY               0x8C1A
#endif

#ifndef GL_MAJOR_VERSION
#define GL_MAJOR_VERSION                  0x821B
#endif

#ifndef GL_MINOR_VERSION
#define GL_MINOR_VERSION                  0x821C
#endif


#ifndef GL_MAP_READ_BIT
#define GL_MAP_READ_BIT                   0x0001
#endif

#ifndef GL_MAP_WRITE_BIT
#define GL_MAP_WRITE_BIT                  0x0002
#endif

#ifndef GL_MAP_INVALIDATE_RANGE_BIT
#define GL_MAP_INVALIDATE_RANGE_BIT       0x0004
#endif

#ifndef GL_MAP_INVALIDATE_BUFFER_BIT
#define GL_MAP_INVALIDATE_BUFFER_BIT      0x0008
#endif

#ifndef GL_MAP_FLUSH_EXPLICIT_BIT
#define GL_MAP_FLUSH_EXPLICIT_BIT         0x0010
#endif

#ifndef GL_MAP_UNSYNCHRONIZED_BIT
#define GL_MAP_UNSYNCHRONIZED_BIT         0x0020
#endif

#ifndef GL_NUM_EXTENSIONS
#define GL_NUM_EXTENSIONS                 0x821d
#endif

#ifndef GL_TEXTURE_SWIZZLE_R
#define GL_TEXTURE_SWIZZLE_R              0x8E42
#define GL_TEXTURE_SWIZZLE_G              0x8E43
#define GL_TEXTURE_SWIZZLE_B              0x8E44
#define GL_TEXTURE_SWIZZLE_A              0x8E45
#define GL_TEXTURE_SWIZZLE_RGBA           0x8E46
#endif
