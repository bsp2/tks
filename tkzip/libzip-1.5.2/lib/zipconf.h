#ifndef _HAD_ZIPCONF_H
#define _HAD_ZIPCONF_H

/*
   zipconf.h -- platform specific include file

   This file was generated automatically by CMake
   based on ../cmake-zipconf.h.in.
 */

#define LIBZIP_VERSION "1.52@"
#define LIBZIP_VERSION_MAJOR 1
#define LIBZIP_VERSION_MINOR 52
#define LIBZIP_VERSION_MICRO 0

#define ZIP_STATIC

//${ZIP_NULLABLE_DEFINES}
#define _Nullable
#define _Nonnull

//${LIBZIP_TYPES_INCLUDE}

#define SIZEOF_OFF_T 8

typedef char           zip_int8_t;
typedef unsigned char  zip_uint8_t;
typedef short          zip_int16_t;
typedef unsigned short zip_uint16_t;
typedef int            zip_int32_t;
typedef unsigned int   zip_uint32_t;
#ifdef _MSC_VER
 typedef unsigned __int64        zip_uint64_t;
 typedef signed __int64          zip_int64_t;
#else
 typedef unsigned long long      zip_uint64_t;
 typedef signed long long        zip_int64_t;
#endif

#define ZIP_INT8_MIN	 (-ZIP_INT8_MAX-1)
#define ZIP_INT8_MAX	 0x7f
#define ZIP_UINT8_MAX	 0xff

#define ZIP_INT16_MIN	 (-ZIP_INT16_MAX-1)
#define ZIP_INT16_MAX	 0x7fff
#define ZIP_UINT16_MAX	 0xffff

#define ZIP_INT32_MIN	 (-ZIP_INT32_MAX-1L)
#define ZIP_INT32_MAX	 0x7fffffffL
#define ZIP_UINT32_MAX	 0xffffffffLU

#define ZIP_INT64_MIN	 (-ZIP_INT64_MAX-1LL)
#define ZIP_INT64_MAX	 0x7fffffffffffffffLL
#define ZIP_UINT64_MAX	 0xffffffffffffffffULL

#endif /* zipconf.h */
