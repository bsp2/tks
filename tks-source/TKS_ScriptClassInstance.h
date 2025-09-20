/// TKS_ScriptClassInstance.h
///
/// (c) 2001-2020 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_SCRIPTCLASSINSTANCE_H__
#define __TKS_SCRIPTCLASSINSTANCE_H__


class PTN_ArgList; 

 
class TKS_ScriptClassInstance : public YAC_Object {
public:
   TKS_ClassDecl *class_decl;
   yacmem        *member_data;
   yacmemptr      custom_data; 
#ifdef TKS_MT
   TKS_Mutex     *mtx_this; // !=NULL if any method is synchronized to this ('==')
#endif //TKS_MT

#ifdef TKS_SCI_OBJECTCOUNTER
   static sSI object_counter;
#endif // TKS_SCI_OBJECTCOUNTER

    
   // ---- The delegate HashTable dynamically maps named "slots" to PTN_Functino references. 
   YAC_HashTable *delegates; 
    
   
public:
   TKS_ScriptClassInstance(void);
   ~TKS_ScriptClassInstance();
   
   void              YAC_VCALL yacOperator                    (sSI _i, YAC_Object *_o, YAC_Value *_r);
   void              YAC_VCALL yacOperatorInit                (void *_context, YAC_Object *_arg); // calls constructors
   void              YAC_VCALL yacOperatorAssign              (YAC_Object *_arg);
   void              YAC_VCALL yacSerialize                   (YAC_Object *, sUI);
   sUI               YAC_VCALL yacDeserialize                 (YAC_Object *, sUI);
   void              YAC_VCALL yacSerializeClassName          (YAC_Object *); 
   void              YAC_VCALL yacFinalizeObject              (void *_context); // calls destructors
   sBool             YAC_VCALL yacIsComposite                 (void) {return 1;}

   void clean (void);

   void              lazyInitMutexAndMemberData     (void *_context, TKS_ScriptClassInstance *_template);
////void              serializeClassMembers          (YAC_Object *, TKS_ClassDecl *);
   void              freeMemberData                 (void);
   TKS_CachedObject *getMember                      (YAC_String *_name);
   TKS_CachedObject *getMethod                      (YAC_String *_name); // search all ancestors and current
   TKS_CachedObject *getMethod                      (const char *_name); // search all ancestors and current
   PTN_Function     *getMethodFunctionByName        (const char *_name);
   yacmem           *getMemberAdrByID               (sUI _id);
   yacmem           *getIntMemberAdrByName          (const char *_name);
   void              getClassMemberValueByID        (YAC_Value *, sUI _id);
   void              getDerefClassMemberValueByID   (YAC_Value *, sUI _id);
   void              setClassMemberValueByID        (sUI _id, YAC_Value*_v, sU8 _op);
   void              setClassMemberPointerValueByID (sUI _id, YAC_Value*_v);
   void              getPostIncClassMemberValueByID (YAC_Value *, sUI _id);
   void              getPostDecClassMemberValueByID (YAC_Value *, sUI _id);
   void              getPreIncClassMemberValueByID  (YAC_Value *, sUI _id);
   void              getPreDecClassMemberValueByID  (YAC_Value *, sUI _id);
   void              postIncClassMemberValueByID    (sUI _id);
   void              postDecClassMemberValueByID    (sUI _id);
   void              callMemberInitializersPushSelf (PTN_Env *_env); 
   void              callMemberInitializers         (PTN_Env *_env); 
   void              callConstructors               (PTN_Env *_env);
   void              callDestructors                (PTN_Env *_env);
   sBool             initFromHashTable              (YAC_HashTable *_ht); 
   sBool             writeToHashTable               (YAC_HashTable *_ht); 
   
   sChar        *YAC_VCALL yacMetaClassName                      (void);
   sUI           YAC_VCALL yacMetaClassMemberGetNum              (void);
   sUI           YAC_VCALL yacMetaClassMemberGetAccessKeyByIndex (sUI _idx);
   sUI           YAC_VCALL yacMetaClassMemberGetAccessKeyByName  (const sChar *);
   sUI           YAC_VCALL yacMetaClassMemberGetType             (sUI _ak); // 1,2,3
   sChar        *YAC_VCALL yacMetaClassMemberGetName             (sUI _ak);
   void          YAC_VCALL yacMetaClassMemberSet                 (sUI _ak, YAC_Value*);
   void          YAC_VCALL yacMetaClassMemberGet                 (sUI _ak, YAC_Value*);
   sSI           YAC_VCALL yacMetaClassInstanceOf                (YAC_Object *); 
    
   sBool         YAC_VCALL yacToString(YAC_String *_r) const; 
   sBool         YAC_VCALL yacEquals(YAC_Object *_o); 
    
    
   void          setDelegate  (YAC_String *_name, PTN_Function *_fun); 
   sBool         evalDelegate (YAC_String *_name, PTN_ArgList *_args, sUI _srcLoc, PTN_Env *_env, YAC_Value *_r) const; 
   
};


#endif // __TKS_SCRIPTCLASSINSTANCE_H__
