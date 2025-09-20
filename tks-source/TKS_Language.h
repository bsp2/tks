/// TKS_Language.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef TKS_LANGUAGE_H__
#define TKS_LANGUAGE_H__

enum __tks_language_signals {
   TKS_SIG_SIGNAL       = 0,
   TKS_SIG_SEGV         = 1,
   TKS_SIG_SEGVCONTINUE = 2
};

class TKS_Language : public YAC_Object {
public:
   YAC_String s_versionString;

public:
   TKS_Language                       (void);
   ~TKS_Language                      ();

   void YAC_VCALL yacGetSignalStringList             (YAC_String *_sig);
   void YAC_VCALL yacRegisterSignal                  (sUI _id, YAC_FunctionHandle _f);

   YAC_String *constantToString            (sSI _const       , YAC_String *_strmatch                );
   YAC_String *scriptClassConstantToString (YAC_Object *_vo, YAC_Object *_sci, YAC_String *_strmatch);
   void        stringToConstant            (YAC_Object *     , YAC_Value*_r                         );
   YAC_Object *findScriptClass             (YAC_Object *_namespaceName, YAC_Object *_name           );
   void        findClassesByPrefix_YAC_RARG (YAC_Object *_nspaceName, YAC_Object *_prefix, YAC_Object *_r);
   void        findClassesByPrefix_YAC_RVAL (YAC_Object*_nspaceName, YAC_Object *_prefix, YAC_Value *_r);
   void        findClassesByBaseClass      (YAC_Object *_nspaceName, YAC_Object *_baseClass, YAC_Value *_r);
   void        getClassName                (YAC_Object*      , YAC_Value*_r                         );
   void        getNamespaceName            (YAC_Object *     , YAC_Value*_r                         );
   sSI         getVersion                  (void                                                    );
   YAC_String *getVersionString            (void                                                    );
   void        setIntPropertyByName        (YAC_Object *_o   , YAC_String *_name, sSI _value        );
   void        setFloatPropertyByName      (YAC_Object *_o   , YAC_String *_name, sF32 _value       );
   void        setObjectPropertyByName     (YAC_Object *_o   , YAC_String *_name, YAC_Object *_value);
   sSI         getPropertyAccessKeyByName  (YAC_Object *_o   , YAC_Object *_name                    );
   sSI         getPropertyAccessKeyByIndex (YAC_Object *_o   , sSI _idx                             );
   void        getPropertyByAccessKey      (YAC_Object *_o   , sSI _ak          , YAC_Value *_r     );
   void        getPropertyNameByAccessKey  (YAC_Object *_o   , sSI _ak          , YAC_Value *_r     );
   void        getPropertyByName           (YAC_Object *_o   , YAC_String *_name, YAC_Value *_r     );
   sSI         getNumProperties            (YAC_Object *_o                                          );
   void        getPropertyClassName        (YAC_Object *_o, YAC_Value *_r                           );
   void        newObjectByName             (YAC_Object *_nspaceName, YAC_Object *_name, YAC_Value *_r);
   void        newObjectArrayByName        (YAC_Object *_nspaceName, YAC_Object *_name, YAC_Value *_r);
   void        newObjectByID               (sSI              , YAC_Value *_r                        );
   sSI         getClassID                  (YAC_Object *_o                                          );
   sSI         evalMethodByName            (YAC_Object *_o, YAC_String *_name, YAC_Object *_arglist, YAC_Object *_retvalue);
   sSI         classToHashTable            (YAC_Object *_class, YAC_Object *_hashtable);
   sSI         hashTableToClass            (YAC_Object *_hashtable, YAC_Object *_class);
   void        getMethodArgumentNames      (YAC_Object *_nspaceName, YAC_Object *_classname, YAC_Object *_methodname, YAC_Value *_r);
   void        sleep                       (sUI _ms);
   void        fork                        (YAC_String *_chapterName, YAC_Value *_r); // xxx removeme
   void        _yield                      (void);
   void        getMethodNames              (YAC_Object *_class, YAC_Value *_r);
   sSI         _isBigEndian                (void);
   sBool       is64Bit                     (void);
   void        _reinstallDebugHandler      (void);
   void        getTksExePath               (YAC_Value *_r);
   void        getTksExeDir                (YAC_Value *_r);
   void        getDllPrefix                (YAC_Value *_r);
   void        getDllSuffix                (YAC_Value *_r);
   void        newThreadFromCurrent        (YAC_Value *_r);
   void        forceUnlockNamedMutex       (YAC_String *_name);
   void        unlockNamedMutex            (YAC_String *_name);
   void        lockNamedMutex              (YAC_String *_name);
   void        getTksPluginPath            (YAC_Value *_r);
   void        getTksModulePath            (YAC_Value *_r);
   void        getTksLibraryPath           (YAC_Value *_r);
   void        getTksApplicationPath       (YAC_Value *_r);
   void        getHomeDir                  (YAC_Value *_r);
   void        getCurrentDir               (YAC_Value *_r);
   void        getExtraFiles               (YAC_Value *_r);  // returns StringArray
   sBool       loadLocalTSL                (YAC_String *_libName);
   void        evalScript                  (YAC_String *_src, YAC_Value *_r);  // eval and return 'print' output string buffer
};

#endif  // TKS_LANGUAGE_H__
