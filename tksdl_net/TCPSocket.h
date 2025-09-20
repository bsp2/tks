/// TCPSocket.h
///
/// (c) 2002-2008 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __TKSDL_TCPSOCKET_H__
#define __TKSDL_TCPSOCKET_H__

YG("network");


/* @class TCPSocket,Object
Represents an SDL_net TCP socket
*/
YC class _TCPSocket : public YAC_Object {
public:
	TCPsocket sdlnet_socket;
public:
	_TCPSocket(void);
	~_TCPSocket();

	YAC(_TCPSocket);

   /* @method accept:TCPSocket
   Accept client connection and return new TCPSocket object

   This is a wrapper for the SDLNet_TCP_Accept() native function.
   */
	YM void _accept         (YAC_Value *_r);


   /* @method close
   Close socket
   */
	YM void _close          (void);


   /* @method getPeerAddress:IPAddress
   Query peer IPV4 address and port number

   This is a wrapper for the SDLNet_TCP_GetPeerAddress() native function.
   */
	YM void _getPeerAddress (YAC_Value *_r);


   /* @method open,IPAddress ip
   Open socket 

   This is a wrapper for the SDLNet_TCP_Open() native function.
   */
	YM sSI  _open           (YAC_Object *_ipadr);


   /* @method recv,Buffer netbuf,int num
   Receive data
   */
	YM sSI  _recv           (YAC_Object *_netbuf, sSI _num);


   /* @method send,Buffer netbuf,int num
   Send data
   */
	YM sSI  _send           (YAC_Object *_netbuf, sSI _num);


   /* @method sendString,String s
   Send string
   */
   YM sSI  _sendString     (YAC_Object *_s);


   /* @method socketReady:boolean
   Check whether this socket has been marked as active. 

   This is a wrapper for the SDLNet_SocketReady() native function.
   */
	YM sSI  _socketReady    (void);
};

#endif

