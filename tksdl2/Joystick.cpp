/// Joystick.cpp
///
/// (c) 2001-2023 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///

#include <stdlib.h>

#define YAC_BIGSTRING defined
#include <yac.h>

#include "inc_sdl.h"

#include "HAL.h"
#include "Joystick.h"

extern YAC_Host *yac_host;
extern sUI clid_Joystick;


_Joystick::_Joystick(void) {
	sdl_joystick = NULL;
	name.class_ID = YAC_CLID_STRING;
}

_Joystick::~_Joystick() {
	close();
}

void _Joystick::open(sSI _idx) {
	sdl_joystick = ::SDL_JoystickOpen(_idx);

   if(NULL != sdl_joystick)
   {
      const char *c = SDL_JoystickName(sdl_joystick);
      name.copy((sChar*)c);

      if(yac_host->yacGetDebugLevel() > 15)
      {
         yac_host->printf("[...] Joystick[%i]=\"%s\".\n", _idx, (const char*)name.chars);
      }
   }
   else
   {
      yac_host->printf("[~~~] Joystick::open(idx=%d) failed.\n", _idx);
      name.free();
   }
}

void _Joystick::close() {
	if(NULL != sdl_joystick)
	{
		::SDL_JoystickClose(sdl_joystick);

		sdl_joystick = NULL;
	}
}


void YAC_VCALL _Joystick::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   YAC_Object::yacOperator(_cmd, _o, _r);
}

sSI _Joystick::_getAxis(sSI _i) {
	if(NULL != sdl_joystick)
   {
		return SDL_JoystickGetAxis(sdl_joystick, _i);
   }
   return 0;
}

sSI _Joystick::_getBall(sSI _i, YAC_Object *_iv) {
	if(NULL != sdl_joystick)
	{
		if(YAC_VALID(_iv))
		{
         if(_iv->yacArrayGetMaxElements() >= 2)
         {
            int dx;
            int dy;

            sSI r = ::SDL_JoystickGetBall(sdl_joystick, _i, &dx, &dy);

            YAC_Value v;
            v.initInt(dx);
            _iv->yacArraySet(NULL, 0, &v);

            v.initInt(dy);
            _iv->yacArraySet(NULL, 1, &v);

            return r;
         }
		}
	}
	return 0;
}

sSI _Joystick::_getButton(sSI _i) {
   if(NULL != sdl_joystick)
   {
		return ::SDL_JoystickGetButton(sdl_joystick, _i);
   }
   return 0;
}

sSI _Joystick::_getHat(sSI _i) {
   if(NULL != sdl_joystick)
   {
		return ::SDL_JoystickGetHat(sdl_joystick, _i);
   }
   return 0;
}

sSI _Joystick::_getId(void) {
   if(NULL != sdl_joystick)
   {
		// return ::SDL_JoystickIndex(sdl_joystick);
      return ::SDL_JoystickInstanceID(sdl_joystick);
   }
   return -1;
}

sSI _Joystick::_getNumAxes(void) {
   if(NULL != sdl_joystick)
   {
		return ::SDL_JoystickNumAxes(sdl_joystick);
   }
   return 0;
}

sSI _Joystick::_getNumBalls(void) {
   if(NULL != sdl_joystick)
   {
		return ::SDL_JoystickNumBalls(sdl_joystick);
   }
   return 0;
}

sSI _Joystick::_getNumButtons(void) {
   if(NULL != sdl_joystick)
   {
		return ::SDL_JoystickNumButtons(sdl_joystick);
   }
   return 0;
}

sSI _Joystick::_getNumHats(void) {
   if(NULL != sdl_joystick)
   {
		return ::SDL_JoystickNumHats(sdl_joystick);
   }
   return 0;
}

void _Joystick::_getName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->yacCopy(&name);
   _r->initString(s, 1);
}
