/// TKS_PluginCallback.h
///
/// (c) 2006-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_PLUGINCALLBACK_H__
#define __TKS_PLUGINCALLBACK_H__


class TKS_PluginCallback {
   //
   // This is used for "anonymous" callbacks between different C++/YAC plugins.
   //   (making the loading order irrelevant)
   // 
   // This feature is (still) EXPERIMENTAL.
   //
public:
   TKS_PluginCallback *next;
   sSI                 callback_id;
   YAC_String          name;
   YAC_CFunctionPtr    cdecl_function; // the argument list / return type depends on the plugin
};


#endif // __TKS_PLUGINCALLBACK_H__
