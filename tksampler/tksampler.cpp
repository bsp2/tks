// ----
// ---- file   : tksampler.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2009-2026 by bsp
// ----
// ----          Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// ----          associated documentation files (the "Software"), to deal in the Software without restriction, including
// ----          without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// ----          copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to
// ----          the following conditions:
// ----
// ----          The above copyright notice and this permission notice shall be included in all copies or substantial
// ----          portions of the Software.
// ----
// ----          THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// ----          NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// ----          IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// ----          WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// ----          SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ---- changed: 26Dec2009, 27Dec2009, 29Dec2009, 02Jan2010, 05Jan2010, 07Jan2010, 15Jan2010
// ----          19Jan2010, 21Jan2010, 21Feb2010, 01Mar2010, 29Apr2010, 02May2010, 07Jul2010
// ----          18Jul2010, 04Sep2010, 05Mar2011, 10Mar2011, 14Mar2011, 20Mar2011, 22Mar2011
// ----          10May2019, 29Nov2019, 15Feb2020, 16May2020, 19May2020, 03Feb2021, 04Oct2021
// ----          21Dec2022, 30Dec2022, 16Aug2023, 11Dec2024, 24May2026
// ----
// ----
// ----

#include "tksampler.h"

YAC_Host *yac_host;

#include "ying_tksampler_StADSR.cpp"
#include "ying_tksampler_StEnvelope.cpp"
#include "ying_tksampler_StLFO.cpp"
#include "ying_tksampler_StLFOPlayer.cpp"
#include "ying_tksampler_StModSeq.cpp"
#include "ying_tksampler_StRange.cpp"
#include "ying_tksampler_StWaveform.cpp"
#include "ying_tksampler_StSample.cpp"
#include "ying_tksampler_StSampleBank.cpp"
#include "ying_tksampler_StSampleVoice.cpp"
#include "ying_tksampler_StSampleMutexGroup.cpp"
#include "ying_tksampler_StSamplePlayer.cpp"
#include "ying_tksampler_StFFT.cpp"
#include "ying_tksampler_StFFT_BandParams.cpp"
#include "ying_tksampler_StPluginInfo.cpp"
#include "ying_tksampler_StPluginShared.cpp"
#include "ying_tksampler_StPluginSharedMissing.cpp"
#include "ying_tksampler_StPluginVoice.cpp"
#include "ying_tksampler_StPluginLibrary.cpp"

#include "ying_tksampler.cpp"


// Implement standard exception ID variables (see yac.h)
Dyac_std_exid_impl;


// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

   // Resolve "standard" exception IDs
   Dyac_std_exid_resolve;

   YAC_Init_tksampler(_host);

   StSampleVoice::InitLanczosTables();
#ifndef LIBSYNERGY_BUILD
   StSampleVoice::InitAdditiveTables();
#endif // LIBSYNERGY_BUILD
   StSampleVoice::InitMMCurveLUT();
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_tksampler(_host);
}


#include <yac_host.cpp>
