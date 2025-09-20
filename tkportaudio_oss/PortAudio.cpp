
#include "tkportaudio.h"


#include "PaDeviceInfo.h"
#include "PaStreamParameters.h"
#include "PaHostApiInfo.h"
#include "PaStream.h"
#include "PortAudio.h"


_PortAudio::_PortAudio(void) {
}

_PortAudio::~_PortAudio() {
}

void _PortAudio::_Initialize(void) {
}

sSI _PortAudio::_GetNumDevices(void) {
   return 1;
}

void _PortAudio::_GetDeviceInfo(sSI _devIdx, YAC_Value *_r) {
   (void)_devIdx;

   _PaDeviceInfo *o = (_PaDeviceInfo*) YAC_NEW_(PaDeviceInfo);
   o->initDeviceInfo();
   _r->initObject(o, 1);
}

sSI _PortAudio::_GetDefaultInputDeviceIndex(void) {
   return 0;
}

sSI _PortAudio::_GetDefaultOutputDeviceIndex(void) {
   return 0;
}

sSI _PortAudio::_IsFormatSupported(YAC_Object *_inParam, YAC_Object *_outParam, sF32 _sampleRate) {
   (void)_inParam;
   (void)_outParam;
   if( PAR_RATE == _sampleRate )
   {
      return 1;
   }
   else
   {
      return 0;
   }
}


sSI _PortAudio::_OpenStream(YAC_Object *_inParams, YAC_Object *_outParams, sF32 _sampleRate, YAC_Object *_streamCbk, YAC_Object *_retStream) {
   if(YAC_CHK(_inParams, clid_PaStreamParameters))
   {
      YAC_CAST_ARG(_PaStreamParameters, isp, _inParams);

      if(YAC_CHK(_outParams, clid_PaStreamParameters))
      {
         YAC_CAST_ARG(_PaStreamParameters, osp, _outParams);

         if(YAC_VALID(_streamCbk)) //if(YAC_CHK(_streamCbk, YAC_CLID_FUNCTION))
         {
            if(YAC_CHK(_retStream, clid_PaStream))
            {
               YAC_CAST_ARG(_PaStream, s, _retStream);
               
               if(s->initStream((void*)_streamCbk, isp, osp))
               {
                  // Succeeded
                  return 1;
               }
               else
               {
                  yac_host->printf("[---] PortAudio<oss>::openStream: initStream() failed.\n");
               }
               
            }
            else
            {
               Dyac_throw_def(InvalidPointer, "retStream is not a valid PaStream object");
            }
         }
         else
         {
            Dyac_throw_def(InvalidPointer, "streamCbk is not a valid Function object");
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "outParams is not a valid PaStreamParameters object");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "inParams is not a valid PaStreamParameters object");
   }
   return 0;
}

sSI _PortAudio::_GetSampleSize(sSI _format) {
   switch(_format)
   {
      case paInt8:    return 1;
      case paInt16:   return 2;
      case paInt24:   return 3;
      case paInt32:
      case paFloat32: return 4;
   }
   yac_host->printf("[~~~] PortAudio<oss>::GetSampleSize: unsupported format %d\n", _format);
   return 0;
}

void _PortAudio::_Terminate(void) {
}
