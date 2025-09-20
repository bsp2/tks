/// tkopengl.h
///
/// (c) 2001-2025 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef TKOPENGL_H__
#define TKOPENGL_H__

#include <yac.h>

Dyac_std_exid_decl;


#define Demuprintf   if(0);else Dyac_host_printf
#define Demuprintfv  if(0);else Dyac_host_printf
#define Demuprintfvv if(0);else Dyac_host_printf


// used by DX_EMU_USE_VAO
#define TKOPENGL_MAX_GEN_VTX_ATTRIBS  ( 16u)

extern sUI tkopengl_builtin_vao_id;
extern sUI tkopengl_current_vao_id;

extern void tkopengl_vao_init (void);

extern void _glVertexAttribPointer_int_vao (GLuint _index, GLint _size, GLenum _type, GLboolean _normalized, GLsizei _stride, const GLvoid *_pointer);
extern void _glEnableVertexAttribArray_int_vao (sUI _index);
extern void _glDisableVertexAttribArray_int_vao (sUI _index);



extern sBool *tksdl_b_glcore;
extern void tkopengl_lazyinit_tksdl_b_glcore(void);


#endif // TKOPENGL_H__
