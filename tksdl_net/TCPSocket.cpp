/// TCPSocket.cpp
///
/// (c) 2002-2009 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <yac.h>

#include "sdl_net_includes.h"

#include "IPAddress.h"
#include "TCPSocket.h"

extern YAC_Host *yac_host;
extern sUI clid_IPAddress;
extern sUI clid_TCPSocket;
extern sUI clid_UDPSocket;
extern sUI clid_UDPPacket;


_TCPSocket::_TCPSocket(void) {
	sdlnet_socket=0;
}

_TCPSocket::~_TCPSocket() {
	_close();
}

sSI _TCPSocket::_open(YAC_Object *_ip) {
	if(YAC_CHK(_ip, clid_IPAddress))
	{
		_IPAddress *ip=(_IPAddress*)_ip;
		sdlnet_socket=SDLNet_TCP_Open(&ip->sdlnet_ipadr);
		if(sdlnet_socket)
		{
			if(yac_host->yacGetDebugLevel()>0)
			{
				if(ip->sdlnet_ipadr.host==INADDR_ANY)
				{
					yac_host->printf("[...] TCPSocket::open: opened server socket for port %i.\n",
						ip->sdlnet_ipadr.port);
				}
				else
				{
					yac_host->printf("[...] TCPSocket::open: opened socket for address %i.%i.%i.%i.\n",
						(ip->sdlnet_ipadr.host    )&0xFF,
						(ip->sdlnet_ipadr.host>> 8)&0xFF,
						(ip->sdlnet_ipadr.host>>16)&0xFF,
						(ip->sdlnet_ipadr.host>>24)&0xFF);
				}
			}
			return 1;
		}
		else
		{
			if(ip->sdlnet_ipadr.host==INADDR_ANY)
			{
				yac_host->printf("[---] TCPSocket::open: failed to open server socket for port %i.\n",
					ip->sdlnet_ipadr.port);
			}
			else
			{
				yac_host->printf("[---] TCPSocket::open: failed to open socket for address %i.%i.%i.%i.\n",
					(ip->sdlnet_ipadr.host    )&0xFF,
					(ip->sdlnet_ipadr.host>> 8)&0xFF,
					(ip->sdlnet_ipadr.host>>16)&0xFF,
					(ip->sdlnet_ipadr.host>>24)&0xFF);
			}
		}
	}
	return 0;
}

void _TCPSocket::_close(void) {
	if(sdlnet_socket)
	{
		if(yac_host->yacGetDebugLevel()>2)
		{
			yac_host->printf("[...] TCPSocket::close.\n");
		}
		SDLNet_TCP_Close(sdlnet_socket);
		sdlnet_socket=0;
	}
}

void _TCPSocket::_getPeerAddress(YAC_Value *_r) {
	if(sdlnet_socket)
	{
		IPaddress *r=SDLNet_TCP_GetPeerAddress(sdlnet_socket);
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
			yac_host->printf("[---] TCPSocket::getPeerAddress: failed.\n");
		}
	}
	YAC_RETO(0, 0);
}

void _TCPSocket::_accept(YAC_Value *_r) {
	if(sdlnet_socket)
	{
		/// XXX check if server socket
		TCPsocket r;
		r=SDLNet_TCP_Accept(sdlnet_socket);
		if(r)
		{
			_TCPSocket *ns=(_TCPSocket*)yac_host->yacNewByID(clid_TCPSocket);
			ns->sdlnet_socket=r;
			if(yac_host->yacGetDebugLevel()>0)
			{
				yac_host->printf("[+++] TCPSocket::accept: OK.\n");
			}
			YAC_RETO(ns, 1);
			return;
		}
		else
		{
			yac_host->printf("[---] TCPSocket::accept: failed. error=%s\n",
				SDLNet_GetError());
		}
	}
	YAC_RETO(0, 0);
}

sSI _TCPSocket::_send(YAC_Object *_buf, sSI _len) {
	if(sdlnet_socket)
	{
		if(YAC_BCHK(_buf, YAC_CLID_BUFFER))
		{
			YAC_Buffer *buf=(YAC_Buffer*)_buf;
			if(buf->buffer&&buf->size)
			{
				sU32 len=(sU32)_len;
				if(!len)
            {
               len = buf->size;
            }
				if(len <= (buf->size-buf->io_offset))
				{
					int r=SDLNet_TCP_Send(sdlnet_socket, (void*)(buf->buffer+buf->io_offset), (int)len);
					if(r!=(int)len)
					{
						yac_host->printf("[---] TCPSocket::send: failed to send %i bytes.\n",
							len);
					}
					else
					{
						if(yac_host->yacGetDebugLevel()>2)
						{
							yac_host->printf("[+++] TCPSocket::send: sent %i bytes.\n",
								len);
						}
						return 1;
					}
				}
			}
		}
		else
		{
			yac_host->printf("[---] TCPSocket:send: argument is not a Buffer.\n");
		}
	}
	else
	{
		yac_host->printf("[---] TCPSocket::send: no sdlnet_socket.\n");
	}
	return 0;
}

sSI _TCPSocket::_sendString(YAC_Object *_s) {
	if(sdlnet_socket)
	{
      if(YAC_Is_String(_s))
      {
         YAC_String *s=(YAC_String *)_s;
         if(s->length>1)
         {
            int len = (int)s->length - 1;
            int r=SDLNet_TCP_Send(sdlnet_socket, (void*)(s->chars), len);
            if(r!=(int)len)
            {
               yac_host->printf("[---] TCPSocket::sendString: failed to send %i chars.\n",
                  len);
            }
            else
            {
               if(yac_host->yacGetDebugLevel()>2)
               {
                  yac_host->printf("[+++] TCPSocket::sendString: sent %i chars.\n",
                     len);
               }
               return 1;
            }
         }
      }
		else
		{
			yac_host->printf("[---] TCPSocket:sendString: argument is not a String.\n");
		}
	}
	else
	{
		yac_host->printf("[---] TCPSocket::sendString: no sdlnet_socket.\n");
	}
	return 0;
}

sSI _TCPSocket::_recv(YAC_Object *_buf, sSI _len) {
	if(sdlnet_socket)
	{
		if(YAC_BCHK(_buf, YAC_CLID_BUFFER))
		{
			YAC_Buffer *buf=(YAC_Buffer*)_buf;

			if(yac_host->yacGetDebugLevel()>80)
         {
				yac_host->printf("TCPSocket::recv: len=%i buf.size=%i buf.io_offset=%i\n",
                             _len, buf->size, buf->io_offset);
         }

			sU32 len=(sU32)_len;
			if(!len)
         {
            len=buf->size;
         }
			if(len<=(buf->size-buf->io_offset))
			{
_recv_tcpread:
				if(buf->buffer&&buf->size)
				{
					int r=SDLNet_TCP_Recv(sdlnet_socket, (void*)(buf->buffer+buf->io_offset), len);
					{
						if(yac_host->yacGetDebugLevel()>2)
						{
							yac_host->printf("[+++] TCPSocket::recv: received %i bytes.\n",
								len);
						}
						buf->io_offset+=r;
						return r;
					}
				}
			}
			else
			{
				sUI rlen=(buf->io_offset+len);
				if(buf->yacArrayRealloc(rlen, 0,0,0))
            {
					goto _recv_tcpread;
            }
			}
		}
	}
	return 0;
}

sSI _TCPSocket::_socketReady(void) {
  if(sdlnet_socket)
  {
    return SDLNet_SocketReady(sdlnet_socket);
  }
  return 0;
}
