/// shader.h
///
/// (c) 2013-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef TKOPENGL_SHADER_H__
#define TKOPENGL_SHADER_H__

YG("Shader");


#if (defined(DX_GLES) && !defined(DX_GLES_3)) || defined(DX_EMU_GLSL_1)
#define GLSL_ATTRIB_IN   "attribute"
#define GLSL_VARYING_IN  "varying"
#define GLSL_VARYING_OUT "varying"
#else
#define GLSL_ATTRIB_IN   "in"
#define GLSL_VARYING_IN  "in"
#define GLSL_VARYING_OUT "out"
#endif


#ifdef DX_EMU_IMM

#define TKOPENGL_MAX_SHADERS      (128u)

#define DX_AMD_ATTRIB_WORKAROUND  defined


// ---------------------------------------------------------------------------- attribute enumeration
#define ATTR_IDX_VERTEX   (0u)
#define ATTR_IDX_NORMAL   (1u)
#define ATTR_IDX_COLOR    (2u)
#define ATTR_IDX_TEXCOORD (3u)

#define NUM_ATTRIBS       (4u)


// ---------------------------------------------------------------------------- uniform enumeration
#define UNI_IDX_TRANSFORM       ( 0u)
#define UNI_IDX_SAMPLER         ( 1u)
#define UNI_IDX_OBTEXSIZE       ( 2u)  // (1/tex_width; 1/tex_height) for GL_TEXTURE_RECTANGLE emulation
#define UNI_IDX_ENVCOLOR        ( 3u)
#define UNI_IDX_ALPHAREF        ( 4u)
#define UNI_IDX_POINT_SIZE      ( 5u)
#define UNI_IDX_POINT_MINMAX    ( 6u)
#define UNI_IDX_POINT_ATT       ( 7u)
#define UNI_IDX_FOGOBDELTA      ( 8u)  // (1.0f / (end - start))
#define UNI_IDX_FOGEND          ( 9u)
#define UNI_IDX_FOGDENSITY      (10u)
#define UNI_IDX_FOGCOLOR        (11u)
#define UNI_IDX_NORMALMATRIX    (12u)  // for lighting
#define UNI_IDX_MODELVIEW       (13u)  // for spot lights
#define UNI_IDX_LIGHTACS        (14u)  // light_model_ambient (acs)
#define UNI_IDX_LIGHTECM        (15u)  // light material emission (ecm)
#define UNI_IDX_LIGHTACM        (16u)  // light material ambient (acm)
#define UNI_IDX_LIGHTDCM        (17u)  // light material diffuse (dcm)
#define UNI_IDX_LIGHTSCM        (18u)  // light material specular (scm)
#define UNI_IDX_LIGHTSRM        (19u)  // light material specular exponent ('shininess') (srm)

#define UNI_IDX_LIGHT0POS       (20u)  // eye-space transformed light position
#define UNI_IDX_LIGHT0ACL       (21u)  // light0 ambient intensity (acli)
#define UNI_IDX_LIGHT0DCL       (22u)  // light0 diffuse intensity (dcli)
#define UNI_IDX_LIGHT0SCL       (23u)  // light0 specular intensity (scli)
#define UNI_IDX_LIGHT0ATTL      (24u)  // light0 constant/linear/quadratic attenuation factors (k0i, k1i, k2i)
#define UNI_IDX_LIGHT0SDL       (25u)  // light0 spot direction (sdli)
#define UNI_IDX_LIGHT0SRL       (26u)  // light0 spot exponent (srli)
#define UNI_IDX_LIGHT0CRLCOS    (27u)  // light0 cos(spot cutoff angle) (cos(crli))
#define NUM_UNIFORMS_PER_LIGHT (8u)

#define UNI_IDX_TEXTUREMATRIX   (28u)

#define NUM_UNIFORMS            (29u)


// (note) if a custom shader was set using glProgram(), it is (currently) assumed that
//         it is GLES compliant (e.g. no builtin desktop-GL specific vars used)
extern sBool b_tkopengl_custom_shader;


/* vertex program features */
#define TKOPENGL_VP_FEATURE_POINT_SIZE     (1 << 0)  // when drawing GL_POINTS
#define TKOPENGL_VP_FEATURE_POINT_SPRITE   (1 << 1)  // if GL_POINT_SPRITE is enable (depth attenuation)
#define TKOPENGL_VP_FEATURE_POINT_TEXCOORD (1 << 2)  // texture lookup based on gl_PointCoord (GL_COORD_REPLACE mode)

#define TKOPENGL_VP_FEATURE_FOG_MASK     (3 << 3)
#define TKOPENGL_VP_FEATURE_FOG_LINEAR   (1 << 3)
#define TKOPENGL_VP_FEATURE_FOG_EXP      (2 << 3)
#define TKOPENGL_VP_FEATURE_FOG_EXP2     (3 << 3)

#define TKOPENGL_VP_FEATURE_LIGHT0_SHIFT (5)
#define TKOPENGL_VP_FEATURE_LIGHT0       (1 <<  5)
#define TKOPENGL_VP_FEATURE_LIGHT1       (1 <<  6)
#define TKOPENGL_VP_FEATURE_LIGHT2       (1 <<  7)
#define TKOPENGL_VP_FEATURE_LIGHT3       (1 <<  8)
#define TKOPENGL_VP_FEATURE_LIGHT4       (1 <<  9)
#define TKOPENGL_VP_FEATURE_LIGHT5       (1 << 10)
#define TKOPENGL_VP_FEATURE_LIGHT6       (1 << 11)
#define TKOPENGL_VP_FEATURE_LIGHT7       (1 << 12)
#define TKOPENGL_VP_FEATURE_LIGHT_MASK   (      \
   TKOPENGL_VP_FEATURE_LIGHT0 |                 \
   TKOPENGL_VP_FEATURE_LIGHT1 |                 \
   TKOPENGL_VP_FEATURE_LIGHT2 |                 \
   TKOPENGL_VP_FEATURE_LIGHT3 |                 \
   TKOPENGL_VP_FEATURE_LIGHT4 |                 \
   TKOPENGL_VP_FEATURE_LIGHT5 |                 \
   TKOPENGL_VP_FEATURE_LIGHT6 |                 \
   TKOPENGL_VP_FEATURE_LIGHT7 )
#define TKOPENGL_VP_FEATURE_LIGHTCOLORMATERIAL  (1 << 13)
#define TKOPENGL_VP_FEATURE_LIGHTPOSITIONAL     (1 << 14)  // any lightpos.w != 0  (all 0 = directional lights only)
#define TKOPENGL_VP_FEATURE_LIGHTSPOT           (1 << 15)  // any spot cutoff angle != 180
#define TKOPENGL_VP_FEATURE_LIGHTSPECULAR       (1 << 16)  // matspec.rgb!=0 and any light spec.rbg != 0
#define TKOPENGL_VP_FEATURE_LIGHTSPECULARSEP    (1 << 17)  // output separate specular color and add to fragcol
#define TKOPENGL_VP_FEATURE_LIGHTLOCALVIEWER    (1 << 18)

#define TKOPENGL_VP_FEATURE_COLOR        (1 << 19)      // pass attr_color/var_color to fp
#define TKOPENGL_VP_FEATURE_COLORINIT    (1 << 20)      // init v_color w/ (1,1,1,1) (req. for e.g. fog) **REMOVE**

#define TKOPENGL_VP_FEATURE_TEXGEN_MASK            (7 << 21)
#define TKOPENGL_VP_FEATURE_TEXGEN_NONE            (0 << 21)  // no texture coordinates
#define TKOPENGL_VP_FEATURE_TEXGEN_COORD           (1 << 21)  // pass attr_texcoord/var_texcoord to fp
#define TKOPENGL_VP_FEATURE_TEXGEN_OBJECT_LINEAR   (2 << 21)
#define TKOPENGL_VP_FEATURE_TEXGEN_EYE_LINEAR      (3 << 21)
#define TKOPENGL_VP_FEATURE_TEXGEN_SPHERE_MAP      (4 << 21)
#define TKOPENGL_VP_FEATURE_TEXGEN_NORMAL_MAP      (5 << 21)
#define TKOPENGL_VP_FEATURE_TEXGEN_REFLECTION_MAP  (6 << 21)

#define TKOPENGL_VP_FEATURE_TEXTUREMATRIX  (1 << 24)


/* fragment program features */
#define TKOPENGL_FP_FEATURE_TEX_MASK          (7 << 0)
#define TKOPENGL_FP_FEATURE_TEX_NONE          (0 << 0)  // no texture mapping
#define TKOPENGL_FP_FEATURE_TEX_1D            (1 << 0)
#define TKOPENGL_FP_FEATURE_TEX_2D            (2 << 0)
#define TKOPENGL_FP_FEATURE_TEX_3D            (3 << 0)
#define TKOPENGL_FP_FEATURE_TEX_CUBE          (4 << 0)
#define TKOPENGL_FP_FEATURE_TEX_RECT          (5 << 0)

#define TKOPENGL_FP_FEATURE_TEXENV_MASK       (7 << 3)
#define TKOPENGL_FP_FEATURE_TEXENV_REPLACE    (0 << 3)
#define TKOPENGL_FP_FEATURE_TEXENV_MODULATE   (1 << 3)
#define TKOPENGL_FP_FEATURE_TEXENV_DECAL      (2 << 3)
#define TKOPENGL_FP_FEATURE_TEXENV_BLEND      (3 << 3)
#define TKOPENGL_FP_FEATURE_TEXENV_ADD        (4 << 3)

#define TKOPENGL_FP_FEATURE_ALPHATEST_MASK     (7 << 6)
#define TKOPENGL_FP_FEATURE_ALPHATEST_ALWAYS   (0 << 6)  // default / no-op
#define TKOPENGL_FP_FEATURE_ALPHATEST_LESS     (1 << 6)
#define TKOPENGL_FP_FEATURE_ALPHATEST_EQUAL    (2 << 6)
#define TKOPENGL_FP_FEATURE_ALPHATEST_LEQUAL   (3 << 6)
#define TKOPENGL_FP_FEATURE_ALPHATEST_GREATER  (4 << 6)
#define TKOPENGL_FP_FEATURE_ALPHATEST_NOTEQUAL (5 << 6)
#define TKOPENGL_FP_FEATURE_ALPHATEST_GEQUAL   (6 << 6)
#define TKOPENGL_FP_FEATURE_ALPHATEST_NEVER    (7 << 6)  // purpose?

#define TKOPENGL_FP_FEATURE_COLOR             (1 <<  9)      // var_color
#define TKOPENGL_FP_FEATURE_TEXCOORD          (1 << 10)      // var_texcoord

#define TKOPENGL_FP_FEATURE_FOG_ALPHA_MASK   (3 << 11)
#define TKOPENGL_FP_FEATURE_FOG_ALPHA_REPL   (0 << 11)
#define TKOPENGL_FP_FEATURE_FOG_ALPHA_BLEND  (1 << 11)
#define TKOPENGL_FP_FEATURE_FOG_ALPHA_MULT   (2 << 11)

#define TKOPENGL_FP_FEATURE_TEX_ALPHA        (1 << 13)   // read alpha as (1,1,1,a) instead of (0,0,0,a)


extern void tkopengl_shader_init (void);

extern void tkopengl_shader_exit (void);

extern void tkopengl_shader_draw_begin (GLenum _drawMode,
                                        sBool  _bAllowImm,
                                        sUI    _copyNumIndices,
                                        sUI    _copyFirstVertex,
                                        sUI    _copyNumVertices
                                        );

extern void tkopengl_shader_draw_end (void);


#endif // DX_EMU_IMM


// ---------------------------------------------------------------------------- gles_shader_t
typedef struct {
#ifdef DX_EMU_IMM
   sUI vp_features; // see TKOPENGL_VP_FEATURE_xxx
   sUI fp_features; // see TKOPENGL_FP_FEATURE_xxx
#endif // DX_EMU_IMM

   GLuint vp_id;
   GLuint fp_id;
   GLuint prg_id;

#ifdef DX_EMU_IMM
   GLint uni_map[NUM_UNIFORMS];
   GLint attr_map[NUM_ATTRIBS];
#endif // DX_EMU_IMM

} gles_shader_t;


// ---------------------------------------------------------------------------- ZGLShader
/* @class ZGLShader
 */
YC class _ZGLShader : public YAC_Object {
   // (note) OpenGL ES predefines "GL_ES"
  public:
   YAC_String vert_src;
   YAC_String frag_src;

  public:
   static YAC_String def_version_override;

  public:
	_ZGLShader(void);
	~_ZGLShader();

	YAC(_ZGLShader);

  public:
/* #ifdef DX_EMU_IMM */
   gles_shader_t gles_shader;
/* #endif // DX_EMU_IMM */

  protected:
   void lazyAddVersionHeader (sBool _bFrag, YAC_String *_src, YAC_String *_origSrc);

  public:
   // set to "120 \n" on macOS (anything other won't work)
   static void SetDefaultPlatformVersion(void);


  public:
   /* @method SetDefaultVersion,String ver
      Set/Override default #version header (optional)

      (note) applies to all ZGLShader objects
      (note) e.g. ZGLShader.SetDefaultVersion("140\n");
      (note) must end with newline (and may contain additional, non-empty header lines)

      @arg ver Version string (e.g. "140\n")
   */
   YM void SetDefaultVersion (YAC_String *_ver);

   /* @method setVertexSource,String src
    Set GLSL vertex shader source
    */
   YM void setVertexSource (YAC_String *_src);

   /* @method getVertexSource:String
    Get GLSL vertex shader source
    */
   YM void getVertexSource (YAC_Value *_r);

   /* @method setFragmentSource,String src
    Set GLSL fragment shader source
    */
   YM void setFragmentSource (YAC_String *_src);

   /* @method getFragmentSource:String
    Get GLSL fragment shader source
    */
   YM void getFragmentSource (YAC_Value *_r);

   /* @method create:boolean
    Create program, compile vertex+fragment shader, and link program.
   */
	YM sBool create(void);

   /* @method bind
      Bind shader program
    */
   YM void bind(void);

   /* @method unbind
      Unbind shader program
    */
   YM void unbind(void);

   /* @method destroy
      Destroy shader program and attached shaders
    */
   YM void destroy(void);

   /* @method getUniformLocation,String name:int
      Query uniform location
    */
   YM sSI getUniformLocation(YAC_String *_name);

   /* @method getAttribLocation,String name:int
      Query attribute location
    */
   YM sSI getAttribLocation(YAC_String *_name);

   /* @method getProgramId:int
      Get GL program id
    */
   YM sUI getProgramId (void);

   /* @method getFragmentShaderId:int
      Get GL fragment shader id
    */
   YM sUI getFragmentShaderId (void);

   /* @method getVertexShaderId:int
      Get GL vertex shader id
    */
   YM sUI getVertexShaderId (void);

   /* @method getNumActiveAttributes:int
      Query number of active attributes
    */
   YM sUI getNumActiveAttributes (void);

   /* @method getNumActiveUniforms:int
      Query number of active uniforms
    */
   YM sUI getNumActiveUniforms (void);

   /* @method getProgramBinaryLength:int
      Query GL program binary size
    */
   YM sUI getProgramBinaryLength (void);
};


#endif // TKOPENGL_SHADER_H__
