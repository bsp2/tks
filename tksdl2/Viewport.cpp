/// Viewport.cpp
///
/// (c) 2001-2023 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///

#include <stdio.h>
#include <stdlib.h>

#define YAC_PRINTF defined
#include <yac.h>
extern YAC_Host *yac_host;
extern sUI clid_Joystick;

#include "inc_sdl.h"

#include "HAL.h"
#include "Viewport.h"
#include "Mouse.h"


_Viewport::_Viewport(void) {
}

_Viewport::~_Viewport() {
   // yac_host->printf("[dbg] Viewport::~Viewport ENTER\n");
	tks_HAL->closeView();
   // yac_host->printf("[dbg] Viewport::~Viewport LEAVE\n");
}

#if 0
#ifdef YAC_MACOS
extern int testgl_main(int argc, char *argv[]);
void _Viewport::RunTest(void) {
   int argc = 1;
   char *argv[1] = {(char*)"testgl"};
   yac_host->printf("[trc] Viewport::RunTest: call testgl_main()\n");
   (void)testgl_main(argc, argv);
   yac_host->printf("[trc] Viewport::RunTest: testgl_main() returned\n");
}
#else
void _Viewport::RunTest(void) { }
#endif // YAC_MACOS
#endif

void _Viewport::_toggleFullScreen(void) {
	tks_HAL->toggleFullScreen();
}

sBool _Viewport::_isFullScreen(void) {
	return tks_HAL->b_fullscreen;
}

sSI _Viewport::_openWindow(sSI _sx, sSI _sy) {
	return tks_HAL->openWindow(_sx,_sy);
}

sSI _Viewport::_resizeWindow(sSI _sx, sSI _sy) {
	if(!(tks_HAL->view_flags&_HAL::VP_FULLSCREEN))
		if(tks_HAL->view_flags&_HAL::VP_OPENED)
			if((_sx>0) && (_sy>0))
         {
				return tks_HAL->openView(_sx, _sy, 0,0, 1);
         }
	return 0;
}

void _Viewport::_setFlags(sUI _fl) {
	tks_HAL->sdl1_flags =_fl & (16u/*SDL_RESIZABLE*/ | 32u/*SDL_NOFRAME*/);
}

sUI _Viewport::_getFlags(void) {
	return tks_HAL->sdl1_flags;
}

sSI _Viewport::_openScreen(sSI _sx, sSI _sy, sSI _z) {
	return tks_HAL->openScreen(_sx, _sy, _z);
}

void _Viewport::_close(void) {
    tks_HAL->closeView();
}

void _Viewport::_waitVBlank(void) {
	tks_HAL->waitVBlank();
}

sSI _Viewport::_getFrequency(void) {
	return tks_HAL->getMonitorFrequency();
}

void _Viewport::_beginPaint(void) {
	tks_HAL->beginPaint();
}

void _Viewport::_endPaint(void) {
	tks_HAL->endPaint();
}

sSI _Viewport::_getWidth(void) {
	return tks_HAL->view_sx;
}

sSI _Viewport::_getHeight(void) {
	return tks_HAL->view_sy;
}

sSI _Viewport::_getDepth(void) {
	return tks_HAL->view_z;
}

void _Viewport::_setCaption(YAC_String *_sw) {
   tks_HAL->setCaption(_sw);
}

void _Viewport::_needRedraw(void) {
	tks_HAL->b_need_redraw=1;
   tks_HAL->sendRedrawEvent();
}

void _Viewport::_setScreenResolution(sSI _sx, sSI _sy, sSI _bits) {
    if(_sx>=0 && _sy>=0 && _bits>0)
    {
       if(0 == _sx)
       {
          SDL_DisplayMode DM;
          int r = SDL_GetCurrentDisplayMode(0, &DM);
          (void)r;
          _sx = (sSI)DM.w;
       }

       if(0 == _sy)
       {
          SDL_DisplayMode DM;
          int r = SDL_GetCurrentDisplayMode(0, &DM);
          (void)r;
          _sy = (sSI)DM.h;
       }

        tks_HAL->fs_sx = _sx;
        tks_HAL->fs_sy = _sy;

        if(_bits!=16 && _bits!=32)
        {
            if(_bits < 16)
            {
                _bits = 16;
            }
            else
            {
                _bits = 32;
            }
        }
        tks_HAL->fs_z = _bits;
    }
}

sSI _Viewport::_getDoubleBuffer (void) {
   return tks_HAL->b_double_buffer;
}

void _Viewport::_setDoubleBuffer (sSI _enabled) {
   tks_HAL->b_double_buffer = (_enabled!=0);
}


#ifdef YAC_WIN32
typedef void (APIENTRY *PFNWGLEXTSWAPINTERVALPROC) (int);

sSI _Viewport::_swapInterval(sSI _interval) {
   
   static PFNWGLEXTSWAPINTERVALPROC wglSwapIntervalEXT = NULL;
  
   wglSwapIntervalEXT = (PFNWGLEXTSWAPINTERVALPROC) wglGetProcAddress("wglSwapIntervalEXT");

   // Invoke extension function if present
   if(NULL != wglSwapIntervalEXT)
   {
      wglSwapIntervalEXT(_interval);

      // OK
      return 1;
   }
   else
   {
      // Failed
      return 0;
   }
}

#elif defined(DX_GLSWAPINTERVALEXT)

#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glxext.h>

typedef int (*GLXSWAPINTERVALSGIPROC) (int);

sSI _Viewport::_swapInterval(sSI _interval) {
/*
Some info how this could be done on Linux/X11:

  - For NVidia cards:
       "Set the __GL_SYNC_TO_VBLANK environment variable to a non-zero value.
       This is described in the NVIDIA README.txt that is installed with the
       driver."

  - glXSwapIntervalSGI() / GLX_SGI_swap_control extension
     see http://www.opengl.org/registry/specs/SGI/swap_control.txt
*/
#if 1 || !defined(GLX_SGI_swap_control)
   /* (note) force this codepath to improve compatibility with virtualbox 3d accel */

   static GLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = NULL;
  
   glXSwapIntervalSGI = (GLXSWAPINTERVALSGIPROC) glXGetProcAddress((const GLubyte*)"GLX_SGI_swap_control");

   // Invoke extension function if present
   if(NULL != glXSwapIntervalSGI)
   {
      // TODO: check return value.
      //       Spec says that interval=0 is not supported but I hope that in practice
      //       the drivers use 0 to disable vsync (like wglSwapIntervalEXT())
      glXSwapIntervalSGI(_interval);

      // OK
      return 1;
   }
   else
   {
      // Failed
      return 0;
   }
#else

   // May work even if glXGetProcAddress returns NULL (!!?, seen with NVidia Linux driver)
   glXSwapIntervalSGI(_interval);

   return 1;

#endif // defined(DX_GLSWAPINTERVALEXT)   // GLX_SGI_swap_control
}

#else

sSI _Viewport::_swapInterval(sSI _interval) {
   // fallback implementation (Poky Linux, macOS)

   if((sUI)_interval > 4)
   {
      _interval = 4;
   }

   tks_HAL->swap_interval = _interval;

#ifdef DX_DNX_POKY
   dnx_poky_view_swapinterval(_interval);
#else
   if(yac_host->yacGetDebugLevel() >= 2)
      yac_host->printf("[trc] call SDL_GL_SetSwapInterval(%d)\n", _interval);
   SDL_GL_SetSwapInterval(_interval);
#endif // DX_DNX_POKY

   ////puts("[~~~] Viewport::swapInterval: STUB\n");

   return 0;
}


#endif // YAC_WIN32


sSI _Viewport::_getMultisampleBuffers(void) {
   return tks_HAL->multisample_buffers;
}

void _Viewport::_setMultisampleBuffers(sSI _buffers) {
   if(_buffers >= 0)
   {
      tks_HAL->multisample_buffers = _buffers;
   }
}

sSI _Viewport::_getMultisampleSamples(void) {
   return tks_HAL->multisample_samples;
}

void _Viewport::_setMultisampleSamples(sSI _samples) {
   if(_samples >= 0)
   {
      tks_HAL->multisample_samples = _samples;
   }
}

void _Viewport::_setStencilBits(sSI _stencilBits) {
   if(_stencilBits >= 0)
   {
      tks_HAL->stencil_bits = _stencilBits;
   }
}

void _Viewport::_interruptScreenSaver(void) {
   //
   // Send synthetic keyboard event to root/desktop window
   //
#ifdef YAC_WIN32
   ::PostMessage(HWND_TOPMOST, 
                 WM_KEYDOWN, 
                 0x0020/*wParam=keycode*/, 
                 (0x0020<<16) | (1<<31) /*lKeyData*/
                 );
#endif // YAC_WIN32

#ifdef YAC_LINUX
   //tks_HAL->interruptScreenSaverX11();
#endif // YAC_LINUX
}

void _Viewport::_setEnableRedraw(sSI _bEnabled) {
   tks_HAL->b_allow_redraw = _bEnabled;
}

sSI _Viewport::_getDesktopWidth(void) {
   return tks_HAL->desktop_sx;
}

sSI _Viewport::_getDesktopHeight(void) {
   return tks_HAL->desktop_sy;
}

void _Viewport::_setEnableGLError(sSI _bEnabled) {
   tks_HAL->b_glerror = _bEnabled;
}

sSI _Viewport::_getEnableGLError(void) {
   return tks_HAL->b_glerror;
}

void _Viewport::_pushGLContext(void) {
   tks_HAL->pushGLContext();
}

void _Viewport::_popGLContext(void) {
   tks_HAL->popGLContext();
}

sUI _Viewport::_getDpi(void) {
   return tks_HAL->getDpi();
}

void _Viewport::_setGeometry(sSI _x, sSI _y, sUI _w, sUI _h) {
   return tks_HAL->setWindowPositionAndSize(_x, _y, _w, _h);
}

sSI _Viewport::_getPositionX(void) {
   return tks_HAL->getWindowPositionX();
}

sSI _Viewport::_getPositionY(void) {
   return tks_HAL->getWindowPositionY();
}

sSI _Viewport::_getSizeX(void) {
   return tks_HAL->getWindowSizeX();
}

sSI _Viewport::_getSizeY(void) {
   return tks_HAL->getWindowSizeY();
}

void _Viewport::_setMinimumSize(sUI _w, sUI _h) {
   tks_HAL->min_w = _w;
   tks_HAL->min_h = _h;
   if(NULL != tks_HAL->sdl_window)
   {
      ::SDL_SetWindowMinimumSize(tks_HAL->sdl_window, _w, _h);
   }
}
