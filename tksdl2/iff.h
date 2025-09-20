/// iff.h
///
/// (c) 2000-2003 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL)
///
///

#ifndef __TKS_IFF_H__
#define __TKS_IFF_H__

/// Some macros useful to handle with IFF style files 
/// composed of chunks ( IFF8SVX, AIFF, LWO, AON.. )
/// and eventually also subchunks (LWO).
///

#ifdef YAC_LITTLE_ENDIAN

/// get a single big endian 2 byte 16bit unsigned integer from a buffer 
/// on an architecture (CPU)
/// that stores its data in little endian format.
///
/// @param c Buffer (e.g. buf ; if buf was declared as 'byte buf[...];'
/// @param a Index in buffer (number of bytes)
/// @param b Variablename to store value in.
///
#define getI2(c,a,b) b=*(sU16 *)&(c[(a)]); \
          b=\
                      ((b&0x00ff)<<8)|\
                      ((b&0xff00)>>8);

/// get a single big endian 2 byte 16bit signed integer from a buffer 
/// on an architecture (CPU)
/// that stores its data in little endian format.
///
/// @param c Buffer (e.g. buf ; if buf was declared as 'byte buf[...];'
/// @param a Index in buffer (number of bytes)
/// @param b Variablename to store value in.
///
#define getSI2(c,a,b) b=*(sS16 *)&(c[(a)]); \
          b=\
                      ((b&0x00ff)<<8)|\
                      ((b&0xff00)>>8);

/// get a single big endian 4 byte 32bit signed integer from a buffer 
/// on an architecture (CPU)
/// that stores its data in little endian format.
///
/// @param c Buffer (e.g. buf ; if buf was declared as 'byte buf[...];'
/// @param a Index in buffer (number of bytes)
/// @param b Variablename to store value in.
///
#define getI4(c,a,b) b=*(sS32 *)&(c[(a)]); \
	  b=\
		      ((b&0x000000ff)<<24)|\
		      ((b&0x0000ff00)<<8)|\
		      ((b&0x00ff0000)>>8)|\
		      ((b&0xff000000)>>24);




/// get a single big endian 4 byte 32bit IEEE float from a buffer 
/// on an architecture (CPU)
/// that stores its data in little endian format.
///
/// @param c Buffer (e.g. buf ; if buf was declared as 'byte buf[...];'
/// @param a Index in buffer (number of bytes)
/// @param b Variablename to store value in.
///
#define getF4(c,a,b) { register sU32  __bF4=*((sU32 *)(&(c[(a)]))); \
                     __bF4=\
                      ((__bF4&0x000000ff)<<24)|\
                      ((__bF4&0x0000ff00)<<8 )|\
                      ((__bF4&0x00ff0000)>>8 )|\
                      ((__bF4&0xff000000)>>24)\
                      ; b=*(sF32 *)(&__bF4) \
                      ; }


#else

/* Big endian . Amiga rulez :) */

/// get a single big endian 2 byte 16bit unsigned integer from a buffer 
/// on an architecture (CPU)
/// that stores its data in big endian format (68K, PowerPC, recent MIPS)
///
/// @param c Buffer (e.g. buf ; if buf was declared as 'byte buf[...];'
/// @param a Index in buffer (number of bytes)
/// @param b Variablename to store value in.
///
#define getI2(c,a,b) b=*(sU16 *)&(c[(a)]);

/// get a single big endian 2 byte 16bit signed integer from a buffer 
/// on an architecture (CPU)
/// that stores its data in big endian format (68K, PowerPC, recent MIPS)
///
/// @param c Buffer (e.g. buf ; if buf was declared as 'byte buf[...];'
/// @param a Index in buffer (number of bytes)
/// @param b Variablename to store value in.
///
#define getSI2(c,a,b) b=*(sS16 *)&(c[(a)]); 

/// get a single big endian 4 byte 32bit signed integer from a buffer 
/// on an architecture (CPU)
/// that stores its data in big endian format (68K, PowerPC, recent MIPS)
///
/// @param c Buffer (e.g. buf ; if buf was declared as 'byte buf[...];'
/// @param a Index in buffer (number of bytes)
/// @param b Variablename to store value in.
///
#define getI4(c,a,b) b=*(sS32 *)&(c[(a)]);


/// get a single big endian 4 byte 32bit IEEE float from a buffer 
/// on an architecture (CPU)
/// that stores its data in big endian format (68K, PowerPC, recent MIPS)
///
/// @param c Buffer (e.g. buf ; if buf was declared as 'byte buf[...];'
/// @param a Index in buffer (number of bytes)
/// @param b Variablename to store value in.
///
#define getF4(c,a,b) b=*(sF32 *)&(c[(a)]);
#endif


#endif
