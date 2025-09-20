/// Viewport.h
///
/// (c) 2001-2023 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
#ifndef __TKSDL_VIEWPORT_H__
#define __TKSDL_VIEWPORT_H__

YG("hal1");


/* @class Viewport,Object
Static wrapper class for a couple of viewport related functions

@see SDL 
*/
YCS class _Viewport : public YAC_Object {
public:
	 _Viewport(void);
	 ~_Viewport();

	 YAC(_Viewport);

#if 0
    // run test code
    //  (note) added for macOS sdl12-compat debugging
    void RunTest (void);
#endif


    /* @method openWindow,int w, int h:boolean
    Open desktop window.

    This is a wrapper for the SDL_InitSubSystem(), SDL_GetVideoInfo(), SDL_GL_SetAttribute() and SDL_SetVideoMode() native functions.

    @arg w Window width
    @arg h Window height
    @return true if the window has been opened, false if something went terribly wrong.

    @see openScreen
    @see close
    */
	 YM sSI  _openWindow          (sSI _sx, sSI _sy);


    /* @method openScreen,int w,int h,int z:boolean
    Open fullscreen window

    This is a wrapper for the SDL_InitSubSystem(), SDL_GetVideoInfo(), SDL_GL_SetAttribute() and SDL_SetVideoMode() native functions.

    @arg w Screen width
    @arg h Screen height
    @arg z 16,24,32 bits
    @return true if the screen has been opened, false if something went terribly wrong.

    @see openWindow
    @see close
    */
	 YM sSI  _openScreen          (sSI, sSI, sSI);


    /* @method toggleFullScreen
    Toggle fullscreen mode

    @see setScreenResolution

    */
	 YM void _toggleFullScreen    (void);


    /* @method isFullScreen

    @see toggleFullScreen

    */
	 YM sBool _isFullScreen       (void);


    /* @method close
    Close desktop or fullscreen window
    */
	 YM void _close               (void);


    /* @method beginPaint
    Begin painting

    Set viewport cliprect.
    Reset texture and modelview matrices to identity.
    */
	 YM void _beginPaint          (void);


    /* @method endPaint
    Flush GL commandlist and swap buffers
    */
	 YM void _endPaint            (void);


    /* @method getWidth:int
    Query window/screen width
    */
	 YM sSI  _getWidth            (void);


    /* @method getHeight:int
    Query window/screen height
    */
	 YM sSI  _getHeight           (void);


    /* @method getDepth:int
    Query color depth

    @return Number of pixels (15,16,24,32)
    */
	 YM sSI  _getDepth            (void);


    /* @method setCaption:String
    Set window title
    */
	 YM void _setCaption          (YAC_String *);


    /* @method waitVBlank
    Wait for vertical retrace interrupt

    @deprecated Not the optimal way to achieve absolutely smooth scrolling with OpenGL. Use §swapInterval() instead.

    @see swapInterval
    */
	 YM void _waitVBlank          (void);


    /* @method getFrequency:int
    Query monitor frequency
    */
	 YM sSI  _getFrequency        (void);


    /* @method setFlags,int flags
    Set flags for openWindow() resp. openScreen() call

    This method should only be called prior to opening a window or screen.

    @arg flags A bitmask of §SDL_RESIZABLE and §SDL_NOFRAME.
    */
	 YM void _setFlags            (sUI);


    /* @method getFlags:int
    Query openWindow() resp. openScreen() flags
    */
	 YM sUI  _getFlags            (void);


    /* @method resizeWindow,int w,int h:boolean
    Resize desktop window
    */
	 YM sSI  _resizeWindow        (sSI _sx, sSI _sy);


    /* @method needRedraw
    If the repaint strategy is §REDRAW_DIRTY, this will cause a display redraw.
    */
	 YM void _needRedraw          (void);


    /* @method setScreenResolution,int w,int h,int z
    Set fullscreen resolution and depth for next toggleFullScreen() call
    */
	 YM void _setScreenResolution (sSI, sSI, sSI);


    /* @method getDoubleBuffer:boolean
    Query whether a double buffered OpenGL surface shall be allocated
    */
    YM sSI  _getDoubleBuffer     (void);


    /* @method setDoubleBuffer,boolean bEnabled
    Hint that a double buffered OpenGL surface shall be allocated

    This method should only be called prior to opening a window or screen.
    */
    YM void _setDoubleBuffer     (sSI _enabled);


    /* @method swapInterval,int interval
    A wrapper for the wglSwapIntervalEXT() resp. glXSwapIntervalSGI() native functions

    This function can effectively be used to disable or enable vertical blank synchronisation and it is, to my knowledge,
    the best way to achieve absolutely smooth vsync'd OpenGL graphics.

    In practice, the ATI/NVidia GL drivers act a bit different than how this GL extension was originally specified (different in a good sense).

    @arg interval 0=disable vsync, 1=enable vsync, 2..n only swap buffers every 2..nth frame.
    */
    YM sSI  _swapInterval        (sSI _interval); // requires GLX_SGI_swap_control or WGL_EXT_swap_control extension


    /* @method getMultisampleBuffers:int
    Query number of GL multisample buffers
    */
    YM sSI  _getMultisampleBuffers (void);


    /* @method setMultisampleBuffers,int num
    Set number of GL multisample buffers.

    This method should only be called prior to opening a window or screen.
    */
    YM void _setMultisampleBuffers (sSI _buffers);


    /* @method getMultisampleSamples:int
    Query number of GL multisample samples
    */
    YM sSI  _getMultisampleSamples (void);


    /* @method setMultisampleSamples,int num
    Set number of GL multisample samples

    This method should only be called prior to opening a window or screen.
    */
    YM void _setMultisampleSamples (sSI _samples);


    /* @method setStencilBits,int num
    Set number of stencil buffer bits

    This method should only be called prior to opening a window or screen.
    */
    YM void _setStencilBits (sSI _stencilBits);

    
    /* @method interruptScreenSaver
    Try to interrupt the screensaver, usually by sending a synthetic event to the root/desktop window.
    */
    YM void _interruptScreenSaver (void);


    /* @method setEnableRendering,boolean bEnabled
       Mainly for debugging purposes / CPU load measurement
    */
    YM void _setEnableRedraw (sSI _bEnabled);

    
    /* @method getDesktopWidth:int
       Query desktop width
    */
    YM sSI _getDesktopWidth (void);


    /* @method getDesktopHeight:int
       Query desktop height
    */
    YM sSI _getDesktopHeight (void);


    /* @method setEnableGLError,boolean bEnabled
       Enable/disable GL error reporting (default=false).

       Please notice that with some GL drivers (noticably the latest Win32 GF 9800 GT driver on Vista/32bit), a simple call to glGetError()
       will take 16ms at a ~60 FPS refresh rate after ~60 frames / glFlush() / SwapBuffers() calls have been made.

       This didn't happen with older driver releases although there are internet discussions about this going back to 2006. FIX IT, NVIDIA!!

       GL error reporting can also be enabled using the TKSDL_GLERROR environment variable (0=disable, 1=enable).

       @arg bEnabled true=enable glGetError() calls after each GL call outside of glBegin..glEnd. false=never call glGetError.
    */
    YM void _setEnableGLError (sSI _bEnabled);

    /* @method getEnableGLError:boolean
       Query whether GL error reporting (see §setEnableGLError) is currently enabled.
       
       @return Status of GL error tracer
    */
    YM sSI _getEnableGLError (void);

    YM void _pushGLContext (void);
    YM void _popGLContext (void);

    
    /* @method getDpi:int
       Query dots-per-inch resolution for current window/screen.
       Fallback to 96 if API is not supported (requires Windows 10+)
       
       @return dpi
    */
    YM sUI _getDpi (void);


    /* @method setGeometry,int x,int y,int w,int h
       Move (desktop) window to another screen position and (optionally) resize it.

       This is a platform specific call that is currently only implemented in the Windows port of tksdl.

       @arg x Screen position X
       @arg y Screen position Y
       @arg w Width or 0 (== keep size) (must include border/title bar size)
       @arg h Height or 0 (== keep size)
    */
    YM void _setGeometry (sSI _x, sSI _y, sUI _w, sUI _h);

    /* @method getPositionX:int
       Query (desktop) window position x

       This is a platform specific call that is currently only implemented in the Windows port of tksdl.
    */
    YM sSI _getPositionX (void);

    /* @method getPositionY:int
       Query (desktop) window position y

       This is a platform specific call that is currently only implemented in the Windows port of tksdl.
    */
    YM sSI _getPositionY (void);

    /* @method getSizeX:int
       Query (desktop) window width (including borders / title bar)

       This is a platform specific call that is currently only implemented in the Windows port of tksdl.
    */
    YM sSI _getSizeX (void);

    /* @method getSizeY:int
       Query (desktop) window height (including borders / title bar)

       This is a platform specific call that is currently only implemented in the Windows port of tksdl.
    */
    YM sSI _getSizeY (void);

    /* @method setMinimumSize,int w,int h
       Set minimum view size.

       @arg w Width
       @arg h Height
    */
    YM void _setMinimumSize (sUI _w, sUI _h);
};


#endif // __TKSDL_VIEWPORT_H__

