/// Mouse.h
///
/// (c) 2001-2011 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
#ifndef __TKSDL_MOUSE_H__
#define __TKSDL_MOUSE_H__

YG("hal1");


#define TKS_MOUSE_SHOW_ENABLE   0x81
#define TKS_MOUSE_SHOW_DISABLE  0x80


/* @class Mouse,Object
Static wrapper class for the Mouse device
*/
YCS class _Mouse : public YAC_Object {
public:
	YAC(_Mouse);

	 // ------------ exported methods ------------

   /* @method grab
   Grab mouse
   */
	YM void _grab              (void);

   
   /* @method ungrab
   Ungrab mouse
   */
   YM void _ungrab            (void);

   
   /* @method toggleGrab
   Toggle mouse grab
   */
   YM void _toggleGrab        (void);

   
   /* @method getX:int
   Get absolute horizontal mouse position
   */
	YM sSI  _getX              (void);

   
   /* @method getY:int
   Get absolute vertical mouse position
   */
	YM sSI  _getY              (void);

   
   /* @method getButtons:int
   Get current button mask
   */
	YM sSI  _getButtons        (void);

   
   /* @method isGrabbed:boolean
   Query whether the mouse is currently grabbed
   */
	YM sSI  _isGrabbed         (void);

   
   /* @method getDx:int
   Get relative horizontal mouse position
   */
	YM sSI  _getDx             (void);

   
   /* @method getDy
   Get relative vertical mouse position
   */
	YM sSI  _getDy             (void);

   
   /* @method unproject,Object nearRet,Object farRet
   Unproject mouse position and return near/far vectors

   @arg nearRet Receives the near vector. Must be able to hold at least 3 array elements.
   @arg farRet Receives the far vector. Must be able to hold at least 3 array elements.
   */
	YM void _unproject         (YAC_Object *_vnearret, YAC_Object *_vfarret);

   
   /* @method getChangedButtons
   Query changed button mask
   */
	YM sSI  _getChangedButtons (void);

   
   /* @method showPointer,boolean bVisible
   Show/hide the mouse pointer
   */
	YM void _showPointer       (sSI);

   
   /* @method setPointerMode,int mode
   Set autohide pointer mode.

   @arg mode One of §MOUSE_POINTER_ENTER, §MOUSE_POINTER_MOVE or §MOUSE_POINTER_MANUAL
   */
	YM void _setPointerMode    (sSI);

   
   /* @method getPointerMode:int
   Query current mouse pointer autohide mode.
   */
	YM sSI  _getPointerMode    (void);

   
   /* @method warpTo,int x,int y
   Move mouse cursor to specified window coordinates
      @arg x
      @arg y
    */
   YM void warpTo             (sUI _x, sUI _y);
};


#endif // __TKSDL_MOUSE_H__
