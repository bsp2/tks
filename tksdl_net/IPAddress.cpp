/// IPAddress.cpp
///
/// (c) 2002-2009 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#define YAC_BIGSTRING
#include <yac.h>

#include "sdl_net_includes.h"

#include "IPAddress.h"

extern YAC_Host *yac_host;
extern sUI clid_IPAddress;
extern sUI clid_TCPSocket;
extern sUI clid_UDPSocket;
extern sUI clid_UDPPacket;


_IPAddress::_IPAddress(void) {
	sdlnet_ipadr.host=0;
	sdlnet_ipadr.port=0;
}

_IPAddress::~_IPAddress() {
}

void YAC_VCALL _IPAddress::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
	if(YAC_CHK(_o, class_ID))
	{
		if(_cmd==YAC_OP_ASSIGN)
		{
			_IPAddress *o=(_IPAddress*)_o;
			sdlnet_ipadr.host=o->sdlnet_ipadr.host;
			sdlnet_ipadr.port=o->sdlnet_ipadr.port;
		}
	}
}

void _IPAddress::_setHost(sSI _ipadr) {
#ifdef YAC_LITTLE_ENDIAN
	sdlnet_ipadr.host= ((_ipadr&0x000000FF)<<24) |
					   ((_ipadr&0x0000FF00)<< 8) |
					   ((_ipadr&0x00FF0000)>> 8) |
					   ((_ipadr&0xFF000000)>>24) ;
#else
	sdlnet_ipadr.host=(sU32)_ipadr;
#endif
}

sSI _IPAddress::_getHost(void) {
#ifdef YAC_LITTLE_ENDIAN
	return
			 ((sdlnet_ipadr.host&0x000000FF)<<24) |
			 ((sdlnet_ipadr.host&0x0000FF00)<< 8) |
			 ((sdlnet_ipadr.host&0x00FF0000)>> 8) |
			 ((sdlnet_ipadr.host&0xFF000000)>>24) ;
#else
	return sdlnet_ipadr.host;
#endif
}

sSI _IPAddress::_setHostByName(YAC_String *_name) {
	if(_name->chars&&_name->length)
	{
		if(SDLNet_ResolveHost(&sdlnet_ipadr, (char*)_name->chars, 0)==0)
			return 1;
		else
		{
			yac_host->printf("[---] IPAddress::setHostNyName: DNS lookup failed for \"%s\".\n", (char*)_name->chars);
		}
	}
	return 0;
}

void _IPAddress::_getHostName(YAC_Value *_r) {
	char *rs=(char*)SDLNet_ResolveIP(&sdlnet_ipadr);
	if(rs)
	{
		YAC_String *s = YAC_New_String();
      YAC_String t; t.visit(rs);
		s->yacCopy(&t); // Use yacCopy instead of copy to not mix different memory managers
		YAC_RETS(s, 1);
	}
	else
	{
		yac_host->printf("[---] IPAddress::getHostName: DNS lookup failed for %i.%i.%i.%i.\n", 
			(sdlnet_ipadr.host>>24)&0xFF,
			(sdlnet_ipadr.host>>16)&0xFF,
			(sdlnet_ipadr.host>> 8)&0xFF,
			(sdlnet_ipadr.host    )&0xFF
			);
		YAC_RETO(0, 0);
	}
}

void _IPAddress::_setPort(sSI _nr) {
#ifdef YAC_LITTLE_ENDIAN
	sdlnet_ipadr.port=(((sU16)_nr&0xFF00)>>8)|(((sU16)_nr&0x00FF)<<8);
#else
	sdlnet_ipadr.port=(sU16)_nr;
#endif
}

sSI _IPAddress::_getPort(void) {
#ifdef YAC_LITTLE_ENDIAN
	return (((sU16)sdlnet_ipadr.port&0xFF00)>>8)|(((sU16)sdlnet_ipadr.port&0x00FF)<<8);
#else
	return sdlnet_ipadr.port;
#endif
}

void _IPAddress::_init(sSI _adr, sSI _port) {
	_setHost(_adr);//sdlnet_ipadr.host=(sU32)_adr;
	_setPort(_port);//sdlnet_ipadr.port=(sU16)_port;
}

void _IPAddress::_getHostString(YAC_Value *_r) {
	YAC_String *r = YAC_New_String();
	if(r->yacArrayAlloc(17, 0,0,0))
	{
		r->printf("%i.%i.%i.%i:%i",
			(sdlnet_ipadr.host    )&0xFF,
			(sdlnet_ipadr.host>> 8)&0xFF,
			(sdlnet_ipadr.host>>16)&0xFF,
			(sdlnet_ipadr.host>>24)&0xFF,
			 ((sdlnet_ipadr.port&0xFF00)>>8)|
			 ((sdlnet_ipadr.port&0x00FF)<<8)
			);
	}
	YAC_RETS(r, 1);
}
