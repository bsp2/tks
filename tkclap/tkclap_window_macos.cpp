/// tkclap_window_macos.cpp
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

#include "tkclap.h"

// called by window_view.mm:windowWillClose notification
void  tkclap_window_remove_by_nsview (void *_nsView);
// see window_view.mm
void  tkclap_macos_create_window_view     (tkclap_window_view_t *_winView, int _w, int _h, const char *_title);
void  tkclap_macos_get_window_geometry    (tkclap_window_view_t *_winView, int *x, int *y, int *w, int *h);
void  tkclap_macos_set_window_geometry    (tkclap_window_view_t *_winView, int x, int y, int w, int h);
void  tkclap_macos_close_window_view      (tkclap_window_view_t *_winView);
void  tkclap_macos_setvisible_window_view (tkclap_window_view_t *_winView, int bHide);
int   tkclap_macos_isvisible_window_view  (tkclap_window_view_t *_winView);
void  tkclap_macos_tofront_window_view    (tkclap_window_view_t *_winView);
void  tkclap_macos_update_window_view     (tkclap_window_view_t *_winView);


// ---------------------------------------------------------------------------- tkclap_mutex_init
#if defined(YAC_MACOS) || defined(YAC_LINUX)
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
static pthread_mutex_t loc_pthread_mutex_t_init = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#elif defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
//macOS
static pthread_mutex_t loc_pthread_mutex_t_init = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else
// recursive initializer not available, just use the normal one and hope it works ;)
static pthread_mutex_t loc_pthread_mutex_t_init = PTHREAD_MUTEX_INITIALIZER;
#endif // PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#endif // YAC_MACOS || YAC_LINUX

void tkclap_mutex_init(tkclap_mutex_t *mtx) {
   *mtx = loc_pthread_mutex_t_init;
}

// ---------------------------------------------------------------------------- module vars
static tkclap_mutex_t mtx_windows;
static sBool b_mtx_windows = YAC_FALSE;

static TKCLAPWindow *first_window = NULL;
static char loc_init_window_title[TKCLAP_MAX_WINDOWTITLE_SIZE];

// ---------------------------------------------------------------------------- tkclap_window_lazy_init_mtx_windows
void tkclap_window_lazy_init_mtx_windows(void) {
   if(!b_mtx_windows)
   {
      b_mtx_windows = YAC_TRUE;
      tkclap_mutex_init(&mtx_windows);
   }
}

static inline void tkclap_window_lock(void) {
   tkclap_mutex_lock(&mtx_windows);
}

static inline void tkclap_window_unlock(void) {
   tkclap_mutex_unlock(&mtx_windows);
}

// ---------------------------------------------------------------------------- tkclap_window_build_window_title
void tkclap_window_build_window_title(CLAPPlugin *thiz) {
   // Generate window title (also used as class name)
   Dyac_snprintf(loc_init_window_title, TKCLAP_MAX_WINDOWTITLE_SIZE,
                 "%s / %s -- CLAP editor (thiz=%p)",
                 thiz->plugin_desc->vendor, thiz->plugin_desc->name, thiz
                 );
}

// ---------------------------------------------------------------------------- tkclap_window_get_geometry
void tkclap_window_get_geometry(TKCLAPWindow *vw, sSI *x, sSI *y, sSI *w, sSI *h) {
   tkclap_macos_get_window_geometry(&vw->window_view, x, y, w, h);
}

// ---------------------------------------------------------------------------- tkclap_window_set_geometry
void tkclap_window_set_geometry(TKCLAPWindow *vw, sSI x, sSI y, sSI w, sSI h) {
   tkclap_macos_set_window_geometry(&vw->window_view, x, y, w, h);
}

// ---------------------------------------------------------------------------- tkclap_window_set_visible
void tkclap_window_set_visible(TKCLAPWindow *vw, sBool bVisible) {
   tkclap_macos_setvisible_window_view(&vw->window_view, bVisible);
}

// ---------------------------------------------------------------------------- tkclap_window_is_visible
sBool tkclap_window_is_visible(TKCLAPWindow *vw) {
   return tkclap_macos_isvisible_window_view(&vw->window_view);
}

// ---------------------------------------------------------------------------- tkclap_window_close
void tkclap_window_close(TKCLAPWindow *vw) {
   // (note) calls tkclap_window_remove_by_nsview() via windowWillClose notification (see window_view.mm)
   tkclap_macos_close_window_view(&vw->window_view);
}

// ---------------------------------------------------------------------------- tkclap_window_create
TKCLAPWindow *tkclap_window_create(CLAPPlugin *thiz) {

   Dprintf("[dbg] tkclap_window_create: ENTER.\n");

   TKCLAPWindow *vw = NULL;

   tkclap_window_build_window_title(thiz);

   sBool bParent = YAC_TRUE;  // create top-level window and call ext_gui->set_parent() to embed plugin UI
   // sBool bParent = YAC_FALSE;  // let plugin create floating top-level window (does not seem to work with any plugin)

   if(1)
   {
      // Create plugin editor
      if(thiz->ext_gui->create(thiz->plugin, bParent?CLAP_WINDOW_API_COCOA:"", bParent?0:1/*is_floating=0=>embed in parent*/))
      {
         Dprintf("[dbg] tkclap_window_create: ext_gui->create() OK\n");

         if(bParent)
         {
#if 0
            Dprintf("[dbg] tkclap_window_create: call ext_gui->set_scale(1)\n");
            if(thiz->ext_gui->set_scale(thiz->plugin, 1.0))
            {
               Dprintf("[dbg] tkclap_window_create: ext_gui->set_scale() OK\n");
            }
            else
            {
               Dprintf("[~~~] tkclap_window_create: ext_gui->set_scale() failed\n");
            }
#endif

            Dprintf("[dbg] tkclap_window_create: call ext_gui->get_size()\n");
            sU32 sizeX = 0u;
            sU32 sizeY = 0u;
            if(thiz->ext_gui->get_size(thiz->plugin, &sizeX, &sizeY))
            {
               Dprintf("[dbg] tkclap_window_create: ext_gui->get_size() returned (%u,%u)\n", sizeX, sizeY);
            }
            else
            {
               Dprintf("[~~~] tkclap_window_create: ext_gui->get_size() failed\n");
            }

            if(sizeX < 100)
               sizeX = 100;
            if(sizeY < 100)
               sizeY = 100;

            Dprintf("[dbg] tkclap_window_create: creating window with size=(%u,%u)\n", sizeX, sizeY);

            // Create toplevel window
            vw = new(std::nothrow) TKCLAPWindow;
            tkclap_window_view_t windowView = {thiz, vw, NULL, NULL};
            tkclap_macos_create_window_view(&windowView, sizeX, sizeY, loc_init_window_title);

            // Remember in window list
            tkclap_window_lock();
            vw->next                           = first_window;
            vw->window_view                    = windowView;
            vw->plugin                         = thiz;
            vw->b_allow_redirect_close_to_hide = YAC_TRUE;
            ::strncpy(vw->window_title, loc_init_window_title, TKCLAP_MAX_WINDOWTITLE_SIZE);
            first_window = vw;
            tkclap_window_unlock();

            // Set plugin editor parent
            clap_window_t clapParentWindow;
            clapParentWindow.api   = CLAP_WINDOW_API_COCOA;
            clapParentWindow.cocoa = vw->window_view.nsView;
            if(thiz->ext_gui->set_parent(thiz->plugin, &clapParentWindow))
            {
               Dprintf("[dbg] tkclap_window_create: ext_gui->set_parent() OK\n");
            }
            else
            {
               Dprintf("[~~~] tkclap_window_create: ext_gui->set_parent() failed\n");
            }

            if(thiz->ui_window_w >= 100)
            {
               // Restore window geometry
               Dprintf("[dbg] tkclap_window_create: restore geo=(%d; %d; %d; %d)\n",
                       thiz->ui_window_x, thiz->ui_window_y,
                       thiz->ui_window_w, thiz->ui_window_h
                       );
               tkclap_window_set_geometry(vw,
                                          thiz->ui_window_x, thiz->ui_window_y,
                                          thiz->ui_window_w, thiz->ui_window_h
                                          );
            }
            else
            {
               tkclap_window_get_geometry(vw,
                                          &thiz->ui_window_x, &thiz->ui_window_y,
                                          &thiz->ui_window_w, &thiz->ui_window_h
                                          );
               Dprintf("[dbg] tkclap_window_create: query geo=(%d; %d; %d; %d)\n",
                       thiz->ui_window_x, thiz->ui_window_y,
                       thiz->ui_window_w, thiz->ui_window_h
                       );
            }

         } // if bParent
         else
         {
            // (note) none of the tested plugins seem to work with floating windows
            //         (either ext_gui->create() fails or the window is not visible, despite ext_gui->show())

#ifdef USE_CLAP_GUI_TRANSIENT
            // set_transient
            if(NULL != thiz->transient_native_window_handle)
            {
               clap_window_t windowBelow;
               ::memset((void*)&windowBelow, 0, sizeof(clap_window_t));
               windowBelow.api = CLAP_WINDOW_API_COCOA;
               windowBelow.cocoa = (void*)(NSView *) tkclap_macos_nswindow_get_nsview(thiz->transient_native_window_handle);
               if(thiz->ext_gui->set_transient(thiz->plugin, &windowBelow))
               {
                  Dprintf("[~~~] tkclap_window_create: ext_gui->set_transient() OK\n");
               }
               else
               {
                  Dprintf("[~~~] tkclap_window_create: ext_gui->set_transient() failed\n");
               }
            }

            // suggest_title
            char windowTitle[512];
            snprintf(windowTitle, 512, "CLAP editor [%s]", thiz->plugin_desc->name);
            Dprintf("[dbg] tkclap_window_create: call ext_gui->suggest_title()\n");
            thiz->ext_gui->suggest_title(thiz->plugin, windowTitle);
#endif // USE_CLAP_GUI_TRANSIENT
         }
      }
      else
      {
         Dprintf("[~~~] tkclap_window_create: ext_gui->create() failed\n");
      }
   }

   Dprintf("[dbg] tkclap_window_create: LEAVE. vw=%p\n", vw);
   return vw;
}

// ---------------------------------------------------------------------------- tkclap_window_FindCLAPWindowByNSWindow
#if 0
// (todo) unused, remove ?
static TKCLAPWindow *tkclap_window_FindCLAPWindowByNSWindow(NSWindow *_nsWindow) {

   // Dprintf("xxx FindCLAPWindowByHandle 1\n");
   tkclap_window_lock();
   // Dprintf("xxx FindCLAPWindowByHandle 2\n");

   TKCLAPWindow *vw = first_window;
   while(NULL != vw)
   {
      if(vw->window_view.nsWindow == _nsWindow)
      {
         break;
      }
      else
      {
         vw = vw->next;
      }
   }

   // Dprintf("xxx FindCLAPWindowByHandle 3\n");
   tkclap_window_unlock();
   // Dprintf("xxx FindCLAPWindowByHandle 4\n");

   return vw;
}
#endif // 0

// ---------------------------------------------------------------------------- tkclap_window_find_by_plugin
TKCLAPWindow *tkclap_window_find_by_plugin(CLAPPlugin *_plugin, sBool _bLock) {
   if(_bLock)
      tkclap_window_lock();

   TKCLAPWindow *vw = first_window;
   while(NULL != vw)
   {
      if(vw->plugin == _plugin)
         break;
      else
         vw = vw->next;
   }

   if(_bLock)
      tkclap_window_unlock();

   return vw;
}

// ---------------------------------------------------------------------------- tkclap_window_remove_by_nsview
void tkclap_window_remove_by_nsview(void *_nsView) {
   // called by window_view.mm:CLAPView::windowWillClose

   Dprintf("[dbg] tkclap_window_remove_by_nsview: nsView=%p\n", _nsView);

   TKCLAPWindow *vw = first_window;
   while(NULL != vw)
   {
      if(vw->window_view.nsView == _nsView)
      {
         // Remember window position and size
         {
            Dprintf("[dbg] tkclap_window_remove_by_nsview: caching window geometry\n");
            tkclap_window_get_geometry(vw,
                                       &vw->plugin->ui_window_x, &vw->plugin->ui_window_y,
                                       &vw->plugin->ui_window_w, &vw->plugin->ui_window_h
                                       );
            Dprintf("[dbg] tkclap_window_remove_by_nsview: geo=(%d; %d; %d; %d)\n",
                    vw->plugin->ui_window_x, vw->plugin->ui_window_y,
                    vw->plugin->ui_window_w, vw->plugin->ui_window_h
                    );
         }

         // Free plugin editor resources
         Dprintf("[dbg] tkclap_window_remove_by_nsview: call ext_gui->destroy()\n");
         vw->plugin->ext_gui->destroy(vw->plugin->plugin);
         Dprintf("[dbg] tkclap_window_remove_by_nsview: ext_gui->destroy() returned\n");

         Dprintf("[dbg] tkclap_window_remove_by_nsview: window nsView=0x%p destroyed\n", _nsView);

         tkclap_window_lock();

         // Remove from window list
         if(vw == first_window)
         {
            // Unlink from window list (first window)
            first_window = vw->next;
         }
         else
         {
            // Unlink from window list
            TKCLAPWindow *p = first_window;
            while(p->next != vw)
               p = p->next;
            p->next = vw->next;
         }

         // Free memory for TKCLAPWindow
         delete vw;

         tkclap_window_unlock();
         break;
      }
      else
      {
         vw = vw->next;
      }
   } // while vw

}
