/// TKX_File.h
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKX_FILE_H__
#define __TKX_FILE_H__


class TKX_File {
public:
   enum __fileentrytypes {
      CFE_TYPE_UNKNOWN = 0, 
      CFE_TYPE_FILE,   // regular file
      CFE_TYPE_HTTP,   // n/a yet
      CFE_TYPE_HTTPS,  // n/a yet
      CFE_TYPE_FTP,    // n/a yet
      CFE_TYPE_SFTP,   // n/a yet
      CFE_TYPE_LIB,    // dynamically loaded script library in TKS_LIBRARY_PATH
      CFE_TYPE_LOCAL,  // never embed into pakfile, e.g. for config files etc.
      CFE_TYPE_LOCAL_HOME,  // never embed into pakfile, e.g. for config files in $HOME directory etc.

      CFE_NUMTYPES
   };

public:
    TKX_File   *next;
    sU8        *file_data;
    sUI         file_offset; // if <16 this identifies a non-FILE type (see TKX_PakFile::CFE_TYPE_xxx)
    sUI         file_size;
    YAC_String  logic_name;
    YAC_String  source_name;
    sU8         type;        // this field is not stored in pakfiles
    yacmemptr   tokenized_source; // see TKS_TokenizedScript::readTokens/writeTokens
    sBool       b_done;      // true 

public:
    TKX_File(void);
    ~TKX_File();

    sBool initLibFile (const char *_libName);

    void freeTokenizedSource(void);
};


#endif // __TKX_FILE_H__
