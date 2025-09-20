#ifndef __COMMON_H
#define __COMMON_H

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>

// OpenVG-Compatible Type definitions..
typedef unsigned int      VGuint ;
typedef float             VGfloat;
typedef int               VGint;
typedef unsigned short    VGushort;
typedef short             VGshort;
typedef unsigned char     VGubyte;

typedef enum
{
  VG_FALSE = 0,
  VG_TRUE  = 1
} VGboolean;

extern void *vgtesselator_alloc (unsigned int _sz);
extern void  vgtesselator_free (void *_ptr);
extern void *vgtesselator_realloc (void *_ptr, unsigned int _sz);

#endif
