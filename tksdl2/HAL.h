/// tkshal.h
///
/// (c) 2001-2025 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
#ifndef TKSDL_HAL_H__
#define TKSDL_HAL_H__

YG("hal");


// ---- "scrap" is actually an example code by SDL author Sam Lantinga which demonstrates
// ---- how to access the X11/Win32/QNX clipboard. 
// ---- define this constant if you want to enable "scrap" support.

#ifdef YAC_WIN32
   #define TKSDL_WIN32_CLIPBOARD defined
   ////#define TKSDL_SCRAP_CLIPBOARD defined
#else
  #ifdef YAC_MACOS
     #define TKSDL_MACOS_CLIPBOARD defined
  #else
     #if defined(YAC_LINUX) && !defined(DX_DNX_POKY)
        #define TKSDL_SCRAP_CLIPBOARD defined
     #else
        #define TKSDL_STUB_CLIPBOARD defined
     #endif
  #endif
#endif


enum __tksapplicationsignalids {
	TKS_SIG_HALSIGNAL=0,
	TKS_SIG_HALKEYBOARD,
	TKS_SIG_HALMOUSE,
	TKS_SIG_HALJOYAXISMOTION,
	TKS_SIG_HALJOYBUTTON,
	TKS_SIG_HALJOYHATMOTION,
	TKS_SIG_HALJOYBALLMOTION,
	TKS_SIG_HALDRAW,
	TKS_SIG_HALTICK,
	TKS_SIG_HALCLOSE,
	TKS_SIG_HALEVENT,
	TKS_SIG_HALOPEN,
	TKS_SIG_HALRESIZE,
   TKS_SIG_HALEXPOSE,
   TKS_SIG_HALAPPLICATIONACTIVE,
   TKS_SIG_HALMOUSEFOCUSACTIVE,
   TKS_SIG_HALIDLE,
   TKS_SIG_HALINPUTFOCUSACTIVE,
   TKS_SIG_HALDROPFILES,
   TKS_SIG_HALTIMER,
   TKS_SIG_HALCREATEOTHERWINDOW,  // see installWindowCreateAndDestroyHook()
   TKS_SIG_HALFOCUSOTHERWINDOW,
   TKS_SIG_HALMOVEWINDOW,
   TKS_SIG_HALDRAINEVENTQUEUE,

	TKS_HAL_NUMSIGNALS
};


enum __tksmouseflags {
    TKS_MOUSE_LBUTTON     =(1<<0),
    TKS_MOUSE_RBUTTON     =(1<<1),
    TKS_MOUSE_MBUTTON     =(1<<2),
    TKS_MOUSE_WHEELUP     =(1<<3),
    TKS_MOUSE_WHEELDOWN   =(1<<4),

    TKS_MOUSE_PAD
};

enum __mousepointermodes {
	TKS_MOUSE_POINTER_ENTER  =0,
	TKS_MOUSE_POINTER_MOVE   =1,
	TKS_MOUSE_POINTER_MANUAL =2
};

enum __tksredrawflags {
    TKS_REDRAW_ALL=0,
    TKS_REDRAW_DIRTY=1,

    TKS_REDRAW_PAD
};

enum __tkssdlevents {
	TKS_EVENT_USER=0,  // see sendEvent()
   TKS_EVENT_TIMER,
   TKS_EVENT_REDRAW,  // dummy event to break out of waitEvent call

	TKS_EVENT_PAD
};

#define _HAL _SDL


/* @class SDL,Object
Static wrapper class for the SDL library.

Please notice that some functionality is placed in separate classes (§Mouse, §Viewport, §AudioDevice, §Cursor, §FPS)

This class uses the following signals/callbacks:

- onKeyboard(Key k) - Called when a key was pressed or released
- onMouse(int absx, int absy, int currbuttonstate, int changedbuttonstate) - Called when a mouse event occured
- onJoyAxisMotion(int dev, int axis, int val)
- onJoyButton(int dev, int button, int pressed)
- onJoyHatMotion(int dev, int hat, int state)
- onJoyBallMotion(int dev, int hat, int dx, int dy)
- onDraw() - Called when window/view needs to be redrawn
- onOpen() - Called after window/view has been opened
- onClose() - Called when window/view is about to be closed. By returning false in the callback function, the window/view can be prevented from closing (e.g. if the user has clicked the window close button)
- onResize() - Called after window/view has been resized (onOpen() is called immediately after)
- onEvent(Event ev) - Called when user event has been received (see §sendEvent())
- onExpose() - Called when (parts) of the window/view have become visible
- onApplicationActive(boolean bActive) - Called when application window/view becomes active
- onMouseFocusActive(boolean bActive)
- onInputFocusActive(boolean bActive)
- onIdle() - **Deprecated**, only called regularly when event polling is enabled (see §setEventPolling())
- onDropFiles() - Called when one or more files were dragged and dropped onto the window/view [win32 only]
- onTimer() - Called from timer thread when interval has been configured using setTimerInterval().

*/

YCS class _SDL : public YAC_Object {
public:


/* @constant MOUSE_LBUTTON,int
 */
//#define MOUSE_LBUTTON                    YCI 1

   
/* @constant MOUSE_RBUTTON,int
 */
//#define MOUSE_RBUTTON                    YCI 2

   
/* @constant MOUSE_MBUTTON,int
 */
//#define MOUSE_MBUTTON                    YCI 4

   
/* @constant MOUSE_WHEELUP,int
 */
//#define MOUSE_WHEELUP                    YCI 8

   
/* @constant MOUSE_WHEELDOWN,int
 */
//#define MOUSE_WHEELDOWN                  YCI 16

   
/* @constant MOUSE_POINTER_ENTER,int
 */
//#define MOUSE_POINTER_ENTER              YCI 0

   
   
/* @constant MOUSE_POINTER_MOVE,int
 */
//#define MOUSE_POINTER_MOVE               YCI 1


/* @constant MOUSE_POINTER_MANUAL,int
 */
//#define MOUSE_POINTER_MANUAL             YCI 2


/* @constant REDRAW_ALL,int
 */
//#define REDRAW_ALL                       YCI 0


/* @constant REDRAW_DIRTY,int
 */
//#define REDRAW_DIRTY                     YCI 1


/* @constant DEFAULT_KEYBOARD_REPEAT_DELAY,int
 */
//#define DEFAULT_KEYBOARD_REPEAT_DELAY    YCI 500


/* @constant DEFAULT_KEYBOARD_REPEAT_INTERVAL,int
 */
//#define DEFAULT_KEYBOARD_REPEAT_INTERVAL YCI 30


/* @constant SDL_RESIZABLE,int
 */
//#define SDL_RESIZABLE                    YCI 0x10


/* @constant SDL_NOFRAME,int
 */
//#define SDL_NOFRAME                      YCI 0x20


	sBool        b_running;
	sBool        b_graphics_active;
	sBool        b_event_polling;
	sBool        b_fullscreen;
	sBool        b_need_redraw;
	sBool        b_redrawing;
   sBool        b_allow_redraw;
   sBool        b_glerror;   // 1=check glGetError() after each GL call  
   sBool        b_glcore;    // 1=GL 4.x core profile (no glBegin/glEnd, ..)
	sU8          redraw_flag;
	sU32         sdl1_flags; // SDL1-style Viewport flags (SDL_RESIZABLE | SDL_NOFRAME)
   sBool        b_onclose_pending;
   sUI          swap_interval;

   YAC_MutexHandle mtx_event_queued;
   sBool b_timer_event_queued;
   sBool b_redraw_event_queued;
   sBool b_reset_queued_events;

   SDL_Window    *sdl_window;
   SDL_GLContext  sdl_glcontext;

	YAC_FunctionHandle signal_funs[TKS_HAL_NUMSIGNALS];

   sBool b_double_buffer; // 1=create double buffered GL context

   sSI          multisample_buffers; // Number of buffers (default = 0)
   sSI          multisample_samples; // Number of samples (default = 0)

   sSI          stencil_bits; // Number of stencil bits (default = 0)
    
	YAC_String   window_title;
	sU16         view_flags;
	sU16         view_sx;
	sU16         view_sy;
	sU8          view_z;
	sU16         win_sx;
	sU16         win_sy;
	sU16         fs_sx;
	sU16         fs_sy;
	sU16         fs_z;

   sSI          desktop_sx;
   sSI          desktop_sy;

   sUI          min_w;
   sUI          min_h;

   sUI          now;
   sUI          last_time;
   struct _fps {
      sF32 tick_interval; // default interval of onTick(), also affects fps.precision (onDraw)
      sU32 tick_buffer;   // max. number of tick() calls in a row
      sU32 num_frames_rendered;
      sU32 total_time;
      sU32 second_time;
      sU32 last_secondtime;
      sU32 second_frames_rendered;
      sF32 real_fps;
      sU32 last_frametime;
      sF32 average_fps;
      sF32 current_fps;
      sF32 precision;
      sUI  limit_fps; // ---- limit redraw frequency to e.g. 25fps, 0=no limit ----
      sBool b_fullframe; // hack to work around glitchy timers. forces precision=1.0
      sUI  last_idletime;
   } fps;
   sU32 max_frame_deltatime;

   sUI timer_interval; // in milliseconds. 0=disable timer

   struct _mouse {
      sS16      absx;
      sS16      absy;
      sS16      dx;
      sS16      dy;
      sU16      cstate;
      sU16      nstate;
      sBool     grabbed;
      sSI       grab_start_x;
      sSI       grab_start_y;
      sBool     b_grab_changed; // special treatment for first mouse-event after mouse grab/ungrab
      sBool     b_warp_changed; // special treatment for first mouse-event after mouse warp
      sSI       warp_x;  // -1=no warp queued
      sSI       warp_y;
      sU8       show;
      sU8       pointer_mode;
      sU32      auto_hide_timeout;
      sU32      def_auto_hide_timeout;
      sBool     b_touch_input;  // 1=use win8+ WM_POINTER* messages (for simulating LBUTTON events with tablet devices)
   } mouse;

   struct _joysticks {
      sBool enabled;
      sS32  num;
      void *elements; // TKSJoystick object array
   } joysticks;
    
   enum __tksviewportflags {
      VP_OPENED    = (1<<0),
      VP_FULLSCREEN= (1<<1),
      
      VP_PAD
   };
    
   struct __audio {
      sF32 dsp_frequency;
   } audio;

   sSI queued_window_move_px;
   sSI queued_window_move_py;
    
#ifdef YAC_WIN32
    DWORD dw_monitorfreq;
    LPDIRECTDRAW lpDD;
#endif


#ifdef TKSDL_SCRAP_CLIPBOARD
    YAC_String scrap_string;
#endif

    sBool b_window_visible;
    YAC_String viewport_caption;

    sSI pending_resize_ms;
    sSI pending_resize_w;
    sSI pending_resize_h;
    sUI ignore_resize_timeout;  // ms, workaround for WINDOWEVENT_RESIZED after switching to fullscreen

    void *sdl2_gl_context;

    sBool b_key_repeat;
    sU32  last_vmod;
    sU32  last_vkey;

    YAC_StringArray *drop_file_names;

    sUI cursor_zoom;  // e.g. for Linux Debian/Ubuntu 4k screens

  protected:
    void  updateFPSAndLimit (void);
	 sBool loopDelay         (void);

public:
	 _HAL(void);
	 ~_HAL();

	 YAC(_HAL);

	 void  YAC_VCALL yacGetSignalStringList (YAC_String *);
	 void  YAC_VCALL yacRegisterSignal (sUI _sigid, YAC_FunctionHandle _fun);

	 void  setArgs (int _argc, char **_argv);
	 void  tick (void);
	 void  draw (void);

	 void  showCursor (sBool);

    void setCaption (YAC_String *_s);

	 void  beginPaint (void);
	 void  endPaint (void);

    void  pushGLContext (void);
    void  popGLContext (void);
    void  recreateGLContext (void);

	 sBool callOnClose             (sBool _bAllowScriptCbk);
	 void  callOnOpen              (void);
 	 void  callOnResize            (void);
 	 void  callOnExpose            (void);
    void  callOnApplicationActive (sSI _gain);
    void  callOnMouseFocusActive  (sSI _gain);
    void  callOnInputFocusActive  (sSI _gain);
	 void  callOnJoyAxisMotion     (sSI, sSI, sSI);
	 void  callOnJoyButton         (sSI, sSI, sSI);
	 void  callOnJoyHatMotion      (sSI, sSI, sSI);
	 void  callOnJoyBallMotion     (sSI, sSI, sSI, sSI);
    void  callOnIdle              (void);
	 void  callOnKeyboard          (sUI _vkey, sUI _vmod, sUI _unicode);
	 void  callOnMouse             (sSI _mx, sSI _my, sSI _cstate, sSI _nstate);
    void  callOnTimer             (void);
    void  callOnCreateOtherWindow (sUI _hwnd);
    void  callOnFocusOtherWindow  (sUI _hwnd);
    void  callOnMoveWindow        (sSI _x, sSI _y);
	 void  callOnDrainEventQueue   (void);

	 void  initFPS (void);

	 void  resetMouse (void);

	 // ---- joystick input
	 void  openJoysticks (void);
	 void  closeJoysticks (void);

	 // ---- viewport
	 sBool openView (sU16 _sx, sU16 _sy, sU8 _z, sBool _bFullscreen, sBool _bResize);
	 sBool openWindow (sS32 _sx, sS32 _sy);
	 sBool openScreen (sS32 _sx, sS32 _sy, sS32 _z);
	 void  viewOpened (sBool _bResize);
	 void  closeView (void);
	 void  useWindow (sU16 _sx, sU16 _sy);
	 sBool toggleFullScreen (void);

    void loadGLDefaults (void);

	 // ---- vertical sync
	 void  waitVBlank (void);
	 sUI   getMonitorFrequency (void);

    // ---- macOS local key to raw key code map
    const sUI *local_to_raw_key_table;
    sBool setLocalToRawKeyTable (const sUI *_tbl);

    void sendTimerEvent (void);

    void sendRedrawEvent (void);

    void startTimerThread (void);
    void stopTimerThread (void);

#ifdef YAC_LINUX
    void interruptScreenSaverX11 (void);
#endif

    sUI getDpi (void);

    void setWindowPositionAndSize (sSI _x, sSI _y, sUI _w, sUI _h);

    // returns top/left screen position of client area
    sSI getWindowPositionX (void);
    sSI getWindowPositionY (void);

    // returns window size (including borders/title bar)
    sSI getWindowSizeX (void);
    sSI getWindowSizeY (void);

    sUI getCursorZoom (void) const;

public:
	 // ------------ exported methods ------------

   /* @method eventLoop
   Enter SDL event loop. 
   This method does not return until the window is closed resp. §exitEventLoop is called.

   @see exitEventLoop
   */
	 YM void _eventLoop            (void);


    /* @method exitEventLoop
    Exit SDL event loop.

    This causes the §eventLoop method to return.

    @see eventLoop
    */
	 YM void _exitEventLoop        (void);

    /* process single event loop (while event loop is not running, yet)
       should not be necessary to call directly, added for debugging purposes
    */
	YM sBool processSDLEvent (sBool _poll);
	
	// added for macOS, seems to improve mouseclick issue after startup
    YM void drainEventQueue (void);

    /* @method getTicks:int
    Return number of ticks (millisec) since app. start

    @deprecated Use the §milliSeconds() core function instead
    @see milliSeconds
    */
	 YM sUI  _getTicks             (void);


    /* @method triggerEvent,String ev
    Immediately send user event

    @deprecated Call your event handler directly instead
    @see sendEvent
    */
	 YM void _triggerEvent         (YAC_Object *);


    /* @method setRedrawFlag,int flags
    Control redraw behaviour

    @arg flags §REDRAW_ALL = redraw everytime §REDRAW_DIRTY = only redraw if §Viewport.needRedraw() was called
    */
	 YM void _setRedrawFlag        (sSI);


    /* @method getRedrawFlag:int
    Query redraw behaviour
    */
	 YM sSI  _getRedrawFlag        (void);


    /* @method delay,int millisec
    Suspend thread for the given number of millisec

    @deprecated Use §TKS.sleep() instead
    */
	 YM void _delay                (sSI);


    /* @method sendEvent,String ev
    Post event message to event queue

    Useful for asynchronous thread communication.

    This method is usually called from a thread different to the main/UI thread.
    */
	 YM void _sendEvent            (YAC_Object *_s);


    /* @method enableKeyRepeat,int delay,int rate
    Configure SDL key repeat

    This is a wrapper for the SDL_EnableKeyRepeat() native function.

    (note) in SDL2 build, key repeat events are filtered out when (0 == _rate).

    @deprecated Since this is a global setting it is recommend to use the §tkui (per widget) keyrepeat mechanism instead
    */
	 YM void _enableKeyRepeat      (sSI _delay, sSI _rate);


    /* @method getNumJoysticks:int
    Query number of available joystick devices. Lazy-inits SDL JOYSTICK subsystem.
    */
	 YM sSI  _getNumJoysticks      (void);


    /* @method getJoystick,int dev:Joystick
    Return Joystick device object. Lazy-inits SDL JOYSTICK subsystem.
    */
	 YM void _getJoystick          (sSI, YAC_Value*);


    /* @method updateJoysticks
    Update joystick state

    This is a wrapper for the SDL_JoystickUpdate() native function.
    */
	 YM void _updateJoysticks      (void);


    /* @method reportJoystickEvent,boolean bEnable
    Configure whether joystick events shall be reported

    This is a wrapper for the SDL_JoystickEventState() native function.
    */
	 YM void _reportJoystickEvents (sSI);


    /* @method setEventPolling,boolean bEnabled
    Configure whether blocking waits shall be used when retrieving the next SDL message

    @arg bEnabled If true, enable event polling (CPU intensive). It is recommended to set this to false and use the onTimer() callback instead of onIdle(). If event polling is disabled, onIdle() will only be called when a window event occurs.
    */
	 YM void _setEventPolling      (sSI _bEnabled);


    /* @method getEventPolling:boolean
    Query whether blocking waits shall be used when retrieving the next SDL message
    */
	 YM sSI  _getEventPolling      (void);


    /* @method enableUNICODE,boolean bEnabled
    Configure whether unicode conversion shall be performed when processing keyboard events

    This is a wrapper for the SDL_EnableUNICODE() native function.

    No-Op in SDL2 build.
    */
    YM sBool _enableUNICODE       (sSI _enabled);


    /* @method isApplicationActive:boolean
    Query whether the application windows is currently in the foreground.
    */
    YM sSI  _isApplicationActive  (void);


    // do not call this unless you absolutely have to
    YM sBool _pollEvent(void);


    /* @method setTimerInterval,int intervalMillisec
     Set timer interval for onTimer() signal/callback.

     The timer is started when §eventLoop() is called and stops when §exitEventLoop() is called.

     Warning: The timer callback runs in its own thread.

     @arg intervalMillisec Timer interval (milliseconds). 0 disables the timer.
     */
    YM void _setTimerInterval (sUI _intervalMillisec);


    /* @method getTimerInterval:int
     Query timer interval.
     
     @return Current timer interval (milliseconds)
    */
    YM sUI _getTimerInterval (void);

    // returns syswminfo.window (hack for VST editor)
    YM void _getNativeWindowHandle (YAC_Value *_r);

    YM void showWindow (void);
    YM void hideWindow (void);

    // Show (other) window
    //  (note) used for switching between sequencer and VST host process windows
    YM sUI _getNativeProcessId (void);
    YM void _allowShowNativeWindow (sUI _otherProcessId);
    YM void _showNativeWindow (YAC_String *_nativeWindowHandle, sBool _bFocus);

    // Track window create/destroy (used by Eureka VST host to find additional plugin windows)
    YM void _installWindowCreateAndDestroyHook (void);
    YM void _uninstallWindowCreateAndDestroyHook (void);

    // **deprecated** (not available on macOS, unless hwnd is own window)
    YM void _showOtherWindow (sUI _hwnd);
    YM void _hideOtherWindow (sUI _hwnd);
    YM void _bringWindowToTop (sUI _hwnd);

    // DPI awareness for Windows 10. 
    //  0(false)=PROCESS_DPI_UNAWARE (let os scale the window, assume dpi=96)
    //   1(true)=PROCESS_PER_MONITOR_DPI_AWARE (no scaling)
    //  (note) must be called before opening any windows
    YM void setDpiAwareness (sBool _bEnable);

    // Enable WM_POINTER messages (Windows 8 or newer)
    YM void setTouchInput (sBool _bEnable);

    /* // Wait for WM_CLOSE */
    /* void waitForOtherWindow (void); */

    // Show / hide window title bar and/or border
    YM void setEnableWindowTitleAndBorder (sBool _bCaption, sBool _bBorder);

    /* @method setIconFromBMP,Buffer bmpData
       Set window icon

       @arg bmpData BMP buffer
    */
    YM sBool setIconFromBMP (YAC_Object *_bmpBuffer);

    /* @method setCursorZoom,int zoom
       Sets cursor shape zoom factor (e.g. for Linux 4k screens)
       Default is 1.
       @arg zoom Zoom factor
    */
    YM void setCursorZoom (sUI _zoom);

    /* @method getCursorZoom:int
       Query cursor shape zoom factor
       @return Zoom factor
    */
    YM sUI  getCursorZoom (void);
};	 

extern _HAL *tks_HAL;


#endif // TKSDL_HAL_H__
