/// TKS_StreamIO.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///     10Jan2004 <bsp> added stdin support
///

#include <stdio.h> 
#include <stdlib.h>
#include "tks.h"
#ifdef YAC_WIN32
#include <windows.h>
// #include <direct.h>
#endif

#include "TKS_StreamIO.h"


typedef struct __tks_sio_builtin_en {
   char *name;
   sU32 *data;
} tks_sio_builtin_en;


#include "builtinbin.cpp"

static tks_sio_builtin_en tks_sio_builtin_tbl[]= {
   //{ "builtin:gfx/aon2-1920x8x8.png", bin_aon2font },
   { 0, 0 }
};

static sU32 *tks_sio_builtin_FindEntry(YAC_String *_name) {
   tks_sio_builtin_en*en = tks_sio_builtin_tbl;

   while(en->name)
   {
      if(_name->compare(en->name))
      {
         return en->data;
      }
      en++;
   }
   return 0;
}

//=====================================================================================
void tks_init_streamio(void) {
   YAC_String buf;
   buf.alloc(1024);

   // ---- find local_pathname (for persistent files like hiscore tables) ----
#ifdef TKS_PLUGIN
   tkscript->tks_local_pathname.copy("P/A/R/A/N/O/I/A");

#ifdef YAC_WIN32
   {
		DWORD ret = ::GetTempPath(1023, (char*)buf.chars);

		if(0 == ret)
		{
			::printf("[---] failed to query Win32 TempPath. hmmmm....\n");
		}
		else
		{
         buf.fixLength();
         buf.replace("\\", "/"); // breaks remove()
		}
   }
#endif

#ifdef YAC_POSIX
   {
      char *hp = getenv("HOME");
      buf.copy(hp);
      if(buf.isBlank()) tkscript->tks_local_pathname.copy("P/A/R/A/N/O/I/A");
   }
#endif

   if(!buf.endsWith("/"))
   {
		buf.append("/");
   }

   tkscript->tks_local_pathname.yacCopy(&buf);

	// --------test if tks subdir exists, create if necessary-------- XXXX removed

#else
   tkscript->tks_local_pathname.empty(); // allow access to whole filesystem (no prefix)
#endif // TKS_PLUGIN

   //printf("[...] local_pathname=\"%s\".\n", (char*)tks_local_pathname.chars);
}

//=====================================================================================
void tks_free_streamio(void) {
}

//=====================================================================================
TKS_StreamIO::TKS_StreamIO(TKS_ScriptEngine * /*_se*/) {
   io_offset   = 0;
   io_size     = 0;
   error_code  = 0;
   open_mode   = 0;
   read_buffer = NULL;
}

TKS_StreamIO::TKS_StreamIO(void) {
   io_offset   = 0;
   io_size     = 0;
   error_code  = 0;
   open_mode   = 0;
   read_buffer = NULL;
}

TKS_StreamIO::~TKS_StreamIO() {
   if(open_mode)
   {
      TKS_StreamIO::close();
   }
}

sBool TKS_StreamIO::openLogic(char *_name) {
   YAC_String s;
   s.visit(_name);
   return TKS_StreamIO::openLogic(&s);
}

sBool TKS_StreamIO::openLogic(YAC_String *_name) {
   if(_name->startsWith("builtin:"))
   {
      /** check builtin table */
      sU32 *bin_dat = tks_sio_builtin_FindEntry(_name);

      if(bin_dat)
      {
         read_buffer = (sU8*)(bin_dat + 1);
         io_offset   = 0;
         io_size     = *bin_dat;
         error_code  = 0;
         open_mode   = TKS_LOGIC_OPEN;
         return 1;
      }
      else  
		{ 
			read_buffer = NULL;
			io_offset   = 0;
			io_size     = 0;
			error_code  = ERROPENFILE;
			open_mode   = 0;
			return 0;
		}
   }

   TKX_File *tf = tkscript->tkx.findLogic(_name);

   ////Dprintf("xxx TKS_StreamIO::openLogic: name=\"%s\" tf=%08x\n", _name->chars, tf);

   if(NULL != tf)
   {
      ////if(tkscript->configuration.b_simulatevfs)
      if(NULL == tf->file_data)
      {
         /// Load from host file system
         if(tkscript->configuration.debug_level > 63)
         {
            Dprintf("TKS_StreamIO::map \"%s\" to local \"%s\".\n",
                    (char*)_name->chars, (char*)tf->source_name.chars);
         }
         return openLocal(&tf->source_name, 0);
      }
      else
      {
         // "Load" from pakfile
         ////Dprintf("xxx TKS_StreamIO::openLogic:  --> data=%08x size=%u\n", tf->file_data, tf->file_size);
         io_offset   = 0;
         io_size     = tf->file_size;
         read_buffer = tf->file_data;
         open_mode   = TKS_LOGIC_OPEN;
         return 1;
      }
   }
   return 0;
}

void TKS_StreamIO::close(void) {
   switch(open_mode)
   {
      case TKS_LOGIC_OPEN:
         closeLogic();
         break;

      case TKS_LOCAL_OPEN: 
         closeLocal();
         break;
   }
}

void TKS_StreamIO::closeLogic(void) {
   if(TKS_LOGIC_OPEN == open_mode)
   {
      read_buffer = NULL;
      io_offset   = 0;
      io_size     = 0;
      error_code  = 0;
      open_mode   = TKS_CLOSED;
   }
}

void TKS_StreamIO::closeLocal(void) {
   if(TKS_LOCAL_OPEN == open_mode)
   {
      fclose(fhandle);
      read_buffer = NULL;
      io_offset   = 0;
      io_size     = 0;
      error_code  = 0;
      open_mode   = TKS_CLOSED;
   }
}
 
sSI TKS_StreamIO::read(unsigned char *_buf, sUI _num_bytes) { 
	if(NULL == _buf)
   {
      return 0; 
   }

   switch(open_mode)
   {
      case TKS_LOGIC_OPEN:
         return readLogic(_buf, _num_bytes); // fast read

      case TKS_LOCAL_OPEN: 
         return readLocal(_buf, _num_bytes);
   }

   error_code = ERRIO;
   return -1;
}

sSI TKS_StreamIO::readLogic(unsigned char *_buf, sUI _num_bytes) { 
   sSI ret = -1;

   if(TKS_LOGIC_OPEN == open_mode)
   {
      if(io_offset < io_size)
      {
         sUI bytes_left = (io_size - io_offset);

         if(_num_bytes > bytes_left)
         {
            ret = bytes_left; // return remaining bytes
         }
         else
         {
            ret=_num_bytes;
         }

         if(ret)
         {
            // copy bytes to buffer
            int i;
            for(i=0; i<ret; i++)
            {
               _buf[i] = read_buffer[io_offset + i];
            }

            io_offset += ret;
         }
      }
   }
   return ret;
}

sU8 TKS_StreamIO::readChar(void) {
   sU8 c = 0;
   read(&c, 1);
   return c;
}

sU16 TKS_StreamIO::readWord(void) {
   sU16 c = 0;
   read((sU8*)&c, 2);
   return c;
}

sU32 TKS_StreamIO::readLong(void) {
   sU32 c = 0;
   read((sU8*)&c, 4);
   return c;
}

sSI TKS_StreamIO::readLocal(unsigned char *_buf, sUI _num_bytes) {
   sSI ret = -1;

   if(open_mode >= TKS_LOCAL_OPEN)
   {
		if(0 == io_size) // is stream, e.g. reading from /proc filesystem
		{
			int oldoff = ftell(fhandle);

			if(oldoff < 0)
            oldoff = 0;

			{
            int retRead = (int)(size_t)fread((char*)_buf, 1, _num_bytes, fhandle);
            (void)retRead;
         }
			int newoff = ftell(fhandle);

			if(newoff < 0)
         {
            newoff = 0;
            ret = _num_bytes; /*xxx*/
         }
			else
         {
            ret = newoff - oldoff;
         }

			if(ferror(fhandle))
			{
				error_code = ERRIO;
				*_buf = 0;	// return 0 
			}
			else
			{
				io_offset += ret;
				error_code = 0;
			}
		}
		else if(io_offset < io_size)
      {
         sUI bytes_left = (io_size - io_offset);

         if(_num_bytes > bytes_left)
         {
            ret = bytes_left; // return remaining bytes
         }
         else
         {
            ret = _num_bytes;
         }

         if(ret)
         { 
            int nread = (int)::fread((char*)_buf, 1, ret, fhandle);

            if(nread <= 0)
            {
               error_code = ERRIO;
            }
            else
            {
               error_code = 0; 
               io_offset += nread;
               ret = nread;
            }
         }
      }
   }

   return ret;
}

sSI TKS_StreamIO::write(unsigned char *_buf, sUI _num_bytes) {
   if(TKS_LOCAL_OPEN == open_mode)
   {
      return writeLocal(_buf, _num_bytes);
   }

   error_code = ERRIO;
   return -1;
}

sSI TKS_StreamIO::writeLocal(unsigned char *_buf, sUI _num_bytes) {
   sSI ret = -1;

   if(TKS_LOCAL_OPEN == open_mode)
   {
		ret = _num_bytes;
		int nwritten = (int)::fwrite((const char*)_buf, _num_bytes, 1, fhandle);

		if(nwritten <= 0)
		{
			error_code = ERRIO;
			tkscript->printf("[---] writeLocal(_num_bytes=%i) failed.\n", _num_bytes);
		}
		else
		{
         error_code = 0;
			ret = nwritten;
         io_offset += nwritten;

         if(io_offset > io_size)
         {
            io_size = io_offset;
         }
		}
   }

   return ret;
}

sBool TKS_StreamIO::seek(sSI _offset, sUI _type) {
   switch(open_mode)
   {
      default:
         error_code = ERRINVALIDSEEK;
         break;

      case TKS_LOGIC_OPEN:
         seekLogic(_offset, _type);
         break;

      case TKS_LOCAL_OPEN:
         seekLocal(_offset, _type);
         break;
   }

   return (0 == error_code);
}

void TKS_StreamIO::seekLogic(sSI _offset, sUI _type) {
   if(TKS_LOGIC_OPEN == open_mode)
   {
      switch(_type)
      {
         case YAC_BEG:
            if( (_offset >= 0) && (_offset <= io_size) )
            {
               io_offset = _offset;
            }
            else
            {
               error_code = ERRINVALIDSEEK;
            }
            break;

         case YAC_CUR:
            if( ((_offset + io_offset) >= 0) && ((_offset + io_offset) <= io_size) )
            {
               io_offset += _offset;
            }
            else
            {
               error_code = ERRINVALIDSEEK;
            }
            break;

         case YAC_END:
            if( ((io_size + _offset) >= 0) && ((io_size + _offset) <= io_size) )
            {
               io_offset = io_size + _offset;
            }
            else
            {
               error_code = ERRINVALIDSEEK;
            }
            break;
      }
   }
   else
   {
      error_code = ERRINVALIDSEEK;
   }
}

void TKS_StreamIO::seekLocal(sSI _offset, sUI _type) { 
	if(TKS_LOCAL_OPEN == open_mode)
   {
      switch(_type)
      {
         default:
            error_code = ERRINVALIDSEEK;
            break;

         case YAC_BEG:
            if( (_offset >= 0) && (_offset <= io_size) )
            {
               io_offset = _offset;
               fseek(fhandle, _offset, SEEK_SET);
            }
            else
            {
               error_code = ERRINVALIDSEEK;
            }
            break;

         case YAC_CUR:
            if( ((_offset + io_offset) >= 0) && ((_offset + io_offset) <= io_size) )
            {
               io_offset += _offset;
               fseek(fhandle, _offset, SEEK_CUR);
            }
            else
            {
               error_code = ERRINVALIDSEEK;
            }
            break;

         case YAC_END: 
            if( ((io_size + _offset) >= 0) && ((io_size + _offset) <= io_size) )
            {
               io_offset = io_size + _offset;
               fseek(fhandle, _offset, SEEK_END);
            }
            else
            {
               error_code = ERRINVALIDSEEK;
            }
            break;
      }
   }
}

sBool TKS_StreamIO::openLocal(char *_name, sSI _access) {
   // _access: 0=in, 1=out, 2=inandout (see TKS_IOS_xxx)
	if(_access && !tkscript->configuration.b_enablelocalfiles)
	{
		Dprintf("[---] TKS_StreamIO: access restrictions forbid access to local file \"%s\".\n",
              _name);
		return 0;
	}

   YAC_String s;
   s.visit(_name);
   return TKS_StreamIO::openLocal(&s, _access);
}
 
sBool TKS_StreamIO::openLocal(YAC_String *_name, sSI _access) {
	if(_access && !tkscript->configuration.b_enablelocalfiles)
	{
		Dprintf("[---] access restrictions forbid access to local file \"%s\".\n",
              (char*)_name->chars);
		return 0;
	}

	YAC_String *name = NULL;
   YAC_String fname;
   name = _name;
	
   read_buffer = NULL;
   io_offset   = 0;
   io_size     = 0;
   error_code  = 0;

   if(_access)
   { 
		if(TKS_IOS_INOUT == _access)
      {
			fhandle = ::fopen((char*)name->chars, "rb+");
      }
		else
      {
			fhandle = ::fopen((char*)name->chars, "wb");
      }

		if(NULL != fhandle)
      {
         open_mode = TKS_LOCAL_OPEN; 

			if(TKS_IOS_INOUT == _access) 
			{
				fseek(fhandle, 0, SEEK_END); 
				io_size = ftell(fhandle);

				if(io_size < 0)
            {
					io_size = 0;
            }

				io_offset = io_size; 
			} 
			else 
			{ 
				io_size = io_offset = 0; 
			} 
         return 1;
      }
      else
      {
         error_code = ERRCREATEFILE;
      }
   }
   else
   { 
		fhandle = fopen((char*)name->chars, "rb"); 

		if(NULL != fhandle)
      {
			fseek(fhandle, 0, SEEK_END);
			io_size = ftell(fhandle);
			fseek(fhandle, 0, SEEK_SET);
         open_mode = TKS_LOCAL_OPEN; 
         return 1;
      }
      else
      {
         error_code = ERROPENFILE;
      }
   }

   return 0;
}
 
sBool TKS_StreamIO::isOpen(void) {
   return (0 != open_mode);
}

sBool TKS_StreamIO::fail(void) {
   return (0 != error_code);
}

sBool TKS_StreamIO::eof(void) {
   return (io_offset == io_size);
}

sUI TKS_StreamIO::getFileSize(void) {
   return io_size;
}

void TKS_StreamIO::flush(void) { 
	switch(open_mode) 
	{ 
      case TKS_LOCAL_OPEN: 
         fflush(fhandle); 
         break; 
	}
}
