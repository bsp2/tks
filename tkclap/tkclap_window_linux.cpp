/// tkclap_window_linux.cpp
///
/// (c) 2024-2026 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 06Jul2024
/// changed: 27Apr2026, 28Apr2026
///
///

#include <poll.h>
#include "tkclap.h"


void tkclap_window_remove_by_lglw (lglw_t _lglw);

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
   (void)lglw_window_size_get(vw->lglw, w, h);
   (void)lglw_window_pos_get(vw->lglw, x, y);
}

// ---------------------------------------------------------------------------- tkclap_window_set_geometry
void tkclap_window_set_geometry(TKCLAPWindow *vw, sSI x, sSI y, sSI w, sSI h) {
   (void)lglw_window_size_set(vw->lglw, w, h);
   (void)lglw_window_pos_set(vw->lglw, x, y);
}

// ---------------------------------------------------------------------------- tkclap_window_set_visible
void tkclap_window_set_visible(TKCLAPWindow *vw, sBool bVisible) {
   if(NULL != vw->lglw)
   {
      if(bVisible)
         lglw_window_show(vw->lglw);
      else
         lglw_window_hide(vw->lglw);
   }
}

// ---------------------------------------------------------------------------- tkclap_window_is_visible
sBool tkclap_window_is_visible(TKCLAPWindow *vw) {
   if(NULL != vw->lglw)
      return lglw_window_is_visible(vw->lglw);
   return YAC_FALSE;
}

// ---------------------------------------------------------------------------- tkclap_window_close
static void loc_on_lglw_close(lglw_t _lglw) {
   Dprintf("[dbg] loc_on_lglw_close: lglw=%p\n", _lglw);
   TKCLAPWindow *vw = (TKCLAPWindow*)lglw_userdata_get(_lglw);
   vw->b_close_pending = true;
}

// ---------------------------------------------------------------------------- tkclap_window_close
void tkclap_window_close(TKCLAPWindow *vw) {
   lglw_window_close(vw->lglw);
   lglw_exit(vw->lglw);
   vw->lglw = NULL;
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
      if(thiz->ext_gui->create(thiz->plugin, bParent?CLAP_WINDOW_API_X11:"", bParent?0:1/*is_floating=0=>embed in parent*/))
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

            // (VST_COMMENT) will be overwritten by initial windowDidMove() callback (0;0)
            sSI uiWindowX = thiz->ui_window_x;
            sSI uiWindowY = thiz->ui_window_y;

            // Create toplevel window
            vw = new(std::nothrow) TKCLAPWindow;
            lglw_t lglw = lglw_init(32,32);  // initial hidden window size=32x32 (remove??)
            lglw_userdata_set(lglw, (void*)vw);
            lglw_close_callback_set(lglw, &loc_on_lglw_close);
            (void)lglw_window_open(lglw, NULL/*parent*/, uiWindowX/*x*/, uiWindowY/*y*/, sizeX, sizeY);

            // Remember in window list
            tkclap_window_lock();
            vw->plugin                         = thiz;
            vw->next                           = first_window;
            vw->lglw                           = lglw;
            vw->clap_fd                        = -1;
            vw->clap_fd_flags                  = 0;
            vw->b_close_pending                = YAC_FALSE;
            vw->b_allow_redirect_close_to_hide = YAC_TRUE;
            ::strncpy(vw->window_title, loc_init_window_title, TKCLAP_MAX_WINDOWTITLE_SIZE);
            first_window = vw;
            tkclap_window_unlock();

            // Set plugin editor parent
            clap_window_t clapParentWindow;
            clapParentWindow.api = CLAP_WINDOW_API_X11;
            clapParentWindow.x11 = (Window)lglw_window_get_native_handle(lglw);
            if(thiz->ext_gui->set_parent(thiz->plugin, &clapParentWindow))
            {
               Dprintf("[dbg] tkclap_window_create: ext_gui->set_parent() OK\n");
            }
            else
            {
               Dprintf("[~~~] tkclap_window_create: ext_gui->set_parent() failed\n");
            }

            thiz->ui_window_x = uiWindowX;
            thiz->ui_window_y = uiWindowY;

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

            // set_transient
#if 0
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
#endif

            // suggest_title
            char windowTitle[512];
            snprintf(windowTitle, 512, "CLAP editor [%s]", thiz->plugin_desc->name);
            Dprintf("[dbg] tkclap_window_create: call ext_gui->suggest_title()\n");
            thiz->ext_gui->suggest_title(thiz->plugin, windowTitle);
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

// ---------------------------------------------------------------------------- tkclap_window_find_by_fd
TKCLAPWindow *tkclap_window_find_by_fd(int _fd, sBool _bLock) {
   if(_bLock)
      tkclap_window_lock();

   TKCLAPWindow *vw = first_window;
   while(NULL != vw)
   {
      if(vw->clap_fd == _fd)
         break;
      else
         vw = vw->next;
   }

   if(_bLock)
      tkclap_window_unlock();

   return vw;
}

// ---------------------------------------------------------------------------- tkclap_window_find_by_plugin
void tkclap_window_process_events(void) {
   tkclap_window_lock();

   TKCLAPWindow *vw = first_window;
   while(NULL != vw)
   {
      sBool bNext = true;
      if(NULL != vw->lglw)
      {
         lglw_events(vw->lglw);

         if(vw->b_close_pending)
         {
            vw->b_close_pending = false;

            tkclap_window_remove_by_lglw(vw->lglw);

            vw = first_window;
            bNext = false;
         }
         else
         {
            if(-1 != vw->clap_fd)
            {
               struct pollfd fds;
               fds.fd = vw->clap_fd;
               fds.events = POLLIN;
               // (note) fds.revents written by poll()
               int ready = poll(&fds, 1, 0/*timeout*/);
               if(ready >= 1)
               {
                  // Dyac_host_printf("xxx poll fd=%d => ready=%d fds.revents=0x%08x (POLLIN=0x%08x) bCall=%d\n", fds.fd, ready, fds.revents, POLLIN, (fds.revents & POLLIN));
                  if(fds.revents & POLLIN)
                  {
                     // (todo) only call when plugin requested it
                     vw->plugin->callPosixThreadSupportOnFD(vw->clap_fd, CLAP_POSIX_FD_READ);
                  }
               }
            }
            vw->plugin->processTimers(yac_host->yacMilliSeconds());
         }
      }
      if(bNext)
         vw = vw->next;
   }

   tkclap_window_unlock();
}

// ---------------------------------------------------------------------------- tkclap_window_remove_by_lglw
void tkclap_window_remove_by_lglw(lglw_t _lglw) {
   // called by window_view.mm:CLAPView::windowWillClose

   Dprintf("[dbg] tkclap_window_remove_by_lglw: lglw=%p\n", _lglw);

   TKCLAPWindow *vw = first_window;
   while(NULL != vw)
   {
      if(vw->lglw == _lglw)
      {
         // Remember window position and size
         {
            Dprintf("[dbg] tkclap_window_remove_by_lglw: caching window geometry\n");
            tkclap_window_get_geometry(vw,
                                       &vw->plugin->ui_window_x, &vw->plugin->ui_window_y,
                                       &vw->plugin->ui_window_w, &vw->plugin->ui_window_h
                                       );
            Dprintf("[dbg] tkclap_window_remove_by_lglw: geo=(%d; %d; %d; %d)\n",
                    vw->plugin->ui_window_x, vw->plugin->ui_window_y,
                    vw->plugin->ui_window_w, vw->plugin->ui_window_h
                    );
         }

         // Free plugin editor resources
         Dprintf("[dbg] tkclap_window_remove_by_lglw: call ext_gui->destroy()\n");
         vw->plugin->ext_gui->destroy(vw->plugin->plugin);
         Dprintf("[dbg] tkclap_window_remove_by_lglw: ext_gui->destroy() returned\n");

         Dprintf("[dbg] tkclap_window_remove_by_lglw: window lglw=%p destroyed\n", _lglw);

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

         lglw_exit(vw->lglw);
         vw->lglw = NULL;

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
