/// Matrix.h
///
/// (c) 2008-2024 by Bastian Spiegel <bs@tkscript.de>
///
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef MATRIX_H_
#define MATRIX_H_

YG("Math");


/* @class Matrix,Object
Base class for all matrix objects
*/
YCR class _Matrix : public YAC_Object {
  public:
   sBool b_column_major; // true=column major (e.g. OpenGL=row major)  false=row major (e.g. C-Array / Direct3D, default)

  public:
   _Matrix(void);
   ~_Matrix();

   YAC(_Matrix);

   /* @method setColumnMajor,boolean bEnabled
     Set element order (column major/row major)

     @arg bEnable true=Column major mode, false=Row major mode
   */
   YM virtual void _setColumnMajor (sSI _bEnabled);


   /* @method getColumnMajor:boolean
    Get current element order

    @return True if matrix is in column major mode, false if it uses row major mode
   */
   YM virtual sSI  _getColumnMajor (void) const;
};


#endif // MATRIX_H_
