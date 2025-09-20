/// Matrix.cpp
///
/// (c) 2008-2024 by Bastian Spiegel <bs@tkscript.de>
///
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///


#include <math.h>

#include "tkmath_yac.h"
#include "tkmath.h"
#include "matrix.h"


_Matrix::_Matrix(void) {
   b_column_major = YAC_FALSE; // Default = Row Major (C-Array)    ([03Jun2024] changed from Column Major (OpenGL))
}

_Matrix::~_Matrix() {
}

void _Matrix::_setColumnMajor(sSI _bEnabled) {
#ifdef USE_DYNAMIC_ORDER
   b_column_major = _bEnabled & 1;
#else
   // Always row major
#endif // USE_DYNAMIC_ORDER
}

sSI _Matrix::_getColumnMajor(void) const {
#ifdef USE_DYNAMIC_ORDER
   return b_column_major;
#else
   // Always row major
   return YAC_FALSE;
#endif // USE_DYNAMIC_ORDER

}
