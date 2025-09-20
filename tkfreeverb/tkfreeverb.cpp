#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <yac.h>
YAC_Host *yac_host;

#include "tkfreeverb.h"

#include "ying_freeverb.h"
#include "ying_freeverb.cpp"
#include "ying_freeverb_Freeverb.cpp"


Freeverb::Freeverb(void) {
}

Freeverb::~Freeverb() {
}

void Freeverb::mute(void) {
   rev.mute();
}

void Freeverb::processMix(YAC_Object *_in, YAC_Object *_out, sUI _numSamples, sUI _skip) {
   if(YAC_BCHK(_in, YAC_CLID_FLOATARRAY))
   {
      YAC_CAST_ARG(YAC_FloatArray, in, _in);

      if(YAC_BCHK(_out, YAC_CLID_FLOATARRAY))
      {
         YAC_CAST_ARG(YAC_FloatArray, out, _out);

         sUI r = 2 * _numSamples;
         if(in->num_elements >= r)
         {
            if(out->num_elements >= r)
            {

               // benchmark/test: process samples individually
               // // for(sUI x=0; x<r; x+=2)
               // // {
               // //    sF32 *inputL  = in->elements+x;
               // //    sF32 *inputR  = in->elements+x + 1;
               // //    sF32 *outputL = out->elements+x;
               // //    sF32 *outputR = out->elements+x + 1;
               // //    rev.processmix(inputL, inputR,
               // //                   outputL, outputR,
               // //                   1, _skip
               // //                   );
               // // }

               sF32 *inputL  = in->elements;
               sF32 *inputR  = in->elements + 1;
               sF32 *outputL = out->elements;
               sF32 *outputR = out->elements + 1;
               rev.processmix(inputL, inputR,
                              outputL, outputR,
                              _numSamples, _skip
                              );
            }
         }
      }
   }
}

void Freeverb::setRoomSize(sF32 _value) {
   rev.setroomsize(_value);
}

sF32 Freeverb::getRoomSize(void) {
   return rev.getroomsize();
}

void Freeverb::setDamp(sF32 _value) {
   rev.setdamp(_value);
}

sF32 Freeverb::getDamp(void) {
   return rev.getdamp();
}

void Freeverb::setWet(sF32 _value) {
   rev.setwet(_value);
}

sF32 Freeverb::getWet(void) {
   return rev.getwet();
}

void Freeverb::setDry(sF32 _value) {
   rev.setdry(_value);
}

sF32 Freeverb::getDry(void) {
   return rev.getdry();
}

void Freeverb::setWidth(sF32 _value) {
   rev.setwidth(_value);
}

sF32 Freeverb::getWidth(void) {
   return rev.getwidth();
}

void Freeverb::setMode(sF32 _value) {
   rev.setmode(_value);
}

sF32 Freeverb::getMode(void) {
   return rev.getmode();
}





// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;

   YAC_Init_freeverb(_host);
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_freeverb(_host);
}


#include <yac_host.cpp>
