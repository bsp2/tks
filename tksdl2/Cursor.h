/// Cursor.h
///
/// (c) 2006-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __TKSDL_CURSOR_H__
#define __TKSDL_CURSOR_H__


YG("hal1");


/* @class Cursor
   Represents a mouse cursor shape
*/
YC class _Cursor : public YAC_Object {
public:
   SDL_Cursor *sdl_cursor;
   SDL_Cursor *previous_cursor;
   sU8        *data;
   sU8        *mask;
   sSI         width, height, hotx, hoty;
   sBool       b_created;

public:
	_Cursor(void);
	~_Cursor();

	YAC(_Cursor);

   void createCursor (void);
   void freeCursor   (void);
   
   // "X #000000", ". #ffffff", "  transparent"

   /* @method create(String pixels,int w,int h,int hotx,int hoty:boolean
      Create new monochrome cursor shape from string. 'X' or 'x' means a set pixel and '.' means a cleared pixel.
      @arg w Width
      @arg h Height
      @arg hotx Hotspot x
      @arg hoty Hotspot y
      @return True if shape has been created, false if something went terribly wrong
   */
   YM sBool _create (YAC_Object *_s, sSI _w, sSI _h, sSI _hotx, sSI _hoty);

   
   /* @method show:boolean
      Change cursor to this shape. 
      Remembers previous cursor shape so it can be reverted

      @return True if the new shape has been set, false if something went terribly wrong
   */
   YM sBool _show  (void);


   /* @method revert
      Revert to previous cursor shape
   */
   YM void _revert (void);
};



#endif
