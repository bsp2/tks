/// TKS_StreamIO.h
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef __TKS_STREAMIO_H__
#define __TKS_STREAMIO_H__


extern YAC_String tks_local_pathname; // local dir in which to store persistent files.

extern void tks_init_streamio(void); // init local_pathname/base_pathname
extern void tks_free_streamio(void);

enum __openmodes {
    TKS_SIO_LOGIC=0,
    TKS_SIO_LOCAL=1
};

enum __accessmodes {
   TKS_IOS_IN    = 0,
   TKS_IOS_OUT   = 1,
   TKS_IOS_INOUT = 2
};


class TKS_StreamIO {
public:

  enum __streamopentypes {
    TKS_CLOSED=0,
    TKS_LOGIC_OPEN,
    TKS_LOCAL_OPEN,
    TKS_STDIN_OPEN,
    
    TKS_OPEN_PAD
  };
  
  enum __streamerrorcodes {
    ERRINVALIDSEEK=1,
    ERRIO,
    ERRCREATEFILE,
    ERROPENFILE,
    
    ERRPAD
  };

public:
    sSI    io_offset;    // current offset into resource
    sSI    io_size;      // resource size in bytes
    sUI    error_code;   // last error code
    sUI    open_mode;    // see __streamopentypes
    sU8   *read_buffer;
    FILE  *fhandle;

public:
          TKS_StreamIO (TKS_ScriptEngine *);
          TKS_StreamIO (void);
         ~TKS_StreamIO ();

    sBool openLogic  (YAC_String *_name);
    sBool openLogic  (char *_name);
    sBool openLocal  (char *_name, sSI _access);
    sBool openLocal  (YAC_String *_name, sSI _access);
    void  close      (void);
    void  closeLocal (void);
    void  closeLogic (void); 
/* 	void  closeStdIn (void); */
    // ---- negative results are error codes
    sSI   read       (unsigned char *_buf, sUI _num_bytes);
    sSI   readLogic  (unsigned char *_buf, sUI _num_bytes);
    sSI   readLocal  (unsigned char *_buf, sUI _num_bytes); 
/* 	sSI   readStdIn  (unsigned char *_buf, sUI _num_bytes); */
    sU8   readChar   (void);
    sU16  readWord   (void);
    sU32  readLong   (void);
    sSI   write      (unsigned char *_buf, sUI _num_bytes);
    sSI   writeLocal (unsigned char *_buf, sUI _num_bytes);
    /// ---- adjust read/write pointer to given offset
    /// ---- type=TKS_SEEK_BEG|TKS_SEEK_CUR|TKS_SEEK_END
    sBool seek       (sSI _offset, sUI _type=YAC_BEG);
    void  seekLocal  (sSI _offset, sUI _type=YAC_BEG);
    void  seekLogic  (sSI _offset, sUI _type=YAC_BEG);
    sBool isOpen     (void);
    sBool fail       (void);
    sBool eof        (void);
    sUI   getFileSize(void);
/* 	sBool openStdIn  (void); */
	void  flush      (void); /// flush I/O buffers

};


#endif // __TKS_STREAMIO_H__
