/// tksdisplaylist.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de> 
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <stdlib.h>
#include <math.h>

#define YAC_PRINTF defined
#include <yac.h>

extern YAC_Host *yac_host;

#include "inc_opengl.h"

#include "tks-list.h"

#ifdef DX_EMU_MATRIX
#include "matrix.h"
#endif

#include "bitmap.h"
#include "image.h"
#include "tkstexture.h"
#include "tksopengl.h"


_DisplayList::_DisplayList(void) {
	gl_dlist = -1;
} 

_DisplayList::~_DisplayList() {
	_free();
}

void _DisplayList::_setName(YAC_String *_n) {
   name.yacCopy((YAC_String*)_n);
}

YAC_String *_DisplayList::_getName(void) {
   return &name;
}

void _DisplayList::yacOperator(sSI _op, YAC_Object *_o, YAC_Value *_r) {
}

void _DisplayList::_alloc(void) {
	_DisplayList::_free();

	gl_dlist = _glGenLists(1);
}

void _DisplayList::_free(void) {
	if(-1 != gl_dlist)
	{
		_glDeleteLists(gl_dlist, 1);
		gl_dlist=-1;
	}
}

void _DisplayList::_begin(void) {
   if(-1 != gl_dlist)
   {
#ifdef DX_EMU_MATRIX
      tkopengl_matrix_update();
#endif
      _glNewList(gl_dlist, GL_COMPILE_AND_EXECUTE);
   }
}

void _DisplayList::_end(void) {
   if(-1 != gl_dlist)
   {
		_glEndList();
   }
}

void _DisplayList::_draw(void) {
   if(-1 != gl_dlist)
   {
#ifdef DX_EMU_MATRIX
      tkopengl_matrix_update();
#endif
		_glCallList(gl_dlist);
	}
}

sSI _DisplayList::_getId(void) {
	return gl_dlist;
}
