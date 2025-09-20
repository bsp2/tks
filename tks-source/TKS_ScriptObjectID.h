/// TKS_ScriptObjectID.h
///
/// (c) 2005-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef TKS_SCRIPTOBJECTID_H__
#define TKS_SCRIPTOBJECTID_H__


// objects that do *not* originate from temporary modules are always valid
#define TKS_SCRIPTID_ALWAYS_VALID 0x900df00d


typedef struct __s_scriptid {
   sUI magic;
   sUI id;
} s_scriptid;


class TKS_ScriptObjectID {
public:
   s_scriptid s_id;

public:
   TKS_ScriptObjectID(void);
   ~TKS_ScriptObjectID() { }

   sBool isValid(void);
};


#endif // TKS_SCRIPTOBJECTID_H__
