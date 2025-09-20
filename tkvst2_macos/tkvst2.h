/// tkvst2.h
///
/// (c) 2010-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 01Oct2010
/// changed: 02Oct2010, 13Oct2010, 06Jul2018, 26Aug2019, 14Feb2023, 25Feb2023
///
///
///

#ifndef __TKVST2_H__
#define __TKVST2_H__

#define VST_FORCE_DEPRECATED 0

#include <pluginterfaces/vst2.x/aeffect.h>
#include <pluginterfaces/vst2.x/aeffectx.h>

#include <pthread.h> 

#define YAC_BIGSTRING defined
#include <yac.h>

#ifdef YAC_MACOS
#include "CoreFoundation/CoreFoundation.h"
#endif // YAC_MACOS

#include "ying_vst2.h"


YG("vst2");


extern sF32  tkvst2_bpm;
extern sF32  tkvst2_song_pos_ppq;
extern sBool tkvst2_song_playing;


YF void YAC_CALL tkvst2_set_bpm (sF32 _bpm);
YF void YAC_CALL tkvst2_set_song_pos_ppq (sF32 _songPosPPQ, sBool _bPlaying);

YF void YAC_CALL tkvst2_window_test (void);


#endif // __TKVST2_H__
