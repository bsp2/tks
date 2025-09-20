/// Network.cpp
///
/// (c) 2002-2009 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <yac.h>

#include "sdl_net_includes.h"

#include "Network.h"
#include "IPAddress.h"
#include "TCPSocket.h"

extern YAC_Host *yac_host;
extern sUI clid_IPAddress;
extern sUI clid_TCPSocket;
extern sUI clid_UDPSocket;
extern sUI clid_UDPPacket;


_Network::_Network(void) {
	sdlnet_socketset = 0;
}

_Network::~_Network(void) {
	_freeSocketSet();
}

void _Network::_getError(YAC_Value *_r) {
	char *e = SDLNet_GetError();
	YAC_String *s = YAC_New_String();
	if(e != NULL)
   {
      YAC_String t; t.visit(e);
		s->yacCopy(&t);
   }
	YAC_RETS(s, 1);
}

void _Network::_resolveHost(YAC_Object *_name, sSI _port, YAC_Value *_r) {
	if(YAC_BCHK(_name, YAC_CLID_STRING))
	{
		YAC_String *name=(YAC_String*)_name;
		if(name->chars&&name->length)
		{
			_IPAddress *nip=(_IPAddress*)yac_host->yacNewByID(clid_IPAddress);//new _IPAddress();
			if(SDLNet_ResolveHost(&nip->sdlnet_ipadr, (char*)name->chars, 0)==0)
			{
				YAC_RETO(nip, 1); // OK
				return;
			}
			else
			{
				yac_host->printf("[---] Network::resolveHost: DNS lookup failed for \"%s\".\n", (char*)name->chars);
			}
		}
	}
	YAC_RETO(0, 0);
}

void _Network::_resolveIP(YAC_Object *_ipadr, YAC_Value *_r) {
	if(YAC_CHK(_ipadr, clid_IPAddress))
	{
		_IPAddress *ip=(_IPAddress*)_ipadr;
		char *r=(char*)SDLNet_ResolveIP(&ip->sdlnet_ipadr);
		if(r != NULL)
		{
			YAC_String *s = YAC_New_String();
         YAC_String t; t.visit(r);
			s->yacCopy(&t);
			YAC_RETS(s, 1);
			return;
		}
		else
		{
			yac_host->printf("[---] Network::resolveIP: DNS lookup failed for %i.%i.%i.%i.\n", 
				(ip->sdlnet_ipadr.host    )&0xFF,
				(ip->sdlnet_ipadr.host>> 8)&0xFF,
				(ip->sdlnet_ipadr.host>>16)&0xFF,
				(ip->sdlnet_ipadr.host>>24)&0xFF);
			
		}
	}
	YAC_RETO(0, 0);
}

sSI _Network::_allocSocketSet(sSI _num) {
	if(_num>0)
	{
		if(sdlnet_socketset)
		{
			_freeSocketSet();
		}
		sdlnet_socketset = SDLNet_AllocSocketSet(_num);

		if(sdlnet_socketset)
      {
			return 1;
      }
		else
		{
			yac_host->printf("[---] Network::allocSocketSet: failed to alloc socket set for %i sockets.\n",
				_num);
		}
	}
	return 0;
}

void _Network::_freeSocketSet(void) {
	if(sdlnet_socketset)
	{
		SDLNet_FreeSocketSet(sdlnet_socketset);
		sdlnet_socketset=0;
	}
}

sSI _Network::_addTCPSocket(YAC_Object *_sock) {
	return _addSocket(_sock);
}

sSI _Network::_addUDPSocket(YAC_Object *_sock) {
	return _addSocket(_sock);
}

sSI _Network::_addSocket(YAC_Object *_sock) {
   //
   // XXX TODO: finish UDP support
   //
	if(sdlnet_socketset)
	{
		if(YAC_CHK(_sock, clid_TCPSocket))
		{
			_TCPSocket *sock=(_TCPSocket *)_sock;
			int r=SDLNet_TCP_AddSocket(sdlnet_socketset, sock->sdlnet_socket);
			if(r==-1)
			{
				yac_host->printf("[---] Network::addSocket: failed to add TCP socket.\n");
			}
			else
         {
				return 1;
         }
		}
		else
		{
			if(YAC_CHK(_sock, clid_UDPSocket))
			{
				//_UDPSocket *sock=(_UDPSocket*)_sock;
				// xxx UDP not supported, yet.........
			}
		}
	}
	return 0;
}

sSI _Network::_removeSocket(YAC_Object *_sock) {
   //
   // XXX TODO: finish UDP support
   //
	if(sdlnet_socketset)
	{
		if(YAC_CHK(_sock, clid_TCPSocket))
		{
			_TCPSocket *sock=(_TCPSocket*)_sock;
			int r=SDLNet_TCP_DelSocket(sdlnet_socketset, sock->sdlnet_socket);
			if(r==-1)
			{
				yac_host->printf("[---] Network::removeSocket: failed to remove TCP socket.\n");
			}
			else
         {
				return 1;
         }
		}
		else
		{
			if(YAC_CHK(_sock, clid_UDPSocket))
			{
				//_UDPSocket*sock=(_UDPSocket*)_sock;
				// xxx
			}
		}
	}
	return 0;
}

sSI _Network::_checkSockets(sSI _to) {
	if(sdlnet_socketset)
   {
		return SDLNet_CheckSockets(sdlnet_socketset, (sU32)_to);
   }
	return 0;
}
