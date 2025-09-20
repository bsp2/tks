/// tkmath.h
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>,
///                  Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef TKMATH_H_
#define TKMATH_H_

/* @beginmodule tkmath

TkMath - Basic mathematical objects and methods

 */

#ifdef USE_DYNAMIC_ORDER
// Element order is configurable per Object
#define M4(a, row, col)    matrix4_element_order_table  [(a)->b_column_major][row][col]
#define M3(a, row, col)    matrix3_element_order_table  [(a)->b_column_major][row][col]
#define M2x3(a, row, col)  matrix2x3_element_order_table[(a)->b_column_major][row][col]
#define M2(a, row, col)    matrix2_element_order_table  [(a)->b_column_major][row][col]

#if 0
#define M4(a, row, col)   ((a)->b_column_major ? (((col)<<2)+(row)) : (((row)<<2)+(col)))
#define M3(a, row, col)   ((a)->b_column_major ? (((col)<<1)+(col)+(row)) : (((row)<<1)+(row)+(col)))
#define M2(a, row, col)   ((a)->b_column_major ? (((col)<<1)+(row)) : (((row)<<1)+(col)))
#endif // 0

extern const sU8 matrix4_element_order_table  [2][4][4];
extern const sU8 matrix3_element_order_table  [2][3][3];
extern const sU8 matrix2x3_element_order_table[2][2][3];
extern const sU8 matrix2_element_order_table  [2][2][2];

#else

#if 1
// Row major (C-array)
//   0 1 2
//   3 4 5
#define M4(a, row, col)   (((row)<<2)+(col))
#define M3(a, row, col)   (((row)*3)+(col))
#define M2x3(a, row, col) (((row)*3)+(col))
#define M2(a, row, col)   (((row)<<1)+(col))
#else
// Column major (OpenGL)
//   0 2 4
//   1 3 5
#define M4(a, row, col)   (((col)<<2)+(row))
#define M3(a, row, col)   (((col)*3)+(row))
#define M2x3(a, row, col) (((col)*2)+(row))
#define M2(a, row, col)   (((col)<<1)+(row))
#endif // 1
#endif // USE_DYNAMIC_ORDER


#define TM4(row, col)    M4(this,row,col)
#define TM3(row, col)    M3(this,row,col)
#define TM2x3(row, col)  M2x3(this,row,col)
#define TM2(row, col)    M2(this,row,col)

#define OM4(row, col)    M4(o,row,col)
#define OM3(row, col)    M3(o,row,col)
#define OM2x3(row, col)  M2x3(o,row,col)
#define OM2(row, col)    M2(o,row,col)


// Helper macros for accessing elements
#define TM4F(r,c)    floats[TM4(r,c)]
#define TM3F(r,c)    floats[TM3(r,c)]
#define TM2x3F(r,c)  floats[TM2x3(r,c)]
#define TM2F(r,c)    floats[TM2(r,c)]

#define OM4F(r,c)    o->floats[OM4(r,c)]
#define OM3F(r,c)    o->floats[OM3(r,c)]
#define OM2x3F(r,c)  o->floats[OM2x3(r,c)]
#define OM2F(r,c)    o->floats[OM2(r,c)]

#define M4F(a,r,c)   (a)->floats[M4(a,r,c)]
#define M3F(a,r,c)   (a)->floats[M3(a,r,c)]
#define M2x3F(a,r,c) (a)->floats[M2x3(a,r,c)]
#define M2F(a,r,c)   (a)->floats[M2(a,r,c)]


#include "commonmath.h"

Dyac_std_exid_decl;

Dyac_exid_decl(TkMathError);
Dyac_exid_decl(TkMathDivisionByZero);
Dyac_exid_decl(TkMathInvalidValue);
Dyac_exid_decl(TkMathUnsupportedOperator);


#ifndef HAVE_HYPOTF
extern sF32 hypotf_emul(sF32 x, sF32 y);
#endif //HAVE_HYPOTF


#endif // TKMATH_H__
