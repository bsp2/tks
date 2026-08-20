/// TKS_PluginCallback.h
///
/// (c) 2006-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_PLUGINCALLBACK_H__
#define TKS_PLUGINCALLBACK_H__


class TKS_PluginCallback {
   //
   // This is used for "anonymous" callbacks between different C++/YAC plugins.
   //   (making the loading order irrelevant)
   //
public:
   TKS_PluginCallback *next;
   sSI                 callback_id;
   YAC_String          name;
   YAC_CFunctionPtr    cdecl_function; // the argument list / return type depends on the plugin
};


#endif // TKS_PLUGINCALLBACK_H__
