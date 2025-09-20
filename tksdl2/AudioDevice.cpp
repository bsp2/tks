/// AudioDevice.cpp
///
/// (c) 2002-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
///       08012002: file opened
///

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <yac.h>

#include "inc_sdl.h"

#include "AudioDevice.h"

#include "HAL.h"

extern YAC_Host *yac_host;

sSI _AudioDevice::cbid_SDL_FillAudioBuffer = -1;

void FSOUND_MixerClipCopy_Float32(void *dest, void *src, sS32 len) {
   sS32 i=len<<1;
   sS16 *d=(sS16*)dest;
   sF32 *s=(sF32*)src;
   while(--i>=0)
   {
      sS32 val=(sS32)(s[i]*32767.9f);
      d[i]=(sS16)(val < -32768 ? -32768 : val > 32767 ? 32767 : ((sS16)val) );
   }
}

void FSOUND_MixerClipCopy_Float32_SwapStereo(void *dest, void *src, sS32 len) {
   int i=len<<1;
   sS16 *d=(sS16*)dest;
   sF32 *s=(sF32*)src;
   while((i-=2)>=0)
   {
      sS32 vall=(sS32)(s[i]*32767.9f);
      sS32 valr=(sS32)(s[i+1]*32767.9f);
      d[i]=(sS16)(valr < -32768 ? -32768 : valr > 32767 ? 32767 : valr );
      d[i+1]=(sS16)(vall < -32768 ? -32768 : vall > 32767 ? 32767 : vall );
   }
}


void tks_sdl_audio_callback(void *_ud, sU8 *_stream, int _len) {
   _AudioDevice *amix=(_AudioDevice*)_ud;
   if(amix)
   {
      if(amix->running)
      {
         ////yac_host->printf("xxx sdl_audio_callback len=%i\n", _len);
         ////SDL_mutexP((SDL_mutex *)amix->mtx_beginblock);
         if(amix->dsp_active)
            amix->beginBlock((sU32)(_len>>2), (sS16*)_stream);
         ////SDL_mutexV((SDL_mutex *)amix->mtx_beginblock);
      }
   }
}


_AudioDevice::_AudioDevice(void) {
   volume=1.0f;
   swap_stereo=0;
   dsp_active=0; // dsp opened flag
   running=0; // dsp started flag

   dsp_spec=new SDL_AudioSpec;
   dsp_spec->userdata=(void*)this;

   dsp_frequency=44100.0f;
   bpm=125.0f;
   ppq=24;
   calcTimeframeSize();
   sig_time=0;
   sig_render=0;
   sig_postprocess=0;
   sig_block=0;
   b_file=0;
   buildFrequencyTable();
   // ---- allocate temporary floatarray for onAudioPostProcess callback ----
   tmp_fa = YAC_NEW_CORE_POOLED(YAC_CLID_FLOATARRAY);
   ////mtx_beginblock=0;
   sc_audiocontext=0;
   b_enabled=0;
   def_buf_size=8192;
}   

_AudioDevice::~_AudioDevice() {
   _closeDSP();

   if(dsp_spec) { delete dsp_spec; dsp_spec=0; }

   sig_time=0;
   sig_render=0;
   sig_postprocess=0;
   if(tmp_fa)
   {
      YAC_DELETE(((YAC_Object*)tmp_fa));
      tmp_fa = NULL;
   }
   if(sc_audiocontext)
   {
      yac_host->yacContextDestroy(sc_audiocontext);
      sc_audiocontext=0;
   }
}

sF32 _AudioDevice_freqtable[128]; // MIDI note frequency table
void _AudioDevice::buildFrequencyTable(void) {
   sF32 i;
   for(i=0.0f; i<128.0f; i+=1.0f)
   {
      _AudioDevice_freqtable[(sU8)i]=(sF32)((440.0f/32.0f)*::exp( ((i-9.0f)/12.0f)*log(2.0f) ));
   }
   
}

sF32 _MIDINoteToFrequency(sF32 _note) {
   if(_note >= 0)
   {
      int idx = (int)_note;
      if(0 == idx)
      {
         double t; 
         sF32 s = (sF32)modf(_note,&t);
         return ( (s*_AudioDevice_freqtable[1])+(s-1)*_AudioDevice_freqtable[0] );
      }
      else
      {
         if(idx>=127)
         {
            return ( _AudioDevice_freqtable[127] );
         }
         else
         {
            double t; 
            sF32 s=(sF32)modf(_note,&t);
            return ( (s*_AudioDevice_freqtable[idx+1])+(1-s)*_AudioDevice_freqtable[idx] );
         }
      }
   }
   return 440.0f;
}

void _AudioDevice::calcTimeframeSize(void) {
   timeframe_count        = 0;
   timeframe_next_offset  = 0;
   timeframe_last_offset  = 0;
   timeframe_real_size    = (sF32)(( ( (double) dsp_frequency/2 *60 ) / (double) ppq ) / bpm);
   timeframe_size         = (sS32)timeframe_real_size;
   timeframe_samples_done = 0;
}

void YAC_VCALL _AudioDevice::yacGetSignalStringList(YAC_String *_sig) {
   _sig->visit("onAudioFrame:0 onAudioPostProcess:3 onAudioRender:3 onAudioBeginBlock:3 ");
}

void YAC_VCALL _AudioDevice::yacRegisterSignal(sUI _id, YAC_FunctionHandle _fun) {
   switch(_id)
   {
   case 0: sig_time        =_fun; break; // begin new timeframe
   case 1: sig_postprocess =_fun; break; // post process buffer of all rendered frames
   case 2: sig_render      =_fun; break; // render frame
   case 3: sig_block       =_fun; break; // begin new buffer
   }
}

void _AudioDevice::beginTimeframe(void) {
   ////yac_host->printf("xxx beginTimeFrame\n");
   timeframe_count       = (timeframe_count + 1) & 0x7FFF;
   timeframe_last_offset = timeframe_next_offset;
   timeframe_real_size   = 2*(sF32)(( ( (double) dsp_frequency/2 *60 ) / (double) ppq ) / bpm);
   timeframe_next_offset = (sS32) (timeframe_real_size* (double) timeframe_count );
   if(timeframe_next_offset < timeframe_last_offset)
   {
      timeframe_size = (sS32)(timeframe_real_size*32768.0) - timeframe_last_offset;
   }
   else
   {
      timeframe_size = timeframe_next_offset - timeframe_last_offset;
   }
   timeframe_samples_done = 0;
   
   // ---- invoke TKS callback ----
   if(sig_time)
   {
      yac_host->yacEvalFunction(sc_audiocontext, sig_time, 0, 0);
   }
}

void _AudioDevice::beginBlock(sU32 _rs, sS16 *_d) {
   req_size         = _rs;
   req_samples_done = 0;
   dsp_buffer       = _d;
 // ---- invoke TKS callback ----
   if(sig_block)
   {
   ////yac_host->printf("xxx beginBlock rs=%i\n", _rs);
      YAC_FloatArray *fa = (YAC_FloatArray*)tmp_fa;
      fa->own_data     = 0;
      fa->elements     = mix_buffer;
      fa->num_elements = req_size<<1;
      fa->max_elements = req_size<<1;

      YAC_Value args[1];
      args[0].initObject(fa, 0);

      yac_host->yacEvalFunction(sc_audiocontext, sig_block, 1, args);
      fa->elements     = 0;
      fa->num_elements = fa->max_elements = 0;
   }
}

void _AudioDevice::_processTimeFrames(void) {
   // ---- collect request samples, stop at timeframe end to update soundengine state. ----
   //if(!timeframe_size) return;
   while(req_samples_done != req_size)
   {
      sU32 tf_samples_left=timeframe_size-timeframe_samples_done;
      /*yac_host->printf("processTimeFrames: tf_samples_left=%i\n",tf_samples_left);
      yac_host->printf("processTimeFrames: timeframe_size=%i\n",timeframe_size);
      yac_host->printf("processTimeFrames: timeframe_samples_done=%i\n",timeframe_samples_done);*/
      if(!tf_samples_left)
      {
         // ---- start new timeframe, update mixer, effect and generator settings ----
         beginTimeframe();
         tf_samples_left = timeframe_size;
      }
      sU32 req_samples_left = (req_size - req_samples_done);
      sU32 n;
      if(tf_samples_left < req_samples_left)
      {
         n = tf_samples_left; // process rest of timeframe
      }
      else
      {
         n = req_samples_left; // process rest of buffer
      }
      
      //yac_host->printf("n=%i\n",n);
      /*yac_host->printf("processTimeFrames2: tf_samples_left=%i\n",tf_samples_left);
      yac_host->printf("processTimeFrames2: timeframe_size=%i\n",timeframe_size);
      yac_host->printf("processTimeFrames2: timeframe_samples_done=%i\n",timeframe_samples_done);*/
      
      
      // ---- invoke TKS callback ----
      //yac_host->printf("calling sig_render\n");
      if(sig_render)
      {
         YAC_FloatArray *fa = (YAC_FloatArray*)tmp_fa;
         fa->own_data     = 0;
         fa->elements     = mix_buffer+(req_samples_done<<1);
         fa->num_elements = n*2;
         fa->max_elements = n*2;

         YAC_Value args[1];
         args[0].initObject(fa, 0);

         yac_host->yacEvalFunction(sc_audiocontext, sig_render, 1, args);
         fa->elements = NULL;
         fa->num_elements = fa->max_elements = 0;
      }
      timeframe_samples_done+=n;
      //::printf("render req:samples_done=%i n=%i\n", req_samples_done, n);
      req_samples_done+=n;
   }
}

void _AudioDevice::_finishBlock(void) {
   sS16 *_d;
   if(!(_d=dsp_buffer)) return;
   dsp_buffer=0;
   
   // ---- invoke TKS postprocessing callback ----
   if(sig_postprocess)
   {
      void *f=sig_postprocess;
      YAC_FloatArray *fa=(YAC_FloatArray*)tmp_fa;
      fa->num_elements=req_size<<1; // stereo samples
      fa->max_elements=req_size<<1;
      fa->elements=mix_buffer;
      YAC_Value args[1];
      args[0].initObject(fa, 0);
      yac_host->yacEvalFunction(sc_audiocontext, f, 1, args);
      fa->elements=0;
      fa->num_elements=0;
      fa->max_elements=0;
   }
   // ---- convert floats to signed 16bit integers, float range is -1..1 ----
   if(swap_stereo) FSOUND_MixerClipCopy_Float32_SwapStereo((void*)_d, (void*)mix_buffer, req_size);
   else FSOUND_MixerClipCopy_Float32((void*)_d, (void*)mix_buffer, req_size);
#ifdef YAC_MACOSX
   sUI i;
   sU8 *dswap=(sU8*)_d;
   for(i=0; i<(req_size<<2); i+=2) // stereo, 16bit
   {
      sU8 t=dswap[i]; // swap endianess
      dswap[i] = dswap[i+1];
      dswap[i+1] = t;
   }
#endif // YAC_MACOSX
   if(b_file)
   {
      //ofs->write((const char*)_d, req_size<<2); 
      // ---- write stereo sS16 samples into c++ filestream ----
      ::fwrite((void*)_d, (size_t)(req_size<<2), 1, (FILE*)ofs);
   }
}

void YAC_VCALL _AudioDevice::yacOperator(sSI, YAC_Object*, YAC_Value *) {
}

void _AudioDevice::_setVolume(sF32 _vol) {
   volume=_vol;
}

sF32 _AudioDevice::_getVolume(void) {
   return volume;
}

void _AudioDevice::_setSwapstereo(sS32 _enabled) {
   swap_stereo=_enabled;
}

sSI _AudioDevice::_getSwapstereo(void) {
   return swap_stereo;
}

void _AudioDevice::_setBpm(sF32 _bpm) {
   if(_bpm>15.0f && _bpm<384.0f)
      bpm=_bpm;
}

sF32 _AudioDevice::_getBpm(void) {
   return bpm;
}

void _AudioDevice::_setPpq(sS32 _ppq) {
   if(_ppq>23 && _ppq<385)
      ppq=_ppq;
}

sSI _AudioDevice::_getPpq(void) {
   return ppq;
}

sSI _AudioDevice::_openDSP(sF32 _freq, sSI _bufsize) {
   if(b_enabled) 
   {
      return 0;
   }
   running=0;
   if(_bufsize!=0)
   {
      if(_bufsize<256) 
      {
         _bufsize=256;
      }
   }
   else 
   {
      _bufsize=def_buf_size;
   }
   
   if(_bufsize>8192) _bufsize=8192;
   if(_freq==0.0f) return 0;
   if(_freq<11025.0f) _freq=11025.0f;
   if(dsp_active) _closeDSP();

   dsp_spec->freq=(sS32)_freq;
   dsp_spec->format=AUDIO_S16;
   dsp_spec->channels=2;//(sU8)_ch;
   dsp_spec->silence=0;
   dsp_spec->samples=(sU16)_bufsize;
   dsp_spec->callback=tks_sdl_audio_callback;

   // Create script thread context for audio callback
   sc_audiocontext = yac_host->yacContextCreate();
   if(sc_audiocontext==NULL)
   {
      yac_host->printf("[---] AudioDevice::openDSP() failed (yacContextCreate()).\n");
      return 0;
   }

   ////mtx_beginblock=(void*)SDL_CreateMutex();

   int r=SDL_OpenAudio(dsp_spec, 0);
   if(r>=0)
   {
      dsp_block_size=dsp_spec->size;
      dsp_channels=dsp_spec->channels;
      dsp_block_samples=dsp_spec->samples;
      dsp_bits=16;
      dsp_frequency=(sF32)dsp_spec->freq;
      dsp_active=1;
      if(yac_host->yacGetDebugLevel()>0)
      {
         yac_host->printf("[+++] SDL_OpenAudio OK format=%i blksize=%i freq=%i.\n",
                          dsp_spec->format, dsp_spec->samples, dsp_spec->freq
                          );
      }
      calcTimeframeSize();
      tks_HAL->audio.dsp_frequency=dsp_frequency;
      return 1;
   }
   else
   {
      yac_host->printf("[---] AudioDevice::openDSP() failed (SDL_OpenAudio()).\n");
      ////if(mtx_beginblock) { SDL_DestroyMutex((SDL_mutex *)mtx_beginblock);	mtx_beginblock=0; }
      yac_host->yacContextDestroy(sc_audiocontext); sc_audiocontext=NULL;
      return 0;
   }
}

void _AudioDevice::_setFileName(YAC_String *_s) {
   filename.yacCopy(_s);
}

void _AudioDevice::_start(void) {
   if(!dsp_active) return;
   // ---- output to file? ----
   if(b_file)
   {
      // ---- close previous stream ----
      if(ofs)	{
         //delete (ofstream*)ofs; 
         ::fclose((FILE*)ofs);
         ofs=0;	
      }
      //ofs=new ofstream;
      //ofs->open((const char*)filename.chars, ios::out|ios::binary);
      ofs=(void*)::fopen((const char*)filename.chars,
         "wb");
      //if(!ofs->is_open())
      if(!ofs)
      {
         yac_host->printf("[---] AudioMixer::start: failed to open dsp output filestream!\n");
         b_file=0;
         //delete ofs;
         //ofs=0;
      }
      
   }
   // ---- actually start dsp device ----
   running=1;
   SDL_PauseAudio(0);
}

void _AudioDevice::_stop(void) {
   if(dsp_active)
   {
      running=0;
      if(yac_host->yacGetDebugLevel()>0)
      {
         yac_host->printf("[...] AudioMixer::closeDSP: stopping audio.\n");
      }

      SDL_PauseAudio(1);
      while(::SDL_GetAudioStatus()==SDL_AUDIO_PLAYING) 
      {
         ::SDL_Delay(1000);
      }

   }
}

void _AudioDevice::_enableFileOutput(sS32 _b) {
   b_file=(_b!=0);
}

void _AudioDevice::_closeDSP(void) {
   if(!dsp_active)	return;
   ////if(mtx_beginblock) SDL_mutexP((SDL_mutex *)mtx_beginblock);
   dsp_active=0;
   _stop();

   ::SDL_CloseAudio();

   if(yac_host->yacGetDebugLevel()>0)
   {
      yac_host->printf("[...] AudioMixer::closeDSP: device closed.\n");
   }
   if(b_file)
   {
      // ---- close output filestream ----
      //if(ofs)	{ ofs->close(); delete ofs; ofs=0; }
      if(ofs)
      {
         ::fclose((FILE*)ofs);
         ofs=0;
      }
   }
   ////if(mtx_beginblock) SDL_mutexV((SDL_mutex *)mtx_beginblock);
   ////if(mtx_beginblock) { SDL_DestroyMutex((SDL_mutex *)mtx_beginblock);	mtx_beginblock=0; }
   yac_host->yacContextDestroy(sc_audiocontext); sc_audiocontext = NULL;
}

void _AudioDevice::_enable(sS32 _b) {
   b_enabled=(_b!=0);
}

void _AudioDevice::_setDefaultBufferSize(sSI _s) {
   def_buf_size=_s>256?_s:256;
}

