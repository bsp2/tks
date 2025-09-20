// ----
// ---- file   : SampleChain.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2016 by Bastian Spiegel. 
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See 
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- changed: 25Mar2016
// ----
// ----
// ----

#include "tksamplechain.h"


SampleChain::SampleChain(void) {
   sc = NULL;
   b_alg_valid = YAC_FALSE;
}

SampleChain::~SampleChain() {

   free();
}

void SampleChain::free(void) {
   if(NULL != sc)
   {
      alg.exit(&sc);
   }
}

sUI SampleChain::getNumAlgorithms(void) {
   return samplechain_get_num_algorithms();
}

sBool SampleChain::selectAlgorithm(sUI _idx) {
   free();
   b_alg_valid = samplechain_select_algorithm(_idx, &alg);
   return b_alg_valid;
}

void SampleChain::getAlgorithmName(YAC_Value *_r) {
   if(b_alg_valid)
   {
      _r->initEmptyString();
      _r->value.string_val->visit(alg.query_algorithm_name());
   }
   else
   {
      _r->initString(NULL, YAC_FALSE/*bDelete*/);
   }
}

sBool SampleChain::init(sUI _numSlices) {
   free();

   if(b_alg_valid)
   {
      alg.init(&sc, _numSlices);

      sBool ret = (NULL != sc);

      return ret;
   }

   return YAC_FALSE;
}

sBool SampleChain::setParameteri(YAC_String *_pname, sSI _value) {
   if(b_alg_valid)
   {
      if(NULL != sc)
      {
         if(YAC_Is_String(_pname))
         {
            sBool ret = alg.set_parameter_i(sc, (const char*)_pname->chars, _value);

            return ret;
         }
      }
   }
   return YAC_FALSE;
}

sBool SampleChain::setParameterf(YAC_String *_pname, sF32 _value) {
   if(b_alg_valid)
   {
      if(NULL != sc)
      {
         if(YAC_Is_String(_pname))
         {
            sBool ret = alg.set_parameter_f(sc, (const char*)_pname->chars, _value);

            return ret;
         }
      }
   }
   return YAC_FALSE;
}

sBool SampleChain::add(sUI _numFrames, YAC_Object *_userData) {
   if(b_alg_valid)
   {
      if(NULL != sc)
      {
         return alg.add(sc, _numFrames, (void*)_userData);
      }
   }
   return YAC_FALSE;
}

void SampleChain::calc(void) {
   if(b_alg_valid)
   {
      if(NULL != sc)
      {
         alg.calc(sc);
      }
   }
}

sUI SampleChain::queryNumElements(void) {
   if(b_alg_valid)
   {
      if(NULL != sc)
      {
         return alg.query_num_elements(sc);
      }
   }
   return 0;
}

sUI SampleChain::queryTotalSize(void) {
   if(b_alg_valid)
   {
      if(NULL != sc)
      {
         return (sUI)alg.query_total_size(sc);
      }
   }
   return 0;
}

sUI SampleChain::queryElementOffset(sUI _elementIdx) {
   if(b_alg_valid)
   {
      if(NULL != sc)
      {
         return (sUI)alg.query_element_offset(sc, _elementIdx);
      }
   }
   return 0;
}

sUI SampleChain::queryElementTotalSize(sUI _elementIdx) {
   if(b_alg_valid)
   {
      if(NULL != sc)
      {
         return (sUI)alg.query_element_total_size(sc, _elementIdx);
      }
   }
   return 0;
}

sUI SampleChain::queryElementOriginalSize(sUI _elementIdx) {
   if(b_alg_valid)
   {
      if(NULL != sc)
      {
         return (sUI)alg.query_element_original_size(sc, _elementIdx);
      }
   }
   return 0;
}

YAC_Object *SampleChain::queryElementUserData(sUI _elementIdx) {
   if(b_alg_valid)
   {
      if(NULL != sc)
      {
         return (YAC_Object*)alg.query_element_user_data(sc, _elementIdx);
      }
   }
   return NULL;
}

void SampleChain::exit(void) {
   free();
}
