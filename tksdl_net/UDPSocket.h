/// UDPSocket.h
///
/// (c) 2002-2008 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __TKSDL_UDPSOCKET_H__
#define __TKSDL_UDPSOCKET_H__

YG("network");


/* @class UDPSocket,Object
Represents a user datagram protocol socket

@see UDPPacket
@see Network
*/
YC class _UDPSocket : public YAC_Object {
public:
	UDPsocket sdlnet_socket;
public:
	_UDPSocket(void);
	~_UDPSocket();

	YAC(_UDPSocket);

   /* @method open,int port:boolean
   */
	YM sSI  _open           (sSI _port);


   /* @method close
   */
	YM void _close          (void);


   /* @method getPeerAddress,int ch:IPAddress
   */
	YM void _getPeerAddress (sSI _channel, YAC_Value *_r);


   /* @method bind,int ch,IPAddress ip:boolean
   */
	YM sSI  _bind           (sSI _channel, YAC_Object *_ip);


   /* @method send,int ch,UDPPacket packet:boolean
   */
	YM sSI  _send           (sSI _channel, YAC_Object *_udppacket);


   /* @method recv,UDPPacket packetRet:boolean
   */
	YM sSI  _recv           (YAC_Object *_udppacket);


   /* @method socketReady:boolean
   */
	YM sSI  _socketReady    (void);


   /* @method unbind,int ch
   */
	YM void _unbind         (sSI _channel);
};


#endif // __TKSDL_UDPSOCKET_H__


