// ----
// ---- file   : tksampler.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2023 by Bastian Spiegel. 
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See 
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ----          
// ---- changed: 26Dec2009, 27Dec2009, 29Dec2009, 02Jan2010, 05Jan2010, 07Jan2010, 15Jan2010
// ----          19Jan2010, 21Jan2010, 21Feb2010, 01Mar2010, 29Apr2010, 02May2010, 07Jul2010
// ----          18Jul2010, 04Sep2010, 05Mar2011, 10Mar2011, 14Mar2011, 20Mar2011, 22Mar2011
// ----          10May2019, 29Nov2019, 15Feb2020, 16May2020, 19May2020, 03Feb2021, 04Oct2021
// ----          21Dec2022, 30Dec2022, 16Aug2023, 11Dec2024
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
   StSampleVoice::InitAdditiveTables();
   StSampleVoice::InitMMCurveLUT();
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_tksampler(_host);
}


#include <yac_host.cpp>
