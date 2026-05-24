// ----
// ---- file   : tksampler.h
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
// ----          21Jul2019, 15Feb2020, 16May2020, 04Oct2021, 28Oct2021, 12Apr2023, 28Sep2024
// ----          11Dec2024, 24May2026
// ----
// ----
// ----

#ifndef TK_SAMPLER_H__
#define TK_SAMPLER_H__

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#ifndef YAC_PRINTF
//#define YAC_BIGSTRING
#define YAC_PRINTF
#define YAC_EPSILONCOMPARE_ABS defined
#define YAC_EPSILONCOMPARE_ABS_DEFAULT defined
#endif
#include <yac.h>

Dyac_std_exid_decl;

#include "rand.h"
#include "plugin.h"
#include "StPluginWrapper.h"
#include "../tks-source/TKS_Envelope.h"
#include "StEnvelope.h"
#include "StADSR.h"
#include "StADSRPlayer.h"
#include "StLFO.h"
#include "StLFOPlayer.h"
#include "StModSeq.h"
#include "StModSeqPlayer.h"
#include "StRange.h"
#include "StWaveform.h"
#include "StSampleMutexGroup.h"
#include "StSampleVoiceBiquad.h"
#include "StSample.h"
#include "StSampleBank.h"
#include "StSampleVoice.h"
#include "StSamplePlayer.h"
#include "StFFT.h"

#include "ying_tksampler.h"


#endif // TK_SAMPLER_H__
