/// Joystick.h
///
/// (c) 2001-2023 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
#ifndef __TKSDL_JOYSTICK_H__
#define __TKSDL_JOYSTICK_H__

YG("hal1");


/* @class Joystick,Object
Represents an SDL joystick device
*/
YC class _Joystick : public YAC_Object {
private:
	SDL_Joystick *sdl_joystick;
	YAC_String    name;

public:

   /* @constant SDL_HAT_CENTERED,int
   */
//#define SDL_HAT_CENTERED  YCI 0x0


   /* @constant SDL_HAT_UP,int
   */
//#define SDL_HAT_UP        YCI 0x1


   /* @constant SDL_HAT_RIGHT,int
   */
//#define SDL_HAT_RIGHT     YCI 0x2


   /* @constant SDL_HAT_DOWN,int
   */
//#define SDL_HAT_DOWN      YCI 0x4


   /* @constant SDL_HAT_LEFT,int
   */
//#define SDL_HAT_LEFT      YCI 0x8


   /* @constant SDL_HAT_RIGHTUP,int
   */
//#define SDL_HAT_RIGHTUP   YCI 0x3


   /* @constant SDL_HAT_RIGHTDOWN,int
   */
//#define SDL_HAT_RIGHTDOWN YCI 0x6


   /* @constant SDL_HAT_LEFTUP,int
   */
//#define SDL_HAT_LEFTUP    YCI 0x9


   /* @constant SDL_HAT_LEFTDOWN,int
   */
//#define SDL_HAT_LEFTDOWN  YCI 0xc


public:
	_Joystick(void);
	~_Joystick();

	YAC(_Joystick);

	void open(sS32);
	void close(void);

	void YAC_VCALL yacOperator(sSI, YAC_Object*, YAC_Value *);

	// ------------ exported methods ------------

   /* @method getAxis,int axis:int
   Query current value of a joystick axis.

   This is a wrapper for the SDL_JoystickGetAxis() native function.
   */
	YM sSI  _getAxis        (sSI);


   /* @method getBall,int ball,Object ret:boolean
   Query relative trackball motion

   This is a wrapper for the SDL_JoystickGetBall() native function.

   @arg ball ball axis
   @arg array Receives the x/y position. Must be an array able to hold at least 2 integer values
   @return True if the values have been read successfully, false otherwise.
   */
	YM sSI  _getBall        (sSI, YAC_Object *_array);


   /* @method getButton,int button:int
   Query joystick button state

   This is a wrapper for the SDL_JoystickGetButton() native function.

   @arg button Which button
   @return 0=button up, !0=button down
   */
	YM sSI  _getButton      (sSI);


   /* @method getHat,int hat:int
   Query the current state of a joystick hat

   This is a wrapper for the SDL_JoystickGetHat() native function.

   The current state is returned as an unsigned byte is defined as an OR'd combination of one or more of the following:
   §SDL_HAT_CENTERED, §SDL_HAT_UP, §SDL_HAT_RIGHT, §SDL_HAT_DOWN, §SDL_HAT_LEFT, §SDL_HAT_RIGHTUP, §SDL_HAT_RIGHTDOWN, §SDL_HAT_LEFTUP, §SDL_HAT_LEFTDOWN

   @arg hat Which hat
   @return Hat state
   */
	YM sSI  _getHat         (sSI);


   /* @method getIndex:int
   Get the instance ID of an opened joystick (SDL_JoystickInstanceID())
   */
	YM sSI  _getId          (void);


   /* @method getNumAxes:int
   Query number of joystick axes
   */
	YM sSI  _getNumAxes     (void);


   /* @method getNumBalls:int
   Query number of trackballs
   */
	YM sSI  _getNumBalls    (void);


   /* @method getNumButtons:int
   Query number of buttons
   */
	YM sSI  _getNumButtons  (void);


   /* @method getNumHats:int
   Query number of hats
   */
	YM sSI  _getNumHats     (void);


   /* @method getName:String
   Query joystick device name
   */
	YM void _getName        (YAC_Value *_r);
};


#endif  // __TKSDL_JOYSTICK_H__

