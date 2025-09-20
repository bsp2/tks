/// tkmidi.cpp
///
/// (c) 2006-2014 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 08Sep2006, 04Feb2014, 05Feb2014
///

#include <pthread.h>
#include <sys/time.h>

#define YAC_BIGSTRING defined
#include <yac.h>
#include <yac_host.cpp>
YAC_Host *yac_host;

#include "tkmidi.h"
#include "MIDIIn.h"
#include "MIDIOut.h"
#include "MIDITimer.h"

#include "ying_midi.h"
#include "ying_midi.cpp"
#include "ying_midi_RecordedMIDIEvent.cpp"
#include "ying_midi_MIDIIn.cpp"
#include "ying_midi_MIDIOut.cpp"
#include "ying_midi_MIDITimer.cpp"

YAC_MutexHandle mtx_miditimer;


void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

	if(yac_host->yacGetDebugLevel() > 0)
   {
      yac_host->printf("tkmidi::YAC_Init ENTER.\n");
   }

   mtx_miditimer = yac_host->yacMutexCreate();

   YAC_Init_midi(_host);

   MIDIOut::AllocAlsaDeviceInfos();
   MIDIIn::AllocAlsaDeviceInfos();

	if(yac_host->yacGetDebugLevel() > 0)
   {
      yac_host->printf("tkmidi::YAC_Init LEAVE.\n");
   }
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

	if(yac_host->yacGetDebugLevel() > 0)
   {
      yac_host->printf("tkmidi::YAC_Exit ENTER.\n");
   }

   MIDIIn::FreeAlsaDeviceInfos();
   MIDIOut::FreeAlsaDeviceInfos();

   YAC_Exit_midi(_host);

   yac_host->yacMutexDestroy(mtx_miditimer);

	if(yac_host->yacGetDebugLevel() > 0)
   {
      yac_host->printf("tkmidi::YAC_Exit LEAVE.\n");
   }
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00020002;
}
