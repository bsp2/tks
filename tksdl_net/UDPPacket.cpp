/// UDPPacket.cpp
///
/// (c) 2002-2009 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <yac.h>

#include "sdl_net_includes.h"

#include "IPAddress.h"
#include "UDPPacket.h"

extern YAC_Host *yac_host;
extern sUI clid_IPAddress;
extern sUI clid_TCPSocket;
extern sUI clid_UDPSocket;
extern sUI clid_UDPPacket;


_UDPPacket::_UDPPacket(void) {
	netbuf=0;
	sdlnet_packet.channel=0;
	sdlnet_packet.data=0;
	sdlnet_packet.len=0;
	sdlnet_packet.maxlen=0;
	sdlnet_packet.status=0;
	sdlnet_packet.address.host=0;
	sdlnet_packet.address.port=0;
}

_UDPPacket::~_UDPPacket() {
}

void _UDPPacket::_setChannel(sSI _nr) {
	sdlnet_packet.channel=_nr;
}

sSI _UDPPacket::_getChannel(void) {
	return sdlnet_packet.channel;
}

void _UDPPacket::_setData(YAC_Object *_nbuf) {
	if(YAC_BCHK(_nbuf, YAC_CLID_BUFFER))
	{
		netbuf=(YAC_Buffer*)_nbuf;
		sdlnet_packet.data=(sU8*)netbuf->buffer;
		sdlnet_packet.len=0;
		sdlnet_packet.maxlen=netbuf->size;
	}
	else
	{
		netbuf=0;
		sdlnet_packet.data=0;
		sdlnet_packet.len=0;
		sdlnet_packet.maxlen=0;
	}
}

YAC_Object *_UDPPacket::_getData(void) {
	return netbuf;
}

sSI _UDPPacket::_getLength(void) {
	return sdlnet_packet.len;
}

void _UDPPacket::_setLength(sSI _l) {
	if(_l<sdlnet_packet.maxlen)
		sdlnet_packet.len=_l;
	else
	{
		if(yac_host->yacGetDebugLevel()>2)
		{
			yac_host->printf("[---] _UDPPacket::setLength: %i exceeds max buffer size (=%i).\n",
				_l, sdlnet_packet.maxlen);
		}
	}
}

sSI _UDPPacket::_getStatus(void) {
	return sdlnet_packet.status;
}

void _UDPPacket::_setAddress(_IPAddress *_ip) {
	sdlnet_packet.address.host=_ip->sdlnet_ipadr.host;
	sdlnet_packet.address.port=_ip->sdlnet_ipadr.port;
}

void _UDPPacket::_getAddress(YAC_Value *_r) {
	_IPAddress *r=(_IPAddress*)yac_host->yacNewByID(clid_IPAddress);//new _IPAddress();
	r->sdlnet_ipadr.host=sdlnet_packet.address.host;
	r->sdlnet_ipadr.port=sdlnet_packet.address.port;
	YAC_RETO(r, 1);
}
