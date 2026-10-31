/// tksdl_net.cpp
///
/// (c) 2001-2026 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///


#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YAC_PRINTF defined
#define YAC_BIGSTRING defined
#include <yac.h>

#include "sdl_net_includes.h"

#undef INADDR_ANY
#undef INADDR_NONE
#include <yac_host.cpp>
#include "IPAddress.h"
#include "TCPSocket.h"
#include "UDPPacket.h"
#include "UDPSocket.h"
#include "Network.h"

extern sUI clid_IPAddress;

YAC_Host *yac_host;

#include "ying_network_IPAddress.cpp"
#include "ying_network_Network.cpp"
#include "ying_network_TCPSocket.cpp"
#include "ying_network_UDPPacket.cpp"
#include "ying_network_UDPSocket.cpp"
#include "ying_network.cpp"


// ---------------------------------------------------------------------------
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

	if(Dyac_host_yacGetDebugLevel() > 1)
      Dyac_host_log2("tksdl_net::YAC_Init called.");

#ifdef YAC_GLOBAL_NEWDELETE
   _host->printf("[dbg] tksdl_net: yac_global_newdelete_counter=%d\n", yac_host->yacNewDeleteGetCounter());
#endif // YAC_GLOBAL_NEWDELETE

	::SDLNet_Init();

	YAC_Init_network(_host);

	if(Dyac_host_yacGetDebugLevel() > 1)
		Dyac_host_log2("tksdl_net::YAC_Init() finished.");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   sBool bDebug = (Dyac_host_yacGetDebugLevel() > 1);
	if(bDebug)
		Dyac_host_log2("tksdl_net::YAC_Exit() called.");

   YAC_Exit_network      (_host);

	::SDLNet_Quit();

	if(bDebug)
		Dyac_host_log2("tksdl_net::YAC_Exit() finished.");

}

sUI YAC_CALL YAC_Version(void) {
	return 0x00090226;
}
