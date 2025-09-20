/// AudioSampleVoice.cpp
///
/// (c) 2002-2009 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
///       08012002 <fli> file opened
///       25082003 <fli> file re-added
//        12Mar2005 source cleanup
///
/// TODO: this does not actually belong to the SDL bindings and should be moved to a separate plugin
///

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define YAC_PRINTF defined
#include <yac.h>

#include "inc_sdl.h"

#include "AudioDevice.h"

extern YAC_Host *yac_host;

#include "Bitmap.h"
#include "AudioSample.h"
#include "HAL.h"
#include "AudioSampleVoice.h"

extern _HAL *tks_HAL;
extern sUI clid_AudioSample;
extern sF32 _AudioDevice_freqtable[128];


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                   ~
//~ AudioSampleVoice                  ~
//~                                   ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AudioSampleVoice::AudioSampleVoice(void) {
   active=0;
   fracoffset=0;
   fracstep=0;
   intoffset=0;
   loop_index=0;
}

AudioSampleVoice::~AudioSampleVoice() {
}

void AudioSampleVoice::processSamples(sU32 _n, sF32 *_d) {
   if(sample)
   {
      //yac_host->printf("processSamples(%i, %08x)\n", _n, (sUI)_d);
      if(sample->length&&sample->buffer&&sample->buffer->data.any)
      {
         // mix _n stereo samples to buffer _d
         sU32 i;
         const sS16 *sp = sample->buffer->data.s16;
         switch(sample->channels)
         {
         case 1:
            // mono input sample
#define I2F (1.0f/32768.0f)
            for(i=0; i<_n; i++)
            {

               sF32 cf=(sF32)(sp[intoffset]*I2F);
               (*_d++)+=cf*voll;
               (*_d++)+=cf*volr;
               fracoffset+=fracstep;
               intoffset+=(fracoffset>>16);
               fracoffset&=0xFFFF;
               if(intoffset>=length)
               {
                  sample->getNextLoop(&loop_index, &intoffset, &length);
                  active=length!=0;
               }

            }
            break;
         case 2:
            // stereo input sample
            {
               sF32 cf;
               for(i=0; i<_n; i++)
               {
                  cf=(sF32)((sp[(intoffset<<1)+0])*I2F);
                  (*_d++)=cf*voll;
                  cf=(sF32)((sp[(intoffset<<1)+1])*I2F);
                  (*_d++)=cf*volr;
                  fracoffset+=fracstep;
                  intoffset+=(fracoffset>>16);
                  fracoffset&=0xFFFF;
                  if(intoffset>=length)
                  {
                     sample->getNextLoop(&loop_index, &intoffset, &length);
                     active=length!=0;
                  }
               }
#undef I2F
            }
            break;



         }
      }
   }
}

void AudioSampleVoice::_setVolume(sF32 _v) {
   volume=_v;
   if(volume<0.0f) volume=0.0f;
   if(volume>1.0f) volume=1.0f;
   sF32 p=((1.0f-pan)/2.0f);
   voll=volume*p;
   volr=volume*(1.0f-p);
}

void AudioSampleVoice::_setPan(sF32 _pan) {
   if(_pan<-1.0f) _pan=-1.0f;
   else if(_pan>1.0f) _pan=1.0f;
   pan=_pan;
   sF32 p=((1.0f-pan)/2.0f);
   voll=volume*p;
   volr=volume*(1.0f-p);
}

void AudioSampleVoice::_setFrequency(sF32 _p) {
   if(sample)
   {
      frequency=(_p/sample->base_frequency)*sample->frequency;
      fracstep= (sU32) (65536.0*(frequency/tks_HAL->audio.dsp_frequency));
   }
}

void AudioSampleVoice::_setSample(YAC_Object *_sample) {
   if(YAC_CHK(_sample, clid_AudioSample))
   {
      _AudioSample *s=(_AudioSample*)_sample;
      sample=s;
   }
}


void AudioSampleVoice::_noteOn(sSI _note, sF32 _velo) {
   if(_note==0&&sample)
   {
      // original pitch
      note=0;
      _setVolume(_velo);
      playtime=0;
      _setFrequency(sample->base_frequency);
      fracoffset=0;
      intoffset=0;
      length=sample->length;
      loop_index=0;
      active=length!=0;
   }
   else
   {
      if( (_note>0)&&(_note<128))
      {
         note=_note;
         _setVolume(_velo);
         playtime=0;
         _setFrequency(_AudioDevice_freqtable[note]);
         fracoffset=0;
         intoffset=0;
         loop_index=0;
         if(sample) length=sample->length;
         else length=0;
         active=length!=0;
      }
   }
}

void AudioSampleVoice::_noteOff(void) {
   if(active)
   {
      note=0;
      active=0;
   }
}
