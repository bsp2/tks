/// glerror.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL)
///
///

#ifndef TKSOPENGL_ERROR_H__
#define TKSOPENGL_ERROR_H__

extern sBool tkopengl_b_glBegin;

extern void tkopengl_handle_gl_error(GLenum _err, const char *_fxnName);


#ifdef TRACE_GL_ERROR
// Note: glGetError() calls between glBegin/glEnd are NOT allowed according to GL specs
extern sBool *tksdl_b_glerror;
extern void tkopengl_lazyinit_tksdl_b_glerror(void);

extern GLenum last_gl_error;

#define Dtraceglerror(a) {                                              \
      if(tkopengl_b_glBegin)                                            \
      {                                                                 \
         yac_host->printf("[---] tkopengl: illegal call to \"%s\" in between glBegin()..glEnd() (at %s:%d).\n", \
                          a, __FILE__, __LINE__);                       \
      }                                                                 \
      else                                                              \
      {                                                                 \
         GLenum err  = _glGetError_int();                               \
         if(NULL == tksdl_b_glerror)                                    \
            tkopengl_lazyinit_tksdl_b_glerror();                        \
         if(tksdl_b_glerror && *tksdl_b_glerror)                        \
         {                                                              \
            if(GL_NO_ERROR != err)                                      \
            {                                                           \
               tkopengl_handle_gl_error(err, a);                        \
            }                                                           \
         }                                                              \
      }                                                                 \
   }

#define Dhandleglerror(e,a) tkopengl_handle_gl_error(e, a)

#ifdef TRACE_GL_DRAW_ERROR
#define Dtracegldrawerror(a) { if(!tkopengl_b_glBegin) { Dtraceglerror(a); } }
#endif // TRACE_GL_DRAW_ERROR
#else
#define Dtraceglerror(a) (void)0
#define Dtracegldrawerror(a) (void)0
#define Dhandleglerror(e,a) (void)0
#endif // TRACE_GL_ERROR


#endif // TKSOPENGL_ERROR_H__
