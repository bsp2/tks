/// TKS_ScriptModule.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///

#ifndef TKS_SCRIPTMODULE_H__
#define TKS_SCRIPTMODULE_H__


class TKS_ScriptModule : public YAC_Object {
public:
   sUI                      line_offset;
	void/*TKS_CachedScript*/*cscript;
   TKS_ScriptObjectID       script_id;
   TKS_Namespace           *class_nsp;
   sSI                      last_error_line_idx;  // -1=none, 0..65535
   YAC_String              *print_hook_output;  // or NULL

public:
	TKS_ScriptModule  (void);
	~TKS_ScriptModule ();

   sBool setClassNamespace          (YAC_String *_nspName);
   void  installPrintHook           (void);  // mirror stdout/stderr/.. prints to string buffer
   void  uninstallPrintHook         (void);
   void  setLineOffset              (sUI _offset);
	sSI   load                       (YAC_String *_s);
	void  unload                     (void);
   sSI   getLastErrorLineIndex      (void);
	void  findVariable               (YAC_String *_name, YAC_Value *_r);
	void  findTaggedVariableByIndex  (sSI _index, YAC_Value *_r);
	void  findFunction               (YAC_String *_name, YAC_Value *_r);
	void  eval                       (void);
   void  getPrintHookOutputAndClear (YAC_Value *_r);  // get copy of string buffer and empty it afterwards
};


#endif // TKS_SCRIPTMODULE_H__
