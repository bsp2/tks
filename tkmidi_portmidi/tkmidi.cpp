
#define YAC_BIGSTRING defined
#include <yac.h>
#include <yac_host.cpp>
YAC_Host *yac_host;

#include "inc_portmidi.h"

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

extern void MIDITimer__pt_start (void);
extern void MIDITimer__pt_stop (void);

void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

	if(yac_host->yacGetDebugLevel())
	  yac_host->printf("tkmidi<portmidi>::YAC_Init called.\n");

   mtx_miditimer = yac_host->yacMutexCreate();

   YAC_Init_midi(_host);

   // (note) PortMidi timer must be started before opening a MIDI device
   MIDITimer__pt_start();
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

   MIDITimer__pt_stop();

   YAC_Exit_midi(_host);

   yac_host->yacMutexDestroy(mtx_miditimer);
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00010006;
}
