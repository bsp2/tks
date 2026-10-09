/// Key.h
///
/// (c) 2001-2023 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
#ifndef __TKSDL_KEY_H__
#define __TKSDL_KEY_H__

YG("hal1");


#define VKEY_PRESSED 0x40000000


/* @class Key,Object
Represents a keyboard event
*/
YC class _Key : public YAC_Object {
public:


/* @constant VKEY_BACKSPACE,int
*/
#define VKEY_BACKSPACE  YCI 8 


/* @constant VKEY_TAB,int
*/
#define VKEY_TAB        YCI 9


/* @constant VKEY_CLEAR,int
*/
#define VKEY_CLEAR      YCI 12


/* @constant VKEY_RETURN,int
*/
#define VKEY_RETURN     YCI 13


/* @constant VKEY_PAUSE,int
*/
#define VKEY_PAUSE      YCI 19


/* @constant VKEY_ESCAPE,int
*/
#define VKEY_ESCAPE     YCI 27


/* @constant VKEY_SPACE,int
*/
#define VKEY_SPACE      YCI 32


/* @constant VKEY_EXCLAIM,int
*/
#define VKEY_EXCLAIM    YCI 33


/* @constant VKEY_QUOTEDBL,int
*/
#define VKEY_QUOTEDBL   YCI 34


/* @constant VKEY_HASH,int
*/
#define VKEY_HASH       YCI 35


/* @constant VKEY_DOLLAR,int
*/
#define VKEY_DOLLAR     YCI 36


/* @constant VKEY_AMPERSAND,int
*/
#define VKEY_AMPERSAND  YCI 38


/* @constant VKEY_QUOTE,int
*/
#define VKEY_QUOTE      YCI 39


/* @constant VKEY_LEFTPAREN,int
*/
#define VKEY_LEFTPAREN  YCI 40


/* @constant VKEY_RIGHTPAREN,int
*/
#define VKEY_RIGHTPAREN YCI 41


/* @constant VKEY_ASTERISK,int
*/
#define VKEY_ASTERISK   YCI 42


/* @constant VKEY_PLUS,int
*/
#define VKEY_PLUS       YCI 43


/* @constant VKEY_COMMA,int
*/
#define VKEY_COMMA      YCI 44


/* @constant VKEY_MINUS,int
*/
#define VKEY_MINUS      YCI 45


/* @constant VKEY_PERIOD,int
*/
#define VKEY_PERIOD     YCI 46


/* @constant VKEY_SLASH,int
*/
#define VKEY_SLASH      YCI 47


/* @constant VKEY_0,int
*/
#define VKEY_0          YCI 48


/* @constant VKEY_1,int
*/
#define VKEY_1          YCI 49


/* @constant VKEY_2,int
*/
#define VKEY_2          YCI 50


/* @constant VKEY_3,int
*/
#define VKEY_3          YCI 51


/* @constant VKEY_4,int
*/
#define VKEY_4          YCI 52


/* @constant VKEY_5,int
*/
#define VKEY_5          YCI 53


/* @constant VKEY_6,int
*/
#define VKEY_6          YCI 54


/* @constant VKEY_7,int
*/
#define VKEY_7          YCI 55


/* @constant VKEY_8,int
*/
#define VKEY_8          YCI 56


/* @constant VKEY_9,int
*/
#define VKEY_9          YCI 57


/* @constant VKEY_COLON,int
*/
#define VKEY_COLON      YCI 58


/* @constant VKEY_SEMICOLON,int
*/
#define VKEY_SEMICOLON  YCI 59


/* @constant VKEY_LESS,int
*/
#define VKEY_LESS       YCI 60


/* @constant VKEY_EQUALS,int
*/
#define VKEY_EQUALS     YCI 61


/* @constant VKEY_GREATER,int
*/
#define VKEY_GREATER    YCI 62


/* @constant VKEY_QUESTION,int
*/
#define VKEY_QUESTION   YCI 63


/* @constant VKEY_AT,int
*/
#define VKEY_AT         YCI 64


/* @constant VKEY_LEFTBRACKET,int
*/
#define VKEY_LEFTBRACKET YCI 91


/* @constant VKEY_BACKSLASH,int
*/
#define VKEY_BACKSLASH  YCI 92


/* @constant VKEY_RIGHTBRACKET,int
*/
#define VKEY_RIGHTBRACKET YCI 93


/* @constant VKEY_CARET,int
*/
#define VKEY_CARET        YCI 94


/* @constant VKEY_UNDERSCORE,int
*/
#define VKEY_UNDERSCORE   YCI 95


/* @constant VKEY_BACKQUOTE,int
*/
#define VKEY_BACKQUOTE    YCI 96


/* @constant VKEY_DELETE,int
*/
#define VKEY_DELETE       YCI 127



/* Numeric keypad */



/* @constant VKEY_KP_0,int
*/
#define VKEY_KP0          YCI 256
#define VKEY_KP_0         YCI 256


/* @constant VKEY_KP_1,int
*/
#define VKEY_KP1          YCI 257
#define VKEY_KP_1         YCI 257


/* @constant VKEY_KP_2,int
*/
#define VKEY_KP2          YCI 258
#define VKEY_KP_2         YCI 258


/* @constant VKEY_KP_3,int
*/
#define VKEY_KP3          YCI 259
#define VKEY_KP_3         YCI 259


/* @constant VKEY_KP_4,int
*/
#define VKEY_KP4          YCI 260
#define VKEY_KP_4         YCI 260


/* @constant VKEY_KP_5,int
*/
#define VKEY_KP5          YCI 261
#define VKEY_KP_5         YCI 261


/* @constant VKEY_KP_6,int
*/
#define VKEY_KP6          YCI 262
#define VKEY_KP_6         YCI 262


/* @constant VKEY_KP_7,int
*/
#define VKEY_KP7          YCI 263
#define VKEY_KP_7         YCI 263


/* @constant VKEY_KP_8,int
*/
#define VKEY_KP8          YCI 264
#define VKEY_KP_8         YCI 264


/* @constant VKEY_KP_9,int
*/
#define VKEY_KP9          YCI 265
#define VKEY_KP_9         YCI 265


/* @constant VKEY_KP_PERIOD,int
*/
#define VKEY_KP_PERIOD    YCI 266


/* @constant VKEY_KP_DIVIDE,int
*/
#define VKEY_KP_DIVIDE    YCI 267


/* @constant VKEY_KP_MULTIPLY,int
*/
#define VKEY_KP_MULTIPLY  YCI 268


/* @constant VKEY_KP_MINUS,int
*/
#define VKEY_KP_MINUS     YCI 269


/* @constant VKEY_KP_PLUS,int
*/
#define VKEY_KP_PLUS      YCI 270


/* @constant VKEY_KP_ENTER,int
*/
#define VKEY_KP_ENTER     YCI 271


/* @constant VKEY_KP_EQUALS,int
*/
#define VKEY_KP_EQUALS    YCI 272





/* Arrows + Home/End pad */





/* @constant VKEY_UP,int
*/
#define VKEY_UP           YCI 273


/* @constant VKEY_DOWN,int
*/
#define VKEY_DOWN         YCI 274


/* @constant VKEY_RIGHT,int
*/
#define VKEY_RIGHT        YCI 275


/* @constant VKEY_LEFT,int
*/
#define VKEY_LEFT         YCI 276


/* @constant VKEY_INSERT,int
*/
#define VKEY_INSERT       YCI 277


/* @constant VKEY_HOME,int
*/
#define VKEY_HOME         YCI 278


/* @constant VKEY_END,int
*/
#define VKEY_END          YCI 279


/* @constant VKEY_PAGEUP,int
*/
#define VKEY_PAGEUP       YCI 280


/* @constant VKEY_PAGEDOWN,int
*/
#define VKEY_PAGEDOWN     YCI 281




/* Function keys */




/* @constant VKEY_F1,int
*/
#define VKEY_F1           YCI 282


/* @constant VKEY_F2,int
*/
#define VKEY_F2           YCI 283


/* @constant VKEY_F3,int
*/
#define VKEY_F3           YCI 284


/* @constant VKEY_F4,int
*/
#define VKEY_F4           YCI 285


/* @constant VKEY_F5,int
*/
#define VKEY_F5           YCI 286


/* @constant VKEY_F6,int
*/
#define VKEY_F6           YCI 287


/* @constant VKEY_F7,int
*/
#define VKEY_F7           YCI 288


/* @constant VKEY_F8,int
*/
#define VKEY_F8           YCI 289


/* @constant VKEY_F9,int
*/
#define VKEY_F9           YCI 290


/* @constant VKEY_F10,int
*/
#define VKEY_F10          YCI 291


/* @constant VKEY_F11,int
*/
#define VKEY_F11          YCI 292


/* @constant VKEY_F12,int
*/
#define VKEY_F12          YCI 293


/* @constant VKEY_F13,int
*/
#define VKEY_F13          YCI 294


/* @constant VKEY_F14,int
*/
#define VKEY_F14          YCI 295


/* @constant VKEY_F15,int
*/
#define VKEY_F15          YCI 296




/* Key state modifier keys */




/* @constant VKEY_NUMLOCK,int
*/
#define VKEY_NUMLOCK      YCI 300


/* @constant VKEY_CAPSLOCK,int
*/
#define VKEY_CAPSLOCK     YCI 301


/* @constant VKEY_SCROLLOCK,int
*/
#define VKEY_SCROLLOCK    YCI 302


/* @constant VKEY_RSHIFT,int
*/
#define VKEY_RSHIFT       YCI 303


/* @constant VKEY_LSHIFT,int
*/
#define VKEY_LSHIFT       YCI 304


/* @constant VKEY_RCTRL,int
*/
#define VKEY_RCTRL        YCI 305


/* @constant VKEY_LCTRL,int
*/
#define VKEY_LCTRL        YCI 306


/* @constant VKEY_RALT,int
*/
#define VKEY_RALT         YCI 307


/* @constant VKEY_LALT,int
 *  (note) left control key on macOS
*/
#define VKEY_LALT         YCI 308


/* @constant VKEY_RMETA,int
*/
#define VKEY_RMETA        YCI 309


/* @constant VKEY_LMETA,int
 *  (note) left command key on macOS
*/
#define VKEY_LMETA        YCI 310


/* @constant VKEY_LSUPER,int
*/
#define VKEY_LSUPER       YCI 311		/* Left "Windows" key */


/* @constant VKEY_RSUPER,int
*/
#define VKEY_RSUPER       YCI 312		/* Right "Windows" key */


/* @constant VKEY_MODE,int
*/
#define VKEY_MODE         YCI 313		/* "Alt Gr" key */


/* @constant VKEY_COMPOSE,int
*/
#define VKEY_COMPOSE      YCI 314   	/* Multi-key compose key */





/* Miscellaneous function keys */




/* @constant VKEY_HELP,int
*/
#define VKEY_HELP         YCI 315


/* @constant VKEY_PRINT,int
*/
#define VKEY_PRINT        YCI 316


/* @constant VKEY_SYSREQ,int
*/
#define VKEY_SYSREQ       YCI 317


/* @constant VKEY_BREAK,int
*/
#define VKEY_BREAK        YCI 318


/* @constant VKEY_MENU,int
*/
#define VKEY_MENU         YCI 319


/* @constant VKEY_POWER,int
*/
#define VKEY_POWER        YCI 320  	/* Power Macintosh power key */


/* @constant VKEY_EURO,int
*/
#define VKEY_EURO         YCI 321	/* Some european keyboards */




/* virtual digital joystick key-events */




/* @constant VKEY_MOUSEBUTTON0,int
*/
#define VKEY_MOUSEBUTTON0 YCI 65537


/* @constant VKEY_MOUSEBUTTON1,int
*/
#define VKEY_MOUSEBUTTON1 YCI 65538 /* right  */


/* @constant VKEY_MOUSEBUTTON2,int
*/
#define VKEY_MOUSEBUTTON2 YCI 65539 /* middle */


/* @constant VKEY_MOUSEBUTTON3,int
*/
#define VKEY_MOUSEBUTTON3 YCI 65540 /* wheel up */


/* @constant VKEY_MOUSEBUTTON4,int
*/
#define VKEY_MOUSEBUTTON4 YCI 65541 /* wheel down */


/* @constant VKEY_JOY0AXIS0NEG,int
*/
#define VKEY_JOY0AXIS0NEG YCI 65542 /* left */


/* @constant VKEY_JOY0AXIS0POS,int
*/
#define VKEY_JOY0AXIS0POS YCI 65543 /* right */


/* @constant VKEY_JOY0AXIS1NEG,int
*/
#define VKEY_JOY0AXIS1NEG YCI 65544 /* up*/


/* @constant VKEY_JOY0AXIS1POS,int
*/
#define VKEY_JOY0AXIS1POS YCI 65545 /* down */


/* @constant VKEY_JOY0BUTTON0,int
*/
#define VKEY_JOY0BUTTON0  YCI 65546


/* @constant VKEY_JOY0BUTTON1,int
*/
#define VKEY_JOY0BUTTON1  YCI 65547


/* @constant VKEY_JOY0BUTTON2,int
*/
#define VKEY_JOY0BUTTON2  YCI 65548


/* @constant VKEY_JOY0BUTTON3,int
*/
#define VKEY_JOY0BUTTON3  YCI 65549


/* @constant VKEY_JOY0BUTTON4,int
*/
#define VKEY_JOY0BUTTON4  YCI 65550


/* @constant VKEY_JOY0BUTTON5,int
*/
#define VKEY_JOY0BUTTON5  YCI 65551


/* @constant VKEY_JOY0BUTTON6,int
*/
#define VKEY_JOY0BUTTON6  YCI 65552


/* @constant VKEY_JOY0BUTTON7,int
*/
#define VKEY_JOY0BUTTON7  YCI 65553


/* @constant VKEY_JOY0BUTTON8,int
*/
#define VKEY_JOY0BUTTON8  YCI 65554


/* @constant VKEY_JOY1AXIS0NEG,int
*/
#define VKEY_JOY1AXIS0NEG YCI 65555


/* @constant VKEY_JOY1AXIS0POS,int
*/
#define VKEY_JOY1AXIS0POS YCI 65556


/* @constant VKEY_JOY1AXIS1NEG,int
*/
#define VKEY_JOY1AXIS1NEG YCI 65557


/* @constant VKEY_JOY1AXIS1POS,int
*/
#define VKEY_JOY1AXIS1POS YCI 65558


/* @constant VKEY_JOY1BUTTON0,int
*/
#define VKEY_JOY1BUTTON0  YCI 65559


/* @constant VKEY_JOY1BUTTON1,int
*/
#define VKEY_JOY1BUTTON1  YCI 65560


/* @constant VKEY_JOY1BUTTON2,int
*/
#define VKEY_JOY1BUTTON2  YCI 65561


/* @constant VKEY_JOY1BUTTON3,int
*/
#define VKEY_JOY1BUTTON3  YCI 65562


/* @constant VKEY_JOY1BUTTON4,int
*/
#define VKEY_JOY1BUTTON4  YCI 65563


/* @constant VKEY_JOY1BUTTON5,int
*/
#define VKEY_JOY1BUTTON5  YCI 65564


/* @constant VKEY_JOY1BUTTON6,int
*/
#define VKEY_JOY1BUTTON6  YCI 65565


/* @constant VKEY_JOY1BUTTON7,int
*/
#define VKEY_JOY1BUTTON7  YCI 65566


/* @constant VKEY_JOY1BUTTON8,int
*/
#define VKEY_JOY1BUTTON8  YCI 65567


/* @constant VKEY_JOY2AXIS0NEG,int
*/
#define VKEY_JOY2AXIS0NEG YCI 65568


/* @constant VKEY_JOY2AXIS0POS,int
*/
#define VKEY_JOY2AXIS0POS YCI 65569


/* @constant VKEY_JOY2AXIS1NEG,int
*/
#define VKEY_JOY2AXIS1NEG YCI 65570


/* @constant VKEY_JOY2AXIS1POS,int
*/
#define VKEY_JOY2AXIS1POS YCI 65571


/* @constant VKEY_JOY2BUTTON0,int
*/
#define VKEY_JOY2BUTTON0  YCI 65572


/* @constant VKEY_JOY2BUTTON1,int
*/
#define VKEY_JOY2BUTTON1  YCI 65573


/* @constant VKEY_JOY2BUTTON2,int
*/
#define VKEY_JOY2BUTTON2  YCI 65574


/* @constant VKEY_JOY2BUTTON3,int
*/
#define VKEY_JOY2BUTTON3  YCI 65575


/* @constant VKEY_JOY2BUTTON4,int
*/
#define VKEY_JOY2BUTTON4  YCI 65576


/* @constant VKEY_JOY2BUTTON5,int
*/
#define VKEY_JOY2BUTTON5  YCI 65577


/* @constant VKEY_JOY2BUTTON6,int
*/
#define VKEY_JOY2BUTTON6  YCI 65578


/* @constant VKEY_JOY2BUTTON7,int
*/
#define VKEY_JOY2BUTTON7  YCI 65579


/* @constant VKEY_JOY2BUTTON8,int
*/
#define VKEY_JOY2BUTTON8  YCI 65580


/* @constant VKEY_JOY3AXIS0NEG,int
*/
#define VKEY_JOY3AXIS0NEG YCI 65581


/* @constant VKEY_JOY3AXIS0POS,int
*/
#define VKEY_JOY3AXIS0POS YCI 65582


/* @constant VKEY_JOY3AXIS1NEG,int
*/
#define VKEY_JOY3AXIS1NEG YCI 65583


/* @constant VKEY_JOY3AXIS1POS,int
*/
#define VKEY_JOY3AXIS1POS YCI 65584


/* @constant VKEY_JOY3BUTTON0,int
*/
#define VKEY_JOY3BUTTON0  YCI 65585


/* @constant VKEY_JOY3BUTTON1,int
*/
#define VKEY_JOY3BUTTON1  YCI 65586


/* @constant VKEY_JOY3BUTTON2,int
*/
#define VKEY_JOY3BUTTON2  YCI 65587


/* @constant VKEY_JOY3BUTTON3,int
*/
#define VKEY_JOY3BUTTON3  YCI 65588


/* @constant VKEY_JOY3BUTTON4,int
*/
#define VKEY_JOY3BUTTON4  YCI 65589


/* @constant VKEY_JOY3BUTTON5,int
*/
#define VKEY_JOY3BUTTON5  YCI 65590


/* @constant VKEY_JOY3BUTTON6,int
*/
#define VKEY_JOY3BUTTON6  YCI 65591


/* @constant VKEY_JOY3BUTTON7,int
*/
#define VKEY_JOY3BUTTON7  YCI 65592


/* @constant VKEY_JOY3BUTTON8,int
*/
#define VKEY_JOY3BUTTON8  YCI 65593


//#define VKEY_PRESSED      YCI 0x40000000




/* additional modifiers */




/* @constant VMOD_LSHIFT,int
*/
#define VMOD_LSHIFT       YCI 1


/* @constant VMOD_RSHIFT,int
*/
#define VMOD_RSHIFT       YCI 2


/* @constant VMOD_LCTRL,int
*/
#define VMOD_LCTRL        YCI 64


/* @constant VMOD_RCTRL,int
*/
#define VMOD_RCTRL        YCI 128


/* @constant VMOD_LALT,int
*/
#define VMOD_LALT         YCI 256


/* @constant VMOD_RALT,int
*/
#define VMOD_RALT         YCI 512


/* @constant VMOD_LMETA,int
 *  == KMOD_LGUI
*/
#define VMOD_LMETA        YCI 1024


/* @constant VMOD_RMETA,int
 *  == KMOD_RGUI
*/
#define VMOD_RMETA        YCI 2048


/* @constant VMOD_NUM,int
*/
#define VMOD_NUM          YCI 4096


/* @constant VMOD_CAPS,int
*/
#define VMOD_CAPS         YCI 8192


/* @constant VMOD_MODE,int
*/
#define VMOD_MODE         YCI 16384


/* @constant DCON,int
*/
#define DCON              YCI 0




/* @constant UC_CTRL_A,int
*/
#define UC_CTRL_A YCI 1


/* @constant UC_CTRL_B,int
*/
#define UC_CTRL_B YCI 2


/* @constant UC_CTRL_C,int
*/
#define UC_CTRL_C YCI 3


/* @constant UC_CTRL_D,int
*/
#define UC_CTRL_D YCI 4


/* @constant UC_CTRL_E,int
*/
#define UC_CTRL_E YCI 5


/* @constant UC_CTRL_F,int
*/
#define UC_CTRL_F YCI 6


/* @constant UC_CTRL_G,int
*/
#define UC_CTRL_G YCI 7


/* @constant UC_CTRL_H,int
*/
#define UC_CTRL_H YCI 8


/* @constant UC_CTRL_I,int
*/
#define UC_CTRL_I YCI 9


/* @constant UC_CTRL_J,int
*/
#define UC_CTRL_J YCI 10


/* @constant UC_CTRL_K,int
*/
#define UC_CTRL_K YCI 11


/* @constant UC_CTRL_L,int
*/
#define UC_CTRL_L YCI 12


/* @constant UC_CTRL_M,int
*/
#define UC_CTRL_M YCI 13


/* @constant UC_CTRL_N,int
*/
#define UC_CTRL_N YCI 14


/* @constant UC_CTRL_O,int
*/
#define UC_CTRL_O YCI 15


/* @constant UC_CTRL_P,int
*/
#define UC_CTRL_P YCI 16


/* @constant UC_CTRL_Q,int
*/
#define UC_CTRL_Q YCI 17


/* @constant UC_CTRL_R,int
*/
#define UC_CTRL_R YCI 18


/* @constant UC_CTRL_S,int
*/
#define UC_CTRL_S YCI 19


/* @constant UC_CTRL_T,int
*/
#define UC_CTRL_T YCI 20


/* @constant UC_CTRL_U,int
*/
#define UC_CTRL_U YCI 21


/* @constant UC_CTRL_V,int
*/
#define UC_CTRL_V YCI 22


/* @constant UC_CTRL_W,int
*/
#define UC_CTRL_W YCI 23


/* @constant UC_CTRL_X,int
*/
#define UC_CTRL_X YCI 24


/* @constant UC_CTRL_Y,int
*/
#define UC_CTRL_Y YCI 25


/* @constant UC_CTRL_Z,int
*/
#define UC_CTRL_Z YCI 26



// (todo) why are the STDOUT/STDERR defines in Key.h ?!??


/* @constant STDOUT,int
*/
//#define STDOUT            YCI 1




/* @constant STDERR,int
*/
//#define STDERR            YCI 2


public:
    sUI vkey;
    sUI vmod;
    sUI unicode;

public:
    _Key(void);
    _Key(sUI _vkey, sUI _vmod, sUI _unicode);
    ~_Key() { };

	YAC(_Key);

	const char *getCodeName(sUI _vkey);
	void appendModName(YAC_String *_s, sUI _vmod);
	sUI getCodeByName(YAC_String *);
	sUI getModByName(YAC_String *);
	void setModifiersByString(YAC_String *_s);
	
	void YAC_VCALL yacSerialize(YAC_Object *, sUI);
	sUI  YAC_VCALL yacDeserialize(YAC_Object *, sUI);
	
	void YAC_VCALL yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r);

	 // ------------ exported methods ------------
   /* @method getCode:int
   Query raw key code
   */
	YM sUI _getCode      (void);


   /* @method getMod:int
   Query modifier mask
   */
	YM sUI _getMod       (void);


   /* @method getPressed:int
   Query pressed (raw) key code. Returns 0 if this is not a keypress event.
   */
	YM sUI _getPressed   (void);


   /* @method getReleased:int
   Query release (raw) key code. Return 0 if this is not a keyrelease event.
   */
	YM sUI _getReleased  (void);


   /* @method getAlpha:int
   Return (untranslated) ASCII code or 0 if this is a special key event.
   */
	YM sUI _getAlpha     (void);


   /* @method getUnicode:int
   Return (translated) unicode char.

   @see SDL.enableUNICODE
   */
   YM sUI _getUnicode   (void);


   /* @method getCodeName:String
   Return raw keycode name
   */
	YM void _getCodeName (YAC_Value *_r);


   /* @method getModName:String
   Return modifier mask name
   */
	YM void _getModName  (YAC_Value *_r);


   /* @method getName:String
   Return full keyevent name

   E.g. "lctrl-c"
   */
	YM void _getName     (YAC_Value *_r);


   /* @method _getNameFor,int key,int mod:String
   Return full name for given key code and modifier mask.
   */
	YM void _getNameFor  (sUI _vkey, sUI _vmod, YAC_Value *_r);


   /* @method setCode,int code
   Set raw keycode.

   Does not modify press/release mask.
   */
	YM void _setCode     (sUI);


   /* @method setMod,int mod
   Set modifier mask
   */
	YM void _setMod      (sUI);


   /* @method setPressed,int code
   Set raw keycode and set pressed flag.
   */
	YM void _setPressed  (sUI);


   /* @method setReleased,int code
   Set raw keycode and set released flag.
   */
	YM void _setReleased (sUI);


   /* @method setUnicode,int c
   Set unicode char

   The character will *not* be translated back to its original raw key code!
   */
   YM void _setUnicode  (sSI);


   /* @method setAlpha,int c
   Set (untranslated) ASCII key code
   */
	YM void _setAlpha    (sUI);


   /* @method setCodeName,String s
   Set key code by name
   */
	YM void _setCodeName (YAC_String *_s);


   /* @method setModName,String s
   Set modifier mask by name
   */
	YM void _setModName  (YAC_String *_s);


   /* @method setName,String s
   Set key code and modifier mask by name
   */
	YM void _setName     (YAC_Object *_s);


   /* @method modNone:boolean
   Query whether no key modifiers are used
   */
   YM sBool  _modNone   (void);


   /* @method modAlt:boolean
   Query whether left or right alt key modifiers are used
   */
   YM sBool  _modAlt      (void);


   /* @method modAltOnly:boolean
   Query whether only the left or right alt key modifiers are used
   */
   YM sBool  _modAltOnly  (void);


   /* @method modCmd:boolean
   Query whether left or right (macOS) command key modifiers are used
   (== Alt key on PC keyboards)
   */
   YM sBool  _modCmd      (void);


   /* @method modCmdOnly:boolean
   Query whether only the left or right alt key modifiers are used
   (== Alt key on PC keyboards)
   */
   YM sBool  _modCmdOnly  (void);


   /* @method modShift:boolean
   Query whether left or right shift key modifiers are used
   */
   YM sBool  _modShift    (void);


   /* @method modShiftOnly:boolean
   Query whether only the left or right shift key modifiers are used
   */
   YM sBool  _modShiftOnly (void);


   /* @method modCtrl:boolean
   Query whether left or right control key modifiers are used
   */
   YM sBool  _modCtrl     (void);


   /* @method modCtrlOnly:boolean
   Query whether only the left or right control key modifiers are used
   */
   YM sBool  _modCtrlOnly (void);


   /* @method _modCaps:boolean
   Query whether capslock modifier is used
   */
   YM sBool  _modCaps     (void);


   /* @method _modShiftOrCaps:boolean
   Query whether left or right shift key modifiers or capslock is used
   */
   YM sBool  _modShiftOrCaps (void);


   /* @method SetLocalToRawKeyTableByName,String name:boolean
      Install localized keycode to raw scancode translation table.
      (note) added for macOS where the SDL Key.code depends on the current input source setting
      @arg name Local name (domain suffix). "de", "us", "fr", ..
      @return true if table has been installed succesfully
   */
   YM sBool SetLocalToRawKeyTableByName (YAC_String *_name);
};


#endif // __TKSDL_KEY_H__
