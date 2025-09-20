/// tkclap_window.h
///
/// (c) 2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 01Jul2024
/// changed: 02Jul2024, 03Jul2024, 04Jul2024, 05Jul2024
///
///
///
#ifndef TKCLAP_WINDOW_H__


#if defined(YAC_MACOS) || defined(YAC_LINUX)
// (todo) move to tkclap_mutex.h ?
typedef pthread_mutex_t tkclap_mutex_t;
extern void tkclap_mutex_init   (tkclap_mutex_t *mtx);
static inline void tkclap_mutex_lock   (tkclap_mutex_t *mtx) { pthread_mutex_lock(mtx); }
static inline void tkclap_mutex_unlock (tkclap_mutex_t *mtx) { pthread_mutex_unlock(mtx); }
#endif // YAC_MACOS || YAC_LINUX

#if defined(YAC_WIN32)
typedef CRITICAL_SECTION tkclap_mutex_t;
static inline void tkclap_mutex_init   (tkclap_mutex_t *mtx) { ::InitializeCriticalSection(mtx); }
static inline void tkclap_mutex_lock   (tkclap_mutex_t *mtx) { ::EnterCriticalSection(mtx); }
static inline void tkclap_mutex_unlock (tkclap_mutex_t *mtx) { ::LeaveCriticalSection(mtx); }
#endif // YAC_WIN32

#ifdef YAC_MACOS
#ifndef TKCLAP_WINDOW_SKIP_NS_WINDOW_VIEW_TYPES
typedef void *NSWindow;
typedef void *NSView;
#endif // TKCLAP_WINDOW_SKIP_NS_WINDOW_VIEW_TYPES
#endif // YAC_MACOS

class TKCLAPWindow;

#ifdef YAC_MACOS
typedef struct tkclap_window_view_s {
   CLAPPlugin      *plugin;
   TKCLAPWindow    *tkclapwindow;
   NSWindow        *nsWindow;
   NSView          *nsView;
} tkclap_window_view_t;
#endif // YAC_MACOS

#define TKCLAP_MAX_WINDOWTITLE_SIZE 256

class TKCLAPWindow {
public:
   TKCLAPWindow         *next;
   CLAPPlugin           *plugin;
#ifdef YAC_MACOS
   tkclap_window_view_t  window_view;
#elif defined(YAC_WIN32)
   HWND                  hWnd;
#else
   // (todo) Linux
/* #error unhandled OS */
#endif
   char                  window_title[TKCLAP_MAX_WINDOWTITLE_SIZE];
   sBool                 b_allow_redirect_close_to_hide;
};

// implemented in tkclap_window_macos.cpp and tkclap_window_win32.cpp
extern void          tkclap_window_lazy_init_mtx_windows (void);
extern void          tkclap_window_build_window_title    (CLAPPlugin *thiz);
extern TKCLAPWindow *tkclap_window_create                (CLAPPlugin *thiz);
extern TKCLAPWindow *tkclap_window_find_by_plugin        (CLAPPlugin *_plugin, sBool _bLock);
extern void          tkclap_window_get_geometry          (TKCLAPWindow *vw, sSI *x, sSI *y, sSI *w, sSI *h);
extern void          tkclap_window_set_geometry          (TKCLAPWindow *vw, sSI x, sSI y, sSI w, sSI h);
extern void          tkclap_window_set_visible           (TKCLAPWindow *vw, sBool bVisible);
extern sBool         tkclap_window_is_visible            (TKCLAPWindow *vw);
extern void          tkclap_window_close                 (TKCLAPWindow *vw);

#ifdef YAC_MACOS
extern void *tkclap_macos_nswindow_get_nsview (void *nsWindow);
#endif // YAC_MACOS


#endif // TKCLAP_WINDOW_H__
