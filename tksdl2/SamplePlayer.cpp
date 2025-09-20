/// SamplePlayer.cpp
///
/// (c) 2002-2023 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
///       08012002 <fli> file opened
///       25082003 <fli> file re-added
//        12Mar2005 source cleanup
///
/// TODO: this does not actually belong to the SDL bindings and should be moved to a separate plugin


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define YAC_PRINTF defined
#include <yac.h>
extern YAC_Host *yac_host;

#include "inc_sdl.h"

#include "HAL.h"
#include "Bitmap.h"
#include "AudioSample.h"
#include "AudioDevice.h"
#include "AudioSampleVoice.h"
#include "SamplePlayer.h"

extern sUI clid_AudioSample;
extern sF32 _AudioDevice_freqtable[128];


_SamplePlayer::_SamplePlayer(void) {
   polyphony=1;
   volume=1.0f;
   pan=0.0f;
   finetune=0.0f;
   default_sample=0;
}

_SamplePlayer::~_SamplePlayer() {
}

void YAC_VCALL _SamplePlayer::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(YAC_CHK(_o, class_ID))
   {
      _SamplePlayer *o=(_SamplePlayer*)_o;
      polyphony=o->polyphony;
      volume=o->volume;
      pan=o->pan;
      finetune=o->finetune;
      default_sample=o->default_sample;
   }
}

void _SamplePlayer::processSamples(sU32 _n, sF32 *_d) {
   sU8 i;
   for(i=0; i<polyphony; i++)
      if(voices[i].active)
         voices[i].processSamples(_n, _d);
}

void _SamplePlayer::_setSample(YAC_Object *_smp) {
   if(YAC_CHK(_smp, clid_AudioSample))
      default_sample=(_AudioSample*)_smp;
}

YAC_Object *_SamplePlayer::_getSample(void) {
   return default_sample;
}

void _SamplePlayer::_noteOn(sSI _midi, sF32 _volume) {
   sU8 i;
   sU8 maxi=0;
   sU32 maxt=0;
   // ---- find free voice or overwrite longest playing voice ----
   for(i=0; i<polyphony; i++)
   {
      if(!voices[i].active)
      {
         voices[i].sample=default_sample;
         voices[i]._setPan(pan);
         voices[i]._noteOn(_midi, _volume);
         return;
      }
      else
      {
         if(voices[i].playtime>maxt)
         {
            maxt=voices[i].playtime;
            maxi=i;
         }
      }
   }
   voices[maxi].sample=default_sample;
   voices[maxi]._setPan(pan);
   voices[maxi]._noteOn(_midi, _volume);
}

void _SamplePlayer::_noteOff(sSI _note) {
   sU8 i;
   //sU8 maxi=0;
   sU32 maxt=0;
   // ---- find longest playing note and kill voice ----
   for(i=0; i<polyphony; i++)
   {
      if(voices[i].note==_note)
      {
         if(voices[i].playtime>maxt)
         {
            maxt=voices[i].playtime;
            //maxi=i;
         }
      }
   }
   if(maxt) voices[i]._noteOff();
}

void _SamplePlayer::_setPolyphony(sSI _p) {
   if( ((sUI)_p)<TKS_MAX_AUDIOVOICES )
      polyphony=(sUI)_p;
}

sSI _SamplePlayer::_getPolyphony(void) {
   return polyphony;
}

void _SamplePlayer::_setVolume(sF32 _v) {
   volume=_v;
}

sF32 _SamplePlayer::_getVolume(void) {
   return volume;
}

void _SamplePlayer::_setPan(sF32 _v) {
   pan=_v;
}

sF32 _SamplePlayer::_getPan(void) {
   return pan;
}

void _SamplePlayer::_processSamples(YAC_Object *_fa) {
   if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
   {
      YAC_FloatArray *fa=(YAC_FloatArray*)_fa;
      sUI n=fa->num_elements;
      sF32 *d=fa->elements;
      if(n&&d) processSamples(n, d);
   }
}
