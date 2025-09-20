/// Mouse.cpp
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
#include "Mouse.h"

extern YAC_Host *yac_host;

#include "../tkopengl/inc_opengl.h"
#include "../tkopengl/tkopengl_shared.h"


void _Mouse::_grab(void) {
   // yac_host->printf("xxx SDL_WM_GrabInput ON\n");
#if SDL_VERSION_ATLEAST(2,0,16)
   ::SDL_SetWindowMouseGrab(tks_HAL->sdl_window, SDL_TRUE);
#else
   ::SDL_SetWindowGrab(tks_HAL->sdl_window, SDL_TRUE);
#endif
   ::SDL_GetMouseState(&tks_HAL->mouse.grab_start_x, &tks_HAL->mouse.grab_start_y);

   tks_HAL->mouse.warp_x  = tks_HAL->view_sx/2;
   tks_HAL->mouse.warp_y  = tks_HAL->view_sy/2;
   tks_HAL->mouse.grabbed = YAC_TRUE;
   tks_HAL->mouse.show    = TKS_MOUSE_SHOW_DISABLE;

   tks_HAL->mouse.b_grab_changed = YAC_TRUE;
}

void _Mouse::_ungrab(void) {
   // yac_host->printf("xxx SDL_WM_GrabInput OFF\n");

   tks_HAL->mouse.grabbed = YAC_FALSE;
   tks_HAL->mouse.show    = TKS_MOUSE_SHOW_ENABLE;

#if SDL_VERSION_ATLEAST(2,0,16)
   ::SDL_SetWindowMouseGrab(tks_HAL->sdl_window, SDL_FALSE);
#else
   ::SDL_SetWindowGrab(tks_HAL->sdl_window, SDL_FALSE);
#endif

   tks_HAL->mouse.warp_x = tks_HAL->mouse.grab_start_x;
   tks_HAL->mouse.warp_y = tks_HAL->mouse.grab_start_y;

   tks_HAL->mouse.b_grab_changed = YAC_TRUE;
}

void _Mouse::_toggleGrab() {
   if(tks_HAL->mouse.grabbed)
      _ungrab();
   else
      _grab();
}

void _Mouse::warpTo(sUI _x, sUI _y) {
   tks_HAL->mouse.warp_x = (sU16)_x;
   tks_HAL->mouse.warp_y = (sU16)_y;
   tks_HAL->mouse.absx = _x; // [25Jan2025]
   tks_HAL->mouse.absy = _y; // [25Jan2025]
}

sSI _Mouse::_getX(void) {
   return tks_HAL->mouse.absx;
}

sSI _Mouse::_getY(void) {
   return tks_HAL->mouse.absy;
}

sSI _Mouse::_getDx(void) {
   return tks_HAL->mouse.dx;
}

sSI _Mouse::_getDy(void) {
   return tks_HAL->mouse.dy;
}

sSI _Mouse::_getButtons(void) {
   return tks_HAL->mouse.cstate;
}

sSI _Mouse::_getChangedButtons(void) {
   return tks_HAL->mouse.nstate;
}

sSI _Mouse::_isGrabbed(void) {
   return tks_HAL->mouse.grabbed;
}

void _Mouse::_showPointer(sSI _enabled) {
   tks_HAL->showCursor(_enabled);
}

void _Mouse::_unproject(YAC_Object *_rnear, YAC_Object *_rfar) {
   if(YAC_VALID(_rnear))
   {
      if(YAC_VALID(_rfar))
      {
         if(_rnear->yacArrayGetMaxElements() >= 3)
         {
            if(_rfar->yacArrayGetMaxElements() >= 3)
            {
               sF32 vnear[3];
               sF32 vfar[3];

               sF32 x = (sF32) tks_HAL->mouse.absx;
               sF32 y = (sF32) tks_HAL->mouse.absy;

               tkopengl_shared_resolve();
               tkopengl_shared->_unproject(x, y,
                                           &vnear[0], &vnear[1], &vnear[2],
                                           &vfar[0], &vfar[1], &vfar[2]
                                           );

               YAC_Value v;

               v.initFloat(vnear[0]);
               _rnear->yacArraySet(NULL, 0, &v);
               v.initFloat(vnear[1]);
               _rnear->yacArraySet(NULL, 1, &v);
               v.initFloat(vnear[2]);
               _rnear->yacArraySet(NULL, 2, &v);

               v.initFloat(vfar[0]);
               _rfar->yacArraySet(NULL, 0, &v);
               v.initFloat(vfar[1]);
               _rfar->yacArraySet(NULL, 1, &v);
               v.initFloat(vfar[2]);
               _rfar->yacArraySet(NULL, 2, &v);

#if 0
               // GLdouble mvmatrix[4*4];
               // GLdouble prmatrix[4*4];
               // ::glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
               // ::glGetDoublev(GL_PROJECTION_MATRIX, prmatrix);
               // GLdouble x=(GLdouble)tks_HAL->mouse.absx;
               // GLdouble y=(GLdouble)tks_HAL->view_sy-tks_HAL->mouse.absy; //tks_HAL->mouse.last_ev_motion_y;
               // GLdouble z=0.0;
               // GLint view[4];
               // ::glGetIntegerv(GL_VIEWPORT, view);
               // GLdouble rx,ry,rz;
               // ::gluUnProject(x,y,z, mvmatrix, prmatrix, view, &rx,&ry,&rz);
               // YAC_Value v;
               // v.initFloat((sF32)rx);
               // _rnear->yacArraySet(NULL, 0, &v);
               // v.initFloat((sF32)ry);
               // _rnear->yacArraySet(NULL, 1, &v);
               // v.initFloat((sF32)rz);
               // _rnear->yacArraySet(NULL, 2, &v);
               // z=1.0;
               // ::gluUnProject(x,y,z, mvmatrix, prmatrix, view, &rx,&ry,&rz);
               // v.initFloat((sF32)rx);
               // _rfar->yacArraySet(NULL, 0, &v);
               // v.initFloat((sF32)ry);
               // _rfar->yacArraySet(NULL, 1, &v);
               // v.initFloat((sF32)rz);
               // _rfar->yacArraySet(NULL, 2, &v);
#endif
            }
         }
      }
   }
}

void _Mouse::_setPointerMode(sSI _m) {
   switch(_m)
   {
   case TKS_MOUSE_POINTER_ENTER:
   case TKS_MOUSE_POINTER_MOVE:
      tks_HAL->mouse.auto_hide_timeout = tks_HAL->mouse.def_auto_hide_timeout;
      tks_HAL->mouse.pointer_mode      = _m;
      break;

   case TKS_MOUSE_POINTER_MANUAL:
      tks_HAL->mouse.auto_hide_timeout = 0;
      tks_HAL->mouse.pointer_mode      = TKS_MOUSE_POINTER_MANUAL;
      break;
   }
}

sSI _Mouse::_getPointerMode(void) {
   return tks_HAL->mouse.pointer_mode;
}

