/// attrib.h
///
/// (c) 2013-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef TKOPENGL_STATE_H__
#define TKOPENGL_STATE_H__

#define TKOPENGL_ATTRIB_STACK_SIZE  ( 32u)
#define TKOPENGL_MAX_TEXTURE_UNITS  (  8u)
#define TKOPENGL_MAX_TEXTURES       (256u)
#define TKOPENGL_MAX_LIGHTS         (  8u)


typedef struct {
   sBool  b_enable;
   GLenum mode;       // GL_LINEAR/EXP/EXP2
   GLenum alpha_mode; // GL_REPLACE/GL_BLEND/GL_MULT (non-standard)
   sF32   start;
   sF32   end;
   sF32   density;
   sF32   color[4];
} tkopengl_fog_t;

typedef struct {
   sBool  b_enable;
   GLenum func;
   sF32   ref;
} tkopengl_alphatest_t;

typedef struct {
   GLenum mode;         // (enable mode) GL_TEXTURE_1D/2D/3D/CUBE_MAP/RECTANGLE (set via glEnable/glDisable)
   GLenum target;       // last set target for this texture unit (GL_TEXTURE_1D/2D/..) (set via glTexImage*)
   sSI    bound_nr;     // -1=none, texture nr (_not_ necessarily id) otherwise (see glBindTexture())
   GLenum env_mode;     // GL_REPLACE/GL_MODULATE/GL_DECAL/GL_BLEND/GL_ADD
   sF32   env_color[4];
} tkopengl_textureunit_t;

typedef struct {
#ifdef DX_EMU_ATTRIB_TEXTURE_ID_MAP
   GLuint id;
#endif // DX_EMU_ATTRIB_TEXTURE_ID_MAP
   GLenum internal_format; // GL_ALPHA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA. 0=unused
   sUI    sx;
   sUI    sy;
   sUI    sz;
} tkopengl_texture_t;

typedef struct {
   sF32  ambient_color[4];
   sF32  diffuse_color[4];
   sF32  specular_color[4];
   sF32  position[4];
   sF32  position_transform[4]; // transformed by modelview when glLight* is called
   sF32  spot_direction[3];
   sF32  spot_exponent;
   sF32  spot_cutoff;   // 0..90, 180
   sF32  constant_attenuation;
   sF32  linear_attenuation;
   sF32  quadratic_attenuation;
} tkopengl_light_t;

typedef struct {
   sBool b_enable;
   GLenum type; // GL_UNSIGNED_BYTE, GL_FLOAT
   sUI    num_comp; // 1,2,3,4, ..
   sUI stride;
   union {
      GLubyte *pbyte;
      GLfloat *pfloat;
      void    *pany;
#ifdef YAC_64
      sU64     offset;
#else
      sUI      offset;
#endif // YAC_64
   } addr;
} tkopengl_userarray_t;

#define TKOPENGL_USERARRAY_VERTEX    (0)
#define TKOPENGL_USERARRAY_NORMAL    (1)
#define TKOPENGL_USERARRAY_COLOR     (2)
#define TKOPENGL_USERARRAY_TEXCOORD  (3)
#define NUM_TKOPENGL_USERARRAYS      (4)

#define Dactivetextureunit(a) tkopengl_attrib->texture.units[tkopengl_attrib->texture.active_unit].a
#define Dboundtexture(a) tkopengl_textures[Dactivetextureunit(bound_nr)].a

typedef struct {

#ifdef DX_EMU_FOG
   tkopengl_fog_t       fog;
#endif // DX_EMU_FOG

   tkopengl_alphatest_t alphatest;

   struct {
      sUI                    active_unit;
      tkopengl_textureunit_t units[TKOPENGL_MAX_TEXTURE_UNITS];
   } texture;

   GLenum               polygon_mode; // GL_POINT,GL_LINE,GL_FILL

   struct {
      sF32 size;
      sF32 min_size;
      sF32 max_size;
      sF32 fade_threshold_size; // not supported (requires multisampling)
      struct {
         sF32 constant;
         sF32 linear;
         sF32 quadratic;
      } attenuation;
      struct {
         sBool b_coord_replace; // GL_POINT_SPRITE
         sBool b_enable;
      } sprite;
   } point;

   struct {
      sSI x;
      sSI y;
      sSI w;
      sSI h;
   } viewport;

#ifdef DX_EMU_LIGHT
   struct {
      sBool  b_enable;
      sUI    enable_mask;
      sBool  b_color_material;  // 1=color tracks ambient_and_diffuse
      sF32   model_ambient[4];  // ambient color of the entire scene
      GLenum color_control;
      sBool  b_local_viewer;
      tkopengl_light_t lights[TKOPENGL_MAX_LIGHTS];
      struct {
         sF32 ambient_color[4];
         sF32 diffuse_color[4];
         sF32 specular_color[4];
         sF32 emission_color[4];
         sF32 specular_exponent; // shininess
      } material;
   } light;
#endif // DX_EMU_LIGHT

   // (todo) belongs to client attribs
   tkopengl_userarray_t userarray[NUM_TKOPENGL_USERARRAYS];

} tkopengl_attrib_t;

extern tkopengl_attrib_t *tkopengl_attrib;
extern tkopengl_texture_t tkopengl_textures[TKOPENGL_MAX_TEXTURES];

extern void tkopengl_attrib_init (void);
extern void tkopengl_attrib_push (void);
extern void tkopengl_attrib_pop  (void);


#endif // TKOPENGL_STATE_H__
