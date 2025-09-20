/* freeglut_geometry_tks.h
 *
 * Freeglut geometry rendering methods.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * Ported to tkscript by Carsten Busse <carsten.busse@googlemail.com>.
 *
 */

#ifndef __FREEGLUT_GEOMETRY_TKS_H__


YG("freeglut_geometry");


YF void YAC_CALL _glutWireCube(sF32);
YF void YAC_CALL _glutSolidCube(sF32);
YF void YAC_CALL _glutSolidSphere(sF32,sSI,sSI);
YF void YAC_CALL _glutWireSphere(sF32,sSI,sSI);
YF void YAC_CALL _glutSolidCone(sF32,sF32,sSI,sSI);
YF void YAC_CALL _glutWireCone(sF32,sF32,sSI,sSI);
YF void YAC_CALL _glutSolidCylinder(sF32,sF32,sSI,sSI);
YF void YAC_CALL _glutWireCylinder(sF32,sF32,sSI,sSI);
YF void YAC_CALL _glutWireTorus(sF32,sF32,sSI,sSI);
YF void YAC_CALL _glutSolidTorus(sF32,sF32,sSI,sSI);
YF void YAC_CALL _glutWireDodecahedron();
YF void YAC_CALL _glutSolidDodecahedron();
YF void YAC_CALL _glutWireOctahedron();
YF void YAC_CALL _glutSolidOctahedron();
YF void YAC_CALL _glutWireTetrahedron();
YF void YAC_CALL _glutSolidTetrahedron();
YF void YAC_CALL _glutWireIcosahedron();
YF void YAC_CALL _glutSolidIcosahedron();
YF void YAC_CALL _glutWireRhombicDodecahedron();
YF void YAC_CALL _glutSolidRhombicDodecahedron();
YF void YAC_CALL _glutWireSierpinskiSponge(sSI,YAC_Object*,sF32);
YF void YAC_CALL _glutSolidSierpinskiSponge(sSI,YAC_Object*,sF32);


#endif // __FREEGLUT_GEOMETRY_TKS_H__
