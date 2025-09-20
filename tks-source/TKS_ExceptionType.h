/// TKS_ExceptionType.h 
/// 
/// (c) 2006-2009 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 
#ifndef __TKS_EXCEPTIONTYPE_H__
#define __TKS_EXCEPTIONTYPE_H__

// (catchable) Core runtime exceptions
//
// + Error
//   + CriticalError
//     o Death
//     o InvalidPointer    
//
//     + TypeMismatch
//       + ClassTypeMismatch
//         o NativeClassTypeMismatch
//         o ScriptClassTypeMismatch
//
//     + NotFound
//       + ClassMethodNotFound
//         o NativeClassMethodNotFound
//         o ScriptClassMethodNotFound
//       + ClassMemberNotFound
//         o NativeClassMemberNotFound
//         o ScriptClassMemberNotFound
//       o ModuleNotFound
//       o ModuleMemberNotFound
//
//     + ArrayOutOfBounds
//       o ReadArrayOutOfBounds
//       o WriteArrayOutOfBounds
//
//     + ConstraintViolation
//       o NotNullConstraintViolation
//
//   + UncriticalError
//
//  
enum __tks_exception_types {
   TKS_EXCEPTION_ERROR = 0,                  // Base class for all exception types

   TKS_EXCEPTION_CRITICALERROR             , // Base exception for critical errors (better don't catch these)
   TKS_EXCEPTION_UNCRITICALERROR           , // Base exception for uncritical errors (e.g. for API calls whose return type already denotes an error)

   TKS_EXCEPTION_INVALIDPOINTER            , // Reference to dead object detected (better don't catch these)
   ////TKS_EXCEPTION_OUTOFMEMORY               , // Failed to allocate memory (better don't cath these)

   TKS_EXCEPTION_DEATH                     , // Statementblock terminated with "die" statement

   TKS_EXCEPTION_TYPEMISMATCH              , // 
   TKS_EXCEPTION_CLASSTYPEMISMATCH         , // Incompatible class types
   TKS_EXCEPTION_NATIVECLASSTYPEMISMATCH   , // Incompatible C++ YAC class types
   TKS_EXCEPTION_SCRIPTCLASSTYPEMISMATCH   , // Incompatible script class types
   ////TKS_EXCEPTION_DELEGATETYPEMISMATCH      , // trying to assign delegate method to incompatible type
   ////TKS_EXCEPTION_FOREACHTYPEMISMATCH       , // trying to iterate non-object type

   TKS_EXCEPTION_NOTFOUND                  , // Error while resolving run time element
   TKS_EXCEPTION_CLASSMETHODNOTFOUND       , // 
   TKS_EXCEPTION_NATIVECLASSMETHODNOTFOUND , // Native class method could not be resolved
   TKS_EXCEPTION_SCRIPTCLASSMETHODNOTFOUND , // Script class method could not be resolved
   TKS_EXCEPTION_CLASSMEMBERNOTFOUND       , // 
   TKS_EXCEPTION_NATIVECLASSMEMBERNOTFOUND , // Native class member could not be resolved
   TKS_EXCEPTION_SCRIPTCLASSMEMBERNOTFOUND , // Script class member could not be resolved
   TKS_EXCEPTION_MODULENOTFOUND            , // Cross referenced module could not be resolved 
   TKS_EXCEPTION_MODULEMEMBERNOTFOUND      , // Cross referenced module variable could not be resolved

   TKS_EXCEPTION_ARRAYOUTOFBOUNDS          , // Trying to access to non-existing array element 
   TKS_EXCEPTION_READARRAYOUTOFBOUNDS      , // Trying to read from non-existing array element
   TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS     , // Trying to write to non-existing array element

   TKS_EXCEPTION_CONSTRAINTVIOLATION       , // Base class for all constraint violations
   TKS_EXCEPTION_NOTNULLCONSTRAINTVIOLATION, // NOT NULL Argument constraint failed

   TKS_NUM_EXCEPTIONS
};


class TKS_CachedScript;
class TKS_FunctionCallStackEntry;
class TKS_ClassDecl;


class TKS_ExceptionType {
public:
   sBool              is_declared;      // 1=exception was actually declared, 0=exception was forward referenced
   sUI                src_loc;          // where this exception was first forward-referenced 
   TKS_ExceptionType *next_exception;
   TKS_ExceptionType *parent_exception; // base exception class or NULL
   sUI                exception_id;     // 
   TKS_CachedScript  *parent_module;    // module that declared this exception, NULL = core exception
   TKS_ClassDecl     *parent_class;     // class that declared this exception. NULL = global exception type
   ////TKS_ClassDecl     *suggested_class;  // if the parent_class is unknown, an undeclared exception could either be a global or a class exception. the suggested class first is searched first.
   YAC_String         name;

public:
   TKS_ExceptionType(void);
   TKS_ExceptionType(sUI _baseId, sUI _id, const char *_name);
   TKS_ExceptionType(TKS_ExceptionType *_baseType, YAC_String *_name);
   ~TKS_ExceptionType();

};


#endif // __TKS_EXCEPTIONTYPE_H__
