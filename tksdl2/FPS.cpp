/// FPS.cpp
///
/// (c) 2001-2023 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///

#include <stdlib.h>

#define YAC_PRINTF defined
#include <yac.h>

#include "inc_sdl.h"

#include "HAL.h"
#include "FPS.h"

extern YAC_Host *yac_host;
extern sUI clid_Joystick;

sF32 _FPS::_getCurrent(void) {
    return tks_HAL->fps.current_fps;
}

sF32 _FPS::_getAverage(void) {
    return tks_HAL->fps.average_fps;
}

void _FPS::_reset(void) {
    tks_HAL->initFPS();
}

sF32 _FPS::_getMax(void) {
   return (1000.0f / ((sF32)tks_HAL->max_frame_deltatime));
}

void _FPS::_setMax(sF32 _fps) {
   if(_fps > 0.0f)
   {
      tks_HAL->max_frame_deltatime = (sU32) (1000.0f / _fps);
   }
}

sF32 _FPS::_getReal(void) {
   return tks_HAL->fps.real_fps;
}

sF32 _FPS::_getPrecision(void) {
	return tks_HAL->fps.precision;
}

sSI _FPS::_getTotalTime(void) {
	return tks_HAL->fps.total_time;
}

sSI _FPS::_getNumFramesRendered(void) {
	return tks_HAL->fps.num_frames_rendered;
}

void _FPS::_setTickInterval(sF32 _v) {
   if(_v > 0.0f)
   {
      tks_HAL->fps.tick_interval = _v;
   }
}

sF32 _FPS::_getTickInterval(void) {
	return tks_HAL->fps.tick_interval;
}

void _FPS::_setTickBuffer(sSI _ival) {
	tks_HAL->fps.tick_buffer = _ival;
}

sSI _FPS::_getTickBuffer(void) {
	return tks_HAL->fps.tick_buffer;
}

sUI _FPS::_getLimit(void) {
	return tks_HAL->fps.limit_fps;
}

void _FPS::_setLimit(sUI _limit) {
	tks_HAL->fps.limit_fps = _limit;
}
