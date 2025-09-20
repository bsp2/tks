/// CLAPPlugin.cpp
///
/// (c) 2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 01Jul2024
/// changed: 02Jul2024, 03Jul2024, 04Jul2024, 05Jul2024, 06Jul2024, 24Sep2024, 27Sep2024
///          29Nov2024
///
///
///

#include <math.h>

#include "tkclap.h"

#ifdef YAC_LINUX
#include <dlfcn.h>
#endif // YAC_LINUX

#include "CLAPPlugin.h"
#include "../tkmidipipe/HostMIDIEvents.h"
#include "../tkmidipipe/ModInputFilter.h"

YAC_FunctionHandle tkclap_signal_funs[CLAPPLUGIN_NUM_SIGNALS];
YAC_ContextHandle tkclap_script_context;


// ---------------------------------------------------------------------------- clap_host_log_t (CLAP_EXT_LOG)
static clap_host_log_t loc_ext_clap_host_log;

static void CLAP_ABI loc_clap_host_log_log(const clap_host_t *host, clap_log_severity severity, const char *msg) {
   CLAPPlugin *clapPlugin = (CLAPPlugin*)host->host_data;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   yac_host->printf("[log] [\"%s\":\"%s\"] severity=%d msg=\"%s\"\n", pathName, pluginId, severity, msg);
}


// ---------------------------------------------------------------------------- clap_host_params_t (CLAP_EXT_PARAMS)
static clap_host_params_t loc_ext_clap_host_params;

static void CLAP_ABI loc_clap_host_params_rescan(const clap_host_t *host, clap_param_rescan_flags flags) {
   (void)flags;
   CLAPPlugin *clapPlugin = (CLAPPlugin*)host->host_data;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   Dprintf("[trc] [\"%s\":\"%s\"] clap_host_params.rescan()\n", pathName, pluginId);
   clapPlugin->callOnRescanParams();
}

static void CLAP_ABI loc_clap_host_params_clear(const clap_host_t *host, clap_id param_id, clap_param_clear_flags flags) {
   (void)param_id;
   (void)flags;
   CLAPPlugin *clapPlugin = (CLAPPlugin*)host->host_data;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   Dprintf("[trc] [\"%s\":\"%s\"] clap_host_params.clear()\n", pathName, pluginId);
}

static void CLAP_ABI loc_clap_host_params_request_flush(const clap_host_t *host) {
   CLAPPlugin *clapPlugin = (CLAPPlugin*)host->host_data;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   Dprintf_verbose("[>>>] [\"%s\":\"%s\"] clap_host_params.request_flush()\n", pathName, pluginId);
   // (note) param flush must be handled in audio thread while plugin is active (=> process())
   if(!clapPlugin->b_processing)
      clapPlugin->flushParameters();
}


// ---------------------------------------------------------------------------- clap_host_state_t (CLAP_EXT_STATE)
static clap_host_state_t loc_ext_clap_host_state;

static void CLAP_ABI loc_clap_host_state_mark_dirty(const clap_host_t *host) {
   CLAPPlugin *clapPlugin = (CLAPPlugin*)host->host_data;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   Dprintf("[trc] [\"%s\":\"%s\"] clap_host_state.mark_dirty()\n", pathName, pluginId);
}


// ---------------------------------------------------------------------------- clap_host_latency_t (CLAP_EXT_LATENCY)
static clap_host_latency_t loc_ext_clap_host_latency;

static void CLAP_ABI loc_clap_host_latency_changed(const clap_host_t *host) {
   CLAPPlugin *clapPlugin = (CLAPPlugin*)host->host_data;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   Dprintf("[trc] [\"%s\":\"%s\"] clap_host_latency.changed()\n", pathName, pluginId);
}


// ---------------------------------------------------------------------------- clap_host_gui_t (CLAP_EXT_GUI)
#ifndef SKIP_CLAP_UI
static clap_host_gui_t loc_ext_clap_host_gui;  // see clap-main/include/clap/ext/gui.h

static void CLAP_ABI loc_clap_host_gui_resize_hints_changed(const clap_host_t *host) {
   // "The host should call get_resize_hints() again."
   CLAPPlugin *clapPlugin = (CLAPPlugin*)host->host_data;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   Dprintf("[trc] [\"%s\":\"%s\"] clap_host_gui.resize_hints_changed()\n", pathName, pluginId);

   clap_gui_resize_hints_t resizeHints;
   if(clapPlugin->ext_gui->get_resize_hints(clapPlugin->plugin, &resizeHints))
   {
      Dprintf("[dbg] plugin resize_hints:\n");
      Dprintf("[dbg]    can_resize_horizontally: %d\n", resizeHints.can_resize_horizontally);
      Dprintf("[dbg]      can_resize_vertically: %d\n", resizeHints.can_resize_vertically);
      Dprintf("[dbg]      preserve_aspect_ratio: %d\n", resizeHints.preserve_aspect_ratio);
      Dprintf("[dbg]         aspect_ratio_width: %u\n", resizeHints.aspect_ratio_width);
      Dprintf("[dbg]        aspect_ratio_height: %u\n", resizeHints.aspect_ratio_height);
   }
}

static bool CLAP_ABI loc_clap_host_gui_request_resize(const clap_host_t *host, uint32_t width, uint32_t height) {
   // "Request the host to resize the client area to width, height."
   // "Return true if the new size is accepted, false otherwise."
   // "The host doesn't have to call set_size()."
   CLAPPlugin *clapPlugin = (CLAPPlugin*)host->host_data;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   Dprintf("[trc] [\"%s\":\"%s\"] clap_host_gui.request_resize(width=%u height=%u)\n", pathName, pluginId, width, height);
   return clapPlugin->handleRequestResize(width, height);
}

static bool CLAP_ABI loc_clap_host_gui_request_show(const clap_host_t *host) {
   // "Request the host to show the plugin gui."
   CLAPPlugin *clapPlugin = (CLAPPlugin*)host->host_data;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   Dprintf("[trc] [\"%s\":\"%s\"] clap_host_gui.request_show()\n", pathName, pluginId);
   return 0;
}

static bool CLAP_ABI loc_clap_host_gui_request_hide(const clap_host_t *host) {
   // "Request the host to hide the plugin gui."
   CLAPPlugin *clapPlugin = (CLAPPlugin*)host->host_data;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   Dprintf("[trc] [\"%s\":\"%s\"] clap_host_gui.request_hide()\n", pathName, pluginId);
   return 0;
}

static void CLAP_ABI loc_clap_host_gui_closed(const clap_host_t *host, bool was_destroyed) {
   // "The floating window has been closed, or the connection to the gui has been lost."
   //
   // "If was_destroyed is true, then the host must call clap_plugin_gui->destroy() to acknowledge
   // the gui destruction."
   CLAPPlugin *clapPlugin = (CLAPPlugin*)host->host_data;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   Dprintf("[trc] [\"%s\":\"%s\"] clap_host_gui.closed()\n", pathName, pluginId);
#if 0
   clapPlugin->ext_gui->destroy(clapPlugin->plugin);
#endif
}

#endif // SKIP_CLAP_UI

// ---------------------------------------------------------------------------- clap_host_t

// Query an extension.
// The returned pointer is owned by the host.
// It is forbidden to call it before plugin->init().
// You can call it within plugin->init() call, and after.
// [thread-safe]
extern "C" {
const void *CLAP_ABI loc_host_get_extension(const struct clap_host *host, const char *extension_id) {
   (void)host;
   (void)extension_id;
   Dprintf("[trc] CLAPPlugin:loc_host_get_extension: extension_id=\"%s\"\n", extension_id);

   if(!strncmp(extension_id, CLAP_EXT_LOG, 9))
   {
      Dprintf("[trc] CLAPPlugin:loc_host_get_extension:   return &loc_ext_clap_host_log\n");
      return &loc_ext_clap_host_log;
   }

   if(!strncmp(extension_id, CLAP_EXT_PARAMS, 12))
   {
      Dprintf("[trc] CLAPPlugin:loc_host_get_extension:   return &loc_ext_clap_host_params\n");
      return &loc_ext_clap_host_params;
   }

   if(!strncmp(extension_id, CLAP_EXT_STATE, 11))
   {
      Dprintf("[trc] CLAPPlugin:loc_host_get_extension:   return &loc_ext_clap_host_state\n");
      return &loc_ext_clap_host_state;
   }

   if(!strncmp(extension_id, CLAP_EXT_LATENCY, 13))
   {
      Dprintf("[trc] CLAPPlugin:loc_host_get_extension:   return &loc_ext_clap_host_latency\n");
      return &loc_ext_clap_host_latency;
   }

#ifndef SKIP_CLAP_UI
   if(!strncmp(extension_id, CLAP_EXT_GUI, 9))
   {
      Dprintf("[trc] CLAPPlugin:loc_host_get_extension:   return &loc_ext_clap_host_gui\n");
      return &loc_ext_clap_host_gui;
   }
#endif // SKIP_CLAP_UI

   return NULL;
}

// Request the host to deactivate and then reactivate the plugin.
// The operation may be delayed by the host.
// [thread-safe]
void CLAP_ABI loc_host_request_restart(const struct clap_host *host) {
   (void)host;
   Dprintf("[trc] CLAPPlugin:loc_host_request_restart()\n");
}

// Request the host to activate and start processing the plugin.
// This is useful if you have external IO and need to wake up the plugin from "sleep".
// [thread-safe]
void CLAP_ABI loc_host_request_process(const struct clap_host *host) {
   (void)host;
   Dprintf("[trc] CLAPPlugin:loc_host_request_process()\n");
}

// Request the host to schedule a call to plugin->on_main_thread(plugin) on the main thread.
// [thread-safe]
void CLAP_ABI loc_host_request_callback(const struct clap_host *host) {
   CLAPPlugin *plugin = (CLAPPlugin*)host->host_data;
   Dprintf("[trc] CLAPPlugin:loc_host_request_callback()\n");
   if(NULL != plugin)
   {
      plugin->queueCallOnMainThread();
   }
}

// see clap-main/include/clap/host.h
static const clap_host_t loc_host_init = {
   CLAP_VERSION,

   NULL/*host_data*/,  // ref to CLAPPlugin object

   "tkclap",                  // name
   "bsp",                     // vendor
   "http://miditracker.org",  // URL
   "1.0",                     // version

   &loc_host_get_extension,
   &loc_host_request_restart,
   &loc_host_request_process,
   &loc_host_request_callback
};
} // extern "C"

// ---------------------------------------------------------------------------- loc_ostream_write
static int64_t CLAP_ABI loc_ostream_write(const struct clap_ostream *stream, const void *buffer, uint64_t size) {
   YAC_Object *ofs = (YAC_Object*)stream->ctx;
   const sUI sz = (sUI)size;
   const sU8 *s = (const sU8*)buffer;
   for(sUI i = 0u; i < sz; i++)
   {
      ofs->yacStreamWriteI8(s[i]);
   }
   return int64_t(sz);
}

// ---------------------------------------------------------------------------- loc_ostream_read
static int64_t CLAP_ABI loc_istream_read(const struct clap_istream *stream, void *buffer, uint64_t size) {
   YAC_Object *ifs = (YAC_Object*)stream->ctx;
   sUI sz = (sUI)size;
   sUI streamOff = ifs->yacStreamGetOffset();
   sUI streamSz  = ifs->yacStreamGetSize();
   if(streamOff < streamSz)
   {
      if(streamOff + sz > streamSz)
         sz = streamSz - streamOff;
      sU8 *d = (sU8*)buffer;
      Dprintf_verbose("[>>>] CLAPPlugin:loc_istream_read: ifs=%p d=%p sz=%u\n", ifs, d, sz);
      for(sUI i = 0u; i < sz; i++)
      {
         d[i] = ifs->yacStreamReadI8();
      }
      return int64_t(sz);
   }
   // EOF
   return 0;
}

// ---------------------------------------------------------------------------- CLAPPluginBundle

CLAPPluginBundle::CLAPPluginBundle(void) {

   b_debug = YAC_FALSE;

#ifdef YAC_MACOS
   cf_bundle_ref  = NULL;
#elif defined(YAC_WIN32)
   dll_handle = NULL;
#else
   // Linux
   dll_handle = NULL;
#endif

   plugin_entry   = NULL;
   plugin_factory = NULL;
   num_plugins    = 0u;

   loc_ext_clap_host_log.log = &loc_clap_host_log_log;

   loc_ext_clap_host_params.rescan        = &loc_clap_host_params_rescan;
   loc_ext_clap_host_params.clear         = &loc_clap_host_params_clear;
   loc_ext_clap_host_params.request_flush = &loc_clap_host_params_request_flush;

   loc_ext_clap_host_state.mark_dirty = &loc_clap_host_state_mark_dirty;

   loc_ext_clap_host_latency.changed = &loc_clap_host_latency_changed;

#ifndef SKIP_CLAP_UI
   loc_ext_clap_host_gui.resize_hints_changed = &loc_clap_host_gui_resize_hints_changed;
   loc_ext_clap_host_gui.request_resize       = &loc_clap_host_gui_request_resize;
   loc_ext_clap_host_gui.request_show         = &loc_clap_host_gui_request_show;
   loc_ext_clap_host_gui.request_hide         = &loc_clap_host_gui_request_hide;
   loc_ext_clap_host_gui.closed               = &loc_clap_host_gui_closed;
#endif // SKIP_CLAP_UI

   num_open_plugins = 0;
}

CLAPPluginBundle::~CLAPPluginBundle(void) {
   closePluginBundle();
}

void CLAPPluginBundle::setEnableDebug(sBool _bEnable) {
   b_debug = _bEnable;
}

sBool CLAPPluginBundle::openPluginBundle(YAC_String *_pathName) {
   closePluginBundle();

   if(YAC_Is_String(_pathName))
   {
      const char *pathName = (const char*)_pathName->chars;
      Dprintf_debug("[...] CLAPPlugin::loadPlugin(\"%s\")\n", pathName);

#ifdef YAC_MACOS
      CFStringRef pluginPathStringRef = CFStringCreateWithCString(NULL,
                                                                  (char*)pathName,
                                                                  kCFStringEncodingASCII
                                                                  );
      CFURLRef bundleUrl = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                                                         pluginPathStringRef,
                                                         kCFURLPOSIXPathStyle,
                                                         true
                                                         );
      if(NULL == bundleUrl)
      {
         Dprintf_error("[---] CLAPPlugin::openPluginBundle: [\"%s\"] failed to create CFURLRef\n", pathName);
         return YAC_FALSE;
      }

      cf_bundle_ref = CFBundleCreate(kCFAllocatorDefault, bundleUrl);
      CFRelease(pluginPathStringRef);
      CFRelease(bundleUrl);
      if(NULL == cf_bundle_ref)
      {
         Dprintf_error("[---] CLAPPlugin::openPluginBundle: [\"%s\"] failed to create bundle reference\n", pathName);
         return YAC_FALSE;
      }

      clap_plugin_entry_t *pluginEntry = (clap_plugin_entry_t*)CFBundleGetDataPointerForName(cf_bundle_ref, CFSTR("clap_entry"));
#elif defined(YAC_WIN32)
      dll_handle = ::LoadLibrary((char*)_pathName->chars);
      if(NULL == dll_handle)
      {
         Dprintf_error("[---] CLAPPlugin::openPluginBundle: [\"%s\"] LoadLibrary() failed\n", pathName);
         return YAC_FALSE;
      }
      FARPROC mainProc = ::GetProcAddress((HMODULE)dll_handle, "clap_entry");
      clap_plugin_entry_t *pluginEntry = (clap_plugin_entry_t *)mainProc;
#else
      // Linux
      const sUI flags = RTLD_NOW;
      // // sUI flags = RTLD_GLOBAL;
      dll_handle = ::dlopen((const char*)_pathName->chars, flags);
      if(NULL == dll_handle)
      {
         Dprintf_error("[---] CLAPPlugin::openPluginBundle: [\"%s\"] dlopen() failed. dlerror=\"%s\"\n", pathName, ::dlerror());
         return YAC_FALSE;
      }
      void *symHandle = ::dlsym(dll_handle, "clap_entry");
      clap_plugin_entry_t *pluginEntry = (clap_plugin_entry_t *)symHandle;
#endif

      if(NULL != pluginEntry)
      {
         Dprintf_debug("[dbg] CLAPPlugin::openPluginBundle: [\"%s\"] plugin bundle opened\n", pathName);

         Dprintf_debug("[dbg] CLAPPlugin::openPluginBundle: [\"%s\"]    clap_version.major:    0x%08x\n", pathName, pluginEntry->clap_version.major);
         Dprintf_debug("[dbg] CLAPPlugin::openPluginBundle: [\"%s\"]    clap_version.minor:    0x%08x\n", pathName, pluginEntry->clap_version.minor);
         Dprintf_debug("[dbg] CLAPPlugin::openPluginBundle: [\"%s\"]    clap_version.revision: 0x%08x\n", pathName, pluginEntry->clap_version.revision);

         if(pluginEntry->init((const char*)_pathName->chars))
         {
            Dprintf_debug("[dbg] CLAPPlugin::openPluginBundle: [\"%s\"]    init() OK\n", pathName);
            plugin_entry = pluginEntry;
            path_name.copy(_pathName);

            plugin_factory = (clap_plugin_factory_t *)
               pluginEntry->get_factory(CLAP_PLUGIN_FACTORY_ID);  // see clap-main/include/clap/factory/plugin-factory.h

            if(NULL != plugin_factory)
            {
               Dprintf_debug("[dbg] CLAPPlugin::openPluginBundle: [\"%s\"]    get_factory() OK\n", pathName);

               num_plugins = plugin_factory->get_plugin_count(plugin_factory);
               Dprintf_debug("[dbg] CLAPPlugin::openPluginBundle: [\"%s\"]    factory.get_plugin_count() returned %u\n", pathName, num_plugins);
               if(num_plugins > 0u)
               {
                  // Succeeded
                  return YAC_TRUE;
               }
               else
               {
                  Dprintf_error("[---] CLAPPlugin::openPluginBundle: [\"%s\"] factory.get_plugin_count() returned 0\n", pathName);
                  plugin_factory = NULL;
               }
            }
            else
            {
               Dprintf_error("[---] CLAPPlugin::openPluginBundle: [\"%s\"] pluginEntry->init() failed\n", pathName);
            }
         }
         else
         {
            Dprintf_error("[---] CLAPPlugin::openPluginBundle: [\"%s\"] pluginEntry->init() failed\n", pathName);
         }
      }
      else
      {
         Dprintf_error("[---] CLAPPlugin::openPluginBundle: [\"%s\"] failed to resolve \"clap_entry\"\n", pathName);
      }
   }

   // Failed
   closePluginBundle();
   return YAC_FALSE;
}

sUI CLAPPluginBundle::getNumPlugins(void) {
   return num_plugins;
}

void CLAPPluginBundle::createPluginByIndex(sUI _idx, YAC_Value *_r) {
   _r->initNull();
   if(NULL != plugin_factory)
   {
      const char *pathName = (const char*)path_name.chars;

      const clap_plugin_descriptor_t *pluginDesc = plugin_factory->get_plugin_descriptor(plugin_factory, _idx);
      if(NULL != pluginDesc)
      {
         CLAPPlugin *plugin = YAC_NEW(CLAPPlugin);
         if(NULL != plugin)
         {
            plugin->b_debug       = b_debug;
            plugin->plugin_bundle = this;
            plugin->plugin_desc   = pluginDesc;

            num_open_plugins++;

            // Succeeded
            _r->initObject(plugin, YAC_TRUE);
         }
         else
         {
            Dprintf_error("[---] CLAPPlugin::createPluginByIndex: [\"%s\"] failed to allocate CLAPPlugin object (idx=%u)\n", pathName, _idx);
         }
      }
      else
      {
         Dprintf_error("[---] CLAPPluginBundle::createPluginByIndex: [\"%s\"] get_plugin_descriptor(idx=%u) returned NULL\n", pathName, _idx);
      }
   }
}

void CLAPPluginBundle::handleDestroyPlugin(void) {
   num_open_plugins--;
}

sSI CLAPPluginBundle::getNumOpenPlugins(void) {
   return num_open_plugins;
}

void CLAPPluginBundle::closePluginBundle(void) {

   if(num_open_plugins > 0)
   {
      // Not all plugins have been closed (should not be reachable)
      Dprintf("[~~~] CLAPPluginBundle::closePluginBundle: num_open_plugins=%d\n", num_open_plugins);
      num_open_plugins = 0;
   }

#ifdef YAC_MACOS
   if(NULL != cf_bundle_ref)
#elif defined(YAC_WIN32)
   if(NULL != dll_handle)
#else
   // Linux
   if(NULL != dll_handle)
#endif
   {
      const char *pathName = (const char*)path_name.chars;

      if(NULL != plugin_entry)
      {
         Dprintf_debug("[dbg] CLAPPluginBundle::close: [\"%s\"] deinit()\n", pathName);
         plugin_entry->deinit();
         plugin_entry = NULL;
      }

      plugin_factory = NULL;
      num_plugins    = 0u;

      Dprintf_debug("[dbg] CLAPPluginBundle::close: [\"%s\"] closing bundle\n", pathName);

#ifdef YAC_MACOS
      CFRelease(cf_bundle_ref);
      cf_bundle_ref = NULL;
#elif defined(YAC_WIN32)
      ::FreeLibrary(dll_handle);
      dll_handle = NULL;
#else
      // Linux
	   ::dlclose(dll_handle);
      dll_handle = NULL;
#endif

      Dprintf_debug("[dbg] CLAPPluginBundle::close: [\"%s\"] OK, bundle closed\n", pathName);

      path_name.free();
   }

}


// ---------------------------------------------------------------------------- clap_input_events
static uint32_t CLAP_ABI loc_input_events_size(const struct clap_input_events *_list) {
   const tkclap_input_events_t *events = (const tkclap_input_events_t *)_list;
   return events->num_ev;
}

static const clap_event_header_t *CLAP_ABI loc_input_events_get(const struct clap_input_events *_list, uint32_t _index) {
   const tkclap_input_events_t *events = (const tkclap_input_events_t *)_list;
   if(_index < events->num_ev)
   {
      return &events->events[_index].header;
   }
   return NULL;
}


// ---------------------------------------------------------------------------- loc_event_type_to_string
static const char *loc_event_type_to_string(uint16_t _type) {
   switch(_type)
   {
      default: break;
      case /*  0 */ CLAP_EVENT_NOTE_ON:              return "CLAP_EVENT_NOTE_ON";
      case /*  1 */ CLAP_EVENT_NOTE_OFF:             return "CLAP_EVENT_NOTE_OFF";
      case /*  2 */ CLAP_EVENT_NOTE_CHOKE:           return "CLAP_EVENT_NOTE_CHOKE";
      case /*  3 */ CLAP_EVENT_NOTE_END:             return "CLAP_EVENT_NOTE_END";
      case /*  4 */ CLAP_EVENT_NOTE_EXPRESSION:      return "CLAP_EVENT_NOTE_EXPRESSION";
      case /*  5 */ CLAP_EVENT_PARAM_VALUE:          return "CLAP_EVENT_PARAM_VALUE";
      case /*  6 */ CLAP_EVENT_PARAM_MOD:            return "CLAP_EVENT_PARAM_MOD";
      case /*  7 */ CLAP_EVENT_PARAM_GESTURE_BEGIN:  return "CLAP_EVENT_PARAM_GESTURE_BEGIN";
      case /*  8 */ CLAP_EVENT_PARAM_GESTURE_END:    return "CLAP_EVENT_PARAM_GESTURE_END";
      case /*  9 */ CLAP_EVENT_TRANSPORT:            return "CLAP_EVENT_TRANSPORT";
      case /* 10 */ CLAP_EVENT_MIDI:                 return "CLAP_EVENT_MIDI";
      case /* 11 */ CLAP_EVENT_MIDI_SYSEX:           return "CLAP_EVENT_MIDI_SYSEX";
      case /* 12 */ CLAP_EVENT_MIDI2:                return "CLAP_EVENT_MIDI2";
   }
   return "?";
}


// ---------------------------------------------------------------------------- clap_output_events
static bool CLAP_ABI loc_output_events_try_push(const struct clap_output_events *list,
                                                const clap_event_header_t       *event
                                                ) {
   // Called during process() (in audio thread)
   CLAPPlugin *clapPlugin = (CLAPPlugin*)list->ctx;
   tkclap_output_events_t *outputEvents = &clapPlugin->output_events;
   const char *pathName = (const char*)clapPlugin->plugin_bundle->path_name.chars;
   const char *pluginId = clapPlugin->plugin_desc->id;
   Dprintf_verbose("[>>>] [\"%s\":\"%s\"] clap_output_events.try_push: size=%u type=%u(%s)\n", pathName, pluginId, event->size, event->type, loc_event_type_to_string(event->type));

   if(outputEvents->num_ev < CLAPPLUGIN_MAX_EVENTS)
   {
      tkclap_event_t *ev = &outputEvents->events[outputEvents->num_ev++];
      ::memcpy((void*)ev, (const void*)event, event->size);

      // Succeeded
      return 1;
   }

   // Failed: max events exceeded
   return 0;
}


// ---------------------------------------------------------------------------- CLAPPlugin
CLAPPlugin::CLAPPlugin(void) {

   b_debug = YAC_FALSE;

   plugin_bundle = NULL;
   plugin_desc   = NULL;
   plugin        = NULL;

   ::memset((void*)&host, 0, sizeof(host));
   b_queued_call_on_main_thread = YAC_FALSE;
   tkclap_mutex_init(&mtx_call_on_main_thread);

   ext_audio_ports     = NULL;
   num_audio_in        = 0u;
   num_audio_out       = 0u;
   num_input_channels  = 0u;
   num_output_channels = 0u;
   ::memset((void*)audio_in,  0, sizeof(audio_in));
   ::memset((void*)audio_out, 0, sizeof(audio_out));

   ext_note_ports = NULL;
   ::memset(&note_in, 0, sizeof(note_in));

   ext_params  = NULL;
   num_params  = 0u;
   param_infos = NULL;

   ext_state = NULL;

   ext_latency = NULL;

#ifndef SKIP_CLAP_UI
   ext_gui = NULL;
#ifdef USE_CLAP_GUI_TRANSIENT
   transient_native_window_handle = NULL;
#endif // USE_CLAP_GUI_TRANSIENT
   ui_window_x = 0;
   ui_window_y = 0;
   ui_window_w = 0;
   ui_window_h = 0;
#endif // SKIP_CLAP_UI

   ::memset(&input_events, 0, sizeof(input_events));
   input_events.clap_input_events.ctx = (void*)this;
   input_events.clap_input_events.size = &loc_input_events_size;
   input_events.clap_input_events.get  = &loc_input_events_get;
   tkclap_mutex_init(&mtx_events);

   ::memset(&input_events_flush, 0, sizeof(input_events_flush));
   input_events_flush.clap_input_events.ctx = (void*)this;
   input_events_flush.clap_input_events.size = &loc_input_events_size;
   input_events_flush.clap_input_events.get  = &loc_input_events_get;

   ::memset(&output_events, 0, sizeof(output_events));
   output_events.clap_output_events.ctx      = (void*)this;
   output_events.clap_output_events.try_push = &loc_output_events_try_push;

   ::memset(&output_events_flush, 0, sizeof(output_events_flush));
   output_events_flush.clap_output_events.ctx      = (void*)this;
   output_events_flush.clap_output_events.try_push = &loc_output_events_try_push;

   sample_rate = 48000.0f;
   block_size  = 64u;

   tkclap_window_lazy_init_mtx_windows();

   b_processing = YAC_FALSE;

   b_report_transport_playing = YAC_TRUE;
}

CLAPPlugin::~CLAPPlugin() {
   destroyPluginInstance();
}

void CLAPPlugin::destroyPluginInstance(void) {
   // // if(NULL != dll_handle)
   // // {
	// //    ::dlclose(dll_handle);
   // //    dll_handle = NULL;
   // // }

   b_queued_call_on_main_thread = YAC_FALSE;

   if(NULL != plugin)
   {
      const char *pathName = (const char*)plugin_bundle->path_name.chars;
      const char *pluginId = plugin_desc->id;

      if(b_processing)
         stopProcessing();

      closeEditor();

      Dprintf_debug("[dbg] CLAPPlugin::destroy: [\"%s\":\"%s\"] plugin->deactivate()\n", pathName, pluginId);
      plugin->deactivate(plugin);

      Dprintf_debug("[dbg] CLAPPlugin::destroy: [\"%s\":\"%s\"] plugin->destroy()\n", pathName, pluginId);
      plugin->destroy(plugin);
      plugin = NULL;
      plugin_bundle->handleDestroyPlugin();

      ext_audio_ports     = NULL;
      num_audio_in        = 0u;
      num_audio_out       = 0u;
      num_input_channels  = 0u;
      num_output_channels = 0u;

      ext_note_ports = NULL;
      ::memset(&note_in, 0, sizeof(note_in));

      ext_params = NULL;
      freeParamInfos();

      ext_state = NULL;

      ext_latency = NULL;

#ifndef SKIP_CLAP_UI
      ext_gui = NULL;
#endif // SKIP_CLAP_UI

      input_events.num_ev = 0u;
      output_events.num_ev = 0u;

      Dprintf_debug("[dbg] CLAPPlugin::destroy: [\"%s\":\"%s\"] destroyed\n", pathName, pluginId);
   }
}

void CLAPPlugin::freeParamInfos(void) {
   num_params = 0u;
   if(NULL != param_infos)
   {
      delete [] param_infos;
      param_infos = NULL;
   }
}

void CLAPPlugin::rescanParamInfos(void) {
   freeParamInfos();
   if(NULL != ext_params)
   {
      const char *pathName = (const char*)plugin_bundle->path_name.chars;
      const char *pluginId = plugin_desc->id;

      num_params = ext_params->count(plugin);
      Dprintf_debug("[dbg] CLAPPlugin::rescanParamInfos: [\"%s\":\"%s\"] num_params=%u\n", pathName, pluginId, num_params);
      if(num_params > 0u)
      {
         param_infos = new(std::nothrow)tkclap_param_info_t[num_params];
         if(NULL != param_infos)
         {
            ::memset((void*)param_infos, 0, sizeof(tkclap_param_info_t) * num_params);
            for(sUI paramIdx = 0u; paramIdx < num_params; paramIdx++)
            {
               clap_param_info_t *info = &param_infos[paramIdx].clap_param_info;
               if(ext_params->get_info(plugin, paramIdx, info))
               {
                  // Dprintf("[dbg] CLAPPlugin::create: [\"%s\":\"%s\"]    param[%3u] name=\"%s\" min=%f max=%f def=%f\n", pathName, pluginId, paramIdx, info->name, info->min_value, info->max_value, info->default_value);
               }
               else
               {
                  Dprintf("[!!!] CLAPPlugin::rescanParamInfos: [\"%s\":\"%s\"] ext_params->get_info() failed (paramIdx=%u)\n", pathName, pluginId, paramIdx);
                  ext_params = NULL;
                  delete [] param_infos;
                  param_infos = NULL;
                  num_params = 0u;
                  break;
               }
            }
         }
      }
   }
}

CLAPPluginBundle *CLAPPlugin::getPluginBundle(void) {
   return plugin_bundle;
}

void CLAPPlugin::setEnableDebug(sBool _bEnable) {
   b_debug = _bEnable;
}

void CLAPPlugin::setSampleRate(sF32 _sampleRate) {
   if(_sampleRate < 4000.0f)
      sample_rate = 4000.0f;
   else if(_sampleRate > 384000.0f)
      sample_rate = 384000.0f;
   else
      sample_rate = _sampleRate;
}

void CLAPPlugin::setBlockSize(sUI _blockSize) {
   if(_blockSize < 16u)
      block_size = 16u;
   else if(_blockSize > CLAPPLUGIN_MAX_BLOCK_SIZE)
      block_size = CLAPPLUGIN_MAX_BLOCK_SIZE;
   else
      block_size = _blockSize;
}

sUI CLAPPlugin::getNumInputPorts(void) {
   return num_audio_in;
}

sUI CLAPPlugin::getNumOutputPorts(void) {
   return num_audio_out;
}

sUI CLAPPlugin::getNumInputPortChannels(sUI _portIdx) {
   if(_portIdx < num_audio_in)
   {
      return audio_in[_portIdx].num_ch;
   }
   return 0u;
}

sUI CLAPPlugin::getNumOutputPortChannels(sUI _portIdx) {
   if(_portIdx < num_audio_out)
   {
      return audio_out[_portIdx].num_ch;
   }
   return 0u;
}

sUI CLAPPlugin::getNumInputChannels(void) {
   return num_input_channels;
}

sUI CLAPPlugin::getNumOutputChannels(void) {
   return num_output_channels;
}

void CLAPPlugin::getChannelName(const sUI _channelIdx, CLAPAudioPort *_ports, const sUI _numPorts, YAC_Value *_r) {
   _r->initNull();
   sUI channelIdx = 0u;
   for(sUI portIdx = 0u; portIdx < _numPorts; portIdx++)
   {
      CLAPAudioPort *audioPort = &_ports[portIdx];
      if(_channelIdx >= channelIdx && _channelIdx < (channelIdx + audioPort->num_ch))
      {
         sUI relIdx = _channelIdx - channelIdx;
         _r->initEmptyString();
         _r->value.string_val->copy(audioPort->name);
         switch(relIdx)
         {
            case 0:  // L
               _r->value.string_val->append(" L");
               break;

            case 1:  // R
               _r->value.string_val->append(" R");
               break;

            default:
            {
               char buf[16];
               snprintf(buf, 16, "<%u>", (relIdx+1u));
               _r->value.string_val->append(buf);
            }
            break;
         }
         return;
      }
      // Try next port
      channelIdx += audioPort->num_ch;
   }
}

void CLAPPlugin::getInputChannelName(sUI _channelIdx, YAC_Value *_r) {
   getChannelName(_channelIdx, audio_in, num_audio_in, _r);
}

void CLAPPlugin::getOutputChannelName(sUI _channelIdx, YAC_Value *_r) {
   getChannelName(_channelIdx, audio_out, num_audio_out, _r);
}

sBool CLAPPlugin::setExtChannelBuffer(const sUI _channelIdx, YAC_Object *_fa, CLAPAudioPort *_ports, const sUI _numPorts) {
   sUI channelIdx = 0u;
   for(sUI portIdx = 0u; portIdx < _numPorts; portIdx++)
   {
      CLAPAudioPort *audioPort = &_ports[portIdx];
      if(_channelIdx >= channelIdx && _channelIdx < (channelIdx + audioPort->num_ch))
      {
         sUI relIdx = _channelIdx - channelIdx;
         if(relIdx < 2u)
         {
            if(NULL != _fa)
               audioPort->data_ptrs[relIdx] = (sF32*)_fa->yacArrayGetPointer();  // set external buf
            else
               audioPort->data_ptrs[relIdx] = &audioPort->data[CLAPPLUGIN_MAX_BLOCK_SIZE * relIdx];  // reset to internal buf
            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[!!!] CLAPPlugin::setExtChannelBuffer: relIdx=%u out of bounds (portIdx=%u channelIdx=%u)\n", relIdx, portIdx, channelIdx);
         }
         return YAC_FALSE;
      }
      // Try next port
      channelIdx += audioPort->num_ch;
   }
   return YAC_FALSE;
}

sBool CLAPPlugin::setExtInputChannelBuffer(sUI _channelIdx, YAC_Object *_fa) {
   return setExtChannelBuffer(_channelIdx, _fa, audio_in, num_audio_in);
}

sBool CLAPPlugin::setExtOutputChannelBuffer(sUI _channelIdx, YAC_Object *_fa) {
   return setExtChannelBuffer(_channelIdx, _fa, audio_out, num_audio_out);
}

sUI CLAPPlugin::getLatency(void) {
   if(NULL != plugin && NULL != ext_latency)
   {
      return ext_latency->get(plugin);
   }
   return 0u;
}

void CLAPPlugin::getId(YAC_Value *_r) {
   _r->initNull();
   if(NULL != plugin_bundle)
   {
      const char *pathName = (const char*)plugin_bundle->path_name.chars;
      if(NULL != plugin_desc)
      {
         _r->initEmptyString();
         _r->value.string_val->copy(plugin_desc->id);
      }
      else
      {
         Dprintf_error("[---] CLAPPlugin::getId: [\"%s\"] plugin_desc is NULL\n", pathName);
      }
   }
}

void CLAPPlugin::getName(YAC_Value *_r) {
   _r->initNull();
   if(NULL != plugin_bundle)
   {
      const char *pathName = (const char*)plugin_bundle->path_name.chars;
      if(NULL != plugin_desc)
      {
         _r->initEmptyString();
         _r->value.string_val->copy(plugin_desc->name);
      }
      else
      {
         Dprintf_error("[---] CLAPPlugin::getName: [\"%s\"] plugin_desc is NULL\n", pathName);
      }
   }
}

void CLAPPlugin::getVendor(YAC_Value *_r) {
   _r->initNull();
   if(NULL != plugin_bundle)
   {
      const char *pathName = (const char*)plugin_bundle->path_name.chars;
      if(NULL != plugin_desc)
      {
         _r->initEmptyString();
         _r->value.string_val->copy(plugin_desc->vendor);
      }
      else
      {
         Dprintf_error("[---] CLAPPlugin::getVendor: [\"%s\"] plugin_desc is NULL\n", pathName);
      }
   }
}

sBool CLAPPlugin::createPluginInstance(void) {
   if(NULL != plugin_bundle && NULL != plugin_desc)
   {
      const char *pathName = (const char*)plugin_bundle->path_name.chars;
      const char *pluginId = plugin_desc->id;
      Dprintf_debug("[dbg] CLAPPlugin::create: [\"%s\"] id=\"%s\"\n", pathName, pluginId);

      ::memcpy((void*)&host, (const void*)&loc_host_init, sizeof(host));
      host.host_data = (void*)this;

      // Create instance
      plugin = plugin_bundle->plugin_factory->create_plugin(plugin_bundle->plugin_factory,
                                                            &host,
                                                            pluginId
                                                            );
      if(NULL != plugin)
      {
         // Initialize plugin
         if(plugin->init(plugin))
         {
            // Query audio-ports extension
            ext_audio_ports = (clap_plugin_audio_ports_t *)plugin->get_extension(plugin, CLAP_EXT_AUDIO_PORTS);
            // printf("xxx CLAPPlugin::create: b_debug=%d\n", b_debug);
            if(NULL != ext_audio_ports)
            {
               num_audio_in  = ext_audio_ports->count(plugin, 1/*is_input*/);
               num_audio_out = ext_audio_ports->count(plugin, 0/*is_input*/);

               Dprintf_debug("[dbg] CLAPPlugin::create: [\"%s\":\"%s\"] num_audio_in=%u  num_audio_out=%u\n", pathName, pluginId, num_audio_in, num_audio_out);

               if(num_audio_in  < CLAPPLUGIN_MAX_AUDIO_IN  &&
                  num_audio_out < CLAPPLUGIN_MAX_AUDIO_OUT
                  )
               {
                  num_audio_in  = sMIN(num_audio_in,  CLAPPLUGIN_MAX_AUDIO_IN);
                  num_audio_out = sMIN(num_audio_out, CLAPPLUGIN_MAX_AUDIO_OUT);

                  ::memset((void*)audio_in,  0, sizeof(audio_in));
                  ::memset((void*)audio_out, 0, sizeof(audio_out));

                  ::memset((void*)clap_audio_buffers_in,  0, sizeof(clap_audio_buffers_in));
                  ::memset((void*)clap_audio_buffers_out, 0, sizeof(clap_audio_buffers_out));

                  // Query audio inputs
                  for(sUI inIdx = 0u; inIdx < num_audio_in; inIdx++)
                  {
                     clap_audio_port_info_t info;
                     if(ext_audio_ports->get(plugin, inIdx, 1/*is_input*/, &info))
                     {
                        Dprintf_debug("[dbg] CLAPPlugin::create:   audio_in [%2u] #ch=%u name=\"%s\" type=\"%s\" in_place_pair=%u\n", inIdx, info.channel_count, info.name, info.port_type, info.in_place_pair);
                        CLAPAudioPort *audioPort = &audio_in[inIdx];
                        memset((void*)audioPort, 0, sizeof(CLAPAudioPort));
                        memcpy((void*)audioPort->name, (const void*)info.name, sizeof(info.name));
                        audioPort->num_ch = info.channel_count;
                        num_input_channels += info.channel_count;
                        clap_audio_buffer_t *buf = &clap_audio_buffers_in[inIdx];
                        if(1u == audioPort->num_ch)
                        {
                           audioPort->data_ptrs[0] = &audioPort->data[0];
                           audioPort->data_ptrs[1] = NULL;
                           buf->data32        = audioPort->data_ptrs;
                           buf->latency       = 64u;  // (todo) pass actual latency
                           buf->channel_count = 1u;
                        }
                        else if(2u == audioPort->num_ch)
                        {
                           audioPort->data_ptrs[0] = &audioPort->data[0];
                           audioPort->data_ptrs[1] = &audioPort->data[CLAPPLUGIN_MAX_BLOCK_SIZE];
                           buf->data32        = audioPort->data_ptrs;
                           buf->latency       = 64u;  // (todo) pass actual latency
                           buf->channel_count = 2u;
                        }
                        else
                        {
                           Dprintf("[!!!] CLAPPlugin::create: [\"%s\":\"%s\"] unsupported channel_count=%u (inPortIdx=%u)\n", pathName, pluginId, info.channel_count, inIdx);
                        }
                     }
                     else
                     {
                        Dprintf("[!!!] CLAPPlugin::create: [\"%s\":\"%s\"] ext_audio_ports->get() failed (in)\n", pathName, pluginId);
                     }
                  }

                  // Query audio outputs
                  for(sUI outIdx = 0u; outIdx < num_audio_out; outIdx++)
                  {
                     clap_audio_port_info_t info;
                     if(ext_audio_ports->get(plugin, outIdx, 0/*is_input*/, &info))
                     {
                        Dprintf_debug("[dbg] CLAPPlugin::create:   audio_out[%2u] #ch=%u name=\"%s\" type=\"%s\" in_place_pair=%u\n", outIdx, info.channel_count, info.name, info.port_type, info.in_place_pair);
                        CLAPAudioPort *audioPort = &audio_out[outIdx];
                        memset((void*)audioPort, 0, sizeof(CLAPAudioPort));
                        memcpy((void*)audioPort->name, (const void*)info.name, sizeof(info.name));
                        audioPort->num_ch = info.channel_count;
                        num_output_channels += info.channel_count;
                        clap_audio_buffer_t *buf = &clap_audio_buffers_out[outIdx];
                        if(1u == audioPort->num_ch)
                        {
                           audioPort->data_ptrs[0] = &audioPort->data[0];
                           audioPort->data_ptrs[1] = NULL;
                           buf->data32        = audioPort->data_ptrs;
                           buf->latency       = 64u;  // (todo) pass actual latency
                           buf->channel_count = 1u;
                        }
                        else if(2u == audioPort->num_ch)
                        {
                           audioPort->data_ptrs[0] = &audioPort->data[0];
                           audioPort->data_ptrs[1] = &audioPort->data[CLAPPLUGIN_MAX_BLOCK_SIZE];
                           buf->data32        = audioPort->data_ptrs;
                           buf->latency       = 64u;  // (todo) pass actual latency
                           buf->channel_count = 2u;
                        }
                        else
                        {
                           Dprintf("[!!!] CLAPPlugin::create: [\"%s\":\"%s\"] unsupported channel_count=%u (outPortIdx=%u)\n", pathName, pluginId, info.channel_count, outIdx);
                        }
                     }
                     else
                     {
                        Dprintf("[!!!] CLAPPlugin::create: [\"%s\":\"%s\"] ext_audio_ports->get() failed (out)\n", pathName, pluginId);
                     }
                  }

                  if(num_input_channels  <= 48/*Mod.MAX_IO_BUFFERS*/)
                  {
                     if(num_output_channels <= 48/*Mod.MAX_IO_BUFFERS*/)
                     {
                        // Query note-ports extension
                        ext_note_ports = (clap_plugin_note_ports_t *)plugin->get_extension(plugin, CLAP_EXT_NOTE_PORTS);
                        ::memset((void*)&note_in, 0, sizeof(note_in));
                        if(NULL != ext_note_ports)
                        {
                           const sUI numNoteIn  = ext_note_ports->count(plugin, 1/*is_input*/);
                           const sUI numNoteOut = ext_note_ports->count(plugin, 0/*is_input*/);

                           Dprintf_debug("[dbg] CLAPPlugin::create: [\"%s\":\"%s\"] num_note_in=%u  num_note_out=%u\n", pathName, pluginId, numNoteIn, numNoteOut);
                           if(numNoteIn >= 1u)
                           {
                              if(ext_note_ports->get(plugin, 0u/*index*/, 1/*is_input*/, &note_in))
                              {
                                 const uint32_t dia = note_in.supported_dialects;
                                 Dprintf_debug("[dbg] CLAPPlugin::create:   note_in[0] dialects:%s%s%s%s name=\"%s\"\n",
                                         (dia & CLAP_NOTE_DIALECT_CLAP)    ?" CLAP"    :"",
                                         (dia & CLAP_NOTE_DIALECT_MIDI)    ?" MIDI"    :"",
                                         (dia & CLAP_NOTE_DIALECT_MIDI_MPE)?" MIDI_MPE":"",
                                         (dia & CLAP_NOTE_DIALECT_MIDI2)   ?" MIDI2"   :"",
                                         note_in.name
                                         );
                              }
                           }
                        }

                        // Query params extension
                        ext_params = (clap_plugin_params_t *)plugin->get_extension(plugin, CLAP_EXT_PARAMS);
                        if(NULL != ext_params)
                        {
                           rescanParamInfos();
                        }

                        // Query state extension
                        ext_state = (clap_plugin_state_t *)plugin->get_extension(plugin, CLAP_EXT_STATE);

                        // Query latency extension
                        ext_latency = (clap_plugin_latency_t *)plugin->get_extension(plugin, CLAP_EXT_LATENCY);

                        // Query GUI extension
                        ext_gui = (clap_plugin_gui_t *)plugin->get_extension(plugin, CLAP_EXT_GUI);

                        // Activate plugin
                        if(plugin->activate(plugin,
                                            sample_rate,
                                            block_size,  // min_frames_count
                                            block_size   // max_frames_count
                                            )
                           )
                        {
                           // Succeeded
                           Dprintf_debug("[dbg] CLAPPlugin::loadPlugin: [\"%s\":\"%s\"] plugin activated\n", pathName, pluginId);
                           return YAC_TRUE;
                        }
                        else
                        {
                           Dprintf_error("[---] CLAPPlugin::loadPlugin: [\"%s\":\"%s\"] plugin->activate() failed\n", pathName, pluginId);
                           plugin->destroy(plugin);
                           plugin = NULL;
                        }
                     }
                     else
                     {
                        Dprintf_error("[---] CLAPPlugin::loadPlugin: [\"%s\":\"%s\"] num_output_channels exceeds MAX_IO_BUFFERS=%u\n", pathName, pluginId, num_input_channels, 48/*Mod.MAX_IO_BUFFERS*/);
                        plugin->destroy(plugin);
                        plugin = NULL;
                     }
                  }
                  else
                  {
                     Dprintf_error("[---] CLAPPlugin::loadPlugin: [\"%s\":\"%s\"] num_input_channels exceeds MAX_IO_BUFFERS=%u\n", pathName, pluginId, num_input_channels, 48/*Mod.MAX_IO_BUFFERS*/);
                     plugin->destroy(plugin);
                     plugin = NULL;
                  }
               }
               else
               {
                  Dprintf_error("[---] CLAPPlugin::loadPlugin: [\"%s\":\"%s\"] #audioports exceeds MAX=%u\n", pathName, pluginId, num_audio_in, num_audio_out);
                  plugin->destroy(plugin);
                  plugin = NULL;
               }
            } // if ext_audio_ports
            else
            {
               Dprintf_error("[---] CLAPPlugin::loadPlugin: [\"%s\":\"%s\"] ext_audio_ports is NULL\n", pathName, pluginId);
               plugin->destroy(plugin);
               plugin = NULL;
            }
         }
         else
         {
            Dprintf_error("[---] CLAPPlugin::loadPlugin: [\"%s\":\"%s\"] plugin->init() failed\n", pathName, pluginId);
            plugin->destroy(plugin);
            plugin = NULL;
         }
      }
      else
      {
         Dprintf_error("[---] CLAPPlugin::loadPlugin: [\"%s\":\"%s\"] create_plugin() returned NULL\n", pathName, pluginId);
      }
   }
   else
   {
      Dprintf_error("[---] CLAPPlugin::loadPlugin: parent plugin_bundle is NULL\n");
   }

   // Failed
   return YAC_FALSE;
}

void CLAPPlugin::lockEvents(void) {
   tkclap_mutex_lock(&mtx_events);
}

void CLAPPlugin::unlockEvents(void) {
   tkclap_mutex_unlock(&mtx_events);
}

tkclap_event_t *CLAPPlugin::getNextInputEvent(void) {
   tkclap_event_t *r = NULL;
   if(input_events.num_ev < CLAPPLUGIN_MAX_EVENTS)
   {
      r = &input_events.events[input_events.num_ev++];
      ::memset((void*)r, 0, sizeof(tkclap_event_t));
   }
   return r;
}

sBool CLAPPlugin::isInstrument(void) {
   if(NULL != plugin_desc)
   {
      const char *const*features = plugin_desc->features;
      if(NULL != features)
      {
         while(NULL != *features)
         {
            const char *sFeature = *features;
            if(!strncmp(sFeature, CLAP_PLUGIN_FEATURE_INSTRUMENT, 11))
            {
               return YAC_TRUE;
            }

            // Next feature
            features++;
         }
      }
   }
   return YAC_FALSE;
}

sBool CLAPPlugin::isEffect(void) {
   if(NULL != plugin_desc)
   {
      return !isInstrument();
   }
   return YAC_FALSE;
}

sBool CLAPPlugin::hasEditor(void) {
#ifndef SKIP_CLAP_UI
   return (NULL != ext_gui);
#else
   return YAC_FALSE;
#endif // SKIP_CLAP_UI
}

sUI CLAPPlugin::getNumParameters(void) {
   if(NULL != plugin)
   {
      return num_params;
   }
   return 0u;
}

sBool CLAPPlugin::checkParameterFlag(const sUI _index, const sUI _flagMask) {
   if(NULL != plugin)
   {
      if(_index < num_params)
      {
         tkclap_param_info_t *infoTK = &param_infos[_index];
         return (0u != (infoTK->clap_param_info.flags & _flagMask));
      }
   }
   return YAC_FALSE;
}

sBool CLAPPlugin::isParameterStepped(sUI _index) {
   return checkParameterFlag(_index, (CLAP_PARAM_IS_STEPPED | CLAP_PARAM_IS_ENUM));
}

sBool CLAPPlugin::isParameterPeriodic(sUI _index) {
   return checkParameterFlag(_index, CLAP_PARAM_IS_PERIODIC);
}

sBool CLAPPlugin::isParameterHidden(sUI _index) {
   return checkParameterFlag(_index, CLAP_PARAM_IS_HIDDEN);
}

sBool CLAPPlugin::isParameterReadOnly(sUI _index) {
   return checkParameterFlag(_index, CLAP_PARAM_IS_READONLY);
}

sBool CLAPPlugin::isParameterBypass(sUI _index) {
   return checkParameterFlag(_index, CLAP_PARAM_IS_BYPASS);
}

sBool CLAPPlugin::isParameterModulatable(sUI _index) {
   return checkParameterFlag(_index, CLAP_PARAM_IS_MODULATABLE);
}

void CLAPPlugin::setParameter(sUI _index, sF32 _value) {
   if(NULL != plugin)
   {
      if(_index < num_params)
      {
         clap_event_param_value_t *ev = (clap_event_param_value_t*)getNextInputEvent();
         if(NULL != ev)
         {
            clap_param_info_t *info = &param_infos[_index].clap_param_info;
            ev->header.size = sizeof(clap_event_param_value_t);
            ev->header.type  = CLAP_EVENT_PARAM_VALUE;
            // // ev->flags       = CLAP_EVENT_IS_LIVE; // ??
            ev->param_id    = info->id;
            ev->cookie      = info->cookie;  // cached param ptr or NULL
            ev->note_id     = -1;  // wildcard = all
            ev->port_index  = -1;
            ev->channel     = -1;
            ev->key         = -1;
            ev->value       = (sF64)_value;
         }
      }
   }
}

void CLAPPlugin::setParameter_Sync(sUI _index, sF32 _value) {
   lockEvents();
   setParameter(_index, _value);
   unlockEvents();
}

void CLAPPlugin::setParameterMod(sUI _index, sF32 _offset) {
   if(NULL != plugin)
   {
      if(_index < num_params)
      {
         clap_event_param_mod_t *ev = (clap_event_param_mod_t*)getNextInputEvent();
         if(NULL != ev)
         {
            tkclap_param_info_t *infoTK = &param_infos[_index];
            clap_param_info_t *info = &infoTK->clap_param_info;
            ev->header.size = sizeof(clap_event_param_mod_t);
            ev->header.type  = CLAP_EVENT_PARAM_MOD;
            // // ev->flags       = CLAP_EVENT_IS_LIVE; // ??
            ev->param_id    = info->id;
            ev->cookie      = info->cookie;  // cached param ptr or NULL
            ev->note_id     = -1;  // wildcard = all
            ev->port_index  = -1;
            ev->channel     = -1;
            ev->key         = -1;
            ev->amount      = (sF64)_offset;
            infoTK->cur_mod = _offset;
         }
      }
   }
}

void CLAPPlugin::setParameterMod_Sync(sUI _index, sF32 _offset) {
   lockEvents();
   setParameterMod(_index, _offset);
   unlockEvents();
}

void CLAPPlugin::resetParameterMods(void) {
   if(NULL != plugin)
   {
      for(sUI paramIdx = 0u; paramIdx < num_params; paramIdx++)
      {
         tkclap_param_info_t *infoTK = &param_infos[paramIdx];
         if(0.0f != infoTK->cur_mod)
         {
            setParameterMod(paramIdx, 0.0f);
         }
      }
   }
}

void CLAPPlugin::resetParameterMods_Sync(void) {
   lockEvents();
   resetParameterMods();
   unlockEvents();
}

sF32 CLAPPlugin::getParameter(sUI _index) {
   if(NULL != plugin)
   {
      if(_index < num_params)
      {
         sF64 val = 0.0;
         if(ext_params->get_value(plugin, param_infos[_index].clap_param_info.id, &val))
         {
            return (sF32)val;
         }
      }
   }
   return 0.0f;
}

sF32 CLAPPlugin::getParameterMod(sUI _index) {
   if(NULL != plugin)
   {
      if(_index < num_params)
      {
         tkclap_param_info_t *infoTK = &param_infos[_index];
         return infoTK->cur_mod;
      }
   }
   return 0.0f;
}

void CLAPPlugin::getParameterName(sUI _index, YAC_Value *_r) {
   _r->initNull();
   if(NULL != plugin)
   {
      if(_index < num_params)
      {
         _r->initEmptyString();
         _r->value.string_val->copy(param_infos[_index].clap_param_info.name);
      }
   }
}

void CLAPPlugin::getParameterPath(sUI _index, YAC_Value *_r) {
   _r->initNull();
   if(NULL != plugin)
   {
      if(_index < num_params)
      {
         _r->initEmptyString();
         _r->value.string_val->copy(param_infos[_index].clap_param_info.module);
      }
   }
}

sF32 CLAPPlugin::getParameterMinValue(sUI _index) {
   if(NULL != plugin)
   {
      if(_index < num_params)
      {
         return (sF32)param_infos[_index].clap_param_info.min_value;
      }
   }
   return 0.0f;
}

sF32 CLAPPlugin::getParameterMaxValue(sUI _index) {
   if(NULL != plugin)
   {
      if(_index < num_params)
      {
         return (sF32)param_infos[_index].clap_param_info.max_value;
      }
   }
   return 0.0f;
}

sF32 CLAPPlugin::getParameterDefValue(sUI _index) {
   if(NULL != plugin)
   {
      if(_index < num_params)
      {
         return (sF32)param_infos[_index].clap_param_info.default_value;
      }
   }
   return 0.0f;
}

void CLAPPlugin::parameterValueToText(sUI _index, sF32 _value, YAC_Value *_r) {
   _r->initNull();
   if(NULL != plugin)
   {
      if(_index < num_params)
      {
         char buf[512];
         if(ext_params->value_to_text(plugin, param_infos[_index].clap_param_info.id, (sF64)_value, buf, 511))
         {
            buf[511] = 0;
            _r->initEmptyString();
            _r->value.string_val->copy(buf);
         }
      }
   }
}

sF32 CLAPPlugin::parameterTextToValue(sUI _index, YAC_String *_text) {
   if(YAC_Is_String(_text))
   {
      if(NULL != plugin)
      {
         if(_index < num_params)
         {
            sF64 val = 0.0;
            if(ext_params->text_to_value(plugin, param_infos[_index].clap_param_info.id, (const char*)_text->chars, &val))
            {
               return (sF32)val;
            }
         }
      }
   }
   return 0.0f;
}

sUI CLAPPlugin::queueHostMIDIEventsByFlt(YAC_Object *_hostMIDIEvents,
                                         sSI _fltDev, sSI _fltCh,
                                         sSI _forceCh,
                                         sSI _globalCh,
                                         YAC_Object *_modInputFilterOrNull,
                                         sUI _minMillisec, sUI _maxMillisec
                                         ) {
   // Returns number of queued events
   sUI r = 0u;

   if(NULL != plugin)
   {
      // // if(YAC_BCHK(_hostMIDIEvents, clid_HostMIDIEvents))  // (note) moved to tkmidipipe (clid_HostMIDIEvents not available here)
      {
         YAC_CAST_ARG(HostMIDIEvents, hostMIDIEvents, _hostMIDIEvents);
         YAC_CAST_ARG(ModInputFilter, modInputFilter, _modInputFilterOrNull);

         sBool bBeginEv = YAC_FALSE;

         for(sUI i = 0u; i < hostMIDIEvents->num_midi_events; i++)
         {
            HostMIDIEvent *hev = &hostMIDIEvents->midi_events[i];

            if( (hev->millisec >= _minMillisec) && (hev->millisec < _maxMillisec) )
            {
               if( (-1 == _fltDev) || (sUI(_fltDev) == hev->dev_idx) )
               {
                  if( (-1 == _fltCh) || (sUI(_fltCh) == (hev->midi_data[0] & 15u)) | (sUI(_globalCh) == (hev->midi_data[0] & 15u)) )
                  {
                     sBool bFilterOk = YAC_TRUE;

                     if(NULL != modInputFilter)
                     {
                        bFilterOk = modInputFilter->checkMessage(hev->midi_data[0],
                                                                 hev->midi_data[1],
                                                                 hev->midi_data[2]
                                                                 );
                     }

                     if(bFilterOk)
                     {
                        if(YAC_FALSE == bBeginEv)
                        {
                           lockEvents();
                           bBeginEv = YAC_TRUE;
                        }

                        // if((_minMillisec > 0) && (_maxMillisec < ~0u))
                        //    printf("xxx CLAPPlugin::queueHostMIDIEventsByFlt: min=%u max=%u\n", _minMillisec, _maxMillisec);

                        clap_event_midi_t *ev = (clap_event_midi_t*)getNextInputEvent();
                        if(NULL != ev)
                        {
                           ev->header.size = sizeof(clap_event_midi_t);
                           ev->header.type = CLAP_EVENT_MIDI;
                           // ev->port_index = 0u;  // (note) already set to 0 by memset()

                           if(-1 != _forceCh)
                           {
                              // Used by LPC/MPE mode (single lane plugin always expects ch0)
                              ev->data[0] = (hev->midi_data[0] & 0xF0u) | (sUI(_forceCh) & 15u);
                           }
                           else
                           {
                              ev->data[0] = hev->midi_data[0];
                           }

                           ev->data[1] = hev->midi_data[1];
                           ev->data[2] = hev->midi_data[2];

                           // Post range-filter transpose
                           if(NULL != modInputFilter)
                           {
                              if( ((ev->data[0] & 0xF0u) == 0x90u) ||
                                  ((ev->data[0] & 0xF0u) == 0x80u)
                                  ) // note on or off ?
                              {
                                 sS16 xNote = sS16(ev->data[1]) + modInputFilter->note_transpose;
                                 ev->data[1] = (sU8) sRANGE(xNote, 0, 127);
                              }
                           }

                           r++;
                        }
                        // else: maximum number of events exceeded (CLAPPLUGIN_MAX_EVENTS==4096)
                     }
                  }
               }
            }
         }

         if(YAC_TRUE == bBeginEv)
         {
            unlockEvents();
         }
      }
   } // if plugin

   return r;
}

sBool CLAPPlugin::saveState(YAC_Object *_ofs) {
   if(NULL != plugin && NULL != ext_state && YAC_VALID(_ofs))
   {
      clap_ostream_t ostream;
      ostream.ctx   = (void*)_ofs;
      ostream.write = &loc_ostream_write;
      if(ext_state->save(plugin, &ostream))
      {
         // Succeeded
         return YAC_TRUE;
      }
      else
      {
         const char *pathName = (const char*)plugin_bundle->path_name.chars;
         const char *pluginId = plugin_desc->id;
         Dprintf_error("[---] CLAPPlugin::saveState: [\"%s\":\"%s\"] ext_state->save() failed\n", pathName, pluginId);
      }
   }
   return YAC_FALSE;
}

sBool CLAPPlugin::loadState(YAC_Object *_ifs) {
   if(NULL != plugin && NULL != ext_state && YAC_VALID(_ifs))
   {
      clap_istream_t istream;
      istream.ctx  = (void*)_ifs;
      istream.read = &loc_istream_read;
      if(ext_state->load(plugin, &istream))
      {
         // Succeeded
         return YAC_TRUE;
      }
      else
      {
         const char *pathName = (const char*)plugin_bundle->path_name.chars;
         const char *pluginId = plugin_desc->id;
         Dprintf_error("[---] CLAPPlugin::loadState: [\"%s\":\"%s\"] ext_state->load() failed\n", pathName, pluginId);
      }
   }
   return YAC_FALSE;
}

void CLAPPlugin::startProcessing(void) {
   if(NULL != plugin && !b_processing)
   {
      plugin->start_processing(plugin);
      b_processing = YAC_TRUE;
   }
}

void CLAPPlugin::stopProcessing(void) {
   if(NULL != plugin && b_processing)
   {
      plugin->stop_processing(plugin);
      b_processing = YAC_FALSE;
   }
}

void CLAPPlugin::reset(void) {
   // (todo) does this also reset the parameter modulation ? (=> test)
   if(NULL != plugin)
   {
      plugin->reset(plugin);
   }
}

void CLAPPlugin::queueCallOnMainThread(void) {
   tkclap_mutex_lock(&mtx_call_on_main_thread);
   b_queued_call_on_main_thread = YAC_TRUE;
   tkclap_mutex_unlock(&mtx_call_on_main_thread);
}

void CLAPPlugin::callOnMainThread(void) {
   tkclap_mutex_lock(&mtx_call_on_main_thread);
   if(b_queued_call_on_main_thread)
   {
      b_queued_call_on_main_thread = YAC_FALSE;
      tkclap_mutex_unlock(&mtx_call_on_main_thread);

      if(NULL != plugin)
      {
         Dprintf_debug("[dbg] CLAPPlugin::callOnMainThread: call plugin->on_main_thread()\n");
         plugin->on_main_thread(plugin);
      }
   }
   else
   {
      tkclap_mutex_unlock(&mtx_call_on_main_thread);
   }
}

void YAC_VCALL CLAPPlugin::yacGetSignalStringList(YAC_String *_sig) {
   // ---- the number represents the encoded argument type identifiers used by this function
   // ---- 1=int, 2=float, 3=object. bits 0+1 represent argument #0, bits 2+3 represent argument #1 and so on
   //
   _sig->visit("onParamEdit:23 onAutomate:39 onWin32KeyEvent:7 onMacOSKeyDown:7 onRescanParams:3");
}

void YAC_VCALL CLAPPlugin::yacRegisterSignal(sUI _id, YAC_FunctionHandle _fun) {
   if(_id < CLAPPLUGIN_NUM_SIGNALS)
   {
      tkclap_signal_funs[_id] = _fun;
   }
}

void CLAPPlugin::callOnParamEdit(sBool _bBegin, sSI _paramIdx) {
   void *f = tkclap_signal_funs[CLAPPLUGIN_SIGNAL_ONPARAMEDIT];
   ////yac_host->printf("xxx callOnParamEdit f=0x%p\n", f);
   if(NULL != f)
   {
      YAC_Value args[3];
      args[0].initObject(this, 0);
      args[1].initInt(_bBegin);
      args[2].initInt(_paramIdx);
      yac_host->yacEvalFunction(tkclap_script_context, f, 3, args);
   }
}

void CLAPPlugin::callOnAutomate(sSI _paramIdx, sF32 _value) {
   // yac_host->printf("xxx CLAPPlugin::callOnAudioMasterAutomate: call setParameter: idx=%d val=%f\n", _paramIdx, _value);
   void *f = tkclap_signal_funs[CLAPPLUGIN_SIGNAL_ONAUTOMATE];

   ////yac_host->->printf("xxx callOnAudioMasterAutomate: f=0x%p\n", f);
   if(NULL != f)
   {
      YAC_Value args[3];
      args[0].initObject(this, 0);
      args[1].initInt   (_paramIdx);
      args[2].initFloat (_value);
      yac_host->yacEvalFunction(tkclap_script_context, f, 3, args);
      ////yac_host->->printf("xxx callOnAutomate: f call returned\n");
   }
}

void CLAPPlugin::callOnWin32KeyEvent(sUI _lparam) {
   // (note) called via WM_KEYDOWN event (see tkclap_window_win32.cpp)
   Dprintf_debug("[dbg] CLAPPlugin::callOnWin32KeyEvent: lparam=0x%08x\n", _lparam);

   void *f = tkclap_signal_funs[CLAPPLUGIN_SIGNAL_ONWIN32KEYEVENT];
   if(NULL != f)
   {
      YAC_Value args[2];
      args[0].initObject(this, 0);
      args[1].initInt((sSI)_lparam);
      yac_host->yacEvalFunction(tkclap_script_context, f, 2, args);
      ////yac_host->->printf("xxx callOnWin32KeyEvent: f call returned\n");
   }
}

void CLAPPlugin::callOnMacOSKeyDown(sUI _keyCode) {
   // (note) called via keyDown event (see window_view.mm)
   Dprintf_debug("[dbg] CLAPPlugin::callOnMacOSKeyEvent: keyCode=0x%08x\n", _keyCode);

   void *f = tkclap_signal_funs[CLAPPLUGIN_SIGNAL_ONMACOSKEYDOWN];
   if(NULL != f)
   {
      YAC_Value args[2];
      args[0].initObject(this, 0);
      args[1].initInt((sSI)_keyCode);
      yac_host->yacEvalFunction(tkclap_script_context, f, 2, args);
      ////yac_host->->printf("xxx callOnMacOSKeyDown: f call returned\n");
   }
}

void CLAPPlugin::callOnRescanParams(void) {
   const char *pluginId = plugin_desc->id;
   Dprintf_debug("[dbg] CLAPPlugin::callOnRescanParams: pluginId=\"%s\"\n", pluginId);
   void *f = tkclap_signal_funs[CLAPPLUGIN_SIGNAL_ONRESCANPARAMS];
   if(NULL != f)
   {
      YAC_Value args[2];
      args[0].initObject(this, 0);
      yac_host->yacEvalFunction(tkclap_script_context, f, 1, args);
      ////yac_host->->printf("xxx callOnRescanParams: f call returned\n");
   }
}

YM void CLAPPlugin::setTransientNativeWindowHandle(YAC_String *_handle) {
   // (todo) REMOVE
#if defined(YAC_MACOS) && defined(USE_CLAP_GUI_TRANSIENT)
   transient_native_window_handle = NULL;
   if(YAC_Is_String(_handle))
   {
      union {
         sS64     s64;
         void     *any;
         NSWindow *nsWindow;
      } u;

      if(_handle->yacScanI64(&u.s64))
      {
         Dprintf_debug("[dbg] CLAPPlugin::setTransientNativeWindowHandle: nsWindow=%p\n", u.nsWindow);
         transient_native_window_handle = u.any;
         NSView *nsView = (NSView*)tkclap_macos_nswindow_get_nsview(u.any);
         Dprintf_debug("[dbg] CLAPPlugin::setTransientNativeWindowHandle: => nsView=%p\n", nsView);
      }
   }
#endif // YAC_MACOS
}

sBool CLAPPlugin::handleRequestResize(sUI _sizeX, sUI _sizeY) {
   // called by loc_clap_host_gui_request_resize()
   const char *pathName = (const char*)plugin_bundle->path_name.chars;
   const char *pluginId = plugin_desc->id;
   Dprintf_debug("[dbg] CLAPPlugin::handleRequestResize: [\"%s\":\"%s\"] size=(%u,%u)\n", pathName, pluginId, _sizeX, _sizeY);
   TKCLAPWindow *vw = tkclap_window_find_by_plugin(this, YAC_FALSE/*bLock*/);
   if(NULL != vw)  // should not be NULL
   {
      ui_window_w = _sizeX;
      ui_window_h = _sizeY;
      tkclap_window_set_geometry(vw,
                                 ui_window_x, ui_window_y,
                                 ui_window_w, ui_window_h
                                 );
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

void CLAPPlugin::showEditor(void) {
#ifndef SKIP_CLAP_UI
   if(NULL != ext_gui)
   {
      const char *pathName = (const char*)plugin_bundle->path_name.chars;
      const char *pluginId = plugin_desc->id;

      Dprintf_debug("[dbg] CLAPPlugin::showEditor: [\"%s\":\"%s\"]\n", pathName, pluginId);

      // Open editor if it is not already open
      TKCLAPWindow *vw = tkclap_window_find_by_plugin(this, YAC_TRUE/*bLock*/);
      if(NULL == vw)
      {
         Dprintf_debug("[dbg] CLAPPlugin::showEditor: call tkclap_window_create(this=%p)\n", this);
         vw = tkclap_window_create(this);
         Dprintf_debug("[dbg] CLAPPlugin::showEditor: END tkclap_window_create(this=%p)\n", this);
      }

      // To front
      if(NULL != vw)
      {
#if 1
         // // if(NULL == vw)
         {
            Dprintf_debug("[dbg] CLAPPlugin::showEditor: [\"%s\":\"%s\"] call ext_gui->show()\n", pathName, pluginId);
            ext_gui->show(plugin);
         }
#endif

         Dprintf_debug("[dbg] CLAPPlugin::showEditor: calling set_visible(true)\n");
         tkclap_window_set_visible(vw, YAC_TRUE);
      }
   }
#endif // SKIP_CLAP_UI
}

void CLAPPlugin::hideEditor(void) {
#ifndef SKIP_CLAP_UI
   if(NULL != ext_gui)
   {
      const char *pathName = (const char*)plugin_bundle->path_name.chars;
      const char *pluginId = plugin_desc->id;
      Dprintf_debug("[dbg] CLAPPlugin::hideEditor: [\"%s\":\"%s\"]\n", pathName, pluginId);
      TKCLAPWindow *vw = tkclap_window_find_by_plugin(this, YAC_FALSE/*bLock*/);
      if(NULL != vw)
      {
         Dprintf_debug("[dbg] CLAPPlugin::hideEditor: call set_visible(false)\n");
         tkclap_window_set_visible(vw, YAC_FALSE);
      }
      else
      {
         // Plugin has created its own top-level window
         ext_gui->hide(plugin);
      }
   }
#endif // SKIP_CLAP_UI
}

void CLAPPlugin::closeEditor(void) {
#ifndef SKIP_CLAP_UI
   if(NULL != ext_gui)
   {
      const char *pathName = (const char*)plugin_bundle->path_name.chars;
      const char *pluginId = plugin_desc->id;
      Dprintf_debug("[dbg] CLAPPlugin::closeEditor: [\"%s\":\"%s\"]\n", pathName, pluginId);

      TKCLAPWindow *vw = tkclap_window_find_by_plugin(this, false);
      if(NULL != vw)
      {
         vw->b_allow_redirect_close_to_hide = YAC_FALSE;
         tkclap_window_close(vw);
      }
#if 0
      else
      {
         // Plugin has created its own top-level window
         ext_gui->destroy(plugin);
      }
#endif
   }
#endif // SKIP_CLAP_UI
}

sBool CLAPPlugin::isEditorVisible(void) {
#ifndef SKIP_CLAP_UI
   if(NULL != ext_gui)
   {
      TKCLAPWindow *vw = tkclap_window_find_by_plugin(this, true/*bLock*/);
      if(NULL != vw)
      {
         return tkclap_window_is_visible(vw);
      }
   }
#endif // SKIP_CLAP_UI
   return YAC_FALSE;
}

sBool CLAPPlugin::isEditorOpen(void) {
#ifndef SKIP_CLAP_UI
   if(NULL != ext_gui)
   {
      TKCLAPWindow *vw = tkclap_window_find_by_plugin(this, true/*bLock*/);
      return (NULL != vw);
   }
#endif // SKIP_CLAP_UI
   return YAC_FALSE;
}

void CLAPPlugin::setEnableReportTransportPlaying(sBool _bEnable) {
   b_report_transport_playing = _bEnable;
}

sBool CLAPPlugin::getEnableReportTransportPlaying(void) {
   return b_report_transport_playing;
}

sSI CLAPPlugin::mapParamIdToParamIdx(clap_id _paramId) {
   // (note) paramId is a uint32
   // (todo) use hashtable
   sSI r = -1;
   for(sUI paramIdx = 0u; paramIdx < num_params; paramIdx++)
   {
      if(param_infos[paramIdx].clap_param_info.id == _paramId)
      {
         r = (sSI)paramIdx;
         break;
      }
   }
   return r;
}

void CLAPPlugin::processOutputEvents(void) {
   // (note) mtx_events must be locked by caller

#if 1
   for(sUI evIdx = 0u; evIdx < output_events.num_ev; evIdx++)
   {
      tkclap_event_t *ev = &output_events.events[evIdx];
      Dprintf_verbose("[>>>] CLAPPlugin::processOutputEvents: ev[%u] type=%u\n", evIdx, ev->header.type);
      sSI paramIdx;
      switch(ev->header.type)
      {
         default:
            break;

         case CLAP_EVENT_PARAM_GESTURE_BEGIN:
            paramIdx = mapParamIdToParamIdx(ev->gesture.param_id/*uint32_t*/);
            if(paramIdx >= 0)
            {
               callOnParamEdit(YAC_TRUE/*bBegin*/, paramIdx);
            }
            break;

         case CLAP_EVENT_PARAM_VALUE:
            paramIdx = mapParamIdToParamIdx(ev->param_value.param_id/*uint32_t*/);
            if(paramIdx >= 0)
            {
               callOnAutomate(paramIdx, (sF32)ev->param_value.value);
            }
            break;

         case CLAP_EVENT_PARAM_GESTURE_END:
            paramIdx = mapParamIdToParamIdx(ev->gesture.param_id/*uint32_t*/);
            if(paramIdx >= 0)
            {
               callOnParamEdit(YAC_FALSE/*bBegin*/, paramIdx);
            }
            break;
      }
   }
#endif

   // Clear event list
   output_events.num_ev = 0u;
}

void CLAPPlugin::flushParameters(void) {
   // (note) called by loc_clap_host_params_request_flush() or processSilence()
   if(NULL != ext_params)
   {
      // (note) executed when plugin events are not handled in audio thread process()
      lockEvents();

      input_events_flush.num_ev = 0u;
      output_events_flush.num_ev = 0u;

      ext_params->flush(plugin,
                        &input_events_flush.clap_input_events,
                        &output_events_flush.clap_output_events
                        );

      processOutputEvents();

      unlockEvents();
   }
}

void CLAPPlugin::processSilence(sUI _numFrames) {
   if(NULL != plugin)
   {
#if 1
      if(_numFrames > block_size)
      {
         yac_host->printf("[~~~] CLAPPlugin::processSilence: _numFrames exceeds block_size (%u > %u). clipping..\n", _numFrames, block_size);
         _numFrames = block_size;
      }
#endif

      // (note) calls processOutputEvents()
      flushParameters();

      // Fill output buffers with 0
      for(sUI i = 0u; i < num_audio_out; i++)
      {
         CLAPAudioPort *audioPort = &audio_out[i];
         if(NULL != audioPort->data_ptrs[0])
            memset((void*)audioPort->data_ptrs[0], 0, sizeof(sF32) * _numFrames);
         if(NULL != audioPort->data_ptrs[1])
            memset((void*)audioPort->data_ptrs[1], 0, sizeof(sF32) * _numFrames);
      }
   }

}

void CLAPPlugin::process(sUI _numFrames) {
   if(NULL != plugin)
   {
      lockEvents();

      clap_process_t process;
      ::memset((void*)&process, 0, sizeof(process));

      clap_event_transport_t evTransport;
      ::memset((void*)&evTransport, 0, sizeof(evTransport));

      evTransport.flags = CLAP_TRANSPORT_HAS_TEMPO | CLAP_TRANSPORT_HAS_BEATS_TIMELINE;
      if(tkclap_song_playing)
         evTransport.flags |= CLAP_TRANSPORT_IS_PLAYING;

      evTransport.tempo          = (sF64)tkclap_bpm;
      evTransport.song_pos_beats = (int64_t)tkclap_song_pos_beats;

      process.steady_time         = -1;  // n/a
      process.frames_count        = _numFrames;
      process.transport           = &evTransport;
      process.audio_inputs        = clap_audio_buffers_in;
      process.audio_outputs       = clap_audio_buffers_out;
      process.audio_inputs_count  = num_audio_in;
      process.audio_outputs_count = num_audio_out;
      process.in_events           = &input_events.clap_input_events;
      process.out_events          = &output_events.clap_output_events;

      Dprintf_verbose("[trc] CLAPPlugin::process: BEGIN numFrames=%u\n", _numFrames);
      // (note) status=1=CLAP_PROCESS_CONTINUE   see clap-main/include/clap/process.h
      clap_process_status status = plugin->process(plugin, &process);
      Dprintf_verbose("[trc] CLAPPlugin::process: END status=%d\n", status);

      input_events.num_ev = 0u;

      processOutputEvents();

      unlockEvents();
   }
}
