#ifndef TKFILEUTILS_H__
#define TKFILEUTILS_H__


YG("fileutils")


/* @function tkfileutils_read_directory,String pathName:StringArray
  Read directory contents and return results as an array of strings.
  The string format is: <type>,<name>,<size>,<moddate>.
  <type> is one of "f" (regular file), or "d" (directory).

  @arg pathName Absolute or relative pathname
  @return Array of file record strings
*/
YF void YAC_CALL tkfileutils_read_directory (YAC_String *_pathname, YAC_Value *_r);


YF void YAC_CALL tkfileutils_getcwd (YAC_Value *_r);

YF sBool YAC_CALL tkfileutils_setcwd (YAC_String *_pathName);

YF sBool YAC_CALL tkfileutils_mkdir (YAC_String *_pathName);

YF sBool YAC_CALL tkfileutils_rm (YAC_String *_pathName);

YF sBool YAC_CALL tkfileutils_rename (YAC_String *_pathNameOld, YAC_String *_pathNameNew);


#endif // TKFILEUTILS_H__
