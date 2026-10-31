
#include <windows.h>

#define YAC_BIGSTRING defined
#include <yac.h>
#include <yac_host.cpp>
YAC_Host *yac_host;

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

	if(Dyac_host_yacGetDebugLevel() > 1)
	  Dyac_host_log2("tkmidi<winmm>::YAC_Init called.");

   mtx_miditimer = yac_host->yacMutexCreate();

   YAC_Init_midi(_host);
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

   YAC_Exit_midi(_host);

   yac_host->yacMutexDestroy(mtx_miditimer);
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00010002;
}
