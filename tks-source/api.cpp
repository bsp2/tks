/// api.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///    published under terms of the GNU general public license (GPL).
///
/// Note: the ying_* files (except for ying_core_Object) are created with
///       $~/tks-source: tks ../yac/ying -ng -nc api.cpp
///       or
///       $~/tks-source: m yac
///       (with 'm' being an alias for e.g "make -j20 -f makefile.linux")
///
///       the ying_* files for regular plugins are usually created with
///       $~/myplugin: tks ../yac/ying *.h
///

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "tks.h"

#include "YAC_Boolean.h"
#include "YAC_Byte.h"
#include "YAC_Short.h"
#include "YAC_Integer.h"
#include "YAC_Long.h"
#include "YAC_UnsignedByte.h"
#include "YAC_UnsignedShort.h"
#include "YAC_UnsignedInteger.h"
#include "YAC_UnsignedLong.h"
#include "YAC_Float.h"
#include "YAC_Double.h"
#include "YAC_ValueObject.h"
#include "YAC_Event.h"
#include "YAC_IntArray.h"
#include "YAC_FloatArray.h"
#include "YAC_ListNode.h"
#include "YAC_TreeNode.h"
#include "YAC_ObjectArray.h"
#include "YAC_StringArray.h"
#include "YAC_ValueArray.h"
#include "YAC_PointerArray.h"
#include "YAC_HashTable.h"
#include "YAC_List.h"
#include "YAC_StreamBase.h"
#include "YAC_Buffer.h"
#include "YAC_File.h"
#include "YAC_StdInStream.h"
#include "YAC_StdOutStream.h"
#include "YAC_StdErrStream.h"
#include "TKS_StreamIO.h"
#include "YAC_PakFile.h"
#include "TKS_DummyStream.h"
#include "TKS_Process.h"
#include "TKS_LFSR.h"



// Thread safe object counting
#ifdef YAC_OBJECT_COUNTER
#ifdef TKS_MT
#include "TKS_Mutex.h"
TKS_Mutex *mtx_yac_object_counter = NULL;
#define Dyac_object_counter_inc \
   if(mtx_yac_object_counter) mtx_yac_object_counter->lock(); \
   YAC_Object::object_counter++; \
   if(mtx_yac_object_counter) mtx_yac_object_counter->unlock()
#define Dyac_object_counter_dec \
   if(mtx_yac_object_counter) mtx_yac_object_counter->lock(); \
   YAC_Object::object_counter--; \
   if(mtx_yac_object_counter) mtx_yac_object_counter->unlock()

#include "TKS_Condition.h"

#endif // TKS_MT
#endif // YAC_OBJECT_COUNTER

#include <yac_host.cpp>


YG("core");


#include "Pool.h"
#include "ObjectPool.h"

#include "tks_inc_class.h"
////#include "TKS_ClassDeclMethod.h"
////#include "TKS_ClassDeclMember.h"
////#include "TKS_ClassDecl.h"

#include "TKS_ConfigurationInter.h"
#include "YAC_ClassArray.h"
#include "TKS_ScriptClassArrayIterator.h"
////#include "TKS_ScriptClassInstance.h"
#include "TKS_ScriptObjectID.h"
#include "TKS_FunctionObject.h"
#include "TKS_ScriptVariable.h"
#include "TKS_ScriptModule.h"
#include "TKS_Envelope.h"
#include "TKS_Language.h"
#include "TKS_ListIterator.h"
#include "TKS_Pool.h"
#include "TKS_PoolIterator.h"
#include "TKS_Stack.h"
#include "TKS_StringIterator.h"
#include "TKS_StringStackIterator.h"
#include "TKS_Time.h"

#include "TKX_File.h"
#include "TKX_Chapter.h"
#include "TKX_PakFile.h"

#include "TKS_Token.h"
#include "TKS_TokenTable.h"
#include "TKS_TokenizedScript.h"

#include "PTN_Node.h"
#include "PTAnyNodePtr.h"
#include "TKS_StateStackEntry.h"
#include "TKS_CachedScript.h"
#include "TKS_Compiler.h"

#include "TKS_SharedBuffer.h"
#include "TKS_Mutex.h"
#include "TKS_MutexObject.h"
#include "TKS_Mailbox.h"
#include "TKS_Thread.h"


TKS_ClassDecl Class_ClassDecl;



/* @beginmodule core

The core TkScript API.

This is a set of classes and functions that are built into the TkScript engine.

Some of the classes are also available (as a stripped-down version) in the <code>YAC</code> plugin API.
*/

//------------------------------------------------------------------------------------- Object
/* @class Object

This is the baseclass for all scriptengine objects.

Please notice that normally you will not need to use these methods
directly but rather through specialized API classes like e.g. §File, §String, §Buffer
etc.
These methods are mostly useful when dealing with "generic" objects but please make
sure to also look at the §TKS class API.

*/

/* @method yacClassName:String

Return the class name.

Also see @yacMetaClassName.

@return new String instance that holds the class name
@see yacMetaClassName
*/


/* @method yacNew:Object

Return a new instance of this class and initializes it.

@return new Object
*/


/* @method yacNewObject:Object

Return a new instance of this class.

Warning: When instantiating script classes, yacOperatorInit() must be called afterwards.

Applications should call yacNew() instead of yacNewObject() since yacNew() takes care of script class initialization.

Alternatively, Object(myscriptclass) can be used to clone any object (including script classes).

@return new Object
*/


/* @method yacMemberGetNum:int

Return the number of C++ members.

@return Number of C++ members.
@see yacMemberGetNames
@see yacMemberGetObjectTypes
@see yacMemberGetOffsets
@see yacMemberGetTypes
*/

/* @method yacMemberGetNames:StringArray

Return the C++ member names.

@return New StringArray object that holds the member names.
@see yacMemberGetNum
@see yacMemberGetObjectTypes
@see yacMemberGetOffsets
@see yacMemberGetTypes
*/

/* @method yacMemberGetTypes:IntArray

Return the C++ member types.

The §Value type encoding is used, i.e. 0=void, 1=int, 2=float, 3=Object, 4=String.

@return New IntArray object that holds the member types.
@see yacMemberGetNames
@see yacMemberGetNum
@see yacMemberGetObjectTypes
@see yacMemberGetOffsets
*/

/* @method yacMemberGetObjectTypes:StringArray

Return the class names of the C++ object members.

If a member is not an Object, an empty String will be returned for that member.

@return New StringArray that holds the member class names.
@see yacMemberGetNames
@see yacMemberGetNum
@see yacMemberGetOffsets
@see yacMemberGetTypes
*/

/* @method yacMemberGetOffsets:IntArray

Return the byteoffsets of the C++ object members.

@return new IntArray instance that holds the member types
@see yacMemberGetNames
@see yacMemberGetNum
@see yacMemberGetObjectTypes
@see yacMemberGetTypes
*/

/* @method yacMethodGetNum:int

Return the number of C++ methods exposed to the scriptengine.

@return Number of C++ methods
@see yacMethodGetNames
@see yacMethodGetNumParameters
@see yacMethodGetParameterTypes
@see yacMethodGetParameterObjectTypes
@see yacMethodGetReturnTypes
@see yacMethodGetReturnObjectTypes
*/

/* @method yacMethodGetNames:StringArray

Return the C++ method names.

@return new StringArray object that holds the C++ method names.
@see yacMethodGetNum
@see yacMethodGetNumParameters
@see yacMethodGetParameterTypes
@see yacMethodGetParameterObjectTypes
@see yacMethodGetReturnTypes
@see yacMethodGetReturnObjectTypes
*/

/* @method yacMethodGetNumParameters:IntArray

Return the number of parameters to each C++ method exposed to the scriptengine,

@return new IntArray object that holds the parameter counter for each method.
@see yacMethodGetNum
@see yacMethodGetNames
@see yacMethodGetParameterTypes
@see yacMethodGetParameterObjectTypes
@see yacMethodGetReturnTypes
@see yacMethodGetReturnObjectTypes
*/

/* @method yacMethodGetParameterTypes:ValueArray

Return the parameter types arrays for each C++ method exposed to the scriptengine.

The §Value type encoding is used, i.e. 0=void, 1=int, 2=float, 3=Object, 4=String.

@return new ValueArray that holds the parameters types (IntArray) for each C++ method.
@see yacMethodGetNum
@see yacMethodGetNames
@see yacMethodGetNumParameters
@see yacMethodGetParameterObjectTypes
@see yacMethodGetReturnTypes
@see yacMethodGetReturnObjectTypes
*/

/* @method yacMethodGetParameterObjectTypes:ValueArray

Return the object parameter class names for each C++ method exposed to the scriptengine.

@return new ValueArray that holds the parameter class names (StringArray) for each C++ method.
@see yacMethodGetNum
@see yacMethodGetNames
@see yacMethodGetNumParameters
@see yacMethodGetParameterTypes
@see yacMethodGetReturnTypes
@see yacMethodGetReturnObjectTypes
*/

/* @method yacMethodGetReturnTypes:IntArray

Return the return types for each C++ method exposed to the scriptengine.

The §Value type encoding is used, i.e. 0=void, 1=int, 2=float, 3=Object with the exception
that the return type 4 means "variable".
The last parameter to a variable return type method must be <code>YAC_Value *_r</code>.
Methods that want to return <i>deletable</i> objects must also use return type 4.

@return new IntArray object that holds the return types for each C++ method.
@see yacMethodGetNum
@see yacMethodGetNames
@see yacMethodGetNumParameters
@see yacMethodGetParameterTypes
@see yacMethodGetParameterObjectTypes
@see yacMethodGetReturnObjectTypes
*/

/* @method yacMethodGetReturnObjectTypes:StringArray

Return the return object classnames for each C++ method exposed to the scriptengine.

@return new StringArray object that holds the return object classnames for each C++ method.
@see yacMethodGetNum
@see yacMethodGetNames
@see yacMethodGetNumParameters
@see yacMethodGetParameterTypes
@see yacMethodGetParameterObjectTypes
@see yacMethodGetReturnTypes
*/

/* @method yacConstantGetNum:int

Return the number of constants of a C++ class.

@return Number of constants
@see yacConstantGetNames
@see yacConstantGetTypes
@see yacConstantGetValues
*/

/* @method yacConstantGetNames:StringArray

Return the names of the C++ constants.

@return new StringArray instance that holds the constant names
@see yacConstantGetNum
@see yacConstantGetTypes
@see yacConstantGetValues
*/

/* @method yacConstantGetTypes:IntArray

Return the types of the C++ constants.

The §Value type encoding is used, i.e. 0=void, 1=int, 2=float, 3=Object, 4=String.

@return new IntArray that holds the constant types.
@see yacConstantGetNum
@see yacConstantGetNames
@see yacConstantGetValues
*/

/* @method yacConstantGetValues:ValueArray

Return the C++ constant values.

@return new ValueArray that holds the constant values.
@see yacConstantGetNum
@see yacConstantGetNames
@see yacConstantGetTypes
*/

/* @method yacCopy,Object o:boolean
Copy object data from "o".

@return true if copy succeded, false otherwise.
*/

/* @method yacEquals,Object o:boolean

Compare this to object "o".

@return true if objects are equals, false otherwise.
*/

/* @method yacOperator,int cmd,Object o:var

Invoke operator.

Usually this method returns nothing since the object is directly modified.
The scriptengine takes care of duplicating the lefthandside object (this)
if +,-,*,/ etc. commands are used.

@arg cmd Command, see <code>YAC_OP_xxx</code>
@arg o Righthandside object
@return Usually nothing, boolean if a comparison command was used (==, !=, ..)
@see yacOperatorI
@see yacOperatorF32
@see yacOperatorInit
@see yacOperatorAssign
@see yacOperatorAdd
@see yacOperatorSub
@see yacOperatorMul
@see yacOperatorDiv
@see yacOperatorClamp
@see yacOperatorWrap
*/

/* @method yacOperatorInit,Object o

Initialize object from other object.

This is mostly used for meta classes, e.g. to call constructors, initialize
members etc.

@arg o Template object
@see yacOperator
@see yacOperatorI
@see yacOperatorF32
@see yacOperatorAssign
@see yacOperatorAdd
@see yacOperatorSub
@see yacOperatorMul
@see yacOperatorDiv
@see yacOperatorClamp
@see yacOperatorWrap
*/

/* @method yacOperatorAssign,Object o

Assign other object "o" to this object.

@arg o Righthandside object
@see yacOperator
@see yacOperatorI
@see yacOperatorF32
@see yacOperatorInit
@see yacOperatorAdd
@see yacOperatorSub
@see yacOperatorMul
@see yacOperatorDiv
@see yacOperatorClamp
@see yacOperatorWrap
*/

/* @method yacOperatorAdd,Object o

Add other object "o" to this object.

@arg o Righthandside object
@see yacOperator
@see yacOperatorI
@see yacOperatorF32
@see yacOperatorInit
@see yacOperatorAssign
@see yacOperatorSub
@see yacOperatorMul
@see yacOperatorDiv
@see yacOperatorClamp
@see yacOperatorWrap
*/

/* @method yacOperatorSub,Object o

Subtract other object "o" from this object.

@arg o Righthandside object
@see yacOperator
@see yacOperatorI
@see yacOperatorF32
@see yacOperatorInit
@see yacOperatorAssign
@see yacOperatorAdd
@see yacOperatorSub
@see yacOperatorMul
@see yacOperatorDiv
@see yacOperatorClamp
@see yacOperatorWrap
*/

/* @method yacOperatorMul,Object o

Multiply this object by other object "o".

@arg o Righthandside object
@see yacOperator
@see yacOperatorI
@see yacOperatorF32
@see yacOperatorInit
@see yacOperatorAssign
@see yacOperatorAdd
@see yacOperatorSub
@see yacOperatorDiv
@see yacOperatorClamp
@see yacOperatorWrap
*/

/* @method yacOperatorDiv,Object o

Divide this object by other object "o".

@arg o Righthandside object
@see yacOperator
@see yacOperatorI
@see yacOperatorF32
@see yacOperatorInit
@see yacOperatorAssign
@see yacOperatorAdd
@see yacOperatorSub
@see yacOperatorMul
@see yacOperatorClamp
@see yacOperatorWrap
*/

/* @method yacOperatorClamp,Object min,Object max

Clamp this object to the given range.

@arg min Minimum
@arg max Maximum
@see yacOperator
@see yacOperatorI
@see yacOperatorF32
@see yacOperatorInit
@see yacOperatorAssign
@see yacOperatorAdd
@see yacOperatorSub
@see yacOperatorMul
@see yacOperatorDiv
@see yacOperatorWrap
*/

/* @method yacOperatorWrap,Object min,Object max

Wrap this object within the given range.

@arg min Minimum
@arg max Maximum
@see yacOperator
@see yacOperatorI
@see yacOperatorF32
@see yacOperatorInit
@see yacOperatorAssign
@see yacOperatorAdd
@see yacOperatorSub
@see yacOperatorMul
@see yacOperatorDiv
@see yacOperatorClamp
*/

/* @method yacScanI32,Integer r:boolean

Convert this object to a 32bit integer and write to return
object "r".

@arg r Receives the 32bit integer value
@return true if the conversion succeeded, false otherwise.
@see yacValueOfI
@see yacValueOfF32
@see yacScanI64
@see yacScanF32
@see yacScanF64
@see yacToString
*/

/* @method yacScanI64,Long r:boolean

Convert this object to a 64bit integer and write to return
object "r".

@arg r Receives the 64bit integer value
@return true if the conversion succeeded, false otherwise.
@see yacValueOfI
@see yacValueOfF32
@see yacScanI32
@see yacScanF32
@see yacScanF64
@see yacToString
*/

/* @method yacScanF32,Float r:boolean

Convert this object to a 32bit float and write to return
object "r".

@arg r Receives the 32bit float value
@return true if the conversion succeeded, false otherwise.
@see yacValueOfI
@see yacValueOfF32
@see yacScanI32
@see yacScanI64
@see yacScanF64
@see yacToString
*/

/* @method yacScanF64,Double r:boolean

Convert this object to a 64bit float and write to return
object "r".

@arg r Receives the 64bit float value
@return true if the conversion succeeded, false otherwise.
@see yacValueOfI
@see yacValueOfF32
@see yacScanI32
@see yacScanI64
@see yacScanF32
@see yacToString
*/

/* @method yacToString,String r:boolean

Convert this object to a String and write to return object "r".

@arg r Receives the String value
@return true if the conversion succeeded, false otherwise.
@see yacValueOfI
@see yacValueOfF32
@see yacScanI32
@see yacScanI64
@see yacScanF32
@see yacScanF64
*/

/* @method yacToParsableString,String r:boolean

Convert this object to a script-parsable String and write to return object "r".

@arg r Receives the String value
@return true if the conversion succeeded, false otherwise.
@see yacValueOfI
@see yacValueOfF32
@see yacScanI32
@see yacScanI64
@see yacScanF32
@see yacScanF64
*/

/* @method yacOperatorI,int cmd,int i:var

Invoke integer operator.

Also see §yacOperator

@arg cmd Command, see <code>YAC_OP_xxx</code>
@arg i Righthandside integer value
@return Usually nothing, boolean if a comparison command was used (==, !=, ..)
@see yacOperator
@see yacOperatorF32
@see yacOperatorInit
@see yacOperatorAssign
@see yacOperatorAdd
@see yacOperatorSub
@see yacOperatorMul
@see yacOperatorDiv
@see yacOperatorWrap
@see yacOperatorClamp
*/

/* @method yacOperatorF32,int cmd,float f:var

Invoke float operator.

Also see §yacOperator

@arg cmd Command, see <code>YAC_OP_xxx</code>
@arg f Righthandside float value
@return Usually nothing, boolean if a comparison command was used (==, !=, ..)
@see yacOperator
@see yacOperatorI
@see yacOperatorInit
@see yacOperatorAssign
@see yacOperatorAdd
@see yacOperatorSub
@see yacOperatorMul
@see yacOperatorDiv
@see yacOperatorWrap
@see yacOperatorClamp
*/

/* @method yacValueOfI,int i

Initialize from 32bit integer value.

@arg i Integer value
@see yacValueOfF32
@see yacScanI32
@see yacScanI64
@see yacScanF32
@see yacScanF64
@see yacToString
*/

/* @method yacValueOfF32,float f

Initialize from 32bit float value.

@arg f Float value
@see yacValueOfI
@see yacScanI32
@see yacScanI64
@see yacScanF32
@see yacScanF64
@see yacToString
*/

/* @method yacIsStream:boolean

Check whether object is a §Stream.

@return true if the object implements the stream interface, false otherwise.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamClose

Close the stream.

@see yacIsStream
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamOpenLocal,String name,int access:boolean

Open a local file for streaming.

@arg name Local filename
@arg access 0=read, 1=write, 2=read-write
@return true if the file has been opened, false otherwise.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamOpenLogic,String name:boolean

Open a file in the current PAK (.tkx) file for streaming.

@arg name Logic filename
@return true if the file has been opened, false otherwise.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamGetByteOrder:int

Query the current byteorder.

@return Byteorder, <code>YAC_LITTLEENDIAN</code> (default) or <code>YAC_BIGENDIAN</code>.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamSetByteOrder,int byteorder

Set the stream byteorder.

@arg byteorder <code>YAC_LITTLEENDIAN</code> (default) or <code>YAC_BIGENDIAN</code>.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamEOF:boolean

Check whether the end of stream has been reached.

@return true if EOF was found, false otherwise.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamSeek,int off,int mode

Adjust stream offset.

@arg off Stream offset
@arg mode Seek mode. <code>YAC_BEG</code> (0), <code>YAC_CUR/<code> (1) or <code>YAC_END</code> (2)
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamGetOffset:int

Return the current stream offset.

@return Stream offset
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamSetOffset,int off

Set the current stream offset.

@arg off Stream offset
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamGetSize:int

Query the total stream size.

This obviously will only work for limited-size streams like e.g. §File.

@return Stream size or 0 (unknown)
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamRead,Buffer r,int num:int

Read "num" bytes from the stream into §Buffer r.

@arg r Destination §Buffer
@arg num Number of bytes to read
@return Number of bytes read from §Stream.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamReadI8:byte

Read a single byte from the stream.

@return Byte
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamReadI16:short

Read a single 16bit short from the stream.

Perform byteorder conversion if necessary.

@return Short
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamReadI32:short

Read a single 32bit integer from the stream.

Perform byteorder conversion if necessary.

@return integer value
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadI16
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamReadI64:Long

Read a single 64bit integer from the stream.

Perform byteorder conversion if necessary.

@return integer value
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamReadF32:float

Read a single 32bit float from the stream.

@return float value
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamReadF64:Double

Read a single 64bit float from the stream.

@return float value
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamReadObject,Object r

Read a single object from the stream and write to "r".

@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamReadString,String r:boolean

Read a single String from the stream and write to "r".

@return true if the String was read, false otherwise.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamReadBuffer,Buffer r,int off,int num,boolean resize:int

Read a sequence of bytes from the stream and write to §Buffer "r".

@arg r Destination §Buffer
@arg off Destination offset in buffer
@arg num Number of bytes to read
@arg resize If true, resize the §Buffer as required
@return Number of bytes read
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamReadLine,String r,int maxlen:int

Read a String from the stream and write to §String "r".

@arg r Destination §String
@arg maxlen Maximum number of chars to read
@return Number of chars read.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamWrite,Buffer b,int num:int

Write a sequence of bytes to the stream.

@arg b Source buffer
@arg num Number of bytes to write
@return Number of bytes written
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamWriteI8,byte b

Write a single byte to the stream.

@arg b Byte value to write
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamWriteI16,short s

Write a single 16bit short to the stream.

Automatic byteorder conversion is performed according
to the current stream byteorder.

@arg s Short value to write.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamWriteI32,int i

Write a single 32bit integer to the stream.

Automatic byteorder conversion is performed according
to the current stream byteorder.

@arg s Integer value to write.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI8
@see yacStreamWriteI16
@see yacStreamWriteI64
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamWriteI64,Long i

Write a single 64bit integer to the stream.

Automatic byteorder conversion is performed according
to the current stream byteorder.

@arg s Integer value to write.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI8
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamWriteF32,float f

Write a single 32bit float to the stream.

@arg f Float value to write.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamWriteF64,Double f

Write a single 64bit float to the stream.

@arg f Float value to write.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamWriteString,String s,int off,int len:int

Write a sequence of characters to the stream.

@arg s Source §String
@arg off Source offset
@arg len Number of characters to write
@return Number of characters written
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamWriteBuffer,Buffer b,int off,int len:int

Write a sequence of bytes to the stream.

@arg b Source §Buffer
@arg off Source offset
@arg len Number of bytes to write
@return Number of bytes written
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamGetErrorCode:int

Return the last stream error code.

@return Error code. Except for 0=No error, the error code depends on the stream type.
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorStringByCode
*/

/* @method yacStreamGetErrorStringByCode,int errorCode:String

Returns a String representation of the given error code.

@arg errorCode The stream error code
@return new String object
@see yacIsStream
@see yacStreamClose
@see yacStreamOpenLocal
@see yacStreamOpenLogic
@see yacStreamGetByteOrder
@see yacStreamSetByteOrder
@see yacStreamEOF
@see yacStreamSeek
@see yacStreamGetOffset
@see yacStreamSetOffset
@see yacStreamGetSize
@see yacStreamRead
@see yacStreamReadBuffer
@see yacStreamReadF32
@see yacStreamReadF64
@see yacStreamReadI16
@see yacStreamReadI32
@see yacStreamReadI64
@see yacStreamReadI8
@see yacStreamReadLine
@see yacStreamReadObject
@see yacStreamReadString
@see yacStreamWrite
@see yacStreamWriteBuffer
@see yacStreamWriteF32
@see yacStreamWriteF64
@see yacStreamWriteI16
@see yacStreamWriteI32
@see yacStreamWriteI64
@see yacStreamWriteI8
@see yacStreamWriteString
@see yacStreamGetErrorCode
*/

/* @method yacSerializeClassName,Stream ofs

Serialize the class name String to the given outputstream.

@arg ofs Output stream
@see yacCanDeserializeClass
@see yacDeserialize
@see yacSerialize
*/

/* @method yacSerialize,Stream ofs,boolean bTypeInfo

Serialize this object to the given outputstream.

@arg ofs Output stream
@arg bTypeInfo true to include type information, false to serialize just the raw object data
@see yacCanDeserializeClass
@see yacDeserialize
@see yacSerializeClassName
*/

/* @method yacDeserialize,Stream ifs,boolean bTypeInfo

Deserialize object from the given inputstream.

@arg ifs Input stream
@arg bTypeInfo true to expect type information, false to serialize just the raw object data
@see yacCanDeserializeClass
@see yacSerialize
@see yacSerializeClassName
*/

/* @method yacArrayNew:Object

Return new array object for this object.

This is used to create specialized Array objects for a given object type.

@return new array object
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayAlloc,int sx,int sy,int type,int elementByteSize:boolean

Allocate array elements.

Most arrays will only use the first argument ("sx").

@arg sx Array width
@arg sy Array height (e.g. for texture objects).
@arg type Array element type (see §Value type encoding)
@arg elementByteSize Size of an array element
@return true if elements were allocated successfully, false otherwise.
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayRealloc,int sx,int sy,int type,int elementByteSize:boolean

Re-allocate array elements.

Most arrays will only use the first argument ("sx").

@arg sx Array width
@arg sy Array height (e.g. for texture objects).
@arg type Array element type (see §Value type encoding)
@arg elementByteSize Size of an array element
@return true if elements were allocated successfully, false otherwise.
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayGetNumElements:int

Return the number of used array elements.

@return Number of used elements
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayGetMaxElements:int

Return the total number of array elements.

@return Total number of elements.
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayCopySize,Object o

Copy array size from object "o".

@arg o Array object to copy the size from
@see yacArrayAlloc
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArraySet,int index,Value v

Set array element.

@arg index Array index
@arg v New element value
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayGet,int index,Value r

Get array element

@arg index Array index
@arg r Receives the element value
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayGetDeref,int index,Value r

Get (deletable) array element.

If the array stores a deletable object, this method
will flag the element as non-deletable and return the deletable
object.


@arg index Array index
@arg r Receives the element value
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayGetWidth:int

Return the array width.

@return Array width
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayGetHeight:int

Return the array height.

@return Array height
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayGetElementType:int

Return the array element type.

@return Element type. The type is encoded using the §Value type encoding except that 0 means variable type.
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayGetElementByteSize:int

Return the size of a single array element.

@return Element byte size or 0 if the size is unknown.
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArrayGetStride:int

Return the number of bytes between two consecutive lines.

This is mostly used by e.g. the texture class (see tkopengl plugin).

@return Number of bytes per row.
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArraySetWidth,int sx

Set the array width.

@arg sx New array width
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetTemplate
*/

/* @method yacArraySetHeight,int sy

Set the array height.

@arg sy New array height
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetTemplate
@see yacArraySetWidth
*/

/* @method yacArraySetTemplate,Object template

Set the template object to use for allocation of new array elements.

This is mostly used by e.g. the §ObjectArray or §ClassArray classes.

@arg template The new array element template object
@see yacArrayAlloc
@see yacArrayCopySize
@see yacArrayGet
@see yacArrayGetDeref
@see yacArrayGetElementByteSize
@see yacArrayGetElementType
@see yacArrayGetHeight
@see yacArrayGetMaxElements
@see yacArrayGetNumElements
@see yacArrayGetStride
@see yacArrayGetWidth
@see yacArrayNew
@see yacArrayRealloc
@see yacArraySet
@see yacArraySetHeight
@see yacArraySetWidth
*/

/* @method yacHashSet,String key,Value value

Modify/create a hashtable entry.

@arg key The hash key name
@arg value The element value
@see yacHashGet
@see yacHashGetDeref
*/

/* @method yacHashGet,String key,Value r

Get a hashtable entry.

@arg key The hash key name
@arg r Receives the hashtable entry value.
@see yacHashSet
@see yacHashGetDeref
*/

/* @method yacHashGetDeref,String key,Value r

Get (deletable) hashtable element.

If the hashtable stores a deletable object, this method
will flag the element as non-deletable and return the deletable
object.

@arg key The hash key name
@arg r Receives the hashtable entry value
@see yacHashSet
@see yacHashGet
@see yacHashGetDeref
*/

/* @method yacGetSignalStringList:String

Return a String that describe the signal names and callback signatures
exposed by a C++ object.
For an example, take a look at the tksdl plugin.

@return Signal string
*/

/* @method yacMetaClassName:String

Return object meta class name.

This is mostly used with script classes. See §Class.

@return Meta class name
@see yacClassName
@see yacMetaClassInstanceOf
@see yacMetaClassMemberGet
@see yacMetaClassMemberGetAccessKeyByIndex
@see yacMetaClassMemberGetAccessKeyByName
@see yacMetaClassMemberGetName
@see yacMetaClassMemberGetNum
@see yacMetaClassMemberGetType
@see yacMetaClassMemberSet
@see yacMetaClassName
*/

/* @method yacMetaClassMemberGetNum:int

Return number of meta class members.

@return Number of meta class members
@see yacMetaClassInstanceOf
@see yacMetaClassMemberGet
@see yacMetaClassMemberGetAccessKeyByIndex
@see yacMetaClassMemberGetAccessKeyByName
@see yacMetaClassMemberGetName
@see yacMetaClassMemberGetType
@see yacMetaClassMemberSet
@see yacMetaClassName
*/

/* @method yacMetaClassMemberGetAccessKeyByIndex,int index:int

Return access key to meta class member by index.

@arg index Member index
@return Access key
@see yacMetaClassInstanceOf
@see yacMetaClassMemberGet
@see yacMetaClassMemberGetAccessKeyByName
@see yacMetaClassMemberGetName
@see yacMetaClassMemberGetNum
@see yacMetaClassMemberGetType
@see yacMetaClassMemberSet
@see yacMetaClassName
*/

/* @method yacMetaClassMemberGetAccessKeyByName,String name:int

Return access key to meta class member by name.

@arg name Member name
@return Access key
@see yacMetaClassInstanceOf
@see yacMetaClassMemberGet
@see yacMetaClassMemberGetAccessKeyByIndex
@see yacMetaClassMemberGetName
@see yacMetaClassMemberGetNum
@see yacMetaClassMemberGetType
@see yacMetaClassMemberSet
@see yacMetaClassName
*/

/* @method yacMetaClassMemberGetType,int ak

Return meta class member type.

The member type is encoded using the §Value type encoding.

@arg ak Access key
@return Member type
@see yacMetaClassInstanceOf
@see yacMetaClassMemberGet
@see yacMetaClassMemberGetAccessKeyByIndex
@see yacMetaClassMemberGetAccessKeyByName
@see yacMetaClassMemberGetName
@see yacMetaClassMemberGetNum
@see yacMetaClassMemberSet
@see yacMetaClassName
*/

/* @method yacMetaClassMemberGetName,int ak:String

Return name of meta class member associated with the given access key.

@arg ak Access key
@return Meta class member name
@see yacMetaClassInstanceOf
@see yacMetaClassMemberGet
@see yacMetaClassMemberGetAccessKeyByIndex
@see yacMetaClassMemberGetAccessKeyByName
@see yacMetaClassMemberGetNum
@see yacMetaClassMemberGetType
@see yacMetaClassMemberSet
@see yacMetaClassName
*/

/* @method yacMetaClassMemberSet,int ak,Value v

Set value of meta class member.

@arg ak Access key
@arg v New value of meta class member
@see yacMetaClassInstanceOf
@see yacMetaClassMemberGet
@see yacMetaClassMemberGetAccessKeyByIndex
@see yacMetaClassMemberGetAccessKeyByName
@see yacMetaClassMemberGetName
@see yacMetaClassMemberGetNum
@see yacMetaClassMemberGetType
@see yacMetaClassName
*/

/* @method yacMetaClassMemberGet,int ak:var

Return value of meta class member.

@arg ak Access key
@return Member value
@see yacMetaClassInstanceOf
@see yacMetaClassMemberGetAccessKeyByIndex
@see yacMetaClassMemberGetAccessKeyByName
@see yacMetaClassMemberGetName
@see yacMetaClassMemberGetNum
@see yacMetaClassMemberGetType
@see yacMetaClassMemberSet
@see yacMetaClassName
*/

/* @method yacMetaClassInstanceOf,Object o:boolean

Check whether this object is an instance of meta class "o".

@arg o Righthandside object
@return true if this is an instance of meta class "o", false otherwise.
@see yacInstanceOf
@see yacMetaClassMemberGet
@see yacMetaClassMemberGetAccessKeyByIndex
@see yacMetaClassMemberGetAccessKeyByName
@see yacMetaClassMemberGetName
@see yacMetaClassMemberGetNum
@see yacMetaClassMemberGetType
@see yacMetaClassMemberSet
@see yacMetaClassName
*/

/* @method yacCanDeserializeClass,Stream ifs:boolean

Check whether this class can be deserialized from the given inputstream.

@arg ifs Input stream.
@return true If this object can be deserialized from the stream, false otherwise.
@see yacDeserialize
@see yacSerialize
@see yacSerializeClassName
*/

/* @method yacInstanceOf,Object o:boolean

Check whether this C++ class is an instance of C++ class "o".

@arg o Righthandside C++ object
@return true if this is an instance of the C++ object/class "o", false otherwise
@see yacMetaClassInstanceOf
*/



#include "ying_core_Object.cpp"
// ---- see yac/yac.h for interface declaration!
YAC_C_CORE(YAC_Object, "Object", YAC_CLID_OBJECT);

//------------------------------------------------------------------------------------- IntArray
/* @class IntArray,Object

An arraylist of 32bit integer values.

@see IntArray8
@see IntArray16
@see IntArray32
@see IntArray64
@see IntArray128
*/


YC class _IntArray : public YAC_IntArray {
public:
   YAC_POOLED(_IntArray, YAC_POOL_PRIORITY_MEDIUM);

/* @method alloc,int max:int

Allocate the given number of array elements.

The numElements member will be set to 0 after allocation and the maxElements member
will be set to "max".

@arg max Total number of elements to allocate
@return true if the allocation succeeded, false otherwise
@see free
@see realloc
*/
   YM //sBool alloc          (sUI);

/* @method free

Free array elements.
@see alloc
@see realloc
*/
   YM //void free           (void);


/* @method add,int i:boolean

Add the given integer value to this array.

Resize array if necessary.

@arg f Integer value
@return true if the value has been added successfully, false otherwise.
@see add2
@see add3
@see add4
@see delete
@see delete2
@see delete3
@see delete4
@see insert
*/
   YM //sBool add           (sSI _val);


/* @method add2,int i1,int i2:boolean

Add the given integer values to this array.

Resize array if necessary.

@arg i1 First integer value
@arg i2 Second integer value
@return true if the values have been added successfully, false otherwise.
@see add
@see add3
@see add4
@see delete
@see delete2
@see delete3
@see delete4
@see insert
*/
   YM //sBool add2          (sSI _val1, sSI _val2);


/* @method add3,int i1,int i2,int i3:boolean

Add the given integer values to this array.

Resize array if necessary.

@arg i1 First integer value
@arg i2 Second integer value
@arg i3 Third integer value
@return true if the values have been added successfully, false otherwise.
@see add
@see add2
@see add4
@see delete
@see delete2
@see delete3
@see delete4
@see insert
*/
   YM //sBool add3          (sSI _val1, sSI _val2, sSI _val3);


/* @method add4,int i1,int i2,int i3,int i4:boolean

Add the given integer values to this array.

Resize array if necessary.

@arg i1 First integer value
@arg i2 Second integer value
@arg i3 Third integer value
@arg i4 Fourth integer value
@return true if the values have been added successfully, false otherwise.
@see add
@see add2
@see add3
@see delete
@see delete2
@see delete3
@see delete4
@see insert
*/
   YM //sBool add4          (sSI _val1, sSI _val2, sSI _val3, sSI _val4);



/* @method addArray,Object array,int off,int len:boolean

Add the given array like object to this array.

Resize array if necessary.

@arg array Array-like object
@arg off Source offset
@arg len Number of elements
@return true if the array has been added successfully, false otherwise.
@see add
@see addEmpty
*/
   YM //sBool addArray      (YAC_Object *_a, sUI _off, sUI _len) {


/* @method addEmpty,int num:int

Increase numElements by "num".

@arg num Number of elements to add
@return New number of elements
@see delete
@see insert
*/
   YM //sSI  addEmpty       (sUI);

/* @method getNumElements:int

Return number of used array elements.

@return Number of used array elements
@see setNumElements
*/
   YM //sSI  getNumElements (void);

/* @method getMaxElements:int

Return total number of array elements.

@return Total number of elements
@see alloc
@see realloc
*/
   YM //sSI  getMaxElements (void);

/* @method empty

Reset the number of used elements, numElements, to 0.
*/
   YM //void empty          (void);

/* @method isEmpty:boolean

Check if array is empty (numElements == 0).
*/
   YM //sBool isEmpty       (void);

/* @method realloc,int num:boolean

Reallocate array.

Array elements will be discarded if the new array size is smaller
than the current size. New array elements are added if the new array
size is smaller than the current size.

@arg num New total number of elements
@return true when the allocation succeeded, false otherwise.
@see alloc
@see free
*/
   YM //sBool realloc       (sUI);

/* @method insert,int index,int i:boolean

Insert new array element "i" before index "index".

The array will be resized if necessary.

@arg index Where to insert the new element
@arg i New element value
@see add
*/
   YM //sSI  insert         (sSI, sSI);

/* @method delete,int index:boolean
Delete element at the given index.

@arg index Which element to delete
@return true if the element was deleted, false otherwise.
@see add
@see insert
*/
   YM //sBool _delete       (sSI _index);


/* @method delete2,int index:boolean
Delete two elements at the given index.

@arg index First element index
@return true if the element has been deleted, false otherwise.
@see add
@see insert
*/
   YM //sBool _delete2      (sSI _index);


/* @method delete3,int index:boolean
Delete three elements at the given index.

@arg index First element index
@return true if the element has been deleted, false otherwise.
@see add
@see insert
*/
   YM //sBool _delete3      (sSI _index);


/* @method delete4,int index:boolean
Delete four elements at the given index.

@arg index First element index
@return true if the element has been deleted, false otherwise.
@see add
@see insert
*/
   YM //sBool _delete4      (sSI _index);


/* @method fill,int i

Set all used array elements to value "i".

@arg i Fill value
@see add
@see insert
*/
   YM //void fill           (sSI);

/* @method allocAndFill,int num,int i:boolean

Allocate and set all used array elements to value "i".

@arg num Number of elements
@arg i Fill value
@return true when allocation succeeded
@see fill
@see alloc
*/
   YM //sBool allocAndFill  (sSI, sSI);


/* @method fillRegion,int off,int len,int val
Fill a range of elements with a constant value.

The region start/end will be clipped to (0;numElements).

@arg off Start offset (element index)
@arg len Number of elements
@arg val Fill value
@see fill
*/
   YM //void fillRegion   (sSI _off, sSI _len, sSI _val);


/* @method reverse

Reverse element order.
*/
   YM //void reverse        (void);

/* @method setNumElements,int num

Set number of used elements to "num".

"num" is clamped to 0 resp. to the maxElements member.
@arg num New number of used elements
*/
   YM //void setNumElements (sSI);

/* @method swapByteOrder

Swap byteorder of all elements.
*/
   YM //void swapByteOrder  (void);

/* @method visitBytes,Object src,int off,int num:boolean

Create a view into another array-like object.

Keep in mind not to delete the "src" array while this object is still alive!

@arg src Source array object. Must have integer element type and linear memory layout (i.e. stride=width*elementByteSize)
@arg off Source offset
@arg num Number of elements
@return true if the view was created, false otherwise.
*/
   YM //sBool visitBytes    (YAC_Object *, sSI, sSI);

/* @method copyFrom,IntArray src,int off,int len,int destOff

Copy elements from other array.

@arg src Source array
@arg off Source offset
@arg len Number of elements to copy
@arg destOff Destination offset in this array
*/
   YM //void copyFrom       (YAC_Object *_fa, sSI _off, sSI _len, sSI _doff);

/* @method indexOf,int i,int off:int

Return index of element with value "i". -1 if no matching element was found.

@arg i Value to look for
@arg off Start index
@return Index of element, -1 if not found.
*/
   YM //sSI  indexOf        (sSI, sSI);

/* @method indexOfNot,int i,int off:int

Return index of element that does not have value "i". -1 if no matching element was found.

@arg i Value to look for
@arg off Start index
@return Index of element, -1 if not found.
*/
   YM //sSI  indexOfNot     (sSI, sSI);

/* @method contains,int i:boolean
Check whether array contains the given integer value.

@arg i Value to look for
@return true(1) if this array contains the given value, false(0) otherwise.
*/
   YM //sBool _contains      (sSI);


/* @method remove,int i:boolean
Check whether array contains the given integer value and remove it if it does.

@arg i Value to look for
@return true(1) if this array contained the given value which is now removed, false(0) otherwise.
*/
   YM //sBool _remove        (sSI);


/* @method read8,Stream ifs,int num,int destOff:int

Read a sequence of bytes from the given input stream.

The bytes will be extended to 32bit integers.

@arg ifs Input stream
@arg num Number of bytes to read.
@arg destOff Destination offset in this array
@return Number of elements read
*/
   YM //sUI  _read8         (YAC_Object *_st, sUI _num, sUI _doff);

/* @method read16,Stream ifs,int num,int destOff:int

Read a sequence of 16bit short integers from the given input stream.

The shorts will be extended to 32bit integers.

@arg ifs Input stream
@arg num Number of bytes to read.
@arg destOff Destination offset in this array
@return Number of elements read
*/
   YM //sUI  _read16        (YAC_Object *_st, sUI _num, sUI _doff);

/* @method read32,Stream ifs,int num,int destOff:int

Read a sequence of 32bit integers from the given input stream.

@arg ifs Input stream
@arg num Number of bytes to read.
@arg destOff Destination offset in this array
@return Number of elements read
*/
   YM //sUI  _read32        (YAC_Object *_st, sUI _num, sUI _doff);

/* @method write8,Stream ifs,int num,int srcOff:int

Write a sequence of 8bit bytes to the given output stream.

The array elements will be truncated to 8bit.

@arg num Number of bytes to write
@arg srcOff Source offset in this array
@return Number of elements written
*/
   YM //sUI  _write8        (YAC_Object *_st, sUI _num, sUI _doff);

/* @method write16,Stream ifs,int num,int srcOff:int

Write a sequence of 16bit short integers to the given output stream.

The array elements will be truncated to 16bit.

@arg num Number of shorts to write
@arg srcOff Source offset in this array
@return Number of elements written
*/
   YM //sUI  _write16       (YAC_Object *_st, sUI _num, sUI _doff);

/* @method write32,Stream ifs,int num,int srcOff:int

Write a sequence of 32bit integers to the given output stream.

@arg num Number of shorts to write
@arg srcOff Source offset in this array
@return Number of elements written
*/
   YM //sUI  _write32       (YAC_Object *_st, sUI _num, sUI _doff);

/* @method getString:String

Return string representation of this array.

@return String representation of this array.
*/
   YM //void _getString     (YAC_Value *_r);

/* @method getString2:String

Return (x;y) pair string representation of this array.

@return new String
*/
   YM //void _getString2    (YAC_Value *_r);

/* @method getString3:String

Return (x;y;z) tuple string representation of this array.

@return new String
*/
   YM //void _getString3    (YAC_Value *_r);

/* @method getMin:int

Return smallest array element.

@return Minimum value
*/
   YM //sSI  getMin         (void);

/* @method getMax:int

Return biggest array element.

@return Maximum value
*/
   YM //sSI  getMax         (void);

/* @method getAbsMin:int

Return smallest array element, not regarding sign.

@return Minimum value
*/
   YM //sSI  getAbsMin      (void);

/* @method getAbsMax:int

Return biggest array element, not regarding sign.

@return Maximum value
*/
   YM //sSI  getAbsMax      (void);

/* @method getMean:float

Return average element value.

@return Average value
*/
   YM //sF32 getMean        (void);

/* @method getAbsMean:float

Return average element value, not regarding sign.

@return Average value
*/
   YM //sF32 getAbsMean     (void);

/* @method get,int index:int

Return element at index "index".

Silently return 0 if index is out of bounds.

@arg index Array index
@return Element value
*/
   YM //sSI  get            (sSI);

/* @method set,int index,int val

Set element at "index" to "val".
Updates numElements when index exceeds it (>=).

Silently do nothing when index exceed array size (>= maxElements).

@arg index Array index
@arg val New value
*/
   YM //void  _set           (sUI, sSI);

/* @method getRev,int index:int

Return element value at (numElements - 1 - index).

Silently return 0 if resulting index is out of bounds.

@arg index Which element to return (starting from numElements)
@return Element value
*/
   YM //int   _getRev       (sSI);

/* @method setRev,int index,int val

Set element at "numElements - 1 - index" to "val".

Silently do nothing when index exceed array size (<0 || >= maxElements).

@arg index Array index (starting from numElements)
@arg val New value
*/
   YM //void  _setRev       (sSI, sSI);

/* @method getDefault,int index,int def:int

Return array element at 'index', or return 'def' when index is out of bounds.

@arg index Array index
@arg def Default value (when index is out of bounds)
@return Array element or default value
 */
   YM // sSI  _getDefault    (sUI _index, sSI _def);

/* @method identity,int num

Reallocate array to "num" elements and set element[i] to i.

@arg num New number of elements
*/
   YM //void identity       (sSI);

/* @method sortByValue,IntArray r

Sort elements by value and return new element order in "r".

@arg r Receives new element order
*/
   YM //void sortByValue    (YAC_Object *_ia);

/* @method rearrange,IntArray order:boolean

Reorder elements according to given element order.

@arg order New element order
@return true if elements have been reordered successfully, false otherwise.
*/
   YM //sBool rearrange     (YAC_Object *_ia);

/* @method swap,int indexA,int indexB:boolean

Swap two elements.

@arg indexA Index of first element
@arg indexB Index of second element
@return true if elements have been swapped successfully, false otherwise.
*/
   YM //sBool _swap         (sSI _indexs, sSI _indexd);

/* @method visit,IntArray src,int off,int num

Create a view into another IntArray object.

@arg src Source array
@arg off Source offset
@arg num Number of elements
*/
   YM //void _visit         (YAC_Object *_ia, sSI _off, sSI _num);

/* @method useAll

Mark all array elements used, i.e. set numElements = maxElements.

*/
   YM //void  _useAll         (void);

/* @method getFirst:int

Return first array element.

@return First array element or 0 if the array is empty.

*/
   YM //sSI _getFirst         (void);

/* @method getLast:int

Return last used array element.

@return Last used array element (numElements - 1) or 0 if the array is empty.

*/
   YM //sSI _getLast          (void);

/* @method join,Object a,Object b
Join arrays 'a' and 'b'.

@arg a Other array (or this, or null)
@arg b Other array (or this, or null)

@return true if arrays were joined, false if new elements could not be allocated
*/
   YM //sBool _join            (YAC_Object *_a, YAC_Object *_b);


/* @method replace,int a,int b:int
Replace all occurences of 'a' with 'b' and return number of elements replaced.

@arg a Value a
@arg b Value b that replaces a.

@return Number of elements replaced.
*/
   YM //sSI _replace           (sSI _a, sSI _b)


/* @method replaceNeg,int b:int
Replace all occurences of negative values with 'b' and return number of elements replaced.

@arg b Value b that replaces negative values.

@return Number of elements replaced.
*/
   YM //sSI _replaceNeg        (sSI _b)


/* @method inc,int idx
 Increment element at index 'idx'.

 If the index is out of bounds, simply do nothing.
*/
   YM //void _inc              (sUI _idx)


/* @method dec,int idx
 Decrement element at index 'idx'.

 If the index is out of bounds, simply do nothing.
*/
   YM //void _dec              (sUI _idx)


/* @method getNumUniqueValues:int
 Return number of unique values (ignore duplicates).
*/
   YM //sSI _getNumUniqueValues(void)


/* @method isEqual,Array o:boolean
  Compare to other array and return true if the array contents are equal.

  (note) typecast other array elements to int when necessary.

@arg o Other array
@return true if the array contents are equal
*/
   YM //sBool _isEqual (YAC_Object *_o);

/* @method findSubArrayOffset,IntArray ia:int
 Find offset of subarray "ia".

@arg ia Sub-array
@return Offset in this array
*/
   YM //sSI _findSubArrayOffset (YAC_Object *_ia);

};
#include "ying_core_IntArray.cpp"
YAC_C_CORE_POOLED(_IntArray, "IntArray", YAC_CLID_INTARRAY);
//------------------------------------------------------------------------------------- prebuffered IntArrays
#define DprebufIntArray(a) \
class _IntArray##a : public _IntArray {\
private:\
   sSI prebuf[a];\
public:\
   YAC_H(_IntArray##a);\
   YAC_POOLED_H(_IntArray##a, YAC_POOL_PRIORITY_MEDIUM);\
   _IntArray##a(void) { elements = prebuf; max_elements = a; }\
};\
YAC_C_CORE_POOLED(_IntArray##a, "IntArray"#a, YAC_CLID_INTARRAY##a)

#define DregisterprebufIntArray(a) \
   templ=new _IntArray##a();         templ->class_ID=YAC_CLID_INTARRAY##a;\
   if(!registerClass(templ, 1, 0)) return 0;\
   cpp_typecast_map[YAC_CLID_INTARRAY##a][YAC_CLID_INTARRAY]=1;\

DprebufIntArray(8);
DprebufIntArray(16);
DprebufIntArray(32);
DprebufIntArray(64);
DprebufIntArray(128);

/* @class IntArray8,IntArray

An IntArray that can store 8 elements without (re-)allocation.

@see IntArray
*/

/* @class IntArray16,IntArray

An IntArray that can store 16 elements without (re-)allocation.

@see IntArray
*/

/* @class IntArray32,IntArray

An IntArray that can store 32 elements without (re-)allocation.

@see IntArray
*/

/* @class IntArray64,IntArray

An IntArray that can store 64 elements without (re-)allocation.

@see IntArray
*/

/* @class IntArray128,IntArray

An IntArray that can store 128 elements without (re-)allocation.

@see IntArray
*/

//------------------------------------------------------------------------------------- ObjectArray
/* @class ObjectArray,Object

A homogenous array of objects.

A template object is used to allocate new objects.

@see PointerArray
*/

YC class _ObjectArray : public YAC_ObjectArray {
public:
   YAC(_ObjectArray);

/* @method add,Object o:boolean

Add the given object value to this arraylist.

The array will be reallocated if necessary.

@arg o If o is of type §Value, try to release the object from the value container, add an object reference otherwise.
@return true if the Object has been added, false otherwise.
@see delete
@see insert
@see remove
*/
   YM //sSI         _add           (YAC_Object *_value);

/* @method remove,Object o:boolean

Delete the first element whose object pointer equals the given pointer.

@arg o Object (pointer)
@return true if an element matching the given Object address was found and deleted, false otherwise.
@see add
@see delete
@see insert
*/
   YM //sSI         _remove        (YAC_Object *_o);

/* @method insert,int index,Object o:boolean

Insert the given object before the given index.

The array will be reallocated if necessary.

@arg index Where to insert the object
@arg o Object to insert. Similar to add, §Value object receive a special treatment.
@return true if the Object was inserted successfully, false otherwise
@see add
@see delete
@see remove
*/
   YM //sSI         _insert        (sSI _index, YAC_Object *_value);

/* @method delete,int index:boolean

Delete the array element at the given index.

@arg index Which element to delete
@return true if the element was deleted, false otherwise.
@see add
@see insert
@see remove
*/
   YM //sSI         _delete        (sSI _index);

/* @method alloc,int max:boolean

Allocate the given number of array elements.

All previous elements will be discarded.
The number of used elements, numElements, will be reset to 0.

@arg max New total number of elements.
@return true if the elements have been allocated successfully, false otherwise.
@see free
@see realloc
*/
   YM //sBool       alloc          (sUI _maxelements);

/* @method free

Free all array elements
*/
   YM //void        free           (void);

/* @method realloc,int max:boolean

Reallocate array.

Discard/add empty elements as required.

@arg max New total number of elements
@return true if the reallocation succeeded, false otherwise
@see alloc
@see realloc
*/
   YM //sBool       realloc        (sUI _maxelements);

/* @method getNumElements:int

Return the number of used elements.

@return Number of used elements
@see setNumElements
*/
   YM //sUI         getNumElements (void);

/* @method getMaxElements:int

Return the total number of elements.

@return Total number of elements
@see alloc
@see free
@see realloc
*/
   YM //sUI         getMaxElements (void);

/* @method empty

Reset the number of used elements, numElements, to 0.
*/
   YM //void        empty          (void);

/* @method isEmpty:boolean

Check if array is empty (numElements == 0).
*/
   YM //sBool isEmpty       (void);

/* @method reverse

Reverse the element order.
*/
   YM //void        reverse        (void);

/* @method getNextFree:Object

Return a reference to the next free object.

Increase numElements.

@return Next free object or null if the array is full.
@see setNumElements
*/
   YM //YAC_Object *getNextFree    (void);

/* @method setNumElements,int num

Set the number of used array elements.

@arg num New number of elements
@see getNextFree
*/
   YM //void        setNumElements (sSI);

/* @method setTemplate,Object template

Set the template object to use for allocation of new array objects.

@arg template Template object to use for allocations
@see getTemplate
*/
   YM //void        setTemplate    (YAC_Object*);

/* @method getTemplate:Object

Return the template object that is currently being used for allocations of new objects.

@return Template object to use for allocations
@see setTemplate
*/
   YM //YAC_Object *getTemplate    (void);

/* @method getString:String

Return a string representation of this ObjectArray.

@return new String instance that holds a string representation of this ObjectArray
*/
   YM //void _getString     (YAC_Value *_r);

/* @method get,int index:Object

Return a reference to array element at the given index.

Silently return null if "index" exceeds the array bounds.

@arg index Array index
@return Object reference or null
@see getDeref
*/
   YM //YAC_Object *get            (sSI);

/* @method getDeref,int index:Object

Return a deletable reference to array element at the given index.

Silently return null if "index" exceeds the array bounds.

Note: The Object will become a non-deletable reference in this array afterwards.

@arg index Array index
@return Array element or null
@see get
@see unlink
*/
   YM //void        getDeref       (sSI, YAC_Value *_r);


/* @method unlink,Object o:Object
Return a deletable reference to array element at the given index and remove the element from this array.
Return null if 'o' was not found in this array.
@arg o Object
@return Deletable Object
@see getDeref
*/
   YM //void        unlink          (YAC_Object *_o, YAC_Value *_r);


/* @method indexOfPointer,Object o:int

Return the index of the element that points to the given object.

@arg o The object pointer to look for
@arg startOff Start index
@return Array index or -1 if the object address was not found in this array
@see containsPointer
*/
   YM //   sSI         indexOfPointer (YAC_Object *_o, sSI _startOff);

/* @method indexOfObject,Object o:int

Return the index of the element that equals the given object.

@arg o The object to look for
@arg startOff Start index
@return Array index or -1 if no matching object was found in this array
@see containsObject
*/
   YM //   sSI         indexOfObject  (YAC_Object *_o, sSI _startOff);

/* @method containsObject,Object o:boolean

Check whether array contains an object that equals the given object.

@arg o The object to compare with. §Object.yacEquals is used for comparisons.
@return true if a matching object was found, false otherwise.
@see indexOfObject
*/
   YM //sSI          containsObject(YAC_Object *_o);

/* @method containsPointer,Object o:boolean

Check whether array contains the given object address.

@arg o The object (address) to compare with.
@return true if a matching object was found, false otherwise.
@see indexOfPointer
*/
   YM //sSI          containsPointer(YAC_Object *_o);

/* @method swap,int indexA,int indexB:boolean

Swap two array elements

@arg indexA Index of first element
@arg indexB Index of second element
@return true if the elements have been swapped successfully, false otherwise.
*/
   YM //sSI         _swap          (sSI _indexs, sSI _indexd);

/* @method useAll

Mark all array elements used, i.e. set numElements = maxElements.

*/
   YM //void  _useAll         (void);

/* @method getFirst:Object

Return first array element.

@return First array element or null if the array is empty. Objects are returned as non-deletable references.

*/
   YM //YAC_Object *_getFirst (void);

/* @method getLast:Object

Return last used array element.

@return Last used array element (numElements - 1) or null if the array is empty. Objects are returned as non-deletable references.

*/
   YM //YAC_Object *_getLast        (void);

};
#include "ying_core_ObjectArray.cpp"
YAC_C_CORE(_ObjectArray, "ObjectArray", YAC_CLID_OBJECTARRAY);
//------------------------------------------------------------------------------------- FloatArray
/* @class FloatArray,Object

An arraylist of 32bit floating point values.

@see FloatArray8
@see FloatArray16
@see FloatArray32
@see FloatArray64
@see FloatArray128
*/
YC class _FloatArray : public YAC_FloatArray {
public:
   YAC_POOLED(_FloatArray, YAC_POOL_PRIORITY_MEDIUM);

/* @method alloc,int max:boolean

Allocate the given number of array elements.

The number of used elements, numElements, will be reset to 0.

@arg max New total number of elements
@return true if the allocation succeeded, false otherwise.
@see free
@see realloc
*/
   YM //sBool alloc         (sUI _maxelements);

/* @method free

Free all array elements.
*/
   YM //void free           (void);


/* @method add,float f:boolean

Add the given float value to this array.

Resize array if necessary.

@arg f Float value
@return true if the value has been added successfully, false otherwise.
@see add2
@see add3
@see add4
@see delete
@see delete2
@see delete3
@see delete4
@see insert
*/
   YM //sBool add           (sF32 _val);


/* @method add2,float f1,float f2:boolean

Add the given float values to this array.

Resize array if necessary.

@arg f1 First float value
@arg f2 Second float value
@return true if the values have been added successfully, false otherwise.
@see add
@see add3
@see add4
@see delete
@see delete2
@see delete3
@see delete4
@see insert
*/
   YM //sBool add2          (sF32 _val1, sF32 _val2);


/* @method add3,float f1,float f2,float f3:boolean

Add the given float values to this array.

Resize array if necessary.

@arg f1 First float value
@arg f2 Second float value
@arg f3 Third float value
@return true if the values have been added successfully, false otherwise.
@see add
@see add2
@see add4
@see delete
@see delete2
@see delete3
@see delete4
@see insert
*/
   YM //sBool add3          (sF32 _val1, sF32 _val2, sF32 _val3);


/* @method add4,float f1,float f2,float f3,float f4:boolean

Add the given float values to this array.

Resize array if necessary.

@arg f1 First float value
@arg f2 Second float value
@arg f3 Third float value
@arg f4 Fourth float value
@return true if the values have been added successfully, false otherwise.
@see add
@see add2
@see add3
@see delete
@see delete2
@see delete3
@see delete4
@see insert
*/
   YM //sBool add4          (sF32 _val1, sF32 _val2, sF32 _val3, sF32 _val4);


/* @method addFromARGB32,int argb32:boolean
Add normalized R,G,B,A floats from packed ARGB32 integer.

@arg argb32 Packed ARGB32 color
@return true if the R,G,B,A float values have been added successfully, false otherwise.
*/
   YM //sBool addFromARGB32 (sUI _argb32);


/* @method addArray,Object array,int off,int len:boolean

Add the given array like object to this array.

Resize array if necessary.

@arg array Array-like object
@arg off Source offset
@arg len Number of elements
@return true if the array has been added successfully, false otherwise.
@see add
@see addEmpty
*/
   YM //sBool addArray      (YAC_Object *_a, sUI _off, sUI _len) {


/* @method addEmpty,int num:int

Increase the number of used elements.

This will <b>not</b> resize the array!

@arg num Number of elements to add
@return New number of elements
@see empty
@see setNumElements
*/
   YM //sSI  addEmpty       (sUI _numelements);

/* @method getNumElements:int

Return number of used elements.

@return Number of used elements
@see addEmpty
@see empty
@see setNumElements
*/
   YM //sUI  getNumElements (void);

/* @method getMaxElements:int

Return total number of elements.

@return Total number of elements.
@see alloc
@see free
@see realloc
*/
   YM //sUI  getMaxElements (void);

/* @method empty

Reset the number of used elements, numElements, to 0.
@see addEmpty
@see setNumElements
*/
   YM //void empty          (void);

/* @method isEmpty:boolean

Check if array is empty (numElements == 0).
*/
   YM //sBool isEmpty       (void);

/* @method realloc,int max:boolean

Reallocate array to given size.

Discard/add new elements as required.

@arg max New total number of elements
@return true if the reallocation succeeded, false otherwise.
@see alloc
@see free
@see getMaxElements
*/
   YM //sBool realloc       (sUI _maxelements);

/* @method insert,int index,float f:boolean

Insert new element before the given index.

Resize the array if required.

@arg index Where to insert the new element
@arg f Value to insert
@return true if the new elements was inserted successfully, false otherwise.
@see add
@see delete
*/
   YM //sSI  insert         (sSI _index, sF32 _val);


/* @method delete,int index:boolean
Delete element at the given index.

@arg index Which element to delete
@return true if the element was deleted, false otherwise.
@see add
@see insert
*/
   YM //sBool _delete       (sSI _index);


/* @method delete2,int index:boolean
Delete two elements at the given index.

@arg index First element index
@return true if the element has been deleted, false otherwise.
@see add
@see insert
*/
   YM //sBool _delete2      (sSI _index);


/* @method delete3,int index:boolean
Delete three elements at the given index.

@arg index First element index
@return true if the element has been deleted, false otherwise.
@see add
@see insert
*/
   YM //sBool _delete3      (sSI _index);


/* @method delete4,int index:boolean
Delete four elements at the given index.

@arg index First element index
@return true if the element has been deleted, false otherwise.
@see add
@see insert
*/
   YM //sBool _delete4      (sSI _index);


/* @method fill,float f

Set all used elements to the given value.

@arg f Fill value
*/
   YM //void fill           (sF32);


/* @method allocAndFill,int num,float f:boolean

Allocate and set all used array elements to value "f".

@arg num Number of elements
@arg f Fill value
@return true when allocation succeeded
@see fill
@see alloc
*/
   YM //sBool allocAndFill  (sSI, sF32);


/* @method fillRegion,int off,int len,float val
Fill a range of elements with a constant value.

The region start/end will be clipped to (0;numElements).

@arg off Start offset (element index)
@arg len Number of elements
@arg val Fill value
@see fill
*/
   YM //void fillRegion     (sSI _off, sSI _len, sF32 _val);


/* @method fillFromARGB32,int argb32
Unpack ARGB32 integer and fill used_elements in R,G,B,A order.

@arg argb32 Packed ARGB32 integer
*/
   YM //void fillFromARGB32 (sUI _argb32);


/* @method reverse

Reverse element order
*/
   YM //void reverse        (void);

/* @method scale,Object src,float s

Read elements from source array, scale by s and write to this array.

Lazy-allocate elements as required.

@todo Use operator =>
@arg a Source array (IntArray or FloatArray)
@arg s Scale factor
*/
   YM //void scale          (YAC_Object *, sF32);

/* @method setNumElements,int num

Set the number of used elements to the given value.

@arg num New number of used elements
@see addEmpty
@see empty
@see getNumElements
*/
   YM //void setNumElements (sSI);

/* @method blend,FloatArray b,float weight

Blend with other array and write to this array.

Scale delta (otherValue-thisValue) by "weight" and write to this array.

@arg b Other array
@arg weight Scale factor for deltas. Range is 0..1
@see blendAB
*/
   YM //void blend          (YAC_Object*, sF32);

/* @method blendAB,FloatArray a,FloatArray b,float weight

Blend two arrays and write to this array.

Scale delta (bValue-aValue) by "weight" and write to this array.

@arg a First array
@arg b Second array
@arg weight Blend factor. Range is 0..1
@see blend
*/
   YM //void blendAB        (YAC_Object*, YAC_Object*, sF32);

/* @method copyFrom,FloatArray src,int off,int len,int destOff

Copy elements from source array.

@arg off Source offset in source array
@arg len Number of elements to copy
@arg destOff Destination offset in this array
*/
   YM //void copyFrom       (YAC_Object *_fa, sSI _off, sSI _len, sSI _doff);

/* @method read32,Stream ifs,int num,int destOff:int

Read 32bit floating point values from the given input stream.

@arg ifs Input stream
@arg num Number of elements to read
@arg destOff Destination offset in this array
@return Number of elements read
*/
   YM //sUI  _read32        (YAC_Object *_st, sUI _num, sUI _doff);

/* @method read64,Stream ifs,int num,int destOff:int

Read 64bit floating point values from the given input stream and truncate to 32bit.

@arg ifs Input stream
@arg num Number of elements to read
@arg destOff Destination offset in this array
@return Number of elements read
*/
   YM //sUI  _read64        (YAC_Object *_st, sUI _num, sUI _doff);

/* @method write32,Stream ofs,int num,int srcOff:int

Write 32bit floating point values to the given output stream.

@arg ofs Output stream
@arg num Number of elements to write
@arg srcOff Source offset in this array
@return Number of elements written
*/
   YM //sUI  _write32       (YAC_Object *_st, sUI _num, sUI _doff);

/* @method write64,Stream ofs,int num,int srcOff:int

Write 64bit floating point values to the given output stream.

@arg ofs Output stream
@arg num Number of elements to write
@arg srcOff Source offset in this array
@return Number of elements written
*/
   YM //sUI  _write64       (YAC_Object *_st, sUI _num, sUI _doff);

/* @method getString:String

Return string representation of this array.

@return new String
*/
   YM //void _getString     (YAC_Value *_r);

/* @method getString2:String

Return (x;y) pair string representation of this array.

@return new String
*/
   YM //void _getString2    (YAC_Value *_r);

/* @method getString3:String

Return (x;y;z) tuple string representation of this array.

@return new String
*/
   YM //void _getString3    (YAC_Value *_r);

/* @method getMin:float

Return smallest element value.

@return Minimum value
@see getMax
@see getAbsMin
@see getAbsMax
*/
   YM //sF32 getMin         (void);

/* @method getMinIdx:int

Return index of smallest element value.

@return Minimum value index
@see getMin
@see getMax
@see getAbsMin
@see getAbsMax
*/
   YM //sSI getMinIdx       (void);

/* @method getMax:float

Return biggest element value.

@return Maximum value
@see getMaxIdx
@see getMin
@see getMinIdx
@see getAbsMin
@see getAbsMax
*/
   YM //sF32 getMax         (void);

/* @method getMaxIdx:int

Return index of biggest element value.

@return Maximum value index
@see getMax
@see getMin
@see getMinIdx
@see getAbsMin
@see getAbsMax
*/
   YM //sSI getMaxIdx       (void);

/* @method getAbsMin:float

Return smallest element value, not regarding sign.

@return Minimum value
@see getMin
@see getMinIdx
@see getMax
@see getMaxIdx
@see getAbsMax
*/
   YM //sF32 getAbsMin      (void);

/* @method getAbsMax:float

Return biggest element value, not regarding sign.

@return Maximum value
@see getMin
@see getMax
@see getAbsMin
*/
   YM //sF32 getAbsMax      (void);

/* @method getMean:float

Return average element value.

@return Average value
@see getAbsMean
*/
   YM //sF32 getMean        (void);

/* @method getAbsMean:float

Return average element value, not regarding sign.

@return Average value
@see getMean
*/
   YM //sF32 getAbsMean     (void);

/* @method getRms:float

Return RMS (root mean square) for all values in the array.

@return RMS
*/
   YM //sF32  getRms        (void);

/* @method getRmsMax:float

Return maximum RMS (root mean square).

@return Peak RMS
*/
   YM //sF32  getRmsMax     (void);

/* @method get,int index:float

Return element value at the given index.

Silently return 0.0f if "index" is out of bounds.

@arg index Which element to return
@return Element value
*/
   YM //sF32 get            (sSI);

/* @method set,int index,float val

Set element at "index" to "val".
Updates numElements when index exceeds it (>=).

Silently do nothing when index exceed array size (>= maxElements).

@arg index Array index
@arg val New value
*/
   YM //void  _set          (sUI, sF32);

/* @method getRev,int index:float

Return element value at (numElements - 1 - index).

Silently return 0.0f if resulting index is out of bounds.

@arg index Which element to return (starting from numElements)
@return Element value
*/
   YM //sF32  _getRev       (sSI);

/* @method setRev,int index,float val

Set element at "numElements - 1 - index" to "val".

Silently do nothing when index exceed array size (<0 || >= maxElements).

@arg index Array index (starting from numElements)
@arg val New value
*/
   YM //void  _setRev       (sSI, sF32);

/* @method getDefault,int index,float def:float

Return array element at 'index', or return 'def' when index is out of bounds.

@arg index Array index
@arg def Default value (when index is out of bounds)
@return Array element or default value
 */
   YM // sF32 _getDefault    (sUI _index, sF32 _def);

/* @method contains,float f:boolean

Check whether array contains the given value.

@arg f Element value to look for
@return true if an element with value "f" was found, false otherwise.
*/
   YM //sSI  contains       (sF32);

/* @method indexOf,float f,int off:int

Return index of element with value "f". -1 if no matching element was found.

@arg f Value to look for
@arg off Start index
@return Index of element, -1 if not found.
*/
   YM //sSI  indexOf        (sF32, sSI);

/* @method indexOfNot,float f,int off:int

Return index of element that does not have value "f". -1 if no matching element was found.

@arg f Value to look for
@arg off Start index
@return Index of element, -1 if not found.
*/
   YM //sSI  indexOfNot     (sF32, sSI);

/* @method fixNaNToZero
Replace NaN values by 0
*/
   YM //void fixNaNToZero   (void);

/* @method sortByValue,IntArray order

Sort elements by value and write new element order to the given IntArray.

@arg order Receives the new element order
*/
   YM //void sortByValue    (YAC_Object *_ia);

/* @method winLinear,float index:float

Return linear interpolation of elements.

@arg index Element index
@return Interpolated value
@see winCosine
@see winQuadratic
@see winCubic
@see winQuintic
*/
   YM //sF32 winLinear      (sF32 _index);

/* @method winCosine,float index:float

Return cosine interpolation of elements.

@arg index Element index
@return Interpolated value
@see winLinear
@see winQuadratic
@see winCubic
@see winQuintic
*/
   YM //sF32 winCosine      (sF32 _index);

/* @method winQuadratic,float index:float

Return quadratic interpolation of elements.

@arg index Element index
@return Interpolated value
@see winLinear
@see winCosine
@see winCubic
@see winQuintic
*/
   YM //sF32 winQuadratic   (sF32 _index);

/* @method winCubic,float index:float

Return cubic interpolation of elements.

@arg index Element index
@return Interpolated value
@see winLinear
@see winCosine
@see winQuadratic
@see winQuintic
*/
   YM //sF32 winCubic       (sF32 _index);

/* @method winQuintic,float index:float

Return quintic interpolation of elements.

@arg index Element index
@return Interpolated value
@see winLinear
@see winCosine
@see winCubic
@see winQuadratic
*/
   YM //sF32 winQuintic     (sF32 _index);

/* @method rearrange,IntArray order:boolean

Rearrange array elements according to the given IntArray.

@arg order Holds the new element array
@return true if the elements have been re-arranged successfully, false otherwise.
*/
   YM //sSI  rearrange      (YAC_Object *_ia);

/* @method swap,int indexA,int indexB:boolean

Swap two array elements.

@arg indexA Index of first element
@arg indexB Index of second element
@return true if the elements have been swapped successfully, false otherwise.
*/
   YM //sSI  _swap          (sSI _indexs, sSI _indexd);

/* @method visit,FloatArray src,int off,int len

Create a view into the given FloatArray.

@arg src Source array
@arg off Source offset
@arg len Number of elements
*/
   YM //void _visit         (YAC_Object *_fa, sSI _off, sSI _len);

/* @method unlinkFrom,FloatArray src

Grab elements from 'src' and transfer elements ownership to this object.

@arg src Source array
*/
   YM //void _unlinkFrom (YAC_Object *_fa);

/* @method useAll

Mark all array elements used, i.e. set numElements = maxElements.

*/
   YM //void  _useAll         (void);

/* @method getFirst:float

Return first array element.

@return First array element or 0.0f if the array is empty.

*/
   YM //sF32 _getFirst         (void);

/* @method getLast:float

Return last used array element.

@return Last used array element (numElements - 1) or 0.0f if the array is empty.

*/
   YM //sF32 _getLast          (void);


/* @method interleave,int numDestCh,int destCh,FloatArray src,int numSrcCh,int srcCh,int numCh,int numElem
   Copy interleaved elements from multi-channel array.

   @arg numDestCh Number of channels (in this array)
   @arg destCh Destination channel index (in this array)
   @arg src Source buffer
   @arg numSrcCh Number of source channels
   @arg srcCh Source channel index
   @arg numCh Number of channels to copy
   @arg numElem Number of elements/frames to copy
*/
   YM //void _interleave       (sUI _numDestCh, sUI _destCh, YAC_Object *_src, sUI _numSrcCh, sUI _srcCh, sUI numCh, sUI _numElem);



/* @method bezierCubicDeCasteljau2d,float x0,float y0,float x1,float y1,float x2,float y2,float x3,float y3,float scaleX,float scaleY,int recMax,float thresholdB,float thresholdP,FloatArray retT
  Calculate bezier curve using the de Casteljau algorithm.

  Note: the array will first be emptied and then filled with the resulting vertices (check numElements afterwards).

@arg x0 Start point x
@arg y0 Start point y
@arg x1 Control point #1 x
@arg y1 Control point #1 y
@arg x2 Control point #2 x
@arg y2 Control point #2 y
@arg x3 End point x
@arg y3 End point y
@arg scaleX x scaling factor
@arg scaleY y scaling factor
@arg recMax recursion limit
@arg thresholdB bendiness threshold (max distance between bezier midpoint and plane (p3-p0 and perpendicular normal)
@arg thresholdP position threshold (max distance between start and end point)
@arg The (non-linear) per-point curve time is returned in this array
*/
   YM //void _bezierCubicDeCasteljau2d(sF32 x0, sF32 y0, sF32 x1, sF32 y1, sF32 x2, sF32 y2, sF32 x3, sF32 y3, sF32 _scaleX, sF32 _scaleY, sSI _recMax, sF32 _thresholdB, sF32 _thresholdP, YAC_Object *_retT);



/* @method findNearestIdx1d,float x,int off,int stride,float maxDist:int
  Find index of x-element that has the smallest distance to point x.

@arg x Point x
@arg off Element start offset
@arg stride Element advance (must be >0)
@arg maxDist Maximum distance
@return Index of element or -1 if the point is too far away
*/
   YM //sSI findNearestIdx1d(sF32 _x, sUI _off, sUI _stride, sF32 _maxDist);



/* @method findNearestIdx2d,float x,float y,int off,int stride,float maxDist,float aspect:int
  Treat elements as x/y 2d vectors and find index of x-element that has the smallest distance to point (x,y).

@arg x Point x
@arg y Point y
@arg off Element start offset
@arg stride Element advance (must be >0)
@arg maxDist Maximum distance
@arg aspect Aspect ratio (width/height) of graphical representation
@return Index of element (x-component of 2d vector) or -1 if the point is too far away
*/
   YM //sSI findNearestIdx2d(sF32 _x, sF32 _y, sUI _off, sUI _stride, sF32 _maxDist, sF32 _aspect);


/* @method sum,int off,int num:float
  Sum elements

@arg off Index of first element
@arg num Number of elements to sum
@return Sum
*/
   YM //sF32 sum(sUI _off, sUI _num);



/* @method sumPathLen1d,int off,int num:float
  Sum element deltas

@arg off Index of first element
@arg num Number of elements to sum
@return Path length
*/
   YM //sF32 sumPathLen1d(sUI _off, sUI _num);



/* @method sumPathLen2d,int off,int num:float
  Treat elements as x/y 2d vectors and calculate path length.

@arg off Index of first vector (float element offset / 2)
@arg num Number of vectors to sum (number of float elements / 2)
@return Path length
*/
   YM //sF32 sumPathLen2d(sUI _off, sUI _num);



/* @method loadFromStreamUnsigned8,Stream s,int num:boolean
  Read unsigned 8bit integers from stream, convert to float and normalize.

@arg Stream Stream to read from
@arg num Number of samples to read
@return true if the samples have been loaded successfully
*/
   YM //sBool loadFromStreamUnsigned8 (YAC_Object *_stream, sUI _numElements);



/* @method loadFromStreamSigned16,Stream s,int num:boolean
  Read signed 16bit integers from stream, convert to float and normalize.

@arg Stream Stream to read from
@arg num Number of samples to read
@return true if the samples have been loaded successfully
*/
   YM //sBool loadFromStreamSigned16  (YAC_Object *_stream, sUI _numElements);



/* @method loadFromStreamSigned24,Stream s,int num:boolean
  Read signed 24bit integers from stream, convert to float and normalize.

@arg Stream Stream to read from
@arg num Number of samples to read
@return true if the samples have been loaded successfully
*/
   YM //sBool loadFromStreamSigned24  (YAC_Object *_stream, sUI _numElements);



/* @method loadFromStreamFloat32,Stream s,int num:boolean
  Read 24bit IEEE floats from stream.

@arg Stream Stream to read from
@arg num Number of samples to read
@return true if the samples have been loaded successfully
*/
   YM //sBool loadFromStreamFloat32   (YAC_Object *_stream, sUI _numElements);


/* @method saveToStreamSigned16,Stream s,int numElements:boolean
  Write signed 16bit integers to stream (clip to -32768..+32767)
@arg Stream Stream to write to
@arg numElements Number of values to write. 0=write entire array
@return true if the samples have been written successfully
 */
   YM // sBool saveToStreamSigned16 (YAC_Object *_stream, sUI _numElements);


/* @method saveToStreamSigned12FrameEnc,Stream s,int numElements:boolean
  Write signed 12bit integers to stream (clip to -2048..+2047).
  Encode in 8-value frames (4, 8, 16 bit deltas, or 16bit raw values)
@arg Stream Stream to write to
@arg numElements Number of values to write. 0=write entire array
@return true if the samples have been written successfully
 */
   YM // sBool saveToStreamSigned12FrameEnc (YAC_Object *_stream, sUI _numElements);


/* @method isEqual,FloatArray o:boolean
  Compare to other array and return true if the array contents are equal.

@arg o Other array
@return true if the array contents are equal
*/
   YM //sBool _isEqual (YAC_Object *_fa);


/* @method join,Object a,Object b
Join arrays 'a' and 'b'.

@arg a Other array (or this, or null)
@arg b Other array (or this, or null)

@return true if arrays were joined, false if new elements could not be allocated
*/
   YM //sBool _join            (YAC_Object *_a, YAC_Object *_b);


/* @method getAbsMaxDiff,FloatArray o:float
Return absolute maximum difference of 'o' to this array.

@arg o Other array

@return Absolute maximum difference (o - this)
*/
   YM //sF32 getAbsMaxDiff (YAC_Object *_fa);


/* @method findIdxBefore,float f:int
Find index of last element that is smaller than 'f'.
@arg f
@return index
*/
   YM //sSI findIdxBefore (sF32 _val);


/* @method biquad,FloatArray faIn,int type,float dbGain,float freq,float res
Apply biquad filter to input samples and store result in this array.

Caller must resize this array if necessary.

@arg faIn Input samples (may be the same array)
@arg type Filter type, 1=LPF, 2=HPF, 3=BPF, 4=BRF, 5=PEQ, 6=LSH, 7=HSH
@arg dbGain Gain
@arg freq Cutoff frequency (0..1)
@arg res Resonance (0..1)
*/
   YM // void biquad      (YAC_Object *_faIn, sSI _type, sF32 _dbGain, sF32 _freq, sF32 _res);

/* @method mergeArraySkipVal,FloatArray fa,float skipVal
Replace elements in this array by arrays from other array which do not equal 'skipVal'.

@arg fa Other array
@arg skipVal Array value not to be merged
*/
   YM // void mergeArraySkipVal (YAC_Object *_fa, float _skipVal);

};
#include "ying_core_FloatArray.cpp"
YAC_C_CORE_POOLED(_FloatArray, "FloatArray", YAC_CLID_FLOATARRAY);
//------------------------------------------------------------------------------------- prebuffered FloatArrays
#define DprebufFloatArray(a) \
class _FloatArray##a : public _FloatArray {\
private:\
   sF32 prebuf[a];\
public:\
   YAC_H(_FloatArray##a);\
   YAC_POOLED_H(_FloatArray##a, YAC_POOL_PRIORITY_MEDIUM);\
   _FloatArray##a(void) { elements = prebuf; max_elements = a; }\
};\
YAC_C_CORE_POOLED(_FloatArray##a, "FloatArray"#a, YAC_CLID_FLOATARRAY##a)

#define DregisterprebufFloatArray(a) \
   templ=new _FloatArray##a();         templ->class_ID=YAC_CLID_FLOATARRAY##a;\
   if(!registerClass(templ, 1, 0)) return 0;\
   cpp_typecast_map[YAC_CLID_FLOATARRAY##a][YAC_CLID_FLOATARRAY]=1;\

DprebufFloatArray(8);
DprebufFloatArray(16);
DprebufFloatArray(32);
DprebufFloatArray(64);
DprebufFloatArray(128);

/* @class FloatArray8,FloatArray

A §FloatArray that can store 8 elements without (re-)allocation.

@see FloatArray
*/

/* @class FloatArray16,FloatArray

A §FloatArray that can store 16 elements without (re-)allocation.

@see FloatArray
*/

/* @class FloatArray32,FloatArray

A §FloatArray that can store 32 elements without (re-)allocation.

@see FloatArray
*/

/* @class FloatArray64,FloatArray

A §FloatArray that can store 64 elements without (re-)allocation.

@see FloatArray
*/

/* @class FloatArray128,FloatArray

A §FloatArray that can store 128 elements without (re-)allocation.

@see FloatArray
*/


//------------------------------------------------------------------------------------- StringArray
/* @class StringArray,Object

An arraylist of Strings.

@see String
*/
YC class _StringArray : public YAC_StringArray {
public:
   YAC_POOLED(_StringArray, YAC_POOL_PRIORITY_MEDIUM);

/* @method alloc,int max:boolean

Allocate the given number of array elements.

All previous elements will be discarded.
The number of used elements, numElements, will be reset to 0.

@arg max Total number of elements to allocate
@return true if the allocation succeeded, false otherwise.
@see free
@see getMaxElements
@see realloc
*/
   YM //sBool       alloc          (sUI _maxelements);

/* @method free

Free all array elements.
@see alloc
@see getMaxElements
@see realloc
*/
   YM //void        free           (void);

/* @method add,String s:boolean

Add a copy of the given string to this arraylist.

The array will be resized as required.

@arg s The string to append to this arraylist
@return true if the String has been added successfully, false otherwise
@see delete
@see insert
*/
   YM //sSI         add            (YAC_Object *_s);

/* @method addEmpty,int num:int

Increase the number of used elements.

This method will <b>not</b> reallocate the array.

@arg num Number of elements to add
@return New number of used elements.
@see empty
@see getNumElements
@see setNumElements
*/
   YM //sUI         addEmpty       (sUI _numelements);

/* @method remove,String s:boolean

Remove the given string from the arraylist.

The array will be searched for an entry matching the given String content.

@arg s The String to remove.
@return true if the string has been removed successfully, false otherwise.
@see add
@see delete
@see insert
*/
   YM //sBool        remove         (YAC_Object *_s);

/* @method getNumElements:int

Return the number of used elements.

@return Number of used array elements
@see addEmpty
@see empty
@see setNumElements
*/
   YM //sUI         getNumElements (void);

/* @method getMaxElements:int

Return the total number of elements

@return Total number of array elements
@see alloc
@see free
@see realloc
*/
   YM //sUI         getMaxElements (void);

/* @method empty

Reset the number of used elements, numElements, to 0.
@see addEmpty
@see getNumElements
@see setNumElements
*/
   YM //void        empty          (void);

/* @method isEmpty:boolean

Check if array is empty (numElements == 0).
*/
   YM //sBool isEmpty       (void);

/* @method realloc,int max:boolean

Reallocate array.

Discard/add elements as required.

@arg max Total number of elements
@return true if the array has been reallocated successfully, false otherwise.
@see alloc
@see free
@see getMaxElements
*/
   YM //sBool       realloc        (sUI _maxelements);

/* @method insert,int index,String s:boolean

Insert a copy of the given String into this array.

@arg index Where to insert the new element
@arg s Which string to insert
@return true if the String has been inserted successfully, false otherwise.
@see add
@see delete
*/
   YM //sSI         insert         (sSI _index, YAC_Object *_s);

/* @method delete,int index:boolean

Delete element at the given index.

@arg index Which element to delete
@return true if the element has been deleted successfully, false otherwise.
@see add
@see insert
*/
   YM //sSI         _delete        (sSI _index);

/* @method unset

Empty all Strings
*/
   YM //void        unset          (void);

/* @method reverse

Reverse element order.
*/
   YM //void        reverse        (void);

/* @method getNextFree:String

Return next free String.

Increase numElements.

@return Reference to next String or null if array is full.
@see empty
@see addEmpty
@see getNumElements
@see setNumElements
*/
   YM //YAC_Object *getNextFree    (void);

/* @method setNumElements,int num

Set number of used elements.

@arg num New number of used elements
@see empty
@see addEmpty
@see getNextFree
@see getNumElements
*/
   YM //void        setNumElements (sUI);

/* @method sortByLength,IntArray order

Sort strings by length and write new element order to "order".

@arg order Receives the new element order
@see sortByValue
@see rearrange
*/
   YM //void        sortByLength   (YAC_Object *_ia);

/* @method sortByValue,IntArray order,boolean bCaseSensitive

Sort strings by content and write new element order to "order".

@arg order Receives the new element order
@arg bCaseSensitive if true, sort case-sensitive, otherwise sort case-insensitive.
@see sortByLength
@see rearrange
*/
   YM //void        sortByValue    (YAC_Object *_ia, sSI _cs);

/* @method getString:String

Return string representation of this array.

@return new String instance holding a string representation of this array.
*/
   YM //void        _getString     (YAC_Value *_r);

/* @method get,int index:String

Return a reference to an array element.

Silently return null if the given index is out of bounds.

@arg int Array index
@return Reference to String object
@see getCopy
*/
   YM //YAC_Object *get            (sSI);

/* @method set,int index,String val

Set element at "index" to "val".
Updates numElements when index exceeds it (>=).

Silently do nothing when index exceed array size (>= maxElements).

@arg index Array index
@arg val New value
*/
   YM //void  _set                 (sUI, YAC_Object *_val);

/* @method getRev,int index:String

Return a reference to array element at (numElements - 1 - index).

Silently return null if the given index is out of bounds.

@arg int Array index
@return Reference to String object
@see getCopy
*/
   YM //YAC_Object *_getRev        (sSI);

/* @method setRev,int index,String val

Set element at (numElements - 1 -index) to "val".

Silently do nothing when index exceed array size (>= maxElements).

@arg index Array index
@arg val New value
*/
   YM //void  _setRev              (sUI, YAC_Object *_val);

/* @method getCopy,int index:String

Return a copy of an array element.

@arg index Array index
@return new String object holding a copy of the given array element.
@see get
*/
   YM //void        getCopy        (sSI _index, YAC_Value *_r);

/* @method getDefault,int index:var

Return a reference to the array element at 'index', or return 'valOrVO' when index is out of bounds.

If 'valOrVO' is a §Value object, take the ownership of the stored object and return it.

@arg index Array index
@return Array element or default value
 */
   YM // void       _getDefault    (sUI _index, YAC_Object *_valOrVO, YAC_Value *_r);

/* @method indexOfPointer,Object o:int

Return the index of the element that points to the given object.

@arg o The object pointer to look for
@arg startOff Start index
@return Array index or -1 if the object address was not found in this array
@see containsPointer
*/
   YM //   sSI         indexOfPointer (YAC_Object *_o, sSI _startOff);

/* @method indexOfObject,Object o:int

Return the index of the element that equals the given object.

@arg o The object to look for
@arg startOff Start index
@return Array index or -1 if no matching object was found in this array
@see containsObject
*/
   YM //   sSI         indexOfObject  (YAC_Object *_o, sSI _startOff);

/* @method containsObject,String s:boolean

Check whether this array contains the given String. @yacEquals is used to compare to the given string.

@arg s String to look for
@return true if the String was found, false otherwise.
@see indexOfObject
*/
   YM //sSI         containsObject (YAC_Object *_s);

/* @method containsPointer,String s:boolean

Check whether this array contains the given String address.

@arg s String address to look for
@return true if the String address was found, false otherwise.
*/
   YM //sSI         containsPointer (YAC_Object *_s);

/* @method rearrange,IntArray order:boolean

Rearrange array elements according to given order array.

@arg order Holds the new element order
@return true if the elements have been reordered successfully, false otherwise.
@see sortByLength
@see sortByValue
*/
   YM //sSI         rearrange      (YAC_Object *_ia);

/* @method swap,int indexA,int indexB:boolean

Swap two array elements.

@arg indexA Index of first element
@arg indexB Index of second element
@return true if the elements have been swapped successfully, false otherwise.
*/
   YM //sSI         _swap          (sSI _indexs, sSI _indexd);

/* @method useAll

Mark all array elements used, i.e. set numElements = maxElements.

*/
   YM //void  _useAll         (void);

/* @method getFirst:Object

Return first array element.

@return First array element or null if the array is empty. Objects are returned as non-deletable references.

*/
   YM //YAC_Object *_getFirst (void);

/* @method getLast:Object

Return last used array element.

@return Last used array element (numElements - 1) or null if the array is empty. Objects are returned as non-deletable references.

*/
   YM //YAC_Object *_getLast        (void);

/* @method join,Object a,Object b
Join arrays 'a' and 'b'.

@arg a Other array (or this, or null)
@arg b Other array (or this, or null)

@returns True if arrays were joined. False if new elements could not be allocated.
*/
  YM //sBool        _join           (YAC_Object *_a, YAC_Object *_b);

/* @method getMaxChars:int
Return maximum string length (not counting ASCIIZ).

@return max string length w/o ASCIIz
*/
   YM //sUI         _getMaxChars    (void) const;

/* @method mergeToString,String separatorOrNull:String
Merge all strings into one and optionally insert separator string between the elements.

No separator will be inserted before the first or after the last element.

@arg separatorOrNull optional separator §String
@return merged §String
*/
   YM //void        _mergeToString  (YAC_String *_separatorOrNull, YAC_Value *_r) const;
};
#include "ying_core_StringArray.cpp"
YAC_C_CORE_POOLED(_StringArray, "StringArray", YAC_CLID_STRINGARRAY);
//------------------------------------------------------------------------------------- ClassArray
/* @class ClassArray,Object

Array of scriptclass instances.

@see Class
*/
YC class _ClassArray : public YAC_ClassArray {
public:
   YAC_POOLED(_ClassArray, YAC_POOL_PRIORITY_MEDIUM);

/* @method alloc,int max:boolean

Allocate the given number of array elements.

The number of used element, numElements, will be reset to 0.

@arg max Total number of elements.
@return true if the allocation succeeded, false otherwise.
@see free
@see realloc
@see getMaxElements
*/
   YM //sBool       alloc          (sUI _maxelements);

/* @method free

Free array elements.
@see alloc
@see getMaxElements
@see realloc
*/
   YM //void        free           (void);

/* @method realloc,int max:boolean

Reallocate array elements.

Discard/add new elements as required.

@arg max Total number of elements
@return true if the reallocation succeeded, false otherwise.
@see alloc
@see free
@see getMaxElements
*/
   YM //sBool       realloc        (sUI _maxelements);

/* @method getNumElements:int

Return number of used elements.

@return Number of used elements
@see getNextFree
@see empty
@see setNumElements
*/
   YM //sUI         getNumElements (void);

/* @method getMaxElements:int

Return total number of elements.

@return Total number of elements.
@see alloc
@see free
@see realloc
*/
   YM //sUI         getMaxElements (void);

/* @method empty

Reset number of used elements, numElements, to 0.
@see getNumElements
@see setNumElements
*/
   YM //void        empty          (void);

/* @method isEmpty:boolean

Check if array is empty (numElements == 0).
*/
   YM //sBool isEmpty       (void);

/* @method reverse

Reverse element order.
*/
   YM //void        reverse        (void);

/* @method getNextFree:Class

Return next free element.

Increase the number of used elements, numElements.

@return next free Class instance or null if the arraylist is full.
@see empty
@see getNumElements
@see setNumElements
*/
   YM //YAC_Object *getNextFree    (void);

/* @method setNumElements,int num

Set number of used elements.

@arg num New number of used elements.
@see empty
@see getNextFree
@see getNumElements
*/
   YM //void        setNumElements (sUI);

/* @method add,Class c:boolean

Add the given scriptclass instance to this array.

The array will be resized if necessary.

@arg c Scriptclass instance to add. If c is a §Value object, try to release the scriptclass instance pointer from the value container.
@return true if the scriptclass instance has been added successfully, false otherwise.
@see delete
@see insert
@see remove
*/
   YM //sSI         _add           (YAC_Object *_value);

/* @method remove,Class c:boolean

Delete the first element whose pointer equals the given scriptclass instance.

@arg c Scriptclass instance pointer to look for.
@return true if the given pointer was found and the corresponding array element has been removed, false otherwise.
*/
   YM //sSI         _remove        (YAC_Object *_o);

/* @method insert,int index,Class c:boolean

Insert the given scriptclass instance into the array.

@arg index Where to insert the scriptclass instance
@arg c Scriptclass instance to insert. If c is a §Value object, try to release the scriptclass instance pointer from the value container.
@return true if the scriptclassinstance has been inserted successfully, false otherwise.
@see add
@see delete
@see remove
*/
   YM //sSI         _insert        (sSI _index, YAC_Object *_value);

/* @method delete,int index:boolean

Delete the given array element.

@arg index Which element to delete
@return true if the given element has been deleted successfully, false otherwise.
@see add
@see insert
@see remove
*/
   YM //sSI         _delete        (sSI _index);

/* @method get,int index:Class

Return the given array element.

Silently return null if "index" is out of bounds.

@arg index Which element to return
@return Scriptclass instance
@see getDeref
*/
   YM //YAC_Object *get            (sSI);

/* @method getDeref,int index:Class

Unlink and return the given (deletable) array element.

@arg index Which element to return
@return (deletable) Scriptclass instance
@see get
*/
   YM //void        getDeref       (sSI _index, YAC_Value *_r);


/* @method unlink,Object o:Object
Return a deletable reference to array element at the given index and remove the element from this array.
Return null if 'o' was not found in this array.
@arg o Object
@return Deletable Object
@see getDeref
*/
   YM //void        unlink          (YAC_Object *_o, YAC_Value *_r);


/* @method contains,Class c:boolean

Check whether this array contains a scriptclass instance that equals the given object.

@arg c Scriptclass instance to compare to
@return true if the given scriptclass instance was found, false otherwise.
*/
   YM //sSI         contains       (YAC_Object *_o);

/* @method getTemplate:Class

Return the template object that is currently being used to allocate new array elements.

@return Scriptclass (instance)
@see setTemplate
*/
   YM //YAC_Object *getTemplate    (void);

/* @method setTemplate,Class sci

Set the template object that shall be used to allocate new array elements.
@see getTemplate
*/
   YM //void        setTemplate    (YAC_Object *_sci);

/* @method swap,int indexA,int indexB:boolean

Swap two array elements.

@arg indexA Index of first array element
@arg indexB index of second array element
@return true if the elements have been swapped successfully, false otherwise.
*/
   YM //sSI         _swap          (sSI _indexs, sSI _indexd);

/* @method indexOfPointer,Object o:int

Return the index of the element that points to the given object.

@arg o The object pointer to look for
@arg startOff Start index
@return Array index or -1 if the object address was not found in this array
@see containsPointer
*/
   YM //   sSI         indexOfPointer (YAC_Object *_o, sSI _startOff);

/* @method indexOfObject,Object o:int

Return the index of the element that equals the given object.

@arg o The object to look for
@arg startOff Start index
@return Array index or -1 if no matching object was found in this array
@see containsObject
*/
   YM //   sSI         indexOfObject  (YAC_Object *_o, sSI _startOff);

/* @method containsObject,Object o:boolean

Check whether array contains an object that equals the given object.

@arg o The object to compare with. §Object.yacEquals is used for comparisons.
@return true if a matching object was found, false otherwise.
@see indexOfObject
*/
   YM //sSI          containsObject(YAC_Object *_o);

/* @method containsPointer,Object o:boolean

Check whether array contains the given object address.

@arg o The object (address) to compare with.
@return true if a matching object was found, false otherwise.
@see indexOfPointer
*/
   YM //sSI          containsPointer(YAC_Object *_o);


/* @method getFirst:Object

Return first array element.

@return First array element or null if the array is empty. Objects are returned as non-deletable references.

*/
   YM //YAC_Object *_getFirst (void);

/* @method getLast:Object

Return last used array element.

@return Last used array element (numElements - 1) or null if the array is empty. Objects are returned as non-deletable references.

*/
   YM //YAC_Object *_getLast        (void);


/* @method useAll

Mark all array elements used, i.e. set numElements = maxElements.

*/
   YM //void  _useAll         (void);

};
#include "ying_core_ClassArray.cpp"
YAC_C_CORE_POOLED(_ClassArray, "ClassArray", YAC_CLID_CLASSARRAY);
//------------------------------------------------------------------------------------- PointerArray
/* @class PointerArray,Object

An heterogenous array of objects.

A "deleteme" flag is stored with each element.

Object class types may be arbitrarily mixed.

@see ObjectArray
*/
YC class _PointerArray : public YAC_PointerArray {
public:
   YAC_POOLED(_PointerArray, YAC_POOL_PRIORITY_MEDIUM);

/* @method add,Object o:boolean

Add the given object to the arraylist.

The array will be resized as required.

@arg o The object to add to the arraylist. If o is a §Value object, the contained (deletable) object will be released from the Value container.
@return true if the object has been added successfully, false otherwise.
@see delete
@see insert
@see remove
*/
   YM //sBool        _add           (YAC_Object *_value);

/* @method remove,Object o:boolean

Remove the given object from the arraylist.

The array will be searched for an entry matching the given object address.

@arg o The object to remove.
@return true if the object has been removed successfully, false otherwise.
@see add
@see delete
@see insert
*/
   YM //sBool        _remove        (YAC_Object *_o);

/* @method insert,int index,Object o:boolean

Insert the given object into the arraylist.

The array will be resized as required.

@arg index Where to insert the object
@arg o The object to add. If "o" is a §Value object, the contained (deletable) object will be released from the Value container.
@return true if the object has been inserted successfully, false otherwise.
@see add
@see delete
@see remove
*/
   YM //sBool        _insert        (sSI _index, YAC_Object *_value);

/* @method delete,int index

Delete the element at the given index.

@arg index Which element to delete
@return true if the elements has been deleted successfully, false otherwise.
@see add
@see insert
@see remove
*/
   YM //sBool        _delete        (sUI _index);

/* @method alloc,int max:boolean

Allocate the given number of array elements.

The number of used elements, numElements, will be reset to 0.

@arg max Total number of elements to allocate.
@return true if the allocation succeeded, false otherwise.
@see free
@see realloc
@see getMaxElements
*/
   YM //sBool        alloc          (sUI _maxelements);

/* @method free

Free array elements and reset array size (num/max).
@see unsetAll
@see alloc
@see getMaxElements
@see realloc
*/
   YM //void         free           (void);


/* @method unsetAll
Unset (free) all elements but keep array size (num/max).

@see free
*/
   YM //void        unsetAll        (void);


/* @method realloc,int max:boolean

Resize array.

Element will be discarded/added as required.

@arg max New total number of elements.
@return true if the reallocation succeeded, false otherwise.
@see alloc
@see free
@see getMaxElements
*/
   YM //sBool        realloc        (sUI _maxelements);

/* @method getNumElements:int

Return the number of used elements.

@return Number of used elements
@see empty
@see setNumElements
*/
   YM //sUI          getNumElements (void);

/* @method getMaxElements:int

Return the total number of elements.

@return Total number of elements.
@see alloc
@see free
@see realloc
*/
   YM //sUI          getMaxElements (void);

/* @method empty

Reset the number of used elements, numElements, to 0.

This will reset the number of used elements, numElements, back to 0.
@see getNumElements
@see setNumElements
*/
   YM //void         empty          (void);

/* @method isEmpty:boolean

Check if array is empty (numElements == 0).
*/
   YM //sBool        isEmpty        (void);

/* @method reverse

Reverse element order.
*/
   YM //void         reverse        (void);

/* @method setNumElements,int num

Set the number of used elements.

This will <b>not</b> reallocate the array.

@arg num New number of used elements
@see empty
@see getNextFree
@see getNumElements
*/
   YM //void         setNumElements (sUI);

/* @method getString:String

Return a string representation of this array.

@return new String instance holding a string representation of this array.
*/
   YM //void         _getString     (YAC_Value *_r);

/* @method get,int index:Object

Return a reference to the given array element.

This will silently return null if "index" is out of bounds.

@arg index Which element to return
@return Reference to array element.
@see getDeref
*/
   YM //YAC_Object  *get            (sSI);


/* @method getRev,int index:Object

Return element value at (numElements - 1 - index).

Silently return null if resulting index is out of bounds.

@arg index Which element to return (starting from numElements-1)
@return Element value
*/
   YM //YAC_Object  *_getRev        (sSI);


/* @method getDeref,int index:Object

Return a (deletable) reference to the given array element.

This will silently return null if "index" is out of bounds.

@arg index Which element to return
@return (deletable) reference to array element
@see get
*/
   YM //void         getDeref       (sSI _index, YAC_Value *_r);


/* @method unlink,Object o:Object
Return a deletable reference to array element at the given index and remove the element from this array.
Return null if 'o' was not found in this array.
@arg o Object
@return Deletable Object
@see getDeref
*/
   YM //void        unlink          (YAC_Object *_o, YAC_Value *_r);


/* @method indexOfPointer,Object o:int

Return the index of the element that points to the given object.

@arg o The object pointer to look for
@arg startOff Start index
@return Array index or -1 if the object address was not found in this array
@see containsPointer
*/
   YM //   sSI         indexOfPointer (YAC_Object *_o, sSI _startOff);

/* @method indexOfObject,Object o:int

Return the index of the element that equals the given object.

@arg o The object to look for
@arg startOff Start index
@return Array index or -1 if no matching object was found in this array
@see containsObject
*/
   YM //   sSI         indexOfObject  (YAC_Object *_o, sSI _startOff);

/* @method containsObject,Object o:boolean

Check whether array contains an object that equals the given object.

@arg o The object to compare with. §Object.yacEquals is used for comparisons.
@return true if a matching object was found, false otherwise.
@see indexOfObject
*/
   YM //sSI          containsObject(YAC_Object *_o);

/* @method containsPointer,Object o:boolean

Check whether array contains the given object address.

@arg o The object (address) to compare with.
@return true if a matching object was found, false otherwise.
@see indexOfPointer
*/
   YM //sSI          containsPointer(YAC_Object *_o);

/* @method getNextFree:Object

Return a reference to the next free array element.

This will increase the number of used elements, numElements.

@return Reference to next free element.
@see empty
@see getNumElements
@see setNumElements
*/
   YM //YAC_Object  *getNextFree    (void);

/* @method swap,int indexA,int indexB:boolean

Swap two array elements.

@arg indexA Index of first element
@arg indexB Index of second element
@return true if the elements have been swapped successfully, false otherwise.
*/
   YM //sBool       _swap           (sUI _indexs, sUI _indexd);

/* @method useAll

Mark all array elements used, i.e. set numElements = maxElements.

*/
   YM //void  _useAll         (void);

/* @method getFirst:Object

Return first array element.

@return First array element or null if the array is empty. Objects are returned as non-deletable references.

*/
   YM //YAC_Object *_getFirst (void);

/* @method getLast:Object

Return last used array element.

@return Last used array element (numElements - 1) or null if the array is empty. Objects are returned as non-deletable references.

*/
   YM //YAC_Object *_getLast        (void);

/* @method rearrange,IntArray order:boolean

Reorder elements according to given element order.

@arg order New element order
@return true if elements have been reordered successfully, false otherwise.
*/
   YM //sSI  rearrange      (YAC_Object *_ia);

/* @method joinArrays,Object a,Object b,boolean bCopyA,boolean bCopyB,boolean bUnlinkA,boolean bUnlinkB:boolean

Join arrays 'a' and 'b'.

If none of the copy/unlink flags is set, object values will be copied by reference.

In case that 'a' or 'b' is 'this', object values will always be unlinked ("deref").

The §joinCC, §joinCR, §joinCD, §joinRR, §joinRC, §joinRC, §joinRD, §joinDD, §joinDC, §joinDC are provided
for reasons of convenience (permutations of the copyA/copyB/unlinkA/unlinkB arguments).

(C=copy, R=reference, D=deref (unlink))

@arg a Other array (or this, or null)
@arg b Other array (or this, or null)
@arg bCopyA If true, copy/duplicate array 'a' object values
@arg bCopyB If true, copy/duplicate array 'b' object values
@arg bUnlinkA If true, unlink object values from array 'a'. Takes precedence over bCopyA.
@arg bUnlinkB If true, unlink object values from array 'b'. Takes precedence over bCopyA.

@return True if arrays were joined. False when new array elements could not be allocated.
*/
   YM //sBool _joinArrays (YAC_Object *_a, YAC_Object *_b, sBool _bCopyA, sBool _bCopyB, sBool _bUnlinkA, sBool _bUnlinkB);

   /* @method joinCC,Object a,Object b:boolean
Join arrays a and b. Copy elements.
   */
   YM //sBool _joinCC (YAC_Object *_a, YAC_Object *_b);


   /* @method joinCR,Object a,Object b:boolean
Join arrays a and b. Copy 'A' elements, reference 'B' elements.
   */
   YM //sBool _joinCR (YAC_Object *_a, YAC_Object *_b);


   /* @method joinCD,Object a,Object b:boolean
Join arrays a and b. Copy 'A' elements, dereference 'B' elements.
   */
   YM //sBool _joinCD (YAC_Object *_a, YAC_Object *_b);


   /* @method joinRR,Object a,Object b:boolean
Join arrays a and b. Reference elements.
   */
   YM //sBool _joinRR (YAC_Object *_a, YAC_Object *_b);


   /* @method joinRC,Object a,Object b:boolean
Join arrays a and b. Reference 'A' elements, copy 'B' elements.
   */
   YM //sBool _joinRC (YAC_Object *_a, YAC_Object *_b);


   /* @method joinRD,Object a,Object b:boolean
Join arrays a and b. Reference 'A' elements, dereference 'B' elements.
   */
   YM //sBool _joinRD (YAC_Object *_a, YAC_Object *_b);


   /* @method joinDD,Object a,Object b:boolean
Join arrays a and b. Dereference elements.
   */
   YM //sBool _joinDD (YAC_Object *_a, YAC_Object *_b);


   /* @method joinDC,Object a,Object b:boolean
Join arrays a and b. Dereference 'A' elements, copy 'B' elements.
   */
   YM //sBool _joinDC (YAC_Object *_a, YAC_Object *_b);


   /* @method joinDR,Object a,Object b:boolean
Join arrays a and b. Dereference 'A' elements, reference 'B' elements.
   */
   YM //sBool _joinDR (YAC_Object *_a, YAC_Object *_b);


   /* @method isFirst,Object o:boolean
Check if 'o' is the first element in this array.
   */
   YM //sBool        isFirst        (YAC_Object *_o);

   /* @method isLast,Object o:boolean
Check if 'o' is the last element in this array.
   */
   YM //sBool        isLast         (YAC_Object *_o);

};
#include "ying_core_PointerArray.cpp"
YAC_C_CORE_POOLED(_PointerArray, "PointerArray", YAC_CLID_POINTERARRAY);

//------------------------------------------------------------------------------------- Boolean
/* @class Boolean,Object

A number object that wraps a boolean 1bit integer value.

@see Byte
@see Double
@see Float
@see Integer
@see Long
@see Short
@see UnsignedByte
@see UnsignedInteger
@see UnsignedLong
@see UnsignedShort
*/
YC class _Boolean : public YAC_Boolean {
public:
   YAC_POOLED(_Boolean, YAC_POOL_PRIORITY_HIGH);


/* @method operator&nbsp;unary!:Boolean
Logical not
@return new Boolean object holding the result
*/
/* @method operator&nbsp;unary~:Boolean
Bitwise not
@return new Boolean object holding the result
*/
/* @method operator&nbsp;+,boolean v:Boolean
Add the given value
@arg v Addend
@return new Boolean object holding the result
*/
/* @method operator&nbsp;-,boolean v:Boolean
Subtract the given value
@arg v Minuend
@return new Boolean object holding the result
*/
/* @method operator&nbsp;*,boolean v:Boolean
Multiply by the given value
@arg v Factor
@return new Boolean object holding the result
*/
/* @method operator&nbsp;/,boolean v:Boolean
Divide by the given value
@arg v Dividend
@return new Boolean object holding the result
*/
/* @method operator&nbsp;&#38;,boolean v:Boolean
Calculate reminder of this divided by the given value
@arg v Dividend
@return new Boolean object holding the result
*/
/* @method operator&nbsp;&lt;&lt;,boolean v:Boolean
Logical shift left
@arg v How many bits to shift left
@return new Boolean object holding the result
*/
/* @method operator&nbsp;&gt;&gt;,boolean v:Boolean
Logical shift right
@arg v How many bits to shift right
@return new Boolean object holding the result
*/
/* @method operator&nbsp;==,boolean v:boolean
Compare to the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;!=,boolean v:boolean
Check whether the given value does not equal this value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;=,boolean v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;,boolean v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;=,boolean v:boolean
Check whether this value is equal to or greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;,boolean v:boolean
Check whether this value is greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&amp;,boolean v:Boolean
Bitwise AND
@arg v Right-hand side operand
@return new Boolean object holding the result
*/
/* @method operator&nbsp;|,boolean v:Boolean
Bitwise OR
@arg v Right-hand side operand
@return new Boolean object holding the result
*/
/* @method operator&nbsp;^,boolean v:Boolean
Bitwise EOR/XOR (exclusive or)
@arg v Right-hand side operand
@return new Boolean object holding the result
*/
/* @method operator&nbsp;&amp;&amp;,boolean v:boolean
Logical AND
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;||,boolean v:boolean
Logical OR
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;^^,boolean v:boolean
Logical EOR/XOR (exclusive OR)
@arg v Right-hand side operand
@return Result value
*/


/* @method setValue,boolean v

Set a new value.

@arg v The new value
@see getValue
*/
   YM //void setValue          (sSI);

/* @method getValue:boolean

Return the current value.

@return Current value
@see setValue
*/
      YM //sSI  getValue          (void);

/* @method getString:String

Return a string representation of this number object.

@return a new §String instance holding a string representation of this number object
*/
      YM //void  _getString       (YAC_Value *_r);

/* @method New,boolean v:Boolean

Create a new Boolean object and initialize it with the given value.

@arg v The value to assign the new object
@return new Boolean object
*/
      YM //void _New           (sSI, YAC_Value *_r);

/* @method News,String s:Boolean

Create a new Boolean object and initialize it with the given String.

The §String will be converted to an integer number before assignment.

The strings "true","1" and "false","0" will map to 1 resp. 0.

@return new Boolean object
*/
      YM //void _News          (YAC_Object *_s, YAC_Value *_r);

};
#include "ying_core_Boolean.cpp"
YAC_C_CORE_POOLED(_Boolean, "Boolean", YAC_CLID_BOOLEAN);

//------------------------------------------------------------------------------------- UnsignedByte
/* @class UnsignedByte,Object

A number object that wraps an unsigned 8bit integer value.

@see Boolean
@see Byte
@see Double
@see Float
@see Integer
@see Long
@see Short
@see UnsignedInteger
@see UnsignedLong
@see UnsignedShort
*/
YC class _UnsignedByte : public YAC_UnsignedByte {
public:
   YAC_POOLED(_UnsignedByte, YAC_POOL_PRIORITY_HIGH);

/* @method operator&nbsp;unary-:UnsignedByte
Negate value
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;unary!:UnsignedByte
Logical not
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;unary~:UnsignedByte
Bitwise not
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;+,int v:UnsignedByte
Add the given value
@arg v Addend
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;-,int v:UnsignedByte
Subtract the given value
@arg v Minuend
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;*,int v:UnsignedByte
Multiply by the given value
@arg v Factor
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;/,int v:UnsignedByte
Divide by the given value
@arg v Dividend
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;&#38;,int v:UnsignedByte
Calculate reminder of this divided by the given value
@arg v Dividend
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;&lt;&lt;,int v:UnsignedByte
Logical shift left
@arg v How many bits to shift left
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;&gt;&gt;,int v:UnsignedByte
Logical shift right
@arg v How many bits to shift right
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;==,int v:boolean
Compare to the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;!=,int v:boolean
Check whether the given value does not equal this value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;=,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;=,int v:boolean
Check whether this value is equal to or greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;,int v:boolean
Check whether this value is greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&amp;,int v:UnsignedByte
Bitwise AND
@arg v Right-hand side operand
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;|,int v:UnsignedByte
Bitwise OR
@arg v Right-hand side operand
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;^,int v:UnsignedByte
Bitwise EOR/XOR (exclusive or)
@arg v Right-hand side operand
@return new UnsignedByte object holding the result
*/
/* @method operator&nbsp;&amp;&amp;,int v:boolean
Logical AND
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;||,int v:boolean
Logical OR
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;^^,int v:boolean
Logical EOR/XOR (exclusive OR)
@arg v Right-hand side operand
@return Result value
*/


/* @method setValue,byte v

Set a new value.

@arg v The new value
@see getValue
*/
   YM //void setValue          (sSI);

/* @method getValue:byte

Return the current value.

@return Current value
@see setValue
*/
      YM //sSI  getValue          (void);

/* @method printf,String fmt:String

Return a string representation of this number object that is formatted using the given ANSI-C format string.

@arg fmt ANSI-C style format string.
@return new String instance holding a formatted string representation of this number object.
*/
   YM //void  _printf_YAC_RVAL   (YAC_Object*, YAC_Value *_r);
   YM //void  _printf_YAC_RARG   (YAC_Object*, YAC_Object *_r);

/* @method getString:String

Return a string representation of this number object.

@return a new §String instance holding a string representation of this number object
*/
      YM //void  _getString       (YAC_Value *_r);

/* @method New,byte v:UnsignedByte

Create a new UnsignedByte object and initialize it with the given value.

@arg v The value to assign the new object
@return new UnsignedByte object
*/
      YM //void  _New             (sSI, YAC_Value *_r);

};
#include "ying_core_UnsignedByte.cpp"
YAC_C_CORE_POOLED(_UnsignedByte, "UnsignedByte", YAC_CLID_UNSIGNEDBYTE);

//------------------------------------------------------------------------------------- Byte
/* @class Byte,Object

A number object that wraps a signed 8bit integer value.

@see Boolean
@see Double
@see Float
@see Integer
@see Long
@see Short
@see UnsignedByte
@see UnsignedInteger
@see UnsignedLong
@see UnsignedShort
*/
YC class _Byte : public YAC_Byte {
public:
   YAC_POOLED(_Byte, YAC_POOL_PRIORITY_HIGH);

/* @method operator&nbsp;unary-:Byte
Negate value
@return new Byte object holding the result
*/
/* @method operator&nbsp;unary!:Byte
Logical not
@return new Byte object holding the result
*/
/* @method operator&nbsp;unary~:Byte
Bitwise not
@return new Byte object holding the result
*/
/* @method operator&nbsp;+,int v:Byte
Add the given value
@arg v Addend
@return new Byte object holding the result
*/
/* @method operator&nbsp;-,int v:Byte
Subtract the given value
@arg v Minuend
@return new Byte object holding the result
*/
/* @method operator&nbsp;*,int v:Byte
Multiply by the given value
@arg v Factor
@return new Byte object holding the result
*/
/* @method operator&nbsp;/,int v:Byte
Divide by the given value
@arg v Dividend
@return new Byte object holding the result
*/
/* @method operator&nbsp;&#38;,int v:Byte
Calculate reminder of this divided by the given value
@arg v Dividend
@return new Byte object holding the result
*/
/* @method operator&nbsp;&lt;&lt;,int v:Byte
Logical shift left
@arg v How many bits to shift left
@return new Byte object holding the result
*/
/* @method operator&nbsp;&gt;&gt;,int v:Byte
Logical shift right
@arg v How many bits to shift right
@return new Byte object holding the result
*/
/* @method operator&nbsp;==,int v:boolean
Compare to the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;!=,int v:boolean
Check whether the given value does not equal this value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;=,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;=,int v:boolean
Check whether this value is equal to or greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;,int v:boolean
Check whether this value is greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&amp;,int v:Byte
Bitwise AND
@arg v Right-hand side operand
@return new Byte object holding the result
*/
/* @method operator&nbsp;|,int v:Byte
Bitwise OR
@arg v Right-hand side operand
@return new Byte object holding the result
*/
/* @method operator&nbsp;^,int v:Byte
Bitwise EOR/XOR (exclusive or)
@arg v Right-hand side operand
@return new Byte object holding the result
*/
/* @method operator&nbsp;&amp;&amp;,int v:boolean
Logical AND
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;||,int v:boolean
Logical OR
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;^^,int v:boolean
Logical EOR/XOR (exclusive OR)
@arg v Right-hand side operand
@return Result value
*/


/* @method setValue,byte v

Set a new value.

@arg v The new value
@see getValue
*/
   YM //void setValue          (sSI);

/* @method getValue:byte

Return the current value.

@return Current value
@see setValue
*/
      YM //sSI  getValue          (void);

/* @method printf,String fmt:String

Return a string representation of this number object that is formatted using the given ANSI-C format string.

@arg fmt ANSI-C style format string.
@return new String instance holding a formatted string representation of this number object.
*/
   YM //void  _printf_YAC_RVAL   (YAC_Object*, YAC_Value *_r);
   YM //void  _printf_YAC_RARG   (YAC_Object*, YAC_Object *_r);

/* @method getString:String

Return a string representation of this number object.

@return a new §String instance holding a string representation of this number object
*/
      YM //void  _getString       (YAC_Value *_r);

/* @method New,byte v:Byte

Create a new Byte object and initialize it with the given value.

@arg v The value to assign the new object
@return new Byte object
*/
      YM //void  _New             (sSI, YAC_Value *_r);

};
#include "ying_core_Byte.cpp"
YAC_C_CORE_POOLED(_Byte, "Byte", YAC_CLID_BYTE);

//------------------------------------------------------------------------------------- UnsignedShort
/* @class UnsignedShort,Object

A number object that wraps an unsigned 16bit integer value.

@see Boolean
@see Byte
@see Double
@see Float
@see Integer
@see Long
@see Short
@see UnsignedByte
@see UnsignedInteger
@see UnsignedLong
*/
YC class _UnsignedShort : public YAC_UnsignedShort {
public:
   YAC_POOLED(_UnsignedShort, YAC_POOL_PRIORITY_HIGH);

/* @method operator&nbsp;unary-:UnsignedShort
Negate value
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;unary!:UnsignedShort
Logical not
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;unary~:UnsignedShort
Bitwise not
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;+,int v:UnsignedShort
Add the given value
@arg v Addend
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;-,int v:UnsignedShort
Subtract the given value
@arg v Minuend
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;*,int v:UnsignedShort
Multiply by the given value
@arg v Factor
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;/,int v:UnsignedShort
Divide by the given value
@arg v Dividend
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;&#38;,int v:UnsignedShort
Calculate reminder of this divided by the given value
@arg v Dividend
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;&lt;&lt;,int v:UnsignedShort
Logical shift left
@arg v How many bits to shift left
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;&gt;&gt;,int v:UnsignedShort
Logical shift right
@arg v How many bits to shift right
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;==,int v:boolean
Compare to the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;!=,int v:boolean
Check whether the given value does not equal this value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;=,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;=,int v:boolean
Check whether this value is equal to or greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;,int v:boolean
Check whether this value is greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&amp;,int v:UnsignedShort
Bitwise AND
@arg v Right-hand side operand
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;|,int v:UnsignedShort
Bitwise OR
@arg v Right-hand side operand
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;^,int v:UnsignedShort
Bitwise EOR/XOR (exclusive or)
@arg v Right-hand side operand
@return new UnsignedShort object holding the result
*/
/* @method operator&nbsp;&amp;&amp;,int v:boolean
Logical AND
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;||,int v:boolean
Logical OR
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;^^,int v:boolean
Logical EOR/XOR (exclusive OR)
@arg v Right-hand side operand
@return Result value
*/


/* @method setValue,short v

Set a new value.

@arg v The new value
@see getValue
*/
   YM //void setValue          (sSI);

/* @method getValue:short

Return the current value.

@return Current value
@see setValue
*/
   YM //sSI  getValue          (void);

/* @method printf,String fmt:String

Return a string representation of this number object that is formatted using the given ANSI-C format string.

@arg fmt ANSI-C style format string.
@return new String instance holding a formatted string representation of this number object.
*/
   YM //void  _printf_YAC_RVAL   (YAC_Object*, YAC_Value *_r);
   YM //void  _printf_YAC_RARG   (YAC_Object*, YAC_Object *_r);

/* @method getString:String

Return a string representation of this number object.

@return a new §String instance holding a string representation of this number object
*/
      YM //void  _getString       (YAC_Value *_r);

/* @method New,short v:UnsignedShort

Create a new UnsignedShort object and initialize it with the given value.

@arg v The value to assign the new object
@return new UnsignedShort object
*/
      YM //void  _New             (sSI, YAC_Value *_r);

};
#include "ying_core_UnsignedShort.cpp"
YAC_C_CORE_POOLED(_UnsignedShort, "UnsignedShort", YAC_CLID_UNSIGNEDSHORT);

//------------------------------------------------------------------------------------- Short
/* @class Short,Object

A number object that wraps a signed 16bit integer value.

@see Boolean
@see Byte
@see Double
@see Float
@see Integer
@see Long
@see UnsignedByte
@see UnsignedInteger
@see UnsignedLong
@see UnsignedShort
*/
YC class _Short : public YAC_Short {
public:
   YAC_POOLED(_Short, YAC_POOL_PRIORITY_HIGH);

/* @method operator&nbsp;unary-:Short
Negate value
@return new Short object holding the result
*/
/* @method operator&nbsp;unary!:Short
Logical not
@return new Short object holding the result
*/
/* @method operator&nbsp;unary~:Short
Bitwise not
@return new Short object holding the result
*/
/* @method operator&nbsp;+,int v:Short
Add the given value
@arg v Addend
@return new Short object holding the result
*/
/* @method operator&nbsp;-,int v:Short
Subtract the given value
@arg v Minuend
@return new Short object holding the result
*/
/* @method operator&nbsp;*,int v:Short
Multiply by the given value
@arg v Factor
@return new Short object holding the result
*/
/* @method operator&nbsp;/,int v:Short
Divide by the given value
@arg v Dividend
@return new Short object holding the result
*/
/* @method operator&nbsp;&#38;,int v:Short
Calculate reminder of this divided by the given value
@arg v Dividend
@return new Short object holding the result
*/
/* @method operator&nbsp;&lt;&lt;,int v:Short
Logical shift left
@arg v How many bits to shift left
@return new Short object holding the result
*/
/* @method operator&nbsp;&gt;&gt;,int v:Short
Logical shift right
@arg v How many bits to shift right
@return new Short object holding the result
*/
/* @method operator&nbsp;==,int v:boolean
Compare to the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;!=,int v:boolean
Check whether the given value does not equal this value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;=,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;=,int v:boolean
Check whether this value is equal to or greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;,int v:boolean
Check whether this value is greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&amp;,int v:Short
Bitwise AND
@arg v Right-hand side operand
@return new Short object holding the result
*/
/* @method operator&nbsp;|,int v:Short
Bitwise OR
@arg v Right-hand side operand
@return new Short object holding the result
*/
/* @method operator&nbsp;^,int v:Short
Bitwise EOR/XOR (exclusive or)
@arg v Right-hand side operand
@return new Short object holding the result
*/
/* @method operator&nbsp;&amp;&amp;,int v:boolean
Logical AND
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;||,int v:boolean
Logical OR
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;^^,int v:boolean
Logical EOR/XOR (exclusive OR)
@arg v Right-hand side operand
@return Result value
*/



/* @method setValue,short v

Set a new value.

@arg v The new value
@see getValue
*/
   YM //void setValue          (sSI);

/* @method getValue:short

Return the current value.

@return Current value
@see setValue
*/
   YM //sSI  getValue          (void);

/* @method printf,String fmt:String

Return a string representation of this number object that is formatted using the given ANSI-C format string.

@arg fmt ANSI-C style format string.
@return new String instance holding a formatted string representation of this number object.
*/
   YM //void  _printf_YAC_RVAL   (YAC_Object*, YAC_Value *_r);
   YM //void  _printf_YAC_RARG   (YAC_Object*, YAC_Object *_r);

/* @method getString:String

Return a string representation of this number object.

@return a new §String instance holding a string representation of this number object
*/
   YM //void  _getString       (YAC_Value *_r);

/* @method New,short v:Short

Create a new Short object and initialize it with the given value.

@arg v The value to assign the new object
@return new Short object
*/
   YM //void  _New             (sSI, YAC_Value *_r);

};
#include "ying_core_Short.cpp"
YAC_C_CORE_POOLED(_Short, "Short", YAC_CLID_SHORT);

//------------------------------------------------------------------------------------- UnsignedInteger
/* @class UnsignedInteger,Object

A number object that wraps an unsigned 32bit integer value.

@see Boolean
@see Byte
@see Double
@see Float
@see Integer
@see Long
@see Short
@see UnsignedByte
@see UnsignedLong
@see UnsignedShort
*/
YC class _UnsignedInteger : public YAC_UnsignedInteger {
public:
   YAC_POOLED(_UnsignedInteger, YAC_POOL_PRIORITY_HIGH);

/* @method operator&nbsp;unary-:UnsignedInteger
Negate value
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;unary!:UnsignedInteger
Logical not
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;unary~:UnsignedInteger
Bitwise not
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;+,int v:UnsignedInteger
Add the given value
@arg v Addend
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;-,int v:UnsignedInteger
Subtract the given value
@arg v Minuend
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;*,int v:UnsignedInteger
Multiply by the given value
@arg v Factor
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;/,int v:UnsignedInteger
Divide by the given value
@arg v Dividend
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;&#38;,int v:UnsignedInteger
Calculate reminder of this divided by the given value
@arg v Dividend
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;&lt;&lt;,int v:UnsignedInteger
Logical shift left
@arg v How many bits to shift left
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;&gt;&gt;,int v:UnsignedInteger
Logical shift right
@arg v How many bits to shift right
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;==,int v:boolean
Compare to the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;!=,int v:boolean
Check whether the given value does not equal this value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;=,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;=,int v:boolean
Check whether this value is equal to or greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;,int v:boolean
Check whether this value is greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&amp;,int v:UnsignedInteger
Bitwise AND
@arg v Right-hand side operand
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;|,int v:UnsignedInteger
Bitwise OR
@arg v Right-hand side operand
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;^,int v:UnsignedInteger
Bitwise EOR/XOR (exclusive or)
@arg v Right-hand side operand
@return new UnsignedInteger object holding the result
*/
/* @method operator&nbsp;&amp;&amp;,int v:boolean
Logical AND
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;||,int v:boolean
Logical OR
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;^^,int v:boolean
Logical EOR/XOR (exclusive OR)
@arg v Right-hand side operand
@return Result value
*/



/* @method setValue,int v

Set a new value.

@arg v The new value
@see getValue
*/
   YM //void setValue          (sSI);

/* @method getValue:int

Return the current value.

@return Current value
@see setValue
*/
      YM //sSI  getValue          (void);

/* @method printf,String fmt:String

Return a string representation of this number object that is formatted using the given ANSI-C format string.

@arg fmt ANSI-C style format string.
@return new String instance holding a formatted string representation of this number object.
*/
   YM //void  _printf_YAC_RVAL   (YAC_Object*, YAC_Value *_r);
   YM //void  _printf_YAC_RARG   (YAC_Object*, YAC_Object *_r);

/* @method getString:String

Return a string representation of this number object.

@return a new §String instance holding a string representation of this number object
*/
      YM //void  _getString       (YAC_Value *_r);

/* @method New,int v:UnsignedInteger

Create a new UnsignedInteger object and initialize it with the given value.

@arg v The value to assign the new object
@return new UnsignedInteger object
*/
   YM //void  _New             (sSI, YAC_Value *_r);

};
#include "ying_core_UnsignedInteger.cpp"
YAC_C_CORE_POOLED(_UnsignedInteger, "UnsignedInteger", YAC_CLID_UNSIGNEDINTEGER);

//------------------------------------------------------------------------------------- Integer
/* @class Integer,Object

A number object that wraps a signed 32bit integer value.

@see Boolean
@see Byte
@see Double
@see Float
@see Long
@see Short
@see UnsignedByte
@see UnsignedInteger
@see UnsignedLong
@see UnsignedShort
*/
YC class _Integer : public YAC_Integer {
public:
   YAC_POOLED(_Integer, YAC_POOL_PRIORITY_HIGH);

/* @method operator&nbsp;unary-:Integer
Negate value
@return new Integer object holding the result
*/
/* @method operator&nbsp;unary!:Integer
Logical not
@return new Integer object holding the result
*/
/* @method operator&nbsp;unary~:Integer
Bitwise not
@return new Integer object holding the result
*/
/* @method operator&nbsp;+,int v:Integer
Add the given value
@arg v Addend
@return new Integer object holding the result
*/
/* @method operator&nbsp;-,int v:Integer
Subtract the given value
@arg v Minuend
@return new Integer object holding the result
*/
/* @method operator&nbsp;*,int v:Integer
Multiply by the given value
@arg v Factor
@return new Integer object holding the result
*/
/* @method operator&nbsp;/,int v:Integer
Divide by the given value
@arg v Dividend
@return new Integer object holding the result
*/
/* @method operator&nbsp;&#38;,int v:Integer
Calculate reminder of this divided by the given value
@arg v Dividend
@return new Integer object holding the result
*/
/* @method operator&nbsp;&lt;&lt;,int v:Integer
Logical shift left
@arg v How many bits to shift left
@return new Integer object holding the result
*/
/* @method operator&nbsp;&gt;&gt;,int v:Integer
Logical shift right
@arg v How many bits to shift right
@return new Integer object holding the result
*/
/* @method operator&nbsp;==,int v:boolean
Compare to the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;!=,int v:boolean
Check whether the given value does not equal this value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;=,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;,int v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;=,int v:boolean
Check whether this value is equal to or greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;,int v:boolean
Check whether this value is greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&amp;,int v:Integer
Bitwise AND
@arg v Right-hand side operand
@return new Integer object holding the result
*/
/* @method operator&nbsp;|,int v:Integer
Bitwise OR
@arg v Right-hand side operand
@return new Integer object holding the result
*/
/* @method operator&nbsp;^,int v:Integer
Bitwise EOR/XOR (exclusive or)
@arg v Right-hand side operand
@return new Integer object holding the result
*/
/* @method operator&nbsp;&amp;&amp;,int v:boolean
Logical AND
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;||,int v:boolean
Logical OR
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;^^,int v:boolean
Logical EOR/XOR (exclusive OR)
@arg v Right-hand side operand
@return Result value
*/



/* @method setValue,int v

Set a new value.

@arg v The new value
@see getValue
*/
   YM //void setValue          (sSI);

/* @method getValue:int

Return the current value.

@return Current value
@see setValue
*/
      YM //sSI  getValue          (void);

/* @method printf,String fmt:String

Return a string representation of this number object that is formatted using the given ANSI-C format string.

@arg fmt ANSI-C style format string.
@return new String instance holding a formatted string representation of this number object.
*/
   YM //void  _printf_YAC_RVAL   (YAC_Object*, YAC_Value *_r);
   YM //void  _printf_YAC_RARG   (YAC_Object*, YAC_Object *_r);

/* @method getString:String

Return a string representation of this number object.

@return a new §String instance holding a string representation of this number object
*/
      YM //void  _getString       (YAC_Value *_r);

/* @method New,int v:Integer

Create a new Integer object and initialize it with the given value.

@arg v The value to assign the new object
@return new Integer object
*/
   YM //void  _New             (sSI, YAC_Value *_r);

};
#include "ying_core_Integer.cpp"
YAC_C_CORE_POOLED(_Integer, "Integer", YAC_CLID_INTEGER);
//------------------------------------------------------------------------------------- UnsignedLong
/* @class UnsignedLong,Object

A number object that wraps an unsigned 64bit integer value.

@see Boolean
@see Byte
@see Double
@see Float
@see Integer
@see Long
@see Short
@see UnsignedByte
@see UnsignedInteger
@see UnsignedShort
*/
YC class _UnsignedLong : public YAC_UnsignedLong {
public:
   YAC_POOLED(_UnsignedLong, YAC_POOL_PRIORITY_HIGH);

/* @method operator&nbsp;unary-:UnsignedLong
Negate value
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;unary!:UnsignedLong
Logical not
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;unary~:UnsignedLong
Bitwise not
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;+,UnsignedLong v:UnsignedLong
Add the given value
@arg v Addend
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;-,UnsignedLong v:UnsignedLong
Subtract the given value
@arg v Minuend
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;*,UnsignedLong v:UnsignedLong
Multiply by the given value
@arg v Factor
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;/,UnsignedLong v:UnsignedLong
Divide by the given value
@arg v Dividend
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;&#38;,UnsignedLong v:UnsignedLong
Calculate reminder of this divided by the given value
@arg v Dividend
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;&lt;&lt;,UnsignedLong v:UnsignedLong
Logical shift left
@arg v How many bits to shift left
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;&gt;&gt;,UnsignedLong v:UnsignedLong
Logical shift right
@arg v How many bits to shift right
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;==,UnsignedLong v:boolean
Compare to the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;!=,UnsignedLong v:boolean
Check whether the given value does not equal this value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;=,UnsignedLong v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;,UnsignedLong v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;=,UnsignedLong v:boolean
Check whether this value is equal to or greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;,UnsignedLong v:boolean
Check whether this value is greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&amp;,UnsignedLong v:UnsignedLong
Bitwise AND
@arg v Right-hand side operand
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;|,UnsignedLong v:UnsignedLong
Bitwise OR
@arg v Right-hand side operand
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;^,UnsignedLong v:UnsignedLong
Bitwise EOR/XOR (exclusive or)
@arg v Right-hand side operand
@return new UnsignedLong object holding the result
*/
/* @method operator&nbsp;&amp;&amp;,UnsignedLong v:boolean
Logical AND
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;||,UnsignedLong v:boolean
Logical OR
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;^^,UnsignedLong v:boolean
Logical EOR/XOR (exclusive OR)
@arg v Right-hand side operand
@return Result value
*/




/* @method setValue,int v

Set a new value.

@arg v The new value
@see getValue
*/
   YM //void setValue          (sSI);

/* @method getValue:int

Return the current value.

@return Current value
@see setValue
*/
      YM //sSI  getValue          (void);

/* @method printf,String fmt:String

Return a string representation of this number object that is formatted using the given ANSI-C format string.

@arg fmt ANSI-C style format string.
@return new String instance holding a formatted string representation of this number object.
*/
   YM //void  _printf_YAC_RVAL   (YAC_Object*, YAC_Value *_r);
   YM //void  _printf_YAC_RARG   (YAC_Object*, YAC_Object *_r);

/* @method getString:String

Return a string representation of this number object.

@return a new §String instance holding a string representation of this number object
*/
   YM //void  _getString       (YAC_Value *_r);

/* @method Newi,int v:UnsignedLong

Create a new UnsignedLong object and initialize it with the given value.

@arg v The value to assign the new object
@return new UnsignedLong object
*/
   YM //void  _Newi            (sSI, YAC_Value *_r);

/* @method News,String s:UnsignedLong

Create a new UnsignedLong object and initialize it with the given String.

The §String will be converted to an unsigned 64bit integer number before assignment.

@return new UnsignedLong object
*/
      YM //void  _News            (YAC_Object *_s, YAC_Value *_r);
};
#include "ying_core_UnsignedLong.cpp"
YAC_C_CORE_POOLED(_UnsignedLong, "UnsignedLong", YAC_CLID_UNSIGNEDLONG);
//------------------------------------------------------------------------------------- Long
/* @class Long,Object

A number object that wraps a signed 32bit integer value.

@see Boolean
@see Byte
@see Double
@see Float
@see Integer
@see Short
@see UnsignedByte
@see UnsignedInteger
@see UnsignedLong
@see UnsignedShort
*/
YC class _Long : public YAC_Long {
public:
   YAC_POOLED(_Long, YAC_POOL_PRIORITY_HIGH);

/* @method operator&nbsp;unary-:Long
Negate value
@return new Long object holding the result
*/
/* @method operator&nbsp;unary!:Long
Logical not
@return new Long object holding the result
*/
/* @method operator&nbsp;unary~:Long
Bitwise not
@return new Long object holding the result
*/
/* @method operator&nbsp;+,Long v:Long
Add the given value
@arg v Addend
@return new Long object holding the result
*/
/* @method operator&nbsp;-,Long v:Long
Subtract the given value
@arg v Minuend
@return new Long object holding the result
*/
/* @method operator&nbsp;*,Long v:Long
Multiply by the given value
@arg v Factor
@return new Long object holding the result
*/
/* @method operator&nbsp;/,Long v:Long
Divide by the given value
@arg v Dividend
@return new Long object holding the result
*/
/* @method operator&nbsp;&#38;,Long v:Long
Calculate reminder of this divided by the given value
@arg v Dividend
@return new Long object holding the result
*/
/* @method operator&nbsp;&lt;&lt;,Long v:Long
Logical shift left
@arg v How many bits to shift left
@return new Long object holding the result
*/
/* @method operator&nbsp;&gt;&gt;,Long v:Long
Logical shift right
@arg v How many bits to shift right
@return new Long object holding the result
*/
/* @method operator&nbsp;==,Long v:boolean
Compare to the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;!=,Long v:boolean
Check whether the given value does not equal this value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;=,Long v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;,Long v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;=,Long v:boolean
Check whether this value is equal to or greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;,Long v:boolean
Check whether this value is greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&amp;,Long v:Long
Bitwise AND
@arg v Right-hand side operand
@return new Long object holding the result
*/
/* @method operator&nbsp;|,Long v:Long
Bitwise OR
@arg v Right-hand side operand
@return new Long object holding the result
*/
/* @method operator&nbsp;^,Long v:Long
Bitwise EOR/XOR (exclusive or)
@arg v Right-hand side operand
@return new Long object holding the result
*/
/* @method operator&nbsp;&amp;&amp;,Long v:boolean
Logical AND
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;||,Long v:boolean
Logical OR
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;^^,Long v:boolean
Logical EOR/XOR (exclusive OR)
@arg v Right-hand side operand
@return Result value
*/



/* @method setValue,int v

Set a new value.

@arg v The new value
@see getValue
*/
   YM //void setValue          (sSI);

/* @method getValue:int

Return the current value.

The upper 32 bits will be silently stripped off.

@return Current value
@see setValue
*/
   YM //sSI  getValue          (void);

/* @method printf,String fmt:String

Return a string representation of this number object that is formatted using the given ANSI-C format string.

@arg fmt ANSI-C style format string.
@return new String instance holding a formatted string representation of this number object.
*/
   YM //void  _printf_YAC_RVAL   (YAC_Object*, YAC_Value *_r);
   YM //void  _printf_YAC_RARG   (YAC_Object*, YAC_Object *_r);

/* @method getString:String

Return a string representation of this number object.

@return a new §String instance holding a string representation of this number object
*/
      YM //void  _getString       (YAC_Value *_r);

/* @method Newi,int v:Long

Create a new Long object and initialize it with the given value.

@arg v The value to assign the new object
@return new Long object
*/
      YM //void  _Newi            (sSI, YAC_Value *_r);

/* @method News,String s:Long

Create a new Long object and initialize it with the given String.

The §String will be converted to a signed 64bit integer number before assignment.

@return new Long object
*/
      YM //void  _News            (YAC_Object *_s, YAC_Value *_r);
};
#include "ying_core_Long.cpp"
YAC_C_CORE_POOLED(_Long, "Long", YAC_CLID_LONG);
//------------------------------------------------------------------------------------- Float
/* @class Float,Object

A number object that wraps a 32bit IEEE floating point value.

@see Boolean
@see Byte
@see Double
@see Integer
@see Long
@see Short
@see UnsignedByte
@see UnsignedInteger
@see UnsignedLong
@see UnsignedShort
*/
YC class _Float : public YAC_Float {
public:
   YAC_POOLED(_Float, YAC_POOL_PRIORITY_HIGH);

/* @method operator&nbsp;unary-:Float
Negate value
@return new Float object holding the result
*/
/* @method operator&nbsp;unary!:Float
Logical not.

This value is temporarily converted to an integer before the operation is performed.
@return new Float object holding the result
*/
/* @method operator&nbsp;unary~:Float
Bitwise not

This value is temporarily converted to an integer before the operation is performed.
@return new Float object holding the result
*/
/* @method operator&nbsp;+,float v:Float
Add the given value
@arg v Addend
@return new Float object holding the result
*/
/* @method operator&nbsp;-,float v:Float
Subtract the given value
@arg v Minuend
@return new Float object holding the result
*/
/* @method operator&nbsp;*,float v:Float
Multiply by the given value
@arg v Factor
@return new Float object holding the result
*/
/* @method operator&nbsp;/,float v:Float
Divide by the given value
@arg v Dividend
@return new Float object holding the result
*/
/* @method operator&nbsp;&#38;,int v:Float
Calculate reminder of this divided by the given value.

This value is temporarily converted to an integer before the operation is performed.
@arg v Dividend
@return new Float object holding the result
*/
/* @method operator&nbsp;&lt;&lt;,int v:Float
Logical shift left

This value is temporarily converted to an integer before the operation is performed.

@arg v How many bits to shift left
@return new Float object holding the result
*/
/* @method operator&nbsp;&gt;&gt;,int v:Float
Logical shift right

This value is temporarily converted to an integer before the operation is performed.
@arg v How many bits to shift right
@return new Float object holding the result
*/
/* @method operator&nbsp;==,float v:boolean
Compare to the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;!=,float v:boolean
Check whether the given value does not equal this value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;=,float v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;,float v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;=,float v:boolean
Check whether this value is equal to or greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;,float v:boolean
Check whether this value is greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&amp;,int v:Float
Bitwise AND

This value is temporarily converted to an integer before the operation is performed.

@arg v Right-hand side operand
@return new Float object holding the result
*/
/* @method operator&nbsp;|,int v:Float
Bitwise OR

This value is temporarily converted to an integer before the operation is performed.
@arg v Right-hand side operand
@return new Float object holding the result
*/
/* @method operator&nbsp;^,int v:Float
Bitwise EOR/XOR (exclusive or)

This value is temporarily converted to an integer before the operation is performed.
@arg v Right-hand side operand
@return new Float object holding the result
*/
/* @method operator&nbsp;&amp;&amp;,int v:boolean
Logical AND
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;||,int v:boolean
Logical OR
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;^^,int v:boolean
Logical EOR/XOR (exclusive OR)
@arg v Right-hand side operand
@return Result value
*/



/* @method setValue,float v

Set a new value.

@arg v The new value
@see getValue
*/
   YM //void setValue          (sF32);

/* @method getValue:float

Return the current value.

@return Current value
@see setValue
*/
   YM //sF32 getValue          (void);

/* @method printf,String fmt:String

Return a string representation of this number object that is formatted using the given ANSI-C format string.

@arg fmt ANSI-C style format string.
@return new String instance holding a formatted string representation of this number object.
*/
   YM //void  _printf_YAC_RVAL   (YAC_Object*, YAC_Value *_r);
   YM //void  _printf_YAC_RARG   (YAC_Object*, YAC_Object *_r);

/* @method getString:String

Return a string representation of this number object.

@return a new §String instance holding a string representation of this number object
*/
      YM //void  _getString       (YAC_Value *_r);

/* @method New,float v:Float

Create a new Float object and initialize it with the given value.

@arg v The value to assign the new object
@return new Float object
*/
   YM //void  _New             (sF32, YAC_Value *_r);

};
#include "ying_core_Float.cpp"
YAC_C_CORE_POOLED(_Float, "Float", YAC_CLID_FLOAT);
//------------------------------------------------------------------------------------- Double
/* @class Double,Object

A number object that wraps a 64bit IEEE floating point value.

@see Boolean
@see Byte
@see Float
@see Integer
@see Long
@see Short
@see UnsignedByte
@see UnsignedInteger
@see UnsignedLong
@see UnsignedShort
*/
YC class _Double : public YAC_Double {
public:
   YAC_POOLED(_Double, YAC_POOL_PRIORITY_HIGH);

/* @method operator&nbsp;unary-:Double
Negate value
@return new Double object holding the result
*/
/* @method operator&nbsp;unary!:Double
Logical not

This value is temporarily converted to an integer before the operation is performed.
@return new Double object holding the result
*/
/* @method operator&nbsp;unary~:Double
Bitwise not

This value is temporarily converted to an integer before the operation is performed.
@return new Double object holding the result
*/
/* @method operator&nbsp;+,Double v:Double
Add the given value
@arg v Addend
@return new Double object holding the result
*/
/* @method operator&nbsp;-,Double v:Double
Subtract the given value
@arg v Minuend
@return new Double object holding the result
*/
/* @method operator&nbsp;*,Double v:Double
Multiply by the given value
@arg v Factor
@return new Double object holding the result
*/
/* @method operator&nbsp;/,Double v:Double
Divide by the given value
@arg v Dividend
@return new Double object holding the result
*/
/* @method operator&nbsp;&#38;,int v:Double
Calculate reminder of this divided by the given value.

This value is temporarily converted to an integer before the operation is performed.
@arg v Dividend
@return new Double object holding the result
*/
/* @method operator&nbsp;&lt;&lt;,int v:Double
Logical shift left

This value is temporarily converted to an integer before the operation is performed.

@arg v How many bits to shift left
@return new Double object holding the result
*/
/* @method operator&nbsp;&gt;&gt;,int v:Double
Logical shift right

This value is temporarily converted to an integer before the operation is performed.
@arg v How many bits to shift right
@return new Double object holding the result
*/
/* @method operator&nbsp;==,Double v:boolean
Compare to the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;!=,Double v:boolean
Check whether the given value does not equal this value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;=,Double v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&lt;,Double v:boolean
Check whether this value is less than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;=,Double v:boolean
Check whether this value is equal to or greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&gt;,Double v:boolean
Check whether this value is greater than the given value
@arg v Value to compare to
@return Comparison result
*/
/* @method operator&nbsp;&amp;,int v:Double
Bitwise AND

This value is temporarily converted to an integer before the operation is performed.
@arg v Right-hand side operand
@return new Double object holding the result
*/
/* @method operator&nbsp;|,int v:Double
Bitwise OR

This value is temporarily converted to an integer before the operation is performed.
@arg v Right-hand side operand
@return new Double object holding the result
*/
/* @method operator&nbsp;^,int v:Double
Bitwise EOR/XOR (exclusive or)

This value is temporarily converted to an integer before the operation is performed.
@arg v Right-hand side operand
@return new Double object holding the result
*/
/* @method operator&nbsp;&amp;&amp;,int v:boolean
Logical AND
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;||,int v:boolean
Logical OR
@arg v Right-hand side operand
@return Result value
*/
/* @method operator&nbsp;^^,int v:boolean
Logical EOR/XOR (exclusive OR)
@arg v Right-hand side operand
@return Result value
*/


/* @method setValue,float v

Set a new value.

@arg v The new value
@see getValue
*/
   YM //void setValue          (sF32);

/* @method getValue:double

Return the current value.

The value will silently be converted to a single precision float value.

@return Current value
@see setValue
*/
   YM //sF32 getValue          (void);

/* @method printf,String fmt:String

Return a string representation of this number object that is formatted using the given ANSI-C format string.

@arg fmt ANSI-C style format string.
@return new String instance holding a formatted string representation of this number object.
*/
   YM //void  _printf_YAC_RVAL   (YAC_Object*, YAC_Value *_r);
   YM //void  _printf_YAC_RARG   (YAC_Object*, YAC_Object *_r);

/* @method getString:String

Return a string representation of this number object.

@return a new §String instance holding a string representation of this number object
*/
   YM //void  _getString       (YAC_Value *_r);

/* @method Newi,int v:Double

Create a new Double object and initialize it with the given value.

@arg v The value to assign the new object
@return new Double object
*/
      YM //void  _Newi            (sSI, YAC_Value *_r);

/* @method Newf,float v:Double

Create a new Double object and initialize it with the given value.

@arg v The value to assign the new object
@return new Double object
*/
      YM //void  _Newf            (sF32, YAC_Value *_r);

/* @method News,String s:Double

Create a new Double object and initialize it with the given String.

The §String will be converted to a 64bit double precision floating point value before assignment.

@return new Double object
*/
      YM //void  _News            (YAC_Object *_s, YAC_Value *_r);

};
#include "ying_core_Double.cpp"
YAC_C_CORE_POOLED(_Double, "Double", YAC_CLID_DOUBLE);
//------------------------------------------------------------------------------------- Buffer
/* @class Buffer,Stream

An array of bytes.

*/
YC class _Buffer : public YAC_Buffer {
public:
   YAC_POOLED(_Buffer, YAC_POOL_PRIORITY_MEDIUM);

/* @method setSize,int num:boolean

Allocate the given number of bytes.

@arg num New buffer size
@return true if the allocation succeeded, false otherwise
*/
   YM //sBool setSize    (sUI _numbytes);

/* @method resize,int num:boolean

Change the size of the buffer.

@arg num New buffer size
@return true if the reallocation succeeded, false otherwise
*/
      YM //sBool resize    (sUI _numbytes);

/* @method free

Free buffer bytes
*/
      YM //void free       (void);

/* @method pokeI8,int off,byte value

Store byte at the given buffer offset

@arg off Where to storethe byte
@arg value Which byte to store
*/
      YM //void pokeI8     (sSI _offset, sSI _value);

/* @method pokeI14,int off,short value

Store short 7:7 integer at the given buffer offset.

The value byteorder will be swapped according to the current buffer byteorder.

@arg off Where to store the short
@arg value Which short to store
*/
      YM //void pokeI14    (sSI _offset, sSI _value);

/* @method pokeI16,int off,short value

Store short integer at the given buffer offset.

The value byteorder will be swapped according to the current buffer byteorder.

@arg off Where to store the short
@arg value Which short to store
*/
      YM //void pokeI16    (sSI _offset, sSI _value);

/* @method pokeI32,int off,int value

Store 32bit integer at the given buffer offset.

The value byteorder will be swapped according to the current buffer byteorder.

@arg off Where to store the 32bit integer
@arg value Which 32bit integer to store
*/
      YM //void pokeI32    (sSI _offset, sSI _value);

/* @method pokeF32,int off,float value

Store 32bit single precision floating point value at the given buffer offset.

@arg off Where to store the floating point value
@arg value Which floating point value to store
*/
      YM //void pokeF32    (sSI _offset, sF32 _value);

/* @method peekI8,int off:byte

Read byte from the given buffer offset

@arg off Where to read the byte from
@return Byte
*/
      YM //sSI  peekI8     (sSI _offset);

/* @method peekI14,int off:short

Read short 7:7 integer from the given buffer offset.

The return value byteorder will be swapped according to the current buffer byteorder.

@arg off Where to read the short integer from
@return Short integer
*/
      YM //sSI  peekI14    (sSI _offset);

/* @method peekI16,int off:short

Read short integer from the given buffer offset.

The return value byteorder will be swapped according to the current buffer byteorder.

@arg off Where to read the short integer from
@return Short integer
*/
      YM //sSI  peekI16    (sSI _offset);

/* @method peekI32,int off:int

Read 32bit integer from the given buffer offset.

The return value byteorder will be swapped according to the current buffer byteorder.

@arg off Where to read the 32bit integer from
@return 32bit integer
*/
      YM //sSI  peekI32    (sSI _offset);

/* @method peekF32,int off:float

Read 32bit single precision floating point value from the given buffer offset.

@arg off Where to read the 32bit floating point value from
@return 32bit single precision floating point value
*/
      YM //sF32 peekF32    (sSI _offset);

/* @method fillZero

Fill the entire buffer with 0.
*/
      YM //void fillZero   (void);

/* @method fillI8

Fill the entire buffer with byte 'c'.

@arg c Fill byte
*/
      YM //void fillI8     (sUI _c);

/* @method substring,String r,int off,int max

Extract 0 terminated string from buffer.

The result will be written to §String "r".
At most "max" characters will be written.

@arg r Receives the extracted string
@arg off Buffer offset
@arg max Maximum number of characters to extract
@return true when substring has been extracted successfully
*/
      YM //sBool substring  (YAC_String *_d, sUI _off, sUI _num);

/* @method getString,int off,int max:String

Extract 0 terminated string from buffer.

At most "max" characters will be extracted.

@arg off Buffer offset
@arg max Maximum number of characters to extract
@return new String instance holding the extracted string
*/
      YM //void getString  (sUI _off, sUI _len, YAC_Value *_r);

/* @method setString,int off,String s:boolean

Store 0 terminated string at the given buffer offset.

@arg off Where to store the string
@arg s Which string to store
@return true if the string has been stored successfully, false otherwise
*/
      YM //sBool setString  (sUI _off, YAC_String *_s);

/* @method gzip,Buffer src,int off,int len,int level:boolean

Compress the given source buffer.

This buffer will be resized to the compressed size.

@arg src Which buffer to compress
@arg off Source offset
@arg len Number of bytes to compress
@arg level Compression level (0..9)
@return true if the given buffer has been compressed successfully, false otherwise.
*/
      YM //sSI  gzip       (YAC_Object *, sSI, sSI, sSI _level);

/* @method gunzip,Buffer src,int off,int len,int uncompressedSize

Uncompress the given source buffer.

This buffer will be resized to the uncompressed size.

@arg src Which buffer to uncompress
@arg off Source buffer offset
@arg len Compressed size
@arg uncompressedSize (maximum) Uncompressed size
@return true if the given buffer has been uncompressed successfully, false otherwise.
*/
      YM //sSI  gunzip     (YAC_Object *, sSI, sSI, sSI _est_size);

/* @method getChecksum:int
Return checksum for this Buffer.

@return 32bit buffer checksum
*/
      YM //sSI  _getChecksum (void);

/* @method visit:Buffer b,int off,int len
Create view into other buffer.

The previous content of this buffer will be freed.

Make sure to keep the source buffer alive as long as the visited data in this buffer is accessed.

@arg buffer Which buffer to visit (source buffer)
@arg off Source buffer offset
@arg len Area size
@return true if the source buffer is allocated and the area is within bounds, false otherwise.
*/
      YM //sBool visit     (YAC_Buffer *_buffer, sUI _off, sUI _len);

/* @method base64Encode:String out:int
Base64-encode buffer region 0..io_offset and write to out §String.

@return Number of encoded chars (excluding ASCIIZ)
*/
      YM //sUI base64Encode (YAC_String *_out);

/* @method base64Decode:String in:int
Base64-decode String to buffer. Set io_offset to decoded size.

@return Number of decoded chars (excluding ASCIIZ)
*/
      YM //sUI base64Decode (YAC_String *_in);


/* @method compareBuffer:Buffer o:boolean
Compare buffer to other buffer (io_offset bytes).

@return true when bytes match, false otherwise
*/
      YM //sBool _compareBuffer (YAC_Buffer *_o);


/* @method truncate:int sz
Truncate buffer size (w/o reallocation).

Current size must be >= 'sz'.

@arg sz New buffer size (<= old size)
*/
      YM //void truncate (sUI _newSize);


/* @method readBufferBits:int startBit,int numBits:int
Treat buffer as bitstream and read up to 32 bits from the given start bit offset.

@arg startBit Start bit offset
@arg numBits Number of bits (up to 32)
@return Value
*/
      YM //sUI  readBufferBits     (sUI _startBit, sUI _numBits);


/* @method writeBufferBits:int startBit,int numBits,int val
Treat buffer as bitstream and write up to 32 bits to the given start bit offset.

@arg startBit Start bit offset
@arg numBits Number of bits (up to 32)
@arg val Value to store into buffer
@return Value
*/
      YM //void writeBufferBits    (sUI _startBit, sUI _numBits, sUI _val);


/* @method hexdump:int start,int num
Debug-print buffer(area) to console.

@arg Start offset
@arg Number of bytes
 */
      YM //void hexdump      (sUI _start, sUI _num);

/* @method ringWriteBuffer,int ringStartOff,int ringSz,Buffer src,int srcOff,int numBytes
   Copy buffer data to ringbuffer area within this buffer.

@arg ringStartOff Start of ringbuffer area
@arg ringSz Ringbuffer size
@arg writeOff Write offset (relative to start of ringbuffer area)
@arg src Source buffer
@arg srcOff Read offset
@arg numBytes Number of bytes
@return New write offset
*/
      YM //sUI ringWriteBuffer (sUI _ringStartOff, sUI _ringSz, sUI _writeOff, YAC_Buffer *_src, sUI _srcOff, sUI _numBytes);

/* @method ringReadBuffer,int ringStartOff,int ringSz,Buffer dst,int dstOff,int numBytes
   Copy ringbuffer area within this buffer to dst buffer.

@arg ringStartOff Start of ringbuffer area
@arg ringSz Ringbuffer size
@arg readOff Read offset (relative to start of ringbuffer area)
@arg src Destination buffer
@arg dstOff Write offset
@arg numBytes Number of bytes
@return New read offset
*/
      YM //sUI ringReadBuffer (sUI _ringStartOff, sUI _ringSz, sUI _readOff, YAC_Buffer *_dst, sUI _dstOff, sUI _numBytes);

};
#include "ying_core_Buffer.cpp"
YAC_C_CORE_POOLED(_Buffer, "Buffer", YAC_CLID_BUFFER);

//------------------------------------------------------------------------------------- SharedBuffer
/* @class SharedBuffer,Object

A shared memory buffer.

Use §peekI32, §pokeI32, §ringReadBuffer, §ringWriteBuffer to atomatically read / write 32bit integers.

Ring buffer sizes must be dword-aligned (but support un-aligned read/write offsets and sizes).

*/
YC class _SharedBuffer : public TKS_SharedBuffer {
public:
   YAC_POOLED(_Shared_Buffer, YAC_POOL_PRIORITY_LOW);

/* @method allocSharedMemory,int size:boolean

Create shared memory key (name) and allocate the given number of bytes.

@arg size How many bytes to allocate
@return true if the shared memory has been created successfully, false otherwise.
*/
   YM //sSI         allocSharedMemory      (sSI _size);

/* @method allocSharedMemoryByKey,String key,int size:boolean

Allocate shared memory using the given key (name).

@arg key Name of the shared memory area
@arg size Size of the shared memory area (number of bytes)
@return true if the shared memory has been created successfully, false otherwise
*/
   YM //sSI         allocSharedMemoryByKey (YAC_String *_shmKey, sSI _size);

/* @method getSharedMemoryKey:String

Return a (non-deletable) reference to the shared memory key (name).

@return (non-deletable) reference to the shared memory key (name).
*/
   YM //YAC_String *getSharedMemoryKey     (void);

/* @method visitSharedMemory,String key,int size

Open an already existing shared memory area using the given key (name) and size.

@arg key Name of the shared memory area
@arg size Size of the shared memory area (number of bytes)
@return true if the shared memory has been opened successfully, false otherwise
*/
   YM //sSI         visitSharedMemory      (YAC_String *_shmKey, sSI _size);

/* @method lock,float timeoutMS:boolean
Lock shared buffer.

@arg timeoutMS Time out in milliseconds (0=return immediately)
@return true when buffer has been locked, false when an error or timeout occured
*/
   YM //sBool       lock                   (sF32 _timeoutMS);

/* @method unlock
Unlock shared buffer.
*/
   YM //void        unlock                 (void);

/* @method isOpen:boolean
Check if shared memory handle is valid.

@return true when handle is valid
*/
   YM //sBool       isOpen                 (void);

/* @method isOpen:boolean
Check if shared memory is owned by this process.
*/
   YM //sBool       isOwner                (void);

};
#include "ying_core_SharedBuffer.cpp"
YAC_C_CORE_POOLED(_SharedBuffer, "SharedBuffer", TKS_CLID_SHAREDBUFFER);

//------------------------------------------------------------------------------------- Class
/* @class Class,Object

A scriptclass (instance).

@see ClassArray
*/
YC class _Class : public TKS_ScriptClassInstance {
public:
   YAC_POOLED(_Class, YAC_POOL_PRIORITY_HIGH);
};
#include "ying_core_Class.cpp"
YAC_C_CORE_POOLED(_Class, "Class", YAC_CLID_CLASS);

//------------------------------------------------------------------------------------- Envelope
/* @class Envelope,FloatArray

An array of time/value pairs.

Most of the methods assume that time is actually a number of ticks, i.e. an integer number.

The current time offset may be a fractional number resulting in a smooth interpolation of
successive event values if the current interpolation function is set to something other than
§ENV_SHRESET or §ENV_SH.

*/
YC class _Envelope : public TKS_Envelope {
public:
   YAC_POOLED(_Envelope, YAC_POOL_PRIORITY_MEDIUM);

/* @constant ENV_SH

Sample and hold (no interpolation)
*/

/* @constant ENV_LINEAR

Linear interpolation.

<code>y = y1 + (y2 - y1) * x</code>
*/

/* @constant ENV_COSINE

Cosine interpolation

<code>y = y1 + (y2 - y1) * (1 - cos(PI * x)) / 2</code>
*/

/* @constant ENV_QUADRATIC

Quadratic interpolation.

<code>y = (y1 + (y2 - y1) * x) * (1 - x<sup>2</sup>) + (y2 + (y3 - y2) * (x - 1)) * x<sup>2</sup></code>
*/

/* @constant ENV_CUBIC

Cubic interpolation

<code>y = (y1 + y3 - (y0 + y2) * x<sup>3</sup> + (2 * (y0 - y1) + y2 - y3) * x<sup>2</sup> + (y2 - y0) * x + y1</code>
*/

/* @constant ENV_QUINTIC

Quintic interpolation

<code>y = 3 * (y1 + y3 - y2 - y4) * x<sup>5</sup> + (8 * (y2 - y1) + 7 * (y4 - y3)) * x<sup>4</sup> + (6 * (y1 - y2) + 4 * (y3 - y4)) * x<sup>3</sup> + (y3 - y1) * x + y2</code>

*/

/* @constant ENV_SHRESET

Sample and hold + reset (no interpolation).

After reading a new key value, reset current value back to 0. Useful for one-shot triggers.
*/

/* @method get:float

Return (interpolated) value and reset new_event flag (SH, SHRESET).

@return Interpolated value
*/
   YM //sF32 get                   (void);



/* @method getAndResetNew,boolean:float

Return (interpolated) value. Only reset new event flag if bResetNew is true.

@return Interpolated value
@arg bResetNew If true, reset new_event flag after reading.
*/
   YM //sF32 getAndResetNew        (sBool _bResetNew);



/* @method reset

Reset time offset
*/
      YM //void reset                 (void);



/* @method setInterpolation,int interpol

Set the interpolation function.

@arg interpol Interpolation function id. Must be one of §ENV_SH, §ENV_LINEAR, §ENV_COSINE, §ENV_QUADRATIC, §ENV_CUBIC, §ENV_QUINTIC, §ENV_SHRESET.
*/
      YM //void setInterpolation      (sSI _funid);



/* @method getInterpolation:int

Return current interpolation function.

@return Interpolation function id. See ENV_xxx.
*/
      YM //sSI  getInterpolation      (void);



/* @method tickPrecise,float dt

Increase time offset by the given time delta.

@arg dt Time delta
*/
      YM //void tickPrecise           (sF32);



/* @method setSpeed,float speed

Set envelope base speed.

The default speed is 1.0f

@arg speed Base time delta
*/
      YM //void setSpeed              (sF32 _a);



/* @method getSpeed:float

Return the current envelope base speed.

@return The current envelope base speed.
*/
      YM //sF32 getSpeed              (void);



/* @method getTime:float

Return the current time offset.

@return The current time offset
*/
      YM //sF32 getTime               (void);



/* @method setTime,float t

Seek to the current time offset.

@arg t New (absolute) time offset
*/
      YM //void setTime               (sF32 _t);



/* @method getDeltaTime:float

Return delta time to the next event

@return Delta time to the next event
*/
      YM //sF32 getDeltaTime          (void);



/* @method isNewEvent:boolean

Check whether the last §tickPrecise call hit a new key event.

@return true If a new event has been reached, false otherwise
*/
      YM //sSI  isNewEvent            (void);



/* @method valueAtTimeRaster,float t,float res:var

Return the value at the given absolute time offset in the given time window.

@arg t Absolute time
@arg res Window size
@return Event value or <code>void</code> if there is no event within the given window
*/
      YM //void valueAtTimeRaster     (sF32 _t, sF32 _res, YAC_Value *_r);



/* @method valueAtTimeSH,float t:var

Return the value of the last event before the given point in time.

@arg t Absolute time
@return Event value or <code>void</code> if there is no event before the given point in time
*/
      YM //void valueAtTimeSH         (sF32 _t, YAC_Value *_r);


/* @method valueAtTime,float t:float

Seek and return (interpolated) value.

@arg t Absolute time
@return Float value
*/
      YM //sF32 valueAtTime           (sF32 _t);


/* @method timeToIndex,float t:int

Return the last event number (<b>not</b> array offset!) before the given time offset.

@arg Absolute time
@return Event number (array index / 2)
*/
      YM //sSI  timeToIndex           (sF32 _t);



/* @method indexToTime,int event:float

Return the absolute time offset of the given event number.

@arg event Event number (array index / 2)
@return Absolute time
*/
      YM //sF32 indexToTime           (sSI _idx);



/* @method insertReplaceEvent,float t,float v,float w

Insert or replace an event at the given time offset.
Keep deltatime if event already exists within [t..t+w].

@arg t Absolute time
@arg v New event value
@arg w Time window size
*/
      YM //void insertReplaceEvent    (sF32 _t, sF32 _value, sF32 _w);



/* @method insertReplaceEventMSB4,float t,byte v,float w

Insert or replace part of an (8bit) event at the given time offset (most significant nibble).
Keep deltatime if event already exists within [t..t+w].

@arg t Absolute time
@arg v New event value (most significant nibble)
@arg w Time window size
*/
      YM //void insertReplaceEventMSB4(sF32 _t, sSI _value, sF32 _w);



/* @method insertReplaceEventLSB4,float t,byte v,float w

Insert or replace part of an (8bit) event at the given time offset (least significant nibble).
Keep deltatime if event already exists within [t..t+w].

@arg t Absolute time
@arg v New event value (most significant nibble)
@arg w Time window size
*/
      YM //void insertReplaceEventLSB4(sF32 _t, sSI _value, sF32 _w);


/* @method insertReplaceEventPat1,float t,byte v,float w

Insert or replace part of an (7bit) event, formatted as '01A'..'32D' at the given time offset.
Keep deltatime if event already exists within [t..t+w].

@arg t Absolute time
@arg v New event value (first digit)
@arg w Time window size
*/
      YM //void insertReplaceEventPat1(sF32 _t, sSI _value, sF32 _w);


/* @method insertReplaceEventPat2,float t,byte v,float w

Insert or replace part of an (7bit) event, formatted as '01A'..'32D' at the given time offset.
Keep deltatime if event already exists within [t..t+w].

@arg t Absolute time
@arg v New event value (second digit)
@arg w Time window size
*/
      YM //void insertReplaceEventPat2(sF32 _t, sSI _value, sF32 _w);


/* @method insertReplaceEventPat3,float t,byte v,float w

Insert or replace part of an (7bit) event, formatted as '01A'..'32D' at the given time offset.
Keep deltatime if event already exists within [t..t+w].

@arg t Absolute time
@arg v New event value (third digit)
@arg w Time window size
*/
      YM //void insertReplaceEventPat3(sF32 _t, sSI _value, sF32 _w);


/* @method removeEvent,float t

Remove event at the given time offset and shift all succeeding events.

@arg t Absolute time
*/
      YM //void removeEvent           (sF32 _t);



/* @method removeRange,float t,float len

Remove all events within the given time window and shift the succeeding events.

@arg t Absolute time (window start)
@arg len Window size
*/
      YM //void removeRange           (sF32 _t, sF32 _len);



/* @method removeRangeUntilNext,float t,float len,float maxT
Remove range until next event and keep abstime of following event (if any)

@arg t Absolute time (window start)
@arg len Window size
@arg maxT Do not delete events after this point
*/
      YM //void removeRangeUntilNext  (sF32 _t, sF32 _l, sF32 _maxT);



/* @method removeRangeSH,float t,float len
Remove range of sample&hold style envelope.

In contrary to §removeRange, events will only be delete if they are
entirely with the given range. If only covered partly, their delta
times will be adjusted.

@arg t Absolute time (window start)
@arg len Window size
*/
      YM //void removeRangeSH         (sF32 _t, sF32 _l);



/* @method shiftEvents,float t,float l
Shift all events after the given time offset by the given number of ticks.

@arg t Absolute time
@arg l Time shift
*/
      YM //void shiftEvents           (sF32 _t, sF32 _l);



/* @method shiftNextEvent,float t,float l,float maxT
Shift only next event and keep abstime of following event (if any / in range t..maxT)

@arg t Absolute time
@arg l Time shift
@arg maxT Do not shift event after this point
*/
      YM //void shiftNextEvent        (sF32 _t, sF32 _l, sF32 _maxT);



/* @method getNextEventTimeAfter,float t:float
Return absolute time of event after _t, or -1 if there is no event.

@arg t Absolute time
@return Absolute time of event after 't' or -1 if there is no event
*/
      YM //sF32 getNextEventTimeAfter (sF32 _t);


/* @method rotateEvents,float r,float startT,float maxT
Rotate events within (startT..maxT) by delta time 'r'.

@arg r Rotation delta time
@arg startT Rotation window start (must be >= 0)
@arg maxT Rotation window end (must be > startT)
*/
      YM //void rotateEvents (sF32 _r, sF32 _startT, sF32 _maxT);


/* @method getPreviousEventTimeBefore,float t:float
Return absolute time of event before _t, or -1 if there is no event.

@arg t Absolute time
@return Absolute time of event before 't' or -1 if there is no event
*/
      YM //sF32 getPreviousEventTimeBefore (sF32 _t);


/* @method getCurrentIndex:int
Return current envelope index (debug).

@return Current envelope index
*/
      YM //sUI  getCurrentIndex (void);

};
#include "ying_core_Envelope.cpp"
YAC_C_CORE_POOLED(_Envelope, "Envelope", TKS_CLID_ENVELOPE);
//------------------------------------------------------------------------------------- File
/* @class File,Stream

Represents a file in the local file system.

@see PakFile
*/
YC class _File : public YAC_File {
public:
   YAC_POOLED(_File, YAC_POOL_PRIORITY_LOW);

/* @constant IOS_IN,int

File access mode "read-only".

@see IOS_OUT
@see IOS_INOUT
*/

/* @constant IOS_OUT,int

File access mode "write/create".

@see IOS_IN
@see IOS_INOUT
*/

/* @constant IOS_INOUT,int

File access mode "read-write".

@see IOS_IN
@see IOS_OUT
*/

/* @constant SEEK_SET,int

Seek to absolute file offset.

@see SEEK_CUR
@see SEEK_END
*/

/* @constant SEEK_CUR,int

Seek relative to current file offset.

@see SEEK_SET
@see SEEK_END
*/

/* @constant SEEK_END,int

Seek relative to end of file.

@see SEEK_SET
@see SEEK_CUR
*/




/* @method open,String name,int accessFlags:boolean

Open a file in the local file system.

@arg name Which file to open (pathname)
@arg accessFlags §IOS_OUT for writing, §IOS_IN for reading and §IOS_INOUT for read-write access
@return true if the file has been opened successfully, false otherwise

@see openLocal
@see isOpen
@see close
*/
   YM //sSI  _open     (YAC_String *_name, sSI _access);

/* @method openLocal,String name,int accessFlags:boolean

Open a file in the local file system.

@deprecated
@arg name Which file to open (pathname)
@arg accessFlags §IOS_OUT for writing, §IOS_IN for reading and §IOS_INOUT for read-write access
@return true if the file has been opened successfully, false otherwise

@see open
@see isOpen
@see close
  */
      YM //sSI  openLocal (YAC_String *_name, sSI _access);

/* @method isOpen:boolean

Check whether the File object has a valid file handle.

@return true if there is a valid file handle, false otherwise.
@see open
@see openLocal
@see close
*/
      YM //sSI  isOpen    (void);

/* @method setOffset,int offset

Seek to the given absolute file offset (bytes).

@arg offset New absolute file offset (bytes)
@see seek
*/
      YM //void setOffset (sUI);

/* @method seek,int offset,int mode

Seek to the given file offset.

@arg offset Byte offset
@arg mode One of §SEEK_SET, §SEEK_CUR, §SEEK_END
*/
      YM //void _seek     (sSI, sSI);

/* @method flush

Write pending buffers to disk..
*/
      YM //void flush     (void);

/* @method close

Close the current file.

@see isOpen
@see open
@see openLocal
*/

      // deprecated:
      YM //void _openTemp (YAC_String *_dir, YAC_String *_pfx, YAC_Value *_r);
      YM //void removeTemp(void);
};
#include "ying_core_File.cpp"
YAC_C_CORE_POOLED(_File, "File", YAC_CLID_FILE);
//------------------------------------------------------------------------------------- StdOutStream
/* @class StdOutStream,Stream

Represents the stdout output stream.

@see StdInStream
@see StdErrStream

*/
YC class _StdOutStream : public YAC_StdOutStream {
public:
   YAC_POOLED(_StdOutStream, YAC_POOL_PRIORITY_LOW);

/* @method isOpen:boolean

Check whether the stream is open (always true).

@return 1(true)
*/
   YM //sSI  isOpen    (void);

/* @method flush

Write pending/cached output buffers into the stream.
*/
      YM //void flush     (void);
};
#include "ying_core_StdOutStream.cpp"
YAC_C_CORE_POOLED(_StdOutStream, "StdOutStream", YAC_CLID_STDOUTSTREAM);

//------------------------------------------------------------------------------------- StdErrStream
/* @class StdErrStream,Stream

Represents the stderr output stream.

@see StdInStream
@see StdOutStream

*/
YC class _StdErrStream : public YAC_StdErrStream {
public:
   YAC_POOLED(_StdErrStream, YAC_POOL_PRIORITY_LOW);

/* @method isOpen:boolean

Check whether the stream is open (always true).

@return 1(true)
*/
   YM //sSI  isOpen    (void);

/* @method flush

Write pending/cached output buffers into the stream.
*/
      YM //void flush     (void);
};
#include "ying_core_StdErrStream.cpp"
YAC_C_CORE_POOLED(_StdErrStream, "StdErrStream", YAC_CLID_STDERRSTREAM);

//------------------------------------------------------------------------------------- StdInStream
/* @class StdInStream,Stream

Represents the stdin input stream.

@see StdOutStream
@see StdErrStream

*/
YC class _StdInStream : public YAC_StdInStream {
public:
   YAC_POOLED(_StdInStream, YAC_POOL_PRIORITY_LOW);

/* @method isOpen:boolean

Check whether the stream is open (always true).

@return 1(true)
*/
   YM //sSI  isOpen    (void);

      YM //void flush     (void);
};
#include "ying_core_StdInStream.cpp"
YAC_C_CORE_POOLED(_StdInStream, "StdInStream", YAC_CLID_STDINSTREAM);

//------------------------------------------------------------------------------------- PakFile
/* @class PakFile,Stream

Represents a file in a <code>.tkx</code> resp. <code>.tsl</code> archive.

@see File
*/
YC class _PakFile : public YAC_PakFile {
public:
   YAC_POOLED(_PakFile, YAC_POOL_PRIORITY_LOW);

/* @method isOpen:boolean

Check whether the last §open call has succeeded.

@return True if there is valid file handle, false otherwise
*/
   YM //sSI  isOpen    (void);

/* @method open,String name:boolean

Open a file in the current application pak file (<code>.tkx</code> resp. <code>.tsl</code>).

@arg name Which file to open
@return true if the given file has been opened successfully, false otherwise
*/
      YM //sSI  _open     (YAC_String *_name);

/* @method close

Close the current file.

@see isOpen
@see open
*/

      // deprecated:
      YM //sSI  openLocal (YAC_String *_name, sSI _access);
};
#include "ying_core_PakFile.cpp"
YAC_C_CORE_POOLED(_PakFile, "PakFile", YAC_CLID_PAKFILE);

//------------------------------------------------------------------------------------- TKS / Language
/* @class TKS,Object

Static class that provides various TkScript related utility functions (e.g. reflection helpers).

*/
YC class _Language : public TKS_Language {
public:
   YAC(_Language);

/* @constant SIGINT,int
 */
//#define SIGINT                           YCI 2


/* @constant SIGILL,int
 */
//#define SIGILL                           YCI 4


/* @constant SIGFPE,int
 */
//#define SIGFPE                           YCI 8


/* @constant SIGSEGV,int
 */
//#define SIGSEGV                          YCI 11


/* @constant SIGTERM,int
 */
//#define SIGTERM                          YCI 15


/* @constant SIGABRT,int
 */
//#define SIGABRT                          YCI 22


/* @constant YAC_LITTLE_ENDIAN,int
Stream byteorder (LSB first)
*/
   //#define YAC_LITTLE_ENDIAN YCI 0

/* @constant YAC_BIG_ENDIAN,int
Stream byteorder (MSB first)
*/
   //#define YAC_BIG_ENDIAN YCI 1


/* @constant YAC_TYPE_VOID,int
*/
   //#define YAC_TYPE_VOID YCI 0

/* @constant YAC_TYPE_INT,int
*/
   //#define YAC_TYPE_INT YCI 1

/* @constant YAC_TYPE_FLOAT,int
*/
   //#define YAC_TYPE_FLOAT YCI 2

/* @constant YAC_TYPE_OBJECT,int
*/
   //#define YAC_TYPE_OBJECT YCI 3

/* @constant YAC_TYPE_STRING,int
*/
   //#define YAC_TYPE_STRING YCI 4

/* @constant YAC_OP_ASSIGN,int

§Object.yacOperator command id (assign)
*/
   //#define YAC_OP_ASSIGN YCI 0

/* @constant YAC_OP_ADD,int

§Object.yacOperator command id (add)
*/
   //#define YAC_OP_ADD YCI 1

/* @constant YAC_OP_SUB,int

§Object.yacOperator command id (subtract)
*/
   //#define YAC_OP_SUB YCI 2

/* @constant YAC_OP_MUL,int

§Object.yacOperator command id (multiply)
*/
   //#define YAC_OP_MUL YCI 3

/* @constant YAC_OP_DIV,int

§Object.yacOperator command id (divide)
*/
   //#define YAC_OP_DIV YCI 4

/* @constant YAC_OP_MOD,int

§Object.yacOperator command id (modulo)
*/
   //#define YAC_OP_MOD YCI 5

/* @constant YAC_OP_SHL,int

§Object.yacOperator command id (shift left)
*/
   //#define YAC_OP_SHL YCI 6

/* @constant YAC_OP_SHR,int

§Object.yacOperator command id (shift right)
*/
   //#define YAC_OP_SHR YCI 7

/* @constant YAC_OP_CEQ,int

§Object.yacOperator command id (compare equal)
*/
   //#define YAC_OP_CEQ YCI 8

/* @constant YAC_OP_CNE,int

§Object.yacOperator command id (compare not equal)
*/
   //#define YAC_OP_CNE YCI 9

/* @constant YAC_OP_CLE,int

§Object.yacOperator command id (compare less equal)
*/
   //#define YAC_OP_CLE YCI 10

/* @constant YAC_OP_CLT,int

§Object.yacOperator command id (compare less than)
*/
   //#define YAC_OP_CLT YCI 11

/* @constant YAC_OP_CGE,int

§Object.yacOperator command id (compare greater equal)
*/
   //#define YAC_OP_CGE YCI 12

/* @constant YAC_OP_CGT,int

§Object.yacOperator command id (compare greater than)
*/
   //#define YAC_OP_CGT YCI 13

/* @constant YAC_OP_AND,int

§Object.yacOperator command id (bitwise and)
*/
   //#define YAC_OP_AND YCI 14

/* @constant YAC_OP_OR,int

§Object.yacOperator command id (bitwise or)
*/
   //#define YAC_OP_OR YCI 15

/* @constant YAC_OP_EOR,int

§Object.yacOperator command id (bitwise exclusive or)
*/
   //#define YAC_OP_EOR YCI 16

/* @constant YAC_OP_NOT,int

§Object.yacOperator command id (logical not)
*/
   //#define YAC_OP_NOT YCI 17

/* @constant YAC_OP_BITNOT,int

§Object.yacOperator command id (bitwise not)
*/
   //#define YAC_OP_BITNOT YCI 18

/* @constant YAC_OP_LAND,int

§Object.yacOperator command id (logical and)
*/
   //#define YAC_OP_LAND YCI 19

/* @constant YAC_OP_LOR,int

§Object.yacOperator command id (logical or)
*/
   //#define YAC_OP_LOR YCI 20

/* @constant YAC_OP_LEOR,int

§Object.yacOperator command id (logical exclusive or)
*/
   //#define YAC_OP_LEOR YCI 21

/* @constant YAC_OP_NEG,int

§Object.yacOperator command id (negate)
*/
   //#define YAC_OP_NEG YCI 22

/* @constant YAC_OP_INIT,int

§Object.yacOperator command id (init)
*/
   //#define YAC_OP_INIT YCI 23

/* @method constantToString,int c,String prefix:String

Try to map a constant integer value to a literal.

@arg c Integer constant
@arg prefix Prefix of constant name
*/
   YM //YAC_String *constantToString            (sSI _const       , YAC_String *_strmatch                );

/* @method scriptClassConstantToString,Value vo,Class clazz,String prefix:String

Try to map a constant to script class constant name.

@arg vo §ValueObject that the constants are compared with
@arg clazz §Class in which to search
@arg prefix Prefix of constant name
*/
   YM //YAC_String *scriptClassConstantToString (YAC_Object *_vo, YAC_Object *_sci, YAC_String *_strmatch);


/* @method stringToConstant,String name:var

Try to map a named constant to its value.

@arg name Name of the constant
@return Constant value or <code>void</code> if the given name does not map to any constant
*/
   YM //void        stringToConstant            (YAC_Object *     , YAC_Value *_r                        );

/* @method findScriptClass,String nspaceName,String className:Class

Find the template object for the given script class.

@arg nspaceName Name of the namespace or NULL (default namespace)
@arg className Name of the script class
@return template object for the given script class or null if the given name does not map to a script class.
*/
   YM //YAC_Object *findScriptClass             (YAC_Object *_nspaceName, YAC_Object *_className);


/* @method findClassesByPrefix,String nspaceName,String prefix:PointerArray
Find all script and/or C++ classes that match the given class name prefix.

If nspaceName is "yac", only C++ will be searched.

This method supports the => operator.

@arg nspaceName "yac" to only search C++ classes, null to search all classes (script classes first), or namespace name to match.
@arg prefix null to match all class names, class name prefix otherwise.
@return PointerArray that holds template object references to all matching classes.
*/
   YM //void        findClassesByPrefix_YAC_RARG (YAC_Object *_nspaceName, YAC_Object *_prefix, YAC_Object *_r);
   YM //void        findClassesByPrefix_YAC_RVAL (YAC_Object *_nspaceName, YAC_Object *_prefix, YAC_Value *_r);


/* @method findClassesByBaseClass,String nspaceName,Class baseClass:PointerArray
Find all script classes that are derived from the given base class.

@arg nspaceName null to search all classes, or namespace name to match.
@arg baseClass base class type
@return PointerArray that holds template object references to all matching classes.
*/
   YM //void        findClassesByBaseClass      (YAC_Object *_nspaceName, YAC_Object *_baseClass, YAC_Value *_r);


/* @method getClassName,Object o:String

Return the name of the given object

@arg o The object in question
@return new String instance holding the class name or null if "o" is not a valid object.
@see Object.yacClassName
@see Object.yacMetaClassName
*/
   YM //void        getClassName                (YAC_Object *     , YAC_Value *_r                        );

/* @method getNamespaceName,Object o:String

Return the namespace name of the given object

@arg o The object in question
@return new String instance holding the namespace name or null if "o" is not a valid object.
@see getClassName
*/
   YM //void        getNamespaceName            (YAC_Object *     , YAC_Value *_r                        );

/* @method getVersion:int

Return TkScript version identifier.

The format is 0xmmiirrpp, where mm=major version, ii=minor version major, rr=release number, pp=patch.

@return TkScript version 32bit integer value
@see getVersionString
*/
   YM //sSI         getVersion                  (void                                                    );

/* @method getVersionString:String

Return TkScript version string.

Each byte is converted to decimal, sub-version numbers are separated by the '.' char.
@return Version string
@see getVersion
*/
   YM //YAC_String *getVersionString            (void                                                    );

/* @method setIntPropertyByName,Object o,String name,int i

Set an integer member of a meta class object.

This is a tool method to handle meta class objects like e.g. script classes.

@arg o The object in question
@arg name Name of the property to modify
@arg i New value for the integer property
@see Object.yacMetaClassMemberGetAccessKeyByName
@see Object.yacMetaClassMemberSet
*/
   YM //void        setIntPropertyByName        (YAC_Object *_o   , YAC_String *_name, sSI _value        );

/* @method setFloatPropertyByName,Object o,String name,float f

Set a float member of a meta class object.

This is a tool method to handle meta class objects like e.g. script classes.

@arg o The object in question
@arg name Name of the property to modify
@arg f New value for the float property
@see Object.yacMetaClassMemberGetAccessKeyByName
@see Object.yacMetaClassMemberSet
*/
   YM //void        setFloatPropertyByName      (YAC_Object *_o   , YAC_String *_name, sF32 _value       );

/* @method setObjectPropertyByName,Object o,String name,Object o

Set an object member of a meta class object.

This is a tool method to handle meta class objects like e.g. script classes.

@arg o The object in question
@arg name Name of the property to modify
@arg o New value for the object property.
@see Object.yacMetaClassMemberGetAccessKeyByName
@see Object.yacMetaClassMemberSet
*/
   YM //void        setObjectPropertyByName     (YAC_Object *_o   , YAC_String *_name, YAC_Object *_value);

/* @method getPropertyAccessKeyByName,Object o,String name:int

Query the access key for a member of a meta class object.

This is a tool method to handle meta class objects like e.g. script classes.

@arg o The object in question
@arg name Member name
@see getPropertyAccessKeyByIndex
@see Object.yacMetaClassMemberGetAccessKeyByName
*/
   YM //sSI         getPropertyAccessKeyByName  (YAC_Object *_o   , YAC_Object *_name                    );

/* @method getPropertyAccessKeyByIndex,Object o,int index:int

Query the access key for the <i>n</i>th member of a meta class object.

This is a tool method to handle meta class objects like e.g. script classes.

@arg o The object in question
@arg name Member name
@see getPropertyAccessKeyByName
@see Object.yacMetaClassMemberGetAccessKeyByIndex
*/
   YM //sSI         getPropertyAccessKeyByIndex (YAC_Object *_o   , sSI _idx                             );

/* @method getPropertyByAccessKey,int ak:var

Read meta class member.

@arg o The object in question
@arg ak Access key to the meta class member
@return Member value
@see Object.yacMetaClassMemberGet
*/
   YM //void        getPropertyByAccessKey      (YAC_Object *_o   , sSI _ak          , YAC_Value *_r     );

/* @method getPropertyNameByAccessKey,int ak:String

Return meta class member name.

@arg o The object in question
@arg ak Access key to the meta class member
@return Member name
@see Object.yacMetaClassMemberGet
*/
   YM //void        getPropertyNameByAccessKey  (YAC_Object *_o   , sSI _ak          , YAC_Value *_r     );

/* @method getPropertyByName,String name:var

Return meta class member value by name.

@arg o The object in question
@arg name Member name
@return Member value
@see Object.yacMetaClassMemberGet
@see Object.yacMetaClassMemberGetAccessKeyByName
*/
   YM //void        getPropertyByName           (YAC_Object *_o   , YAC_String *_name, YAC_Value *_r     );

/* @method getNumProperties,Object o:int

Return the number of meta class members.

@arg o The object in question
@return Number of meta class members

@see Object.yacMetaClassMemberGetNum
*/
   YM //sSI         getNumProperties            (YAC_Object *_o                                          );

/* @method getPropertyClassName,Object o:String

Return the name of a meta class object.

@arg o The object in question
@return Meta class name

@see Object.yacMetaClassName
*/
   YM //void        getPropertyClassName        (YAC_Object *_o, YAC_Value *_r                           );

/* @method newObjectByName,String nspace,String name:Object

Create a new instance of a script or C++ class.

@arg nspace Namespace name or null
@arg name Class name
@return New object instance or <code>null</code> if the given name does not map to a script or C++ class.
*/
   YM //void        newObjectByName             (YAC_Object *_nspaceName, YAC_Object *_name, YAC_Value *_r);

/* @method newObjectArrayByName,String nspace,String name:Object

Resolve object name and create an empty array of equally typed objects.

If the object class has a special array object (see §Object.yacArrayNew), an instance of the
given array object class will be returned.
A generic §ObjectArray will be created, otherwise.

@arg nspace Namespace name or null
@arg name Name of the element object class
@return Array object
@see Object.yacArrayNew
*/
   YM //void        newObjectArrayByName        (YAC_Object *_nspaceName, YAC_Object *_name, YAC_Value *_r);

/* @method newObjectByID,int classID:Object

Return a new instance of the given C++ class.

This method has to be used with care since classIDs are not constants and may vary between different releases of TkScript.
Plugin classes always have dynamic class IDs.

@arg classID Name-id of a C++ class
@return C++ class instance
*/
   YM //void        newObjectByID               (sSI              , YAC_Value *_r                        );

/* @method getClassID,Object o:int

Return the classID of the given C++ object.

@arg o The object in question
@return C++ classID
*/
   YM //sSI         getClassID                  (YAC_Object *_o                                          );

/* @method evalMethodByName,Object o,String name,Object arglist,Value returnValue:boolean

Evaluate the given C++ or meta class method.

@arg o Object to operate on
@arg name Method name
@arg arglist Argument list (array, list, ..)
@arg returnValue Receives the method return value
@return true if the method has been called successfully, false otherwise
*/
   YM //sSI         evalMethodByName            (YAC_Object *_o, YAC_String *_name, YAC_Object *_arglist, YAC_Object *_retvalue);

/* @method classToHashTable,Object c,HashTable ht:boolean

Write the members of the given meta class instance to the given hash table.

@arg c Meta class instance (script class)
@arg ht Target hashtable
@return true if all object members have been written to the hashtable or false otherwise
*/
   YM //sSI         classToHashTable            (YAC_Object *_class, YAC_Object *_hashtable);

/* @method hashTableToClass,HashTable ht,Object c:boolean

Iterate hash table and copy the elements to members of "c".

All deletable objects will be unlinked from the hashtable.

@arg ht Source hashtable
@arg c Destination metaclass instance (scriptclass object)
@return true if all hash table entries were copied to the object or false, otherwise.
*/
   YM //sSI         hashTableToClass            (YAC_Object *_hashtable, YAC_Object *_class);

/* @method getMethodArgumentNames,String namespaceName,String scriptClassName,String methodName:StringArray

Query the parameter names of the given script class method.

@arg namespaceName Name of the namespace, null to search default namespace
@arg scriptClassName Name of the script class
@arg methodName Method name
@return new StringArray instance holding the method parameter names or null if the class or method was not found.
*/
   YM //void        getMethodArgumentNames      (YAC_Object *_nspaceName, YAC_Object *_classname, YAC_Object *_methodname, YAC_Value *_r);

/* @method sleep,int milliSec

Suspend the current thread for the given number of milli seconds.

@arg milliSec Number of milli seconds
*/

   YM //void        sleep                       (sUI _ms);

/* @method yield

Yield CPU time to the next thread.
*/
   YM //void        _yield                      (void);

/* @method getMethodNames,Object o:StringArray

Query the method names of the given C++ or script class.

@arg o C++ or script (meta) class instance
@return new StringArray instance holding the method names for the given class or null if "o" is not a valid object.
*/
   YM //void        getMethodNames              (YAC_Object *_class, YAC_Value *_r);

/* @method isBigEndian

Check whether TKS is running on a big endian host.

@return 1(true) if host architecture uses big endian, 0(false) if it uses little endian.
*/
   YM //sSI         _isBigEndian                (void);


/* @method is64Bit:boolean
Query whether it's a 64bit runtime environment.

@return True if it's 64bit, false if 32bit (less is not supported).
*/
      YM //sBool is64Bit (void);


/* @method reinstallDebugHandler

Hack to re-install exception filter after a DLL has changed it.
*/
   YM //void        _reinstallDebugHandler      (void);


/* @method getTksExePath:String
Query absolute pathname of tks executable.

@return Absolute pathname of tks executable
*/
      YM //void getTksExePath (YAC_Value *_r);


/* @method getTksExeDir:String
Query absolute dirname of tks executable.

@return Absolute dirname of tks executable
*/
      YM //void getTksExeDir (YAC_Value *_r);


/* @method getTksPluginPath:String
Query absolute pathname of tks plugins directory.

@return Absolute pathname of tks plugins directory
*/
      YM //void getTksPluginPath (YAC_Value *_r);


/* @method getTksModulePath:String
Query absolute pathname of tks modules directory.

@return Absolute pathname of tks modules directory
*/
      YM //void getTksModulePath (YAC_Value *_r);


/* @method getTksLibraryPath:String
Query absolute pathname of tks library directory.

@return Absolute pathname of tks library directory
*/
      YM //void getTksLibraryPath (YAC_Value *_r);


/* @method getTksApplicationPath:String
Query absolute pathname of tks application directory.

@return Absolute pathname of tks application directory
*/
      YM //void getTksApplicationPath (YAC_Value *_r);


/* @method getDllPrefix:String
Query prefix for native code DLLs / shared objects.

@return "" on Windows and macOS, "lib" on Linux, ..
*/
      YM //void getDllPrefix(YAC_Value *_r)


/* @method getDllSuffix:String
Query suffix for native code DLLs / shared objects.

@return ".dll" on Windows, ".so" on Linux, "dylib" on macOS, ..
*/
      YM //void getDllSuffix(YAC_Value *_r)


/* @method newThreadFromCurrent:Thread
Create new thread object and initialize with current thread id.

Sets tks_current_thread TLS (if compiled in).

The returned object must not be deleted until the thread has terminated.

@return new Thread object
*/
      YM //void newThreadFromCurrent(YAC_Value *_r);


/* @method forceUnlockNamedMutex,name
Unlock mutex. Should only be called in case something horribly wrent wrong (e.g. a thread holding the mutex has crashed)

This method might be removed, pls don't call.

@arg name Mutex name
*/
      YM //void forceUnlockNamedMutex(YAC_String *_name);


/* @method unlockNamedMutex,name
Unlock mutex that was declared using the =mutexname= syntax.

@arg name Mutex name
*/
      YM //void unlockNamedMutex(YAC_String *_name);


/* @method lockNamedMutex,name
Lock mutex that was declared using the =mutexname= syntax.

@arg name Mutex name
*/
      YM //void lockNamedMutex(YAC_String *_name);


/* @method getHomeDir:String
Query absolute pathname of user home directory.

@return Absolute pathname of user home directory
*/
      YM //void getHomeDir (YAC_Value *_r);


/* @method getCurrentDir:String
Query current working directory

@return Absolute pathname of current working directory
*/
      YM //void getCurrentDir (YAC_Value *_r);


/* @method getExtraFiles:StringArray
Query array of extra files added to script engine startup (e.g. local config files).

@return §StringArray of extra files
*/
      YM //void getExtraFiles (YAC_Value *_r);


/* @method loadLocalTSL,libName:boolean
Dynamically load TSL (library) file

@arg libName  e.g. "mylib.tsl"
@return true if lib has been loaded, compiled, and initialized
*/
      YM //sBool loadLocalTSL (YAC_String *_libName);


/* @method evalScript,String src:String
Compile and evaluate script and return output string buffer (trace/print/stdout/stderr/..).

This allocates a new §Script object, installs a print hook, loads the given source, eval()s the global statements, and returns the print hook output buffer.

@arg src Script source (e.g. 'trace "hello, world.";')
@return a deletable copy of the output string buffer (never null)
*/
      YM //void  evalScript (YAC_String *_src, YAC_Value *_r);

};
#include "ying_core_Language.cpp"
YAC_C_CORE(_Language, "TKS", TKS_CLID_LANGUAGE);

//------------------------------------------------------------------------------------- Stream
/* @class Stream,Object

Base class for input/output stream objects.

*/
YC class _Stream : public YAC_StreamBase {
public:
   YAC(_Stream);

/* @method deserialize,Object o,boolean bTypeInfo:boolean

@arg o
@arg bTypeInfo
@return
*/
   YM //sSI  _deserialize                  (YAC_Object *_dest, sSI _typeinfo);

/* @method eof:boolean

@return
*/
      YM //sSI   eof                          (void);

/* @method getByteOrder:int

@return
*/
      YM //sSI  getByteOrder                  (void);

/* @method getErrorCode:int

@return
*/
      YM //sSI  getErrorCode                  (void);

/* @method getErrorStringByCode,int errorCode:String

@arg errorCode
@return
*/
      YM //void getErrorStringByCode          (sSI _code, YAC_Value *_r);

/* @method getI8:byte

Read unsigned byte from stream.

@return
*/
      YM //sSI  getI8                         (void);

/* @method getU8:byte

Read unsigned byte from stream.

@return
*/
      YM //sSI  getU8                         (void);

/* @method getS8:byte

Read signed byte from stream (extend to int).

@return
*/
      YM //sSI  getS8                         (void);

/* @method getB8:boolean

Read 8bit boolean from stream (either 1 or 0).

@return
*/
      YM //sSI  getB8                         (void);

/* @method getI16:short

Read unsigned short from stream.

@return
*/
      YM //sSI  getI16                        (void);

/* @method getU16:short

Read unsigned short from stream.

@return
*/
      YM //sSI  getU16                        (void);

/* @method getS16:short

Read signed short from stream (extend to int).

@return
*/
      YM //sSI  getS16                        (void);

/* @method getI32:int

@return
*/
      YM //sSI  getI32                        (void);

/* @method getF32:float

@return
*/
      YM //sF32 getF32                        (void);

/* @method getF64:float

@todo Make it return a Double object
@return
*/
      YM //sF32 getF64                        (void);

/* @method getOffset:int

@return
*/
      YM //sUI  getOffset                     (void);

/* @method getSize:int

@return
*/
      YM //sUI  getSize                       (void);

/* @method readBuffer,Buffer buf,int off,int num,boolean bResize:int

@arg buf
@arg off
@arg num
@arg bResize
@return
*/
      YM //sSI  readBuffer                    (YAC_Object *_buf, sSI _off, sSI _num, sSI _resize);

/* @method readLine,String s,int max:boolean

@arg s
@arg num
@return
*/
      YM //sSI  readLine                      (YAC_String *_s, sSI _num);

/* @method readString,String s,int num:boolean

@arg s
@arg num
@return
*/
      YM //sSI  readString                    (YAC_String *_s, sSI _num);

/* @method seek,int off,int mode

@arg off
@arg mode
*/
      YM //void _seek                         (sSI, sSI);

/* @method serialize,Object src,boolean bTypeInfo

@arg src
@arg bTypeInfo
*/
      YM //void _serialize                    (YAC_Object *_src, sSI _typeinfo);

/* @method setByteOrder,int byteOrder

@arg byteOrder
*/
      YM //void setByteOrder                  (sSI);

/* @method setOffset,int offset

@arg offset
*/
      YM //void setOffset                     (sUI);

/* @method writeBuffer,Buffer buf,int off,int num:boolean

@arg buf
@arg off
@arg num
@return
*/
      YM //sSI  writeBuffer                   (YAC_Object *_buf, sSI _off, sSI _num);

/* @method writeString,String s,int off,int num:boolean

@arg s
@arg off
@arg num
@return
*/
      YM //sSI  writeString                   (YAC_String *_s  , sSI _off, sSI _num);

/* @method setObject,Object src

@arg src
*/
      YM //void setObject                     (YAC_Object* _src); //setObject

/* @method getObject,Object dest

@arg dest
*/
      YM //void getObject                     (YAC_Object* _dest); //getObject

/* @method setI8,byte v

@arg v
*/
      YM //void setI8                         (sSI);

/* @method setI16,short v

@arg v
*/
      YM //void setI16                        (sSI);

/* @method setI32,int v

@arg v
*/
      YM //void setI32                        (sSI);

/* @method setF32,float v

@arg v
*/
      YM //void setF32                        (sF32);

/* @method setF64,float v

@todo make this method take a Double object argument
@arg v
*/
      YM //void setF64                        (sF32);

/* @method close

*/
      YM //void _close                        (void);

/* @method setI8a,Object a
Write bytes from generic array object
*/
      YM //void setI8a                        (YAC_Object *_a);

/* @method setI16a,Object a
Write shorts from generic array object
*/
      YM //void setI16a                       (YAC_Object *_a);

/* @method setI32a,Object a
Write double words from generic array object
*/
      YM //void setI32a                       (YAC_Object *_a);

/* @method setF32a,Object a
Write floats from generic array object
*/
      YM //void setF32a                       (YAC_Object *_a);

/* @method setF64a,Object a
Write doubles from generic array object
*/
      YM //void setF64a                       (YAC_Object *_a);

/* @method getI8a,Object a,int num
Read signed bytes and store in array object (will be reallocated).
*/
      YM //void getI8a                        (YAC_Object *_a, sUI _num);

/* @method getU8a,Object a,int num
Read unsigned bytes and store in array object (will be reallocated).
*/
      YM //void getU8a                        (YAC_Object *_a, sUI _num);

/* @method getI16a,Object a,int num
Read signed shorts and store in array object (will be reallocated).
*/
      YM //void getI16a                       (YAC_Object *_a, sUI _num);

/* @method getU16a,Object a,int num
Read unsigned shorts and store in array object (will be reallocated).
*/
      YM //void getU16a                       (YAC_Object *_a, sUI _num);

/* @method getI32a,Object a,int num
Read signed dwords and store in array object (will be reallocated).
*/
      YM //void getI32a                       (YAC_Object *_a, sUI _num);

/* @method getF32a,Object a,int num
Read 32bit floats and store in array object (will be reallocated).
*/
      YM //void getF32a                       (YAC_Object *_a, sUI _num);

/* @method getF64a,Object a,int num
Read 64bit doubles and store in array object (will be reallocated).
*/
      YM //void getF64a                       (YAC_Object *_a, sUI _num);

/* @method add2f,float f1,float f2
Write two 32bit floats to the stream.
*/
   YM //void add2f (sF32 _val1, sF32 _val2);

/* @method add3f,float f1,float f2,float f3
Write three 32bit floats to the stream.
*/
   YM //void add3f (sF32 _val1, sF32 _val2, sF32 _val3);

/* @method add4f,float f1,float f2,float f3,float f4
Write four 32bit floats to the stream.
*/
   YM //void add4f (sF32 _val1, sF32 _val2, sF32 _val3, sF32 _val4);

/* @method setRgba,int argb32
Write r,g,b,a bytes from packed ARGB32 integer
 */
   YM //void setRgba (sUI _argb32);

/* @method setBgra,int argb32
Write b,g,r,a bytes from packed ARGB32 integer
 */
   YM //void setBgra (sUI _argb32);

};
#include "ying_core_Stream.cpp"
YAC_C_CORE(_Stream, "Stream", YAC_CLID_STREAM);

//------------------------------------------------------------------------------------- String
/* @class String,Object

A buffered String class that can handle ASCII character sequences.

@see StringArray
*/

/* !!! do NOT remove comments !!!
YC class _String : public YAC_String {
public:
   YAC(_String);
*/

/* @method freeStack
*/
      YM //void        freeStack                 (void);

/* @method replace,String a,String b

Replace all occurences of String a by String b.

This method supports the => operator.
@arg a
@arg b
*/
      YM //void         _replace_YAC_RARG       (YAC_String *, YAC_String *, YAC_Object *_r);
      YM //void         _replace_YAC_RVAL       (YAC_String *, YAC_String *, YAC_Value *_r);
      YM //void         _replace_YAC_RSELF      (YAC_String *, YAC_String *);

/* @method replaceReturnNum,String a,String b:int

@arg a
@arg b
@return
*/
      YM //sSI          _replaceReturnNum        (YAC_String *, YAC_String *);

/* @method split,char delimiter:int

@deprecated Use §splitChar instead
@arg delimiter
@return
*/
      YM //sSI         split                     (sSI _delimiter);

/* @method words,boolean bQuotes:int

@deprecated Use §splitSpace instead
@arg bQuotes
@return
*/
      YM //sSI         words                     (sSI _consider_quotes);

/* @method fixLength
*/
      YM //void        fixLength                 (void);

/* @method getLength:int
Return number of chars, including ASCIIZ.

@return
*/
      YM //sUI         getLength                 (void);

/* @method getNumChars:int
Return number of chars, excluding ASCIIZ.

@return
*/
      YM //sUI         getNumChars               (void);

/* @method substring,int off,int len:String

This method supports the => operator
@arg off
@arg len
@return
*/
      YM //void        _substring_YAC_RARG      (sUI _startbyteoff, sUI _endbyteoff, YAC_Object *_r);
      YM //void        _substring_YAC_RVAL      (sUI _startbyteoff, sUI _endbyteoff, YAC_Value *_r);
      YM //void        _substring_YAC_RSELF     (sUI _startbyteoff, sUI _endbyteoff);

/* @method indexOf,String s,int off:int

@arg s
@arg off
@return
*/
      YM //sSI         _indexOf                  (YAC_Object*, sSI);

/* @method lastIndexOf,String s:int

@arg s
@return
*/
      YM //sSI         _lastIndexOf              (YAC_Object *);

/* @method loadLocal,String fileName,boolean bRemoveCR:boolean

@arg fileName
@arg bRemoveCR
@return
*/
      YM //sSI         _loadLocal                (YAC_String *_filerl, sSI _asciiflag);

/* @method saveLocal,String fileName:boolean

@arg fileName
@return
*/
      YM //sSI         _saveLocal                (YAC_String *_filerl);

/* @method trim:String

This method supports the => operator.
@return
*/
      YM //void        _trim_YAC_RARG           (YAC_Object *_s);
      YM //void        _trim_YAC_RVAL           (YAC_Value *_r);
      YM //void        _trim_YAC_RSELF          (void);

/* @method alloc,int bufSize

@arg bufSize
@return
*/
      YM //sSI         _alloc                    (sSI _numbytes);

/* @method getc,int offset:char

@arg offset
@return
*/
      YM //sSI         _getc                     (sSI offset);

/* @method putc,int offset,char c

@arg offset
@arg c
*/
      YM //void        _putc                     (sSI offset, sSI _char);

/* @method isBlank:boolean
@return
*/
      YM //sSI         isBlank                   (void); /// Check if string only contains blank characters (" ", "\n". "\t", 0)

/* @method copy,Object o

@arg o
*/
      YM //void        _copy                     (YAC_Object *);

/* @method getWord,int wordNr:String

@deprecated Use §splitSpace, §splitChar, §splitCharset, §tokenize instead
@arg wordNr
@return
*/
      YM //YAC_String *getWord                   (sSI);

/* @method startsWith,String s:boolean

@arg s
@return
*/
      YM //sSI         _startsWith               (YAC_String *);

/* @method endsWith,String s:boolean

@arg s
@return
*/
      YM //sSI         _endsWith                 (YAC_String *);

/* @method patternMatch,String pattern:int

@arg pattern
@return
*/
      YM //sSI         _patternMatch             (YAC_Object *_s); // need default return other than 0, hence no YAC_String *

/* @method numIndicesOf,String s:int

@arg s
@return
*/
      YM //sSI         _numIndicesOf             (YAC_String *_s);

/* @method insert,int index,String s:String

This method supports the => operator.

@arg index
@arg s
@return
*/
      YM //void        _insert_YAC_RARG         (sSI _index, YAC_Object *_s, YAC_Object *_r);
      YM //void        _insert_YAC_RVAL         (sSI _index, YAC_Object *_s, YAC_Value *_r);
      YM //void        _insert_YAC_RSELF        (sSI _index, YAC_Object *_s);

/* @method append,String s:String

This method supports the => operator.

@arg s
@return
*/
      YM //void        _append_YAC_RARG          (YAC_Object *_s, YAC_Object *_r);
      YM //void        _append_YAC_RVAL          (YAC_Object *_s, YAC_Value *_r);
      YM //void        _append_YAC_RSELF         (YAC_Object *_s);

/* @method appendRepeat,String s,int numRepeats

Append the given §String 'numRepeats' times.

Does nothing when 's' is empty or numRepeats is <= 0.

This method supports the => operator.

@arg s
@arg count
@return
*/
      YM //void        _appendRepeat_YAC_RARG    (YAC_String *_s, sSI _numRepeats, YAC_Object *_r);
      YM //void        _appendRepeat_YAC_RVAL    (YAC_String *_s, sSI _numRepeats, YAC_Value *_r);
      YM //void        _appendRepeat_YAC_RSELF   (YAC_String *_s, sSI _numRepeats);

/* @method load,String fileName,boolean bRemoveCR:boolean

@arg fileName
@arg bRemoveCR
@return
@see loadLocal
*/
      YM //sSI         _load                     (YAC_String *_filerl, sSI _asciiflag);

/* @method parseXML:TreeNode

@return
*/
      YM //void        _parseXML                 (YAC_Value *_r);

/* @method splitSpace,boolean bQuotes:StringArray

This method supports the => operator.

@arg bQuotes
@return
*/
      YM //void        _splitSpace_YAC_RARG     (sSI _scanprotected, YAC_Object *_r);
      YM //void        _splitSpace_YAC_RVAL     (sSI _scanprotected, YAC_Value *_r);

/* @method splitChar,char delimiter:StringArray

This method supports the => operator.

@arg delimiter
@return
*/
      YM //void        _splitChar_YAC_RARG      (sSI _char, YAC_Object *_r);
      YM //void        _splitChar_YAC_RVAL      (sSI _char, YAC_Value *_r);

/* @method splitCharset,String delimiterCharset:StringArray

This method supports the => operator.

@arg delimiterCharset
@return
*/
      YM //void        _splitCharset_YAC_RARG   (YAC_Object *_cs, YAC_Object *_r);
      YM //void        _splitCharset_YAC_RVAL   (YAC_Object *_cs, YAC_Value *_r);


/* @method wordIndexAtCharIndex,int charIndex,boolean bQuot:int

Find word index at char index.

@arg charIndex
@arg bQuot true=conside words enclosed in \"\" or \'\' as one word
@return wordIndex or -1 if not found
*/
      YM //sSI         _wordIndexAtCharIndex (sUI _charIdx, sSI _use_quot);


/* @method tokenize,String delimiterCharset:StringArray
Split string at delimiter chars and return substring array.

This method supports the => operator.

Unlike §splitCharset, the result array will contain the delimiter chars and this method will consider quoted strings.

@arg delimiterCharset
@return
*/
      YM //void        _tokenize_YAC_RARG   (YAC_Object *_cs, YAC_Object *_r);
      YM //void        _tokenize_YAC_RVAL   (YAC_Object *_cs, YAC_Value *_r);


/* @method tokenizeWithRanges,String delimiterCharset,IntArray retRanges:StringArray
Split string at delimiter chars and return substring and startoffset+numchars arrays.

This method supports the => operator.

Unlike §splitCharset, the result array will contain the delimiter chars and this method will consider quoted strings.

@arg delimiterCharset
@arg retRanges returns the substring start offsets and lengths (two elements for each substring)
@return
*/
      YM //void        _tokenizeWithRanges_YAC_RARG   (YAC_Object *_cs, YAC_Object *_retRanges, YAC_Object *_r);
      YM //void        _tokenizeWithRanges_YAC_RVAL   (YAC_Object *_cs, YAC_Object *_retRanges, YAC_Value *_r);


/* @method toLower:String

This method supports the => operator.

@return
*/
      YM //void        _toLower_YAC_RARG        (YAC_Object *_r);
      YM //void        _toLower_YAC_RVAL        (YAC_Value *_r);
      YM //void        _toLower_YAC_RSELF       (void);

/* @method toUpper:String

This method supports the => operator.

@return
*/
      YM //void        _toUpper_YAC_RARG        (YAC_Object *_r);
      YM //void        _toUpper_YAC_RVAL        (YAC_Value *_r);
      YM //void        _toUpper_YAC_RSELF       (void);

/* @method empty
*/
      YM //void         empty                    (void);

/* @method free

@see alloc
*/
      YM //void         free                     (void);

/* @method charsetIndexOf,String charset,int off:int

@arg charset
@arg off
@return
*/
      YM //sSI          charsetIndexOf           (YAC_String *_s, sSI _off);

/* @method indexOfControlCharacter,char c:int

@arg c
@return
*/
      YM //sSI          indexOfControlCharacter  (sSI);

/* @method abbrev,int length:String

This method supports the => operator.

@arg length
@return
*/
      YM //void        _abbrev_YAC_RARG         (sSI _length, YAC_Object *_r);
      YM //void        _abbrev_YAC_RVAL         (sSI _length, YAC_Value *_r);
      YM //void        _abbrev_YAC_RSELF        (sSI _length);

/* @method realloc,int bufSize:boolean

@arg bufSize
@return
*/
      YM //sBool       _realloc                 (sSI _numChars);

/* @method deleteRegion,int off,int len:String

This method supports the => operator.

@arg off
@arg len
@return
*/
      YM //void        _deleteRegion_YAC_RARG   (sSI _off, sSI _length, YAC_Object *_r);
      YM //void        _deleteRegion_YAC_RVAL   (sSI _off, sSI _length, YAC_Value *_r);
      YM //void        _deleteRegion_YAC_RSELF  (sSI _off, sSI _length);


/* @method indexOfWordStart,int off:int

@arg off
@return
*/
      YM //sSI         _indexOfWordStart         (sSI _off);


/* @method charsetIndexOfWordStart,String charset,int off:int

@arg charset
@arg off
@return
*/
      YM //sSI         _charsetIndexOfWordStart  (YAC_String *_charset, sSI _off) const;


/* @method indexOfWordEnd,int off:int

@arg off
@return
*/
      YM //sSI         _indexOfWordEnd           (sSI _off);


/* @method charsetIndexOfWordEnd,String charset,int off:int

@arg charset
@arg off
@return
*/
      YM //sSI         _charsetIndexOfWordEnd  (YAC_String *_charset, sSI _off) const;


/* @method compare,String s:boolean

@arg s
@return
*/
      YM //sSI         _compare                  (YAC_Object *_s);

/* @method compareIgnoreCase,String s:boolean

@arg s
@return
*/
      YM //sBool       _compareIgnoreCase        (YAC_Object *_s);

/* @method compareIgnoreWhitespace,String s:boolean

@arg s
@return
*/
      YM //sBool       _compareIgnoreWhitespace  (YAC_Object *_s);

/* @method replaceCharReturnNum,char a,char b:int

@arg a
@arg b
@return
*/
      YM //sSI         _replaceCharReturnNum     (sSI _a, sSI _b);

/* @method replaceChar,char a,char b:String

This method supports the => operator.

@arg a
@arg b
@return
*/
      YM //void        _replaceChar_YAC_RARG    (sSI _a, sSI _b, YAC_Object *_r);
      YM //void        _replaceChar_YAC_RVAL    (sSI _a, sSI _b, YAC_Value *_r);
      YM //void        _replaceChar_YAC_RSELF   (sSI _a, sSI _b);

/* @method indexOfChar,char c,int off:int

@arg c
@arg off
@return
*/
      YM //sSI         _indexOfChar              (sSI _a, sSI _off);

/* @method revIndexOfChar,char c,int off:int

@arg c
@arg off
@return
*/
      YM //sSI         _revIndexOfChar           (sSI _a, sSI _off);

/* @method indexOfFirstDifference,String o:int

@arg o
@return
*/
      YM //sSI         _indexOfFirstDifference   (YAC_Object *_s) const;


/* @method charsetBlacklist,String charset
Remove all chars present in 'charset'.

@arg charset
*/
      YM //void        _charsetBlacklist_YAC_RARG   (YAC_Object *_s, YAC_Object *_r);
      YM //void        _charsetBlacklist_YAC_RVAL   (YAC_Object *_s, YAC_Value *_r);
      YM //void        _charsetBlacklist_YAC_RSELF  (YAC_Object *_s);


/* @method charsetWhitelist,String charset
Keep all chars present in 'charset'.

@arg charset
*/
      YM //void        _charsetWhitelist_YAC_RARG   (YAC_Object *_s, YAC_Object *_r);
      YM //void        _charsetWhitelist_YAC_RVAL   (YAC_Object *_s, YAC_Value *_r);
      YM //void        _charsetWhitelist_YAC_RSELF  (YAC_Object *_s);


/* @method New,String s:String

@arg s
@return
*/
      YM //void        _New                      (YAC_Object *_s, YAC_Value *_r);

/* @method replaceRegion,int startOff,int endOff,String s:String

This method supports the => operator.
@arg startOff
@arg endOff
@arg s
@return
*/
      YM //void        _replaceRegion_YAC_RARG  (sSI _startOff, sSI _endOff, YAC_Object *_repl, YAC_Object *_r);
      YM //void        _replaceRegion_YAC_RVAL  (sSI _startOff, sSI _endOff, YAC_Object *_repl, YAC_Value *_r);
      YM //void        _replaceRegion_YAC_RSELF (sSI _startOff, sSI _endOff, YAC_Object *_repl);

/* @method replaceIncludes,String optFilePrefix:int

@arg optFilePrefix
@return
*/
      YM //sSI         _replaceIncludes          (YAC_String *_optFilePrefix);

/* @method visit,String s,int off,int len

@arg s
@arg off
@arg len
*/
      YM //void        _visit                    (YAC_Object *_s, sSI _off, sSI _len);

/* @method getChecksum:int
Return checksum for the given String.

@return 32bit string checksum
*/
      YM //sSI        _getChecksum              (void);

/* @method getChecksumIgnoreWhitespace:int
Return checksum for the given String, ignoring all white space (< 32) characters.

@return 32bit string checksum
*/
      YM //sSI        _getChecksumIgnoreWhitespace (void);

/* @method getDirName:String
Return directory part of file.

This method supports the => operator.

@return Directory name (including trailing forward/backward slash resp. drive letter)
*/
      YM //void       _getDirName_YAC_RVAL     (YAC_Value *_r);
      YM //void       _getDirName_YAC_RARG     (YAC_Object *_r);


/* @method getDebugString:String

@return
*/
      YM //void       _getDebugString          (YAC_Value *_r);


/* @method checkConversions:int
Check available type conversions

@return YAC_TYPE_INT if string can be converted to int (dec/hex), or YAC_TYPE_FLOAT, or YAC_TYPE_OBJECT (arbitrary string)
*/
      YM //sSI        _checkConversions         (void);


/* @method checkIntConversion:boolean
Check if string is an integer

@return true if string can be converted to integer
*/
      YM //sBool       checkIntConversion     (void) const;


/* @method checkFloatConversion:boolean
Check if string is a float

@return true if string can be converted to float
*/
      YM //sBool       checkFloatConversion     (void) const;


/* @method getMetaFlags:int
Get string meta flags (e.g. whether String was originally enclosed in \"\", TKS_STRFL_QUOT)

@return flags
*/
      YM //sUI        _getMetaFlags             (void);


/* @method wasQuoted:boolean
Check whether String was originally enclosed in \"\" or \'\'

@return flags
*/
      YM //sBool      _wasQuoted                (void);


/* @method wasQuotedSingle:boolean
Check whether String was originally enclosed in \'\'

@return flags
*/
      YM //sBool      _wasQuotedSingle          (void);


/* @method wasQuotedDouble:boolean
Check whether String was originally enclosed in \"\"

@return flags
*/
      YM //sBool      _wasQuotedDouble          (void);


/* @method isPredecessor,String o,boolean bCaseSensitive:boolean
Check whether String is lexicographical predecessor of other string.

@arg o Other string
@arg bCaseSensitive true=perform case sensitive comparison
@return true or false
*/
      YM //sBool      _isPredecessor            (YAC_String *_o, sBool _bCaseSensitive);


/* @method numCharsAt,char c:int
Count number of consecutive characters at the given string offset.

@arg char Character
@arg off Start offset
@return Number of consecutive chars
*/
      YM // sUI       _numCharsAt               (sSI _char, sSI _off) const;


/* @method setQuotFlag,int flag
Set quotation flag

@arg flag
@return Number of consecutive chars
*/
      YM // void      setQuotFlag         (sSI _quotFlag);


/* @method getQuotFlag:int
Get quotation flag.

@return flag 1 when String was wrapped in \"\", 2 when String was wrapped in \'\'.
*/
      YM // sSI       getQuotFlag         (void) const;


/* @method utf8ToASCII
Convert UTF-8 string to ASCII.
Replace non-ASCII chars (>=128) with SUB (26) character.
*/
      YM // void        _utf8ToASCII_YAC_RARG     (YAC_Object *_r);
      YM // void        _utf8ToASCII_YAC_RVAL     (YAC_Value *_r);
      YM // void        _utf8ToASCII_YAC_RSELF    (void);


/* @method utf8ToCP1252
Convert UTF-8 string to CP1252 extended ASCII.
Replace non-convertable chars with SUB (26) character.

(note) CP1252 is the same as ISO8859-1 except for the 0x80..0x9F area (ctl codes in ISO)
(note) CP1252 is the HTML5 default encoding for text labeled as ISO8859-1
*/
      YM // void        _utf8ToCP1252_YAC_RARG  (YAC_Object *_r);
      YM // void        _utf8ToCP1252_YAC_RVAL  (YAC_Value *_r);
      YM // void        _utf8ToCP1252_YAC_RSELF (void);


/* @method cp1252ToUTF8,boolean bBOM
Convert CP1252 extended ASCII string to UTF-8

(note) CP1252 is the HTML5 default encoding for text labeled as ISO8859-1

@arg bBom When true, add U+FEFF byte order mark.
*/
      YM // void        _cp1252ToUTF8_YAC_RARG  (sBool _bBOM, YAC_Object *_r);
      YM // void        _cp1252ToUTF8_YAC_RVAL  (sBool _bBOM, YAC_Value *_r);
      YM // void        _cp1252ToUTF8_YAC_RSELF (sBool _bBOM);


/* @method hasUTFBOM:boolean
Check if string starts with UTF-8 byte order mark (U+FEFF encoded as 0xEF 0xBB 0xBF)

@return True if BOM is present
*/
      YM // sBool       _hasUTF8BOM (void) const;

/* @method isUTF8:boolean
Check if string starts with UTF-8 byte order mark (see §hasUTF8BOM) or if it contains any UTF-8 multi-byte chars

@return True if BOM or UTF-8 multi-byte chars are present
*/
      YM // sBool       _isUTF8 (void) const;

/* @method escapeControlChars
Replace control characters and "' by escape sequences

*/
      YM //void       _escapeControlChars_YAC_RSELF (void) ;
      YM //void       _escapeControlChars_YAC_RVAL (YAC_Value *_r) const;
      YM //void       _escapeControlChars_YAC_RARG (YAC_Object *_r) const;

/*
};
*/
#include "ying_core_String.cpp"
YAC_C_CORE(_String, "String", YAC_CLID_STRING);


#if 0


      /* @method substEscapeSeq
Replace ASCII-style escape sequences by internal ones.

This method is mostly useful when generating script source.
Please see script engine source for details.
@see resubstEscapeSeq,resubstEscapeSeq2
*/
       // void      substEscapeSeq           (void);


/* @method resubstEscapeSeq
Replace internal escape sequences by ASCII-style ones.

Please see script engine source for details.
@see substEscapeSeq,resubstEscapeSeq2
*/
       // void      resubstEscapeSeq         (void);


/* @method resubstEscapeSeq2
Replace internal escape sequences by script-parseable ASCII-style ones.

Please see script engine source for details.
@see substEscapeSeq,resubstEscapeSeq
*/
       // void      resubstEscapeSeq2        (void);
#endif // 0

//------------------------------------------------------------------------------------- prebuffered Strings
#define DprebufString(a) \
class _String##a : public _String {\
private:\
   sU8 prebuf[a];\
public:\
   YAC_H(_String##a);\
   YAC_POOLED_H(_String##a, YAC_POOL_PRIORITY_HIGH);\
   _String##a(void) { chars = prebuf; buflen = a; bflags=0; chars[0]=0; chars[a-1]=0; length=1; }\
};\
YAC_C_CORE_POOLED(_String##a, "String"#a, YAC_CLID_STRING##a)

#define DregisterprebufString(a) \
   templ=new _String##a();         templ->class_ID=YAC_CLID_STRING##a;\
   if(!registerClass(templ, 1, 0)) return 0;\
   cpp_typecast_map[YAC_CLID_STRING##a][YAC_CLID_STRING]=1;\


DprebufString(8);
DprebufString(16);
DprebufString(32);
DprebufString(64);
DprebufString(128);

/* @class String8

A String that can store 8 characters without (re-)allocation.

@see String
*/

/* @class String16

A String that can store 16 characters without (re-)allocation.

@see String
*/

/* @class String32

A String that can store 32 characters without (re-)allocation.

@see String
*/

/* @class String64

A String that can store 64 characters without (re-)allocation.

@see String
*/

/* @class String128

A String that can store 128 characters without (re-)allocation.

@see String
*/

//-------------------------------------------------------------------------------------StringIterator
/* @class StringIterator,Object

Used to iterate a String word stack.

@deprecated Use §String.splitSpace, §String.splitChar, §String.splitCharset, §String.tokenize instead
@see String
*/
YC class _StringIterator : public TKS_StringIterator {
public:
   YAC_POOLED(_StringIterator, YAC_POOL_PRIORITY_HIGH);

/* @method begin,String s

@arg s
*/
   YM //void        begin               (YAC_String *);

/* @method end
*/
      YM //void        end                 (void);

/* @method head
*/
      YM //void        head                (void);

/* @method tail:int

@return
*/
      YM //sSI         _tail               (void);

/* @method next:int

@return
*/
      YM //sSI         _next               (void);

/* @method previous:int

@return
*/
      YM //sSI         _previous           (void);

/* @method getCurrent:String

@return
*/
      YM //YAC_String *_getCurrent         (void);

};
#include "ying_core_StringIterator.cpp"
YAC_C_CORE_POOLED(_StringIterator, "StringIterator", TKS_CLID_STRINGITERATOR);
//------------------------------------------------------------------------------------- Event
/* @class Event,Value

A generic event.

The user-definable integers "id" and "subId" can be used to implement timestamps, event class ids etc.

*/
YC class _Event : public YAC_Event {
public:
   YAC_POOLED(_Event, YAC_POOL_PRIORITY_HIGH);


/* @method setId,int id
@return
*/
   YM //void setId    (sSI _id);


/* @method getId:int
@return
*/
   YM //sSI  getId    (void);


/* @method setSubId,int subId
@return
*/
   YM //void setSubId (sSI _subId);


/* @method getSubId:int
@return
*/
   YM //sSI  getSubId (void);
};
#include "ying_core_Event.cpp"
YAC_C_CORE_POOLED(_Event, "Event", YAC_CLID_EVENT);

//------------------------------------------------------------------------------------- Mailbox
/* @class Mailbox

A generic event mailbox.


*/
YC class _Mailbox : public TKS_Mailbox {
public:
   YAC_POOLED(_Mailbox, YAC_POOL_PRIORITY_MEDIUM);


/* @method getNumQueuedEvents:int
 thread-safe way to get the number of queued events
@return
*/
   YM //sUI getNumQueuedEvents(void);


/* @method freeEventQueue
 Discard all events
*/
   YM //void freeEventQueue  (void);


/* @method allocEventQueue,int size
 Discard events and allocate new (empty) queue
@arg size Queue size
*/
   YM //void allocEventQueue (sUI _size);


/* @method sendEvent,Event ev
 Create a copy of "ev" (deref value) and add to queue
@arg ev Event to send to this mailbox
*/
   YM //sSI  _sendEvent       (YAC_Object *_event);


/* @method peekEvent:Event
 Return reference to oldest event without removing it from the queue
@return Event
*/
   // TODO YAC_Event *peekEvent (void);


/* @method peekEvent,int id:Event
 Return reference to oldest event that matches the given id
@arg id Id to match
@return Event
*/
   // TODO YAC_Event *peekEventById (sSI _id);


/* @method waitEvent,int timeout:Event
 Wait until next event arrives or until timeout expires. return null or next event
@arg timeout Timeout (in millisec). 0=forever
@return Event
*/
   YM //void waitEvent       (sUI _timeout, YAC_Value *_r);

/* @method waitEvent,int id,int timeout:Event
 Wait until next event arrives that matches the given id or until timeout expires. return null or next event
@arg id Id to match
@arg timeout Timeout (in millisec). 0=forever
@return Event
*/
   YM //void waitEventById   (sSI _id, sUI _timeout, YAC_Value *_r);
};
#include "ying_core_Mailbox.cpp"
YAC_C_CORE_POOLED(_Mailbox, "Mailbox", TKS_CLID_MAILBOX);

//------------------------------------------------------------------------------------- HashTable
/* @class HashTable

A dictionary that maps Strings to arbitrary values.
*/
YC class _HashTable : public YAC_HashTable {
public:
   YAC_POOLED(_HashTable, YAC_POOL_PRIORITY_MEDIUM);

/* @method addInt,String key,int i:int

@arg key
@arg i
@return
*/
      YM //sSI  _addInt                 (YAC_String *_s, sSI _val);

/* @method addFloat,String key,float f:int

@arg key
@arg f
@return
*/
      YM //sSI  _addFloat               (YAC_String *_s, sF32 _val);

/* @method addString,String key,String s:int

@arg key
@arg s
@return
*/
      YM //sSI  _addString              (YAC_String *_s, YAC_String *_val);

/* @method addObject,String key,Object o:int

@arg key
@arg o
@return
*/
      YM //sSI  _addObject              (YAC_String *_s, YAC_Object *_val);

/* @method addObjectRef,String key,Value v:int

@arg key
@arg v
@return
*/
      YM //sSI  _addObjectRef           (YAC_String *_s, YAC_Object *_val);

/* @method get,String key:var

@arg key
@return
*/
      YM //void _get                    (YAC_Object *_s, YAC_Value *_r);

/* @method getDeref,String key:var

@arg key
@return
*/
      YM //void _getDeref               (YAC_Object *_s, YAC_Value *_r);

/* @method exists,String key:boolean

@arg key
@return
*/
      YM //sSI  _exists                 (YAC_String *_key);

/* @method delete,String key

@arg key
*/
      YM //void _delete                 (YAC_String *_s);

/* @method free


@see alloc
*/
      YM //void _free                   (void);

/* @method alloc,int cacheSize:boolean
Set new capacity and discard all entries.

@arg cacheSize
@return
@see free
@see realloc
*/
      YM //sBool _alloc                  (sUI _cachesize);

/* @method realloc,int cacheSize:boolean
Set new capacity while keeping current entries.

@arg cacheSize
@return
@see free
@see alloc
*/
      YM //sBool _realloc               (sUI _cachesize);

/* @method getNumElements:int

@return

*/
      YM //sSI  _getNumElements         (void);

/* @method getMaxElements:int

@return
*/
      YM //sSI  _getMaxElements         (void);

/* @method getString:String

@return
*/
      YM //void _getString              (YAC_Value *_r);


/* @method getReparsableString:String
Prepare keys and string values so they can be reparsed.

Mostly usuable for generating script source code.
@return
*/
      YM //void _getReparsableString    (YAC_Value *_r);


/* @method set,String key,Object value

@arg key
@arg value
*/
      YM //void _set                    (YAC_Object *_key, YAC_Object *_value);

/* @method setInt,String key,int i

@arg key
@arg i
*/
      YM //void _setInt                 (YAC_Object *_key, sSI _value);

/* @method setFloat,String key,float f

@arg key
@arg f
*/
      YM //void _setFloat               (YAC_Object *_key, sF32 _value);

/* @method setObject,String key,Object value

@arg key
@arg value
*/
      YM //void _setObject              (YAC_Object *_key, YAC_Object *_value);

/* @method setCopy,String key,Object o

@arg key
@arg o
*/
      YM //void _setCopy                (YAC_Object *_key, YAC_Object *_value);

/* @method setString,String key,String s

@arg key
@arg s
*/
      YM //void _setString              (YAC_Object *_key, YAC_Object *_s);

/* @method keyToArray,Object array:int

@arg array
@return
*/
      YM //sSI  _keysToArray            (YAC_Object *_array);

/* @method valuesToArray,Object array:int

@arg array
@return
*/
      YM //sSI  _valuesToArray          (YAC_Object *_array);

/* @method replaceKey,String oldKey,String newKey:boolean

@arg oldKey
@arg newKey
@return true if key has been replaced
*/
      YM //sBool _replaceKey            (YAC_Object *_oldKey, YAC_Object *_newKey);

};
#include "ying_core_HashTable.cpp"
YAC_C_CORE_POOLED(_HashTable, "HashTable", YAC_CLID_HASHTABLE);


//-------------------------------------------------------------------------------------Condition
/* @class Condition,Object

A boolean condition variable that can be used for multi-threaded notifications resp. barriers.

*/
YC class _Condition : public TKS_Condition {
public:
   YAC(_Condition);

/* @method create,boolean bManualReset:int
   Initialize condition object.

@arg bManualReset - 1=all waiting threads return (must call §reset after §wait), 0=one thread returns, auto-reset condition to 0

@return 0 (success) or error code
*/
   YM //sSI create(sSI _bManualReset);


/* @method raise:int
   Raise condition and notify threads

@return 0 (success) or error code
*/
   YM //sSI raise(void);


/* @method wait,int timeout:int
   Wait for condition to be raised.

   Auto-reset condition to 0 (false) if this not a manual reset condition (see §create).

@arg timeout 0=wait forever, millisec. otherwise

@return 0 (success) or error code
*/
   YM //sSI wait(sSI _timeout);


/* @method reset:int
   Reset condition.

@return 0 (success) or error code
*/
   YM //sSI reset (void);

};
#include "ying_core_Condition.cpp"
YAC_C_CORE(_Condition, "Condition", TKS_CLID_CONDITION);


//-------------------------------------------------------------------------------------Configuration
/* @class Configuration,Object

Provides access to various TkScript related configuration options.

*/
YC class _Configuration : public TKS_ConfigurationInter {
public:
   YAC(_Configuration);

/* @method setDebugLevel,int level

Set the TkScript debug level.

@arg level Debug level (0..99)
*/
   YM //void setDebugLevel (sSI);

/* @method getDebugLevel:int

Return the current TkScript debug level.

@return Current debug level (0..99)
*/
      YM //sSI  getDebugLevel (void);

/* @method setForceInt,boolean b

Enable/disable forced interpretation.

Setting "forceInt" to true will cause all JITted code to be executed by the interpreter.
@arg b Whether to use JIT acceleration (true) or not (false)
*/
      YM //void setForceInt   (sSI);

/* @method getForceInt:boolean

Query whether JIT acceleration is enabled.

@return true if JIT acceleration is enabled, false otherwise.
*/
      YM //sSI  getForceInt   (void);

   // **EXPERIMENTAL** don't call this in regular applications
   YM // void setThreadTerminateOnException (sBool _bEnable);
   YM // void setLockGlobalContext (sBool _bEnable);

};
#include "ying_core_Configuration.cpp"
YAC_C_CORE(_Configuration, "Configuration", TKS_CLID_CONFIGURATION);

//------------------------------------------------------------------------------------- Pool
/* @class Pool,Object

Manages a fixed number of homogenous objects.

Features quite fast alloc/free methods.
*/
YC class _Pool : public TKS_Pool {
public:
   YAC_POOLED(_Pool, YAC_POOL_PRIORITY_MEDIUM);

/* @method setTemplate,Object o

@arg o

*/
   YM //void        setTemplate             (YAC_Object *);

/* @method getTemplate:Object

@return
*/
      YM //YAC_Object *getTemplate             (void);

/* @method alloc,int num:boolean

@arg num
@return
@see free
*/
      YM //sSI         _alloc                  (sSI _num);

/* @method free

@see alloc
*/
      YM //void        _free                   (void);

/* @method qAlloc:int

@return
@see qFree
@see qFreeByObject
*/
      YM //sSI         qAlloc                  (void);

/* @method qFree:int id

@arg id
@see qAlloc
@see qFreeByObject
*/
      YM //void        qFree                   (sSI _nameid);

/* @method empty

*/
      YM //void        empty                   (void);

/* @method isEmpty:boolean

Check if Pool is empty (numElements == 0).
*/
   YM //sBool          isEmpty                 (void);

/* @method getNumElements:int

Query number of used elements.

@return
*/
      YM //sSI         getNumElements          (void);

/* @method getMaxElements:int

@return
*/
      YM //sSI         getMaxElements          (void);

/* @method qFreeByObject,Object o

@arg o
@see qAlloc
@see qFree
*/
      YM //void        _qFreeByObject          (YAC_Object *_o);

/* @method getIDByObject,Object o:int

@arg o
@return
*/
      YM //sSI         getIDByObject           (YAC_Object *_o);
};
#include "ying_core_Pool.cpp"
YAC_C_CORE_POOLED(_Pool, "Pool", TKS_CLID_POOL);

//------------------------------------------------------------------------------------- Stack
/* @class Stack,Object

A LIFO stack of objects.

Dates back to the time when TkScript did not have local variables, yet :-)
*/
YC class _Stack : public TKS_Stack {
public:
   YAC_POOLED(_Stack, YAC_POOL_PRIORITY_HIGH);

/* @method setTemplate,Object o

@arg o
*/
   YM //void        setTemplate   (YAC_Object*);

/* @method getTemplate:Object

@return

*/
      YM //YAC_Object *getTemplate   (void);

/* @method push:Object

@return

*/
      YM //YAC_Object *push          (void);

/* @method pop:Object

@return
*/
      YM //YAC_Object *pop           (void);

/* @method setSize,int size

@arg size

*/
      YM //sSI         setSize       (sSI);

/* @method getSize:int

@return

*/
      YM //sSI         getSize       (void);

/* @method setIndex,int index

@arg index
*/
      YM //void        setIndex      (sSI);

/* @method getIndex:int

@return

*/
      YM //sSI         getIndex      (void);

/* @method init,Object template,int size

@arg template
@arg size
*/
      YM //void        init          (YAC_Object *_template, sSI _size);
};
#include "ying_core_Stack.cpp"
YAC_C_CORE_POOLED(_Stack, "Stack", TKS_CLID_STACK);

//------------------------------------------------------------------------------------- TreeNode
/* @class TreeNode,Value

An L/R tree that can store arbitrary user data along with a String identifier.

@see ListNode
@see Value
*/
YC class _TreeNode : public YAC_TreeNode {
public:
   YAC_POOLED(_TreeNode, YAC_POOL_PRIORITY_MEDIUM);

/* @method insertLeft,Object o:TreeNode

Insert a copy of the given object into the tree.

@arg o
@return
@todo Provide script context for object clone
*/
   YM //YAC_Object *_insertLeft       (YAC_Object *_templ);

/* @method insertRight,Object o:TreeNode

Insert a copy of the given object into the tree.

@arg o
@return
@todo Provide script context for object clone

*/
      YM //YAC_Object *_insertRight      (YAC_Object *_templ);

/* @method findByName,String name,int depth:TreeNode
Find TreeNode by type name

@arg name
@arg depth
@return
*/
      YM //YAC_Object *_findByName       (YAC_String *_s, sSI _depth);


/* @method findById,String id,int depth:TreeNode
Find TreeNode by id.

Note: It might be necessary to replace "name=" by "id=". SGML/HTML usually uses "id" to set the node id, XML uses "name" (not to be confused by type name!).

@arg id
@arg depth
@return
*/
      YM //YAC_Object *_findById         (YAC_String *_s, sSI _depth);


/* @method free

*/
      YM //void        _free             (void);

/* @method getLeft:TreeNode

@return
@see setLeft
@see setRight
@see getRight
*/
      YM //YAC_Object *getLeft           (void);

/* @method getName:String

@return
@see getId
@see setId
@see setName
*/
      YM //YAC_String *getName           (void);

/* @method getParent:TreeNode

@return
*/
      YM //YAC_Object *getParent         (void);

/* @method getRight:TreeNode

@return
@see setRight
*/
      YM //YAC_Object *getRight          (void);

/* @method getRoot:TreeNode

@return
*/
      YM //YAC_Object *getRoot           (void);

/* @method seekByPathName,String pathName:TreeNode

@arg pathName
@return
*/
      YM //YAC_Object *_seekByPathName   (YAC_String *_s);

/* @method setName,String name

@arg name
@see getId
@see getName
@see setId
*/
      YM //void        _setName          (YAC_String *_s);

/* @method setLeft,TreeNode n

@arg n
@see getLeft
@see getRight
@see setRight
*/
      YM //void        _setLeft          (YAC_Object *_o);

/* @method setRight,TreeNode n

@arg n
@see setLeft
@see getLeft
@see getRight
*/
      YM //void        _setRight         (YAC_Object *_o);

/* @method writeToHashTable,HashTable ht

@arg ht
*/
      YM //void        _writeToHashTable (YAC_Object *_o);

/* @method getNumNodes:int

@return
*/
      YM //sUI         getNumNodes       (void);

/* @method getId:String

@return
@see setId
@see setName
@see getName
*/
      YM //YAC_String *getId             (void);

/* @method setId,String id

@arg id
@see getId
@see setName
@see getName
*/
      YM //void        _setId            (YAC_String *_s);
};
#include "ying_core_TreeNode.cpp"
YAC_C_CORE_POOLED(_TreeNode, "TreeNode", YAC_CLID_TREENODE);

// --------------------------------------------------------------------------- Thread
/* @class Thread,Object

Represents a simultaneously running program task.

A thread may run on a different processor (if available).

Each thread has a mailbox that can be used to receive messages from other threads.

@see Mutex
@see GetCurrentThread
*/
YC class _Thread : public TKS_Thread {
public:

/* @constant THREAD_PRIORITY_ABOVE_NORMAL,int
*/
//#define _THREAD_PRIORITY_ABOVE_NORMAL YCI 1

/* @constant THREAD_PRIORITY_BELOW_NORMAL,int
*/
//#define _THREAD_PRIORITY_BELOW_NORMAL YCI -1

/* @constant THREAD_PRIORITY_HIGHEST,int
*/
//#define _THREAD_PRIORITY_HIGHEST YCI 2

/* @constant THREAD_PRIORITY_IDLE,int
*/
//#define _THREAD_PRIORITY_IDLE YCI -15

/* @constant THREAD_PRIORITY_LOWEST,int
*/
//#define _THREAD_PRIORITY_LOWEST YCI -2

/* @constant THREAD_PRIORITY_NORMAL,int
*/
//#define _THREAD_PRIORITY_NORMAL YCI 0

/* @constant THREAD_PRIORITY_TIME_CRITICAL,int
*/
//#define _THREAD_PRIORITY_TIME_CRITICAL YCI 15



/* @constant PROCESS_PRIORITY_CLASS_IDLE,int
*/
//#define _PROCESS_PRIORITY_CLASS_IDLE YCI 0

/* @constant PROCESS_PRIORITY_CLASS_NORMAL,int
*/
//#define _PROCESS_PRIORITY_CLASS_NORMAL YCI 1

/* @constant PROCESS_PRIORITY_CLASS_HIGH,int
*/
//#define _PROCESS_PRIORITY_CLASS_HIGH YCI 2

/* @constant PROCESS_PRIORITY_CLASS_REALTIME,int
*/
//#define _PROCESS_PRIORITY_CLASS_REALTIME YCI 3

	YAC_POOLED(_Thread, YAC_POOL_PRIORITY_LOW);

/* @method create,Function entryFunction:boolean

@arg entryFunction §Function object to be used for thread startup. The arbitrary user argument can be accessed by using the §Thread.getUserData getter function.
@return

*/
	YM //sSI         _create            (YAC_Object *_entryFunction);

/* @method kill

*/
   YM //void        _kill              (void);

/* @method wait:int

@return
*/
   YM //sSI         _wait              (void);

/* @method getId:int

@return
*/
   YM //sSI         _getId             (void);

/* @method setUserData,Object o

@arg o

*/
   YM //void        _setUserdata       (YAC_Object*);

/* @method getUserData:Object

@return

*/
   YM //YAC_Object *_getUserdata       (void);

/* @method isRunning:boolean

@return
*/
   YM //sSI         _isRunning         (void);

/* @method setName,String name

@arg name
*/
   YM //void        _setName           (YAC_String *_name);

/* @method getName:String

@return

*/
   YM //void        _getName           (YAC_Value *_r);

/* @method setPriority,int prio

@arg prio

*/
   YM //void        _setPriority       (sSI _priority);

/* @method getPriority:int

@return
*/
   YM //sSI         _getPriority       (void);


/* @method SetProcessPriorityClass,int priorityClass
  Set process priority class (Windows only).

@arg priorityClass One of PROCESS_PRIORITY_CLASS_IDLE, PROCESS_PRIORITY_CLASS_NORMAL, PROCESS_PRIORITY_CLASS_HIGH, or PROCESS_PRIORITY_CLASS_REALTIME
*/
   YM //void        _SetProcessPriorityClass  (sSI _priorityClass);


/* @method SetProcessAffinityMask32,int mask:boolean
Set process affinity mask (Windows only).

@arg mask Logical processor core bit mask
@return true when the system call succeeded, false otherwise.
*/
   YM //sBool       _SetProcessAffinityMask32 (sUI _mask);


/* @method SetProcessAffinityMaskByString,String mask:boolean
Set process affinity mask (Windows only).

The mask is currently limited to 64 logical processors (todo: support processor groups).

This method is not supported on macOS.

@arg mask Logical processor core bit mask. 'x' denotes set bits, '.' denotes cleared bits, space/tab/CR/LF are ignored and can be used for grouping logical processors
@return true when the system call succeeded, false otherwise.
*/
   YM //sBool       _SetProcessAffinityMaskByString (YAC_Object *_s);


/* @method IsAffinityMaskStringBitSet:String mask,int bit:boolean
Pseudo-static method. Test if the given bit is set in the affinity string mask.

@arg mask Logical processor core bit mask. 'x' denotes set bits, '.' denotes cleared bits, space/tab/CR/LF are ignored and can be used for grouping logical processors
@arg bit 0..63
*/
   YM //sBool       _IsAffinityMaskStringBitSet (YAC_Object *_s, sUI _bit);


/* @method GetMaxAffinityMask32:int
Pseudo-static method. Queries maximum supported affinity mask ( (1<<numProcessors)-1 ).

@arg mask
*/
   YM //sUI        _GetMaxAffinityMask32 (void);


/* @method setAffinityMask32,int mask
Set thread affinity mask.

Keep in mind that this call will fail on Windows if the mask contains bits for non-available processor cores.
Please use §GetMaxAffinityMask32 to query the maximum supported mask.

This method is not supported on macOS.

@arg mask Logical processor core bit mask
*/
   YM //void        _setAffinityMask32 (sUI _mask);


/* @method setAffinityMaskByString,String mask
Set thread affinity mask.

The mask is currently limited to 64 logical processors (todo: support processor groups).

Keep in mind that this call will fail on Windows if the mask contains bits for non-available processor cores.
Please use §GetMaxAffinityMask32 to query the maximum supported mask.

On macOS the mask should contain only a single set bit and the method will fall back to §setCPUCore.

@arg mask Logical processor core bit mask. 'x' denotes set bits, '.' denotes cleared bits, space/tab/CR/LF are ignored and can be used for grouping logical processors
@return true when affinity mask has been updated
*/
   YM //void        _setAffinityMaskByString (YAC_Object *_s);


/* @method setCPUCore,int coreIdx
  Assign thread to specified CPU core.

  Fall back to §setAffinityMask32 when tks was not compiled with HAVE_WIN10 define.

@arg coreIdx Logical CPU core index
*/
   YM //void        _setCPUCore        (sSI _coreIdx);


/* @method allocEventQueue,int size
  Allocate mailbox event queue/ring buffer.

@arg size Maximum number of events that can be posted without discarding old events.
*/
   YM //void        _allocEventQueue (sSI _size);


/* @method sendEvent,Event ev
  Post new message in mailbox.

  This will clone the given §Event object and unlink any deletable userdata object from "ev".

@arg ev New message
*/
   YM //sSI         _sendEvent       (YAC_Object *_ev);


/* @method peekEvent:Event
  Return next pending event without removing it from the queue.

@return Pending event or null
*/
   YM //YAC_Object *_peekEvent       (void);


/* @method peekEventById,int id:Event
  Return next pending event whose id matches "id" without removing it from the queue.

@return Pending event or null
*/
   YM //YAC_Object *_peekEventById   (sSI _id);


/* @method waitEvent,int timeoutMS:Event
  Return next pending event or wait for new event to arrive.

  Please notice that this method returns null in case the wait() timed out or got interrupted.

@arg timeoutMS 0=wait forever, millisec. until timeout otherwise
@return Event or null (timeout or interrupt)
*/
   YM //void        _waitEvent       (sUI _timeoutMS, YAC_Value *_r);


/* @method waitEventById,int id,int timeoutMS:Event
   Return next pending event whose id matches "id" or wait for new matching event to arrive.

  Please notice that this method returns null in case the wait() timed out or got interrupted.

@arg Event id filter
@arg timeoutMS 0=wait forever, millisec. until timeout otherwise
@return Event or null (timeout or interrupt)
*/
   YM //void        _waitEventById   (sSI _id, sUI _timeoutMS, YAC_Value *_r);

   YM //sUI         _rand            (void);
   YM //void        _srand           (sUI _seed);

/* @method SwitchToThread
Schedule next waiting thread.
 */
   YM //sBool       _SwitchToThread  (void);

/* @method Yield
Schedule next waiting thread (same as §SwitchToThread)

Can cause high CPU load when no other threads are waiting to be scheduled.
 */
   YM //sBool       _Yield           (void);

/* @method Delay
Put thread to sleep for a short time (low CPU load).
 */
   YM //sBool       _Delay           (void);

/* @method GetNumCPUCores:int
Query number of logical CPU cores
@ret number of logical CPU cores
*/
   YM //sUI         _GetNumCPUCores  (void);

   /* @method IsCPUCoreShared,int coreIdx:boolean
@arg coreIdx Logical CPU core index
@ret true when logical CPU shares HW resources with other core (hyper threading)
   */
   YM //sBool       _IsCPUCoreShared (sUI _coreIdx);

   /* @method IsPerformanceCPUCore,int coreIdx:boolean
@arg coreIdx Logical CPU core index
@ret true when logical CPU is a performance core (big/little systems)
   */
   YM //sBool       _IsPerformanceCPUCore (sUI _coreIdx);

   /* @method IsCPUCoreShared,int coreIdx:boolean
@arg coreIdx Logical CPU core index
@ret true when logical CPU is an efficiency performance core (big/little systems)
   */
   YM //sBool       _IsEfficiencyCPUCore (sUI _coreIdx);

};
#include "ying_core_Thread.cpp"
YAC_C_CORE_POOLED(_Thread, "Thread", TKS_CLID_THREAD);
// --------------------------------------------------------------------------- Mutex
/* @class Mutex,Object

A mutual exclusive lock is used to avoid the simultaneous execution of a specific code path.

Please notice that TkScript provides for automatic use of mutexes when the <code>=</code>, <code>==</code> resp. <code>=<i>name</i>=</code>
keywords are used in a method declaration.

@see Thread
*/
YC class _Mutex : public TKS_MutexObject {
public:
	YAC_POOLED(_Mutex, YAC_POOL_PRIORITY_LOW);

/* @method lock

@see timedLock
@see unlock
*/
   YM //void lock       (void);

/* @method timedLock,int timeoutSec:boolean
Lock with timeout

@arg timeoutSec Maximum number of seconds to wait
@return true when lock was acquired, false when timeout expired

@see lock
@see unlock
*/
   YM //sBool timedLock (sUI _timeoutSec);

/* @method unlock

@see lock
@see timedLock
*/
	YM //void unlock     (void);
};
#include "ying_core_Mutex.cpp"
YAC_C_CORE_POOLED(_Mutex, "Mutex", TKS_CLID_MUTEX);
// --------------------------------------------------------------------------- Process
/* @class Process,Stream

Represents an external process that communicates with its parent via anonymous pipes.

Writing to the stream sends data to the standard input of the external process.

The standard output (and error output) can be accessed by reading data from the stream.

The stream size indicates how many bytes can be read from the child process.

The §waitAvail() method supports an additional timeout parameter (uses select() on Linux and a polling loop on Windows).

@see Stream
*/
YC class _Process : public TKS_Process {
public:
	YAC_POOLED(_Process, YAC_POOL_PRIORITY_LOW);

   /* @method exec,String cmd,StringArray args
      Create new process and execute command

      @arg cmd Command pathname (e.g. "/bin/ls").
      @arg args Command arguments. §StringArray or null.
      @return true when the process has been started.
   */
   YM // sBool exec (YAC_String *_cmd, YAC_Object *_args);

   /* @method waitAvail,int timeoutMillisec
      Query number of available bytes. Wait up to <timeoutMillisec> for new data to arrive.

      @arg timeoutMillisec Timeout. 0=no timeout.
      @return Number of bytes available for reading
   */
   YM // sUI waitAvail (sUI _timeoutMillisec);

   /* @method kill
      Kill child process.

      On Linux, this sends the SIGTERM signal, and after one second SIGKILL.
      On Windows, TerminateProcess() is called.
   */
   YM // void kill (void);

   /* @method GetPID:int
   Get pid of current process.
   @return pid
   */
   YM // sSI GetPID (void);

   /* @method SendSignal,int pid,int signal
   Send signal to other process. Not implemented on Windows.
   */
   YM // void SendSignal (sSI _pid, sSI _signal);
   YM // void SendSignal_SIGUSR1 (sSI _pid);
   YM // void WaitSignal_SIGUSR1 (void);
};
#include "ying_core_Process.cpp"
YAC_C_CORE_POOLED(_Process, "Process", TKS_CLID_PROCESS);
// --------------------------------------------------------------------------- Time
/* @class Time,Object

A utility class to handle time stamps. Provides date conversion.

*/
YC class _Time : public TKS_Time {
public:
   YAC_POOLED(_Time, YAC_POOL_PRIORITY_MEDIUM);

/* @method calc
*/
      YM //void calc           (void);

/* @method getMin:int

@return
*/
      YM //sSI  getMin         (void);

/* @method setMin,int min

@arg min
*/
      YM //void setMin         (sSI);

/* @method getHour:int

@return
*/
      YM //sSI  getHour        (void);

/* @method getMonth:int

@return
*/
      YM //sSI  getMonth       (void);

/* @method getMonthday:int

@return
*/
      YM //sSI  getMonthday    (void);

/* @method getNanoSec:int

@return
*/
      YM //sSI  getNanoSec     (void);

/* @method getSec:int

@return
*/
      YM //sSI  getSec         (void);

/* @method getUtime:int

@return
*/
      YM //sUI  getUtime       (void);

/* @method getWeekday:int

@return
*/
      YM //sSI  getWeekday     (void);

/* @method getYear:int

@return
*/
      YM //sSI  getYear        (void);

/* @method getYearDay:int

@return
*/
      YM //sSI  getYearday     (void);

/* @method gmtime

*/
      YM //void _gmtime        (void);

/* @method localtime

*/
      YM //void _localtime     (void);

/* @method now

*/
      YM //void now            (void);

/* @method setHour,int h

@arg h
*/
      YM //void setHour        (sSI);

/* @method setMonth,int m

@arg m
*/
      YM //void setMonth       (sSI);

/* @method setNanoSec,int ns

@arg ns

*/
      YM //void setNanoSec     (sSI);

/* @method setSec,int s

@arg s
*/
      YM //void setSec         (sSI);

/* @method setUtime,int u

@arg u
*/
      YM //void setUtime       (sUI);

/* @method setMonthday,int md

@arg md
*/
      YM //void setMonthday    (sSI);

/* @method setYear,int yr

@arg yr
*/
      YM //void setYear        (sSI);

/* @method getString:String
*/
      YM //void _getString     (YAC_Value *_r)

/* @method liesBetween,Time tStart,Time tEnd:boolean
Check if date lies between tStart and tEnd (inclusive).

@return true when (tStart <= this <= tEnd)
*/
      YM //sBool _liesBetween  (YAC_Object *_tStart, YAC_Object *_tEnd);


};
#include "ying_core_Time.cpp"
YAC_C_CORE_POOLED(_Time, "Time", TKS_CLID_TIME);
// --------------------------------------------------------------------------- Value
/* @class Value,Object

An object container for arbitrary script values.

@see ValueArray
*/
YC class _Value : public YAC_ValueObject {
public:
   YAC_POOLED(_Value, YAC_POOL_PRIORITY_HIGH);

/* @constant YAC_TYPE_VOID,int

Script type id (0) for void values.
*/

/* @constant YAC_TYPE_INT,int

Script type id (1) for 32bit signed integer values.
*/

/* @constant YAC_TYPE_FLOAT,int

Script type id (2) for 32bit single precision IEEE floating point values.
*/

/* @constant YAC_TYPE_OBJECT,int

Script type id (3) for object pointer values.
*/

/* @constant YAC_TYPE_STRING,int

Script type id (4) for string values.

The string type id is more like a hint for the script engine.

Strings are just Objects, too.
*/


/* @method yacOperatorAssign,Object o

@arg o
*/
   YM //void         yacOperatorAssign (YAC_Object*);

/* @method getIntValue:int

@return
*/
      YM //sSI          getIntValue       (void);

/* @method getFloatValue:float

@return
*/
      YM //sF32         getFloatValue     (void);

/* @method getObjectValue:Object

@return
*/
      YM //YAC_Object  *getObjectValue    (void);

/* @method getValue:var

@return
*/
      YM //void        _getValue          (YAC_Value *_r);

/* @method getType:int

@return
*/
      YM //sSI         _getType           (void);

/* @method setObjectValue,Object o

@arg o
*/
      YM //void        _setObjectValue    (YAC_Object *_o);

/* @method setStringValue,String s

@arg s
*/
      YM //void        _setStringValue    (YAC_Object *_s);

/* @method typecast,int type

@arg type
*/
      YM //void        typecast           (sSI);

/* @method unset
*/
      YM //void        _unset             (void);

/* @method setNewObject,Object template:Object

@arg template
@return
*/
      YM //YAC_Object *_setNewObject      (YAC_Object *_templ);

/* @method setValue,Value value

@arg value
*/
      YM //void        _setValue          (YAC_Object *);

/* @method getString:String

@return
*/
      YM //void        _getString         (YAC_Value *_r);

/* @method getStringValue:String

@return
*/
      YM //void        getStringValue     (YAC_Value *_r);

/* @method setIntValue,int i

@arg i
*/
      YM //void        _setIntValue       (sSI);

/* @method setFloatValue,float f

@arg f
*/
      YM //void        _setFloatValue     (sF32);

/* @method initNull
*/
      YM //void       initNull            (void);

/* @method initVoid
*/
      YM //void       initVoid            (void);

/* @method derefObjectValue:var

@return
*/
      YM //void       derefObjectValue    (YAC_Value *_r);

/* @method getDeref:var

@return
*/
      YM //void        _getDeref          (YAC_Value *_r);

/* @method isDeletable:boolean
Check whether the associated object value is deletable.

@return 1(true) if object value is deletable, 0(false) if value is not an object or the object is not deletable.
*/
      YM //sSI         _isDeletable       (void);

/* @method unlinkObject:var
Unlink deletable object pointer from value container.
The object will be tagged as non-deletable in this container.

@return Deletable object value or regular int/float/Object value.
*/
      YM //void        _unlinkObject      (YAC_Value *_r);

/* @method assign,Value v

Assign value and keep original value type (typecast if necessary).

@return
*/
   YM //void        _assign     (YAC_Object *_valueObject);

/* @method pointerAssign,Value v

Assign/Dereference §Object pointer. Both values must have type §Object or §String.
*/
   YM //void        _pointerAssign     (YAC_Object *_valueObject);

   /* @method isVoid:boolean
      @return true when value has type void
    */
   YM //sBool       _isVoid            (void);


   /* @method isInt:boolean
      @return true when value has type int
    */
   YM //sBool       _isInt             (void);


   /* @method isFloat:boolean
      @return true when value has type float
    */
   YM //sBool       _isFloat           (void);


   /* @method isObject:boolean
      @return true when value has type §Object
    */
   YM //sBool       _isObject          (void);


   /* @method isString:boolean
      @return true when value has type §String or has type §Object and object is a §String
    */
   YM //sBool       _isString          (void);
};
#include "ying_core_Value.cpp"
YAC_C_CORE_POOLED(_Value, "Value", YAC_CLID_VALUE);
// --------------------------------------------------------------------------- Variable
/* @class Variable,Object

Represents a script variable.

@see Function
@see Script
*/
YC class _Variable : public TKS_ScriptVariable {
public:
   YAC_POOLED(_Variable, YAC_POOL_PRIORITY_MEDIUM);


/* @method query:boolean

@return
*/
   YM //sSI         query       (void);

/* @method store:boolean

@return
*/
   YM //sSI         store       (void);

/* @method storeDeref

@return
*/
   YM //sSI         storeDeref  (void);

/* @method getName:String

@return
*/
   YM //YAC_String *getName     (void);
};
#include "ying_core_Variable.cpp"
YAC_C_CORE_POOLED(_Variable, "Variable", TKS_CLID_VARIABLE);
// --------------------------------------------------------------------------- Function
/* @class Function,Object

Represents a script function.

@see Variable
@see Script
*/
YC class _Function : public TKS_FunctionObject {
public:
   YAC_POOLED(_Function, YAC_POOL_PRIORITY_LOW);

/* @method getName:String

@return
*/
   YM //YAC_String *getName      (void);

/* @method eval,Object va:var

@arg va Argument list (or null)
@return
*/
   YM //void        eval         (YAC_Object *_va, YAC_Value *_r);

/* @method findVariable,String name:Variable

@arg name
@return
*/
   YM //void        findVariable (YAC_String *_s, YAC_Value *_r);
};
#include "ying_core_Function.cpp"
YAC_C_CORE_POOLED(_Function, "Function", TKS_CLID_FUNCTION);
// --------------------------------------------------------------------------- Script
/* @class Script,Object

Represents a dynamically loaded/compiled script.

@see Function
@see Variable
*/
YC class _Script : public TKS_ScriptModule {
public:
   YAC_POOLED(_Script, YAC_POOL_PRIORITY_LOW);


/* @method setClassNamespace,String nspName
Set namespace name for classes declared in this (temporary) script module.

(note) a unique namespace must be set in case the script source is loaded in multiple §Script instances
@arg nspName Unique namespace name
@return true if namespace was set
*/
   YM //sBool setClassNamespace (YAC_String *_nspName);


/* @method installPrintHook
Install trace / print / stdout / stderr hook and capture output to string buffer.
*/
   YM //void  installPrintHook (void);


/* @method uninstallPrintHook
Uninstall trace / print / stdout / stderr hook.
*/
   YM //void  uninstallPrintHook (void);


/* @method getPrintHookOutputAndClear:String
Return a copy of the current trace / print / stdout / stderr capture buffer and clear the buffer.
*/
   YM //void  getPrintHookOutputAndClear (YAC_Value *_r);


/* @method setLineOffset,int
Skip line numbers, e.g. after adding implicit header lines to script source.
@arg offset 0..n
*/
   YM //void  setLineOffset (sUI _offset);


/* @method load,String script:boolean

@arg script
@return
*/
   YM //sSI  load         (YAC_String *_s);

/* @method unload
*/
   YM //void unload       (void);


/* @method getLastErrorLineIndex:int
*/
   YM //sSI  getLastErrorLineIndex (void);


/* @method findVariable,String name:Variable

@arg name
@return
*/
   YM //void findVariable (YAC_String *_name, YAC_Value *_r);


/* @method findTaggedVariableByIndex,int index:Variable

@arg index
@return
*/
   YM //void findTaggedVariableByIndex (sSI _index, YAC_Value *_r);



/* @method findFunction,String name:Function

@arg name
@return
*/
   YM //void findFunction (YAC_String *_name, YAC_Value *_r);

/* @method eval
Run script statements
*/
   YM //void eval         (void);
};
#include "ying_core_Script.cpp"
YAC_C_CORE_POOLED(_Script, "Script", TKS_CLID_SCRIPT);

// --------------------------------------------------------------------------- ListNode
/* @class ListNode,Value

Represents a single node in a double linked list. Can store arbitrary script values.

@see List
@see TreeNode
@see Value
*/
YC class _ListNode : public YAC_ListNode {
public:
   YAC_POOLED(_ListNode, YAC_POOL_PRIORITY_MEDIUM);

/* @method getHead:ListNode

@return
*/
   YM //YAC_ListNode * getHead         (void);

/* @method  getTail:ListNode

@return
*/
   YM //YAC_ListNode * getTail         (void);

/* @method getPrev:ListNode

@return
*/
   YM //YAC_ListNode * getPrev         (void);

/* @method getNext:ListNode

@return
*/
   YM //YAC_ListNode * getNext         (void);

/* @method freeList
*/
   YM //void          _freeList        (void);

/* @method appendEmpty:ListNode

@return
*/
   YM //YAC_ListNode *_appendEmpty     (void);

/* @method appendValue,Value v:ListNode

@deprecated superceeded by §append
@arg v
@return
*/
   YM //YAC_ListNode *_appendValue     (YAC_Object *_v); // deprecated, superceeded by append(value)

/* @method append,Object value:ListNode
Append empty list node and copy value.

@arg value Which value to copy. Will grab deletable objects if argument is a §Value object.
@return
*/
   YM //YAC_ListNode *_append          (YAC_Object *_v);

/* @method getString:String

@return
*/
   YM //void          _getString       (YAC_Value *_r);

/* @method getDebugString:String

@return
*/
   YM //void          _getDebugString  (YAC_Value *_r);

/* @method getDebugStrings:StringArray

@return
*/
   YM //void          _getDebugStrings (YAC_Value *_r);

/* @method getCopy:var

@return
*/
   YM //void          _getCopy         (YAC_Value *_r);

/* @method getSize:int

@return
*/
   YM //sUI           _getSize         (void);

/* @method getSubList,int off:ListNode

@arg off
@return
*/
   YM //YAC_Object *  getSubList       (sSI);

   /////////// sBool yacToString(YAC_Value *_r) { _getString(_r); }
};
#include "ying_core_ListNode.cpp"
YAC_C_CORE_POOLED(_ListNode, "ListNode", YAC_CLID_LISTNODE);
// --------------------------------------------------------------------------- List
/* @class List,Object

A container for list nodes.

@see ListNode
*/
YC class _List : public YAC_List {
public:
   YAC_POOLED(_List, YAC_POOL_PRIORITY_MEDIUM);

/* @method addFirst,Object o:ListNode

@arg o
@return
*/
   YM //YAC_Object *  _addFirst       (YAC_Object *_value);

/* @method addFirstInt,int i:ListNode

@arg i
@return
*/
   YM //YAC_Object *  _addFirstInt    (sSI);

/* @method addFirstFloat,float f:ListNode

@arg f
@return
*/
   YM //YAC_Object *  _addFirstFloat  (sF32);

/* @method addFirstObject,Object o:ListNode

@arg o
@return
*/
   YM //YAC_Object *  _addFirstObject (YAC_Object *_oref);

/* @method addFirstCopy,Object o:ListNode

@arg o
@return
*/
   YM //YAC_Object *  _addFirstCopy   (YAC_Object *_oref);

/* @method addFirstString,String s:ListNode

@arg s
@return
*/
   YM //YAC_Object *  _addFirstString (YAC_Object *_sval);

/* @method addLast,Object o:ListNode

@arg o
@return
*/
   YM //YAC_Object *  _addLast        (YAC_Object *_value);

/* @method addLastInt,int i:ListNode

@arg i
@return
*/
   YM //YAC_Object *  _addLastInt     (sSI);

/* @method addLastFloat,float f:ListNode

@arg f
@return
*/
   YM //YAC_Object *  _addLastFloat   (sF32);

/* @method addLastObject,Object o:ListNode

@arg o
@return
*/
   YM //YAC_Object *  _addLastObject  (YAC_Object *_oref);

/* @method addLastCopy,Object o:ListNode

@arg o
@return
*/
   YM //YAC_Object *  _addLastCopy    (YAC_Object *_oref);

/* @method addLastString,String s:ListNode

@arg s
@return
*/
   YM //YAC_Object *  _addLastString  (YAC_Object *_sval);

/* @method getCopy:var

@return
*/
   YM //void          _getCopy        (YAC_Value *_r);

/* @method getHead:ListNode

@return
*/
   YM //YAC_Object *   getHead        (void);

/* @method getSize:int

@return
*/
   YM //sUI           _getSize        (void);

/* @method getString:String

@return
*/
   YM //void          _getString      (YAC_Value *_r);

/* @method getTail:ListNode

@return
*/
   YM //YAC_Object *   getTail        (void);

/* @method isEmpty:boolean

@return
*/
   YM //sSI            isEmpty        (void);

/* @method removeAll:ListNode

@return
*/
   YM //void          _removeAll      (YAC_Value *_r);

/* @method removeFirst:ListNode

@return
*/
   YM //void          _removeFirst    (YAC_Value *_r);

/* @method removeLast:ListNode

@return
*/
   YM //void          _removeLast     (YAC_Value *_r);

/* @method inverse
*/
   YM //void          _reverse        (void);

/* @method findPointer,Object o:ListNode

@arg o
@return
*/
   YM //YAC_Object *  _findPointer    (YAC_Object *_ptr);

/* @method remove,Object o:ListNode

@arg
@return
*/
   YM //void          _remove         (YAC_Object *_ptr, YAC_Value *_r);

/* @method getSubList,int off:ListNode

@arg off
@return
*/
   YM //YAC_Object *   getSubList     (sSI);

/* @method insert,Object o, Object ln:ListNode

@arg o
@arg ln
@return
*/
   YM //YAC_ListNode *_insert         (YAC_Object *_v, YAC_Object *_ln);
};
#include "ying_core_List.cpp"
YAC_C_CORE_POOLED(_List, "List", YAC_CLID_LIST);

//------------------------------------------------------------------------------------- ValueArray
/* @class ValueArray,Object

An array of dynamically typed script values.

@see Value
*/
YC class _ValueArray : public YAC_ValueArray {
public:
   YAC_POOLED(_ValueArray, YAC_POOL_PRIORITY_MEDIUM);

/* @method alloc,int max:boolean

Allocate the given number of array elements.

@arg max Total number of elements to allocate
@return true when the allocation succeeded, false otherwise.
@see free
@see realloc
@see getMaxElements
*/
   YM //sBool alloc          (sUI _maxelements);

/* @method free

Free all array elements.
@see alloc
@see realloc
@see getMaxElements
*/
   YM //void  free           (void);

/* @method realloc,int max:boolean

Resize array.

Elements will be discarded/added as required.

@arg max New total number of elements
@return true when the reallocation succeeded, false otherwise.
@see alloc
@see free
@see getMaxElements
*/
   YM //sBool realloc        (sUI _maxelements);

/* @method getNumElements:int

Return the number of used elements, numElements.

@return Number of used elements
@see empty
@see setNumElements
*/
   YM //sUI   getNumElements (void);

/* @method getMaxElements:int

Return the total number of elements.

@return Total number of elements
@see alloc
@see free
@see realloc
*/
   YM //sUI   getMaxElements (void);

/* @method empty

Reset number of elements.

@see getNumElements
@see setNumElements
*/
   YM //void  empty          (void);

/* @method isEmpty:boolean

Check if array is empty (numElements == 0).
*/
   YM //sBool isEmpty        (void);

/* @method add,Value val

Add new §Value.
*/
   YM //sBool _add            (YAC_Object *_valueObject);

/* @method addVoid

Add void value.
*/
   YM //sBool _addVoid        (void);

/* @method insert

Insert §Value at index
*/
   YM //sBool _insert         (sSI _index, YAC_Object *_valueObject);

/* @method delete

Delete §Value at index
*/
   YM //sBool _delete         (sSI _index);

/* @method reverse

Reverse element order
*/
   YM //void  reverse        (void);

/* @method setNumElements,int num

Set the number of used elements.

@arg num New number of used elements
@see empty
@see getNumElements
*/
   YM //void  setNumElements (sUI);

/* @method indexOfPointer,Object o:int

Return the index of the element that points to the given object.

@arg o The object pointer to look for
@arg startOff Start index
@return Array index or -1 if the object address was not found in this array
@see containsPointer
*/
   YM //   sSI         indexOfPointer (YAC_Object *_o, sSI _startOff);

/* @method indexOfObject,Object o:int

Return the index of the element that equals the given object.

@arg o The object to look for
@arg startOff Start index
@return Array index or -1 if no matching object was found in this array
@see containsObject
*/
   YM //   sSI         indexOfObject  (YAC_Object *_o, sSI _startOff);

/* @method containsObject,Object o:boolean

Check whether array contains an object that equals the given object.

@arg o The object to compare with. §Object.yacEquals is used for comparisons.
@return true if a matching object was found, false otherwise.
@see indexOfObject
*/
   YM //sSI          containsObject(YAC_Object *_o);

/* @method containsPointer,Object o:boolean

Check whether array contains the given object address.

@arg o The object (address) to compare with.
@return true if a matching object was found, false otherwise.
@see indexOfPointer
*/
   YM //sSI          containsPointer(YAC_Object *_o);

/* @method getString:String

Return a string representation of this array.

@return new String instance holding a string representation of this array.
*/
   YM //void  _getString     (YAC_Value *_r);

/* @method get,int index:var

Return the given array element.

Silently return <code>void</code> if the given index is out of bounds.

@arg index Which array element to return
@return element value (non-deletable reference if the element holds an §Object)
@see getDeref
*/
   YM //void  get            (sSI, YAC_Value *_r);

/* @method set,int index,Object val

Set element at "index" to "val".
Updates numElements when index exceeds it (>=).

Silently do nothing when index exceed array size (>= maxElements).

Transfers pointer ownership if "val" is a §Value object which stores a deletable object.

@arg index Array index
@arg val New value
*/
   YM //void  _set           (sUI, YAC_Object *_val);

/* @method getRev,int index:var

Return the array element at (numElements - 1 - index).

Silently return <code>void</code> if the given index is out of bounds.

@arg index Which array element to return
@return element value (non-deletable reference if the element holds an §Object)
@see getDeref
*/
   YM //void  _getRev        (sSI, YAC_Value *_r);

/* @method setRev,int index,Object val

Set element at (numElements - 1 - index) to "val".

Silently do nothing when index exceed array size (>= maxElements).

Transfers pointer ownership if "val" is a §Value object which stores a deletable object.

@arg index Array index
@arg val New value
*/
   YM //void  _setRev        (sUI, YAC_Object *_val);

/* @method getDeref,int index:var

Return the given (deletable) array element.

Silently return <code>void</code> if the given index is out of bounds.

@arg index Which array element to return
@return element value (deletable reference if the elements holds an §Object)
@see get
*/
   YM //void  getDeref       (sSI, YAC_Value *_r);

/* @method swap:int indexA,int indexB:boolean

Swap two array elements.

@arg indexA Index of first element
@arg indexB Index of second element
@return true if the elements have been swapped successfully, false otherwise.
*/
   YM //sSI   _swap          (sSI _indexs, sSI _indexd);

/* @method useAll

Mark all array elements used, i.e. set numElements = maxElements.

*/
   YM //void  _useAll         (void);

/* @method getFirst:var

Return first array element.

@return First array element or void if the array is empty. Objects are returned as non-deletable references.

*/
   YM //void  _getFirst       (YAC_Value *_r);

/* @method getLast:var

Return last used array element.

@return Last used array element (numElements - 1) or void if the array is empty. Objects are returned as non-deletable references.

*/
   YM //void  _getLast        (YAC_Value *_r);

/* @method joinArrays,Object a,Object b,boolean bCopyA,boolean bCopyB,boolean bUnlinkA,boolean bUnlinkB:boolean

Join arrays 'a' and 'b'.

If none of the copy/unlink flags is set, object values will be copied by reference.

In case that 'a' or 'b' is 'this', object values will always be unlinked ("deref").

The §joinCC, §joinCR, §joinCD, §joinRR, §joinRC, §joinRC, §joinRD, §joinDD, §joinDC, §joinDC are provided
for reasons of convenience (permutations of the copyA/copyB/unlinkA/unlinkB arguments).

(C=copy, R=reference, D=deref (unlink))

@arg a Other array (or this, or null)
@arg b Other array (or this, or null)
@arg bCopyA If true, copy/duplicate array 'a' object values
@arg bCopyB If true, copy/duplicate array 'b' object values
@arg bUnlinkA If true, unlink object values from array 'a'. Takes precedence over bCopyA.
@arg bUnlinkB If true, unlink object values from array 'b'. Takes precedence over bCopyA.

@return True if arrays were joined. False when new array elements could not be allocated.
*/
   YM //sBool _joinArrays (YAC_Object *_a, YAC_Object *_b, sBool _bCopyA, sBool _bCopyB, sBool _bUnlinkA, sBool _bUnlinkB);

   /* @method joinCC,Object a,Object b:boolean
Join arrays a and b. Copy elements.
   */
   YM //sBool _joinCC (YAC_Object *_a, YAC_Object *_b);


   /* @method joinCR,Object a,Object b:boolean
Join arrays a and b. Copy 'A' elements, reference 'B' elements.
   */
   YM //sBool _joinCR (YAC_Object *_a, YAC_Object *_b);


   /* @method joinCD,Object a,Object b:boolean
Join arrays a and b. Copy 'A' elements, dereference 'B' elements.
   */
   YM //sBool _joinCD (YAC_Object *_a, YAC_Object *_b);


   /* @method joinRR,Object a,Object b:boolean
Join arrays a and b. Reference elements.
   */
   YM //sBool _joinRR (YAC_Object *_a, YAC_Object *_b);


   /* @method joinRC,Object a,Object b:boolean
Join arrays a and b. Reference 'A' elements, copy 'B' elements.
   */
   YM //sBool _joinRC (YAC_Object *_a, YAC_Object *_b);


   /* @method joinRD,Object a,Object b:boolean
Join arrays a and b. Reference 'A' elements, dereference 'B' elements.
   */
   YM //sBool _joinRD (YAC_Object *_a, YAC_Object *_b);


   /* @method joinDD,Object a,Object b:boolean
Join arrays a and b. Dereference elements.
   */
   YM //sBool _joinDD (YAC_Object *_a, YAC_Object *_b);


   /* @method joinDC,Object a,Object b:boolean
Join arrays a and b. Dereference 'A' elements, copy 'B' elements.
   */
   YM //sBool _joinDC (YAC_Object *_a, YAC_Object *_b);


   /* @method joinDR,Object a,Object b:boolean
Join arrays a and b. Dereference 'A' elements, reference 'B' elements.
   */
   YM //sBool _joinDR (YAC_Object *_a, YAC_Object *_b);

};
#include "ying_core_ValueArray.cpp"
YAC_C_CORE_POOLED(_ValueArray, "ValueArray", YAC_CLID_VALUEARRAY);


//------------------------------------------------------------------------------------- Exception
/* @class Exception,Object

Represents an Error object generated by the <code>throw</code> statement.

*/
YC class _Exception: public TKS_Exception {
public:
   YAC_POOLED(_Exception, YAC_POOL_PRIORITY_HIGH);

/* @method getId:int

Get exception/error id.

@return Exception/error id
*/
   YM //sSI         getId         (void);

/* @method getName:String

Get exception/error class name
@return Exception/error class name
*/
   YM //YAC_String *getName       (void);

/* @method getFullName:String

Get exception/error class name, prefixed by parent class name (if any)

@return Exception/error class name
*/
   YM //void        getFullName   (YAC_Value *_r);

/* @method getMessage:String

Get the user defined error message.

@return Error message
*/
   YM //YAC_String *getMessage    (void);

/* @method getStackTrace:String

Return a string-representation of the stack trace (as seen when the error occured).

@return Stack trace debug string
*/
   YM //void        getStackTrace (YAC_Value *_r);

};
#include "ying_core_Exception.cpp"
YAC_C_CORE_POOLED(_Exception, "Exception", TKS_CLID_EXCEPTION);


//------------------------------------------------------------------------------------- DummyStream
/* @class DummyStream,Object

A no-op §Stream that can be used to determine buffer sizes for real write ops.

*/
YC class _DummyStream: public TKS_DummyStream {
public:
   YAC_POOLED(_DummyStream, YAC_POOL_PRIORITY_LOW);

/* @method setSize,int num:boolean

Set stream size.

@arg num New stream size
@return true
*/
   YM //sSI  setSize    (sSI _numbytes);
};
#include "ying_core_DummyStream.cpp"
YAC_C_CORE_POOLED(_DummyStream, "DummyStream", TKS_CLID_DUMMYSTREAM);


// --------------------------------------------------------------------------- LFSR
/* @class LFSR

Pseudo random number generator (linear feedback shift register).
*/
YC class _LFSR : public TKS_LFSR {
public:
   YAC_POOLED(_LFSR, YAC_POOL_PRIORITY_MEDIUM);

/* @method init,int seed

@arg seed Random seed
@arg numPre Number of warm-up rands() calls
*/
   YM //void init        (sUI _seed, sUI _numPre);

/* @method rands:int
@return Pseudo-random signed integer
*/
   YM //sSI  rands       (void);

/* @method randu:int
@return Pseudo-random unsigned integer
*/
   YM //sUI  randu       (void);

/* @method randf:float max
@return Pseudo-random float between 0..max
*/
   YM //sF32 randf       (sF32 _max);

};
#include "ying_core_LFSR.cpp"
YAC_C_CORE_POOLED(_LFSR, "LFSR", YAC_CLID_LFSR);


// --------------------------------------------------------------------------- LFSR_NR
/* @class LFSR_NR

Non-repeating Pseudo random number generator (linear feedback shift register).

Warning: This is meant for _small_ random arrays (e.g. a sequence of 32 random values).
 It becomes exponentially slower with larger array sizes, e.g.:
    len=   8: ~9.1 mrand/sec
    len=  16: ~7.0 mrand/sec
    len=  32: ~5.8 mrand/sec
    len=  64: ~4.2 mrand/sec
    len= 128: ~2.9 mrand/sec
    len= 256: ~1.9 mrand/sec
    len= 512: ~1.1 mrand/sec
    len=1024: ~0.6 mrand/sec
    len=2048: ~0.3 mrand/sec
    len=4096: ~0.15 mrand/sec
 (see tks-examples/lfsr.tks)
*/
YC class _LFSR_NR : public TKS_LFSR_NR {
public:
   YAC_POOLED(_LFSR_NR, YAC_POOL_PRIORITY_MEDIUM);

/* @method init,int seed,int len
@arg seed 0=use address of this (pseudo-random seed). >0: use given uint32 value
@arg len Number of random values
@arg numPre Number of warm-up rands() calls (suggested value: 8)
*/
   YM //void init        (sUI _seed, sUI _len, sUI _numPre);

/* @method getNext
Return next random value in the range 0..(len-1)
*/
   YM //sUI getNext      (void);

};
#include "ying_core_LFSR_NR.cpp"
YAC_C_CORE_POOLED(_LFSR_NR, "LFSR_NR", YAC_CLID_LFSR_NR);



// ---------------------------------------------------------------------------- function impl
/* @function exit,int return
Exit with the given return code
*/
void YAC_CALL APIC_exit(sSI _return) { // *NOT* static !
   tkscript->i_return = _return;
   // TKS_MT: stop all threads
   tkscript->main_context->b_running = YAC_FALSE;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static void YAC_CALL _APIC_exit(yacmemptr _args) { APIC_exit(_args.mem[0].si); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function GetCurrentThread:Thread
Query current thread object

TkScript must have been compiled with thread local storage support (HAVE_TLS)
or this function will always return null.

@return Current §Thread object
@see Thread
*/
static YAC_Object *YAC_CALL APIC_GetCurrentThread(void) {
#ifndef YAC_FORCE_NO_TLS
   return tks_current_thread;
#else
   tkscript->printf("[~~~] GetCurrentThread() called but TLS support is not compiled in (define HAVE_TLS and recompile!). returning main_thread..\n");
   return tkscript->main_thread;
#endif // HAVE_TLS
}




/* @function getenv,String name:String
Get environment variable

This is a wrapper for the getenv() native function.
*/
static YAC_Object *YAC_CALL APIC_getenv(YAC_Object *_name) {
#ifdef DX_SYSEXEC
   if(YAC_BCHK(_name, YAC_CLID_STRING))
   {
      YAC_String *name=(YAC_String*)_name;
      char *s=::getenv((const char*)name->chars);
      if(s)
      {
         tkscript->s_getenv.visit(s);
         return &tkscript->s_getenv;
      }
   }
#endif
   return yac_null;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static YAC_Object *YAC_CALL _APIC_getenv(yacmemptr _args) { return APIC_getenv(_args.mem[0].o); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function putenv,String name:String
Set environment variable

This is a wrapper for the putenv() native function.
*/
static sSI YAC_CALL APIC_putenv(YAC_Object *_name) {
#ifdef DX_SYSEXEC
   if(YAC_BCHK(_name, YAC_CLID_STRING))
   {
      YAC_String *name=(YAC_String*)_name;
#ifdef HAVE_ISOCPP
	  return ::_putenv((const char*)name->chars);
#else
      return ::putenv((char*)name->chars);
#endif // HAVE_ISOCPP
   }
#endif
   return 0;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_putenv(yacmemptr _args) { return APIC_putenv(_args.mem[0].o); }
#endif // TKS_ALLOWINTFLOATMIXUP



sBool tks_debug_called = 0;

static void YAC_CALL APIC__Debug(void) {
#ifdef TKS_DCON
   Dprintf("[...] _Debug() called.\n");

   tks_debug_called = 1;

#ifdef DX_TRACEFUNCTIONCALLS
   if(tkscript->configuration.debug_level>10)
   {
      // xxx TKS_MT: synchronized print all contexts
      tkscript->printAllFunctionCallStacks();
   }
#endif

   ObjectPool::Print();

   // debug: force hard crash
   //*((sUI*)0) = 0xdeadbeef;

#endif // TKS_DCON
}

static void YAC_CALL APIC__DebugLoop(void) {
#ifdef TKS_DCON
   Dprintf("[...] _DebugLoop() called.\n");

#endif // TKS_DCON

   ::exit(20);
   ////while(1);
}



static void YAC_CALL APIC__DebugOC(void) {
#ifdef YAC_OBJECT_COUNTER
   Dprintf("[dbg]       Object::object_counter = %i\n", (YAC_Object::object_counter-TKS_NUM_STATIC_OBJECTS));
#endif // YAC_OBJECT_COUNTER

#ifdef TKS_STRING_OBJECTCOUNTER
   Dprintf("[dbg]         (%d strings)\n", YAC_String::string_object_counter);
#endif // TKS_STRING_OBJECTCOUNTER

#ifdef TKS_SCI_OBJECTCOUNTER
   Dprintf("[dbg]         (%d script class instances)\n", TKS_ScriptClassInstance::object_counter);
#endif // TKS_SCI_OBJECTCOUNTER

#ifdef YAC_GLOBAL_NEWDELETE
   Dprintf("[dbg] yac_global_newdelete_counter = %i\n", yac_global_newdelete_counter);
#endif //YAC_GLOBAL_NEWDELETE

#ifdef TKS_CO_OBJECTCOUNTER
   Dprintf("[dbg]   CachedObject::object_counter = %d\n", TKS_CachedObject::object_counter);
   Dprintf("[dbg]  CachedObject::refname_counter = %d\n", TKS_CachedObject::refname_counter);
   Dprintf("[dbg] CachedObject::copyname_counter = %d\n", TKS_CachedObject::copyname_counter);
#endif // TKS_CO_OBJECTCOUNTER

   ObjectPool::Print();

#ifdef YAC_TRACK_CHARALLOC
   Dprintf("[dbg]    peak_char_size = %d\n", yac_string_peak_char_size);
   Dprintf("[dbg]   total_char_size = %d\n", yac_string_total_char_size);
#endif //YAC_TRACK_CHARALLOC
}




/* @function mathPowerf,float x,float y:float
Return x raised to the power of y.

This is different to pow() in regard to negative 'x': mathPowerf() returns (sign(x) * pow(abs(x), y)).
*/
static sF32 YAC_CALL APIC_mathPowerf(sF32 _x, sF32 _y) {
   sF32 r;
   if(_y != 0.0f)
   {
      if(_x < 0.0f)
      {
         r = (sF32)( -exp(_y*log(-_x)) );
      }
      else if(_x > 0.0f)
      {
         r = (sF32)( exp(_y*log(_x)) );
      }
      else
      {
         r = 0.0f;
      }
   }
   else
   {
      r = 1.0f;
   }
   return Dyac_denorm_32(r);
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathPowerf(yacmemptr _args) { return APIC_mathPowerf(_args.mem[0].f32, _args.mem[1].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP



/* @function mathPowerfC,float x,float y:float
Return x raised to the power of y (pow())
*/
static sF32 YAC_CALL APIC_pow(sF32 _x, sF32 _y) {
   return powf(_x, _y);
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_pow(yacmemptr _args) { return APIC_pow(_args.mem[0].f32, _args.mem[1].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function mathMaxf,float x,float y:float
Return bigger value
*/
static sF32 YAC_CALL APIC_mathMaxf(sF32 _x, sF32 _y) {
   return (_x > _y)? _x : _y;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathMaxf(yacmemptr _args) { return APIC_mathMaxf(_args.mem[0].f32, _args.mem[1].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function mathMinf,float x,float y:float
Return smaller value
*/
static sF32 YAC_CALL APIC_mathMinf(sF32 _x, sF32 _y) {
   return (_x < _y)? _x : _y;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathMinf(yacmemptr _args) { return APIC_mathMinf(_args.mem[0].f32, _args.mem[1].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function mathAbsMaxf,float x,float y:float
Return absolutely bigger value
*/
static sF32 YAC_CALL APIC_mathAbsMaxf(sF32 _x, sF32 _y) {
   return ( ( (_x<0)?-_x:_x)>((_y<0)?-_y:_y)?_x:_y );
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathAbsMaxf(yacmemptr _args) { return APIC_mathAbsMaxf(_args.mem[0].f32, _args.mem[1].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function mathAbsMinf,float x,float y:float
Return absolutely smaller value
*/
static sF32 YAC_CALL APIC_mathAbsMinf(sF32 _x, sF32 _y) {
   return ( ((_x<0)?-_x:_x)<((_y<0)?-_y:_y)?_x:_y );
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathAbsMinf(yacmemptr _args) { return APIC_mathAbsMinf(_args.mem[0].f32, _args.mem[1].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP


/* @function mathPoweri,int x,int y:int
Return x raised to the power of y
*/
static sSI YAC_CALL APIC_mathPoweri(sSI _x, sSI _y) {
   return (sSI)( exp(_y*log((sF32)_x)) );
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_mathPoweri(yacmemptr _args) { return APIC_mathPoweri(_args.mem[0].si, _args.mem[1].si); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function mathMaxi,int x,int y:int
Return bigger value
*/
static sSI YAC_CALL APIC_mathMaxi(sSI _x, sSI _y) {
   return (_x>_y)? _x : _y;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_mathMaxi(yacmemptr _args) { return APIC_mathMaxi(_args.mem[0].si, _args.mem[1].si); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function mathMini,int x,int y:int
Return smaller value
*/
static sSI YAC_CALL APIC_mathMini(sSI _x, sSI _y) {
   return (_x<_y)? _x : _y;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_mathMini(yacmemptr _args) { return APIC_mathMini(_args.mem[0].si, _args.mem[1].si); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function mathAbsMini,int x,int y:int
Return absolutely bigger value
*/
static sSI YAC_CALL APIC_mathAbsMaxi(sSI _x, sSI _y) {
   return ( ((_x<0)?-_x:_x)>((_y<0)?-_y:_y)?_x:_y );
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_mathAbsMaxi(yacmemptr _args) { return APIC_mathAbsMaxi(_args.mem[0].si, _args.mem[1].si); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function mathAbsMini,int x,int y:int
Return absolutely smaller value
*/
static sSI YAC_CALL APIC_mathAbsMini(sSI _x, sSI _y) {
   return ( ((_x<0)?-_x:_x)<((_y<0)?-_y:_y)?_x:_y );
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_mathAbsMini(yacmemptr _args) { return APIC_mathAbsMini(_args.mem[0].si, _args.mem[1].si); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function floor,float f:float
Round to next smaller integer
*/
static sF32 YAC_CALL APIC_floor(sF32 _v) {
   return (sF32)floor((double)_v);
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_floor(yacmemptr _args) { return APIC_floor(_args.mem[0].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function ceil,float f:float
Round to next bigger integer
*/
static sF32 YAC_CALL APIC_ceil(sF32 _v) {
   return (sF32)ceil((double)_v);
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_ceil(yacmemptr _args) { return APIC_ceil(_args.mem[0].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function mathLerpf,float a,float b,float t:float
Return linear interpolation of values a and b according to t (0..1) (a + (b-a)*t)
*/
static sF32 YAC_CALL APIC_mathLerpf(sF32 _a, sF32 _b, sF32 _t) {
   return _a + (_b - _a) * _t;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathLerpf(yacmemptr _args) { return APIC_mathLerpf(_args.mem[0].f32, _args.mem[1].f32, _args.mem[2].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP


/* @function mathSerpf,float a,float b,float t:float
Return sine interpolation of values a and b according to t (0..1)
*/
static sF32 YAC_CALL APIC_mathSerpf(sF32 _a, sF32 _b, sF32 _t) {
   return _a + (_b - _a) * sinf(_t * sF32(sM_PI_2));
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathSerpf(yacmemptr _args) { return APIC_mathSerpf(_args.mem[0].f32, _args.mem[1].f32, _args.mem[2].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP


/* @function mathCerpf,float a,float b,float t:float
Return cosine interpolation of values a and b according to t (0..1)
*/
static sF32 YAC_CALL APIC_mathCerpf(sF32 _a, sF32 _b, sF32 _t) {
   return _a + (_b - _a) * (1.0f - cosf(_t * sF32(sM_PI_2)));
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathCerpf(yacmemptr _args) { return APIC_mathCerpf(_args.mem[0].f32, _args.mem[1].f32, _args.mem[2].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP



/* @function mathLerpCyclicf,float x,float y,float c,float t:float
Cyclic interpolation

@arg x
@arg y
@arg c Cycle interval (e.g. 2PI or 360)
@return interpolated value
*/
static sF32 YAC_CALL APIC_mathLerpCyclicf(sF32 _x, sF32 _y, sF32 _c, sF32 _t) {
   if(_c > 0.0f)
   {
#define Dfltmod(a,b) ((a) - (b) * floorf((a) / (b)))
      _x = Dfltmod(_x, _c);
      _y = Dfltmod(_y, _c);
      if(_y > _x)
      {
         sF32 d = (_y - _x);
         sF32 ymc = (_y - _c);
         sF32 dc = (ymc - _x);
         if(sABS(dc) < sABS(d))
         {
            sF32 r = (_x + (ymc - _x) * _t);
            if(r < 0.0f)
               r += _c;
            return r;
         }
         else
         {
            sF32 r = _x + (_y - _x) * _t;
            return r;
         }
      }
      else
      {
         sF32 d = (_y - _x);
         sF32 xmc = (_x - _c);
         sF32 dc = (_y - xmc);
         if(sABS(dc) < sABS(d))
         {
            sF32 r = (xmc + (_y - xmc) * _t);
            if(r < 0.0f)
               r += _c;
            return r;
         }
         else
         {
            sF32 r = _x + (_y - _x) * _t;
            return r;
         }
      }
   }
   // invalid cycle interval, fall back to linear interpolation
   return APIC_mathLerpf(_x, _y, _t);
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathLerpCyclicf(yacmemptr _args) { return APIC_mathLerpCyclicf(_args.mem[0].f32, _args.mem[1].f32, _args.mem[2].f32, _args.mem[3].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP



/* @function mathNextCyclicf,float c,float n,float cycle:float
Return next 'n' (shortest distance between last 'c' and next 'n' with cyclic period = 'cycle', e.g. 360.0 or "PI)
@arg c current value
@arg n next value
@arg cycle Period len, e.g. 360.0 or 2PI
*/
static sF32 YAC_CALL APIC_mathNextCyclicf(sF32 _c, sF32 _n, sF32 _cycle) {
   sF32 dist = sABS(_n - _c);
   if(dist >= (_cycle * 0.5f))
   {
      if(_n > _c)
         return _n - _cycle;
      else
         return _n + _cycle;
   }
   else
      return _n;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathNextCyclicf(yacmemptr _args) { return APIC_mathNextCyclicf(_args.mem[0].f32, _args.mem[1].f32, _args.mem[2].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function mathSmoothStepf,float a,float b,float s:float
Return smooth-step interpolation of values a and b according to s (0..1) (a*(1-t)+b*t, with t=(s*s*(3-2*s)) )
*/
static sF32 YAC_CALL APIC_mathSmoothStepf(sF32 _a, sF32 _b, sF32 _s) {
   sF32 t = _s * _s * (3.0f - 2.0f * _s);
   return _a + (_b - _a) * t;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathSmoothStepf(yacmemptr _args) { return APIC_mathSmoothStepf(_args.mem[0].f32, _args.mem[1].f32, _args.mem[2].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP


/* @function mathSmoothStepNf,float a,float b,float s,int n:float
Return (higher-order) smooth-step interpolation of values a and b. n must be in the range (0..6).
*/
static sF32 YAC_CALL APIC_mathSmoothStepNf(sF32 _a, sF32 _b, sF32 _s, sSI _n) {
   sF32 t;
   switch(_n & 7)
   {
      default:
      case 0:
         t = _s;
         break;

      case 1:
         t = _s * _s * (3.0f - 2.0f * _s);
         break;

      case 2:
      {
         sF32 s2 = _s * _s;
         sF32 s3 = s2 * _s;
         t = s3 * (6.0f*s2 - 15.0f*_s + 10.0f);
      }
      break;

      case 3:
      {
         sF32 s2 = _s * _s;
         sF32 s3 = s2 * _s;
         sF32 s4 = s2 * s2;
         t = s4 * (-20.0f*s3 + 70.0f*s2 - 84.0f*_s + 35.0f);
      }
      break;

      case 4:
      {
         sF32 s2 = _s * _s;
         sF32 s3 = s2 * _s;
         sF32 s4 = s2 * s2;
         sF32 s5 = s2 * s3;
         t = s5 * (70.0f*s4 - 315.0f*s3 + 540.0f*s2 - 420.0f*_s + 126.0f);
      }
      break;

      case 5:
      {
         sF32 s2 = _s * _s;
         sF32 s3 = s2 * _s;
         sF32 s4 = s2 * s2;
         sF32 s5 = s2 * s3;
         sF32 s6 = s5 * _s;
         t = s6 * (-252.0f*s5 + 1386.0f*s4 - 3080.0f*s3 + 3465.0f*s2 - 1980.0f*_s + 462.0f);
      }
      break;

      case 6:
      case 7: // (todo)
      {
         sF32 s2 = _s * _s;
         sF32 s3 = s2 * _s;
         sF32 s4 = s2 * s2;
         sF32 s5 = s2 * s3;
         sF32 s6 = s5 * _s;
         sF32 s7 = s4 * s3;
         t = s7 * (924.0f*s6 - 6006.0f*s5 + 16380.0f*s4 - 24024.0f*s3 + 20020.0f*s2 - 9009.0f*_s + 1716.0f);
      }
      break;
   }

   return _a + (_b - _a) * t;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathSmoothStepNf(yacmemptr _args) { return APIC_mathSmoothStepNf(_args.mem[0].f32, _args.mem[1].f32, _args.mem[2].f32, _args.mem[3].s32); }
#endif // TKS_ALLOWINTFLOATMIXUP



/* @function mathClampf,float a,float b,float t:float
Clamp value to min/max range
*/
static sF32 YAC_CALL APIC_mathClampf(sF32 _a, sF32 _b, sF32 _c) {
   if(_a < _b)
   {
      return _b;
   }
   else if(_a > _c)
   {
      return _c;
   }
   else
   {
      return _a;
   }
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathClampf(yacmemptr _args) { return APIC_mathClampf(_args.mem[0].f32, _args.mem[1].f32, _args.mem[2].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP



/* @function mathClampi,int a,int b,int c:int
Clamp value to min/max range
*/
static sSI YAC_CALL APIC_mathClampi(sSI _a, sSI _b, sSI _c) {
   if(_a < _b)
   {
      return _b;
   }
   else if(_a > _c)
   {
      return _c;
   }
   else
   {
      return _a;
   }
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_mathClampi(yacmemptr _args) { return APIC_mathClampi(_args.mem[0].si, _args.mem[1].si, _args.mem[2].si); }
#endif // TKS_ALLOWINTFLOATMIXUP



/* @function mathWrapf,float a,float b,float t:float
Wrap value around min/max range
*/
static sF32 YAC_CALL APIC_mathWrapf(sF32 _a, sF32 _b, sF32 _c) {
   sF32 b, c;
   if(_b < _c)
   {
      b = _b;
      c = _c;
   }
   else
   {
      b = _c;
      c = _b;
   }
   sF32 r = (c - b);
   if(0.0f != r)
   {
      if(_a < b)
      {
         sF32 d = b - _a;
         if(d > r)
            d = d - r * floorf(d / r);
         if(d <= YAC_FLT_EPSILON)
            return b;
         return c - d;
      }
      else
      {
         sF32 d = _a - b;
         if(d >= r)
            d = d - r * floorf(d / r);
         return b + d;
      }
   }
   // b == c
   return _b;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathWrapf(yacmemptr _args) { return APIC_mathWrapf(_args.mem[0].f32, _args.mem[1].f32, _args.mem[2].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP



/* @function mathWrapi,int a,int b,int t:int
Wrap value around min/max range
*/
static sSI YAC_CALL APIC_mathWrapi(sSI _a, sSI _b, sSI _c) {
   sSI b, c;
   if(_b < _c)
   {
      b = _b;
      c = _c;
   }
   else
   {
      b = _c;
      c = _b;
   }
   sSI r = (c - b);
   if(0 != r)
   {
      if(_a < b)
      {
         sSI d = b - _a;
         if(d >= r)
            d = d % r;
         if(0 == d)
            return b;
         return c - d;
      }
      else
      {
         sSI d = _a - b;
         if(d >= r)
            d = d % r;
         return b + d;
      }
   }
   // b == c
   return _b;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_mathWrapi(yacmemptr _args) { return APIC_mathWrapi(_args.mem[0].si, _args.mem[1].si, _args.mem[2].si); }
#endif // TKS_ALLOWINTFLOATMIXUP


/* @function mathFoldf,float a,float b,float t:float
Fold value into min/max range
*/
static sF32 YAC_CALL APIC_mathFoldf(sF32 _a, sF32 _b, sF32 _c) {
   sF32 b, c;
   if(_b < _c)
   {
      b = _b;
      c = _c;
   }
   else
   {
      b = _c;
      c = _b;
   }
   sF32 r = (c - b);
   if(0.0f != r)
   {
      if(_a < b)
      {
         sF32 d = b - _a;
         if(d < r)
         {
            return b + d;
         }
         else
         {
            const sF32 dr = d / r;
            d = d - r * floorf(dr);
            const sSI num = sSI(dr);
            if(num & 1)
               return c - d;
            return b + d;
         }
      }
      else if(_a >= c)
      {
         sF32 d = _a - c;
         if(d < r)
         {
            return c - d;
         }
         else
         {
            const sF32 dr = d / r;
            d = d - r * floorf(dr);
            const sSI num = sSI(dr);
            if(num & 1)
               return b + d;
            return c - d;
         }
      }
      return _a;
   }
   // b == c
   return b;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathFoldf(yacmemptr _args) { return APIC_mathFoldf(_args.mem[0].f32, _args.mem[1].f32, _args.mem[2].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP


/* @function mathFoldi,int a,int b,int t:int
Fold value into min/max range
*/
static sSI YAC_CALL APIC_mathFoldi(sSI _a, sSI _b, sSI _c) {
   sSI b, c;
   if(_b < _c)
   {
      b = _b;
      c = _c;
   }
   else
   {
      b = _c;
      c = _b;
   }
   sSI r = (_c - b);
   if(0 != r)
   {
      if(_a < b)
      {
         sSI d = b - _a;
         if(d < r)
         {
            return b + d;
         }
         else
         {
            const sSI num = (d / r);
            d = d % r;
            if(num & 1)
               return c - d;
            return b + d;
         }
      }
      else if(_a >= c)
      {
         sSI d = _a - c;
         if(d < r)
         {
            return c - d;
         }
         else
         {
            const sSI num = (d / r);
            d = d % r;
            if(num & 1)
               return b + d;
            return c - d;
         }
      }
      return _a;
   }
   // b == c
   return b;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_mathFoldi(yacmemptr _args) { return APIC_mathFoldi(_args.mem[0].si, _args.mem[1].si, _args.mem[2].si); }
#endif // TKS_ALLOWINTFLOATMIXUP





/* @function mathDistancePointPlane2d,float px,float py,float qx,float qy,float nx,float ny:float
Return distance of point p to infinite plane.


q is a point on the plane and n is the plane normal.

@arg px a point
@arg py a point
@arg qx a point on the plane
@arg qy a point on the plane
@arg nx the plane normal
@arg ny the plane normal
@return distance of point to plane
*/

sF32 YAC_CALL APIC_mathDistancePointPlane2d(sF32 px, sF32 py, sF32 qx, sF32 qy, sF32 nx, sF32 ny) {
   //
   //
   sF32 vx, vy;
   vx = px - qx;
   vy = py - qy;
   sF32 vdotn = vx * nx + vy * ny;
   sF32 nlen = sqrtf(nx*nx + ny*ny);
   //return (v.Dot(*n)/n->Abs());
   return vdotn / nlen;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathDistancePointPlane2d(yacmemptr _args) {
   return APIC_mathDistancePointPlane2d(
      _args.mem[0].f32, _args.mem[1].f32,
      _args.mem[2].f32, _args.mem[3].f32,
      _args.mem[4].f32, _args.mem[5].f32
      );
}
#endif // TKS_ALLOWINTFLOATMIXUP


/* @function mathGCD,int x,int y:int
Return greatest common divisor via Euclidean algorithm
*/
static sSI YAC_CALL APIC_mathGCD(sSI _x, sSI _y) {
   // Euclidean algorithm
   if((_x > 0) && (_y > 0))
   {
      while(_x != _y)
      {
         if(_x > _y)
            _x = (_x - _y);
         else
            _y = (_y - _x);
      }
   }
   return _x;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_mathGCD(yacmemptr _args) { return APIC_mathGCD(_args.mem[0].si, _args.mem[1].si); }
#endif // TKS_ALLOWINTFLOATMIXUP


static sF32 YAC_CALL APIC_mathLogLinExpf(sF32 _f, sF32 _c) {
   // c: <0: log
   //     0: lin
   //    >0: exp
   yacmem uSign;
   uSign.f32 = _f;
   yacmem u;
   u.f32 = _f;
   u.u32 &= 0x7fffFFFFu;
   u.f32 = powf(u.f32, powf(2.0f, _c));
   u.u32 |= uSign.u32 & 0x80000000u;
   return u.f32;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathLogLinExpf(yacmemptr _args) { return APIC_mathLogLinExpf(_args.mem[0].f32, _args.mem[1].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP


static sF32 YAC_CALL APIC_mathAtan2f(sF32 _x, sF32 _y) {
   return atan2f(_x, _y);
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathAtan2f(yacmemptr _args) { return APIC_mathAtan2f(_args.mem[0].f32, _args.mem[1].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP


static sF32 YAC_CALL APIC_mathLogf(sF32 _x) {
   // portable approximation of logf()
   //   (note) based on public domain ln() code by Lingdong Huang <https://gist.github.com/LingDong-/7e4c4cae5cbbc44400a05fba65f06f23>
   //   (note) max error in range 0.0000000001 .. 100 is 6.19888e-05
   union {
      float f;
      sU32 ui;
   } bx;
   bx.f = _x;
   const sU32 ex = bx.ui >> 23;
   const sS32 t = sS32(ex) - sS32(127);
   // const sU32 s = (t < 0) ? (-t) : t;
   bx.ui = 1065353216u | (bx.ui & 8388607u);
   return
#if 0
      /* less accurate */
      -1.49278f + (2.11263f + (-0.729104f + 0.10969f*bx.f)*bx.f)*bx.f
#else
      /* OR more accurate */
      -1.7417939f + (2.8212026f + (-1.4699568f + (0.44717955f - 0.056570851f * bx.f)*bx.f)*bx.f)*bx.f
#endif
      + 0.6931471806f * t;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sF32 YAC_CALL _APIC_mathLogf(yacmemptr _args) { return APIC_mathLogf(_args.mem[0].f32); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function system,String cmd
Invoke system command

@arg cmd Command line
*/
static sSI YAC_CALL APIC_system(YAC_Object *_s) {
#ifdef DX_SYSEXEC
   if(YAC_BCHK(_s, YAC_CLID_STRING))
   {
      YAC_CAST_STRING(s, _s);
      const char *str = (const char*)s->chars;
      if(tkscript->yacGetDebugLevel())
      {
         Dprintf("[...] system(str=%s).\n", str);
      }
      int r = ::system(str);
#ifndef YAC_VC
      if(r >= 0)
      {
         r = WEXITSTATUS(r);
      }
#endif // YAC_VC
      return r;
   }
#endif
   return -1;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_system(yacmemptr _args) { return APIC_system(_args.mem[0].o); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function psystem,String cmd,char access,String buf:int
Invoke system command

@arg cmd Command line
@arg access r to read result to buf, w to read input from buf
@arg buf Input resp. output buffer
@return Number of bytes read resp. written
*/
static sSI YAC_CALL APIC_psystem(YAC_Object *_p, sSI _t, YAC_Object *_s) {
   sSI r=0;
#ifdef DX_SYSEXEC
   if(YAC_BCHK(_p, YAC_CLID_STRING))
   {
      if(YAC_BCHK(_s, YAC_CLID_STRING))
      {
         YAC_CAST_STRING(p, _p);
         YAC_CAST_STRING(s, _s);
         switch(_t)
         {
         case 'r':
            {
               FILE *f;

// #define Dpipeprintf if(0);else yac_host->printf
#define Dpipeprintf if(1);else yac_host->printf

               Dpipeprintf("[trc] psystem: call popen\n");
#ifdef YAC_VC
               f = ::_popen((const char*)p->chars, "rb");
#else
               f = ::popen((const char*)p->chars, "r");
#endif
               Dpipeprintf("[trc] psystem: popen returned f=%p\n", f);
               int fe = ferror(f);
               Dpipeprintf("[trc] psystem: ferror returned fe=%d\n", fe);
               if(NULL != f)
               {
                  Dpipeprintf("[trc] psystem: call fread chars=%p buflen=%u\n", s->chars, s->buflen);
// #ifndef YAC_WIN32
#if 1
                  r = (sSI)::fread(s->chars, 1, s->buflen, f);
                  Dpipeprintf("[trc] psystem: fread returned r=%d\n", r);
                  fe = ferror(f);
                  if( ((sUI)r+1) < s->buflen )
                  {
                     s->chars[r] = 0;
                     s->length = r + 1;
                  }
#elif 0
                  char buf[128];
                  while(::fgets(buf, 128, f))
                  {
                     Dpipeprintf("[trc] psystem: read from pipe: %s", buf);
                     s->append(buf);
                  }
#else
                  // (note) fread() on windows sometimes blocks indefinitely
                  sUI bufOff = 0u;
                  sUI numSleeps = 0u;
                  // while(!feof(f))
                  int fd = _fileno(f);
                  for(;;)
                  {
                     struct _stat fst;
                     if(0 == ::_fstat(fd, &fst))
                     {
                        Dpipeprintf("[trc] psystem: [%u] fst.st_size=%d feof()=%d\n", numSleeps, fst.st_size, feof(f));
                        if(fst.st_size > 0)
                        {
                           numSleeps = 0u;
                           for(int i = 0; i< fst.st_size; i++)
                           {
                              sU8 t;
                              r = (sSI)::fread((char*)&t, 1, 1, f);
                              if(1 == r)
                              {
                                 if((bufOff+1u) < s->buflen)
                                 {
                                    s->chars[bufOff++] = t;
                                 }
                              }
                           }
                        }
                        else if(feof(f))
                        {
                           break;
                        }
                        else
                        {
                           tkscript->sleep(100);
                           numSleeps++;
                           if(100 == numSleeps)
                           {
                              Dpipeprintf("[---] psystem: timeout while reading from pipe (%u ms)\n", numSleeps);
                              break;
                           }
                        }
                     }
                     else
                     {
                        Dprintf("[---] psystem: fstat() failed\n");
                        break;
                     }
                  }
                  s->chars[bufOff++] = 0u;
                  s->length = bufOff;
                  s->key = YAC_LOSTKEY;
                  Dpipeprintf("[trc] psystem: read %u bytes from pipe\n", bufOff);
#endif
                  //////s->fixLength();
                  Dpipeprintf("[trc] psystem: call pclose\n");
#ifdef YAC_VC
                  ::_pclose(f);
#else
                  ::pclose(f);
#endif
                  Dpipeprintf("[trc] psystem: pclose returned\n");
               }
               if( (0 != fe) || (NULL == f) )
               {
                  Dprintf("[---] psystem(\"%s\", \'r\', ..) failed with error code %i.\n",
                          (char*)p->chars,
                          fe
                          );
                  s->empty();
               }
            }
            break;

         case 'w':
            {
               FILE *f;
#ifdef YAC_VC
               f = ::_popen((const char*)p->chars, "wb");
#else
               f = ::popen((const char*)p->chars, "wb");
#endif
               int fe = ferror(f);
               if(NULL != f)
               {
                  r = (sSI)::fwrite(s->chars, 1, s->length, f);
                  fe = ferror(f);
                  ::fflush(f);
#ifdef YAC_VC
                  ::_pclose(f);
#else
                  ::pclose(f);
#endif
               }
               if( (0 != fe) || (NULL == f))
               {
                  Dprintf("[---] psystem(\"%s\", \'w\', ..) failed with error code %i.\n",
                     (char*)p->chars,
                     fe);
               }
            }
            break;
         }
      }
   }
#endif // DX_SYSEXEC
   return r;
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_psystem(yacmemptr _args) { return APIC_psystem(_args.mem[0].o, _args.mem[1].si, _args.mem[2].o); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function lcchar,char c:char
Convert character to lower case
*/
static sSI YAC_CALL APIC_lcchar(sSI _c) {
   return ( ((_c>='A')&&(_c<='Z'))?(_c|32):_c );
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_lcchar(yacmemptr _args) { return APIC_lcchar(_args.mem[0].si); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function ucchar,char c:char
Convert character to upper case
*/
static sSI YAC_CALL APIC_ucchar(sSI _c) {
   return ( ((_c>='a')&&(_c<='z'))?(_c&~32):_c );
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static sSI YAC_CALL _APIC_ucchar(yacmemptr _args) { return APIC_ucchar(_args.mem[0].si); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function srand,int seed
Set random seed
*/
static void YAC_CALL APIC_srand(sSI _c) {
   ::srand((sUI)_c);
}
#ifndef TKS_ALLOWINTFLOATMIXUP
static void YAC_CALL _APIC_srand(yacmemptr _args) { APIC_srand(_args.mem[0].si); }
#endif // TKS_ALLOWINTFLOATMIXUP




/* @function milliSeconds:int
Return milliseconds since system start
*/
static sSI YAC_CALL APIC_milliSeconds(void) {
   return (sSI)tkscript->getMilliSeconds();
}


/* @function milliSecondsDouble,Double d
For profiling purposes only, cannot be used to measure long time spans.

@arg d Preallocated Double object that will be used to store the timestamp
*/
static void YAC_CALL APIC_milliSecondsDouble(yacmemptr _args) {
   if(YAC_Is_Double(_args.mem[0].o))
   {
      YAC_CAST_ARG(YAC_Double, d, _args.mem[0].o);
      d->value = TKS_ScriptEngine::GetMilliSecondsDouble();
   }
}




//------------------------------------------------------------------------------------ classes
sBool TKS_ScriptEngine::registerBuiltinClasses(void) {
   YAC_String sts;
   YAC_Object *templ;

   // ---- init typecast map ----
   sSI i;
   for(i=0; i<YAC_MAX_CLASSES; i++)
   {
      cpp_typecast_map[i][i]=cpp_typecast_map[i][YAC_CLID_OBJECT]=1;
   }

   templ=new YAC_Object();       templ->class_ID=YAC_CLID_OBJECT;
   if(!registerClass(templ, YAC_CLASSTYPE_NOINST, 0)) return 0; // 2=restricted class, no instances allowed
   yac_null=templ;
   yac_null->validation_tag=YAC_INVALID_TAG;

   // -------------------------------------------------------------------- YAC value objects
   templ=new _Boolean();          templ->class_ID=YAC_CLID_BOOLEAN;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Byte();             templ->class_ID=YAC_CLID_BYTE;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Short();           templ->class_ID=YAC_CLID_SHORT;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Integer();         templ->class_ID=YAC_CLID_INTEGER;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Long();             templ->class_ID=YAC_CLID_LONG;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _UnsignedByte();     templ->class_ID=YAC_CLID_UNSIGNEDBYTE;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _UnsignedShort();    templ->class_ID=YAC_CLID_UNSIGNEDSHORT;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _UnsignedInteger();  templ->class_ID=YAC_CLID_UNSIGNEDINTEGER;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _UnsignedLong();  templ->class_ID=YAC_CLID_UNSIGNEDLONG;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Float();           templ->class_ID=YAC_CLID_FLOAT;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Double();          templ->class_ID=YAC_CLID_DOUBLE;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _String();          templ->class_ID=YAC_CLID_STRING;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Event();            templ->class_ID=YAC_CLID_EVENT;
   if(!registerClass(templ, 1, 0)) return 0;
   cpp_typecast_map[YAC_CLID_EVENT][YAC_CLID_VALUE]=1;

   templ=new _Value();           templ->class_ID=YAC_CLID_VALUE;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _ListNode();        templ->class_ID=YAC_CLID_LISTNODE;
   if(!registerClass(templ, 1, 0)) return 0;
   cpp_typecast_map[YAC_CLID_LISTNODE][YAC_CLID_VALUE]=1;

   templ=new _TreeNode();        templ->class_ID=YAC_CLID_TREENODE;
   if(!registerClass(templ, 1, 0)) return 0;
   cpp_typecast_map[YAC_CLID_TREENODE][YAC_CLID_VALUE]=1;

   // -------------------------------------------------------------------- YAC containers
   templ=new _Class();           templ->class_ID=YAC_CLID_CLASS;
   ((TKS_ScriptClassInstance*)templ)->class_decl = &Class_ClassDecl;
   Class_ClassDecl.class_template = (TKS_ScriptClassInstance*)templ;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _List();            templ->class_ID=YAC_CLID_LIST;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _IntArray();        templ->class_ID=YAC_CLID_INTARRAY;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _FloatArray();       templ->class_ID=YAC_CLID_FLOATARRAY;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _StringArray();      templ->class_ID=YAC_CLID_STRINGARRAY;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _ObjectArray();      templ->class_ID=YAC_CLID_OBJECTARRAY;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _ClassArray();      templ->class_ID=YAC_CLID_CLASSARRAY;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _ValueArray();      templ->class_ID=YAC_CLID_VALUEARRAY;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _PointerArray();     templ->class_ID=YAC_CLID_POINTERARRAY;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _HashTable();       templ->class_ID=YAC_CLID_HASHTABLE;
   if(!registerClass(templ, 1, 0)) return 0;

   // -------------------------------------------------------------------- YAC streams
   templ=new _Stream();          templ->class_ID=YAC_CLID_STREAM;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _StdInStream();     templ->class_ID=YAC_CLID_STDINSTREAM;
   if(!registerClass(templ, 0, 0)) return 0;
   cpp_typecast_map[YAC_CLID_STDINSTREAM][YAC_CLID_STREAM]=1;

   templ=new _StdErrStream();    templ->class_ID=YAC_CLID_STDERRSTREAM;
   if(!registerClass(templ, 0, 0)) return 0;
   cpp_typecast_map[YAC_CLID_STDERRSTREAM][YAC_CLID_STREAM]=1;

   templ=new _StdOutStream();    templ->class_ID=YAC_CLID_STDOUTSTREAM;
   if(!registerClass(templ, 0, 0)) return 0;
   cpp_typecast_map[YAC_CLID_STDOUTSTREAM][YAC_CLID_STREAM]=1;

   templ=new _Buffer();          templ->class_ID=YAC_CLID_BUFFER;
   if(!registerClass(templ, 1, 0)) return 0;
   cpp_typecast_map[YAC_CLID_BUFFER][YAC_CLID_STREAM]=1;

   templ=new _File();            templ->class_ID=YAC_CLID_FILE;
   if(!registerClass(templ, 1, 0)) return 0;
   cpp_typecast_map[YAC_CLID_FILE][YAC_CLID_STREAM]=1;

   templ=new _PakFile();         templ->class_ID=YAC_CLID_PAKFILE;
   if(!registerClass(templ, 1, 0)) return 0;
   cpp_typecast_map[YAC_CLID_PAKFILE][YAC_CLID_STREAM]=1;

   DregisterprebufString(8);
   DregisterprebufString(16);
   DregisterprebufString(32);
   DregisterprebufString(64);
   DregisterprebufString(128);

   DregisterprebufIntArray(8);
   DregisterprebufIntArray(16);
   DregisterprebufIntArray(32);
   DregisterprebufIntArray(64);
   DregisterprebufIntArray(128);

   DregisterprebufFloatArray(8);
   DregisterprebufFloatArray(16);
   DregisterprebufFloatArray(32);
   DregisterprebufFloatArray(64);
   DregisterprebufFloatArray(128);

   // -------------------------------------------------------------------- TKS core classes
   templ=new _Envelope();        templ->class_ID=TKS_CLID_ENVELOPE;
   if(!registerClass(templ, 1, 0)) return 0;
   cpp_typecast_map[TKS_CLID_ENVELOPE][YAC_CLID_FLOATARRAY]=1;

   templ=new _Pool();           templ->class_ID=TKS_CLID_POOL;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Stack();           templ->class_ID=TKS_CLID_STACK;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Time();            templ->class_ID=TKS_CLID_TIME;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Variable();       templ->class_ID=TKS_CLID_VARIABLE;
   if(!registerClass(templ, 1, 0)) return 0;
   cpp_typecast_map[TKS_CLID_VARIABLE][YAC_CLID_VALUE]=1;

   templ=new _Function();        templ->class_ID=TKS_CLID_FUNCTION;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Script();          templ->class_ID=TKS_CLID_SCRIPT;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _StringIterator();  templ->class_ID=TKS_CLID_STRINGITERATOR;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _SharedBuffer();    templ->class_ID=TKS_CLID_SHAREDBUFFER;
   if(!registerClass(templ, 1, 0)) return 0;
   cpp_typecast_map[TKS_CLID_SHAREDBUFFER][YAC_CLID_BUFFER]=1;
   cpp_typecast_map[TKS_CLID_SHAREDBUFFER][YAC_CLID_STREAM]=1;

   templ=new _Exception();       templ->class_ID=TKS_CLID_EXCEPTION;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Thread();          templ->class_ID=TKS_CLID_THREAD;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Mutex();           templ->class_ID=TKS_CLID_MUTEX;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Condition();       templ->class_ID=TKS_CLID_CONDITION;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _Mailbox();         templ->class_ID=TKS_CLID_MAILBOX;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _DummyStream();     templ->class_ID=TKS_CLID_DUMMYSTREAM;
   if(!registerClass(templ, 1, 0)) return 0;
   cpp_typecast_map[TKS_CLID_DUMMYSTREAM][YAC_CLID_STREAM]=1;

   templ=new _Process();         templ->class_ID=TKS_CLID_PROCESS;
   if(!registerClass(templ, 1, 0)) return 0;
   cpp_typecast_map[TKS_CLID_PROCESS][YAC_CLID_STREAM]=1;

   // -------------------------------------------------------------------- "static" core
   templ=new _Configuration();   templ->class_ID=TKS_CLID_CONFIGURATION;
   if(!registerClass(templ, 0, 0)) return 0;
   sts.visit("configuration");
   global_instance_namespace.createObjectEntry(&sts, 1, templ, 0);

   templ=new _Language();        templ->class_ID=TKS_CLID_LANGUAGE;
   if(!registerClass(templ, 0, 0)) return 0;

   // -------------------------------------------------------------------- other
   templ=new _LFSR();            templ->class_ID=YAC_CLID_LFSR;
   if(!registerClass(templ, 1, 0)) return 0;

   templ=new _LFSR_NR();         templ->class_ID=YAC_CLID_LFSR_NR;
   if(!registerClass(templ, 1, 0)) return 0;

   // -------------------------------------------------------------------- functions

   static const sUI arg_i    [] = {1,};
   static const sUI arg_ii   [] = {1,1,};
   static const sUI arg_iii  [] = {1,1,1,};
   static const sUI arg_f    [] = {2,};
   static const sUI arg_ff   [] = {2,2,};
   static const sUI arg_fff  [] = {2,2,2,};
   static const sUI arg_fffi [] = {2,2,2,1,};
   static const sUI arg_4f   [] = {2,2,2,2,};
   static const sUI arg_6f   [] = {2,2,2,2,2,2,};
   static const sUI arg_o    [] = {3,};
   static const sUI arg_oio  [] = {3,1,3,};

   static const char *argt[] = {0,0,0,0,0,0,};

#ifdef TKS_ALLOWINTFLOATMIXUP
   // Pass int/float/Object arguments directly. Assumes that all args are passed on the stack (cdecl)
   //  (note) see autogen_PTN_FunctionECallY_switch.cpp
   //  (**DEPRECATED**)
   yacRegisterFunction((void*)APIC__Debug,            "_Debug",          0,0,   0,0,       argt,  8);
   yacRegisterFunction((void*)APIC__DebugLoop,        "_DebugLoop",      0,0,   0,0,       argt,  8);
   yacRegisterFunction((void*)APIC__DebugOC,          "_DebugOC",        0,0,   0,0,       argt,  8);
   yacRegisterFunction((void*)APIC_exit,              "exit",            0,0,   1,arg_i,   argt, 12);
   yacRegisterFunction((void*)APIC_getenv,            "getenv",          3,0,   1,arg_o,   argt, 19);
   yacRegisterFunction((void*)APIC_lcchar,            "lcchar",          1,0,   1,arg_i,   argt, 13);
   yacRegisterFunction((void*)APIC_mathPowerf,        "mathPowerf",      2,0,   2,arg_ff,  argt, 22);
   yacRegisterFunction((void*)APIC_pow,               "pow",             2,0,   2,arg_ff,  argt, 22);
   yacRegisterFunction((void*)APIC_mathPoweri,        "mathPoweri",      1,0,   2,arg_ii,  argt, 21);
   yacRegisterFunction((void*)APIC_mathMaxf,          "mathMaxf",        2,0,   2,arg_ff,  argt, 22);
   yacRegisterFunction((void*)APIC_mathMaxi,          "mathMaxi",        1,0,   2,arg_ii,  argt, 21);
   yacRegisterFunction((void*)APIC_mathMinf,          "mathMinf",        2,0,   2,arg_ff,  argt, 22);
   yacRegisterFunction((void*)APIC_mathMini,          "mathMini",        1,0,   2,arg_ii,  argt, 21);
   yacRegisterFunction((void*)APIC_mathAbsMaxf,       "mathAbsMaxf",     2,0,   2,arg_ff,  argt, 22);
   yacRegisterFunction((void*)APIC_mathAbsMaxi,       "mathAbsMaxi",     1,0,   2,arg_ii,  argt, 21);
   yacRegisterFunction((void*)APIC_mathAbsMinf,       "mathAbsMinf",     2,0,   2,arg_ff,  argt, 22);
   yacRegisterFunction((void*)APIC_mathAbsMini,       "mathAbsMini",     1,0,   2,arg_ii,  argt, 21);
   yacRegisterFunction((void*)APIC_mathLerpf,         "mathLerpf",       2,0,   3,arg_fff, argt, 58);
   yacRegisterFunction((void*)APIC_mathSerpf,         "mathSerpf",       2,0,   3,arg_fff, argt, 58);
   yacRegisterFunction((void*)APIC_mathCerpf,         "mathCerpf",       2,0,   3,arg_fff, argt, 58);
   yacRegisterFunction((void*)APIC_mathLerpCyclicf,   "mathLerpCyclicf", 2,0,   4,arg_4f,  argt, 70);
   yacRegisterFunction((void*)APIC_mathNextCyclicf,   "mathNextCyclicf", 2,0,   3,arg_fff, argt, 58);
   yacRegisterFunction((void*)APIC_mathSmoothStepf,   "mathSmoothStepf", 2,0,   3,arg_fff, argt, 58);
   yacRegisterFunction((void*)APIC_mathSmoothStepNf,  "mathSmoothStepNf",2,0,   4,arg_fffi,argt, 70);
   yacRegisterFunction((void*)APIC_mathGCD,           "mathGCD",         1,0,   2,arg_ii,  argt, 21);
   yacRegisterFunction((void*)APIC_mathLogLinExpf,    "mathLogLinExpf",  2,0,   2,arg_ff,  argt, 22);
   yacRegisterFunction((void*)APIC_mathAtan2f,        "mathAtan2f",      2,0,   2,arg_ff,  argt, 22);
   yacRegisterFunction((void*)APIC_mathLogf,          "mathLogf",        2,0,   1,arg_f,   argt, 14);
   yacRegisterFunction((void*)APIC_floor,             "floor",           2,0,   1,arg_f,   argt, 14);
   yacRegisterFunction((void*)APIC_ceil,              "ceil",            2,0,   1,arg_f,   argt, 14);
   yacRegisterFunction((void*)APIC_milliSeconds,      "milliSeconds",    1,0,   0,0,       argt, 9);
   yacRegisterFunction((void*)APIC_psystem,           "psystem",         1,0,   3,arg_oio, argt, 57);
   yacRegisterFunction((void*)APIC_putenv,            "putenv",          1,0,   1,arg_o,   argt, 17);
   yacRegisterFunction((void*)APIC_srand,             "srand",           0,0,   1,arg_i,   argt, 12);
   yacRegisterFunction((void*)APIC_system,            "system",          1,0,   1,arg_o,   argt, 17);
   yacRegisterFunction((void*)APIC_ucchar,            "ucchar",          1,0,   1,arg_i,   argt, 13);
   yacRegisterFunction((void*)APIC_GetCurrentThread,  "GetCurrentThread", 3,0,   0,0,       argt, 11);

#else
   // Wrapped arglists, suitable for e.g. the fastcall/x86-64 calling convention
   //                         entrypoint             name            returntype args       argobjtypes/calltype (see PTN_FunctionECallY.cpp)
   yacRegisterFunction((void*)_APIC_exit,               "exit",              0,0, 1,arg_i,   argt, 0);
   yacRegisterFunction((void*)_APIC_getenv,             "getenv",            3,0, 1,arg_o,   argt, 3);
   yacRegisterFunction((void*)_APIC_putenv,             "putenv",            1,0, 1,arg_o,   argt, 1);
   yacRegisterFunction((void*) APIC__Debug,             "_Debug",            0,0, 0,0,       argt, 8);
   yacRegisterFunction((void*) APIC__DebugLoop,         "_DebugLoop",        0,0, 0,0,       argt, 8);
   yacRegisterFunction((void*) APIC__DebugOC,           "_DebugOC",          0,0, 0,0,       argt, 8);
   yacRegisterFunction((void*)_APIC_mathPowerf,         "mathPowerf",        2,0, 2,arg_ff,  argt, 2);
   yacRegisterFunction((void*)_APIC_pow,                "pow",               2,0, 2,arg_ff,  argt, 2);
   yacRegisterFunction((void*)_APIC_mathMaxf,           "mathMaxf",          2,0, 2,arg_ff,  argt, 2);
   yacRegisterFunction((void*)_APIC_mathMinf,           "mathMinf",          2,0, 2,arg_ff,  argt, 2);
   yacRegisterFunction((void*)_APIC_mathAbsMaxf,        "mathAbsMaxf",       2,0, 2,arg_ff,  argt, 2);
   yacRegisterFunction((void*)_APIC_mathAbsMinf,        "mathAbsMinf",       2,0, 2,arg_ff,  argt, 2);
   yacRegisterFunction((void*)_APIC_mathPoweri,         "mathPoweri",        1,0, 2,arg_ii,  argt, 1);
   yacRegisterFunction((void*)_APIC_mathMaxi,           "mathMaxi",          1,0, 2,arg_ii,  argt, 1);
   yacRegisterFunction((void*)_APIC_mathMini,           "mathMini",          1,0, 2,arg_ii,  argt, 1);
   yacRegisterFunction((void*)_APIC_mathAbsMaxi,        "mathAbsMaxi",       1,0, 2,arg_ii,  argt, 1);
   yacRegisterFunction((void*)_APIC_mathAbsMini,        "mathAbsMini",       1,0, 2,arg_ii,  argt, 1);
   yacRegisterFunction((void*)_APIC_mathClampf,         "mathClampf",        2,0, 3,arg_fff, argt, 2);
   yacRegisterFunction((void*)_APIC_mathClampi,         "mathClampi",        1,0, 3,arg_iii, argt, 1);
   yacRegisterFunction((void*)_APIC_mathWrapf,          "mathWrapf",         2,0, 3,arg_fff, argt, 2);
   yacRegisterFunction((void*)_APIC_mathWrapi,          "mathWrapi",         1,0, 3,arg_iii, argt, 1);
   yacRegisterFunction((void*)_APIC_mathFoldf,          "mathFoldf",         2,0, 3,arg_fff, argt, 2);
   yacRegisterFunction((void*)_APIC_mathFoldi,          "mathFoldi",         1,0, 3,arg_iii, argt, 1);
   yacRegisterFunction((void*)_APIC_mathLerpf,          "mathLerpf",         2,0, 3,arg_fff, argt, 2);
   yacRegisterFunction((void*)_APIC_mathSerpf,          "mathSerpf",         2,0, 3,arg_fff, argt, 2);
   yacRegisterFunction((void*)_APIC_mathCerpf,          "mathCerpf",         2,0, 3,arg_fff, argt, 2);
   yacRegisterFunction((void*)_APIC_mathLerpCyclicf,    "mathLerpCyclicf",   2,0, 4,arg_4f,  argt, 2);
   yacRegisterFunction((void*)_APIC_mathNextCyclicf,    "mathNextCyclicf",   2,0, 3,arg_fff, argt, 2);
   yacRegisterFunction((void*)_APIC_mathSmoothStepf,    "mathSmoothStepf",   2,0, 3,arg_fff, argt, 2);
   yacRegisterFunction((void*)_APIC_mathSmoothStepNf,   "mathSmoothStepNf",  2,0, 4,arg_fffi,argt, 2);
   yacRegisterFunction((void*)_APIC_mathDistancePointPlane2d,
                                                        "mathDistancePointPlane2d",
                                                                             2,0, 6,arg_6f,  argt, 2);
   yacRegisterFunction((void*)_APIC_mathGCD,            "mathGCD",           1,0, 2,arg_ii,  argt, 1);
   yacRegisterFunction((void*)_APIC_mathLogLinExpf,     "mathLogLinExpf",    2,0, 2,arg_ff,  argt, 2);
   yacRegisterFunction((void*)_APIC_mathAtan2f,         "mathAtan2f",        2,0, 2,arg_ff,  argt, 2);
   yacRegisterFunction((void*)_APIC_mathLogf,           "mathLogf",          2,0, 1,arg_f,   argt, 2);
   yacRegisterFunction((void*)_APIC_floor,              "floor",             2,0, 1,arg_f,   argt, 2);
   yacRegisterFunction((void*)_APIC_ceil,               "ceil",              2,0, 1,arg_f,   argt, 2);
   yacRegisterFunction((void*)_APIC_system,             "system",            1,0, 1,arg_o,   argt, 1);
   yacRegisterFunction((void*)_APIC_psystem,            "psystem",           1,0, 3,arg_oio, argt, 1);
   yacRegisterFunction((void*)_APIC_lcchar,             "lcchar",            1,0, 1,arg_i,   argt, 1);
   yacRegisterFunction((void*)_APIC_ucchar,             "ucchar",            1,0, 1,arg_i,   argt, 1);
   yacRegisterFunction((void*)_APIC_srand,              "srand",             0,0, 1,arg_i,   argt, 0);
   yacRegisterFunction((void*) APIC_milliSeconds,       "milliSeconds",      1,0, 0,0,       argt, 9);
   yacRegisterFunction((void*) APIC_milliSecondsDouble, "milliSecondsDouble",0,0, 1,arg_o,   argt, 0);
   yacRegisterFunction((void*) APIC_GetCurrentThread,   "GetCurrentThread",  3,0, 0,0,       argt, 11);
#endif //TKS_ALLOWINTFLOATMIXUP

   next_plugin_clid = TKS_CLID_NUMINTERNALS;

   ObjectPool::b_initialized = 1;

   return 1;
}


/* @endmodule core
*/
