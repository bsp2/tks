/// UDPSocket.cpp
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
#include "UDPSocket.h"

extern YAC_Host *yac_host;
extern sUI clid_IPAddress;
extern sUI clid_TCPSocket;
extern sUI clid_UDPSocket;
extern sUI clid_UDPPacket;


_UDPSocket::_UDPSocket(void) {
	sdlnet_socket=0;
}

_UDPSocket::~_UDPSocket() {
	_close();
}

sSI _UDPSocket::_open(sSI _port) {
	sU16 port=(sU16)_port;
	sdlnet_socket=SDLNet_UDP_Open(port);
	if(sdlnet_socket)
	{
		if(yac_host->yacGetDebugLevel()>0)
			yac_host->printf("[---] UDPSocket::open: opened socket for port %i.\n", port);
		return 1;
	}
	else
		yac_host->printf("[---] UDPSocket::open: failed to open socket for port %i.\n", port);
	return 0;
}

void _UDPSocket::_close(void) {
	if(sdlnet_socket)
	{
		SDLNet_UDP_Close(sdlnet_socket);
		sdlnet_socket=0;
	}
}

void _UDPSocket::_getPeerAddress(sSI _channel, YAC_Value *_r) {
	if(sdlnet_socket)
	{
		IPaddress *r=SDLNet_UDP_GetPeerAddress(sdlnet_socket, _channel);
		if(r)
		{
			_IPAddress *ip=(_IPAddress*)yac_host->yacNewByID(clid_IPAddress);
			ip->sdlnet_ipadr.host=r->host;
			ip->sdlnet_ipadr.port=r->port;
			YAC_RETO(ip, 1);
			return;
		}
		else
		{
			yac_host->printf("[---] UDPSocket::getPeerAddress: failed (channel=%i).\n", _channel);
		}
	}
	YAC_RETO(0, 0);
}

sSI _UDPSocket::_bind(sSI _channel, YAC_Object *_ip) {
	if(sdlnet_socket)
	{
		if(YAC_CHK(_ip, clid_IPAddress))
		{
			_IPAddress *ip=(_IPAddress*)_ip;
			int r=SDLNet_UDP_Bind(sdlnet_socket, _channel, &ip->sdlnet_ipadr);
			if(r==-1)
			{
				yac_host->printf("[---] UDPSocket::bind: failed (channel=%i, ip host=%i.%i.%i.%i port=%i).\n",
					_channel, 
					(ip->sdlnet_ipadr.host    )&0xFF,
					(ip->sdlnet_ipadr.host>> 8)&0xFF,
					(ip->sdlnet_ipadr.host>>16)&0xFF,
					(ip->sdlnet_ipadr.host>>24)&0xFF,
					ip->sdlnet_ipadr.port);
			}
			else
			{
				if(yac_host->yacGetDebugLevel()>2)
				{
					yac_host->printf("[---] UDPSocket::bind: OK (channel=%i, ip host=%i.%i.%i.%i port=%i).\n",
						_channel, 
						(ip->sdlnet_ipadr.host    )&0xFF,
						(ip->sdlnet_ipadr.host>> 8)&0xFF,
						(ip->sdlnet_ipadr.host>>16)&0xFF,
						(ip->sdlnet_ipadr.host>>24)&0xFF,
						ip->sdlnet_ipadr.port);
				}
				return 1;
			}
		}
	}
	return 0;
}

void _UDPSocket::_unbind(sSI _channel) {
	if(sdlnet_socket)
		SDLNet_UDP_Unbind(sdlnet_socket, _channel);
}


sSI _UDPSocket::_send(sSI _channel, YAC_Object *_pak) {
	if(sdlnet_socket)
	{
		if(YAC_CHK(_pak, clid_UDPPacket))
		{
			_UDPPacket *pak=(_UDPPacket*)_pak;
			if(pak->netbuf)
			{
				// update package data if netbuffer has changed (reallocation..)
				pak->sdlnet_packet.data=(sU8*)pak->netbuf->buffer;
				pak->sdlnet_packet.maxlen=pak->netbuf->size;
				if(pak->sdlnet_packet.len>(int)pak->netbuf->size)
					pak->sdlnet_packet.len=pak->netbuf->size;

				int r=SDLNet_UDP_Send(sdlnet_socket, _channel, &pak->sdlnet_packet);

				if(r==0)
				{
					yac_host->printf("[---] UDPSocket::send: failed to send %i bytes to %i.%i.%i.%i:%i channel=%i.\n",
						pak->sdlnet_packet.len,
						(pak->sdlnet_packet.address.host    )&0xFF,
						(pak->sdlnet_packet.address.host>> 8)&0xFF,
						(pak->sdlnet_packet.address.host>>16)&0xFF,
						(pak->sdlnet_packet.address.host>>24)&0xFF,
						pak->sdlnet_packet.address.port,
						_channel);
				}
				else
				{
					if(yac_host->yacGetDebugLevel()>2)
					{
						yac_host->printf("[---] UDPSocket::send: OK, sent %i bytes to %i.%i.%i.%i:%i channel=%i numdest=%i.\n",
							pak->sdlnet_packet.len,
							(pak->sdlnet_packet.address.host    )&0xFF,
							(pak->sdlnet_packet.address.host>> 8)&0xFF,
							(pak->sdlnet_packet.address.host>>16)&0xFF,
							(pak->sdlnet_packet.address.host>>24)&0xFF,
							pak->sdlnet_packet.address.port,
							_channel, r);
					}
					return 1;
				}
			}
		}
	}
	return 0;
}

sSI _UDPSocket::_recv(YAC_Object *_pak) {
	if(sdlnet_socket)
	{
		if(YAC_CHK(_pak, clid_UDPPacket))
		{
			_UDPPacket*pak=(_UDPPacket*)_pak;

			if(pak->netbuf)
			{
				// ---- update package data if netbuffer has changed (reallocation..)
				pak->sdlnet_packet.data=(sU8*)pak->netbuf->buffer;
				pak->sdlnet_packet.maxlen=pak->netbuf->size;
				if(pak->sdlnet_packet.len>(int)pak->netbuf->size)
				{
					/// xxx warn
					pak->sdlnet_packet.len=pak->netbuf->size;
				} 
				
				int r=SDLNet_UDP_Recv(sdlnet_socket, &pak->sdlnet_packet);
				if(r==0)
				{
					yac_host->printf("[---] UDPSocket::recv: failed to receive packet.\n");
				}
				else
				{
					if(yac_host->yacGetDebugLevel()>2)
					{
						yac_host->printf("[---] UDPSocket::recv: received %i bytes from %i.%i.%i.%i:%i channel=%i.\n",
							pak->sdlnet_packet.len,
							(pak->sdlnet_packet.address.host    )&0xFF,
							(pak->sdlnet_packet.address.host>> 8)&0xFF,
							(pak->sdlnet_packet.address.host>>16)&0xFF,
							(pak->sdlnet_packet.address.host>>24)&0xFF,
							pak->sdlnet_packet.address.port,
							pak->sdlnet_packet.channel);
					}	
					return 1;
				}
			}
		}
	}
	return 0;
}

sSI _UDPSocket::_socketReady(void) {
	if(sdlnet_socket)
		return SDLNet_SocketReady(sdlnet_socket);
	return 0;
}
