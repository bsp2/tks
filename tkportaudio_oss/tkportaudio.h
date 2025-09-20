
#ifndef __TKPORTAUDIO_H__
#define __TKPORTAUDIO_H__

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

//#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>

Dyac_std_exid_decl;


#define paFloat32        (0x00000001) /**< @see PaSampleFormat */
#define paInt32          (0x00000002) /**< @see PaSampleFormat */
#define paInt24          (0x00000004) /**< Packed 24 bit format. @see PaSampleFormat */
#define paInt16          (0x00000008) /**< @see PaSampleFormat */
#define paInt8           (0x00000010) /**< @see PaSampleFormat */
#define paUInt8          (0x00000020) /**< @see PaSampleFormat */

#define paContinue (0)

#define TKPORTAUDIO_MAX_BUFSIZE (8192)
#define TKPORTAUDIO_MAX_CHANNELS 2

//#define PAR_RATE         44100
#define PAR_RATE         48000
#define PAR_FRAG         2
#define PAR_LATENCY_BITS 6
#define PAR_LATENCY      (1<< PAR_LATENCY_BITS)
#define PAR_LATENCY_SEC  ( PAR_LATENCY / ((float)PAR_RATE) )
#define PAR_BITS         16       // do not change
#define PAR_FORMAT       paInt16  // do not change
#define PAR_CHANNELS     2        // do not change
#define PAR_FRAGBYTESIZE (PAR_CHANNELS * PAR_LATENCY * (PAR_BITS >> 3))
#define PAR_DSPDEV       "/dev/dsp"


#include "PaHostApiInfo.h"
#include "PaDeviceInfo.h"
#include "PaStreamParameters.h"
#include "PaStream.h"
#include "PortAudio.h"

#include "ying_portaudio.h"



#endif // __TKPORTAUDIO_H__
