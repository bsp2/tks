// ----
// ---- file   : tkmidipipe.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 07Sep2014, 06Apr2020, 26Jun2023, 01Jul2024
// ----
// ----
// ----

#define Dprintf if(1);else yac_host->printf
/* #define Dprintf if(0);else yac_host->printf */

#define YAC_BIGSTRING defined
#include <yac.h>

#include "ying_midipipe.h"

#include "MIDIPipeFrameValueArray.h"
#include "MIDIPipeFrame.h"
#include "MIDIPipeEvent.h"
#include "MIDIPipeDevice.h"
#include "MIDIPipeNode.h"
#include "MIDIPipe.h"
#include "MIDIPipeRoot.h"
#include "MIDIPipeNodeScriptProxy.h"
#include "ModInputFilter.h"
#include "HostMIDIEvents.h"
